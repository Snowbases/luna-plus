//=================================================================================================
//	FILE		: CChatRoomJoinDlg.cpp
//	DESC		: Implementation part of CChatRoomJoinDlg class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 25, 2008
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Define caption string.
//-------------------------------------------------------------------------------------------------
//	ERR-IMP		: Invalid a message parameter!!
//	ERR-IVDCS	: Invalid controls!!
//	ERR-FRCD	: Failed to receive create dialog!!
//	ERR-FRMD	: Failed to receive a main dialog!!
//	ERR-IVIM	: Invalid input mode!!
//
//CRJD-LK	: CChatRoomJoinDlg::Linking
//CRJD-SA	: CChatRoomJoinDlg::SetActive
//CRJD-OAE	: CChatRoomJoinDlg::OnActionEvent
//CRJD-SJI	: CChatRoomJoinDlg::SetJoinInfo
//CRJD-SIM	: CChatRoomJoinDlg::SetInputMode




//-------------------------------------------------------------------------------------------------
//		Include header files.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "../WindowIDEnum.h"
#include "../cIMEex.h"

#include "./Input/Keyboard.h"

#include "./Interface/cWindowManager.h"

#include "./Interface/cButton.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cStatic.h"

#include "./ChatRoomMgr.h"

#include "./ChatRoomJoinDlg.h"
#include "./ChatRoomMainDlg.h"

#include "./GameIn.h"





//-------------------------------------------------------------------------------------------------
//	NAME : CChatRoomJoinDlg
//	DESC : 생성자 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomJoinDlg::CChatRoomJoinDlg(void)
{
	// 초기화 처리.
	m_byInputMode		= e_JoinDlg_Type_RoonIndex ;							// Initialize member variables.

	m_pStc_RoomIndex	= NULL ;
	m_pStc_SecretCode	= NULL ;

	m_pStc_IndexDesc	= NULL ;
	m_pStc_CodeDesc		= NULL ;

	m_pEdb_Code			= NULL ;

	m_pBtn_OK			= NULL ;
	m_pBtn_Cancel		= NULL ;

	//m_byRoomIdx = 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomJoinDlg
//	DESC : 소멸자 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomJoinDlg::~CChatRoomJoinDlg(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME : Linking
//	DESC : 하위 컨트롤들을 링크하는 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomJoinDlg::Linking()
{
	// 하위 컨트롤 링크.
	m_pStc_RoomIndex	= (cStatic*)GetWindowForID(CRJD_STC_ROOMINDEX) ;		// Linking child controls.
	m_pStc_SecretCode	= (cStatic*)GetWindowForID(CRJD_STC_CODE) ;

	m_pStc_IndexDesc	= (cStatic*)GetWindowForID(CRJD_STC_INDEX_DESC) ;
	m_pStc_CodeDesc		= (cStatic*)GetWindowForID(CRJD_STC_CODE_DESC) ;

	m_pEdb_Code			= (cEditBox*)GetWindowForID(CRJD_EDB_CODE) ;

	m_pBtn_OK			= (cButton*)GetWindowForID(CRJD_BTN_OK) ;
	m_pBtn_Cancel		= (cButton*)GetWindowForID(CRJD_BTN_CANCEL) ;



	// 하위 컨트롤 체크.
	ASSERT(m_pStc_RoomIndex) ;													// Check controls.
	ASSERT(m_pStc_SecretCode) ;

	ASSERT(m_pStc_IndexDesc) ;
	ASSERT(m_pStc_CodeDesc) ;

	ASSERT(m_pEdb_Code) ;

	ASSERT(m_pBtn_OK) ;
	ASSERT(m_pBtn_Cancel) ;

	if( !m_pStc_RoomIndex || !m_pStc_SecretCode || !m_pEdb_Code || 
		!m_pBtn_OK || !m_pBtn_Cancel || !m_pStc_IndexDesc || !m_pStc_CodeDesc)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}


	//m_pEdb_Code->SetValidCheck(VCM_NORMAL_NUMBER) ;									// Setting input mode.

	//SetInputMode(m_byInputMode) ;



	// 채팅방 매니저에 등록.
	CHATROOMMGR->SetJoinDlg(this) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ActionKeyboardEvent
//	DESC : 키보드 이벤트를 처리하는 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
DWORD CChatRoomJoinDlg::ActionKeyboardEvent( CKeyboard * keyInfo )
{
	DWORD we = WE_NULL ;

	if( !m_bActive ) return we ;

	we |= cDialog::ActionKeyboardEvent(keyInfo) ;



	// 에디트 박스 체크.
	ASSERT(m_pEdb_Code) ;

	if(!m_pEdb_Code)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return we ;
	}



	// 포커스 있으면,
	if(m_pEdb_Code->IsFocus())
	{
		if( keyInfo->GetKeyPressed(KEY_RETURN) || keyInfo->GetKeyPressed(KEY_PADENTER) )
		{
			// 메인 다이얼로그 받기.
			CChatRoomMainDlg* pDlg = NULL ;
			pDlg = GAMEIN->GetChatRoomMainDlg() ;						// Receive main dialog.

			ASSERT(pDlg) ;

			if(!pDlg)
			{
				CHATROOMMGR->Throw_Error("Failed to receive main dialog.", __FUNCTION__) ;
				return we ;
			}

			// 입력 모드 확인.
			switch(m_byInputMode)												
			{
			// 방번호 입력 모드.
			case e_JoinDlg_Type_RoonIndex :
				{
					pDlg->Check_Room_To_Join(e_JoinDlg_Type_RoonIndex, m_pEdb_Code->GetEditText()) ;
				}
				break ;

			// 비밀번호 입력 모드.
			case e_JoinDlg_Type_SecretCode :
				{
					pDlg->Check_Room_To_Join(e_JoinDlg_Type_SecretCode, m_pEdb_Code->GetEditText()) ;
				}
				break ;

			default : break ;
			}
		}
	}

	return we ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetActive
//	DESC : 참여 다이얼로그 활성/비활성화 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomJoinDlg::SetActive(BOOL val)
{
	SetInputMode(m_byInputMode) ;												// Setting and active or not.
	
	cDialog::SetActive(val) ;													
}





//-------------------------------------------------------------------------------------------------
//	NAME : OnActionEvent
//	DESC : EventFunc로 부터 넘어온 Event를 처리하는 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomJoinDlg::OnActionEvent(LONG id, void* p, DWORD event)
{
	// 함수 파라메터 체크.
	ASSERT(p) ;																	// Check parameter.

	if(!p)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// 에디트 박스 체크.
	ASSERT(m_pEdb_Code) ;

	if(!m_pEdb_Code)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	//char tempStr[SECRET_CODE_SIZE] = {0,} ;										

	//SafeStrCpy(tempStr, m_pEdb_Code->GetEditText(), SECRET_CODE_SIZE) ;						// Receive string info.

	// 컨트롤 아이디 확인.
	switch(id)
	{
	// 확인 버튼.
	case CRJD_BTN_OK : 
		{
			// 메인 다이얼로그 받기.
			CChatRoomMainDlg* pDlg = NULL ;
			pDlg = GAMEIN->GetChatRoomMainDlg() ;						// Receive main dialog.

			ASSERT(pDlg) ;

			if(!pDlg)
			{
				CHATROOMMGR->Throw_Error("Failed to receive md!!", __FUNCTION__) ;
				return ;
			}

			// 입력모드 확인.
			switch(m_byInputMode)												// Check current this dialog type.
			{
			// 방번호 입력 모드.
			case e_JoinDlg_Type_RoonIndex :
				{
					pDlg->Check_Room_To_Join(e_JoinDlg_Type_RoonIndex, m_pEdb_Code->GetEditText()) ;

					//MSG_CR_JOIN_SYN msg ;
					//memset(&msg, 0, sizeof(MSG_CR_JOIN_SYN)) ;

					//msg.Category	= MP_CHATROOM ;
					//msg.Protocol	= MP_CHATROOM_JOIN_ROOM_SYN ;
					//msg.dwObjectID	= HEROID ;

					//msg.byRoomIdx	= (BYTE)atoi(tempStr) ;

					//NETWORK->Send( &msg, sizeof(MSG_CR_JOIN_SYN) ) ;

					//this->SetActive(FALSE) ;
				}
				break ;

			// 비밀번호 입력 모드.
			case e_JoinDlg_Type_SecretCode :									// Copy secret code & Send join message to server.
				{
					pDlg->Check_Room_To_Join(e_JoinDlg_Type_SecretCode, m_pEdb_Code->GetEditText()) ;

					//MSG_CR_JOIN_SYN msg ;
					//memset(&msg, 0, sizeof(MSG_CR_JOIN_SYN)) ;

					//msg.Category	= MP_CHATROOM ;
					//msg.Protocol	= MP_CHATROOM_JOIN_ROOM_SYN ;
					//msg.dwObjectID	= HEROID ;

					//msg.byRoomIdx	= m_byRoomIdx ;

					//SafeStrCpy(msg.code, m_pEdb_Code->GetEditText(), SECRET_CODE_SIZE) ;

					//NETWORK->Send( &msg, sizeof(MSG_CR_JOIN_SYN) ) ;

					//this->SetActive(FALSE) ;
				}
				break ;

			default : break ;
			}
		}
		break ;

	// 취소 버튼.
	case CRJD_BTN_CANCEL : this->SetActive(FALSE) ; break ;						// Deactive this dialog.

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetInputMode
//	DESC : 입력 모드를 설정하는 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomJoinDlg::SetInputMode(BYTE byMode)
{
	// 입력 모드 확인.
	if( byMode > e_JoinDlg_Type_SecretCode )									// Check input mode.
	{
		CHATROOMMGR->Throw_Error("Invalid md!!", __FUNCTION__) ;
		return ;
	}



	// 하위 컨트롤 체크.
	ASSERT(m_pStc_RoomIndex) ;													// Check controls.
	ASSERT(m_pStc_SecretCode) ;

	ASSERT(m_pStc_IndexDesc) ;
	ASSERT(m_pStc_CodeDesc) ;

	ASSERT(m_pEdb_Code) ;

	if( !m_pStc_RoomIndex || !m_pStc_SecretCode || !m_pEdb_Code || 
		!m_pStc_IndexDesc || !m_pStc_CodeDesc)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 입력 모드 확인.
	switch(byMode)																// Check current input mode.
	{
	// 방번호 입력모드.
	case e_JoinDlg_Type_RoonIndex :												// Setting room index mode.
		{
			// 방번호 입력 모드로 세팅.
			m_pStc_RoomIndex->SetActive(TRUE) ;
			m_pStc_SecretCode->SetActive(FALSE) ;

			m_pStc_IndexDesc->SetActive(TRUE) ;
			m_pStc_CodeDesc->SetActive(FALSE) ;

			//m_pEdb_Code->SetValidCheck(VCM_NORMAL_NUMBER) ;
		}
		break ;

	// 비밀번호 입력 모드.
	case e_JoinDlg_Type_SecretCode :											// Setting secret code mode.
		{
			// 비밀번호 입력 모드로 세팅.
			m_pStc_RoomIndex->SetActive(FALSE) ;
			m_pStc_SecretCode->SetActive(TRUE) ;

			m_pStc_IndexDesc->SetActive(FALSE) ;
			m_pStc_CodeDesc->SetActive(TRUE) ;

			//m_pEdb_Code->SetValidCheck(VCM_NORMAL_NUMBER) ;
		}
		break ;

	default : break ;
	}
	

	
	// 입력 모드 세팅.
	m_byInputMode = byMode ;													// Setting mode.



	// 에디트 박스 초기화.
	m_pEdb_Code->SetEditText("") ;												// Clear editbox.
	m_pEdb_Code->SetFocusEdit(TRUE) ;
}





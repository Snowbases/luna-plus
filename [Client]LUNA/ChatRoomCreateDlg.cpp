#include "stdafx.h"
#include "../WindowIDEnum.h"
#include "../cMsgBox.h"
#include "./Interface/cResourceManager.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cButton.h"
#include "./Interface/cComboBox.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cPushupButton.h"
#include "./ChatManager.h"
#include "./FilteringTable.h"
#include "./ChatRoomMgr.h"
#include "./ChatRoomCreateDlg.h"
#include "cIMEex.h"

CChatRoomCreateDlg::CChatRoomCreateDlg(void)
{
	// 초기화 처리.
	m_pOpen				= NULL ;																		// Initialize all member variables.
	m_pClose			= NULL ;

	m_pCodeBox			= NULL ;

	m_pGuestCount		= NULL ;
	m_pRoomType			= NULL ;

	m_pTitleBox			= NULL ;

	m_pOKBtn			= NULL ;
	m_pCancelBtn		= NULL ;

	m_bySecretMode		= e_RM_Open ;

	m_byGeustCount		= e_GC_20 ;
	m_byRoomType		= e_RT_Party ;

	m_bIsWritedTitle	= FALSE;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomCreateDlg
//	DESC : 소멸자 함수.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
CChatRoomCreateDlg::~CChatRoomCreateDlg(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME : Linking
//	DESC : 하위 컨트롤들을 링크하는 함수.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Linking()
{
	// 하위 컨트롤들을 링크.
	m_pOpen				= (cPushupButton*)GetWindowForID(CRCD_CHK_ON) ;									// Link child controls.
	m_pClose			= (cPushupButton*)GetWindowForID(CRCD_CHK_OFF) ;

	m_pCodeBox			= (cEditBox*)GetWindowForID(CRCD_EDB_CODE) ;

	m_pGuestCount		= (cComboBox*)GetWindowForID(CRCD_CMB_GUEST) ;
	m_pRoomType			= (cComboBox*)GetWindowForID(CRCD_CMB_ROOMTYPE) ;

	m_pTitleBox			= (cEditBox*)GetWindowForID(CRCD_EDB_TITLE) ;

	m_pOKBtn			= (cButton*)GetWindowForID(CRCD_BTN_OK) ;
	m_pCancelBtn		= (cButton*)GetWindowForID(CRCD_BTN_CANCEL) ;

	if( !m_pOpen	 || !m_pClose	 || !m_pCodeBox || !m_pGuestCount || 
		!m_pRoomType || !m_pTitleBox || !m_pOKBtn	|| !m_pCancelBtn )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	//m_pCodeBox->SetValidCheck( VCM_NORMAL_NUMBER ) ;													// Setting valid to number.



	// 기본 상태의 창으로 세팅.
	SetDefaultDlg() ;																					// Setting dialog state to default.



	// 채팅방 매니져에 생성창 등록.
	CHATROOMMGR->SetCreateDlg(this) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetActive
//	DESC : 채팅방 생성 다이얼로그를 기본 상태로 세팅하는 함수.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::SetActive(BOOL val)
{
	if(val) SetDefaultDlg() ;																			// Setting dialog state to default.

	cDialog::SetActive(val) ;																			// Active this dialog or not.
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetDefaultDlg
//	DESC : 채팅방 생성 창을 기본상태로 세팅하는 함수.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::SetDefaultDlg() 
{
	if( !m_pOpen || !m_pClose || !m_pCodeBox || !m_pGuestCount || !m_pTitleBox )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 채팅방 공개모드 세팅.
	m_pOpen->SetPush(TRUE) ;
	m_pClose->SetPush(FALSE) ;

	m_bySecretMode		= e_RM_Open ;

	// 채팅방 제목 세팅.
	m_pCodeBox->SetEditText("") ;
	m_pCodeBox->SetDisable(TRUE) ;
	m_pCodeBox->SetFocusEdit(FALSE) ;
	m_pGuestCount->SetMaxLine(e_GC_Max);
	m_pGuestCount->SetCurSelectedIdx(0) ;
	m_pGuestCount->SelectComboText(0) ;

	m_pRoomType->SetMaxLine(e_RT_Max);
	m_pRoomType->SetCurSelectedIdx(0);
	m_pRoomType->SelectComboText(0);

	m_byGeustCount = e_GC_20;
	m_byRoomType = e_RT_Party;

	m_pTitleBox->SetEditText(CHATMGR->GetChatMsg(1479)) ;												// Setting editbox for title of chatroom.
	m_bIsWritedTitle = FALSE;
}





//-------------------------------------------------------------------------------------------------
//	NAME : OnActionEvent
//	DESC : EventFunc로 부터 넘어온 Event를 처리하는 함수.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::OnActionEvent(LONG id, void* p, DWORD event)
{
	if(!p)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// Check id & Handling message.
	switch(id)
	{
	case CRCD_CHK_ON :			Chk_Open() ;		break ;
	case CRCD_CHK_OFF :			Chk_Close() ;		break ;

	case CRCD_CMB_GUEST :		Cmb_Guest() ;		break ;
	case CRCD_CMB_ROOMTYPE :	Cmb_Roomtype() ;	break ;

	case CRCD_BTN_OK :			Btn_Ok() ;			break ;
	case CRCD_BTN_CANCEL :		SetActive(FALSE);	break ;
	// 100205 ONS 최초 제목에디트 클릭시 박스 초기화
	case CRCD_EDB_TITLE:
		{
			if( !m_bIsWritedTitle )
			{
				m_pTitleBox->SetEditText(""); 
				m_bIsWritedTitle = TRUE;
			}
		}
		break;
	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Chk_Open
//	DESC : 공개모드로 세팅하는 함수.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Chk_Open() 
{
	// 하위 컨트롤 체크.
	ASSERT(m_pOpen) ;
	ASSERT(m_pClose) ;

	if(!m_pOpen || !m_pClose)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 공개 모드로 세팅.
	BOOL bChecked = FALSE ;

	bChecked = m_pOpen->IsPushed() ;																	// Receive current state.
	if(!bChecked) m_pOpen->SetPush(TRUE) ;

	m_bySecretMode = e_RM_Open ;
	m_pClose->SetPush(FALSE) ;																			// Switching secret mode.

	m_pCodeBox->SetEditText("") ;
	m_pCodeBox->SetDisable(TRUE) ;
	m_pCodeBox->SetFocusEdit(FALSE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Chk_Close
//	DESC : 비공개 모드로 세팅하는 함수.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Chk_Close()
{
	if(!m_pOpen || !m_pClose)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 비공개 모드로 세팅.
	BOOL bChecked = FALSE ;

	bChecked = m_pClose->IsPushed() ;																	// Receive current state.
	if(!bChecked) m_pClose->SetPush(TRUE) ;

	m_bySecretMode = e_RM_Close ;
	m_pOpen->SetPush(FALSE) ;																			// Switching secret mode.

	m_pCodeBox->SetEditText("") ;
	m_pCodeBox->SetDisable(FALSE) ;
	m_pCodeBox->SetFocusEdit(TRUE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Cmb_Guest
//	DESC : 최대 참여자 수를 세팅하는 함수.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Cmb_Guest()
{
	if(!m_pGuestCount)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	m_byGeustCount = BYTE(m_pGuestCount->GetCurSelectedIdx());
}





//-------------------------------------------------------------------------------------------------
//	NAME : Cmb_Roomtype
//	DESC : 채팅방 분류를 세팅하는 함수.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Cmb_Roomtype()
{
	if(!m_pRoomType)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	switch(m_pRoomType->GetCurSelectedIdx())
	{
	case 0 : m_byRoomType = e_RT_Party ;		break ;
	case 1 : m_byRoomType = e_RT_Deal ;			break ;
	case 2 : m_byRoomType = e_RT_Normal_Chat ;	break ;
	case 3 : m_byRoomType = e_RT_Local_Chat ;	break ;
	case 4 : m_byRoomType = e_RT_Make_Friend ;	break ;
	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Btn_Ok
//	DESC : 확인 버튼 이벤트를 처리하는 함수.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Btn_Ok()
{
	if(!m_pCodeBox || !m_pTitleBox)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 비밀번호 받기.
	// 080704 LYW --- ChatRoomCreateDlg : 임시 버퍼 사이즈 확장.
	//char codeBuf[SECRET_CODE_SIZE] = {0,} ;
	char codeBuf[256] = {0,} ;
	strcpy(codeBuf, m_pCodeBox->GetEditText()) ;

	// 비밀번호 체크.
	if(m_bySecretMode == e_RM_Close)
	{
		if( strcmp("", codeBuf) == 0 )
		{
			WINDOWMGR->MsgBox(	MBI_CR_SECRETCODE_ERR, 
								MBT_OK, 
								CHATMGR->GetChatMsg(1478) ) ;
			return ;
		}
	}

	// 비밀번호 필터링.
	if( FILTERTABLE->FilterChat(codeBuf) )
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1225)) ;
		m_pCodeBox->SetEditText("") ;
		SetActive(FALSE) ;
		return;
	}



	// 제목 받기.
	// 080704 LYW --- ChatRoomCreateDlg : 임시 버퍼 사이즈 확장.
	//char titleBuf[TITLE_SIZE] = {0,} ;
	char titleBuf[256] = {0,} ;
	strcpy(titleBuf, m_pTitleBox->GetEditText()) ;

	// 제목 체크.
	if( strcmp("", titleBuf) == 0 )
	{
		if( strcmp("", codeBuf) == 0 )
		{
			WINDOWMGR->MsgBox(	MBI_CR_ROOMTITLE_ERR, 
								MBT_OK, 
								CHATMGR->GetChatMsg(1479) ) ;
			return ;
		}
	}

	// 제목 필터링.
	if( FILTERTABLE->FilterChat(titleBuf) )
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1225)) ;
		m_pTitleBox->SetEditText("") ;
		SetActive(FALSE) ;
		return;
	}



	// 재사용 시간 체크.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}



	// 채팅방 생성 정보 세팅 후 메시지 전송.
	MSG_CR_ELEMENT msg ;
	memset(&msg, 0, sizeof(MSG_CR_ELEMENT)) ;

	msg.Category			= MP_CHATROOM ;
	msg.Protocol			= MP_CHATROOM_CREATE_ROOM_SYN ;

	msg.dwObjectID			= HEROID ;

	msg.bySecretMode		= m_bySecretMode ;
	SafeStrCpy(msg.code, m_pCodeBox->GetEditText(), SECRET_CODE_SIZE) ;

	msg.byTotalGuestCount	= m_byGeustCount ;
	msg.byRoomType			= m_byRoomType ;

	SafeStrCpy(msg.title, titleBuf, 64) ;

	NETWORK->Send( &msg, sizeof(msg) ) ;																// Send message to server.

	SetActive(FALSE) ;

	// 시간 업데이트.
	CHATROOMMGR->UpdateMyLastComTime() ;
}
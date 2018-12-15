#include "stdafx.h"
#include "../WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "../Interface/cResourceManager.h"
#include "./GameIn.h"
#include "./ChatManager.h"
#include "./ChatRoomMgr.h"
#include "./ChatRoomDlg.h"
#include "./ChatRoomGuestListDlg.h"
#include "./ChatRoomOptionDlg.h"
#include "./Input/Mouse.h"
#include "./Input/Keyboard.h"
#include "./Interface/cButton.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cListDialog.h"
#include "./Interface/cStatic.h"
#include "./cMsgBox.h"
#include "Interface/cScriptManager.h"
#include "./FilteringTable.h"
#include "./ObjectManager.h"

CChatRoomDlg::CChatRoomDlg(void)
{
	// 초기화 처리.
	m_pRoomTitle		= NULL ;

	m_pChatList			= NULL ;

	m_pInputBox			= NULL ;

	m_pBtn_GuestList	= NULL ;
	m_pBtn_Option		= NULL ;

	m_pBtn_Close		= NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomDlg
//	DESC : 소멸자 함수.
//-------------------------------------------------------------------------------------------------
CChatRoomDlg::~CChatRoomDlg(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME : Linking
//	DESC : 하위 컨트롤 링크 함수.
//-------------------------------------------------------------------------------------------------
void CChatRoomDlg::Linking()
{
	// 하위 컨트롤 링크.
	m_pRoomTitle		= (cStatic*)GetWindowForID(CRD_STC_ROOMTITLE) ;

	m_pChatList			= (cListDialog*)GetWindowForID(CRD_LST_CHATLIST) ;

	m_pInputBox			= (cEditBox*)GetWindowForID(CRD_EDB_INPUTBOX) ;
	//m_pInputBox			= (cTextArea*)GetWindowForID(CRD_EDB_INPUTBOX) ;

	m_pBtn_GuestList	= (cButton*)GetWindowForID(CRD_BTN_GUESTLIST) ;
	m_pBtn_Option		= (cButton*)GetWindowForID(CRD_BTN_OPTION) ;

	m_pBtn_Close		= (cButton*)GetWindowForID(CRD_BTN_CLOSE_DLG) ;

	if(!m_pRoomTitle || !m_pChatList || !m_pInputBox || !m_pBtn_GuestList || !m_pBtn_Option || !m_pBtn_Close)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 채팅방 매니져에 등록.
	CHATROOMMGR->SetChatRoomDlg(this) ;
}

void CChatRoomDlg::OnActionEvent(LONG id, void* p, DWORD event)
{
	if(!p)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	if(!m_pBtn_GuestList || !m_pBtn_Option)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 컨트롤 아이디 체크.
	switch(id)
	{
	// 참여자 버튼.
	case CRD_BTN_GUESTLIST : 
		{
			// 참여자 다이얼로그 받기.
			CChatRoomGuestListDlg* pDlg = NULL ;
			pDlg = GAMEIN->GetChatRoomGuestListDlg() ;

			ASSERT(pDlg) ;

			if(!pDlg)
			{
				CHATROOMMGR->Throw_Error("Failed to receive gld!!", __FUNCTION__) ;
				return ;
			}

			// 참여자 다이얼로그 열기/닫기 처리.
			pDlg->SetActive(!pDlg->IsActive()) ;
		}
		break ;

	// 채팅 옵션 버튼.
	case CRD_BTN_OPTION :
		{
			// 방장인지 체크.
			if(CHATROOMMGR->GetMyRoomOwnerIdx() != HEROID)
			{
				AddMsg(CHATMGR->GetChatMsg(1487), e_ChatMsg_System) ;
				return ;
			}

			// 옵션 다이얼로그 받기.
			CChatRoomOptionDlg* pDlg = NULL ;
			pDlg = GAMEIN->GetChatRoomOptionDlg() ;

			ASSERT(pDlg) ;

			if(!pDlg)
			{
				CHATROOMMGR->Throw_Error("Failed to receive od!!", __FUNCTION__) ;
				return ;
			}

			// 옵션창 열기/닫기 처리.
			pDlg->SetActive(!pDlg->IsActive()) ;
		}
		break ;

	// 채팅방 닫기 버튼.
	case CRD_BTN_CLOSE_DLG :
		{
			// 종료 확인 알림창 활성화.
			WINDOWMGR->MsgBox( MBI_CR_ENDCHAT, MBT_YESNO, CHATMGR->GetChatMsg(1480) ) ;
		}
		break ;

	default : break ;
	}
}

void CChatRoomDlg::AddMsg(char* pMsg, BYTE byType)
{
	if(!pMsg) 
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	char buf[MAX_PATH] = {0};
	int nLen = strlen( pMsg );
	int nCpyNum = 0;
	int ntextLen = 54 ;

	while( nLen > ntextLen )
	{
		if( (pMsg + ntextLen ) != CharNext( CharPrev( pMsg, pMsg + ntextLen ) ) )
		{
			nCpyNum = ntextLen - 1 ;
		}
		else
		{
			nCpyNum = ntextLen ;
		}

		strncpy( buf, pMsg, nCpyNum ) ;
		buf[ nCpyNum ] = 0 ;

		m_pChatList->AddItem( buf, GetMsgColor(byType) ) ;

		nLen -= nCpyNum ;
		pMsg  += nCpyNum ;

		if( *pMsg == ' ' ) ++pMsg ;
	}

	if( nLen > 0 )
	{
		m_pChatList->AddItem( pMsg, GetMsgColor(byType) ) ;
	}

	//m_pChatList->AddItem(pMsg, GetMsgColor(byType)) ;
}

void CChatRoomDlg::AddCommonMsg(BYTE byType, char* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// 메시지 타입 확인.
	// 080704 LYW --- ChatRoomDlg : 임시 버퍼 사이즈 확장.
	//char szStr[128] = {0,} ;
	char szStr[256] = {0,} ;

	strcat(szStr, "[") ;
	strcat(szStr, "<") ;
	switch(byType)
	{
	// 채팅방 입장.
	case e_ChatMsg_StepIn :
		{
			strcat(szStr, pMsg) ;
			strcat(szStr, ">") ;
			strcat(szStr, CHATMGR->GetChatMsg(1481)) ;
		}
		break ;

	// 채팅방 퇴장.
	case e_ChatMsg_StepOut :
		{
			strcat(szStr, pMsg) ;
			strcat(szStr, ">") ;
			strcat(szStr, CHATMGR->GetChatMsg(1482)) ;
		}
		break ;

	default : break ;
	}
	strcat(szStr, "]") ;

	// 메시지 출력.
	m_pChatList->AddItem(szStr, GetMsgColor(byType)) ;
}

void CChatRoomDlg::SetRoomTitle(ST_CR_ROOM_CLT* pInfo)
{
	if(!pInfo)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	char tempMsg[256] = {0, } ;
	char tempRoomNum[256] = {0, } ;

	strcat(tempMsg, "[") ;
	sprintf(tempRoomNum, CHATMGR->GetChatMsg(1483), pInfo->byIdx) ;
	strcat(tempMsg, tempRoomNum) ;
	strcat(tempMsg, "]") ;
	strcat(tempMsg, "-") ;



	// 공개/비공개 세팅.
	strcat(tempMsg, "[") ;
	switch(pInfo->bySecretMode)
	{
	case e_RM_Open :	strcat(tempMsg, RESRCMGR->GetMsg(892)) ; break ;
	case e_RM_Close :	strcat(tempMsg, RESRCMGR->GetMsg(893)) ; break ;
	default : break ;
	}
	strcat(tempMsg, "]") ;



	// 채팅방 분류 세팅.
	strcat(tempMsg, "[") ;
	switch(pInfo->byRoomType)
	{
	case e_RT_Party :			strcat(tempMsg, RESRCMGR->GetMsg(880)) ;	break ;
	case e_RT_Deal :			strcat(tempMsg, RESRCMGR->GetMsg(881)) ;	break ;
	case e_RT_Normal_Chat :		strcat(tempMsg, RESRCMGR->GetMsg(882)) ;	break ;
	case e_RT_Local_Chat :		strcat(tempMsg, RESRCMGR->GetMsg(883)) ;	break ;
	case e_RT_Make_Friend :		strcat(tempMsg, RESRCMGR->GetMsg(884)) ;	break ;
	default : break ;
	}
	strcat(tempMsg, "]") ;



	// 채팅방 제목 세팅.
	m_pRoomTitle->SetStaticText(tempMsg) ;



	// 툴팁 세팅.
	m_pRoomTitle->SetToolTip(pInfo->title, RGBA_MAKE(255, 255, 255, 255)) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetMsgColor
//	DESC : 메시지 타입에 따라 색상을 반환하는 함수.
//-------------------------------------------------------------------------------------------------
DWORD CChatRoomDlg::GetMsgColor(BYTE byColorType)
{
	// 색상 변수 선언.
	DWORD dwColor = RGBA_MAKE(255, 255, 255, 255) ;



	// 메시지 타입 확인.
	switch(byColorType)
	{
	case e_ChatMsg_StepIn :
	case e_ChatMsg_StepOut :
	case e_ChatMsg_System :				dwColor = RGBA_MAKE(255, 255, 0, 255) ; break ;

	case e_ChatMsg_MyMsg :				dwColor = RGBA_MAKE(0, 255, 0, 255) ; break ;
	case e_ChatMsg_OtherMsg :			dwColor = RGBA_MAKE(255, 255, 255, 255) ; break ;

	case e_ChatMsg_Whisper_Sener :		dwColor = RGBA_MAKE(255, 0, 255, 255) ; break ;
	case e_ChatMsg_Whisper_Receiver :	dwColor = RGBA_MAKE(0, 255, 255, 255) ; break ;
	default : break ;
	}



	// 세팅 된 색상 반환.
	return dwColor ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : CloseChatRoom
//	DESC : 채팅방을 닫는 처리를 하는 함수.
//  DATE : APRIL 29, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomDlg::CloseChatRoom()
{
	SetActive(FALSE) ;
	SetDlgToDefault() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetDlgToDefault
//	DESC : 채팅창을 처음(기본) 상태로 초기화 하는 함수.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomDlg::SetDlgToDefault()
{
	// 채팅방 제목 초기화.
	m_pRoomTitle->SetStaticText("") ;

	// 채팅 입력 내용 초기화.
	m_pInputBox->SetEditText("") ;
	//m_pInputBox->SetScriptText("") ;

	// 채팅 내용 초기화.
	m_pChatList->RemoveAll() ;
}
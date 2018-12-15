#include "StdAfx.h"
#include "../WindowIDEnum.h"

#include "./Input/Mouse.h"
#include "./Input/Keyboard.h"

#include "./FilteringTable.h"

#include "./Interface/cResourceManager.h"
#include "./Interface/cWindowManager.h"

#include "./Interface/cButton.h"
#include "./Interface/cComboBox.h"
#include "./Interface/cPushupButton.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cListDialog.h"
#include "./Interface/cStatic.h"

#include "./cMsgBox.h"
#include "./GameIn.h"

#include "./ObjectManager.h"
#include "./ChatManager.h"
#include "./ChatRoomMgr.h"

#include "./ChatRoomCreateDlg.h"
#include "./ChatRoomDlg.h"
#include "./ChatRoomGuestListDlg.h"
#include "./ChatRoomJoinDlg.h"
#include "./ChatRoomMainDlg.h"
#include "./ChatRoomOptionDlg.h"

CChatRoomMainDlg::CChatRoomMainDlg(void)
{
	// 초기화 처리.
	m_pRoomList			= NULL ;

	m_pSearchBtn		= NULL ;
	m_pSearchBox		= NULL ;
	m_pTitle			= NULL ;
	m_pName				= NULL ;

	m_pRoomType			= NULL ;

	m_pCreateBtn		= NULL ;
	m_pJoinBtn			= NULL ;
	m_pNumJoinBtn		= NULL ;
	m_pRefreshBtn		= NULL ;

	m_pPrevBtn			= NULL ;
	m_pNextBtn			= NULL ;

	m_pPage				= NULL ;

	//m_pLoad_Chatroom	= NULL ;

	//memset(m_SearchBuff, 0, sizeof(m_SearchBuff)) ;

	m_bySearchMode		= e_Search_Title ;

	//m_byRoomType		= e_RTM_Party ;

	m_byCurPage			= 0 ;
	m_byMaxPage			= 0 ;

	memset(&m_SelectRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	m_byRoomType		= e_RTM_AllLooK ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomMainDlg
//	DESC : 소멸자 함수.
//  DATE : APRIL 3, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomMainDlg::~CChatRoomMainDlg(void)
{
}


void CChatRoomMainDlg::Linking()
{
	// 하위 컨트롤 포인터 링크.
	m_pRoomList			= (cListDialog*)GetWindowForID(CRMD_LST_ROOMLIST) ;

	m_pSearchBtn		= (cButton*)GetWindowForID(CRMD_BTN_SEARCH) ;
	m_pSearchBox		= (cEditBox*)GetWindowForID(CRMD_EDB_SEARCH) ;
	m_pTitle			= (cPushupButton*)GetWindowForID(CRMD_CHK_TITLE) ;
	m_pName				= (cPushupButton*)GetWindowForID(CRMD_CHK_NAME) ;

	m_pRoomType			= (cComboBox*)GetWindowForID(CRMD_CMB_ROOMTYPE) ;

	m_pCreateBtn		= (cButton*)GetWindowForID(CRMD_BTN_CREATE) ;
	m_pJoinBtn			= (cButton*)GetWindowForID(CRMD_BTN_JOIN) ;
	m_pNumJoinBtn		= (cButton*)GetWindowForID(CRMD_BTN_NUMJOIN) ;
	m_pRefreshBtn		= (cButton*)GetWindowForID(CRMD_BTN_REFRESH) ;

	m_pPrevBtn			= (cButton*)GetWindowForID(CRMD_BTN_PREV) ;
	m_pNextBtn			= (cButton*)GetWindowForID(CRMD_BTN_NEXT) ;

	m_pPage				= (cStatic*)GetWindowForID(CRMD_STC_PAGE) ;

	if( !m_pRoomList	|| !m_pSearchBtn	|| !m_pSearchBox || !m_pTitle || !m_pName ||
		!m_pRoomType	|| !m_pCreateBtn	|| !m_pJoinBtn  ||
		!m_pNumJoinBtn	|| !m_pRefreshBtn	|| //!m_pLoad_Chatroom ||
		!m_pPrevBtn		|| !m_pNextBtn		|| !m_pPage )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 채팅방 분류 세팅.
	//m_pRoomType->SetCurSelectedIdx(e_RTM_AllLooK) ;
	m_pRoomType->SetMaxLine(e_RT_Max) ;



	// 기본 검색 모드 세팅.
	if(m_pTitle)		m_pTitle->SetPush(TRUE) ;
	if(m_pName)			m_pName->SetPush(FALSE) ;

	// 로딩 스태틱 비활성화.
	//m_pLoad_Chatroom->SetActive(FALSE) ;



	// 페이지 세팅.
	//SetPage(1, 1) ;
	m_pPage->SetStaticText("") ;



	// 채팅방 매니져에 등록.
	CHATROOMMGR->SetMainDlg(this) ;
}

DWORD CChatRoomMainDlg::ActionEvent( CMouse* mouseInfo ) 
{
	DWORD we = WE_NULL ;

	// 함수 파라메터 체크.
	ASSERT(mouseInfo) ;

	if( !mouseInfo )
	{
		CHATROOMMGR->Throw_Error("Invalid a mouse info!!", __FUNCTION__) ;
		return we ;
	}



	// 다이얼로그 이벤트를 받는다.
	we = cDialog::ActionEvent( mouseInfo ) ;



	// 유저 리스트 상에 마우스가 있으면,
	if(m_pRoomList->PtInWindow( (LONG)mouseInfo->GetMouseX(), (LONG)mouseInfo->GetMouseY() ))
	{
		// 클릭 이벤트가 발생했을 경우.
		if( we & WE_LBTNCLICK )
		{
			ITEM* pItem ;

			DWORD dwColor ;

			LONG lItemCount = 0 ;
			lItemCount = m_pRoomList->GetItemCount() ;

			int nSelectedIdx = 0 ;
			nSelectedIdx  = m_pRoomList->GetSelectRowIdx() ;
			
			for(LONG count = 0 ; count < lItemCount ; ++count)
			{
				pItem = NULL ;
				pItem = m_pRoomList->GetItem(count) ;

				if(!pItem) continue ;

				if(pItem->line == nSelectedIdx)
				{
					dwColor = RGBA_MAKE(255, 0, 0, 255) ;
				}
				else
				{
					dwColor = RGBA_MAKE(255, 255, 255, 255) ;
				}

				pItem->rgb = dwColor ;
			}

			LIST_ROOM::iterator it ;
			it = m_Room.begin() ;

			std::advance( it, nSelectedIdx ) ;

			if(it == m_Room.end())
			{
				memset(&m_SelectRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;
			}
			else
			{
				m_SelectRoom = *it ;
			}
		}
		else if( we & WE_LBTNDBLCLICK )
		{
			Join_Syn() ;
		}
	}

	return we ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : OnActionEvent
//	DESC : 하위컨트롤에 이벤트가 발생했을 때 처리하는 함수.
//  DATE : APRIL 3, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::OnActionEvent(LONG id, void* p, DWORD event)
{
	// 함수 파라메터 체크.
	ASSERT(p) ;

	if(!p)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// 컨트롤 아이디 확인.
	switch(id)
	{
	case CRMD_BTN_SEARCH :		Search_Syn() ;			break ;

	case CRMD_CHK_TITLE :		Check_Title() ;			break ;
	case CRMD_CHK_NAME :		Check_Name() ;			break ;

	case CRMD_BTN_CREATE :		Create_Syn() ;			break ;
	case CRMD_BTN_JOIN :		Join_Syn() ;			break ;
	case CRMD_BTN_NUMJOIN :		NumJoin_Syn() ;			break ;
	case CRMD_BTN_REFRESH :		Refresh_Syn() ;			break ;

	case CRMD_BTN_PREV :		Btn_Prev() ;			break ;
	case CRMD_BTN_NEXT :		Btn_Next() ;			break ;

	case CRMD_CMB_ROOMTYPE :	
	{
		if( event == WE_COMBOBOXSELECT )
		{
			//SortRoomByType() ;

			// 재사용 시간 체크.
			if(!CHATROOMMGR->IsEnableExecuteCom())
			{
				WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
				return ;
			}

			m_byRoomType = (BYTE)m_pRoomType->GetCurSelectedIdx() ;

			MSG_BYTE2 msg ;
			memset(&msg, 0, sizeof(MSG_BYTE2)) ;

			msg.Category		= MP_CHATROOM ;
			msg.Protocol		= MP_CHATROOM_ROOM_SYN ;
			msg.dwObjectID		= HEROID ;

			msg.bData1			= m_byRoomType ;
			msg.bData2			= m_byCurPage ;

			NETWORK->Send( &msg, sizeof(MSG_BYTE2) ) ;
			CHATROOMMGR->UpdateMyLastComTime() ;
		}
	}
	break ;

	default : break ;
	}
}

void CChatRoomMainDlg::Search_Syn()
{
	// 하위 컨트롤 체크.
	ASSERT(m_pSearchBox) ;

	if(!m_pSearchBox)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 검색 내용 체크.
	char* word ;

	word = NULL ;
	word = m_pSearchBox->GetEditText() ;

	m_pSearchBox->SetEditText("") ;

	if(strlen(word) <= 1)
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1491)) ;
		return ;
	}

	// 필터링 체크.
	if( FILTERTABLE->FilterChat(word) )
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1225)) ;
		return ;
	}

	// 검색 모드 체크.
	if(m_bySearchMode == e_Search_Title)
	{
		MSG_CR_SEARCH_TITLE msg ;
		memset(&msg, 0, sizeof(MSG_CR_SEARCH_TITLE)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_SEARCH_FOR_TITLE_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.byRoomIdx	= 0 ;
		
		SafeStrCpy(msg.title, word, TITLE_SIZE) ;

		NETWORK->Send( &msg, sizeof(MSG_CR_SEARCH_TITLE) ) ;
	}
	else
	{
		if( strcmp(word, HERO->GetObjectName()) == 0)
		{
			WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1538)) ;
			return ;
		}

		MSG_CR_SEARCH_NAME msg ;
		memset(&msg, 0, sizeof(MSG_CR_SEARCH_NAME)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_SEARCH_FOR_NAME_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.byRoomIdx	= 0 ;
		
		SafeStrCpy(msg.name, word, MAX_NAME_LENGTH + 1) ;

		NETWORK->Send( &msg, sizeof(MSG_CR_SEARCH_NAME) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Check_Title
//	DESC : 검색 모드를 방제목 검색으로 세팅한다.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Check_Title() 
{
	// 하위 컨트롤 체크.
	ASSERT(m_pTitle) ;
	ASSERT(m_pName) ;

 	if(!m_pTitle || !m_pName) 
 	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
 	}

	m_pTitle->SetPush(TRUE) ;
 	m_pName->SetPush(FALSE) ;

 	m_bySearchMode = e_Search_Title ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Check_Name
//	DESC : 검색모드를 이름 검색으로 세팅한다.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Check_Name()
{
	// 하위 컨트롤 체크.
	ASSERT(m_pTitle) ;
	ASSERT(m_pName) ;

 	if(!m_pTitle || !m_pName) 
 	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
 	}

	m_pName->SetPush(TRUE) ;
 	m_pTitle->SetPush(FALSE) ;

 	m_bySearchMode = e_Search_Name ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Create_Syn
//	DESC : 채팅방 생성 다이얼로그를 연다.
//  DATE : APRIL 3, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Create_Syn()
{
	if(CHATROOMMGR->Get_ChatRoomState() == e_State_In_ChatRoom) 
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1505)) ;
		return ;
	}

	// 채팅방 생성 창 받기.
	CChatRoomCreateDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomCreateDlg() ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive create dialog!!", __FUNCTION__) ;
		return ;
	}

	pDlg->SetActive(TRUE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Join_Syn
//	DESC : 선택한 방에 참여하기를 요청하는 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Join_Syn()
{
	if(CHATROOMMGR->Get_ChatRoomState() == e_State_In_ChatRoom) 
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1505)) ;
		return ;
	}
	
	// 선택한 채팅방 체크.
	if(m_SelectRoom.byIdx == 0)
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1516)) ;
		return ;
	}

	//MSG_CR_JOIN_SYN msg ;
	//memset(&msg, 0, sizeof(MSG_CR_JOIN_SYN)) ;

	//msg.Category	= MP_CHATROOM ;
	//msg.Protocol	= MP_CHATROOM_JOIN_ROOM_SYN ;
	//msg.dwObjectID	= HEROID ;

	//msg.byRoomIdx	= m_SelectRoom.byIdx ;

	//NETWORK->Send( &msg, sizeof(MSG_CR_JOIN_SYN) ) ;

	// 공개모드라면,
	if(m_SelectRoom.bySecretMode == e_RM_Open)
	{
		// 참여 요청을 한다.
		MSG_CR_JOIN_SYN msg ;
		memset(&msg, 0, sizeof(MSG_CR_JOIN_SYN)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_JOIN_ROOM_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.byRoomIdx	= m_SelectRoom.byIdx ;

		NETWORK->Send( &msg, sizeof(MSG_CR_JOIN_SYN) ) ;
	}
	// 비공개 모드라면,
	else if(m_SelectRoom.bySecretMode == e_RM_Close)
	{
		// 참여 창을 받는다.
		CChatRoomJoinDlg* pDlg = NULL ;
		pDlg = GAMEIN->GetChatRoomJoinDlg() ;

		if(!pDlg)
		{
			CHATROOMMGR->Throw_Error("Failed to receive join dialog!!", __FUNCTION__) ;
			return ;
		}

		// 비밀번호 입력 모드로 활성화 시킨다.
		pDlg->SetInputMode(e_JoinDlg_Type_SecretCode) ;
		pDlg->SetActive(TRUE) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : NumJoin_Syn
//	DESC : 번호 입력으로 참여하기를 눌렀을 때 처리를 하는 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::NumJoin_Syn() 
{
	if(CHATROOMMGR->Get_ChatRoomState() == e_State_In_ChatRoom) 
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1505)) ;
		return ;
	}

	// 참여 창 받기.
	CChatRoomJoinDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomJoinDlg() ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive join dialog!!", __FUNCTION__) ;	
		return ;
	}

	// 방번호 입력 모드로 참여창 열기.
	pDlg->SetInputMode(e_JoinDlg_Type_RoonIndex) ;
	pDlg->SetActive(TRUE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Refresh_Syn
//	DESC : 새로고침 요청을 처리하는 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Refresh_Syn()
{
	//// 하위 컨트롤 체크.
	//ASSERT(m_pLoad_Chatroom) ;

 //	if(!m_pLoad_Chatroom) 
 //	{
	//	CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
	//	return ;
 //	}

	//m_pLoad_Chatroom->SetActive(TRUE) ;

	// 재사용 시간 체크.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}

	MSG_BYTE2 msg ;
	memset(&msg, 0, sizeof(MSG_BYTE2)) ;

	msg.Category		= MP_CHATROOM ;
	msg.Protocol		= MP_CHATROOM_ROOM_SYN ;
	msg.dwObjectID		= HEROID ;

	if(m_byRoomType == e_RTM_Searched)
	{
		msg.bData1			= e_RTM_AllLooK ;
		msg.bData2			= 0 ;
	}
	else
	{
		msg.bData1			= m_byRoomType ;
		msg.bData2			= m_byCurPage ;
	}

	NETWORK->Send( &msg, sizeof(MSG_BYTE2) ) ;

	// 시간 업데이트.
	CHATROOMMGR->UpdateMyLastComTime() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Btn_Prev
//	DESC : 이전 페이지 요청을 하는 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Btn_Prev() 
{
	// 페이지 확인.
	if(m_byCurPage == 0) return ;
	else
	{
		//// 하위 컨트롤 체크.
		//ASSERT(m_pLoad_Chatroom) ;

 	//	if(!m_pLoad_Chatroom) 
 	//	{
		//	CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		//	return ;
 	//	}

		//m_pLoad_Chatroom->SetActive(TRUE) ;

		// 재사용 시간 체크.
		if(!CHATROOMMGR->IsEnableExecuteCom())
		{
			WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
			return ;
		}

		if( m_byCurPage == 0 ) return ;

		MSG_BYTE2 msg ;
		memset(&msg, 0, sizeof(MSG_BYTE2)) ;

		msg.Category		= MP_CHATROOM ;
		msg.Protocol		= MP_CHATROOM_ROOM_SYN ;
		msg.dwObjectID		= HEROID ;

		msg.bData1			= m_byRoomType ;
		msg.bData2			= m_byCurPage-1 ;

		NETWORK->Send( &msg, sizeof(MSG_BYTE2) ) ;
		CHATROOMMGR->UpdateMyLastComTime() ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Btn_Next
//	DESC : 다음 페이지 요청을 하는 함수.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Btn_Next()
{
	// 페이지 확인.
	if(m_byCurPage == m_byMaxPage) return ;
	else
	{
		//// 하위 컨트롤 체크.
		//ASSERT(m_pLoad_Chatroom) ;

 	//	if(!m_pLoad_Chatroom) 
 	//	{
		//	CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		//	return ;
 	//	}

		//m_pLoad_Chatroom->SetActive(TRUE) ;

		// 재사용 시간 체크.
		if(!CHATROOMMGR->IsEnableExecuteCom())
		{
			WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
			return ;
		}

		if(m_byCurPage >= m_byMaxPage) return ;

		MSG_BYTE2 msg ;
		memset(&msg, 0, sizeof(MSG_BYTE2)) ;

		msg.Category		= MP_CHATROOM ;
		msg.Protocol		= MP_CHATROOM_ROOM_SYN ;
		msg.dwObjectID		= HEROID ;

		msg.bData1			= m_byRoomType ;
		msg.bData2			= m_byCurPage+1 ;

		NETWORK->Send( &msg, sizeof(MSG_BYTE2) ) ;
		CHATROOMMGR->UpdateMyLastComTime() ;
	}
}

void CChatRoomMainDlg::Check_Room_To_Join(BYTE inputMode, char* pCode)
{
	// 함수 파라메터 체크.
	ASSERT(pCode) ;

	if(!pCode || strlen(pCode) == 0)
	{
		//CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1510)) ;
		return ;
	}

	// 참여창 받기.
	CChatRoomJoinDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomJoinDlg() ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive join dialog!!", __FUNCTION__) ;
		return ;
	}

	// 채팅방이 담긴 Map의 시작 받기.
	LIST_ROOM::iterator it ;
	it = m_Room.begin() ;

	// 방이 없으면 메시지 박스 출력.
	if(it == m_Room.end())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1490)) ;

		m_pRoomList->RemoveAll() ;

		m_Room.clear() ;
		m_TempRoom.clear() ;

		return ;
	}

	// 방번호 입력 모드면, 
	if(inputMode == e_JoinDlg_Type_RoonIndex)
	{
		BYTE byCheckCount = BYTE(m_Room.size());

		for(BYTE count = 0 ; count < byCheckCount ; ++count)
		{
			if(it->byIdx != (BYTE)(atoi(pCode)))
			{
				++it ;
				continue ;
			}

			// 선택한 방 설정.
			m_SelectRoom = *it ;

			// 공개/비공개 확인.
			if(it->bySecretMode == e_RM_Close)
			{
				// 비밀번호 창을 활성화 한다.
				pDlg->SetInputMode(e_JoinDlg_Type_SecretCode) ;
				pDlg->SetActive(TRUE) ;
				return ;
			}
		}

		MSG_CR_JOIN_SYN msg ;
		memset(&msg, 0, sizeof(MSG_CR_JOIN_SYN)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_JOIN_ROOM_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.byRoomIdx	= (BYTE)atoi(pCode) ;

		NETWORK->Send( &msg, sizeof(MSG_CR_JOIN_SYN) ) ;

		pDlg->SetActive(FALSE) ;
	}
	// 비밀번호 입력 모드면,
	else if(inputMode == e_JoinDlg_Type_SecretCode)
	{
		// 비밀번호를 담아 참여 요청을 한다.
		MSG_CR_JOIN_SYN msg ;
		memset(&msg, 0, sizeof(MSG_CR_JOIN_SYN)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_JOIN_ROOM_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.byRoomIdx	= m_SelectRoom.byIdx ;
		SafeStrCpy(msg.code, pCode, SECRET_CODE_SIZE) ;

		NETWORK->Send( &msg, sizeof(MSG_CR_JOIN_SYN) ) ;

		pDlg->SetActive(FALSE) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Init_RoomList
//		 : 
//	DESC : 서버로 부터 넘어 온, 채팅방 정보를 세팅하는 함수.
//		 : 
//		 : 1. 채팅방을 담고 있는 Map을 비운다.
//		 : 2. 방정보를 받는다.
//		 : 3. 페이지 세팅.
//		 : 4. 로딩 텍스트 비활성화.
//		 : 5. 채팅방 리스트 다이얼로그 새로고침.
//		 : 
//  DATE : APRIL 9, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Init_RoomList(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환
	MSG_CR_ROOMLIST* pmsg = NULL ;
	pmsg = (MSG_CR_ROOMLIST*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		CHATROOMMGR->Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pRoomList) ;
	//ASSERT(m_pLoad_Chatroom) ;

	if(!m_pRoomList /*|| !m_pLoad_Chatroom*/)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 채팅방 타입 세팅.
	m_pRoomType->SetCurSelectedIdx(pmsg->byRoomType) ;
	m_pRoomType->SelectComboText(pmsg->byRoomType) ;
	m_byRoomType = pmsg->byRoomType ;

	// 채팅방 리스트 비움.
	m_Room.clear() ;

	// 채팅방 개수 만큼 루프.
	for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
	{
		if(pmsg->room[count].byIdx == 0) continue ;

		const ST_CR_ROOM_CLT& room = pmsg->room[ count ] ;

		m_Room.push_back(room) ;
	}

	SetPage(pmsg->byCurPage, pmsg->byTotalPage);
	Refresh_RoomList();
}


void CChatRoomMainDlg::Refresh_RoomList()
{
	// 하위 컨트롤 체크.
	ASSERT(m_pRoomList) ;

	if(!m_pRoomList)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	// 활성화 여부 체크.
	if(!IsActive()) return ;



	// 리스트 다이얼로그 비우기.
	m_pRoomList->RemoveAll() ;

	// 방 위치 받기.
	LIST_ROOM::iterator it = m_Room.begin();

	if(it == m_Room.end()) return;

	char tempBuf[256] = {0,};
	char tempNum[256] = {0,};
	BYTE byCount = BYTE(m_Room.size());
	BYTE byStartCount = 0 ;
	byStartCount = m_byCurPage*ROOM_COUNT_PER_PAGE ;



	// 방 개수 만큼 루프.
	for(BYTE count = 0 ; count < byCount ; ++count)
	{
		if(it == m_Room.end()) return ;

		memset(tempBuf, 0, sizeof(tempBuf)) ;
		memset(tempNum, 0, sizeof(tempNum)) ;

		// 인덱스 세팅.
		if( it->byIdx / 100 <= 0 )
		{
			if( it->byIdx / 10 <= 0 )
			{
				strcat(tempBuf, RESRCMGR->GetMsg(10)) ;
				strcat(tempBuf, RESRCMGR->GetMsg(10)) ;
			}
			else
			{
				strcat(tempBuf, RESRCMGR->GetMsg(10)) ;
			}
		}

		strcat(tempBuf, itoa(it->byIdx, tempNum, 10)) ;
		strcat(tempBuf, ".") ;

		// 공개/비공개 세팅.
		strcat(tempBuf, "[") ;
		if( it->bySecretMode == e_RM_Open )
		{
			strcat(tempBuf, RESRCMGR->GetMsg(892)) ;
		}
		else
		{
			strcat(tempBuf, RESRCMGR->GetMsg(893)) ;
		}
		strcat(tempBuf, "]") ;

		// 채팅방 분류 세팅.
		strcat(tempBuf, "[") ;
		switch(it->byRoomType)
		{
		case e_RT_Party :			strcat(tempBuf, RESRCMGR->GetMsg(880)) ;	break ;
		case e_RT_Deal :			strcat(tempBuf, RESRCMGR->GetMsg(881)) ;	break ;
		case e_RT_Normal_Chat :		strcat(tempBuf, RESRCMGR->GetMsg(882)) ;	break ;
		case e_RT_Local_Chat :		strcat(tempBuf, RESRCMGR->GetMsg(883)) ;	break ;
		case e_RT_Make_Friend :		strcat(tempBuf, RESRCMGR->GetMsg(884)) ;	break ;
		default : break ;
		}
		strcat(tempBuf, "]") ;

		// 채팅방 제목 세팅.
		strcat(tempBuf, it->title) ;

		// 현재 참여자 세팅.
		strcat(tempBuf, "(") ;

		if(it->byCurGuestCount % 10 < 0)
		{
			strcat(tempBuf, RESRCMGR->GetMsg(10)) ;
		}

		memset(tempNum, 0, sizeof(tempNum)) ;
		strcat(tempBuf, itoa(it->byCurGuestCount, tempNum, 10)) ;

		strcat(tempBuf, "/") ;

		if(it->byCurGuestCount % 10 < 0)
		{
			strcat(tempBuf, RESRCMGR->GetMsg(10)) ;
		}

		// 최대 참여자 세팅.
		memset(tempNum, 0, sizeof(tempNum)) ;

		switch(it->byTotalGuestCount)
		{
		case e_GC_20 :	strcat(tempBuf, RESRCMGR->GetMsg(922)) ;	break ;
		case e_GC_15 :	strcat(tempBuf, RESRCMGR->GetMsg(921)) ;	break ;
		case e_GC_10 :	strcat(tempBuf, RESRCMGR->GetMsg(920)) ;	break ;
		case e_GC_5 :		strcat(tempBuf, RESRCMGR->GetMsg(919)) ;	break ;
		default : break ;
		}
		strcat(tempBuf, ")") ;

		// 리스트 다이얼로그에 추가.
		m_pRoomList->AddItem( tempBuf, RGBA_MAKE(255, 255, 255, 255), count ) ;

		++it ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Clear_RoomList
//	DESC : 채팅방 리스트를 모두 비우는 함수.
//  DATE : APRIL 11, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Clear_RoomList()
{
	// 하위 컨트롤 체크.
	ASSERT(m_pRoomList) ;

	if(!m_pRoomList)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	// 채팅방 리스트 다이얼로그 비우기.
	m_pRoomList->RemoveAll() ;

	// 채팅방 리스트 비우기.
	m_Room.clear() ;

	// 선택 된 방 정보 지우기.
	memset(&m_SelectRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;
}

BOOL CChatRoomMainDlg::Add_Room(ST_CR_ROOM_CLT* pInfo)
{
	// 함수 파라메터 체크.
	ASSERT(pInfo) ;

	if(!pInfo)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return FALSE ;
	}

	const ST_CR_ROOM_CLT& addInfo = *pInfo ;
	m_Room.push_back(addInfo) ;

	Refresh_RoomList() ;

	return TRUE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Update_RoomInfo
//	DESC : 방 정보 하나를 업데이트 할 때 처리하는 함수.
//  DATE : APRIL 9, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Update_RoomInfo(ST_CR_ROOM_CLT* pInfo)
{
	// 함수 파라메터 체크.
	ASSERT(pInfo) ;

	if(!pInfo)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 방리스트를 참조할 이더레이터 받기.
	LIST_ROOM::iterator it ;
	it = m_Room.begin() ;

	// 리스트 사이즈 받기.
	if(it == m_Room.end()) return ;

	BYTE bySize = BYTE(m_Room.size());

	// 업데이트 할 방 찾기.
	for(BYTE count = 0 ; count < bySize ; ++count)
	{
		if(it->byIdx != pInfo->byIdx) continue ;

		// 방정보 업데이트.
		it->byIdx				= pInfo->byIdx ;
		it->bySecretMode		= pInfo->bySecretMode ;
		it->byRoomType			= pInfo->byRoomType ;
		it->byCurGuestCount		= pInfo->byCurGuestCount ;
		it->byTotalGuestCount	= pInfo->byTotalGuestCount ;

		SafeStrCpy(it->title, pInfo->title, TITLE_SIZE) ;

		break ;
	}

	// 채팅방 리스트 업데이트.
	Refresh_RoomList() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : DeleteRoom
//	DESC : 인자로 넘어온 방을 찾아서, 리스트에서 삭제한다.
//  DATE : APRIL 9, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::DeleteRoom(BYTE byRoomIdx)
{
	// 방리스트를 참조할 이더레이터 받기.
	LIST_ROOM::iterator it ;
	it = m_Room.begin() ;

	// 리스트 사이즈 받기.
	if(it == m_Room.end()) return ;

	BYTE bySize = BYTE(m_Room.size());

	// 업데이트 할 방 찾기.
	for(BYTE count = 0 ; count < bySize ; ++count)
	{
		if(it->byIdx != byRoomIdx) continue ;

		m_Room.erase(it) ;
		break ;
	}

	// 채팅방 리스트 업데이트.
	Refresh_RoomList() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetPage
//	DESC : 채팅방 리스트의 페이지를 세팅한다.
//  DATE : APRIL 9, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::SetPage(BYTE num1, BYTE num2)
{
	// 채팅방 리스트 페이지 세팅.
	// 080704 LYW --- CharRoomMainDlg : 임시 버퍼 사이즈 확장.
	//char tempBuff[10] = {0,} ;
	//char tempNum[10]  = {0,} ;

	char tempBuff[256] = {0,} ;
	char tempNum[256]  = {0,} ;

	sprintf(tempBuff, "%d", num1+1) ;
	strcat(tempBuff, "/") ;
	//strcat(tempBuff, itoa(num2+1, tempNum, 10)) ;
	strcat(tempBuff, itoa(num2+1, tempNum, 256)) ;

	m_pPage->SetStaticText(tempBuff) ;

	m_byCurPage = num1 ;
	m_byMaxPage = num2 ;
}




//-------------------------------------------------------------------------------------------------
//	NAME : GetRoomListItem
//	DESC : 인자로 넘어온 번호의 방을 찾아서 반환한다.
//  DATE : APRIL 9, 2008 LYW
//-------------------------------------------------------------------------------------------------

ST_CR_ROOM_CLT* CChatRoomMainDlg::GetRoomListItem(BYTE byRoomIdx)
{
	// 방리스트를 참조할 이더레이터 받기.
	LIST_ROOM::iterator it ;
	it = m_Room.begin() ;

	// 리스트 사이즈 받기.
	if(it == m_Room.end()) return NULL ;

	BYTE bySize = BYTE(m_Room.size());

	// 업데이트 할 방 찾기.
	for(BYTE count = 0 ; count < bySize ; ++count)
	{
		if(it->byIdx != byRoomIdx)
		{
			++it ;
			continue ;
		}

		ST_CR_ROOM_CLT& room = *it ;

		return &room ;
	}

	return NULL ;
}





void CChatRoomMainDlg::Err_EmptyRoom()
{
	ASSERT(m_pRoomList) ;
	//ASSERT(m_pLoad_Chatroom) ;

	if(!m_pRoomList /*|| !m_pLoad_Chatroom*/)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	if(!IsActive())
	{
		SetActive(TRUE) ;
	}

	WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1490)) ;

	m_pRoomList->RemoveAll() ;

	m_Room.clear() ;
	m_TempRoom.clear() ;

	/*if(m_pLoad_Chatroom->IsActive())
	{
		m_pLoad_Chatroom->SetActive(FALSE) ;
	}*/
}
















#include "Stdafx.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "ChatRoomGuestListDlg.h"
#include "Input/Mouse.h"
#include "Interface/cResourceManager.h"
#include "Interface/cScriptManager.h"
#include "Interface/cWindowManager.h"
#include "Interface/cButton.h"
#include "Interface/cListDialog.h"
#include "ChatRoomMgr.h"
#include "ChatManager.h"

CChatRoomGuestListDlg::CChatRoomGuestListDlg(void)
{
	// 100107 ONS 대확상대목록을 닉네임, 상태, 레벨의 세개의 리스트로 변경
	m_pGuestNicknameList		= NULL ;
	m_pGuestStateList			= NULL ;
	m_pGuestLevelList			= NULL ;

	m_pBtn_ChangeOwner	= NULL ;
	m_pBtn_FreezeGuest	= NULL ;
	m_pBtn_KickGuest	= NULL ;
	m_pBtn_AddFriend	= NULL ;

	//// 클래스 마크 초기화.
	//for(BYTE count = 0 ; count < MAX_ROW_GUESTLIST ; ++count)
	//{
	//	ZeroMemory(&m_MarkImg[count], sizeof(cImage)) ;
	//}

	// 선택 된 유저 정보 초기화.
	memset(&m_SelectedUser, 0, sizeof(ST_CR_USER)) ;

	// 내 정보 초기화.
	//memset(&m_MyUserInfo, 0, sizeof(ST_CR_USER)) ;

	m_plGuest = NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomGuestListDlg
//	DESC : 소멸자 함수.
//  DATE : APRIL 12, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomGuestListDlg::~CChatRoomGuestListDlg(void)
{
	//ST_CR_USER* pInfo ;
	//PTRLISTPOS pos = NULL ;
	//pos = m_plGuest->GetHeadPosition() ;

	//BYTE byCount = 0 ;
	//while(pos)
	//{
	//	pInfo = NULL ;
	//	pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

	//	if(!pInfo) continue ;

	//	m_plGuest->Remove(pInfo) ;

	//	delete pInfo ;
	//}

	//m_plGuest->RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Linking
//	DESC : 하위 컨트롤들을 링크하는 함수.
//  DATE : APRIL 12, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::Linking()
{
	// 하위 컨트롤 링크.
	// 100107 ONS 대확상대목록을 닉네임, 상태, 레벨의 세개의 리스트로 변경
	m_pGuestNicknameList= (cListDialog*)GetWindowForID(CRGLD_LST_GUESTNICKNAME) ;
	m_pGuestStateList	= (cListDialog*)GetWindowForID(CRGLD_LST_GUESTSTATE) ;
	m_pGuestLevelList	= (cListDialog*)GetWindowForID(CRGLD_LST_GUESTLEVEL) ;

	m_pBtn_ChangeOwner	= (cButton*)GetWindowForID(CRGLD_BTN_CHANGEOWNER) ;
	m_pBtn_FreezeGuest	= (cButton*)GetWindowForID(CRGLD_BTN_FREEZEGUEST) ;
	m_pBtn_KickGuest	= (cButton*)GetWindowForID(CRGLD_BTN_KICKGUEST) ;
	m_pBtn_AddFriend	= (cButton*)GetWindowForID(CRGLD_BTN_ADDFRIEND) ;



	// 링크 된 컨트롤 유효성 체크.
	ASSERT(m_pGuestNicknameList) ;
	ASSERT(m_pGuestStateList) ;
	ASSERT(m_pGuestLevelList) ;

	ASSERT(m_pBtn_ChangeOwner) ;
	ASSERT(m_pBtn_FreezeGuest) ;
	ASSERT(m_pBtn_KickGuest) ;
	ASSERT(m_pBtn_AddFriend) ;

	if( !m_pGuestNicknameList	|| 
		!m_pGuestStateList		|| 
		!m_pGuestLevelList		||
		!m_pBtn_ChangeOwner		|| 
		!m_pBtn_FreezeGuest		|| 
		!m_pBtn_KickGuest		||
		!m_pBtn_AddFriend)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 채팅방 매니져에 등록.
	CHATROOMMGR->SetGuestListDlg(this) ;



	// 채팅방 매니져가 관리하고 있는 참여자 리스트를 받는다.
	m_plGuest = CHATROOMMGR->GetGuestList() ;
}





////-------------------------------------------------------------------------------------------------
////	NAME : Render
////	DESC : 유저 리스트 컨트롤에 클래스 마크를 출력한다.
////  DATE : APRIL 12, 2008 LYW
////-------------------------------------------------------------------------------------------------
//void CChatRoomGuestListDlg::Render()
//{
//	// 활성화 상태 체크.
//	if( !IsActive() ) return ;
//
//	// 하위 컨트롤 체크.
//	ASSERT(m_pGuestList) ;
//
//	if(!m_pGuestList)
//	{
//		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
//		SetActive(FALSE) ;
//		return ;
//	}
//
//	// 다이얼로그/하위 컨트롤 렌더.
//	cDialog::RenderWindow() ;
//	cDialog::RenderComponent() ;
//
//	// 이미지 렉트 선언.
//	cImageRect* pRect ;
//
//	// 스케일 세팅.
//	VECTOR2 vScale ;
//	memset(&vScale, 0, sizeof(VECTOR2)) ;
//
//	vScale.x = 1.0f ;
//	vScale.y = 1.0f ;
//
//	// 위치 세팅.
//	VECTOR2 vPos ;
//	memset(&vPos, 0, sizeof(VECTOR2)) ;
//
//	vPos.x = m_pGuestList->GetAbsX() ;
//	vPos.y = m_pGuestList->GetAbsY() ;//+ m_pGuestList->GetHeadLineHeight() + 4 ;
//
//	// 출력 라인 개수 받기.
//	int nRows = 0 ;
//	//nRows = m_pGuestList->GetRowsNum() ;
//	nRows = m_pGuestList->GetMinMiddleNum() ;
//
//	// 출력 가능한 라인 수 만큼 for문.
//	for(int count = 0 ; count < nRows ; ++count)
//	{
//		// 이미지 유효성 체크.
//		if(m_MarkImg[count].IsNull()) continue ;
//
//		// 이미지 렌더.
//		m_MarkImg[count].RenderSprite(&vScale, 0, 0, &vPos, 0xffffffff) ;
//
//		// 이미지 렉트 세팅.
//		pRect = NULL ;
//		pRect = m_MarkImg[count].GetImageRect() ;
//
//		ASSERT(pRect) ;
//
//		if(!pRect)
//		{
//			CHATROOMMGR->Throw_Error("Invalid rect!!", __FUNCTION__) ;
//			SetActive(FALSE) ;
//			return ;
//		}
//
//		vPos.y += ((pRect->bottom-pRect->top)+6) ;
//	}
//}





//-------------------------------------------------------------------------------------------------
//	NAME : ActionEvent
//	DESC : 마우스 이벤트를 처리하는 함수.
//  DATE : APRIL 12, 2008 LYW
//-------------------------------------------------------------------------------------------------
DWORD CChatRoomGuestListDlg::ActionEvent( CMouse* mouseInfo ) 
{
	// 이벤트 변수 선언.
	DWORD we = WE_NULL ;



	// 하위 컨트롤 체크.
	ASSERT(m_pGuestNicknameList) ;
	ASSERT(m_pGuestStateList) ;
	ASSERT(m_pGuestLevelList) ;

	if(!m_pGuestNicknameList || !m_pGuestStateList || !m_pGuestLevelList)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		SetActive(FALSE) ;
		return we ;
	}



	// 마우스 정보 체크.
	if( !mouseInfo ) return we ;



	// 다이얼로그 이벤트를 받는다.
	we = cDialog::ActionEvent( mouseInfo ) ;

	// 유저 리스트 상에 마우스가 있으면,
	if(m_pGuestNicknameList->PtInWindow( (LONG)mouseInfo->GetMouseX(), (LONG)mouseInfo->GetMouseY() ))
	{
		// 참여자 리스트 체크.
		ASSERT(m_plGuest) ;

		if(!m_plGuest)
		{
			CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
			return we ;
		}
		else if( we & WE_LBTNCLICK )
		{
			// 100107 ONS 대확상대목록을 닉네임, 상태, 레벨의 세개의 리스트로 변경
			BYTE byCount = (BYTE)m_pGuestNicknameList->GetItemCount();
			BYTE bySelectRow = BYTE(m_pGuestNicknameList->GetSelectRowIdx() + m_pGuestNicknameList->GetTopListItemIdx());
			DWORD dwColor = RGBA_MAKE(255, 255, 255, 255);

			for(BYTE count = 0 ; count < byCount ; ++count)
			{
				ITEM* pNickItem = NULL ;
				ITEM* pStateItem = NULL;
				ITEM* pLevelItem = NULL;
					
				pNickItem	= (ITEM*)m_pGuestNicknameList->GetItem((LONG)count) ;
				pStateItem	= (ITEM*)m_pGuestStateList->GetItem((LONG)count) ;
				pLevelItem	= (ITEM*)m_pGuestLevelList->GetItem((LONG)count) ;

				if(!pNickItem || !pStateItem || !pLevelItem) continue ;

				// 선택한 아이템이면, 
				if(count == bySelectRow)
				{
					// 선택한 색상으로 업데이트.
					dwColor = RGBA_MAKE(255, 0, 0, 255) ;
				}
				// 기본 색상으로 세팅.
				else
				{
					dwColor = RGBA_MAKE(255, 255, 255, 255) ;
				}
				
				pNickItem->rgb  = dwColor;
				pStateItem->rgb = dwColor;
				pLevelItem->rgb = dwColor;
			}

			ST_CR_USER* pInfo ;
			PTRLISTPOS pos = NULL ;
			pos = m_plGuest->GetHeadPosition() ;

			BYTE byCheckCount = 0 ;
			while(pos)
			{
				pInfo = NULL ;
				pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

				if(!pInfo) continue ;

				if(byCheckCount == bySelectRow)
				{
					memcpy(&m_SelectedUser, pInfo, sizeof(ST_CR_USER)) ;
					break ;
				}
				++byCheckCount ;
			}
		}
	}

	return we ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : OnActionEvent
//	DESC : EventFunc로 부터 넘어온 Event를 처리하는 함수.
//  DATE : APRIL 12, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::OnActionEvent(LONG id, void* p, DWORD event)
{
	// 함수 파라메터 체크.
	ASSERT(p) ;	

	if(!p)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// 하위 컨트롤 체크.
	ASSERT(m_pBtn_ChangeOwner) ;
	ASSERT(m_pBtn_FreezeGuest) ;
	ASSERT(m_pBtn_KickGuest) ;
	ASSERT(m_pBtn_AddFriend) ;

	if( !m_pGuestNicknameList	||
		!m_pGuestStateList		|| 
		!m_pGuestLevelList		|| 
		!m_pBtn_ChangeOwner		|| 
		!m_pBtn_FreezeGuest		|| 
		!m_pBtn_KickGuest		|| 
		!m_pBtn_AddFriend)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 컨트롤 아이디 체크.
	switch(id)
	{
	case CRGLD_BTN_CHANGEOWNER : ChangeOwner() ; break ;
	case CRGLD_BTN_FREEZEGUEST : FreezeGuest() ; break ;
	case CRGLD_BTN_KICKGUEST :	 KickGuest() ;	 break ;
	case CRGLD_BTN_ADDFRIEND :	 AddFriend() ;	 break ;
	default : break ;
	}



	//참여자 정보를 리스트 다이얼로그에 세팅.
	SetClassMark() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : InitGuestList
//	DESC : 참여자 리스트를 세팅하는 함수.
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::InitGuestList(ST_CR_USER* pUser, BYTE byCount)
{
	// 함수 파라메터 체크.
	ASSERT(pUser) ;

	if(!pUser)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// 참여자 리스트 체크.
	ASSERT(m_plGuest) ;

	if(!m_plGuest)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 유저 관리 리스트 비움.
	ST_CR_USER* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_plGuest->GetHeadPosition() ;

	while(pos)
	{
		pInfo = NULL ;
		pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

		if(!pInfo) continue ;

		m_plGuest->Remove(pInfo) ;

		delete pInfo ;
	}
	m_plGuest->RemoveAll() ;

	// 리스트 다이얼로그 비우기.
	//m_pGuestList->DeleteAllItems() ;
	m_pGuestNicknameList->RemoveAll() ;
	m_pGuestStateList->RemoveAll() ;
	m_pGuestLevelList->RemoveAll() ;




	// 참여자 리스트에 유저 정보 등록.
	for(BYTE count = 0 ; count < byCount ; ++count)
	{
		if(pUser[count].dwPlayerID == 0) continue ;

		//if(HEROID == pUser[count].dwPlayerID)
		//{
		//	m_MyUserInfo = pUser[count] ;
		//}

		ST_CR_USER* pGuest = NULL ;
		pGuest = new ST_CR_USER ;

		ASSERT(pGuest) ;

		if(!pGuest)
		{
			CHATROOMMGR->Throw_Error("Failed to create user!!", __FUNCTION__) ;
			return ;
		}

		memcpy(pGuest, &pUser[count], sizeof(ST_CR_USER)) ;
		m_plGuest->AddTail(pGuest) ;
	}



	// 참여자 정보를 리스트 다이얼로그에 세팅
	SetClassMark() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : AddGuest
//	DESC : 참여자 추가 처리를 하는 함수.
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
BOOL CChatRoomGuestListDlg::AddGuest(ST_CR_USER* pUser)
{
	// 함수 파라메터 체크.
	ASSERT(pUser) ;

	if(!pUser)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return FALSE ;
	}



	// 참여자 리스트 체크.
	ASSERT(m_plGuest) ;

	if(!m_plGuest)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return FALSE ;
	}



	// 유저 정보 생성.
	ST_CR_USER* pGuest = NULL ;
	pGuest = new ST_CR_USER ;

	// 유저 정보 체크.
	ASSERT(pGuest) ;

	if(!pGuest)
	{
		CHATROOMMGR->Throw_Error("Failed to create user!!", __FUNCTION__) ;
		return FALSE ;
	}

	// 유저 정보 등록.
	memcpy(pGuest, pUser, sizeof(ST_CR_USER)) ;
	m_plGuest->AddTail(pGuest) ;



	// 참여자 정보를 리스트 다이얼로그에 세팅.
	SetClassMark() ;

	return TRUE ;
}




//-------------------------------------------------------------------------------------------------
//	NAME : DeleteGuest
//	DESC : 참여자 삭제 처리를 하는 함수.
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::DeleteGuest(DWORD dwPlayerID)
{
	// 참여자 리스트 체크.
	ASSERT(m_plGuest) ;

	if(!m_plGuest)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 선택 된 대상 삭제 처리.
	ST_CR_USER* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_plGuest->GetHeadPosition() ;

	while(pos)
	{
		pInfo = NULL ;
		pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

		if(!pInfo) continue ;

		if(pInfo->dwPlayerID != dwPlayerID) continue ;

		// 100427 ONS 선택한 유저정보를 초기화한다.
		if( m_SelectedUser.dwPlayerID == pInfo->dwPlayerID )
		{
			ZeroMemory(&m_SelectedUser, sizeof(ST_CR_USER)) ;
		}

		m_plGuest->Remove(pInfo) ;

		delete pInfo ;

		break ;
	}



	// 참여자 정보를 리스트 다이얼로그에 세팅
	SetClassMark() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetClassMark
//	DESC : 참여자 정보를 리스트 다이얼로그에 세팅하는 함수.
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::SetClassMark()
{
	// 하위 컨트롤 체크.
	ASSERT(m_pGuestNicknameList) ;
	ASSERT(m_pGuestStateList) ;
	ASSERT(m_pGuestLevelList) ;
	
	if( !m_pGuestNicknameList || !m_pGuestStateList || !m_pGuestLevelList )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		SetActive(FALSE) ;
		return ;
	}



	// 참여자 리스트 체크.
	ASSERT(m_plGuest) ;

	if(!m_plGuest)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 유저 아이템 수 받기.
	int nMaxRow = 0 ;
	nMaxRow = m_plGuest->GetCount() ;

	// 최상위 인덱스 받기.
	LONG lTopIdx = 0 ;
	//lTopIdx = m_pGuestList->GetStartIdxOfPrintedItems() ;
	lTopIdx = m_pGuestNicknameList->GetTopListItemIdx() ;

	// 인덱스 유효 체크.
	ASSERT(lTopIdx >= 0) ;

	if(lTopIdx < 0)
	{
		CHATROOMMGR->Throw_Error("Invalid Index!!", __FUNCTION__) ;
		SetActive(FALSE) ;
		return ;
	}

	//// 클래스 마크 초기화.
	//for(BYTE count = 0 ; count < MAX_ROW_GUESTLIST ; ++count)
	//{
	//	ZeroMemory(&m_MarkImg[count], sizeof(cImage)) ;
	//}

	//m_pGuestList->DeleteAllItems() ;
	m_pGuestNicknameList->RemoveAll() ;
	m_pGuestStateList->RemoveAll() ;
	m_pGuestLevelList->RemoveAll() ;


	//char Level[4] ;
	//char State[5] ;

	// 080704 LYW --- ChatRoomGuestListDlg : 임시 버퍼 사이즈 확장.
	//char tempBuff[128] = {0,} ;
	//char szState[24] = {0, } ;

	char tempBuff[256] = {0,} ;
	char szState[256] = {0, } ;

	DWORD dwColor = 0 ;
	dwColor = RGBA_MAKE(255, 255, 255, 255) ;

	ST_CR_USER* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_plGuest->GetHeadPosition() ;

	BYTE byCount = 0 ;
	while(pos)
	{
		pInfo = NULL ;
		pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

		if(!pInfo) continue ;

		memset(&tempBuff, 0, sizeof(tempBuff)) ;
		memset(&szState, 0, sizeof(szState)) ;

		// 상태 세팅.
		if(CHATROOMMGR->GetMyRoomOwnerIdx() == pInfo->dwPlayerID)
		{
			if(CHATROOMMGR->IsFreezed(pInfo->dwPlayerID))
			{
				strcat(szState, CHATMGR->GetChatMsg(1484)) ;
				strcat(szState, "/") ;
				strcat(szState, CHATMGR->GetChatMsg(1485)) ;
			}
			else
			{
				SafeStrCpy(szState, CHATMGR->GetChatMsg(1484), 5) ;
			}
		}
		else if(CHATROOMMGR->IsFreezed(pInfo->dwPlayerID))
		{
			SafeStrCpy(szState, CHATMGR->GetChatMsg(1485), 5) ;
		}
		else
		{
			SafeStrCpy(szState, CHATMGR->GetChatMsg(1486), 5) ;
		}

		// 100107 ONS 대확상대목록을 닉네임, 상태, 레벨의 세개의 리스트로 변경
		// 이름 세팅.
		sprintf(tempBuff, "%-28s", pInfo->name) ;
		m_pGuestNicknameList->AddItem(tempBuff, dwColor, byCount) ;	

		// 상태 세팅
		memset(&tempBuff, 0, sizeof(tempBuff)) ;
		sprintf(tempBuff, "%-8s", szState) ;
		m_pGuestStateList->AddItem(tempBuff, dwColor, byCount) ;	

		// 레벨 세팅
		memset(&tempBuff, 0, sizeof(tempBuff)) ;
		sprintf(tempBuff, "%5d", pInfo->byLevel) ;
		m_pGuestLevelList->AddItem(tempBuff, dwColor, byCount) ;	

		// 100427 ONS 대화상대목록 리스트가 갱신될때 선택된 상대의 색상을 갱신한다.
		ITEM* pNickItem		= (ITEM*)m_pGuestNicknameList->GetItem((LONG)byCount);
		ITEM* pStateItem	= (ITEM*)m_pGuestStateList->GetItem((LONG)byCount);
		ITEM* pLevelItem	= (ITEM*)m_pGuestLevelList->GetItem((LONG)byCount);

		if(!pNickItem || !pStateItem || !pLevelItem)
		{
			++byCount;
			continue;
		}

		if( m_SelectedUser.dwPlayerID == pInfo->dwPlayerID )
		{
			dwColor = RGBA_MAKE(255, 0, 0, 255) ;
		}
		else
		{
			dwColor = RGBA_MAKE(255, 255, 255, 255) ;
		}

		pNickItem->rgb  = dwColor;
		pStateItem->rgb = dwColor;
		pLevelItem->rgb = dwColor;

		++byCount ;

	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetDlgToDefault
//	DESC : 참여자 리스트를 기본 상태로 세팅하는 함수.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::SetDlgToDefault()
{
	// 참여자 리스트 체크.
	ASSERT(m_plGuest) ;

	if(!m_plGuest)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 리스트 컨트롤 비우기.
	//m_pGuestList->DeleteAllItems() ;
	m_pGuestNicknameList->RemoveAll() ;
	m_pGuestStateList->RemoveAll() ;
	m_pGuestLevelList->RemoveAll() ;

	// 리스트 비우기.
	ST_CR_USER* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_plGuest->GetHeadPosition() ;

	while(pos)
	{
		pInfo = NULL ;
		pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

		if(!pInfo) continue ;

		m_plGuest->Remove(pInfo) ;

		delete pInfo ;
	}
	m_plGuest->RemoveAll() ;

	//// 클래스 마크 초기화.
	//for(BYTE count = 0 ; count < MAX_ROW_GUESTLIST ; ++count)
	//{
	//	ZeroMemory(&m_MarkImg[count], sizeof(cImage)) ;
	//}



	// 선택 된 유저 정보 초기화.
	memset(&m_SelectedUser, 0, sizeof(ST_CR_USER)) ;

	// 내 정보 초기화.
	//memset(&m_MyUserInfo, 0, sizeof(ST_CR_USER)) ;



	// 참여자 정보를 리스트 다이얼로그에 세팅
	SetClassMark() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ChangeOwner
//	DESC : 방장위임 버튼이 눌렸을 때 처리를 하는 함수.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::ChangeOwner()
{
	// 방장인지 체크.
	if(HEROID != CHATROOMMGR->GetMyRoomOwnerIdx())
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1487), e_ChatMsg_System) ;
		return ;
	}

	// 대상의 아이디를 체크.
	if(m_SelectedUser.dwPlayerID == HEROID)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1488), e_ChatMsg_System) ;
		return ;
	}

	// 대상의 유효성 체크.
	if(m_SelectedUser.dwPlayerID == 0)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1489), e_ChatMsg_System) ;
		return ;
	}

	// 재사용 시간 체크.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}



	// 방장 위임 요청.
	MSG_DWORD msg ;
	memset(&msg, 0, sizeof(MSG_DWORD)) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_CHANGE_OWNER_SYN ;
	msg.dwObjectID	= HEROID ;
	
	msg.dwData		= m_SelectedUser.dwPlayerID ;

	NETWORK->Send( &msg, sizeof(MSG_DWORD) ) ;



	// 시간 업데이트.
	CHATROOMMGR->UpdateMyLastComTime() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : FreezeGuest
//	DESC : 차단 버튼이 눌렸을 때 처리를 하는 함수.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::FreezeGuest()
{
	// 대상의 아이디를 체크.
	if(m_SelectedUser.dwPlayerID == HEROID)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1488), e_ChatMsg_System) ;
		return ;
	}

	// 대상의 유효성 체크.
	if(m_SelectedUser.dwPlayerID == 0)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1489), e_ChatMsg_System) ;
		return ;
	}

	// 대상의 상태를 변경한다.
	if(CHATROOMMGR->IsFreezed(m_SelectedUser.dwPlayerID))
	{
		CHATROOMMGR->UnFreezeUser(m_SelectedUser.dwPlayerID) ;
	}
	else if(!CHATROOMMGR->IsFreezed(m_SelectedUser.dwPlayerID))
	{
		CHATROOMMGR->FreezeUser(m_SelectedUser.dwPlayerID) ;
	}
	else return ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : KickGuest
//	DESC : 강제퇴장 버튼이 눌렸을 때 처리를 하는 함수.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::KickGuest()
{
	// 방장인지 체크.
	if(HEROID != CHATROOMMGR->GetMyRoomOwnerIdx())
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1487), e_ChatMsg_System) ;
		return ;
	}

	// 대상의 아이디를 체크.
	if(m_SelectedUser.dwPlayerID == HEROID)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1488), e_ChatMsg_System) ;
		return ;
	}

	// 대상의 유효성 체크.
	if(m_SelectedUser.dwPlayerID == 0)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1489), e_ChatMsg_System) ;
		return ;
	}

	// 재사용 시간 체크.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}



	// 방장 위임 요청.
	MSG_DWORD msg ;
	memset(&msg, 0, sizeof(MSG_DWORD)) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_KICK_GUEST_SYN ;
	msg.dwObjectID	= HEROID ;
	
	msg.dwData		= m_SelectedUser.dwPlayerID ;

	NETWORK->Send( &msg, sizeof(MSG_DWORD) ) ;



	// 시간 업데이트.
	CHATROOMMGR->UpdateMyLastComTime() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : AddFriend
//	DESC : 친구추가 버튼이 눌렸을 때 처리를 하는 함수.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::AddFriend()
{
	// 대상의 아이디를 체크.
	if(m_SelectedUser.dwPlayerID == HEROID)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1488), e_ChatMsg_System) ;
		return ;
	}

	// 대상의 유효성 체크.
	if(m_SelectedUser.dwPlayerID == 0)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1489), e_ChatMsg_System) ;
		return ;
	}

	// 재사용 시간 체크.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}



	// 방장 위임 요청.
	MSG_DWORD msg ;
	memset(&msg, 0, sizeof(MSG_DWORD)) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_REQUEST_FRIEND_SYN ;
	msg.dwObjectID	= HEROID ;
	
	msg.dwData		= m_SelectedUser.dwPlayerID ;

	NETWORK->Send( &msg, sizeof(MSG_DWORD) ) ;

	CHATROOMMGR->SetRequestFriendState(TRUE) ;



	// 시간 업데이트.
	CHATROOMMGR->UpdateMyLastComTime() ;
}

// FriendDialog.cpp: implementation of the CFriendDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FriendDialog.h"
#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"
#include "FriendManager.h"
#include "Gamein.h"
// 061220 LYW --- Delete this code.
//#include "MainBarDialog.h"
#include "MininoteDialog.h"
#include "MininoteDialogRead.h"
#include "cRitemEx.h"

// 071022 LYW --- FriendDialog : Include header file of tutorial manager.
#include "TutorialManager.h"

//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
#include "../Interface/cResourceManager.h"
#include "./Interface/cScriptManager.h"
#include "../Input/Mouse.h"
#include "ChatManager.h"
#include "PopupMenuDlg.h"
#include "Object.h"
#include "ObjectManager.h"
#include "PCRoomManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFriendDialog::CFriendDialog()
{
	m_type = WT_FRIENDDLG;
	m_SelectedFriendPge = 1;
	m_LogInGroup = 0;
	m_LogOutGroup = 0;
	m_pPCRoomRegisted = NULL;
	m_pPCRoomBenefitLevel = NULL;

//	strcpy(m_SelectedFriendName, "");
}

CFriendDialog::~CFriendDialog()
{

}

//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가( Tab 다이얼로그로 변경 )
void CFriendDialog::Add( cWindow* window )
{
	WORD wWindowType = window->GetType() ;																		// 윈도우 타입을 받아온다.

	if( wWindowType == WT_PUSHUPBUTTON )																		// 윈도우 타입이 푸쉬업 버튼이면,
	{
		AddTabBtn( curIdx1++, (cPushupButton*)window ) ;														// 현재 인덱스1로 탭 버튼을 추가한다.
	}
	else if( wWindowType == WT_DIALOG )																			// 윈도우 타입이 다이얼로그이면,
	{
		AddTabSheet( curIdx2++, window ) ;																		// 현재 인덱스2로 쉬트를 추가한다.
	}
	else																										// 이도 저도 아니면,
	{
		cTabDialog::Add( window ) ;																				// 윈도우를 추가한다.
	}
}

void CFriendDialog::Linking()
{
	//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
	m_pFriendTabBtn = (cPushupButton*)GetWindowForID( FRI_TAB_FRIEND );
	m_pPCRoomTabBtn = (cPushupButton*)GetWindowForID( FRI_TAB_PCROOM );
	m_pFriendTabBtn->SetActive(FALSE);
	m_pPCRoomTabBtn->SetActive(FALSE);

	m_pFriendListLCtl = (cListCtrl*)((cTabDialog*)GetTabSheet(0))->GetWindowForID(FRI_FRIENDLISTLCTL);
	for(int i=0; i<MAX_FRIEND_PAGE; ++i)
	{
		m_FriendPageBtn[i] = (cPushupButton*)((cTabDialog*)GetTabSheet(0))->GetWindowForID(FRI_PAGE1BTN+i);
	}
	m_AddFriendBtn = (cButton*)((cTabDialog*)GetTabSheet(0))->GetWindowForID(FRI_ADDFRIENDBTN);
	m_DelFriendBtn = (cButton*)((cTabDialog*)GetTabSheet(0))->GetWindowForID(FRI_DELFRIENDBTN);
	for(i=0; i<MAX_FRIEND_LIST; ++i)
	{
		m_pFriendChkBox[i] = (cCheckBox*)((cTabDialog*)GetTabSheet(0))->GetWindowForID(FRI_FRIENDLISTCHK1+i);
	}
	
	m_SendNoteBtn = (cButton*)GetWindowForID(FRI_SENDNOTEBTN);
	m_SendWhisperBtn = (cButton*)((cTabDialog*)GetTabSheet(0))->GetWindowForID(FRI_SENDWHISPERBTN);

	//090623 pdy 하우징 기능추가 (친구목록 링크 하우스입장) 
	for(i=0; i<MAX_FRIEND_LIST; ++i)
	{
		m_LinkHouseBtnArr[i] = (cButton*)((cTabDialog*)GetTabSheet(0))->GetWindowForID(FRI_FRIENDLIHOUSE1+i);
	}

	// 100118 ONS PC방관련 기능 추가()
	m_pPCRoomRegisted = (cStatic*)((cTabDialog*)GetTabSheet(1))->GetWindowForID(FRI_PCROOM_REGISTED);
    m_pPCRoomBenefitLevel = (cStatic*)((cTabDialog*)GetTabSheet(1))->GetWindowForID(FRI_PCROOM_BENEFIT_LEVEL);
	m_pPCRoomListCtrl = (cListCtrl*)((cTabDialog*)GetTabSheet(1))->GetWindowForID(FRI_PCROOM_MEMLISTCTRL);
	for(i=0; i<MAX_PCROOM_MEMBER_NUM/MAX_PCROOM_MEMBER_LIST; ++i)
	{
		m_PCRoomPageBtn[i] = (cPushupButton*)((cTabDialog*)GetTabSheet(1))->GetWindowForID(FRI_PCROOM_PAGE1BTN+i);
	}
	
// 100315 ONS PC방정보를 초기화한다.
	SetPCRoomInfo( FALSE );
}

void CFriendDialog::SetActive( BOOL val )
{
	cTabDialog::SetActive( val );

//--- main bar icon
	// 061220 LYW --- Delete this code.
	/*
	CMainBarDialog* pDlg = GAMEIN->GetMainInterfaceDialog();
	if( pDlg )
		pDlg->SetPushBarIcon( OPT_FRIENDDLGICON, m_bActive );
		*/

	// 071022 LYW --- FriendDialog : Check open dialog.
	TUTORIALMGR->Check_OpenDialog(this->GetID(), val) ;
}

DWORD CFriendDialog::ActionEvent(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;
	we = cTabDialog::ActionEvent(mouseInfo);
	
	if( (we & WE_ROWCLICK) && 
		(m_pFriendTabBtn->IsActive()) )
	{
		cRITEMEx * pRItem = (cRITEMEx *)m_pFriendListLCtl->GetRItem(m_pFriendListLCtl->GetSelectedRowIdx());
		if(pRItem != NULL)
		{
			if(pRItem->dwID) //자동 쪽지 기능 추가.
			{
				CMiniNoteDialogWrite* dialog = GAMEIN->GetMiniNoteDialogWrite();
				ASSERT( dialog );

				dialog->SetActive( TRUE );
				dialog->SetMiniNote(pRItem->pString[0], "", 0);
			}
		}
	}
	else if( (we & WE_RBTNCLICK) && 
			 (m_pPCRoomTabBtn->IsActive()) )
	{
		//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
		LONG x = mouseInfo->GetMouseEventX();
		LONG y = mouseInfo->GetMouseEventY();
		int selIdx = m_pPCRoomListCtrl->PtIdxInRow(x, y);

		cRITEMEx * pRItem = (cRITEMEx *)m_pPCRoomListCtrl->GetRItem( selIdx );
		if(pRItem != NULL)
		{
			// 100217 ONS 자신을 선택했을때 팝업메뉴를 띄우지 않도록 수정.
			if(pRItem->dwID && pRItem->dwID != HEROID) 
			{
				GAMEIN->GetPopupMenuDlg()->SetAbsXY( x-1, y-1 );
				GAMEIN->GetPopupMenuDlg()->SetActivePCRoomDlg( pRItem->dwID, pRItem->pString[0], TRUE );
			}
		}
	}
	return we;
}
/*
void CFriendDialog::SetFriendList(MSG_FRIEND_LIST_DLG* pmsg)
{
	m_LogInGroup = 0;
	m_LogOutGroup = 0;

	ShowFriendPageBtn(pmsg->TotalPage);

	m_pFriendListLCtl->DeleteAllItems();
	
	for(int i=0; i<MAX_FRIEND_LIST; ++i)
	{
		m_pFriendChkBox[i]->SetChecked(0);
		m_pFriendChkBox[i]->SetActive(TRUE);
		
		if(pmsg->FriendList[i].Id != 0)
		{
			cRITEMEx *ritem = new cRITEMEx(1);
			strcpy(ritem->pString[0], pmsg->FriendList[i].Name);
			if(pmsg->FriendList[i].IsLoggIn)
			{
				ritem->rgb[0] = RGBA_MAKE(157,215,206,255);
			}
			else
			{
				ritem->rgb[0] = RGBA_MAKE(180,180,180,255);
			}
			ritem->dwID = pmsg->FriendList[i].Id;
			m_pFriendListLCtl->InsertItem(i, ritem);
		}
		else
			m_pFriendChkBox[i]->SetActive(FALSE);
	}
}
*/

//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가(클래스 아이콘 출력)
void CFriendDialog::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();

	if(m_pPCRoomTabBtn->IsActive())
	{
		VECTOR2 scale = {0,};																// 이미지 스케일을 적용할 벡터를 선언한다.
		scale.x = 1.0f;																// 스케일 X를 1로 세팅한다.
		scale.y = 1.0f;																// 스케일 Y를 1로 세팅한다.

		VECTOR2 vPos = {0,};																// 이미지 위치를 적용할 벡터를 선언한다.
		DWORD dwColor = 0xffffffff ;												// 이미지 출력에 적용할 색상을 선언한다.
		for( int nRow = 0 ; nRow < MAX_PCROOM_MEMBER_LIST ; ++nRow )		
		{
			vPos.y = (float)(GetAbsY() + 122) + (20*nRow) ;							// 이미지 출력위치 Y를 세팅한다.

			vPos.x = (float)(GetAbsX() + 30) ;										// 이미지 출력위치 X를 세팅한다.

			if( !m_pClassImg[nRow].IsNull() )										// 멤버 클래스 이미지가 널이 아니라면,
			{
				m_pClassImg[nRow].RenderSprite(&scale, NULL, 0, &vPos, dwColor) ;	// 멤버 클래스 이미지를 출력한다.
			}
		}
	}
}

//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가( PC방 가맹점여부/혜택정보 설정 )
void CFriendDialog::SetPCRoomInfo( BOOL bIsPCRoom, WORD PartyStage, WORD MemberMin )
{
	// 가맹 PC방인지여부
	if( bIsPCRoom )
	{
		m_pPCRoomRegisted->SetStaticText( RESRCMGR->GetMsg(1222) );
	}
	else
	{
		m_pPCRoomRegisted->SetStaticText( RESRCMGR->GetMsg(1223) );
	}

	if(PartyStage > 0)
	{
		// PC방혜택
		char buf[64] = {0,};
		sprintf(buf,  RESRCMGR->GetMsg(1224), PartyStage, MemberMin );
		m_pPCRoomBenefitLevel->SetStaticText(buf);
	}
}

//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
void CFriendDialog::SetPCRoomMemberList( int gotopage )
{
	PCROOM_MEMBER_LIST MemberInfo;
	memset(&MemberInfo, 0, sizeof(PCROOM_MEMBER_LIST));
	PCROOMMGR->GetMemberInfoList( &MemberInfo, gotopage );
	
	ShowPCRoomPageBtn(MemberInfo.totalnum);
	ZeroMemory( &m_pClassImg, sizeof(m_pClassImg) ) ;
	m_pPCRoomListCtrl->DeleteAllItems();
	for(int i=0; i<MAX_PCROOM_MEMBER_LIST; ++i)
	{
		if( MemberInfo.Member[i].CharIndex != 0 )
		{
 			int nIconNum = RESRCMGR->GetClassIconNum( MemberInfo.Member[i].CharClass ) ;
 			if( nIconNum >= 0 && RESRCMGR->GetClassIconCount() >= nIconNum )
			{
				SCRIPTMGR->GetImage( nIconNum, &m_pClassImg[i], PFT_CLASS_ICON_PATH ) ;	// 클래스 아이콘 이미지를 할당받는다.
			}

			cRITEMEx *ritem = new cRITEMEx(2);
			strcpy(ritem->pString[0], MemberInfo.Member[i].Name);
			strcpy(ritem->pString[1], RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum( MemberInfo.Member[i].CharClass )));	
	
			// 100312 ONS 리스트에서 자신의 색상은 붉은색을 설정.
			if( HEROID == MemberInfo.Member[i].CharIndex )
			{
				const DWORD loginColor( RGBA_MAKE( 255, 0, 0, 0 ) );
				ritem->rgb[0] = loginColor;
				ritem->rgb[1] = loginColor;
			}
	

			ritem->dwID = MemberInfo.Member[i].CharIndex;
			m_pPCRoomListCtrl->InsertItem(i, ritem);
		}
	}

	// 080121 LYW --- FriendDialog : 선택 된 탭 버튼만 푸쉬 상태로 세팅.
	int nCurPage = gotopage-1 ;

	for( int count = 0 ; count < MAX_PCROOM_MEMBER_NUM/MAX_PCROOM_MEMBER_LIST ; ++count )
	{
		if( !m_PCRoomPageBtn[count] ) continue ;

		if( nCurPage == count )
		{
			m_PCRoomPageBtn[count]->SetPush(TRUE) ;
		}
		else
		{
			m_PCRoomPageBtn[count]->SetPush(FALSE) ;
		}
	}
}

void CFriendDialog::SetFriendList(int gotopage)
{
	m_LogInGroup = 0;
	m_LogOutGroup = 0;
	
	FRIEND_LIST FriendInfo;
	memset(&FriendInfo, 0, sizeof(FRIEND_LIST));
	FRIENDMGR->GetFriendListInfo(&FriendInfo, gotopage);
	
	ShowFriendPageBtn(FriendInfo.totalnum);

	m_pFriendListLCtl->DeleteAllItems();
	
	for(int i=0; i<MAX_FRIEND_LIST; ++i)
	{
		m_pFriendChkBox[i]->SetChecked(0);
		m_pFriendChkBox[i]->SetActive(TRUE);
		m_LinkHouseBtnArr[i]->SetActive(TRUE);
		
		if(FriendInfo.Friend[i].Id != 0)
		{
			cRITEMEx *ritem = new cRITEMEx(1);
			strcpy(ritem->pString[0], FriendInfo.Friend[i].Name);
			if(FriendInfo.Friend[i].IsLoggIn)
			{
				const DWORD loginColor( RGBA_MAKE( 0, 48, 255, 0 ) );

				ritem->rgb[0] = loginColor;
			}
			else
			{
				// 100311 ONS 친구리스트 색상 흰색으로 수정
				const DWORD logoutcolor( RGBA_MAKE( 255, 255, 255, 0 ) );

				ritem->rgb[0] = logoutcolor;
			}
			ritem->dwID = FriendInfo.Friend[i].Id;
			m_pFriendListLCtl->InsertItem(i, ritem);
		}
		else
		{
			m_pFriendChkBox[i]->SetActive(FALSE);
			m_LinkHouseBtnArr[i]->SetActive(FALSE);
		}
	}

	// 080121 LYW --- FriendDialog : 선택 된 탭 버튼만 푸쉬 상태로 세팅.
	int nCurPage = gotopage-1 ;

	for( int count = 0 ; count < MAX_FRIEND_PAGE ; ++count )
	{
		if( !m_FriendPageBtn[count] ) continue ;

		if( nCurPage == count )
		{
			m_FriendPageBtn[count]->SetPush(TRUE) ;
		}
		else
		{
			m_FriendPageBtn[count]->SetPush(FALSE) ;
		}
	}
}

void CFriendDialog::ShowFriendPageBtn(BYTE TotalPage)
{
	if(TotalPage == 0)
	{
		for(BYTE i=0; i<MAX_FRIEND_PAGE; ++i)
			m_FriendPageBtn[i]->SetActive(FALSE);
		return;
	}

	for(DWORD i=0; i<MAX_FRIEND_PAGE; ++i)
	{
		m_FriendPageBtn[i]->SetTextValue(i+1);
		m_FriendPageBtn[i]->SetActive(i+1 <= TotalPage);
	}
}

//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가( PC방멤버 리스트 페이지버튼처리 )
void CFriendDialog::ShowPCRoomPageBtn(BYTE TotalPage)
{
	if(TotalPage == 0)
	{
		for(BYTE i=0; i<MAX_PCROOM_MEMBER_NUM/MAX_PCROOM_MEMBER_LIST; ++i)
			m_PCRoomPageBtn[i]->SetActive(FALSE);
		return;
	}

	for(BYTE i=0; i<MAX_PCROOM_MEMBER_NUM/MAX_PCROOM_MEMBER_LIST; ++i)
	{
		m_PCRoomPageBtn[i]->SetTextValue(i+1);
		m_PCRoomPageBtn[i]->SetActive(i+1 <= TotalPage);
	}
}

/*
void CFriendDialog::UpdateLogIn(char* LogInName)
{
	for(int i=0; i<MAX_FRIEND_LIST; ++i)
	{
		if(strcmp(m_FriendList[i].Name, LogInName) == 0)
		{
			m_FriendList[i].IsLoggIn = TRUE;
			RefreshDlg();
			return;
		}
	}
}

void CFriendDialog::UpdateLogOut(char* LogOutName)
{
	for(int i=0; i<MAX_FRIEND_LIST; ++i)
	{
		if(strcmp(m_FriendList[i].Name, LogOutName) == 0)
		{
			m_FriendList[i].IsLoggIn = FALSE;
			RefreshDlg();
			return;
		}
	}
}
*/


/*
void CFriendDialog::RefreshDlg()
{
	if(m_bDlgActive == TRUE)
	{
		m_pFriendListLCtl->DeleteAllItems();
		
		for(int i=0; i<MAX_FRIEND_LIST; ++i)
		{
			if(m_FriendList[i].Id != 0)
			{
				RITEMExf *ritem = new RITEMExf(2);
				
				strcpy(ritem->pString[0], m_FriendList[i].Name);
				if(m_FriendList[i].IsLoggIn)
				{
					if(m_LogInGroup == FALSE)
					{
						strcpy(ritem->pString[1], "온라인");
						m_LogInGroup = TRUE;
					}
					else
						strcpy(ritem->pString[1], "");
				}
				else
				{
					if(m_LogOutGroup == FALSE)
					{
						strcpy(ritem->pString[1], "오프라인");
						m_LogOutGroup = TRUE;
					}
					else
						strcpy(ritem->pString[1], "");
					
					for(int n=0; n<2; ++i)
						ritem->rgb[n] = RGBA_MAKE(150,150,150,255);
				}
				ritem->dwID = m_FriendList[i].Id; 
				m_pFriendListLCtl->InsertItem(i, ritem);
			}
			
			ShowFriendPageBtn(m_TotalPage);
		}
	}
}
*/


void CFriendDialog::CheckedDelFriend()
{
	DWORD IDs[MAX_FRIEND_LIST];
	memset(IDs,0,sizeof(DWORD)*MAX_FRIEND_LIST);

	for(int i=0; i<MAX_FRIEND_LIST; ++i)
	{
		// 100216 pdy FriendDlg 채크박스 처리 잘못된코드 수정
		if(m_pFriendChkBox[i]->IsChecked())
		{
			cRITEMEx * rItem = (cRITEMEx *)m_pFriendListLCtl->GetRItem(i);
			if(rItem->dwID)
			{
				IDs[i] = rItem->dwID;
			}
		}
	}
	
	int lastidx = MAX_FRIEND_LIST;
	for(i=0;i<MAX_FRIEND_LIST;++i)
	{
		if(IDs[i] != 0)
			lastidx = i;
	}

	for(i=0;i<MAX_FRIEND_LIST;++i)
	{
		if(IDs[i] != 0)
		{
			BOOL bLast;
			if(i==lastidx)
				bLast = TRUE;
			else
				bLast = FALSE;
			FRIENDMGR->DelFriendSynbyID(IDs[i],bLast);
		}
	}
}

char* CFriendDialog::GetChkedName()
{
	for(int i=0; i<MAX_FRIEND_LIST; ++i)
	{
		// 100216 pdy FriendDlg 채크박스 처리 잘못된코드 수정
		if(m_pFriendChkBox[i]->IsChecked())
		{
			cRITEMEx * rItem = (cRITEMEx *)m_pFriendListLCtl->GetRItem(i);
			return rItem->pString[0];
		}
	}
	return "";
}
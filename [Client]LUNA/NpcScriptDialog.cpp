#include "stdafx.h"
#include "WindowIDEnum.h"
#include "GameIn.h"
#include "Quest.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "ChatManager.h"
#include "QuestManager.h"
#include "NpcScriptManager.h"
#include "Interface/cWindowManager.h"
#include "Interface/cStatic.h"
#include "Input/UserInput.h"
#include "cMsgBox.h"
#include "cDialogueList.h"
#include "cHyperTextList.h"
#include "NpcImageDlg.h"
#include "NpcScriptDialog.h"
#include "QuestDialog.h"
#include "./Interface/GameNotifyManager.h"
#include "FishingPointDialog.h"
#include "interface/cFont.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "SiegeWarfareMgr.h"
#include "MHMap.h"
#include "cMapMoveDialog.h"
#include "cResourceManager.h"
#include "./NpcRecallMgr.h"
#include "../[CC]Header/GameResourceManager.h"
#include "./Interface/cIcon.h"														// 아이콘 클래스 헤더를 불러온다.
#include "./Interface/cIconGridDialog.h"											// 아이콘 그리드 다이얼로그 클래스 헤더를 불러온다.
#include "./Interface/cScriptManager.h"												// 스크립트 매니져 클래스 헤더를 불러온다.
#include "./Interface/cFont.h"														// 폰트 클래스 헤더를 불러온다.
#include "..\[CC]Quest\QuestString.h"												// 퀘스트 스트링 클래스 헤더를 불러온다.
#include "..\[CC]Quest\QuestTrigger.h"
#include "..\[CC]Quest\QuestExecute_Item.h"											// 퀘스트 실행 아이템 클래스 헤더를 불러온다.
#include "..\[CC]Quest\QuestExecute_SelectItem.h"											// 퀘스트 실행 아이템 클래스 헤더를 불러온다.
#include "itemmanager.h"
#include "QuestQuickViewDialog.h"
#include "ChattingDlg.h"
#include "cPage.h"
#include "cListDialogEx.h"

extern HWND _g_hWnd;

#define MAX_FIX_REQUITAL_ICON_CELL 5
#define MAX_SELECT_REQUITAL_ICON_CELL 10

cNpcScriptDialog::cNpcScriptDialog()													// 생성자 함수.
{
	m_type = WT_NPCSCRIPDIALOG;
	m_dwCurNpc = 0;
	m_wNpcChxNum = 0;
	m_pListDlg = NULL;
	m_pLinkListDlg = NULL;
	m_pQuestBtn = NULL;
	m_pTitleText = NULL;
	m_dwQuestIdx = 0;
	m_dwMainLinkPageId = 0;

	// 100414 ONS 퀘스트보상관련 변수 초기화
	m_pRequitalInfoDlg			= NULL;
	m_pFixRequitalGrid			= NULL;
	m_pSelectRequitalGrid		= NULL;
	m_pRequitalQuestName		= NULL;
	m_pRequitalGold				= NULL;
	m_pRequitalExp				= NULL;
	m_RequitalItemTooltipIdx	= -1 ;
	m_RequitalIndex				= 0;
	m_RequitalCount				= 0;
}

cNpcScriptDialog::~cNpcScriptDialog()													// 소멸자 함수.
{
	m_pListDlg->RemoveAll() ;
	m_pLinkListDlg->RemoveAll() ;

	// 100414 ONS 퀘스트 보상관련 메모리 해제 
	RequitalItem* pDeleteItem = NULL ;
	PTRLISTPOS deletePos = NULL ;
	deletePos = m_QuestFixRequitalList.GetHeadPosition() ;
	while(deletePos)
	{
		pDeleteItem = NULL ;
		pDeleteItem = (RequitalItem*)m_QuestFixRequitalList.GetNext(deletePos) ;
		if( pDeleteItem )
		{
			m_QuestFixRequitalList.Remove(pDeleteItem) ;
			delete pDeleteItem ;
		}
	}
	m_QuestFixRequitalList.RemoveAll() ;

	deletePos = m_QuestSelectRequitalList.GetHeadPosition() ;
	while(deletePos)
	{
		pDeleteItem = NULL ;
		pDeleteItem = (RequitalItem*)m_QuestSelectRequitalList.GetNext(deletePos) ;
		if( pDeleteItem )
		{
			m_QuestSelectRequitalList.Remove(pDeleteItem) ;
			delete pDeleteItem ;
		}
	}
	m_QuestSelectRequitalList.RemoveAll() ;

	m_IndexGenerator.Release();
}

void cNpcScriptDialog::Linking()														// 링크 함수.
{
	m_pListDlg = (cListDialogEx*)this->GetWindowForID( NSI_LISTDLG );
	m_pLinkListDlg = (cListDialogEx*)this->GetWindowForID(NSI_LISTDLG2);
	m_pQuestBtn = (cButton*)GetWindowForID(NSI_QUESTBTN);

	// 100414 ONS 퀘스트보상정보를 다이얼로그로 받아온다.
	m_pRequitalInfoDlg		= (cDialog*)this->GetWindowForID( NSI_DLG_SELECTITEM );
	m_pRequitalInfoDlg->SetDisable(TRUE);

	// 일반보상과 선택보상 그리드 설정.
	m_pFixRequitalGrid		= (cIconGridDialog*)m_pRequitalInfoDlg->GetWindowForID(NSI_REQUITAL_GRID) ;
	m_pSelectRequitalGrid	= (cIconGridDialog*)m_pRequitalInfoDlg->GetWindowForID(NSI_REQUITAL_SELECT_GRID) ;

	m_pRequitalQuestName	= (cStatic*)m_pRequitalInfoDlg->GetWindowForID(NSI_REQUITAL_QUESTNAME);
	m_pRequitalGold			= (cStatic*)m_pRequitalInfoDlg->GetWindowForID(NSI_REQUITAL_GOLD);
	m_pRequitalExp			= (cStatic*)m_pRequitalInfoDlg->GetWindowForID(NSI_REQUITAL_EXP);

	m_IndexGenerator.Init( 20, IG_SHOPITEM_STARTINDEX );

	m_pTitleText = new cStatic;															// 타이틀 텍스트만큼 메모리를 할당한다.

	if( m_pTitleText )																	// 메모리 할당 정보가 유효하다면,
	{
		m_pTitleText->Init( 15, 15, 0, 0, NULL, -1 );									// 타이틀 텍스트를 초기화 한다.

		m_pTitleText->SetShadow( TRUE );												// 그림자 여부를 TRUE로 세팅한다.

		m_pTitleText->SetFontIdx( 5 );													// 2번 폰트로 세팅한다.

		m_pTitleText->SetAlign( TXT_LEFT );												// 텍스트 정렬을 왼쪽 정렬로 세팅한다.
		
		m_pTitleText->SetStaticText( "" );												// 텍스트를 ""로 세팅한다.

		m_pTitleText->SetFGColor( RGBA_MAKE( 170, 236, 4, 255 ) );
		
		Add( m_pTitleText );															// 다이얼로그에 타이틀을 추가한다.
	}
}

DWORD cNpcScriptDialog::ActionEvent(CMouse* mouseInfo)	
{
	// 100414 ONS 보상 그리드의 아이콘 툴팁을 표시한다.
	DWORD we = WE_NULL;	
	if( !m_bActive )	
	{
		return we;		
	}

	we = cDialog::ActionEvent(mouseInfo);

	int nMouseX = mouseInfo->GetMouseX() ;
	int nMouseY = mouseInfo->GetMouseY() ;

	if( m_pFixRequitalGrid->PtInWindow(nMouseX,nMouseY))
	{
		WORD Pos = WORD(-1);

		if(m_pFixRequitalGrid->GetPositionForXYRef(nMouseX,nMouseY,Pos))
		{
			ShowRequitalItemToolTip(m_QuestFixRequitalList, Pos);		
			return 0;													
		}
	}
	if( m_pSelectRequitalGrid->PtInWindow(nMouseX,nMouseY))
	{
		WORD Pos = WORD(-1);

		if(m_pSelectRequitalGrid->GetPositionForXYRef(nMouseX,nMouseY,Pos))
		{
			ShowRequitalItemToolTip(m_QuestSelectRequitalList, Pos);	
			return 0;													
		}
	}

	return we;
}

void cNpcScriptDialog::OnActionEvent(LONG lId, void* p, DWORD we)
{
	switch( lId )
	{
	case NSI_TOPBTN :
		{
			OpenDialog(m_dwCurNpc, m_wNpcChxNum) ;
		}
		break ;

	case NSI_ENDBTN :
		{
			EndDialog() ;
			SetActive(FALSE) ;
		}
		break ;

	// 090924 ShinJS --- 퀘스트 수락/완료 버튼이 눌린 경우
	case NSI_QUESTBTN :
		{
			CQuest* pQuest = QUESTMGR->GetQuest( m_dwQuestIdx );
			if( !pQuest )
			{
				// 퀘스트가 선택이 안된 상태에서 모두완료버튼이 눌린 경우 현재 수행중인 퀘스트 중에서 완료가능한 퀘스트를 모두 완료시킨다.
				if( m_QuestBtnType == eQuestBtnType_CompleteAll )
				{
					DWORD dwPageId = 0 ;
					if( m_dwMainLinkPageId != 0 )
						dwPageId = m_dwMainLinkPageId ;
					else
						dwPageId = m_dwCurPageId ;

					cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );						// 현재 npc와 페이지 아이디로 페이지 정보를 얻는다.
					if( pPage == NULL ) break;

					int nLinkCount = pPage->GetHyperLinkCount();										// 링크 카운트를 세팅한다.
					for( int i = 0; i < nLinkCount; ++i )												// 링크 카운트 수 만큼 for문을 돌린다.
					{
						HYPERLINK* pLink = pPage->GetHyperText(i);										// 하이퍼 링크 정보를 받는다.
						if( pLink == NULL ) continue;

						CQuest* pQuest = QUESTMGR->GetQuest( pLink->dwData );							// 퀘스트 정보를 받는다.
						if( !pQuest ) continue;
						 
						if( pQuest->IsQuestState( m_dwCurNpc ) == eQuestState_Executed &&				// 수행중인 퀘스트인 경우
							pQuest->IsPossibleQuestComplete() )											// 완료 가능한 퀘스트인 경우
						{
							// 100414 ONS 선택보상아이템이 없는 경우 완료.
							if( !CanTakeSelectRequital(pLink->dwData) )
							{
								// 퀘스트를 완료시킨다.
								QUESTMGR->SendNpcTalkEvent( m_dwCurNpc, pLink->dwData );
							}
							else
							{
								// 보상아이템이 있으면 해당퀘스트의 보상페이지를 설정한다.
								WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, CHATMGR->GetChatMsg(2214) );

								m_pLinkListDlg->SetActive( FALSE );
								m_pRequitalInfoDlg->SetDisable( FALSE );
								m_pRequitalInfoDlg->SetActive( TRUE );

								OpenRequitalPage( pLink->dwData );

								DWORD dwPage = pQuest->GetNpcScriptPage( m_dwCurNpc );
								OpenLinkPage( dwPage );	

								m_dwQuestIdx = pLink->dwData;
								return;								
							}
						}
					}

					// 퀘스트 Link Page Update
					RefreshQuestLinkPage();

					// 완료 버튼을 비활성화 시킨다
					m_pQuestBtn->SetActive( FALSE );
				}

				break;
			}
			
			// 100414 ONS 완료버튼 클릭시 선택보상 선택여부 판단
			if(	!IsRequitalSelected( m_dwQuestIdx )	)
			{
				WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, CHATMGR->GetChatMsg(2214) );
				return;
			}

			// 100524 ONS 퀘스트리스트 최대개수 체크처리 추가.
			CQuestDialog* pQuestDialog = GAMEIN->GetQuestDialog() ;
			if( pQuestDialog )
			{
				pQuestDialog->SetQuestCount();
				if( pQuestDialog->GetQuestCount() >= MAX_QUEST_COUNT )
				{
					// 최대갯수일경우, 퀘스트가 목록에 존재(진행중, 완료)하면 진행시키지만,
					// 그렇지않을 경우(수락)는 퀘스트FULL메세지를 출력하고 종료한다.
					if( FALSE == pQuestDialog->IsExistInQuestList( m_dwQuestIdx ) )
					{
						WINDOWMGR->MsgBox( MBI_QUEST_FULL, MBT_OK, CHATMGR->GetChatMsg( 1315 ) );
						return;
					}
				}
			}

			if( m_QuestBtnType == eQuestBtnType_Progress )
			{
				// 퀘스트진행버튼 클릭시 목록 페이지 Open
				OpenQuestLinkPage( m_dwMainLinkPageId );
			}

			// 마지막 페이지 Open
			OpenLastPageOfQuest();

			// 퀘스트 수락/완료 요청
			if( m_dwQuestIdx )
			{
				QUESTMGR->SendNpcTalkEvent( m_dwCurNpc, m_dwQuestIdx, m_RequitalIndex, m_RequitalCount );
				m_dwQuestIdx = 0;
			}
			// 수락/완료 버튼을 비활성화 시킨다
			m_pQuestBtn->SetActive( FALSE );

			// 100414 ONS 보상다이얼로그를 비활성화시키고 퀘스트리스트를 활성화한다.
			m_pRequitalInfoDlg->SetActive( FALSE );
			m_pLinkListDlg->SetActive( TRUE );
		}
		break;
	}
}

void cNpcScriptDialog::ResetConversationList(cDialogueList* pList, DWORD dwMsg)			// 대화창을 다시 세팅 하는 함수.
{
	if( !pList )
	{
		return ;
	}

	m_pListDlg->RemoveAll();

	WORD wIdx = 0;																		// 인덱스를 담을 변수를 선언하고 0으로 세팅한다.

	WORD LineInfo = 0;																	// 라인 정보를 받을 변수를 선언하고 0으로 세팅한다.

	DIALOGUE* temp = NULL;																// 다이얼로그 정보를 담을 포인터 변수를 선언하고 NULL처리를 한다.

	LINKITEM* pItem = NULL;																// 링크 아이템 정보를 담을 포인터 변수를 선언하고 NULL 처리를 한다.
	LINKITEM* Prev = NULL;																// 이전 링크 아이템 정보를 담을 포인터 변수를 선언하고 NULL 처리를 한다.

	while(1)																			// while문을 돌린다.
	{
		temp = pList->GetDialogue( dwMsg, wIdx );										// 랜덤하게 뽑은 페이지로 다이얼로그 정보를 받는다.

		if( temp == NULL ) break;														// 정보가 유효하지 않으면, while문을 빠져나간다.

		pItem = new LINKITEM;															// 아이템 메모리를 할당하여 포인터로 넘긴다.

		strcpy(pItem->string, temp->str);												// 임시 정보의 스트링을 아이템 스트링에 복사를 한다.

		pItem->rgb = temp->dwColor;														// 임시 정보의 색상을 아이템 색상으로 세팅한다.

		//m_pConversationList.AddTail(pItem) ;

		if( Prev == NULL )																// 이전 아이템 정보가 없다면,
		{
			m_pListDlg->cListItem::AddItem(pItem);										// 아이템 정보를 대사 리스트에 추가한다.

			LineInfo = temp->wLine;														// 라인정보는 임시 정보의 라인으로 세팅한다.
		}
		else																			// 이전 아이템 정보가 있다면,
		{
			if( LineInfo == temp->wLine )												// 라인 정보가 임시 라인과 같으면,
			{
				Prev->NextItem = pItem;													// 이전 아이템의 다음 아이템으로 현재 아이템을 세팅한다.
			}
			else																		// 라인 정보가 임시 라인과 같지 않으면,
			{
				m_pListDlg->cListItem::AddItem(pItem);									// 대사 리스트 다이얼로그에 아이템을 추가한다.

				LineInfo = temp->wLine;													// 라인 정보는 임시 라인으로 세팅한다.
			}
		}
		
		Prev = pItem;																	// 이전 아이템 정보에 현재 아이템 정보를 세팅한다.

		++wIdx;																			// 인덱스 정보를 증가한다.
	}

	m_pListDlg->ResetGuageBarPos() ;													// 대화 리스트의 게이지바 위치를 다시 잡는다.
}

void cNpcScriptDialog::ResetMainLinkList(cPage* pMainPage, cHyperTextList* pHyper, cListDialogEx* pListDlg)	// 링크 페이지를 다시 세팅하는 함수.
{
	m_pLinkListDlg->RemoveAll();

	DIALOGUE* temp = NULL;																// 다이얼로그 정보를 담을 포인터 변수를 선언하고 NULL처리를 한다.
	HYPERLINK* pLink;																	// 링크 정보를 담을 포인터를 선언하고 NULL처리를 한다.

	int nLinkCount = pMainPage->GetHyperLinkCount();									// 페이지의 링크 카운트를 받는다.

	// 협행 보일지 말지 결정
	BOOL bQuest = QUESTMGR->IsNpcRelationQuest( m_dwCurNpc );							// 퀘스트 여부를 받는다.

	for( int j = 0 ; j < nLinkCount ; ++j )												// 링크 카운트 만큼 for문을 돌린다.
	{
		pLink = pMainPage->GetHyperText(j);												// 카운트에 해당하는 링크 정보를 받는다.

		if( pLink == NULL )																// 링크 정보가 유효하지 않으면 
		{
			break;																		// for문을 탈출한다.
		}

		if( pLink->wLinkType == emLink_Quest && !bQuest )								// 링크 타입이 퀘스트와 같고, bQuest가 FALSE와 같으면,
		{
			continue;																	// 계속한다.
		}

		temp = pHyper->GetHyperText( pLink->wLinkId );									// 링크 아이디로 다이얼로그 정보를 받는다.

		if( temp )																		// 다이얼로그 정보가 유효하면,
		{
			LINKITEM* pItem = new LINKITEM;												// 링크 아이템 메모리를 할당한다.

			strcpy(pItem->string, temp->str);											// 아이템 스트링을 다이얼로그 스트링으로 세팅한다.

			pItem->dwType = pLink->wLinkType;											// 아이템 링크 타입을 세팅한다.
			pItem->dwData = pLink->dwData;
			if( pLink->wLinkType == emLink_Quest )
			{
				pItem->rgb = RGBA_MAKE( 170, 236, 4, 255 );	
			}
			pListDlg->cListItem::AddItem( pItem );
		}
	}

	pListDlg->ResetGuageBarPos() ;														// 리스트의 게이지바 위치를 다시 잡는다.
}

void cNpcScriptDialog::ResetConversationLinkList(cPage* pMainPage, cHyperTextList* pHyper)
{
	int nLinkCount = pMainPage->GetHyperLinkCount();
	if( 0 == nLinkCount )
	{
		m_dwCurPageId = m_dwMainLinkPageId;
	}

	BOOL bQuest = QUESTMGR->IsNpcRelationQuest( m_dwCurNpc );

	{
		LINKITEM* pItem = pItem = new LINKITEM;
		strcpy( pItem->string, "" );
		m_pListDlg->cListItem::AddItem( pItem );
	}

	for( int j = 0 ; j < nLinkCount ; ++j )												// 링크 카운트 만큼 for문을 돌린다.
	{
		HYPERLINK* pLink = pMainPage->GetHyperText(j);												// 카운트에 해당하는 링크 정보를 받는다.

		if( pLink == NULL )																// 링크 정보가 유효하지 않으면 
		{
			break;																		// for문을 탈출한다.
		}

		if( pLink->wLinkType == emLink_Quest && !bQuest )								// 링크 타입이 퀘스트와 같고, bQuest가 FALSE와 같으면,
		{
			continue;																	// 계속한다.
		}

		if(DIALOGUE* temp = pHyper->GetHyperText( pLink->wLinkId ))
		{
			LINKITEM* pItem = new LINKITEM;												// 링크 아이템 메모리를 할당한다.	

			strcpy(pItem->string, temp->str);											// 아이템 스트링을 다이얼로그 스트링으로 세팅한다.

			pItem->dwType = pLink->wLinkType;											// 아이템 링크 타입을 세팅한다.
			pItem->dwData = pLink->dwData;
			pItem->rgb = RGBA_MAKE( 170, 236, 4, 255 );
			m_pListDlg->cListItem::AddItem( pItem );
		}
	}

	m_pListDlg->ResetGuageBarPos() ;													//대화 리스트의 게이지바 위치를 다시 잡는다.
}

void cNpcScriptDialog::SettingNpcImage(cPage* pMainPage, WORD npcChxNum)				// NPC 이미지와 배경을 세팅하는 함수.											
{
   	GAMEIN->GetNpcImageDlg()->SetNpc( npcChxNum );										// NPC 이미지 다이얼로그에 현재 NPC를 세팅한다.
 	GAMEIN->GetNpcImageDlg()->SetEmotion( pMainPage->GetEmotion() );					// NPC의 이모션을 세팅한다.
	GAMEIN->GetNpcImageDlg()->SetActive( TRUE );										// NPC 이미지 다이얼로그를 활성화 한다.
	GAMEIN->GetNpcImageDlg()->SetDisable( TRUE );										// NPC 이미지 다이얼로그를 디스에이블 시킨다.

	CNpc* pObj = NPCSCRIPTMGR->GetSelectedNpc();										// 선택 된 NPC 정보를 받는다.

	if( pObj )																			// NPC 정보가 유효하면,
	{
		m_pTitleText->SetStaticText( pObj->GetObjectName() );
	}
	else																				// NPC 정보가 유효하지 않으면,
	{	
 		m_pTitleText->SetStaticText( CHATMGR->GetChatMsg(42) );
		
		CSHFarmRenderObj* pFarmObj = g_csFarmManager.CLI_GetTargetObj();

		if (pFarmObj &&
			pFarmObj->GetInfo()->nKind == CSHFarmManager::RENDER_STEWARD_KIND)
		{
			m_pTitleText->SetStaticText( pFarmObj->GetObjectName() );
 			GAMEIN->GetChattingDlg()->SetActive( TRUE );
 			SetFocus(TRUE);
		}
	}

	SetActive( TRUE );																	// NPC 스크립트 다이얼로그를 활성화 한다.
}

BOOL cNpcScriptDialog::OpenDialog( DWORD dwNpcId, WORD npcChxNum )						// 다이얼로그를 여는 함수.
{
	m_dwCurNpc = dwNpcId;																// 현재 NPC 아이디를 세팅한다.
	m_wNpcChxNum = npcChxNum ;															// 현재 NPC CHX 번호를 세팅한다.

	cPage* pMainPage = NPCSCRIPTMGR->GetMainPage( m_dwCurNpc );							// NPC 스크립트 매니져로 부터 현재 NPC의 메인 페이지 정보를 받는다.

	if( pMainPage == NULL ) return FALSE;												// 페이지 정보가 유효하지 않으면, FALSE 리턴 처리를 한다.

	cDialogueList* pList = NPCSCRIPTMGR->GetDialogueList();								// 스트립트 매니져로 부터 다이얼로그 정보를 받는다.

	if( pList == NULL ) return FALSE;													// 다이얼로그 정보가 유효하지 않으면 FALSE 리턴 처리를 한다.

	cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();							// 스크립트 매니져로 부터 하이퍼 텍스트 정보를 받는다.

	if( pHyper == NULL ) return FALSE;

	ResetConversationList(
		pList,
		pMainPage->GetRandomDialogue());
	ResetMainLinkList(
		pMainPage,
		pHyper,
		m_pLinkListDlg);
	MoveCursorToLink();
	SettingNpcImage(
		pMainPage,
		npcChxNum);

	// 100414 ONS 보상다이얼로그를 비활성화한다.
	m_pRequitalInfoDlg->SetActive( FALSE );
	m_pRequitalInfoDlg->SetDisable( TRUE );

	return TRUE;																		// TRUE를 리턴한다.
}

BOOL cNpcScriptDialog::OpenLinkPage( DWORD dwPageId )									// 링크 페이지를 여는 함수.
{
	m_dwCurPageId = dwPageId;															// 임시로 현재 표시 페이지를 가진다.	

	cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );						// 현재 NPC와 현재 페이지 아이디로 페이지 정보를 받는다.

	if( pPage == NULL ) return FALSE;													// 페이지 정보가 유효하지 못하면 FALSE 리턴 처리를 한다.

	cDialogueList* pList = NPCSCRIPTMGR->GetDialogueList();								// 메세지 번호를 이용하여 사용할 메세지를 가져온다.

	if( pList == NULL ) return FALSE;													// 다이얼로그 정보가 유효하지 않으면, FALSE 리턴 처리를 한다.

	cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();							// 하이퍼 텍스트 리스트 정보를 받는다.

	if( pHyper == NULL ) return FALSE;													// 하이퍼 텍스트 리스트 정보가 유효하지 않으면 FALSE 리턴 처리를 한다.

	DWORD dwMsg = pPage->GetRandomDialogue();											// 페이지 정보를 이용하여 페이지를 구성한다. // 메세지의 번호

	ResetConversationList(pList, dwMsg) ;												// 대화 리스트를 다시 세팅한다.
	
	ResetConversationLinkList(pPage, pHyper) ;											// 대화 리스트에 링크를 세팅한다.
	
	GAMEIN->GetNpcImageDlg()->SetEmotion( pPage->GetEmotion() );						// NPC 이미지 다이얼로그의 이모션을 설정한다.

	return TRUE;																		// TRUE를 리턴한다.
}

void cNpcScriptDialog::EndDialog()														// 다이얼로그를 종료하는 함수.
{
	m_pListDlg->RemoveAll();
	m_dwCurNpc = 0;																		// 현재 npc를 0으로 세팅한다.
	m_wNpcChxNum = 0 ;																	// 현재 npc chx 번호를 0으로 세팅한다.

	GAMEIN->GetNpcImageDlg()->SetDisable( FALSE );										// NPC 이미지 다이얼로그의 Disable을 해제한다.
	GAMEIN->GetNpcImageDlg()->SetActive( FALSE );										// NPC 이미지 다이얼로그를 비활성화 한다.

	NPCSCRIPTMGR->SetSelectedNpc( NULL );												// 선택한 NPC를 해제한다.
	
	//죽은후에 들어올 수도 있다.
	if( HERO->GetState() != eObjectState_Die )											// HERO가 죽은 상태가 아니면,
	{
		if(FALSE == ((cDialog*)GAMEIN->GetStorageDialog())->IsActive() &&
			FALSE == ((cDialog*)GAMEIN->GetDealDialog())->IsActive() &&
			FALSE == ((cDialog*)GAMEIN->GetGuildWarehouseDlg())->IsActive())
		{
			OBJECTSTATEMGR->EndObjectState(
				HERO,
				eObjectState_Deal);
		}
	}
}

void cNpcScriptDialog::HyperLinkParser(LPCTSTR selectedText,DWORD selectedData)
{
	const HYPERLINK& hyperLink = GetHyperLink(
		selectedText, selectedData);
	
	switch(hyperLink.wLinkType)
	{
	case emLink_Page:																	// 링크 페이지라면,
		{
			OpenLinkPage(
				hyperLink.dwData);
			m_pListDlg->ResetGuageBarPos();
		}
		break;

	case emLink_Open:																	// 링크 오픈이면,
		{
			// desc_hseos_농장시스템_01
			// S 농장시스템 추가 added by hseos 2008.01.23
			// ..농장 업그레이드 처리
			if (m_dwCurNpc == 999)
			{		
				if (m_dwCurPageId == 90	&&
					g_csFarmManager.CLI_ProcUpgradeDlg(hyperLink.dwData))
				{
					return;
				}
				// ..농장 관리비 납부 처리
				if (hyperLink.wLinkId == 691 &&
					hyperLink.dwData == 6 &&
					g_csFarmManager.CLI_ProcPayTaxDlg())
				{
					return;
				}
			}

			// 길드토너먼트 추가
			if(m_dwCurNpc == 125)
			{
				MSGBASE msg;
				MSG_WORD msgGTTableSyn;
				MSG_DWORD msgMapSyn;

				switch(hyperLink.wLinkId)
				{
				case 734:	//토너먼트 신청을 한다
					// 다이얼로그로 빼자
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_REGIST_SYN;
					msg.dwObjectID = HEROID;
					NETWORK->Send( &msg, sizeof(msg) );

					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					return;
				case 735:	//참가자 명단을 수정한다
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_REGISTPLAYER_SYN;
					msg.dwObjectID = HEROID;
					NETWORK->Send( &msg, sizeof(msg) );

					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					return;
				case 736:	//토너먼트 대진표를 확인한다
					// 다이얼로그로 빼자
					msgGTTableSyn.Category = MP_GTOURNAMENT;
					msgGTTableSyn.Protocol = MP_GTOURNAMENT_STANDINGINFO_SYN;
					msgGTTableSyn.dwObjectID = HEROID;
					//msgGTTableSyn.wData = 1;
					NETWORK->Send( &msgGTTableSyn, sizeof(msgGTTableSyn) );

					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					return;
				case 737:	//토너먼트 경기장으로 이동을 한다
					msgMapSyn.Category = MP_GTOURNAMENT;
					msgMapSyn.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_SYN;
					msgMapSyn.dwObjectID = HEROID;
					msgMapSyn.dwData = 0;	// 관전원하는 배틀번호
					NETWORK->Send( &msgMapSyn, sizeof(msgMapSyn) );

					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					return;
				//case 738:	//토너먼트 보상을 받는다
				//	GAMEIN->GetNpcScriptDialog()->EndDialog();
				//	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
				//	return;
				default:
					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					return;
				}
			}

			// 080514 KTH -- 주석 처리
			//NPCSCRIPTMGR->StartNpcBusiness( dwData );									// 거래창을 연다.
			// 080514 KTH -- Hack Check
			CObject* pObject = OBJECTMGR->GetSelectedObject();

			// 090601 LUJ, 전송을 보내기 전에 선택을 해제할 경우, 클라이언트가 강제 종료된다
			if( 0 == pObject )
			{
				break;
			}

			MSG_DWORD3	msg;
			msg.Protocol = MP_NPC_CHECK_HACK_SYN;
			msg.Category = MP_NPC;
			msg.dwObjectID = HEROID;
			msg.dwData1 = pObject->GetID() ;//m_dwCurNpc;
			msg.dwData2 = (DWORD)((CNpc*)pObject)->GetNpcUniqueIdx();
			msg.dwData3 = hyperLink.dwData;
			NETWORK->Send(&msg, sizeof(msg));
		}
		break;

	case emLink_End:																	// 링크 종료면,
		{
			EndDialog();																// 다이얼로그를 종료한다.
			DisActiveEx();																// Disable한다.
		}
		break;

	case emLink_Quest:																	// 링크 퀘스트면,
		{
			if( m_dwCurNpc == 135 || m_dwCurNpc == 136 )
			{
				//공성전 중이라면 
				if( SIEGEWARFAREMGR->GetState( ) != eSiegeWarfare_State_Before )
				{
					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					NPCSCRIPTMGR->SetSelectedNpc(NULL);
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1705));
					return;
				}
			}
			// 090924 ShinJS --- 퀘스트가 선택이 안된 경우를 판단하기 위해 초기화 추가
			m_dwQuestIdx = 0;			
			m_dwMainLinkPageId = hyperLink.dwData;

			OpenQuestLinkPage(
				hyperLink.dwData);
		}
		break;

	case emLink_QuestLink:																// 링크 퀘스트 링크면,
		{
			m_dwQuestIdx = 0;															// 퀘스트 인덱스를 0으로 세팅한다.

			CQuest* pQuest = QUESTMGR->GetQuest(
				hyperLink.dwData);

			if(0 == pQuest)
			{
				break;
			}

			DWORD dwPage = pQuest->GetNpcScriptPage( m_dwCurNpc );

			if( dwPage == 0 )
			{
				dwPage = 30;
			}

			// 100414 ONS 퀘스트리스트를 비활성화시키고, 보상다이얼로그를 활성화한다.
			m_pLinkListDlg->SetActive( FALSE );
			m_pRequitalInfoDlg->SetDisable( FALSE );
			m_pRequitalInfoDlg->SetActive( TRUE );

			OpenRequitalPage(
				hyperLink.dwData);
			OpenLinkPage(
				dwPage);

			m_dwQuestIdx = hyperLink.dwData;

			switch(pQuest->IsQuestState( m_dwCurNpc ))
			{
				// 수락 가능 퀘스트
			case eQuestState_Possible:
				{
					m_pQuestBtn->SetText( RESRCMGR->GetMsg( 1158 ), m_pQuestBtn->GetBasicColor() );				// "퀘스트 수락"
					m_pQuestBtn->SetToolTip( RESRCMGR->GetMsg( 1160 ), RGB_HALF(255,255,255) );					// "현재 선택된 퀘스트를 수락합니다"
					m_pQuestBtn->SetActive( TRUE );
					m_pQuestBtn->SetRenderArea();
					m_QuestBtnType = eQuestBtnType_Allow;
				}
				break;

				// 수행중인 퀘스트
			case eQuestState_Executed:
				{
					// 퀘스트가 완료 가능한 경우
					if( pQuest->IsPossibleQuestComplete() )
					{
						m_pQuestBtn->SetText( RESRCMGR->GetMsg( 1159 ), m_pQuestBtn->GetBasicColor() );			// "퀘스트 완료"
						m_pQuestBtn->SetToolTip( RESRCMGR->GetMsg( 1161 ), RGB_HALF(255,255,255) );				// "현재 선택된 퀘스트를 완료합니다"
						m_pQuestBtn->SetActive( TRUE );
						m_pQuestBtn->SetRenderArea();
						m_QuestBtnType = eQuestBtnType_Complete;
					}
					else
					{
						// 100525 ONS 퀘스트가 진행 가능한 경우
						m_pQuestBtn->SetText( RESRCMGR->GetMsg( 1449 ), m_pQuestBtn->GetBasicColor() );			// "퀘스트 완료"
						m_pQuestBtn->SetToolTip( RESRCMGR->GetMsg( 1450 ), RGB_HALF(255,255,255) );				// "현재 선택된 퀘스트를 완료합니다"
						m_pQuestBtn->SetActive( TRUE );
						m_pQuestBtn->SetRenderArea();
						m_QuestBtnType = eQuestBtnType_Progress;
					}
				}
				break;
			}
		}
		break;

	case emLink_QuestStart:																// 퀘스트를 수락했다면,
		{
			// 100414 ONS 퀘스트 완료링크를 클릭했을때 보상이 선택되어있는지 체크
			if( !IsRequitalSelected( m_dwQuestIdx ) )
			{
				WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, CHATMGR->GetChatMsg(2214) );
				return;
			}

			// 100524 ONS 퀘스트리스트 최대개수 체크처리 이동 (링크페이지가 열리기전에 체크하도록 변경)
			CQuestDialog* pQuestDialog = GAMEIN->GetQuestDialog() ;
			if( pQuestDialog )
			{
				pQuestDialog->SetQuestCount();
				if( pQuestDialog->GetQuestCount() >= MAX_QUEST_COUNT )
				{
					// 최대갯수일경우, 퀘스트가 목록에 존재(진행중, 완료)하면 진행시키지만,
					// 그렇지않을 경우(수락)는 퀘스트FULL메세지를 출력하고 종료한다.
					if( FALSE == pQuestDialog->IsExistInQuestList( m_dwQuestIdx ) )
					{
						WINDOWMGR->MsgBox( MBI_QUEST_FULL, MBT_OK, CHATMGR->GetChatMsg( 1315 ) );
						return;
					}
				}
			}

			if( m_dwQuestIdx == 45 || m_dwQuestIdx == 46 || m_dwQuestIdx == 47 )		// 퀘스트 인덱스가 45, 46, 47이면,
			{
				if(CQuest* pQuest = QUESTMGR->GetQuest( m_dwQuestIdx ))
				{
					if(FALSE == pQuest->IsTimeCheck())
					{
						if( hyperLink.wLinkId == 347 || hyperLink.wLinkId == 349 )
						{
							HERO->StartSocietyActForQuest( 259, -1, -1, TRUE );			// 퀘스트를 위한 액션을 시작한다.
						}

						OpenLinkPage(
							hyperLink.dwData);
					}
				}
			}
			else																		// 퀘스트 인덱스가 45, 46, 47이 아니면,
			{
				OpenLinkPage(
					hyperLink.dwData);
			}

			if( m_dwQuestIdx )															// 퀘스트 인덱스정보가 유효하다면,
			{
				QUESTMGR->SendNpcTalkEvent( m_dwCurNpc, m_dwQuestIdx, m_RequitalIndex, m_RequitalCount );					// NPC 대화 이벤트를 보낸다.
			}
			
			if(NPCSCRIPTMGR->GetSelectedNpc())											// 선택한 NPC 정보가 유효하다면,
			{
				if(NPCSCRIPTMGR->GetSelectedNpc()->GetNpcJob() == BOMUL_ROLE)			// NPC의 직업이 보물 롤이라면,
				{
					MSG_DWORD msg;														// 메시지 구조체를 선언한다.
					msg.Category = MP_NPC;												// 카테고리를 NPC로 세팅한다.
					msg.Protocol = MP_NPC_DOJOB_SYN;									// 프로토콜을 직업싱크로 세팅한다.
					msg.dwData = NPCSCRIPTMGR->GetSelectedNpc()->GetID();				// 선택한 NPC의 아이디를 세팅한다.
					msg.dwObjectID = HEROID;											// HERO의 아이 세팅한다.
					NETWORK->Send(&msg, sizeof(msg));									// 메시지를 전송한다.
				}
			}

			// 090924 ShinJS --- 퀘스트 수락/완료 버튼 비활성화
			m_pQuestBtn->SetActive( FALSE );
			m_pRequitalInfoDlg->SetActive( FALSE );
			m_pLinkListDlg->SetActive( TRUE );
		}
		break;

	case emLink_EventQuestStart:														// 퀘스트 시작 이벤트 링크라면,
		{
			if(hyperLink.dwData)
			{
				QUESTMGR->SendNpcTalkEventForEvent(
					m_dwCurNpc,
					hyperLink.dwData);
			}

			EndDialog();																// 다이얼로그를 닫는다.
			DisActiveEx();																// 비활성화 시킨다.
		}
		break;

	case emLink_QuestContinue:															// 퀘스트를 계속하는 링크라면,
		{
			OpenLinkPage(
				hyperLink.dwData);
		}
		break;

	case emLink_QuestEnd:																// 퀘스트를 종료하는 링크라면,
		{
			QUESTMGR->QuestAbandon(
				m_dwQuestIdx);
			OpenLinkPage(
				hyperLink.dwData);
		}
		break;

	case emLink_MapChange:																// 맵 체인지 링크라면,
		{
			g_UserInput.GetHeroMove()->MapChangeToNpc(
				hyperLink.dwData,
				hyperLink.fXpos,
				hyperLink.fZpos);
		}
		break;

	case emLink_ChangeMapPage :
		{
			cPage* pPage = NPCSCRIPTMGR->GetPage(
				m_dwCurNpc,
				hyperLink.dwData);

			if( pPage == NULL ) return ;												// 페이지 정보가 유효하지 못하면 FALSE 리턴 처리를 한다.

			cDialogueList* pList = NPCSCRIPTMGR->GetDialogueList();						// 메세지 번호를 이용하여 사용할 메세지를 가져온다.
			if( pList == NULL ) return ;												// 다이얼로그 정보가 유효하지 않으면, FALSE 리턴 처리를 한다.

			cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();					// 하이퍼 텍스트 리스트 정보를 받는다.
			if( pHyper == NULL ) return ;												// 하이퍼 텍스트 리스트 정보가 유효하지 않으면 FALSE 리턴 처리를 한다.

			DWORD dwMsg = pPage->GetRandomDialogue();									// 페이지 정보를 이용하여 페이지를 구성한다. // 메세지의 번호
			ResetConversationList(pList, dwMsg) ;										// 대화 리스트를 다시 세팅한다.

			m_pListDlg->ResetGuageBarPos();												// 게이지바 위치를 리세팅 한다.

			m_dwMainLinkPageId = hyperLink.dwData;

			OpenChangeMapLinkPage(
 				hyperLink.dwData);
		}
		break ;

	case emLink_FishPointChange:
		{
			GAMEIN->GetNpcScriptDialog()->EndDialog();
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
			GAMEIN->GetFishingPointDlg()->ShowDealDialog(TRUE);
		}
		break;

		// 080818 LYW --- NpcScriptDialog : Add link type to recall npc.
   	case emLink_RecallNpc :
   		{
			CNpcRecallBase* pBaseRecallInfo = NPCRECALLMGR->Get_RecallBase(
				hyperLink.dwData);

			if( ! pBaseRecallInfo )
				return;

			DWORD dwParentNpcIdx  = pBaseRecallInfo->Get_ParentNpcIndex() ;
			DWORD dwParentNpcType = (DWORD) pBaseRecallInfo->Get_ParentNpcType() ;

			if(	dwParentNpcType == CNpcRecallBase::eParentNpcType_Static )
			{
				if( dwParentNpcIdx != m_dwCurNpc )
					return;
			}
			else if( dwParentNpcType == CNpcRecallBase::eParentNpcType_Recall )
			{
				if( NPCRECALLMGR->Get_RecallBase( dwParentNpcIdx ) == NULL )
					return;
			}

			//수상한촛대일경우 
			if( m_dwCurNpc == 130 )
			{
   				if( !SIEGEWARFAREMGR->UseCandleStand() )
   				{
   					// Error Message " 고요한 적막감만 흐를 뿐 아무 반응이 없습니다."
   					WINDOWMGR->MsgBox(MBI_RECALLPORTAL_NOTUSE, MBT_OK, CHATMGR->GetChatMsg( 1727 ) ) ;
   					EndDialog();
   					DisActiveEx();
   					return;
   				}
			}
	
   			MSG_DWORD msg;
   			msg.Category = MP_RECALLNPC;
   			msg.Protocol = MP_RECALLNPC_CTOM_RECALL_SYN;
   			msg.dwObjectID = gHeroID;
   			msg.dwData = hyperLink.dwData;
   			NETWORK->Send(
				&msg,
				sizeof(MSG_DWORD));
	
   			EndDialog() ;
   			DisActiveEx() ;

   		}
   		break;
   	case emLink_SpellAllBuff : 
   		{
			WORD wSiegeState = SIEGEWARFAREMGR->GetState() ;

   			// 공성중에는 버프 시전을 받을 수 없다는 메시지를 출력한다.
			// 081009 LYW --- NpcScriptDialog : 공성 소스 머지
			// 081106 LYW --- NpcScriptDialog : 공성 종료 후, 바로 집행위원의 버프를 받을 수 없는 버그 수정.
   			//if( wSiegeState > eSiegeWarfare_State_Before )//>= eSiegeWarfare_Notify_Begin && wSiegeState < eSiegeWarfare_Notify_End )
			if( wSiegeState > eSiegeWarfare_State_Before && wSiegeState != eSiegeWarfare_State_End )
			{
				WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, CHATMGR->GetChatMsg(1717) ) ;
			}
   			else
   			{
   				MSG_DWORD msg;
   				msg.Category = MP_SIEGEWARFARE;
   				msg.Protocol = MP_SIEGEWARFARE_BUFF_SYN;
   				msg.dwObjectID = gHeroID;
   				msg.dwData = hyperLink.dwData;
   				NETWORK->Send(
					&msg,
					sizeof(MSG_DWORD));
   			}
   
   			EndDialog() ;
   			DisActiveEx() ;
   		}
   		break ;
   
   		// 080904 LYW --- NpcScriptDialog : Add link type to change map by castle flag.
   	case emLink_ChangeMapRecallNpc :
   		{
   			MSG_NPCRECALL_CHANGEMAP msg;
			ZeroMemory(
				&msg,
				sizeof(msg));
   			msg.Category = MP_RECALLNPC;
   			msg.Protocol = MP_RECALLNPC_CTOAM_CHANGEMAP_SYN;
   			msg.dwObjectID = gHeroID;
   			msg.CurMapNum = MAP->GetMapNum();
   			msg.ChangeMapNum = (MAPTYPE)hyperLink.dwData;
   			msg.ChannelNum = (BYTE)gChannelNum;
   			NETWORK->Send(
				&msg,
				sizeof(msg));
   		}
   		break ;
   
   		// 080919 LYW --- NpcScriptDialog : 루쉔 집행위원의 맵이동 창 열기 처리 추가.
   	case emLink_MapMoveList_Rushen :
   		{
			//공성전 중이라면 
			if( SIEGEWARFAREMGR->GetState( ) != eSiegeWarfare_State_Before )
			{
				GAMEIN->GetNpcScriptDialog()->EndDialog();
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1705));
				return;
			}

   			EndDialog() ;
   			DisActiveEx() ;
   
   			cMapMoveDialog* pDlg = NULL ;
   			pDlg = GAMEIN->GetMapMoveDlg() ;
   
   			if( !pDlg )
   			{
   #ifdef _GMTOOL_
   				MessageBox(NULL, "Failed to receive MapMoveListDlg_Rushen.", __FUNCTION__, MB_OK) ;
   				return ;
   #else
   				return ;
   #endif //_GMTOOL_
   			}
   			
   			pDlg->SetUseCase(eCastle_Rushen) ;
   			pDlg->SetActive(TRUE) ;
   		}
   		break ;
   
   		// 080919 LYW --- NpcScriptDialog : 루쉔 집행위원의 맵이동 창 열기 처리 추가.
   	case emLink_MapMoveList_Zevyn :
   		{
			//공성전 중이라면 
			if( SIEGEWARFAREMGR->GetState( ) != eSiegeWarfare_State_Before )
			{
				GAMEIN->GetNpcScriptDialog()->EndDialog();
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1705));
				return;
			}

   			EndDialog() ;
   			DisActiveEx() ;
   
   			cMapMoveDialog* pDlg = NULL ;
   			pDlg = GAMEIN->GetMapMoveDlg() ;
   
   			if( !pDlg )
   			{
   #ifdef _GMTOOL_
   				MessageBox(NULL, "Failed to receive MapMoveListDlg_Zevyn.", __FUNCTION__, MB_OK) ;
   				return ;
   #else
   				return ;
   #endif //_GMTOOL_
   			}
   
   			pDlg->SetUseCase(eCastle_Zevyn) ;
   			pDlg->SetActive(TRUE) ;
   		}
   		break ;
   	case emLink_MoveToDungeon :
   		{
			//공성전 중이라면 
			if( SIEGEWARFAREMGR->GetState( ) != eSiegeWarfare_State_Before )
			{
				GAMEIN->GetNpcScriptDialog()->EndDialog();
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1705));
				return;
			}

   			EndDialog() ;
   			DisActiveEx() ;
   
   			MSGBASE msg ;
   
   			msg.Category	= MP_SIEGEWARFARE ;
   			msg.Protocol	= MP_SIEGEWARFARE_MOVETODUNGEON_SYN ;
   
   			msg.dwObjectID	= HEROID ;
   
   			NETWORK->Send( &msg, sizeof(msg) ) ;
   		}
   		break ;
	}

	MoveCursorToLink();
}

void cNpcScriptDialog::SetActive(BOOL val)
{
	if(FALSE == val)
	{
		if(CNpc* const pNpc = NPCSCRIPTMGR->GetSelectedNpc())
		{			
			if( pNpc->GetNpcJob() == BOMUL_ROLE)										// NPC 직업을 받고, 직업이 보물 롤과 같으면,
			{
				MSG_DWORD msg;															// 메시지 구조체를 선언한다.
				msg.Category = MP_NPC;													// 카테고리를 NPC로 세팅한다.
				msg.Protocol = MP_NPC_CLOSEBOMUL_SYN;									// 프로토콜을 MP_NPC_CLOSEBOMUL_SYN로 세팅한다.
				msg.dwObjectID = gHeroID;											// HERO의 아이디를 세팅한다.
				msg.dwData = NPCSCRIPTMGR->GetSelectedNpc()->GetID();					// 현재 선택 된 NPC 아이디를 세팅한다.
				NETWORK->Send(&msg, sizeof(msg));										// 메시지를 전송한다.
			}
		}

		if(IsActive())
		{
			EndDialog();
		}

		if(CQuestQuickViewDialog* const pQuickDlg = GAMEIN->GetQuestQuickViewDialog())
		{
			pQuickDlg->SetHide( FALSE );
			pQuickDlg->UpdateQuickView();
		}
	}
	else
	{
		CQuestQuickViewDialog* pQuickDlg = GAMEIN->GetQuestQuickViewDialog();
		if( pQuickDlg					&& 
			NULL == pQuickDlg->IsActive() )
		{
			pQuickDlg->SetActive( TRUE );
			pQuickDlg->SetHide( TRUE );
		}
	}

	cDialog::SetActiveRecursive( val );

	m_pQuestBtn->SetActive( FALSE );
}

// 다른곳에서 절대 호출하지 마시오!!
void cNpcScriptDialog::DisActiveEx()													// 비활성화 확장 함수.
{
	m_pListDlg->RemoveAll();

	m_dwCurNpc = 0;
	m_wNpcChxNum = 0;

	NPCSCRIPTMGR->SetSelectedNpc(NULL);
	cDialog::SetActiveRecursive(FALSE);
	WINDOWMGR->SetMouseInputProcessed();
}

void cNpcScriptDialog::RefreshQuestLinkPage()
{
	m_pLinkListDlg->RemoveAll();

	DWORD dwPageId = 0 ;

	if( m_dwMainLinkPageId != 0 )
	{
		dwPageId = m_dwMainLinkPageId ;
	}
	else
	{
		dwPageId = m_dwCurPageId ;
	}

	cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );						// 현재 npc와 페이지 아이디로 페이지 정보를 얻는다.

	if( pPage == NULL ) return;															// 페이지 정보가 유효하지 않으면 리턴 처리를 한다.

	cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();							// 하이퍼 텍스트 정보를 받는다.

	if( pHyper == NULL ) return;														// 하이퍼 텍스트 정보가 유효하지 않으면 리턴 처리를 한다.

	DIALOGUE* temp = NULL;																// 임시 다이얼로그 정보를 받을 포인터 변수를 선언하고 NULL 처리를 한다.

	LINKITEM* pItem = NULL;																// 링크 아이템 정보를 받을 포인터 변수를 선언하고 NULL 처리를 한다.

	HYPERLINK* pLink;																	// 링크 정보를 받을 포인터를 선언한다. // 링크 정보를 가져와서 설정한다.

	int nLinkCount = pPage->GetHyperLinkCount();										// 링크 카운트를 세팅한다.

	pItem = new LINKITEM;																// 링크 아이템 메모리를 할당한다.
	strcpy( pItem->string, CHATMGR->GetChatMsg(239) );									// 아이템 스트링으로 [ 퀘스트 목록 ]을 세팅한다.
	pItem->rgb = RGBA_MAKE	( 170, 236, 4, 255 );											// 아이템 색상을 세팅한다.
	m_pLinkListDlg->cListItem::AddItem( pItem );										// 링크 리스트 다이얼로그에 아이템을 추가한다.

	// 한줄 띄우기
	pItem = new LINKITEM;																// 링크 아이템 메모리를 할당한다.
	strcpy( pItem->string, "" );														// 아이템 스트링을 ""로 세팅한다.
	m_pLinkListDlg->cListItem::AddItem( pItem );										// 현재 아이템을 추가한다.

	BOOL bCheck = FALSE;																// 체크 여부를 FALSE로 세팅한다.

	// 수행가능한 협행
	for( int j = 0; j < nLinkCount; ++j )												// 링크 카운트 만큼 for문을 돌린다.
	{
		pLink = pPage->GetHyperText(j);													// 하이퍼 텍스트 정보를 받는다.

		if( pLink == NULL )																// 하이퍼 텍스트 정보가 유효하지 않으면,
		{
			break;																		// for문을 탈출한다.
		}

		CQuest* pQuest = QUESTMGR->GetQuest( pLink->dwData );							// 퀘스트 정보를 받는다.

		if( pQuest )																	// 퀘스트 정보가 유효하면,
		{
			if( pQuest->IsQuestState( m_dwCurNpc ) != eQuestState_Possible )			// 퀘스트 상태가를 체크한다.
			{
				continue;																// 계속한다.
			}
		}
		else																			// 퀘스트 정보가 유효하지 않으면,
			continue;																		// 계속한다.

		if( !bCheck )																	// 체크 여부가 FALSE와 같으면,
		{		
			LINKITEM* pSimbol = new LINKITEM;
			if( pSimbol == NULL ) break;

			strcpy( pSimbol->string, "!  ");
			pSimbol->rgb = RGBA_MAKE( 157, 246, 254, 255 );
			
			pItem = new LINKITEM;														// 링크 아이템 메모리를 세팅한다.
			strcpy( pItem->string, CHATMGR->GetChatMsg(243) );												// 수행 가능한 퀘스트를 세팅한다.
			pItem->rgb = RGBA_MAKE( 255, 204, 0, 255 );									// 아이템 색상을 세팅한다.

			pSimbol->NextItem = pItem;
			m_pLinkListDlg->cListItem::AddItem( pSimbol );								// 리스트 다이얼로그에 아이템을 추가한다.

			bCheck = TRUE;																// 체크 여부를 TRUE로 세팅한다.
		}

		temp = pHyper->GetHyperText( pLink->wLinkId );									// 하이퍼 텍스트 정보를 받는다.

		if( temp )																		// 정보가 유효하면,
		{
			pItem = new LINKITEM;														// 링크 아이템 메모리를 할당한다.
			strcpy( pItem->string, temp->str );											// 링크 스트링을 아이템 스트링에 세팅한다.
			pItem->dwType = pLink->wLinkType;											// 아이템 타입을 링크 타입으로 세팅한다.
			pItem->dwData = pLink->dwData;
			
			m_pLinkListDlg->cListItem::AddItem( pItem );
		}
	}

	if( bCheck )																		// 체크 여부가 TRUE와 같다면,
	{
		pItem = new LINKITEM;															// 링크 아이템 메모리를 할당한다.
		strcpy( pItem->string, "" );													// 스트링을 ""로 세팅한다.
		m_pLinkListDlg->cListItem::AddItem( pItem );									// 리스트 다이얼로그에 아이템을 추가한다.
	}

	bCheck = FALSE;																		// 체크 여부를 FALSE로 세팅한다.
	// 수행중인 협행

	for( j = 0; j < nLinkCount; ++j )													// 링크 카운트 수 만큼 for문을 돌린다.
	{
		pLink = pPage->GetHyperText(j);													// 하이퍼 링크 정보를 받는다.

		if( pLink == NULL ) break;														// 하이퍼 링크 정보가 유효하지 않으면 for문을 탈출한다.

		CQuest* pQuest = QUESTMGR->GetQuest( pLink->dwData );							// 퀘스트 정보를 받는다.

		temp = pHyper->GetHyperText( pLink->wLinkId	);									// 하이퍼 텍스트 정보를 받는다.
		if( pQuest )																	// 퀘스트 정보가 유효하면,
		{
			if( pQuest->IsQuestState( m_dwCurNpc ) != eQuestState_Executed )			// 퀘스트 상태를 체크한다.
			{
				continue;																// 계속한다.
			}
		}
		else																			// 퀘스트 정보가 유효하지 않으면,
			continue;																	// 계속한다.

		if( !bCheck )																	// 체크여부가 FALSE와 같으면,
		{
			LINKITEM* pSimbol = new LINKITEM;
			if( pSimbol == NULL ) break;

			strcpy( pSimbol->string, "? ");
			pSimbol->rgb = RGBA_MAKE( 157, 246, 254, 255 );

			pItem = new LINKITEM;														// 링크 아이템 메모리를 할당한다.
			strcpy( pItem->string, CHATMGR->GetChatMsg(244) );							// 스트링을 수행중인 퀘스트로 세팅한다.
			pItem->rgb = RGBA_MAKE( 255, 204, 0, 255 );									// 아이템 색상을 세팅한다.

			pSimbol->NextItem = pItem;
			m_pLinkListDlg->cListItem::AddItem( pSimbol );								// 리스트 다이얼로그에 아이템을 추가한다.

			bCheck = TRUE;																// 체크 여부를 TRUE로 세팅한다.
		}

		// 090924 ShinJS --- 완료 가능한 퀘스트인 경우 완료버튼을 활성화시킨다.
		if( pQuest->IsPossibleQuestComplete() )
		{
			m_pQuestBtn->SetText( RESRCMGR->GetMsg( 1159 ), m_pQuestBtn->GetBasicColor() );		// "퀘스트 완료"
			m_pQuestBtn->SetToolTip( RESRCMGR->GetMsg( 1162 ), RGB_HALF(255,255,255) );			// "현재 완료 가능한 퀘스트를 모두 완료합니다"
			m_pQuestBtn->SetActive( TRUE );
			m_pQuestBtn->SetRenderArea();
			m_QuestBtnType = eQuestBtnType_CompleteAll;
		}

		temp = pHyper->GetHyperText( pLink->wLinkId );									// 하이퍼 텍스트 정보를 받는다.

		if( temp )																		// 하이퍼 텍스트 정보가 유효하면,
		{
			pItem = new LINKITEM;														// 링크 아이템 메모리를 할당한다.
			strcpy( pItem->string, temp->str );											// 아이템 스트링을 링크 스트링으로 세팅한다.

			pItem->dwType = pLink->wLinkType;											// 아이템 타입을 링크 타입으로 세팅한다.
			pItem->dwData = pLink->dwData;
			
			m_pLinkListDlg->cListItem::AddItem( pItem );
		}
	}

	if( m_pLinkListDlg->GetItemCount() <= 2 )
	{
		m_pLinkListDlg->RemoveAll() ;
	}

	m_pLinkListDlg->ResetGuageBarPos();
	MoveCursorToLink();
}

// 070917 LYW --- NpcScriptDialog : Add function to refresh page to change map.
void cNpcScriptDialog::RefreshChangeMapLinkPage()
{
	m_pLinkListDlg->RemoveAll();

	DWORD dwPageId = 0 ;

	if( m_dwMainLinkPageId != 0 )
	{
		dwPageId = m_dwMainLinkPageId ;
	}
	else
	{
		dwPageId = m_dwCurPageId ;
	}

	cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );						// 현재 npc와 페이지 아이디로 페이지 정보를 얻는다.

	if( pPage == NULL ) return;															// 페이지 정보가 유효하지 않으면 리턴 처리를 한다.

	cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();							// 하이퍼 텍스트 정보를 받는다.

	if( pHyper == NULL ) return;														// 하이퍼 텍스트 정보가 유효하지 않으면 리턴 처리를 한다.

	DIALOGUE* temp = NULL;																// 임시 다이얼로그 정보를 받을 포인터 변수를 선언하고 NULL 처리를 한다.

	LINKITEM* pItem = NULL;																// 링크 아이템 정보를 받을 포인터 변수를 선언하고 NULL 처리를 한다.

	HYPERLINK* pLink;																	// 링크 정보를 받을 포인터를 선언한다. // 링크 정보를 가져와서 설정한다.

	int nLinkCount = pPage->GetHyperLinkCount();										// 링크 카운트를 세팅한다.

	pItem = new LINKITEM;																// 링크 아이템 메모리를 할당한다.
	strcpy( pItem->string, CHATMGR->GetChatMsg(1472) );										// 아이템 스트링으로 [ 퀘스트 목록 ]을 세팅한다.
	pItem->rgb = RGBA_MAKE( 0, 0, 255, 255 );											// 아이템 색상을 세팅한다.
	m_pLinkListDlg->cListItem::AddItem( pItem );										// 링크 리스트 다이얼로그에 아이템을 추가한다.

	// 한줄 띄우기
	pItem = new LINKITEM;																// 링크 아이템 메모리를 할당한다.
	strcpy( pItem->string, "" );														// 아이템 스트링을 ""로 세팅한다.
	m_pLinkListDlg->cListItem::AddItem( pItem );										// 현재 아이템을 추가한다.

	// 수행가능한 협행
	for( int j = 0; j < nLinkCount; ++j )												// 링크 카운트 만큼 for문을 돌린다.
	{
		pLink = pPage->GetHyperText(j);													// 하이퍼 텍스트 정보를 받는다.

		if( pLink == NULL )																// 하이퍼 텍스트 정보가 유효하지 않으면,
		{
			break;																		// for문을 탈출한다.
		}

		temp = pHyper->GetHyperText( pLink->wLinkId );									// 하이퍼 텍스트 정보를 받는다.

		if( temp )																		// 정보가 유효하면,
		{
			pItem = new LINKITEM;														// 링크 아이템 메모리를 할당한다.
			strcpy( pItem->string, temp->str );											// 링크 스트링을 아이템 스트링에 세팅한다.
			pItem->dwType = pLink->wLinkType;											// 아이템 타입을 링크 타입으로 세팅한다.
			pItem->dwData = pLink->dwData;
			
			m_pLinkListDlg->cListItem::AddItem( pItem );
		}
	}

	m_pLinkListDlg->ResetGuageBarPos() ;												// 링크 리스트의 게이지바 위치를 다시 잡는다.
}


void cNpcScriptDialog::OpenQuestLinkPage( DWORD dwPageId )								// 퀘스트 링크 페이지를 여는 함수.
{
	m_dwCurPageId = dwPageId;															// 페이지 정보를 세이브 한다.

	RefreshQuestLinkPage() ;															// 퀘스트 링크 리스트를 새로고침 한다.
}

// 070917 LYW --- NpcScriptDialog : Add function to open page to change map.
void cNpcScriptDialog::OpenChangeMapLinkPage( DWORD dwPageId )							// 맵 변경 링크 페이지를 여는 함수.
{
	m_dwCurPageId = dwPageId;															// 페이지 정보를 세이브 한다.

	//RefreshQuestLinkPage() ;															// 퀘스트 링크 리스트를 새로고침 한다.
	RefreshChangeMapLinkPage() ;
}

void cNpcScriptDialog::OpenLastPageOfQuest()
{
	// 현재 선택된 퀘스트를 구한다.
	CQuest* pQuest = QUESTMGR->GetQuest( m_dwQuestIdx );
	if( !pQuest )
		return;

	// 퀘스트의 Page를 구한다.
	DWORD dwPageId = pQuest->GetNpcScriptPage( m_dwCurNpc );
	cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );
	if( !pPage )
		return;

	int nHyperCnt = pPage->GetHyperLinkCount();
	if( nHyperCnt <= 0 || nHyperCnt > 1 )
		return;

	// 퀘스트의 마지막 Page를 찾는다.
	while( nHyperCnt > 0 )
	{
		pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );
		if( !pPage )
			return;

		HYPERLINK* pLink = pPage->GetHyperText( 0 );
		if( !pLink )
			return;

		dwPageId = pLink->dwData;
		cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );
		if( !pPage )
			return;

		nHyperCnt = pPage->GetHyperLinkCount();
	}

	// 마지막 Page Open
	OpenLinkPage( dwPageId );
}

void cNpcScriptDialog::MoveCursorToLink()
{
	cListDialogEx* const listDialog = (m_pRequitalInfoDlg->IsActive() ? m_pListDlg : m_pLinkListDlg);
	int selectedRow = listDialog->GetCurSelectedRowIdx() + 1;

	if(listDialog->GetTopListItemIdx() + listDialog->GetVisibleLineSize() <= selectedRow)
	{
		selectedRow	= 0;
	}

	int firstLinkedRow = INT_MAX;

	for(int i = 0; i < listDialog->GetVisibleLineSize(); ++i)
	{
		const int row = listDialog->GetTopListItemIdx() + i;
		const LINKITEM* const item = (LINKITEM*)listDialog->GetItem(
			row);

		if(0 == item)
		{
			break;
		}
		else if(emLink_Null == item->dwType)
		{
			continue;
		}

		firstLinkedRow = min(
			firstLinkedRow,
			row);
		const int linkedRow = row;

		if(selectedRow == linkedRow)
		{
			break;
		}
		else if(selectedRow < linkedRow)
		{
			selectedRow = linkedRow;
			break;
		}
	}

	const LINKITEM* listItem = (LINKITEM*)listDialog->GetItem(
		selectedRow);

	if(0 == listItem ||
		emLink_Null == listItem->dwType)
	{
		listItem = (LINKITEM*)listDialog->GetItem(
			firstLinkedRow);

		if(0 == listItem)
		{
			return;
		}

		selectedRow = firstLinkedRow;
	}

	POINT point = {LONG(listDialog->GetAbsX()), LONG(listDialog->GetAbsY())};
	ClientToScreen(
		_g_hWnd,
		&point);

	const LONG textWidth = CFONT_OBJ->GetTextExtentEx(
		listDialog->GetFontIdx(),
		LPTSTR(listItem->string),
		_tcslen(listItem->string));
	SetCursorPos(
		point.x + textWidth,
		point.y + (selectedRow - listDialog->GetTopListItemIdx()) * listDialog->GetLineHeight() + listDialog->GetLineHeight() / 2);
}

DWORD cNpcScriptDialog::ActionKeyboardEvent(CKeyboard* keyboard)
{
	if(keyboard->GetKeyDown(KEY_TAB))
	{
		MoveCursorToLink();
	}
	else if(keyboard->GetKeyDown(KEY_SPACE))
	{
		if(m_pRequitalInfoDlg->IsActive())
		{
			if(ITEM* const item = m_pListDlg->GetItem(m_pListDlg->GetCurSelectedRowIdx()))
			{
				HyperLinkParser(
					item->string, 
					item->dwData);
			}
		}
		else
		{
			if(ITEM* const item = m_pLinkListDlg->GetItem(m_pLinkListDlg->GetCurSelectedRowIdx()))
			{
				HyperLinkParser(
					item->string,
					item->dwData);
			}
		}
	}

	return WE_NULL;
}

void cNpcScriptDialog::Render()														
{
	cDialog::Render();																
	if( !m_pRequitalInfoDlg->IsActive() ) return;

	int Count = 0;

	PTRLISTSEARCHSTART(m_QuestFixRequitalList, RequitalItem*, pSQuest) ;
		if( !pSQuest )			break ;

		if( pSQuest->nItemCount > 1 )
		{
			static char nums[4] ;
			int absX=0; int absY=0;

			wsprintf(nums, "%3d", pSQuest->nItemCount) ;

			if( m_pFixRequitalGrid->GetCellAbsPos(pSQuest->wPos, absX, absY))
			{
				// 070209 LYW --- QuestDialog : Modified render part.
				m_renderRect.left	= (LONG)absX+16 ;				
				m_renderRect.top	= (LONG)absY+24 ;
				m_renderRect.right	= 1 ;
				m_renderRect.bottom	= 1 ;
																	
				CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_renderRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
			}
		}
		++Count;														
		if(Count >= MAX_FIX_REQUITAL_ICON_CELL)		break;	
	PTRLISTSEARCHEND

	// 선택 보상아이템의 갯수를 출력한다.
	Count	= 0;
	PTRLISTSEARCHSTART(m_QuestSelectRequitalList, RequitalItem*, pSQuest);
		if(!pSQuest)				break;								

		if(pSQuest->nItemCount > 1)									
		{
			static char nums[4];
			int absX=0;	int absY=0;									

			wsprintf(nums,"%3d", pSQuest->nItemCount);				

			if(m_pSelectRequitalGrid->GetCellAbsPos(pSQuest->wPos, absX, absY))
			{
				// 070209 LYW --- QuestDialog : Modified render part.
				m_renderRect.left	= (LONG)absX+16 ;						
				m_renderRect.top	= (LONG)absY+24 ;
				m_renderRect.right	= 1 ;
				m_renderRect.bottom	= 1 ;
																			
				CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_renderRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
			}
		}
		++Count;
		if(Count >= MAX_SELECT_REQUITAL_ICON_CELL)		break;	
	PTRLISTSEARCHEND		

}

// 100412 ONS 보상아이템 페이지를 연다.
void cNpcScriptDialog::OpenRequitalPage( DWORD dwQuestIdx )
{
	// 퀘스트 제목 스트링을 받는다.
	CQuestString* pTitleString = QUESTMGR->GetQuestString(dwQuestIdx, 0) ;
	if( !pTitleString )
	{
		return ;
	}

	char tempBuf[128] = {0, } ;
	PTRLISTPOS pos2 = pTitleString->GetTitle()->GetHeadPosition();				
	while(pos2)																	
	{
		ITEM* pItem = (ITEM*)pTitleString->GetTitle()->GetNext(pos2);				
		if(!pItem) continue ;														

		sprintf(tempBuf, "%s", pItem->string) ;									
	}	
	m_pRequitalQuestName->SetStaticText(tempBuf);

	m_pRequitalGold->SetStaticText("0");
	m_pRequitalExp->SetStaticText("0");
	m_pSelectRequitalGrid->SetCurSelCellPos(-1);
	m_RequitalIndex = 0;
	m_RequitalCount = 0;
	
	PTRLISTPOS deletePos = m_QuestFixRequitalList.GetHeadPosition() ;
	while(deletePos)
	{
		RequitalItem* pDeleteItem = (RequitalItem*)m_QuestFixRequitalList.GetNext(deletePos) ;
		if( !pDeleteItem ) continue ;

	    m_pFixRequitalGrid->DeleteIcon( pDeleteItem->wPos, &pDeleteItem->pIcon ) ;
		pDeleteItem->pIcon->SetActive(FALSE) ;
		WINDOWMGR->AddListDestroyWindow(pDeleteItem->pIcon) ;
		m_IndexGenerator.ReleaseIndex( pDeleteItem->pIcon->GetID() ) ;
		m_QuestFixRequitalList.Remove(pDeleteItem) ;
		delete pDeleteItem ;
	}
	m_QuestFixRequitalList.RemoveAll() ;

	deletePos = m_QuestSelectRequitalList.GetHeadPosition() ;
	while(deletePos)
	{
		RequitalItem* pDeleteItem = (RequitalItem*)m_QuestSelectRequitalList.GetNext(deletePos) ;
		if( !pDeleteItem ) continue ;

	    m_pSelectRequitalGrid->DeleteIcon( pDeleteItem->wPos, &pDeleteItem->pIcon ) ;
		pDeleteItem->pIcon->SetActive(FALSE) ;
		WINDOWMGR->AddListDestroyWindow(pDeleteItem->pIcon) ;
		m_IndexGenerator.ReleaseIndex( pDeleteItem->pIcon->GetID() ) ;
		m_QuestSelectRequitalList.Remove(pDeleteItem) ;
		delete pDeleteItem ;
	}
	m_QuestSelectRequitalList.RemoveAll() ;

	const cPtrList* pTriggerList = QUESTMGR->GetLastSubQuestTrigger( dwQuestIdx );
	if( !pTriggerList ) return;
	
	int nTriggerCount = pTriggerList->GetCount() ;							
	for( int count = 0 ; count < nTriggerCount ; ++count )
	{
		PTRLISTPOS pos = pTriggerList->FindIndex(count) ;
		if( !pos ) continue ;							
		
		// 해당 트리거 정보를 받는다.
		CQuestTrigger* pTrigger = (CQuestTrigger*)pTriggerList->GetAt(pos) ;			
		if( !pTrigger )	 continue ;	

		// 트리거 실행 리스트를 받는다.
		cPtrList* pExeList = pTrigger->GetExeList() ;								
		if( !pExeList )	continue ;

		int nExeCount = pExeList->GetCount() ;
		for(int count2 = 0 ; count2 < nExeCount ; ++count2 )
		{
			PTRLISTPOS exePos = pExeList->FindIndex(count2) ;									
			if( !exePos ) continue ;

			// 퀘스트 실행 정보를 받는다.
			CQuestExecute* pExecute = (CQuestExecute*)pExeList->GetAt(exePos) ;	
			if( !pExecute ) continue ;

			CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute) ;
			DWORD dwExeKind = pExecute->GetQuestExecuteKind() ;

			switch(dwExeKind)
			{
			case eQuestExecute_TakeItem :				// 실행 타입이 아이템을 받는 타입이라면,
				{
					ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(pExecute_Item->GetItemKind());
					if( pItemInfo )
					{
						AddRequitalItem( pItemInfo, pExecute_Item->GetItemNum(), BYTE(eQuestExecute_TakeItem) );
						RefreshRequitalGrid( BYTE(eQuestExecute_TakeItem) );
					}
				}
				break ;
				
			case eQuestExecute_TakeSelectItem:
				{
					CQuestExecute_SelectItem* pExecute_selectItem = ((CQuestExecute_SelectItem*)pExecute) ;

					DWORD dwMaxCount = pExecute_selectItem->GetMaxItemCount();
					for( DWORD i = 0; i < dwMaxCount; i++ )
					{
						ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(pExecute_selectItem->GetItemKind(i));
						if( !pItemInfo ) continue;

						AddRequitalItem( pItemInfo, pExecute_selectItem->GetItemNum(i), BYTE(eQuestExecute_TakeSelectItem) );
					}
					RefreshRequitalGrid( BYTE(eQuestExecute_TakeSelectItem) );
				}
				break;

			case eQuestExecute_TakeMoney :				// 실행 타입이 골드를 받는 타입이라면,
				{
					m_pRequitalGold->SetStaticText( AddComma(pExecute_Item->GetItemNum()) );
				}
				break ;

			case eQuestExecute_TakeExp :				// 실행 타입이 경험치를 받는 타입이라면,
				{
					m_pRequitalExp->SetStaticText( AddComma(pExecute_Item->GetItemNum()) );
				}
				break ;
			}
		}
	}
}

// 보상아이템 그리드를 갱신한다.
void cNpcScriptDialog::RefreshRequitalGrid( BYTE bRequitalType )
{
	cIconGridDialog*	pRequitalGrid = NULL;
	cPtrList*			pRequitalList = NULL;
	DWORD				dwMaxCellNum = 0;

	if( bRequitalType == eQuestExecute_TakeItem )
	{
		pRequitalList = &m_QuestFixRequitalList;
		pRequitalGrid = m_pFixRequitalGrid;
		dwMaxCellNum = MAX_FIX_REQUITAL_ICON_CELL;
	}
	else if( bRequitalType == eQuestExecute_TakeSelectItem )
	{
		pRequitalList = &m_QuestSelectRequitalList;
		pRequitalGrid = m_pSelectRequitalGrid;
		dwMaxCellNum = MAX_SELECT_REQUITAL_ICON_CELL;
	}
	else
	{
		return;
	}

	DWORD Count = 0;	
	WORD Pos = 0;	
	PTRLISTPOS pos = pRequitalList->GetHeadPosition() ;
	while(pos)
	{
		RequitalItem* pItem = (RequitalItem*)pRequitalList->GetNext(pos) ;
		if( pItem )
		{
			if( Count >= 0 && Count < dwMaxCellNum )
			{
				pItem->wPos = Pos ;
				pRequitalGrid->AddIcon( pItem->wPos, pItem->pIcon ) ;
				pItem->pIcon->SetActive(TRUE) ;
				++Pos ;
			}
			else
			{
				pItem->wPos = WORD(-1);
				pItem->pIcon->SetActive(FALSE) ;
			}
			++Count ;
		}
	}				
}

// 보상아이템을 리스트에 추가한다.
void cNpcScriptDialog::AddRequitalItem( ITEM_INFO* pItemInfo, DWORD Count, BYTE bType )
{
	if( !pItemInfo ) return ;

	RequitalItem* pSQuest = new RequitalItem;										// 퀘스트 아이템 메모리를 할당한다.
	memcpy(&pSQuest->itemInfo, pItemInfo, sizeof(ITEM_INFO)) ;						// 퀘스트 아이템 정보에 인자로 넘어온 정보를 세팅한다.
	pSQuest->nItemCount = (int)Count;												// 퀘스트 아이템의 카운트를 세팅한다.
	pSQuest->wPos = 0;

	cImage Image;																	// 이미지를 선언한다.
	SCRIPTMGR->GetImage(pItemInfo->Image2DNum, &Image, PFT_ITEMPATH);				// 퀘스트 아이템 정보의 이미지로 이미지를 세팅한다.

	cIcon* pIcon = new cIcon;														// 아이콘 메모리를 할당한다.
	pIcon->SetBasicImage(&Image);													// 아이콘의 기본 이미지로 위에서 세팅한 이미지를 설정한다.
	pIcon->Init(0, 0, 40, 40, &Image, m_IndexGenerator.GenerateIndex());			// 아이콘을 초기화 한다.
	pIcon->SetMovable(FALSE);														// 이동 여부를 FALSE로 세팅한다.
	
	pSQuest->pIcon = pIcon ;
	
	if( bType == eQuestExecute_TakeItem )
		m_QuestFixRequitalList.AddTail(pSQuest);
	else
		m_QuestSelectRequitalList.AddTail(pSQuest);

	WINDOWMGR->AddWindow(pIcon);													// 윈도우 매니져에 아이콘 정보를 추가한다.
}

// 보상아이템툴팁을 표시한다.
void cNpcScriptDialog::ShowRequitalItemToolTip( cPtrList& RequitalList, DWORD Index )
{
	char buf[64] = { 0, };															
	DWORD dwCount = 0;																
	PTRLISTSEARCHSTART(RequitalList, RequitalItem*, pSQuest);						
		if(!pSQuest)			break;												

		if(dwCount == Index)														
		{
			cImage Image;															
			SCRIPTMGR->GetImage(0, &Image, PFT_HARDPATH);							
			pSQuest->pIcon->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &Image, TTCLR_ITEM_CANEQUIP );

			wsprintf(buf, "[%s]", pSQuest->itemInfo.ItemName);
			pSQuest->pIcon->AddToolTipLine(buf, ITEMMGR->GetItemNameColor( pSQuest->itemInfo ));
			
			ITEMMGR->AddToolTip( pSQuest->pIcon, pSQuest->itemInfo.ItemTooltipIdx );
		}

		++dwCount;																	

	PTRLISTSEARCHEND																
}

// 선택보상아이템이 선택되었는지 여부를 판단한다.
BOOL cNpcScriptDialog::IsRequitalSelected( DWORD dwQuestIdx )
{
	CQuest* pQuest = QUESTMGR->GetQuest( dwQuestIdx );
	if( !pQuest ) return TRUE;

	// 퀘스트를 수락할때는 처리하지 않는다.
	if( pQuest->IsQuestState( m_dwCurNpc ) != eQuestState_Executed	&& !pQuest->IsPossibleQuestComplete() )
		return TRUE;

	m_RequitalIndex = 0;
	m_RequitalCount = 0;

	DWORD dwCount = 0;

	PTRLISTSEARCHSTART(m_QuestSelectRequitalList, RequitalItem*, pSQuest);
		if(!pSQuest)			break;									

		// 선택보상아이템 그리드로부터 선택한 보상아이템의 인덱스와 개수를 가져온다.
		if(dwCount == (DWORD)m_pSelectRequitalGrid->GetCurSelCellPos())	
		{
			m_RequitalIndex = pSQuest->itemInfo.ItemIdx;
			m_RequitalCount = pSQuest->nItemCount;
			break;
		}
		++dwCount;	
	PTRLISTSEARCHEND

	// 선택하지 않은 상태
	if( m_QuestSelectRequitalList.GetCount() > 0 &&	m_RequitalIndex == 0 )											
	{
		return FALSE;
	}

	return TRUE;
}

// 해당 퀘스트가 선택보상아이템을 지급하는지 여부를 판단한다.
BOOL cNpcScriptDialog::CanTakeSelectRequital( DWORD dwQuestIdx )
{
	const cPtrList* pTriggerList = QUESTMGR->GetLastSubQuestTrigger( dwQuestIdx );
	if( !pTriggerList ) return FALSE;

	int nTriggerCount = pTriggerList->GetCount();		

	for( int count = 0; count < nTriggerCount; ++count )
	{
		PTRLISTPOS pos = pTriggerList->FindIndex(count);
		if( !pos ) continue;							

		CQuestTrigger* pTrigger = (CQuestTrigger*)pTriggerList->GetAt(pos);
		if( !pTrigger )	 continue;

		cPtrList* pExeList = pTrigger->GetExeList();
		if( !pExeList )	continue;

		int nExeCount = pExeList->GetCount();
		for( int count2 = 0; count2 < nExeCount; ++count2 )
		{
			PTRLISTPOS exePos = pExeList->FindIndex(count2);
			if( !exePos ) continue;

			CQuestExecute* pExecute = (CQuestExecute*)pExeList->GetAt(exePos);
			if( !pExecute ) continue;

			DWORD dwExeKind = pExecute->GetQuestExecuteKind();
			if( dwExeKind == eQuestExecute_TakeSelectItem )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

WORD cNpcScriptDialog::GetConversationListFontIdx() const
{
	return m_pListDlg->GetFontIdx();
}

LONG cNpcScriptDialog::GetConversationListTextWidth() const
{
	return m_pListDlg->GetTextWidth();
}

const HYPERLINK& cNpcScriptDialog::GetHyperLink(LPCTSTR text,DWORD data)
{
	cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();

	typedef std::list< cPage* > PageContainer;
	PageContainer pageContainer;
	pageContainer.push_back(
		NPCSCRIPTMGR->GetPage(m_dwCurNpc, m_dwCurPageId));
	pageContainer.push_back(
		NPCSCRIPTMGR->GetMainPage(m_dwCurNpc));

	for(PageContainer::const_iterator iterator = pageContainer.begin();
		pageContainer.end() != iterator;
		++iterator)
	{
		cPage* const page = *iterator;

		if(0 == page)
		{
			continue;
		}

		for(int i = 0; i < page->GetHyperLinkCount(); ++i)
		{
			const HYPERLINK* const hyperlink = page->GetHyperText(
				i);

			if(0 == hyperlink )
			{
				break;
			}

			if( hyperlink->dwData != data)
			{
				continue;
			}

			const DIALOGUE* const dialogue = pHyper->GetHyperText(hyperlink->wLinkId);

			if(0 == dialogue)
			{
				continue;
			}

			if(0 == _tcscmp(dialogue->str, text))
			{
				return *hyperlink;
			}
		}
	}

	static HYPERLINK emptyHyperLink = {0};

	return emptyHyperLink;
}
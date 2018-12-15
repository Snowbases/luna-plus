#include "stdafx.h"
#include "NpcNoticeDlg.h"
#include "WindowIDEnum.h"
#include "NpcScriptManager.h"
#include "NpcImageDlg.h"
#include "cDialogueList.h"
#include "cPage.h"
#include "GameIn.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"

cNpcNoticeDlg::cNpcNoticeDlg()
{
	m_pListDlg = NULL;
}

cNpcNoticeDlg::~cNpcNoticeDlg()
{
}

void cNpcNoticeDlg::Linking()
{
	m_pListDlg = (cListDialogEx*)this->GetWindowForID( NND_LISTDLG );
}

BOOL cNpcNoticeDlg::OpenDialog( DWORD dwNpcId, WORD npcChxNum )
{
	cPage* pMainPage = NPCSCRIPTMGR->GetMainPage( dwNpcId );
	if( pMainPage == NULL ) return FALSE;

	cDialogueList* pList = NPCSCRIPTMGR->GetDialogueList();
	if( pList == NULL ) return FALSE;

	DWORD dwMsg = pMainPage->GetRandomDialogue();

	m_pListDlg->RemoveAll();															// 대사 리스트를 모두 비운다.

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

	GAMEIN->GetNpcImageDlg()->SetNpc( npcChxNum );										// NPC 이미지 다이얼로그에 현재 NPC를 세팅한다.
 	GAMEIN->GetNpcImageDlg()->SetEmotion( pMainPage->GetEmotion() );					// NPC의 이모션을 세팅한다.
	GAMEIN->GetNpcImageDlg()->SetActive( TRUE );										// NPC 이미지 다이얼로그를 활성화 한다.
	GAMEIN->GetNpcImageDlg()->SetDisable( TRUE );										// NPC 이미지 다이얼로그를 디스에이블 시킨다.

	SetActive( TRUE );

	return TRUE;
}

void cNpcNoticeDlg::SetActive( BOOL val )
{
	if( !val )
	{
		BOOL chk = FALSE;

		chk = ( chk || ( ( cDialog* )GAMEIN->GetStorageDialog() )->IsActive() );
		chk = ( chk || ( ( cDialog* )GAMEIN->GetDealDialog() )->IsActive() );
		chk = ( chk || ( ( cDialog* )GAMEIN->GetGuildWarehouseDlg() )->IsActive() );

		if( !chk )
		{
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);						// 오브젝트 상태를 종료한다.
		}

		NPCSCRIPTMGR->SetSelectedNpc( NULL );												// 선택한 NPC를 해제한다.

		m_pListDlg->RemoveAll();															// 리스트 다이얼로그를 모두 비운다.

		GAMEIN->GetNpcImageDlg()->SetDisable( FALSE );										// NPC 이미지 다이얼로그의 Disable을 해제한다.
		GAMEIN->GetNpcImageDlg()->SetActive( FALSE );										// NPC 이미지 다이얼로그를 비활성화 한다.
	}
	else
	{
	}

	cDialog::SetActiveRecursive( val );
}
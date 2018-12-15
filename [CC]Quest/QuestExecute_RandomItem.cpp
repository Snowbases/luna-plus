// QuestExecute_RandomItem.cpp: implementation of the CQuestExecute_RandomItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																		// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestExecute_RandomItem.h"													// 퀘스트 실행 랜덤 아이템 헤더를 불러온다.

#include "QuestScriptLoader.h"															// 퀘스트 스크립트 로더 헤더를 불러온다.

#ifdef _MAPSERVER_																		// 맵 서버가 선언되어 있다면,

#include "QuestGroup.h"																	// 퀘스트 그룹 헤더를 불러온다.

#include "ItemSlot.h"

#include "Player.h"

#include "ItemManager.h"

#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 생성자 함수.
CQuestExecute_RandomItem::CQuestExecute_RandomItem( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: CQuestExecute( dwExecuteKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	GetScriptParam( m_dwMaxItemCount, pTokens );										// 아이템 최대 수를 받는다.
	GetScriptParam( m_dwRandomItemCount, pTokens );										// 랜덤 아이템 카운트를 받는다.
	
	m_pRandomItem = new RANDOMITEM[m_dwMaxItemCount];									// 최대 수 만큼 랜덤 아이템 정보를 담을 메모리를 할당하여 포인터로 받는다.

	for( DWORD i = 0; i < m_dwMaxItemCount; ++i )										// 최대 아이템 수 만큼 for물을 돌린다.
	{
		GetScriptParam( m_pRandomItem[i].wItemIdx, pTokens );							// 아이템 인덱스를 받는다.
		GetScriptParam( m_pRandomItem[i].wItemNum, pTokens );							// 아이템 개수를 받는다.
		GetScriptParam( m_pRandomItem[i].wPercent, pTokens );							// 퍼센트를 받는다.
	}

	m_dwQuestIdx = dwQuestIdx;
	m_dwSubQuestIdx = dwSubQuestIdx;
}

CQuestExecute_RandomItem::~CQuestExecute_RandomItem()									// 소멸자 함수.
{
	if( m_pRandomItem )																	// 랜덤 아이템 정보를 나타내는 포인터 정보가 유효하면,
	{
		delete [] m_pRandomItem;														// 랜덤 아이템 정보를 삭제한다.
		m_pRandomItem = NULL;															// 정보를 나타내는 포인터를 NULL 처리를 한다.
	}
}

// 실행 함수.
BOOL CQuestExecute_RandomItem::Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_																		// 맵 서버가 선언되어 있다면,

	switch( m_dwExecuteKind )															// 실행 종류를 확인한다.
	{
	case eQuestExecute_RandomTakeItem:													// 랜덤 아이템을 받는 실행이면,
		{
			cPtrList* pRandomItemList = NULL ;											// 랜덤 아이템 리스트 정보를 받을 포인터를 선언한다.
			pRandomItemList = pQuestGroup->GetRandomItemList() ;						// 퀘스트 그룹의 랜덤 아이템 리스트를 받는다.

			if( !pRandomItemList ) return FALSE ;

			RANDOMITEM* pItem ;

			PTRLISTPOS pos = NULL ;
			pos = pRandomItemList->GetHeadPosition() ;

			while(pos)
			{
				pItem = NULL ;
				pItem = (RANDOMITEM*)pRandomItemList->GetNext(pos) ;

				if( !pItem ) continue ;
		
				pQuestGroup->TakeItem( pPlayer, m_dwQuestIdx, m_dwSubQuestIdx, pItem->wItemIdx, pItem->wItemNum, MAX_QUEST_PROBABILITY );
			}

			pQuestGroup->ReleaseRandomItemList() ;										// 현재 퀘스트 그룹의 랜덤 아이템 리스트를 모두 비운다.
		}
		break;
	}

#endif	

	return TRUE;																		// TRUE를 리턴한다.
}

int CQuestExecute_RandomItem::CheckCondition( PLAYERTYPE* pPlayer,				
							CQuestGroup* pQuestGroup, CQuest* pQuest )					
{
	int nErrorCode = e_EXC_ERROR_NO_ERROR ;												// 에러 코드를 담을 변수를 선언하고 e_EXE_ERROR_NO_ERROR로 세팅한다.

	if( !pPlayer )																		// 플레이어 정보가 유효하지 않으면, 
	{
		nErrorCode = e_EXC_ERROR_NO_PLAYERINFO ;										// 플레이어 정보가 없다는 에러 코드를 세팅한다.
		return nErrorCode ;																// 에러 코드를 return 처리를 한다.
	}

	if( !pQuestGroup )																	// 퀘스트 그룹 정보가 유효하지 않으면,
	{
		nErrorCode = e_EXC_ERROR_NO_QUESTGROUP ;										// 퀘스트 그룹 정보가 없다는 에러 코드를 세팅한다.
		return nErrorCode ;																// 에러 코드를 return 처리를 한다.
	}

	if( !pQuest )																		// 퀘스트 정보가 유효하지 않다면,
	{
		nErrorCode = e_EXC_ERROR_NO_QUEST ;												// 퀘스트 정보가 없다는 에러 코드를 세팅한다.
		return nErrorCode ;																// 에러 코드를 return 처리를 한다.
	}

#ifdef _MAPSERVER_																		// 맵 서버가 선언되어 있다면,

	switch( m_dwExecuteKind )															// 실행 종류를 확인한다.
	{
	case eQuestExecute_RandomTakeItem:													// 랜덤 아이템을 받는 실행이면,
		{
			RANDOMITEM* pItem = NULL;													// 랜덤 아이템 정보를 담을 포인터를 선언하고 NULL 처리를 한다.

			CItemSlot* pSlot ;															// 인벤토리 슬롯 정보를 받을 포인터를 선언한다.

			cPtrList* pRandomItemList = NULL ;											// 랜덤 아이템 리스트 정보를 받을 포인터를 선언한다.
			pRandomItemList = pQuestGroup->GetRandomItemList() ;						// 퀘스트 그룹의 랜덤 아이템 리스트를 받는다.

			if( !pRandomItemList )
			{
				nErrorCode = e_EXC_ERROR_NO_RANDOMITEMLIST ;							// 퀘스트 그룹의 랜덤 아이템 리스트가 없다는 에러를 세팅한다.
				return nErrorCode ;														// 에러 코드를 return 처리를 한다.
			}

			pQuestGroup->ReleaseRandomItemList() ;										// 현재 퀘스트 그룹의 랜덤 아이템 리스트를 모두 비운다.

			for( DWORD i = 0; i < m_dwRandomItemCount; ++i )							// 최대 아이템 수 만큼 for문을 돌린다.
			{
				pItem = NULL ;
				pItem = GetRandomItem();												// 랜덤 아이템 정보를 받는다.

				if( ! pItem ) continue ;												// 정보가 유효하지 않으면 continue 처리를 한다.

				pSlot = NULL ;															// 아이템 슬롯 정보를 받을 포인터를 선언하고 null 처리를 한다.
				pSlot = pPlayer->GetSlot(eItemTable_Inventory) ;						// 플레이어의 인벤토리 스롯 정보를 받는다.

				if( !pSlot )															// 인벤토리 정보가 유효하지 않으면,
				{
					nErrorCode = e_EXC_ERROR_NO_INVENTORYINFO ;							// 인벤토리 정보가 없다는 에러코드를 세팅한다.
					return nErrorCode ;													// 에러 코드를 return 처리를 한다.

					//pQuestGroup->ReleaseRandomItemList() ;								// 현재 퀘스트 그룹의 랜덤 아이템 리스트를 모두 비운다.
				}

				WORD EmptyCellPos[255];													// 빈 슬롯 위치를 받을 변수를 선언한다.
				WORD EmptyCellNum;														// 빈 슬롯 개수를 받을 변수를 선언한다.

				WORD wResult = 0 ;														// 빈 슬롯의 체크 결과 값을 받을 변수를 선언하고 0으로 세팅한다.

				wResult = ITEMMGR->CheckExtraSlot(pPlayer, pSlot, pItem->wItemIdx,		// 아이템을 받을 공간이 충분한지 결과 값을 받는다.
								pItem->wItemNum, EmptyCellPos, EmptyCellNum) ;

				if( wResult == 0 )														// 결과 값이 0과 같으면,
				{
					nErrorCode = e_EXC_ERROR_NO_INVENTORYINFO_RANDOMITEM ;				// 렌덤 아이템을 위한 인벤토리가 부족하다는 처리를 한다.
					return nErrorCode ;													// 에러 코드를 return 처리를 한다.

					//pQuestGroup->ReleaseRandomItemList() ;								// 현재 퀘스트 그룹의 랜덤 아이템 리스트를 모두 비운다.
				}

				RANDOMITEM* pRandomItem = new RANDOMITEM ;
				pRandomItem->wItemIdx = pItem->wItemIdx ;
				pRandomItem->wItemNum = pItem->wItemNum ;
				pRandomItemList->AddTail(pRandomItem) ;									// 랜덤으로 생성된 아이템 정보를 추가한다.
			}
		}
		break;
	}

#endif //_MAPSERVER_

	return nErrorCode ;																	// 기본 값을 리턴한다.
}


RANDOMITEM* CQuestExecute_RandomItem::GetRandomItem()									// 랜덤 아이템 정보를 받환하는 함수.
{
	WORD RandRate = WORD( rand()%10001 );												// 랜덤률을 뽑는다.
	WORD FromPercent = 0;																// From 퍼센트 변수를 선언하고 0으로 세팅한다.
	WORD ToPercent = 0;																	// To 퍼센트 변수를 선언하고 0으로 세팅한다.

	for( DWORD i = 0; i < m_dwMaxItemCount; ++i )										// 최대 아이템 수 만큼 for문을 돌린다.
	{
		FromPercent = ToPercent;														// From 퍼센트를 To 퍼센트로 세팅한다.

		ToPercent = WORD( ToPercent + m_pRandomItem[i].wPercent );						// To 퍼센트에 랜덤 퍼센트를 더한다.

		if( FromPercent <= RandRate && RandRate < ToPercent )							// From 퍼센트가 랜덤률 이하이고, 랜덤률이 To퍼센트 보다 작으면,
		{
			return &m_pRandomItem[i];													// 랜덤 아이템 정보를 리턴한다.
		}
	}

	return NULL;																		// NULL을 리턴한다.
}

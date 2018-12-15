// QuestExecute_RandomItem.cpp: implementation of the CQuestExecute_RandomItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																		// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestExecute_SelectItem.h"													// 퀘스트 실행 랜덤 아이템 헤더를 불러온다.

#include "QuestScriptLoader.h"															// 퀘스트 스크립트 로더 헤더를 불러온다.

#ifdef _MAPSERVER_																		// 맵 서버가 선언되어 있다면,

#include "QuestGroup.h"																	// 퀘스트 그룹 헤더를 불러온다.

#include "ItemSlot.h"

#include "Player.h"

#include "ItemManager.h"

#include "Quest.h"																// 퀘스트 헤더를 불러온다.

#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 생성자 함수.
CQuestExecute_SelectItem::CQuestExecute_SelectItem( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: CQuestExecute( dwExecuteKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	GetScriptParam( m_dwMaxItemCount, pTokens );										// 아이템 최대 수를 받는다.

	m_pSelectItem = new SELECTITEM[m_dwMaxItemCount];

	m_dwQuestIdx = dwQuestIdx;
	m_dwSubQuestIdx = dwSubQuestIdx;

	for( DWORD i = 0; i < m_dwMaxItemCount; ++i )										// 최대 아이템 수 만큼 for물을 돌린다.
	{
		GetScriptParam( m_pSelectItem[i].dwItemIdx, pTokens );							// 아이템 종류를 받는다.
		GetScriptParam( m_pSelectItem[i].dwItemNum, pTokens );							// 아이템 수를 받는다.
	}
}

CQuestExecute_SelectItem::~CQuestExecute_SelectItem()									// 소멸자 함수.
{
	SAFE_DELETE_ARRAY(
		m_pSelectItem);
}

// 실행 함수.
BOOL CQuestExecute_SelectItem::Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_																		// 맵 서버가 선언되어 있다면,

	if( eQuestExecute_TakeSelectItem == m_dwExecuteKind )															// 실행 종류를 확인한다.
	{
		return pQuestGroup->TakeSelectItem( pPlayer, pQuestGroup, m_dwQuestIdx, m_dwSubQuestIdx );
	}
	
#endif	

	return TRUE;																		// TRUE를 리턴한다.
}

int CQuestExecute_SelectItem::CheckCondition( PLAYERTYPE* pPlayer,				
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
	case eQuestExecute_TakeSelectItem:													// 랜덤 아이템을 받는 실행이면,
		{
			// 1. 퀘스트 그룹정보에서 보상아이템 정보가 
			const REQUITALINFO* pInfo = pQuestGroup->FindRequitalIndex( pQuest->GetQuestIdx() );
			if( !pInfo )
			{
				nErrorCode = e_EXC_ERROR_NOT_EXIST_REQUITAL;
				return nErrorCode;
			}

			nErrorCode = e_EXC_ERROR_NOT_EXIST_REQUITAL;
			for( DWORD n = 0; n < m_dwMaxItemCount; n++)
			{
				if( m_pSelectItem[n].dwItemIdx != pInfo->dwRequitalIndex )
					continue;
				if( m_pSelectItem[n].dwItemNum != pInfo->dwRequitalCount )
					continue;
	
				nErrorCode = e_EXC_ERROR_NO_ERROR;
			}
			if( nErrorCode == e_EXC_ERROR_NOT_EXIST_REQUITAL )
			{
				return nErrorCode;
			}

			// 아이템 슬롯 정보를 받는다.
			CItemSlot* pSlot = NULL ;
			pSlot = pPlayer->GetSlot(eItemTable_Inventory) ;

			if( !pSlot )
			{
				nErrorCode = e_EXC_ERROR_NO_INVENTORYINFO ;
				return nErrorCode ;
			}

			WORD EmptyCellPos[255];											
			WORD EmptyCellNum;												
			WORD wResult = 0 ;												
			wResult = ITEMMGR->CheckExtraSlot(pPlayer, pSlot, pInfo->dwRequitalIndex, pInfo->dwRequitalCount, EmptyCellPos, EmptyCellNum);
			if( wResult == 0 )								
			{
				nErrorCode = e_EXC_ERROR_NO_INVENTORYINFO ;	
				return nErrorCode ;							
			}
		}
		break;
	}

#endif //_MAPSERVER_

	return nErrorCode ;																	// 기본 값을 리턴한다.
}


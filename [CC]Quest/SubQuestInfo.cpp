
// SubQuestInfo.cpp: implementation of the CSubQuestInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																			// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "SubQuestInfo.h"																	// 서브 퀘스트 정보 해더를 불러온다.

#include "QuestLimit.h"																		// 퀘스트 제한 헤더를 불러온다.
#include "QuestTrigger.h"																	// 퀘스트 트리거 헤더를 불러온다.
#include "QuestNpcScript.h"																	// 퀘스트 npc 스크립트 헤더를 불러온다.
#include "QuestNpc.h"																		// 퀘스트 npc 헤더를 불러온다.
#include "QuestExecute.h"
#include "QuestManager.h"

#ifdef _MAPSERVER_
	#include "Quest.h"
	#include "QuestGroup.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSubQuestInfo::CSubQuestInfo( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	m_dwQuestIdx = dwQuestIdx;
	m_dwSubQuestIdx = dwSubQuestIdx;
	m_dwMaxCount = 0;																		// 최대 카운트를 0으로 세팅한다.
}

CSubQuestInfo::~CSubQuestInfo()																// 소멸자 함수.
{
	Release();																				// 해제 함수를 호출한다.
}

void CSubQuestInfo::Release()																// 해제 함수.
{
	CQuestLimit* pLimit = NULL;																// 퀘스트 제한 정보를 받을 포인터 변수를 선언하고 null 처리를 한다.

	PTRLISTPOS pos = m_QuestLimitList.GetHeadPosition();									// 퀘스트 제한 리스트를 헤드로 세팅하고 위치를 받는다.

	while( pos )																			// 위치 정보가 유효한 동안 while문을 돌린다.
	{
		pLimit = (CQuestLimit*)m_QuestLimitList.GetNext( pos );								// 위치 정보에 해당하는 퀘스트 제한 정보를 받는다.

		if( pLimit )																		// 퀘스트 제한 정보가 유효한지 체크한다.
		{
			delete pLimit;																	// 퀘스트 제한 정보를 삭제한다.
		}
	}

	m_QuestLimitList.RemoveAll();															// 퀘스트 제한 리스트를 모두 비운다.

	CQuestTrigger* pTrigger = NULL;															// 크리거 정보를 받을 포인터를 선언하고 NULL 처리를 한다.

	pos = m_QuestTriggerList.GetHeadPosition();												// 퀘스트 트리가 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while( pos )																			// 위치 정보가 유효한 동안 while 문을 돌린다.
	{
		pTrigger = (CQuestTrigger*)m_QuestTriggerList.GetNext( pos );						// 위치에 해당하는 트리거 정보를 받는다.

		if( pTrigger )																		// 트리거 정보가 유효한지 체크한다.
		{
			delete pTrigger;																// 트리거 정보를 삭제한다.
		}
	}

	m_QuestTriggerList.RemoveAll();															// 트리거 리스트를 모두 비운다.


	for( std::map< DWORD, CQuestNpcScript* >::iterator npcScriptIter = m_QuestNpcScript.begin() ; npcScriptIter != m_QuestNpcScript.end() ; ++npcScriptIter )
	{
		CQuestNpcScript* pNpcScript = npcScriptIter->second;
		SAFE_DELETE( pNpcScript );
	}
	m_QuestNpcScript.clear();

	CQuestNpc* pQuestNpc = NULL;															// 퀘스트 npc 정보를 받을 포인터를 선언하고 null 처리를 한다.

	pos = m_QuestNpcList.GetHeadPosition();													// 퀘스트 npc 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while( pos )																			// 위치 정보가 유효한동안 while문을 돌린다.
	{
		pQuestNpc = (CQuestNpc*)m_QuestNpcList.GetNext( pos );								// 퀘스트 npc 정보를 받는다.

		if( pQuestNpc )																		// 퀘스트 npc 정보가 유효한지 체크한다.
		{
			delete pQuestNpc;																// 퀘스트 npc 정보를 삭제한다.
		}
	}

	m_QuestNpcList.RemoveAll();																// 퀘스트 npc 리스트를 모두 비운다.
}

void CSubQuestInfo::AddQuestLimit( CQuestLimit* pQuestLimit )								// 퀘스트 제한 정보를 추가하는 함수.
{
	if( !pQuestLimit ) return;																// 퀘스트 제한 정보가 유효하지 않으면 리턴 처리를 한다.

	m_QuestLimitList.AddTail( pQuestLimit );												// 퀘스트 제한 리스트에 추가한다.
}

void CSubQuestInfo::AddQuestTrigger( CQuestTrigger* pQuestTrigger )							// 퀘스트 트리거를 추가하는 함수.
{
	if( !pQuestTrigger ) return;															// 퀘스트 트리거 정보가 유효하지 않으면 리턴 처리를 한다.

	m_QuestTriggerList.AddTail( pQuestTrigger );											// 퀘스트 트리거 리스트에 추가한다.
}

void CSubQuestInfo::AddQuestNpcScipt( CQuestNpcScript* pQuestNpcScript )					// 퀘스트 npc 스크립트를 추가하는 함수.
{
	if( !pQuestNpcScript ) return;															// 퀘스트 npc 스크립트 정보가 유효하지 않으면 리턴 처리를 한다.

	m_QuestNpcScript.insert( std::make_pair( pQuestNpcScript->GetKind(), pQuestNpcScript ) );	// 퀘스트 npc 스크립트 리스트에 추가한다.
}

CQuestNpcScript* CSubQuestInfo::GetQuestNpcScript()
{
	// 100517 ShinJS --- 퀘스트 요구사항을 완료했는지 검사(일반 아이템 등)
	const DWORD dwNpcScriptState = ( QUESTMGR->IsCompletedDemandQuestExecute( m_dwQuestIdx ) == TRUE ? eQuest_NpcScript_Normal : eQuest_NpcScript_Lack );
	std::map< DWORD, CQuestNpcScript* >::const_iterator npcScriptIter = m_QuestNpcScript.find( dwNpcScriptState );
	if( npcScriptIter == m_QuestNpcScript.end() )
	{
		if( dwNpcScriptState != eQuest_NpcScript_Normal )
		{
			npcScriptIter = m_QuestNpcScript.find( eQuest_NpcScript_Normal );
			if( npcScriptIter == m_QuestNpcScript.end() )
				return NULL;
		}
		else
		{
			return NULL;
		}
	}

	return npcScriptIter->second;
}

void CSubQuestInfo::AddQuestNpc( CQuestNpc* pQuestNpc )										// 퀘스트 npc 를 추가하는 함수.
{
	if( !pQuestNpc ) return;																// 퀘스트 npc 정보가 유효 하지 않으면 리턴 처리를 한다.

	m_QuestNpcList.AddTail(  pQuestNpc );													// 퀘스트 npc 리스트에 추가한다.
}

BOOL CSubQuestInfo::OnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest, CQuestEvent* pQuestEvent )	// 퀘스트 이벤트를 처리하는 함수.
{
	// 100405 ShinJS --- QuestEvent 실행 가능 판단
	if( !CanDoOnQuestEvent( pPlayer, pQuestGroup, pQuest ) )
		return FALSE;

	BOOL bIsExcuted = TRUE;
	
	// trigger
	PTRLISTPOS pos = m_QuestTriggerList.GetHeadPosition();									// 트리거 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	CQuestTrigger* pTrigger = NULL;															// 트리거 정보를 받을 포인터를 선언하고 null 처리를 한다.

	while( pos )																			// 위치 정보가 있는동안 while문을 돌린다.
	{
		pTrigger = (CQuestTrigger*)m_QuestTriggerList.GetNext( pos );						// 위치에 해당하는 트리거 정보를 받는다.

		if( pTrigger )																		// 트리거 정보가 유효한지 체크한다.
		{
			if( pTrigger->OnQuestEvent( pPlayer, pQuestGroup, pQuest, pQuestEvent ) == FALSE )
				bIsExcuted = FALSE;
		}
	}

	return bIsExcuted;
}

BOOL CSubQuestInfo::IsNpcRelationQuest( DWORD dwNpcIdx )									// npc 관련 퀘스트인지 체크하는 함수.
{
	DWORD dwQuestIdx, dwSubQuestIdx;														// 퀘스트 인덱스와 서브 퀘스트 인덱스를 담을 변수를 선언한다.

	dwQuestIdx = dwSubQuestIdx = 0;															// 퀘스트 인덱스와 서브 퀘스트 인덱스를 0으로 세팅한다.

	// 100517 ShinJS --- 퀘스트 요구사항을 완료했는지 검사(일반 아이템 등)
	const DWORD dwNpcScriptState = ( QUESTMGR->IsCompletedDemandQuestExecute( m_dwQuestIdx ) == TRUE ? eQuest_NpcScript_Normal : eQuest_NpcScript_Lack );
	std::map< DWORD, CQuestNpcScript* >::const_iterator npcScriptIter = m_QuestNpcScript.find( dwNpcScriptState );
	if( npcScriptIter == m_QuestNpcScript.end() )
	{
		if( dwNpcScriptState != eQuest_NpcScript_Normal )
		{
			npcScriptIter = m_QuestNpcScript.find( eQuest_NpcScript_Normal );
			if( npcScriptIter == m_QuestNpcScript.end() )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	CQuestNpcScript* pNpcScript = npcScriptIter->second;
	if( !pNpcScript )
		return FALSE;

	if( pNpcScript->IsSameNpc( dwNpcIdx, &dwQuestIdx, &dwSubQuestIdx ) == 0 )
		return FALSE;

	// 100405 ShinJS --- QuestEvent 실행 가능 판단
	if( !CanDoOnQuestEvent( dwQuestIdx, dwSubQuestIdx ) )
		return FALSE;

	return TRUE;																			// TRUE 리턴 처리를 한다.
}

DWORD CSubQuestInfo::GetNpcScriptPage( DWORD dwNpcIdx )										// NPC 스크립트 페이지를 리턴하는 함수.
{
	// 100517 ShinJS --- 퀘스트 요구사항을 완료했는지 검사(일반 아이템 등)
	const DWORD dwNpcScriptState = ( QUESTMGR->IsCompletedDemandQuestExecute( m_dwQuestIdx ) == TRUE ? eQuest_NpcScript_Normal : eQuest_NpcScript_Lack );
	std::map< DWORD, CQuestNpcScript* >::const_iterator npcScriptIter = m_QuestNpcScript.find( dwNpcScriptState );
	if( npcScriptIter == m_QuestNpcScript.end() )
	{
		if( dwNpcScriptState != eQuest_NpcScript_Normal )
		{
			npcScriptIter = m_QuestNpcScript.find( eQuest_NpcScript_Normal );
			if( npcScriptIter == m_QuestNpcScript.end() )
				return 0;
		}
		else
		{
			return 0;
		}
	}

	CQuestNpcScript* pNpcScript = npcScriptIter->second;
	if( !pNpcScript )
		return 0;

	DWORD dwQuestIdx = 0, dwSubQuestIdx = 0;
	return pNpcScript->IsSameNpc( dwNpcIdx, &dwQuestIdx, &dwSubQuestIdx );
}

DWORD CSubQuestInfo::GetNpcMarkType( DWORD dwNpcIdx )										// npc 마크 타입을 반환하는 함수.
{
	// 100517 ShinJS --- 퀘스트 요구사항을 완료했는지 검사(일반 아이템 등)
	const DWORD dwNpcScriptState = ( QUESTMGR->IsCompletedDemandQuestExecute( m_dwQuestIdx ) == TRUE ? eQuest_NpcScript_Normal : eQuest_NpcScript_Lack );
	std::map< DWORD, CQuestNpcScript* >::const_iterator npcScriptIter = m_QuestNpcScript.find( dwNpcScriptState );
	if( npcScriptIter == m_QuestNpcScript.end() )
	{
		if( dwNpcScriptState != eQuest_NpcScript_Normal )
		{
			npcScriptIter = m_QuestNpcScript.find( eQuest_NpcScript_Normal );
			if( npcScriptIter == m_QuestNpcScript.end() )
				return 0;
		}
		else
		{
			return 0;
		}
	}

	CQuestNpcScript* pNpcScript = npcScriptIter->second;
	if( !pNpcScript )
		return 0;

	// 100405 ShinJS --- QuestEvent 실행 가능 판단
	if( !CanDoOnQuestEvent( m_dwQuestIdx, m_dwSubQuestIdx ) )
		return 0;

	DWORD dwQuestIdx = 0, dwSubQuestIdx = 0;
	return pNpcScript->IsSameNpcMark( dwNpcIdx, &dwQuestIdx, &dwSubQuestIdx );
}
	
eQuestState CSubQuestInfo::IsQuestState( DWORD dwNpcIdx )											// 퀘스트 상태를 리턴하는 함수.
{
	// 100517 ShinJS --- 퀘스트 요구사항을 완료했는지 검사(일반 아이템 등)
	const DWORD dwNpcScriptState = ( QUESTMGR->IsCompletedDemandQuestExecute( m_dwQuestIdx ) == TRUE ? eQuest_NpcScript_Normal : eQuest_NpcScript_Lack );
	std::map< DWORD, CQuestNpcScript* >::const_iterator npcScriptIter = m_QuestNpcScript.find( dwNpcScriptState );
	if( npcScriptIter == m_QuestNpcScript.end() )
	{
		if( dwNpcScriptState != eQuest_NpcScript_Normal )
		{
			npcScriptIter = m_QuestNpcScript.find( eQuest_NpcScript_Normal );
			if( npcScriptIter == m_QuestNpcScript.end() )
				return eQuestState_ImPossible;
		}
		else
		{
			return eQuestState_ImPossible;
		}
	}

	CQuestNpcScript* pNpcScript = npcScriptIter->second;
	if( !pNpcScript )
		return eQuestState_ImPossible;

	DWORD dwQuestIdx = 0, dwSubQuestIdx = 0;
	DWORD dwPage = pNpcScript->IsSameNpc( dwNpcIdx, &dwQuestIdx, &dwSubQuestIdx );
	if( dwPage == 0 )																		// 페이지가 0과 같다면,
	{
		return eQuestState_ImPossible;
	}

	// 100405 ShinJS --- QuestEvent 실행 가능 판단
	if( !CanDoOnQuestEvent( dwQuestIdx, dwSubQuestIdx ) )
		return eQuestState_ImPossible;

	if( dwSubQuestIdx == 0 )	return eQuestState_Possible;			// 수행가능한 협행						// 서브 퀘스트 인덱스가 0과 같으면, 수행가능한 협행.
	else						return eQuestState_Executed;			// 수행중인 협행						// 서브 퀘스트 인덱스가 0과 같지 않으면, 수행중인 협행.

}

BOOL CSubQuestInfo::CheckLimitCondition( DWORD dwQuestIdx )									// 퀘스트 제한 상태를 확인하는 함수.
{
	// 100405 ShinJS --- QuestEvent 실행 가능 판단
	if( !CanDoOnQuestEvent( dwQuestIdx, 0 ) )
		return FALSE;

	return TRUE;
}

BOOL CSubQuestInfo::CanDoOnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_
	// 100402 ShinJS --- 수락/완료 가능 시간인지 검사
	if( !pQuestGroup->IsValidDateTime( pQuest->GetQuestIdx() ) )
		return FALSE;
#endif

	// 퀘스트 제한정보 검사
	PTRLISTPOS pos = m_QuestLimitList.GetHeadPosition();
	CQuestLimit* pLimit = NULL;
	while( pos )
	{
		pLimit = (CQuestLimit*)m_QuestLimitList.GetNext( pos );
		if( pLimit && !pLimit->CheckLimit( pPlayer, pQuestGroup, pQuest ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CSubQuestInfo::CanDoOnQuestEvent( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
#ifdef _CLIENT_
	// 100402 ShinJS --- 수락/완료 가능 시간인지 검사
	if( !QUESTMGR->IsValidDateTime( dwQuestIdx ) )
		return FALSE;
#endif

	// 퀘스트 제한정보 검사
	PTRLISTPOS pos = m_QuestLimitList.GetHeadPosition();
	CQuestLimit* pLimit = NULL;
	while( pos )
	{
		pLimit = (CQuestLimit*)m_QuestLimitList.GetNext( pos );
		if( pLimit && !pLimit->CheckLimit( dwQuestIdx, dwSubQuestIdx ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CSubQuestInfo::StartLoopByQuestExecuteKind( DWORD dwExecuteKind )
{
	m_FoundQuestExecute.clear();

	// 같은 퀘스트실행 정보를 찾는다.
	PTRLISTPOS pos = m_QuestTriggerList.GetHeadPosition();
	CQuestTrigger* pTrigger = NULL;
	while( pos )
	{
		pTrigger = (CQuestTrigger*)m_QuestTriggerList.GetNext( pos );
		if( !pTrigger )
			continue;

		cPtrList* pQuestExeList = pTrigger->GetExeList();
		if( !pQuestExeList )
			continue;

		PTRLISTPOS exePos = pQuestExeList->GetHeadPosition();
		while( exePos )
		{
			CQuestExecute* pQuestExecute = (CQuestExecute*)pQuestExeList->GetNext( exePos );
			if( !pQuestExecute )
				continue;

			if( pQuestExecute->GetQuestExecuteKind() == dwExecuteKind )
			{
				m_FoundQuestExecute.push_back( pQuestExecute );
			}
		}
	}

	m_FoundQuestExecuteIter = m_FoundQuestExecute.begin();
}

CQuestExecute* CSubQuestInfo::GetNextQuestExecute()
{
	if( m_FoundQuestExecute.empty() ||
		m_FoundQuestExecuteIter == m_FoundQuestExecute.end() )
		return NULL;

	CQuestExecute* pQuestExecute = *m_FoundQuestExecuteIter;
	++m_FoundQuestExecuteIter;

	return pQuestExecute;
}
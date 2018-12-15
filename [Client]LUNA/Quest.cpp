// Quest.cpp: implementation of the CQuest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Quest.h"
#include "..\[CC]Quest\QuestInfo.h"
#include "..\[CC]Quest\SubQuestInfo.h"
#include "ObjectManager.h"
#include "StatusIconDlg.h"
#include "MHTimeManager.h"
#include "../[CC]Quest/QuestExecute.h"
#include "../[CC]Quest/QuestExecute_Item.h"
#include "GameIn.h"
#include "InventoryExDialog.h"
#include "../[CC]Quest/QuestNpcScript.h"
#include "QuestManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuest::CQuest( CQuestInfo* pQuestInfo )													// 생성자 함수.
{
	m_pQuestInfo = pQuestInfo;																// 퀘스트 정보를 받아 멤버 변수에 세팅한다.

	memset( m_CurSubQuestInfoArray, 0, sizeof(CSubQuestInfo*)*MAX_SUBQUEST );				// 서브 퀘스트 정보를 담는 배열을 메모리 세팅을 한다.

	m_CurSubQuestInfoArray[0] = m_pQuestInfo->GetSubQuestInfo( 0 );							// 퀘스트 정보에서 0번째 서브 퀘스트 정보를 받아 배열0번에 세팅한다.

	m_dwSubQuestCount = m_pQuestInfo->GetSubQuestCount();									// 서브 퀘스트 카운트를 받아 멤버 카운트에 세팅한다.


	memset( m_SubQuestTable, 0, sizeof(SUBQUEST)*MAX_SUBQUEST );							// 서브 퀘스트 테이블을 메모리 세팅한다.

	m_dwSubQuestFlag = m_dwData = 0;
	m_RegistTime = 0;
	
	m_bCheckTime = FALSE;																	// 시간체크 여부를 FALSE로 세팅한다.
	m_CheckType = 0;																		// 체크 타입은 0으로 세팅한다.
	m_CheckTime.value = 0;																	// 시간 값을 0으로 세팅한다.

	m_dwCurSubQuestIdx = (DWORD)-1;

	InitDateTime();

	InitDemandQuestExecute();
}

CQuest::~CQuest()																			// 소멸자 함수.
{
}

void CQuest::Initialize()																	// 초기화 함수.
{
	memset( m_CurSubQuestInfoArray, 0, sizeof(CSubQuestInfo*)*MAX_SUBQUEST );				// 서브 퀘스트 정보를 담는 배열을 메모리 세팅한다.

	m_CurSubQuestInfoArray[0] = m_pQuestInfo->GetSubQuestInfo( 0 );							// 퀘스트 정보에서 0번째 서브 퀘스트 정보를 받아 배열0번에 세팅한다.

	m_dwSubQuestCount = m_pQuestInfo->GetSubQuestCount();									// 서브 퀘스트 카운트를 받아 멤버 카운트에 세팅한다.

	memset( m_SubQuestTable, 0, sizeof(SUBQUEST)*MAX_SUBQUEST );							// 서브 퀘스트 테이블을 메모리 세팅한다.

	m_dwSubQuestFlag = m_dwData = 0;
	m_RegistTime = 0;

	m_bCheckTime = FALSE;																	// 시간 체크 여부를 FALSE로 세팅한다.
	m_CheckType = 0;																		// 체크 타입을 0으로 세팅한다.
	m_CheckTime.value = 0;																	// 시간 값을 0으로 세팅한다.

	m_dwCurSubQuestIdx = (DWORD)-1;
}

void CQuest::SetMainQuestData( DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime, BYTE bCheckType, DWORD dwCheckTime )	// 메인 퀘스트 데이터를 세팅하는 함수.
{
	m_CurSubQuestInfoArray[0] = NULL;														// 현재, 서브 퀘스트 정보 배열 0을 null 처리 한다.
	m_dwSubQuestFlag = dwSubQuestFlag;														// 서브 퀘스트 플래그를 세팅한다.
	m_dwData = dwData;																		// 데이터를 세팅한다.
	m_RegistTime = registTime;																		// 시간 값을 세팅한다.

	m_CheckType = bCheckType;																// 체크 타입을 세팅한다.
	m_CheckTime.value = dwCheckTime;														// 체크 타임을 세팅한다.

	if( m_CheckType != 0 )																	// 체크 타입이 0과 같지 않으면,
	{
		m_bCheckTime = TRUE;																// 시간 체크 여부를 TRUE로 세팅한다.

		if( GetCurTime() < m_CheckTime.value )												// 현재 시간이 체크 타임보다 작으면,
            STATUSICONDLG->AddQuestIconCount();												// 퀘스트 아이콘 카운트를 추가한다.
	}
}

void CQuest::SetSubQuestData( DWORD dwSubQuestIdx, DWORD dwSubData, DWORD dwSubTime )		// 서브 퀘스트 데이터를 세팅하는 함수.
{
	m_CurSubQuestInfoArray[dwSubQuestIdx] = m_pQuestInfo->GetSubQuestInfo( dwSubQuestIdx );	// 서브 퀘스트 인덱스에 해당하는, 서브 퀘스트 배열에 퀘스트 정보를 담는다.
	m_SubQuestTable[dwSubQuestIdx].dwData = dwSubData;										// 서브 퀘스트 데이터를 세팅한다.
	m_SubQuestTable[dwSubQuestIdx].dwTime = dwSubTime;										// 서브 퀘스트 시간을 세팅한다.
	m_SubQuestTable[dwSubQuestIdx].dwMaxCount = m_CurSubQuestInfoArray[dwSubQuestIdx]->GetMaxCount();	// 서브 퀘스트 카운트를 세팅한다.

	m_dwCurSubQuestIdx = dwSubQuestIdx ;																	// 시작하는 서브 퀘스트 인덱스를 세팅한다.

	InitDemandQuestExecute();
}

void CQuest::StartQuest( DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime )		// 퀘스트를 시작하는 함수.
{
	m_CurSubQuestInfoArray[0] = NULL;														// 서브 퀘스트 정보 배열0을 NULL 처리 한다.
	m_dwSubQuestFlag = dwSubQuestFlag;														// 서브 퀘스트 플래그를 세팅한다.
	m_dwData = dwData;																		// 데이터를 세팅한다.
	m_RegistTime = registTime;																		// 시간을 세팅한다.
}

void CQuest::EndQuest( DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime )					// 퀘스트를 종료하는 함수.
{
	if( dwSubQuestFlag == 0 )	// 반복														// 서브 퀘스트 플래그가 0과 같다면,
	{
		Initialize();																		// 초기화 함수를 호촐한다.
	}
	else																					// 그렇지 않으면,
	{
		m_dwSubQuestFlag = dwSubQuestFlag;													// 서브 퀘스트 플래그를 세팅한다.
		m_dwData = dwData;																	// 데이터를 세팅한다.
		m_RegistTime = registTime;															// 시간 값을 세팅한다.

		m_bCheckTime = FALSE;																// 시간 체크 여부를 FALSE로 세팅한다.
		m_CheckType = 0;																	// 체크 타입을 0으로 세팅한다.
		m_CheckTime.value = 0;																// 시간 값을 0으로 세팅한다.
	}
}

void CQuest::StartSubQuest( DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime )	// 서브 퀘스트를 시작하는 함수.
{
	m_CurSubQuestInfoArray[dwSubQuestIdx] = m_pQuestInfo->GetSubQuestInfo( dwSubQuestIdx );					// 서브 퀘스트 정보를 세팅한다.
	m_dwSubQuestFlag = dwSubQuestFlag;																		// 서브 퀘스트 플래그를 세팅한다.
	m_SubQuestTable[dwSubQuestIdx].dwData = dwSubData;														// 서브 퀘스트 데이터를 세팅한다.
	m_SubQuestTable[dwSubQuestIdx].dwTime = dwSubTime;														// 서브 퀘스트 시간을 세팅한다.
	m_SubQuestTable[dwSubQuestIdx].dwMaxCount = m_CurSubQuestInfoArray[dwSubQuestIdx]->GetMaxCount();		// 서브 퀘스트 카운트를 세팅한다.

	m_dwCurSubQuestIdx = dwSubQuestIdx ;																	// 시작하는 서브 퀘스트 인덱스를 세팅한다.

	InitDemandQuestExecute();
}

void CQuest::EndSubQuest( DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime )		// 서브 퀘스트를 종료하는 함수.
{
	m_CurSubQuestInfoArray[dwSubQuestIdx] = NULL;															// 서브 퀘스트 정보를 null 처리를 한다.
	m_dwSubQuestFlag = dwSubQuestFlag;																		// 서브 퀘스트 플래그를 세팅한다.
	m_SubQuestTable[dwSubQuestIdx].dwData = dwSubData;														// 서브 퀘스트 데이터를 세팅한다.
	m_SubQuestTable[dwSubQuestIdx].dwTime = dwSubTime;														// 서브 퀘스트 시간을 세팅한다.

	m_dwCurSubQuestIdx = dwSubQuestIdx ;																	// 시작하는 서브 퀘스트 인덱스를 세팅한다.

	InitDemandQuestExecute();
}

void CQuest::UpdateSubQuestData( DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwSubData, DWORD dwSubTime )	// 서브 퀘스트를 업데이트 하는 함수.
{
	m_SubQuestTable[dwSubQuestIdx].dwMaxCount = dwMaxCount;													// 서브 퀘스트 최대 카운트를 세팅한다.
	m_SubQuestTable[dwSubQuestIdx].dwData = dwSubData;														// 서브 퀘스트 데이터를 세팅한다.
	m_SubQuestTable[dwSubQuestIdx].dwTime = dwSubTime;														// 서브 퀘스트 시간을 세팅한다.

	m_dwCurSubQuestIdx = dwSubQuestIdx ;																	// 시작하는 서브 퀘스트 인덱스를 세팅한다.

	InitDemandQuestExecute();
}

DWORD CQuest::GetQuestIdx()																					// 퀘스트 인덱스를 반환하는 함수.
{
	return m_pQuestInfo->GetQuestIdx();																		// 퀘스트 정보에 퀘스트 인덱스를 반환한다.
}

BOOL CQuest::IsNpcRelationQuest( DWORD dwNpcIdx )															// npc관련 퀘스트인지 체크하는 함수.
{
	for( DWORD i = 0; i < m_dwSubQuestCount; ++i )															// 서브 퀘스트 수만큼 for문을 돌린다.
	{
		if( m_CurSubQuestInfoArray[i] )																		// 카운트에 해당하는 서브 퀘스트 정보가 유효하면,
		if( m_CurSubQuestInfoArray[i]->IsNpcRelationQuest( dwNpcIdx ) )										// npc 관련 퀘스트가 맞다면,
			return TRUE;																					// TRUE를 리턴한다.
	}
	return FALSE;																							// FALSE를 리턴한다.
}

DWORD CQuest::GetNpcScriptPage( DWORD dwNpcIdx )															// NPC 스크립트 페이지를 반환하는 함수.
{
	for( DWORD i = 0; i < m_dwSubQuestCount; ++i )															// 서브 퀘스트 수만큼 for문을 돌린다.
	{
		CSubQuestInfo* const info = m_CurSubQuestInfoArray[i];

		if(0 == info)
		{
			continue;
		}

		const DWORD dwPage = info->GetNpcScriptPage(dwNpcIdx);

		if(0 == dwPage)
		{
			continue;
		}

		return dwPage;
	}
	return 0;																							// 페이지를 리턴한다.
}

BOOL CQuest::IsSubQuestComplete( DWORD dwSubQuestIdx )														// 서브 퀘스트의 완료 여부를 리턴하는 함수.
{
	return ( m_dwSubQuestFlag & ( 1 << (31-dwSubQuestIdx) ) ) ? TRUE : FALSE;							// 서브 퀘스트 플래그에 따른 값을 리턴한다.
}

DWORD CQuest::GetNpcMarkType( DWORD dwNpcIdx )																// npc 마크 타입을 반환한다.
{
	for( DWORD i = 0; i < m_dwSubQuestCount; ++i )															// 서브 퀘스트 카운트 만큼 for문을 돌린다.
	{
		CSubQuestInfo* const info = m_CurSubQuestInfoArray[i];

		if(0 == info)
		{
			continue;
		}

		const DWORD dwNpcMarkType = info->GetNpcMarkType(dwNpcIdx);

		if(0 == dwNpcMarkType)
		{
			continue;
		}

		return dwNpcMarkType;
	}

	return 0;
}

CSubQuestInfo* CQuest::GetSubQuestInfo( DWORD dwSubQuestIdx )												// 서브 퀘스트 정보를 리턴하는 함수.
{
	return m_pQuestInfo->GetSubQuestInfo( dwSubQuestIdx );													// 들어온 서브 퀘스트 인덱스로 서브 퀘스트 정보를 반환한다.
}

eQuestState CQuest::IsQuestState( DWORD dwNpcIdx )																	// 퀘스트 상태를 반환하는 함수.
{
	for( DWORD i = 0; i < m_dwSubQuestCount; ++i )															// 서브 퀘스트 카운트 만큼 for문을 돌린다.
	{
		if( m_CurSubQuestInfoArray[i] )																		// 서브 퀘스트 정보가 유효하면,
		{
			eQuestState state = m_CurSubQuestInfoArray[i]->IsQuestState( dwNpcIdx );						// 현재 서브 퀘스트의 상태 값을 결과로 받는다.
			if( state != eQuestState_ImPossible )
				return state;
		}
	}
	return eQuestState_ImPossible;
}

BOOL CQuest::CanItemUse( DWORD dwQuestIdx )																	// 아이템 사용여부를 리턴한다.
{
	if( m_CurSubQuestInfoArray[0] )																			// 서브 퀘스트 정보 배열 0번이 유효하면,
	{
		return m_CurSubQuestInfoArray[0]->CheckLimitCondition( dwQuestIdx );								// 퀘스트 제한을 체크하여 리턴한다.
	}
	return FALSE;																							// FALSE를 리턴한다.
}

void CQuest::RegistCheckTime( BYTE bCheckType, DWORD dwCheckTime )											// 시간 체크를 등록하는 함수.
{
	m_bCheckTime = TRUE;																					// 시간 체크 여부를 TRUE로 세팅한다.
	m_CheckType = bCheckType;																				// 체크 타입을 세팅한다.
	m_CheckTime.value = dwCheckTime;																		// 타임 값을 세팅한다.
}

void CQuest::UnRegistCheckTime()																			// 시간 체크를 해제 하는 함수.
{
	m_bCheckTime = FALSE;																					// 시간 체크 여부를 FALSE로 세팅한다.
	m_CheckType = 0;																						// 체크 타입을 0으로 세팅한다.
	m_CheckTime.value = 0;																					// 타임 값을 0으로 세팅한다.
}

void CQuest::InitDateTime()
{
	if( !m_pQuestInfo->HasDateLimit() )
	{
		m_NextDateTime = 0;
		m_IsValidDateTime = TRUE;
		return;
	}

	// 유효시간정보 설정
	m_IsValidDateTime = m_pQuestInfo->IsValidDateTime();
	m_NextDateTime = ( m_IsValidDateTime == TRUE ? m_pQuestInfo->GetNextEndDateTime() : m_pQuestInfo->GetNextStartDateTime() );
}

BOOL CQuest::IsValidDateTime()
{
	if( !m_pQuestInfo->HasDateLimit() )
		return TRUE;

	__time64_t curTime = MHTIMEMGR->GetServerTime();

	// 다음 시작/종료 시간 검사
	if( m_NextDateTime < curTime )
	{
		InitDateTime();
	}

	return m_IsValidDateTime;
}

BOOL CQuest::HasDateLimit() const
{
	return m_pQuestInfo->HasDateLimit();
}

void CQuest::InitDemandQuestExecute()
{
	// 갱신이 필요한 CQuestExecute 들
	int nNeedRefreshQuestExecute[] = {
		eQuestExecute_GiveItem,
	};

	m_DemandQuestExecuteMap.clear();


	for( DWORD i = 0; i < m_dwSubQuestCount; ++i )
	{
		if( !m_CurSubQuestInfoArray[i] )
			continue;

		for( int nLoop=0 ; nLoop < sizeof(*nNeedRefreshQuestExecute)/sizeof(nNeedRefreshQuestExecute) ; ++nLoop )
		{
			m_CurSubQuestInfoArray[i]->StartLoopByQuestExecuteKind( nNeedRefreshQuestExecute[ nLoop ] );
			CQuestExecute* pQuestExecute = NULL;
			while( (pQuestExecute = m_CurSubQuestInfoArray[i]->GetNextQuestExecute()) != NULL )
			{
				DemandQuestExecuteKey key;
				key.first = m_pQuestInfo->GetQuestIdx();
				key.second = i;

				m_DemandQuestExecuteMap.insert( std::make_pair( key, pQuestExecute ) );
			}
		}
	}
}

BOOL CQuest::IsCompletedDemandQuestExecute() const
{
	if( m_DemandQuestExecuteMap.empty() )
		return TRUE;

	typedef std::multimap< DemandQuestExecuteKey, CQuestExecute* >::const_iterator QuestExecuteIter;
	std::pair<QuestExecuteIter, QuestExecuteIter> equalRange;

	DemandQuestExecuteKey key;
	key.first = m_pQuestInfo->GetQuestIdx();
	key.second = m_dwCurSubQuestIdx;
    equalRange = m_DemandQuestExecuteMap.equal_range( key );

	if( equalRange.first == m_DemandQuestExecuteMap.end() &&
		equalRange.second == m_DemandQuestExecuteMap.end() )
		return TRUE;

	for( QuestExecuteIter iter = equalRange.first ; iter != equalRange.second ; ++iter )
	{
		CQuestExecute* pQuestExecute = iter->second;

		switch( pQuestExecute->GetQuestExecuteKind() )
		{
		case eQuestExecute_GiveItem:
			{
				CQuestExecute_Item* pQuestExecute_Item = (CQuestExecute_Item*)pQuestExecute;
				const DWORD dwItemIdx = pQuestExecute_Item->GetItemKind();
				const DWORD dwNeedCnt = pQuestExecute_Item->GetItemNum();
				
				const DWORD dwInvenCnt = GAMEIN->GetInventoryDialog()->GetTotalItemDurability( dwItemIdx );
				if( dwInvenCnt < dwNeedCnt )
					return FALSE;
			}
		}
	}

	return TRUE;
}

void CQuest::AddNpcHasQuest()
{
	for( DWORD i = 0; i < m_pQuestInfo->GetSubQuestCount(); ++i )
	{
		CSubQuestInfo* pSubQuestInfo = m_pQuestInfo->GetSubQuestInfo( i );
		if( pSubQuestInfo == NULL )
			continue;

		const std::map< DWORD, CQuestNpcScript* >& questNpcScriptList = pSubQuestInfo->GetQuestNpcScriptList();
		for( std::map< DWORD, CQuestNpcScript* >::const_iterator questNpcScriptIter = questNpcScriptList.begin() ; questNpcScriptIter != questNpcScriptList.end() ; ++questNpcScriptIter )
		{
			const CQuestNpcScript* const pQuestNpcScript = questNpcScriptIter->second;
			const DWORD dwNpcUniqueIdx = pQuestNpcScript->GetNpcIdx();
			const DWORD dwQuestIdx = GetQuestIdx();

			QUESTMGR->AddNpcHasQuest( dwNpcUniqueIdx, dwQuestIdx );
		}
	}
}
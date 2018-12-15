// QuestLimitKind_Quest.cpp: implementation of the CQuestLimitKind_Quest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"														// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestLimitKind_Quest.h"										// 퀘스트 리미트 퀘스트 헤더를 불러온다.

#include "QuestScriptLoader.h"											// 퀘스트 리미트 스크립트 로더 헤더를 불러온다.
#include "Quest.h"														// 퀘스트 헤더를 불러온다.

#ifdef _MAPSERVER_														// 맵 서버가 선언되어 있다면,

#include "QuestGroup.h"													// 퀘스트 그룹 헤더를 불러온다.

#else																	// 맵 서버가 선언되어 있지 않다면,

#include "QuestManager.h"												// 퀘스트 매니져 헤더를 불러온다.

#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 생성자 함수.
CQuestLimitKind_Quest::CQuestLimitKind_Quest( DWORD dwLimitKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: CQuestLimitKind( dwLimitKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	GetScriptParam( m_dwIdx, pTokens );									// 퀘스트 인덱스를 받는다.
}

// 소멸자 함수.
CQuestLimitKind_Quest::~CQuestLimitKind_Quest()							
{
}

// 퀘스트 리미트 체크함수.(서버)
BOOL CQuestLimitKind_Quest::CheckLimit( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
	// 071011 LYW --- QuestLimitKind_Quest : Check parameter info.
	if( !pPlayer || !pQuestGroup || !pQuest ) return FALSE ;			// 인자로 넘어온 정보가 유효하지 않으면, FALSE 리턴 처리를 한다.

#ifdef _MAPSERVER_														// 맵서버가 선언되어 있다면,

	switch( m_dwLimitKind )												// 퀘스트 리미트 종류를 확인한다.
	{
	case eQuestLimitKind_Quest:											// 리미트 종류가 퀘스트라면,
		{
			if( pQuestGroup->GetQuest( m_dwIdx )->IsQuestComplete() )	// 퀘스트 인덱스로 완료한 퀘스트인지 체크한다.
			{
				return TRUE;											// TRUE를 리턴한다.
			}
		}
		break;

	case eQuestLimitKind_SubQuest:										// 리미트 종류가 서브 퀘스트라면,
		{
			if( pQuest->IsSubQuestComplete( m_dwIdx ) )					// 퀘스트 인덱스로 완료한 서브 퀘스트 인지 체크한다.
			{
				return TRUE;											// TRUE를 리턴한다.
			}
		}
		break;

	// 071011 LYW --- QuestLimitKind_Quest : Add quest limit to check running quest.
	case eQuestLimitKind_RunningQuest :									// 진행중인 퀘스트를 체크하는 리미트라면,
		{
			CQuest* pQuest = NULL ;										// 퀘스트 정보를 담을 포인터를 선언한다.
			pQuest = pQuestGroup->GetQuest( m_dwIdx ) ;					// 퀘스트 정보를 받는다.

			if( !pQuest ) return FALSE ;								// 퀘스트 정보가 유효하지 않으면, false 리턴 처리를 한다.

			if( pQuest->GetSubQuestFlag() == 0 ) return TRUE ;			// 퀘스트의 서브 퀘스트 플래그가 0이 아니면, true 리턴 처리를 한다.
		}
		break ;
	}
#endif

	return FALSE;														// FALSE를 리턴한다.
}

// 퀘스트 리미트 체크함수.(클라이언트)
BOOL CQuestLimitKind_Quest::CheckLimit( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
#ifndef _MAPSERVER_														// 맵 서버가 선언되지 않았다면,

	switch( m_dwLimitKind )												// 퀘스트 리미트 종류를 확인한다.
	{
	case eQuestLimitKind_Quest:											// 리미트 종류가 퀘스트라면,
		{
			if( QUESTMGR->GetQuest( m_dwIdx )->IsQuestComplete() )		// 퀘스트 인덱스로 완료한 퀘스트 인지 체크한다.
			{
				return TRUE;											// TRUE를 리턴한다.
			}
		}
		break;

	case eQuestLimitKind_SubQuest:										// 리미트 종류가 서브 퀘스트라면,
		{
			if( QUESTMGR->GetQuest( dwQuestIdx )->IsSubQuestComplete( m_dwIdx ) )	// 퀘스트 인덱스로 완료한 서브퀘스트 인지 체크한다.
			{
				return TRUE;											// TRUE를 리턴한다.
			}
		}
		break;
	// 071011 LYW --- QuestLimitKind_Quest : Add quest limit to check running quest.
	case eQuestLimitKind_RunningQuest :									// 진행중인 퀘스트를 체크하는 리미트라면,
		{
			CQuest* pQuest = NULL ;										// 퀘스트 정보를 담을 포인터를 선언한다.
			pQuest = QUESTMGR->GetQuest( m_dwIdx ) ;					// 퀘스트 정보를 받는다.

			if( !pQuest ) return FALSE ;								// 퀘스트 정보가 유효하지 않으면, false 리턴 처리를 한다.

			if( pQuest->GetSubQuestFlag() == 0 ) return TRUE ;			// 퀘스트의 서브 퀘스트 플래그가 0이 아니면, true 리턴 처리를 한다.
		}
		break ;
	}
#endif

	return FALSE;														// FALSE를 리턴한다.
}
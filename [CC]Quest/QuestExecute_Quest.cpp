// QuestExecute_Quest.cpp: implementation of the CQuestExecute_Quest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																		// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestExecute_Quest.h"															// 퀘스트 실행 퀘스트 헤더를 불러온다.

#include "QuestScriptLoader.h"															// 퀘스트 스트립트 로더 헤더를 불러온다.

#ifdef _MAPSERVER_																		// 맵 서버가 선언되어 있지 않으면,

#include "QuestGroup.h"																	// 퀘스트 그룹 헤더를 불러온다.
#include "QuestManager.h"																// 퀘스트 매니져 헤더를 불러온다.
#include "QuestInfo.h"																	// 퀘스트 정보 헤더를 불러온다.

#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 생성자 함수.
CQuestExecute_Quest::CQuestExecute_Quest( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: CQuestExecute( dwExecuteKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	m_dwExeQuestIdx = m_dwExeSubQuestIdx = 0;											// 실행 퀘스트 인덱스, 실행 서브 퀘스트 인덱스를 0으로 세팅한다.

	if( m_dwExecuteKind == eQuestExecute_StartSub )										// 실행 종류가 서브 퀘스트 실행과 같다면,
	{
		GetScriptParam( m_dwExeQuestIdx, pTokens );										// 실행 퀘스트 인덱스를 받는다.
		GetScriptParam( m_dwExeSubQuestIdx, pTokens );									// 실행 서브 퀘스트 인덱스를 받는다.
	}
	else if( m_dwExecuteKind == eQuestExecute_EndQuest )								// 실행 종류가 퀘스트 종료라면,
	{
		GetScriptParam( m_dwExeQuestIdx, pTokens );										// 실행 퀘스트 인덱스를 받는다.
	}
	else if( m_dwExecuteKind == eQuestExecute_EndOtherSub )								// 실행 종류가 다른 서브 퀘스트 종료라면,
	{
		GetScriptParam( m_dwExeQuestIdx, pTokens );										// 실행 퀘스트 인덱스를 받는다.
		GetScriptParam( m_dwExeSubQuestIdx, pTokens );									// 실행 서브 퀘스트 인덱스를 받는다.
	}
	else if( m_dwExecuteKind == eQuestExecute_RegenMonster )							// 실행 종류가 몬스터 리젠과 같다면,
	{
		GetScriptParam( m_dwExeQuestIdx, pTokens );										// 실행 퀘스트 인덱스를 받는다.
	}
	// 080104 LYW --- QuestExecute_Quest : 맵 이동 기능의 처리를 수정함.
	else if( m_dwExecuteKind == eQuestExecute_MapChange )
	{
		GetScriptParam( m_DestinationMapNum, pTokens ) ;								// 도착지역의 맵 번호를 받는다.
		GetScriptParam( m_fDestination_XPos, pTokens ) ;								// 도착지역의 X좌표를 받는다.
		GetScriptParam( m_fDestination_ZPos, pTokens ) ;								// 도착지역의 Z좌표를 받는다.
	}
	else if( m_dwExecuteKind == eQuestExecute_ChangeStage )								// 실행 종류가 스테이지 체인지 라면,
	{
		GetScriptParam( m_dwExeQuestIdx, pTokens );										// 실행 퀘스트 인덱스를 받는다.
		GetScriptParam( m_dwExeSubQuestIdx, pTokens ) ;									// 실행 서브 퀘스트 인덱스를 받는다.
	}
	else if( m_dwExecuteKind == eQuestExecute_ChangeSubAttr )							// 실행 종류가 서브 퀘스트 Attr 변경이라면,
	{
		GetScriptParam( m_dwExeQuestIdx, pTokens );										// 실행 퀘스트 인덱스를 받는다.
	}
}

CQuestExecute_Quest::~CQuestExecute_Quest()												// 소멸자 함수.
{
}

// 실행 함수.
BOOL CQuestExecute_Quest::Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_																		// 맵 서버가 선언되어 있다면,

	switch( m_dwExecuteKind )															// 실행 종류를 확인한다.
	{
	case eQuestExecute_EndQuest:														// 퀘스트를 종료하는 실행이라면,
		{			
			pQuestGroup->EndQuest( m_dwQuestIdx, m_dwExeQuestIdx );						// 퀘스트 종료 함수를 호출한다.
		}
		break;		

	case eQuestExecute_StartSub:														// 서브 퀘스트를 시작하는 실행이라면,
		{
			pQuestGroup->StartSubQuest( m_dwExeQuestIdx, m_dwExeSubQuestIdx );			// 서브 퀘스트를 시작한다.
		}
		break;

	case eQuestExecute_EndSub:															// 서브 퀘스트를 종료하는 실행이라면,
		{
			pQuestGroup->EndSubQuest( m_dwQuestIdx, m_dwSubQuestIdx );					// 서브 퀘스트를 종료하는 함수를 호출한다.
		}
		break;

	case eQuestExecute_EndOtherSub:														// 다른 서브 퀘스트를 종료하는 실행이라면,
		{
			pQuestGroup->EndSubQuest( m_dwExeQuestIdx, m_dwExeSubQuestIdx );			// 다른 서브 퀘스트를 종료하는 함수를 호출한다.
		}
		break;

	case eQuestExecute_RegenMonster:													// 몬스터를 리젠 하는 실행히면,
		{
			pQuestGroup->RegenMonster( m_dwExeQuestIdx );								// 몬스터를 리젠 하는 함수를 호출한다.
		}
		break;

	case eQuestExecute_MapChange:														// 맵 체인지를 하는 실행이면,
		{
			// 080104 LYW --- QuestExecute_Quest : 맵 변경을 하는 기능을 수정함.
			pQuestGroup->MapChange( m_DestinationMapNum, m_fDestination_XPos, m_fDestination_ZPos ) ;
			//pQuestGroup->MapChange( (WORD)m_dwExeQuestIdx, (WORD)m_dwExeSubQuestIdx );	// 맵 체인지를 하는 함수를 호출한다.
		}
		break;

	case eQuestExecute_ChangeStage:														// 스테이지를 바꾸는 실행이라면,
		{
			pQuestGroup->ChangeStage( m_dwExeQuestIdx, m_dwExeSubQuestIdx );			// 스테이지를 바꾸는 함수를 호출한다.
		}
		break;
	}
#endif	// _MAPSERVER_
	return TRUE;
}

int CQuestExecute_Quest::CheckCondition( PLAYERTYPE* pPlayer,							// 퀘스트 이벤트를 실행하기 위한 조건을 만족하는지 체크하는 함수.
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

	//switch( m_dwExecuteKind )															// 실행 종류를 확인한다.
	//{
	//case eQuestExecute_EndQuest:														// 퀘스트를 종료하는 실행이라면,
	//	{			
	//		return nErrorCode ;															// 에러 코드를 return 처리를 한다.
	//	}
	//	break;		

	//case eQuestExecute_StartSub:														// 서브 퀘스트를 시작하는 실행이라면,
	//	{
	//		return nErrorCode ;															// 에러 코드를 return 처리를 한다.
	//	}
	//	break;

	//case eQuestExecute_EndSub:															// 서브 퀘스트를 종료하는 실행이라면,
	//	{
	//		return nErrorCode ;															// 에러 코드를 return 처리를 한다.
	//	}
	//	break;

	//case eQuestExecute_EndOtherSub:														// 다른 서브 퀘스트를 종료하는 실행이라면,
	//	{
	//		return nErrorCode ;															// 에러 코드를 return 처리를 한다.
	//	}
	//	break;

	//case eQuestExecute_RegenMonster:													// 몬스터를 리젠 하는 실행히면,
	//	{
	//		return nErrorCode ;															// 에러 코드를 return 처리를 한다.
	//	}
	//	break;

	//case eQuestExecute_MapChange:														// 맵 체인지를 하는 실행이면,
	//	{
	//		return nErrorCode ;															// 에러 코드를 return 처리를 한다.
	//	}
	//	break;

	//case eQuestExecute_ChangeStage:														// 스테이지를 바꾸는 실행이라면,
	//	{
	//		return nErrorCode ;															// 에러 코드를 return 처리를 한다.
	//	}
	//	break;
	//}

#endif //_MAPSERVER_

	return nErrorCode ;																	// 기본 값을 리턴한다.
}

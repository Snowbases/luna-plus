// QuestExecute_Time.cpp: implementation of the QuestExecute_Time class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																			// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestExecute_Time.h"																// 퀘스트 실행 시간 헤더를 불러온다.

#include "QuestScriptLoader.h"																// 퀘스트 스크립트 로더 헤더를 불러온다.

#ifdef _MAPSERVER_																			// 맵 서버가 선언되어 있다면,

#include "QuestGroup.h"																		// 퀘스트 그룹 헤더를 불러온다.

#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 생성자.
CQuestExecute_Time::CQuestExecute_Time( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: CQuestExecute( dwExecuteKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	GetScriptParam( m_bType, pTokens );														// 타입을 받는다.
	GetScriptParam( m_dwDay, pTokens );														// 날짜를 받는다.
	GetScriptParam( m_dwHour, pTokens );													// 시간을 받는다.
	GetScriptParam( m_dwMin, pTokens );														// 분을 받는다.
}

CQuestExecute_Time::~CQuestExecute_Time()													// 소멸자 함수.
{
}

// 실행 함수.
BOOL CQuestExecute_Time::Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_																			// 맵 서버가 선언되어 있다면,

	switch( m_dwExecuteKind )																// 실행 종류를 확인한다.
	{
	case eQuestExecute_RegistTime:															// 시간 등록 실행이라면,
		{
			// 시간 체크를 등록하는 함수를 호출한다.
			pQuestGroup->RegistCheckTime( m_dwQuestIdx, m_dwSubQuestIdx, m_bType, m_dwDay, m_dwHour, m_dwMin );
		}
		break;
	}

#endif	

	return TRUE;																			// TRUE를 리턴한다.
}

int CQuestExecute_Time::CheckCondition( PLAYERTYPE* pPlayer, 
							CQuestGroup* pQuestGroup, CQuest* pQuest )			// 퀘스트 이벤트를 실행하기 위한 조건을 만족하는지 체크하는 함수.
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

#ifdef _MAPSERVER_																// 맵 서버가 선언되어 있다면,

#endif //_MAPSERVER_

	return nErrorCode ;															// 기본 값을 리턴한다.
}

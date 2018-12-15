// QuestExecute_Count.cpp: implementation of the CQuestExecute_Count class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"															// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestExecute_Count.h"												// 퀘스트 실행 카운트 헤더를 불러온다.

#include "QuestScriptLoader.h"												// 퀘스트 스크립트 로더 헤더를 불러온다.

#ifdef _MAPSERVER_															// 맵서버가 선언되어 있다면,

#include "QuestGroup.h"														// 퀘스트 그룹 헤더를 불러온다.

#include "Player.h"															// 플레이어 헤더를 불러온다.

#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 생성자 함수.
CQuestExecute_Count::CQuestExecute_Count( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: CQuestExecute( dwExecuteKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	m_dwMaxCount = m_dwWeaponKind = m_dwParam1 = 0;							// 최대 카운트, 무기 종류, 파라메터 1을 0으로 세팅한다.

	GetScriptParam( m_dwRealSubQuestIdx, pTokens );							// 리얼 서브 퀘스트 인덱스를 받는다.
	GetScriptParam( m_dwMaxCount, pTokens );								// 최대 카운트를 받는다.

	if( m_dwExecuteKind == eQuestExecute_AddCountFQW						// 실행 타입이 특정 무기로 카운트를 추가하는 것이라면,
		|| m_dwExecuteKind == eQuestExecute_AddCountFW )
	{
		GetScriptParam( m_dwWeaponKind, pTokens );							// 무기 타입을 받는다.
	}

	else if( m_dwExecuteKind == eQuestExecute_LevelGap ||						// 실행 타입이 레벨차이 라면,
		m_dwExecuteKind == eQuestExecute_MonLevel )
	{
		GetScriptParam( m_dwWeaponKind, pTokens );							// 무기 타입을 받는다.
		GetScriptParam( m_dwParam1, pTokens );								// 파라메터 1을 받는다.
	}
}

// 소멸자 함수.
CQuestExecute_Count::~CQuestExecute_Count()									
{
}

// 실행 함수.
BOOL CQuestExecute_Count::Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_															// 맵 서버가 선언되어 있다면,

	switch( m_dwExecuteKind )												// 실행 타입을 확인한다.
	{
	case eQuestExecute_AddCount:											// 카운트 추가라면,
		{
			// 퀘스트 그룹의 카운트를 증가 시키는 함수 호출.
			pQuestGroup->AddCount( m_dwQuestIdx, m_dwRealSubQuestIdx, m_dwMaxCount );
		}
		break;

	case eQuestExecute_AddCountFQW:											// 특정 무기 카운트 증가 라면,
		{
			// 퀘스트 그룹의 특정 무기로 부터 카운트를 증가 시키는 함수 호출.
			pQuestGroup->AddCountFromQWeapon( m_dwQuestIdx, m_dwRealSubQuestIdx, m_dwMaxCount, m_dwWeaponKind );
		}
		break;

	case eQuestExecute_AddCountFW:											// 특정 무기 카운트 증가라면,
		{
			// 퀘스트 그룹의 특정 무기로 부터 카운트를 증가 시키는 함수 호출.
			pQuestGroup->AddCountFromWeapon( m_dwQuestIdx, m_dwRealSubQuestIdx, m_dwMaxCount, m_dwWeaponKind );
		}
		break;
	case eQuestExecute_LevelGap:											// 레벨 차이라면,
		{
			// 퀘스트 그룹의 레벨 차이로 부터 카운트를 증가 시키는 함수 호출.
			pQuestGroup->AddCountFromLevelGap( m_dwQuestIdx, m_dwRealSubQuestIdx, m_dwMaxCount, m_dwWeaponKind, m_dwParam1 );
		}
		break;
	case eQuestExecute_MonLevel:											// 몬스터 레벨 이라면,
		{
			// 퀘스트 그룹의 몬스터 레벨로 부터 카운트를 증가 시키는 함수 호출.
			pQuestGroup->AddCountFromMonLevel( m_dwQuestIdx, m_dwRealSubQuestIdx, m_dwMaxCount, m_dwWeaponKind, m_dwParam1 );
		}
		break;
	}
#endif

	return TRUE;															// TRUE를 리턴한다.
}

int CQuestExecute_Count::CheckCondition( PLAYERTYPE* pPlayer,					// 퀘스트 이벤트를 실행하기 위한 조건을 만족하는지 체크하는 함수.
							CQuestGroup* pQuestGroup, CQuest* pQuest )			
{
	int nErrorCode = e_EXC_ERROR_NO_ERROR ;										// 에러 코드를 담을 변수를 선언하고 e_EXE_ERROR_NO_ERROR로 세팅한다.

	if( !pPlayer )																// 플레이어 정보가 유효하지 않으면, 
	{
		nErrorCode = e_EXC_ERROR_NO_PLAYERINFO ;								// 플레이어 정보가 없다는 에러 코드를 세팅한다.
		return nErrorCode ;														// 에러 코드를 return 처리를 한다.
	}

	if( !pQuestGroup )															// 퀘스트 그룹 정보가 유효하지 않으면,
	{
		nErrorCode = e_EXC_ERROR_NO_QUESTGROUP ;								// 퀘스트 그룹 정보가 없다는 에러 코드를 세팅한다.
		return nErrorCode ;														// 에러 코드를 return 처리를 한다.
	}

	if( !pQuest )																// 퀘스트 정보가 유효하지 않다면,
	{
		nErrorCode = e_EXC_ERROR_NO_QUEST ;										// 퀘스트 정보가 없다는 에러 코드를 세팅한다.
		return nErrorCode ;														// 에러 코드를 return 처리를 한다.
	}

#ifdef _MAPSERVER_																// 맵 서버가 선언되어 있다면,

	switch( m_dwExecuteKind )													// 실행 타입을 확인한다.
	{
	case eQuestExecute_AddCount:												// 카운트 추가라면,
		{
			return nErrorCode ;													// 에러 코드를 return 처리를 한다.
		}
		break;

	case eQuestExecute_AddCountFQW:												// 특정 무기를 체크해서 퀘스트 아이템을 받는 실행이면,
		{
			if( pPlayer->GetWearedWeapon() != m_dwWeaponKind )					// 플레이어가 착용하고 있는 무기와 비교대상 무기가 같지 않으면,
			{
				nErrorCode = e_EXC_ERROR_NOT_SAME_WEAPONKIND_COUNT ;			// 무기 종류가 달라서 퀘스트 카운트 추가를 못한다는 에러 메시지를 세팅한다.
				return nErrorCode ;												// 에러 코드를 return 처리를 한다.
			}
		}
		break;

	case eQuestExecute_AddCountFW:												// 무기 계열을 체크해서 퀘스트 아이템을 받는 실행이면,
		{
			return nErrorCode ;													// 에러 코드를 return 처리를 한다.

			// 이 처리는 사용하지 않기로 결정.
		}
		break;
	case eQuestExecute_LevelGap:												// 레벨 차이라면,
		{
			return nErrorCode ;													// 에러 코드를 return 처리를 한다.
			/*int nMin = pPlayer->GetLevel() - pPlayer->GetKillMonsterLevel();
			int nMax = pPlayer->GetKillMonsterLevel() - pPlayer->GetLevel();
			if( nMin > 0 && nMin > (int)dwMinLevel )	return;
			if( nMax > 0 && nMax > (int)dwMaxLevel )	return;*/
		}
		break;
	case eQuestExecute_MonLevel:												// 몬스터 레벨 이라면,
		{
			return nErrorCode ;													// 에러 코드를 return 처리를 한다.
			/*WORD wMonLevel = pPlayer->GetKillMonsterLevel();

			if( wMonLevel < dwMinLevel )	return;
			if( wMonLevel > dwMaxLevel )	return;*/
		}
		break;
	}

#endif //_MAPSERVER_

	return nErrorCode ;															// 기본 값을 리턴한다.
}

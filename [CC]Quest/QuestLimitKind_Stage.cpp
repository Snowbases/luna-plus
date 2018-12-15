




#include "stdafx.h"															// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include ".\questlimitkind_stage.h"											// 퀘스트 리미트 종류 스테이지 헤더를 불러온다.

#include "QuestScriptLoader.h"												// 퀘스트 스크립트 로더 헤더를 불러온다.

#ifdef _MAPSERVER_															// 맵서버가 선언되어 있으면,

#include "Player.h"															// 플레이어 헤더를 불러온다.

#else																		// 맵서버가 선언되어 있지 않으면,

#include "ObjectManager.h"													// 오브젝트 매니져 헤더를 불러온다.

#endif //_MAPSERVER_

// 생성자 함수.
CQuestLimitKind_Stage::CQuestLimitKind_Stage(DWORD dwLimitKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx)
: CQuestLimitKind( dwLimitKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	byClass    = 0 ;												// 클래스를 받을 변수를 0으로 세팅한다.
	byRacial   = 0 ;												// 종족을 받을 변수를 0으로 세팅한다.
	byJobGrade = 0 ;												// 잡 그레이드를 받을 변수를 0으로 세팅한다.
	byJobIndex = 0 ;												// 잡 인덱스를 받을 변수를 0으로 세팅한다.

	dwClassIndex = 0 ;												// 클래스 인덱스를 받을 변수를 0으로 세팅한다.

	m_pClassIndex = NULL ;											// 클래스 인덱스 포인터를 null 처리를 한다.

	m_byCount = 0 ;													// 카운트 변수를 0으로 세팅한다.
    
	if( m_dwLimitKind == eQuestLimitKind_Stage )					// 리미트 종류가 스테이지와 같으면,
	{
		GetScriptParam( m_byCount, pTokens ) ;						// 카운트를 받는다.
	}

	if( m_byCount > 0 )												// 카운트가 0보다 크면,
	{
		m_pClassIndex = new DWORD[m_byCount] ;						// 클래스 인덱스를 담을 메모리를 할당한다.
	}
	else															// 그렇지 않으면,
	{
		return ;													// 리턴 처리를 한다.
	}

	for( BYTE count = 0 ; count < m_byCount ; ++count )				// 카운트 수 만큼 for문을 돌린다.
	{
		GetScriptParam( m_pClassIndex[count], pTokens ) ;			// 클래스 인덱스를 받는다.
	}
}

CQuestLimitKind_Stage::~CQuestLimitKind_Stage(void)					// 소멸자 함수.
{
	if( m_pClassIndex )												// 클래스 인덱스 정보가 유효한지 체크한다.
	{
		delete [] m_pClassIndex ;									// 클래스 인덱스를 삭제한다.
		m_pClassIndex = NULL ;										// 클래스 인덱스 포인터를 NULL 처리를 한다.
	}
}

// 퀘스트 리미트 체크 함수.(서버)
BOOL CQuestLimitKind_Stage::CheckLimit( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_													// 맵 서버가 선언되어 있다면,

	switch(m_dwLimitKind)											// 리미트 종류를 확인한다.
	{
	case eQuestLimitKind_Stage :									// 리미트 종류가 스테이지와 같다면,
		{
			CHARACTER_TOTALINFO totalInfo ;							// 퀘스트 토탈 정보를 받을 구조체를 선언한다.

			memset(&totalInfo, 0, sizeof(CHARACTER_TOTALINFO)) ;	// 구조체 정보를 메모리 셋 한다.

			pPlayer->GetCharacterTotalInfo(&totalInfo) ;			// 플레이어의 캐릭터 토탈 정보를 받는다.

			BYTE byClass    	 = totalInfo.Job[0] ;				// 클래스 값을 받는다.
			BYTE byRacial   	 = totalInfo.Race+1 ;				// 종족 값을 받는다.
			BYTE byJobGrade 	 = totalInfo.JobGrade ;				// 잡 그레이드 값을 받는다.
			BYTE byJobIndex 	 = 0 ;								// 잡 인덱스를 0으로 세팅한다.
	
			if(byJobGrade == 1)										// 잡 그레이드가 1과 같다면,
			{	
				byJobIndex = 1 ;									// 잡 인덱스를 1로 세팅한다.
			}
			else													// 잡 그레이드가 1이 아니라면,
			{
				byJobIndex = totalInfo.Job[byJobGrade-1] ;			// 잡 인덱스를 받는다.
			}

			for( BYTE count = 0 ; count < m_byCount ; ++count )		// 카운트 수 만큼 for문을 돌린다.
			{
				if( m_pClassIndex[count] <= 1000 ) continue ;		// 카운트에 해당하는 클래스 인덱스가 1000보다 작으면, 계속~

				// 클래스 인덱스를 생성한다.
				DWORD dwLimitClass	  = m_pClassIndex[count] / 1000 ;
				DWORD dwLimitRacial	  = (m_pClassIndex[count] - (1000*dwLimitClass)) / 100 ;
				DWORD dwLimitGrade	  = (m_pClassIndex[count] - (1000*dwLimitClass) - (100*dwLimitRacial)) / 10 ;
				DWORD dwLimitJobIndex = (m_pClassIndex[count] - (1000*dwLimitClass) - (100*dwLimitRacial) - (10*dwLimitGrade)) ;

				// 스테이지 제한을 만족하면,
				if( (dwLimitClass == byClass) && (dwLimitRacial == byRacial) && (dwLimitGrade == byJobGrade) && (dwLimitJobIndex == byJobIndex) )
				{
					return TRUE ;									// TRUE를 리턴한다.
				}
			}

			return FALSE ;											// FALSE를 리턴한다.
		}
		break ;
	}
#endif // _MAPSERVER_

	return FALSE ;													// FALSE를 리턴한다.
}

// 퀘스트 리미트 체크 함수.( 클라이언트 )
BOOL CQuestLimitKind_Stage::CheckLimit( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
#ifndef _MAPSERVER_													// 맵 서버가 선언되어 있지 않다면,

	switch(m_dwLimitKind)											// 리미트 종류를 확인한다.
	{
	case eQuestLimitKind_Stage :									// 리미트 종류가 스테이지와 같으면,
		{
			CHARACTER_TOTALINFO totalInfo ;							// 캐릭터 토탈 정보를 받을 구조체를 선언한다.

			memset(&totalInfo, 0, sizeof(CHARACTER_TOTALINFO)) ;	// 캐릭터 토탈 정보를 받을 구조체를 메모리 셋한다.

			HERO->GetCharacterTotalInfo(&totalInfo) ;				// HERO의 캐릭터 토탈 정보를 받는다.

			BYTE byClass    	 = totalInfo.Job[0] ;				// 클래스 값을 받는다.
			BYTE byRacial   	 = totalInfo.Race+1 ;				// 종족 값을 받는다.
			BYTE byJobGrade 	 = totalInfo.JobGrade ;				// 잡 그레이드 값을 받는다.
			BYTE byJobIndex 	 = 0 ;								// 잡 인덱스를 0으로 세팅한다.

			if(byJobGrade == 1)										// 잡 그레이드가 1과 같으면,
			{
				byJobIndex = 1 ;									// 잡 인덱스를 1로 세팅한다.
			}
			else													// 잡 그레이드가 1과 같지 않으면,
			{
				byJobIndex = totalInfo.Job[byJobGrade-1] ;			// 잡 인덱스를 받는다.
			}

			for( BYTE count = 0 ; count < m_byCount ; ++count )		// 카운트 수 만큼 for문을 돌린다.
			{
				if( m_pClassIndex[count] <= 1000 ) continue ;		// 클래스 인덱스가 1000 이하라면,

				// 클래스 인덱스를 생성한다.
				DWORD dwLimitClass	  = m_pClassIndex[count] / 1000 ;
				DWORD dwLimitRacial	  = (m_pClassIndex[count] - (1000*dwLimitClass)) / 100 ;
				DWORD dwLimitGrade	  = (m_pClassIndex[count] - (1000*dwLimitClass) - (100*dwLimitRacial)) / 10 ;
				DWORD dwLimitJobIndex = (m_pClassIndex[count] - (1000*dwLimitClass) - (100*dwLimitRacial) - (10*dwLimitGrade)) ;

				// 스테이지 제한을 만족하면,
				if( (dwLimitClass == byClass) && (dwLimitRacial == byRacial) && (dwLimitGrade == byJobGrade) && (dwLimitJobIndex == byJobIndex) )
				{
					return TRUE ;									// TRUE를 리턴한다.
				}
			}

			return FALSE ;											// FALSE를 리턴한다.
		}
		break ;
	}
#endif//_MAPSERVER_
	return FALSE ;													// FALSE를 리턴한다.
}

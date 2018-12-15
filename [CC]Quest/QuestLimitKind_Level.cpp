// QuestLimitKind_Level.cpp: implementation of the CQuestLimitKind_Level class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"														// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestLimitKind_Level.h"										// 퀘스트 리미트 종류 레벨 헤더를 불러온다.

#include "QuestScriptLoader.h"											// 퀘스트 스크립트 로더 헤더를 불러온다.

#ifdef _MAPSERVER_														// 맵 서버가 선언 되어 있다면,

#include "Player.h"														// 플레이어 헤더파일을 불러온다.

#else																	// 맵 서버가 선언되지 않았다면,

#include "ObjectManager.h"												// 오브젝트 매니져 헤더를 불러온다.

#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 생성자 함수.
CQuestLimitKind_Level::CQuestLimitKind_Level( DWORD dwLimitKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: CQuestLimitKind( dwLimitKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	m_dwMin = m_dwMax = 0;												// 최소, 최대 레벨 값을 0으로 세팅한다.

	if( m_dwLimitKind == eQuestLimitKind_Level )						// 퀘스트 리미트 종류가 레벨 제한과 같다면,
	{
		GetScriptParam( m_dwMin, pTokens );								// 최소 레벨 값을 받는다.
		GetScriptParam( m_dwMax, pTokens );								// 최대 레벨 값을 받는다.
	}
	else if( m_dwLimitKind == eQuestLimitKind_Money )					// 퀘스트 리미트 종류가 머니 제한과 같다면,
	{
		GetScriptParam( m_dwMax, pTokens );								// 최대 머니 값을 받는다.
	}
	else if( m_dwLimitKind == eQuestLimitKind_Stage )					// 퀘스트 리미트 종류가 스테이지와 같다면,
	{
		GetScriptParam( m_dwMin, pTokens );								// 최소 레벨 값을 받는다.
		GetScriptParam( m_dwMax, pTokens );								// 최대 레벨 값을 받는다.
	}
	else if( m_dwLimitKind == eQuestLimitKind_Attr )					// 퀘스트 리미트 값이 Attr과 같으면,
	{
		GetScriptParam( m_dwMin, pTokens );								// 최소 값을 받는다.
	}
}

CQuestLimitKind_Level::~CQuestLimitKind_Level()							// 소멸자 함수.
{
}

// 퀘스트 리미트 체크함수.(서버)
BOOL CQuestLimitKind_Level::CheckLimit( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_														// 맵서버가 선언되어 있다면,

	switch( m_dwLimitKind )												// 리미트 종류를 확인한다.
	{
	case eQuestLimitKind_Level:											// 리미트 종류가 레벨과 같다면,
		{
			DWORD dwlevel = pPlayer->GetLevel();						// 플레이어의 레벨을 받는다.

			if( dwlevel >= m_dwMin && dwlevel <= m_dwMax )				// 최소레벨 이상, 최대 레벨 이하면,
			{
				return TRUE;											// TRUE를 리턴한다.
			}
		}
		break;

	case eQuestLimitKind_Money:											// 리미트 종류가 머니와 같다면,
		{
			MONEYTYPE Money = pPlayer->GetMoney();						// 플레이어의 머니를 받는다.

			if( Money >= m_dwMax )										// 플레이어의 머니가 최대 머니보다 크다면,
			{
				return TRUE;											// TRUE를 리턴한다.
			}
		}
		break;

	case eQuestLimitKind_Stage:											// 리미트 종류가 스테이지와 같다면,
		{
			BYTE bStage = pPlayer->GetStage();							// 플레이어의 스테이지를 받는다. ( 몇단계, 무슨 직업인가?)

			if( bStage == (BYTE)m_dwMin || bStage == (BYTE)m_dwMax )	// 최소 단계 이상, 최대 단계 이하면,
			{
				return TRUE;											// TRUE를 리턴한다.
			}
		}
		break;
	}
#endif

	return FALSE;														// FALSE를 리턴한다.
}

// 퀘스트 리미트 체크함수.(클라이언트)
BOOL CQuestLimitKind_Level::CheckLimit( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
#ifndef _MAPSERVER_														// 맵서버가 선언되어 있지 않으면,

	switch( m_dwLimitKind )												// 퀘스트 리미트 종류를 확인한다.
	{
	case eQuestLimitKind_Level:											// 퀘스트 리미트 종류가 레벨 제한이라면,
		{
			DWORD dwlevel = HERO->GetLevel();							// HERO의 레벨을 받는다.

			if( dwlevel >= m_dwMin && dwlevel <= m_dwMax )				// HERO의 레벨이 최소 레벨 이상, 최대 레벨 이하면,
			{
				return TRUE;											// TRUE를 리턴한다.
			}
		}
		break;

	case eQuestLimitKind_Money:											// 퀘스트 리미트 종류가 머니라면,
		{
			MONEYTYPE Money = HERO->GetMoney();							// HERO의 머니를 받는다.

			if( Money >= m_dwMax )										// HERO의 머니가 최대 머니 이상이면,
			{
				return TRUE;											// TRUE를 리턴한다.
			}
		}
		break;

	case eQuestLimitKind_Stage:											// 퀘스트 리미트 종류가 스테이지와 같다면,
		{
			BYTE bStage = HERO->GetStage();								// HERO의 스테이지를 받는다.(몇단계, 어떤직업인가?)

			if( bStage == (BYTE)m_dwMin || bStage == (BYTE)m_dwMax )	// 최소 단계 이상, 최대 단계 이하라면,
			{
				return TRUE;											// TRUE를 리턴한다.
			}
		}
		break;
	}
#endif

	return FALSE;														// FALSE를 리턴한다.
}
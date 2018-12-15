// QuestEvent.cpp: implementation of the CQuestEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																	// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestEvent.h"																// 퀘스트 이벤트 헤더 파일을 불러온다.

#include "StrTokenizer.h"															// 스트링 토크나이져 헤더를 불러온다.
#include "QuestScriptLoader.h"														// 퀘스트 스크립트 로더 헤더를 불러온다.

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 생성자 함수.
CQuestEvent::CQuestEvent( DWORD dwQuestEventKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: m_dwQuestEventKind(dwQuestEventKind)
{
	GetScriptParam( m_dwParam1, pTokens );											// 파라메터 1을 세팅한다.
	GetScriptParam( m_lParam2, pTokens );											// 파라메터 2를 세팅한다.
}
CQuestEvent::CQuestEvent( DWORD dwQuestEventKind, DWORD dwParam1, LONG lParam2 )	// 생성자 함수.
{
	SetValue(dwQuestEventKind,dwParam1,lParam2);									// 파라메터 값을 가지고 퀘스트 이벤트를 세팅한다.
}

CQuestEvent::~CQuestEvent()															// 소멸자 함수.
{
}

void CQuestEvent::SetValue( CQuestEvent* pQuestEvent )								// 퀘스트 이벤트를 세팅하는 함수.
{
	m_dwQuestEventKind = pQuestEvent->m_dwQuestEventKind;							// 퀘스트 이벤트 종류를 세팅한다.
	m_dwParam1 = pQuestEvent->m_dwParam1;											// 파라메터 1을 세팅한다.
	m_lParam2 = pQuestEvent->m_lParam2;												// 파라메터 2를 세팅한다.
}

void CQuestEvent::SetValue( DWORD dwQuestEventKind, DWORD dwParam1, LONG lParam2 )	// 퀘스트 이벤트를 세팅하는 함수.
{
	m_dwQuestEventKind = dwQuestEventKind;											// 퀘스트 이벤트 중류를 세팅한다.
	m_dwParam1 = dwParam1;															// 파라메터 1을 세팅한다.
	m_lParam2 = lParam2;															// 파라메터 2를 세팅한다.
}

BOOL CQuestEvent::IsSame( CQuestEvent* pQuestEvent )								// 같은 퀘스트 이벤트인지 여부를 리턴하는 함수.
{
	if( m_dwQuestEventKind != pQuestEvent->m_dwQuestEventKind )						// 퀘스트 이벤트가 같지 않으면,
	{
		return FALSE;																// FALSE 리턴 처리를 한다.
	}

	if( m_dwParam1 != pQuestEvent->m_dwParam1 )										// 파라메터 1이 같지 않으면,
	{
		return FALSE;																// FALSE 리턴 처리를 한다.
	}

	if( m_lParam2 != pQuestEvent->m_lParam2 )										// 파라메터 2가 같지 않으면,
	{
		return FALSE;																// FALSE 리턴 처리를 한다.
	}

	return TRUE;																	// TRUE 리턴 처리를 한다.
}

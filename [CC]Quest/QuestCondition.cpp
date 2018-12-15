// QuestCondition.cpp: implementation of the CQuestCondition class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"															// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestCondition.h"													// 퀘스트 상태 헤더를 불러온다.
#include "QuestDefines.h"													// 퀘스트 정의 선언 헤더를 불러온다.
#include "QuestEvent.h"														// 퀘스트 이벤트 헤더를 불러온다.

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 생성자 함수.
CQuestCondition::CQuestCondition( CQuestEvent* pQuestEvent, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	m_dwQuestIdx = dwQuestIdx;												// 퀘스트 인덱스를 세팅한다.
	m_dwSubQuestIdx = dwSubQuestIdx;										// 서브 퀘스트 인덱스를 세팅한다.
	m_pQuestEvent = pQuestEvent;											// 퀘스트 이벤트를 받는다.
}
// 소멸자 함수.
CQuestCondition::~CQuestCondition()
{
	if( m_pQuestEvent )														// 퀘스트 이벤트 정보가 유효하면,
	{
		delete m_pQuestEvent;												// 퀘스트 이벤트 정보를 삭제한다.
	}
}

BOOL CQuestCondition::CheckCondition( CQuestEvent* pQuestEvent )			// 상태를 체크하는 함수.
{
	if( m_pQuestEvent->m_dwQuestEventKind == eQuestEvent_HuntAll )			// 퀘스트 이벤트 종류가 모두 사냥이면,
	{
		if( pQuestEvent->m_dwQuestEventKind == eQuestEvent_Hunt )			// 퀘스트 이벤트 종류가 사냥과 같다면,
		{
			return TRUE;													// TRUE를 리턴한다.
		}
		else																// 퀘스트 이벤트가 사냥이 아니면,
		{
			return FALSE;													// FALSE를 리턴한다.
		}
	}

	return m_pQuestEvent->IsSame( pQuestEvent );							// 퀘스트가 같은지 여부를 리턴한다.
}
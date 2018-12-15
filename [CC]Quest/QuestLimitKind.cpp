// QuestLimitKind.cpp: implementation of the CQuestLimitKind class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"															// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestLimitKind.h"													// 퀘스트 리미트 종류 헤더 파일을 불러온다.

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 생성자 함수.
CQuestLimitKind::CQuestLimitKind( DWORD dwLimitKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	m_dwQuestIdx = dwQuestIdx;						// 퀘스트 인덱스를 세팅한다.
	m_dwSubQuestIdx = dwSubQuestIdx;				// 서브 퀘스트 인덱스를 세팅한다.
	m_dwLimitKind = dwLimitKind;					// 퀘스트 리미트 종류를 세팅한다.
}

// 소멸자 함수.
CQuestLimitKind::~CQuestLimitKind()
{
}
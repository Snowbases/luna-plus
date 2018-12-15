// QuestLimitKind_Level.h: interface for the CQuestLimitKind_Level class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTLIMITKIND_LEVEL_H__8B4328F3_3190_4E4D_82D4_184C42A6B07C__INCLUDED_)
#define AFX_QUESTLIMITKIND_LEVEL_H__8B4328F3_3190_4E4D_82D4_184C42A6B07C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestLimitKind.h"											// 퀘스트 리미트 종류 헤더를 불러온다.

class CQuestLimitKind_Level : public CQuestLimitKind				// 퀘스트 리미트 종류 클래스를 상속 받은, 퀘스트 리미트 종류 레벨 클래스.
{
protected:
	DWORD		m_dwMin;											// 최소 값을 담을 변수.
	DWORD		m_dwMax;											// 최대 값을 담을 변수.

public:
	CQuestLimitKind_Level( DWORD dwLimitKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestLimitKind_Level();																			// 소멸자 함수.

	virtual BOOL	CheckLimit( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );				// 퀘스트 리미트 체크함수.
	virtual BOOL	CheckLimit( DWORD dwQuestIdx, DWORD dwSubQuestIdx );										// 퀘스트 리미트 체크함수.

	DWORD	GetMin() { return m_dwMin; }
	DWORD	GetMax() { return m_dwMax; }
};

#endif // !defined(AFX_QUESTLIMITKIND_LEVEL_H__8B4328F3_3190_4E4D_82D4_184C42A6B07C__INCLUDED_)

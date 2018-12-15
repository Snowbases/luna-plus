// QuestLimitKind_Quest.h: interface for the CQuestLimitKind_Quest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTLIMITKIND_QUEST_H__F6BD9EEC_1613_4D16_B5B9_999DEF6F0864__INCLUDED_)
#define AFX_QUESTLIMITKIND_QUEST_H__F6BD9EEC_1613_4D16_B5B9_999DEF6F0864__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestLimitKind.h"												// 퀘스트 리미트 종류 헤더를 불러온다.

class CQuestLimitKind_Quest : public CQuestLimitKind					// 퀘스트 리미트 종류 클래스를 상속받은 퀘스트 리미트 퀘스트 클래스.
{
protected:
	DWORD		m_dwIdx;												// 퀘스트 인덱스를 담을 변수.

public:
	CQuestLimitKind_Quest( DWORD dwLimitKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestLimitKind_Quest();																			// 소멸자 함수.

	virtual BOOL	CheckLimit( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );				// 리미트 체크 함수(서버)
	virtual BOOL	CheckLimit( DWORD dwQuestIdx, DWORD dwSubQuestIdx );										// 리미트 체크 함수(클라이언트)

	DWORD	GetIdx() { return m_dwIdx; }
};

#endif // !defined(AFX_QUESTLIMITKIND_QUEST_H__F6BD9EEC_1613_4D16_B5B9_999DEF6F0864__INCLUDED_)

// QuestLimitKind.h: interface for the CQuestLimitKind class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTLIMITKIND_H__4048A25A_091D_4BEB_986E_3C324C0D32EF__INCLUDED_)
#define AFX_QUESTLIMITKIND_H__4048A25A_091D_4BEB_986E_3C324C0D32EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestDefines.h"																		// 퀘스트 정의 선언 헤더파일을 불러온다.

class CStrTokenizer;																			// 스트링 토크나이져 클래스를 선언한다.
class CQuestGroup;																				// 퀘스트 그룹 클래스를 선언한다.
class CQuest;																					// 퀘스트 클래스를 선언한다.

class CQuestLimitKind																			// 퀘스트 리미트 클래스.
{
protected:
	DWORD	m_dwQuestIdx;																		// 퀘스트 인덱스를 담을 변수.
	DWORD	m_dwSubQuestIdx;																	// 서브 퀘스트 인덱스를 담을 변수.
	DWORD	m_dwLimitKind;																		// 퀘스트 리미트 종류를 담을 변수.
	
public:
	CQuestLimitKind( DWORD dwLimitKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestLimitKind();																				// 소멸자 함수.

	virtual BOOL	CheckLimit( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest ) = 0;		// 퀘스트 리미트 체크 함수.
	virtual BOOL	CheckLimit( DWORD dwQuestIdx, DWORD dwSubQuestIdx ) = 0;								// 퀘스트 리미트 체크 함수.

	// 070424 LYW --- QuestLimitKind : Add function to return limit kind.
	DWORD GetLimitKind() { return m_dwLimitKind ; }															// 퀘스트 리미트 종류를 반환하는 함수.
};

#endif // !defined(AFX_QUESTLIMITKIND_H__4048A25A_091D_4BEB_986E_3C324C0D32EF__INCLUDED_)

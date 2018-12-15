// QuestLimit.h: interface for the CQuestLimit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTLIMIT_H__B72ED18D_6647_469A_B38E_3BD1B295B469__INCLUDED_)
#define AFX_QUESTLIMIT_H__B72ED18D_6647_469A_B38E_3BD1B295B469__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestDefines.h"																		// 퀘스트 정의 선언 헤더파일을 불러온다.

class CStrTokenizer;																			// 스트링 토크나이져 클래스를 선언한다.
class CQuestGroup;																				// 퀘스트 그룹 클래스를 선언한다.
class CQuest;																					// 퀘스트 클래스를 선언한다.

class CQuestLimit																				// 퀘스트 제한 클래스.
{
protected:
	cPtrList	m_QuestLimitKindList;															// 퀘스트 제한 종류 리스트.

public:
	CQuestLimit( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );				// 생성자 함수.
	virtual ~CQuestLimit();																		// 소멸자 함수.

	void	Release();																			// 해제 함수.
	// script
	BOOL	ReadLimitKind( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );		// 제한 종류를 읽는 함수.
	// check
	BOOL	CheckLimit( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );		// 퀘스트 제한을 체크한다. - server
	BOOL	CheckLimit( DWORD dwQuestIdx, DWORD dwSubQuestIdx );								// 퀘스트 제한을 체크한다. - client

	cPtrList* GetLimitKindList() { return &m_QuestLimitKindList ; }
};

#endif // !defined(AFX_QUESTLIMIT_H__B72ED18D_6647_469A_B38E_3BD1B295B469__INCLUDED_)

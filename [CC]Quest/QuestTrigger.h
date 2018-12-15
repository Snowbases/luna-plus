// QuestTrigger.h: interface for the CQuestTrigger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTTRIGGER_H__E9012D36_44B8_469F_A5D6_4E42CDE29383__INCLUDED_)
#define AFX_QUESTTRIGGER_H__E9012D36_44B8_469F_A5D6_4E42CDE29383__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestDefines.h"															// 퀘스트 정의 선언 헤더파일을 불러온다.

class CQuestCondition;																// 퀘스트 상태 클래스를 선언한다.
class CStrTokenizer;																// 스트링 토크나이져 클래스를 선언한다.

class CQuestGroup;																	// 퀘스트 그룹 클래스를 선언한다.
class CQuest;																		// 퀘스트 클래스를 선언한다.
class CQuestEvent;																	// 퀘스트 이벤트 클래스를 선언한다.

class CQuestTrigger																	// 퀘스트 트리거 클래스.
{
protected:
	CQuestCondition*	m_pQuestCondition;											// 퀘스트 상태 클래스 포인터.
	cPtrList			m_QuestExeList;												// 퀘스트 실행 리스트.
	DWORD				m_dwEndParam;												// 종료 파라메터.

public:
	CQuestTrigger( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestTrigger();														// 소멸자 함수.

	void	Release();																// 해제 함수.
	
	// script
	BOOL	ReadTrigger( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );							// 트리거를 읽어들이는 함수.

	// event
	BOOL	OnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest, CQuestEvent* pQuestEvent );// 퀘스트 이벤트를 처리하는 함수.	

	//
	DWORD	GetEndParam()		{ return m_dwEndParam;	}															// 종료 파라메터를 반환하는 함수.

	cPtrList* GetExeList() { return &m_QuestExeList ; }
	CQuestCondition* GetQuestCondition() { return m_pQuestCondition; }
};

#endif // !defined(AFX_QUESTTRIGGER_H__E9012D36_44B8_469F_A5D6_4E42CDE29383__INCLUDED_)

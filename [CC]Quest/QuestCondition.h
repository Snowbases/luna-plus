// QuestCondition.h: interface for the CQuestCondition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTCONDITION_H__ED1643D9_D166_4EB4_B20B_C085F28ED8E6__INCLUDED_)
#define AFX_QUESTCONDITION_H__ED1643D9_D166_4EB4_B20B_C085F28ED8E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQuestEvent;																		// 퀘스트 이벤트 클래스를 선언한다.

class CQuestCondition																	// 퀘스트 상태 클래스.
{
protected:
	DWORD			m_dwQuestIdx;														// 퀘스트 인덱스를 담을 변수.
	DWORD			m_dwSubQuestIdx;													// 서브 퀘스트 인덱스를 담을 변수.
	CQuestEvent*	m_pQuestEvent;														// 퀘스트 이벤트 포인터 변수.

public:
	CQuestCondition( CQuestEvent* pQuestEvent, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestCondition();															// 소멸자 함수.

	CQuestEvent*	GetQuestEvent()		{ return m_pQuestEvent; }						// 퀘스트 이벤트를 반환하는 함수.

	BOOL	CheckCondition( CQuestEvent* pQuestEvent );									// 상태를 체크하는 함수.
	BOOL	IsSameQuest( DWORD dwQuestIdx )	{ return (m_dwQuestIdx == dwQuestIdx) ? TRUE : FALSE; }		// 같은 퀘스트인지 체크하는 함수.
};

#endif // !defined(AFX_QUESTCONDITION_H__ED1643D9_D166_4EB4_B20B_C085F28ED8E6__INCLUDED_)

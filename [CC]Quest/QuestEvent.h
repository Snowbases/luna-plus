// QuestEvent.h: interface for the CQuestEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTEVENT_H__0FAE1D70_917B_4C7D_A672_60DE84A4413B__INCLUDED_)
#define AFX_QUESTEVENT_H__0FAE1D70_917B_4C7D_A672_60DE84A4413B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStrTokenizer;														// 스트링 토크나이져 클래스를 선언한다.

class CQuestEvent															// 퀘스트 이벤트 클래스.
{
public:
	DWORD	m_dwQuestEventKind;												// 퀘스트 이벤트 종류를 담는 변수.
	DWORD	m_dwParam1;														// 파라메터 1.
	LONG	m_lParam2;														// 파라메터 2.
	
public:
	CQuestEvent() : m_dwQuestEventKind(0), m_dwParam1(0), m_lParam2(0)	{}	// 생성자 함수.	기본 인자를 0으로 세팅한다.
	CQuestEvent( DWORD dwQuestEventKind, DWORD dwParam1, LONG lParam2 );	// 생성자 함수. 기본 인자를 받는다.
	CQuestEvent( DWORD dwQuestEventKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수. 기본인자를 받아들인 값으로 세팅한다.
	virtual ~CQuestEvent();													// 소멸자 함수.
	
	void	SetValue( CQuestEvent* pQuestEvent );							// 퀘스트 이벤트 값을 세팅한다.
	void	SetValue( DWORD dwQuestEventKind, DWORD dwParam1, LONG lParam2 );	// 퀘스트 이벤트 값을 세팅한다.
	BOOL	IsSame( CQuestEvent* pQuestEvent );								// 같은 이벤트인지 여부를 반환하는 함수.

	DWORD	GetQuestEventKind() { return m_dwQuestEventKind; }
	DWORD	GetDwParam1()		{ return m_dwParam1; }
	LONG	GetLParam2()		{ return m_lParam2; }
};

#endif // !defined(AFX_QUESTEVENT_H__0FAE1D70_917B_4C7D_A672_60DE84A4413B__INCLUDED_)

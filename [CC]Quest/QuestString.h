#pragma once


#include "..\[CC]Quest\QuestDefines.h"


//struct QString
//{
//	QString() :
//	Str( 0 ),
//	Line( 0 )
//	{}
//
//	~QString()
//	{
//		if(Str) delete [] Str;
//	}
//
//	char* Str;
//	int		Line;
//	DWORD	Color;
//
//	void operator = (char* pchar)
//	{
//		Str = new char[strlen(pchar)+1];
//		strcpy(Str,pchar);
//	}
//	operator LPCTSTR() const
//	{
//		return Str;
//	}
//	operator PCHAR() const
//	{
//		return Str;
//	}
//};

struct QString
{
	char Str[128] ;
	int		Line;
	DWORD	Color;
};



class CQuestString												// 퀘스트 스트링 클래스.
{	
	cPtrList		m_TitleList;								// 타이틀 리스트.
	cPtrList		m_DescList;									// 설명 리스트.

	DWORD			m_mIndex;									// 메인 인덱스.
	DWORD			m_sIndex;									// 서브 인덱스.
	DWORD			m_Key;										// 키값을 담는 변수.

	BOOL			m_bTreeTitle ;								// 트리 제목인지 여부를 확인하는 변수.

	char			m_TitleBuf[128] ;

	BOOL			m_bComplete ;

	int				m_nCount ;
	int				m_nTotalCount ;
	
public:
	
	CQuestString();												// 생성자 함수.
	virtual ~CQuestString();									// 소멸자 함수.

	void	StringLoader(char* str);							// 스트링을 로딩하는 함수.

	void	SetIndex(DWORD mIdx, DWORD sIdx)		{	m_mIndex = mIdx;	m_sIndex=sIdx;	COMBINEKEY(m_mIndex, m_sIndex, m_Key);	}	// 인덱스를 설정하는 함수.

	void	SetQuestID(DWORD dwIdx)	{ m_mIndex = dwIdx ; }		// 메인 퀘스트 인덱스를 세팅하는 함수.
	DWORD	GetQuestID()	{	return m_mIndex;	}			// 메인 퀘스트 인덱스를 반환하는 함수.

	void	SetSubID(DWORD dwIdx) { m_sIndex = dwIdx ; }		// 서브 퀘스트 인덱스를 세팅하는 함수.
	DWORD	GetSubID()		{	return m_sIndex;	}			// 서브 퀘스트 인덱스를 반환하는 함수.

	DWORD	GetKey()		{	return m_Key;		}			// 키 값을 반환하는 함수.

	BOOL IsHighlight(char* pStr, int& line, BOOL bTitle = FALSE) ;
	void	AddLine(char* pStr, int& line, BOOL bTitle=FALSE);	// 라인을 추가하는 함수.

	cPtrList*	GetTitle()	{	return &m_TitleList;	}		// 타이틀 리스트를 반환하는 함수.
	cPtrList*	GetDesc()	{	return &m_DescList;	}			// 설명 리스트를 반환하는 함수.
	//void ReleaseComponents() ;

	void SetTitleStr(char* pStr) { memset(m_TitleBuf, 0, 128) ; memcpy(m_TitleBuf, pStr, strlen(pStr)) ; }
	char* GetTitleStr() { return m_TitleBuf ; }

	void SetTreeTitle(BOOL val) { m_bTreeTitle = val ; }
	BOOL IsTreeTitle() { return m_bTreeTitle ; }

	void SetComplete(BOOL val) 
	{
		m_bComplete = val ; 
	}
	BOOL IsComplete() { return m_bComplete ; }

	void SetCount(int nCount) { m_nCount = nCount ; }
	int  GetCount() { return m_nCount ; }

	void SetTotalCount(int nTotalCount) { m_nTotalCount = nTotalCount ; }
	int  GetTotalCount() { return m_nTotalCount ; }
};

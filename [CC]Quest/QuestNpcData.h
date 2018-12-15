// QuestNpcData.h: interface for the CQuestNpcData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTNPCDATA_H__BFE07D68_8F07_4142_9C5B_1CBFD5DC1D69__INCLUDED_)
#define AFX_QUESTNPCDATA_H__BFE07D68_8F07_4142_9C5B_1CBFD5DC1D69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStrTokenizer;									// 스트링 토크나이져 클래스를 선언한다.

class CQuestNpcData										// 퀘스트 NPC 데이터 클래스.
{
protected:
	DWORD		m_dwNpcIdx;								// NPC 인덱스를 담을 변수.

public:
	CQuestNpcData( DWORD dwKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestNpcData();																		// 소멸자 함수.

	// getfn
	DWORD	GetNpcIdx()			{ return m_dwNpcIdx; }												// NPC 인덱스를 반환하는 함수.
};

#endif // !defined(AFX_QUESTNPCDATA_H__BFE07D68_8F07_4142_9C5B_1CBFD5DC1D69__INCLUDED_)

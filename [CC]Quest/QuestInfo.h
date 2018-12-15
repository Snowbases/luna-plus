// QuestInfo.h: interface for the CQuestInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTINFO_H__50E7B712_25F6_44BA_91DF_710117F02237__INCLUDED_)
#define AFX_QUESTINFO_H__50E7B712_25F6_44BA_91DF_710117F02237__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestDefines.h"																	// 퀘스트 정의 선언 헤더파일을 불러온다.

class CSubQuestInfo;																		// 서브 퀘스트 정보 클래스를 선언한다.

class CQuestInfo																			// 퀘스트 정보 클래스.
{
protected:
	DWORD						m_dwQuestIdx;												// 퀘스트 인덱스.
	DWORD						m_dwSubQuestCount;											// 서브 퀘스트 카운트.
	DWORD						m_dwEndParam;												// 종료 파라메터.
	BOOL						m_bRepeat;

	int							m_nMapNum ;													// 퀘스트를 수락한 맵 번호를 담을 변수.

	DWORD						m_dwNpcIdx ;												// 퀘스트를 수락한 npc 인덱스를 담을 변수.

	CYHHashTable<CSubQuestInfo>	m_SubQuestTable;											// 서브 퀘스트 정보를 담고 있는 퀘스트 테이블.

	// 100402 ShinJS --- 시간제한을 위한 정보추가
	typedef std::pair< struct tm, struct tm > QuestDateLimitData;
	std::vector< QuestDateLimitData >	m_DateLimitList;									// Or 조건으로 검사할 시간정보들

public:
	CQuestInfo( DWORD dwQuestIdx );															// 생성자 함수.
	virtual ~CQuestInfo();																	// 소멸자 함수.

	void	Release();																		// 해제 함수.

	// script
	void	AddSubQuestInfo( DWORD dwSubQuestIdx, CSubQuestInfo* pSubQuest );				// 서브 퀘스트 정보를 추가하는 함수.
	void	SetEndParam( DWORD Data )						{ m_dwEndParam = Data;	}		// 종료 파라메터를 세팅하는 함수.

	// getfn
	DWORD			GetQuestIdx()							{ return m_dwQuestIdx; }								// 퀘스트 인덱스를 반환하는 함수.
	DWORD			GetSubQuestCount()						{ return m_dwSubQuestCount; }							// 서브 퀘스트 개수를 반환하는 함수.
	CSubQuestInfo*	GetSubQuestInfo( DWORD dwSubQuestIdx )	{ return m_SubQuestTable.GetData( dwSubQuestIdx ); }	// 서브 퀘스트 정보를 반환하는 함수.
	DWORD			GetEndParam()							{ return m_dwEndParam;	}								// 종료 파라메터를 반환하는 함수.

	void SetMapNum(int nMapNum)		{ m_nMapNum = nMapNum ; }								// 퀘스트를 수락한 맵 번호를 세팅하는 함수.
	int	 GetMapNum()				{ return m_nMapNum ; }									// 퀘스트를 수락한 맵 번호를 반환하는 함수.

	void  SetNpcIdx(DWORD dwNpcIdx)	{ m_dwNpcIdx = dwNpcIdx ; }								// 퀘스트를 수락한 npc 인덱스를 세팅하는 함수.
	DWORD GetNpcIdx()				{ return m_dwNpcIdx ; }									// 퀘스트를 수락한 npc 인덱스를 반환하는 함수.

	void  SetRepeat()				{ m_bRepeat = TRUE; }
	BOOL  IsRepeat()				{ return m_bRepeat; }

	// 시간제한 정보 추가
	void AddDateLimit( const QuestDateLimitData& dateLimit )	{ m_DateLimitList.push_back( dateLimit ); }
	BOOL HasDateLimit() const									{ return !m_DateLimitList.empty(); }

	// 현재 유효한 시간인지 판단.
	BOOL IsValidDateTime() const;
	// 다음 시작가능 시각을 반환
	__time64_t GetNextStartDateTime( __time64_t curTime=0 ) const;
	// 다음 종료 시각을 반환
	__time64_t GetNextEndDateTime( __time64_t curTime=0 ) const;
};

#endif // !defined(AFX_QUESTINFO_H__50E7B712_25F6_44BA_91DF_710117F02237__INCLUDED_)

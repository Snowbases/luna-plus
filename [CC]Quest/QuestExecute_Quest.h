// QuestExecute_Quest.h: interface for the CQuestExecute_Quest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTEXECUTE_QUEST_H__57E0B691_1C0E_4CD1_9AFA_7466B934B556__INCLUDED_)
#define AFX_QUESTEXECUTE_QUEST_H__57E0B691_1C0E_4CD1_9AFA_7466B934B556__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestExecute.h"														// 퀘스트 실행 헤더 파일을 불러온다.

class CQuestExecute_Quest : public CQuestExecute								// 퀘스트 실행 클래스를 상속받은 퀘스트 실행 아이템 클래스.
{
protected:	
	DWORD		m_dwExeQuestIdx;												// 실행 퀘스트 인덱스.
	DWORD		m_dwExeSubQuestIdx;												// 실행 서브 퀘스트 인덱스.

	// 080104 LYW --- QuestExecute_Quest : 퀘스트의 맵 이동 기능을 위해, 맵 이동 관련 멤버를 추가한다.
	MAPTYPE		m_DestinationMapNum ;											// 도착지 맵 번호을 담을 변수.

	float		m_fDestination_XPos ;											// 도착지역의 X 좌표.
	float		m_fDestination_ZPos ;											// 도착지역의 Z 좌표.

public:
	CQuestExecute_Quest( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestExecute_Quest();																				// 소멸자 함수.

	virtual BOOL	Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );					// 실행 함수.

	virtual int		CheckCondition( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );

	DWORD			GetQuestExeIdx()		{ return m_dwExeQuestIdx;		}									// 실행 퀘스트 인덱스 반환 함수.
	DWORD			GetSubQuestExeIdx()		{ return m_dwExeSubQuestIdx;	}

	DWORD			GetMapNum()		{ return m_DestinationMapNum;	}
	float			GetXPos()		{ return m_fDestination_XPos;	}
	float			GetZPos()		{ return m_fDestination_ZPos;	}
};

#endif // !defined(AFX_QUESTEXECUTE_QUEST_H__57E0B691_1C0E_4CD1_9AFA_7466B934B556__INCLUDED_)

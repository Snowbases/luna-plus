// QuestExecute_Count.h: interface for the CQuestExecute_Count class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTEXECUTE_COUNT_H__DE6A351F_3B48_4B9F_9724_EBAABAAD99B2__INCLUDED_)
#define AFX_QUESTEXECUTE_COUNT_H__DE6A351F_3B48_4B9F_9724_EBAABAAD99B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestExecute.h"													// 퀘스트 실행 헤더 파일을 불러온다.

class CQuestExecute_Count : public CQuestExecute							// 퀘스트 실행 클래스를 상속받은 퀘스트 실행 카운트 클래스.
{
protected:
	DWORD		m_dwRealSubQuestIdx;										// 리얼 서브 퀘스트 인덱스.
	DWORD		m_dwMaxCount;												// 최대 카운트.
	DWORD		m_dwWeaponKind;												// 무기 종류
	DWORD		m_dwParam1;													// 파라메터 1
	
public:
	CQuestExecute_Count( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수
	virtual ~CQuestExecute_Count();																				// 소멸자 함수.

	virtual BOOL	Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );					// 실행 함수.

	virtual int		CheckCondition( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );

	DWORD			GetRealSubQuestIdx()	{ return m_dwRealSubQuestIdx;	}
	DWORD			GetMaxCount()			{ return m_dwMaxCount;			}
	DWORD			GetWeaponKind()			{ return m_dwWeaponKind;		}
	DWORD			GetParam1()				{ return m_dwParam1;			}
};

#endif // !defined(AFX_QUESTEXECUTE_COUNT_H__DE6A351F_3B48_4B9F_9724_EBAABAAD99B2__INCLUDED_)

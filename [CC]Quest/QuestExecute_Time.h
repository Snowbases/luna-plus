// QuestExecute_Time.h: interface for the QuestExecute_Time class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTEXECUTE_TIME_H__2D255BAF_3708_4FDD_A755_9A7E9509CFE9__INCLUDED_)
#define AFX_QUESTEXECUTE_TIME_H__2D255BAF_3708_4FDD_A755_9A7E9509CFE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestExecute.h"													// 퀘스트 실행 헤더 파일을 불러온다.

class CQuestExecute_Time : public CQuestExecute								// 퀘스트 실행 클래스를 상속받은 퀘스트 실행 시간 클래스.
{
	BYTE		m_bType;													// 타입.
	DWORD		m_dwDay;													// 날짜.
	DWORD		m_dwHour;													// 시간.
	DWORD		m_dwMin;													// 분.

public:
	CQuestExecute_Time( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestExecute_Time();																				// 소멸자 함수.

	virtual BOOL	Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );					// 실행 함수.

	virtual int		CheckCondition( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );
};

#endif // !defined(AFX_QUESTEXECUTE_TIME_H__2D255BAF_3708_4FDD_A755_9A7E9509CFE9__INCLUDED_)

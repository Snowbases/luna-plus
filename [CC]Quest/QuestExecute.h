// QuestExecute.h: interface for the CQuestExecute class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTEXECUTE_H__4FDEA86A_E9B2_40CC_A83D_F7864A04BDBA__INCLUDED_)
#define AFX_QUESTEXECUTE_H__4FDEA86A_E9B2_40CC_A83D_F7864A04BDBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestDefines.h"													// 퀘스트 정의 선언 헤더를 불러온다.

enum ENUM_QUEST_EXC_ERROR
{
	e_EXC_ERROR_NO_ERROR = -1,
	e_EXC_ERROR_NO_PLAYERINFO = 0,
	e_EXC_ERROR_NO_QUESTGROUP, 
	e_EXC_ERROR_NO_QUEST,
	e_EXC_ERROR_NO_INVENTORYINFO,
	e_EXC_ERROR_NO_EXTRASLOT,
	e_EXC_ERROR_NO_EXTRASLOTQUEST,
	e_EXC_ERROR_NO_QUESTITEM,
	e_EXC_ERROR_NOT_ENOUGH_QUESTITEM,
	e_EXC_ERROR_NO_GIVEITEM,
	e_EXC_ERROR_NOT_ENOUGH_GIVEITEM,
	e_EXC_ERROR_NOT_ENOUGH_GIVEMONEY,
	e_EXC_ERROR_NOT_SAME_WEAPONKIND,
	e_EXC_ERROR_NOT_SAME_WEAPONKIND_COUNT,
	e_EXC_ERROR_NO_RANDOMITEMLIST,
	e_EXC_ERROR_NO_INVENTORYINFO_RANDOMITEM,
	e_EXC_ERROR_CANT_RECEIVE_MONEY,
	e_EXC_ERROR_NOT_EXIST_REQUITAL,
} ;

class CStrTokenizer;														// 스트링 토크나이져 클래스를 선언한다.
class CQuestGroup;															// 퀘스트 그룹 클래스를 선언한다.
class CQuest;																// 퀘스트 클래스를 선언한다.

class CQuestExecute															// 퀘스트 실행 클래스.
{
protected:		
	DWORD	m_dwQuestIdx;													// 퀘스트 인덱스를 담을 변수.
	DWORD	m_dwSubQuestIdx;												// 스브 퀘스트 인덱스를 담을 변수.
	DWORD	m_dwExecuteKind;												// 퀘스트 실행 타입을 담을 변수.

public:
	CQuestExecute( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestExecute();																				// 소멸자 함수.

	virtual BOOL	Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest ) = 0;			// 실행 함수.

	virtual int		CheckCondition(PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest ) = 0;		// 실행을 위한 조건 체크 함수.
	
	DWORD			GetQuestExecuteKind()		{ return m_dwExecuteKind;	}								// 실행 타입을 반환하는 함수.
};

#endif // !defined(AFX_QUESTEXECUTE_H__4FDEA86A_E9B2_40CC_A83D_F7864A04BDBA__INCLUDED_)

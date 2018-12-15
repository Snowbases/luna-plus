// QuestExecute_RandomItem.h: interface for the CQuestExecute_RandomItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTEXECUTE_RANDOMITEM_H__E5FAF1F8_D9F2_4B21_9792_D7F488E99EF6__INCLUDED_)
#define AFX_QUESTEXECUTE_RANDOMITEM_H__E5FAF1F8_D9F2_4B21_9792_D7F488E99EF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestExecute.h"														// 퀘스트 실행 헤더 파일을 불러온다.

struct RANDOMITEM																// 렌덤 아이템 정보를 담는 구조체.
{
	WORD	wItemIdx;															// 아이템 인덱스를 담는 변수.
	WORD	wItemNum;															// 아이템 수를 담는 변수.
	WORD	wPercent;															// 퍼센트를 담는 변수.
};

class CQuestExecute_RandomItem : public CQuestExecute							// 퀘스트 실행 클래스를 상속받은 퀘스트 실행 랜덤 아이템 클래스.
{
protected:
	DWORD			m_dwMaxItemCount;											// 아이템 최대 카운트.
	DWORD			m_dwRandomItemCount;										// 랜덤 아이템 카운트.
	RANDOMITEM*		m_pRandomItem;												// 랜덤 아이템 포인터.
	
public:
	CQuestExecute_RandomItem( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestExecute_RandomItem();																			// 소멸자 함수.

	virtual BOOL	Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );						// 실행 함수.

	virtual int		CheckCondition( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );

	RANDOMITEM*		GetRandomItem();																				// 랜덤 아이템 정보를 반환하는 함수.
};

#endif // !defined(AFX_QUESTEXECUTE_RANDOMITEM_H__E5FAF1F8_D9F2_4B21_9792_D7F488E99EF6__INCLUDED_)

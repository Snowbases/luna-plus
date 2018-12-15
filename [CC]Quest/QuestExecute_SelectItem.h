// QuestExecute_SelectItem.h: interface for the CQuestExecute_SelectItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTEXECUTE_SELECTITEM_H__E5FAF1F8_D9F2_4B21_9792_D7F488E99EF6__INCLUDED_)
#define AFX_QUESTEXECUTE_SELECTITEM_H__E5FAF1F8_D9F2_4B21_9792_D7F488E99EF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestExecute.h"														// 퀘스트 실행 헤더 파일을 불러온다.

struct SELECTITEM																// 선택 아이템 정보를 담는 구조체.
{
	DWORD	dwItemIdx;															// 아이템 인덱스를 담는 변수.
	DWORD	dwItemNum;															// 아이템 수를 담는 변수.
};

class CQuestExecute_SelectItem : public CQuestExecute							// 퀘스트 실행 클래스를 상속받은 퀘스트 실행 랜덤 아이템 클래스.
{
protected:
	DWORD			m_dwMaxItemCount;												// 아이템 최대 카운트.
	SELECTITEM*		m_pSelectItem;
	
public:
	CQuestExecute_SelectItem( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	// 생성자 함수.
	virtual ~CQuestExecute_SelectItem();																			// 소멸자 함수.

	virtual BOOL	Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );						// 실행 함수.

	virtual int		CheckCondition( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );
	
	DWORD			GetMaxItemCount()			{ return m_dwMaxItemCount; }
	DWORD			GetItemKind( int nIndex )	{ return m_pSelectItem[nIndex].dwItemIdx; }
	DWORD			GetItemNum( int nIndex )	{ return m_pSelectItem[nIndex].dwItemNum; }

};

#endif // !defined(AFX_QUESTEXECUTE_RANDOMITEM_H__E5FAF1F8_D9F2_4B21_9792_D7F488E99EF6__INCLUDED_)

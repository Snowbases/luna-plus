// QuestExecute_Item.h: interface for the CQuestExecute_Item class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTEXECUTE_ITEM_H__7ED33FD7_DE37_44D7_80C3_E4EC290224C2__INCLUDED_)
#define AFX_QUESTEXECUTE_ITEM_H__7ED33FD7_DE37_44D7_80C3_E4EC290224C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestExecute.h"															// 퀘스트 실행 헤더 파일을 불러온다.

class CQuestExecute_Item : public CQuestExecute										// 퀘스트 실행 클래스를 상속받은 퀘스트 실행 아이템 클래스.
{
protected:
	DWORD		m_dwItemKind;														// 아이템 종류.
	DWORD		m_dwItemNum;														// 아이템 개수.
	DWORD		m_dwItemProbability;												// 아이템 속성?확률?
	DWORD		m_dwWeaponKind;														// 무기 종류.

public:
	CQuestExecute_Item( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	virtual ~CQuestExecute_Item();

	virtual BOOL	Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );

	virtual int		CheckCondition( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );

	DWORD		GetItemKind()			{ return m_dwItemKind ; }
	DWORD		GetItemNum()			{ return m_dwItemNum ; }
	DWORD		GetItemProbability()	{ return m_dwItemProbability ; }
	DWORD		GetWeaponKind()			{ return m_dwWeaponKind ; }
};

#endif // !defined(AFX_QUESTEXECUTE_ITEM_H__7ED33FD7_DE37_44D7_80C3_E4EC290224C2__INCLUDED_)

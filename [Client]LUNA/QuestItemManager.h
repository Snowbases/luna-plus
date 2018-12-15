// QuestItemManager.h: interface for the CQuestItemManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTITEMMANAGER_H__8C0E130D_7E31_450F_BDD1_EAC967EE5918__INCLUDED_)
#define AFX_QUESTITEMMANAGER_H__8C0E130D_7E31_450F_BDD1_EAC967EE5918__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "..\[CC]Quest\QuestDefines.h"								// 퀘스트 정의 해더 파일을 불러 온다.



class CQuestItemManager												// 퀘스트 아이템 매니져 클래스.
{
	CYHHashTable<QUEST_ITEM_INFO>		m_QuestItemList;			// 퀘스트 아이템 정보를 담은 해쉬 테이블 멤버.

public:
	CQuestItemManager();											// 생성자 함수.
	virtual ~CQuestItemManager();									// 소멸자 함수.

	void	Release();												// 해제 함수.
	void	QuestItemInfoLoad();									// 퀘스트 아이템 정보를 로딩하는 함수.
	QUEST_ITEM_INFO*	GetQuestItem(DWORD ItemIdx);				// 들어온 인덱스로 퀘스트 아이템 정보를 반환하는 함수.

};


#endif // !defined(AFX_QUESTITEMMANAGER_H__8C0E130D_7E31_450F_BDD1_EAC967EE5918__INCLUDED_)

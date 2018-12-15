// QuestUpdater.h: interface for the CQuestUpdater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTUPDATER_H__4AA13D4F_B271_43EE_B11B_163762DD8EE3__INCLUDED_)
#define AFX_QUESTUPDATER_H__4AA13D4F_B271_43EE_B11B_163762DD8EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\[CC]Quest\QuestDefines.h"										// 퀘스트 정의 선언 해더파일을 불러온다.


class CPlayer;																// 플레이어 클래스를 선언한다.
class CQuestString;															// 퀘스트 스트링 클래스를 선언한다.

class CQuestUpdater															// 퀘스트 업데이터 클래스.
{
public:
	CQuestUpdater();
	virtual ~CQuestUpdater();
	void	QuestItemAdd(QUEST_ITEM_INFO* pQuestItemInfo , DWORD Count);	// 퀘스트 아이템 추가 함수.
	void	QuestItemDelete(DWORD ItemIdx);									// 퀘스트 아이템 삭제 함수.
	void	QuestItemUpdate(DWORD type, DWORD ItemIdx, DWORD Count);		// 퀘스트 아이템 업데이트 함수.
	DWORD	GetQuestItemCount(DWORD ItemIdx);								// 퀘스트 아이템 수 반환 함수.
	DWORD	GetSelectedQuestID();
	void	DeleteQuest(DWORD QuestIdx);									// 퀘스트 삭제 함수.
};

#endif // !defined(AFX_QUESTUPDATER_H__4AA13D4F_B271_43EE_B11B_163762DD8EE3__INCLUDED_)

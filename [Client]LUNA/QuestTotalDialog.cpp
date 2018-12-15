// QuestTotalDialog.cpp: implementation of the CQuestTotalDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuestTotalDialog.h"

#include "QuestDialog.h"
#include "QuestManager.h"
#include "GameIn.h"

// 071022 LYW --- QuestTotalDialog : Include header file of tutorial manager.
#include "TutorialManager.h"

// 061220 LYW --- Delete this code.
//#include "MainBarDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestTotalDialog::CQuestTotalDialog()
{
	m_type = WT_QUESTTOTALDIALOG;
	m_pQuestDlg = NULL;
}

CQuestTotalDialog::~CQuestTotalDialog()
{

}
void CQuestTotalDialog::SetActive( BOOL val )
{
	cTabDialog::SetActive( val );

	TUTORIALMGR->Check_OpenDialog(this->GetID(), val);
}

void CQuestTotalDialog::Add(cWindow * window)
{	
	if(window->GetType() == WT_QUESTDIALOG)
		m_pQuestDlg = (CQuestDialog*)window;
		

	if(window->GetType() == WT_PUSHUPBUTTON)
	{
		AddTabBtn(curIdx1++, (cPushupButton * )window);
		return;
	}
	else if(window->GetType() == WT_QUESTDIALOG)
	{
		AddTabSheet(curIdx2++, window);
		return;
	}
	cDialog::Add(window);
}

void CQuestTotalDialog::QuestItemAdd(QUEST_ITEM_INFO* pQuestItemInfo, DWORD Count)
{
	m_pQuestDlg->QuestItemAdd(pQuestItemInfo, Count);
}

void CQuestTotalDialog::QuestItemDelete(DWORD ItemIdx)
{
	m_pQuestDlg->QuestItemDelete(ItemIdx);
}

DWORD CQuestTotalDialog::QuestItemUpdate(DWORD type, DWORD ItemIdx, DWORD data)
{
	return m_pQuestDlg->QuestItemUpdate(type, ItemIdx, data);
}

DWORD CQuestTotalDialog::GetSelectedQuestID()
{
	return m_pQuestDlg->GetSelectedQuestID();
}

void CQuestTotalDialog::GiveupQuestDelete(DWORD QuestIdx)
{
	m_pQuestDlg->GiveupQuestDelete(QuestIdx);
}
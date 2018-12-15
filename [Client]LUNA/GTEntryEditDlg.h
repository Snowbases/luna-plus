#pragma once
#include "stdAfx.h"
#include "./Interface/cDialog.h"

class cStatic;
class cListDialog;

struct GMEMBERINFO
{
	DWORD dwPlayerID;
	char szName[MAX_NAME_LENGTH+1];
	WORD wLevel;
};

enum
{
	GT_ENTRYEDIT_GMEMBERLIST = 0,
	GT_ENTRYEDIT_ENTRYLIST,
	GT_ENTRYEDIT_MAX,
};

class CGTEntryEditDlg :
	public cDialog
{
	BOOL							m_bChangedColor;
	WORD							m_wLastSec;
	WORD							m_wSelectedList;
	DWORD							m_dwRemainTime;

	std::list< GUILDMEMBERINFO >	m_lstMemberList;
	std::list< DWORD >				m_lstEntryList;

	cListDialog*					m_pGMemberList;
	cListDialog*					m_pEntryList;
	cStatic*						m_pRemainTime;
	cButton*						m_pEntryChange;

public:
	CGTEntryEditDlg(void);
	virtual ~CGTEntryEditDlg(void);

	void Linking();
	virtual void SetActive( BOOL val );
	DWORD ActionEvent(CMouse * mouseInfo);
	void OnActionEvent( LONG lId, void * p, DWORD we );
	virtual void Render();

	void CalcRemainTime();
	void SetRemainTime(DWORD dwTime) {m_dwRemainTime = dwTime; CalcRemainTime();}
	void ReduceTickTime(DWORD dwTickTime) {if(dwTickTime<m_dwRemainTime) m_dwRemainTime-=dwTickTime;}
	void SetList(DWORD* pEntryList);

	GUILDMEMBERINFO* GetMember( DWORD playerIndex );
	BOOL IsInEntry(DWORD dwPlayerID);
	BOOL AddToEntry(DWORD dwPlayerID);
};

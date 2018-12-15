#pragma once
#include "stdAfx.h"
#include "./Interface/cDialog.h"

class cStatic;
class cListDialog;

struct MEMBERSCOREINFO
{
	DWORD dwPlayerID;
	char szName[MAX_NAME_LENGTH+1];
	WORD wScore;
	WORD wTeam;

	MEMBERSCOREINFO()
	{
		dwPlayerID = 0;
		ZeroMemory(szName, sizeof(szName));
		wScore = 0;
		wTeam = 0;
	}
};

class CGTResultDlg :
	public cDialog
{
	FILE*							m_fp;
	cStatic*						m_pGuildName[2];
	cListDialog*					m_pMemberList[2];
	char							m_szGuildName[2][MAX_GUILD_NAME];

	CYHHashTable<MEMBERSCOREINFO>	m_KillCountList;

public:
	CGTResultDlg(void);
	virtual ~CGTResultDlg(void);

	void Linking();
	void FillName();
	void SetGuildName(char* pTeam1Name, char* pTeam2Name);
	void SetKillCnt(DWORD dwPlayerID, DWORD dwCnt);
	void SetResult();
	void ClearResult();
	void RefreshList();
};

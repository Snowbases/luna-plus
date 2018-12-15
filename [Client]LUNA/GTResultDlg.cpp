#include "gtresultdlg.h"
#include "ObjectManager.h"

#include "WindowIDEnum.h"
#include "interface/cListDialog.h"
#include "interface/cStatic.h"
#include "BattleSystem_Client.h"

CGTResultDlg::CGTResultDlg(void)
{
}

CGTResultDlg::~CGTResultDlg(void)
{
	ClearResult();
}

void CGTResultDlg::Linking()
{
	m_pGuildName[0] = (cStatic*)GetWindowForID( GT_RESULTGUILDNAME );
	m_pGuildName[1] = (cStatic*)GetWindowForID( GT_RESULTGUILDNAME2 );

	m_pMemberList[0] = (cListDialog*)GetWindowForID( GT_RESULTLIST );
	if(m_pMemberList[0])
	{
		m_pMemberList[0]->SetShowSelect( FALSE );
	}
	m_pMemberList[1] = (cListDialog*)GetWindowForID( GT_RESULTLIST2 );
	if(m_pMemberList[1])
	{
		m_pMemberList[1]->SetShowSelect( FALSE );
	}

	m_KillCountList.Initialize(32);
}

void CGTResultDlg::FillName()
{
	CBattle* pBattle = BATTLESYSTEM->GetBattle();
	if( !pBattle )		return;

	m_KillCountList.SetPositionHead();

	for(MEMBERSCOREINFO* pInfo = m_KillCountList.GetData();
		0 < pInfo;
		pInfo = m_KillCountList.GetData())
	{
		CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pInfo->dwPlayerID);
		if(pPlayer)
		{
			SafeStrCpy(pInfo->szName, pPlayer->GetObjectName(), sizeof(pInfo->szName));
			pInfo->wTeam = (WORD)pBattle->GetBattleTeamID(pPlayer);
		}
	}
}

void CGTResultDlg::SetGuildName(char* pTeam1Name, char* pTeam2Name)
{
	if(!pTeam1Name || !pTeam2Name)
		return;

	strcpy(m_szGuildName[0], pTeam1Name);
	strcpy(m_szGuildName[1], pTeam2Name);
}

void CGTResultDlg::SetKillCnt(DWORD dwPlayerID, DWORD dwCnt)
{
	if(0 == dwPlayerID)
		return;

	MEMBERSCOREINFO* pInfo = m_KillCountList.GetData(dwPlayerID);

	if(pInfo)
	{
		pInfo->wScore = WORD(dwCnt);
	}
	else
	{
		pInfo = new MEMBERSCOREINFO;
		if(pInfo)
		{
			pInfo->dwPlayerID = dwPlayerID;
			pInfo->wScore = WORD(dwCnt);
			m_KillCountList.Add(pInfo, dwPlayerID);
		}
	}

	if(m_bActive)
		RefreshList();
}

void CGTResultDlg::SetResult()
{
	RefreshList();


	SetActive(TRUE);
}

void CGTResultDlg::ClearResult()
{
	m_KillCountList.SetPositionHead();

	for(MEMBERSCOREINFO* pInfo = m_KillCountList.GetData();
		0 < pInfo;
		pInfo = m_KillCountList.GetData())
	{
		SAFE_DELETE(
			pInfo);
	}
	m_KillCountList.RemoveAll();
}

void CGTResultDlg::RefreshList()
{
	char buf[128] = {0,};

	if(!HERO)			return;

	CBattle* pBattle = BATTLESYSTEM->GetBattle();
	if(!pBattle)		return;

	m_pMemberList[0]->RemoveAll();
	m_pMemberList[1]->RemoveAll();

	if(HERO->GetBattleTeam() == eBattleTeam1)
	{
		m_pGuildName[0]->SetFGColor(0xff00ff00);
		m_pGuildName[1]->SetFGColor(0xffff0000);
	}
	else if(HERO->GetBattleTeam() == eBattleTeam2)
	{
		m_pGuildName[0]->SetFGColor(0xffff0000);
		m_pGuildName[1]->SetFGColor(0xff00ff00);
	}
	else
	{
		m_pGuildName[0]->SetFGColor(0xffffffff);
		m_pGuildName[1]->SetFGColor(0xffffffff);
	}

	m_pGuildName[0]->SetStaticText(m_szGuildName[0]);
	m_pGuildName[1]->SetStaticText(m_szGuildName[1]);

	FillName();

	m_KillCountList.SetPositionHead();

	int nTeam1Cnt = 1;
	int nTeam2Cnt = 1;

	m_KillCountList.SetPositionHead();

	for(MEMBERSCOREINFO* pInfo = m_KillCountList.GetData();
		0 < pInfo;
		pInfo = m_KillCountList.GetData())
	{
		if(0 == strcmp(pInfo->szName, ""))
			continue;

		if(pInfo->wTeam == eBattleTeam1)
		{
			sprintf(buf, "%2d    %-18s     %4d", nTeam1Cnt, pInfo->szName, pInfo->wScore);
			m_pMemberList[0]->AddItem( buf, RGB( 255, 255, 255 ));
			nTeam1Cnt++;
		}
		else if(pInfo->wTeam == eBattleTeam2)
		{
			sprintf(buf, "%2d    %-18s     %4d", nTeam2Cnt, pInfo->szName, pInfo->wScore);
			m_pMemberList[1]->AddItem( buf, RGB( 255, 255, 255 ));
			nTeam2Cnt++;
		}
	}
}
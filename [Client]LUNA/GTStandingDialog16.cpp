#include "stdafx.h"
#include ".\gtstandingdialog16.h"
#include "WindowIDEnum.h"
#include "./Interface/cStatic.h"
#include "./Interface/cPushupButton.h"

CGTStandingDialog16::CGTStandingDialog16(void)
{
	m_wCurTournament = 0;
	m_wCurRound = 0;
}

CGTStandingDialog16::~CGTStandingDialog16(void)
{
}

void CGTStandingDialog16::Linking()
{
	int i;
	for(i=0; i<eGTStanding_16TH; i++)
	{
		m_pBtn16[i] = (cPushupButton*)GetWindowForID(GT_BTN_16_1 + i);
		m_pBtn16[i]->SetDisable(TRUE);
	}
	for(i=0; i<eGTStanding_8TH; i++)
	{
		m_pBtn8[i] = (cPushupButton*)GetWindowForID(GT_BTN_8_1 + i);
		m_pBtn8[i]->SetDisable(TRUE);
	}
	for(i=0; i<eGTStanding_4TH; i++)
	{
		m_pBtn4[i] = (cPushupButton*)GetWindowForID(GT_BTN_4_1 + i);
		m_pBtn4[i]->SetDisable(TRUE);
	}
	for(i=0; i<eGTStanding_2ND; i++)
	{
		m_pBtn2[i] = (cPushupButton*)GetWindowForID(GT_BTN_2_1 + i);
		m_pBtn2[i]->SetDisable(TRUE);
	}
	m_pBtn1 = (cPushupButton*)GetWindowForID(GT_BTN_1_1);
	m_pBtn1->SetDisable(TRUE);
}

void CGTStandingDialog16::SetActive( BOOL val )
{
	cDialog::SetActive( val );

	if( val )
	{
	}
	else
	{
	}
}

void CGTStandingDialog16::Render()
{
	if( !m_bActive )
	{
		int a;
		a = 10;

		return;
	}

	cDialog::Render();
}

void CGTStandingDialog16::ResetAll()
{
	ZeroMemory(m_BattleTable, sizeof(m_BattleTable));
	
	int i;
	for(i=0; i<eGTStanding_16TH; i++)
	{
		m_pBtn16[i]->SetText("¹Ìµî·Ï", 0xff707070);
		m_pBtn16[i]->SetPush(FALSE);
	}
	for(i=0; i<eGTStanding_8TH; i++)
	{
		m_pBtn8[i]->SetText("", 0xff707070);
		m_pBtn8[i]->SetPush(FALSE);
	}
	for(i=0; i<eGTStanding_4TH; i++)
	{
		m_pBtn4[i]->SetText("", 0xff707070);
		m_pBtn4[i]->SetPush(FALSE);
	}
	for(i=0; i<eGTStanding_2ND; i++)
	{
		m_pBtn2[i]->SetText("", 0xff707070);
		m_pBtn2[i]->SetPush(FALSE);
	}

	m_pBtn1->SetText("", 0xff707070);
	m_pBtn1->SetPush(FALSE);
}

void CGTStandingDialog16::RefreshGuildInfo()
{
	int i;
	for(i=0; i<eGTStanding_16TH; i++)
	{
		if(0 == m_BattleTable[i].GuildIdx)
			continue;

		if(m_wCurRound==eGTFight_End && eGTFight_1<=m_BattleTable[i].ProcessTournament)
		{
			if(eGTFight_1 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn1->SetText(m_BattleTable[i].GuildName, 0xffffffff);
				m_pBtn1->SetPush(TRUE);
			}
			if(eGTFight_2 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn2[i/8]->SetText(m_BattleTable[i].GuildName, 0xffffffff);
				m_pBtn2[i/8]->SetPush(TRUE);
			}
			if(eGTFight_4 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn4[i/4]->SetText(m_BattleTable[i].GuildName, 0xffffffff);
				m_pBtn4[i/4]->SetPush(TRUE);
			}		
			if(eGTFight_8 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn8[i/2]->SetText(m_BattleTable[i].GuildName, 0xffffffff);
				m_pBtn8[i/2]->SetPush(TRUE);
			}
			if(eGTFight_16 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn16[i]->SetText(m_BattleTable[i].GuildName, 0xffffffff);
				m_pBtn16[i]->SetPush(TRUE);
			}
		}
		else if(m_wCurRound <= m_BattleTable[i].ProcessTournament)
		{
			if(eGTFight_1 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn1->SetText(m_BattleTable[i].GuildName, 0xffffffff);
				m_pBtn1->SetPush(TRUE);
			}
			if(eGTFight_2 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn2[i/8]->SetText(m_BattleTable[i].GuildName, 0xffffffff);
				m_pBtn2[i/8]->SetPush(TRUE);
			}
			if(eGTFight_4 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn4[i/4]->SetText(m_BattleTable[i].GuildName, 0xffffffff);
				m_pBtn4[i/4]->SetPush(TRUE);
			}		
			if(eGTFight_8 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn8[i/2]->SetText(m_BattleTable[i].GuildName, 0xffffffff);
				m_pBtn8[i/2]->SetPush(TRUE);
			}
			if(eGTFight_16 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn16[i]->SetText(m_BattleTable[i].GuildName, 0xffffffff);
				m_pBtn16[i]->SetPush(TRUE);
			}
		}
		else
		{
			if(eGTFight_2 == m_BattleTable[i].ProcessTournament)
			{
				m_pBtn2[i/8]->SetText(m_BattleTable[i].GuildName, 0xff707070);
				m_pBtn2[i/8]->SetPush(FALSE);
			}
			if(eGTFight_4 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn4[i/4]->SetText(m_BattleTable[i].GuildName, 0xff707070);
				m_pBtn4[i/4]->SetPush(FALSE);
			}
			if(eGTFight_8 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn8[i/2]->SetText(m_BattleTable[i].GuildName, 0xff707070);
				m_pBtn8[i/2]->SetPush(FALSE);
			}
			if(eGTFight_16 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn16[i]->SetText(m_BattleTable[i].GuildName, 0xff707070);
				m_pBtn16[i]->SetPush(FALSE);
			}
		}
	}
}

void CGTStandingDialog16::SetCurTournament(WORD wCurTournament)
{
	m_wCurTournament = wCurTournament;
}

void CGTStandingDialog16::SetCurRound(WORD wCurRound)
{
	m_wCurRound = wCurRound;
}

void CGTStandingDialog16::AddGuildInfo(REGISTEDGUILDINFO* pInfo)
{
	memcpy(&m_BattleTable[pInfo->Position], pInfo, sizeof(REGISTEDGUILDINFO));
}
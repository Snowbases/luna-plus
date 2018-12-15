// GTStandingDialog.cpp: implementation of the CGTStandingDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GTStandingDialog.h"
#include "WindowIDEnum.h"
#include "./Interface/cStatic.h"
#include "./Interface/cPushupButton.h"
#include "ChatManager.h"


CGTStandingDialog::CGTStandingDialog()
{
	m_wCurTournament = 0;
	m_wCurRound = 0;
	m_wLastSec = 0;
	m_dwRemainTime = 0;
}

CGTStandingDialog::~CGTStandingDialog()
{

}


void CGTStandingDialog::Linking()
{
	int i;
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

	m_pBtnState = (cPushupButton*)GetWindowForID(GT_BTN_PROCESS);
	m_pBtnState->SetDisable(TRUE);
}

void CGTStandingDialog::Render()
{
	if(!m_bActive)		return;

	CalcRemainTime();
	cDialog::Render();
}

void CGTStandingDialog::ResetAll()
{
	ZeroMemory(m_BattleTable, sizeof(m_BattleTable));
	
	int i;
	for(i=0; i<eGTStanding_8TH; i++)
	{
		m_pBtn8[i]->SetText(CHATMGR->GetChatMsg(1646), 0xff909090);
		m_pBtn8[i]->SetPush(FALSE);
		m_pBtn8[i]->SetRenderArea();
	}
	for(i=0; i<eGTStanding_4TH; i++)
	{
		m_pBtn4[i]->SetText("", 0xff909090);
		m_pBtn4[i]->SetPush(FALSE);
		m_pBtn4[i]->SetRenderArea();
	}
	for(i=0; i<eGTStanding_2ND; i++)
	{
		m_pBtn2[i]->SetText("", 0xff909090);
		m_pBtn2[i]->SetPush(FALSE);
		m_pBtn2[i]->SetRenderArea();
	}

	m_pBtn1->SetText("", 0xff909090);
	m_pBtn1->SetPush(FALSE);
	m_pBtn1->SetRenderArea();

	m_pBtnState->SetText("", 0xffffffff);
	m_pBtnState->SetRenderArea();
}

void CGTStandingDialog::RefreshGuildInfo()
{
	char buf[128] = {0};
	DWORD color = 0xffffffff;

	for(int i=0; i<eGTStanding_8TH; ++i)
	{
		if(0 == m_BattleTable[i].GuildIdx)
			continue;

		if(m_wCurState == eGTState_BeforeRegist)
		{
			if(eGTFight_1 <= m_BattleTable[i].ProcessTournament)
				color = 0xffffffff;
			else
				color = 0xff909090;
		}
		else
			color = 0xffffffff;

#ifdef _GMTOOL_
		sprintf(buf, "%s(%d)", m_BattleTable[i].GuildName, m_BattleTable[i].BattleID);
#else if
		strcpy(buf, m_BattleTable[i].GuildName);
#endif

		if(m_wCurRound==eGTFight_End && eGTFight_1<=m_BattleTable[i].ProcessTournament)
		{
			if(eGTFight_1 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn1->SetText(buf, color);
				m_pBtn1->SetPush(TRUE);
				m_pBtn1->SetRenderArea();
			}
			if(eGTFight_2 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn2[i/4]->SetText(buf, color);
				m_pBtn2[i/4]->SetPush(TRUE);
				m_pBtn2[i/4]->SetRenderArea();
			}
			if(eGTFight_4 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn4[i/2]->SetText(buf, color);
				m_pBtn4[i/2]->SetPush(TRUE);
				m_pBtn4[i/2]->SetRenderArea();
			}		
			if(eGTFight_8 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn8[i]->SetText(buf, color);
				m_pBtn8[i]->SetPush(TRUE);
				m_pBtn8[i]->SetRenderArea();
			}
		}
		else if(m_wCurRound <= m_BattleTable[i].ProcessTournament)
		{
			if(eGTFight_1 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn1->SetText(buf, color);
				m_pBtn1->SetPush(TRUE);
				m_pBtn1->SetRenderArea();
			}
			if(eGTFight_2 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn2[i/4]->SetText(buf, color);
				m_pBtn2[i/4]->SetPush(TRUE);
				m_pBtn2[i/4]->SetRenderArea();
			}
			if(eGTFight_4 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn4[i/2]->SetText(buf, color);
				m_pBtn4[i/2]->SetPush(TRUE);
				m_pBtn4[i/2]->SetRenderArea();
			}		
			if(eGTFight_8 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn8[i]->SetText(buf, color);
				m_pBtn8[i]->SetPush(TRUE);
				m_pBtn8[i]->SetRenderArea();
			}
		}
		else
		{
			if(eGTFight_2 == m_BattleTable[i].ProcessTournament)
			{
				m_pBtn2[i/4]->SetText(buf, 0xff707070);
				m_pBtn2[i/4]->SetPush(FALSE);
				m_pBtn2[i/4]->SetRenderArea();
			}
			if(eGTFight_4 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn4[i/2]->SetText(buf, 0xff707070);
				m_pBtn4[i/2]->SetPush(FALSE);
				m_pBtn4[i/2]->SetRenderArea();
			}
			if(eGTFight_8 <= m_BattleTable[i].ProcessTournament)
			{
				m_pBtn8[i]->SetText(buf, 0xff707070);
				m_pBtn8[i]->SetPush(FALSE);
				m_pBtn8[i]->SetRenderArea();
			}
		}
	}
}

void CGTStandingDialog::SetCurTournament(WORD wCurTournament)
{
	m_wCurTournament = wCurTournament;
}

void CGTStandingDialog::SetCurRound(WORD wCurRound)
{
	m_wCurRound = wCurRound;
}

void CGTStandingDialog::SetCurState(WORD wState)
{
	m_wCurState = wState;
}

void CGTStandingDialog::AddGuildInfo(REGISTEDGUILDINFO* pInfo)
{
	memcpy(&m_BattleTable[pInfo->Position], pInfo, sizeof(REGISTEDGUILDINFO));
}

void CGTStandingDialog::SetStateText(char* pState)
{
	if(m_pBtnState)
	{
		m_pBtnState->SetText(pState, 0xffffff77);
		m_pBtnState->SetRenderArea();
	}
}

void CGTStandingDialog::CalcRemainTime()
{
	if(m_dwRemainTime == 0)
		return;

	DWORD dwRemainTime = m_dwRemainTime;
	
	DWORD wDay = dwRemainTime / (60000 * 60 * 24);
	if(wDay)
		dwRemainTime = dwRemainTime - (60000 * 60 * 24 * wDay);

	DWORD wHour = dwRemainTime / (60000 * 60);
	if(wHour)
		dwRemainTime = dwRemainTime - (60000 * 60 * wHour);

	DWORD wMin = dwRemainTime / 60000;
	if(wMin)
		dwRemainTime = dwRemainTime - (60000 * wMin);

	DWORD wSec = dwRemainTime / 1000;

	if(m_wLastSec != wSec)
	{
		m_pBtnState->SetText("", 0xffffff77);

		char* pRound = NULL;
		char buf[256] = {0,};
		char remain[128] = {0,};
		if(wDay)
			sprintf(remain, "(%dDay %02d:%02d:%02d)", wDay, wHour, wMin, wSec);
		else
			sprintf(remain, "(%02d:%02d:%02d)", wHour, wMin, wSec);

		switch(m_wCurRound)
		{
		case eGTFight_32:	pRound = CHATMGR->GetChatMsg(1624);		break;
		case eGTFight_16:	pRound = CHATMGR->GetChatMsg(1625);		break;
		case eGTFight_8:	pRound = CHATMGR->GetChatMsg(1626);		break;
		case eGTFight_4:	pRound = CHATMGR->GetChatMsg(1627);		break;
		case eGTFight_2:	pRound = CHATMGR->GetChatMsg(1628);		break;
		}

		switch(m_wCurState)
		{
		case eGTState_BeforeRegist:
			{
				if(0 < m_wCurTournament)
				{
					sprintf(buf, CHATMGR->GetChatMsg(1647), m_wCurTournament);
					strcat(buf, remain);
					m_pBtnState->SetText(buf, 0xffffff77);
				}
			}
			break;
		case eGTState_BeforeEntrance:
			{
				sprintf(buf, CHATMGR->GetChatMsg(1648), pRound);
				strcat(buf, remain);
				m_pBtnState->SetText(buf, 0xffffff77);
			}
			break;
		case eGTState_Entrance:
			{
				sprintf(buf, CHATMGR->GetChatMsg(1649), pRound);
				strcat(buf, remain);
				m_pBtnState->SetText(buf, 0xffffff77);
			}
			break;
		case eGTState_Process:
			{
				sprintf(buf, CHATMGR->GetChatMsg(1650), pRound);
				strcat(buf, remain);
				m_pBtnState->SetText(buf, 0xffffff77);
			}
			break;
		}

		m_pBtnState->SetRenderArea();
		m_wLastSec = WORD(wSec);
	}
}
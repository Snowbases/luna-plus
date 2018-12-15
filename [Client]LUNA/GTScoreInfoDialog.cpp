// GTScoreInfoDailog.cpp: implementation of the CGTScoreInfoDailog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GTScoreInfoDialog.h"
#include "WindowIDEnum.h"
#include "./interface/cStatic.h"
#include "./interface/cButton.h"
#include "..\effect\DamageNumber.h"
#include "./Interface/cPushupButton.h"
#include "cScriptManager.h"
#include "ChatManager.h"
#include "GuildTournamentMgr.h"
#include "MHMap.h"

CGTScoreInfoDialog::CGTScoreInfoDialog()
{
	m_bStart = FALSE;
}

CGTScoreInfoDialog::~CGTScoreInfoDialog()
{

}


void CGTScoreInfoDialog::Linking()
{
	m_pGuildScore[0] = (cStatic*)GetWindowForID( GT_SCORE1 );
	m_pGuildScore[1] = (cStatic*)GetWindowForID( GT_SCORE2 );
	m_pGoalScoreImg = (cStatic*)GetWindowForID( GT_GOALSCOREIMG );
	m_pTimerImg = (cStatic*)GetWindowForID( GT_TIMERIMG );
	//m_pOutBtn = (cButton*)GetWindowForID( GDT_OUTBTN );

	m_pGuildName[0] = (cPushupButton*)GetWindowForID( GT_GUILDNAME1 );
	m_pGuildName[1] = (cPushupButton*)GetWindowForID( GT_GUILDNAME2 );
	m_pScoreBack[0] = (cPushupButton*)GetWindowForID( GT_SCOREBACK1 );
	m_pScoreBack[1] = (cPushupButton*)GetWindowForID( GT_SCOREBACK2 );
	m_pGoalScore = (cPushupButton*)GetWindowForID( GT_GOALSCORE );
	m_pTimer = (cPushupButton*)GetWindowForID( GT_TIMER );

	m_Score[0] = 0;
	m_Score[1] = 0;
	m_FightTime = 120000;

	for( int i=0; i<2; ++i)
	{
		m_ScoreImage[i].Init( 20, 0 );
		m_ScoreImage[i].SetFillZero( TRUE );
		m_ScoreImage[i].SetLimitCipher( 3 );
		m_ScoreImage[i].SetPosition( (int)m_pGuildScore[i]->GetAbsX()+65, (int)m_pGuildScore[i]->GetAbsY()+2 );
		m_ScoreImage[i].SetScale( 1.0f, 1.0f );

		m_TimerImg[i].Init( 12, 0 );
		m_TimerImg[i].SetFillZero( TRUE );
		m_TimerImg[i].SetLimitCipher( 2 );
		m_TimerImg[i].SetPosition( (int)m_pTimerImg->GetAbsX()+(i*30), (int)m_pTimerImg->GetAbsY()+2 );
		m_TimerImg[i].SetScale( 0.5f, 0.5f );

		for(int j=0; j<10; j++)
		{
			m_ScoreImage[i].InitDamageNumImage(CDamageNumber::GetImage(eDNK_Yellow, j), j);
			m_TimerImg[i].InitDamageNumImage(CDamageNumber::GetImage(eDNK_Yellow, j), j);
		}
	}
	m_GoalScoreImage.Init( 12, 0 );
	m_GoalScoreImage.SetFillZero( FALSE );
	m_GoalScoreImage.SetLimitCipher( 3 );
	m_GoalScoreImage.SetPosition( (int)m_pGoalScoreImg->GetAbsX(), (int)m_pGoalScoreImg->GetAbsY()+2 );
	m_GoalScoreImage.SetScale( 0.5f, 0.5f );
	for(int j=0; j<10; j++)
	{
		m_GoalScoreImage.InitDamageNumImage(CDamageNumber::GetImage(eDNK_Yellow, j), j);
	}
}

void CGTScoreInfoDialog::Render()
{
	if( m_bActive )
	{
		cDialog::Render();
		Process();

		static WORD wLastSec = 0;
		DWORD wMin = (m_FightTime/60000)%60;
		DWORD wSec = (m_FightTime/1000)%60;

		// 090817 ONS 종료시 남은 시간이 00:00으로 표시되도록 수정
		if(wLastSec <= wSec)
		{
			m_TimerImg[0].SetNumber( wMin );
			m_TimerImg[1].SetNumber( wSec );
		}

		for(int i=0; i<2; ++i)
		{
			m_ScoreImage[i].SetPosition((int)m_pGuildScore[i]->GetAbsX()+65, (int)m_pGuildScore[i]->GetAbsY()+2);
			m_ScoreImage[i].SetNumber( m_Score[i] );
			m_ScoreImage[i].RenderWithDamageNumImage();

			m_TimerImg[i].SetPosition((int)m_pTimerImg->GetAbsX()+(i*30), (int)m_pTimerImg->GetAbsY()+2);
			m_TimerImg[i].RenderWithDamageNumImage();
		}

		m_GoalScoreImage.SetPosition((int)m_pGoalScoreImg->GetAbsX(), (int)m_pGoalScoreImg->GetAbsY());
		m_GoalScoreImage.RenderWithDamageNumImage();
	}
}


void CGTScoreInfoDialog::Process()
{
	if(GTMAPNUM == MAP->GetMapNum())
	{
		return;
	}
	else if(FALSE == m_bStart)
	{
		return;
	}

	if( gTickTime < m_FightTime )			m_FightTime -= gTickTime;
	else									m_FightTime = 0;
}


void CGTScoreInfoDialog::SetBattleInfo( SEND_BATTLESCORE_INFO* pInfo )
{
	for( int i=0; i<2; ++i)
	{
		WCHAR pWName[64] = {0,};
		char pName[64] = {0,};

		if(12 < strlen(pInfo->GuildName[i]))
		{
			// 길드이름 컷팅은 유니코드로 변환후 작업한다.
			MultiByteToWideChar(CP_ACP, 0, pInfo->GuildName[i], 10, pWName, 5);
			WideCharToMultiByte(CP_ACP, 0, pWName, 5, pName, 10, 0, 0);

			strcat(pName, CHATMGR->GetChatMsg(1657));
			m_pGuildName[i]->SetText(pName, 0xffffffff);
		}
		else
		{
			m_pGuildName[i]->SetText(pInfo->GuildName[i], 0xffffffff);
		}

		m_Score[i] = pInfo->Score[i];
	}

	m_FightTime = pInfo->FightTime;

	if(pInfo->State == eGTState_Process)
	{
		m_bStart = TRUE;
	}

	m_GoalScoreImage.SetNumber(pInfo->GoalScore);
}

//void CGTScoreInfoDialog::ShowOutBtn( BOOL bShow )
//{
//	m_pOutBtn->SetActive( bShow );		
//}
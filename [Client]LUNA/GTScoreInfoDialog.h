// GTScoreInfoDailog.h: interface for the CGTScoreInfoDailog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GTSCOREINFODAILOG_H__49E37D19_391B_480A_B515_AE1DEA41C09F__INCLUDED_)
#define AFX_GTSCOREINFODAILOG_H__49E37D19_391B_480A_B515_AE1DEA41C09F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cDialog.h"
#include "../ImageNumber.h"


class cStatic;
class cButton;
class cPushupButton;



class CGTScoreInfoDialog : public cDialog
{
	CImageNumber	m_ScoreImage[2];
	CImageNumber	m_GoalScoreImage;
	CImageNumber	m_TimerImg[2];

	cStatic*		m_pGuildScore[2];
	cStatic*		m_pGoalScoreImg;
	cStatic*		m_pTimerImg;
	//cButton*		m_pOutBtn;
	cPushupButton*	m_pGuildName[2];
	cPushupButton*	m_pScoreBack[2];
	cPushupButton*	m_pGoalScore;
	cPushupButton*	m_pTimer;
	int				m_Score[2];
	DWORD			m_FightTime;
	BOOL			m_bStart;

public:
	CGTScoreInfoDialog();
	virtual ~CGTScoreInfoDialog();

	void Linking();
	void Render();
	void Process();
	
	void SetBattleInfo( SEND_BATTLESCORE_INFO* pInfo );
	void StartBattle()			{	m_bStart = TRUE;	}
	void EndBattle()			{	m_bStart = FALSE;	m_FightTime=0;	}
	BOOL IsBattle()				{   return m_bStart;}
	//
	void SetTeamScore( DWORD Team, DWORD Count )		{m_Score[Team] = Count;}
	//void ShowOutBtn( BOOL bShow );
};

#endif // !defined(AFX_GTSCOREINFODAILOG_H__49E37D19_391B_480A_B515_AE1DEA41C09F__INCLUDED_)

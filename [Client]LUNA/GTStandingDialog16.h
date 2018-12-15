#pragma once
#include "stdAfx.h"
#include "./Interface/cDialog.h"

class cPushupButton;
class cStatic;
class CGuildMark;

class CGTStandingDialog16 :
	public cDialog
{
	WORD				m_wCurTournament;
	WORD				m_wCurRound;
	cPushupButton*		m_pBtn16[eGTStanding_16TH];
	cPushupButton*		m_pBtn8[eGTStanding_8TH];
	cPushupButton*		m_pBtn4[eGTStanding_4TH];
	cPushupButton*		m_pBtn2[eGTStanding_2ND];
	cPushupButton*		m_pBtn1;
	REGISTEDGUILDINFO	m_BattleTable[eGTStanding_16TH];

public:
	CGTStandingDialog16(void);
	virtual ~CGTStandingDialog16(void);

	void Linking();
	void SetActive( BOOL val );
	void Render();

	void ResetAll();
	void RefreshGuildInfo();
	void SetCurTournament(WORD wCurTournament);
	void SetCurRound(WORD wCurRound);
	void AddGuildInfo(REGISTEDGUILDINFO* pInfo);
};

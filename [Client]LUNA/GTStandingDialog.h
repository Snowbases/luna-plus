// GTStandingDialog.h: interface for the CGTStandingDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GTSTANDINGDIALOG_H__FE986491_5B8E_40B8_BFE1_A7799EC00F9A__INCLUDED_)
#define AFX_GTSTANDINGDIALOG_H__FE986491_5B8E_40B8_BFE1_A7799EC00F9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "./Interface/cDialog.h"

class cPushupButton;
class cStatic;
class CGuildMark;


class CGTStandingDialog : public cDialog
{
	WORD				m_wCurTournament;
	WORD				m_wCurRound;
	WORD				m_wCurState;

	WORD				m_wLastSec;
	DWORD				m_dwRemainTime;

	cPushupButton*		m_pBtn8[eGTStanding_8TH];
	cPushupButton*		m_pBtn4[eGTStanding_4TH];
	cPushupButton*		m_pBtn2[eGTStanding_2ND];
	cPushupButton*		m_pBtn1;
	cPushupButton*		m_pBtnState;
	REGISTEDGUILDINFO	m_BattleTable[eGTStanding_8TH];

public:
	CGTStandingDialog();
	virtual ~CGTStandingDialog();


	void Linking();
	virtual void Render();

	void ResetAll();
	void RefreshGuildInfo();
	void SetCurTournament(WORD wCurTournament);
	void SetCurRound(WORD wCurRound);
	void SetCurState(WORD wState);
	void AddGuildInfo(REGISTEDGUILDINFO* pInfo);

	void SetStateText(char* pState);

	void CalcRemainTime();
	void SetRemainTime(DWORD dwTime) {m_dwRemainTime = dwTime; CalcRemainTime();}
	void ReduceTickTime(DWORD dwTickTime) {if(dwTickTime<m_dwRemainTime) m_dwRemainTime-=dwTickTime;}
};

#endif // !defined(AFX_GTSTANDINGDIALOG_H__FE986491_5B8E_40B8_BFE1_A7799EC00F9A__INCLUDED_)

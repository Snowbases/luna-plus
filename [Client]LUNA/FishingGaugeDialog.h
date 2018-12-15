#pragma once
#include "./Interface/cDialog.h"

#define FISHING_GAUGE_HOLDTIME	200

class cGuageBar;
class CObjectGuagen;

class CFishingGaugeDialog :
	public cDialog
{
protected:
	cGuageBar*		m_pFishingGB;		// 낚시 게이지바.
	CObjectGuagen*	m_pFishingTimeGauge;// 낚시 시간게이지
	DWORD			m_dwHoldTime;		// 게이지 홀딩시간

public:
	CFishingGaugeDialog(void);
	virtual ~CFishingGaugeDialog(void);

	void Linking();
	virtual void SetActive(BOOL val);
	virtual void Render();

	cGuageBar*		GetFishingGB()			{return m_pFishingGB;}
	CObjectGuagen*	GetFishingTimeGauge()	{return m_pFishingTimeGauge;}

	void SetHoldTime();
	DWORD GetHoldTime()						{return m_dwHoldTime;}
};

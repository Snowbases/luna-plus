#include "stdafx.h"
#include "fishinggaugedialog.h"
#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"
#include ".\interface\cGuageBar.h"
#include "ObjectGuagen.h"
#include "FishingManager.h"

CFishingGaugeDialog::CFishingGaugeDialog(void)
{
	m_dwHoldTime = 0;
}

CFishingGaugeDialog::~CFishingGaugeDialog(void)
{
}

void CFishingGaugeDialog::Linking()
{
	m_pFishingGB = (cGuageBar*)GetWindowForID(FISHINGBAR);
	m_pFishingGB->SetDisable(TRUE);
	m_pFishingTimeGauge = (CObjectGuagen*)GetWindowForID(FISHINGTIMEGAUGE);
	m_pFishingTimeGauge->SetDisable(TRUE);
}

void CFishingGaugeDialog::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(val == TRUE)
	{
		m_dwHoldTime = 0;
	}
	else
	{
		m_pFishingTimeGauge->SetValue(0, 0);
		m_pFishingGB->SetCurValue(0);
		m_dwHoldTime = 0;
	}
	cDialog::SetActive(val);
}

void CFishingGaugeDialog::Render()
{
	if(!m_bActive) return;	

	// 게이지 바 이동
	if(FISHINGMGR->IsPulling())
	{
		LONG CurValue = m_pFishingGB->GetCurValue();
		float fSpeed = FISHINGMGR->GetGaugeBarSpeed();
		float fDiffTime = 0.0f;
		int nDir = FISHINGMGR->GetGaugeBarDir();
		int nInteger = 0;
		
		static float fFraction = 0.0f;
		static DWORD dwElapsedTime = gCurTime;

		if(0==m_dwHoldTime)
		{
			fDiffTime = (float)gCurTime-dwElapsedTime;
			fFraction += fSpeed * fDiffTime;
			
			if(1.0f < fFraction)
			{
				nInteger = (int)fFraction;
				fFraction = fFraction - nInteger;
			}

			CurValue += (nInteger * nDir);
		}
		else
		{
			if(m_dwHoldTime+FISHING_GAUGE_HOLDTIME <= gCurTime)
				m_dwHoldTime = 0;
		}

		if(0 < nDir)
		{
			if(m_pFishingGB->GetMaxValue() <= CurValue)
			{
				CurValue = m_pFishingGB->GetMaxValue();
				FISHINGMGR->SetReverseDir();
			}
		}
		else
		{
			if(CurValue <= m_pFishingGB->GetMinValue())
			{
				CurValue = m_pFishingGB->GetMinValue();
				FISHINGMGR->SetReverseDir();
			}
		}

		m_pFishingGB->SetCurValue(CurValue);

		dwElapsedTime = gCurTime;
	}

	cDialog::Render();
}

void CFishingGaugeDialog::SetHoldTime()
{
	if(FISHINGMGR->IsPulling() && 0==m_dwHoldTime)
	{
		m_dwHoldTime = gCurTime;
	}
}
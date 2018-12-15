#include "stdafx.h"
#include "TriggerTimerDlg.h"
#include "GameResourceManager.h"
#include "WindowIDEnum.h"
#include "cStatic.h"
#include "ImageNumber.h"
#include "..\effect\DamageNumber.h"
#include "ChatManager.h"

CTriggerTimerDlg::CTriggerTimerDlg(void) :
m_csTimeMin( new CImageNumber ),
m_csTimeSec( new CImageNumber )
{
	m_Type = eTimerType_CountDown;
	m_State = eTimerState_Ready;
	m_dwStartTime = 0;
	m_dwDurationTime = 0;

	m_csTimeMin->Init( 24, 0 );
	m_csTimeSec->Init( 24, 0 );

	for(int i=0; i<10; i++)
	{
		m_csTimeMin->InitDamageNumImage(CDamageNumber::GetImage(eDNK_Red, i), i);
		m_csTimeSec->InitDamageNumImage(CDamageNumber::GetImage(eDNK_Red, i), i);
	}

	m_csTimeMin->SetFillZero( TRUE );
	m_csTimeMin->SetScale( 1.0f, 1.0f );
	m_csTimeMin->SetLimitCipher( 2 );
	m_csTimeSec->SetFillZero( TRUE );
	m_csTimeSec->SetScale( 1.0f, 1.0f );
	m_csTimeSec->SetLimitCipher( 2 );
}

CTriggerTimerDlg::~CTriggerTimerDlg(void)
{
	SAFE_DELETE( m_csTimeMin );
	SAFE_DELETE( m_csTimeSec );
}

void CTriggerTimerDlg::Linking()
{
	m_csTime	= (cStatic*)GetWindowForID(TRIGGER_TIMER_TIME);

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	VECTOR2 stPos = {float(dispInfo.dwWidth) - 80, 8};
	m_csTimeMin->SetPosition((int)stPos.x, (int)stPos.y);
	m_csTimeSec->SetPosition((int)stPos.x + 60, (int)stPos.y);
	m_csTimeMin->SetNumber(0);
	m_csTimeSec->SetNumber(0);
}

void CTriggerTimerDlg::Render()
{
 	if( !IsActive() ) return;
	
	static DWORD dwLastTimeTick = gCurTime;

	DWORD dwDiffTick = gCurTime - dwLastTimeTick;
	dwLastTimeTick = gCurTime;

	if(m_State == eTimerState_Pause)
	{
		if(m_Type == eTimerType_CountUp)
		{
			m_dwStartTime += dwDiffTick;
		}
		else if(m_Type == eTimerType_CountDown)
			m_dwDurationTime += dwDiffTick;
	}

	DWORD dwElapsedTime = gCurTime - m_dwStartTime;

	DWORD dwMin = 0;
	DWORD dwSec = 0;

	if(gCurTime > m_dwStartTime+m_dwDurationTime)
	{
		if(m_Type == eTimerType_CountUp)
		{
			dwMin = m_dwDurationTime / 60000;
			dwSec = (m_dwDurationTime - (dwMin*60000)) / 1000;
		}
		else if(m_Type == eTimerType_CountDown)
		{
			dwMin = 0;
			dwSec = 0;
		}
	}
	else
	{
		if(m_Type == eTimerType_CountUp)
		{
			dwMin = dwElapsedTime / 60000;
			dwSec = (dwElapsedTime - (dwMin*60000)) / 1000;
		}
		else if(m_Type == eTimerType_CountDown)
		{
			dwMin = (m_dwDurationTime - dwElapsedTime) / 60000;
			dwSec = ((m_dwDurationTime - dwElapsedTime) % 60000) / 1000;
		}
	}

	m_csTimeMin->SetNumber(dwMin);
	m_csTimeSec->SetNumber(dwSec);

	m_csTimeMin->RenderWithDamageNumImage();
	m_csTimeSec->RenderWithDamageNumImage();

 	cDialog::Render();
}

void CTriggerTimerDlg::Start(eTimerType type, DWORD dwDuration, DWORD dwElapsedTime)
{
	m_Type = type;
	m_State = eTimerState_Run;
	m_dwStartTime = gCurTime - dwElapsedTime;
	m_dwDurationTime = dwDuration;

	SetActive(TRUE);
}

void CTriggerTimerDlg::SetState(eTimerState state)
{
	if(state==eTimerState_Pause && m_State==eTimerState_Run)
	{
		m_State = eTimerState_Pause;
	}
	else if(state==eTimerState_Resume && m_State==eTimerState_Pause)
	{
		m_State = eTimerState_Run;
	}
	else if(state==eTimerState_Stop && (m_State==eTimerState_Run || m_State==eTimerState_Pause))
	{
		m_State = eTimerState_Stop;
		SetActive(FALSE);
	}
}
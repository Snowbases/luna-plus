#include "StdAfx.h"
#include "Timer.h"
#include "Message.h"
#include "Manager.h"

namespace Trigger
{
	CTimer::CTimer(void)
	{
	}

	CTimer::~CTimer(void)
	{
	}

	void CTimer::Init(DWORD alias, DWORD channel, eTimerType type, DWORD duration)
	{
		m_TimeOver = FALSE;
		m_Alias = alias;
		m_Channel = channel;

		m_Type = type;
		m_State = eTimerState_Run;
		m_dwStartTime = gCurTime;
		m_dwDurationTime = duration;

		m_dwElapsedTime = 0;
		m_dwElapsedSecond = 0;
		m_dwLastCheckTime = gCurTime;
	}

	void CTimer::Process()
	{
		if(m_TimeOver || m_State==eTimerState_Stop)
			return;

		DWORD dwDiffTick = gCurTime - m_dwLastCheckTime;
		m_dwLastCheckTime = gCurTime;

		if(m_State == eTimerState_Pause)
		{
			if(m_Type == eTimerType_CountDown)
				m_dwDurationTime += dwDiffTick;
			else if(m_Type == eTimerType_CountUp)
				m_dwStartTime += dwDiffTick;
		}

		m_dwElapsedTime = gCurTime - m_dwStartTime;

		if(m_Type == eTimerType_CountDown)
		{
			if(gCurTime > m_dwStartTime+m_dwDurationTime)
				m_TimeOver = TRUE;
		}
		else if(m_Type == eTimerType_CountUp)
		{
			if(gCurTime - m_dwStartTime > m_dwDurationTime)
				m_TimeOver = TRUE;
		}

		if(m_dwElapsedSecond < m_dwElapsedTime/1000)
		{
			m_dwElapsedSecond = m_dwElapsedTime/1000;

			// 091116 LUJ, 채널에 해당하는 메시지를 할당받도록 한다
			CMessage* const message = TRIGGERMGR->AllocateMessage(m_Channel);
			message->AddValue(eProperty_TimerName, m_Alias);
			message->AddValue(eProperty_TimerElapsedSecond, m_dwElapsedSecond);
		}

		if(m_TimeOver)
		{
			// 091116 LUJ, 채널에 해당하는 메시지를 할당받도록 한다
			CMessage* const message = TRIGGERMGR->AllocateMessage(m_Channel);
			message->AddValue(eProperty_TimerName, m_Alias);
			message->AddValue(eProperty_Event, eEvent_TimeOver);
		}
	}

	BOOL CTimer::SetState(eTimerState state)
	{
		if(state==eTimerState_Pause && m_State==eTimerState_Run)
		{
			m_State = eTimerState_Pause;
			return TRUE;
		}
		else if(state==eTimerState_Resume && m_State==eTimerState_Pause)
		{
			m_State = eTimerState_Run;
			return TRUE;
		}
		else if(state==eTimerState_Stop && (m_State==eTimerState_Run || m_State==eTimerState_Pause))
		{
			m_State = eTimerState_Stop;
			return TRUE;
		}

		return FALSE;
	}
}

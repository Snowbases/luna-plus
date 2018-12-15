#pragma once
#include "Pool.h"
#include "common.h"
#include "../[cc]Header/CommonGameDefine.h"

namespace Trigger
{
	class CTimer :
		public CPoolObject
	{
		friend class CManager;

	private:
		BOOL	m_TimeOver;
		DWORD	m_Alias;
		DWORD	m_Channel;

		eTimerType		m_Type;
		eTimerState		m_State;
		DWORD			m_dwStartTime;
		DWORD			m_dwDurationTime;

		DWORD			m_dwElapsedTime;
		DWORD			m_dwElapsedSecond;
		DWORD			m_dwLastCheckTime;

		// CManager::AllocateMessage()를 통해서만 메시지 메모리를 할당받을 수 있도록 private으로 지정한다
		CTimer(void);
		virtual ~CTimer(void);

	public:
		void Init(DWORD alias, DWORD channel, eTimerType type, DWORD duration);
		void Process();
		BOOL SetState(eTimerState state);
		DWORD		GetAlias()		{return m_Alias;}
		DWORD		GetChannel()	{return m_Channel;}
		BOOL		GetWaitDie()	{return m_State == eTimerState_Stop;}
		eTimerType	GetType()		{return m_Type;}
		eTimerState	GetState()		{return m_State;}
		DWORD		GetDuration()	{return m_dwDurationTime;}
		DWORD		GetElapsedTime(){return m_dwElapsedTime;}
		
	};
}

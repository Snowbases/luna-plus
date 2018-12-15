#include "StdAfx.h"
#include "ActionTimer.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"
#include "Manager.h"

namespace Trigger
{
	void CActionTimerStart::DoAction()
	{
		DWORD alias = GetValue(eProperty_TimerName);
		eTimerType type = (eTimerType)GetValue(eProperty_TimerType);
		DWORD duration = GetValue(eProperty_TimerDuration);

		TRIGGERMGR->StartTimer(alias, GetChannelID(), type, duration);

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			MSG_DWORD4	msg;
			msg.Category = MP_TRIGGER;
			msg.Protocol = MP_TRIGGER_TIMER_START;
			msg.dwObjectID = object->GetID();
			msg.dwData1 = type;
			msg.dwData2 = eTimerState_Ready;
			msg.dwData3 = duration;
			msg.dwData4 = 0;
			object->SendMsg(&msg, sizeof(msg));
		}
	}

	void CActionTimerState::DoAction()
	{
		DWORD alias = GetValue(eProperty_TimerName);
		eTimerState state = (eTimerState)GetValue(eProperty_TimerState);

		TRIGGERMGR->SetTimerState(alias, state);

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			MSG_DWORD	msg;
			msg.Category = MP_TRIGGER;
			msg.Protocol = MP_TRIGGER_TIMER_STATE;
			msg.dwObjectID = object->GetID();
			msg.dwData = state;
			object->SendMsg(&msg, sizeof(msg));
		}
	}

}
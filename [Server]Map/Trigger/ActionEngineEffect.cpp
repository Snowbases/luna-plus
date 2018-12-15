#include "StdAfx.h"
#include "ActionEngineEffect.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"

namespace Trigger
{
	void CActionEngineEffect::DoAction()
	{
		const DWORD hashCode = GetValue(eProperty_Name);
		const DWORD positionX = GetValue(eProperty_PosX);
		const DWORD positionZ = GetValue(eProperty_PosZ);		
		const DWORD motionIndex = GetValue(eProperty_Value);
		const DWORD angle = GetValue(eProperty_Angle);
		const DWORD isLoop = GetValue(eProperty_Loop);

		MSG_NAME_DWORD5 message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_ENGINE_EFFECT_ACK;
		message.dwData1 = positionX;
		message.dwData2 = positionZ;
		message.dwData3 = motionIndex;
		message.dwData4 = angle;
		message.dwData5 = isLoop;
		SafeStrCpy(
			message.Name,
			GetAliasName(hashCode),
			sizeof(message.Name) / sizeof(*message.Name));
		
		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}
			else if(eObjectKind_Player != object->GetObjectKind())
			{
				continue;
			}
		
			object->SendMsg(
				&message,
				sizeof(message));
		}
	}
}
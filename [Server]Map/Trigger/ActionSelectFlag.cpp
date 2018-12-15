#include "StdAfx.h"
#include "ActionSelectFlag.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"

namespace Trigger
{
	void CActionSelectFlag::DoAction()
	{
		const DWORD hashCode = GetValue(eProperty_Name);
		const DWORD yesValue = GetValue(eProperty_Value);
		const DWORD noValue = GetValue(eProperty_ValueReject);
		const DWORD messageIndex = GetValue(eProperty_Text);
		const DWORD waitSecond = GetValue(eProperty_ElapsedSecond);

		MSG_NAME_DWORD5 message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_SELECT_FLAG_ACK;
		message.dwData1 = yesValue;
		message.dwData2 = noValue;
		message.dwData3 = MAKELPARAM(messageIndex, waitSecond);
		message.dwData4 = mParameter.mOwnerIndex;
		message.dwData5 = mParameter.mOwnerType;
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
#include "StdAfx.h"
#include "ActionAnimation.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"
#include "..\CharMove.h"

namespace Trigger
{
	void CActionAnimation::DoAction()
	{
		const DWORD hashCode = GetValue(eProperty_Name);
		TESTMSG message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_ANIMATION_BEGIN_ACK;
		SafeStrCpy(
			message.Msg,
			GetAliasName(hashCode),
			sizeof(message.Msg) / sizeof(*message.Msg));
		const int messageLength = message.GetMsgLength();
		
		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			CCharMove::SetPosition( object, CCharMove::GetPosition( object ) );
		
			object->SendMsg(
				&message,
				messageLength);
		}
	}
}
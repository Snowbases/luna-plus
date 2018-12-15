#include "StdAfx.h"
#include "ActionPutNpcChat.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"

namespace Trigger
{
	void CActionPutNpcChat::DoAction()
	{
		const DWORD hashCode = GetValue(eProperty_Name);
		const DWORD second = GetValue(eProperty_ElapsedSecond);
		const DWORD textIndex = GetValue(eProperty_Text);

		MSG_NAME_DWORD2 message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_NPC;
		message.Protocol = MP_NPC_PUT_CHAT_ACK;
		SafeStrCpy(
			message.Name,
			GetAliasName(hashCode),
			sizeof(message.Name) / sizeof(*message.Name));
		message.dwData1 = second;
		message.dwData2 = textIndex;
		
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
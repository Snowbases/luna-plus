#include "StdAfx.h"
#include "ActionSetReviveFlag.h"
#include "common.h"
#include "..\Player.h"
#include "..\UserTable.h"

namespace Trigger
{
	void CActionSetReviveFlag::DoAction()
	{
		MSG_INT message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_CHARACTER_REVIVE_FLAG_ACK;
		const ReviveFlag flag = ReviveFlag(GetValue(eProperty_ReviveFlag));
		
		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(objectIndex);

			if(0 == player)
			{
				continue;
			}
			else if(eObjectKind_Player != player->GetObjectKind())
			{
				continue;
			}

			message.nData = int(flag);
			player->SendMsg(
				&message,
				sizeof(message));
			player->SetReviveFlag(flag);
		}
	}
}
#include "StdAfx.h"
#include "ActionAddMoney.h"
#include "common.h"
#include "..\Player.h"
#include "..\UserTable.h"

namespace Trigger
{
	void CActionAddMoney::DoAction()
	{
		const int money = GetValue(eProperty_Money);
		const BYTE moneyType = BYTE(0 < money ? MONEY_ADDITION : MONEY_SUBTRACTION);

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

			player->SetMoney(
				money,
				moneyType,
				MF_NOMAL,
				eItemTable_Inventory,
				eMoneyLog_TriggerAction);
		}
	}
}
#include "StdAfx.h"
#include "ActionGetItem.h"
#include "common.h"
#include "Trigger.h"
#include "..\Player.h"
#include "..\UserTable.h"
#include "..\ItemManager.h"
#include "..\ItemDrop.h"

namespace Trigger
{
	void CActionGetItem::DoAction()
	{
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

			const DWORD itemIndex = GetValue(eProperty_ItemIndex);
			const WORD itemQuantity = (WORD)GetValue(eProperty_ItemQuantity);
			WORD itemDropRate = (WORD)GetValue(eProperty_ItemDropRate);
			// 만분율에 주의 itemDropRate=1 ==> 1/10000 확률
			const int randvalue = rand() % 10000;

			if(0 == itemDropRate)
			{
				itemDropRate = MAX_DROP_ITEM_PERCENT;
			}

			PutLog("%d: DropRate(%d) : RandValue(%d)\n",
				GetTrigger().GetIndex(),
				itemDropRate,
				WORD(randvalue));

			if(randvalue <= itemDropRate)
			{
				ITEMMGR->CheatObtainItem(
					player,
					itemIndex,
					itemQuantity);
			}
		}
	}
}
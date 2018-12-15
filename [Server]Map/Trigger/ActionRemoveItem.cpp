#include "StdAfx.h"
#include "ActionRemoveItem.h"
#include "common.h"
#include "..\UserTable.h"
#include "..\Player.h"

namespace Trigger
{
	void CActionRemoveItem::DoAction()
	{
		const DWORD itemIndex = GetValue(eProperty_ItemIndex);
		const DURTYPE itemQuantity = GetValue(eProperty_ItemQuantity);

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

			player->RemoveItem(
				itemIndex,
				itemQuantity,
				eLog_RemoveItemByTrigger);
		}
	}
}
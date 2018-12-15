#include "StdAfx.h"
#include "ActionCristalEmpireBonus.h"
#include "common.h"
#include "Manager.h"
#include "..\UserTable.h"
#include "..\Object.h"

namespace Trigger
{
	void CActionCristalEmpireBonus::DoAction()
	{
		const DWORD itemIndex = GetValue(eProperty_ItemIndex);
		const ITEM_OPTION::Drop::Key key = ITEM_OPTION::Drop::Key(GetValue(eProperty_ItemOption));
		float power = float(GetValue(eProperty_Value));
		const LEVELTYPE level = LEVELTYPE(GetValue(eProperty_Event));
		const size_t size = GetValue(eProperty_ItemQuantity);

		switch(key)
		{
		case ITEM_OPTION::Drop::KeyPercentStrength:
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
		case ITEM_OPTION::Drop::KeyPercentDexterity:
		case ITEM_OPTION::Drop::KeyPercentWisdom:
		case ITEM_OPTION::Drop::KeyPercentVitality:
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
		case ITEM_OPTION::Drop::KeyPercentEvade:
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
		case ITEM_OPTION::Drop::KeyPercentLife:
		case ITEM_OPTION::Drop::KeyPercentMana:
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				power = power / 100;
				break;
			}
		}

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			Trigger::CManager::GetInstance().BatchItemOption(
				objectIndex,
				itemIndex,
				key,
				power,
				level,
				size);
		}
	}
}
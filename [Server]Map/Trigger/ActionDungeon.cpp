#include "StdAfx.h"
#include "ActionDungeon.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"
#include "./Dungeon/DungeonMgr.h"
#include "..\PackedData.h"

namespace Trigger
{
	void CActionDungeonSwitchOn::DoAction()
	{
		const WORD switchNum = (WORD)GetValue(eProperty_Dungeon_SwitchOn);
		if(0 == switchNum)
			return;

		// 해당 스위치가 On상태이면 처리할 필요가 없으므로 종료시킨다.
		int switchState = DungeonMGR->GetSwitch(GetChannelID(), switchNum - 1);
		if( 0 != switchState )
			return;

		DungeonMGR->SetSwitch(GetChannelID(), switchNum - 1, TRUE);

		// 스위치로 NPC를 사용할 경우, 이펙트를 표시하여 스위치 On/Off를 표현한다.
		const WORD dwNpcIndex = (WORD)GetValue(eProperty_ActiveNpcId);
		if( 0 == dwNpcIndex || GetValue(eProperty_ObjectKind) != eObjectKind_Npc)
			return;
		
        const DWORD dwObjectIndex = DungeonMGR->GetNpcObjectIndex(GetChannelID(), dwNpcIndex);
		if( 0 == dwObjectIndex )
			return;

		const WORD wEffectIndex = (WORD)GetValue(eProperty_EffectDescNum);

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}
			MSG_DWORD5	msg;
			msg.Category = MP_TRIGGER;
			msg.Protocol = MP_TRIGGER_ADDEFFECT_ACK;
			msg.dwObjectID = dwObjectIndex;
			msg.dwData1 = wEffectIndex;
			msg.dwData2 = GetValue(eProperty_EffectAddToMap);
			msg.dwData3 = GetValue(eProperty_PosX);
			msg.dwData4 = GetValue(eProperty_PosY);
			msg.dwData5 = GetValue(eProperty_PosZ);

			PACKEDDATA_OBJ->QuickSend( object, &msg, sizeof( msg ) );
		}
		return ;
	}

	void CActionDungeonSwitchOff::DoAction()
	{
		const WORD switchNum = (WORD)GetValue(eProperty_Dungeon_SwitchOff);
		if(0 == switchNum)
			return;

		// 해당 스위치가 Off상태이면 처리할 필요가 없으므로 종료시킨다.
		int switchState = DungeonMGR->GetSwitch(GetChannelID(), switchNum - 1);
		if( 0 == switchState )
			return;

		DungeonMGR->SetSwitch(GetChannelID(), switchNum - 1, FALSE);

		// 스위치로 NPC를 사용할 경우, 이펙트를 표시하여 스위치 On/Off를 표현한다.
		const WORD dwNpcIndex = (WORD)GetValue(eProperty_ActiveNpcId);
		if( 0 == dwNpcIndex || GetValue(eProperty_ObjectKind) != eObjectKind_Npc)
			return;
		
        const DWORD dwObjectIndex = DungeonMGR->GetNpcObjectIndex(GetChannelID(), dwNpcIndex);
		if( 0 == dwObjectIndex )
			return;

		const WORD wEffectIndex = (WORD)GetValue(eProperty_EffectDescNum);

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}
			MSG_DWORD2	msg;
			msg.Category = MP_TRIGGER;
			msg.Protocol = MP_TRIGGER_REMOVEEFFECT_ACK;
			msg.dwObjectID = dwObjectIndex;
			msg.dwData1 = wEffectIndex;
			msg.dwData2 = 0;
			PACKEDDATA_OBJ->QuickSend( object, &msg, sizeof( msg ) );
		}
	}

	void CActionDungeonSwitchToggle::DoAction()
	{
		const WORD switchNum = (WORD)GetValue(eProperty_Dungeon_SwitchToggle);
		if(0 == switchNum)
			return;

		const BOOL toggle = ! DungeonMGR->GetSwitch(GetChannelID(), switchNum - 1);
		DungeonMGR->SetSwitch(GetChannelID(), switchNum - 1, toggle);
	}

	void CActionDungeonWarpOn::DoAction()
	{
		const DWORD warpIndex = GetValue(eProperty_Dungeon_WarpOn);
		if(0 == warpIndex)
			return;

		DungeonMGR->SetWarp(GetChannelID(), warpIndex, TRUE);
	}

	void CActionDungeonWarpOff::DoAction()
	{
		const DWORD warpIndex = GetValue(eProperty_Dungeon_WarpOff);
		if(0 == warpIndex)
			return;

		DungeonMGR->SetWarp(GetChannelID(), warpIndex, FALSE);
	}

	void CActionDungeonSetBossMonster::DoAction()
	{
		DungeonMGR->SetBossMonster(GetChannelID());
	}

	void CActionDungeonAddPoint::DoAction()
	{
		const int addPoint = GetValue(eProperty_Dungeon_AddPoint);
		if(0 < addPoint)
			DungeonMGR->AddPoint(GetChannelID(), addPoint);
		else if(0 > addPoint)
			DungeonMGR->SubtractPoint(GetChannelID(), abs(addPoint));		
	}

	void CActionDungeonSummonMonsterGroup::DoAction()
	{
		const DWORD dwGroupAlias	= GetValue(eProperty_GroupAlias);
		const DWORD dwAlias			= GetValue(eProperty_Alias);
		DWORD dwAliasValue			= 0;
		eAliasKind	Aliaskind		= eAliasKind_None;	

		if(dwAlias)
		{
			dwAliasValue = dwAlias;
			Aliaskind = eAliasKind_Alias;
		}
		if(dwGroupAlias)
		{		
			dwAliasValue = dwGroupAlias;
			Aliaskind = eAliasKind_GroupAlias;
		}

		if(0 == dwAliasValue)
			return ;

		DungeonMGR->SummonMonsterGroup( dwAliasValue, mParameter.mChannelID, Aliaskind );
	}

	void CActionDungeonSwitchNpcOn::DoAction()
	{
		const DWORD npcIndex = GetValue(eProperty_ActiveNpcId);
		if(0 == npcIndex)
			return;

		DungeonMGR->SetSwitchNpc(GetChannelID(), (WORD)npcIndex, TRUE);
	}

	void CActionDungeonSwitchNpcOff::DoAction()
	{
		const DWORD npcIndex = GetValue(eProperty_ActiveNpcId);
		if(0 == npcIndex)
			return;

		DungeonMGR->SetSwitchNpc(GetChannelID(), (WORD)npcIndex, FALSE);
	}

	void CActionDungeonZoneBlock::DoAction()
	{
		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			MSG_DWORD3	msg;
			msg.Category = MP_DUNGEON;
			msg.Protocol = MP_DUNGEON_ZONEBLOCK;
			msg.dwObjectID = object->GetID();
			msg.dwData1 = GetValue(eProperty_RangeX);
			msg.dwData2 = GetValue(eProperty_RangeZ);
			msg.dwData3 = GetValue(eProperty_Range);
			object->SendMsg(&msg, sizeof(msg));
		}
	}

	void CActionDungeonZoneFree::DoAction()
	{
		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			MSG_DWORD3	msg;
			msg.Category = MP_DUNGEON;
			msg.Protocol = MP_DUNGEON_ZONEFREE;
			msg.dwObjectID = object->GetID();
			msg.dwData1 = GetValue(eProperty_RangeX);
			msg.dwData2 = GetValue(eProperty_RangeZ);
			msg.dwData3 = GetValue(eProperty_Range);
			object->SendMsg(&msg, sizeof(msg));
		}
	}

	void CActionDungeonEdgeBlock::DoAction()
	{
		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			MSG_DWORD3	msg;
			msg.Category = MP_DUNGEON;
			msg.Protocol = MP_DUNGEON_EDGEBLOCK;
			msg.dwObjectID = object->GetID();
			msg.dwData1 = GetValue(eProperty_RangeX);
			msg.dwData2 = GetValue(eProperty_RangeZ);
			msg.dwData3 = GetValue(eProperty_Range);
			object->SendMsg(&msg, sizeof(msg));
		}
	}

	void CActionDungeonEdgeFree::DoAction()
	{
		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			MSG_DWORD3	msg;
			msg.Category = MP_DUNGEON;
			msg.Protocol = MP_DUNGEON_EDGEFREE;
			msg.dwObjectID = object->GetID();
			msg.dwData1 = GetValue(eProperty_RangeX);
			msg.dwData2 = GetValue(eProperty_RangeZ);
			msg.dwData3 = GetValue(eProperty_Range);
			object->SendMsg(&msg, sizeof(msg));
		}
	}
}

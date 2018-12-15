#include "StdAfx.h"
#include ".\dungeonprocessorbrokenshipwrecked.h"

#include "..\[CC]Header\GameResourceManager.h"
#include "..\Monster.h"
#include "..\RegenManager.h"
#include "..\AISystem.h"
#include "DungeonMgr.h"
#include "..\Object.h"
#include "..\UserTable.h"
#include "..\Trigger\Manager.h"

CDungeonProcessorBrokenShipwrecked::CDungeonProcessorBrokenShipwrecked(void)
{
}

CDungeonProcessorBrokenShipwrecked::~CDungeonProcessorBrokenShipwrecked(void)
{
}

void CDungeonProcessorBrokenShipwrecked::Create(stDungeon* pDungeon)
{
}

void CDungeonProcessorBrokenShipwrecked::Process()
{
}

void CDungeonProcessorBrokenShipwrecked::SetSwitch(DWORD dwChannelID, WORD num, BOOL val)
{
	if(val && num == Switch_for_FallingRock)
	{
		// 동굴이 무너짐 이펙트. 트리거의 AddEffect 액션을 사용하여 정보를 날리자.
		g_pUserTable->SetPositionHead();
		CObject* pObject;
		while((pObject = g_pUserTable->GetData()) != NULL)
		{
			if(pObject->GetObjectKind() == eObjectKind_Player &&
				pObject->GetGridID() == dwChannelID)
			{
				MSG_DWORD5	msg;
				msg.Category = MP_TRIGGER;
				msg.Protocol = MP_TRIGGER_ADDEFFECT_ACK;
				msg.dwObjectID = pObject->GetID();
				msg.dwData1 = EffectDescNum_FallingRock;
				msg.dwData2 = 1;		// 맵이펙트
				msg.dwData3 = 43336;	//GetValue(eProperty_PosX);
				msg.dwData4 = 33;		//GetValue(eProperty_PosY);
				msg.dwData5 = 16135;	//GetValue(eProperty_PosZ);
				pObject->SendMsg(&msg, sizeof(msg));

				// 영역블럭
				MSG_DWORD3	msg2;
				msg2.Category = MP_DUNGEON;
				msg2.Protocol = MP_DUNGEON_ZONEBLOCK;
				msg2.dwObjectID = pObject->GetID();
				msg2.dwData1 = Range_X_FallenRock;	//GetValue(eProperty_RangeX);
				msg2.dwData2 = Range_Z_FallenRock;	//GetValue(eProperty_RangeZ);
				msg2.dwData3 = Range_FallenRock;	//GetValue(eProperty_Range);
				pObject->SendMsg(&msg2, sizeof(msg2));
			}
		}
	}
	else if(val && num == Switch_for_FallenRock)
	{
		// 동굴이 무너짐 이펙트. 트리거의 AddEffect 액션을 사용하여 정보를 날리자.
		g_pUserTable->SetPositionHead();
		CObject* pObject;
		while((pObject = g_pUserTable->GetData()) != NULL)
		{
			if(pObject->GetObjectKind() == eObjectKind_Player &&
				pObject->GetGridID() == dwChannelID)
			{
				MSG_DWORD2	msg;
				msg.Category = MP_TRIGGER;
				msg.Protocol = MP_TRIGGER_REMOVEEFFECT_ACK;
				msg.dwObjectID = pObject->GetID();
				msg.dwData1 = EffectDescNum_FallingRock;
				msg.dwData2 = 1;	// 맵이펙트
				pObject->SendMsg(&msg, sizeof(msg));

				MSG_DWORD5	msg2;
				msg2.Category = MP_TRIGGER;
				msg2.Protocol = MP_TRIGGER_ADDEFFECT_ACK;
				msg2.dwObjectID = pObject->GetID();
				msg2.dwData1 = EffectDescNum_FallenRock;
				msg2.dwData2 = 1;		// 맵이펙트
				msg2.dwData3 = 43336;	//GetValue(eProperty_PosX);
				msg2.dwData4 = 33;		//GetValue(eProperty_PosY);
				msg2.dwData5 = 16135;	//GetValue(eProperty_PosZ);
				pObject->SendMsg(&msg2, sizeof(msg2));
			}
		}
		
		DungeonMGR->SetSwitch(dwChannelID, Switch_for_FallingRock, FALSE);
	}
}

void CDungeonProcessorBrokenShipwrecked::Info_Syn(CObject* pObject)
{
	if(! pObject)
		return;

	// 스위치의 상태에 따라 동굴무너짐 정보를 보내준다.
	if(DungeonMGR->GetSwitch(pObject->GetGridID(), Switch_for_FallingRock))
	{
		// 동굴 무너지는 중.
		MSG_DWORD5	msg;
		msg.Category = MP_TRIGGER;
		msg.Protocol = MP_TRIGGER_ADDEFFECT_ACK;
		msg.dwObjectID = pObject->GetID();
		msg.dwData1 = EffectDescNum_FallingRock;
		msg.dwData2 = 1;		// 맵이펙트
		msg.dwData3 = 43336;	//GetValue(eProperty_PosX);
		msg.dwData4 = 33;		//GetValue(eProperty_PosY);
		msg.dwData5 = 16135;	//GetValue(eProperty_PosZ);
		pObject->SendMsg(&msg, sizeof(msg));

		// 영역블럭
		MSG_DWORD3	msg2;
		msg2.Category = MP_DUNGEON;
		msg2.Protocol = MP_DUNGEON_ZONEBLOCK;
		msg2.dwObjectID = pObject->GetID();
		msg2.dwData1 = Range_X_FallenRock;	//GetValue(eProperty_RangeX);
		msg2.dwData2 = Range_Z_FallenRock;	//GetValue(eProperty_RangeZ);
		msg2.dwData3 = Range_FallenRock;	//GetValue(eProperty_Range);
		pObject->SendMsg(&msg2, sizeof(msg2));
	}
	else if(DungeonMGR->GetSwitch(pObject->GetGridID(), Switch_for_FallenRock))
	{
		// 동굴 무너졌음.
		MSG_DWORD2	msg;
		msg.Category = MP_TRIGGER;
		msg.Protocol = MP_TRIGGER_REMOVEEFFECT_ACK;
		msg.dwObjectID = pObject->GetID();
		msg.dwData1 = EffectDescNum_FallingRock;
		msg.dwData2 = 1;	// 맵이펙트
		pObject->SendMsg(&msg, sizeof(msg));

		MSG_DWORD5	msg2;
		msg2.Category = MP_TRIGGER;
		msg2.Protocol = MP_TRIGGER_ADDEFFECT_ACK;
		msg2.dwObjectID = pObject->GetID();
		msg2.dwData1 = EffectDescNum_FallenRock;
		msg2.dwData2 = 1;		// 맵이펙트
		msg2.dwData3 = 43336;	//GetValue(eProperty_PosX);
		msg2.dwData4 = 33;		//GetValue(eProperty_PosY);
		msg2.dwData5 = 16135;	//GetValue(eProperty_PosZ);
		pObject->SendMsg(&msg2, sizeof(msg2));

		// 영역블럭
		MSG_DWORD3	msg3;
		msg3.Category = MP_DUNGEON;
		msg3.Protocol = MP_DUNGEON_ZONEBLOCK;
		msg3.dwObjectID = pObject->GetID();
		msg3.dwData1 = Range_X_FallenRock;	//GetValue(eProperty_RangeX);
		msg3.dwData2 = Range_Z_FallenRock;	//GetValue(eProperty_RangeZ);
		msg3.dwData3 = Range_FallenRock;	//GetValue(eProperty_Range);
		pObject->SendMsg(&msg3, sizeof(msg3));
	}
}

void CDungeonProcessorBrokenShipwrecked::SetBossMonster(stDungeon* pDungeon)
{
	if(! pDungeon)
		return;

	WORD wBossMonsterKind = 0;
	switch(pDungeon->difficulty)
	{
	case eDIFFICULTY_EASY:
		{
			if(0<=pDungeon->dwPoint && pDungeon->dwPoint<101)
				wBossMonsterKind = 696;
			else if(101<=pDungeon->dwPoint && pDungeon->dwPoint<201)
				wBossMonsterKind = 697;
			else if(201<=pDungeon->dwPoint && pDungeon->dwPoint<301)
				wBossMonsterKind = 698;
			else
				wBossMonsterKind = 699;
		}
		break;
	case eDIFFICULTY_NORMAL:
		{
			if(0<=pDungeon->dwPoint && pDungeon->dwPoint<101)
				wBossMonsterKind = 700;
			else if(101<=pDungeon->dwPoint && pDungeon->dwPoint<201)
				wBossMonsterKind = 701;
			else if(201<=pDungeon->dwPoint && pDungeon->dwPoint<301)
				wBossMonsterKind = 702;
			else
				wBossMonsterKind = 703;
		}
		break;
	case eDIFFICULTY_HARD:
		{
			if(0<=pDungeon->dwPoint && pDungeon->dwPoint<101)
				wBossMonsterKind = 704;
			else if(101<=pDungeon->dwPoint && pDungeon->dwPoint<201)
				wBossMonsterKind = 705;
			else if(201<=pDungeon->dwPoint && pDungeon->dwPoint<301)
				wBossMonsterKind = 706;
			else
				wBossMonsterKind = 707;
		}
		break;
	}

	if(! wBossMonsterKind)
		return;

	// 보스생성하자~
	BASE_MONSTER_LIST* pMonsterList = GAMERESRCMNGR->GetMonsterListInfo(wBossMonsterKind);
	if(! pMonsterList)
		return;

	VECTOR3 vPos = {12300, 0, 42400};
	// 100104 LUJ, 서브 인덱스는 리젠 스크립트(Monster_%02d.bin)를 읽을 때
	//			생성되는 고유 번호이다. 따라서 리젠 스크립트에서 생성되지
	//			않는 몬스터는 0이어도 관계없다
	CMonster* pAddMonster = REGENMGR->RegenObject(
		g_pAISystem.GeneraterMonsterID(),
		0,
		pDungeon->dwChannelID,
		pMonsterList->ObjectKind,
		wBossMonsterKind,
		&vPos,
		(DWORD)DUNGEON_MONSTER_GROUP_ID,
		0,
		0,
		FALSE);

	DWORD alias = 0;
	DWORD count = 1;
	DWORD groupAlias = 0;
	// 별칭은 맵전용으로 등록한다.
	char buffer[MAX_PATH] = {0};
	sprintf(
		buffer,
		"[%s%d].%d.%d.%d",
		"보스", count,
		Trigger::eOwner_Server,
		pDungeon->dwChannelID,
		pDungeon->dwChannelID);
	alias = TRIGGERMGR->GetHashCode(buffer);
	pAddMonster->SetAlias(alias);

	// 그룹별칭은 맵전용으로 등록한다.
	//char buffer[MAX_PATH] = {0};
	sprintf(
		buffer,
		"[%s].%d.%d.%d",
		"보스그룹",
		Trigger::eOwner_Server,
		pDungeon->dwChannelID,
		pDungeon->dwChannelID);
	groupAlias = TRIGGERMGR->GetHashCode(buffer);
	pAddMonster->SetGroupAlias(groupAlias);
}
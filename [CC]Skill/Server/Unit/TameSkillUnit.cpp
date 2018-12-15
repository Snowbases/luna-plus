#include "StdAfx.h"
#include "TameSkillUnit.h"
#include "..\Object\ActiveSkillObject.h"
#include "..\Target\Target.h"
#include "Monster.h"
#include "PackedData.h"
#include "UserTable.h"
#include "StateMachinen.h"

cTameSkillUnit::cTameSkillUnit(cActiveSkillObject& skillObject) :
cSkillUnit(skillObject)
{}

cTameSkillUnit::~cTameSkillUnit(void)
{}

BOOL cTameSkillUnit::Excute()
{
	CObject* const sourceObject = mpParentSkill->GetOperator();

	if(0 == sourceObject)
	{
		return FALSE;
	}

	size_t tamingSize = sourceObject->GetSummondSize(
		mpParentSkill->GetInfo());
	const size_t maxTamingSize = mpParentSkill->GetInfo().UnitDataType;

	if(maxTamingSize < tamingSize)
	{
		return FALSE;
	}
	else if(eObjectKind_Player == sourceObject->GetObjectKind())
	{
		if(maxTamingSize == tamingSize)
		{
			sourceObject->RemoveSummonedOldest(
				mpParentSkill->GetInfo());
		}

		tamingSize = 1;
	}

	const int successRate = int(float(RAND_MAX) * mpParentSkill->GetInfo().UnitData / 100.0f);

	mpParentSkill->GetTarget().SetPositionHead();
	
	while(CMonster* const targetMonster = (CMonster*)mpParentSkill->GetTarget().GetData())
	{
		if(0 == tamingSize)
		{
			break;
		}
		else if(eObjectKind_Monster != targetMonster->GetObjectKind())
		{
			continue;
		}
		else if(rand() > successRate)
		{
			continue;
		}
		else if(CObject* const ownerObject = g_pUserTable->FindUser(targetMonster->GetOwnerIndex()))
		{
			if(targetMonster->IsInvalidTarget(*ownerObject))
			{
				continue;
			}
		}

		targetMonster->SetTObject(
			sourceObject);
		GSTATEMACHINE.SetState(
			sourceObject,
			eMA_PERSUIT);

		MONSTER_TOTALINFO& monsterTotalInfo = targetMonster->GetMonsterTotalInfo();
		monsterTotalInfo.OwnedObjectIndex = sourceObject->GetID();

		sourceObject->AddSummoned(
			mpParentSkill->GetInfo(),
			targetMonster->GetID());
		targetMonster->RemoveAllAggro();

		MSG_DWORD message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_MONSTER_TAME_ACK;
		message.dwObjectID = sourceObject->GetID();
		message.dwData = targetMonster->GetID();

		PACKEDDATA_OBJ->QuickSend(
			targetMonster,
			&message,
			sizeof(message));
	}

	return TRUE;
}
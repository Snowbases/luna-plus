#include "StdAfx.h"
#include "SummonSkillUnit.h"
#include "..\Object\ActiveSkillObject.h"
#include "..\Target\Target.h"
#include "AISystem.h"
#include "RegenManager.h"
#include "Monster.h"
#include "StateMachinen.h"
#include "..\[CC]Header\GameResourceManager.h"

cSummonSkillUnit::cSummonSkillUnit(cActiveSkillObject& skillObject) :
cSkillUnit(skillObject)
{}

cSummonSkillUnit::~cSummonSkillUnit(void)
{}

BOOL cSummonSkillUnit::Excute()
{
	CObject* const ownerObject = mpParentSkill->GetOperator();

	if(0 == ownerObject)
	{
		return FALSE;
	}

	const int totalSize = mpParentSkill->GetInfo().UnitDataType;
	const int summonedSize = ownerObject->GetSummondSize(mpParentSkill->GetInfo());

	if(summonedSize > totalSize)
	{
		return FALSE;
	}

	int summoningSize = totalSize - summonedSize;

	// 플레이어는 1개씩 소환한다
	if(eObjectKind_Player == ownerObject->GetObjectKind())
	{		
		if(summonedSize == totalSize)
		{
			ownerObject->RemoveSummonedOldest(
				mpParentSkill->GetInfo());
		}

		summoningSize = 1;
	}

	const WORD summonMonsterKind = WORD(mpParentSkill->GetInfo().UnitData);
	const BASE_MONSTER_LIST* const monsterList = GAMERESRCMNGR->GetMonsterListInfo(
		summonMonsterKind);

	if(0 ==	monsterList)
	{
		return FALSE;
	}

	mpParentSkill->GetTarget().SetPositionHead();
	size_t targetSize = mpParentSkill->GetTarget().GetSize();

	while(CObject* const targetObject = mpParentSkill->GetTarget().GetData())
	{
		VECTOR3 targetPosition = {0};
		targetObject->GetPosition(
			&targetPosition);

		if(0 >= summoningSize)
		{
			break;
		}

		/*
		이 스킬을 광역으로 쓸 경우를 가정해보자. 소환수 5개를 대상 7곳에 적용하려면 어떻게 나눠야할까. 또는
		소환수 11개를 대상 5곳에 적용하려면 어떻게 나눠야할까.

		ceil()을 사용해서 조금이라도 소수점이 발생하면 1을 더준다. 항상 맨 나중에는 1로 나누기 때문에 모든
		개수를 다 가져간다. 나눌 개수가 없으면 0이 된다. 이런 식으로 모자라든 남든 항상 골고루 분해할 수 있다
		*/
		const int eachSummoningSize = int(ceil(float(summoningSize) / targetSize--));
		summoningSize = summoningSize - eachSummoningSize;

		for(int i = 0; i < eachSummoningSize; ++i)
		{
			const float randomRateX = float(rand()) / RAND_MAX;
			const float randomRateZ = float(rand()) / RAND_MAX;
			const float randomAxisX = (randomRateX < 0.5f ? -1.0f : 1.0f) * (ownerObject->GetRadius() * (1.0f + randomRateX));
			const float randomAxisZ = (randomRateZ < 0.5f ? -1.0f : 1.0f) * (ownerObject->GetRadius() * (1.0f + randomRateZ));

			VECTOR3 summonPosition = {0};
			summonPosition.x = targetPosition.x + randomAxisX;
			summonPosition.z = targetPosition.z + randomAxisZ;

			CMonster* const monster = REGENMGR->RegenObject(
				g_pAISystem.GeneraterMonsterID(),
				0,
				ownerObject->GetGridID(),
				monsterList->ObjectKind,
				monsterList->MonsterKind,
				&summonPosition,
				0,
				0,
				0,
				FALSE,
				TRUE,
				ownerObject->GetID());

			if(0 == monster)
			{
				return FALSE;
			}
			else if(TRUE == monster->SetTObject(ownerObject->GetTObject()))
			{
				GSTATEMACHINE.SetState(
					monster,
					eMA_PERSUIT);
			}

			ownerObject->AddSummoned(
				mpParentSkill->GetInfo(),
				monster->GetID());
		}
	}

	return TRUE;
}
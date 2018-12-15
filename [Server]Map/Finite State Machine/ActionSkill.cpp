#include "StdAfx.h"
#include "ActionSkill.h"
#include "ActionPause.h"
#include "ActionMove.h"
#include "Machine.h"
#include "Memory.h"
#include "..\Monster.h"
#include "..\StateMachinen.h"
#include "..\UserTable.h"
#include "..\StateMachinen.h"
#include "..\..\[CC]Skill\Server\Info\ActiveSkillInfo.h"
#include "..\..\[CC]Skill\Server\Manager\SkillManager.h"
#include "..\..\[CC]Skill\Server\Object\BuffSkillObject.h"
#include "..\..\4DyuchiGXGFunc\global.h"

namespace FiniteStateMachine
{
	CActionSkill::CActionSkill(DWORD objectIndex, DWORD skillIndex) :
	CAction("Skill"),
	mObjectIndex(objectIndex),
	mSkillIndex(skillIndex)
	{}

	int CActionSkill::Run(CMachine& machine) const
	{
		CMonster* const sourceObject = (CMonster*)g_pUserTable->FindUser(
			machine.GetObjectIndex());

		if(0 == sourceObject)
		{
			return 0;
		}
		else if(FALSE == (eObjectKind_Monster & sourceObject->GetObjectKind()))
		{
			return 0;
		}

		CObject* const targetObject = g_pUserTable->FindUser(
			mObjectIndex);

		if(0 == targetObject)
		{
			return 0;
		}

		const cActiveSkillInfo* const skillInfo = SKILLMGR->GetActiveInfo(
			mSkillIndex);

		if(0 == skillInfo)
		{
			return 0;
		}

		// 버프인 경우 이미 스킬이 적용되었는지 검사한다
		{
			targetObject->GetBuffList().SetPositionHead();

			while(cBuffSkillObject* const skillObject = targetObject->GetBuffList().GetData())
			{
				for(int i = 0; i < _countof(skillInfo->GetInfo().Buff); ++i)
				{
					if(skillObject->GetSkillIdx() == skillInfo->GetInfo().Buff[i])
					{
						return 1;
					}
				}
			}
		}

		VECTOR3 sourcePosition = {0};
		sourceObject->GetPosition(
			&sourcePosition);
		VECTOR3	targetPosition = {0};
		targetObject->GetPosition(
			&targetPosition);

		if(skillInfo->GetInfo().Range < CalcDistance(&sourcePosition, &targetPosition))
		{
			VECTOR3 directionVector = targetPosition - sourcePosition;
			VECTOR3 normalizedVector = {0};
			Normalize(
				&normalizedVector,
				&directionVector);

			CActionMove action(
				targetPosition + normalizedVector * skillInfo->GetInfo().Range,
				eMA_SCRIPT_RUN);
			action.Run(
				machine);
			return 0;
		}

		sourceObject->MoveStop();
		SKILLMGR->MonsterAttack(
			mSkillIndex,
			sourceObject,
			targetObject);

		CActionPause actionPause(
			skillInfo->GetInfo().CastingTime + skillInfo->GetInfo().AnimationTime);
		actionPause.Run(
			machine);
		return 1;
	}
}
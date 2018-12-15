#include "StdAfx.h"
#include "ActionSkillToTarget.h"
#include "ActionPause.h"
#include "ActionMove.h"
#include "Machine.h"
#include "Parser.h"
#include "..\Monster.h"
#include "..\StateMachinen.h"
#include "..\UserTable.h"
#include "..\..\[CC]Skill\Server\Info\ActiveSkillInfo.h"
#include "..\..\[CC]Skill\Server\Manager\SkillManager.h"
#include "..\..\4DyuchiGXGFunc\global.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			const DWORD skillIndex = _ttoi(
				parameter1);

			return new CActionSkillToTarget(
				skillIndex);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"SkillToTarget",
			Clone);
	}

	CActionSkillToTarget::CActionSkillToTarget(DWORD skillIndex) :
	CAction("SkillToTarget"),
	mSkillIndex(skillIndex)
	{}

	int CActionSkillToTarget::Run(CMachine& machine) const
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

		CObject* const targetObject = sourceObject->GetTObject();

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
		return actionPause.Run(
			machine);
	}
}
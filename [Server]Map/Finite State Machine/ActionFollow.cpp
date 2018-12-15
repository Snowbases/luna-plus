#include "StdAfx.h"
#include "ActionFollow.h"
#include "Machine.h"
#include "Memory.h"
#include "..\UserTable.h"
#include "..\Monster.h"
#include "..\CharMove.h"
#include "..\StateMachinen.h"
#include "..\..\4DyuchiGXGFunc\global.h"

namespace FiniteStateMachine
{
	CActionFollow::CActionFollow(DWORD targetObjectIndex, float distance) :
	CAction("Follow"),
	mTargetObjectIndex(targetObjectIndex),
	mDistance(distance)
	{}

	int CActionFollow::Run(CMachine& machine) const
	{
		CMonster* const monsterObject = (CMonster*)g_pUserTable->FindUser(
			machine.GetObjectIndex());

		if(0 == monsterObject)
		{
			return 0;
		}
		
		const StateParameter& stateParameter = monsterObject->mStateParamter;

		switch(stateParameter.stateCur)
		{
		case eMA_ATTACK:
		case eMA_PERSUIT:
		case eMA_PAUSE:
		case eMA_WALKAROUND:
		case eMA_SCRIPT_RUN:
		case eMA_SCRIPT_WALK:
			{
				return 0;
			}
		}

		VECTOR3 monsterPosition = *(CCharMove::GetPosition(
			monsterObject));
		VECTOR3 targetPosition = {0};
		
		{
			CMonster* const targetObject = (CMonster*)g_pUserTable->FindUser(
				mTargetObjectIndex);

			if(0 == targetObject)
			{
				GSTATEMACHINE.SetState(
					monsterObject,
					eMA_WALKAROUND);
				return 0;
			}
			else if(FALSE == (eObjectKind_Monster & targetObject->GetObjectKind()))
			{
				return 0;
			}

			targetPosition = *(CCharMove::GetPosition(
				targetObject));

			if(CalcDistance(&targetPosition, &monsterPosition) < mDistance)
			{
				GSTATEMACHINE.SetState(
					monsterObject,
					eMA_STAND);
				return 0;
			}
		}

		const float randomRateX = float(rand()) / RAND_MAX;
		const float randomRateZ = float(rand()) / RAND_MAX;
		const float randomAxisX = (randomRateX < 0.5f ? -1.0f : 1.0f) * (monsterObject->GetRadius() * (1.0f + randomRateX));
		const float randomAxisZ = (randomRateZ < 0.5f ? -1.0f : 1.0f) * (monsterObject->GetRadius() * (1.0f + randomRateZ));
		
		machine.GetMemory().SetVariable(
			"__move_x__",
			int(targetPosition.x + randomAxisX));
		machine.GetMemory().SetVariable(
			"__move_z__",
			int(targetPosition.z + randomAxisZ));
		
		GSTATEMACHINE.SetState(
			monsterObject,
			eMA_SCRIPT_RUN);
		return 1;
	}
}
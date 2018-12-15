#include "StdAfx.h"
#include "ActionMove.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"
#include "..\UserTable.h"
#include "..\Monster.h"
#include "..\StateMachinen.h"
#include "..\CharMove.h"
#include "..\..\4DyuchiGXGFunc\global.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* CloneWalk(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();
			LPCTSTR parameter2 = parameterContainer["parameter2"].c_str();

			const VECTOR3 position = {float(_tstof(parameter1)) * 100.0f, 0, float(_tstof(parameter2)) * 100.0f};

			return new CActionMove(
				position,
				eMA_SCRIPT_WALK);
		}

		const BOOL isRegisteredWalk = CMachine::GetParser().Register(
			"Walk",
			CloneWalk);

		CAction* CloneRun(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();
			LPCTSTR parameter2 = parameterContainer["parameter2"].c_str();

			const VECTOR3 position = {float(_tstof(parameter1)) * 100.0f, 0, float(_tstof(parameter2)) * 100.0f};

			return new CActionMove(
				position,
				eMA_SCRIPT_RUN);
		}

		const BOOL isRegisteredRun = CMachine::GetParser().Register(
			"Run",
			CloneRun);
	}

	CActionMove::CActionMove(const VECTOR3& position, eMONSTER_ACTION monsterAction) :
	CAction("Move"),
	mPosition(position),
	mMonsterAction(monsterAction)
	{}

	int CActionMove::Run(CMachine& machine) const
	{
		CMonster* const monsterObject = (CMonster*)g_pUserTable->FindUser(
			machine.GetObjectIndex());

		if(0 == monsterObject)
		{
			return 0;
		}
		else if(FALSE == (eObjectKind_Monster & monsterObject->GetObjectKind()))
		{
			return 0;
		}

		const StateParameter& stateParameter = monsterObject->mStateParamter;

		if(eMA_ATTACK == stateParameter.stateCur ||
			eMA_PERSUIT == stateParameter.stateCur)
		{
			return 0;
		}

		VECTOR3 targetPosition = mPosition;
		VECTOR3 currentPosition = {0};
		monsterObject->GetPosition(
			&currentPosition);
		const isOnTarget = (50.0f > CalcDistance(&currentPosition, &targetPosition));

		if(isOnTarget)
		{
			GSTATEMACHINE.SetState(
				monsterObject,
				eMA_STAND);
			return 1;
		}

		machine.GetMemory().SetVariable(
			"__move_x__",
			int(mPosition.x));
		machine.GetMemory().SetVariable(
			"__move_z__",
			int(mPosition.z));
		
		GSTATEMACHINE.SetState(
			monsterObject,
			mMonsterAction);
		return 0;
	}
}
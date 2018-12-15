#include "StdAfx.h"
#include "ActionRunaway.h"
#include "ActionPause.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"
#include "..\Monster.h"
#include "..\UserTable.h"
#include "..\StateMachinen.h"
#include "..\CharMove.h"
#include "..\..\4DyuchiGXGFunc\global.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* CloneRunaway(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();
			LPCTSTR parameter2 = parameterContainer["parameter2"].c_str();
			LPCTSTR parameter3 = parameterContainer["parameter3"].c_str();

			const VECTOR3 position = {float(_tstof(parameter1) * 100.0f), 0, float(_tstof(parameter2) * 100.0f)};
			const float bias = float(_tstof(parameter3));

			return new CActionRunaway(
				position,
				bias,
				eMA_RUNAWAY);
		}

		const BOOL isRegisteredRunaway = CMachine::GetParser().Register(
			"Runaway",
			CloneRunaway);

		CAction* CloneWalkaway(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();
			LPCTSTR parameter2 = parameterContainer["parameter2"].c_str();
			LPCTSTR parameter3 = parameterContainer["parameter3"].c_str();

			const VECTOR3 position = {float(_tstof(parameter1) * 100.0f), 0, float(_tstof(parameter2) * 100.0f)};
			const float bias = float(_tstof(parameter3));

			return new CActionRunaway(
				position,
				bias,
				eMA_WALKAWAY);
		}

		const BOOL isRegisteredWalkaway = CMachine::GetParser().Register(
			"Walkaway",
			CloneWalkaway);
	}

	CActionRunaway::CActionRunaway(const VECTOR3& position, float bias, eMONSTER_ACTION action) :
	CAction("Runaway"),
	mPosition(position),
	mBias(bias),
	mAction(action)
	{}

	int CActionRunaway::Run(CMachine& machine) const
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
		else if(mAction == monsterObject->mStateParamter.stateCur)
		{
			return 0;
		}

		VECTOR3 monsterPosition = {0};
		monsterObject->GetPosition(
			&monsterPosition);
		VECTOR3 runawayPosition = mPosition;

		const float distance = CalcDistance(
			&runawayPosition,
			&monsterPosition);

		const BOOL isRunawayPosition = (distance < mBias + 10.0f);

		if(isRunawayPosition)
		{
			return 1;
		}

		monsterObject->MoveStop();

		const float randomRate = float(rand()) / RAND_MAX;

		// 랜덤 방향 값을 가진 단위 벡터를 구한다
		VECTOR3 relatedPosition = {0};
		relatedPosition.x = -1 + randomRate * 2.0f;
		relatedPosition.z = sqrt(1.0f - pow(relatedPosition.z, 2.0f));
		relatedPosition.x *= mBias;
		relatedPosition.z *= mBias;

		runawayPosition = runawayPosition + relatedPosition;

		machine.GetMemory().SetVariable(
			"__runaway_x__",
			int(runawayPosition.x));
		machine.GetMemory().SetVariable(
			"__runaway_z__",
			int(runawayPosition.z));

		GSTATEMACHINE.SetState(
			monsterObject,
			mAction);
		return 0;
	}
}
#include "StdAfx.h"
#include "ActionPause.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"
#include "..\Monster.h"
#include "..\UserTable.h"
#include "..\StateMachinen.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();			

			return new CActionPause(
				_ttoi(parameter1) * 1000);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Pause",
			Clone);
	}

	CActionPause::CActionPause(DWORD tick) :
	CAction("Pause"),
	mPausedTick(tick)
	{}

	int CActionPause::Run(CMachine& machine) const
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

		machine.GetMemory().SetVariable(
			"__lastState__",
			monsterObject->mStateParamter.stateCur);
		machine.GetMemory().SetVariable(
			"__tick__",
			mPausedTick);
		GSTATEMACHINE.SetState(
			monsterObject,
			eMA_PAUSE);
		return 1;
	}
}
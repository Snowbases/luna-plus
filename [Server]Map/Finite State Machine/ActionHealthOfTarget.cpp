#include "StdAfx.h"
#include "ActionHealthOfTarget.h"
#include "Machine.h"
#include "Parser.h"
#include "..\Monster.h"
#include "..\UserTable.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			return new CActionHealthOfTarget;
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"HealthOfTarget",
			Clone);
	}

	CActionHealthOfTarget::CActionHealthOfTarget() :
	CAction("HealthOfTarget")
	{}

	int CActionHealthOfTarget::Run(CMachine& machine) const
	{
		CMonster* const monsterObject = (CMonster*)g_pUserTable->FindUser(
			machine.GetObjectIndex());

		if(0 == monsterObject)
		{
			return 0;
		}

		CObject* const targetObject = monsterObject->GetTObject();

		if(0 == targetObject)
		{
			return 0;
		}

		const float healthRate = float(targetObject->GetLife()) / targetObject->GetMaxLife();

		return int(healthRate * 100.0f);
	}
}
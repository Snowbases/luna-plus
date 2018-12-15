#include "StdAfx.h"
#include "ActionStop.h"
#include "Machine.h"
#include "Parser.h"
#include "..\Monster.h"
#include "..\StateMachinen.h"
#include "..\UserTable.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR channel = parameterContainer["channel"].c_str();
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			LPCTSTR alias = CMachine::GetParser().GetAlias(
				parameter1,
				_ttoi(channel));

			return new CActionStop(
				alias);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Stop",
			Clone);
	}

	CActionStop::CActionStop(LPCTSTR alias) :
	CAction("Stop"),
	mAlias(alias)
	{}

	int CActionStop::Run(CMachine& machine) const
	{
		CMonster* const monsterObject = (CMonster*)g_pUserTable->FindUser(
			machine.GetObjectIndex(mAlias.c_str()));

		if(0 == monsterObject)
		{
			return 0;
		}
		else if(FALSE == (eObjectKind_Monster & monsterObject->GetObjectKind()))
		{
			return 0;
		}

		GSTATEMACHINE.SetState(
			monsterObject,
			eMA_STAND);
		monsterObject->MoveStop();
		return 1;
	}
}
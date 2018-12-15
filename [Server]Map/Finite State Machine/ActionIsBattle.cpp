#include "StdAfx.h"
#include "ActionIsBattle.h"
#include "Machine.h"
#include "Parser.h"
#include "..\UserTable.h"
#include "..\Monster.h"

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

			return new CActionIsBattle(
				alias);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"IsBattle",
			Clone);
	}

	CActionIsBattle::CActionIsBattle(LPCTSTR alias) :
	CAction("IsBattle"),
	mAlias(alias)
	{}

	int CActionIsBattle::Run(CMachine& machine) const
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

		switch(monsterObject->mStateParamter.stateCur)
		{
		case eMA_ATTACK:
		case eMA_PERSUIT:
			{
				return 1;
			}
		}

		return 0;
	}
}
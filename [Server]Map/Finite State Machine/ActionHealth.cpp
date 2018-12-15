#include "StdAfx.h"
#include "ActionHealth.h"
#include "Machine.h"
#include "Parser.h"
#include "..\Object.h"
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

			return new CActionHealth(
				alias);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Health",
			Clone);
	}

	CActionHealth::CActionHealth(LPCTSTR alias) :
	CAction("Health"),
	mAlias(alias)
	{}

	int CActionHealth::Run(CMachine& machine) const
	{
		CObject* const object = g_pUserTable->FindUser(
			machine.GetObjectIndex(mAlias.c_str()));

		if(0 == object)
		{
			return 0;
		}

		const float healthRate = float(object->GetLife()) / float(object->GetMaxLife());

		return int(healthRate * 100.0f);
	}
}
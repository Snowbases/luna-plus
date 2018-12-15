#include "StdAfx.h"
#include "ActionAlias.h"
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

			return new CActionAlias(
				alias);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Alias",
			Clone);
	}

	CActionAlias::CActionAlias(LPCTSTR alias) :
	CAction("Alias"),
	mAlias(alias)
	{}

	int CActionAlias::Run(CMachine& machine) const
	{
		CObject* const object = g_pUserTable->FindUser(
			machine.GetObjectIndex());

		if(0 == object)
		{
			return 0;
		}

		g_pUserTable->SetObjectIndex(
			mAlias.c_str(),
			object->GetID());

		machine.SetAlias(
			mAlias.c_str());
		return 1;
	}
}
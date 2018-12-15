#include "StdAfx.h"
#include "ActionTeam.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"
#include "..\UserTable.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR channel =  parameterContainer["channel"].c_str();
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			LPCTSTR alias = CMachine::GetParser().GetAlias(
				parameter1,
				_ttoi(channel));

			return new CActionTeam(
				alias);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Team",
			Clone);
	}

	CActionTeam::CActionTeam(LPCTSTR teamName) :
	CAction("Team"),
	mTeamName(teamName)
	{}

	int CActionTeam::Run(CMachine& machine) const
	{
		g_pUserTable->AddTeamMember(
			mTeamName.c_str(),
			machine.GetObjectIndex());

		machine.GetMemory().SetTeam(
			mTeamName.c_str());
		return 1;
	}
}
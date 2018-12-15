#include "StdAfx.h"
#include "ActionTeamChief.h"
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
			LPCTSTR channel = parameterContainer["channel"].c_str();
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			LPCTSTR alias = CMachine::GetParser().GetAlias(
				parameter1,
				_ttoi(channel));

			return new CActionTeamChief(
				alias);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"TeamChief",
			Clone);
	}

	CActionTeamChief::CActionTeamChief(LPCTSTR teamName) :
	CAction("TeamChief"),
	mTeamName(teamName)
	{}

	int CActionTeamChief::Run(CMachine& machine) const
	{
		g_pUserTable->SetTeamLeader(
			mTeamName.c_str(),
			machine.GetObjectIndex());

		machine.GetMemory().SetTeam(
			mTeamName.c_str());
		return 1;
	}
}
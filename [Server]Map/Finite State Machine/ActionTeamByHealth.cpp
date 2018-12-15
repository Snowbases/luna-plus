#include "StdAfx.h"
#include "ActionTeamByHealth.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"
#include "..\UserTable.h"
#include "..\Monster.h"
#include "..\StateMachinen.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR channel = parameterContainer["channel"].c_str();
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();
			LPCTSTR parameter2 = parameterContainer["parameter2"].c_str();

			LPCTSTR alias = CMachine::GetParser().GetAlias(
				parameter1,
				_ttoi(channel));
			const float rate = float(_tstof(
				parameter2)) / 100.0f;

			return new CActionTeamByHealth(
				alias,
				rate);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"TeamByHealth",
			Clone);
	}

	CActionTeamByHealth::CActionTeamByHealth(LPCTSTR teamName, float minimumRate) :
	CAction("TeamByHealth"),
	mTeamName(teamName),
	mMinimumRate(minimumRate)
	{}

	int CActionTeamByHealth::Run(CMachine& machine) const
	{
		const CUserTable::Team& team = g_pUserTable->GetTeam(
			mTeamName.c_str());
		machine.GetMemory().GetFoundObject().clear();

		for(CUserTable::Team::ObjectIndexContainer::const_iterator iterator = team.mMemberContainer.begin();
			team.mMemberContainer.end() != iterator;
			++iterator)
		{
			const DWORD objectIndex = *iterator;

			if(machine.GetObjectIndex() == objectIndex)
			{
				continue;
			}

			CObject* const object = g_pUserTable->FindUser(
				objectIndex);

			if(0 == object)
			{
				continue;
			}

			const float rate = float(object->GetLife()) / object->GetMaxLife();

			if(mMinimumRate < rate)
			{
				continue;
			}

			machine.GetMemory().AddFoundObject(
				objectIndex);
			break;
		}

		return false == machine.GetMemory().GetFoundObject().empty();
	}
}
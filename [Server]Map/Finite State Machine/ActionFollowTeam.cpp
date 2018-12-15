#include "StdAfx.h"
#include "ActionFollowTeam.h"
#include "ActionFollow.h"
#include "Machine.h"
#include "Parser.h"
#include "..\UserTable.h"
#include "..\Object.h"


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
			const float distance = float(
				_tstof(parameter2));

			return new CActionFollowTeam(
				alias,
				distance);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"FollowTeam",
			Clone);
	}

	CActionFollowTeam::CActionFollowTeam(LPCTSTR teamName, float distance) :
	CAction("FollowTeam"),
	mTeamName(teamName),
	mDistance(distance)
	{}

	int CActionFollowTeam::Run(CMachine& machine) const
	{
		const CUserTable::Team& team = g_pUserTable->GetTeam(
			mTeamName.c_str());

		CObject* const object = g_pUserTable->FindUser(
			team.mLeaderObjectIndex);

		if(0 == object)
		{
			return 0;
		}

		CActionFollow action(
			object->GetID(),
			mDistance);
		return action.Run(
			machine);
	}
}
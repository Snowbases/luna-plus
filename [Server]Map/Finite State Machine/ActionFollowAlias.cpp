#include "StdAfx.h"
#include "ActionFollowAlias.h"
#include "ActionFollow.h"
#include "Machine.h"
#include "Parser.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR channel = parameterContainer["channel"].c_str();
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();
			LPCTSTR parameter2 = parameterContainer["parameter2"].c_str();

			const float distance = float(
				_tstof(parameter2));
			LPCTSTR alias = CMachine::GetParser().GetAlias(
				parameter1,
				_ttoi(channel));

			return new CActionFollowAlias(
				alias,
				distance);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Follow",
			Clone);
	}

	CActionFollowAlias::CActionFollowAlias(LPCTSTR alias, float distance) :
	CAction("Follow"),
	mAlias(alias),
	mDistance(distance)
	{}

	int CActionFollowAlias::Run(CMachine& machine) const
	{
		CActionFollow action(
			machine.GetObjectIndex(mAlias.c_str()),
			mDistance);
		action.Run(
			machine);
		return 1;
	}
}
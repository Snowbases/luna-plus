#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionFollowTeam : public CAction
	{
	private:
		const std::string mTeamName;
		const float mDistance;

	public:
		CActionFollowTeam(LPCTSTR teamName, float distance);
		virtual ~CActionFollowTeam() {}
		virtual int Run(CMachine&) const;
	};
}
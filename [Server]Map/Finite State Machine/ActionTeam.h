#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionTeam : public CAction
	{
	private:
		const std::string mTeamName;

	public:
		CActionTeam(LPCTSTR teamName);
		virtual ~CActionTeam() {}
		virtual int Run(CMachine&) const;
	};
}
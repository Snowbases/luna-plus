#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionTeamChief : public CAction
	{
	private:
		const std::string mTeamName;

	public:
		CActionTeamChief(LPCTSTR teamName);
		virtual ~CActionTeamChief() {}
		virtual int Run(CMachine&) const;
	};
}
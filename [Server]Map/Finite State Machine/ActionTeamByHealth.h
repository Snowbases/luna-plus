#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionTeamByHealth : public CAction
	{
	private:
		const std::string mTeamName;
		const float mMinimumRate;

	public:
		CActionTeamByHealth(LPCTSTR teamName, float minimumRate);
		virtual ~CActionTeamByHealth() {}
		virtual int Run(CMachine&) const;
	};
}
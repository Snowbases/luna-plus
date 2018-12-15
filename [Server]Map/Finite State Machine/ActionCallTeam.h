#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionCallTeam : public CAction
	{
	private:
		const std::string mTeamName;

	public:
		CActionCallTeam(LPCTSTR teamName);
		virtual ~CActionCallTeam() {}
		virtual int Run(CMachine&) const;
	};
}
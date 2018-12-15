#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionHealth : public CAction
	{
	private:
		const std::string mAlias;

	public:
		CActionHealth(LPCTSTR alias);
		virtual ~CActionHealth() {}
		virtual int Run(CMachine&) const;
	};
}
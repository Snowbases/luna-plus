#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionQuit : public CAction
	{
	public:
		CActionQuit();
		virtual ~CActionQuit() {}
		virtual int Run(CMachine&) const;
	};
}
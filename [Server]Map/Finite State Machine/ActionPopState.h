#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionPopState : public CAction
	{
	public:
		CActionPopState();
		virtual ~CActionPopState() {}
		virtual int Run(CMachine&) const;
	};
}
#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionHealthOfTarget : public CAction
	{
	public:
		CActionHealthOfTarget();
		virtual ~CActionHealthOfTarget() {}
		virtual int Run(CMachine&) const;
	};
}
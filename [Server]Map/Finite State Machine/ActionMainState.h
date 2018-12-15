#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionMainState : public CAction
	{
	private:
		const DWORD mStateIndex;

	public:
		CActionMainState(DWORD stateIndex);
		virtual ~CActionMainState() {}
		virtual int Run(CMachine&) const;
	};
}
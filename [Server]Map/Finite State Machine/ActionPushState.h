#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionPushState : public CAction
	{
	private:
		const DWORD mStateIndex;

	public:
		CActionPushState(DWORD stateIndex);
		virtual ~CActionPushState() {}
		virtual int Run(CMachine&) const;
	};
}
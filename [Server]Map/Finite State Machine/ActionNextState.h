#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionNextState : public CAction
	{
	private:
		const DWORD mNextState;
		const int mRandomValue;

	public:
		CActionNextState(DWORD nextState, int randomValue);
		virtual ~CActionNextState() {}
		virtual int Run(CMachine&) const;
	};
}
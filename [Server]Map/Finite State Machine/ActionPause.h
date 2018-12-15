#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionPause : public CAction
	{
	private:
		const DWORD mPausedTick;

	public:
		CActionPause(DWORD tick);
		virtual ~CActionPause() {}
		virtual int Run(CMachine&) const;
	};
}
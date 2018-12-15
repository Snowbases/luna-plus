#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionRandom : public CAction
	{
	private:
		const int mStart;
		const int mRange;

	public:
		CActionRandom(int start, int range);
		virtual ~CActionRandom() {}
		virtual int Run(CMachine&) const;
	};
}
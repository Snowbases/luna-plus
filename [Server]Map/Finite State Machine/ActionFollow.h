#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionFollow : public CAction
	{
	private:
		const DWORD mTargetObjectIndex;
		const float mDistance;

	public:
		CActionFollow(DWORD targetObjectIndex, float distance);
		virtual ~CActionFollow() {}
		virtual int Run(CMachine&) const;
	};
}
#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionRunaway : public CAction
	{
	private:
		const float mBias;
		const VECTOR3 mPosition;
		eMONSTER_ACTION mAction;
		
	public:
		CActionRunaway(const VECTOR3& position, float distance, eMONSTER_ACTION);
		virtual ~CActionRunaway() {}
		virtual int Run(CMachine&) const;
	};
}
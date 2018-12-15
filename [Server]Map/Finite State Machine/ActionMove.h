#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionMove : public CAction
	{
	private:
		const VECTOR3 mPosition;
		const eMONSTER_ACTION mMonsterAction;

	public:
		CActionMove(const VECTOR3&, eMONSTER_ACTION);
		virtual ~CActionMove() {}
		virtual int Run(CMachine&) const;
	};
}
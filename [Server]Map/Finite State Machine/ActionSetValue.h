#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionSetValue : public CAction
	{
	private:
		const std::string mLeftHandVariable;
		const std::string mRightHandVariable;

	public:
		CActionSetValue(LPCTSTR lhs, LPCTSTR rhs);
		virtual ~CActionSetValue() {}
		virtual int Run(CMachine&) const;
	};
}
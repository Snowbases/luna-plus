#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionFollowAlias : public CAction
	{
	private:
		const std::string mAlias;
		const float mDistance;

	public:
		CActionFollowAlias(LPCTSTR alias, float distance);
		virtual ~CActionFollowAlias() {}
		virtual int Run(CMachine&) const;
	};
}
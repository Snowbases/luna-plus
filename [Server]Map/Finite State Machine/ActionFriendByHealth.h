#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionFriendByHealth : public CAction
	{
		const std::string mAlias;
		const float mMinimumRate;

	public:
		CActionFriendByHealth(LPCTSTR alias, float rate);
		virtual ~CActionFriendByHealth() {}
		virtual int Run(CMachine&) const;
	};
}
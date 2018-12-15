#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionFriend : public CAction
	{
	private:
		const std::string mFriendName;

	public:
		CActionFriend(LPCTSTR friendName);
		virtual ~CActionFriend() {}
		virtual int Run(CMachine&) const;
	};
}
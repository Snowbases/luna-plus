#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionCallFriend : public CAction
	{
	private:
		const std::string mAlias;

	public:
		CActionCallFriend(LPCTSTR alias);
		virtual ~CActionCallFriend() {}
		virtual int Run(CMachine&) const;
	};
}
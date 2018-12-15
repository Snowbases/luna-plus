#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionDistance : public CAction
	{
		const std::string mFriendName;

	public:
		CActionDistance(LPCTSTR friendName);
		virtual ~CActionDistance() {}
		virtual int Run(CMachine&) const;
	};
}
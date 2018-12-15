#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionTalkToFriend : public CAction
	{
	private:
		const DWORD mSpeechIndex;

	public:
		CActionTalkToFriend(DWORD speechIndex);
		virtual ~CActionTalkToFriend() {}
		virtual int Run(CMachine&) const;
	};
}
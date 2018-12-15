#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionTalk : public CAction
	{
	protected:
		const DWORD mSpeechIndex;

	public:
		CActionTalk(DWORD speechIndex);
		virtual ~CActionTalk() {}
		virtual int Run(CMachine&) const;
	};
}
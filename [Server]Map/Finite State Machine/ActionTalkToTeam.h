#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionTalkToTeam : public CAction
	{
	private:
		const DWORD mSpeechIndex;

	public:
		CActionTalkToTeam(DWORD speechIndex);
		virtual ~CActionTalkToTeam() {}
		virtual int Run(CMachine&) const;
	};
}
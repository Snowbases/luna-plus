#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionListen : public CAction
	{
	private:
		const DWORD mSpeechIndex;

	public:
		CActionListen(DWORD speechIndex);
		virtual ~CActionListen() {}
		virtual int Run(CMachine&) const;
	};
}
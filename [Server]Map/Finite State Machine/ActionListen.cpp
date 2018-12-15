#include "StdAfx.h"
#include "ActionListen.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			return new CActionListen(
				_ttoi(parameter1));
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Listen",
			Clone);
	}

	CActionListen::CActionListen(DWORD speechIndex) :
	CAction("Listen"),
	mSpeechIndex(speechIndex)
	{}

	int CActionListen::Run(CMachine& machine) const
	{
		if(FALSE == machine.GetMemory().IsHeardSpeech(mSpeechIndex))
		{
			machine.GetMemory().SetResult(
				0);
			return 0;
		}

		machine.GetMemory().RemoveHeardSpeech(
			mSpeechIndex);
		return 1;
	}
}
#include "StdAfx.h"
#include "ActionNextState.h"
#include "ActionQuit.h"
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
			LPCTSTR parameter2 = parameterContainer["parameter2"].c_str();
			LPCTSTR fileName = parameterContainer["fileName"].c_str();

			if(0 == _tcsnicmp("0%", parameter2, _tcslen("0%")) ||
				0 == _tcslen(parameter2))
			{
				parameter2 = "100";
			}

			const DWORD stateIndex = CMachine::GetParser().GetHashCode(
				CMachine::GetParser().GetStateName(fileName, parameter1));
			const int rate = int(_tstof(parameter2) / 100 * RAND_MAX);

			return new CActionNextState(
				stateIndex,
				rate);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"NextState",
			Clone);
	}

	CActionNextState::CActionNextState(DWORD nextState, int randomValue) :
	CAction("NextState"),
	mNextState(nextState),
	mRandomValue(randomValue)
	{}

	int CActionNextState::Run(CMachine& machine) const
	{
		if(rand() < mRandomValue)
		{
			machine.GetMemory().SetNextState(
				mNextState);
			
			CActionQuit action;
			return action.Run(
				machine);
		}

		return 0;
	}
}
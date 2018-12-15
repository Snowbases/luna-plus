#include "StdAfx.h"
#include "ActionMainState.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR fileName = parameterContainer["fileName"].c_str();
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			LPCTSTR stateName = CMachine::GetParser().GetStateName(
				fileName,
				parameter1);
			const DWORD stateIndex = CMachine::GetParser().GetHashCode(
				stateName);

			return new CActionMainState(
				stateIndex);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"MainState",
			Clone);
	}

	CActionMainState::CActionMainState(DWORD stateIndex) :
	CAction("MainState"),
	mStateIndex(stateIndex)
	{}

	int CActionMainState::Run(CMachine& machine) const
	{
		machine.SetMainState(
			mStateIndex);
		machine.GetMemory().SetNextState(
			TRUE);
		return 1;
	}
}
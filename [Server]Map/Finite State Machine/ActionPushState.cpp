#include "StdAfx.h"
#include "ActionPushState.h"
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

			return new CActionPushState(
				stateIndex);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"PushState",
			Clone);
	}

	CActionPushState::CActionPushState(DWORD stateIndex) :
	CAction("PushState"),
	mStateIndex(stateIndex)
	{}

	int CActionPushState::Run(CMachine& machine) const
	{
		machine.PushState(
			mStateIndex);
		machine.GetMemory().SetNextState(
			TRUE);
		return 1;
	}
}
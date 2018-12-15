#include "StdAfx.h"
#include "ActionPopState.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			return new CActionPopState;
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"PopState",
			Clone);
	}

	CActionPopState::CActionPopState() :
	CAction("PopState")
	{}

	int CActionPopState::Run(CMachine& machine) const
	{
		machine.PopState();
		machine.GetMemory().SetNextState(
			TRUE);
		return 1;
	}
}
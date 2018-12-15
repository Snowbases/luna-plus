#include "StdAfx.h"
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
			return new CActionQuit;
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Quit",
			Clone);
	}

	CActionQuit::CActionQuit() :
	CAction("Quit")
	{}

	int CActionQuit::Run(CMachine& machine) const
	{
		machine.GetMemory().SetExitState(
			TRUE);
		return 1;
	}
}
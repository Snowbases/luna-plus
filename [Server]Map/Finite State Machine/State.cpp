#include "StdAfx.h"
#include "State.h"
#include "Machine.h"
#include "Action.h"
#include "Memory.h"
#include "Parser.h"

namespace FiniteStateMachine
{
	CState::CState(DWORD index, LPCTSTR name) :
	mName(name),
	mIndex(index)
	{}

	CState::~CState()
	{}

	void CState::Run(CMachine& machine) const
	{
		machine.GetMemory().SetNextState(
			0);
		machine.GetMemory().SetExitState(
			FALSE);

		for(ActionContainer::const_iterator iterator = mActionContainer.begin();
			mActionContainer.end() != iterator;
			++iterator)
		{
			const ActionIndex actionIndex = *iterator;
			machine.Log(
				mIndex,
				actionIndex);

			const CAction& action = machine.GetParser().GetAction(
				actionIndex);
			machine.GetMemory().SetResult(
				action.Run(machine));

			if(machine.GetMemory().IsExitState())
			{
				break;
			}
		}
	}
}
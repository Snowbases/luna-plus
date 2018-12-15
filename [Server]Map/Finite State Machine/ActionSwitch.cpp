#include "StdAfx.h"
#include "ActionSwitch.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR operation = parameterContainer["parameter1"].c_str();
			LPCTSTR actionIndex = parameterContainer["parameter2"].c_str();
			LPCTSTR variable = parameterContainer["parameter3"].c_str();
			LPCTSTR parameter4 = parameterContainer["parameter4"].c_str();

			CActionSwitch::IsValid isValid = CActionSwitch::IsNone;

			if(0 == _tcsicmp(parameter4, "<"))
			{
				isValid = CActionSwitch::IsLess;
			}
			else if(0 == _tcsicmp(parameter4, "<="))
			{
				isValid = CActionSwitch::IsLessEqual;
			}
			else if(0 == _tcsicmp(parameter4, ">"))
			{
				isValid = CActionSwitch::IsMore;
			}
			else if(0 == _tcsicmp(parameter4, ">="))
			{
				isValid = CActionSwitch::IsMoreEqual;
			}
			else if(0 == _tcsicmp(parameter4, "="))
			{
				isValid = CActionSwitch::IsEqual;
			}
			else if(0 == _tcsicmp(parameter4, "!="))
			{
				isValid = CActionSwitch::IsNoEqual;
			}

			return new CActionSwitch(
				isValid,
				_ttoi(operation),
				_ttoi(actionIndex),
				variable);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Switch",
			Clone);
	}

	CActionSwitch::CActionSwitch(CActionSwitch::IsValid isValid, int operation, DWORD actionIndex, LPCTSTR variable) :
	CAction("Switch"),
	mOperation(operation),
	mIsValid(isValid),
	mActionIndex(actionIndex),
	mVaraible(variable)
	{}

	int CActionSwitch::Run(CMachine& machine) const
	{
		const int variable = machine.GetMemory().GetVariable(
			mVaraible.c_str());

		if(FALSE == (*mIsValid)(
			variable,
			mOperation))
		{
			return 0;
		}

		const CAction& action = machine.GetParser().GetAction(
			mActionIndex);
		action.Run(
			machine);

		machine.GetMemory().SetExitState(
			TRUE);
		machine.Log(
			machine.GetMemory().GetCurrentState(),
			mActionIndex);
		return mActionIndex;
	}
}
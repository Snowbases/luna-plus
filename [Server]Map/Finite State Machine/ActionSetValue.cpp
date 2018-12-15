#include "StdAfx.h"
#include "ActionSetValue.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR leftHandName = parameterContainer["parameter1"].c_str();
			LPCTSTR rightHandName = parameterContainer["parameter2"].c_str();

			return new CActionSetValue(
				leftHandName,
				rightHandName);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"SetValue",
			Clone);
	}

	CActionSetValue::CActionSetValue(LPCTSTR lhs, LPCTSTR rhs) :
	CAction("SetValue"),
	mLeftHandVariable(lhs),
	mRightHandVariable(rhs)
	{}

	int CActionSetValue::Run(CMachine& machine) const
	{
		int value = 0;

		if(mRightHandVariable.empty())
		{
			value = machine.GetMemory().GetResult();
		}
		else
		{
			value = machine.GetMemory().GetVariable(
				mRightHandVariable.c_str());
		}

		machine.GetMemory().SetVariable(
			mLeftHandVariable.c_str(),
			value);
		return 1;
	}
}
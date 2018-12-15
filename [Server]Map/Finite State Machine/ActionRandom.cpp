#include "StdAfx.h"
#include "ActionRandom.h"
#include "Parser.h"
#include "Machine.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();
			LPCTSTR parameter2 = parameterContainer["parameter2"].c_str();

			const int beginValue = _ttoi(
				parameter1);
			const int endValue = _ttoi(
				parameter2);

			return new CActionRandom(
				beginValue,
				abs(endValue - beginValue) + 1);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Random",
			Clone);
	}

	CActionRandom::CActionRandom(int start, int range) :
	CAction("Random"),
	mStart(start),
	mRange(range)
	{}

	int CActionRandom::Run(CMachine&) const
	{
		const int random = mStart + rand() % mRange;

		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			"random: %d\n",
			random);
		OutputDebugString(
			text);

		return random;
	}
}
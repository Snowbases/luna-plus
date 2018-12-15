#include "StdAfx.h"
#include "ActionAggroBuff.h"
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

			const float aggroRate = _ttoi(parameter2) / 100.0f;

			return new CActionAggroBuff(
				parameter1,
				aggroRate);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"AggroBuff",
			Clone);
	}

	CActionAggroBuff::CActionAggroBuff(LPCTSTR filterText, float rate) :
	CAction("AggroBuff"),
	mFilterText(filterText),
	mAggroRate(rate)
	{}

	int CActionAggroBuff::Run(CMachine& machine) const
	{
		if(0 == _tcsicmp(mFilterText.c_str(), _T("all")))
		{
			for(int i = eStatusKind_Str; i < eStatusKind_Max; ++i)
			{
				machine.GetMemory().AddAggroRate(
					eStatusKind(i),
					mAggroRate);
			}
		}
		else
		{
			machine.GetMemory().AddAggroRate(
				eStatusKind(_ttoi(mFilterText.c_str())),
				mAggroRate);
		}

		return 1;
	}
}
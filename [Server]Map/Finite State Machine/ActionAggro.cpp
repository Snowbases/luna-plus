#include "StdAfx.h"
#include "ActionAggro.h"
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

			return new CActionAggro(
				parameter1,
				aggroRate);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Aggro",
			Clone);
	}

	CActionAggro::CActionAggro(LPCTSTR filterText, float rate) :
	CAction("Aggro"),
	mFilterText(filterText),
	mAggroRate(rate)
	{}

	int CActionAggro::Run(CMachine& machine) const
	{
		if(0 == _tcsicmp(mFilterText.c_str(), _T("all")))
		{
			for(int i = UNITKIND_PHYSIC_ATTCK; UNITKIND_MAX > i; ++i)
			{
				machine.GetMemory().AddAggroRate(
					UNITKIND(i),
					mAggroRate);
			}
		}
		else
		{
			machine.GetMemory().AddAggroRate(
				UNITKIND(_ttoi(mFilterText.c_str())),
				mAggroRate);
		}

		return 1;
	}
}
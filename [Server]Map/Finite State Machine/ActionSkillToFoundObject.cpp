#include "StdAfx.h"
#include "ActionSkillToFoundObject.h"
#include "ActionSkill.h"
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

			return new CActionSkillToFoundObject(
				_ttoi(parameter1));
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"SkillToFoundObject",
			Clone);
	}

	CActionSkillToFoundObject::CActionSkillToFoundObject(DWORD skillIndex) :
	CAction("SkillToFoundObject"),
	mSkillIndex(skillIndex)
	{}

	int CActionSkillToFoundObject::Run(CMachine& machine) const
	{
		CMemory::IndexContainer& friendContainer = machine.GetMemory().GetFoundObject();

		if(friendContainer.empty())
		{
			return 0;
		}

		const DWORD objectIndex = *(friendContainer.begin());

		CActionSkill actionSkill(
			objectIndex,
			mSkillIndex);
		return actionSkill.Run(
			machine);
	}
}
#include "StdAfx.h"
#include "ActionSkillToAlias.h"
#include "ActionSkill.h"
#include "Machine.h"
#include "Parser.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR channel = parameterContainer["channel"].c_str();
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();
			LPCTSTR parameter2 = parameterContainer["parameter2"].c_str();

			const DWORD skillIndex = _ttoi(
				parameter2);
			LPCTSTR alias = CMachine::GetParser().GetAlias(
				parameter1,
				_ttoi(channel));

			return new CActionSkillToAlias(
				alias,
				skillIndex);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Skill",
			Clone);
	}

	CActionSkillToAlias::CActionSkillToAlias(LPCTSTR alias, DWORD skillIndex) :
	CAction("Skill"),
	mAlias(alias),
	mSkillIndex(skillIndex)
	{}

	int CActionSkillToAlias::Run(CMachine& machine) const
	{
		CActionSkill actionSkill(
			machine.GetObjectIndex(mAlias.c_str()),
			mSkillIndex);
		actionSkill.Run(
			machine);
		return 1;
	}
}
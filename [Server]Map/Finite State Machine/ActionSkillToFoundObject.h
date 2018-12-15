#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionSkillToFoundObject : public CAction
	{
		const DWORD mSkillIndex;

	public:
		CActionSkillToFoundObject(DWORD skillIndex);
		virtual ~CActionSkillToFoundObject() {}
		virtual int Run(CMachine&) const;
	};
}
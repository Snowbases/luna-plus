#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionSkillToTarget : public CAction
	{
	private:
		const DWORD mSkillIndex;

	public:
		CActionSkillToTarget(DWORD skillIndex);
		virtual ~CActionSkillToTarget() {}
		virtual int Run(CMachine&) const;
	};
}
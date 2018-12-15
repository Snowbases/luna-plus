#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionSkill : public CAction
	{
	private:
		const DWORD mObjectIndex;
		const DWORD mSkillIndex;

	public:
		CActionSkill(DWORD objectIndex, DWORD skillIndex);
		virtual ~CActionSkill() {}
		virtual int Run(CMachine&) const;
	};
}
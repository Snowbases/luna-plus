#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionSkillToAlias : public CAction
	{
	private:
		const std::string mAlias;
		const DWORD mSkillIndex;

	public:
		CActionSkillToAlias(LPCTSTR alias, DWORD skillIndex);
		virtual ~CActionSkillToAlias() {}
		virtual int Run(CMachine&) const;
	};
}
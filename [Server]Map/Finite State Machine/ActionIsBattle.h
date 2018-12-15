#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionIsBattle : public CAction
	{
	private:
		const std::string mAlias;

	public:
		CActionIsBattle(LPCTSTR alias);
		virtual ~CActionIsBattle() {}
		virtual int Run(CMachine&) const;
	};
}
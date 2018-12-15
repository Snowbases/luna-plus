#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionStop : public CAction
	{
		const std::string mAlias;

	public:
		CActionStop(LPCTSTR alias);
		virtual ~CActionStop() {}
		virtual int Run(CMachine&) const;
	};
}
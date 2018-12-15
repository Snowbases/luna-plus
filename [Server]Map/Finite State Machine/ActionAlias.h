#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionAlias : public CAction
	{
	private:
		const std::string mAlias;

	public:
		CActionAlias(LPCTSTR);
		virtual ~CActionAlias() {}
		virtual int Run(CMachine&) const;
	};
}
#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionAggroBuff : public CAction
	{
	private:
		const std::string mFilterText;
		const float mAggroRate;

	public:
		CActionAggroBuff(LPCTSTR, float rate);
		virtual ~CActionAggroBuff() {}
		virtual int Run(CMachine&) const;
	};
}
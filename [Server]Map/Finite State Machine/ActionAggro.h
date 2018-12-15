#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionAggro : public CAction
	{
	private:
		const std::string mFilterText;
		const float mAggroRate;

	public:
		CActionAggro(LPCTSTR, float rate);
		virtual ~CActionAggro() {}
		virtual int Run(CMachine&) const;
	};
}
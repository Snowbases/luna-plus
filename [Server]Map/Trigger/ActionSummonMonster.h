#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionSummonMonster :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionSummonMonster;
		}
	};
}
#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionAddMoney :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionAddMoney;
		}
	};
}
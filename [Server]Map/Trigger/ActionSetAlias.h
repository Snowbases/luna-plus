#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionSetAlias :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionSetAlias;
		}
	};
}
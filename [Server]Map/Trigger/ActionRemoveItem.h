#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionRemoveItem :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionRemoveItem;
		}
	};
}
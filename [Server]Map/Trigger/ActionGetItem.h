#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionGetItem : 
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionGetItem;
		}
	};
}
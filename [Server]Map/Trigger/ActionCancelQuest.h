#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionCancelQuest :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionCancelQuest;
		}
	};
}
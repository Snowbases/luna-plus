#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionSetReviveFlag :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionSetReviveFlag;
		}
	};
}
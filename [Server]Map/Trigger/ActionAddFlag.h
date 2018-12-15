#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionAddFlag :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionAddFlag;
		}
	};
}
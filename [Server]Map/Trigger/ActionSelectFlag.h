#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionSelectFlag :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionSelectFlag;
		}
	};
}
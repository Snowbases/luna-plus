#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionRemoveBuff :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionRemoveBuff;
		}
	};
}
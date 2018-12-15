#pragma once
#include "Action.h"

namespace Trigger
{	
	class CActionsummonMonsterGroup :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionsummonMonsterGroup;
		}
	};
}
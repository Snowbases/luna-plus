#pragma once
#include "Action.h"

namespace Trigger
{

	class CActionTimerStart :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionTimerStart;
		}
	};

	class CActionTimerState :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionTimerState;
		}
	};
}

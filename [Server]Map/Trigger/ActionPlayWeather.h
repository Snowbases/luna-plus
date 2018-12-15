#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionPlayWeather :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionPlayWeather;
		}
	};
}
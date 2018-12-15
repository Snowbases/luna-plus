#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionPutNpcChat :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionPutNpcChat;
		}
	};
}
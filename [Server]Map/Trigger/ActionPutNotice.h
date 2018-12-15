#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionPutNotice :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionPutNotice;
		}
	};
}
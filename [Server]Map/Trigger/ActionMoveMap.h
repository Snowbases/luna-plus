#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionMoveMap :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionMoveMap;
		}
	};

	class CActionMoveObject :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionMoveObject;
		}
	};

	class CActionAddShockedTime :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionAddShockedTime;
		}
	};
}
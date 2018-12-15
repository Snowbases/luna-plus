#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionAddEffect :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionAddEffect;
		}
	};

	class CActionRemoveEffect :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionRemoveEffect;
		}
	};

	class CActionDropObject :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDropObject;
		}
	};
}

#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionBuff :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionBuff;
		}
	};

	
	class CActionActiveSkill : public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionActiveSkill;
		}
	};
}
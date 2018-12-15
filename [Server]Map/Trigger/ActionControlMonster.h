#pragma once
#include "Action.h"

class CMonster;

namespace Trigger
{
	class CActionControlMonster :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionControlMonster;
		}

	private:
		void UseSkill(CMonster&);
	};
}
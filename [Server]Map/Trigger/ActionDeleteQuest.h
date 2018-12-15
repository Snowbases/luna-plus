#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionDeleteQuest :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDeleteQuest;
		}
		void DeleteQuest(CPlayer&, DWORD mainQuestIndex, BOOL isForcedly);
	};
}
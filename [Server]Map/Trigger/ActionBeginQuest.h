#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionBeginQuest :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionBeginQuest;
		}
		void BeginQuest(CPlayer&, DWORD mainQuestIndex, DWORD subQuestIndex);
	};
}
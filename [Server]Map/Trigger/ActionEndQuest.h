#pragma once
#include "Action.h"

class CQuestInfo;

namespace Trigger
{
	class CActionEndQuest :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionEndQuest;
		}

		void EndQuest(CPlayer&, CQuestInfo&, DWORD mainQuestIndex, DWORD subQuestIndex);
	};
}
#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionCreateTrigger :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionCreateTrigger;
		}

	private:
		void CreateTrigger(eOwner, DWORD ownerIndex) const;
	};
}
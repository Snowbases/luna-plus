#include "StdAfx.h"
#include "ActionSetFlag.h"
#include "Manager.h"
#include "Trigger.h"

namespace Trigger
{
	void CActionSetFlag::DoAction()
	{
		CManager::GetInstance().SetFlag(
			GetTrigger().GetOwnerType(),
			GetTrigger().GetOwnerIndex(),
			GetValue(eProperty_Name),
			GetValue(eProperty_Value));
	}
}
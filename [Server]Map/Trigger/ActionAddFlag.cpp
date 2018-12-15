#include "StdAfx.h"
#include "ActionAddFlag.h"
#include "Manager.h"
#include "Trigger.h"

namespace Trigger
{
	void CActionAddFlag::DoAction()
	{
		const int flag = CManager::GetInstance().GetFlag(
			GetTrigger().GetOwnerType(),
			GetTrigger().GetOwnerIndex(),
			GetValue(eProperty_Name));
		const int variable = GetValue(
			eProperty_Value);

		CManager::GetInstance().SetFlag(
			GetTrigger().GetOwnerType(),
			GetTrigger().GetOwnerIndex(),
			GetValue(eProperty_Name),
			(INT_MAX == flag ? 0 : flag) + variable);
	}
}
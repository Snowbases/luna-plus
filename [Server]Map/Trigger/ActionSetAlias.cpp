#include "StdAfx.h"
#include "ActionSetAlias.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"

namespace Trigger
{
	void CActionSetAlias::DoAction()
	{
		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			const DWORD aliasValue = GetValue(eProperty_Alias);
			object->SetAlias(aliasValue);
		}
	}
}
#include "StdAfx.h"
#include "ActionKillMonster.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"

namespace Trigger
{
	void CActionKillMonster::DoAction()
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
			else if(FALSE == (object->GetObjectKind() & eObjectKind_Monster))
			{
				continue;
			}

			g_pServerSystem->RemoveMonster(object->GetID());
		}
	}
}
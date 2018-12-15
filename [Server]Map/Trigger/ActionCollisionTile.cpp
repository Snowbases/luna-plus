#include "StdAfx.h"
#include "ActionCollisionTile.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"

namespace Trigger
{
	void CActionCollisionTile::DoAction()
	{
		const DWORD hashCode = GetValue(eProperty_Name);
		const DWORD positionLeft = GetValue(eProperty_RectangleLeft) * 100;
		const DWORD positionTop = GetValue(eProperty_RectangleTop) * 100;
		const DWORD positionRight = GetValue(eProperty_RectangleRight) * 100;
		const DWORD positionBottom = GetValue(eProperty_RectangleBottom) * 100;
		const DWORD angle = GetValue(eProperty_Angle);
		const DWORD value = GetValue(eProperty_Value);

		MSG_NAME_DWORD5 message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_COLLISION_TILE_ACK;
		message.dwData1 = positionLeft;
		message.dwData2 = positionTop;
		message.dwData3 = positionRight;
		message.dwData4 = positionBottom;		
		message.dwData5 = MAKELPARAM(angle, value);
		SafeStrCpy(
			message.Name,
			GetAliasName(hashCode),
			sizeof(message.Name) / sizeof(*message.Name));
		
		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}
			else if(eObjectKind_Player != object->GetObjectKind())
			{
				continue;
			}
		
			object->SendMsg(
				&message,
				sizeof(message));
		}
	}
}
#include "StdAfx.h"
#include "ActionEffect.h"
#include "common.h"
#include "..\Object.h"
#include "..\Player.h"
#include "..\Vehicle.h"
#include "..\VehicleManager.h"
#include "..\UserTable.h"
#include "..\PackedData.h"
#include "..\CharMove.h"


namespace Trigger
{
	void CActionAddEffect::DoAction()
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

			MSG_DWORD5	msg;
			msg.Category = MP_TRIGGER;
			msg.Protocol = MP_TRIGGER_ADDEFFECT_ACK;
			msg.dwObjectID = object->GetID();
			msg.dwData1 = GetValue(eProperty_EffectDescNum);
			msg.dwData2 = GetValue(eProperty_EffectAddToMap);
			msg.dwData3 = GetValue(eProperty_PosX);
			msg.dwData4 = GetValue(eProperty_PosY);
			msg.dwData5 = GetValue(eProperty_PosZ);
			object->SendMsg(&msg, sizeof(msg));
		}
	}

	void CActionRemoveEffect::DoAction()
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

			MSG_DWORD2	msg;
			msg.Category = MP_TRIGGER;
			msg.Protocol = MP_TRIGGER_REMOVEEFFECT_ACK;
			msg.dwObjectID = object->GetID();
			msg.dwData1 = GetValue(eProperty_EffectDescNum);
			msg.dwData2 = GetValue(eProperty_EffectAddToMap);
			object->SendMsg(&msg, sizeof(msg));
		}
	}

	void CActionDropObject::DoAction()
	{
		const BOOL bDrop = GetValue(eProperty_Drop);

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const pObject = g_pUserTable->FindUser(objectIndex);

			if( pObject == NULL )
			{
				continue;
			}

			CCharMove::SetPosition( pObject, CCharMove::GetPosition( pObject ) );

			if( pObject->GetObjectKind() == eObjectKind_Player )
			{
				CPlayer* pPlayer = (CPlayer*)pObject;
				// 탈것에 탑승한 경우
				if( pPlayer->GetMountedVehicle() != 0 )
				{
					// 소유자인 경우 소환해제
					if( pPlayer->GetSummonedVehicle() == pPlayer->GetMountedVehicle() )
					{
						VEHICLEMGR->Unsummon( pPlayer->GetID(), FALSE );
					}
					// 내린다.
					else
					{
						VEHICLEMGR->Dismount( pPlayer->GetID(), FALSE );
					}
				}
			}

			if( bDrop )
			{
				MSG_INT2 msg;
				msg.Category = MP_TRIGGER;
				msg.Protocol = MP_TRIGGER_DROP_OBJECT_ON_ACK;
				msg.dwObjectID = pObject->GetID();
				msg.nData1 = GetValue(eProperty_GravityAcceleration);
				msg.nData2 = GetValue(eProperty_ElapsedSecond) * 1000;
				PACKEDDATA_OBJ->QuickSend(pObject,&msg,sizeof(msg));
			}
			else
			{
				MSGBASE msg;
				msg.Category = MP_TRIGGER;
				msg.Protocol = MP_TRIGGER_DROP_OBJECT_OFF_ACK;
				msg.dwObjectID = pObject->GetID();
				PACKEDDATA_OBJ->QuickSend(pObject,&msg,sizeof(msg));
			}
		}
	}
}
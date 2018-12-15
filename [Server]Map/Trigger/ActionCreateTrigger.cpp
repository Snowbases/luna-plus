#include "StdAfx.h"
#include "ActionCreateTrigger.h"
#include "common.h"
#include "Manager.h"
#include "Trigger.h"
#include "..\UserTable.h"
#include "..\Object.h"

namespace Trigger
{
	void CActionCreateTrigger::DoAction()
	{
		// 타겟이 없고 서버트리거일 경우 항상 맵 트리거로 생성된다
		if( (eTarget_None  == GetValue(eProperty_Target)) && 
			(eOwner_Server == mParameter.mOwnerType) )
		{
			CreateTrigger(
				eOwner_Server,
				mParameter.mChannelID);
			return;
		}

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			eOwner ownerType = eOwner_None;

			switch(object->GetObjectKind())
			{
			case eObjectKind_Player:
				{
					ownerType = eOwner_Player;
					break;
				}
			case eObjectKind_Monster:
			case eObjectKind_BossMonster:
			case eObjectKind_FieldBossMonster:
			case eObjectKind_FieldSubMonster:
				{
					ownerType = eOwner_Monster;
					break;
				}
			}

			CreateTrigger(
				ownerType,
				objectIndex);
		}
	}

	void CActionCreateTrigger::CreateTrigger(eOwner ownerType, DWORD ownerIndex) const
	{
		const CTrigger& parentTrigger = GetTrigger();
		const DWORD triggerName = GetValue(eProperty_Name);
		__time64_t time = 0;
		_time64(&time);
		struct tm regTime = *_localtime64(&time);

		// 같은 트리거일 경우 반복 회수를 상속해야한다
		CTrigger* const childTrigger = TRIGGERMGR->CreateTrigger(
			ownerType,
			ownerIndex,
			mParameter.mChannelID,
			triggerName,
			triggerName == parentTrigger.GetHashCode() ? parentTrigger.GetRepeatCount() : 0,
			regTime);

		if(0 == childTrigger)
		{
			PutLog(
				"CActionCreateTrigger is failed. parent: %s, child: %s",
				GetAliasName(parentTrigger.GetHashCode()),
				GetAliasName(triggerName));
			return;
		}
	}
}
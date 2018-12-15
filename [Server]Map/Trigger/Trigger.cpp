#include "StdAfx.h"
#include "common.h"
#include "Trigger.h"
#include "Manager.h"
#include "Message.h"
#include "Condition.h"
#include "ActionGetItem.h"
#include "..\UserTable.h"
#include "..\Object.h"
#include "..\GuildManager.h"
#include "..\PartyManager.h"
#include "..\Guild.h"
#include "..\Party.h"
#include "..\ChannelSystem.h"
#include "..\MapDBMsgParser.h"

namespace Trigger
{
	CTrigger::CTrigger() :
	mState(eState_Start)
	{
		ZeroMemory(&mParameter, sizeof(mParameter));
	}

	void CTrigger::ReleaseCondition()
	{
		for(ConditionMap::iterator iter = mConditionMap.begin();
			mConditionMap.end() != iter;
			++iter)
		{
			const CCondition* condition = iter->second;
			SAFE_DELETE(condition);
		}

		mConditionMap.clear();
	}

	void CTrigger::UpdateCondition()
	{
		const TriggerDesc& desc = TRIGGERMGR->GetTriggerDesc(GetHashCode());

		ReleaseCondition();
		
		for(TriggerDesc::IndexList::const_iterator iter = desc.mConditionList.begin();
			iter != desc.mConditionList.end(); 
			++iter)
		{
			const DWORD conditionDescIndex = *iter;

			CCondition::Parameter parameter;
			parameter.mChannelID = mParameter.mChannelID;
			parameter.mDescIndex = conditionDescIndex;
			parameter.mOwnerType = GetOwnerType();
			parameter.mOwnerIndex = GetOwnerIndex();
			parameter.mTriggerIndex = GetIndex();

			CCondition* const condition = new CCondition(parameter);
			mConditionMap[conditionDescIndex] = condition;
		}
	}

	CTrigger::~CTrigger(void)
	{
		ReleaseCondition();
	}

	void CTrigger::Process(const CMessage& message)
	{
		if(eState_Stop == GetState())
		{
			return;
		}
		else if(eState_Hold == GetState())
		{
			return;
		}
		else if(FailCondition(message))
		{
			return;
		}

		++mParameter.mRepeatCount;
		// 등록시각 갱신
		__time64_t time = 0;
		_time64(&time);
		mParameter.mRegTime = *_localtime64(&time);

		DoAction();
		SaveToDb();
		SetState(eState_Stop);

		if(IsLoop())
		{
			ResetCondition();
			PutLog("trigger [%s](%d) is restarted", GetName(), GetIndex());
			SetState(eState_Start);	
		}
	}

	void CTrigger::ResetCondition()
	{
		for(ConditionMap::const_iterator iter = mConditionMap.begin();
			iter != mConditionMap.end();
			++iter)
		{
			CCondition* const condition = iter->second;
			condition->Reset();
		}
	}

	BOOL CTrigger::IsLoop() const
	{
		const TriggerDesc& desc = TRIGGERMGR->GetTriggerDesc(GetHashCode());

		if(FALSE == desc.mLoop)
		{
			return FALSE;
		}

		const BOOL isInfiniteCount(0 == desc.mRepeatCount);

		if(isInfiniteCount)
		{
			return TRUE;
		}
		else if(desc.mRepeatCount > GetRepeatCount())
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOL CTrigger::FailCondition(const CMessage& message) const
	{
		DWORD successCount = 0;

		for(ConditionMap::const_iterator iter = mConditionMap.begin();
			iter != mConditionMap.end();
			++iter)
		{
			CCondition* const condition = iter->second;

			if(condition->Check(message))
			{
				++successCount;
			}
		}

		return successCount != mConditionMap.size();
	}

	void CTrigger::DoAction() const
	{
		PutLog(
			"%s(%d)'s action started",
			TRIGGERMGR->GetAliasName(GetHashCode()),
			GetIndex());

		const TriggerDesc& TriggerDesc = TRIGGERMGR->GetTriggerDesc(GetHashCode());

		for(TriggerDesc::IndexList::const_iterator iter = TriggerDesc.mActionList.begin();
			TriggerDesc.mActionList.end() != iter;
			++iter)
		{
			const DWORD actionDescIndex = *iter;
			const ActionDesc& actionDesc = TRIGGERMGR->GetActionDesc(actionDescIndex);

			CAction::Parameter parameter;
			parameter.mOwnerIndex = GetOwnerIndex();
			parameter.mChannelID = mParameter.mChannelID;
			parameter.mOwnerType = GetOwnerType();
			parameter.mDescIndex = actionDescIndex;
			parameter.mTriggerIndex = GetIndex();

			const eAction actionType = (eAction)actionDesc.GetValue(eProperty_ActionType);
			CAction* const action = TRIGGERMGR->CreateAction(actionType);

			if(0 == action)
			{
				continue;
			}

			action->SetParameter(parameter);
			action->DoAction();
		}
	}

	DWORD CTrigger::GetHashCode() const
	{
		return mParameter.mHashCode;
	}

	DWORD CTrigger::GetHashCode(DWORD aliasHashCode) const
	{
		LPCTSTR const aliasName = TRIGGERMGR->GetAliasName(aliasHashCode);
		// 오너 타입, 오너 번호, 채널로 소유주에 종속처리된다
		char buffer[MAX_PATH] = {0};
		sprintf(
			buffer,
			"%s.%d.%d.%d",
			aliasName,
			GetOwnerType(),
			GetOwnerIndex(),
			mParameter.mChannelID);
		return TRIGGERMGR->GetHashCode(buffer);
	}

	int CTrigger::GetActionValue(DWORD descIndex, eProperty property) const
	{
		const ActionDesc& desc = TRIGGERMGR->GetActionDesc(descIndex);
		const int value = desc.GetValue(property);

		switch(property)
		{
		case eProperty_GroupAlias:
		case eProperty_Alias:
		case eProperty_TimerName:
			{
				return value ? GetHashCode(value) : 0;
			}
		case eProperty_PosX:
		case eProperty_PosY:
		case eProperty_PosZ:
		case eProperty_RangeX:
		case eProperty_RangeZ:
			{
				return value * 100;
			}
		}

		return value;
	}

	DWORD CTrigger::GetSuccessObjectIndex(DWORD conditionHashcode) const
	{
		const ConditionMap::const_iterator iter = mConditionMap.find(conditionHashcode);

		if(mConditionMap.end() == iter)
		{
			return 0;
		}

		const CCondition* const condition = iter->second;

		return condition->GetSuccessObjectIndex();
	}

	void CTrigger::SaveToDb()
	{
		const TriggerDesc& desc = TRIGGERMGR->GetTriggerDesc(GetHashCode());

		// 몬스터 소유의 트리거는 처리할 필요가 없다
		if(eOwner_Monster == GetOwnerType())
		{
			return;
		}
		// 091029 LUJ, DB에 값을 저장하지 않는 트리거도 있다
		else if(FALSE == desc.mSaveToDb)
		{
			return;
		}

		g_DB.FreeMiddleQuery(
			0,
			0,
			"EXEC DBO.MP_TRIGGER_UPDATE %u, \'%s\', %d, %d, %d, %d",
			GetHashCode(),
			TRIGGERMGR->GetAliasName(GetHashCode()),
			GetOwnerType(),
			GetOwnerIndex(),
			GetRepeatCount(),
			g_pServerSystem->GetMapNum());
	}

	void CTrigger::LoadFromDb()
	{
		const TriggerDesc& desc = TRIGGERMGR->GetTriggerDesc(GetHashCode());

        // 메인 트리거는 이미 값을 읽은 상태이다
		if(eOwner_None != desc.mOwner)
		{
			return;
		}
		// 몬스터 소유의 트리거는 처리할 필요가 없다
		else if(eOwner_Monster == GetOwnerType())
		{
			return;
		}
		// 반복 회수가 이미 설정된 경우 읽어들일 필요가 없다
		else if(0 < GetRepeatCount())
		{
			return;
		}
		// 091029 LUJ, DB에 저장하지 않는 트리거는 값을 불러들일 필요도 없다
		else if(FALSE == desc.mSaveToDb)
		{
			return;
		}
		// 처음 실행된 자동시작 트리거는 기본 값을 저장한다.
		// 이래야 자동으로 재시작할 수 있다. 다음 턴에 재시작될 경우에는
		// 이미 반복 회수를 가지고 있으므로 저장할 필요가 없다.
		else if(desc.mAutoStart &&
			0 == GetRepeatCount())
		{
			SaveToDb();
			return;
		}

		g_DB.FreeMiddleQuery(
			RGetTriggerRepeatCount,
			GetIndex(),
			"EXEC DBO.MP_TRIGGER_SELECT_COUNT %u, %d, %d, %d",
			GetHashCode(),
			GetOwnerType(),
			GetOwnerIndex(),
			g_pServerSystem->GetMapNum());
		// DB로부터 값을 복구하기 전까지는 메시지 처리를 중지한다
		SetState(eState_Hold);
	}

	void CTrigger::RestoreRepeatCount(DWORD repeatCount)
	{
		const TriggerDesc& desc = TRIGGERMGR->GetTriggerDesc(GetHashCode());

		if( desc.mRepeatCount > 0 &&
			repeatCount == desc.mRepeatCount)
		{
			SetState(eState_Stop);
			return;
		}

		mParameter.mRepeatCount = repeatCount;
		SetState(eState_Start);
	}

	void CTrigger::SetRegTime(const struct tm& when )
	{
		mParameter.mRegTime = when;
	}

	void CTrigger::UpdateParamter(const CTrigger::Parameter& paramter)
	{
		mParameter = paramter;
		UpdateCondition();
		LoadFromDb();
	}
}
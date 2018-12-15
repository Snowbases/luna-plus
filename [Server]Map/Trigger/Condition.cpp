#include "StdAfx.h"
#include "common.h"
#include "Condition.h"
#include "Manager.h"
#include "CheckFunc.h"
#include "Trigger.h"
#include "../Object.h"
#include "../UserTable.h"

namespace Trigger
{
	CCondition::CCondition(const CCondition::Parameter& parameter):
	mParameter(parameter),
	mCheckCount(0),
	mType(eCheckType(GetValue(eProperty_CheckType))),
	mSuccessObjectIndex(0)
	{}

	CCondition::~CCondition(void)
	{}

	BOOL CCondition::IsInvalid(const CMessage& message) const
	{
		const ConditionDesc& desc = TRIGGERMGR->GetConditionDesc(GetParameter().mDescIndex);

		for(ConditionDesc::PropertyMap::const_iterator iter = desc.mPropertyMap.begin();
			iter != desc.mPropertyMap.end();
			++iter)
		{
			const stProperty& data = iter->second;
			const eProperty property = data.mType;
			const CheckFunc checkFunc = TRIGGERMGR->GetCheckFunc(property);

			if(FALSE == checkFunc(*this, message))
			{
				return TRUE;
			}
			else if(CheckFlag == checkFunc)
			{
				if(IsInvalidFlag(data))
				{
					return TRUE;
				}
			}
		}

		return FALSE;
	}

	BOOL CCondition::IsInvalidFlag(const stProperty& data) const
	{
		const int flag = CManager::GetInstance().GetFlag(
			mParameter.mOwnerType,
			mParameter.mOwnerIndex,
			data.mType);

		if(UINT_MAX == flag)
		{
			return TRUE;
		}

		return ! IsTrue(
			GetProperty(data.mType),
			flag);
	}

	BOOL CCondition::Check(const CMessage& message)
	{
		if(eCheckType_OneTime==mType && 0<mCheckCount)
			return TRUE;

		if(IsInvalid(message))
		{
			return FALSE;
		}
		else if(eObjectKind_None != message.GetValue(eProperty_ObjectKind))
		{
			mSuccessObjectIndex = message.GetValue(eProperty_ObjectIndex);
		}

		++mCheckCount;
		return IsValidCheckCount(mCheckCount);
	}

	BOOL CCondition::IsValidCheckCount(DWORD checkCount) const
	{
		const ConditionDesc& desc = TRIGGERMGR->GetConditionDesc(GetParameter().mDescIndex);
		const stProperty& data = desc.GetProperty(eProperty_CheckCount);

		// 반복 회수가 설정되지 않았으므로 검사를 완료할 수 있다
		if(0 == data.mValue)
		{
			return TRUE;
		}

		return IsTrue(data, checkCount);
	}

	void CCondition::Reset()
	{
		mCheckCount = 0;
		mSuccessObjectIndex = 0;
	}

	const CCondition::Parameter& CCondition::GetParameter() const
	{
		return mParameter;
	}

	const stProperty& CCondition::GetProperty(eProperty property) const
	{
		const ConditionDesc& desc = TRIGGERMGR->GetConditionDesc(mParameter.mDescIndex);
		return desc.GetProperty(property);
	}

	int CCondition::GetValue(eProperty property) const
	{
		const stProperty& data = GetProperty(property);
		return data.mValue;
	}

	const CTrigger& CCondition::GetTrigger() const
	{
		return TRIGGERMGR->GetTrigger(GetParameter().mTriggerIndex);
	}

	DWORD CCondition::GetChannelID() const
	{
		return mParameter.mChannelID;
	}
}

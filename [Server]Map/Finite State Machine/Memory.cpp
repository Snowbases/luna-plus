#include "StdAfx.h"
#include "Memory.h"

namespace FiniteStateMachine
{
	CMemory::CMemory() :
	mNextState(0),
	mIsExitState(TRUE),
	mFoundObjectIndex(0),
	mCurrnetState(0)
	{}

	CMemory::~CMemory()
	{}

	void CMemory::Initialize()
	{
		mFriendContainer.clear();
		mFoundObjectContainer.clear();
		mAggroRateBySkillKindContainer.clear();
	}

	void CMemory::Clear()
	{
		mCurrnetState = 0;
		mNextState = 0;
		mIsExitState = FALSE;
		mResult = 0;
	}

	void CMemory::SetVariable(LPCTSTR name, Value value)
	{
		mVariableContainer[name] = value;
	}

	CMemory::Value CMemory::GetVariable(LPCTSTR name) const
	{
		const VariableContainer::const_iterator iterator = mVariableContainer.find(
			name);

		if(mVariableContainer.end() == iterator)
		{
			OutputDebugString(
				"Value is not defined\n");
			return INT_MIN;
		}

		return iterator->second;
	}

	float CMemory::GetAggroRate(UNITKIND skillKind) const
	{
		const AggroRateBySkillKindContainer::const_iterator iterator = mAggroRateBySkillKindContainer.find(
			skillKind);

		return mAggroRateBySkillKindContainer.end() == iterator ? 0 : iterator->second;
	}

	float CMemory::GetAggroRate(eStatusKind statusKind) const
	{
		const AggroRateBySkillStatusContainer::const_iterator iterator = mAggroRateBySkillStatusContainer.find(
			statusKind);

		return mAggroRateBySkillStatusContainer.end() == iterator ? 0 : iterator->second;
	}
}

#include "StdAfx.h"
#include "AIGroupPrototype.h"
#include "RegenManager.h"
#include "AISystem.h"
#include "Monster.h"

CAIGroup::CAIGroup() :
m_dwGroupID(0),
m_dwGridID(0)
{}

CAIGroup::~CAIGroup()
{}

void CAIGroup::Die(DWORD id)
{
	ScriptIndexContainer::iterator iterator = mScriptIndexContainer.find(id);

	// 100104 LUJ, 고유 번호가 바닥나는 것을 막기 위해, 다른 형태의 고유 값인
	//			스크립트 번호를 사용한다. ScriptIndexContainer에는 [키]/[값]으로 각각
	//			[몬스터 고유 번호]/[스크립트 번호]가 설정되어 있다. 이 컨테이너에 
	//			값이 없는 것은 리젠 스크립트로 생성하지 않았다는 의미이다.
	if(mScriptIndexContainer.end() == iterator)
	{
		mRegenIndexContainer.insert(id);
		return;
	}

	const DWORD scriptIndex = iterator->second;
	mScriptIndexContainer.erase(iterator);

	mRegenIndexContainer.insert(scriptIndex);
}

void CAIGroup::RegenCheck()
{
	const float ratio = float(mScriptIndexContainer.size()) / mParameterContainer.size();

	for(ConditionContainer::iterator iterator = mConditionContainer.begin();
		mConditionContainer.end() != iterator;
		++iterator)
	{
		Condition& condition = *iterator;

		if(ratio > condition.mRatio)
		{
			continue;
		}

		condition.mStartTick = gCurTime + condition.mDelayTick;
		condition.mIsRegen = TRUE;
	}
}

void CAIGroup::AddRegenObject(EObjectKind objectKind, WORD monsterKind, const VECTOR3& position, LPCTSTR finiteStateMachine)
{
	const DWORD regenIndex = mParameterContainer.size();

	Parameter& regenObject = mParameterContainer[regenIndex];
	ZeroMemory(
		&regenObject,
		sizeof(regenObject));
	regenObject.ObjectKind = objectKind;
	regenObject.wMonsterKind = monsterKind;
	regenObject.vPos = position;
	SafeStrCpy(
		regenObject.mMachine,
		finiteStateMachine,
		_countof(regenObject.mMachine));

	Die(regenIndex);
}

void CAIGroup::AddCondition(DWORD targetGroup, float ratio, DWORD delayTick, BOOL isRegen, DWORD range)
{
	Condition condition = {0};
	condition.mTargetGroup = targetGroup;
	condition.mRatio = ratio;
	condition.mDelayTick = delayTick;
	condition.mStartTick = gCurTime + delayTick;
	condition.mIsRegen = isRegen;

	mConditionContainer.push_back(condition);
	m_dwRegenRange = range;
}

void CAIGroup::RegenProcess()
{
	for(ConditionContainer::iterator iterator = mConditionContainer.begin();
		mConditionContainer.end() != iterator;
		++iterator)
	{
		Condition& condition = *iterator;

		if(FALSE == condition.mIsRegen)
		{
			continue;
		}
		else if(condition.mStartTick > gCurTime)
		{
			continue;
		}

		condition.mStartTick = gCurTime + condition.mDelayTick;
		condition.mIsRegen = FALSE;

		REGENMGR->RegenGroup(
			condition.mTargetGroup,
			GetGridID());
	}
}

void CAIGroup::ForceRegen()
{
	IndexContainer failedIndexContainer;

	for(IndexContainer::const_iterator iterator = mRegenIndexContainer.begin();
		mRegenIndexContainer.end() != iterator;
		++iterator)
	{
		const DWORD regenIndex = *iterator;

		if(Create(regenIndex))
		{
			continue;
		}

		failedIndexContainer.insert(regenIndex);
	}

	mRegenIndexContainer = failedIndexContainer;

	if(false == mRegenIndexContainer.empty())
	{
		RegenCheck();
	}
}

CAIGroup::Parameter* CAIGroup::GetRegenObject(DWORD regenIndex)
{
	const ParameterContainer::iterator iterator = mParameterContainer.find(regenIndex);

	return mParameterContainer.end() == iterator ? 0 : &(iterator->second);
}

BOOL CAIGroup::Create(DWORD regenIndex)
{
	Parameter* const regenObject = GetRegenObject(regenIndex);

	if(0 == regenObject)
	{
		return FALSE;
	}

	CMonster* const monster = REGENMGR->RegenObject(
		g_pAISystem.GeneraterMonsterID(),
		regenIndex,
		GetGridID(),
		regenObject->ObjectKind,
		regenObject->wMonsterKind,
		&(regenObject->vPos),
		GetGroupID(),
		0,
		100,
		TRUE);

	if(0 == monster)
	{
		return FALSE;
	}

	mScriptIndexContainer.insert(
		std::make_pair(monster->GetID(), regenIndex));
	return TRUE;
}

void CAIGroup::SetRegenDelayTime(DWORD delayTime)
{
	for(ConditionContainer::iterator iterator = mConditionContainer.begin();
		mConditionContainer.end() != iterator;
		++iterator)
	{
		Condition& condition = *iterator;
		condition.mDelayTick = delayTime;
	}
}
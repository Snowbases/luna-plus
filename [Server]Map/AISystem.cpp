#include "StdAfx.h"
#include "AISystem.h"
#include "UserTable.h"
#include "Monster.h"
#include "MsgRouter.h"
#include "MHFile.h"
#include "Player.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "ChannelSystem.h"
#include "RegenManager.h"
#include "FieldBossMonsterManager.h"
#include "AIGroupManager.h"
#include "StateMachinen.h"

CAISystem g_pAISystem;

CAISystem::CAISystem() :
m_pROUTER(new CMsgRouter)
{
	m_AISubordinatedObject.Initialize(500);
	
	m_MonsterIDGenerator.Init(
		MONSTER_INDEX_SIZE,
		MONSTER_STARTINDEX);
}

CAISystem::~CAISystem()
{
	SAFE_DELETE(m_pROUTER);
	m_MonsterIDGenerator.Release();
}

void CAISystem::RemoveAllList()
{
	m_AISubordinatedObject.RemoveAll();
}

void CAISystem::Process()
{
	CObject * pObj= NULL;
	m_AISubordinatedObject.SetPositionHead();
	while((pObj = m_AISubordinatedObject.GetData())!= NULL)
	{
		ConstantProcess(pObj);
	}

	m_pROUTER->MsgLoop();

	static DWORD dwRegenCheckTime = 0;

	if(gCurTime > dwRegenCheckTime)
	{
		GROUPMGR->RegenProcess();
		dwRegenCheckTime = gCurTime + 3000;
	}
}
void CAISystem::ConstantProcess(CObject * obj)
{	
	//보스몹 스테이트는 따로 처리
	if(obj->GetObjectKind() == eObjectKind_BossMonster)
		return;

	GSTATEMACHINE.Process(
		obj,
		eSEVENT_Process);
}

void CAISystem::AddObject(CObject * obj)
{	
	m_AISubordinatedObject.Add(obj, obj->GetID());
}
CObject * CAISystem::RemoveObject(DWORD dwID)
{
	CObject *outObj = m_AISubordinatedObject.GetData(dwID);
	if(!outObj)
		return NULL;
	m_AISubordinatedObject.Remove(dwID);

	CAIGroup * pGroup = GROUPMGR->GetGroup(((CMonster*)outObj)->GetMonsterGroupNum(), ((CMonster*)outObj)->GetGridID());
	if(pGroup)
	{
		pGroup->Die(outObj->GetID());
		pGroup->RegenCheck();
	}
	
	return outObj;
}

void CAISystem::SendMsg(WORD msgID, DWORD src, DWORD dest, DWORD delay, DWORD flag)
{
	m_pROUTER->SendMsg(msgID, src, dest, delay, flag);
}

DWORD CAISystem::GeneraterMonsterID()
{
	return m_MonsterIDGenerator.GenerateIndex();
}
void CAISystem::ReleaseMonsterID(DWORD id)
{
	m_MonsterIDGenerator.ReleaseIndex(id);
}

void CAISystem::LoadAIGroupList()
{
	TCHAR path[MAX_PATH] = {0};
	_stprintf(
		path,
		"System/Resource/Monster_%02d.bin",
		g_pServerSystem->GetMapNum());
	Load(path);
	SummonOnAllChannel();
}

void CAISystem::Load(LPCTSTR fileName)
{
	// 091106 LUJ, 저장된 스크립트를 초기화해야한다
	mScriptMap.clear();
	mScriptUniqueMap.clear();

	CMHFile file;
	file.Init(
		LPTSTR(fileName),
		"rb");

	const DWORD oneMinute = 60 * 1000;
	DWORD groupIndex = 0;

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));
		LPCTSTR seperator = " \t{}$#";
		LPCTSTR token = _tcstok(buffer, seperator);

		if(0 == token)
		{
			continue;
		}
		else if('@' == token[0])
		{
			continue;
		}
		else if(0 == _tcsicmp("group", token))
		{
			LPCTSTR indexText = _tcstok(0, seperator);
			groupIndex = _ttoi(indexText ? indexText : "");
			Script& script = mScriptMap[groupIndex];
			script.mGroupIndex = groupIndex;
		}
		else if(0 == _tcsicmp("unique", token))
		{
			LPCTSTR indexText = _tcstok(0, seperator);
			groupIndex = _ttoi(indexText ? indexText : "");
			Script& script = mScriptUniqueMap[groupIndex];
			script.mGroupIndex = groupIndex;
		}
		else if(0 == _tcsicmp("addCondition", token))
		{
			LPCTSTR targetGroupText = _tcstok(0, seperator);
			LPCTSTR ratioText = _tcstok(0, seperator);
			LPCTSTR delayText = _tcstok(0, seperator);
			LPCTSTR regenText = _tcstok(0, seperator);
			LPCTSTR rangeText = _tcstok(0, seperator);

			Script::Condition condition;
			ZeroMemory(&condition, sizeof(condition));
			condition.mGroupIndex = _ttoi(targetGroupText ? targetGroupText : "");
			condition.mRatio = float(_tstof(ratioText ? ratioText : ""));
			condition.mDelay = _ttoi(delayText ? delayText : "");
			condition.mIsRegen = _ttoi(regenText ? regenText : "");
			// 100125 LUJ, 리젠 범위가 0으로 설정되면 리젠 위치를 설정할 때 오류가 발생한다
			condition.mRange = max(1, _ttoi(rangeText ? rangeText : ""));

			Script& script = mScriptMap[groupIndex];
			script.mConditionList.push_back(condition);
		}
		else if(0 == _tcsicmp("uniqueAddCondition", token))
		{
			LPCTSTR targetGroupText = _tcstok(0, seperator);
			LPCTSTR ratioText = _tcstok(0, seperator);
			LPCTSTR delayText = _tcstok(0, seperator);
			LPCTSTR regenText = _tcstok(0, seperator);
			LPCTSTR rangeText = _tcstok(0, seperator);

			Script::Condition condition;
			ZeroMemory(&condition, sizeof(condition));
			condition.mGroupIndex = _ttoi(targetGroupText ? targetGroupText : "");
			condition.mRatio = float(_tstof(ratioText ? ratioText : ""));
			condition.mDelay = _ttoi(delayText ? delayText : "");
			condition.mIsRegen = _ttoi(regenText ? regenText : "");
			// 100125 LUJ, 리젠 범위가 0으로 설정되면 리젠 위치를 설정할 때 오류가 발생한다
			condition.mRange = max(1, _ttoi(rangeText ? rangeText : ""));

			Script& script = mScriptUniqueMap[groupIndex];
			script.mConditionList.push_back(condition);
		}
		else if(0 == _tcsicmp("add", token))
		{
			LPCTSTR objectKindText = _tcstok(0, seperator);
			_tcstok(0, seperator);
			LPCTSTR monsterKindText = _tcstok(0, seperator);
			LPCTSTR xText = _tcstok(0, seperator);
			LPCTSTR zText = _tcstok(0, seperator);
			LPCTSTR finiteStateMachine = _tcstok(0, seperator);

			Script& script = mScriptMap[groupIndex];

			Script::Monster monster;
			ZeroMemory(&monster, sizeof(monster));
			monster.mObjectKind = EObjectKind(_ttoi(objectKindText ? objectKindText : ""));
			monster.mMonsterKind = WORD(_ttoi(monsterKindText ? monsterKindText : ""));
			monster.mPosition.x = float(_tstof(xText ? xText : ""));
			monster.mPosition.z = float(_tstof(zText ? zText : ""));
			SafeStrCpy(
				monster.mMachine,
				finiteStateMachine ? finiteStateMachine : "",
				_countof(monster.mMachine));

			script.mMonsterList.push_back(monster);
		}
		else if(0 == _tcsicmp("uniqueAdd", token))
		{
			LPCTSTR objectKindText = _tcstok(0, seperator);
			_tcstok(0, seperator);
			LPCTSTR monsterKindText = _tcstok(0, seperator);
			LPCTSTR xText = _tcstok(0, seperator);
			LPCTSTR zText = _tcstok(0, seperator);
			LPCTSTR finiteStateMachine = _tcstok(0, seperator);

			Script& script = mScriptUniqueMap[groupIndex];

			Script::Monster monster;
			ZeroMemory(&monster, sizeof(monster));
			monster.mObjectKind = EObjectKind(_ttoi(objectKindText ? objectKindText : ""));
			monster.mMonsterKind = WORD(_ttoi(monsterKindText ? monsterKindText : ""));
			monster.mPosition.x = float(_tstof(xText ? xText : ""));
			monster.mPosition.z = float(_tstof(zText ? zText : ""));
			SafeStrCpy(
				monster.mMachine,
				finiteStateMachine ? finiteStateMachine : "",
				_countof(monster.mMachine));

			script.mMonsterList.push_back(monster);
		}
		else if(0 == _tcsicmp("fieldBossRegenPosition", token))
		{
			LPCTSTR xText = _tcstok(0, seperator);
			LPCTSTR zText = _tcstok(0, seperator);

			VECTOR3 position = {0};
			position.x = float(_tstof(xText ? xText : ""));
			position.z = float(_tstof(zText ? zText : ""));

			Script& script = mScriptMap[groupIndex];
			script.mFieldBossPositionList.push_back(position);
		}
		else if(0 == _tcsicmp("uniqueRegenDelay", token))
		{
			LPCTSTR secondText = _tcstok(0, seperator);
			const DWORD tickCount = _ttoi(secondText ? secondText : "");

			Script::Delay delay = {0};
			delay.mMaxTick = tickCount * oneMinute;
			delay.mMinTick = tickCount * oneMinute;

			Script& script = mScriptUniqueMap[groupIndex];
			script.mDelayList.push_back(delay);
		}
		else if(0 == _tcsicmp("randomRegenDelay", token))
		{
			LPCTSTR minSecondText = _tcstok(0, seperator);
			LPCTSTR maxSecondText = _tcstok(0, seperator);

			const DWORD minSecond = _ttoi(minSecondText ? minSecondText : "");
			const DWORD maxSecond = _ttoi(maxSecondText ? maxSecondText : "");

			Script::Delay delay = {0};
			delay.mMinTick = minSecond * oneMinute;
			delay.mMaxTick = maxSecond * oneMinute;

			Script& script = mScriptMap[groupIndex];
			script.mDelayList.push_back(delay);
		}
		else if(0 == _tcsicmp("uniqueRandomRegenDelay", token))
		{
			LPCTSTR minSecondText = _tcstok(0, seperator);
			_tcstok(0, seperator);

			const DWORD minSecond = _ttoi(minSecondText ? minSecondText : "");

			Script::Delay delay = {0};
			delay.mMinTick = oneMinute;
			delay.mMaxTick = minSecond * oneMinute;

			Script& script = mScriptUniqueMap[groupIndex];
			script.mDelayList.push_back(delay);
		}
		else if(0 == _tcsicmp("uniqueRandomRegenDelay2", token))
		{
			LPCTSTR minSecondText = _tcstok(0, seperator);
			LPCTSTR maxSecondText = _tcstok(0, seperator);

			const DWORD minSecond = _ttoi(minSecondText ? minSecondText : "");
			const DWORD maxSecond = _ttoi(maxSecondText ? maxSecondText : "");

			Script::Delay delay = {0};
			delay.mMinTick = minSecond * oneMinute;
			delay.mMaxTick = maxSecond * oneMinute;

			Script& script = mScriptUniqueMap[groupIndex];
			script.mDelayList.push_back(delay);
		}
	}
}

void CAISystem::AddCondition(const CAISystem::Script& script, CAIGroup& aiGroup) const
{
	const Script::ConditionList& conditionList = script.mConditionList;

	for(Script::ConditionList::const_iterator conditionIterator = conditionList.begin();
		conditionList.end() != conditionIterator;
		++conditionIterator)
	{
		const Script::Condition& condition = *conditionIterator;
		
		aiGroup.AddCondition(
			condition.mGroupIndex,
			condition.mRatio,
			condition.mDelay,
			condition.mIsRegen,
			condition.mRange);
	}
}

void CAISystem::AddMonster(const CAISystem::Script& script, CAIGroup& aiGroup) const
{
	const Script::MonsterList& monsterList = script.mMonsterList;

	for(Script::MonsterList::const_iterator monsterIterator = monsterList.begin();
		monsterList.end() != monsterIterator;
		++monsterIterator)
	{
		const Script::Monster& monster = *monsterIterator;

		aiGroup.AddRegenObject(
			monster.mObjectKind,
			monster.mMonsterKind,
			monster.mPosition,
			monster.mMachine);
	}
}

void CAISystem::AddFieldBossPosition(const Script& script) const
{
	const Script::FieldBossPositionList& positionList = script.mFieldBossPositionList;

	for(Script::FieldBossPositionList::const_iterator iterator = positionList.begin();
        positionList.end() != iterator;
        ++iterator)
	{
		const VECTOR3& position = *iterator;
		FIELDBOSSMONMGR->AddRegenPosition(
			position.x,
			position.z);
	}
}

void CAISystem::SetRegenDelay(const CAISystem::Script& script, CAIGroup& aiGroup) const
{
	const Script::DelayList& delayList = script.mDelayList;

	for(Script::DelayList::const_iterator iterator = delayList.begin();
		delayList.end() != iterator;
		++iterator)
	{
		const Script::Delay& delay = *iterator;

		const DWORD randomTime = random(delay.mMinTick, delay.mMaxTick);
		aiGroup.SetRegenDelayTime(randomTime);
	}
}

void CAISystem::Summon(DWORD gridIndex)
{
	Summon(
		mScriptMap,
		gridIndex);
}

void CAISystem::Summon(const ScriptMap& scriptMap, DWORD gridIndex)
{
	for(ScriptMap::const_iterator scriptIterator = scriptMap.begin();
		scriptMap.end() != scriptIterator;
		++scriptIterator)
	{
		const Script& script = scriptIterator->second;

		CAIGroup& aiGroup = GROUPMGR->AddGroup(
			script.mGroupIndex,
			gridIndex);
		AddCondition(
			script,
			aiGroup);
		AddMonster(
			script,
			aiGroup);
		AddFieldBossPosition(
			script);
		SetRegenDelay(
			script,
			aiGroup);
	}
}

void CAISystem::SummonOnAllChannel()
{
	for(DWORD channel = 0; channel < CHANNELSYSTEM->GetChannelCount(); ++channel)
	{
		const DWORD gridIndex = CHANNELSYSTEM->GetChannelID(channel);

		Summon(
			mScriptMap,
			gridIndex);
	}

	// 091106 LUJ, 전 채널에 유일하게 생성되는 존재들을 어떤 채널에 생성하고 재생성 요청이
	//		올 경우 판단이 애매하기 때문에, 전 채널 소환 경우에만 유니크 소환을 사용한다
	// 100111 LUJ, 데이트 매칭 등은 전혀 다른 채널 정보를 사용한다. 그래서 채널 개수를
	//		카운팅하면 0이 반환된다
	if(0 < CHANNELSYSTEM->GetChannelCount())
	{
		const DWORD channel = rand() % max(1, CHANNELSYSTEM->GetChannelCount());
		const DWORD gridIndex = CHANNELSYSTEM->GetChannelID( channel );
		Summon(
			mScriptUniqueMap,
			gridIndex);
	}
}
#pragma once

class CObject;
class CMsgRouter;
class CAIGroup;
enum EObjectKind;

class CAISystem  
{
	struct Script
	{
		DWORD mGroupIndex;
		struct Condition
		{
			DWORD mGroupIndex;
			float mRatio;
			DWORD mDelay;
			BOOL mIsRegen;
			DWORD mRange;
		};
		typedef std::list< Condition > ConditionList;
		ConditionList mConditionList;

		struct Monster
		{
			EObjectKind mObjectKind;
			WORD mMonsterKind;
			VECTOR3 mPosition;
			TCHAR mMachine[MAX_PATH];
		};
		typedef std::list< Monster > MonsterList;
		MonsterList mMonsterList;

		typedef std::list< DWORD > UniqueGroupIndexList;
		UniqueGroupIndexList mUniqueGroupIndexList;
		typedef std::list< VECTOR3 > FieldBossPositionList;
		FieldBossPositionList mFieldBossPositionList;

		struct Delay
		{
			DWORD mMinTick;
			DWORD mMaxTick;
		};
		typedef std::list< Delay > DelayList;
		DelayList mDelayList;

		Script() :
		mGroupIndex(0)
		{}
	};
	// 091106 LUJ, 키: 그룹 번호
	typedef std::map< DWORD, Script > ScriptMap;
	ScriptMap mScriptMap;
	ScriptMap mScriptUniqueMap;
	CYHHashTable<CObject> m_AISubordinatedObject;
	CMsgRouter * m_pROUTER;
	CIndexGenerator m_MonsterIDGenerator;

public:
	CAISystem();
	virtual ~CAISystem();
	void Process();
	void ConstantProcess(CObject*);
	void AddObject(CObject*);
	CObject * RemoveObject(DWORD dwID);
	void SendMsg(WORD msgID, DWORD src, DWORD dest, DWORD delay, DWORD flag);
	DWORD GeneraterMonsterID();
	void ReleaseMonsterID(DWORD id);
	void LoadAIGroupList();
	void RemoveAllList();
	void Load(LPCTSTR fileName);
	void Summon(DWORD channelIndex);

protected:
	void Summon(const ScriptMap&, DWORD gridIndex);
	void Unsummon(DWORD groupIndex, DWORD gridIndex);
	void SummonOnAllChannel();
	void AddCondition(const Script&, CAIGroup&) const;
	void AddMonster(const Script&, CAIGroup&) const;
	void AddFieldBossPosition(const Script&) const;
	void SetRegenDelay(const Script&, CAIGroup&) const;
};

extern CAISystem g_pAISystem;
#pragma once

namespace Trigger
{
	enum eProperty
	{
		eProperty_None,
		eProperty_ChannelID,
		eProperty_Event,
		eProperty_ObjectIndex,	// Unique Index
		eProperty_ObjectKind,
		eProperty_Owner,
		eProperty_MapIndex,
		eProperty_PosX,			// 위치검사
		eProperty_PosY,			// 위치검사
		eProperty_PosZ,			// 위치검사
		eProperty_MoveX,		// 이동검사
		eProperty_MoveZ,		// 이동검사
		eProperty_RangeX,		// 영역검사
		eProperty_RangeZ,		// 영역검사
		eProperty_Range,
		eProperty_ItemIndex,
		eProperty_ItemQuantity,
		eProperty_ItemDropRate,
		eProperty_DamageValue,
		eProperty_Health,
		eProperty_Mana,
		eProperty_CreateTrigger,
		eProperty_StopTrigger,
		eProperty_Repeat,
		eProperty_ActionType,
		eProperty_MonsterKind,
		eProperty_Alias,
		eProperty_GroupAlias,
		eProperty_ElapsedDay,
		eProperty_ByMidnight,
		eProperty_ElapsedSecond,
		eProperty_PlayerCountInChannel,
		eProperty_CheckType,
		eProperty_Text,
		eProperty_WhatDay,
		eProperty_EffectDescNum,
		eProperty_EffectAddToMap,
		eProperty_SkillIndex,
		eProperty_ConditionAlias,
		eProperty_Sound,
		eProperty_BGM,
		eProperty_Target,
		eProperty_AutoStart,
		eProperty_Money,
		eProperty_MonsterSpeech,
		eProperty_CheckCount,
		eProperty_Name,
		eProperty_Quest,
		eProperty_QuestSub,
		eProperty_QuestState,
		eProperty_IsForcedly,
		eProperty_AttackerObjectIndex,
		eProperty_AttackerObjectKind,
		eProperty_VictimObjectKind,
		eProperty_VictimMonsterKind,
		eProperty_VictimMonsterAlias,
		eProperty_VictimMonsterGroupAlias,
		eProperty_LoginGuildMember,
		eProperty_GuildRank,
		eProperty_PartySize,
		// properties for Instance Dungeon(ID)
		eProperty_Dungeon_SwitchOn,
		eProperty_Dungeon_SwitchOff,
		eProperty_Dungeon_SwitchToggle,
		eProperty_Dungeon_WarpOn,
		eProperty_Dungeon_WarpOff,
		eProperty_Dungeon_Difficulty,
		eProperty_Dungeon_CheckPoint,
		eProperty_Dungeon_AddPoint,
		eProperty_Dungeon_CheckPlayerNum,
		// Trigger Property for Camera 
		eProperty_CameraShakeNum,
		eProperty_CameraShakeAmp,
		eProperty_CameraFilterIndex,
		eProperty_CameraFilterNoRepeat,
		eProperty_CameraFilterFadeIn,
		eProperty_CameraFilterFadeOut,
		eProperty_CameraFilterFadeTime,
		eProperty_CameraShakeTimes,
		// Trigger Property for NPC	
		eProperty_ActiveNpcId,
		eProperty_Loop,
		eProperty_SaveToDb,
		eProperty_ObjectState,
		eProperty_Value,
		// Trigger Property for Timer
		eProperty_TimerElapsedSecond,
		eProperty_TimerDuration,
		eProperty_TimerName,
		eProperty_TimerState,
		eProperty_TimerType,
		eProperty_ControlType,
		eProperty_ReviveFlag,
		eProperty_WeatherState,
		eProperty_WeatherIntensity,
		eProperty_Rectangle,
		eProperty_RectangleTop,
		eProperty_RectangleLeft,
		eProperty_RectangleRight,
		eProperty_RectangleBottom,
		eProperty_Angle,
		eProperty_ValueReject,
		eProperty_ItemOption,
		eProperty_Moving,
		eProperty_MoveMode,
		eProperty_TtbProperty,
		eProperty_BuffRandomAnlgeRange,
		eProperty_RangeFlag,
		eProperty_RangeMin,
		eProperty_RangeMax,
		eProperty_Drop,
		eProperty_GravityAcceleration,
	};

	enum eEvent
	{
		eEvent_None,
		eEvent_EnterMap,
		eEvent_GiveItem,
		eEvent_CheckSelf,
		eEvent_Kill,
		eEvent_IsKilled,
		eEvent_TimeOver,
	};

	enum eState
	{
		eState_None,
		eState_Start,
		eState_Hold,
		eState_Stop,
	};

	enum eAction
	{
		eAction_None,
		eAction_GetItem,
		eAction_KillMonster,
		eAction_SummonMonster,
		eAction_CreateTrigger,
		eAction_PutNotice,
		eAction_SetAlias,
		eAction_AddBuff,
		eAction_RemoveBuff,
		eAction_MoveMap,
		eAction_AddMoney,
		eAction_RemoveItem,
		eAction_Chat,
		eAction_MonsterChat,
		eAction_BeginQuest,
		eAction_EndQuest,
		eAction_DeleteQuest,
		eAction_AddEffect,
		eAction_RemoveEffect,
		eAction_PlaySound,
		eAction_SetFlag,
		eAction_AddFlag,
		// Actions for Instance Camera
		eAction_CameraShake,
		eAction_CameraFilterAttach,
		eAction_CameraFilterDetach,
		eAction_CameraFilterMove,
		eAction_ChangeState,
		// Actions for Instance Dungeon(ID)
		eAction_Dungeon_SwitchOn,
		eAction_Dungeon_SwitchOff,
		eAction_Dungeon_SwitchToggle,
		eAction_Dungeon_WarpOn,
		eAction_Dungeon_WarpOff,
		eAction_Dungeon_SetBossMonster,
		eAction_Dungeon_AddPoint,
		eAction_Dungeon_SummonMonsterGroup,
		eAction_Dungeon_SwitchNpcOn,
		eAction_Dungeon_SwitchNpcOff,
		eAction_Dungeon_ZoneBlock,
		eAction_Dungeon_ZoneFree,
		eAction_Dungeon_EdgeBlock,
		eAction_Dungeon_EdgeFree,
		eAction_Animation,
		eAction_TimerStart,
		eAction_TimerState,
		eAction_ControlMonster,
		eAction_SetReviveFlag,
		eAction_PutNpcChat,
		eAction_PlayWeather,
		eAction_EngineEffect,
		eAction_CollisionTile,
		eAction_SelectFlag,
		eAction_ActiveSkill,
		eAction_CristalEmpireBonus,
		eAction_AddShockedTime,
		eAction_DropObject,
		eAction_MoveObject,
	};

	enum eOwner
	{
		eOwner_None,
		eOwner_Player,
		eOwner_Monster,
		eOwner_Server,
	};

	enum eCheckType
	{
		eCheckType_None,	//매 프레임마다 검사
		eCheckType_OneTime,	//참인 경우 더 이상 검사하지 않는다
	};

	enum eTarget
	{
		eTarget_None,
		eTarget_Alias,		// 별칭을 가진 오브젝트가 대상
		eTarget_GroupAlias,	// 그룹 별칭을 가진 오브젝트가 대상
		eTarget_Condition,	// 조건을 만족한 오브젝트가 대상
		eTarget_ChannelPlayer,	// 트리거가 속한 채널의 모든 플레이어
		eTarget_PartyMaster,
	};

	enum eMessage
	{
		eMessage_None,
		eMessage_EquipItem,
		eMessage_UseItem,
		eMessage_DropItem,
		eMessage_GetItem,
		eMessage_ExitMap,
		eMessage_EnterMap,
		eMessage_BuildParty,
		eMessage_JoinParty,
		eMessage_DisbandParty,
		eMessage_Kill,
		eMessage_BeKilled,
		eMessage_Attack,
		eMessage_BeAttacked,
		eMessage_MoveObject,
		eMessage_GetQuest,
		eMessage_GiveUpQuest,
		eMessage_ClearQuest,
		eMessage_LevelUpPlayer,
		//eMessage_GainAttributeValue,
		eMessage_ChangeJob,
		eMessage_Heal,
		eMessage_Damage,
	};	

	enum eOperation
	{
		eOperation_None,
		eOperation_More,	// >
		eOperation_Less,	// <
		eOperation_Equal,	// =
		eOperation_Not,		// <>
		eOperation_LessThan,// >=
		eOperation_MoreThan,// <=
	};

	enum eObjectState
	{
		eObjectState_Stop,
		eObjectState_Run,
		eObjectState_Walk,
	};

	enum eControlType
	{
		eControlTypeNone,
		eControlTypePause,
		eControlTypeResume,
		eControlTypeSkill,
	};

	struct stProperty
	{
		eProperty mType;
		eOperation mOperation;
		int  mValue;
		BOOL mIsPercent;
		char mTypeText[MAX_PATH];
		char mValueText[MAX_PATH];

		stProperty() :
		mType(eProperty_None),
		mOperation(eOperation_None),
		mValue(0),
		mIsPercent(FALSE)
		{
			ZeroMemory(mTypeText, sizeof(mTypeText));
			ZeroMemory(mValueText, sizeof(mValueText));
		}
	};

	struct TriggerDesc
	{
		DWORD mHashCode;
		char mName[MAX_PATH];
		DWORD mRepeatCount;
		typedef std::list< DWORD > IndexList;
		IndexList mConditionList;
		IndexList mActionList;
		eOwner mOwner;
		DWORD mTimeStamp;
		BOOL mAutoStart;
		BOOL mLoop;
		BOOL mSaveToDb;

		TriggerDesc() :
		mHashCode(0),
		mRepeatCount(0),
		mOwner(eOwner_None),
		mTimeStamp(GetTickCount()),
		mAutoStart(FALSE),
		mLoop(FALSE),
		mSaveToDb(FALSE)
		{
			ZeroMemory(mName, sizeof(mName));
		}
	};

	struct ConditionDesc
	{
		DWORD mIndex;
		typedef stdext::hash_map< eProperty, stProperty > PropertyMap;
		PropertyMap mPropertyMap;
		char mName[MAX_PATH];

		ConditionDesc() :
		mIndex(0)
		{
			ZeroMemory(mName, sizeof(mName));
		}

		const stProperty& GetProperty(eProperty property) const
		{
			PropertyMap::const_iterator iter = mPropertyMap.find(property);
			
			static const stProperty emptyProperty;
			return mPropertyMap.end() == iter ? emptyProperty : iter->second;
		}
	};

	struct ActionDesc
	{
		eAction	mTriggerAction;
		int mIndex;
		typedef stdext::hash_map< eProperty, int > PropertyMap;
		PropertyMap	mPropertyMap;
		char mName[MAX_PATH];

		ActionDesc() :
		mTriggerAction( eAction_None ),
		mIndex(0)
		{
			ZeroMemory(mName, sizeof(mName));
		}

		int GetValue(eProperty property) const
		{
			PropertyMap::const_iterator iter = mPropertyMap.find(property);

			return mPropertyMap.end() == iter ? 0 : iter->second;
		}
	};

	void PutLog(LPCTSTR, ...);
}

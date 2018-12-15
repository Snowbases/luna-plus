#include "StdAfx.h"
#include <sys/stat.h>
#include <sys/types.h>
#include "Parser.h"
#include "common.h"
#include "CheckFunc.h"
#include "..\..\[CC]ServerModule\MHFile.h"
#include "..\Object.h"
#include "..\..\[CC]Quest\QuestDefines.h"
#include "ActionKillMonster.h"
#include "ActionGetItem.h"
#include "ActionSummonMonster.h"
#include "ActionSummonMonsterGroup.h"
#include "ActionPutNotice.h"
#include "ActionSetAlias.h"
#include "ActionBuff.h"
#include "ActionRemoveBuff.h"
#include "ActionCreateTrigger.h"
#include "ActionMoveMap.h"
#include "ActionAddMoney.h"
#include "ActionRemoveItem.h"
#include "ActionChat.h"
#include "ActionMonsterChat.h"
#include "ActionDungeon.h"
#include "ActionBeginQuest.h"
#include "ActionEndQuest.h"
#include "ActionDeleteQuest.h"
#include "ActionEffect.h"
#include "ActionPlaySound.h"
#include "ActionCamera.h"
#include "ActionSetFlag.h"
#include "ActionTimer.h"
#include "ActionAnimation.h"
#include "ActionControlMonster.h"
#include "ActionSetReviveFlag.h"
#include "ActionPutNpcChat.h"
#include "ActionPlayWeather.h"
#include "ActionEngineEffect.h"
#include "ActionCollisionTile.h"
#include "ActionSelectFlag.h"
#include "ActionCristalEmpireBonus.h"
#include "ActionAddFlag.h"

namespace Trigger
{
	struct PropertyText
	{
		LPCTSTR mText;
		eProperty mValue;		
		CheckFunc mCheckFunc;
		DWORD mLine;
	};

	// 문자열에 따라 어떤 속성이 선택되는지 정해진 상수 테이블. 찾기 쉽도록 알파벳 정렬로 배치하자
	// 컨디션은 정의된 속성에 따라 검사하는 함수가 정해진다. 필요할 경우 적절히 연결시켜준다
	const PropertyText propertyTextTable[] =
	{
		{"actionType", eProperty_ActionType, CheckNone, __LINE__},
		{"activeNpcId", eProperty_ActiveNpcId, CheckActiveNpcId, __LINE__},
		{"alias", eProperty_Alias, CheckAlias, __LINE__},
		{"angle", eProperty_Angle, CheckNone, __LINE__},
		{"autoStart", eProperty_AutoStart, CheckNone, __LINE__},

		{"bgm", eProperty_BGM, CheckNone, __LINE__},
		{"byMidnight", eProperty_ByMidnight, CheckNone, __LINE__},
		{"buffRandomAngleRange", eProperty_BuffRandomAnlgeRange, CheckNone, __LINE__},

		{"cameraFilterIndex", eProperty_CameraFilterIndex, CheckNone, __LINE__},
		{"cameraFilterNoRepeat", eProperty_CameraFilterNoRepeat, CheckNone, __LINE__},
		{"cameraFilterFadeIn", eProperty_CameraFilterFadeIn, CheckNone, __LINE__},
		{"cameraFilterFadeOut", eProperty_CameraFilterFadeOut, CheckNone, __LINE__},
		{"cameraFilterFadeTime", eProperty_CameraFilterFadeTime, CheckNone, __LINE__},
		{"cameraShakeAmp", eProperty_CameraShakeAmp, CheckNone, __LINE__},
		{"cameraShakeNum", eProperty_CameraShakeNum, CheckNone, __LINE__},
		{"cameraShakeTimes", eProperty_CameraShakeTimes, CheckNone, __LINE__},
		{"checkCount", eProperty_CheckCount, CheckNone, __LINE__},
		{"checkType", eProperty_CheckType, CheckNone, __LINE__},
		{"conditionAlias", eProperty_ConditionAlias, CheckNone, __LINE__},
		{"controlObject", eProperty_ControlType, CheckNone, __LINE__},

		{"drop", eProperty_Drop, CheckNone, __LINE__},
		{"Dungeon_switchOn", eProperty_Dungeon_SwitchOn, Dungeon_CheckSwitchOn, __LINE__},
		{"Dungeon_switchOff", eProperty_Dungeon_SwitchOff, Dungeon_CheckSwitchOff, __LINE__},
		{"Dungeon_switchToggle", eProperty_Dungeon_SwitchToggle, CheckNone, __LINE__},
		{"Dungeon_warpOn", eProperty_Dungeon_WarpOn, CheckNone, __LINE__},
		{"Dungeon_warpOff", eProperty_Dungeon_WarpOff, CheckNone, __LINE__},
		{"Dungeon_Difficulty", eProperty_Dungeon_Difficulty, Dungeon_CheckDifficulty, __LINE__},
		{"Dungeon_CheckPoint", eProperty_Dungeon_CheckPoint, Dungeon_CheckPoint, __LINE__},
		{"Dungeon_AddPoint", eProperty_Dungeon_AddPoint, CheckNone, __LINE__},
		{"Dungeon_CheckPlayerNum", eProperty_Dungeon_CheckPlayerNum, Dungeon_CheckPlayerNum, __LINE__},

		{"effectDescNum", eProperty_EffectDescNum, CheckNone, __LINE__},
		{"EffectAddToMap", eProperty_EffectAddToMap, CheckNone, __LINE__},
		{"elapsedDay", eProperty_ElapsedDay, CheckElapsedDay, __LINE__},
		{"elapsedSecond", eProperty_ElapsedSecond, CheckElapedSecond, __LINE__},
		{"event", eProperty_Event, CheckEvent, __LINE__},

		{"isForcedly", eProperty_IsForcedly, CheckNone, __LINE__},
		{"itemDropRate", eProperty_ItemDropRate, CheckNone, __LINE__},
		{"itemIndex", eProperty_ItemIndex, CheckNone, __LINE__},
		{"itemQuantity", eProperty_ItemQuantity, CheckNone, __LINE__},
		{"itemOption", eProperty_ItemOption, CheckNone, __LINE__},

		{"gravityAcceleration", eProperty_GravityAcceleration, CheckNone, __LINE__},
		{"groupAlias", eProperty_GroupAlias, CheckGroupAlias, __LINE__},
		{"guildRank", eProperty_GuildRank, CheckGuildRank, __LINE__},

		{"loginGuildMember", eProperty_LoginGuildMember, CheckLoginGuildMember, __LINE__},
		{"life", eProperty_Health, CheckHP, __LINE__},
		{"loop", eProperty_Loop, CheckNone, __LINE__},

		{"map", eProperty_MapIndex, CheckNone, __LINE__},
		{"mana", eProperty_Mana, CheckMP, __LINE__},
		{"money", eProperty_Money, CheckNone, __LINE__},
		{"monsterKind", eProperty_MonsterKind, CheckMonsterKind, __LINE__},
		{"monsterSpeech", eProperty_MonsterSpeech, CheckNone, __LINE__},
		{"move_x", eProperty_MoveX, CheckNone, __LINE__},
		{"move_Z", eProperty_MoveZ, CheckNone, __LINE__},

		{"name", eProperty_Name, CheckNone, __LINE__},

		{"objectKind", eProperty_ObjectKind, CheckObjectKind, __LINE__},
		{"objectState", eProperty_ObjectState, CheckObjectState, __LINE__},
		
		{"partySize", eProperty_PartySize, CheckPartySize, __LINE__},
		{"playerCountInChannel", eProperty_PlayerCountInChannel, CheckPlayerCountInChannel, __LINE__},
		{"pos_x", eProperty_PosX, CheckNone, __LINE__},
		{"pos_y", eProperty_PosY, CheckNone, __LINE__},
		{"pos_z", eProperty_PosZ, CheckNone, __LINE__},

		{"quest", eProperty_Quest, CheckNone, __LINE__},
		{"questSub", eProperty_QuestSub, CheckNone, __LINE__},
		{"questState", eProperty_QuestState, CheckQuestState, __LINE__},

		{"range", eProperty_Range, CheckRange, __LINE__},
		{"range_x", eProperty_RangeX, CheckNone, __LINE__},
		{"range_z", eProperty_RangeZ, CheckNone, __LINE__},
		{"rangeFlag", eProperty_RangeFlag, CheckRangeFlag, __LINE__},
		{"rangeMin", eProperty_RangeMin, CheckNone, __LINE__},
		{"rangeMax", eProperty_RangeMax, CheckNone, __LINE__},
		{"rectangle", eProperty_Rectangle, CheckRectangle, __LINE__},
		{"rectLeft", eProperty_RectangleLeft, CheckNone, __LINE__},
		{"rectTop", eProperty_RectangleTop, CheckNone, __LINE__},
		{"rectRight", eProperty_RectangleRight, CheckNone, __LINE__},
		{"rectBottom", eProperty_RectangleBottom, CheckNone, __LINE__},

		{"reviveFlag", eProperty_ReviveFlag, CheckNone, __LINE__},
		{"repeat", eProperty_Repeat, CheckNone, __LINE__},

		{"moveMode", eProperty_MoveMode, CheckMoveMode, __LINE__},
		{"moving", eProperty_Moving, CheckMoving, __LINE__},

		{"save", eProperty_SaveToDb, CheckNone, __LINE__},
		{"skillIndex", eProperty_SkillIndex, CheckNone, __LINE__},
		{"sound", eProperty_Sound, CheckNone, __LINE__},

		{"target", eProperty_Target, CheckNone, __LINE__},
		{"timerElapsedSecond", eProperty_TimerElapsedSecond, CheckTimerElapsedSecond, __LINE__},
		{"timerDuration", eProperty_TimerDuration, CheckNone, __LINE__},
		{"timerName", eProperty_TimerName, CheckTimerAlias, __LINE__},
		{"timerState", eProperty_TimerState, CheckNone, __LINE__},
		{"timerType", eProperty_TimerType, CheckNone, __LINE__},
		{"text", eProperty_Text, CheckNone, __LINE__},
		{"ttbProperty", eProperty_TtbProperty, CheckTtbProperty, __LINE__},
		

		{"value", eProperty_Value, CheckNone, __LINE__},
		{"valueReject", eProperty_ValueReject, CheckNone, __LINE__},
		{"victimObjectKind", eProperty_VictimObjectKind, CheckVictimObjectKind, __LINE__},
		{"victimMonsterKind", eProperty_VictimMonsterKind, CheckVictimMonsterKind, __LINE__},
		{"victimMonsterGroupAlias", eProperty_VictimMonsterGroupAlias, CheckVictimMonsterGroupAlias, __LINE__},
		{"victimMonsterAlias", eProperty_VictimMonsterAlias, CheckVictimMonsterAlias, __LINE__},

		{"whatIsDay", eProperty_WhatDay, CheckWhatDay, __LINE__},
		
		{"weatherState", eProperty_WeatherState, CheckNone, __LINE__},
		{"weatherIntensity", eProperty_WeatherIntensity, CheckNone, __LINE__},
	};

	struct ValueText
	{
		eProperty mProperty;
		LPCTSTR mText;
		int mValue;
		ActionFunc mActionFunc;
		DWORD mLine;
	};

	// 문자열에 따라 게임에서 어떤 값이 쓰여지는지 정의된 상수 테이블.
	// 찾기 쉽도록 가능한 알파벳 순으로 배치하자.
	// 액션은 어떤 형이냐에 따라 각각의 액션이 실행된다. 적절히 연결시켜주자
	const ValueText valueTextTable[] =
	{
		{eProperty_ActionType, "getItem", eAction_GetItem, CActionGetItem::Clone, __LINE__},
		{eProperty_ActionType, "killMonster", eAction_KillMonster, CActionKillMonster::Clone, __LINE__},
		{eProperty_ActionType, "createTrigger", eAction_CreateTrigger, CActionCreateTrigger::Clone, __LINE__},
		{eProperty_ActionType, "summonMonster", eAction_SummonMonster, CActionSummonMonster::Clone, __LINE__},
		{eProperty_ActionType, "putNotice", eAction_PutNotice, CActionPutNotice::Clone, __LINE__},
		{eProperty_ActionType, "setAlias", eAction_SetAlias, CActionSetAlias::Clone, __LINE__},
		{eProperty_ActionType, "buff", eAction_AddBuff, CActionBuff::Clone, __LINE__},
		{eProperty_ActionType, "removeBuff", eAction_RemoveBuff, CActionRemoveBuff::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_switchOn", eAction_Dungeon_SwitchOn, CActionDungeonSwitchOn::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_switchOff", eAction_Dungeon_SwitchOff, CActionDungeonSwitchOff::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_switchToggle", eAction_Dungeon_SwitchToggle, CActionDungeonSwitchToggle::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_warpOn", eAction_Dungeon_WarpOn, CActionDungeonWarpOn::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_warpOff", eAction_Dungeon_WarpOff, CActionDungeonWarpOff::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_SetBossMonster", eAction_Dungeon_SetBossMonster, CActionDungeonSetBossMonster::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_AddPoint", eAction_Dungeon_AddPoint, CActionDungeonAddPoint::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_SummonMonsterGroup", eAction_Dungeon_SummonMonsterGroup, CActionDungeonSummonMonsterGroup::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_SwitchNpcOn", eAction_Dungeon_SwitchNpcOn, CActionDungeonSwitchNpcOn::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_SwitchNpcOff", eAction_Dungeon_SwitchNpcOff, CActionDungeonSwitchNpcOff::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_ZoneBlock", eAction_Dungeon_ZoneBlock, CActionDungeonZoneBlock::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_ZoneFree", eAction_Dungeon_ZoneFree, CActionDungeonZoneFree::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_EdgeBlock", eAction_Dungeon_EdgeBlock, CActionDungeonEdgeBlock::Clone, __LINE__},
		{eProperty_ActionType, "Dungeon_EdgeFree", eAction_Dungeon_EdgeFree, CActionDungeonEdgeFree::Clone, __LINE__},
		{eProperty_ActionType, "MoveMap", eAction_MoveMap, CActionMoveMap::Clone, __LINE__},
		{eProperty_ActionType, "AddMoney", eAction_AddMoney, CActionAddMoney::Clone, __LINE__},
		{eProperty_ActionType, "RemoveItem", eAction_RemoveItem, CActionRemoveItem::Clone, __LINE__},
		{eProperty_ActionType, "chat", eAction_Chat, CActionChat::Clone, __LINE__},
		{eProperty_ActionType, "monsterChat", eAction_MonsterChat, CActionMonsterChat::Clone, __LINE__},
		{eProperty_ActionType, "beginQuest", eAction_BeginQuest, CActionBeginQuest::Clone, __LINE__},
		{eProperty_ActionType, "endQuest", eAction_EndQuest, CActionEndQuest::Clone, __LINE__},
		{eProperty_ActionType, "deleteQuest", eAction_DeleteQuest, CActionDeleteQuest::Clone, __LINE__},
		{eProperty_ActionType, "addEffect", eAction_AddEffect, CActionAddEffect::Clone, __LINE__},
		{eProperty_ActionType, "removeEffect", eAction_RemoveEffect, CActionRemoveEffect::Clone, __LINE__},
		{eProperty_ActionType, "playSound", eAction_PlaySound, CActionPlaySound::Clone, __LINE__},
		{eProperty_ActionType, "CameraFilterAttach", eAction_CameraFilterAttach, CActionCameraFilterAttach::Clone, __LINE__},
		{eProperty_ActionType, "CameraFilterDetach", eAction_CameraFilterDetach, CActionCameraFilterDetach::Clone, __LINE__},
		{eProperty_ActionType, "CameraShake", eAction_CameraShake, CActionCameraShake::Clone, __LINE__},
		{eProperty_ActionType, "CameraFilterMove", eAction_CameraFilterMove, CActionCameraFilterMove::Clone, __LINE__},
		{eProperty_ActionType, "setFlag", eAction_SetFlag, CActionSetFlag::Clone, __LINE__},
		{eProperty_ActionType, "addFlag", eAction_AddFlag, CActionAddFlag::Clone, __LINE__},
		{eProperty_ActionType, "TimerStart", eAction_TimerStart, CActionTimerStart::Clone, __LINE__},
		{eProperty_ActionType, "TimerState", eAction_TimerState, CActionTimerState::Clone, __LINE__},
		{eProperty_ActionType, "Animation", eAction_Animation, CActionAnimation::Clone, __LINE__},
		{eProperty_ActionType, "ControlMonster", eAction_ControlMonster, CActionControlMonster::Clone, __LINE__},
		{eProperty_ActionType, "SetReviveFlag", eAction_SetReviveFlag, CActionSetReviveFlag::Clone, __LINE__},
		{eProperty_ActionType, "PutNpcChat", eAction_PutNpcChat, CActionPutNpcChat::Clone, __LINE__},
		{eProperty_ActionType, "playWeather", eAction_PlayWeather, CActionPlayWeather::Clone, __LINE__},
		{eProperty_ActionType, "EngineEffect", eAction_EngineEffect, CActionEngineEffect::Clone, __LINE__},
		{eProperty_ActionType, "CollisionTile", eAction_CollisionTile, CActionCollisionTile::Clone, __LINE__},
		{eProperty_ActionType, "SelectFlag", eAction_SelectFlag, CActionSelectFlag::Clone, __LINE__},
		{eProperty_ActionType, "ActiveSkill", eAction_ActiveSkill, CActionActiveSkill::Clone, __LINE__},
		{eProperty_ActionType, "CristalEmpireBonus", eAction_CristalEmpireBonus, CActionCristalEmpireBonus::Clone, __LINE__},
		{eProperty_ActionType, "AddShockedTime", eAction_AddShockedTime, CActionAddShockedTime::Clone, __LINE__},
		{eProperty_ActionType, "DropObject", eAction_DropObject, CActionDropObject::Clone, __LINE__},
		{eProperty_ActionType, "MoveObject", eAction_MoveObject, CActionMoveObject::Clone, __LINE__},
		

		{eProperty_AutoStart, "true", TRUE, 0, __LINE__},
		{eProperty_AutoStart, "false", FALSE, 0, __LINE__},

		{eProperty_BGM, "true", TRUE, 0, __LINE__},
		{eProperty_BGM, "false", FALSE, 0, __LINE__},

		{eProperty_ByMidnight, "true", TRUE, 0, __LINE__},
		{eProperty_ByMidnight, "false", FALSE, 0, __LINE__},

		{eProperty_CheckType, "oneTime", eCheckType_OneTime, 0, __LINE__},

		{eProperty_ControlType, "pause", eControlTypePause, 0, __LINE__},
		{eProperty_ControlType, "resume", eControlTypeResume, 0, __LINE__},
		{eProperty_ControlType, "skill", eControlTypeSkill, 0, __LINE__},
		
		{eProperty_CameraFilterNoRepeat, "true", TRUE, 0, __LINE__},
		{eProperty_CameraFilterNoRepeat, "false", FALSE, 0, __LINE__},
		{eProperty_CameraFilterFadeIn, "true", TRUE, 0, __LINE__},
		{eProperty_CameraFilterFadeIn, "false", FALSE, 0, __LINE__},
		{eProperty_CameraFilterFadeOut, "true", TRUE, 0, __LINE__},
		{eProperty_CameraFilterFadeOut, "false", FALSE, 0, __LINE__},

		{eProperty_Drop, "on", TRUE, 0, __LINE__},
		{eProperty_Drop, "off", FALSE, 0, __LINE__},

		{eProperty_Event, "kill", eEvent_Kill, 0, __LINE__},
		{eProperty_Event, "isKilled", eEvent_IsKilled, 0, __LINE__},
		{eProperty_Event, "timeOver", eEvent_TimeOver, 0, __LINE__},

		{eProperty_GuildRank, "master", GUILD_MASTER, 0, __LINE__},
		{eProperty_GuildRank, "viceMaster", GUILD_VICEMASTER, 0, __LINE__},
		{eProperty_GuildRank, "senior", GUILD_SENIOR, 0, __LINE__},
		{eProperty_GuildRank, "junior", GUILD_JUNIOR, 0, __LINE__},
		{eProperty_GuildRank, "member", GUILD_MEMBER, 0, __LINE__},
		{eProperty_GuildRank, "student", GUILD_STUDENT, 0, __LINE__},
		{eProperty_GuildRank, "none", GUILD_NOTMEMBER, 0, __LINE__},

		{eProperty_IsForcedly, "true", TRUE, 0, __LINE__},
		{eProperty_IsForcedly, "false", FALSE, 0, __LINE__},

		{eProperty_ItemOption, "strength", ITEM_OPTION::Drop::KeyPlusStrength, 0, __LINE__},
		{eProperty_ItemOption, "intelligence", ITEM_OPTION::Drop::KeyPlusIntelligence, 0, __LINE__},
		{eProperty_ItemOption, "dexterity", ITEM_OPTION::Drop::KeyPlusDexterity, 0, __LINE__},
		{eProperty_ItemOption, "wisdom", ITEM_OPTION::Drop::KeyPlusWisdom, 0, __LINE__},
		{eProperty_ItemOption, "vitality", ITEM_OPTION::Drop::KeyPlusVitality, 0, __LINE__},
		{eProperty_ItemOption, "physicalAttack", ITEM_OPTION::Drop::KeyPlusPhysicalAttack, 0, __LINE__},
		{eProperty_ItemOption, "physicalDefence", ITEM_OPTION::Drop::KeyPlusPhysicalDefence, 0, __LINE__},
		{eProperty_ItemOption, "magicalAttack", ITEM_OPTION::Drop::KeyPlusMagicalAttack, 0, __LINE__},
		{eProperty_ItemOption, "magicalDefence", ITEM_OPTION::Drop::KeyPlusMagicalDefence, 0, __LINE__},
		{eProperty_ItemOption, "criticalRate", ITEM_OPTION::Drop::KeyPlusCriticalRate, 0, __LINE__},
		{eProperty_ItemOption, "criticalDamage", ITEM_OPTION::Drop::KeyPlusCriticalDamage, 0, __LINE__},
		{eProperty_ItemOption, "accuracy", ITEM_OPTION::Drop::KeyPlusAccuracy, 0, __LINE__},
		{eProperty_ItemOption, "evade", ITEM_OPTION::Drop::KeyPlusEvade, 0, __LINE__},
		{eProperty_ItemOption, "moveSpeed", ITEM_OPTION::Drop::KeyPlusMoveSpeed, 0, __LINE__},
		{eProperty_ItemOption, "life", ITEM_OPTION::Drop::KeyPlusLife, 0, __LINE__},
		{eProperty_ItemOption, "mana", ITEM_OPTION::Drop::KeyPlusMana, 0, __LINE__},
		{eProperty_ItemOption, "lifeRecovery", ITEM_OPTION::Drop::KeyPlusLifeRecovery, 0, __LINE__},
		{eProperty_ItemOption, "manaRecovery", ITEM_OPTION::Drop::KeyPlusManaRecovery, 0, __LINE__},
		{eProperty_ItemOption, "strength%", ITEM_OPTION::Drop::KeyPercentStrength, 0, __LINE__},
		{eProperty_ItemOption, "intelligence%", ITEM_OPTION::Drop::KeyPercentIntelligence, 0, __LINE__},
		{eProperty_ItemOption, "dexterity%", ITEM_OPTION::Drop::KeyPercentDexterity, 0, __LINE__},
		{eProperty_ItemOption, "wisdom%", ITEM_OPTION::Drop::KeyPercentWisdom, 0, __LINE__},
		{eProperty_ItemOption, "vitality%", ITEM_OPTION::Drop::KeyPercentVitality, 0, __LINE__},
		{eProperty_ItemOption, "physicalAttack%", ITEM_OPTION::Drop::KeyPercentPhysicalAttack, 0, __LINE__},
		{eProperty_ItemOption, "physicalDefence%", ITEM_OPTION::Drop::KeyPercentPhysicalDefence, 0, __LINE__},
		{eProperty_ItemOption, "magicalAttack%", ITEM_OPTION::Drop::KeyPercentMagicalAttack, 0, __LINE__},
		{eProperty_ItemOption, "magicalDefence%", ITEM_OPTION::Drop::KeyPercentMagicalDefence, 0, __LINE__},
		{eProperty_ItemOption, "criticalRate%", ITEM_OPTION::Drop::KeyPercentCriticalRate, 0, __LINE__},
		{eProperty_ItemOption, "criticalDamage%", ITEM_OPTION::Drop::KeyPercentCriticalDamage, 0, __LINE__},
		{eProperty_ItemOption, "accuracy%", ITEM_OPTION::Drop::KeyPercentAccuracy, 0, __LINE__},
		{eProperty_ItemOption, "evade%", ITEM_OPTION::Drop::KeyPercentEvade, 0, __LINE__},
		{eProperty_ItemOption, "moveSpeed%", ITEM_OPTION::Drop::KeyPercentMoveSpeed, 0, __LINE__},
		{eProperty_ItemOption, "life%", ITEM_OPTION::Drop::KeyPercentLife, 0, __LINE__},
		{eProperty_ItemOption, "mana%", ITEM_OPTION::Drop::KeyPercentMana, 0, __LINE__},
		{eProperty_ItemOption, "lifeRecovery%", ITEM_OPTION::Drop::KeyPercentLifeRecovery, 0, __LINE__},
		{eProperty_ItemOption, "manaRecovery%", ITEM_OPTION::Drop::KeyPercentManaRecovery, 0, __LINE__},

		{eProperty_Loop, "true", TRUE, 0, __LINE__},
		{eProperty_Loop, "false", FALSE, 0, __LINE__},

		{eProperty_MoveMode, "run", 0, 0, __LINE__},	// 0:뛰기 1:걷기
		{eProperty_MoveMode, "walk", 1, 0, __LINE__},

		{eProperty_MonsterKind, "monster", eObjectKind_Monster, 0, __LINE__},
		{eProperty_MonsterKind, "fieldBoss", eObjectKind_FieldBossMonster, 0, __LINE__},
		{eProperty_MonsterKind, "boss", eObjectKind_BossMonster, 0, __LINE__},

		{eProperty_Moving, "true", TRUE, 0, __LINE__},
		{eProperty_Moving, "false", FALSE, 0, __LINE__},

		{eProperty_ObjectState, "stop", eObjectState_Stop, 0, __LINE__},
		{eProperty_ObjectState, "run",  eObjectState_Run, 0, __LINE__},
		{eProperty_ObjectState, "walk", eObjectState_Walk, 0, __LINE__},
		
		{eProperty_ObjectKind, "player", eObjectKind_Player, 0, __LINE__},
		{eProperty_ObjectKind, "monster", eObjectKind_Monster, 0, __LINE__},
		{eProperty_ObjectKind, "npc", eObjectKind_Npc, 0, __LINE__},

		{eProperty_QuestState, "complete", eQuestExecute_EndQuest, 0, __LINE__},
		{eProperty_QuestState, "incomplete", eQuestExecute_StartQuest, 0, __LINE__},

		{eProperty_Rectangle, "in", TRUE, 0, __LINE__},
		{eProperty_Rectangle, "out", FALSE, 0, __LINE__},
		{eProperty_ReviveFlag, "none", ReviveFlagNone, 0, __LINE__},
		{eProperty_ReviveFlag, "here", ReviveFlagHere, 0, __LINE__},
		{eProperty_ReviveFlag, "town", ReviveFlagTown, 0, __LINE__},

		{eProperty_SaveToDb, "true", TRUE, 0, __LINE__},
		{eProperty_SaveToDb, "false", FALSE, 0, __LINE__},

		{eProperty_Target, "alias", eTarget_Alias, 0, __LINE__},
		{eProperty_Target, "groupAlias", eTarget_GroupAlias, 0, __LINE__},
		{eProperty_Target, "condition", eTarget_Condition, 0, __LINE__},
		{eProperty_Target, "channelPlayer", eTarget_ChannelPlayer, 0, __LINE__},
		{eProperty_Target, "partyMaster", eTarget_PartyMaster, 0, __LINE__},

		{eProperty_TimerState, "Pause", eTimerState_Pause, 0, __LINE__},
		{eProperty_TimerState, "Resume", eTimerState_Resume, 0, __LINE__},
		{eProperty_TimerState, "Stop", eTimerState_Stop, 0, __LINE__},

		{eProperty_TimerType, "CountDown", eTimerType_CountDown, 0, __LINE__},
		{eProperty_TimerType, "CountUp", eTimerType_CountUp, 0, __LINE__},

		{eProperty_VictimObjectKind, "monster", eObjectKind_Monster, 0, __LINE__},
		{eProperty_VictimObjectKind, "fieldBoss", eObjectKind_FieldBossMonster, 0, __LINE__},
		{eProperty_VictimObjectKind, "boss", eObjectKind_BossMonster, 0, __LINE__},

		{eProperty_WhatDay, "sunday", eGTDay_SUNDAY, 0, __LINE__},
		{eProperty_WhatDay, "monday", eGTDay_MONDAY, 0, __LINE__},
		{eProperty_WhatDay, "tuesday", eGTDay_TUESDAY, 0, __LINE__},
		{eProperty_WhatDay, "wednesday", eGTDay_WEDNESDAY, 0, __LINE__},
		{eProperty_WhatDay, "thursday", eGTDay_THURSDAY, 0, __LINE__},
		{eProperty_WhatDay, "friday", eGTDay_FRIDAY, 0, __LINE__},
		{eProperty_WhatDay, "saturday", eGTDay_SATURDAY, 0, __LINE__},

		{eProperty_WeatherState, "On", TRUE, 0, __LINE__},
		{eProperty_WeatherState, "Off", FALSE, 0, __LINE__},
	};

	CParser::CParser(MAPTYPE mapType) :
	mMapType(mapType)
	{
		CheckData(propertyTextTable, sizeof(propertyTextTable) / sizeof(*propertyTextTable));
		CheckData(valueTextTable, sizeof(valueTextTable) / sizeof(*valueTextTable));
		ZeroMemory(&mFileStatus, sizeof(mFileStatus));
	}

	CParser::~CParser()
	{}

	LPCTSTR CParser::GetDescName() const
	{
		static char name[MAX_PATH];
		sprintf(
			name,
			"system/resource/trigger_%02d.bin",
			mMapType);
		return name;
	}

	BOOL CParser::Update()
	{
		LPCTSTR fileName = GetDescName();
		struct stat fileStatus = {0};

		if(stat(fileName, &fileStatus))
		{
			return FALSE;
		}
		else if(0 == memcmp(&fileStatus, &mFileStatus, sizeof(fileStatus)))
		{
			return FALSE;
		}

		// TODO: 로그 출력
		mFileStatus = fileStatus;
		Release();
		Load(fileName);
		UpdateFuncMap();
		PutLog(
			"Trigger script is loaded(size: %d)",
			mTriggerDescMap.size());
		return TRUE;
	}

	void CParser::Release()
	{
		mTriggerDescMap.clear();
		mConditionDescMap.clear();
		mActionDescMap.clear();
	}

	void CParser::Load(LPCTSTR fileName)
	{
		enum Block
		{
			BlockNone,
			BlockTrigger,
			BlockTriggerInfo,
			BlockTriggerCondition,
			BlockTriggerAction,
		}
		blockType = BlockNone;

		DWORD curTriggerIndex = 0;
		BOOL isComment = FALSE;
		CMHFile file;
		file.Init((char*)fileName, "rb");

		while(FALSE == file.IsEOF())
		{
			TCHAR text[ MAX_PATH ] = { 0 };
			file.GetLine(text, sizeof(text) / sizeof(*text));		
			const int length = _tcslen(text);

			// 090316 LUJ, 중간의 주석 제거
			for(int i = 0; i < length - 1; ++i)
			{
				char& first = text[i];
				char& second = text[i+1];

				if('/' == first && '/' == second)
				{
					first = 0;
					break;
				}
				else if('/' == first && '*' == second)
				{
					first = 0;
					isComment = TRUE;
					break;
				}
				else if('*' == first && '/' == second)
				{
					first = ' ';
					second = ' ';
					isComment = FALSE;
					break;
				}
				else if('@' == first)
				{
					first = 0;
					break;
				}
			}

			TCHAR buffer[MAX_PATH] = { 0 };
			SafeStrCpy(buffer, text, sizeof(text) / sizeof(*text));
			LPCTSTR separator = " \"~\n\t,();";
			LPCTSTR markBegin = "{";
			LPCTSTR markEnd = "}";
			LPCTSTR token = _tcstok(text, separator);

			if(0 == token)
			{
				continue;
			}
			else if(isComment)
			{
				continue;
			}
			else if(0 == stricmp("trigger", token))
			{
				blockType = BlockTrigger;
				token = _tcstok(0, separator);

				eOwner ownerType = eOwner_None;
				char name[MAX_PATH] = {0,};

				if(NULL == token)
				{
					continue;
				}
				else if(0 == stricmp("map", token))
				{
					ownerType = eOwner_Server;
					LPCTSTR triggerName = _tcstok(0, separator);
					TCHAR textNumber[MAX_PATH] = {0};
					_stprintf(
						name,
						"map.%s",
						triggerName ? triggerName : itoa(mTriggerDescMap.size(), textNumber, 10));
				}
				else if(0 == stricmp("player", token))
				{
					ownerType = eOwner_Player;
					LPCTSTR triggerName = _tcstok(0, separator);
					TCHAR textNumber[MAX_PATH] = {0};
					_stprintf(
						name,
						"player.%s",
						triggerName ? triggerName : itoa(mTriggerDescMap.size(), textNumber, 10));
				}
				else
				{
					SafeStrCpy(name, token, sizeof(name) / sizeof(*name));
				}

				curTriggerIndex = GetHashCode(name);
				TriggerDesc& desc = mTriggerDescMap[curTriggerIndex];
				desc.mHashCode = curTriggerIndex;
				desc.mOwner = ownerType;
				SafeStrCpy(desc.mName, name, sizeof(desc.mName) / sizeof(*desc.mName));

				if( ownerType == eOwner_Server || ownerType == eOwner_Player )
					desc.mAutoStart = TRUE;
			}
			else if(0 == stricmp("condition", token))
			{
				blockType = BlockTriggerCondition;

				TriggerDesc& triggerDesc = mTriggerDescMap[curTriggerIndex];
				LPCTSTR alias = _tcstok(0, separator);
				char name[MAX_PATH] = {0};
				
				// 별칭이 있으면 그 값으로, 없으면 트리거 이름에 번호를 붙여 정한다
				if(alias)
				{
					_stprintf(
						name,
						"%s",
						alias);
				}
				else
				{
					_stprintf(
						name,
						"condition,%s,%d",	/*action과 구분하자*/
						triggerDesc.mName,
						triggerDesc.mConditionList.size());
				}

				const DWORD index = GetHashCode(name);
				triggerDesc.mConditionList.push_back(index);
				ConditionDesc& conditionDesc = mConditionDescMap[index];
				SafeStrCpy(
					conditionDesc.mName,
					name,
					sizeof(conditionDesc.mName) / sizeof(*conditionDesc.mName));
			}
			else if(0 == stricmp("action", token))
			{
				blockType = BlockTriggerAction;

				TriggerDesc& triggerDesc = mTriggerDescMap[curTriggerIndex];
				LPCTSTR alias = _tcstok(0, separator);
				char name[MAX_PATH] = {0};

				// 별칭이 있으면 그 값으로, 없으면 트리거 이름에 번호를 붙여 정한다
				if(alias)
				{
					_stprintf(
						name,
						"%s,%s",
						triggerDesc.mName,
						alias);
				}
				else
				{
					_stprintf(
						name,
						"action,%s,%d",	/*condition과 구분하자*/
						triggerDesc.mName,
						triggerDesc.mActionList.size());
				}

				const DWORD index = GetHashCode(name);
				triggerDesc.mActionList.push_back(index);
				ActionDesc& actionDesc = mActionDescMap[index];
				SafeStrCpy(
					actionDesc.mName,
					name,
					sizeof(actionDesc.mName) / sizeof(*actionDesc.mName));
			}
			else if(0 == stricmp("info", token))
			{
				blockType = BlockTriggerInfo;
			}
			else if(0 == stricmp(markBegin, token))
			{
				continue;
			}
			else if(0 == stricmp(markEnd, token))
			{
				continue;
			}
			else if(blockType == BlockTriggerInfo ||
				blockType == BlockTriggerCondition ||
				blockType == BlockTriggerAction)
			{
				LPCTSTR textKeyword = token;
				LPCTSTR textOperation = _tcstok(0, separator);
				LPCTSTR textValue = _tcstok(0, separator);

				if(0==textKeyword || 0==textOperation || 0==textValue)
				{
					continue;
				}
				// 문자열인 경우 후속 문자열을 결합한다
				else if(0 == stricmp("text", textKeyword))
				{
					static char statement[MAX_PATH] = {0};
					ZeroMemory(statement, sizeof(statement));

					while(textValue)
					{
						const DWORD size = sizeof(statement) / sizeof(*statement);
						LPCTSTR textSpace = " ";
						const DWORD nextLength = strlen(statement) + strlen(textValue) + strlen(textSpace);

						if(nextLength > size)
						{
							break;
						}

						strncat(statement, textValue, strlen(textValue));
						strncat(statement, textSpace, strlen(textSpace));
						textValue = _tcstok(0, separator);
					}

					textValue = statement;
				}

				TriggerDesc& triggerDesc = mTriggerDescMap[curTriggerIndex];
				const eProperty propertyType = ParseKeyword(textKeyword);
				stProperty property;
				property.mType = propertyType;
				property.mOperation = ParseOperation(textOperation);
				property.mValue = ParseValue(propertyType, textValue);
				property.mIsPercent = ('%' == textValue[strlen(textValue) - 1]);
				// 디버그를 위해 문자열을 저장해둔다
				SafeStrCpy(property.mTypeText, textKeyword, sizeof(property.mTypeText)/sizeof(*property.mTypeText));
				SafeStrCpy(property.mValueText, textValue, sizeof(property.mValueText)/sizeof(*property.mValueText));

				switch(blockType)
				{
				case BlockTriggerInfo:
					{
						switch(property.mType)
						{
						case eProperty_Repeat:
							{
								triggerDesc.mRepeatCount = property.mValue;
								break;
							}
						case eProperty_AutoStart:
							{
								triggerDesc.mAutoStart = property.mValue;
								break;
							}
						case eProperty_Loop:
							{
								triggerDesc.mLoop = property.mValue;
								break;
							}
						case eProperty_SaveToDb:
							{
								triggerDesc.mSaveToDb = property.mValue;
								break;
							}
						}
						break;
					}
				case BlockTriggerCondition:
					{
						const TriggerDesc::IndexList& indexList = triggerDesc.mConditionList;

						if(indexList.empty())
						{
							break;
						}

						const DWORD descIndex = indexList.back();
						ConditionDesc& conditionDesc = mConditionDescMap[descIndex];
						conditionDesc.mIndex = descIndex;
						conditionDesc.mPropertyMap[propertyType] = property;
						break;
					}
				case BlockTriggerAction:
					{
						const TriggerDesc::IndexList& indexList = triggerDesc.mActionList;

						if(indexList.empty())
						{
							break;
						}

						const DWORD descIndex = indexList.back();
						ActionDesc& actionDesc = mActionDescMap[descIndex];
						actionDesc.mIndex = descIndex;
						actionDesc.mPropertyMap[propertyType] = property.mValue;

						if(eProperty_ActionType == propertyType)
						{
							SafeStrCpy(
								actionDesc.mName,
								textValue,
								sizeof(actionDesc.mName)/sizeof(*actionDesc.mName));
							actionDesc.mTriggerAction = (eAction)property.mValue;
						}
						break;
					}
				}
			}
		}
	}

	eProperty CParser::ParseKeyword(LPCTSTR keyword)
	{
		for(int i = sizeof(propertyTextTable)/sizeof(*propertyTextTable); i-- > 0;)
		{
			const PropertyText& data = propertyTextTable[i];

			if(0 == _tcsicmp(data.mText, keyword))
			{
				return data.mValue;
			}
		}

		// 091104 LUJ, 미지정 속성은 플래그 처리로 간주한다
		return eProperty(GetHashCode(keyword));
	}

	int CParser::ParseValue(eProperty property, LPCTSTR text)
	{
		// 0901027 LUJ, 문자열인 경우 해쉬 코드 형태로 반환한다
		if(eProperty_Text == property)
		{
			for(LPCTSTR ch = text; 0 != *ch; ch = _tcsinc(ch))
			{
				if('0' <= *ch &&
					'9' >= *ch)
				{
					continue;
				}
				else if(' ' == *ch)
				{
					continue;
				}

				return GetHashCode(text);
			}
		}

		const int value = atoi(text ? text : "");

		// 숫자인 경우 더 이상의 처리가 필요없다
		if(0 != value)
		{
			return value;
		}
		// 0을 처리할 수 있도록 한다
		else if(0 == stricmp("0", text))
		{
			return 0;
		}

		// 상수 테이블에서 값이 있는지 찾아낸다
		for(DWORD i = sizeof(valueTextTable)/sizeof(*valueTextTable); i-- > 0;)
		{
			const ValueText& data = valueTextTable[i];

			if(data.mProperty != property)
			{
				continue;
			}
			else if(0 == _tcsicmp(data.mText, text))
			{
				return data.mValue;
			}
		}

		// 그래도 없으면 해쉬 코드를 반환한다
		return GetHashCode(text);
	}

	eOperation CParser::ParseOperation(LPCTSTR text) const
	{
		if(0 == stricmp(">", text))
		{
			return eOperation_More;
		}
		else if(0 == stricmp("<", text))
		{
			return eOperation_Less;
		}
		else if(0 == stricmp("=", text))
		{
			return eOperation_Equal;
		}
		else if(0 == stricmp("<>", text))
		{
			return eOperation_Not;
		}
		else if(0 == stricmp(">=", text))
		{
			return eOperation_LessThan;
		}
		else if(0 == stricmp("<=", text))
		{
			return eOperation_MoreThan;
		}

		return eOperation_None;
	}

	const TriggerDesc& CParser::GetTriggerDesc(DWORD hashCode) const
	{
		const TriggerDescMap::const_iterator iter = mTriggerDescMap.find(hashCode);

		static TriggerDesc emptyDesc;
		return mTriggerDescMap.end() == iter ? emptyDesc : iter->second;
	}

	const ActionDesc& CParser::GetActionDesc(DWORD actionDescIndex) const
	{
		const ActionDescMap::const_iterator iter = mActionDescMap.find(actionDescIndex);

		static ActionDesc emptyDesc;
		return mActionDescMap.end() == iter ? emptyDesc : iter->second;
	}

	const ConditionDesc& CParser::GetConditionDesc(DWORD conditionDescIndex) const
	{
		const ConditionDescMap::const_iterator iter = mConditionDescMap.find(conditionDescIndex);

		static ConditionDesc emptyDesc;
		return mConditionDescMap.end() == iter ? emptyDesc : iter->second;
	}

	// 출처: http://www.gpgstudy.com/forum/viewtopic.php?t=795&highlight=%C7%D8%BD%AC+%B9%AE%C0%DA%BF%AD
	DWORD CParser::GetHashCode(LPCTSTR text)
	{
		const HashCodeMap::const_iterator iterator = mHashCodeMap.find(text);

		if(mHashCodeMap.end() != iterator)
		{
			return iterator->second;
		}

		const DWORD length = strlen(text);
		DWORD hashCode = 5381;

		for(DWORD i = 0; i < length; ++i)
		{
			DWORD ch = DWORD(text[ i ]);
			hashCode = ((hashCode << 5) + hashCode) + ch;
		}

		mAliasMap[hashCode] = text;
		mHashCodeMap[text] = hashCode;
		return hashCode;
	}

	LPCTSTR CParser::GetAliasName(DWORD hashCode) const
	{
		const AliasMap::const_iterator iter = mAliasMap.find(hashCode);

		return mAliasMap.end() == iter ? "?" : iter->second.c_str();
	}

	void CParser::CheckData(const PropertyText* textArray, size_t size)
	{
		typedef stdext::hash_set< DWORD > TextHashSet;
		TextHashSet textHashSet;
		
		for(size_t i = 0; i < size; ++i)
		{
			const PropertyText& data = textArray[i];
			const DWORD hashCode = GetHashCode(data.mText);

			if(textHashSet.end() != textHashSet.find(hashCode))
			{
				PutLog(
					"There is duplicated keyword: %s(line: %d). It'll not proceed.",
					data.mText,
					data.mLine);
				continue;
			}

			textHashSet.insert(hashCode);
		}
	}

	void CParser::CheckData(const ValueText* textArrary, size_t size)
	{
		// 값: 해쉬 코드
		typedef stdext::hash_set< DWORD > TextHashSet;
		typedef stdext::hash_map< eProperty, TextHashSet > TextHashMap;
		TextHashMap textHashMap;

		for(size_t i = 0; i < size; ++i)
		{
			const ValueText& data = textArrary[i];
			const DWORD hashCode = GetHashCode(data.mText);
			TextHashSet& textHashSet = textHashMap[data.mProperty];

			if(textHashSet.end() != textHashSet.find(hashCode))
			{
				PutLog(
					"There is duplicated keyword in same property: %s(line: %d). It'll not proceed.",
					data.mText,
					data.mLine);
				continue;
			}

			textHashSet.insert(hashCode);
		}
	}

	void CParser::UpdateFuncMap()
	{
		mCheckFuncMap.clear();
		mActionFuncMap.clear();

		for(size_t i = sizeof(propertyTextTable) / sizeof(*propertyTextTable);
			i-- > 0;)
		{
			const PropertyText& propertyText = propertyTextTable[i];

			if(0 == propertyText.mCheckFunc)
			{
				continue;
			}

			mCheckFuncMap[propertyText.mValue] = propertyText.mCheckFunc;
		}

		for(size_t i = sizeof(valueTextTable) / sizeof(*valueTextTable);
			i-- > 0;)
		{
			const ValueText& valueText = valueTextTable[i];

			if(0 == valueText.mActionFunc)
			{
				continue;
			}

			mActionFuncMap[eAction(valueText.mValue)] = valueText.mActionFunc;
		}
	}

	CheckFunc CParser::GetCheckFunc(eProperty property) const
	{
		const CheckFuncMap::const_iterator iter = mCheckFuncMap.find(property);

		if(mCheckFuncMap.end() == iter)
		{
			return CheckFlag;
		}

		return iter->second;
	}

	CAction* CParser::CreateAction(eAction action) const
	{
		const ActionFuncMap::const_iterator iter = mActionFuncMap.find(action);

		if(mActionFuncMap.end() == iter)
		{
			return 0;
		}

		const ActionFunc func = iter->second;
		return func();
	}
}

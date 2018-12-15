#pragma once

namespace Trigger
{
	struct stProperty;
	class CMessage;
	class CCondition;

	BOOL IsTrue(const stProperty&, int);
	BOOL IsObjectStateChanged(const stProperty&, int);
	BOOL CheckNone(const CCondition&, const CMessage&);
	BOOL CheckFlag(const CCondition&, const CMessage&);
	BOOL CheckHP(const CCondition&, const CMessage&);
	BOOL CheckMP(const CCondition&, const CMessage&);
	BOOL CheckRange(const CCondition&, const CMessage&);
	BOOL CheckEvent(const CCondition&, const CMessage&);
	BOOL CheckAlias(const CCondition&, const CMessage&);
	BOOL CheckGroupAlias(const CCondition&, const CMessage&);
	BOOL CheckElapsedDay(const CCondition&, const CMessage&);
	BOOL CheckElapedSecond(const CCondition&, const CMessage&);
	BOOL CheckObjectKind(const CCondition&, const CMessage&);
	BOOL CheckObjectIndex(const CCondition&, const CMessage&);
	BOOL CheckPlayerCountInChannel(const CCondition&, const CMessage&);
	BOOL CheckWhatDay(const CCondition&, const CMessage&);
	BOOL CheckMonsterKind(const CCondition&, const CMessage&);
	BOOL CheckVictimObjectKind(const CCondition&, const CMessage&);
	BOOL CheckVictimMonsterKind(const CCondition&, const CMessage&);
	BOOL CheckVictimMonsterGroupAlias(const CCondition&, const CMessage&);
	BOOL CheckVictimMonsterAlias(const CCondition&, const CMessage&);
	BOOL CheckActiveNpcId(const CCondition& , const CMessage&);
	BOOL CheckLoginGuildMember(const CCondition&, const CMessage&);
	BOOL CheckQuestState(const CCondition&, const CMessage&);
	BOOL CheckGuildRank(const CCondition&, const CMessage&);
	BOOL CheckPartySize(const CCondition&, const CMessage&);
	BOOL CheckObjectState(const CCondition&, const CMessage&);
	BOOL CheckTimerElapsedSecond(const CCondition&, const CMessage&);
	BOOL CheckTimerAlias(const CCondition&, const CMessage&);
	BOOL CheckRectangle(const CCondition&, const CMessage&);
	BOOL Dungeon_CheckSwitchOn(const CCondition&, const CMessage&);
	BOOL Dungeon_CheckSwitchOff(const CCondition&, const CMessage&);
	BOOL Dungeon_CheckDifficulty(const CCondition&, const CMessage&);
	BOOL Dungeon_CheckPoint(const CCondition&, const CMessage&);
	BOOL Dungeon_CheckPlayerNum(const CCondition&, const CMessage&);
	BOOL CheckMoveMode(const CCondition&, const CMessage&);
	BOOL CheckMoving(const CCondition&, const CMessage&);
	BOOL CheckTtbProperty(const CCondition&, const CMessage&);
	BOOL CheckRangeFlag(const CCondition&, const CMessage&);
}

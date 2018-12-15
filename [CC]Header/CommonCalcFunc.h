#pragma once

struct ITEM_OPTION;
struct ITEM_INFO;

BOOL CheckRandom(float SeedVal,LEVELTYPE OperatorLevel,LEVELTYPE TargetLevel);
WORD GetPercent(float SeedVal,LEVELTYPE OperatorLevel,LEVELTYPE TargetLevel);
float Round(float value, int precision);
DWORD GetBonusEnchantValue( const ITEM_INFO&, const ITEM_OPTION&, DWORD itemValue, const stEnchantLvBonusRate&);

enum SetSkillStatusType
{
	SetSkillStatusNone,
	SetSkillStatusAdd,
	SetSkillStatusRemove,
};
// 081203 LUJ, 스탯을 계산한다
void SetSkillStatus( Status* const plusStatus, Status* const percentStatus, const SkillScript&, SetSkillStatusType );
// 081231 LUJ, 스탯을 계산 타입
enum SetValueType
{
	SetValueTypeNone,
	SetValueTypeAdd,
	SetValueTypeRemove,
};
// 081231 LUJ, 스탯을 계산한다
void SetValue( PlayerStat::Value& lhs, const PlayerStat::Value& rhs, SetValueType );
void SetValue( PlayerStat::Value&, int value, SetValueType );
void SetPlayerStat( PlayerStat& lhs, const PlayerStat& rhs, SetValueType );
float GetItemDungeonBonus(float currentLevel, float maxLevel, float power);
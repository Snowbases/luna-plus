#include "stdafx.h"
#include "CommonCalcFunc.h"
#include "..\4DyuchiGRX_common\typedef.h"
// 080324 LUJ, 에이전트와 디스트리뷰트에서 참조되지 않도록 전처리기 처리함
#if ! _AGENTSERVER && ! _DISTRIBUTESERVER
#include "CommonStruct.h"
#include "GameResourceStruct.h"
#endif

WORD GetPercent(float SeedVal,LEVELTYPE OperatorLevel,LEVELTYPE TargetLevel)
{	
	//SeedVal+{(ÀÚ½ÅÀÇ ·¹º§ - »ó´ë ·¹º§)/2.5*0.01}
	int LevelGap = OperatorLevel - TargetLevel;
	float fRate = SeedVal + LevelGap*0.025f;
	if(fRate <= 0)
		return 0;

	return (WORD)(fRate * 100);
}

BOOL CheckRandom(float SeedVal,LEVELTYPE OperatorLevel,LEVELTYPE TargetLevel)
{
	WORD Percent = GetPercent(SeedVal,OperatorLevel,TargetLevel);
	WORD randVal = (WORD)(rand()%100);
	if(randVal <= Percent)
		return TRUE;
	else
		return FALSE;
}

// 080221 LUJ, 주어진 자리 수만큼 반올림 시킴
float Round( float value, int precision )
{
	// 081231 LUJ, 반올림이 되지 않는 문제 수정
	const double base = pow( 10.0f, max( precision - 1, 0 ) );

	return float( floor( value * base + 0.5f ) / base );
}


// 080324 LUJ, 에이전트와 디스트리뷰트에서 참조되지 않도록 전처리기 처리함
#if ! _AGENTSERVER && ! _DISTRIBUTESERVER
DWORD GetBonusEnchantValue(const ITEM_INFO& info, const ITEM_OPTION& option, DWORD itemValue, const stEnchantLvBonusRate& pEnchantLvBonusRate)
{
	// 080320 LUJ,	실수 오차를 없애기 위해 반올림한 값을 사용해야 한다. 공식 작성: 송가람
	// 090714 ShinJS --- 공식 수정. 공식 작성: 원종규
	//
	//				B																 C											D															E
	//				(보너스상수1 * 인챈트 레벨 * (인챈트 레벨 + 1) / 보너스상수2) * { (인챈트 수치 * (인챈트 레벨 * 2 + 1 )) + (아이템 능력 * 인챈트 레벨 * (인챈트 레벨 + 1) * 0.0015) + (아이템 등급 * (인챈트 레벨 * 아이템 레벨 * 0.01 + 아이템 등급 + 1)) }	
	const float b = pEnchantLvBonusRate.fEnchantLvRate1 * option.mEnchant.mLevel * (option.mEnchant.mLevel + 1) / pEnchantLvBonusRate.fEnchantLvRate2;
	const float c = info.EnchantDeterm * (option.mEnchant.mLevel * 2.0f + 1.0f);
	const float d = float( itemValue ) * option.mEnchant.mLevel * (option.mEnchant.mLevel + 1.0f) * 0.0015f;
	const float e = info.kind * (option.mEnchant.mLevel * info.LimitLevel * 0.01f + info.kind + 1);

	// 080421 LUJ, 반올림한 값을 반환한다
	return DWORD( max( 0, floor( 0.5f + b * (c + d + e) ) ) );
}

void SetSkillStatus(Status* const plusStatus, Status* const percentStatus, const SkillScript& script, SetSkillStatusType type)
{
	if( !	plusStatus ||
		!	percentStatus )
	{
		return;
	}

	class Function
	{
	public:
		virtual void operator()( float&, float ) const = 0;
	};

	// 081203 LUJ, 값을 더하는 내부 함수
	class AddFunction : public Function
	{
	public:
		virtual void operator()( float& target, float source ) const
		{
			target = target + source;
		}
	}
	Add;

	// 081203 LUJ, 값을 빼는 내부 함수
	class RemoveFunction : public Function 
	{
	public:
		virtual void operator()( float& target, float source ) const
		{
			target = target - source;
		}
	}
	Remove;

	class Function* function = 0;

	switch( type )
	{
	case SetSkillStatusAdd:
		{
			function = &Add;
			break;
		}
	case SetSkillStatusRemove:
		{
			function = &Remove;
			break;
		}
	}

	if( ! function )
	{
		return;
	}

	for(	SkillScript::ValueList::const_iterator it = script.mStatusBonusList.begin();
			script.mStatusBonusList.end() != it;
			++it )
	{
		const SkillScript::Value& value = *it;

		switch( value.mType )
		{
		case SkillScript::TypeStrength:
			{
				( *function )( plusStatus->Str,		value.mPlus );
				( *function )( percentStatus->Str,	value.mPercent );
				break;
			}
		case SkillScript::TypeDexterity:
			{
				( *function )( plusStatus->Dex,		value.mPlus );
				( *function )( percentStatus->Dex,	value.mPercent );
				break;
			}
		case SkillScript::TypeVitality:
			{
				( *function )( plusStatus->Vit,		value.mPlus );
				( *function )( percentStatus->Vit,	value.mPercent );
				break;
			}
		case SkillScript::TypeIntelligence:
			{
				( *function )( plusStatus->Int,		value.mPlus );
				( *function )( percentStatus->Int,	value.mPercent );
				break;
			}
		case SkillScript::TypeWisdom:
			{
				( *function )( plusStatus->Wis,		value.mPlus );
				( *function )( percentStatus->Wis,	value.mPercent );
				break;
			}
		case SkillScript::TypeLife:
			{
				( *function )( plusStatus->MaxLife,		value.mPlus );
				( *function )( percentStatus->MaxLife,	value.mPercent );
				break;
			}
		case SkillScript::TypeMana:
			{
				( *function )( plusStatus->MaxMana,		value.mPlus );
				( *function )( percentStatus->MaxMana,	value.mPercent );
				break;
			}
		case SkillScript::TypeLifeRecovery:
			{
				( *function )( plusStatus->LifeRecover,		value.mPlus );
				( *function )( percentStatus->LifeRecover,	value.mPercent );
				break;
			}
		case SkillScript::TypeManaRecovery:
			{
				( *function )( plusStatus->ManaRecover,		value.mPlus );
				( *function )( percentStatus->ManaRecover,	value.mPercent );
				break;
			}
		case SkillScript::TypePhysicAttack:
			{
				( *function )( plusStatus->PhysicAttack,	value.mPlus );
				( *function )( percentStatus->PhysicAttack,	value.mPercent );
				break;
			}
		case SkillScript::TypePhysicDefence:
			{
				( *function )( plusStatus->PhysicDefense,		value.mPlus );
				( *function )( percentStatus->PhysicDefense,	value.mPercent );
				break;
			}
		case SkillScript::TypeMagicAttack:
			{
				( *function )( plusStatus->MagicAttack,		value.mPlus );
				( *function )( percentStatus->MagicAttack,	value.mPercent );
				break;
			}
		case SkillScript::TypeMagicDefence:
			{
				( *function )( plusStatus->MagicDefense,	value.mPlus );
				( *function )( percentStatus->MagicDefense,	value.mPercent );
				break;
			}
		case SkillScript::TypeMoveSpeed:
			{
				( *function )( plusStatus->MoveSpeed,		value.mPlus );
				( *function )( percentStatus->MoveSpeed,	value.mPercent );
				break;
			}
		case SkillScript::TypeEvade:
			{
				( *function )( plusStatus->Avoid,		value.mPlus );
				( *function )( percentStatus->Avoid,	value.mPercent );
				break;
			}
		case SkillScript::TypeAccuracy:
			{
				( *function )( plusStatus->Accuracy,	value.mPlus );
				( *function )( percentStatus->Accuracy,	value.mPercent );
				break;
			}
		case SkillScript::TypeCriticalRate:
			{
				( *function )( plusStatus->CriticalRate,	value.mPlus );
				( *function )( percentStatus->CriticalRate,	value.mPercent );
				break;
			}
		case SkillScript::TypeCriticalDamage:
			{
				( *function )( plusStatus->CriticalDamage,		value.mPlus );
				( *function )( percentStatus->CriticalDamage,	value.mPercent );
				break;
			}
		}
	}
}

// 081231 LUJ, 스탯을 계산한다
void SetValue( PlayerStat::Value& lhs, const PlayerStat::Value& rhs, SetValueType type )
{
	switch( type )
	{
	case SetValueTypeAdd:
		{
			lhs.mPlus		+= rhs.mPlus;
			lhs.mPercent	+= rhs.mPercent;
			break;
		}
	case SetValueTypeRemove:
		{
			lhs.mPlus		-= rhs.mPlus;
			lhs.mPercent	-= rhs.mPercent;
			break;
		}
	}
}

// 081231 LUJ, 스탯을 계산한다
void SetValue( PlayerStat::Value& lhs, int value, SetValueType type )
{
	switch( type )
	{
	case SetValueTypeAdd:
		{
			lhs.mPlus	+= float( value );
			break;
		}
	case SetValueTypeRemove:
		{
			lhs.mPlus	-= float( value );
			break;
		}
	}
}

// 081231 LUJ, 스탯을 계산한다
void SetPlayerStat( PlayerStat& lhs, const PlayerStat& rhs, SetValueType setValueType )
{
	SetValue( lhs.mPhysicAttack,	rhs.mPhysicAttack,		setValueType );
	SetValue( lhs.mPhysicDefense,	rhs.mPhysicDefense,		setValueType );
	SetValue( lhs.mMagicAttack,		rhs.mMagicAttack,		setValueType );
	SetValue( lhs.mMagicDefense,	rhs.mMagicDefense,		setValueType );
	SetValue( lhs.mStrength,		rhs.mStrength,			setValueType );
	SetValue( lhs.mDexterity,		rhs.mDexterity,			setValueType );
	SetValue( lhs.mVitality,		rhs.mVitality,			setValueType );
	SetValue( lhs.mIntelligence,	rhs.mIntelligence,		setValueType );
	SetValue( lhs.mWisdom,			rhs.mWisdom,			setValueType );
	SetValue( lhs.mLife,			rhs.mLife,				setValueType );
	SetValue( lhs.mMana,			rhs.mMana,				setValueType );
	SetValue( lhs.mRecoveryLife,	rhs.mRecoveryLife,		setValueType );
	SetValue( lhs.mRecoveryMana,	rhs.mRecoveryMana,		setValueType );
	SetValue( lhs.mCriticalRate,	rhs.mCriticalRate,		setValueType );
	SetValue( lhs.mCriticalDamage,	rhs.mCriticalDamage,	setValueType );
	SetValue( lhs.mMoveSpeed,		rhs.mMoveSpeed,			setValueType );
	SetValue( lhs.mAccuracy,		rhs.mAccuracy,			setValueType );
	SetValue( lhs.mEvade,			rhs.mEvade,				setValueType );
}

float GetItemDungeonBonus(float currentLevel, float maxLevel, float power)
{
	return power * sin(min(1, currentLevel / maxLevel) * PI / 2);
}

#endif

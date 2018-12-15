#include "stdafx.h"
#include ".\buffskillinfo.h"
#include "ObjectManager.h"
#include "../[CC]Skill/Client/Manager/SkillManager.h"
#include "CommonCalcFunc.h"
#include "../[CC]Header/GameResourceManager.h"

cBuffSkillInfo::cBuffSkillInfo(void) :
cSkillInfo(cSkillInfo::TypeBuff)
{
	ZeroMemory(
		&mInfo,
		sizeof(mInfo));
}

cBuffSkillInfo::~cBuffSkillInfo(void)
{}

void cBuffSkillInfo::Parse(LPCTSTR text)
{
	GAMERESRCMNGR->Parse(
		text,
		mInfo);
}

void cBuffSkillInfo::AddPassiveStatus()
{
	Status* pStatus = NULL;

	if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypeAdd )
	{
		pStatus = HERO->GetPassiveStatus();
	}
	else if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypePercent ) 
	{
		pStatus = HERO->GetRatePassiveStatus();
	}
	else
	{
		return;
	}

	CHero* const hero= OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}
	
	SkillScript::Buff buff;
	buff.mKind	= mInfo.Status;
	buff.mValue	= mInfo.StatusData;
	SetStatus(
		*hero,
		*pStatus,
		buff,
		SetSkillStatusAdd );

	// 081203 LUJ, 스킬 스크립트 사용
	{
		const SkillScript& script = SKILLMGR->GetSkillScript( GetIndex() );

		::SetSkillStatus(
			HERO->GetPassiveStatus(),
			HERO->GetRatePassiveStatus(),
			script,
			SetSkillStatusAdd );
		SetBuff(
			*hero,
			script,
			SetSkillStatusAdd );
	}
}

void cBuffSkillInfo::RemovePassiveStatus()
{
	Status* pStatus = NULL;

	if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypeAdd )
	{
		pStatus = HERO->GetPassiveStatus();
	}
	else if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypePercent ) 
	{
		pStatus = HERO->GetRatePassiveStatus();
	}
	else
	{
		return;
	}

	CHero* const hero= OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}

	SkillScript::Buff buff;
	buff.mKind	= mInfo.Status;
	buff.mValue	= mInfo.StatusData;
	SetStatus(
		*hero,
		*pStatus,
		buff,
		SetSkillStatusRemove );

	// 081203 LUJ, 스킬 스크립트 사용
	{
		const SkillScript& script = SKILLMGR->GetSkillScript( GetIndex() );

		::SetSkillStatus(
			HERO->GetPassiveStatus(),
			HERO->GetRatePassiveStatus(),
			script,
			SetSkillStatusRemove );
		SetBuff(
			*hero,
			script,
			SetSkillStatusRemove );
	}
}

// 080805 LUJ, 이전에는 상태 이상을 플래그로 처리했음. 이럴 경우 같은 상태 이상이 중첩된 상태에서
//				하나가 해제되면, 유효한 상태 이상이 있어도 일괄로 해제되는 문제가 있었음. 이를 해결하기
//				위해 상태 이상을 카운팅하도록 함.
void cBuffSkillInfo::AddBuffStatus()
{
	Status* pStatus = NULL;

	if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypeAdd )
	{
		pStatus = HERO->GetBuffStatus();
	}
	// 080707 LUJ, 상수를 enum형으로 변경하면서 Percent값을 적용하지 않는 오류가 있어 수정함
	else if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypePercent )
	{
		pStatus = HERO->GetRateBuffStatus();
	}
	else
	{
		return;
	}

	CHero* const hero= OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}

	SkillScript::Buff buff;
	buff.mKind	= mInfo.Status;
	buff.mValue	= mInfo.StatusData;
	SetStatus(
		*hero,
		*pStatus,
		buff,
		SetSkillStatusAdd );

	// 081203 LUJ, 스킬 스크립트 사용
	{
		const SkillScript& script = SKILLMGR->GetSkillScript( GetIndex() );

		::SetSkillStatus(
			HERO->GetBuffStatus(),
			HERO->GetRateBuffStatus(),
			script,
			SetSkillStatusAdd );
		SetBuff(
			*hero,
			script,
			SetSkillStatusAdd );
	}
}

// 080805 LUJ, 이전에는 상태 이상을 플래그로 처리했음. 이럴 경우 같은 상태 이상이 중첩된 상태에서
//				하나가 해제되면, 유효한 상태 이상이 있어도 일괄로 해제되는 문제가 있었음. 이를 해결하기
//				위해 상태 이상을 카운팅하도록 함.
void cBuffSkillInfo::RemoveBuffStatus()
{
	Status* pStatus = NULL;

	if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypeAdd )
	{
		pStatus = HERO->GetBuffStatus();
	}
	else if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypePercent )
	{
		pStatus = HERO->GetRateBuffStatus();
	}
	else
	{
		return;
	}

	CHero* const hero= OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}

	SkillScript::Buff buff;
	buff.mKind	= mInfo.Status;
	buff.mValue	= mInfo.StatusData;
	SetStatus(
		*hero,
		*pStatus,
		buff,
		SetSkillStatusRemove );

	// 081203 LUJ, 스킬 스크립트 사용
	{
		const SkillScript& script = SKILLMGR->GetSkillScript( GetIndex() );

		::SetSkillStatus(
			HERO->GetBuffStatus(),
			HERO->GetRateBuffStatus(),
			script,
			SetSkillStatusRemove );
		SetBuff(
			*hero,
			script,
			SetSkillStatusRemove );
	}
}

// 080805 LUJ, 이전에는 상태 이상을 플래그로 처리했음. 이럴 경우 같은 상태 이상이 중첩된 상태에서
//				하나가 해제되면, 유효한 상태 이상이 있어도 일괄로 해제되는 문제가 있었음. 이를 해결하기
//				위해 상태 이상을 카운팅하도록 함.
void cBuffSkillInfo::AddPetBuffStatus()
{
	Status* pStatus = NULL;

	if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypeAdd )
	{
		pStatus = HEROPET->GetBuffStatus();
	}
	// 080707 LUJ, 상수를 enum형으로 변경하면서 Percent값을 적용하지 않는 오류가 있어 수정함
	else if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypePercent )
	{
		pStatus = HEROPET->GetRateBuffStatus();
	}
	else
	{
		return;
	}
	
	CHeroPet* const heroPet = OBJECTMGR->GetHeroPet();

	if( ! heroPet )
	{
		return;
	}

	SkillScript::Buff buff;
	buff.mKind	= mInfo.Status;
	buff.mValue	= mInfo.StatusData;
	SetStatus(
		*heroPet,
		*pStatus,
		buff,
		SetSkillStatusAdd );

	// 081203 LUJ, 스킬 스크립트 사용
	{
		const SkillScript& script = SKILLMGR->GetSkillScript( GetIndex() );

		::SetSkillStatus(
			HEROPET->GetBuffStatus(),
			HEROPET->GetRateBuffStatus(),
			script,
			SetSkillStatusAdd );
		SetBuff(
			*heroPet,
			script,
			SetSkillStatusAdd );
	}

	HEROPET->CalcStats();
}

// 080805 LUJ, 이전에는 상태 이상을 플래그로 처리했음. 이럴 경우 같은 상태 이상이 중첩된 상태에서
//				하나가 해제되면, 유효한 상태 이상이 있어도 일괄로 해제되는 문제가 있었음. 이를 해결하기
//				위해 상태 이상을 카운팅하도록 함.
void cBuffSkillInfo::RemovePetBuffStatus()
{
	Status* pStatus = NULL;

	if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypeAdd )
	{
		pStatus = HEROPET->GetBuffStatus();
	}
	else if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypePercent )
	{
		pStatus = HEROPET->GetRateBuffStatus();
	}
	else
	{
		return;
	}

	CHeroPet* const heroPet = OBJECTMGR->GetHeroPet();

	if( ! heroPet )
	{
		return;
	}

	SkillScript::Buff buff;
	buff.mKind	= mInfo.Status;
	buff.mValue	= mInfo.StatusData;
	SetStatus(
		*heroPet,
		*pStatus,
		buff,
		SetSkillStatusRemove );

	// 081203 LUJ, 스킬 스크립트 사용
	{
		const SkillScript& script = SKILLMGR->GetSkillScript( GetIndex() );

		::SetSkillStatus(
			HEROPET->GetBuffStatus(),
			HEROPET->GetRateBuffStatus(),
			script,
			SetSkillStatusRemove );
		SetBuff(
			*heroPet,
			script,
			SetSkillStatusRemove );
	}

	HEROPET->CalcStats();
}

void cBuffSkillInfo::SetObjectBattleState( CObject* pOperator, CObject* pTarget )
{	
	switch(mInfo.Status)
	{
	case eStatusKind_None:
	case eStatusKind_Str:
	case eStatusKind_Dex:
	case eStatusKind_Vit:
	case eStatusKind_Int:
	case eStatusKind_Wis:
	case eStatusKind_All:
	case eStatusKind_PhysicAttack:
	case eStatusKind_PhysicDefense:
	case eStatusKind_MagicAttack:
	case eStatusKind_MagicDefense:
	case eStatusKind_Accuracy:
	case eStatusKind_Avoid:
	case eStatusKind_CriticalRate:
	case eStatusKind_Range:
	case eStatusKind_CriticalDamage:
	case eStatusKind_MoveSpeed:
	case eStatusKind_Block:
	case eStatusKind_CoolTime:
	case eStatusKind_CastingProtect:
	case eStatusKind_MaxLife:
	case eStatusKind_MaxMana:
	case eStatusKind_LifeRecoverRate:
	case eStatusKind_ManaRecoverRate:
		{
			if( mInfo.StatusData < 0 )
			{
				if( pOperator ) pOperator->SetObjectBattleState( eObjectBattleState_Battle );
				if( pTarget ) pTarget->SetObjectBattleState( eObjectBattleState_Battle );
			}
			else
			{
				if( pTarget && pTarget->GetObjectBattleState() == eObjectBattleState_Battle )
				{
					if( pOperator ) pOperator->SetObjectBattleState( eObjectBattleState_Battle );
				}
			}
		}
		break;
	case eStatusKind_Poison:
	case eStatusKind_Paralysis:
	case eStatusKind_Stun:
	case eStatusKind_Slip:
	case eStatusKind_Freezing:
	case eStatusKind_Stone:
	case eStatusKind_Silence:
	case eStatusKind_BlockAttack:
	case eStatusKind_Bleeding:
	case eStatusKind_Burning:
	case eStatusKind_HolyDamage:
		// 090204 LUJ, 마나 손실
	case eStatusKind_ManaBurn:
		{
			if( pOperator ) pOperator->SetObjectBattleState( eObjectBattleState_Battle );
			if( pTarget ) pTarget->SetObjectBattleState( eObjectBattleState_Battle );
		}
		break;
	case eStatusKind_God:
	case eStatusKind_MoveStop:
	case eStatusKind_Shield:
		{
			if( pTarget && pTarget->GetObjectBattleState() == eObjectBattleState_Battle )
			{
				if( pOperator ) pOperator->SetObjectBattleState( eObjectBattleState_Battle );
			}
		}
		break;
	}
}

// 081203 LUJ, 스크립트로 수치 보너스를 적용한다
void cBuffSkillInfo::SetStatus( CObject& object, Status& status, const SkillScript::Buff& buff, SetSkillStatusType type ) const
{
	class Function
	{
	public:
		virtual void operator()( BOOL& ) const = 0;
		virtual void operator()( float&, float ) const = 0;
		virtual void operator()( int&, int ) const = 0;
	};

	// 081203 LUJ, 값을 더하는 내부 함수
	class AddFunction : public Function
	{
	public:
		virtual void operator()( BOOL& target ) const
		{
			target = TRUE;
		}
		virtual void operator()( float& target, float source ) const
		{
			target = target + source;
		}
		virtual void operator()( int& target, int source ) const
		{
			target = target + source;
		}
	}
	Add;

	// 081203 LUJ, 값을 빼는 내부 함수
	class RemoveFunction : public Function
	{
	public:
		virtual void operator()( BOOL& target ) const
		{
			target = FALSE;
		}
		virtual void operator()( float& target, float source ) const
		{
			target = target - source;
		}
		virtual void operator()( int& target, int source ) const
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

	AbnormalStatus* abnormalStatus = 0;

	if( HERO == &object )
	{
		abnormalStatus = HERO->GetAbnormalStatus();
	}
	else if( HEROPET == &object )
	{
		abnormalStatus = HEROPET->GetAbnormalStatus();
	}
	else if( abnormalStatus )
	{
		return;
	}

	switch( buff.mKind )
	{
	case eStatusKind_Str:
		{
			( *function )( status.Str, buff.mValue );
			break;
		}
	case eStatusKind_Dex:
		{
			( *function )( status.Dex, buff.mValue );
			break;
		}
	case eStatusKind_Vit:
		{
			( *function )( status.Vit, buff.mValue );
			break;
		}
	case eStatusKind_Int:
		{
			( *function )( status.Int, buff.mValue );
			break;
		}
	case eStatusKind_Wis:
		{
			( *function )( status.Wis, buff.mValue );
			break;
		}
	case eStatusKind_All:
		{
			( *function )( status.Str, buff.mValue );
			( *function )( status.Dex, buff.mValue );
			( *function )( status.Vit, buff.mValue );
			( *function )( status.Int, buff.mValue );
			( *function )( status.Wis, buff.mValue );
			break;
		}
	case eStatusKind_PhysicAttack:
		{
			( *function )( status.PhysicAttack, buff.mValue );
			break;
		}
	case eStatusKind_PhysicDefense:
		{
			( *function )( status.PhysicDefense, buff.mValue );
			break;
		}
	case eStatusKind_MagicAttack:
		{
			( *function )( status.MagicAttack, buff.mValue );
			break;
		}
	case eStatusKind_MagicDefense:
		{
			( *function )( status.MagicDefense, buff.mValue );
			break;
		}
	case eStatusKind_Accuracy:
		{
			( *function )( status.Accuracy, buff.mValue );
			break;
		}
	case eStatusKind_Avoid:
		{
			( *function )( status.Avoid, buff.mValue );
			break;
		}
	case eStatusKind_CriticalRate:
		{
			( *function )( status.CriticalRate, buff.mValue );
			break;
		}
	case eStatusKind_MagicCriticalRate:
		{
			( *function )( status.MagicCriticalRate, buff.mValue );
			break;
		}
	case eStatusKind_Range:
		{
			( *function )( status.Range, buff.mValue );
			break;
		}
	case eStatusKind_CriticalDamage:
		{
			( *function )( status.CriticalDamage, buff.mValue );
			break;
		}
	case eStatusKind_MoveSpeed:
		{
			( *function )( status.MoveSpeed, buff.mValue );
			break;
		}
	case eStatusKind_Block:
		{
			( *function )( status.Block, buff.mValue );
			break;
		}
	case eStatusKind_CoolTime:
		{
			( *function )( status.CoolTime, buff.mValue );
			break;
		}
	case eStatusKind_CastingProtect:
		{
			( *function )( status.CastingProtect, buff.mValue );
			break;
		}
	case eStatusKind_MaxLife:
		{
			( *function )( status.MaxLife, buff.mValue );
			break;
		}
	case eStatusKind_MaxMana:
		{
			( *function )( status.MaxMana, buff.mValue );
			break;
		}
	case eStatusKind_LifeRecoverRate:
		{
			( *function )( status.LifeRecoverRate, buff.mValue );
			break;
		}
	case eStatusKind_ManaRecoverRate:
		{
			( *function )( status.ManaRecoverRate, buff.mValue );
			break;
		}
	case eStatusKind_LifeRecover:
		{
			( *function )( status.LifeRecover, buff.mValue );
			break;
		}
	case eStatusKind_ManaRecover:
		{
			( *function )( status.ManaRecover, buff.mValue );
			break;
		}
	case eStatusKind_Reflect:
		{
			( *function )( status.Reflect, buff.mValue );
			break;
		}
	case eStatusKind_Absorb:
		{
			( *function )( status.Absorb, buff.mValue );
			break;
		}
	case eStatusKind_DamageToLife:
		{
			( *function )( status.DamageToLife, buff.mValue );
			break;
		}
	case eStatusKind_DamageToMana:
		{
			( *function )( status.DamageToMana, buff.mValue );
			break;
		}
	case eStatusKind_GetLife:
		{
			( *function )( status.GetLife, buff.mValue );
			break;
		}
	case eStatusKind_GetMana:
		{
			( *function )( status.GetMana, buff.mValue );
			break;
		}
	case eStatusKind_GetExp:
		{
			( *function )( status.GetExp, buff.mValue );
			break;
		}
	case eStatusKind_GetGold:
		{
			( *function )( status.GetGold, buff.mValue );
			break;
		}
	case eStatusKind_Attrib_None:
		{
			( *function )( status.Attrib_None, buff.mValue );
			break;
		}
	case eStatusKind_Attrib_Earth:
		{
			( *function )( status.Attrib_Earth, buff.mValue );
			break;
		}
	case eStatusKind_Attrib_Water:
		{
			( *function )( status.Attrib_Water, buff.mValue );
			break;
		}
	case eStatusKind_Attrib_Divine:
		{
			( *function )( status.Attrib_Divine, buff.mValue );
			break;
		}
	case eStatusKind_Attrib_Wind:
		{
			( *function )( status.Attrib_Wind, buff.mValue );
			break;
		}
	case eStatusKind_Attrib_Fire:
		{
			( *function )( status.Attrib_Fire, buff.mValue );
			break;
		}
	case eStatusKind_Attrib_Dark:
		{
			( *function )( status.Attrib_Dark, buff.mValue );
			break;
		}
	case eStatusKind_Sword:
		{
			( *function )( status.Sword, buff.mValue );
			break;
		}
	case eStatusKind_Mace:
		{
			( *function )( status.Mace, buff.mValue );
			break;
		}
	case eStatusKind_Axe:
		{
			( *function )( status.Axe, buff.mValue );
			break;
		}
	case eStatusKind_Staff:
		{
			( *function )( status.Staff, buff.mValue );
			break;
		}
	case eStatusKind_Bow:
		{
			( *function )( status.Bow, buff.mValue );
			break;
		}
	case eStatusKind_Gun:
		{
			( *function )( status.Gun, buff.mValue );
			break;
		}
	case eStatusKind_Dagger:
		{
			( *function )( status.Dagger, buff.mValue );
			break;
		}
	case eStatusKind_Spear:
		{
			( *function )( status.Spear, buff.mValue );
			break;
		}
	case eStatusKind_TwoHanded:
		{
			( *function )( status.TwoHanded, buff.mValue );
			break;
		}
	case eStatusKind_TwoBlade:
		{
			( *function )( status.TwoBlade, buff.mValue );
			break;
		}
	case eStatusKind_OneHanded:
		{
			( *function )( status.OneHanded, buff.mValue );
			break;
		}
	case eStatusKind_RobeArmor:
		{
			( *function )( status.Robe, buff.mValue );
			break;
		}
	case eStatusKind_LightArmor:
		{
			( *function )( status.LightArmor, buff.mValue );
			break;
		}
	case eStatusKind_HeavyArmor:
		{
			( *function )( status.HeavyArmor, buff.mValue );
			break;
		}
	case eStatusKind_ShieldArmor:
		{
			( *function )( status.Shield, buff.mValue );
			break;
		}
	case eStatusKind_NormalSpeedRate:
		{
			( *function )( status.NormalSpeedRate, buff.mValue );
			break;
		}
	case eStatusKind_PhysicSkillSpeedRate:
		{
			( *function )( status.PhysicSkillSpeedRate, buff.mValue );
			break;
		}
	case eStatusKind_MagicSkillSpeedRate:
		{
			( *function )( status.MagicSkillSpeedRate, buff.mValue );
			break;
		}
	case eStatusKind_NormalPhysicSkillSpeedRate:
		{
			( *function )( status.NormalSpeedRate, buff.mValue );
			( *function )( status.PhysicSkillSpeedRate, buff.mValue );
			break;
		}
		// 080703 LUJ, 이도류 사용 가능 여부 설정
	case eStatusKind_EnableTwoBlade:
		{
			if( HERO == &object )
			{
				( *function )( HERO->GetHeroTotalInfo()->bUsingTwoBlade );
			}
			break;
		}
		// 071203 KTH, 소모되는 마나량의 % 감소율을 가르킨다
	case eStatusKind_DecreaseManaRate:
		{
			( *function )( status.DecreaseManaRate, buff.mValue );
			break;
		}
		// 071204 KTH, 드랍율을 상승 시켜준다
	case eStatusKind_IncreaseDropRate:
		{
			( *function )( status.IncreaseDropRate, buff.mValue );
			break;
		}
		// 071217 KTH, 경험치 보호 가동
	case eStatusKind_ProtectExp:
		{
			( *function )( status.IsProtectExp );
			break;
		}	
	case eStatusKind_Poison:
		{
			( *function )( abnormalStatus->IsPoison, 1 );
			( *function )( abnormalStatus->Poison, buff.mValue );
			break;
		}
	case eStatusKind_Bleeding:
		{
			( *function )( abnormalStatus->IsBleeding, 1 );
			( *function )( abnormalStatus->Bleeding, buff.mValue );
			break;
		}
	case eStatusKind_Burning:
		{
			( *function )( abnormalStatus->IsBurning, 1 );
			( *function )( abnormalStatus->Burning, buff.mValue );
			break;
		}
	case eStatusKind_HolyDamage:
		{
			( *function )( abnormalStatus->IsHolyDamage, 1 );
			( *function )( abnormalStatus->HolyDamage, buff.mValue );
			break;
		}
	case eStatusKind_Shield:
		{
			( *function )( abnormalStatus->IsShield, 1 );
			( *function )( abnormalStatus->Shield, buff.mValue );
			break;
		}
	case eStatusKind_Hide:
		{
			// 081203 LUJ, 클라이언트에서 처리하지 않는다
			break;
		}
	case eStatusKind_Detect:
		{
			// 081203 LUJ, 클라이언트에서 처리하지 않는다
			break;
		}
	case eStatusKind_Paralysis:
		{
			( *function )( abnormalStatus->IsParalysis, 1 );
			break;
		}
	case eStatusKind_Stun:
		{
			( *function )( abnormalStatus->IsStun, 1 );
			break;
		}
	case eStatusKind_Slip:
		{
			( *function )( abnormalStatus->IsSlip, 1 );
			break;
		}
	case eStatusKind_Freezing:
		{
			( *function )( abnormalStatus->IsFreezing, 1 );
			break;
		}
	case eStatusKind_Stone:
		{
			( *function )( abnormalStatus->IsStone, 1 );
			break;
		}
	case eStatusKind_Silence:
		{
			( *function )( abnormalStatus->IsSilence, 1 );
			break;
		}
	case eStatusKind_BlockAttack:
		{
			( *function )( abnormalStatus->IsBlockAttack, 1 );
			break;
		}
	case eStatusKind_God:
		{
			( *function )( abnormalStatus->IsGod, 1 );
			break;
		}
	case eStatusKind_MoveStop:
		{
			( *function )( abnormalStatus->IsMoveStop, 1 );
			break;
		}
	case eStatusKind_Attract:
		{
			( *function )( abnormalStatus->Attract, buff.mValue );
			break;
		}
	case eStatusKind_UnableUseItem:
		{
			( *function )( abnormalStatus->IsUnableUseItem, 1 );
			break;
		}
	case eStatusKind_UnableBuff:
		{
			( *function )( abnormalStatus->IsUnableBuff, 1 );
			break;
		}
	}
}

// 081203 LUJ, 스크립트로 버프 상태를 적용한다
void cBuffSkillInfo::SetBuff( CObject& object, const SkillScript& script, SetSkillStatusType type ) const
{
	Status* status = 0;

	if( HERO == &object )
	{
		status = HERO->GetBuffStatus();
	}
	else if( HEROPET == &object )
	{
		status = HEROPET->GetBuffStatus();
	}
	else
	{
		return;
	}

	for(	SkillScript::BuffList::const_iterator it = script.mBuffList.begin();
			script.mBuffList.end() != it;
			++it )
	{
		const SkillScript::Buff& buff = *it;

		SetStatus(
			object,
			*status,
			buff,
			type );
	}
}
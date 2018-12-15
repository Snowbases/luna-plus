#include "stdafx.h"
#include "buffskillinfo.h"
#include "player.h"
#include "monster.h"
#include "CharacterCalcManager.h"
#include "CharMove.h"
#include "pet.h"
#include "..\Object\BuffSkillObject.h"
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

void cBuffSkillInfo::AddPassiveStatus( CPlayer* pPlayer ) const
{
	Status* pStatus = NULL;

	if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypeAdd )
	{
		pStatus = pPlayer->GetPassiveStatus();
	}
	else if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypePercent ) 
	{
		pStatus = pPlayer->GetRatePassiveStatus();
	}
	else
	{
		pStatus = pPlayer->GetPassiveStatus();
	}

	SkillScript::Buff buff;
	buff.mKind	= mInfo.Status;
	buff.mValue	= mInfo.StatusData;
	// 081203 LUJ, 속성을 적용한다
	SetStatus(
		*pPlayer,
		*pStatus,
		buff,
		SetSkillStatusAdd );

	// 081203 LUJ, 스킬 스크립트를 적용한다
	{
		const SkillScript& script = GAMERESRCMNGR->GetSkillScript( GetIndex() );

		::SetSkillStatus(
			pPlayer->GetPassiveStatus(),
			pPlayer->GetRatePassiveStatus(),
			script,
			SetSkillStatusAdd );
		SetBuff(
			*pPlayer,
			script,
			SetSkillStatusAdd );
	}
}

void cBuffSkillInfo::RemovePassiveStatus( CPlayer* pPlayer ) const
{
	Status* pStatus = NULL;

	if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypeAdd )
	{
		pStatus = pPlayer->GetPassiveStatus();
	}
	else if( mInfo.StatusDataType == BUFF_SKILL_INFO::StatusTypePercent ) 
	{
		pStatus = pPlayer->GetRatePassiveStatus();
	}
	else
	{
		pStatus = pPlayer->GetPassiveStatus();
	}

	SkillScript::Buff buff;
	buff.mKind	= mInfo.Status;
	buff.mValue	= mInfo.StatusData;
	// 081203 LUJ, 속성을 적용한다
	SetStatus(
		*pPlayer,
		*pStatus,
		buff,
		SetSkillStatusRemove );
	
	// 081203 LUJ, 스킬 스크립트를 적용한다
	{
		const SkillScript& script = GAMERESRCMNGR->GetSkillScript( GetIndex() );

		::SetSkillStatus(
			pPlayer->GetPassiveStatus(),
			pPlayer->GetRatePassiveStatus(),
			script,
			SetSkillStatusRemove );
		SetBuff(
			*pPlayer,
			script,
			SetSkillStatusRemove );
	}
}

void cBuffSkillInfo::AddBuffStatus( CObject* pTarget ) const
{
	if(0 == pTarget)
	{
		return;
	}

	Status* status = pTarget->GetBuffStatus();

	switch( mInfo.StatusDataType )
	{
	case BUFF_SKILL_INFO::StatusTypePercent:
		{
			status = pTarget->GetRateBuffStatus();
			break;
		}
	}

	if( ! status )
	{
		return;
	}

	SkillScript::Buff buff;
	buff.mKind	= mInfo.Status;
	buff.mValue	= mInfo.StatusData;
	// 081203 LUJ, 속성을 적용한다
	SetStatus(
		*pTarget,
		*status,
		buff,
		SetSkillStatusAdd );

	// 081203 LUJ, 스킬 스크립트를 적용한다
	{
		const SkillScript& script = GAMERESRCMNGR->GetSkillScript( GetIndex() );

		::SetSkillStatus(
			pTarget->GetBuffStatus(),
			pTarget->GetRateBuffStatus(),
			script,
			SetSkillStatusAdd );
		SetBuff(
			*pTarget,
			script,
			SetSkillStatusAdd );
	}

	switch( pTarget->GetObjectKind() )
	{
	case eObjectKind_Player:
		{
			CPlayer* player = ( CPlayer* )pTarget;

			CHARCALCMGR->CalcCharStats( player );
			break;
		}
	case eObjectKind_Pet:
		{
			CPet* pet = ( CPet* )pTarget;

			pet->CalcStats();
			break;
		}
	}
}

void cBuffSkillInfo::RemoveBuffStatus( CObject* pTarget ) const
{
	if(0 == pTarget)
	{
		return;
	}

	Status* status = pTarget->GetBuffStatus();

	switch( mInfo.StatusDataType )
	{
	case BUFF_SKILL_INFO::StatusTypePercent:
		{
			status = pTarget->GetRateBuffStatus();
			break;
		}
	}

	if( ! status )
	{
		return;
	}

	SkillScript::Buff buff;
	buff.mKind	= mInfo.Status;
	buff.mValue	= mInfo.StatusData;
	// 081203 LUJ, 속성을 적용한다
	SetStatus(
		*pTarget,
		*status,
		buff,
		SetSkillStatusRemove );

	// 081203 LUJ, 스킬 스크립트를 적용한다
	{
		const SkillScript& script = GAMERESRCMNGR->GetSkillScript( GetIndex() );

		::SetSkillStatus(
			pTarget->GetBuffStatus(),
			pTarget->GetRateBuffStatus(),
			script,
			SetSkillStatusRemove );
		SetBuff(
			*pTarget,
			script,
			SetSkillStatusRemove );
	}

	switch( pTarget->GetObjectKind() )
	{
	case eObjectKind_Player:
		{
			CPlayer* player = ( CPlayer* )pTarget;

			CHARCALCMGR->CalcCharStats( player );
			break;
		}
	case eObjectKind_Pet:
		{
			CPet* pet = ( CPet* )pTarget;

			pet->CalcStats();
			break;
		}
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

	AbnormalStatus* const abnormalStatus = object.GetAbnormalStatus();

	if( ! abnormalStatus )
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
			if( eObjectKind_Player == object.GetObjectKind() )
			{
				CPlayer& player = ( CPlayer& )object;
				( *function )( player.GetHeroTotalInfo()->bUsingTwoBlade );
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
			if( eObjectKind_Player == object.GetObjectKind() )
			{
				CPlayer&	player		= ( CPlayer& )object;
				int			hideLevel	= int( player.GetHideLevel() );

				// 090217 LUJ, 버프 레벨이 증감되도록 수정
				( *function )( hideLevel, (int)buff.mValue );
				player.SetHideLevel( WORD( hideLevel ) );
			}
			break;
		}
	case eStatusKind_Detect:
		{
			if( eObjectKind_Player == object.GetObjectKind() )
			{
				CPlayer&	player		= ( CPlayer& )object;
				int			detectLevel	= int( player.GetDetectLevel() );

				// 090217 LUJ, 버프 레벨이 증감되도록 수정
				( *function )( detectLevel, (int)buff.mValue );
				player.SetDetectLevel( WORD( detectLevel ) );
			}
			break;
		}
	case eStatusKind_Paralysis:
		{
			( *function )( abnormalStatus->IsParalysis, 1 );
			CCharMove::CorrectPlayerPosToServer( &object );

			if( type == SetSkillStatusAdd &&
				object.GetObjectKind() == eObjectKind_Player )
				((CPlayer*)&object)->CancelCurrentCastingSkill( FALSE );
			break;
		}
	case eStatusKind_Stun:
		{
			( *function )( abnormalStatus->IsStun, 1 );
			CCharMove::CorrectPlayerPosToServer( &object );

			if( type == SetSkillStatusAdd &&
				object.GetObjectKind() == eObjectKind_Player )
				((CPlayer*)&object)->CancelCurrentCastingSkill( FALSE );
			break;
		}
	case eStatusKind_Slip:
		{
			( *function )( abnormalStatus->IsSlip, 1 );
			CCharMove::CorrectPlayerPosToServer( &object );

			if( type == SetSkillStatusAdd &&
				object.GetObjectKind() == eObjectKind_Player )
				((CPlayer*)&object)->CancelCurrentCastingSkill( FALSE );
			break;
		}
	case eStatusKind_Freezing:
		{
			( *function )( abnormalStatus->IsFreezing, 1 );
			CCharMove::CorrectPlayerPosToServer( &object );

			if( type == SetSkillStatusAdd &&
				object.GetObjectKind() == eObjectKind_Player )
				((CPlayer*)&object)->CancelCurrentCastingSkill( FALSE );
			break;
		}
	case eStatusKind_Stone:
		{
			( *function )( abnormalStatus->IsStone, 1 );
			CCharMove::CorrectPlayerPosToServer( &object );

			if( type == SetSkillStatusAdd &&
				object.GetObjectKind() == eObjectKind_Player )
				((CPlayer*)&object)->CancelCurrentCastingSkill( FALSE );
			break;
		}
	case eStatusKind_Silence:
		{
			( *function )( abnormalStatus->IsSilence, 1 );

			if( type == SetSkillStatusAdd &&
				object.GetObjectKind() == eObjectKind_Player )
				((CPlayer*)&object)->CancelCurrentCastingSkill( FALSE );
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
	Status* const status = object.GetBuffStatus();

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

cSkillObject* cBuffSkillInfo::GetSkillObject() const
{
	// 버프 외에 패시브, 토글 종류가 생성될 경우 서버 종료 시까지 제거되지 않는 것들이 생긴다
	if(SKILLKIND_BUFF != mInfo.Kind)
	{
		return 0;
	}

	return new cBuffSkillObject(
		*this);
}
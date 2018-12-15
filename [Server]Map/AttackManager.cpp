#include "StdAfx.h"
#include "AttackManager.h"
#include "Network.h"
#include "PackedData.h"
#include "ServerSystem.h"
#include "CharMove.h"
#include "UserTable.h"
#include "ObjectStateManager.h"
#include "Monster.h"
#include "PartyManager.h"
#include "Party.h"
#include "Player.h"
#include "GuildManager.h"
#include "pet.h"
#include "petmanager.h"

#include "SiegeWarfareMgr.h"

#include "GuildTournamentMgr.h"
#include "ItemManager.h"

#include "..\[CC]Header\CommonCalcFunc.h"
// 080616 LUJ, 이벤트 발생 시에 스킬이 액션을 취하기 위해 참조
#include "Event.h"

CAttackManager::CAttackManager()
{
	// 080121 KTH -- 강피엠님의 요청으로 기본 35%로 설정 한다.
	m_PvpDamageRatio = 35;
	m_GTDamageRatio = 10.0f;
}

CAttackManager::~CAttackManager()
{

}

void CAttackManager::sendDieMsg(CObject * pAttacker,CObject* pTarget)
{
	MSG_DWORD2 m2c;
	m2c.Category	= MP_USERCONN;

	if(pTarget->GetObjectKind() & eObjectKind_Monster)
	{
		m2c.Protocol	= MP_USERCONN_MONSTER_DIE;

		m2c.dwObjectID	= pAttacker->GetID();
		m2c.dwData1		= 0;
		m2c.dwData2		= pTarget->GetID();
		
		PACKEDDATA_OBJ->QuickSend(pTarget,&m2c,sizeof(m2c));
	}
	else if(pTarget->GetObjectKind() == eObjectKind_Player)
	{
		m2c.Protocol = MP_USERCONN_CHARACTER_DIE;

		m2c.dwObjectID	= pAttacker->GetID();
		m2c.dwData1		= pAttacker->GetID();
		m2c.dwData2		= pTarget->GetID();
		
		PACKEDDATA_OBJ->QuickSend(pTarget,&m2c,sizeof(m2c));
	}
	else if(pTarget->GetObjectKind() & eObjectKind_Pet)
	{
		// 091110 ONS 펫 애니메이션 추가
		m2c.Protocol = MP_USERCONN_PET_DIE;

		m2c.dwObjectID	= pAttacker->GetID();
		m2c.dwData1		= pAttacker->GetID();
		m2c.dwData2		= pTarget->GetID();
		
		PACKEDDATA_OBJ->QuickSend(pTarget,&m2c,sizeof(m2c));
	}
}

void CAttackManager::RecoverLife(CObject* pOperator,CObject* pObject, float RecoverLifeVal,RESULTINFO* pResultInfo)
{
	DWORD realAddVal = 0;
	DWORD val = 0;	//KES 070918 val; --> val = 0;  (val이 쓰레기값인 상태로 AddLife가 될 우려가 있어서. RecoverMana()함수도 동일)

	// 100219 ShinJS --- 힐량 공식 수정 
	// 힐 회복량 = ( ( (정신 * 11) + ( 지능 * 4 ) + ( 레벨 * 20 ) ) * 스킬위력 ) / 800 + 100
	if( pOperator->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pOperator;
		val = ( DWORD )( ( ( ( pPlayer->GetWisdom() * 11 ) + ( pPlayer->GetIntelligence() * 4 ) + ( pPlayer->GetLevel() * 20 ) ) * RecoverLifeVal ) / 800.f + 100.f );
	}
	else if( pOperator->GetObjectKind() == eObjectKind_Pet )
	{
		PlayerStat& pStat = ((CPet*)pOperator)->GetStat();
		val = ( DWORD )( ( ( ( pStat.mWisdom.mPlus * 11 ) + ( pStat.mIntelligence.mPlus * 4 ) + ( pOperator->GetLevel() * 20 ) ) * RecoverLifeVal ) / 800.f + 100.f );
	}
	else if(pObject->GetObjectKind() & eObjectKind_Monster)
	{
		val = DWORD(RecoverLifeVal);
	}

	// 080728 KTH -- 공성전 모드 일때 힐량 감소
	if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  		SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
  		SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
  	{
  		val = DWORD( val * (SIEGEWARFAREMGR->GetDecreaseHealRatio() / 100) );
  		
  		if( val < 1 )
  			val = 1;
  	}

	if(g_pServerSystem->GetMapNum() == GTMAPNUM)
		val = DWORD( val * 0.5f );	// 길드토너먼트 에서는 50%만

	pObject->AddLife( val, &realAddVal, FALSE );
	pResultInfo->HealLife = realAddVal;

	if( pOperator->GetObjectKind() == eObjectKind_Player||
		pOperator->GetObjectKind() == eObjectKind_Pet)
	{
		if(pObject->GetObjectKind() == eObjectKind_Player)
		{
			((CPlayer*)pObject)->AddAggroToMyMonsters(
				int(realAddVal) / 3,
				pOperator->GetID(),
				pResultInfo->mSkillIndex);
		}
		else if(pObject->GetObjectKind() == eObjectKind_Pet)
		{
			((CPet*)pObject)->AddAggroToMyMonsters(
				int(realAddVal) / 3,
				pOperator->GetID(),
				pResultInfo->mSkillIndex);
		}
	}

	// 090204 LUJ, 카운트 조건이 있는 스킬을 업데이트한다
	pOperator->RemoveBuffCount( eBuffSkillCountType_HealVolumn, val );
	pObject->RemoveBuffCount( eBuffSkillCountType_BeHealedVolumn, val );
}

void CAttackManager::RecoverMana(CObject* pOperator,CObject* pObject, float RecoverManaVal,RESULTINFO* pResultInfo)
{
	DWORD realAddVal = 0;
	DWORD val = 0;

	if( pOperator->GetObjectKind() == eObjectKind_Player )
	{
		// 마나 회복량 = ( ( (정신 * 11) + ( 지능 * 4 ) + ( 레벨 * 20 ) ) * 스킬위력 ) / 800 + 100
		CPlayer* pPlayer = (CPlayer*)pOperator;
		val = ( DWORD )( ( ( (pPlayer->GetWisdom() * 11) + ( pPlayer->GetIntelligence() * 4 ) + ( pPlayer->GetLevel() * 20 ) ) * RecoverManaVal ) / 800.f + 100.f );
	}
	else if( pOperator->GetObjectKind() == eObjectKind_Pet )
	{
		PlayerStat& pStat = ((CPet*)pOperator)->GetStat();
		val = ( DWORD )( ( ( ( pStat.mWisdom.mPlus * 11 ) + ( pStat.mIntelligence.mPlus * 4 ) + ( pOperator->GetLevel() * 20 ) ) * RecoverManaVal ) / 800.f + 100.f );
	}
	pObject->AddMana( val, &realAddVal );
	pResultInfo->RechargeMana = realAddVal;
}

void CAttackManager::PhysicAttack( CObject* pAttacker, CObject* pTarget, RESULTINFO* pDamageInfo, float Accuracy, float AddDamage, WORD AddType )
{
	if( !pAttacker || !pTarget )
		return;

	if( pTarget->GetAbnormalStatus()->IsGod )
	{
		return;
	}

	// 기본 수치 산출
	float Avoid = 0;
	float Attack = 0;
	float Defense = 1;
	float Shield = 0;
	float Block = 0;
	float CriticalRate = 0;
	float CriticalDamagePlus = 0;
	float CriticalDamageRate = 0;
	float Strength = 0;
	float OriginalStragnth = 0;
	float RateAddValue = 0;
	float PlusAddValue = 0;
	float Weapon = 1;
	BOOL bLevelPenalty = FALSE;
	// 080910 LUJ, 블록 시 감소할 수치를 계산하기 위해 선언
	float vitality = 0;

	if( pAttacker->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pAttacker;
		
		Accuracy += pPlayer->GetAccuracy();

		int min = ( int )( pPlayer->GetPhysicAttackMin() );
		int max = ( int )( pPlayer->GetPhysicAttackMax() );

		if( max > min )
		{
			Attack = float( random( min, max ) );
		}
		else
		{
			Attack = float( min );
		}

		CriticalRate = pPlayer->GetCriticalRate();
		CriticalDamagePlus = pPlayer->GetCriticalDamagePlus();
		CriticalDamageRate = pPlayer->GetCriticalDamageRate();

		Weapon = pPlayer->GetItemBaseStats().mPhysicAttack.mPlus;
		OriginalStragnth = (float)pPlayer->GetRealStrength();
		// 100112 ONS 물리공격에 힘적용.
		Strength		 = (float)pPlayer->GetStrength();

		bLevelPenalty = TRUE;
	}
	else if( pAttacker->GetObjectKind() & eObjectKind_Monster )
	{
		CMonster* pMonster = (CMonster*)pAttacker;
		const BASE_MONSTER_LIST& baseMonsterList = pMonster->GetSMonsterList();

		float accuracy = baseMonsterList.Accuracy + pMonster->GetBuffStatus()->Accuracy;
		if( accuracy > 0 )
		{
			Accuracy = accuracy;
		}

		float min = baseMonsterList.AttackPhysicalMin + pMonster->GetBuffStatus()->PhysicAttack;
		float max = baseMonsterList.AttackPhysicalMax + pMonster->GetBuffStatus()->PhysicAttack;

		if( min < 0 ) min = 0;
		if( max < 0 ) max = 0;

		Attack = random( min, max );
			
		float criticalRate = baseMonsterList.CriticalPercent + pMonster->GetBuffStatus()->CriticalRate;

		if( criticalRate )
		{
			CriticalRate = criticalRate;
		}
	
		CriticalDamageRate = pMonster->GetBuffStatus()->CriticalDamage;
	}
	else if( pAttacker->GetObjectKind() == eObjectKind_Pet )
	{
		CPet* pPet = ( CPet* )pAttacker;

		PlayerStat& pStat = pPet->GetStat();
		Accuracy += pStat.mAccuracy.mPlus;

		int iMax	=	(int)pPet->GetPhysicAttackMax();
		int	iMin	=	(int)pPet->GetPhysicAttackMin();

		float	fTempAttack	=	0.0f;

		if( iMax > iMin )
		{
			fTempAttack = float( random( iMin, iMax ) );
		}
		else
		{
			fTempAttack = float( iMin );
		}

		Attack	=	( pPet->CalcPhysicAttack( fTempAttack ) ) * PETMGR->GetFriendlyPenalty( pPet );

		Strength = pStat.mStrength.mPlus;
		Weapon	=	pPet->GetItemStat().mPhysicAttack.mPlus;

		ITEMBASE* pitembase = pPet->GetWearedItem( ePetEquipSlot_Weapon );

		if( pitembase )
		{
			ITEM_INFO* piteminfo = ITEMMGR->GetItemInfo( pitembase->wIconIdx );

			if( piteminfo )
			{
				Weapon = float(piteminfo->PhysicAttack);
			}
		}

		CriticalRate = pStat.mCriticalRate.mPlus;
		CriticalDamageRate = pStat.mCriticalDamage.mPlus;

		bLevelPenalty = TRUE;
	}
	else
	{
		ASSERT(0);
	}

	if( pTarget->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pTarget;
		
		Avoid = pPlayer->GetAvoid();
		Defense = pPlayer->GetPhysicDefense();
		Block = pPlayer->GetBlock();

		// 080910 LUJ, 플레이어가 장착한 방패의 방어력을 가져옴
		Shield		= float( pPlayer->GetShieldDefense() );
		vitality	= float( pPlayer->GetVitality() );

		if( pAttacker->GetObjectKind() == eObjectKind_Player )	//때린 유저가 사람
		{
			CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
			if( pAttackPlayer->IsPKMode() )		//PK유저가 사람을 때렸으면
				pAttackPlayer->SetPKStartTimeReset();

			if( pPlayer->IsPKMode() )			//PK유저가 사람한테 맞았으면
				pPlayer->SetPKStartTimeReset();
		}

	}
	// 090316 LUJ, 탈것은 명중시킬 수 없다. 주의! 몬스터 여부를 비트 연산로 검사한다.
	//			몬스터 다음에 비교문이 위치할 경우, 몬스터로 체크되어 공격받게 된다.
	else if( pTarget->GetObjectKind() == eObjectKind_Vehicle )
	{
		pDamageInfo->bDodge = TRUE;
		pDamageInfo->RealDamage = 0;
		return;
	}
	else if( pTarget->GetObjectKind() & eObjectKind_Monster )
	{
		CMonster* pMonster = (CMonster*)pTarget;
		const BASE_MONSTER_LIST& baseMonsterList = pMonster->GetSMonsterList();

		float avoid = baseMonsterList.Avoid + pMonster->GetBuffStatus()->Avoid;
		if( avoid > 0 )
			Avoid = avoid;
		Defense = max(baseMonsterList.PhysicalDefense + pMonster->GetBuffStatus()->PhysicDefense, 0);
	}
	else if( pTarget->GetObjectKind() == eObjectKind_Pet )
	{
		CPet* pPet = (CPet*)pTarget;
		PlayerStat& pStat = pPet->GetStat();

		vitality = pStat.mVitality.mPlus;
		Avoid = pStat.mEvade.mPlus;
		Defense = pStat.mPhysicDefense.mPlus;
		Block = 0;

		Defense *= PETMGR->GetFriendlyPenalty( pPet );
		if( pAttacker->GetObjectKind() == eObjectKind_Player )	//때린 유저가 사람
		{
			CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
			if( pAttackPlayer->IsPKMode() )		//PK유저가 사람을 때렸으면
				pAttackPlayer->SetPKStartTimeReset();
		}
	}
	else
	{
		ASSERT(0);
	}

	// 어그로에 대한 회피 패널티
	const DWORD dwAggroNum = pTarget->GetAggroNum();
	if( dwAggroNum >= 3 )
	{
		const float fAggroPanalty = (float)pow( (double)( dwAggroNum - 2 ), 2 );
		Avoid = max( Avoid * ( 1 - ( fAggroPanalty * 0.01f ) ), 0 );
	}

	// 명중률 = 85 + 명중 - 회피
	const float ACC = 85.f + Accuracy - Avoid;
	if( ACC < float( ( rand() % 100 ) + 1 ) )
	{
		pDamageInfo->bDodge = TRUE;
		pDamageInfo->RealDamage = 0;
		return;
	}

	// 어그로에 대한 방어력 패널티
	if( dwAggroNum >= 6 )
	{
		const float fAggroPanalty = (float)pow( (double)( dwAggroNum - 5 ), 1.5 );
		Defense = max( Defense * ( 1 - ( fAggroPanalty * 0.01f ) ), 0 );
	}

	float damage = 1.f;
	if( AddType == 1 )
	{
		// ( 공격력 - 방어력 ) * ( ( 1000 + 스킬위력 + 스탯 ) / 1000 )
		damage = ( Attack - Defense ) * ( ( 1000.f + AddDamage + Strength ) / 1000.f );
	}
	else if( AddType == 2 )
	{
		// ( 공격력 - 방어력 ) * ( ( 1000 + 스킬위력 + 무기공격력 ) / 1000 )
		damage = ( Attack - Defense ) * ( ( 1000.f + AddDamage + Weapon ) / 1000.f );
	}

	damage = ( damage * ( 1 +  ( RateAddValue / 100 ) ) ) + PlusAddValue;

	// 방어자의 레벨이 더 높을 경우 레벨당 1.5%씩 데미지 감소
	const float fAttackerLevel = pAttacker->GetLevel();
	const float fTargetLevel = pTarget->GetLevel();
	if( fAttackerLevel < fTargetLevel )
	{
		damage = ( damage < 1 ? 1 : damage );
		damage = damage * ( 1 + ( fAttackerLevel - fTargetLevel ) * 0.015f );
	}

	// 블럭
	if( 0 < Shield && Block > float(rand() % 100) )
	{
		pDamageInfo->bBlock	= TRUE;
		damage = ( damage * ( 0.6f - ( vitality / 4000 ) ) ) - Shield;
	}
	// 100221 ShinJS --- 블럭인 경우 크리티컬 없음.
	else if( (CriticalRate >= 100.f || CriticalRate >= ( ( rand() % 100 ) + 1 )) )
	{
		damage = ( damage * 1.5f ) * ( 1 + ( CriticalDamageRate / 100 ) ) + CriticalDamagePlus;

		// 크리티컬인 경우
		pDamageInfo->bCritical = TRUE;
	}

	if( pTarget->GetObjectKind() == eObjectKind_Player && pAttacker->GetObjectKind() == eObjectKind_Player )
	{
		if(g_pServerSystem->GetMapNum() == GTMAPNUM)
			damage *= (this->GetGTDamageRatio() / 100);
	}

	// 080728 KTH -- 공성전 모드 일때
	if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  		SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
  		SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
	{
		damage *= (SIEGEWARFAREMGR->GetDecreaseDamageRatio() / 100 );
	}

	if( damage < 1 )
	{
		damage = 1;
	}

	pDamageInfo->RealDamage = (DWORD)damage;

	// 090204 LUJ, 카운트 조건이 있는 스킬을 업데이트한다
	{
		float count = Attack;

		switch( AddType )
		{
		case 1:
			{
				count += AddDamage;
				break;
			}
		case 2:
			{
				count *= ( 1.0f + AddDamage / 100.0f );
				break;
			}
		}

		pAttacker->RemoveBuffCount( eBuffSkillCountType_PhysicalHitVolumn, int( count ) );
		pTarget->RemoveBuffCount( eBuffSkillCountType_PhysicalDamageVolumn, int( count ) );
	}
}

// 080912 LUJ, 마법 크리티컬 공식 수정(기획: 송가람)
void CAttackManager::MagicAttack( CObject* pAttacker, CObject* pTarget, RESULTINFO* pDamageInfo, float Accuracy, float AddDamage, WORD AddType )
{
	if( !pAttacker || !pTarget )
		return;

	if( pTarget->GetAbnormalStatus()->IsGod )
	{
		return;
	}

	// 기본 수치 산출
	float Attack = 1;
	float Defense = 1;
	float CriticalRate = 0;
	float CriticalDamagePlus = 0;
	float CriticalDamageRate = 0;
	float Intelligence = 1;
	float Vitality = 0;
	float OriginalIntelligence = 0;
	float RateAddValue = 0;
	float PlusAddValue = 0;
	float Weapon = 1;
	// 080912 LUJ, 크리티컬 실패 계산에 사용하기 위해 추가
	float wisdom				= 0;
	float castingProtectValue	= pAttacker->GetBuffStatus()->CastingProtect;
	float castingProtectRate	= pAttacker->GetRateBuffStatus()->CastingProtect + 1.0f;

	if( pAttacker->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pAttacker;
		
		Attack = pPlayer->GetMagicAttack();
		Intelligence = float( pPlayer->GetIntelligence() );
		OriginalIntelligence = (float)pPlayer->GetRealIntelligence();

		CriticalRate = pPlayer->GetMagicCriticalRate();
		CriticalDamageRate = pPlayer->GetMagicCriticalDamageRate();
		CriticalDamagePlus = pPlayer->GetMagicCriticalDamagePlus();

		// 080912 LUJ, 크리티컬 실패 계산에 사용하기 위해 적용
		wisdom 				= 	float( pPlayer->GetWisdom() );
		castingProtectValue	+= 	pPlayer->GetPassiveStatus()->CastingProtect;
		castingProtectRate	+= 	pPlayer->GetRatePassiveStatus()->CastingProtect;

		Weapon = pPlayer->GetItemBaseStats().mMagicAttack.mPlus;
	}
	else if( pAttacker->GetObjectKind() & eObjectKind_Monster )
	{
		CMonster* pMonster = (CMonster*)pAttacker;
		const BASE_MONSTER_LIST& baseMonsterList = pMonster->GetSMonsterList();
		
		float min = baseMonsterList.AttackMagicalMin + pMonster->GetBuffStatus()->MagicAttack;
		float max = baseMonsterList.AttackMagicalMax + pMonster->GetBuffStatus()->MagicAttack;

		if( min < 0 ) min = 0;
		if( max < 0 ) max = 0;

		Attack = random( min, max );

		Weapon = float(baseMonsterList.Level * 2);
	}
	else if( pAttacker->GetObjectKind() == eObjectKind_Pet )
	{
		CPet* pPet = (CPet*)pAttacker;
		PlayerStat& pStat = pPet->GetStat();

		int iMax	=	(int)pPet->GetMagicAttackMax();
		int	iMin	=	(int)pPet->GetMagicAttackMin();

		float	fTempAttack	=	0.0f;

		if( iMax > iMin )
		{
			fTempAttack = float( random( iMin, iMax ) );
		}
		else
		{
			fTempAttack = float( iMin );
		}

		Attack	=	pPet->CalcMagicAttack( fTempAttack );

		Intelligence = pStat.mIntelligence.mPlus;
		
		ITEMBASE* pitembase = pPet->GetWearedItem( ePetEquipSlot_Weapon );

		if( pitembase )
		{
			ITEM_INFO* piteminfo = ITEMMGR->GetItemInfo( pitembase->wIconIdx );

			if( piteminfo )
			{
				Weapon = float(piteminfo->MagicAttack);
			}
		}

		Attack *= PETMGR->GetFriendlyPenalty( pPet );
		CriticalRate += pPet->GetPassiveStatus().MagicCriticalRate;

		Weapon	=	pPet->GetItemStat().mMagicAttack.mPlus;
	}
	else
	{
		ASSERT(0);
	}

	if( pTarget->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pTarget;
		
		Defense = pPlayer->GetMagicDefense();
		Vitality = (float)pPlayer->GetVitality();

		if( pAttacker->GetObjectKind() == eObjectKind_Player )	//때린 유저가 사람
		{
			CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
			if( pAttackPlayer->IsPKMode() )		//PK유저가 사람을 때렸으면
				pAttackPlayer->SetPKStartTimeReset();

			if( pPlayer->IsPKMode() )			//PK유저가 사람한테 맞았으면
				pPlayer->SetPKStartTimeReset();
		}
	}
	// 090316 LUJ, 탈것은 명중시킬 수 없다. 주의! 몬스터 여부를 비트 연산로 검사한다.
	//			몬스터 다음에 비교문이 위치할 경우, 몬스터로 체크되어 공격받게 된다.
	else if( pTarget->GetObjectKind() == eObjectKind_Vehicle )
	{
		Accuracy = 0;
	}
	else if( pTarget->GetObjectKind() & eObjectKind_Monster )
	{
		CMonster* pMonster = (CMonster*)pTarget;
		const BASE_MONSTER_LIST& baseMonsterList = pMonster->GetSMonsterList();

		Defense = max(baseMonsterList.MagicalDefense + pMonster->GetBuffStatus()->MagicDefense, 0);
	}
	else if( pTarget->GetObjectKind() == eObjectKind_Pet )
	{
		CPet* pPet = (CPet*)pTarget;
		PlayerStat& pStat = pPet->GetStat();

		Vitality = pStat.mVitality.mPlus;
		Defense = pStat.mMagicDefense.mPlus;
		Defense *= PETMGR->GetFriendlyPenalty( pPet );
		if( pAttacker->GetObjectKind() == eObjectKind_Player )	//때린 유저가 사람
		{
			CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
			if( pAttackPlayer->IsPKMode() )		//PK유저가 사람을 때렸으면
				pAttackPlayer->SetPKStartTimeReset();
		}
	}
	else
	{
		ASSERT(0);
	}

	float damage = 1.f;
	if( AddType == 1 )
	{
		// ( 공격력 - 방어력 ) * ( ( 1000 + 스킬위력 + 스탯 ) / 1000 )
		damage = ( Attack - Defense ) * ( ( 1000.f + AddDamage + Intelligence ) / 1000.f );
	}
	else if( AddType == 2 )
	{
		// ( 공격력 - 방어력 ) * ( ( 1000 + 스킬위력 + 무기공격력 ) / 1000 )
		damage = ( Attack - Defense ) * ( ( 1000.f + AddDamage + Weapon ) / 1000.f );
	}

	damage = ( damage * ( 1 +  ( RateAddValue / 100 ) ) ) + PlusAddValue;

	// 방어자의 레벨이 더 높을 경우 레벨당 1.5%씩 데미지 감소
	const float fAttackerLevel = pAttacker->GetLevel();
	const float fTargetLevel = pTarget->GetLevel();
	if( fAttackerLevel < fTargetLevel )
	{
		damage = ( damage < 1 ? 1 : damage );
		damage = damage * ( 1 + ( fAttackerLevel - fTargetLevel ) * 0.015f );
	}

	// 크리티컬인 경우
	if( CriticalRate >= 100.f || CriticalRate >= ( ( rand() % 100 ) + 1 ) )
	{
		damage = ( damage * 1.5f ) * ( 1 + ( CriticalDamageRate / 100 ) ) + CriticalDamagePlus;

		pDamageInfo->bCritical = TRUE;
	}

	if( pTarget->GetObjectKind() == eObjectKind_Player && pAttacker->GetObjectKind() == eObjectKind_Player )
	{
		if(g_pServerSystem->GetMapNum() == GTMAPNUM)
			damage *= (this->GetGTDamageRatio() / 100);
	}

	// 080728 KTH -- 공성전 모드 일때
	if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  		SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
  		SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
	{
		damage *= (SIEGEWARFAREMGR->GetDecreaseDamageRatio() / 100 );
	}

	if( damage < 1 )
	{
		damage = 1;
	}
	pDamageInfo->RealDamage = (DWORD)damage;

	// 090204 LUJ, 카운트 조건이 있는 스킬을 업데이트한다
	{
		float count = Attack;

		switch( AddType )
		{
		case 1:
			{
				count += AddDamage;
				break;
			}
		case 2:
			{
				count *= ( 1.0f + AddDamage / 100.0f );
				break;
			}
		}

		// 090204 LUJ, 카운트 조건이 있는 스킬을 업데이트한다
		pAttacker->RemoveBuffCount( eBuffSkillCountType_MagicalHitVolumn, int( count ) );
		pTarget->RemoveBuffCount( eBuffSkillCountType_MagicalDamageVolumn, int( count ) );
	}
}

void CAttackManager::SpecialAttack( CObject* pAttacker, CObject* pTarget, RESULTINFO* pDamageInfo, float Damage, WORD Type )
{
	if( !pAttacker || !pTarget )
		return;

	if( pTarget->GetAbnormalStatus()->IsGod )
	{
		return;
	}

	float damage = 0.f;
	float fTempDamage = 0.0f;

	if( Damage < 0 )
	{
		// 100226 ShinJS --- DOT 데미지 수정
		float fAttackStrength = 0.f;
		float fAttackIntelligense = 0.f;
		float fTargetVitality = 0.f;
		float fTargetWisdom = 0.f;
		float fAttackerLevel = (float)pAttacker->GetLevel();
		float fTargetLevel = (float)pTarget->GetLevel();

		switch( pAttacker->GetObjectKind() )
		{
		case eObjectKind_Player:
			{
				fAttackStrength = (float)((CPlayer*)pAttacker)->GetStrength();
				fAttackIntelligense = (float)((CPlayer*)pAttacker)->GetIntelligence();
			}
			break;
		case eObjectKind_Pet:
			{
				PlayerStat& pStat = ((CPet*)pTarget)->GetStat();
				fAttackStrength = (float)(pStat.mStrength.mPlus);
				fAttackIntelligense = (float)(pStat.mIntelligence.mPlus);
			}
			break;
		}

		switch( pTarget->GetObjectKind() )
		{
		case eObjectKind_Player:
			{
				fTargetVitality = (float)((CPlayer*)pTarget)->GetVitality();
				fTargetWisdom = (float)((CPlayer*)pTarget)->GetWisdom();
			}
			break;
		case eObjectKind_Pet:
			{
				PlayerStat& pStat = ((CPet*)pTarget)->GetStat();
				fTargetVitality = (float)(pStat.mVitality.mPlus);
				fTargetWisdom = (float)(pStat.mWisdom.mPlus);
			}
			break;
		}

		//BUFF DOT	= DOT_dmg * ( 1 + ( ( 공격자 힘 + 공격자 지능 - 방어자 건강 - 방어자 정신 ) / 3000 ) ) + ( 공격자 레벨 - 방어자 레벨 )									
		fTempDamage = ( -Damage * ( 1 + ( ( fAttackStrength + fAttackIntelligense - fTargetVitality - fTargetWisdom ) ) / 3000.f ) );
		
		if( fAttackerLevel <= fTargetLevel )
		{
			damage = fTempDamage + ( fAttackerLevel - fTargetLevel );
		}	
		else
		{
			damage = fTempDamage + 1;
		}

		// 090804 pdy 공성전시 디버프 데미지 감소 적용
		if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  			SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
  			SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
  		{
			damage *= (SIEGEWARFAREMGR->GetDecreaseDamageRatio() / 100 );
  		}

		// 080115 KTH -- PVP Damage Ratio Setting
		if( pTarget->GetObjectKind() == eObjectKind_Player && pAttacker->GetObjectKind() == eObjectKind_Player )
		{
			if(g_pServerSystem->GetMapNum() == GTMAPNUM)
				damage = damage * (this->GetGTDamageRatio() / 100);
			else
				damage = damage * (this->GetPvpDamageRatio() / 100);

			CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
			if( pAttackPlayer->IsPKMode() )			//PK유저가 사람을 때렸으면
				pAttackPlayer->SetPKStartTimeReset();

			CPlayer* pTargetPlayer = (CPlayer*)pTarget;
			if( pTargetPlayer->IsPKMode() )			//PK유저가 사람한테 맞았으면
				pTargetPlayer->SetPKStartTimeReset();
		}

		if( damage < 1 )
			damage = 1;

		pDamageInfo->RealDamage = (DWORD)damage;

		// 100310 ShinJS --- 데미지를 입는 대상이 마나실드를 사용중인 경우
		if( pTarget->HasEventSkill( eStatusKind_DamageToManaDamage ) )
		{
			// eStatusKind_DamageToManaDamage 에 대한 처리만 할수 있도록 한다.
			pTarget->Execute( CTakeDamageEvent( pAttacker, *pDamageInfo, eStatusKind_DamageToManaDamage ) );

			// Mana Damage
			pTarget->ManaDamage( pAttacker, pDamageInfo );
		}

		DWORD newLife = pTarget->Damage(pAttacker,pDamageInfo);

		if(newLife == 0)
		{
			// 080616 LUJ, 사망 시 이벤트를 처리하도록 한다
			// 080708 LUJ, 가해자/피해자를 모두 인자로 넘긴다
			pTarget->Execute( CDieEvent( pAttacker, pTarget ) );
			pAttacker->Execute( CKillEvent( pAttacker, pTarget ) );

			ATTACKMGR->sendDieMsg(pAttacker,pTarget);
			pTarget->Die(pAttacker);
		}
	}
	else
	{
		RecoverLife( pAttacker, pTarget, Damage, pDamageInfo );
	}
}

// 090204 LUJ, 피격 시 MP가 손실된다
void CAttackManager::BurnMana( CObject* offenseObject, CObject* defenseObject, const BUFF_SKILL_INFO* buffSkillInfo )
{
	if( ! offenseObject || ! defenseObject || ! buffSkillInfo )
	{
		return;
	}
	else if( defenseObject->GetAbnormalStatus()->IsGod )
	{
		return;
	}

	SKILL_INFO::Value damage = { 0 };

	// 090204 LUJ, 스킬에 따른 피해 수치를 구한다
	switch( buffSkillInfo->StatusDataType )
	{
	case BUFF_SKILL_INFO::StatusTypeAdd:
		{
			damage.mPlus += buffSkillInfo->StatusData;
			break;
		}
	case BUFF_SKILL_INFO::StatusTypePercent:
		{
			damage.mPlus += defenseObject->GetMaxMana() * buffSkillInfo->StatusData / 100.0f;
			break;
		}
	}

	// 090204 LUJ, MP 수치를 갱신한다
	{
		const float manaDamage	= damage.mPlus * ( 1.0f + damage.mPercent );
		const float currentMana = float( defenseObject->GetMana() ) + manaDamage;

		defenseObject->SetMana( DWORD( max( 0, currentMana ) ) );
	}
}
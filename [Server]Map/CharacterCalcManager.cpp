#include "stdafx.h"
#include "CharacterCalcManager.h"
#include "Player.h"
#include "CharMove.h"
#include "BossMonster.h"
#include "WeatherManager.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "ItemManager.h"
#include "MHFile.h"
#include "FishingManager.h"
#include "pet.h"
#include "MapDBMsgParser.h"
#include "..\[cc]header\GameResourceManager.h"
#include "UserTable.h"
#include "petmanager.h"

// 070410 LYW --- CCharacterCalcManager : Add struct for hp mp determination.
struct HPMP_DETERMINATE
{
	WORD	wClass_Idx ;

	BYTE	byHP_Value ;
	BYTE	byMP_Value ;
} ;


CCharacterCalcManager::CCharacterCalcManager()
{
	LoadHPMPDeterminateList();
	LoadJobSkillList();
}


CCharacterCalcManager::~CCharacterCalcManager()
{
	PTRLISTPOS pos = m_HPMPDeterminateList.GetHeadPosition();

	for(	HPMP_DETERMINATE* pDeterminate;
			(pDeterminate = (HPMP_DETERMINATE*)m_HPMPDeterminateList.GetNext(pos)) != NULL ;
			)
	{
		SAFE_DELETE( pDeterminate );
	}
}


CCharacterCalcManager* CCharacterCalcManager::GetInstance()
{
	static CCharacterCalcManager instance;

	return &instance;
}


void CCharacterCalcManager::StartUpdateLife( CObject* pObject, DWORD plusLife, BYTE recoverInterval, DWORD recoverTime )
{
	DWORD MaxVal = pObject->GetMaxLife();
	DWORD CurVal = pObject->GetLife();
	DWORD DamagedVal = MaxVal-CurVal;
	if(DamagedVal < plusLife)
		plusLife = DamagedVal;

	// 100624 ONS HP업데이트처리를 큐에 저장한다.
	// 쿨타임이 없는 물약의 경우, 연속사용시 직전의 남은 HP가 무시되어버리는 
	// 문제가 있으므로 큐에 저장해서 실행하도록 수정.(MP도 동일)
	if( DamagedVal > 0)
	{
		if( 0 == recoverTime )
		{
			// 100729 ONS 즉시 HP가 업데이트되는 경우 처리 추가.
			pObject->SetLifeRecoverDirectly( plusLife );
		}
		else
		{
			YYRECOVER_TIME YYLifeRecoverTime;
			YYLifeRecoverTime.bStart = TRUE;
			YYLifeRecoverTime.count = recoverInterval;											// recoverInterval번에 걸쳐 회복한다
			YYLifeRecoverTime.recoverUnitAmout = plusLife/YYLifeRecoverTime.count;				// 한번에 회복될 양
			YYLifeRecoverTime.recoverDelayTime = recoverTime/YYLifeRecoverTime.count;			// recoverTime초간 회복이 완료된다
			YYLifeRecoverTime.lastCheckTime = 0;												// 처음은 바로 회복한다.

			pObject->AddLifeRecoverTime( YYLifeRecoverTime );
		}
	}

	UpdateLife( pObject );
}

void CCharacterCalcManager::StartUpdateMana( CPlayer* pPlayer, DWORD plusMana, BYTE recoverInterval, DWORD recoverTime  )
{
	DWORD MaxVal = pPlayer->GetMaxMana();
	DWORD CurVal = pPlayer->GetMana();
	DWORD DamagedVal = MaxVal-CurVal;
	if(DamagedVal < plusMana)
		plusMana = DamagedVal;

	if( DamagedVal > 0)
	{
		if( 0 == recoverTime )
		{
			// 100729 ONS 즉시 MP가 업데이트되는 경우 처리 추가.
			pPlayer->SetManaRecoverDirectly( plusMana );
		}
		else
		{
			YYRECOVER_TIME YYManaRecoverTime;
			YYManaRecoverTime.bStart = TRUE;
			YYManaRecoverTime.count = recoverInterval;												// recoverInterval번에 걸쳐 회복한다
			YYManaRecoverTime.recoverUnitAmout = plusMana/YYManaRecoverTime.count;					// 한번에 회복될 양
			YYManaRecoverTime.recoverDelayTime = recoverTime/YYManaRecoverTime.count;				// recoverTime초간 회복이 완료된다
			YYManaRecoverTime.lastCheckTime = 0;													// 처음은 바로 회복한다.

			pPlayer->AddManaRecoverTime( YYManaRecoverTime );
		}
	}

	UpdateMana( pPlayer );
}
void CCharacterCalcManager::UpdateMana(CPlayer* pPlayer)
{
	if( pPlayer->GetState() == eObjectState_Die )
		return;

	pPlayer->UpdateMana();
}

void CCharacterCalcManager::UpdateLife(CObject* pObject)
{
	if( pObject->GetState() == eObjectState_Die )
		return;

	pObject->UpdateLife();
}

// 070410 LYW --- CStatsCalcManager : Add function to load HP & MP determination file.
BOOL CCharacterCalcManager::LoadHPMPDeterminateList()
{
	CMHFile fp ;

	if( ! fp.Init( "./System/Resource/HPMPdetermination.bin", "rb" ) )
	{
		MessageBox(NULL,"Loading failed!! ./System/Resource/HPMPdetermination.bin",0,0);
		return FALSE;
	}

	while( ! fp.IsEOF() )
	{
		HPMP_DETERMINATE* pDeterminate = new HPMP_DETERMINATE ;

		pDeterminate->wClass_Idx = fp.GetWord() ;
		pDeterminate->byHP_Value = fp.GetByte() ;
		pDeterminate->byMP_Value = fp.GetByte() ;

		m_HPMPDeterminateList.AddTail( pDeterminate ) ;
	}

	return TRUE ;
}

// 081022 KTH -- Load File Job Skill List
BOOL CCharacterCalcManager::LoadJobSkillList()
{
	CMHFile fp;

	if( !fp.Init( "./System/Resource/AddJobSkillList.bin", "rb" ) )
	{
		MessageBox(NULL, "Loading failed!! ./System/Resource/AddJobSkillList.bin", 0, 0);
		return FALSE;
	}

	while( ! fp.IsEOF() )
	{
		char szLine[256];
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//")) continue;	

		JobSkillInfo stJobSkillInfo;
		memset(&stJobSkillInfo, NULL, sizeof(JobSkillInfo));

		stJobSkillInfo.wClassIndex = fp.GetWord();
		stJobSkillInfo.dwSkillIndex = fp.GetDword();
		stJobSkillInfo.byPlusLevel = fp.GetByte();

		m_JobSkillList.push_back(stJobSkillInfo);
	}

	return TRUE;
}

void CCharacterCalcManager::AddPlayerJobSkill(CPlayer* pPlayer)
{
	//WORD wJobIndex[MAX_JOB_GRADE];
	CHARACTER_TOTALINFO pTotalInfo;
	pPlayer->GetCharacterTotalInfo( &pTotalInfo );

	pPlayer->ClearJobSkill();

	for( WORD grade = pTotalInfo.JobGrade; grade > 0; grade-- )
	{
		int jop = grade;

		if(  jop > 1 )
		{
			jop =  jop-1;
		}

		jop = pTotalInfo.Job[grade - 1];
		
		WORD wJobIndex = WORD( (pTotalInfo.Job[0]*1000) +
			( (pTotalInfo.Race + 1) * 100 ) +
			( grade *10 ) + jop );

		std::list<JobSkillInfo>::iterator BeginItr = m_JobSkillList.begin();

		for( ; BeginItr != m_JobSkillList.end(); BeginItr++ )
		{
			if( BeginItr->wClassIndex == wJobIndex )
			{
				pPlayer->AddJobSkill(BeginItr->dwSkillIndex, BeginItr->byPlusLevel);
				//pPlayer->AddSetSkill(BeginItr->dwSkillIndex, BeginItr->byPlusLevel);
			}
		}
	}
}

// 070410 LYW --- CStatsCalcManager : Add function to setting hp & mp determinate.
void CCharacterCalcManager::Init_HPMP_Point(CPlayer * pPlayer)
{
	// Setting class index.
	BYTE byFirst	= pPlayer->GetJop(0) ;
	BYTE bySecond	= pPlayer->GetRacial() + 1 ;	// Have to plus 1 cause of start index of racial is 1. ( from determination.bin )
	BYTE byThird	= pPlayer->GetJopGrade() ;	// Have to minus 1 cause of start index of jop array is 0.( from array of code. 
	BYTE byFourth	= 0;

	if( byThird == 1 )
	{
		byFourth = 1;
	}
	else
	{
		byFourth = pPlayer->GetJop(byThird - 1);
	}

	WORD wClassIdx	= ( byFirst * 1000 ) + ( bySecond * 100 ) + ( byThird * 10 ) + byFourth ;


	// Setting hp & mp determination value.
	PTRLISTPOS pos = m_HPMPDeterminateList.GetHeadPosition() ;

	while( pos )
	{
		HPMP_DETERMINATE* pDeterminate = (HPMP_DETERMINATE*)m_HPMPDeterminateList.GetNext(pos) ;

		if( pDeterminate )
		{
			if( pDeterminate->wClass_Idx == wClassIdx )
			{
				pPlayer->SetHP_Point(pDeterminate->byHP_Value) ;
				pPlayer->SetMP_Point(pDeterminate->byMP_Value) ;

				break ;
			}
		}
	}
}

// 070429 LYW --- CCharacterCalcManager : Add function to return hp&mp determinate.
BOOL CCharacterCalcManager::Get_HPMP_Point(CPlayer* pPlayer, BYTE* hpPoint, BYTE* mpPoint)
{
	// Setting class index.
	BYTE byFirst	= pPlayer->GetJop(0) ;
	BYTE bySecond	= pPlayer->GetRacial() + 1 ;	// Have to plus 1 cause of start index of racial is 1. ( from determination.bin )
	BYTE byThird	= pPlayer->GetJopGrade() ;	// Have to minus 1 cause of start index of jop array is 0.( from array of code. )
	BYTE byFourth	= 0;

	if( byThird == 1 )
	{
		byFourth = 1;
	}
	else
	{
		byFourth = pPlayer->GetJop(byThird - 1);
	}

	WORD wClassIdx	= ( byFirst * 1000 ) + ( bySecond * 100 ) + ( byThird * 10 ) + byFourth ;


	// Setting hp & mp determination value.
	PTRLISTPOS pos = m_HPMPDeterminateList.GetHeadPosition() ;

	while( pos )
	{
		HPMP_DETERMINATE* pDeterminate = (HPMP_DETERMINATE*)m_HPMPDeterminateList.GetNext(pos) ;

		if( pDeterminate )
		{
			if( pDeterminate->wClass_Idx == wClassIdx )
			{
				*hpPoint = pDeterminate->byHP_Value ;
				*mpPoint = pDeterminate->byMP_Value ;

				return TRUE ;
			}
		}
	}

	return FALSE ;
}

void CCharacterCalcManager::CalcMaxLife(CPlayer * pPlayer)
{
	// 레벨
	LEVELTYPE level = pPlayer->GetLevel() ;

	// 기본 스탯
	RaceType race			= (RaceType)pPlayer->GetRacial();
	BYTE byClass			=  pPlayer->m_HeroCharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );
	
	// 건강
	float fCurVital		= (float)pPlayer->GetVitality();
	float fBaseVitality	= stat.mVitality.mPlus;
	

	// %옵션
	float	fPercent	= pPlayer->GetRatePassiveStatus()->MaxLife
						+ pPlayer->GetRateBuffStatus()->MaxLife
						+ (pPlayer->GetItemOptionStats().mLife.mPercent * 100.f)
						+ (pPlayer->GetSetItemStats().mLife.mPercent * 100.f)
						+ (pPlayer->GetItemBaseStats().mLife.mPercent * 100.f);
	
	// +옵션
	float	fPlus		= pPlayer->GetPassiveStatus()->MaxLife
						+ pPlayer->GetBuffStatus()->MaxLife
						+ pPlayer->GetItemOptionStats().mLife.mPlus
						+ pPlayer->GetSetItemStats().mLife.mPlus
						+ pPlayer->GetItemBaseStats().mLife.mPlus;

	// HP					= ( ( 레벨 - 1 ) * 10 ) + ( ( 건강 - 초기건강 + 4 ) * 10 ) + ( 직업상수 * 레벨 * 7 )
	double	dTotalHP		= ( ( ( level - 1 ) * 10 ) + ( ( fCurVital - fBaseVitality + 4 ) * 10 ) + ( pPlayer->GetHP_Point() * level * 7 ) )
							* ( 1.f + ( fPercent / 100.f ) ) + fPlus;

	pPlayer->SetMaxLife(
		DWORD(max(1, dTotalHP)));
}


void CCharacterCalcManager::CalcMaxMana(CPlayer * pPlayer)
{
	// 레벨
	LEVELTYPE level = pPlayer->GetLevel() ;

	// 기본 스탯
	RaceType race			= (RaceType)pPlayer->GetRacial();
	BYTE byClass			=  pPlayer->m_HeroCharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );
	
	// 정신
	float fCurWisdom  = float( pPlayer->GetWisdom() );
	float fBaseWisdom	= stat.mWisdom.mPlus;

	// %옵션
	float	fPercent	= pPlayer->GetRatePassiveStatus()->MaxMana
						+ pPlayer->GetRateBuffStatus()->MaxMana
						+ (pPlayer->GetItemOptionStats().mMana.mPercent * 100.f)
						+ (pPlayer->GetSetItemStats().mMana.mPercent * 100.f)
						+ (pPlayer->GetItemBaseStats().mMana.mPercent * 100.f);
	
	// +옵션
	float	fPlus		= pPlayer->GetPassiveStatus()->MaxMana
						+ pPlayer->GetBuffStatus()->MaxMana
						+ pPlayer->GetItemOptionStats().mMana.mPlus
						+ pPlayer->GetSetItemStats().mMana.mPlus
						+ pPlayer->GetItemBaseStats().mMana.mPlus;

	// MP					= ( ( 레벨 - 1 ) * 10 ) + ( ( 정신 - 초기정신 + 4 ) * 10 ) + ( 직업상수 * 레벨 * 7 )
	double	dTotalMP		= ( ( ( level - 1 ) * 10 ) + ( ( fCurWisdom - fBaseWisdom + 4 ) * 10 ) + ( pPlayer->GetMP_Point() * level * 7 ) )
							* ( 1.f + ( fPercent / 100.f ) ) + fPlus;

	pPlayer->SetMaxMana(
		DWORD(max(1, dTotalMP)));
}


void CCharacterCalcManager::CalcCharPhyAttack( CPlayer* pPlayer )
{
//최대 물리 공격력	= ( ( 무기 공격력 + 레벨 ) * ( 1 + ( 힘 * 0.001 ) ) + ( 순수힘 - 30 ) ) * ( 1 + ( 무기 인챈트 레벨 ^ 2 / 400 ) )
//					* ( 1 +  ( 버프% + 세트% + 드랍옵션% / 100 ) )
//					+ 버프plus + 강화plus + 세트plus + 드랍옵션plus
	
//최소 물리 공격력	= (  ( 무기 공격력 + 레벨 ) * ( 1 + ( 힘 / 2.6 + 민첩 / 1.6 ) ) + ( 순수힘 - 30 ) ) * ( 1 + ( 무기 인챈트 레벨 ^ 2 / 400 ) )
//					* ( 1 +  ( 버프% + 세트% + 드랍옵션% / 100 ) )
//					+ 버프plus + 강화plus + 세트plus + 드랍옵션plus

	// 무기 공격력
	float	fWeaponAttack	= 0;
	{
		const PlayerStat::Value& value = pPlayer->GetItemBaseStats().mPhysicAttack;
		fWeaponAttack += value.mPlus * ( 1.0f + value.mPercent );
	}

	// 캐릭터 레벨
	WORD	wLevel			= pPlayer->GetLevel() ;
	float	AEnchant		= float( pPlayer->mWeaponEnchantLevel );

	// 힘
	float	fStrengthAll	= float( pPlayer->GetStrength() );

	// 순수 힘
	float	fRealStrengthAll= float( pPlayer->GetRealStrength() );

	// 민첩
	float	fDexterityAll	= float( pPlayer->GetDexterity() );

	// %옵션
	float	fPercent		= pPlayer->GetRatePassiveStatus()->PhysicAttack
							+ pPlayer->GetRateBuffStatus()->PhysicAttack
							+ (pPlayer->GetItemOptionStats().mPhysicAttack.mPercent * 100.f)
							+ (pPlayer->GetSetItemStats().mPhysicAttack.mPercent * 100.f);
	
	// +옵션
	float	fPlus			= pPlayer->GetPassiveStatus()->PhysicAttack
							+ pPlayer->GetBuffStatus()->PhysicAttack
							+ pPlayer->GetItemOptionStats().mPhysicAttack.mPlus
							+ pPlayer->GetSetItemStats().mPhysicAttack.mPlus;
	
	float	fMaxAttack		= ( ( ( fWeaponAttack + wLevel ) * ( 1 + ( fStrengthAll * 0.001f ) ) + ( fRealStrengthAll - 30 ) ) * ( 1 + ( AEnchant * AEnchant / 400.f ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	float	fMinAttack		= ( ( ( fWeaponAttack + wLevel ) * ( 1 + ( ( fStrengthAll / 2.6f + fDexterityAll / 1.6f ) * 0.001f ) ) + ( fRealStrengthAll - 30 ) ) * ( 1 + ( AEnchant * AEnchant / 400.f ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	pPlayer->mPhysicAttackMax = max( 0, fMaxAttack );

	if( fMinAttack > fMaxAttack )
	{
		fMinAttack = fMaxAttack;
	}

	pPlayer->mPhysicAttackMin = max( 0, fMinAttack );
}

void CCharacterCalcManager::CalcCharPhyDefense( CPlayer* pPlayer ) 
{
//물리 방어력	= ( ( 장비 방어력 + 레벨 ) + ( (순수건강 - 40) / 5 ) ) * ( 1 + ( 방어구 인챈트 레벨 총합 / 200 )
//				* ( 1 +  ( 버프% + 세트% + 드랍옵션% / 100 ) )
//				+ 버프plus + 강화plus + 세트plus + 드랍옵션plus

	// 방어구 방어력
	float fDefenseAll		= 0;

	{
		const PlayerStat::Value& value = pPlayer->GetItemBaseStats().mPhysicDefense;

		fDefenseAll = value.mPlus * ( 1.0f + value.mPercent );
	}

	// 100226 ShinJS --- 계열에 맞지 않은 장비를 착용했는지 검사(천/라이트/헤비)
	if( IsInvalidEquip( pPlayer ) )
		fDefenseAll = 0.f;

	// 캐릭터 레벨
	WORD	wLevel			= pPlayer->GetLevel();
	float	DEnchant		= float( pPlayer->mPhysicDefenseEnchantLevel );
	float	fRealVitalityAll	= float( pPlayer->GetRealVitality() );

	float	fVitality			=	(float)( pPlayer->GetVitality() );

	// %옵션
	float	fPercent		= pPlayer->GetRatePassiveStatus()->PhysicDefense
							+ pPlayer->GetRateBuffStatus()->PhysicDefense
							+ (pPlayer->GetItemOptionStats().mPhysicDefense.mPercent * 100.f)
							+ (pPlayer->GetSetItemStats().mPhysicDefense.mPercent * 100.f)
							+ pPlayer->GetRatePassiveStatus()->Shield
							+ pPlayer->GetRateBuffStatus()->Shield;
	
	// +옵션
	float	fPlus			= pPlayer->GetPassiveStatus()->PhysicDefense
							+ pPlayer->GetBuffStatus()->PhysicDefense
							+ pPlayer->GetItemOptionStats().mPhysicDefense.mPlus
							+ pPlayer->GetSetItemStats().mPhysicDefense.mPlus 
							+ pPlayer->GetPassiveStatus()->Shield
							+ pPlayer->GetBuffStatus()->Shield;

	// 물리 방어력			= ( ( 장비 방어력 + 레벨 )   * ( 1 + ( 건강 / 3000 ) )         + ( (순수건강 - 40) / 5 ) ) * ( 1 + ( 방어구 인챈트 레벨 총합 / 200 ) )
	float	fTotalDefense	= ( ( ( fDefenseAll + wLevel ) * ( 1.0f + ( fVitality / 3000 ) ) + ( ( fRealVitalityAll - 40 ) / 5 ) ) * ( 1 + DEnchant / 200 ) ) 
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	pPlayer->mPhysicDefense = max( 0, fTotalDefense );
}

void CCharacterCalcManager::CalcCharMagAttack( CPlayer* pPlayer ) 
{
//최대 마법 공격력	= ( ( 무기 공격력 + 레벨 ) * ( 1 + (지능 * 0.001) ) + ( 순수지능 - 40 ) ) * ( 1 + ( 무기 인챈트 레벨 ^ 2 / 400 ) )
//					* ( 1 +  ( 버프% + 세트% + 드랍옵션% / 100 ) )
//					+ 버프plus + 강화plus + 세트plus + 드랍옵션plus

//최소 마법 공격력	= (  ( 무기 공격력 + 레벨 ) * ( 1 + ( ( 지능 / 2.6 + 정신 / 1.6 ) * 0.001) ) + ( 순수지능 - 40 ) ) * ( 1 + ( 무기 인챈트 레벨 ^ 2 / 400 ) )
//					* ( 1 +  ( 버프% + 세트% + 드랍옵션% / 100 ) )
//					+ 버프plus + 강화plus + 세트plus + 드랍옵션plus

	// 무기 공격력
	float	fMagicAttack		= 0;

	{
		const PlayerStat::Value& value = pPlayer->GetItemBaseStats().mMagicAttack;

		fMagicAttack = value.mPlus * ( 1.0f + value.mPercent );
	}

	// 캐릭터 레벨
	WORD	wLevel			= pPlayer->GetLevel() ;
	float	AEnchant		= float( pPlayer->mWeaponEnchantLevel );

	// 지능
	float	fIntelligenceAll	= float( pPlayer->GetIntelligence() );

	// 순수 지능
	float	fRealIntelligenceAll= float( pPlayer->GetRealIntelligence() );

	// 정신
	float	fWisdomAll		= float( pPlayer->GetWisdom() );

	// %옵션
	float	fPercent		= pPlayer->GetRatePassiveStatus()->MagicAttack
							+ pPlayer->GetRateBuffStatus()->MagicAttack
							+ (pPlayer->GetItemOptionStats().mMagicAttack.mPercent * 100.f)
							+ (pPlayer->GetSetItemStats().mMagicAttack.mPercent * 100.f);
	
	// +옵션
	float	fPlus			= pPlayer->GetPassiveStatus()->MagicAttack
							+ pPlayer->GetBuffStatus()->MagicAttack
							+ pPlayer->GetItemOptionStats().mMagicAttack.mPlus
							+ pPlayer->GetSetItemStats().mMagicAttack.mPlus;


	float	fMaxAttack		= ( ( ( fMagicAttack + wLevel ) * ( 1 + ( fIntelligenceAll * 0.001f ) ) + ( fRealIntelligenceAll - 40 ) ) * ( 1 + ( AEnchant * AEnchant / 400.f ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	float	fMinAttack		= ( ( ( fMagicAttack + wLevel ) * ( 1 + ( ( fIntelligenceAll / 2.6f + fWisdomAll / 1.6f ) * 0.001f ) ) + ( fRealIntelligenceAll - 40 ) ) * ( 1 + ( AEnchant * AEnchant / 400.f ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	
	if( fMinAttack > fMaxAttack )
	{
		fMinAttack = fMaxAttack;
	}

	pPlayer->mMagicAttackMax = max( 0, fMaxAttack );
	pPlayer->mMagicAttackMin = max( 0, fMinAttack );

}

void CCharacterCalcManager::CalcCharMagDefense( CPlayer* pPlayer ) 
{
//마법 방어력	= ( ( 장비 방어력 + 레벨 ) * ( 1 + ( 정신 / 3000 ) ) + ( (순수정신 - 30) / 5 ) ) * ( 1 + ( 방어구 인챈트 레벨 총합 / 200 )
//				* ( 1 +  ( 버프% + 세트% + 드랍옵션% / 100 ) )
//				+ 버프plus + 강화plus + 세트plus + 드랍옵션plus

	// 방어구 방어력
	float fDefenseAll		= 0;

	{
		const PlayerStat::Value& value = pPlayer->GetItemBaseStats().mMagicDefense;

		fDefenseAll = value.mPlus * ( 1.0f + value.mPercent );
	}

	// 캐릭터 레벨
	WORD	wLevel			= pPlayer->GetLevel() ;
	float	DEnchant		= float( pPlayer->mMagicDefenseEnchantLevel );

	// 정신
	float	fWisdomAll		= float( pPlayer->GetWisdom() );
	float	fRealWisdomAll	= float( pPlayer->GetRealWisdom() );

	// %옵션
	float	fPercent		= pPlayer->GetRatePassiveStatus()->MagicDefense
							+ pPlayer->GetRateBuffStatus()->MagicDefense
							+ (pPlayer->GetItemOptionStats().mMagicDefense.mPercent * 100.f)
							+ (pPlayer->GetSetItemStats().mMagicDefense.mPercent * 100.f);
	
	// +옵션
	float	fPlus			= pPlayer->GetPassiveStatus()->MagicDefense
							+ pPlayer->GetBuffStatus()->MagicDefense
							+ pPlayer->GetItemOptionStats().mMagicDefense.mPlus
							+ pPlayer->GetSetItemStats().mMagicDefense.mPlus;


	float	fTotalDefense	= ( ( ( fDefenseAll + wLevel ) * ( 1 + ( fWisdomAll / 3000 ) ) + ( fRealWisdomAll / 5 ) ) * ( 1 + ( DEnchant / 200 ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	pPlayer->mMagicDefense = max( 0, fTotalDefense );
}

void CCharacterCalcManager::CalcCharAccuracy( CPlayer* pPlayer ) 
{
//명중	= ( ( ( 민첩 - 최초 민첩 ) / ( ( 레벨 - 1 ) * 5 ) ) * 0.6 + ( ( 최초 민첩 - 30 ) / 1000 ) + ( 레벨 * 명중상수 / 15000 ) * 100 )
//		* ( 1 +  ( 버프% + 세트% + 드랍옵션% / 100 ) )
//		+ 버프plus + 강화plus + 세트plus + 드랍옵션plus

	// 레벨
	DWORD wLevel		= pPlayer->GetLevel() ;

	// 기본 스탯
	RaceType race			= (RaceType)pPlayer->GetRacial();
	BYTE byClass			=  pPlayer->m_HeroCharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );
	
	// 민첩
	float fDexterityAll	= float( pPlayer->GetDexterity() );
	float fBaseDexterity	= stat.mDexterity.mPlus;
	float fDexterityRate	= ( wLevel > 1 ? max(fDexterityAll - fBaseDexterity, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// 직업상수
	float fClassOrderVal = 0 ;

	switch( pPlayer->m_HeroCharacterInfo.Job[0] )
	{
	case 1 : fClassOrderVal = 14.f ; break ;
	case 2 : fClassOrderVal = 18.f ; break ;
	case 3 : fClassOrderVal = 11.f ; break ;
	case 4 : fClassOrderVal = 15.f ; break ;
	}

	// %옵션
	float	fPercent		= pPlayer->GetRatePassiveStatus()->Accuracy
							+ pPlayer->GetRateBuffStatus()->Accuracy
							+ (pPlayer->GetItemOptionStats().mAccuracy.mPercent * 100.f)
							+ (pPlayer->GetSetItemStats().mAccuracy.mPercent * 100.f);
	
	// +옵션
	float	fPlus			= pPlayer->GetPassiveStatus()->Accuracy
							+ pPlayer->GetBuffStatus()->Accuracy
							+ pPlayer->GetItemOptionStats().mAccuracy.mPlus
							+ pPlayer->GetSetItemStats().mAccuracy.mPlus;

	float	fTotalAccuracy	= ( ( fDexterityRate * 0.6f + ( ( fBaseDexterity - 30.f ) / 1000.f ) + ( wLevel * fClassOrderVal / 15000.f ) ) * 100.f )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;
	
	pPlayer->mAccuracy = max( 0, fTotalAccuracy );

}

void CCharacterCalcManager::CalcCharEvasion( CPlayer* pPlayer ) 
{
//회피	= ( ( ( 민첩 - 최초 민첩 ) / ( ( 레벨 - 1 ) * 5 ) ) * 0.6 + (  ( 최초 민첩 - 30 ) / 1000 ) + ( 레벨 * 명중상수 / 15000 ) * 100 )
//		* ( 1 +  ( 버프% + 세트% + 드랍옵션% / 100 ) )
//		+ 버프plus + 강화plus + 세트plus + 드랍옵션plus

	// 레벨
	DWORD wLevel		= pPlayer->GetLevel() ;

	// 기본 스탯
	RaceType race			= (RaceType)pPlayer->GetRacial();
	BYTE byClass			=  pPlayer->m_HeroCharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );
	
	// 민첩
	float fDexterityAll	= float( pPlayer->GetDexterity() );
	float fBaseDexterity	= stat.mDexterity.mPlus;
	float fDexterityRate	= ( wLevel > 1 ? max(fDexterityAll - fBaseDexterity, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// 직업상수
	float fClassOrderVal = 0 ;

	switch( pPlayer->m_HeroCharacterInfo.Job[0] )
	{
	case 1 : fClassOrderVal = 14.f ; break ;
	case 2 : fClassOrderVal = 18.f ; break ;
	case 3 : fClassOrderVal = 11.f ; break ;
	case 4 : fClassOrderVal = 15.f ; break ;
	}

	// %옵션
	float	fPercent		= pPlayer->GetRatePassiveStatus()->Avoid
							+ pPlayer->GetRateBuffStatus()->Avoid
							+ (pPlayer->GetItemOptionStats().mEvade.mPercent * 100.f)
							+ (pPlayer->GetSetItemStats().mEvade.mPercent * 100.f);
	
	// +옵션
	float	fPlus			= pPlayer->GetPassiveStatus()->Avoid
							+ pPlayer->GetBuffStatus()->Avoid
							+ pPlayer->GetItemOptionStats().mEvade.mPlus
							+ pPlayer->GetSetItemStats().mEvade.mPlus;

	float	fTotalEvasion	= ( ( fDexterityRate * 0.6f + ( ( fBaseDexterity - 30.f ) / 1000.f ) + ( wLevel * fClassOrderVal / 15000.f ) ) * 100.f )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	pPlayer->mAvoid = max( 0, fTotalEvasion );
}

void CCharacterCalcManager::CalcCharCriticalRate( CPlayer* pPlayer ) 
{
//물리 크리티컬	 = ( 45 * ( 민첩 - 최초 민첩 ) / ( ( 레벨 - 1 ) * 5 ) ) + (  ( 민첩 - 25 ) / 5 )

	// 레벨
	WORD	wLevel		= pPlayer->GetLevel() ;

	// 기본 스탯
	RaceType race			= (RaceType)pPlayer->GetRacial();
	BYTE byClass			=  pPlayer->m_HeroCharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );
	
	// 민첩
	float fDexterityAll	= float( pPlayer->GetDexterity() );
	float fBaseDexterity	= stat.mDexterity.mPlus;
	float fDexterityRate	= ( wLevel > 1 ? max(fDexterityAll - fBaseDexterity, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	float	fPlus		= pPlayer->GetPassiveStatus()->CriticalRate
						+ pPlayer->GetBuffStatus()->CriticalRate
						+ pPlayer->GetItemOptionStats().mCriticalRate.mPlus
						+ pPlayer->GetSetItemStats().mCriticalRate.mPlus;
	// %옵션
	float	fPercent	= pPlayer->GetRatePassiveStatus()->CriticalRate
						+ pPlayer->GetRateBuffStatus()->CriticalRate
						+ (pPlayer->GetItemOptionStats().mCriticalRate.mPercent * 100.f)
						+ (pPlayer->GetSetItemStats().mCriticalRate.mPercent * 100.f);
	
	float fCritical			= ( 45.f * fDexterityRate ) + ( ( fBaseDexterity - 25 ) / 5.f ) + fPercent + ( fPlus / 45.f );
	
	pPlayer->mCriticalRate = max( 0, fCritical );

//	마법 크리티컬	 = ( 10 * ( 지능 - 최초 지능 ) / ( ( 레벨 - 1 ) * 5 ) ) + ( 20 * ( 정신 - 최초 정신 ) / ( ( 레벨 - 1 ) * 5 ) ) + (  ( 최초 지능 - 25 ) / 5 ) + 퍼센트 + ( 수치 / 45 )

	// 지능
	float	fIntelligenceAll	= float( pPlayer->GetIntelligence() );
	float fBaseIntelligence		= stat.mIntelligence.mPlus;
	float fIntelligenceRate		= ( wLevel > 1 ? max(fIntelligenceAll - fBaseIntelligence, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// 정신
	float	fWisdomAll			= float( pPlayer->GetWisdom() );
	float fBaseWisdomAll		= stat.mWisdom.mPlus;
	float fWisdomAllRate		= ( wLevel > 1 ? max(fWisdomAll - fBaseWisdomAll, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// %옵션
	fPlus				= pPlayer->GetPassiveStatus()->MagicCriticalRate
						+ pPlayer->GetBuffStatus()->MagicCriticalRate
						+ pPlayer->GetItemOptionStats().mMagicCriticalRate.mPlus
						+ pPlayer->GetSetItemStats().mMagicCriticalRate.mPlus;
	// %옵션
	fPercent			= pPlayer->GetRatePassiveStatus()->MagicCriticalRate
						+ pPlayer->GetRateBuffStatus()->MagicCriticalRate
						+ (pPlayer->GetItemOptionStats().mMagicCriticalRate.mPercent * 100.f)
						+ (pPlayer->GetSetItemStats().mMagicCriticalRate.mPercent * 100.f);
	
	fCritical				= ( 10.f * fIntelligenceRate ) + ( 20.f * fWisdomAllRate ) + ( ( fBaseIntelligence - 25 ) / 5.f ) + fPercent + ( fPlus / 45.f );
	
	pPlayer->mMagicCriticalRate = max( 0, fCritical );
}


void CCharacterCalcManager::CalcCharCriticalDamage( CPlayer* pPlayer ) 
{
	float	fPlus		= pPlayer->GetPassiveStatus()->CriticalDamage
						+ pPlayer->GetBuffStatus()->CriticalDamage
						+ pPlayer->GetItemOptionStats().mCriticalDamage.mPlus
						+ pPlayer->GetSetItemStats().mCriticalDamage.mPlus;
	// %옵션
	float	fPercent	= pPlayer->GetRatePassiveStatus()->CriticalDamage
						+ pPlayer->GetRateBuffStatus()->CriticalDamage
						+ (pPlayer->GetItemOptionStats().mCriticalDamage.mPercent * 100.f)
						+ (pPlayer->GetSetItemStats().mCriticalDamage.mPercent * 100.f);

	pPlayer->mCriticalDamagePlus = max(0, fPlus);
	pPlayer->mCriticalDamageRate = max(0, fPercent);

	fPlus				= pPlayer->GetPassiveStatus()->MagicCriticalDamage
						+ pPlayer->GetBuffStatus()->MagicCriticalDamage
						+ pPlayer->GetItemOptionStats().mMagicCriticalDamage.mPlus
						+ pPlayer->GetSetItemStats().mMagicCriticalDamage.mPlus;
	// %옵션
	fPercent			= pPlayer->GetRatePassiveStatus()->MagicCriticalDamage
						+ pPlayer->GetRateBuffStatus()->MagicCriticalDamage
						+ (pPlayer->GetItemOptionStats().mMagicCriticalDamage.mPercent * 100.f)
						+ (pPlayer->GetSetItemStats().mMagicCriticalDamage.mPercent * 100.f);

	pPlayer->mMagicCriticalDamagePlus = max(0, fPlus);
	pPlayer->mMagicCriticalDamageRate = max(0, fPercent);
}

void CCharacterCalcManager::CalcCharAddDamage( CPlayer* pPlayer )
{
	// %옵션
	float	fPercent		= pPlayer->GetRatePassiveStatus()->AddDamage
							+ pPlayer->GetRateBuffStatus()->AddDamage
							+ (pPlayer->GetItemOptionStats().mAddDamage.mPercent * 100.f)
							+ (pPlayer->GetSetItemStats().mAddDamage.mPercent * 100.f);
	// +옵션
	float	fPlus			= pPlayer->GetPassiveStatus()->AddDamage
							+ pPlayer->GetBuffStatus()->AddDamage
							+ pPlayer->GetItemOptionStats().mAddDamage.mPlus
							+ pPlayer->GetSetItemStats().mAddDamage.mPlus;

	pPlayer->mAddDamage = max( 0, fPlus );
	pPlayer->mRateAddDamage = max( 0, fPercent );
}

void CCharacterCalcManager::CalcCharReduceDamage( CPlayer* pPlayer )
{
	// %옵션
	float	fPercent		= pPlayer->GetRatePassiveStatus()->ReduceDamage
							+ pPlayer->GetRateBuffStatus()->ReduceDamage
							+ (pPlayer->GetItemOptionStats().mReduceDamage.mPercent * 100.f)
							+ (pPlayer->GetSetItemStats().mReduceDamage.mPercent * 100.f);
	// +옵션
	float	fPlus			= pPlayer->GetPassiveStatus()->ReduceDamage
							+ pPlayer->GetBuffStatus()->ReduceDamage
							+ pPlayer->GetItemOptionStats().mReduceDamage.mPlus
							+ pPlayer->GetSetItemStats().mReduceDamage.mPlus;

	pPlayer->mReduceDamage = max( 0, fPlus );
	pPlayer->mRateReduceDamage = max( 0, fPercent );
}

// 080910 LUJ, 플레이어의 방패 방어력을 계산
void CCharacterCalcManager::CalcCharShieldDefense(CPlayer* pPlayer)
{
	CItemSlot* slot = pPlayer->GetSlot( eItemTable_Weared );
	
	if( ! slot )
	{
		return;
	}

	const ITEMBASE*	itemBase = slot->GetItemInfoAbs( TP_WEAR_START + eWearedItem_Shield );
	
	if( ! itemBase )
	{
		return;
	}
	
	const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo( itemBase->wIconIdx );
	
	// 080910 LUJ, 방패가 장착되어 있지 않음
	if( !	itemInfo ||
			itemInfo->Part3DType != ePartType_Shield )
	{
		pPlayer->SetShieldDefence( 0 );
		return;
	}

	float defenseValue = float(itemInfo->PhysicDefense);
	const ITEM_OPTION& itemOption = ITEMMGR->GetOption(
		*itemBase);
	
	if(const EnchantScript* const script = g_CGameResourceManager.GetEnchantScript(itemOption.mEnchant.mIndex))
	{
		const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo(
			itemOption.mEnchant.mLevel);

		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it )
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypePhysicDefence:
				{
					defenseValue += ( itemOption.mEnchant.mLevel * itemInfo->EnchantDeterm );
					defenseValue += GetBonusEnchantValue( *itemInfo, itemOption, (DWORD)itemInfo->PhysicDefense, pEnchantLvBonusRate );
					break;
				}
			}
		}
	}
	
	pPlayer->SetShieldDefence( DWORD( defenseValue ) );
}

void CCharacterCalcManager::ProcessLife(CPlayer * pPlayer)
{
	if(pPlayer->GetState() == eObjectState_Die)
		return;

	DWORD life = pPlayer->GetLife();
	DWORD maxlife = pPlayer->GetMaxLife();

	if( life == maxlife )
	{
		return;
	}
	
	DWORD curTime = 0;
	if(!pPlayer->m_LifeRecoverTime.bStart)
	{
		curTime = gCurTime;
	}
	else
	{
		curTime = pPlayer->m_LifeRecoverTime.lastCheckTime = gCurTime;
		pPlayer->m_LifeRecoverTime.bStart = FALSE;
		return;
	}

	General_LifeCount(pPlayer, curTime, life);
}

void CCharacterCalcManager::General_LifeCount(CPlayer * pPlayer, DWORD curTime, DWORD life)
{
	// 건강
	float fCurVital  = (float)pPlayer->GetVitality();

	// 100301 ShinJS --- 서있는 경우와 앉아 있는경우 회복시간을 다르게 함.
	int nUpLife				= 0;
	DWORD TimeLength		= 0;

	// 앉아있는 경우
	if( pPlayer->GetState() == eObjectState_Rest )
	{
		// 회복량	= 건강 * 직업상수 * 0.125
		nUpLife		= (int)( fCurVital * pPlayer->GetHP_Point() * 0.125f );
		TimeLength	= 3000;
	}
	else
	{
		nUpLife		= (int)( ( fCurVital - 20 ) * 0.3f );
		TimeLength	= 5000;
	}

	if(curTime - pPlayer->m_LifeRecoverTime.lastCheckTime > TimeLength)
	{
		// %옵션
		float	fPercent	= pPlayer->GetRatePassiveStatus()->LifeRecoverRate
							+ pPlayer->GetRateBuffStatus()->LifeRecoverRate
							+ (pPlayer->GetItemOptionStats().mRecoveryLife.mPercent * 100.f)
							+ (pPlayer->GetSetItemStats().mRecoveryLife.mPercent * 100.f);

		// +옵션
		float	fPlus		= pPlayer->GetPassiveStatus()->LifeRecoverRate
							+ pPlayer->GetBuffStatus()->LifeRecoverRate
							+ pPlayer->GetItemOptionStats().mRecoveryLife.mPlus
							+ pPlayer->GetSetItemStats().mRecoveryLife.mPlus;

		nUpLife = (int)(nUpLife * ( 1 +  ( fPercent / 100 ) ) + fPlus);

		if( nUpLife < 1 )
		{
			nUpLife = 1 ;
		}

		DWORD dwCalcLife = (DWORD)( life + nUpLife );

		pPlayer->SetLife(dwCalcLife);
		pPlayer->m_LifeRecoverTime.lastCheckTime = curTime;
	}
}

void CCharacterCalcManager::ProcessPetLife(CPet * pPet)
{
	if(pPet->GetState() == eObjectState_Die)
		return;

	DWORD life = pPet->GetLife();
	DWORD maxlife = pPet->GetMaxLife();

	if( life == maxlife )
	{
		return;
	}
	
	DWORD curTime = 0;
	if(!pPet->m_LifeRecoverTime.bStart)
	{
		curTime = gCurTime;
	}
	else
	{
		curTime = pPet->m_LifeRecoverTime.lastCheckTime = gCurTime;
		pPet->m_LifeRecoverTime.bStart = FALSE;
		return;
	}

	General_PetLifeCount(pPet, curTime, life);
}

void CCharacterCalcManager::General_PetLifeCount(CPet * pPet, DWORD curTime, DWORD life)
{
	CPlayer* pPlayer	=	(CPlayer*)g_pUserTable->FindUser( pPet->GetOwnerIndex() );

	if( !pPlayer )
		return;

	BYTE idx = BYTE(pPet->GetObjectInfo().Type) * 10 + pPet->GetObjectInfo().Grade;
	PET_HP_MP_INFO*	pHPMPInfo = PETMGR->GetHPMPInfo( idx );

	DWORD TimeLength = 0;

	if( pPlayer->GetState() == eObjectState_Rest )
		TimeLength	=	3000;
	else
		TimeLength	=	5000;

	if(curTime - pPet->m_LifeRecoverTime.lastCheckTime > TimeLength)
	{
		//HP 회복	= (건강 - 20) * 0.3		5초
		
		DWORD dwUpLife	=	0;

		if( pPlayer->GetState() == eObjectState_Rest )
			dwUpLife	=	(DWORD)( pPet->mPetStat.mVitality.mPlus * pHPMPInfo->HP * 0.125f );
		else
			dwUpLife	=	(DWORD)( ( pPet->mPetStat.mVitality.mPlus - 20.0f ) * 0.3f );

		if( dwUpLife <= 0 )
		{
			dwUpLife = 1 ;
		}

		// % 옵션
		float fPercent	=	( ( pPet->GetRatePassiveStatus().LifeRecoverRate + 
							pPet->GetRateBuffStatus()->LifeRecoverRate ) * 0.01f )   +
							pPet->GetItemOptionStat().mRecoveryLife.mPercent;

		// + 옵션
		float fPlus		=	pPet->GetPassiveStatus().LifeRecoverRate +
							pPet->GetBuffStatus()->LifeRecoverRate   +
							pPet->GetItemOptionStat().mRecoveryLife.mPlus;

		dwUpLife	=	(DWORD)( ( dwUpLife * ( 1 + fPercent ) ) + fPlus );

		DWORD dwCalcLife = life + dwUpLife ;
				
		pPet->SetLife(dwCalcLife);
		pPet->m_LifeRecoverTime.lastCheckTime = curTime;
	}
}

void CCharacterCalcManager::General_LifeCountBoss(CBossMonster * pBoss, DWORD curTime, DWORD life, DWORD maxlife)
{
	// 	평상시 회복 (5초당) 체력 : 전체의 1%
	if(curTime - pBoss->m_LifeRecoverTime.lastCheckTime > 5000)
	{
		pBoss->SetLife(life+(DWORD)(maxlife*0.001f), TRUE);
		pBoss->m_LifeRecoverTime.lastCheckTime = curTime;
	}
}

void CCharacterCalcManager::ProcessLifeBoss(CBossMonster * pBoss)
{
	if(pBoss->GetState() == eObjectState_Die)
		return;

	DWORD life = pBoss->GetLife();
	DWORD maxlife = pBoss->GetMaxLife();

	if(life == maxlife) return;
	
	DWORD curTime = 0;
	if(!pBoss->m_LifeRecoverTime.bStart)
	{
		curTime = gCurTime;
	}
	else
	{
		curTime = pBoss->m_LifeRecoverTime.lastCheckTime = gCurTime;
		pBoss->m_LifeRecoverTime.bStart = FALSE;
		return;
	}

	General_LifeCountBoss(pBoss, curTime, life, maxlife);
}

void CCharacterCalcManager::ProcessMana(CPlayer * pPlayer)
{
	if(pPlayer->GetState() == eObjectState_Die)
		return;
	
	DWORD energy = pPlayer->GetMana();
	DWORD maxEnergy = pPlayer->GetMaxMana();

	if(energy == maxEnergy) return;

	DWORD curTime = 0;
	if(!pPlayer->m_ManaRecoverTime.bStart)
	{
		curTime = gCurTime;
	}
	else
	{
		curTime = pPlayer->m_ManaRecoverTime.lastCheckTime = gCurTime;
		pPlayer->m_ManaRecoverTime.bStart = FALSE;
		return;
	}

	General_ManaCount(pPlayer, curTime, energy);
}

void CCharacterCalcManager::General_ManaCount(CPlayer* pPlayer, DWORD curTime, DWORD energy)
{
	// 정신
	float fCurWisdom  = float( pPlayer->GetWisdom() );

	// 100301 ShinJS --- 서있는 경우와 앉아 있는경우 회복시간을 다르게 함.
	int nUpMana				= 0;
	DWORD TimeLength		= 0;

	// 앉아있는 경우
	if( pPlayer->GetState() == eObjectState_Rest )
	{
		// 회복량	= 정신 * 직업상수 * 0.125
		nUpMana	= (int)( fCurWisdom * pPlayer->GetMP_Point() * 0.125f );
		TimeLength	= 3000;
	}
	else
	{
		nUpMana	= (int)( ( fCurWisdom - 20 ) * 0.2f );
		TimeLength	= 6000;
	}

	if(curTime - pPlayer->m_ManaRecoverTime.lastCheckTime > TimeLength)
	{
		// %옵션
		float	fPercent	= pPlayer->GetRatePassiveStatus()->ManaRecoverRate
							+ pPlayer->GetRateBuffStatus()->ManaRecoverRate
							+ (pPlayer->GetItemOptionStats().mRecoveryMana.mPercent * 100.f)
							+ (pPlayer->GetSetItemStats().mRecoveryMana.mPercent * 100.f);

		// +옵션
		float	fPlus		= pPlayer->GetPassiveStatus()->ManaRecoverRate
							+ pPlayer->GetBuffStatus()->ManaRecoverRate
							+ pPlayer->GetItemOptionStats().mRecoveryMana.mPlus
							+ pPlayer->GetSetItemStats().mRecoveryMana.mPlus;

		nUpMana = (int)(nUpMana + ( 1 +  ( fPercent / 100 ) ) + fPlus);

		if( nUpMana < 1 )
		{
			nUpMana = 1 ;
		}

		DWORD dwCalcMana = (DWORD)( energy + nUpMana );
				
		pPlayer->SetMana(dwCalcMana);
		pPlayer->m_ManaRecoverTime.lastCheckTime = curTime;
	}
}


void CCharacterCalcManager::ProcessPetMana(CPet * pPet)
{
	if(pPet->GetState() == eObjectState_Die)
		return;
	
	DWORD energy = pPet->GetMana();
	DWORD maxEnergy = pPet->GetMaxMana();

	if(energy == maxEnergy) return;

	DWORD curTime = 0;
	if(!pPet->m_ManaRecoverTime.bStart)
	{
		curTime = gCurTime;
	}
	else
	{
		curTime = pPet->m_ManaRecoverTime.lastCheckTime = gCurTime;
		pPet->m_ManaRecoverTime.bStart = FALSE;
		return;
	}

	General_PetManaCount(pPet, curTime, energy);
}

void CCharacterCalcManager::General_PetManaCount(CPet* pPet, DWORD curTime, DWORD energy)
{
	// 070412 LYW --- 마나 증가 수정
	// 070412 LYW --- CharacterCalcManager : Modified function General_Mana.

	CPlayer* pPlayer	=	(CPlayer*)g_pUserTable->FindUser( pPet->GetOwnerIndex() );

	if( !pPlayer )
		return;

	BYTE idx = BYTE(pPet->GetObjectInfo().Type) * 10 + pPet->GetObjectInfo().Grade;
	PET_HP_MP_INFO*	pHPMPInfo = PETMGR->GetHPMPInfo( idx );

	DWORD TimeLength = 0;

	if( pPlayer->GetState()	== eObjectState_Rest )
		TimeLength = 3000;
	else
		TimeLength = 6000;


	if(curTime - pPet->m_ManaRecoverTime.lastCheckTime > TimeLength)
	{
		//MP 회복	= (정신 - 20) * 0.2		6초

		DWORD dwUpMana	=	0;

		if( pPlayer->GetState() == eObjectState_Rest )
			dwUpMana	=	(DWORD)( pPet->mPetStat.mWisdom.mPlus * pHPMPInfo->MP * 0.125f );
		else
			dwUpMana	=	(DWORD)( ( pPet->mPetStat.mWisdom.mPlus - 20.0f ) * 0.2f );

		if( dwUpMana <= 0 )  
		{
			dwUpMana = 1 ;
		}

		// % 옵션
		float	fPercent	=	( ( pPet->GetRatePassiveStatus().ManaRecoverRate +
								pPet->GetRateBuffStatus()->ManaRecoverRate )  * 0.01f )	 + 
								pPet->GetItemOptionStat().mRecoveryMana.mPercent;

		// + 옵션
		float	fPlus		=	pPet->GetPassiveStatus().ManaRecoverRate + 
								pPet->GetBuffStatus()->ManaRecoverRate   +
								pPet->GetItemOptionStat().mRecoveryMana.mPlus;

		dwUpMana	=	(DWORD)( ( dwUpMana * ( 1 + fPercent ) ) + fPlus );

		DWORD dwCalcMana = energy + dwUpMana ;
				
		pPet->SetMana(dwCalcMana);
		pPet->m_ManaRecoverTime.lastCheckTime = curTime;
	}
}

void CCharacterCalcManager::Initialize( CPlayer* player )
{
	// 세트 능력 초기화
	player->ResetSetItemStatus();

	PlayerStat& base_stat = player->GetItemBaseStats();
	PlayerStat& option_stat = player->GetItemOptionStats();
	ZeroMemory( &base_stat, sizeof( PlayerStat ) );
	ZeroMemory( &option_stat, sizeof( PlayerStat ) );

	typedef std::map< const SetScript*, DWORD > SetItemSize;
	SetItemSize									setItemSize;

	float* pFishItemRate = player->GetFishItemRate();
	for(POSTYPE part = TP_WEAR_START ; part < TP_WEAR_END ; ++part )
	{
		const ITEMBASE* item = ITEMMGR->GetItemInfoAbsIn(player, part);

		if(0 == item)
		{
			continue;
		}

		const ITEM_INFO * info = ITEMMGR->GetItemInfo(item->wIconIdx);

		if( ! info )
		{
			continue;
		}
		// 091211 LUJ, 이도류 마스터리가 없는 경우에 왼손에 무기가 있을 경우 적용하지 않는다
		//			이것은 버그 플레이이며, 무기를 교환하거나 새로 장착할 수는 없으므로
		//			초기화시에만 처리한다
		else if(TP_WEAR_START + eWearedItem_Shield == part)
		{
			const BOOL isNoShield = (info->Part3DType != ePartType_Shield);
			const BOOL isUnableTwoBlade = (player->GetHeroTotalInfo()->bUsingTwoBlade == FALSE);

			if(isNoShield &&
				isUnableTwoBlade)
			{
				continue;
			}
		}

		// 세트 아이템 능력 추가를 위해, 같은 종류의 세트 스크립트를 가진 아이템 개수도 조사해두자
		{
			const SetScript* script = GAMERESRCMNGR->GetSetScript( item->wIconIdx );

			if( script )
			{
				++setItemSize[ script ];

				// 081231 LUJ, 아이템의 추가 속성을 적용한다
				{
					const SetScript::Element& itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );

					for(	SetScript::Element::Skill::const_iterator skill_it = itemElement.mSkill.begin();
							itemElement.mSkill.end() != skill_it;
							++skill_it )
					{
						const DWORD	skillIndex	= skill_it->first;
						const BYTE	skillLevel	= skill_it->second;

						player->AddSetSkill(
							skillIndex,
							skillLevel );
					}

					SetPlayerStat(
						player->GetItemOptionStats(),
						itemElement.mStat,
						SetValueTypeAdd );
				}
			}
		}		

		const ITEM_OPTION& option = ITEMMGR->GetOption( *item );
		AddItemBaseStat( *info, option, base_stat );
		AddItemOptionStat( *info, option, option_stat );

		switch( info->EquipType )
		{
		case eEquipType_Weapon:
			{
				if( TP_WEAR_START + eWearedItem_Shield == part )
				{
					// 100831 ShinJS 이도류 왼손인 경우 평균값 저장
					player->mWeaponEnchantLevel = (DWORD)((player->mWeaponEnchantLevel + option.mEnchant.mLevel) / 2.f);
				}
				else
				{
					player->mWeaponEnchantLevel += option.mEnchant.mLevel;
				}
			}
			break;
		case eEquipType_Armor:
			{
				switch( info->EquipSlot )
				{
				case eWearedItem_Dress:
				case eWearedItem_Hat:
				case eWearedItem_Glove:
				case eWearedItem_Shoes:
				case eWearedItem_Belt:
				case eWearedItem_Band:
					{
						player->mPhysicDefenseEnchantLevel += option.mEnchant.mLevel;
					}
					break;
				}
			}
			break;
		case eEquipType_Accessary:
			{
				switch( info->EquipSlot )
				{
				case eWearedItem_Ring1:
				case eWearedItem_Ring2:
				case eWearedItem_Necklace:
				case eWearedItem_Earring1:
				case eWearedItem_Earring2:
					{
						player->mMagicDefenseEnchantLevel += option.mEnchant.mLevel;
					}
					break;
				}
			}
			break;
		}

		// 아이템 장착에 따른 낚시물고기확률 계산
		stFishingRate* pFishingRate = FISHINGMGR->GetFishingUtilityRate(info->ItemIdx);
		if(pFishingRate)
		{
			int i;
			for(i=0; i<MAX_FISHITEM; i++)
			{
				if(pFishingRate->FishList[i].nGrade < 0)
					continue;

				pFishItemRate[pFishingRate->FishList[i].nGrade] += pFishingRate->FishList[i].fRate;

				if(99.0f < pFishItemRate[pFishingRate->FishList[i].nGrade])
					pFishItemRate[pFishingRate->FishList[i].nGrade] = 99.0f;
			}
		}
	}

	// 세트 능력 적용. 낮은 단계부터 누적시키며 적용해야 한다
	for(
		SetItemSize::const_iterator it = setItemSize.begin();
		setItemSize.end() != it;
		++it )
	{
		const SetScript*	script	= it->first;
		const DWORD			size	= it->second;

		for( DWORD i = 1; i <= size; ++i )
		{
			AddSetItemStats( player, *script, i );
		}
	}

	CalcCharStats( player );
}


void CCharacterCalcManager::CalcCharStats(CPlayer* player)
{
	PlayerStat& char_stats = player->GetCharStats();
	ZeroMemory( &char_stats, sizeof( char_stats ) );

	HERO_TOTALINFO pHeroInfo;
	player->GetHeroTotalInfo( &pHeroInfo );
	
	char_stats.mDexterity.mPlus		= float( pHeroInfo.Dex );
	char_stats.mStrength.mPlus		= float( pHeroInfo.Str );
	char_stats.mVitality.mPlus		= float( pHeroInfo.Vit  );
	char_stats.mIntelligence.mPlus	= float( pHeroInfo.Int  );
	char_stats.mWisdom.mPlus		= float( pHeroInfo.Wis  );

	Init_HPMP_Point( player ) ;
	CalcMaxLife( player ) ;
	CalcMaxMana( player ) ;
	CalcCharPhyAttack( player ) ;
	CalcCharPhyDefense( player ) ;
	CalcCharMagAttack( player ) ;
	CalcCharMagDefense( player ) ;
	CalcCharAccuracy( player ) ;
	CalcCharEvasion( player ) ;
	CalcCharCriticalRate( player ) ;
	CalcCharCriticalDamage( player ) ;
	CalcCharAddDamage( player ) ;
	CalcCharReduceDamage( player ) ;
	// 080910 LUJ, 방패 방어력 계산
	CalcCharShieldDefense( player );
}


void CCharacterCalcManager::AddSetItemStats( CPlayer* player, const SetScript& script, int setItemSize )
{
	// 세트 아이템은 다음과 같이 적용된다. 세트로 구성되는 아이템 그룹들이 있고, 장비한 세트 아이템 개수에 따라 능력이 차례로 부여된다.

	// 세트 스크립트로 부여된 레벨을 가져와서 그 다음부터 조사
	// 세트 스크립트의 능력이 요구하는 세트 아이템 개수 조건을 충족하면 능력 부여

	typedef CPlayer::SetItemLevel SetItemLevel;

	CPlayer::SetItemLevel& setLevel = player->GetSetItemLevel();

	int& level = setLevel[ &script ];

	ASSERT( size_t( level ) < script.mAbility.size() );
	SetScript::Ability::const_iterator it = script.mAbility.begin();
	std::advance( it, level );

	for(	;
		script.mAbility.end() != it;
		++it, ++level )
	{
		const int size = it->first;

		if( size > setItemSize )
		{
			break;
		}

		// 능력을 부여하자
		{
			const SetScript::Element&	element = it->second;

			// 080313 LUJ, 세트 아이템 스탯에 처리
			AddStat( element.mStat, player->GetSetItemStats() );

			for(	SetScript::Element::Skill::const_iterator inner = element.mSkill.begin();
				element.mSkill.end() != inner;
				++inner )
			{
				const DWORD index	= inner->first;
				const BYTE	level	= BYTE( inner->second );

				player->AddSetSkill( index, level );
			}
		}
	}

	if( 0 == level )
	{
		setLevel.erase( &script );
	}
}


void CCharacterCalcManager::RemoveSetItemStats( CPlayer* player, const SetScript& script, int setItemSize )
{
	// 해당 플레이어가 가진 세트 스크립트로 부여된 레벨을 가져온다.
	// 있으면 거기부터 시작해서 아래로 조사. 없으면 그냥 끝
	// 캐릭터 세트 능력을 가져와 변경
	// 수치가 변화했으면 수치 재계산 요청

	typedef CPlayer::SetItemLevel SetItemLevel;

	CPlayer::SetItemLevel& setLevel = player->GetSetItemLevel();

	// 해당 스크립트로 저장된 레벨이 없다는 건 적용된 세트 능력이 없다는 뜻
	if( setLevel.end() == setLevel.find( &script ) )
	{
		return;
	}

	CPlayer::SetItemLevel::iterator level_it = setLevel.find( &script );

	if( setLevel.end() == level_it )
	{
		return;
	}

	// 080602 LUJ, 레벨 값을 참조로 받아야만 값이 바뀔 때 적용됨
	int& level = level_it->second;

	if( level > int( script.mAbility.size() ) )
	{
		return;
	}

	SetScript::Ability::const_iterator it = script.mAbility.begin();
	std::advance( it, level - 1 );

	for( ;; )	
	{
		const int size = it->first;

		if( size <= setItemSize )
		{
			break;
		}

		// 능력을 제거하자
		{
			const SetScript::Element&	element = it->second;
			
			// 080313 LUJ, 세트 아이템 스탯에 처리
			RemoveStat( element.mStat, player->GetSetItemStats() );

			for(
				SetScript::Element::Skill::const_iterator inner = element.mSkill.begin();
				element.mSkill.end() != inner;
				++inner )
			{
				player->RemoveSetSkill(
					inner->first,
					inner->second);
			}
		}

		--level;

		if( script.mAbility.begin() == it-- )
		{
			break;
		}
	}


	if( ! level )
	{
		setLevel.erase( &script );
	}
}

void CCharacterCalcManager::AddItem( CPlayer* player, const ITEMBASE& item )
{
	PlayerStat& base_stat = player->GetItemBaseStats();
	PlayerStat& option_stat = player->GetItemOptionStats();
	float* pFishItemRate = player->GetFishItemRate();

	const ITEM_INFO * info = ITEMMGR->GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	const ITEM_OPTION& option = ITEMMGR->GetOption( item );
	AddItemBaseStat( *info, option, base_stat );
	AddItemOptionStat( *info, option, option_stat );
	
	switch( info->EquipType )
	{
	case eEquipType_Weapon:
		{
			// 100831 ShinJS 왼손, 오른손 장비를 확인
			const ITEMBASE* weaponItem = ITEMMGR->GetItemInfoAbsIn( player, TP_WEAR_START + eWearedItem_Weapon );
			const ITEMBASE* shieldItem = ITEMMGR->GetItemInfoAbsIn( player, TP_WEAR_START + eWearedItem_Shield );
			
			const ITEM_INFO* weaponItemInfo	= ( weaponItem ? ITEMMGR->GetItemInfo( weaponItem->wIconIdx ) : 0 );
			const ITEM_INFO* shieldItemInfo	= ( shieldItem ? ITEMMGR->GetItemInfo( shieldItem->wIconIdx ) : 0 );

			DWORD weaponEnchantLevel = 0;

			if( weaponItemInfo != NULL )
			{
				const ITEM_OPTION& weaponItemOption = ITEMMGR->GetOption( *weaponItem );
				weaponEnchantLevel = weaponItemOption.mEnchant.mLevel;
				
			}

			// 왼손 장비가 무기인 경우
			if( shieldItemInfo != NULL && shieldItemInfo->EquipType == eEquipType_Weapon )
			{
				const ITEM_OPTION& shieldItemOption = ITEMMGR->GetOption( *shieldItem );
				weaponEnchantLevel = (DWORD)max((weaponEnchantLevel + shieldItemOption.mEnchant.mLevel) / 2.f, 0);
			}

			player->mWeaponEnchantLevel = weaponEnchantLevel;
		}
		break;
	case eEquipType_Armor:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Dress:
			case eWearedItem_Hat:
			case eWearedItem_Glove:
			case eWearedItem_Shoes:
			case eWearedItem_Belt:
			case eWearedItem_Band:
				{
					player->mPhysicDefenseEnchantLevel += option.mEnchant.mLevel;
				}
				break;
			}
		}
		break;
	case eEquipType_Accessary:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Ring1:
			case eWearedItem_Ring2:
			case eWearedItem_Necklace:
			case eWearedItem_Earring1:
			case eWearedItem_Earring2:
				{
					player->mMagicDefenseEnchantLevel += option.mEnchant.mLevel;
				}
				break;
			}
		}
		break;
	}

	// 세트 아이템 계산, 웅주 070613
	{
		const SetScript* setScript = GAMERESRCMNGR->GetSetScript( item.wIconIdx );

		if( setScript )
		{
			AddSetItemStats( player, *setScript, ITEMMGR->GetSetItemSize( player, setScript ) );

			// 081231 LUJ, 아이템의 추가 속성을 적용한다
			{
				const SetScript::Element& itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );

				for(	SetScript::Element::Skill::const_iterator skill_it = itemElement.mSkill.begin();
						itemElement.mSkill.end() != skill_it;
						++skill_it )
				{
					const DWORD	skillIndex	= skill_it->first;
					const BYTE	skillLevel	= skill_it->second;

					player->AddSetSkill(
						skillIndex,
						skillLevel );
				}

				SetPlayerStat(
					player->GetSetItemStats(),
					itemElement.mStat,
					SetValueTypeAdd );
			}
		}
	}

	// TODO : 아이템 장착
	player->PassiveSkillCheckForWeareItem();
	CalcCharStats( player );	

	// 아이템 장착에 따른 낚시물고기확률 계산
	stFishingRate* pFishingRate = FISHINGMGR->GetFishingUtilityRate(info->ItemIdx);
	if(pFishingRate)
	{
		int i;
		for(i=0; i<MAX_FISHITEM; i++)
		{
			if(pFishingRate->FishList[i].nGrade < 0)
				continue;

			pFishItemRate[pFishingRate->FishList[i].nGrade] += pFishingRate->FishList[i].fRate;

			if(99.0f < pFishItemRate[pFishingRate->FishList[i].nGrade])
				pFishItemRate[pFishingRate->FishList[i].nGrade] = 99.0f;
		}
	}
}


void CCharacterCalcManager::RemoveItem( CPlayer* player, const ITEMBASE& item )
{
	const ITEM_INFO * info = ITEMMGR->GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	PlayerStat& base_stat = player->GetItemBaseStats();
	PlayerStat& option_stat = player->GetItemOptionStats();
	float* pFishItemRate = player->GetFishItemRate();

	const ITEM_OPTION& option = ITEMMGR->GetOption( item );
	RemoveItemBaseStat( *info, option, base_stat );
	RemoveItemOptionStat( *info, option, option_stat );

	// 100219 ShinJS --- 인챈트 레벨 제거
	switch( info->EquipType )
	{
	case eEquipType_Weapon:
		{
			// 100831 ShinJS 왼손, 오른손 장비를 확인
			const ITEMBASE* weaponItem = ITEMMGR->GetItemInfoAbsIn( player, TP_WEAR_START + eWearedItem_Weapon );
			const ITEMBASE* shieldItem = ITEMMGR->GetItemInfoAbsIn( player, TP_WEAR_START + eWearedItem_Shield );
			
			const ITEM_INFO* weaponItemInfo	= ( weaponItem ? ITEMMGR->GetItemInfo( weaponItem->wIconIdx ) : 0 );
			const ITEM_INFO* shieldItemInfo	= ( shieldItem ? ITEMMGR->GetItemInfo( shieldItem->wIconIdx ) : 0 );

			DWORD weaponEnchantLevel = 0;

			if( weaponItemInfo != NULL )
			{
				const ITEM_OPTION& weaponItemOption = ITEMMGR->GetOption( *weaponItem );
				weaponEnchantLevel = weaponItemOption.mEnchant.mLevel;
				
			}

			// 왼손 장비가 무기인 경우
			if( shieldItemInfo != NULL && shieldItemInfo->EquipType == eEquipType_Weapon )
			{
				const ITEM_OPTION& shieldItemOption = ITEMMGR->GetOption( *shieldItem );
				weaponEnchantLevel = (DWORD)max((weaponEnchantLevel + shieldItemOption.mEnchant.mLevel) / 2.f, 0);
			}

			player->mWeaponEnchantLevel = weaponEnchantLevel;
		}
		break;
	case eEquipType_Armor:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Dress:
			case eWearedItem_Hat:
			case eWearedItem_Glove:
			case eWearedItem_Shoes:
			case eWearedItem_Belt:
			case eWearedItem_Band:
				{
					player->mPhysicDefenseEnchantLevel -= option.mEnchant.mLevel;
				}
				break;
			}
		}
		break;
	case eEquipType_Accessary:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Ring1:
			case eWearedItem_Ring2:
			case eWearedItem_Necklace:
			case eWearedItem_Earring1:
			case eWearedItem_Earring2:
				{
					player->mMagicDefenseEnchantLevel -= option.mEnchant.mLevel;
				}
				break;
			}
		}
		break;
	}

	// 세트 아이템 계산, 웅주 070613
	{
		const SetScript* setScript = GAMERESRCMNGR->GetSetScript( item.wIconIdx );

		if( setScript )
		{
			RemoveSetItemStats( player, *setScript, ITEMMGR->GetSetItemSize( player, setScript ) );

			// 081231 LUJ, 아이템의 추가 속성을 적용한다
			{
				const SetScript::Element& itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );

				for(	SetScript::Element::Skill::const_iterator skill_it = itemElement.mSkill.begin();
						itemElement.mSkill.end() != skill_it;
						++skill_it )
				{
					player->RemoveSetSkill(
						skill_it->first,
						skill_it->second);
				}

				SetPlayerStat(
					player->GetSetItemStats(),
					itemElement.mStat,
					SetValueTypeRemove );
			}
		}
	}

	// TODO : 아이템 해제
	player->PassiveSkillCheckForWeareItem();
	CalcCharStats( player );
	
	// 아이템 해제에 따른 낚시물고기확률 계산
	stFishingRate* pFishingRate = FISHINGMGR->GetFishingUtilityRate(info->ItemIdx);
	if(pFishingRate)
	{
		int i;
		for(i=0; i<MAX_FISHITEM; i++)
		{
			if(pFishingRate->FishList[i].nGrade < 0)
				continue;

			pFishItemRate[pFishingRate->FishList[i].nGrade] -= pFishingRate->FishList[i].fRate;

			if(pFishItemRate[pFishingRate->FishList[i].nGrade] < 0.0f)
				pFishItemRate[pFishingRate->FishList[i].nGrade] = 0.0f;
		}
	}
}


void CCharacterCalcManager::AddStat( const ITEM_INFO& info, PlayerStat& stat )
{
	stat.mPhysicAttack.mPlus	+= float( info.PhysicAttack );
	stat.mMagicAttack.mPlus		+= float( info.MagicAttack );
	stat.mPhysicDefense.mPlus	+= float( info.PhysicDefense );
	stat.mMagicDefense.mPlus	+= float( info.MagicDefense );
	stat.mStrength.mPlus		+= float( info.ImprovementStr );
	stat.mDexterity.mPlus		+= float( info.ImprovementDex );
	stat.mVitality.mPlus 		+= float( info.ImprovementVit );
	stat.mWisdom.mPlus 			+= float( info.ImprovementWis );
	stat.mIntelligence.mPlus	+= float( info.ImprovementInt );
	stat.mLife.mPlus 			+= float( info.ImprovementLife );
	stat.mMana.mPlus 			+= float( info.ImprovementMana );
}


// 080319 LUJ, 추가 인챈트 수치를 부여한다
// 080320 LUJ, 함수를 통해 값을 가져오도록 함
void CCharacterCalcManager::AddStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;

	stat.mPhysicAttack.mPlus	+= float( reinforce.mPhysicAttack );
	stat.mPhysicDefense.mPlus	+= float( reinforce.mPhysicDefence );
	stat.mMagicAttack.mPlus		+= float( reinforce.mMagicAttack );
	stat.mMagicDefense.mPlus	+= float( reinforce.mMagicDefence );
	stat.mStrength.mPlus		+= float( reinforce.mStrength		+ mix.mStrength );
	stat.mDexterity.mPlus		+= float( reinforce.mDexterity		+ mix.mDexterity );
	stat.mVitality.mPlus		+= float( reinforce.mVitality		+ mix.mVitality );
	stat.mWisdom.mPlus			+= float( reinforce.mWisdom			+ mix.mWisdom );
	stat.mIntelligence.mPlus	+= float( reinforce.mIntelligence	+ mix.mIntelligence );
	stat.mLife.mPlus			+= float( reinforce.mLife );
	stat.mMana.mPlus			+= float( reinforce.mMana );
	stat.mRecoveryLife.mPlus	+= float( reinforce.mLifeRecovery );
	stat.mRecoveryMana.mPlus	+= float( reinforce.mManaRecovery );
	stat.mCriticalRate.mPlus	+= float( reinforce.mCriticalRate );
	stat.mCriticalDamage.mPlus	+= float( reinforce.mCriticalDamage );
	stat.mAccuracy.mPlus		+= float( reinforce.mAccuracy );
	stat.mMoveSpeed.mPlus		+= float( reinforce.mMoveSpeed );
	stat.mEvade.mPlus			+= float( reinforce.mEvade );

	const ITEM_OPTION::Drop& drop = option.mDrop;

	for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
	{
		const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

		// 080410 LUJ, 실수 오차를 없애기 위해 소수점 세자리 아래는 절삭한다
		switch( value.mKey )
		{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				stat.mStrength.mPlus = Round( stat.mStrength.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				stat.mIntelligence.mPlus = Round( stat.mIntelligence.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				stat.mDexterity.mPlus = Round( stat.mDexterity.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				stat.mWisdom.mPlus = Round( stat.mWisdom.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				stat.mVitality.mPlus = Round( stat.mVitality.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				stat.mPhysicAttack.mPlus = Round( stat.mPhysicAttack.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				stat.mPhysicDefense.mPlus = Round( stat.mPhysicDefense.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				stat.mMagicAttack.mPlus = Round( stat.mMagicAttack.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				stat.mMagicDefense.mPlus = Round( stat.mMagicDefense.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				stat.mCriticalRate.mPlus = Round( stat.mCriticalRate.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				stat.mCriticalDamage.mPlus = Round( stat.mCriticalDamage.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				stat.mAccuracy.mPlus = Round( stat.mAccuracy.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				stat.mEvade.mPlus = Round( stat.mEvade.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				stat.mMoveSpeed.mPlus = Round( stat.mMoveSpeed.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				stat.mLife.mPlus = Round( stat.mLife.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				stat.mMana.mPlus = Round( stat.mMana.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				stat.mRecoveryLife.mPlus = Round( stat.mRecoveryLife.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				stat.mRecoveryMana.mPlus = Round( stat.mRecoveryMana.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				stat.mStrength.mPercent = Round( stat.mStrength.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				stat.mIntelligence.mPercent = Round( stat.mIntelligence.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				stat.mDexterity.mPercent = Round( stat.mDexterity.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				stat.mWisdom.mPercent = Round( stat.mWisdom.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				stat.mVitality.mPercent = Round( stat.mVitality.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				stat.mPhysicAttack.mPercent = Round( stat.mPhysicAttack.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				stat.mPhysicDefense.mPercent = Round( stat.mPhysicDefense.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				stat.mMagicAttack.mPercent = Round( stat.mMagicAttack.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				stat.mMagicDefense.mPercent = Round( stat.mMagicDefense.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				stat.mCriticalRate.mPercent = Round( stat.mCriticalRate.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				stat.mCriticalDamage.mPercent = Round( stat.mCriticalDamage.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				stat.mAccuracy.mPercent = Round( stat.mAccuracy.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				stat.mEvade.mPercent = Round( stat.mEvade.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				stat.mMoveSpeed.mPercent = Round( stat.mMoveSpeed.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				stat.mLife.mPercent = Round( stat.mLife.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				stat.mMana.mPercent = Round( stat.mMana.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				stat.mRecoveryLife.mPercent = Round( stat.mRecoveryLife.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				stat.mRecoveryMana.mPercent = Round( stat.mRecoveryMana.mPercent + value.mValue, 3 );
				break;
			}
		}
	}

	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript(
		option.mEnchant.mIndex);
	const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo(
		option.mEnchant.mLevel);

	if(script && option.mEnchant.mLevel)
	{
		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it)
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus += GetBonusEnchantValue( info, option, info.ImprovementStr, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus += GetBonusEnchantValue( info, option, info.ImprovementDex, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus += GetBonusEnchantValue( info, option, info.ImprovementVit, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus += GetBonusEnchantValue( info, option, info.ImprovementWis, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus += GetBonusEnchantValue( info, option, info.ImprovementInt, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus += GetBonusEnchantValue( info, option, info.ImprovementLife, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus += GetBonusEnchantValue( info, option, info.ImprovementMana, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus += GetBonusEnchantValue( info, option, info.PhysicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus += GetBonusEnchantValue( info, option, (DWORD)info.PhysicDefense, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus += GetBonusEnchantValue( info, option, info.MagicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus += GetBonusEnchantValue( info, option, (DWORD)info.MagicDefense, pEnchantLvBonusRate );
					break;
				}
			}
		}
	}
}


void CCharacterCalcManager::AddStat( const PlayerStat& srcStat, PlayerStat& destStat )
{
	destStat.mPhysicAttack.mPercent		+= srcStat.mPhysicAttack.mPercent;
	destStat.mPhysicAttack.mPlus		+= srcStat.mPhysicAttack.mPlus;

	destStat.mMagicAttack.mPercent		+= srcStat.mMagicAttack.mPercent;
	destStat.mMagicAttack.mPlus			+= srcStat.mMagicAttack.mPlus;

	destStat.mPhysicDefense.mPercent	+= srcStat.mPhysicDefense.mPercent;
	destStat.mPhysicDefense.mPlus		+= srcStat.mPhysicDefense.mPlus;

	destStat.mMagicDefense.mPercent		+= srcStat.mMagicDefense.mPercent;
	destStat.mMagicDefense.mPlus		+= srcStat.mMagicDefense.mPlus;

	destStat.mStrength.mPercent			+= srcStat.mStrength.mPercent;
	destStat.mStrength.mPlus			+= srcStat.mStrength.mPlus;

	destStat.mDexterity.mPercent 		+= srcStat.mDexterity.mPercent;
	destStat.mDexterity.mPlus 			+= srcStat.mDexterity.mPlus;

	destStat.mVitality.mPercent			+= srcStat.mVitality.mPercent;
	destStat.mVitality.mPlus			+= srcStat.mVitality.mPlus;

	destStat.mIntelligence.mPercent 	+= srcStat.mIntelligence.mPercent;
	destStat.mIntelligence.mPlus 		+= srcStat.mIntelligence.mPlus;

	destStat.mWisdom.mPercent 			+= srcStat.mWisdom.mPercent;
	destStat.mWisdom.mPlus 				+= srcStat.mWisdom.mPlus;

	destStat.mLife.mPercent				+= srcStat.mLife.mPercent;
	destStat.mLife.mPlus				+= srcStat.mLife.mPlus;

	destStat.mMana.mPercent				+= srcStat.mMana.mPercent;
	destStat.mMana.mPlus				+= srcStat.mMana.mPlus;

	destStat.mRecoveryLife.mPercent	+= srcStat.mRecoveryLife.mPercent;
	destStat.mRecoveryLife.mPlus		+= srcStat.mRecoveryLife.mPlus;

	destStat.mRecoveryMana.mPercent		+= srcStat.mRecoveryMana.mPercent;
	destStat.mRecoveryMana.mPlus		+= srcStat.mRecoveryMana.mPlus;

	destStat.mAccuracy.mPercent			+= srcStat.mAccuracy.mPercent;
	destStat.mAccuracy.mPlus			+= srcStat.mAccuracy.mPlus;

	destStat.mEvade.mPercent			+= srcStat.mEvade.mPercent;
	destStat.mEvade.mPlus				+= srcStat.mEvade.mPlus;

	destStat.mCriticalRate.mPercent		+= srcStat.mCriticalRate.mPercent;
	destStat.mCriticalRate.mPlus		+= srcStat.mCriticalRate.mPlus;

	destStat.mCriticalDamage.mPercent	+= srcStat.mCriticalDamage.mPercent;
	destStat.mCriticalDamage.mPlus		+= srcStat.mCriticalDamage.mPlus;

	destStat.mMoveSpeed.mPercent		+= srcStat.mMoveSpeed.mPercent;
	destStat.mMoveSpeed.mPlus			+= srcStat.mMoveSpeed.mPlus;
}


void CCharacterCalcManager::RemoveStat( const ITEM_INFO& info, PlayerStat& stat )
{
	stat.mPhysicAttack.mPlus	-= float( info.PhysicAttack );
	stat.mMagicAttack.mPlus		-= float( info.MagicAttack );
	stat.mPhysicDefense.mPlus	-= float( info.PhysicDefense );
	stat.mMagicDefense.mPlus	-= float( info.MagicDefense );
	stat.mStrength.mPlus		-= float( info.ImprovementStr );
	stat.mDexterity.mPlus		-= float( info.ImprovementDex );
	stat.mVitality.mPlus 		-= float( info.ImprovementVit );
	stat.mWisdom.mPlus 			-= float( info.ImprovementWis );
	stat.mIntelligence.mPlus	-= float( info.ImprovementInt );
	stat.mLife.mPlus 			-= float( info.ImprovementLife );
	stat.mMana.mPlus 			-= float( info.ImprovementMana );
}


// 080319 LUJ, 추가 인챈트 수치를 제거함
// 080320 LUJ, 제거할 추가 인챈트 수치를 함수로 가져옴
void CCharacterCalcManager::RemoveStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;

	stat.mPhysicAttack.mPlus	-= float( reinforce.mPhysicAttack );
	stat.mPhysicDefense.mPlus	-= float( reinforce.mPhysicDefence );
	stat.mMagicAttack.mPlus		-= float( reinforce.mMagicAttack );
	stat.mMagicDefense.mPlus	-= float( reinforce.mMagicDefence );
	stat.mStrength.mPlus		-= float( reinforce.mStrength		+ mix.mStrength );
	stat.mDexterity.mPlus		-= float( reinforce.mDexterity		+ mix.mDexterity );
	stat.mVitality.mPlus		-= float( reinforce.mVitality		+ mix.mVitality );
	stat.mWisdom.mPlus			-= float( reinforce.mWisdom			+ mix.mWisdom );
	stat.mIntelligence.mPlus	-= float( reinforce.mIntelligence	+ mix.mIntelligence );
	stat.mLife.mPlus			-= float( reinforce.mLife );
	stat.mMana.mPlus			-= float( reinforce.mMana );
	stat.mRecoveryLife.mPlus	-= float( reinforce.mLifeRecovery );
	stat.mRecoveryMana.mPlus	-= float( reinforce.mManaRecovery );
	stat.mCriticalRate.mPlus	-= float( reinforce.mCriticalRate );
	stat.mCriticalDamage.mPlus	-= float( reinforce.mCriticalDamage );
	stat.mAccuracy.mPlus		-= float( reinforce.mAccuracy );
	stat.mMoveSpeed.mPlus		-= float( reinforce.mMoveSpeed );
	stat.mEvade.mPlus			-= float( reinforce.mEvade );

	const ITEM_OPTION::Drop& drop = option.mDrop;

	for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
	{
		const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

		// 080410 LUJ, 실수 오차를 없애기 위해 소수점 세자리 아래는 절삭한다
		switch( value.mKey )
		{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				stat.mStrength.mPlus = Round( stat.mStrength.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				stat.mIntelligence.mPlus = Round( stat.mIntelligence.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				stat.mDexterity.mPlus = Round( stat.mDexterity.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				stat.mWisdom.mPlus = Round( stat.mWisdom.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				stat.mVitality.mPlus = Round( stat.mVitality.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				stat.mPhysicAttack.mPlus = Round( stat.mPhysicAttack.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				stat.mPhysicDefense.mPlus = Round( stat.mPhysicDefense.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				stat.mMagicAttack.mPlus = Round( stat.mMagicAttack.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				stat.mMagicDefense.mPlus = Round( stat.mMagicDefense.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				stat.mCriticalRate.mPlus = Round( stat.mCriticalRate.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				stat.mCriticalDamage.mPlus = Round( stat.mCriticalDamage.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				stat.mAccuracy.mPlus = Round( stat.mAccuracy.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				stat.mEvade.mPlus = Round( stat.mEvade.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				stat.mMoveSpeed.mPlus = Round( stat.mMoveSpeed.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				stat.mLife.mPlus = Round( stat.mLife.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				stat.mMana.mPlus = Round( stat.mMana.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				stat.mRecoveryLife.mPlus = Round( stat.mRecoveryLife.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				stat.mRecoveryMana.mPlus = Round( stat.mRecoveryMana.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				stat.mStrength.mPercent = Round( stat.mStrength.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				stat.mIntelligence.mPercent = Round( stat.mIntelligence.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				stat.mDexterity.mPercent = Round( stat.mDexterity.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				stat.mWisdom.mPercent = Round( stat.mWisdom.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				stat.mVitality.mPercent = Round( stat.mVitality.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				stat.mPhysicAttack.mPercent = Round( stat.mPhysicAttack.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				stat.mPhysicDefense.mPercent = Round( stat.mPhysicDefense.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				stat.mMagicAttack.mPercent = Round( stat.mMagicAttack.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				stat.mMagicDefense.mPercent = Round( stat.mMagicDefense.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				stat.mCriticalRate.mPercent = Round( stat.mCriticalRate.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				stat.mCriticalDamage.mPercent = Round( stat.mCriticalDamage.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				stat.mAccuracy.mPercent = Round( stat.mAccuracy.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				stat.mEvade.mPercent = Round( stat.mEvade.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				stat.mMoveSpeed.mPercent = Round( stat.mMoveSpeed.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				stat.mLife.mPercent = Round( stat.mLife.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				stat.mMana.mPercent = Round( stat.mMana.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				stat.mRecoveryLife.mPercent = Round( stat.mRecoveryLife.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				stat.mRecoveryMana.mPercent = Round( stat.mRecoveryMana.mPercent - value.mValue, 3 );
				break;
			}
		}
	}

	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript(
		option.mEnchant.mIndex);
	const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo(
		option.mEnchant.mLevel);

	if(script && option.mEnchant.mLevel)
	{
		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it)
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementStr, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementDex, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementVit, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementWis, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementInt, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementLife, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementMana, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus -= GetBonusEnchantValue( info, option, info.PhysicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus -= GetBonusEnchantValue( info, option, (DWORD)info.PhysicDefense, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus -= GetBonusEnchantValue( info, option, info.MagicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus -= GetBonusEnchantValue( info, option, (DWORD)info.MagicDefense, pEnchantLvBonusRate );
					break;
				}
			}
		}
	}
}


void CCharacterCalcManager::RemoveStat( const PlayerStat& srcStat, PlayerStat& destStat )
{
	destStat.mPhysicAttack.mPercent		-= srcStat.mPhysicAttack.mPercent;
	destStat.mPhysicAttack.mPlus		-= srcStat.mPhysicAttack.mPlus;

	destStat.mMagicAttack.mPercent		-= srcStat.mMagicAttack.mPercent;
	destStat.mMagicAttack.mPlus			-= srcStat.mMagicAttack.mPlus;

	destStat.mPhysicDefense.mPercent	-= srcStat.mPhysicDefense.mPercent;
	destStat.mPhysicDefense.mPlus		-= srcStat.mPhysicDefense.mPlus;

	destStat.mMagicDefense.mPercent		-= srcStat.mMagicDefense.mPercent;
	destStat.mMagicDefense.mPlus		-= srcStat.mMagicDefense.mPlus;

	destStat.mStrength.mPercent			-= srcStat.mStrength.mPercent;
	destStat.mStrength.mPlus			-= srcStat.mStrength.mPlus;

	destStat.mDexterity.mPercent 		-= srcStat.mDexterity.mPercent;
	destStat.mDexterity.mPlus 			-= srcStat.mDexterity.mPlus;

	destStat.mVitality.mPercent			-= srcStat.mVitality.mPercent;
	destStat.mVitality.mPlus			-= srcStat.mVitality.mPlus;

	destStat.mIntelligence.mPercent 	-= srcStat.mIntelligence.mPercent;
	destStat.mIntelligence.mPlus 		-= srcStat.mIntelligence.mPlus;

	destStat.mWisdom.mPercent 			-= srcStat.mWisdom.mPercent;
	destStat.mWisdom.mPlus 				-= srcStat.mWisdom.mPlus;

	destStat.mLife.mPercent				-= srcStat.mLife.mPercent;
	destStat.mLife.mPlus				-= srcStat.mLife.mPlus;

	destStat.mMana.mPercent				-= srcStat.mMana.mPercent;
	destStat.mMana.mPlus				-= srcStat.mMana.mPlus;

	destStat.mRecoveryLife.mPercent	-= srcStat.mRecoveryLife.mPercent;
	destStat.mRecoveryLife.mPlus		-= srcStat.mRecoveryLife.mPlus;

	destStat.mRecoveryMana.mPercent		-= srcStat.mRecoveryMana.mPercent;
	destStat.mRecoveryMana.mPlus		-= srcStat.mRecoveryMana.mPlus;

	destStat.mAccuracy.mPercent			-= srcStat.mAccuracy.mPercent;
	destStat.mAccuracy.mPlus			-= srcStat.mAccuracy.mPlus;

	destStat.mEvade.mPercent			-= srcStat.mEvade.mPercent;
	destStat.mEvade.mPlus				-= srcStat.mEvade.mPlus;

	destStat.mCriticalRate.mPercent		-= srcStat.mCriticalRate.mPercent;
	destStat.mCriticalRate.mPlus		-= srcStat.mCriticalRate.mPlus;

	destStat.mCriticalDamage.mPercent	-= srcStat.mCriticalDamage.mPercent;
	destStat.mCriticalDamage.mPlus		-= srcStat.mCriticalDamage.mPlus;

	destStat.mMoveSpeed.mPercent		-= srcStat.mMoveSpeed.mPercent;
	destStat.mMoveSpeed.mPlus			-= srcStat.mMoveSpeed.mPlus;
}

void CCharacterCalcManager::AddItemBaseStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	stat.mPhysicAttack.mPlus	+= float( info.PhysicAttack );
	stat.mMagicAttack.mPlus		+= float( info.MagicAttack );
	stat.mPhysicDefense.mPlus	+= float( info.PhysicDefense );
	stat.mMagicDefense.mPlus	+= float( info.MagicDefense );
	stat.mStrength.mPlus		+= float( info.ImprovementStr );
	stat.mDexterity.mPlus		+= float( info.ImprovementDex );
	stat.mVitality.mPlus 		+= float( info.ImprovementVit );
	stat.mWisdom.mPlus 			+= float( info.ImprovementWis );
	stat.mIntelligence.mPlus	+= float( info.ImprovementInt );
	stat.mLife.mPlus 			+= float( info.ImprovementLife );
	stat.mMana.mPlus 			+= float( info.ImprovementMana );
	
	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript(
		option.mEnchant.mIndex);
	const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo(
		option.mEnchant.mLevel);

	if( script && option.mEnchant.mLevel )
	{
		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it)
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mStrength.mPlus += GetBonusEnchantValue( info, option, info.ImprovementStr, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mDexterity.mPlus += GetBonusEnchantValue( info, option, info.ImprovementDex, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mVitality.mPlus += GetBonusEnchantValue( info, option, info.ImprovementVit, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mWisdom.mPlus += GetBonusEnchantValue( info, option, info.ImprovementWis, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mIntelligence.mPlus += GetBonusEnchantValue( info, option, info.ImprovementInt, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mLife.mPlus += GetBonusEnchantValue( info, option, info.ImprovementLife, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMana.mPlus += GetBonusEnchantValue( info, option, info.ImprovementMana, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mPhysicAttack.mPlus += GetBonusEnchantValue( info, option, info.PhysicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mPhysicDefense.mPlus += GetBonusEnchantValue( info, option, (DWORD)info.PhysicDefense, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMagicAttack.mPlus += GetBonusEnchantValue( info, option, info.MagicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMagicDefense.mPlus += GetBonusEnchantValue( info, option, (DWORD)info.MagicDefense, pEnchantLvBonusRate );
					break;
				}
			}
		}
	}
}

void CCharacterCalcManager::RemoveItemBaseStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	stat.mPhysicAttack.mPlus	-= float( info.PhysicAttack );
	stat.mMagicAttack.mPlus		-= float( info.MagicAttack );
	stat.mPhysicDefense.mPlus	-= float( info.PhysicDefense );
	stat.mMagicDefense.mPlus	-= float( info.MagicDefense );
	stat.mStrength.mPlus		-= float( info.ImprovementStr );
	stat.mDexterity.mPlus		-= float( info.ImprovementDex );
	stat.mVitality.mPlus 		-= float( info.ImprovementVit );
	stat.mWisdom.mPlus 			-= float( info.ImprovementWis );
	stat.mIntelligence.mPlus	-= float( info.ImprovementInt );
	stat.mLife.mPlus 			-= float( info.ImprovementLife );
	stat.mMana.mPlus 			-= float( info.ImprovementMana );
	
	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript(
		option.mEnchant.mIndex);
	const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo(
		option.mEnchant.mLevel);

	if( script && option.mEnchant.mLevel )
	{
		for(
			EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
		++it )
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mStrength.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementStr, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mDexterity.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementDex, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mVitality.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementVit, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mWisdom.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementWis, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mIntelligence.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementInt, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mLife.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementLife, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMana.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementMana, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mPhysicAttack.mPlus -= GetBonusEnchantValue( info, option, info.PhysicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mPhysicDefense.mPlus -= GetBonusEnchantValue( info, option, (DWORD)info.PhysicDefense, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMagicAttack.mPlus -= GetBonusEnchantValue( info, option, info.MagicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMagicDefense.mPlus -= GetBonusEnchantValue( info, option, (DWORD)info.MagicDefense, pEnchantLvBonusRate );
					break;
				}
			}
		}
	}
}

void CCharacterCalcManager::AddItemOptionStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;

	stat.mPhysicAttack.mPlus	+= float( reinforce.mPhysicAttack );
	stat.mPhysicDefense.mPlus	+= float( reinforce.mPhysicDefence );
	stat.mMagicAttack.mPlus		+= float( reinforce.mMagicAttack );
	stat.mMagicDefense.mPlus	+= float( reinforce.mMagicDefence );
	stat.mStrength.mPlus		+= float( reinforce.mStrength		+ mix.mStrength );
	stat.mDexterity.mPlus		+= float( reinforce.mDexterity		+ mix.mDexterity );
	stat.mVitality.mPlus		+= float( reinforce.mVitality		+ mix.mVitality );
	stat.mWisdom.mPlus			+= float( reinforce.mWisdom			+ mix.mWisdom );
	stat.mIntelligence.mPlus	+= float( reinforce.mIntelligence	+ mix.mIntelligence );
	stat.mLife.mPlus			+= float( reinforce.mLife );
	stat.mMana.mPlus			+= float( reinforce.mMana );
	stat.mRecoveryLife.mPlus	+= float( reinforce.mLifeRecovery );
	stat.mRecoveryMana.mPlus	+= float( reinforce.mManaRecovery );
	stat.mCriticalRate.mPlus	+= float( reinforce.mCriticalRate );
	stat.mCriticalDamage.mPlus	+= float( reinforce.mCriticalDamage );
	stat.mAccuracy.mPlus		+= float( reinforce.mAccuracy );
	stat.mMoveSpeed.mPlus		+= float( reinforce.mMoveSpeed );
	stat.mEvade.mPlus			+= float( reinforce.mEvade );

	const ITEM_OPTION::Drop& drop = option.mDrop;

	for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
	{
		const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

		// 080410 LUJ, 실수 오차를 없애기 위해 소수점 세자리 아래는 절삭한다
		switch( value.mKey )
		{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				stat.mStrength.mPlus = Round( stat.mStrength.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				stat.mIntelligence.mPlus = Round( stat.mIntelligence.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				stat.mDexterity.mPlus = Round( stat.mDexterity.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				stat.mWisdom.mPlus = Round( stat.mWisdom.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				stat.mVitality.mPlus = Round( stat.mVitality.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				stat.mPhysicAttack.mPlus = Round( stat.mPhysicAttack.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				stat.mPhysicDefense.mPlus = Round( stat.mPhysicDefense.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				stat.mMagicAttack.mPlus = Round( stat.mMagicAttack.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				stat.mMagicDefense.mPlus = Round( stat.mMagicDefense.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				stat.mCriticalRate.mPlus = Round( stat.mCriticalRate.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				stat.mCriticalDamage.mPlus = Round( stat.mCriticalDamage.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				stat.mAccuracy.mPlus = Round( stat.mAccuracy.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				stat.mEvade.mPlus = Round( stat.mEvade.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				stat.mMoveSpeed.mPlus = Round( stat.mMoveSpeed.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				stat.mLife.mPlus = Round( stat.mLife.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				stat.mMana.mPlus = Round( stat.mMana.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				stat.mRecoveryLife.mPlus = Round( stat.mRecoveryLife.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				stat.mRecoveryMana.mPlus = Round( stat.mRecoveryMana.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				stat.mStrength.mPercent = Round( stat.mStrength.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				stat.mIntelligence.mPercent = Round( stat.mIntelligence.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				stat.mDexterity.mPercent = Round( stat.mDexterity.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				stat.mWisdom.mPercent = Round( stat.mWisdom.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				stat.mVitality.mPercent = Round( stat.mVitality.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				stat.mPhysicAttack.mPercent = Round( stat.mPhysicAttack.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				stat.mPhysicDefense.mPercent = Round( stat.mPhysicDefense.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				stat.mMagicAttack.mPercent = Round( stat.mMagicAttack.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				stat.mMagicDefense.mPercent = Round( stat.mMagicDefense.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				stat.mCriticalRate.mPercent = Round( stat.mCriticalRate.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				stat.mCriticalDamage.mPercent = Round( stat.mCriticalDamage.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				stat.mAccuracy.mPercent = Round( stat.mAccuracy.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				stat.mEvade.mPercent = Round( stat.mEvade.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				stat.mMoveSpeed.mPercent = Round( stat.mMoveSpeed.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				stat.mLife.mPercent = Round( stat.mLife.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				stat.mMana.mPercent = Round( stat.mMana.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				stat.mRecoveryLife.mPercent = Round( stat.mRecoveryLife.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				stat.mRecoveryMana.mPercent = Round( stat.mRecoveryMana.mPercent + value.mValue, 3 );
				break;
			}
		}
	}

	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript(
		option.mEnchant.mIndex);

	if( script && option.mEnchant.mLevel )
	{
		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it )
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			}
		}
	}
}

void CCharacterCalcManager::RemoveItemOptionStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;

	stat.mPhysicAttack.mPlus	-= float( reinforce.mPhysicAttack );
	stat.mPhysicDefense.mPlus	-= float( reinforce.mPhysicDefence );
	stat.mMagicAttack.mPlus		-= float( reinforce.mMagicAttack );
	stat.mMagicDefense.mPlus	-= float( reinforce.mMagicDefence );
	stat.mStrength.mPlus		-= float( reinforce.mStrength		+ mix.mStrength );
	stat.mDexterity.mPlus		-= float( reinforce.mDexterity		+ mix.mDexterity );
	stat.mVitality.mPlus		-= float( reinforce.mVitality		+ mix.mVitality );
	stat.mWisdom.mPlus			-= float( reinforce.mWisdom			+ mix.mWisdom );
	stat.mIntelligence.mPlus	-= float( reinforce.mIntelligence	+ mix.mIntelligence );
	stat.mLife.mPlus			-= float( reinforce.mLife );
	stat.mMana.mPlus			-= float( reinforce.mMana );
	stat.mRecoveryLife.mPlus	-= float( reinforce.mLifeRecovery );
	stat.mRecoveryMana.mPlus	-= float( reinforce.mManaRecovery );
	stat.mCriticalRate.mPlus	-= float( reinforce.mCriticalRate );
	stat.mCriticalDamage.mPlus	-= float( reinforce.mCriticalDamage );
	stat.mAccuracy.mPlus		-= float( reinforce.mAccuracy );
	stat.mMoveSpeed.mPlus		-= float( reinforce.mMoveSpeed );
	stat.mEvade.mPlus			-= float( reinforce.mEvade );

	const ITEM_OPTION::Drop& drop = option.mDrop;

	for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
	{
		const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

		// 080410 LUJ, 실수 오차를 없애기 위해 소수점 세자리 아래는 절삭한다
		switch( value.mKey )
		{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				stat.mStrength.mPlus = Round( stat.mStrength.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				stat.mIntelligence.mPlus = Round( stat.mIntelligence.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				stat.mDexterity.mPlus = Round( stat.mDexterity.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				stat.mWisdom.mPlus = Round( stat.mWisdom.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				stat.mVitality.mPlus = Round( stat.mVitality.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				stat.mPhysicAttack.mPlus = Round( stat.mPhysicAttack.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				stat.mPhysicDefense.mPlus = Round( stat.mPhysicDefense.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				stat.mMagicAttack.mPlus = Round( stat.mMagicAttack.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				stat.mMagicDefense.mPlus = Round( stat.mMagicDefense.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				stat.mCriticalRate.mPlus = Round( stat.mCriticalRate.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				stat.mCriticalDamage.mPlus = Round( stat.mCriticalDamage.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				stat.mAccuracy.mPlus = Round( stat.mAccuracy.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				stat.mEvade.mPlus = Round( stat.mEvade.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				stat.mMoveSpeed.mPlus = Round( stat.mMoveSpeed.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				stat.mLife.mPlus = Round( stat.mLife.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				stat.mMana.mPlus = Round( stat.mMana.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				stat.mRecoveryLife.mPlus = Round( stat.mRecoveryLife.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				stat.mRecoveryMana.mPlus = Round( stat.mRecoveryMana.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				stat.mStrength.mPercent = Round( stat.mStrength.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				stat.mIntelligence.mPercent = Round( stat.mIntelligence.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				stat.mDexterity.mPercent = Round( stat.mDexterity.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				stat.mWisdom.mPercent = Round( stat.mWisdom.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				stat.mVitality.mPercent = Round( stat.mVitality.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				stat.mPhysicAttack.mPercent = Round( stat.mPhysicAttack.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				stat.mPhysicDefense.mPercent = Round( stat.mPhysicDefense.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				stat.mMagicAttack.mPercent = Round( stat.mMagicAttack.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				stat.mMagicDefense.mPercent = Round( stat.mMagicDefense.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				stat.mCriticalRate.mPercent = Round( stat.mCriticalRate.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				stat.mCriticalDamage.mPercent = Round( stat.mCriticalDamage.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				stat.mAccuracy.mPercent = Round( stat.mAccuracy.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				stat.mEvade.mPercent = Round( stat.mEvade.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				stat.mMoveSpeed.mPercent = Round( stat.mMoveSpeed.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				stat.mLife.mPercent = Round( stat.mLife.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				stat.mMana.mPercent = Round( stat.mMana.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				stat.mRecoveryLife.mPercent = Round( stat.mRecoveryLife.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				stat.mRecoveryMana.mPercent = Round( stat.mRecoveryMana.mPercent - value.mValue, 3 );
				break;
			}
		}
	}
	
	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript(
		option.mEnchant.mIndex);

	if( script && option.mEnchant.mLevel )
	{
		for(
			EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
		++it )
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			}
		}
	}
}

BOOL CCharacterCalcManager::IsInvalidEquip( CPlayer* pPlayer ) const
{
	DWORD Armor = pPlayer->GetWearedItemIdx( eWearedItem_Dress );
	DWORD Boots = pPlayer->GetWearedItemIdx( eWearedItem_Shoes );
	DWORD Glove = pPlayer->GetWearedItemIdx( eWearedItem_Glove );

	ITEM_INFO* pArmor = ITEMMGR->GetItemInfo( Armor );
	ITEM_INFO* pBoots = ITEMMGR->GetItemInfo( Boots );
	ITEM_INFO* pGlove = ITEMMGR->GetItemInfo( Glove );

	/// 로그 계열
	if( pPlayer->m_HeroCharacterInfo.Job[0] == 2 )
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( pPlayer->GetPassiveStatus()->HeavyArmor < 1 )
				return TRUE;
		}
	}
	/// 메이지 계열
	else if( pPlayer->m_HeroCharacterInfo.Job[0] == 3 )
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( pPlayer->GetPassiveStatus()->HeavyArmor < 1 )
				return TRUE;
		}
		else if( ( pArmor && pArmor->ArmorType == eArmorType_Leather ) ||
				 ( pBoots && pBoots->ArmorType == eArmorType_Leather ) ||
				 ( pGlove && pGlove->ArmorType == eArmorType_Leather ) )
		{
			if( pPlayer->GetPassiveStatus()->LightArmor < 1 )
				return TRUE;
		}
	}

	return FALSE;
}

void CCharacterCalcManager::ArrangeCharLevelPoint(CPlayer * pPlayer, WORD type)
{
	if(pPlayer->GetPlayerLevelUpPoint() <= 0)
	{
		pPlayer->SetPlayerLevelUpPoint(0);
		return;
	}

	pPlayer->SetPlayerLevelUpPoint(
		pPlayer->GetPlayerLevelUpPoint() - 1);

	switch(type)
	{
	case STR_POINT:
		{
			pPlayer->AddStrength(1);
			break;
		}
	case WIS_POINT:
		{
			pPlayer->AddWisdom(1);
			break;
		}
	case DEX_POINT:
		{
			pPlayer->AddDexterity(1);
			break;
		}
	case VIT_POINT:
		{
			pPlayer->AddVitality(1);
			break;
		}
	case INT_POINT :
		{
			pPlayer->AddIntelligence(1);
			break;
		}
	}

	InsertLogCharacter(
		pPlayer->GetID(),
		pPlayer->GetLevel(),
		pPlayer->GetHeroTotalInfo());
}
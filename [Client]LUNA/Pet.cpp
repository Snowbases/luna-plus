#include "stdafx.h"
#include ".\pet.h"
#include "player.h"
#include "MOTIONDESC.h"
#include "PetManager.h"
#include "ObjectManager.h"
#include "MoveManager.h"

CPet::CPet(void)
{
}

CPet::~CPet(void)
{
}

LPTSTR CPet::GetObjectName()
{
	return mpPetInfo ? mpPetInfo->Name : "";
}

void CPet::InitPet( PET_OBJECT_INFO* pPetObjectInfo ) 
{
	mID = pPetObjectInfo->ID;
	SafeStrCpy( mMasterName, pPetObjectInfo->MasterName, MAX_NAME_LENGTH + 1 );
	mKind = pPetObjectInfo->Kind;
	mGrade = pPetObjectInfo->Grade;
	mState = pPetObjectInfo->State;
	mLevel = pPetObjectInfo->Level;
	mHP = pPetObjectInfo->HP;
	mMaxHP = pPetObjectInfo->MAXHP;

	mpPetInfo = PETMGR->GetPetInfo( mKind );
}

void CPet::Release()
{
	CObject::Release();

	mpPetInfo = NULL;

	mID = 0;
	mMasterIdx = 0;
	mMasterName[0] = 0;
	mKind = 0;
	mGrade = 0;
	mState = ePetState_None;
	mLevel = 0;
	mHP = 0;

	memset( mWearedItem, 0, sizeof( DWORD ) * ePetEquipSlot_Max );
}

// 091110 ONS 펫 애니메이션 추가 : 죽었을때 애니메이션 처리 추가
void CPet::Process()
{
	if(IsDied())
	{
		DWORD time;
		time = REMOVEFADE_START_TIME;
		

		if( gCurTime - m_DiedTime > time )
		{
			ShowObjectName( FALSE );
			ShowChatBalloon( FALSE, NULL );
			//그림자도 없애주자.
			if(m_ShadowObj.IsInited())
			{
				m_ShadowObj.Release();
			}

			DWORD ToTime = time;
			DWORD ElTime = gCurTime - m_DiedTime - time;
			float alp = 1 - (ElTime / (float)ToTime);
			if(alp <= 0)
				alp = 0;
			GetEngineObject()->SetAlpha(alp);
		}
	}
	CObject::Process();
}

float CPet::DoGetMoveSpeed()
{
	CHeroPet* hero = HEROPET;

	float Speed = RUNSPEED;
		
	if( GetMaster() )
		Speed = GetMaster()->DoGetMoveSpeed();

	if( this == HEROPET )
	{		
		// 080630 LUJ, 세트 아이템으로 획득된 이동 속도 보너스가 적용되도록 함
		float addrateval	= ( hero->GetRateBuffStatus()->MoveSpeed + hero->GetRatePassiveStatus()->MoveSpeed ) / 100.f + hero->GetItemStats().mMoveSpeed.mPercent;
		float addval		= hero->GetBuffStatus()->MoveSpeed + hero->GetPassiveStatus()->MoveSpeed + hero->GetItemStats().mMoveSpeed.mPlus;

		m_MoveInfo.AddedMoveSpeed = Speed * addrateval + addval;
	}

	Speed += m_MoveInfo.AddedMoveSpeed;

	return max( 1, Speed );
}

void CPet::SetMotionInState(EObjectState State)
{
//	CheckTmpFunc();
	switch(State)
	{
	case eObjectState_Move:
		{			
			switch(m_MoveInfo.MoveMode)
			{
			case eMoveMode_Walk:
				{
					// 091110 ONS 펫 애니메이션 추가	
					m_ObjectState.State_Start_Motion = GetMotionIDX(eMonsterMotionN_Walk);
					m_ObjectState.State_Ing_Motion = GetMotionIDX(eMonsterMotionN_Walk);
					m_ObjectState.State_End_Motion = GetMotionIDX(eMonsterMotionN_Walk);
					m_ObjectState.MotionSpeedRate = 1.0f;
				}
				break;
			case eMoveMode_Run:
				{
					// 091110 ONS 펫 애니메이션 추가	
					m_ObjectState.State_Start_Motion = GetMotionIDX(eMonsterMotionN_Run);
					m_ObjectState.State_Ing_Motion = GetMotionIDX(eMonsterMotionN_Run);
					m_ObjectState.State_End_Motion = GetMotionIDX(eMonsterMotionN_Run);
					m_ObjectState.MotionSpeedRate = 1.0f;
				}
				break;
			}
			m_ObjectState.State_End_MotionTime = 0;
			
			ChangeMotion(m_ObjectState.State_Start_Motion,FALSE);
			if(m_ObjectState.State_Ing_Motion != -1)
				ChangeBaseMotion(m_ObjectState.State_Ing_Motion);
		}
		break;
	case eObjectState_None:
		{
			// 091110 ONS 펫 애니메이션 추가 : 3가지의 standing모션을 지정한 비율로 설정한다.
			// eMonsterMotionIdle_Battle 모션도 standing모션으로 사용.
			int nSubMotion = rand()%100;
			if(nSubMotion < m_pMotionDESC->GetStandRate())
			{
				if(nSubMotion%2)
				{
					nSubMotion = eMonsterMotionIdle_Stand2;
				}
				else
				{
					nSubMotion = eMonsterMotionIdle_Battle;
				}
			}
			else
				nSubMotion = eMonsterMotionIdle_Stand;

			m_ObjectState.State_Start_Motion = GetMotionIDX(eMonsterMotionN_Stand, nSubMotion);;
			m_ObjectState.State_Ing_Motion = GetMotionIDX(eMonsterMotionN_Stand, nSubMotion);
			m_ObjectState.State_End_Motion = GetMotionIDX(eMonsterMotionN_Stand, nSubMotion);
			m_ObjectState.State_End_MotionTime = 0;
			m_ObjectState.MotionSpeedRate = 1.0f;
		}
		break;
	default:
		{
			m_ObjectState.State_Start_Motion = -1;
			m_ObjectState.State_Ing_Motion = -1;
			m_ObjectState.State_End_Motion = -1;
			m_ObjectState.State_End_MotionTime = 0;
			m_ObjectState.MotionSpeedRate = 1.0f;
		}
		break;
	}
}

void CPet::SetWearedItem( ITEMBASE* pItemInfo )
{
	for( int i = 0; i < ePetEquipSlot_Max; i++ )
	{
		mWearedItem[ i ] = pItemInfo [ i ].wIconIdx;
	}
}

void CPet::Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive) 
{	
	if(pAttacker)
	{
		MOVEMGR->SetLookatPos(this,&pAttacker->GetCurPosition(),0,gCurTime);
	}

	ChangeMotion(
		GetMotionIDX(eMonsterMotionN_Die, 0),
		0);
	ChangeBaseMotion(
		GetMotionIDX(eMonsterMotionN_Die, 1));

	CObject::Die(pAttacker,bFatalDamage,bCritical,bDecisive);
}

// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
void CPet::Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{
	// desc_hseos_전투평화상태01
	// S 전투평화상태 주석처리 added by hseos 2007.08.02
	SetObjectBattleState( eObjectBattleState_Battle );
	if (pAttacker)
		pAttacker->SetObjectBattleState( eObjectBattleState_Battle );
	// E 전투평화상태 주석처리 added by hseos 2007.08.02

	if(Damage != 0)
	{
		if(GetState() == eObjectState_None)
		{
			if(pAttacker)
				MOVEMGR->SetLookatPos(this,&pAttacker->GetCurPosition(),0,gCurTime);
		}
		
		if(GetState() == eObjectState_None)
		{
			// 091110 ONS 펫 애니메이션 추가
			ChangeMotion(GetMotionIDX(eMonsterMotionN_Damage),FALSE);
		}
		
		int DamageEffect = -1;

		if( result.bCritical )
		{
			DamageEffect = eEffect_CriticalDamage;
		}
		else
		{
			DamageEffect = eEffect_NormalDamage;
		}

		if(DamageEffect != -1)
		{
			TARGETSET set;
			set.pTarget = this;
			set.ResultInfo = result;
			
			EFFECTMGR->StartEffectProcess(DamageEffect,pAttacker,&set,1,GetID(),
											EFFECT_FLAG_GENDERBASISTARGET);

			if( result.bCritical )
			{
				TARGETSET set;
				set.pTarget = this;
				EFFECTMGR->StartEffectProcess(19,this,&set,0,GetID());
			}
		}
	}

	if( pAttacker == HERO || pAttacker == HEROPET || this == HEROPET )
	{
		EFFECTMGR->AddDamageNumber(Damage,pAttacker,this,DamageKind,result);
	}

	// Life 셋팅
	DWORD life = GetLife();
	if(life < Damage)
		life = 0;
	else
		life = life - Damage;
	SetLife(life);
}

void CPet::Heal(CObject* pHealer,BYTE HealKind,DWORD HealVal)
{

	// desc_hseos_전투평화상태01
	// S 전투평화상태 주석처리 added by hseos 2007.08.02
	if( GetObjectBattleState() == eObjectBattleState_Battle )
	{
		if( pHealer )	
			pHealer->SetObjectBattleState( eObjectBattleState_Battle );
	}
	// E 전투평화상태 주석처리 added by hseos 2007.08.02

	DWORD life = GetLife() + HealVal;
	SetLife(life);
}

void CPet::Recharge(CObject* pHealer,BYTE RechargeKind,DWORD RechargeVal)
{
	// desc_hseos_전투평화상태01
	// S 전투평화상태 주석처리 added by hseos 2007.08.02
	if( GetObjectBattleState() == eObjectBattleState_Battle )
	{
		if( pHealer )
			pHealer->SetObjectBattleState( eObjectBattleState_Battle );
	}
	// E 전투평화상태 주석처리 added by hseos 2007.08.02

	DWORD mana = GetMana() + RechargeVal;
	SetMana(mana);
}

CPlayer* CPet::GetMaster()
{
	return ( CPlayer* )OBJECTMGR->GetObject( mMasterIdx );
}

void CPet::SetLife( DWORD life, BYTE type )
{
	CObject::SetLife( life );
	mHP = life;
}

DWORD CPet::DoGetMaxLife()
{
	return	mMaxHP;
}

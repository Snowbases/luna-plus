// BossMonster.cpp: implementation of the CBossMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BossMonster.h"
#include "GameResourceManager.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "PartyManager.h"
#include "OptionManager.h"

#ifdef _TESTCLIENT_
#include "MHFile.h"
//#include "BossMonsterInfo.h"
#include "MoveManager.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBossMonster::CBossMonster()
{
	m_bOpenning = FALSE;
}

CBossMonster::~CBossMonster()
{

}

// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
void CBossMonster::Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{	
//	CheckTmpFunc();

	SetObjectBattleState( eObjectBattleState_Battle );

	if(Damage != 0)
	{
		const int DamageEffect = GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind())->Damage1DramaNum;

		if(DamageEffect != -1)
		{
			TARGETSET set;
			set.pTarget = this;
			set.ResultInfo = result;
			
			EFFECTMGR->StartEffectProcess(DamageEffect,pAttacker,&set,1,GetID());
		}
	}

	if( pAttacker )
	{
		if( pAttacker != HEROPETBACKUP )
			pAttacker->SetObjectBattleState( eObjectBattleState_Battle );

		if(pAttacker->GetID() == gHeroID ||
			pAttacker == HEROPET ||
			OPTIONMGR->IsShowMemberDamage() && PARTYMGR->IsPartyMember(pAttacker->GetID()))
		{
			EFFECTMGR->AddDamageNumber(
				Damage,
				pAttacker,
				this,
				DamageKind,result);
		}
		else if(eObjectKind_Monster & pAttacker->GetObjectKind())
		{
			CMonster* const monster = (CMonster*)pAttacker;

			if(monster->GetMonsterTotalInfo().OwnedObjectIndex == gHeroID)
			{
				EFFECTMGR->AddDamageNumber(
					Damage,
					pAttacker,
					this,
					DamageKind,result);
			}
		}
	}
	
	// Life 셋팅
	DWORD life = GetLife();
	if(life < Damage)
		life = 0;
	else
		life = life - Damage;
	SetLife(life);
}

void CBossMonster::SetMotionInState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Move:
		{
			switch(m_MoveInfo.KyungGongIdx)
			{
			case eMA_SCRIPT_WALK:
			case eMA_WALKAROUND:
			case eMA_WALKAWAY:
				{
					m_ObjectState.State_Start_Motion = eBossMonsterMotion_Walk;
					m_ObjectState.State_Ing_Motion = eBossMonsterMotion_Walk;
					m_ObjectState.State_End_Motion = eBossMonsterMotion_Walk;
					m_ObjectState.State_End_MotionTime = 0;
					m_ObjectState.MotionSpeedRate = 1.0f;
				}
				break;
			case eMA_RUNAWAY:
				{
					m_ObjectState.State_Start_Motion = eBossMonsterMotion_Run1;
					m_ObjectState.State_Ing_Motion = eBossMonsterMotion_Run2;
					m_ObjectState.State_End_Motion = eBossMonsterMotion_Run3;
					m_ObjectState.State_End_MotionTime = 0;
					m_ObjectState.MotionSpeedRate = 1.0f;
				}
				break;
			case eMA_SCRIPT_RUN:
			case eMA_PERSUIT:
				{
					m_ObjectState.State_Start_Motion = eBossMonsterMotion_Run1;
					m_ObjectState.State_Ing_Motion = eBossMonsterMotion_Run2;
					m_ObjectState.State_End_Motion = eBossMonsterMotion_Run3;
					m_ObjectState.State_End_MotionTime = 0;
					m_ObjectState.MotionSpeedRate = 1.0f;
				}
				break;
			}
		}
		break;
	case eObjectState_None:
		{
			m_ObjectState.State_Start_Motion = eBossMonsterMotion_Stand;
			m_ObjectState.State_Ing_Motion = eBossMonsterMotion_Stand;
			m_ObjectState.State_End_Motion = eBossMonsterMotion_Stand;
			m_ObjectState.State_End_MotionTime = 0;
			m_ObjectState.MotionSpeedRate = 1.0f;
		}
		break;
	case eObjectState_Rest:
		{
			m_ObjectState.State_Start_Motion = -1;
			m_ObjectState.State_Ing_Motion = -1;
			m_ObjectState.State_End_Motion = -1;
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
/*
void CBossMonster::AddStatus(CStatus* pStatus)
{
	//boss block status
}
*/
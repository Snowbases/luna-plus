#include "stdafx.h"
#include "Monster.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "ObjectGuagen.h"
#include "ObjectManager.h"
#include "GameResourceManager.h"
#include "MOTIONDESC.h"
#include "GameIn.h"
#include "MonsterGuageDlg.h"
#include "PartyManager.h"
#include "OptionManager.h"

CMonster::CMonster()
{
	m_pSInfo = 0;
	mLastCheckedTick = 0;
}

CMonster::~CMonster()
{

}

LPTSTR CMonster::GetObjectName()
{
	return m_pSInfo ? m_pSInfo->Name : "";
}

void CMonster::InitMonster(MONSTER_TOTALINFO* pTotalInfo)
{
	memcpy(&m_MonsterInfo,pTotalInfo,sizeof(MONSTER_TOTALINFO));
	m_pSInfo = GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind());

	// 080616 LUJ, 마지막 체크 시간 초기화
	mLastCheckedTick = gCurTime + 1000;
}

void CMonster::Process()
{
	if(IsDied())
	{
		DWORD time = REMOVEFADE_START_TIME;
		if(GetObjectKind() == eObjectKind_BossMonster)
		{
			time += 5000;
		}

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
#ifdef _TESTCLIENT_
		if(gCurTime - m_DiedTime > MONSTERREMOVE_TIME)
		{
			OBJECTMGR->AddGarbageObject(this);
		}
#endif
	}
	else
	{
		const EObjectKind objectKind = EObjectKind( GetSInfo() ? GetSInfo()->ObjectKind : eObjectKind_None );

		switch( objectKind )
		{
			// 080616 LUJ, 함정인 경우 초당 1씩 피를 감소시키고, 최후에는 자살시킨다
		case eObjectKind_Trap:
			{
				if( gCurTime < mLastCheckedTick )
				{
					break;
				}
				else if( ! GetLife() )
				{
					SetDieFlag();
					break;
				}

				SetLife( GetLife() - 1 );
				mLastCheckedTick = gCurTime + 1000;
				break;
			}
		}
	}

	if(GetObjectKind() == eObjectKind_BossMonster)
	{
		CBossMonster* const pMon = (CBossMonster*)this;

		// 080109 KTH -- 보스 몬스터 등장 연출
		if( pMon && pMon->IsOpenning() )
		{
			if( pMon->GetOpenningTime()->Check() == 0 )
			{
				pMon->GetEngineObject()->EnablePick();
				pMon->SetOpenningEnd();
			}
		}
	}
	
	CObject::Process();
}

void CMonster::SetMotionInState(EObjectState State)
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
					m_ObjectState.State_Start_Motion = GetMotionIDX(eMonsterMotionN_Walk);
					m_ObjectState.State_Ing_Motion = GetMotionIDX(eMonsterMotionN_Walk);
					m_ObjectState.State_End_Motion = GetMotionIDX(eMonsterMotionN_Walk);
					m_ObjectState.MotionSpeedRate = 1.0f;
				}
				break;
			case eMA_RUNAWAY:
				{
					m_ObjectState.State_Start_Motion = GetMotionIDX(eMonsterMotionN_Run);
					m_ObjectState.State_Ing_Motion = GetMotionIDX(eMonsterMotionN_Run);
					m_ObjectState.State_End_Motion = GetMotionIDX(eMonsterMotionN_Run);
					m_ObjectState.MotionSpeedRate = 1.0f;
				}
				break;
			case eMA_SCRIPT_RUN:
			case eMA_PERSUIT:
				{
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
			if(	GetObjectBattleState() )
			{
				m_ObjectState.State_Start_Motion = GetMotionIDX(eMonsterMotionN_Stand, eMonsterMotionIdle_Battle);
				m_ObjectState.State_Ing_Motion = GetMotionIDX(eMonsterMotionN_Stand, eMonsterMotionIdle_Battle);
				m_ObjectState.State_End_Motion = GetMotionIDX(eMonsterMotionN_Stand, eMonsterMotionIdle_Battle);
				m_ObjectState.State_End_MotionTime = 0;
				m_ObjectState.MotionSpeedRate = 1.0f;
			}
			else
			{
				// 080321 NYJ --- Stand Motion은 랜덤하게 결정한다.
				int nSubMotion = rand()%100;
				if(nSubMotion < m_pMotionDESC->GetStandRate())
					nSubMotion = eMonsterMotionIdle_Stand2;
				else
					nSubMotion = eMonsterMotionIdle_Stand;

				m_ObjectState.State_Start_Motion = GetMotionIDX(eMonsterMotionN_Stand, nSubMotion);
				m_ObjectState.State_Ing_Motion = GetMotionIDX(eMonsterMotionN_Stand, nSubMotion);
				m_ObjectState.State_End_Motion = GetMotionIDX(eMonsterMotionN_Stand, nSubMotion);
				m_ObjectState.State_End_MotionTime = 0;
				m_ObjectState.MotionSpeedRate = 1.0f;
			}
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

void CMonster::Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive)
{
	if(pAttacker)
		MOVEMGR->SetLookatPos(this,&pAttacker->GetCurPosition(),0,gCurTime);

	if( OBJECTMGR->GetSelectedObjectID() == GetID() )
	{
		OBJECTMGR->SetSelectedEffect( NULL );
	}

	int DieEffect;
	if(bFatalDamage)
		DieEffect = GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind())->Die2DramaNum;	// 날라가서 죽기
	else
		DieEffect = GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind())->Die1DramaNum;	// 그냥 죽기
		
	if(DieEffect != -1)
	{
		TARGETSET set;
		set.pTarget = this;
		set.ResultInfo.bCritical = bCritical;
		set.ResultInfo.bDecisive = bDecisive;
		
		EFFECTMGR->StartEffectProcess(DieEffect,pAttacker,&set,1,GetID(),
							EFFECT_FLAG_DIE|EFFECT_FLAG_GENDERBASISTARGET);
	}

	// Life 셋팅
	SetLife(0);
}

// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
void CMonster::Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{	
	SetObjectBattleState( eObjectBattleState_Battle );

	// 091218 LUJ, 널포인터 예외가 발생해서 검사를 추가함
	if(pAttacker &&
		pAttacker != HEROPETBACKUP)
	{
		pAttacker->SetObjectBattleState( eObjectBattleState_Battle );
	}

	if(Damage != 0)
	{
		if(GetState() == eObjectState_None)
		{
			if(pAttacker)
				MOVEMGR->SetLookatPos(this,&pAttacker->GetCurPosition(),0,gCurTime);

			ChangeMotion(1,FALSE);
			switch(DamageKind) {
			case eDamageKind_Front:
				ChangeMotion(eMonsterMotion_DamageFront,FALSE);
				break;
			case eDamageKind_Left:
				ChangeMotion(eMonsterMotion_DamageLeft,FALSE);
				break;
			case eDamageKind_Right:
				ChangeMotion(eMonsterMotion_DamageRight,FALSE);
				break;
			}	
		}
		
		int DamageEffect = GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind())->Damage1DramaNum;

		if(DamageEffect != -1)
		{
			TARGETSET set;
			set.pTarget = this;
			set.ResultInfo = result;
			
			EFFECTMGR->StartEffectProcess(
				DamageEffect,
				pAttacker,
				&set,
				1,
				GetID(),
				EFFECT_FLAG_GENDERBASISTARGET);

			if( result.bCritical )
			{
				TARGETSET set;
				set.pTarget = this;
				EFFECTMGR->StartEffectProcess(19,this,&set,0,GetID());
			}
		}
	}

	if( pAttacker )
	{
		if(pAttacker->GetID() == gHeroID ||
			pAttacker == HEROPET ||
			GetMonsterTotalInfo().OwnedObjectIndex == gHeroID ||
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
void CMonster::Heal(CObject* pHealer,BYTE HealKind,DWORD HealVal)
{
	SetLife(GetLife() + HealVal);
}

float CMonster::GetWeight()
{
	BASE_MONSTER_LIST* pList = GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind());
	ASSERT(pList->MonsterKind == GetMonsterKind());
	return pList->Weight;
}

float CMonster::GetRadius()
{
	BASE_MONSTER_LIST* pList = GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind());
	ASSERT(pList->MonsterKind == GetMonsterKind());
	return (float)pList->MonsterRadius;
}

void CMonster::SetLife(DWORD val, BYTE type)
{
	CObject::SetLife( val, type );
	m_MonsterInfo.Life = val;

	if( GAMEIN->GetMonsterGuageDlg() && m_pSInfo )
	{
		if( GAMEIN->GetMonsterGuageDlg()->GetWantedTargetID() == GetID() )
		{
			GAMEIN->GetMonsterGuageDlg()->SetMonsterLife( val, m_pSInfo->Life );
		}
	}
}

float CMonster::DoGetMoveSpeed()
{
	float Speed = 0;
	WORD kind = GetMonsterKind();

	switch(m_MoveInfo.KyungGongIdx)
	{
	case eMA_SCRIPT_WALK:
	case eMA_WALKAROUND:
	case eMA_WALKAWAY:
		{
			Speed = (float)GAMERESRCMNGR->GetMonsterListInfo(kind)->WalkMove;
		}
		break;
	case eMA_RUNAWAY:
		{
			Speed = (float)GAMERESRCMNGR->GetMonsterListInfo(kind)->RunawayMove;
		}
		break;
	case eMA_SCRIPT_RUN:
	case eMA_PERSUIT:
		{
			Speed = (float)GAMERESRCMNGR->GetMonsterListInfo(kind)->RunMove;
		}
		break;
	}

	return max( 0, Speed + m_MoveInfo.AddedMoveSpeed );
}

DWORD CMonster::DoGetMaxLife()
{
	if( m_pSInfo )
		return	m_pSInfo->Life;
	else
		return	NULL;
}

DWORD CMonster::GetMonsterTargetType()
{
	return	m_pSInfo->dwMonsterTargetType;
}
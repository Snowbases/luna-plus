// CObjectActionManager.cpp: implementation of the CObjectActionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ObjectActionManager.h"
#include "ObjectStateManager.h"
#include "Object.h"
#include "MoveManager.h"
#include "ObjectManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CObjectActionManager);
CObjectActionManager::CObjectActionManager()
{

}

CObjectActionManager::~CObjectActionManager()
{

}

void CObjectActionManager::Die(CObject* pObject,CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive)
{
	if(MOVEMGR->IsMoving(pObject)==TRUE)
		MOVEMGR->MoveStop(pObject);
	if(pObject->GetState() != eObjectState_Die)
	{
		if(pObject->GetState() != eObjectState_None)
			OBJECTSTATEMGR->EndObjectState(pObject,pObject->GetState());

		OBJECTSTATEMGR->StartObjectState(pObject,eObjectState_Die);
		pObject->Die(pAttacker,bFatalDamage,bCritical, bDecisive);
	}
//	else
//	{
//		int a=0;
//	}
}

// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
void CObjectActionManager::Damage(CObject* pObject,CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{
	if(0 == pObject)
	{
		return;
	}

	pObject->Damage(pAttacker,DamageKind,Damage,result);
}

// 100223 ShinJS --- 마나 데미지 추가
void CObjectActionManager::ManaDamage(CObject* pObject,CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{	
	pObject->ManaDamage(pAttacker,DamageKind,Damage,result);
}

void CObjectActionManager::Dodge(CObject* pObject,CObject* pAttacker)
{
	if( pObject == HERO || pObject == HEROPET || pAttacker == HERO || pAttacker == HEROPET )
		EFFECTMGR->AddDamageNumberDodge(pObject);
}

void CObjectActionManager::Revive(CObject* pObject,VECTOR3* pRevivePos)
{
	OBJECTSTATEMGR->EndObjectState(pObject,eObjectState_Die);
	pObject->Revive(pRevivePos);
}
	
void CObjectActionManager::Heal(CObject* pObject,CObject* pHealer,BYTE DamageKind,DWORD HealVal)
{
	pObject->Heal(pHealer,DamageKind,HealVal);
}
void CObjectActionManager::Recharge(CObject* pObject,CObject* pRecharger,BYTE RechargeKind,DWORD RechargeVal)
{
	pObject->Recharge(pRecharger,RechargeKind,RechargeVal);
}
void CObjectActionManager::ApplyTargetList(CObject* pOperator,CTargetList* pTList,BYTE DamageKind)
{
	CObject* pTarget;
	RESULTINFO rinfo;
	CTargetListIterator iter(pTList);
	while(iter.GetNextTarget())
	{
		pTarget = OBJECTMGR->GetObject(iter.GetTargetID());
		iter.GetTargetData(&rinfo);
		if(pTarget == NULL)
			continue;
		OBJECTACTIONMGR->ApplyResult(pTarget,pOperator,&rinfo,DamageKind);
	}
	iter.Release();
}

// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
void CObjectActionManager::ApplyResult(CObject* pObject,CObject* pOperator,RESULTINFO* pRInfo,BYTE DamageKind)
{
	if(pRInfo->RealDamage)
	{
		Damage(pObject,pOperator,DamageKind,pRInfo->RealDamage,*pRInfo);
		if(pObject->IsDied())
			Die(pObject,pOperator,FALSE,pRInfo->bCritical,pRInfo->bDecisive);
	}
	// 100223 ShinJS --- 마나 데미지 추가
	if(pRInfo->ManaDamage)
	{
		ManaDamage(pObject,pOperator,DamageKind,pRInfo->ManaDamage,*pRInfo);
	}
	if(pRInfo->CounterDamage)
	{
		const RESULTINFO emptyResult = { 0 };
		Damage(pOperator,pObject,eDamageKind_Counter,pRInfo->CounterDamage,emptyResult);
		if(pOperator->IsDied())
			Die(pOperator,pObject,FALSE,FALSE,FALSE);
	}
	if(pRInfo->HealLife)
		Heal(pObject,pOperator,DamageKind,pRInfo->HealLife);
	if(pRInfo->RechargeMana)
		Recharge(pObject,pOperator,0,pRInfo->RechargeMana);	
	
	if(pRInfo->StunTime)
	
	if(pRInfo->Vampiric_Life)
	{
		if(pOperator->GetID() == gHeroID)
			HERO->ChangeLife(pRInfo->Vampiric_Life);
		else
			pOperator->SetLife(pOperator->GetLife()+pRInfo->Vampiric_Life);
	}
	
	if(pRInfo->Vampiric_Mana)
	{
		pOperator->SetMana(pOperator->GetMana()+pRInfo->Vampiric_Mana);
	}
}
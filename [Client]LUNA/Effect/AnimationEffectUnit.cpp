#include "StdAfx.h"
#include "AnimationEffectUnit.h"
#include "AnimationEffectUnitDesc.h"
#include "Object.h"

CAnimationEffectUnit::CAnimationEffectUnit()
{

}

CAnimationEffectUnit::~CAnimationEffectUnit()
{

}

void CAnimationEffectUnit::DoOn(EFFECTPARAM* pParam,TARGETSET* pObject)
{
	if(pObject->pTarget->GetState() == eObjectState_Die)
	{
		if((pParam->m_dwFlag & EFFECT_FLAG_DIE) == FALSE)
			return;
	}

	CAnimationEffectUnitDesc* pDesc = (CAnimationEffectUnitDesc*)m_pEffectUnitDesc;

	pObject->pTarget->ChangeMotion(1,FALSE);

	float fAniRate = pParam->m_AnimateRate;

	// 100623 ShinJS Ani Play 비율을 고정/조정
	switch( pDesc->m_TimeRateType )
	{
	case CAnimationEffectUnitDesc::eTimeRateType_FixRate:
		{
			fAniRate = pDesc->m_fFixTimeRate;
		}
		break;
	case CAnimationEffectUnitDesc::eTimeRateType_LimitRate:
		{
			CEffect* pEffect = EFFECTMGR->GetEffect( pParam->m_EffectID );
			if( pEffect )
			{
				const DWORD dwMontionAniTime = pObject->pTarget->GetEngineObject()->GetAnimationTime( pDesc->m_MotionNum );
				const float fCalcAniTime = dwMontionAniTime / fAniRate;
                if( fCalcAniTime < pDesc->m_fMinLimitAniTime )
				{
					fAniRate = dwMontionAniTime / pDesc->m_fMinLimitAniTime;
				}
				else if( fCalcAniTime > pDesc->m_fMaxLimitAniTime )
				{
					fAniRate = dwMontionAniTime / pDesc->m_fMaxLimitAniTime;
				}
			}
		}
		break;
	}

	if(pDesc->m_CustumMotion)
		pObject->pTarget->GetEngineObject()->ChangeCustumMotion(pDesc->m_MotionNum,
								pDesc->m_StartFrame,pDesc->m_EndFrame,pDesc->m_bRepeat,fAniRate);
	else
		pObject->pTarget->ChangeMotion(pDesc->m_MotionNum,pDesc->m_bRepeat,fAniRate);
}

void CAnimationEffectUnit::DoSetBaseMotion(EFFECTPARAM* pParam,TARGETSET* pObject,int BaseAnimationNum)
{
	if(pObject->pTarget->GetState() == eObjectState_Die)
	{
		if((pParam->m_dwFlag & EFFECT_FLAG_DIE) == FALSE)
			return;
	}
	
	pObject->pTarget->ChangeBaseMotion(BaseAnimationNum);
}

void CAnimationEffectUnit::Initialize(CEffectUnitDesc* pEffectUnitDesc,EFFECTPARAM* pEffectParam)
{

	CEffectUnit::Initialize(pEffectUnitDesc,pEffectParam);
}
void CAnimationEffectUnit::Release()
{
	CAnimationEffectUnitDesc::ReleaseUnit(this);
}
#include "stdafx.h"
#include "Effect/EffectManager.h"
#include ".\skilleffect.h"
#include "Object.h"
#include "ObjectManager.h"
#include "ObjectActionManager.h"

cSkillEffect::cSkillEffect(void)
{
	mSkillObjectID = 0;
}

cSkillEffect::~cSkillEffect(void)
{
}

void cSkillEffect::Init( int EffectNum, CObject* pOperator, MAINTARGET* pMainTarget )
{
	mEffectNum = EffectNum;
	mpOperator = pOperator;
	mpMainTarget = pMainTarget;
	m_hEff = 0;
}

void cSkillEffect::StartEffect( BOOL bFatal, float rate )
{
	DWORD EffectFlag = 0;
	if(mpOperator->GetID() == HEROID)
		EffectFlag |= EFFECT_FLAG_HEROATTACK;
	if(bFatal)
		EffectFlag |= EFFECT_FLAG_FATALDAMAGE;

	m_hEff = EFFECTMGR->StartEffectProcessWithTargetList( mEffectNum, mpOperator, NULL, mpMainTarget, EffectFlag, NULL, rate);
}

void cSkillEffect::EndEffect()
{
	if(m_hEff)
	{
		EFFECTMGR->ForcedEndEffect( m_hEff );
		m_hEff = 0;
	}
}

void cSkillEffect::StartTargetEffect( DWORD id )
{
	CObject* pTarget = NULL;
    
	if( mpMainTarget->MainTargetKind == MAINTARGET::MAINTARGETKIND_OBJECTID )
	{
		pTarget = OBJECTMGR->GetObject( mpMainTarget->dwMainTargetID );
	}
	else
	{
		return;
	}

	if( pTarget )
	{
		mSkillObjectID = id;
		OBJECTEFFECTDESC desc( mEffectNum );
		pTarget->AddObjectEffect( mSkillObjectID, &desc, 1, mpOperator );
	}
}

void cSkillEffect::EndTargetEffect()
{
	CObject* pTarget = NULL;

	if( mpMainTarget->MainTargetKind == MAINTARGET::MAINTARGETKIND_OBJECTID )
	{
		pTarget = OBJECTMGR->GetObject( mpMainTarget->dwMainTargetID );
	}
	else
	{
		return;
	}

	if( pTarget )
	{
		pTarget->RemoveObjectEffect( mSkillObjectID );
		mSkillObjectID = 0;
	}
}

void cSkillEffect::SetTargetArray(CObject* pOperator,CTargetList* pTList,MAINTARGET* pMainTarget)
{
	if(m_hEff)
	{
		if(EFFECTMGR->SetTargetArray(m_hEff,pTList,pMainTarget) == FALSE)
			m_hEff = NULL;
	}

	if(m_hEff == NULL)
	{
		OBJECTACTIONMGR->ApplyTargetList(pOperator,pTList,eDamageKind_Front);
	}
}

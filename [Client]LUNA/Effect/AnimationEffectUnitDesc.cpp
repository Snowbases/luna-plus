#include "stdafx.h"
#include "AnimationEffectUnitDesc.h"
#include "AnimationEffectUnit.h"
#include "MHFile.h"
#include "GameResourceManager.h"

CMemoryPoolTempl<CAnimationEffectUnit>* CAnimationEffectUnitDesc::m_pUnitPool = NULL;

CAnimationEffectUnitDesc::CAnimationEffectUnitDesc(BOOL bDangledToOperator)
: CEffectUnitDesc(eEffectUnit_Animation,bDangledToOperator)
{
	if(m_pUnitPool == NULL)
	{
		m_pUnitPool = new CMemoryPoolTempl<CAnimationEffectUnit>;
		m_pUnitPool->Init(200,50,"CAnimationEffectUnit");
	}

	m_TimeRateType = eTimeRateType_None;
	m_fMinLimitAniTime = m_fMaxLimitAniTime = 0.f;
	m_fFixTimeRate = 1.f;
}

CAnimationEffectUnitDesc::~CAnimationEffectUnitDesc()
{
	if(m_pUnitPool)
	{
		m_pUnitPool->Release();
		delete m_pUnitPool;
		m_pUnitPool = NULL;
	}
}

void CAnimationEffectUnitDesc::DoParseScript(char* szKey,CMHFile* pFile)
{
	CMD_ST(szKey);
		CMD_CS("#MOTION")
			m_MotionNum = pFile->GetDword();
			m_CustumMotion = FALSE;
			m_bRepeat = FALSE;
		CMD_CS("#REPEAT")
			m_bRepeat = pFile->GetBool();
		CMD_CS("#FRAME")
			m_CustumMotion = pFile->GetBool();
			m_StartFrame = pFile->GetInt();
			m_EndFrame = pFile->GetInt();
		CMD_CS("#LIMITFRAMERATE")
			m_TimeRateType = eTimeRateType_LimitRate;
			m_fMinLimitAniTime = (gTickPerFrame * pFile->GetDword());
			m_fMaxLimitAniTime = (gTickPerFrame * pFile->GetDword());
		CMD_CS("#FIXFRAMERATE")
			m_TimeRateType = eTimeRateType_FixRate;
			m_fFixTimeRate = pFile->GetFloat();
	CMD_EN
}

CEffectUnit* CAnimationEffectUnitDesc::GetEffectUnit(EFFECTPARAM* pEffectParam)
{
	CAnimationEffectUnit* pEffectUnit = m_pUnitPool->Alloc();
	pEffectUnit->Initialize(this,pEffectParam);
	
	return pEffectUnit;
}
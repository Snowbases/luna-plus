// EffectTriggerOnUnitDesc.cpp: implementation of the CEffectTriggerOnUnitDesc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectTriggerOnUnitDesc.h"
#include "EffectUnit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectTriggerOnUnitDesc::CEffectTriggerOnUnitDesc(DWORD dwTime,DWORD dwUnitNum)
:	CEffectTriggerUnitDesc(dwTime,dwUnitNum)
{

}

CEffectTriggerOnUnitDesc::~CEffectTriggerOnUnitDesc()
{

}

BOOL CEffectTriggerOnUnitDesc::Operate(CEffect* pEffect)
{
	if(0 == pEffect)
	{
		return FALSE;
	}
	else if(CEffectUnit* const effectUnit = pEffect->GetEffectUnit(m_dwUnitNum))
	{
		return effectUnit->On(
			pEffect->GetEffectParam());
	}

	return FALSE;
}

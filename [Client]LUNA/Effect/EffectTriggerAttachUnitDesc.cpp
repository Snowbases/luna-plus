// EffectTriggerAttachUnitDesc.cpp: implementation of the CEffectTriggerAttachUnitDesc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectTriggerAttachUnitDesc.h"
#include "EffectUnit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectTriggerAttachUnitDesc::CEffectTriggerAttachUnitDesc(DWORD dwTime,DWORD dwUnitNum)
:	CEffectTriggerUnitDesc(dwTime,dwUnitNum), m_EnableScale(FALSE)
{
}

CEffectTriggerAttachUnitDesc::~CEffectTriggerAttachUnitDesc()
{

}


void CEffectTriggerAttachUnitDesc::ParseScript(CMHFile* pFile)
{
	char buf[128];
	pFile->GetString(buf);	// skip  '{'
	if(buf[0] != '{')
	{
		ERRORBSGBOX("ERROR[%s]!! { } 가 필요한 트리거입니다!!!",pFile->GetFileName());
		return;
	}

	while(1)
	{
		pFile->GetString(buf);
		CMD_ST(buf)
			CMD_CS("}")
				break;
			CMD_CS("#ATTACHPART")
				m_AttachPartName = pFile->GetStringInQuotation();
			CMD_CS("#ENABLESCALE")
				m_EnableScale = pFile->GetBool();
		CMD_EN
	}
}
BOOL CEffectTriggerAttachUnitDesc::Operate(CEffect* pEffect)
{	
	return pEffect->GetEffectUnit(m_dwUnitNum)->Attach(pEffect->GetEffectParam(),m_AttachPartName,m_EnableScale);
}


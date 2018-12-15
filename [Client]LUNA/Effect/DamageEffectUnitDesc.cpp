#include "StdAfx.h"
#include "DamageEffectUnitDesc.h"
#include "DamageEffectUnit.h"
#include "MHFile.h"

CMemoryPoolTempl<CDamageEffectUnit>* CDamageEffectUnitDesc::m_pUnitPool = NULL;

CDamageEffectUnitDesc::CDamageEffectUnitDesc(BOOL bDangledToOperator)
: CEffectUnitDesc(eEffectUnit_Damage,bDangledToOperator)
{
	if(m_pUnitPool == NULL)
	{
		m_pUnitPool = new CMemoryPoolTempl<CDamageEffectUnit>;
		m_pUnitPool->Init(200,50,"CDamageEffectUnit");
	}
	m_DamageRate = 1;
	m_FadeDuration = 1000;
	m_DamageKind = eDamageKind_Front;
}

CDamageEffectUnitDesc::~CDamageEffectUnitDesc()
{
	if(m_pUnitPool)
	{
		m_pUnitPool->Release();
		delete m_pUnitPool;
		m_pUnitPool = NULL;
	}
}

void CDamageEffectUnitDesc::DoParseScript(char* szKey,CMHFile* pFile)
{
	CMD_ST(szKey);
		CMD_CS("#DURATION")
			m_FadeDuration = pFile->GetDword();
			
		CMD_CS("#PERCENT")
			m_DamageRate = pFile->GetFloat() * 0.01f;
			
		CMD_CS("#DAMAGEKIND")
			LPCTSTR strKind = pFile->GetString();
			m_DamageKind = BYTE(_ttoi(strKind));

			if(m_DamageKind == 0)	// 글자로 써놓은 경우
			{
				if(strcmp(strKind,"FRONT")==0)
					m_DamageKind = eDamageKind_Front;
				else if(strcmp(strKind,"LEFT")==0)
					m_DamageKind = eDamageKind_Left;
				else if(strcmp(strKind,"RIGHT")==0)
					m_DamageKind = eDamageKind_Right;
			}
	CMD_EN
}

CEffectUnit* CDamageEffectUnitDesc::GetEffectUnit(EFFECTPARAM* pEffectParam)
{
	CDamageEffectUnit* pEffectUnit = m_pUnitPool->Alloc();
	pEffectUnit->Initialize(this,pEffectParam);
	
	return pEffectUnit;
}
#include "StdAfx.h"
#include "CameraEffectUnitDesc.h"
#include "CameraEffectUnit.h"
#include "MHFile.h"

CMemoryPoolTempl<CCameraEffectUnit>* CCameraEffectUnitDesc::m_pUnitPool = NULL;

CCameraEffectUnitDesc::CCameraEffectUnitDesc(BOOL bDangledToOperator, eCameraEffectUnitType eType)
: CEffectUnitDesc(eEffectUnit_Camera,bDangledToOperator)
{
	if(m_pUnitPool == NULL)
	{
		m_pUnitPool = new CMemoryPoolTempl<CCameraEffectUnit>;
		m_pUnitPool->Init(50,5,"CCameraEffectUnit");
	}

	m_CameraEffectUnitType = eType;

	m_CameraNum = -1;
	m_InitAngleY = 0;
	m_InitAngleY = 0;
	m_InitDistance = 500;

	m_dwFilterIndex = 0;
	m_bFilterRepeat = FALSE;
	m_fFilterFadeTime = 0.f;
	m_bFilterDetachAll = FALSE;
	m_bFilterOnlyOne = FALSE;
	m_bFilterRelease = TRUE;
	m_dwFilterRemainTime = 0;
}

CCameraEffectUnitDesc::~CCameraEffectUnitDesc()
{
	if(m_pUnitPool)
	{
		m_pUnitPool->Release();
		delete m_pUnitPool;
		m_pUnitPool = NULL;
	}
}

void CCameraEffectUnitDesc::DoParseScript(char* szKey,CMHFile* pFile)
{
	CMD_ST(szKey);
		CMD_CS("#CAMERANUM")
			m_CameraNum = pFile->GetInt();
		
		CMD_CS("#ANGLEX")
			m_InitAngleX = pFile->GetFloat();

		CMD_CS("#ANGLEY")
			m_InitAngleY = pFile->GetFloat();
		
		CMD_CS("#DISTANCE")
			m_InitDistance = pFile->GetFloat();

		CMD_CS("#FILTERNUM")
			m_dwFilterIndex = pFile->GetDword();

		CMD_CS("#REPEAT")
			m_bFilterRepeat = pFile->GetBool();

		CMD_CS("#FADETIME")
			m_fFilterFadeTime = pFile->GetFloat();

		CMD_CS("#DETACHALL")
			m_bFilterDetachAll = pFile->GetBool();

		CMD_CS("#ONLYONE")
			m_bFilterOnlyOne = pFile->GetBool();

		CMD_CS("#RELEASE")
			m_bFilterRelease = pFile->GetBool();

		CMD_CS("#REMAINTIME")
			m_dwFilterRemainTime = pFile->GetDword();

	CMD_EN
}

CEffectUnit* CCameraEffectUnitDesc::GetEffectUnit(EFFECTPARAM* pEffectParam)
{
	ASSERT(m_CameraNum != -1);
	CCameraEffectUnit* pEffectUnit = m_pUnitPool->Alloc();
	pEffectUnit->Initialize(this,pEffectParam);
	
	return pEffectUnit;
}
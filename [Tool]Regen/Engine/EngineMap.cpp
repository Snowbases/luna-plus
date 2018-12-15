// EngineMap.cpp: implementation of the EngineMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EngineMap.h"
//#include "../MainGame.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngineMap::CEngineMap()
{
	m_bMapBuild = FALSE;
}

CEngineMap::~CEngineMap()
{
	Release();
}

BOOL CEngineMap::Init(char* szMapFile)
{
	Release();

	return g_pExecutive->LoadMapScript(
		szMapFile,
		0,
		0);
}

void CEngineMap::Release()
{
	if(g_pExecutive)
	{
		for(GXOBJECT_HANDLE handle = m_GXObjectList.RemoveTail();
			0 < handle;
			handle = m_GXObjectList.RemoveTail())
		{
			g_pExecutive->DeleteGXObject(handle);
		}

		for(GXLIGHT_HANDLE handle = m_GXLightList.RemoveTail();
			0 < handle;
			handle = m_GXLightList.RemoveTail())
		{
			g_pExecutive->DeleteGXLight(handle);
		}

		CleanHeightField();
		CleanStaticModel();
		g_pExecutive->DeleteGXMap(m_ghMap);		
	}
}

void CEngineMap::SetAmbientColor(DWORD Color)
{
	g_pExecutive->GetGeometry()->SetAmbientColor(0,Color);
}



BOOL CEngineMap::ImportHeightField(char* szFileName,DWORD dwIndexBufferNum)
{
	BOOL			bResult = FALSE;
	
	CleanHeightField();

	bResult = m_pExecutive->GetGeometry()->CreateHeightField(&m_pHField,0);

	if (!m_pHField->ReadFile(szFileName,dwIndexBufferNum,NULL,0))
	{
		m_pHField->Release();
		m_pHField = NULL;
		memset(m_szHFieldName,0,sizeof(m_szHFieldName));
		return FALSE;
	}
	lstrcpy(m_szHFieldName,szFileName);


	m_pHField->GetHFieldDesc(&m_hfDesc);
	
//	for (DWORD i=0; i<4; i++)
//		ResetDefaultCamera(i);

	m_pExecutive->GetRenderer()->SetLightMapFlag(0);

	return bResult;

}

BOOL CEngineMap::ImportStaticModel(char* szFileName)
{
	I3DStaticModel*	pStaticModel = NULL;
	
	if (0xffffffff == m_pExecutive->GetGeometry()->CreateStaticModel(&pStaticModel,4000,500,0))
		return FALSE;


	if (!pStaticModel->ReadFile(szFileName,NULL,STATIC_MODEL_LOAD_ENABLE_SHADE))
	{
		pStaticModel->Release();
		return FALSE;
	}

	if (m_ghMap)
	{
		m_pExecutive->DeleteGXMap(m_ghMap);
		m_ghMap = NULL;
	}

	CleanStaticModel();

	m_pStaticModel = pStaticModel;
	m_pStaticModel->BeginCreateMesh(0);

	return TRUE;
}

void CEngineMap::CleanHeightField()
{	
	if (m_pHField)
	{
		m_pHField->Release();
		m_pHField = NULL;
	}

	memset(&m_hfDesc,0,sizeof(m_hfDesc));
	
	m_bMapBuild = FALSE;
}
void CEngineMap::CleanStaticModel()
{
	if (m_pStaticModel)
	{
		m_pStaticModel->Release();
		m_pStaticModel = NULL;
	}
	m_bMapBuild = FALSE;
}

BOOL CEngineMap::BuildMap(MAABB* pWorldBox)
{
	if (m_bMapBuild)
	{
		m_pExecutive->RebuildMap(pWorldBox->Max.y, pWorldBox->Min.y);
		m_WorldBox	=	*pWorldBox;
		return TRUE;

	}
	if( (pWorldBox->Max.y - pWorldBox->Min.y) <= 0.0f)
	{
		pWorldBox->Max.y	=	pWorldBox->Min.y	+	100.0f;
	}
	m_WorldBox.Max.y	=	pWorldBox->Max.y;
	m_WorldBox.Min.y	=	pWorldBox->Min.y;

	m_ghMap = m_pExecutive->CreateGXMap(NULL,0,NULL);

	m_pExecutive->BeginBuildMap(0);

	if (m_pStaticModel)
		m_pExecutive->InsertStaticModelTOGXMap(m_pStaticModel);
	
	if (m_pHField)
		m_pExecutive->InsertHFieldToGXMap(m_pHField);

	m_pExecutive->EndBuildMap(pWorldBox->Max.y,pWorldBox->Min.z);

	m_pExecutive->GetWorldBoundingBox( &m_WorldBox);

	m_bMapBuild = TRUE;
	return TRUE;
}
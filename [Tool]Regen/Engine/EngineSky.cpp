// EngineSky.cpp: implementation of the CEngineSky class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EngineSky.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngineSky::CEngineSky()
{
	m_pSkyMesh = NULL;

}

CEngineSky::~CEngineSky()
{
	if(g_pExecutive)
		SAFE_RELEASE(m_pSkyMesh);

}

BOOL CEngineSky::CreateSky(char* filename,char* anifile)
{
	g_pExecutive->GetGeometry()->CreateSkyMesh(&m_pSkyMesh,filename,0);

	if(0 == m_pSkyMesh)
	{
		return FALSE;
	}

	m_pSkyMesh->SetViewportIndex(0);
	VECTOR3	v3SkyOffset = {0};
	m_pSkyMesh->SetOffset(
		&v3SkyOffset);
	m_pSkyMesh->CreateMotionList(
		1);
	m_pSkyMesh->AddMotion(
		anifile,
		0);
	return TRUE;
}

void CEngineSky::ChangeSkyAni(int ani)
{
	m_pSkyMesh->SetCurrentMotionIndex(ani);
}
void CEngineSky::RenderSky()
{
	g_pExecutive->GetGeometry()->BeginRender(0,g_bColor,0);
	g_pExecutive->GetGeometry()->RenderSkyMesh(m_pSkyMesh,0);
	g_pExecutive->GetGeometry()->EndRender();
}
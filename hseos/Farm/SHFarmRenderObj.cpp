/*********************************************************************

	 파일		: SHFarmRenderObj.cpp
	 작성자		: hseos
	 작성일		: 2007/05/07

	 파일설명	: xxx 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "SHFarmRenderObj.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHFarmRenderObj
//

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHFarmRenderObj Method																										  생성자
//
CSHFarmRenderObj::CSHFarmRenderObj()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHFarmRenderObj Method																										  파괴자
//
CSHFarmRenderObj::~CSHFarmRenderObj()
{
}

BOOL CSHFarmRenderObj::Init(EObjectKind kind, BASEOBJECT_INFO* pBaseObjectInfo, stFarmRenderObjInfo* pstFarmRenderObjInfo)
{
#ifndef _AGENTSERVER
	m_stFarmRenderObjInfo = *pstFarmRenderObjInfo;

	return CObject::Init(kind, pBaseObjectInfo);
#else
	return TRUE;
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Release Method																												 
//
VOID CSHFarmRenderObj::Release()
{
#if !defined(_AGENTSERVER)
	CObject::Release();
#endif
}
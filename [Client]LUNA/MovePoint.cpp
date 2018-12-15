// MovePoint.cpp: implementation of the CMovePoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MovePoint.h"
#include "GameResourceManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CMovePoint)
CMovePoint::CMovePoint()
{
	m_bInited = FALSE;
}

CMovePoint::~CMovePoint()
{

}

BOOL CMovePoint::Init()
{
	if(FALSE == m_PointModel.Init(
		GAMERESRCMNGR->m_GameDesc.MovePoint,
		0,
		eEngineObjectType_GameSystemObject,
		0,
		USHRT_MAX))
	{
		return FALSE;
	}

	m_PointModel.ApplyHeightField(TRUE);
	m_bInited = TRUE;
	return TRUE;
}

void CMovePoint::Release()
{
	m_PointModel.Release();
}

void CMovePoint::SetPoint(VECTOR3* pPos)
{
	if(m_bInited == FALSE)
	{
		// 090625 LUJ, 이동 커서가 초기화되지 않았으면 다시 시도한다
		if( FALSE == Init() )
		{
			return;
		}
	}

	m_PointModel.SetEngObjPosition(pPos);
	m_PointModel.Show();
}
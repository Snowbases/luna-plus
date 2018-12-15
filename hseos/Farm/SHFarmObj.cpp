/*********************************************************************

	 파일		: SHFarmObj.cpp
	 작성자		: hseos
	 작성일		: 2007/04/18

	 파일설명	: 농장 기본 물체 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "../SHMain.h"
#include "SHFarmObj.h"

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHFarmObj Method																											  생성자
//
CSHFarmObj::CSHFarmObj()
{
	m_pcsParent = NULL;
	ZeroMemory(m_pcsChild, sizeof(m_pcsChild));
	m_nChildNum = 0;
	m_nID = 0;
	m_nOwnerID = 0;
	m_eOwnState = OWN_STATE_EMPTY;
	m_nLife = 0;
	m_nGrade = 0;
	ZeroMemory(&m_stEvent, sizeof(m_stEvent));
	m_nEventKind = 0;
	m_pcsRenderObj = NULL;
	ZeroMemory(m_ppcsRenderObjEx,	sizeof(m_ppcsRenderObjEx));
	m_nDir = 0.0f;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHFarmObj Method																											  파괴자
//
CSHFarmObj::~CSHFarmObj()
{
	if (m_pcsRenderObj)
	{
		m_pcsRenderObj->Release();
	} 

	int nCnt = sizeof(m_ppcsRenderObjEx)/sizeof(m_ppcsRenderObjEx[0]);
	for(int i=0; i<nCnt; i++)
	{
		if (m_ppcsRenderObjEx[i])
		{
			m_ppcsRenderObjEx[i]->Release();
		}
	}
}

VOID CSHFarmObj::MainLoop()
{
	if (m_pcsChild)
	{
		for(int i=0; i<m_nChildNum; i++)
		{
			m_pcsChild[i]->MainLoop();
		}
	}
}
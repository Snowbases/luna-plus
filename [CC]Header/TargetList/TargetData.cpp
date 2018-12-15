// TargetData.cpp: implementation of the CTargetData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TargetData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTargetData::CTargetData()
{

}

CTargetData::~CTargetData()
{

}

void CTargetData::SetTarget(DWORD id,WORD flag,BYTE kind)
{
	TargetID = id;
	TargetFlag = flag;
	TargetKind = kind;
}

void CTargetData::SetTargetData(int n,TARGETDATA data)
{
	TargetData[n] = data;
}
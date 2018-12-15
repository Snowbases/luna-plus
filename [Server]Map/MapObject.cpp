// MapObject.cpp: implementation of the CMapObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapObject.h"

CMapObject::CMapObject()
{

}

CMapObject::~CMapObject()
{

}


void CMapObject::InitMapObject( MAPOBJECT_INFO* pInitInfo )
{
	memcpy( &m_MapObjectInfo, pInitInfo, sizeof(MAPOBJECT_INFO) );
	m_Level = 0;
}


DWORD CMapObject::GetLife()
{
	return m_MapObjectInfo.Life;
}

void CMapObject::SetLife(DWORD Life,BOOL bSendMsg)
{
	m_MapObjectInfo.Life = Life;
}

DWORD CMapObject::DoGetMaxLife()
{ 
	return m_MapObjectInfo.MaxLife; 
}

DWORD CMapObject::DoGetPhyDefense()
{ 
	return m_MapObjectInfo.PhyDefence;
}
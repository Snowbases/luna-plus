// MapObject.h: interface for the CMapObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPOBJECT_H__D84C4D07_428D_427E_A1F0_390A8401E3E4__INCLUDED_)
#define AFX_MAPOBJECT_H__D84C4D07_428D_427E_A1F0_390A8401E3E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Object.h"


class CMapObject : public CObject
{
	MAPOBJECT_INFO	m_MapObjectInfo;
	DWORD			m_Level;

public:
	CMapObject();
	virtual ~CMapObject();

	void InitMapObject( MAPOBJECT_INFO* pMOInfo );
	virtual float GetRadius()	{	return m_MapObjectInfo.Radius;	}
	virtual DWORD GetLife();
	virtual void SetLife(DWORD Life,BOOL bSendMsg = TRUE);
	virtual DWORD DoGetMaxLife();
	virtual DWORD DoGetPhyDefense();
	void SetLevel( DWORD Level )		{	m_Level = Level;	}

};

#endif // !defined(AFX_MAPOBJECT_H__D84C4D07_428D_427E_A1F0_390A8401E3E4__INCLUDED_)

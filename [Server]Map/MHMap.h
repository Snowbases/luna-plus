// MHMap.h: interface for the CMHMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHMAP_H__A2087CD2_73AB_4751_8B7E_D99BEA2111C6__INCLUDED_)
#define AFX_MHMAP_H__A2087CD2_73AB_4751_8B7E_D99BEA2111C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include "ServerTable.h"

class CTileManager;

struct MAPSERVERDESC
{
	WORD	MapNum;
	WORD	ChannelNum;
	BOOL	bVillage;
	BOOL	bPKAllow;
	WORD	MaxDBThread;
	WORD	MaxQueryInSameTime;
	BOOL	bAutoNoteAllow;

	MAPSERVERDESC()
	{
		MapNum		= 0;
		ChannelNum	= 1;
		bVillage	= FALSE;
		bPKAllow	= FALSE;
		MaxDBThread	= 4;
		MaxQueryInSameTime = 1024;
		bAutoNoteAllow = TRUE;
	}
};

class CMHMap  
{
	CTileManager* m_pTileManager;
	char m_szTileName[MAX_MAP_NUM][MAX_FILE_NAME];

//KES 040712 MapServerDesc
	MAPSERVERDESC	m_msDesc;

public:
	inline CTileManager* GetTileManager()	{	return m_pTileManager;	}

	CMHMap();
	virtual ~CMHMap();

	void InitMap(MAPTYPE MapNum);
	void LoadMapServerDesc( char* strFileName, MAPTYPE MapNum );
	void LoadMapChannel(char* strFileName, MAPTYPE MapNum);
	void Release();
	
	int GetTileWidth(MAPTYPE MapNum);

	BOOL IsInTile(int x, int y, CObject* pObject);
	BOOL CollisionTilePos(float x, float y, CObject* pObject);
	BOOL CollisionTilePos(float x, float y, MAPTYPE MapNum);
	BOOL CollisionTileCell(int x, int y, CObject* pObject);

	BOOL CollisionLine(VECTOR3* pStart,VECTOR3* pEnd, VECTOR3 * Target, CObject* pObject);
	BOOL CollisionCheck(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos, CObject* pObject);		// Ãæµ¹ÇÏ¸é TRUE¹ÝÈ¯ÇÏ°í pRtCollisonPos¿¡ »õ·Î¿î Å¸°Ù ¼³Á¤

	char* GetTileName(WORD MapNum){return m_szTileName[MapNum];}
	void TileNameSetting();
	WORD GetChannelNum() const {return m_msDesc.ChannelNum; }
	BOOL IsVillage() const { return m_msDesc.bVillage; }
	BOOL IsPKAllow() const { return m_msDesc.bPKAllow; }
	WORD GetMaxDBThread() const { return m_msDesc.MaxDBThread; }
	WORD GetMaxQueryInSameTime() const { return m_msDesc.MaxQueryInSameTime; }
	BOOL IsAutoNoteAllow() const { return m_msDesc.bAutoNoteAllow; }
};

#endif // !defined(AFX_MHMAP_H__A2087CD2_73AB_4751_8B7E_D99BEA2111C6__INCLUDED_)

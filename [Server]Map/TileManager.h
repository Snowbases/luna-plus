// TileManager.h: interface for the CTileManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILEMANAGER_H__5FD3CE62_5211_46E8_9C7B_4B5CB5D006CA__INCLUDED_)
#define AFX_TILEMANAGER_H__5FD3CE62_5211_46E8_9C7B_4B5CB5D006CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CObject;
class CTileGroup;
class CFixedTileInfo;

#define TILECOLLISON_DETAIL	50.f

class CTileManager  
{
	CYHHashTable<CTileGroup> m_TileGroupTable;
	CYHHashTable<CFixedTileInfo> m_FixedTileInfoTable;

public:
	CTileManager();
	virtual ~CTileManager();
	BOOL LoadFixedTileInfo(MAPTYPE MapNum, char* pMapFile);
	void ReadMapFile(MAPTYPE MapNum, char* filename);
	BOOL CollisionCheck(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos,CObject* pObject);		// Ãæµ¹ÇÏ¸é TRUE¹ÝÈ¯ÇÏ°í pRtCollisonPos¿¡ »õ·Î¿î Å¸°Ù ¼³Á¤

	CFixedTileInfo* GetFixedTileInfo(MAPTYPE);
	CFixedTileInfo* GetFixedTileInfo(CObject*);
	CTileGroup* GetTileGroup(CObject*);
	CTileGroup* GetTileGroup(DWORD TileGroupID);

	BOOL CreateTileGroup(DWORD TileGroupID, MAPTYPE);
	void DeleteTileGroup(DWORD TileGroupID);
	
	int GetTileWidth(MAPTYPE MapNum);

	BOOL IsInTile(int cellX, int cellY, CObject* pObject);
	BOOL CollisionTileWithTileIndex( int x, int y, CObject* pObject);
	BOOL CollisionTileWithTileIndex( int x, int y, MAPTYPE MapNum);
	BOOL CollisionTileWithPosition( float fx, float fy, CObject* pObject);
	BOOL CollisionTileWithPosition( float fx, float fy, MAPTYPE MapNum);

	BOOL CollisionLine(VECTOR3* pStart,VECTOR3* pEnd, VECTOR3 * Target, CObject* pObject);
};

#endif // !defined(AFX_TILEMANAGER_H__5FD3CE62_5211_46E8_9C7B_4B5CB5D006CA__INCLUDED_)

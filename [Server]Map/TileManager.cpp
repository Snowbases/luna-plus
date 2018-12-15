// TileManager.cpp: implementation of the CTileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileManager.h"
#include "FixedTile.h"
#include "FixedTileInfo.h"
#include "TileGroup.h"
#include "MHFile.h"
#include "Battle.h"
#include "Object.h"

CTileManager::CTileManager()
{
	//////////////////////////////////////////////////////////////////////////
	// test	
	{
		VECTOR3 vdir;
		SetVector3(&vdir,0,0,0);		// ???  => 90
		float angle = RADTODEG(VECTORTORAD(vdir));
		angle = angle;
	}
	{
		VECTOR3 vdir;
		SetVector3(&vdir,0,0,-1);		// 0
		float angle = RADTODEG(VECTORTORAD(vdir));
		angle = angle;
	}
	{
		VECTOR3 vdir;
		SetVector3(&vdir,1,0,0);		// 90
		float angle = RADTODEG(VECTORTORAD(vdir));
		angle = angle;
	}
	{
		VECTOR3 vdir;
		SetVector3(&vdir,0,0,1);		// 180
		float angle = RADTODEG(VECTORTORAD(vdir));
		angle = angle;
	}
	{
		VECTOR3 vdir;
		SetVector3(&vdir,-1,0,0);		// 270
		float angle = RADTODEG(VECTORTORAD(vdir));
		angle = angle;
	}


	m_TileGroupTable.Initialize(10);
	m_FixedTileInfoTable.Initialize(10);
}

CTileManager::~CTileManager()
{
	CTileGroup* pInfo; 
	m_TileGroupTable.SetPositionHead();
	while((pInfo = m_TileGroupTable.GetData()) != NULL)
	{
		delete	pInfo;
	}
	m_TileGroupTable.RemoveAll();

	CFixedTileInfo* pData = NULL;
	m_FixedTileInfoTable.SetPositionHead();
	while((pData = m_FixedTileInfoTable.GetData())!= NULL)
	{
		delete pData;
	}
	m_FixedTileInfoTable.RemoveAll();
}

BOOL CTileManager::LoadFixedTileInfo(MAPTYPE MapNum, char* pMapFile)
{	
	CMHFile file;
	char filename[256];
	sprintf(filename, "MurimMapInfo%d.txt", MapNum);
	if(file.Init(filename, "r", 1) == FALSE)
	{
		ReadMapFile(MapNum, pMapFile);
		return TRUE;
	}

	
	else
	{
		char Mapfile[64];
		while(1)
		{
			if(file.IsEOF() != FALSE)
				break;
			
			MapNum = file.GetWord();
			strcpy(Mapfile, g_pServerSystem->GetMap()->GetTileName(MapNum));
			ReadMapFile(MapNum, Mapfile);			
		}
	}
	
	file.Release();
	return TRUE; 
}

void CTileManager::ReadMapFile(MAPTYPE MapNum, char* filename)
{
	HANDLE	hFile = NULL;
	DWORD dwRead = 0;
	BYTE bResult = 0;

	int nTileWidth;
	int nTileHeight;

	hFile = CreateFile(filename,GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		nTileWidth = nTileHeight = 1024;
		CFixedTileInfo* pFixedTileInfo = new CFixedTileInfo;
		pFixedTileInfo->Init(nTileWidth, nTileHeight);
		
		CFixedTile *pTile;
		FIXEDTILE_ATTR Attr;
		Attr.uFixedAttr = 0;
		for(int i=0; i< (nTileHeight * nTileWidth); i++)
		{
			pTile = pFixedTileInfo->GetFixedTile() + i;
			pTile->InitTileAttrib(Attr);
		}
		
		CloseHandle(hFile);
		m_FixedTileInfoTable.Add(pFixedTileInfo, MapNum);
		return;
	}
	
	//속성파일 헤더 읽어드림...
	bResult = BYTE( ReadFile(hFile, &nTileWidth, sizeof(nTileWidth), &dwRead, NULL) );
	ASSERT(bResult);
	bResult = BYTE( ReadFile(hFile, &nTileHeight, sizeof(nTileHeight), &dwRead, NULL) );
	ASSERT(bResult);
	
	CFixedTileInfo* pFixedTileInfo = new CFixedTileInfo;
	pFixedTileInfo->Init(nTileWidth, nTileHeight);
	//속성 정보 읽어드림 
	CFixedTile *pTile;
	FIXEDTILE_ATTR Attr;
	for(int i=0; i< (nTileHeight * nTileWidth); i++)
	{
		pTile = pFixedTileInfo->GetFixedTile() + i;
		bResult = BYTE( ReadFile(hFile, &Attr , sizeof(WORD), &dwRead, NULL) );
		
		ASSERT(bResult);		
		ASSERT(!(bResult && dwRead == 0 ));
		
		pTile->InitTileAttrib(Attr);
	}
	
	CloseHandle(hFile);
	m_FixedTileInfoTable.Add(pFixedTileInfo, MapNum);
}

BOOL CTileManager::CreateTileGroup(DWORD TileGroupID, MAPTYPE MapNum)
{
	CFixedTileInfo* pFixedTileInfo = GetFixedTileInfo(MapNum);
	if(!pFixedTileInfo)
	{
		ASSERT(0);
		return FALSE;
	}

	CTileGroup* pInfo;
	pInfo = new CTileGroup;
	pInfo->Init(pFixedTileInfo);
	m_TileGroupTable.Add(pInfo, TileGroupID);
	
	return TRUE;
}

void CTileManager::DeleteTileGroup(DWORD TileGroupID)
{
	CTileGroup* pTileGroup = GetTileGroup(TileGroupID);
	ASSERT(pTileGroup);
	//타일 지울때 쩌구쩌구
	m_TileGroupTable.Remove(TileGroupID);
	delete pTileGroup;
}
CTileGroup* CTileManager::GetTileGroup(CObject* pObject)
{
	DWORD TGID = pObject->GetGridID();

	CTileGroup* pInfo = m_TileGroupTable.GetData(TGID);
//	ASSERT(pInfo);

	// 임시
	if(pInfo == NULL)
		pInfo = m_TileGroupTable.GetData((DWORD)0);

	return pInfo;
}
CTileGroup* CTileManager::GetTileGroup(DWORD TileGroupID)
{
	CTileGroup* pInfo = m_TileGroupTable.GetData(TileGroupID);
//	ASSERT(pInfo);
	
	// 임시
	if(pInfo == NULL)
		pInfo = m_TileGroupTable.GetData((DWORD)0);

	return pInfo;
}

CFixedTileInfo* CTileManager::GetFixedTileInfo(MAPTYPE MapNum)
{
	CFixedTileInfo* pInfo = m_FixedTileInfoTable.GetData(MapNum);

	if(pInfo == NULL)
	{			
		char filename[256];
		sprintf(filename,"system/map/%d.ttb",MapNum);
		LoadFixedTileInfo(MapNum,filename);
	}

	pInfo = m_FixedTileInfoTable.GetData(MapNum);

	ASSERT(pInfo);
	return pInfo;
}
CFixedTileInfo* CTileManager::GetFixedTileInfo(CObject* pObject)
{
	CTileGroup* pTileGroup = GetTileGroup(pObject);
	if(pTileGroup == NULL)
	{
		m_FixedTileInfoTable.SetPositionHead();
		return m_FixedTileInfoTable.GetData();
	}
	
	ASSERT(pTileGroup);
	CFixedTileInfo* pInfo = pTileGroup->GetFixedTileInfo();
	ASSERT(pInfo);
	return pInfo;
}

int CTileManager::GetTileWidth(MAPTYPE MapNum)
{
	CFixedTileInfo* pInfo = GetFixedTileInfo(MapNum);
	return pInfo->GetTileWidth();
}
BOOL CTileManager::IsInTile(int cellX, int cellY, CObject* pObject)
{
	CFixedTileInfo* pInfo = GetFixedTileInfo(pObject);
	return pInfo->IsInTile(cellX, cellY);
}
BOOL CTileManager::CollisionTileWithTileIndex( int x, int y, CObject* pObject)
{ 
	CFixedTileInfo* pInfo = GetFixedTileInfo(pObject);
	CFixedTile *pTile = pInfo->GetFixedTile(x, y);
	if(pTile == NULL)
		return TRUE;
	return pTile->IsCollisonTile();
}
BOOL CTileManager::CollisionTileWithTileIndex( int x, int y, MAPTYPE MapNum)
{
	CFixedTileInfo* pInfo = GetFixedTileInfo(MapNum);
	CFixedTile *pTile = pInfo->GetFixedTile(x, y);
	if(pTile == NULL)
		return TRUE;
	return pTile->IsCollisonTile();	
}
BOOL CTileManager::CollisionTileWithPosition( float fx, float fy, CObject* pObject)
{ 
	int x = int(fx / TILECOLLISON_DETAIL);
	int y = int(fy / TILECOLLISON_DETAIL);
	return CollisionTileWithTileIndex(x,y,pObject);
}
BOOL CTileManager::CollisionTileWithPosition( float fx, float fy, MAPTYPE MapNum)
{
	int x = int(fx / TILECOLLISON_DETAIL);
	int y = int(fy / TILECOLLISON_DETAIL);
	return CollisionTileWithTileIndex(x,y,MapNum);
}
BOOL CTileManager::CollisionLine(VECTOR3* pSrc,VECTOR3* pDest, VECTOR3 * pTarget, CObject* pObject)
{
	int x1 = int(pSrc->x / TILECOLLISON_DETAIL);
	int y1 = int(pSrc->z / TILECOLLISON_DETAIL);
	int x2 = int(pDest->x / TILECOLLISON_DETAIL);
	int y2 = int(pDest->z / TILECOLLISON_DETAIL);

	int dx = x2 - x1;
	int dy = y2 - y1;
	
	int absDX = abs(dx);
	int absDY = abs(dy);
	int MaxDelta = max(absDX, absDY);
	
	int CellX = x1;
	int CellY = y1;
	int x = 0;
	int y = 0;
	CFixedTileInfo* pFixedTile = GetFixedTileInfo(pObject);

	int signDX = sign(dx);
	int signDY = sign(dy);

	int PrevCellX = 0;
	int PrevCellY = 0;
	
	for( int i = 0 ; i <= MaxDelta ; ++i )
	{
		PrevCellX = CellX;
		PrevCellY = CellY;

		x += absDX;  
		y += absDY;
		
		if( x > MaxDelta)  
		{  
			x -= MaxDelta;
			CellX += signDX;
		}
		
		if( y > MaxDelta ) 
		{  
			y -= MaxDelta;  
			CellY += signDY;  
		}
		CFixedTile *pTile = pFixedTile->GetFixedTile(CellX, CellY);
		ASSERT(pTile);
		if(pTile == NULL)
		{
			if(pTarget)
			{
				pTarget->x = PrevCellX*TILECOLLISON_DETAIL;
				pTarget->y = 0;
				pTarget->z = PrevCellY*TILECOLLISON_DETAIL;
			}
			return TRUE;
		}
		if(pTile->IsCollisonTile())
		{
			if(pTarget)
			{
				pTarget->x = PrevCellX*TILECOLLISON_DETAIL;
				pTarget->y = 0;
				pTarget->z = PrevCellY*TILECOLLISON_DETAIL;
			}
			return TRUE;
		}
	}
	if(pTarget)
	{
		pTarget->x = x2*TILECOLLISON_DETAIL;
		pTarget->y = 0;
		pTarget->z = y2*TILECOLLISON_DETAIL;
	}

	return FALSE;
}
BOOL CTileManager::CollisionCheck(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos, CObject* pObject)
{	
	if( pStart->x >= 51200 || pStart->x < 0 ||
		pStart->z >= 51200 || pStart->z < 0 )
	{
//		ASSERTMSG(0,"이동영역을 벗어났습니다.");
		pRtCollisonPos->x = 25000;
		pRtCollisonPos->z = 25000;
		return TRUE;
	}

	int x0 = int(pStart->x / TILECOLLISON_DETAIL);
	int y0 = int(pStart->z / TILECOLLISON_DETAIL);
	int x1 = int(pEnd->x / TILECOLLISON_DETAIL);
	int y1 = int(pEnd->z / TILECOLLISON_DETAIL);
	
	int dx = x1-x0,dy = y1-y0;
	int sx,sy;
	if(dx >= 0)
	{
		sx = 1;
	}
	else
	{
		sx = -1;
		dx *= -1;
	}
	if(dy >= 0)
	{
		sy = 1;
	}
	else
	{
		sy = -1;
		dy *= -1;
	}
	
	int ax = 2*dx,ay = 2*dy;

	int x = x0, y = y0;
	int lastx = x, lasty = y;
	
	BOOL bFirst = TRUE;
	if(dx == 0 && dy == 0)
		bFirst = FALSE;
	
	CFixedTileInfo* pFixedTile = GetFixedTileInfo(pObject);

	if(dx >= dy)
	{
		for(int desc = ay - dx; ;x += sx,desc += ay)
		{			
			if(bFirst == TRUE)
			{
				bFirst = FALSE;
				continue;
			}

			CFixedTile *pTile = pFixedTile->GetFixedTile(x, y);
			ASSERT(pTile);
			if(pTile == NULL)
			{
				if(pRtCollisonPos)
				{					
					pRtCollisonPos->x = lastx*TILECOLLISON_DETAIL;
					pRtCollisonPos->y = 0;
					pRtCollisonPos->z = lasty*TILECOLLISON_DETAIL;
				}
				return TRUE;
			}

			if(pTile->IsCollisonTile())
			{
				if(pRtCollisonPos)
				{					
					pRtCollisonPos->x = lastx*TILECOLLISON_DETAIL;
					pRtCollisonPos->y = 0;
					pRtCollisonPos->z = lasty*TILECOLLISON_DETAIL;
				}
				return TRUE;
			}

			if(x == x1)
				break;

			lastx = x;
			lasty = y;

			if(desc > 0)
			{
				y += sy;
				desc -= ax;
			}
		}
	}
	else
	{
		for(int desc = ax - dy; ;y += sy,desc += ax)
		{
			if(bFirst == TRUE)
			{
				bFirst = FALSE;
				continue;
			}

			CFixedTile *pTile = pFixedTile->GetFixedTile(x, y);
			ASSERT(pTile);
			if(pTile == NULL)
			{
				if(pRtCollisonPos)
				{
					pRtCollisonPos->x = lastx*TILECOLLISON_DETAIL;
					pRtCollisonPos->y = 0;
					pRtCollisonPos->z = lasty*TILECOLLISON_DETAIL;
				}
				return TRUE;
			}

			if(pTile->IsCollisonTile())
			{
				if(pRtCollisonPos)
				{
					pRtCollisonPos->x = lastx*TILECOLLISON_DETAIL;
					pRtCollisonPos->y = 0;
					pRtCollisonPos->z = lasty*TILECOLLISON_DETAIL;
				}
				return TRUE;
			}

			if(y == y1)
				break;
			
			lastx = x;
			lasty = y;

			if(desc > 0)
			{
				x += sx;
				desc -= ay;
			}
		}
	}
	return FALSE;
}
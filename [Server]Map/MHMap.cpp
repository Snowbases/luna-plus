// MHMap.cpp: implementation of the CMHMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MHMap.h"
#include "TileManager.h"
#include "MHFile.h"
#include "PKManager.h"


extern int	g_nServerSetNum;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMHMap::CMHMap()
{
	m_pTileManager = NULL;

}

CMHMap::~CMHMap()
{

}

void CMHMap::InitMap( MAPTYPE MapNum )
{
	m_pTileManager = new CTileManager;
	TileNameSetting();	// Å¸ÀÏ ÀÌ¸§ ¼ÂÆÃ
	
	char MapFile[64];
	strcpy(MapFile, m_szTileName[MapNum]);
	m_pTileManager->LoadFixedTileInfo(MapNum, MapFile);

//KES 040712 MapServerDesc
	char filename[256];
	sprintf(filename,"serverset/%d/MapServerDesc.txt",g_nServerSetNum);
	LoadMapServerDesc( filename, MapNum );

	sprintf(filename,"system/MapChannel.bin");
	LoadMapChannel( filename, MapNum);
}

void CMHMap::LoadMapServerDesc( char* strFileName, MAPTYPE MapNum )
{
	m_msDesc.MapNum = MapNum;
	bool isFound	= false;
	char buf[256];
	CMHFile file;

	if( file.Init( strFileName,"rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
	{
		while( ! file.IsEOF() )
		{
			file.GetString( buf );

			if( strcmp( buf, "$MAPNUM" ) == 0 &&
				file.GetWord() == MapNum )
			{
				file.GetLine( buf, 256 );
				file.GetString( buf );

				if( buf[0] == '{' )
				{
					file.GetLine( buf, 256 );

					while( ! file.IsEOF() )
					{
						file.GetString( buf );

						if( strcmp( buf, "*VILLAGE" ) == 0 )
						{
							m_msDesc.bVillage = file.GetBool();
						}
						else if( strcmp( buf, "*PKALLOW" ) == 0 )
						{
							m_msDesc.bPKAllow = file.GetBool();
							// 090824 ONS PK메니져의 기본값으로 설정.
							PKMGR->SetPKAllow( m_msDesc.bPKAllow );
						}
						else if( strcmp( buf, "*MAXTHREAD" ) == 0 )
						{
							m_msDesc.MaxDBThread = file.GetWord();
						}
						else if( strcmp( buf, "*MAXQUERY" ) == 0 )
						{
							m_msDesc.MaxQueryInSameTime = file.GetWord();
						}
						else if( strcmp( buf, "*AUTONOTE" ) == 0 )
						{
							m_msDesc.bAutoNoteAllow = file.GetBool();
						}
						else if( buf[0] == '}' )
						{
							break;
						}
					}
				}

				isFound	= true;
				break;
			}
		}
	}
	file.Release();

	if( false == isFound )
	{
		char buf[ 1024 ];
		sprintf( buf, "맵 번호 %d에 대한 설정 정보가 %s에 없습니다", MapNum, strFileName );
		ASSERTMSG( isFound, buf );
	}
}

void CMHMap::LoadMapChannel(char* strFileName, MAPTYPE MapNum)
{
	bool isFound	= false;
	char buf[256];
	CMHFile file;
	int nMapNum;

	if( file.Init( strFileName,"rb") )
	{
		while( ! file.IsEOF() )
		{
			file.GetString( buf );

			if( strcmp( buf, "$MAP_CHANNEL" ) == 0)
			{
				nMapNum = file.GetWord();

				if(nMapNum == MapNum)
				{
					m_msDesc.ChannelNum = file.GetWord();
					isFound = true;
				}
			}
		}
	}
	file.Release();

	if( false == isFound )
	{
		char buf[ 80 ];
		sprintf( buf, "맵번호 %d에 대한 채널정보가 %s에 없습니다", MapNum, strFileName );
		ASSERTMSG( isFound, buf );
	}
}

void CMHMap::TileNameSetting()
{
	for(int i = 0; i < MAX_MAP_NUM; i++)
	{
		memset(m_szTileName[i], 0, sizeof(MAX_FILE_NAME));
		sprintf(m_szTileName[i], "system/map/%d.ttb",i);
	}	
}

void CMHMap::Release()
{
	SAFE_DELETE(m_pTileManager);
}
int CMHMap::GetTileWidth(MAPTYPE MapNum)
{ 
	if(m_pTileManager == NULL)
		return 0;

	return m_pTileManager->GetTileWidth(MapNum);
}
BOOL CMHMap::CollisionLine(VECTOR3* pStart,VECTOR3* pEnd, VECTOR3* Target, CObject* pObject)
{
	if(m_pTileManager == NULL)
		return FALSE;

	return m_pTileManager->CollisionLine(pStart, pEnd, Target, pObject);
}

BOOL CMHMap::IsInTile(int cellX, int cellY, CObject* pObject)
{
	if(m_pTileManager == NULL)
		return FALSE;

	if(m_pTileManager->IsInTile(cellX, cellY, pObject))
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CMHMap::CollisionTileCell(int x, int y, CObject* pObject)
{
	if(m_pTileManager == NULL)
		return FALSE;

	if(m_pTileManager->CollisionTileWithTileIndex(x, y, pObject))
	{
		return FALSE;
	}
	return TRUE;
}
BOOL CMHMap::CollisionTilePos(float x, float y, CObject* pObject)
{
	if(m_pTileManager == NULL)
		return FALSE;

	if(m_pTileManager->CollisionTileWithPosition(x, y, pObject))
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CMHMap::CollisionTilePos(float x, float y, MAPTYPE MapNum)
{
	if(m_pTileManager == NULL)
		return FALSE;

	if(m_pTileManager->CollisionTileWithPosition(x, y, MapNum))
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CMHMap::CollisionCheck(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos, CObject* pObject)
{
	if(m_pTileManager == NULL)
		return FALSE;
	if(m_pTileManager->CollisionCheck(pStart,pEnd,pRtCollisonPos, pObject) == TRUE)
		return TRUE;
	
	


	return FALSE;
}

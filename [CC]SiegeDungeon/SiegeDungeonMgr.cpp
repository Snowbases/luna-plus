// SiegeDungeonMgr.cpp: implementation of the CSiegeDungeonMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SiegeDungeonMgr.h"
#include "MHFile.h"

#ifdef _MAPSERVER_
#include "Network.h"
#include "ServerTable.h"
#include "PackedData.h"
#endif

CSiegeDungeonMgr::CSiegeDungeonMgr()
{
	m_wMapType = eSiegeDungeonType_Max;
	sizeof(
		m_dwKindMapList,
		sizeof(m_dwKindMapList));
}

CSiegeDungeonMgr::~CSiegeDungeonMgr()
{
}

VOID CSiegeDungeonMgr::LoadInfo()
{
	CMHFile fp;
	char szFile[256] = {0,};
	char szLine[256] = {0,};
	sprintf(szFile, "./System/Resource/SiegeDungeon.bin");
	fp.Init(szFile, "rb");
	int nKind = 0;

	if(!fp.IsInited())
	{
		char szTmp[256];
		sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while(TRUE)
	{
		if (fp.IsEOF()) break;
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if (strstr(szLine, "END")) 
		{
			break;
		}

		sscanf(szLine, "%d", &m_dwKindMapList[nKind]);

		nKind++;
		
		if( nKind >= eSiegeDungeonType_Max )
			break;
	}

	nKind = 0;

	sprintf(szFile, "./System/Resource/SiegeDungeonPanaltyInfo.bin");
	fp.Init(szFile, "rb");

	if(!fp.IsInited())
	{
		char szTmp[256];
		sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while(TRUE)
	{
		if (fp.IsEOF()) break;
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if (strstr(szLine, "END")) 
		{
			break;
		}

		DWORD mapType = 0, panaltyType = 0, value = 0;
		sscanf(szLine, "%d %d %d", &mapType, &panaltyType, &value);

		switch( panaltyType )
		{
		case eDeathPanalty_DecreaseExp:
			m_stPanalty[mapType].DecreaseExp = value;
			break;
		case eDaathPanalty_CanItemLooting:
			m_stPanalty[mapType].CanItemLooting = (BOOL)value;
			break;
		}
	}

	fp.Release();
}

// 081009 LYW --- SiegeDungeonMgr : 공성소스 Merge
BOOL CSiegeDungeonMgr::IsSiegeDungeon(MAPTYPE MapNum)
{
	for( int i = eSiegeDungeonType_Nera; i < eSiegeDungeonType_Max; i++ )
	{
		if( m_dwKindMapList[i] == MapNum )
		{
			m_wMapType = WORD( i );
			return TRUE;
		}
	}

	return FALSE;
}

stSiegeDungeonDeathPanaltyInfo* CSiegeDungeonMgr::GetPanalty(MAPTYPE MapNum)
{
	WORD MapType = eSiegeDungeonType_Max;
	for(int i = eSiegeDungeonType_Nera; i < eSiegeDungeonType_Max; i++ )
	{
		if( m_dwKindMapList[i] == MapNum )
		{
			MapType = (WORD)i;
		}
	}

	if( MapType == eSiegeDungeonType_Max )
		return NULL;
	//g_pServerSystem->GetMap()->IsPKAllow();
	return &m_stPanalty[MapType];
}

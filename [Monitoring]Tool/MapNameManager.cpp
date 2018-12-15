// MapNameManager.cpp: implementation of the CMapNameManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapNameManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CMapNameManager)

CMapNameManager::CMapNameManager()
{
	// 080820 LUJ, 맵 이름을 읽는다
	{	
		CMHFile file;
		if( !file.Init( "./Data/MapName.txt", "rt" ) )
			return;
	
		const DWORD dwMax = file.GetDword();
	
		for( DWORD i = 0; i < dwMax; ++i )
		{	
			const DWORD dwMapNum = file.GetInt();
	
			char buffer[ MAX_PATH ] = { 0 };
			file.GetStringInQuotation( buffer );
	
			m_MapNameTable.insert( std::make_pair( dwMapNum, CString( buffer ) ) );
		}
	}	
}

CMapNameManager::~CMapNameManager()
{}
	
const TCHAR* CMapNameManager::GetMapName( DWORD dwMapNum )
{	
	const MapNameTable::const_iterator it = m_MapNameTable.find( dwMapNum );
	
	return m_MapNameTable.end() == it ? _T( "" ) : it->second;
}
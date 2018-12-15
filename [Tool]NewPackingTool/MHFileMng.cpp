#define __MHFILEMNG_C__

#include "stdafx.h"
#include "MHFileMng.h"

CMHFileMng::CMHFileMng()
{
	m_nFileNum = 0;
}

CMHFileMng::~CMHFileMng()
{
	Clear();
}

BOOL CMHFileMng::AddFile( const char* fullfilename, char* mode )
{
	if( fullfilename == NULL ) return FALSE;

	CMHFileEx* pFile = new CMHFileEx;

	if( !_tcscmp(mode, _T("rt")) )
	{
		if( pFile->OpenTxt( fullfilename ) )
		{
			m_mapFile.insert( make_pair( m_nFileNum, pFile ) );
			++m_nFileNum;
			return TRUE;
		}
	}
	else if( !_tcscmp(mode, _T("rb")) )
	{
		if( pFile->OpenBin( fullfilename ) )
		{
			m_mapFile.insert( make_pair( m_nFileNum, pFile ) );
			++m_nFileNum;
			return TRUE;
		}
	}
	else if( !_tcscmp(mode, _T("dof")) )
	{
		if( pFile->OpenDOF( fullfilename ) )
		{
			m_mapFile.insert( make_pair( m_nFileNum, pFile ) );
			++m_nFileNum;
			return TRUE;
		}
	}

	return FALSE;
}

void CMHFileMng::DeleteFile( int index )
{
	index;
}

void CMHFileMng::Clear()
{
	mi = m_mapFile.begin();
	while( mi != m_mapFile.end() )
	{
		if( (*mi).second )
		{
			delete (*mi).second;
			(*mi).second = NULL;
		}
		++mi;
	}
	m_mapFile.clear();
	m_nFileNum = 0;
}

char* CMHFileMng::GetFullFileName( int index )
{
	mi = m_mapFile.find( index );
	if( mi != m_mapFile.end() )
		return (*mi).second->GetFullFileName();

	return NULL;
}

char* CMHFileMng::GetFileName( int index )
{
	mi = m_mapFile.find( index );
	if( mi != m_mapFile.end() )
		return (*mi).second->GetFileName();

	return NULL;
}

void CMHFileMng::SetExt( const char* ext )
{
	mi = m_mapFile.begin();
	while( mi != m_mapFile.end() )
	{
		(*mi).second->SetExt( ext );
		++mi;
	}
}

BOOL CMHFileMng::Save( int index, bool bAES )
{
	mi = m_mapFile.find( index );

	if( mi != m_mapFile.end() )
	{
		TCHAR strDir[_MAX_DIR];
		TCHAR strDrv[_MAX_DRIVE];
		TCHAR strFileName[200];
		TCHAR strExt[20];

		_tsplitpath( (*mi).second->GetFileName(), strDrv, strDir, strFileName, strExt );

		strupr(strExt);
		
		if( strcmp( strExt, ".BIN" ) == 0 || 
			strcmp( strExt, ".BEFF" ) == 0 || 
			strcmp( strExt, ".BEFL" ) == 0 || 
			strcmp( strExt, ".BMHM" ) == 0 || 
			strcmp( strExt, ".BSAD" ) == 0 )
		{
			(*mi).second->SaveToBin(bAES);
		}
		else if( strcmp( strExt, ".DOF" ) == 0 )
		{
			(*mi).second->SaveToDOF();
		}
		else
		{
			(*mi).second->SaveToTxt();
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CMHFileMng::SaveAs( int index, const char* filename, bool bAES )
{
	mi = m_mapFile.find( index );

	if( mi != m_mapFile.end() )
	{
		TCHAR strDir[_MAX_DIR];
		TCHAR strDrv[_MAX_DRIVE];
		TCHAR strFileName[200];
		TCHAR strExt[20];

		_tsplitpath( filename, strDrv, strDir, strFileName, strExt );

		strupr(strExt);

		if( strcmp( strExt, ".BIN" ) == 0 || 
			strcmp( strExt, ".BEFF" ) == 0 || 
			strcmp( strExt, ".BEFL" ) == 0 || 
			strcmp( strExt, ".BMHM" ) == 0 || 
			strcmp( strExt, ".BSAD" ) == 0 )
		{
			(*mi).second->SaveToBin(filename, bAES);
		}
		else if( strcmp( strExt, ".DOF" ) == 0 )
		{
			(*mi).second->SaveToDOF(filename);
		}
		else
		{
			(*mi).second->SaveToTxt(filename);
		}

		return TRUE;
	}

	return FALSE;
}

void CMHFileMng::SaveToBin()
{
	mi = m_mapFile.begin();
	while( mi != m_mapFile.end() )
	{
		(*mi).second->SaveToBin();
		++mi;
	}
}

void CMHFileMng::SaveAsBin( int index, const char* filename )
{
	mi = m_mapFile.find( index );
	if( mi != m_mapFile.end() )
		(*mi).second->SaveToBin( filename );
}

void CMHFileMng::SaveToTxt()
{
	mi = m_mapFile.begin();
	while( mi != m_mapFile.end() )
	{
		(*mi).second->SaveToTxt();
		++mi;
	}
}

CMHFileEx* CMHFileMng::GetFile( int index )
{
	mi = m_mapFile.find( index );
	if( mi != m_mapFile.end() )
		return (*mi).second;

	return NULL;
}

void CMHFileMng::New( const char* fullfilename )
{
	if( fullfilename == NULL ) return;

	CMHFileEx* pFile = new CMHFileEx;
	pFile->New( fullfilename );

	m_mapFile.insert( make_pair( m_nFileNum, pFile ) );
	++m_nFileNum;
}

void CMHFileMng::SetFileData( int index, char* str )
{
	mi = m_mapFile.find( index );
	if( mi != m_mapFile.end() )
		(*mi).second->SetData( str );
}

void CMHFileMng::SaveToDOF()
{
	mi = m_mapFile.begin();
	while( mi != m_mapFile.end() )
	{
		(*mi).second->SaveToDOF();
		++mi;
	}
}
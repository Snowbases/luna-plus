#include "stdafx.h"
#include "DirectoryName.h"
#include "YHFileFind.h"


CDirectoryName::CDirectoryName(CFileName* pParent,CYHFileFind* pFileFind)
: CFileName(pParent,pFileFind)
{
	const int length = m_FileName.GetLength();

	if( length )
	{
		if(m_FileName.GetAt( length - 1 ) != '\\' )
		{
			m_FileName += _T( "\\" );
		}
	}

	m_bIsDirectory = TRUE;
}

CDirectoryName::~CDirectoryName()
{
	Release();
}

void CDirectoryName::Release()
{
	for(
		POSITION position = mChildList.GetHeadPosition();
		position; )
	{
		CFileName* name = mChildList.GetNext( position );

		if( ! name )
		{
			continue;
		}

		delete name;
	}

	mChildList.RemoveAll();
}

void CDirectoryName::LoadChild( DWORD flag )
{
	CYHFileFind fileFind;
	fileFind.FindFile();

	while( fileFind.FindNextFile() )
	{
		if( fileFind.GetFileName() == _T( "." ) ||
			fileFind.GetFileName() == _T( ".." ) )
		{
			continue;
		}
		else if( fileFind.IsDirectory() )
		{
			if( ! ( flag & LCO_DIRECTORY ) )
			{
				continue;
			}

			CFileName* file = new CDirectoryName( this, &fileFind );

			mChildList.AddTail( file );
		}
		else
		{
			if( ! ( flag & LCO_FILE ) )
			{
				continue;
			}

			CFileName* file = new CFileName( this, &fileFind );

			mChildList.AddTail( file );
		}		
	}

	for(
		POSITION position = mChildList.GetHeadPosition();
		position; )
	{
		CFileName* file = mChildList.GetNext( position );

		if( ! file ||
			! file->IsDirectory() )
		{
			continue;
		}

		CDirectoryName* directory = ( CDirectoryName* )file;

		SetCurrentDirectory( directory->GetFileName() );
		directory->LoadChild();
		SetCurrentDirectory( _T( ".." ) );
	}
}

void CDirectoryName::SetPositionHead()
{
	mResult		= FALSE;
	mPosition	= mChildList.GetHeadPosition();

	for(
		POSITION position = mChildList.GetHeadPosition();
		position; )
	{
		CFileName* name = mChildList.GetNext( position );

		if( ! name ||
			! name->IsDirectory() )
		{
			continue;
		}
        
		( ( CDirectoryName* )name )->SetPositionHead();
	}
}

DWORD CDirectoryName::GetNextFileName( CString& pFileNameOut )
{
	if( ! mResult )
	{
		pFileNameOut	= GetFullPathName();
		mResult	= TRUE;
		return GETNEXTFILENAME_DIRECTORY;
	}
	else if( ! mPosition )
	{
		return GETNEXTFILENAME_END;
	}

	CFileName* name = mChildList.GetAt( mPosition );

	if( ! name )
	{
		return GETNEXTFILENAME_END;
	}
	else if( name->IsDirectory() )
	{
		const DWORD result = ( ( CDirectoryName* )name )->GetNextFileName( pFileNameOut );

		if( result )
		{
			return TRUE;
		}
		
		mChildList.GetNext( mPosition );

		return GetNextFileName( pFileNameOut );
	}

	pFileNameOut = name->GetFullPathName();

	mChildList.GetNext( mPosition );

	return GETNEXTFILENAME_FILE;
}
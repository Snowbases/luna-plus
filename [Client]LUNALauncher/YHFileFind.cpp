#include "stdafx.h"
#include "YHFileFind.h"

CYHFileFind::CYHFileFind() :
m_hFileFind( 0 )
{}

CYHFileFind::~CYHFileFind()
{
	if( m_hFileFind )
	{
		FindClose( m_hFileFind );
	}
}

BOOL CYHFileFind::FindFile( const TCHAR* filter )
{
	m_hFileFind = ::FindFirstFile(
		filter ? filter : _T( "*.*" ),
		&file );

	return m_hFileFind != 0;
}
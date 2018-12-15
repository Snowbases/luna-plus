// stdafx.cpp : source file that includes just the standard includes
//	MHAutoPatch.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <conio.h>


CLog::CLog()
{
	::AllocConsole();
}


CLog::~CLog()
{
	::FreeConsole();
}


void CLog::Put( const TCHAR* text, ... ) const
{
	TCHAR buffer[ MAX_PATH * 5 ] = { 0 };

	va_list vl;
	va_start( vl, text );
	vsprintf( buffer, text, vl );
	va_end( vl );

	_cprintf( "%d\t%s\n", GetTickCount(), buffer );
}


const CLog& CLog::GetInstance()
{
	static CLog instance;

	return instance;
}
// stdafx.cpp : source file that includes just the standard includes
//	ServerTool.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

CString GetDataDirectory()
{
	TCHAR buffer[ MAX_PATH ] = { 0 };
	GetCurrentDirectory( sizeof( buffer ), buffer );

	return CString( buffer ) + _T( "/Data/" );
}

CString GetUserCountDirectory()
{
	TCHAR buffer[ MAX_PATH ] = { 0 };
	GetCurrentDirectory( sizeof( buffer ), buffer );

	return CString( buffer ) + _T( "/UserCount/" );
}

void LOG( LPCTSTR msg, ... )
{
	TCHAR buff[ MAX_PATH * 5 ] = { 0 };
	
	va_list vl;
	va_start( vl, msg );
	_vstprintf( buff, msg, vl );
	va_end( vl );

	SYSTEMTIME time = { 0 };
	TCHAR szTime[ MAX_PATH * 5 ] = { 0 };
	GetLocalTime( &time );
	_stprintf(
		szTime,
		_T( "%02d.%02d.%02d %02d:%02d:%02d" ),
		time.wYear,
		time.wMonth,
		time.wDay,
		time.wHour,
		time.wMinute,
		time.wSecond );

	FILE* file = _tfopen( _T( "Log.txt" ), _T( "a+" ) );
	if( file )
	{
		_ftprintf( file, _TEXT( "[%s]%s\n" ), szTime, buff );
		fclose( file );
	}
}

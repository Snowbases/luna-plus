// stdafx.cpp : 표준 포함 파일을 포함하는 소스 파일입니다.
// LunaGameDataExtracter.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj는 미리 컴파일된 형식 정보를 포함합니다.

#include "stdafx.h"



void OutputDebug( char* pMsg, ... )
{
#ifdef _DEBUG
	if( pMsg == NULL )
		return;

	char msg[512] = {0,};
	
	
	va_list vList;
	va_start( vList, pMsg );
	vsprintf( msg, pMsg, vList );
	va_end( vList );

	strcat( msg, "\n" );

	OutputDebugString( msg );
#endif
}

void OutputFile( char* szFileName, char* pMsg, ... )
{
#ifdef _DEBUG
	if( szFileName == NULL ||
		pMsg == NULL )
		return;

	char msg[512] = {0,};
	
	va_list vList;
	va_start( vList, pMsg );
	vsprintf( msg, pMsg, vList );
	va_end( vList );

	// File Log
	FILE* fp = NULL;
	fp = fopen( szFileName, "a+" );
	if( fp )
	{
		SYSTEMTIME sysTime;
		GetLocalTime( &sysTime );

		fprintf( fp, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n", sysTime.wYear, 
															sysTime.wMonth, 
															sysTime.wDay, 
															sysTime.wHour, 
															sysTime.wMinute, 
															sysTime.wSecond, 
															msg );

		fclose( fp );
	}
#endif
}

void ChangeLunaGameString( CString& strFilename )
{
	strFilename.Replace( "^s", " " );
	strFilename.Replace( "\"", "" );
}

CString GetChangedLunaGameString( CString strFilename )
{
	ChangeLunaGameString( strFilename );
	return strFilename;
}

void RemoveNotAvailibleFilename( CString& strFilename )
{
	// 파일이름으로 사용하지 못하는 문자제거
	strFilename.Replace( "\\", "" );
	strFilename.Replace( "/", "" );
	strFilename.Replace( ":", "" );
	strFilename.Replace( "*", "" );
	strFilename.Replace( "?", "" );
	strFilename.Replace( "\"", "" );
	strFilename.Replace( "<", "" );
	strFilename.Replace( ">", "" );
	strFilename.Replace( "|", "" );
}

LPCWSTR MultiToWideChar( LPCTSTR string )
{
	static WCHAR wideChar[1024];

	MultiByteToWideChar( CP_ACP, 0, string, _tcslen(string)+1, wideChar, sizeof(wideChar)/sizeof(*wideChar) );

	return wideChar;
}

void OutputMessageBox( char* pMsg, ... )
{
	if( pMsg == NULL )
		return;

	char msg[512] = {0,};
	
	va_list vList;
	va_start( vList, pMsg );
	vsprintf( msg, pMsg, vList );
	va_end( vList );

	AfxMessageBox( msg, 0, 0 );
}

// 출처: http://www.gpgstudy.com/forum/viewtopic.php?t=795&highlight=%C7%D8%BD%AC+%B9%AE%C0%DA%BF%AD
DWORD GetHashCodeFromTxt( const char* txt )
{
	const DWORD dwLength = (DWORD)strlen( txt );
	DWORD dwHashCode = 5381;

	for(DWORD i = 0 ; i < dwLength ; ++i )
	{
		DWORD ch = DWORD( txt[ i ] );
		dwHashCode = ((dwHashCode << 5) + dwHashCode) + ch;
	}

	return dwHashCode;
}

void AddComma( char* pBuf )
{
	if( *pBuf == 0 ) return;

	char buf[64];
	strncpy( buf, pBuf, 64 );
	char* p = buf;

	*(pBuf++) = *(p++);

	int n = strlen( pBuf ) % 3;

	while( *p )
	{
		if( n != 0 )
		{
			*pBuf = *p;
			++p;
		}
		else
		{
			*pBuf = ',';
		}

		++pBuf;
		n = ( n == 0 ) ? 3 : n - 1;
	}
	
	*pBuf = 0;
}

char* AddComma( unsigned long long dwMoney )
{
	static char buf[32];

	wsprintf( buf, "%I64d", dwMoney );
	AddComma( buf );
	return buf;
}

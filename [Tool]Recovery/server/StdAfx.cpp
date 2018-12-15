// stdafx.cpp : source file that includes just the standard includes
//	MonitoringServer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


void WriteAssertMsg(TCHAR* pStrFileName,int Line,TCHAR* pMsg)
{
}


void PutLog( const TCHAR* text, ... )
{
	SYSTEMTIME time;
	::GetLocalTime( &time );

	TCHAR log[ 2048 ] = { 0 };
	{
		TCHAR buffer[ 2048 ] = { 0 };
		{
			va_list argument;
			va_start( argument, text );
			_vstprintf( buffer, text, argument );
			va_end( argument);
		}

		_stprintf(
			log,
			_T( "[%04d.%02d.%02d %02d:%02d:%02d] %s\n" ),
			time.wYear,
			time.wMonth,
			time.wDay,
			time.wHour,
			time.wMinute,
			time.wSecond,
			buffer );
	}	

	_tcprintf( log );

	// 파일 출력
	{
		// 080702 LUJ, 로그 파일을 log 폴더 안에 저장되도록 한다
		::CreateDirectory( _T( "log" ), 0 );

		std::string name;
		{
			const std::string prefix( _T( "log\\RecoveryServer" ) );

			TCHAR infix[ MAX_PATH ] = { 0 };
			_stprintf(
				infix,
				"%04d%02d%02d",
				time.wYear,
				time.wMonth,
				time.wDay );

			const std::string postfix( _T( "log" ) );

			name = prefix + "." + infix + "." + postfix;
		}

		std::ofstream file( name.c_str(), std::ios_base::app );

		if( ! file )
		{
			file.open( name.c_str() );

			if( ! file )
			{
				return;
			}
		}

        file << log;
	}
}
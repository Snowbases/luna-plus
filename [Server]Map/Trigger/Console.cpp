#include "stdafx.h"
#include "Console.h"

namespace Trigger
{
	CConsole::CConsole()
	{}

	CConsole::~CConsole()
	{}

	void CConsole::Put(LPCTSTR text, ...) const
	{
		SYSTEMTIME time = { 0 };
		::GetLocalTime( &time );

		TCHAR log[ 2048 ] = { 0 };
		{
			TCHAR buffer[ 2048 ] = { 0 };
			va_list argument;
			va_start( argument, text );
			_vstprintf( buffer, text, argument );
			va_end( argument);

			_stprintf(
				log,
				_T( "%04d.%02d.%02d %02d:%02d:%02d|trigger| %s\n" ),
				time.wYear,
				time.wMonth,
				time.wDay,
				time.wHour,
				time.wMinute,
				time.wSecond,
				buffer );
		}

		_tcprintf( log );
	}
}
// stdafx.cpp : source file that includes just the standard includes
//	MHAutoPatch.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

bool CLog::mIsEnable;

CLog::CLog()
{
	::AllocConsole();
}

CLog::~CLog()
{
	::FreeConsole();
}

void CLog::SetEnable( bool isEnable )
{
	mIsEnable = isEnable;
}

void CLog::Put( const TCHAR* text, ... )
{
#ifndef _DEBUG
	if( ! mIsEnable )
	{
		return;
	}
#endif

	static CLog instance;

	const CTime time( CTime::GetCurrentTime() );

	CString log;

	TCHAR buffer[ MAX_PATH * 5 ] = { 0 };
	{
		va_list argument;
		va_start( argument, text );
		_vstprintf( buffer, text, argument);
		va_end( argument);
	}

	log.Format(
		_T( "%04d.%02d.%02d %02d:%02d:%02d> %s\r\n" ),
		time.GetYear(),
		time.GetMonth(),
		time.GetDay(),
		time.GetHour(),
		time.GetMinute(),
		time.GetSecond(),
		buffer );

	_tcprintf( log );

	// 파일 출력
	{
		CString name;
		{
			CString infix;
			infix.Format(
				_T( "%04d%02d%02d" ),
				time.GetYear(),
				time.GetMonth(),
				time.GetDay() );
			
			name = CString( _T( "LunaLauncher" ) ) + _T( "." ) + infix + _T( "." ) + _T( "log" );
		}		

		CStdioFileEx file;

		// 쓰기 우선으로 열되, 없으면 생성한다
		if( !	file.Open( name, CFile::modeWrite  ) &&
			!	file.Open( name, CFile::modeCreate | CFile::modeWrite ) )
		{
			return;
		}

		file.Seek( file.GetLength(), 0 );
		file.WriteString( log );
	}
}

void MsgBoxLog( LPCTSTR msg, LPCTSTR caption, HWND hwnd )
{
#ifdef _FOR_SERVER_
	CStdioFileEx file;

	if( !	file.Open( _T("./Log/AutoPatchLog.txt"), CFile::modeWrite  ) &&
		!	file.Open( _T("./Log/AutoPatchLog.txt"), CFile::modeCreate | CFile::modeWrite ) )
	{
		return;
	}

	SYSTEMTIME sysTime;
	GetLocalTime( &sysTime );

	CString log;
	log.Format( _T("[Date : %04d-%02d-%02d %02d:%02d:%02d] %s\r\n"), 
				sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
				msg );

	file.Seek( file.GetLength(), CFile::begin );
	file.WriteString( log );
#else
	::MessageBox( hwnd, msg, caption, MB_OK );
#endif
}

void DrawText(CxImage& image, const CPoint& point, LPCTSTR text, const RGBQUAD& color, long size, LPCTSTR font, long weight, long outline, const RGBQUAD& outlineColor)
{
	for(long x = point.x - outline; x < point.x + outline + 1; ++x)
	{
		for(long y = point.y - outline; y < point.y + outline + 1; ++y)
		{
			image.DrawString(
				0,
				x,
				y,
				text,
				outlineColor,
				font,
				size,
				weight,
				0,
				0,
				false);
		}
	}

	image.DrawString(
		0,
		point.x,
		point.y,
		text,
		color,
		font,
		size,
		weight,
		0,
		0,
		false);
}
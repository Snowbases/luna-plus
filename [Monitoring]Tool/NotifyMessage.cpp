// NotifyMessage.cpp: implementation of the CNotifyMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerTool.h"
#include "NotifyMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotifyMessage::CNotifyMessage()
{
	memset( m_sMessage, 0, sizeof(char)*1000*256 );
	m_nMaxLine = 0;
	m_nCurLine = 0;

	m_bRepeat = FALSE;
	m_nRepeatNum = 0;

	memset( m_sEventTitle, 0, sizeof(char)*100*32 );
	memset( m_sEventMsg, 0, sizeof(char)*100*128 );
}

CNotifyMessage::~CNotifyMessage()
{
}

BOOL CNotifyMessage::Open( char* filename )
{
	if( filename == "" && filename == "None" )
		return FALSE;

	CMHFile file;
	char temp[256];
	int index;

	if( !file.Init( filename, "rt" ) )
		return FALSE;

	Release();

	while( !file.IsEOF() )
	{
		strcpy( temp, strupr(file.GetString()) );

		if( temp[0] == '/' ) 
		{
			file.GetLine( temp, 256 );
			continue;
		}
		
		if( strcmp( temp, "*MAX" ) == 0 )
		{
			file.GetString( m_sMessage[0] );
			m_nMaxLine = atoi( m_sMessage[0] );			
			while( !file.IsEOF() )
			{
				index = file.GetInt();
				file.GetStringInQuotation( m_sMessage[index] );	
				if( m_nMaxLine == index )
					break;
			}
		}
	}

	file.Release();

	return TRUE;
}

char* CNotifyMessage::GetMessage()
{
	++m_nCurLine;

	if( m_nMaxLine < m_nCurLine )
	{
		--m_nRepeatNum;
		if( m_nRepeatNum > 0 )
		{
			m_nCurLine = 1;
		}
		else
		{
			m_nCurLine = 0;
			m_bRepeat = FALSE;
			return NULL;
		}				
	}

	return m_sMessage[m_nCurLine];
}

void CNotifyMessage::GetEventMsg( LPTSTR pTitle, LPTSTR pMsg )
{
	++m_nCurLine;

	if( m_nMaxLine < m_nCurLine )
	{
		--m_nRepeatNum;
		if( m_nRepeatNum > 0 )
		{
			m_nCurLine = 1;
		}
		else
		{
			m_nCurLine = 0;
			m_bRepeat = FALSE;
			return;
		}				
	}

	_tcsncpy( pTitle, CA2WEX< MAX_PATH >( m_sEventTitle[m_nCurLine] ), 32 );
	_tcsncpy( pMsg, CA2WEX< MAX_PATH >( m_sEventMsg[m_nCurLine] ), 128 );
}

void CNotifyMessage::SetRepeatNum( int repeatnum )
{
	m_nRepeatNum = repeatnum;

	if( m_nRepeatNum > 0 )
		m_bRepeat = TRUE;
}

void CNotifyMessage::Release()
{
	memset( m_sMessage, 0, sizeof(char)*1000*256 );
	m_nMaxLine = 0;
	m_nCurLine = 0;

	m_bRepeat = FALSE;
	m_nRepeatNum = 0;
}

BOOL CNotifyMessage::OpenEventMsgFile( char* filename )
{
	if( filename == "" && filename == "None" )	return FALSE;

	CMHFile file;
	char temp[256];
	int index;

	if( !file.Init( filename, "rt" ) )	return FALSE;

	while( !file.IsEOF() )
	{
		strcpy( temp, strupr(file.GetString()) );

		if( temp[0] == '/' ) 
		{
			file.GetLine( temp, 256 );
			continue;
		}
		
		if( strcmp( temp, "*MAX" ) == 0 )
		{
			file.GetString( m_sEventTitle[0] );
			m_nMaxLine = atoi( m_sEventTitle[0] );			
			while( !file.IsEOF() )
			{
				index = file.GetInt();
				file.GetStringInQuotation( temp );	
				strncpy( m_sEventTitle[index], temp, 32 );
				file.GetStringInQuotation( temp );	
				strncpy( m_sEventMsg[index], temp, 128 );
				if( m_nMaxLine == index || m_nMaxLine > 100 )
					break;
			}
		}
	}

	file.Release();

	return TRUE;
}
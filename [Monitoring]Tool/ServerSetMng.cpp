// ServerSetMng.cpp: implementation of the CServerSetMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerTool.h"
#include "ServerSetMng.h"

#include "MHNetwork.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CServerSetMng)

CServerSetMng::CServerSetMng()
{
	m_nMaxServerSet = 1;
	m_dwTotalUserCount = 0;
}

CServerSetMng::~CServerSetMng()
{
	Release();
}

BOOL CServerSetMng::Init( char* filename )
{
	return ReadMASList(filename);
}

BOOL CServerSetMng::ReadMASList( char* filename )
{
	Release();

	char temp[ MAX_PATH ] = { 0 };
	strcpy( temp, CW2AEX< MAX_PATH >( GetDataDirectory() ) );
	strcat( temp, filename );

	CMHFile file;
	
	if( !file.Init( temp, "rt" ) )
		return FALSE;

	char setName[256];
	char setFilename[256];

	m_nMaxServerSet = file.GetInt();
	for(int n=0;n<m_nMaxServerSet;++n)
	{
		file.GetString( setFilename );
		file.GetString( setName );

		m_ServerSet[n].Init(n,setName,setFilename);		
	}

	file.Release();

	return TRUE;
}


void CServerSetMng::Release()
{
	m_nMaxServerSet = 1;
	m_dwTotalUserCount = 0;

	for( int i = 0; i < MAX_SERVERSET; ++i )
		m_ServerSet[i].Release();
}

CServerSet* CServerSetMng::GetServerSet( int setindex )
{
	if( setindex < m_nMaxServerSet )
		return &m_ServerSet[setindex];

	return NULL;
}

CServerSet* CServerSetMng::GetServerSet( LPCTSTR setname )
{
	for( int i = 0; i < m_nMaxServerSet; ++i )
	{
		if( m_ServerSet[i].m_bInit )
		if( _tcscmp( m_ServerSet[i].m_sServerSetName, setname ) == 0 )
			return &m_ServerSet[i];
	}

	return NULL;
}

DWORD CServerSetMng::GetTotalUserCount()
{
	m_dwTotalUserCount = 0;
	for( int i = 0; i < m_nMaxServerSet; ++i )
	{
		if( m_ServerSet[i].m_bInit )
			m_dwTotalUserCount += m_ServerSet[i].GetTotalUserCount();
	}

	return m_dwTotalUserCount;
}

void CServerSetMng::WriteClientExeVersion( LPCTSTR setname, LPCTSTR version )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].WriteClientExeVersion( CW2AEX< MAX_PATH >( version ) );
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->WriteClientExeVersion( CW2AEX< MAX_PATH >( version ) );
	}
}

void CServerSetMng::WriteUserCount( SYSTEMTIME time )
{
	m_dwTotalUserCount = 0;
	for( int i = 0; i < m_nMaxServerSet; ++i )
	{
		if( m_ServerSet[i].m_bInit )
		{
			m_ServerSet[i].WriteUserCount( time );
			m_dwTotalUserCount += m_ServerSet[i].m_dwTotalUserCount;
		}
	}
}

void CServerSetMng::DoServerSetOn( LPCTSTR setname )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoServerSetOn();
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoServerSetOn();
	}
}

void CServerSetMng::DoServerSetOff( LPCTSTR setname )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoServerSetOff();
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoServerSetOff();
	}
}

void CServerSetMng::DoServerSetRefresh( LPCTSTR setname )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoServerSetRefresh();
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoServerSetRefresh();
	}
}

void CServerSetMng::DoQueryClientExeVersion()
{
	for( int i = 0; i < m_nMaxServerSet; ++i )
		m_ServerSet[i].DoQueryClientExeVersion();
}

void CServerSetMng::DoChangeClientExeVersion( LPCTSTR setname, LPCTSTR version )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoChangeClientExeVersion( version );
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoChangeClientExeVersion( version );
	}
}

void CServerSetMng::DoQueryMaxUser()
{
	for( int i = 0; i < m_nMaxServerSet; ++i )
		m_ServerSet[i].DoQueryMaxUser();
}

void CServerSetMng::DoChangeMaxUser( LPCTSTR setname, DWORD maxuser )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoChangeMaxUser( maxuser );
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoChangeMaxUser( maxuser );
	}
}

void CServerSetMng::DoQueryUserLevel()
{
	for( int i = 0; i < m_nMaxServerSet; ++i )
		m_ServerSet[i].DoQueryUserLevel();
}

void CServerSetMng::DoChangeUserLevel( LPCTSTR setname, int userlevel )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoChangeUserLevel( userlevel );
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoChangeUserLevel( userlevel );
	}
}

void CServerSetMng::DoQueryUserCount()
{
	for( int i = 0; i < m_nMaxServerSet; ++i )
		m_ServerSet[i].DoQueryUserCount();	
}

void CServerSetMng::DoSendMessage( LPCTSTR setname, const char* pmsg )
{
	int len = strlen( pmsg );
	if( pmsg == NULL || len > 255 )
		return;

	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoSendMessage( ( char* )pmsg );
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoSendMessage( ( char* )pmsg );
	}
}

void CServerSetMng::DoSendEventMessage( LPCTSTR setname, LPCTSTR pTitle, LPCTSTR pMsg )
{
	if( pTitle == NULL || pMsg == NULL )	return;
	int len1 = _tcslen( pTitle );
	int len2 = _tcslen( pMsg );
	if( len1 == 0 && len2 == 0 )	return;	

	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoSendEventMessage( pTitle, pMsg );
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoSendEventMessage( pTitle, pMsg );
	}
}

void CServerSetMng::DoSendEventMessageOff( LPCTSTR setname )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoSendEventMessageOff();
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoSendEventMessageOff();
	}
}

void CServerSetMng::DoAutoPatch( LPCTSTR setname )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoAutoPatch();
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoAutoPatch();
	}
}

void CServerSetMng::DoAssertMsgBox( LPCTSTR setname, BOOL bState )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoAssertMsgBox( bState );
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoAssertMsgBox( bState );
	}
}

void CServerSetMng::DoAllUserDisconnect( LPCTSTR setname )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoAllUserDisconnect();
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoAllUserDisconnect();
	}
}

void CServerSetMng::DoServerSetOffAfterUserDiscon( LPCTSTR setname )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoServerSetOffAfterUserDiscon();
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoServerSetOffAfterUserDiscon();
	}
}


void CServerSetMng::DoMSStateAll( LPCTSTR setname )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoMSStateAll();
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoMSStateAll();
	}
}

void CServerSetMng::DoQueryAutoPatchVersion( LPCTSTR setname )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoQueryAutoPatchVersion();
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoQueryAutoPatchVersion();
	}
}

void CServerSetMng::DoQueryChannelInfo( LPCTSTR setname )
{
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
			m_ServerSet[i].DoQueryChannelInfo();
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			pServerSet->DoQueryChannelInfo();
	}
}

BOOL CServerSetMng::IsThereOnServer( LPCTSTR setname )
{
	BOOL ret = FALSE;
	if( _tcscmp( L"All Server", setname ) == 0 )
	{
		for( int i = 0; i < m_nMaxServerSet; ++i )
		{
			ret = m_ServerSet[i].IsThereOnServer();
			if( ret )	
				return ret;
		}
	}
	else
	{
		CServerSet* pServerSet = GetServerSet( setname );
		if( pServerSet )
			ret = pServerSet->IsThereOnServer();
	}

	return ret;
}

void CServerSetMng::CheckAndSendSMS()
{
	BOOL ret = FALSE;
	for( int i = 0; i < m_nMaxServerSet; ++i )
	{
		ret = m_ServerSet[i].IsThereAbnormalServer();
		if( ret )	break;
		ret = m_ServerSet[i].IsThereAbnormalMS();
		if( ret )	break;
	}
	if( ret )
		CopyFile( L"./Data/SMS_Msg.txt", L"./SMS/SMSList.ini", FALSE );
}
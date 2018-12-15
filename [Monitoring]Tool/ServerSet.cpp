// ServerSet.cpp: implementation of the CServerSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerTool.h"
#include "ServerSet.h"
#include "..\[CC]ServerModule\ServerListManager.h"
#include "mhnetwork.h"
// 080822 LUJ, 반복 메시지를 전송하기 위해 참조
#include "EventNofifyDlg.h"
#include "ServerToolView.h"

extern CServerToolView*	g_ServerToolView;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CServerSet::CServerSet()
{
	memset( m_sMASIP, 0, 16 );
	m_nMASPort = 0;

	memset( m_sServerInfoFileName, 0, 256 );
	memset( m_sClientExeVersion, 0, 256 );

	m_nServerSet = 0;
	memset( m_sServerSetName, 0, 256 );

	m_dwTotalUserCount = 0;
	m_nUserLevel = eUSERLEVEL_SUPERUSER;
	_tcscmp( m_sUserLevel, L"SuperUser" );
	
	m_bInit = FALSE;
	
	m_dwMsgCheckTime = 0;

	m_dwMaxUser = 5000;
}

CServerSet::~CServerSet()
{	
	Release();
}

BOOL CServerSet::Init(int ServerSetNum, char* setName, char* setFileName)
{
	m_nServerSet = ServerSetNum;
	_tcscpy(m_sServerSetName, CString( setName ) );
	return InitFromFile(setFileName);
}

BOOL CServerSet::InitFromFile(char* setFileName)
{
	// 091216 ONS 서버리스트 Text파일 로드처리 추가
	if( NULL == setFileName )
		return FALSE;

	const char* pExt = strchr(setFileName, '.');
	if( NULL == pExt )
		return FALSE;

	char temp[ 256 ] = { 0 };
	strcpy( temp, CW2AEX< MAX_PATH >( GetDataDirectory() ) );
	strcat( temp, setFileName );

	if(!memcmp(pExt,".txt",4))
	{
		SERVERLISTMGR->LoadFromText(temp);
	}
	else if(!memcmp(pExt,".msl",4))
	{
		SERVERLISTMGR->Load(temp);
	}
	else
	{
		LOG( L"Serverlist Open Failed : %s is not exist", setFileName );
		return FALSE;
	}

	SERVERLISTMGR->SetPositionHead();
	SERVERINFO* pInfo;
	
	CServer* pServer;
	while(pInfo = SERVERLISTMGR->GetNextServerInfo())
	{
		pServer = new CServer(m_nServerSet,pInfo);

		if( pInfo->wServerKind == MONITOR_AGENT_SERVER )
		{
			_tcsncpy(
				m_sMASIP,
				pInfo->szIPForUser,
				_countof(pInfo->szIPForUser));
			m_nMASPort = pInfo->wPortForUser;

			m_vecMSServer.push_back( pServer );
		}
		else if( pInfo->wServerKind == MONITOR_SERVER )
		{
			m_vecMSServer.push_back( pServer );
		}
		else
		{
			m_vecServer.push_back(pServer);
		}
	}

	m_bInit = TRUE;
	
	ReadClientExeVersion();
	CreateUserCountFile();	

	SERVERLISTMGR->Clear();

	return TRUE;
}

void CServerSet::Release()
{
	memset( m_sMASIP, 0, 16 );
	m_nMASPort = 0;

	memset( m_sServerInfoFileName, 0, 256 );
	memset( m_sClientExeVersion, 0, 256 );

	m_nServerSet = 0;
	memset( m_sServerSetName, 0, 256 );

	m_dwTotalUserCount = 0;
	m_nUserLevel = eUSERLEVEL_SUPERUSER;
	_tcscpy( m_sUserLevel, L"SuperUser" );
	
	m_bInit = FALSE;

	m_dwMsgCheckTime = 0;

	m_dwMaxUser = 5000;

	vector<CServer*>::iterator iter = m_vecServer.begin();
	for(;iter != m_vecServer.end();++iter)
		delete (*iter);
	m_vecServer.clear();

	iter = m_vecMSServer.begin();
	for(;iter != m_vecMSServer.end();++iter)
		delete (*iter);
	m_vecMSServer.clear();
}

CServer* CServerSet::GetServer( WORD serverkind, WORD servernum )
{	
	vector<CServer*>::iterator iter = m_vecServer.begin();
	for(;iter != m_vecServer.end();++iter)
	{
		if( (*iter)->m_nServerKind == serverkind &&
			(*iter)->m_nServerNum == servernum )
			return (*iter);
	}

	return NULL;
}

CServer* CServerSet::GetServer( SERVERSTATE* pState )
{
	CServer* pServer = GetServer(pState->ServerType,pState->ServerNum);
	if(pServer == NULL)
		return NULL;

//	ASSERT(strcmp( pState->ServerIP, pServer->m_sIP ) == 0);
//	ASSERT(pState->ServerPort == pServer->m_nPort);

	return pServer;
}

void CServerSet::SetUserLevel( int userlevel )
{
	m_nUserLevel = userlevel;

	switch( userlevel )
	{
	case eUSERLEVEL_GOD:		_tcscpy( m_sUserLevel, L"God");			break;
	case eUSERLEVEL_PROGRAMMER:	_tcscpy( m_sUserLevel, L"Programmer");	break;
	case eUSERLEVEL_DEVELOPER:	_tcscpy( m_sUserLevel, L"Developer");	break;
	case eUSERLEVEL_GM:			_tcscpy( m_sUserLevel, L"GM");			break;
	case eUSERLEVEL_SUPERUSER:	_tcscpy( m_sUserLevel, L"SuperUser");	break;
	case eUSERLEVEL_USER:		_tcscpy( m_sUserLevel, L"User");		break;
	case eUSERLEVEL_SUBUSER:	_tcscpy( m_sUserLevel, L"SubUser");		break;
	}	
}

void CServerSet::ReadClientExeVersion()
{
	char temp[256] = { 0 };
	strcpy( temp, CW2AEX< MAX_PATH >( GetDataDirectory() ) );
	strcat( temp, CW2AEX< MAX_PATH >( m_sServerSetName ) );
	strcat( temp, "_ClientExeVersion.ver" );
	
	FILE* fp = fopen( temp, "r" );
	if( fp )
	{
		fscanf( fp, "%s", temp );
		fclose( fp );
		_tcscpy( m_sClientExeVersion, CString( temp ) );
		return;
	}

	_tcscpy( m_sClientExeVersion, L"None!!" );
}

void CServerSet::WriteClientExeVersion( char* version )
{
	if( m_bInit )
	{
		_tcscpy( m_sClientExeVersion, CString( version ) );

		char temp[ 256 ] = { 0 };
		strcpy( temp, CW2AEX< MAX_PATH >( GetDataDirectory() ) );
		strcat( temp, CW2AEX< MAX_PATH >( m_sServerSetName ) );
		strcat( temp, "_ClientExeVersion.ver" );

		FILE* fp = fopen( temp, "w" );
		if( fp )
		{
			fwrite( version, strlen(version), 1, fp );
			fclose( fp );
		}
	}
}

void CServerSet::CreateUserCountFile()
{
	SYSTEMTIME time;
	GetLocalTime( &time );
	char temp[ 256 ] = { 0 };
	char filename[ 256 ] = { 0 };
	strcpy( temp, CW2AEX< MAX_PATH >( GetUserCountDirectory() ) );
	sprintf( filename, "%s_%04d%02d%02d_UserCount.txt", m_sServerSetName, time.wYear, time.wMonth, time.wDay );
	strcat( temp, filename );

	FILE* fp = fopen( temp, "a+" );
	if( fp == NULL )
		return;

	fprintf( fp, "Time\tTotal\tDist\tAgent\tMuim\t" );

	// map
	
	vector<CServer*>::iterator iter = m_vecServer.begin();
	for(;iter != m_vecServer.end();++iter)
	{
		if((*iter)->m_nServerKind != MAP_SERVER)
			continue;

		fprintf( fp, "%s\t", (*iter)->m_sName );
	}

	fprintf( fp, "\n" );

	fclose( fp );
}

void CServerSet::WriteUserCount( SYSTEMTIME time )
{
	vector<CServer*>::iterator iter = m_vecServer.begin();
	if( m_bInit )
	if( IsThereOnServer() )
	{
		char temp[ 256 ] = { 0 };
		char filename[ 256 ] = { 0 };	
		strcpy( temp, CW2AEX< MAX_PATH >( GetUserCountDirectory() ) );
		sprintf( filename, "%s_%04d%02d%02d_UserCount.txt", m_sServerSetName, time.wYear, time.wMonth, time.wDay );
		strcat( temp, filename );

		FILE* fp = fopen( temp, "a+" );
		if( fp == NULL )
			return;

		fprintf( fp, "%02d:%02d:%02d\t", time.wHour, time.wMinute, time.wSecond );

		DWORD total = 0;
		DWORD dist = 0;
		DWORD agent = 0;
		DWORD murim = 0;

		// distribute
		iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != DISTRIBUTE_SERVER)
				continue;
			
			dist += (*iter)->m_dwUserCount;
		}
		total += dist;

		// agent
		iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != AGENT_SERVER)
				continue;
			
			agent += (*iter)->m_dwUserCount;
		}
		total += agent;

		// murim
		iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != MURIM_SERVER)
				continue;
			
			agent += (*iter)->m_dwUserCount;
		}

		fprintf( fp, "%d\t%d\t%d\t%d\t", total, dist, agent, murim );
		// map
		iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != MAP_SERVER)
				continue;
			
			fprintf( fp, "%d\t", (*iter)->m_dwUserCount );
		}
		fprintf( fp, "\n" );

		fclose(fp);
	}
}

BOOL CServerSet::IsThereOnServer()
{	
	vector<CServer*>::iterator iter = m_vecServer.begin();
	for(;iter != m_vecServer.end();++iter)
	{
		if((*iter)->m_nState == eServerState_On)
			return TRUE;
	}
	return FALSE;
}

BOOL CServerSet::IsThereAbnormalServer()
{
	if( IsThereOnServer() )
	{
		vector<CServer*>::iterator iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nState == eServerState_AbNormal || (*iter)->m_nState == eServerState_Off )
				return TRUE;
		}
	}
	else
	{
		vector<CServer*>::iterator iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nState == eServerState_AbNormal)
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CServerSet::IsThereAbnormalMS()
{
	vector<CServer*>::iterator iter = m_vecMSServer.begin();
	for(;iter != m_vecMSServer.end();++iter)
	{
		if((*iter)->m_nBeforState == eServerState_AbNormal )
			return TRUE;
	}
	return FALSE;
}

DWORD CServerSet::GetTotalUserCount()
{
	DWORD total = 0;
	DWORD dist = 0;
	DWORD agent = 0;
	
	vector<CServer*>::iterator iter = m_vecServer.begin();
	// distribute
	for(;iter != m_vecServer.end();++iter)
	{
		if((*iter)->m_nServerKind != DISTRIBUTE_SERVER)
			continue;
		
		dist += (*iter)->m_dwUserCount;
	}
	total += dist;
	
	// agent
	iter = m_vecServer.begin();
	for(;iter != m_vecServer.end();++iter)
	{
		if((*iter)->m_nServerKind != AGENT_SERVER)
			continue;
		
		agent += (*iter)->m_dwUserCount;
	}
	total += agent;

	m_dwTotalUserCount = total;

	return m_dwTotalUserCount;
}

BOOL CServerSet::CheckDisconnectUser()
{
	vector<CServer*>::iterator iter = m_vecServer.begin();
	for(;iter != m_vecServer.end();++iter)
	{
		if((*iter)->m_bDisconnectUser)
			return FALSE;
	}

	return TRUE;
}

// operate fn----------------------------------------------------------------------------
void CServerSet::DoServerSetOn() 
{
	if( m_bInit )
	{
		// on 순서 : map, murim -> agent -> distribute
		vector<CServer*>::iterator iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != MAP_SERVER)
				continue;
			
			(*iter)->DoServerOn();
		}
		iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != MURIM_SERVER)
				continue;
			
			(*iter)->DoServerOn();
		}
		iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != AGENT_SERVER)
				continue;
			
			(*iter)->DoServerOn();
		}
		iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != DISTRIBUTE_SERVER)
				continue;
			
			(*iter)->DoServerOn();
		}
	}
}

void CServerSet::DoServerSetOff()
{
	if( m_bInit )
	{
		// on 순서 : map, murim -> agent -> distribute
		vector<CServer*>::iterator iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != MAP_SERVER)
				continue;
			
			(*iter)->DoServerOff();
		}
		iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != MURIM_SERVER)
				continue;
			
			(*iter)->DoServerOff();
		}
		iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != AGENT_SERVER)
				continue;
			
			(*iter)->DoServerOff();
		}
		iter = m_vecServer.begin();
		for(;iter != m_vecServer.end();++iter)
		{
			if((*iter)->m_nServerKind != DISTRIBUTE_SERVER)
				continue;
			
			(*iter)->DoServerOff();
		}
	}
}

void CServerSet::DoServerSetRefresh()
{
	if( m_bInit )
	{
		MSGROOT msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_MAPSERVERSTATE_SYN;

		NETWORK->Send( m_nServerSet, &msg, sizeof(msg) );
	}
}

void CServerSet::DoServerOn( WORD serverkind, WORD servernum )
{
	if( m_bInit )
	{
		CServer* pServer = GetServer(serverkind,servernum);
		if(pServer)
			pServer->DoServerOn();
	}
}

void CServerSet::DoServerOff( WORD serverkind, WORD servernum )
{
	if( m_bInit )
	{
		CServer* pServer = GetServer(serverkind,servernum);
		if(pServer)
			pServer->DoServerOff();
	}
}

void CServerSet::DoQueryClientExeVersion()
{
	if( m_bInit )
	{
		MSGROOT msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_QUERY_VERSION_SYN;
		
		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServerSet::DoChangeClientExeVersion( LPCTSTR version )
{
	if( m_bInit )
	{
		MSGNOTICE msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_CHANGE_VERSION_SYN;
		strcpy( msg.Msg, CW2AEX< MAX_PATH >( version ) );
		
		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServerSet::DoQueryMaxUser()
{
	if( m_bInit )
	{
		MSGROOT msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_QUERY_MAXUSER_SYN;
		
		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServerSet::DoChangeMaxUser( DWORD maxuser )
{
	if( m_bInit )
	{
		MSG_DWORD msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_CHANGE_MAXUSER_SYN;
		msg.dwData = maxuser;
		
		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServerSet::DoQueryUserLevel()
{
	if( m_bInit )
	{
		MSG_DWORD msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_CHANGE_USERLEVEL_SYN;
		msg.dwData = 0;
	
		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServerSet::DoChangeUserLevel( int userlevel )
{
	if( m_bInit )
	{
		MSG_DWORD msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_CHANGE_USERLEVEL_SYN;
		msg.dwData = userlevel;

		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServerSet::DoQueryUserCount()
{
	if( m_bInit )
	{
		MSGROOT msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_QUERYUSERCOUNT_SYN;

		NETWORK->Send( m_nServerSet, &msg, sizeof(msg) );
	}
}

void CServerSet::DoSendMessage( char* pmsg )
{
	if( m_bInit )
	{
		MSGNOTICE msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_NOTICESEND_SYN;
		strcpy( msg.Msg, pmsg );
		
		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServerSet::DoSendEventMessage( LPCTSTR pTitle, LPCTSTR pMsg )
{
	if( m_bInit )
	{
		MSG_EVENTNOTIFY_ON msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_EVENTNOTICE_ON;
		strncpy( msg.strTitle, CW2AEX< MAX_PATH >( pTitle ), 32 );
		strncpy( msg.strContext, CW2AEX< MAX_PATH >( pMsg ), 128 );
	
		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServerSet::DoSendEventMessageOff()
{
	if( m_bInit )
	{
		MSGBASE msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_EVENTNOTICE_OFF;
	
		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServerSet::DoAutoPatch()
{
	if( m_bInit )
	{
		MSGROOT msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_EXECUTE_AUTOPATCH_SYN;
		
		NETWORK->Send( m_nServerSet, &msg, sizeof(msg) );
	}
}

void CServerSet::DoAssertMsgBox( BOOL bState )
{
	if( m_bInit )
	{
		MSG_DWORD msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_ASSERTMSGBOX_SYN;
		msg.dwData = (DWORD)bState;
		
		NETWORK->Send( m_nServerSet, &msg, sizeof(msg) );
	}
}

void CServerSet::DoAllUserDisconnect()
{
	if( m_bInit )
	{
		MSGROOT msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_DISCONNECT_ALLUSER_SYN;

		NETWORK->Send( m_nServerSet, &msg, sizeof(msg) );
	}
}

void CServerSet::DoServerSetOffAfterUserDiscon()
{
	if( CheckDisconnectUser() )
		DoServerSetOff();
}

void CServerSet::DoMSStateAll()
{
//	if( m_bInit )
	{
		MSGROOT msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_MSSTATEALL_SYN;

		NETWORK->Send( m_nServerSet, &msg, sizeof(msg) );	
	}
}

void CServerSet::DoQueryChannelInfo()
{
	if( m_bInit )
	{
		MSGROOT msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_QUERY_CHANNELINFO_SYN;

		NETWORK->Send( m_nServerSet, &msg, sizeof(msg) );	
	}
}

CServer* CServerSet::GetMSServer( MSSTATE* pState )
{
	vector<CServer*>::iterator iter = m_vecMSServer.begin();
	for(;iter != m_vecMSServer.end();++iter)
	{
		if( ( (*iter)->m_nPort == pState->wServerPort ) &&
			( strcmp( CW2AEX< MAX_PATH >( (*iter)->m_sIP ), pState->sServerIP) == 0 ) )
			return (*iter);
	}

	return NULL;
}

CServer* CServerSet::GetServer( LPCTSTR pServerIp, WORD wServerPort )
{
	vector<CServer*>::iterator iter = m_vecServer.begin();
	for(;iter != m_vecServer.end();++iter)
	{
		if( ( (*iter)->m_nPort == wServerPort ) &&
			( _tcscmp((*iter)->m_sIP, pServerIp) == 0 ) )
			return (*iter);
	}

	return NULL;
}

void CServerSet::ClearMSStateAll( int nState )
{
	vector<CServer*>::iterator iter = m_vecMSServer.begin();
	for(;iter != m_vecMSServer.end();++iter)
	{
		(*iter)->SetState( nState );
	}
}

void CServerSet::CheckMSState()
{
	vector<CServer*>::iterator iter = m_vecMSServer.begin();
	for(;iter != m_vecMSServer.end();++iter)
	{
		(*iter)->CheckState();
	}
}

void CServerSet::DoQueryAutoPatchVersion()
{
	if( m_bInit )
	{
		MSGROOT msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_QUERY_AUTOPATCHVERSION_SYN;

		NETWORK->Send( m_nServerSet, &msg, sizeof(msg) );	
	}
}
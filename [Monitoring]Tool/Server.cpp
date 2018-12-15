// Server.cpp: implementation of the CServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerTool.h"
#include "Server.h"
#include "MapNameManager.h"

#include "MHNetwork.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern sMSGOPTION	gMsgOption;
extern BOOL			g_bSound;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServer::CServer(int ServerSetNum,SERVERINFO* pInfo)
{
	m_nServerSet = ServerSetNum;
	m_nServerKind = pInfo->wServerKind;
	m_nState = eServerState_Off;
	_tcscpy( m_sState, L"OFF" );
	m_nServerNum = pInfo->wServerNum;
	_tcsncpy(
		m_sIP,
		pInfo->szIPForServer,
		_countof(m_sIP));
	m_nPort = pInfo->wPortForServer;
	m_dwUserCount = 0;
	m_bDisconnectUser = FALSE;
	m_nAutoPatchState = AUTOPATCHSTATE_END;
	_tcscpy( m_sAutoPatchState, L"End" );
	_tcscpy( m_sAutoPatchVersion, L"Null" );

	memset( m_nChannelUserCount, 0, sizeof(int)*10 );
	m_nMaxChannel = 0;

	switch(m_nServerKind)
	{
	case DISTRIBUTE_SERVER:
		_tcscpy( m_sName, L"Distribute" );
		break;
	case AGENT_SERVER:
		_tcscpy( m_sName, L"Agent" );
		break;
	case MURIM_SERVER:
		_tcscpy( m_sName, L"Murim" );
		break;
	case MAP_SERVER:
//		_tcscpy( m_sName, GetMapName(m_nServerNum) );
		_tcscpy( m_sName, MAPNAME->GetMapName(m_nServerNum) );
		break;
	case MONITOR_AGENT_SERVER:
		{
			_tcscpy( m_sName, L"MAS" );
			_tcscpy( m_sState, L"Disconnect" );
		}
		break;
	case MONITOR_SERVER:
		{
			_tcscpy( m_sName, L"MS" );
			_tcscpy( m_sState, L"Disconnect" );
		}
		break;
	default:
		_tcscpy( m_sName, L"ETC" );
	}
	
	m_nBeforState = eServerState_Off;

	m_nServerOff = eServerState_Off;
}

CServer::~CServer()
{
	Release();
}

void CServer::Release()
{
	m_nServerSet = -1;
	m_nServerKind = 0;
	m_nState = eServerState_Off;
	_tcscpy( m_sState, L"OFF" );
	m_nServerNum = 0;
	memset( m_sIP, 0, 16 );	
	m_nPort = 0;
	memset( m_sName, 0, 64 );
	m_dwUserCount = 0;	
	m_bDisconnectUser = FALSE;
	m_nAutoPatchState = AUTOPATCHSTATE_END;
	_tcscpy( m_sAutoPatchState, L"End" );
	_tcscpy( m_sAutoPatchVersion, L"Null" );

	memset( m_nChannelUserCount, 0, sizeof(int)*10 );
	m_nMaxChannel = 0;

	m_nServerOff = eServerState_Off;
}

int CServer::SetState( int state )
{
	switch( state )
	{
	case eServerState_Off:
		{
			if( m_nState != eServerState_AbNormal )
			{
				m_nState = state;
				_tcscpy( m_sState, L"OFF" );
				m_dwUserCount = 0;
				m_bDisconnectUser = FALSE;
			}
			
			memset( m_nChannelUserCount, 0, sizeof(int)*10 );
			m_nMaxChannel = 0;

			if( m_nServerOff == eServerState_On )
				m_nServerOff = eServerState_AbNormal;
		}
		break;
		
	case eServerState_On:
		{
			m_nState = state;
			_tcscpy( m_sState, L"ON" );
			if( m_nServerKind == AGENT_SERVER && m_nState == eServerState_Off )
				m_bDisconnectUser = TRUE;

			m_nServerOff = eServerState_On;
		}
		break;
		
	case eServerState_AbNormal:
		{
			m_nState = state;
			_tcscpy( m_sState, L"AbNormal" );

			CString str;
			TCHAR temp[256];
			_tcscpy( temp, GetDataDirectory() );
			_tcscat( temp, L"ringin.wav" );
			sndPlaySound( temp, SND_LOOP | SND_NODEFAULT | SND_ASYNC );
			g_bSound = TRUE;
			str.Format( L"%s Server AbNormal!!", m_sName );
			if( gMsgOption.bServer )
			{				
				if( MessageBox( NULL, str, L"Error!!", MB_OK ) == IDOK )
				{
					sndPlaySound( NULL, SND_LOOP | SND_NODEFAULT | SND_ASYNC );
					g_bSound = FALSE;
					return m_nServerSet;
				}
			}
			else
				return m_nServerSet;

		}
		break;

	case eServerState_Disconnect:
		{
			_tcscpy( m_sState, L"Disconnect" );
			m_nState = eServerState_Off;
		}
		break;
	case eServerState_Connect:
		{
			_tcscpy( m_sState, L"Connect" );
			m_nState = eServerState_On;			
			m_nBeforState = m_nState;
		}
		break;
	}

	return -1;
}

void CServer::DoServerOn()
{
	if( m_nState == eServerState_Off )
	{
		MSGEXECUTESERVER msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_EXECUTE_MAPSERVER_SYN;
		msg.ServerType = m_nServerKind;
		msg.ServerNum = m_nServerNum;
		strcpy( msg.ServerIP, CW2AEX< MAX_PATH >( m_sIP ) );
		msg.ServerPort = m_nPort;		

		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServer::DoServerOff()
{
	if( m_nState == eServerState_On || m_nState == eServerState_AbNormal )
	{
		MSGEXECUTESERVER msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_SHUTDOWN_MAPSERVER_SYN;
		msg.ServerType = m_nServerKind;
		msg.ServerNum = m_nServerNum;
		strcpy( msg.ServerIP, CW2AEX< MAX_PATH >( m_sIP ) );
		msg.ServerPort = m_nPort;
		
		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServer::DoKillServer()
{
	if( m_nState == eServerState_On || m_nState == eServerState_AbNormal )
	{
		MSGEXECUTESERVER msg;
		msg.Category = MP_MORNITORTOOL;
		msg.Protocol = MP_MORNITORTOOL_KILL_SERVER_SYN;
		msg.ServerType = m_nServerKind;
		msg.ServerNum = m_nServerNum;
		strcpy( msg.ServerIP, CW2AEX< MAX_PATH >( m_sIP ) );
		msg.ServerPort = m_nPort;
		
		NETWORK->Send( m_nServerSet, (MSGROOT*)&msg, sizeof(msg) );
	}
}

void CServer::SetAutoPatchState( int nState, char* sVersion )
{
	if( m_nAutoPatchState == AUTOPATCHSTATE_END && nState == AUTOPATCHSTATE_PROC )
		return;

	m_nAutoPatchState = nState;
	switch( m_nAutoPatchState )
	{
	case AUTOPATCHSTATE_START:	_tcscpy( m_sAutoPatchState, L"Start" );	break;
	case AUTOPATCHSTATE_PROC:	_tcscpy( m_sAutoPatchState, L"Proc" );	break;
	case AUTOPATCHSTATE_END:	_tcscpy( m_sAutoPatchState, L"End" );		break;			
	}

	_tcscpy( m_sAutoPatchVersion, CA2WEX< sizeof( sVersion ) >( sVersion ) );
}

void CServer::SetAutoPatchVersion( char* sVersion )
{
	_tcscpy( m_sAutoPatchVersion, CString( sVersion ) );
}

void CServer::SetChannelUserCount( MSG_CHANNEL_INFO* pChannelInfo )
{
	memset( m_nChannelUserCount, 0, sizeof(int)*10 );
	m_nMaxChannel = 0;

	m_nMaxChannel = pChannelInfo->Count;
	for( int i = 0; i < m_nMaxChannel; ++i )
		m_nChannelUserCount[i] = pChannelInfo->PlayerNum[i];
}

void CServer::CheckState()
{
	if( m_nBeforState == eServerState_On && m_nState == eServerState_Off )
	{
		m_nBeforState = eServerState_AbNormal;

		TCHAR temp[256];
		_tcscpy( temp, GetDataDirectory() );
		_tcscat( temp, L"ringin.wav" );
		sndPlaySound( temp, SND_LOOP | SND_NODEFAULT | SND_ASYNC );
		g_bSound = TRUE;
		if( gMsgOption.bServer )
		if( MessageBox( NULL, L"MS State Change!!", L"Confirm!!", MB_OK ) == IDOK )
		{
			sndPlaySound( NULL, SND_LOOP | SND_NODEFAULT | SND_ASYNC );
			g_bSound = FALSE;
		}
	}
}


// MASManager.cpp: implementation of the CMASManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MASManager.h"
#include "MHFile.h"
#include "MonitorNetwork.h"
#include "MonitorUserTable.h"
#include "MonitorServerTable.h"
#include "ServerListManager.h"
#include "UserIPCheckMgr.h"

GLOBALTON(CMASManager)
CMASManager::CMASManager()
{
	m_ServerStateTable.Initialize(200);
	m_SMSReceiverTable.Initialize(10);	
}

CMASManager::~CMASManager()
{
	m_ServerStateTable.RemoveAll();
}
void CMASManager::Release()
{
	m_ServerStateTable.SetPositionHead();
	SERVERSTATE * info = NULL;
	while(info = (SERVERSTATE *)m_ServerStateTable.GetData())
	{		
		m_ServerStateTable.Remove(info->ServerPort);
		delete info;
	}

	RemoveReceiverInfo();
}

void CMASManager::SetStateInitForConn(DWORD dwConnectionIdx)
{
	MSERVERINFO * pMSInfo = MServerTable->GetServerInfoForConn(dwConnectionIdx);

	//yh
	ASSERT(pMSInfo);
	if(pMSInfo == NULL)
		return;

	m_ServerStateTable.SetPositionHead();
	SERVERSTATE * info = NULL;
	while(info = (SERVERSTATE *)m_ServerStateTable.GetData())
	{
		if(strcmp(pMSInfo->szIPForServer, info->ServerIP) == 0)
		{
			info->ServerState = SERVERSTATE_OFF;
			info->UserCount = 0;
		}
	}
}

void CMASManager::SendServerStateBootListToMS(DWORD dwConnectionIdx, WORD ServerTypeOrig, WORD ServerPortOrig)
{
	MSG_PWRUP_BOOTLIST msg;
	msg.Category = MP_MORNITORSERVER;
	msg.Protocol = MP_MORNITORSERVER_BOOTLIST_NOTIFY;
	msg.Num = 0;
	msg.MapServerPort = ServerPortOrig;

	switch(ServerTypeOrig)
	{
	case MAP_SERVER:
	case DISTRIBUTE_SERVER:
		{
			m_ServerStateTable.SetPositionHead();
			SERVERSTATE * info = NULL;
			while(info = (SERVERSTATE *)m_ServerStateTable.GetData())
			{
				if(info->ServerType == AGENT_SERVER)
				{
					if(info->ServerState == SERVERSTATE_ON)
					{					
						SERVERINFO ServerInfo;
						ServerInfo.dwConnectionIndex	= 0;
						ServerInfo.wServerKind			= info->ServerType;					
						strcpy(ServerInfo.szIPForServer, info->ServerIP);
						ServerInfo.wPortForServer		= info->ServerPort;						
						ServerInfo.wServerNum			= info->ServerNum;							
						ServerInfo.wAgentUserCnt		= 0;
						msg.AddList(&ServerInfo);
					}
				}
			}
		}
		break;
	case AGENT_SERVER:
		{
			m_ServerStateTable.SetPositionHead();
			SERVERSTATE * info = NULL;
			while(info = (SERVERSTATE *)m_ServerStateTable.GetData())
			{
				if(info->ServerPort != ServerPortOrig)
				{
					if(info->ServerState == SERVERSTATE_ON)
					{	
						SERVERINFO ServerInfo;
						ServerInfo.dwConnectionIndex	= 0;
						ServerInfo.wServerKind			= info->ServerType;					
						strcpy(ServerInfo.szIPForServer, info->ServerIP);
						//ServerInfo.szIPForUser		;	
						ServerInfo.wPortForServer		= info->ServerPort;						
						//ServerInfo.wPortForUser		;						
						ServerInfo.wServerNum			= info->ServerNum;							
						ServerInfo.wAgentUserCnt		= 0;
						msg.AddList(&ServerInfo);
					}
				}
					
			}
		}
		break;
	}
	if(msg.Num != 0)
	MNETWORK->SendToMS(dwConnectionIdx, (char *)&msg, msg.GetSize());
}

void CMASManager::SendServerStateToMCs(DWORD key)
{
	MUserTable->m_InfoTable.SetPositionHead();
	MCINFO * info = NULL;
	while(info = (MCINFO *)MUserTable->m_InfoTable.GetData())
	{
		SendServerStateToMC(info->dwConnectionIndex, key);
	}
}

void CMASManager::SendServerStateToMC(DWORD dwConnectionIdx, DWORD key/*port*/)
{
	SERVERSTATE * info = m_ServerStateTable.GetData(key);
	MSGSERVERSTATE msg;
	msg.Category = MP_MORNITORTOOL;
	msg.Protocol = MP_MORNITORTOOL_MAPSERVERSTATE_NOTIFY;
	msg.serverState = *info;
	MNETWORK->SendToMC(dwConnectionIdx, (char *)&msg, sizeof(msg));
}

void CMASManager::SendServerStateAllToMCAll()
{
	MSGMONITORTOOLALLSTATE msg;
	msg.Category = MP_MORNITORTOOL;
	msg.Protocol = MP_MORNITORTOOL_MAPSERVERSTATE_ACK;
	msg.num = 0;
	m_ServerStateTable.SetPositionHead();
	SERVERSTATE * info = NULL;
	while(info = (SERVERSTATE *)m_ServerStateTable.GetData())
	{		
		msg.AddServerState(info);
	}

	MUserTable->m_InfoTable.SetPositionHead();
	MCINFO * mcInfo = NULL;
	while(mcInfo = (MCINFO *)MUserTable->m_InfoTable.GetData())
	{
		MNETWORK->SendToMC(mcInfo->dwConnectionIndex, (char *)&msg, msg.GetSize());
	}
}

void CMASManager::SendMSServerStateAllToMCAll()
{
	MSERVERINFO* info = NULL;
	MSGMSSTATEALL Msg;
	MServerTable->SetPositionHeadInfo();			
	while( info = (MSERVERINFO*)MServerTable->GetDataInfo() )
	{
		MSSTATE state;
		strcpy( state.sServerIP, info->szIPForServer );
		state.wServerPort = info->wPortForServer;

		Msg.AddMSServerState( &state );
	}

	Msg.Category = MP_MORNITORTOOL;
	Msg.Protocol = MP_MORNITORTOOL_MSSTATEALL_ACK;

	MCINFO* mcInfo = NULL;
	MUserTable->m_InfoTable.SetPositionHead();
	while(mcInfo = (MCINFO *)MUserTable->m_InfoTable.GetData())
	{
		MNETWORK->SendToMC( mcInfo->dwConnectionIndex, (char*)&Msg, Msg.GetSize() );
	}
}

void CMASManager::SendServerStateAllToMC(DWORD dwConnectionIdx)
{
	MSGMONITORTOOLALLSTATE msg;
	msg.Category = MP_MORNITORTOOL;
	msg.Protocol = MP_MORNITORTOOL_MAPSERVERSTATE_ACK;
	msg.num = 0;
	m_ServerStateTable.SetPositionHead();
	SERVERSTATE * info = NULL;
	while(info = (SERVERSTATE *)m_ServerStateTable.GetData())
	{		
		msg.AddServerState(info);
	}

	IPCHECKMGR->Filter( dwConnectionIdx, msg );
	MNETWORK->SendToMC(dwConnectionIdx, (char *)&msg, msg.GetSize());
}
void CMASManager::AddServerState(SERVERSTATE * NewState, DWORD key)
{
	ASSERT(!m_ServerStateTable.GetData(key));
	m_ServerStateTable.Add(NewState, key);
}
SERVERSTATE * CMASManager::GetServerState(DWORD key)
{
	return m_ServerStateTable.GetData(key);
}

void CMASManager::SendToAllMC( char* pMsg, DWORD dwLength )
{
	MUserTable->m_InfoTable.SetPositionHead();
	MCINFO* mcInfo = NULL;
	while(mcInfo = (MCINFO*)MUserTable->m_InfoTable.GetData())
	{
		MNETWORK->SendToMC(mcInfo->dwConnectionIndex, pMsg, dwLength);
	}
}

#define MAX_DIST_SERVER		100
#define MAX_AGENT_SERVER	100
#define MAX_MAP_SERVER		100
#define MAX_MURIM_SERVER	100

/* defined  in "msdefine.h"
enum SERVER_KIND
{
	ERROR_SERVER,
	DISTRIBUTE_SERVER,
	AGENT_SERVER,
	MAP_SERVER,
	CHAT_SERVER,
	MURIM_SERVER,
	MONITOR_AGENT_SERVER,
	MONITOR_SERVER,
	MAX_SERVER_KIND,
};
*/
char serverkindstring[MAX_SERVER_KIND][20] = 
{
	"ERROR!!!"
	"Dist",
	"Agent",
	"Map",
	"Chat",
	"Murim",
	"MonitorAgent",
	"Monitor",
};



void CMASManager::LoadMapServerInfo()
{
	SERVERSTATE * info = NULL;
	SERVERINFO* pServerInfo;
	
	SERVERLISTMGR->SetPositionHead();
	while(pServerInfo = SERVERLISTMGR->GetNextServerInfo())
	{
		if( pServerInfo->wServerKind != DISTRIBUTE_SERVER &&
			pServerInfo->wServerKind != AGENT_SERVER &&
			pServerInfo->wServerKind != MAP_SERVER )
			continue;

		info = new SERVERSTATE;
		info->ServerType		= pServerInfo->wServerKind;
		info->ServerNum			= pServerInfo->wServerNum;
		strcpy(info->ServerIP, pServerInfo->szIPForServer);
		info->ServerPort		= pServerInfo->wPortForServer;
		info->ServerState		= SERVERSTATE_OFF;
		info->UserCount			= 0;
		AddServerState(info, info->ServerPort);
	}
}

void CMASManager::LoadSMSReceiverInfo()
{
	CMHFile file;
	if( stat( SMSRECEIVER_FILE, &m_SMSFileStat ) )
	{
		return;
	}

	if( !file.Init( SMSRECEIVER_FILE, "r", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
	{
		return;
	}
	
	char buf[ MAX_PATH ] = {0,};
	DWORD dwCnt = 0;
	stSMSReceiver* pReceiver = NULL;

	while( ! file.IsEOF() )
	{
		file.GetString( buf );
		if( strcmp( buf, "//" ) == 0 )
		{
			file.GetLineX( buf, MAX_PATH );
			continue;
		}
		else if( strcmp( buf, "$RECEIVER" ) == 0 )
		{
			pReceiver = new stSMSReceiver;
            memset( pReceiver, 0, sizeof(stSMSReceiver) );

			m_SMSReceiverTable.Add( pReceiver, ++dwCnt );
		}
		else if( pReceiver == NULL )
		{
			continue;
		}
		else if( strcmp( buf, "#NAME" ) == 0 )
		{
			file.GetStringInQuotation( pReceiver->Name );
		}
		else if( strcmp( buf, "#PHONE" ) == 0 )
		{
			file.GetString( buf );
			const char* token = strtok( buf, "-" );
			if( token )
				StringCopySafe( pReceiver->Phone1, token, sizeof(pReceiver->Phone1)/sizeof(*pReceiver->Phone1) );

			token = strtok( 0, "-" );
			if( token )
				StringCopySafe( pReceiver->Phone2, token, sizeof(pReceiver->Phone2)/sizeof(*pReceiver->Phone2) );

			token = strtok( 0, "-" );
			if( token )
				StringCopySafe( pReceiver->Phone3, token, sizeof(pReceiver->Phone3)/sizeof(*pReceiver->Phone3) );

		}
		else if( strcmp( buf, "#SENDER" ) == 0 )
		{
			file.GetStringInQuotation( pReceiver->SenderName );
		}
		else if( strcmp( buf, "#SENDERPHONE" ) == 0 )
		{
			file.GetString( buf );
			const char* token = strtok( buf, "-" );
			if( token )
				StringCopySafe( pReceiver->SenderPhone1, token, sizeof(pReceiver->SenderPhone1)/sizeof(*pReceiver->SenderPhone1) );

			token = strtok( 0, "-" );
			if( token )
				StringCopySafe( pReceiver->SenderPhone2, token, sizeof(pReceiver->SenderPhone2)/sizeof(*pReceiver->SenderPhone2) );

			token = strtok( 0, "-" );
			if( token )
				StringCopySafe( pReceiver->SenderPhone3, token, sizeof(pReceiver->SenderPhone3)/sizeof(*pReceiver->SenderPhone3) );
		}
		else if( strcmp( buf, "#MSG" ) == 0 )
		{
			file.GetStringInQuotation( pReceiver->Msg );
		}
		else if( buf[0] == '}' )
		{
			pReceiver = NULL;
		}
	}
}

void CMASManager::CheckSMSReceiverFile()
{
	// 파일 변경 확인
	struct stat fileStatus = { 0 };
	if( stat( SMSRECEIVER_FILE, &fileStatus ) )
	{
		// 090706 ShinJS --- 파일이 없는 경우 모든 정보 제거
		RemoveReceiverInfo();

		// 파일 상태 초기화
		memset( &m_SMSFileStat, 0, sizeof( fileStatus ) );

		return;
	}

	// 파일이 변경되지 않은 경우
	if( 0 == memcmp( &m_SMSFileStat, &fileStatus, sizeof( fileStatus ) ) )
	{
		return;
	}

	// 변경 상태 저장
	m_SMSFileStat = fileStatus;

	// 파일 변경시 모든 정보 제거 후 다시 로드
	RemoveReceiverInfo();
	LoadSMSReceiverInfo();

	return;
}

void CMASManager::RemoveReceiverInfo()
{
	m_SMSReceiverTable.SetPositionHead();
	stSMSReceiver* pSMSInfo = NULL;
	while( pSMSInfo = (stSMSReceiver*)m_SMSReceiverTable.GetData() )
	{
		delete pSMSInfo;
	}
	m_SMSReceiverTable.RemoveAll();
}
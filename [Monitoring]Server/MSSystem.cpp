// MSSystem.cpp: implementation of the CMSSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MSSystem.h"
#include "MonitorNetwork.h"
#include "MonitorServerTable.h"
#include "MSManager.h"
#include "MonitorUserTable.h"
#include "MASManager.h"
#include "DataBase.h"
#include "CMSDBMsgParser.h"
#include < winsock2.h >
#include "UserIPCheckMgr.h"
#include <queue>
#include <time.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define	AUTOPATCH	"DarkStoryOnline"	// ¿ÀÅäÆÐÄ¡CaptionÀÌ¸§

extern HWND gMainHwnd;

BOOL bAutoPatch = FALSE;

CMSSystem::CMSSystem()
{
	m_wServerType = TYPE_NONE;
	m_bInit = FALSE;
}

CMSSystem::~CMSSystem()
{
}
	
void CMSSystem::Start(WORD wServerType)
{
	m_wServerType = wServerType;
	DESC_NETWORK desc;

	char host[128],ipName[2][20];
	HOSTENT* hostent;
	
    WSADATA wsaData ;
	WSAStartup(0x0002,&wsaData);
	gethostname(host,128);
	hostent=gethostbyname(host);
	
	//inet_ntoa ÇÔ¼ö´Â msdn¿¡¼­ Ã£¾Æº¸±æ...
	for(int n=0;n<2;++n)
	{
		if(hostent->h_addr_list[n] == NULL)
			break;

		strncpy(ipName[n],inet_ntoa(*(struct in_addr *)hostent->h_addr_list[n]),20);
	}

	WSACleanup();
	
	char temp[256] = {0, };
	ySWITCH(m_wServerType)
	yCASE(TYPE_MS)
		{
			sprintf( temp, "MS_%s", SERVERTOOLVERSION );
			SetWindowText(GetActiveWindow(), temp );

			// ¼­¹ö Á¤º¸ ·Îµå
			MServerTable->LoadMonitorInfo(TYPE_MS,ipName[0],ipName[1]);

			CUSTOM_EVENT ev[1];
			ev[0].dwPeriodicTime = 100;
			ev[0].pEventFunc = CMSSystem::Process;
	
			desc.OnAcceptServer = OnAcceptServer;
			desc.OnDisconnectServer = OnDisconnectServer;
			desc.OnAcceptUser = NULL;
			desc.OnDisconnectUser = NULL;
			desc.OnRecvFromServerTCP = ReceivedMsgFromServer;
			desc.OnRecvFromUserTCP = NULL;
			desc.dwCustomDefineEventNum = 1;
			desc.pEvent = ev;

			desc.dwMainMsgQueMaxBufferSize = 512000;
			desc.dwMaxServerNum = 100;
			desc.dwMaxUserNum = 0;
			desc.dwServerBufferSizePerConnection = 25600;
			desc.dwServerMaxTransferSize = 6500;
			desc.dwUserBufferSizePerConnection = 0;
			desc.dwUserMaxTransferSize = 0;
			desc.dwConnectNumAtSameTime = 100;
			desc.dwFlag = NULL;

			MNETWORK->Init(&desc);
			MNETWORK->ConnectToMAS();

			// 080118 KTH -- 
			MNETWORK->ConnectToServers();
		}
	yCASE(TYPE_MAS)
		{
			sprintf( temp, "MAS_%s", SERVERTOOLVERSION );
			SetWindowText(GetActiveWindow(), temp );

			// ¼­¹ö Á¤º¸ ·Îµå
			MServerTable->LoadMonitorInfo(TYPE_MAS,ipName[0],ipName[1]);
			MASMGR->LoadMapServerInfo();
			// 090623 ShinJS --- SMS Receiver 정보 읽기
			MASMGR->LoadSMSReceiverInfo();
			CUSTOM_EVENT ev[1];
			ev[0].dwPeriodicTime = 100;
			ev[0].pEventFunc = CMSSystem::Process;

			desc.OnAcceptServer = OnAcceptServer;
			desc.OnDisconnectServer = OnDisconnectServer;
			desc.OnAcceptUser = OnAcceptUser;
			desc.OnDisconnectUser = OnDisconnectUser;
			desc.OnRecvFromServerTCP = ReceivedMsgFromServer;
			desc.OnRecvFromUserTCP = ReceivedMsgFromUser;
			desc.dwCustomDefineEventNum = 1;
			desc.pEvent = ev;

			desc.dwMainMsgQueMaxBufferSize = 5120000;
			desc.dwMaxServerNum = 300;
			desc.dwMaxUserNum = 50;
			desc.dwServerBufferSizePerConnection = 256000;
			desc.dwServerMaxTransferSize = 65000;
			desc.dwUserBufferSizePerConnection = 256000;
			desc.dwUserMaxTransferSize = 65000;
			desc.dwConnectNumAtSameTime = 100;
			desc.dwFlag = NULL;

			MNETWORK->Init(&desc);
			MNETWORK->StartServer(m_wServerType);
			
			if(g_DB.Init(1, 32, TRUE) == FALSE)
				MessageBox(NULL,"Fail To DataBase",0,0);

			m_bInit = TRUE;

			// IP Á¤º¸ ÀúÀå
//			TotalIpInfo( 0 );
		}
	yENDSWITCH
}

void CMSSystem::End()
{
	m_bInit = FALSE;

	g_DB.Release();
	MNETWORK->Release();
	CoFreeUnusedLibraries();
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::Process( DWORD eventIndex )
{
	ySWITCH(g_pMSSystem->m_wServerType)
	yCASE(TYPE_MS)
	{
		static KillServerMap mKillServerList;
		__time64_t time64t;
		_time64( &time64t );

		MServerTable->SetPositionHeadInfo();
		MSERVERINFO * pInfo = NULL;
		while(pInfo = MServerTable->GetDataInfo())
		{
			if(pInfo->wServerType != MONITOR_AGENT_SERVER && pInfo->wServerState == SERVERSTATE_ON)
			{
				if( time64t - pInfo->ExpiredTime > 10)
				{
					// ping send
					MSGROOT msg;
					msg.Category = MP_MORNITORMAPSERVER;
					msg.Protocol = MP_MORNITORMAPSERVER_PING_SYN;
					MNETWORK->SendToMapServer(pInfo->dwConnectionIndex, (char *)&msg, sizeof(MSGROOT));
				}
				
				if( time64t - pInfo->ExpiredTime > 60)
				{
					MSMGR->SendServerState(SERVERSTATE_ABNORMAL, pInfo);
				}
			}
			else if( pInfo->wServerState == SERVERSTATE_ABNORMAL && time64t - pInfo->ExpiredTime > 120)
			{
				if( pInfo->wServerType == MAP_SERVER	&& 
					pInfo->wServerNum  == 0				)
				{
					// 프로세스를 종료한다.
					MSMGR->KillServer(pInfo->wPortForServer);
					continue;
				}

				KillServerInfo	KillServer;
				ZeroMemory(&KillServer, sizeof(KillServer));

				KillServer.KillTime		= time64t;
				KillServer.wServerNum	= pInfo->wServerNum;
				KillServer.wServerType	= pInfo->wServerType;
				
				
				mKillServerList.insert( std::make_pair( 
												std::make_pair( pInfo->wServerNum, pInfo->wServerType), 
												KillServer ) );

				// 프로세스를 종료한다.
				MSMGR->KillServer(pInfo->wPortForServer);
			}
		}

		for( KillServerMap::iterator iter = mKillServerList.begin(); 
			iter != mKillServerList.end(); 
			++iter)
		{
			const KillServerInfo ServerInfo = iter->second;
			if( time64t - ServerInfo.KillTime > 60 )
			{
				// 서버를 실행한다.
				MSMGR->ExecuteServer(ServerInfo.wServerType, ServerInfo.wServerNum);
				mKillServerList.erase( iter );


				// 서버 자동재기동관련 로그를 남긴다.
				char buff[MAX_PATH] = {0};
				char path[MAX_PATH] = {0};
				struct tm excuteTime = *_localtime64( &time64t );

				GetCurrentDirectory(MAX_PATH, path);
				sprintf( buff, "%s/Log/ServerToolLog_%04d_%02d.txt", path, excuteTime.tm_year+1900, excuteTime.tm_mon+1 );
				FILE* fpLog = NULL;
				fpLog = fopen( buff, "a+" );
				if( fpLog )
				{
					struct tm killTime = *_localtime64( &ServerInfo.KillTime );

					fprintf( fpLog, "ServerType : %d, ServerNum : %d, KillTime : %04d-%02d-%02d %02d:%02d:%02d, ExcuteTime : %04d-%02d-%02d %02d:%02d:%02d \n", 
						ServerInfo.wServerType,
						ServerInfo.wServerNum,
						killTime.tm_year+1900, killTime.tm_mon+1, killTime.tm_mday, killTime.tm_hour, killTime.tm_min, killTime.tm_sec,
						excuteTime.tm_year+1900, excuteTime.tm_mon+1, excuteTime.tm_mday, excuteTime.tm_hour, excuteTime.tm_min, excuteTime.tm_sec );

					fclose( fpLog );
				}

				break;
			}			
		}


		if( bAutoPatch )
			AutoPatchProcess();		
	}
	yCASE(TYPE_MAS)
	{
		if( g_pMSSystem->m_bInit )
			g_DB.ProcessingDBMessage();
	}
	yCASE(TYPE_RMS)
	{
		if( g_pMSSystem->m_bInit )
			g_DB.ProcessingDBMessage();
	}
	yENDSWITCH
}

void CMSSystem::AutoPatchProcess()
{
	static BOOL bStart = FALSE;
	static __time64_t oldtick = 0;
	__time64_t tick;
	_time64( &tick );

	if( (tick - oldtick) > 1 )
	{
		oldtick = tick;

		HWND hWnd = FindWindow( NULL, AUTOPATCH );
		if( bStart == FALSE )	// Ã³À½
		{
			if( hWnd )
			{
				// SendStartAutoPatch;
				MSGMSSTATE Msg;
				Msg.Category = MP_MORNITORSERVER;
				Msg.Protocol = MP_MORNITORSERVER_CHECK_AUTOPATCH_ACK;
				MSERVERINFO* pInfo = MServerTable->GetMSInfo();
				strcpy( Msg.MSState.sServerIP, pInfo->szIPForServer );
				Msg.MSState.wServerPort = pInfo->wPortForServer;
				Msg.MSState.wAutoPatchState = AUTOPATCHSTATE_START;
				strcpy( Msg.MSState.sAutoPatchVersion, MSMGR->GetAutoPatchVersion() );
				MNETWORK->SendToMAS( (char*)&Msg, sizeof(Msg) );

				bStart = TRUE;
			}
		}
		else if( bStart == TRUE )
		{
			if( hWnd )
			{
				// SendProcessAutoPatch;			// ÁøÇà
				MSGMSSTATE Msg;
				Msg.Category = MP_MORNITORSERVER;
				Msg.Protocol = MP_MORNITORSERVER_CHECK_AUTOPATCH_ACK;
				MSERVERINFO* pInfo = MServerTable->GetMSInfo();
				strcpy( Msg.MSState.sServerIP, pInfo->szIPForServer );
				Msg.MSState.wServerPort = pInfo->wPortForServer;
				Msg.MSState.wAutoPatchState = AUTOPATCHSTATE_PROC;
				strcpy( Msg.MSState.sAutoPatchVersion, MSMGR->GetAutoPatchVersion() );
				MNETWORK->SendToMAS( (char*)&Msg, sizeof(Msg) );
			}
			else
			{
				// SendEndAutoPatch;				// ³¡
				MSGMSSTATE Msg;
				Msg.Category = MP_MORNITORSERVER;
				Msg.Protocol = MP_MORNITORSERVER_CHECK_AUTOPATCH_ACK;
				MSERVERINFO* pInfo = MServerTable->GetMSInfo();
				strcpy( Msg.MSState.sServerIP, pInfo->szIPForServer );
				Msg.MSState.wServerPort = pInfo->wPortForServer;
				Msg.MSState.wAutoPatchState = AUTOPATCHSTATE_END;
				MSMGR->ReadAutoPatchVersion();
				strcpy( Msg.MSState.sAutoPatchVersion, MSMGR->GetAutoPatchVersion() );
				MNETWORK->SendToMAS( (char*)&Msg, sizeof(Msg) );

				bStart = FALSE;
				bAutoPatch = FALSE;
			}
		}
	}
}

void CMSSystem::OnConnectMASSuccess(DWORD dwConnectionIndex, void* pVoid)
{
	MSERVERINFO * pInfo = (MSERVERINFO * )pVoid;
	pInfo->dwConnectionIndex = dwConnectionIndex;
	ySWITCH(g_pMSSystem->m_wServerType)
	yCASE(TYPE_MS)	
		LogConsole("MAPÁ¢¼Ó¼º°ø");
		MSMGR->SendMSInfo();
		MNETWORK->StartServer(TYPE_MS);
	yCASE(TYPE_MAS)
			ASSERT(0);
	yENDSWITCH
}

void CMSSystem::OnConnectMASFail(void* pVoid)
{
	ASSERTMSG(0, "MAS¸¦ ¸ÕÀú ½ÇÇàÇÏ¼¼¿ä!");

	ySWITCH(g_pMSSystem->m_wServerType)
	yCASE(TYPE_MS)
	yCASE(TYPE_MAS)
			ASSERT(0);
	yENDSWITCH
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::OnAcceptServer(DWORD dwConnectionIndex)
{
	ySWITCH(g_pMSSystem->m_wServerType)
	yCASE(TYPE_MS)
		// map server Á¢¼ÓÇØ µé¾î¿È
		LogConsole("MAPÁ¢¼Ó");
	yCASE(TYPE_MAS)
		// ms server Á¢¼ÓÇØ µé¾î¿È
		LogConsole("MSÁ¢¼Ó");
	yENDSWITCH

}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::OnDisconnectServer(DWORD dwConnectionIndex)
{
	ySWITCH(g_pMSSystem->m_wServerType)
	yCASE(TYPE_MS)
		MSERVERINFO* pInfo = MServerTable->GetServerInfoForConn(dwConnectionIndex);
		if(pInfo == NULL)	// MONITOR_AGENT_SERVER
		{
			LogConsole("MAS²÷¾îÁü");
			::SendMessage(gMainHwnd,WM_CLOSE,0,0);
			return;
		}
			
		switch(pInfo->wServerType)
		{
		case MAP_SERVER:
		case AGENT_SERVER:
		case DISTRIBUTE_SERVER:
			{
				LogConsole("MAP²÷¾îÁü");
				// map server ²÷¾îÁü
				// 1. ¼­¹ö Å×ÀÌºí¿¡¼­ »èÁ¦
				// 2. mas¿¡°Ô »óÅÂ ¾Ë¸²
				MSMGR->SendServerState(SERVERSTATE_OFF, MServerTable->GetServerInfoForConn(dwConnectionIndex));
				MSMGR->ServerUnRegiest(dwConnectionIndex);
			}
			return;
		}
		LogConsole("ºñÁ¤»ó ²÷¾îÁü");
	yCASE(TYPE_MAS)
		// ms server ²÷¾îÁü
		// »óÅÂ ÃÊ±âÈ­
		LogConsole("MS²÷¾îÁü");
		MASMGR->SetStateInitForConn(dwConnectionIndex);
		MServerTable->RemoveServerForConn( dwConnectionIndex );

		MASMGR->SendServerStateAllToMCAll();

		MASMGR->SendMSServerStateAllToMCAll();

	yENDSWITCH
	
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::ReceivedMsgFrom2Server(DWORD dwConnectionIndex,char* pmsg,DWORD dwLength)
{
	MSGROOT* pMsg = (MSGROOT*)pmsg;
	ySWITCH(pMsg->Category)
	yCASE(MP_MORNITORMAPSERVER)		// from MapServer
	{
		ySWITCH(pMsg->Protocol)
		yCASE(MP_MORNITORMAPSERVER_QUERYUSERCOUNT_ACK)
		{
			MSGUSERCOUNT * pMsg = (MSGUSERCOUNT *)pmsg;
			MSERVERINFO * pInfo = MServerTable->GetServerInfoForConn(dwConnectionIndex);
			pMsg->Category = MP_MORNITORSERVER;
			pMsg->Protocol = MP_MORNITORSERVER_QUERYUSERCOUNT_ACK;
			pMsg->ServerPort = pInfo->wPortForServer;
			MNETWORK->SendToMAS(pmsg, sizeof(MSGUSERCOUNT));
		}
		yCASE(MP_MORNITORMAPSERVER_QUERY_CHANNELINFO_ACK)
		{			
			MSG_CHANNEL_INFO_MORNITOR Msg;
			Msg.ChannelInfo = *((MSG_CHANNEL_INFO*)pmsg);
			MSERVERINFO* pInfo = MServerTable->GetServerInfoForConn( dwConnectionIndex );
			strcpy( Msg.sServerIP, pInfo->szIPForServer );
			Msg.wServerPort = pInfo->wPortForServer;
			Msg.Category = MP_MORNITORSERVER;
			Msg.Protocol = MP_MORNITORSERVER_QUERY_CHANNELINFO_ACK;
			MNETWORK->SendToMAS( (char*)&Msg, sizeof(MSG_CHANNEL_INFO_MORNITOR) );
		}
		yCASE(MP_MORNITORMAPSERVER_BOOTUP_NOTIFY)
		{
			//send to mas
			MSG_PWRUP_BOOTUP * pmsg2 = (MSG_PWRUP_BOOTUP *)pmsg;
			MSMGR->AddServerRegiest(dwConnectionIndex, pmsg2->dwProcessID, &pmsg2->BootInfo);
			MSMGR->SendServerState(SERVERSTATE_ON, &pmsg2->BootInfo);
		}
		yCASE(MP_MORNITORMAPSERVER_PING_ACK)
		{
			MSERVERINFO * pInfo = MServerTable->GetServerInfoForConn(dwConnectionIndex);
			__time64_t tick;
			_time64( &tick );
			pInfo->ExpiredTime = tick;
			if(pInfo->wServerState == SERVERSTATE_ABNORMAL )
			{
				MSMGR->SendServerState(SERVERSTATE_RECOVER, pInfo);
			}
			pInfo->wServerState = SERVERSTATE_ON;
		}
		yCASE(MP_MORNITORMAPSERVER_CHANGE_USERLEVEL_ACK)
		{
			MSG_MCDWORD* pMsg = (MSG_MCDWORD*)pmsg;
			pMsg->Category = MP_MORNITORSERVER;
			pMsg->Protocol = MP_MORNITORSERVER_CHANGE_USERLEVEL_ACK;
			MNETWORK->SendToMAS( (char*)pMsg, sizeof(MSG_MCDWORD) );
		}
		yCASE(MP_MORNITORMAPSERVER_DISCONNECT_ALLUSER_ACK)
		{
			MSERVERINFO* pInfo = MServerTable->GetServerInfoForConn(dwConnectionIndex);

			MSGSERVERSTATE msg;
			msg.Category = MP_MORNITORSERVER;
			msg.Protocol = MP_MORNITORSERVER_DISCONNECT_ALLUSER_ACK;
			strcpy(msg.serverState.ServerIP, pInfo->szIPForServer);
			msg.serverState.ServerPort		= pInfo->wPortForServer;
			msg.serverState.ServerType		= pInfo->wServerType;
			msg.serverState.ServerNum		= pInfo->wServerNum;
			MNETWORK->SendToMAS((char*)&msg, sizeof(msg));
		}
		yCASE(MP_MORNITORMAPSERVER_QUERY_VERSION_ACK)
		{
			MSGNOTICE* msg = (MSGNOTICE*)pmsg;
			msg->Category = MP_MORNITORSERVER;
			msg->Protocol = MP_MORNITORSERVER_QUERY_VERSION_ACK;
			MNETWORK->SendToMAS( (char*)msg, sizeof(MSGNOTICE) );
		}
		yCASE(MP_MORNITORMAPSERVER_CHANGE_VERSION_ACK)
		{
			MSGNOTICE* msg = (MSGNOTICE*)pmsg;
			msg->Category = MP_MORNITORSERVER;
			msg->Protocol = MP_MORNITORSERVER_CHANGE_VERSION_ACK;
			MNETWORK->SendToMAS( (char*)msg, sizeof(MSGNOTICE) );
		}
		yCASE(MP_MORNITORMAPSERVER_QUERY_MAXUSER_ACK)
		{
			MSG_MCDWORD* pMsg = (MSG_MCDWORD*)pmsg;
			pMsg->Category = MP_MORNITORSERVER;
			pMsg->Protocol = MP_MORNITORSERVER_QUERY_MAXUSER_ACK;
			MNETWORK->SendToMAS( (char*)pMsg, sizeof(MSG_MCDWORD) );
		}
		yCASE(MP_MORNITORMAPSERVER_CHANGE_MAXUSER_ACK)
		{
			MSG_MCDWORD* pMsg = (MSG_MCDWORD*)pmsg;
			pMsg->Category = MP_MORNITORSERVER;
			pMsg->Protocol = MP_MORNITORSERVER_CHANGE_MAXUSER_ACK;
			MNETWORK->SendToMAS( (char*)pMsg, sizeof(MSG_MCDWORD) );
		}
		yENDSWITCH
	}
	yCASE(MP_MORNITORSERVER)	// from MAS
	{
		ySWITCH(pMsg->Protocol)
		yCASE(MP_MORNITORSERVER_QUERYUSERCOUNT_SYN)
		{
			// send to mapserver all
			MSGMC * pMsg = (MSGMC *)pmsg;
			pMsg->Category = MP_MORNITORMAPSERVER;
			pMsg->Protocol = MP_MORNITORMAPSERVER_QUERYUSERCOUNT_SYN;
			MNETWORK->SendToMapServerAll((char*)pMsg, dwLength);
		}
		yCASE(MP_MORNITORSERVER_QUERY_CHANNELINFO_SYN)
		{
			MSGROOT Msg;
			Msg.Category = MP_MORNITORMAPSERVER;
			Msg.Protocol = MP_MORNITORMAPSERVER_QUERY_CHANNELINFO_SYN;
			MNETWORK->SendToMapServerAll( (char*)&Msg, sizeof(MSGROOT) );
		}
		yCASE(MP_MORNITORSERVER_BOOTLIST_NOTIFY)
		{
			//to mapserver
			MSG_PWRUP_BOOTLIST *pmsg2 = (MSG_PWRUP_BOOTLIST *)pmsg;
			pmsg2->Category = MP_POWERUP;
			pmsg2->Protocol = MP_POWERUP_BOOTLIST_ACK;
			MNETWORK->SendToMapServer(MServerTable->GetServerInfoForPort(pmsg2->MapServerPort)->dwConnectionIndex, pmsg, dwLength);
		}
		//to mapserver
		yCASE(MP_MORNITORSERVER_EXECUTE_MAPSERVER_SYN)
		{
			// ¼­¹ö ±¸µ¿!!!!
			MSGEXECUTESERVER * pmsg2 = (MSGEXECUTESERVER *)pmsg;
			MSMGR->ExecuteServer(pmsg2->ServerType, pmsg2->ServerNum);
		}
		yCASE(MP_MORNITORSERVER_SHUTDOWN_MAPSERVER_SYN)		// msg¸¦ ÅëÇØ
		{
			MSGSHUTDOWNSERVER * pmsg2 = (MSGSHUTDOWNSERVER *)pmsg;
			MSMGR->ServerOff(pmsg2->ServerPort);
		}
		yCASE(MP_MORNITORSERVER_KILL_SERVER_SYN)			// °­Á¦Á¾·á(abnormal)
		{
			MSGSHUTDOWNSERVER* pmsg2 = (MSGSHUTDOWNSERVER*)pmsg;
			MSMGR->KillServer(pmsg2->ServerPort);
		}
		yCASE(MP_MORNITORSERVER_NOTICESEND_SYN)
		{
			MSGNOTICE * pmsg2 = (MSGNOTICE *)pmsg;
			pmsg2->Category = MP_MORNITORMAPSERVER;
			pmsg2->Protocol = MP_MORNITORMAPSERVER_NOTICESEND_SYN;
			// ¿¡ÀÌÀüÆ®¿¡°Ô¸¸
			MSMGR->SendToNotice(pmsg2, dwLength);
		}
		yCASE(MP_MORNITORSERVER_EVENTNOTICE_ON)
		{
			MSG_EVENTNOTIFY_ON* pmsg2 = (MSG_EVENTNOTIFY_ON*)pmsg;
			pmsg2->Category = MP_MORNITORMAPSERVER;
			pmsg2->Protocol = MP_MORNITORMAPSERVER_EVENTNOTICE_ON;
			MSMGR->SendToAgent( (char*)pmsg2, dwLength );
		}
		yCASE(MP_MORNITORSERVER_EVENTNOTICE_OFF)
		{
			MSGBASE* pmsg2 = (MSGBASE*)pmsg;
			pmsg2->Category = MP_MORNITORMAPSERVER;
			pmsg2->Protocol = MP_MORNITORMAPSERVER_EVENTNOTICE_OFF;
			MSMGR->SendToAgent( (char*)pmsg2, dwLength );
		}
		yCASE(MP_MORNITORSERVER_EXECUTE_AUTOPATCH_SYN)	// AutoPatch
		{			
			MSMGR->ReadAutoPatchVersion();
			MSMGR->AutoPatch();
			bAutoPatch = TRUE;
		}
		yCASE(MP_MORNITORSERVER_CHANGE_USERLEVEL_SYN)	// UserLevel
		{
			MSG_MCDWORD* msg = (MSG_MCDWORD*)pmsg;
			msg->Category = MP_MORNITORMAPSERVER;
			msg->Protocol = MP_MORNITORMAPSERVER_CHANGE_USERLEVEL_SYN;

			// Distribute
			MSMGR->SendToDistribute( (char*)msg, dwLength );
		}
		yCASE(MP_MORNITORSERVER_ASSERTMSGBOX_SYN)
		{
			MSG_MCDWORD* pMsg = (MSG_MCDWORD *)pmsg;
			pMsg->Category = MP_MORNITORMAPSERVER;
			pMsg->Protocol = MP_MORNITORMAPSERVER_ASSERTMSGBOX_SYN;
			MNETWORK->SendToMapServerAll((char*)pMsg, dwLength);
		}
		yCASE(MP_MORNITORSERVER_DISCONNECT_ALLUSER_SYN)
		{
			MSG_MCDWORD msg;
			msg.Category = MP_MORNITORMAPSERVER;
			msg.Protocol = MP_MORNITORMAPSERVER_DISCONNECT_ALLUSER_SYN;
			MNETWORK->SendToMapServerAll((char*)&msg, sizeof(msg));
		}
		yCASE(MP_MORNITORSERVER_QUERY_VERSION_SYN)
		{
			MSGROOT* pmsg = (MSGROOT*)pMsg;
			pmsg->Category = MP_MORNITORMAPSERVER;
			pmsg->Protocol = MP_MORNITORMAPSERVER_QUERY_VERSION_SYN;
			MSMGR->SendToDistribute( (char*)pmsg, dwLength );
		}
		yCASE(MP_MORNITORSERVER_CHANGE_VERSION_SYN)
		{
			MSGNOTICE* pmsg = (MSGNOTICE*)pMsg;
			pmsg->Category = MP_MORNITORMAPSERVER;
			pmsg->Protocol = MP_MORNITORMAPSERVER_CHANGE_VERSION_SYN;
			MSMGR->SendToDistribute( (char*)pmsg, dwLength );
		}
		yCASE(MP_MORNITORSERVER_QUERY_MAXUSER_SYN)
		{
			MSGROOT* pmsg = (MSGROOT*)pMsg;
			pmsg->Category = MP_MORNITORMAPSERVER;
			pmsg->Protocol = MP_MORNITORMAPSERVER_QUERY_MAXUSER_SYN;
			MSMGR->SendToDistribute( (char*)pmsg, dwLength );
		}
		yCASE(MP_MORNITORSERVER_CHANGE_MAXUSER_SYN)
		{
			MSG_MCDWORD* pmsg = (MSG_MCDWORD*)pMsg;
			pmsg->Category = MP_MORNITORMAPSERVER;
			pmsg->Protocol = MP_MORNITORMAPSERVER_CHANGE_MAXUSER_SYN;
			MSMGR->SendToDistribute( (char*)pmsg, dwLength );
		}
		yCASE(MP_MORNITORSERVER_QUERY_AUTOPATCHVERSION_SYN)
		{
			MSGMSSTATE Msg;
			Msg.Category = MP_MORNITORSERVER;
			Msg.Protocol = MP_MORNITORSERVER_QUERY_AUTOPATCHVERSION_ACK;
			MSERVERINFO* pInfo = MServerTable->GetMSInfo();
			strcpy( Msg.MSState.sServerIP, pInfo->szIPForServer );
			Msg.MSState.wServerPort = pInfo->wPortForServer;
			Msg.MSState.wAutoPatchState = AUTOPATCHSTATE_END;
			MSMGR->ReadAutoPatchVersion();
			strcpy( Msg.MSState.sAutoPatchVersion, MSMGR->GetAutoPatchVersion() );
			MNETWORK->SendToMAS( (char*)&Msg, sizeof(Msg) );
		}
		yENDSWITCH
	}
	yENDSWITCH
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::ReceivedMsgFromMS(DWORD dwConnectionIndex,char* pmsg,DWORD dwLength)
{
	MSGROOT* pMsg = (MSGROOT*)pmsg;
	ySWITCH(pMsg->Category)
	yCASE(MP_MORNITORSERVER)
	{
		ySWITCH(pMsg->Protocol)
		yCASE(MP_MORNITORSERVER_QUERYUSERCOUNT_ACK)
		{
			MSGUSERCOUNT * pMsg = (MSGUSERCOUNT *)pmsg;
			SERVERSTATE * pState = MASMGR->GetServerState(pMsg->ServerPort);
			pState->UserCount = pMsg->dwUserCount;
			MASMGR->SendServerStateToMC(pMsg->dwConnectionIdxMC, pMsg->ServerPort);
		}
		yCASE(MP_MORNITORSERVER_QUERY_CHANNELINFO_ACK)
		{
			MSG_CHANNEL_INFO_MORNITOR* pMsg = (MSG_CHANNEL_INFO_MORNITOR*)pmsg;
			pMsg->Category = MP_MORNITORTOOL;
			pMsg->Protocol = MP_MORNITORTOOL_QUERY_CHANNELINFO_ACK;
			MASMGR->SendToAllMC( (char*)pMsg, sizeof(MSG_CHANNEL_INFO_MORNITOR) );			
		}
		yCASE(MP_MORNITORSERVER_MSINFO_NOTIFY)
		{
			MSGMSINFO * pmsg2 = (MSGMSINFO *)pMsg;
			pmsg2->MSInfo.dwConnectionIndex = dwConnectionIndex;
			//*MServerTable->GetMSInfo() = pmsg2->MSInfo;
			// ¿©·¯°³ÀÇ MS°¡ ºÙÀ½
			// MAPÀÌ ¾î´À MC¿¡ ºÙ¾î ÀÖ´ÂÁö ¾Ë¾Æ¾ß ÇÔ
			LogConsole("From MSINFO_NOTIFY : CON:%d, IP:%s, PORT:%d",pmsg2->MSInfo.dwConnectionIndex, pmsg2->MSInfo.szIPForServer, pmsg2->MSInfo.wPortForServer);
			MServerTable->AddServer(&pmsg2->MSInfo, pmsg2->MSInfo.wPortForServer);

			MASMGR->SendMSServerStateAllToMCAll();
		}
		yCASE(MP_MORNITORSERVER_EXECUTE_MAPSERVER_ACK)
		//send to mc
		{
			MSGEXECUTESERVER * pmsg2 = (MSGEXECUTESERVER *)pMsg;
			pmsg2->Category = MP_MORNITORTOOL;
			pmsg2->Protocol = MP_MORNITORTOOL_EXECUTE_MAPSERVER_ACK;
			MNETWORK->SendToMS(pmsg2->dwConnectionIdxMC, pmsg, dwLength);
		}
		yCASE(MP_MORNITORSERVER_MAPSERVERSTATE_ACK)
		{
			MSGSERVERSTATE * pmsg2 = (MSGSERVERSTATE *)pMsg;
			SERVERSTATE * pState = MASMGR->GetServerState(pmsg2->serverState.ServerPort);
			WORD OldState = pState->ServerState = pmsg2->serverState.ServerState;
						
			// 1. mapserver bootup		on
			// send conn list
			// 2. mapserver down		abnormal
			// 3. mapserver shutdown	off
			ySWITCH(pmsg2->serverState.ServerState)
				yCASE(SERVERSTATE_ON)
					MASMGR->SendServerStateBootListToMS(dwConnectionIndex, pState->ServerType, pState->ServerPort);
				
				yCASE(SERVERSTATE_OFF)

					// 090519 ShinJS --- Server Off 시 Insert SMS Data
					// 090709 ShinJS --- Server Tool에서 Off정보 확인
					if( pmsg2->bOffByServerTool == FALSE )
						InsertSMSData( dwConnectionIndex, pmsg2->serverState.ServerType, pmsg2->serverState.ServerNum );

				yCASE(SERVERSTATE_ABNORMAL)
					if(OldState == SERVERSTATE_OFF)
					{
						ASSERTMSG(0,"OFFµÇ¾ú´Â µ¥ ´Ù½Ã ABNORMAL msg recv");
						pState->ServerState = SERVERSTATE_OFF;
					}
					
				yCASE(SERVERSTATE_RECOVER)
					pState->ServerState = SERVERSTATE_ON;
				
			yENDSWITCH

			MASMGR->SendServerStateToMCs(pmsg2->serverState.ServerPort);

		}
		yCASE(MP_MORNITORSERVER_SHUTDOWN_MAPSERVER_ACK)
		//send to mc
		{
			MSGSHUTDOWNSERVER * pmsg2 = (MSGSHUTDOWNSERVER *)pMsg;
			pmsg2->Category = MP_MORNITORTOOL;
			pmsg2->Protocol = MP_MORNITORTOOL_NOTICESEND_ACK;
			MNETWORK->SendToMS(pmsg2->dwConnectionIdxMC, pmsg, dwLength);
		}
		yCASE(MP_MORNITORSERVER_CHANGE_USERLEVEL_ACK)
		//send to mc
		{
			MSG_MCDWORD * pmsg = (MSG_MCDWORD*)pMsg;
			pmsg->Category = MP_MORNITORTOOL;
			pmsg->Protocol = MP_MORNITORTOOL_CHANGE_USERLEVEL_ACK;
			MASMGR->SendToAllMC( (char*)pmsg, dwLength );
		}
		yCASE(MP_MORNITORSERVER_DISCONNECT_ALLUSER_ACK)
		{
			MSGSERVERSTATE* pmsg = (MSGSERVERSTATE*)pMsg;
			pmsg->Category = MP_MORNITORTOOL;
			pmsg->Protocol = MP_MORNITORTOOL_DISCONNECT_ALLUSER_ACK;
			MASMGR->SendToAllMC( (char*)pmsg, dwLength );
		}
		yCASE(MP_MORNITORSERVER_QUERY_VERSION_ACK)
		{
			MSGNOTICE* msg = (MSGNOTICE*)pMsg;
			msg->Category = MP_MORNITORTOOL;
			msg->Protocol = MP_MORNITORTOOL_QUERY_VERSION_ACK;
			MASMGR->SendToAllMC( (char*)msg, sizeof(MSGNOTICE) );
		}
		yCASE(MP_MORNITORSERVER_CHANGE_VERSION_ACK)
		{
			MSGNOTICE* msg = (MSGNOTICE*)pMsg;
			msg->Category = MP_MORNITORTOOL;
			msg->Protocol = MP_MORNITORTOOL_CHANGE_VERSION_ACK;
			MASMGR->SendToAllMC( (char*)msg, sizeof(MSGNOTICE) );
		}
		yCASE(MP_MORNITORSERVER_QUERY_MAXUSER_ACK)
		{
			MSG_MCDWORD* msg = (MSG_MCDWORD*)pMsg;
			msg->Category = MP_MORNITORTOOL;
			msg->Protocol = MP_MORNITORTOOL_QUERY_MAXUSER_ACK;
			MASMGR->SendToAllMC( (char*)msg, sizeof(MSG_MCDWORD) );
		}
		yCASE(MP_MORNITORSERVER_CHANGE_MAXUSER_ACK)
		{
			MSG_MCDWORD* msg = (MSG_MCDWORD*)pMsg;
			msg->Category = MP_MORNITORTOOL;
			msg->Protocol = MP_MORNITORTOOL_CHANGE_MAXUSER_ACK;
			MASMGR->SendToAllMC( (char*)msg, sizeof(MSG_MCDWORD) );
		}
		// AutoPatch
		yCASE(MP_MORNITORSERVER_CHECK_AUTOPATCH_ACK)
		{
			MSGMSSTATE* pmsg = (MSGMSSTATE*)pMsg;
			pmsg->Category = MP_MORNITORTOOL;
			pmsg->Protocol = MP_MORNITORTOOL_CHECK_AUTOPATCH_ACK;
			MASMGR->SendToAllMC( (char*)pmsg, sizeof(MSGMSSTATE) );
		}
		yCASE(MP_MORNITORSERVER_QUERY_AUTOPATCHVERSION_ACK)
		{
			MSGMSSTATE* pmsg = (MSGMSSTATE*)pMsg;
			pmsg->Category = MP_MORNITORTOOL;
			pmsg->Protocol = MP_MORNITORTOOL_QUERY_AUTOPATCHVERSION_ACK;
			MASMGR->SendToAllMC( (char*)pmsg, sizeof(MSGMSSTATE) );
		}
		yENDSWITCH
	}
	yENDSWITCH
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::ReceivedMsgFromServer(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	ySWITCH(g_pMSSystem->m_wServerType)
	yCASE(TYPE_MS)
		ReceivedMsgFrom2Server(dwConnectionIndex, pMsg, dwLength);
	yCASE(TYPE_MAS)
		ReceivedMsgFromMS(dwConnectionIndex,pMsg, dwLength);
	yENDSWITCH
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::ReceivedMsgFromUser(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	if( pTempMsg->Category == MP_RMTOOL_CONNECT )
	{
		ConnentMsgParse(dwConnectionIndex, pMsg, dwLength);
		return;
	}

	if( !IPCHECKMGR->CheckIP( dwConnectionIndex ) )
	{
		MUserTable->RemoveUser( dwConnectionIndex );
		return;
	}

	ySWITCH(pTempMsg->Category)
	yCASE(MP_MORNITORTOOL)
	{
		// 090511 LUJ, 요청이 권한에 맞는지 검사한다
		if( IPCHECKMGR->IsInvalidProtocol( dwConnectionIndex, MP_PROTOCOL_MORNITORTOOL( pTempMsg->Protocol ) ) )
		{
			MSG_DWORD message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category = MP_MORNITORTOOL;
			message.Protocol = MP_MORNITORTOOL_NO_AUTHORITY_ACK;
			message.dwData = pTempMsg->Protocol;
			MNETWORK->SendToMC( dwConnectionIndex, LPTSTR( &message ), sizeof( message ) );

			char text[ MAX_PATH ] = { 0 };
			sprintf( text, "request failed(protocol: %d)\n", pTempMsg->Protocol );
			OutputDebugString( text );
			break;
		}

		ySWITCH(pTempMsg->Protocol)
		yCASE(MP_MORNITORTOOL_QUERYUSERCOUNT_SYN)
		{
			// send to ms all
			MSGMC msg;
			msg.Category = MP_MORNITORSERVER;
			msg.Protocol = MP_MORNITORSERVER_QUERYUSERCOUNT_SYN;
			msg.dwConnectionIdxMC = dwConnectionIndex;
			MNETWORK->SendToMSAll((char*)&msg, sizeof(msg));
		}
		yCASE(MP_MORNITORTOOL_QUERY_CHANNELINFO_SYN)
		{
			MSGROOT Msg;
			Msg.Category = MP_MORNITORSERVER;
			Msg.Protocol = MP_MORNITORSERVER_QUERY_CHANNELINFO_SYN;
			MNETWORK->SendToMSAll( (char*)&Msg, sizeof(MSGROOT) );
		}
		yCASE(MP_MORNITORTOOL_EXECUTE_MAPSERVER_SYN)
		{
			MSGEXECUTESERVER * pmsg = (MSGEXECUTESERVER *)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_EXECUTE_MAPSERVER_SYN;
			pmsg->dwConnectionIdxMC = dwConnectionIndex;
			// map¼­¹ö Æ÷Æ®·Î mapÀÌ ¼Ò¼ÓµÈ ms¸¦ Ã£¾Æ¼­ ms·Î º¸³½´Ù.
			
			MSERVERINFO * pInfo = MServerTable->GetMSServerInfoForIP(pmsg->ServerIP);
			if(pInfo)
			{

				MNETWORK->SendToMS(pInfo->dwConnectionIndex, pMsg, dwLength);
				LogConsole("sendEXEMsg To MS(CON:%d, IP:%s, PORT:%d)", pInfo->dwConnectionIndex, pInfo->szIPForServer, pInfo->wPortForServer);
			}			
		}
		yCASE(MP_MORNITORTOOL_SHUTDOWN_MAPSERVER_SYN)	// Msg·Î ¼­¹ö ²û.
		{
			MSGSHUTDOWNSERVER * pmsg = (MSGSHUTDOWNSERVER *)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_SHUTDOWN_MAPSERVER_SYN;
			pmsg->dwConnectionIdxMC = dwConnectionIndex;
			// map¼­¹ö Æ÷Æ®·Î mapÀÌ ¼Ò¼ÓµÈ ms¸¦ Ã£¾Æ¼­ ms·Î º¸³½´Ù.
			MSERVERINFO * pInfo = MServerTable->GetMSServerInfoForIP(pmsg->ServerIP);
			if(pInfo)
			MNETWORK->SendToMS(pInfo->dwConnectionIndex, pMsg, dwLength);
		}
		yCASE(MP_MORNITORTOOL_KILL_SERVER_SYN)			// °­Á¦ Á¾·á(abnormal)
		{
			MSGSHUTDOWNSERVER* pmsg = (MSGSHUTDOWNSERVER*)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_KILL_SERVER_SYN;
			pmsg->dwConnectionIdxMC = dwConnectionIndex;
			// map¼­¹ö Æ÷Æ®·Î mapÀÌ ¼Ò¼ÓµÈ ms¸¦ Ã£¾Æ¼­ ms·Î º¸³½´Ù.
			MSERVERINFO * pInfo = MServerTable->GetMSServerInfoForIP(pmsg->ServerIP);
			if(pInfo)
				MNETWORK->SendToMS(pInfo->dwConnectionIndex, pMsg, dwLength);
		}
		yCASE(MP_MORNITORTOOL_NOTICESEND_SYN)
		{
			MSGNOTICE * pmsg = (MSGNOTICE *)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_NOTICESEND_SYN;
			pmsg->dwConnectionIdxMC = dwConnectionIndex;
			MNETWORK->SendToMSAll(pMsg, dwLength);
		}
		yCASE(MP_MORNITORTOOL_EVENTNOTICE_ON)
		{
			MSG_EVENTNOTIFY_ON* pmsg = (MSG_EVENTNOTIFY_ON*)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_EVENTNOTICE_ON;
			MNETWORK->SendToMSAll(pMsg, dwLength);
		}
		yCASE(MP_MORNITORTOOL_EVENTNOTICE_OFF)
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_EVENTNOTICE_OFF;
			MNETWORK->SendToMSAll(pMsg, dwLength);
		}
		yCASE(MP_MORNITORTOOL_MAPSERVERSTATE_SYN)
		{
			// Request
			//¼­¹ö »óÅÂ ACK
			MASMGR->SendServerStateAllToMC(dwConnectionIndex);
		}
		yCASE(MP_MORNITORTOOL_EXECUTE_AUTOPATCH_SYN)	// AutoPatch
		{
			MSGROOT msg;
			msg.Category = MP_MORNITORSERVER;
			msg.Protocol = MP_MORNITORSERVER_EXECUTE_AUTOPATCH_SYN;
			MNETWORK->SendToMSAll( (char*)&msg, sizeof(msg));
		}
		yCASE(MP_MORNITORTOOL_CHANGE_USERLEVEL_SYN)		// UserLevel
		{
			MSGROOT* msg = (MSGROOT*)pMsg;
			msg->Category = MP_MORNITORSERVER;
			msg->Protocol = MP_MORNITORSERVER_CHANGE_USERLEVEL_SYN;
			MNETWORK->SendToMSAll( (char*)msg, dwLength );			
		}
		yCASE(MP_MORNITORTOOL_ASSERTMSGBOX_SYN)
		{
			MSG_MCDWORD* msg = (MSG_MCDWORD*)pMsg;
			msg->Category = MP_MORNITORSERVER;
			msg->Protocol = MP_MORNITORSERVER_ASSERTMSGBOX_SYN;
			MNETWORK->SendToMSAll( (char*)msg, dwLength );
		}
		yCASE(MP_MORNITORTOOL_DISCONNECT_ALLUSER_SYN)
		{
			MSGEXECUTESERVER* pmsg = (MSGSHUTDOWNSERVER *)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_DISCONNECT_ALLUSER_SYN;
			pmsg->dwConnectionIdxMC = dwConnectionIndex;
			// map¼­¹ö Æ÷Æ®·Î mapÀÌ ¼Ò¼ÓµÈ ms¸¦ Ã£¾Æ¼­ ms·Î º¸³½´Ù.
			MSERVERINFO * pInfo = MServerTable->GetMSServerInfoForIP(pmsg->ServerIP);
			if(pInfo)
				MNETWORK->SendToMS(pInfo->dwConnectionIndex, pMsg, dwLength);
			
//			MNETWORK->SendToMSAll( (char*)pmsg, dwLength );
		}		
		yCASE(MP_MORNITORTOOL_QUERY_VERSION_SYN)
		{
			MSGROOT* pmsg = (MSGROOT*)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_QUERY_VERSION_SYN;
			MNETWORK->SendToMSAll( (char*)pmsg, dwLength );
		}
		yCASE(MP_MORNITORTOOL_CHANGE_VERSION_SYN)
		{
			MSGNOTICE* pmsg = (MSGNOTICE*)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_CHANGE_VERSION_SYN;
			MNETWORK->SendToMSAll( (char*)pmsg, dwLength );
		}
		yCASE(MP_MORNITORTOOL_QUERY_MAXUSER_SYN)
		{
			MSGROOT* pmsg = (MSGROOT*)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_QUERY_MAXUSER_SYN;
			MNETWORK->SendToMSAll( (char*)pmsg, dwLength );	
		}
		yCASE(MP_MORNITORTOOL_CHANGE_MAXUSER_SYN)
		{
			MSG_MCDWORD* pmsg = (MSG_MCDWORD*)pMsg;
			pmsg->Category = MP_MORNITORSERVER;
			pmsg->Protocol = MP_MORNITORSERVER_CHANGE_MAXUSER_SYN;
			MNETWORK->SendToMSAll( (char*)pmsg, dwLength );			
		}
		yCASE(MP_MORNITORTOOL_MSSTATEALL_SYN)
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

			MNETWORK->SendToMC( dwConnectionIndex, (char*)&Msg, Msg.GetSize() );
		}
		yCASE(MP_MORNITORTOOL_QUERY_AUTOPATCHVERSION_SYN)
		{
			MSGROOT Msg;
			Msg.Category = MP_MORNITORSERVER;
			Msg.Protocol = MP_MORNITORSERVER_QUERY_AUTOPATCHVERSION_SYN;
			MNETWORK->SendToMSAll( (char*)&Msg, sizeof(MSGROOT) );
		}
		yENDSWITCH
	}
/*	yCASE(MP_RMTOOL_CONNECT)
	{
		ConnentMsgParse(dwConnectionIndex, pMsg, dwLength);
	}
	yCASE(MP_RMTOOL_USER)
	{
		UserMsgParse(dwConnectionIndex, pMsg, dwLength);
	}
	yCASE(MP_RMTOOL_MUNPA)
	{
		MunpaMsgParse(dwConnectionIndex, pMsg, dwLength);
	}
	yCASE(MP_RMTOOL_GAMELOG)
	{
		GameLogMsgParse(dwConnectionIndex, pMsg, dwLength);
	}
	yCASE(MP_RMTOOL_OPERLOG)
	{
		OperLogMsgParse(dwConnectionIndex, pMsg, dwLength);
	}
	yCASE(MP_RMTOOL_STATISTICS)
	{
		StatisticsMsgParse(dwConnectionIndex, pMsg, dwLength);
	}
	yCASE(MP_RMTOOL_ADMIN)
	{
		AdminMsgParse(dwConnectionIndex, pMsg, dwLength);
	}
	yCASE(MP_RMTOOL_CHARACTER)
	{
		CharacterMsgParse(dwConnectionIndex, pMsg, dwLength);
	}
	yCASE( MP_RMTOOL_ITEM )
	{
		ItemMsgParse(dwConnectionIndex, pMsg, dwLength);
	}
	*/
	yENDSWITCH
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::OnAcceptUser(DWORD dwConnectionIndex)
{
	MCINFO Info;
	Info.dwConnectionIndex = dwConnectionIndex;
	MUserTable->AddUser(&Info, dwConnectionIndex);
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::OnDisconnectUser(DWORD dwConnectionIndex)
{
	MUserTable->RemoveUser(dwConnectionIndex);
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::ReceivedMsgFromRMTool( DWORD dwConnectionIndex,char* pMsg,DWORD dwLength )
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;

	if( pTempMsg->Category == MP_RMTOOL_CONNECT )
	{
		ConnentMsgParse(dwConnectionIndex, pMsg, dwLength);
		return;
	}

	if( !IPCHECKMGR->CheckIP( dwConnectionIndex ) )
	{
		MUserTable->RemoveUser( dwConnectionIndex );
		return;
	}

}

void CMSSystem::ConnentMsgParse(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_RMTOOL_CONNECT_SYN:
		{
			TMSG_OPERATOR_LOGIN* msg = (TMSG_OPERATOR_LOGIN*)pMsg;
			char strIP[20] = {0,};
			WORD wPort = 0;
			MNETWORK->GetUserAddress(dwConnectionIndex, strIP, &wPort);

			if( g_pMSSystem->m_wServerType == TYPE_MAS )
			{
				OperatorLoginCheck(dwConnectionIndex, msg->sOper.strOperID, msg->sOper.strOperPWD, strIP);
			}

			break;
		}
	}
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::OnConnectServerSuccess(DWORD dwConnectionIndex, void* pVoid)
{
	MSERVERINFO* pInfo = (MSERVERINFO*)pVoid;
	MSERVERINFO Info;
	memcpy( &Info, pInfo, sizeof(MSERVERINFO) );
	Info.dwConnectionIndex = dwConnectionIndex;
	Info.ExpiredTime = 0;
	Info.wServerState = SERVERSTATE_ON;
	MServerTable->AddServer(&Info, Info.wPortForServer);
}

// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
void __stdcall CMSSystem::OnConnectServerFail(void* pVoid)
{
	//MessageBox( NULL, "MAS is Off!!. First You Turn On MAS!!!", "Error!!", MB_OK );
	//::SendMessage(gMainHwnd,WM_CLOSE,0,0);
}
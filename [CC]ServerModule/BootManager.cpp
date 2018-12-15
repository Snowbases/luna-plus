// BootManager.cpp: implementation of the CBootManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BootManager.h"
#include "Network.h"
#include "ServerTable.h"
#include "..\[CC]Header\CommonGameFunc.h"
#include "MHFile.h"
#include "ServerListManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern int g_nServerSetNum;

GLOBALTON(CBootManager);
CBootManager::CBootManager()
{
	
}

CBootManager::~CBootManager()
{

}

BOOL CBootManager::ConnectToMS(CNetwork * pNet, CServerTable * pServerTable)
{
	
	SERVERINFO* pNewServerInfo = GetServerInfoByIp(MONITOR_SERVER,
										pServerTable->GetSelfServer()->szIPForServer,
										pServerTable->GetSelfServer()->szIPForUser);
	
	if(pNewServerInfo == NULL)
		return FALSE;

	pServerTable->AddMSServer(pNewServerInfo);

	return pNet->ConnectToServer(pNewServerInfo->szIPForServer, pNewServerInfo->wPortForServer, (void *)pNewServerInfo);
}

void CBootManager::AddSelfBootList(WORD MyServerKind, WORD MyServerNum,  CServerTable * pServerTable)
{
	char filename[256];
	FILE* fp = fopen("serverset.txt","r");
	if(fp == NULL)
	{
		MessageBox(NULL,"serverset.txt is not exist. this server will be closed",0,0);
		exit(0);
		return;
	}
	int serversetnum;
	fscanf(fp,"%d",&serversetnum);
	fclose(fp);

	// 20090303 nsoh 서버리스트 Text파일 로드
	//sprintf(filename,"serverset/%d/serverlist.msl",serversetnum);
	//SERVERLISTMGR->Load(filename);
	sprintf(filename,"serverset/%d/serverlist.txt",serversetnum);
	if(!SERVERLISTMGR->LoadFromText(filename))
	{
		sprintf( filename, "serverset/%d/serverlist.msl",serversetnum );

		// 090525 LUJ, serverlist.msl도 로딩 시도한다
		if( FALSE == SERVERLISTMGR->Load( filename ) )
		{
			MessageBox( 0," serverlist.msl/txt is not found", 0, 0 );
			exit( 0 );
		}
	}
	
	if( !ReadServerSetNum() )
		g_nServerSetNum = 1;

	ASSERT(g_nServerSetNum == serversetnum);

	SERVERINFO* pSelfServerInfo = GetServerInfoByNum(MyServerKind,MyServerNum);

	pServerTable->AddSelfServer(pSelfServerInfo);
}

void CBootManager::AddBootListINI(WORD MyServerKind, WORD MyServerNum,  CServerTable * pServerTable)
{
	SERVERINFO* pInfo;
	SERVERLISTMGR->SetPositionHead();
	while((pInfo = SERVERLISTMGR->GetNextServerInfo()) != NULL)
	{
		if(CheckLinkServer(MyServerKind,pInfo->wServerKind) == FALSE)
			continue;
		if( MyServerKind == pInfo->wServerKind &&
			MyServerNum == pInfo->wServerNum )
			continue;
		
		SERVERINFO * NewServerInfo = new SERVERINFO(pInfo);
		pServerTable->AddServer(NewServerInfo, NewServerInfo->wPortForServer);
	}
}

BOOL CBootManager::CheckLinkServer(WORD SelfServerType,WORD ToServerType)
{
	switch(SelfServerType)
	{
	case DISTRIBUTE_SERVER:
		if(ToServerType == AGENT_SERVER)
			return TRUE;

		break;
	case AGENT_SERVER:
		if(ToServerType == AGENT_SERVER)
			return TRUE;
		if(ToServerType == MAP_SERVER)
			return TRUE;
		if(ToServerType == DISTRIBUTE_SERVER)
			return TRUE;

		break;
	case MAP_SERVER:
		if(ToServerType == AGENT_SERVER)
			return TRUE;

		break;

	default:
		ASSERT(0);
	}

	return FALSE;
}

BOOL CBootManager::StartServer(CNetwork * pNet, 	SERVERINFO * pSelfServerInfo)
{
	if(!pNet->StartServerServer(pSelfServerInfo->szIPForServer, pSelfServerInfo->wPortForServer))
	{
		g_Console.LOG(4, "Error StartServerForServer : %s, %d", pSelfServerInfo->szIPForServer, pSelfServerInfo->wPortForServer);
		return FALSE;
	}
	else
	{
		g_Console.LOG(4, "StartServerForServer : %s, %d", pSelfServerInfo->szIPForServer, pSelfServerInfo->wPortForServer);
	}
	if( pSelfServerInfo->wServerKind == AGENT_SERVER || pSelfServerInfo->wServerKind == DISTRIBUTE_SERVER )
	{
		if(!pNet->StartUserServer(pSelfServerInfo->szIPForUser, pSelfServerInfo->wPortForUser))
		{
			g_Console.LOG(4, "Error StartServerForUser : %s, %d", pSelfServerInfo->szIPForUser, pSelfServerInfo->wPortForUser);
			return FALSE;
		}
		else
		{
			g_Console.LOG(4, "StartServerForUser : %s, %d", pSelfServerInfo->szIPForUser, pSelfServerInfo->wPortForUser);
		}
	}
	return TRUE;
}

void CBootManager::NotifyBootUpToMS(CNetwork * pNet)
{
	MSG_PWRUP_BOOTUP msg;
	msg.Category = MP_MORNITORMAPSERVER;
	msg.Protocol = MP_MORNITORMAPSERVER_BOOTUP_NOTIFY;
	msg.dwProcessID = GetCurrentProcessId();
	msg.BootInfo = *g_pServerTable->GetSelfServer();
	pNet->Send2Server(g_pServerTable->GetMSServer()->dwConnectionIndex, (char*)&msg, sizeof(MSG_PWRUP_BOOTUP));
	g_Console.LOG(4, "bootup send");
}

void CBootManager::AddBootList(CServerTable * pServerTable, SERVERINFO * pServerInfo)
{
	// YH Edit 확인 요
	//ASSERT(!pServerTable->GetData(pServerInfo->wPortForServer));
	// 090706 ShinJS --- 서버정보 존재시 Update
	if( pServerTable->GetData(pServerInfo->wPortForServer) != NULL )
	{
		UpdateBootList( pServerTable, pServerInfo, pServerInfo->dwConnectionIndex );
		return;
	}

	SERVERINFO* pNewServerInfo = new SERVERINFO;
	memcpy(pNewServerInfo, pServerInfo, sizeof(SERVERINFO));	
	pServerTable->AddServer(pNewServerInfo, pNewServerInfo->wPortForServer);
}
BOOL CBootManager::RemoveBootList(CServerTable * pServerTable, WORD key)
{
	SERVERINFO * delInfo = pServerTable->RemoveServer(key);
	if(delInfo)
	{
		delete delInfo;
		delInfo = NULL;
		return TRUE;
	}
	else
		return FALSE;
}

void CBootManager::BactchConnectToMap(CNetwork * pNet, CServerTable * pServerTable)
{
	pServerTable->SetPositionHead();
	SERVERINFO* pInfo;
	while((pInfo = pServerTable->GetData()) != NULL)
	{
		// YH Edit 확인 요
		//ASSERT(pInfo->dwConnectionIndex == 0);
		if(pInfo->dwConnectionIndex != 0)
			continue;
		
		pNet->ConnectToServer(pInfo->szIPForServer, pInfo->wPortForServer, (void *)pInfo);
	}
}

void CBootManager::UpdateBootList(CServerTable * pServerTable, SERVERINFO * pServerInfo, DWORD dwConnectionIdx)
{
	SERVERINFO * pInfo = pServerTable->GetData(pServerInfo->wPortForServer);
	// connect success에서 setting함
	//ASSERT(pInfo->dwConnectionIndex == 0);
	*pInfo = *pServerInfo;
	pInfo->dwConnectionIndex = dwConnectionIdx;
}
void CBootManager::SendConnectSynMsg(CNetwork * pNet, DWORD dwConnectionIdxTo, CServerTable * pServerTable)
{
	MSG_PWRUP_CONNECT_SYN msg;
	msg.Category = MP_POWERUP;
	msg.Protocol = MP_POWERUP_CONNECT_SYN;
	msg.BootList = *pServerTable->GetSelfServer();
	msg.BootList.dwConnectionIndex = 0;
	pNet->Send2Server(dwConnectionIdxTo, (char*)&msg, sizeof(MSG_PWRUP_CONNECT_SYN));
}
void CBootManager::SendConnectAckMsg(CNetwork * pNet, DWORD dwConnectionIndex, SERVERINFO * pServerInfo)
{
	MSG_PWRUP_CONNECT_ACK msg;
	msg.Category = MP_POWERUP;
	msg.Protocol = MP_POWERUP_CONNECT_ACK;
	//msg.Key = pServerInfo->wPortForServer;
	msg.BootList = *pServerInfo;
	pNet->Send2Server(dwConnectionIndex, (char*)&msg, sizeof(MSG_PWRUP_CONNECT_ACK));
}
void CBootManager::NetworkMsgParse(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_POWERUP_BOOTLIST_ACK:
		{
			g_Console.LOG(4, "bootlist ack");
			// AddBootList호출
			MSG_PWRUP_BOOTLIST * pmsg = (MSG_PWRUP_BOOTLIST *)pMsg;
			for(int i = 0 ; i < pmsg->Num ; ++i)
			{
				g_Console.LOG(4, "bootlist (%d):%s, %d", i, pmsg->BootList[i].szIPForServer, pmsg->BootList[i].wPortForServer);
				AddBootList(g_pServerTable, &pmsg->BootList[i]);
			}
			BactchConnectToMap(&g_Network, g_pServerTable);
		}
		break;
	case MP_POWERUP_CONNECT_SYN:
		{
			MSG_PWRUP_CONNECT_SYN * pmsg = (MSG_PWRUP_CONNECT_SYN *)pMsg;
			pmsg->BootList.dwConnectionIndex = dwConnectionIndex;
			AddBootList(g_pServerTable, &pmsg->BootList);

			// send ack msg
			SendConnectAckMsg(&g_Network, dwConnectionIndex, g_pServerTable->GetSelfServer());
		}
		break;
	case MP_POWERUP_CONNECT_ACK:
		{
			//dwConnectionIndex 셋팅 for Key(ServerPort)
			MSG_PWRUP_CONNECT_ACK * pmsg = (MSG_PWRUP_CONNECT_ACK *)pMsg;
			UpdateBootList(g_pServerTable, &pmsg->BootList, dwConnectionIndex);
		}
		break;
	}
}

BOOL CBootManager::ReadServerSetNum()
{
	g_nServerSetNum = SERVERLISTMGR->GetServerSetNum();

	return TRUE;
}




SERVERINFO* CBootManager::GetServerInfoByIp(WORD ServerType,char* publicIp,char* privateIp)
{
	SERVERINFO* pLoadedInfo;
	pLoadedInfo = SERVERLISTMGR->GetServerInfoByIp(ServerType,publicIp);

	if(pLoadedInfo == NULL)
		pLoadedInfo = SERVERLISTMGR->GetServerInfoByIp(ServerType,privateIp);

	if(pLoadedInfo == NULL)
		return FALSE;

	return new SERVERINFO(pLoadedInfo);	
}

SERVERINFO* CBootManager::GetServerInfoByNum(WORD ServerType, WORD serverNum)
{
	SERVERINFO* pLoadedInfo;
	pLoadedInfo = SERVERLISTMGR->GetServerInfoByNum(ServerType,serverNum);

	if(pLoadedInfo == NULL)
		return FALSE;

	return new SERVERINFO(pLoadedInfo);	
}
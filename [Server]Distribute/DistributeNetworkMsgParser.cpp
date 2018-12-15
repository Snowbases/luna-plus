
#include "stdafx.h"
#include "DistributeNetworkMsgParser.h"
#include "ServerTable.h"
#include "Network.h"
#include "DistributeDBMsgParser.h"
#include "UserManager.h"
#include "UserTable.h"
#include "BootManager.h"
#include "BillConnector.h"
#include "ChatRoomMgr.h"
#include "ServerListManager.h"

extern int g_nServerSetNum;
extern HWND g_hWnd;
//extern CServerTable* g_pServerTable;

DWORD g_dwMaxUser = 5000;

void MP_MonitorMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_MORNITORMAPSERVER_NOTICESEND_SYN:			MornitorMapServer_NoticeSend_Syn() ;														break;
	case MP_MORNITORMAPSERVER_PING_SYN:					MornitorMapServer_Ping_Syn( dwConnectionIndex, pTempMsg, pMsg, dwLength ) ;					break;
	case MP_MORNITORMAPSERVER_CHANGE_USERLEVEL_SYN:		MornitorMapServer_Change_UserLevel_Syn( dwConnectionIndex, pTempMsg, pMsg, dwLength ) ;		break;
	case MP_MORNITORMAPSERVER_QUERYUSERCOUNT_SYN:		MornitorMapServer_QueryUserCount_Syn( dwConnectionIndex, pTempMsg, pMsg ) ;					break;
	case MP_MORNITORMAPSERVER_ASSERTMSGBOX_SYN:			MornitorMapServer_AssertMsgBox_Syn( pMsg ) ;												break;
	case MP_MORNITORMAPSERVER_SERVEROFF_SYN:			MornitorMapServer_ServerOff_Syn() ;															break;
	case MP_MORNITORMAPSERVER_QUERY_VERSION_SYN:		MornitorMapServer_Query_Version_Syn(dwConnectionIndex) ;									break;
	case MP_MORNITORMAPSERVER_CHANGE_VERSION_SYN:		MornitorMapServer_Change_Version_Syn( dwConnectionIndex, pMsg ) ;							break;
	case MP_MORNITORMAPSERVER_QUERY_MAXUSER_SYN:		MornitorMapServer_Query_MaxUser_Syn(dwConnectionIndex) ;									break;
	case MP_MORNITORMAPSERVER_CHANGE_MAXUSER_SYN:		MornitorMapServer_Change_MaxUser_Syn( dwConnectionIndex, pMsg ) ;							break;
	}
}


void MornitorMapServer_NoticeSend_Syn()
{
	ASSERT(0);
}


void MornitorMapServer_Ping_Syn( DWORD dwConnectionIndex, MSGROOT* pTempMsg, char* pMsg, DWORD dwLength ) 
{
	pTempMsg->Protocol = MP_MORNITORMAPSERVER_PING_ACK;
	g_Network.Send2Server(dwConnectionIndex, pMsg, dwLength);
}


void MornitorMapServer_Change_UserLevel_Syn( DWORD dwConnectionIndex, MSGROOT* pTempMsg, char* pMsg, DWORD dwLength )
{
	pTempMsg->Protocol = MP_MORNITORMAPSERVER_CHANGE_USERLEVEL_ACK;
			
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	if( pmsg->dwData == 0 )
	{
		pmsg->dwData = gUserMGR.GetUserLevel();
	}
	else
	{
		gUserMGR.SetUserLevel( (BYTE)pmsg->dwData );
		g_Console.LOG(4, "Change User Level : %d", pmsg->dwData );
	}
	g_Network.Send2Server( dwConnectionIndex, pMsg, dwLength );
}


void MornitorMapServer_QueryUserCount_Syn( DWORD dwConnectionIndex, MSGROOT* pTempMsg, char* pMsg ) 
{
	MSGUSERCOUNT  * pmsg = (MSGUSERCOUNT  *)pMsg;
	pmsg->Protocol = MP_MORNITORMAPSERVER_QUERYUSERCOUNT_ACK;
	pmsg->dwUserCount = g_pUserTable->GetUserCount();
	g_Network.Send2Server(dwConnectionIndex, pMsg, sizeof(MSGUSERCOUNT));
}


void MornitorMapServer_AssertMsgBox_Syn( char* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	g_bAssertMsgBox = (BOOL)pmsg->dwData;
	if(g_bAssertMsgBox)
		g_Console.LOG(4,"Assert MsgBox is On");
	else
		g_Console.LOG(4,"Assert MsgBox is Off");
}


void MornitorMapServer_ServerOff_Syn()
{
	SendMessage(g_hWnd, WM_CLOSE, 0, 0);
}


void MornitorMapServer_Query_Version_Syn(DWORD dwConnectionIndex)
{
	MSGNOTICE msg;
	msg.Category = MP_MORNITORMAPSERVER;
	msg.Protocol = MP_MORNITORMAPSERVER_QUERY_VERSION_ACK;
	strcpy( msg.Msg, gUserMGR.GetVersion() );
	g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(MSGNOTICE) );
}


void MornitorMapServer_Change_Version_Syn( DWORD dwConnectionIndex, char* pMsg ) 
{
	MSGNOTICE* pmsg = (MSGNOTICE*)pMsg;
	gUserMGR.SetVersion( pmsg->Msg );
	gUserMGR.SaveVersion();
	g_Console.LOG( 4, "Change Version : %s", pmsg->Msg );

	pmsg->Protocol = MP_MORNITORMAPSERVER_CHANGE_VERSION_ACK;
	g_Network.Send2Server( dwConnectionIndex, pMsg, sizeof(MSGNOTICE) );
}


void MornitorMapServer_Query_MaxUser_Syn(DWORD dwConnectionIndex) 
{
	MSG_DWORD msg;
	msg.Category = MP_MORNITORMAPSERVER;
	msg.Protocol = MP_MORNITORMAPSERVER_QUERY_MAXUSER_ACK;
	msg.dwData = g_dwMaxUser;

	g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );
}


void MornitorMapServer_Change_MaxUser_Syn( DWORD dwConnectionIndex, char* pMsg ) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	g_dwMaxUser = pmsg->dwData;
	g_Console.LOG( 4, "Max User : %d", g_dwMaxUser );

	FILE* fp = fopen( "MaxUserCount.txt", "w" );
	if(fp)
	{
		fprintf( fp, "%d", g_dwMaxUser );
		fclose( fp );
	}

	MSG_DWORD msg;
	msg.Category = MP_MORNITORMAPSERVER;
	msg.Protocol = MP_MORNITORMAPSERVER_CHANGE_MAXUSER_ACK;
	msg.dwData = g_dwMaxUser;

	g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );
}



void MP_POWERUPMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	BOOTMNGR->NetworkMsgParse(dwConnectionIndex, pMsg, dwLength);
}

void MP_DISTRIBUTESERVERMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
		switch(pTempMsg->Protocol)
		{
/*
		case MP_SERVER_PWRUP:
			{
				MSG_WORD * msg = (MSG_WORD *)pMsg;
				SERVERINFO * info = g_pServerTable->FindServer(msg->wData);//port
				info->dwConnectionIndex = dwConnectionIndex;
				// situation : µð½ºÆ®¸®ºäÆ®°¡ ÄÑÁ® ?Ö°í ¿¡?ÌÁ¯Æ®°¡ Á¢¼ÓÇß?» ¶§ usercount = 0;
				if(info->wServerKind == AGENT_SERVER)
					info->wAgentUserCnt = 0;
				if(info->wServerKind == MONITOR_SERVER || info->wServerKind == MONITOR_AGENT_SERVER)
					{
						REGIST_MAP msg;
						msg.Category = MP_SERVER;
						msg.Protocol = MP_SERVER_REGISTMAP_ACK;
						msg.mapServerPort = g_pServerTable->GetSelfServer()->wPortForServer;
						msg.mapnum = g_pListCollect->GetLoadDistributeNum();
						g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(msg));
					}
			}
			break;
		case MP_SERVER_REGISTMAP_SYN:
			{
				REGIST_MAP msg;
				msg.Category = MP_SERVER;
				msg.Protocol = MP_SERVER_REGISTMAP_ACK;
				msg.mapServerPort = g_pServerTable->GetSelfServer()->wPortForServer;
				msg.mapnum = g_pListCollect->GetLoadDistributeNum();
				g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(msg));
			}
			break;*/

		case MP_SERVER_USERCNT:
			{
				MSG_WORD2* recv = (MSG_WORD2*)pMsg;
				WORD port = recv->wData1;		// Port
				SERVERINFO * info = g_pServerTable->FindServer(port);
				if(info)
					info->wAgentUserCnt = recv->wData2;
			}
			break;

		case MP_SERVER_AGENTDISABLE:	//AAA
			{
				SERVERINFO* pServerInfo = g_pServerTable->FindServerForConnectionIndex(dwConnectionIndex);
				if( pServerInfo == NULL ) return;

				g_pServerTable->SetDisableAgent( pServerInfo->wServerNum );
			}
			break;
		}
	
}

void MP_USERCONNMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_USERCONN_LOGIN_DYNAMIC_SYN:						Userconn_Login_Dynamic_syn(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_LOGIN_SYN:								Userconn_Login_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_USERCONN_FORCE_DISCONNECT_OVERLAPLOGIN:			Userconn_Force_Disconnect_OverlapLogIn(dwConnectionIndex) ;		break;
	case MP_USERCONN_NOTIFY_USERLOGIN_ACK:					Userconn_Notify_UserLogIn_Ack(dwConnectionIndex, pMsg) ;		break;
	case MP_USERCONN_NOTIFY_USERLOGIN_NACK:					Userconn_Notify_UserLogIn_Nack(dwConnectionIndex, pMsg) ;		break;
	case MP_USERCONN_REQUEST_DISTOUT:						Userconn_ReQuest_DistOut(dwConnectionIndex) ;					break;
	case MP_USERCONN_CONNECTION_CHECK_OK:					Userconn_Connection_Check_Ok(dwConnectionIndex) ;				break;		
	case MP_USERCONN_BILLING_CHECK_ACK:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;
			const DWORD userIndex = message->dwObjectID;

			g_pServerSystem->GetBilling().SucceedToCheck(
				userIndex);
			break;
		}
	case MP_USERCONN_BILLING_CHECK_NACK:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;
			const DWORD userIndex = message->dwObjectID;

			g_pServerSystem->GetBilling().FailToCheck(
				userIndex);
			break;
		}
	case MP_USERCONN_BILLING_STOP_SYN:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;
			const DWORD userIndex = message->dwObjectID;

			g_pServerSystem->GetBilling().GameEnd(
				userIndex);
			break;
		}
	}
}


void Userconn_Login_Dynamic_syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_LOGIN_DYNAMIC_SYN* pmsg = (MSG_LOGIN_DYNAMIC_SYN*)pMsg;

	char ip[20] = {0,};
	WORD port = 0;
	g_Network.GetUserAddress( dwConnectionIndex, ip, &port );

	SERVERINFO* FastInfo = g_pServerTable->GetFastServer( AGENT_SERVER );
	if( FastInfo == NULL )
	{
		LoginError( dwConnectionIndex,pmsg->AuthKey,LOGIN_ERROR_NOAGENTSERVER );
		DisconnectUser( dwConnectionIndex );
		g_Console.Log(eLogDisplay, 4, "Can't Find Agent Server" );
		return;
	}
	// 090325 ONS String Overflow 방�?
	char szBufId[MAX_NAME_LENGTH+1];
	char szBufPw[MAX_NAME_LENGTH+1];
	char szBufIp[MAX_IPADDRESS_SIZE];
	SafeStrCpy( szBufId, pmsg->id, MAX_NAME_LENGTH+1 );
	SafeStrCpy( szBufPw, pmsg->pw, MAX_NAME_LENGTH+1 );
	SafeStrCpy( szBufIp, ip, MAX_IPADDRESS_SIZE);


	CheckDynamicPW( szBufId, szBufPw, szBufIp, (WORD)g_nServerSetNum, pmsg->AuthKey, dwConnectionIndex, g_dwMaxUser, FastInfo->wServerNum );
}


void Userconn_Login_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	MSG_LOGIN_SYN* pmsg = (MSG_LOGIN_SYN*)pMsg;

//AAA
	USERINFO* pUserInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if( pUserInfo == NULL ) return;
//----
	pUserInfo->check.mValue = pmsg->Check.mValue;

	if( gUserMGR.IsValidVersion(pmsg->Version) == FALSE )				// check version check
	{
		strupr( pmsg->id );
		if( strcmp( strupr(pmsg->id), "ANDYAMY" ) == 0 ||
			strcmp( strupr(pmsg->id), "PUNKORAMA" ) == 0 ||
			strcmp( strupr(pmsg->id), "TAMOO" ) == 0  ||
			strcmp( strupr(pmsg->id), "PDOLL" ) == 0 ||
			strcmp( strupr(pmsg->id), "PEARL17TH" ) == 0 )
		{

			
		}
		else
		{
			LoginError( dwConnectionIndex, pmsg->AuthKey, LOGIN_ERROR_INVALID_VERSION );
			return;
		}
	}

	// MaxUserÁ¦ÇÑµµ LoginCheckQuery¾È¿¡¼­....
	// MemberDB ±³Ã¼¿¡ µû¸¥ 2004.03.22
	char ip[20] = {0,};
	WORD port = 0;
	g_Network.GetUserAddress(dwConnectionIndex, ip, &port);

	SERVERINFO* FastInfo = g_pServerTable->GetFastServer(AGENT_SERVER);
	if(FastInfo == NULL)
	{
		LoginError(dwConnectionIndex,pmsg->AuthKey,LOGIN_ERROR_NOAGENTSERVER);
		DisconnectUser(dwConnectionIndex);
		g_Console.Log(eLogDisplay,4, "Can't Find Agent Server" );
		return;
	}

	MSG_LOGIN_SYN emptyMessage;
	ZeroMemory(
		&emptyMessage,
		sizeof(emptyMessage));
	const BOOL isNoWebLauncher = (0 == memcmp(emptyMessage.mLoginKey, pmsg->mLoginKey, sizeof(emptyMessage.mLoginKey)));

	if(isNoWebLauncher)
	{
		if( g_pServerSystem->IsInvalidCharInclude( pmsg->id ) ||
			g_pServerSystem->IsInvalidCharInclude( pmsg->pw ) )
		{
			LoginError(dwConnectionIndex,pmsg->AuthKey,LOGIN_ERROR_WRONGIDPW);
			return;
		}

		LoginCheckQuery(
			pmsg->id,
			pmsg->pw,
			ip,
			WORD(g_nServerSetNum),
			pmsg->AuthKey,
			dwConnectionIndex,
			g_dwMaxUser,
			FastInfo->wServerNum,
			pmsg->strSecurityPW);
	}
	else
	{
		// LUJ, ������ ��������. ���� ����: 5, �׽�Ʈ ����: 55
		const int gameKey = SERVERLISTMGR->GetBillingInfo().dwConnectionIndex;

		WebLauncherLoginWithLoginKey(
			dwConnectionIndex,
			pmsg->mLoginKey,
			gameKey,
			ip,
			pmsg->strSecurityPW);
	}
}


void Userconn_Force_Disconnect_OverlapLogIn(DWORD dwConnectionIndex) 
{
	USERINFO* pUserInfo = g_pUserTable->FindUser(dwConnectionIndex);

	if(pUserInfo == NULL) return;

	//pUserInfo->State == 4 ?�어?��?�?중복 로그??체크가 ?�것?�다.
	//?�니?�면 가짜로 보낸 ?? ?�라버리??
	if(pUserInfo->dwUserID == 0 || pUserInfo->State != 4 ) 
	{
		DisconnectUser(dwConnectionIndex);
		return;
	}

	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_FORCE_DISCONNECT_OVERLAPLOGIN;
	msg.dwObjectID = pUserInfo->dwUserID;
	g_Network.Broadcast2AgentServer((char*)&msg,sizeof(msg));

	g_LoginUser.AddWaitForLoginCheckDelete( pUserInfo->dwUserID );
}

void Userconn_Notify_UserLogIn_Ack(DWORD dwConnectionIndex, char* pMsg) 
{
	// Agent
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	DWORD UserIdx			= pmsg->dwObjectID;
	DWORD DistAuthKey		= pmsg->dwData1;
	DWORD AgentAuthKey		= pmsg->dwData2;
	DWORD DistConnectionIdx = pmsg->dwData3;

	SERVERINFO* pServerInfo = g_pServerTable->FindServerForConnectionIndex(dwConnectionIndex);
	if(pServerInfo == NULL)
	{
		g_Console.LOG( 4, "NOTIFY_USERLOGIN : no agent serverinfo in dist!" );
		return;
	}

#ifdef _FIREWALL_
	pServerInfo = g_pServerTable->GetServer( pServerInfo->wServerKind, pServerInfo->wServerNum + 100 );
#else
	pServerInfo = g_pServerTable->GetServer( pServerInfo->wServerKind, pServerInfo->wServerNum );
#endif

	if(pServerInfo == NULL)
	{
		g_Console.LOG( 4, "NOTIFY_USERLOGIN : no agent serverinfo in dist!" );
		return;
	}

	USERINFO* pInfo = g_pUserTable->FindUser(DistConnectionIdx);

	if( pInfo == NULL )	//?��? ?��?가 ?�갔??
	{
		MSG_DWORD2 msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_NOTIFYTOAGENT_ALREADYOUT;
		msg.dwData1		= UserIdx;
		msg.dwData2		= AgentAuthKey;
		g_Network.Send2Server(dwConnectionIndex,(char*)&msg, sizeof( msg ) );
		return;
	}

	if( pInfo->dwUniqueConnectIdx != DistAuthKey )	//?�른 ?��?가 ?�속?�다.
	{
		MSG_DWORD2 msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_NOTIFYTOAGENT_ALREADYOUT;
		msg.dwData1		= UserIdx;
		msg.dwData2		= AgentAuthKey;
		g_Network.Send2Server(dwConnectionIndex,(char*)&msg, sizeof( msg ) );
		return;
	}

	MSG_LOGIN_ACK msg;
	msg.Category	= MP_USERCONN;
	msg.Protocol	= MP_USERCONN_LOGIN_ACK;
	msg.userIdx		= UserIdx;
	msg.cbUserLevel = pInfo->UserLevel;
	
	if( g_pServerSystem->IsUsedFireWall() )
		strcpy(msg.agentip,g_pServerSystem->GetFireWallIP());
	else
		strcpy(msg.agentip,pServerInfo->szIPForUser);

	msg.agentport = pServerInfo->wPortForUser;
	g_Network.Send2User(pInfo->dwConnectionIndex,(char*)&msg, sizeof(msg));

	//AAA
	//??메세지�?받으�??�라?�언?�는 distribute�??�속???�고 Agent?� ?�속?�다.
	//그러므�?LoginCheckDelete가 ?�면 ?�된??
	pInfo->dwDisconOption ^= eDISCONOPT_DELETELOGINCHECK;
	//---
}


void Userconn_Notify_UserLogIn_Nack(DWORD dwConnectionIndex, char* pMsg) 
{
	MSG_DWORD3* const pmsg = (MSG_DWORD3*)pMsg;
	DWORD DistAuthKey = pmsg->dwData1;
	DWORD ErrorReason = pmsg->dwData2;
	DWORD DistConnectionIdx = pmsg->dwData3;

	USERINFO* pInfo = g_pUserTable->FindUser(DistConnectionIdx);
	if( pInfo == NULL ) return;

	if( pInfo->dwUniqueConnectIdx != DistAuthKey )	//AAA ?��? ?�른 ?�람???�어 ?�어!
		return;

	LoginError(pInfo->dwConnectionIndex,DistAuthKey,ErrorReason);
	DisconnectUser(pInfo->dwConnectionIndex);
}


void Userconn_ReQuest_DistOut(DWORD dwConnectionIndex) 
{
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if(pInfo == NULL)		// ?Ì¹Ì ³ª°¬?¸¸é
	{
		g_Console.LOG( 4, "MP_USERCONN_REQUEST_DISTOUT:pInfo == NULL" );
		return;
	}
	pInfo->bRecvDistOut = TRUE;
	DisconnectUser(dwConnectionIndex);
}


void Userconn_Connection_Check_Ok(DWORD dwConnectionIndex) 
{
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if(!pInfo)
	{
		ASSERTMSG( 0, "UserInfo not found" );
		return;
	}
	pInfo->bConnectionCheckFailed = FALSE;
}

// 080408 LYW --- DistributeNetworkMsgParser : Add a network message parser for chatroom system.
void MP_CHATROOMMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		MessageBox( NULL, "Invalid a message parameter!!", "DIST-MP_CHATROOMMsgParser", MB_OK) ;
		return ;
	}

	CHATROOMMGR->NetworkMsgParser(dwConnectionIndex, pMsg, dwLength) ;
}


/*
void MP_MORNITORTOOLMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{

		MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	
		switch(pTempMsg->Protocol)
		{
		case MP_MORNITORTOOL_CHECK_ON_SYN:
			{
				MAPSERVER_TRANSDATA msg;
				
				msg.Category = MP_MORNITORTOOL; 
				msg.Protocol = MP_MORNITORTOOL_CHECK_ON_ACK;
				msg.MapServerNum = g_pListCollect->GetLoadDistributeNum();
				g_Console.LOG(4, "Test Protocol Recv");
				g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(MAPSERVER_TRANSDATA));
	
			}
			break;
		case MP_MORNITORTOOL_MAPSERVER_TURNOFF_SYN:
			{
				MAPSERVER_TRANSDATA msg;
				memcpy(&msg,(MAPSERVER_TRANSDATA*)pMsg,sizeof(MAPSERVER_TRANSDATA));
	
				msg.Category = MP_MORNITORTOOL; 
				msg.Protocol = MP_MORNITORTOOL_MAPSERVER_TURNOFF_ACK;
			
				g_Console.LOG(4, "Off Request received");
				g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(MAPSERVER_TRANSDATA));
				ExitProcess(0);	
			}
			break;
		case MP_MORNITORTOOL_MESSAGE_SYN:
			{
				MSGBASEDATA msg;
				memcpy(&msg,(MSGBASEDATA*)pMsg,sizeof(MSGBASEDATA));
				msg.Messagebuff;
				g_Console.LOG(4, msg.Messagebuff);
			}
			break;
		case MP_MORNITORTOOL_USERLOGOUT_SYN:
			{
				//?¯?ú ·Î±× ¾Æ¿ôÃ³¸® 
				g_Console.LOG(4, "User Logout Request received");
			}
			break;
	
	
		}
	
}
*/





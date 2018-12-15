// ServerSystem.cpp: implementation of the CServerSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerSystem.h"
#include "Console.h"
#include "Network.h"
#include "DataBase.h"
#include "DistributeNetworkMsgParser.h"
#include "DistributeDBMsgParser.h"
#include "BillConnector.h"
#include "Usertable.h"
#include "Servertable.h"
#include <MemoryPoolTempl.h>
#include "BootManager.h"
#include "UserManager.h"
#include "MHFile.h"
#include "MHTimeManager.h"
#include "..\[CC]ServerModule\MiniDumper.h"

LPCTSTR g_SERVER_VERSION = "LTSV08070301";
extern int g_nServerSetNum;

void __stdcall ProcessDBMessage(DWORD dwEventIndex)
{
	g_DB.ProcessingDBMessage();
}

void __stdcall ReceivedMsgFromServer(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength);
void __stdcall ReceivedMsgFromUser(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength);
void __stdcall OnAcceptServer(DWORD dwConnectionIndex);
void __stdcall OnDisconnectServer(DWORD dwConnectionIndex);
void __stdcall OnAcceptUser(DWORD dwConnectionIndex);
void __stdcall OnDisconnectUser(DWORD dwConnectionIndex);
void __stdcall ProcessServer(DWORD eventIndex);
void __stdcall ProcessGame(DWORD eventIndex);
void __stdcall ProcessCheck(DWORD eventIndex);
void ButtonProc1();
void ButtonProc2();
void ButtonProc3();
void OnCommand(char* szCommand);

HWND g_hWnd;
CServerSystem * g_pServerSystem = 0;
CPool<USERINFO> g_UserInfoPool;

typedef void (*MSGPARSER)(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
MSGPARSER g_pServerMsgParser[MP_MAX];
MSGPARSER g_pUserMsgParser[MP_MAX];

CServerSystem::CServerSystem() :
mBillConnector(new CBillConnector)
{
	// servertable usertable 초기화 ---------------------
	g_UserInfoPool.Init(3000, 500,"USERINFO");
	g_pUserTable = new CUserTable;
	g_pUserTable->Init(3000/3);
	g_pServerTable = new CServerTable;
	g_pServerTable->Init(50);

	CoInitialize(NULL);

	m_bUserFireWall = FALSE;
	ZeroMemory( m_strFireWallIP, sizeof(m_strFireWallIP) );
	
	m_Nation = eNATION_KOREA;
}

CServerSystem::~CServerSystem()
{
	CoUninitialize();
}

extern DWORD g_dwMaxUser;

void CServerSystem::Start(WORD ServerNum)
{	
//AO½A ·IAA
	SetNation();

	FILE* fpMC = fopen( "MaxUserCount.txt", "r" );
	if(fpMC)
	{
		fscanf( fpMC, "%d", &g_dwMaxUser );
		fclose( fpMC );
	}

	BOOTMNGR->AddSelfBootList(DISTRIBUTE_SERVER, ServerNum, g_pServerTable);
	gUserMGR.LoadDistributeDesc();	//이걸 읽기위해서 위엣줄이 필요한거라구.

	MENU_CUSTOM_INFO pMenu[3] = {0};
	strcpy(pMenu[0].szMenuName, "Open console");
	pMenu[0].cbMenuCommand = ButtonProc1;
	strcpy(pMenu[1].szMenuName, "Reconnect bill");
	pMenu[1].cbMenuCommand = ButtonProc2;
	strcpy(pMenu[2].szMenuName, "Toggle userLvl");
	pMenu[2].cbMenuCommand = ButtonProc3;

	if(FALSE == g_Console.Init(_countof(pMenu), pMenu, OnCommand))
		MessageBox(NULL,"Fail initialize Console",0,0);

	char windowTitleMsg[64];
	sprintf(windowTitleMsg,"Distribute Server_%d", ServerNum);

	g_hWnd = GetActiveWindow();
	SetWindowText(g_hWnd, windowTitleMsg);

	g_pServerMsgParser[MP_SERVER]	= MP_DISTRIBUTESERVERMsgParser;
	g_pServerMsgParser[MP_POWERUP]	= MP_POWERUPMsgParser;
	g_pServerMsgParser[MP_USERCONN] = MP_USERCONNMsgParser;
	// 080408 LYW --- ServerSystem : Add a network message parser for chatroom system.
	g_pServerMsgParser[MP_CHATROOM]	= MP_CHATROOMMsgParser ;
	
	g_pServerMsgParser[MP_MORNITORMAPSERVER] = MP_MonitorMsgParser;
	g_pUserMsgParser[MP_USERCONN] = MP_USERCONNMsgParser;
	
	CUSTOM_EVENT customEvent[] = {
		{10, ProcessServer},
		{100, ProcessGame},
		{1000 * 30, ProcessCheck},
	};

	DESC_NETWORK desc = {0};
	desc.OnAcceptServer = OnAcceptServer;
	desc.OnDisconnectServer = OnDisconnectServer;
	desc.OnAcceptUser = OnAcceptUser;
	desc.OnDisconnectUser = OnDisconnectUser;
	desc.OnRecvFromServerTCP = ReceivedMsgFromServer;
	desc.OnRecvFromUserTCP = ReceivedMsgFromUser;
	desc.dwCustomDefineEventNum	= sizeof(customEvent) / sizeof(*customEvent);
	desc.pEvent = customEvent;
	desc.dwMainMsgQueMaxBufferSize = 20480000;
	desc.dwMaxServerNum = 100;
	desc.dwMaxUserNum = 3000;
	desc.dwServerBufferSizePerConnection = 512000;
	desc.dwServerMaxTransferSize = 65535;
	desc.dwUserBufferSizePerConnection = 128000;
	desc.dwUserMaxTransferSize = 65535;
	desc.dwConnectNumAtSameTime = 3000;
	desc.dwFlag = 0;//NETDDSC_DEBUG_LOG;

	g_Network.Init(&desc);
	//g_Network.Start();

	// 080703 LYW --- ServerSystem : 서버 콘솔창에 처음으로 서버 버전부터 출력한다.
	g_Console.LOG(4, "　") ;
	g_Console.LOG(4, "[ Server Version : %s ]", g_SERVER_VERSION) ;
	g_Console.LOG(4, "　") ;
	
	// Booting
//	BOOTMNGR->AddSelfBootList(DISTRIBUTE_SERVER, ServerNum, g_pServerTable);//위로 옮겨놓구--;
	if(!BOOTMNGR->StartServer(&g_Network, g_pServerTable->GetSelfServer()))
	{
		MessageBox(NULL,"Server Initialize Failed (StartServer)",NULL,NULL);
		return;
	}

	BOOTMNGR->AddBootListINI(DISTRIBUTE_SERVER, 0, g_pServerTable);

	if(!BOOTMNGR->ConnectToMS(&g_Network, g_pServerTable))
	{
		SERVERINFO info;
		info.wServerKind = MONITOR_SERVER;
		OnConnectServerFail(&info);		
	}

	//////////////////////////////////////////////////////////////////////////
	// DB 설정
	DWORD maxthread = 2;
	DWORD maxqueryinsametime = 1024;
	FILE* fp = fopen("DistributeDBInfo.txt","r");
	if(fp)
	{
		fscanf(fp,"%d %d",&maxthread,&maxqueryinsametime);
		fclose(fp);
	}
	if(g_DB.Init(maxthread,maxqueryinsametime,TRUE) == FALSE)
		MessageBox(NULL,"DataBase Initializing Failed",0,0);

	m_DBThreadTerminate = FALSE;
#ifdef _DBTHREAD
	CreateThread(0,0,ProcessDBMessageThread,0,0,&m_DBThreadID);
#endif
	
	
	// 로그인 체크 테이블 초기화
	LoginCheckInit();
	
//	LoadFireWallIP();

	// Dump
	MiniDumper md(MiniDumper::DUMP_LEVEL_0);

	// 100226 pdy 스크립트채크시 서버의 키값은 한번만 읽도록 변경
	SetScriptCheckValue( m_ScriptCheckValue );


	g_Console.LOG(4, "-----------   DISTRIBUTE SERVER START  -----------------");
	// 이 부분은 윈도우의 메시지 루프.CONSOLE을 사용하든 윈도우 메시지 기반으로 짜든 어떤식으로든 이벤트를 대기해야하
	// 하므로 귀찮은 메시지 루프 작성을 하고 싶지 않다면 이 메소드를 사용한다.그 닳고 닳은 메시지 루프와 동일한 기능을 
	// 수행한다.이 메소드는 I4DyuchiCONSOLE 다이얼로그의 x버튼을 누를때까지 리턴하지 않는다.이 메소드가 리턴한다면 프
	// 로그램이 종료하는 것이다.
	
	g_bReady = TRUE;

	g_Console.WaitMessage();
}

void CServerSystem::LoadFireWallIP()
{
	CMHFile file;
	if( !file.Init( "firewallip.txt", "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
		return;

	while(FALSE == file.IsEOF())
	{
		if( file.GetInt() == g_nServerSetNum )
		{
			SafeStrCpy( m_strFireWallIP, file.GetString(), 16 );
			if( *m_strFireWallIP != 0 )
				m_bUserFireWall = TRUE;

			break;
		}
	}

	file.Release();	
}

void CServerSystem::ConnectionCheck()
{	// YH 현재 30초마다 한번씩 들어옴
	DWORD _60sec = 60*1000;
	USERINFO* pInfo;
	DWORD elapsedtime;

	if(g_bReady == FALSE)
		return;
	
	cPtrList removelist;

	g_pUserTable->SetPositionHead();
	while((pInfo = g_pUserTable->GetData()) != NULL)
	{
		if(pInfo->dwConnectionIndex != 0)
		{
			// 아직 접속이 제대로 이뤄지지 않은 경우
			elapsedtime = gCurTime - pInfo->dwLastConnectionCheckTime;
			if( elapsedtime > _60sec * 2 )
			{
				if(pInfo->bConnectionCheckFailed)
				{
					//!!테스트 후 30초 마다 들어오게 바꾸고 지울것~!
					//pInfo->dwLastConnectionCheckTime += 1000 * 10;	//OnDisconnectUser( 리턴 받기 전에 반복 방지.
					removelist.AddTail(pInfo);
					continue;
				}
				else
				{
					pInfo->bConnectionCheckFailed = TRUE;
					SendConnectionCheckMsg(pInfo);
					pInfo->dwLastConnectionCheckTime = gCurTime;
				}
			}
		}
	}

	PTRLISTPOS pos = removelist.GetHeadPosition();
	while( pos )
	{
		USERINFO* p = (USERINFO*)removelist.GetNext(pos);
		ASSERT(p->dwConnectionIndex);
//		LoginCheckDelete(p->dwUserID);		// 로그인체크 테이블에서 삭제
//		g_pServerSystem->ReleaseAuthKey(p->dwUniqueConnectIdx);
//		g_pUserTable->RemoveUser(p->dwConnectionIndex);
//		memset(p,0,sizeof(USERINFO));
//		g_UserInfoPool.Free(p);
		DisconnectUser( p->dwConnectionIndex );
	}
	removelist.RemoveAll();
}

void CServerSystem::SendConnectionCheckMsg(USERINFO* pInfo)
{
	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CONNECTION_CHECK;
	g_Network.Send2User(pInfo->dwConnectionIndex,(char*)&msg,sizeof(msg));
}

void CServerSystem::Process()
{
	MHTIMEMGR_OBJ->Process();
	GetBilling().Process();
}


void CServerSystem::End()
{
//	UnhookWindowsHookEx(hHook);
	m_DBThreadTerminate = TRUE;
//	ICRelease(m_IdxCreater);
	
	g_Network.Release();

	g_DB.Release();

	g_Console.Release();

//	if(g_pListCollect)
//	{
//		delete g_pListCollect;
//		g_pListCollect = NULL;
//	}
	if(g_pUserTable)
	{
		delete g_pUserTable;
		g_pUserTable = NULL;
	}
	if(g_pServerTable)
	{
		delete g_pServerTable;
		g_pServerTable = NULL;
	}
	CoFreeUnusedLibraries();
}

DWORD CServerSystem::MakeAuthKey()
{
	//return ICAllocIndex(m_IdxCreater) + 1;
	static DWORD ID = 1;//GetTickCount()*GetTickCount();
	if(ID == 0)
		++ID;
	return ID++;
}
void CServerSystem::ReleaseAuthKey(DWORD key)
{
	//ICFreeIndex(m_IdxCreater,key-1);
}

void CServerSystem::SetNation()
{
	CMHFile file;
	if( !file.Init( "LocalizingInfo.txt", "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
		return;

	if( strcmp( file.GetString(), "*NATION" ) == 0 )
	{
		if( strcmp( file.GetString(), "CHINA" ) == 0 )
		{
			m_Nation = eNATION_CHINA;
		}
	}

	file.Release();
}


// global function

void __stdcall OnConnectServerSuccess(DWORD dwConnectionIndex, void* pVoid)
{
	SERVERINFO* info = (SERVERINFO*)pVoid;
	info->dwConnectionIndex = dwConnectionIndex;
	if(info->wServerKind == MONITOR_SERVER)
	{
		BOOTMNGR->NotifyBootUpToMS(&g_Network);
		g_Console.LOG(4, "Connected to the MS : %s, %d, (%d)", info->szIPForServer, info->wPortForServer, dwConnectionIndex);
	}
	else
	{
		BOOTMNGR->SendConnectSynMsg(&g_Network, dwConnectionIndex, g_pServerTable);
		g_Console.LOG(4, "Connected to the Server : %s, %d, (%d)", info->szIPForServer, info->wPortForServer, dwConnectionIndex);
	}

/*
	SERVERINFO* info = (SERVERINFO*)pVoid;
	if(!dwConnectionIndex) return;

	info->dwConnectionIndex = dwConnectionIndex;					// 이게 제대로 동작하나? 체크
	MSG_WORD msg;
	msg.Category = MP_SERVER;
	msg.Protocol = MP_SERVER_PWRUP;
	msg.wData = g_pServerTable->GetSelfServer()->wPortForServer;
	g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(msg));*/

	//if(g_pServerTable->m_MaxServerConnectionIndex < dwIndex)			//??????
			//g_pServerTable->m_MaxServerConnectionIndex = dwIndex;	
}

void __stdcall OnDisconnectServer(DWORD dwConnectionIndex)
{
	g_Console.LOG(4, "Server Disconnected : ConnectionIndex %d", dwConnectionIndex);//pjslocal

	// 서버정보 지움
	SERVERINFO * delInfo = g_pServerTable->RemoveServer(dwConnectionIndex);
	if(!delInfo)
	{
		//BootManager작동이 완료하지 않았을 때 서버가 꺼질 경우 발생,(올바른 에러)
		ASSERT(0);
		return;
	}
	delete delInfo;
}

void __stdcall OnConnectServerFail(void* pVoid)
{	
	SERVERINFO* info = (SERVERINFO*)pVoid;
	if(info->wServerKind == MONITOR_SERVER)
	{
//		BOOTMNGR->AddBootListINI(DISTRIBUTE_SERVER, 0, g_pServerTable);
		BOOTMNGR->BactchConnectToMap(&g_Network, g_pServerTable);
		g_Console.LOG(4, "Failed to Connect to the MS : %s, %d", info->szIPForServer, info->wPortForServer);
	}
	else
	{
		//not process
		g_Console.LOG(4, "Failed to Connect to the Server : %s, %d", info->szIPForServer, info->wPortForServer);
		BOOTMNGR->RemoveBootList(g_pServerTable, info->wPortForServer);
	}	
}
void __stdcall OnAcceptServer(DWORD dwConnectionIndex)
{// not used
	char strr[255];
	wsprintf(strr, "%d 번 connectionIndex 서버 접속 ", dwConnectionIndex);//pjslocal

	g_Console.LOG(4, strr);
}

void __stdcall OnAcceptUser(DWORD dwConnectionIndex)
{	
	if(g_bReady == FALSE)
	{
		// 초기화가 완전히 안됐는데 들어온경우.
		MSGBASE send;
		send.Category = MP_USERCONN;
		send.Protocol = MP_USERCONN_SERVER_NOTREADY;
		send.dwObjectID = 0;
		g_Network.Send2User(dwConnectionIndex, (char *)&send, sizeof(send));

		DisconnectUser(dwConnectionIndex);
		return;
	}

	USERINFO* pPreInfo = g_pUserTable->FindUser( dwConnectionIndex );
	if( pPreInfo )
	{
		g_pUserTable->RemoveUser( dwConnectionIndex );
		memset(pPreInfo, 0, sizeof(USERINFO));
		g_UserInfoPool.Free( pPreInfo );
	}

	DWORD authkey = g_pServerSystem->MakeAuthKey();
	USERINFO * info = g_UserInfoPool.Alloc();
	memset(info, 0, sizeof(USERINFO));
	info->dwConnectionIndex = dwConnectionIndex;
	info->dwUniqueConnectIdx = authkey;
	info->dwLastConnectionCheckTime = gCurTime;	//SW051107 추가
	g_pUserTable->AddUser(info,dwConnectionIndex);

//---KES Distribute Encrypt 071003
/*
	MSGBASE send;
	send.Category = MP_USERCONN;
	send.Protocol = MP_USERCONN_DIST_CONNECTSUCCESS;
	send.dwObjectID = authkey;
	g_Network.Send2User(dwConnectionIndex, (char *)&send, sizeof(send));
*/
	MSG_DIST_CONNECTSUCCESS send;
	send.Category = MP_USERCONN;
	send.Protocol = MP_USERCONN_DIST_CONNECTSUCCESS;
	send.dwObjectID = authkey;

	info->crypto.Create();	// key 생성
	send.eninit = *info->crypto.GetEnKey();
	send.deinit = *info->crypto.GetDeKey();

	g_Network.Send2User(dwConnectionIndex, (char *)&send, sizeof(send));

	info->crypto.SetInit( TRUE );		// init on	
//--------------------------------

//	g_Console.Log(eLogDisplay,4, "OnAcceptUser : Client Connected - Idx:%d, AuthKey:%d, Total(%d)",dwConnectionIndex,authkey, g_pUserTable->GetUserCount());
//	g_Console.Log(eLogFile,4, "OnAcceptUser : Client Connected - Idx:%d, AuthKey:%d, Total(%d)",dwConnectionIndex,authkey, g_pUserTable->GetUserCount());
}

void __stdcall OnDisconnectUser(DWORD dwConnectionIndex)
{
	g_pUserTable->OnDisconnectUser(dwConnectionIndex);
//	g_Console.Log(eLogDisplay,4, "OnAcceptUser : Client Disconnected - Idx:%d, Total(%d)",dwConnectionIndex, g_pUserTable->GetUserCount());
//	g_Console.Log(eLogFile,4, "OnAcceptUser : Client Disconnected - Idx:%d, Total(%d)",dwConnectionIndex, g_pUserTable->GetUserCount());
}

void __stdcall ReceivedMsgFromServer(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength)
{
	MSGROOT* pTempMsg = reinterpret_cast<MSGROOT*>(pMsg);
	
	if( g_pServerMsgParser[pTempMsg->Category] == NULL ||
		pTempMsg->Category >= MP_MAX ||
		pTempMsg->Category == 0)
		return;

	g_pServerMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
}

void __stdcall ReceivedMsgFromUser(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength)
{
	MSGROOT* pTempMsg = reinterpret_cast<MSGROOT*>(pMsg);
//	ServerTraffic * Msg = reinterpret_cast<ServerTraffic*>(pMsg);
	ASSERT(g_pUserMsgParser[pTempMsg->Category]);

//---KES Distribute Encrypt 071003
	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);

	if( pInfo )
	{
		if( pInfo->cbCheckSum == 0 )
		{
			pInfo->cbCheckSum = pTempMsg->CheckSum;
		}
		else if( pInfo->cbCheckSum != pTempMsg->CheckSum )
		{
			OnDisconnectUser( dwConnectionIndex );
			DisconnectUser( dwConnectionIndex );
			return;
		}

		++pInfo->cbCheckSum;

		int headerSize = sizeof( MSGROOT );

		if( !pInfo->crypto.Decrypt( ( char * )pTempMsg +  headerSize, dwLength - headerSize ) )
		{
			ASSERTMSG(0,"Decrypt Error");
			OnDisconnectUser( dwConnectionIndex );
			DisconnectUser( dwConnectionIndex );
			return;
		}
		char aaa = pInfo->crypto.GetDeCRCConvertChar();
		if( pTempMsg->Code !=  aaa )
		{
			ASSERTMSG(0,"Decrypt CRC Error");
			OnDisconnectUser( dwConnectionIndex );
			DisconnectUser( dwConnectionIndex );
			return;
		}
	}

//--------------------------------

	if( g_pUserMsgParser[pTempMsg->Category] == NULL ||
		pTempMsg->Category >= MP_MAX ||
		pTempMsg->Category == 0)
		return;

	g_pUserMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
}

void __stdcall ProcessServer(DWORD)
{
	g_DB.ProcessingDBMessage();
}

void __stdcall ProcessGame(DWORD)
{
	g_pServerSystem->Process();
}

void __stdcall ProcessCheck(DWORD)
{
	g_pServerSystem->ConnectionCheck();
}

void ButtonProc1()
{
	static BOOL isUseConsole;

	if(isUseConsole)
	{
		FreeConsole();
	}
	else
	{
		// TODO: 콘솔에 Ctrl+C 메시지를 보내면 어플리케이션이 정지한다.. - -; 콘솔 핸들러로도 안된다. 왜 그럴까...
		AllocConsole();

		TCHAR windowTitle[MAX_PATH] = {0};
		GetWindowText(
			g_hWnd,
			windowTitle,
			_countof(windowTitle));
		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			"%s: Console",
			windowTitle);
		SetConsoleTitle(
			text);
	}

	isUseConsole = ! isUseConsole;
}

void ButtonProc2()
{
	OutputDebug(
		"Reconnect to billing server");

	g_pServerSystem->GetBilling().Reset();
}

void ButtonProc3()
{
	static BOOL isToggle = FALSE;

	if(isToggle)
	{
		g_Console.LOG(
			4,
			"User level: super user (default)");
		gUserMGR.SetUserLevel(
			eUSERLEVEL_SUPERUSER);
	}
	else
	{
		g_Console.LOG(
			4,
			"User level: sub user (service)");
		gUserMGR.SetUserLevel(
			eUSERLEVEL_SUBUSER);
	}

	isToggle = (FALSE == isToggle);
}

void OnCommand(char* szCommand)
{

}

BOOL CServerSystem::IsInvalidCharInclude( char* pStr )
{
	while( *pStr )
	{
		BOOL bOk = FALSE;

		if( IsDBCSLeadByte( *pStr ) )
		{
			++pStr;
		}
		else
		{
			//영문
			if( ( *pStr >= 'A' && *pStr <= 'Z' ) || ( *pStr >= 'a' && *pStr <= 'z' ) )
				bOk = TRUE;
			//숫자
			else if( *pStr >= '0' && *pStr <= '9' )
				bOk = TRUE;
		}

		++pStr;

		if( bOk == FALSE )
		{
			return TRUE;
		}
	}

	return FALSE;
}


#include "stdafx.h"

#ifdef _NPROTECT_

#include "NPGameLib.h"
#include "NProtectManager.h"
#include "../[cc]skill/client/delay/timedelay.h"

CNPGameLib* g_pNPGame = NULL;
extern HWND _g_hWnd;
GLOBALTON(CNProtectManager);

CNProtectManager::CNProtectManager(void)
{
	m_szHackMsg[0] = 0;
}

CNProtectManager::~CNProtectManager(void)
{
}

void CNProtectManager::Check()
{
	if( !mpCheckDelay->Check() )
	{
		if(g_pNPGame->Check() != NPGAMEMON_SUCCESS)
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );

		mpCheckDelay->Start();
	}
}

void CNProtectManager::NetworkMsgParse(BYTE Protocol, void* pMsg)
{
	switch(Protocol)
	{
	case MP_NPROTECT_QUERY:
		{
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

			GG_AUTH_DATA ggData;

			ggData.dwIndex = pmsg->dwData1;
			ggData.dwValue1 = pmsg->dwData2;
			ggData.dwValue2 = pmsg->dwData3;
			ggData.dwValue3 = pmsg->dwData4;

			g_pNPGame->Auth2(&ggData);
		}
		break;
	case MP_NPROTECT_DISCONNECT:
		{
			// 서버로 응답을 보내지 못했거나
			// 서버에 보낸 메세지가 잘못되었을경우
			// Agent에서 Disconnect시키면서 이 메세지를 보낸다
		}
		break;
	default:
		break;
	}
}

BOOL CNProtectManager::Init(HWND hWnd)
{
	FILE *fp = fopen( "LunaOnlineTW.ini", "r" );
	if( fp )
	{
		g_pNPGame = new CNPGameLib("LunaOnlineTW");
		fclose(fp);
	}
	else
	{
		fp = fopen( "LunaOnlineTWTest.ini", "r" );

		if( fp )
		{
			g_pNPGame = new CNPGameLib("LunaOnlineTWTest");
			fclose(fp);
		}
		else
		{
			char msg[256];
			char lpszMsg[256];

			wsprintf(lpszMsg, "GameGuard initialization error. Try rebooting and executing the game or close the program considered to cause a collision.");
			wsprintf(msg, "GameGuard execute error : %lu", NPGAMEMON_ERROR_INIT);
			MessageBox(NULL, lpszMsg, msg, MB_OK);
			
			Release();

			return FALSE;
		}
	}
	mpCheckDelay = new cTimeDelay();
	mpCheckDelay->Init( 10000 );
	
	DWORD dwResult = g_pNPGame->Init();
	if (dwResult != NPGAMEMON_SUCCESS)
	{
		char msg[256];
		char lpszMsg[256];

		// ‘6. 주요에러코드’를 참조하여 상황에 맞는 메시지를 출력해줍니다.
		switch (dwResult)
		{
		case NPGAMEMON_ERROR_EXIST:
			wsprintf(lpszMsg, "GameGuard is already running. Try rebooting first and executing the game again."); 
			break;
		case NPGAMEMON_ERROR_GAME_EXIST:
			wsprintf(lpszMsg, "There are multiple events of game execution or GameGuard is already running. Close the game then try again.");
			break;
		case NPGAMEMON_ERROR_INIT:
			wsprintf(lpszMsg, "GameGuard initialization error. Try rebooting and executing the game or close the program considered to cause a collision.");
			break;
		case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
		case NPGAMEMON_ERROR_NFOUND_GG:
		case NPGAMEMON_ERROR_AUTH_INI:
		case NPGAMEMON_ERROR_NFOUND_INI:
			wsprintf(lpszMsg, "GameGuard file does not exist or is corrupted. Please install the GameGuard setup file.");
			break;
		case NPGAMEMON_ERROR_CRYPTOAPI:
			wsprintf(lpszMsg, "Window system files might be corrupted. Please reinstall the Internet Explorer(IE).");
			break;
		case NPGAMEMON_ERROR_EXECUTE:
			wsprintf(lpszMsg, "Fail to run GameGuard. Please reinstall the GameGuard setup file.");
			break;
		case NPGAMEMON_ERROR_ILLEGAL_PRG:
			wsprintf(lpszMsg, "Illegal program detected. Close all the unnecessary programs before running the game.");
			break;
		case NPGMUP_ERROR_ABORT:
			wsprintf(lpszMsg, "GameGuard update has been aborted. Please check the status of Internet network or personal firewall settings when unable to connect continuously.");
			break;
		case NPGMUP_ERROR_CONNECT:
		case NPGMUP_ERROR_DOWNCFG:
			wsprintf(lpszMsg, "Fail to connect the GameGuard update server. Please try again after a while, or check personal Firewall settings if any.");
			break;
		case NPGMUP_ERROR_AUTH:
			wsprintf(lpszMsg, "Fail to complete GameGuard update. Suspend Anti-Virus program temporarily and try the game, or check the settings of PC management programs if any.");
			break;
		case NPGAMEMON_ERROR_NPSCAN:
			wsprintf(lpszMsg, "Failed to load virus and hacking tool scanning module. Possibly due to lack of memory or virus infection");
			break;
		default:
			// Display relevant messages.
			wsprintf(lpszMsg, " Error occurred while running GameGuard. Please send all *.erl files in GameGuard folder to Game2@inca.co.kr via email.");
				break;

		}
		wsprintf(msg, "GameGuard execute error : %lu", dwResult);
		MessageBox(NULL, lpszMsg, msg, MB_OK);

		Release();
		return FALSE;
	}

	g_pNPGame->SetHwnd(hWnd);

	Check();

	return TRUE;
}

void CNProtectManager::Release()
{
	if(g_pNPGame)
		delete g_pNPGame;

	g_pNPGame = NULL;
}

BOOL CNProtectManager::MsgProc(DWORD dwMsg, DWORD dwArg)
{
	switch (dwMsg)
	{
	case NPGAMEMON_COMM_ERROR:
	case NPGAMEMON_COMM_CLOSE:
		{
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL ); // 종료 코드
		}
		return FALSE;
	case NPGAMEMON_INIT_ERROR:
		{
			wsprintf(m_szHackMsg, "GameGuard initialization error : %lu", dwArg);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL ); // 종료 코드
		}
		return FALSE;
	case NPGAMEMON_SPEEDHACK:
		{
			wsprintf(m_szHackMsg, "Speedhack detected.");
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL ); // 종료 코드

			MSG_DWORD msg;

			msg.Category = MP_NPROTECT;
			msg.Protocol = MP_NPROTECT_USER_DISCONNECT;
			msg.dwData = dwMsg;

			NETWORK->Send( &msg, sizeof( msg ) );
		}
		return FALSE;
	case NPGAMEMON_GAMEHACK_KILLED:
	case NPGAMEMON_GAMEHACK_DETECT:
		{
			wsprintf(m_szHackMsg, "Gamehack detected.\r\n%s", g_pNPGame->GetInfo());
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL ); // 종료 코드
			
			MSG_DWORD msg;

			msg.Category = MP_NPROTECT;
			msg.Protocol = MP_NPROTECT_USER_DISCONNECT;
			msg.dwData = dwMsg;

			NETWORK->Send( &msg, sizeof( msg ) );
		}
		return FALSE;
	case NPGAMEMON_GAMEHACK_DOUBT:
		{
			wsprintf(m_szHackMsg, "game or GameGuard file corrupted.");
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL ); // 종료 코드

			MSG_DWORD msg;

			msg.Category = MP_NPROTECT;
			msg.Protocol = MP_NPROTECT_USER_DISCONNECT;
			msg.dwData = dwMsg;

			NETWORK->Send( &msg, sizeof( msg ) );
		}
		return FALSE;
	case NPGAMEMON_CHECK_CSAUTH2:
		{
			GG_AUTH_DATA ggData;

			memcpy(&ggData, (PVOID)dwArg, sizeof(GG_AUTH_DATA));

			MSG_DWORD4 msg;

			msg.Category = MP_NPROTECT;
			msg.Protocol = MP_NPROTECT_ANSWER;
			msg.dwData1 = ggData.dwIndex;
			msg.dwData2 = ggData.dwValue1;
			msg.dwData3 = ggData.dwValue2;
			msg.dwData4 = ggData.dwValue3;

			NETWORK->Send( &msg, sizeof( msg ) );
		}
		return TRUE;
	}

	return TRUE; // 계속 진행
}

char* CNProtectManager::GetMsg()
{
	if(m_szHackMsg[0])
		return m_szHackMsg;
	else
		return NULL;
}

void CNProtectManager::UserIDSend( char* userID )
{
	g_pNPGame->Send( userID );
}

#endif

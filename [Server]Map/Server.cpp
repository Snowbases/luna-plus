// Server.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef _MAPSERVER_
#include "..\[CC]Header\GameResourceManager.h"
#include "PartyManager.h"
#endif

HANDLE g_hMap = NULL;
char g_szMapName[64];
void LeaveServerApplication();
BOOL EnterServerApplication();
char * GetCommandLineArg(int idx, char * szCommandLine);
char g_szHeroIDName[17] = { 0, };

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CreateDirectory( "./Log", NULL );

	SEND_CHARACTER_TOTALINFO ss;
	SEND_MONSTER_TOTALINFO mm;

	GetCurTimeToString();
	WORD mapnum = WORD( atoi(lpCmdLine) );
	GAMERESRCMNGR->SetLoadMapNum(mapnum);
	GAMERESRCMNGR->LoadRevivePoint();
	// 071221 KTH -- 기억 할수 있는 맵 리스트를 읽어온다.
	//GAMERESRCMNGR->LoadPreMemoryMapList();
	if(mapnum == 0)
	{
		MessageBox(NULL,"Give Parameter as a Map Number",NULL,NULL);
		return 0;
	}
	wsprintf(g_szMapName, "MapServer_%d", mapnum); 
	if(!EnterServerApplication()) return 0;

	FILE* fp = fopen("_ASSERTBOXON","r");
	if(fp)
	{
		g_bAssertMsgBox = TRUE;
		fclose(fp);
	}

	g_pServerSystem = new CServerSystem;
	g_pServerSystem->Start(mapnum);
	g_pServerSystem->End();
	delete g_pServerSystem;
	g_pServerSystem = NULL;
	
	LeaveServerApplication();

	return 0;
}

char * GetCommandLineArg(int idx, char * szCommandLine)
{
	char sep[]   = " ";
	char line[255];
//	strcpy(line, szCommandLine);
	SafeStrCpy( line, szCommandLine, 255 );
	char * token = strtok( line, sep );
	while( token != NULL && idx != 0)
	{
		token = strtok( NULL, sep );
		--idx;
	}
	return token;
}

BOOL EnterServerApplication()
{//taiyo
    g_hMap = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 1024, g_szMapName);
    if(g_hMap != NULL && GetLastError() == ERROR_ALREADY_EXISTS) 
    { 
		CloseHandle(g_hMap); 
		g_hMap = NULL;
		return FALSE;
    }
	
	return TRUE;
}

void LeaveServerApplication()
{//taiyo
	if(g_hMap)
	{
		CloseHandle(g_hMap);
		g_hMap = NULL;
	}	
}

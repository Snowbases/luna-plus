#include "stdafx.h"
#include "MainGame.h"
#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cTextArea.h"
#include "./Interface/cFont.h"
#include "cMsgBox.h"
#include "./Input/UserInput.h"
#include "./Input/cIMEWnd.h"
#include "ChatManager.h"
#include "../[CC]Header/GameResourceManager.h"
#include "MouseCursor.h"
#include "mhFile.h"
#include "OptionManager.h"
#include "MHAudioManager.h"
#include "ObjectManager.h"
#include "MHMap.h"
#include "ExitManager.h"
#include "QuestManager.h"
#include "ServerConnect.h"
#include "cResourceManager.h"
#include "../hseos/Farm/SHFarmManageDlg.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <exdispid.h>
#include "MainTitle.h"

#ifdef _TESTCLIENT_
	#include "TSToolManager.h"
	#include "CommCtrl.h"
#endif

#ifdef _GMTOOL_
	#include "GMToolManager.h"
	#include "CommCtrl.h"
#endif

#ifdef TAIWAN_LOCAL
	#include <shellapi.h>
#endif

#ifdef _HACK_SHIELD_
	#include "HShield.h"
	#include "HackShieldManager.h"
#endif
#ifdef _NPROTECT_
// nProtect
//#define NO_GAMEGUARD
	#include "NPGameLib.h"
	#include "NProtectManager.h"
#endif

#ifdef _ERROR_REPORT_

#include "./ErrorReport/include/IErrorReportLib.h"

#ifdef _DEBUG
	#ifdef _WIN64
		#pragma comment( lib, "./ErrorReport/lib/ErrorReportLib_x64d.lib")
	#else
		#pragma comment( lib, "./ErrorReport/lib/ErrorReportLib_x86d.lib")
	#endif
#else
	#ifdef _WIN64
		#pragma comment( lib, "./ErrorReport/lib/ErrorReportLib_x64.lib")
	#else
		#pragma comment( lib, "./ErrorReport/lib/ErrorReportLib_x86.lib")
	#endif
#endif

#endif

#define _LOGOWINDOW_

//////////////////////////////////////////////////////////////////////////
// Version 정보
// 각 로컬별 클라이언트의 버전을 적어준다

/////////////////////////////////////////////////////////////////////////

char g_CLIENTVERSION[32] = "LPTW10062701";


/////////////////////////////////////////////////////////////////////////	

#define MAX_LOADSTRING 100

// Global Variables:
HANDLE g_hMap = NULL;
HINSTANCE g_hInst;								// current instance
HWND _g_hWnd;
char szTitle[MAX_LOADSTRING];								// The title bar text
char szWindowClass[MAX_LOADSTRING];								// The title bar text
BOOL g_bActiveApp = FALSE;
char g_AgentAddr[16];
WORD g_AgentPort;
char g_PATCHVERSION[256];
#ifdef _TW_LOCAL_
char g_szHeroIDName[21];
char g_szHeroKey[21];
#else
char g_szHeroIDName[MAX_NAME_LENGTH+1];
#endif
int	g_nServerSetNum;
ScriptCheckValue g_Check;

HIMC g_hIMC = NULL;


CComModule _Module; 
BEGIN_OBJECT_MAP(ObjectMap) 
END_OBJECT_MAP() 

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void LeaveClientApplication();
BOOL EnterClientApplication();

//BOOL CheckParameter(LPSTR parameter);
BOOL	GetPatchVersion();
BOOL	CheckResourceFile();
void	ChangeVerInfo();
BOOL	SaveFileList();

#ifdef _HACK_SHIELD_
BOOL HS_Init();
BOOL HS_StartService();
BOOL HS_StopService();
BOOL HS_UnInit();
int __stdcall HS_CallbackProc (long lCode, long lParamSize, void* pParam );
BOOL HS_PauseService();
BOOL HS_ResumeService();
void HS_SaveFuncAddress();

LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr)
{
	HS_StopService();
	HS_UnInit();
	return EXCEPTION_EXECUTE_HANDLER;
}

#endif

#ifdef _NPROTECT_

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg);

LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr)
{
	NPROTECTMGR->Release();
	return EXCEPTION_EXECUTE_HANDLER;
}

#endif


void ClearDirectory( LPCTSTR pStrName )
{
	WIN32_FIND_DATA FindFileData;
	char buf[MAX_PATH];
	wsprintf( buf, "%s\\*", pStrName );

	HANDLE hFind = FindFirstFile( buf, &FindFileData);

	if( hFind == INVALID_HANDLE_VALUE ) return;

	while( 1 )
	{
		if( FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
		{
			if( strcmp( FindFileData.cFileName, "." ) != 0 &&
				strcmp( FindFileData.cFileName, ".." ) != 0 )
			{
				wsprintf( buf, "%s\\%s", pStrName, FindFileData.cFileName );
				ClearDirectory( buf );
				RemoveDirectory( buf );
			}
		}
		else
		{
			wsprintf( buf, "%s\\%s", pStrName, FindFileData.cFileName );
			DeleteFile( buf );
		}
		if( FindNextFile( hFind, &FindFileData ) == FALSE )
			break;
	}
	FindClose( hFind );	
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	GUID guid; 
	AtlAxWinInit(); 
	_Module.Init(ObjectMap, hInstance, &guid); 

#ifdef _TW_LOCAL_
	sscanf( lpCmdLine, "%s %s", g_szHeroIDName, g_szHeroKey );
#endif

	SetScriptCheckValue( g_Check );

	char buf[MAX_PATH]={0,};
	char commandToken[MAX_PATH]={0,};
	const char* delimit = " ";
	strncpy( buf, lpCmdLine, sizeof(buf) );
	char* token = strtok( buf, delimit );
	if( token )
		strncpy( commandToken, token, sizeof(commandToken) );

#ifdef _KOR_WEBLAUNCHER_

#ifdef _GMTOOL_
	if( lpCmdLine )
	{
		FILE* fpLog = NULL;
		fpLog = fopen( "./ClientStartLog.txt", "a+" );
		if( fpLog )
		{
			SYSTEMTIME sysTime;
			GetLocalTime( &sysTime );

			fprintf( fpLog, "[%04d-%02d-%02d %02d:%02d:%02d] Execute Command Line : %s\n", 
				sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
				lpCmdLine );

			fclose( fpLog );
		}	
	}
#endif

	token = strtok( 0, delimit );
	if( token == NULL )
	{
		// 홈페이지
		ShellExecute( 0, "open", "explorer", "http://lunaplus.enpang.com/", 0, SW_SHOWNORMAL );
		return 0;
	}

	TITLE->SetLoginKey(token);
#endif

	FILE* fp = NULL;

#ifndef _TW_LOCAL_
#ifndef _GMTOOL_
	if( strcmp( commandToken, "anrgideoqkr" ) != 0)
	{
		MessageBox( NULL, "You should not execute directly this file!!", "Error!!", MB_OK );
		return 0;
	}

#endif
#endif
	if(!EnterClientApplication()) return 0;


#ifdef _HACK_SHIELD_

	DIRECTORYMGR->Init();

	if( !HS_Init() )
	{
		HS_UnInit();
		MessageBox( NULL, "HackShield Init Failed.", "ERROR", MB_OK );	
		return 0;
	}
	if( !HS_StartService() )
	{
		HS_StopService();
		HS_UnInit();
		MessageBox( NULL, "HackShield Init Failed.", "ERROR", MB_OK );
		return 0;
	}
	HS_SaveFuncAddress();

	::SetUnhandledExceptionFilter(TopLevelFilter);

#endif

	DeleteFile("Debug.txt");
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtMemDumpAllObjectsSince(0);
	
	fp = fopen("_DONOTUSEENGINEPACK","r");
	if(fp)
	{
		g_bUsingEnginePack = FALSE;
		fclose(fp);
	}
 
	// Initialize global strings
	GetPatchVersion();

	GAMERESRCMNGR->LoadGameDesc();

#ifdef _KOR_
#ifdef _TESTCLIENT_
	sprintf(szTitle,"루나플러스(TEST) %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__,g_CLIENTVERSION,g_PATCHVERSION);
	sprintf(szWindowClass,"루나플러스(TEST) %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__, g_CLIENTVERSION,g_PATCHVERSION);
#elif _GMTOOL_
	sprintf(szTitle,"루나플러스[GM_TOOL] %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__,g_CLIENTVERSION,g_PATCHVERSION);
	sprintf(szWindowClass,"루나플러스[GM_TOOL] %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__, g_CLIENTVERSION,g_PATCHVERSION);
#else
	sprintf(szTitle,"루나플러스");
	sprintf(szWindowClass,"루나플러스");
#endif
#else
#ifdef _TESTCLIENT_
	sprintf(szTitle,"LUNA Online(TEST) %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__,g_CLIENTVERSION,g_PATCHVERSION);
	sprintf(szWindowClass,"LUNA Online(TEST) %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__, g_CLIENTVERSION,g_PATCHVERSION);
#elif _GMTOOL_
	sprintf(szTitle,"LUNA Online[GM_TOOL] %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__,g_CLIENTVERSION,g_PATCHVERSION);
	sprintf(szWindowClass,"LUNA Online[GM_TOOL] %s %s ExeVer:%s PatchVer:%s",__DATE__,__TIME__, g_CLIENTVERSION,g_PATCHVERSION);
#else
	sprintf(szTitle,"LUNA Online");
	sprintf(szWindowClass,"LUNA Online");
#endif
#endif
	MyRegisterClass(hInstance);
	//LOGEX(lpCmdLine, PT_MESSAGEBOX);

	GAMERESRCMNGR->LoadDeveloperInfo();
	// quest data load
	QUESTMGR->LoadQuestString( "./system/Resource/QuestString.bin" );
	QUESTMGR->LoadQuestScript( "./system/Resource/QuestScript.bin" );
	//QUESTMGR->LoadNewbieGuide( "./system/Resource/NewbieGuide.bin" );
	QUESTMGR->LoadQuestNpc( "./system/Resource/QuestNpcList.bin" );

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		LeaveClientApplication();
		return FALSE;
	}


#ifdef _ERROR_REPORT_
	// 요렇게 걸어놓으시면 됩니다.
	INIT_DUMP_HANDLER init;
	memset( &init, 0, sizeof(init));
	init.iBuildNum	=	1;
	init.dumpType	=	MiniDumpNormal;
	strcpy( init.szFtpServerIP, "218.234.20.167");				// 여기에 ftp 서버 ip 넣는다.
	init.iFtpServerPort	=	21;
	strcpy( init.szUserName, "lunasrv_ftp");
	strcpy( init.szPass, "fnskeksehrftp~!");
	init.hApplicationInstance = hInstance;
	init.bUseDescriptionDialog = TRUE;

	InitErrorDumpHandler(NULL);		// szFtpServerIP == NULL 일 경우 dump 만 생성.

	SetUnhandledExceptionFilter( ErrorDumpHandler);
#endif

//	HACCEL hAccelTable;
//	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TEST);

	// Main message loop:
	MSG msg;
	while (1) //GetMessage(&msg, NULL, NULL, NULL));
    {
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
//			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
//			{
#ifdef _GMTOOL_
			if( !GMTOOLMGR->IsGMDialogMessage(&msg) )
#elif _TESTCLIENT_
			if( !TSTOOLMGR->IsTSDialogMessage(&msg) )
#endif
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); 
			}
//			}
		}
		else
		{
			if(MAINGAME->Process() == -1)
				break;
		}
	}

//	DeleteFile("recv.txt");

	LeaveClientApplication();

//	_ASSERTE( _CrtCheckMemory( ) );

#ifdef _HACK_SHIELD_
	HS_StopService();
	HS_UnInit();
#endif
#ifdef _NPROTECT_	
	NPROTECTMGR->Release();
#endif
	return msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 
#ifndef _GMTOOL_
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_NOCLOSE;
#else
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
#endif
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
//	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TEST);
//	wcex.hIcon			= NULL;
	wcex.hIcon			= LoadIcon( hInstance, MAKEINTRESOURCE(IDI_GAMEICON) );

	wcex.hCursor		= NULL;
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//if(!EnterClientApplication()) return FALSE;	//A¨Iⓒª=8￠®￠´I ¡E?A￠®¨¡a¡E¡IU. //KES

	CURSOR->Init( hInstance );
	
	HWND hWnd;
	g_hInst = hInstance; // Store instance handle in our global variable
//	int x = GetSystemMetrics(SM_CXSIZEFRAME)*2;
	int x = GetSystemMetrics(SM_CXFIXEDFRAME)*2;
//	int y = GetSystemMetrics(SM_CYSIZEFRAME)*2+GetSystemMetrics(SM_CYCAPTION);
	int y = GetSystemMetrics(SM_CXFIXEDFRAME)*2+GetSystemMetrics(SM_CYCAPTION);

#ifndef _GMTOOL_
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_CLIPCHILDREN |
		WS_SYSMENU;
#else
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_CLIPCHILDREN |
		WS_SYSMENU | WS_MINIMIZEBOX;
#endif
	// 070201 LYW --- Modified window style and whidth height.
	//_g_hWnd = hWnd = CreateWindow(szWindowClass, szTitle, dwStyle, //WS_OVERLAPPEDWINDOW,
		//CW_USEDEFAULT, 0, 1280+x, 1024+y, NULL, NULL, hInstance, NULL);	//WINDOW SIZE

#ifdef _TESTCLIENT_
	dwStyle |= WS_THICKFRAME;
#endif
	DWORD width		= 0 ;
	DWORD height	= 0 ;

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	width	= dispInfo.dwWidth ;
	height	= dispInfo.dwHeight ;

	// desc_hseos_해상도01
	// S 해상도 추가 added by hseos 2007.06.25
	// ..윈도우 모드에서 사용자의 디스플레이 해상도와 게임 해상도가 같으면 오른쪽이 삐져나가고, 아래쪽은 
	// ..잘리는 문제가 있다. 오른쪽이 삐져나가는 이유는 위의 x = GetSystemMetrics(SM_CXFIXEDFRAME)*2;
	// ..에서 윈도우 시작점에 x 를 더해주기 때문이다. (좌우 프레임을 고려하기 위해)
	// ..윈도우 모드에서 사용자의 디스플레이 해상도x와 게임 해상도x가 같으면 프레임을 고려하지 않기로 한다.
	// ..(근데, 다른 해상도에서도 x = 0 으로 해도 별반 차이가 없다.....뭐지??? 윈도우 버전에 따라 달라지는 것 같기도..)
	// ..(어쨌든 같은 해상도에선 문제가 되므로 x 는 0..)
	// ..아래쪽이 잘리는 이유는 잘 모르겠음..
	DWORD nUserScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	if (nUserScreenWidth == width) x = 0;
	// E 해상도 추가 added by hseos 2007.06.25

	_g_hWnd = hWnd = CreateWindow(szWindowClass, szTitle, dwStyle, //WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, width+x, height+y, NULL, NULL, hInstance, NULL);	//WINDOW SIZE
	
	if (!hWnd)
	{
		return FALSE;
	}

#ifndef _NO_NPROTECT_START
#ifdef _NPROTECT_
	if(!NPROTECTMGR->Init(_g_hWnd))
	{
		return FALSE;
	}

	::SetUnhandledExceptionFilter(TopLevelFilter);
#endif
#endif

//	gMp3 = new CBGMLoader(hInstance,hWnd);

	ShowWindow(hWnd, nCmdShow);
	
//	if( g_bWindowMode )
//		ShowWindow(hWnd, nCmdShow);
//	else
//		ShowWindow(hWnd, SW_HIDE);

	UpdateWindow(hWnd);
	ShowCursor(TRUE);

	CoInitialize(NULL);

#ifndef _LOGOWINDOW_
	PlayCutscene("logo.avi",g_hInst);
#endif

	//////////////////////////////////////////////////////////////////////////
	// MAININIT
	MAINGAME->Init(hWnd);
		
#ifndef _TESTCLIENT_
	MAINGAME->SetGameState(eGAMESTATE_CONNECT);
#else
	TSTOOLMGR->CreateTSDialog();

	int initKind = eGameInInitKind_Login;
	MAINGAME->SetGameState(eGAMESTATE_GAMEIN,&initKind,sizeof(int));
#endif                 

#ifdef _GMTOOL_
	GMTOOLMGR->CreateGMDialog();
#endif
	
	
	return TRUE;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MOUSE->GetWindowMessage(hWnd, message, wParam, lParam);
	CHATMGR->MsgProc( message, wParam ) ;
	CIMEWND->OnMsg(hWnd, message, wParam, lParam);
	
	switch (message) 
	{
		// 100209 LUJ, 아래 메시지 처리를 지우면, IME 입력이 정상적으로 되지 않는다
	case WM_IME_CHAR:
#ifndef _TW_LOCAL_
	case WM_IME_STARTCOMPOSITION:
#endif
	case WM_CHAR:
	case WM_SYSKEYUP:
		return 0;
	case WM_SYSCOMMAND:
		{
			if(SC_KEYMENU == wParam)
			{
				return 0;
			}
		}
		break;
	case WM_ACTIVATEAPP:
		{
			g_bActiveApp = (BOOL)wParam;

			if( !g_bActiveApp )
			{
				AUDIOMGR->MuteBGM( TRUE );

				CIMEWND->SetCtrlPushed( FALSE );
			}
			else
			{
				AUDIOMGR->MuteBGM( FALSE );
			}
		}
		break;

	case WM_ACTIVATE:
		
		if( g_UserInput.GetKeyboard()->GetDirectInput() )
		{
			if( WA_INACTIVE != wParam )
			{
				// Make sure the device is acquired, if we are gaining focus.
				g_UserInput.GetKeyboard()->ClearKeyState();
				g_UserInput.GetKeyboard()->UpdateKeyboardState();
				//g_UserInput.SetInputFocus( TRUE );
				OBJECTMGR->SetAllNameShow( FALSE );
			}
		}
		break;

	case WM_USER+155:
		{
			ISC_BaseNetwork* lpNet = NETWORK->GetBaseNetwork();
			if(lpNet)
				lpNet->SocketEventProc();
		}
		return TRUE;		
		
	case WM_CLOSE:
		{
			MAINGAME->SetGameState( eGAMESTATE_END );

#ifdef _NPROTECT_
			if(NPROTECTMGR->GetMsg())
			{
				MessageBox(hWnd, NPROTECTMGR->GetMsg(), "nProtect GameGuard", MB_OK);
			}
#endif
		}
		break;

	case WM_SETFOCUS:
	case WM_SETCURSOR:
		{ 
			CURSOR->Process();
		}
		return 0;
	case WM_SYSKEYDOWN:		//Alt + F4 ¨Io¨I￠®Ao
		{
			if( wParam == VK_F4 )
			{
				if( MAINGAME->GetCurStateNum() == eGAMESTATE_TITLE )
				{
					break;		// ￠®¨¡OAO E¡EcAI ¡§uE￠®¨¡i ~￠®￠´a
				}
				else if( MAINGAME->GetCurStateNum() == eGAMESTATE_CHARSELECT ||
					MAINGAME->GetCurStateNum() == eGAMESTATE_CHARMAKE )
				{
					WINDOWMGR->MsgBox( MBI_EXIT, MBT_YESNO, RESRCMGR->GetMsg( 263 ) );
					return 0;
				}
			}
		}
		return 0;
	case WM_MOVE:
#ifdef _GMTOOL_
		GMTOOLMGR->SetPositionByMainWindow();
#elif _TESTCLIENT_
		TSTOOLMGR->SetPositionByMainWindow();
		TSTOOLMGR->SetPositionByOutputWindow();
#endif
		break;


	case WM_CREATE:
		{
#ifdef _GMTOOL_
			INITCOMMONCONTROLSEX icex;
			icex.dwSize = sizeof( icex );
			icex.dwICC	= ICC_DATE_CLASSES;
			InitCommonControlsEx( &icex );
#elif _TESTCLIENT_
			INITCOMMONCONTROLSEX icex;
			icex.dwSize = sizeof( icex );
			icex.dwICC	= ICC_DATE_CLASSES;
			InitCommonControlsEx( &icex );
#endif

#ifdef _JAPAN_LOCAL_
			HIMC hImc = ImmGetContext( hWnd );
			DWORD dwConversion, dwSentence;
			ImmGetConversionStatus( hImc, &dwConversion, &dwSentence );
			dwSentence |= IME_SMODE_SINGLECONVERT;
			ImmSetConversionStatus( hImc, dwConversion, dwSentence );
			ImmReleaseContext( hWnd, hImc );
#endif
		}
		break;
	}
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}



// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


BOOL GetPatchVersion()
{
	CMHFile file;
	if(!file.Init("LunaVerInfo.ver", "r"))
		return FALSE;
	strcpy(g_PATCHVERSION, file.GetString());

	return TRUE;
}

BOOL EnterClientApplication()
{
#ifndef _GMTOOL_
    g_hMap = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 1024, "LUNA Online Client Application");
    if(g_hMap != NULL && GetLastError() == ERROR_ALREADY_EXISTS) 
    { 
		CloseHandle(g_hMap); 
		g_hMap = NULL;
		return FALSE;
    }
#endif

	return TRUE;
}


void LeaveClientApplication()
{//taiyo
	if(g_hMap)
	{
		CloseHandle(g_hMap);
		g_hMap = NULL;
	}	
}


//pjslocal
#ifdef TAIWAN_LOCAL
int is_big5(BYTE *str)
{
	if( str[0] & 0x80 )
	{
		if ( str[0] == 0xa2)
		{
			if( str[1]>= 0xaf && str[1]<= 0xb8 )
				return 1;
			else if( str[1] >= 0xcf && str[1] <= 0xff )
				return 1;
		} 
		if ( str[0] == 0xa3)
		{
			if( str[1]>=0x43 && str[1]<=0xff )
				return 1;
		}

		if ( str[0] == 0xa4)
		{
			if( str[1] >= 0x40 && str[1] <= 0xff )
				return 1;
		}

		if ( str[0] >= 0xa5 && str[0] <= 0xc5 )
			return 1;

		if ( str[0] == 0xc6 )
		{
			if( str[1] >= 0x00 && str[1] <= 0x7e )
				return 1;
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////
// resource ¡§￠®u¡E¡Io¡§uEAC ¡§￠®AAI ￠®¨¡E￠®ic(jsd)
///////////////////////////////////////////////////////////////////////////////////////
BOOL CheckResourceFile()
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	SYSTEMTIME time;
	char Dir[MAX_PATH];
	char filename[MAX_PATH];
	BOOL bResult = TRUE;
	CMHFile file;
	char temp[256];
	WORD year, month, day, hour, min;	

	GetWindowsDirectory( Dir, MAX_PATH );
	strcat( Dir, "\\mfl.dat" );
		
	GetCurrentDirectory( MAX_PATH, filename );
	strcat( filename, "\\Resource\\*.bin" );
	hSrch = FindFirstFile( filename, &wfd );
	while( bResult )
	{
		FileTimeToSystemTime( &wfd.ftLastWriteTime, &time );

		///
		if( !file.Init( Dir, "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
		{
			file.Release();
			goto filesave;
		}
		while( !file.IsEOF() )
		{
			file.GetString( temp );
			if( strcmp( wfd.cFileName, temp ) == 0 )
			{
				year = file.GetWord();
				month = file.GetWord();
				day = file.GetWord();
				hour = file.GetWord();
				min = file.GetWord();
				
				if( year != time.wYear || month != time.wMonth || day != time.wDay ||
					hour != time.wHour || min != time.wMinute )
				{
					file.Release();
					return FALSE;
				}
				else
					break;
			}
		}
		file.Release();
		///		

		bResult = FindNextFile( hSrch, &wfd );
	}

	bResult = TRUE;
	GetCurrentDirectory( MAX_PATH, filename );
	strcat( filename, "\\Resource\\SkillArea\\*.bsad" );
	hSrch = FindFirstFile( filename, &wfd );
	while( bResult )
	{
		FileTimeToSystemTime( &wfd.ftLastWriteTime, &time );

		///
		if( !file.Init( Dir, "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
		{
			file.Release();
			goto filesave;
		}
		while( !file.IsEOF() )
		{
			file.GetString( temp );
			if( strcmp( wfd.cFileName, temp ) == 0 )
			{
				year = file.GetWord();
				month = file.GetWord();
				day = file.GetWord();
				hour = file.GetWord();
				min = file.GetWord();
				
				if( year != time.wYear || month != time.wMonth || day != time.wDay ||
					hour != time.wHour || min != time.wMinute )
				{
					file.Release();
					return FALSE;
				}
				else
					break;
			}
		}
		file.Release();
		///		

		bResult = FindNextFile( hSrch, &wfd );
	}

	return TRUE;

filesave:
	SaveFileList();
	return TRUE;
}

void ChangeVerInfo()
{
	char temp[256];
	FILE* fp = fopen( "LunaVerInfo.ver", "r" );
	if( fp == NULL )
		return;
	fscanf( fp, "%s", temp );
	fclose( fp );

	temp[10] = NULL;
	fp = fopen( "LunaVerInfo.ver", "w" );
	if( fp == NULL )
		return;
	fprintf( fp, "%s%s", temp, "01" );
	fclose( fp );
}

BOOL SaveFileList()
{
	DWORD count = 1;
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	SYSTEMTIME time;
	char Dir[MAX_PATH];
	char filename[MAX_PATH];
	BOOL bResult = TRUE;
	
	GetWindowsDirectory( Dir, MAX_PATH );
	strcat( Dir, "\\mfl.dat" );
	FILE* fp = fopen( Dir, "w" );
	if( fp == NULL )
	{
		fclose( fp );
		return FALSE;
	}

	GetCurrentDirectory( MAX_PATH, filename );
	strcat( filename, "\\Resource\\*.bin" );
	hSrch = FindFirstFile( filename, &wfd );
	while( bResult )
	{
		FileTimeToSystemTime( &wfd.ftLastWriteTime, &time );
		fprintf( fp, "%d %s\t", count, wfd.cFileName );
		fprintf( fp, "%4d %02d %02d %02d %02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute );
		bResult = FindNextFile( hSrch, &wfd );
		++count;
	}
	
	bResult = TRUE;
	GetCurrentDirectory( MAX_PATH, filename );
	strcat( filename, "\\Resource\\SkillArea\\*.bsad" );
	hSrch = FindFirstFile( filename, &wfd );
	while( bResult )
	{
		FileTimeToSystemTime( &wfd.ftLastWriteTime, &time );
		fprintf( fp, "%d %s\t", count, wfd.cFileName );
		fprintf( fp, "%4d %02d %02d %02d %02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute );
		bResult = FindNextFile( hSrch, &wfd );
		++count;
	}

	fclose( fp );

	return TRUE;
}

#ifdef _HACK_SHIELD_
BOOL HS_Init(void)
{
	int		dwRet = 0;			// _AhnHS_Initialize 함수 리턴 값


	TCHAR	szFullFilePath[MAX_PATH];	// 파일 경로
	TCHAR	szMsg[MAX_PATH];		// 메시지 버퍼
	DWORD	dwOption = 0;			// 초기화 옵션

	// 현재 게임의 실행 경로를 구합니다.
	// ※ 주의 :
	//    GetCurrentDirectory 함수가 아닌, GetModuleFileName 함수를 이용해서 현재실행중인
	//    파일의 경로를 구하시기 바랍니다.
	strcpy(  szFullFilePath, DIRECTORYMGR->GetFullDirectoryName(eLM_Root) );
	// 게임과 연동된 HackShield의 EhSvc.Dll의 경로 설정	
	lstrcat ( szFullFilePath, _T( "HShield\\EhSvc.dll" ) );

	// 초기화 옵션을 설정합니다. AHNHS_CHKOPT_로 시작하는 옵션을 모두 적용하려면


	// dwOption = AHNHS_CHKOPT_ALL 로 간단하게 정의할 수 있습니다.
	// 자세한 내용은 "프로그래밍 가이드"를 참조하세요.
	// ※ 주의 :
	//    게임 디버깅 시, 
	//    AHNHS_CHKOPT_READWRITEPROCESSMEMORY, AHNHS_CHKOPT_KDTARCER
	//    AHNHS_CHKOPT_OPENPROCESS, AHNHS_CHKOPT_MESSAGEHOOK 옵션은 제거
	//    하셔야 디버깅이 가능합니다.
	dwOption = 
#ifndef _GMTOOL_
		AHNHS_CHKOPT_READWRITEPROCESSMEMORY |
		AHNHS_CHKOPT_KDTRACER |
//		AHNHS_CHKOPT_KDTARCER |
		AHNHS_CHKOPT_OPENPROCESS |
		AHNHS_CHKOPT_MESSAGEHOOK |
#endif
		AHNHS_CHKOPT_ALL;
		

	// HackShield 서비스 초기화


	// 초기화 함수 _AhnHS_Initialize는 다음과 같은 인자 값을 가지고 있습니다.
	// 3번째: 게임코드,
	// 4번째: 라이센스 키,
	// 6번째: 스피드핵 민감도


	// 적용 시 알맞은 값으로 대체하시기 바랍니다.
	// 자세한 내용은 "프로그래밍 가이드"를 참조하세요

	dwRet = _AhnHS_Initialize ( szFullFilePath, HS_CallbackProc, 5885, "01740D2BA4853F462EAE1256", 
		dwOption, AHNHS_SPEEDHACK_SENSING_RATIO_HIGH );

	// HackShield 초기화 실패 시 오류 처리
	if ( dwRet != HS_ERR_OK ) 
	{
		// _AhnHS_Initialize 함수의 호출 실패 시에 넘어오는 코드에 따라 처리해주는 부분입니다.
		// "프로그래밍 가이드"를 참조하시어 적당한 오류메시지를 설정해주시기 바랍니다.
		switch ( dwRet )
		{
		case HS_ERR_INVALID_PARAM:
			{
				wsprintf ( szMsg, "[HS Code=%x] Init: 잘못된 Callback함수 포인터 지정 및 라이센스 키 값 지정이 올바르지 않습니다. ", 
					dwRet );
				break;
			}
		case HS_ERR_INVALID_LICENSE:
			{
				wsprintf ( szMsg, "[HS Code=%x] Init: 라이센스 키가 올바르지 않습니다.", 
					dwRet );
				break;
			}
		default:
			{
				wsprintf ( szMsg, "[HS Code=%x] Init: 알 수 없는 오류가 발생하였습니다.", 
					dwRet );
				break;
			}
		}
		// 게임 내에서 알림 메시지를 발생합니다.
//		MessageBox( _g_hWnd, szMsg, "HSERR", MB_OK );


		return FALSE;
	}
	return TRUE;
}

// HackShield 서비스 시작
BOOL HS_StartService ( void )
{
	int		nReturn = 0;
	TCHAR	szMsg[MAX_PATH];

	// 시작 함수 호출 
	nReturn = _AhnHS_StartService();

	// HackShield 시작 실패시 오류 처리
	if ( nReturn != HS_ERR_OK )
	{
		switch ( nReturn )
		{
		case HS_ERR_NOT_INITIALIZED:
			{
				wsprintf ( szMsg, "[HS Code=%x] StartService: _Ahn_HS_Initialize 초기화가 필요합니다. ", nReturn ); 
					break;
			}
		case HS_ERR_ALREADY_SERVICE_RUNNING:
			{
				wsprintf ( szMsg, "[HS Code=%x] StartService: _Ahn_HS_Initialize 가 이미 초기화 되어 있습니다.", nReturn );
					break;
			}

		default:
			{
				wsprintf ( szMsg, "[HS Code=%x] StartService: 알 수 없는 오류가 발생하였습니다. ", nReturn );
					break;
			}
		}
		// 게임내에서 알림 메시지를 발생합니다.
//		MsgBox (szMsg, "알림" ..);
		return FALSE;
	}
	return TRUE;
}

// HackShield 서비스 중지
BOOL HS_StopService ( void )
{
	int		nReturn = 0;
//	TCHAR	szMsg[MAX_PATH];

	nReturn = _AhnHS_StopService();

	// HackShield 중지 실패 시 오류 처리
	if ( nReturn != HS_ERR_OK ) 
	{
//		...
			return FALSE;
	}
	return TRUE;
}

// HackShield 서비스 해제
BOOL HS_UnInit ( void )
{
	int		nReturn = 0;
//	TCHAR	szMsg[MAX_PATH];

	nReturn = _AhnHS_Uninitialize();

	// HackShield 해제 실패 시 오류 처리
	if ( nReturn != HS_ERR_OK ) 
	{
//		..
			return FALSE;
	}
	return TRUE;
}

// HackShield 서비스로부터 감지되는 모든 해킹관련 이벤트를 수신하기 위한 콜백 함수
int __stdcall HS_CallbackProc( long lCode, long lParamSize, void* pParam )
{
	TCHAR	szMsg[MAX_PATH];

	switch ( lCode )
	{
	case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			wsprintf ( szMsg, "[HS Code=%x] MoSiang can't play with this program[%s].", lCode, ( LPTSTR ) pParam );
			MessageBox( _g_hWnd, szMsg, "HSError", MB_OK );

				// 게임 내에서 알림 메시지를 발생합니다.
//				MsgBox (szMsg, "알림" ..);
			// 게임종료를 권장합니다.
			//PostQuitMessage(0);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
//	case AHNHS_ACTAPC_DETECT_SPEEDHACK_APP:
	case AHNHS_ACTAPC_DETECT_SPEEDHACK:
		{
			wsprintf ( szMsg, "[HS Code=%x] Detected SpeedHack.", lCode);
			MessageBox( _g_hWnd, szMsg, "HSError", MB_OK );
				// 게임 내에서 알림 메시지를 발생합니다.
	//			MsgBox (szMsg, "알림" ..);
			// 스피드핵 사용에 민감한 게임프로그램은 강제 종료합니다.
			//PostQuitMessage(0);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
	case AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED:
	case AHNHS_ACTAPC_DETECT_KDTRACE:
		{
			wsprintf ( szMsg, "[HS Code=%x] Detected Debuging.", lCode);
			MessageBox( _g_hWnd, szMsg, "HSError", MB_OK );

			// 게임 내에서 알림 메시지를 발생합니다.
//			MsgBox (szMsg, "알림" ..);
			// 사용자가 게임프로그램을 디버깅 작업을 진행할 가능성이 높으므로 종료를 권장합니다.
			//PostQuitMessage(0);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
	case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
	case AHNHS_ACTAPC_DETECT_MESSAGEHOOK:
	case AHNHS_ACTAPC_DETECT_MODULE_CHANGE:
		{
			wsprintf ( szMsg, "[HS Code=%x] HackShield Error.", lCode );
			MessageBox( _g_hWnd, szMsg, "HSError", MB_OK );
			// 게임 내에서 알림 메시지를 발생합니다.
//			MsgBox (szMsg, "알림" ..);
			// 게임종료를 권장함


			//PostQuitMessage(0);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
//	case AHNHS_ENGINE_DETECT_GENERAL_HACK:
	case AHNHS_ACTAPC_DETECT_ALREADYHOOKED:
	case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
//	case AHNHS_ACTAPC_DETECT_SPEEDHACK_APP:
		{
			// 위의 경우는 별도로 처리하지 않으셔도 됩니다.
			break;
		}
	}
	return 1;
}

// ※ HS_PauseService :
//    특정 XP OS에서 채팅창 한글키 입력이 불가능한 경우,
//    키보드 MSG Hooking 보호 기능을 일시적으로 해제하는 함수입니다.
BOOL HS_PauseService ( void )
{
	int		nReturn = 0;
	TCHAR	szMsg[MAX_PATH];

	nReturn = _AhnHS_PauseService ( AHNHS_CHKOPT_MESSAGEHOOK );

	if ( nReturn != HS_ERR_OK )
	{
		switch ( nReturn )
		{
		case HS_ERR_NOT_INITIALIZED:
			{
				wsprintf ( szMsg, "[HS Code=%x] PauseService: _Ahn_HS_Initialize 초기화가 필요합니다. ", nReturn );
					break;
			}
		case HS_ERR_SERVICE_NOT_RUNNING:
			{
				wsprintf ( szMsg, "[HS Code=%x] PauseService: _Ahn_HS_StartService 실행이 필요합니다.", nReturn );
				break;
			}

		default:
			{
				wsprintf ( szMsg, "[HS Code=%x] PauseService: 알수없는 오류가 발생 하였습니다.", 
					nReturn );
				break;
			}
		}
		// 게임내에서 알림 메시지를 발생합니다.
//		MsgBox (szMsg, "알림" ..);
		return FALSE;
	}
	return TRUE;
}

// _AhnHS_PauseService() 함수를 이용하여,
// 해제된 키보드 MSG Hooking 보호 기능을 재실행 한다.
BOOL HS_ResumeService ( void )
{
	int		nReturn = 0;
	TCHAR	szMsg[MAX_PATH];

	nReturn = _AhnHS_ResumeService ( AHNHS_CHKOPT_MESSAGEHOOK );

	if ( nReturn != HS_ERR_OK )
	{
		switch ( nReturn )
		{
		case HS_ERR_NOT_INITIALIZED:
			{
				wsprintf ( szMsg, "[HS Code=%x] ResumeService: _Ahn_HS_Initialize 초기화가 필요합니다.", nReturn );
					break;
			}
		case HS_ERR_SERVICE_NOT_RUNNING:
			{
				wsprintf ( szMsg, "[HS Code=%x] ResumeService: _Ahn_HS_StartService 실행이 필요합니다.", nReturn );
					break;
			}

		default:
			{
				wsprintf( szMsg, "[HS Code=%x] ResumeService: 알 수 없는 오류가 발생하였습니다.", nReturn );
					break;
			}
		}
		// 게임내에서 알림 메시지를 발생합니다.
//		MsgBox (szMsg, "알림" ..);
		return FALSE;
	}
	return TRUE;
}

void HS_SaveFuncAddress()
{
//	TCHAR	szMsg[MAX_PATH];

    int nResult = _AhnHS_SaveFuncAddress( 3,
											HS_Init,
											HS_CallbackProc,
											CHackShieldManager::NetworkMsgParse
											);
}

#endif

#ifdef _NPROTECT_
BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	return NPROTECTMGR->MsgProc(dwMsg, dwArg);
}
#endif

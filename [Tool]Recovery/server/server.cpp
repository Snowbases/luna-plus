#include "stdafx.h"
#include "MSSystem.h"
#include "..\common\version.h"

CMSSystem*	recoverySystem;
bool		isRunning;


void Exit();


BOOL WINAPI ConsoleCtrlHandler( DWORD event )
{
	switch ( event ) 
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		{
			Exit();
			//return TRUE;
		}		
	}

	return FALSE;
}


void Exit()
{
	SetConsoleCtrlHandler( ConsoleCtrlHandler, FALSE );

	SAFE_DELETE( recoverySystem );

	isRunning = false;
}


int main(int argc, char *argv[])
{
	{
		_tcprintf( _T( "\nCopyright 2008 Eya Interactive Co.Ltd. All rights reservered\n\n" ) );
		_tcprintf( _T( "\t q: exit program\n\n\n" ) );
	}

	{
		TCHAR title[ MAX_PATH ] = { 0 };

		_stprintf(
			title,
			_T( "Welcome to Recovery Server %d.%d.%d" ),
			MAJORVERSION,
			MINORVERSION,
			BUILDNUMBER );
		PutLog( title );

		_stprintf(
			title,
			_T( "Lune Online: Recovery Server %d.%d" ),
			MAJORVERSION,
			MINORVERSION );
		SetConsoleTitle( title );
		SetConsoleCtrlHandler( ConsoleCtrlHandler, TRUE );
	}	

	recoverySystem = new CMSSystem;

	if( recoverySystem->IsRunning() )
	{
		isRunning = true;
	}
	else
	{
		SAFE_DELETE( recoverySystem );
		return 0;
	}

	while( true )
	{
		switch( tolower( getch() ) )
		{
		case 'q':
			{
				Exit();
				break;
			}
		}

		if( ! isRunning )
		{
			break;
		}
	};

	return 0;
}


//int APIENTRY WinMain(HINSTANCE hInstance,
//                     HINSTANCE hPrevInstance,
//                     LPSTR     lpCmdLine,
//                     int       nCmdShow)
//{
// 	// TODO: Place code here.
//	MSG msg;
//	HACCEL hAccelTable;
//
//	// Initialize global strings
//	LoadString(hInstance, IDS_APP_TITLE, szTitle, sizeof( szTitle ) );
//	LoadString(hInstance, IDC_MONITORINGSERVER, szWindowClass, sizeof( szWindowClass ) );
//	MyRegisterClass(hInstance);
//
//	// Perform application initialization:
//	if (!InitInstance (hInstance, nCmdShow)) 
//	{
//		return false;
//	}
//
//	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MONITORINGSERVER);
//	
//	// main recovery system
//	CMSSystem system;
//
//	// Main message loop:
//	while (GetMessage(&msg, NULL, 0, 0)) 
//	{
//		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
//	return msg.wParam;
//}
//
//
////
////  FUNCTION: MyRegisterClass()
////
////  PURPOSE: Registers the window class.
////
////  COMMENTS:
////
////    This function and its usage is only necessary if you want this code
////    to be compatible with Win32 systems prior to the 'RegisterClassEx'
////    function that was added to Windows 95. It is important to call this function
////    so that the application will get 'well formed' small icons associated
////    with it.
////
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//	WNDCLASSEX wcex;
//
//	wcex.cbSize = sizeof(WNDCLASSEX); 
//
//	wcex.style			= CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc	= (WNDPROC)WndProc;
//	wcex.cbClsExtra		= 0;
//	wcex.cbWndExtra		= 0;
//	wcex.hInstance		= hInstance;
//	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_MONITORINGSERVER);
//	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
//	wcex.lpszMenuName	= (LPCSTR)IDC_MONITORINGSERVER;
//	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//
//	return RegisterClassEx(&wcex);
//}
//
////
////   FUNCTION: InitInstance(HANDLE, int)
////
////   PURPOSE: Saves instance handle and creates main window
////
////   COMMENTS:
////
////        In this function, we save the instance handle in a global variable and
////        create and display the main program window.
////
//bool InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   hInst = hInstance; // Store instance handle in our global variable
//
//   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      //CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
//	  CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, hInstance, NULL);
//
//   if (!hWnd)
//   {
//      return false;
//   }
//
//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   //gMainHwnd = hWnd;
//
//   return true;
//}
//
////
////  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
////
////  PURPOSE:  Processes messages for the main window.
////
////  WM_COMMAND	- process the application menu
////  WM_PAINT	- Paint the main window
////  WM_DESTROY	- post a quit message and return
////
////
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	int wmId, wmEvent;
//	//PAINTSTRUCT ps;
//	//HDC hdc;
//	TCHAR szHello[ MAX_PATH ];
//	LoadString(hInst, IDS_HELLO, szHello, sizeof( szHello ) );
//
//	switch (message) 
//	{
//		case WM_COMMAND:
//			wmId    = LOWORD(wParam); 
//			wmEvent = HIWORD(wParam); 
//			// Parse the menu selections:
//			switch (wmId)
//			{
//				case IDM_ABOUT:
//				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
//				   break;
//				case IDM_EXIT:
//				   DestroyWindow(hWnd);
//				   break;
//				default:
//				   return DefWindowProc(hWnd, message, wParam, lParam);
//			}
//			break;
//			/*
//		case WM_PAINT:
//			hdc = BeginPaint(hWnd, &ps);
//			// TODO: Add any drawing code here...
//			RECT rt;
//			GetClientRect(hWnd, &rt);
//			if( g_pMSSystem )
//			{
//				const CNetwork::Address& address = NETWORK->GetAddress();
//
//				sprintf( szHello, "Recovery Server(%s:%d)", address.mIp, address.mPort );
//			}	
//			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
//			EndPaint(hWnd, &ps);
//			break;
//			*/
//		case WM_DESTROY:
//			PostQuitMessage(0);
//			break;
//		default:
//			return DefWindowProc(hWnd, message, wParam, lParam);
//   }
//   return 0;
//}
//
//// Mesage handler for about box.
//LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch (message)
//	{
//		case WM_INITDIALOG:
//				return true;
//
//		case WM_COMMAND:
//			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
//			{
//				EndDialog(hDlg, LOWORD(wParam));
//				return true;
//			}
//			else if(LOWORD(wParam) == IDC_BUTTON2)
//			{
//				//InitConsole();
//				return true;
//			}
//			break;
//	}
//    return false;
//}
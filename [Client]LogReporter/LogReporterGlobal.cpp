#include "StdAfx.h"
#include "resource.h"
#include "LogReporterGlobal.h"
#include "LogReporter.h"

#ifdef _FOR_CHINA_
	#include "SkinStatic.h"
	#include "LogReporterChinaDlg.h"
#else
	#include "LogReporterDlg.h"
#endif



/// Global variables

INT g_iTimer = 0;


BOOL IsSupportSSE()
{
	BOOL	bResult = TRUE;
	__try 
	{
		// 	xorps	xmm0,xmm0
        __asm _emit 0x0f 
		__asm _emit 0x57 
		__asm _emit 0xc0

    }
	__except ( EXCEPTION_EXECUTE_HANDLER) 
	{
		bResult = FALSE;
		
	}
	return bResult;
}



BOOL SelectGFunc()
{
	CopyFile(
	IsSupportSSE() ? _T("SS3DGFuncSSE.dll") : _T("SS3DGFuncN.dll"),
		_T("SS3DGFunc.dll"),
		FALSE);

	return TRUE;
}

void OnStartTimer(HWND hWnd) 
{
	//g_iTimer = SetTimer(1, 1000, 0);
	g_iTimer = SetTimer(hWnd, 1, 1000, 0);	
}

void OnStopTimer(HWND hWnd) 
{
	KillTimer(hWnd, g_iTimer);  
}

void ExecuteMHAutoPatchEXE(LPCTSTR pszExecuteFileName, LPCTSTR pszArg)
{
	//////////////////////////////////////////////////////////////////////////
	// 바꿀파일의 갯수
	// 090907 ShinJS --- DLL 추가
	const int arraySize = 6;
	//////////////////////////////////////////////////////////////////////////
	
	FAILPAIR files[ arraySize ];
#ifdef _DEBUG
	//*.new 파일은 릴리즈 파일이므로 디버그로 바꾸는 것은 오히려 문제를 발생시킬 수 있다.
	//files[temp++].Set( LAUNCHER_DEBUG,"LunaLauncher.new");
#else
	files[ 0 ].Set( LAUNCHER_RELEASE,"LunaLauncher.new");
#endif
	files[ 1 ].Set(
		_T("SS3DGFunc.dll"),
		_T("SS3DGFunc.new"));
	files[ 2 ].Set(
		_T("FileStorage.dll"),
		_T("FileStorage.new"));
	files[ 3 ].Set(
		_T("Executive.dll"),
		_T("Executive.new"));
	files[ 4 ].Set(
		_T("Geometry.dll"),
		_T("Geometry.new"));
	files[ 5 ].Set(
		_T("Renderer.dll"),
		_T("Renderer.new"));

	for(int n=0;n< arraySize;++n)
	{
		// TODO: Place code here.
		FILE* fp = _tfopen(
			files[n].newfilename,
			_T("rb"));

		if(fp != NULL)
		{
			fclose(fp);
			while(1)
			{
				if(DeleteFile(files[n].oldfilename) == TRUE)
					break;
				Sleep(500);
			}
			BOOL rt = MoveFile(files[n].newfilename,files[n].oldfilename);
			assert(rt);
		}
	}

	TCHAR szDir[MAX_PATH] = {0};

	GetCurrentDirectory(
		sizeof(szDir) / sizeof(*szDir),
		szDir);
	CString strFullPath;
	strFullPath.Format(
		_T("%s\\%s"),
		szDir,
		pszExecuteFileName);

	STARTUPINFO	sp = { 0 };
	sp.cb = sizeof(STARTUPINFO);
	sp.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE | STARTF_USESTDHANDLES; //STARTF_USESHOWWINDOW 
	sp.wShowWindow = SW_SHOW; 

	PROCESS_INFORMATION pi = {0};
	TCHAR CmdLine[MAX_PATH] = _T(" ");
#ifdef _LOCAL_JP_
	_tcscat(
		CmdLine,
		theApp.m_CmdLineFromPurple);
#else
	_tcscat(
		CmdLine,
		AfxGetApp()->m_lpCmdLine);
#endif

	CreateProcess( 
		_T("LunaLauncher.exe"),
		CmdLine,											// command line string
		0,													// SD
		0,													// SD
		FALSE,												// handle inheritance option
		CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,  // creation flags
		0,													// new environment block
		0,													// current directory name
		&sp,												// startup information
		&pi);												// process information
}
#ifndef _LOG_REPORTER_GLOBAL_
#define _LOG_REPORTER_GLOBAL_

#define LAUNCHER_RELEASE _T("LunaLauncher.exe")
#define LAUNCHER_DEBUG _T("LunaLauncher_d.exe")
#define CAPTION	_T("루나플러스")

struct FAILPAIR
{
	TCHAR oldfilename[MAX_PATH];
	TCHAR newfilename[MAX_PATH];

	void Set(LPCTSTR oldf, LPCTSTR newf)
	{
		_tcsncpy(
			oldfilename,
			oldf,
			sizeof(oldfilename) / sizeof(*oldfilename));
		_tcsncpy(
			newfilename,
			newf,
			sizeof(newfilename) / sizeof(*newfilename));
	}
};



/// Global variables

extern INT g_iTimer;


/// Function prototypes

BOOL IsSupportSSE();
BOOL SelectGFunc();
void OnStartTimer(HWND hWnd);
void OnStopTimer(HWND hWnd);
void ExecuteMHAutoPatchEXE(LPCTSTR pszExecuteFileName, LPCTSTR pszArg);

#endif
// LogReporter.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LogReporter.h"


#ifdef _FOR_CHINA_
	#include "LogReporterChinaDlg.h"
#else
	#include "LogReporterDlg.h"
#endif

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
// CLogReporterApp

BEGIN_MESSAGE_MAP(CLogReporterApp, CWinApp)
	//{{AFX_MSG_MAP(CLogReporterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLogReporterApp construction

CLogReporterApp::CLogReporterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLogReporterApp object

CLogReporterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLogReporterApp initialization

BOOL CLogReporterApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
#ifdef _LOCAL_JP_
	char buffer[256] = { 0 };
	strcpy(buffer, GetCommandLine() );
	if( strlen(buffer) > 0)
	{
		// 퍼플런처로부터의 인자를 런처실행시킬때 전달하기 위해서 저장한다.
		strcpy( m_CmdLineFromPurple, buffer );
	}

	const TCHAR* separator = _T( " " );
	// 런처위치 경로 획득
	const TCHAR* token = _tcstok( buffer, separator );
	// 핸들번호 획득
	token = _tcstok( 0, separator );
	// 루나설치 경로 획득
	token = _tcstok( 0, separator );
	CString installpath = token;
	// 기타인지 획득
	token = _tcstok( 0, separator );
	
	// 루나 설치경로가 존재하면 현재 디렉토리를 루나설치경로로 재설정한다.
	if(!installpath.IsEmpty())
	{
		CString fullPath = installpath;
		fullPath.Replace(_T("\\"),_T("\\\\"));
		fullPath.Remove('"');
		SetCurrentDirectory(fullPath);
	}

#endif

#ifdef _FOR_CHINA_
	
	CLogReporterChinaDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

#else
	CLogReporterDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

#endif
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

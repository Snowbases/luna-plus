// LogReporterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogReporterGlobal.h"
#include "LogReporter.h"
#include "LogReporterDlg.h"

/*

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogReporterDlg dialog

CLogReporterDlg::CLogReporterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogReporterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogReporterDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLogReporterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogReporterDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLogReporterDlg, CDialog)
	//{{AFX_MSG_MAP(CLogReporterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogReporterDlg message handlers

BOOL CLogReporterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	SetWindowText( CAPTION );

	CWnd* const window = GetDlgItem(
		IDC_STATIC);
	CRect rect;
	window->GetClientRect(
		rect);
	SetWindowPos(
		0,
		0,
		0,
		rect.Width(),
		rect.Height(),
		SWP_NOZORDER | SWP_NOREDRAW);
	SetWindowLong(
		GetSafeHwnd(),
		GWL_EXSTYLE,
		GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(
		0,
		0,
		LWA_COLORKEY);
	// TODO: Add extra initialization here
	SelectGFunc();

	m_dwStartTime = GetTickCount();
	m_bEnd = FALSE;
	IngValue = 1;
	//OnStartTimer();
	OnStartTimer(m_hWnd);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLogReporterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLogReporterDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();		
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLogReporterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLogReporterDlg::OnTimer(UINT nIDEvent) 
{
	if(TRUE == m_bEnd)
	{
		return;
	}

	const DWORD runSecond = 30000;

	if(GetTickCount() % runSecond * 2 > runSecond)
	{
		OnStopTimer(m_hWnd);
#ifdef _DEBUG
		ExecuteMHAutoPatchEXE(
			LAUNCHER_DEBUG,
			_T("a"));
#else
		ExecuteMHAutoPatchEXE(
			LAUNCHER_RELEASE,
			_T("a"));
#endif
		PostMessage(WM_CLOSE);
		m_bEnd = FALSE;
	}	

	CDialog::OnTimer(nIDEvent);
}
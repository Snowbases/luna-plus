// 4DyuchiGXFileExtractorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "4DyuchiGXFileExtractor.h"
#include "4DyuchiGXFileExtractorDlg.h"
#include "../4DyuchiGXGFunc/global.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "../4DyuchiGRX_Common/IExecutive.h"
I4DyuchiGXExecutive* 	g_pExecutive = NULL;


BOOL			CreateDataGroup(char* szFileNameFullPath);
DWORD __stdcall FileNotFoundHandleProc(ERROR_TYPE type,DWORD dwErrorPriority,void* pCodeAddress,char* szStr);


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
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy4DyuchiGXFileExtractorDlg dialog

CMy4DyuchiGXFileExtractorDlg::CMy4DyuchiGXFileExtractorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy4DyuchiGXFileExtractorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMy4DyuchiGXFileExtractorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy4DyuchiGXFileExtractorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMy4DyuchiGXFileExtractorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMy4DyuchiGXFileExtractorDlg, CDialog)
	//{{AFX_MSG_MAP(CMy4DyuchiGXFileExtractorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy4DyuchiGXFileExtractorDlg message handlers

BOOL CMy4DyuchiGXFileExtractorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
		
	CoInitialize(0);

	HRESULT hr;
	hr = CoCreateInstance(
       CLSID_4DyuchiGXExecutive,
       NULL,
       CLSCTX_INPROC_SERVER,
       IID_4DyuchiGXExecutive,
       (void**)&g_pExecutive);

	if (hr != S_OK)
		__asm int 3


	g_pExecutive->InitializeFileStorage(0,4096,0,FILE_ACCESS_METHOD_ONLY_FILE,NULL,0);
	g_pExecutive->Initialize(m_hWnd,NULL,
		4096,		// 오브젝트 최대갯수 
		0,			// 라이트 최대갯수 
		0,			// 트리거 최대갯수 
		0,			// 뷰포트 최대갯수 
		0,		// 데칼 최대갯수
		FileNotFoundHandleProc);		

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMy4DyuchiGXFileExtractorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMy4DyuchiGXFileExtractorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

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
HCURSOR CMy4DyuchiGXFileExtractorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

DWORD __stdcall GetPathFromFileName(char* dest,char* src)
{
	DWORD	dwLen;
	dwLen = 0;
	
	DWORD len_src = lstrlen(src);
	DWORD count = 0;
	DWORD i=0;

	for (i=len_src; i>0; i--)
	{
		if (src[i-1] == '\\')
		{
			dwLen = i;
			memcpy(dest,src,dwLen);
			goto lb_return;
		}
	}
	memcpy(dest,src,len_src);
	dwLen = len_src;

lb_return:
	dest[dwLen] = 0;
	return dwLen;

}
char	g_szSrcDataPath[_MAX_PATH];
void CMy4DyuchiGXFileExtractorDlg::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	char	szFileName[_MAX_PATH];
	char	szExt[8];

	int		count = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, szFileName, _MAX_PATH);

	
//	WriteConsole(m_hConsole,"Begin convering\n",lstrlen("Begin convering\n"),&dwWrittenSize,0);
	for(int i=0; i<count ; i++)
	{
		::DragQueryFile(hDropInfo, i, szFileName, _MAX_PATH);
		DWORD	dwSize = GetExt(szExt,szFileName);
		if (dwSize)
		{
			GetPathFromFileName(g_szSrcDataPath,szFileName);

			if (!lstrcmp(".mod",szExt))
			{
				CreateDataGroup(szFileName);
	
			}
			if (!lstrcmp(".MOD",szExt))
			{
				CreateDataGroup(szFileName);
				
			}
			if (!lstrcmp(".chr",szExt))
			{
				CreateDataGroup(szFileName);
				// mod 파일을 죽 찾아서...

			}
			if (!lstrcmp(".CHR",szExt))
			{
				CreateDataGroup(szFileName);

			}
			if (!lstrcmp(".chx",szExt))
			{
				CreateDataGroup(szFileName);


			}
			if (!lstrcmp(".CHX",szExt))
			{
				CreateDataGroup(szFileName);

			}
			return;



		}
	}
//	WriteConsole(m_hConsole,"End convering",lstrlen("End convering"),&dwWrittenSize,0);
	CDialog::OnDropFiles(hDropInfo);
}



DWORD __stdcall FileNotFoundHandleProc(ERROR_TYPE type,DWORD dwErrorPriority,void* pCodeAddress,char* szStr)
{
	if (type == ERROR_TYPE_FILE_NOT_FOUND)
	{
		char	szFileName[_MAX_PATH];
		// 패스제거
		GetNameRemovePath(szFileName,szStr);


		char	szSrcFileName[_MAX_PATH];
		// 카피해올 파일의 소스 패스명을 만든다.
		wsprintf(szSrcFileName,"%s%s",g_szSrcDataPath,szFileName);
	

		HANDLE hFile = CreateFile(szSrcFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			char txt[_MAX_PATH+128];
			wsprintf(txt,"%s 파일을 찾을수 없습니다.",szSrcFileName);
			MessageBox(NULL,txt,"Error",MB_OK);
			return 0;
		}
		CloseHandle(hFile);

		CopyFile(szSrcFileName,szFileName,TRUE);

		char	ext[8];
		GetExt(ext,szSrcFileName);
		if ( !lstrcmp(ext,".mod") || !lstrcmp(ext,".MOD") )
		{

			GXOBJECT_HANDLE	gxh = g_pExecutive->CreateGXObject(szFileName,NULL,0,0);
			if (gxh)
				g_pExecutive->DeleteGXObject(gxh);

		}
	}

	return 0;
	
}


BOOL CreateDataGroup(char* szFileNameFullPath)
{
	BOOL	bResult = FALSE;
	
	
	char	szDirName[_MAX_PATH];
	char	szFileName[_MAX_PATH];
	
	

	// 패스제거
	GetNameRemovePath(szFileName,szFileNameFullPath);

	// 현재 폴더 저장 
	char	szCurrentPath[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentPath);

	// 데이타 폴더로 이동
	SetCurrentDirectory(g_szSrcDataPath);

	// 데스티네이션 폴더를 만든다
	RemoveExt(szDirName,szFileName);
	CreateDirectory(szDirName,NULL);

	// 데스티네이션 폴더로 이동
	SetCurrentDirectory(szDirName);
	
	CopyFile(szFileNameFullPath,szFileName,TRUE);

	GXOBJECT_HANDLE	gxh = g_pExecutive->CreateGXObject(szFileName,NULL,0,0);
	if (!gxh)
	{

		// 실패하면 한번 더 해주자.CHR,CHX인 경우 해당mod가 없으면 실패하기 때문에 MOD관련 데이타는 
		// 카피해오겠지만 ANM까지 가져올순 없다.
		gxh = g_pExecutive->CreateGXObject(szFileName,NULL,0,0);
		
	}
	if (gxh)
		g_pExecutive->DeleteGXObject(gxh);

	SetCurrentDirectory(szCurrentPath);

lb_return:
	return bResult;
}

void CMy4DyuchiGXFileExtractorDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if (g_pExecutive)
	{
		g_pExecutive->Release();
		g_pExecutive = NULL;
	}

	CoUninitialize();
	CDialog::OnClose();
}

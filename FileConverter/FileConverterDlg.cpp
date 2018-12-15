// FileConverterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FileConverter.h"
#include "FileConverterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "../4DyuchiGXGFunc/global.h"
#pragma comment(lib, "d:\\4DyuchiDLL_MUK\\SS3DGFunc.lib")

#define CURRENT_VERSION		0x00000001



/////////////////////////////////////////////////////////////////////////////
// CFileConverterDlg dialog

CFileConverterDlg::CFileConverterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileConverterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileConverterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CFileConverterDlg::~CFileConverterDlg()
{
	Sleep(100);
}
void CFileConverterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileConverterDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileConverterDlg, CDialog)
	//{{AFX_MSG_MAP(CFileConverterDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileConverterDlg message handlers

BOOL CFileConverterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	DragAcceptFiles(TRUE);
	AllocConsole();
	m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

//	DWORD	dwWrittendSize;
//	WriteConsole(m_hConsole,"hohohohho",lstrlen("hohohohho"),&dwWrittendSize,0);
	
	CoInitialize(NULL);

	HRESULT hr;

	BOOL	bResult = FALSE;

	hr = CoCreateInstance(
           CLSID_4DyuchiFileStorage,
           NULL,
           CLSCTX_INPROC_SERVER,
           IID_4DyuchiFileStorage,
           (void**)&m_pFileStorage);

	if (hr != S_OK)
	{
		::MessageBox(NULL,"Fail to Create 4DyuchiFileStorage","Error",MB_OK);
		return FALSE;
	}

    hr = CoCreateInstance(
			CLSID_4DyuchiGXRenderer,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_4DyuchiGXRenderer,
			(void**)&m_pRenderer);

	if (hr != S_OK)
	{
		::MessageBox(NULL,"Fail to Create 4DyuchiGXRenderer","Error",MB_OK);
		return FALSE;
	}

	
/*	hr = CoCreateInstance(
           CLSID_4DyuchiGXGeometry,
           NULL,
           CLSCTX_INPROC_SERVER,
           IID_4DyuchiGXGeometry,
           (void**)&m_pGeometry);


	if (hr != S_OK)
	{
		MessageBox(NULL,"Fail to Create 4DyuchiGXGeometry","Error",MB_OK);
		return FALSE;
	}*/

	// 파일 스토리지 초기화
	m_pFileStorage->Initialize(0,4096,_MAX_PATH,FILE_ACCESS_METHOD_ONLY_FILE);

	// 3D 디바이스 생성
	m_pRenderer->Create(m_hWnd,NULL,m_pFileStorage,NULL);
	
	// 지오메트리 초기화
//	m_pGeometry->Initialize(m_pRenderer,m_pFileStorage,16,MAX_FILEITEM_NUM,MAX_FILEBUCKET_NUM,NULL);	// 초기화

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileConverterDlg::OnPaint() 
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
HCURSOR CFileConverterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFileConverterDlg::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	char	szFileName[_MAX_PATH];
	char	szExt[8];

	int		count = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, szFileName, _MAX_PATH);

	DWORD	dwWrittenSize;
	WriteConsole(m_hConsole,"Begin convering\n",lstrlen("Begin convering\n"),&dwWrittenSize,0);
	for(int i=0; i<count ; i++)
	{
		::DragQueryFile(hDropInfo, i, szFileName, _MAX_PATH);
		DWORD	dwSize = GetExt(szExt,szFileName);
		if (dwSize)
		{
			if (!lstrcmp(".tga",szExt))
			{
				ConvertTGA(szFileName);
				continue;
			}
			if (!lstrcmp(".TGA",szExt))
			{
				ConvertTGA(szFileName);
				continue;
			}
			if (!lstrcmp(".tif",szExt))
			{
				ConvertTGA(szFileName);
				continue;
			}
			if (!lstrcmp(".TIF",szExt))
			{
				ConvertTGA(szFileName);
				continue;
			}

			if (!lstrcmp(".mod",szExt))
			{
				ConvertMOD(szFileName);
				continue;
			}
			if (!lstrcmp(".MOD",szExt))
			{
				ConvertMOD(szFileName);
				continue;
			}
			
			if (!lstrcmp(".hfl",szExt))
			{
				ConvertHFL(szFileName);
				continue;
			}
			if (!lstrcmp(".HFL",szExt))
			{
				ConvertHFL(szFileName);
				continue;
			}

			if (!lstrcmp(".stm",szExt))
			{
				ConvertSTM(szFileName);
				continue;
			}
			if (!lstrcmp(".STM",szExt))
			{
				ConvertSTM(szFileName);
				continue;
			}

		}
	}
	WriteConsole(m_hConsole,"End convering",lstrlen("End convering"),&dwWrittenSize,0);
	CDialog::OnDropFiles(hDropInfo);
}
void CFileConverterDlg::ConvertSTM(char* szFileName)
{

	ConvertHFL(szFileName);
}
void CFileConverterDlg::ConvertTGA(char* szFileName)
{
	char	txt[_MAX_PATH];
	memset(txt,0,sizeof(txt));

	
	
	BOOL	bResult = m_pRenderer->ConvertCompressedTexture(szFileName,0);
	

	if (bResult)
	{
		wsprintf(txt,"%s - Converted to CompressedTexture Successfully!!! \n",szFileName);
	}
	else
	{
		wsprintf(txt,"%s - Fail to Converting \n",szFileName);
	}
	
	
	DWORD	dwWrittenSize;
	WriteConsole(m_hConsole,txt,lstrlen(txt),&dwWrittenSize,0);

}
void CFileConverterDlg::ConvertMOD(char* szFileName)
{
}
void CFileConverterDlg::ConvertANM(char* szFileName)
{
}
void CFileConverterDlg::ConvertHFL(char* szFileName)
{
	DWORD	dwOrigSize,dwReadSize,dwWriteSize;
	char*	pBuffer;
	HANDLE hFile = CreateFile(szFileName,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		goto lb_return;

	dwOrigSize = GetFileSize(hFile,NULL);
	pBuffer = new char[dwOrigSize+8];
	ReadFile(hFile,pBuffer+4,dwOrigSize,&dwReadSize,NULL);
	
			
	//버젼 기록
	*(DWORD*)pBuffer = CURRENT_VERSION;
	
	// 끝에 프로퍼티 갯수
	*(DWORD*)(pBuffer+dwOrigSize+4) = 0;

	SetFilePointer(hFile,0,NULL,FILE_BEGIN);
	WriteFile(hFile,pBuffer,dwOrigSize+8,&dwWriteSize,NULL);

	CloseHandle(hFile);
	
	delete [] pBuffer;


	char	txt[_MAX_PATH];
	memset(txt,0,sizeof(txt));
	wsprintf(txt,"%s Converted to New Verstion. \n",szFileName);
	DWORD	dwWrittenSize;
	WriteConsole(m_hConsole,txt,lstrlen(txt),&dwWrittenSize,0);
lb_return:
	return;
}
BOOL CFileConverterDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::PreCreateWindow(cs);
}

void CFileConverterDlg::OnDestroy() 
{
	FreeConsole();

	if (m_pFileStorage)
	{
		m_pFileStorage->Release();
		m_pFileStorage = NULL;
	}
	if (m_pRenderer)
	{
		m_pRenderer->Release();
		m_pRenderer = NULL;
		
	}
	
	CDialog::OnDestroy();

	CoUninitialize();

	// TODO: Add your message handler code here
	
}

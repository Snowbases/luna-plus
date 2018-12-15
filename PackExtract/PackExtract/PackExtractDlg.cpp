// PackExtractDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "PackExtract.h"
#include "PackExtractDlg.h"
#include ".\packextractdlg.h"
#include "ExtractSelectedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPackExtractDlg* g_pPackExtractDlg;

UINT CallExtractPackFile( LPVOID pParam )
{
	CPackExtractDlg* pDlg = (CPackExtractDlg*)pParam;
	if( pDlg )
		pDlg->ExtractPackFile();

	return 0;
}

UINT CallCompressPackFile( LPVOID pParam )
{
	CPackExtractDlg* pDlg = (CPackExtractDlg*)pParam;
	if( pDlg )
		pDlg->CompressPackFile();

	return 0;
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPackExtractDlg 대화 상자



CPackExtractDlg::CPackExtractDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPackExtractDlg::IDD, pParent)
	, m_bDeleteSource(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	ZeroMemory( m_szBaseDirectory, MAX_PATH );

	m_pidlDesktop = NULL;
	m_pDesktopFolder = NULL;
	SHGetDesktopFolder(&m_pDesktopFolder);
	m_pMalloc = NULL;
	SHGetMalloc(&m_pMalloc);

	m_pStorage = NULL;

	CMiniDump::Begin();
}

CPackExtractDlg::~CPackExtractDlg()
{
	if (m_pidlDesktop != NULL)
		m_pMalloc->Free(m_pidlDesktop);
	if (m_pMalloc != NULL)
		m_pMalloc->Release();
	if (m_pDesktopFolder != NULL)
		m_pDesktopFolder->Release();

	if( m_pStorage )
		m_pStorage->Release();

	m_vPackFile.clear();
	m_vCompressFolder.clear();
	m_vSelectedPackFileList.clear();

	CMiniDump::End();
}

void CPackExtractDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PACK_LIST, m_PackListBox);
	DDX_Control(pDX, IDC_PROGRESS_LIST, m_ListProgress);
	DDX_Control(pDX, IDC_PROGRESS_PACK, m_PackProgress);
	DDX_Control(pDX, IDC_TREE1, m_ExtractFolderPos);
	DDX_Control(pDX, IDC_MODE_EXTRACT_RADIO, m_ModeExtractBtn);
	DDX_Control(pDX, IDC_MODE_COMPRESS_RADIO, m_ModeCompressBtn);
	DDX_Check(pDX, IDC_DELETE_AFTER_PROC_CHECK, m_bDeleteSource);
	DDX_Control(pDX, IDC_EXTRACT_FILENAME_BUTTON1, m_ExtractFileNameInputBtn);
}

BEGIN_MESSAGE_MAP(CPackExtractDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_EXTRACT_PACKFILE_BUTTON, OnBnClickedExtractPackfileButton)
	ON_BN_CLICKED(ID_DELETE_PACKFILE_BUTTON, OnBnClickedDeletePackfileButton)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE1, OnTvnItemexpandedTree1)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, OnTvnItemexpandingTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_MODE_EXTRACT_RADIO, OnBnClickedModeExtractRadio)
	ON_BN_CLICKED(IDC_MODE_COMPRESS_RADIO, OnBnClickedModeCompressRadio)
	ON_BN_CLICKED(IDC_DELETE_AFTER_PROC_CHECK, OnBnClickedDeleteAfterProcCheck)
	ON_MESSAGE( UM_UPDATEDATA, OnUpdateDlg )
	ON_BN_CLICKED(IDC_EXTRACT_FILENAME_BUTTON1, OnBnClickedExtractFilenameButton1)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_HELP_BUTTON, OnBnClickedHelpButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_LBN_DBLCLK(IDC_PACK_LIST, OnLbnDblclkPackList)
END_MESSAGE_MAP()


// CPackExtractDlg 메시지 처리기

BOOL CPackExtractDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	GetCurrentDirectory( MAX_PATH, m_szBaseDirectory );

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	HMODULE hStorage = LoadLibrary( "FileStorage.dll" );
	if( hStorage )
	{
		CREATE_INSTANCE_FUNC	pFunc;
		pFunc = (CREATE_INSTANCE_FUNC)GetProcAddress(hStorage,"DllCreateInstance");
		HRESULT hr = pFunc((void**)&m_pStorage);
		if (hr != S_OK)
		{
			AfxMessageBox( "Failed LoadLibrary() - FileStorage.dll" );
			SendMessage( WM_CLOSE );
			return FALSE;
		}
	}
	else
	{
		AfxMessageBox( "Failed LoadLibrary() - FileStorage.dll" );
		SendMessage( WM_CLOSE );
		return FALSE;
	}

	m_pStorage->Initialize( MAX_FILE_ATOM_NUM, 4096, 128, FILE_ACCESS_METHOD_ONLY_FILE );

	/////////////////////////////////////////////////////////////
	// Tree Ctrl
	//- Desktop 의 PIDL를 얻는다. (최상위 PIDL)
	//- 폴더 아이콘을 위하여 System Image List 를 Setting 한다.
	//- Desktop 상의 아이템들을 Tree View Item 으로 등록한다.
	//- Tree View 의 초기 Item Expand (확장)을 실행한다.
	/////////////////////////////////////////////////////////////

	CTreeCtrl& Tree = m_ExtractFolderPos;
	///////////////////////////////////////////////////
	// Get Desktop PIDL
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &m_pidlDesktop);
	///////////////////////////////////////////////////
	// Set Image List
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	HIMAGELIST hSysImageList = (HIMAGELIST) SHGetFileInfo((LPCTSTR)m_pidlDesktop, 0,
		&sfi, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	m_IconImageList.Attach( hSysImageList );
	Tree.SetImageList( &m_IconImageList, TVSIL_NORMAL );    
//	TreeView_SetImageList(this->m_hWnd, hSysImageList, TVSIL_NORMAL);
	///////////////////////////////////////////////////
	// Insert Desktop Folder as a Tree View Item
	// to get file info
	UINT uFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON
		| SHGFI_DISPLAYNAME;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	SHGetFileInfo((LPCTSTR) m_pidlDesktop, 0, &sfi, sizeof(SHFILEINFO), uFlags);
	TVINSERTSTRUCT tvi;
	ZeroMemory(&tvi, sizeof(TVINSERTSTRUCT));
	tvi.hParent = TVI_ROOT;
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.pszText = sfi.szDisplayName;
	tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
	tvi.item.iImage = tvi.item.iSelectedImage = sfi.iIcon;
	tvi.item.cChildren = 1;
	tvi.item.lParam = (LPARAM) m_pidlDesktop;

	m_hDesktopItem = Tree.InsertItem(&tvi);
	///////////////////////////////////////////////////
	// Expand desktop folder
	EnumChildItems(m_pDesktopFolder, m_pidlDesktop, m_hDesktopItem);
	Tree.Expand(m_hDesktopItem, TVE_EXPAND);


	// Mode 설정
	m_eMode = ePackMode_Extract;
	m_ModeExtractBtn.SetCheck( TRUE );
	m_ModeCompressBtn.SetCheck( FALSE );
	m_bIsWorking = FALSE;

	g_pPackExtractDlg = this;
	m_pSelectedPackFileHandle = NULL;


	// 명령어 인자가 있는 경우
	if( __argc > 1 )
	{
		CString strPath = __argv[1];

		// 파일경로에서 폴더경로만 남긴다.
		strPath = strPath.Left( strPath.ReverseFind( '\\' ) );

		CString strFileName = __argv[1];
		strFileName.Delete( 0, strFileName.ReverseFind( '\\' ) + 1 );

		// pak 파일이 아닌경우
		if( strstr( (LPCTSTR)strFileName, ".pak" ) == NULL )
		{
			UpdateTreePath( m_szBaseDirectory );
		}
		else
		{
			// List에 넣을수 있도록 설정한다.(pak파일 선택 모드인 경우에만)
			UpdateTreePath( strPath );

			m_PackListBox.ResetContent();
			m_vPackFile.clear();

			m_PackListBox.AddString( strFileName );
			m_vPackFile.push_back( strFileName );
		}
	}
	else
	{
		// Tree를 실행경로로 설정한다.
		UpdateTreePath( m_szBaseDirectory );
	}
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CPackExtractDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CPackExtractDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CPackExtractDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPackExtractDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage( WM_NCLBUTTONDOWN, HTCAPTION, 0 );
	CDialog::OnLButtonDown(nFlags, point);
}

void CPackExtractDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	SendMessage( WM_NCLBUTTONUP, HTCAPTION, 0 );
	CDialog::OnLButtonUp(nFlags, point);
}


void CPackExtractDlg::OnBnClickedExtractPackfileButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_bIsWorking )
	{
		AfxMessageBox( "작업이 종료될때까지 기다려 주십시오!" );
		return;
	}

	switch( m_eMode )
	{
	case ePackMode_Extract:
		if( m_vPackFile.empty() )
			return;

		AfxBeginThread( CallExtractPackFile, (LPVOID)this );
		break;
	case ePackMode_Compress:
		if( m_vCompressFolder.empty() )
			return;

		AfxBeginThread( CallCompressPackFile, (LPVOID)this );
		break;
	}
}

void CPackExtractDlg::Release()
{
	m_PackListBox.ResetContent();
	m_vPackFile.clear();
	m_vCompressFolder.clear();

	m_ListProgress.SetPos( 0 );
	m_PackProgress.SetPos( 0 );
}

BOOL CPackExtractDlg::GetSelectedFolderByTree( char* pDirName, DWORD dwDirNameLen )
{
	// No Selected Folder
	HTREEITEM hSelectedItem = m_ExtractFolderPos.GetSelectedItem();
	if( hSelectedItem == NULL )
		return FALSE;

	// Get Current Selected Folder
	ZeroMemory( pDirName, dwDirNameLen );
	LPITEMIDLIST pidl = (LPITEMIDLIST) m_ExtractFolderPos.GetItemData( hSelectedItem );
	SHGetPathFromIDList( pidl, pDirName );

	return TRUE;
}

void CPackExtractDlg::LoadFolderFile( LPCTSTR strLoadPath, LPCTSTR strFileFilter, vector<CString>& vecName, BOOL bLoadDirectory, BOOL bFullPath )
{
	char szBaseDir[ MAX_PATH ] = {0,};
	GetCurrentDirectory( MAX_PATH, szBaseDir );
	if( !SetCurrentDirectory( strLoadPath ) )
		return;

	HANDLE hFindFile = NULL;
	WIN32_FIND_DATA FindFileData = { 0, };	
    
	hFindFile = ::FindFirstFile( strFileFilter, &FindFileData );
	if( hFindFile == INVALID_HANDLE_VALUE )
	{
		SetCurrentDirectory( szBaseDir );
		return;
	}

	// 첫번째 파일 저장
	if( !(FindFileData.cFileName == _T( "." ) || _tcscmp( FindFileData.cFileName, _T( ".." ) ) == 0 ) &&
		!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		// 절대경로 저장
		if( bFullPath )
		{
			CString strPath( strLoadPath );
			strPath.AppendFormat( "\\%s", FindFileData.cFileName );
			vecName.push_back( strPath );
		}
		else
		{
			CString str( FindFileData.cFileName );
			vecName.push_back( str );
		}
	}

	while( ::FindNextFile( hFindFile, &FindFileData ) )
	{
		if( FindFileData.cFileName == _T( "." ) ||
			_tcscmp( FindFileData.cFileName, _T( ".." ) ) == 0 )
		{
			continue;
		}
		else if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
				bLoadDirectory )
		{			
			// 절대경로 저장
			if( bFullPath )
			{
				CString strPath( strLoadPath );
				strPath.AppendFormat( "\\%s", FindFileData.cFileName );
				vecName.push_back( strPath );
			}
			else
			{
				CString str( FindFileData.cFileName );
				vecName.push_back( str );
			}
				
		}
		else if( !(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				!bLoadDirectory )
		{
			// 절대경로 저장
			if( bFullPath )
			{
				CString strPath( strLoadPath );
				strPath.AppendFormat( "\\%s", FindFileData.cFileName );
				vecName.push_back( strPath );
			}
			else
			{
				CString str( FindFileData.cFileName );
				vecName.push_back( str );
			}
		}
	}

	::FindClose( hFindFile );

	SetCurrentDirectory( szBaseDir );
}

void CPackExtractDlg::RemoveFileInDir( LPCTSTR strDirPath )
{
	CFileFind finder;
	BOOL bWorking = TRUE;
	CString strDirFile( strDirPath );
	strDirFile.Append( "\\*.*" );
	bWorking = finder.FindFile(strDirFile);
	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDots()) continue;
		if(finder.IsDirectory())    RemoveDir(finder.GetFilePath());
		else    ::DeleteFile(finder.GetFilePath());
	}
	finder.Close();
}

void CPackExtractDlg::RemoveDir( LPCTSTR strDirPath )
{
	RemoveFileInDir( strDirPath );

	::RemoveDirectory( strDirPath );
}

void CPackExtractDlg::LoadPackFile()
{
	if( !GetSelectedFolderByTree( m_szCurDirectory, MAX_PATH ) )
	{
		return;
	}

	Release();

	// ".pak" File Load
	LoadFolderFile( m_szCurDirectory, "*.pak", m_vPackFile, FALSE, FALSE );

	// Load된 File ListBox에 등록
	vector<CString>::iterator iter;
	for( iter = m_vPackFile.begin() ; iter != m_vPackFile.end() ; ++iter )
	{
		m_PackListBox.AddString( *iter );
	}

	UpdateData( FALSE );
}

void CPackExtractDlg::LoadCompressFolder()
{
	if( !GetSelectedFolderByTree( m_szCurDirectory, MAX_PATH ) )
	{
		return;
	}

	Release();

	// Directory Load For ListBox
	LoadFolderFile( m_szCurDirectory, "*.*", m_vCompressFolder, TRUE, FALSE );

	// Load된 File ListBox에 등록
	vector<CString>::iterator iter;
	for( iter = m_vCompressFolder.begin() ; iter != m_vCompressFolder.end() ; ++iter )
	{
		m_PackListBox.AddString( *iter );
	}
}

void CPackExtractDlg::ExtractPackFile()
{
	m_bIsWorking = TRUE;

	// Extract Pack File
	int nListProgressPos, nPackProgressPos;
	nListProgressPos = nPackProgressPos = 0;

	m_ListProgress.SetRange( 0, (short)m_vPackFile.size() );

	vector<CString>::iterator iter;
	for( iter = m_vPackFile.begin() ; iter != m_vPackFile.end() ; ++iter )
	{
		// Create Folder
		CString strDirectory( m_szCurDirectory );
		strDirectory.AppendFormat( "\\%s", *iter );
		strDirectory.Delete( strDirectory.Find( ".pak" ), 4 );

		if( !CreateDirectory( strDirectory, NULL ) )
		{
			switch( GetLastError() )
			{
			case ERROR_ALREADY_EXISTS:
				{
					// Folder 존재시 폴더안의 파일 지우기
					RemoveFileInDir( strDirectory );
					break;
				}
			default:
				{
					__asm int 3;
					AfxMessageBox( "Failed CreateDirectory() !" );
					return;
				}
			}
		}

		SetCurrentDirectory( strDirectory );

		// File Extract
		CString strPackFile( m_szCurDirectory );
		strPackFile.AppendFormat( "\\%s", *iter );
		void*	pPackFileHandle = NULL;
		pPackFileHandle = m_pStorage->MapPackFile( (LPSTR)(LPCSTR)strPackFile, TRUE );
		if (!pPackFileHandle)
			break;

		FSPACK_FILE_INFO	fileInfo;
		FSFILE_ATOM_INFO*	pFileInfoList;
		DWORD dwFileInfoNum;
		m_pStorage->GetPackFileInfo(pPackFileHandle,&fileInfo);
		if(pPackFileHandle)
		{
			dwFileInfoNum = m_pStorage->CreateFileInfoList(pPackFileHandle,&pFileInfoList,MAX_FILE_ATOM_NUM);

			m_PackProgress.SetRange( 0, (short)dwFileInfoNum );
			nPackProgressPos = 0;

			// 파일 순회
			for (DWORD i=0; i<dwFileInfoNum; i++)
			{
				m_pStorage->ExtractFile( pFileInfoList[i].pszFileName );

				m_PackProgress.SetPos( ++nPackProgressPos );

				SendMessage( UM_UPDATEDATA );
			}

		}

		m_pStorage->UnmapPackFile( pPackFileHandle );

		m_ListProgress.SetPos( ++nListProgressPos );

		SendMessage( UM_UPDATEDATA );

		if( m_bDeleteSource )
		{
			::DeleteFile( strPackFile );
		}
	}

	m_bIsWorking = FALSE;

	SetCurrentDirectory( m_szCurDirectory );

	AfxMessageBox( "압축해제 완료" );

	Release();
}

void CPackExtractDlg::OpenExtractPackFileWithSelectListDlg( CString strSelectedPackFile )
{
	m_bIsWorking = TRUE;

	// PackFile Open
	CString strPackFile( m_szCurDirectory );
	strPackFile.AppendFormat( "\\%s", strSelectedPackFile );	
	m_pSelectedPackFileHandle = m_pStorage->MapPackFile( (LPSTR)(LPCSTR)strPackFile, TRUE );
	if (!m_pSelectedPackFileHandle)
	{
		return;
	}

	m_strSelectedPackFileName = strSelectedPackFile;

	ExtractSelectedDlg SelectedDlg;

	FSPACK_FILE_INFO	fileInfo;
	FSFILE_ATOM_INFO*	pFileInfoList;
	DWORD dwFileInfoNum;
	m_pStorage->GetPackFileInfo( m_pSelectedPackFileHandle, &fileInfo );
	if( m_pSelectedPackFileHandle )
	{
		dwFileInfoNum = m_pStorage->CreateFileInfoList( m_pSelectedPackFileHandle, &pFileInfoList, MAX_FILE_ATOM_NUM );

		// 파일 순회
		for (DWORD i=0; i<dwFileInfoNum; i++)
		{
			// 파일 이름 저장
			CString str( pFileInfoList[i].pszFileName );
			m_vSelectedPackFileList.push_back( str );
		}
	}

	if( SelectedDlg.DoModal() == IDOK )
	{
	}
}

void CPackExtractDlg::AddListFromSelectedPackFile( CListBox& listbox )
{
	vector<CString>::iterator iter;
	for( iter = m_vSelectedPackFileList.begin() ; iter != m_vSelectedPackFileList.end() ; ++iter )
	{
		listbox.AddString( *iter );
	}
}

void CPackExtractDlg::StartExtractPackFileWithSelectListDlg( BOOL bClearFoloder )
{
	// Create Folder
	CString strDirectory( m_szCurDirectory );
	strDirectory.AppendFormat( "\\%s", m_strSelectedPackFileName );
	strDirectory.Delete( strDirectory.Find( ".pak" ), 4 );

	if( !CreateDirectory( strDirectory, NULL ) )
	{
		switch( GetLastError() )
		{
		case ERROR_ALREADY_EXISTS:
			{
				// Folder 존재시 폴더안의 파일 지우기
				if( bClearFoloder )
					RemoveFileInDir( strDirectory );
				break;
			}
		default:
			{
#ifdef _DEBUG
				__asm int 3;
#endif
				AfxMessageBox( "Failed CreateDirectory() !" );
				return;
			}
		}
	}

	SetCurrentDirectory( strDirectory );
}

void CPackExtractDlg::EndExtractPackFileWithSelectListDlg()
{
	m_vSelectedPackFileList.clear();
	SetCurrentDirectory( m_szCurDirectory );

	if( !m_pSelectedPackFileHandle )
		return;

	m_bIsWorking = FALSE;

	m_pStorage->UnmapPackFile( m_pSelectedPackFileHandle );

	m_pSelectedPackFileHandle = NULL;
}

BOOL CPackExtractDlg::StartDeletePackFileWithSelectListDlg()
{
	return m_pStorage->LockPackFile( m_pSelectedPackFileHandle, 0 );
}

void CPackExtractDlg::EndDeletePackFileWithSelectListDlg()
{
	m_dwUnlockFileProcTotalNum = 0;
	m_pStorage->UnlockPackFile( m_pSelectedPackFileHandle, CallUnlockFileProc );
}

void CPackExtractDlg::ExtractPackFile( char* szExtractFileName )
{
	m_pStorage->ExtractFile( szExtractFileName );
}

void CPackExtractDlg::DeletePackFile( char* szExtractFileName )
{
	m_pStorage->DeleteFileFromPackFile( szExtractFileName );
}

void CPackExtractDlg::CompressPackFile()
{
	m_bIsWorking = TRUE;

	int nListProgressPos = 0;
	m_ListProgress.SetRange( 0, (short)m_vCompressFolder.size() );

	vector<CString>::iterator iter;
	for( iter = m_vCompressFolder.begin() ; iter != m_vCompressFolder.end() ; ++iter )
	{
		CompressFile( *iter );
		
		m_ListProgress.SetPos( ++nListProgressPos );
	}

	SetCurrentDirectory( m_szCurDirectory );

	AfxMessageBox( "압축 완료!" );

	m_bIsWorking = FALSE;

	Release();
}

void CPackExtractDlg::CompressFile( CString strFolderName )
{
	SetCurrentDirectory( m_szCurDirectory );

	// 전체 파일 읽기
	CString strPath( m_szCurDirectory );
	CString strPackName( m_szCurDirectory );
	strPath.AppendFormat( "\\%s", strFolderName );
	strPackName.AppendFormat( "\\%s.pak", strFolderName );

	// Folder 내 파일 읽기
	vector<CString> vecFileName;
	LoadFolderFile( strPath, "*.*", vecFileName, FALSE, TRUE );
	
	void*	pPackFileHandle = NULL;
	pPackFileHandle = m_pStorage->MapPackFile( (LPSTR)(LPCSTR)strPackName, TRUE );
	if (!pPackFileHandle)
	{
		m_pStorage->UnmapPackFile( pPackFileHandle );
		return;
	}

	m_dwUnlockFileProcTotalNum = 0;

	m_pStorage->LockPackFile( pPackFileHandle, 0 );
	
	vector<CString>::iterator iter;
	for( iter = vecFileName.begin() ; iter != vecFileName.end() ; ++iter )
	{
		if( m_pStorage->IsExistInFileStorage( (LPSTR)(LPCTSTR)*iter ) )
		{
			char msg[512] = {0,};
			sprintf( msg, "[ 존재하는 파일 %s.pak, %s ]", strFolderName, *iter );
			OutputDebugString( msg );

			m_pStorage->DeleteFileFromPackFile( (LPSTR)(LPCTSTR)*iter );
		}
		m_pStorage->InsertFileToPackFile( pPackFileHandle, (LPSTR)(LPCTSTR)*iter );
	}

	m_pStorage->UnlockPackFile( pPackFileHandle, CallUnlockFileProc );
	m_pStorage->UnmapPackFile( pPackFileHandle );

	// 폴더, 파일 제거
	if( m_bDeleteSource )
	{
		RemoveDir( strPath );
	}
}

DWORD __stdcall CallUnlockFileProc( DWORD dwCurCount, DWORD dwTotalCount, void* pArg )
{
	if( g_pPackExtractDlg )
		g_pPackExtractDlg->UnlockFileProc( dwCurCount, dwTotalCount, pArg );

	return 0;
}

void CPackExtractDlg::UnlockFileProc( DWORD dwCurCount, DWORD dwTotalCount, void* pArg )
{
	if( m_dwUnlockFileProcTotalNum == 0 )
	{
		m_dwUnlockFileProcTotalNum = dwTotalCount;
		m_PackProgress.SetRange( 0, (short)dwTotalCount );
	}

	m_PackProgress.SetPos( dwCurCount + 1 );

	SendMessage( UM_UPDATEDATA );
}

void CPackExtractDlg::OnBnClickedDeletePackfileButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_PackListBox.GetCurSel();
	int nCount = m_PackListBox.GetSelCount();

	CArray<int,int> aryListBoxSel;
	CArray<CString> arySelectedFileName;

	aryListBoxSel.SetSize( nCount );
	m_PackListBox.GetSelItems( nCount, aryListBoxSel.GetData() );
	for( int i=0; i < nCount ; ++i )
	{
		CString strFileName;
		m_PackListBox.GetText( aryListBoxSel[i], strFileName );
		arySelectedFileName.Add( strFileName );
	}

	if( (nIndex != LB_ERR) && (nCount > 0) )
	{
		switch( m_eMode )
		{
		case ePackMode_Extract:
			{
				for( int i=0; i < nCount ; ++i )
				{
					CString strFileName = arySelectedFileName[i];
					// 해당 파일 List 에서 제거
					vector<CString>::iterator iter;
					for( iter = m_vPackFile.begin() ; iter != m_vPackFile.end() ; ++iter )
					{
						if( iter->Compare( arySelectedFileName[i] ) == 0 )
						{
							nIndex = m_PackListBox.FindString( -1, arySelectedFileName[i] );
							m_PackListBox.DeleteString( nIndex );
							m_vPackFile.erase( iter );
							break;
						}
					}				
				}
			}
			break;
		case ePackMode_Compress:
			{
				for( int i=0; i < nCount ; ++i )
				{
					CString strFileName = arySelectedFileName[i];

					// 해당 파일 List 에서 제거
					vector<CString>::iterator iter;
					for( iter = m_vCompressFolder.begin() ; iter != m_vCompressFolder.end() ; ++iter )
					{
						if( iter->Compare( strFileName ) == 0 )
						{
							nIndex = m_PackListBox.FindString( -1, strFileName );
							m_PackListBox.DeleteString( nIndex );
							m_vCompressFolder.erase( iter );
							break;
						}
					}
				}
			}
			break;
		}	

		
	}

	UpdateData( FALSE );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EnumChildItems()
//		- 상위(부모) IShellFolder pointer 와 PIDL, HTREEITEM을 받고 안에 있는 Child Item 들을 Tree 에 등록한다.
//		* 처음부터 모든 디렉토리, 서브디렉토리를 읽고 Tree Item 을 등록하지 않는다. [+] 버튼을 누르게 되는 시점에서 필요시 등록한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPackExtractDlg::EnumChildItems(LPSHELLFOLDER pFolder, LPITEMIDLIST pParent, HTREEITEM hParent)
{
	LPENUMIDLIST pEnumIDList = NULL;
	LPITEMIDLIST pItem = NULL;
	LPITEMIDLIST pidlAbs = NULL;
	ULONG ulFetched = 0;
	TVINSERTSTRUCT tvi;
	if (pFolder->EnumObjects(this->m_hWnd, SHCONTF_FOLDERS, &pEnumIDList) == NOERROR)
	{
		while (pEnumIDList->Next(1, &pItem, &ulFetched) == NOERROR)
		{
			// to memorize absolute PIDL
			pidlAbs = ILAppend(pParent, pItem);
			// to set tree view item
			ZeroMemory(&tvi, sizeof(TVINSERTSTRUCT));
			tvi.hParent = hParent;
			tvi.hInsertAfter = TVI_LAST;
			tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
			tvi.item.lParam = (LPARAM) pidlAbs;
			// fill another properties of item(.pszText, .iImage, ...)
			FillItem(tvi, pFolder, pidlAbs, pItem);
			m_pMalloc->Free(pItem);
		}
		pEnumIDList->Release();
	}
}

///////////////////////////////////////////////////////////////
// FillItem ()
//		- Tree View에 등록할 요소를 결정하고 Tree 에 Insert 한다
///////////////////////////////////////////////////////////////
void CPackExtractDlg::FillItem(TVINSERTSTRUCT &tvi, LPSHELLFOLDER pFolder, LPITEMIDLIST pidl, LPITEMIDLIST pidlRelative)
{
	CTreeCtrl& Tree = m_ExtractFolderPos;
	STRRET sName;
	TCHAR szBuf[MAX_PATH] = {0};
	DWORD dwAttributes = 0;
	UINT uFlags = 0;
	SHFILEINFO sfi;
	// ** In the virtual folder cases, SHGetFileInfo.szDisplayName is impossible
	///////////////////////////////////////////////////
	// Set Display Name
	if (tvi.item.mask & TVIF_TEXT)
	{
		// get display name
		pFolder->GetDisplayNameOf(pidlRelative, 0, &sName);
		UTStrretToString(pidlRelative, &sName, szBuf);
		tvi.item.pszText = szBuf;
	}
	///////////////////////////////////////////////////
	// Set Display Image

	if (tvi.item.mask & (TVIF_IMAGE | TVIF_SELECTEDIMAGE))
	{
		// get some attributes
		dwAttributes = SFGAO_FOLDER | SFGAO_LINK;
		pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);
		// get correct icon
		uFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_DISPLAYNAME;
		if (dwAttributes & SFGAO_LINK)
			uFlags |= SHGFI_LINKOVERLAY;
		if (tvi.item.mask & TVIF_IMAGE)
		{
			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);

			tvi.item.iImage = sfi.iIcon;
		}
		if (tvi.item.mask & TVIF_SELECTEDIMAGE)
		{
			if (dwAttributes & SFGAO_FOLDER)
				uFlags |= SHGFI_OPENICON;
			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);

			tvi.item.iSelectedImage = sfi.iIcon;
		}
	}
	///////////////////////////////////////////////////
	// Set (+) Button or not
	if (tvi.item.mask & (TVIF_CHILDREN))
	{
		// get some attributes
		dwAttributes = SFGAO_FOLDER;
		pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);
		// get children
		tvi.item.cChildren = 0;
		if (dwAttributes & SFGAO_FOLDER)
		{
			dwAttributes = SFGAO_HASSUBFOLDER;
			pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);
			tvi.item.cChildren = (dwAttributes & SFGAO_HASSUBFOLDER) ? 1 : 0;
		}
	}
	
	Tree.InsertItem(&tvi);
}

//////////////////////////////////////////////////////////
// UTStrretToString()
//		- GetDisplayName()으로 갖고온 것을 String으로 변환
//////////////////////////////////////////////////////////
void CPackExtractDlg::UTStrretToString(LPITEMIDLIST pidl, LPSTRRET pStr, LPSTR pszBuf)
{
	lstrcpy(pszBuf, "");
	switch(pStr->uType)
	{
	case STRRET_WSTR: // Unicode string
		WideCharToMultiByte(CP_ACP, 0, pStr->pOleStr, -1, pszBuf, MAX_PATH, NULL, NULL);
		break;
	case STRRET_OFFSET: // Offset
		lstrcpy(pszBuf, reinterpret_cast<LPSTR>(pidl) + pStr->uOffset);
		break;
	case STRRET_CSTR: // ANSI string
		lstrcpy(pszBuf, pStr->cStr);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OnItemexpanding()
//		- 버튼이 눌렸을 때 해당 Tree Item에서 PIDL를 받는다. (lParam)
//		- 만약에 해당 폴더가 한 번도 확장된 적이 없다면 즉, 하부 아이템들이 등록되지 않았다면 Absolute PIDL를 갖고
//		  Desktop Shell Folder Interface 에서 해당 Shell Folder Interface를 얻어 EnumChildItems()를 호출해준다.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPackExtractDlg::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CTreeCtrl& Tree = m_ExtractFolderPos;
	if (pNMTreeView->action & TVE_EXPAND)
	{
		///////////////////////////////////////////////////
		// insert item when it needs children
		if (Tree.GetChildItem( pNMTreeView->itemNew.hItem ) == NULL)
		{
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			LPITEMIDLIST pidl;
			LPSHELLFOLDER pFolder;
			pidl = (LPITEMIDLIST) Tree.GetItemData(pNMTreeView->itemNew.hItem);
			if (pNMTreeView->itemNew.hItem != m_hDesktopItem)
			{
				m_pDesktopFolder->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID *)&pFolder);
				EnumChildItems(pFolder, pidl, pNMTreeView->itemNew.hItem);
				pFolder->Release();
			}
			else
				EnumChildItems(m_pDesktopFolder, pidl, pNMTreeView->itemNew.hItem);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}

	*pResult = 0;
}

// refer to `Working with Item ID Lists` of MSDN
LPITEMIDLIST CPackExtractDlg::ILGetNextItemID(LPCITEMIDLIST pidl)
{
	// Check for valid pidl.
	if (pidl == NULL)
		return NULL;
	// Get the size of the specified item identifier.
	int cb = pidl->mkid.cb;
	// If the size is zero, it is the end of the list.
	if (cb == 0)
		return NULL;
	// Add cb to pidl (casting to increment by bytes).
	pidl = (LPITEMIDLIST) (((LPBYTE) pidl) + cb);
	// Return NULL if it is null-terminating, or a pidl otherwise.
	return (pidl->mkid.cb == 0) ? NULL : (LPITEMIDLIST) pidl;
}
UINT CPackExtractDlg::ILGetSize(LPCITEMIDLIST pidl)
{
	UINT cbTotal = 0;

	if (pidl)
	{
		cbTotal += sizeof(pidl->mkid.cb); // Null terminator
		while (pidl)
		{
			cbTotal += pidl->mkid.cb;
			pidl = ILGetNextItemID(pidl);
		}
	}

	return cbTotal;
}
LPITEMIDLIST CPackExtractDlg::ILAppend(LPITEMIDLIST pidlParent, LPITEMIDLIST pidlChild)
{
	LPITEMIDLIST pidl = NULL;
	if(pidlParent == NULL)
		return NULL;
	if(pidlChild == NULL)
		return pidlParent;

	UINT cb1 = ILGetSize(pidlParent) - sizeof(pidlParent->mkid.cb);
	UINT cb2 = ILGetSize(pidlChild);
	pidl = (LPITEMIDLIST)m_pMalloc->Alloc(cb1 + cb2);
	if(pidl) {
		CopyMemory(pidl, pidlParent, cb1);
		CopyMemory(((LPSTR)pidl) + cb1, pidlChild, cb2);
	}
	return pidl;
}

void CPackExtractDlg::OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	OnItemexpanding( pNMHDR, pResult );
}

void CPackExtractDlg::OnTvnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	OnItemexpanding( pNMHDR, pResult );
}

void CPackExtractDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if( m_bIsWorking )
	{
		AfxMessageBox( "작업이 끝난 후에 시도하세요..." );
		return;
	}

	switch( m_eMode )
	{
	case ePackMode_Extract:
		LoadPackFile();
		break;
	case ePackMode_Compress:
		LoadCompressFolder();
		break;
	}	
}

void CPackExtractDlg::OnBnClickedModeExtractRadio()
{
	if( m_bIsWorking && m_eMode == ePackMode_Extract )
	{
		m_ModeExtractBtn.SetCheck( FALSE );
		m_ModeCompressBtn.SetCheck( TRUE );
		AfxMessageBox( "작업이 끝난 후에 시도하세요!" );

		return;
	}
	m_eMode = ePackMode_Extract;

	switch( m_eMode )
	{
	case ePackMode_Extract:
		LoadPackFile();
		break;
	case ePackMode_Compress:
		LoadCompressFolder();
		break;
	}
}

void CPackExtractDlg::OnBnClickedModeCompressRadio()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_bIsWorking && m_eMode == ePackMode_Compress )
	{
		m_ModeExtractBtn.SetCheck( TRUE );
		m_ModeCompressBtn.SetCheck( FALSE );
		AfxMessageBox( "작업이 끝난 후에 시도하세요!" );

		return;
	}
	m_eMode = ePackMode_Compress;

	switch( m_eMode )
	{
	case ePackMode_Extract:
		LoadPackFile();
		break;
	case ePackMode_Compress:
		LoadCompressFolder();
		break;
	}
}

void CPackExtractDlg::OnBnClickedDeleteAfterProcCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );
}

LRESULT CPackExtractDlg::OnUpdateDlg( WPARAM wParam, LPARAM lParam )
{
	UpdateData( BOOL(wParam) );

	return 0;
}

void CPackExtractDlg::OnBnClickedExtractFilenameButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_eMode != ePackMode_Extract )
		return;

	int nIndex = m_PackListBox.GetCurSel();
	int nCount = m_PackListBox.GetCount();

	if( (nIndex != LB_ERR) && (nCount > nIndex) )
	{
		CString str;
		int nTextLen = m_PackListBox.GetTextLen( nIndex );
		m_PackListBox.GetText( nIndex, str.GetBuffer( nTextLen ) );

		OpenExtractPackFileWithSelectListDlg( str );
	}
	else
	{
		AfxMessageBox( "항목을 선택하고 진행하셔야 합니다!" );
	}
}

void CPackExtractDlg::OnLbnDblclkPackList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedExtractFilenameButton1();
}


void CPackExtractDlg::ExpandTreeItem( HTREEITEM hItem )
{
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	LPITEMIDLIST pidl;
	LPSHELLFOLDER pFolder;
	pidl = (LPITEMIDLIST) m_ExtractFolderPos.GetItemData( hItem );
	if ( hItem != m_hDesktopItem)
	{
		m_pDesktopFolder->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID *)&pFolder);
		EnumChildItems(pFolder, pidl, hItem);
		pFolder->Release();
	}
	else
		EnumChildItems(m_pDesktopFolder, pidl, hItem);
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

HTREEITEM CPackExtractDlg::GetTreeItem( HTREEITEM hRootItem, LPCTSTR szName, BOOL bExactly )
{
	if( hRootItem == NULL )
		hRootItem = m_ExtractFolderPos.GetRootItem();

	HTREEITEM hChildItem = m_ExtractFolderPos.GetNextItem( hRootItem, TVGN_CHILD );
	if( hChildItem == NULL )
	{
		ExpandTreeItem( hRootItem );
		hChildItem = m_ExtractFolderPos.GetNextItem( hRootItem, TVGN_CHILD );
	}
	CString strChildName = m_ExtractFolderPos.GetItemText( hChildItem );
	strChildName.MakeUpper();
	CString strTargetName( szName );
	strTargetName.MakeUpper();


	if( bExactly )
	{
		if( strcmp( (LPCTSTR)strChildName, strTargetName ) == 0 )
			return hChildItem;
	}
	else
	{
		if( strstr( (LPCTSTR)strChildName, strTargetName ) != NULL )
			return hChildItem;
	}

	while( (hChildItem = m_ExtractFolderPos.GetNextItem( hChildItem, TVGN_NEXT )) != NULL )
	{
		strChildName = m_ExtractFolderPos.GetItemText( hChildItem );
		strChildName.MakeUpper();
		if( bExactly )
		{
			if( strcmp( (LPCTSTR)strChildName, strTargetName ) == 0 )
				return hChildItem;
		}
		else
		{
			if( strstr( (LPCTSTR)strChildName, strTargetName ) != NULL )
				return hChildItem;
		}
	}

	return NULL;
}

HTREEITEM CPackExtractDlg::GetTreeItem( HTREEITEM hRootItem, int csidl )
{
	if( hRootItem == NULL )
		hRootItem = m_ExtractFolderPos.GetRootItem();

	HTREEITEM hChildItem = m_ExtractFolderPos.GetNextItem( hRootItem, TVGN_CHILD );
	if( hChildItem == NULL )
	{
		ExpandTreeItem( hRootItem );
		hChildItem = m_ExtractFolderPos.GetNextItem( hRootItem, TVGN_CHILD );
	}

	LPITEMIDLIST pidl;
	if( SHGetSpecialFolderLocation(NULL, csidl, &pidl) != S_OK )
		return NULL;

	CString strChildName = m_ExtractFolderPos.GetItemText( hChildItem );
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	UINT uFlags = SHGFI_PIDL | SHGFI_DISPLAYNAME;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);

	if( strcmp( (LPCTSTR)strChildName, sfi.szDisplayName ) == 0 )
		return hChildItem;

	while( (hChildItem = m_ExtractFolderPos.GetNextItem( hChildItem, TVGN_NEXT )) != NULL )
	{
		strChildName = m_ExtractFolderPos.GetItemText( hChildItem );
		if( strcmp( (LPCTSTR)strChildName, sfi.szDisplayName ) == 0 )
			return hChildItem;
	}

	return NULL;
}

BOOL CPackExtractDlg::UpdateTreePath( CString strPath )
{
	HTREEITEM hItem = GetTreeItem( NULL, CSIDL_DRIVES );
	if( hItem == NULL )
		return FALSE;

	HTREEITEM hTargetTree = NULL;
	CString token;
	int nCurPos = 0;
	token = strPath.Tokenize( "\\", nCurPos );
	while( token != "" )
	{
		hTargetTree = GetTreeItem( hItem, token, TRUE );
		if( hTargetTree == NULL )
		{
			hTargetTree = GetTreeItem( hItem, token, FALSE );
			if( hTargetTree == NULL )
				return FALSE;

		}

		m_ExtractFolderPos.SelectItem( hTargetTree );
		hItem = hTargetTree;
		token = strPath.Tokenize( "\\", nCurPos );
	}

	if( hTargetTree == NULL )
		return FALSE;

	// 결과 확인
	LPITEMIDLIST pidl;
	pidl = (LPITEMIDLIST)m_ExtractFolderPos.GetItemData( hTargetTree );
	char szTargetPath[MAX_PATH];
	::SHGetPathFromIDList( pidl, (LPSTR)(LPCTSTR)szTargetPath );
	CString strTargetPath = szTargetPath;

	// 대문자로 변환
	strTargetPath.MakeUpper();
	strPath.MakeUpper();

	// 마지막이 '\'가 아닌경우 '\'를 붙여준다
	if( strPath.GetAt( strPath.GetLength() - 1 ) != '\\' )
		strPath.Append( "\\" );
	if( strTargetPath.GetAt( strPath.GetLength() - 1 ) != '\\' )
		strTargetPath.Append( "\\" );

	if( strPath.Compare( strTargetPath ) != 0 )
	{
		return FALSE;
	}

	// 활성화 후 포커스를 넘겨준다
	SetForegroundWindow();
    m_ExtractFolderPos.SetFocus();
	return TRUE;
}
void CPackExtractDlg::OnDropFiles(HDROP hDropInfo)
{
	// Drag File 갯수 저장
	int nDragFileCnt = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0 );

	// 1번째 Drag File 이름 읽어오기
	char szPath[256];
	DragQueryFile( hDropInfo, 0, szPath, 256 );
	DWORD dwFileAttr = GetFileAttributes( szPath );
	

	// Directory 인 경우
	if( dwFileAttr & FILE_ATTRIBUTE_DIRECTORY )
	{
		// Mode 설정
		m_eMode = ePackMode_Compress;
		m_ModeExtractBtn.SetCheck( FALSE );
		m_ModeCompressBtn.SetCheck( TRUE );

		CString strPath = szPath;
		// 상위 폴더를 위치로 지정한다.
		strPath = strPath.Left( strPath.ReverseFind( '\\' ) );
		UpdateTreePath( strPath );

		m_PackListBox.ResetContent();
		m_vCompressFolder.clear();

		for( int index=0 ; index < nDragFileCnt ; ++index )
		{
			DragQueryFile( hDropInfo, index, szPath, 256 );
			DWORD dwFileAttr = GetFileAttributes( szPath );
			// Directory가 아닌 경우
			if( !(dwFileAttr & FILE_ATTRIBUTE_DIRECTORY) )
				continue;

			CString strFolder = szPath;
			strFolder.Delete( 0, strFolder.ReverseFind( '\\' ) + 1 );

			m_PackListBox.AddString( strFolder );
			m_vCompressFolder.push_back( strFolder );
		}
	}
	// File 인 경우
	else
	{
		// Mode 설정
		m_eMode = ePackMode_Extract;
		m_ModeExtractBtn.SetCheck( TRUE );
		m_ModeCompressBtn.SetCheck( FALSE );

		CString strPath = szPath;
		// 파일경로에서 폴더경로만 남긴다.
		strPath = strPath.Left( strPath.ReverseFind( '\\' ) );

		// Drop File의 경로로 Tree를 설정한다.
		if( UpdateTreePath( strPath ) == FALSE )
		{
			// 설정 실패시 기본(실행경로)으로 되돌린다.
			UpdateTreePath( m_szBaseDirectory );
			return;
		}

		// 선택된 File만 List에 넣을수 있도록 설정한다.
		m_PackListBox.ResetContent();
		m_vPackFile.clear();

		for( int index=0 ; index < nDragFileCnt ; ++index )
		{
			DragQueryFile( hDropInfo, index, szPath, 256 );
			DWORD dwFileAttr = GetFileAttributes( szPath );
			// Directory인 경우
			if( dwFileAttr & FILE_ATTRIBUTE_DIRECTORY )
				continue;

			strPath = szPath;
			strPath.Delete( 0, strPath.ReverseFind( '\\' ) + 1 );

			// pak 파일이 아닌경우
			if( strstr( (LPCTSTR)strPath, ".pak" ) == NULL )
				continue;

			m_PackListBox.AddString( strPath );
			m_vPackFile.push_back( strPath );
		}
	}

	

	CDialog::OnDropFiles(hDropInfo);
}

void CPackExtractDlg::OnBnClickedHelpButton()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

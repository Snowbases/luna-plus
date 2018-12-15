#include "stdafx.h"
#include "../4DyuchiGXGFunc/global.h"
#include "../4dyuchiGRX_common/IExecutive.h"
#include "Application.h"
#include "MainDialog.h"
#include "RainFTP_TypeDefine.h"
#include "common.h"
#include "StrClass.h"
#include "DirectoryName.h"
#include ".\maindialog.h"


DWORD __stdcall PackingCallBackFunc( DWORD dwCurCount,DWORD dwTotalCount,void* pArg );

HANDLE packingThreadHandle;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const CString FTP_userID		( "anonymous" );
const CString FTP_password		( "" );
const CString URL_registration	( "http://luna.eyainteractive.com/member_zone/join_01.asp" );
const CString URL_homepage		( "http://luna.eyainteractive.com" );
const CString URL_content		( "http://luna.eyainteractive.com/img/launcher/launcher01.htm" );
const CString ReleaseFile		( "lunaclient.exe" );
const CString DebugFile			( "LUNAClient-GMTool-Debug.exe" );
const CString LogFile			( "LunaLauncher.log" );

// 주의: 대소문자 구분함
#ifdef _FOR_TEST_SERVER
const CString FTP_directory( "pub/eya/LUNA/TestPatch/" );
#else
const CString FTP_directory( "pub/eya/LUNA/Patch/" );
#endif

// 다른 FTP 주소가 있으면 배열에 추가만 하면 된다.
const TCHAR* FTP_address[] = {
	"eyaftp.nefficient.co.kr:21",
};


BOOL	g_bUsingEnginePack = TRUE;
DWORD	dwFileSizeMax = 0;
BOOL	gbCheckVersion = FALSE;
BOOL	gbConnect	= FALSE;

extern char g_szCurVer[256];
extern char g_szLastestVer[256];

CString packingFileName;

CMainDialog* CMainDialog::m_self;

const CSize PackingTextSize( 120, 20 );

//CMainDialog::CMainDialog(CWnd* pParent /*=NULL*/) :
CMainDialog::CMainDialog(CWnd* pParent, CApplication::Config& config ) :
CDialog(CMainDialog::IDD, pParent),
m_config( config ),
m_pHTML( 0 ),
m_isReady( false )
{
	ASSERT( ! m_self );

	m_self = this;

	const CString& path = m_config.m_imagePath;

	{
		if( ! m_comboBox.Load( path + m_config.m_ImageComboCover, CXIMAGE_FORMAT_TIF ) )
		{
			ASSERT( 0 );
		}

		// 080507 LUJ, 이미지 경로 변경용 콤보 커버
		if( ! m_InterfaceComboBox.Load( path + m_config.m_ImageInterfaceComboCover, CXIMAGE_FORMAT_TIF ) )
		{
			ASSERT( 0 );
		}
	}

	// 배경: 일반 컨트롤 그림은 배경과 합쳐버린다. 주의: 합칠 때의 좌표계는 하단좌측에서 얼마나 떨어졌냐로 표시됨
	{
		if( ! m_backgroundImage.Load( path + m_config.m_imageBackground, CXIMAGE_FORMAT_TIF ) )
		{
			ASSERT( 0 );
		}

		m_backgroundImage.Crop( m_config.m_status, &m_statusImage );
		/*m_backgroundImage.Crop( 
		CRect( 
			m_config.m_progress.left,
			m_config.m_progress.top - PackingTextSize.cy * 2,
			m_config.m_progress.left + PackingTextSize.cx,
			m_config.m_progress.top ),
			&m_statusPacking );*/

			const long width = m_backgroundImage.GetWidth();
		const long height = m_backgroundImage.GetHeight();

		// 홈페이지
		{
			CxImage image( path + m_config.m_imageHomepageUp, CXIMAGE_FORMAT_TIF );

			m_config.m_homePage.right = m_config.m_homePage.left + image.GetWidth();
			m_config.m_homePage.bottom = m_config.m_homePage.top + image.GetHeight();

			m_backgroundImage.Mix( 
				image, 
				CxImage::OpDstCopy,			
				-m_config.m_homePage.left, 
				m_config.m_homePage.bottom - height );

			if( ! m_homePageImage.Load( path + m_config.m_imageHomepageDown, CXIMAGE_FORMAT_TIF ) )
			{
				ASSERT( 0 );
			}

			m_homePageImage.Enable( false );
		}

		// 등록 버튼
		{
			CxImage image( path + m_config.m_imageRegisterUp, CXIMAGE_FORMAT_TIF );

			m_config.m_register.right = m_config.m_register.left + image.GetWidth();
			m_config.m_register.bottom = m_config.m_register.top + image.GetHeight();

			m_backgroundImage.Mix( 
				image,
				CxImage::OpDstCopy,
				-m_config.m_register.left, 
				m_config.m_register.bottom - height );

			if( ! m_registerImage.Load( path + m_config.m_imageRegisterDown, CXIMAGE_FORMAT_TIF ) )
			{
				ASSERT( 0 );
			}

			m_registerImage.Enable( false );
		}

		// 체크 박스
		{
			CxImage image( path + m_config.m_imageCheckBoxUnchecked, CXIMAGE_FORMAT_TIF );

			m_config.m_checkBox.right = m_config.m_checkBox.left + image.GetWidth();
			m_config.m_checkBox.bottom = m_config.m_checkBox.top + image.GetHeight();

			m_backgroundImage.Mix( 
				image,
				CxImage::OpDstCopy,
				-m_config.m_checkBox.left, 
				m_config.m_checkBox.bottom - height );

			if( ! m_checkBoxImage.Load( path + m_config.m_imageCheckBoxChecked, CXIMAGE_FORMAT_TIF ) )
			{
				ASSERT( 0 );
			}

			m_checkBoxImage.Enable( m_config.m_isWindowMode );
		}

		// 시작 버튼
		{
			CxImage image( path + m_config.m_imageBeginUp, CXIMAGE_FORMAT_TIF );
			image.GrayScale();

			m_config.m_begin.right = m_config.m_begin.left + image.GetWidth();
			m_config.m_begin.bottom = m_config.m_begin.top + image.GetHeight();

			m_backgroundImage.Mix( 
				image,
				CxImage::OpDstCopy,
				-m_config.m_begin.left, 
				m_config.m_begin.bottom - height );

			if( ! m_beginImage.Load( path + m_config.m_imageBeginDown, CXIMAGE_FORMAT_TIF ) )
			{
				ASSERT( 0 );
			}

			m_beginImage.Enable( false );
		}

		// 닫기 버튼
		{
			CxImage image( path + m_config.m_imageCloseUp, CXIMAGE_FORMAT_TIF );

			m_config.m_close.right = m_config.m_close.left + image.GetWidth();
			m_config.m_close.bottom = m_config.m_close.top + image.GetHeight();

			m_backgroundImage.Mix( 
				image,
				CxImage::OpDstCopy,
				-m_config.m_close.left, 
				m_config.m_close.bottom - height );

			if( ! m_closeImage.Load( path + m_config.m_imageCloseDown, CXIMAGE_FORMAT_TIF ) )
			{
				ASSERT( 0 );
			}

			m_closeImage.Enable( false );
		}

		// 프로그레스
		{
			m_progressImage.Load( path + m_config.m_imageProgressBar, CXIMAGE_FORMAT_TIF );

			m_config.m_progress.right = m_config.m_progress.left + m_progressImage.GetWidth();
			m_config.m_progress.bottom = m_config.m_progress.top + m_progressImage.GetHeight();

			m_backgroundImage.Mix( 
				m_progressImage,
				CxImage::OpDstCopy,
				-m_config.m_progress.left, 
				m_config.m_progress.bottom - height );

			if( ! m_progress.m_cellImage.Load( path + m_config.m_imageProgressCell, CXIMAGE_FORMAT_TIF ) )
			{
				ASSERT( 0 );
			}

			m_progress.m_cellImage.Enable( false );
			m_progress.m_value = 0;
		}
	}

	// 080507 LUJ, 콤보 박스 이미지 설정
	{
		{
			COwnerDrawComboBox::ImageNameList	nameList;
			COwnerDrawComboBox::ImageName		name;

			name.mSelect	= path + config.m_imageCombo1Selected;
			name.mUnselect	= path + config.m_imageCombo1;
			nameList.push_back( name );

			name.mSelect	= path + config.m_imageCombo2Selected;
			name.mUnselect	= path + config.m_imageCombo2;
			nameList.push_back( name );

			name.mSelect	= path + config.m_imageCombo3Selected;
			name.mUnselect	= path + config.m_imageCombo3;
			nameList.push_back( name );

			m_ComboScreenSize.SetImage( nameList );
		}
		
		{
			COwnerDrawComboBox::ImageNameList	nameList;
			COwnerDrawComboBox::ImageName		name;

			name.mSelect	= path + config.m_ImageInterfaceCombo1Selected;
			name.mUnselect	= path + config.m_imageInterfaceCombo1;
			nameList.push_back( name );

			name.mSelect	= path + config.m_ImageInterfaceCombo2Selected;
			name.mUnselect	= path + config.m_ImageInterfaceCombo2;
			nameList.push_back( name );

			name.mSelect	= path + config.m_ImageInterfaceCombo3Selected;
			name.mUnselect	= path + config.m_ImageInterfaceCombo3;
			nameList.push_back( name );

			m_ComboInterface.SetImage( nameList );
		}
	}
}


CMainDialog::~CMainDialog()
{
	m_self = 0;
}


CMainDialog* CMainDialog::GetInstance()
{
	ASSERT( m_self );

	return m_self;
}


void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SCREENSIZE, m_ComboScreenSize);
	DDX_Control(pDX, IDC_INTERFACE, m_ComboInterface);
}

BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	//{{AFX_MSG_MAP(CMainDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()

	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_SCREENSIZE, OnCbnSelchangeScreensize)
	ON_WM_NCPAINT()
	ON_WM_SETFOCUS()
	//	ON_WM_UPDATEUISTATE()
	ON_WM_KILLFOCUS()
	ON_CBN_SELCHANGE(IDC_INTERFACE, OnCbnSelchangeInterface)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDialog message handlers

BOOL CMainDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	BOOL bReturn = UpdateVersionInfoFile( VERSION_FILE, newverFilename);

	CRect myRect ;
	GetClientRect( myRect );

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	//SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon

	SetWindowText( m_config.m_textCaption );

	m_pHTML	= new CHtmlViewNotice;
	m_pHTML->Create( NULL, NULL, WS_CHILD, m_config.m_html, this, 1 );
	m_pHTML->Navigate2( URL_content );

	// 웅주, 해상도 콤보 박스 설정
	{
		CRect rect;
		m_ComboScreenSize.GetClientRect( rect );

		m_ComboScreenSize.SetWindowPos( 
			0,
			m_config.m_resolution.left,
			m_config.m_resolution.top,
			m_config.m_resolution.Width(),
			m_config.m_resolution.Height() * ( m_ComboScreenSize.GetCount() + 1 ),
			SWP_NOREDRAW );

		// 입력창 크기 변경
		m_ComboScreenSize.SetItemHeight( -1, m_config.m_resolution.Height() - 5 );

		// 콤보박스를 SetWindowPos 해준 다음에 아이템 크기를 바꿔줘야 적용된다.
		//
		// http://www.codeproject.com/dialog/layout_helper.asp?df=100&forumid=207570&exp=0&select=1447772#xx1447772xx
		for( int i = 0; i < m_ComboScreenSize.GetCount(); ++i )
		{
			m_ComboScreenSize.SetItemHeight( i, 20 );
		}

		// TODO: 저장된 해상도로 변경
		const long select = m_ComboScreenSize.GetCount() + 1 < m_config.m_SelectedResolution ? 1 : m_config.m_SelectedResolution;

		m_ComboScreenSize.SetCurSel( select );
	}

	// 080507 LUJ, 인터페이스 선택 콤보 박스 설정
	{
		CRect rect;
		m_ComboInterface.GetClientRect( rect );

		m_ComboInterface.SetWindowPos( 
			0,
			m_config.m_interface.left,
			m_config.m_interface.top,
			m_config.m_interface.Width(),
			m_config.m_interface.Height() * ( m_ComboInterface.GetCount() + 1 ),
			SWP_NOREDRAW );

		// 입력창 크기 변경
		m_ComboInterface.SetItemHeight( -1, m_config.m_interface.Height() - 5 );

		m_ComboInterface.SetCurSel( 1 );
	
		// 콤보박스를 SetWindowPos 해준 다음에 아이템 크기를 바꿔줘야 적용된다.
		//
		// http://www.codeproject.com/dialog/layout_helper.asp?df=100&forumid=207570&exp=0&select=1447772#xx1447772xx
		for( int i = 0; i < m_ComboInterface.GetCount(); ++i )
		{
			m_ComboInterface.SetItemHeight( i, 20 );

			const CString path( "data\\interface\\windows\\" );
			const CString blueInterface( path + "image_path.bin" );
			const CString pinkInterface( path + "image_path_pink.bin" );
			const CString grayInterface( path + "image_path_grey.bin" );

			if( blueInterface == m_config.m_SelectedInterface )
			{
				m_ComboInterface.SetCurSel( 0 );
			}
			else if( grayInterface == m_config.m_SelectedInterface )
			{
				m_ComboInterface.SetCurSel( 1 );
			}
			else if( pinkInterface == m_config.m_SelectedInterface )
			{
				m_ComboInterface.SetCurSel( 2 );
			}
		}
	}

	PutStatus( m_config.m_textCheck );

	//-------------
	FILE* fp = fopen("./_DONOTUSEENGINEPACK","r");
	if(fp)
	{
		g_bUsingEnginePack = FALSE;
		fclose(fp);
	}

	InvalidateRect( 0, false );
	UpdateWindow();

	return TRUE;  // return TRUE  unless you set the focus to a control
}


BOOL CMainDialog::ConnectFtp()
{
	const TCHAR* seperator	= _T( ":" );
	const SIZE_T size		= sizeof( FTP_address ) / sizeof( TCHAR* );

	TCHAR buffer[ 80 ];

	for( size_t i = 0; i < size; ++i )
	{
		_tcscpy( buffer, FTP_address[ i ] );

		const CString IP( _tcstok( buffer, seperator ) );
		const CString port( _tcstok( 0, seperator ) );

		if( RETURN_FAIL != g_pFtp->ConnectFtp( IP, _ttoi( port ) ) &&
			RETURN_GOOD == g_pFtp->LogIn( FTP_userID, FTP_password ) )
		{
			// 대소문자 구분함
			sprintf( buffer, "%s", FTP_directory );

			if( RETURN_FAIL == g_pFtp->RemoteChangeDirectory( buffer ) )
			{
				return FALSE;
			}

			g_pFtp->SetTransferDataSpeed( 0 );

			return TRUE;
		}
	}

	return FALSE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CMainDialog::OnPaint() 
{
	CPaintDC dc( this );

	{
		//if( ! m_progressText.IsEmpty() )
		//{
		//	/*m_backgroundImage.Mix( 
		//		m_statusPacking,
		//		CxImage::OpDstCopy,
		//		-m_config.m_progress.left, 
		//		m_config.m_progress.top - m_backgroundImage.GetHeight() );*/

		//	m_backgroundImage.DrawString( 
		//		dc.m_hDC,
		//		m_config.m_progress.left,
		//		m_config.m_progress.top + 5,
		//		m_progressText,
		//		m_config.m_fontStatusColor,
		//		"MS Sans Serif", 
		//		3 );
		//}

		m_backgroundImage.Draw( dc.m_hDC );
		m_homePageImage.Draw( dc.m_hDC, m_config.m_homePage.left, m_config.m_homePage.top );
		m_registerImage.Draw( dc.m_hDC, m_config.m_register.left, m_config.m_register.top );
		m_beginImage.Draw( dc.m_hDC, m_config.m_begin.left, m_config.m_begin.top );
		m_closeImage.Draw( dc.m_hDC, m_config.m_close.left, m_config.m_close.top );
		m_checkBoxImage.Draw( dc.m_hDC, m_config.m_checkBox.left, m_config.m_checkBox.top );

		// 패킹 중에 파일 이름을 표시한다.
		//m_statusPacking.Draw( dc.m_hDC, 0, 10 );

	}

	CDialog::OnPaint();


}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
//HCURSOR CMainDialog::OnQueryDragIcon()
//{
//	//return (HCURSOR) m_hIcon;
//}


void CMainDialog::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if( m_pHTML )
	{
		delete m_pHTML;
		m_pHTML = NULL;
	}

	CDialog::OnClose();
}

BOOL CMainDialog::Clear3DData()
{
	char CurDir[MAX_PATH];
	CStrClass datafolder = "3DData\\";
	GetCurrentDirectory(MAX_PATH,CurDir);
	BOOL bSD = SetCurrentDirectory("3ddata");
	if(bSD == FALSE)
		return TRUE;	

	CDirectoryName dirname(NULL,NULL);
	dirname.LoadChild();

	SetCurrentDirectory(CurDir);

	dirname.SetPositionHead();
	DWORD kind;
	CStrClass filename;
	CStrClass tname;	

	dirname.SetPositionHead();
	while((kind=dirname.GetNextFileName(&filename)) != GETNEXTFILENAME_END)
	{
		if(kind != GETNEXTFILENAME_FILE)
			continue;
		if(filename == "")
			continue;

		tname = datafolder + filename;

		DeleteFile(tname);
	}
	dirname.SetPositionHead();
	while((kind=dirname.GetNextFileName(&filename)) != GETNEXTFILENAME_END)
	{
		if(kind != GETNEXTFILENAME_DIRECTORY)
			continue;
		tname = datafolder + filename;
		RemoveDirectory(tname);
	}
	RemoveDirectory(datafolder);

	return TRUE;
}

LRESULT CMainDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( message )
	{
	case WM_RUNPROGRAM:
		{
			BOOL bReturn = UpdateVersionInfoFile( VERSION_FILE, newverFilename);

			SetReady();
		}
		break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


BOOL CMainDialog::RunLuna()
{
	SelectGFunc();

	STARTUPINFO	sp;
	ZeroMemory( &sp, sizeof( STARTUPINFO ) );
	sp.cb = sizeof(STARTUPINFO);
	sp.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE | STARTF_USESTDHANDLES; //STARTF_USESHOWWINDOW 
	sp.wShowWindow = SW_SHOW; 

	PROCESS_INFORMATION pi;

	if( ! CreateProcess( 
#ifdef _DEBUG
		DebugFile,
#else
		ReleaseFile,
#endif
		" anrgideoqkr",										// command line string
		0,													// SD
		0,													// SD
		FALSE,												// handle inheritance option
		CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,  // creation flags
		0,													// new environment block
		0,													// current directory name
		&sp,												// startup information
		&pi	) )												// process information
	{
		return FALSE;
	}

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	return TRUE;
}


BOOL CMainDialog::ExtractFile( char* fileName )
{
	CZipArchive& zipArchive = CApplication::GetInstance()->m_zipArchive;
	zipArchive.Open( GAME_PATCH, CZipArchive::zipOpen );

	//bReturn = TRUE;

	// 로그
	{
		CString log;
		log.Format(
			"%d extraction information(temp path: %s, root path: %s, archive path: %s)\n",
			GetTickCount(),
			zipArchive.GetTempPath(),
			zipArchive.GetRootPath(),
			zipArchive.GetArchivePath() );

		PutLog( log );

		log.Format(
			"\tcount: %d, password: %s)\n",
			zipArchive.GetCount( true ),
			zipArchive.GetPassword() );

		PutLog( log );
	}

	//for ( int dwi = 0; dwi < g_pZip->GetCount(); dwi++)
	for ( int i = 0; i < zipArchive.GetCount(); ++ i )
	{
		CZipFileHeader head;

		//if( g_pZip->GetFileInfo( head, dwi)  == false )
		if( ! zipArchive.GetFileInfo( head, i) )
		{
			zipArchive.Close();

			// 로그
			{
				CString log;
				log.Format( "%d ...%dth file information is invalid\n", GetTickCount(), i );

				PutLog( log );
			}

			return FALSE;
		}

		// 로그
		{
			CString log;
			log.Format( "%d ...%dth %s information(size: %d)\n",
				GetTickCount(),
				i,
				head.GetFileName(),
				head.GetSize() );

			PutLog( log );
		}

		SetFileAttributes( head.GetFileName(), FILE_ATTRIBUTE_NORMAL );

		//if( g_pZip->ExtractFile( dwi, NULL ) == false )
		if( ! zipArchive.ExtractFile( i, 0 ) )
		{
			zipArchive.Close();

			// 로그
			{
				CString log;
				log.Format( "%d ...file extraction is failed\n",
					GetTickCount() );

				PutLog( log );
			}

			return FALSE;
		}

		// 로그
		{
			CString log;
			log.Format( "%d ...file extraction is succeed\n",
				GetTickCount() );

			PutLog( log );
		}

		PutProgress( float( i + 1 ) / zipArchive.GetCount() );

		MSG message;

		if( PeekMessage(&message, NULL, 0, 0, PM_REMOVE) ) 
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	//g_pZip->Close();
	zipArchive.Close();

	return TRUE;
}

void CMainDialog::StartPack3DData()
{	
	PutProgress( 0 );
	PutStatus( m_config.m_textPack );

	packingThreadHandle = ( HANDLE )_beginthread( Pack3DData, 0, m_hWnd );
}


BOOL CMainDialog::UpdateVersionInfoFile( char* lpszOldVerInfoFile, char* lpszNewVerInfoFile )
{
	HANDLE			hFind;
	WIN32_FIND_DATA wfd;
	BOOL			bReturn;

	hFind = FindFirstFile( lpszNewVerInfoFile, &wfd);
	if( hFind == INVALID_HANDLE_VALUE )
		return FALSE;

	bReturn = DeleteFile( lpszOldVerInfoFile);

	if( !bReturn )
	{
		//MessageBox( m_config.m_messageBoxFailedVersionFileDelete, m_config.m_messageBoxCaption, MB_OK );
		return FALSE;
	}

	bReturn = MoveFile( lpszNewVerInfoFile, lpszOldVerInfoFile); // for win2000 later ==> , MOVEFILE_REPLACE_EXISTING);
	if( !bReturn )
	{
		MessageBox( m_config.m_messageBoxFailedVersionFileRename, m_config.m_messageBoxCaption, MB_OK );
		return FALSE;
	}

	return TRUE;
}


BOOL IsSupportSSE()
{
	BOOL	bResult = TRUE;
	__try 
	{
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

BOOL CMainDialog::SelectGFunc()
{
	if (IsSupportSSE())
		CopyFile("SS3DGFuncSSE.dll","SS3DGFunc.dll",FALSE);
	else
		CopyFile("SS3DGFuncN.dll","SS3DGFunc.dll",FALSE);
	return TRUE;
}

BOOL CMainDialog::InitEngine()
{
	SelectGFunc();

	g_hExecutiveHandle = LoadLibrary("Executive.dll");

	if( ! g_hExecutiveHandle )
	{
		return FALSE;
	}

	CREATE_INSTANCE_FUNC pFunc = (CREATE_INSTANCE_FUNC)GetProcAddress(g_hExecutiveHandle,"DllCreateInstance");

	if (pFunc((void**)&g_pExecutive) != S_OK)
	{
		const CApplication::Config& config = CApplication::GetInstance()->m_config;

		::MessageBox( CMainDialog::GetInstance()->GetSafeHwnd(), config.m_messageBoxFailedCreateExecutive, config.m_messageBoxCaption, MB_OK );
		return FALSE;
	}

	// geometry DLL의 이름, renderer DLL의 이름을 넣어주면 executive가 알아서 생성,초기화해준다.

	// 080221 LUJ, 패킹시킬 파일 개수를 15000 -> 25000으로 수정
	return g_pExecutive->InitializeFileStorageWithoutRegistry(
		"FileStorage.dll",
		25000,
		4096,
		128,
		FILE_ACCESS_METHOD_ONLY_FILE,
		NULL,
		0 );
}


BOOL AddDataToPackFile( CStrClass pakname, CStrClass dir )
{
	I4DyuchiFileStorage* pFS;
	g_pExecutive->GetFileStorage(&pFS);

	void* pPakFile = pFS->MapPackFile((char*)(LPCTSTR)pakname);
	if(pPakFile == NULL)
	{
		pFS->Release();
		return FALSE;
	}

	char CurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,CurDir);
	BOOL bSD = SetCurrentDirectory(dir);

	if(bSD == FALSE)
	{
		pFS->UnmapPackFile(pPakFile);
		pFS->Release();
		return FALSE;
	}

	BOOL rt = pFS->LockPackFile(pPakFile,0);
	if(rt == FALSE)
	{
		pFS->UnmapPackFile(pPakFile);
		pFS->Release();
		return FALSE;
	}

	CDirectoryName dirname(NULL,NULL);

	dirname.LoadChild(LCO_FILE);

	SetCurrentDirectory(CurDir);

	dirname.SetPositionHead();
	DWORD kind;
	CStrClass filename;
	CStrClass tname;
	while((kind=dirname.GetNextFileName(&filename)) != GETNEXTFILENAME_END)
	{
		if(kind != GETNEXTFILENAME_FILE)
			continue;
		if(filename == "")
			continue;

		tname = dir + "\\" + filename;

		packingFileName = ( LPCTSTR )filename;

		if(pFS->InsertFileToPackFile(pPakFile,( char* )( LPCTSTR )tname) == FALSE)
		{
			pFS->UnlockPackFile(pPakFile,PackingCallBackFunc);			
			pFS->UnmapPackFile(pPakFile);
			pFS->Release();

			return FALSE;
		}

		Sleep( 1 );
	}

	BOOL unlock = pFS->UnlockPackFile(pPakFile,PackingCallBackFunc);

	pFS->UnmapPackFile(pPakFile);
	pFS->Release();

	return unlock;
}

void CMainDialog::Pack3DData(void* p)
{
	BOOL bSuccess = TRUE;
	CDirectoryName dirname(NULL,NULL);
	CDirectoryName dirname2(NULL,NULL);
	CDirectoryName dirname3(NULL,NULL);
	CDirectoryName dirname4(NULL,NULL);
	CDirectoryName dirname5(NULL,NULL);


	DWORD kind;
	CStrClass filename;
	CStrClass pakname,tname;
	//HWND hDlg = (HWND)p;

	char CurDir[MAX_PATH];

	CStrClass datafolder = "Data\\3DData\\";

	if( InitEngine() == FALSE )
	{
		bSuccess = FALSE;
		goto packend;
	}

	GetCurrentDirectory(MAX_PATH,CurDir);
	BOOL bSD = SetCurrentDirectory("Data\\3DData");

	if(bSD == FALSE)
	{
		goto pack2d;
	}

	dirname.LoadChild();

	SetCurrentDirectory(CurDir);

	dirname.SetPositionHead();
	while((kind=dirname.GetNextFileName(&filename)) != GETNEXTFILENAME_END)
	{
		if(kind != GETNEXTFILENAME_DIRECTORY)
			continue;
		if(filename == "")
			continue;

		if(filename.Right(1) == "\\")
			filename = filename.Left(filename.GetLength()-1);

		pakname = datafolder + filename + ".pak";

		tname = datafolder + filename;
		if(AddDataToPackFile(pakname,tname) == FALSE)
		{
			//			bSuccess = FALSE;
			goto pack2d;
		}

		Sleep( 1 );
	}

	dirname.SetPositionHead();
	while((kind=dirname.GetNextFileName(&filename)) != GETNEXTFILENAME_END)
	{
		if(kind != GETNEXTFILENAME_FILE)
			continue;
		if(filename == "")
			continue;

		if( filename.Right(4) == ".pak" )
			continue;

		tname = datafolder + filename;

		BOOL rt = DeleteFile(tname);

		Sleep( 1 );
	}

	dirname.SetPositionHead();
	while((kind=dirname.GetNextFileName(&filename)) != GETNEXTFILENAME_END)
	{
		if(kind != GETNEXTFILENAME_DIRECTORY)
			continue;
		if(filename == "")
			continue;

		tname = datafolder + filename;
		RemoveDirectory(tname);

		Sleep( 1 );
	}


	//2d image들 패킹
	//해당폴더에 있는 파일들만(폴더제외)
pack2d:
	PackingCallBackFunc( 0, 0, 0 );	

	AddDataToPackFile( "data\\interface\\2dimage\\image.pak", "data\\interface\\2dimage\\image" );
	AddDataToPackFile( "data\\interface\\2dimage\\npcimage.pak", "data\\interface\\2dimage\\npcimage" );

	GetCurrentDirectory(MAX_PATH,CurDir);
	bSD = SetCurrentDirectory("data\\interface\\2dimage\\image");
	if( bSD )
	{
		dirname2.LoadChild(LCO_FILE);
		dirname2.SetPositionHead();
		while((kind=dirname2.GetNextFileName(&filename)) != GETNEXTFILENAME_END)
		{
			if(kind != GETNEXTFILENAME_FILE)
				continue;
			if(filename == "")
				continue;
			//			if( filename.Right(4) == ".pak" )
			//				continue;
			tname = filename;

			BOOL rt = DeleteFile(tname);

			Sleep( 1 );
		}
	}
	SetCurrentDirectory(CurDir);
	RemoveDirectory("data\\interface\\2dimage\\image");

	GetCurrentDirectory(MAX_PATH,CurDir);
	bSD = SetCurrentDirectory("data\\interface\\2dimage\\npcimage");
	if( bSD )
	{
		dirname3.LoadChild(LCO_FILE);
		dirname3.SetPositionHead();
		while((kind=dirname3.GetNextFileName(&filename)) != GETNEXTFILENAME_END)
		{
			if(kind != GETNEXTFILENAME_FILE)
				continue;
			if(filename == "")
				continue;
			//			if( filename.Right(4) == ".pak" )
			//				continue;
			tname = filename;

			BOOL rt = DeleteFile(tname);

			Sleep( 1 );
		}
	}
	SetCurrentDirectory(CurDir);
	RemoveDirectory("data\\interface\\2dimage\\npcimage");


packend:

	if( CMainDialog::GetInstance() )
	{
		::SendMessage( CMainDialog::GetInstance()->GetSafeHwnd(), WM_RUNPROGRAM, 0, 0 );
	}

	packingThreadHandle = 0;
	packingFileName.Empty();

	return;
}


void CMainDialog::SetReady()
{
	{
		CFile file;

		if( file.Open( LAUNCHER_PATCH, CFile::modeRead ) )
		{
			EndDialog( IDTRYAGAIN );
			return;
		}
		else if( file.Open( "SS3DGFunc.new", CFile::modeRead ) )
		{
			EndDialog( IDTRYAGAIN );
			return;
		}
	}

	// 다운로드 성공했으므로 로그 파일 필요 없음
	DeleteFile( LogFile );

	m_isReady = true;
	packingFileName.Empty();

	PutProgress( 1, true );
	PutStatus( m_config.m_textBegin );

	CxImage image;

	if( image.Load( m_config.m_imagePath + m_config.m_imageBeginUp, CXIMAGE_FORMAT_TIF ) )
	{
		m_backgroundImage.Mix( 
			image,
			CxImage::OpDstCopy,
			-m_config.m_begin.left, 
			m_config.m_begin.bottom - m_backgroundImage.GetHeight() );

		InvalidateRect( m_config.m_begin, false );
		UpdateWindow();
	}
	else
	{
		ASSERT( 0 );
	}
}

void CMainDialog::OnOK() 
{
	ASSERT( CApplication::GetInstance() );
	CApplication::GetInstance()->SaveOption( m_config );
	CApplication::GetInstance()->ReleaseHandle();

	//((CApplication*)AfxGetApp())->ReleaseHandle();

	// TODO: Add extra validation here
	if( ! RunLuna() )
	{
		MessageBox( m_config.m_messageBoxFailedRunClient, m_config.m_messageBoxCaption, MB_OK );
	}

	CDialog::OnOK();
}

void CMainDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	switch( nIDCtl )
	{
	case IDC_SCREENSIZE:
		{
			m_ComboScreenSize.DrawItem(lpDrawItemStruct);
			m_comboBox.Draw( GetDC()->m_hDC, m_config.m_resolution.left, m_config.m_resolution.top );

			CxImage& image = m_ComboScreenSize.GetCurrentImage();

			const int left = m_config.m_resolution.left;
			const int top = m_config.m_resolution.top + 2;

			// 버튼 크기를 뺀 영역만 그린다. 25가 버튼 크기
			image.Draw( 
				GetDC()->m_hDC, 
				left, 
				top,
				image.GetWidth(),
				image.GetHeight(),
				CRect(
				left, 
				top,
				left + image.GetWidth() - 25,
				top + image.GetHeight() ) );
			break;
		}
	// 080507 LUJ, 인터페이스 선택 콤보 그리기
	case IDC_INTERFACE:
		{
			m_ComboInterface.DrawItem(lpDrawItemStruct);
			m_InterfaceComboBox.Draw( GetDC()->m_hDC, m_config.m_interface.left, m_config.m_interface.top );

			CxImage& image = m_ComboInterface.GetCurrentImage();

			const int left = m_config.m_interface.left;
			const int top = m_config.m_interface.top + 2;

			// 버튼 크기를 뺀 영역만 그린다. 25가 버튼 크기
			image.Draw( 
				GetDC()->m_hDC, 
				left, 
				top,
				image.GetWidth(),
				image.GetHeight(),
				CRect(
				left, 
				top,
				left + image.GetWidth() - 25,
				top + image.GetHeight() ) );
			break;
		}
	case IDC_HTML_VIEW:
		{
			m_pHTML->SetRedraw( true );
			break;
		}
	}
}

void CMainDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	// 홈페이지
	if( m_config.m_homePage.PtInRect( point ) &&
		m_homePageImage.IsEnabled() )
	{
		ShellExecute( 
			0, 
			_T( "open" ), 
			URL_homepage,
			0,
			0,
			SW_SHOWNORMAL );
	}
	// 계정 등록
	else if(	m_config.m_register.PtInRect( point ) &&
		m_registerImage.IsEnabled() )
	{
		ShellExecute( 
			0, 
			_T( "open" ), 
			URL_registration,
			0,
			0,
			SW_SHOWNORMAL );
	}
	// 시작 버튼
	else if(	m_isReady							&&
		m_config.m_begin.PtInRect( point )	&&
		m_beginImage.IsEnabled() )
	{
		OnOK();
	}
	// 닫기
	else if(	m_config.m_close.PtInRect( point ) &&
		m_closeImage.IsEnabled() )
	{
		OnCancel();
	}
	// 창 모드
	else if(	m_config.m_checkBox.PtInRect( point ) )
	{
		const bool isEnabled = ! m_checkBoxImage.IsEnabled();

		m_checkBoxImage.Enable( isEnabled );
		m_config.m_isWindowMode = isEnabled;

		InvalidateRect( m_config.m_checkBox, false );
	}

	if( m_backgroundImage.IsInside( point.x, point.y ) )
	{
		SendMessage( WM_NCLBUTTONDOWN, HTCAPTION, 0 );
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CMainDialog::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//OnNcLButtonUp( HTCAPTION, point );

	if( m_backgroundImage.IsInside( point.x, point.y ) )
	{
		SendMessage( WM_NCLBUTTONUP, HTCAPTION, 0 );
	}

	CDialog::OnLButtonUp(nFlags, point);
}



void CMainDialog::OnCancel() 
{
	if( packingThreadHandle )
	{
		// 패킹 스레드가 도는 동안 종료할 수 없는 이유:
		//
		// 파일을 패킹하는 스레드는 Executive를 사용하는데, 메인 스레드를 종료하면 이것이 해제된다.
		// 그러나 스레드는 Executive가 제거되었는지 일일이 체크하는 수 밖에 방법이 없다. 이를 뮤텍스나
		// 크리티컬 섹션으로 걸어 해결할 수는 있으나, 패킹하는 참조 단계가 복잡하여 중간에 빠져나오도록
		// 하려면 코드를 많이 수정해야 한다.
		MessageBox( m_config.m_messageBoxFailedExitDuringPacking, m_config.m_messageBoxCaption, MB_OK );
		return;
	}

	g_pFtp->ReleaseExtendDownload();
	g_pFtp->LogOut();

	DeleteFile( GAME_PATCH );
	DeleteFile( LAUNCHER_PATCH );

	CDialog::OnCancel();
}


void CMainDialog::ReconnectFtp()
{
	if( g_pFtp )
	{
		g_pFtp->LogOut();
		delete g_pFtp;
		g_pFtp = NULL;
	}

	g_pFtp = new CFtpClient;


	if( ConnectFtp() == FALSE )
	{
		int m_nRetryCount = 0;

		while( m_nRetryCount < 7 )
		{
			Sleep(1000);
			if( ConnectFtp() )
				break;

			++m_nRetryCount;
		}

		if( m_nRetryCount > 6 )
		{
			MessageBox( m_config.m_messageBoxFailedConnect, m_config.m_messageBoxCaption, MB_OK );
			OnCancel();
			return;
		}
	}

	SetTimer( WM_FTP_VERSIONCHK, 1000, NULL );
}

void CMainDialog::OnTimer(UINT nIDEvent)
{
	switch( nIDEvent )
	{
	case WM_FTP_CONNECT:
		{
			if( gbConnect == TRUE ) return;

			KillTimer( WM_FTP_CONNECT );

			gbConnect = TRUE;

			if( ConnectFtp() == FALSE )
			{
				static int m_nRetryCount;

				while( m_nRetryCount < 7 )
				{
					Sleep(1000);
					if( ConnectFtp() )
						break;
					++m_nRetryCount;
				}

				if( m_nRetryCount > 6 )
				{
					MessageBox( m_config.m_messageBoxFailedConnect, m_config.m_messageBoxCaption, MB_OK );
					OnCancel();
					return;
				}
			}

			if( gbCheckVersion == FALSE )
			{
				SetTimer( WM_FTP_VERSIONCHK, 2000, NULL );
			}
		}
		break;
	case WM_FTP_VERSIONCHK:
		{
			if( gbCheckVersion == TRUE ) return;
			KillTimer( WM_FTP_VERSIONCHK );

			static int m_nDownloadCount;

			if( RETURN_GOOD != g_pFtp->DownloadFile( newverFilename, ".\\" ) )
			{
				if( m_nDownloadCount > 7 )
				{
					gbCheckVersion = FALSE;
					MessageBox( m_config.m_messageBoxInvalidateVersion, m_config.m_messageBoxCaption, MB_OK );
					g_pFtp->LogOut();
					OnCancel();
					return;
				}

				++m_nDownloadCount;
				ReconnectFtp();				
				return;
			}

			gbCheckVersion = TRUE; //fail also true.
			m_nDownloadCount = 0;

			FILE* fpnew = fopen( newverFilename, "r" );
			if( fpnew == NULL )
			{
				MessageBox( m_config.m_messageBoxInvalidateVersion, m_config.m_messageBoxCaption, MB_OK );
				g_pFtp->LogOut();
				OnCancel();
				return;
			}

			// 테스트 서버용 런처는 버전 파일 안에 LNTC00000000의 내용이 있다
			FILE* fpown = fopen( VERSION_FILE, "r" );
			if( fpown == NULL )
			{
				MessageBox( m_config.m_messageBoxFailedVersionFileOpen, m_config.m_messageBoxCaption, MB_OK );
				g_pFtp->LogOut();
				return;
			}

			char temp1[256], temp2[256];
			fscanf( fpnew, "%s", temp1 );
			fscanf( fpown, "%s", temp2 );
			fclose( fpnew );
			fclose( fpown );

			strcpy( g_szLastestVer, temp1 );

			DeleteFile( newverFilename );

			if( strcmp( temp1, temp2 ) == 0 )
			{
				g_pFtp->LogOut();

				if(g_bUsingEnginePack)
					StartPack3DData();
				else
					SetReady();
			}
			else
			{
				//FILE* pf = fopen( GAME_PATCH, "r" );
				//if( pf )
				//{
				//	fclose( pf );
				//	DeleteFile( GAME_PATCH );
				//}

				dwFileSizeMax = g_pFtp->SetupExtendDownload( temp2, ".\\", TRUE );
				if( dwFileSizeMax == RETURN_FAIL )
				{
					sprintf( &temp2[VERSION_HEADER_TEXT_LENGTH], "00000000" );
					dwFileSizeMax = g_pFtp->SetupExtendDownload( temp2, ".\\", TRUE );
					if( dwFileSizeMax == RETURN_FAIL )
					{
						MessageBox( m_config.m_messageBoxFailedFileDownload, m_config.m_messageBoxCaption, MB_OK );
						g_pFtp->LogOut();
						return;
					}
				}

				SetTimer( WM_FTP_FILEDOWNLOAD, 10, NULL );
				//SetStatusText( _T( "패치 받는 중" ), eMSGTXT_DOWNLOADING );

				PutStatus( m_config.m_textDownload );

				gbCheckVersion = FALSE;
			}
		}
		break;
	case WM_FTP_FILEDOWNLOAD:
		{
			///파일다운로드중에만!!!!
			const DWORD recvsize = g_pFtp->WriteExtendDownloadFile();

			PutProgress( ( float )recvsize / dwFileSizeMax );

			if( recvsize > dwFileSizeMax )
			{
				KillTimer( WM_FTP_FILEDOWNLOAD );

				g_pFtp->ReleaseExtendDownload();
				g_pFtp->LogOut();

				//DeleteFile( GAME_PATCH );
				//DeleteFile( LAUNCHER_PATCH );

				if( IDYES == MessageBox( "파일이 잘못 전송되어 다시 다운로드해야합니다", "오류", MB_YESNO ) )
				{
					SetTimer( WM_FTP_VERSIONCHK, 1000, 0 );
				}
				else
				{
					OnCancel();
				}
			}			
			else if( recvsize == dwFileSizeMax )
			{
				KillTimer( WM_FTP_FILEDOWNLOAD );

				g_pFtp->ReleaseExtendDownload();
				g_pFtp->LogOut();

				PutStatus( m_config.m_textExtract );
				PutProgress( 0 );

				// 로그
				{
					const CTime time = CTime::GetCurrentTime();

					CString log;
					log.Format( "%dtreceived size: %d (%d.%d.%d %2d:%2d:%2d)\n",
						GetTickCount(),
						recvsize,
						time.GetYear(),
						time.GetMonth(),
						time.GetDay(),
						time.GetHour(),
						time.GetMinute(),
						time.GetSecond() );

					PutLog( "-------------------------------------------------------------\n" );
					PutLog( log );
					PutLog( "-------------------------------------------------------------\n" );
				}

				BOOL bReturn = ExtractFile( GAME_PATCH );

				DeleteFile( GAME_PATCH );

				if( !bReturn )
				{
					// 로그
					{
						CFile patch;
						const BOOL isExistPatch = patch.Open( GAME_PATCH, CFile::modeRead );

						CString log;
						log.Format( "%dextraction failed: %s(%s)", GetTickCount(), GAME_PATCH, isExistPatch ? "existed" : "not existed" );

						PutLog( log );
					}

					//	압축 풀기 실패.			
					MessageBox( m_config.m_messageBoxFailedExtract, m_config.m_messageBoxCaption, MB_OK );
					OnCancel();
					return;
				}

				if( g_bUsingEnginePack )
					StartPack3DData();
				else
				{
					bReturn = UpdateVersionInfoFile( VERSION_FILE, newverFilename);

					SetReady();
				}
			}

		}
		break;
	case WM_REFRESH:
		{
			if( IsTopParentActive() )
			{
				return;
			}

			// top
			{
				CRect rect( 
					0,
					0,
					m_backgroundImage.GetWidth(),
					m_config.m_html.top );

				InvalidateRect( rect, false );
				UpdateWindow();
			}

			// left
			{
				CRect rect( 
					0,
					m_config.m_html.top,
					m_config.m_html.left,
					m_config.m_html.bottom );

				InvalidateRect( rect, false );
				UpdateWindow();
			}

			// right
			{
				CRect rect( 
					m_config.m_html.right,
					m_config.m_html.top,
					m_backgroundImage.GetWidth(),
					m_config.m_html.bottom );

				InvalidateRect( rect, false );
				UpdateWindow();
			}

			// bottom
			{
				CRect rect( 
					0,
					m_config.m_html.bottom,
					m_backgroundImage.GetWidth(),
					m_backgroundImage.GetHeight() );

				InvalidateRect( rect, false );
				UpdateWindow();
			}
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CMainDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);


	SetTimer( WM_FTP_CONNECT, 1000, NULL );
}


void CMainDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	// 홈페이지 이미지 검사
	{
		if( m_config.m_homePage.PtInRect( point ) )
		{
			if( ! m_homePageImage.IsEnabled() )
			{
				m_homePageImage.Enable( true );

				InvalidateRect( m_config.m_homePage, false );
			}
		}
		else if( m_homePageImage.IsEnabled() )
		{
			m_homePageImage.Enable( false );

			InvalidateRect( m_config.m_homePage, false );
		}
	}

	// 등록 이미지 검사
	{
		if( m_config.m_register.PtInRect( point ) )
		{
			if( ! m_registerImage.IsEnabled() )
			{
				m_registerImage.Enable( true );

				InvalidateRect( m_config.m_register, false );
			}
		}
		else if( m_registerImage.IsEnabled() )
		{
			m_registerImage.Enable( false );

			InvalidateRect( m_config.m_register, false );
		}
	}

	// 닫기 버튼 검사
	{
		if( m_config.m_close.PtInRect( point ) )
		{
			if( ! m_closeImage.IsEnabled() )
			{
				m_closeImage.Enable( true );

				InvalidateRect( m_config.m_close, false );
			}
		}
		else if( m_closeImage.IsEnabled() )
		{
			m_closeImage.Enable( false );

			InvalidateRect( m_config.m_close, false );
		}
	}

	// 시작 버튼 검사
	if( m_isReady )
	{
		if( m_config.m_begin.PtInRect( point ) )
		{
			if( ! m_beginImage.IsEnabled() )
			{
				m_beginImage.Enable( true );

				InvalidateRect( m_config.m_begin, false );
			}
		}
		else if( m_beginImage.IsEnabled() )
		{
			m_beginImage.Enable( false );

			InvalidateRect( m_config.m_begin, false );
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}


int CMainDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	{
		DeleteFile( GAME_PATCH );
		DeleteFile( LAUNCHER_PATCH );
	}

	SetWindowPos( 
		0,
		0,
		0,
		m_backgroundImage.GetWidth(),
		m_backgroundImage.GetHeight(), 
		SWP_NOZORDER );

	{
		m_backgroundImage.DrawString( 
			CPaintDC( this ), 
			m_config.m_windowMode.x, 
			m_config.m_windowMode.y, 
			m_config.m_textWindowMode,
			m_config.m_fontControlColor, 
			m_config.m_fontName, 
			m_config.m_fontSize );
	}

	if( m_config.m_flagShowTitle )
	{
		m_backgroundImage.DrawString( 
			CPaintDC( this ), 
			m_config.m_title.x, 
			m_config.m_title.y, 
			m_config.m_textTitle,
			m_config.m_fontTitleColor, 
			m_config.m_fontName, 
			m_config.m_fontSize );
	}

	SetTimer( WM_REFRESH, 1000, 0 );
	return 0;
}


void CMainDialog::PutProgress( float ratio, bool isInstant, const CString* text )
{
	if( text )
	{
		m_progressText = *text;
	}
	else
	{
		m_progressText = "";
	}

	if( 1.0f < ratio )
	{
		return;
	}

	if( 0 >= ratio ||
		m_progress.m_value > ratio )
	{
		m_backgroundImage.Mix( 
			m_progressImage,
			CxImage::OpDstCopy,
			-m_config.m_progress.left, 
			m_config.m_progress.bottom - m_backgroundImage.GetHeight() );

		m_progress.m_value = 0;

		InvalidateRect( m_config.m_progress, false );
	}

	//static CxImage layer;

	//// 텍스트 표시 위해 이미지를 복구한다
	//{
	//	m_backgroundImage.Mix( 
	//		layer,
	//		CxImage::OpDstCopy,
	//		-m_config.m_progress.left, 
	//		m_config.m_progress.bottom - m_backgroundImage.GetHeight() );
	//}

	static const long step	= m_config.m_flagProgressCellType ? m_progress.m_cellImage.GetWidth() * 2 : 1;
	static const long y		= m_config.m_progress.bottom - m_backgroundImage.GetHeight() - m_progress.m_cellImage.GetHeight() / 2;
	static const long maxX	= m_config.m_progress.right - m_progress.m_cellImage.GetWidth() - step;
	static const long width = m_config.m_progress.Width() - step * 2;

	const long	begin	= step * ( long )( m_progress.m_value * width / step ) + m_config.m_progress.left + step + 1;
	long		end		= step * ( long )( ratio * width / step ) + m_config.m_progress.left + step + 1;

	if( maxX < end )
	{
		end = maxX;
	}

	for(	long x = begin; 
		end > x;
		x += step )
	{
		m_backgroundImage.Mix( 
			m_progress.m_cellImage,
			CxImage::OpDstCopy, 
			-x,
			y );

		if( ! isInstant )
		{
			InvalidateRect( 
				CRect( x - 1, m_config.m_progress.top, x + step + 1, m_config.m_progress.bottom ),
				false );
			UpdateWindow();

			//// 텍스트 내용이 있으면 표시해준다
			//if( ! m_progressText.IsEmpty() )
			//{
			//	m_backgroundImage.Crop( 
			//		CRect( 
			//		m_config.m_progress.left,
			//		m_config.m_progress.bottom - PackingTextSize.cy,
			//		m_config.m_progress.left + PackingTextSize.cx,
			//		m_config.m_progress.bottom ),
			//		&layer );

			//	m_backgroundImage.DrawString( 
			//		0,
			//		m_config.m_progress.left,
			//		m_config.m_progress.top + 5,
			//		m_progressText,
			//		m_config.m_fontStatusColor,
			//		"MS Sans Serif", 
			//		3 );

			//	InvalidateRect( 
			//		CRect( 
			//			m_config.m_progress.left,
			//			m_config.m_progress.bottom - PackingTextSize.cy,
			//			m_config.m_progress.left + PackingTextSize.cx,
			//			m_config.m_progress.bottom ), 
			//		false );
			//	UpdateWindow();
			//}
		}
	}

	if( isInstant )
	{
		InvalidateRect( 
			CRect( begin, m_config.m_progress.top, end, m_config.m_progress.bottom ),
			false );
		UpdateWindow();

		if( ! m_progressText.IsEmpty() )
		{
			InvalidateRect( 
				CRect(	
				m_config.m_progress.left,
				m_config.m_progress.top - PackingTextSize.cy,
				m_config.m_progress.left + PackingTextSize.cx,
				m_config.m_progress.top ),
				false );
			UpdateWindow();
		}
	}

	m_progress.m_value = ratio;
}


void CMainDialog::PutStatus( const TCHAR* text )
{
	m_backgroundImage.Mix( 
		m_statusImage, 
		CxImage::OpDstCopy,
		-m_config.m_status.left,
		m_config.m_status.bottom - m_backgroundImage.GetHeight() );

	m_backgroundImage.DrawString( 
		CPaintDC( this ),
		m_config.m_status.left, 
		m_config.m_status.bottom,
		text,
		m_config.m_fontStatusColor,
		m_config.m_fontName, 
		m_config.m_fontSize );

	InvalidateRect( m_config.m_status, false );
	UpdateWindow();
}


DWORD __stdcall PackingCallBackFunc( DWORD dwCurCount,DWORD dwTotalCount,void* pArg )
{
	if( ! packingThreadHandle )
	{
		return 0;
	}

	CMainDialog* mainDialog = CMainDialog::GetInstance();

	if( mainDialog )
	{
		mainDialog->PutProgress( ( float )dwCurCount / dwTotalCount, false, &packingFileName );
	}
	else if( packingThreadHandle )
	{
		CloseHandle( packingThreadHandle );

		packingThreadHandle = 0;
	}

	return 0;
}


void CMainDialog::OnCbnSelchangeScreensize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_config.m_SelectedResolution = m_ComboScreenSize.GetCurSel();
}


// 080507 LUJ, 인터페이스 선택
void CMainDialog::OnCbnSelchangeInterface()
{
	const CString path( "data\\interface\\windows\\" );

	switch( m_ComboInterface.GetCurSel() )
	{
	case 0:
		{
			m_config.m_SelectedInterface = path + "image_path.bin";
			break;
		}
	case 1:
		{
			m_config.m_SelectedInterface = path + "image_path_grey.bin";
			break;
		}
	case 2:
		{
			m_config.m_SelectedInterface = path + "image_path_pink.bin";
			break;
		}
	}
}


void CMainDialog::PutLog( const TCHAR* log ) const
{
	CStdioFile file;

	if( ! file.Open( LogFile, CFile::modeWrite ) )
	{
		file.Open( LogFile, CFile::modeCreate | CFile::modeWrite );
	}

	{
		file.SeekToEnd();

		file.WriteString( log );
	}
}


// 080306 LUJ, ESC키를 눌러 프로그램을 종료하거나, 엔터키를 눌러 패치 중에 게임을 실행할 수 있어 이를 막는다
BOOL CMainDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if( WM_KEYDOWN == pMsg->message )
	{
		switch( pMsg->wParam )
		{
		case VK_RETURN:
		case VK_ESCAPE:
			{
				return FALSE;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

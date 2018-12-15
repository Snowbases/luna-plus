// MHAutoPatch.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "Interface_RainFTP.h"
#include "../4dyuchiGRX_common/IExecutive.h"
#include "Application.h"
#include "common.h"
#include "MainDialog.h"
#include "atlconv.h"
#include "StdioFileEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CApplication* CApplication::m_self;

CFtpClient*				g_pFtp				= NULL;

char newverFilename[256];

//-------
char g_szCurVer[256];
char g_szLastestVer[256];

I4DyuchiGXExecutive* g_pExecutive = NULL;
HMODULE        g_hExecutiveHandle = NULL;

/////////////////////////////////////////////////////////////////////////////
// CApplication

BEGIN_MESSAGE_MAP(CApplication, CWinApp)
	//{{AFX_MSG_MAP(CApplication)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// 텍스트 중 " " 사이의 문자열을 반환한다. 예를 들어 "aa"가 있으면 aa가 반환된다.
const TCHAR* GetString( const TCHAR* text )
{
	const TCHAR* begin = text;
	const TCHAR* end = text + sizeof( text );

	for( const TCHAR* ch = text; ch; ++ch )
	{
		if( '\"' == *ch )
		{
			begin = ++ch;
			break;
		}
	}

	for( const TCHAR* ch = begin; ch; ++ch )
	{
		if( '\"' == *ch )
		{
			end = --ch;
			break;
		}
	}

	const size_t size = abs( begin - end ) + 1;

	static TCHAR buffer[ MAX_PATH ];
	_tcsncpy( buffer, begin, size );
	
	buffer[ size  ] = '\0';
	return buffer;
}


/////////////////////////////////////////////////////////////////////////////
// CApplication construction

CApplication::CApplication()
{
	// TODO: add construction code here,

	// Place all significant initialization in InitInstance

	ASSERT( ! m_self );

	m_self = this;

	// 080508 LUJ, 초기화. sav 파일이 바뀌지 않아도 실행가능하도록 한다
	{
		const CString path( "data\\interface\\windows\\" );
		const CString blueInterface( path + "image_path.bin" );

		m_config.m_SelectedInterface	= blueInterface;
		m_config.m_SelectedResolution	= 1;
		m_config.m_isWindowMode			= false;
	}

	// 세이브 파일 읽기
	{
		CStdioFile file;

		if( file.Open( "system\\launcher.sav", CStdioFile::modeRead ) )
		{
			CString line;
			TCHAR buffer[ MAX_PATH ] = { 0 };
			const TCHAR* separator = _T( "\n,= " );

			while( file.ReadString( line ) )
			{
				_tcscpy( buffer, line );

				const char* token = _tcstok( buffer, separator );
				
				if( 0 == token		||		// empty line
					';' == token[ 0 ] ) 	// comment
				{
					continue;
				}
				if( ! _tcsicmp( _T( "resolution" ), token ) )
				{
					m_config.m_SelectedResolution = _ttoi( _tcstok( 0, separator ) );
				}
				else if( ! _tcsicmp( _T( "windowMode" ), token ) )
				{
					token = _tcstok( 0, separator );

					m_config.m_isWindowMode = ! _tcsicmp( "true", token );
				}
				// 080507 LUJ, 선택된 이미지 경로 값 가져옴
				else if( ! _tcsicmp( _T( "imagePath" ), token ) )
				{
					token = _tcstok( 0, separator );

					// 080508 LUJ, 값이 있을 경우에만 복사해야함
					if( token )
					{
						m_config.m_SelectedInterface = token;
					}
				}
			}
		}
	}
	
	// 스킨 초기화
	{
		/*
		유니코드 파일을 읽어들이는 클래스

		참조: http://www.ucancode.net/faq/CStdioFile-MFC-Example-ReadString-WriteString.htm
		*/
		CStdioFileEx file;

		if( ! file.Open( "system\\setting.cfg", CStdioFile::modeRead ) )
		{
			ASSERT( 0 && "It's critical" );
			PostQuitMessage( 1 );
			return;
		}

		TCHAR buffer[ MAX_PATH ] = { 0 };
		const TCHAR* separator = _T( "\n,= " );
		CString line;
		CString usingSkin;
		bool foundSkin = false;

		while( file.ReadString( line ) )
		{
			_tcscpy( buffer, line );
					
			const TCHAR* token = _tcstok( buffer, separator );

			if( 0 == token		||	// empty line
				';' == token[ 0 ] ) 	// comment
			{
				continue;
			}
			else if( ! _tcsicmp( _T( "ftp" ), token ) )
			{
				const char* address = _tcstok( 0, separator );

				m_config.m_ftp.AddTail( address );
			}
			else if( ! _tcsicmp( _T( "usingSkin" ), token ) )
			{
				usingSkin = _tcstok( 0, separator );
				continue;
			}

			if( '[' == token[ 0 ] )
			{
				// 같은 스킨 이름을 찾을 때까지 반복
				foundSkin = ( usingSkin == token );
				continue;
			}
			
			if( ! foundSkin )
			{
				continue;
			}
			else if( ! _tcsicmp( _T( "control_title" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );

				m_config.m_title = CPoint( left, top );
			}
			else if( ! _tcsicmp( _T( "control_homepage" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );

				m_config.m_homePage = CRect( left, top, 0, 0 );
			}
			else if( ! _tcsicmp( _T( "control_register" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );

				m_config.m_register = CRect( left, top, 0, 0 );
			}
			else if( ! _tcsicmp( _T( "control_checkbox" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );

				m_config.m_checkBox = CRect( left, top, 0, 0 );
			}
			else if( ! _tcsicmp( _T( "control_begin" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );

				m_config.m_begin = CRect( left, top, 0, 0 );
			}
			else if( ! _tcsicmp( _T( "control_close" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );

				m_config.m_close = CRect( left, top, 0, 0 );
			}
			else if( ! _tcsicmp( _T( "control_progress" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );

				m_config.m_progress = CRect( left, top, 0, 0 );
			}
			else if( ! _tcsicmp( _T( "control_windowMode" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );

				m_config.m_windowMode = CPoint( left, top );
			}
			else if( ! _tcsicmp( _T( "control_html" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );
				const int right = _ttoi( _tcstok( 0, separator ) );
				const int bottom = _ttoi( _tcstok( 0, separator ) );

				m_config.m_html = CRect( left, top, right, bottom );
			}
			else if( ! _tcsicmp( _T( "control_status" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );
				const int right = _ttoi( _tcstok( 0, separator ) );
				const int bottom = _ttoi( _tcstok( 0, separator ) );

				m_config.m_status = CRect( left, top, right, bottom );
			}
			else if( ! _tcsicmp( _T( "control_resolution" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );
				const int right = _ttoi( _tcstok( 0, separator ) );
				const int bottom = _ttoi( _tcstok( 0, separator ) );

				m_config.m_resolution = CRect( left, top, right, bottom );
			}
			// 080507 LUJ, 이미지 경로 선택 컨트롤 위치
			else if( ! _tcsicmp( _T( "control_interface" ), token ) )
			{
				const int left = _ttoi( _tcstok( 0, separator ) );
				const int top = _ttoi( _tcstok( 0, separator ) );
				const int right = _ttoi( _tcstok( 0, separator ) );
				const int bottom = _ttoi( _tcstok( 0, separator ) );

				m_config.m_interface = CRect( left, top, right, bottom );
			}
			else if( ! _tcsicmp( _T( "path_image" ), token ) )
			{
				m_config.m_imagePath = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_background" ), token ) )
			{
				m_config.m_imageBackground = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_close_up" ), token ) )
			{
				m_config.m_imageCloseUp = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_close_down" ), token ) )
			{
				m_config.m_imageCloseDown = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_progressBar" ), token ) )
			{
				m_config.m_imageProgressBar = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_progressCell" ), token ) )
			{
				m_config.m_imageProgressCell = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_homepage_up" ), token ) )
			{
				m_config.m_imageHomepageUp = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_homepage_down" ), token ) )
			{
				m_config.m_imageHomepageDown = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_register_up" ), token ) )
			{
				m_config.m_imageRegisterUp = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_register_down" ), token ) )
			{
				m_config.m_imageRegisterDown = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_combo1" ), token ) )
			{
				m_config.m_imageCombo1 = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_combo2" ), token ) )
			{
				m_config.m_imageCombo2 = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_combo3" ), token ) )
			{
				m_config.m_imageCombo3 = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_combo1selected" ), token ) )
			{
				m_config.m_imageCombo1Selected = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_combo2selected" ), token ) )
			{
				m_config.m_imageCombo2Selected = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_combo3selected" ), token ) )
			{
				m_config.m_imageCombo3Selected = _tcstok( 0, separator );
			}
			// 080507 LUJ, 해상도 변경 콤보 커버
			else if( ! _tcsicmp( _T( "image_combo_cover" ), token ) )
			{
				m_config.m_ImageComboCover = _tcstok( 0, separator );
			}
			// 080507 LUJ, 이미지 경로 선택용 콤보 배경
			else if( ! _tcsicmp( _T( "image_interface_combo1" ), token ) )
			{
				m_config.m_imageInterfaceCombo1 = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_interface_combo2" ), token ) )
			{
				m_config.m_ImageInterfaceCombo2 = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_interface_combo3" ), token ) )
			{
				m_config.m_ImageInterfaceCombo3 = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_interface_combo1selected" ), token ) )
			{
				m_config.m_ImageInterfaceCombo1Selected = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_interface_combo2selected" ), token ) )
			{
				m_config.m_ImageInterfaceCombo2Selected = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_interface_combo3selected" ), token ) )
			{
				m_config.m_ImageInterfaceCombo3Selected = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_interface_combo_cover" ), token ) )
			{
				m_config.m_ImageInterfaceComboCover = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_checkBoxChecked" ), token ) )
			{
				m_config.m_imageCheckBoxChecked = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_checkBoxUnchecked" ), token ) )
			{
				m_config.m_imageCheckBoxUnchecked = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_begin_up" ), token ) )
			{
				m_config.m_imageBeginUp = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "image_begin_down" ), token ) )
			{
				m_config.m_imageBeginDown = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "text_caption" ), token ) )
			{
				m_config.m_textCaption = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_title" ), token ) )
			{
				m_config.m_textTitle = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_check" ), token ) )
			{
				m_config.m_textCheck = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_windowMode" ), token ) )
			{
				m_config.m_textWindowMode += GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_packing" ), token ) )
			{
				m_config.m_textPack = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_begin" ), token ) )
			{
				m_config.m_textBegin = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_download" ), token ) )
			{
				m_config.m_textDownload = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_extract" ), token ) )
			{
				m_config.m_textExtract = GetString( line );
			}
			else if( ! _tcsicmp( _T( "flag_show_title" ), token ) )
			{
				token = _tcstok( 0, separator );

				m_config.m_flagShowTitle = ! _tcsicmp( token, "true" );
			}
			else if( ! _tcsicmp( _T( "flag_progress_cell_type" ), token ) )
			{
				token = _tcstok( 0, separator );

				m_config.m_flagProgressCellType = ! _tcsicmp( token, "true" );
			}
			else if( ! _tcsicmp( _T( "messageBox_caption" ), token ) )
			{
				m_config.m_messageBoxCaption = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_selfUpdateFailed" ), token ) )
			{
				m_config.m_messageBoxSelfUpdateFailed = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_notFoundVersionFile" ), token ) )
			{
				m_config.m_messageBoxNotFoundVersionFile = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_incorrectVersionFile" ), token ) )
			{
				m_config.m_messageBoxIncorrectVersionFile = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileDelete" ), token ) )
			{
				m_config.m_messageBoxFailedVersionFileDelete = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileRename" ), token ) )
			{
				m_config.m_messageBoxFailedVersionFileRename = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedCreateExecutive" ), token ) )
			{
				m_config.m_messageBoxFailedCreateExecutive = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedRunClient" ), token ) )
			{
				m_config.m_messageBoxFailedRunClient = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedExitDuringPacking" ), token ) )
			{
				m_config.m_messageBoxFailedExitDuringPacking = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedConnect" ), token ) )
			{
				m_config.m_messageBoxFailedConnect = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileOpen" ), token ) )
			{
				m_config.m_messageBoxInvalidateVersion = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedFileDownload" ), token ) )
			{
				m_config.m_messageBoxFailedFileDownload = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedExtract" ), token ) )
			{
				m_config.m_messageBoxFailedExtract = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_invalidateVersion" ), token ) )
			{
				m_config.m_messageBoxInvalidateVersion = GetString( line );
			}
			else if( ! _tcsicmp( _T( "font_size" ), token ) )
			{
				const long size = _ttoi( _tcstok( 0, separator ) );

				m_config.m_fontSize = size;
			}
			else if( ! _tcsicmp( _T( "font_name" ), token ) )
			{
				m_config.m_fontName = GetString( line );
			}
			else if( ! _tcsicmp( _T( "font_control_color" ), token ) )
			{
				m_config.m_fontControlColor.rgbRed = ( BYTE )_ttoi( _tcstok( 0, separator ) );
				m_config.m_fontControlColor.rgbGreen = ( BYTE )_ttoi( _tcstok( 0, separator ) );
				m_config.m_fontControlColor.rgbBlue = ( BYTE )_ttoi( _tcstok( 0, separator ) );
				m_config.m_fontControlColor.rgbReserved = 0;
			}
			else if( ! _tcsicmp( _T( "font_status_color" ), token ) )
			{
				m_config.m_fontStatusColor.rgbRed = ( BYTE )_ttoi( _tcstok( 0, separator ) );
				m_config.m_fontStatusColor.rgbGreen = ( BYTE )_ttoi( _tcstok( 0, separator ) );
				m_config.m_fontStatusColor.rgbBlue = ( BYTE )_ttoi( _tcstok( 0, separator ) );
				m_config.m_fontStatusColor.rgbReserved = 0;
			}
			else if( ! _tcsicmp( _T( "font_title_color" ), token ) )
			{
				m_config.m_fontTitleColor.rgbRed = ( BYTE )_ttoi( _tcstok( 0, separator ) );
				m_config.m_fontTitleColor.rgbGreen = ( BYTE )_ttoi( _tcstok( 0, separator ) );
				m_config.m_fontTitleColor.rgbBlue = ( BYTE )_ttoi( _tcstok( 0, separator ) );
				m_config.m_fontTitleColor.rgbReserved = 0;
			}
		}
	}
}


CApplication::~CApplication()
{
	SaveOption( m_config );

	m_self = 0;
}


CApplication* CApplication::GetInstance()
{
	ASSERT( m_self );

	return m_self;
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CApplication object

CApplication theApp;

/////////////////////////////////////////////////////////////////////////////
// CApplication initialization

BOOL CApplication::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	m_hMap = CreateFileMapping( (HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 1024, "LUNA Online Client Application" );
	
	if( m_hMap != NULL && GetLastError() == ERROR_ALREADY_EXISTS ) 
	{ 
		PostQuitMessage(0);
		CloseHandle( m_hMap );
		return FALSE;
	}

	if( strcmp( AfxGetApp()->m_lpCmdLine, "INITFILELIST" ) == 0 )
	{
		PostQuitMessage(0);
		CloseHandle( m_hMap );
		return FALSE;
	}

	CoInitialize( 0 );

	char file[ MAX_PATH ];

	if( GetVersionInfoFile( VERSION_FILE, file ) == FALSE )
	{
		PostQuitMessage(1);
		CloseHandle( m_hMap );
		return FALSE;
	}

	char verHEADER[5];
	memset( verHEADER, 0, 5 );
	strncpy( verHEADER, file, VERSION_HEADER_TEXT_LENGTH );
	sprintf( newverFilename, "%s%s", verHEADER, MH_VERSION_INFO_FINE_NEW );

	if( InitFTP() == FALSE )
	{
		CoUninitialize();
		PostQuitMessage(1);
		CloseHandle( m_hMap );
		return FALSE;
	}

// Dialog
	CMainDialog dlg( 0, m_config );
	m_pMainWnd = &dlg;
	
	const int nResponse = dlg.DoModal();

	UnInitFTP();
	CoUninitialize();

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
	else if(nResponse == IDTRYAGAIN)
	{
		if( -1 == _execl("LunaExecuter.exe","a", 0 ) )

		{
			ASSERT( 0 );
		}
	}

	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.	
	return TRUE;
}


BOOL CApplication::SaveFileList()
{
	{
		CFile file;

		if( ! file.Open( "mht.dll", CFile::modeRead ) )
		{
			return TRUE;
		}
	}
	
	char file[MAX_PATH];
	GetWindowsDirectory( file, MAX_PATH );
	strcat( file, "\\mfl.dat" );

	FILE* fp = fopen( file, "w" );
	if( ! fp )
	{
		return FALSE;
	}
	
	{
		char name[MAX_PATH];
		GetCurrentDirectory( MAX_PATH, name );
		strcat( name, "\\Resource\\*.bin" );

		WIN32_FIND_DATA wfd;
		HANDLE hSrch = FindFirstFile( name, &wfd );

		if( INVALID_HANDLE_VALUE != hSrch )
		{
			DWORD count = 1;
			SYSTEMTIME time;

			do
			{
				FileTimeToSystemTime( &wfd.ftLastWriteTime, &time );
				fprintf( fp, "%d %s\t", count++, wfd.cFileName );
				fprintf( fp, "%4d %02d %02d %02d %02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute );
			}
			while( FindNextFile( hSrch, &wfd ) );
		}
	}	
	
	{
		char name[MAX_PATH];
		GetCurrentDirectory( MAX_PATH, name );
		strcat( name, "\\Resource\\SkillArea\\*.bsad" );

		WIN32_FIND_DATA wfd;
		HANDLE hSrch = FindFirstFile( name, &wfd );
		
		if( INVALID_HANDLE_VALUE != hSrch )
		{
			SYSTEMTIME time;
			DWORD count = 1;

			do
			{
				FileTimeToSystemTime( &wfd.ftLastWriteTime, &time );
				fprintf( fp, "%d %s\t", count++, wfd.cFileName );
				fprintf( fp, "%4d %02d %02d %02d %02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute );
			}
			while( FindNextFile( hSrch, &wfd ) );
		}
	}
	
	fclose( fp );
	return TRUE;
}


BOOL CApplication::GetVersionInfoFile( char* lpszVerInfoFile, char* lpszVerInfo)
{
#ifdef _FOR_TEST_SERVER
	const char* defaultVersion = "LNTC00000000";
#else
	const char* defaultVersion = "LNMC00000000";
#endif

	char		version[ MAX_PATH ] =  { 0 };

	{
		CFile inFile;

		if( ! inFile.Open( lpszVerInfoFile, CFile::modeRead ) )
		{
			CFile outFile( lpszVerInfoFile, CFile::modeCreate | CFile::modeWrite );

			outFile.Write( defaultVersion, strlen( defaultVersion ) );
		}
		// 파일이 없을 경우 기본 정보를 입력하여 생성한다.
		else
		{
			inFile.Read( version, VERSION_TOTAL_LENGTH );
		}
	}
	
	// 버전 파일 정보가 정상인지 체크한다
	if( VERSION_TOTAL_LENGTH == strlen( version ) )
	{
		strcpy( lpszVerInfo, version );	
	}
	else
	{
		CFile outFile( lpszVerInfoFile, CFile::modeCreate | CFile::modeWrite );

		outFile.Write( defaultVersion, strlen( defaultVersion ) );

		strcpy( lpszVerInfo, defaultVersion );
	}

	strcpy( g_szCurVer, lpszVerInfo );
	strcpy( g_szLastestVer, lpszVerInfo );
	
	memcpy( lpszVerInfo + VERSION_TOTAL_LENGTH, ".zip", 4 );
	
	return TRUE;
}


BOOL CApplication::InitFTP()
{
	if( g_pFtp )
	{
		g_pFtp->LogOut();
		delete g_pFtp;
	}

	g_pFtp = new CFtpClient;

	return TRUE;
}

void CApplication::UnInitFTP()
{
	if( g_pFtp )
	{
		g_pFtp->LogOut();
		delete g_pFtp;
		g_pFtp = 0;
	}

	if( g_pExecutive )
	{
		g_pExecutive->Release();
		g_pExecutive = 0;
	}

	if( g_hExecutiveHandle )
	{
		FreeLibrary(g_hExecutiveHandle);
	}	
}


void CApplication::SaveOption( const CApplication::Config& config )
{
	CStringList record;
	CString		text;

	{
		text.Format( "resolution = %d\n", m_config.m_SelectedResolution );

		record.AddTail( "; resolution\n" );
		record.AddTail( ";0 - 800 X 600\n" );
		record.AddTail( ";1 - 1024 x 768\n" );
		record.AddTail( ";2 - 1280 x 1024\n" );
		record.AddTail( text );
		record.AddTail( "\n" );
	}

	{
		text.Format( "windowMode = %s\n", m_config.m_isWindowMode ? "true" : "false" );

		record.AddTail( "; window mode\n" );
		record.AddTail( text );
		record.AddTail( "\n" );
	}

	// 080507 LUJ, 선택된 이미지 경로 저장
	{
		text.Format( "imagePath = %s\n", m_config.m_SelectedInterface );

		record.AddTail( "; image path\n" );
		record.AddTail( text );
	}

	CStdioFile outputFile;

	if( ! outputFile.Open( "system\\launcher.sav", CFile::modeCreate | CFile::modeWrite ) )
	{
		ASSERT( 0 );
		return;
	}

	for( POSITION pos = record.GetHeadPosition(); pos; )
	{
		const CString& line = record.GetNext( pos );

		outputFile.WriteString( line );
	}
}
#include "stdafx.h"
#include "Application.h"
#include "MainDialog.h"
#include "Network.h"

/////////////////////////////////////////////////////////////////////////////
// The one and only CApplication object
CApplication theApp;

/////////////////////////////////////////////////////////////////////////////
// CApplication

BEGIN_MESSAGE_MAP(CApplication, CWinApp)
	//{{AFX_MSG_MAP(CApplication)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

void TokenizePosition(CApplication::Position& position, LPCTSTR seperator)
{
	LPCTSTR left = _tcstok(
		0,
		seperator);
	LPCTSTR top = _tcstok(
		0,
		seperator);
	LPCTSTR right = _tcstok(
		0,
		seperator);
	LPCTSTR bottom = _tcstok(
		0,
		seperator);
	LPCTSTR x = _tcstok(
		0,
		seperator);
	LPCTSTR y = _tcstok(
		0,
		seperator);

	position.mRect.left = _ttoi(left ? left : _T(""));
	position.mRect.top = _ttoi(top ? top : _T(""));
	position.mRect.right = position.mRect.left + _ttoi(right ? right : _T(""));
	position.mRect.bottom = position.mRect.top + _ttoi(bottom ? bottom : _T(""));
	position.mPoint.x = _ttoi(x ? x : _T(""));
	position.mPoint.y = _ttoi(y ? y : _T(""));
}

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
	for( int i = 1; i < __argc; ++i )
	{
		// 081202 LUJ, 옵션 변경
		if( ! _tcsicmp( __targv[ i ], _T( "/log" ) ) )
		{
			CLog::SetEnable( true );
		}
		// 081202 LUJ, 데이터를 메모리에 다운받는다. 패치 파일을 쉽게 획득할 수 없도록 한다.
		//			그러나 패치 크기가 클수록 점유 메모리가 커짐을 유의해야한다
		else if( ! _tcsicmp( __targv[ i ], _T( "/memory" ) ) )
		{
			mConfig.mFileType = Config::FileTypeMemory;

			CLog::Put( _T( "Download data will be store to memory." ) );
		}
		// 081202 LUJ, 도움말을 표시한다
		else
		{
			CLog::Put( _T( "help:" ) );
			CLog::Put( _T( "	/log		put log" ) );
			CLog::Put( _T( "	/memory		download to memory" ) );
		}
	}

	CLog::Put( _T("-----------------------------------------------") );
	CLog::Put( _T( "Luna Launcher %s. Eya Soft Co.Ltd." ), VERSION );
	CLog::Put( _T("-----------------------------------------------") );

#ifdef _LOCAL_JP_
	TCHAR buffer[ 256 ] = { 0 };
	_tcscpy( buffer, GetCommandLine() );

	const TCHAR* separator = _T( " " );
	// 런처위치 경로 획득
	const TCHAR* token = _tcstok( buffer, separator );
	m_Purple.m_LauncherPath = token;
	// 핸들번호 획득
	token = _tcstok( 0, separator );
	CString TargetHandle = token;
	m_Purple.m_hTarget = (HWND)_ttol(TargetHandle);
	// 루나설치 경로 획득
	token = _tcstok( 0, separator );
	m_Purple.m_GameInstallPath = token;
	// 기타인지 획득
	token = _tcstok( 0, separator );
	m_Purple.m_ETCPath = token;
	
	// 루나 설치경로가 존재하면 현재 디렉토리를 루나설치경로로 재설정한다.
	if(!m_Purple.m_GameInstallPath.IsEmpty())
	{
		CString fullPath = m_Purple.m_GameInstallPath;
		fullPath.Replace(_T("\\"),_T("\\\\"));
		fullPath.Remove('"');
		SetCurrentDirectory(fullPath);
	}

	CLog::Put( _T("parameter : %s %s %s %s" ), m_Purple.m_LauncherPath, TargetHandle, m_Purple.m_GameInstallPath, m_Purple.m_ETCPath);
#endif

	// 080515 LUJ, 서비스 지역을 초기화한다
	SetLocal( Local::TypeNone );

	// 080508 LUJ, 초기화. sav 파일이 바뀌지 않아도 실행가능하도록 한다
	{
		const CString path( _T( "data\\interface\\windows\\" ) );
		const CString blueInterface( path + _T( "image_path.bin" ) );

		mConfig.m_SelectedInterface		= blueInterface;
		mConfig.m_isWindowMode			= false;
	}

	// 세이브 파일 읽기
	{
		CStdioFileEx file;

		if( file.Open( _T( "system\\launcher.sav" ), CFile::modeRead | CFile::typeText ) )
		{
			for(CString line;
				file.ReadString(line);)
			{
				TCHAR buffer[ MAX_PATH ] = { 0 };
				_tcscpy( buffer, line );

				LPCTSTR separator = _T( "\n,= " );
				LPCTSTR token = _tcstok( buffer, separator );
				
				if( 0 == token		||		// empty line
					';' == token[ 0 ] ) 	// comment
				{
					continue;
				}
				// 0912221 LUJ, resolution use for both - enumerated or detect type
				else if( ! _tcsicmp( _T( "resolution" ), token ) )
				{
					LPCTSTR textWidth = _tcstok(0, separator);
					LPCTSTR textHeight = _tcstok(0, separator);

					mConfig.mResolution.cx = _ttoi(textWidth ? textWidth : _T(""));
					mConfig.mResolution.cy = _ttoi(textHeight ? textHeight : _T(""));
				}
				else if( ! _tcsicmp( _T( "windowMode" ), token ) )
				{
					token = _tcstok( 0, separator );

					mConfig.m_isWindowMode = ! _tcsicmp( _T( "true" ), token );
				}
				// 080507 LUJ, 선택된 이미지 경로 값 가져옴
				else if( ! _tcsicmp( _T( "imagePath" ), token ) )
				{
					token = _tcstok( 0, separator );

					// 080508 LUJ, 값이 있을 경우에만 복사해야함
					if( token )
					{
						mConfig.m_SelectedInterface = token;
					}
				}
			}

			CLog::Put( _T( "system\\Launcher.sav is loaded" ) );
		}
	}
	
	// 스킨 초기화
	{
		/*
		유니코드 파일을 읽어들이는 클래스

		참조: http://www.ucancode.net/faq/CStdioFile-MFC-Example-ReadString-WriteString.htm
		*/
		CStdioFileEx file;

// 091117 ONS 「LunaLauncher.exe」와 같은 폴더의 「setting.cfg」를 읽어 올 수있도록 수정
// 만약 존재하지 않으면 루나 인스톨 폴더의 setting.cfg를 읽어온다.
#ifdef _LOCAL_JP_

		// LunaLauncher.exe와 같은 폴더의 setting.cfg파일을 연다.
		CString strFilePath;
		CString strPath = m_Purple.m_LauncherPath;

		CLog::Put( _T( "LauncherFilePath : %s" ), strPath );

		if( !strPath.IsEmpty() )
		{
			const int nIndex = strPath.ReverseFind( '\\' );
			strFilePath = strPath.Left(nIndex) + _T( "\\setting.cfg" );
		}

		CLog::Put( _T( "SettingFilePath : %s" ), strFilePath );

		if( !file.Open( strFilePath, CFile::modeRead | CFile::typeText ) )
		{
			CLog::Put( _T( "%s is not found" ), strFilePath );
			if( ! file.Open( _T( "system\\setting.cfg" ), CFile::modeRead | CFile::typeText ) )
			{
				MsgBoxLog( _T( "system\\setting.cfg is not found" ), _T( "" ) );
				
				PostQuitMessage( 1 );
				return;
			}			
		}
		
#else
		if( ! file.Open( _T( "system\\setting.cfg" ), CFile::modeRead | CFile::typeText ) )
		{
			MsgBoxLog( _T( "system\\setting.cfg is not found" ), _T( "" ) );
			
			PostQuitMessage( 1 );
			return;
		}
#endif
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
			else if( ! _tcsicmp( _T( "ftp_address" ), token ) )
			{
				mLocal.mDownload.mPath = _tcstok( 0, separator );
			}			
			else if( ! _tcsicmp( _T( "version_tag" ), token ) )
			{
				mLocal.mDownload.mVersionTag = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "version_filename" ), token ) )
			{
				mLocal.mDownload.mVersionFile = _tcstok( 0, separator );
			}
			else if( ! _tcsicmp( _T( "messageBox_caption" ), token ) )
			{
				mConfig.m_messageBoxCaption = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_selfUpdateFailed" ), token ) )
			{
				mConfig.m_messageBoxSelfUpdateFailed = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_notFoundVersionFile" ), token ) )
			{
				mConfig.m_messageBoxNotFoundVersionFile = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_incorrectVersionFile" ), token ) )
			{
				mConfig.m_messageBoxIncorrectVersionFile = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileDelete" ), token ) )
			{
				mConfig.m_messageBoxFailedVersionFileDelete = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileRename" ), token ) )
			{
				mConfig.m_messageBoxFailedVersionFileRename = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedCreateExecutive" ), token ) )
			{
				mConfig.m_messageBoxFailedCreateExecutive = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedRunClient" ), token ) )
			{
				mConfig.m_messageBoxFailedRunClient = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedExitDuringPacking" ), token ) )
			{
				mConfig.m_messageBoxFailedExitDuringPacking = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedConnect" ), token ) )
			{
				mConfig.m_messageBoxFailedConnect = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileOpen" ), token ) )
			{
				mConfig.m_messageBoxInvalidateVersion = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedFileDownload" ), token ) )
			{
				mConfig.m_messageBoxFailedFileDownload = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedExtract" ), token ) )
			{
				mConfig.m_messageBoxFailedExtract = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_invalidateVersion" ), token ) )
			{
				mConfig.m_messageBoxInvalidateVersion = GetString( line );
			}
			//090819 ONS 퍼플런처관련 메세지 추가
			else if( ! _tcsicmp( _T( "messageBox_message_Pruple_InvalidArgument" ), token ) )
			{
				mConfig.m_messageBoxPrupleInvalidArgument = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_Pruple_InvalidHandle" ), token ) )
			{
				mConfig.m_messageBoxPrupleInvalidHandle = GetString( line );
			}
			else if( ! _tcsicmp( _T( "local" ), token ) )
			{
				const TCHAR* local = _tcstok( 0, separator );

				if( ! _tcsicmp( _T( "Korea" ), local ) )
				{
					SetLocal( Local::TypeKorea );

					CLog::Put( _T( "Local is Korea" ) );
				}
				else if( ! _tcsicmp( _T( "Korea.test" ), local ) )
				{
					SetLocal( Local::TypeKoreaTest );

					CLog::Put( _T( "Local is Korea.Test" ) );
				}
				else if( ! _tcsicmp( _T( "Korea.plus" ), local ) )
				{
					SetLocal( Local::TypeKoreaPlus );

					CLog::Put( _T( "Local is Korea.Plus" ) );
				}
				else if( ! _tcsicmp( _T( "Thai" ), local ) )
				{
					SetLocal( Local::TypeThai );

					CLog::Put( _T( "Local is Thai" ) );
				}
				else if( ! _tcsicmp( _T( "Taiwan" ), local ) )
				{
					SetLocal( Local::TypeTaiwan );

					CLog::Put( _T( "Local is Taiwan" ) );
				}
				else if( ! _tcsicmp( _T( "Taiwan.Test" ), local ) )
				{
					SetLocal( Local::TypeTaiwanTest );

					CLog::Put( _T( "Local is Taiwan.Test" ) );
				}
				else if( ! _tcsicmp( _T( "Malaysia" ), local ) )
				{
					SetLocal( Local::TypeSingapore );

					CLog::Put( _T( "Local is Singapore" ) );
				}
				else if( ! _tcsicmp( _T( "MY" ), local ) )
				{
					SetLocal( Local::TypeMalaysia );

					CLog::Put( _T( "Local is Malaysia" ) );
				}
				else if( !  _tcsicmp( _T( "MYTC" ), local ) )
				{
					SetLocal( Local::TypeMalaysiaTestServer );

					CLog::Put( _T( "Local is Malaysia Test Server" ) );
				}
				else if( ! _tcsicmp( _T( "USA" ), local ) )
				{
					SetLocal( Local::TypeUSA );

					CLog::Put( _T( "Local is USA" ) );
				}
				else if( ! _tcsicmp( _T( "USA.Test" ), local ) )
				{
					SetLocal( Local::TypeUSATest );

					CLog::Put( _T( "Local is USA.Test" ) );
				}
				else if( ! _tcsicmp( _T( "China" ), local ) )
				{
					SetLocal( Local::TypeChina );

					CLog::Put( _T( "Local is China.Test" ) );
				}
				else if( ! _tcsicmp( _T( "China.test" ), local ) )
				{
					SetLocal( Local::TypeChinaTest );

					CLog::Put( _T( "Local is China.Test" ) );
				}
				else if( ! _tcsicmp( _T( "China.ISBN" ), local ) )
				{
					SetLocal( Local::TypeChinaISBN );

					CLog::Put( _T( "Local is China.ISBN" ) );
				}
				else if( ! _tcsicmp( _T( "HongKong.test" ), local ) )
				{
					SetLocal( Local::TypeHongKongTest );

					CLog::Put( _T( "Local is HongKong.Test" ) );
				}
				else if( ! _tcsicmp( _T( "HongKong.Main" ), local ) )
				{
					SetLocal( Local::TypeHongKongMain );

					CLog::Put( _T( "Local is HongKong.Main" ) );
				}
				else if( ! _tcsicmp( _T( "Philippines.Test" ), local ) )
				{
					SetLocal( Local::TypePhilippinesTest );

					CLog::Put( _T( "Local is Philippines.Test" ) );
				}
				else if( ! _tcsicmp( _T( "Philippines.Main" ), local ) )
				{
					SetLocal( Local::TypePhilippinesMain );

					CLog::Put( _T( "Local is Philippines.Main" ) );
				}
				// 090819 ONS 일본로컬 처리 추가
				else if( ! _tcsicmp( _T( "Japan.Test" ), local ) )
				{
					SetLocal( Local::TypeJapanTest );

					CLog::Put( _T( "Local is TypeJapan.Test" ) );
				}
				else if( ! _tcsicmp( _T( "Japan.Main" ), local ) )
				{
					SetLocal( Local::TypeJapanMain );

					CLog::Put( _T( "Local is TypeJapan.Main" ) );
				}

				continue;
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
			else if( ! _tcsicmp( _T( "control_html" ), token ) )
			{
				TokenizePosition(
					mConfig.mHtmlPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "control_resolution_combo" ), token ) )
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mResolutionCombo.x = _ttoi(x ? x : _T(""));
				mConfig.mResolutionCombo.y = _ttoi(y ? y : _T(""));
			}
			else if( ! _tcsicmp( _T( "control_resolution_arrow" ), token ) )
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mResolutionArrow.x = _ttoi(x ? x : _T(""));
				mConfig.mResolutionArrow.y = _ttoi(y ? y : _T(""));
			}
			// 080507 LUJ, 이미지 경로 선택 컨트롤 위치
			else if( ! _tcsicmp( _T( "control_interface_combo" ), token ) )
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mInterfaceCombo.x = _ttoi(x ? x : _T(""));
				mConfig.mInterfaceCombo.y = _ttoi(y ? y : _T(""));
			}
			else if( ! _tcsicmp( _T( "control_interface_arrow" ), token ) )
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mInterfaceArrow.x = _ttoi(x ? x : _T(""));
				mConfig.mInterfaceArrow.y = _ttoi(y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("image"), token))
			{
				mConfig.mImage = _tcstok(0, separator);
			}
			else if( ! _tcsicmp( _T( "image_background" ), token ) )
			{
				TokenizePosition(
					mConfig.mBackgroundPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_close_up" ), token ) )
			{
				TokenizePosition(
					mConfig.mCloseUpPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_close_down" ), token ) )
			{
				TokenizePosition(
					mConfig.mCloseDownPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_checkBoxChecked" ), token ) )
			{
				TokenizePosition(
					mConfig.mCheckedPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_checkBoxUnchecked" ), token ) )
			{
				TokenizePosition(
					mConfig.mUncheckedPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_begin_up" ), token ) )
			{
				TokenizePosition(
					mConfig.mBeginUpPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_begin_down" ), token ) )
			{
				TokenizePosition(
					mConfig.mBeginDownPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_progressBar" ), token ) )
			{
				TokenizePosition(
					mConfig.mProgressPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_progressCell" ), token ) )
			{
				TokenizePosition(
					mConfig.mProgressCellPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_progressCellPack" ), token ) )
			{
				TokenizePosition(
					mConfig.mProgressCellPackPosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "image_titleDecorator" ), token ) )
			{
				TokenizePosition(
					mConfig.mTitleDecoratorCellPosition,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("text_title_position"), token))
			{
				TokenizePosition(
					mConfig.mTitleTextPosition,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_top"), token))
			{
				TokenizePosition(
					mConfig.mTopCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_top_over"), token))
			{
				TokenizePosition(
					mConfig.mTopOverCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_middle"), token))
			{
				TokenizePosition(
					mConfig.mMiddleCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_middle_over"), token))
			{
				TokenizePosition(
					mConfig.mMiddleOverCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_bottom"), token))
			{
				TokenizePosition(
					mConfig.mBottomCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_bottom_over"), token))
			{
				TokenizePosition(
					mConfig.mBottomOverCombo,
					separator);
			}
			else if(0 ==  _tcsicmp(_T("image_combo_cover"), token))
			{
				TokenizePosition(
					mConfig.mCoverCombo,
					separator);
			}
			else if(0 == _tcsicmp(_T("image_arrow"), token))
			{
				TokenizePosition(
					mConfig.mArrowPosition,
					separator);
			}
			else if(0 == _tcsicmp(_T("image_arrow_push"), token))
			{
				TokenizePosition(
					mConfig.mArrowPushedPosition,
					separator);
			}
			else if(0 == _tcsicmp(_T("text_windowMode_position"), token))
			{
				TokenizePosition(
					mConfig.mWindowModePosition,
					separator);
			}
			else if( ! _tcsicmp( _T( "text_caption" ), token ) )
			{
				mConfig.m_textCaption = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_title" ), token ) )
			{
				mConfig.m_textTitle = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_check" ), token ) )
			{
				mConfig.m_textCheck = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_windowMode" ), token ) )
			{
				mConfig.m_textWindowMode += GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_packing" ), token ) )
			{
				mConfig.m_textPack = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_begin" ), token ) )
			{
				mConfig.m_textBegin = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_download" ), token ) )
			{
				mConfig.m_textDownload = GetString( line );
			}
			else if( ! _tcsicmp( _T( "text_extract" ), token ) )
			{
				mConfig.m_textExtract = GetString( line );
			}
			else if(0 == _tcsicmp(_T("text_interface"), token))
			{
                mConfig.mInterfaceTextList.push_back(
					GetString(line));
			}
			else if(0 == _tcsicmp(_T("text_interface_tip_text"), token))
			{
				mConfig.mInterfaceTip.mText = GetString(
					line);
			}
			else if(0 == _tcsicmp(_T("text_interface_tip_text_offset"), token))
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mInterfaceTip.mTextOffset.x = _ttoi(x ? x : _T(""));
				mConfig.mInterfaceTip.mTextOffset.y = _ttoi(y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_interface_tip_image"), token))
			{
				TokenizePosition(
					mConfig.mInterfaceTip.mPosition,
					separator);
			}
			else if(0 == _tcsicmp(_T("text_resolution_tip_text"), token))
			{
				mConfig.mResolutionTip.mText = GetString(
					line);
			}
			else if(0 == _tcsicmp(_T("text_resolution_tip_text_offset"), token))
			{
				LPCTSTR x = _tcstok(
					0,
					separator);
				LPCTSTR y = _tcstok(
					0,
					separator);

				mConfig.mResolutionTip.mTextOffset.x = _ttoi(x ? x : _T(""));
				mConfig.mResolutionTip.mTextOffset.y = _ttoi(y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_resolution_tip_image"), token))
			{
				TokenizePosition(
					mConfig.mResolutionTip.mPosition,
					separator);
			}
			else if(0 == _tcsicmp(_T("text_interface_loading_text"), token))
			{
				mConfig.mLoadingTip.mText	=	GetString( 
					line );
			}
			else if(0 == _tcsicmp(_T("text_interface_loading_text_offset"), token))
			{
				LPCTSTR x	=	_tcstok( 
					0, 
					separator );
				LPCTSTR y	=	_tcstok( 
					0, 
					separator );

				mConfig.mLoadingTip.mTextOffset.x	=	_ttoi( x ? x : _T(""));
				mConfig.mLoadingTip.mTextOffset.y	=	_ttoi( y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_interface_loading_image"), token))
			{
				TokenizePosition( 
					mConfig.mLoadingTip.mPosition, 
					separator );
			}	
			else if(0 == _tcsicmp(_T("text_interface_Packing_text"), token))
			{
				mConfig.mPackingTip.mText	=	GetString( 
					line );
			}
			else if(0 == _tcsicmp(_T("text_interface_Packing_text_offset"), token))
			{
				LPCTSTR	x	=	_tcstok(
					0,
					separator );
				LPCTSTR y	=	_tcstok(
					0,
					separator );

				mConfig.mPackingTip.mTextOffset.x	=	_ttoi( x ? x : _T(""));
				mConfig.mPackingTip.mTextOffset.y	=	_ttoi( y ? y : _T(""));
			}
			else if(0 == _tcsicmp(_T("text_interface_Packing_image"), token))
			{
				TokenizePosition(
					mConfig.mPackingTip.mPosition,
					separator );
			}
			else if(0 == _tcsicmp(_T("text_tip_text"), token))
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR weight = _tcstok(
					0,
					separator);
				LPCTSTR size = _tcstok(
					0,
					separator);

				Config::Font& font = mConfig.mInterfaceTip.mFont;
				font.mColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				font.mColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				font.mColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				font.mWeight = _ttoi(weight ? weight : _T(""));
				font.mSize = _ttoi(size ? size : _T(""));

				mConfig.mResolutionTip.mFont = font;
				
				mConfig.mLoadingTip.mFont	=	font;
				mConfig.mPackingTip.mFont	=	font;
			}
			else if(0 == _tcsicmp(_T("text_tip_text_outline"), token))
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR border = _tcstok(
					0,
					separator);

				Config::Font& font = mConfig.mInterfaceTip.mFont;
				font.mBorderColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				font.mBorderColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				font.mBorderColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				font.mBorderSize = _ttoi(border ? border : _T(""));

				mConfig.mResolutionTip.mFont = font;

				mConfig.mLoadingTip.mFont	=	font;
				mConfig.mPackingTip.mFont	=	font;
			}
			else if( ! _tcsicmp( _T( "messageBox_caption" ), token ) )
			{
				mConfig.m_messageBoxCaption = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_selfUpdateFailed" ), token ) )
			{
				mConfig.m_messageBoxSelfUpdateFailed = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_notFoundVersionFile" ), token ) )
			{
				mConfig.m_messageBoxNotFoundVersionFile = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_incorrectVersionFile" ), token ) )
			{
				mConfig.m_messageBoxIncorrectVersionFile = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileDelete" ), token ) )
			{
				mConfig.m_messageBoxFailedVersionFileDelete = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileRename" ), token ) )
			{
				mConfig.m_messageBoxFailedVersionFileRename = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedCreateExecutive" ), token ) )
			{
				mConfig.m_messageBoxFailedCreateExecutive = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedRunClient" ), token ) )
			{
				mConfig.m_messageBoxFailedRunClient = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedExitDuringPacking" ), token ) )
			{
				mConfig.m_messageBoxFailedExitDuringPacking = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedConnect" ), token ) )
			{
				mConfig.m_messageBoxFailedConnect = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedVersionFileOpen" ), token ) )
			{
				mConfig.m_messageBoxInvalidateVersion = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedFileDownload" ), token ) )
			{
				mConfig.m_messageBoxFailedFileDownload = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_failedExtract" ), token ) )
			{
				mConfig.m_messageBoxFailedExtract = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_invalidateVersion" ), token ) )
			{
				mConfig.m_messageBoxInvalidateVersion = GetString( line );
			}
			//090819 ONS 퍼플런처관련 메세지 추가
            else if( ! _tcsicmp( _T( "messageBox_message_Pruple_InvalidArgument" ), token ) )
			{
				mConfig.m_messageBoxPrupleInvalidArgument = GetString( line );
			}
			else if( ! _tcsicmp( _T( "messageBox_message_Pruple_InvalidHandle" ), token ) )
			{
				mConfig.m_messageBoxPrupleInvalidHandle = GetString( line );
			}
			else if( ! _tcsicmp( _T( "font_name" ), token ) )
			{
				mConfig.m_fontName = GetString( line );
			}
			else if( ! _tcsicmp( _T( "font_control_color" ), token ) )
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR weight = _tcstok(
					0,
					separator);
				LPCTSTR size = _tcstok(
					0,
					separator);

				mConfig.mControlFont.mColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mControlFont.mColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mControlFont.mColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mControlFont.mWeight = _ttoi(weight ? weight : _T(""));
				mConfig.mControlFont.mSize = _ttoi(size ? size : _T(""));
			}
			else if( ! _tcsicmp( _T( "font_control_outline" ), token ) )
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR border = _tcstok(
					0,
					separator);

				mConfig.mControlFont.mBorderColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mControlFont.mBorderColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mControlFont.mBorderColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mControlFont.mBorderSize = _ttoi(border ? border : _T(""));
			}
			else if( ! _tcsicmp( _T( "font_title_color" ), token ) )
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR weight = _tcstok(
					0,
					separator);
				LPCTSTR size = _tcstok(
					0,
					separator);

				mConfig.mTitleFont.mColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mTitleFont.mColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mTitleFont.mColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mTitleFont.mWeight = _ttoi(weight ? weight : _T(""));
				mConfig.mTitleFont.mSize = _ttoi(size ? size : _T(""));
			}
			else if( ! _tcsicmp( _T( "font_title_outline" ), token ) )
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR border = _tcstok(
					0,
					separator);

				mConfig.mTitleFont.mBorderColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mTitleFont.mBorderColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mTitleFont.mBorderColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mTitleFont.mBorderSize = _ttoi(border ? border : _T(""));
			}
			else if(0 == _tcsicmp(_T("font_windowMode_color"), token))
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR weight = _tcstok(
					0,
					separator);
				LPCTSTR size = _tcstok(
					0,
					separator);

				mConfig.mWindowModeFont.mColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mWindowModeFont.mColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mWindowModeFont.mColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mWindowModeFont.mWeight = _ttoi(weight ? weight : _T(""));
				mConfig.mWindowModeFont.mSize = _ttoi(size ? size : _T(""));
			}
			else if( ! _tcsicmp( _T( "font_windowMode_outline" ), token ) )
			{
				LPCTSTR red = _tcstok(
					0,
					separator);
				LPCTSTR green = _tcstok(
					0,
					separator);
				LPCTSTR blue = _tcstok(
					0,
					separator);
				LPCTSTR border = _tcstok(
					0,
					separator);

				mConfig.mWindowModeFont.mBorderColor.rgbRed = BYTE(_ttoi(red ? red : _T("")));
				mConfig.mWindowModeFont.mBorderColor.rgbGreen = BYTE(_ttoi(green ? green : _T("")));
				mConfig.mWindowModeFont.mBorderColor.rgbBlue = BYTE(_ttoi(blue ? blue : _T("")));
				mConfig.mWindowModeFont.mBorderSize = _ttoi(border ? border : _T(""));
			}
		}

		CLog::Put( _T( "system\\setting.cfg is loaded" ) );		
	}
}

CApplication::~CApplication()
{
	// 080515 LUJ, 할당된 메모리 초기화
	for(
		POSITION position = mDownloadVariableList.GetHeadPosition();
		position; )
	{
		DownloadVariable* variable = mDownloadVariableList.GetNext( position );

		if( ! variable )
		{
			continue;
		}

		delete variable;
		variable = 0;

		CLog::Put( _T( "allocated memories are released" ) );
	}

	// 080515 LUJ, 선택 사항을 저장한다
	{
		CStringList record;
		CString		text;

		{
			text.Format(
				_T("resolution = %d %d\r\n"),
				mConfig.mResolution.cx,
				mConfig.mResolution.cy);

			record.AddTail( _T( "; resolution\r\n" ) );
			record.AddTail( _T( ";0 - 800 X 600\r\n" ) );
			record.AddTail( _T( ";1 - 1024 x 768\r\n" ) );
			record.AddTail( _T( ";2 - 1280 x 1024\r\n" ) );
			record.AddTail( text );
			record.AddTail( _T( "\r\n" ) );
		}

		{
			text.Format( _T( "windowMode = %s\r\n" ), mConfig.m_isWindowMode ? _T( "true" ) : _T( "false" ) );

			record.AddTail( _T( "; window mode\r\n" ) );
			record.AddTail( text );
			record.AddTail( _T( "\r\n" ) );
		}

		// 080507 LUJ, 선택된 이미지 경로 저장
		{
			text.Format( _T( "imagePath = %s\r\n" ), mConfig.m_SelectedInterface );

			record.AddTail( _T( "; image path\r\n" ) );
			record.AddTail( text );
		}

		CStdioFileEx outputFile;

		if( ! outputFile.Open( _T( "system\\launcher.sav" ), CFile::modeCreate | CFile::modeWrite ) )
		{
			ASSERT( 0 && _T( "system\\launcher.sav is not found" ) );
			return;
		}

		for( POSITION pos = record.GetHeadPosition(); pos; )
		{
			const CString& line = record.GetNext( pos );

			outputFile.WriteString( line );
		}

		CLog::Put( _T( "Launcher.sav is saved" ) );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CApplication initialization

BOOL CApplication::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifndef _FOR_SERVER_
	m_hMap = CreateFileMapping(
		(HANDLE)0xFFFFFFFF,
		NULL,
		PAGE_READWRITE,
		0,
		1024,
		_T( "LUNA Online Client Application" ) );
	
	if( Local::TypeNone == mLocal.mType )
	{
		MsgBoxLog( _T( "Local is not defined in system\\setting.cfg" ), _T( "" ) );

		//CLog::Put( _T( "\tLocal is not set. Program'll exit. Check this in system\\setting.cfg: local = [type]" ) );

		PostQuitMessage( 0 );
		CloseHandle( m_hMap );

		return FALSE;
	}
	else if( m_hMap != NULL && GetLastError() == ERROR_ALREADY_EXISTS ) 
	{ 
		PostQuitMessage( 0 );
		CloseHandle( m_hMap );

		return FALSE;
	}
	else if( ! _tcsicmp( AfxGetApp()->m_lpCmdLine, _T( "INITFILELIST" ) ) )
	{
		PostQuitMessage( 0 );
		CloseHandle( m_hMap );

		return FALSE;
	}

	CoInitialize( 0 );
#endif

	CMainDialog dlg;
	m_pMainWnd = &dlg;

	//SetActiveWindow(
	//	m_pMainWnd->GetSafeHwnd());
	
	const INT_PTR response = dlg.DoModal();

	CoUninitialize();

	if ( response == IDOK )
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if ( response == IDCANCEL )
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if( response == IDRETRY )
	{
		CLog::Put( _T( "LunaExecuter is running to patch LunaLauncher.exe" ) );

		STARTUPINFO	sp = { 0 };
		sp.cb			= sizeof(STARTUPINFO);
		sp.dwFlags		= STARTF_USEPOSITION | STARTF_USESIZE | STARTF_USESTDHANDLES; //STARTF_USESHOWWINDOW 
		sp.wShowWindow	= SW_SHOW; 

		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof( pi ) );

		wchar_t CmdLine[256] = _T(" ");
		if( theApp.GetLocal().mType == CApplication::Local::TypeTaiwan || 
			theApp.GetLocal().mType == CApplication::Local::TypeTaiwanTest 
#ifdef _KOR_WEBLAUNCHER_
			// 100503 ShinJS --- 웹런처 사용시 로그인키 전달
			|| theApp.GetLocal().mType == CApplication::Local::TypeKorea ||
			theApp.GetLocal().mType == CApplication::Local::TypeKoreaTest ||
			theApp.GetLocal().mType == CApplication::Local::TypeKoreaPlus 
			)
#else
			)
#endif
		{
			wcscat( CmdLine, AfxGetApp()->m_lpCmdLine );
		}

		if( ! CreateProcess( 
			_T( "LunaExecuter.exe" ),
			CmdLine,											// command line string
			0,													// SD
			0,													// SD
			FALSE,												// handle inheritance option
			CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,  // creation flags
			0,													// new environment block
			0,													// current directory name
			&sp,												// startup information
			&pi	) )												// process information
		{
			CLog::Put( _T( "LunaExecuter.exe is failed to run" ) );
		}
	}
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.	
	return TRUE;
}

// 080515 LUJ, 서비스 지역에 따른 정보를 반환한다
const CApplication::Local& CApplication::GetLocal() const
{
	return mLocal;
}

// 080515 LUJ, 서비스 지역을 설정한다
void CApplication::SetLocal( Local::Type type )
{
	mLocal.mType = type;

	switch( type )
	{
	case Local::TypeNone:
		{
			{
				Local::Download& download = mLocal.mDownload;

				download.mPath			= _T( "" );
				download.mVersionTag	= _T( "" );
				download.mVersionFile	= _T( "" );
			}

			{
				Local::URL& url = mLocal.mURL;

				url.mJoin		= _T( "" );
				url.mHomepage	= _T( "" );
				url.mContent	= _T( "" );
			}
			
			break;
		}
	case Local::TypeKorea:
		{
			{
				Local::Download& download = mLocal.mDownload;

				download.mPath			= _T( "ftp://eyaftp.nefficient.co.kr/pub/eya/LUNA/Patch/" );
				download.mVersionTag	= _T( "LNMC" );
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				url.mJoin		= _T( "http://luna.eyainteractive.com/member_zone/join_01.asp" );
				url.mHomepage	= _T( "http://luna.eyainteractive.com" );
				url.mContent	= _T( "http://luna.eyainteractive.com/img/launcher/launcher01.htm" );
			}

			break;
		}
	case Local::TypeKoreaTest:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080515 LUJ, 테스트 완료 후 FTP 프로토콜로 받도록 할 것
				download.mPath			= _T( "ftp://eyaftp.nefficient.co.kr/pub/eya/LUNA/TestPatch/" );
				//download.mPath			= _T( "http://luna.eyainteractive.com/ver3/lunaprog/" );
				download.mVersionTag	= _T( "LBTC" );
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				//download.mVersionFile	= _T( "MHVerInfo_New.txt" );
			}

			{
				Local::URL& url = mLocal.mURL;

				url.mJoin		= _T( "http://join.enpang.com/member/?gameKey=5" );
				url.mHomepage	= _T( "http://lunaplus.enpang.com/" );
				url.mContent	= _T( "http://lunaplus.enpang.com/launcher/" );
			}

			break;
		}
	case Local::TypeKoreaPlus:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080515 LUJ, 테스트 완료 후 FTP 프로토콜로 받도록 할 것
				download.mPath			= _T( "ftp://eyaftp.nefficient.co.kr/pub/eya/LUNAplus/Patch/" );
				//download.mPath			= _T( "http://luna.eyainteractive.com/ver3/lunaprog/" );
				download.mVersionTag	= _T( "LBMC" );
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				//download.mVersionFile	= _T( "MHVerInfo_New.txt" );
			}

			{
				Local::URL& url = mLocal.mURL;

				url.mJoin		= _T( "http://join.enpang.com/member/?gameKey=5" );
				url.mHomepage	= _T( "http://lunaplus.enpang.com/" );
				url.mContent	= _T( "http://lunaplus.enpang.com/launcher/" );
			}

			break;
		}
	case Local::TypeThai:
		{
			{
				Local::Download& download = mLocal.mDownload;

				download.mPath			= _T( "http://202.43.34.110/patch/" );
				download.mVersionTag	= _T( "LTHC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.txt" );
			}

			{
				Local::URL& url = mLocal.mURL;

				url.mJoin		= _T( "http://www.lunaonline.in.th/account/memlogin.aspx" );
				url.mHomepage	= _T( "http://www.lunaonline.in.th" );
				url.mContent	= _T( "http://www.lunaonline.in.th/news/what_news.aspx" );
			}

			break;
		}
	case Local::TypeTaiwan:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://lapatch.omg.com.tw/pub/luna/" );
				download.mVersionTag	= _T( "LTWC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "http://luna.omg.com.tw/game/game_a01.aspx" );
				url.mHomepage	= _T( "http://luna.omg.com.tw/" );
				url.mContent	= _T( "http://luna.omg.com.tw/bulletin/" );
			}

			break;
		}
	case Local::TypeTaiwanTest:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://210.242.174.113/" );
				download.mVersionTag	= _T( "LTWT" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "http://luna.omg.com.tw/game/game_a01.aspx" );
				url.mHomepage	= _T( "http://luna.omg.com.tw/" );
				url.mContent	= _T( "http://luna.omg.com.tw/bulletin/" );
			}

			break;
		}
	case Local::TypeMalaysia:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://lunapatchmy:ukmnal8bVhA@203.188.237.135/MYPatchDownload/" );
				download.mVersionTag	= _T( "MYMC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "http://www.runup.com.my" );
				url.mHomepage	= _T( "http://luna.runup.com.my" );
				url.mContent	= _T( "http://www.on9g.com/bulletin/luna/my/bulletin.html" );
			}

			break;
		}

	case Local::TypeMalaysiaTestServer:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://mlftp:mlftp@203.188.234.235/PatchDownload/" );
				download.mVersionTag	= _T( "MYTC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MYTCMHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "http://www.runup.com.my" );
				url.mHomepage	= _T( "http://luna.runup.com.my" );
				url.mContent	= _T( "http://www.on9g.com/bulletin/luna/my/bulletin.html" );
			}

			break;
		}
	case Local::TypeSingapore:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://lunapatchmy:ukmnal8bVhA@203.188.237.135/PatchDownload/" );
				download.mVersionTag	= _T( "MLMC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "http://www.runup.com.sg" );
				url.mHomepage	= _T( "http://luna.runup.com.sg" );
				url.mContent	= _T( "http://www.on9g.com/bulletin/luna/sg/bulletin.html" );
			}

			break;
		}
	case Local::TypeUSA:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "http://lunadownload.gpotato.com/" );
				download.mVersionTag	= _T( "USMC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.txt" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "http://register.gpotato.com/index.php?m=Register&a=Registration" );
				url.mHomepage	= _T( "http://luna.gpotato.com" );
				url.mContent	= _T( "http://luna.gpotato.com/launcher.html" );
			}

			break;
		}
	case Local::TypeUSATest:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://lunatest:lunatestpatch_galanet!@38.112.59.182/PatchDownload/" );
				download.mVersionTag	= _T( "USTC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "http://register.gpotato.com/index.php?m=Register&a=Registration" );
				url.mHomepage	= _T( "http://luna.gpotato.com" );
				url.mContent	= _T( "http://luna.gpotato.com/launcher.html" );
			}

			break;
		}
	case Local::TypeChina:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://patch.fsjoy.com/lunapatch/" );
				download.mVersionTag	= _T( "CNTC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "http://passport.t2cn.com/login/register_luna.jsp" );
				url.mHomepage	= _T( "http://www.lunajoy.com/" );
				url.mContent	= _T( "http://www.lunajoy.com/notice" );
			}

			break;
		}
	case Local::TypeChinaTest:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://T2CN:WReQe8red6@222.73.107.174/PatchDownload/" );
				download.mVersionTag	= _T( "CNTC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "" );
				url.mHomepage	= _T( "" );
				url.mContent	= _T( "" );
			}

			break;
		}
	case Local::TypeChinaISBN:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://T2CN:WReQe8red6@125.64.15.183/PatchDownload/" );
				download.mVersionTag	= _T( "ISBN" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "" );
				url.mHomepage	= _T( "" );
				url.mContent	= _T( "" );
			}

			break;
		}
	case Local::TypeHongKongTest:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://ftp-luna:ftplunaqsz208@58.86.38.98/PatchDownload/" );
				download.mVersionTag	= _T( "HKTC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "" );
				url.mHomepage	= _T( "" );
				url.mContent	= _T( "" );
			}

			break;
		}
	case Local::TypeHongKongMain:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://lunapatch:lunaq!wqaz@lunapatch.gamecyber.net/" );
				download.mVersionTag	= _T( "HKMC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "https://www.gamecyber.com.hk/" );
				url.mHomepage	= _T( "http://luna.gamecyber.net/" );
				url.mContent	= _T( "http://luna.gamecyber.net/patch/" );
			}

			break;
		}	
	case Local::TypePhilippinesTest:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://testlunapatch.runup.com.ph/" );
				download.mVersionTag	= _T( "PHTC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "http://www.runup.com.ph/register/register.php" );
				url.mHomepage	= _T( "http://luna.runup.com.ph/" );
				url.mContent	= _T( "http://luna.runup.com.ph/includes/bulletin.php" );
			}

			break;
		}	
	case Local::TypePhilippinesMain:
		{
			{
				Local::Download& download = mLocal.mDownload;

				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "ftp://lunaclient:112233@lunapatch.runup.com.ph/" );
				download.mVersionTag	= _T( "PHMC" );
				// 080515 LUJ, HTTP로는 *.ver 파일을 내려받을 수 없다
				download.mVersionFile	= _T( "MHVerInfo_New.ver" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "http://www.runup.com.ph/register/register.php" );
				url.mHomepage	= _T( "http://luna.runup.com.ph/" );
				url.mContent	= _T( "http://luna.runup.com.ph/includes/bulletin.php" );
			}

			break;
		}	
	// 090831 ONS 일본로컬 서버정보 추가
	case Local::TypeJapanTest:
		{
			{
				Local::Download& download = mLocal.mDownload;
				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "http://pubdownt.hangame.co.jp/downbase/purple/dist/j_luna/" );
				download.mVersionTag	= _T( "JPTC" );
				download.mVersionFile	= _T( "MHVerInfo_New.txt" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "" );
				url.mHomepage	= _T( "" );
				url.mContent	= _T( "" );
			}

			break;
		}	
	case Local::TypeJapanMain:
		{
			{
				Local::Download& download = mLocal.mDownload;
				// 080912 LUJ, FTP 계정과 암호가 있을 때는 아래와 같이 경로를 작성해준다
				//				ftp://[계정]:[암호]@[주소]
				download.mPath			= _T( "http://down.hangame.co.jp/jp/purple/dist/j_luna/" );
				download.mVersionTag	= _T( "JPMC" );
				download.mVersionFile	= _T( "MHVerInfo_New.txt" );
			}

			{
				Local::URL& url = mLocal.mURL;

				// 080919 LUJ, 가입 웹페이지 주소 수정
				url.mJoin		= _T( "" );
				url.mHomepage	= _T( "" );
				url.mContent	= _T( "" );
			}
			break;
		}	
	}
}

// 080515 LUJ, 파일을 다운로드받기 위해 스레드를 생성한다
// 081201 LUJ, 다운로드 실패 시 처리할 함수를 별도로 지정한다
void CApplication::Download( const TCHAR* path, const TCHAR* file, CApplication::DiskCallBackFunction diskFunction, CApplication::MemoryCallBackFunction memoryFunction, CApplication::ProgressCallBackFunction progressFunction, CApplication::Config::FileType fileType )
{
	DownloadVariable* variable = new DownloadVariable;

	if( ! variable )
	{
		ASSERT( 0 && _T( "DownloadVariable is 0" ) );
		return;
	}

	mDownloadVariableList.AddTail( variable );
	
	variable->mPath				= path;
	variable->mFile				= file;
	variable->mDiskCallBack		= diskFunction;
	variable->mMemoryCallBack	= memoryFunction;
	variable->mProgressCallBack	= progressFunction;
	variable->mType				= DownloadVariable::TypeNone;

	switch( fileType )
	{
	case Config::FileTypeDisk:
		{
			variable->mType = DownloadVariable::TypeDisk;
			break;
		}
	case Config::FileTypeMemory:
		{
			variable->mType = DownloadVariable::TypeMemory;
			break;
		}
	}

	AfxBeginThread(
		CApplication::Download,
		( LPVOID )variable );
}

// 080515 LUJ, 파일을 다운로드받고 완료 후 콜백 함수에 결과를 전달한다. 스레드를 생성해서 실행되어야 한다
UINT CApplication::Download( LPVOID parameter )
{
	const DownloadVariable* variable = ( DownloadVariable* )parameter;

	if( ! variable )
	{
		ASSERT( 0 && _T( "DownloadVariable is 0" ) );
		return 1;
	}

	CNetwork::GetInstance().Download(
		variable->mPath,
		variable->mFile,
		variable->mProgressCallBack,
		DownloadVariable::TypeMemory == variable->mType);

	switch(variable->mType)
	{
	case DownloadVariable::TypeMemory:
		{
			if(const MemoryCallBackFunction memoryFunction	= variable->mMemoryCallBack)
			{
				( *memoryFunction )( CNetwork::GetInstance().GetMemoryFile() );
			}

			break;
		}
	case DownloadVariable::TypeDisk:
		{
			if(const DiskCallBackFunction diskFunction	= variable->mDiskCallBack)
			{
				( *diskFunction )( variable->mFile );
			}

			break;
		}
	}

	return 0;
}

// 080515 LUJ, 설정 사항을 반환한다
CApplication::Config& CApplication::GetConfig()
{
	return mConfig;
}

// 080625 LUJ, 네트워크 DLL을 해제한다
void CApplication::ReleaseNetwork() const
{
	CNetwork::GetInstance().Release();
}

#ifdef _LOCAL_JP_
// 090831 ONS 인자로 전달된 URL을 설정한다.
void CApplication::SetLocalEx()
{
	if(m_Purple.m_ETCPath.IsEmpty()) return;

	Local::Download& download = mLocal.mDownload;
	download.mPath = m_Purple.m_ETCPath;
}
#endif


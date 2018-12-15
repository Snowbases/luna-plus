#include "stdafx.h"
#include "VideoCaptureManager.h"
#include "ChatManager.h"
#include "VideoCaptureDlg.h"
#include "GameIn.h"

CVideoCaptureManager::CVideoCaptureManager()
{
}

CVideoCaptureManager::~CVideoCaptureManager()
{
}

BOOL CVideoCaptureManager::IsCapturing()
{
	return m_bandiCaptureLibrary.IsCapturing();
}

BOOL CVideoCaptureManager::CaptureStart()
{
	// 캡처를 시작합니다.
	if( m_bandiCaptureLibrary.IsCapturing() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1999 ) );		//"녹화중입니다."
		return FALSE;
	}

	void* pd3d9Device = g_pExecutive->GetRenderer()->GetD3DDevice();
	if( !pd3d9Device )
		return FALSE;

	// 최초 실행시 DLL 을 로드하고 내부 인스턴스를 생성합니다. 
	if( m_bandiCaptureLibrary.IsCreated() == FALSE )
	{
		// 실패시 DLL과 .H파일의 버전이 동일한지 여부등을 확인해 보시기 바랍니다.
		if( FAILED( m_bandiCaptureLibrary.Create( BANDICAP_RELEASE_DLL_FILE_NAME ) ) )
		{
#ifdef _GMTOOL_
			CHATMGR->AddMsg( CTC_SYSMSG, "Video Capture DLL Load Error" );
#endif
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2000 ) );		// "녹화에 실패하였습니다."
			return FALSE;
		}

		// 100218 ShinJS --- 반디 소프트 인증키 등록
		if( FAILED( m_bandiCaptureLibrary.Verify( "EYA_LUNAPLUS_20100218", "e5b68af6" ) ) )
		{
#ifdef _GMTOOL_
			CHATMGR->AddMsg( CTC_SYSMSG, "Video Capture DLL Verify Failed" );
#endif
		}
	}

	if( m_bandiCaptureLibrary.IsCreated() )
	{
		BCAP_CONFIG cfg;
		m_bandiCaptureLibrary.SetMinMaxFPS( 30, 60 );	// 최소, 최대 프레임 설정

		// 녹화 옵션 설정
		SetCaptureOpt();

		// 저장할 Direct를 생성한다.
		char strDirectory[MAX_PATH];
		sprintf( strDirectory, "%sScreenShot", DIRECTORYMGR->GetFullDirectoryName( eLM_Root ) );
		if( ! ::CreateDirectory( strDirectory, NULL ) &&
			GetLastError() != ERROR_ALREADY_EXISTS )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2000 ) );		// "녹화에 실패하였습니다."
			return FALSE;
		}

		// 현재시간을 이용해서 파일명을 만듭니다. 
		TCHAR pathName[MAX_PATH];
		m_bandiCaptureLibrary.MakePathnameByDate( strDirectory, _T("Capture"), _T("avi"), pathName, MAX_PATH);


		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2001 ), pathName );		// "녹화를 시작합니다. %s 에 저장됩니다."

		// 캡처를 시작합니다.
		HRESULT hr = m_bandiCaptureLibrary.Start( pathName, NULL, BCAP_MODE_D3D9_SCALE, (LONG_PTR)pd3d9Device );
		if( FAILED( hr ) )
			return FALSE;
	}

	return TRUE;
}

void CVideoCaptureManager::CaptureStop()
{
	m_bandiCaptureLibrary.Stop();

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2002 ) );		// "녹화를 종료합니다."
}

void CVideoCaptureManager::Process()
{
	if( !m_bandiCaptureLibrary.IsCapturing() )
		return;

	// DX Device 구하기
	void* pd3d9Device = g_pExecutive->GetRenderer()->GetD3DDevice();
	if( !pd3d9Device )
		return;

	m_bandiCaptureLibrary.Work( (LONG_PTR)pd3d9Device );
}

void CVideoCaptureManager::SetCaptureOpt()
{
	CVideoCaptureDlg* pVideoCaptureDlg = GAMEIN->GetVideoCaptureDlg();
	if( !pVideoCaptureDlg )
		return;

	BCAP_CONFIG cfg;
	m_bandiCaptureLibrary.GetConfig( &cfg );

	// 해상도 설정
	switch( (CVideoCaptureDlg::eVideoCaptureSize)pVideoCaptureDlg->GetSelecedSize() )
	{
	case CVideoCaptureDlg::eVideoCaptureSize_Default:
		{
			cfg.Default();
		}
		break;

	case CVideoCaptureDlg::eVideoCaptureSize_500x375:
		{
			cfg.VideoSizeW = 500;
			cfg.VideoSizeH = 375;
		}
		break;
	}

	// 화질 설정
	switch( (CVideoCaptureDlg::eVideoCaptureOpt)pVideoCaptureDlg->GetSelecedOpt() )
	{
	case CVideoCaptureDlg::eVideoCaptureOpt_High:
		{
			cfg.VideoCodec		= FOURCC_MJPG;
			cfg.VideoQuality	= 90;
			cfg.VideoFPS		= 30;

			cfg.AudioChannels	= 2;
			cfg.AudioCodec		= WAVETAG_MP2;
			cfg.AudioSampleRate = 44100;
		}
		break;

	case CVideoCaptureDlg::eVideoCaptureOpt_Low:
		{
			cfg.VideoCodec		= FOURCC_MPEG;
			cfg.VideoQuality	= 70;
			cfg.VideoFPS		= 24;

			cfg.AudioChannels	= 1;
			cfg.AudioCodec		= WAVETAG_PCM;
			cfg.AudioSampleRate = 22050;
		}
		break;
	}

	cfg.AdjustAudioMixer	= TRUE;

	// 커서 포함 여부
	cfg.IncludeCursor		= pVideoCaptureDlg->IsIncludeCursor();

	// 잘못된 설정 확인
	m_bandiCaptureLibrary.CheckConfig( &cfg );

	// 설정 적용
	m_bandiCaptureLibrary.SetConfig( &cfg );
}
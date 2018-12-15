// LunaGameDataExtracterDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "LunaGameDataExtracter.h"
#include "LunaGameDataExtracterDlg.h"
#include "./MHFile/MHFile.h"
#include ".\extractoptiondlg.h"
#include "SimpleTextListCtrl.h"
#include ".\lunagamedataextracterdlg.h"

UINT WINAPI GameDataLoadThread(LPVOID lpVoid)
{
	CLunaGameDataExtracterDlg* pDlg = (CLunaGameDataExtracterDlg*)lpVoid;
	if( pDlg )
	{
		pDlg->LoadGameData();
	}

	_endthreadex( 0 );
	return 0;
}

UINT WINAPI ExtractGameDataThread(LPVOID lpVoid)
{
	CLunaGameDataExtracterDlg* pDlg = (CLunaGameDataExtracterDlg*)lpVoid;
	if( pDlg )
	{
		pDlg->ExtractGameData();
	}

	_endthreadex( 0 );
	return 0;
}

DWORD _stdcall AfterRenderFunc()
{
	CLunaGameDataExtracterDlg* pLunaGameDataExtracterDlg = (CLunaGameDataExtracterDlg*)AfxGetApp()->GetMainWnd();
	if( !pLunaGameDataExtracterDlg )
		return 0;

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( pLunaGameDataExtracterDlg->GetSettingScriptFileName() );

	BOOL bIsRenderBoundBox = settingScript.GetGlobalBoolean( "RenderBoundBox" );
	BOOL bIsRenderBoundSphere = settingScript.GetGlobalBoolean( "RenderBoundSphere" );

	if( bIsRenderBoundBox == FALSE &&
		bIsRenderBoundSphere == FALSE )
		return 0;

	I4DyuchiGXExecutive* pExecutive = pLunaGameDataExtracterDlg->GetExecutive();
	GXOBJECT_HANDLE handle = pLunaGameDataExtracterDlg->GetAddedGXObject();

	if( handle == NULL )
		return 0;

	COLLISION_MESH_OBJECT_DESC collisionDesc={0};
	pExecutive->GXOGetCollisionMesh( handle, &collisionDesc );

	if( bIsRenderBoundBox )
		pExecutive->GetGeometry()->RenderBox( collisionDesc.boundingBox.v3Oct, 0xff00ff00 );
	if( bIsRenderBoundSphere )
		pExecutive->GetGeometry()->RenderSphere( &collisionDesc.boundingSphere.v3Point, collisionDesc.boundingSphere.fRs, 0xffffffff );

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


// CLunaGameDataExtracterDlg 대화 상자



CLunaGameDataExtracterDlg::CLunaGameDataExtracterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLunaGameDataExtracterDlg::IDD, pParent),
	SETTING_LUA_SCRIPT_FILENAME(_T("setting.lua"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pExecutive = NULL;
	m_bLoadCompleted = FALSE;
	m_bExtracting = FALSE;

	m_hLodingThread = NULL;
	m_ExtractingThread = NULL;
}

void CLunaGameDataExtracterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_PictureCtrl);
	DDX_Control(pDX, IDC_BUTTON_EXTRACT, m_ExtractBtn);
	DDX_Control(pDX, IDC_STATIC_PROGRESS_STATUS, m_ProgressStatusStaticCtrl);
	DDX_Control(pDX, IDC_PROGRESS_CTRL, m_ProgressCtrl);
	DDX_Control(pDX, IDC_BUTTON_SCRIPT_RELOAD, m_ScriptReLoadBtn);
}

BEGIN_MESSAGE_MAP(CLunaGameDataExtracterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EXTRACT, OnBnClickedButtonExtract)
	ON_BN_CLICKED(IDC_BUTTON_SCRIPT_RELOAD, OnBnClickedButtonScriptReload)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CLunaGameDataExtracterDlg 메시지 처리기

BOOL CLunaGameDataExtracterDlg::OnInitDialog()
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

	char exeutedPath[MAX_PATH]={0};
	::GetCurrentDirectory( MAX_PATH, exeutedPath );
	m_strExecutedPath = exeutedPath;

	InitializeWindowFromScript();
    
	return TRUE;
}

void CLunaGameDataExtracterDlg::LoadGameData()
{
	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	// Luna Renderer Load
	int nProgressPercent = 0;
	m_ProgressCtrl.SetRange( 0, 100 );
	m_ProgressCtrl.SetPos( nProgressPercent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Executive.dll.. ") );

	m_hExecutiveHandle = LoadLibrary( "Executive.dll" );
	
	CREATE_INSTANCE_FUNC pFunc = (CREATE_INSTANCE_FUNC)GetProcAddress( m_hExecutiveHandle, "DllCreateInstance" );
	if( pFunc == NULL ||
		pFunc((void**)&m_pExecutive) != S_OK)
	{
		OutputMessageBox( "%s Excutive.dll Create Failed..", __FUNCTION__ );
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" Executiive.dll Load Failed.. ") );
		return;
	}

	m_ProgressCtrl.SetPos( nProgressPercent+=2 );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Pack File.. ") );

	PACKFILE_NAME_TABLE nameTable[] = {
			{"data/interface/2dImage/image.pak", 0},
			//{"data/interface/2dImage/npcimage.pak", 0},
			//{"data/3dData/map.pak", 0},
			//{"data/3dData/character.pak", 0},
			{"data/3dData/effect.pak", 0},
			{"data/3dData/monster.pak", 0},
			{"data/3dData/housing.pak", 0},
			//{"data/3dData/npc.pak", 0},
		};

	for( int i=0 ; i < sizeof(nameTable) / sizeof(*nameTable) ; ++i )
	{
		char path[ MAX_PATH ]={0};
		_stprintf( path, _T("%s%s"), (LPCTSTR)m_strLunaDataPath, nameTable[i].szFileName );
		_stprintf( nameTable[i].szFileName, _T("%s"), path );
	}

	m_pExecutive->InitializeFileStorageWithoutRegistry(
			"FileStorage.dll",
			35000,
			10000,
			MAX_PATH,
			FILE_ACCESS_METHOD_FILE_OR_PACK,
			nameTable,
			sizeof(nameTable) / sizeof(*nameTable),
			TRUE);

	m_pExecutive->InitializeWithoutRegistry(
		"Geometry.dll",
		"Renderer.dll",
		m_PictureCtrl.GetSafeHwnd(),
		0,
		160000,
		4096,
		512,
		32,
		32,
		0);

	// 폰트 생성
	LOGFONT	font;
	font.lfHeight = 12;
	font.lfWidth = 0;
	font.lfEscapement = 0;
	font.lfOrientation = 0;
	font.lfWeight = FW_NORMAL;
	font.lfItalic = 0;
	font.lfUnderline = 0;
	font.lfStrikeOut = 0;
	font.lfCharSet = DEFAULT_CHARSET;//HANGUL_CHARSET;
	font.lfOutPrecision = 0;
	font.lfQuality = PROOF_QUALITY;//ANTIALIASED_QUALITY;//NONANTIALIASED_QUALITY;//PROOF_QUALITY;
	font.lfPitchAndFamily = 0;
	lstrcpy(font.lfFaceName,"굴림");
	m_pFontObject = m_pExecutive->GetRenderer()->CreateFontObject( &font, D3DX_FONT );//SS3D_FONT);//D3DX_FONT);

	m_Font.CreateFontIndirect( &font );

	m_ProgressCtrl.SetPos( nProgressPercent+=13 );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Initailize Render Environment.. ") );

	m_pExecutive->SetViewport(0);

	// 깜깜하니 캐릭터를 잘 알아볼수가 없다.빛을 주자.
	DIRECTIONAL_LIGHT_DESC	dirLight;
	dirLight.v3Dir.x = (float)settingScript.GetGlobalTableFieldNumber( "LightDirectional", "x" );
	dirLight.v3Dir.y = (float)settingScript.GetGlobalTableFieldNumber( "LightDirectional", "y" );
	dirLight.v3Dir.z = (float)settingScript.GetGlobalTableFieldNumber( "LightDirectional", "z" );
	dirLight.dwSpecular = 0xffffffff;
	dirLight.dwDiffuse = 0xfffffff;

	BYTE diffuseR = (BYTE)settingScript.GetGlobalTableFieldNumber( "LightColor", "r" );
	BYTE diffuseG = (BYTE)settingScript.GetGlobalTableFieldNumber( "LightColor", "g" );
	BYTE diffuseB = (BYTE)settingScript.GetGlobalTableFieldNumber( "LightColor", "b" );
	dirLight.dwAmbient = RGBA_MAKE( diffuseR, diffuseG, diffuseB, 255 );
	
	m_pExecutive->GetGeometry()->SetDirectionalLight( &dirLight, 0 );
	m_pExecutive->GetGeometry()->EnableDirectionalLight( 0 );	
	m_pExecutive->GetGeometry()->SetShadowFlag( ENABLE_PROJECTION_SHADOW |	ENABLE_PROJECTION_TEXMAP );

	m_ProgressCtrl.SetPos( nProgressPercent+=5 );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Luna Game Data.. ") );

	//::SetCurrentDirectory( m_strExecutedPath );

	// 게임데이타 로드 전까지 20%..
	if( LoadAllData( nProgressPercent ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" Load Failed..... after 5 seconds terminated... ") );
		m_ProgressCtrl.SetPos( 0 );

		Sleep( 5000 );
		SendMessage( WM_CLOSE );
		return;
	}

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Load Complete. ") );
	m_ProgressCtrl.SetPos( 100 );

	////////////////////////////////////////////////////////////////////////////////////////
	// Capture Test
	////////////////////////////////////////////////////////////////////////////////////////

	const DWORD dwBackGroundColor = 0;
	m_pExecutive->Run( dwBackGroundColor, 0, 0, 0 );

	char testloadchxfilename[MAX_PATH]={0};
	settingScript.GetGlobalTableFieldString( "TestLoadChx", "FileName", testloadchxfilename, sizeof(testloadchxfilename) );
	float fScale = (float)settingScript.GetGlobalTableFieldNumber( "TestLoadChx", "Scale" );
	InsertGXObject( testloadchxfilename, fScale );

	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	KillTimer( 1 );
	SetTimer( 1, 10, 0 );

	m_bLoadCompleted = TRUE;
	m_hLodingThread = NULL;
}

void CLunaGameDataExtracterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLunaGameDataExtracterDlg::OnPaint() 
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

	if (IsIconic())
	{
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
HCURSOR CLunaGameDataExtracterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLunaGameDataExtracterDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch( nIDEvent )
	{
	case 1:
		{
			if( m_pExecutive == NULL )
			{
				break;
			}

			CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

			const BOOL bMovable = settingScript.GetGlobalBoolean( "KeyboardMovable" );
			if( bMovable )
			{
				POINT cursorPoint = {0};
				GetCursorPos(
					&cursorPoint);

				CRect rect;
				GetWindowRect(
					rect);

				if(FALSE == rect.PtInRect(cursorPoint))
				{
					break;
				}

				const float fMoveStep = 200.f;
				CAMERA_DESC Desc;
				m_pExecutive->GetGeometry()->GetCameraDesc( &Desc, 0);
				VECTOR3 vecLook = Desc.v3EyeDir;
				VECTOR3 vecUp = Desc.v3Up;
				VECTOR3 vecRight;
				Normalize( &vecLook, &vecLook );
				Normalize( &vecUp, &vecUp );
				CrossProduct( &vecRight, &vecUp, &vecLook );
				Normalize( &vecRight, &vecRight );

				vecLook = vecLook * fMoveStep;
				vecRight = vecRight * fMoveStep;

				// 키보드 이동 처리
				// w, W
				if( GetAsyncKeyState( 0x57 ) )
				{
					vecLook.y = 0;
					m_pExecutive->GetGeometry()->MoveCamera( &vecLook, 0 );
				}
				// s, S
				if( GetAsyncKeyState( 0x53 ) )
				{
					vecLook.y = 0;
					vecLook = vecLook * -1.f;
					m_pExecutive->GetGeometry()->MoveCamera( &vecLook, 0 );
				}
				// d, D
				if( GetAsyncKeyState( 0x44 ) )
				{
					m_pExecutive->GetGeometry()->MoveCamera( &vecRight, 0 );
				}
				// a, A
				if( GetAsyncKeyState( 0x41 ) )
				{
					vecRight = vecRight * -1.f;
					m_pExecutive->GetGeometry()->MoveCamera( &vecRight, 0 );
				}

				// 상하 이동
				vecUp.x = vecUp.z = 0;
				vecUp.y = 1;
				vecUp = vecUp * fMoveStep;
				//(51) Q key
				if( GetAsyncKeyState( 0x51 ) )
				{
					vecUp = vecUp * -1.f;
					m_pExecutive->GetGeometry()->MoveCamera( &vecUp, 0 );
				}
				//(45) E key
				else if( GetAsyncKeyState( 0x45 ) )
				{
					m_pExecutive->GetGeometry()->MoveCamera( &vecUp, 0 );
				}
			}

			const BOOL bVisibleEnginePerformance = settingScript.GetGlobalBoolean( "VisibleEnginePerformance" );
			if( bVisibleEnginePerformance )
				m_pExecutive->GetRenderer()->BeginPerformanceAnalyze();

			BYTE backgroundR = (BYTE)settingScript.GetGlobalTableFieldNumber( "BackgroundColor", "r" );
			BYTE backgroundG = (BYTE)settingScript.GetGlobalTableFieldNumber( "BackgroundColor", "g" );
			BYTE backgroundB = (BYTE)settingScript.GetGlobalTableFieldNumber( "BackgroundColor", "b" );
			const DWORD dwBackGroundColor = RGBA_MAKE( backgroundR, backgroundG, backgroundB, 255 );
			m_pExecutive->Run( dwBackGroundColor, 0, AfterRenderFunc, 0 );

			if( bVisibleEnginePerformance )
				m_pExecutive->GetRenderer()->EndPerformanceAnalyze();

			m_pExecutive->GetGeometry()->Present(0);

			break;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CLunaGameDataExtracterDlg::PostNcDestroy()
{
	ReleaseEngine();

	CDialog::PostNcDestroy();
}

void CLunaGameDataExtracterDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );
	if( settingScript.GetGlobalBoolean( "ForceTerminate" ) == TRUE )
	{
		if( !m_bLoadCompleted )
		{
			BOOL bIsTerminated = FALSE;
			while( !bIsTerminated )
			{
				DWORD dwExitCode;
				GetExitCodeThread( m_hLodingThread, &dwExitCode );
				if( dwExitCode == STILL_ACTIVE )
				{
					TerminateThread( m_hLodingThread, 0 );
					Sleep( 100 );
				}
				else
				{
					CloseHandle( m_hLodingThread );
					bIsTerminated = TRUE;
				}
			}
		}

		if( m_bExtracting )
		{
			BOOL bIsTerminated = FALSE;
			while( !bIsTerminated )
			{
				DWORD dwExitCode;
				GetExitCodeThread( m_ExtractingThread, &dwExitCode );
				if( dwExitCode == STILL_ACTIVE )
				{
					TerminateThread( m_ExtractingThread, 0 );
					Sleep( 100 );
				}
				else
				{
					CloseHandle( m_ExtractingThread );
					bIsTerminated = TRUE;
				}
			}
		}
	}
	else
	{
		// 강제종료하지 않는 경우
		if( !m_bLoadCompleted )
		{
			AfxMessageBox( "During Loading Process Can't Stop !" );
			return;
		}
		else if( m_bExtracting )
		{
			AfxMessageBox( "During Extracting Process Can't Stop !" );
			return;
		}
	}

	CDialog::OnCancel();
}


void CLunaGameDataExtracterDlg::InitializeWindowFromScript()
{
	::SetCurrentDirectory( m_strExecutedPath );

	SIMPLELUAMGR->RemoveSimpleLua( SETTING_LUA_SCRIPT_FILENAME );
	if( SIMPLELUAMGR->CreateSimpleLua( SETTING_LUA_SCRIPT_FILENAME ) == FALSE )
	{
		OutputMessageBox( "%s File Load Failed..", SETTING_LUA_SCRIPT_FILENAME );
		return;
	}

	::SetCurrentDirectory( m_strLunaDataPath );

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );


	CRect windowRect(0,0,800,600);
	CRect pictureRect(0,0,800,600);
	CRect scriptReloadBtnRect(0,0,800,600);
	CRect extractBtnRect(0,0,800,600);
	CRect progressStatusStaticRect(0,0,800,600);
	CRect progressCtrlRect(0,0,800,600);

	{
		windowRect.right					= (LONG)settingScript.GetGlobalTableFieldNumber( "windowsize", "width" );
		windowRect.bottom					= (LONG)settingScript.GetGlobalTableFieldNumber( "windowsize", "height" );

		CRect currentWindowRect;
		GetWindowRect( &currentWindowRect );
		if( currentWindowRect.left > 0 )
		{
			windowRect.left += currentWindowRect.left;
			windowRect.top += currentWindowRect.top;
			windowRect.right += currentWindowRect.left;
			windowRect.bottom += currentWindowRect.top;
		}
	}

	{
		const double leftPercent		= settingScript.GetGlobalTableFieldNumber( "pictureCtrlPos", "leftPercent" ) / 100;
		const double topPercent			= settingScript.GetGlobalTableFieldNumber( "pictureCtrlPos", "topPercent" ) / 100;
		const double rightPercent		= settingScript.GetGlobalTableFieldNumber( "pictureCtrlPos", "rightPercent" ) / 100;
		const double bottomPercent		= settingScript.GetGlobalTableFieldNumber( "pictureCtrlPos", "bottomPercent" ) / 100;

		pictureRect.left				= (LONG)((windowRect.right - windowRect.left) * leftPercent);
		pictureRect.right				= (LONG)((windowRect.right - windowRect.left) * rightPercent);
		pictureRect.top					= (LONG)((windowRect.bottom - windowRect.top) * topPercent);
		pictureRect.bottom				= (LONG)((windowRect.bottom - windowRect.top) * bottomPercent);
	}

	{
		const double leftPercent		= settingScript.GetGlobalTableFieldNumber( "ProgressStatusStaticPos", "leftPercent" ) / 100;
		const double topPercent			= settingScript.GetGlobalTableFieldNumber( "ProgressStatusStaticPos", "topPercent" ) / 100;
		const double rightPercent		= settingScript.GetGlobalTableFieldNumber( "ProgressStatusStaticPos", "rightPercent" ) / 100;
		const double bottomPercent		= settingScript.GetGlobalTableFieldNumber( "ProgressStatusStaticPos", "bottomPercent" ) / 100;

		progressStatusStaticRect.left	= (LONG)((windowRect.right - windowRect.left) * leftPercent);
		progressStatusStaticRect.right	= (LONG)((windowRect.right - windowRect.left) * rightPercent);
		progressStatusStaticRect.top	= (LONG)((windowRect.bottom - windowRect.top) * topPercent);
		progressStatusStaticRect.bottom	= (LONG)((windowRect.bottom - windowRect.top) * bottomPercent);
	}

	{
		const double leftPercent		= settingScript.GetGlobalTableFieldNumber( "ProgressCtrlPos", "leftPercent" ) / 100;
		const double topPercent			= settingScript.GetGlobalTableFieldNumber( "ProgressCtrlPos", "topPercent" ) / 100;
		const double rightPercent		= settingScript.GetGlobalTableFieldNumber( "ProgressCtrlPos", "rightPercent" ) / 100;
		const double bottomPercent		= settingScript.GetGlobalTableFieldNumber( "ProgressCtrlPos", "bottomPercent" ) / 100;

		progressCtrlRect.left			= (LONG)((windowRect.right - windowRect.left) * leftPercent);
		progressCtrlRect.right			= (LONG)((windowRect.right - windowRect.left) * rightPercent);
		progressCtrlRect.top			= (LONG)((windowRect.bottom - windowRect.top) * topPercent);
		progressCtrlRect.bottom			= (LONG)((windowRect.bottom - windowRect.top) * bottomPercent);
	}

	{
		const double leftPercent		= settingScript.GetGlobalTableFieldNumber( "ScriptReLoadBtnPos", "leftPercent" ) / 100;
		const double topPercent			= settingScript.GetGlobalTableFieldNumber( "ScriptReLoadBtnPos", "topPercent" ) / 100;
		const double rightPercent		= settingScript.GetGlobalTableFieldNumber( "ScriptReLoadBtnPos", "rightPercent" ) / 100;
		const double bottomPercent		= settingScript.GetGlobalTableFieldNumber( "ScriptReLoadBtnPos", "bottomPercent" ) / 100;

		scriptReloadBtnRect.left		= (LONG)((windowRect.right - windowRect.left) * leftPercent);
		scriptReloadBtnRect.right		= (LONG)((windowRect.right - windowRect.left) * rightPercent);
		scriptReloadBtnRect.top			= (LONG)((windowRect.bottom - windowRect.top) * topPercent);
		scriptReloadBtnRect.bottom		= (LONG)((windowRect.bottom - windowRect.top) * bottomPercent);
	}

	{
		const double leftPercent		= settingScript.GetGlobalTableFieldNumber( "ExtractBtnPos", "leftPercent" ) / 100;
		const double topPercent			= settingScript.GetGlobalTableFieldNumber( "ExtractBtnPos", "topPercent" ) / 100;
		const double rightPercent		= settingScript.GetGlobalTableFieldNumber( "ExtractBtnPos", "rightPercent" ) / 100;
		const double bottomPercent		= settingScript.GetGlobalTableFieldNumber( "ExtractBtnPos", "bottomPercent" ) / 100;

		extractBtnRect.left				= (LONG)((windowRect.right - windowRect.left) * leftPercent);
		extractBtnRect.right			= (LONG)((windowRect.right - windowRect.left) * rightPercent);
		extractBtnRect.top				= (LONG)((windowRect.bottom - windowRect.top) * topPercent);
		extractBtnRect.bottom			= (LONG)((windowRect.bottom - windowRect.top) * bottomPercent);
	}

	MoveWindow( &windowRect );
	m_PictureCtrl.MoveWindow( &pictureRect );
	m_ProgressStatusStaticCtrl.MoveWindow( &progressStatusStaticRect );
	m_ProgressCtrl.MoveWindow( &progressCtrlRect );
	m_ScriptReLoadBtn.MoveWindow( &scriptReloadBtnRect );
	m_ExtractBtn.MoveWindow( &extractBtnRect );

	char lunaDataFolderPath[ MAX_PATH ]={0};
	settingScript.GetGlobalString( "LunaDataFolderPath", lunaDataFolderPath, MAX_PATH );
	if( _tcsicmp( m_strLunaDataPath ,lunaDataFolderPath ) != 0 )
	{
		m_strLunaDataPath = lunaDataFolderPath;
		if( strlen( lunaDataFolderPath ) > 0 )
			::SetCurrentDirectory( m_strLunaDataPath );
		InitializeEngine();
	}

	

	if( m_pExecutive )
	{
		m_pExecutive->GetGeometry()->UpdateWindowSize();
	}
}

void CLunaGameDataExtracterDlg::InitializeEngine()
{
	KillTimer( 1 );

	ReleaseEngine();

	// Create Game Data Load Thread
	UINT uiThreadID;
	m_hLodingThread = (HANDLE)_beginthreadex( NULL, NULL, GameDataLoadThread, this, NULL, &uiThreadID );
}

void CLunaGameDataExtracterDlg::ReleaseEngine()
{
	m_bLoadCompleted = FALSE;

	if( m_pExecutive != NULL )
	{
		m_pExecutive->DeleteAllGXObjects();
		m_pExecutive->Release();
		m_pExecutive = NULL;
	}

	FreeLibrary( m_hExecutiveHandle );
}

void CLunaGameDataExtracterDlg::InsertGXObject( LPCTSTR objectname, float fScale )
{
	if( !m_pExecutive )
		return;

	m_pExecutive->DeleteAllGXObjects();
	m_AddedGXObjectHandel = NULL;

	GXOBJECT_HANDLE handle  = m_pExecutive->CreateGXObject( (LPSTR)objectname, NULL, 0, 0);
	if( handle == NULL )
		return;

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	m_AddedGXObjectHandel = handle;

	m_pExecutive->GXOEnableHFieldApply(handle);
	//m_pExecutive->GXOSetPosition( handle, TargetPos, FALSE );
	VECTOR3 vecScale = { 1, 1, 1 };
	vecScale = vecScale * fScale;
	m_pExecutive->GXOSetScale( handle, &vecScale );

	float fRotation = (float)settingScript.GetGlobalNumber( "RotationDegree" );
	VECTOR3 axis = { 0, 1, 0 };
	m_pExecutive->GXOSetDirection( handle, &axis, DEGTORAD( fRotation ) );

	COLLISION_MESH_OBJECT_DESC collisionDesc={0};
	m_pExecutive->GXOGetCollisionMesh( handle, &collisionDesc );

	VECTOR3 collisionBoxSize={0};
	collisionBoxSize.x = abs( collisionDesc.boundingBox.v3Oct[3].x - collisionDesc.boundingBox.v3Oct[1].x );
	collisionBoxSize.y = abs( collisionDesc.boundingBox.v3Oct[0].y - collisionDesc.boundingBox.v3Oct[1].y );
	collisionBoxSize.z = abs( collisionDesc.boundingBox.v3Oct[4].z - collisionDesc.boundingBox.v3Oct[0].z );


	BOOL bFitScreen = settingScript.GetGlobalBoolean( "FitScreen" );
	float fDeg = (float)settingScript.GetGlobalNumber( "CameraViewDegee" );
	float fDownPercent = (float)settingScript.GetGlobalNumber( "DownPercent" ) / 100.f;
	float z = abs( collisionBoxSize.y/2 / tan( DEGTORAD( fDeg ) ) ) * -1;

	VECTOR3	v3From={0,0,-1000};

	if( bFitScreen )
	{
		v3From.y = collisionDesc.boundingBox.v3Oct[1].y + ( collisionBoxSize.y / 2 );
		v3From.z = z;
	}
	else
	{
		v3From.x = (float)settingScript.GetGlobalTableFieldNumber( "DefaultCameraPosition", "x" );
		v3From.y = (float)settingScript.GetGlobalTableFieldNumber( "DefaultCameraPosition", "y" );
		v3From.z = (float)settingScript.GetGlobalTableFieldNumber( "DefaultCameraPosition", "z" );

		if( z < v3From.z )
		{
			v3From.y = collisionBoxSize.y * fDownPercent;
			v3From.z = z;
		}
	}
	
	m_pExecutive->GetGeometry()->ResetCamera(&v3From,DEFAULT_NEAR,DEFAULT_FAR,DEFAULT_FOV,0);

	return;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Game Data
/////////////////////////////////////////////////////////////////////////////

BOOL CLunaGameDataExtracterDlg::LoadAllData( int nProgressPercent )
{
	// 20 % 부터 시작..
	const int stepPercent = 5;

	// System Message Load
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Game System Message.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadSystemMsg() )
		return FALSE;

	// Interface Message Load
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Interface System Message.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadInterfaceMsg() )
		return FALSE;

	// Image
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading 2D Image.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !Load2DImageInfo() )
		return FALSE;

	// Monster
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading MonsterList Info.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadMonsterList() )
		return FALSE;

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Monster Regen Info.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadMonsterRegenInfo() )
		return FALSE;

	// Item
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading ItemList Info.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadItemInfo() )
		return FALSE;

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading SetItem Info.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadSetItmeInfo() )
		return FALSE;

	// Skill
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading ActiveSkill Info.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadActiveSkillInfo() )
		return FALSE;

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading BuffSkill Info.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadBuffSkillInfo() )
		return FALSE;

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Skill_Get_List Info.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadSkillGetListInfo() )
		return FALSE;

	// Tooltip Message Load (항상 스킬/아이템 로드 다음에!)
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Tooltip Message.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadToolTipMsg() )
		return FALSE;

	// Quest data
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Npc Message.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadNpcMsg() )
		return FALSE;

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Npc Hyper Text.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadNpcHyperText() )
		return FALSE;

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Npc Script.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadNpcScript() )
		return FALSE;

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Npc Info.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadNpcInfo() )
		return FALSE;

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Quest Title Info.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadQuestTitleText() )
		return FALSE;

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Loading Quest Item Info.. ") );
	m_ProgressCtrl.SetPos( nProgressPercent+=stepPercent );
	if( !LoadQuestItemInfo() )
		return FALSE;

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadSystemMsg()
{
	// System Message Load
	CMHFile mhFile;
	if( mhFile.Init( "./Data/Interface/Windows/SystemMsg.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" SystemMsg.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		const char* delimit = "\t\n";
		mhFile.GetLine( buf, sizeof(buf) );

		char* token = strtok( buf, delimit );
		if( token == NULL ) continue;
		const DWORD index = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;

		m_SysteMsgList[ index ] = GetChangedLunaGameString( token );
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadInterfaceMsg()
{
	// Interface Message Load
	CMHFile mhFile;
	if( mhFile.Init( "./Data/Interface/Windows/InterfaceMsg.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" InterfaceMsg.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		const char* delimit = "\t\n";
		mhFile.GetLine( buf, sizeof(buf) );

		char* token = strtok( buf, delimit );
		if( token == NULL ) continue;
		const DWORD index = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;

		m_InterfaceMsgList[ index ] = token;
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadToolTipMsg()
{
	CMHFile mhFile;
	if( mhFile.Init( "./Data/Interface/Windows/ToolTipMsg.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" ToolTipMsg.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	DWORD dwIndex = 0;
	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		_tcsncpy( buf, mhFile.GetString(), sizeof(buf) );

		if( _tcsicmp( buf, "#Msg" ) == 0 )
		{
			dwIndex = mhFile.GetDword();	//msg index
			_tcsncpy( buf, mhFile.GetString(), sizeof(buf) );
			if( strcmp( buf, "{" ) == 0 )
			{
				BOOL bIsFirst = TRUE;
				// ToolTipMsg 를 모두 읽은 후 예약어를 변환한다.
				std::string strToolTipMsg;
				mhFile.GetLine( buf, sizeof(buf) );
				strToolTipMsg.append( buf );

				mhFile.GetLine( buf, sizeof(buf) );
				while( *buf != '}' )
				{
					if( !bIsFirst )
                        strToolTipMsg += "\n";
					strToolTipMsg.append( buf );
					mhFile.GetLine( buf, sizeof(buf) );

					bIsFirst = FALSE;
				}

				// 100312 ShinJS --- 툴팁 메세지 예약어 변환
 				m_TooltipConverter.ConvertToolTipMsgKeyWordFormText( dwIndex, strToolTipMsg );

				m_TooltipMsgList.insert( std::map< DWORD, std::string >::value_type( dwIndex, strToolTipMsg ) );
			}
		}
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::Load2DImageInfo()
{
	char fileName[ MAX_PATH ] = "data\\interface\\windows\\image_path.bin";

	FILE* file = fopen( "system\\launcher.sav", "r" );
	if( file )
	{
		char		buffer[ MAX_PATH ]	= { 0 };
		const char* separator			= " =\n";

		while( fgets( buffer, sizeof( buffer ), file ) )
		{
			const char* token = strtok( buffer, separator );

			if( 0 == token ||	// empty line
				';' == token[ 0 ] )	// comment
			{
				continue;
			}
			else if( ! strcmpi( "imagePath",  token ) )
			{
				token = strtok( 0, separator );
				FILE* imagepathfile = fopen( token, "r" );

				if( token &&
					imagepathfile != NULL )
				{
					strcpy( fileName, token );
					fclose( imagepathfile );
				}
			}
		}
		fclose( file );
	}

	CMHFile mhFile;
	if( mhFile.Init( fileName, "rb" ) == FALSE )
	{
		CString strMsg;
		strMsg.Format( " %s Open Failed ! ", fileName );
		m_ProgressStatusStaticCtrl.SetWindowText( strMsg );
		Sleep( 1000 );

		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char buf[MAX_PATH]={0};
		const char* delimit = "\t\n ";
		mhFile.GetLine( buf, sizeof(buf) );

		stImagePathInfo imagePathInfo;

		char* token = strtok( buf, delimit );
		if( token == NULL ) continue;
		imagePathInfo.index = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imagePathInfo.path.assign( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imagePathInfo.width = (LONG)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imagePathInfo.height = (LONG)atoi( token );

		m_ImagePathInfoList.insert( std::map< DWORD, stImagePathInfo >::value_type( imagePathInfo.index, imagePathInfo ) );
	}
	mhFile.Clear();

	// Item IconImage Info Load
	if( mhFile.Init( "./Data/Interface/Windows/image_item_path.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" image_item_path.bin Open Failed ! " ) );
		Sleep( 1000 );

		return TRUE;
	}

	while( !mhFile.IsEOF() )
	{
		char buf[MAX_PATH]={0};
		const char* delimit = "\t\n ";
		mhFile.GetLine( buf, sizeof(buf) );

		stImageInfo imageInfo;

		char* token = strtok( buf, delimit );
		if( token == NULL ) continue;
		imageInfo.index = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imageInfo.imagePathInfoIdx = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imageInfo.rect.left = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imageInfo.rect.top = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imageInfo.rect.right = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imageInfo.rect.bottom = (DWORD)atoi( token );

		m_ItemIconImageInfoList.insert( IMAGEINFOMAP::value_type( imageInfo.index, imageInfo ) );
	}
	mhFile.Clear();

	// Skill IconImage Info Load
	if( mhFile.Init( "./Data/Interface/Windows/image_skill_path.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" image_skill_path.bin Open Failed ! " ) );
		Sleep( 1000 );

		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char buf[MAX_PATH]={0};
		const char* delimit = "\t\n ";
		mhFile.GetLine( buf, sizeof(buf) );

		stImageInfo imageInfo;

		char* token = strtok( buf, delimit );
		if( token == NULL ) continue;
		imageInfo.index = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imageInfo.imagePathInfoIdx = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imageInfo.rect.left = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imageInfo.rect.top = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imageInfo.rect.right = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		imageInfo.rect.bottom = (DWORD)atoi( token );

		m_SkillIconImageInfoList.insert( IMAGEINFOMAP::value_type( imageInfo.index, imageInfo ) );
	}
	mhFile.Clear();

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadMonsterList()
{
	CMHFile file;
	if( file.Init( "./System/Resource/MonsterList.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" MonsterList.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !file.IsEOF() )
	{
		BASE_MONSTER_LIST monsterinfo={0};

		monsterinfo.MonsterKind				= file.GetWord();		// 번호
		_tcsncpy( monsterinfo.Name, GetChangedLunaGameString( file.GetString() ), sizeof(monsterinfo.Name)/sizeof(*monsterinfo.Name) );	// 이름
		file.GetString();// 영문명
		_tcsncpy( monsterinfo.ChxName, file.GetString(), sizeof(monsterinfo.ChxName)/sizeof(*monsterinfo.ChxName) );	// CHX 파일

		//monsterinfo.Damage1DramaNum			= FindEffectNum(file.GetString());// 맞음 1 연출 번호	
		//monsterinfo.Die1DramaNum				= FindEffectNum(file.GetString());// 죽음 1 연출 번호	
		//monsterinfo.Die2DramaNum				= FindEffectNum(file.GetString());// 죽음 2 연출 번호	
		file.GetString();
		file.GetString();
		file.GetString();

		monsterinfo.SpecialType				= file.GetWord();	// 몬스터 종류
		monsterinfo.ObjectKind				= file.GetWord();
		monsterinfo.MonsterType				= file.GetWord();	// 유형분류
		monsterinfo.MonsterAttribute			= file.GetWord();	// 속성
		monsterinfo.MonsterRace				= file.GetWord();	// 대표 종족 코드

		monsterinfo.Level					= (LEVELTYPE)file.GetLevel();		// 몬스터레벨	
		// 080604 LYW --- GameResourceManager : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
		//monsterinfo.ExpPoint					= file.GetExpPoint();	// 경험치
		monsterinfo.ExpPoint					= file.GetDword() ;	// 경험치

		monsterinfo.Life						= file.GetDword();		// 체력
		monsterinfo.BaseReinforce			= file.GetByte();
		monsterinfo.Scale					= file.GetFloat();		// 크기	
		monsterinfo.Weight					= file.GetFloat();		// 무게
		monsterinfo.Gravity					= file.GetDword();		// 비중
		monsterinfo.MonsterRadius			= file.GetWord();		// 반지름 	
		monsterinfo.Tall						= file.GetWord();		// 키(이름출력 위치)

		monsterinfo.Accuracy					= file.GetWord();
		monsterinfo.Avoid					= file.GetWord();

		monsterinfo.AttackPhysicalMax		= file.GetWord();// 공격 최대물리 공격력	
		monsterinfo.AttackPhysicalMin		= ( WORD )( monsterinfo.AttackPhysicalMax * 0.95f );// 공격 최소물리 공격력	
		monsterinfo.PhysicAttackReinforce	= file.GetByte();
		monsterinfo.AttackMagicalMax			= file.GetWord();// 공격 최대물리 공격력	
		monsterinfo.AttackMagicalMin			= ( WORD )( monsterinfo.AttackMagicalMax * 0.95f );// 공격 최소물리 공격력	
		monsterinfo.MagicAttackReinforce		= file.GetByte();
		monsterinfo.CriticalPercent			= file.GetWord();// 크리티컬 퍼센트
		file.GetFloat();
		monsterinfo.PhysicalDefense			= file.GetWord();// 물리 방어력	
		monsterinfo.PhysicDefenseReinforce	= file.GetByte();
		monsterinfo.MagicalDefense			= file.GetWord();// 물리 방어력	
		monsterinfo.MagicDefenseReinforce	= file.GetByte();
		monsterinfo.WalkMove					= file.GetWord();// 몬스터걷기이동력	
		monsterinfo.RunMove					= file.GetWord();// 몬스터뛰기이동력	
		monsterinfo.RunawayMove				= monsterinfo.RunMove;// 몬스터도망이동력


		monsterinfo.StandTime				= file.GetDword();
		monsterinfo.StandRate				= file.GetByte();
		monsterinfo.WalkRate					= file.GetByte();
		monsterinfo.RunRate					= 0;
		monsterinfo.DomainRange				= file.GetDword();
		// 100104 LUJ, 몬스터 이동 중에 이 값이 최소 1이 되도록 항상 검사했음. 스크립트로 이동시킴
		monsterinfo.DomainRange = max(1, monsterinfo.DomainRange);
		monsterinfo.PursuitForgiveTime		= file.GetDword();
		monsterinfo.PursuitForgiveDistance	= file.GetDword();

		monsterinfo.bForeAttack				= file.GetBool();					// 선공격
		monsterinfo.bDetectSameRace			= file.GetBool();
		monsterinfo.mHelpType				= BASE_MONSTER_LIST::HelpType(file.GetInt());
		monsterinfo.bHearing					= file.GetBool();
		monsterinfo.HearingDistance			= file.GetDword();

		monsterinfo.SearchPeriodicTime		= file.GetDword();				// 탐색 주기
		monsterinfo.TargetSelect				= file.GetWord();					// 타겟 선택 : FIND_CLOSE, FIND_FIRST
		monsterinfo.SearchRange				= file.GetWord();
		monsterinfo.SearchAngle				= file.GetByte();
		monsterinfo.TargetChange				= file.GetWord();
		monsterinfo.AttackNum				= file.GetWord();
		
		for( WORD  cnt = 0; cnt < 5; cnt++ )
		{
			monsterinfo.AttackIndex[cnt]	= file.GetDword();
			//monsterinfo.SkillInfo[cnt] = (cActiveSkillInfo*)SKILLMGR->GetSkillInfo(monsterinfo.AttackIndex[cnt]);
		}
		for( cnt = 0; cnt < 5; cnt++ )
		{
			monsterinfo.AttackRate[cnt]		= file.GetWord();
		}

		// 070423 LYW --- GameResourceManager : Applay drop item.
		monsterinfo.dropMoney		= file.GetDword();
		monsterinfo.dwMoneyDropRate	= file.GetDword();

		for( BYTE count = 0 ; count < eDROPITEMKIND_MAX-1 ; ++count )
		{
			file.GetString();
			monsterinfo.dropItem[count].wItemIdx = file.GetDword();
			monsterinfo.dropItem[count].byCount  = file.GetByte() ;
			monsterinfo.dropItem[count].dwDropPercent = file.GetDword() ;
		}

		m_MonsterInfoList.insert( std::map< DWORD, BASE_MONSTER_LIST >::value_type( monsterinfo.MonsterKind, monsterinfo ) );
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadMonsterRegenInfo()
{
	// 기본 리젠파일 파싱
	class {
	public:
		void operator() ( std::map< DWORD, MAPINDEXLIST >& monsterRegenInfoList, LPCTSTR path, DWORD mapnum )
		{
			CMHFile mhFile;
			if( mhFile.Init( (LPTSTR)path, "rb" ) == TRUE )
			{
				while( !mhFile.IsEOF() )
				{
					char buf[MAX_PATH]={0};
					const char* delimit = "\t\n ";
					mhFile.GetLine( buf, sizeof(buf) );

					char* token = strtok( buf, delimit );
					while( token != NULL )
					{
						if( _tcsicmp( token, "#ADD" ) == 0 ||
							_tcsicmp( token, "#UNIQUEADD" ) == 0 )
						{
							token = strtok( 0, delimit );
							if( token == NULL )		break;

							token = strtok( 0, delimit );
							if( token == NULL )		break;

							token = strtok( 0, delimit );
							if( token == NULL )		break;
							const DWORD monsterKind = (DWORD)atoi( token );

							MAPINDEXLIST& mapIndexList = monsterRegenInfoList[ monsterKind ];
							mapIndexList.insert( mapnum );
						}

						token = strtok( 0, delimit );
					}
				}
			}
			mhFile.Release();
		}
	}ParseMonsterRegenInfo;


	for( int mapnum = MapNameNone ; mapnum < MAX_MAP_NUM ; ++mapnum )
	{
		TCHAR filename[MAX_PATH]={0};
		CMHFile mhFile;

		// 기본 리젠파일
		_stprintf( filename, _T("System/Resource/Monster_%02d.bin"), mapnum );
		ParseMonsterRegenInfo( m_MonsterRegenInfoList, filename, mapnum );


		// Date 매칭 구간 갯수만큼
		// 매칭 제한 구간 임의 설정(20개)
		const int MaxDateMatchingSection = 20;
		for( int section = 0 ; section < MaxDateMatchingSection ; ++section )
		{
			_stprintf( filename, _T("System/Resource/ChallengeZoneMonster%02d%02d.bin"), mapnum, section );
			if( mhFile.Init( filename, "rb" ) == TRUE )
			{
				while( !mhFile.IsEOF() )
				{
					char buf[MAX_PATH]={0};
					const char* delimit = "\t\n ";
					mhFile.GetLine( buf, sizeof(buf) );

					if( strstr( buf, "//" ) != NULL )
						continue;

					char* token = strtok( buf, delimit );
					if( token == NULL ) continue;
					const DWORD monsterKind = (DWORD)atoi( token );

					MAPINDEXLIST& mapIndexList = m_MonsterRegenInfoList[ monsterKind ];
					mapIndexList.insert( mapnum );

				}
			}
			mhFile.Release();
		}

		// 인던 
		_stprintf( filename, _T("System/Resource/DungeonInfo_%02d.bin"), mapnum );
		if( mhFile.Init( filename, "rb" ) == TRUE )
		{
			while( !mhFile.IsEOF() )
			{
				char buf[MAX_PATH]={0};
				const char* delimit = "\t\n ";
				mhFile.GetLine( buf, sizeof(buf) );

				char* token = strtok( buf, delimit );
				while( token != NULL )
				{
					// 인던 구버전 리젠
					if( _tcsicmp( token, "#MONSTER" ) == 0 )
					{
						token = strtok( 0, delimit );
						if( token == NULL )		break;

						token = strtok( 0, delimit );
						if( token == NULL )		break;
						const DWORD monsterKind = (DWORD)atoi( token );

						MAPINDEXLIST& mapIndexList = m_MonsterRegenInfoList[ monsterKind ];
						mapIndexList.insert( mapnum );
					}
					// 인던 신버전 리젠
					else if( _tcsicmp( token, "#REGEN_FILE_EASY" ) == 0 ||
						_tcsicmp( token, "#REGEN_FILE_NORMAL" ) == 0 ||
						_tcsicmp( token, "#REGEN_FILE_HARD" ) == 0 )
					{
						token = strtok( 0, delimit );
						if( token == NULL )		break;

						// 인던용 리젠파일 열기
						CMHFile dungeonRegenFile;
						std::string dungeonRegenFilePath( _T("System/Resource/") );
						dungeonRegenFilePath.append( token );
						ParseMonsterRegenInfo( m_MonsterRegenInfoList, dungeonRegenFilePath.c_str(), mapnum );
					}

					token = strtok( 0, delimit );
				}
			}
		}
		mhFile.Release();

	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadItemInfo()
{
	CMHFile file;
	if( file.Init( "./System/Resource/ItemList.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" ItemList.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !file.IsEOF() )
	{
		const DWORD dwItemIndex = file.GetDword();
		if( dwItemIndex == 0 )
		{
			char buf[MAX_PATH]={0,};
			file.GetLine( buf, sizeof(buf) );
			continue;
		}

		ITEM_INFO info={0};

		info.ItemIdx = dwItemIndex;

		_tcsncpy( info.ItemName, GetChangedLunaGameString( file.GetString() ), sizeof(info.ItemName)/sizeof(*info.ItemName) - 1 );
		info.ItemTooltipIdx = file.GetDword();
		info.Image2DNum = file.GetWord();	
		info.Part3DType = file.GetWord();
		info.Part3DModelNum = file.GetWord();	

		info.Shop = file.GetWord(); // 아이템몰 여부
		info.Category = eItemCategory(file.GetWord());
		info.Grade = file.GetWord();
		info.LimitRace = LIMIT_RACE(file.GetWord());
		info.LimitGender = file.GetWord();		
		info.LimitLevel = (WORD)file.GetLevel();	

		info.Stack = file.GetWord();
		info.Trade = file.GetWord();
		info.Deposit = file.GetWord();
		info.Sell = file.GetWord();
		info.Decompose = file.GetWord();

		info.DecomposeIdx = file.GetDword();
		info.Time = file.GetDword();

		info.BuyPrice = file.GetDword();			
		info.SellPrice = file.GetDword();	

		info.EquipType = eEquipType(file.GetWord());
		info.EquipSlot = EWEARED_ITEM(file.GetWord() - 1);
		info.Durability = file.GetDword();
		file.GetDword();

		info.Repair = file.GetWord();
		info.WeaponAnimation = eWeaponAnimationType(file.GetWord());
		info.WeaponType = eWeaponType(file.GetWord());
		info.PhysicAttack = file.GetInt();
		info.MagicAttack = file.GetInt();
		info.ArmorType = eArmorType(file.GetWord());
		info.AccessaryType = eAccessaryType(file.GetWord());
		info.PhysicDefense = file.GetInt();
		info.MagicDefense = file.GetInt();

		info.IsEnchant = file.GetBool();
		info.Improvement = file.GetWord();

		info.EnchantValue		= file.GetDword();
		info.EnchantDeterm	= file.GetDword();

		info.ImprovementStr = file.GetInt();			
		info.ImprovementDex = file.GetInt();			
		info.ImprovementVit = file.GetInt();		//10
		info.ImprovementInt = file.GetInt();
		info.ImprovementWis = file.GetInt();			
		info.ImprovementLife = file.GetInt();			
		info.ImprovementMana = file.GetInt();			

		info.ImprovementSkill = file.GetDword();
		info.Skill		= file.GetDword();
		info.SupplyType	= ITEM_KIND(file.GetWord());
		info.SupplyValue	= file.GetDword();
		info.kind			= ITEM_INFO::eKind( file.GetDword() );

		info.RequiredBuff = file.GetDword();
		info.Battle = file.GetBool();
		info.Peace = file.GetBool();
		info.Move = file.GetBool();
		info.Stop = file.GetBool();
		info.Rest = file.GetBool();
		info.wSeal = ITEM_SEAL_TYPE(file.GetWord());
		info.nTimeKind = ITEM_TIMEKIND(file.GetInt());
		info.dwUseTime = file.GetDword();
		info.dwBuyFishPoint = file.GetDword();
		info.wPointType = eItemPointType(file.GetWord());
		info.dwPointTypeValue1 = file.GetDword();
		info.dwPointTypeValue2 = file.GetDword();
		info.dwType =  file.GetDword(); // Item 종류
		info.dwTypeDetail =  file.GetDword();	// Item 세부종류

		m_ItemInfoList.insert( std::map< DWORD, ITEM_INFO >::value_type( info.ItemIdx, info ) );
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadSetItmeInfo()
{
	CMHFile mhFile;
	if( mhFile.Init("./data/interface/windows/setItemName.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" setItemName.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	std::map< DWORD, std::string> setItemNameList;

	while( !mhFile.IsEOF() )
	{
		char buf[MAX_PATH]={0};
		const char* delimit = "\t\n ";
		mhFile.GetLine( buf, sizeof(buf) );

		stImageInfo imageInfo;

		char* token = strtok( buf, delimit );
		if( token == NULL ) continue;
		const DWORD nameIndex = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		
		setItemNameList[ nameIndex ] = GetChangedLunaGameString( token );
	}
	mhFile.Clear();

	// setitem info load

	int nOpenCnt = 0;
	std::vector< int > setItemGroup;
	std::list< std::string > textList;

	enum Block
	{
		BlockNone,
		BlockSetGroup,
		BlockSetAbilityInfo,
	}
	blockType = BlockNone;

	if( mhFile.Init("./system/resource/setItem.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" setItem.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char line[MAX_PATH]={0};
		const char* delimit = "\t\n =,";
		mhFile.GetLine( line, sizeof(line) );

		// 중간의 주석 제거
		const int length = _tcslen( line );
		for( int i = 0; i < length - 1; ++i )
		{
			if( '/' == line[ i ] &&
				'/' == line[ i + 1 ] )
			{
				line[ i ] = 0;
				break;
			}
		}

		textList.push_back( line );

		char* token = strtok( line, delimit );
		if( token == NULL )
		{
			textList.pop_back();
			continue;
		}

		while( token != NULL )
		{
			if( _tcsicmp( token, "set" ) == 0 )
			{
				blockType = BlockSetGroup;
				textList.pop_back();
			}
			else if( _tcsicmp( token, "{" ) == 0 )
			{
				++nOpenCnt;

				blockType = BlockSetAbilityInfo;
			}
			else if( _tcsicmp( token, "}" ) == 0 )
			{
				if( --nOpenCnt == 0 )
				{
					// setitem info save
					if( !setItemGroup.empty() )
					{
						const int& keyitemIndex = setItemGroup.front();
						SetScript& script = m_SetItemInfoList[ keyitemIndex ];

						ParseSetItemAbility( textList, script, setItemNameList );

						for( std::vector< int >::const_iterator iterSetItemGroupIdx = setItemGroup.begin() ; iterSetItemGroupIdx != setItemGroup.end() ; ++iterSetItemGroupIdx )
						{
							const DWORD setItemIndex = (DWORD)*iterSetItemGroupIdx;
							script.mItem.insert( setItemIndex );
						}

						int a=0;
					}

					blockType = BlockNone;
					setItemGroup.clear();
					textList.clear();
				}
			}
			else
			{
				switch( blockType )
				{
				case BlockSetGroup:
					{
						const int itemIndex = atoi( token );
						setItemGroup.push_back( itemIndex );

						const int& keyitemIndex = setItemGroup.front();
						m_SetItemKeyIndexList.insert( std::map< DWORD, DWORD >::value_type( keyitemIndex, itemIndex ) );
					}
					break;
				}
			}

			token = strtok( NULL, delimit );
		}
	}

	return TRUE;
}

void CLunaGameDataExtracterDlg::ParseSetItemAbility( std::list< std::string >& text, SetScript& data, const std::map< DWORD, std::string>& name )
{
	// 080912 LUJ, 인덱스로 세트 아이템 이름을 가져오는 내부 클래스
	class SetItemNameContainer
	{
	public:
		SetItemNameContainer( const std::map< DWORD, std::string>& name ):
		  mSetName( name )
		  {}

		  const TCHAR* GetName( const TCHAR* index ) const
		  {
			  const std::map< DWORD, std::string>::const_iterator it = mSetName.find( _ttoi( index ) );

			  return mSetName.end() == it ? "" : it->second.c_str();
		  }

	private:
		const std::map< DWORD, std::string>& mSetName;
	}
	setItemNameContainer( name );

	int					openCount			= 0;
	SetScript::Element*	setElement			= 0;
	SetScript::Element* itemElement			= 0;

	for(	std::list< std::string >::const_iterator it = text.begin();
			text.end() != it;
			++it )
	{
		TCHAR line[ MAX_PATH ] = { 0 };
		_tcscpy( line, it->c_str() );

		const TCHAR* separator	= _T( " \n\t=,+\"()" );
		const TCHAR* markBegin	= _T( "{" );
		const TCHAR* markEnd	= _T( "}" );
		const TCHAR* token		= _tcstok( line, separator );
		
		if( ! token )
		{
			continue;
		}
		else if( ! _tcsicmp( "name", token ) )
		{
			token					= _tcstok( 0, separator );
			const DWORD nameIndex	= _tcstoul( token, 0, 10 );

			std::map< DWORD, std::string>::const_iterator name_it = name.find( nameIndex );

			if( name.end() != name_it )
			{
				data.mName = name_it->second;
			}
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "hand", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Weapon ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "shield", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Shield ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "armor", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Dress ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "helmet", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Hat ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "gloves", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Glove ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "boots", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Shoes ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "ring", token ) )
		{
			token = _tcstok( 0, separator );

			data.mSlotNameMap[ eWearedItem_Ring1 ] = setItemNameContainer.GetName( token );
			data.mSlotNameMap[ eWearedItem_Ring2 ] = setItemNameContainer.GetName( token );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "necklace", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Necklace ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		else if( ! _tcsicmp( "earring", token ) )
		{
			token = _tcstok( 0, separator );

			data.mSlotNameMap[ eWearedItem_Earring1 ] = setItemNameContainer.GetName( token );
			data.mSlotNameMap[ eWearedItem_Earring2 ] = setItemNameContainer.GetName( token );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "belt", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Belt ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "band", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Band ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "glasses", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Glasses ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "mask", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Mask ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "wing", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Wing ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "costume_head", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Costume_Head ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "costume_dress", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Costume_Dress ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "costume_glove", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Costume_Glove ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "costume_shoes", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Costume_Shoes ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 개수별 능력 파싱
		else if( ! _tcsicmp( "size", token ) )
		{
			// 081231 LUJ, 세트/아이템 능력 두 가지를 세트 스크립트에서 처리하므로, 값이 저장될 곳을 구분할 수 있도록 포인터를 초기화시켜야한다.
			setElement	= 0;
			itemElement	= 0;

			const size_t size = _tcstoul( _tcstok( 0, separator ), 0, 10 );

			if( setElement )
			{
				return;
			}

			setElement = &( data.mAbility[ size ] );
		}
		// 081231 LUJ, 아이템의 기능을 확장한다
		else if( ! _tcsicmp( "item", token ) )
		{
			// 081231 LUJ, 세트/아이템 능력 두 가지를 세트 스크립트에서 처리하므로, 값이 저장될 곳을 구분할 수 있도록 포인터를 초기화시켜야한다.
			setElement	= 0;
			itemElement	= 0;

			const TCHAR* const	text		= _tcstok( 0, separator );
			const DWORD			itemIndex	= _ttoi( text ? text : _T( "" ) );

			itemElement = &( data.mItemElementMap[ itemIndex ] );
		}
		else if( 0 == _tcsnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++openCount;
		}
		else if( 0 == _tcsnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--openCount;

			setElement = 0;
		}
		// 081231 LUJ, 세트 스크립트를 통해 아이템에 추가 능력을 부여할 수 있도록 한다
		else if( setElement || itemElement )
		{
			// 081231 LUJ, 인자로 받아들여진 포인터 중 널이 아닌 저장 공간을 참조로 반환하는 내부 함수
			class
			{
			public:
				PlayerStat& operator()( SetScript::Element* const setElement, SetScript::Element* const itemElement ) const
				{
					if( setElement )
					{
						return setElement->mStat;
					}
					else if( itemElement )
					{
						return itemElement->mStat;
					}

					static PlayerStat emptyStat;
					return emptyStat;
				}
			}
			GetValidStat;

			PlayerStat& stat = GetValidStat( setElement, itemElement );

			if( 0 == _tcsicmp( token, "physic_attack" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mPhysicAttack.mPercent = value / 100.0f;
				}
				else
				{
					stat.mPhysicAttack.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "magic_attack" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mMagicAttack.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mMagicAttack.mPlus	= value;
				}
			}
			else if( ! _tcsicmp( token, "physic_defense" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mPhysicDefense.mPercent = value / 100.0f;
				}
				else
				{
					stat.mPhysicDefense.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "magic_defense" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mMagicDefense.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mMagicDefense.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "strength" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mStrength.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mStrength.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "dexterity" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mDexterity.mPercent = value / 100.0f;
				}
				else
				{
					stat.mDexterity.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "vitality" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mVitality.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mVitality.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "intelligence" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mIntelligence.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mIntelligence.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "wisdom" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mWisdom.mPercent = value / 100.0f;
				}
				else
				{
					stat.mWisdom.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "life" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mLife.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mLife.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "mana" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mMana.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mMana.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "recovery_life" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mRecoveryLife.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mRecoveryLife.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "recovery_mana" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mRecoveryMana.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mRecoveryMana.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "accuracy" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mAccuracy.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mAccuracy.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "evade" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mEvade.mPercent = value / 100.0f;
				}
				else
				{
					stat.mEvade.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "critical_rate" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mCriticalRate.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mCriticalRate.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "critical_damage" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mCriticalDamage.mPercent = value / 100.0f;
				}
				else
				{
					stat.mCriticalDamage.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "move_speed" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mMoveSpeed.mPercent = value / 100.0f;
				}
				else
				{
					stat.mMoveSpeed.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "skill" ) )
			{
				SetScript::Element::Skill* skillMap = 0;

				if( setElement )
				{
					skillMap = &( setElement->mSkill );
				}
				else if( itemElement )
				{
					skillMap = &( itemElement->mSkill );
				}
				else
				{
					static SetScript::Element::Skill emptySkillMap;
					skillMap = &emptySkillMap;
				}
				
				while( (token = _tcstok( 0, separator ) )!= NULL)
				{
					const DWORD index	= _tcstoul( token, 0, 10 );
					const BYTE	level	= BYTE( _ttoi( _tcstok( 0, separator ) ) );

					( *skillMap )[ index ] = level;
				}
			}
		}
	}
}

BOOL CLunaGameDataExtracterDlg::LoadActiveSkillInfo()
{
	CMHFile mhFile;
	if( mhFile.Init("./system/resource/SkillList.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" SkillList.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char text[MAX_PATH]={0};
		LPCTSTR	separator = "\t";
		mhFile.GetLine( text, sizeof(text) );

		ACTIVE_SKILL_INFO info;
		ZeroMemory( &info, sizeof(info) );

		LPCTSTR textIndex = _tcstok(text, separator);
		info.Index = _ttoi(textIndex ? textIndex : "");

		LPCTSTR textName = _tcstok(0, separator);
		_tcsncpy( info.Name, GetChangedLunaGameString( textName ), sizeof(info.Name) / sizeof(*info.Name) );

		LPCTSTR textLevel = _tcstok(0, separator);
		info.Level = WORD(_ttoi(textLevel ? textLevel : ""));

		LPCTSTR textImage = _tcstok(0, separator);
		info.Image = _ttoi(textImage ? textImage : "");

		LPCTSTR textToolTip = _tcstok(0, separator);
		info.Tooltip = _ttoi(textToolTip ? textToolTip : "");

		LPCTSTR textOperatorEffect = _tcstok(0, separator);
		info.OperatorEffect = _ttoi(textOperatorEffect ? textOperatorEffect : "");

		LPCTSTR textTargetEffect = _tcstok(0, separator);
		info.TargetEffect = _ttoi(textTargetEffect ? textTargetEffect : "");

		LPCTSTR textSkillEffect = _tcstok(0, separator);
		info.SkillEffect = _ttoi(textSkillEffect ? textSkillEffect : "");

		LPCTSTR textAnimationTime = _tcstok(0, separator);
		info.AnimationTime = _ttoi(textAnimationTime ? textAnimationTime : "");

		LPCTSTR textCastingTime = _tcstok(0, separator);
		info.CastingTime = _ttoi(textCastingTime ? textCastingTime : "");

		LPCTSTR textCoolTime = _tcstok(0, separator);
		info.CoolTime = _ttoi(textCoolTime ? textCoolTime : "");

		LPCTSTR textDelayTime = _tcstok(0, separator);
		info.DelayTime = _ttoi(textDelayTime ? textDelayTime : "");

		LPCTSTR textDelayType = _tcstok(0, separator);
		info.DelayType = WORD(_ttoi(textDelayType ? textDelayType : ""));

		LPCTSTR textBattle = _tcstok(0, separator);
		info.mIsBattle = _ttoi(textBattle ? textBattle : "");

		LPCTSTR textPeace = _tcstok(0, separator);
		info.mIsPeace = _ttoi(textPeace ? textPeace : "");

		LPCTSTR textMove = _tcstok(0, separator);
		info.mIsMove = _ttoi(textMove ? textMove : "");

		LPCTSTR textStop = _tcstok(0, separator);
		info.mIsStop = _ttoi(textStop ? textStop : "");

		// 091211 LUJ, 휴식 시 사용 여부. 사용하지 않음
		_tcstok(0, separator);

		LPCTSTR textRequiredBuff = _tcstok(0, separator);
		info.RequiredBuff = _ttoi(textRequiredBuff ? textRequiredBuff : "");

		LPCTSTR textTarget = _tcstok(0, separator);
		info.Target = WORD(_ttoi(textTarget ? textTarget : ""));

		LPCTSTR textKind = _tcstok(0, separator);
		info.Kind = SKILLKIND(_ttoi(textKind ? textKind : ""));

		// 091211 LUJ, 계열 값. 사용하지 않음
		_tcstok(0, separator);

		LPCTSTR textEquip = _tcstok(0, separator);

		// 091211 LUJ, 장비 제한 처리를 간단히 하기 위해 게임 내 쓰이는 enum 상수로 대체한다
		{
			// 090204 LUJ, 장비 속성 추가. EquipItem 배열 대신 사용함
			enum Equip
			{
				EquipNone,
				EquipWeaponOneHand,
				EquipWeaponTwoHand,
				EquipWeaponTwoBlade,
				EquipWeaponBow,
				EquipWeaponGun,
				EquipWeaponDagger,
				EquipWeaponSpear,
				EquipWeaponStaff,
				EquipWeaponShield,
				EquipArmorRobe,
				EquipArmorLeather,
				EquipArmorMetal,
				EquipWeaponSwordOneHand,
				EquipWeaponSwordTwoHand,
				EquipWeaponSwordAll,
				EquipWeaponMaceOneHand,
				EquipWeaponMaceTwoHand,
				EquipWeaponMaceAll,
				EquipWeaponAxeOneHand,
				EquipWeaponAxeTwoHand,
				EquipWeaponAxeAll,
				EquipWeaponBothDagger,
			};

			switch(Equip(_ttoi(textEquip ? textEquip : "")))
			{
			case EquipWeaponOneHand:
				{
					info.mWeaponAnimationType = eWeaponAnimationType_OneHand;
					break;
				}
			case EquipWeaponTwoHand:
				{
					info.mWeaponAnimationType = eWeaponAnimationType_TwoHand;
					break;
				}
			case EquipWeaponTwoBlade:
				{
					info.mWeaponAnimationType = eWeaponAnimationType_TwoBlade;
					break;
				}
			case EquipWeaponBow:
				{
					info.mWeaponType = eWeaponType_Bow;
					break;
				}
			case EquipWeaponGun:
				{
					info.mWeaponType = eWeaponType_Gun;
					break;
				}
			case EquipWeaponDagger:
				{
					info.mWeaponType = eWeaponType_Dagger;
					break;
				}
			case EquipWeaponSpear:
				{
					info.mWeaponAnimationType = eWeaponAnimationType_Spear;
					break;
				}
			case EquipWeaponStaff:
				{
					info.mWeaponType = eWeaponType_Staff;
					break;
				}
			case EquipWeaponShield:
				{
					info.mArmorType = eArmorType_Shield;
					break;
				}
			case EquipArmorRobe:
				{
					info.mArmorType = eArmorType_Robe;
					break;
				}
			case EquipArmorLeather:
				{
					info.mArmorType = eArmorType_Leather;
					break;
				}
			case EquipArmorMetal:
				{
					info.mArmorType = eArmorType_Metal;
					break;
				}
			case EquipWeaponSwordOneHand:
				{
					info.mWeaponType = eWeaponType_Sword;
					info.mWeaponAnimationType = eWeaponAnimationType_OneHand;
					break;
				}
			case EquipWeaponSwordTwoHand:
				{
					info.mWeaponType = eWeaponType_Sword;
					info.mWeaponAnimationType = eWeaponAnimationType_TwoHand;
					break;
				}
			case EquipWeaponSwordAll:
				{
					info.mWeaponType = eWeaponType_Sword;
					info.mWeaponAnimationType = eWeaponAnimationType_OneHand;
					break;
				}
			case EquipWeaponMaceOneHand:
				{
					info.mWeaponType = eWeaponType_Mace;
					break;
				}
			case EquipWeaponMaceTwoHand:
				{
					info.mWeaponType = eWeaponType_Mace;
					info.mWeaponAnimationType = eWeaponAnimationType_TwoHand;
					break;
				}
			case EquipWeaponMaceAll:
				{
					info.mWeaponType = eWeaponType_Mace;
					break;
				}
			case EquipWeaponAxeOneHand:
				{
					info.mWeaponType = eWeaponType_Axe;
					info.mWeaponAnimationType = eWeaponAnimationType_OneHand;
					break;
				}
			case EquipWeaponAxeTwoHand:
				{
					info.mWeaponType = eWeaponType_Axe;
					info.mWeaponAnimationType = eWeaponAnimationType_TwoHand;
					break;
				}
			case EquipWeaponAxeAll:
				{
					info.mWeaponType = eWeaponType_Axe;
					break;
				}
			case EquipWeaponBothDagger:
				{
					info.mWeaponType = eWeaponType_BothDagger;
					break;
				}
			}
		}

		// 090204 LUJ, 무기2 항목. 사용하지 않음
		_tcstok(0, separator);

		LPCTSTR textArea = _tcstok(0, separator);
		info.Area = TARGET_KIND(_ttoi(textArea ? textArea : ""));

		LPCTSTR textAreaTarget = _tcstok(0, separator);
		info.AreaTarget = eSkillAreaTarget(_ttoi(textAreaTarget ? textAreaTarget : ""));

		LPCTSTR textAreaData = _tcstok(0, separator);
		info.AreaData = WORD(_ttoi(textAreaData ? textAreaData : ""));

		LPCTSTR textUnit = _tcstok(0, separator);
		info.Unit = UNITKIND(_ttoi(textUnit ? textUnit : ""));

		LPCTSTR textUnitDataType = _tcstok(0, separator);
		info.UnitDataType = WORD(_ttoi(textUnitDataType ? textUnitDataType : ""));

		LPCTSTR textUnitDataValue = _tcstok(0, separator);
		info.UnitData = _ttoi(textUnitDataValue ? textUnitDataValue : "");

		LPCTSTR textSuccess = _tcstok(0, separator);
		info.Success = WORD(_ttoi(textSuccess ? textSuccess : ""));

		LPCTSTR textCancel = _tcstok(0, separator);
		info.Cancel = WORD(_ttoi(textCancel ? textCancel : ""));

		LPCTSTR textRange = _tcstok(0, separator);
		info.Range = WORD(_ttoi(textRange ? textRange : ""));

		LPCTSTR textConsumeType = _tcstok(0, separator);
		info.mConsumePoint.mType = ACTIVE_SKILL_INFO::ConsumePoint::Type(_ttoi(textConsumeType ? textConsumeType : ""));

		LPCTSTR textConsumeValue = _tcstok(0, separator);
		info.mConsumePoint.mValue = _ttoi(textConsumeValue ? textConsumeValue : "");

		LPCTSTR textConsumeItemIndex = _tcstok(0, separator);
		info.mConsumeItem.wIconIdx = _ttoi(textConsumeItemIndex ? textConsumeItemIndex : "");

		LPCTSTR textConsumeItemQuantity = _tcstok(0, separator);
		info.mConsumeItem.Durability = _ttoi(textConsumeItemQuantity ? textConsumeItemQuantity : "");

		LPCTSTR textMoney = _tcstok(0, separator);
		info.Money = _ttoi(textMoney ? textMoney : "");

		LPCTSTR textMana = _tcstok(0, separator);

		if(textMana)
		{
			if('%' == textMana[_tcslen(textMana) - 1])
			{
				info.mMana.mPercent= float(_ttoi(textMana) / 100.0f);
			}
			else
			{
				info.mMana.mPlus = float(_ttoi(textMana));
			}
		}

		LPCTSTR textLife = _tcstok(0, separator);

		if(textLife)
		{
			if('%' == textLife[_tcslen(textLife) - 1])
			{
				info.mLife.mPercent= float(_ttoi(textLife) / 100.0f);
			}
			else
			{
				info.mLife.mPlus= float(_ttoi(textLife));
			}
		}

		LPCTSTR textTrainPoint = _tcstok(0, separator);
		info.TrainPoint = WORD(_ttoi(textTrainPoint ? textTrainPoint : ""));

		LPCTSTR textTrainMoney = _tcstok(0, separator);
		info.TrainMoney = _ttoi(textTrainMoney ? textTrainMoney : "");

		for(DWORD i = 0; i < MAX_BUFF_COUNT; ++i)
		{
			LPCTSTR textBuffIndex = _tcstok(0, separator);
			LPCTSTR textBuffRate = _tcstok(0, separator);

			if(0 == textBuffIndex)
			{
				continue;
			}
			else if(0 == textBuffRate)
			{
				continue;
			}

			info.Buff[i] = _ttoi(textBuffIndex ? textBuffIndex : "");
			info.BuffRate[i] = WORD(_ttoi(textBuffRate ? textBuffRate : ""));
		}

		LPCTSTR textIsMove = _tcstok(0, separator);
		info.IsMove = _ttoi(textIsMove ? textIsMove : "");

		LPCTSTR textRequiredPlayerLevel = _tcstok(0, separator);
		info.RequiredPlayerLevel = LEVELTYPE(_ttoi(textRequiredPlayerLevel ? textRequiredPlayerLevel : ""));

		m_ActiveSkillInfoList.insert( std::map< DWORD, ACTIVE_SKILL_INFO >::value_type( info.Index, info ) );
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadBuffSkillInfo()
{
	CMHFile mhFile;
	if( mhFile.Init("./system/resource/Skill_Buff_List.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" Skill_Buff_List.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char text[MAX_PATH]={0};
		LPCTSTR	separator = "\t";
		mhFile.GetLine( text, sizeof(text) );

		BUFF_SKILL_INFO info;
		ZeroMemory( &info, sizeof(info) );

		LPCTSTR textIndex = _tcstok(text, separator);
		info.Index = _ttoi(textIndex ? textIndex : "");

		LPCTSTR textName = _tcstok(0, separator);
		_tcsncpy( info.Name, GetChangedLunaGameString( textName ), sizeof(info.Name) / sizeof(*info.Name) );

		LPCTSTR textLevel = _tcstok(0, separator);
		info.Level = WORD(_ttoi(textLevel ? textLevel : ""));

		LPCTSTR textImage = _tcstok(0, separator);
		info.Image = _ttoi(textImage ? textImage : "");

		LPCTSTR textTip = _tcstok(0, separator);
		info.Tooltip = _ttoi(textTip ? textTip : "");

		LPCTSTR textSkillEffect = _tcstok(0, separator);
		info.SkillEffect = _ttoi(textSkillEffect ? textSkillEffect : "");

		LPCTSTR textKind = _tcstok(0, separator);
		info.Kind = SKILLKIND(_ttoi(textKind ? textKind : ""));

		LPCTSTR textDelayTime = _tcstok(0, separator);
		info.DelayTime = _ttoi(textDelayTime ? textDelayTime : "");

		// 091211 LUJ, 계열 값. 사용하지 않음
		_tcstok(0, separator);

		LPCTSTR textWeapon = _tcstok(0, separator);

		// 091211 LUJ, 방패 값이 갑옷 변수에 들어갈 수 있다. 방패 값이 설정된 후 0값으로 덮지 않도록 먼저 대입한다
		LPCTSTR textArmor = _tcstok(0, separator);
		info.mArmorType = eArmorType(_ttoi(textArmor ? textArmor : ""));

		// 091211 LUJ, 장비 제한 처리를 간단히 하기 위해 게임 내 쓰이는 enum 상수로 대체한다
		{
			enum eBuffSkillWeapon
			{
				eBuffSkillWeapon_None,
				eBuffSkillWeapon_Sword,
				eBuffSkillWeapon_Shield,
				eBuffSkillWeapon_Mace,
				eBuffSkillWeapon_Axe,
				eBuffSkillWeapon_Staff,
				eBuffSkillWeapon_Bow,
				eBuffSkillWeapon_Gun,
				eBuffSkillWeapon_Dagger,
				eBuffSkillWeapon_Spear,
				eBuffSkillWeapon_TwoHanded,
				eBuffSkillWeapon_OneHanded,
				eBuffSkillWeapon_TwoBlade,
				eBuffSkillWeapon_BothDagger,
				eBuffSkillWeapon_Cooking,
				eBuffSkillWeapon_FishingPole,
			};

			switch(eBuffSkillWeapon(_ttoi(textWeapon ? textWeapon : "")))
			{
			case eBuffSkillWeapon_Sword:
				{
					info.mWeaponType = eWeaponType_Sword;
					break;
				}
			case eBuffSkillWeapon_Shield:
				{
					info.mArmorType = eArmorType_Shield;
					break;
				}
			case eBuffSkillWeapon_Mace:
				{
					info.mWeaponType = eWeaponType_Mace;
					break;
				}
			case eBuffSkillWeapon_Axe:
				{
					info.mWeaponType = eWeaponType_Axe;
					break;
				}
			case eBuffSkillWeapon_Staff:
				{
					info.mWeaponType = eWeaponType_Staff;
					break;
				}
			case eBuffSkillWeapon_Bow:
				{
					info.mWeaponType = eWeaponType_Bow;
					break;
				}
			case eBuffSkillWeapon_Gun:
				{
					info.mWeaponType = eWeaponType_Gun;
					break;
				}
			case eBuffSkillWeapon_Dagger:
				{
					info.mWeaponType = eWeaponType_Dagger;
					break;
				}
			case eBuffSkillWeapon_Spear:
				{
					info.mWeaponAnimationType = eWeaponAnimationType_Spear;
					break;
				}
			case eBuffSkillWeapon_TwoHanded:
				{
					info.mWeaponAnimationType = eWeaponAnimationType_TwoHand;
					break;
				}
			case eBuffSkillWeapon_OneHanded:
				{
					info.mWeaponAnimationType = eWeaponAnimationType_OneHand;
					break;
				}
			case eBuffSkillWeapon_TwoBlade:
				{
					info.mWeaponAnimationType = eWeaponAnimationType_TwoBlade;
					break;
				}
			case eBuffSkillWeapon_BothDagger:
				{
					info.mWeaponType = eWeaponType_BothDagger;
					break;
				}
			case eBuffSkillWeapon_Cooking:
				{
					info.mWeaponType = eWeaponType_CookUtil;
					break;
				}
			case eBuffSkillWeapon_FishingPole:
				{
					info.mWeaponType = eWeaponType_FishingPole;
					break;
				}
			}
		}

		LPCTSTR textStatus = _tcstok(0, separator);
		info.Status = eStatusKind(_ttoi(textStatus ? textStatus : ""));

		LPCTSTR textStatusDataType = _tcstok(0, separator);
		LPCTSTR textStatuaDataValue = _tcstok(0, separator);

		switch( info.Status )
		{
		case eStatusKind_EventSkillToVictim:
		case eStatusKind_EventSkillToKiller:
			{
				info.mEventSkillIndex = _ttoi(textStatuaDataValue ? textStatuaDataValue : "");
				break;
			}
		default:
			{
				info.StatusDataType = BUFF_SKILL_INFO::DataType(_ttoi(textStatusDataType ? textStatusDataType : ""));
				info.StatusData = float(_tstof(textStatuaDataValue ? textStatuaDataValue : ""));
				break;
			}
		}

		LPCTSTR textConditionType = _tcstok(0, separator);
		info.mCondition.mType = BUFF_SKILL_INFO::Condition::Type(_ttoi(textConditionType ? textConditionType : ""));

		LPCTSTR textConditionOperation = _tcstok(0, separator);
		info.mCondition.mOperation = BUFF_SKILL_INFO::Condition::Operation(_ttoi(textConditionOperation ? textConditionOperation : ""));

		LPCTSTR textConditionValue = _tcstok(0, separator);
		info.mCondition.mValue = float(_tstof(textConditionValue ? textConditionValue : ""));

		LPCTSTR textBattle = _tcstok(0, separator);
		info.mRuleBattle = eBuffSkillActiveRule(_ttoi(textBattle ? textBattle : ""));

		LPCTSTR textPeace = _tcstok(0, separator);
		info.mRulePeace = eBuffSkillActiveRule(_ttoi(textPeace ? textPeace : ""));

		LPCTSTR textMove = _tcstok(0, separator);
		info.mRuleMove = eBuffSkillActiveRule(_ttoi(textMove ? textMove : ""));

		LPCTSTR textStop = _tcstok(0, separator);
		info.mRuleStop = eBuffSkillActiveRule(_ttoi(textStop ? textStop : ""));

		LPCTSTR textRest = _tcstok(0, separator);
		info.mRuleRest = eBuffSkillActiveRule(_ttoi(textRest ? textRest : ""));

		LPCTSTR textDie = _tcstok(0, separator);
		info.Die = _ttoi(textDie ? textDie : "");

		LPCTSTR textNoUpdate = _tcstok(0, separator);
		info.NoUpdate = _ttoi(textNoUpdate ? textNoUpdate : "");

		LPCTSTR textUseItem1Count = _tcstok(0, separator);
		info.ItemCount[0] = WORD(_ttoi(textUseItem1Count ? textUseItem1Count : ""));

		LPCTSTR textUseItem2Count = _tcstok(0, separator);
		info.ItemCount[1] = WORD(_ttoi(textUseItem2Count ? textUseItem2Count : ""));

		LPCTSTR textUseItem1Index = _tcstok(0, separator);
		info.Item[0] = DWORD(_ttoi(textUseItem1Index ? textUseItem1Index : ""));

		LPCTSTR textUseItem2Index = _tcstok(0, separator);
		info.Item[1] = DWORD(_ttoi(textUseItem2Index ? textUseItem2Index : ""));

		LPCTSTR textMoney = _tcstok(0, separator);
		info.Money = _ttoi(textMoney ? textMoney : "");

		LPCTSTR textMana = _tcstok(0, separator);
		info.mMana.mPlus = float(_tstof(textMana ? textMana : ""));

		LPCTSTR textLife = _tcstok(0, separator);
		info.mLife.mPlus = float(_tstof(textLife ? textLife : ""));

		LPCTSTR textCount = _tcstok(0, separator);
		info.Count = _ttoi(textCount ? textCount : "");

		LPCTSTR textCountType = _tcstok(0, separator);
		info.CountType = eBuffSkillCountType(_ttoi(textCountType ? textCountType : ""));

		LPCTSTR textIsEndTime = _tcstok(0, separator);
		info.IsEndTime = _ttoi(textIsEndTime ? textIsEndTime : "");

		m_BuffSkillInfoList.insert( std::map< DWORD, BUFF_SKILL_INFO >::value_type( info.Index, info ) );
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadSkillGetListInfo()
{
	CMHFile mhFile;
	if( mhFile.Init("./system/resource/skill_get_list.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" skill_get_list.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		LPCTSTR	delimit = "\t\n ";
		mhFile.GetLine( buf, sizeof(buf) );

		char* token = strtok( buf, delimit );
		if( token == NULL ) continue;
		const DWORD classindex = (DWORD)atoi( token );

		token = strtok( NULL, delimit );
		if( token == NULL ) continue;
		int count = atoi( token );

		while( ( token = strtok( NULL, delimit ) ) != NULL )
		{
			const DWORD skillindex = (DWORD)atoi( token );

			CANLEARNSKILLINDEXLIST& canLearnSkillIndexList = m_ClassCanLearnSkillIndex[ classindex ];
			canLearnSkillIndexList.push_back( skillindex );

			m_SkillIndexBelongToClass.insert( std::map< DWORD, DWORD >::value_type( skillindex, classindex ) );
		}
	}
	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadNpcMsg()
{
	CMHFile mhFile;
	if( mhFile.Init("./data/script/npc/Npc_Msg.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" Npc_Msg.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	enum Block{
		BlockNone,
		BlockMsg,
	}
	blockType = BlockNone;

	DWORD msgindex = 0;

	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		LPCTSTR	delimitNone = "\t\n{ ";
		LPCTSTR	delimitMsg = "\t\n{";
		mhFile.GetLine( buf, sizeof(buf) );

		char* token = NULL;
		switch( blockType )
		{
		case BlockNone:		token = strtok( buf, delimitNone );		break;
		case BlockMsg:		token = strtok( buf, delimitMsg );		break;
		}

		while( token != NULL )
		{
			if( _tcsicmp( token, "#msg" ) == 0 )
			{
				msgindex = (DWORD)_ttoi( strtok( 0, delimitNone ) );
				blockType = BlockMsg;
				token = strtok( 0, delimitMsg );
				continue;
			}

			switch( blockType )
			{
			case BlockMsg:
				{
					if( _tcsicmp( token, "}" ) == 0 )
					{
						blockType = BlockNone;
					}
					else
					{
						std::string& msg = m_NpcMsgList[ msgindex ];
						CString strMsg( token );
						strMsg.Replace( "$cs", "" );
						strMsg.Replace( "$ce", "" );
						msg.append( strMsg );
						msg.append( "\n" );
					}
				}
				break;
			}

			switch( blockType )
			{
			case BlockNone:		token = strtok( 0, delimitNone );		break;
			case BlockMsg:		token = strtok( 0, delimitMsg );		break;
			}
		}
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadNpcHyperText()
{
	CMHFile mhFile;
	if( mhFile.Init("./data/script/npc/Npc_HyperText.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" Npc_HyperText.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		LPCTSTR	delimit = "\t\n";
		mhFile.GetLine( buf, sizeof(buf) );

		char* token = strtok( buf, delimit );
		if( token == NULL ) continue;
		const DWORD hyperindex = (DWORD)_ttoi( token );

		token = strtok( 0, delimit );
		if( token == NULL ) continue;
		m_NpcHyperTextList[ hyperindex ] = token;
	}
	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadNpcScript()
{
	CMHFile mhFile;
	if( mhFile.Init("./data/script/npc/Npc_Script.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" Npc_Script.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	enum Block{
		BlockNone,
		BlockNpc,
		BlockPage,
	}
	blockType = BlockNone;

	DWORD npcid = 0;
	DWORD pageindex = 0;

	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		LPCTSTR	delimit = "\t\n {";
		mhFile.GetLine( buf, sizeof(buf) );

		char* token = strtok( buf, delimit );
		while( token != NULL )
		{
			if( _tcsicmp( token, "$npc" ) == 0 )
			{
				blockType = BlockNpc;
			}
			else if( _tcsicmp( token, "$page" ) == 0 )
			{
				blockType = BlockPage;
			}

			switch( blockType )
			{
			case BlockNpc:
				{
					if( _tcsicmp( token, "}" ) == 0 )
					{
						npcid = 0;
						blockType = BlockNone;
					}
					else if( _tcsicmp( token, "#npcid" ) == 0 )
					{
						npcid = (DWORD)_ttoi( strtok( 0, delimit ) );
					}
				}
				break;
			case BlockPage:
				{
					if( _tcsicmp( token, "}" ) == 0 )
					{
						pageindex = 0;
						blockType = BlockNpc;
					}
					else if( _tcsicmp( token, "#pageinfo" ) == 0 )
					{
						token = strtok( 0, delimit );
						if( token == NULL ) continue;
						pageindex = (DWORD)_ttoi( token );
					}
					else if( _tcsicmp( token, "#dialogue" ) == 0 )
					{
						token = strtok( 0, delimit );
						if( token == NULL ) continue;
						const msgindex = (DWORD)_ttoi( token );

						if( npcid == 0 || pageindex == 0 ) continue;

						NPCPAGEMSG& npcpagemsg = m_NpcScriptMsgList[ npcid ];
						std::string& npcmsg = npcpagemsg[ pageindex ];
						const std::string npcdialogmsg = m_NpcMsgList[ msgindex ];

						if( !npcmsg.empty() )
							npcmsg.append( "\n" );
						npcmsg.append( npcdialogmsg );
					}
					else if( _tcsicmp( token, "#hyperlink" ) == 0 )
					{
						token = strtok( 0, delimit );
						if( token == NULL ) continue;
						const DWORD hyperindex = (DWORD)_ttoi( token );

						token = strtok( 0, delimit );
						if( token == NULL ) continue;
						const DWORD hypertype = (DWORD)_ttoi( token );

						token = strtok( 0, delimit );
						if( token == NULL ) continue;
						const DWORD hyperdata = (DWORD)_ttoi( token );

						if( npcid == 0 || pageindex == 0 ) continue;

						NPCPAGEMSG& npcpagemsg = m_NpcScriptMsgList[ npcid ];
						std::string& npcmsg = npcpagemsg[ pageindex ];
						const std::string hypertext = m_NpcHyperTextList[ hyperindex ];

						if( !npcmsg.empty() )
							npcmsg.append( "\n" );
						npcmsg.append( "[" );
						npcmsg.append( hypertext );
						npcmsg.append( "]" );

						if( hypertype != 0 )
							continue;

						const std::string npcdialogmsg = m_NpcMsgList[ hyperdata ];
						npcmsg.append( "\n" );
						npcmsg.append( npcdialogmsg );
					}
				}
				break;
			}

			token = strtok( 0, delimit );
		}
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadNpcInfo()
{
	CMHFile mhFile;
	if( mhFile.Init("./system/resource/StaticNpc.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" StaticNpc.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		LPCTSTR	delimit = "\t\n {";
		mhFile.GetLine( buf, sizeof(buf) );

		char* token = strtok( buf, delimit );
		if( token == NULL ) continue;

		token = strtok( 0, delimit );
		if( token == NULL ) continue;

		token = strtok( 0, delimit );
		if( token == NULL ) continue;

		char name[MAX_PATH]={0};
		_tcsncpy( name, token, sizeof(name)/sizeof(*name) );

		token = strtok( 0, delimit );
		if( token == NULL ) continue;
		DWORD npcindex = (DWORD)_ttoi( token );

		m_NpcName[ npcindex ] = GetChangedLunaGameString( name );
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadQuestTitleText()
{
	CMHFile mhFile;
	if( mhFile.Init("./system/resource/QuestString.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" QuestString.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	enum Block{
		BlockNone,
		BlockDesc,
	}
	blockType = BlockNone;


	std::pair< DWORD, DWORD > key;

	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		LPCTSTR	delimitNone = "\t\n {";
		LPCTSTR	delimitDesc = "\t\n{";

		mhFile.GetLine( buf, sizeof(buf) );

		char* token = NULL;
		switch( blockType )
		{
		case BlockNone:		token = strtok( buf, delimitNone );		break;
		case BlockDesc:		token = strtok( buf, delimitDesc );		break;
		}

		while( token != NULL )
		{
			if( _tcsicmp( token, "$SUBQUESTSTR" ) == 0 )
			{
				token = strtok( 0, delimitNone );
				if( token == NULL ) continue;
				const DWORD questindex = (DWORD)_ttoi( token );

				token = strtok( 0, delimitNone );
				if( token == NULL ) continue;
				const DWORD subquestindex = (DWORD)_ttoi( token );

				key = std::make_pair( questindex, subquestindex );
			}
			else if( _tcsicmp( token, "#TITLE" ) == 0 )
			{
				token = strtok( 0, "\t{}" );
				if( token == NULL ) continue;
				m_QuestTitleList[ key ] = token;
			}
			else if( _tcsicmp( token, "#DESC" ) == 0 )
			{
				blockType = BlockDesc;
				token = strtok( 0, delimitDesc );
				continue;
			}

			switch( blockType )
			{
			case BlockDesc:
				{
					if( _tcsicmp( token, "}" ) == 0 )
					{
						blockType = BlockNone;
					}
					else
					{
						std::string& title = m_QuestTitleList[ key ];
						title.append( "\n" );
						title.append( token );
					}
				}
				break;
			}

			switch( blockType )
			{
			case BlockNone:		token = strtok( 0, delimitNone );		break;
			case BlockDesc:		token = strtok( 0, delimitDesc );		break;
			}
		}
	}

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::LoadQuestItemInfo()
{
	CMHFile mhFile;
	if( mhFile.Init("./system/resource/QuestItemList.bin", "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" QuestItemList.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		LPCTSTR	delimit = "\t\n ";
		mhFile.GetLine( buf, sizeof(buf) );

		char* token = strtok( buf, delimit );
		if( token == NULL ) continue;
		DWORD questitemindex = (DWORD)_ttoi( token );

		stQuestItenInfo& questiteminfo = m_QuestItemInfo[ questitemindex ];
		questiteminfo.index = questitemindex;

		token = strtok( 0, delimit );
		if( token == NULL ) continue;
		questiteminfo.name = GetChangedLunaGameString( token );

		token = strtok( 0, delimit );
		if( token == NULL ) continue;
		questiteminfo.questindex = (DWORD)_ttoi( token );

		token = strtok( 0, delimit );
		if( token == NULL ) continue;
		questiteminfo.subquestindex = (DWORD)_ttoi( token );

		token = strtok( 0, delimit );
		if( token == NULL ) continue;

		token = strtok( 0, delimit );
		if( token == NULL ) continue;

		token = strtok( 0, delimit );
		if( token == NULL ) continue;
		questiteminfo.itemdesc = GetChangedLunaGameString( token );
	}

	return TRUE;
}

// 090603 LUJ, 맵 추가 작업이 쉽도록 수정
LPCTSTR CLunaGameDataExtracterDlg::GetMapName( DWORD mapIndex )
{
	switch( MapName( mapIndex ) )
	{
	case Alker_Farm:
#ifdef _CLIENT_
		return GetSystemMsg( 1067 );
#else
		return _T( "Alker Farm" );
#endif
		// 090921 LUJ, 네라 농장 추가
	case Nera_Farm:
#ifdef _CLIENT_
		return GetSystemMsg( 1937 );
#else
		return _T( "Nera Farm" );
#endif
	case Moon_Blind_Swamp:
#ifdef _CLIENT_
		return GetSystemMsg( 142 );
#else
		return _T( "Moon Blind Swamp" );
#endif
	case Red_Orc_Outpost:
#ifdef _CLIENT_
		return GetSystemMsg( 143 );
#else
		return _T( "Red Orc Outpost" );
#endif
	case Moon_Blind_Forest:
#ifdef _CLIENT_
		return GetSystemMsg( 141 );
#else
		return _T( "Moon Blind Forest" );
#endif
	case Haunted_Mine_1F:
#ifdef _CLIENT_
		return GetSystemMsg( 1226 );
#else
		return _T( "Haunted Mine 1F" );
#endif
	case Haunted_Mine_2F:
#ifdef _CLIENT_
		return GetSystemMsg( 1227 );
#else
		return _T( "Haunted Mine 2F" );
#endif
	case The_Gate_Of_Alker:
#ifdef _CLIENT_
		return GetSystemMsg( 137 );
#else
		return _T( "The Gate Of Alker" );
#endif
	case Alker_Harbor:
#ifdef _CLIENT_
		return GetSystemMsg( 138 );
#else
		return _T( "Alker Harbor" );
#endif
	case Ruins_Of_Draconian:
#ifdef _CLIENT_
		return GetSystemMsg( 139 );
#else
		return _T( "Ruins of Draconian" );
#endif
	case Zakandia:
#ifdef _CLIENT_
		return GetSystemMsg( 145 );
#else
		return _T( "Zakandia" );
#endif
	case Tarintus:
#ifdef _CLIENT_
		return GetSystemMsg( 144 );
#else
		return _T( "Tarintus" );
#endif
	case Mont_Blanc_Port:
#ifdef _CLIENT_
		return GetSystemMsg( 146 );
#else
		return _T( "Mont Blanc Port" );
#endif
	case Dryed_Gazell_Fall:
#ifdef _CLIENT_
		return GetSystemMsg( 147 );
#else
		return _T( "Dried Gazell Fall" );
#endif
	case Zakandia_Outpost:
#ifdef _CLIENT_
		return GetSystemMsg( 140 );
#else
		return _T( "Zakandia Outpost" );
#endif
	case The_Dark_Portal:
#ifdef _CLIENT_
		return GetSystemMsg( 150 );
#else
		return _T( "The Dark Portal" );
#endif
	case Distorted_crevice:
#ifdef _CLIENT_
		return GetSystemMsg( 1260 );
#else
		return _T( "Distorted crevice" );
#endif
	case The_Way_To_The_Howling_Ravine:
#ifdef _CLIENT_
		return GetSystemMsg( 148 );
#else
		return _T( "The Way to the Howling Ravine" );
#endif
	case Howling_Ravine:
#ifdef _CLIENT_
		return GetSystemMsg( 1036 );
#else
		return _T( "Howling Ravine" );
#endif
	case Howling_Cave_1F:
#ifdef _CLIENT_
		return GetSystemMsg( 1032 );
#else
		return _T( "Howling Cave 1F" );
#endif
	case Howling_Cave_2F:
#ifdef _CLIENT_
		return GetSystemMsg( 1033 );
#else
		return _T( "Howling Cave 2F" );
#endif
	case Ghost_Tree_Swamp:
#ifdef _CLIENT_
		return GetSystemMsg( 1251 );
#else
		return _T( "Ghost Tree Swamp" );
#endif
	case Lair_of_Kierra:
#ifdef _CLIENT_
		return GetSystemMsg( 1328 );
#else
		return _T( "Lair of Kierra" );
#endif
	case The_Valley_Of_Fairy:
#ifdef _CLIENT_
		return GetSystemMsg( 1426 );
#else
		return _T( "The Valley Of Fairy" );
#endif
	case The_Town_of_Nera_Castle:
#ifdef _CLIENT_
		return GetSystemMsg( 1569 );
#else
		return _T( "The Town of Nera Castle" );
#endif
	case The_Great_Garden:
#ifdef _CLIENT_
		return GetSystemMsg( 1572 );
#else
		return _T( "The Great Garden" );
#endif
	case TheKnightsGrave:
#ifdef _CLIENT_
		return GetSystemMsg( 1596 );
#else
		return _T( "The Knights Grave" );
#endif
	case A_Harbor_of_Nera:
#ifdef _CLIENT_
		return GetSystemMsg( 1570 );
#else
		return _T( "A Harbor of Nera" );
#endif
	case Temple_of_Greed:
#ifdef _CLIENT_
		return GetSystemMsg( 2209 );
#else
		return _T( "Temple_of_Greed" );
#endif
	case Broken_Shipwrecked:
#ifdef _CLIENT_
		return GetSystemMsg( 2208 );
#else
		return _T( "Broken_Shipwrecked" );
#endif

	case The_tower_of_wizardly_Low:
#ifdef _CLIENT_
		return GetSystemMsg( 1758 );
#else
		return _T( "The Tower of Wizardly Low" );
#endif
	case The_tower_of_wizardly_Middle:
#ifdef _CLIENT_
		return GetSystemMsg( 1759 );
#else
		return _T( "The Tower of Wizardly Middle" );
#endif
	case The_tower_of_wizardly_High:
#ifdef _CLIENT_
		return GetSystemMsg( 1760 );
#else
		return _T( "The Tower of Wizardly High" );
#endif
	case The_Death_tower:
#ifdef _CLIENT_
		return GetSystemMsg( 2030 );
#else
		return _T( "The Death tower" );
#endif
	case The_Spirit_Forest:
#ifdef _CLIENT_
		return GetSystemMsg( 2031 );
#else
		return _T( "The Spirit Forest" );
#endif
	case The_Cristal_Empire:
#ifdef _CLIENT_
		return GetSystemMsg( 2032 );
#else
		return _T( "The Cristal Empire" );
#endif
	case SahelZone:
#ifdef _CLIENT_
		return GetSystemMsg( 1911 );
#else
		return _T( "Sahel Zone" );
#endif
	case MirageInTheParasus:
#ifdef _CLIENT_
		return GetSystemMsg( 1912 );
#else
		return _T( "Mirage in the Parasus" );
#endif
	case RuinsOfAncientElpis:
#ifdef _CLIENT_
		return GetSystemMsg( 1913 );
#else
		return _T( "Ruins of Ancient Elpis" );
#endif
	case DragoniansTomb1:
#ifdef _CLIENT_
		return GetSystemMsg( 1914 );
#else
		return _T( "Dragonian's Tomb #1" );
#endif
	case DragoniansTomb2:
#ifdef _CLIENT_
		return GetSystemMsg( 1915 );
#else
		return _T( "Dragonian's Tomb #2" );
#endif
	case The_Castle_of_Rushen:
#ifdef _CLIENT_
		return GetSystemMsg( 1675 );
#else
		return _T( "The Castle of Rushen" );
#endif
	case The_Culvert:
#ifdef _CLIENT_
		return GetSystemMsg( 1673 );
#else
		return _T( "The Culvert" );
#endif
	case The_Castle_of_Zevyn:
#ifdef _CLIENT_
		return GetSystemMsg( 1674 );
#else
		return _T( "The Castle of Zevyn" );
#endif
	case Advanced_Dungeon:
#ifdef _CLIENT_
		return GetSystemMsg( 1676 );
#else
		return _T( "Advanced Dungeon" );
#endif
	case DateDungeon_Cemetery:
#ifdef _CLIENT_
		return GetSystemMsg( 2317 );
#else
		return _T( "Date Dungeon - Cemetery" );
#endif
	case DateDungeon_BrokenHouse:
#ifdef _CLIENT_
		return GetSystemMsg( 2318 );
#else
		return _T( "Date Dungeon - Broken House" );
#endif
	case DateDungeon_DarkCave:
#ifdef _CLIENT_
		return GetSystemMsg( 2319 );
#else
		return _T( "Date Dungeon - Dark Cave" );
#endif
	case GuildTournamentStadium:
#ifdef _CLIENT_
		return GetSystemMsg( 1654 );
#else
		return _T( "Guild Tournament" );
#endif
	case DesolatedGorge:
#ifdef _CLIENT_
		return GetSystemMsg( 1918 );
#else
		return _T( "Desolated Gorge" );
#endif
	case DateDungeon_CookieGarden:
#ifdef _CLIENT_
		return GetSystemMsg( 2320 );
#else
		return _T( "Date Dungeon - Cookie Garden" );
#endif
	case AlkerResidence:
#ifdef _CLIENT_
		return GetSystemMsg( 1917 );
#else
		return _T( "Alker residence" );
#endif
	}

	return _T( "?" );
}

// !주의
// skill_get_list.bin에서 엘프/휴먼 종족간 갈수 없는 직업 구분이 되어 있지않아
// 리소스로는 구분될수 잇는 방법이 없음.
// 클라이언트와 별도로 현재 클래스숫자에서 갈수 없는 직업의 숫자를 이름함수에서 제거/구분한다.
DWORD CLunaGameDataExtracterDlg::GetClassNameNum( int classIdx )
{
	DWORD msg = 0;
	switch( classIdx )
	{
	case 1111:	msg = 364;	break;
	case 1121:	msg = 365;	break;
	case 1122:	msg = 366;	break;
	case 1131:	msg = 367;	break;
	case 1132:	msg = 368;	break;
	case 1133:	msg = 369;	break;
	case 1141:	msg = 370;	break;
	case 1142:	msg = 371;	break;
	case 1143:	msg = 372;	break;
//	case 1144:	msg = 373;	break;		룬나이트(휴먼)
	case 1151:	msg = 374;	break;
	case 1152:	msg = 375;	break;
	case 1153:	msg = 376;	break;
	case 1154:	msg = 377;	break;
	case 1155:	msg = 378;	break;
//	case 1156:	msg = 379;	break;		매그너스(휴먼)
	case 1161:	msg = 380;	break;
	case 1162:	msg = 381;	break;
	case 1163:	msg = 382;	break;

	case 1211:	msg = 364;	break;
	case 1221:	msg = 365;	break;
	case 1222:	msg = 366;	break;
	case 1231:	msg = 367;	break;
	case 1232:	msg = 368;	break;
//	case 1233:	msg = 369;	break;		머셔너리(엘프)
	case 1241:	msg = 370;	break;
	case 1242:	msg = 371;	break;
//	case 1243:	msg = 372;	break;		글래디에이터(엘프)
	case 1244:	msg = 373;	break;
	case 1251:	msg = 374;	break;
	case 1252:	msg = 375;	break;
	case 1253:	msg = 376;	break;
//	case 1254:	msg = 377;	break;		디스트로이어(엘프)
	case 1255:	msg = 378;	break;
	case 1256:	msg = 379;	break;
	case 1261:	msg = 380;	break;
	case 1262:	msg = 381;	break;
	case 1263:	msg = 382;	break;

	case 2111:	msg = 383;	break;
	case 2121:	msg = 384;	break;
	case 2122:	msg = 385;	break;
	case 2131:	msg = 386;	break;
	case 2132:	msg = 387;	break;
//	case 2133:	msg = 388;	break;		스카우트(휴먼)
	case 2141:	msg = 389;	break;
	case 2142:	msg = 390;	break;
	case 2143:	msg = 391;	break;
//	case 2144:	msg = 392;	break;		룬워커(휴먼)
	case 2151:	msg = 393;	break;
	case 2152:	msg = 394;	break;
	case 2153:	msg = 395;	break;
//	case 2154:	msg = 396;	break;		템퍼 마스터(휴먼)

	// 080709 LYW --- cResourceManager : 아크레인저 직업을 추가한다.
	case 2155:  msg = 1004;	break;

	case 2161:	msg = 397;	break;
	case 2162:	msg = 398;	break;
	case 2163:	msg = 399;	break;

	case 2211:	msg = 383;	break;
	case 2221:	msg = 384;	break;
	case 2222:	msg = 385;	break;
	case 2231:	msg = 386;	break;
//	case 2232:	msg = 387;	break;		시프(엘프)
	case 2233:	msg = 388;	break;
	case 2241:	msg = 389;	break;
	case 2242:	msg = 390;	break;
//	case 2243:	msg = 391;	break;		어쎄신(엘프)
	case 2244:	msg = 392;	break;
	case 2251:	msg = 393;	break;
	case 2252:	msg = 394;	break;
//	case 2253:	msg = 395;	break;		블레이드 테이커(엘프)
	case 2254:	msg = 396;	break;

	// 080709 LYW --- cResourceManager : 아크레인저 직업을 추가한다.
	case 2255:	msg = 1004;	break;

	case 2261:	msg = 397;	break;
	case 2262:	msg = 398;	break;
	case 2263:	msg = 399;	break;

	case 3111:	msg = 400;	break;
	case 3121:	msg = 401;	break;
	case 3122:	msg = 402;	break;
	case 3131:	msg = 403;	break;
	case 3132:	msg = 404;	break;
	case 3133:	msg = 405;	break;
	case 3141:	msg = 406;	break;
	case 3142:	msg = 407;	break;
	case 3143:	msg = 408;	break;
//	case 3144:	msg = 409;	break;		엘리멘탈 마스터(휴먼)
	case 3151:	msg = 410;	break;
	case 3152:	msg = 411;	break;
	case 3153:	msg = 412;	break;
	case 3154:	msg = 413;	break;
//	case 3155:	msg = 414;	break;		룬 마스터(휴먼)
	case 3161:	msg = 415;	break;
	case 3162:	msg = 416;	break;
	case 3163:	msg = 417;	break;

	case 3211:	msg = 400;	break;
	case 3221:	msg = 401;	break;
	case 3222:	msg = 402;	break;
	case 3231:	msg = 403;	break;
	case 3232:	msg = 404;	break;
//	case 3233:	msg = 405;	break;		몽크(엘프)
	case 3241:	msg = 406;	break;
	case 3242:	msg = 407;	break;
//	case 3243:	msg = 408;	break;		인콰이어러(엘프)
	case 3244:	msg = 409;	break;
	case 3251:	msg = 410;	break;
	case 3252:	msg = 411;	break;
	case 3253:	msg = 412;	break;
//	case 3254:	msg = 413;	break;		네크로맨서(엘프)
	case 3255:	msg = 414;	break;
	case 3261:	msg = 415;	break;
	case 3262:	msg = 416;	break;
	case 3263:	msg = 417;	break;

	// 090504 ONS 신규종족 추가에 따른 직업(전직단계)추가
	case 4311:	msg = 1132; break;
	case 4321:	msg = 1133; break;
	case 4331:	msg = 1134; break;
	case 4341:	msg = 1135; break;
	case 4351:	msg = 1136; break;
	}

	return msg ;
}

LPCTSTR CLunaGameDataExtracterDlg::GetSystemMsg( DWORD index )
{
	std::map< DWORD, std::string >::const_iterator iterSysteMsg = m_SysteMsgList.find( index );
	if( iterSysteMsg == m_SysteMsgList.end() )
		return _T("");

	return iterSysteMsg->second.c_str();
}

LPCTSTR CLunaGameDataExtracterDlg::GetInterfaceMsg( DWORD index )
{
	std::map< DWORD, std::string >::const_iterator iterInterfaceMsg = m_InterfaceMsgList.find( index );
	if( iterInterfaceMsg == m_InterfaceMsgList.end() )
		return _T("");

	return iterInterfaceMsg->second.c_str();
}

LPCTSTR CLunaGameDataExtracterDlg::GetToolTipMsg( DWORD index )
{
	std::map< DWORD, std::string >::const_iterator iterTooltipMsg = m_TooltipMsgList.find( index );
	if( iterTooltipMsg == m_TooltipMsgList.end() )
		return _T("");

	return iterTooltipMsg->second.c_str();	
}

const ITEM_INFO& CLunaGameDataExtracterDlg::GetItemInfo( DWORD index ) const
{
	std::map< DWORD, ITEM_INFO >::const_iterator iter = m_ItemInfoList.find( index );
	if( iter == m_ItemInfoList.end() )
	{
		static ITEM_INFO emptyItemInfo;
		return emptyItemInfo;
	}

	return iter->second;
}

BOOL CLunaGameDataExtracterDlg::GetItemIconInfo( DWORD itemIconImageIndex, CString& strImageFilePath, RECT& imageRect ) const 
{
	IMAGEINFOMAP::const_iterator iterItemIconImageInfo =  m_ItemIconImageInfoList.find( itemIconImageIndex );
	if( iterItemIconImageInfo == m_ItemIconImageInfoList.end() )
		return FALSE;

	const stImageInfo& imageInfo = iterItemIconImageInfo->second;
	std::map< DWORD, stImagePathInfo >::const_iterator iterImagePathInfo = m_ImagePathInfoList.find( imageInfo.imagePathInfoIdx );
	if( iterImagePathInfo == m_ImagePathInfoList.end() )
		return FALSE;

	const stImagePathInfo& imagePathInfo = iterImagePathInfo->second;
	strImageFilePath = imagePathInfo.path.c_str();
	imageRect = imageInfo.rect;

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::GetSkillIconInfo( DWORD skillIconImageIndex, CString& strImageFilePath, RECT& imageRect ) const
{
	IMAGEINFOMAP::const_iterator iterSkillIconImageInfo =  m_SkillIconImageInfoList.find( skillIconImageIndex );
	if( iterSkillIconImageInfo == m_SkillIconImageInfoList.end() )
		return FALSE;

	const stImageInfo& imageInfo = iterSkillIconImageInfo->second;
	std::map< DWORD, stImagePathInfo >::const_iterator iterImagePathInfo = m_ImagePathInfoList.find( imageInfo.imagePathInfoIdx );
	if( iterImagePathInfo == m_ImagePathInfoList.end() )
		return FALSE;

	const stImagePathInfo& imagePathInfo = iterImagePathInfo->second;
	strImageFilePath = imagePathInfo.path.c_str();
	imageRect = imageInfo.rect;

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::IsSetItem( DWORD itemIndex ) const
{
	return (m_SetItemKeyIndexList.find( itemIndex ) != m_SetItemKeyIndexList.end());
}

const SetScript& CLunaGameDataExtracterDlg::GetSetItemScript( DWORD itemIndex ) const
{
	static SetScript emptyScript;
	std::map< DWORD, DWORD >::const_iterator iterSetItemKey = m_SetItemKeyIndexList.find( itemIndex );
	if( iterSetItemKey == m_SetItemKeyIndexList.end() )
	{
		return emptyScript;
	}

	const DWORD keyItemIndex = iterSetItemKey->second;
	std::map< DWORD, SetScript >::const_iterator iterSetItemInfo = m_SetItemInfoList.find( keyItemIndex );
	if( iterSetItemInfo == m_SetItemInfoList.end() )
	{
		return emptyScript;
	}

	return iterSetItemInfo->second;
}

const SetScript::Element& CLunaGameDataExtracterDlg::GetItemElement( DWORD itemIndex ) const
{
	static SetScript::Element emptyElement;

	const SetScript& setScript = GetSetItemScript( itemIndex );

	const SetScript::ItemElementMap&				itemElementMap	= setScript.mItemElementMap;
	const SetScript::ItemElementMap::const_iterator elementIterator = itemElementMap.find( itemIndex );

	return itemElementMap.end() == elementIterator ? emptyElement : elementIterator->second;
}

const ACTIVE_SKILL_INFO& CLunaGameDataExtracterDlg::GetActiveSkillInfo( DWORD index )
{
	std::map< DWORD, ACTIVE_SKILL_INFO >::const_iterator iterActiveSkillInfo = m_ActiveSkillInfoList.find( index );
	if( iterActiveSkillInfo == m_ActiveSkillInfoList.end() )
	{
		static ACTIVE_SKILL_INFO emptySkillInfo;
		return emptySkillInfo;
	}

	return iterActiveSkillInfo->second;
}

const BUFF_SKILL_INFO& CLunaGameDataExtracterDlg::GetBuffSkillInfo( DWORD index )
{
	std::map< DWORD, BUFF_SKILL_INFO >::const_iterator iterBuffSkillInfo = m_BuffSkillInfoList.find( index );
	if( iterBuffSkillInfo == m_BuffSkillInfoList.end() )
	{
		static BUFF_SKILL_INFO emptySkillInfo;
		return emptySkillInfo;
	}

	return iterBuffSkillInfo->second;
}

BOOL CLunaGameDataExtracterDlg::Extract2DItemIconImage( int progressStartPercent, int progressEndPercent, BOOL bAllExtract, DWORD startItemIdx, DWORD endItemIdx )
{
	if( m_pExecutive == NULL )
		return FALSE;

	if( endItemIdx < startItemIdx )
		return FALSE;

	I4DyuchiFileStorage* pFileStorage = NULL;
	m_pExecutive->GetFileStorage( &pFileStorage );
	if( pFileStorage == NULL )
		return FALSE;

	::SetCurrentDirectory( m_strExecutedPath );

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );
	TCHAR itemImageSaveFolderName[ MAX_PATH ]={0};
	settingScript.GetGlobalString( "ItemIconImageSaveFolder", itemImageSaveFolderName, sizeof(itemImageSaveFolderName) );

	if( _tcslen( itemImageSaveFolderName ) > 0 )
	{
		CString path;
		path.Format( "%s/%s/", m_strExecutedPath, itemImageSaveFolderName );

		::CreateDirectory( path, NULL );

		::SetCurrentDirectory( path );
	}

	DWORD dwCaptureImageFormat = CXIMAGE_FORMAT_TGA;
	TCHAR captuerFormatTxt[MAX_PATH]={0};
	settingScript.GetGlobalString( "CaptureImageFormat", captuerFormatTxt, sizeof(captuerFormatTxt) );
	if( _tcsicmp( captuerFormatTxt, "jpg" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_JPG;
	}
	else if( _tcsicmp( captuerFormatTxt, "tga" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_TGA;
	}
	else if( _tcsicmp( captuerFormatTxt, "png" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_PNG;
	}


	int cnt = 0;
	int maxCnt = (int)m_ItemInfoList.size();
	if( bAllExtract == FALSE )
	{
		maxCnt = 0;
		for( std::map< DWORD, ITEM_INFO >::const_iterator iterItemInfo = m_ItemInfoList.begin() ; iterItemInfo != m_ItemInfoList.end() ; ++iterItemInfo )
		{
			const ITEM_INFO& itemInfo = iterItemInfo->second;
			if( startItemIdx <= itemInfo.ItemIdx && itemInfo.ItemIdx <= endItemIdx )
			{
				++maxCnt;
			}
		}
	}

	CString strStaticText;
	m_ProgressCtrl.SetPos( progressStartPercent );
	strStaticText.Format( _T(" Extract 2D Item Icon Image... (0 / %d) "), maxCnt );
	m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

	std::set< DWORD > createdImagenum;
	std::map< DWORD, std::string > deleteFileList;
	const int deleteProgressPercent = 3;

	for( std::map< DWORD, ITEM_INFO >::const_iterator iterItemInfo = m_ItemInfoList.begin() ; iterItemInfo != m_ItemInfoList.end() ; ++iterItemInfo )
	{
		const ITEM_INFO& itemInfo = iterItemInfo->second;
		if( !bAllExtract )
		{
			if( itemInfo.ItemIdx < startItemIdx || itemInfo.ItemIdx > endItemIdx )
				continue;
		}

		CString strIconImgPath;
		RECT iconImgRect;
		GetItemIconInfo( itemInfo.Image2DNum, strIconImgPath, iconImgRect );

		if( strIconImgPath.GetLength() == 0 )
		{
			OutputFile( "noPathItemIcon.txt", "itemIdx=%d, name=%s, image2Dnum=%d", 
				itemInfo.ItemIdx,
				itemInfo.ItemName,
				itemInfo.Image2DNum );
			continue;
		}

        int curPos=0;
		CString strIconImgFileName;
		if( strIconImgPath.Find( "/" ) < 0 )
		{
			strIconImgFileName = strIconImgPath;
		}
		else
		{
			CString strToken = strIconImgPath.Tokenize( "/", curPos );
			while( strToken != "" )
			{
				strToken = strIconImgPath.Tokenize( "/", curPos );
				if( curPos > 0 )
					strIconImgFileName = strToken;
			}
		}
		

		const DWORD hashcode = GetHashCodeFromTxt( strIconImgFileName );
		if( deleteFileList.find( hashcode ) == deleteFileList.end() )
		{
			if( pFileStorage->ExtractFile( (LPSTR)(LPCTSTR)strIconImgFileName ) == false )
			{
				OutputFile( "noImageItemIcon.txt", "itemIdx=%d, name=%s, image2Dnum=%d, itemimagefilename=%s", 
					itemInfo.ItemIdx,
					itemInfo.ItemName,
					itemInfo.Image2DNum,
					strIconImgFileName );
				continue;
			}

			deleteFileList.insert( std::map< DWORD, std::string >::value_type( hashcode, (LPCTSTR)strIconImgFileName ) );
		}

		if( createdImagenum.find( itemInfo.Image2DNum ) == createdImagenum.end() )
		{

			CxImage sourceImage;
			if( sourceImage.Load( strIconImgFileName, CXIMAGE_FORMAT_TIF ) == true )
			{
				CxImage cropImage;
				sourceImage.Crop( iconImgRect, &cropImage );
				CString strSaveFileName;
				switch( dwCaptureImageFormat )
				{
				case CXIMAGE_FORMAT_JPG:
					{
						strSaveFileName.Format( "%s%d.JPG", settingScript.GetGlobalString( "ItemIconImagePrefix" ), itemInfo.Image2DNum );
					}
					break;
				case CXIMAGE_FORMAT_TGA:
					{
						strSaveFileName.Format( "%s%d.TGA", settingScript.GetGlobalString( "ItemIconImagePrefix" ), itemInfo.Image2DNum );
					}
					break;
				case CXIMAGE_FORMAT_PNG:
					{
						strSaveFileName.Format( "%s%d.PNG", settingScript.GetGlobalString( "ItemIconImagePrefix" ), itemInfo.Image2DNum );
					}
					break;
				}

				// ^s 제거
				ChangeLunaGameString( strSaveFileName );

				// 파일이름으로 사용하지 못하는 문자제거
				RemoveNotAvailibleFilename( strSaveFileName );
				
				if( cropImage.Save( strSaveFileName, dwCaptureImageFormat ) == false )
				{
					OutputFile( "CxImageError.txt", "Crop Image Save Error itemIdx=%d, name=%s, image2Dnum=%d, itemimagefilename=%s", 
						itemInfo.ItemIdx,
						itemInfo.ItemName,
						itemInfo.Image2DNum,
						strIconImgFileName );
				}

				createdImagenum.insert( itemInfo.Image2DNum );
			}
			else
			{
				OutputFile( "CxImageError.txt", "Source Image Load Error itemIdx=%d, name=%s, image2Dnum=%d, itemimagefilename=%s", 
						itemInfo.ItemIdx,
						itemInfo.ItemName,
						itemInfo.Image2DNum,
						strIconImgFileName );
			}
			sourceImage.Clear();
		}

		int percent = (int)((progressEndPercent - progressStartPercent) * ((float)++cnt / maxCnt)) + progressStartPercent;
		percent = max( percent - deleteProgressPercent, progressStartPercent );
		m_ProgressCtrl.SetPos( percent );
		strStaticText.Format( _T(" Extract 2D Item Icon Image... (%d / %d) "), cnt, maxCnt );
		m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );
	}

	cnt = 0;
	for( std::map< DWORD, std::string >::const_iterator iterDeleteFile = deleteFileList.begin() ; iterDeleteFile != deleteFileList.end() ; ++iterDeleteFile )
	{
		const std::string& deleteFile = iterDeleteFile->second;

		DeleteFile( deleteFile.c_str() );


		int percent = (int)(deleteProgressPercent * ((float)++cnt / deleteFileList.size())) + (progressEndPercent - progressStartPercent - deleteProgressPercent);
		m_ProgressCtrl.SetPos( percent );
		strStaticText.Format( _T(" Delete Temporary Image File... (%d / %d) "), cnt, deleteFileList.size() );
		m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );
	}

	m_ProgressCtrl.SetPos( progressEndPercent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract 2D Item Icon Image Complete.. ") );

	pFileStorage->Release();

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::Extract2DActiveSkillIconImage( int progressStartPercent, int progressEndPercent, BOOL bAllExtract, DWORD startItemIdx, DWORD endItemIdx )
{
	if( m_pExecutive == NULL )
		return FALSE;

	if( endItemIdx < startItemIdx )
		return FALSE;

	I4DyuchiFileStorage* pFileStorage = NULL;
	m_pExecutive->GetFileStorage( &pFileStorage );
	if( pFileStorage == NULL )
		return FALSE;

	::SetCurrentDirectory( m_strExecutedPath );

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );
	TCHAR activeSkillImageSaveFolderName[ MAX_PATH ]={0};
	settingScript.GetGlobalString( "ActiveSkillIconImageSaveFolder", activeSkillImageSaveFolderName, sizeof(activeSkillImageSaveFolderName) );

	if( _tcslen( activeSkillImageSaveFolderName ) > 0 )
	{
		CString path;
		path.Format( "%s/%s/", m_strExecutedPath, activeSkillImageSaveFolderName );

		::CreateDirectory( path, NULL );

		::SetCurrentDirectory( path );
	}

	DWORD dwCaptureImageFormat = CXIMAGE_FORMAT_TGA;
	TCHAR captuerFormatTxt[MAX_PATH]={0};
	settingScript.GetGlobalString( "CaptureImageFormat", captuerFormatTxt, sizeof(captuerFormatTxt) );
	if( _tcsicmp( captuerFormatTxt, "jpg" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_JPG;
	}
	else if( _tcsicmp( captuerFormatTxt, "tga" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_TGA;
	}
	else if( _tcsicmp( captuerFormatTxt, "png" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_PNG;
	}


	int cnt = 0;
	int maxCnt = (int)m_ActiveSkillInfoList.size();
	if( bAllExtract == FALSE )
	{
		maxCnt = 0;
		for( std::map< DWORD, ACTIVE_SKILL_INFO >::const_iterator iterActiveSkillInfo = m_ActiveSkillInfoList.begin() ; iterActiveSkillInfo != m_ActiveSkillInfoList.end() ; ++iterActiveSkillInfo )
		{
			const ACTIVE_SKILL_INFO& activeSkillInfo = iterActiveSkillInfo->second;
			if( startItemIdx <= activeSkillInfo.Index && activeSkillInfo.Index <= endItemIdx )
			{
				++maxCnt;
			}
		}
	}

	CString strStaticText;
	m_ProgressCtrl.SetPos( progressStartPercent );
	strStaticText.Format( _T(" Extract 2D Item Icon Image... (0 / %d) "), maxCnt );
	m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

	std::set< DWORD > createdImagenum;
	std::map< DWORD, std::string > deleteFileList;
	const int deleteProgressPercent = 3;

	for( std::map< DWORD, ACTIVE_SKILL_INFO >::const_iterator iterActiveSkillInfo = m_ActiveSkillInfoList.begin() ; iterActiveSkillInfo != m_ActiveSkillInfoList.end() ; ++iterActiveSkillInfo )
	{
		const ACTIVE_SKILL_INFO& activeSkillInfo = iterActiveSkillInfo->second;
		if( !bAllExtract )
		{
			if( activeSkillInfo.Index < startItemIdx || activeSkillInfo.Index > endItemIdx )
				continue;
		}

		CString strIconImgPath;
		RECT iconImgRect;
		GetSkillIconInfo( activeSkillInfo.Image, strIconImgPath, iconImgRect );

		if( strIconImgPath.GetLength() == 0 )
		{
			OutputFile( "noPathActiveIcon.txt", "ActiveSkill Index=%d, name=%s, image index=%d", 
				activeSkillInfo.Index,
				activeSkillInfo.Name,
				activeSkillInfo.Image );
			continue;
		}

        int curPos=0;
		CString strIconImgFileName;
		if( strIconImgPath.Find( "/" ) < 0 )
		{
			strIconImgFileName = strIconImgPath;
		}
		else
		{
			CString strToken = strIconImgPath.Tokenize( "/", curPos );
			while( strToken != "" )
			{
				strToken = strIconImgPath.Tokenize( "/", curPos );
				if( curPos > 0 )
					strIconImgFileName = strToken;
			}
		}
		

		const DWORD hashcode = GetHashCodeFromTxt( strIconImgFileName );
		if( deleteFileList.find( hashcode ) == deleteFileList.end() )
		{
			if( pFileStorage->ExtractFile( (LPSTR)(LPCTSTR)strIconImgFileName ) == false )
			{
				OutputFile( "noImageItemIcon.txt", "itemIdx=%d, name=%s, image2Dnum=%d, itemimagefilename=%s", 
					activeSkillInfo.Index,
					activeSkillInfo.Name,
					activeSkillInfo.Image,
					strIconImgFileName );
				continue;
			}

			deleteFileList.insert( std::map< DWORD, std::string >::value_type( hashcode, (LPCTSTR)strIconImgFileName ) );
		}

		if( createdImagenum.find( activeSkillInfo.Image ) == createdImagenum.end() )
		{
			CxImage sourceImage;
			if( sourceImage.Load( strIconImgFileName, CXIMAGE_FORMAT_TIF ) == true )
			{
				CxImage cropImage;
				sourceImage.Crop( iconImgRect, &cropImage );
				CString strSaveFileName;

				switch( dwCaptureImageFormat )
				{
				case CXIMAGE_FORMAT_JPG:
					{
						strSaveFileName.Format( "%s%d.JPG", settingScript.GetGlobalString( "ActiveSkillIconImagePrefix" ), activeSkillInfo.Image );
					}
					break;
				case CXIMAGE_FORMAT_TGA:
					{
						strSaveFileName.Format( "%s%d.TGA", settingScript.GetGlobalString( "ActiveSkillIconImagePrefix" ), activeSkillInfo.Image );
					}
					break;
				case CXIMAGE_FORMAT_PNG:
					{
						strSaveFileName.Format( "%s%d.PNG", settingScript.GetGlobalString( "ActiveSkillIconImagePrefix" ), activeSkillInfo.Image );
					}
					break;
				}

				// ^s 제거
				ChangeLunaGameString( strSaveFileName );

				// 파일이름으로 사용하지 못하는 문자제거
				RemoveNotAvailibleFilename( strSaveFileName );

				if( cropImage.Save( strSaveFileName, dwCaptureImageFormat ) == false )
				{
					OutputFile( "CxImageError.txt", "Crop Image Save Error activeSkillIIndex=%d, name=%s, image2Dnum=%d, activeSkillimagefilename=%s", 
						activeSkillInfo.Index,
						activeSkillInfo.Name,
						activeSkillInfo.Image,
						strIconImgFileName );
				}

				createdImagenum.insert( activeSkillInfo.Image );
			}
			else
			{
				OutputFile( "CxImageError.txt", "Source Image Load Error activeSkillIIndex=%d, name=%s, image2Dnum=%d, activeSkillimagefilename=%s", 
						activeSkillInfo.Index,
						activeSkillInfo.Name,
						activeSkillInfo.Image,
						strIconImgFileName );
			}
			sourceImage.Clear();
		}

		int percent = (int)((progressEndPercent - progressStartPercent) * ((float)++cnt / maxCnt)) + progressStartPercent;
		percent = max( percent - deleteProgressPercent, progressStartPercent );
		m_ProgressCtrl.SetPos( percent );
		strStaticText.Format( _T(" Extract 2D Active Skill Icon Image... (%d / %d) "), cnt, maxCnt );
		m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );
	}

	cnt = 0;
	for( std::map< DWORD, std::string >::const_iterator iterDeleteFile = deleteFileList.begin() ; iterDeleteFile != deleteFileList.end() ; ++iterDeleteFile )
	{
		const std::string& deleteFile = iterDeleteFile->second;

		DeleteFile( deleteFile.c_str() );


		int percent = (int)(deleteProgressPercent * ((float)++cnt / deleteFileList.size())) + (progressEndPercent - progressStartPercent - deleteProgressPercent);
		m_ProgressCtrl.SetPos( percent );
		strStaticText.Format( _T(" Delete Temporary Image File... (%d / %d) "), cnt, deleteFileList.size() );
		m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );
	}

	m_ProgressCtrl.SetPos( progressEndPercent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract 2D Active Skill Icon Image Complete.. ") );

	pFileStorage->Release();
	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::Extract2DBuffSkillIconImage( int progressStartPercent, int progressEndPercent, BOOL bAllExtract, DWORD startItemIdx, DWORD endItemIdx )
{
	if( m_pExecutive == NULL )
		return FALSE;

	if( endItemIdx < startItemIdx )
		return FALSE;

	I4DyuchiFileStorage* pFileStorage = NULL;
	m_pExecutive->GetFileStorage( &pFileStorage );
	if( pFileStorage == NULL )
		return FALSE;

	::SetCurrentDirectory( m_strExecutedPath );

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );
	TCHAR buffSkillImageSaveFolderName[ MAX_PATH ]={0};
	settingScript.GetGlobalString( "BuffSkillIconImageSaveFolder", buffSkillImageSaveFolderName, sizeof(buffSkillImageSaveFolderName) );

	if( _tcslen( buffSkillImageSaveFolderName ) > 0 )
	{
		CString path;
		path.Format( "%s/%s/", m_strExecutedPath, buffSkillImageSaveFolderName );

		::CreateDirectory( path, NULL );

		::SetCurrentDirectory( path );
	}

	DWORD dwCaptureImageFormat = CXIMAGE_FORMAT_TGA;
	TCHAR captuerFormatTxt[MAX_PATH]={0};
	settingScript.GetGlobalString( "CaptureImageFormat", captuerFormatTxt, sizeof(captuerFormatTxt) );
	if( _tcsicmp( captuerFormatTxt, "jpg" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_JPG;
	}
	else if( _tcsicmp( captuerFormatTxt, "tga" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_TGA;
	}
	else if( _tcsicmp( captuerFormatTxt, "png" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_PNG;
	}


	int cnt = 0;
	int maxCnt = (int)m_BuffSkillInfoList.size();
	if( bAllExtract == FALSE )
	{
		maxCnt = 0;
		for( std::map< DWORD, BUFF_SKILL_INFO >::const_iterator iterBuffSkillInfo = m_BuffSkillInfoList.begin() ; iterBuffSkillInfo != m_BuffSkillInfoList.end() ; ++iterBuffSkillInfo )
		{
			const BUFF_SKILL_INFO& buffSkillInfo = iterBuffSkillInfo->second;
			if( startItemIdx <= buffSkillInfo.Index && buffSkillInfo.Index <= endItemIdx )
			{
				++maxCnt;
			}
		}
	}

	CString strStaticText;
	m_ProgressCtrl.SetPos( progressStartPercent );
	strStaticText.Format( _T(" Extract 2D Item Icon Image... (0 / %d) "), maxCnt );
	m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

	std::set< DWORD > createdImagenum;
	std::map< DWORD, std::string > deleteFileList;
	const int deleteProgressPercent = 3;

	for( std::map< DWORD, BUFF_SKILL_INFO >::const_iterator iterBuffSkillInfo = m_BuffSkillInfoList.begin() ; iterBuffSkillInfo != m_BuffSkillInfoList.end() ; ++iterBuffSkillInfo )
	{
		const BUFF_SKILL_INFO& buffSkillInfo = iterBuffSkillInfo->second;
		if( !bAllExtract )
		{
			if( buffSkillInfo.Index < startItemIdx || buffSkillInfo.Index > endItemIdx )
				continue;
		}

		CString strIconImgPath;
		RECT iconImgRect;
		GetSkillIconInfo( buffSkillInfo.Image, strIconImgPath, iconImgRect );

		if( strIconImgPath.GetLength() == 0 )
		{
			OutputFile( "noPathBuffIcon.txt", "BuffSkill Index=%d, name=%s, image index=%d", 
				buffSkillInfo.Index,
				buffSkillInfo.Name,
				buffSkillInfo.Image );
			continue;
		}

        int curPos=0;
		CString strIconImgFileName;
		if( strIconImgPath.Find( "/" ) < 0 )
		{
			strIconImgFileName = strIconImgPath;
		}
		else
		{
			CString strToken = strIconImgPath.Tokenize( "/", curPos );
			while( strToken != "" )
			{
				strToken = strIconImgPath.Tokenize( "/", curPos );
				if( curPos > 0 )
					strIconImgFileName = strToken;
			}
		}
		

		const DWORD hashcode = GetHashCodeFromTxt( strIconImgFileName );
		if( deleteFileList.find( hashcode ) == deleteFileList.end() )
		{
			if( pFileStorage->ExtractFile( (LPSTR)(LPCTSTR)strIconImgFileName ) == false )
			{
				OutputFile( "noImageItemIcon.txt", "itemIdx=%d, name=%s, image2Dnum=%d, itemimagefilename=%s", 
					buffSkillInfo.Index,
					buffSkillInfo.Name,
					buffSkillInfo.Image,
					strIconImgFileName );
				continue;
			}

			deleteFileList.insert( std::map< DWORD, std::string >::value_type( hashcode, (LPCTSTR)strIconImgFileName ) );
		}

		if( createdImagenum.find( buffSkillInfo.Image ) == createdImagenum.end() )
		{
			CxImage sourceImage;
			if( sourceImage.Load( strIconImgFileName, CXIMAGE_FORMAT_TIF ) == true )
			{
				CxImage cropImage;
				sourceImage.Crop( iconImgRect, &cropImage );
				CString strSaveFileName;

				switch( dwCaptureImageFormat )
				{
				case CXIMAGE_FORMAT_JPG:
					{
						strSaveFileName.Format( "%s%d.JPG", settingScript.GetGlobalString( "BuffSkillIconImagePrefix" ), buffSkillInfo.Image );
					}
					break;
				case CXIMAGE_FORMAT_TGA:
					{
						strSaveFileName.Format( "%s%d.TGA", settingScript.GetGlobalString( "BuffSkillIconImagePrefix" ), buffSkillInfo.Image );
					}
					break;
				case CXIMAGE_FORMAT_PNG:
					{
						strSaveFileName.Format( "%s%d.PNG", settingScript.GetGlobalString( "BuffSkillIconImagePrefix" ), buffSkillInfo.Image );
					}
					break;
				}

				// ^s 제거
				ChangeLunaGameString( strSaveFileName );

				// 파일이름으로 사용하지 못하는 문자제거
				RemoveNotAvailibleFilename( strSaveFileName );
				
				if( cropImage.Save( strSaveFileName, dwCaptureImageFormat ) == false )
				{
					OutputFile( "CxImageError.txt", "Crop Image Save Error buffSkillInfoIndex=%d, name=%s, image2Dnum=%d, buffSkillInfoimagefilename=%s", 
						buffSkillInfo.Index,
						buffSkillInfo.Name,
						buffSkillInfo.Image,
						strIconImgFileName );
				}

				createdImagenum.insert( buffSkillInfo.Image );
			}
			else
			{
				OutputFile( "CxImageError.txt", "Source Image Load Error buffSkillInfoIndex=%d, name=%s, image2Dnum=%d, buffSkillInfoimagefilename=%s",
						buffSkillInfo.Index,
						buffSkillInfo.Name,
						buffSkillInfo.Image,
						strIconImgFileName );
			}
			sourceImage.Clear();
		}

		int percent = (int)((progressEndPercent - progressStartPercent) * ((float)++cnt / maxCnt)) + progressStartPercent;
		percent = max( percent - deleteProgressPercent, progressStartPercent );
		m_ProgressCtrl.SetPos( percent );
		strStaticText.Format( _T(" Extract 2D Buff Skill Icon Image... (%d / %d) "), cnt, maxCnt );
		m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );
	}

	cnt = 0;
	for( std::map< DWORD, std::string >::const_iterator iterDeleteFile = deleteFileList.begin() ; iterDeleteFile != deleteFileList.end() ; ++iterDeleteFile )
	{
		const std::string& deleteFile = iterDeleteFile->second;

		DeleteFile( deleteFile.c_str() );


		int percent = (int)(deleteProgressPercent * ((float)++cnt / deleteFileList.size())) + (progressEndPercent - progressStartPercent - deleteProgressPercent);
		m_ProgressCtrl.SetPos( percent );
		strStaticText.Format( _T(" Delete Temporary Image File... (%d / %d) "), cnt, deleteFileList.size() );
		m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );
	}

	m_ProgressCtrl.SetPos( progressEndPercent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract 2D Buff Skill Icon Image Complete.. ") );

	pFileStorage->Release();
	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::Extract3DMonsterImage( int progressStartPercent, int progressEndPercent, BOOL bAllExtract, DWORD startItemIdx, DWORD endItemIdx )
{
	::SetCurrentDirectory( m_strExecutedPath );

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );
	TCHAR monsterImageSaveFolderName[ MAX_PATH ]={0};
	settingScript.GetGlobalString( "MonsterImageSaveFolder", monsterImageSaveFolderName, sizeof(monsterImageSaveFolderName) );

	if( _tcslen( monsterImageSaveFolderName ) > 0 )
	{
		CString path;
		path.Format( "%s/%s/", m_strExecutedPath, monsterImageSaveFolderName );

		::CreateDirectory( path, NULL );

		::SetCurrentDirectory( path );
	}

	BOOL bIsResampling = FALSE;
	CSize resampleSize;
	if( settingScript.GetGlobalBoolean( "CaptureImageResampling" ) )
	{
		bIsResampling = TRUE;
		resampleSize.cx = (int)(settingScript.GetGlobalTableFieldNumber( "CaptureImageResamplingSize", "width" ));
		resampleSize.cy = (int)(settingScript.GetGlobalTableFieldNumber( "CaptureImageResamplingSize", "height" ));
	}

	

	DWORD dwCaptureImageFormat = CXIMAGE_FORMAT_TGA;
	TCHAR captuerFormatTxt[MAX_PATH]={0};
	settingScript.GetGlobalString( "CaptureImageFormat", captuerFormatTxt, sizeof(captuerFormatTxt) );
	if( _tcsicmp( captuerFormatTxt, "jpg" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_JPG;
	}
	else if( _tcsicmp( captuerFormatTxt, "tga" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_TGA;
	}
	else if( _tcsicmp( captuerFormatTxt, "png" ) == 0 )
	{
		dwCaptureImageFormat = CXIMAGE_FORMAT_PNG;
	}

	const char* capturefilename = "capture.tga";
	CString strStaticText;
	int cnt = 0;
	int maxCnt = 0;
	std::set< DWORD > createCHXObject;
	for( std::map< DWORD, BASE_MONSTER_LIST >::const_iterator iterMonsterInfo = m_MonsterInfoList.begin() ; iterMonsterInfo != m_MonsterInfoList.end() ; ++iterMonsterInfo )
	{
		const BASE_MONSTER_LIST& monsterInfo = iterMonsterInfo->second;
		const DWORD hashcode = GetHashCodeFromTxt( monsterInfo.ChxName );

		if( createCHXObject.find( hashcode ) != createCHXObject.end() )
		{
			continue;
		}
		else if( bAllExtract == FALSE &&
			(monsterInfo.MonsterKind < startItemIdx || monsterInfo.MonsterKind > endItemIdx) )
		{
			continue;
		}
		++maxCnt;
		createCHXObject.insert( hashcode );
	}
	createCHXObject.clear();

	for( std::map< DWORD, BASE_MONSTER_LIST >::const_iterator iterMonsterInfo = m_MonsterInfoList.begin() ; iterMonsterInfo != m_MonsterInfoList.end() ; ++iterMonsterInfo )
	{
		const BASE_MONSTER_LIST& monsterInfo = iterMonsterInfo->second;
		const DWORD hashcode = GetHashCodeFromTxt( monsterInfo.ChxName );

		if( createCHXObject.find( hashcode ) != createCHXObject.end() )
		{
			continue;
		}
		else if( bAllExtract == FALSE &&
			(monsterInfo.MonsterKind < startItemIdx || monsterInfo.MonsterKind > endItemIdx) )
		{
			continue;
		}

		CString strMonsterChxName(monsterInfo.ChxName);
		InsertGXObject( (LPTSTR)(LPCTSTR)strMonsterChxName, monsterInfo.Scale );

		BYTE backgroundR = (BYTE)settingScript.GetGlobalTableFieldNumber( "BackgroundColor", "r" );
		BYTE backgroundG = (BYTE)settingScript.GetGlobalTableFieldNumber( "BackgroundColor", "g" );
		BYTE backgroundB = (BYTE)settingScript.GetGlobalTableFieldNumber( "BackgroundColor", "b" );
		const DWORD dwBackGroundColor = RGBA_MAKE( backgroundR, backgroundG, backgroundB, 255 );
		m_pExecutive->Run( dwBackGroundColor, 0, 0, 0 );
		m_pExecutive->GetGeometry()->Present(0);

		if( m_pExecutive->GetRenderer()->CaptureScreen( (char*)capturefilename ) == false )
		{
			AfxMessageBox( "Capture failed.." );
			return FALSE;
		}

		CxImage captureImage;
		if( captureImage.Load( capturefilename, CXIMAGE_FORMAT_TGA ) == true )
		{
			CxImage cropImage;
			captureImage.Crop( 0, 0, captureImage.GetWidth(), captureImage.GetHeight(), &cropImage );
			CString strChxFileName;
			CString strSaveFileName;

			strChxFileName = monsterInfo.ChxName;
			strChxFileName = strChxFileName.MakeUpper();
			strChxFileName.Replace( ".CHX", "" );
			strChxFileName.Replace( ".CHR", "" );
			strChxFileName.Replace( ".MOD", "" );

			switch( dwCaptureImageFormat )
			{
			case CXIMAGE_FORMAT_JPG:
				{
					strSaveFileName.Format( "%s%s.JPG", settingScript.GetGlobalString( "MonsterImagePrefix" ), strChxFileName );
				}
				break;
			case CXIMAGE_FORMAT_TGA:
				{
					strSaveFileName.Format( "%s%s.TGA", settingScript.GetGlobalString( "MonsterImagePrefix" ), strChxFileName );
				}
				break;
			case CXIMAGE_FORMAT_PNG:
				{
					strSaveFileName.Format( "%s%s.PNG", settingScript.GetGlobalString( "MonsterImagePrefix" ), strChxFileName );
				}
				break;
			}

			
			if( bIsResampling )
			{
				cropImage.Resample( resampleSize.cx, resampleSize.cy );
			}

			// 파일이름으로 사용하지 못하는 문자제거
			RemoveNotAvailibleFilename( strSaveFileName );
			
			if( cropImage.Save( strSaveFileName, dwCaptureImageFormat ) == false )
			{
				OutputFile( "CxImageError.txt", "Crop Image Save Error MonsterKind=%d, name=%s",
					monsterInfo.MonsterKind,
					monsterInfo.Name );
			}
		}
		else
		{
			OutputFile( "CxImageError.txt", "Capture Image Load Error MonsterKind=%d, name=%s",
					monsterInfo.MonsterKind,
					monsterInfo.Name );
		}
		captureImage.Clear();

		createCHXObject.insert( hashcode );

		int percent = (int)((progressEndPercent - progressStartPercent) * ((float)++cnt / maxCnt)) + progressStartPercent;
		m_ProgressCtrl.SetPos( percent );
		strStaticText.Format( _T(" Extract 3D Monster Image... (%d / %d) "), cnt, maxCnt );
		m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );
	}

	::DeleteFile( capturefilename );

	m_ProgressCtrl.SetPos( progressEndPercent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract 3D Monster Image Complete.. ") );

	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::ExtractItemData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	::SetCurrentDirectory( m_strExecutedPath );

	TCHAR extractDataInfoSaveFolderName[ MAX_PATH ]={0};
	settingScript.GetGlobalString( "ExtractDataInfoSaveFolder", extractDataInfoSaveFolderName, sizeof(extractDataInfoSaveFolderName) );

	if( _tcslen( extractDataInfoSaveFolderName ) > 0 )
	{
		CString path;
		path.Format( "%s/%s/", m_strExecutedPath, extractDataInfoSaveFolderName );

		::CreateDirectory( path, NULL );

		::SetCurrentDirectory( path );
	}

	const float totalStepCnt = 5;
	int step = 0;
	int startPercent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	int endPercent		= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	MakeItemWeaponData( startPercent, endPercent, bOutText, bOutExcel );

	startPercent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	endPercent		= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	MakeItemArmorData( startPercent, endPercent, bOutText, bOutExcel );

	startPercent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	endPercent		= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	MakeItemAccessaryData( startPercent, endPercent, bOutText, bOutExcel );

	startPercent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	endPercent		= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	MakeItemEtcData( startPercent, endPercent, bOutText, bOutExcel );

	startPercent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	endPercent		= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	MakeItemSetData( startPercent, endPercent, bOutText, bOutExcel );

	return TRUE;
}

LPCTSTR CLunaGameDataExtracterDlg::GetItemDetailTypeText( const ITEM_INFO& itemInfo )
{
	static TCHAR buf[MAX_PATH];
	ZeroMemory( buf, sizeof(buf) );

	switch( itemInfo.dwTypeDetail )
	{
	// Weapon Type
		// 한손 검
	case 11101:		_stprintf( buf, _T("%s %s"), GetSystemMsg( 658 ), GetSystemMsg( 660 ) );		break;
		// 한손 둔기
	case 11102:		_stprintf( buf, _T("%s %s"), GetSystemMsg( 658 ), GetSystemMsg( 662 ) );		break;
		// 한손 도끼
	case 11103:		_stprintf( buf, _T("%s %s"), GetSystemMsg( 658 ), GetSystemMsg( 661 ) );		break;
		// 한손 지팡이
	case 11104:		_stprintf( buf, _T("%s %s"), GetSystemMsg( 658 ), GetSystemMsg( 665 ) );		break;
		// 단검
	case 11105:		_stprintf( buf, _T("%s %s"), GetSystemMsg( 658 ), GetSystemMsg( 663 ) );		break;

		// 양손 검
	case 11201:		_stprintf( buf, _T("%s %s"), GetSystemMsg( 659 ), GetSystemMsg( 660 ) );		break;
		// 양손 둔기
	case 11202:		_stprintf( buf, _T("%s %s"), GetSystemMsg( 659 ), GetSystemMsg( 662 ) );		break;
		// 양손 도끼
	case 11203:		_stprintf( buf, _T("%s %s"), GetSystemMsg( 659 ), GetSystemMsg( 661 ) );		break;
		// 양손 지팡이
	case 11204:		_stprintf( buf, _T("%s %s"), GetSystemMsg( 659 ), GetSystemMsg( 665 ) );		break;
		// 활
	case 11205:		_stprintf( buf, _T("%s"), GetSystemMsg( 664 ) );			break;
		// 머스켓
	case 11206:		_stprintf( buf, _T("%s"), GetSystemMsg( 2048 ) );		break;
		// 쌍검
	case 11207:		_stprintf( buf, _T("%s"), GetSystemMsg( 1924 ) );		break;

	// Armor Type
		// 로브 - 갑옷
	case 12101:		_stprintf( buf, _T("%s - %s"), GetSystemMsg( 692 ), GetSystemMsg( 683 ) );		break;
		// 로브 - 장갑
	case 12102:		_stprintf( buf, _T("%s - %s"), GetSystemMsg( 692 ), GetSystemMsg( 685 ) );		break;
		// 로브 - 신발
	case 12103:		_stprintf( buf, _T("%s - %s"), GetSystemMsg( 692 ), GetSystemMsg( 686 ) );		break;

		// 라이트 - 갑옷
	case 12201:		_stprintf( buf, _T("%s - %s"), GetSystemMsg( 693 ), GetSystemMsg( 683 ) );		break;
		// 라이트 - 장갑
	case 12202:		_stprintf( buf, _T("%s - %s"), GetSystemMsg( 693 ), GetSystemMsg( 685 ) );		break;
		// 라이트 - 신발
	case 12203:		_stprintf( buf, _T("%s - %s"), GetSystemMsg( 693 ), GetSystemMsg( 686 ) );		break;

		// 헤비 - 갑옷
	case 12301:		_stprintf( buf, _T("%s - %s"), GetSystemMsg( 694 ), GetSystemMsg( 683 ) );		break;
		// 헤비 - 장갑
	case 12302:		_stprintf( buf, _T("%s - %s"), GetSystemMsg( 694 ), GetSystemMsg( 685 ) );		break;
		// 헤비 - 신발
	case 12303:		_stprintf( buf, _T("%s - %s"), GetSystemMsg( 694 ), GetSystemMsg( 686 ) );		break;

		// 투구
	case 12401:		_stprintf( buf, _T("%s"), GetSystemMsg( 684 ) );		break;
		// 방패
	case 12402:		_stprintf( buf, _T("%s"), GetSystemMsg( 695 ) );		break;

	// Accessary Type
		// 머리장식
	case 14101:		_stprintf( buf, _T("%s"), GetSystemMsg( 691 ) );		break;
		// 안경
	case 14102:		_stprintf( buf, _T("%s"), GetSystemMsg( 832 ) );		break;
		// 아티팩트
	case 14103:		_stprintf( buf, _T("%s"), "아티팩트" );		break;
		// 벨트
	case 14104:		_stprintf( buf, _T("%s"), GetSystemMsg( 690 ) );		break;
		// 날개
	case 14105:		_stprintf( buf, _T("%s"), GetSystemMsg( 834 ) );		break;
		// 망토
	case 14106:		_stprintf( buf, _T("%s"), GetSystemMsg( 834 ) );		break;

	default:		ZeroMemory( buf, sizeof(buf) );	break;
	}

	return buf;
}

LPCTSTR CLunaGameDataExtracterDlg::GetItemGradeText( const ITEM_INFO& itemInfo )
{
	static TCHAR buf[MAX_PATH];
	ZeroMemory( buf, sizeof(buf) );

	switch( itemInfo.kind )
	{
		// 일반
	case 0:			_stprintf( buf, _T("%s"), GetInterfaceMsg( 1427 ) );		break;
		// 고급
	case 1:			_stprintf( buf, _T("%s"), GetInterfaceMsg( 1428 ) );		break;
		// 레어
	case 2:			_stprintf( buf, _T("%s"), GetInterfaceMsg( 1429 ) );		break;
		// 유니크
	case 3:			_stprintf( buf, _T("%s"), GetInterfaceMsg( 1430 ) );		break;
		// 전설
	case 4:			_stprintf( buf, _T("%s"), GetInterfaceMsg( 1431 ) );		break;
	default:		ZeroMemory( buf, sizeof(buf) );	break;
	}

	return buf;
}

std::string CLunaGameDataExtracterDlg::GetItemOptionText( const ITEM_INFO& itemInfo )
{
	std::string option;
	TCHAR buf[MAX_PATH]={0};
	BOOL bIsFirst = TRUE;

	if( itemInfo.ImprovementStr > 0 )
	{
		if( !bIsFirst )
			option.append( ", " );

		_stprintf( buf, _T("%s %+d"), GetSystemMsg( 676 ), itemInfo.ImprovementStr );
		option.append( buf );
		bIsFirst = FALSE;
	}

	if( itemInfo.ImprovementDex > 0 )
	{
		if( !bIsFirst )
			option.append( ", " );

		_stprintf( buf, _T("%s %+d"), GetSystemMsg( 677 ), itemInfo.ImprovementDex );
		option.append( buf );
		bIsFirst = FALSE;
	}

	if( itemInfo.ImprovementVit > 0 )
	{
		if( !bIsFirst )
			option.append( ", " );

		_stprintf( buf, _T("%s %+d"), GetSystemMsg( 678 ), itemInfo.ImprovementVit );
		option.append( buf );
		bIsFirst = FALSE;
	}

	if( itemInfo.ImprovementInt > 0 )
	{
		if( !bIsFirst )
			option.append( ", " );

		_stprintf( buf, _T("%s %+d"), GetSystemMsg( 679 ), itemInfo.ImprovementInt );
		option.append( buf );
	}

	if( itemInfo.ImprovementWis > 0 )
	{
		if( !bIsFirst )
			option.append( ", " );

		_stprintf( buf, _T("%s %+d"), GetSystemMsg( 680 ), itemInfo.ImprovementWis );
		option.append( buf );
		bIsFirst = FALSE;
	}

	if( itemInfo.ImprovementLife > 0 )
	{
		if( !bIsFirst )
			option.append( ", " );

		_stprintf( buf, _T("%s %+d"), GetSystemMsg( 681 ), itemInfo.ImprovementLife );
		option.append( buf );
		bIsFirst = FALSE;
	}

	if( itemInfo.ImprovementMana > 0 )
	{
		if( !bIsFirst )
			option.append( ", " );

		_stprintf( buf, _T("%s %+d"), GetSystemMsg( 682 ), itemInfo.ImprovementMana );
		option.append( buf );
		bIsFirst = FALSE;
	}

	class {
	public:
		void operator() ( const char* optionName, const PlayerStat::Value& baseValue, std::string& option, BOOL& bIsFirst )
		{
			TCHAR buf[MAX_PATH]={0};

			if( baseValue.mPlus )
			{
				if( !bIsFirst )
					option.append( ", " );

				_stprintf( buf, _T("%s %+d"), optionName, baseValue.mPlus );
				option.append( buf );
				bIsFirst = FALSE;
			}

			if( baseValue.mPercent )
			{
				if( !bIsFirst )
					option.append( ", " );

				_stprintf( buf, _T("%s %+0.f%%"), optionName, baseValue.mPercent * 100 );
				option.append( buf );
				bIsFirst = FALSE;
			}
		}
	}AddOptionTooltip;


	// 081231 LUJ, 아이템 추가 능력이 설정된 경우 표시하기 위해 더해준다
	{
		const SetScript::Element&	itemElement = GetItemElement( itemInfo.ItemIdx );
		const PlayerStat&			itemStat	= itemElement.mStat;

		// 물공
		AddOptionTooltip( GetSystemMsg( 671 ), itemStat.mPhysicAttack, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 673 ), itemStat.mPhysicDefense, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 672 ), itemStat.mMagicAttack, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 674 ), itemStat.mMagicDefense, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 676 ), itemStat.mStrength, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 677 ), itemStat.mDexterity, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 678 ), itemStat.mVitality, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 679 ), itemStat.mIntelligence, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 680 ), itemStat.mWisdom, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 681 ), itemStat.mLife, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 682 ), itemStat.mMana, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 846 ), itemStat.mRecoveryLife, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 847 ), itemStat.mRecoveryMana, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 840 ), itemStat.mMoveSpeed, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 841 ), itemStat.mEvade, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 162 ), itemStat.mAccuracy, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 842 ), itemStat.mCriticalRate, option, bIsFirst );
		AddOptionTooltip( GetSystemMsg( 843 ), itemStat.mCriticalDamage, option, bIsFirst );

		// 081231 LUJ, 스킬을 표시한다
		const SetScript::Element::Skill& skillMap = itemElement.mSkill;
		for( SetScript::Element::Skill::const_iterator skill_it = skillMap.begin() ; skillMap.end() != skill_it ; ++skill_it )
		{
			const BYTE			skillLevel	= skill_it->second;
			const DWORD			skillIndex	= skill_it->first / 100 * 100 + skillLevel;	
			const ACTIVE_SKILL_INFO&	activeSkillInfo	= GetActiveSkillInfo( skillIndex );

			if( activeSkillInfo.Index != skillIndex )
			{
				continue;
			}
			else if( activeSkillInfo.Level )
			{
				if( !bIsFirst )
					option.append( ", " );

				_stprintf( buf, _T("[%s] %s %+d "), activeSkillInfo.Name, GetInterfaceMsg( 636 ), activeSkillInfo.Level );
				option.append( buf );

				bIsFirst = FALSE;
			}
			else
			{
				if( !bIsFirst )
					option.append( ", " );

				_stprintf( buf, _T("[%s] "), activeSkillInfo.Name );
				option.append( buf );

				bIsFirst = FALSE;
			}
		}
	}

	return option;
}

std::string CLunaGameDataExtracterDlg::GetItemEtcInfoText( const ITEM_INFO& itemInfo )
{
	std::string option;
	TCHAR buf[MAX_PATH]={0};
	BOOL bIsFirst = TRUE;

	// Add 성별 제한
	if( itemInfo.LimitGender > 0 )
	{
		if( !bIsFirst )
			option.append( " / " );

		switch( itemInfo.LimitGender )
		{
		case 1:		option.append( GetSystemMsg( 666 ) );		break;
		case 2:		option.append( GetSystemMsg( 667 ) );		break;
		}

		bIsFirst = FALSE;
	}

	// Add 종족 제한
	{
		if( !bIsFirst )
			option.append( " / " );

		switch( itemInfo.LimitRace )
		{
		case 0:		option.append( GetSystemMsg( 1927 ) );		break;
		case 1:		option.append( GetSystemMsg( 668 ) );		break;
		case 2:		option.append( GetSystemMsg( 669 ) );		break;
		}
		bIsFirst = FALSE;
	}

	// Add Set Item
	if( m_SetItemKeyIndexList.find( itemInfo.ItemIdx ) != m_SetItemKeyIndexList.end() )
	{
		if( !bIsFirst )
			option.append( " / " );

		// Need Msg Index
		option.append( "세트아이템" );

		bIsFirst = FALSE;
	}

	return option;
}

void CLunaGameDataExtracterDlg::GetItemSetOptionText( const SetScript& script, std::string& setOptText )
{
	const BOOL bHasAbility = script.mAbility.size() > 0;
	if( !bHasAbility )
		return;

	TCHAR buf[MAX_PATH]={0};

	// 어빌리티 이름
	_stprintf( buf, _T("[%s]\n"), script.mName.c_str() );
	setOptText.append( buf );

	// 셋트 필요아이템목록
	if( script.mSlotNameMap.empty() )
	{
		for( SetScript::Item::const_iterator iterSetItem = script.mItem.begin() ; iterSetItem != script.mItem.end() ; ++iterSetItem )
		{
			const DWORD setItemIndex = *iterSetItem;
			const ITEM_INFO& itemInfo = GetItemInfo( setItemIndex );

			setOptText.append( itemInfo.ItemName );
			setOptText.append( _T("\n") );
		}
	}
	else
	{
		// 슬롯이름으로 지정된 경우
		for( SetScript::SlotNameMap::const_iterator iterSlotName = script.mSlotNameMap.begin() ; iterSlotName != script.mSlotNameMap.end() ; ++iterSlotName )
		{
			const std::string& slotname = iterSlotName->second;

			setOptText.append( slotname );
			setOptText.append( _T("\n") );
		}
	}

	/// 세트효과 글자 만큼 태핑할 수 있도록 공백 문자열을 만든다
	std::string tabString = "\n";
	char buffer[MAX_PATH]={0};
	char line[MAX_PATH]={0};

	// 메시지 [0]세트 효과
	for( size_t tabSize = strlen( GetInterfaceMsg( 632 ) ) ; tabSize-- ; )
	{
		tabString += " ";
	}

	// 셋트 효과
	for( SetScript::Ability::const_iterator it = script.mAbility.begin() ; script.mAbility.end() != it ; ++it )
	{
		{
			const int					size	= it->first;
			const SetScript::Element&	element = it->second;
			
			//sprintf( buffer, "[%d]세트효과: ", size );
			_stprintf( buffer, GetInterfaceMsg( 633 ), size );
			const char* prefix = buffer;

			setOptText.append( _T("\n") );

			// 물리 공격
			{
				const PlayerStat::Value& value = element.mStat.mPhysicAttack;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 671 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 671 ), value.mPlus );
					_tcscat( line, " " );	// 파싱 버그일까? 웬일인지 위의 sprintf에서 뒤의 여백은 무시가 된다...
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}
			
			// 마법 공격
			{
				const PlayerStat::Value& value = element.mStat.mMagicAttack;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 672 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 672 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 물리 방어
			{
				const PlayerStat::Value& value = element.mStat.mPhysicDefense;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 673 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f "), prefix, GetSystemMsg( 673 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 마법 방어
			{
				const PlayerStat::Value& value = element.mStat.mMagicDefense;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 674 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 674 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 힘
			{
				const PlayerStat::Value& value = element.mStat.mStrength;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 676 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 676 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 민첩
			{
				const PlayerStat::Value& value = element.mStat.mDexterity;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 677 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 677 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 생명
			{
				const PlayerStat::Value& value = element.mStat.mVitality;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 678 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 678 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 지능
			{
				const PlayerStat::Value& value = element.mStat.mIntelligence;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 679 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 679 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 지혜
			{
				const PlayerStat::Value& value = element.mStat.mWisdom;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 680 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 680 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 생명
			{
				const PlayerStat::Value& value = element.mStat.mLife;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 681 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 681 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 마나
			{
				const PlayerStat::Value& value = element.mStat.mMana;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 682 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 682 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 생명 회복
			{
				const PlayerStat::Value& value = element.mStat.mRecoveryLife;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetInterfaceMsg( 634 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetInterfaceMsg( 634 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 마나 회복
			{
				const PlayerStat::Value& value = element.mStat.mRecoveryMana;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetInterfaceMsg( 635 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetInterfaceMsg( 635 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 명중
			{
				const PlayerStat::Value& value = element.mStat.mAccuracy;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetInterfaceMsg( 282 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetInterfaceMsg( 282 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 회피
			{
				const PlayerStat::Value& value = element.mStat.mEvade;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetInterfaceMsg( 281 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetInterfaceMsg( 281 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 크리티컬 확률
			{
				const PlayerStat::Value& value = element.mStat.mCriticalRate;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 842 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 842 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 크리티컬 피해
			{
				const PlayerStat::Value& value = element.mStat.mCriticalDamage;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 843 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 843 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			// 080226 LUJ, 이동 속도
			{
				const PlayerStat::Value& value = element.mStat.mMoveSpeed;

				if( value.mPercent )
				{
					_stprintf( line, _T("%s%s %+.0f%% "), prefix, GetSystemMsg( 840 ), value.mPercent * 100 );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					_stprintf( line, _T("%s%s %+.0f%"), prefix, GetSystemMsg( 840 ), value.mPlus );
					strcat( line, " " );
					setOptText.append( line );

					prefix = tabString.c_str();
				}
			}

			for(
				SetScript::Element::Skill::const_iterator it = element.mSkill.begin();
				element.mSkill.end() != it;
				++it )
			{
				const short	level	= it->second;
				const DWORD	index	= it->first / 100 * 100 + level;
				const ACTIVE_SKILL_INFO& activeSkillInfo = GetActiveSkillInfo( index );
				
				if( activeSkillInfo.Index != index )
				{
					continue;
				}
				else if( activeSkillInfo.Level )
				{
					_stprintf( line, _T("%s[%s] %s %+d "), prefix, activeSkillInfo.Name, GetInterfaceMsg( 636 ), activeSkillInfo.Level );
				}
				else
				{
					_stprintf( line, _T("%s[%s] "), prefix, activeSkillInfo.Name );
				}

				setOptText.append( line );
				
				prefix = tabString.c_str();
			}
		}
	}

}

void CLunaGameDataExtracterDlg::MakeItemWeaponData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	// Weapon Type에 대한 Data 분류
	typedef std::set< DWORD > ITEMINDEXLIST;
	std::map< DWORD, ITEMINDEXLIST > mapWeaponList;

	const float totalStepCnt = 3;
	int step = 0;
	int percent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Sort Weapon Data.. ") );

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	int totalDataCnt = 0;
	for( std::map< DWORD, ITEM_INFO >::const_iterator iterItemInfo = m_ItemInfoList.begin() ; iterItemInfo != m_ItemInfoList.end() ; ++iterItemInfo )
	{
		const ITEM_INFO& itemInfo = iterItemInfo->second;

		// 11. 캐릭터 무기
		if( itemInfo.dwType != 11 )
			continue;

		// DetailType 정렬
		ITEMINDEXLIST& itemIndexList = mapWeaponList[ itemInfo.dwTypeDetail ];
		itemIndexList.insert( itemInfo.ItemIdx );
		++totalDataCnt;
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Sort Weapon Data Complete.. ") );

	CSimpleTextListCtrl listCtrl;
	int row = 1;
	int totalCol = 0;
	TCHAR buf[1024]={0};

	listCtrl.SetItemText( 0, totalCol, _T("Item Index") );					// Item Index
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 1188 ) );			// 아이템 명
	listCtrl.SetItemText( 0, ++totalCol, _T("Image Path") );				// Image Path
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 145 ) );			// 아이템 종류
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 421 ) );			// 레벨
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 865 ) );			// 등급
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 271 ) );			// 물리공격력
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 274 ) );			// 마법공격력
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 852 ) );			// 옵션부여
	listCtrl.SetItemText( 0, ++totalCol, GetSystemMsg( 48 ) );				// 기타

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Make Weapon Data.. ") );

	for( std::map< DWORD, ITEMINDEXLIST >::const_iterator iterWeaponList = mapWeaponList.begin() ; iterWeaponList != mapWeaponList.end() ; ++iterWeaponList)
	{
		const ITEMINDEXLIST& itemIndexList = iterWeaponList->second;
		for( ITEMINDEXLIST::const_iterator iterItemIndex = itemIndexList.begin() ; iterItemIndex != itemIndexList.end() ; ++iterItemIndex, ++row )
		{
			const DWORD itemIndex = *iterItemIndex;
			const ITEM_INFO& itemInfo = GetItemInfo( itemIndex );

			int col = 0;

			// Add Item Idx
			_stprintf( buf, _T("%d"), itemInfo.ItemIdx );
			listCtrl.SetItemText( row, col, buf );

			// Add Item Name
			listCtrl.SetItemText( row, ++col, itemInfo.ItemName );

			// Add Item Image Name
			_stprintf( buf, _T("%s%d.%s"), 
				settingScript.GetGlobalStringWithSTLString( "ItemIconImagePrefix" ).c_str(), 
				itemInfo.Image2DNum, 
				settingScript.GetGlobalStringWithSTLString( "CaptureImageFormat" ).c_str() );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Weapon Type
			listCtrl.SetItemText( row, ++col, GetItemDetailTypeText( itemInfo ) );

			// Add Item Level
			_stprintf( buf, _T("%d"), itemInfo.LimitLevel );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Item Grade
			listCtrl.SetItemText( row, ++col, GetItemGradeText( itemInfo ) );

			// Add 물공
			_stprintf( buf, _T("%d"), itemInfo.PhysicAttack );
			listCtrl.SetItemText( row, ++col, buf );

			// Add 마공
			_stprintf( buf, _T("%d"), itemInfo.MagicAttack );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Option
			const std::string& optionTxt = GetItemOptionText( itemInfo );
			listCtrl.SetItemText( row, ++col, optionTxt.c_str() );

			// Add Etc
			const std::string& etcTxt = GetItemEtcInfoText( itemInfo );
			listCtrl.SetItemText( row, ++col, etcTxt.c_str() );


			CString strStaticText;
			strStaticText.Format( _T(" Extract Item Data - Make Weapon Data(%d/%d).. "), row, totalDataCnt );
			m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

			int makeProcessPercent	= (int)(percent + (((progressEndPercent - progressStartPercent) / totalStepCnt) * (row / (float)totalDataCnt)));
			m_ProgressCtrl.SetPos( makeProcessPercent );
		}
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Save Weapon Data.. ") );

	CString path = m_strExecutedPath + _T("/") + settingScript.GetGlobalString( "ExtractDataInfoSaveFolder" ) + _T("/ItemWeaponInfo");

	if( bOutText )
		SaveToText( path, listCtrl );

	if( bOutExcel )
		SaveToExcel( path, listCtrl );

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Delete Temporary Weapon Data.. ") );
}

void CLunaGameDataExtracterDlg::MakeItemArmorData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	// Armor Type에 대한 Data 분류
	typedef std::set< DWORD > ITEMINDEXLIST;
	std::map< DWORD, ITEMINDEXLIST > mapArmorList;

	const float totalStepCnt = 3;
	int step = 0;
	int percent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Sort Armor Data.. ") );

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	int totalDataCnt = 0;
	for( std::map< DWORD, ITEM_INFO >::const_iterator iterItemInfo = m_ItemInfoList.begin() ; iterItemInfo != m_ItemInfoList.end() ; ++iterItemInfo )
	{
		const ITEM_INFO& itemInfo = iterItemInfo->second;

		// 12. 캐릭터 방어구
		if( itemInfo.dwType != 12 )
			continue;

		// DetailType 정렬
		ITEMINDEXLIST& itemIndexList = mapArmorList[ itemInfo.dwTypeDetail ];
		itemIndexList.insert( itemInfo.ItemIdx );
		++totalDataCnt;
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Sort Armor Data Complete.. ") );

	CSimpleTextListCtrl listCtrl;
	int row = 1;
	int totalCol = 0;
	TCHAR buf[1024]={0};

	listCtrl.SetItemText( 0, totalCol, _T("Item Index") );					// Item Index
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 1188 ) );			// 아이템 명
	listCtrl.SetItemText( 0, ++totalCol, _T("Image Path") );				// Image Path
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 145 ) );			// 아이템 종류
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 421 ) );			// 레벨
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 865 ) );			// 등급
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 272 ) );			// 물리방어력
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 275 ) );			// 마법방어력
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 852 ) );			// 옵션부여
	listCtrl.SetItemText( 0, ++totalCol, GetSystemMsg( 48 ) );				// 기타

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Make Armor Data.. ") );

	for( std::map< DWORD, ITEMINDEXLIST >::const_iterator iterArmorList = mapArmorList.begin() ; iterArmorList != mapArmorList.end() ; ++iterArmorList)
	{
		const ITEMINDEXLIST& itemIndexList = iterArmorList->second;
		for( ITEMINDEXLIST::const_iterator iterItemIndex = itemIndexList.begin() ; iterItemIndex != itemIndexList.end() ; ++iterItemIndex, ++row )
		{
			const DWORD itemIndex = *iterItemIndex;
			const ITEM_INFO& itemInfo = GetItemInfo( itemIndex );

			int col = 0;

			// Add Item Idx
			_stprintf( buf, _T("%d"), itemInfo.ItemIdx );
			listCtrl.SetItemText( row, col, buf );

			// Add Item Name
			listCtrl.SetItemText( row, ++col, itemInfo.ItemName );

			// Add Item Image Name
			_stprintf( buf, _T("%s%d.%s"), 
				settingScript.GetGlobalStringWithSTLString( "ItemIconImagePrefix" ).c_str(), 
				itemInfo.Image2DNum, 
				settingScript.GetGlobalStringWithSTLString( "CaptureImageFormat" ).c_str() );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Armor Type
			listCtrl.SetItemText( row, ++col, GetItemDetailTypeText( itemInfo ) );

			// Add Item Level
			_stprintf( buf, _T("%d"), itemInfo.LimitLevel );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Item Grade
			listCtrl.SetItemText( row, ++col, GetItemGradeText( itemInfo ) );

			// Add 물방
			_stprintf( buf, _T("%d"), itemInfo.PhysicDefense );
			listCtrl.SetItemText( row, ++col, buf );

			// Add 마방
			_stprintf( buf, _T("%d"), itemInfo.MagicDefense );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Option
			const std::string& optionTxt = GetItemOptionText( itemInfo );
			listCtrl.SetItemText( row, ++col, optionTxt.c_str() );

			// Add Etc
			const std::string& etcTxt = GetItemEtcInfoText( itemInfo );
			listCtrl.SetItemText( row, ++col, etcTxt.c_str() );


			CString strStaticText;
			strStaticText.Format( _T(" Extract Item Data - Make Armor Data(%d/%d).. "), row, totalDataCnt );
			m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

			int makeProcessPercent	= (int)(percent + (((progressEndPercent - progressStartPercent) / totalStepCnt) * (row / (float)totalDataCnt)));
			m_ProgressCtrl.SetPos( makeProcessPercent );
		}
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Save Armor Data.. ") );

	CString path = m_strExecutedPath + _T("/") + settingScript.GetGlobalString( "ExtractDataInfoSaveFolder" ) + _T("/ItemArmorInfo");

	if( bOutText )
		SaveToText( path, listCtrl );

	if( bOutExcel )
		SaveToExcel( path, listCtrl );

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Delete Temporary Armor Data.. ") );
}

void CLunaGameDataExtracterDlg::MakeItemAccessaryData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	// Accessary Type에 대한 Data 분류
	typedef std::set< DWORD > ITEMINDEXLIST;
	std::map< DWORD, ITEMINDEXLIST > mapAccessaryList;

	const float totalStepCnt = 3;
	int step = 0;
	int percent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Sort Accessary Data.. ") );

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	int totalDataCnt = 0;
	for( std::map< DWORD, ITEM_INFO >::const_iterator iterItemInfo = m_ItemInfoList.begin() ; iterItemInfo != m_ItemInfoList.end() ; ++iterItemInfo )
	{
		const ITEM_INFO& itemInfo = iterItemInfo->second;

		// 13. 캐릭터 액세서리 (Main),
		// 14. 캐릭터 액세서리 (Sub)
		if( (itemInfo.dwType == 13 ||
			itemInfo.dwType == 14) == FALSE  )
			continue;

		// DetailType 정렬
		ITEMINDEXLIST& itemIndexList = mapAccessaryList[ itemInfo.dwTypeDetail ];
		itemIndexList.insert( itemInfo.ItemIdx );
		++totalDataCnt;
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Sort Accessary Data Complete.. ") );

	CSimpleTextListCtrl listCtrl;
	int row = 1;
	int totalCol = 0;
	TCHAR buf[1024]={0};

	listCtrl.SetItemText( 0, totalCol, _T("Item Index") );					// Item Index
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 1188 ) );			// 아이템 명
	listCtrl.SetItemText( 0, ++totalCol, _T("Image Path") );				// Image Path
	listCtrl.SetItemText( 0, ++totalCol, GetSystemMsg( 1961 ) );			// 무기
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 421 ) );			// 레벨
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 146 ) );			// 아이템 분류
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 272 ) );			// 물리방어력
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 275 ) );			// 마법방어력
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 852 ) );			// 옵션부여
	listCtrl.SetItemText( 0, ++totalCol, GetSystemMsg( 48 ) );				// 기타

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Make Accessary Data.. ") );

	for( std::map< DWORD, ITEMINDEXLIST >::const_iterator iterAccessaryList = mapAccessaryList.begin() ; iterAccessaryList != mapAccessaryList.end() ; ++iterAccessaryList)
	{
		const ITEMINDEXLIST& itemIndexList = iterAccessaryList->second;
		for( ITEMINDEXLIST::const_iterator iterItemIndex = itemIndexList.begin() ; iterItemIndex != itemIndexList.end() ; ++iterItemIndex, ++row )
		{
			const DWORD itemIndex = *iterItemIndex;
			const ITEM_INFO& itemInfo = GetItemInfo( itemIndex );

			int col = 0;

			// Add Item Idx
			_stprintf( buf, _T("%d"), itemInfo.ItemIdx );
			listCtrl.SetItemText( row, col, buf );

			// Add Item Name
			listCtrl.SetItemText( row, ++col, itemInfo.ItemName );

			// Add Item Image Name
			_stprintf( buf, _T("%s%d.%s"),
				settingScript.GetGlobalStringWithSTLString( "ItemIconImagePrefix" ).c_str(), 
				itemInfo.Image2DNum, 
				settingScript.GetGlobalStringWithSTLString( "CaptureImageFormat" ).c_str() );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Accessary Type
			listCtrl.SetItemText( row, ++col, GetItemDetailTypeText( itemInfo ) );

			// Add Item Level
			_stprintf( buf, _T("%d"), itemInfo.LimitLevel );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Item Grade
			listCtrl.SetItemText( row, ++col, GetItemGradeText( itemInfo ) );

			// Add 물방
			_stprintf( buf, _T("%d"), itemInfo.PhysicDefense );
			listCtrl.SetItemText( row, ++col, buf );

			// Add 마방
			_stprintf( buf, _T("%d"), itemInfo.MagicDefense );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Option
			const std::string& optionTxt = GetItemOptionText( itemInfo );
			listCtrl.SetItemText( row, ++col, optionTxt.c_str() );

			// Add Etc
			const std::string& etcTxt = GetItemEtcInfoText( itemInfo );
			listCtrl.SetItemText( row, ++col, etcTxt.c_str() );


			CString strStaticText;
			strStaticText.Format( _T(" Extract Item Data - Make Accessary Data(%d/%d).. "), row, totalDataCnt );
			m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

			int makeProcessPercent	= (int)(percent + (((progressEndPercent - progressStartPercent) / totalStepCnt) * (row / (float)totalDataCnt)));
			m_ProgressCtrl.SetPos( makeProcessPercent );
		}
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Save Accessary Data.. ") );

	CString path = m_strExecutedPath + _T("/") + settingScript.GetGlobalString( "ExtractDataInfoSaveFolder" ) + _T("/ItemAccessaryInfo");

	if( bOutText )
		SaveToText( path, listCtrl );

	if( bOutExcel )
		SaveToExcel( path, listCtrl );

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Delete Temporary Accessary Data.. ") );
}

void CLunaGameDataExtracterDlg::MakeItemEtcData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	// Etc Type에 대한 Data 분류
	typedef std::set< DWORD > ITEMINDEXLIST;
	std::map< DWORD, ITEMINDEXLIST > mapEtcList;

	const float totalStepCnt = 3;
	int step = 0;
	int percent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Sort Etc Data.. ") );

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	int totalDataCnt = 0;
	for( std::map< DWORD, ITEM_INFO >::const_iterator iterItemInfo = m_ItemInfoList.begin() ; iterItemInfo != m_ItemInfoList.end() ; ++iterItemInfo )
	{
		const ITEM_INFO& itemInfo = iterItemInfo->second;

		// 캐릭터 무기/방어구/악세서리를 제외한 모두
		// 11. 캐릭터 무기
		// 12. 캐릭터 방어구
		// 13. 캐릭터 액세서리 (Main),
		// 14. 캐릭터 액세서리 (Sub)
		if( itemInfo.dwType == 11 || 
			itemInfo.dwType == 12 ||
			itemInfo.dwType == 13 ||
			itemInfo.dwType == 14 )
			continue;

		// DetailType 정렬
		ITEMINDEXLIST& itemIndexList = mapEtcList[ itemInfo.dwTypeDetail ];
		itemIndexList.insert( itemInfo.ItemIdx );
		++totalDataCnt;
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Sort Etc Data Complete.. ") );

	CSimpleTextListCtrl listCtrl;
	int row = 1;
	int totalCol = 0;
	TCHAR buf[1024]={0};

	listCtrl.SetItemText( 0, totalCol, _T("Item Index") );					// Item Index
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 1188 ) );			// 아이템 명
	listCtrl.SetItemText( 0, ++totalCol, _T("Image Path") );				// Image Path
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 333 ) );			// 상세정보

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Make Etc Data.. ") );

	for( std::map< DWORD, ITEMINDEXLIST >::const_iterator iterEtcList = mapEtcList.begin() ; iterEtcList != mapEtcList.end() ; ++iterEtcList )
	{
		const ITEMINDEXLIST& itemIndexList = iterEtcList->second;
		for( ITEMINDEXLIST::const_iterator iterItemIndex = itemIndexList.begin() ; iterItemIndex != itemIndexList.end() ; ++iterItemIndex, ++row )
		{
			const DWORD itemIndex = *iterItemIndex;
			const ITEM_INFO& itemInfo = GetItemInfo( itemIndex );

			int col = 0;

			// Add Item Idx
			_stprintf( buf, _T("%d"), itemInfo.ItemIdx );
			listCtrl.SetItemText( row, col, buf );

			// Add Item Name
			listCtrl.SetItemText( row, ++col, itemInfo.ItemName );

			// Add Item Image Name
			_stprintf( buf, _T("%s%d.%s"), 
				settingScript.GetGlobalStringWithSTLString( "ItemIconImagePrefix" ).c_str(), 
				itemInfo.Image2DNum, 
				settingScript.GetGlobalStringWithSTLString( "CaptureImageFormat" ).c_str() );
			listCtrl.SetItemText( row, ++col, buf );

			// Add ToolTip
			listCtrl.SetItemText( row, ++col, GetToolTipMsg( itemIndex ) );

			CString strStaticText;
			strStaticText.Format( _T(" Extract Item Data - Make Etc Data(%d/%d).. "), row, totalDataCnt );
			m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

			int makeProcessPercent	= (int)(percent + (((progressEndPercent - progressStartPercent) / totalStepCnt) * (row / (float)totalDataCnt)));
			m_ProgressCtrl.SetPos( makeProcessPercent );
		}
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Save Etc Data.. ") );

	CString path = m_strExecutedPath + _T("/") + settingScript.GetGlobalString( "ExtractDataInfoSaveFolder" ) + _T("/ItemEtcInfo");

	if( bOutText )
		SaveToText( path, listCtrl );

	if( bOutExcel )
		SaveToExcel( path, listCtrl );

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Delete Temporary Etc Data.. ") );
}

void CLunaGameDataExtracterDlg::MakeItemSetData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	int totalDataCnt = 0;
	for( std::map< DWORD, SetScript >::const_iterator iterSetItemInfo = m_SetItemInfoList.begin() ; iterSetItemInfo != m_SetItemInfoList.end() ; ++iterSetItemInfo )
	{
		const SetScript& script = iterSetItemInfo->second;
		if( script.mAbility.empty() )
		{
			continue;
		}

		++totalDataCnt;
	}

	const float totalStepCnt = 1;
	int step = 0;
	int percent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Make Set Item Data.. ") );

	CSimpleTextListCtrl listCtrl;
	int row = 0;
	int totalCol = 0;
	TCHAR buf[1024]={0};

	listCtrl.SetItemText( 0, totalCol, _T("Item Index") );					// Item Index
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 1188 ) );			// 아이템 명
	listCtrl.SetItemText( 0, ++totalCol, _T("Image Path") );				// Image Path
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 145 ) );			// 아이템 종류
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 421 ) );			// 레벨
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 865 ) );			// 등급
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 271 ) );			// 물리공격력
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 274 ) );			// 마법공격력
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 272 ) );			// 물리방어력
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 275 ) );			// 마법방어력
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 852 ) );			// 옵션부여
    
	int treatedDataCnt = 0;
	for( std::map< DWORD, SetScript >::const_iterator iterSetItemInfo = m_SetItemInfoList.begin() ; iterSetItemInfo != m_SetItemInfoList.end() ; ++iterSetItemInfo )
	{
		const SetScript& script = iterSetItemInfo->second;

		if( script.mAbility.empty() )
		{
			continue;
		}

		BOOL bIsFirstItem = TRUE;

		for( SetScript::Item::const_iterator iterSetItem = script.mItem.begin() ; iterSetItem != script.mItem.end() ; ++iterSetItem )
		{
			const DWORD setItemIndex = *iterSetItem;
			const ITEM_INFO& itemInfo = GetItemInfo( setItemIndex );

			int col = 0;

			// Add Item Idx
			_stprintf( buf, _T("%d"), itemInfo.ItemIdx );
			listCtrl.SetItemText( ++row, col, buf );

			// Add Item Name
			listCtrl.SetItemText( row, ++col, itemInfo.ItemName );

			// Add Item Image Name
			_stprintf( buf, _T("%s%d.%s"), 
				settingScript.GetGlobalStringWithSTLString( "ItemIconImagePrefix" ).c_str(), 
				itemInfo.Image2DNum, 
				settingScript.GetGlobalStringWithSTLString( "CaptureImageFormat" ).c_str() );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Weapon Type
			listCtrl.SetItemText( row, ++col, GetItemDetailTypeText( itemInfo ) );

			// Add Item Level
			_stprintf( buf, _T("%d"), itemInfo.LimitLevel );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Item Grade
			listCtrl.SetItemText( row, ++col, GetItemGradeText( itemInfo ) );

			// Add 물공
			_stprintf( buf, _T("%d"), itemInfo.PhysicAttack );
			listCtrl.SetItemText( row, ++col, buf );

			// Add 마공
			_stprintf( buf, _T("%d"), itemInfo.MagicAttack );
			listCtrl.SetItemText( row, ++col, buf );

			// Add 물방
			_stprintf( buf, _T("%d"), itemInfo.PhysicDefense );
			listCtrl.SetItemText( row, ++col, buf );

			// Add 마방
			_stprintf( buf, _T("%d"), itemInfo.MagicDefense );
			listCtrl.SetItemText( row, ++col, buf );

			// Add Option
			const std::string& optionTxt = GetItemOptionText( itemInfo );
			listCtrl.SetItemText( row, ++col, optionTxt.c_str() );

			// Add SetOption text
			if( bIsFirstItem )
			{
				bIsFirstItem = FALSE;
				// setOption text
				std::string setOptText;
				GetItemSetOptionText( script, setOptText );
				listCtrl.SetItemText( row, ++col, setOptText.c_str() );
			}
		}

		listCtrl.SetItemText( ++row, 0, _T("") );

		CString strStaticText;
		strStaticText.Format( _T(" Extract Item Data - Make Set Item Data(%d/%d).. "), ++treatedDataCnt, totalDataCnt );
		m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

		int makeProcessPercent	= (int)(percent + (((progressEndPercent - progressStartPercent) / totalStepCnt) * (treatedDataCnt / (float)totalDataCnt)));
		m_ProgressCtrl.SetPos( makeProcessPercent );
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Save Set Item Data.. ") );

	CString path = m_strExecutedPath + _T("/") + settingScript.GetGlobalString( "ExtractDataInfoSaveFolder" ) + _T("/SetItemInfo");

	if( bOutText )
		SaveToText( path, listCtrl );

	if( bOutExcel )
		SaveToExcel( path, listCtrl );

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Item Data - Delete Temporary Set Item Data.. ") );
}

BOOL CLunaGameDataExtracterDlg::ExtractMonsterData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	::SetCurrentDirectory( m_strExecutedPath );

	TCHAR extractDataInfoSaveFolderName[ MAX_PATH ]={0};
	settingScript.GetGlobalString( "ExtractDataInfoSaveFolder", extractDataInfoSaveFolderName, sizeof(extractDataInfoSaveFolderName) );

	if( _tcslen( extractDataInfoSaveFolderName ) > 0 )
	{
		CString path;
		path.Format( "%s/%s/", m_strExecutedPath, extractDataInfoSaveFolderName );

		::CreateDirectory( path, NULL );

		::SetCurrentDirectory( path );
	}

	const float totalStepCnt = 1;
	int step = 0;
	int percent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Monster Data - Make Monster Data.. ") );

	CSimpleTextListCtrl listCtrl;
	int row = 0;
	int totalCol = 0;
	TCHAR buf[1024]={0};
	int totalDataCnt = m_MonsterInfoList.size();
	int treatedDataCnt = 0;

	listCtrl.SetItemText( 0, totalCol, _T("Monster Index") );				// Monster Index
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 25 ) );			// 이름
	listCtrl.SetItemText( 0, ++totalCol, _T("Image Path") );				// Image Path
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 421 ) );			// 레벨
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 284 ) );			// HP
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 975 ) );			// 경험치
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 974 ) );			// 타 입
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 1081 ) );			// 배치
	listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 1193 ) );			// 전체아이템


	for( std::map< DWORD, BASE_MONSTER_LIST >::const_iterator iterMonsterInfo = m_MonsterInfoList.begin() ; iterMonsterInfo != m_MonsterInfoList.end() ; ++iterMonsterInfo )
	{
		const BASE_MONSTER_LIST& monsterInfo = iterMonsterInfo->second;

		int col = 0;

		// Add Monster Index
		_stprintf( buf, _T("%d"), monsterInfo.MonsterKind );
		listCtrl.SetItemText( ++row, col, buf );

		// Add 이름
		listCtrl.SetItemText( row, ++col, monsterInfo.Name );

		// Add Image Path
		CString strChxFileName = monsterInfo.ChxName;
		strChxFileName = strChxFileName.MakeUpper();
		strChxFileName.Replace( ".CHX", "" );
		strChxFileName.Replace( ".CHR", "" );
		strChxFileName.Replace( ".MOD", "" );

		_stprintf( buf, _T("%s%s.%s"), 
			settingScript.GetGlobalStringWithSTLString( "MonsterImagePrefix" ).c_str(), 
			strChxFileName, 
			settingScript.GetGlobalStringWithSTLString( "CaptureImageFormat" ).c_str() );
		listCtrl.SetItemText( row, ++col, buf );

		// Add 레벨
		_stprintf( buf, _T("%d"), monsterInfo.Level );
		listCtrl.SetItemText( row, ++col, buf );

		// Add HP
		_stprintf( buf, _T("%d"), monsterInfo.Life );
		listCtrl.SetItemText( row, ++col, buf );

		// Add 경험치
		_stprintf( buf, _T("%d"), monsterInfo.ExpPoint );
		listCtrl.SetItemText( row, ++col, buf );

		// Add 타 입
		switch( monsterInfo.ObjectKind )
		{
			// Need Msg Index
		case eObjectKind_BossMonster:		listCtrl.SetItemText( row, ++col, _T("보스 몬스터") );		break;
		case eObjectKind_FieldBossMonster:	listCtrl.SetItemText( row, ++col, _T("필드 보스 몬스터") );		break;
		case eObjectKind_FieldSubMonster:	listCtrl.SetItemText( row, ++col, _T("슬레이브 몬스터") );		break;
		case eObjectKind_Trap:				listCtrl.SetItemText( row, ++col, GetSystemMsg( 2132 ) );		break;
		case eObjectKind_Vehicle :			listCtrl.SetItemText( row, ++col, GetInterfaceMsg( 1485 ) );		break;
		default:							listCtrl.SetItemText( row, ++col, _T("일반 몬스터") );		break;
		}

		// Add 배치
		std::map< DWORD, MAPINDEXLIST >::const_iterator iterRegenInfo = m_MonsterRegenInfoList.find( monsterInfo.MonsterKind );
		if( iterRegenInfo == m_MonsterRegenInfoList.end() )
		{
			listCtrl.SetItemText( row, ++col, _T("-") );
		}
		else
		{
			BOOL bIsFirst = TRUE;
			std::string regenInfoText;
			const MAPINDEXLIST& mapIndexList = iterRegenInfo->second;
			for( MAPINDEXLIST::const_iterator iterMapIndex = mapIndexList.begin() ; iterMapIndex != mapIndexList.end() ; ++iterMapIndex )
			{
				const DWORD mapnum = *iterMapIndex;

				if( bIsFirst == FALSE )
					regenInfoText.append( _T("\n") );

				regenInfoText.append( _T("- ") );
				regenInfoText.append( GetMapName( mapnum ) );
				bIsFirst = FALSE;
			}
			
			listCtrl.SetItemText( row, ++col, regenInfoText.c_str() );
		}

		// Add 전체아이템
		{
			BOOL bIsFirst = TRUE;
			std::string dropInfoText;
			for( int count = 0 ; count < eDROPITEMKIND_MAX-1 ; ++count )
			{
				const ITEM_INFO& dropItemInfo = GetItemInfo( monsterInfo.dropItem[ count ].wItemIdx );
				if( dropItemInfo.ItemIdx == 0 ||
					dropItemInfo.ItemIdx != monsterInfo.dropItem[ count ].wItemIdx )
					continue;

				if( bIsFirst == FALSE )
					dropInfoText.append( _T("\n") );

				dropInfoText.append( _T("- ") );
				dropInfoText.append( dropItemInfo.ItemName );

				_stprintf( buf, _T(" %d개"), monsterInfo.dropItem[ count ].byCount );
				dropInfoText.append( buf );


				if( settingScript.GetGlobalBoolean( "VisibleMonsterDropRate" ) == TRUE )
				{
					const double droprate = monsterInfo.dropItem[ count ].dwDropPercent / 1000000.0;
					_stprintf( buf, _T("(%.3f%%)"), droprate );
					dropInfoText.append( buf );
				}

				bIsFirst = FALSE;
			}

			listCtrl.SetItemText( row, ++col, dropInfoText.c_str() );
		}


		CString strStaticText;
		strStaticText.Format( _T(" Extract Monster Data - Make Monster Data(%d/%d).. "), ++treatedDataCnt, totalDataCnt );
		m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

		int makeProcessPercent	= (int)(percent + (((progressEndPercent - progressStartPercent) / totalStepCnt) * (treatedDataCnt / (float)totalDataCnt)));
		m_ProgressCtrl.SetPos( makeProcessPercent );
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Monster Data - Save Monster Data.. ") );

	CString path = m_strExecutedPath + _T("/") + settingScript.GetGlobalString( "ExtractDataInfoSaveFolder" ) + _T("/MonsterInfo");

	if( bOutText )
		SaveToText( path, listCtrl );

	if( bOutExcel )
		SaveToExcel( path, listCtrl );

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Monster Data - Delete Temporary Monster Data.. ") );


	return TRUE;
}

BOOL CLunaGameDataExtracterDlg::ExtractEachClassSkillData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	::SetCurrentDirectory( m_strExecutedPath );

	TCHAR extractDataInfoSaveFolderName[ MAX_PATH ]={0};
	settingScript.GetGlobalString( "ExtractDataInfoSaveFolder", extractDataInfoSaveFolderName, sizeof(extractDataInfoSaveFolderName) );

	if( _tcslen( extractDataInfoSaveFolderName ) > 0 )
	{
		CString path;
		path.Format( "%s/%s/", m_strExecutedPath, extractDataInfoSaveFolderName );

		::CreateDirectory( path, NULL );

		::SetCurrentDirectory( path );
	}

	const float totalStepCnt = 2;
	int step = 0;
	int startPercent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	int endPercent		= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	MakeEachClassSkillData( startPercent, endPercent, bOutText, bOutExcel );

	startPercent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	endPercent		= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	MakeEachClassAllSkillData( startPercent, endPercent, bOutText, bOutExcel );

	return TRUE;
}

void CLunaGameDataExtracterDlg::MakeEachClassSkillData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	const float totalStepCnt = 1;
	int step = 0;
	int percent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Skill Data - Make Each Class Skill Data.. ") );


	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );
	std::map< int, std::pair< CSimpleTextListCtrl, int > > mapListCtrl;
	TCHAR buf[1024]={0};
	int totalDataCnt = 0;
	int treatedDataCnt = 0;

	// 클래스(파이터, 로그, 메이지, 마족)
	for( int classindex = 1 ; classindex <= eClass_MAXCLASS ; ++classindex )
	{
		const int maxClassGrade = 10;
		for( int classgrade = 1 ; classgrade <= maxClassGrade ; ++classgrade )
		{
			const int maxJob = 10;
			for( int job = 1 ; job <= maxJob ; ++job )
			{
				int baseTribe = 0;

				// 직업전환 가능 종족( 휴먼, 엘프, 마족 ) 구분
				for( int tribe = 0 ; tribe < RaceType_Max ; ++tribe )
				{
					int classNum = classindex * 1000 + (tribe + 1) * 100  + classgrade * 10 + job;
					if( GetClassNameNum( classNum ) != 0 &&
						baseTribe == 0 )
					{
						baseTribe = tribe + 1;
					}
				}

				int classNum = classindex * 1000 + baseTribe * 100  + classgrade * 10 + job;
				const DWORD classmsgindex = GetClassNameNum( classNum );
				if( classmsgindex ==  0 )
					continue;

				std::map< DWORD, CANLEARNSKILLINDEXLIST >::const_iterator iterClassCanLearnSkillList = m_ClassCanLearnSkillIndex.find( classNum );
				if( iterClassCanLearnSkillList == m_ClassCanLearnSkillIndex.end() )
					continue;

				const CANLEARNSKILLINDEXLIST& canLearnSkillIndexList = iterClassCanLearnSkillList->second;

				totalDataCnt += canLearnSkillIndexList.size();
			}
		}
	}


	// 클래스(파이터, 로그, 메이지, 마족)
	for( int classindex = 1 ; classindex <= eClass_MAXCLASS ; ++classindex )
	{
		const int maxClassGrade = 10;
		for( int classgrade = 1 ; classgrade <= maxClassGrade ; ++classgrade )
		{
			const int maxJob = 10;
			for( int job = 1 ; job <= maxJob ; ++job )
			{
				int baseTribe = 0;
				BOOL bValidTribe[RaceType_Max]={0};

				// 직업전환 가능 종족( 휴먼, 엘프, 마족 ) 구분
				for( int tribe = 0 ; tribe < RaceType_Max ; ++tribe )
				{
					int classNum = classindex * 1000 + (tribe + 1) * 100  + classgrade * 10 + job;
					bValidTribe[ tribe ] = GetClassNameNum( classNum ) != 0;

					if( bValidTribe[ tribe ] == TRUE &&
						baseTribe == 0 )
					{
						baseTribe = tribe + 1;
					}
				}

				int classNum = classindex * 1000 + baseTribe * 100  + classgrade * 10 + job;
				const DWORD classmsgindex = GetClassNameNum( classNum );
				if( classmsgindex ==  0 )
					continue;

				LPCTSTR classname = GetInterfaceMsg( classmsgindex );

				std::map< DWORD, CANLEARNSKILLINDEXLIST >::const_iterator iterClassCanLearnSkillList = m_ClassCanLearnSkillIndex.find( classNum );
				if( iterClassCanLearnSkillList == m_ClassCanLearnSkillIndex.end() )
					continue;

				const CANLEARNSKILLINDEXLIST& canLearnSkillList = iterClassCanLearnSkillList->second;
				if( canLearnSkillList.empty() )
					continue;

				BOOL bFirstSkillIndex = TRUE;

				std::pair< CSimpleTextListCtrl, int >& pairListCtrlInfo = mapListCtrl[ classindex ];
				CSimpleTextListCtrl& listCtrl = pairListCtrlInfo.first;
				int& row = pairListCtrlInfo.second;

				for( CANLEARNSKILLINDEXLIST::const_iterator iterCanLearnSkillIndexList = canLearnSkillList.begin() ; iterCanLearnSkillIndexList != canLearnSkillList.end() ; ++iterCanLearnSkillIndexList )
				{
					const DWORD skillIndex = *iterCanLearnSkillIndexList;
					const ACTIVE_SKILL_INFO& activeSkillInfo = GetActiveSkillInfo( skillIndex );
					if( activeSkillInfo.Index != skillIndex )
						continue;

					int col = 0;

					if( bFirstSkillIndex )
					{
						// Add 차수 정보
						_stprintf( buf, "%d차", classgrade );
						listCtrl.SetItemText( ++row, col, buf );

						std::string validTribeText;
						BOOL bFirstTribe = TRUE;
						// 가능 종족 정보설정
						validTribeText.append( "(" );
						for( int tribe = 0 ; tribe < RaceType_Max ; ++tribe )
						{
							if( bValidTribe[ tribe ] == TRUE )
							{
								if( bFirstTribe == FALSE )
									validTribeText.append( ", " );

								switch( tribe )
								{
								case RaceType_Human:		validTribeText.append( GetInterfaceMsg( 247 ) );		break;
								case RaceType_Elf:			validTribeText.append( GetInterfaceMsg( 248 ) );		break;
								case RaceType_Devil:		validTribeText.append( GetInterfaceMsg( 1129 ) );		break;
								}

								bFirstTribe = FALSE;
							}
						}
						validTribeText.append( ")" );

						// 직업 이름 정보
						_stprintf( buf, "%s%s", GetInterfaceMsg( classmsgindex ), validTribeText.c_str() );
						listCtrl.SetItemText( row, ++col, buf );
					}
					else
					{
						++row;
						++col;
					}

					// Add 스킬 이름
					listCtrl.SetItemText( row, ++col, activeSkillInfo.Name );

					// Add image path
					_stprintf( buf, _T("%s%d.%s"), 
						settingScript.GetGlobalStringWithSTLString( "ActiveSkillIconImagePrefix" ).c_str(), 
						activeSkillInfo.Image,
						settingScript.GetGlobalStringWithSTLString( "CaptureImageFormat" ).c_str() );
					listCtrl.SetItemText( row, ++col, buf );

					// Add attack type
					listCtrl.SetItemText( row, ++col, GetAttackTypeStringOfActiveSkill( skillIndex ) );

					// Add skill level
					_stprintf( buf, _T("%d"), activeSkillInfo.Level );
					listCtrl.SetItemText( row, ++col, buf );

					// Add skill tooltip
					listCtrl.SetItemText( row, ++col, GetToolTipMsg( skillIndex ) );

					bFirstSkillIndex = FALSE;

					CString strStaticText;
					strStaticText.Format( _T(" Extract Skill Data - Make Each Class Skill Data(%d/%d).. "), ++treatedDataCnt, totalDataCnt );
					m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

					int makeProcessPercent	= (int)(percent + (((progressEndPercent - progressStartPercent) / totalStepCnt) * (treatedDataCnt / (float)totalDataCnt)));
					m_ProgressCtrl.SetPos( makeProcessPercent );
				}

				listCtrl.SetItemText( ++row, 0, _T("") );
			}
		}
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Skill Data - Save Each Class Skill Data.. ") );
	

	// 입력된 정보들의 컬럼명 쓰기 & 저장
	for( std::map< int, std::pair< CSimpleTextListCtrl, int > >::iterator iterListCtrl = mapListCtrl.begin() ; iterListCtrl != mapListCtrl.end() ; ++iterListCtrl )
	{
		// 클래스(파이터, 로그, 메이지, 마족)
		const int classIndex = iterListCtrl->first;
		std::pair< CSimpleTextListCtrl, int >& pairListCtrlInfo = iterListCtrl->second;
		CSimpleTextListCtrl& listCtrl = pairListCtrlInfo.first;
		const int& row = pairListCtrlInfo.second;


		int totalCol = 0;

		// 컬럼명 설정

		// Need Msg Index
		listCtrl.SetItemText( 0, totalCol, _T("차수") );
		listCtrl.SetItemText( 0, ++totalCol, _T("직업") );
		listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 25 ) );	// 이름
		listCtrl.SetItemText( 0, ++totalCol, _T("Image Path") );		// Image Path
		listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 974 ) );	// 타 입
		listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 421 ) );	// 레벨
		listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 333 ) );	// 상세정보


		// 저장
		DWORD classIndexMsgIndex = 0;
		switch( classIndex - 1 )
		{
		case eClass_FIGHTER:	classIndexMsgIndex = 364;		break;
		case eClass_ROGUE:		classIndexMsgIndex = 34;		break;
		case eClass_MAGE:		classIndexMsgIndex = 35;		break;
		case eClass_DEVIL:		classIndexMsgIndex = 1129;		break;
		}

		// 클래스 마다 파일명 다르게 저장
		TCHAR safeFilename[MAX_PATH]={0};
		_stprintf( safeFilename, _T("/%s_Job_SkillList"), GetInterfaceMsg( classIndexMsgIndex ) );
		CString path = m_strExecutedPath + _T("/") + settingScript.GetGlobalString( "ExtractDataInfoSaveFolder" ) + safeFilename;

		if( bOutText )
			SaveToText( path, listCtrl );

		if( bOutExcel )
			SaveToExcel( path, listCtrl );
	}

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Skill Data - Delete Temporary Each Class Skill Data.. ") );
}

void CLunaGameDataExtracterDlg::MakeEachClassAllSkillData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	const float totalStepCnt = 1;
	int step = 0;
	int percent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Skill Data - Make Each Class All Skill Data.. ") );


	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );
	std::map< int, std::pair< CSimpleTextListCtrl, int > > mapListCtrl;
	TCHAR buf[1024]={0};
	int totalDataCnt = 0;
	int treatedDataCnt = 0;

	// 클래스(파이터, 로그, 메이지, 마족)
	for( int classindex = 1 ; classindex <= eClass_MAXCLASS ; ++classindex )
	{
		const int maxClassGrade = 10;
		for( int classgrade = 1 ; classgrade <= maxClassGrade ; ++classgrade )
		{
			const int maxJob = 10;
			for( int job = 1 ; job <= maxJob ; ++job )
			{
				int baseTribe = 0;

				// 직업전환 가능 종족( 휴먼, 엘프, 마족 ) 구분
				for( int tribe = 0 ; tribe < RaceType_Max ; ++tribe )
				{
					int classNum = classindex * 1000 + (tribe + 1) * 100  + classgrade * 10 + job;
					if( GetClassNameNum( classNum ) != 0 &&
						baseTribe == 0 )
					{
						baseTribe = tribe + 1;
					}
				}

				int classNum = classindex * 1000 + baseTribe * 100  + classgrade * 10 + job;
				const DWORD classmsgindex = GetClassNameNum( classNum );
				if( classmsgindex ==  0 )
					continue;

				std::map< DWORD, CANLEARNSKILLINDEXLIST >::const_iterator iterClassCanLearnSkillList = m_ClassCanLearnSkillIndex.find( classNum );
				if( iterClassCanLearnSkillList == m_ClassCanLearnSkillIndex.end() )
					continue;

				const CANLEARNSKILLINDEXLIST& canLearnSkillIndexList = iterClassCanLearnSkillList->second;

				for( CANLEARNSKILLINDEXLIST::const_iterator iterCanLearnSkillIndexList = canLearnSkillIndexList.begin() ; iterCanLearnSkillIndexList != canLearnSkillIndexList.end() ; ++iterCanLearnSkillIndexList )
				{
					const DWORD skillIndex = *iterCanLearnSkillIndexList;
					int skillLevel = (int)(skillIndex % 100);
					totalDataCnt += skillLevel;
				}
			}
		}
	}

	// 클래스(파이터, 로그, 메이지, 마족)
	for( int classindex = 1 ; classindex <= eClass_MAXCLASS ; ++classindex )
	{
		const int maxClassGrade = 10;
		for( int classgrade = 1 ; classgrade <= maxClassGrade ; ++classgrade )
		{
			const int maxJob = 10;
			for( int job = 1 ; job <= maxJob ; ++job )
			{
				int baseTribe = 0;
				BOOL bValidTribe[RaceType_Max]={0};

				// 직업전환 가능 종족( 휴먼, 엘프, 마족 ) 구분
				for( int tribe = 0 ; tribe < RaceType_Max ; ++tribe )
				{
					int classNum = classindex * 1000 + (tribe + 1) * 100  + classgrade * 10 + job;
					bValidTribe[ tribe ] = GetClassNameNum( classNum ) != 0;

					if( bValidTribe[ tribe ] == TRUE &&
						baseTribe == 0 )
					{
						baseTribe = tribe + 1;
					}
				}

				int classNum = classindex * 1000 + baseTribe * 100  + classgrade * 10 + job;
				const DWORD classmsgindex = GetClassNameNum( classNum );
				if( classmsgindex ==  0 )
					continue;

				LPCTSTR classname = GetInterfaceMsg( classmsgindex );

				std::map< DWORD, CANLEARNSKILLINDEXLIST >::const_iterator iterClassCanLearnSkillList = m_ClassCanLearnSkillIndex.find( classNum );
				if( iterClassCanLearnSkillList == m_ClassCanLearnSkillIndex.end() )
					continue;

				const CANLEARNSKILLINDEXLIST& canLearnSkillList = iterClassCanLearnSkillList->second;
				if( canLearnSkillList.empty() )
					continue;

				std::pair< CSimpleTextListCtrl, int >& pairListCtrlInfo = mapListCtrl[ classindex ];
				CSimpleTextListCtrl& listCtrl = pairListCtrlInfo.first;
				int& row = pairListCtrlInfo.second;

				for( CANLEARNSKILLINDEXLIST::const_iterator iterCanLearnSkillIndexList = canLearnSkillList.begin() ; iterCanLearnSkillIndexList != canLearnSkillList.end() ; ++iterCanLearnSkillIndexList )
				{
					const DWORD jobSkillIndex = *iterCanLearnSkillIndexList;

					for( int skillLv = 1 ; skillLv <= (int)(jobSkillIndex % 100) ; ++skillLv )
					{
						const DWORD skillIndex = (int)(jobSkillIndex / 100) * 100 + skillLv;

						const ACTIVE_SKILL_INFO& activeSkillInfo = GetActiveSkillInfo( skillIndex );
						if( activeSkillInfo.Index != skillIndex )
							continue;

						int col = 0;

						// Need Msg Index
						// Add 차수 정보
						_stprintf( buf, "%d차", classgrade );
						listCtrl.SetItemText( ++row, col, buf );

						{
							std::string validTribeText;
							BOOL bFirstTribe = TRUE;
							// 가능 종족 정보설정
							validTribeText.append( "(" );
							for( int tribe = 0 ; tribe < RaceType_Max ; ++tribe )
							{
								if( bValidTribe[ tribe ] == TRUE )
								{
									if( bFirstTribe == FALSE )
										validTribeText.append( ", " );

									switch( tribe )
									{
									case RaceType_Human:		validTribeText.append( GetInterfaceMsg( 247 ) );		break;
									case RaceType_Elf:			validTribeText.append( GetInterfaceMsg( 248 ) );		break;
									case RaceType_Devil:		validTribeText.append( GetInterfaceMsg( 1129 ) );		break;
									}

									bFirstTribe = FALSE;
								}
							}
							validTribeText.append( ")" );

							// Add 직업 이름 정보
							_stprintf( buf, "%s%s", GetInterfaceMsg( classmsgindex ), validTribeText.c_str() );
							listCtrl.SetItemText( row, ++col, buf );
						}

						// Add 스킬 이름
						listCtrl.SetItemText( row, ++col, activeSkillInfo.Name );

						// Add skill level
						_stprintf( buf, _T("%d"), activeSkillInfo.Level );
						listCtrl.SetItemText( row, ++col, buf );

						// Add limit level
						_stprintf( buf, _T("%d"), activeSkillInfo.RequiredPlayerLevel );
						listCtrl.SetItemText( row, ++col, buf );

						// Add cool time
						_stprintf( buf, _T("%.1f초"), activeSkillInfo.CoolTime/1000.f );
						listCtrl.SetItemText( row, ++col, buf );

						// Add need mana
						_stprintf( buf, _T("%d"), (int)activeSkillInfo.mMana.mPlus );
						listCtrl.SetItemText( row, ++col, buf );

						// Add need sp
						_stprintf( buf, _T("%d"), activeSkillInfo.TrainPoint );
						listCtrl.SetItemText( row, ++col, buf );

						// Add need gold
						_stprintf( buf, _T("%d"), activeSkillInfo.TrainMoney );
						listCtrl.SetItemText( row, ++col, buf );

						// Add skill tooltip
						listCtrl.SetItemText( row, ++col, GetToolTipMsg( skillIndex ) );

						CString strStaticText;
						strStaticText.Format( _T(" Extract Skill Data - Make Each Class All Skill Data(%d/%d).. "), ++treatedDataCnt, totalDataCnt );
						m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

						int makeProcessPercent	= (int)(percent + (((progressEndPercent - progressStartPercent) / totalStepCnt) * (treatedDataCnt / (float)totalDataCnt)));
						m_ProgressCtrl.SetPos( makeProcessPercent );
					}
				}

				listCtrl.SetItemText( ++row, 0, _T("") );
			}
		}
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Skill Data - Save Each Class All Skill Data.. ") );
	

	// 입력된 정보들의 컬럼명 쓰기 & 저장
	for( std::map< int, std::pair< CSimpleTextListCtrl, int > >::iterator iterListCtrl = mapListCtrl.begin() ; iterListCtrl != mapListCtrl.end() ; ++iterListCtrl )
	{
		// 클래스(파이터, 로그, 메이지, 마족)
		const int classIndex = iterListCtrl->first;
		std::pair< CSimpleTextListCtrl, int >& pairListCtrlInfo = iterListCtrl->second;
		CSimpleTextListCtrl& listCtrl = pairListCtrlInfo.first;
		const int& row = pairListCtrlInfo.second;


		int totalCol = 0;

		// 컬럼명 설정

		// Need Msg Index
		listCtrl.SetItemText( 0, totalCol, _T("차수") );
		listCtrl.SetItemText( 0, ++totalCol, _T("직업") );
		listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 25 ) );	// 이름
		listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 636 ) );	// 스킬 레벨
		listCtrl.SetItemText( 0, ++totalCol, _T("제한 레벨") );			// 제한 레벨
		listCtrl.SetItemText( 0, ++totalCol, GetSystemMsg( 2147 ) );	// 쿨타임
		listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 494 ) );	// 소모 마나
		listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 495 ) );	// 필요 SP
		listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 496 ) );	// 필요 GOLD
		listCtrl.SetItemText( 0, ++totalCol, GetInterfaceMsg( 333 ) );	// 상세정보


		// 저장
		DWORD classIndexMsgIndex = 0;
		switch( classIndex - 1 )
		{
		case eClass_FIGHTER:	classIndexMsgIndex = 364;		break;
		case eClass_ROGUE:		classIndexMsgIndex = 34;		break;
		case eClass_MAGE:		classIndexMsgIndex = 35;		break;
		case eClass_DEVIL:		classIndexMsgIndex = 1129;		break;
		}

		// 클래스 마다 파일명 다르게 저장
		TCHAR safeFilename[MAX_PATH]={0};
		_stprintf( safeFilename, _T("/%s_All_SkillList"), GetInterfaceMsg( classIndexMsgIndex ) );
		CString path = m_strExecutedPath + _T("/") + settingScript.GetGlobalString( "ExtractDataInfoSaveFolder" ) + safeFilename;

		if( bOutText )
			SaveToText( path, listCtrl );

		if( bOutExcel )
			SaveToExcel( path, listCtrl );
	}

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Skill Data - Delete Temporary Each Class All Skill Data.. ") );
}

LPCTSTR CLunaGameDataExtracterDlg::GetAttackTypeStringOfActiveSkill( DWORD skillIndex )
{
	static TCHAR buf[MAX_PATH];
	ZeroMemory( buf, sizeof(buf) );

	const ACTIVE_SKILL_INFO& activeSkillInfo = GetActiveSkillInfo( skillIndex );
	if( activeSkillInfo.Index != skillIndex )
		return buf;

	DWORD attacktypeStringIndex = 0;
	switch( activeSkillInfo.Kind )
	{
	case SKILLKIND_PHYSIC:		attacktypeStringIndex = 2100;		break;		// 액티브(물리)
	case SKILLKIND_MAGIC:		attacktypeStringIndex = 2101;		break;		// 액티브(마법)
	case SKILLKIND_PASSIVE:		attacktypeStringIndex = 176;		break;		// 패시브
	case SKILLKIND_ONOFF:		attacktypeStringIndex = 2102;		break;		// 토글
	case SKILLKIND_BUFF:		attacktypeStringIndex = 2103;		break;		// 버프
	}

	DWORD attackdetailtypeStringIndex = 0;
	switch( activeSkillInfo.UnitDataType )
	{
		// 스킬 위력 ( 스탯 )
	case 1:
		{
			switch( activeSkillInfo.Unit )
			{
			case UNITKIND_PHYSIC_ATTCK:		attackdetailtypeStringIndex = 157;		break;
			case UNITKIND_MAGIC_ATTCK:		attackdetailtypeStringIndex = 160;		break;
			}
		}
		break;
		// 스킬 위력 ( 무기 )
	case 2:
		{
			attackdetailtypeStringIndex = 1961;
		}
		break;
	}

	if( attackdetailtypeStringIndex )
		_stprintf( buf, _T("%s - %s"), GetSystemMsg( attacktypeStringIndex ), GetSystemMsg( attackdetailtypeStringIndex ) );
	else
		_stprintf( buf, _T("%s"), GetSystemMsg( attacktypeStringIndex ), GetSystemMsg( attackdetailtypeStringIndex ) );

	return buf;
}

BOOL CLunaGameDataExtracterDlg::ExtractQuestScriptData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel )
{
	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );

	::SetCurrentDirectory( m_strExecutedPath );

	TCHAR extractDataInfoSaveFolderName[ MAX_PATH ]={0};
	settingScript.GetGlobalString( "ExtractDataInfoSaveFolder", extractDataInfoSaveFolderName, sizeof(extractDataInfoSaveFolderName) );

	if( _tcslen( extractDataInfoSaveFolderName ) > 0 )
	{
		CString path;
		path.Format( "%s/%s/", m_strExecutedPath, extractDataInfoSaveFolderName );

		::CreateDirectory( path, NULL );

		::SetCurrentDirectory( path );
	}

	const float totalStepCnt = 1;
	int step = 0;
	int percent	= (int)((progressEndPercent - progressStartPercent) * ( step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Quest Script Data.. ") );

	CMHFile mhFile;
	CString strScriptPath( m_strLunaDataPath );
	strScriptPath.Append( "/system/resource/QuestScript.bin" );
	
	if( mhFile.Init( (LPSTR)(LPCTSTR)strScriptPath, "rb" ) == FALSE )
	{
		m_ProgressStatusStaticCtrl.SetWindowText( _T(" QuestScript.bin Open Failed ! " ) );
		Sleep( 1000 );
		return FALSE;
	}

	CSimpleTextListCtrl listCtrl;
	int row = 0;
	int col = 0;
	int totalDataCnt = 0;
	int treatedDataCnt = 0;

	DWORD questindex = 0;


	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		mhFile.GetLine( buf, sizeof(buf) );
		++totalDataCnt;
	}
	mhFile.Release();


	mhFile.Init( (LPSTR)(LPCTSTR)strScriptPath, "rb" );
	while( !mhFile.IsEOF() )
	{
		char buf[1024]={0};
		char line[1024]={0};
		LPCTSTR	delimit = "\t ";
		mhFile.GetLine( buf, sizeof(buf) );
		_tcsncpy( line, buf, sizeof(line)/sizeof(*line) );

		col = 0;

		for( int len=0 ; len < (int)_tcslen(line) ; ++len )
		{
			if( line[ len ] == '\t' )
				++col;
			else
				break;
		}

		listCtrl.SetItemText( ++row, col, line );

		char* token = strtok( buf, delimit );
		while( token != NULL )
		{
			if( _tcsicmp( token, "$quest" ) == 0 )
			{
				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				questindex = (DWORD)_ttoi( token );

				token = strtok( 0, delimit );
				if( token == NULL ) continue;

				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				DWORD npcindex = (DWORD)_ttoi( token );
				const std::string& npcname = m_NpcName[ npcindex ];

				listCtrl.SetItemText( ++row, col, npcname.c_str() );
			}
			else if( _tcsicmp( token, "$subquest" ) == 0 )
			{
				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				DWORD subquestindex = (DWORD)_ttoi( token );

				std::pair< DWORD, DWORD > key;
				key.first = questindex;
				key.second = subquestindex;

				const std::string& questtitle = m_QuestTitleList[ key ];

				listCtrl.SetItemText( ++row, col, questtitle.c_str() );
			}
			else if( _tcsicmp( token, "#NPCSCRIPT" ) == 0 )
			{
				token = strtok( 0, delimit );
				if( token == NULL ) continue;

				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD npcid = (DWORD)_ttoi( token );

				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD pageindex = (DWORD)_ttoi( token );

				CString addmsg;
				const std::string& npcname = m_NpcName[ npcid ];
				addmsg.Format( "(%s)\n", npcname.c_str() );

				NPCPAGEMSG& npcpagemsg = m_NpcScriptMsgList[ npcid ];
				std::string& npcmsg = npcpagemsg[ pageindex ];
				addmsg.Append( npcmsg.c_str() );

				listCtrl.SetItemText( ++row, col, addmsg );
			}
			else if( _tcsicmp( token, "@HUNT" ) == 0 )
			{
				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD monsterkind = (DWORD)_ttoi( token );
				const BASE_MONSTER_LIST& monsterinfo = m_MonsterInfoList[ monsterkind ];

				CString addmsg;
				addmsg.Format( " - Hunt %s(Lv %d)", monsterinfo.Name, monsterinfo.Level );

				listCtrl.SetItemText( ++row, col, addmsg );
			}
			else if( _tcsicmp( token, "*TAKEITEM" ) == 0 )
			{
				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD itemindex = (DWORD)_ttoi( token );

				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD itemcount = (DWORD)_ttoi( token );

				const ITEM_INFO& iteminfo = m_ItemInfoList[ itemindex ];

				CString addmsg;
				addmsg.Format( " - (획득아이템) %s %d개", iteminfo.ItemName, itemcount );

				listCtrl.SetItemText( ++row, col, addmsg );
			}
			else if( _tcsicmp( token, "*GIVEITEM" ) == 0 )
			{
				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD itemindex = (DWORD)_ttoi( token );

				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD itemcount = (DWORD)_ttoi( token );

				const ITEM_INFO& iteminfo = m_ItemInfoList[ itemindex ];

				CString addmsg;
				addmsg.Format( " - (필요아이템) %s %d개", iteminfo.ItemName, itemcount );

				listCtrl.SetItemText( ++row, col, addmsg );
			}
			else if( _tcsicmp( token, "*TAKEQUESTITEM" ) == 0 )
			{
				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD questitemindex = (DWORD)_ttoi( token );

				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD questitemcount = (DWORD)_ttoi( token );

				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD droprate = (DWORD)_ttoi( token );

				const stQuestItenInfo& questiteminfo = m_QuestItemInfo[ questitemindex ];

				CString addmsg;
				addmsg.Format( " - (획득 퀘스트아이템) %s %d개(%.1f%%)\n      %s", 
					questiteminfo.name.c_str(),
					questitemcount,
					droprate/100.f, 
					questiteminfo.itemdesc.c_str() );

				listCtrl.SetItemText( ++row, col, addmsg );
			}
			else if( _tcsicmp( token, "*GIVEQUESTITEM" ) == 0 )
			{
				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD questitemindex = (DWORD)_ttoi( token );

				token = strtok( 0, delimit );
				if( token == NULL ) continue;
				const DWORD questitemcount = (DWORD)_ttoi( token );

				const stQuestItenInfo& questiteminfo = m_QuestItemInfo[ questitemindex ];

				CString addmsg;
				addmsg.Format( " - (필요 퀘스트아이템) %s %d개\n      %s", 
					questiteminfo.name.c_str(),
					questitemcount,
					questiteminfo.itemdesc.c_str() );

				listCtrl.SetItemText( ++row, col, addmsg );
			}

			token = strtok( 0, delimit );
		}

		CString strStaticText;
		strStaticText.Format( _T(" Extract Quest Script Data - Make Quest Script Data(%d/%d).. "), ++treatedDataCnt, totalDataCnt );
		m_ProgressStatusStaticCtrl.SetWindowText( strStaticText );

		int makeProcessPercent	= (int)(percent + (((progressEndPercent - progressStartPercent) / totalStepCnt) * (treatedDataCnt / (float)totalDataCnt)));
		m_ProgressCtrl.SetPos( makeProcessPercent );
	}

	percent	= (int)((progressEndPercent - progressStartPercent) * ( ++step / totalStepCnt)) + progressStartPercent;
	m_ProgressCtrl.SetPos( percent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Quest Script Data - Save Quest Script Data.. ") );

	CString path = m_strExecutedPath + _T("/") + settingScript.GetGlobalString( "ExtractDataInfoSaveFolder" ) + _T("/QuestScriptInfo");

	if( bOutText )
		SaveToText( path, listCtrl );

	if( bOutExcel )
		SaveToExcel( path, listCtrl );

	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Quest Script Data - Delete Temporary Quest Script Data.. ") );

	return TRUE;
}

void CLunaGameDataExtracterDlg::SaveToText( LPCTSTR savePath, const CSimpleTextListCtrl& listCtrl )
{
	CString path = savePath;
	path = path + _T(".txt");

	const int colCnt = listCtrl.GetColCount();
	const int rowCnt = listCtrl.GetRowCount();

	std::string text;
	for( int row = 0 ; row < rowCnt ; ++row )
	{
		for( int col = 0 ; col < colCnt ; ++col )
		{
			text.append( listCtrl.GetItemText( row, col ) );
			text.append( _T("\t") );
		}

		text.append( _T("\n") );
	}
	FILE* fp = fopen( path, "w+" );
	if( !fp )
		return;

	fwrite( text.c_str(), text.length(), 1, fp );
	fclose( fp );
}

void CLunaGameDataExtracterDlg::SaveToExcel( LPCTSTR savePath, const CSimpleTextListCtrl& listCtrl )
{
	CString path = savePath;
	path = path + _T(".xlsx");
	DeleteFile( path );

	// 081017 LUJ, 엑셀 기능 호출을 단순화하는 내부 클래스
	class
	{
	public:
		HRESULT operator()( int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs... )
		{
			va_list marker;
			va_start(marker, cArgs);

			if(!pDisp) {
				AfxMessageBox( _T( "NULL IDispatch passed to AutoWrap()" ) );
				return S_FALSE;
			}

			// Variables used...
			DISPPARAMS dp = { NULL, NULL, 0, 0 };
			DISPID dispidNamed = DISPID_PROPERTYPUT;
			DISPID dispID;
			HRESULT hr;
			TCHAR buf[ MAX_PATH ]		= { 0 };
			TCHAR szName[ MAX_PATH ]	= { 0 };

			// Get DISPID for name passed...
			hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
			if(FAILED(hr))
			{
				return hr;
			}

			// Allocate memory for arguments...
			VARIANT *pArgs = new VARIANT[cArgs+1];
			// Extract arguments...
			for(int i=0; i<cArgs; i++)
			{
				pArgs[i] = va_arg(marker, VARIANT);
			}

			// Build DISPPARAMS
			dp.cArgs = cArgs;
			dp.rgvarg = pArgs;

			// Handle special-case for property-puts!
			if(autoType & DISPATCH_PROPERTYPUT) {
				dp.cNamedArgs = 1;
				dp.rgdispidNamedArgs = &dispidNamed;
			}

			// Make the call!
			hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, pvResult, NULL, NULL);
			if(FAILED(hr))
			{
				va_end(marker);
				delete [] pArgs;
				return hr;
			}

			// End variable-argument section...
			va_end(marker);
			delete [] pArgs;

			return hr;
		}
	}
	AutoWrap;

	// Initialize COM for this thread...
	CoInitialize(NULL);

	// Get CLSID for our server...
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);

	if(FAILED(hr))
	{
		::MessageBox(NULL, _T( "Excel must be installed" ), _T( "Error" ), 0x10010);
		return;
	}

	// Start server and get IDispatch...
	IDispatch *pXlApp;
	hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&pXlApp);
	if(FAILED(hr))
	{
		::MessageBox(NULL, _T( "Excel not registered properly" ), _T( "Error" ), 0x10010);
		return;
	}

	// Make it visible (i.e. app.visible = 1)
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 0;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlApp, L"Visible", 0, x);
	}

	// Get Workbooks collection
	IDispatch *pXlBooks;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"Workbooks", 0);
		pXlBooks = result.pdispVal;
	}

	// Call Workbooks.Add() to get a new workbook...
	IDispatch *pXlBook;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlBooks, L"Add", 0);
		pXlBook = result.pdispVal;
	}

	typedef std::pair< CString, CString > Field;
	typedef std::list< Field > FieldList;

	const int columnSize = listCtrl.GetColCount();

	// Create a 15x15 safearray of variants...
	VARIANT variantArray = { 0 };
	variantArray.vt = VT_ARRAY | VT_VARIANT;
	{
		SAFEARRAYBOUND sab[2];
		sab[0].lLbound		= 1;
		sab[0].cElements	= listCtrl.GetRowCount() + 1;
		sab[1].lLbound		= 1;
		sab[1].cElements	= columnSize;
		variantArray.parray = SafeArrayCreate( VT_VARIANT, 2, sab );
	}

	// 081020 LUJ, 내용을 메모리에 복사한다
	for( int row = 0 ; row < listCtrl.GetRowCount() ; ++row )
	{
		CString statement;
		for( int column = 0 ; column < columnSize ; ++column )
		{
			VARIANT variant = { 0 };
			variant.vt		= VT_BSTR;
			variant.bstrVal	= ::SysAllocString( MultiToWideChar( listCtrl.GetItemText( row, column ) ) );

			long indices[] = { row+1, column+1 };
			SafeArrayPutElement( variantArray.parray, indices, ( void* )&variant );
		}
	}
	
	// Get ActiveSheet object
	IDispatch *pXlSheet = 0;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"ActiveSheet", 0);
		pXlSheet = result.pdispVal;
	}

	// Get Range object for the Range A1:O15...
	IDispatch *pXlRange = 0;
	{
		CString columnText;
		{
			int			size			= columnSize - 1;
			const int	alphabetSize	= abs( 'z' - 'a' ) + 1;

			// 081017 LUJ, 엑셀의 주소는 A ... Z -> AA ... ZZ -> AAA ... ZZZ 이런 식으로 증가된다
			//			따라서 끝 주소를 만드는데는 조금 복잡한 식이 필요하다
			do
			{
				const char text = 'A' + size;
				columnText += CString( text );

				size -= alphabetSize;
			}
			while( alphabetSize < size );
		}

		// 081017 LUJ, 셀 좌표 생성. 남게 지정하면 빈칸에 N/A값이 셀에 표시된다. 모자라면 내용이 적게 표시된다
		CString coordinate;
		coordinate.Format(
			_T( "A1:%s%u" ),
			columnText,
			listCtrl.GetRowCount() + 1 );

		VARIANT parm;
		parm.vt = VT_BSTR;
		parm.bstrVal = ::SysAllocString( MultiToWideChar( coordinate ) );

		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
		VariantClear(&parm);

		pXlRange = result.pdispVal;
	}

	// Set range with our safearray...
	AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"Value", 1, variantArray);

	{
		VARIANT result;
		VariantInit(&result);
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 0;
		VARIANT fname;
		fname.vt = VT_BSTR;
		fname.bstrVal= ::SysAllocString( MultiToWideChar( path ) );
		AutoWrap(DISPATCH_METHOD, &result, pXlBook, L"SaveAs", 1, fname );
	}

	// Set .Saved property of workbook to TRUE so we aren't prompted
	// to save when we tell Excel to quit...
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 1;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlBook, L"Saved", 1, x);
	}

	// Tell Excel to quit (i.e. App.Quit)
	AutoWrap(DISPATCH_METHOD, NULL, pXlApp, L"Quit", 0);

	// Release references...
	pXlRange->Release();
	pXlSheet->Release();
	pXlBook->Release();
	pXlBooks->Release();
	pXlApp->Release();
	VariantClear(&variantArray);

	// Uninitialize COM for this thread...
	CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


void CLunaGameDataExtracterDlg::OnBnClickedButtonExtract()
{
	if( !m_bLoadCompleted )
	{
		AfxMessageBox( _T("Try it After Loading..!") );
		return;
	}

	if( m_bExtracting )
	{
		AfxMessageBox( _T("Already Extracting!") );
		return;
	}

	// Create Game Data Load Thread
	UINT uiThreadID;
	m_ExtractingThread = (HANDLE)_beginthreadex( NULL, NULL, ExtractGameDataThread, this, NULL, &uiThreadID );
	m_bExtracting = TRUE;
}

void CLunaGameDataExtracterDlg::OnBnClickedButtonScriptReload()
{
	if( !m_bLoadCompleted )
	{
		AfxMessageBox( _T("Try it After Loading..!") );
		return;
	}

	if( m_bExtracting )
	{
	AfxMessageBox( _T("Try it After Extracting!") );
		return;
	}

	InitializeWindowFromScript();

	CSimpleLua& settingScript = SIMPLELUAMGR->GetSimpleLua( SETTING_LUA_SCRIPT_FILENAME );
	char testloadchxfilename[MAX_PATH]={0};
	settingScript.GetGlobalTableFieldString( "TestLoadChx", "FileName", testloadchxfilename, sizeof(testloadchxfilename) );
	float fScale = (float)settingScript.GetGlobalTableFieldNumber( "TestLoadChx", "Scale" );
	InsertGXObject( testloadchxfilename, fScale );
}


void CLunaGameDataExtracterDlg::ExtractGameData()
{
	KillTimer( 1 );

	CExtractOptionDlg optdlg;
	if( optdlg.DoModal() != IDOK )
	{
		m_bExtracting = FALSE;
		m_ExtractingThread = NULL;
		OnBnClickedButtonScriptReload();
		return;
	}

	int extractcnt = optdlg.GetTotalExtractDataCnt();
	const int maxPercent = 1000;
	const int unitPercent = extractcnt > 0 ? maxPercent / extractcnt : maxPercent;
	int currentUnitMaxPercent = 0;

	m_ProgressCtrl.SetRange( 0, maxPercent );
	m_ProgressCtrl.SetPos( 0 );

	// 2D Item Icon Image Extract
	if( optdlg.m_b2DItemIconImageCheckBox )
	{
		BOOL rt = Extract2DItemIconImage( currentUnitMaxPercent, 
			currentUnitMaxPercent+unitPercent, 
			optdlg.m_b2DItemIconImageALLCheckBox, 
			optdlg.m_dw2DItemIconImageRangeMin, 
			optdlg.m_dw2DItemIconImageRangeMax );

		if( rt == FALSE )
		{
			m_bExtracting = FALSE;
			m_ExtractingThread = NULL;
			OnBnClickedButtonScriptReload();
			return;
		}

		currentUnitMaxPercent += unitPercent;
		m_ProgressCtrl.SetPos( currentUnitMaxPercent );
	}

    // 2D Active Skill Icon Image Extract
	if( optdlg.m_b2DActiveSkillIconImageCheckBox )
	{
		BOOL rt = Extract2DActiveSkillIconImage( currentUnitMaxPercent, 
			currentUnitMaxPercent+unitPercent, 
			optdlg.m_b2DActiveIconImageALLCheckBox, 
			optdlg.m_dw2DActiveIconImageRangeMin, 
			optdlg.m_dw2DActiveIconImageRangeMax );

		if( rt == FALSE )
		{
			m_bExtracting = FALSE;
			m_ExtractingThread = NULL;
			OnBnClickedButtonScriptReload();
			return;
		}

		currentUnitMaxPercent += unitPercent;
		m_ProgressCtrl.SetPos( currentUnitMaxPercent );
	}

	// 2D Buff Skill Icon Image Extract
	if( optdlg.m_b2DBuffSkillIconImageCheckBox )
	{
		BOOL rt = Extract2DBuffSkillIconImage( currentUnitMaxPercent, 
			currentUnitMaxPercent+unitPercent, 
			optdlg.m_b2DBuffIconImageALLCheckBox, 
			optdlg.m_dw2DBuffIconImageRangeMin, 
			optdlg.m_dw2DBuffIconImageRangeMax );

		if( rt == FALSE )
		{
			m_bExtracting = FALSE;
			m_ExtractingThread = NULL;
			OnBnClickedButtonScriptReload();
			return;
		}

		currentUnitMaxPercent += unitPercent;
		m_ProgressCtrl.SetPos( currentUnitMaxPercent );
	}

	// 3D Monster Image Extract
	if( optdlg.m_b3DMonsterImageCheckBox )
	{
		BOOL rt = Extract3DMonsterImage( currentUnitMaxPercent
			, currentUnitMaxPercent+unitPercent, 
			optdlg.m_b3DMonsterImageCheckBoxALL, 
			optdlg.m_dw3DMonsterImageRangeMin, 
			optdlg.m_dw3DMonsterImageRangeMax);

		if( rt == FALSE )
		{
			m_bExtracting = FALSE;
			m_ExtractingThread = NULL;
			OnBnClickedButtonScriptReload();
			return;
		}

		currentUnitMaxPercent += unitPercent;
		m_ProgressCtrl.SetPos( currentUnitMaxPercent );
	}

	if( optdlg.m_bExtractItemInfo )
	{
		ExtractItemData( currentUnitMaxPercent, 
			currentUnitMaxPercent+unitPercent,
			optdlg.m_bExtractFileText,
			optdlg.m_bExtractFileExcel );

		currentUnitMaxPercent += unitPercent;
		m_ProgressCtrl.SetPos( currentUnitMaxPercent );
	}

	if( optdlg.m_bExtractMonsterInfo )
	{
		ExtractMonsterData( currentUnitMaxPercent, 
			currentUnitMaxPercent+unitPercent,
			optdlg.m_bExtractFileText,
			optdlg.m_bExtractFileExcel );

		currentUnitMaxPercent += unitPercent;
		m_ProgressCtrl.SetPos( currentUnitMaxPercent );
	}

	if( optdlg.m_bExtractSkillInfo )
	{
		ExtractEachClassSkillData( currentUnitMaxPercent, 
			currentUnitMaxPercent+unitPercent,
			optdlg.m_bExtractFileText,
			optdlg.m_bExtractFileExcel );

		currentUnitMaxPercent += unitPercent;
		m_ProgressCtrl.SetPos( currentUnitMaxPercent );
	}

	if( optdlg.m_bExtractQuestInfo )
	{
		ExtractQuestScriptData( currentUnitMaxPercent, 
			currentUnitMaxPercent+unitPercent,
			optdlg.m_bExtractFileText,
			optdlg.m_bExtractFileExcel );

		currentUnitMaxPercent += unitPercent;
		m_ProgressCtrl.SetPos( currentUnitMaxPercent );
	}


	m_ProgressCtrl.SetPos( maxPercent );
	m_ProgressStatusStaticCtrl.SetWindowText( _T(" Extract Complete.. ") );

	SetTimer( 1, 10, NULL );
	m_bExtracting = FALSE;
	m_ExtractingThread = NULL;
}
void CLunaGameDataExtracterDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect windowRect;
	GetClientRect( &windowRect );

	// Dialog Background 클릭시 Window 이동 시작
	if( windowRect.PtInRect( point ) )
	{
		SendMessage( WM_NCLBUTTONDOWN, HTCAPTION, 0 );
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CLunaGameDataExtracterDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect windowRect;
	GetClientRect( &windowRect );

	// Window 이동 종료
	if( windowRect.PtInRect( point ) )
	{
		SendMessage( WM_NCLBUTTONUP, HTCAPTION, 0 );
	}

	CDialog::OnLButtonUp(nFlags, point);
}

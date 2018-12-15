#include "StdAfx.h"
#include "RegenTool.h"
#include "RegenToolDoc.h"
#include "LoadList.h"
#include "MainFrm.h"
#include "MHMap.h"
#include "../4DyuchiGRX_myself97_util/MToolMouseStatus.h"
#include "Rectangle.h"
#include "RegenToolView.h"
#include ".\regentoolview.h"

const DWORD START_MONSTERINDEX = 100001;

IMPLEMENT_DYNCREATE(CRegenToolView, CFormView)

BEGIN_MESSAGE_MAP(CRegenToolView, CFormView)
	//{{AFX_MSG_MAP(CRegenToolView)
	ON_WM_DRAWITEM()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_CBN_SELCHANGE(IDC_MAPCOMBO, OnSelchangeMapcombo)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_FOG_TOGGLE, OnBnClickedCheckFogToggle)
	ON_BN_CLICKED(IDC_CHECK_MAP_OBJECT_TOGGLE, OnBnClickedCheckMapObjectToggle)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_MENU_OBJECT_ADD, OnMenuObjectAdd)
	ON_COMMAND(ID_MENUM_OBJECT_REMOVE, OnMenumObjectRemove)
	ON_COMMAND(ID_MENU_OBJECT_MOVE, OnMenuObjectMove)
	ON_COMMAND(ID_MENU_OBJECT_CLOSE, OnMenuObjectClose)
	ON_COMMAND(ID_MENU_CAMERA_MOVE, OnMenuCameraMove)
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CRegenToolView::CRegenToolView() :
CFormView(CRegenToolView::IDD),
mRectangle(new CRectangle)
{
	mMouseStatus = NULL;

	m_pFontObject = NULL;
	m_bSelectedObjectMove = FALSE;
	m_bCanUsePopupMenu = FALSE;

	m_bIsMovingCameraSmoothly = FALSE;
	m_bIsMovingCameraY = FALSE;
}

CRegenToolView::~CRegenToolView()
{}

void CRegenToolView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegenToolView)
	DDX_Control(pDX, IDC_MAPCOMBO, m_MapKind);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHECK_FOG_TOGGLE, mFogButton);
	DDX_Control(pDX, IDC_CHECK_MAP_OBJECT_TOGGLE, mMapObjectButton);
}

void CRegenToolView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
		
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return;
	CSheetPageMonster* pMonsterPage = (CSheetPageMonster*)pFrm->GetMonsterPage();
	if( !pMonsterPage )		return;
	
	g_pLoadList.SetMapCombo( m_MapKind );
	g_pLoadList.LoadMonsterList();
	g_pLoadList.SetMonsterCombo( pMonsterPage->GetMonsterListComboBox() );

	SetTimer( 1, 10, 0 );
	SetTimer( 2, 10, 0 );
}
#ifdef _DEBUG
CRegenToolDoc* CRegenToolView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRegenToolDoc)));
	return (CRegenToolDoc*)m_pDocument;
}
#endif //_DEBUG

void CRegenToolView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd* const window = GetDlgItem(IDC_MAPIMAGE);

	if(window)
	{
		CRect rect;
		window->GetClientRect(
			rect);

		if(rect.PtInRect(point))
		{
			window->SetFocus();
		}
	}

	// 이동 처리 중인 경우
	if( mSelectedHandleContainer.empty() == FALSE && m_bSelectedObjectMove )
	{
		EndMoveOfSelectedObjects();
	}
	else
	{
		SetSelectedObjectHandle( GetSelectedObjectHandle(point) );
	}

	{
		const CPoint clickedPosition = CalcSelectPoint(point);
		mRectangle.get()->SetRect(
			CRect(clickedPosition, clickedPosition));
	}

	CFormView::OnLButtonDown(nFlags, point);
}

void CRegenToolView::OnLButtonUp(UINT nFlags, CPoint point)
{
	SelectObject(
		mRectangle.get()->GetRect());

	mRectangle.get()->SetRect(
		CRect(0, 0, 0, 0));

	CFormView::OnLButtonUp(nFlags, point);
}

void CRegenToolView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if( GetSelectedObjectHandle() != NULL )
		m_bSelectedObjectMove = TRUE;

	CFormView::OnLButtonDblClk(nFlags, point);
}

void CRegenToolView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWnd* const window = GetDlgItem(IDC_MAPIMAGE);

	if(window)
	{
		CRect rect;
		window->GetClientRect(
			rect);

		if(rect.PtInRect(point))
		{
			window->SetFocus();
		}
	}

	m_bCanUsePopupMenu = TRUE;
	m_RBtnDownPos = point;

	CFormView::OnRButtonDown(nFlags, point);
}

void CRegenToolView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( m_bCanUsePopupMenu )
	{
		if( GetSelectedObjectHandle() == NULL )
		{
			SetSelectedObjectHandle( GetSelectedObjectHandle( point ) );
		}

		m_bCanUsePopupMenu = FALSE;
		PutPopup( point );
	}

	CFormView::OnRButtonUp(nFlags, point);
}

void CRegenToolView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bCanUsePopupMenu )
	{
		CPoint dist = m_RBtnDownPos - point;
		if( sqrt( pow( dist.x, 2 ) + pow( dist.y, 2 ) ) > 10 )
			m_bCanUsePopupMenu = FALSE;
	}

	PutPosition(point);

	if( mSelectedHandleContainer.empty() == FALSE && m_bSelectedObjectMove )
	{
		MovingOfSelectedObjects( point );
	}
	else if(mMouseStatus)
	{
		// 왼쪽클릭으로 카메라 Rotate 가능하게 한다.
		if( nFlags & MK_LBUTTON )
		{	
//			nFlags = MK_RBUTTON;
		}
		CPoint movePoint( point.x - m_RBtnDownPos.x, point.y - m_RBtnDownPos.y );
		mMouseStatus->MoveMouse((DWORD)nFlags, movePoint.x, movePoint.y);
	}

	if(FALSE == mRectangle.get()->IsEmpty())
	{
		mRectangle.get()->SetBottomRight(
			CalcSelectPoint(point));
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CRegenToolView::OnSelchangeMapcombo()
{	
	CString mapName;
	m_MapKind.GetLBText(
		m_MapKind.GetCurSel(),
		mapName);

	AfxGetMainWnd()->SetWindowText(
		mapName);

	int i = 0;
	const CString mapType = mapName.Tokenize(
		" ",
		i);

	if(0 == _ttoi(mapType))
	{
		m_MapKind.SetCurSel(0);
		return;
	}
	else if(IDCANCEL == MessageBox(
		_T("맵을 읽을까요?"),
		_T("알림"),
		MB_OKCANCEL))
	{
		AfxGetMainWnd()->SetWindowText(
			"제목 없음");
		m_MapKind.SetCurSel(0);
		return;
	}

	DIRECTORYMGR->SetLoadMode(
		eLM_Root);
	MAP->InitMap(
		_ttoi(mapType));
	
	{
		CWnd* const window = GetDlgItem(IDC_MAPIMAGE);

		if(window)
		{
			window->SetFocus();
		}
	}

	{
		CWnd* const window = GetDlgItem(IDC_MONSTER_LOAD_BUTTON);

		if(window)
		{
			window->EnableWindow(TRUE);
		}
	}

	SetDefaultCamera();

	DIRECTORYMGR->SetLoadMode(
		eLM_Root);
	TCHAR fileName[MAX_PATH] = {0};
	_stprintf(
		fileName,
		_T("system\\resource\\Monster_%02d.bin"),
		_ttoi(mapType));

	CRegenToolDoc* pDoc = (CRegenToolDoc*)GetDocument();
	pDoc->LoadRegenScript( fileName );
	mMapObjectButton.SetCheck(TRUE);

	SetSelectedObjectHandle( NULL );
}

void CRegenToolView::SetDefaultCamera()
{
	CAMERA_DESC cameraDescription = {0};
	g_pExecutive->GetGeometry()->GetCameraDesc(
		&cameraDescription,
		0);
	cameraDescription.fXRot = -0.4f;
	cameraDescription.fYRot = 0.8f;
	cameraDescription.fFovX = 0.78f;
	cameraDescription.fFovY = 0.53f;
	cameraDescription.fAspect = 0.66f;

	{
		VECTOR3& position = cameraDescription.v3From;
		position.x = -12278;
		position.y = 22439;
		position.z = -11765;
	}

	{
		VECTOR3& position = cameraDescription.v3To;
		position.x = -11614;
		position.y = 22032;
		position.z = -11137;
	}

	{
		VECTOR3& position = cameraDescription.v3Up;
		position.x = 0.29f;
		position.y = 0.9f;
		position.z = 0.27f;
	}

	{
		VECTOR3& position = cameraDescription.v3EyeDir;
		position.x = 663;
		position.y = -406;
		position.z = 627;
	}

	g_pExecutive->GetGeometry()->SetCameraDesc(
		&cameraDescription,
		0);
}

void CRegenToolView::InitEngine()
{
	CRect clientRect;
	GetClientRect( &clientRect );
	clientRect.DeflateRect( 5, 40, 7, 10 );
	CWnd * wnd = GetDlgItem(IDC_MAPIMAGE);
	wnd->MoveWindow( &clientRect );

	CoInitialize(NULL);
	char temp[256] = {0};
	GetCurrentDirectory(256,temp);
	
	HMODULE g_hExecutiveHandle = LoadLibrary("Executive.dll");
		
	// DllCreateInstance 는 3개의 엔진 DLL이 모두 자신의 것을 가지고 있다.각 엔진 COM의 인터페이스를 
	// 얻게 해주는 함수이다. 일단 executive의 것만 얻어내면 다른건 executive초기화 시에 자동으로 얻을수 있다.
	CREATE_INSTANCE_FUNC        pFunc;
	pFunc = (CREATE_INSTANCE_FUNC)GetProcAddress(g_hExecutiveHandle,"DllCreateInstance");
	HRESULT hr = pFunc((void**)&g_pExecutive);
	if (hr != S_OK)
	{		
		MessageBox("Executive생성 실패","Error",MB_OK);
		return;
	}	

	PACKFILE_NAME_TABLE table[] = {
		{"data/3dData/map.pak", 0},
		{"data/3dData/monster.pak", 0},
		{"data/3dData/npc.pak", 0},
	};
	// geometry DLL의 이름, renderer DLL의 이름을 넣어주면 executive가 알아서 생성,초기화해준다.
	g_pExecutive->InitializeFileStorageWithoutRegistry(
		"FileStorage.dll",
		35000,
		10000,
		256,
		FILE_ACCESS_METHOD_FILE_OR_PACK,			
		table,
		sizeof(table) / sizeof(*table));	

	g_pExecutive->InitializeWithoutRegistry(
		"Geometry.dll",
		"Renderer.dll",
		wnd->GetSafeHwnd(),
		0,
		160000,
		4096,
		512,
		32,
		32,
		0);

	// 마우스로 카메라를 제어하기 위한 헬퍼클래스 초기화.
	mMouseStatus = new MToolMouseStatus;
	mMouseStatus->Initialize( g_pExecutive->GetGeometry());
	
	// executive는 뷰포트 0번을 사용할 것이다.		
	g_pExecutive->SetViewport(0);
	
	// 카메라 초기화
	VECTOR3	v3From;
	v3From.x = 0.0f;
	v3From.y = 1000.0f;
	v3From.z = 0.0f;
	
	g_pExecutive->GetGeometry()->ResetCamera(&v3From,DEFAULT_NEAR,DEFAULT_FAR,DEFAULT_FOV,0);
	// 깜깜하니 캐릭터를 잘 알아볼수가 없다.빛을 주자.
	DIRECTIONAL_LIGHT_DESC	dirLight;
	dirLight.v3Dir.x = 0.5f;
	dirLight.v3Dir.y = -1.0f;
	dirLight.v3Dir.z = 0.5f;
	dirLight.dwSpecular = 0xffffffff;
	dirLight.dwDiffuse = 0xfffffff;
	dirLight.dwAmbient = 0xff202020;
	
	g_pExecutive->GetGeometry()->SetDirectionalLight(
		&dirLight,
		0);
	g_pExecutive->GetGeometry()->EnableDirectionalLight(
		0);	
	g_pExecutive->GetGeometry()->SetShadowFlag(
		ENABLE_PROJECTION_SHADOW |	ENABLE_PROJECTION_TEXMAP);
	g_pExecutive->Run(
		0xff0000ff,
		0,
		0,
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
	lstrcpy(font.lfFaceName,"2002_EYA");
	m_pFontObject = g_pExecutive->GetRenderer()->CreateFontObject( &font, D3DX_FONT );//SS3D_FONT);//D3DX_FONT);

	m_Font.CreateFontIndirect( &font );
}

void CRegenToolView::InsertMonster(VECTOR3* TargetPos)
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return;
	CSheetPageMonster* pMonsterPage = (CSheetPageMonster*)pFrm->GetMonsterPage();
	if( !pMonsterPage )		return;

	// 선택된 Group Index를 가져온다.
	DWORD dwGroup = pMonsterPage->GetSelectedGroupIndexFromTree();
	
	if( dwGroup == 0 )
	{
		MessageBox(
			"몬스터를 추가하려면 소속될 그룹이 있어야합니다.\n그룹을 선택하거나 추가하세요",
			"오류",
			MB_OK);		
		return;
	}

	CRegenToolDoc* pDoc = (CRegenToolDoc*)GetDocument();
	Script::Monster* pMonsterInfo = pDoc->AddMonster( dwGroup );
	if( !pMonsterInfo )
		return;

	DIRECTORYMGR->SetLoadMode(eLM_Monster);

	CRect rect;

	CString textMonster;
	CComboBox& monsterListComboBox = pMonsterPage->GetMonsterListComboBox();
	monsterListComboBox.GetLBText( monsterListComboBox.GetCurSel(), textMonster );
	int start = 0;
	CString textMonsterKind = textMonster.Tokenize( _T(" "), start );
	const BASE_MONSTER_LIST& baseMonsterInfo = g_pLoadList.GetMonster( _ttoi(textMonsterKind) );
	GXOBJECT_HANDLE handle  = g_pExecutive->CreateGXObject( LPSTR(baseMonsterInfo.ChxName), MHPlayerPROC, 0, 0);
	if(!handle)
		handle = g_pExecutive->CreateGXObject("data/3dData/monster/L008.chx", MHPlayerPROC,0,0);
	g_pExecutive->GXOEnableHFieldApply(handle);
	g_pExecutive->GXOSetPosition( handle, TargetPos, FALSE );
	VECTOR3 vecScale = { baseMonsterInfo.Scale, baseMonsterInfo.Scale, baseMonsterInfo.Scale };
	g_pExecutive->GXOSetScale( handle, &vecScale );


	pMonsterInfo->mObjectKind = EObjectKind( baseMonsterInfo.ObjectKind );
	pMonsterInfo->mMonsterKind = baseMonsterInfo.MonsterKind;
	pMonsterInfo->mPosition = *TargetPos;
	pMonsterInfo->hGXO = handle;

	// Tree에 정보를 추가한다.
	HTREEITEM hTreeItem = pMonsterPage->AddMonsterInTree( dwGroup, baseMonsterInfo.Name, handle );
	pMonsterPage->SelectTreeItem( hTreeItem );

	DIRECTORYMGR->SetLoadMode(eLM_Root);
	return;
}

CPoint CRegenToolView::CalcSelectPoint(CPoint point)
{
	CWnd * wnd = GetDlgItem(IDC_MAPIMAGE);
	CRect wrect, crect, prect;
	CPoint ViewMousePos(0, 0);
	//	POINT pos;
	wnd->GetWindowRect(&wrect);	// 전체 윈도우 기준에서의 ImageView Rect
	wnd->GetClientRect(&crect);	// 자기자신 기준에서의 ImageView Rect
	CWnd* pWnd = wnd->GetParent();
	pWnd->GetWindowRect(&prect); // 자기 부모FromView기준에서의 ImageView Rect 
	
	// 전체 윈도 기준에서 FromView기준을 빼면 FormView에서 자기자신까지의 사이값이 나온다.
	// 이값을 ImageBox의 시작 좌표로 설정하고 마우스 좌표에서 이값을 빼면 정확한 포인트가 나온다.
	CRect ImageBox;
	ImageBox.left = wrect.left - prect.left; 
	ImageBox.top = wrect.top - prect.top;
	ImageBox.SetRect(ImageBox.TopLeft(), wrect.BottomRight());
	ViewMousePos.x = point.x - ImageBox.left;
	ViewMousePos.y = point.y - ImageBox.top;

	g_ScreenInfo.dwWidth = crect.right;
	g_ScreenInfo.dwHeight = crect.bottom;
	if(ViewMousePos.x > crect.left && ViewMousePos.y > crect.top &&
		ViewMousePos.x < crect.right && ViewMousePos.y < crect.bottom)
		return ViewMousePos;
	return CPoint(0, 0);
}

GXOBJECT_HANDLE CRegenToolView::GetSelectedObjectHandle(CPoint point)
{
	CPoint ViewMousePos = CalcSelectPoint(point);
	const int maxPickDesc = 10;
	PICK_GXOBJECT_DESC gxoDesc[maxPickDesc] = {0};
	DWORD dwPickNum = g_pExecutive->GXOGetMultipleObjectWithScreenCoord(
		gxoDesc,
		_countof(gxoDesc),
		&ViewMousePos,
		0,
		PICK_TYPE_DEFAULT /* PICK_TYPE_DEFAULT*/ /*PICK_TYPE_PER_FACE*//*PICK_TYPE_PER_COLLISION_MESH*/);

	if(0 == dwPickNum)
	{
		mClickedPositionOnField = GetFieldPositionByCursor( point );
		return 0;
	}

	float minimumDistance = FLT_MAX;
	GXOBJECT_HANDLE handle = 0;

	for(DWORD i = dwPickNum; i-- > 0;)
	{
		const PICK_GXOBJECT_DESC& description = gxoDesc[i];

		if(minimumDistance > description.fDist)
		{
			minimumDistance = description.fDist;
			handle = description.gxo;
		}
	}

	return handle;
}

void CRegenToolView::OnFileSave() 
{
	CString text;
	m_MapKind.GetLBText(
		m_MapKind.GetCurSel(),
		text);
	int start = 0;
	CString textMapType = text.Tokenize(
		" ",
		start);

	if(0 == _ttoi(textMapType))
	{
		return;
	}

	TCHAR path[MAX_PATH] = {0};
	GetCurrentDirectory(
		sizeof(path) / sizeof(*path),
		path);

	CString fileName;
	fileName.Format(
		_T("%s\\system\\resource\\Monster_%02d.bin"),
		path,
		_ttoi(textMapType));
	GetDocument()->OnSaveDocument(
		fileName);
}

void CRegenToolView::OnDestroy() 
{
	CFormView::OnDestroy();

	SAFE_DELETE(mMouseStatus);

	MAP->Release();
	SAFE_RELEASE( m_pFontObject );

	if( g_pExecutive )
	{
		g_pExecutive->DeleteAllGXEventTriggers();
		g_pExecutive->DeleteAllGXLights();
		g_pExecutive->DeleteAllGXObjects();
		g_pExecutive->UnloadAllPreLoadedGXObject(0);
		g_pExecutive->DeleteGXMap(NULL);
		g_pExecutive->Release();
		g_pExecutive = NULL;
	}

	CoFreeUnusedLibraries();
	CoUninitialize();
}
BOOL CRegenToolView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CAMERA_DESC cameraDescription = {0};
	g_pExecutive->GetGeometry()->GetCameraDesc(
		&cameraDescription,
		0);
	VECTOR3 unitVector = cameraDescription.v3EyeDir;
	const float sign = (float)(0 > zDelta ? -1 : 1);
	const float delta = (float)(pow(zDelta, 2) * sign / 30);
	const float length = sqrt(
		pow(unitVector.x, 2) +
		pow(unitVector.y, 2) +
		pow(unitVector.z, 2));
	VECTOR3 position = unitVector / length * delta;

	g_pExecutive->GetGeometry()->MoveCamera(
		&position,
		0);

	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

void CRegenToolView::OnBnClickedButton1()
{
	CString mapName;
	m_MapKind.GetLBText(
		m_MapKind.GetCurSel(),
		mapName);

	if(0 == _ttoi(mapName))
	{
		return;
	}

	CFileDialog fileDialog(
		TRUE,
		_T("bin"),
		_T("*"),
		OFN_EXPLORER,
		_T("*.bin"));
	fileDialog.m_ofn.lpstrInitialDir = _T("D:\\study\\Luna\\debug\\System\\Resource\\") ;

	if(IDCANCEL == fileDialog.DoModal())
	{
		return;
	}

	CRegenToolDoc* pDoc = (CRegenToolDoc*)GetDocument();
	pDoc->LoadRegenScript(fileDialog.GetPathName());
	SetSelectedObjectHandle( NULL );
}


void CRegenToolView::OnTimer(UINT nIDEvent)
{
	switch( nIDEvent )
	{
	case 1:
		{
			// 카메라 이동처리
			MoveCameraSmoothly();

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
			g_pExecutive->GetGeometry()->GetCameraDesc( &Desc, 0);
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
				g_pExecutive->GetGeometry()->MoveCamera( &vecLook, 0 );
			}
			// s, S
			if( GetAsyncKeyState( 0x53 ) )
			{
				vecLook.y = 0;
				vecLook = vecLook * -1.f;
				g_pExecutive->GetGeometry()->MoveCamera( &vecLook, 0 );
			}
			// d, D
			if( GetAsyncKeyState( 0x44 ) )
			{
				g_pExecutive->GetGeometry()->MoveCamera( &vecRight, 0 );
			}
			// a, A
			if( GetAsyncKeyState( 0x41 ) )
			{
				vecRight = vecRight * -1.f;
				g_pExecutive->GetGeometry()->MoveCamera( &vecRight, 0 );
			}

			// 상하 이동
			vecUp.x = vecUp.z = 0;
			vecUp.y = 1;
			vecUp = vecUp * fMoveStep;
			//(51) Q key
			if( GetAsyncKeyState( 0x51 ) )
			{
				vecUp = vecUp * -1.f;
				g_pExecutive->GetGeometry()->MoveCamera( &vecUp, 0 );
			}
			//(45) E key
			else if( GetAsyncKeyState( 0x45 ) )
			{
				g_pExecutive->GetGeometry()->MoveCamera( &vecUp, 0 );
			}

			
			break;
		}
	case 2:
		{
			if(0 == g_pExecutive)
			{
				break;
			}

			g_pExecutive->GetRenderer()->BeginPerformanceAnalyze();

			MAP->Process(0);
			g_pExecutive->Run(
				0xff0000ff,
				0,
				AfterRenderFunc,
				MAP->IsHaveSky() ? BEGIN_RENDER_FLAG_DONOT_CLEAR_FRAMEBUFFER : 0);
			g_pExecutive->GetRenderer()->EndPerformanceAnalyze();
			g_pExecutive->GetGeometry()->Present(0);
			break;
		}
	}

	CFormView::OnTimer(nIDEvent);
}

void CRegenToolView::OnBnClickedCheckFogToggle()
{
	if(mFogButton.GetCheck())
	{
		g_pExecutive->GetGeometry()->EnableFog(0);
	}
	else
	{
		g_pExecutive->GetGeometry()->DisableFog(0);
	}
}

void CRegenToolView::PutPosition(CPoint& point)
{
	const CPoint selectPoint = CalcSelectPoint(point);
	VECTOR3* const pickedPosition = GetPickedPosition(
		selectPoint.x,
		selectPoint.y);

	CString text;

	if(pickedPosition)
	{
		text.Format(
			_T("(%0.1f, %0.1f)"),
			pickedPosition->x,
			pickedPosition->z);	
	}
	else
	{
		text = _T("잘못된 지점");
	}

	CMainFrame* const mainFrame = (CMainFrame*)AfxGetMainWnd();

	if(0 == mainFrame)
	{
		return;
	}

	mainFrame->SetStatusText(
		text);
}

void CRegenToolView::OnBnClickedCheckMapObjectToggle()
{
	const DWORD alphaValue = (TRUE == mMapObjectButton.GetCheck() ? 255 : 0);
	
	const size_t maxObjectSize = 1000;
	GXOBJECT_HANDLE handleContainer[maxObjectSize] = {0};
	const size_t objectCount = g_pExecutive->GXOGetAllObjectsWitLoadMapScript(
		handleContainer,
		sizeof(handleContainer) / sizeof(*handleContainer));

	for(size_t i = 0; i < objectCount; ++i)
	{
		g_pExecutive->SetAlphaFlag(
			handleContainer[i],
			alphaValue);
	}
}
void CRegenToolView::OnFileSaveAs()
{
	CString text;
	m_MapKind.GetLBText(
		m_MapKind.GetCurSel(),
		text);
	int start = 0;
	CString textMapType = text.Tokenize(
		_T(" "),
		start);

	if(0 == _ttoi(textMapType))
	{
		return;
	}

	CString fileName;
	fileName.Format(
		_T("Monster_%02d"),
		_ttoi(textMapType));

	CFileDialog fileDialog(
		FALSE,
		"bin",
		fileName,
		OFN_HIDEREADONLY, 
		"몬스터 리젠 파일(*.bin)");	

	if(IDCANCEL == fileDialog.DoModal())
	{
		return;
	}

	GetDocument()->OnSaveDocument(
		fileDialog.GetPathName());
}

void CRegenToolView::RenderFont(LONG x, LONG y, DWORD dwColor, LPCTSTR str, ...)
{
	char txt[MAX_PATH] = {0,};
	va_list vList;
	va_start( vList, str );
	vsprintf( txt, str, vList );
	va_end( vList );

	// 실제 사용한 Text의 Width 계산
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject( &m_Font );
	SIZE size;
	GetTextExtentPoint32( dc.m_hDC, txt, strlen(txt), &size );
	dc.SelectObject( pOldFont );

	for(int row = y - 1; row <= y + 1; ++row)
	{
		for(int column = x - 1; column <= x + 1; ++column)
		{
			CRect rect;
			rect.left = column;
			rect.top = row;
			rect.right = column + size.cx;
			rect.bottom = row + size.cy;

			g_pExecutive->GetGeometry()->RenderFont(
				m_pFontObject,
				LPTSTR(txt),
				_tcslen(txt),
				&rect,
				RGBA_MAKE(0, 0, 0, 255),
				CHAR_CODE_TYPE_ASCII,
				0,
				0);
		}
	}

	g_pExecutive->GetGeometry()->RenderFont(
		m_pFontObject,
		LPTSTR(txt),
		_tcslen(txt),
		CRect(x, y, x + size.cx, y + size.cy),
		dwColor,
		CHAR_CODE_TYPE_ASCII,
		0,
		0);
}

void CRegenToolView::RenderSelectedObjectInfo()
{
	if(0 == m_pFontObject)
	{
		return;
	}

	for(HandleContainer::const_iterator iterator = mSelectedHandleContainer.begin();
		mSelectedHandleContainer.end() != iterator;
		++iterator)
	{
		const GXOBJECT_HANDLE handle = *iterator;
		const Script::Monster* const monster = GetDocument()->GetMonsterInfo(
			handle);

		if(0 == monster)
		{
			continue;
		}

		const Script* const script = GetDocument()->GetScriptInfoFromGXOHandle(
			handle);

		if(0 == script)
		{
			continue;
		}

		VECTOR3 position = {0};
		g_pExecutive->GXOGetPosition(
			handle,
			&position);
		GetScreenXYFromXYZ(
			g_pExecutive->GetGeometry(),
			0,
			&position,
			&position);

		const float fontHeight = 14.0f;
		position.x *= g_ScreenInfo.dwWidth - 100;
		position.y *= g_ScreenInfo.dwHeight - fontHeight;

		const BASE_MONSTER_LIST& baseMonsterList = g_pLoadList.GetMonster(
			monster->mMonsterKind);
		RenderFont(
			LONG(position.x),
			LONG(position.y),
			RGBA_MAKE(255, 255, 255, 255),
			"#%u %s",
			script->mGroupIndex,
			baseMonsterList.Name);
		RenderFont(
			LONG(position.x),
			LONG(position.y + fontHeight),
			RGBA_MAKE(255, 255, 255, 255),
			"%s",
			monster->mFiniteStateMachine.IsEmpty() ? " " : monster->mFiniteStateMachine);
	}

	if(1 == mSelectedHandleContainer.size())
	{
		const GXOBJECT_HANDLE handle = *(mSelectedHandleContainer.begin());
		const Script* const script = GetDocument()->GetScriptInfoFromGXOHandle(
			handle);

		if(0 == script)
		{
			return;
		}

		for(Script::MonsterList::const_iterator iterator = script->mMonsterList.begin();
			script->mMonsterList.end() != iterator;
			++iterator)
		{
			const Script::Monster& monster = *iterator;

			if(monster.hGXO == handle)
			{
				continue;
			}

			VECTOR3 position = {0};
			g_pExecutive->GXOGetPosition(
				monster.hGXO,
				&position);
			GetScreenXYFromXYZ(
				g_pExecutive->GetGeometry(),
				0,
				&position,
				&position);

			const float fontHeight = 14.0f;
			position.x *= g_ScreenInfo.dwWidth - 100;
			position.y *= g_ScreenInfo.dwHeight - fontHeight;

			const BASE_MONSTER_LIST& baseMonsterInfo = g_pLoadList.GetMonster(
				monster.mMonsterKind);
			RenderFont(
				LONG(position.x),
				LONG(position.y),
				RGBA_MAKE(65, 175, 255, 255),
				"#%u %s",
				script->mGroupIndex,
				baseMonsterInfo.Name);
			RenderFont(
				LONG(position.x),
				LONG(position.y + fontHeight),
				RGBA_MAKE(255, 255, 255, 255),
				"%s",
				monster.mFiniteStateMachine.IsEmpty() ? " " : monster.mFiniteStateMachine);
		}
	}
}

void CRegenToolView::PutPopup( CPoint& point )
{
	CWnd* const window = GetDlgItem(
		IDC_MAPIMAGE);

	if(0 == window)
	{
		return;
	}

	POINT cursorPosition = {0};
	GetCursorPos(
		&cursorPosition);
	CRect windowRect;
	window->GetWindowRect( &windowRect );

	if(FALSE == windowRect.PtInRect(
		cursorPosition))
	{
		return;
	}

	CMenu menu;
	menu.LoadMenu(
		IDR_MENU_OBJECT);
	CMenu* const subMenu = menu.GetSubMenu(
		0);

	if(0 == subMenu)
	{
		return;
	}

	CString name;
	BOOL isClickedGround = (NULL == GetSelectedObjectHandle());

	if(isClickedGround)
	{
		const CPoint viewMousePosition = CalcSelectPoint(
			point );
		const VECTOR3* const position = GetPickedPosition(
			viewMousePosition.x,
			viewMousePosition.y);

		const bool isClickedOnSky = (0 == position);

		if(isClickedOnSky)
		{
			return;
		}

		m_vecPopupMenuPos = *position;

		name.Format(
			_T("(%4.2f, %4.2f)"),
			position->x,
			position->z);

		subMenu->EnableMenuItem( ID_MENUM_OBJECT_REMOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		subMenu->EnableMenuItem( ID_MENU_OBJECT_MOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}
	else
	{
		CRegenToolDoc* pDoc = (CRegenToolDoc*)GetDocument();
		Script::Monster* pMonsterInfo = pDoc->GetMonsterInfo( GetSelectedObjectHandle() );
		if( !pMonsterInfo )
			return;

		const BASE_MONSTER_LIST& baseMonsterInfo = g_pLoadList.GetMonster( pMonsterInfo->mMonsterKind );
		name.Format( _T("%s"), baseMonsterInfo.Name );

		subMenu->EnableMenuItem( ID_MENU_OBJECT_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		subMenu->EnableMenuItem( ID_MENU_CAMERA_MOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}

	subMenu->ModifyMenu(
		ID_MENU_OBJECT_NAME,
		MF_BYCOMMAND,
		ID_MENU_OBJECT_NAME,
		name);
	subMenu->TrackPopupMenu(
		TPM_LEFTALIGN | TPM_LEFTBUTTON,
		cursorPosition.x,
		cursorPosition.y,
		this);
	menu.DestroyMenu();
}

VECTOR3 CRegenToolView::GetFieldPositionByCursor( CPoint& point )
{
	const CPoint viewMousePosition = CalcSelectPoint(
		point);
	const VECTOR3* const pickedPosition = GetPickedPosition(
		viewMousePosition.x,
		viewMousePosition.y);

	if(0 == pickedPosition)
	{
		return VECTOR3();
	}

	return *pickedPosition;
}

void CRegenToolView::OnMenuObjectAdd()
{
	const CPoint viewMousePosition = CalcSelectPoint(
		m_RBtnDownPos);
	VECTOR3* const position = GetPickedPosition(
		viewMousePosition.x,
		viewMousePosition.y);

	if(MAP->CollisonCheck(
		position,
		position,
		position))
	{
		return;
	}

	InsertMonster(position);
}

void CRegenToolView::OnMenumObjectRemove()
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return;
	CRegenToolDoc* pDoc = (CRegenToolDoc*)GetDocument();
	if( !pDoc )				return;
	CSheetPageMonster* pMonsterPage = (CSheetPageMonster*)pFrm->GetMonsterPage();
	if( !pMonsterPage )		return;

	GXOBJECT_HANDLE handle = GetSelectedObjectHandle();
	// 몬스터 제거
	if( !pDoc->DeleteMonster( handle ) )
		return;

	pMonsterPage->DeleteMonsterInTree( handle );
	g_pExecutive->DeleteGXObject( handle );
}

void CRegenToolView::OnMenuObjectClose()
{
}

void CRegenToolView::OnMenuObjectMove()
{
	m_bSelectedObjectMove = TRUE;

	StartMoveOfSelectedObjects();
}

void CRegenToolView::OnMenuCameraMove()
{
	SetCameraPos( m_vecPopupMenuPos, TRUE );
}

void CRegenToolView::AfterRender()
{
	RenderSelectedObjectInfo();
	mRectangle.get()->Draw();
}

void CRegenToolView::SetSelectedObjectHandle( GXOBJECT_HANDLE handle )
{
	const GXOBJECT_HANDLE selectedObjectHandle = GetSelectedObjectHandle();
	if( handle == NULL && selectedObjectHandle == NULL )
		return;

	CRegenToolDoc* pDoc = (CRegenToolDoc*)GetDocument();
	if( !pDoc )				return;

	if( GetSelectedObjectHandle() != NULL )
	{
		// 이전 선택된 Object 선택 해제

		for( HandleContainer::const_iterator selectedhandleiter = mSelectedHandleContainer.begin() ; selectedhandleiter != mSelectedHandleContainer.end() ; ++selectedhandleiter )
		{
			const GXOBJECT_HANDLE& selectedhandle = *selectedhandleiter;

			Script::Monster* pMonsterInfo = pDoc->GetMonsterInfo( selectedhandle );
			if( !pMonsterInfo )		return;
			const BASE_MONSTER_LIST& baseMonsterInfo = g_pLoadList.GetMonster( pMonsterInfo->mMonsterKind );
			g_pExecutive->SetAlphaFlag( selectedhandle, 255 );
			VECTOR3 vecScale = { baseMonsterInfo.Scale, baseMonsterInfo.Scale, baseMonsterInfo.Scale };
			g_pExecutive->GXOSetScale( selectedhandle, &vecScale );
		}

		mSelectedHandleContainer.clear();
		m_SelectedObjectMoveInfo.clear();
	}

	// Object 선택
	Script::Monster* pMonsterInfo = pDoc->GetMonsterInfo( handle );
	if( !pMonsterInfo )		return;
	const BASE_MONSTER_LIST& baseMonsterInfo = g_pLoadList.GetMonster( pMonsterInfo->mMonsterKind );

	g_pExecutive->SetAlphaFlag( handle, 125 );
	VECTOR3 vecScale = { baseMonsterInfo.Scale*1.3f, baseMonsterInfo.Scale*1.3f, baseMonsterInfo.Scale*1.3f };
	g_pExecutive->GXOSetScale( handle, &vecScale );

	// Tree Update
	// Tree의 선택된 Object를 변경하는 경우
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return;
	CSheetPageMonster* pMonsterPage = (CSheetPageMonster*)pFrm->GetMonsterPage();
	if( !pMonsterPage )		return;

	pMonsterPage->SelectMonster( handle );

	CPoint movepoint(0,0);
	mSelectedHandleContainer.insert( handle );
	m_SelectedObjectMoveInfo.insert( SelectedHandleMoveInfo::value_type( handle, movepoint ) );
}

void CRegenToolView::SetCameraPos( GXOBJECT_HANDLE handle, BOOL bMoveCameraY, BOOL bMoveDirectly )
{
	VECTOR3 vecMonsterPos;
	g_pExecutive->GXOGetPosition( handle, &vecMonsterPos );

	SetCameraPos( vecMonsterPos, bMoveCameraY, bMoveDirectly );
}

void CRegenToolView::SetCameraPos( VECTOR3 vecTargetPos, BOOL bMoveCameraY, BOOL bMoveDirectly )
{
	CAMERA_DESC Desc;
	g_pExecutive->GetGeometry()->GetCameraDesc( &Desc, 0 );

	if( bMoveCameraY )
	{
		vecTargetPos = vecTargetPos - Desc.v3EyeDir;
	}
	
	if( bMoveDirectly )
	{
		if( bMoveCameraY )
		{
			g_pExecutive->GetGeometry()->SetCameraPos( &vecTargetPos, 0 );
		}
		else
		{
			// xz평면 이동 Vector를 구한다
			// 지정된 좌표의 카메라 Look에 해당되는 좌표를 찾는다.
			const float fRatio = (Desc.v3From.y - vecTargetPos.y) / Desc.v3EyeDir.y;
			VECTOR3 vecMovePos = vecTargetPos + Desc.v3EyeDir * fRatio;
			VECTOR3 vecMove = vecMovePos - Desc.v3From;
			vecMove.y = 0;
			g_pExecutive->GetGeometry()->MoveCamera( &vecMove, 0 );
			g_pExecutive->GetGeometry()->MoveCamera( &vecMove, 0 );
		}
	}
	else
	{
		if( bMoveCameraY )
		{
			vecTargetPos = vecTargetPos - Desc.v3EyeDir;
		}

		m_vecMoveTargetPos = vecTargetPos;
		m_bIsMovingCameraSmoothly = TRUE;
		m_bIsMovingCameraY = bMoveCameraY;
	}
}

void CRegenToolView::MoveCameraSmoothly()
{
	// 이동설정되지 않은 경우
	if( !m_bIsMovingCameraSmoothly )
		return;

	CAMERA_DESC Desc;
	g_pExecutive->GetGeometry()->GetCameraDesc( &Desc, 0);
	VECTOR3 vecMove;
	if( m_bIsMovingCameraY )
	{
		vecMove = m_vecMoveTargetPos - Desc.v3From;
	}
	else
	{
		const float fRatio = (Desc.v3From.y - m_vecMoveTargetPos.y) / Desc.v3EyeDir.y;
		VECTOR3 vecTargetPos = m_vecMoveTargetPos + Desc.v3EyeDir * fRatio;
		vecMove = vecTargetPos - Desc.v3From;
		vecMove.y = 0;
	}

	const float fA = 0.002f;
	const float fB = 0.15f;
	float fDist = VECTOR3Length( &vecMove );
	float fTick = fA * fDist;
	if( fTick > fB )
	{
		fTick = fB;
	}

	// 일정거리 이하 이동완료로 판단.
	if( fDist < 300.f )
	{
		m_bIsMovingCameraSmoothly = FALSE;
		return;
	}

	// 시간에 따라 이동.. 거리에 따라 가속
	vecMove = vecMove * fTick;
	g_pExecutive->GetGeometry()->MoveCamera( &vecMove, 0 );
}

void CRegenToolView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	CWnd * wnd = GetDlgItem(IDC_MAPIMAGE);
	if( wnd && wnd->GetSafeHwnd() )
	{
		CRect clientRect;
		GetClientRect( &clientRect );
		clientRect.DeflateRect( 5, 40, 7, 10 );
		wnd->MoveWindow( &clientRect );
		if( g_pExecutive )
			g_pExecutive->GetGeometry()->UpdateWindowSize();
	}
}
BOOL CRegenToolView::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			if(this != GetFocus())
			{
				break;
			}
			
			switch( pMsg->wParam )
			{
			// Tree에서 찾기
			case 'f':
			case 'F':
				{
					BOOL bIsPressCtrl = GetAsyncKeyState( VK_CONTROL );
					if( !bIsPressCtrl )
						break;

					CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
					if( !pFrm )				break;
					CSheetPageMonster* pMonsterPage = (CSheetPageMonster*)pFrm->GetMonsterPage();
					if( !pMonsterPage )		break;
					
					pMonsterPage->ShowFindDialog();
				}
				break;
			case 'c':
			case 'C':
				{
					if(FALSE == GetAsyncKeyState(
						VK_CONTROL))
					{
						break;
					}

					CRegenToolDoc* const document = GetDocument();

					if(0 == document)
					{
						break;
					}

					const Script::Monster* const monster = document->GetMonsterInfo( GetSelectedObjectHandle() );

					if(0 == monster)
					{
						break;
					}

					// InsertMonster()를 호출한다
				}
				break;
			case VK_DELETE:
				{
					CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
					if( !pFrm )				break;
					CRegenToolDoc* pDoc = (CRegenToolDoc*)GetDocument();
					if( !pDoc )				break;
					CSheetPageMonster* pMonsterPage = (CSheetPageMonster*)pFrm->GetMonsterPage();
					if( !pMonsterPage )		break;

					GXOBJECT_HANDLE handle = GetSelectedObjectHandle();
					// 몬스터 제거
					if( !pDoc->DeleteMonster( handle ) )
						break;

					pMonsterPage->DeleteMonsterInTree( handle );
					g_pExecutive->DeleteGXObject( handle );
				}
				break;
			}
		}
		break;
	}

	return CFormView::PreTranslateMessage(pMsg);
}

void CRegenToolView::SelectObject(const CRect& rect)
{
	if( rect.Width() == 0 || rect.Height() == 0 )
		return;

	// 선택된 몬스터 해제
	while(false == mSelectedHandleContainer.empty())
	{
		const GXOBJECT_HANDLE handle = (*mSelectedHandleContainer.begin());
		mSelectedHandleContainer.erase(
			handle);

		const Script::Monster* const monster = GetDocument()->GetMonsterInfo(
			handle);

		if(0 == monster)
		{
			continue;
		}

		const BASE_MONSTER_LIST& baseMonsterList = g_pLoadList.GetMonster(
			monster->mMonsterKind);
		VECTOR3 scale = {
			baseMonsterList.Scale,
			baseMonsterList.Scale,
			baseMonsterList.Scale,
		};

		g_pExecutive->GXOSetScale(
			handle,
			&scale);
		g_pExecutive->SetAlphaFlag(
			handle,
			255);
	}

	const int maxPickDesc = 100;
	PICK_GXOBJECT_DESC gxoDesc[maxPickDesc] = {0};
	DWORD pickedSize = 0;

	if(rect.TopLeft() == rect.BottomRight())
	{
		CPoint point(
			rect.TopLeft());

		pickedSize = g_pExecutive->GXOGetMultipleObjectWithScreenCoord(
			gxoDesc,
			_countof(gxoDesc),
			&point,
			0,
			PICK_TYPE_PER_FACE);
	}
	else
	{
		pickedSize = g_pExecutive->GXOGetMultipleObjectWithScreenCoordRect(
			gxoDesc,
			_countof(gxoDesc),
			CRect(rect),
			0,
			PICK_TYPE_PER_COLLISION_MESH);
	}

	for(DWORD i = pickedSize; i-- > 0;)
	{
		const PICK_GXOBJECT_DESC& description = gxoDesc[i];
		Script::Monster* const monster = GetDocument()->GetMonsterInfo(
			description.gxo);

		if(0 == monster)
		{
			continue;
		}

		mSelectedHandleContainer.insert(
			description.gxo);

		const BASE_MONSTER_LIST& baseMonsterList = g_pLoadList.GetMonster(
			monster->mMonsterKind);
		VECTOR3 scale = {
			baseMonsterList.Scale,
			baseMonsterList.Scale,
			baseMonsterList.Scale,
		};

		g_pExecutive->GXOSetScale(
			description.gxo,
			&scale);
		g_pExecutive->SetAlphaFlag(
			description.gxo,
			125);
	}

	if(1 == mSelectedHandleContainer.size())
	{
		const GXOBJECT_HANDLE handle = (*mSelectedHandleContainer.begin());
		CMainFrame* const mainFrame = (CMainFrame*)AfxGetMainWnd();

		if(0 == mainFrame)
		{
			return;
		}

		CSheetPageMonster* const sheet = (CSheetPageMonster*)mainFrame->GetMonsterPage();

		if(0 == sheet)
		{
			return;
		}

		sheet->SelectMonster(
			handle);
	}
}

void CRegenToolView::StartMoveOfSelectedObjects()
{
	if( mSelectedHandleContainer.empty() )
		return;

	m_SelectedObjectMoveInfo.clear();

	m_bSelectedObjectMove = TRUE;

	const GXOBJECT_HANDLE& baseSelectedhandle = *mSelectedHandleContainer.begin();
	VECTOR3 baseObjectPosition = {0};
	g_pExecutive->GXOGetPosition( baseSelectedhandle, &baseObjectPosition );

	for( HandleContainer::const_iterator selectedhandleiter = mSelectedHandleContainer.begin() ; selectedhandleiter != mSelectedHandleContainer.end() ; ++selectedhandleiter )
	{
		const GXOBJECT_HANDLE& selectedhandle = *selectedhandleiter;

		VECTOR3 objectPosition = {0};
		g_pExecutive->GXOGetPosition( selectedhandle, &objectPosition );

		CPoint point(0, 0);
		point.x = (int)(objectPosition.x - baseObjectPosition.x);
		point.y = (int)(objectPosition.z - baseObjectPosition.z);

		m_SelectedObjectMoveInfo.insert( SelectedHandleMoveInfo::value_type( selectedhandle, point ) );
	}
}

void CRegenToolView::MovingOfSelectedObjects( const CPoint& mousepoint )
{
	if( mSelectedHandleContainer.empty() ||
		!m_bSelectedObjectMove )
		return;

	const CPoint ViewMousePos = CalcSelectPoint( mousepoint );
	VECTOR3* baseTargetPos = GetPickedPosition( ViewMousePos.x, ViewMousePos.y );
	if( !baseTargetPos || 
		MAP->CollisonCheck( baseTargetPos, baseTargetPos, baseTargetPos ) )
		return;

	for( HandleContainer::const_iterator selectedhandleiter = mSelectedHandleContainer.begin() ; selectedhandleiter != mSelectedHandleContainer.end() ; ++selectedhandleiter )
	{
		const GXOBJECT_HANDLE& selectedhandle = *selectedhandleiter;

		SelectedHandleMoveInfo::const_iterator selectedObjectMoveInfoIter = m_SelectedObjectMoveInfo.find( selectedhandle );
		if( selectedObjectMoveInfoIter == m_SelectedObjectMoveInfo.end() )
			continue;
	
		const CPoint& selectedObjectmoveInfo = selectedObjectMoveInfoIter->second;

		VECTOR3 targetPos={0};
		targetPos.x = baseTargetPos->x + selectedObjectmoveInfo.x;
		targetPos.z = baseTargetPos->z + selectedObjectmoveInfo.y;

		g_pExecutive->GXMGetHFieldHeight( &targetPos.y, targetPos.x, targetPos.z );
		g_pExecutive->GXOSetPosition( selectedhandle, &targetPos, FALSE );
	}
}

void CRegenToolView::EndMoveOfSelectedObjects()
{
	if( mSelectedHandleContainer.empty() ||
		!m_bSelectedObjectMove )
		return;

	for( HandleContainer::const_iterator selectedhandleiter = mSelectedHandleContainer.begin() ; selectedhandleiter != mSelectedHandleContainer.end() ; ++selectedhandleiter )
	{
		const GXOBJECT_HANDLE& selectedhandle = *selectedhandleiter;

		VECTOR3 targetPos={0};
		g_pExecutive->GXOGetPosition( selectedhandle, &targetPos );

		Script::Monster* pMonsterInfo = GetDocument()->GetMonsterInfo( selectedhandle );
		if( !pMonsterInfo )		return;

		pMonsterInfo->mPosition = targetPos;
	}

	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return;
	CSheetPageMonster* pMonsterPage = (CSheetPageMonster*)pFrm->GetMonsterPage();
	if( !pMonsterPage )		return;

	pMonsterPage->SetUseTreeSelectFunc( FALSE );
	pMonsterPage->UpdateSelectedInfo();

	m_bSelectedObjectMove = FALSE;

	SetSelectedObjectHandle( NULL );
}
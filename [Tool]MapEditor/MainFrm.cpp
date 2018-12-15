// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "4DyuchiGXMapEditor.h"
#include "MainFrm.h"
#include "4DyuchiGXMapEditorDoc.h"
#include "4DyuchiGXMapEditorView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "tool.h"
#include "RightView.h"
#include ".\mainfrm.h"


CDialogBar* g_pDlgBar		=	NULL;
CMainFrame*	g_pMainFrame	=	NULL;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

#pragma comment(lib, "../lib/for_sse_debug/SS3DGFunc.lib")

//---
BOOL		bSplitterSize	= FALSE;
//---

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_HFIELD, OnNewHfield)
	ON_COMMAND(ID_EXPORT_HFIELD, OnExportHfield)
	ON_COMMAND(ID_IMPORT_HFIELD, OnImportHfield)
	ON_COMMAND(ID_SAMPLE_FROM_MOD, OnSampleFromMod)
	ON_COMMAND(ID_CREATE_STATIC_MODEL_FROM_MOD, OnCreateStaticModelFromMod)
	ON_COMMAND(ID_BUILD_MAP, OnBuildMap)
	ON_COMMAND(ID_NEW_STATIC_MODEL, OnNewStaticModel)
	ON_COMMAND(ID_EXPORT_STATIC_MODEL, OnExportStaticModel)
	ON_COMMAND(ID_IMPORT_STATIC_MODEL, OnImportStaticModel)
	ON_COMMAND(ID_BEGIN_CREATE_STATIC_MODEL, OnBeginCreateStaticModel)
	ON_COMMAND(ID_END_CREATE_STATIC_MODEL, OnEndCreateStaticModel)
	ON_COMMAND(ID_VIEW_TILE_PALETTE, OnViewTilePalette)
	ON_COMMAND(ID_VIEW_ZONE_EDIT, OnViewEditZone)
	ON_COMMAND(ID_EXPORT_TILE_TABLE, OnExportTileTable)
	ON_COMMAND(ID_MENU_BOUNDINGBOX, OnMenuBoundingbox)
	ON_COMMAND(ID_NEW_MAP, OnNewMap)
	ON_COMMAND(ID_EDIT_UNDO, OnUndo)
	ON_COMMAND(ID_EDIT_REDO, OnRedo)
	ON_COMMAND(ID_DELETE_ALL_GXLIGHTS, OnDeleteAllGxlights)
	ON_COMMAND(ID_DELETE_ALL_GXOBJECTS, OnDeleteAllGxobjects)
	ON_COMMAND(ID_SAVE_MAP, OnSaveMap)
	ON_COMMAND(ID_LOAD_MAP, OnLoadMap)
	ON_COMMAND(ID_CREATE_LIGHT_FROM_MOD, OnCreateLightFromMod)
	ON_COMMAND(ID_SAVE_ALL_LIGHTS, OnSaveAllLights)
	ON_COMMAND(ID_LOAD_LIGHTS, OnLoadLights)
	ON_WM_SIZE()
	ON_COMMAND(ID_CALL_HELP, OnCallHelp)
	ON_COMMAND(ID_MENUITEM_HFIELD_PROPERTY_CHANGE, OnMenuitemHfieldPropertyChange)
	ON_COMMAND(ID_LOAD_VIEWONLY_HFIELD, OnLoadViewonlyHfield)
	ON_COMMAND(ID_VIEW_ONLY_HFIELD, OnViewOnlyHfield)
	ON_COMMAND(ID_TOGGLE_ONLY_LIGHTTEXTURE, OnToggleOnlyLighttexture)
	ON_COMMAND(ID_SAVE_ALL_GXOBJECTS, OnSaveAllGxobjects)
	ON_COMMAND(ID_LOAD_GXOBJECTS, OnLoadGxobjects)
	ON_COMMAND(ID_VIEW_COLLISION_STM, OnViewCollisionStm)
	ON_COMMAND(ID_MENUITEM_CLEAR_ALPHAMAP_TILEINDEX, OnMenuitemClearAlphamapTileindex)
	ON_COMMAND(ID_MENUITEM_CLEAR_ALPHAMAP_OBJECT, OnMenuitemClearAlphamapObject)
	ON_COMMAND(ID_MENUITEM_CLEAR_ALPHAMAP, OnMenuitemClearAlphamap)
	ON_COMMAND(ID_VIEW_TILESET, OnViewTileset)	
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_CAMERA_INFO, OnViewCameraInfo)
	ON_COMMAND(ID_VIEW_LOAD_FAILED_TEXTURE_LIST, OnViewLoadFailedTextureList)
	ON_COMMAND(ID_VIEW_TOGGLE_TOOL_OBJECT, OnViewToggleToolObject)
	ON_COMMAND(ID_VIEW_RENDER_STATIC_MODEL, OnViewRenderStaticModel)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ACCELERATOR_PICKER, OnAcceleratorPicker)
	ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{}

CMainFrame::~CMainFrame()
{
	DeleteObject(m_hBmpSelectButton);
	DeleteObject(m_hBmpPickColor);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//trustpak
	if (!m_wndDlgBar.Create(this, IDD_DLGBAR_PANEL1, CBRS_TOP, IDD_DLGBAR_PANEL1) ||
		!m_wndDlgBar2.Create(this, IDD_DLGBAR_PANEL2, CBRS_TOP, IDD_DLGBAR_PANEL1) ||
		!m_wndDlgBarStatus.Create(this, IDD_DLGBAR_STATUS, CBRS_BOTTOM, IDD_DLGBAR_STATUS))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}
	//


/*	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}*/
	

	m_hBmpSelectButton = LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP_PICK));
	::SendMessage(m_wndDlgBar2.GetDlgItem(IDC_CHECK_SELECT)->m_hWnd, BM_SETIMAGE, 0, (LPARAM)m_hBmpSelectButton);

	m_hBmpPickColor = LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP_PICK_COLOR));
	::SendMessage(m_wndDlgBar2.GetDlgItem(IDC_CHECK_PICK_COLOR)->m_hWnd, BM_SETIMAGE, 0, (LPARAM)m_hBmpPickColor);


	g_pDlgBar		=	&m_wndDlgBar;
	g_pDlgBarHField =	&m_wndDlgBar2;
	g_pMainFrame = this;
	
	
	SetTilePos(0,0);

	MAABB	DefaultBox;
	ZeroMemory( &DefaultBox, sizeof( MAABB));
	DefaultBox.Max.y	=	1000.0f;
	DefaultBox.Min.y	=	-1000.0f;
	SetEditMaxMin( &DefaultBox);
		
	CMenu*	pMenu = GetMenu();
	pMenu->CheckMenuItem(ID_VIEW_ONLY_HFIELD,MF_CHECKED);

	//trustpak 2005/06/09
	pMenu->CheckMenuItem(ID_VIEW_TOGGLE_TOOL_OBJECT, MF_CHECKED);
	pMenu->CheckMenuItem(ID_VIEW_RENDER_STATIC_MODEL, MF_CHECKED);

	///

	m_wndDlgBar2.OnInitdialog();

	m_menuHFieldAlphaMap.LoadMenu(IDR_POPUP_HFIELD_ALPHAMAP);	

	g_pDlgBarTool = &m_wndDlgBar;

	// 자동 저장을 위한 타이머
	SetTimer( 1, 300000, 0 );
	return 0;
}

void CMainFrame::OnHFieldAlphaMapMenu(CPoint* point)
{
	
//	m_menuHFieldAlphaMap.EnableMenuItem(IDR_POPUP_HFIELD_ALPHAMAP,TRUE);
	
	CMenu* pSubMenu;
    pSubMenu = m_menuHFieldAlphaMap.GetSubMenu( 0 );
	pSubMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, point->x, point->y,this);
	pSubMenu->EnableMenuItem(ID_MENUITEM_CLEAR_ALPHAMAP,TRUE);
	pSubMenu->EnableMenuItem(ID_MENUITEM_CLEAR_ALPHAMAP_TILEINDEX,TRUE);
	pSubMenu->EnableMenuItem(ID_MENUITEM_CLEAR_ALPHAMAP_OBJECT,TRUE);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

//	cs.style |= WS_MAXIMIZE;


	//trustpak
	// View size를 800 * 600 으로 맞춰줌.. (게임 클라이언트와 똑같은 비율의 뷰가 필요할 때 용이하도록. )
	cs.cx = 968;
	cs.cy = 807;

	return TRUE;
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
{
	m_bSplitterSize		= FALSE;
	m_iRightMenuSize	= 160;
	m_iBottomMenuSize	= 150;

	m_wndRightSplitter.CreateStatic(this,1,2);			// 위 아래로 나누고..
	
//	int idPane = m_wndBottomSplitter.IdFromRowCol(0,0);	// 위의 것을 얻어와서..
//	m_wndLeftSplitter.CreateStatic(&m_wndBottomSplitter,1,2,WS_CHILD|WS_VISIBLE,idPane);	// 위의 것을 다시 양쪽으로 나눈다.
	
	m_wndRightSplitter.CreateView(0,0,RUNTIME_CLASS(CMy4DyuchiGXMapEditorView),
								 CSize( 0,0),pContext);						// 아래쪽 메뉴 붙이고..

	m_wndRightSplitter.CreateView(0,1,RUNTIME_CLASS(CRightView),
								 CSize( 0, 0),pContext);						// 아래쪽 메뉴 붙이고..
	

	g_pRightView = (CRightView*)m_wndRightSplitter.GetPane(0,1);
	// 각 페인의 높이와 너비를 설정한다.
	RECT rc;
	this->GetClientRect( &rc );	
	
	m_wndRightSplitter.SetColumnInfo(0,rc.right - m_iRightMenuSize , 0 );	

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG

void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnNewHfield() 
{
	// TODO: Add your command handler code here
	g_pTool->CreateNewHeightField();
}

void CMainFrame::OnExportHfield() 
{	
	// TODO: Add your control notification handler code here
	char szFilters[] = "Height Field Files (*.hfl) | *.hfl | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(FALSE,"hfl","*.hfl",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;
	// TODO: Add your command handler code here
	g_pTool->ExportHeightField(filename);
	
}

void CMainFrame::OnImportHfield() 
{
	// TODO: Add your command handler code here
		// TODO: Add your control notification handler code here
	char szFilters[] = "Height Field Files (*.hfl) | *.hfl | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(TRUE,"hfl","*.hfl",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;
	// TODO: Add your command handler code here
	g_pTool->ImportHeightField(filename,0);
		
}

void CMainFrame::OnSampleFromMod() 
{
	char szFilters[] = "Model Files (*.mod) | *.mod | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(TRUE,"mod","*.mod",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;

	// TODO: Add your command handler code here
	g_pTool->SampleFromMod(filename);
	
}

void CMainFrame::OnCreateStaticModelFromMod() 
{
	char szFilters[] = "Model Files (*.mod) | *.mod | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(TRUE,"mod","*.mod",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;

	// TODO: Add your command handler code here
	g_pTool->CreateStaticModelFromMod(filename);

}

void CMainFrame::OnBuildMap() 
{
	// 들어가기 전에 범위 테스트.
	MAABB	Box;
	memset( &Box, 0, sizeof(Box));
	GetEditMaxMin( &Box);
	
	if( Box.Max.y - Box.Min.y <= 0.0f)
	{
		::MessageBox( 0, "Min보다 Max가 크게 잡아주십쇼.!", "에러", MB_OK);
		return	;
	}

	g_pTool->BuildMap(&Box);
	// TODO: Add your command handler code here

	g_pTool->GetWorldBox( &Box);
	SetEditMaxMin( &Box);
	
}

void CMainFrame::SetTilePos(DWORD dwPosX,DWORD dwPosZ)
{
	char	txt[16];

	wsprintf(txt,"%d",dwPosX);
	m_wndDlgBar2.SetDlgItemText(IDC_STATIC_TILE_X,txt);
		
	wsprintf(txt,"%d",dwPosZ);
	m_wndDlgBar2.SetDlgItemText(IDC_STATIC_TILE_Z,txt);
}

void CMainFrame::OnNewStaticModel() 
{
	// TODO: Add your command handler code here
	g_pTool->CreateNewStaticModel();
	
}

void CMainFrame::OnExportStaticModel() 
{
	// TODO: Add your command handler code here
	char szFilters[] = "StaticModel Files (*.stm) | *.stm | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(FALSE,"stm","*.stm",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;

	g_pTool->ExportStaticModel(filename);	
}

void CMainFrame::OnImportStaticModel() 
{
	// TODO: Add your command handler code here
	char szFilters[] = "StaticModel Files (*.stm) | *.stm | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(TRUE,"stm","*.stm",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;

	g_pTool->ImportStaticModel(filename);
}

void CMainFrame::OnBeginCreateStaticModel() 
{
	// TODO: Add your command handler code here
	g_pTool->BeginCreateStaticModel();
}

void CMainFrame::OnEndCreateStaticModel() 
{
	// TODO: Add your command handler code here
	g_pTool->EndCreateStaticModel();
}



void CMainFrame::OnViewTilePalette() 
{
	g_pTool->ViewTilePalette();
}

void CMainFrame::OnViewEditZone() 
{
	g_pTool->ViewEditZone();
}

void CMainFrame::OnExportTileTable() 
{
	// TODO: Add your command handler code here
	char szFilters[] = "TILE Table Files(*.ttb)|*.ttb";
	
	char* filename;
		
	CString pathname;
	CFileDialog dlg(FALSE, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES,szFilters, NULL); 
	
	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;

	if( g_pTool->m_pSectionSplitter->bSectionInputMode )
	{
		MessageBox( "Section Input Mode를 체크하지 마세요!", "저장실패", MB_ICONSTOP );
	}
	else if( g_pTool->m_pSpotSplitter->bSectionInputMode )
	{
		MessageBox( "MoveSpot Input Mode를 체크하지 마세요!", "저장실패", MB_ICONSTOP );
	}
	else
	{
		g_pTool->ExportTileTable(filename);
	}
}

void CMainFrame::OnMenuBoundingbox() 
{
	// TODO: Add your command handler code here

		
	BOOL bResult = 	g_pTool->ToggleBoundingBox();
	CMenu*	pMenu = GetMenu();
	if (bResult)
		pMenu->CheckMenuItem(ID_MENU_BOUNDINGBOX,MF_CHECKED);
	else
		pMenu->CheckMenuItem(ID_MENU_BOUNDINGBOX,MF_UNCHECKED);


	
}


void CMainFrame::OnNewMap() 
{
	// TODO: Add your command handler code here

	//trustpak
	if (IDYES == MessageBox("All contents will be removed.\nDo you really want to create new map?",
							"warning!",
							MB_YESNO))
	{
		g_pTool->OnNewMap();
	}	

	//trustpak	
	DisplayCurrentMapFileName();
}

void CMainFrame::OnDeleteAllGxlights() 
{
	// TODO: Add your command handler code here
	g_pTool->OnDeleteAllGXLights();
	
}

void CMainFrame::OnDeleteAllGxobjects() 
{
	// TODO: Add your command handler code here
	g_pTool->OnDeleteAllGXObjects();
}

void CMainFrame::OnSaveMap() 
{
	// TODO: Add your command handler code here

	char szFilters[] = "Map Files (*.map) | *.map | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(FALSE,"map","*.map",NULL,szFilters,AfxGetMainWnd());


	
	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;

	MAABB	Box;
	memset( &Box, 0, sizeof(Box));
	GetEditMaxMin( &Box);

	g_pTool->SetWorldBox( &Box);
	g_pTool->SaveMap(filename);
	g_pTool->GetWorldBox( &Box);
	SetEditMaxMin( &Box);


	//trustpak

	DisplayCurrentMapFileName();
	
	//

}

void CMainFrame::OnLoadMap() 
{
	// TODO: Add your command handler code here
		// TODO: Add your command handler code here
	char szFilters[] = "Map Script Flles(*.map)|*.map";
	
	char* filename;
		
	CString pathname;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES,szFilters, NULL); 
	
	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;

	g_pTool->LoadMap(filename);

	MAABB	Box;
	g_pTool->GetWorldBox( &Box);

	SetEditMaxMin( &Box);


	//trustpak
	DisplayCurrentMapFileName();

	//

}


void CMainFrame::OnCreateLightFromMod() 
{
	// TODO: Add your command handler code here
	char szFilters[] = "Model Files (*.mod) | *.mod | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(TRUE,"mod","*.mod",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;

	// TODO: Add your command handler code here
	g_pTool->CreateLightFromMod(filename);
		
}


void CMainFrame::OnSaveAllLights() 
{
	// TODO: Add your command handler code here
	char szFilters[] = "Lights Config Files (*.lfg) | *.lfg | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(FALSE,"lfg","*.lfg",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;
	g_pTool->SaveLights(filename);	
}

void CMainFrame::OnLoadLights() 
{
	// TODO: Add your command handler code here
	char szFilters[] = "Lights Config Files (*.lfg) | *.lfg | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(TRUE,"lfg","*.lfg",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;

	g_pTool->LoadLights(filename);
}


//------------------------------------------------------//
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( nType == SIZE_MAXIMIZED )
	{
		bSplitterSize = true; // 처음 프로그램이 시작 될때 최대크기로 시작함으로 이때 플래그를 바꾸어 주면 된다.
	}
	
	if( bSplitterSize && cx != 0 && cy != 0 )
	{
		m_wndRightSplitter.SetColumnInfo(0, cx - m_iRightMenuSize, 0 );
		m_wndRightSplitter.RecalcLayout();
	}	
}

void CMainFrame::SetEditMaxMin(MAABB *pWorldBox)
{
	char	buff[128];
	itoa( (int)pWorldBox->Max.y, buff, 10);
	m_wndDlgBar.SetDlgItemText( IDC_EDIT_MAX_HEIGHT, buff);
	itoa( (int)pWorldBox->Min.y, buff, 10);
	m_wndDlgBar.SetDlgItemText( IDC_EDIT_MIN_HEIGHT, buff);
}

void CMainFrame::GetEditMaxMin(MAABB *pWorldBox)
{
	CString		str;
	g_pDlgBar->GetDlgItemText( IDC_EDIT_MAX_HEIGHT, str);
	pWorldBox->Max.y	=	(float)atof( str);
	g_pDlgBar->GetDlgItemText( IDC_EDIT_MIN_HEIGHT, str);
	pWorldBox->Min.y	=	(float)atof( str);
}

void CMainFrame::OnCallHelp() 
{
	// TODO: Add your command handler code here
	MessageBox("아직 도움말이 지원 안됩니다.", "헬프", MB_OK);
}

void CMainFrame::OnMenuitemHfieldPropertyChange() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnLoadViewonlyHfield() 
{
	// TODO: Add your command handler code here

	char szFilters[] = "Height Field Files (*.hfl) | *.hfl | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(TRUE,"hfl","*.hfl",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;
	// TODO: Add your command handler code here
	g_pTool->ImportViewOnlyHeightField(filename);
	
}

void CMainFrame::OnViewOnlyHfield() 
{
	// TODO: Add your command handler code here
	BOOL bResult = g_pTool->ToggleViewOnlyHField();
	CMenu*	pMenu = GetMenu();
	if (bResult)
		pMenu->CheckMenuItem(ID_VIEW_ONLY_HFIELD,MF_CHECKED);
	else
		pMenu->CheckMenuItem(ID_VIEW_ONLY_HFIELD,MF_UNCHECKED);
}

void CMainFrame::OnToggleOnlyLighttexture() 
{
	// TODO: Add your command handler code here
	BOOL bResult = g_pTool->ToggleViewOnlyLightTexture();

	CMenu*	pMenu = GetMenu();
	if (bResult)
		pMenu->CheckMenuItem(ID_TOGGLE_ONLY_LIGHTTEXTURE,MF_CHECKED);
	else
		pMenu->CheckMenuItem(ID_TOGGLE_ONLY_LIGHTTEXTURE,MF_UNCHECKED);
	
}

void CMainFrame::OnSaveAllGxobjects() 
{
	// TODO: Add your command handler code here

	// TODO: Add your command handler code here
	char szFilters[] = "GXObjects Config Files (*.ofg) | *.ofg | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(FALSE,"ofg","*.ofg",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;
	
	g_pTool->SaveGXObjects(filename);	

}

void CMainFrame::OnLoadGxobjects() 
{

	// TODO: Add your command handler code here
	char szFilters[] = "Lights Config Files (*.ofg) | *.ofg | All files (*.*) |*.*||";
	char* filename;
		
	CString pathname;
	CFileDialog dlg(TRUE,"ofg","*.ofg",NULL,szFilters,AfxGetMainWnd());

	if (dlg.DoModal() == IDOK)
	{
		pathname = (LPSTR)(LPCSTR)(dlg.GetPathName());
		if (pathname == "")
			return;
	}

	if (!(filename = (LPSTR)(LPCSTR)pathname))
		return;

	if (!lstrlen(filename))
		return;

	g_pTool->LoadGXObjects(filename);

}

void CMainFrame::OnViewCollisionStm() 
{
	// TODO: Add your command handler code here
	
	BOOL bResult = g_pTool->ToggleViewColSTM();
	CMenu*	pMenu = GetMenu();
	if (bResult)
		pMenu->CheckMenuItem(ID_VIEW_COLLISION_STM,MF_CHECKED);
	else
		pMenu->CheckMenuItem(ID_VIEW_COLLISION_STM,MF_UNCHECKED);
}

void CMainFrame::OnMenuitemClearAlphamapTileindex() 
{
	// TODO: Add your command handler code here
	POINT	point;
	GetCursorPos(&point);
	g_pTool->OnClearAlphaMapWithTileIndex(&point,0);
	
}

void CMainFrame::OnMenuitemClearAlphamapObject() 
{
	// TODO: Add your command handler code here
	POINT	point;
	GetCursorPos(&point);
	g_pTool->OnClearAlphaMapWithObjectIndex(&point,0);
	
}

void CMainFrame::OnMenuitemClearAlphamap() 
{
	// TODO: Add your command handler code here
	POINT	point;
	GetCursorPos(&point);
	g_pTool->OnClearAlphaMapAll(&point,0);
	
}

//yh
void CMainFrame::OnViewTileset() 
{
	g_pTool->ViewTileSet();
}

void CMainFrame::OnClose() 
{
	if (IDNO == MessageBox("프로그램을 종료합니다.\n\n정말로 프로그램을 닫으시겠습니까?", "LeHide", MB_YESNO))
	{
		return;
	}

    CFrameWnd::OnClose();
}


//trustpak
void CMainFrame::DisplayCurrentMapFileName(void)
{
	char szCaption[MAX_PATH] = {0};
	strncpy(
		szCaption,
		g_pTool->GetCurrentMapFileName(),
		sizeof(szCaption));
	strncat(szCaption, " - 4DyuchiGXMapEditor", sizeof(szCaption));
		
	SetWindowText(szCaption);
}

void CMainFrame::OnViewCameraInfo() 
{
	BOOL bResult = g_pTool->ToggleDisplayCameraInfo();
	CMenu*	pMenu = GetMenu();
	if (bResult)
		pMenu->CheckMenuItem(ID_VIEW_CAMERA_INFO,MF_CHECKED);
	else
		pMenu->CheckMenuItem(ID_VIEW_CAMERA_INFO,MF_UNCHECKED);
	
}

////

void CMainFrame::OnViewLoadFailedTextureList() 
{
	g_pTool->UpdateLoadFailedTextureDialog();	
	g_pTool->ShowFailedTextureDialog();
}

void CMainFrame::OnViewToggleToolObject() 
{
	// 아래의 GetRenderMode 함수는 Tool에 국한된 기능으로,
	// 게임과는 관련이 없다. 
	DWORD dwRenderToolMode = g_pTool->GetRenderToolMode();

	// Not 연산자를 통해 뒤집어 줄 수도 있겠지만 Tool_ RenderMode가 추가될 수 있기 때문에
	// 나중에 안전하게 If 문을 사용하여 뒤집어 준다. 

	if (RENDER_MODE_TOOL & dwRenderToolMode)
	{
		dwRenderToolMode = RENDER_MODE_DEFAULT;
	}
	else
	{
		dwRenderToolMode = RENDER_MODE_DEFAULT|RENDER_MODE_TOOL;		
	}

	g_pTool->SetRenderToolMode(dwRenderToolMode);

	
	CMenu*	pMenu = GetMenu();
	if (NULL == pMenu)
	{
		return;
	}

	if (RENDER_MODE_TOOL & dwRenderToolMode)
	{
		pMenu->CheckMenuItem(ID_VIEW_TOGGLE_TOOL_OBJECT, MF_CHECKED);
	}
	else
	{
		pMenu->CheckMenuItem(ID_VIEW_TOGGLE_TOOL_OBJECT, MF_UNCHECKED);		
	}

	g_pTool->Render();
}

void CMainFrame::OnViewRenderStaticModel() 
{
	BOOL bResult = g_pTool->ToggleRenderStaticModel();
	CMenu*	pMenu = GetMenu();
	if (NULL == pMenu)
	{
		return;
	}

	if (TRUE == bResult)
	{
		pMenu->CheckMenuItem(ID_VIEW_RENDER_STATIC_MODEL, MF_CHECKED);
	}
	else
	{
		pMenu->CheckMenuItem(ID_VIEW_RENDER_STATIC_MODEL, MF_UNCHECKED);
	}

}

void CMainFrame::OnAcceleratorPicker()
{
	// TODO: Add your command handler code here
	g_pDlgBarHField->OnCheckPickColor();
}

void CMainFrame::OnUndo()
{
	g_pTool->Undo();
	g_pTool->Render();
}

void CMainFrame::OnRedo()
{
	g_pTool->Redo();
	g_pTool->Render();
}
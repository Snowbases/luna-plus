// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "RegenTool.h"
#include "RegenToolDoc.h"
#include "RegenToolView.h"
#include "MainFrm.h"
#include "MHMap.h"
#include "TileManager.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_VIEW_CONTROLVIEW, OnViewControlview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CONTROLVIEW, OnUpdateViewControlview)
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
{
	// 100201 LUJ, http://msdn.microsoft.com/en-us/library/dz33x6wb%28VS.80%29.aspx
	m_bAutoMenuEnable = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	CString title = _T("Base Dialog Bar");
	if (!m_CoolDlgBarBase.Create(this, &m_BaseDlgBar, title, IDD_DLGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

	m_CoolDlgBarBase.SetBarStyle(m_CoolDlgBarBase.GetBarStyle()|CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC);
	m_CoolDlgBarBase.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(&m_CoolDlgBarBase, AFX_IDW_DOCKBAR_LEFT);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.cx = 1010;
	cs.cy = 720;
	cs.x = 0;
	cs.y = 0;
	
	return CFrameWnd::PreCreateWindow(cs);
//	return TRUE;
}

void CMainFrame::OnFileOpen() 
{
	char Filter[] = "MAP format(*.map)|*.map|BMP format(*.bmp)|*.bmp|TXT format(*.txt)|*.txt|TTB format(*.ttb)|*.ttb|All format(*.*)|*.*|";
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, Filter);
	if(fileDlg.DoModal() == IDOK)
	{
		CString data = fileDlg.GetPathName();
		CString Ext = fileDlg.GetFileExt();
		CString filename = fileDlg.GetFileName();
	}
}

void CMainFrame::SetStatusText(LPCTSTR text)
{
	m_wndStatusBar.SetPaneText(
		0,
		text);
}
void CMainFrame::OnViewControlview()
{
	ShowControlBar( &m_CoolDlgBarBase, !m_CoolDlgBarBase.IsWindowVisible(), FALSE );
}

void CMainFrame::OnUpdateViewControlview(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_BaseDlgBar.IsWindowVisible() );
}

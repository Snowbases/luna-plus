// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "NewPackingTool.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_USE_AES, &CMainFrame::OnUseAES)
	ON_COMMAND(ID_SCRIPT_TEST, OnScriptTest)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_INFO,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	m_bUseAES = false;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	DWORD dwStyle;
	dwStyle = m_wndToolBar.GetButtonStyle(m_wndToolBar.CommandToIndex(ID_USE_AES));
	dwStyle |= (TBSTYLE_CHECK);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_USE_AES), dwStyle);

	// default CHECK true
	//m_wndToolBar.SendMessage(TB_CHECKBUTTON, (WPARAM)ID_USE_AES, (LPARAM) MAKELONG (true, 0));

	int index = 0;
	while (m_wndToolBar.GetItemID(index) != IDP_PLACEHOLDER)
		index++;

	RECT rect;

	//next convert that button to a seperator and get its position
	m_wndToolBar.SetButtonInfo(index, IDP_PLACEHOLDER, TBBS_SEPARATOR,
		SNAP_WIDTH);
	m_wndToolBar.GetItemRect(index, &rect);

	//expand the rectangle to allow the combo box room to drop down
	rect.top+=2;
	rect.bottom += 200;

	// then .Create the combo box and show it
	if (!m_wndToolBar.m_wndSnap.Create(WS_CHILD|WS_VISIBLE|CBS_AUTOHSCROLL| 
		CBS_DROPDOWNLIST|CBS_HASSTRINGS,
		rect, &m_wndToolBar, IDC_SNAP_COMBO))
	{
		TRACE0("Failed to create combo-box\n");
		return FALSE;
	}
	m_wndToolBar.m_wndSnap.ShowWindow(SW_SHOW);

	//fill the combo box
	if (g_ScriptTest.m_bLoad)
	{
		ScriptTestModeVecItr itr;
		for (itr=g_ScriptTest.m_vecScript.begin(); itr!=g_ScriptTest.m_vecScript.end(); ++itr)
		{
			m_wndToolBar.m_wndSnap.AddString((*itr)->m_modeName.c_str());
		}
		m_wndToolBar.m_wndSnap.SetCurSel(0);
	}

	//if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
	//	CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	//{
	//	TRACE0("대화 상자 모음을 만들지 못했습니다.\n");
	//	return -1;		// 만들지 못했습니다.
	//}

	//if (!m_wndReBar.Create(this) ||
	//	!m_wndReBar.AddBar(&m_wndToolBar) ||
	//	!m_wndReBar.AddBar(&m_wndDlgBar))
	//{
	//	TRACE0("크기 조정 막대를 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_NORMAL, 100);

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP);

	// Output Bar
	CString title(_T("Output Dialog"));
	if (!m_wndOutPutBar.Create(this, &m_cOutPutDlg, title, IDD_OUTPUT_DLG))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

	m_wndOutPutBar.SetBarStyle(m_wndOutPutBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC);
	m_wndOutPutBar.EnableDocking(CBRS_ALIGN_BOTTOM);
	DockControlBar(&m_wndOutPutBar, AFX_IDW_DOCKBAR_BOTTOM);

	//ShowControlBar(&m_wndOutPutBar, TRUE, FALSE);
	
	title = _T("FileList Dialog");
	// FileListBar
	if (!m_wndFileListBar.Create(this, &m_cFileListDlg, title, IDD_FILELIST_DLG))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

	m_wndFileListBar.SetBarStyle(m_wndFileListBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC);
	m_wndFileListBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndFileListBar, AFX_IDW_DOCKBAR_LEFT);

	//ShowControlBar(&m_wndFileListBar, TRUE, FALSE);

	m_ReservationViewIndex = -1;

	return 0;
}
//
//BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
//	CCreateContext* pContext)
//{
//	return m_wndSplitter.Create(this,
//		2, 1,               // TODO: 행 및 열의 개수를 조정합니다.
//		CSize(500, 100),      // TODO: 최소 창 크기를 조정합니다.
//		pContext);
//}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if ( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

BOOL CMainFrame::DestroyWindow()
{
	//CNewPackingToolView* pView = (CNewPackingToolView*)this->GetActiveView();
	m_cFileListDlg.ResetAll();

	return CFrameWnd::DestroyWindow();
}

// CMainFrame 진단

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


// CMainFrame 메시지 처리기


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	CRect sz;
	GetClientRect(&sz);

	if (GetActiveView())
		GetActiveView()->GetWindowRect(&sz);
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	SetActiveWindow();      // activate us first !
	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);
	for (UINT iFile = 0; iFile < nFiles; iFile++)
	{
		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
		pApp->OpenDocumentFile(szFileName);
	}
	::DragFinish(hDropInfo);
}

void CMainFrame::ChangeFile(int index, LPCTSTR filename)
{
	if (index < 0)
		return;

	m_cFileListDlg.ChangeFile(index, filename);
}

void CMainFrame::AddFile(LPCTSTR filename)
{
	SelChange( m_cFileListDlg.AddFile(filename) );

	CString outStr;

	outStr.Format("Load File ( %s ) is Complete. \r\n", filename);

	AddOutStr(outStr);
}

void CMainFrame::SelChange(int index)
{
	CNewPackingToolView* pView = (CNewPackingToolView*)this->GetActiveView();

	if (pView)
		pView->SelChange(index);
	else
	{
		m_ReservationViewIndex = index;
	}
}

void CMainFrame::MoveLine(int line)
{
	CNewPackingToolView* pView   = (CNewPackingToolView*)this->GetActiveView();

	pView->MoveLine(line);
}


void CMainFrame::ResetOutStr()
{
	m_cOutPutDlg.Reset();
}

void CMainFrame::AddOutStr(LPCTSTR str)
{
	m_cOutPutDlg.AddOutStr(str);
}

void CMainFrame::OnUseAES()
{
	m_bUseAES = m_bUseAES ? false : true;
}

void CMainFrame::OnScriptTest()
{
	UINT testType = m_wndToolBar.m_wndSnap.GetCurSel();

	CNewPackingToolView* pView = (CNewPackingToolView*)this->GetActiveView();

	ResetOutStr();

	pView->ScriptTest(testType);
}

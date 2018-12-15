// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "client.h"
#include "MainFrm.h"
#include "..\common\version.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	//ON_WM_DROPFILES()
	//ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	//ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_LOG_COPY, OnEditCopy)
	ON_COMMAND(ID_LOG_PRINT, OnFilePrint)
	//ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_LOG_SAVE, OnFileSave)
	//ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	//ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
		ID_INDICATOR_CAPS,
		ID_INDICATOR_NUM,
		ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_strPathname = _T( "log\\" );
}

CMainFrame::~CMainFrame()
{}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a RichEdit window to occupy the client area of the frame
	if (!m_RichEdit.Create(WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY |
		ES_MULTILINE | WS_HSCROLL | WS_VSCROLL,
		CRect(), this, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create RichEdit window\n");
		return -1;
	}

	// Set extended style: 3D look - ie. sunken edge
	::SetWindowLong(m_RichEdit.GetSafeHwnd(), GWL_EXSTYLE, WS_EX_CLIENTEDGE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE |
		CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
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

	//Set Charactistics of the Status Bar
	UINT nID, nStyle;
	int cxWidth;

	m_wndStatusBar.GetPaneInfo( 0, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo( 0, nID, SBPS_NORMAL | SBPS_STRETCH, cxWidth);

	//Set the default font
	{
		CHARFORMAT format;
		format.cbSize		= sizeof(format);
		format.dwEffects	= CFE_PROTECTED;
		format.dwMask		= CFM_BOLD | CFM_FACE | CFM_SIZE | CFM_CHARSET | CFM_PROTECTED;
		format.yHeight		= 180;
		format.bCharSet		= 0x00;
		_tcscpy( format.szFaceName, _T( "Trebuchet MS" ) );

		m_RichEdit.SetDefaultCharFormat(format);
	}

	m_RichEdit.SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_PROTECTED);

	//Support Drag and Drop
	DragAcceptFiles();

	// 080311 LUJ, 버전과 함께 제목줄 표시
	{
		CString title;
		title.LoadString( IDR_MAINFRAME );

		CString version;
		version.Format( _T( " %d.%d.%d" ), MAJORVERSION, MINORVERSION, BUILDNUMBER );

		SetWindowText( title + version );
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0,0,0,LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)));
	return TRUE;
}


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


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* pWnd)
{
	// forward focus to the RichEdit window
	m_RichEdit.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the RichEdit have first crack at the command
	if (m_RichEdit.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


//void CMainFrame::OnEditPaste()
//{
//	m_RichEdit.PasteSpecial(CF_TEXT);
//}

//void CMainFrame::OnEditCut()
//{
//	m_RichEdit.Cut();
//}

void CMainFrame::OnEditCopy()
{
	m_RichEdit.Copy();
}

//void CMainFrame::OnEditUndo()
//{
//	m_RichEdit.Undo();
//}

void CMainFrame::OnFilePrint()
{
	Print(true);
}

void CMainFrame::Print(bool bShowPrintDialog)
{
	CPrintDialog printDialog(false);

	if (bShowPrintDialog)
	{
		if(printDialog.DoModal() == IDCANCEL)
			return; // User pressed cancel, don't print.
	}
	else
	{
		printDialog.GetDefaults();
	}

	HDC hPrinterDC = printDialog.GetPrinterDC();

	// This code basically taken from MS KB article Q129860

	FORMATRANGE fr;
	int	  nHorizRes = GetDeviceCaps(hPrinterDC, HORZRES);
	int	  nVertRes = GetDeviceCaps(hPrinterDC, VERTRES);
	int     nLogPixelsX = GetDeviceCaps(hPrinterDC, LOGPIXELSX);
	int     nLogPixelsY = GetDeviceCaps(hPrinterDC, LOGPIXELSY);
	LONG	      lTextLength;   // Length of document.
	LONG	      lTextPrinted;  // Amount of document printed.

	// Ensure the printer DC is in MM_TEXT mode.
	SetMapMode ( hPrinterDC, MM_TEXT );

	// Rendering to the same DC we are measuring.
	ZeroMemory(&fr, sizeof(fr));
	fr.hdc = fr.hdcTarget = hPrinterDC;

	// Set up the page.
	fr.rcPage.left     = fr.rcPage.top = 0;
	fr.rcPage.right    = (nHorizRes/nLogPixelsX) * 1440;
	fr.rcPage.bottom   = (nVertRes/nLogPixelsY) * 1440;

	// Set up 0" margins all around.
	fr.rc.left   = fr.rcPage.left ;//+ 1440;  // 1440 TWIPS = 1 inch.
	fr.rc.top    = fr.rcPage.top ;//+ 1440;
	fr.rc.right  = fr.rcPage.right ;//- 1440;
	fr.rc.bottom = fr.rcPage.bottom ;//- 1440;

	// Default the range of text to print as the entire document.
	fr.chrg.cpMin = 0;
	fr.chrg.cpMax = -1;
	m_RichEdit.FormatRange(&fr,true);

	// Set up the print job (standard printing stuff here).
	DOCINFO di;
	ZeroMemory(&di, sizeof(di));
	di.cbSize = sizeof(DOCINFO);

	di.lpszDocName = m_strPathname;

	// Do not print to file.
	di.lpszOutput = NULL;


	// Start the document.
	StartDoc(hPrinterDC, &di);

	// Find out real size of document in characters.
	lTextLength = m_RichEdit.GetTextLength();

	do
	{
		// Start the page.
		StartPage(hPrinterDC);

		// Print as much text as can fit on a page. The return value is
		// the index of the first character on the next page. Using TRUE
		// for the wParam parameter causes the text to be printed.

		lTextPrinted =m_RichEdit.FormatRange(&fr,true);
		m_RichEdit.DisplayBand(&fr.rc );

		// Print last page.
		EndPage(hPrinterDC);

		// If there is more text to print, adjust the range of characters
		// to start printing at the first character of the next page.
		if (lTextPrinted < lTextLength)
		{
			fr.chrg.cpMin = lTextPrinted;
			fr.chrg.cpMax = -1;
		}
	}
	while (lTextPrinted < lTextLength);

	// Tell the control to release cached information.
	m_RichEdit.FormatRange(NULL,false);

	EndDoc (hPrinterDC);

	DeleteDC(hPrinterDC);

}


//void CMainFrame::OnDropFiles(HDROP hDropInfo)
//{
//	CString FileName;
//
//	::DragQueryFile(hDropInfo, 0, FileName.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
//	FileName.ReleaseBuffer();
//	::DragFinish(hDropInfo);
//
//	m_strPathname = FileName;
//
//	SetWindowTitle();
//	ReadFile();
//}


//void CMainFrame::ReadFile()
//{
//	CString FileName = m_strPathname;
//
//	// Convert full filename characters from "\" to "\\"
//	FileName.Replace(_T("\\"), _T("\\\\"));
//
//	// The file from which to load the contents of the rich edit control.
//	CFile cFile(FileName, CFile::modeRead);
//	EDITSTREAM es;
//
//	es.dwCookie =  (DWORD) &cFile;
//	es.pfnCallback = (EDITSTREAMCALLBACK) MyStreamInCallback;
//
//	m_RichEdit.StreamIn(SF_TEXT,es); // Perform the streaming
//}


void CMainFrame::WriteFile()
{

	// Convert full filename characters from "\" to "\\"
	CString Pathname = m_strPathname;
	Pathname.Replace(_T("\\"), _T("\\\\"));

	// The file from which to load the contents of the rich edit control.
	CFile cFile(Pathname, CFile::modeCreate|CFile::modeWrite);
	EDITSTREAM es;

	es.dwCookie =  (DWORD) &cFile;
	es.pfnCallback = (EDITSTREAMCALLBACK) MyStreamOutCallback;

	m_RichEdit.StreamOut(SF_TEXT,es); // Perform the streaming
}

//void CMainFrame::OnFileOpen()
//{
//
//	// szFilters is a text string that includes two file name filters:
//	// "*.my" for "MyType Files" and "*.*' for "All Files."
//	char szFilters[]= "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";
//
//	// Create an Open dialog; the default file name extension is ".my".
//	CFileDialog fileDlg (TRUE, "txt", "*.txt",
//		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
//
//	// Display the file dialog. When user clicks OK, fileDlg.DoModal()
//	// returns IDOK.
//	if( fileDlg.DoModal ()==IDOK )
//	{
//		m_strPathname = fileDlg.GetPathName();
//
//		//Change the window's title to the opened file's title.
//		SetWindowTitle();
//		ReadFile();
//	}
//}


DWORD CALLBACK MyStreamInCallback(CFile* dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	// Required for StreamIn
	CFile* pFile = (CFile*) dwCookie;

	*pcb = pFile->Read(pbBuff, cb);

	return 0;
}


DWORD CALLBACK MyStreamOutCallback(CFile* dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	// Required for StreamOut
	CFile* pFile = (CFile*) dwCookie;

	pFile->Write(pbBuff, cb);
	*pcb = cb;

	return 0;
}


void CMainFrame::OnFileSave()
{
	if (m_strPathname == "") OnFileSaveas();
	else WriteFile();
}


//void CMainFrame::OnFileNew()
//{
//	//Clear the Richedit text
//	m_RichEdit.SetWindowText("");
//
//	m_strPathname = "";
//
//	SetWindowTitle();
//}


void CMainFrame::OnFileSaveas()
{
	// szFilters is a text string that includes two file name filters:
	// "*.txt" for "Text Files" and "*.*" for "All Files."
	TCHAR szFilters[] = _T( "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||" );

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg (FALSE, _T( "txt" ), _T( "*.txt" ),
		OFN_OVERWRITEPROMPT| OFN_HIDEREADONLY, szFilters, this);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal()
	// returns IDOK.
	if( fileDlg.DoModal ()==IDOK )
	{
		m_strPathname = fileDlg.GetPathName();

		WriteFile();
	}
}


void CMainFrame::Put( const CString& text )
{
	const int length = m_RichEdit.GetWindowTextLength();

	m_RichEdit.SetSel( length, length );
    m_RichEdit.ReplaceSel( text );
	m_RichEdit.SetFocus();
}
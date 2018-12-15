// MainFrm.h : interface of the CMainFrame class
//
//	source:		http://www.codeproject.com/KB/cpp/TextEdit.aspx
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__0BC0FB9E_F0B7_486A_A939_5894B9E590A9__INCLUDED_)
#define AFX_MAINFRM_H__0BC0FB9E_F0B7_486A_A939_5894B9E590A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

DWORD CALLBACK MyStreamInCallback(CFile* dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
DWORD CALLBACK MyStreamOutCallback(CFile* dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

class CMainFrame : public CFrameWnd
{

public:
	CMainFrame();
	CRichEditCtrl m_RichEdit;
	//void ReadFile();
	void WriteFile();
protected:
	DECLARE_DYNAMIC(CMainFrame)

	// Attributes
public:

	// Operations
public:
	void Put( const CString& );

	// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	// Implementation
public:
	virtual ~CMainFrame();
	void Print(bool bShowPrintDialog);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar	m_wndToolBar;
	CString		m_strPathname;


	// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	//afx_msg void OnEditPaste();
	//afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnFilePrint();
	//afx_msg void OnDropFiles(HDROP hDropInfo);
	//afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	//afx_msg void OnFileNew();
	afx_msg void OnFileSaveas();
	//afx_msg void OnEditUndo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__0BC0FB9E_F0B7_486A_A939_5894B9E590A9__INCLUDED_)


#pragma once

class CDialogTileSet;

class CTileSetView : public CWnd
{
// Construction
public:
	CTileSetView();

	//trustpak

	CDialogTileSet* m_pDlgTileSet;
	///

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTileSetView)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTileSetView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTileSetView)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
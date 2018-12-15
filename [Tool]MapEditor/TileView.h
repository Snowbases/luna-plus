#pragma once

class CDialogTilePalette;

class CTileView : public CWnd
{
	CDialogTilePalette*	m_pPalette;

public:
	void SetTilePalette(CDialogTilePalette* pPalette) {m_pPalette = pPalette;}
	CTileView();
	virtual ~CTileView();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTileView)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CTileView)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
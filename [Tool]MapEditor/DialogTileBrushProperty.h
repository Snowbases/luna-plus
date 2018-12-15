#pragma once

struct TILE_BRUSH_PROPERTY;

class CDialogTileBrushProperty : public CDialog
{
	TILE_BRUSH_PROPERTY*			m_pTileBrushProperty;
// Construction
public:
	void		SetTileBrushProperty(TILE_BRUSH_PROPERTY* pProperty) {m_pTileBrushProperty = pProperty;}
	CDialogTileBrushProperty(CWnd* pParent = NULL);   // standard constructor


// Dialog Data
	//{{AFX_DATA(CDialogTileBrushProperty)
	enum { IDD = IDD_DIALOG_TILEBRUSH_PROPERTY };
	CString	m_strTileBrushName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTileBrushProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogTileBrushProperty)
	afx_msg void OnButtonOk();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
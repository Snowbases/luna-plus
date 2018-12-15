#pragma once

#include "ListBoxContainer.h"

class CTool;
class CTileBrush;
class C3DBrush;
class CTileBrush;

#define MAX_BRUSHES_NUM	4096
#define MAX_TILE_BRUSHES_NUM 4096

class CRightView : public CFormView
{
	CListBoxContainer		m_lbContainerTileBrush;
	CListBoxContainer		m_lbContainerModelBrush;

	BOOL					InsertTileBrush(CTileBrush* pTileBrush);
	void					ReleaseBrush(int iListBoxIndex);
protected:
	CRightView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRightView)

public:	
	C3DBrush*				GetSelectedBrush();
	C3DBrush*				GetBrush(POINT* ptCursor);
	BOOL					GetGXObjectName(char* szFileName);

	void					ToggleTileBrushMode();
	CTileBrush*				GetSelectedTileBrush();
	BOOL					SaveTileBrushes(char* szFileName);
	BOOL					LoadTileBrushes(char* szFileName);
	void					LoadGXObject(char* szFileName);
	void					UpdateColor(void);
	BYTE m_byBackRed;
	BYTE m_byBackGreen;
	BYTE m_byBackBlue;

	//{{AFX_DATA(CRightView)
	enum { IDD = IDD_FORMVIEW_RIGHT };
	CListBox	m_lcTileBrushList;
	CListBox	m_lcGXObjects;
	CListBox	m_lcBrushList;
	BYTE	m_bR;
	BYTE	m_bG;
	BYTE	m_bB;
	BOOL	m_bEnableShadow;
	BYTE	m_bHFieldAlpha;
	BYTE	m_bSTMAlpha;
	BOOL	m_btTileBrushToggle;
	BYTE	m_bViewOnlyHFieldAlpha;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRightView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRightView();

	// Generated message map functions
	//{{AFX_MSG(CRightView)
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonDeleteBrush();
	afx_msg void OnButtonLoadGxobject();
	afx_msg void OnButtonShadeStart();
	afx_msg void OnButtonTransparencySet();
	afx_msg void OnButtonCreateBrush();
	afx_msg void OnCheckTileBrushMode();
	afx_msg void OnButtonDeleteTileBrush();
	afx_msg void OnButtonDeleteallTileBrush();
	afx_msg void OnSaveTileBrush();
	afx_msg void OnLoadTileBrush();
	afx_msg void OnDestroy();
	afx_msg void OnButtonBackModify();	
	afx_msg void OnChangeEditBackBlue();
	afx_msg void OnChangeEditBackGreen();
	afx_msg void OnChangeEditBackRed();
	afx_msg void OnChangeEditShadeColorB();
	afx_msg void OnChangeEditShadeColorG();
	afx_msg void OnChangeEditShadeColorR();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CRightView*	g_pRightView;
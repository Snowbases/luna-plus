#pragma once

class CDlgBarTool : public CDialogBar
{
// Construction
public:
	CDlgBarTool();

	//trustpak

	VOID	ToggleTileEditButton(VOID);
	VOID	ToggleTileSetEditButton(VOID);

	///

// Dialog Data
	//{{AFX_DATA(CDlgBarTool)
	enum { IDD = IDD_DLGBAR_PANEL1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBarTool)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBarTool)
	afx_msg void OnRadioXaxis();
	afx_msg void OnRadioYaxis();
	afx_msg void OnRadioZaxis();
	afx_msg void OnRadioAxisFree();
	afx_msg void OnRadioCreate();
	afx_msg void OnRadioMove();
	afx_msg void OnRadioRot();
	afx_msg void OnRadioScale();
	afx_msg void OnButtonResetTr();
	afx_msg void OnRadioEditTile();
	afx_msg void OnRadioEditVertex();
	afx_msg void OnRadioEditGxo();
	afx_msg void OnRadioEditBrush();
	afx_msg void OnRadioLightMode();
	afx_msg void OnRadioTriggerMode();
	afx_msg void OnRadioGetPosTile();
	afx_msg void OnRadioRmodeSolid();
	afx_msg void OnRadioRmodeWire();
	afx_msg void OnButtonLightPos();
	afx_msg void OnRadioGetHfobj();
	afx_msg void OnUpdateCmdUIAllLightMove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdUIDirectionalLightButton(CCmdUI* pCmdUI);
	afx_msg void OnButtonDirectionalLight();
	afx_msg void OnRadioDrawLightmap();
	afx_msg void OnRadioDrawHFieldVertexColor();
	afx_msg void OnRadioDrawAlphaTexel();
	afx_msg void OnRadioTs();
	afx_msg void OnRadioRmodeBoth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#pragma once

class CDlgBarHField : public CDialogBar
{
public:
	//{{AFX_DATA(CDlgBarHField)
	enum { IDD = IDD_DLGBAR_PANEL2 };
	CButton	m_btPickColor;
	CButton	m_btSelect;
	DWORD	m_dwBrushSize;
	float	m_fHeightPitch;
	float	m_fBias;
	float	m_fRadius;
	DWORD	m_dwHFieldDetail;
	DWORD	m_dwFreezeDetailLevel;
	BYTE	m_bG;
	BYTE	m_bR;
	BYTE	m_bB;
	float	m_fHFieldColorBrushSize;
	BOOL	m_bEnableHFieldBlend;
	//}}AFX_DATA

	float			m_fHFBrushSize;

	int				OnInitdialog();

	void			UpdateColor();
	DWORD			GetBrushSize()	{return m_dwBrushSize;}
	float			GetHeightPitch()	{return m_fHeightPitch;}
	float			GetInterpolationBias() {return m_fBias;}
	float			GetRadius()				{return m_fRadius;}
	DWORD			GetHFieldDetail()		{return m_dwHFieldDetail;}

	DWORD			GetLightTexelColor();
	void			SetLightTexelColor(DWORD dwColor);
	void			ToggleCheckSelect();
	void			ToggleCheckPickColor();
	CDlgBarHField();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBarHField)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:
public:


	// Generated message map functions
	//{{AFX_MSG(CDlgBarHField)
	afx_msg void OnChangeEditBrushSize();
	afx_msg void OnButtonViewProperty();
	afx_msg void OnUpdateCmdUIPropertyButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdUISelectButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdUIRefreshButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdUIHFObjDisableButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdUIHFObjEnableButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdUIResetZButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdUIHFObjFreezeDetailButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdUIHFObjUnfreezeDetailButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdUIEditBrush(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdUIEditFreezeDetail(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIUpdateEditHfbrushSize(CCmdUI* pCmdUI);
	afx_msg	void OnCmdUIUpdateEditLM_R(CCmdUI* pCmdUI);
	afx_msg	void OnCmdUIUpdateEditLM_G(CCmdUI* pCmdUI);
	afx_msg	void OnCmdUIUpdateEditLM_B(CCmdUI* pCmdUI);
	afx_msg void OnCheckSelect();
	afx_msg void OnChangeEditHeightPitch();
	afx_msg void OnChangeEditRadius();
	afx_msg void OnChangeEditInterpolationBias();
	afx_msg void OnButtonRefreshHfield();
	afx_msg void OnChangeEditHfieldDetail();
	afx_msg void OnButtonHfobjDisable();
	afx_msg void OnButtonHfobjEnable();
	afx_msg void OnButtonResetZ();
	afx_msg void OnButtonHfobjFreezeDetail();
	afx_msg void OnButtonHfobjUnfreezeDetail();
	afx_msg void OnChangeEditFreezeDetail();
	afx_msg void OnChangeEditLmB();
	afx_msg void OnChangeEditLmG();
	afx_msg void OnChangeEditLmR();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	afx_msg void OnCheckPickColor();
	afx_msg void OnChangeEditHfbrushSize();
	afx_msg void OnCheckHfieldBlend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CDlgBarHField*	g_pDlgBarHField;
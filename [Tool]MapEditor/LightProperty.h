#pragma once

class CLightProperty : public CDialog
{
	
	GXLIGHT_PROPERTY*		m_pLightProperty;
// Construction
public:
	void			SetGXLightProperty(GXLIGHT_PROPERTY* pLightProperty);
	
	CLightProperty(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLightProperty)
	enum { IDD = IDD_DIALOG_LIGHT_PROPERTY };
	DWORD	m_R;
	DWORD	m_B;
	DWORD	m_G;
	float	m_fRS;
	float	m_fPosX;
	float	m_fPosY;
	float	m_fPosZ;
	BOOL	m_bLightSwitch;
	BOOL	m_bShadowSwitch;
	BOOL	m_bEnabeDynamicLight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLightProperty)
	afx_msg void OnButtonOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
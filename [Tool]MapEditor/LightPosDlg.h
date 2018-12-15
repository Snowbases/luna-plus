#pragma once

class CLightPosDlg : public CDialog
{
// Construction
public:
	CLightPosDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLightPosDlg)
	enum { IDD = IDD_DIALOG2 };
	float	m_fPosX;
	float	m_fPosY;
	float	m_fPosZ;
	//}}AFX_DATA

	float	GetPosX()	{ return m_fPosX; }
	float	GetPosY()	{ return m_fPosY; }
	float	GetPosZ()	{ return m_fPosZ; }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightPosDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLightPosDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
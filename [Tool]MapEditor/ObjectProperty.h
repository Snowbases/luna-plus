#pragma once

class CObjectProperty : public CDialog
{

	GXOBJECT_PROPERTY*		m_pObjectProperty;
	GXOBJECT_HANDLE mHandle;
// Construction
public:
	CObjectProperty(CWnd* pParent = NULL);   // standard constructor
	void					SetGXObjectProperty(GXOBJECT_PROPERTY* pProperty, GXOBJECT_HANDLE);

// Dialog Data
	//{{AFX_DATA(CObjectProperty)
	enum { IDD = IDD_DIALOG_OBJECT_PROPERTY };
	CString m_strName;
	float	m_fX;
	float	m_fY;
	float	m_fZ;
	DWORD	m_dwID;
	float	m_fRad;
	float	m_fAxisX;
	float	m_fAxisY;
	float	m_fAxisZ;
	BOOL	m_bApplyHField;
	BOOL	m_bAsEffect;

	
	float m_fScaleX;
	float m_fScaleY;
	float m_fScaleZ;

	BOOL m_bLock;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectProperty)
	afx_msg void OnButtonOk();
	afx_msg void OnButtonDelete();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
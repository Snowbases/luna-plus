#pragma once

class CDialogHFieldProperty : public CDialog
{

// Construction
public:
// Dialog Data
	//{{AFX_DATA(CDialogHFieldProperty)
	enum { IDD = IDD_DIALOG_HFIELD_PROPERTY };
	float	m_fBottom;
	float	m_fFaceSize;
	DWORD	m_dwFacesNumPerObjAxis;
	CString	m_strName;
	float	m_fLeft;
	float	m_fRight;
	float	m_fTop;
	DWORD	m_dwObjNumX;
	DWORD	m_dwObjNumZ;
	DWORD	m_dwDetailLevelNum;
	DWORD	m_dwIndexBufferNum;
	float	m_fMinTileLength;
	//}}AFX_DATA

	void						Present(HFIELD_CREATE_DESC* phfCreate,BOOL bShow);
	void						Calc();
	DWORD						GetIndexBufferNum() {return m_dwIndexBufferNum;}

	CDialogHFieldProperty(CWnd* pParent = NULL);   // standard constructor




// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogHFieldProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogHFieldProperty)
	afx_msg void OnButtonHfieldPropertyOk();
	afx_msg void OnButtonHfieldEx1();
	afx_msg void OnButtonHfieldEx2();
	afx_msg void OnButtonHfieldEx3();
	afx_msg void OnButtonHfieldEx4();
	afx_msg void OnChangeEditObnnumX();
	afx_msg void OnChangeEditObjnumZ();
	afx_msg void OnChangeEditFaceSize();
	afx_msg void OnChangeEditFacesNumPerObj();
	afx_msg void OnChangeEditLeft();
	afx_msg void OnChangeEditTop();
	afx_msg void OnChangeEditLodNum();
	afx_msg void OnButtonHfieldEx5();
	afx_msg void OnButtonHfieldTtb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
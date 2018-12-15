#pragma once

class CLoadFailedTextureDialog : public CDialog
{
public:
	CLoadFailedTextureDialog(CWnd* pParent = NULL);   // standard constructor
	void UpdateListBoxData(TEXTURE_TABLE* pTextureTable, DWORD dwCount);

	TEXTURE_TABLE*		m_pLoadFailedTextureTable;
	DWORD				m_dwLoadFailedTextureTableCount;

// Dialog Data
	//{{AFX_DATA(CLoadFailedTextureDialog)
	enum { IDD = IDD_LOADFAILEDTEXTURE_DIALOG_ };	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadFailedTextureDialog)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadFailedTextureDialog)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSavelistButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
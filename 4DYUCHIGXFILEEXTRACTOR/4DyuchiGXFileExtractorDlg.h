// 4DyuchiGXFileExtractorDlg.h : header file
//

#if !defined(AFX_4DYUCHIGXFILEEXTRACTORDLG_H__F2ACA9F4_3C09_4A53_A28C_45236B600720__INCLUDED_)
#define AFX_4DYUCHIGXFILEEXTRACTORDLG_H__F2ACA9F4_3C09_4A53_A28C_45236B600720__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMy4DyuchiGXFileExtractorDlg dialog

class CMy4DyuchiGXFileExtractorDlg : public CDialog
{
// Construction
public:
	CMy4DyuchiGXFileExtractorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMy4DyuchiGXFileExtractorDlg)
	enum { IDD = IDD_MY4DYUCHIGXFILEEXTRACTOR_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy4DyuchiGXFileExtractorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMy4DyuchiGXFileExtractorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_4DYUCHIGXFILEEXTRACTORDLG_H__F2ACA9F4_3C09_4A53_A28C_45236B600720__INCLUDED_)

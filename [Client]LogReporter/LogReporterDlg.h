// LogReporterDlg.h : header file
//

#if !defined(AFX_LOGREPORTERDLG_H__3639C487_C7F9_465D_A4B9_B8D765ECB241__INCLUDED_)
#define AFX_LOGREPORTERDLG_H__3639C487_C7F9_465D_A4B9_B8D765ECB241__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/*
#define FTP_IP		"211.233.41.139"
#define FTP_PORT	3100
#define SPECFILE		"SpecInfo.txt"
#define CLIENTBUGFILE	"assert.txt"
#define MAPCHANGEERRORFILE	"debug.txt"
#define RECVERRORFILE	"recv.txt"
#define FTP_USER_ID		"bugreporter"
#define FTP_USER_PASSWORD		"bug0!!##"
*/

/////////////////////////////////////////////////////////////////////////////
// CLogReporterDlg dialog

class CLogReporterDlg : public CDialog
{
// Construction
public:
	CLogReporterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLogReporterDlg)	
	enum { IDD = IDD_LOGREPORTER_DIALOG };
	//}}AFX_DATA
	int IngValue;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogReporterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	//UINT m_nTimer;
	DWORD m_dwStartTime;
// Implementation
	BOOL m_bEnd;
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLogReporterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGREPORTERDLG_H__3639C487_C7F9_465D_A4B9_B8D765ECB241__INCLUDED_)

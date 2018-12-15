// FileConverterDlg.h : header file
//

#if !defined(AFX_FILECONVERTERDLG_H__B55A31F3_6937_416E_B3F7_9AF4E34FB395__INCLUDED_)
#define AFX_FILECONVERTERDLG_H__B55A31F3_6937_416E_B3F7_9AF4E34FB395__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../4DyuchiGRX_Common/IFileStorage.h"
#include "../4DyuchiGRX_Common/IRenderer.h"


/////////////////////////////////////////////////////////////////////////////
// CFileConverterDlg dialog

class CFileConverterDlg : public CDialog
{
	I4DyuchiGXRenderer*		m_pRenderer;
	I4DyuchiFileStorage*	m_pFileStorage;
	
	HANDLE	m_hConsole;
	void ConvertSTM(char* szFileName);
	void ConvertMOD(char* szFileName);
	void ConvertANM(char* szFileName);
	void ConvertHFL(char* szFileName);
	void ConvertTGA(char* szFileName);
// Construction
public:
	CFileConverterDlg(CWnd* pParent = NULL);	// standard constructor
	~CFileConverterDlg();

// Dialog Data
	//{{AFX_DATA(CFileConverterDlg)
	enum { IDD = IDD_FILECONVERTER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileConverterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFileConverterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILECONVERTERDLG_H__B55A31F3_6937_416E_B3F7_9AF4E34FB395__INCLUDED_)

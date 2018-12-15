#pragma once
#include "afxwin.h"


// ExtractSelectedDlg 대화 상자입니다.

class ExtractSelectedDlg : public CDialog
{
	DECLARE_DYNAMIC(ExtractSelectedDlg)

public:
	ExtractSelectedDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ExtractSelectedDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EXTRACT_SELECTED_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListBox m_SeletList;
	virtual BOOL OnInitDialog();
	BOOL m_bClearDir;
	afx_msg void OnBnClickedDelteButton();
};

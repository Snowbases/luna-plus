#pragma once
#include "afxwin.h"


// COutPutDlg 대화 상자입니다.

class COutPutDlg : public CDialog
{
	DECLARE_DYNAMIC(COutPutDlg)

public:
	COutPutDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~COutPutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OUTPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void MoveLine();
	afx_msg void OnScriptTest();

	void Reset();
	void AddOutStr(LPCTSTR str);

public:
	BOOL		m_bInit;
	CEdit		m_cOutput;

	CString		m_strOut;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

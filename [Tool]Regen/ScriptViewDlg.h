#pragma once


// CScriptViewDlg 대화 상자입니다.

class CScriptViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CScriptViewDlg)

public:
	CScriptViewDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CScriptViewDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SCRIPT_VIEW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strScriptEdit;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnPaint();
protected:
	virtual void OnOK();
};

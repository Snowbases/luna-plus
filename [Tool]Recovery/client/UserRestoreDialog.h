#pragma once
#include "afxwin.h"


// CUserRestoreDialog 대화 상자입니다.

class CUserRestoreDialog : public CDialog
{
	DECLARE_DYNAMIC(CUserRestoreDialog)

public:
	CUserRestoreDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUserRestoreDialog();

	virtual void OnFinalRelease();
	
	const CString& GetName() const;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_USERRESTOREDIALOG };


protected:
	CclientApp&	mApplication;

	CString	mPlayerName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	CEdit mNameEdit;
	virtual INT_PTR DoModal( const CString& playerName );
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	//afx_msg void OnBnClickedUserRestoreSubmitButton();
	afx_msg void OnBnClickedOk();
};

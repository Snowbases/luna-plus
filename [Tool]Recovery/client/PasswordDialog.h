#pragma once
#include "afxwin.h"


// CPasswordDialog 대화 상자입니다.

class CPasswordDialog : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDialog)

public:
	CPasswordDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPasswordDialog();

	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PASSWORDDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	CEdit mPreviousPasswordEdit;
	CEdit mNextPasswordEdit;
	CEdit mConfirmPasswordEdit;
	
public:
	const TCHAR* GetPreviousPassword() const;
	const TCHAR* GetNextPassword() const;

protected:
	CString mPreviousPassword;
	CString mNextPassword;
	virtual void OnOK();
};

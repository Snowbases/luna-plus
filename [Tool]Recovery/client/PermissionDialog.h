#pragma once
#include "afxcmn.h"


// CPermissionDialog 대화 상자입니다.

class CPermissionDialog : public CDialog
{
	DECLARE_DYNAMIC(CPermissionDialog)

public:
	CPermissionDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPermissionDialog();

	virtual void OnFinalRelease();

	void Parse( DWORD serverIndex, const MSGROOT* );
	void Request( DWORD serverIndex );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PERMISSIONDIALOG };

protected:
	CString GetIpAddress();

protected:
	CclientApp& mApplication;
	DWORD		mServerIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	CIPAddressCtrl mIpAddressCtrl;
	afx_msg void OnBnClickedPermissionAddButton();
	afx_msg void OnBnClickedPermissionRemoveButton();
};

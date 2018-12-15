#pragma once
#include "afxwin.h"
#include "afxcmn.h"


class CclientApp;


// CLoginDialog 대화 상자입니다.

class CLoginDialog : public CDialog
{
	DECLARE_DYNAMIC(CLoginDialog)

public:
	CLoginDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoginDialog();

	virtual void OnFinalRelease();
	void Parse( DWORD serverIndex, const MSGROOT* );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOGINDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
//	afx_msg void OnOK();
	

private:
	CclientApp&	mApplication;
	CEdit			mIdEdit;
	CEdit			mPasswordEdit;
	CListCtrl		mServerListCtrl;
	CStatic			mVersionStatic;
	CButton			mSubmitButton;
	CButton			mCancelButton;
	CProgressCtrl	mLoadingProgressCtrl;
	CBitmap			mTitleImage;

public:
	virtual BOOL	OnInitDialog();
	afx_msg void OnBnClickedLoginConnectButton();
	inline void SetProgressMax( int value )	{ mLoadingProgressCtrl.SetRange32( 0, value ); }
	// 080630 LUJ, 로딩 프로그레스 컨트롤의 값을 설정한다
	void SetProgressValue( int value, const TCHAR* message = 0 );
protected:
	virtual void OnCancel();
public:
	CStatic mTitleImageStatic;
};

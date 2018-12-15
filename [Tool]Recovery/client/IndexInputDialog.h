#pragma once
#include "afxwin.h"


// CIndexInputDialog 대화 상자입니다.

class CIndexInputDialog : public CDialog
{
	DECLARE_DYNAMIC(CIndexInputDialog)
public:
	struct Configuration
	{
		bool mIsEnableItemDbIndex;
		bool mIsEnableItemIndex;
		bool mIsEnablePlayerIndex;
		bool mIsEnableKeyword;
		bool mIsEnableUserIndex;
		bool mIsEnableConsignIndex;
	};

public:
	CIndexInputDialog( CclientApp&, const Configuration&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CIndexInputDialog();

	virtual void OnFinalRelease();
	virtual void OnOK();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INDEXINPUTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
	DWORD	mItemIndex;
	DWORD	mItemDbIndex;
	DWORD	mPlayerIndex;
	CString	mKeyword;
	DWORD mUserIndex;
	DWORD	mConsignIndex;

public:
	inline DWORD GetItemIndex()		const { return mItemIndex; }
	inline DWORD GetItemDbIndex()	const { return mItemDbIndex; }
	inline DWORD GetPlayerIndex()	const { return mPlayerIndex; }
	inline LPCTSTR GetKeyword()		const { return mKeyword; }
	inline DWORD GetUserIndex() const { return mUserIndex; }
	inline DWORD GetConsignIndex() const { return mConsignIndex; }
	CEdit mIndexInputEdit;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedIndexInputItemRadio();

private:
	CclientApp&			mApplication;
	const Configuration	mConfiguration;
	CButton mItemRadioButton;
	CButton mItemDbRadioButton;
	CButton mPlayerRadioButton;
	CButton mKeywordRadioButton;
	CButton mUserIndexButton;
	CButton mConsignIndexButton;
};

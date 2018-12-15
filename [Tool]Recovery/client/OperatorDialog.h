#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// COperatorDialog 대화 상자입니다.

class COperatorDialog : public CDialog
{
	DECLARE_DYNAMIC(COperatorDialog)

public:
	COperatorDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~COperatorDialog();

	void Parse( DWORD serverIndex, const MSGROOT* );
	void Request( DWORD serverIndex );
	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OPERATORDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	CclientApp&	mApplication;
	DWORD		mServerIndex;

	CListCtrl mIpListCtrl;
	CListCtrl mOperatorListCtrl;
	CEdit mIdEdit;
	CEdit mNameEdit;
	CComboBox mRankCombo;
	CButton mAddButton;
	CIPAddressCtrl mIpAddressCtrl;
	virtual BOOL OnInitDialog();	
	//afx_msg void OnBnClickedOperatorRemoveButton();
	afx_msg void OnBnClickedOperatorAddButton();
	CEdit mAddIdEdit;
	CEdit mAddNameEdit;
	afx_msg void OnBnClickedOperatorIpAddButton();
	afx_msg void OnBnClickedOperatorIpRemoveButton();
	afx_msg void OnLvnItemchangedOperatorListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedIpListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOperatorUpdateButton();
	CStatic mRankStatic;
	afx_msg void OnBnClickedOperatorRemoveButton();
};

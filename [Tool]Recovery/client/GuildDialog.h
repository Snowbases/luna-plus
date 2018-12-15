#pragma once
#include "afxwin.h"
#include "afxcmn.h"
//#include "afxwin.h"
//#include "afxcmn.h"


class CclientApp;


// CGuildDialog 대화 상자입니다.

class CGuildDialog : public CDialog
{
	DECLARE_DYNAMIC(CGuildDialog)

public:
	CGuildDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGuildDialog();

	virtual void OnFinalRelease();
	void Request( const CString& serverName, const CString& playerName, DWORD playerIndex );

	void Parse( DWORD serverIndex, const MSGROOT* );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GUILDDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


protected:
	CString GetServerName() const;

protected:
	DWORD	mServerIndex;
	DWORD	mGuildIndex;
	MSG_RM_GUILD_DATA	mDataMessage;


	CclientApp&	mApplication;
	//CComboBox mServerCombo;
	CEdit mKeywordEdit;
	CButton mFindButton;
	CComboBox mResultCombo;
	CButton mDeletedGuildCheck;
	CButton mSpecificDataButton;
	CEdit mNameEdit;
	CEdit mBuildDateEdit;
	CEdit mLevelEdit;
	CEdit mScoreEdit;
	CEdit mAllianceEdit;
	CButton mDataUpdateButton;
public:
	CListCtrl mStoreItemList;
	CEdit mMoneyEdit;
	CButton mMoneyUpdateButton;
	CButton mStoreLogButton;
	CListCtrl mMemberList;
	CButton mViewMemberButton;
	CListCtrl mFriendlyList;
	CButton mFriendlyDataButton;
	CListCtrl mEnemyList;
	CButton mEnemeyDataButton;
	virtual BOOL OnInitDialog();
	//afx_msg void OnEnChangeGuildUnionNameEdit2();
	CEdit mAllianceEntryDateEdit;
	afx_msg void OnBnClickedGuildFindButton();
	afx_msg void OnBnClickedGuildSpecificDataButton();
	afx_msg void OnBnClickedGuildDataUpdateButton();
	afx_msg void OnBnClickedGuildMoneyUpdateButton();
	afx_msg void OnBnClickedGuildSelectedMemberButton();
	afx_msg void OnBnClickedGuildFriendlyDataButton();
	afx_msg void OnBnClickedGuildEnemyDataButton();
	CStatic mResultSizeStatic;
	afx_msg void OnBnClickedGuildLogButton();
	afx_msg void OnBnClickedGuildStoreLogButton();
	afx_msg void OnBnClickedGuildScoreLogButton();
	CComboBox mServerCombo;
	afx_msg void OnBnClickedGuildMemberModifyButton();
	CComboBox mRankCombo;
	afx_msg void OnLvnItemchangedGuildMemberList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeGuildRankCombo();
	afx_msg void OnLvnColumnclickGuildMemberList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedGuildSkillDeleteButton();
	CListCtrl mSkillListCtrl;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedGuildLogAllButton();
};

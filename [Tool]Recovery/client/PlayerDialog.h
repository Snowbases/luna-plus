#pragma once
#include "afxwin.h"
#include "afxcmn.h"


class CclientApp;


// CPlayerDialog 대화 상자입니다.

class CPlayerDialog : public CDialog
{
	DECLARE_DYNAMIC(CPlayerDialog)

public:
	CPlayerDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPlayerDialog();

	virtual void OnFinalRelease();
	virtual BOOL	OnInitDialog();

	void Request( DWORD serverIndex, const CString& playerName, DWORD playerIndex );
	void Request( const CString& serverName, const CString& playerName, DWORD playerIndex );
	void Parse( DWORD serverIndex, const MSGROOT* );


protected:
	CclientApp& mApplication;

	DWORD	mPlayerIndex;
	// 080731 LUJ, 패밀리 정보를 효율적으로 읽기 위해 추가
	DWORD	mFamilyIndex;
	CString	mPlayerName;
	CString	mServerName;
	DWORD	mUserIndex;

	CGridCtrl mDataGrid;
	CGridCtrl mExtendedDataGrid;
	CGridCtrl mRemovedDataGrid;
	CGridCtrl mFishingDataGrid;
	CGridCtrl mCookDataGrid;
	CGridCtrl mHouseDataGrid;

	typedef DWORD NoteIndex;
	struct NoteData
	{
		CString mSendedPlayer;
		CString mMemo;
		BOOL mIsRead;
		DWORD mItemIndex;
		DWORD mPackageItemIndex;
		MONEYTYPE mPackageMoney;
		BOOL mIsStored;
	};
	typedef std::map< NoteIndex, NoteData > NoteContainer;
	NoteContainer mNoteContainer;

	// stored received data from server
	MSG_RM_PLAYER_DATA				mReceivedDataMessage;
	MSG_RM_PLAYER_EXTENDED_DATA		mReceivedExtendedMessage;
	MSG_RM_PLAYER_LOVE				mReceivedLicenseMessage;	
	DWORD	mServerIndex;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PLAYERDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void PutNote() const;

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	afx_msg void OnViewStatusBar();
//	afx_msg void OnBnClickedPlayerDialogUpdate();
	afx_msg void OnBnClickedPlayerUpdateButton();
	//CListCtrl mJobList;
	//CStatic mJobCategoryStatic;

	CEdit mLoveScoreEdit;
	CTreeCtrl mGoodThingsTree;
	CTreeCtrl mBadThingsTree;
	CButton mLicenseUpdateButton;
	afx_msg void OnBnClickedPlayerLicenseUpdateButtno();
	CEdit mAgeEdit;
	afx_msg void OnBnClickedPlayerExpLogButton();
	afx_msg void OnBnClickedPlayerStatLogButton();
	//CButton mGuildButton;
	afx_msg void OnBnClickedPlayerGuildButton();
	CEdit mUserNameEdit;
	afx_msg void OnBnClickedPlayerUserViewButton();
	//afx_msg void OnBnClickedPlayerJobLogButton();
	afx_msg void OnBnClickedPlayerJobButton();
	afx_msg void OnBnClickedPlayerItemLogButton();
	//afx_msg void OnBnClickedPlayerItemAddButton();
	CComboBox mAreaCombo;
	CButton mGuildButton;
	CComboBox mJob1Combo;
	CComboBox mJob2Combo;
	CComboBox mJob3Combo;
	CComboBox mJob4Combo;
	CComboBox mJob5Combo;
	CComboBox mJob6Combo;
	afx_msg void OnBnClickedPlayerJobSubmitButton();
	afx_msg void OnCbnSelchangePlayerJob1Combo();
	afx_msg void OnCbnSelchangePlayerJob2Combo();
	afx_msg void OnCbnSelchangePlayerJob3Combo();
	afx_msg void OnCbnSelchangePlayerJob4Combo();
	afx_msg void OnCbnSelchangePlayerJob5Combo();
	afx_msg void OnCbnSelchangePlayerJob6Combo();
	CTabCtrl mTabCtrl;
	afx_msg void OnTcnSelchangePlayerDataTab(NMHDR *pNMHDR, LRESULT *pResult);
	CButton mDataSubmitButton;
	afx_msg void OnBnClickedPlayerNameLogButton();
	afx_msg void OnBnClickedPlayerFamilyButton();
	CButton mFamilyButton;
	afx_msg void OnBnClickedFishLogButton();
	// 080716 LUJ, 펫 로그 창 표시
	afx_msg void OnBnClickedPlayerPetLogButton();
	afx_msg void OnBnClickedPlayerFamilyLogButton();
	afx_msg void OnBnClickedPlayerLogButton();
	afx_msg void OnBnClickedPlayerMoneyLogButton();
	afx_msg void OnBnClickedPlayerAutonoteLogButton();
	afx_msg void OnBnClickedPlayerHouse();
	afx_msg void OnBnClickedPlayerCookRecipeAddButton();
	afx_msg void OnBnClickedPlayerCookRecipeRemoveButton();
	afx_msg void OnBnClickedPlayerTabLogButton();
	afx_msg void OnBnClickedPlayerTabLogButton2();
	CListCtrl mLearnedRecipeListCtrl;
	CListCtrl mForgottenRecipeListCtrl;
	CListCtrl mMailBoxListCtrl;
	afx_msg void OnLvnItemchangedPlayerMailBoxListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedPlayerForbidChatLogButton();
private:
	CComboBox mPartyCombo;
};

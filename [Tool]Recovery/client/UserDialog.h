#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CUserDialog 대화 상자입니다.

class CUserDialog : public CDialog
{
	DECLARE_DYNAMIC(CUserDialog)

public:
	CUserDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUserDialog();

	virtual void OnFinalRelease();

	void Request( DWORD serverIndex, const CString& userName, DWORD userIndex );
	void Parse( DWORD serverIndex, const MSGROOT* );

	//void AddLivePlayer( DWORD playerIndex, const CString& name, DWORD level );
	void RestorePlayer( DWORD playerIndex, const CString& name );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_USERDIALOG };

protected:
	CclientApp& mApplication;

	DWORD		mUserIndex;
	DWORD		mServerIndex;
	CString		mUserName;

	CEdit mJoinedDateEdit;
	CEdit mLogoutDateEdit;
	CListCtrl mLivePlayerListCtrl;
	CStatic mAgentStatic;
	CListCtrl mDeletedPlayerListCtrl;
	CButton mLivePlayerButton;
	CEdit midEdit;
	CStatic mConnectStatic;
	CStatic mNameStatic;
	CComboBox mLevelCombo;
	CComboBox mStateCombo;
	CComboBox mAutoPunishStateCombo;
	CEdit mAutoPunishStartDate;
	CEdit mAutoPunishEndDate;
	CEdit mAutoPunishTime;
	CGridCtrl mGameRoomGridCtrl;

	MSG_RM_USER_DATA	mReceivedData;

	// 080725 LUJ, 상태 상수
	enum State
	{
		StateNone,
		StateLive	= 1,
		StateBlock	= 6,
		StateSecede	= 8,
	};

	// 090618 ShinJS --- Punish 상태 추가
	enum PunishState
	{
		StateAutoPunishLogin = 0,
		StateAutoPunishUseNote = 3,
		StateAutoPunishBan = 4,
		StateAutoPunishNone,
		StateAutoPunishNoChange,
	};

	// 080725 LUJ, 상태에 관한 문자열 반환
	CString GetText( State ) const;
	// 090618 ShinJS --- Punish 상태에 관한 문자열 반환
	CString GetText( PunishState ) const;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedUserLivePlayerButton();
	afx_msg void OnBnClickedUserUpdateButton();
	afx_msg void OnBnClickedUserCreateButton();
	afx_msg void OnBnClickedUserRestoreButton();
	afx_msg void OnBnClickedUserDeleteButton();
	afx_msg void OnBnClickedUserItemShopLogButton();
	afx_msg void OnBnClickedUserRemovePlayerButton();	
	afx_msg void OnBnClickedUserGameroomLogButton();
	afx_msg void OnBnClickedUserGameroomUpdateButton();
};

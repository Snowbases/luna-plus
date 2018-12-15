#pragma once
#include "afxwin.h"


// CUserCreateDialog 대화 상자입니다.

class CUserCreateDialog : public CDialog
{
	DECLARE_DYNAMIC(CUserCreateDialog)

public:
	CUserCreateDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUserCreateDialog();

	virtual void OnFinalRelease();
	//void Request( const CString& serverName, const CString& playerName, DWORD playerIndex );
	//void Parse( DWORD serverIndex, const MSGROOT* );
	
	// 대화 상자 데이터입니다.
	enum { IDD = IDD_USERCREATEDIALOG };

	const CString& GetName() const;

	int	GetRace() const;
	int	GetJob() const;
	int	GetGender() const;
	int	GetHair() const;
	int	GetFace() const;

protected:
	CclientApp& mApplication;

	int		mRace;
	int		mJob;
	int		mGender;
	int		mHair;
	int		mFace;
	CString mName;

	CHARACTERMAKEINFO	mMessage;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	//virtual INT_PTR DoModal( DWORD serverIndex, DWORD userIndex, const CString& userName );
	CComboBox mRaceCombo;
	CComboBox mJobCombo;
	CComboBox mGenderCombo;
	CComboBox mHairCombo;
	CComboBox mFaceCombo;
	CEdit mNameEdit;

	virtual void OnOK();
	//afx_msg void OnBnClickedUserCreateButton();
	//afx_msg void OnBnClickedOk();
};

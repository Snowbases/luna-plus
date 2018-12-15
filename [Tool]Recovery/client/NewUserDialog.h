/**********************************************************************

이름: NewUserDialog
작성: 2007/10/25 12:14:16, 이웅주

목적: 새로운 사용자를 검색할 때 쓰임

***********************************************************************/
#pragma once
#include "afxwin.h"


class CclientApp;


// CNewUserDialog 대화 상자입니다.

class CNewUserDialog : public CDialog
{
	DECLARE_DYNAMIC(CNewUserDialog)

public:
	CNewUserDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNewUserDialog();

//	virtual void OnFinalRelease();

	void Parse( DWORD serverIndex, const MSGROOT* );

	void FindPlayer( DWORD serverIndex, DWORD playerIndex );

protected:	
	CclientApp&	mApplication;
	CComboBox	mServerComboBox;
	CComboBox	mPlayerComboBox;
	CString		mKeyword;
	CButton		mIdRadioButton;
	CButton		mNameRadioButton;
	CButton		mIndexRadioButton;

	DWORD		mServerIndex;

	// 키: 플레이어 인덱스
	struct Player 
	{
		DWORD	mPlayerIndex;
		DWORD	mServerIndex;	// 해당 플레이어를 조회한 서버
		CString	mName;
		DWORD	mUserIndex;
		DWORD	mStandIndex;	// 5는 삭제된 캐릭
	};

	friend bool SortByStandIndex( const Player&, const Player& );
	
	// 키: 사용자 콤보 박스에서의 순서. 리스트는 플레이어 콤보 박스에 든 순서대로이다
	typedef std::list< Player >						PlayerList;
	typedef	stdext::hash_map< DWORD, PlayerList >	UserMap;
	UserMap											mUserMap;

	struct User
	{
		CString	mName;
		DWORD	mUserIndex;
	};

	// 유저 이름과 번호가 리스트에 담겨 있다
	typedef std::list< User >	UserList;
	UserList					mUserList;


protected:
	//CString			GetPlayerName() const;
	DWORD			GetServerIndex() const;
	//DWORD			GetPlayerIndex() const;
	const Player&	GetPlayer() const;
	CString GetUserName(LPCTSTR loginName, LPCTSTR enpangName) const;
	DWORD GetUserIndex(LPCTSTR) const;

	enum { IDD = IDD_NEWUSERDIALOG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL	OnInitDialog();
	afx_msg void OnBnClickedNewuserSearchButton();
	afx_msg void OnMenuNewuserDialog();
	afx_msg void OnBnClickedNewuserPlayerButton();
	afx_msg void OnBnClickedNewuserItemButton();
	afx_msg void OnBnClickedNewuserSkillButton();
	afx_msg void OnBnClickedNewuserButtonQuest();
protected:
	//	virtual void OnOK();
//	virtual void OnCancel();
public:
	//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//afx_msg void OnBnClickedNewuserButtonQuest2();
	CStatic mPlayerSizeStatic;
	CButton mPlayerRadioButton;
	CComboBox mUserComboBox;
	CStatic mUserSizeStatic;
	afx_msg void OnCbnSelchangeNewuserUserCombo();
	afx_msg void OnBnClickedNewuserUserDataButton();
	CEdit mKeywordEdit;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CButton mUserIndexRadioButton;
};
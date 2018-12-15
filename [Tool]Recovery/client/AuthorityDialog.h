/*
081021 LUJ, 권한 관리 창 추가
*/
#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CAuthorityDialog 대화 상자입니다.

class CAuthorityDialog : public CDialog
{
	DECLARE_DYNAMIC(CAuthorityDialog)

public:
	CAuthorityDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAuthorityDialog();

	virtual void OnFinalRelease();
	void Parse( DWORD serverIndex, const MSGROOT* );
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoModal( DWORD serverIndex );
	virtual void OnOK();
	
// 대화 상자 데이터입니다.
	enum { IDD = IDD_AUTHORITYDIALOG };
private:
	// 081021 LUJ, 탭에서 선택된 권한을 가져온다
	eGM_POWER	GetRankFromTab() const;
private:
	CclientApp& mApplication;
	DWORD		mServerIndex;
	// 081021 LUJ, 서버에서 전송된 권한 정보
	typedef std::list< MSG_RM_AUTHORITY::Authority >		AuthorityList;
	typedef stdext::hash_map< eGM_POWER, AuthorityList >	AuthorityMap;
	AuthorityMap											mAuthorityMap;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	CTabCtrl mLevelTabCtrl;
	CButton mPlayerLevelReadButton;
	CButton mPlayerLevelWriteButton;
	CButton mPlayerCreateReadButton;
	CButton mPlayerCreateWriteButton;
	CButton mPlayerDeleteReadButton;
	CButton mPlayerDeleteWriteButton;
	CButton mPlayerRecoverReadButton;
	CButton mPlayerRecoverWriteButton;
	CButton mPlayerUpdateReadButton;
	CButton mPlayerUpdateWriteButton;
	CButton mSkillReadButton;
	CButton mSkillWriteButton;
	CButton mQuestReadButton;
	CButton mQuestWriteButton;
	CButton mGuildReadButton;
	CButton mGuildWriteButton;
	CButton mFamilyReadButton;
	CButton mFamilyWriteButton;
	CButton mPlayerLogReadButton;
	CButton mPlayerLogWriteButton;
	CButton mOperatorReadButton;
	CButton mOperatorWriteButton;
	CButton mAllowIpReadButton;
	CButton mAllowIpWriteButton;
	CButton mOperatorLogReadButton;
	CButton mOperatorLogWriteButton;
	CButton mItemReadButton;
	CButton mItemWriteButton;
	afx_msg void OnTcnSelchangeAuthorityTab(NMHDR *pNMHDR, LRESULT *pResult);
};
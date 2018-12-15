#pragma once
#include "afxcmn.h"
//#include "afxwin.h"


class CclientApp;


// CItemFindDialog 대화 상자입니다.

class CItemFindDialog : public CDialog
{
	DECLARE_DYNAMIC(CItemFindDialog)

public:
	CItemFindDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CItemFindDialog();

	virtual void OnFinalRelease();
	void Parse( DWORD serverIndex, const MSGROOT* );


protected:
	void PutPage( DWORD index );
	int GetCurrentPage() const;

protected:
	CclientApp&	mApplication;
	CListCtrl	mResultListCtrl;
	CListCtrl	mItemListctrl;
	CComboBox	mServerComboBox;
    CEdit		mKeywordEdit;
	CStatic		mPageStatic;
	CStatic		mResultStatic;
	CString		mSelectedServerName;
	CButton		mPagePrevButton;
	CButton		mPageNextButton;
	CButton		mViewPlayerButton;
	CButton		mFindButton;
	CButton		mMoreFindButton;

	struct Player
	{
		DWORD	mItemIndex;
		DWORD	mItemSize;	// 아이템 보유 개수
		CString mName;		// 플레이어 이름
		DWORD	mUserIndex;
	};

	// 키: 플레이어 인덱스
	typedef std::map< DWORD, Player >	Page;

	// 키: 쪽 번호
	typedef std::map< DWORD, Page >		PageMap;
	PageMap								mPageMap;


// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMFINDDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedItemFindPrevPageButton();
	afx_msg void OnBnClickedItemFindNextPageButton();
	
	
	afx_msg void OnBnClickedItemFindMoreSearchButton();
	
	
	afx_msg void OnBnClickedItemFindViewPlayer();
	afx_msg void OnBnClickedItemFindSearchButton();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnChangeItemFindKeyword();
};

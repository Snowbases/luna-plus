#pragma once
#include "afxwin.h"
#include "afxcmn.h"


class CclientApp;


// CQuestDialog 대화 상자입니다.

class CQuestDialog : public CDialog
{
	DECLARE_DYNAMIC(CQuestDialog)

public:
	CQuestDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CQuestDialog();

	void Parse( DWORD serverIndex, const MSGROOT* );
	virtual void OnFinalRelease();

	void Request( DWORD serverIndex, const CString& playerName, DWORD playerIndex );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_QUESTDIALOG };

protected:
	void RefreshSubQuest();
	void RefreshItem();
	LPCTSTR GetTimeText(__time64_t) const;
	BOOL IsEndSubQuest( DWORD subQuestIndex, DWORD state ) const;

protected:
	CclientApp&	mApplication;
	DWORD		mServerIndex;
	DWORD		mPlayerIndex;
	CString		mPlayerName;

	//CListCtrl	mMainListCtrl;
	CListCtrl	mSubListCtrl;
	CListCtrl	mItemListCtrl;
	CListCtrl	mMonsterListCtrl;
	//CGridCtrl	mDataGrid;
	CStatic		mDescrptionStatic;

	// 메인퀘스트 정보. 키: 메인 퀘스트 인덱스
	typedef std::map< DWORD, QMBASE >	MainQuestMap;
	MainQuestMap						mMainQuestMap;

	// 키: 서브퀘스트 인덱스
	typedef std::map< DWORD, QSUBASE >	SubQuest;

	// 서브 퀘스트 정보, 키: 메인 퀘스트 인덱스
	typedef std::map< DWORD, SubQuest >	SubQuestMap;
	SubQuestMap							mSubQuestMap;

	// 퀘스트 아이템 정보, 키: 퀘스트 아이템 인덱스
	typedef std::map< DWORD, QITEMBASE >	ItemMap;
	ItemMap									mItemMap;

	// 중복 퀘스트 체크. 키: 퀘스트 인덱스, 값: 개수
	typedef std::map< DWORD, DWORD >	CheckMap;
	CheckMap							mCheckMap;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedQuestMainList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedQuestSubList(NMHDR *pNMHDR, LRESULT *pResult);
	CButton mFinishSubQuestButton;
	afx_msg void OnBnClickedQuestSelectedOneFinishButton();
	afx_msg void OnBnClickedQuestMainDeleteButton();
	CListCtrl mMainListCtrl;
	CListCtrl mTotalItemListCtrl;
	afx_msg void OnLvnItemchangedQuestItemTotalList(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic mItemDescriptionStatic;
	//CComboBox mItemSizeCombo;
	CButton mItemUpdateButton;
	afx_msg void OnBnClickedQuestItemUpdateButton();
	//afx_msg void OnBnClickedQuestItemUpdateButton2();
	CEdit mItemSizeEdit;
	afx_msg void OnBnClickedQuestItemDeleteButton();
	afx_msg void OnBnClickedQuestLogButton();
	CStatic mReportStatic;
	afx_msg void OnBnClickedQuestItemAddButton();
	CStatic mMainQuestStatic;
};

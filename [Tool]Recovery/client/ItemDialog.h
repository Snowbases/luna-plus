#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CclientApp;

class CItemDialog : public CDialog
{
	DECLARE_DYNAMIC(CItemDialog)

public:
	CItemDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CItemDialog();
	virtual void OnFinalRelease();
	void Parse( DWORD serverIndex, const MSGROOT* );
	void Request( DWORD serverIndex, const CString& playerName, DWORD playerIndex, DWORD userIndex );

protected:
	CclientApp&	mApplication;	
	// 개수 등 아이템의 일반적인 정보를 표시;
	CGridCtrl	mItemGrid;
	// 아이템 옵션 정보를 표시
	CGridCtrl	mOptionGrid;
	// 080320 LUJ, 드롭 옵션 그리드
	CGridCtrl	mDropOptionGrid;
	// 080716 LUJ, 펫 그리드
	CGridCtrl	mPetGrid;

	DWORD		mServerIndex;
	DWORD		mSelectedItemDbIndex;

	CString		mPlayerName;
	DWORD		mPlayerIndex;
	DWORD		mUserIndex;
	
	DWORD		mStorageSize;
	DWORD		mExtendedInventorySize;

	// 선택된 데이터를 표시한다
	void PutData( DWORD itemDbIndex );
	// 080401 LUJ, 반환 타입을 Item으로 변경
	// 080731 LUJ, 참조로 반환하도록 함
	Item&		GetItem( DWORD itemDbIndex );
	// 080731 LUJ, 특정 인벤토리에서 아이템 정보를 새로 고친다
	void		PutItem( const Item&, CListCtrl&, bool isAdd = false );
	// 080731 LUJ, 모든 인벤토리에서  아이템 정보를 새로 고친다
	void		PutItem( const Item&, bool isAdd = false );
	// 081201 LUJ, 옵션을 문자열로 출력한다
	CString		MakeText( const ITEM_OPTION& ) const;
	// 키: 아이템 DB인덱스
	typedef stdext::hash_map< DWORD, Item >	ItemMap;
	ItemMap									mItemMap;
	
	// 키: 아이템 DB인덱스
	typedef stdext::hash_map< DWORD, ITEM_OPTION >	OptionMap;
	OptionMap										mOptionMap;

	// 080716 LUJ, 펫 정보가 저장된 맵. 키: 아이템 DB인덱스
	typedef MSG_RM_PET::Pet					Pet;
	typedef stdext::hash_map< DWORD, Pet >	PetMap;
	PetMap									mPetMap;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedItemDialogInventory(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnBnClickedItemDialogDataUpdate();
	afx_msg void OnBnClickedItemDialogDataUpdateButton();
	//afx_msg void OnLvnItemchangedItemStorage(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedItemDataDeleteButton();
	//CStatic mPlayerIndexEdit;
	CStatic mDataFrameStatic;
	afx_msg void OnBnClickedItemInventoryLogButton();
	
	afx_msg void OnBnClickedItemAddButton();
	CStatic mDataFrame;
	CEdit mEndTimeStatic;
	CEdit mRemainTimeStatic;
	//afx_msg void OnEnSetfocusItemEndTimeStatic();
	//afx_msg void OnEnSetfocusItemRemainTimeStatic();
	afx_msg void OnBnClickedItemEndTimeButton();
	afx_msg void OnBnClickedItemRemainTimeButton();
	CComboBox mStorageSizeCombo;
	CEdit mStorageMoneyEdit;
	// 080611 LUJ, 봉인 토글 버튼
	CButton	mSealingRadioButton;
	CTabCtrl mValueTabCtrl;
	afx_msg void OnTcnSelchangeItemTab(NMHDR *pNMHDR, LRESULT *pResult);
	CButton mItemAddButton;
	afx_msg void OnBnClickedItemStorageUpdateButton();
	CButton mStorageUpdateButton;
	CButton mSafeMoveCheckBox;

	// 080328 LUJ, 도움말 표시
	afx_msg void OnBnClickedItemHelpButton();
	afx_msg void OnBnClickedItemDialogSealRadio();
	CButton mRemainTimeButton;
	CButton mEndTimeButton;
	CButton mUpdateDataButton;
	CListCtrl mInventoryListCtrl;
	CListCtrl mPetInventoryListCtrl;
	CListCtrl mHouseInventoryListCtrl;
	CTabCtrl mInventoryTabCtrl;
	// 080716 LUJ, 인벤토리 탭이 바뀔 때마다 표시될 리스트 컨트롤을 결정한다
	afx_msg void OnTcnSelchangeItemDialogInventoryTab(NMHDR *pNMHDR, LRESULT *pResult);
	// 080716 LUJ, 펫 인벤토리 목록 중에서 선택할 경우 실행됨
	afx_msg void OnLvnItemchangedItemPetInventoryList(NMHDR *pNMHDR, LRESULT *pResult);
	CButton mPetSummonButton;
	afx_msg void OnBnClickedItemPetSummonButton();
	afx_msg void OnBnClickedItemCopyClipboard();
	afx_msg void OnBnClickedItemSaveExcelButton();
};

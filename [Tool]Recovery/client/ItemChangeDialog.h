#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CItemChangeDialog 대화 상자입니다.

class CItemChangeDialog : public CDialog
{
	DECLARE_DYNAMIC(CItemChangeDialog)

public:
	CItemChangeDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CItemChangeDialog();

	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMCHANGEDIALOG };

private:
	CclientApp& mApplication;

	// 090304 LUJ, 스크립트를 읽는다
private:
	struct ChangeScript
	{
		ChangeScript() :
		mItemIndex( 0 ),
		mRate( 0 )
		{}

		DWORD	mItemIndex;
		typedef size_t MinimumCount;
		typedef size_t MaximumCount;
		typedef std::pair< MinimumCount, MaximumCount > Count;
		Count	mCount;
		typedef float Rate;
		Rate mRate;
	};
	typedef float Rate;
	typedef std::map< Rate, ChangeScript > ChangeScriptMap;
	typedef std::list< ChangeScriptMap > ChangeScriptMapList;
	typedef DWORD ItemIndex;
	typedef stdext::hash_map< ItemIndex, ChangeScriptMapList > ItemChangeScriptMap;
	static ItemChangeScriptMap mItemChangeScriptMap;
	const int mMaxChangeCount;
	const int mDefaultChangecount;
private:
	void LoadScript();
	void SetItemCombo( CComboBox& );
	ItemIndex GetSelectedItemIndex() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
	CListCtrl mResultListCtrl;
	CComboBox mItemComboBox;
	CStatic mItemDescriptionStatic;
	CToolTipCtrl mItemToolTipCtrl;
	CButton mSubmitButton;

public:
	virtual BOOL OnInitDialog();	
	afx_msg void OnCbnSelchangeItemchangeItemList();
	afx_msg void OnBnClickedItemChangeSubmitButton();
	afx_msg void OnBnClickedItemChangeCopyClipboardButton();
	virtual BOOL PreTranslateMessage( MSG* );
	afx_msg void OnBnClickedItemChangeFindButton();
	CEdit mCountEdit;
	CButton mCountMaxButton;
	afx_msg void OnBnClickedItemChangeCountMaxButton();
};
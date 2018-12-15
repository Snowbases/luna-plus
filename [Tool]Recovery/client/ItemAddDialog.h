#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CItemAddDialog 대화 상자입니다.

class CItemAddDialog : public CDialog
{
	DECLARE_DYNAMIC(CItemAddDialog)

public:
	CItemAddDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CItemAddDialog();

	//void Parse( DWORD serverIndex, const MSGROOT* );

	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMADDDIALOG };

	// 선택된 아이템 번호를 반환한다. 없으면 0이 반환된다
	DWORD	GetSelectedItemIndex() const;

protected:
	//CString mServerName;
	//CString	mPlayerName;
	//DWORD	mServerIndex;
	//DWORD	mPlayerIndex;
	CclientApp&	mApplication;

	DWORD	mSelectedItemIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeItemAddTab(NMHDR *pNMHDR, LRESULT *pResult);
	CTabCtrl mTabCtrl;
	CListCtrl mWeaponListCtrl;
	CListCtrl mPotionListCtrl;
	CListCtrl mMaterialListCtrl;
	CListCtrl mEtcListCtrl;
	CToolTipCtrl mItemToolTipCtrl;
protected:
	virtual void OnOK();
public:
	CEdit mItemIndexEdit;
	CStatic mItemNameStatic;
	afx_msg void OnEnChangeItemAddIndexEdit();
	afx_msg void OnLvnItemchangedItemAddListWeapon(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedItemAddListPotion(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedItemAddListMaterial(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox mItemNameCombo;
	afx_msg void OnCbnSelchangeItemAddCombo();
	
	afx_msg void OnLvnItemchangedItemAddListEtc(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit mItemDescriptionEdit;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// QuestItemAddDialog 대화 상자입니다.

class CQuestItemAddDialog : public CDialog
{
	DECLARE_DYNAMIC(CQuestItemAddDialog)

public:
	CQuestItemAddDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CQuestItemAddDialog();

	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_QUESTITEMADDDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	afx_msg void OnEnChangeQuestItemAddEdit();
	afx_msg void OnLvnItemchangedQuestItemFindedQuestList(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl mQuestListCtrl;
	CListCtrl mitemListCtrl;
	
protected:
	virtual void OnOK();

	CclientApp& mApplication;
	
	DWORD	mQuestItemIndex;

public:
	virtual BOOL OnInitDialog();

	DWORD GetQuestItemIndex() const;
	CEdit mKeywordEdit;
	CStatic mQuestDescriptionStatic;
	CStatic mItemDescriptionStatic;
	afx_msg void OnLvnItemchangedQuestItemFindedItemList(NMHDR *pNMHDR, LRESULT *pResult);
	CButton mQuestRadio;
	CButton mItemRadio;
};

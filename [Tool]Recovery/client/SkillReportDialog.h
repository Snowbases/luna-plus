#pragma once
#include "afxcmn.h"


// CSkillReportDialog 대화 상자입니다.

class CSkillReportDialog : public CDialog
{
	DECLARE_DYNAMIC(CSkillReportDialog)

public:
	CSkillReportDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSkillReportDialog();

	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SKILLREPORTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
//	virtual INT_PTR DoModal();

protected:
	CclientApp& mApplication;

public:
	virtual BOOL OnInitDialog();
	CListCtrl mReportListCtrl;
};
#pragma once
#include "afxcmn.h"


// CMemoDialog 대화 상자입니다.

class CMemoDialog : public CDialog
{
	DECLARE_DYNAMIC(CMemoDialog)

public:
	CMemoDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMemoDialog();
	void SetText(LPCTSTR);

	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MEMODIALOG };

private:
    CString mText;
	CRichEditCtrl mMemoEditCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

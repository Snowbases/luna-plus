#pragma once
#include "afxwin.h"


// CQuantityDialog 대화 상자입니다.

class CQuantityDialog : public CDialog
{
	DECLARE_DYNAMIC(CQuantityDialog)

public:
	CQuantityDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CQuantityDialog();

	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_QUANTITYDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	CComboBox mQuantityCombo;
	afx_msg void OnBnClickedOk();


private:
	int	mQuantity;
public:
	int GetQuantity() const;

	virtual BOOL OnInitDialog();
};

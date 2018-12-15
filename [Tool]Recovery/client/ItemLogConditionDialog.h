#pragma once
#include "afxwin.h"


// CItemLogConditionDialog 대화 상자입니다.

class CItemLogConditionDialog : public CDialog
{
	DECLARE_DYNAMIC(CItemLogConditionDialog)

public:
	CItemLogConditionDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CItemLogConditionDialog();

	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMLOGCONDITIONDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	CEdit mItemIndexEdit;
	CEdit mItemDbIndexEdit;
	CEdit mPlayerIndexEdit;
	CComboBox mLogTypeCombo;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	
	struct InputData
	{
		DWORD			mPlayerIndex;
		DWORD			mItemIndex;
		DWORD			mItemDbIndex;
		eLogitemmoney	mLogType;
	};

	inline const InputData& GetInputData() const
	{
		return mInputData;
	}

private:
	CclientApp&	mApplication;	
	InputData mInputData;

	typedef std::pair< CString, eLogitemmoney >	LogData;
	typedef std::list< LogData >				LogDataList;
	LogDataList									mLogDataList;
public:
	afx_msg void OnBnClickedItemLogConditionItemIdxButton();
};

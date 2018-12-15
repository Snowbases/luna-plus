#pragma once
#include "afxwin.h"


// CDateDialog 대화 상자입니다.

class CDateDialog : public CDialog
{
	DECLARE_DYNAMIC(CDateDialog)

public:
	CDateDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDateDialog();

	DWORD GetSecond() const;

	virtual void OnFinalRelease();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DATEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

protected:
	DWORD		mSecond;
	CString		mDateString;


public:
	CStatic mPreviousDateStatic;
	CComboBox mYearCombo;
	CComboBox mMonthCombo;
	CComboBox mDayCombo;
	CComboBox mHourCombo;
	CComboBox mMinuteCombo;
	CComboBox mSecondCombo;
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoModal( DWORD second, const CString& dateString );
protected:
	virtual void OnOK();
};

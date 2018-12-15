/*
080401 LUJ, 로그 창. 커맨드 패턴을 사용하여 같은 인터페이스를 가지고 여러 동작을 할 수 있게 한다
*/
#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"


class CCommand;


class CLogDialog : public CDialog
{
	DECLARE_DYNAMIC(CLogDialog)

public:
	CLogDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLogDialog();

	virtual void OnFinalRelease();

	void Parse( DWORD serverIndex, const MSGROOT* );

	
// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOGDIALOG };


private:
	DWORD		mServerIndex;
	CCommand*	mCommand;
	CclientApp& mApplication;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoModal( DWORD serverIndex, CCommand& );

	afx_msg void OnBnClickedLogClipboardButton();
	afx_msg void OnBnClickedLogExcelButton();
	afx_msg void OnBnClickedLogFindButton();
	afx_msg void OnBnClickedLogStopButton();
	CListCtrl mResultListCtrl;
	CStatic mResultStatic;
	CDateTimeCtrl mBeginDateCtrl;
	CDateTimeCtrl mBeginTimeCtrl;
	CDateTimeCtrl mEndDateCtrl;
	CDateTimeCtrl mEndTimeCtrl;
	CButton mFindButton;
	CButton mStopButton;
	CProgressCtrl mProgressCtrl;
	CComboBox mServerCombo;
	CStatic mMaxLogStatic;
};

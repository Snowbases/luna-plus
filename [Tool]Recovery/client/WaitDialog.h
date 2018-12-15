#pragma once


#include "PictureEx.h"
#include "afxwin.h"


// CWaitDialog 대화 상자입니다.

class CWaitDialog : public CDialog
{
	DECLARE_DYNAMIC(CWaitDialog)

public:
	CWaitDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWaitDialog();

	virtual void OnFinalRelease();
	virtual LRESULT SendMessage( UINT message, WPARAM = 0, LPARAM = 0 );

	void SetWaiting( bool );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WAITDIALOG };

protected:
	int mSecond;
	int mMaxSecond;

	bool	mIsWaiting;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	//afx_msg void OnPaint();
	CPictureEx m_Picture;
	afx_msg void OnTimer(UINT nIDEvent);
	CStatic mTimeStatic;
	CButton mCancelButton;
protected:
	virtual void OnCancel();
	virtual void OnOK();
};

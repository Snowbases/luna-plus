#if !defined(AFX_EVENTNOFIFYDLG_H__D133AD43_10EC_4926_A4C0_91DC9F61B919__INCLUDED_)
#define AFX_EVENTNOFIFYDLG_H__D133AD43_10EC_4926_A4C0_91DC9F61B919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventNofifyDlg.h : header file
//

#include "MyEdit.h"
#include "NotifyMessage.h"
/////////////////////////////////////////////////////////////////////////////
// CEventNofifyDlg dialog

class CEventNofifyDlg : public CDialog
{
// Construction
public:
	BOOL m_bAutoNotify;
	CBitmap		m_bmpGreen;
	int			m_nRepeatNum;
	int			m_nRepeatTime;
	int			m_nCheckTime;
	CNotifyMessage	m_NotifyMessage;
	CEventNofifyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEventNofifyDlg)
	enum { IDD = IDD_DIALOG_EVENTNOTIFY };
	CButton	m_btnEventMsgOff;
	CMyEdit	m_editMsg;
	CMyEdit	m_editTitle;
	CEdit	m_editRepeat;
	CEdit	m_editTime;
	CButton	m_btnNotify;
	CComboBox	m_cbServerSet;
	CString	m_strCurFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventNofifyDlg)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventNofifyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonEventNotify();
	afx_msg void OnButtonEventOpen();
	virtual void OnCancel();
	afx_msg void OnChangeEditEventTitle();
	afx_msg void OnChangeEditEventMsg();
	afx_msg void OnButtonOff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTNOFIFYDLG_H__D133AD43_10EC_4926_A4C0_91DC9F61B919__INCLUDED_)

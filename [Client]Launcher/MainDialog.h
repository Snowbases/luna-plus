#if !defined(AFX_MHAUTOPATCHDLG_H__C64FC497_45E4_40DC_AEB4_AB3A77ADF731__INCLUDED_)
#define AFX_MHAUTOPATCHDLG_H__C64FC497_45E4_40DC_AEB4_AB3A77ADF731__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HtmlViewNotice.h"
#include "BGMManager.h"
#include "OwnerDrawComboBox.h"


class CMainDialog : public CDialog
{
public:
	//CMainDialog(CWnd* pParent = NULL);
	CMainDialog( CWnd* pParent, CApplication::Config& );
	virtual ~CMainDialog();
	
	COwnerDrawComboBox m_ComboScreenSize;
	// Dialog Data
	enum { IDD = IDD_MHAUTOPATCH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	BOOL Clear3DData();

	BOOL RunLuna();
	
	BOOL ExtractFile( char* lpFileName);
	BOOL UpdateVersionInfoFile( char* lpszOldVerInfoFile, char* lpszNewVerInfoFile );

	void StartPack3DData();
	void SetReady();

	static BOOL SelectGFunc();
	static BOOL InitEngine();
	static void Pack3DData( void* p );
	
	BOOL ConnectFtp();
	void ReconnectFtp();

protected: 
	CHtmlViewNotice*	m_pHTML;

	// Generated message map functions
	//{{AFX_MSG(CMainDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnCancel();
	virtual void OnOK();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	
	CxImage m_backgroundImage;
	CxImage m_progressImage;
	CxImage m_homePageImage;
	CxImage m_registerImage;
	CxImage m_beginImage;
	CxImage m_closeImage;
	CxImage m_checkBoxImage;
	CxImage m_statusImage;
	CxImage m_comboBox;
	CxImage m_statusPacking;
	// 080507 LUJ, 이미지 경로 변경 콤보 커버
	CxImage m_InterfaceComboBox;

	struct ImageProgress
	{
		CxImage m_cellImage;
		float m_value;	// 0 ~ 1
	}
	m_progress;

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	void PutProgress( float ratio, bool isInstant = false, const CString* = 0 );

private:
	void PutStatus( const TCHAR* );
	void PutLog( const TCHAR* ) const;

	static CMainDialog* m_self;

	CApplication::Config& m_config;
	
	bool m_isReady;

public:
	static CMainDialog* GetInstance();
	afx_msg void OnCbnSelchangeScreensize();

	CString	m_progressText;

	// 080306 LUJ, ESC키를 눌러 프로그램을 종료하거나, 엔터키를 눌러 패치 중에 게임을 실행할 수 있어 이를 막는다
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	COwnerDrawComboBox m_ComboInterface;
	afx_msg void OnCbnSelchangeInterface();
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MHAUTOPATCHDLG_H__C64FC497_45E4_40DC_AEB4_AB3A77ADF731__INCLUDED_)



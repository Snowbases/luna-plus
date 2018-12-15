// ServerToolView.h : interface of the CServerToolView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERTOOLVIEW_H__FC61667A_7DCE_440B_A72D_800AB1ADFD7F__INCLUDED_)
#define AFX_SERVERTOOLVIEW_H__FC61667A_7DCE_440B_A72D_800AB1ADFD7F__INCLUDED_

//#include "ReservationDlg.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NotifyDlg.h"	// Added by ClassView
#include "NotifyMessage.h"	// Added by ClassView
#include "LoginDlg.h"	// Added by ClassView
#include "MsgBoxOptionDlg.h"	// Added by ClassView

class CMSInfoDlg;
class CUserCount;
class CEventNofifyDlg;

class CServerToolView : public CFormView
{
protected: // create from serialization only
	CServerToolView();
	DECLARE_DYNCREATE(CServerToolView)

public:
	//{{AFX_DATA(CServerToolView)
	enum { IDD = IDD_SERVERTOOL_FORM };
	CButton	m_btnMsgOption;
	CButton	m_btnUserCount;
	CButton	m_btnTServer;
	CButton	m_btnVersionChange;
	CButton	m_btnMaxUserChange;
	CEdit	m_editMaxUser;
	CEdit	m_editVersion;
	CButton	m_btnUserOut;
	CButton	m_btnServerRefresh;
	CButton	m_btnServerOff;
	CButton	m_btnServerOn;
	CComboBox	m_cbUserLevel;
	CButton	m_btnChangeUserLevel;
	CButton	m_btnAutoPatch;
	CEdit	m_editRepeatNum;
	CButton	m_stTotal;
	CListCtrl	m_lcTotal;
	CComboBox	m_cbServerSet;
	CString		m_strCurTime;
	CListCtrl	m_lcDist;
	CListCtrl	m_lcAgent;
	CListCtrl	m_lcMurim;
	CListCtrl	m_lcMap;
	CButton		m_stDist;
	CButton		m_stAgent;
	CButton		m_stMurim;
	CButton		m_stEtc;
	CButton		m_stMap;
	CComboBox	m_cbCmdServerSet;
	// 080829 LUJ, 멀티 라인 복사를 위해 표준 컨트롤 사용
	CEdit		m_RepeatEdit;
	CString	m_strCurFileName;
	CString	m_stUserCount;
	//}}AFX_DATA

// Attributes
public:
// 모달리스 대화상자 헤더문제로 지움....
//	CServerToolDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerToolView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
	
	void OnRdblClickServerList(CListCtrl* pListCtrl,WORD ServerKind);
	void OnDblClickServerList(CListCtrl* pListCtrl,WORD ServerKind);
	void OnClickServerList(CListCtrl* pListCtrl,WORD ServerKind);

private:
	// 080829 LUJ, 스크립트(ServerTool.cfg)에서 읽어들인 정보를 저장한다
	struct Script
	{
		CString mTernimateText;
	}
	mScript;

public:
	CEventNofifyDlg* m_pEventNotifyDlg;
	CMsgBoxOptionDlg m_MsgBoxOption;
	CFont		m_Font;
	CUserCount*	m_pUserCountDlg;
	CMSInfoDlg*	m_pMSInfoDlg;
	CLoginDlg	m_LoginDlg;
	CString		m_strDir;

	DWORD		m_AutoPatchTime;
	CBitmap		m_bmpGreen;
	CBitmap		m_bmpGray;
	CNotifyMessage	m_NotifyMessage;
	CNotifyDlg	m_NotifyDlg;	
	CImageList	m_StateImageList;
	// 090511 LUJ, 열거형 변수로 변경
	eOPERPOWER	m_nToolLevel;		// 사용 가능 레벨
	CString		m_strMessage;		// 공지 사항 message
	BOOL		m_bNotify;
	int			m_nRepeatNum;
	int			m_nRepeatTime;
	int			m_nCheckTime;	
	
	BOOL		m_bReconnect;
	CString		m_strAutoPatch;	
	DWORD		m_dwServerOffTime;
	DWORD		m_dwDisconNum;	
	WORD		m_wRefreshTime;
	WORD		m_wWriteCountTime;
	BOOL		m_bServerOff;
	int			m_nKill;
	
	BOOL	Init();
	void	InitServerControl();
	void	InitServerMonitor();
	void	InitCommand();

	void	UpdateServerControl();
	void	UpdateUserCount();
	void	UpdateServerMonitor();
	void	UpdateCommand();

	void	EnableServerControl( BOOL bShow );
	void	EnableServerMonitor( BOOL bShow );
	void	EnableCommand( BOOL bShow );
	// 090511 LUJ, 열거형 변수로 변경
	void	EnableToolFromLevel( eOPERPOWER );
	void	ShowServerMonitor( BOOL bShow );

	void	ReadIniFile( char* filename );
	void	ReConnectToMAS();
	
	BOOL	IsEnableTool( int nToolLevel );
	void	SetMSStateAll( int nServerSet, MSGMSSTATEALL* pState );
		
	// network
	void	NetworkMsgParse( int ServerSet, BYTE Category, BYTE Protocol, void* pMsg );
	void	SetServerState( int setindex, SERVERSTATE* pstate );	
	void	NetworkQueryVersion( int setindex, LPCTSTR version );
	void	NetworkChangeVersion( int setindex, LPCTSTR version );
	void	NetworkQueryMaxUser( int setindex, DWORD maxuser );
	void	NetworkChangeMaxUser( int setindex, DWORD maxuser );
	void	NetworkUserLevel( int setindex, DWORD userlevel );
	void	NetworkAllUserDisconnect( int setindex, SERVERSTATE* pstate );
	void	Disconnect( int serverset );

	virtual ~CServerToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
// Generated message map functions
protected:
	//{{AFX_MSG(CServerToolView)
	afx_msg void OnSelchangeComboServerset();
	afx_msg void OnButtonServerOn();
	afx_msg void OnButtonServerOff();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonCommandSend();
	afx_msg void OnDblclkListcontrolDist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListcontrolAgent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListcontrolMap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListcontrolMurim(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListcontrolMap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListcontrolDist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListcontrolAgent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListcontrolMurim(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnButtonServerRefresh();
	afx_msg void OnButtonAutopatch();
	afx_msg void OnFileMasOpen();
	afx_msg void OnButtonUserlevel();
	afx_msg void OnButtonMsgbox();
	afx_msg void OnButtonOffmsgbox();
	afx_msg void OnButtonOnmsgbox();
	afx_msg void OnButtonServerUserout();
	afx_msg void OnButtonVersionChange();
	afx_msg void OnButtonMaxuserChange();
	afx_msg void OnButtonUsercount();
	afx_msg void OnClickListcontrolDist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListcontrolAgent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListcontrolMap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonTerminateserver();
	afx_msg void OnButtonOption();
	afx_msg void OnButtonEvent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	// 080822 LUJ, 클립보드의 내용을 에디트 컨트롤에 복사한다
	afx_msg void OnBnClickedServerViewCopyFromClipboardButton();
	afx_msg void OnBnClickedServerSendMessageButton();
	CEdit mLInePerSecondEdit;
	CButton mMessageSendButton;
	CButton mCopyButton;
	CButton mAddScriptButton;
	afx_msg void OnBnClickedAddSecondScriptButton();
	CEdit mMessageEdit;
	afx_msg void OnBnClickedCopyShutdownScriptButton();
	CButton mCopyScriptButton;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERTOOLVIEW_H__FC61667A_7DCE_440B_A72D_800AB1ADFD7F__INCLUDED_)

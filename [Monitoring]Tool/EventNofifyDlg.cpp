// EventNofifyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "servertool.h"
#include "EventNofifyDlg.h"
#include "MainFrm.h"
#include "ServerToolView.h"
#include "ServerSetMng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventNofifyDlg dialog


CEventNofifyDlg::CEventNofifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventNofifyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventNofifyDlg)
	m_strCurFileName = _T("");
	//}}AFX_DATA_INIT

	m_bAutoNotify = FALSE;
	m_nRepeatNum = m_nRepeatTime = m_nCheckTime = 0;
}


void CEventNofifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventNofifyDlg)
	DDX_Control(pDX, IDC_BUTTON_OFF, m_btnEventMsgOff);
	DDX_Control(pDX, IDC_EDIT_EVENT_MSG, m_editMsg);
	DDX_Control(pDX, IDC_EDIT_EVENT_TITLE, m_editTitle);
	DDX_Control(pDX, IDC_EDIT_EVENT_REPEAT, m_editRepeat);
	DDX_Control(pDX, IDC_EDIT_EVENT_SEC, m_editTime);
	DDX_Control(pDX, IDC_BUTTON_EVENT_NOTIFY, m_btnNotify);
	DDX_Control(pDX, IDC_COMBO_EVENT_SERVERSET, m_cbServerSet);
	DDX_Text(pDX, IDC_STATIC_EVENT_FILENAME, m_strCurFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventNofifyDlg, CDialog)
	//{{AFX_MSG_MAP(CEventNofifyDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EVENT_NOTIFY, OnButtonEventNotify)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_OPEN, OnButtonEventOpen)
	ON_EN_CHANGE(IDC_EDIT_EVENT_TITLE, OnChangeEditEventTitle)
	ON_EN_CHANGE(IDC_EDIT_EVENT_MSG, OnChangeEditEventMsg)
	ON_BN_CLICKED(IDC_BUTTON_OFF, OnButtonOff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventNofifyDlg message handlers

BOOL CEventNofifyDlg::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void CEventNofifyDlg::PostNcDestroy() 
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	((CServerToolView*)pMainFrame->GetActiveView())->m_pEventNotifyDlg = NULL;

	delete this;
	
	CDialog::PostNcDestroy();
}

BOOL CEventNofifyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetTimer( 0, 1000, NULL );
	
	m_bAutoNotify = FALSE;

	// combobox
	m_cbServerSet.ResetContent();
	m_cbServerSet.AddString( L"All Server" );
	CServerSet* pServerSet = NULL;
	for( int i = 0; i < SERVERSETMNG->GetMaxServerSet(); ++i )
	{
		pServerSet = SERVERSETMNG->GetServerSet( i );
		if( pServerSet->m_bInit )
		{
			m_cbServerSet.AddString( pServerSet->m_sServerSetName );
		}
	}
	m_cbServerSet.SetCurSel( 0 );

	m_strCurFileName = L"None";
	m_bmpGreen.LoadBitmap( IDB_BITMAP_GREEN );
	m_editRepeat.SetWindowText( L"1" );
	m_nRepeatNum = 1;
	m_editTime.SetWindowText( L"60" );
	m_nRepeatTime = 60;	

	m_editTitle.SetLimitText( 32 );
	m_editMsg.SetLimitText( 128 );

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventNofifyDlg::OnTimer(UINT nIDEvent) 
{
	if( m_bAutoNotify )
	{
		--m_nCheckTime;
		if( m_nCheckTime <= 0 )
		{
			m_nCheckTime = m_nRepeatTime;

			if( m_NotifyMessage.IsRepeat() )
			{
				TCHAR title[32] = {0,};
				TCHAR Msg[128] = {0, };
				m_NotifyMessage.GetEventMsg( title, Msg );

				CString str;
				m_cbServerSet.GetLBText( m_cbServerSet.GetCurSel(), str );
				SERVERSETMNG->DoSendEventMessage( str, title, Msg );
					
				TCHAR temp[256];
				_stprintf( temp, L"%d", m_NotifyMessage.GetRepeatNum() );
				m_editRepeat.SetWindowText( temp );

				m_editTitle.SetWindowText( title );
				m_editMsg.SetWindowText( Msg );
			}
			else
			{
				m_bAutoNotify = FALSE;
				m_btnNotify.SetBitmap( NULL );
				m_editRepeat.EnableWindow( TRUE );
				m_editTime.EnableWindow( TRUE );
				m_btnEventMsgOff.EnableWindow( TRUE );
			}
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CEventNofifyDlg::OnButtonEventNotify() 
{
	if( m_bAutoNotify )
	{
		if( MessageBox( L"Stop To Send Event Notification Message?", L"Confirm!!", MB_OKCANCEL ) == IDOK )
		{
			m_btnNotify.SetBitmap( NULL );	

			m_editRepeat.EnableWindow( TRUE );
			m_editTime.EnableWindow( TRUE );			

			m_btnEventMsgOff.EnableWindow( TRUE );
		}
		else
			return;
	}
	else
	{
		if( m_strCurFileName == L"" || m_strCurFileName == L"None" )
		{
			MessageBox( L"First, You Have To Open Event Notification File!!", L"Error!!" );
			return;
		}
		else
		{
			if( MessageBox( L"Send Event Notification Message?", L"Confirm!!", MB_OKCANCEL ) == IDOK )
			{
				m_btnNotify.SetBitmap( m_bmpGreen );

				CString str;
				m_editRepeat.GetWindowText( str );
				m_nRepeatNum = _ttoi( str );
				m_editTime.GetWindowText( str );
				m_nRepeatTime = _ttoi( str );
				m_nCheckTime = m_nRepeatTime;
				m_editRepeat.EnableWindow( FALSE );
				m_editTime.EnableWindow( FALSE );

				m_NotifyMessage.SetRepeatNum( m_nRepeatNum );

				m_btnEventMsgOff.EnableWindow( FALSE );
			}
			else
				return;
		}
	}

	m_bAutoNotify = !m_bAutoNotify;	
}

void CEventNofifyDlg::OnButtonEventOpen() 
{
	CFileDialog dlg( TRUE, NULL, L"*.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
					 L"txt ÆÄÀÏ (*.txt)|*.txt||", NULL );

	if( dlg.DoModal() == IDOK )
	{
		m_strCurFileName = dlg.GetFileName();
		m_NotifyMessage.OpenEventMsgFile( (char*)(LPCTSTR)m_strCurFileName );

		m_editRepeat.EnableWindow( TRUE );
		m_editTime.EnableWindow( TRUE );

		UpdateData( FALSE );
	}
}

void CEventNofifyDlg::OnCancel() 
{
	KillTimer( 0 );

	DestroyWindow();	
	
//	CDialog::OnCancel();
}

void CEventNofifyDlg::OnChangeEditEventTitle() 
{
	if( m_editTitle.m_bReturn )
	{
		m_editMsg.SetFocus();

		m_editTitle.m_bReturn = FALSE;
	}	
}

void CEventNofifyDlg::OnChangeEditEventMsg() 
{
	if( m_editMsg.m_bReturn )
	{
		CString title, msg, temp, setname;
		m_editTitle.GetWindowText( title );
		m_editMsg.GetWindowText( msg );
		
		if( MessageBox( L"Send Event Notification Message?", L"Message", MB_OKCANCEL ) == IDOK )
		{
			m_cbServerSet.GetLBText( m_cbServerSet.GetCurSel(), setname );
			
			SERVERSETMNG->DoSendEventMessage( setname, title, msg );
				
			m_editTitle.SetWindowText( L"" );
			m_editMsg.SetWindowText( L"" );
		}		
		m_editMsg.m_bReturn = FALSE;

		m_editTitle.SetFocus();
	}	
}

void CEventNofifyDlg::OnButtonOff() 
{
	if( m_bAutoNotify )	return;

	if( MessageBox( L"Event Notification Message Off?", L"Confirm!!", MB_OKCANCEL ) == IDOK )
	{
		CString setname;
		m_cbServerSet.GetLBText( m_cbServerSet.GetCurSel(), setname );
			
		SERVERSETMNG->DoSendEventMessageOff( setname );
	}
}

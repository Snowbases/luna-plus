// ServerToolView.cpp : implementation of the CServerToolView class
//

#include "stdafx.h"
#include "ServerTool.h"

#include "MainFrm.h"
#include "ServerToolDoc.h"
#include "ServerToolView.h"
#include "ServerSetMng.h"
#include "MSInfoDlg.h"
#include "UserCount.h"
// 080822 LUJ, 서버를 내리기 전에 확인을 묻는 창
#include "ConfirmDialog.h"
#include "MHNetwork.h"
#include "EventNofifyDlg.h"
#include "StdioFileEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CServerToolView*	g_ServerToolView = NULL;
extern sMSGOPTION	gMsgOption;
extern BOOL			g_bSound;

/////////////////////////////////////////////////////////////////////////////
// CServerToolView

IMPLEMENT_DYNCREATE(CServerToolView, CFormView)

BEGIN_MESSAGE_MAP(CServerToolView, CFormView)
	//{{AFX_MSG_MAP(CServerToolView)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVERSET, OnSelchangeComboServerset)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_ON, OnButtonServerOn)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_OFF, OnButtonServerOff)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_COMMAND_SEND, OnButtonCommandSend)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCONTROL_DIST, OnDblclkListcontrolDist)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCONTROL_AGENT, OnDblclkListcontrolAgent)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCONTROL_MAP, OnDblclkListcontrolMap)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCONTROL_MURIM, OnDblclkListcontrolMurim)
	ON_NOTIFY(NM_RDBLCLK, IDC_LISTCONTROL_MAP, OnRdblclkListcontrolMap)
	ON_NOTIFY(NM_RDBLCLK, IDC_LISTCONTROL_DIST, OnRdblclkListcontrolDist)
	ON_NOTIFY(NM_RDBLCLK, IDC_LISTCONTROL_AGENT, OnRdblclkListcontrolAgent)
	ON_NOTIFY(NM_RDBLCLK, IDC_LISTCONTROL_MURIM, OnRdblclkListcontrolMurim)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SERVER_REFRESH, OnButtonServerRefresh)
	ON_BN_CLICKED(IDC_BUTTON_AUTOPATCH, OnButtonAutopatch)
	ON_COMMAND(ID_FILE_MAS_OPEN, OnFileMasOpen)
	ON_BN_CLICKED(IDC_BUTTON_USERLEVEL, OnButtonUserlevel)
	ON_BN_CLICKED(IDC_BUTTON_OFFMSGBOX, OnButtonOffmsgbox)
	ON_BN_CLICKED(IDC_BUTTON_ONMSGBOX, OnButtonOnmsgbox)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_USEROUT, OnButtonServerUserout)
	ON_BN_CLICKED(IDC_BUTTON_VERSION_CHANGE, OnButtonVersionChange)
	ON_BN_CLICKED(IDC_BUTTON_MAXUSER_CHANGE, OnButtonMaxuserChange)
	ON_BN_CLICKED(IDC_BUTTON_USERCOUNT, OnButtonUsercount)
	ON_NOTIFY(NM_CLICK, IDC_LISTCONTROL_DIST, OnClickListcontrolDist)
	ON_NOTIFY(NM_CLICK, IDC_LISTCONTROL_AGENT, OnClickListcontrolAgent)
	ON_NOTIFY(NM_CLICK, IDC_LISTCONTROL_MAP, OnClickListcontrolMap)
	ON_BN_CLICKED(IDC_BUTTON_TERMINATESERVER, OnButtonTerminateserver)
	ON_BN_CLICKED(IDC_BUTTON_OPTION, OnButtonOption)
	ON_BN_CLICKED(IDC_BUTTON_EVENT, OnButtonEvent)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SERVER_VIEW_COPY_FROM_CLIPBOARD_BUTTON, OnBnClickedServerViewCopyFromClipboardButton)
	ON_BN_CLICKED(IDC_SERVER_SEND_MESSAGE_BUTTON, OnBnClickedServerSendMessageButton)
	ON_BN_CLICKED(IDC_ADD_SECOND_SCRIPT_BUTTON, OnBnClickedAddSecondScriptButton)
	ON_BN_CLICKED(IDC_COPY_SHUTDOWN_SCRIPT_BUTTON, OnBnClickedCopyShutdownScriptButton)
END_MESSAGE_MAP()

// 080829 LUJ, 반복 메시지 전송위한 정적 클래스
class CMessage
{
public:
	static UINT SendRepeatedly( LPVOID )
	{
		// 080829 LUJ, 틱 당 간격마다 대기한다
		class
		{
		public:
			void operator()( DWORD waitTick ) const
			{
				const DWORD beginTick	= GetTickCount();
				const DWORD endTick		= beginTick + waitTick;

				while( waitTick )
				{
					const DWORD currentTick			= GetTickCount();
					const BOOL	isOver				= ( beginTick < endTick && endTick < currentTick );
					const BOOL	isOverflewOver		= ( beginTick > endTick && currentTick < beginTick && currentTick > endTick );

					if( isOver ||
						isOverflewOver )
					{
						break;
					}
				}
			}
		}
		Wait;

		// 080829 LUJ, 공지 관련 컨트롤 잠금/해제
		class 
		{
		public:
			void operator()( BOOL isLock ) const
			{
				if( ! g_ServerToolView )
				{
					return;
				}

				g_ServerToolView->m_RepeatEdit.EnableWindow( isLock );
				g_ServerToolView->mMessageEdit.EnableWindow( isLock );
				g_ServerToolView->m_editRepeatNum.EnableWindow( isLock );
				g_ServerToolView->mMessageSendButton.EnableWindow( isLock );
				g_ServerToolView->mCopyButton.EnableWindow( isLock );
				g_ServerToolView->mAddScriptButton.EnableWindow( isLock );
				g_ServerToolView->mCopyScriptButton.EnableWindow( isLock );
			}
		}
		UnlockControl;

		UnlockControl( FALSE );

		while( mRepeatCount	)
		{
			// 080829 LUJ, 리스트의 메시지를 보낸다
			for(
				MessageList::const_iterator it = mMessageList.begin();
				mMessageList.end() != it;
				++it )
			{
				const Message& message = *it;

#ifdef _DEBUG
				{
					// 080922 LUJ, 버퍼 오버플로를 막기 위해 버퍼 크기를 확장함
					TCHAR buffer[ MAX_PATH * 2 ]	= { 0 };
					TCHAR messageText[ MAX_PATH ]		= { 0 };
					_tcsncpy( messageText, message.mText, sizeof( messageText ) / sizeof( *messageText ) );
					_stprintf(
						buffer,
						_T( "%s(%d)\n" ),
						( message.mText.IsEmpty() ? _T( "<empty>" ) : messageText ),
						message.mWaitTick );

					OutputDebugStr( buffer );
				}
#endif

				if( ! message.mText.IsEmpty() )
				{
					char buffer[ MAX_PATH ] = { 0 };
					strncpy(
						buffer,
						CW2AEX< MAX_PATH >( message.mText ),
						sizeof( buffer ) / sizeof( *buffer ) );

					SERVERSETMNG->DoSendMessage(
						mServerSet,
						buffer );
				}

				Wait( message.mWaitTick );
			}

			CString text;
			text.Format( _T( "%d" ), mRepeatCount );
			g_ServerToolView->m_editRepeatNum.SetWindowText( text );

			if( ! --mRepeatCount )
			{
				break;
			}

#ifdef _DEBUG
			{
				TCHAR buffer[ MAX_PATH ] = { 0 };
				_stprintf(
					buffer,
					_T( "<message per tick>(%d)\n" ),
					mMessagePerTick );

				OutputDebugStr( buffer );
			}
#endif
			Wait( mMessagePerTick );
		}

		UnlockControl( TRUE );

		mMessageList.clear();
		return 0;
	}

	static void AddMessage( const TCHAR* text, DWORD waitTick )
	{
		mMessageList.push_back( Message() );

		Message& message	= mMessageList.back();
		message.mText		= text;
		message.mWaitTick	= waitTick;
	}

public:
	// 080822 LUJ, 반복 회수
	static DWORD				mRepeatCount;
	// 080822 LUJ, 틱 당 전송할 메시지 수
	static DWORD				mMessagePerTick;
	// 080822 LUJ, 전송할 서버
	static CString				mServerSet;
	// 080829 LUJ, 틱 당 전송할 라인 수
	static DWORD				mLinePerTick;

	struct Message
	{
		DWORD	mWaitTick;
		CString	mText;
	};

	typedef std::list< Message >	MessageList;
	static MessageList				mMessageList;
};

// 080822 LUJ, CMessage위한 정적 변수 선언
DWORD					CMessage::mRepeatCount;
DWORD					CMessage::mMessagePerTick;
DWORD					CMessage::mLinePerTick;
CString					CMessage::mServerSet;
CMessage::MessageList	CMessage::mMessageList;

/////////////////////////////////////////////////////////////////////////////
// CServerToolView construction/destruction

CServerToolView::CServerToolView()
	: CFormView(CServerToolView::IDD)
{
	//{{AFX_DATA_INIT(CServerToolView)
	m_strCurTime = _T("");
	m_stUserCount = _T("0");
	m_strDir = _T("");
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	g_ServerToolView = this;

	m_bReconnect = FALSE;
	m_dwDisconNum = 0;

	m_wRefreshTime = 30;
	m_wWriteCountTime = 30;

	m_dwServerOffTime = 0;

	m_nToolLevel = eOperPower_Patroller;
	m_bServerOff = FALSE;
	
	m_pUserCountDlg = NULL;
	m_pMSInfoDlg = NULL;
	m_nKill = 0;
	m_pEventNotifyDlg = NULL;

	// 080829 LUJ, 스크립트 로딩
	{
		// 090721 LUJ, CStdioFile은 유니코드 처리를 하지 못하여 변경
		CStdioFileEx file;

		if( ! file.Open( _T( "ServerTool.cfg" ), CFile::modeRead | CFile::typeText ) )
		{
			ASSERT( 0 && "file is not found" );
			return;
		}

		TCHAR line[ MAX_PATH ] = { 0 };

		enum State
		{
			StateNone,
			StateTerminateText,
		}
		state = StateNone;

		while( file.ReadString( line, sizeof( line ) / sizeof( *line ) ) )
		{
			const size_t length = _tcslen( line );

			if( ! length )
			{
				continue;
			}

			// 080829 LUJ, 중간의 주석 제거
			for(
				TCHAR* character = line;
				*character;
			character = _tcsinc( character ) )
			{
				const TCHAR* comment = _T( "//" );

				if( ! _tcsnicmp( character, comment, _tcslen( comment ) ) )
				{
					*character = 0;
					break;
				}
			}

			const TCHAR* separator	= _T( "\t\r\n" );
			const TCHAR* token		= _tcstok( line, separator );

			if( ! token )
			{
				continue;
			}
			else if( ! _tcsicmp( _T( "terminateText" ), token ) )
			{
				state = StateTerminateText;
				continue;
			}
			else if( ! _tcsicmp( _T( "{" ), token ) )
			{
				continue;
			}
			else if( ! _tcsicmp( _T( "}" ), token ) )
			{
				state = StateNone;
				continue;
			}

			switch( state )
			{
			case StateTerminateText:
				{
					mScript.mTernimateText += CString( token ) + _T( "\r\n" );
					break;
				}
			}
		}
	}
}

CServerToolView::~CServerToolView()
{
}

void CServerToolView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerToolView)
	DDX_Control(pDX, IDC_BUTTON_OPTION, m_btnMsgOption);
	DDX_Control(pDX, IDC_BUTTON_USERCOUNT, m_btnUserCount);
	DDX_Control(pDX, IDC_BUTTON_TERMINATESERVER, m_btnTServer);
	DDX_Control(pDX, IDC_BUTTON_VERSION_CHANGE, m_btnVersionChange);
	DDX_Control(pDX, IDC_BUTTON_MAXUSER_CHANGE, m_btnMaxUserChange);
	DDX_Control(pDX, IDC_EDIT_MAXUSER, m_editMaxUser);
	DDX_Control(pDX, IDC_EDIT_VERSION, m_editVersion);
	DDX_Control(pDX, IDC_BUTTON_SERVER_USEROUT, m_btnUserOut);
	DDX_Control(pDX, IDC_BUTTON_SERVER_REFRESH, m_btnServerRefresh);
	DDX_Control(pDX, IDC_BUTTON_SERVER_OFF, m_btnServerOff);
	DDX_Control(pDX, IDC_BUTTON_SERVER_ON, m_btnServerOn);
	DDX_Control(pDX, IDC_COMBO_USERLEVEL, m_cbUserLevel);
	DDX_Control(pDX, IDC_BUTTON_USERLEVEL, m_btnChangeUserLevel);
	DDX_Control(pDX, IDC_BUTTON_AUTOPATCH, m_btnAutoPatch);
	DDX_Control(pDX, IDC_EDIT_REPEAT_NUM, m_editRepeatNum);
	DDX_Control(pDX, IDC_STATIC_TOTAL, m_stTotal);
	DDX_Control(pDX, IDC_LISTCONTROL_TOTAL, m_lcTotal);
	DDX_Control(pDX, IDC_COMBO_SERVERSET, m_cbServerSet);
	DDX_Text(pDX, IDC_STATIC_CURTIME, m_strCurTime);
	DDX_Control(pDX, IDC_LISTCONTROL_DIST, m_lcDist);
	DDX_Control(pDX, IDC_LISTCONTROL_AGENT, m_lcAgent);
	DDX_Control(pDX, IDC_LISTCONTROL_MAP, m_lcMap);
	DDX_Control(pDX, IDC_STATIC_DISTRIBUTE, m_stDist);
	DDX_Control(pDX, IDC_STATIC_AGENT, m_stAgent);
	DDX_Control(pDX, IDC_STATIC_MAP, m_stMap);
	DDX_Control(pDX, IDC_COMBO_SERVERSET_COMMAND, m_cbCmdServerSet);
	DDX_Control(pDX, IDC_EDIT_REPEAT, m_RepeatEdit);
	DDX_Text(pDX, IDC_STATIC_USERCOUNT, m_stUserCount);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SERVER_SEND_MESSAGE_BUTTON, mMessageSendButton);
	DDX_Control(pDX, IDC_SERVER_VIEW_COPY_FROM_CLIPBOARD_BUTTON, mCopyButton);
	DDX_Control(pDX, IDC_ADD_SECOND_SCRIPT_BUTTON, mAddScriptButton);
	DDX_Control(pDX, IDC_MESSAGE_EDIT, mMessageEdit);
	DDX_Control(pDX, IDC_COPY_SHUTDOWN_SCRIPT_BUTTON, mCopyScriptButton);
}

BOOL CServerToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	return CFormView::PreCreateWindow(cs);
}

void CServerToolView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();	

	TCHAR temp[ MAX_PATH ] = { 0 };
	_stprintf( temp, L"ServerTool(%s)", CString( SERVERTOOLVERSION ) );
	AfxGetMainWnd()->SetWindowText( temp );
	
	m_Font.CreatePointFont(100, L"Arial");
	m_lcTotal.SetFont( &m_Font );
	m_lcDist.SetFont( &m_Font );
	m_lcAgent.SetFont( &m_Font );
	m_lcMap.SetFont( &m_Font );
		
	// timer 설정( 1초마다 )
	SetTimer( 0, 1000, NULL );
	
	// 이미지 불러오기
	m_StateImageList.Create( IDB_BITMAP_STATE, 16, 1, RGB(255,255,255) );
	m_bmpGray.LoadBitmap( IDB_BITMAP_GRAY );
	m_bmpGreen.LoadBitmap( IDB_BITMAP_GREEN );
	
	InitServerControl();
	InitServerMonitor();	
	InitCommand();

	if( !Init() )
	{
		GetParentFrame()->PostMessage( WM_CLOSE );
		return;
	}

	// Login Dlg
	if( m_LoginDlg.DoModal() == IDCANCEL && !m_LoginDlg.m_bConnect )
	{
		GetParentFrame()->PostMessage( WM_CLOSE );
	}
	else
	{
		LOG( L"!!!!! ServerTool Start !!!!!" );

		SERVERSETMNG->DoQueryClientExeVersion();
		SERVERSETMNG->DoQueryMaxUser();		
		SERVERSETMNG->DoQueryUserLevel();
		SERVERSETMNG->DoQueryUserCount();
	}

	UpdateData( FALSE );	
}

BOOL CServerToolView::Init()
{
	if( !SERVERSETMNG->Init( "MASList.ini" ) )
		return FALSE;

	// ReadINI
	ReadIniFile( "Tool.ini" );
	
	// Network Init
	if( !NETWORK->Init( this->GetParentFrame()->m_hWnd ) )
		return FALSE;

	m_cbServerSet.ResetContent();
	m_cbCmdServerSet.ResetContent();
	m_cbServerSet.AddString( L"All Server" );
	m_cbCmdServerSet.AddString( L"All Server" );

	DWORD tick = GetTickCount();
	CServerSet* pServerSet = NULL;
	int count = 0;
	int successedset = -1;
	for( int i = 0; i < SERVERSETMNG->GetMaxServerSet(); ++i )
	{
		pServerSet = SERVERSETMNG->GetServerSet( i );
		if( pServerSet )
		{
			if( !NETWORK->ConnectToServer( i, CW2AEX< MAX_PATH >( pServerSet->m_sMASIP ), pServerSet->m_nMASPort ) )
			{
				pServerSet->Release();
			}
			else
			{
				pServerSet->m_dwMsgCheckTime = tick;

				m_cbServerSet.AddString( pServerSet->m_sServerSetName );
				m_cbCmdServerSet.AddString( pServerSet->m_sServerSetName );
				if(successedset == -1)
					successedset = i;
				++count;
			}
		}
	}
	m_cbServerSet.SetCurSel( 0 );
	m_cbCmdServerSet.SetCurSel( 0 );

	if( count )
	{	
		m_LoginDlg.m_nServerSet = successedset;

		UpdateServerControl();
		OnSelchangeComboServerset();	// UpdateServerMonitor()
	}
	else
	{
		MessageBox( L"Can't Connect To MAS!!", L"Error!!", MB_OK );
		return FALSE;
	}
	
	m_LoginDlg.m_nReceiveCount = count;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CServerToolView diagnostics

#ifdef _DEBUG
void CServerToolView::AssertValid() const
{
	CFormView::AssertValid();
}

void CServerToolView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
//// 모달리스 대화상자 헤더문제로 지움....
/*
CServerToolDoc* CServerToolView::GetDocument() // non-debug version is inline
{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServerToolDoc)));
//	return (CServerToolDoc*)m_pDocument;
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CServerToolView message handlers

void CServerToolView::OnTimer(UINT nIDEvent) 
{
	SYSTEMTIME time;
	GetLocalTime( &time );
	m_strCurTime.Format( L"%d-%d-%d %d:%d:%d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );

	/*
#ifdef _CHINA_LOCAL_
	if( m_nToolLevel == eOperPower_Master )
	{
		// UserCount
		if( (time.wSecond%m_wRefreshTime) == (m_wRefreshTime-2) )
			SERVERSETMNG->DoQueryUserCount();

		if( (time.wSecond%m_wWriteCountTime) == 0 )
		{
			SERVERSETMNG->WriteUserCount( time );
			UpdateUserCount();
		}
	}
#else
	*/
	if( m_nToolLevel == eOperPower_Master || m_nToolLevel == eOperPower_Monitor )
	{
		// UserCount
		if( (time.wSecond%m_wRefreshTime) == (m_wRefreshTime-2) )
			SERVERSETMNG->DoQueryUserCount();

		if( m_nToolLevel == eOperPower_Master )
		{
			if( (time.wSecond%m_wWriteCountTime) == 0 )
			{
				SERVERSETMNG->WriteUserCount( time );
				UpdateUserCount();
			}
		}
	}

	// MAS와의 접속이 끊어졌을때 다시 접속한다.
	if( m_bReconnect )
	{
		ReConnectToMAS(); 
		LOG( L"Disconnect : Reconnect To MAS (Count = %d)", m_dwDisconNum );
	}

	// 유저 접속 끊기.
	if( m_bServerOff )
	{
		++m_dwServerOffTime;
		if( m_dwServerOffTime > 5 )
		{
			SERVERSETMNG->DoServerSetOffAfterUserDiscon( m_strAutoPatch );

			m_bServerOff = FALSE;
			m_dwServerOffTime = 0;

			LOG( L"ServerOffAfterUserDiscon : %s", m_strAutoPatch );
		}
	}

	// MsgCheckTime
//	if( (time.wMinute%2) == 0 )	// 2분마다
	if( time.wSecond == 0 )		// 1분마다
	{
		DWORD tick = GetTickCount();
		CServerSet* pServerSet = NULL;
		for( int i = 0; i < SERVERSETMNG->GetMaxServerSet(); ++i )
		{
			pServerSet = SERVERSETMNG->GetServerSet( i );
			if( pServerSet )
			if( pServerSet->m_bInit )
			if( pServerSet->IsThereOnServer() )
			{
				if( (tick - pServerSet->m_dwMsgCheckTime) >= 180000 )
				{					
					LOG( L"MsgCheckTime : Reconnect To MAS" );
					ReConnectToMAS();
					return;
				}
			}
		}
	}

#ifdef _KOREA_LOCAL_		// sms	3분마다	
	if( (time.wMinute%3) == 0 && time.wSecond == 0 )
	{
		SERVERSETMNG->CheckAndSendSMS();
	}		
#endif
	
	if( (time.wMinute%5) == 0 )
	if( time.wSecond == 0 )
	{
		SERVERSETMNG->DoMSStateAll( L"All Server" );
	}

	UpdateData( FALSE );

	if( (time.wMinute%2) == 0 )
	{
		if( g_bSound )
		{
			sndPlaySound( NULL, SND_LOOP | SND_NODEFAULT | SND_ASYNC );
			g_bSound = FALSE;
		}
	}

	CFormView::OnTimer(nIDEvent);
}

void CServerToolView::OnDestroy() 
{
	CFormView::OnDestroy();

	KillTimer( 0 );
}

// Init -------------------------------------------------------------------------------------------
void CServerToolView::InitServerControl()
{	
	m_editVersion.SetWindowText( L"Version" );
	m_editMaxUser.SetWindowText( L"MaxUser" );

	// userlevel
	CString user[7] = { L"God", L"Programmer", L"Developer", L"GM", L"SuperUser", L"User", L"SubUser" };
	for( int i = 0; i < 7; ++i )
		m_cbUserLevel.AddString( user[i] );
	m_cbUserLevel.SetCurSel( 0 );

	EnableServerControl( FALSE );
}

void CServerToolView::InitServerMonitor()
{
	LPCTSTR tcolumn[4] = { L"State", L"UserCount", L"Server Name", L"Index" };
	int tcolumnsize[4] = { 50, 70, 80, 50 };
	
	// listctrl column 설정
	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;

	int i;
	for( i = 0; i < 4; ++i )
	{
		lvcolumn.iSubItem = i;
		lvcolumn.cx = tcolumnsize[i];
		lvcolumn.pszText = LPTSTR( tcolumn[i] );
		m_lcTotal.InsertColumn( i, &lvcolumn );
	}

	//
	LPCTSTR column[7] = { L"State", L"UserCount", L"Server Name", L"MapNum", L"IP", L"Port", L"Index" };
	int columnsize[7] = { 50, 70, 80, 50, 100, 50, 30 };
	
	for( i = 0; i < 7; ++i )
	{
		lvcolumn.iSubItem = i;
		lvcolumn.cx = columnsize[i];
		lvcolumn.pszText = LPTSTR( column[i] );
		m_lcDist.InsertColumn( i, &lvcolumn );
		m_lcAgent.InsertColumn( i, &lvcolumn );
//		m_lcMurim.InsertColumn( i, &lvcolumn );
		m_lcMap.InsertColumn( i, &lvcolumn );
	}

	// style
	m_lcTotal.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	m_lcDist.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	m_lcAgent.SetExtendedStyle( LVS_EX_FULLROWSELECT );
//	m_lcMurim.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	m_lcMap.SetExtendedStyle( LVS_EX_FULLROWSELECT );

	// state image
	m_lcTotal.SetImageList( &m_StateImageList, LVSIL_STATE );
	m_lcDist.SetImageList( &m_StateImageList, LVSIL_STATE );
	m_lcAgent.SetImageList( &m_StateImageList, LVSIL_STATE );
//	m_lcMurim.SetImageList( &m_StateImageList, LVSIL_STATE );
	m_lcMap.SetImageList( &m_StateImageList, LVSIL_STATE );

	ShowServerMonitor( FALSE );	
	EnableServerMonitor( FALSE );
}

void CServerToolView::InitCommand()
{
	// 공지 button
	m_bNotify = FALSE;

	// 반복 횟수 edit
	m_editRepeatNum.SetWindowText( L"1" );
	m_nRepeatNum = 1;

	// 시간 edit
	m_RepeatEdit.SetWindowText( L"60" );
	m_nRepeatTime = m_nCheckTime = 60;	
	
	EnableCommand( FALSE );
}

void CServerToolView::EnableServerControl( BOOL bShow )
{	
	m_cbServerSet.EnableWindow( TRUE );
	m_btnServerOn.EnableWindow( bShow );
	m_btnServerOff.EnableWindow( bShow );
	m_btnServerRefresh.EnableWindow( TRUE );
	m_btnUserOut.EnableWindow( FALSE );

	m_editVersion.EnableWindow( bShow );
	m_editMaxUser.EnableWindow( bShow );
	m_cbUserLevel.EnableWindow( bShow );
	m_btnVersionChange.EnableWindow( bShow );
	m_btnMaxUserChange.EnableWindow( bShow );
	m_btnChangeUserLevel.EnableWindow( bShow );

	m_btnUserCount.EnableWindow( bShow );
	m_btnAutoPatch.EnableWindow( bShow );
}

void CServerToolView::EnableServerMonitor( BOOL bShow )
{
	m_lcTotal.EnableWindow( bShow );
	m_stDist.EnableWindow( bShow );
	m_stAgent.EnableWindow( bShow );
	m_stMap.EnableWindow( bShow );
}

void CServerToolView::EnableCommand( BOOL bShow )
{
	m_cbCmdServerSet.EnableWindow( bShow );
}

void CServerToolView::EnableToolFromLevel( eOPERPOWER nUserLevel )
{
	m_nToolLevel = nUserLevel;

	switch( m_nToolLevel )
	{
	case eOperPower_Master:
	case eOperPower_Monitor:
		{
			EnableServerControl( TRUE );
			EnableServerMonitor( TRUE );
			EnableCommand( TRUE );
		}
		break;
	case eOperPower_Patroller:
		{
			EnableServerControl( FALSE );
			EnableServerMonitor( TRUE );
			EnableCommand( FALSE );
		}
		break;
	}
}

void CServerToolView::ShowServerMonitor( BOOL bShow )
{
	// total
	m_lcTotal.ShowWindow( !bShow );
	m_stTotal.ShowWindow( !bShow );

	// distribute zone
	m_stDist.ShowWindow( bShow );
	m_lcDist.ShowWindow( bShow );

	// agent zone
	m_stAgent.ShowWindow( bShow );
	m_lcAgent.ShowWindow( bShow );

	// map zone
	m_stMap.ShowWindow( bShow );
	m_lcMap.ShowWindow( bShow );

	m_btnTServer.ShowWindow( bShow );
}

// update -----------------------------------------------------------------------------------------
void CServerToolView::UpdateServerControl()
{
	UpdateUserCount();

	m_editMaxUser.SetWindowText( L"MaxUser" );
	m_editVersion.SetWindowText( L"Version" );
	m_cbUserLevel.SetCurSel( 0 );

	if( m_cbServerSet.GetCurSel() > 0 )
	{		
		CString str;
		m_cbServerSet.GetLBText( m_cbServerSet.GetCurSel(), str );

		CServerSet* pServerSet = SERVERSETMNG->GetServerSet( (LPCTSTR)str );
		if( pServerSet )
		{
			TCHAR temp[ MAX_PATH ] = { 0 };

			switch( m_nToolLevel )
			{
			case eOperPower_Master:
				{
					_stprintf( temp, L"%d", pServerSet->m_dwMaxUser );
					// MaxUser
					m_editMaxUser.SetWindowText( temp );
					// ClientExeVersion
					m_editVersion.SetWindowText( pServerSet->m_sClientExeVersion );
					// UserLevel
					m_cbUserLevel.SetCurSel( pServerSet->m_nUserLevel-1 );
					break;
				}
			case eOperPower_Monitor:
				{
					m_editMaxUser.EnableWindow( FALSE );
					m_editVersion.EnableWindow( FALSE );
					m_cbUserLevel.EnableWindow( FALSE );
					break;
				}
			default:
				{
					m_editMaxUser.EnableWindow( FALSE );
					m_editVersion.EnableWindow( FALSE );
					m_cbUserLevel.EnableWindow( FALSE );
					m_btnServerOn.EnableWindow( FALSE );
					m_btnServerOff.EnableWindow( FALSE );
					m_btnTServer.ShowWindow( SW_HIDE );
					break;
				}
			}

			UpdateData( FALSE );
		}
	}
}

void CServerToolView::UpdateUserCount()
{
	if( m_cbServerSet.GetCurSel() > 0 )
	{
		CString str;
		m_cbServerSet.GetLBText( m_cbServerSet.GetCurSel(), str );

		CServerSet* pServerSet = SERVERSETMNG->GetServerSet( str );
		if( pServerSet )
			m_stUserCount.Format( L"%d", pServerSet->GetTotalUserCount() );
	}
	else
		m_stUserCount.Format( L"%d", SERVERSETMNG->GetTotalUserCount() );

	UpdateData( FALSE );
}

void CServerToolView::UpdateServerMonitor()
{
	CServerSet* pServerSet = NULL;
	CServer* pServer = NULL;
	LV_ITEM lvitem;
	TCHAR temp[ MAX_PATH ] = { 0 };
	int i = 0;
	int index = m_cbServerSet.GetCurSel();
	
	if( index == 0 )	// All Server
	{
		m_lcTotal.DeleteAllItems();
		
		for( i = 1; i < m_cbServerSet.GetCount(); ++i )
		{
			m_cbServerSet.GetLBText( i, temp );
			pServerSet = SERVERSETMNG->GetServerSet( temp );
			if( pServerSet )
			{
				pServer = pServerSet->GetServer( DISTRIBUTE_SERVER, 0 );
				if( pServer )
				{
					lvitem.iItem = m_lcTotal.GetItemCount();		// 행
					
					lvitem.iSubItem = 0;
					lvitem.mask = LVIF_TEXT | LVIF_STATE;
					lvitem.stateMask = LVIS_STATEIMAGEMASK;
					lvitem.state = INDEXTOSTATEIMAGEMASK( pServer->m_nState );
					lvitem.pszText = pServer->m_sState;
					
					CString text;
					text.Format( L"%d", pServerSet->GetTotalUserCount() );
					m_lcTotal.InsertItem( &lvitem );
					m_lcTotal.SetItemText( lvitem.iItem, 1, text );
					m_lcTotal.SetItemText( lvitem.iItem, 2, pServerSet->m_sServerSetName );
					
					text.Format( L"%d", pServerSet->m_nServerSet );
					m_lcTotal.SetItemText( lvitem.iItem, 3, text );
				}
			}

			m_stUserCount.Format( L"%d", SERVERSETMNG->m_dwTotalUserCount );
		}
	}
	else
	{
		// delete item
		m_lcDist.DeleteAllItems();
		m_lcAgent.DeleteAllItems();
//		m_lcMurim.DeleteAllItems();
		m_lcMap.DeleteAllItems();
		
		m_cbServerSet.GetLBText( index, temp );
		pServerSet = SERVERSETMNG->GetServerSet( temp );
		if( pServerSet == NULL ) return;
		
		// distribute
		CListCtrl* pListCtrl;
		vector<CServer*>::iterator iter = pServerSet->m_vecServer.begin();
		for(;iter != pServerSet->m_vecServer.end();++iter)
		{
			pServer = (*iter);
			switch(pServer->m_nServerKind)
			{
			case DISTRIBUTE_SERVER:	pListCtrl = &m_lcDist;	break;
			case AGENT_SERVER:	pListCtrl = &m_lcAgent;	break;
//			case MURIM_SERVER:	pListCtrl = &m_lcMurim;	break;
			case MAP_SERVER:	pListCtrl = &m_lcMap;	break;
			default:
				continue;
			}

			lvitem.iItem = pListCtrl->GetItemCount();		// 행
			
			lvitem.iSubItem = 0;
			lvitem.mask = LVIF_TEXT | LVIF_STATE;
			lvitem.stateMask = LVIS_STATEIMAGEMASK;
			lvitem.state = INDEXTOSTATEIMAGEMASK( pServer->m_nState );
			lvitem.pszText = pServer->m_sState;
			
			pListCtrl->InsertItem( &lvitem );
			_stprintf( temp, L"%d", pServer->m_dwUserCount );
			pListCtrl->SetItemText( lvitem.iItem, 1, temp );
			pListCtrl->SetItemText( lvitem.iItem, 2, pServer->m_sName );
			_stprintf( temp, L"%d", pServer->m_nServerNum );
			pListCtrl->SetItemText( lvitem.iItem, 3, temp );
			pListCtrl->SetItemText( lvitem.iItem, 4, pServer->m_sIP );
			_stprintf( temp, L"%d", pServer->m_nPort );
			pListCtrl->SetItemText( lvitem.iItem, 5, temp );
		}
	}
	UpdateData( FALSE );
}

void CServerToolView::UpdateCommand()
{
}

// Server Control ---------------------------------------------------------------------------------
void CServerToolView::OnSelchangeComboServerset() 
{
	if( m_cbServerSet.GetCurSel() > 0 )
		ShowServerMonitor( TRUE );
	else
		ShowServerMonitor( FALSE );

	UpdateServerControl();
	UpdateServerMonitor();
}

void CServerToolView::OnButtonServerOn() 
{
	if( !IsEnableTool( eOperPower_Monitor ) )	return;

	CString str, temp;
	m_cbServerSet.GetWindowText( str );

	temp.Format( L"Turn On %s Server?", str );
	if( MessageBox( temp, L"Turn On", MB_OKCANCEL ) == IDOK )
	{
		SERVERSETMNG->DoServerSetOn(str );
		m_bServerOff = FALSE;

		LOG( L"Button ServerOn : %s", str );
	}
}

void CServerToolView::OnButtonServerOff() 
{
	// 080822 LUJ, 서버를 내리기 전에 확인한다
	{
		CConfirmDialog dialog;

		const INT_PTR result = dialog.DoModal();

		if( IDOK != result )
		{
			return;
		}
		else if( ! dialog.IsConfirm() )
		{
			return;
		}
	}

	if( !IsEnableTool( eOperPower_Monitor ) )	return;

	CString str, temp;
	m_cbServerSet.GetWindowText( m_strAutoPatch );

	temp.Format( L"Turn Off %s Server?", m_strAutoPatch );
	if( MessageBox( temp, L"Turn Off", MB_OKCANCEL ) == IDOK )
//		SERVERSETMNG->DoServerSetOff( (char*)(LPCTSTR)m_strAutoPatch );
	{
		SERVERSETMNG->DoAllUserDisconnect( m_strAutoPatch );
		m_bServerOff = TRUE;

		m_dwServerOffTime = 0;

		LOG( L"Button ServerOff(AllUserDisconnect) : %s", (char*)(LPCTSTR)m_strAutoPatch );
	}
}

void CServerToolView::OnButtonServerRefresh() 
{
/*	CString str;
	m_cbServerSet.GetWindowText( str );

//	SERVERSETMNG->DoServerSetRefresh( (char*)(LPCTSTR)str );
	SERVERSETMNG->DoQueryUserCount();
*/
	ReConnectToMAS();

	LOG( L"Button Refresh : Reconnect To MAS!!" );
}

void CServerToolView::OnButtonServerUserout() 
{
/*	CString str, temp;
	m_cbServerSet.GetWindowText( str );

	temp.Format( "%s를(을) Turn Off 하시겠습니까?", str );
	if( MessageBox( temp, "Turn Off", MB_OKCANCEL ) == IDOK )
		SERVERSETMNG->DoAllUserDisconnect( (char*)(LPCTSTR)str );
*/
}

void CServerToolView::OnButtonVersionChange() 
{
	if( !IsEnableTool( eOperPower_Monitor ) )	return;

	CString str, temp, version;
	m_cbServerSet.GetWindowText( str );
	m_editVersion.GetWindowText( version );	

	temp.Format( L"Change %s's Version To %s?", str, version );
	if( MessageBox( temp, L"Version Change", MB_OKCANCEL ) == IDOK )
	{		
		SERVERSETMNG->DoChangeClientExeVersion( str, version );

		LOG( L"Button VersionChange : %s", version );
	}
}

void CServerToolView::OnButtonMaxuserChange() 
{	
	if( !IsEnableTool( eOperPower_Monitor ) )	return;

	CString str, temp, maxuser;
	m_cbServerSet.GetWindowText( str );
	m_editMaxUser.GetWindowText( maxuser );
	DWORD user = _ttoi( maxuser );

	temp.Format( L"Change %s's MaxUser To %s?", str, maxuser );
	if( MessageBox( temp, L"MaxUser Change", MB_OKCANCEL ) == IDOK )
	{		
		SERVERSETMNG->DoChangeMaxUser( str, user );

		LOG( L"Button MaxUserChange : %d", user );
	}
}

void CServerToolView::OnButtonUserlevel() 
{
	CString str;
	m_cbServerSet.GetWindowText( str );

	CString user[7] = { L"God", L"Programmer", L"Developer", L"GM", L"SuperUser", L"User", L"SubUser" };
	int level = m_cbUserLevel.GetCurSel();
	
	TCHAR temp[ MAX_PATH ] = { 0 };
	_stprintf( temp, L"Change %s's UserLevel To %s?", str, user[level] );
	if( MessageBox( temp, L"UserLevel", MB_OKCANCEL ) == IDOK )
	{
		SERVERSETMNG->DoChangeUserLevel( str, level+1 );

		LOG( L"Button UserLevelChange : %d", level+1 );
	}
}

void CServerToolView::OnButtonAutopatch() 
{
	if( !IsEnableTool( eOperPower_Monitor ) )	return;

	if( m_pMSInfoDlg )
		m_pMSInfoDlg->SetFocus();
	else
	{
		m_pMSInfoDlg = new CMSInfoDlg;
		m_pMSInfoDlg->Create( (CMainFrame*)AfxGetMainWnd() );
		m_pMSInfoDlg->ShowWindow( SW_SHOW );
	}	
}


void CServerToolView::OnButtonUsercount() 
{
	if( !IsEnableTool( eOperPower_Master ) )	return;

	if( m_pUserCountDlg )
		m_pUserCountDlg->SetFocus();
	else
	{
		m_pUserCountDlg = new CUserCount;
		m_pUserCountDlg->Create( (CMainFrame*)AfxGetMainWnd() );
		m_pUserCountDlg->ShowWindow( SW_SHOW );
	}	
}

// 필요없을지도...
void CServerToolView::OnButtonOffmsgbox() 
{
	CString str, temp;
	m_cbServerSet.GetWindowText( str );

	temp.Format( L"Turn Off %s's AssertMsgBox?", str );
	if( MessageBox( temp, L"Assert MsgBox Off", MB_OKCANCEL ) == IDOK )
	{
		SERVERSETMNG->DoAssertMsgBox( str, FALSE );
	}
}

void CServerToolView::OnButtonOnmsgbox() 
{
	CString str, temp;
	m_cbServerSet.GetWindowText( str );

	temp.Format( L"Turn On %s's AssertMsgBox?", str );
	if( MessageBox( temp, L"Assert MsgBox On", MB_OKCANCEL ) == IDOK )
	{
		SERVERSETMNG->DoAssertMsgBox( str, TRUE );
	}
}

void CServerToolView::ReConnectToMAS() 
{
	m_bReconnect = FALSE;
	
	NETWORK->Release();

	if( SERVERSETMNG->Init( "MASList.ini" ) )
	{	
		NETWORK->Init(this->GetParentFrame()->m_hWnd );
//------------------------------------------------------------------------------------
		m_cbServerSet.ResetContent();
		m_cbCmdServerSet.ResetContent();
		m_cbServerSet.AddString( L"All Server" );
		m_cbCmdServerSet.AddString( L"All Server" );

		DWORD tick = GetTickCount();
		CServerSet* pServerSet = NULL;
		int count = 0;
		for( int i = 0; i < SERVERSETMNG->GetMaxServerSet(); ++i )
		{
			pServerSet = SERVERSETMNG->GetServerSet( i );
			if( pServerSet )
			{
				if( !NETWORK->ConnectToServer( i, CW2AEX< MAX_PATH >( pServerSet->m_sMASIP ), pServerSet->m_nMASPort ) )
				{
					pServerSet->Release();
				}
				else
				{
					pServerSet->m_dwMsgCheckTime = tick;

					m_cbServerSet.AddString( pServerSet->m_sServerSetName );
					m_cbCmdServerSet.AddString( pServerSet->m_sServerSetName );

					++count;
				}
			}
		}

		m_cbServerSet.SetCurSel( 0 );
		m_cbCmdServerSet.SetCurSel( 0 );
//------------------------------------------------------------------------------------

		if( count )
		{				
			EnableServerControl( FALSE );
			EnableServerMonitor( FALSE );
			EnableCommand( FALSE );
			EnableToolFromLevel( m_nToolLevel );

			UpdateServerControl();
			OnSelchangeComboServerset();	// UpdateServerMonitor()

			SERVERSETMNG->DoQueryClientExeVersion();
			SERVERSETMNG->DoQueryMaxUser();		
			SERVERSETMNG->DoQueryUserLevel();
			SERVERSETMNG->DoQueryUserCount();
			SERVERSETMNG->DoServerSetRefresh( L"All Server" );
		}
		else
		{
			MessageBox( L"Can't Connect To MAS!!", L"Error!!", MB_OK );
			GetParentFrame()->PostMessage( WM_CLOSE );
		}
	}

	UpdateData( FALSE );
}

void CServerToolView::OnButtonCommandSend() 
{
}

// Server Monitor ---------------------------------------------------------------------------------
void CServerToolView::OnDblClickServerList(CListCtrl* pListCtrl,WORD ServerKind)
{
	// 090511 LUJ, 특정 권한만 허용된다
	switch( m_nToolLevel )
	{
	case eOperPower_Master:
	case eOperPower_Monitor:
		{
			break;
		}
	default:
		{
			return;
		}
	}

	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();

	if( pos )
	{
		int nItem = pListCtrl->GetNextSelectedItem( pos );
		TCHAR servernum[ MAX_PATH ] = { 0 };
		pListCtrl->GetItemText( nItem, 3, servernum, 256 );

		CString str, temp;
		m_cbServerSet.GetWindowText( str );		// serverset name

		CServerSet* pServerSet = SERVERSETMNG->GetServerSet( str );
		if( pServerSet )
		{
			CServer* pServer = pServerSet->GetServer( ServerKind, _ttoi( servernum ) );
			if( pServer )
			{
				switch( pServer->m_nState )
				{
				case eServerState_On:
					{
						temp.Format( L"Turn Off %s's %sServer?", str, pServer->m_sName );
						if( MessageBox( temp, L"Turn Off", MB_OKCANCEL ) == IDOK )
							pServer->DoServerOff();
					}
					break;

				case eServerState_Off:
					{
						temp.Format( L"Turn On %s's %sServer?", str, pServer->m_sName );
						if( MessageBox( temp, L"Turn On", MB_OKCANCEL ) == IDOK )
							pServer->DoServerOn();
					}
					break;

				case eServerState_AbNormal:
					{
						temp.Format( L"Turn Off %s's %sServer?", str, pServer->m_sName );
						if( MessageBox( temp, L"Turn Off", MB_OKCANCEL ) == IDOK )
							pServer->DoKillServer();
//							pServer->DoServerOff();
					}
					break;
				}
			}
		}
	}
}
void CServerToolView::OnDblclkListcontrolDist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDblClickServerList(&m_lcDist,DISTRIBUTE_SERVER);

	*pResult = 0;
}

void CServerToolView::OnDblclkListcontrolAgent(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDblClickServerList(&m_lcAgent,AGENT_SERVER);

	*pResult = 0;
}

void CServerToolView::OnDblclkListcontrolMurim(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	OnDblClickServerList(&m_lcMurim,MURIM_SERVER);
	
	*pResult = 0;
}

void CServerToolView::OnDblclkListcontrolMap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDblClickServerList(&m_lcMap,MAP_SERVER);

	*pResult = 0;
}

void CServerToolView::OnRdblClickServerList(CListCtrl* pListCtrl,WORD ServerKind)
{	
	int count = pListCtrl->GetSelectedCount();
	int nItem = -1;
	TCHAR servernum[ MAX_PATH ] = { 0 };

	if( count > 1 )
	{
		if( MessageBox( L"Change Server's State Which Is Selected?", L"On/Off", MB_OKCANCEL ) == IDOK )
		{
			CString str;
			m_cbServerSet.GetWindowText( str );
			CServerSet* pServerSet = SERVERSETMNG->GetServerSet( str );
			if( pServerSet )
			{
				for( int i = 0; i < count; ++i )
				{
					nItem = pListCtrl->GetNextItem( nItem, LVNI_SELECTED );
					pListCtrl->GetItemText( nItem, 3, servernum, 256 );

					CServer* pServer = pServerSet->GetServer( ServerKind, _ttoi( servernum ) );
					if( pServer->m_nState == eServerState_Off )
						pServer->DoServerOn();
					else if( pServer->m_nState == eServerState_On )	// On
						pServer->DoServerOff();
					else if( pServer->m_nState == eServerState_AbNormal )
						pServer->DoKillServer();
				}
			}
		}
	}
}

void CServerToolView::OnRdblclkListcontrolDist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnRdblClickServerList(&m_lcDist,DISTRIBUTE_SERVER);

	*pResult = 0;
}

void CServerToolView::OnRdblclkListcontrolAgent(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnRdblClickServerList(&m_lcAgent,AGENT_SERVER);
	
	*pResult = 0;
}

void CServerToolView::OnRdblclkListcontrolMurim(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	OnRdblClickServerList(&m_lcMurim,MURIM_SERVER);
	
	*pResult = 0;
}

void CServerToolView::OnRdblclkListcontrolMap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnRdblClickServerList(&m_lcMap,MAP_SERVER);
	
	*pResult = 0;
}

// network ----------------------------------------------------------------------------------------
void CServerToolView::NetworkMsgParse( int ServerSet, BYTE Category, BYTE Protocol, void* pMsg )
{
	switch( Category ) 
	{
	case MP_RMTOOL_CONNECT:
		{
//#ifndef _KOREA_LOCAL_
			--m_LoginDlg.m_nReceiveCount;
//#endif
			switch( Protocol )
			{
			case MP_RMTOOL_CONNECT_ACK:
				{
					TMSG_OPERATOR_LOGIN* msg = (TMSG_OPERATOR_LOGIN*)pMsg;

					if( m_LoginDlg.m_nReceiveCount > 0 )
						break;

					m_LoginDlg.m_bConnect = TRUE;
					m_LoginDlg.EndDialog(IDOK);

					EnableToolFromLevel( msg->sOper.mPower );
					SERVERSETMNG->DoServerSetRefresh( L"All Server" );
				}
				break;

			case MP_RMTOOL_CONNECT_NACK:
				{
					TMSG_WORD* msg = (TMSG_WORD*)pMsg;
					// 090626 ShinJS --- 서버의 에러코드를 사용하도록 수정
					m_LoginDlg.LoginError( msg->Code );
					return;
				}
				break;
				
			case MP_CHECKIP_ERROR:
				{
					if( MessageBox( L"Tool Is Off For A Wrong IP Address", L"Error!!", MB_OK ) == IDOK )
					{
						GetParentFrame()->PostMessage( WM_CLOSE );
						return;
					}
				}
				break;
			}
		}
		break;

	case MP_MORNITORTOOL:
		{
#ifdef _CHINA_LOCAL_
			if( m_nToolLevel == eOperPower_Master || m_nToolLevel == eOperPower_Monitor )
			{
			}
			else
				return;
#elif _JAPAN_LOCAL_
			if( m_nToolLevel == eOperPower_Master || m_nToolLevel == eOperPower_Monitor )
			{
			}
			else
				return;
#endif
			switch( Protocol )
			{
			case MP_MORNITORTOOL_MAPSERVERSTATE_ACK:		// all state
				{
					MSGMONITORTOOLALLSTATE* pmsg = (MSGMONITORTOOLALLSTATE*)pMsg;
					SERVERSTATE* pState = NULL;

					for( int i = 0; i < pmsg->num; ++i )
					{
						pState = &pmsg->serverState[i];
						SetServerState( ServerSet, pState );
					}					
				}
				break;

			case MP_MORNITORTOOL_MAPSERVERSTATE_NOTIFY:		// one state
				{
					MSGSERVERSTATE* pmsg = (MSGSERVERSTATE*)pMsg;
					SetServerState( ServerSet, &pmsg->serverState );
				}
				break;			

			case MP_MORNITORTOOL_DISCONNECT_ALLUSER_ACK:
				{
					MSGSERVERSTATE* pmsg = (MSGSERVERSTATE*)pMsg;
					NetworkAllUserDisconnect( ServerSet, &pmsg->serverState );
				}
				break;
				
			case MP_MORNITORTOOL_QUERY_VERSION_ACK:
				{
					MSGNOTICE* pmsg = (MSGNOTICE*)pMsg;
					NetworkQueryVersion( ServerSet, CA2WEX< sizeof( pmsg->Msg ) >( pmsg->Msg ) );
					UpdateServerControl();
				}
				break;
			case MP_MORNITORTOOL_CHANGE_VERSION_ACK:
				{
					MSGNOTICE* pmsg = (MSGNOTICE*)pMsg;
					NetworkChangeVersion( ServerSet, CA2WEX< sizeof( pmsg->Msg ) >( pmsg->Msg ) );
					UpdateServerControl();
				}
				break;
			case MP_MORNITORTOOL_QUERY_MAXUSER_ACK:
				{
					MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
					NetworkQueryMaxUser( ServerSet, pmsg->dwData );					
					UpdateServerControl();
				}
				break;
			case MP_MORNITORTOOL_CHANGE_MAXUSER_ACK:
				{
					MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
					NetworkChangeMaxUser( ServerSet, pmsg->dwData );					
					UpdateServerControl();
				}
				break;
			case MP_MORNITORTOOL_CHANGE_USERLEVEL_ACK:
				{
					MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;	
					NetworkUserLevel( ServerSet, pmsg->dwData );
					UpdateServerControl();
				}
				break;
				
			case MP_MORNITORTOOL_MSSTATEALL_ACK:
				{
					MSGMSSTATEALL* pmsg = (MSGMSSTATEALL*)pMsg;

					if( m_pMSInfoDlg )
//					if( m_pMSInfoDlg->m_bInit )
					{						
						m_pMSInfoDlg->SetMSStateAll( ServerSet, pmsg );
					}
					else
						SetMSStateAll( ServerSet, pmsg );

				}
				break;

			case MP_MORNITORTOOL_CHECK_AUTOPATCH_ACK:
				{
					if( m_pMSInfoDlg )
					if( m_pMSInfoDlg->m_bInit )
					{
						MSGMSSTATE* pmsg = (MSGMSSTATE*)pMsg;
						m_pMSInfoDlg->SetAutoPatchState( ServerSet, &pmsg->MSState );		
					}
				}
				break;

			case MP_MORNITORTOOL_QUERY_AUTOPATCHVERSION_ACK:
				{
					if( m_pMSInfoDlg )
					if( m_pMSInfoDlg->m_bInit )
					{
						MSGMSSTATE* pmsg = (MSGMSSTATE*)pMsg;
						m_pMSInfoDlg->SetAutoPatchVersion( ServerSet, &pmsg->MSState );		
					}
				}
				break;
				
			case MP_MORNITORTOOL_QUERY_CHANNELINFO_ACK:
				{
					if( m_pUserCountDlg )
					if( m_pUserCountDlg->m_bInit )
					{
						MSG_CHANNEL_INFO_MORNITOR* pmsg = (MSG_CHANNEL_INFO_MORNITOR*)pMsg;
						m_pUserCountDlg->SetChannelData( ServerSet, pmsg );
					}
				}
				break;
				// 090511 LUJ, 요청이 권한에 맞지 않아 실패함
			case MP_MORNITORTOOL_NO_AUTHORITY_ACK:
				{
					const MSG_DWORD* message = ( MSG_DWORD* )pMsg;

					TCHAR text[ MAX_PATH ] = { 0 };
					_stprintf(
						text,
						_T( "%s(protocol: %d)\n" ),
						_T( "You have no authority to do it" ),
						message->dwData );
					OutputDebugString( text );
				}
				break;
			}

//			UpdateServerControl();
			UpdateServerMonitor();			
		}
		break;
	}
}

//-------------------------------------------------------------------------------------------------
void CServerToolView::SetServerState( int setindex, SERVERSTATE* pstate )
{
	int flag = -1;
	// 서버 정보
	CServerSet* pServerSet = SERVERSETMNG->GetServerSet( setindex );
	if( pServerSet )
	{
		CServer* pServer = pServerSet->GetServer( pstate );
		if( pServer )
		{
#ifdef _CHINA_LOCAL_
			if( m_nToolLevel == eOperPower_Master )
				pServer->m_dwUserCount = pstate->UserCount;
			else
				pServer->m_dwUserCount = 0;
#elif _JAPAN_LOCAL_
			if( m_nToolLevel == eOperPower_Master )
				pServer->m_dwUserCount = pstate->UserCount;
			else
				pServer->m_dwUserCount = 0;
#else
			pServer->m_dwUserCount = pstate->UserCount;		
#endif
			flag = pServer->SetState( pstate->ServerState );			
		}

		pServerSet->m_dwMsgCheckTime = GetTickCount();
	}
	if( flag != -1 )
	{
		m_cbServerSet.SetCurSel( flag + 1);
		OnSelchangeComboServerset();
	}
}

void CServerToolView::NetworkQueryVersion( int setindex, LPCTSTR version )
{
	CServerSet* pServerSet = SERVERSETMNG->GetServerSet( setindex );

	if( pServerSet )
	{
		if( _tcscmp( pServerSet->m_sClientExeVersion, version ) != 0 )
		{
			_tcscpy( pServerSet->m_sClientExeVersion, version );
			pServerSet->WriteClientExeVersion( CW2AEX< MAX_PATH >( version ) );

			TCHAR temp[ MAX_PATH ] = { 0 };
			_stprintf( temp, L"%s's Version Is %s.", pServerSet->m_sServerSetName, pServerSet->m_sClientExeVersion );
			if( gMsgOption.bExeVer )
				MessageBox( temp, L"Version", MB_OK );
		}

		UpdateData( FALSE );

		pServerSet->m_dwMsgCheckTime = GetTickCount();
	}
}

void CServerToolView::NetworkChangeVersion( int setindex, LPCTSTR version )
{
	CServerSet* pServerSet = SERVERSETMNG->GetServerSet( setindex );
	if( pServerSet )
	{
		_tcscpy( pServerSet->m_sClientExeVersion, version );
		pServerSet->WriteClientExeVersion( CW2AEX< MAX_PATH >( version ) );

		TCHAR temp[ MAX_PATH ] = { 0 };
		wsprintf( temp, L"%s's Version Is %s.", pServerSet->m_sServerSetName, pServerSet->m_sClientExeVersion );
		if( gMsgOption.bExeVer )
			MessageBox( temp, L"Version", MB_OK );

		UpdateData( FALSE );

		pServerSet->m_dwMsgCheckTime = GetTickCount();
	}
}

void CServerToolView::NetworkQueryMaxUser( int setindex, DWORD maxuser )
{
	CServerSet* pServerSet = SERVERSETMNG->GetServerSet( setindex );

	if( pServerSet )
	{
		if( pServerSet->m_dwMaxUser != maxuser )
		{
			pServerSet->m_dwMaxUser = maxuser;

			TCHAR temp[ MAX_PATH ] = { 0 };
			_stprintf( temp, L"%s's MaxUser Is %d.", pServerSet->m_sServerSetName, pServerSet->m_dwMaxUser );
			if( gMsgOption.bMaxUser )
				MessageBox( temp, L"MaxUser", MB_OK );
		
			UpdateData( FALSE );
		}

		pServerSet->m_dwMsgCheckTime = GetTickCount();
	}
}

void CServerToolView::NetworkChangeMaxUser( int setindex, DWORD maxuser )
{
	CServerSet* pServerSet = SERVERSETMNG->GetServerSet( setindex );
	if( pServerSet )
	{
		pServerSet->m_dwMaxUser = maxuser;

		TCHAR temp[ MAX_PATH ] = { 0 };
		_stprintf( temp, L"%s's MaxUser Is %d.", pServerSet->m_sServerSetName, pServerSet->m_dwMaxUser );
		if( gMsgOption.bMaxUser )
			MessageBox( temp, L"MaxUser", MB_OK );

		UpdateData( FALSE );

		pServerSet->m_dwMsgCheckTime = GetTickCount();
	}
}

void CServerToolView::NetworkUserLevel( int setindex, DWORD userlevel )
{
	CServerSet* pServerSet = SERVERSETMNG->GetServerSet( setindex );
	if( pServerSet )
	{
		if( pServerSet->m_nUserLevel != (int)userlevel )
		{
			pServerSet->SetUserLevel( userlevel );

			TCHAR temp[ MAX_PATH ] = { 0 };
			_stprintf( temp, L"%s's UserLevel Is %s.", pServerSet->m_sServerSetName, pServerSet->m_sUserLevel );
			if( gMsgOption.bUserLevel )
				MessageBox( temp, L"UserLevel", MB_OK );
		}
		if( userlevel != 0 )
			m_cbUserLevel.SetCurSel( userlevel-1 );

		UpdateData( FALSE );

		pServerSet->m_dwMsgCheckTime = GetTickCount();
	}
}

void CServerToolView::NetworkAllUserDisconnect( int setindex, SERVERSTATE* pstate )
{
	// 서버 정보
	CServerSet* pServerSet = SERVERSETMNG->GetServerSet( setindex );
	if( pServerSet )
	{
		CServer* pServer = pServerSet->GetServer( pstate );
		if( pServer )
			pServer->m_bDisconnectUser = FALSE;
	}
}

void CServerToolView::Disconnect( int serverset )
{
	m_bReconnect = TRUE;
	++m_dwDisconNum;
}

// menu -------------------------------------------------------------------------------------------
void CServerToolView::OnFileMasOpen() 
{
//	ReConnectToMAS();
}

void CServerToolView::ReadIniFile( char* filename )
{
	m_wRefreshTime = 30;
	m_wWriteCountTime = 30;

	CMHFile file;
	char temp[ MAX_PATH ] = { 0 };
	strcpy( temp, CW2AEX< MAX_PATH >( GetDataDirectory() ) );
	strcat( temp, filename );

	if( !file.Init( temp, "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
		return;

	while( !file.IsEOF() )
	{
		strcpy( temp, strupr(file.GetString()) );

		if( temp[0] == '/' ) 
		{
			file.GetLine( temp, 256 );
			continue;
		}
		
		if( strcmp( temp, "*REFRESHTIME" ) == 0 )
		{
			m_wRefreshTime = file.GetWord();
		}
		else if( strcmp( temp, "*WRITETIME" ) == 0 )
		{
			m_wWriteCountTime = file.GetWord();
		}
	}
	file.Release();
}

void CServerToolView::OnClickListcontrolDist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnClickServerList(&m_lcDist,DISTRIBUTE_SERVER);

	*pResult = 0;
}

void CServerToolView::OnClickListcontrolAgent(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnClickServerList(&m_lcAgent,AGENT_SERVER);
	
	*pResult = 0;
}

void CServerToolView::OnClickListcontrolMap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnClickServerList(&m_lcMap,MAP_SERVER);
	
	*pResult = 0;
}

void CServerToolView::OnClickServerList(CListCtrl* pListCtrl,WORD ServerKind)
{
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();

	if( pos )
	{
		m_nKill = ServerKind;
		m_btnTServer.EnableWindow( TRUE );
	}
	else
	{
		m_nKill = 0;
		m_btnTServer.EnableWindow( FALSE );
	}
}

void CServerToolView::OnButtonTerminateserver() 
{
	if( m_nKill == 0 )
		return;

	CListCtrl* pListCtrl = NULL;
	POSITION pos = NULL;
	int nServerKind = 0;
	switch( m_nKill )
	{
	case DISTRIBUTE_SERVER:
		{
			pos = m_lcDist.GetFirstSelectedItemPosition();
			pListCtrl = &m_lcDist;
			nServerKind = DISTRIBUTE_SERVER;
		}
		break;
	case AGENT_SERVER:
		{
			pos = m_lcAgent.GetFirstSelectedItemPosition();
			pListCtrl = &m_lcAgent;
			nServerKind = AGENT_SERVER;
		}
		break;
	case MAP_SERVER:
		{
			pos = m_lcMap.GetFirstSelectedItemPosition();
			pListCtrl = &m_lcMap;
			nServerKind = MAP_SERVER;
		}
		break;
	}

	if( pos )
	{
		int nItem = pListCtrl->GetNextSelectedItem( pos );
		TCHAR servernum[ MAX_PATH ] = { 0 };
		pListCtrl->GetItemText( nItem, 3, servernum, 256 );

		CString str, temp;
		m_cbServerSet.GetWindowText( str );		// serverset name

		CServerSet* pServerSet = SERVERSETMNG->GetServerSet( str );
		if( pServerSet )
		{
			CServer* pServer = pServerSet->GetServer( nServerKind, _ttoi( servernum ) );
			if( pServer )
			{
				temp.Format( L"Terminate %s's %sServer?", str, pServer->m_sName );
				if( MessageBox( temp, L"Turn Off", MB_OKCANCEL ) == IDOK )
					pServer->DoKillServer();
			}
		}
	}
}

void CServerToolView::OnButtonOption() 
{
	m_MsgBoxOption.DoModal();		
}

BOOL CServerToolView::IsEnableTool( int nToolLevel )
{
	switch( nToolLevel )
	{
	case eOperPower_Master:
		{
			if( m_nToolLevel == eOperPower_Master )
				return TRUE;
		}
		break;
	case eOperPower_Monitor:
		{
			if( m_nToolLevel == eOperPower_Master || m_nToolLevel == eOperPower_Monitor )
				return TRUE;
		}
		break;
	}

	return FALSE;
}

void CServerToolView::SetMSStateAll( int nServerSet, MSGMSSTATEALL* pState )
{
	CServerSet* pServerSet = SERVERSETMNG->GetServerSet( nServerSet );
	if( pServerSet )
	{
		pServerSet->ClearMSStateAll( eServerState_Disconnect );

		for( int i = 0; i < pState->nNum; ++i )
		{
			CServer* pServer = pServerSet->GetMSServer( &pState->MSState[i] );
			if( pServer )
			{
				pServer->SetState( eServerState_Connect );
			}
		}
		pServerSet->CheckMSState();
	}
}

void CServerToolView::OnButtonEvent() 
{
	if( !IsEnableTool( eOperPower_Monitor ) )	return;

	if( m_pEventNotifyDlg )
		m_pEventNotifyDlg->SetFocus();
	else
	{
		m_pEventNotifyDlg = new CEventNofifyDlg;
		m_pEventNotifyDlg->Create( (CMainFrame*)AfxGetMainWnd() );
		m_pEventNotifyDlg->ShowWindow( SW_SHOW );
	}	
}

// 080822 LUJ, 클립보드의 내용을 에디트 컨트롤에 복사한다
// 090721 LUJ, 유니코드 문자열을 처리할 수 있도록 수정
void CServerToolView::OnBnClickedServerViewCopyFromClipboardButton()
{
	if( ! IsClipboardFormatAvailable( CF_UNICODETEXT ) )
	{
		return;
	}
	else if( ! OpenClipboard() )
	{
		return;
	}

	HGLOBAL handleGlobal = GetClipboardData( CF_UNICODETEXT );

	if( ! handleGlobal )
	{
		return;
	}

	const LPTSTR textPointer = ( LPTSTR )GlobalLock( handleGlobal );

	if( ! textPointer )
	{
		return;
	}

	mMessageEdit.SetWindowText( textPointer );
	GlobalUnlock( handleGlobal );

	CloseClipboard();

	mMessageEdit.SetFocus();
}

void CServerToolView::OnBnClickedServerSendMessageButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 080829 LUJ, 전송할 대상을 얻어온다
	m_cbCmdServerSet.GetLBText(
		m_cbCmdServerSet.GetCurSel(),
		CMessage::mServerSet );

	// 080829 LUJ, 에디트 박스에 입력된 문자열을 줄 단위로 분리하여 메시지 목록에 담는다
	{
		CString text;
		mMessageEdit.GetWindowText( text );
		mMessageEdit.SetWindowText( _T( "" ) );

		int		start		= 0;
		CString token;
		// 081002 LUJ, 확인을 위해 임시로 전송할 문자열을 저장해두는 변수
		CString	sendText;
		
		do
		{
			token = text.Tokenize( _T( "\n\r" ), start );

			BOOL isScript = FALSE;

			// 080829 LUJ, 스크립트로 정의된 초 정보를 추출한다
			{
				TCHAR line[ MAX_PATH ] = { 0 };
				_tcsncpy( line, token, sizeof( line ) / sizeof( *line ) );

				for(
					const TCHAR* character	= line;
					*character;
					character = _tcsinc( character ) )
				{
					const TCHAR* keyword		= _T( "<second=" );
					const TCHAR* endScript		= _T( ">" );

					if( ! _tcsnicmp( keyword, character, _tcslen( keyword ) ) )
					{
						const CString buffer( character );

						int i = 0;
						const CString number( buffer.Tokenize( keyword, i ) );
						
						const DWORD waitTick = _ttoi( number );

						CMessage::AddMessage( _T( "" ), waitTick * 1000 );

						isScript = TRUE;
					}
				}
			}

			if( isScript )
			{
				continue;
			}

			TCHAR buffer[ MAX_PATH ] = { 0 };
			_tcsncpy( buffer, token, sizeof( buffer ) / sizeof( *buffer ) );

			CMessage::AddMessage( buffer, 0 );

			sendText += CString( buffer ) + _T( "\n" );
		}
		while( ! token.IsEmpty() );

		// 081002 LUJ, 메시지를 보낼지 확인한다
		if( IDCANCEL == MessageBox(
							sendText + _T( "Do you send it really? If it is too long, it'll not\n" ),
							_T( "help" ),
							MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2 ) )
		{
			return;
		}
	}

	// 080829 LUJ, 반복 회수 세팅
	{
		CString text;
		m_editRepeatNum.GetWindowText( text );

		CMessage::mRepeatCount = _ttoi( text );
	}

	// 080829 LUJ, 반복 간격 세팅
	{
		CString text;
		m_RepeatEdit.GetWindowText( text );

		CMessage::mMessagePerTick = _ttoi( text ) * 1000;
	}

	AfxBeginThread(
		CMessage::SendRepeatedly,
		0 );
}

void CServerToolView::OnBnClickedAddSecondScriptButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	TCHAR		line[ MAX_PATH ]	= { 0 };
	const DWORD lineSize			= sizeof( line ) / sizeof( *line );
	mMessageEdit.GetWindowText( line, lineSize );

	DWORD cursorPosition = min( lineSize, HIWORD( mMessageEdit.GetSel() ) );

	TCHAR text[ MAX_PATH ] = { 0 };

	_tcsncpy( text, line, min( lineSize, cursorPosition ) );

	const TCHAR* lineCharacter = line;
	// 080829 LUJ, 선택된 셀 뒤를 따로 복사하기 위해 포인터를 전진시킨다
	while( cursorPosition-- )
	{
		lineCharacter = _tcsinc( lineCharacter );
	}

	const TCHAR* script		= _T( "<second=1>" );
	const TCHAR* lineFeed	= _T( "\r\n" );

	_tcscat( text, CString( lineFeed ) + script + lineFeed + lineCharacter );

	mMessageEdit.SetWindowText( text );
	mMessageEdit.SetFocus();
	const int size = int( _tcslen( text ) );
	mMessageEdit.SetSel( size, size );
}

void CServerToolView::OnBnClickedCopyShutdownScriptButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	mMessageEdit.SetWindowText( mScript.mTernimateText );
}

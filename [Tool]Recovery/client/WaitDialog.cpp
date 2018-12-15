// WaitDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "WaitDialog.h"
#include "PictureEx.h"
#include ".\waitdialog.h"


const int timerId = WM_USER + 12345;


// CWaitDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWaitDialog, CDialog)
CWaitDialog::CWaitDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDialog::IDD, pParent)
	, mMaxSecond( 30 )
	, mIsWaiting( false )
{
	EnableAutomation();

}

CWaitDialog::~CWaitDialog()
{
}

void CWaitDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	KillTimer( timerId );

	CDialog::OnFinalRelease();
}

void CWaitDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WAIT_GIF, m_Picture);
	DDX_Control(pDX, IDC_WAITING_TIME_STATIC, mTimeStatic);
	DDX_Control(pDX, IDCANCEL, mCancelButton);}


BEGIN_MESSAGE_MAP(CWaitDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CWaitDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IWaitDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {367CA3B6-B67D-45F7-A925-2ED77EA80882}
static const IID IID_IWaitDialog =
{ 0x367CA3B6, 0xB67D, 0x45F7, { 0xA9, 0x25, 0x2E, 0xD7, 0x7E, 0xA8, 0x8, 0x82 } };

BEGIN_INTERFACE_MAP(CWaitDialog, CDialog)
	INTERFACE_PART(CWaitDialog, IID_IWaitDialog, Dispatch)
END_INTERFACE_MAP()


// CWaitDialog 메시지 처리기입니다.


BOOL CWaitDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if (m_Picture.Load( MAKEINTRESOURCE( IDR_WAIT_LOADING_GIF ), _T( "GIF" ) ) )
	{
		m_Picture.Draw();
	}

	// 타이머
	{
		SetTimer( timerId, 1000, 0 );
	}

	mSecond = 0;

	mCancelButton.ShowWindow( SW_HIDE );

	mIsWaiting = true;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CWaitDialog::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if( nIDEvent == timerId )
	{
		if( mMaxSecond == ++mSecond )
		{
			mCancelButton.ShowWindow( SW_SHOW );

			KillTimer( timerId );

			mTimeStatic.SetWindowText( _T( "" ) );

			mIsWaiting = false;
		}
		else
		{
			CString textHowManySeconedLimited;
			textHowManySeconedLimited.LoadString( IDS_STRING226 );

			CString text;
			text.Format( textHowManySeconedLimited, mMaxSecond - mSecond );

			mTimeStatic.SetWindowText( text );
		}
	}

	CDialog::OnTimer(nIDEvent);
}


LRESULT CWaitDialog::SendMessage( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CLOSE:
		{
			mIsWaiting = false;
			break;
		}
	}

	return CDialog::SendMessage( message, wParam, lParam );
}


void CWaitDialog::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if( mIsWaiting )
	{
		return;
	}

  	CDialog::OnCancel();
}


void CWaitDialog::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if( mIsWaiting )
	{
		return;
	}

	CDialog::OnOK();
}

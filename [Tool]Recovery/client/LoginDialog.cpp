// LoginDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "LoginDialog.h"
#include "..\common\version.h"
#include ".\logindialog.h"


// CLoginDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLoginDialog, CDialog)
CLoginDialog::CLoginDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDialog::IDD, pParent)
	, mApplication( application )
{
	EnableAutomation();
}

CLoginDialog::~CLoginDialog()
{}

void CLoginDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Text( pDX, IDC_LOGIN_ID, mIdEdit );
	//DDX_Text( pDX, IDC_LOGIN_PASSWORD, mPasswordEdit );

	//DDX_Control( pDX, IDC_LOGIN_ID, mIdEdit );
	DDX_Control( pDX, IDC_LOGIN_PASSWORD, mPasswordEdit );
	//DDX_Control(pDX, IDC_LOGIN_VERSION_STATIC, mVersionStatic);
	DDX_Control(pDX, IDC_LOGIN_SERVER_LIST, mServerListCtrl);
	DDX_Control(pDX, IDC_LOGIN_VERSION_STATIC, mVersionStatic);
	DDX_Control(pDX, IDC_LOGIN_ID2, mIdEdit);
	DDX_Control(pDX, IDC_LOGIN_CONNECT_BUTTON, mSubmitButton);
	DDX_Control(pDX, IDCANCEL, mCancelButton);
	DDX_Control(pDX, IDC_LOGIN_LOADING_PROGRESS, mLoadingProgressCtrl);
	DDX_Control(pDX, IDC_STATIC_TITLE, mTitleImageStatic);
}


BEGIN_MESSAGE_MAP(CLoginDialog, CDialog)
	//ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDC_LOGIN_CONNECT_BUTTON, OnBnClickedLoginConnectButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CLoginDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_ILoginDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {E8293FEB-4841-4E9F-9297-89C994942D83}
static const IID IID_ILoginDialog =
{ 0xE8293FEB, 0x4841, 0x4E9F, { 0x92, 0x97, 0x89, 0xC9, 0x94, 0x94, 0x2D, 0x83 } };

BEGIN_INTERFACE_MAP(CLoginDialog, CDialog)
	INTERFACE_PART(CLoginDialog, IID_ILoginDialog, Dispatch)
END_INTERFACE_MAP()


// CLoginDialog 메시지 처리기입니다.

//void CLoginDialog::OnOK()
//{
//	CDialog::OnOK();
//}


void CLoginDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	switch( message->Protocol )
	{
	case MP_RM_CONNECT_ACK:
		{
			static size_t count;

			if( mApplication.GetConnectionSize() == ++count )
			{
				// 080304 LUJ, 로그
				{
					CString id;
					mIdEdit.GetWindowText( id );

					const TCHAR* line = _T( "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *" );

					mApplication.Log( line );
					mApplication.Log(
						_T( "\t Recovery Client %d.%d.%d is running by %s. Welcome" ),
						MAJORVERSION,
						MINORVERSION,
						BUILDNUMBER,
						id );
					mApplication.Log( line );

					// 081021 LUJ, 오퍼레이터를 창 제목에 표시한다
					{
						CWnd* window = AfxGetApp()->GetMainWnd();

						if( ! window )
						{
							break;
						}

						CString title;
						window->GetWindowText( title );
						window->SetWindowText( title + _T( ": " ) + id );
					}
				}

				CString textWarning;
				textWarning.LoadString( IDS_STRING127 );

				MessageBox( textWarning, _T( "" ), MB_ICONWARNING | MB_OK );

				OnOK();

				{
					CWnd* window = mApplication.GetMainWnd();
					ASSERT( window );

					if( window )
					{
						window->EnableWindow( TRUE );
					}

					CDialog* dialog = ( CDialog* )( mApplication.GetNewUserDialog() );
					ASSERT( dialog );

					if( dialog )
					{
						dialog->ShowWindow( SW_SHOW );
					}
				}
			}
			
			break;
		}
	case MP_RM_CONNECT_NACK:
		{
			MSG_DWORD* m = ( MSG_DWORD* )message;

			switch( m->dwData )
			{
			case 1:
				{
					CString textItIsNoConnectableIp;
					textItIsNoConnectableIp.LoadString( IDS_STRING130 );

					MessageBox( textItIsNoConnectableIp, _T( "" ), MB_ICONERROR | MB_OK );

					OnCancel();
					break;
				}
			case 2:
			case 3:
				{
					CString textItIsWrongIdOrPassword;
					textItIsWrongIdOrPassword.LoadString( IDS_STRING134 );

					mVersionStatic.SetWindowText( textItIsWrongIdOrPassword );

					{
						mIdEdit.EnableWindow( TRUE );
						mPasswordEdit.EnableWindow( TRUE );
						mSubmitButton.EnableWindow( TRUE );
					}

					break;
				}
			default:
				{
					CString textItIsConnectionFault;
					textItIsConnectionFault.LoadString( IDS_STRING131 );

					MessageBox( textItIsConnectionFault, _T( "" ), MB_ICONERROR | MB_OK );

					OnCancel();
					break;
				}
			}
			
			break;
		}
	case MP_RM_CONNECT_NACK_BY_IP:
	case MP_RM_CONNECT_NACK_BY_ID:
		{
			CString textYouHaveLoggedAlready;
			textYouHaveLoggedAlready.LoadString( IDS_STRING132 );

			MessageBox( textYouHaveLoggedAlready, _T( "" ), MB_ICONERROR | MB_OK );

			OnCancel();
			break;
		}
	case MP_RM_CONNECT_NACK_BY_AUTH:
		{
			CString textYouAreBlocked;
			textYouAreBlocked.LoadString( IDS_STRING133 );

			MessageBox( textYouAreBlocked, _T( "" ), MB_OK | MB_ICONERROR );

			OnCancel();
			break;
		}
	case MP_RM_CONNECT_NACK_BY_VERSION:
		{
			const MSG_INT2* m = ( MSG_INT2* )message;

			CString text;
			{
				CString textInvalidVersionFound;
				textInvalidVersionFound.LoadString( IDS_STRING343 );

				text.Format(
					textInvalidVersionFound,
					mApplication.GetServerName( serverIndex ),
					m->nData1,
					m->nData2 );
			}

			mApplication.Log( text );

			MessageBox( text, _T( "" ), MB_OK | MB_ICONERROR );
            OnCancel();            			
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


BOOL CLoginDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifdef _DEBUG
	mIdEdit.SetWindowText( _T( "ehei" ) );
	mPasswordEdit.SetWindowText( _T( "1488" ) );
#endif

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	CString textBuildData;
	textBuildData.LoadString( IDS_STRING135 );

	// 리스트 컨트롤 세팅
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
		CRect rect;

		{
			int step = -1;

			mServerListCtrl.GetWindowRect( rect );
			mServerListCtrl.SetExtendedStyle( mServerListCtrl.GetExtendedStyle() | addStyle );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mServerListCtrl.InsertColumn( ++step, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mServerListCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.4f ) );

			CString textIpAddress;
			textIpAddress.LoadString( IDS_STRING136 );
			mServerListCtrl.InsertColumn( ++step, textIpAddress, LVCFMT_LEFT, int( rect.Width() * 0.45f ) );

			CString textEnable;
			textEnable.LoadString( IDS_STRING25 );
			mServerListCtrl.InsertColumn( ++step, textEnable, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

			mApplication.SetServerListCtrl( mServerListCtrl );
		}
	}

	// 080630 LUJ, 로딩이 끝난 후 활성화시킨다
	mIdEdit.EnableWindow( FALSE );
	mPasswordEdit.EnableWindow( FALSE );
	mSubmitButton.EnableWindow( FALSE );
	mCancelButton.EnableWindow( FALSE );

	const int titleImage[] = {
		IDB_BITMAP3,
		IDB_BITMAP4,
		IDB_BITMAP5,
		IDB_BITMAP6,
		IDB_BITMAP7,
		IDB_BITMAP8,
		IDB_BITMAP9,
		IDB_BITMAP10,
		IDB_BITMAP11,
		IDB_BITMAP12,
		IDB_BITMAP13,
		IDB_BITMAP14,
		IDB_BITMAP15,
		IDB_BITMAP16,
	};

	srand(
		GetTickCount());
	mTitleImage.LoadBitmap(
		titleImage[rand() % _countof(titleImage)]);
	mTitleImageStatic.SetBitmap(
		mTitleImage);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CLoginDialog::OnBnClickedLoginConnectButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( ! mApplication.GetConnectionSize() )
	{
		CString textAnyConnectableServerDoNotExist;
		textAnyConnectableServerDoNotExist.LoadString( IDS_STRING137 );

		MessageBox( textAnyConnectableServerDoNotExist, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CDataExchange dx( this, TRUE );
	DoDataExchange( &dx );

	CString id;
	CString password;

	mIdEdit.GetWindowText( id );
	mPasswordEdit.GetWindowText( password );

	if( id.IsEmpty() || password.IsEmpty() )
	{
		CString textIdOrPasswordIsEmpty;
		textIdOrPasswordIsEmpty.LoadString( IDS_STRING138 );

		MessageBox( textIdOrPasswordIsEmpty, _T( "" ), MB_OK );
		return;
	}

	{
		mIdEdit.EnableWindow( FALSE );
		mPasswordEdit.EnableWindow( FALSE );
		mSubmitButton.EnableWindow( FALSE );
	}

	{
		MSG_RM_LOGIN message;

		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_CONNECT;
		message.Protocol = MP_RM_CONNECT_SYN;

		StringCopySafe( message.mId,	CW2AEX< MAX_PATH >( id ),		sizeof( message.mId ) );
		StringCopySafe( message.mPass,	CW2AEX< MAX_PATH >( password ),	sizeof( message.mPass ) );

		{
			MSG_RM_LOGIN::Version& version = message.mVersion;

			version.mMajor	= MAJORVERSION;
			version.mMinor	= MINORVERSION;
		}
		
		mApplication.SendAll( message, sizeof( message ) );
	}
}


// 080630 LUJ, 로딩 프로그레스 컨트롤의 값을 설정한다
void CLoginDialog::SetProgressValue( int value, const TCHAR* message )
{
	mLoadingProgressCtrl.SetPos( value );
	mVersionStatic.SetWindowText( message );

	// 080630 LUJ, 모두 로딩했으면 로그인할 수 있도록 한다
	{
		int minRange = 0;
		int maxRange = 0;
		mLoadingProgressCtrl.GetRange( minRange, maxRange );

		if( mLoadingProgressCtrl.GetPos() >= maxRange )
		{
			mIdEdit.EnableWindow( TRUE );
			mPasswordEdit.EnableWindow( TRUE );
			mSubmitButton.EnableWindow( TRUE );
			mCancelButton.EnableWindow( TRUE );

			// 서버 상태를 업데이트한다.
			mApplication.SetServerListCtrl( mServerListCtrl );
		}
	}	
}


void CLoginDialog::OnCancel()
{
	AfxGetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );

	CDialog::OnCancel();
}
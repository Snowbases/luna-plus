// PermissionDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "PermissionDialog.h"
#include ".\permissiondialog.h"


// CPermissionDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPermissionDialog, CDialog)
CPermissionDialog::CPermissionDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CPermissionDialog::IDD, pParent),
		mApplication( application )
{
	EnableAutomation();

}

CPermissionDialog::~CPermissionDialog()
{
}

void CPermissionDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CPermissionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PERMISSION_IPADDRESS, mIpAddressCtrl);
}


BEGIN_MESSAGE_MAP(CPermissionDialog, CDialog)
	ON_BN_CLICKED(IDC_PERMISSION_ADD_BUTTON, OnBnClickedPermissionAddButton)
	ON_BN_CLICKED(IDC_PERMISSION_REMOVE_BUTTON, OnBnClickedPermissionRemoveButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CPermissionDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IPermissionDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {E7861B05-F869-48A8-B029-C11065B5BA1F}
static const IID IID_IPermissionDialog =
{ 0xE7861B05, 0xF869, 0x48A8, { 0xB0, 0x29, 0xC1, 0x10, 0x65, 0xB5, 0xBA, 0x1F } };

BEGIN_INTERFACE_MAP(CPermissionDialog, CDialog)
	INTERFACE_PART(CPermissionDialog, IID_IPermissionDialog, Dispatch)
END_INTERFACE_MAP()


// CPermissionDialog 메시지 처리기입니다.

void CPermissionDialog::OnBnClickedPermissionAddButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	const CString address( GetIpAddress() );

	{
		MSG_NAME message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_PERMISSION;
		message.Protocol	= MP_RM_PERMISSION_ADD_SYN;

		StringCopySafe( message.Name, CW2AEX< MAX_PATH >( address ), sizeof( message.Name ) );
		
		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}	
}


void CPermissionDialog::OnBnClickedPermissionRemoveButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	BYTE field0;
	BYTE field1;
	BYTE field2;
	BYTE field3;

	mIpAddressCtrl.GetAddress( field0, field1, field2, field3 );

	if( ! field0 &&
		! field1 &&
		! field2 &&
		! field3 )
	{
		CString textInputAddress;
		textInputAddress.LoadString( IDS_STRING145 );

		MessageBox( textInputAddress, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	{
		MSG_NAME message;

		message.Category	= MP_RM_PERMISSION;
		message.Protocol	= MP_RM_PERMISSION_REMOVE_SYN;
		sprintf( message.Name, "%d.%d.%d.%d", field0, field1, field2, field3 );

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}	
}


void CPermissionDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
	case MP_RM_PERMISSION_ADD_ACK:
		{
			CString textIpAddressIsAllowed;
			textIpAddressIsAllowed.LoadString( IDS_STRING157 );

			MessageBox( textIpAddressIsAllowed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_PERMISSION_ADD_NACK:
		{
			CString textIpAddressIsAllowedAlready;
			textIpAddressIsAllowedAlready.LoadString( IDS_STRING158 );

			MessageBox( textIpAddressIsAllowedAlready, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_PERMISSION_REMOVE_ACK:
		{
			CString textIpAddressIsRemoved;
			textIpAddressIsRemoved.LoadString( IDS_STRING159 );

			MessageBox( textIpAddressIsRemoved, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_PERMISSION_REMOVE_NACK:
		{
			CString textIpAddressIsNotExisted;
			textIpAddressIsNotExisted.LoadString( IDS_STRING160 );

			MessageBox( textIpAddressIsNotExisted, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_PERMISSION_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CPermissionDialog::Request( DWORD serverIndex )
{
	mServerIndex = serverIndex;

	ShowWindow( SW_SHOW );
}


CString CPermissionDialog::GetIpAddress()
{
	BYTE field0;
	BYTE field1;
	BYTE field2;
	BYTE field3;

	mIpAddressCtrl.GetAddress( field0, field1, field2, field3 );	

	CString text;

	text.Format( _T( "%d.%d.%d.%d" ), field0, field1, field2, field3 );

	return text;
}
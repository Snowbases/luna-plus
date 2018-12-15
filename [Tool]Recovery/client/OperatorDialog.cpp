// OperatorDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "OperatorDialog.h"


// COperatorDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(COperatorDialog, CDialog)
COperatorDialog::COperatorDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(COperatorDialog::IDD, pParent)
	, mApplication( application )
{
	EnableAutomation();
}


COperatorDialog::~COperatorDialog()
{}


void COperatorDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}


void COperatorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPERATOR_LIST_CTRL, mOperatorListCtrl);
	DDX_Control(pDX, IDC_OPERATOR_ID_EDIT, mIdEdit);
	DDX_Control(pDX, IDC_OPERATOR_NAME_EDIT, mNameEdit);
	DDX_Control(pDX, IDC_OPERATOR_AUTH_COMBO, mRankCombo);
	DDX_Control(pDX, IDC_OPERATOR_ADD_BUTTON, mAddButton);
	DDX_Control(pDX, IDC_OPERATOR_IPADDRESS, mIpAddressCtrl);
	DDX_Control(pDX, IDC_IP_LIST_CTRL, mIpListCtrl);
	DDX_Control(pDX, IDC_OPERATOR_ADD_ID_EDIT, mAddIdEdit);
	DDX_Control(pDX, IDC_OPERATOR_ADD_NAME_EDIT, mAddNameEdit);
}


BEGIN_MESSAGE_MAP(COperatorDialog, CDialog)
	//ON_BN_CLICKED(IDC_OPERATOR_REMOVE_BUTTON, OnBnClickedOperatorRemoveButton)
	ON_BN_CLICKED(IDC_OPERATOR_ADD_BUTTON, OnBnClickedOperatorAddButton)
	ON_BN_CLICKED(IDC_OPERATOR_IP_ADD_BUTTON, OnBnClickedOperatorIpAddButton)
	ON_BN_CLICKED(IDC_OPERATOR_IP_REMOVE_BUTTON, OnBnClickedOperatorIpRemoveButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_OPERATOR_LIST_CTRL, OnLvnItemchangedOperatorListCtrl)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_IP_LIST_CTRL, OnLvnItemchangedIpListCtrl)
	ON_BN_CLICKED(IDC_OPERATOR_UPDATE_BUTTON, OnBnClickedOperatorUpdateButton)
	ON_BN_CLICKED(IDC_OPERATOR_REMOVE_BUTTON, OnBnClickedOperatorRemoveButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COperatorDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IOperatorDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {3A19ACEC-EFED-4BF4-AA74-7C78F2B176CF}
static const IID IID_IOperatorDialog =
{ 0x3A19ACEC, 0xEFED, 0x4BF4, { 0xAA, 0x74, 0x7C, 0x78, 0xF2, 0xB1, 0x76, 0xCF } };

BEGIN_INTERFACE_MAP(COperatorDialog, CDialog)
	INTERFACE_PART(COperatorDialog, IID_IOperatorDialog, Dispatch)
END_INTERFACE_MAP()


// COperatorDialog 메시지 처리기입니다.


BOOL COperatorDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// list control
	{
		const DWORD style = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;

		{
			CRect	rect;
			int		step = -1;

			mOperatorListCtrl.GetWindowRect( &rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );

			mOperatorListCtrl.InsertColumn( ++step, textIndex,			LVCFMT_LEFT, int( rect.Width() * 0.0f ) );

			CString textId;
			textId.LoadString( IDS_STRING139 );
			mOperatorListCtrl.InsertColumn( ++step, textId,				LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mOperatorListCtrl.InsertColumn( ++step, textName,			LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

			CString textRankCode;
			textRankCode.LoadString( IDS_STRING4 );
			mOperatorListCtrl.InsertColumn( ++step, textRankCode,		LVCFMT_LEFT, int( rect.Width() * 0.0f ) );

			CString textRank;
			textRank.LoadString( IDS_STRING43 );
			mOperatorListCtrl.InsertColumn( ++step, textRank,			LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

			CString textConnectedIp;
			textConnectedIp.LoadString( IDS_STRING140 );
			mOperatorListCtrl.InsertColumn( ++step, textConnectedIp,	LVCFMT_LEFT, int( rect.Width() * 0.4f ) );

			CString textDate;
			textDate.LoadString( IDS_STRING44 );
			mOperatorListCtrl.InsertColumn( ++step, textDate,			LVCFMT_LEFT, int( rect.Width() * 0.4f ) );

			mOperatorListCtrl.SetExtendedStyle( mOperatorListCtrl.GetExtendedStyle() | style );
		}

		{
			CRect	rect;
			int		step = -1;

			mIpListCtrl.GetWindowRect( &rect );
			mIpListCtrl.InsertColumn( ++step, _T( "IP" ),	LVCFMT_LEFT, rect.Width() );
			mIpListCtrl.SetExtendedStyle( mIpListCtrl.GetExtendedStyle() | style );
		}	
	}

	// initialize combo box
	{
		CRect rect;

		mRankCombo.SetCurSel( 0 );
		mRankCombo.GetWindowRect( rect );
		mRankCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

		//mRankCombo.AddString( mApplication.GetText( MSG_RM_OPERATOR::Operator::None ) );
		//mRankCombo.AddString( mApplication.GetText( MSG_RM_OPERATOR::Operator::Monitor ) );
		//mRankCombo.AddString( mApplication.GetText( MSG_RM_OPERATOR::Operator::Staff ) );
		//mRankCombo.AddString( mApplication.GetText( MSG_RM_OPERATOR::Operator::Manager ) );

		// add string by enum value order

		mRankCombo.InsertString( eGM_POWER_MASTER,		mApplication.GetText( eGM_POWER_MASTER		) );
		mRankCombo.InsertString( eGM_POWER_MONITOR,		mApplication.GetText( eGM_POWER_MONITOR		) );
		mRankCombo.InsertString( eGM_POWER_PATROLLER,	mApplication.GetText( eGM_POWER_PATROLLER	) );
		mRankCombo.InsertString( eGM_POWER_AUDITOR,		mApplication.GetText( eGM_POWER_AUDITOR		) );
		mRankCombo.InsertString( eGM_POWER_EVENTER,		mApplication.GetText( eGM_POWER_EVENTER		) );
		mRankCombo.InsertString( eGM_POWER_QA,			mApplication.GetText( eGM_POWER_QA			) );
	}

	// edit control
	{
		mIdEdit.SetLimitText( MAX_NAME_LENGTH );
		mNameEdit.SetLimitText( MAX_NAME_LENGTH );

		mAddIdEdit.SetLimitText( MAX_NAME_LENGTH );
		mAddNameEdit.SetLimitText( MAX_NAME_LENGTH );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void COperatorDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
	case MP_RM_OPERATOR_GET_ACK:
		{
			const MSG_RM_OPERATOR* m = ( MSG_RM_OPERATOR* )message;

			for( size_t i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_OPERATOR::Operator& data = m->mOperator[ i ];

				const int row = mOperatorListCtrl.GetItemCount();

				{
					CString text;
					text.Format( _T( "%d" ), data.mIndex );
					mOperatorListCtrl.InsertItem( row, text, 0 );
				}				

				mOperatorListCtrl.SetItemText( row, 1, CA2WEX< sizeof( data.mId ) >( data.mId ) );
				mOperatorListCtrl.SetItemText( row, 2, CA2WEX< sizeof( data.mName ) >( data.mName ) );

				{
					CString text;
					text.Format( _T( "%d" ), data.mPower );
					mOperatorListCtrl.SetItemText( row, 3, text );
				}				
				
				mOperatorListCtrl.SetItemText( row, 4, mApplication.GetText( data.mPower ) );
				mOperatorListCtrl.SetItemText( row, 5, CA2WEX< sizeof( data.mIp ) >( data.mIp ) );
				mOperatorListCtrl.SetItemText( row, 6, CA2WEX< sizeof( data.mIp ) >( data.mRegistedDate ) );
			}
			
			break;
		}
	case MP_RM_OPERATOR_IP_GET_ACK:
		{
			const MSG_RM_OPERATOR_IP* m = ( MSG_RM_OPERATOR_IP* )message;

			for( size_t i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_OPERATOR_IP::Address& address = m->mAddress[ i ];

				const int row = mOperatorListCtrl.GetItemCount();

				mIpListCtrl.InsertItem( row, CA2WEX< sizeof( address.mIp ) >( address.mIp ), 0 );
			}
			
			break;
		}
	case MP_RM_OPERATOR_NACK_BY_AUTH:
		{
			ShowWindow( SW_HIDE );

			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	case MP_RM_OPERATOR_ADD_ACK:
		{
			const MSG_RM_OPERATOR* m = ( MSG_RM_OPERATOR* )message;			
			const MSG_RM_OPERATOR::Operator& data = m->mOperator[ 0 ];

			// add to list control
			{
				const int row = mOperatorListCtrl.GetItemCount();

				CString text;
				text.Format( _T( "%d" ), data.mIndex );

				mOperatorListCtrl.InsertItem( row, text, 0 );

				mOperatorListCtrl.SetItemText( row, 1, CA2WEX< sizeof( data.mId ) >( data.mId ) );;
				mOperatorListCtrl.SetItemText( row, 2, CA2WEX< sizeof( data.mName ) >( data.mName ) );

				{
					CString text;
					text.Format( _T( "%d" ), data.mPower );
					mOperatorListCtrl.SetItemText( row, 3, text );
				}				

				mOperatorListCtrl.SetItemText( row, 4, mApplication.GetText( data.mPower ) );
				mOperatorListCtrl.SetItemText( row, 5, CA2WEX< sizeof( data.mIp ) >( data.mIp ) );
				mOperatorListCtrl.SetItemText( row, 6, CA2WEX< sizeof( data.mRegistedDate ) >( data.mRegistedDate ) );
			}

			CString textAddingForOperatorIsSucceed;
			textAddingForOperatorIsSucceed.LoadString( IDS_STRING141 );

			MessageBox( textAddingForOperatorIsSucceed, _T( "" ), MB_OK | MB_ICONINFORMATION );
			break;
		}
	case MP_RM_OPERATOR_REMOVE_ACK:
		{
			const MSGBASE* m = ( MSGBASE* )message;
			
			for( int row = 0; row < mOperatorListCtrl.GetItemCount(); ++row )
			{
				if( m->dwObjectID == _ttoi( mOperatorListCtrl.GetItemText( row, 0 ) ) )
				{
					mOperatorListCtrl.DeleteItem( row );
					break;
				}
			}

			mNameEdit.SetWindowText( _T( "" ) );
			mIdEdit.SetWindowText( _T( "" ) );

			mRankCombo.SetCurSel( -1 );

			break;
		}
	case MP_RM_OPERATOR_ADD_NACK_BY_ID:
		{
			CString textSameIsExisted;
			textSameIsExisted.LoadString( IDS_STRING142 );;

			MessageBox( textSameIsExisted, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	case MP_RM_OPERATOR_SET_ACK:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;
			
			for( int row = 0; row < mOperatorListCtrl.GetItemCount(); ++row )
			{
				if( _ttoi( mOperatorListCtrl.GetItemText( row, 0 ) ) == m->dwObjectID )
				{
					CString text;
					text.Format( _T( "%d" ), m->dwData );
					mOperatorListCtrl.SetItemText(
						row,
						3,
						text );

					mOperatorListCtrl.SetItemText(
						row,
						4,
						mApplication.GetText( eGM_POWER( m->dwData ) ) );
					break;
				}
			}


			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_OK | MB_ICONINFORMATION );
			break;
		}
	case MP_RM_OPERATOR_IP_ADD_ACK:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			mIpListCtrl.InsertItem( mIpListCtrl.GetItemCount(), CA2WEX< sizeof( m->Name ) >( m->Name ), 0 );
		
			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_OK | MB_ICONINFORMATION );
			break;
		}
	case MP_RM_OPERATOR_IP_ADD_NACK:
		{
			CString textUpdateWasFailed;
			textUpdateWasFailed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateWasFailed, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	case MP_RM_OPERATOR_IP_REMOVE_ACK:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			for( int row = 0; row < mIpListCtrl.GetItemCount(); ++row )
			{
				if( mIpListCtrl.GetItemText( row, 0 ) == m->Name )
				{
					mIpListCtrl.DeleteItem( row );
					break;
				}
			}

			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_OK | MB_ICONINFORMATION );
			break;
		}
	case MP_RM_OPERATOR_IP_REMOVE_NACK:
		{
			CString textUpdateWasFailed;
			textUpdateWasFailed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateWasFailed, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void COperatorDialog::Request( DWORD serverIndex )
{
	{
		mServerIndex = serverIndex;
	}
	
	{
		mIpListCtrl.DeleteAllItems();
		mOperatorListCtrl.DeleteAllItems();

		mAddIdEdit.SetWindowText( _T( "" ) );
		mAddNameEdit.SetWindowText( _T( "" ) );
		mRankCombo.SetCurSel( 0 );
	}

	{
		MSGROOT message;
		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_GET_SYN;

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}





void COperatorDialog::OnBnClickedOperatorUpdateButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int row;
	{
		POSITION position = mOperatorListCtrl.GetFirstSelectedItemPosition();

		row = mOperatorListCtrl.GetNextSelectedItem( position );
	}

	{
		MSG_DWORD message;
		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_SET_SYN;
		message.dwObjectID	= _ttoi( mOperatorListCtrl.GetItemText( row, 0 ) );
		message.dwData		= mRankCombo.GetCurSel();

		if( _ttoi( mOperatorListCtrl.GetItemText( row, 3 ) ) == message.dwData )
		{
			return;
		}

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}


void COperatorDialog::OnBnClickedOperatorAddButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	MSG_RM_OPERATOR message;
	message.Category	= MP_RM_OPERATOR;
	message.Protocol	= MP_RM_OPERATOR_ADD_SYN;
	message.mSize		= 1;

	CString id;
	{
		mAddIdEdit.GetWindowText( id );

		if( id.IsEmpty() )
		{
			CString textNameOrIdIsEmpty;
			textNameOrIdIsEmpty.LoadString( IDS_STRING143 );

			MessageBox( textNameOrIdIsEmpty, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}
		else if( MAX_ROW_NUM == mOperatorListCtrl.GetItemCount() )
		{
			CString textYouCanNotAddAnyMore;
			textYouCanNotAddAnyMore.LoadString( IDS_STRING220 );

			MessageBox( textYouCanNotAddAnyMore, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}

		char buffer[ MAX_PATH ] = { 0 };
		StringCopySafe( buffer, CW2AEX< MAX_PATH >( id ), sizeof( buffer ) );

		for(
			const char* ch = buffer;
			*ch;
			++ch )
		{
			if(		IsDBCSLeadByte( *ch ) ||
				!	isalnum( *ch ) )
			{
				CString textItIsAllowEnglishOrNumericOnly;
				textItIsAllowEnglishOrNumericOnly.LoadString( IDS_STRING44 );

				MessageBox( textItIsAllowEnglishOrNumericOnly, _T( "" ), MB_ICONERROR | MB_OK );
				return;
			}
		}
	}	

	CString name;
	{
		mAddNameEdit.GetWindowText( name );

		if( name.IsEmpty() )
		{
			CString textNameOrIdIsEmpty;
			textNameOrIdIsEmpty.LoadString( IDS_STRING143 );

			MessageBox( textNameOrIdIsEmpty, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}
	}	

	for( int row = 0; row < mOperatorListCtrl.GetItemCount(); ++row )
	{
		if( id == mOperatorListCtrl.GetItemText( row, 1 ) )
		{
			CString textSameIdExist;
			textSameIdExist.LoadString( IDS_STRING142 );

			MessageBox( textSameIdExist, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}
	}

	MSG_RM_OPERATOR::Operator& data = message.mOperator[ 0 ];
	{
		ZeroMemory( &data, sizeof( data ) );

		CString text;

		mAddIdEdit.GetWindowText( text );
		StringCopySafe( data.mId, CW2AEX< MAX_PATH >( id ), sizeof( data.mId ) );

		mAddNameEdit.GetWindowText( text );
		StringCopySafe( data.mName, CW2AEX< MAX_PATH >( name ), sizeof( data.mName ) );
	}

	mApplication.Send( mServerIndex, message, message.GetSize() );
}


void COperatorDialog::OnBnClickedOperatorIpAddButton()
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
		CString textItIsWrongAddress;
		textItIsWrongAddress.LoadString( IDS_STRING145 );

		MessageBox( textItIsWrongAddress, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CString ip;
	ip.Format( _T( "%d.%d.%d.%d" ), field0, field1, field2, field3 );

	for( int row = 0; row < mIpListCtrl.GetItemCount(); ++row )
	{
		if( ip == mIpListCtrl.GetItemText( row, 0 ) )
		{
			CString textItIsRegistedIpAddressAlready;
			textItIsRegistedIpAddressAlready.LoadString( IDS_STRING146 );

			MessageBox( textItIsRegistedIpAddressAlready, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}
	}

	{
		MSG_NAME message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_IP_ADD_SYN;

		StringCopySafe( message.Name, CW2AEX< MAX_PATH >( ip ), sizeof( message.Name ) );
		
		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}


void COperatorDialog::OnBnClickedOperatorIpRemoveButton()
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
		CString textItIsWrongAddress;
		textItIsWrongAddress.LoadString( IDS_STRING145 );

		MessageBox( textItIsWrongAddress, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	{
		MSG_NAME message;
		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_IP_REMOVE_SYN;

		sprintf( message.Name, "%d.%d.%d.%d", field0, field1, field2, field3 );
		
		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}


void COperatorDialog::OnLvnItemchangedOperatorListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int row;
	{
		POSITION position = mOperatorListCtrl.GetFirstSelectedItemPosition();

		row = mOperatorListCtrl.GetNextSelectedItem( position );
	}

	{
		mIdEdit.SetWindowText( mOperatorListCtrl.GetItemText( row, 1 ) );
		mNameEdit.SetWindowText( mOperatorListCtrl.GetItemText( row, 2 ) );

		mRankCombo.SetCurSel( _ttoi( mOperatorListCtrl.GetItemText( row, 3 ) ) );
	}
}


void COperatorDialog::OnLvnItemchangedIpListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int row;
	{
		POSITION position = mIpListCtrl.GetFirstSelectedItemPosition();

		row = mIpListCtrl.GetNextSelectedItem( position );
	}

	mIpAddressCtrl.SetWindowText( mIpListCtrl.GetItemText( row, 0 ) );
}

void COperatorDialog::OnBnClickedOperatorRemoveButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	CString name;
	mNameEdit.GetWindowText( name );

	if( name.IsEmpty() )
	{
		return;
	}

	CString textDoYouRemoveit;
	textDoYouRemoveit.LoadString( IDS_STRING108 );

	if( IDNO == MessageBox( textDoYouRemoveit, _T( "" ), MB_YESNO | MB_ICONWARNING ) )
	{
		return;
	}

	MSGBASE message;
	message.Category	= MP_RM_OPERATOR;
	message.Protocol	= MP_RM_OPERATOR_REMOVE_SYN;

	POSITION	position	= mOperatorListCtrl.GetFirstSelectedItemPosition();
	const int	row			= mOperatorListCtrl.GetNextSelectedItem( position );

	message.dwObjectID	= _ttoi( mOperatorListCtrl.GetItemText( row, 0 ) );

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}

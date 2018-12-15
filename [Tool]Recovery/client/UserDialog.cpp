// UserDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "UserDialog.h"
#include "PlayerDialog.h"
#include "UserRestoreDialog.h"
#include "UserCreateDialog.h"
// 080403 LUJ, 아이템 구입 내역 로그 창
#include "LogDialog.h"
#include "ItemShopLogCommand.h"
#include "GameRoomPointLogCommand.h"
#include ".\userdialog.h"

IMPLEMENT_DYNAMIC(CUserDialog, CDialog)
CUserDialog::CUserDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CUserDialog::IDD, pParent)
	, mApplication( application )
{
	EnableAutomation();

}

CUserDialog::~CUserDialog()
{
}

void CUserDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CUserDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_USER_LOGID_EDIT, midEdit);
	//DDX_Control(pDX, IDC_USER_PASSWORD_CHANGE_BUTTON, mPasswordChangeButton);
	//DDX_Control(pDX, IDC_USER_PASSWORD_HISTORY_BUTTON, mPasswordHistoryButton);
	DDX_Control(pDX, IDC_USER_CREATED_DATE_EDIT, mJoinedDateEdit);
	DDX_Control(pDX, IDC_USER_LOGOUT_DATE_EDIT, mLogoutDateEdit);
	//DDX_Control(pDX, IDC_USER_PLAY_TIME_EDIT, mPlayTimeEdit);
	//DDX_Control(pDX, IDC_USER_LEVEL_COMBO, mLevelCombo);
	DDX_Control(pDX, IDC_USER_CREATED_PLAYER_LIST, mLivePlayerListCtrl);
	//DDX_Control(pDX, IDC_USER_DELETED_PLAYER_LIST, mDeletedPlayerListCtrl);
	//DDX_Control(pDX, IDC_USER_CREATED_PLAYER_BUTTON, mLivePlayerButton);
	//DDX_Control(pDX, IDC_USER_DELETED_PLAYER_BUTTON, mDeletedPlayerButton);
	//DDX_Control(pDX, IDC_USER_LOGID_EDIT2, midEdit);
	//DDX_Control(pDX, IDC_USER_IS_CONNECT_STATIC, mIsConnectStatic);
	//DDX_Control(pDX, IDC_USER_CONNECT_STATIC, mConnectStatic);
	DDX_Control(pDX, IDC_USER_AGENT_STATIC2, mAgentStatic);
	DDX_Control(pDX, IDC_USER_DELETED_PLAYER_LIST2, mDeletedPlayerListCtrl);
	DDX_Control(pDX, IDC_USER_LIVE_PLAYER_BUTTON, mLivePlayerButton);
	DDX_Control(pDX, IDC_USER_LOGID_EDIT3, midEdit);
	DDX_Control(pDX, IDC_USER_CONNECT_STATIC2, mConnectStatic);
	DDX_Control(pDX, IDC_NAME_STATIC, mNameStatic);
	DDX_Control(pDX, IDC_USER_LEVEL_COMBO, mLevelCombo);
	DDX_Control(pDX, IDC_USER_STATE_COMBO, mStateCombo);
	DDX_Control(pDX, IDC_USER_AUTONOTE_STATE_COMBO, mAutoPunishStateCombo);
	DDX_Control(pDX, IDC_USER_AUTONOTE_LIMITSTARTDATE_COMBO, mAutoPunishStartDate);
	DDX_Control(pDX, IDC_USER_AUTONOTE_LIMITENDDATE_COMBO, mAutoPunishEndDate);
	DDX_Control(pDX, IDC_USER_AUTONOTE_LIMITTIME_COMBO, mAutoPunishTime);
}


BEGIN_MESSAGE_MAP(CUserDialog, CDialog)
	//ON_BN_CLICKED(IDC_USER_PASSWORD_CHANGE_BUTTON, OnBnClickedUserPasswordChangeButton)
	//ON_BN_CLICKED(IDC_USER_PASSWORD_HISTORY_BUTTON, OnBnClickedUserPasswordHistoryButton)
	ON_BN_CLICKED(IDC_USER_CREATED_PLAYER_BUTTON, OnBnClickedUserLivePlayerButton)
	ON_BN_CLICKED(IDC_USER_UPDATE_BUTTON, OnBnClickedUserUpdateButton)
	ON_BN_CLICKED(IDC_USER_CREATE_BUTTON, OnBnClickedUserCreateButton)
	ON_BN_CLICKED(IDC_USER_RESTORE_BUTTON, OnBnClickedUserRestoreButton)
	ON_BN_CLICKED(IDC_USER_DELETE_BUTTON, OnBnClickedUserDeleteButton)
	ON_BN_CLICKED(IDC_USER_ITEM_SHOP_LOG_BUTTON, OnBnClickedUserItemShopLogButton)
	ON_BN_CLICKED(IDC_USER_REMOVE_PLAYER_BUTTON, OnBnClickedUserRemovePlayerButton)
	ON_BN_CLICKED(IDC_USER_GAMEROOM_LOG_BUTTON, OnBnClickedUserGameroomLogButton)
	ON_BN_CLICKED(IDC_USER_GAMEROOM_UPDATE_BUTTON, OnBnClickedUserGameroomUpdateButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CUserDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IUserDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {F335733F-FD60-4843-B9CD-9FB81EF04BB5}
static const IID IID_IUserDialog =
{ 0xF335733F, 0xFD60, 0x4843, { 0xB9, 0xCD, 0x9F, 0xB8, 0x1E, 0xF0, 0x4B, 0xB5 } };

BEGIN_INTERFACE_MAP(CUserDialog, CDialog)
	INTERFACE_PART(CUserDialog, IID_IUserDialog, Dispatch)
END_INTERFACE_MAP()


// CUserDialog 메시지 처리기입니다.

//void CUserDialog::OnBnClickedUserPasswordChangeButton()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//}
//
//
//void CUserDialog::OnBnClickedUserPasswordHistoryButton()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//}


void CUserDialog::OnBnClickedUserLivePlayerButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION		position	= mLivePlayerListCtrl.GetFirstSelectedItemPosition();
	const DWORD		row			= mLivePlayerListCtrl.GetNextSelectedItem( position );
	const CString	playerIndex	= mLivePlayerListCtrl.GetItemText( row, 0 );

	if( playerIndex.IsEmpty() )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	const CString	playerName	= mLivePlayerListCtrl.GetItemText( row, 1 );

	CPlayerDialog* dialog = mApplication.GetPlayerDialog();
	ASSERT( dialog );

	dialog->Request( mServerIndex, playerName, _ttoi( playerIndex ) );
}

BOOL CUserDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	// 리스트 컨트롤 세팅
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
		CRect		rect;

		{
			int step = -1;

			mLivePlayerListCtrl.GetClientRect( rect );
			mLivePlayerListCtrl.SetExtendedStyle( mLivePlayerListCtrl.GetExtendedStyle() | addStyle );

			CString textDbIndex;
			textDbIndex.LoadString( IDS_STRING13 );
			mLivePlayerListCtrl.InsertColumn( ++step, textDbIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mLivePlayerListCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 1.0f )  );
		}

		{
			int step = -1;

			mDeletedPlayerListCtrl.GetClientRect( rect );
			mDeletedPlayerListCtrl.SetExtendedStyle( mDeletedPlayerListCtrl.GetExtendedStyle() | addStyle );

			CString textDbIndex;
			textDbIndex.LoadString( IDS_STRING13 );
			mDeletedPlayerListCtrl.InsertColumn( ++step, textDbIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mDeletedPlayerListCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 1.0f )  );
		}
	}

	// 콤보 박스 세팅
	{
		// 080725 LUJ, 레벨
		{
			CRect rect;
			mLevelCombo.GetWindowRect( rect );
			mLevelCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			mApplication.SetUserLevelCombo( mLevelCombo );
		}
		
		// 080725 LUJ, 상태
		{
			CRect rect;
			mStateCombo.GetWindowRect( rect );
			mStateCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			mStateCombo.AddString( GetText( StateNone ) );
			mStateCombo.AddString( GetText( StateLive ) );
			mStateCombo.AddString( GetText( StateBlock ) );
			mStateCombo.AddString( GetText( StateSecede ) );
		}

		// 090618 ShinJS --- AutoNote 상태
		{
			CRect rect;
			mAutoPunishStateCombo.GetWindowRect( rect );
			mAutoPunishStateCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			mAutoPunishStateCombo.AddString( GetText( StateAutoPunishNone ) );
			mAutoPunishStateCombo.AddString( GetText( StateAutoPunishUseNote ) );
			mAutoPunishStateCombo.AddString( GetText( StateAutoPunishLogin ) );
			mAutoPunishStateCombo.AddString( GetText( StateAutoPunishBan ) );
		}
	}

	// 게임방 그리드 컨트롤
	if(CWnd* const frameWindow = GetDlgItem(IDC_USER_GAMEROOM_FRAME))
	{
		CRect frameWindowRect;
		frameWindow->GetWindowRect(
			frameWindowRect);
		ScreenToClient(
			frameWindowRect);

		CRect gridRect = frameWindowRect;
		const int titleColumn = int( gridRect.Width() * 0.6f );
		const int valueColumn = int( gridRect.Width() * 0.4f );
		const int columnWidth = titleColumn + valueColumn;

		CString textGameRoomIndex;
		textGameRoomIndex.LoadString(
			IDS_STRING533);
		CString textGameRoomPoint;
		textGameRoomPoint.LoadString(
			IDS_STRING530);
		CString textItemAddCount;
		textItemAddCount.LoadString(
			IDS_STRING531);
		CString textItemNextTime;
		textItemNextTime.LoadString(
			IDS_STRING532);

		const CString title[] = {
			textGameRoomIndex,
			textGameRoomPoint,
			textItemAddCount,
			textItemNextTime,
			_T(""),	// it must end with empty string 
		};

		mApplication.CreateGrid(
			this,
			&mGameRoomGridCtrl,
			IDC_USER_GAMEROOM_GRID,
			gridRect,
			title,
			FALSE);

		mGameRoomGridCtrl.SetColumnWidth( 0, titleColumn );
		mGameRoomGridCtrl.SetColumnWidth( 1, valueColumn );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CUserDialog::Request( DWORD serverIndex, const CString& userName, DWORD userIndex )
{
	mServerIndex	= serverIndex;
	mUserName		= userName;
	mUserIndex		= userIndex;
	
	{
		CString textTitleFormat;
		textTitleFormat.LoadString( IDS_STRING212 );

		CString	text;
		text.Format( textTitleFormat, mApplication.GetServerName( serverIndex ), userName, userIndex );

		SetWindowText( text );
	}

	mLivePlayerListCtrl.DeleteAllItems();
	mDeletedPlayerListCtrl.DeleteAllItems();

	// 응답이 있을때까지 컨트롤을 잠근다
	{
		mLivePlayerListCtrl.EnableWindow( FALSE );
		mDeletedPlayerListCtrl.EnableWindow( FALSE );
		mLevelCombo.EnableWindow( FALSE );
	}

	{
		MSG_NAME message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_USER;
		message.Protocol	= MP_RM_USER_GET_DATA_SYN;
		message.dwObjectID	= userIndex;
		StringCopySafe( message.Name, CW2AEX< MAX_PATH >( userName ), sizeof( message.Name ) );
		
		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}


void CUserDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
	case MP_RM_USER_GET_DATA_ACK:
		{
			memcpy( &mReceivedData, message, sizeof( mReceivedData ) );
			
			mJoinedDateEdit.SetWindowText( CA2WEX< sizeof( mReceivedData.mJoinedDate ) >( mReceivedData.mJoinedDate ) );
			mLogoutDateEdit.SetWindowText( CA2WEX< sizeof( mReceivedData.mLogedoutDate ) >( mReceivedData.mLogedoutDate ) );

			mLevelCombo.SetCurSel( mReceivedData.mLevel - 1 );
			mLevelCombo.EnableWindow( TRUE );

			{
				CString text;
				const MSG_RM_USER_DATA::Connection& data = mReceivedData.mConnection;

				if( *data.mIp )
				{
					mConnectStatic.SetWindowText( CString( data.mIp ) + _T( "connect" ) );

					CString textWhichAgentUsed;
					textWhichAgentUsed.LoadString( IDS_STRING213 );

					text.Format( textWhichAgentUsed, data.mAgent );
						
					mAgentStatic.SetWindowText( text );
				}
				else
				{
					CString textDisconnect;
					textDisconnect.LoadString( IDS_STRING214 );

					mConnectStatic.SetWindowText( textDisconnect );
					mAgentStatic.SetWindowText( _T( "" ) );
				}
			}


			{
				if(0 < strlen(mReceivedData.mEnpangName))
				{
					TCHAR text[MAX_PATH] = {0};
					_sntprintf(
						text,
						_countof(text),
						0 < strlen(mReceivedData.mId) ?  _T("%s (이전: %s)") : _T("%s%s"),
						CString(mReceivedData.mEnpangName),
						CString(mReceivedData.mId));
					midEdit.SetWindowText(
						text);
				}
				else
				{
					midEdit.SetWindowText(
						CString(mReceivedData.mId));
				}

				midEdit.SetFocus();
				midEdit.SetSel(
					0,
					midEdit.GetWindowTextLength());
			}

			{
				int start = 0;
				mNameStatic.SetWindowText( CString( mReceivedData.mName ).Tokenize( _T( " " ), start ) );
			}

			// 080725 LUJ, 상태 콤보 세팅
			mStateCombo.SetCurSel( 
				mStateCombo.FindString( 0, GetText( State( mReceivedData.mState ) ) ) );

			// 090618 ShinJS --- Auto Punish 상태 정보 셋팅
			mAutoPunishStateCombo.SetCurSel(
				mAutoPunishStateCombo.FindString( 0, GetText( PunishState( mReceivedData.mAutoPunishKind ) ) ) );
			mAutoPunishStartDate.SetWindowText( CA2WEX< sizeof( mReceivedData.mAutoPunishStartDate ) >( mReceivedData.mAutoPunishStartDate ) );
			mAutoPunishEndDate.SetWindowText( CA2WEX< sizeof( mReceivedData.mAutoPunishEndDate ) >( mReceivedData.mAutoPunishEndDate ) );

			CString textAutoPunishTime;
			textAutoPunishTime.Format( _T( "%d" ), mReceivedData.mAutoPunishTime );
			mAutoPunishTime.SetWindowText( textAutoPunishTime );

			break;
		}
	case MP_RM_USER_GET_PLAYER_LIST_ACK:
		{
			const MSG_RM_OPEN_ACK* m = ( MSG_RM_OPEN_ACK* )message;

			CString text;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_OPEN_ACK::Data& data = m->mData[ i ];

				CListCtrl&	listCtrl = ( 5 > data.mStandIndex ? mLivePlayerListCtrl : mDeletedPlayerListCtrl );
				const DWORD row	= listCtrl.GetItemCount();
				int	step = -1;
				bool isExistPlayer = false;

				for(int listRow = 0; listCtrl.GetItemCount() > listRow; ++listRow)
				{
					const CString textPlayerIndex = listCtrl.GetItemText(
						listRow,
						0);

					if(data.mPlayerIndex == _ttoi(textPlayerIndex))
					{
						isExistPlayer = true;
						break;
					}
				}

				if(isExistPlayer)
				{
					continue;
				}

				text.Format( _T( "%d" ), data.mPlayerIndex );
				listCtrl.InsertItem( row, text, ++step );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mPlayerName ) >( data.mPlayerName ) );
			}

			// 응답이 오기전까지 잠궜던 컨트롤을 활성화
			{
				mLivePlayerListCtrl.EnableWindow( TRUE );
				mDeletedPlayerListCtrl.EnableWindow( TRUE );
			}

			break;
		}
	case MP_RM_USER_GET_DATA_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );

			MessageBox( textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_USER_SET_DATA_ACK:
		{
			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_OK | MB_ICONINFORMATION );

			const MSG_DWORD5* m = ( MSG_DWORD5* )message;

			mReceivedData.mLevel = m->dwData2;
			mReceivedData.mState = m->dwData3;
			// 090618 ShinJS --- Auto Punish 정보 추가
			mReceivedData.mAutoPunishKind = m->dwData4;
			mReceivedData.mAutoPunishTime = m->dwData5;
			break;
		}
	case MP_RM_USER_DELETE_NACK:
	case MP_RM_USER_SET_DATA_NACK:
		{
			CString textUpdateWasFailed;
			textUpdateWasFailed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasFailed, _T( "" ), MB_ICONERROR | MB_OK );

			Parse( mServerIndex, &mReceivedData );
			break;
		}
	case MP_RM_USER_DELETE_ACK:
		{
			const MSGBASE* m = ( MSGBASE* )message;

			// 플레이어를 활성 리스트에서 삭제 플레이어 리스트로 이동시킨다
			{
				POSITION	position	= mLivePlayerListCtrl.GetFirstSelectedItemPosition();
				const int	targetRow	= mLivePlayerListCtrl.GetNextSelectedItem( position );
				const int	destRow		= mDeletedPlayerListCtrl.GetItemCount();

				mDeletedPlayerListCtrl.InsertItem( destRow, mLivePlayerListCtrl.GetItemText( targetRow, 0 ), 0 );
				mDeletedPlayerListCtrl.SetItemText( destRow, 1, mLivePlayerListCtrl.GetItemText( targetRow, 1 ) );
				
				mLivePlayerListCtrl.DeleteItem( targetRow );
			}

			break;
		}
	case MP_RM_USER_DELETE_NACK_BY_FAMILY:
		{
			CString textMemberCanDeleteAfterSecedingFromFamily;
			textMemberCanDeleteAfterSecedingFromFamily.LoadString( IDS_STRING215 );

			MessageBox( textMemberCanDeleteAfterSecedingFromFamily, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_USER_DELETE_NACK_BY_PARTY:
		{
			CString textMemberCanDeleteAfterSecedingFromParty;
			textMemberCanDeleteAfterSecedingFromParty.LoadString( IDS_STRING216 );

			MessageBox( textMemberCanDeleteAfterSecedingFromParty, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_USER_DELETE_NACK_BY_GUILD:
		{
			CString textMemberCanDeleteAfterSecedingFromGuild;
			textMemberCanDeleteAfterSecedingFromGuild.LoadString( IDS_STRING217 );

			MessageBox( textMemberCanDeleteAfterSecedingFromGuild, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_USER_CREATE_ACK:
		{
			Request( mServerIndex, mUserName, mUserIndex );

			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_USER_CREATE_NACK:
	case MP_RM_USER_RESTORE_NACK:
		{
			CString textUpdateWasFailed;
			textUpdateWasFailed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateWasFailed, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_USER_CREATE_NACK_BY_DUPLICATED_NAME:
		{
			CString textPlayerWhoHaveSameNameExist;
			textPlayerWhoHaveSameNameExist.LoadString( IDS_STRING218 );

			MessageBox( textPlayerWhoHaveSameNameExist, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_USER_CREATE_NACK_BY_GUILD:
		{
			CString textNameEqualToSomeGuild;
			textNameEqualToSomeGuild.LoadString( IDS_STRING219 );

			MessageBox( textNameEqualToSomeGuild, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_USER_CREATE_NACK_BY_FULL:
		{
			CString textYouCanNotCreatePlayerAnyMore;
			textYouCanNotCreatePlayerAnyMore.LoadString( IDS_STRING220 );

			MessageBox( textYouCanNotCreatePlayerAnyMore, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_USER_RESTORE_NACK_BY_FULL:
		{
			CString textMoreSpaceNeedToRestorePlayer;
			textMoreSpaceNeedToRestorePlayer.LoadString( IDS_STRING221 );

			MessageBox( textMoreSpaceNeedToRestorePlayer, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_USER_RESTORE_NACK_BY_NOT_EXIST:
		{
			CString textPlayerIsNotExisted;
			textPlayerIsNotExisted.LoadString( IDS_STRING222 );

			MessageBox( textPlayerIsNotExisted, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_USER_RESTORE_ACK:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			{
				CUserDialog* dialog = mApplication.GetUserDialog();
				ASSERT( dialog );
				
				dialog->RestorePlayer( m->dwObjectID, CString( CA2WEX< sizeof( m->Name ) >( m->Name ) ) );
			}

			break;
		}
	case MP_RM_USER_RESTORE_NACK_BY_DUPLICATED_NAME:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			CString textWarningAboutDuplicatedName;
			textWarningAboutDuplicatedName.LoadString( IDS_STRING223 );

			CString text;
			text.Format( textWarningAboutDuplicatedName, CA2WEX< sizeof( m->Name ) >( m->Name ) );

			MessageBox( text, _T( "" ) );
			break;
		}
	case MP_RM_USER_RESTORE_NACK_BY_GUILD:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			CString textWarningAboutDuplicatedName;
			textWarningAboutDuplicatedName.LoadString( IDS_STRING387 );

			CString text;
			text.Format( textWarningAboutDuplicatedName, CA2WEX< sizeof( m->Name ) >( m->Name ) );

			MessageBox( text, _T( "" ) );
			break;
		}
	case MP_RM_USER_RESTORE_NACK_BY_FAMILY:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			CString textWarningAboutDuplicatedName;
			textWarningAboutDuplicatedName.LoadString( IDS_STRING386 );

			CString text;
			text.Format( textWarningAboutDuplicatedName, CA2WEX< sizeof( m->Name ) >( m->Name ) );

			MessageBox( text, _T( "" ) );
			break;
		}
	case MP_RM_USER_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	case MP_RM_USER_UPDATE_PUNISH_DATA:
		{
			const MSG_RM_PUNISH_DATA* m = (MSG_RM_PUNISH_DATA*)message;
			mAutoPunishStartDate.SetWindowText( CA2WEX< sizeof( m->mAutoPunishStartDate ) >( m->mAutoPunishStartDate ) );
			mAutoPunishEndDate.SetWindowText( CA2WEX< sizeof( m->mAutoPunishEndDate ) >( m->mAutoPunishEndDate ) );
			break;
		}
	case MP_RM_USER_GAMEROOM_GET_ACK:
		{
			const MSG_NAME_DWORD3* const receivedMessage = (MSG_NAME_DWORD3*)message;
			const DWORD gameRoomIndex = receivedMessage->dwData1;
			const DWORD gameRoomPoint = receivedMessage->dwData2;
			const DWORD itemAddCount = receivedMessage->dwData3;
			const CString itemNextTime = CString(receivedMessage->Name);

			GV_ITEM cell = { 0 };
			cell.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
			cell.col = 1;
			cell.mask = GVIF_TEXT;
            
			CclientApp::SetCellValue(
				gameRoomIndex,
				mGameRoomGridCtrl,
				cell,
				GVIS_READONLY);
			++cell.row;

			CclientApp::SetCellValue(
				gameRoomPoint,
				mGameRoomGridCtrl,
				cell,
				GVIS_MODIFIED);
			++cell.row;

			cell.strText.Format(
				_T("%d개"),
				itemAddCount);

			CclientApp::SetCellValue(
				cell.strText,
				mGameRoomGridCtrl,
				cell,
				GVIS_READONLY);
			++cell.row;

			CclientApp::SetCellValue(
				itemNextTime,
				mGameRoomGridCtrl,
				cell,
				GVIS_READONLY);
			++cell.row;
			break;
		}
	case MP_RM_USER_GAMEROOM_SET_ACK:
		{
			break;
		}
	default:
		{
			ASSERT( 0 && "It is no defined protocol" );
			break;
		}
	}
}


void CUserDialog::OnBnClickedUserUpdateButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	const DWORD level = mLevelCombo.GetCurSel() + 1;
	State		state = StateNone;
	{
		CString text;
		mStateCombo.GetLBText( mStateCombo.GetCurSel(), text );
		
		// 080725 LUJ, 문자열과 매칭시켜 값을 얻는다. 아래 상수 값은 MEMBERDB.up_GameLoginAdult_FREE에서 인용
		if( GetText( StateLive ) == text )
		{
			state = StateLive;
		}
		else if( GetText( StateBlock ) == text )
		{
			state = StateBlock;
		}
		else if( GetText( StateSecede ) == text )
		{
			state = StateSecede;
		}
	}

	// 090618 ShinJS --- Auto Punish 상태추가
	PunishState punishState;
	{
		CString text;
		mAutoPunishStateCombo.GetLBText( mAutoPunishStateCombo.GetCurSel(), text );
				
		if( GetText( StateAutoPunishLogin ) == text )
		{
			punishState = StateAutoPunishLogin;
		}
		else if( GetText( StateAutoPunishUseNote ) == text )
		{
			punishState = StateAutoPunishUseNote;
		}
		else if( GetText( StateAutoPunishBan ) == text )
		{
			punishState = StateAutoPunishBan;
		}
		else if( GetText( StateAutoPunishNone ) == text )
		{
			punishState = StateAutoPunishNone;
		}
	}
	CString textPunishTime;
	mAutoPunishTime.GetWindowText( textPunishTime );
	const DWORD punishTime = DWORD( _ttoi( textPunishTime ) );

	MSG_DWORD5 message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_USER;
	message.Protocol	= MP_RM_USER_SET_DATA_SYN;
	message.dwObjectID	= mUserIndex;
	message.dwData1		= mReceivedData.mIndex;
	message.dwData2		= level;
	message.dwData3		= state;
	// 090618 ShinJS --- Auto Punish 상태추가
	// 변화가 없는 경우
	if( mReceivedData.mAutoPunishKind == punishState && 
		mReceivedData.mAutoPunishTime == punishTime )
	{
		message.dwData4 = StateAutoPunishNoChange;
	}
	else
	{
		message.dwData4 = punishState;
		message.dwData5 = punishTime;
	}
		

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CUserDialog::OnBnClickedUserCreateButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( MAX_CHARACTER_NUM <= mLivePlayerListCtrl.GetItemCount() )
	{
		CString textItIsMaximumForSpaceOfPlayer;
		textItIsMaximumForSpaceOfPlayer.LoadString( IDS_STRING224 );

		MessageBox( textItIsMaximumForSpaceOfPlayer, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CUserCreateDialog dialog( mApplication );

	if( IDOK != dialog.DoModal() )
	{
		return;
	}
    
	{
		CHARACTERMAKEINFO message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_USER;
		message.Protocol	= MP_RM_USER_CREATE_SYN;
		message.UserID		= mUserIndex;
		message.FaceType	= dialog.GetFace();
		message.HairType	= dialog.GetHair();
		message.JobType		= dialog.GetJob();
		message.SexType		= dialog.GetGender();
		message.RaceType	= dialog.GetRace();

		// 080401 LUJ, 포인터를 직접 함수에 넣으면 복사가 되지 않는 경우가 있다
		StringCopySafe( message.Name, CW2AEX< MAX_PATH >( dialog.GetName() ), sizeof( message.Name ) );

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}


void CUserDialog::OnBnClickedUserRestoreButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	const int maxPlayerSpaceSize = 5;

	if( maxPlayerSpaceSize < mLivePlayerListCtrl.GetItemCount() )
	{
		CString textItIsMaximumForSpaceOfPlayer;
		textItIsMaximumForSpaceOfPlayer.LoadString( IDS_STRING224 );

		MessageBox( textItIsMaximumForSpaceOfPlayer, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	POSITION	position	= mDeletedPlayerListCtrl.GetFirstSelectedItemPosition();
	const int	row			= mDeletedPlayerListCtrl.GetNextSelectedItem( position );

	const CString playerName	= mDeletedPlayerListCtrl.GetItemText( row, 1 );
	const CString playerIndex	= mDeletedPlayerListCtrl.GetItemText( row, 0 );

	if( playerName.IsEmpty() )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CUserRestoreDialog dialog( mApplication );

	if(	IDOK != dialog.DoModal( playerName ) )
	{
		return;
	}

	MSG_NAME message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_USER;
		message.Protocol	= MP_RM_USER_RESTORE_SYN;
		message.dwObjectID	= _ttoi( playerIndex );

		StringCopySafe( message.Name, CW2AEX< MAX_PATH >( dialog.GetName() ), sizeof( message.Name ) );
	}

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CUserDialog::OnBnClickedUserDeleteButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 선택된 캐릭터가 있어야함

	POSITION	position	= mLivePlayerListCtrl.GetFirstSelectedItemPosition();
	const int	row			= mLivePlayerListCtrl.GetNextSelectedItem( position );

	const CString playerName	= mLivePlayerListCtrl.GetItemText( row, 1 );
	const CString playerIndex	= mLivePlayerListCtrl.GetItemText( row, 0 );

	if( playerIndex.IsEmpty() )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	{
		CString textWarningAboutDeletePlayer;
		textWarningAboutDeletePlayer.LoadString( IDS_STRING225 );

		CString text;
		text.Format(
			textWarningAboutDeletePlayer,
			playerName );

		if( IDNO == MessageBox( text, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
		{
			return;
		}
	}

	{
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		
		message.Category	= MP_RM_USER;
		message.Protocol	= MP_RM_USER_DELETE_SYN;
		message.dwObjectID	= _ttoi( playerIndex );

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}


void CUserDialog::RestorePlayer( DWORD playerIndex, const CString& name )
{
	for( int row = 0; row < mDeletedPlayerListCtrl.GetItemCount(); ++row )
	{
		if( playerIndex != _ttoi( mDeletedPlayerListCtrl.GetItemText( row, 0 ) ) )
		{
			continue;
		}
		
		const int	destRow		= mLivePlayerListCtrl.GetItemCount();

		mLivePlayerListCtrl.InsertItem( destRow, mDeletedPlayerListCtrl.GetItemText( row, 0 ), 0 );
		mLivePlayerListCtrl.SetItemText( destRow, 1, name );

		mDeletedPlayerListCtrl.DeleteItem( row );
		break;
	}
}


void CUserDialog::OnBnClickedUserItemShopLogButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textCashItem;
		textCashItem.LoadString( IDS_STRING271 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textCashItem,
			textLog,
			mUserName,
			mUserIndex );

		dialog->DoModal(
			UINT_MAX,
			CItemShopLogCommand( mApplication, title, mUserIndex ) );
	}
}


// 080725 LUJ, 상태에 관한 문자열 반환
CString CUserDialog::GetText( CUserDialog::State state ) const
{
	switch( state )
	{
	case StateNone:
		{
			CString textNone;
			textNone.LoadString( IDS_STRING203 );

			return textNone;
		}
	case StateLive:
		{
			CString textLive;
			textLive.LoadString( IDS_STRING246 );

			return textLive;
		}
	case StateBlock:
		{
			CString textBlock;
			textBlock.LoadString( IDS_STRING303 );

			return textBlock;
		}
	case StateSecede:
		{
			CString textSecede;
			textSecede.LoadString( IDS_STRING244 );
			
			return textSecede;
		}
	}

	return _T( "?" );
}

// 090618 ShinJS --- Punish 상태에 관한 문자열 반환
CString CUserDialog::GetText( PunishState state) const
{
	switch( state )
	{
	case StateAutoPunishNone:
		{
			CString textSecede;
			textSecede.LoadString( IDS_STRING203 );
			
			return textSecede;
		}
	case StateAutoPunishUseNote:
		{
			CString textSecede;
			textSecede.LoadString( IDS_STRING448 );
			
			return textSecede;
		}
	case StateAutoPunishLogin:
		{
			CString textSecede;
			textSecede.LoadString( IDS_STRING303 );
			
			return textSecede;
		}
	case StateAutoPunishBan:
		{
			CString textSecede;
			textSecede.LoadString( IDS_STRING447 );
			
			return textSecede;
		}
	}

	return _T( "?" );
}

// 081202 LUJ, 삭제된 플레이어의 상세 정보를 표시한다
void CUserDialog::OnBnClickedUserRemovePlayerButton()
{
	POSITION		position	= mDeletedPlayerListCtrl.GetFirstSelectedItemPosition();
	const DWORD		row			= mDeletedPlayerListCtrl.GetNextSelectedItem( position );
	const CString	playerIndex	= mDeletedPlayerListCtrl.GetItemText( row, 0 );

	if( playerIndex.IsEmpty() )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	const CString	playerName	= mDeletedPlayerListCtrl.GetItemText( row, 1 );
	CPlayerDialog*	dialog		= mApplication.GetPlayerDialog();
	
	if( dialog )
	{
		dialog->Request( mServerIndex, playerName, _ttoi( playerIndex ) );
	}
}

void CUserDialog::OnBnClickedUserGameroomLogButton()
{
	if(CLogDialog* dialog = mApplication.GetLogDialog())
	{
		CString textCashItem;
		textCashItem.LoadString(
			IDS_STRING530);
		CString textLog;
		textLog.LoadString(
			IDS_STRING10);

		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textCashItem,
			textLog,
			mUserName,
			mUserIndex );

		CGameRoomPointLogCommand::Configuration configuration = {0};
		configuration.mUserIndex = mUserIndex;

		dialog->DoModal(
			UINT_MAX,
			CGameRoomPointLogCommand(mApplication, title, configuration));
	}	
}

void CUserDialog::OnBnClickedUserGameroomUpdateButton()
{
	CString gameRoomPoint = mGameRoomGridCtrl.GetItemText(
		1,
		1);

	MSG_DWORD message;
	message.Category = MP_RM_USER;
	message.Protocol = MP_RM_USER_GAMEROOM_SET_SYN;
	message.dwObjectID = mUserIndex;
	message.dwData = _ttoi(gameRoomPoint);

	mApplication.Send(
		mServerIndex,
		message,
		sizeof(message));
}

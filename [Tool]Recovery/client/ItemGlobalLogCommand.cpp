#include "StdAfx.h"
#include "ItemGlobalLogCommand.h"
#include "client.h"


CItemGlobalLogCommand::CItemGlobalLogCommand( CclientApp& application, const TCHAR* title, const CItemGlobalLogCommand::Configuration& configuration ) :
CCommand( application, title ),
mConfiguration( configuration )
{}


void CItemGlobalLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	textHead.LoadString( IDS_STRING189 );

	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}


void CItemGlobalLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect	rect;
	int		step = -1;

	listCtrl.GetClientRect( rect );

	listCtrl.InsertColumn( ++step,  _T( "key 1" ), LVCFMT_LEFT, 0 );
	listCtrl.InsertColumn( ++step,  _T( "key 2" ), LVCFMT_LEFT, 0 );

	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textType;
	textType.LoadString( IDS_STRING4 );
	listCtrl.InsertColumn( ++step, textType, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

	CString textItemDbIndex;
	textItemDbIndex.LoadString( IDS_STRING13 );
	listCtrl.InsertColumn( ++step, textItemDbIndex, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

	CString textItemIndex;
	textItemIndex.LoadString( IDS_STRING22 );
	listCtrl.InsertColumn( ++step, textItemIndex, LVCFMT_RIGHT, int( rect.Width() * 0.0f ) );

	CString textName;
	textName.LoadString( IDS_STRING20 );
	listCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.3f ) );

	CString textQuantity;
	textQuantity.LoadString( IDS_STRING42 );
	listCtrl.InsertColumn( ++step, textQuantity, LVCFMT_RIGHT, int( rect.Width() * 0.1f ) );

	CString textSendPosition;
	textSendPosition.LoadString( IDS_STRING56 );
	listCtrl.InsertColumn( ++step, textSendPosition, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

	CString textReceivedPosition;
	textReceivedPosition.LoadString( IDS_STRING57 );
	listCtrl.InsertColumn( ++step, textReceivedPosition, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

	CString textSentPlayer;
	textSentPlayer.LoadString( IDS_STRING118 );
	listCtrl.InsertColumn( ++step, textSentPlayer, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

	CString textSentPlayerIndex;
	textSentPlayerIndex.LoadString( IDS_STRING119 );
	listCtrl.InsertColumn( ++step, textSentPlayerIndex, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

	CString textSentPlayerMoney;
	textSentPlayerMoney.LoadString( IDS_STRING120 );
	listCtrl.InsertColumn( ++step, textSentPlayerMoney, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

	CString textReceivedPlayer;
	textReceivedPlayer.LoadString( IDS_STRING121 );
	listCtrl.InsertColumn( ++step, textReceivedPlayer, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

	CString textReceivedPlayerIndex;
	textReceivedPlayerIndex.LoadString( IDS_STRING122 );
	listCtrl.InsertColumn( ++step, textReceivedPlayerIndex, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

	CString textReceivedPlayerMoney;
	textReceivedPlayerMoney.LoadString( IDS_STRING123 );
	listCtrl.InsertColumn( ++step, textReceivedPlayerMoney, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

	CString textVariatedMoney;
	textVariatedMoney.LoadString( IDS_STRING124 );
	listCtrl.InsertColumn( ++step, textVariatedMoney, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

	CString textExperience;
	textExperience.LoadString( IDS_STRING9 );
	listCtrl.InsertColumn( ++step, textExperience, LVCFMT_RIGHT, int( rect.Width() * 0.1f ) );
}


void CItemGlobalLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_ITEM_GLOBAL_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_ITEM_GLOBAL_LOG;
	message.Protocol		= MP_RM_ITEM_GLOBAL_LOG_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	message.dwObjectID		= mConfiguration.mPlayerIndex;
	message.mItemIndex		= mConfiguration.mItemIndex;
	message.mItemDbIndex	= mConfiguration.mItemDbIndex;
	message.mLogType		= mConfiguration.mLogType;
	
	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),	sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),		sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CItemGlobalLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_ITEM_GLOBAL_LOG;
	message.Protocol	= MP_RM_ITEM_GLOBAL_LOG_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CItemGlobalLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_ITEM_GLOBAL_LOG_ACK:
		{
			const MSG_RM_ITEM_GLOBAL_LOG* m = ( MSG_RM_ITEM_GLOBAL_LOG* )message;

			if( m->mRequestTick != mTickCount )
			{
				findButton.EnableWindow( TRUE );
				stopButton.EnableWindow( FALSE );

				CString textSucceed;
				textSucceed.LoadString( IDS_STRING28 );
				MessageBox( 0, textSucceed, _T( "" ), MB_OK );
				break;
			}
			else if( !	m->mSize &&
					 !	listCtrl.GetItemCount() )
			{
				CString textThereIsNoResult;
				textThereIsNoResult.LoadString( IDS_STRING1 );
				MessageBox( 0, textThereIsNoResult, _T( "" ), MB_OK );
				break;
			}

			class
			{
			public:
				LONGLONG operator() ( DWORD key1, DWORD key2 ) const
				{
					LONGLONG key = 0;
					memcpy(	( DWORD* )&key + 0, &( key1 ),	sizeof( key1 ) );
					memcpy(	( DWORD* )&key + 1, &( key2 ),	sizeof( key2 ) );

					return key;
				}
			}
			MakeKey;

			typedef stdext::hash_set< LONGLONG >	IndexSet;
			IndexSet								indexSet;
			{
				// 080716 LUJ, 4바이트 값 2개를 8바이트 자료형에 복사하여 고유 키를 만든다
				
				for( int row = 0; row < listCtrl.GetItemCount(); ++row )
				{
					const DWORD key1 = _ttoi( listCtrl.GetItemText( row, 0 ) );
					const DWORD key2 = _ttoi( listCtrl.GetItemText( row, 1 ) );

					indexSet.insert( MakeKey( key1, key2 ) );
				}
			}

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_ITEM_GLOBAL_LOG::Log&	data	= m->mLog[ i ];

				if( indexSet.end() != indexSet.find( MakeKey( data.mKey, data.mLogIndex ) ) )
				{
					continue;
				}

				const DWORD	row		= listCtrl.GetItemCount();
				int			step	= 0;

				CString text;
				text.Format( _T( "%d" ), data.mKey );
				listCtrl.InsertItem( row, text, 0 );
				text.Format( _T( "%d" ), data.mLogIndex );
				listCtrl.SetItemText( row, ++step, text );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );
				listCtrl.SetItemText( row, ++step, mApplication.GetText( data.mType ) );

				{
					const MSG_RM_ITEM_GLOBAL_LOG::Item& item = data.mItem;

					text.Format( _T( "%d" ), item.mDbIndex );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), item.mIndex );
					listCtrl.SetItemText( row, ++step, text );

					listCtrl.SetItemText( row, ++step, mApplication.GetItemName( item.mIndex ) );

					text.Format( _T( "%d" ), item.mQuantity );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), item.mFromPosition );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), item.mToPosition );
					listCtrl.SetItemText( row, ++step, text );
				}

				{
					const MSG_RM_ITEM_GLOBAL_LOG::Player& player = data.mFromPlayer;

					listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( player.mName ) >( player.mName ) );

					text.Format( _T( "%d" ), player.mIndex );
					listCtrl.SetItemText( row, ++step, text );
					listCtrl.SetItemText(
						row,
						++step,
						mApplication.ConvertToText(player.mMoney));
				}

				{
					const MSG_RM_ITEM_GLOBAL_LOG::Player& player = data.mToPlayer;

					listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( player.mName ) >( player.mName ) );

					text.Format( _T( "%d" ), player.mIndex );
					listCtrl.SetItemText( row, ++step, text );
					listCtrl.SetItemText(
						row,
						++step,
						mApplication.ConvertToText(player.mMoney));
				}

				listCtrl.SetItemText(
					row,
					++step,
					mApplication.ConvertToText(data.mMoneyVariation));

				text.Format( _T( "%d" ), data.mExperience );
				listCtrl.SetItemText( row, ++step, text );
			}

			{
				CString text;
				text.Format( _T( "%d/%d" ), listCtrl.GetItemCount(), MAX_LOG_SIZE );
				resultStatic.SetWindowText( text );

				progressCtrl.SetPos( progressCtrl.GetPos() + int( m->mSize ) );
			}

			// 080716 LUJ, 전역 아이템 로그 검색은 개수를 모르므로, 최대 로그 개수보다 적게 올 경우 끝으로 간주한다
			//				최대 로그 개수보다 MAX_ROW_NUM만큼 적어도 완료로 간주한다. 중복되는 레코드를 빼므로 대부분 MAX_LOG_SIZE를 채우지 못한다
			if( MAX_ROW_NUM > m->mSize ||
				MAX_LOG_SIZE - MAX_ROW_NUM < listCtrl.GetItemCount() )
			{
				findButton.EnableWindow( TRUE );
				stopButton.EnableWindow( FALSE );

				CString textSucceed;
				textSucceed.LoadString( IDS_STRING28 );
				MessageBox( 0, textSucceed, _T( "" ), MB_OK );
			}

			break;
		}
	case MP_RM_ITEM_GLOBAL_LOG_STOP_ACK:
		{
			findButton.EnableWindow( TRUE );
			stopButton.EnableWindow( FALSE );

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString( IDS_STRING2 );

			MessageBox( 0, textSearchWasStopped, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_ITEM_GLOBAL_LOG_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( 0, textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	case MP_RM_ITEM_GLOBAL_LOG_READY_ACK:
		{
			listCtrl.DeleteAllItems();

			findButton.EnableWindow( FALSE );
			stopButton.EnableWindow( TRUE );

			progressCtrl.SetRange32( 0, MAX_LOG_SIZE );
			progressCtrl.SetPos( 0 );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}
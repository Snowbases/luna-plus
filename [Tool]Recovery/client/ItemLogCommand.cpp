#include "stdafx.h"
#include "ItemLogCommand.h"
#include "client.h"


CItemLogCommand::CItemLogCommand( CclientApp& application, const TCHAR* title, const Configuration& configuration ) :
CCommand( application, title ),
mConfiguration( configuration )
{}


void CItemLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
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


void CItemLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect	rect;
	int		step = -1;

	listCtrl.GetClientRect( rect );

	listCtrl.InsertColumn( ++step,  _T( "log index" ), LVCFMT_LEFT, 0 );

	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

	CString textType;
	textType.LoadString( IDS_STRING4 );
	listCtrl.InsertColumn( ++step, textType, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

	CString textItemDbIndex;
	textItemDbIndex.LoadString( IDS_STRING13 );
	listCtrl.InsertColumn( ++step, textItemDbIndex, LVCFMT_RIGHT, int( rect.Width() * 0.1f ) );

	CString textItemIndex;
	textItemIndex.LoadString( IDS_STRING22 );
	listCtrl.InsertColumn( ++step, textItemIndex, LVCFMT_RIGHT, int( rect.Width() * 0.0f ) );

	CString textName;
	textName.LoadString( IDS_STRING20 );
	listCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

	CString textQuantity;
	textQuantity.LoadString( IDS_STRING42 );
	listCtrl.InsertColumn( ++step, textQuantity, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

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
	listCtrl.InsertColumn( ++step, textExperience, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );
}


void CItemLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_ITEM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_ITEM_LOG;
	message.Protocol		= MP_RM_ITEM_LOG_GET_SIZE_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	message.dwObjectID		= 0;
	message.mItemIndex		= mConfiguration.mItemIndex;
	message.mItemDbIndex	= mConfiguration.mItemDbIndex;
	message.mPlayerIndex	= mConfiguration.mPlayerIndex;

	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),	sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),		sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CItemLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_ITEM_LOG;
	message.Protocol	= MP_RM_ITEM_LOG_GET_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CItemLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_ITEM_LOG_GET_ACK:
		{
			// 080401 LUJ, 이전 로그에 대한 인덱스 세트를 만들어 중복되지 않도록 체크한다
			std::set< DWORD > indexSet;
			{
				for( int row = 0; row < listCtrl.GetItemCount(); ++row )
				{
					indexSet.insert( _ttoi( listCtrl.GetItemText( row, 0 ) ) );
				}
			}

			const MSG_RM_ITEM_LOG* m = ( MSG_RM_ITEM_LOG* )message;

			if( m->mRequestTick != mTickCount )
			{
				return;
			}

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_ITEM_LOG::Log& data	= m->mLog[ i ];
				const DWORD					row		= listCtrl.GetItemCount();
				int							step	= 0;

				if( indexSet.end() != indexSet.find( data.mLogIndex ) )
				{
					continue;
				}

				CString text;
				text.Format( _T( "%d" ), data.mLogIndex );
				listCtrl.InsertItem( row, text, 0 );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );
				listCtrl.SetItemText( row, ++step, mApplication.GetText( data.mType ) );

				{
					const MSG_RM_ITEM_LOG::Item& item = data.mItem;

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
					const MSG_RM_ITEM_LOG::Player& player = data.mFromPlayer;

					listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( player.mName ) >( player.mName ) );

					text.Format( _T( "%d" ), player.mIndex );
					listCtrl.SetItemText( row, ++step, text );
					listCtrl.SetItemText( row, ++step, 
						mApplication.ConvertToText(player.mMoney));
				}

				{
					const MSG_RM_ITEM_LOG::Player& player = data.mToPlayer;

					listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( player.mName ) >( player.mName ) );

					text.Format( _T( "%d" ), player.mIndex );
					listCtrl.SetItemText( row, ++step, text );

					listCtrl.SetItemText( row, ++step, 
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
				int minRange;
				int maxRange;
				progressCtrl.GetRange( minRange, maxRange );
				progressCtrl.SetPos( progressCtrl.GetPos() + int( m->mSize ) );

				CString text;
				text.Format( _T( "%d/%d" ), progressCtrl.GetPos(), maxRange );
				resultStatic.SetWindowText( text );

				// 080523 LUJ, 버튼 활성화 체크가 정상적으로 되도록 수정함
				if( progressCtrl.GetPos() == maxRange )
				{
					findButton.EnableWindow( TRUE );
					stopButton.EnableWindow( FALSE );
				}
			}

			break;
		}
	case MP_RM_ITEM_LOG_GET_SIZE_ACK:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			const DWORD size = m->dwData;

			CString text;
			text.Format( _T( "0/%d" ), size );
			resultStatic.SetWindowText( text );

			progressCtrl.SetRange32( 0, size );
			progressCtrl.SetPos( 0 );

			listCtrl.DeleteAllItems();
			break;
		}
	case MP_RM_ITEM_LOG_GET_SIZE_NACK:
		{			
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );

			MessageBox( 0, textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_ITEM_LOG_GET_STOP_ACK:
		{
			findButton.EnableWindow( TRUE );
			stopButton.EnableWindow( FALSE );

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString( IDS_STRING2 );

			MessageBox( 0, textSearchWasStopped, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_ITEM_LOG_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( 0, textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

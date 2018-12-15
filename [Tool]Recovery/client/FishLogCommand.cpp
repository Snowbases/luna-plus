#include "stdafx.h"
#include "FishLogCommand.h"
#include "client.h"


CFishLogCommand::CFishLogCommand( CclientApp& application, const TCHAR* title, DWORD playerIndex ) :
CCommand( application, title ),
mPlayerIndex( playerIndex )
{}


void CFishLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	textHead.LoadString( IDS_STRING379 );

	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}


void CFishLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect	rect;
	int		step = -1;

	listCtrl.GetClientRect( rect );

	listCtrl.InsertColumn( ++step,  _T( "log index" ), LVCFMT_LEFT, 0 );

	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textPlayer;
	textDate.LoadString( IDS_STRING54 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

	CString textType;
	textType.LoadString( IDS_STRING4 );
	listCtrl.InsertColumn( ++step, textType, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

	CString textPoint;
	textPoint.LoadString( IDS_STRING34 );
	CString textVariant;
	textVariant.LoadString( IDS_STRING6 );
	listCtrl.InsertColumn( ++step, textPoint + _T( "" ) + textVariant, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );
	
	listCtrl.InsertColumn( ++step, textPoint, LVCFMT_RIGHT, int( rect.Width() * 0.2f ) );

	CString textBuy;
	textBuy.LoadString( IDS_STRING267 );
	CString textItem;
	textItem.LoadString( IDS_STRING189 );
	CString textIndex;
	textIndex.LoadString( IDS_STRING22 );
	listCtrl.InsertColumn( ++step, textBuy + _T( " " ) + textItem + _T( " " ) + textIndex, LVCFMT_RIGHT, 0 );

	CString textName;
	textName.LoadString( IDS_STRING20 );
	listCtrl.InsertColumn( ++step, textBuy + _T( " " ) + textItem + _T( " " ) + textName, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

	CString textUse;
	textUse.LoadString( IDS_STRING270 );
	listCtrl.InsertColumn( ++step, textUse + _T( " " ) + textItem + _T( " " ) + textIndex, LVCFMT_LEFT, 0 );

	listCtrl.InsertColumn( ++step, textUse + _T( " " ) + textItem + _T( " " ) + textName, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );	

	// 080808 LUJ, 경험치 추가
	CString textExperience;
	textExperience.LoadString( IDS_STRING9 );
	listCtrl.InsertColumn( ++step, textExperience, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );

	// 080808 LUJ, 레벨 추가
	CString textLevel;
	textLevel.LoadString( IDS_STRING5 );
	listCtrl.InsertColumn( ++step, textLevel, LVCFMT_RIGHT, int( rect.Width() * 0.1f ) );
}


void CFishLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_FISH_LOG;
	message.Protocol		= MP_RM_FISH_LOG_SIZE_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	message.dwObjectID		= mPlayerIndex;

	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),	sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),		sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CFishLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_FISH_LOG;
	message.Protocol	= MP_RM_FISH_LOG_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CFishLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_FISH_LOG_ACK:
		{
			const MSG_RM_FISH_LOG* m = ( MSG_RM_FISH_LOG* )message;

			// 080401 LUJ, 이전 로그에 대한 인덱스 세트를 만들어 중복되지 않도록 체크한다
			std::set< DWORD > indexSet;
			{
				for( int row = 0; row < listCtrl.GetItemCount(); ++row )
				{
					indexSet.insert( _ttoi( listCtrl.GetItemText( row, 0 ) ) );
				}
			}

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_FISH_LOG::Log& data	= m->mLog[ i ];
				const DWORD					row		= listCtrl.GetItemCount();
				int							step	= 0;

				if( indexSet.end() != indexSet.find( data.mIndex ) )
				{
					continue;
				}

				CString text;
				text.Format( _T( "%d" ), data.mIndex );
				listCtrl.InsertItem( row, text, 0 );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );

				text.Format( _T( "%d" ), data.mPlayerIndex );
				listCtrl.SetItemText( row, ++step, text );

				listCtrl.SetItemText( row, ++step, mApplication.GetText( data.mType ) );

				text.Format( _T( "%d" ), data.mVariant );
				listCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mPoint );
				listCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mBuyItemIndex );
				listCtrl.SetItemText( row, ++step, text );

				listCtrl.SetItemText( row, ++step, mApplication.GetItemName( data.mBuyItemIndex ) );

				text.Format( _T( "%d" ), data.mUseItemIndex );
				listCtrl.SetItemText( row, ++step, text );

				listCtrl.SetItemText( row, ++step, mApplication.GetItemName( data.mUseItemIndex ) );

				text.Format( _T( "%u" ), data.mExperience );
				listCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mLevel );
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
	case MP_RM_FISH_LOG_SIZE_ACK:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			const DWORD size = m->dwData;

			CString text;
			text.Format( _T( "0/%d" ), size );

			resultStatic.SetWindowText( text );
			progressCtrl.SetRange32( 0, size );
			progressCtrl.SetPos( 0 );
			findButton.EnableWindow( FALSE );
			stopButton.EnableWindow( TRUE );

			listCtrl.DeleteAllItems();
			break;
		}
	case MP_RM_FISH_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );

			MessageBox( 0, textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_FISH_LOG_STOP_ACK:
		{
			findButton.EnableWindow( TRUE );
			stopButton.EnableWindow( FALSE );

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString( IDS_STRING2 );

			MessageBox( 0, textSearchWasStopped, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_FISH_LOG_NACK_BY_AUTH:
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
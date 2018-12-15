#include "stdafx.h"
#include "OperatorLogCommand.h"
#include "client.h"


COperatorLogCommand::COperatorLogCommand( CclientApp& application, const TCHAR* title ) :
CCommand( application, title )
{}


void COperatorLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	// 080716 LUJ, 잘못된 리소스 연결 수정
	textHead.LoadString( IDS_STRING306 );

	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}


void COperatorLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect	rect;
	int		step = -1;

	listCtrl.GetClientRect( rect );

	listCtrl.InsertColumn( ++step,  _T( "log index" ), LVCFMT_LEFT, 0 );

	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textType;
	textType.LoadString( IDS_STRING4 );
	listCtrl.InsertColumn( ++step, textType, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textOperator;
	textOperator.LoadString( IDS_STRING20 );
	listCtrl.InsertColumn( ++step, textOperator, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	{
		CString textKey;
		textKey.LoadString( IDS_STRING151 );

		CString textValue;
		textValue.LoadString( IDS_STRING152 );

		listCtrl.InsertColumn( ++step, textKey		+ _T( "1" ), LVCFMT_LEFT, int( rect.Width() * 0.25f ) );
		listCtrl.InsertColumn( ++step, textValue	+ _T( "1" ), LVCFMT_RIGHT, int( rect.Width() * 0.25f ) );
		listCtrl.InsertColumn( ++step, textKey		+ _T( "2" ), LVCFMT_LEFT, int( rect.Width() * 0.25f ) );
		listCtrl.InsertColumn( ++step, textValue	+ _T( "2" ), LVCFMT_RIGHT, int( rect.Width() * 0.25f ) );
		listCtrl.InsertColumn( ++step, textKey		+ _T( "3" ), LVCFMT_LEFT, int( rect.Width() * 0.25f ) );
		listCtrl.InsertColumn( ++step, textValue	+ _T( "3" ), LVCFMT_RIGHT, int( rect.Width() * 0.25f ) );
	}
}


void COperatorLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_OPERATOR_LOG;
	message.Protocol		= MP_RM_OPERATOR_LOG_SIZE_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	message.dwObjectID		= 0;

	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),	sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),		sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void COperatorLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_OPERATOR_LOG;
	message.Protocol	= MP_RM_OPERATOR_LOG_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void COperatorLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_OPERATOR_LOG_ACK:
		{
			// 080401 LUJ, 이전 로그에 대한 인덱스 세트를 만들어 중복되지 않도록 체크한다
			std::set< DWORD > indexSet;
			{
				for( int row = 0; row < listCtrl.GetItemCount(); ++row )
				{
					indexSet.insert( _ttoi( listCtrl.GetItemText( row, 0 ) ) );
				}
			}

			const MSG_RM_OPERATOR_LOG* m = ( MSG_RM_OPERATOR_LOG* )message;

			if( mTickCount != m->mRequestTick )
			{
				break;
			}

			for( size_t i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_OPERATOR_LOG::Log& data = m->mLog[ i ];

				const DWORD row		= listCtrl.GetItemCount();
				int			step	= 0;

				if( indexSet.end() != indexSet.find( data.mIndex ) )
				{
					continue;
				}

				CString text;
				text.Format( _T( "%d" ), data.mIndex );
				listCtrl.InsertItem( row, text, 0 );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );
				listCtrl.SetItemText( row, ++step, mApplication.GetText( data.mType ) );
				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mOperator ) >( data.mOperator ) );
				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mKey1 ) >( data.mKey1 ) );
				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mValue1 ) >( data.mValue1 ) );
				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mKey2 ) >( data.mKey2 ) );
				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mValue2 ) >( data.mValue2 ) );
				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mKey3 ) >( data.mKey3 ) );
				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mValue3 ) >( data.mValue3 ) );
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
	case MP_RM_OPERATOR_LOG_SIZE_ACK:
		{
			const MSG_RM_LOG_SIZE* m = ( MSG_RM_LOG_SIZE* )message;

			if( mTickCount != m->mRequestTick )
			{
				break;
			}

			CString text;
			text.Format( _T( "0/%d" ), m->mSize );

			resultStatic.SetWindowText( text );
			progressCtrl.SetRange32( 0, int( m->mSize ) );
			progressCtrl.SetPos( 0 );
			findButton.EnableWindow( FALSE );
			stopButton.EnableWindow( TRUE );

			listCtrl.DeleteAllItems();
			break;
		}
	case MP_RM_OPERATOR_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );

			MessageBox( 0, textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_OPERATOR_LOG_NACK_BY_AUTH:
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

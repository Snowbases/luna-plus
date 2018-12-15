#include "stdafx.h"
#include "SiegeLogCommand.h"
#include "client.h"

CSiegeLogCommand::CSiegeLogCommand( CclientApp& application, const TCHAR* title ) :
CCommand( application, title )
{}


void CSiegeLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	textHead.LoadString( IDS_STRING164 );

	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}

void CSiegeLogCommand::Initialize( CListCtrl& listCtrl ) const
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

	CString textGuild;
	textGuild.LoadString( IDS_STRING164 );
	listCtrl.InsertColumn( ++step, textGuild, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textPlayer;
	textPlayer.LoadString( IDS_STRING54 );
	listCtrl.InsertColumn( ++step, textPlayer, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textMemo;
	textMemo.LoadString( IDS_STRING17 );
	listCtrl.InsertColumn( ++step, textMemo, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );
}


void CSiegeLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category		= MP_RM_SIEGE_LOG;
	message.Protocol		= MP_RM_SIEGE_LOG_SIZE_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),	sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),		sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CSiegeLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_SIEGE_LOG;
	message.Protocol	= MP_RM_SIEGE_LOG_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CSiegeLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_SIEGE_LOG_ACK:
		{
			// 080401 LUJ, 이전 로그에 대한 인덱스 세트를 만들어 중복되지 않도록 체크한다
			std::set< DWORD > indexSet;
			{
				for( int row = 0; row < listCtrl.GetItemCount(); ++row )
				{
					indexSet.insert( _ttoi( listCtrl.GetItemText( row, 0 ) ) );
				}
			}

			// 081027 LUJ, 메모를 판단해서 적절한 글로 변경한다. 원래 state안의 숫자는
			//			[Map]Server\SiegeWareMgr.h에 선언된 enum 상수이나, 헤더를 참조
			//			하면 컴파일에 문제가 있어 유연성이 없지만 분리했다
			class
			{
			public:
				CString operator()( const CString& text ) const
				{
					if( text == _T( "state:0" ) )
					{
						CString textWait;
						textWait.LoadString( IDS_STRING420 );

						return textWait;
					}
					else if( text == _T( "state:1" ) )
					{
						CString textStart;
						textStart.LoadString( IDS_STRING291 );

						return textStart;
					}
					else if( text == _T( "state:2" ) )
					{
						CString textReady;
						textReady.LoadString( AFX_IDS_IDLEMESSAGE );
						CString textInstall;
						textInstall.LoadString( IDS_STRING418 );
						CString textWaterseed;
						textWaterseed.LoadString( IDS_STRING417 );
					}
					else if( text == _T( "state:3" ) )
					{
						CString textEnd;
						textEnd.LoadString( IDS_STRING292 );

						return textEnd;
					}
					
					return text;
				}
			}
			Translate;

			const MSG_RM_SIEGE_LOG* m = ( MSG_RM_SIEGE_LOG* )message;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_SIEGE_LOG::Log&	data	= m->mLog[ i ];
				const DWORD						row		= listCtrl.GetItemCount();
				int								step	= 0;

				if( indexSet.end() != indexSet.find( data.mIndex ) )
				{
					continue;
				}

				CString text;
				text.Format( _T( "%d" ), data.mIndex );
				listCtrl.InsertItem( row, text, 0 );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );

				listCtrl.SetItemText( row, ++step, mApplication.GetText( data.mType ) );

				text.Format( _T( "%d" ), data.mGuildIndex );
				listCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mPlayerIndex );
				listCtrl.SetItemText( row, ++step, text );
				
				listCtrl.SetItemText( row, ++step, Translate( CString( data.mMemo ) ) );
			}

			{
				int minRange;
				int maxRange;
				progressCtrl.GetRange( minRange, maxRange );

				progressCtrl.SetPos( progressCtrl.GetPos() + m->mSize );

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
	case MP_RM_SIEGE_LOG_SIZE_ACK:
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
	case MP_RM_SIEGE_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );
			MessageBox( 0, textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_SIEGE_LOG_STOP_ACK:
		{
			findButton.EnableWindow( TRUE );
			stopButton.EnableWindow( FALSE );

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString( IDS_STRING2 );

			MessageBox( 0, textSearchWasStopped, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_SIEGE_LOG_NACK_BY_AUTH:
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
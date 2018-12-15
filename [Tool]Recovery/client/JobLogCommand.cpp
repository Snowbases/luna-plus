#include "stdafx.h"
#include "JobLogCommand.h"
#include "client.h"


CJobLogCommand::CJobLogCommand( CclientApp& application, const TCHAR* title, DWORD playerIndex ) :
CCommand( application, title ),
mPlayerIndex( playerIndex )
{}


void CJobLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	textHead.LoadString( IDS_STRING126 );

	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}


void CJobLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect	rect;
	int		step = -1;

	listCtrl.GetClientRect( rect );

	listCtrl.InsertColumn( ++step,  _T( "log index" ), LVCFMT_LEFT, 0 );

	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.32f ) );

	CString textJob;
	textJob.LoadString( IDS_STRING126 );
	listCtrl.InsertColumn( ++step, textJob, LVCFMT_LEFT, int( rect.Width() * 0.32f ) );

	CString textMemo;
	textMemo.LoadString( IDS_STRING17 );
	listCtrl.InsertColumn( ++step, textMemo, LVCFMT_LEFT, int( rect.Width() * 0.32f ) );
}


void CJobLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_JOB_LOG;
	message.Protocol		= MP_RM_JOB_LOG_SIZE_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	message.dwObjectID		= mPlayerIndex;

	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),	sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),		sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CJobLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_JOB_LOG;
	message.Protocol	= MP_RM_JOB_LOG_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CJobLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_JOB_LOG_GET_ACK:
		{
			const MSG_RM_JOB_LOG* m = ( MSG_RM_JOB_LOG* )message;

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
				const MSG_RM_JOB_LOG::Log&	data	= m->mLog[ i ];
				const DWORD					row		= listCtrl.GetItemCount();
				int							step	= 0;

				if( indexSet.end() != indexSet.find( data.mLogIndex ) )
				{
					continue;
				}

				const DWORD index = data.mClassIndex * 1000 + 100 + data.mJobGrade * 10 + data.mJobIndex;

				CString text;
				text.Format( _T( "%d" ), data.mLogIndex );
				listCtrl.InsertItem( row, text, 0 );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );

				listCtrl.SetItemText( row, ++step, mApplication.GetJobName( index ) );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mMemo ) >( data.mMemo ) );
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
	case MP_RM_JOB_LOG_SIZE_ACK:
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
	case MP_RM_JOB_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );
			MessageBox( 0, textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_JOB_LOG_STOP_ACK:
		{
			findButton.EnableWindow( TRUE );
			stopButton.EnableWindow( FALSE );

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString( IDS_STRING2 );

			MessageBox( 0, textSearchWasStopped, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_JOB_LOG_NACK_BY_AUTH:
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

#include "stdafx.h"
#include "NameLogCommand.h"
#include "client.h"

CNameLogCommand::CNameLogCommand( CclientApp& application, const TCHAR* title, const CNameLogCommand::Configuration& configuration ) :
CCommand( application, title ),
mConfiguration( configuration )
{}

void CNameLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	textHead.LoadString( IDS_STRING20 );

	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}


void CNameLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect	rect;
	int		step = -1;

	listCtrl.GetClientRect( rect );

	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.4f ) );
	CString textPlayer;
	textPlayer.LoadString( IDS_STRING54 );
	listCtrl.InsertColumn( ++step, textPlayer, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );
	CString textName;
	textName.LoadString( IDS_STRING20 );
	listCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.6f ) );
}


void CNameLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_RENAME_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_NAME_LOG;
	message.Protocol		= MP_RM_NAME_LOG_GET_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	message.dwObjectID		= mConfiguration.mPlayerIndex;
	strncpy( message.mKeyword,		CW2AEX< MAX_PATH >( mConfiguration.mKeyword ),	sizeof( message.mKeyword ) );
	mApplication.ConvertWildCardForDb( message.mKeyword );
	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),					sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),					sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CNameLogCommand::Stop( DWORD serverIndex ) const
{
	/*
	// 080304 LUJ, 이름 로그는 100개만 검색한다
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_NAME_LOG;
	message.Protocol	= MP_RM_NAME_LOG_GET_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
	*/
}


void CNameLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_NAME_LOG_GET_ACK:
		{
			typedef	std::map< DWORD, MSG_RM_NAME_LOG::Log >	LogMap;
			LogMap											logMap;	

			const MSG_RM_NAME_LOG* m = ( MSG_RM_NAME_LOG* )message;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_NAME_LOG::Log& log = m->mLog[ i ];

				logMap.insert( std::make_pair( log.mIndex, log ) );
			}

			listCtrl.DeleteAllItems();

			for(
				LogMap::const_iterator it = logMap.begin();
				logMap.end() != it;
				++it )
			{
				const MSG_RM_NAME_LOG::Log& log = it->second;

				const int row = listCtrl.GetItemCount();

				listCtrl.InsertItem( row, CA2WEX< sizeof( log.mDate ) >( log.mDate ), 0 );
				
				// 080402 LUJ, 매크로로 할 경우 함수 안에서 처리하지 않도록 주의하자. 실행 순서가 의존적일 경우 잘못 처리될 수 있다
				const CString previousName( CA2WEX< sizeof( log.mPreviousName ) >( log.mPreviousName ) );
				const CString name( CA2WEX< sizeof( log.mName ) >( log.mName ) );

				CString text;
				text.Format(
					_T( "%d" ),
					log.mIndex );
				listCtrl.SetItemText( row, 1, text );

				text.Format( _T( "%s > %s" ),
					previousName,
					name );
				listCtrl.SetItemText( row, 2, text );
			}

			if( logMap.empty() )
			{
				CString textThereIsNoResult;
				textThereIsNoResult.LoadString( IDS_STRING1 );

				MessageBox( 0, textThereIsNoResult, _T( "" ), MB_OK | MB_ICONINFORMATION );
			}

			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

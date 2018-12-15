#include "stdafx.h"
#include "FarmCropLogCommand.h"
#include "client.h"


CFarmCropLogCommand::CFarmCropLogCommand( CclientApp& application, const TCHAR* title, DWORD farmIndex ) :
CCommand( application, title ),
mFarmIndex( farmIndex )
{}


void CFarmCropLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	textHead.LoadString( IDS_STRING362 );

	CString textCrop;
	textCrop.LoadString( IDS_STRING14 );

	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + _T( " " ) + textCrop + _T( " " ) + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}


void CFarmCropLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect	rect;
	int		step = -1;

	listCtrl.GetClientRect( rect );

	listCtrl.InsertColumn( ++step,  _T( "log index" ), LVCFMT_LEFT, 0 );

	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textCategory;
	textCategory.LoadString( IDS_STRING4 );
	listCtrl.InsertColumn( ++step, textCategory, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textFarmIndex;
	textFarmIndex.LoadString( IDS_STRING11 );
	listCtrl.InsertColumn( ++step, textFarmIndex, LVCFMT_RIGHT, int( rect.Width() * 0.25f ) );

	CString textOwner;
	textOwner.LoadString( IDS_STRING24 );
	listCtrl.InsertColumn( ++step, textOwner, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textCropDbIndex;				
	textCropDbIndex.LoadString( IDS_STRING27 );
	listCtrl.InsertColumn( ++step, textCropDbIndex, LVCFMT_RIGHT, int( rect.Width() * 0.25f ) );

	CString textName;
	textName.LoadString( IDS_STRING20 );
	listCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textLife;
	textLife.LoadString( IDS_STRING15 );
	listCtrl.InsertColumn( ++step, textLife, LVCFMT_RIGHT, int( rect.Width() * 0.25f ) );

	CString textQualityy;
	textQualityy.LoadString( IDS_STRING16 );
	listCtrl.InsertColumn( ++step, textQualityy, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );

	CString textMemo;
	textMemo.LoadString( IDS_STRING17 );
	listCtrl.InsertColumn( ++step, textMemo, LVCFMT_LEFT, int( rect.Width() * 0.25f ) );
}


void CFarmCropLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_FAMILY_CROP_LOG;
	message.Protocol		= MP_RM_FAMILY_CROP_LOG_SIZE_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	message.dwObjectID		= mFarmIndex;

	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),	sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),		sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CFarmCropLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_FAMILY_CROP_LOG;
	message.Protocol	= MP_RM_FAMILY_CROP_LOG_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CFarmCropLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_FAMILY_CROP_LOG_GET_ACK:
		{
			// 080401 LUJ, 이전 로그에 대한 인덱스 세트를 만들어 중복되지 않도록 체크한다
			std::set< DWORD > indexSet;
			{
				for( int row = 0; row < listCtrl.GetItemCount(); ++row )
				{
					indexSet.insert( _ttoi( listCtrl.GetItemText( row, 0 ) ) );
				}
			}

			const MSG_RM_FAMILY_CROP_LOG* m = ( MSG_RM_FAMILY_CROP_LOG* )message;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_FAMILY_CROP_LOG::Log&	data	= m->mLog[ i ];
				const DWORD							row		= listCtrl.GetItemCount();
				int									step	= 0;

				if( indexSet.end() != indexSet.find( data.mLogIndex ) )
				{
					continue;
				}

				CString text;
				text.Format( _T( "%d" ), data.mLogIndex );
				listCtrl.InsertItem( row, text, 0 );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );

				listCtrl.SetItemText( row, ++step, mApplication.GetText( data.mType ) );

				text.Format( _T( "%d" ), data.mFarmIndex );
				listCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mOwnerIndex );
				listCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mCropIndex );
				listCtrl.SetItemText( row, ++step, text );

				listCtrl.SetItemText( row, ++step, mApplication.GetCropName( data.mCropKind, data.mCropStep ) );

				text.Format( _T( "%d" ), data.mCropLife );
				listCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%d" ), data.mCropSeedGrade );
				listCtrl.SetItemText( row, ++step, text );

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
	case MP_RM_FAMILY_CROP_LOG_SIZE_ACK:
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
	case MP_RM_FAMILY_CROP_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );

			MessageBox( 0, textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_FAMILY_CROP_LOG_STOP_ACK:
		{
			findButton.EnableWindow( TRUE );
			stopButton.EnableWindow( FALSE );

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString( IDS_STRING2 );

			MessageBox( 0, textSearchWasStopped, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_FAMILY_CROP_LOG_NACK_BY_AUTH:
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

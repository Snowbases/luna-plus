#include "stdafx.h"
#include "ItemOptionLogCommand.h"
#include "client.h"


CItemOptionLogCommand::CItemOptionLogCommand( CclientApp& application, const TCHAR* title, DWORD itemDbIndex ) :
CCommand( application, title ),
mItemDbIndex( itemDbIndex )
{}


void CItemOptionLogCommand::SaveToExcel( DWORD serverIndex, const CListCtrl& listCtrl ) const
{
	CString textHead;
	textHead.LoadString( IDS_STRING314 );

	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	CclientApp::SaveToExcel(
		textHead + textLog + _T( "-" ) + mApplication.GetServerName( serverIndex ),
		textLog,
		listCtrl );
}


void CItemOptionLogCommand::Initialize( CListCtrl& listCtrl ) const
{
	CRect	rect;
	int		step = -1;

	listCtrl.GetClientRect( rect );

	listCtrl.InsertColumn( ++step,  _T( "log index" ), LVCFMT_LEFT, 0 );

	CString textDate;
	textDate.LoadString( IDS_STRING3 );
	listCtrl.InsertColumn( ++step, textDate, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

	CString textType;
	textType.LoadString( IDS_STRING4 );
	listCtrl.InsertColumn( ++step, textType, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

	CString textItemDbIndex;
	textItemDbIndex.LoadString( IDS_STRING13 );
	listCtrl.InsertColumn( ++step, textItemDbIndex, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );

	CString textStrength;
	textStrength.LoadString( IDS_STRING70 );

	CString textDexterity;
	textDexterity.LoadString( IDS_STRING71 );

	CString textVitality;
	textVitality.LoadString( IDS_STRING72 );

	CString textIntelligence;
	textIntelligence.LoadString( IDS_STRING73 );

	CString textWisdom;
	textWisdom.LoadString( IDS_STRING74 );

	CString textLife;
	textLife.LoadString( IDS_STRING75 );

	CString textMana;
	textMana.LoadString( IDS_STRING76 );

	CString textLifeRecovery;
	textLifeRecovery.LoadString( IDS_STRING77 );

	CString textManaRecovery;
	textManaRecovery.LoadString( IDS_STRING78 );

	CString textPhysicalAttack;
	textPhysicalAttack.LoadString( IDS_STRING79 );

	CString textPhysicalDefence;
	textPhysicalDefence.LoadString( IDS_STRING80 );

	CString textMagicalAttack;
	textMagicalAttack.LoadString( IDS_STRING81 );

	CString textMagicalDefence;
	textMagicalDefence.LoadString( IDS_STRING82 );

	CString textMoveSpeed;
	textMoveSpeed.LoadString( IDS_STRING83 );

	CString textEvade;
	textEvade.LoadString( IDS_STRING84 );

	CString textAccuracy;
	textAccuracy.LoadString( IDS_STRING85 );

	CString textCriticalDamage;
	textCriticalDamage.LoadString( IDS_STRING86 );

	CString textCriticalRate;
	textCriticalRate.LoadString( IDS_STRING87 );

	CString textReinforce;
	textReinforce.LoadString( IDS_STRING89 );

	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textStrength, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textDexterity, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textVitality, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textIntelligence, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textWisdom, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textLife, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textMana, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textLifeRecovery, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textManaRecovery, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textPhysicalAttack, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textPhysicalDefence, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textMagicalAttack, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textMagicalDefence, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textCriticalRate, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textCriticalDamage, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textMoveSpeed, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textEvade, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textReinforce + _T( ": " ) + textAccuracy, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );

	CString textMix;
	textMix.LoadString( IDS_STRING90 );

	listCtrl.InsertColumn( ++step, textMix + _T( ": " ) + textStrength, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textMix + _T( ": " ) + textIntelligence, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textMix + _T( ": " ) + textDexterity, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textMix + _T( ": " ) + textWisdom, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textMix + _T( ": " ) + textVitality, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );

	CString textEnchant;
	textEnchant.LoadString( IDS_STRING91 );

	CString textLevel;
	textLevel.LoadString( IDS_STRING5 );
	CString textIndex;
	textIndex.LoadString( IDS_STRING22 );

	listCtrl.InsertColumn( ++step, textEnchant + _T( ": " ) + textIndex, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );
	listCtrl.InsertColumn( ++step, textEnchant + _T( ": " ) + textLevel, LVCFMT_RIGHT, int( rect.Width() * 0.15f ) );

	CString textMemo;
	textMemo.LoadString( IDS_STRING17 );
	listCtrl.InsertColumn( ++step, textMemo, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );
}


void CItemOptionLogCommand::Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime )
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_ITEM_OPTION_LOG;
	message.Protocol		= MP_RM_ITEM_OPTION_LOG_SIZE_SYN;
	message.mRequestTick	= mTickCount = GetTickCount();
	message.dwObjectID		= mItemDbIndex;

	strncpy( message.mBeginDate,	CW2AEX< MAX_PATH >( beginTime ),	sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate,		CW2AEX< MAX_PATH >( endTime ),		sizeof( message.mEndDate ) );

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CItemOptionLogCommand::Stop( DWORD serverIndex ) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_ITEM_OPTION_LOG;
	message.Protocol	= MP_RM_ITEM_OPTION_LOG_STOP_SYN;

	mApplication.Send( serverIndex, message, sizeof( message ) );
}


void CItemOptionLogCommand::Parse( const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton ) const
{
	switch( message->Protocol )
	{
	case MP_RM_ITEM_OPTION_LOG_GET_ACK:
		{
			// 080401 LUJ, 이전 로그에 대한 인덱스 세트를 만들어 중복되지 않도록 체크한다
			std::set< DWORD > indexSet;
			{
				for( int row = 0; row < listCtrl.GetItemCount(); ++row )
				{
					indexSet.insert( _ttoi( listCtrl.GetItemText( row, 0 ) ) );
				}
			}

			const MSG_RM_ITEM_OPTION_LOG* m = ( MSG_RM_ITEM_OPTION_LOG* )message;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_ITEM_OPTION_LOG::Log& data = m->mLog[ i ];

				const DWORD row		= listCtrl.GetItemCount();
				int			step	= -1;

				if( indexSet.end() != indexSet.find( data.mLogIndex ) )
				{
					continue;
				}

				CString text;
				text.Format( _T( "%d" ), data.mLogIndex );
				listCtrl.InsertItem( row, text, ++step );

				listCtrl.SetItemText( row, ++step, CA2WEX< sizeof( data.mDate ) >( data.mDate ) );

				listCtrl.SetItemText( row, ++step, mApplication.GetText( data.mType ) );

				text.Format( _T( "%d" ), data.mItemDbIndex );
				listCtrl.SetItemText( row, ++step, text );

				{
					const ITEM_OPTION::Reinforce& reinforce = data.mOption.mReinforce;

					text.Format( _T( "%d" ), reinforce.mStrength );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mDexterity );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mVitality );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mIntelligence );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mWisdom );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mLife );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mMana );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mLifeRecovery );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mManaRecovery );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mPhysicAttack );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mPhysicDefence );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mMagicAttack );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), reinforce.mMagicDefence );
					listCtrl.SetItemText( row, ++step, text );

					// 080428 LUJ, 크리티컬 레이트가 이동 속도로 잘못 저장되고 있음
					text.Format( _T( "%d" ), reinforce.mMoveSpeed );
					listCtrl.SetItemText( row, ++step, text );

					// 080428 LUJ, 크리티컬 데미지가 회피로 잘못 저장되고 있음
					text.Format( _T( "%d" ), reinforce.mEvade );
					listCtrl.SetItemText( row, ++step, text );

					// 080428 LUJ, 이동 속도가 명중으로 잘못 저장되고 있음. 
					text.Format( _T( "%d" ), reinforce.mAccuracy );
					listCtrl.SetItemText( row, ++step, text );

					// 080428 LUJ, 회피가 크리티컬 레이트로 잘못 저장되고 있음
					text.Format( _T( "%d" ), reinforce.mCriticalRate );
					listCtrl.SetItemText( row, ++step, text );

					// 080428 LUJ, 명중이 크리티컬 데미지로 잘못 저장되고 있음
					text.Format( _T( "%d" ), reinforce.mCriticalDamage );
					listCtrl.SetItemText( row, ++step, text );
				}

				{
					const ITEM_OPTION::Mix& mix = data.mOption.mMix;

					text.Format( _T( "%d" ), mix.mStrength );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), mix.mIntelligence );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), mix.mDexterity );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), mix.mWisdom );
					listCtrl.SetItemText( row, ++step, text );

					text.Format( _T( "%d" ), mix.mVitality );
					listCtrl.SetItemText( row, ++step, text );
				}

				{
					const ITEM_OPTION::Enchant& enchant = data.mOption.mEnchant;
					text.Format( _T( "%d" ), enchant.mIndex );
					listCtrl.SetItemText( row, ++step, text );
					text.Format( _T( "%d" ), enchant.mLevel );
					listCtrl.SetItemText( row, ++step, text );
				}

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
	case MP_RM_ITEM_OPTION_LOG_SIZE_ACK:
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
	case MP_RM_ITEM_OPTION_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString( IDS_STRING1 );

			MessageBox( 0, textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_ITEM_OPTION_LOG_STOP_ACK:
		{
			findButton.EnableWindow( TRUE );
			stopButton.EnableWindow( FALSE );

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString( IDS_STRING2 );

			MessageBox( 0, textSearchWasStopped, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_ITEM_OPTION_LOG_NACK_BY_AUTH:
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

#include "stdafx.h"
#include "GuildFieldWar.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include "GameIn.h"
#include "GuildManager.h"
#include "GuildFieldWarDialog.h"
#include "CharacterDialog.h"
#include "GuildDialog.h"
#include "GuildLevelupDialog.h"
#include "GuildRankDialog.h"
#include "MHMap.h"
#include "GuildUnion.h"
#include "../Interface/cWindowManager.h"
#include "../interface/cDialog.h"


class SortEnemy
{
public:
	bool operator() ( const MSG_GUILD_LIST::Data& lhs, const MSG_GUILD_LIST::Data& rhs )
	{
		if( lhs.UnionIdx > rhs.UnionIdx )
		{
			return TRUE;
		}
		else if( lhs.UnionIdx < rhs.UnionIdx )
		{
			return FALSE;
		}

		if( lhs.UnionIdx )
		{
			return TRUE == lhs.mIsUnionMaster;
		}

		return lhs.GuildIdx > lhs.GuildIdx;
	}
};


CGuildWarManager::CGuildWarManager()
{
	//mEnemyList.Initialize( 20 );
	mSelectedGuild		= 0;
	mSelectedEnemyGuildIndex	= 0;
	mTempMoney		= 0;

	mVictoryCount		= 0;
	mDrawCount			= 0;
	mLooseCount			= 0;
}


CGuildWarManager::~CGuildWarManager()
{
	Release();
}

void CGuildWarManager::Init()
{
	Release();
	mSelectedGuild = NULL;
	mSelectedEnemyGuildIndex = 0;
	mTempMoney = 0;

	mVictoryCount = mDrawCount = mLooseCount = 0;

	if( GAMEIN->GetGuildWarInfoDlg()->IsActive() )
		GAMEIN->GetGuildWarInfoDlg()->SetActive( FALSE );
}

void CGuildWarManager::Release()
{
	//GUILDINFO* pGuildInfo = NULL;
	//mEnemyList.SetPositionHead();
	//while( pGuildInfo = mEnemyList.GetData() )
	//	delete pGuildInfo;
	//mEnemyList.RemoveAll();

	mEnemyList.clear();
}

BOOL CGuildWarManager::IsEnemy( CPlayer* player )
{
	ASSERT( player );

	if( MAP->IsVillage() )
	{
		return FALSE;
	}

//	if( HERO && HERO->GetGuildIdx() == 0 )	return FALSE;
	//DWORD dwGuildIdx = pTarget->GetGuildIdx();
	//if( dwGuildIdx == 0 )	return FALSE;

	for( EnemyList::const_iterator it = mEnemyList.begin(); mEnemyList.end() != it; ++it )
	{
		const MSG_GUILD_LIST::Data& data = *it;

		if( data.GuildIdx == player->GetGuildIdx() )
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CGuildWarManager::IsEnemy( const char* guildName ) const
{
	for( EnemyList::const_iterator it = mEnemyList.begin(); mEnemyList.end() != it; ++it )
	{
		const MSG_GUILD_LIST::Data& data = *it;

		if( ! strcmp( guildName, data.GuildName ) )
		{
			return TRUE;
		}
	}

	return FALSE;

	//GUILDINFO* pInfo = NULL;
	//mEnemyList.SetPositionHead();
	//while( pInfo = mEnemyList.GetData() )
	//{
	//	if( strcmp( pGuildName, pInfo->GuildName ) == 0 )
	//		return TRUE;
	//}
	//return FALSE;
}

void CGuildWarManager::AddEnemy( const MSG_GUILD_LIST::Data& data )
{
	if( IsEnemy( data.GuildName ) )
	{
		return;
	}

	mEnemyList.push_back( data );

	OBJECTMGR->ApplyOverInfoOptionToAll();

	mEnemyList.sort( SortEnemy() );
	
	//if( mEnemyList.GetData( pGuildInfo->GuildIdx ) )
	//{
	//	return;
	//}

	//GUILDINFO* pInfo = new GUILDINFO;
	//*pInfo = *pGuildInfo;
	//mEnemyList.Add( pInfo, pInfo->GuildIdx );

	//OBJECTMGR->ApplyOverInfoOptionToAll();
}

void CGuildWarManager::RemoveEnemy( DWORD guildIndex )
{
	for( EnemyList::iterator it = mEnemyList.begin(); mEnemyList.end() != it; ++it )
	{
		const MSG_GUILD_LIST::Data& data = *it;

		if( data.GuildIdx == guildIndex )
		{
			mEnemyList.erase( it );
			mEnemyList.sort( SortEnemy() );

			OBJECTMGR->ApplyOverInfoOptionToAll();

			return;
		}
	}

	//GUILDINFO* pInfo = mEnemyList.GetData( dwEnemyGuildIdx );
	//if( !pInfo )	return;

	//delete pInfo;
	//mEnemyList.Remove( dwEnemyGuildIdx );

	//OBJECTMGR->ApplyOverInfoOptionToAll();
}


const MSG_GUILD_LIST::Data* CGuildWarManager::GetEnemy( DWORD guildIndex ) const
{
	for( EnemyList::const_iterator it = mEnemyList.begin(); mEnemyList.end() != it; ++it )
	{
		const MSG_GUILD_LIST::Data& data = *it;

		if( data.GuildIdx == guildIndex )
		{
			return &data;
		}
	}

	return 0;
}

const MSG_GUILD_LIST::Data* CGuildWarManager::GetEnemyFromListIndex( int index )
{
	int count = 0;

	for( EnemyList::const_iterator it = mEnemyList.begin(); mEnemyList.end() != it; ++it )
	{
		mSelectedGuild = &( *it );

		if( count++ == index )
		{
			return mSelectedGuild;
		}
	}

	return 0;

	//int count = 0;

	//mEnemyList.SetPositionHead();
	//while( mSelectedGuild = mEnemyList.GetData() )
	//{
	//	if( count == nIdx )
	//	{
	//		return mSelectedGuild;
	//	}
	//	++count;
	//}

	//return NULL;
}

BOOL CGuildWarManager::IsValid( DWORD dwKind )
{
	switch( dwKind )
	{
	case 0:		// declare
		{	
			// 주의! 테스트를 위해 길드전 가능한 회원수 체크를 없앰
			//if( GUILDMGR->GetMemberSize() < 5 )
			//{
			//	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 855 ) );
			//	return FALSE;
			//}							
		}
		break;
	case 1:
	case 2:		// suggest
		{
			if( mEnemyList.size() < 1 )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 856 ) );
				return FALSE;
			}
		}
		break;
	case 3:		// breakup guild
		{
			if( mEnemyList.size() > 0 )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 857 ) );
				return FALSE;
			}
		}
		break;
	case 4:		// checkmoney
		{
			if( mTempMoney )
			{
				if( HERO->GetMoney() < mTempMoney )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 858 ) );
					return FALSE;
				}
			}
		}
		break;
	}
	
	return TRUE;
}

void CGuildWarManager::GetVictory( char* temp )
{
	sprintf( temp, "%d - %d - %d", mVictoryCount, mDrawCount, mLooseCount );
}

void CGuildWarManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_GUILD_WAR_DECLARE:
		{
			const MSG_GUILD_LIST*		message = ( MSG_GUILD_LIST* )pMsg;
			const MSG_GUILD_LIST::Data& data	= message->mData[ 0 ];

			mSelectedEnemyGuildIndex	= data.GuildIdx;
			mTempMoney		= data.MarkName;

			CGFWarResultDlg* dialog = GAMEIN->GetGFWarResultDlg();
			ASSERT( dialog );

			dialog->ShowDeclare( data );

			//// 버튼 표시 갱신
			//{
			//	CDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
			//	ASSERT( dialog );

			//	dialog->SetActive( TRUE );
			//}
		}
		break;
	case MP_GUILD_WAR_NACK:
		{
			MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;


			switch( pmsg->bData )
			{
			case 1:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 859 ) );	break;	// sender is not master
			case 2:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 860 )  );	break;	// is not guild
			case 3:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 861 )  );	break;	// same guild
			case 4:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 862 ) );	break;	// not login
			case 5:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 863 )  );	break;	// other guild master money not enough
			case 6:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 864 )  );	break;	// self money not enough			
			}

			GAMEIN->GetGFWarDeclareDlg()->SetActive( FALSE );			
		}
		break;
	case MP_GUILD_WAR_DECLARE_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			switch( pmsg->dwData2 )
			{
			//case 0:	CHATMGR->AddMsg( CTC_SYSMSG,  "문파(단)의 Level이 맞지않아 선전포고를 할 수 없습니다." );	break;	// guild level is wrong
			//case 1:	CHATMGR->AddMsg( CTC_SYSMSG,  "상대문파(단)의 Level이 맞지않아 선전포고를 할 수 없습니다." );	break;	// other guild level is wrong
			case 2:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 867 )  );	break;	// guild membernum is wrong
			case 3:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 868 )  );	break;	// other guild membernum is wrong
			case 4:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 869 ) );	break;	// other guild deny
			case 5:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 870 )  );	break;	// other guild war end
			}

			GAMEIN->GetGFWarDeclareDlg()->SetActive( FALSE );
		}
		break;
	// 상대 길드가 연합에 속해있지만 마스터 길드가 아니어서 서버에 의해 자동 거절됨
	case MP_GUILD_WAR_DECLARE_NACK_BY_UNION:
		{
			const MSG_NAME* message = ( MSG_NAME* )pMsg;

			CHATMGR->AddMsg(
				CTC_GUILD_NOTICE,
				CHATMGR->GetChatMsg( 1125 ),
				message->Name );
		}
		break;

	case MP_GUILD_WAR_WAIT:
		{
			MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;

			switch( pmsg->bData )
			{
			case MP_GUILD_WAR_DECLARE:
				{
					GAMEIN->GetGFWarDeclareDlg()->SetActive( FALSE );
				}
				break;
			case MP_GUILD_WAR_SUGGESTEND:
				break;
			}			

			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 871 )  );
		}
		break;
	case MP_GUILD_WAR_DECLARE_ACCEPT:
		{
		}
		break;
	case MP_GUILD_WAR_DECLARE_DENY:
		{
			GAMEIN->GetGFWarDeclareDlg()->SetActive( FALSE );

			WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_CONFIRM, MBT_OK, CHATMGR->GetChatMsg( 872 )  );
		}
		break;
	
	case MP_GUILD_WAR_START:		// guildfieldwar start
		{
			const MSG_GUILD_LIST* message = ( MSG_GUILD_LIST* )pMsg;

			for( DWORD i = 0; i < message->mSize; ++i )
			{
				const MSG_GUILD_LIST::Data& data = message->mData[ i ];

				AddEnemy( data );

				// 동맹과 전쟁할 경우, 마스터 길드만 메시지를 표시해주기 위해 필터링한다.
				if(		data.UnionIdx &&
					!	data.mIsUnionMaster )
				{
					continue;
				}

				char name[ MAX_PATH ];

				if( strlen( data.mUnionName ) )
				{
					sprintf( name, CHATMGR->GetChatMsg( 1126 ), data.mUnionName );
				}
				else
				{
					sprintf( name, CHATMGR->GetChatMsg( 1127 ), data.GuildName );
				}

				const DWORD money = data.MarkName;

				if( money )
				{
					WINDOWMGR->MsgBox(
						MBI_GUILDFIELDWAR_CONFIRM,
						MBT_OK,
						CHATMGR->GetChatMsg( 1128 ),
						name,
						AddComma( money ) );
				}
				else
				{
					WINDOWMGR->MsgBox(
						MBI_GUILDFIELDWAR_CONFIRM,
						MBT_OK,
						CHATMGR->GetChatMsg( 1129 ),
						name );
				}
			}

			GAMEIN->GetGFWarInfoDlg()->AddGuildInfoToList();
			GAMEIN->GetGuildWarInfoDlg()->AddListData();
		}
		break;
	case MP_GUILD_WAR_END:			// guildfieldwar end
		{
			const MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			switch( pmsg->dwData1 )
			{
			case 0:	// lose
				{
					WINDOWMGR->MsgBox(
						MBI_GUILDFIELDWAR_CONFIRM,
						MBT_OK,
						CHATMGR->GetChatMsg( 1130 ) );

					++mLooseCount;
					break;
				}
			case 1:	// won
				{
					WINDOWMGR->MsgBox(
						MBI_GUILDFIELDWAR_CONFIRM,
						MBT_OK,
						CHATMGR->GetChatMsg( 1131 ),
						GetEnemy(pmsg->dwData2)->GuildName );

					++mVictoryCount;
					break;
				}
			case 2:	// draw
				{
					const GUILDINFO* info = GetEnemy( pmsg->dwData2 );

					if( ! info )
					{
						break;
					}
					// 동맹에 속한 길드인 경우 마스터 길드가 아닌 경우, 처리만 하고 메시지는 표시하지 않는다
					else if(		info->UnionIdx &&
								!	info->mIsUnionMaster )
					{
						RemoveEnemy( pmsg->dwData2 );

						GAMEIN->GetGFWarInfoDlg()->AddGuildInfoToList();
						GAMEIN->GetGuildWarInfoDlg()->AddListData();
						break;
					}

					WINDOWMGR->MsgBox(
						MBI_GUILDFIELDWAR_CONFIRM,
						MBT_OK,
						CHATMGR->GetChatMsg( 1132 ),
						info->GuildName );

					++mDrawCount;
					
					break;
				}
			}

			RemoveEnemy( pmsg->dwData2 );

			GAMEIN->GetGFWarInfoDlg()->AddGuildInfoToList();
			GAMEIN->GetGuildWarInfoDlg()->AddListData();
		}
		break;
	case MP_GUILD_WAR_SURRENDER:
		{
			const MSG_DWORD4*	pmsg				= ( MSG_DWORD4* )pMsg;
			const DWORD			enemeyGuildIndex	= pmsg->dwData2;
			const DWORD			scoreValue			= pmsg->dwData3;
			const DWORD			scoreVariation		= pmsg->dwData4;
			
			// 점수 업데이트
			{
				CGuildDialog* dialog = ( CGuildDialog* )WINDOWMGR->GetWindowForID( GD_GUILDDLG );
				ASSERT( dialog );

				dialog->SetScore( scoreValue );
			}

			const MSG_GUILD_LIST::Data* data	= GetEnemy( enemeyGuildIndex );

			if( ! data )
			{
				break;
			}

			// 복사해놓고 지우자
			const MSG_GUILD_LIST::Data enemy	= *data;

			RemoveEnemy( enemy.GuildIdx );

			GAMEIN->GetGFWarInfoDlg()->AddGuildInfoToList();
			GAMEIN->GetGuildWarInfoDlg()->AddListData();

			// 동맹일 경우 마스터만 메시지를 표시하자
			if(		enemy.UnionIdx	 &&
				!	enemy.mIsUnionMaster )
			{
				break;
			}

			char line[ MAX_PATH ] = { 0 };

			switch( pmsg->dwData1 )
			{
			case 0:	// surrender
				{
					++mLooseCount;

					if( enemy.UnionIdx )
					{
						sprintf( line, CHATMGR->GetChatMsg( 1133 ), enemy.mUnionName, scoreVariation );
					}
					else
					{
						sprintf( line, CHATMGR->GetChatMsg( 1134 ), enemy.GuildName, scoreVariation );
					}

					break;
				}
			case 1:	// win
				{
					++mVictoryCount;

					if( enemy.UnionIdx )
					{
						sprintf( line, CHATMGR->GetChatMsg( 1135 ), enemy.mUnionName, scoreVariation );
					}
					else
					{
						sprintf( line, CHATMGR->GetChatMsg( 1136 ), enemy.GuildName, scoreVariation );
					}
					
					break;
				}
			}

			WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_CONFIRM, MBT_OK, line );
		}
		break;
	case MP_GUILD_WAR_PROC:
		{
			const MSG_GUILD_LIST* message = ( MSG_GUILD_LIST* )pMsg;

			for( DWORD i = 0; i < message->mSize; ++i )
			{
				const MSG_GUILD_LIST::Data& data = message->mData[ i ];

				AddEnemy( data );

				if(		data.UnionIdx &&
					!	data.mIsUnionMaster )
				{
					continue;
				}

				if( data.mIsUnionMaster )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1137 ), data.mUnionName );
				}
				else
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 880 ) , data.GuildName );
				}
			}

			// interface
			GAMEIN->GetGFWarInfoDlg()->AddGuildInfoToList();
			GAMEIN->GetGuildWarInfoDlg()->AddListData();
		}
		break;	
	case MP_GUILD_WAR_SUGGESTEND:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			const MSG_GUILD_LIST::Data* data = GetEnemy( pmsg->dwData );

			if( data )
			{
				mSelectedEnemyGuildIndex = pmsg->dwData;
				WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_SUGGEST_ACCEPTDENY, MBT_YESNO, CHATMGR->GetChatMsg( 881 ) , data->GuildName );
			}

			//// 버튼 표시 갱신
			//{
			//	CDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
			//	ASSERT( dialog );

			//	dialog->SetActive( TRUE );
			//}
		}
		break;
	case MP_GUILD_WAR_SUGGESTEND_NACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, "Declare or Stop Error!!" );
		}
		break;
	case MP_GUILD_WAR_SUGGESTEND_DENY:
		{
			const MSG_DWORD*			message = (MSG_DWORD*)pMsg;
			const MSG_GUILD_LIST::Data* data	= GetEnemy( message->dwData );

			if( data )
			{
				WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_CONFIRM, MBT_OK, CHATMGR->GetChatMsg( 882 ), data->GuildName );
			}
		}
		break;
	case MP_GUILD_WAR_LEVELDOWN:
		{
			GAMEIN->GetCharacterDialog()->RefreshGuildInfo();
			//GAMEIN->GetGuildDlg()->Refresh( GUILDMGR->GetGuild() );
			//GAMEIN->GetGuildLevelUpDlg()->SetLevel( pmsg->bData );
			//GAMEIN->GetGuildRankDlg()->ShowGuildRankMode( pmsg->bData );
		}
		break;
	case MP_GUILD_WAR_RECORD:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			mVictoryCount = pmsg->dwData1;
			mDrawCount = pmsg->dwData2;
			mLooseCount = pmsg->dwData3;
		}
		break;
	case MP_GUILD_WAR_ADDMONEY:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 883 ), pmsg->dwData );
		}
		break;
	case MP_GUILD_WAR_REMOVEMONEY:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 884 ), pmsg->dwData );
		}
		break;
	case MP_GUILD_WAR_RESULT_TOALLUSER:
		{
			MSG_NAME2* pmsg = (MSG_NAME2*)pMsg;
			switch( pmsg->dwObjectID )
			{
			case 0:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 885 ), pmsg->str1, pmsg->str2, pmsg->str1 );	break;
			case 1:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 886 ), pmsg->str1, pmsg->str2 );	break;
			case 2:	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 887 ), pmsg->str1, pmsg->str2, pmsg->str2, pmsg->str1 );	break;
			}
		}
		break;
	}

	// 버튼 표시 갱신
	{
		cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
		ASSERT( dialog );

		if( dialog->IsActive() )
		{
			dialog->SetActive( TRUE );
		}
	}
}

void CGuildWarManager::SendDeclare( char* pName, char* pMoney )
{
	if( strcmp( pName, "" ) == 0 || strlen(pName) > MAX_GUILD_NAME )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 888 ) );
		return;
	}
	else if( strcmp( GUILDMGR->GetGuildName(), pName ) == 0 )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 889 ) );
		return;
	}
	else if( IsEnemy( pName ) )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 890 ) );
		return;
	}
	//if( GUILDUNION->IsUnionGuild( pName ) )
	//{
	//	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 891 ) );
	//	return;		
	//}

	const DWORD dwMoney = atoi( pMoney );

	if( GAMEIN->GetGFWarDeclareDlg()->IsChecked() )
	{		
		if( dwMoney < 100000 )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 892 ) );
			return;
		}
		if( HERO->GetMoney() < dwMoney )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 893 ) );
			return;
		}
	}

	MSG_NAME_DWORD message;
	message.Category	= MP_GUILD_WAR;
	message.Protocol	= MP_GUILD_WAR_DECLARE;
	message.dwObjectID	= HEROID;
	message.dwData		= dwMoney;
	SafeStrCpy( message.Name, pName, sizeof( message.Name ) );
	
	NETWORK->Send( &message, sizeof( message ) );
}

void CGuildWarManager::SendDeclareAcceptDeny( BYTE Protocol )
{
	if( Protocol == MP_GUILD_WAR_DECLARE_ACCEPT )
	{
		if( !IsValid( 4 ) )
			return;
	}

	GAMEIN->GetGFWarResultDlg()->SetActive( FALSE );

	MSG_DWORD2 Msg;
	Msg.Category	= MP_GUILD_WAR;
	Msg.Protocol	= Protocol;
	Msg.dwObjectID	= HEROID;
	Msg.dwData1		= mSelectedEnemyGuildIndex;
	Msg.dwData2		= mTempMoney;

	NETWORK->Send( &Msg, sizeof(Msg) );
}

void CGuildWarManager::SendSurrender()
{
	GAMEIN->GetGFWarInfoDlg()->SetActive( FALSE );

	if( mSelectedGuild == NULL )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 894 ) );
		return;
	}
	
	MSG_DWORD Msg;
	Msg.Category = MP_GUILD_WAR;
	Msg.Protocol = MP_GUILD_WAR_SURRENDER;
	Msg.dwObjectID = HEROID;
	Msg.dwData = mSelectedGuild->GuildIdx;

	NETWORK->Send( &Msg, sizeof(Msg) );	
}

void CGuildWarManager::SendSuggest()
{
	GAMEIN->GetGFWarInfoDlg()->SetActive( FALSE );

	if( mSelectedGuild == NULL )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 895 ) );
		return;
	}

	MSG_NAME_DWORD Msg;
	Msg.Category = MP_GUILD_WAR;
	Msg.Protocol = MP_GUILD_WAR_SUGGESTEND;
	Msg.dwObjectID = HEROID;
	Msg.dwData = 0;
	SafeStrCpy( Msg.Name, mSelectedGuild->GuildName, MAX_NAME_LENGTH+1 );

	NETWORK->Send( &Msg, sizeof(Msg) );	
}

void CGuildWarManager::SendSuggestAcceptDeny( BYTE Protocol )
{
	MSG_DWORD message;
	message.Category	= MP_GUILD_WAR;
	message.Protocol	= Protocol;
	message.dwObjectID	= HEROID;
	message.dwData		= mSelectedEnemyGuildIndex;

	NETWORK->Send( &message, sizeof( message ) );
}

DWORD CGuildWarManager::GetEnemySize() const
{
	//return mEnemyList.GetDataNum();

	return mEnemyList.size();
}


CGuildWarManager* CGuildWarManager::GetInstance()
{
	static CGuildWarManager instance;

	return &instance;
}


const CGuildWarManager::EnemyList& CGuildWarManager::GetEnemyList() const
{
	return mEnemyList;
}


const MSG_GUILD_LIST::Data* CGuildWarManager::GetUnionMasterEnemy( DWORD unionIndex ) const
{
	for( EnemyList::const_iterator it = mEnemyList.begin(); mEnemyList.end() != it; ++it )
	{
		const MSG_GUILD_LIST::Data& data = *it;

		if( data.UnionIdx == unionIndex &&
			data.mIsUnionMaster )
		{
			return &data;
		}
	}

	return 0;
}
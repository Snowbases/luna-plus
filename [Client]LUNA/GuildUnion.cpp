#include "stdafx.h"
#include "GuildUnion.h"
#include "ObjectManager.h"
#include "GuildManager.h"
#include "GameIn.h"
#include "GuildDialog.h"
//#include "GuildCreateDialog.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "./Interface/cWindowManager.h"
#include "ChatManager.h"
#include "GuildUnionMarkMgr.h"
#include "GuildFieldWar.h"
#include "GuildFieldWarDialog.h"
#include "MHMap.h"
#include "UnionNoteDlg.h"


extern int g_nServerSetNum;


GLOBALTON(CGuildUnion)
CGuildUnion::CGuildUnion()
{
	mIndex = 0;
	memset( mName, 0, MAX_GUILD_NAME+1 );
	memset( mData, 0, sizeof(sGUILDIDXNAME)*MAX_GUILD_UNION_NUM );

	m_dwTempPlayerId = m_dwTempGuildUnionIdx = m_dwTempGuildIdx = 0;
}

CGuildUnion::~CGuildUnion()
{}


BOOL CGuildUnion::IsMaster( CPlayer* player )
{
	ASSERT( player );

	if(	player											&&
		player->GetGuildMemberRank() == GUILD_MASTER	&&
		mData[ 0 ].dwGuildIdx == player->GetGuildIdx() )
	{
		return TRUE;
	}

	return FALSE;
}


const char* CGuildUnion::GetNameFromListIndex( int nIdx )
{
	if( nIdx < 0 || nIdx >= MAX_GUILD_UNION_NUM )
	{
		m_dwTempGuildIdx = 0;
		return NULL;
	}
	m_dwTempGuildIdx = mData[nIdx+1].dwGuildIdx;
	if( !m_dwTempGuildIdx )
		return NULL;
	return mData[nIdx+1].sGuildName;
}

void CGuildUnion::Clear()
{
	mIndex = 0;

	ZeroMemory( mName, sizeof( mName ) );
	ZeroMemory( mData, sizeof( mData ) );
	
	m_dwTempPlayerId		= 0;
	m_dwTempGuildUnionIdx	= 0;
	m_dwTempGuildIdx		= 0;

	CHero* hero = OBJECTMGR->GetHero();

	if( hero )
	{
		hero->SetGuildUnionIdx( 0 );
		hero->SetGuildUnionName( "" );
		hero->SetGuildUnionMarkIdx( 0 );
	}
		
	GAMEIN->GetGuildDlg()->SetUnionName( "" );
}

void CGuildUnion::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_GUILD_UNION_PLAYER_INFO:
		{
			MSG_NAME_DWORD3*	pmsg	= ( MSG_NAME_DWORD3* )pMsg;
			CPlayer*			pPlayer = ( CPlayer* )OBJECTMGR->GetObject( pmsg->dwData1 );

			if( pPlayer )
			{
				pPlayer->SetGuildUnionIdx( pmsg->dwData2 );
				pPlayer->SetGuildUnionName( pmsg->Name );
				pPlayer->SetGuildUnionMarkIdx( pmsg->dwData3 );
			}
			
			break;
		}
	case MP_GUILD_UNION_INFO:
		{
			SEND_GUILD_UNION_INFO* pmsg = (SEND_GUILD_UNION_INFO*)pMsg;

			mIndex = pmsg->dwGuildUnionIdx;

			SafeStrCpy( mName, pmsg->sGuildUnionName, sizeof( mName ) );
			for( int i = 0; i < pmsg->nMaxGuildNum; ++i )
				mData[i] = pmsg->GuildInfo[i];
			HERO->SetGuildUnionIdx( mIndex );
			HERO->SetGuildUnionName( mName );
			HERO->SetGuildUnionMarkIdx( pmsg->dwGuildUnionMarkIdx );

			GAMEIN->GetGuildDlg()->SetUnionName( mName );

			break;
		}
	case MP_GUILD_UNION_CREATE_ACK:
		{
			MSG_NAME_DWORD2* pmsg = (MSG_NAME_DWORD2*)pMsg;	
			if( pmsg->dwData2 != HERO->GetGuildIdx() )	return;

			mIndex = pmsg->dwData1;
			SafeStrCpy( mName, pmsg->Name, sizeof( mName ) );
			mData[0].dwGuildIdx = pmsg->dwData2;
			SafeStrCpy( mData[0].sGuildName, GUILDMGR->GetGuildName(), sizeof( mData[ 0 ].sGuildName ) );

			HERO->SetGuildUnionIdx( mIndex );
			HERO->SetGuildUnionName( mName );
			HERO->SetGuildUnionMarkIdx( 0 );

			// 길드 창 업데이트
			{
				CGuildDialog* dialog = GAMEIN->GetGuildDlg();
				ASSERT( dialog );

				dialog->SetUnionName( mName );

				if( dialog->IsActive() )
				{
					dialog->SetActive( TRUE );
				}
			}

			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1010), mName );

			break;
		}
	case MP_GUILD_UNION_DESTROY_ACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			if( mIndex == pmsg->dwData1 )
			{
				CGuildDialog* dialog = GAMEIN->GetGuildDlg();
				ASSERT( dialog );

				// chat msg
				switch( pmsg->dwData2 )
				{
				case 0:
					{
						CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1011), dialog->GetUnionName() );
						break;
					}
				case 1:
					{
						CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1012), dialog->GetUnionName() );
						break;
					}
				case 2:
					{
						CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1013), dialog->GetUnionName() );
						break;
					}
				}

				Clear();

				GAMEIN->GetGuildWarInfoDlg()->AddListData();

				if( dialog->IsActive() )
				{
					dialog->SetActive( TRUE );
				}
			}

			break;
		}
	case MP_GUILD_UNION_INVITE:
		{
			MSG_NAME_DWORD2* pmsg = (MSG_NAME_DWORD2*)pMsg;

			m_dwTempPlayerId = pmsg->dwData1;
			m_dwTempGuildUnionIdx = pmsg->dwData2;
			WINDOWMGR->MsgBox( MBI_UNION_INVITE, MBT_YESNO, CHATMGR->GetChatMsg( 1030 ), pmsg->Name );	

			break;
		}
	case MP_GUILD_UNION_INVITE_WAIT:
		{
			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1028) );

			break;
		}
	case MP_GUILD_UNION_INVITE_DENY:
		{
			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1014) );

			break;
		}
	case MP_GUILD_UNION_ADD_ACK:
		{
			/*
			두 가지 경우에 여기로 서버 측으로부터의 메시지가 온다

			1. 내가 다른 연합에 가입했을 때
			2. 내 연합에 다른 길드가 가입했을 때
			*/

			SEND_GUILD_UNION_INFO* pmsg = (SEND_GUILD_UNION_INFO*)pMsg;

			mIndex = pmsg->dwGuildUnionIdx;

			SafeStrCpy( mName, pmsg->sGuildUnionName, sizeof( mName ) );
			//ZeroMemory( mData, sizeof( mData ) );

			CHero* hero = OBJECTMGR->GetHero();
			ASSERT( hero );

			if( hero )
			{
				hero->SetGuildUnionIdx( mIndex );
				hero->SetGuildUnionName( mName );
				hero->SetGuildUnionMarkIdx( pmsg->dwGuildUnionMarkIdx );
			}

			for( int i = 0; i < pmsg->nMaxGuildNum; ++i )
			{
				const sGUILDIDXNAME&	source	= pmsg->GuildInfo[i];
				sGUILDIDXNAME&			dest	= mData[ i ];

				if( dest.dwGuildIdx == source.dwGuildIdx )
				{
					continue;
				}

				dest = source;

				if( dest.dwGuildIdx != hero->GetGuildIdx() )
				{
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg( 1015 ), source.sGuildName );
				}
			}

			// 길드 창 업데이트
			{
				CGuildDialog* dialog = GAMEIN->GetGuildDlg();
				ASSERT( dialog );

				dialog->SetUnionName( mName );

				if( dialog->IsActive() )
				{
					dialog->SetActive( TRUE );
				}
			}

			GAMEIN->GetGuildWarInfoDlg()->AddListData();

			break;
		}
	case MP_GUILD_UNION_REMOVE_ACK:
		{
			SEND_GUILD_UNION_INFO* pmsg = (SEND_GUILD_UNION_INFO*)pMsg;

			//memset( mData, 0, sizeof(sGUILDIDXNAME)*MAX_GUILD_UNION_NUM );
			for( int i = 0; i < pmsg->nMaxGuildNum; ++i )
				mData[i] = pmsg->GuildInfo[i];

			GAMEIN->GetGuildWarInfoDlg()->AddListData();

			// chat msg

			sGUILDIDXNAME& removedData = mData[ i ];

			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1016), removedData.sGuildName );

			ZeroMemory( &removedData, sizeof( removedData ) );

			break;
		}
	case MP_GUILD_UNION_SECEDE_ACK:
		{
			/*
			주의 서버에서는 탈퇴되고 남은 동맹 멤버 정보만 보내온다.
			*/

			typedef std::map< DWORD, sGUILDIDXNAME >	DataMap;
			DataMap										previousDataMap;

			typedef std::set< DWORD >					IndexSet;
			IndexSet									indexSet;

			// 지우기 전의 정보를 저장해둔다. 탈퇴한 길드만 메시지를 표시해주기 위해 사용한다.
			for( DWORD size = sizeof( mData ) / sizeof( sGUILDIDXNAME ); size--; )
			{
				const sGUILDIDXNAME& info = mData[ size ];

				if( info.dwGuildIdx )
				{
					previousDataMap[ info.dwGuildIdx ] = info;
				}
			}

			SEND_GUILD_UNION_INFO* pmsg = (SEND_GUILD_UNION_INFO*)pMsg;

			ZeroMemory( mData, sizeof( mData ) );

			// 새로 서버에서 보내진 정보도 저장한다. 
			for( int i = 0; i < pmsg->nMaxGuildNum; ++i )
			{
				sGUILDIDXNAME& info = mData[ i ];

				info = pmsg->GuildInfo[i];

				indexSet.insert( info.dwGuildIdx );
			}

			// 이전 정보를 기준으로 돌려서 없을 경우 메시지를 표시한다.
			for( DataMap::const_iterator it = previousDataMap.begin(); previousDataMap.end() != it; ++it )
			{
				const sGUILDIDXNAME& previousInfo = it->second;

				if( indexSet.end() == indexSet.find( previousInfo.dwGuildIdx ) )
				{
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg( 1017 ), previousInfo.sGuildName );
				}
			}

			GAMEIN->GetGuildWarInfoDlg()->AddListData();

			break;
		}
	case MP_GUILD_UNION_MARK_REGIST_ACK:
		{
			MSG_GUILDUNIONMARK_IMG* pmsg = (MSG_GUILDUNIONMARK_IMG*)pMsg;

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwObjectID );
			if( !pPlayer )	return;

			pPlayer->SetGuildUnionMarkIdx( pmsg->dwMarkIdx );
			GUILDUNIONMARKMGR->SaveGuildUnionMark( g_nServerSetNum, pmsg->dwGuildUnionIdx, pmsg->dwMarkIdx, pmsg->imgData );

			if(gHeroID == pmsg->dwObjectID)
			{
				if(0 == pmsg->dwMarkIdx)
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1974));
					WINDOWMGR->MsgBox(
						MBI_NOTICE,
						MBT_OK,
						CHATMGR->GetChatMsg(1974));
				}
				else
				{
					CHATMGR->AddMsg(
						CTC_GUILD_NOTICE,
						CHATMGR->GetChatMsg(1018));
				}
			}

			break;
		}
	case MP_GUILD_UNION_MARK_REQUEST_ACK:
		{
			MSG_GUILDUNIONMARK_IMG* pmsg = (MSG_GUILDUNIONMARK_IMG*)pMsg;

			GUILDUNIONMARKMGR->SaveGuildUnionMark( g_nServerSetNum, pmsg->dwGuildUnionIdx, pmsg->dwMarkIdx, pmsg->imgData );

			break;
		}
	case MP_GUILD_UNION_NOTE_ACK:	
		{
			GAMEIN->GetUnionNoteDlg()->Use();	
			CHATMGR->AddMsg(CTC_TOWHOLE, CHATMGR->GetChatMsg( 1296 ) );

			break;
		}
	case MP_GUILD_UNION_CREATE_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			switch( pmsg->dwData1 )
			{
			case eGU_Not_ValidName:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1027) );	break;
			case eGU_Not_InGuild:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1020) );	break;
			case eGU_Not_GuildMaster:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1021) );	break;
			case eGU_Not_Level:				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1007) );	break;
			case eGU_Aleady_InGuildUnion:	CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1008) );	break;
			case eGU_Time:
				{
					stTIME time;
					time.value = pmsg->dwData2;
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg( 1031 ), time.GetYear()+2000, time.GetMonth(),
						time.GetDay(), time.GetHour(), time.GetMinute() );
				}
				break;
			case eGU_Not_ValidMap:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1005) );	break;
			default:						CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );	break;
			}

			break;
		}
	case MP_GUILD_UNION_DESTROY_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			switch( pmsg->dwData1 )
			{
			case eGU_Not_InGuild:
				{
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1020) );
					break;
				}
			case eGU_Not_GuildMaster:
				{
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1021) );
					break;
				}
			case eGU_Not_InGuildUnion:
				{
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1009) );
					break;
				}
			case eGU_Not_GuildUnionMaster:
				{
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );
					break;
				}
			case eGU_Not_ValidMap:
				{
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1005) );
					break;
				}
			case eGU_No_Lone_Union:
				{
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg( 1102 ) );
					break;
				}
			case eGU_Is_GuildFieldWar:
				{
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg( 857 ) );
					break;
				}
			default:
				{
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );
					break;
				}
			}

			break;
		}
	case MP_GUILD_UNION_INVITE_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			switch( pmsg->dwData1 )
			{
			case eGU_Not_InGuild:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1020) );	break;
			case eGU_Not_GuildMaster:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1021) );	break;
			case eGU_Not_InGuildUnion:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1009) );	break;
			case eGU_Not_GuildUnionMaster:	CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );	break;
			case eGU_Is_Full:				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1022) );	break;
			case eGU_Other_Not_InGuild:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1023) );	break;
			case eGU_Other_Not_GuildMaster:	CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1024) );	break;
			case eGU_Other_Aleady_InGuildUnion:	CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1025) );	break;
			case eGU_Other_Time:
				{
					stTIME time;
					time.value = pmsg->dwData2;
					CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg( 1103 ),
						time.GetYear()+2000,
						time.GetMonth(),
						time.GetDay(),
						time.GetHour(),
						time.GetMinute() );
				}
				break;
			case eGU_Is_GuildFieldWar:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1029) );	break;
			case eGU_Not_ValidMap:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1005) );	break;
			default:						CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );	break;
			}

			break;
		}
	case MP_GUILD_UNION_REMOVE_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			switch( pmsg->dwData1 )
			{
			case eGU_Not_InGuild:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1020) );	break;
			case eGU_Not_GuildMaster:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1021) );	break;
			case eGU_Not_InGuildUnion:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1009) );	break;
			case eGU_Not_GuildUnionMaster:	CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );	break;
			case eGU_Other_Not_InGuild:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1023) );	break;
			case eGU_Other_Not_InGuildUnion:	CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1026) );	break;
			case eGU_Not_ValidMap:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1005) );	break;
			default:						CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );	break;
			}

			break;
		}
	case MP_GUILD_UNION_SECEDE_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			switch( pmsg->dwData1 )
			{
			case eGU_Not_InGuildUnion:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1009) );	break;
			case eGU_Not_InGuild:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1020) );	break;
			case eGU_Not_GuildMaster:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1021) );	break;
			case eGU_Not_ValidMap:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1005) );	break;
			default:						CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );	break;
			}

			break;
		}
	case MP_GUILD_UNION_MARK_REGIST_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			switch( pmsg->dwData1 )
			{
			case eGU_Not_InGuildUnion:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1009) );	break;
			case eGU_Not_InGuild:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1020) );	break;
			case eGU_Not_GuildMaster:		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1021) );	break;
			case eGU_Not_GuildUnionMaster:	CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );	break;
			case eGU_Not_ValidMap:			CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1005) );	break;
			default:						CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1010) );	break;
			}

			break;
		}
	}	
}


void CGuildUnion::SendDestroyGuildUnion()
{
	if( CheckGuildUnionCondition( 1 ) )
	{
		MSG_DWORD message;
		message.Category	= MP_GUILD_UNION;
		message.Protocol	= MP_GUILD_UNION_DESTROY_SYN;
		message.dwObjectID	= HEROID;
		message.dwData		= mIndex;

		NETWORK->Send( &message, sizeof( message ) );
	}
}


void CGuildUnion::SendInviteAcceptDeny( BYTE Protocol )
{
	MSG_DWORD2 Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = Protocol;
	Msg.dwObjectID = HEROID;
	Msg.dwData1 = m_dwTempPlayerId;
	Msg.dwData2 = m_dwTempGuildUnionIdx;

	NETWORK->Send( &Msg, sizeof(Msg) );
}

void CGuildUnion::SendRemoveGuildUnion()
{
	if( !m_dwTempGuildIdx )	return;
	if( !CheckGuildUnionCondition( 3 ) )	return;

	GAMEIN->GetGFWarInfoDlg()->SetActive( FALSE );

	MSG_DWORD2 Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_REMOVE_SYN;
	Msg.dwObjectID = HEROID;
	Msg.dwData1 = mIndex;
	Msg.dwData2 = m_dwTempGuildIdx;
	
	NETWORK->Send( &Msg, sizeof(Msg) );
}


BOOL CGuildUnion::CheckGuildUnionCondition( int nCondition )
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		ASSERT( 0 );
		return FALSE;
	}
	else if( !hero->GetGuildIdx() )
	{
		return FALSE;
	}
/*
	else if( SIEGEMGR->GetSiegeWarMapNum() == MAP->GetMapNum() )
	{
		CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1005) );
		return FALSE;
	}
*/
	switch( nCondition )
	{
	case 0:		// create
		{			
			if( hero->GetGuildMemberRank() != GUILD_MASTER )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );
				return FALSE;
			}

			const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( GUILDMGR->GetLevel() );

            if( 1 > setting->mUnionSize )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1007) );
				return FALSE;
			}
			else if( mIndex )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1008) );
				return FALSE;
			}
		}
		break;
	case 1:		// destroy
		{
			if( hero->GetGuildMemberRank() != GUILD_MASTER || mData[0].dwGuildIdx != hero->GetGuildIdx() )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );
				return FALSE;
			}
			if( mIndex == 0 )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1009) );
				return FALSE;
			}
		}
		break;
	case 2:		// invite
		{			
			ASSERT( hero );

			if( hero->GetGuildMemberRank() != GUILD_MASTER )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );
				return FALSE;
			}
			if( mIndex == 0 )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1009) );
				return FALSE;
			}

			const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( GUILDMGR->GetLevel() );

			if( ! setting )
			{
				return FALSE;
			}
			else if( setting->mUnionSize == GetSize() )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1019) );
				return FALSE;
			}
		}
		break;
	case 3:		// remove
		{
			if( hero->GetGuildMemberRank() != GUILD_MASTER )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );
				return FALSE;
			}
			if( mIndex == 0 )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1009) );
				return FALSE;
			}
		}
		break;	
	case 4:		// secede
		{
			if( hero->GetGuildMemberRank() != GUILD_MASTER )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );
				return FALSE;
			}
			if( mIndex == 0 )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1009) );
				return FALSE;
			}
		}
		break;
	case 5:		// mark
		{
			if( hero->GetGuildMemberRank() != GUILD_MASTER )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1006) );
				return FALSE;
			}
			if( mIndex == 0 )
			{
				CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(1009) );
				return FALSE;
			}
		}
		break;
	}
	return TRUE;
}


DWORD CGuildUnion::GetSize() const
{
	const int maxSize = sizeof( mData ) / sizeof( *mData );

	int size = 0;

	for( int i = 0; i < maxSize; ++i )
	{
		const sGUILDIDXNAME& data = mData[ i ];

		if( data.dwGuildIdx )
		{
			++size;
		}
	}

	return size;
}
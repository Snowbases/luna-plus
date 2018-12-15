#include "stdafx.h"
#include "GuildManager.h"
#include "Guild.h"
#include "GuildTournamentMgr.h"
#include "GuildUnionManager.h"
#include "GuildFieldWarMgr.h"
#include "GuildScore.h"
#include "GuildMark.h"

#include "MHFile.h"
#include "MapDBMsgParser.h"
#include "UserTable.h"
#include "Network.h"
#include "Player.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "PackedData.h"
#include "ItemManager.h"
#include "Monster.h"
// desc_hseos_패밀리01
// S 패밀리 추가 added by hseos 2007.07.15
#include "../hseos/Family/SHFamilyManager.h"
// 081031 LUJ, 길드원 소환위해 참조
#include "MoveManager.h"
// 080417 LUJ, 길드 스킬 획득위해 참조
#include "../[cc]skill/Server/Manager/SkillManager.h"
#include "../[cc]skill/Server/Info/ActiveSkillInfo.h"
#include "SiegeWarfareMgr.h"


CGuildManager::CGuildManager()
{
	m_GuildRoomHashTable.Initialize(256);
	m_GuildMarkTable.Initialize(128);

	LoadScript();
}

CGuildManager::~CGuildManager()
{
	m_GuildRoomHashTable.SetPositionHead();

	while(CGuild* const pGuild = m_GuildRoomHashTable.GetData())
	{
		delete pGuild;
	}

	m_GuildMarkTable.SetPositionHead();

	while(CGuildMark* pMarkInfo = m_GuildMarkTable.GetData())
	{
		delete pMarkInfo;
	}

	for(	GuildMap::iterator it = mGuildScoreMap.begin();
		mGuildScoreMap.end() != it;
		++it )
	{
		CGuildScore* guild = it->second;

		SAFE_DELETE( guild );
	}
}

void CGuildManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch(Protocol)
	{
	case MP_GUILD_CREATE_SYN:
		{
			SEND_GUILD_CREATE_SYN* pmsg = (SEND_GUILD_CREATE_SYN*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			CreateGuildSyn(pPlayer, pmsg->GuildName, pmsg->Intro);
		}
		break;
	case MP_GUILD_BREAKUP_SYN:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			if( GUILDWARMGR->IsGuildWar( pPlayer->GetGuildIdx() ) )
				return;
			BreakUpGuildSyn(pPlayer);
		}
		break;
	case MP_GUILD_DELETEMEMBER_SYN:
		{
			const MSG_DWORD*	message				= (MSG_DWORD*)pMsg;
			const DWORD			playerIndex			= message->dwObjectID;
			const DWORD			kickedPlayerIndex	= message->dwData;

			CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( playerIndex );

			// 자기 자신 강퇴 금지
			if( !	player							||
					playerIndex	== kickedPlayerIndex )
			{
				return;
			}

			const BYTE rank = player->GetGuildMemberRank();

			// 탈퇴 권한을 가진 랭크인가
			if(	GUILD_MASTER		!= rank &&
				GUILD_VICEMASTER	!= rank )
			{
				SendNackMsg( player, MP_GUILD_DELETEMEMBER_NACK, eGuildErr_DeleteMember_NothaveAuthority );
				break;
			}

			//db update
			GuildDeleteMember( player->GetGuildIdx(), kickedPlayerIndex );
		}
		break;
	case MP_GUILD_ADDMEMBER_SYN:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pMaster = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pMaster == 0)
				return;
			AddMemberSyn(pMaster, pmsg->dwData);		
		}
		break;
	case MP_GUILD_INVITE_ACCEPT:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			AddMemberResult(pPlayer, pmsg->dwData);
		}
		break;
	case MP_GUILD_INVITE_DENY:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			CPlayer* pMaster = (CPlayer*)g_pUserTable->FindUser(pmsg->dwData);
			if(pMaster == 0)
				return;
			MSGBASE msg;
			msg.Category = MP_GUILD;
			msg.Protocol = MP_GUILD_INVITE_DENY;
			pMaster->SendMsg(&msg, sizeof(msg));
		}
		break;
	case MP_GUILD_SECEDE_SYN:
		{
			MSGBASE * pmsg = (MSGBASE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			SecedeSyn(pPlayer);
		}
		break;
	case MP_GUILD_LEVELUP_SYN:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;
			CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(
				message->dwObjectID);

			if( ! player )
			{
				break;
			}
			else if(GUILD_MASTER != player->GetGuildMemberRank())
			{
				break;
			}

			CGuild* const guild = GUILDMGR->GetGuild(
				player->GetGuildIdx());

			if(0 == guild)
			{
				break;
			}

			const LevelSetting* setting	= GetLevelSetting(guild->GetLevel() + 1);

			if( !	setting												||
					setting->mRequiredMoney > player->GetMoney()		||
					setting->mRequiredMasterLevel > player->GetLevel() )
			{
				ASSERT( 0 && "It guess two case. One is client test is failed. Other is hacking" );
				return;
			}

			g_DB.FreeMiddleQuery(
				RGuildSetLevel,
				player->GetID(),
				"EXEC dbo.MP_GUILD_LEVEL_UPDATE %d, %d, %d, %d, %d, %d",
				guild->GetIdx(),
				player->GetID(),
				setting->mRequiredMoney,
				setting->mRequiredScore,
				setting->mRequiredQuest,
				setting->mWarehouseSize);
		}
		break;
	case MP_GUILD_CHANGERANK_SYN:
		{
			MSG_DWORDBYTE* pmsg = (MSG_DWORDBYTE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			ChangeMemberRank(pPlayer, pmsg->dwData, pmsg->bData);
		}
		break;
	case MP_GUILD_GIVENICKNAME_SYN:
		{
			SEND_GUILD_NICKNAME * pmsg = (SEND_GUILD_NICKNAME*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;

			// 090325 ONS String Overflow 방지
			char szBuf[MAX_GUILD_NICKNAME+1];
			SafeStrCpy( szBuf, pmsg->NickName, MAX_GUILD_NICKNAME+1 );

			GiveNickNameSyn(pPlayer, pmsg->TargetIdx, szBuf);
		}
		break;
	case MP_GUILD_CREATE_NOTIFY_TO_MAP:
		{
			SEND_GUILD_CREATE_NOTIFY* pmsg = (SEND_GUILD_CREATE_NOTIFY*)pMsg;
			CGuild* pGuild = RegistGuild( pmsg->GuildInfo, 0 );
			if(pGuild == 0)
				return;
			
			pGuild->AddMember( &( pmsg->mMaster ) );
		}
		break;
	case MP_GUILD_BREAKUP_NOTIFY_TO_MAP:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			DoBreakUp(pmsg->dwObjectID);
		}
		break;
	case MP_GUILD_DELETEMEMBER_NOTIFY_TO_MAP:
		{
			MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;
			DoDeleteMember(pmsg->dwObjectID, pmsg->dwData, eGuild_Delete, pmsg->Name);
		}
		break;
	case MP_GUILD_ADDMEMBER_NOTIFY_TO_MAP:
		{
			SEND_GUILD_MEMBER_INFO* pmsg = (SEND_GUILD_MEMBER_INFO*)pMsg;
			CGuild* pGuild = GetGuild(pmsg->GuildIdx);
			if(pGuild == 0)
			{
				ASSERTMSG(0, "Different Guild Info!");
				return;
			}
			
			GUILDMEMBERINFO& member = pmsg->MemberInfo;
			
			if(pGuild->AddMember(&member) == FALSE)
			{
				char buf[128];
				sprintf(buf, "Add Notify Failed! GuildIdx : %u, MemberIdx : %u", pmsg->GuildIdx, pmsg->MemberInfo.MemberIdx);
				ASSERTMSG(0, buf);
			}			
		}
		break;
	case MP_GUILD_SECEDE_NOTIFY_TO_MAP:
		{
			MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
			DoDeleteMember(pmsg->dwObjectID, pmsg->dwData, eGuild_Secede, "2000.01.01");
		}
		break;
	case MP_GUILD_LEVELUP_NOTIFY_TO_MAP:
		{
			const MSG_DWORD3* const message = (MSG_DWORD3*)pMsg;			
			const DWORD guildIndex = message->dwData1;
			const DWORD guildScore = message->dwData2;
			const LEVELTYPE guildLevel = LEVELTYPE(message->dwData3);

			if(CGuild* const guild = GetGuild(guildIndex))
			{
				if(guild->GetLevel() < guildLevel)
				{
					MSG_DWORD2 sendMessage;
					ZeroMemory(
						&sendMessage,
						sizeof(sendMessage));
					sendMessage.Category = MP_GUILD;
					sendMessage.Protocol = MP_GUILD_LEVELUP_ACK;
					sendMessage.dwData1 = guildScore;
					sendMessage.dwData2 = guildLevel;

					guild->SendMsgToAll(
						&sendMessage,
						sizeof(sendMessage));
					guild->SetLevel(
						guildLevel);
				}
			}

			break;
		}
		break;
	case MP_GUILD_CHANGERANK_NOTIFY_TO_MAP:
		{
			MSG_DWORDBYTE* pmsg = (MSG_DWORDBYTE*)pMsg;			
			CGuild* pGuild = GetGuild(pmsg->dwObjectID);
			if(pGuild == 0)
			{
				char buf[128];
				sprintf(buf, "ChangeRank Notify Failed! GuildIdx: %d", pmsg->dwObjectID);
				ASSERTMSG(0, buf);
				break;
			}
			if(pGuild->ChangeMemberRank(pmsg->dwData, pmsg->bData) == FALSE)
			{
				ASSERTMSG(0, "Notify Change Rank Err, Must Check.");
				break;
			}
		}
		break;
	case MP_GUILD_MEMBERLEVEL_NOTIFY_TO_MAP:
		{
			MSG_DWORD3 * pmsg = (MSG_DWORD3*)pMsg;
			CGuild* pGuild = GetGuild(pmsg->dwData1);
			if(pGuild == 0)
				return;
			pGuild->SetMemberLevel(pmsg->dwData2, (LEVELTYPE)pmsg->dwData3);
		}
		break;
	case MP_GUILD_GIVENICKNAME_NOTIFY_TO_MAP:
		{
			SEND_GUILD_NICKNAME * pmsg = (SEND_GUILD_NICKNAME*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->TargetIdx);
			if(pPlayer == 0)
				return;

			pPlayer->SetNickName(pmsg->NickName);

			// SEND TO 
			SEND_GUILD_NICKNAME msg;
			msg.Category = MP_GUILD;
			msg.Protocol = MP_GUILD_GIVENICKNAME_ACK;
			msg.TargetIdx = pmsg->TargetIdx;
			SafeStrCpy(msg.NickName, pmsg->NickName, MAX_GUILD_NICKNAME+1);
			PACKEDDATA_OBJ->QuickSend(pPlayer, &msg, sizeof(msg));	
		}
		break;
	case MP_GUILD_LOGINFO_NOTIFY_TO_MAP:
		{
			SEND_GUILDMEMBER_LOGININFO * pmsg = (SEND_GUILDMEMBER_LOGININFO *)pMsg;
			CGuild* pGuild = GetGuild(pmsg->dwObjectID);
			if(pGuild == 0)
				return;
			if(pGuild->IsMember(pmsg->MemberIdx) == FALSE)
				return;
			// 081031 LUJ, 맵 정보 추가
			pGuild->SetLogInfo(pmsg->MemberIdx, pmsg->bLogIn, pmsg->mMapType);
		}
		break;
		///////////////////////////////////////////////////////////////////////////
		//MARK
	case MP_GUILDMARK_UPDATE_SYN:
		{
			MSG_GUILDMARK_IMG * pmsg	= ( MSG_GUILDMARK_IMG* )pMsg;
			CPlayer*			pPlayer = ( CPlayer* )g_pUserTable->FindUser(pmsg->dwObjectID );

			if( !	pPlayer		||
					pPlayer->GetGuildMemberRank() != GUILD_MASTER )
			{
				break;
			}

			CGuild* const guild = GetGuild(
				pPlayer->GetGuildIdx());

			if(0 == guild)
			{
				SendNackMsg(
					pPlayer,
					MP_GUILDMARK_UPDATE_NACK,
					eGuildErr_Mark_NoGuild);
				break;
			}

			const CGuildManager::LevelSetting* setting = GetLevelSetting( guild->GetLevel() );

			if( ! setting ||
				! setting->mGuildMark )
			{
				ASSERT( 0 && "It maybe be hacking" );
				break;
			}

			//RegistMarkSyn(pmsg->GuildIdx, pmsg->imgData, pmsg->dwObjectID);
			GuildMarkRegist( pmsg->GuildIdx, pmsg->imgData, pmsg->dwObjectID );
		}
		break;
	case MP_GUILDMARK_REQUEST_SYN:
		{
			MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;

			RequestMark(pPlayer, pmsg->dwData);
		}
		break;
	case MP_GUILDMARK_NOTIFY_TO_MAP:
		{
			MSG_GUILDMARK_IMG* pmsg = (MSG_GUILDMARK_IMG*)pMsg;
			CGuild* pGuild = GetGuild(pmsg->GuildIdx);
			if(pGuild == 0)
				return;
			DoRegistMark(pGuild, pmsg->MarkName, pmsg->imgData);
		}
		break;
	// 091111 ONS 길드 문장 삭제 
	case MP_GUILDMARK_DEL_SYN:
		{
			MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			
			GuildMarkDelete(pmsg->dwData, pmsg->dwObjectID);
		}
		break;
	case MP_GUILD_NOTE_SYN:
		{
			MsgGuildNoteSyn( pMsg );
			
		}
		break;
	case MP_GUILD_NOTICE_SYN:
		{
			MSG_GUILDNOTICE*	message = (MSG_GUILDNOTICE*)pMsg;
			CPlayer*			player = (CPlayer*)g_pUserTable->FindUser( message->dwObjectID );

			if( ! player )
			{
				return;
			}

			const BYTE rank = player->GetGuildMemberRank();

			if( GUILD_MASTER		!= rank &&
				GUILD_VICEMASTER	!= rank )
			{
				return;
			}

			const DWORD		guildIndex	= player->GetGuildIdx();
			CGuildScore*	guild		= GetGuildScore( guildIndex );

			if( ! guild )
			{
				SendNackMsg( player, MP_GUILD_NOTICE_NACK, eGuildErr_NoGuild );
				return;
			}

			char szBuf[MAX_GUILD_NOTICE+1];
			SafeStrCpy(szBuf, message->Msg, MAX_GUILD_NOTICE+1);
			if(IsCharInString(szBuf, "'"))
			{
				SendNackMsg( player, MP_GUILD_NOTICE_NACK, eGuildErr_Notice_FilterCharacter );
				return;
			}

			GuildUpdateNotice( guildIndex, /*message->Msg*/szBuf, player->GetID() );

			// 공지를 브로드캐스팅하자
			{
				message->Protocol = MP_GUILD_NOTICE_NOTIFY;

				g_Network.Send2AgentServer( ( char* )message, message->GetMsgLength() );
			}
			
			// 길드에게도 보내자
			{
				message->Protocol = MP_GUILD_NOTICE_ACK;

				guild->Send( message, message->GetMsgLength() );
			}
		}
		break;
	case MP_GUILD_NOTICE_NOTIFY:
		{
			//MsgGuildNoticeNotify(pMsg);

			MSG_GUILDNOTICE* message = ( MSG_GUILDNOTICE* )pMsg;

			CGuildScore* guild = GetGuildScore( message->dwGuildId );

			if( guild )
			{
				message->Protocol	= MP_GUILD_NOTICE_ACK;

				guild->Send( message, message->GetMsgLength() );
			}
		}
		break;
		//SW060713 문하생
	case MP_GUILD_ADDSTUDENT_SYN:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pMaster = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if( ! pMaster )
			{
				return;
			}
			
			CGuild*		pGuild		= GetGuild(pMaster->GetGuildIdx());
			const DWORD guestIndex	= pmsg->dwData;
			CPlayer*	pTarget		= (CPlayer*)g_pUserTable->FindUser( guestIndex );

			// 검사
			{
				MSG_NAME_DWORD message;
				ZeroMemory( &message, sizeof( message ) );
				message.dwObjectID	= pmsg->dwObjectID;
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_ADDSTUDENT_NACK;

				if( ! pTarget )
				{
					message.dwData	= eGuildErr_AddMember_NotPlayer;
				}
				else if( pTarget->GetGuildIdx() )
				{
					message.dwData	= eGuildErr_AddMember_OtherGuild;
				}
				else if(pTarget->GetLevel() >= GUILD_STUDENT_MAX_LEVEL)
				{
					message.dwData	= eGuildErr_AddStudent_TooHighLevelAsStudent;
				}
				else if(pMaster->GetGuildMemberRank() < GUILD_SENIOR)
				{
					message.dwData	= eGuildErr_AddMember_NothaveAuthority;
				}			
				//else if( pGuild->IsMember( guestIndex ) )
				//{
				//	message.dwData	= eGuildErr_AddMember_AlreadyMember;
				//}
				if( FALSE == pGuild->CanAddStudent() )	//문하생 인원이 다 찼거나
				{
					message.dwData	= eGuildErr_AddMember_FullMember;
				}

				// 값에 0이 있으면 검사 성공이다
				if( message.dwData )
				{
					pMaster->SendMsg( &message, sizeof( message ) );
					return;
				}
			}

			{
				MSGBASE message;
				message.Category = MP_GUILD;
				message.Protocol = MP_GUILD_ADDSTUDENT_INVITE;

				pMaster->SendMsg( &message, sizeof( message ) );
			}
			
			{
				SEND_GUILD_INVITE message;
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_INVITE_AS_STUDENT;
				message.MasterIdx	= pMaster->GetID();

				SafeStrCpy( message.MasterName, pMaster->GetObjectName(), sizeof( message.MasterName ) );
				SafeStrCpy( message.GuildName, pGuild->GetGuildName(), sizeof( message.GuildName ) );

				pTarget->SendMsg( &message, sizeof( message )) ;
			}
		}
		break;
	case MP_GUILD_INVITE_AS_STUDENT_ACCEPT:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			AddStudentResult(pPlayer, pmsg->dwData);
		}
		break;
	case MP_GUILD_OPEN_WAREHOUSE_SYN:
		{
			const MSGBASE*	message		= ( MSGBASE* )pMsg;
			const DWORD		playerIndex = message->dwObjectID;
			CPlayer*		player		= ( CPlayer* )g_pUserTable->FindUser( playerIndex );

			if( player )
			{
				GuildOpenWarehouse( player->GetGuildIdx(), playerIndex, g_pServerSystem->GetMapNum() );
			}

			break;
		}
	case MP_GUILD_CLOSE_WAREHOUSE:
		{
			const MSGBASE*	message		= ( MSGBASE* )pMsg;
			const DWORD		playerIndex = message->dwObjectID;
			CPlayer*		player		= ( CPlayer* )g_pUserTable->FindUser( playerIndex );

			if( player )
			{
				GuildCloseWarehouse( player->GetGuildIdx(), playerIndex, g_pServerSystem->GetMapNum() );
			}			

			break;
		}
	case MP_GUILD_SET_WAREHOUSE_RANK_SYN:
		{
			const MSG_GUILD_SET_WAREHOUSE_RANK* message		= ( MSG_GUILD_SET_WAREHOUSE_RANK* )pMsg;
			const DWORD							guildIndex	= message->mGuildIndex;
			const DWORD							playerIndex	= message->dwObjectID;
			CPlayer*							player		= ( CPlayer* )g_pUserTable->FindUser( playerIndex );
			
			if( !	GetGuildScore( guildIndex )			||
				!	player								||
					player->GetGuildIdx() != guildIndex	)
			{
				ASSERT( 0 && "it doubt with hacking" );
				return;
			}

			const BYTE rank = player->GetGuildMemberRank();

			if( rank != GUILD_MASTER		&&
				rank != GUILD_VICEMASTER	)
			{
				ASSERT( 0 && "it doubt with hacking" );
				return;
			}

			GuildSetWarehouseRank( guildIndex, playerIndex, message->mRank );
			break;
		}
	case MP_GUILD_RETIRE_ACK:
		{
			const MSG_DWORD* message = ( MSG_DWORD* )pMsg;

			CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( message->dwObjectID );

			if( !	player	||
					player->GetGuildMemberRank() == GUILD_STUDENT )
			{
				return;
			}

			const DWORD	guildIndex	= player->GetGuildIdx();
			CGuild*		guild		= GUILDMGR->GetGuild( guildIndex );

			// 주의: 공성전 여부는 클라이언트에서 검사하지 않는다. 공성전이 추가될 때 유의하자

			if( !	guild									||
					guild->GetUnionIndex()					||
					GUILDWARMGR->IsGuildWar( guildIndex ) )
			{
				// 클라이언트에서 검사했음
				return;
			}

			GuildChangeMaster( guildIndex, player->GetID(), GUILD_MASTER, GUILD_MEMBER );
			
			break;
		}
	case MP_GUILD_CHANGE_MASTER_TO_AGENT:
		{
			MSG_DWORD2 message;
			memcpy( &message, pMsg, sizeof( message ) );
			message.Protocol = MP_GUILD_CHANGE_MASTER_TO_USER;

			const DWORD	guildIndex = message.dwData1;
			const DWORD	newMasterIndex = message.dwData2;

			CGuild* guild = GetGuild( guildIndex );

			if(0 == guild)
			{
				break;
			}

			guild->SendMsgToAll( &message, sizeof( message ) );

			// 이전 마스터의 등급을 회원으로 낮춘다
			{
				const DWORD	oldMasterIndex = guild->GetMasterIdx();

				if(GUILDMEMBERINFO*	info = guild->GetMemberInfo(oldMasterIndex))
				{
					info->Rank = GUILD_MEMBER;
				}

				if(CPlayer* player = (CPlayer*)g_pUserTable->FindUser(oldMasterIndex))
				{
					player->SetGuildMemberRank( GUILD_MEMBER );
				}

				guild->UpdateSkill( oldMasterIndex );
			}

			// 새로운 마스터의 등급또한 마스터로 올린다
			{
				if(GUILDMEMBERINFO* info = guild->GetMemberInfo( newMasterIndex ))
				{
					info->Rank = GUILD_MASTER;
					SafeStrCpy(
						guild->GetInfo().MasterName,
						info->MemberName,
						sizeof(guild->GetInfo().MasterName));
				}

				if(CPlayer* player = (CPlayer*)g_pUserTable->FindUser(newMasterIndex))
				{
					player->SetGuildMemberRank( GUILD_MASTER );
				}

				guild->UpdateSkill( newMasterIndex );
				guild->SetMasterIndex( newMasterIndex );
			}

			break;
		}
		// 080225 LUJ, 회원의 직업 정보를 갱신한다
	case MP_GUILD_SET_MEMBER_TO_MAP:
		{
			SEND_GUILD_MEMBER_INFO* message = ( SEND_GUILD_MEMBER_INFO* )pMsg;

			CGuild* guild = GetGuild( message->GuildIdx );

			if( ! guild	)
			{
				break;
			}

			const GUILDMEMBERINFO& member = message->MemberInfo;

			if( ! guild->SetMember( member ) )
			{
				break;
			}

			message->Protocol	= MP_GUILD_SET_MEMBER_ACK;

			guild->SendMsgToAll( message, sizeof( *message ) );
			break;
		}
		// 080417 LUJ, 길드 스킬 획득
	case MP_GUILD_ADD_SKILL_SYN:
		{
			const MSG_SKILL_UPDATE* message = ( MSG_SKILL_UPDATE* )	pMsg;
			CPlayer*				player	= ( CPlayer* )			g_pUserTable->FindUser( message->dwObjectID );

			if( ! player )
			{
				break;
			}

			CGuild* guild = GetGuild( player->GetGuildIdx() );
			
			// 080417 LUJ, 마스터인지 검사
			if( !	guild									||
					guild->GetMasterIdx() != player->GetID() )
			{
				SendNackMsg( player, MP_GUILD_ADD_SKILL_NACK, eGuildErr_NoMaster );
				break;
			}

			const SKILL_BASE& skill	= message->SkillBase;

			if(const LEVELTYPE guildSkillLevel = guild->GetSkillLevel(skill.wSkillIdx))
			{
				if(skill.Level == guildSkillLevel)
				{
					SendNackMsg(
						player,
						MP_GUILD_ADD_SKILL_NACK,
						eGuildErr_Skill_InvalidLevel);
					break;
				}
			}
			else
			{
				const LevelSetting* setting = GetLevelSetting( guild->GetLevel() );

				if( !	setting	||
						setting->mSkillSize <= guild->GetSkillSize() )
				{
					SendNackMsg( player, MP_GUILD_ADD_SKILL_NACK, eGuildErr_Skill_Max );
					break;
				}
			}

			// 080417 LUJ, 길드 설정에 있는 스킬인지 검사
			{
				const SkillSettingMap* settingMap = GetSkillSetting( guild->GetLevel() );

				// 080417 LUJ,	현 레벨에 설정 조건이 없는 경우 최대 레벨의 설정을 쓴다
				//				스킬 스크립트의 길드 레벨 설정에 건너뛰는 번호가 있을 경우 잘못된 체크를 할 것이다
				if( ! settingMap )
				{
					settingMap = GetMaxLevelSkillSetting();
					
					if( ! settingMap )
					{
						SendNackMsg( player, MP_GUILD_ADD_SKILL_NACK, eGuildErr_NoScript );
						break;
					}
				}

				SkillSettingMap::const_iterator settingMap_it	= settingMap->find( skill.wSkillIdx );

				if( settingMap->end() == settingMap_it )
				{
					SendNackMsg( player, MP_GUILD_ADD_SKILL_NACK, eGuildErr_Skill_NotExist );
					break;
				}

				const SkillSetting& setting = settingMap_it->second;

				if( setting.mLevel < skill.Level )
				{
					SendNackMsg( player, MP_GUILD_ADD_SKILL_NACK, eGuildErr_Skill_InvalidLevel );
					break;
				}
			}

			// 080417 LUJ, 획득 가능한 스킬인지 검사
			{
				const cActiveSkillInfo* const skillInfo = SKILLMGR->GetActiveInfo( skill.wSkillIdx - 1 + skill.Level );

				if(0 == skillInfo ||
					player->GetMoney() < skillInfo->GetInfo().TrainMoney)
				{
					SendNackMsg( player, MP_GUILD_ADD_SKILL_NACK, eGuildErr_InsufficentMoney );
					break;
				}

				// 080417 LUJ, 각각의 맵은 총점이 얼마인지 모른다. 따라서 길드 점수 비교는 서버에서 한다. DB를 거치므로 이 검사를 최종적으로 해야한다
				g_DB.FreeMiddleQuery(
					RGuildAddSkill,
					player->GetID(),
					"EXEC MP_GUILD_ADD_SKILL %d, %d, %d, %d, %d",
					guild->GetIdx(),
					skillInfo->GetInfo().TrainPoint,
					skillInfo->GetInfo().TrainMoney,
					skill.wSkillIdx,
					skill.Level );
			}

			break;
		}
		// 080417 LUJ, 획득 길드 스킬을 맵에 전파
	case MP_GUILD_ADD_SKILL_ACK_TO_MAP:
		{
			MSG_SKILL_UPDATE message;
			memcpy( &message, pMsg, sizeof( message ) );
			// 080417 LUJ, 프로토콜을 바꾸지 않으면 에이전트에서 브로드캐스팅해버린다
			message.Protocol = MP_GUILD_ADD_SKILL_ACK;

			CGuild* guild = GetGuild( message.dwObjectID );

			if( guild )
			{
				const SKILL_BASE& skill = message.SkillBase;

				guild->AddSkill( skill.wSkillIdx, skill.Level );
				guild->SendMsgToAll( &message, sizeof( message ) );
			}

			break;
		}
		// 080602 LUJ, 길드 점수를 새로고침한다
	case MP_GUILD_SCORE_UPDATE_TO_MAP:
		{
			const MSGBASE*	message		= ( MSGBASE* )pMsg;
			const DWORD		guildIndex	= message->dwObjectID;

			GuildAddScore( guildIndex, 0 );
			break;
		}
	}
}

// 081006 LUJ, 길드 정보를 여러번 변경할 수 있도록 수정
CGuild* CGuildManager::RegistGuild( const GUILDINFO& info, MONEYTYPE GuildMoney)
{
	CGuild* pGuild = GetGuild( info.GuildIdx );

	if( pGuild )
	{	
		pGuild->Update( info );
	}	
	else
	{	
		pGuild = new CGuild( info, GuildMoney );

		m_GuildRoomHashTable.Add(pGuild, info.GuildIdx);
	}	

	return pGuild;
}

void CGuildManager::CreateGuildSyn(CPlayer* player, const char* guildName, const char* guildIntro )
{
	const LevelSetting* setting = GetLevelSetting( 1 );

	// 메시지를 전송하지 않는 오류는 정상적인 경우 클라이언트에서 체크되어 전달되지 않기 때문이다
	if( !	setting												||
			player->GetGuildIdx()								||
			player->GetLevel() < setting->mRequiredMasterLevel	||
			player->GetMoney() < setting->mRequiredMoney )
	{
		return;
	}
	else if( ! CanEntryGuild(player->GetGuildCanEntryDate() ) )
	{
		char	date	[ 11 ]	= { 0 };
		char	number	[ 11 ]	= { 0 };
		char*	number_ch		= number;

		SafeStrCpy( date, player->GetGuildCanEntryDate(), sizeof( date ) );

		// 타이픈이 들어있나 보고.. 숫자만 따로 집어넣자..
		for( const char* date_ch = date; *date_ch; ++date_ch  )
		{
			if( isdigit( *date_ch ) )
			{
				*number_ch++	= *date_ch;
			}
		}

		MSG_DWORD2	message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_CREATE_NACK;
		message.dwData1		= eGuildErr_Create_Unable_Entry_Data;
		message.dwData2		= atoi( number );

		player->SendMsg( &message, sizeof( message ) );
		return;
	}
	else if( ! IsVaildGuildName( guildName ) )
	{
		MSG_DWORD2	message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_CREATE_NACK;
		message.dwData1		= eGuildErr_Create_Invalid_Name;
		
		player->SendMsg( &message, sizeof( message ) );
		return;
	}

	char name	[ MAX_GUILD_NAME + 1]	= { 0 };
	char intro	[ MAX_GUILD_INTRO + 1 ]	= { 0 };

	SafeStrCpy( name, guildName, sizeof( name ) );
	SafeStrCpy( intro, guildIntro, sizeof( intro ) );

	GuildCreate( player->GetID(), player->GetObjectName(), player->GetLevel(), name, intro, GUILD_MASTER );

	// desc_hseos_패밀리01
	// S 패밀리 추가 added by hseos 2007.07.15
	// 패밀리 멤버에게 알리기
	g_csFamilyManager.SRV_UpdateMapPlayerInfo( player, name );
	// E 패밀리 추가 added by hseos 2007.07.15
}

void CGuildManager::CreateGuildResult(LEVELTYPE MasterLvl, const GUILDINFO& info)
{
	CGuild* pGuild = RegistGuild(info, 0);

	if(pGuild == 0)
	{
		return;
	}

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(info.MasterIdx);

	if(pPlayer)
	{
		pPlayer->SetGuildInfo(info.GuildIdx, GUILD_MASTER, info.GuildName, 0);

		//ack
		SEND_GUILD_CREATE_ACK message;
		message.Category = MP_GUILD;
		message.Protocol = MP_GUILD_CREATE_ACK;
		message.GuildIdx = info.GuildIdx;
		SafeStrCpy(message.GuildName, info.GuildName, sizeof( message.GuildName ) );
		pPlayer->SendMsg(&message, sizeof(message));

		SendGuildName(pPlayer, info.GuildIdx, info.GuildName);
	}

	{
		SEND_GUILD_CREATE_NOTIFY message;
		message.Category				= MP_GUILD;
		message.Protocol				= MP_GUILD_CREATE_NOTIFY_TO_MAP;
		message.GuildInfo.GuildIdx		= info.GuildIdx;
		message.GuildInfo.GuildLevel	= 1;
		message.GuildInfo.MasterIdx		= info.MasterIdx;
		message.GuildInfo.UnionIdx		= 0;
		message.MasterLvl				= MasterLvl;
		message.GuildInfo.MarkName		= info.MarkName;

		// 080225 LUJ, 설정할 회원 정보
		{
			GUILDMEMBERINFO& member = message.mMaster;

			member.MemberIdx	= info.MasterIdx;
			SafeStrCpy( member.MemberName, info.MasterName, sizeof( member.MemberName ) );
			member.Memberlvl	= MasterLvl;
			member.Rank			= GUILD_MASTER;
			member.bLogged		= TRUE;

			const CHARACTER_TOTALINFO& info = pPlayer->GetCharacterTotalInfo();

			member.mJobGrade	= info.JobGrade;
			member.mRace		= info.Race;
			memcpy( member.mJob, info.Job, sizeof( member.mJob ) );			
		}

		SafeStrCpy(message.GuildInfo.MasterName, info.MasterName, sizeof( message.GuildInfo.MasterName ) );
		SafeStrCpy(message.GuildInfo.GuildName, info.GuildName, sizeof( message.GuildInfo.GuildName ) );

		g_Network.Send2AgentServer((char*)&message, sizeof(message));

		pGuild->AddMember( &( message.mMaster ) );
	}	

	AddPlayer( pPlayer );

	LogGuild( pGuild->GetMasterIdx(), pGuild->GetIdx(), eLogGuild_GuildCreate );
}

void CGuildManager::BreakUpGuildSyn(CPlayer* pMaster)
{
	if(pMaster->GetGuildMemberRank() != GUILD_MASTER)
	{
		SendNackMsg( pMaster, MP_GUILD_BREAKUP_NACK, eGuildErr_BreakUp);
		return;
	}
	
	GuildBreakUp(pMaster->GetGuildIdx(), pMaster->GetID());
}

void CGuildManager::BreakUpGuildResult(CPlayer* pMaster, DWORD GuildIdx, char* EntryDate)
{
	if(0 == pMaster)
	{
		return;
	}

	const BYTE rt = DoBreakUp(GuildIdx);

	if( rt )
	{
		SendNackMsg( pMaster, MP_GUILD_BREAKUP_NACK, rt);
		return;
	}

	LogGuild(
		pMaster->GetID(),
		GuildIdx,
		eGuildLog_GuildBreakUp);

	// 페널티 부여
	{
		const EXPTYPE pointForLevel = GAMERESRCMNGR->GetMaxExpPoint(pMaster->GetLevel());
		const EXPTYPE minusExp		= EXPTYPE( pointForLevel * 0.05 );

		pMaster->ReduceExpPoint(minusExp, eExpLog_LosebyBreakupGuild);

		SendGuildEntryDate(pMaster, EntryDate);
	}	

	{
		MSGBASE message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_BREAKUP_NOTIFY_TO_MAP;
		message.dwObjectID	= GuildIdx;

		g_Network.Send2AgentServer((char*)&message, sizeof(message));
	}	
}


BYTE CGuildManager::DoBreakUp( DWORD GuildIdx )
{
	// 길드해채시 공성전 성점령 정보도 초기화 시켜주자.
	SIEGEWARFAREMGR->GuildBreakUp( GuildIdx ) ;

	CGuild* pGuild = GetGuild(GuildIdx);
	if(!pGuild)
	{
		ASSERT(0);
		return eGuildErr_BreakUp; 
	}

	if(pGuild->GetMarkName())
	{
		DeleteMark(pGuild->GetMarkName());
	}

	pGuild->BreakUp();
	m_GuildRoomHashTable.Remove(GuildIdx);

	SAFE_DELETE( pGuild );

	// 길드도 지운다...
	{
		GuildMap::const_iterator it = mGuildScoreMap.find( GuildIdx );

		if( mGuildScoreMap.end() != it )
		{
			const CGuildScore* guild = it->second;

			SAFE_DELETE( guild );

			mGuildScoreMap.erase( GuildIdx );
		}
	}

	return 0;
}


CGuild* CGuildManager::GetGuild(DWORD GuildIdx)
{
	return m_GuildRoomHashTable.GetData(GuildIdx);
}


CGuild* CGuildManager::GetGuildFromMasterIdx( DWORD dwMasterIdx )
{
	CGuild* pGuild = NULL;
	m_GuildRoomHashTable.SetPositionHead();
	while( (pGuild = m_GuildRoomHashTable.GetData()) != NULL)
	{
		if( pGuild->GetMasterIdx() == dwMasterIdx )
			return pGuild;
	}
	return NULL;
}


CGuildMark* CGuildManager::GetMark(MARKNAMETYPE MarkName)
{
	CGuildMark* pMark = NULL;
	pMark = m_GuildMarkTable.GetData(MarkName);
	return pMark;
}

void CGuildManager::SendNackMsg(CPlayer* pPlayer, BYTE Protocol, BYTE errstate)
{
	MSG_BYTE nmsg;
	nmsg.Category	= MP_GUILD;
	nmsg.Protocol	= Protocol;
	nmsg.bData		= errstate;
	pPlayer->SendMsg(&nmsg, sizeof(nmsg));
}

BOOL CGuildManager::IsVaildGuildName( const char* GuildName )
{
	CGuild * pInfo;
	m_GuildRoomHashTable.SetPositionHead();
	while( (pInfo = m_GuildRoomHashTable.GetData() ) != NULL)
	{
		if(strcmp(pInfo->GetGuildName(), GuildName) == 0)
			return FALSE;
	}
	if(strlen(GuildName) > MAX_GUILD_NAME)
		return FALSE;
	return TRUE;
}

void CGuildManager::DeleteMemberResult(DWORD GuildIdx, DWORD MemberIDX, char* EntryDay)
{
	CGuild* pGuild = GetGuild(GuildIdx);
	if(pGuild == 0)
		return;

	const BYTE err = DoDeleteMember(GuildIdx, MemberIDX, eGuild_Delete, EntryDay);
	if(err)
	{
		ASSERT(0);
		CPlayer* pMaster = (CPlayer*)g_pUserTable->FindUser(pGuild->GetMasterIdx());

		if(pMaster )
		{
			SendNackMsg( pMaster, MP_GUILD_DELETEMEMBER_NACK, err);
		}

		return;
	}

	LogGuild(MemberIDX, GuildIdx, eGuildLog_MemberBan);

	{
		MSG_NAME_DWORD message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_DELETEMEMBER_NOTIFY_TO_MAP;
		message.dwObjectID	= GuildIdx;
		message.dwData		= MemberIDX;
		SafeStrCpy(message.Name, EntryDay, 11);
		g_Network.Send2AgentServer((char*)&message, sizeof(message));	
	}

	// 071213 LUJ, 탈퇴 시에 길드 점수 계산에서 빼야 함
	{
		CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( MemberIDX );

		if( player )
		{
			//090622 pdy 탈퇴 제명시 길드인덱스가 초기화되어있어 처리가 안되던 버그 수정 
			player->SetGuildIdx( GuildIdx ) ;
			RemovePlayer( player );
			player->SetGuildIdx( 0 ) ;
		}
	}	
}


BYTE CGuildManager::DoDeleteMember(DWORD GuildIDX, DWORD MemberIDX, BYTE bType, char* EntryDay)
{
	CGuild* pGuild = m_GuildRoomHashTable.GetData(GuildIDX);
	if(pGuild == 0)
	{
		ASSERT(0);
		return eGuildErr_Err;
	}
	if(pGuild->IsMember(MemberIDX) == FALSE)
	{
		ASSERT(0);
		return eGuildErr_DeleteMember;
	}

	//member delete
	if(pGuild->DeleteMember(MemberIDX, bType) == FALSE)
	{
		ASSERT(0);
		return eGuildErr_Err;
	}	

	//TO DO 호칭도 삭제 추가
	CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(MemberIDX);
	if(pMember)
	{
		pMember->SetGuildInfo(0, GUILD_NOTMEMBER, "", 0);
		pMember->SetNickName("");
		SendGuildEntryDate(pMember, EntryDay);			
		MSG_DWORDBYTE msg;
		msg.Category = MP_GUILD;
		msg.Protocol = MP_GUILD_DELETEMEMBER_ACK;
		msg.dwData = MemberIDX;
		msg.bData = eGuild_Delete;
		pMember->SendMsg(&msg, sizeof(msg));

		MarkChange(pMember, 0, 0);
	}

	pGuild->SendDeleteMsg(MemberIDX, eGuild_Delete);

	return 0;
}

void CGuildManager::LoadMembers(GUILDMEMBERINFO_ID* pInfo)
{
	CGuild * pGuild = GetGuild(pInfo->GuildIdx);
	// 081006 LUJ, 정보가 없으면 생성한다
	if(pGuild == 0)
	{
		GUILDINFO guildInfo = { 0 };
		guildInfo.GuildIdx	= pInfo->GuildIdx;

		pGuild = new CGuild( guildInfo, 0 );

		m_GuildRoomHashTable.Add(pGuild, pInfo->GuildIdx);
	}
	pGuild->AddMember(&pInfo->MemberInfo);
}

void CGuildManager::LoadMark(MARKNAMETYPE MarkName, char* pImgData)
{
	if(m_GuildMarkTable.GetData(MarkName))
	{
		ASSERTMSG(0, "LoadMark");
		return;
	}
	char Img[GUILDMARK_BUFSIZE];
	char tempBuf[3] = {0,};
	int curpos = 0;
	for(int n=0;n<GUILDMARK_BUFSIZE;++n)
	{
		strncpy(tempBuf,&pImgData[curpos],2); // "FF"
		Img[n] = HexToByte(tempBuf);
		curpos += 2;
	}
	CGuildMark* pMark = new CGuildMark;
	pMark->Init(MarkName, Img);

	m_GuildMarkTable.Add(pMark, MarkName);
}

void CGuildManager::AddMemberSyn(CPlayer* host, DWORD guestPlayerIndex )
{
	CPlayer*	guest = (CPlayer*)g_pUserTable->FindUser( guestPlayerIndex );
	CGuild*		guild = GetGuild( host->GetGuildIdx() );

	// 검사
	{
		MSG_NAME_DWORD	message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_ADDMEMBER_NACK;
		message.dwObjectID	= host->GetID();

		if( 0 == guest )
		{
			message.dwData = eGuildErr_AddMember_NotPlayer;
		}
		else if( guest->GetGuildIdx() )
		{
			message.dwData = eGuildErr_AddMember_OtherGuild;
		}
		else if( GUILD_MEMBER == host->GetGuildMemberRank() )
		{
			message.dwData = eGuildErr_AddMember_NothaveAuthority;
		}
		else if( ! guild->CanAddMember() )
		{
			message.dwData = eGuildErr_AddMember_FullMember;
		}
		else if( ! CanEntryGuild(guest->GetGuildCanEntryDate()))
		{
			message.dwData		= eGuildErr_AddMember_TargetNotDay;
			SafeStrCpy( message.Name, guest->GetGuildCanEntryDate(), sizeof( message.Name ) );
		}		

		// 에러 코드가 있으면 알린다
		if( message.dwData )
		{
			host->SendMsg( &message, sizeof( message ) );
			return;
		}
	}

	// 요청 측에 알려준다
	{
		MSGBASE message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_ADDMEMBER_INVITE;
		
		host->SendMsg( &message, sizeof( message ) );
	}
	
	{
		SEND_GUILD_INVITE message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_INVITE;
		message.MasterIdx	= host->GetID();
		SafeStrCpy( message.MasterName, host->GetObjectName(),	sizeof( message.MasterName ) );
		SafeStrCpy( message.GuildName,	guild->GetGuildName(),	sizeof( message.GuildName ) );

		guest->SendMsg( &message, sizeof( message ) );
	}
}

void CGuildManager::AddMemberResult(CPlayer* pTarget, DWORD MasterIdx)
{
	CPlayer* pMaster = (CPlayer*)g_pUserTable->FindUser(MasterIdx);
	
	if(pMaster == 0)
	{
		SendNackMsg( pTarget, MP_GUILD_INVITE_ACCEPT_NACK, eGuildErr_InviteApt_NoMaster);
		return;
	}

	CGuild*	pGuild	= GetGuild(pMaster->GetGuildIdx());

	if(pGuild == 0)
	{
		SendNackMsg( pTarget, MP_GUILD_INVITE_ACCEPT_NACK, eGuildErr_Err);
		return;
	}
	else if(pGuild->IsMember(pTarget->GetID()) == TRUE)
	{
		SendNackMsg( pTarget, MP_GUILD_INVITE_ACCEPT_NACK, eGuildErr_Err);
		return;
	}
	else if(pGuild->CanAddMember() == FALSE)
	{
		SendNackMsg( pMaster, MP_GUILD_INVITE_ACCEPT_NACK, eGuildErr_AddMember_FullMember);
		return;
	}

	//db update
	GuildAddMember(pMaster->GetGuildIdx(), pTarget->GetID(), pGuild->GetGuildName(), GUILD_MEMBER);

	pTarget->SetGuildInfo(pMaster->GetGuildIdx(), GUILD_MEMBER, pGuild->GetGuildName(), pGuild->GetMarkName());
	MarkChange(pTarget, pMaster->GetGuildIdx(), pGuild->GetMarkName());

	// 080225 LUJ, 직업 정보 설정
	GUILDMEMBERINFO member;
	{
		member.MemberIdx	= pTarget->GetID();
		SafeStrCpy( member.MemberName, pTarget->GetObjectName(), sizeof( member.MemberName ) );
		member.Memberlvl	= pTarget->GetLevel();
		member.Rank			= GUILD_MEMBER;
		member.bLogged		= TRUE;

		const CHARACTER_TOTALINFO& info = pTarget->GetCharacterTotalInfo();

		member.mJobGrade	= info.JobGrade;
		member.mRace		= info.Race;
		memcpy( member.mJob, info.Job, sizeof( member.mJob ) );		
	}

	pGuild->AddMember(&member);
	// 081031 LUJ, 맵 번호 추가
	pGuild->SetLogInfo(pTarget->GetID(), TRUE, g_pServerSystem->GetMapNum() );

	{
		SEND_GUILD_TOTALINFO message;
		ZeroMemory( &message, sizeof( message ) );
		
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_ACCEPT_ACK;
		message.InitGuildInfo(pGuild->GetInfo());
		pGuild->GetTotalMember(message.MemberInfo);
		message.membernum	= pGuild->GetMemberNum();
		pTarget->SendMsg(&message, message.GetMsgLength());
	}
	
	{
		SEND_GUILD_MEMBER_INFO message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_ADDMEMBER_NOTIFY_TO_MAP;
		message.GuildIdx	= pMaster->GetGuildIdx();
		SafeStrCpy(message.GuildName, pGuild->GetGuildName(), sizeof( message.GuildName ) );
		message.MemberInfo	= member;

		g_Network.Send2AgentServer((char*)&message, sizeof(message));
	}

	GUILDWARMGR->AddPlayer( pTarget );

	if( pGuild->GetUnionIndex() )
	{
		GUILDUNIONMGR->AddPlayer( pTarget, pGuild->GetUnionIndex() );		
	}

	LogGuild(pTarget->GetID(), pGuild->GetIdx(), eGuildLog_MemberAdd);
}

void CGuildManager::UserLogIn(CPlayer* pPlayer)
{
	if(CGuild* pGuild = GetGuild(pPlayer->GetGuildIdx()))
	{
		SetLogInfo(
			pGuild,
			pPlayer->GetID(),
			TRUE);
		GUILDUNIONMGR->LoginPlayer(
			pPlayer,
			pGuild->GetUnionIndex());
	}
}

void CGuildManager::UserLogOut(CPlayer* pPlayer)
{
	if(CGuild* pGuild = GetGuild(pPlayer->GetGuildIdx()))
	{
		SetLogInfo(
			pGuild,
			pPlayer->GetID(),
			FALSE);
	}
}

void CGuildManager::SecedeSyn(CPlayer* pPlayer)
{
	DWORD GuildIdx = pPlayer->GetGuildIdx();
	CGuild* pGuild = GetGuild(GuildIdx);

	if(pGuild == 0)
	{
		SendNackMsg( pPlayer, MP_GUILD_SECEDE_NACK, eGuildErr_NoGuild);
		return;
	}
	else if(pPlayer->GetGuildMemberRank() >= GUILD_MASTER)
	{
		SendNackMsg( pPlayer, MP_GUILD_SECEDE_NACK, eGuildErr_Err);
		return;
	}
	
	GuildSecedeMember(GuildIdx, pPlayer->GetID());
}

void CGuildManager::SecedeResult(DWORD GuildIdx, DWORD MemberIDX, char* EntryDay)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(MemberIDX);

	if(pPlayer)
	{		
		pPlayer->SetGuildInfo(0, GUILD_NOTMEMBER, "", 0);
		pPlayer->SetNickName("");
		SendGuildEntryDate(pPlayer, EntryDay);
		MarkChange(pPlayer, 0, 0);

		MSGBASE msg;
		msg.Category = MP_GUILD;
		msg.Protocol = MP_GUILD_SECEDE_ACK;
		pPlayer->SendMsg(&msg, sizeof(msg));

	}

	CGuild* pGuild = GetGuild(GuildIdx);

	if( pGuild == NULL )
	{
		return;
	}

	//	BOOL rt = pGuild->DeleteMember(pPlayer->GetID(), eGuild_Secede);
	BOOL rt = pGuild->DeleteMember(MemberIDX, eGuild_Secede);
	if(rt == FALSE)
		return;	

	//	pGuild->SendDeleteMsg(pPlayer->GetID(), eGuildLog_MemberSecede);
	pGuild->SendDeleteMsg(MemberIDX, eGuildLog_MemberSecede);

	//notify
	MSG_DWORD notify;
	notify.Category = MP_GUILD;
	notify.Protocol = MP_GUILD_SECEDE_NOTIFY_TO_MAP;
	notify.dwObjectID = GuildIdx;
	//	notify.dwData = pPlayer->GetID();
	notify.dwData = MemberIDX;
	g_Network.Send2AgentServer((char*)&notify, sizeof(notify));

	LogGuild(MemberIDX, GuildIdx, eGuildLog_MemberSecede);

	// 071213 LUJ, 제명 시에 길드 점수 계산에서 빼야 함
	{
		CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( MemberIDX );

		if( player )
		{
			//090622 pdy 탈퇴 제명시 길드인덱스가 초기화되어있어 처리가 안되던 버그 수정 
			player->SetGuildIdx( GuildIdx ) ;
			RemovePlayer( player );
			player->SetGuildIdx( 0 ) ;
		}
	}
}


void CGuildManager::SendChatMsg(DWORD GuildIdx, DWORD PlayerIdx, char* pMsg)
{
	CGuild* pGuild = GetGuild(GuildIdx);
	if(pGuild == 0)
		return;

	MSG_GUILD_CHAT msg;					// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경(TESTMSGID->MSG_GUILD_CHAT)
	msg.Category = MP_CHAT;
	msg.Protocol = MP_CHAT_GUILD;
	msg.dwObjectID = PlayerIdx;			// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경
	msg.dwGuildIdx = GuildIdx;			// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경
	msg.dwSenderIdx = PlayerIdx;
	SafeStrCpy(msg.Msg, pMsg, MAX_CHAT_LENGTH+1);

	pGuild->SendMsgToAll(&msg, msg.GetMsgLength());

}

void CGuildManager::RegistMarkResult(DWORD GuildIdx, MARKNAMETYPE MarkName, char * pImgData)
{	
	CGuild* pGuild = GetGuild(GuildIdx);
	if(pGuild == 0)
		return;

	MSG_GUILDMARK_IMG msg;
	char *pStr = (char*)pImgData;
	char tempBuf[3] = {0,};
	int curpos = 0;
	for(int n=0;n<GUILDMARK_BUFSIZE;++n)
	{
		strncpy(tempBuf,&pStr[curpos],2); // "FF"
		msg.imgData[n] = HexToByte(tempBuf);
		curpos += 2;
	}

	// 091111 ONS 길드 문장 등록시 길드원 전체에게 알려준다.
	GUILDMEMBERINFO memberInfo[ MAX_GUILD_MEMBER ] = { 0 };
	pGuild->GetTotalMember( memberInfo );
	const DWORD nMemberCount = pGuild->GetMemberNum();
	for( DWORD size = 0; size < nMemberCount; size++)
	{
		DWORD dwPlayerID = memberInfo[size].MemberIdx;
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( dwPlayerID );
		if( 0 == pPlayer )
			continue;

		pPlayer->SetGuildMarkName(MarkName);
		MarkChange(pPlayer, GuildIdx, MarkName);
	}

	DoRegistMark(pGuild, MarkName, msg.imgData);

    msg.Category = MP_GUILD;
	msg.GuildIdx = GuildIdx;
	msg.MarkName = MarkName;
	msg.Protocol = MP_GUILDMARK_NOTIFY_TO_MAP;
	g_Network.Send2AgentServer((char*)&msg, sizeof(msg));
}

// 091111 ONS 길드 문장 삭제를 길드원 전체에게 알려준다.
void CGuildManager::DeleteMarkResult(DWORD dwPlayerIdx, DWORD dwMarkIdx, DWORD dwGuildIdx)
{
	GUILDMEMBERINFO memberInfo[ MAX_GUILD_MEMBER ] = { 0 };

	CGuild* pGuild = GetGuild(dwGuildIdx);
	if( 0 == pGuild )
		return;

	pGuild->SetMarkName(0);
	DeleteMark(dwMarkIdx);

	pGuild->GetTotalMember( memberInfo );
	DWORD nMemberCount = pGuild->GetMemberNum();
	for( DWORD size = 0; size < nMemberCount; size++)
	{
		DWORD dwPlayerID = memberInfo[size].MemberIdx;
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( dwPlayerID );
		if( 0 == pPlayer )
			continue;

		pPlayer->SetGuildMarkName(0);

		MSG_WORD msg;
		ZeroMemory(&msg, sizeof(MSG_WORD));
		msg.Category	= MP_GUILD;
		msg.Protocol	= MP_GUILDMARK_DEL_ACK;
		msg.dwObjectID	= dwPlayerID;
		msg.wData		= 0;
		PACKEDDATA_OBJ->QuickSend(pPlayer, &msg, sizeof(msg));
	}
}

void CGuildManager::DoRegistMark(CGuild* pGuild, MARKNAMETYPE MarkName, char* imgData)
{
	pGuild->SetMarkName(MarkName);
	CGuildMark * pMark = new CGuildMark;
	pMark->Init(MarkName, imgData);
	m_GuildMarkTable.Add(pMark, MarkName);
}

int CGuildManager::convertCharToInt(char c)
{
	if('0' <= c && c <= '9')
		return c - '0';
	if('A' <= c && c <= 'F')
		return c - 'A' +10;
	if('a' <= c && c <= 'f')
		return c - 'a' +10;
	return 0;
}

//반드시 "FF" 이런식으로 입력이 되어야 한다.
BYTE CGuildManager::HexToByte(char* pStr)
{
	int n1 = convertCharToInt(pStr[0]);
	int n2 = convertCharToInt(pStr[1]);

	return BYTE( n1 * 16 + n2 );
}

void CGuildManager::RequestMark(CPlayer* pPlayer, MARKNAMETYPE MarkName)
{
	CGuildMark* pMark = GetMark(MarkName);
	if(pMark == 0)
	{
		return;
	}
	MSG_GUILDMARK_IMG msg;
	msg.Category = MP_GUILD;
	msg.Protocol = MP_GUILDMARK_REQUEST_ACK;
	msg.MarkName = MarkName;
	memcpy(msg.imgData, pMark->GetImgData(), GUILDMARK_BUFSIZE);
	pPlayer->SendMsg(&msg, sizeof(msg));
}

void CGuildManager::DeleteMark(MARKNAMETYPE MarkName)
{
	CGuildMark* pMark = m_GuildMarkTable.GetData(MarkName);
	if(pMark == 0)
		return;

	m_GuildMarkTable.Remove(MarkName);
	delete pMark;
}

void CGuildManager::MarkChange(CPlayer* pPlayer, DWORD GuildIdx, MARKNAMETYPE MarkName)
{
	MSG_DWORD3 msg;
	msg.Category = MP_GUILD;
	msg.Protocol = MP_GUILDMARK_CHANGE;
	msg.dwData1 = pPlayer->GetID();
	msg.dwData2 = GuildIdx;
	msg.dwData3 = MarkName;

	PACKEDDATA_OBJ->QuickSend(pPlayer, &msg, sizeof(msg));
}

void CGuildManager::SendGuildName(CPlayer* pPlayer, DWORD dwGuildIdx, const char * GuildName)
{
	SEND_GUILD_CREATE_ACK msg;
	msg.Category	= MP_GUILD;
	msg.Protocol	= MP_GUILDNAME_CHANGE;
	msg.dwObjectID	= pPlayer->GetID();
	msg.GuildIdx	= dwGuildIdx;
	SafeStrCpy( msg.GuildName, GuildName, sizeof( msg.GuildName ) );

	PACKEDDATA_OBJ->QuickSendExceptObjectSelf(pPlayer, &msg, sizeof(msg));
}

BOOL CGuildManager::CanEntryGuild(char* date)
{
	SYSTEMTIME ti;
	GetLocalTime(&ti);
	DWORD year;
	DWORD month;
	DWORD day;

	char buf[5]={ 0, };
	strncpy(buf, &date[0],4);
	year = atoi(buf);
	if( ti.wYear > year)
		return TRUE;
	else if(ti.wYear < year)
		return FALSE;

	char dbuf[3]={ 0, };
	strncpy(dbuf, &date[5], 2);
	month = atoi(dbuf);

	if(ti.wMonth > month)
		return TRUE;
	else if(ti.wMonth < month)
		return FALSE;

	strncpy(dbuf, &date[8], 2);
	dbuf[2] = 0;
	day = atoi(dbuf);
	if(ti.wDay < day)
		return FALSE;

	return TRUE;
}

void CGuildManager::SendGuildEntryDate(CPlayer* pPlayer, char* day)
{
	pPlayer->SetGuildCanEntryDate(day);

	MSG_NAME dmsg;
	dmsg.Category = MP_GUILD;
	dmsg.Protocol = MP_GUILD_SETENTRYDATE;
	SafeStrCpy(dmsg.Name, day, 11);
	pPlayer->SendMsg(&dmsg, sizeof(dmsg));
}

void CGuildManager::ChangeMemberRank(CPlayer* player, DWORD targetPlayerIndex, BYTE Rank)
{	
	CGuild* guild = GetGuild(player->GetGuildIdx());

	if( ! guild )
	{
		SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_ChangeRank_NoGuild );
		return;
	}
	else if( ! guild->IsMember( targetPlayerIndex ) )
	{
		SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_ChangeRank_NotMember );
		return;
	}
	if( player->GetID() == targetPlayerIndex )	// 자기 자신은 변경 불가
	{
		ASSERT( 0 );
		SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_Err );
		return;
	}
	else if( player->GetGuildMemberRank() <= Rank )
	{
		SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_ChangeRank_NoAuthority );
		return;
	}
	else if( GUILD_MASTER == Rank )
	{
		SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_ChangeRank_Master );
		return;
	}
	
	{
		const BYTE rank = player->GetGuildMemberRank();

		if( rank != GUILD_MASTER &&
			rank != GUILD_VICEMASTER )
		{
			SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_Err );
			return;
		}
	}	
	
	GUILDMEMBERINFO* targetMember = guild->GetMemberInfo(targetPlayerIndex);
	if(NULL == targetMember) return;

	if( targetMember->Rank == Rank )
	{
		SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_ChangeRank_AlreadyHave );
		return;
	}
	// 자신과 같거나 더 높은 직위를 가진 회원을 변경시킬 수 없음
	else if( player->GetGuildMemberRank() <= targetMember->Rank )
	{
		SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_ChangeRank_LowerLevel );
		return;
	}
	else if( targetMember->Rank == GUILD_STUDENT )
	{
		if( targetMember->Memberlvl < GUILD_STUDENT_MAX_LEVEL )
		{
			SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_ChangeRank_NoGraduatedStudent );
			return;
		}
		else if( ! guild->CanAddMember() )
		{
			SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_ChangeRank_Student );
			return;
		}
	}
	
	// 직위 변경
	{
		if( ! guild->ChangeMemberRank(targetPlayerIndex, Rank ) )
		{
			SendNackMsg( player, MP_GUILD_CHANGERANK_NACK, eGuildErr_Err);
			return;
		}

		GuildChangeMemberRank(player->GetGuildIdx(), targetPlayerIndex, Rank);

		CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(targetPlayerIndex);

		if(pMember)
		{
			pMember->SetGuildMemberRank(Rank);
		}
	}	

	{
		MSG_DWORDBYTE message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_CHANGERANK_NOTIFY_TO_MAP;
		message.dwObjectID	= player->GetGuildIdx();
		message.dwData		= targetPlayerIndex;
		message.bData		= Rank;
		//g_Network.Send2AgentServer( (char*)&message, sizeof( message ) );
		g_Network.Broadcast2AgentServer( ( char* )&message, sizeof( message ) );
	}

	LogGuild(player->GetID(), player->GetGuildIdx(), eGuildLog_MasterChangeRank, targetPlayerIndex, Rank);
}

void CGuildManager::GiveNickNameSyn(CPlayer* pMaster, DWORD TargetId, char* NickName)
{
	CGuild* pGuild = GetGuild(pMaster->GetGuildIdx());

	if( ! pGuild )
	{		
		SendNackMsg(pMaster, MP_GUILD_GIVENICKNAME_NACK, eGuildErr_NoGuild);
		return;
	}
	else if(pMaster->GetGuildMemberRank() == GUILD_MEMBER )
	{
		SendNackMsg(pMaster, MP_GUILD_GIVENICKNAME_NACK, eGuildErr_Err);
		return;
	}
	else if(pGuild->IsMember(TargetId) == FALSE)
	{
		SendNackMsg(pMaster, MP_GUILD_GIVENICKNAME_NACK, eGuildErr_Nick_NotMember);
		return;
	}

	const DWORD length = strlen( NickName );
	// 091106 ONS 길드원호칭 삭제위해 공백 입력 가능하도록 수정
	if((length > MAX_GUILD_NICKNAME) )
	{
		SendNackMsg(pMaster, MP_GUILD_GIVENICKNAME_NACK, eGuildErr_Nick_NotAvailableName);
		return;
	}
	
	GuildGiveMemberNickName(TargetId, NickName);

	CPlayer* pTarget = (CPlayer*)g_pUserTable->FindUser(TargetId);
	if(pTarget)
	{
		pTarget->SetNickName(NickName);

		// SEND TO 
		SEND_GUILD_NICKNAME msg;
		msg.Category = MP_GUILD;
		msg.Protocol = MP_GUILD_GIVENICKNAME_ACK;
		msg.TargetIdx = TargetId;
		SafeStrCpy(msg.NickName, NickName, MAX_GUILD_NICKNAME+1);
		PACKEDDATA_OBJ->QuickSend(pTarget, &msg, sizeof(msg));	
	}
	else
	{
		//notify
		SEND_GUILD_NICKNAME nmsg;
		nmsg.Category = MP_GUILD;
		nmsg.Protocol = MP_GUILD_GIVENICKNAME_NOTIFY_TO_MAP;
		nmsg.TargetIdx = TargetId;
		SafeStrCpy(nmsg.NickName, NickName, MAX_GUILD_NICKNAME+1);
		g_Network.Broadcast2AgentServer((char*)&nmsg,sizeof(nmsg));
	}
}

CItemSlot* CGuildManager::GetSlot(DWORD GuildIdx)
{
	CGuild* pGuild = GetGuild(GuildIdx);
	if(pGuild)
	{
		return pGuild->GetSlot();
	}
	return NULL;
}


void CGuildManager::MemberLevelUp(DWORD GuildIdx, DWORD PlayerIdx, LEVELTYPE PlayerLvl)
{
	CGuild* pGuild = GetGuild(GuildIdx);
	if(pGuild == 0)
		return;
	if(pGuild->IsMember(PlayerIdx) == FALSE)
		return;
	pGuild->SetMemberLevel(PlayerIdx, PlayerLvl);

	MSG_DWORD3 msg;
	msg.Category = MP_GUILD;
	msg.Protocol = MP_GUILD_MEMBERLEVEL_NOTIFY_TO_MAP;
	msg.dwData1 = GuildIdx;
	msg.dwData2 = PlayerIdx;
	msg.dwData3 = PlayerLvl;
	g_Network.Send2AgentServer((char*)&msg,sizeof(msg));	
}

void CGuildManager::SetLogInfo(CGuild* pGuild, DWORD PlayerIdx, BOOL vals)
{
	if(pGuild->IsMember(PlayerIdx) == FALSE)
		return;

	pGuild->SetLogInfo(
		PlayerIdx,
		vals,
		g_pServerSystem->GetMapNum() );

	SEND_GUILDMEMBER_LOGININFO msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	= MP_GUILD;
	msg.Protocol	= MP_GUILD_LOGINFO_NOTIFY_TO_MAP;
	msg.dwObjectID	= pGuild->GetIdx();
	msg.MemberIdx	= PlayerIdx;
	msg.bLogIn		= vals;
	// 081027 LUJ, 맵 번호
	msg.mMapType	= g_pServerSystem->GetMapNum();
	g_Network.Send2AgentServer((char*)&msg,sizeof(msg));
}

void CGuildManager::MsgGuildNoteSyn( void* pMsg )
{
// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경
//  MSG_GUILD_SEND_NOTE -> MSG_GUILD_UNION_SEND_NOTE
//  msg->FromID -> msg->dwObjectID
	MSG_GUILD_UNION_SEND_NOTE* msg = (MSG_GUILD_UNION_SEND_NOTE*) pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
	if( !pPlayer )	return;

	DWORD dwGuildIdx = pPlayer->GetGuildIdx();
	if( !dwGuildIdx )
	{
		SendNackMsg( pPlayer, MP_GUILD_NOTE_NACK, eGuildErr_NoGuild );
		return;
	}
	CGuild* pGuild = m_GuildRoomHashTable.GetData(dwGuildIdx);
	if( !pGuild )
	{
		SendNackMsg( pPlayer, MP_GUILD_NOTE_NACK, eGuildErr_NoGuild );
		return;
	}

	pGuild->SendAllNote(pPlayer, msg->Note);

	MSGBASE ack;
	ack.Category = MP_GUILD;
	ack.Protocol = MP_GUILD_NOTE_ACK;
	pPlayer->SendMsg(&ack, sizeof(ack));
}

void CGuildManager::AddStudentResult( CPlayer* pTarget, DWORD MasterID )
{
	CPlayer*	pMaster = (CPlayer*)g_pUserTable->FindUser(MasterID);
	CGuild*		pGuild	= GetGuild( pMaster ? pMaster->GetGuildIdx() : 0 );

	// 검사
	{
		MSG_NAME_DWORD message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_INVITE_AS_STUDENT_ACCEPT_NACK;
		
		if( ! pMaster )
		{
			message.dwData = eGuildErr_InviteApt_NoMaster;
		}
		else if( !	pGuild ||
					pTarget->GetGuildIdx() )
		{
			message.dwData = eGuildErr_Err;
		}
		else if(  ! pGuild->CanAddStudent() )
		{
			message.dwData = eGuildErr_AddMember_FullMember;
		}
		else if( pTarget->GetLevel() >= GUILD_STUDENT_MAX_LEVEL )
		{
			message.dwData = eGuildErr_AddStudent_TooHighLevelAsStudent;
		}

		if( message.dwData )
		{
			pTarget->SendMsg( &message, sizeof( message ) );
			return;
		}
	}
	
	GuildAddMember(pMaster->GetGuildIdx(), pTarget->GetID(), pGuild->GetGuildName(), GUILD_STUDENT);
	
	pTarget->SetGuildInfo(pMaster->GetGuildIdx(), GUILD_STUDENT, pGuild->GetGuildName(), pGuild->GetMarkName());
	MarkChange(pTarget, pMaster->GetGuildIdx(), pGuild->GetMarkName());

	// 080225 LUJ, 직업 정보 설정
	GUILDMEMBERINFO member;
	{
		member.MemberIdx	= pTarget->GetID();
		SafeStrCpy( member.MemberName, pTarget->GetObjectName(), sizeof( member.MemberName ) );
		member.Memberlvl	= pTarget->GetLevel();

		// 080408 LUJ, 수련생으로 가입하였는데 신입으로 맵에 전파되는 문제 수정
		member.Rank			= GUILD_STUDENT;
		member.bLogged		= TRUE;

		const CHARACTER_TOTALINFO& info = pTarget->GetCharacterTotalInfo();

		member.mJobGrade	= info.JobGrade;
		member.mRace		= info.Race;
		memcpy( member.mJob, info.Job, sizeof( member.mJob ) );
	}
	
	pGuild->AddMember(&member);
	// 081031 LUJ, 맵 번호 추가
	pGuild->SetLogInfo(pTarget->GetID(), TRUE, g_pServerSystem->GetMapNum() );

	{
		SEND_GUILD_TOTALINFO message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_ACCEPT_STUDENT_ACK;
		message.membernum	= pGuild->GetMemberNum();
		message.InitGuildInfo(pGuild->GetInfo());
		pGuild->GetTotalMember(message.MemberInfo);
		
		pTarget->SendMsg( &message, message.GetMsgLength() );
	}
	
	{
		SEND_GUILD_MEMBER_INFO message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_ADDMEMBER_NOTIFY_TO_MAP;
		message.GuildIdx	= pMaster->GetGuildIdx();
		SafeStrCpy(message.GuildName, pGuild->GetGuildName(), sizeof( message.GuildName ) );
		message.MemberInfo.MemberIdx = pTarget->GetID();
		message.MemberInfo.Memberlvl = pTarget->GetLevel();
		SafeStrCpy(message.MemberInfo.MemberName, pTarget->GetObjectName(), sizeof( message.MemberInfo.MemberName ) );;
		message.MemberInfo.Rank = GUILD_STUDENT;

		g_Network.Send2AgentServer((char*)&message, sizeof( message ) );
	}
	
	GUILDWARMGR->AddPlayer( pTarget );

	if( pGuild->GetUnionIndex() )
		GUILDUNIONMGR->AddPlayer( pTarget, pGuild->GetUnionIndex() );		

	LogGuild(pTarget->GetID(), pGuild->GetIdx(), eGuildLog_StudentAdd );
}

void CGuildManager::AddHuntedMonster( CPlayer* player, CObject* object )
{
	if(FALSE == (eObjectKind_Monster & object->GetObjectKind()))
	{
		return;
	}

	CMonster*		monster = ( CMonster* )object;
	CGuildScore*	guild	= GetGuildScore( player->GetGuildIdx() );

	if( guild &&
		player->GetLevel() <= monster->GetLevel() )
	{
		guild->AddScore( mScoreSetting.mHuntScore );
	}
}

void CGuildManager::Process()
{
	// 타임 리스트에 DB에 업데이트 시간이 적게 남은 길드 순으로 저장되어 있다
	// 리스트의 head를 참조하여 처리할 시간이 되었으면 DB에 갱신한다.

	// 길드 점수 정보 처리
	{
		if( mTimeList.empty() )
		{
			return;
		}

		const DWORD		guildIndex	= mTimeList.front();
		CGuildScore*	guild		= GetGuildScore( guildIndex );

		// 길드 정보가 없으면 처리할 이유가 없다
		if(	! guild )
		{	
			mTimeList.pop_front();
			return;
		}

		const DWORD tick = GetTickCount();

		// 계산 가능한 시점에만 점수를 얻어야 한다.
		if( ! guild->IsEnableScore( tick ) )
		{
			return;
		}

		const float score = guild->GetScore( tick );
		
		GuildAddScore( guildIndex, score );

		// 071114 웅주, 로그
		InsertLogGuildScore( guildIndex, score, eGuildLog_TimeScoreAdd );

		guild->Reset( tick );

		// 리스트 맨 뒤로 이동시킨다
		mTimeList.pop_front();
		mTimeList.push_back( guildIndex );
	}
}

void CGuildManager::AddPlayer( CPlayer* player )
{
	ASSERT( player );

	const DWORD guildIndex = player->GetGuildIdx();

	if( ! guildIndex )
	{
		return;
	}

	// 길드 정보에 추가
	{
		CGuildScore* guild = GetGuildScore( guildIndex );

		if( ! guild )
		{
			guild = new CGuildScore( mScoreSetting.mUpdateGap, mScoreSetting.mUnitTimeScore / mScoreSetting.mUnitTime );

			mGuildScoreMap.insert( std::pair< DWORD, CGuildScore* >( guildIndex, guild ) );

			mTimeList.push_back( guildIndex );

			// 080417 LUJ, 맵에 처음 길드원이 접속한 시점에 전체 길드 스킬을 읽는다
			g_DB.FreeMiddleQuery(
				RGuildGetSkill,
				guildIndex,
				"SELECT TOP %d SKILL_IDX, SKILL_LEVEL FROM TB_GUILD_SKILL WHERE GUILD_IDX = %d AND SKILL_IDX > 0 ORDER BY SKILL_IDX",
				MAX_MIDDLEROW_NUM,
				guildIndex );
		}
		// 080417 LUJ, 스킬 정보를 초기화한 후라면 가진 스킬 정보를 플레이어에게 전송한다
		else
		{
			CGuild* guild = GetGuild( guildIndex );

			if( guild )
			{
				guild->UpdateSkill( player->GetID() );
				guild->SendSkill( *player );
			}
		}

		ASSERT( guild );
		guild->AddPlayer( player->GetID() );
	}

	// 데이터 요청. 
	// 주의:	각 맵 서버 간의 동기 관리를 위해 가능한 메모리에 데이터를 적재하거나 브로드캐스팅하지 않는다
	//			최대한 DB의 정보를 가져오도록 하자. 이쪽이 프로세스도 효율적으로 쓸 수 있다.
	{
		const DWORD playerIndex = player->GetID();

		GuildReadData( playerIndex, guildIndex );
		//GuildReadMember( playerIndex, guildIndex );
	}	
}


void CGuildManager::RemovePlayer( const CPlayer* player )
{
	if( ! player )
	{
		ASSERT( 0 );
		return;
	}
	
	const DWORD playerIndex	= player->GetID();
	const DWORD guildIndex	= player->GetGuildIdx();
	
	CGuildScore* guildScore = GetGuildScore( guildIndex );
	
	if( ! guildScore )
	{
		return;
	}

	const float score = guildScore->RemovePlayer( playerIndex );

	if( guildScore->GetPlayerSize() )
	{
		guildScore->AddScore( score );
	}
	// 더 이상 로그인된 회원이 없어 GuildScore클래스가 점수를 보관할 필요가 없다는 뜻.
	// 즉시 누적된 점수를 업데이트하고 메모리에서 삭제한다
	else
	{
		GuildAddScore( guildIndex, score + guildScore->GetScore( GetTickCount() ) );

		InsertLogGuildScore( guildIndex, score, eGuildLog_TimeScoreAdd );

		SAFE_DELETE( guildScore );
		mGuildScoreMap.erase( guildIndex );

		// 080417 LUJ, 길드 스킬을 제거한다
		{
			CGuild* guild = GetGuild( guildIndex );

			if( guild )
			{
				guild->RemoveAllSkill();
			}
		}
	}
	
	GuildCloseWarehouse( guildIndex, playerIndex, g_pServerSystem->GetMapNum() );
}


CGuildManager* CGuildManager::GetInstance()
{
	static CGuildManager instance;

	return &instance;
}

// 081031 LUJ, 소환 스크립트 파싱 추가
void CGuildManager::LoadScript()
{
	CMHFile file;

	if( ! file.Init( "system/resource/guild_setting.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	enum eParseType
	{
		eNone,
		eScore,
		eLevel,
		// 080417 LUJ, 길드 스킬 스크립트
		eSkill,
	}
	parseType = eNone;

	std::list< std::string > text;

	int	openCount	= 0;

	for(	TCHAR line[ MAX_PATH ];
		! file.IsEOF();
		ZeroMemory( line, sizeof( line ) ) )
	{
		file.GetLine( line, sizeof( line ) / sizeof( TCHAR ) );

		const int length = _tcslen( line );

		if( 0 == length )
		{
			continue;
		}

		// 중간의 주석 제거
		{
			for( int i = 0; i < length - 1; ++i )
			{
				const char a = line[ i ];
				const char b = line[ i + 1 ];

				if( '/' == a &&
					'/' == b )
				{
					line[ i ] = 0;
					break;
				}
			}
		}

		text.push_back( line );

		const TCHAR* separator	= _T( " \n\t=,+\"()" );
		const TCHAR* token		= _tcstok( line, separator );
		const TCHAR* markBegin	= _T( "{" );
		const TCHAR* markEnd	= _T( "}" );

		if( ! token )
		{
			text.pop_back();
			continue;
		}		
		else if( ! _tcsicmp( token, "score_setting" ) )
		{
			parseType = eScore;
		}
		else if( ! _tcsicmp( token, "level_setting" ) )
		{
			parseType = eLevel;
		}
		// 080417 LUJ, 길드 스킬 스크립트 로딩
		else if( ! _tcsicmp( token, "skill_setting" ) )
		{
			parseType = eSkill;
		}

		if( ! _tcsnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++openCount;
		}
		else if( ! _tcsnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			if( 0 == --openCount )
			{
				switch( parseType )
				{
				case eScore:
					{
						ParseScoreScript( text );
						break;
					}
				case eLevel:
					{
						ParseLevelScript( text );
						break;
					}
					// 080417 LUJ, 길드 스킬 스크립트 로딩
				case eSkill:
					{
						ParseSkillScript( text );
						break;
					}
				}

				parseType = eNone;

				text.clear();
			}
		}
	}
}


void CGuildManager::ParseScoreScript( const std::list< std::string >& text )
{
	//ZeroMemory( &mScoreSetting, sizeof( mScoreSetting ) );
	{
		mScoreSetting.mUnitTime			= 0;
		mScoreSetting.mUnitTimeScore	= 0;
		mScoreSetting.mHuntScore		= 0;
		mScoreSetting.mUpdateGap		= 0;
		mScoreSetting.mKillerScore		= 0;
		mScoreSetting.mCorpseScore		= 0;
	}

	int					openCount	= 0;
	TCHAR				line[ MAX_PATH ];
	ScoreSetting::Unit*	lostScore	= 0;

	for(	std::list< std::string >::const_iterator it = text.begin();
		text.end() != it;
		++it )
	{
		_tcscpy( line, it->c_str() );

		const TCHAR* separator	= _T( " \n\t=,+\"()" );
		const TCHAR* token		= _tcstok( line, separator );

		// 이름 파싱
		if( 0 == token )
		{
			continue;
		}

		const TCHAR* markBegin	= _T( "{" );
		const TCHAR* markEnd	= _T( "}" );

		if( 0 == _tcsnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++openCount;
		}
		else if( 0 == _tcsnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--openCount;
		}

		if( ! _tcsicmp( "unit_time", token ) )
		{
			mScoreSetting.mUnitTime	= _ttoi( _tcstok( 0, separator ) ) * 60 * 1000;
		}
		if( ! _tcsicmp( "unit_time_score", token ) )
		{
			mScoreSetting.mUnitTimeScore	= float( _tstof( _tcstok( 0, separator ) ) );
		}
		else if( ! _tcsicmp( "hunt_score", token ) )
		{
			mScoreSetting.mHuntScore	= float( _tstof( _tcstok( 0, separator ) ) );
		}
		else if( ! _tcsicmp( "update_time", token ) )
		{
			mScoreSetting.mUpdateGap	= _ttoi( _tcstok( 0, separator ) ) * 60 * 1000;
		}
		else if( ! _tcsicmp( "winningValue", token  ) )
		{
			mScoreSetting.mWinScore.mValue		= float( _tstof( _tcstok( 0, separator ) ) );
		}
		else if( ! _tcsicmp( "winningPercent", token  ) )
		{
			mScoreSetting.mWinScore.mPercent	= float( _tstof( _tcstok( 0, separator ) ) ) / 100.0f;
		}
		else if( ! _tcsicmp( "winningMoney", token  ) )
		{
			mScoreSetting.mWinMoneyRate	= float( _tstof( _tcstok( 0, separator ) ) ) / 100.0f;
		}
		else if( ! _tcsicmp( "killerScore", token  ) )
		{
			mScoreSetting.mKillerScore	= float( _tstof( _tcstok( 0, separator ) ) );
		}
		else if( ! _tcsicmp( "corpseScore", token  ) )
		{
			mScoreSetting.mCorpseScore	= float( _tstof( _tcstok( 0, separator ) ) );
		}
		else if( ! _tcsicmp( "scoreRange", token  ) )
		{
			_tcstok( 0, separator );	// begin score
			_tcstok( 0, separator );	// ~
			const char* token3		= _tcstok( 0, separator );	// end score

			//const DWORD beginScore	= _ttoi( token1 );
			const DWORD endScore	= token3 ? _ttoi( token3 ) : UINT_MAX;

			// 구간이 연속되어 있으므로 끝값만 설정하면 된다.
			// 예를 들어 0~1000, 1001~2000, 2001~ 이런 식으로 같이 있다고 하자.
			// std::map::lower_bound()는 주어진 값보다 작은 키를 가진 값을 가져오므로 쉽게 값을 얻을 수 있다.			
			//
			// 주의: stdext:hash_map은 올바른 결과를 보장하지 않는다.
			lostScore = &( mScoreSetting.mLostScoreMap[ endScore ] );
		}
		else if( ! _tcsicmp( "lostScore", token  ) )
		{
			token = _tcstok( 0, separator );
			ASSERT( 0 );

			if( lostScore && token )
			{
				if( '%' == token[ _tcslen( token ) - 1 ] )
				{
					lostScore->mPercent	= float( _tstof( token ) ) / 100.0f;
				}
				else
				{
					lostScore->mValue	= float( _tstof( token ) );
				}
			}
		}
	}

	if( openCount )
	{
		ASSERT( 0 );
	}
}


void CGuildManager::ParseLevelScript( const std::list< std::string >& text )
{
	int		openCount	= 0;
	TCHAR	line[ MAX_PATH ];

	LevelSetting* setting = 0;

	for(	std::list< std::string >::const_iterator it = text.begin();
		text.end() != it;
		++it )
	{
		_tcscpy( line, it->c_str() );

		const TCHAR* separator	= _T( " \n\t=,+\"()" );
		const TCHAR* token		= _tcstok( line, separator );

		// 이름 파싱
		if( 0 == token )
		{
			continue;
		}

		const TCHAR* markBegin	= _T( "{" );
		const TCHAR* markEnd	= _T( "}" );

		if( 0 == _tcsnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++openCount;
		}
		else if( 0 == _tcsnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--openCount;
		}

		if( ! strcmpi( "level", token ) )
		{
			const DWORD level = _ttoi( _tcstok( 0, separator ) );

			ASSERT( mLevelSettingMap.end() == mLevelSettingMap.find( level ) );

			setting = &( mLevelSettingMap[ level ] );

			// 이전 길드 레벨 정보가 있다면 그대로 적용하기 위해 복사한다
			if( 1 < level && 
				mLevelSettingMap.end() != mLevelSettingMap.find( level - 1 ) )
			{
				*setting = mLevelSettingMap[ level - 1 ];
			}
			else
			{
				ZeroMemory( setting, sizeof( *setting ) );
			}
		}
		else if( ! strcmpi( "master_level", token ) )
		{
			setting->mRequiredMasterLevel = _ttoi( _tcstok( 0, separator ) );
		}
		else if( ! strcmpi( "score", token ) )
		{
			setting->mRequiredScore		= _ttoi( _tcstok( 0, separator ) );
		}
		else if( ! strcmpi( "gold", token ) )
		{
			setting->mRequiredMoney		= _ttoi( _tcstok( 0, separator ) );
		}
		else if( ! strcmpi( "member_size", token ) )
		{
			setting->mMemberSize		= _ttoi( _tcstok( 0, separator ) );
		}
		else if( ! strcmpi( "quest", token ) )
		{
			setting->mRequiredQuest		= _ttoi( _tcstok( 0, separator ) );
		}
		else if( ! strcmpi( "guild_mark", token ) )
		{
			setting->mGuildMark			= ! _tcsicmp( "true", _tcstok( 0, separator ) );
		}
		else if( ! strcmpi( "union_size", token ) )
		{
			setting->mUnionSize			= _ttoi( _tcstok( 0, separator ) );
		}
		else if( ! strcmpi( "apprentice_size", token ) )
		{
			setting->mApprenticeSize	= _ttoi( _tcstok( 0, separator ) );
		}
		else if( ! strcmpi( "warehouse_size", token ) )
		{
			setting->mWarehouseSize	= _ttoi( _tcstok( 0, separator ) );
		}
		// 080417 LUJ, 획득 가능한 길드 스킬 개수
		else if( ! strcmpi( "skill_size", token ) )
		{
			setting->mSkillSize		= _ttoi( _tcstok( 0, separator ) );
		}
	}
}


CGuildScore* CGuildManager::GetGuildScore( DWORD guildIndex )
{
	GuildMap::iterator it = mGuildScoreMap.find( guildIndex );

	return mGuildScoreMap.end() == it ? 0 : it->second;
}


const CGuildManager::LevelSetting* CGuildManager::GetLevelSetting( DWORD level ) const
{	
	LevelSettingMap::const_iterator it = mLevelSettingMap.find( level );

	return mLevelSettingMap.end() == it ? 0 : &it->second;
}


float CGuildManager::GetLosingScore( float score ) const
{
	ScoreSetting::LostScoreMap::const_iterator it = mScoreSetting.mLostScoreMap.lower_bound( DWORD( score ) );

	if( mScoreSetting.mLostScoreMap.end() == it )
	{
		return 0;
	}

	const ScoreSetting::Unit& unit = it->second;

	return unit.mValue + score * unit.mPercent;
}


const CGuildManager::ScoreSetting& CGuildManager::GetScoreSetting() const
{
	return mScoreSetting;
}

// 080417 LUJ, 길드 스킬 스크립트 로딩
void CGuildManager::ParseSkillScript( const std::list< std::string >& text )
{
	int					openCount	= 0;
	SkillSettingMap*	settingMap	= 0;

	for(
		std::list< std::string >::const_iterator it = text.begin();
		text.end() != it;
		++it )
	{
		char line[ MAX_PATH ] = { 0 };

		strcpy( line, it->c_str() );

		const char* separator	= "~=,+\"() \t";
		const char* token		= strtok( line, separator );

		if( ! token )
		{
			continue;
		}

		const char* markBegin	= _T( "{" );
		const char* markEnd	= _T( "}" );

		if( 0 == strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++openCount;
		}
		else if( 0 == strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--openCount;
		}

		if( ! stricmp( "level", token ) )
		{
			const DWORD level = atoi( strtok( 0, separator ) );

			// 080417 LUJ,	이전 설정 정보를 복사한다
			//				레벨은 작은 숫자부터 큰 숫자로 증가되어야 한다. 그렇지 않으면 이전 설정 정보가 정상적으로 설정되지 않는다
			if( mSkillSettingLevelMap.end() != mSkillSettingLevelMap.find( level - 1 ) )
			{
				mSkillSettingLevelMap[ level ] = mSkillSettingLevelMap[ level - 1 ];
			}

			settingMap = &( mSkillSettingLevelMap[ level ] );
		}
		else if( atoi( token ) )
		{
			const int	beginSkillIndex = atoi( token );
			const char* endSkillIndex	= strtok( 0, separator );

			if( settingMap &&
				endSkillIndex )
			{
				SkillSetting& setting = ( *settingMap )[ beginSkillIndex ];
				setting.mLevel = BYTE( abs( beginSkillIndex - atoi( endSkillIndex ) ) ) + 1;

				while( (token = strtok( 0, separator )) != NULL)
				{
					if( ! stricmp( "master", token ) )
					{
						setting.mRank.insert( GUILD_MASTER );
					}
					else if( ! stricmp( "vice_master", token ) )
					{
						setting.mRank.insert( GUILD_VICEMASTER );
					}
					else if( ! stricmp( "senior", token ) )
					{
						setting.mRank.insert( GUILD_SENIOR );
					}
					else if( ! stricmp( "junior", token ) )
					{
						setting.mRank.insert( GUILD_JUNIOR );
					}
					else if( ! stricmp( "member", token ) )
					{
						setting.mRank.insert( GUILD_MEMBER );
					}
					else if( ! stricmp( "student", token ) )
					{
						setting.mRank.insert( GUILD_STUDENT );
					}
				}
			}
		}
	}
}

// 080417 LUJ, 레벨별 길드 스킬 설정 반환
const CGuildManager::SkillSettingMap* CGuildManager::GetSkillSetting( DWORD level ) const
{
	SkillSettingLevelMap::const_iterator it = mSkillSettingLevelMap.find( level );

	return mSkillSettingLevelMap.end() == it ? 0 : &( it->second );
}

// 080417 LUJ, 최대 길드 레벨의 스킬 정보를 얻는다
const CGuildManager::SkillSettingMap*  CGuildManager::GetMaxLevelSkillSetting()	const
{
	DWORD maxLevel = 0;

	for(
		SkillSettingLevelMap::const_iterator it = mSkillSettingLevelMap.begin();
		mSkillSettingLevelMap.end() != it;
		++it )
	{
		maxLevel = max( maxLevel, it->first );
	}

	return GetSkillSetting( maxLevel );
}

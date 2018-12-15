#include "stdafx.h"
#include "MapNetworkMsgParser.h"
#include "Network.h"
#include "MapDBMsgParser.h"
#include "CharMove.h"
#include "PackedData.h"
#include "SkillTreeManager.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "Player.h"
#include "Npc.h"
#include "PartyManager.h"
#include "UserTable.h"
#include "ItemManager.h"
#include "QuickManager.h"
#include "StreetStallManager.h"
#include "StorageManager.h"
#include "BattleSystem_Server.h"
#include "BootManager.h"
#include "objectstatemanager.h"
#include "PKManager.h"
#include "Finite State Machine/Machine.h"
#include "VehicleManager.h"
#include "ChannelSystem.h"
#include "RecallManager.h"
#include "QuestManager.h"
#include "LootingManager.h"
#include "Party.h"
#include "CharacterCalcManager.h"
#include "ObjectFactory.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GuildFieldWarMgr.h"
#include "QuestMapMgr.h"
#include "GuildTournamentMgr.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "GuildUnionManager.h"
#include "Monster.h"
// 081031 LUJ, 이동 처리 매니저 추가
#include "MoveManager.h"
#include "../[CC]Skill/Server/Tree/SkillTree.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "../hseos/Date/SHDateManager.h"
#include "AutoNoteManager.h"
#include "FishingManager.h"
#include "AISystem.h"
#include "RegenManager.h"

//#include "FamilyManager.h"
#include "AttackManager.h"

#include "MHFile.h"

// 070429 LYW --- MapNetworkMsgParser : Add for test.
#ifdef _DEBUG
#include "CharacterCalcManager.h"
#include "ItemDrop.h"
#endif //_DEBUG

#include "GridSystem.h"

#include "PetManager.h"
#include "Pet.h"
#include "..\hseos\ResidentRegist\SHResidentRegistManager.h"
#include "SiegeRecallMgr.h"
#include "NpcRecallMgr.h"
#include "SiegeWarfareMgr.h"
#include "LimitDungeonMgr.h"
#include "cCookManager.h"
#include "HousingMgr.h"
#include "./Trigger/Manager.h"
#include "./Dungeon/DungeonMgr.h"
// 090227 ShinJS --- 이동NPC 관리를 위해 include
#include "NPCMoveMgr.h"
#include "Vehicle.h"
#include "PCRoomManager.h"
#include "ConsignmentMgr.h"

extern HWND g_hWnd;
extern BOOL g_bCloseWindow;
extern int	g_nHackCheckNum;
extern int	g_nHackCheckWriteNum;
//extern BOOL g_bPlusTime;

void MP_MonitorMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_MORNITORMAPSERVER_NOTICESEND_SYN:			MornitorMapServer_NoticeSend_Syn() ;											break;
	case MP_MORNITORMAPSERVER_PING_SYN:					MornitorMapServer_Ping_Syn(pTempMsg, dwConnectionIndex, pMsg, dwLength) ;		break;
	case MP_MORNITORMAPSERVER_QUERYUSERCOUNT_SYN:		MornitorMapServer_QueryUserCount_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_MORNITORMAPSERVER_QUERY_CHANNELINFO_SYN:	MornitorMapServer_Query_ChannelInfo_Syn(dwConnectionIndex) ;					break;
	case MP_MORNITORMAPSERVER_ASSERTMSGBOX_SYN:			MornitorMapServer_AssertMsgBox_Syn(pMsg) ;										break;
	case MP_MORNITORMAPSERVER_SERVEROFF_SYN:			MornitorMapServer_ServerOff_Syn() ;												break;
	}
}


void MornitorMapServer_NoticeSend_Syn() 
{
	ASSERT(0);
}


void MornitorMapServer_Ping_Syn(MSGROOT* pTempMsg, DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	pTempMsg->Protocol = MP_MORNITORMAPSERVER_PING_ACK;
	g_Network.Send2Server(dwConnectionIndex, pMsg, dwLength);
}


void MornitorMapServer_QueryUserCount_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	MSGUSERCOUNT  * pmsg = (MSGUSERCOUNT  *)pMsg;
	pmsg->Protocol = MP_MORNITORMAPSERVER_QUERYUSERCOUNT_ACK;
	pmsg->dwUserCount = g_pUserTable->GetUserCount();
	pmsg->ServerPort = g_pServerTable->GetSelfServer()->wPortForServer;
	g_Network.Send2Server(dwConnectionIndex, pMsg, sizeof(MSGUSERCOUNT));
}


void MornitorMapServer_Query_ChannelInfo_Syn(DWORD dwConnectionIndex) 
{
	CHANNELSYSTEM->SendChannelInfoToMS( dwConnectionIndex );			
}


void MornitorMapServer_AssertMsgBox_Syn(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	g_bAssertMsgBox = (BOOL)pmsg->dwData;
	if(g_bAssertMsgBox)
		g_Console.LOG(4,"Assert MsgBox is On");
	else
		g_Console.LOG(4,"Assert MsgBox is Off");
}


void MornitorMapServer_ServerOff_Syn() 
{
	/*
	g_bCloseWindow = TRUE;	//콘솔창 닫는다.

	g_pServerSystem->HandlingBeforeServerEND();*/

	//SW060719 수정
	g_pServerSystem->SetStart(FALSE);
	g_pServerSystem->HandlingBeforeServerEND();

	g_Console.LOG(4, "Close Window" );
	g_bCloseWindow = TRUE;
}




void MP_POWERUPMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	BOOTMNGR->NetworkMsgParse(dwConnectionIndex, pMsg, dwLength);
}

void MP_CHARMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol)
	{
	//case MP_CHAR_LEVELDOWN_SYN:					Char_LevelDown_Syn(pMsg) ;									break;
	case MP_CHAR_POINTADD_SYN:						Char_PointAdd_Syn(pMsg) ;									break;
	case MP_CHAR_STATE_NOTIFY:						Char_State_Notify(pMsg) ;									break;
	case MP_CHAR_EXITSTART_SYN:						Char_ExitStart_Syn(pMsg) ;									break;
	case MP_CHAR_EXIT_SYN:							Char_Exit_Syn(pMsg) ;										break;
	case MP_CHAR_EMERGENCY_SYN:						Char_Emergency_Syn(pMsg) ;									break;
		// 080616 LUJ, 플레이어의 HP 정보를 반환한다
		//				서버가 클라이언트의 값을 업데이트한 후, 클라이언트는 EffectUnit 단계에서 HP 값 변경을 지연 처리한다
		//				이에 따라 서버와 클라이언트 값의 비동기 현상이 발생한다. 이를 막기 위해 클라이언트에서
		//				HP 정보 업데이트를 요청하도록 한다
		// 090507 LUJ, 요청/대상 오브젝트 모두의 HP를 갱신하도록 수정
	case MP_CHAR_LIFE_GET_SYN:
		{
			const MSG_DWORD* const m = ( MSG_DWORD* )pMsg;
			const DWORD requestObjectIndex = m->dwObjectID;
			const DWORD targetObjectIndex = m->dwData;
			CObject* const requestObject = g_pUserTable->FindUser( requestObjectIndex );
			CObject* const targetObject = g_pUserTable->FindUser( targetObjectIndex );

			if( 0 == requestObject ||
				0 == targetObject )
			{
				break;
			}

			MSG_DWORD3 message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category = MP_CHAR;
			message.Protocol = MP_CHAR_LIFE_GET_ACK;
			message.dwData1 = requestObject->GetLife();
			message.dwData2 = targetObjectIndex;
			message.dwData3	= targetObject->GetLife();
			requestObject->SendMsg( &message, sizeof( message ) );


			// 100310 ShinJS --- 마나실드를 사용중인 경우
			if( requestObject->HasEventSkill( eStatusKind_DamageToManaDamage ) )
			{
				// 마나정보를 전송한다.
				MSG_DWORD msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_CHAR;
				msg.Protocol = MP_CHAR_MANA_GET_ACK;
				msg.dwObjectID = requestObjectIndex;
				msg.dwData = requestObject->GetMana();
				requestObject->SendMsg( &msg, sizeof( msg ) );
			}

			break;
		}
		// 100222 ShinJS --- 마나 데미지로 인한 Mana 정보 요청 추가
	case MP_CHAR_MANA_GET_SYN:
		{
			const MSGBASE* const pmsg = (MSGBASE*)pMsg;
			CObject* const pRequestObject = g_pUserTable->FindUser( pmsg->dwObjectID );
			if( !pRequestObject )
				break;

			MSG_DWORD msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_CHAR;
			msg.Protocol = MP_CHAR_MANA_GET_ACK;
			msg.dwObjectID = pmsg->dwObjectID;
			msg.dwData = pRequestObject->GetMana();
			pRequestObject->SendMsg( &msg, sizeof( msg ) );
		}
		break;
	}
}

void Char_PointAdd_Syn(char* pMsg)
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer || pPlayer->IsResetStat() ) return;

	CHARCALCMGR->ArrangeCharLevelPoint(
		pPlayer,
		pmsg->wData);
}


void Char_State_Notify(char* pMsg)
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;

	if( pPlayer->GetState() == pmsg->bData && pPlayer->GetState() != eObjectState_Die )
	{
		OBJECTSTATEMGR_OBJ->EndObjectState(
			pPlayer,
			EObjectState(pmsg->bData));
	}
}

void Char_ExitStart_Syn(char* pMsg)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if( !pPlayer ) return;
	if( pPlayer->IsExitStart() ) return;

	int errorCode;
	if( ( errorCode = pPlayer->CanExitStart() ) == eEXITCODE_OK )
	{
		OBJECTSTATEMGR_OBJ->StartObjectState( pPlayer, eObjectState_Exit, 0 );
		pPlayer->SetExitStart( TRUE );
		MSGBASE msgAck;
		msgAck.Category		= MP_CHAR;
		msgAck.Protocol		= MP_CHAR_EXITSTART_ACK;
		pPlayer->SendMsg(&msgAck, sizeof(msgAck));
	}
	else
	{
		MSG_BYTE msgNack;
		msgNack.Category	= MP_CHAR;
		msgNack.Protocol	= MP_CHAR_EXITSTART_NACK;
		msgNack.bData		= BYTE( errorCode );
		pPlayer->SendMsg(&msgNack, sizeof(msgNack));
	}
}

void Char_Emergency_Syn(char* pMsg)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if( !pPlayer ) return;

	pPlayer->SetEmergency();
}


void Char_Exit_Syn(char* pMsg)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if( !pPlayer ) return;
	if( !pPlayer->IsExitStart() ) return;
	
	int nErrorCode;
	if( ( nErrorCode = pPlayer->CanExit() ) == eEXITCODE_OK )
	{
		OBJECTSTATEMGR_OBJ->StartObjectState( pPlayer, eObjectState_Exit, 0 );
		pPlayer->SetNormalExit();
		pPlayer->SetExitStart( FALSE );

		MSGBASE msgAck;
		msgAck.Category = MP_CHAR;
		msgAck.Protocol = MP_CHAR_EXIT_ACK;				
		pPlayer->SendMsg(&msgAck, sizeof(msgAck));
	}
	else
	{
		pPlayer->SetExitStart( FALSE );
		MSG_BYTE msgNack;
		msgNack.Category	= MP_CHAR;
		msgNack.Protocol	= MP_CHAR_EXIT_NACK;
		msgNack.bData		= BYTE( nErrorCode );
		pPlayer->SendMsg(&msgNack, sizeof(msgNack));				
	}	
}

void MP_CHATMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;

	switch(pTempMsg->Protocol)
	{
		// 061130 LYW --- Add New Msg Parser.
	case MP_CHAT_NORMAL :
	case MP_CHAT_ALL:
		{
			TESTMSG* pmsg = (TESTMSG*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(!pPlayer) return;

			if( pPlayer->GetUserLevel() == eUSERLEVEL_GM )
				pmsg->Protocol = MP_CHAT_GM;
			
			PACKEDDATA_OBJ->QuickSend(pPlayer,pmsg,dwLength);
		}
		break;
	// 070105 LYW --- Add protocol.
	case MP_CHAT_WHOLE :
	case MP_CHAT_TRADE :
	case MP_CHAT_SMALLSHOUT:			//같은 채널의 모두에게 보내기!
		{
			MSG_CHAT_WITH_SENDERID* pmsg = (MSG_CHAT_WITH_SENDERID*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(!pPlayer) return;

			MSG_CHAT_WITH_SENDERID msg;
			msg.Category = pmsg->Category;
			msg.Protocol = pmsg->Protocol;
			msg.dwObjectID = pmsg->dwObjectID;
			msg.dwSenderID = pPlayer->GetID();
			SafeStrCpy( msg.Msg, pmsg->Msg, MAX_CHAT_LENGTH+1 );
			SafeStrCpy( msg.Name, pmsg->Name, MAX_NAME_LENGTH+1 );
 
			if( pPlayer->GetUserLevel() == eUSERLEVEL_GM )
 				msg.Protocol = MP_CHAT_GM_SMALLSHOUT;
			else
			{
				if( pPlayer->GetBattle()->GetBattleKind() != eBATTLE_KIND_NONE && pPlayer->GetBattle()->GetBattleKind() != eBATTLE_KIND_SHOWDOWN )
					return;
			}
			
			g_pUserTable->SetPositionUserHead();
			CObject* pObject = NULL ;
			while( (pObject = g_pUserTable->GetUserData()) != NULL )
			{
				if( pObject->GetObjectKind() != eObjectKind_Player ) continue;
				
				CPlayer* pReceiver = (CPlayer*)pObject;
				if( pPlayer->GetChannelID() == pReceiver->GetChannelID() /*&&
					pPlayer->GetBattleID() < 5 && pReceiver->GetBattleID() < 5*/ )
				{
 					pReceiver->SendMsg( &msg, msg.GetMsgLength() );
				}
			}
		}
		break;
	case MP_CHAT_GUILD:
		{
			// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경
			// TESTMSGID -> MSG_GUILD_CHAT
			// pmsg->dwObjectID -> pmsg->dwGuildIdx
			// pmsg->dwSenderID -> pmsg->dwObjectID
			MSG_GUILD_CHAT* pmsg = (MSG_GUILD_CHAT*)pMsg;
			GUILDMGR->SendChatMsg(pmsg->dwGuildIdx, pmsg->dwObjectID, pmsg->Msg);
		}
		break;
	//case MP_CHAT_FAMILY :
	//	{
	//		SEND_FAMILY_CHAT * pmsg = (SEND_FAMILY_CHAT*)pMsg;
	//		FAMILYMGR->SendChatMsg(pmsg);
	//	}
	//	break ;
	case MP_CHAT_GUILDUNION:
		{
			// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경
			// MSG_CHAT_WITH_SENDERID -> MSG_GUILDUNION_CHAT
			MSG_GUILDUNION_CHAT* pmsg = (MSG_GUILDUNION_CHAT*)pMsg;
			GUILDUNIONMGR->SendChatMsg( pmsg );
		}
		break;

	default:
		//GAMESYSTEM_OBJ->m_ConnectionTable.SendToCharacter(pTempMsg,dwLength);
		break;
	}
}

void MP_USERCONNMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_USERCONN_CHANNELINFO_SYN:				UserConn_ChannelInfo_Syn(pTempMsg, dwConnectionIndex);			break;
	case MP_USERCONN_GAMEIN_OTHERMAP_SYN:			UserConn_GameIn_OtherMap_Syn(pMsg) ;							break;
	case MP_USERCONN_GAMEIN_SYN:					UserConn_GameIn_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_USERCONN_GAMEINPOS_SYN:					UserConn_GameInPos_Syn(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_GAMEIN_NACK:					UserConn_GameIn_Nack(pTempMsg) ;								break;
	case MP_USERCONN_GAMEOUT_SYN:					UserConn_GameOut_Syn(pMsg) ;									break;
	case MP_USERCONN_NOWAITEXITPLAYER:				UserConn_NoWaitExitPlayer(pMsg) ;								break;
	case MP_USERCONN_DISCONNECTED:					UserConn_Disconnected(dwConnectionIndex, pMsg) ;				break;
	case MP_USERCONN_CHANGEMAP_SYN:
		{
			const MSG_DWORD5* const message = ( MSG_DWORD5* )pMsg;
			const DWORD playerIndex = message->dwObjectID;
			const MAPCHANGE_INFO* const mapChangeInfo = GAMERESRCMNGR->GetMapChangeInfo( (WORD)message->dwData1 );

			if( 0 == mapChangeInfo )
			{
				break;
			}

			// 100518 ShinJS --- 같은 맵의 다른 이동정보의 경우를 위해 이동정보를 그대로 넘겨준다.
			const BOOL vehicleFailedToTeleport = ( FALSE == VEHICLEMGR->Teleport( playerIndex, mapChangeInfo ) );

			if( vehicleFailedToTeleport )
			{
				VEHICLEMGR->Unsummon( playerIndex, FALSE );
				UserConn_ChangeMap_Syn( dwConnectionIndex, pMsg );
			}
		}
		break;
	case MP_USERCONN_CHANGEMAP_NACK:				UserConn_ChangeMap_Nack() ;										break;
	// 070917 LYW --- MapNetworkMsgParser : Add process to change map through the npc.
	case MP_USERCONN_QUEST_CHANGEMAP_SYN:
	case MP_USERCONN_NPC_CHANGEMAP_SYN :			UserConn_Npc_ChangeMap_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_USERCONN_SAVEPOINT_SYN:					UserConn_SavePoint_Syn(pMsg) ;									break;
	case MP_USERCONN_BACKTOCHARSEL_SYN:				UserConn_BackToCharSel_Syn(pTempMsg, dwConnectionIndex, pMsg) ;	break;
	case MP_USERCONN_SETVISIBLE:					UserConn_SetVisible(pMsg) ;										break;		
	case MP_USERCONN_RETURN_SYN :					UserConn_Return_Syn(dwConnectionIndex, pMsg) ;					break;
	case MP_USERCONN_NPC_ADD_SYN:					UserConn_NpcAdd_Syn(pMsg); break;
	case MP_USERCONN_NPC_DIE_SYN:					UserConn_NpcDie_Syn(pMsg); break;
	case MP_USERCONN_OPTION_SYN:					UserConn_Option_Syn(pMsg); break;
	// 100408 --- Server 시간을 구함.
	case MP_USERCONN_GETSERVERTIME_SYN:
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pTempMsg->dwObjectID );
			if( !pPlayer )
				break;

			stTime64t msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_GETSERVERTIME_ACK;
			msg.dwObjectID = pTempMsg->dwObjectID;
			_time64( &msg.time64t );

			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
		break;
	case MP_USERCONN_CHANGE_CHANNEL_IN_GAME_SYN:
		{
			const MSG_WORD* const pmsg = (MSG_WORD*)pMsg;
			CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if(0 == pPlayer)
			{
				break;
			}
			else if(g_pServerSystem->IsNoRecallMap(*pPlayer))
			{
				MSGBASE message;
				ZeroMemory(
					&message,
					sizeof(message));
				message.Category = MP_USERCONN;
				message.Protocol = MP_USERCONN_CHANGE_CHANNEL_IN_GAME_NACK;
				
				pPlayer->SendMsg(
					&message,
					sizeof(message));
				break;
			}
			
			const WORD		wPrevChannel	= WORD(pPlayer->GetCurChannel());
			const WORD		wChannel		= pmsg->wData;

			// 같은 채널 이거나 범위외의 채널인 경우
			if( wPrevChannel == wChannel || wChannel >= CHANNELSYSTEM->GetChannelCount() )
				break;

			const DWORD		dwPlayerID		= pPlayer->GetID();
			const DWORD		dwUserID		= pPlayer->GetUserID();
			const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
			const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );
			const MAPTYPE	mapType			= g_pServerSystem->GetMapNum();
			const VECTOR3	pos				= *CCharMove::GetPosition( pPlayer );
            
			pPlayer->UpdateLogoutToDB( FALSE );
			pPlayer->SetMapMoveInfo( mapType, DWORD( pos.x ), DWORD( pos.z ) );

			g_pServerSystem->RemovePlayer( dwPlayerID );

			// 081218 LUJ, 업데이트보다 맵 이동이 빨리 진행될 경우 업데이트되지 않은 정보가 오게된다.
			//			이를 막기 위해 프로시저 처리 완료 후 진행한다
			UpdatePlayerOnMapOut( dwPlayerID, dwUserID, inventoryMoney, storageMoney, mapType, wChannel );

			// 081031 LUJ, 전 서버에 알린다
			{
				MSG_WORD messageToChannel;
				ZeroMemory( &messageToChannel, sizeof( messageToChannel ) );
				messageToChannel.Category	= MP_USERCONN;
				messageToChannel.Protocol	= MP_USERCONN_CHANGE_CHANNEL_SYN;
				messageToChannel.dwObjectID	= dwPlayerID;
				messageToChannel.wData		= wChannel;
				g_Network.Broadcast2AgentServer( (char*)&messageToChannel, sizeof( messageToChannel ) );
			}
		}
		break;
	case MP_USERCONN_BILLING_START_ACK:
		{
			const MSG_PACKET_GAME* const message = (MSG_PACKET_GAME*)pMsg;
			const DWORD playerIndex = message->dwObjectID;

			PCROOMMGR->SetPacketGame(
				playerIndex,
				message->mPacketGame);
		}
		break;
	case MP_USERCONN_BILLING_STOP_ACK:
		{
			const MSGBASE* const receivedMessage = (MSGBASE*)pMsg;
			const DWORD playerIndex = receivedMessage->dwObjectID;

			PCROOMMGR->RemovePacketGame(
				playerIndex);
			PCROOMMGR->RemovePCRoomBuff(
				playerIndex);
			PCROOMMGR->RemovePlayer(
				playerIndex);

			if(CObject* const object = g_pUserTable->FindUser(playerIndex))
			{
				MSGBASE sendMessage;
				ZeroMemory(
					&sendMessage,
					sizeof(sendMessage));
				sendMessage.Category = MP_USERCONN;
				sendMessage.Protocol = MP_USERCONN_BILLING_STOP_ACK;

				object->SendMsg(
					&sendMessage,
					sizeof(sendMessage));
			}
		}
		break;
	case MP_USERCONN_BILLING_UPDATE_ACK:
		{
			const MSG_PACKET_GAME* const message = (MSG_PACKET_GAME*)pMsg;
			const DWORD playerIndex = message->dwObjectID;

			PCROOMMGR->SetPacketGame(
				playerIndex,
				message->mPacketGame);
		}
		break;
	}
}

void UserConn_Option_Syn(LPVOID pMsg)
{
	const MSG_DWORD6* const receivedMessage = (MSG_DWORD6*)pMsg;
	const DWORD playerIndex = receivedMessage->dwObjectID;
	const BOOL isNoDeal = LOWORD(receivedMessage->dwData1);
	const BOOL isNoParty = HIWORD(receivedMessage->dwData1);
	const BOOL isNoShowDown = LOWORD(receivedMessage->dwData2);
	const BOOL isNeedAllow = HIWORD(receivedMessage->dwData2);
	const MONEYTYPE chargedMoney = receivedMessage->dwData3;
	const DWORD hideFlag = receivedMessage->dwData6;
	CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(
		playerIndex);

	if(0 == player)
	{
		return;
	}
	else if(eObjectKind_Player != player->GetObjectKind())
	{
		return;
	}

	sGAMEOPTION gameOption = {0};
	gameOption.bNoDeal = isNoDeal;
	gameOption.bNoParty = isNoParty;
	gameOption.bNoShowdown = isNoShowDown;
	player->SetGameOption(
		&gameOption);
	VEHICLEMGR->SetOption(
		playerIndex,
		isNeedAllow,
		chargedMoney);

	if(player->GetCharacterTotalInfo().HideFlag != hideFlag)
	{
		player->GetCharacterTotalInfo().HideFlag = hideFlag;

		MSG_DWORD sendMessage;
		ZeroMemory(
			&sendMessage,
			sizeof(sendMessage));
		sendMessage.Category = MP_USERCONN;
		sendMessage.Protocol = MP_USERCONN_OPTION_ACK;
		sendMessage.dwObjectID = player->GetID();
		sendMessage.dwData = hideFlag;

		PACKEDDATA_OBJ->QuickSend(
			player,
			&sendMessage,
			sizeof(sendMessage));
	}
}

void UserConn_ChannelInfo_Syn(MSGBASE* pTempMsg, DWORD dwConnectionIndex)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pTempMsg;
	CHANNELSYSTEM->SendChannelInfo(pmsg, dwConnectionIndex);
}


void UserConn_GameIn_OtherMap_Syn(char* pMsg) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	g_pServerSystem->RemovePlayer( pmsg->dwObjectID );
}


void UserConn_GameIn_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_NAME_DWORD4* pmsg = (MSG_NAME_DWORD4*)pMsg;
	CObject* pObject = g_pUserTable->FindUser(pmsg->dwObjectID);

	if( pObject != NULL &&
		pObject->GetObjectKind() == eObjectKind_Player )
	{
//			g_pServerSystem->RemovePlayer(pmsg->dwObjectID);

		MSG_DWORD msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_GAMEIN_NACK;
		msg.dwData		= pmsg->dwObjectID;

		g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );					
		return;
	}

	const DWORD uniqueIndexInAgent = pmsg->dwData1;
	const int channelIndex = pmsg->dwData2;
	const eUSERLEVEL userLevel = eUSERLEVEL(pmsg->dwData3);	

	CPlayer* pPlayer = g_pServerSystem->AddPlayer(
		pmsg->dwObjectID,
		dwConnectionIndex,
		uniqueIndexInAgent,
		channelIndex,
		userLevel);

	if(!pPlayer) return;

	// 080424 NYJ --- 낚시정보를 받아온다
	FishingData_Load(pmsg->dwObjectID);
	CookingData_Load(pmsg->dwObjectID);
	Cooking_Recipe_Load(pmsg->dwObjectID);
	HouseData_Load(pmsg->dwObjectID);

	CharacterNumSendAndCharacterInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
	CharacterSkillTreeInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
	CharacterItemInfo( pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN );
	
	/// 단축창 정보
	QuickInfo( pmsg->dwObjectID );


	// quest 정보 qurey 추가
	QUESTMGR->CreateQuestForPlayer( pPlayer );

	//퀘스트정보를 받아오도록 한다.
	QuestTotalInfo( pmsg->dwObjectID );				//QuestMainQuestLoad(), QuestSubQuestLoad(), QuestItemLoad(), TutorialLoad()이 연계된다.

	//농장 및 기타정보를 받아오도록 한다.
	Farm_LoadTimeDelay( pmsg->dwObjectID );			//MostserMeter_Load(), ResidentRegist_LoadInfo()가 연계된다.
	CYHHashTable<HIDE_NPC_INFO>* pHideNpcTable = GAMERESRCMNGR->GetHideNpcTable();
	
	// 06. 05 HIDE NPC - 이영준
	// 현재 맵의 숨김 가능 NPC들의 정보를 보내준다
	{
		pHideNpcTable->SetPositionHead();

		MSG_WORD2 msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_NPCHIDE_ACK;

		for(	HIDE_NPC_INFO* pInfo = NULL;
				(pInfo = pHideNpcTable->GetData())!=NULL;
				)
		{			
			msg.wData1 = pInfo->UniqueIdx;
			msg.wData2 = WORD( pInfo->ChannelInfo[pmsg->dwData2] );

			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
	}


	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.27	2007.11.28
	// ..함수 순서를 지키는 것이 좋음
	// ..챌린지 존 시작
	g_csDateManager.SRV_StartChallengeZone(pPlayer, pmsg->dwData2, pmsg->dwData4);
	// ..챌린지 존 입장 회수 로드
	ChallengeZone_EnterFreq_Load(pPlayer->GetID());

	// 090227 ShinJS --- 이동NPC 정보를 보낸다
	{
		CYHHashTable<STATIC_NPCINFO>* pMoveNpcInfo = GAMERESRCMNGR->GetMoveNpcInfoTable();
		if( pMoveNpcInfo )
		{
			STATIC_NPCINFO* pInfo = NULL;
			MSG_DWORD4 msg;
			memset( &msg, 0, sizeof(MSG_DWORD4) );

			msg.Category	= MP_NPC;
			msg.Protocol	= MP_NPC_ADD_MOVENPC_SYN;

			pMoveNpcInfo->SetPositionHead();
			while( (pInfo = pMoveNpcInfo->GetData()) != NULL )
			{
				msg.dwData1		= MAKEDWORD( pInfo->MapNum, pInfo->wDir );						// 맵번호, NPC 방향
				msg.dwData2		= MAKEDWORD( pInfo->wNpcUniqueIdx, pInfo->wNpcJob );			// NPC Index, NPC Kind
				msg.dwData3		= MAKEDWORD( pInfo->vPos.x, pInfo->vPos.z );					// NPC 위치
				msg.dwData4		= DWORD( NPCMOVEMGR->IsReadyToMove( pInfo->wNpcUniqueIdx ) );	// 이동준비상태 정보

				// User에게 이동NPC 생성 전송
				pPlayer->SendMsg( &msg, sizeof(MSG_DWORD4) );
			}
		}
	}
}


void UserConn_GameInPos_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	MSG_NAME_DWORD4* pmsg = (MSG_NAME_DWORD4*)pMsg;
	CObject* pObject = g_pUserTable->FindUser(pmsg->dwObjectID);

	if( pObject != NULL )	//이미 있다.
	{
		if( pObject->GetObjectKind() == eObjectKind_Player )	//유저인가?
		{
//					g_pServerSystem->RemovePlayer(pmsg->dwObjectID);

			MSG_DWORD msg;
			msg.Category	= MP_USERCONN;
			msg.Protocol	= MP_USERCONN_GAMEIN_NACK;
			msg.dwData		= pmsg->dwObjectID;
			g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );					
			return;
		}
	}

	CPlayer* pPlayer = g_pServerSystem->AddPlayer(
		pmsg->dwObjectID,
		dwConnectionIndex,
		pmsg->dwData1,
		pmsg->dwData2,
		eUSERLEVEL(pmsg->dwData4));

	if(0 == pPlayer)
		return;
	
	CharacterNumSendAndCharacterInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
	CharacterSkillTreeInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
	CharacterItemInfo( pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN );
	
	/// 단축창 정보
	QuickInfo( pmsg->dwObjectID );

	// quest 정보 qurey 추가
	QUESTMGR->CreateQuestForPlayer( pPlayer );

	//퀘스트정보를 받아오도록 한다.
	QuestTotalInfo( pmsg->dwObjectID );				//QuestMainQuestLoad(), QuestSubQuestLoad(), QuestItemLoad(), TutorialLoad()이 연계된다.

	//농장 및 기타정보를 받아오도록 한다.
	Farm_LoadTimeDelay( pmsg->dwObjectID );
}


void UserConn_GameIn_Nack(MSGBASE* pTempMsg) 
{
	// GAMEIN_ACK를 보냈을때 에이젼트에서 유니크 아이디가 틀리면
	// 이 메세지를 날려준다. 그 케릭터를 그냥 지워버리면 된다.
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pTempMsg->dwObjectID);
	if(pPlayer == NULL)
		return;

	g_pServerSystem->RemovePlayer(pTempMsg->dwObjectID);
}


void UserConn_GameOut_Syn(char* pMsg) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if( pPlayer == NULL ) return;

	g_pServerSystem->RemovePlayer( pmsg->dwObjectID );
	g_Console.LOG( 4, "Charackter Overlap!" );
}


void UserConn_NoWaitExitPlayer(char* pMsg) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if(pPlayer == NULL) return;

	pPlayer->SetWaitExitPlayer( FALSE );	//no wait for deleteing player.
}


void UserConn_Disconnected(DWORD dwConnectionIndex, char* pMsg) 
{
	//KES	//agent가 유저 정보 DB업데이트를 위해서 보내준다.
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if(pPlayer == NULL)
	{
		//KES 로그아웃 프로시져 호출하기 위해 에이젼트로 보내준다.
		MSGBASE msgLogout;
		msgLogout.Category		= MP_USERCONN;
		msgLogout.Protocol		= MP_USERCONN_LOGINCHECK_DELETE;
		msgLogout.dwObjectID	= pmsg->dwData;

		g_Network.Send2Server( dwConnectionIndex, (char*)&msgLogout, sizeof(msgLogout) );
		return;
	}

	if( !pPlayer->IsNormalExit() )
	{
		//강제 종료에 따른 제제
		if( pPlayer->IsPKMode() )
		{
			//PKMGR->PKModeExitPanelty( pPlayer );
			//---KES PK 071202
			PKMGR->PKModeDiePanelty( pPlayer ); //강제 종료일 경우는 PK모드 죽음 패널티를 적용한다.
		}

		LOOTINGMGR->DiePlayerExitPanelty( pPlayer );
		// 090316 LUJ, 탈것 소환 해제
		VEHICLEMGR->Dismount( pPlayer->GetID(), FALSE );
		AUTONOTEMGR->AutoPlayerLogOut(pPlayer);

		//강제 종료한 유저 잡아두기
		{
			// RaMa - 04.12.14
			pPlayer->UpdateLogoutToDB();

			PARTYMGR->UserLogOut(pPlayer);
			FriendNotifyLogouttoClient(pmsg->dwObjectID);
			GUILDMGR->UserLogOut(pPlayer);

			// 100105 ONS PC방버프지급 여부정보를 초기화한다.
			PCROOMMGR->RemovePCRoomBuff( pPlayer->GetID() );
			g_pServerSystem->RemovePlayer(pmsg->dwObjectID);
			//
			ConfirmUserOut( dwConnectionIndex, pPlayer->GetUserID(), pPlayer->GetID(), 2 );
		}
	}
	else
	{
		// RaMa - 04.12.14
		pPlayer->UpdateLogoutToDB();

		PARTYMGR->UserLogOut(pPlayer);
		FriendNotifyLogouttoClient(pmsg->dwObjectID);
		GUILDMGR->UserLogOut(pPlayer);

		// 100105 ONS PC방버프지급 여부정보를 초기화한다.
		PCROOMMGR->RemovePCRoomBuff( pPlayer->GetID() );
		g_pServerSystem->RemovePlayer(pmsg->dwObjectID);
		//
		ConfirmUserOut( dwConnectionIndex, pPlayer->GetUserID(), pPlayer->GetID(), 2 );
	}
}

void UserConn_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	// desc_hseos_결혼_01
	// S 결혼 추가 added by hseos 2008.01.30
	// MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
	MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;
	// E 결혼 추가 added by hseos 2008.01.30

	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pPlayer == NULL)
		return;
	// 090701 LUJ, 맵 이동 시 플레이어 정보를 DB에 쓰는데, 초기화가 끝나지 않았을 경우 쓰레기값이
	//		저장될 수 있다. 초기화가 끝난 객체만 처리한다
	else if( FALSE == pPlayer->GetInited() )
	{
		return;
	}

	//---KES PK 071202
	//---KES AUTONOTE
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.12.03
	// ..챌린지 존에서 죽었을 경우는 맵 이동을 시켜야 함.
	// if( pPlayer->IsPKMode() || LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) || pPlayer->GetState() == eObjectState_Die )
	if( pPlayer->GetAutoNoteIdx() || pPlayer->IsPKMode() || LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) || (pPlayer->GetState() == eObjectState_Die && g_csDateManager.IsChallengeZoneHere() == FALSE))
	// E 데이트 존 추가 added by hseos 2007.12.03
	{
		MSGBASE msgNack;
		msgNack.Category	= MP_USERCONN;
		msgNack.Protocol	= MP_USERCONN_CHANGEMAP_NACK;
		msgNack.dwObjectID	= pPlayer->GetID();
		pPlayer->SendMsg( &msgNack, sizeof(msgNack) );
		return;
	}

	MAPCHANGE_INFO* ChangeInfo = NULL;

	ChangeInfo = GAMERESRCMNGR->GetMapChangeInfo( (WORD)pmsg->dwData1 );
	if( !ChangeInfo )
	{
		MSG_ITEM_ERROR msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_SHOPITEM_USE_NACK;
		pPlayer->SendMsg( &msg, sizeof(msg) );
		return;
	}

	// 090511 ONS 신규종족 맵이동 제한 처리 : '적막한 협곡'맵은 신규종족만 이동 가능
	if(RaceType_Devil != pPlayer->GetCharacterTotalInfo().Race && 96 == ChangeInfo->MoveMapNum)
	{
		MSGBASE msgNack;
		msgNack.Category	= MP_USERCONN;
		msgNack.Protocol	= MP_USERCONN_DEVILMARKET_CHANGEMAP_NACK;
		msgNack.dwObjectID	= pPlayer->GetID();
		pPlayer->SendMsg( &msgNack, sizeof(msgNack) );
		return;
	}

	// RaMa - 04.12.14
	pPlayer->UpdateLogoutToDB(FALSE);

	VECTOR3 RandPos = ChangeInfo->MovePoint;

	if(PARTYMGR->CanUseInstantPartyMap(ChangeInfo->MoveMapNum))
	{
		// 기존파티는 탈퇴하자.
		CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx());
		if(pParty)
		{
			if(pParty->GetMasterID() == pPlayer->GetID())
				PARTYMGR->BreakupParty(pParty->GetPartyIdx(), pPlayer->GetID());
			else
				PARTYMGR->DelMemberSyn(pPlayer->GetID(), pParty->GetPartyIdx());
		}
	}
	
	pPlayer->SetMapMoveInfo( ChangeInfo->MoveMapNum, (DWORD)RandPos.x, (DWORD)RandPos.z );
	// desc_hseos_결혼_01
	// S 결혼 추가 added by hseos 2008.01.30
	// ..배우자 위치로 텔레포트 시 배우자 좌표가 전송된다.
	if (pmsg->dwData4 && pmsg->dwData5)
	{
		pPlayer->SetMapMoveInfo( ChangeInfo->MoveMapNum, pmsg->dwData4, pmsg->dwData5 );
	}
	// E 결혼 추가 added by hseos 2008.01.30

	// 081218 LUJ, 해제하기 전에 값을 복사하자
	const DWORD		playerIndex		= pPlayer->GetID();
	const LEVELTYPE	level			= pPlayer->GetLevel();
	const EXPTYPE	experience		= pPlayer->GetPlayerExpPoint();
	const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
	const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );

	g_pServerSystem->RemovePlayer(pPlayer->GetID());
	
	// 리턴 받으면 맵 아웃 메시지 보냄.
	// 081218 LUJ, 플레이어 소지금 처리 추가
	SaveMapChangePointReturn(
		playerIndex,
		ChangeInfo->Kind,
		dwConnectionIndex,
		level,
		experience,
		inventoryMoney,
		storageMoney );
}

// 070917 LYW --- MapNetworkMsgParser : Add function to process change map through the npc.
void UserConn_Npc_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pPlayer == NULL) return;

	// RaMa - 04.12.14
	pPlayer->UpdateLogoutToDB(FALSE);
	pPlayer->SetMapMoveInfo( ( MAPTYPE )( pmsg->dwData1 ), pmsg->dwData2, pmsg->dwData3 );

	// 081218 LUJ, 해제하기 전에 값을 복사하자
	const DWORD		playerIndex		= pPlayer->GetID();
	const LEVELTYPE	level			= pPlayer->GetLevel();
	const EXPTYPE	experience		= pPlayer->GetPlayerExpPoint();
	const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
	const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );
	
	g_pServerSystem->RemovePlayer( playerIndex );

	// 리턴 받으면 맵 아웃 메시지 보냄.
	// 081218 LUJ, 플레이어 소지금 처리 추가
	SaveNpcMapChangePointReturn(
		playerIndex,
		WORD( pmsg->dwData1 ),
		dwConnectionIndex,
		level,
		experience,
		inventoryMoney,
		storageMoney );
}

void UserConn_Return_Syn(DWORD dwConnectionIndex, char* pMsg) 
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;

	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pPlayer == NULL)
		return;

	LOGINPOINT_INFO* ReturnInfo = NULL;

	ReturnInfo = GAMERESRCMNGR->GetLoginPointInfo( pmsg->wData );

	if( !ReturnInfo )
	{
		return;
	}


	if( ReturnInfo->MapNum == GAMERESRCMNGR->GetLoadMapNum() )
	{
		VECTOR3 RandPos;

		int temp;
		temp = rand() % 500 - 250;
		RandPos.x = ReturnInfo->CurPoint[0].x + temp;
		temp = rand() % 500 - 250;
		RandPos.z = ReturnInfo->CurPoint[0].z + temp;
		RandPos.y = 0;

		// 091214 ShinJS --- 탈것 이동처리
		CVehicle* pVehicle = ( CVehicle* )g_pUserTable->FindUser( pPlayer->GetSummonedVehicle() );
		if( pVehicle &&	pVehicle->GetObjectKind() == eObjectKind_Vehicle )
		{
			// 이동형
			if( pVehicle->GetMoveSpeed() > 0.0f )
			{
				pVehicle->Recall( RandPos );
			}
			// 고정형
			else
			{
				// 탑승중인 경우
				if( pPlayer->GetMountedVehicle() != 0 )
					VEHICLEMGR->Dismount( pPlayer->GetID(), FALSE );
			}
		}

		CCharMove::Warp(pPlayer,&RandPos);

		return;
	}

	const BOOL vehicleFailedToTeleport = ( FALSE == VEHICLEMGR->Teleport( pPlayer->GetID(), (MAPTYPE)ReturnInfo->MapNum ) );
	if( vehicleFailedToTeleport )
	{
		VEHICLEMGR->Unsummon( pPlayer->GetID(), FALSE );
	}

	// RaMa - 04.12.14
	pPlayer->UpdateLogoutToDB(FALSE);
	
	VECTOR3 RandPos = {0};

	int temp;
	temp = rand() % 500 - 250;
	RandPos.x = ReturnInfo->CurPoint[0].x + temp;
	temp = rand() % 500 - 250;
	RandPos.z = ReturnInfo->CurPoint[0].z + temp;

	pPlayer->SetMapMoveInfo( ReturnInfo->MapNum, (DWORD)RandPos.x, (DWORD)RandPos.z );

	// 081218 LUJ, 해제하기 전에 값을 복사하자
	const DWORD		playerIndex		= pPlayer->GetID();
	const LEVELTYPE	level			= pPlayer->GetLevel();
	const EXPTYPE	experience		= pPlayer->GetPlayerExpPoint();
	const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );
	const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );

	g_pServerSystem->RemovePlayer(pPlayer->GetID());

	// 리턴 받으면 맵 아웃 메시지 보냄.
	// 081218 LUJ, 플레이어 소지금 처리 추가
	SaveMapChangePointReturn(
		playerIndex,
		ReturnInfo->MapNum + 2000,
		dwConnectionIndex,
		level,
		experience,
		inventoryMoney,
		storageMoney );
}


void UserConn_NpcAdd_Syn(char* pMsg)
{
	MSG_EVENT_NPCSUMMON* pmsg = (MSG_EVENT_NPCSUMMON*)pMsg;



	int i;
	for(i=0; i<pmsg->cbNpcCount; i++)
	{
		VECTOR3 vOutPos = pmsg->Pos;

		MAPTYPE MapNum = pmsg->wMap;
		vOutPos.x = ((DWORD)(vOutPos.x/50.0f)*50.0f);
		vOutPos.z = ((DWORD)(vOutPos.z/50.0f)*50.0f);



		BASEOBJECT_INFO Baseinfo;
		NPC_TOTALINFO NpcTotalInfo;
		NPC_LIST* pNpcList = GAMERESRCMNGR->GetNpcInfo(pmsg->NpcKind);
		if(pNpcList == 0)
			return;

		DWORD dwID = 0;
		Baseinfo.dwObjectID = g_pAISystem.GeneraterMonsterID();
		Baseinfo.BattleID = pmsg->cbChannel;
		
		SafeStrCpy(Baseinfo.ObjectName, pNpcList->Name, MAX_NAME_LENGTH+1);
		NpcTotalInfo.Group = 0;
		NpcTotalInfo.MapNum = MapNum;
		NpcTotalInfo.NpcJob = pNpcList->JobKind;
		NpcTotalInfo.NpcKind = pNpcList->NpcKind;
		NpcTotalInfo.NpcUniqueIdx = (WORD)dwID;

		CNpc* pNpc = g_pServerSystem->AddNpc(&Baseinfo, &NpcTotalInfo, &vOutPos);
		if(pNpc)
		{
			pNpc->SetDieTime(0);	// 자동소멸되지 않도록.
		}
	}
}

void UserConn_NpcDie_Syn(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CNpc* pNpc = (CNpc*)g_pUserTable->FindUser(pmsg->dwData);
	if(pNpc == NULL)	return;

	MSG_DWORD2 msg;
	msg.Category	= MP_USERCONN;

	if(pNpc->GetObjectKind() & eObjectKind_Npc)
	{
		msg.Protocol	= MP_USERCONN_NPC_DIE;

		msg.dwObjectID	= 0;
		msg.dwData1		= pmsg->dwObjectID;
		msg.dwData2		= pNpc->GetID();
		
		PACKEDDATA_OBJ->QuickSend((CObject*)pNpc,&msg,sizeof(msg));
	}

	pNpc->Die(NULL);
}


void UserConn_ChangeMap_Nack() 
{
	g_Console.Log(eLogDisplay, 4, "Recv OK!!!");
}


void UserConn_SavePoint_Syn(char* pMsg) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if( pPlayer == NULL )	return;

	LOGINPOINT_INFO* LoginInfo = GAMERESRCMNGR->GetLoginPointInfo(pmsg->dwData);
	if( LoginInfo == NULL )	return;

	// checkhacknpc
	if( CheckHackNpc( pPlayer, (WORD)pmsg->dwData ) == FALSE )
		return;
	//
	
	WORD ServerMapNum = GAMERESRCMNGR->GetLoadMapNum();
	SavePointUpdate(pmsg->dwObjectID, LoginInfo->Kind, ServerMapNum);

	// 맵전환포인트를 항상 0으로 셋팅해준다.
/*			MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( (WORD)pmsg->dwData1 );
	if( !ChangeInfo ) return;
	SaveMapChangePointUpdate( pPlayer->GetID(), ChangeInfo->Kind );*/

	SaveMapChangePointUpdate(pmsg->dwObjectID, 0);
	
	pPlayer->InitMapChangePoint();
	pPlayer->SetLoginPoint(LoginInfo->Kind);
}


void UserConn_BackToCharSel_Syn(MSGBASE* pTempMsg, DWORD dwConnectionIndex, char* pMsg) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pPlayer == NULL)
		return;
	// RaMa - 04.12.14
	pPlayer->UpdateLogoutToDB();
	// 090316 LUJ, 탈 것을 소환 해제한다
	VEHICLEMGR->Dismount( pPlayer->GetID(), FALSE );
	PARTYMGR->UserLogOut(pPlayer);
	FriendNotifyLogouttoClient(pTempMsg->dwObjectID);
	GUILDMGR->UserLogOut(pPlayer);
	//GUILDMGR->RemovePlayer( pPlayer->GetGuildIdx(), pPlayer->GetID() );

	// 100105 ONS PC방버프지급 여부정보를 초기화한다.
	PCROOMMGR->RemovePCRoomBuff( pPlayer->GetID() );

	ConfirmUserOut(
		dwConnectionIndex,
		pPlayer->GetUserID(),
		pmsg->dwObjectID,
		1);
	// 마지막에 실행되어야 한다
	g_pServerSystem->RemovePlayer(
		pmsg->dwObjectID);
}


void UserConn_SetVisible(char* pMsg) 
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pPlayer == NULL)
		return;
	pPlayer->SetVisible(FALSE);
	PACKEDDATA_OBJ->QuickSendExceptObjectSelf(pPlayer,pmsg,sizeof(MSG_DWORD2));
}

void MP_MOVEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MOVEMGR->NetworkMsgParse(
		( MSGBASE* )pMsg,
		dwLength );
}

void MP_ITEMMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE *pmsg = (MSGBASE *)pMsg;
	ITEMMGR->NetworkMsgParse( dwConnectionIndex, pmsg->Protocol, pMsg );
}
void MP_SKILLTREEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	SKILLTREEMGR->NetworkMsgParse(pTempMsg->Protocol, pMsg);
}
void MP_CHEATMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	CPlayer* pSender = (CPlayer*)g_pUserTable->FindUser( pTempMsg->dwObjectID );
	if(pSender)
	{
		if( pSender->GetUserLevel() > eUSERLEVEL_GM )
		{
			char buf[64];
			sprintf(buf, "User Lvl Chk! Character_idx : %d", pTempMsg->dwObjectID);
			ASSERTMSG(0, buf);
			return;
		}
	}
	switch(pTempMsg->Protocol)
	{
	case MP_CHEAT_WHEREIS_SYN:					Cheat_WhereIs_Syn(dwConnectionIndex, pMsg) ;			break;
	case MP_CHEAT_HIDE_SYN:						Cheat_Hide_Syn(pMsg) ;									break;
	case MP_CHEAT_PKALLOW_SYN:					Cheat_PkAllow_Syn(pSender, pMsg) ;						break;
	case MP_CHEAT_MOVETOCHAR_SYN:				Cheat_MoveToChar_Syn(pMsg) ;							break;
	case MP_CHEAT_MOVE_SYN:						Cheat_Move_Syn(pMsg) ;									break;		
	case MP_CHEAT_MOVEUSER_SYN:					Cheat_MoveUser_Syn(pMsg) ;								break;		
	case MP_CHEAT_CHANGEMAP_SYN:
		{
			const MSG_NAME_DWORD2* const message = ( MSG_NAME_DWORD2* )pMsg;
			const DWORD playerIndex = message->dwObjectID;
			const MAPTYPE mapType = MAPTYPE(message->dwData1);
			const BOOL vehicleFailedToTeleport = ( FALSE == VEHICLEMGR->Teleport( playerIndex, mapType ) );
			
			if( vehicleFailedToTeleport )
			{
				VEHICLEMGR->Unsummon( playerIndex, FALSE );
				Cheat_ChangeMap_Syn( pMsg );
			}

			break;
		}
	case MP_CHEAT_ITEM_SYN:						Cheat_Item_Syn(pMsg) ;									break;
	case MP_CHEAT_MANA_SYN:						Cheat_Mana_Syn(pMsg) ;									break;		
	case MP_CHEAT_UPDATE_SKILL_SYN:				Cheat_Update_Skill_Syn(pMsg) ;							break;
	case MP_CHEAT_MONEY_SYN:					Cheat_Money_Syn(pMsg) ;									break;
	case MP_CHEAT_RELOADING:					Cheat_ReLoading() ;										break;
	case MP_CHEAT_LEVELUP_SYN:					Cheat_LevelUp_Syn(pMsg) ;								break;
	case MP_CHEAT_PET_LEVELUP_SYN:				Cheat_PetLevelUp_Syn(pMsg) ;								break;
	case MP_CHEAT_EVENT_MONSTER_REGEN:			Cheat_Event_Monster_Regen(pMsg) ;						break;
	case MP_CHEAT_EVENT_MONSTER_DELETE:			Cheat_Event_Monster_Delete(pMsg) ;						break;
	case MP_CHEAT_STR_SYN:						Cheat_Str_Syn(pMsg) ;									break;
	case MP_CHEAT_DEX_SYN:						Cheat_Dex_Syn(pMsg) ;									break;
	case MP_CHEAT_VIT_SYN:						Cheat_Vit_Syn(pMsg) ;									break;
	case MP_CHEAT_WIS_SYN:						Cheat_Wis_Syn(pMsg) ;									break;
	case MP_CHEAT_INT_SYN:						Cheat_Int_Syn(pMsg) ;									break;
	case MP_CHEAT_KILL_ALLMONSTER_SYN:			Cheat_Kill_AllMonster_Syn() ;							break;
	case MP_CHEAT_PARTYINFO_SYN:				Cheat_PartyInfo_Syn(pMsg) ;								break;
	case MP_CHEAT_EVENT_SYN:					Cheat_Event_Syn(pMsg) ;									break;
	case MP_CHEAT_PLUSTIME_ALLOFF:				Cheat_PlusTime_AllOff(pMsg) ;							break;
	case MP_CHEAT_PLUSTIME_ON:					Cheat_PlusTime_On(pMsg) ;								break;
	case MP_CHEAT_PLUSTIME_OFF:					Cheat_PlusTime_Off(pMsg) ;								break;
	case MP_CHEAT_MUNPADATECLR_SYN:				Cheat_MunpaDateClr_Syn(pMsg) ;							break;
	case MP_CHEAT_GUILDUNION_CLEAR:				Cheat_GuildUnion_Clear(pMsg) ;							break;
	case MP_CHEAT_MOB_CHAT:						Cheat_Mob_Chat(pMsg) ;									break;
	case MP_CHEAT_NPCHIDE_SYN:					Cheat_NpcHide_Syn(pMsg) ;								break;
	case MP_CHEAT_SKILLPOINT_SYN:				Cheat_SkillPoint_Syn(pMsg);								break;
	case MP_CHEAT_GOD_SYN:						Cheat_God_Syn(pMsg);									break;
	// 081022 LYW --- Pet : 펫 버그/기타 테스트를 위하여 Pet에도 GOD 모드를 적용한다.
	case MP_CHEAT_PETGOD_SYN :					Cheat_PetGod_Syn(pMsg) ;								break ;
	case MP_CHEAT_RESET_INVENTORY_SYN :			Cheat_Reset_Inventory(pMsg);							break ;
	case MP_CHEAT_PVP_DAMAGE_RATE_SYN :			Cheat_PvP_Damage_Rate(pMsg);							break ;
	case MP_CHEAT_CELAR_INVENTORY_SYN:			Cheat_Clear_Inventory(pMsg);							break;

//---KES CHEAT PKEVENT
	case MP_CHEAT_PKEVENT:
		{
			MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;
			if( pmsg->bData == 0 )
				PKMGR->SetPKEvent( FALSE );
			else if( pmsg->bData == 1 )
				PKMGR->SetPKEvent( TRUE );
		}
		break;
//--------------------
//---KES CHEAT CHANGESIZE
	case MP_CHEAT_CHANGESIZE:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwData1 );
			if( !pPlayer ) return;

			float fSize = (float)pmsg->dwData2 / 100.f;
			if( fSize > 3.0f || fSize < .3f ) return;
			
			pPlayer->SetCharacterSize( fSize );

			MSG_DWORD2 msg;
			msg.Category	= MP_CHEAT;
			msg.Protocol	= MP_CHEAT_CHANGESIZE;
			msg.dwData1		= pmsg->dwData1;
			msg.dwData2		= pmsg->dwData2;

			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
		}
		break;
	case MP_CHEAT_FISHING_FISHINGEXP:
		Cheat_Fishing_Exp(pMsg);
		break;
	case MP_CHEAT_FISHING_FISHPOINT:
		Cheat_Fishing_FishPoint(pMsg);
		break;
	case MP_CHEAT_VEHICLE_SUMMON_SYN:
		{
			const MSG_DWORD3* const message = (MSG_DWORD3*)pMsg;
			const DWORD playerIndex = message->dwObjectID;
			const DWORD monsterKind = message->dwData1;
			VECTOR3 position = {
				float(message->dwData2),
				0,
				float(message->dwData3),
			};

			VEHICLEMGR->Summon(
				monsterKind,
				position,
				playerIndex);
			VEHICLEMGR->Save();
		}
		break;
	case MP_CHEAT_VEHICLE_GET_SYN:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;
			const DWORD playerIndex = message->dwObjectID;

			VEHICLEMGR->GetSummonedVehicle(
				playerIndex);
		}
		break;
	case MP_CHEAT_VEHICLE_UNSUMMON_SYN:
		{
			const MSG_DWORD2* const message = (MSG_DWORD2*)pMsg;
			const DWORD playerIndex = message->dwObjectID;
			VECTOR3 position = {
				float(message->dwData1),
				0,
				float(message->dwData2),
			};

			VEHICLEMGR->Unsummon(
				position);
			VEHICLEMGR->Save();
			VEHICLEMGR->GetSummonedVehicle(
				playerIndex);
		}
		break;
	case MP_CHEAT_MONSTER_SCRIPT_SYN:
		{
			const MSG_GUILDNOTICE* const receivedMessage = (MSG_GUILDNOTICE*)pMsg;
			const DWORD objectIndex = receivedMessage->dwGuildId;
			LPCTSTR fileName = receivedMessage->Msg;

            CMonster* const monsterObject = (CMonster*)g_pUserTable->FindUser(
				objectIndex);

			if(0 == monsterObject)
			{
				break;
			}
			else if(FALSE == (eObjectKind_Monster & monsterObject->GetObjectKind()))
			{
				break;
			}

			const BOOL isInitialized = monsterObject->GetFiniteStateMachine().Initialize(
				fileName,
				objectIndex,
				monsterObject->GetGridID());

			if(FALSE == isInitialized)
			{
				TESTMSG sendMessage;
				ZeroMemory(
					&sendMessage,
					sizeof(sendMessage));
				sendMessage.Category = MP_CHEAT;
				sendMessage.Protocol = MP_CHEAT_MONSTER_SCRIPT_NACK;
				SafeStrCpy(
					sendMessage.Msg,
					fileName,
					_countof(sendMessage.Msg));

				CObject* const object = g_pUserTable->FindUser(
					receivedMessage->dwObjectID);

				if(0 == object)
				{
					break;
				}

				object->SendMsg(
					&sendMessage,
					sendMessage.GetMsgLength());
				break;
			}

			SafeStrCpy(
				monsterObject->GetMonsterTotalInfo().mScriptName,
				fileName,
				_countof(monsterObject->GetMonsterTotalInfo().mScriptName));
			monsterObject->AddChat(
				fileName);

			TESTMSG sendMessage;
			ZeroMemory(
				&sendMessage,
				sizeof(sendMessage));
			sendMessage.Category = MP_CHEAT;
			sendMessage.Protocol = MP_CHEAT_MONSTER_SCRIPT_ACK;
			sendMessage.dwObjectID = objectIndex;
			SafeStrCpy(
				sendMessage.Msg,
				fileName,
				_countof(sendMessage.Msg));
			PACKEDDATA_OBJ->QuickSend(
				monsterObject,
				&sendMessage,
				sendMessage.GetMsgLength());
		}
		break;
	case MP_CHEAT_DUNGEON_OBSERVER_SYN:
		Cheat_Dungeon_Observer_Syn(dwConnectionIndex, pMsg);
		break;

	case MP_CHEAT_DUNGEON_OBSERVER_ACK:
		{
			MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(!pPlayer)	return;

			pPlayer->UpdateLogoutToDB(FALSE);
			g_pServerSystem->RemovePlayer(pPlayer->GetID());

			MSG_WORD2 msg;
			msg.Category = MP_CHEAT;
			msg.Protocol = MP_CHEAT_DUNGEON_OBSERVER_ACK;
			msg.dwObjectID = pmsg->dwObjectID;
			msg.wData1 = pmsg->wData1;
			msg.wData2 = pmsg->wData2;
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
		break;

	case MP_CHEAT_DUNGEON_OBSERVERJOIN_SYN:
		Cheat_Dungeon_ObserverJoin_Syn(dwConnectionIndex, pMsg);
		break;

	case MP_CHEAT_DUNGEON_GETINFOALL_SYN:
		Cheat_Dungeon_GetInfoAll_Syn(dwConnectionIndex, pMsg);
		break;

	case MP_CHEAT_DUNGEON_GETINFOALL_ACK:
		{
			MSG_DUNGEON_INFO_ALL* pmsg = (MSG_DUNGEON_INFO_ALL*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(!pPlayer)	return;

			pPlayer->SendMsg(pmsg, dwLength);
		}
		break;

	case MP_CHEAT_DUNGEON_GETINFOONE_SYN:
		Cheat_Dungeon_GetInfoOne_Syn(dwConnectionIndex, pMsg);
		break;

	case MP_CHEAT_DUNGEON_GETINFOONE_ACK:
		{
			MSG_DUNGEON_INFO_ONE* pmsg = (MSG_DUNGEON_INFO_ONE*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(!pPlayer)	return;

			pPlayer->SendMsg(pmsg, dwLength);
		}
		break;
	// 100611 ONS 채팅 금지 기능 추가
	case MP_CHEAT_FORBID_CHAT_SYN:				Cheat_ForbidChat( pMsg );							break;
	case MP_CHEAT_FORBID_CHAT_OTHERMAP_SYN:		Cheat_ForbidChat_Server( dwConnectionIndex, pMsg );	break;
	case MP_CHEAT_PERMIT_CHAT_SYN:				Cheat_PermitChat( pMsg );							break;
	case MP_CHEAT_PERMIT_CHAT_OTHERMAP_SYN:		Cheat_PermitChat_Server( dwConnectionIndex, pMsg );	break;
	}
}


void Cheat_WhereIs_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pSearcher	= (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	CPlayer* pTarget	= (CPlayer*)g_pUserTable->FindUser( pmsg->dwData );
	if( !pTarget )
	{
		MSG_BYTE msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_WHEREIS_NACK;
		msg.dwObjectID	= pmsg->dwObjectID;
		msg.bData		= CHATERR_ERROR;
		if( pSearcher )
			pSearcher->SendMsg( &msg, sizeof( msg ) );
		else
			g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );

		return;
	}

	MSG_WHERE_INFO msgWhere;
	msgWhere.Category	= MP_CHEAT;
	msgWhere.Protocol	= MP_CHEAT_WHEREIS_MAPSERVER_ACK;
	msgWhere.dwObjectID	= pmsg->dwObjectID;
	msgWhere.wMapNum	= g_pServerSystem->GetMapNum();
	msgWhere.bChannel	= (BYTE)pTarget->GetGridID();

	VECTOR3 TargetPos = *CCharMove::GetPosition(pTarget);
	msgWhere.cpos.Compress(&TargetPos);

	if( pSearcher )
		pSearcher->SendMsg( &msgWhere, sizeof( msgWhere ) );
	else
		g_Network.Send2Server( dwConnectionIndex, (char*)&msgWhere, sizeof( msgWhere ) );
}


void Cheat_Hide_Syn(char* pMsg) 
{
	MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;

	CPlayer* pPlayer;

	if( pmsg->Name[0] == 0 )	//hero
		pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	else
		pPlayer = (CPlayer*)g_pUserTable->FindUserForName( pmsg->Name );
	if( !pPlayer )	return;
	
	if( pmsg->dwData == 1 )
	{
		if( pPlayer->IsVisible() == FALSE ) return;
		pPlayer->SetVisible(FALSE);
	}
	else
	{
		if( pPlayer->IsVisible() == TRUE )	return;
		pPlayer->SetVisible(TRUE);
	}

	if( pPlayer->GetUserLevel() <= eUSERLEVEL_GM )
	{
		CGridTable* const pGridTable = g_pServerSystem->GetGridSystem()->GetGridTable( pPlayer );

		if(0 == pGridTable)
			return;

		MSGBASE* message = 0;
		DWORD messageLength = 0;

		// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
		if( pPlayer->IsVisible() )
		{
			messageLength = pPlayer->SetAddMsg(
				0,
				TRUE,
				message);
		}
		else
		{
			messageLength = pPlayer->SetRemoveMsg(
				0,
				message);
		}

		PACKEDDATA_OBJ->QuickSendExceptObjectSelf(
			pPlayer,
			message,
			messageLength);
	}

	MSG_DWORD2 msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_HIDE_ACK;
	msg.dwObjectID	= pPlayer->GetID();
	msg.dwData1		= pPlayer->GetID();
	msg.dwData2		= pmsg->dwData;

	PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof(MSG_DWORD2) );
}

void Cheat_PkAllow_Syn(CPlayer* pSender, char* pMsg) 
{
	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;
	BOOL bAllow = (BOOL)pmsg->wData2;
	PKMGR->SetPKAllowWithMsg( bAllow );
}


void Cheat_MoveToChar_Syn(char* pMsg) 
{
	MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;

	CObject* pPlayer = g_pUserTable->FindUser( pmsg->dwObjectID );
	if( !pPlayer ) return;
	CObject* pTarget = g_pUserTable->FindUserForName( pmsg->Name );			
	if( !pTarget ) return;
	
	VECTOR3 TargetPos;

	MOVE_POS msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_MOVE_ACK;

	if( pmsg->dwData == 0 )
	{
		TargetPos = *CCharMove::GetPosition(pTarget);
		CCharMove::SetPosition( pPlayer,&TargetPos);
		msg.cpos.Compress(&TargetPos);
		((CPlayer*)pPlayer)->SendMsg( &msg, sizeof(msg) );
	}
	else
	{
		TargetPos = *CCharMove::GetPosition(pPlayer);
		CCharMove::SetPosition( pTarget,&TargetPos);
		msg.cpos.Compress(&TargetPos);
		((CPlayer*)pTarget)->SendMsg( &msg, sizeof(msg) );
		//그리드에만 보내면 되는가?
	}
}


void Cheat_Move_Syn(char* pMsg) 
{
	MOVE_POS* pmsg = (MOVE_POS*)pMsg;

	CObject* pObject = g_pUserTable->FindUser(pmsg->dwMoverID);
	if(pObject == NULL)
		return;

	VECTOR3 pos;
	pmsg->cpos.Decompress(&pos);
	
	//ASSERTVALID_POSITION(pos);
	if(CheckValidPosition(pos) == FALSE)
	{
		return;
	}

	CCharMove::SetPosition(pObject,&pos);

	pmsg->Protocol = MP_CHEAT_MOVE_ACK;

	pObject->SendMsg(pmsg,sizeof(MOVE_POS));
}


void Cheat_MoveUser_Syn(char* pMsg) 
{
	MOVE_POS_USER* pmsg = (MOVE_POS_USER*)pMsg;

	CObject* pObject = g_pUserTable->FindUserForName(pmsg->Name);
	if(pObject == NULL)
		return;

	VECTOR3 pos;
	pmsg->cpos.Decompress(&pos);
	CCharMove::SetPosition(pObject,&pos);

	MOVE_POS posMsg;
	posMsg.Category = MP_CHEAT;
	posMsg.Protocol = MP_CHEAT_MOVE_ACK;
	posMsg.cpos.Compress(&pos);

	((CPlayer*)pObject)->SendMsg(&posMsg,sizeof(posMsg));
	//내 그리드에만 보낸다?
}


void Cheat_ChangeMap_Syn(char* pMsg) 
{
	MSG_NAME_DWORD2* pmsg = (MSG_NAME_DWORD2*)pMsg;
	CPlayer* pPlayer;

	if( pmsg->Name[0] == 0 )
		pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	else
		pPlayer = (CPlayer*)g_pUserTable->FindUserForName(pmsg->Name);

	if(pPlayer == NULL)
		return;
	// 090701 LUJ, 맵 이동 시 플레이어 정보를 DB에 쓰는데, 초기화가 끝나지 않았을 경우 쓰레기값이
	//		저장될 수 있다. 초기화가 끝난 객체만 처리한다
	else if( FALSE == pPlayer->GetInited() )
	{
		return;
	}

	//---KES PK 071202
	//---KES AUTONOTE
	if( pPlayer->IsPKMode() || LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) || pPlayer->GetAutoNoteIdx() )
	{
		MSG_DWORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_CHANGEMAP_NACK;
		msg.dwData		= pPlayer->GetGridID() - 1;
		pPlayer->SendMsg( &msg, sizeof( msg ) );
		return;
	}
		
	MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( (WORD)pmsg->dwData1 );
	if( !ChangeInfo ) return;	//nackif( !ChangeInfo ) return;	//nack
	SaveMapChangePointUpdate( pPlayer->GetID(), ChangeInfo->Kind );

	// RaMa - 04.12.14
	pPlayer->UpdateLogoutToDB(FALSE);

	VECTOR3 RandPos = { 0 };

	if( pPlayer->GetMountedVehicle() )
	{
		RandPos = ChangeInfo->MovePoint;
	}
	else
	{
		const float variation = float(rand() % 500 - 250);
		RandPos.x = ChangeInfo->MovePoint.x + variation;
		RandPos.z = ChangeInfo->MovePoint.z + variation;
	}

	pPlayer->SetMapMoveInfo( ChangeInfo->MoveMapNum, (DWORD)RandPos.x, (DWORD)RandPos.z);

	// 081218 LUJ, 플레이어를 해제하기 전에 DB에 업데이트할 주요 정보를 복사해놓는다
	const MAPTYPE	mapType			= MAPTYPE( pmsg->dwData1 );
	const WORD		channelIndex	= WORD( pmsg->dwData2 );
	const DWORD		playerIndex		= pPlayer->GetID();
	const DWORD		userIndex		= pPlayer->GetUserID();
	const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
	const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );

	g_pServerSystem->RemovePlayer( pPlayer->GetID() );

	// 081218 LUJ, 업데이트보다 맵 이동이 빨리 진행될 경우 업데이트되지 않은 정보가 오게된다.
	//			이를 막기 위해 프로시저 처리 완료 후 진행한다
	UpdatePlayerOnMapOut(
		playerIndex,
		userIndex,
		inventoryMoney,
		storageMoney,
		mapType,
		channelIndex );
}

void Cheat_Item_Syn(char* pMsg) 
{
	MSG_DWORD_WORD* pmsg = (MSG_DWORD_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;

	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( pmsg->dwData );
	if(!pItemInfo)		return;

	ITEMMGR->CheatObtainItem( pPlayer, pmsg->dwData, pmsg->wData );
}

void Cheat_Mana_Syn(char* pMsg)
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;
	int tmp = pPlayer->GetMaxMana() - (pPlayer->GetMana()+pmsg->wData);
	if(tmp < 0)
		pPlayer->SetMana(pPlayer->GetMaxMana());
	else
		pPlayer->SetMana(pPlayer->GetMana()+pmsg->wData);
}

void Cheat_Update_Skill_Syn(char* pMsg) 
{
	MSG_DWORD_WORD* pmsg = (MSG_DWORD_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;

	SKILL_BASE* pSkillBase = pPlayer->GetSkillTree().GetData(pmsg->dwData);

	if( pSkillBase )
	{
		pSkillBase->Level = ( BYTE )( pmsg->wData );
		
		SKILLTREEMGR->UpdateSkill( pSkillBase, pPlayer );
		SKILLTREEMGR->UpdateAck( pPlayer, pSkillBase );

		// 071109 웅주, 스킬 로그 추가
		InsertLogSkill( pPlayer, pSkillBase, eLog_SkillCheatLevelup );
	}
	else
	{
		SKILL_BASE skill;
		skill.dwDBIdx = 0;
		skill.wSkillIdx = pmsg->dwData;
		skill.Level = ( BYTE )( pmsg->wData );

		SKILLTREEMGR->AddSkill( pPlayer, &skill );
	}
}


void Cheat_Money_Syn(char* pMsg) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;

	if( pPlayer->GetMoney() > pmsg->dwData )
	{
		pPlayer->SetMoney(pPlayer->GetMoney() - pmsg->dwData, MONEY_SUBTRACTION, 0, eItemTable_Inventory, eMoneyLog_GetCheatMoney, 0);
	}
	else if( pPlayer->GetMoney() < pmsg->dwData )
	{
		pPlayer->SetMoney(pmsg->dwData - pPlayer->GetMoney(), MONEY_ADDITION, 0, eItemTable_Inventory, eMoneyLog_GetCheatMoney, 0);
	}
	
	LogCheat(pPlayer->GetID(),eCHT_Money,pmsg->dwData,0,0);		
}


void Cheat_ReLoading() 
{
	g_pServerSystem->ReloadResourceData();
}


void Cheat_LevelUp_Syn(char* pMsg) 
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) 
		return;
	EXPTYPE pointForLevel=0;
	LEVELTYPE curlvl = pPlayer->GetLevel();
	if(curlvl > pmsg->wData)
		return;
	else
	{
		LEVELTYPE diff = pmsg->wData - curlvl;
		for(LEVELTYPE i=0; i<diff; ++i)
		{
			pointForLevel = pointForLevel + GAMERESRCMNGR->GetMaxExpPoint(pPlayer->GetLevel()+i);
			
		}
		pPlayer->SetLevel(pmsg->wData-1);
		pPlayer->SetPlayerExpPoint(pointForLevel);
		pPlayer->SetPlayerExpPoint(1);					
		
		// YH
		CharacterHeroInfoUpdate(pPlayer);
		CharacterTotalInfoUpdate(pPlayer);		

		LogCheat(pPlayer->GetID(),eCHT_LevelUp,pmsg->wData,0,0);		
	}
}

void Cheat_PetLevelUp_Syn(char* pMsg) 
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) 
		return;

	CPet* const petObject = PETMGR->GetPet(
		pPlayer->GetPetItemDbIndex());

	if(0 == petObject)
	{
		return;
	}

	const BYTE cheatLevel = BYTE(pmsg->wData);
	const PET_STATUS_INFO* const petInfo = PETMGR->GetPetStatusInfo(
		petObject->GetObjectInfo().Type,
		cheatLevel);

	if(0 == petInfo)
	{
		return;
	}
    
	petObject->SetLevel(
		cheatLevel);
}

void Cheat_Event_Monster_Regen(char* pMsg) 
{
	MSG_EVENT_MONSTERREGEN* pmsg = (MSG_EVENT_MONSTERREGEN*)pMsg;
	CObject* pPlayer = g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;
	RECALLMGR->EventRecall( pPlayer, pmsg->MonsterKind, pmsg->cbMobCount, pmsg->cbChannel, &pmsg->Pos, pmsg->wRadius, pmsg->ItemID, pmsg->dwDropRatio );
}


void Cheat_Event_Monster_Delete(char* pMsg) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	CObject* pPlayer = g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;

	RECALLMGR->DeleteEventRecallMonster();
}


void Cheat_Str_Syn(char* pMsg) 
{
	MSG_WORD *pmsg = (MSG_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;
	if((pmsg->wData < 1) || (pmsg->wData > 9999))
		return;
	pPlayer->SetStrength(pmsg->wData);
	LogCheat(pPlayer->GetID(),eCHT_Str,pmsg->wData,0,0);
}


void Cheat_Dex_Syn(char* pMsg) 
{
	MSG_WORD *pmsg = (MSG_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;
	if((pmsg->wData < 1) || (pmsg->wData > 9999))
		return;
	pPlayer->SetDexterity(pmsg->wData);
	LogCheat(pPlayer->GetID(),eCHT_Dex,pmsg->wData,0,0);
}


void Cheat_Vit_Syn(char* pMsg)
{
	MSG_WORD *pmsg = (MSG_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;
	if((pmsg->wData < 1) || (pmsg->wData > 9999))
		return;
	pPlayer->SetVitality(pmsg->wData);
	LogCheat(pPlayer->GetID(),eCHT_Vit,pmsg->wData,0,0);
}


void Cheat_Wis_Syn(char* pMsg) 
{
	MSG_WORD *pmsg = (MSG_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;
	if((pmsg->wData < 1) || (pmsg->wData > 9999))
		return;
	pPlayer->SetWisdom(pmsg->wData);
	LogCheat(pPlayer->GetID(),eCHT_Wis,pmsg->wData,0,0);
}
 
void Cheat_Int_Syn(char* pMsg)
{
	MSG_WORD *pmsg = (MSG_WORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer) return;
	if((pmsg->wData < 1) || (pmsg->wData > 9999))
		return;
	pPlayer->SetIntelligence(pmsg->wData);
	LogCheat(pPlayer->GetID(),eCHT_Wis,pmsg->wData,0,0);
}

void Cheat_Kill_AllMonster_Syn() 
{
	CObject* pInfo = NULL;
	g_pUserTable->SetPositionHead();
	while( (pInfo = g_pUserTable->GetData()) != NULL )
	{
		if(pInfo->GetObjectKind() & eObjectKind_Monster)
		{
			if(pInfo->GetGridID() == 1)
			{
				//g_pServerSystem->RemoveMonster( pInfo->GetID() );
				pInfo->Die(NULL);
			}
		}
	}
}


void Cheat_PartyInfo_Syn(char* pMsg)
{
	MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
	CParty * pParty = PARTYMGR->GetParty(pmsg->dwData);
	PARTY_INFO SendParty;
	memset(&SendParty, 0, sizeof(PARTY_INFO));
	if(pParty)
	{
		SendParty.Category = MP_CHEAT;
		SendParty.Protocol = MP_CHEAT_PARTYINFO_ACK;
		SendParty.PartyDBIdx = pmsg->dwData;
		for(int i=0; i<MAX_PARTY_LISTNUM; ++i)
		{
			SendParty.Member[i] = pParty->GetMember(i);
		}
		SendParty.Option = pParty->GetOption();
	}			
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pPlayer)
	{
		pPlayer->SendMsg(&SendParty, sizeof(SendParty));
	}
}


// 080205 LYW --- MapNetworkMsgParser : 이밴트 적용, 전 맵에 처리 되도록 수정.
void Cheat_Event_Syn(char* pMsg) 
{
	if( !pMsg ) return ;														// 메시지 체크.

	MSG_WORD2* pmsg = NULL ;
	pmsg = (MSG_WORD2*)pMsg ;													// 메시지를 변환 받는다.

	if( !pmsg ) return ;														// 변환 된 메시지 체크.

	WORD wEventType = 0 ;
	wEventType = pmsg->wData1 ;													// 이벤트 종류를 받는다.

	WORD wEventRate = 0 ;
	wEventRate = pmsg->wData2 ;													// 이벤트율을 받는다.

	if( wEventRate > 3000 ) return ;											// 최대 30배 제한.

	// 080227 LYW --- MapNetworkMsgParser : EventRateFile도 변경된는 값을 적용한다.
	if( wEventType == eEvent_ExpRate )											// 경험치를 적용한다.
	{
		gEventRate[eEvent_ExpRate] = wEventRate*0.01f;							
		gEventRateFile[eEvent_ExpRate] = wEventRate*0.01f;
	}
	else if( wEventType == eEvent_ItemRate )									// 아이템 드랍률을 적용한다.
	{
		gEventRate[eEvent_ItemRate] = wEventRate*0.01f ;
		gEventRateFile[eEvent_ItemRate] = wEventRate*0.01f;
	}
	else if( wEventType == eEvent_MoneyRate )									// 머니 드랍률을 적용한다.
	{
		gEventRate[eEvent_MoneyRate] = wEventRate*0.01f;
		gEventRateFile[eEvent_MoneyRate] = wEventRate*0.01f;
	}

	// 080118 KTH -- 내용을 파일에 기록하기

	FILE* fp = NULL;
	fp = fopen("./System/Resource/droprate.txt", "wb");

	char lineBuf[128] ;															// 기록을 위한 임시 버퍼 선언 및 초기화.
	char dataBuf[2048] = {0, } ;

	if( !fp )																	// 열기에 실패 했다면, 에러 메시지 출력 후 return.
	{
		char errorBuf[64] = {0,} ;
		strcpy(errorBuf, "UpdateEventRate - Failed to create bin file.") ;		
		g_Console.Log(eLogDisplay, 4, errorBuf) ;
		return ;
	}

	for( int count = 0 ; count < eEvent_Max ; ++count )								
	{
		memset(lineBuf, 0, sizeof(lineBuf)) ;

		switch(count)
		{
		case eEvent_ExpRate :			sprintf(lineBuf, "#EXP\t%f\r\n",			gEventRate[eEvent_ExpRate]) ;			break ;
		case eEvent_AbilRate :			sprintf(lineBuf, "#ABIL\t%f\r\n",			gEventRate[eEvent_AbilRate]) ;			break ;
		case eEvent_ItemRate :			sprintf(lineBuf, "#ITEM\t%f\r\n",			gEventRate[eEvent_ItemRate]) ;			break ;
		case eEvent_MoneyRate :			sprintf(lineBuf, "#MONEY\t%f\r\n",			gEventRate[eEvent_MoneyRate]) ;			break ;
		case eEvent_GetMoney :			sprintf(lineBuf, "#GETMONEY\t%f\r\n",		gEventRate[eEvent_GetMoney]) ;			break ;
		case eEvent_DamageReciveRate :	sprintf(lineBuf, "#DAMAGERECIVE\t%f\r\n",	gEventRate[eEvent_DamageReciveRate]) ;	break ;
		case eEvent_DamageRate :		sprintf(lineBuf, "#DAMAGERATE\t%f\r\n",		gEventRate[eEvent_DamageRate]) ;		break ;
		case eEvent_ManaRate :			sprintf(lineBuf, "#MANASPEND\t%f\r\n",		gEventRate[eEvent_ManaRate]) ;			break ;
		case eEvent_UngiSpeed :			sprintf(lineBuf, "#UNGISPEED\t%f\r\n",		gEventRate[eEvent_UngiSpeed]) ;			break ;
		case eEvent_PartyExpRate :		sprintf(lineBuf, "#PARTYEXP\t%f\r\n",		gEventRate[eEvent_PartyExpRate]) ;		break ;
		case eEvent_SkillExp :			sprintf(lineBuf, "#SKILLEXPRATE\t%f\r\n",	gEventRate[eEvent_SkillExp]) ;			break ;
		}

		if( strlen(lineBuf) == 0 ) continue ;
		strcat(dataBuf, lineBuf) ;
	}

	fwrite(dataBuf, strlen(dataBuf), 1, fp);
	fclose(fp);
}


void Cheat_PlusTime_AllOff(char* pMsg) 
{
	for(int i=0; i<eEvent_Max; ++i)
	{
		if( gEventRate[i] != gEventRateFile[i] )
			break;
	}
	
	if( i != eEvent_Max )
	{
		// 적용
		for(i=0; i<eEvent_Max; ++i)
			gEventRate[i] = gEventRateFile[i];
	}
}


void Cheat_PlusTime_On(char* pMsg) 
{
	// 080118 LYW --- MapNetworkMsgParser : GM툴로 처리 된 이벤트 종류의 범위를 체크.

	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;

	if( pmsg->wData1 >= eEvent_Max ) return ;

//	g_bPlusTime = TRUE;

	// 다른에이전트에서 오는건 막는다.
	if( gEventRate[pmsg->wData1] != pmsg->wData2*0.01f )
		gEventRate[pmsg->wData1] = pmsg->wData2*0.01f;
}


void Cheat_PlusTime_Off(char* pMsg) 
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;

	// 다른에이전트 에서 오는건 막는다.
	if( gEventRate[pmsg->wData] != gEventRateFile[pmsg->wData] )
		gEventRate[pmsg->wData] = gEventRateFile[pmsg->wData];
	else
		return;

	// 모든플러스타임이 종료되었으면 플러스타임 종료
	for(int i=0; i<eEvent_Max; ++i)
	{
		if( gEventRate[i] != gEventRateFile[i] )
			break;
	}
}


void Cheat_MunpaDateClr_Syn(char* pMsg) 
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pPlayer == 0)
		return;
	pPlayer->ClrGuildCanEntryDate();
}

void Cheat_GuildUnion_Clear(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if( !pPlayer )	return;

	GUILDUNIONMGR->ClearEntryTime( pPlayer->GetGuildIdx() );
}


void Cheat_Mob_Chat(char* pMsg)
{
	TESTMSGID* pmsg = (TESTMSGID*)pMsg;
	CObject* pObject = g_pUserTable->FindUser( pmsg->dwSenderID );
	if( pObject == NULL ) return;

	char buf[MAX_CHAT_LENGTH+1];
	SafeStrCpy( buf, pmsg->Msg, MAX_CHAT_LENGTH+1 );

	if( pObject->GetObjectKind() & eObjectKind_Monster )
	{
		((CMonster*)pObject)->AddChat( buf );
	}
	else if( pObject->GetObjectKind() == eObjectKind_Npc )
	{
		((CNpc*)pObject)->AddChat( buf );
	}
}

void Cheat_NpcHide_Syn(char* pMsg)
{
	MSG_WORD3* pmsg = (MSG_WORD3*)pMsg;

	// 숨김 가능 NPC 정보를 가져온다
	HIDE_NPC_INFO* pInfo = GAMERESRCMNGR->GetHideNpcInfo(pmsg->wData1);

	if(pInfo)
	{
		// 전체 채널
		if(pmsg->wData2 == 0)
		{
			for( DWORD i = 0 ; i < MAX_CHANNEL_NUM ; ++i )
				pInfo->ChannelInfo[i] = (BOOL)pmsg->wData3;

			CObject* pObject = NULL;
			g_pUserTable->SetPositionHead();

			MSG_WORD2 msg;
			msg.Category = MP_CHEAT;
			msg.Protocol = MP_CHEAT_NPCHIDE_ACK;
			msg.wData1 = pmsg->wData1;
			msg.wData2 = pmsg->wData3;

			while( (pObject = g_pUserTable->GetData()) != NULL )
			{
				if( pObject->GetObjectKind() == eObjectKind_Player )
					pObject->SendMsg(&msg, sizeof(msg));
			}
		}
		// 특정 채널
		else
		{
			pInfo->ChannelInfo[pmsg->wData2 - 1] = (BOOL)pmsg->wData3;
			
			DWORD dwChannelID = CHANNELSYSTEM->GetChannelID( pmsg->wData2 - 1 );

			if(dwChannelID)
			{
				CObject* pObject = NULL;
				g_pUserTable->SetPositionHead();

				MSG_WORD2 msg;
				msg.Category = MP_CHEAT;
				msg.Protocol = MP_CHEAT_NPCHIDE_ACK;
				msg.wData1 = pmsg->wData1;
				msg.wData2 = pmsg->wData3;

				while( (pObject = g_pUserTable->GetData()) != NULL )
				{
					if( pObject->GetObjectKind() == eObjectKind_Player )
						if( ((CPlayer*)pObject)->GetChannelID() == dwChannelID )
							pObject->SendMsg(&msg, sizeof(msg));
				}
			}
		}
	}
}

void Cheat_SkillPoint_Syn(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	
	// 080611 LYW --- Player : 스킬포인트 업데이트 처리를 수정함.
	// (아이템으로 스킬 포인트를 추가하는 기능이 생겼기 때문.)
	//pPlayer->SetSkillPoint( pmsg->dwData );
	pPlayer->SetSkillPoint( pmsg->dwData, TRUE ) ;
}

void Cheat_God_Syn(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	
	pPlayer->SetGod( pmsg->dwData );
}

// 081022 LYW --- MapNetworkMsgParser : 펫 버그/기타 테스트를 위하여 Pet에도 GOD 모드를 적용한다.
void Cheat_PetGod_Syn(char* pMsg)
{
	const MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(
		pmsg->dwObjectID) ;

	if(0 == pPlayer)
	{
		return;
	}

	CPet* const petObject = PETMGR->GetPet(
		pPlayer->GetPetItemDbIndex());

	if(petObject)
	{
		petObject->SetGod(
			0 < pmsg->dwData);
	}
}

void MP_QUICKMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	QUICKMNGR->NetworkMsgParse(pTempMsg->Protocol, pMsg);
}

void MP_PARTYMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol) 
	{
	case MP_PARTY_ADD_SYN: 
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
//			PARTYMGR->AddParty(pmsg->dwData2,pmsg->dwObjectID,pmsg->dwData1); //include DB
			PARTYMGR->AddPartyInvite(pmsg->dwObjectID, pmsg->dwData1);
		}
		break;

	// 090525 ShinJS --- Same or Different Agent , Different MapServer
	case MP_PARTY_INVITE_BYNAME_SYN:
		{
			const MSG_DWORD* const pmsg = (MSG_DWORD*)pMsg;
			const DWORD memberPlayerIndex = pmsg->dwObjectID;
			const DWORD guestPlayerIndex = pmsg->dwData;
			CPlayer* const memberPlayer = (CPlayer*)g_pUserTable->FindUser(
				memberPlayerIndex);

			if(NULL == memberPlayer)
			{
				break;
			}
			
			CParty* const party = PARTYMGR->GetParty(
				memberPlayer->GetPartyIdx());

			if(NULL == party)
			{
				break;
			}
			else if(party->IsPartyMember(guestPlayerIndex))
			{
				MSG_DWORD message;
				ZeroMemory(
					&message,
					sizeof(message));
				message.Category = MP_PARTY;
				message.Protocol = MP_PARTY_ADD_NACK;
				message.dwData = eErr_Add_AlreadyinParty;
				memberPlayer->SendMsg(
					&message,
					sizeof(message));
				break;
			}

			MSG_NAME_DWORD4 message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_PARTY;
			message.Protocol = MP_PARTY_INVITE_BYNAME_ACK;
			message.dwData1	= memberPlayerIndex;
			message.dwData2	= guestPlayerIndex;
			message.dwData3	= memberPlayer->GetPartyIdx();
			message.dwData4	= DWORD(party->GetOption());
			SafeStrCpy(
				message.Name,
				memberPlayer->GetObjectName(),
				_countof(message.Name));
			g_Network.Send2Server(
				dwConnectionIndex,
				(char*)&message,
				sizeof(message));
		}
		break;
	case MP_PARTY_DEL_SYN: 
		{
			MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
			PARTYMGR->DelMemberSyn(pmsg->dwObjectID, pmsg->dwData);
		}
		break;
	case MP_PARTY_CREATE_SYN: 
		{
			MSG_BYTE2* pmsg = (MSG_BYTE2*) pMsg;
			CPlayer* pMaster = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID ) ;

			if( pMaster == NULL ) return ;

			PARTYMGR->CreateParty(
				pMaster,
				pmsg->bData1,
				pmsg->bData2);
		}
		break;
		// 070110 LYW --- Add case for change pickup item from party part.
	case MP_PARTY_CHANGE_PICKUPITEM_SYN :
		{
			MSG_DWORD2 * pmsg = (MSG_DWORD2*)pMsg;
			CParty* pParty = PARTYMGR->GetParty(pmsg->dwData2) ;
			if( pParty == NULL )
			{
				PARTYMGR->SendErrMsg( pmsg->dwObjectID, eErr_Fail_Change_PickupItem, MP_PARTY_CHANGE_PICKUPITEM_NACK ) ;
			}
			else
			{
				pParty->SetOption(BYTE(pmsg->dwData1));

				MSG_BYTE msg ;
				msg.Category = MP_PARTY;
				msg.Protocol = MP_PARTY_CHANGE_PICKUPITEM_ACK;
				msg.bData	 = (BYTE)pmsg->dwData1 ;

				pParty->SendMsgToAll(
					&msg,
					sizeof(msg));
				pParty->NotifyChangedOption(
					pmsg->dwData1);
			}
		}
		break ;

	case MP_PARTY_CHANGE_DICEGRADE_SYN:
		{
			MSG_DWORD2 * pmsg = (MSG_DWORD2*)pMsg;
			CParty* pParty = PARTYMGR->GetParty(pmsg->dwData2) ;
			if( pParty == NULL )
			{
				PARTYMGR->SendErrMsg( pmsg->dwObjectID, eErr_Fail_Change_DiceGrade, MP_PARTY_CHANGE_DICEGRADE_NACK ) ;
			}
			else
			{
				pParty->SetDiceGradeOpt((BYTE)pmsg->dwData1 ) ;

				MSG_BYTE msg ;
				msg.Category = MP_PARTY;
				msg.Protocol = MP_PARTY_CHANGE_DICEGRADE_ACK;
				msg.bData	 = (BYTE)pmsg->dwData1 ;

				pParty->SendMsgToAll(
					&msg,
					sizeof(msg));
			}
		}
		break;

	case MP_PARTY_CHANGEMASTER_SYN: 
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			PARTYMGR->ChangeMasterParty(pmsg->dwData2,pmsg->dwObjectID,pmsg->dwData1); 
		}
		break;
	case MP_PARTY_BREAKUP_SYN: 
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			PARTYMGR->BreakupParty(pmsg->dwData,pmsg->dwObjectID); //include DB
		}
		break; 
	case MP_PARTY_BAN_SYN: 
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			PARTYMGR->BanParty(pmsg->dwData2,pmsg->dwObjectID,pmsg->dwData1); 
		}
		break;
	case MP_PARTY_INVITE_ACCEPT_SYN: 
		{
			MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
			PARTYMGR->AddMember(pmsg->dwData, pmsg->dwObjectID);
		}
		break;
	case MP_PARTY_INVITE_DENY_SYN: 
		{
			const MSG_DWORD2* const pmsg = (MSG_DWORD2*)pMsg;
			const DWORD inviterPlayerIndex = pmsg->dwData2;
			
			MSG_DWORD message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_PARTY;
			message.Protocol = MP_PARTY_INVITE_DENY_ACK;
			message.dwData = inviterPlayerIndex;

			g_Network.Broadcast2AgentServer(
				(char*)&message,
				sizeof(message));
		}
		break;
	case MP_PARTY_NOTIFYMEMBER_LOGIN_TO_MAPSERVER:
		{
			SEND_PARTY_MEMBER_INFO* pmsg = (SEND_PARTY_MEMBER_INFO*)pMsg;
			PARTYMGR->NotifyUserLogIn(pmsg);
		}
		break;
	case MP_PARTY_NOTIFYMEMBER_LOGOUT_TO_MAPSERVER:
		{
			SEND_PARTY_MEMBER_INFO* pmsg = (SEND_PARTY_MEMBER_INFO*)pMsg;

			PARTYMGR->NotifyUserLogOut(pmsg);
		}
		break;
	case MP_PARTY_NOTIFYADD_TO_MAPSERVER:
		{
			SEND_PARTY_MEMBER_INFO* pmsg = (SEND_PARTY_MEMBER_INFO*)pMsg;
			PARTYMGR->NotifyAddParty(pmsg); 
		}
		break;
	case MP_PARTY_NOTIFYDELETE_TO_MAPSERVER: 
		{
			SEND_PARTY_MEMBER_INFO* pmsg = (SEND_PARTY_MEMBER_INFO*)pMsg;
			PARTYMGR->NotifyDelParty(pmsg);
		}
		break;
	case MP_PARTY_NOTIFYBAN_TO_MAPSERVER:
		{
			SEND_PARTY_MEMBER_INFO* pmsg = (SEND_PARTY_MEMBER_INFO*)pMsg;
			PARTYMGR->NotifyBanParty(pmsg); 
		}
		break;
	case MP_PARTY_NOTIFYCHANGEMASTER_TO_MAPSERVER:
		{
			SEND_PARTY_MEMBER_INFO* pmsg = (SEND_PARTY_MEMBER_INFO*)pMsg;
			PARTYMGR->NotifyChangeMasterParty(pmsg); 
		}
		break;
	case MP_PARTY_NOTIFYBREAKUP_TO_MAPSERVER:
		{
			SEND_PARTY_MEMBER_INFO* pmsg = (SEND_PARTY_MEMBER_INFO*)pMsg;
			PARTYMGR->NotifyBreakupParty(pmsg); 
		}
		break;
	case MP_PARTY_SYNDELETE_TO_MAPSERVER: 
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			PARTYMGR->NotifyDelSyn(pmsg);			
		}
		break;
	case MP_PARTY_NOTIFYCREATE_TO_MAPSERVER:
		{
			MSG_PARTY_CREATE* pmsg = (MSG_PARTY_CREATE*)pMsg;
			PARTYMGR->NotifyCreateParty( pmsg );
		}
		break;
	case MP_PARTY_NOTIFYMEMBER_LOGINMSG:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			PARTYMGR->NotifyMemberLoginMsg(pmsg->dwData, pmsg->dwObjectID);
		}
		break;
	case MP_PARTY_NOTIFYMEMBER_LEVEL:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			PARTYMGR->DoMemberLevelUp(pmsg->dwData1, pmsg->dwData2, (LEVELTYPE)pmsg->dwData3);
		}
		break;

	// 071002 LYW --- MapNetworkMsgParser : Add process to change option of party.
	case MP_PARTY_NOTIFY_CHANGE_OPTION :
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg ;
			PARTYMGR->SendOptionMsg( pmsg ) ;
		}
		break ;

	case MP_PARTY_INSTANTPARTY_REGIST_SYN:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			PARTYMGR->AddReservationInstantPartyMember(pmsg->dwData1, pmsg->dwData2);
		}
		break;
	} 
}

void MP_STORAGEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	STORAGEMGR->NetworkMsgParse(pmsg->Protocol,pMsg);
}

void MP_SkillMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	SKILLMGR->NetworkMsgParse(pmsg->Protocol,pMsg);
}

void MP_EXCHANGEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	EXCHANGEMGR->NetworkMsgParse( pmsg->Protocol, pMsg );
}

void MP_STREETSTALLMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	STREETSTALLMGR->NetworkMsgParse( pmsg->Protocol, pMsg );
}

void MP_BattleMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	BATTLESYSTEM->NetworkMsgParse( pmsg->Protocol, pmsg );
}

void MP_REVIVEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_CHAR_REVIVE_PRESENTSPOT_SYN:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pPlayer )
			{
				pPlayer->RevivePresentSpot();
			}
		}
		break;

	case MP_CHAR_REVIVE_VILLAGESPOT_SYN:
	case MP_CHAR_REVIVE_LOGINSPOT_SYN:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pPlayer )
			{
				pPlayer->ReviveLogIn();
			}
		}
		break;
	}
}

void MP_NPCMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	switch(pTempMsg->Protocol)
	{
	case MP_NPC_SPEECH_SYN:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			CNpc* pNpc = (CNpc*)g_pUserTable->FindUser(pmsg->dwData1);

			MSG_DWORD2 msg = *pmsg;
			msg.Protocol = MP_NPC_SPEECH_ACK;
			//msg.dwObjectID = pmsg->dwObjectID;

			// error Check
			if(!pPlayer) return;

			BYTE BattleKind = pPlayer->GetBattle()->GetBattleKind();
			if( BattleKind != eBATTLE_KIND_NONE )
				goto SPEECH_NACK;

			// checkhacknpc
			if(pNpc == 0)
			{
				if( CheckHackNpc( pPlayer, (WORD)pmsg->dwData2 ) == FALSE )
				{
					goto SPEECH_NACK;
				}
			}
			else
			{
				VECTOR3 ObjectPos	= *CCharMove::GetPosition(pPlayer);
				VECTOR3 TObjectPos	= *CCharMove::GetPosition(pNpc);
				DWORD	Distance	= (DWORD)CalcDistanceXZ( &ObjectPos, &TObjectPos );
				if(Distance > 3000.0f)	//1000->3000
				{
					goto SPEECH_NACK;
				}
				if(pNpc->GetNpcJob() == BOMUL_ROLE)
				{
					DWORD PlayerID = pNpc->GetFirstTalkCharacterID();
					if(PlayerID)
					{
						CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(PlayerID);
						if(pPlayer == 0) // maybe?
						{
							ASSERT(0); 
							pNpc->Die(NULL);
						}

						goto SPEECH_NACK;
					}
				}
				if(pNpc->GetState() == eObjectState_Die)
					goto SPEECH_NACK;
				if(pNpc->GetNpcJob() == BOMUL_ROLE)
					pNpc->SetFirstTalkCharacterID(pmsg->dwObjectID);
			}

			// State Check
			if( !OBJECTSTATEMGR_OBJ->StartObjectState(pPlayer, eObjectState_Deal, 0) ) goto SPEECH_NACK;

			pPlayer->SendMsg(&msg, sizeof(msg));
					
			return;

SPEECH_NACK:
			MSG_DWORD NackMsg;
			NackMsg.Category = MP_NPC;
			NackMsg.Protocol = MP_NPC_SPEECH_NACK;
			NackMsg.dwObjectID = pmsg->dwObjectID;
			NackMsg.dwData = pmsg->dwData2;
			pPlayer->SendMsg(&NackMsg, sizeof(NackMsg));
		}
		break;
	case MP_NPC_CLOSEBOMUL_SYN:
		{
			MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			CNpc* pNpc = (CNpc*)g_pUserTable->FindUser(pmsg->dwData); //pmsg->dwData : NpcObjectIdx

			// 검사
			if((pNpc == 0) || (pNpc->GetFirstTalkCharacterID() != pPlayer->GetID()))
				return;
			else
			{				
				pNpc->SetFirstTalkCharacterID(0);
			}			
		}
		break;
	case MP_NPC_OPENBOMUL_SYN:
		{
			MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			CNpc* pNpc = (CNpc*)g_pUserTable->FindUser(pmsg->dwData); //pmsg->dwData : NpcObjectIdx
			if((pNpc == 0) || (pNpc->GetFirstTalkCharacterID() != pPlayer->GetID()))
			{
				ASSERT(0);
				return;
			}
			pNpc->DoDie(pPlayer);
		}
		break;
	case MP_NPC_DOJOB_SYN:
		{
			MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == 0)
				return;
			CNpc* pNpc = (CNpc*)g_pUserTable->FindUser(pmsg->dwData); //pmsg->dwData : NpcObjectIdx
			if(pNpc == 0)
				return;
			pNpc->DoJob(pPlayer, dwConnectionIndex);			
		}
		break;

		// 080514 KTH -- npc action check hack
	case MP_NPC_CHECK_HACK_SYN:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			// pmsg->dwData1 = NPC Idx
			// pmsg->dwData2 = NPC UniqueIdx
			// pmsg->dwData3 = Action Value
			MSG_DWORD3 msg = *pmsg;
			msg.Protocol = MP_NPC_CHECK_HACK_ACK;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pPlayer == 0 )
				return;
			CNpc* pNpc = (CNpc*)g_pUserTable->FindUser(pmsg->dwData1);

			// 081009 LYW --- MapNetworkMsgParser : 공성소스 머지
			// 135 -- 제뷘, 136 -- 루쉔
			// 081008 KTH -- 공성 집행위원은 아무나 클릭 할 수 없다.
			if( SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
				SIEGEWARFAREMGR->Is_CastleMap() == FALSE
				)
			{
				// 081106 LYW --- MapNetworkMsgParser : 공성 최초 진행시나, 
				// 공성을 소유한 길드가 없을경우예외 처리를 한다.
				switch( pmsg->dwData2 )
				{
				// 제뷘 집행 위원
				case 135:
					{
						DWORD dwCastleGuildIdx = SIEGEWARFAREMGR->GetCastleGuildIdx(eNeraCastle_Zebin) ;

						if( dwCastleGuildIdx != pPlayer->GetGuildIdx() || dwCastleGuildIdx == 0 )
							goto FIND_HACK;
					}
					break;
				// 루센 집행 위원
				case 136:
					{
						DWORD dwCastleGuildIdx = SIEGEWARFAREMGR->GetCastleGuildIdx(eNeraCastle_Lusen) ;
					if( dwCastleGuildIdx != pPlayer->GetGuildIdx() || dwCastleGuildIdx == 0 )
						goto FIND_HACK;
					}
					break;
				}

				if( SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
				{
					goto FIND_HACK;
				}
			}
			
			if(pNpc == 0)
			{
				if( CheckHackNpc( pPlayer, (WORD)pmsg->dwData2 ) == FALSE )
				{
					goto FIND_HACK;
				}
			}
			else
			{
				VECTOR3 ObjectPos	= *CCharMove::GetPosition(pPlayer);
				VECTOR3 TObjectPos	= *CCharMove::GetPosition(pNpc);
				DWORD	Distance	= (DWORD)CalcDistanceXZ( &ObjectPos, &TObjectPos );
				if(Distance > 3000.0f)	//1000->3000
				{
					goto FIND_HACK;
				}
			}

			pPlayer->SendMsg(&msg, sizeof(msg));
			return;
FIND_HACK:
			MSG_DWORD HackMsg;
			HackMsg.Category = MP_NPC;
			HackMsg.Protocol = MP_NPC_CHECK_HACK_NACK;
			HackMsg.dwObjectID = pmsg->dwObjectID;
			HackMsg.dwData = pmsg->dwData2;
			pPlayer->SendMsg(&HackMsg, sizeof(HackMsg));
		}
		break;

	// 090227 ShinJS --- 이동NPC의 이동처리 추가
	case MP_NPC_REMOVE_MOVENPC_SYN:
		{
			// 이동NPC 제거 메세지
			MSG_WORD* pmsg = (MSG_WORD*)pMsg;		// (이동NPC Index)

			// 이동NPC 제거
			NPCMOVEMGR->RemoveMoveNpc( pmsg->wData );

			// User에게 이동NPC 제거 메세지 전송
			g_pUserTable->SetPositionUserHead();
			CObject* pObject = NULL ;
			while( ( pObject = g_pUserTable->GetUserData() ) != NULL )
			{	
				if( pObject->GetObjectKind() != eObjectKind_Player ) continue;

				CPlayer* pReceiver = (CPlayer*)pObject;
				pReceiver->SendMsg( pmsg, sizeof(MSG_DWORD) );
			}
		}
		break;

	case MP_NPC_MOVE_MOVENPC_SYN:
		{
			// 이동NPC 이동정보를 받은경우 이동NPC 관리자에 이동NPC를 추가하고
			// 이동응답 메세지를 전송하여 제거 할수 있도록 한다
			MSG_WORD3* pmsg = (MSG_WORD3*)pMsg;		// (이동전맵번호, NPC Index, NPC MoveIndex)
			WORD wMoveNpcIdx = pmsg->wData2;									// 이동NPC Index
			WORD wCurMoveIdx = pmsg->wData3;									// 이동NPC 이동정보 Index			

			// NPC 이동 관리자에 이동NPC 추가
			if( NPCMOVEMGR->AddMoveNpc( wMoveNpcIdx, wCurMoveIdx ) )
			{
				// 추가된 경우 응답 메세지를 이동전 맵에 전송하여 이동NPC를 제거한다
				MSG_WORD2 msg;
				ZeroMemory( &msg, sizeof(MSG_WORD2) );
				
				msg.Category = MP_NPC;
				msg.Protocol = MP_NPC_MOVE_MOVENPC_ACK;
				msg.wData1 = pmsg->wData1;										// 이동전 맵 번호
				msg.wData2 = wMoveNpcIdx;										// 이동NPC Index

				g_Network.Send2AgentServer( (char*)&msg, sizeof(MSG_WORD2) );
			}
		}
		break;
	// 091014 ONS 스위치역할 NPC 활성화시 트리거 메세지를 발생시킨다.
	case MP_NPC_SWITCHNPC_SYN:
		{
			const MSG_DWORD2* const pmsg = (MSG_DWORD2*)pMsg;
			CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if(0 == pPlayer || eObjectKind_Player != pPlayer->GetObjectKind())
			{
				break;
			}
			else if( DungeonMGR->CheckHackNpc( pPlayer, (WORD)pmsg->dwData2 ) == FALSE )
			{
				break;
			}

			// 091116 LUJ, 채널에 해당하는 메시지를 할당받도록 한다
			Trigger::CMessage* const message = TRIGGERMGR->AllocateMessage(pPlayer->GetGridID());
			message->AddValue(Trigger::eProperty_ObjectKind, pPlayer->GetObjectKind());
			message->AddValue(Trigger::eProperty_ObjectIndex, pPlayer->GetID());
			message->AddValue(Trigger::eProperty_ActiveNpcId, pmsg->dwData2);
		}
		break;
	}
}

void MP_QUESTMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	QUESTMGR->NetworkMsgParse( pTempMsg->Protocol, pMsg );

	switch(pTempMsg->Protocol)
	{
	case MP_QUEST_CHANGESTATE:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer == NULL)
			{
				return;
			}		

			pPlayer->SetQuestState(pmsg->dwData1, pmsg->dwData2);
			
		}
		break;
	}

}

void MP_PKMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength )
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	PKMGR->NetworkMsgParse( pmsg->Protocol, pMsg );	
}


void MP_HACKCHECKMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;

	switch( pTempMsg->Protocol )
	{
	case MP_HACKCHECK_BAN_USER:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pPlayer == NULL ) return;
			
			if( pmsg->dwData )
				InsertSpeedHackCheck( pPlayer->GetID(), pPlayer->GetObjectName(), pmsg->dwData, 0 );
		}
		break;
	}
}

void MP_GUILDMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	GUILDMGR->NetworkMsgParse(pmsg->Protocol,pMsg);
}

void MP_GUILDFIELDWARMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	GUILDWARMGR->NetworkMsgParse(pmsg->Protocol,pMsg);
}

void MP_GTOURNAMENTMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	GTMGR->NetworkMsgParse( dwConnectionIndex, pmsg->Protocol, pMsg );
}

void MP_GUILUNIONMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength )
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	GUILDUNIONMGR->NetworkMsgParse( pmsg->Protocol, pMsg );
}

 // 080723 KTH - SEIGEWARFARE MESSAGE PARSER
  void MP_SIEGEWARFAREMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
  {
  	MSGBASE* pmsg = (MSGBASE*)pMsg;
  	SIEGEWARFAREMGR->NetworkMsgParse( dwConnectionIndex, pmsg->Protocol, pMsg);
  }

// S 농장시스템 추가 added by hseos 2007.04.11
void MP_FARM_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength )
{
	g_csFarmManager.SRV_ParseRequestFromClient(dwConnectionIndex, pMsg, dwLength);
}
// E 농장시스템 추가 added by hseos 2007.04.11

// desc_hseos_주민등록01
// S 주민등록 추가 added by hseos 2007.06.04
void MP_RESIDENTREGIST_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength )
{
	g_csResidentRegistManager.SRV_ParseRequestFromClient(dwConnectionIndex, pMsg, dwLength);
}
// E 주민등록 추가 added by hseos 2007.06.04

void MP_FAMILYMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	g_csFamilyManager.SRV_ParseRequestFromClient(dwConnectionIndex, pMsg, dwLength);
}

// desc_hseos_데이트 존_01
// S 데이트 존 추가 added by hseos 2007.11.19
void MP_DATE_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	g_csDateManager.SRV_ParseRequestFromClient(dwConnectionIndex, pMsg, dwLength);
}

void MP_AUTONOTE_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	AUTONOTEMGR->NetworkMsgParse( pmsg->Protocol, pMsg, dwLength );
}

void MP_FISHING_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	FISHINGMGR->NetworkMsgParse( pmsg->Protocol, pMsg, dwLength );
}

void MP_COOK_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	COOKMGR->NetworkMsgParse( pmsg->Protocol, pMsg, dwLength );
}

void MP_HOUSE_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	HOUSINGMGR->NetworkMsgParser( dwConnectionIndex, pmsg->Protocol, pMsg, dwLength );
}

void MP_TRIGGER_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	TRIGGERMGR->NetworkMsgParser( dwConnectionIndex, pmsg->Protocol, pMsg, dwLength );
}

void MP_DUNGEON_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	DungeonMGR->NetworkMsgParser( dwConnectionIndex, pmsg->Protocol, pMsg, dwLength );
}

void MP_PET_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	PETMGR->NetworkMsgParse( pmsg->Protocol, pMsg, dwLength );
}

// 070228 LYW --- MapNetworkMsgParser : Add message parser for facial part.
void MP_FACIALMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSG_BYTE* pTempMsg = ( MSG_BYTE* )pMsg ;

	switch( pTempMsg->Protocol )
	{
	case MP_FACIAL_NORMAL :
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pTempMsg->dwObjectID);
			if(!pPlayer) return;
			
			PACKEDDATA_OBJ->QuickSend(pPlayer,pTempMsg,dwLength);
		}
		break ;
	}
}

// 070228 LYW --- MapNetworkMsgParser : Add message parser for emotion part.
void MP_EMOTIONMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSG_INT* pTempMsg = ( MSG_INT* )pMsg ;

	switch( pTempMsg->Protocol )
	{
	case MP_EMOTION_NORMAL :
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pTempMsg->dwObjectID);
			if(!pPlayer) return;

			// 090423 ShinJS --- 탈것에 탑승 상태인 경우 모션 불가
			if( pPlayer->GetMountedVehicle() != 0 )
			{
				return;
			}

			PACKEDDATA_OBJ->QuickSend(pPlayer,pTempMsg,dwLength);
		}
		break ;

	// 070502 LYW --- MapNetworkMsgParser : Add message to setting rest mode.
	case MP_EMOTION_SITSTAND_SYN :
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pTempMsg->dwObjectID);
			if( pPlayer )
			{
				if( pPlayer->IsShowdown() || pPlayer->IsPKMode() )
				{
					return;
				}

				// 090423 ShinJS --- 탈것에 탑승 상태인 경우 앉기/서기 불가
				if( pPlayer->GetMountedVehicle() != 0 )
				{
					return;
				}

				MSG_BYTE* message = ( MSG_BYTE* )pTempMsg;

				pTempMsg->Protocol = MP_EMOTION_SITSTAND_ACK ;
				PACKEDDATA_OBJ->QuickSend(pPlayer,pTempMsg,dwLength);

				// 080513 LYW --- MapNetworkMsgParser : 모션 처리 수정.
				//pPlayer->SetState( message->bData ? eObjectState_Rest : eObjectState_None );
				if(message->bData)
				{
					OBJECTSTATEMGR_OBJ->StartObjectState( pPlayer, eObjectState_Rest, 0 ) ;
				}
				else
				{
					OBJECTSTATEMGR_OBJ->StartObjectState( pPlayer, eObjectState_None, 0 ) ;
				}
			}
		}
		break ;
	// 070502 LYW --- MapNetworkMsgParser : Add message to setting ing motion.
	case MP_EMOTION_ING_MOTION_SYN :
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pTempMsg->dwObjectID);
			if(!pPlayer) return;
			
			pTempMsg->Protocol = MP_EMOTION_ING_MOTION_ACK ;
			PACKEDDATA_OBJ->QuickSend(pPlayer,pTempMsg,dwLength);
		}
		break ;
	// 070528 LYW --- MapNetworkMsgParser : Add message to play emoticon part.
	case MP_EMOTICON_SYN :
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pTempMsg->dwObjectID);
			if(!pPlayer) return;
			
			pTempMsg->Protocol = MP_PLAY_EMOTICON_ACK ;
			PACKEDDATA_OBJ->QuickSend(pPlayer,pTempMsg,dwLength);
		}
		break ;
	}
}

// 081126 LYW --- MapNetworkMsgParser : 리미트 던전 추가.
void MP_LIMITDUNGEON_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	// 리미트 던전 매니져로 넘긴다.
	LIMITDUNGEONMGR->NetworkMsgParser( ( MSGBASE* )pMsg ) ;
}

BOOL CheckHackNpc( CPlayer* pPlayer, WORD wNpcIdx )
{
	STATIC_NPCINFO* pInfo = GAMERESRCMNGR->GetStaticNpcInfo( wNpcIdx );
	if( pInfo == NULL )
	{
		return FALSE;
	}
	
	VECTOR3 vPos;
	pPlayer->GetPosition( &vPos );
	float dist = CalcDistanceXZ( &vPos, &pInfo->vPos );
	if( dist > 3000.0f )
	{
		ASSERT(0); //TL임시
		return FALSE;
	}

	return TRUE;
}

// 071023 LYW --- MapNetworkMsgParser : Add message parser for tutorial part.
void MP_TUTORIALMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSG_INT* pmsg = ( MSG_INT* )pMsg ;

	switch( pmsg->Protocol )
	{
	case MP_TUTORIAL_UPDATE_SYN :
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pPlayer == NULL )	return;

			TutorialUpdateToDB( pmsg->dwObjectID, pmsg->nData, TRUE ) ;
		}
		break ;
	}
}

// 071212 KTH -- 확장 인벤토리를 초기화 하여 주는 치트키
void Cheat_Reset_Inventory(char* pMsg) 
{
	MSGBASE* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

	ResetCharacterInventory(pPlayer->GetID());

	pPlayer->SetInventoryExpansion(0);

	pmsg->Protocol = MP_CHEAT_RESET_INVENTORY_ACK;

	pPlayer->SendMsg(pmsg, sizeof(MSGBASE) );
}

// 080115 KTH -- PVP Damage Ratio
void Cheat_PvP_Damage_Rate(char* pMsg)
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

	ATTACKMGR->SetPvpDamageRatio(pmsg->bData);

	pmsg->Protocol = MP_CHEAT_PVP_DAMAGE_RATE_ACK;

	pPlayer->SendMsg(pmsg, sizeof(MSG_BYTE) );
}

void Cheat_Fishing_Exp(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*) pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	pPlayer->SetFishingExp(pmsg->dwData);

	MSG_DWORDEX2 msgFishingExp;
	msgFishingExp.Category = MP_FISHING;
	msgFishingExp.Protocol = MP_FISHING_EXP_ACK;
	msgFishingExp.dwObjectID = pPlayer->GetID();
	msgFishingExp.dweData1 = (DWORDEX)pPlayer->GetFishingLevel();
	msgFishingExp.dweData2  = pPlayer->GetFishingExp();
	pPlayer->SendMsg( &msgFishingExp, sizeof( msgFishingExp ) );
}

void Cheat_Fishing_FishPoint(char* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*) pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	pPlayer->SetFishPoint(pmsg->dwData);

	MSG_DWORD msgFishPoint;
	msgFishPoint.Category = MP_FISHING;
	msgFishPoint.Protocol = MP_FISHING_POINT_ACK;
	msgFishPoint.dwData	= pPlayer->GetFishPoint();
	pPlayer->SendMsg(&msgFishPoint, sizeof(msgFishPoint));

	// 080808 LUJ, 낚시 경험치/레벨 추가
	Log_Fishing(
		pPlayer->GetID(),
		eFishingLog_SetPointFromCheat,
		pPlayer->GetFishPoint(),
		pPlayer->GetFishPoint(),
		0,
		0,
		pPlayer->GetFishingExp(),
		pPlayer->GetFishingLevel() );
}

void Cheat_Dungeon_Observer_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_WORD3* pmsg = (MSG_WORD3*)pMsg;

	if(pmsg->wData1 != g_pServerSystem->GetMapNum())
	{
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
		if(!pPlayer)	return;

		MAPTYPE wMapNum = (MAPTYPE)pmsg->wData1;
		
		MSG_WORD3 msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_DUNGEON_OBSERVER_SYN;
		msg.dwObjectID = pmsg->dwObjectID;
		msg.wData1 = pmsg->wData1;		// Dest MapNum
		msg.wData2 = pmsg->wData2;		// Channel
		msg.wData3 = g_pServerSystem->GetMapNum();

		PACKEDDATA_OBJ->SendToMapServer(wMapNum, &msg, sizeof(msg));
		return;
	}

	WORD wError = eDungeonERR_None;
	MAPTYPE wReturnMapNum = (MAPTYPE)pmsg->wData3;

	if(pmsg->wData1==0 || pmsg->wData2==0 || pmsg->wData1!=g_pServerSystem->GetMapNum())
	{
		wError = eDungeonERR_WrongParam;
		goto DUNGEON_OB_FAIL;
	}

	CBattle* pBattle = BATTLESYSTEM->GetBattle(pmsg->wData2);
	if(!pBattle->GetBattleID())
	{
		wError = eDungeonERR_NoChannel;
		goto DUNGEON_OB_FAIL;
	}

	{
		MSG_WORD2 msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_DUNGEON_OBSERVER_ACK;
		msg.dwObjectID = pmsg->dwObjectID;
		msg.wData1 = pmsg->wData1;
		msg.wData2 = pmsg->wData2;

		if(wReturnMapNum)
			PACKEDDATA_OBJ->SendToMapServer(wReturnMapNum, &msg, sizeof(msg));
		else
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
				pPlayer->SendMsg(&msg, sizeof(msg));

			DungeonMGR->MoveToLoginMap(pPlayer->GetID());
		}

		return;
	}


DUNGEON_OB_FAIL:
	{
		MSG_WORD msgFail;
		msgFail.Category = MP_CHEAT;
		msgFail.Protocol = MP_CHEAT_DUNGEON_OBSERVER_NACK;
		msgFail.dwObjectID = pmsg->dwObjectID;
		msgFail.wData = 0;

		if(wReturnMapNum)
			PACKEDDATA_OBJ->SendToMapServer(wReturnMapNum, &msgFail, sizeof(msgFail));
		else
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
				pPlayer->SendMsg(&msgFail, sizeof(msgFail));
		}
	}
	return;
}

void Cheat_Dungeon_ObserverJoin_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	if(FALSE == DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
	{
		return;
	}

	MSG_WORD4* pmsg = (MSG_WORD4*)pMsg;

	if(g_pUserTable->FindUser(pmsg->dwObjectID))
	{
		return;
	}

	const eUSERLEVEL userLevel = eUSERLEVEL(pmsg->wData3);

	CPlayer* pPlayer = g_pServerSystem->AddPlayer(
		pmsg->dwObjectID,
		dwConnectionIndex,
		pmsg->wData2,
		pmsg->wData1,
		userLevel);

	if(!pPlayer) return;

	FishingData_Load(pmsg->dwObjectID);
	CookingData_Load(pmsg->dwObjectID);
	Cooking_Recipe_Load(pmsg->dwObjectID);
	HouseData_Load(pmsg->dwObjectID);
	
	CharacterNumSendAndCharacterInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
	CharacterSkillTreeInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
	CharacterItemInfo( pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN );

	VECTOR3 vDungeonPos = {0};
	const stDungeon& Dungeon = DungeonMGR->GetDungeon(pmsg->wData1);
	
	if(CParty* pParty = PARTYMGR->GetParty(Dungeon.dwPartyIndex))
	{
		const PARTY_MEMBER& member = pParty->GetMember(
			pParty->GetRandomMember());

		if(CObject* const object = g_pUserTable->FindUser(member.dwMemberID))
		{
			object->GetPosition(
				&vDungeonPos);
		}
		else
		{
			MAPCHANGE_INFO* pChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( g_pServerSystem->GetMapNum() );
			if( pChangeInfo )
			{
				vDungeonPos = pChangeInfo->MovePoint;
			}
		}
	}
	/// 단축창 정보
	QuickInfo( pmsg->dwObjectID );
	// quest 정보 qurey 추가
	QUESTMGR->CreateQuestForPlayer( pPlayer );
	//퀘스트정보를 받아오도록 한다.
	QuestTotalInfo( pmsg->dwObjectID );	
	//농장 및 기타정보를 받아오도록 한다.
	Farm_LoadTimeDelay( pmsg->dwObjectID );	

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.27	2007.11.28
	// ..함수 순서를 지키는 것이 좋음
	// ..챌린지 존 시작
	g_csDateManager.SRV_StartChallengeZone(pPlayer, pmsg->wData2, 0);
	// ..챌린지 존 입장 회수 로드
	ChallengeZone_EnterFreq_Load(pPlayer->GetID());

	pPlayer->SetDungeonObserver(TRUE);
	pPlayer->SetDungeonObserverPos(&vDungeonPos);
}

void Cheat_Dungeon_GetInfoAll_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;

	if(pmsg->wData1 != g_pServerSystem->GetMapNum())
	{
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
		if(!pPlayer)	return;

		MAPTYPE wMapNum = (MAPTYPE)pmsg->wData1;
		
		MSG_WORD2 msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_DUNGEON_GETINFOALL_SYN;
		msg.dwObjectID = pmsg->dwObjectID;
		msg.wData1 = wMapNum;
		msg.wData2 = g_pServerSystem->GetMapNum();

		PACKEDDATA_OBJ->SendToMapServer(wMapNum, &msg, sizeof(msg));
		return;
	}

	DungeonMGR->SendInfoSummary(dwConnectionIndex, pMsg);
}

void Cheat_Dungeon_GetInfoOne_Syn(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_WORD3* pmsg = (MSG_WORD3*)pMsg;

	if(pmsg->wData1 != g_pServerSystem->GetMapNum())
	{
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
		if(!pPlayer)	return;

		MAPTYPE wMapNum = (MAPTYPE)pmsg->wData1;
		
		MSG_WORD3 msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_DUNGEON_GETINFOONE_SYN;
		msg.dwObjectID = pmsg->dwObjectID;
		msg.wData1 = pmsg->wData1;
		msg.wData2 = pmsg->wData2;
		msg.wData3 = g_pServerSystem->GetMapNum();

		PACKEDDATA_OBJ->SendToMapServer(wMapNum, &msg, sizeof(msg));
		return;
	}

	DungeonMGR->SendInfoDetail(dwConnectionIndex, pMsg);
}

void Cheat_Clear_Inventory(char* pMsg)
{
	MSGBASE* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

	if( pPlayer->ClearInventory() )
	{
		pmsg->Protocol = MP_CHEAT_CELAR_INVENTORY_ACK;
		pPlayer->SendMsg(pmsg, sizeof(MSG_BYTE));
	}
}



//-------------------------------------------------------------------------------------------------
//	NAME		: MP_SIEGERECALL_MsgParser
//	DESC		: Add message parser for siege recall part.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void MP_SIEGERECALL_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	SIEGERECALLMGR->NetworkMsgParser(pMsg) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: MP_RECALLNPC_MsgParser
//	DESC		: Add message parser for for recall npc..
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 20, 2008
//-------------------------------------------------------------------------------------------------
void MP_RECALLNPC_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	NPCRECALLMGR->NetworkMsgParser(dwConnectionIndex, pMsg, dwLength);
}

void MP_SIEGEWARFARE_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;

	SIEGEWARFAREMGR->NetworkMsgParse( dwConnectionIndex, pmsg->Protocol, pMsg) ;
}

// 090316 LUJ, 탈것 메시지 처리 함수
void MP_VEHICLE_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	const MSGBASE* const message = ( MSGBASE* )pMsg;
	VEHICLEMGR->NetworkMsgParse( dwConnectionIndex, message, dwLength );
}

void MP_PCROOM_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	const MSGBASE* const message = ( MSGBASE* )pMsg;
	PCROOMMGR->NetworkMsgParse( message, dwLength );
}

void MP_CONSIGNMENT_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	CONSIGNMENTMGR->NetworkMsgParser( dwConnectionIndex, pmsg->Protocol, pMsg, dwLength) ;
}

void MP_NOTE_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength)
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;

	switch(pmsg->Protocol)
	{
	case MP_NOTE_GETPACKAGE_SYN:
		{
			MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(! pPlayer)
				return;

			DWORD dwErrorKind = 0;
			DWORD dwNoteID = pmsg->dwData1;
			DWORD dwItemDBIdx = pmsg->dwData3;
			DWORD dwMoney = pmsg->dwData5;

			CItemSlot* pSlot = pPlayer->GetSlot(eItemTable_Inventory);
			if(! pSlot)
				return;

			WORD emptyPosition[SLOT_MAX_INVENTORY_NUM] = {0,};
			if(dwItemDBIdx > 0)
			{
				WORD emptyCount = pSlot->GetEmptyCell( emptyPosition, 1 );
				if(emptyCount == 0)
					dwErrorKind = eConsignmentERR_PoorSlot;	// 인벤 빈공간 없음
			}

			if( LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) )
				dwErrorKind = eConsignmentERR_WrongState;

			unsigned long long nTotalMoney = dwMoney + pPlayer->GetMoney();
			if(nTotalMoney > MAX_INVENTORY_MONEY)
				dwErrorKind = eConsignmentERR_GoldOver;

			if(dwErrorKind)
			{
				MSG_DWORD msg;
				msg.Category = MP_NOTE;
				msg.Protocol = MP_NOTE_GETPACKAGE_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = dwErrorKind;
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}

			Note_GetPackage(pPlayer->GetID(), dwNoteID, emptyPosition[0], dwItemDBIdx, dwMoney);
		}
		break;
	}
}

// 100611 ONS 채팅 금지 기능 추가
void Cheat_ForbidChat( char* pMsg )
{
	MSG_FORBID_CHAT* pmsg = (MSG_FORBID_CHAT*)pMsg;

	CPlayer* const pOperator = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if(!pOperator)
	{
		return;
	}

	CPlayer* const pTarget = (CPlayer*)g_pUserTable->FindUserForName( pmsg->Name );
	if(!pTarget)
	{
		// 대상이 해당맵에 존재하지 않을 경우, 다른맵에서 처리한다.
		MSG_FORBID_CHAT msg;
		ZeroMemory( &msg, sizeof(msg) );

		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_FORBID_CHAT_OTHERMAP_SYN;
		msg.dwObjectID	= pmsg->dwObjectID;
		msg.dwData		= pmsg->dwData;
		SafeStrCpy( msg.Name, pmsg->Name, MAX_NAME_LENGTH + 1 );
		SafeStrCpy( msg.Reason, pmsg->Reason, MAX_CHAT_LENGTH + 1 );

		PACKEDDATA_OBJ->SendToBroadCastMapServer( &msg, sizeof(msg) );
		return;
	}

	// 대상이 채팅금지상태인지 확인
	if( pTarget->IsForbidChat() )
	{
		stTime64t msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_FORBID_ALREADYEXIST;
		msg.dwObjectID	= pOperator->GetID();
		msg.time64t		= pTarget->GetForbidChatTime();
		pOperator->SendMsg(&msg, sizeof(msg));
		return;
	}

	// GM에게 성공메세지를 전송한다.
	MSGBASE msg;
	ZeroMemory( &msg, sizeof(msg) );
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_FORBID_CHAT_ACK;
	msg.dwObjectID	= pmsg->dwObjectID;
	pOperator->SendMsg(&msg, sizeof(msg));	

	// DB에 저장한다.
	ForbidChatUpdate( pmsg->dwObjectID, pTarget->GetID(), pmsg->Reason, pmsg->dwData, TRUE );
}

void Cheat_ForbidChat_Server( DWORD dwConnectionIndex, char* pMsg )
{
	MSG_FORBID_CHAT* pmsg = (MSG_FORBID_CHAT*)pMsg;

	CPlayer* const pTarget = (CPlayer*)g_pUserTable->FindUserForName( pmsg->Name );
	if(!pTarget)
	{
		return;
	}

	if( pTarget->IsForbidChat() )
	{
		stTime64t msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_FORBID_ALREADYEXIST;
		msg.dwObjectID	= pmsg->dwObjectID;
		msg.time64t		= pTarget->GetForbidChatTime();
		g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );
		return;
	}

	// GM에게 성공메세지를 전송한다.
	MSGBASE msg;
	ZeroMemory( &msg, sizeof(msg) );
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_FORBID_CHAT_ACK;
	msg.dwObjectID	= pmsg->dwObjectID;
	g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );

	// DB에 저장한다.
	ForbidChatUpdate( pmsg->dwObjectID, pTarget->GetID(), pmsg->Reason, pmsg->dwData, TRUE );
}

void Cheat_PermitChat( char* pMsg )
{
	MSG_NAME* pmsg = (MSG_NAME*)pMsg;
	CPlayer* const pOperator = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if(!pOperator)
	{
		return;
	}

	CPlayer* const pTarget = (CPlayer*)g_pUserTable->FindUserForName( pmsg->Name );
	if(!pTarget)
	{
		// 대상이 해당맵에 존재하지 않을 경우, 다른맵에서 처리한다.
		MSG_NAME msg;
		ZeroMemory( &msg, sizeof(msg) );

		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_PERMIT_CHAT_OTHERMAP_SYN;
		msg.dwObjectID	= pmsg->dwObjectID;
		SafeStrCpy( msg.Name, pmsg->Name, MAX_NAME_LENGTH + 1 );

		PACKEDDATA_OBJ->SendToBroadCastMapServer( &msg, sizeof(msg) );
		return;
	}

	// 대상이 채팅금지상태인지 확인
	if( !pTarget->IsForbidChat() )
	{
		stTime64t msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_FORBID_NONTARGET;
		msg.dwObjectID	= pOperator->GetID();
		msg.time64t		= pTarget->GetForbidChatTime();
		pOperator->SendMsg(&msg, sizeof(msg));
		return;
	}

	MSGBASE msg;
	ZeroMemory( &msg, sizeof(msg) );
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_PERMIT_CHAT_ACK;
	msg.dwObjectID	= pmsg->dwObjectID;
	pOperator->SendMsg(&msg, sizeof(msg));	

	// DB에 저장한다.
	ForbidChatUpdate( pmsg->dwObjectID, pTarget->GetID(), NULL, 0, FALSE );
}

void Cheat_PermitChat_Server( DWORD dwConnectionIndex, char* pMsg )
{
	MSG_NAME* pmsg = (MSG_NAME*)pMsg;

	CPlayer* const pTarget = (CPlayer*)g_pUserTable->FindUserForName( pmsg->Name );
	if(!pTarget)
	{
		return;
	}

	if( !pTarget->IsForbidChat() )
	{
		stTime64t msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_FORBID_NONTARGET;
		msg.dwObjectID	= pmsg->dwObjectID;
		msg.time64t		= pTarget->GetForbidChatTime();
		g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );
		return;
	}

	MSGBASE msg;
	ZeroMemory( &msg, sizeof(msg) );
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_PERMIT_CHAT_ACK;
	msg.dwObjectID	= pmsg->dwObjectID;
	g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );

	// DB에 저장한다.
	ForbidChatUpdate( pmsg->dwObjectID, pTarget->GetID(), NULL, 0, FALSE );
}
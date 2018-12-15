#include "stdafx.h"
#include "PartyManager.h"
#include "Party.h"
#include "MapDBMsgParser.h"
#include "UserTable.h"
#include "Network.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "Battle.h"
#include "Player.h"
#include "../hseos/Date/SHDateManager.h"
#include "MapNetworkMsgParser.h"

CPartyManager::CPartyManager() 
{
	m_partyRoomPool = new CPool< CParty >;
	m_partyRoomPool->Init( 300, 100, "PartyRoomPool" );
    m_partyRoomHashTable.Initialize(256);	
	m_tempPartyRoomHashTable.Initialize(32);	

	const DWORD indexSize = 50000;
	m_InstantPartyIDXGenerator.Init(
		indexSize,
		UINT_MAX - indexSize);

	if(CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
	{
		m_InstantPartyReservationPool = new CPool<stInstantPartyReservation>;
		m_InstantPartyReservationPool->Init(100, 100, "InstantPartyReservationPool");
		m_InstantPartyReservationList.Initialize(1000);
	}
}

CPartyManager::~CPartyManager() 
{
	SAFE_DELETE( m_partyRoomPool );

	SAFE_DELETE( m_InstantPartyReservationPool );
	m_InstantPartyIDXGenerator.Release();
}


CParty* CPartyManager::RegistParty(DWORD PartyIDX)
{
	CParty* party = m_partyRoomHashTable.GetData(PartyIDX);

	if(0 == party)
	{
		party = m_partyRoomPool->Alloc();
		m_partyRoomHashTable.Add(party,PartyIDX);
	}

	party->Init(
		PartyIDX,
		0,
		"",
		0,
		ePartyOpt_Damage,
		PARTY_DICEGRADE_DEFAULT);
	return party;
}

void CPartyManager::CreatePartyQuery(CPlayer * pMaster, BYTE bOption, BYTE bSubOption)
{
	DWORD PartyIdx = pMaster->GetPartyIdx();
	if(PartyIdx != 0)
	{
		int errorFlag = 0;
		CParty* pParty = GetParty( PartyIdx );

		if( pParty )
		{
			if( pParty->GetMasterID() == pMaster->GetID() )
			{
				errorFlag = 1;
				//breakup
				BreakupParty( PartyIdx, pMaster->GetID() );
			}
			else if( pParty->IsPartyMember(pMaster->GetID()) )
			{
				errorFlag = 2;
				//DelMember
				DelMember( pMaster->GetID(), PartyIdx );
			}
		}
		else
		{
			errorFlag = 3;
		}

		pMaster->SetPartyIdx(0);
	}		

	PartyCreate(pMaster->GetID(), bOption, bSubOption);
}

void CPartyManager::AddPartyInvite(DWORD MasterID, DWORD TargetID)
{
	CPlayer* pMaster = (CPlayer*)g_pUserTable->FindUser(MasterID);
	if(!pMaster)
		return;
	CParty* pParty = GetParty(pMaster->GetPartyIdx());
	if(!pParty)
	{
		ASSERTMSG(0,"[파티]-멤버추가-파티가 없습니다");
		return;
	}

	if(pParty->IsPartyMember(MasterID) == FALSE)
		return;
	if(pParty->IsPartyMember(TargetID) == TRUE)
		return;

	
	MSG_DWORD msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_ADD_NACK;
	msg.dwData = 0;

	CPlayer* pTargetPlayer = (CPlayer*)g_pUserTable->FindUser(TargetID);	
	if(!pTargetPlayer)
	{
		msg.dwData = eErr_Add_NoPlayer;
	}
	else if(GetParty(pTargetPlayer->GetPartyIdx()))
		msg.dwData		= eErr_Add_AlreadyinParty;
	else if(pTargetPlayer->GetGameOption()->bNoParty)
		msg.dwData		= eErr_Add_OptionNoParty;
	else if(pTargetPlayer->GetState() == eObjectState_Die)
		msg.dwData		= eErr_Add_DiedPlayer;	
	else if( pMaster->GetBattle()->GetBattleKind() == eBATTLE_KIND_GTOURNAMENT )
	{
		//// RaMa - 05.05.20   문파 토너먼트시 같은 문파만 파티를 맺을 수 있다.
		//if(	pTargetPlayer->GetPartyIdx() != pMaster->GetPartyIdx() )
		//	msg.dwData = eErr_Add_NotOurGuild;

		if(pTargetPlayer->GetBattleTeam() != pMaster->GetBattleTeam())
			msg.dwData = eErr_Add_NotOurGuild;
	}	
	if(msg.dwData > 0) //nack
		pMaster->SendMsg(&msg, sizeof(msg));
	else //ack
	{
		// LUJ, 초대 처리를 일원화하기 위해 이름 초대와 같은 방식으로 변경. 에이전트에서 처리한다
		MSG_DWORD inviteMsg;
		ZeroMemory(
			&inviteMsg,
			sizeof(inviteMsg));
		inviteMsg.Category = MP_PARTY;
		inviteMsg.Protocol = MP_PARTY_INVITE_BYNAME_SYN;
		inviteMsg.dwObjectID = MasterID;
		inviteMsg.dwData = TargetID;
		MP_PARTYMsgParser(
			pMaster->GetAgentNum(),
			(char*)&inviteMsg,
			sizeof(inviteMsg));
	}
}

void CPartyManager::AddMember(DWORD PartyID, DWORD TargetPlayerID)
{
	CPlayer* pNewMember = (CPlayer*)g_pUserTable->FindUser(TargetPlayerID);
	if(!pNewMember)
		return;

	if(GetParty(pNewMember->GetPartyIdx()))
	{
		return;
	}

	CParty* pParty = m_tempPartyRoomHashTable.GetData( PartyID );

	if( pParty )
	{
		if(pParty->m_Member[1].dwMemberID == TargetPlayerID)
		{
			PartyCreate(
				pParty->GetMasterID(),
				pParty->GetOption(),
				pParty->GetDiceGradeOpt());
		}
	}
	else
	{
		pParty = GetParty(PartyID);
		if(pParty == NULL)
		{
			//	nack 처리 필요 - 파티가 해산되었을꺼다. 
			MSG_BYTE msg;
			msg.Category = MP_PARTY;
			msg.Protocol = MP_PARTY_INVITE_ACCEPT_NACK;
			msg.bData = eErr_BrokenParty;
			pNewMember->SendMsg(&msg, sizeof(msg));
			return;
		}
		if(pParty->IsPartyMember(pNewMember->GetID()) == TRUE)
			return;

		PartyAddMember(PartyID,TargetPlayerID); //DB update
	}
}

void CPartyManager::AddMemberResult(CParty* party, CPlayer* player)
{
	player->SetPartyIdx(party->GetPartyIdx());
	party->AddPartyMember(
		player->GetID(),
		player->GetObjectName(),
		player,
		player->GetLevel());

	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		const PARTY_MEMBER& member = party->GetMember(i);

		if(player->GetID() != member.dwMemberID)
		{
			continue;
		}

		SEND_PARTY_MEMBER_INFO message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_PARTY;
		message.Protocol = MP_PARTY_ADD_ACK;
		message.MemberInfo = party->GetMember(i);
		message.mIsMaster = (party->GetMasterID() == player->GetID());
		message.PartyID = player->GetPartyIdx();
		party->SendMsgToAll(
			&message,
			sizeof(message));

		NotifyChangesOtherMapServer(
			player->GetID(),
			party,
			MP_PARTY_NOTIFYADD_TO_MAPSERVER,
			player->GetPartyIdx(),
			player->GetLevel());
		break;
	}
}

void CPartyManager::NotifyAddParty(SEND_PARTY_MEMBER_INFO* pmsg)
{
	CParty* pParty = GetParty(pmsg->PartyID);
	if(pParty ==NULL)
		return;
	
	if(pParty->GetMasterID() == 0) //파티 정보가 없으면
		return;

	const PARTY_MEMBER& member = pmsg->MemberInfo;
	pParty->AddPartyMember(
		member.dwMemberID,
		member.Name,
		0,
		member.Level);

	SEND_PARTY_MEMBER_INFO smsg;
	ZeroMemory(&smsg, sizeof(smsg));
	smsg.Category = MP_PARTY;
	smsg.Protocol = MP_PARTY_ADD_ACK;
	smsg.MemberInfo = member;
	smsg.mIsMaster = (pParty->GetMasterID() == member.dwMemberID);
	smsg.PartyID = pmsg->PartyID;
	pParty->SendMsgToAll(&smsg,sizeof(smsg));
}

void CPartyManager::DelMember(DWORD TargetPlayerID,DWORD PartyID)
{
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.30
	// ..챌린지 존에서는 탈퇴 불가
	if (g_csDateManager.IsChallengeZoneHere())
		return;
	// E 데이트 존 추가 added by hseos 2007.11.30

	CParty* pParty = GetParty(PartyID);
	if(pParty == NULL)
	{
		ASSERTMSG(0,"There is no Party.");
		return;
	}
	
	if(pParty->IsPartyMember(TargetPlayerID) == FALSE)
		return;
	if(pParty->GetMasterID() == 0) //파티 정보가 없으면
		return;

	MSG_DWORD smsg;
	smsg.Category = MP_PARTY;
	smsg.Protocol = MP_PARTY_DEL_ACK;
	smsg.dwData = TargetPlayerID;
	pParty->SendMsgToAll(&smsg,sizeof(smsg));

	NotifyChangesOtherMapServer(TargetPlayerID,pParty,MP_PARTY_NOTIFYDELETE_TO_MAPSERVER, PartyID);
	
	pParty->RemovePartyMember(TargetPlayerID);
	
	CPlayer * pPlayer = (CPlayer*)g_pUserTable->FindUser(TargetPlayerID);
	if(pPlayer)
		pPlayer->SetPartyIdx(0);

//	CharacterTablePartyIdxUpdate(TargetPlayerID); //로그 아웃 한 사람 ban할 때 캐릭터 테이블.partyidx 업데이트
}

void CPartyManager::NotifyDelParty(SEND_PARTY_MEMBER_INFO* pmsg)
{
	CParty* pParty = GetParty(pmsg->PartyID);
	if(pParty ==NULL)
		return;
	
	MSG_DWORD smsg;
	smsg.Category = MP_PARTY;
	smsg.Protocol = MP_PARTY_DEL_ACK;
	smsg.dwData = pmsg->dwObjectID;
	pParty->SendMsgToAll(&smsg,sizeof(smsg));
	pParty->RemovePartyMember(pmsg->dwObjectID);
	
	CPlayer * pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pPlayer)
		pPlayer->SetPartyIdx(0);
}

void CPartyManager::BanParty(DWORD PartyID,DWORD PlayerID,DWORD TargetPlayerID) 
{
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.30
	// ..챌린지 존에서는 추방 불가
	if (g_csDateManager.IsChallengeZoneHere())
		return;
	// E 데이트 존 추가 added by hseos 2007.11.30

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(PlayerID);
	if(pPlayer == NULL)
		return;

	DWORD PartyIdx = pPlayer->GetPartyIdx();
	CParty* pParty = GetParty(PartyIdx);
	if(pParty == NULL)
	{
		ASSERTMSG(0,"파티가 존재하지 않습니다.");//pjslocal
		return;
	}
	if(pParty->IsMasterChanging(pPlayer, MP_PARTY_BAN_NACK) == TRUE)
		return;
	
	if(pParty->IsPartyMember(pPlayer->GetID()) && pParty->IsPartyMember(TargetPlayerID))
	{
		MSG_DWORD smsg;
		smsg.Category = MP_PARTY;
		smsg.Protocol = MP_PARTY_BAN_ACK;
		smsg.dwData = TargetPlayerID;
		pParty->SendMsgToAll(&smsg,sizeof(smsg));
		
		CPlayer* pBanedPlayer = (CPlayer *)g_pUserTable->FindUser(TargetPlayerID);
		if(pBanedPlayer)
			pBanedPlayer->SetPartyIdx(0);

		NotifyChangesOtherMapServer(TargetPlayerID,pParty,MP_PARTY_NOTIFYBAN_TO_MAPSERVER, PartyID);

		pParty->RemovePartyMember(TargetPlayerID);	
		PartyDelMember(PartyID, TargetPlayerID);

//		PartyMemberUpdate(pParty, PartyID);	
	}
	else
	{
		ASSERTMSG(0,"파티원이 아닌 사람이 Ban을시도했습니다");
	}
}
void CPartyManager::NotifyBanParty(SEND_PARTY_MEMBER_INFO* pmsg)
{
	CParty* pParty = GetParty(pmsg->PartyID);
	if(pParty == NULL)
		return;
	if(pParty->GetMasterID() == 0) //파티 정보가 없으면
		return;
	MSG_DWORD smsg;
	smsg.Category = MP_PARTY;
	smsg.Protocol = MP_PARTY_BAN_ACK;
	smsg.dwData = pmsg->dwObjectID;
	pParty->SendMsgToAll(&smsg,sizeof(smsg));
	pParty->RemovePartyMember(pmsg->dwObjectID);

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(pPlayer)
		pPlayer->SetPartyIdx(0);
}
void CPartyManager::ChangeMasterParty(DWORD PartyID,DWORD PlayerID,DWORD TargetPlayerID) 
{
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.30
	// ..챌린지 존에서는 불가
	if (g_csDateManager.IsChallengeZoneHere())
		return;
	// E 데이트 존 추가 added by hseos 2007.11.30

	if(PlayerID == TargetPlayerID)
		return;
	
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(PlayerID);
	if(pPlayer == NULL)
		return;

	CParty* pParty = GetParty(pPlayer->GetPartyIdx());
	if(pParty == NULL)
	{
		ASSERTMSG(0,"파티가 존재하지 않습니다.");//pjslocal
		return;
	}

	if(pParty->IsPartyMember(pPlayer->GetID()) == FALSE)
		return;
	if(pParty->IsPartyMember(TargetPlayerID) == FALSE)
		return;
	if(pParty->GetMemberID(0) != pPlayer->GetID())		// 마스터가 아님
		return;

	PartyChangeMaster(pPlayer->GetPartyIdx(), PlayerID, TargetPlayerID);
	pParty->SetMasterChanging(TRUE);
}

void CPartyManager::NotifyChangeMasterParty(SEND_PARTY_MEMBER_INFO* pmsg)
{	
	CParty* pParty = GetParty(pmsg->PartyID);
	if(pParty == NULL)
		return;
	if(pParty->GetMasterID() == 0) //파티 정보가 없으면
		return;
	DoChangeMasterParty(pParty, pmsg->dwObjectID);
}
void CPartyManager::DoChangeMasterParty(CParty* pParty, DWORD TargetID)
{
	pParty->ChangeMaster(pParty->GetMasterID(),TargetID);

	MSG_DWORD smsg;
	smsg.Category = MP_PARTY;
	smsg.Protocol = MP_PARTY_CHANGEMASTER_ACK;
	smsg.dwData = TargetID;
	
	pParty->SendMsgToAll(&smsg,sizeof(smsg));
}
void CPartyManager::BreakupParty(DWORD PartyID,DWORD PlayerID) 
{
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.30
	// ..챌린지 존에서는 해체 불가
	if (g_csDateManager.IsChallengeZoneHere())
		return;
	// E 데이트 존 추가 added by hseos 2007.11.30

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(PlayerID);
	if(pPlayer == NULL)
		return;

	CParty* pParty = GetParty(pPlayer->GetPartyIdx());
	if(pParty == NULL)
	{
		ASSERTMSG(0,"파티가 존재하지 않습니다.");//pjslocal
		return;
	}

	if(pParty->IsMasterChanging(pPlayer, MP_PARTY_BREAKUP_NACK) == TRUE)
		return;
		
	MSGBASE send_msg;
	send_msg.Category = MP_PARTY;
	send_msg.Protocol = MP_PARTY_BREAKUP_ACK;
	send_msg.dwObjectID = pPlayer->GetID();

	pParty->SendMsgToAll(&send_msg,sizeof(send_msg));
	
	PartyBreakup(
		PartyID,
		PlayerID);
	NotifyChangesOtherMapServer(
		PlayerID,
		pParty,
		MP_PARTY_NOTIFYBREAKUP_TO_MAPSERVER,
		PartyID);
	
    pParty->BreakUp();

	m_partyRoomHashTable.Remove(PartyID);
	m_partyRoomPool->Free( pParty );
}

void CPartyManager::NotifyBreakupParty(SEND_PARTY_MEMBER_INFO* pmsg)
{	
	CParty* pParty = GetParty(pmsg->PartyID);
	if(pParty == NULL)
		return;

	MSGBASE send_msg;
	send_msg.Category = MP_PARTY;
	send_msg.Protocol = MP_PARTY_BREAKUP_ACK;
		
	pParty->SendMsgToAll(&send_msg,sizeof(send_msg));

	pParty->BreakUp();

	m_partyRoomHashTable.Remove(pmsg->PartyID);
	m_partyRoomPool->Free( pParty );
}

void CPartyManager::CreatePartyResult(CPlayer * pMaster, DWORD PartyIDX, BYTE Option, BYTE SubOption)
{
	CParty* pParty = NULL;

	if(CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
	{
		pParty = m_partyRoomPool->Alloc();
	}
	else
	{
		pParty = m_tempPartyRoomHashTable.GetData( pMaster->GetID() );

		/// 임시 파티가 있다면
		if( !pParty )
		{
			return;
		}
	}

	/// 임시파티를 다시 초기화 한뒤 정식 파티로 등록한다
	pParty->Init( PartyIDX, pMaster->GetID(), pMaster->GetObjectName(), pMaster, Option, SubOption );
	
	if(! CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
		m_tempPartyRoomHashTable.Remove( pMaster->GetID() );

	m_partyRoomHashTable.Add(pParty,PartyIDX);

	pMaster->SetPartyIdx(PartyIDX);

	MSG_DWORDBYTE2 send_msg;
	send_msg.Category = MP_PARTY;
	send_msg.Protocol = MP_PARTY_CREATE_ACK;
	send_msg.dwObjectID = pMaster->GetID();
	send_msg.dwData = PartyIDX;
	send_msg.bData1 = Option;
	send_msg.bData2 = SubOption;
	pMaster->SendMsg(&send_msg,sizeof(send_msg));

	pParty->SetMaster(pMaster);
	//	UserLogIn(pMaster);

	//Broadcasting
	// 090526 ShinJS --- Master를 등록할수 있도록 수정
	if(! CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
	{
		MSG_PARTY_CREATE msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category = MP_PARTY;
		msg.Protocol = MP_PARTY_NOTIFYCREATE_TO_MAPSERVER;
		msg.PartyIDX = PartyIDX;
		msg.PartyOpt = Option;
		msg.MasterID = pMaster->GetID();
		msg.MasterLv = pMaster->GetLevel();
		msg.DiceGrade = SubOption;
		SafeStrCpy( msg.Name, pMaster->GetObjectName(), MAX_NAME_LENGTH+1 );

		// 100722 ONS 파티생성시 Master의 Tip정보를 등록하도록 수정.
		const CHARACTER_TOTALINFO& totalInfo = pMaster->GetCharacterTotalInfo();
		msg.MapType = g_pServerSystem->GetMapNum();
		msg.JobGrade = totalInfo.JobGrade;
		msg.Race = totalInfo.Race;
		memcpy(
			msg.Job,
			totalInfo.Job,
			sizeof(msg.Job));

		g_Network.Send2AgentServer((char*)&msg, sizeof(msg));
	}

	/// 임시 맴버를 정식으로 등록한다.
	DWORD member = pParty->m_Member[1].dwMemberID;
	pParty->m_Member[1].dwMemberID = 0;
	AddMember( PartyIDX, member );
}

void CPartyManager::NotifyCreateParty(MSG_PARTY_CREATE* pmsg)
{
	const DWORD PartyIDX = pmsg->PartyIDX;
	const BYTE PartyOpt  =  pmsg->PartyOpt;
	const DWORD dwMasterID = pmsg->MasterID;
	const DWORD dwMasterLv = pmsg->MasterLv;
	const BYTE DiceGrade = pmsg->DiceGrade;
    
	CParty* pParty = m_partyRoomPool->Alloc();
	
	// 090526 ShinJS --- Master를 등록할수 있도록 수정
	pParty->Init(PartyIDX, dwMasterID, pmsg->Name, NULL, PartyOpt, DiceGrade, (LEVELTYPE)dwMasterLv);
	
	// 100722 ONS 파티생성시 파티장의 Tip정보를 설정한다.
	pParty->SetMasterTipInfo( pmsg->MapType, pmsg->JobGrade, pmsg->Job, pmsg->Race );
	    
	if(m_partyRoomHashTable.GetData(PartyIDX))
	{
		ASSERTMSG(0, "notify : 파티인덱스가 이미 있습니다.");
		return;
	}		

	m_partyRoomHashTable.Add(pParty,PartyIDX);	
}

CParty* CPartyManager::GetParty(DWORD partyIDx)
{
	CParty* pParty = (CParty*)m_partyRoomHashTable.GetData(partyIDx);
	return pParty;
}

void CPartyManager::UserLogIn(CPlayer* pPlayer,BOOL bNotifyUserLogin)
{
	if(0 == pPlayer->GetPartyIdx())
	{
		return;
	}

	CParty* const pParty = GetParty(pPlayer->GetPartyIdx());

	if(0 == pParty)
	{
		PartyInfoByUserLogin(pPlayer->GetPartyIdx(),pPlayer->GetID());
		return;
	}
	else if(FALSE == pParty->IsPartyMember(pPlayer->GetID()))
	{
		PartyInfoByUserLogin(pPlayer->GetPartyIdx(),pPlayer->GetID());
		return;
	}

	pParty->UserLogIn(pPlayer,bNotifyUserLogin);
	
	if(!pPlayer->GetMapChangePoint()) //로그인이면 파티원들에게 메시지
	{
		pParty->SendMemberLogInMsg(pPlayer->GetID());
		pParty->NotifyMemberLogin(pPlayer->GetID());
	}

	ChangeMasterOneself(*pPlayer);
}

void CPartyManager::NotifyUserLogIn(SEND_PARTY_MEMBER_INFO* pmsg)
{
	CParty* pParty = GetParty(pmsg->PartyID);
	if(pParty ==NULL)
		return;

	pParty->NotifyUserLogIn(pmsg);
}
void CPartyManager::UserLogOut(CPlayer* pPlayer)
{
	DWORD PartyID = pPlayer->GetPartyIdx();
	if(PartyID == 0)
		return;

	CParty* pParty = GetParty(PartyID);
	if(pParty == NULL)
	{
		char buf[128];
		sprintf(buf, "파티가 존재하지 않습니다. PartyIdx: %d", PartyID);
		ASSERTMSG(0,buf);
		return;
	}

	ChangeMasterRandomly(*pPlayer);
	pParty->UserLogOut(pPlayer->GetID()); 
	pParty->SendMsgUserLogOut(pPlayer->GetID());

	if(CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
	{
		stInstantPartyReservation* pReservation = m_InstantPartyReservationList.GetData(pPlayer->GetID());
		if(pReservation)
		{
			m_InstantPartyReservationList.Remove(pPlayer->GetID());
			m_InstantPartyReservationPool->Free(pReservation);
		}
	}

	NotifyChangesOtherMapServer(pPlayer->GetID(),pParty,MP_PARTY_NOTIFYMEMBER_LOGOUT_TO_MAPSERVER, PartyID);
}

void CPartyManager::NotifyUserLogOut(SEND_PARTY_MEMBER_INFO* pmsg)
{
	CParty* pParty = GetParty(pmsg->PartyID);
	if(pParty == NULL)
		return;
	if(pParty->GetMasterID() == 0) //파티 정보가 없으면
		return;
	pParty->UserLogOut(pmsg->dwObjectID);
	pParty->SendMsgUserLogOut(pmsg->dwObjectID);
}

void CPartyManager::DelMemberSyn(DWORD PlayerID, DWORD PartyID)
{
	if (g_csDateManager.IsChallengeZoneHere())
		return;

	PartyDelMember(PartyID, PlayerID);
}

void CPartyManager::NotifyDelSyn(MSG_DWORD2* pmsg)
{	
	DelMember(pmsg->dwData1,pmsg->dwData2);
}
void CPartyManager::NotifyChangesOtherMapServer(DWORD TargetPlayerID,CParty* pParty, BYTE Protocol, DWORD PartyIDX, LEVELTYPE Level)
{
	if(CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
		return;

	SEND_PARTY_MEMBER_INFO msg;
	ZeroMemory(&msg, sizeof(msg));
	msg.Category = MP_PARTY;
	msg.Protocol = Protocol;
	msg.dwObjectID = TargetPlayerID;
	msg.PartyID = PartyIDX;

	PARTY_MEMBER& member = msg.MemberInfo;
	member.dwMemberID = TargetPlayerID;
	member.Level = Level;
	member.mMapType = g_pServerSystem->GetMapNum();
	SafeStrCpy(
		member.Name,
		pParty->GetMemberName(TargetPlayerID),
		sizeof(member.Name) / sizeof(*member.Name));

	CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(TargetPlayerID);

	if(player)
	{
		const CHARACTER_TOTALINFO& totalInfo = player->GetCharacterTotalInfo();
		member.mRace = totalInfo.Race;
		member.mJobGrade = totalInfo.JobGrade;
		memcpy(
			member.mJob,
			totalInfo.Job,
			sizeof(member.mJob));
	}

	g_Network.Send2AgentServer((char*)&msg,sizeof(msg));
}

void CPartyManager::RemoveParty(DWORD DeletedPlayerID, DWORD PartyID)
{
	CParty* pParty = GetParty(PartyID);
	if(pParty == NULL)
		return;
	pParty->RemovePartyMember(DeletedPlayerID);
}


void CPartyManager::SendErrMsg(DWORD PlayerID, int ErrKind, BYTE Protocol)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(PlayerID);
	if(pPlayer == NULL)
		return;

	MSG_BYTE msg;
	msg.Category = MP_PARTY;
	msg.Protocol = Protocol;
	msg.bData = BYTE( ErrKind );
	pPlayer->SendMsg(&msg, sizeof(msg));
}

void CPartyManager::SendOptionMsg(MSG_DWORD2* pMsg)
{
	CParty* pParty = GetParty(pMsg->dwData1) ;
	if( !pParty ) return ;

	const BYTE option = BYTE(pMsg->dwData2);

	MSG_BYTE message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_PARTY;
	message.Protocol = MP_PARTY_CHANGE_PICKUPITEM_ACK;
	message.bData = BYTE(option);
	pParty->SendMsgToAll(
		&message,
		sizeof(message));
	pParty->SetOption(option);
}

void CPartyManager::NotifyMemberLoginMsg(DWORD PartyID, DWORD PlayerID)
{
	CParty* pParty = GetParty(PartyID);
	if(pParty == NULL)
		return;

	pParty->SendMemberLogInMsg(PlayerID);
}

void CPartyManager::MemberLevelUp(DWORD PartyIDX, DWORD PlayerIDX, LEVELTYPE lvl)
{
	DoMemberLevelUp(PartyIDX, PlayerIDX, lvl);
	
	if(CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
		return;

	MSG_DWORD3 msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_NOTIFYMEMBER_LEVEL;
	msg.dwData1 = PartyIDX;
	msg.dwData2 = PlayerIDX;
	msg.dwData3 = lvl;	
	g_Network.Send2AgentServer((char*)&msg,sizeof(msg));	
}

void CPartyManager::DoMemberLevelUp(DWORD PartyIDX, DWORD PlayerIDX, LEVELTYPE lvl)
{
	CParty* pParty = PARTYMGR->GetParty(PartyIDX);
	if(pParty == NULL)
		return;

	pParty->SetMemberLevel(PlayerIDX, lvl);

	MSG_DWORD2 msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_MEMBERLEVEL;
	msg.dwData1 = PlayerIDX;
	msg.dwData2 = lvl;
	pParty->SendMsgExceptOne(&msg, sizeof(msg), PlayerIDX);
}

void CPartyManager::SendObtainItemMsg(CPlayer* pPlayer, DWORD ObtainIdx)
{
	CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx());
	if(pParty == NULL)
		return;
	MSG_DWORD2 msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_MONSTER_OBTAIN_NOTIFY;
	msg.dwData1 = pPlayer->GetID();
	msg.dwData2 = ObtainIdx;
	pParty->SendMsgExceptOneinChannel(&msg, sizeof(msg), pPlayer->GetID(), pPlayer->GetGridID());
}

void CPartyManager::SendObtainItemByDiceMsg(CPlayer* pPlayer, DWORD ObtainIdx, DWORD Num)
{
	CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx());
	if(pParty == NULL)
		return;
	MSG_DWORD3 msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_MONSTER_OBTAIN_BYDICE_NOTIFY;
	msg.dwData1 = pPlayer->GetID();
	msg.dwData2 = ObtainIdx;
	msg.dwData3 = Num;
	pParty->SendMsgExceptOneinChannel(&msg, sizeof(msg), pPlayer->GetID(), pPlayer->GetGridID());
}

void CPartyManager::CreateParty(CPlayer* pMaster, BYTE bOption, BYTE bSubOption)
{
	/// 임시 파티 생성
	CParty* pParty = m_partyRoomPool->Alloc();
	pParty->Init(0, pMaster->GetID(), "", NULL,bOption, bSubOption);
	m_tempPartyRoomHashTable.Add( pParty, pMaster->GetID() );

	PartyCreate( pParty->GetMasterID(), pParty->GetOption(), pParty->GetDiceGradeOpt() );
}

DWORD CPartyManager::GetRandomMember(DWORD PartyID)
{
	CParty* pParty = GetParty(PartyID);
	if(pParty)
		return pParty->GetRandomMember();

	return 0;
}

void CPartyManager::ChangeMasterRandomly(CPlayer& player)
{
	CParty* const party = GetParty(player.GetPartyIdx());

	if(0 == party)
	{
		return;
	}
	else if(party->GetMasterID() != player.GetID())
	{
		return;
	}
	else if(2 > party->GetPartyMemberNum())
	{
		return;
	}

	for(DWORD i = 1; i < MAX_PARTY_LISTNUM; ++i)
	{
		const PARTY_MEMBER& member = party->GetMember(i);

		if(0 == member.dwMemberID)
		{
			continue;
		}
		else if(FALSE == member.bLogged)
		{
			continue;
		}

		PartyChangeMaster(
			party->GetPartyIdx(),
			party->GetMasterID(),
			member.dwMemberID);
		break;
	}
}

void CPartyManager::ChangeMasterOneself(CPlayer& player)
{
	CParty* const party = GetParty(player.GetPartyIdx());

	if(0 == party)
	{
		return;
	}
	else if(party->GetMasterID() == player.GetID())
	{
		return;
	}
	else if(2 > party->GetPartyMemberNum())
	{
		return;
	}

	size_t loginMemberSize = 0;

	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		const PARTY_MEMBER& member = party->GetMember(i);

		if(0 == member.dwMemberID)
		{
			continue;
		}
		else if(FALSE == member.bLogged)
		{
			continue;
		}

		++loginMemberSize;
	}

	if(1 < loginMemberSize)
	{
		return;
	}

	PartyChangeMaster(
		party->GetPartyIdx(),
		party->GetMasterID(),
		player.GetID());
}

BOOL CPartyManager::CanUseInstantPartyMap(MAPTYPE mapNum)
{
	if(g_csDateManager.IsChallengeZone(mapNum))
		return TRUE;

	return FALSE;
}

DWORD CPartyManager::CreateInstantParty(DWORD dwMasterIdx)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwMasterIdx);
	if(! pPlayer)
		return 0;

	DWORD dwInstantPartyIndex = GenerateInstantPartyIdx();
	if(! dwInstantPartyIndex)
		return 0;

	CreatePartyResult(pPlayer, dwInstantPartyIndex, ePartyOpt_Sequence, 0);
	CParty* pParty = GetParty(dwInstantPartyIndex);

	if(! pParty)
		return 0;

	CPlayer* pMember;
	stInstantPartyReservation* pReservation;
	pReservation = m_InstantPartyReservationList.GetData(dwMasterIdx);
	if(! pReservation)
		return 0;

	m_InstantPartyReservationList.Remove(dwMasterIdx);
	m_InstantPartyReservationPool->Free(pReservation);

	// 파티를 생성했다. 파티장보다 먼저 들어온 파티원들 초대하자~
	m_InstantPartyReservationList.SetPositionHead();
	while((pReservation = m_InstantPartyReservationList.GetData()) != NULL)
	{
		if(pReservation->dwMasterID == pPlayer->GetID())
		{
			pMember = (CPlayer*)g_pUserTable->FindUser(pReservation->dwPlayerID);
			if(pMember && (pPlayer->GetGridID() == pMember->GetGridID()))
			{
				AddMemberResult(pParty, pMember);
				PARTYMGR->UserLogIn( pMember, TRUE );

				m_InstantPartyReservationList.Remove(pReservation->dwPlayerID);
				m_InstantPartyReservationPool->Free(pReservation);
			}
		}
	}

	return dwInstantPartyIndex;
}

DWORD CPartyManager::GenerateInstantPartyIdx()
{
	return m_InstantPartyIDXGenerator.GenerateIndex();
}

void CPartyManager::ReleaseInstantPartyIdx(DWORD dwInstantPartyIdx)
{
	m_InstantPartyIDXGenerator.ReleaseIndex(dwInstantPartyIdx);
}

void CPartyManager::AddReservationInstantPartyMember(DWORD dwPlayerIndex, DWORD dwMasterIndex)
{
	stInstantPartyReservation* pReservation = m_InstantPartyReservationList.GetData(dwPlayerIndex);
	if(pReservation)
	{
		pReservation->dwPlayerID = dwPlayerIndex;
		pReservation->dwMasterID = dwMasterIndex;
	}
	else
	{
		pReservation = m_InstantPartyReservationPool->Alloc();
		if(pReservation)
		{
			ZeroMemory(pReservation, sizeof(stInstantPartyReservation));
			pReservation->dwPlayerID = dwPlayerIndex;
			pReservation->dwMasterID = dwMasterIndex;
			m_InstantPartyReservationList.Add(pReservation, dwPlayerIndex);
		}
	}
}

DWORD CPartyManager::AutoJoinToInstantParty(DWORD dwPlayerIndex, DWORD dwMasterIndex)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if(! pPlayer)
		return 0;

	CParty* pParty = GetPartyFromMasterID(dwMasterIndex);
	if(! pParty)
		return 0;

	stInstantPartyReservation* pReservation;
	pReservation = m_InstantPartyReservationList.GetData(dwPlayerIndex);
	if(! pReservation)
		return 0;

	m_InstantPartyReservationList.Remove(pReservation->dwPlayerID);
	m_InstantPartyReservationPool->Free(pReservation);

	AddMemberResult(pParty, pPlayer);
	PARTYMGR->UserLogIn( pPlayer, TRUE );
	return pParty->GetPartyIdx();
}

CParty* CPartyManager::GetPartyFromMasterID(DWORD dwMasterID)
{
	CParty* pParty;
	m_partyRoomHashTable.SetPositionHead();
	while((pParty = m_partyRoomHashTable.GetData()) != NULL)
	{
		if(pParty->GetMasterID() == dwMasterID)
			return pParty;
	}

	return NULL;
}

void CPartyManager::ProcessReservationList(DWORD dwPlayerIndex)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if(! pPlayer)
		return;

	stInstantPartyReservation* pReservation = m_InstantPartyReservationList.GetData(dwPlayerIndex);
	if(! pReservation)
		return;

	if(pReservation->dwMasterID == pPlayer->GetID())
		CreateInstantParty(pPlayer->GetID());
	else
		AutoJoinToInstantParty(pPlayer->GetID(), pReservation->dwMasterID);
}

void CPartyManager::BreakupInstantParty(DWORD PartyID,DWORD PlayerID)
{
	if(! CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
		return;

	if(PartyID >= (UINT_MAX - 50000))
		ReleaseInstantPartyIdx(PartyID);

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(PlayerID);
	if(pPlayer == NULL)
		return;

	CParty* pParty = GetParty(pPlayer->GetPartyIdx());
	if(pParty == NULL)
	{
		ASSERTMSG(0,"파티가 존재하지 않습니다.");//pjslocal
		return;
	}

	MSGBASE send_msg;
	send_msg.Category = MP_PARTY;
	send_msg.Protocol = MP_PARTY_BREAKUP_ACK;
	send_msg.dwObjectID = pPlayer->GetID();

	pParty->SendMsgToAll(&send_msg,sizeof(send_msg));

	
    pParty->BreakUp();

	m_partyRoomHashTable.Remove(PartyID);
	m_partyRoomPool->Free( pParty );
}

void CPartyManager::DelMemberInstantParty(DWORD PlayerID,DWORD PartyID)
{
	CParty* pParty = GetParty(PartyID);
	if(pParty == NULL)
	{
		ASSERTMSG(0,"There is no Party.");
		return;
	}
	
	if(pParty->IsPartyMember(PlayerID) == FALSE)
		return;
	if(pParty->GetMasterID() == 0) //파티 정보가 없으면
		return;

	MSG_DWORD smsg;
	smsg.Category = MP_PARTY;
	smsg.Protocol = MP_PARTY_DEL_ACK;
	smsg.dwData = PlayerID;
	pParty->SendMsgToAll(&smsg,sizeof(smsg));

	pParty->RemovePartyMember(PlayerID);
	
	CPlayer * pPlayer = (CPlayer*)g_pUserTable->FindUser(PlayerID);
	if(pPlayer)
		pPlayer->SetPartyIdx(0);
}
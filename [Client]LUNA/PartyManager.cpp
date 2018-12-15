#include "stdafx.h"
#include "GameIn.h"
#include "PartyManager.h"

#include "WindowIdEnum.h"
#include "./interface/cWindowManager.h"
#include "objectmanager.h"
#include "ChatManager.h"
#include "ItemManager.h"
#include "PartyIconManager.h"
#include "PartyInviteDlg.h"
#include "cResourceManager.h"
#include "Interface/cStatic.h"
#include "PartyBtnDlg.h"
#include "PartyMemberDlg.h"
#include "DateMatchingDlg.h"
#include "../hseos/Date/SHDateManager.h"

GLOBALTON(CPartyManager);

// 070226 LYW --- PartyManager : Modified message number.

CPartyManager::CPartyManager()
{
}

CPartyManager::~CPartyManager()
{
}

void CPartyManager::Init()
{
	m_MasterChanging = FALSE;
	m_Party.Clear();
	m_bIsProcessing = FALSE;
	m_RequestPartyID = 0;
	m_PartyOption = ePartyOpt_Damage;
}

void CPartyManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_PARTY_INFO: 						Party_Info( pMsg ) ;										break;		
	case MP_PARTY_ADD_INVITE:					Party_Add_Invite( pMsg ) ;									break;	
	case MP_PARTY_ADD_ACK:						Party_Add_Ack( pMsg ) ;										break; 
	case MP_PARTY_ADD_NACK:						Party_Add_Nack( pMsg ) ;									break;		
	case MP_PARTY_DEL_ACK:						Party_Del_Ack( pMsg ) ;										break;		
	case MP_PARTY_DEL_NACK:						Party_Del_Nack( pMsg ) ;									break;		
	case MP_PARTY_CREATE_ACK:					Party_Create_Ack( pMsg ) ;									break;		
	case MP_PARTY_CREATE_NACK:					Party_Create_Nack( pMsg ) ;									break;		
	case MP_PARTY_BAN_ACK:						Party_Ban_Ack( pMsg ) ;										break;		
	case MP_PARTY_BAN_NACK:																					break;		
	case MP_PARTY_MEMBER_LOGIN:					Party_Member_Login( pMsg ) ;								break;		
	case MP_PARTY_MEMBER_LOGOUT:				Party_Member_Logout( pMsg ) ;								break;		
	case MP_PARTY_CHANGEMASTER_ACK:				Party_ChangeMaster_Ack( pMsg ) ;							break;		
	case MP_PARTY_CHANGEMASTER_NACK:			ASSERT(0); SetMasterChanging(FALSE);						break;		
	case MP_PARTY_BREAKUP_ACK:					Party_Breakup_Ack( pMsg ) ;									break;		
	case MP_PARTY_BREAKUP_NACK:					Party_Breaup_Nack( pMsg ) ;									break;
	case MP_PARTY_MEMBERLIFE:					Party_MemberLife( pMsg ) ;									break;
	case MP_PARTY_MEMBERMANA:					Party_MemberMana( pMsg ) ;									break;
	case MP_PARTY_MEMBERLEVEL:
		{
			const MSG_DWORD2* message = (MSG_DWORD2*)pMsg;
			const DWORD playerIndex = message->dwData1;
			const LEVELTYPE playerLevel = LEVELTYPE(message->dwData2);

			SetMemberLevel(
				playerIndex,
				playerLevel);
			break;
		}
	case MP_PARTY_SENDPOS:						Party_SendPos( pMsg ) ;										break;
	case MP_PARTY_REVIVEPOS:					Party_RevivePos( pMsg ) ;									break;
	case MP_PARTY_INVITE_DENY_ACK:				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(407) );	break;
	case MP_PARTY_INVITE_ACCEPT_ACK:			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(529) );		break;
	case MP_PARTY_INVITE_ACCEPT_NACK:			Party_Invite_Accept_Nack( pMsg ) ;							break;
	case MP_PARTY_MEMBER_LOGINMSG:				Party_Member_LoginMsg( pMsg ) ;								break;
	case MP_PARTY_MONSTER_OBTAIN_NOTIFY:		Party_Monster_Obtain_Notify( pMsg ) ;						break;
	case MP_PARTY_MONSTER_OBTAIN_BYDICE_NOTIFY:	Party_Monster_ObtainByDice_Notify( pMsg );					break;
	case MP_PARTY_CHANGE_PICKUPITEM_ACK :		Party_Change_PickupItem_Ack( pMsg ) ;						break;		
	case MP_PARTY_CHANGE_PICKUPITEM_NACK :		Party_Change_PickupItem_Nack( pMsg ) ;						break;
	case MP_PARTY_CHANGE_DICEGRADE_ACK :		Party_Change_DiceGrade_Ack( pMsg ) ;						break;
	case MP_PARTY_CHANGE_DICEGRADE_NACK :		Party_Change_DiceGrade_Nack( pMsg ) ;						break;
	
	// 090528 ShinJS --- 이름으로 파티초대시 에러처리
	case MP_PARTY_INVITE_BYNAME_NACK:			Party_InviteByName_Nack( pMsg );							break;
	} 
}

void CPartyManager::SetPickupItem( int option )
{
	MSG_DWORD2 msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_CHANGE_PICKUPITEM_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = option;
	msg.dwData2 = HERO->GetPartyIdx();
	NETWORK->Send(&msg, sizeof(msg));
}

void CPartyManager::SetDiceGrade( int option )
{
	MSG_DWORD2 msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_CHANGE_DICEGRADE_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = option;
	msg.dwData2 = HERO->GetPartyIdx();
	NETWORK->Send(&msg, sizeof(msg));
}

void CPartyManager::AddPartyMemberSyn(DWORD AddPartymemeberID) 
{
	if(AddPartymemeberID == 0)
	{
		ASSERT(0);
		return;
	}
	if(m_Party.IsPartyMember(AddPartymemeberID) == TRUE)
		return;

	if(m_Party.GetMemberNum() != MAX_PARTY_LISTNUM)
	{
		MSG_DWORD2 msg;
		msg.Category	= MP_PARTY;
		msg.Protocol	= MP_PARTY_ADD_SYN;
		msg.dwObjectID	= HEROID;
		msg.dwData1		= AddPartymemeberID;			
		msg.dwData2		= HERO->GetPartyIdx();
		NETWORK->Send(&msg,sizeof(msg));
	}
	else 
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(110) );
}

void CPartyManager::InvitePartyMemberByNameSyn( char* szCharName )
{
	// 이미 파티원인 경우
	if( m_Party.IsPartyMember( szCharName ) )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1038 ) );
		return;
	}

	if(m_Party.GetMemberNum() != MAX_PARTY_LISTNUM)
	{
		MSG_NAME_DWORD msg;
		msg.Category	= MP_PARTY;
		msg.Protocol	= MP_PARTY_INVITE_BYNAME_SYN;
		msg.dwObjectID	= HEROID;
		SafeStrCpy( msg.Name, szCharName, MAX_NAME_LENGTH+1 );
		msg.dwData		= HERO->GetPartyIdx();
		NETWORK->Send( &msg,sizeof(msg) );
	}
	else 
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(110) );
}

void CPartyManager::DelPartyMemberSyn() 
{
	if(HEROID == GetMasterID())
		ASSERTMSG(0,"방장은 탈퇴를 할 수 없다.");//pjslocal
	
	MSG_DWORD msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_DEL_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData = HERO->GetPartyIdx();
	NETWORK->Send(&msg,sizeof(msg));
}

void CPartyManager::ChangeMasterPartySyn(DWORD FromMemberID, DWORD ToMemberID) 
{
	// 100120 LUJ, 오브젝트 선택으로도 처리할 수 있도록 한다
	ToMemberID = (0 == ToMemberID ? OBJECTMGR->GetSelectedObjectID() : ToMemberID);

	if (g_csDateManager.IsChallengeZoneHere())
	{
		return;
	}
	else if(ToMemberID == FromMemberID)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1488));
		return;
	}
	else if(m_Party.IsPartyMember(ToMemberID) == FALSE)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1043));
		return;
	}
	
	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		const PARTY_MEMBER* const member = m_Party.GetPartyMemberInfo(i);

		if(member->dwMemberID != ToMemberID)
		{
			continue;
		}
		else if(FALSE == member->bLogged)
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1920));
			return;
		}

		break;
	}

	MSG_DWORD2 msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_CHANGEMASTER_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = ToMemberID;
	msg.dwData2 = HERO->GetPartyIdx();
	NETWORK->Send(&msg, sizeof(msg));
	
	SetMasterChanging(TRUE);	
}

void CPartyManager::BreakUpSyn() 
{
	if(IsMasterChanging() == TRUE)
		return;
	if(IsProcessing() == TRUE)
		return;

	MSG_DWORD msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_BREAKUP_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData = HERO->GetPartyIdx();
	NETWORK->Send(&msg,sizeof(msg));

	SetIsProcessing(TRUE);
}

void CPartyManager::BanPartyMemberSyn(DWORD DelPartymemberID) 
{
	// 100120 LUJ, 오브젝트 선택으로도 처리할 수 있도록 한다
	DelPartymemberID = (0 == DelPartymemberID ? OBJECTMGR->GetSelectedObjectID() : DelPartymemberID);

	if (g_csDateManager.IsChallengeZoneHere())
	{
		return;
	}
	else if(IsMasterChanging() == TRUE)
	{
		return;
	}
	else if(m_Party.IsPartyMember(DelPartymemberID) == FALSE)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1043));
		return;
	}

	MSG_DWORD2 msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_BAN_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = DelPartymemberID;
	msg.dwData2 = HERO->GetPartyIdx();
	NETWORK->Send(&msg,sizeof(msg));
}

void CPartyManager::CreatePartyResult(MSG_DWORDBYTE2* pMsg)
{
	m_Party.Clear();
//	m_Party.SetPartyIdx(pMsg->dwData);
	HERO->SetPartyIdx(pMsg->dwData);
	m_Party.SetDiceGrade(pMsg->bData2);
	m_Party.SetMaster(HEROID,HERO->GetObjectName(),TRUE,HERO->GetLifePercent(),HERO->GetManaPercent());

	SetIsProcessing(FALSE);
	GAMEIN->GetPartyDialog()->RefreshDlg();
	
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(96) );
	GAMEIN->GetPartyDialog()->SetActive(TRUE);
	
	OBJECTMGR->ApplyOverInfoOption( HERO );//

	CPartyBtnDlg* pPartySet = NULL ;
	pPartySet = (CPartyBtnDlg*)WINDOWMGR->GetWindowForID( PA_BTNDLG );

	if( pPartySet )
	{
		pPartySet->SetDistribute(pMsg->bData1);
		pPartySet->SetDiceGrade(pMsg->bData2);
	}
}

void CPartyManager::AddPartyMemberResult(SEND_PARTY_MEMBER_INFO* pMsg) 
{
	if(pMsg->MemberInfo.dwMemberID == HEROID) //추가된 사람
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(98) );
		HERO->SetPartyIdx(pMsg->PartyID);

		OBJECTMGR->ApplyOverInfoOptionToAll();//--setpartyinfo와 두번겹치는가? confirm
	}
	else //방파원들
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(97), pMsg->MemberInfo.Name);
		m_Party.AddMember(&pMsg->MemberInfo);

		CObject* pMember = OBJECTMGR->GetObject( pMsg->MemberInfo.dwMemberID );
		OBJECTMGR->ApplyOverInfoOption( pMember );//

		// 파티원 아이콘 추가.
		if(pMsg->MemberInfo.bLogged && pMsg->MemberInfo.LifePercent)
		{
			PICONMGR->CreatePartyIcon(pMsg->MemberInfo.dwMemberID, &pMsg->MemberInfo);
		}
	}
	
	GAMEIN->GetPartyDialog()->RefreshDlg();

	// 080318 LUJ,	파티에 추가된 플레이어의 스킬을 검색해서 추가해준다. 스킬 정보가 따로 서버에서 전송되지 않으므로 클라이언트에서 찾는다
	GAMEIN->GetPartyDialog()->InitializeSkill( pMsg->MemberInfo );
}

void CPartyManager::DelPartyMemberResult(MSG_DWORD* pMsg) 
{
	if(pMsg->dwData == HEROID) 
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(103) );
		HERO->SetPartyIdx(0);
		m_Party.Clear();

		GAMEIN->GetPartyDialog()->SetActive(FALSE);
		OBJECTMGR->ApplyOverInfoOptionToAll();//

		// 090121 NYJ - 파티 나갈때 파티멤버의 스킬정보 초기화.
		GAMEIN->GetPartyDialog()->ClearMemberSkillList();
	}
	else 
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(102), m_Party.GetMemberName(pMsg->dwData));
		m_Party.DelMember(pMsg->dwData);		
		CObject* pMember = OBJECTMGR->GetObject( pMsg->dwData );
		OBJECTMGR->ApplyOverInfoOption( pMember );//
	}
	GAMEIN->GetPartyDialog()->RefreshDlg();
}

void CPartyManager::BanPartyMemberResult(MSG_DWORD* pMsg) 
{
	if(pMsg->dwData == HEROID)
	{		
		GAMEIN->GetPartyDialog()->SetActive(FALSE);
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(106) );
		HERO->SetPartyIdx(0);
		m_Party.Clear();
		
		OBJECTMGR->ApplyOverInfoOptionToAll();

		PICONMGR->DeleteAllPartyIcon();
	}
	else 
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(105), m_Party.GetMemberName(pMsg->dwData));
		
		m_Party.DelMember(pMsg->dwData);

		CObject* pMember = OBJECTMGR->GetObject( pMsg->dwData );
		OBJECTMGR->ApplyOverInfoOption( pMember );

		PICONMGR->DeletePartyIcon(pMsg->dwData);
	}
	GAMEIN->GetPartyDialog()->SetClickedMemberID(0);
	GAMEIN->GetPartyDialog()->RefreshDlg();
}

void CPartyManager::UserLogIn(SEND_PARTY_MEMBER_INFO* pMsg) 
{
	m_Party.LogIn(&pMsg->MemberInfo);
	GAMEIN->GetPartyDialog()->RefreshDlg();
}

void CPartyManager::UserLogOut(MSG_DWORD* pMsg) 
{
	m_Party.LogOut(pMsg->dwData);
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(404), m_Party.GetMemberName(pMsg->dwData));

	GAMEIN->GetPartyDialog()->RefreshDlg();
}

void CPartyManager::ChangeMasterPartyResult(MSG_DWORD* pMsg) 
{
	// 1.멤버별 스킬리스트 백업
	GAMEIN->GetPartyDialog()->BackupMemberSkillList();

	m_Party.ChangeMaster(pMsg->dwData);

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(406), m_Party.GetMemberName(pMsg->dwData) );
	GAMEIN->GetPartyDialog()->RefreshDlg();

	// 2.재배치된 결과에 따라 스킬리스트 갱신한다.
	GAMEIN->GetPartyDialog()->UpdateMemberSkillList();

	// 100127 ONS 데이트매칭 1:1채팅상태에서 파티장을 이양했을경우, 매칭챌린지 버튼을 갱신시킨다.
	CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;
	if( !pDlg )
		return;

	if(pDlg->GetChatingDlg()->IsOnChatMode())
	{
		const DWORD dwPlayerIndex = pMsg->dwData;
		if(dwPlayerIndex == HEROID)
		{
			pDlg->ActivateBtnEnterChallengeZone(TRUE);
		}
		else
		{
			pDlg->ActivateBtnEnterChallengeZone(FALSE);
		}
	}
}

void CPartyManager::BreakUpPartyResult()
{
	HERO->SetPartyIdx(0);
	m_Party.Clear();
	m_Party.BreakUpResult();
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 104 ) );
	GAMEIN->GetPartyDialog()->RefreshDlg();
	GAMEIN->GetPartyDialog()->SetActive(FALSE);
	SetIsProcessing(FALSE);
	OBJECTMGR->ApplyOverInfoOptionToAll();

	GAMEIN->GetPartyDialog()->ClearMemberSkillList();
}

void CPartyManager::SetPartyInfo(PARTY_INFO* pmsg) 
{
	m_Party.InitParty(pmsg);
	
	OBJECTMGR->ApplyOverInfoOptionToAll();//로그인이나 파티에 들어왔을 때
}

void CPartyManager::PartyInviteAccept()
{
	MSG_DWORD msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_INVITE_ACCEPT_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData = GetRequestPartyID();
	
	NETWORK->Send(&msg,sizeof(msg));
}

void CPartyManager::PartyInviteDeny()
{
	MSG_DWORD2 msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_INVITE_DENY_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = GetRequestPartyID();
	msg.dwData2 = GAMEIN->GetPartyInviteDialog()->GetInviterPlayerIndex();
	NETWORK->Send(&msg,sizeof(msg));

	SetRequestPartyID(0);
}

BOOL CPartyManager::IsPartyMember(DWORD PlayerID)
{
	return m_Party.IsPartyMember(PlayerID);
}

PARTY_MEMBER* CPartyManager::GetPartyMemberInfo(int i)
{
	return m_Party.GetPartyMemberInfo(i);
}

BOOL CPartyManager::PartyChat(char* ChatMsg, char* SenderName)
{
	if( HERO->GetPartyIdx() == 0 )
		return FALSE;
	
	SEND_PARTY_CHAT msg;
	memset(&msg, 0, sizeof(msg));
	msg.Category = MP_CHAT;
	msg.Protocol = MP_CHAT_PARTY;
	msg.dwObjectID = gHeroID;
	strcpy(msg.Name, SenderName);
	strcpy(msg.Msg, ChatMsg);
	for(int n=0; n<MAX_PARTY_LISTNUM; ++n)
	{
		const PARTY_MEMBER* const member = m_Party.GetPartyMemberInfo(n);

		if(0 == member)
		{
			continue;
		}
		else if(FALSE == member->bLogged)
		{
			continue;
		}

		msg.MemberID[msg.MemberNum] = member->dwMemberID;
		++msg.MemberNum;
	}
	//NETWORK->Send(&msg, sizeof(msg));
	NETWORK->Send(&msg, msg.GetMsgLength()); //CHATMSG 040324

	return TRUE;
}

BOOL CPartyManager::IsMasterChanging()
{
	if(m_MasterChanging == TRUE)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(412) );
		return TRUE;
	}
	return FALSE;
}

BOOL CPartyManager::CanActivate()
{
	if(HERO->GetPartyIdx() != 0)
	{
		return TRUE;
	}
	else if(m_bIsProcessing == TRUE)
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(408));
		return FALSE;
	}

	cDialog* const dialog = g_cWindowManager.GetWindowForID(PA_CREATEDLG);

	if(dialog)
	{
		dialog->SetActive(TRUE);
	}

	return FALSE;
}

void CPartyManager::Party_Info( void* pMsg )
{
	PARTY_INFO* pmsg = (PARTY_INFO*)pMsg;
	SetPartyInfo(pmsg);
	GAMEIN->GetPartyDialog()->SetActive(TRUE);
	GAMEIN->GetPartyDialog()->RefreshDlg();

	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		const PARTY_MEMBER& member = pmsg->Member[i];

		if(member.dwMemberID == gHeroID)
		{
			continue;
		}
		else if(FALSE == member.bLogged)
		{
			continue;
		}
		else if(0 == member.LifePercent)
		{
			continue;
		}

		PICONMGR->CreatePartyIcon(pmsg->Member[i].dwMemberID, &pmsg->Member[i]);

		// 080318 LUJ, 파티원 각각의 보유 스킬도 갱신해주자
		GAMEIN->GetPartyDialog()->InitializeSkill(member);
	}
}


void CPartyManager::Party_Add_Invite( void* pMsg )
{
	MSG_NAME_DWORD3* pmsg = (MSG_NAME_DWORD3*)pMsg;
	SetRequestPartyID(pmsg->dwData1);

	// 챌린지 존에서는 자동거부하자.
	if(g_csDateManager.IsChallengeZoneHere())
	{
		PartyInviteDeny();
		return;
	}

	GAMEIN->GetPartyInviteDialog()->Update(
		*pmsg);
	GAMEIN->GetPartyInviteDialog()->SetActive(
		TRUE);
}


void CPartyManager::Party_Add_Ack( void* pMsg )
{
	SEND_PARTY_MEMBER_INFO* pmsg = (SEND_PARTY_MEMBER_INFO*)pMsg;
	CPartyDialog* dialog = GAMEIN->GetPartyDialog();
	dialog->BackupMemberSkillList();
	AddPartyMemberResult(pmsg);	
	dialog->UpdateMemberSkillList();
}


void CPartyManager::Party_Add_Nack( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	
	switch(pmsg->dwData)
	{
	case eErr_Add_NoPlayer:
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(405)); break;
	case eErr_Add_AlreadyinParty:
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(107)); break;
	case eErr_Add_OptionNoParty:
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(108)); break;
	case eErr_Add_DiedPlayer:
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(411)); break;
	}			
}


void CPartyManager::Party_Del_Ack( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CPartyDialog* dialog = GAMEIN->GetPartyDialog();
	dialog->BackupMemberSkillList();
	dialog->RemoveMemberSkillList(pmsg->dwData);
	DelPartyMemberResult(pmsg);
	
	if(pmsg->dwData == gHeroID)			
		PICONMGR->DeleteAllPartyIcon();			
	else			
		PICONMGR->DeletePartyIcon(pmsg->dwData);

	dialog->UpdateMemberSkillList();
}


void CPartyManager::Party_Del_Nack( void* pMsg )
{
	MSG_INT* pmsg = (MSG_INT*)pMsg;
	
	switch(pmsg->nData)
	{
	case eErr_ChangingMaster:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(412) ); break;
	}
}


void CPartyManager::Party_Create_Ack( void* pMsg )
{
	MSG_DWORDBYTE2* pmsg = (MSG_DWORDBYTE2*)pMsg;
	CreatePartyResult(pmsg);
}


void CPartyManager::Party_Create_Nack( void* pMsg )
{
	ASSERT(0);
	MSG_BYTE* pmsg = (MSG_BYTE* )pMsg;

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(409), pmsg->bData);
	
	//SW060712 release Lock
	SetIsProcessing(FALSE);
}


void CPartyManager::Party_Ban_Ack( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CPartyDialog* dialog = GAMEIN->GetPartyDialog();
	dialog->BackupMemberSkillList();
	dialog->RemoveMemberSkillList(pmsg->dwData);
	BanPartyMemberResult(pmsg);	

	dialog->UpdateMemberSkillList();
}


// 080318 LUJ, 파티원 스킬 아이콘 초기화
void CPartyManager::Party_Member_Login( void* pMsg )
{
	SEND_PARTY_MEMBER_INFO* pmsg = (SEND_PARTY_MEMBER_INFO*)pMsg;
	UserLogIn(pmsg);
	
	if(pmsg->MemberInfo.bLogged && pmsg->MemberInfo.dwMemberID != gHeroID)
	{
		if(pmsg->MemberInfo.LifePercent)			// 새로 접속한 파티원
			PICONMGR->CreatePartyIcon(pmsg->MemberInfo.dwMemberID, &pmsg->MemberInfo);
		else			// 기존에 접속해 있던 파티원
		{
			PICONMGR->DeletePartyIcon(pmsg->MemberInfo.dwMemberID);

			// 080318 LUJ, 파티원의 스킬 아이콘 표시를 제거한다
			{
				CPartyDialog* dialog = GAMEIN->GetPartyDialog();

				if( dialog )
				{
					dialog->RemoveAllSkill( pmsg->MemberInfo.dwMemberID );
				}
			}			
		}
	}
}


// 080318 LUJ, 파티원 스킬 아이콘 초기화
void CPartyManager::Party_Member_Logout( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	UserLogOut(pmsg);
	
	PICONMGR->DeletePartyIcon(pmsg->dwData);

	// 080318 LUJ, 해당 파티원이 가진 스킬 아이콘을 초기화한다
	{
		CPartyDialog* dialog = GAMEIN->GetPartyDialog();

		if( dialog )
		{
			dialog->RemoveAllSkill( pmsg->dwData );
		}
	}
}


void CPartyManager::Party_ChangeMaster_Ack( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	ChangeMasterPartyResult(pmsg);
	SetMasterChanging(FALSE);
}


void CPartyManager::Party_Breakup_Ack( void* pMsg )
{
	BreakUpPartyResult();

	PICONMGR->DeleteAllPartyIcon();
}


void CPartyManager::Party_Breaup_Nack( void* pMsg )
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(412) );
						
	//SW060712 release Lock
	SetIsProcessing(FALSE);
}

void CPartyManager::Party_MemberLife( void* pMsg )
{
	const MSG_DWORD2* const message = (MSG_DWORD2*)pMsg;
	const DWORD playerIndex = message->dwData1;
	const float value = float(message->dwData2);
	CPartyDialog* const partyDialog = GAMEIN->GetPartyDialog();

	if(0 == partyDialog)
	{
		return;
	}

	for(BYTE i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		CPartyMemberDlg* const memberDialog = partyDialog->GetMemberDlg(i);

		if(0 == memberDialog)
		{
			continue;
		}
		else if(memberDialog->GetPlayerIndex() != playerIndex)
		{
			continue;
		}

		memberDialog->SetLife(value);
		break;
	}
}


void CPartyManager::Party_MemberMana( void* pMsg )
{
	const MSG_DWORD2* const message = (MSG_DWORD2*)pMsg;
	const DWORD playerIndex = message->dwData1;
	const float value = float(message->dwData2);
	CPartyDialog* const partyDialog = GAMEIN->GetPartyDialog();

	if(0 == partyDialog)
	{
		return;
	}

	for(BYTE i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		CPartyMemberDlg* const memberDialog = partyDialog->GetMemberDlg(i);

		if(0 == memberDialog)
		{
			continue;
		}
		else if(memberDialog->GetPlayerIndex() != playerIndex)
		{
			continue;
		}

		memberDialog->SetMana(value);
		break;
	}
}

void CPartyManager::SetMemberLevel(DWORD playerIndex, LEVELTYPE level)
{
	CPartyDialog* const partyDialog = GAMEIN->GetPartyDialog();

	if(0 == partyDialog)
	{
		return;
	}

	for(BYTE i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		CPartyMemberDlg* const memberDialog = partyDialog->GetMemberDlg(i);

		if(0 == memberDialog)
		{
			continue;
		}
		else if(memberDialog->GetPlayerIndex() != playerIndex)
		{
			continue;
		}

		memberDialog->SetLevel(level);
		break;
	}
}


void CPartyManager::Party_SendPos( void* pMsg )
{
	SEND_PARTYICON_MOVEINFO* pmsg = (SEND_PARTYICON_MOVEINFO*)pMsg;
	if(pmsg->MoveInfo.dwMoverID != gHeroID)
		PICONMGR->SetMoveInfo(&pmsg->MoveInfo);
}


void CPartyManager::Party_RevivePos( void* pMsg )
{
	SEND_PARTYICON_REVIVE* pmsg = (SEND_PARTYICON_REVIVE*)pMsg;
	PICONMGR->SetCurPosition(pmsg->dwMoverID, pmsg->Pos);
}


void CPartyManager::Party_Invite_Accept_Nack( void* pMsg )
{
	MSG_BYTE * pmsg = (MSG_BYTE*)pMsg;
	
	if(pmsg->bData == eErr_BrokenParty)
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(104) );
	else if(pmsg->bData == eErr_Add_Full)
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(110) );
}


void CPartyManager::Party_Member_LoginMsg( void* pMsg ) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	
	if(pmsg->dwData != HEROID)
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(403), m_Party.GetMemberName(pmsg->dwData));
}


void CPartyManager::Party_Monster_Obtain_Notify( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(pmsg->dwData2);
	if(pItemInfo == 0)
		return;
	CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 100 ), GetPartyMemberName(pmsg->dwData1), pItemInfo->ItemName );
}

void CPartyManager::Party_Monster_ObtainByDice_Notify( void* pMsg ) 
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(pmsg->dwData2);
	if(pItemInfo == 0)
		return;
	CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 1995 ), GetPartyMemberName(pmsg->dwData1), pItemInfo->ItemName, pmsg->dwData3 );
}

void CPartyManager::Party_Change_PickupItem_Ack( void* pMsg )
{
	if( !pMsg ) return ;

	char tempStr[128] = {0,} ;

	MSG_BYTE* pmsg = ( MSG_BYTE* )pMsg ;

	CPartyBtnDlg* pPartySet = NULL ;
	pPartySet = (CPartyBtnDlg*)WINDOWMGR->GetWindowForID( PA_BTNDLG );

	if( !pPartySet ) return ;

	switch( pmsg->bData )
	{
	case ePartyOpt_Damage :
		{
			sprintf( tempStr, CHATMGR->GetChatMsg(1115) ) ;
			m_PartyOption = ePartyOpt_Damage ;
		}
		break;
	case ePartyOpt_Sequence :
		{
			sprintf( tempStr, CHATMGR->GetChatMsg(1114) ) ;
			m_PartyOption = ePartyOpt_Sequence ;
		}
		break ;
	}

	pPartySet->SetDistribute(pmsg->bData);

	CHATMGR->AddMsg( CTC_PARTYCHAT, tempStr );
}

// 071002 LYW --- PartyManager : Add function to parsing message to nack change option.
void CPartyManager::Party_Change_PickupItem_Nack( void* pMsg )
{
	CHATMGR->AddMsg( CTC_PARTYCHAT, CHATMGR->GetChatMsg( 1297 ) );

	CPartyBtnDlg* pPartySet = NULL ;
	pPartySet = (CPartyBtnDlg*)WINDOWMGR->GetWindowForID( PA_BTNDLG );

	if( !pPartySet ) return ;

	pPartySet->SetDistribute(m_PartyOption);
}

void CPartyManager::Party_Change_DiceGrade_Ack( void* pMsg )
{
	MSG_BYTE* pmsg = ( MSG_BYTE* )pMsg ;

	CPartyBtnDlg* pPartySet = (CPartyBtnDlg*)WINDOWMGR->GetWindowForID( PA_BTNDLG );

	if( !pPartySet ) return ;

	m_DiceGrade = pmsg->bData;
	pPartySet->SetDiceGrade(pmsg->bData);

	switch(m_DiceGrade)
	{
	case 0:
		CHATMGR->AddMsg( CTC_PARTYCHAT, CHATMGR->GetChatMsg( 1978 ), RESRCMGR->GetMsg(673));
		break;
	case 1:
		CHATMGR->AddMsg( CTC_PARTYCHAT, CHATMGR->GetChatMsg( 1978 ), RESRCMGR->GetMsg(1182));
		break;
	case 2:
		CHATMGR->AddMsg( CTC_PARTYCHAT, CHATMGR->GetChatMsg( 1978 ), RESRCMGR->GetMsg(1183));
		break;
	case 3:
		CHATMGR->AddMsg( CTC_PARTYCHAT, CHATMGR->GetChatMsg( 1978 ), RESRCMGR->GetMsg(1184));
		break;
	case 4:
		CHATMGR->AddMsg( CTC_PARTYCHAT, CHATMGR->GetChatMsg( 1978 ), RESRCMGR->GetMsg(1185));
		break;
	}
}

void CPartyManager::Party_Change_DiceGrade_Nack( void* pMsg )
{
	CHATMGR->AddMsg( CTC_PARTYCHAT, CHATMGR->GetChatMsg( 1979 ) );

	CPartyBtnDlg* pPartySet = (CPartyBtnDlg*)WINDOWMGR->GetWindowForID( PA_BTNDLG );

	if( !pPartySet ) return ;

	pPartySet->SetDistribute(m_PartyOption);
}

// 090528 ShinJS --- 이름으로 파티초대시 에러처리
void CPartyManager::Party_InviteByName_Nack( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	
	switch(pmsg->dwData)
	{
	case eErr_Add_NoPlayer:
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(405)); break;
	case eErr_Add_AlreadyinParty:
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(107)); break;	
	case eErr_Add_NotConnectedPlayer:
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1920)); break;
	case eErr_AlreadySearchingByName:
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1921)); break;

	case eErr_SettingLimitTime:
		// 파티초대 Error 누적초과에 대한 제한시간 설정
		MSG_DWORD2* pLimitTimeMsg = (MSG_DWORD2*)pmsg;
		const DWORD dwLimitTime = pLimitTimeMsg->dwData2;

		// 작업제한시간 설정
		HERO->SetInvitePartyByNameLastTime( gCurTime );
		HERO->SetInvitePartyByNameLimitTime( dwLimitTime );
		break;
	}
}

const PARTY_MEMBER& CPartyManager::GetMember(LPCTSTR name)
{
	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		const PARTY_MEMBER* const member = m_Party.GetPartyMemberInfo(i);

		if(0 == member)
		{
			continue;
		}
		else if(_tcsicmp(member->Name, name))
		{
			continue;
		}

		return *member;
	}

	static const PARTY_MEMBER emptyMember = {0};
	return emptyMember;
}
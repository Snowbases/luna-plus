// PartyManager.h: interface for the CPartyManager class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _CPARTYMANAGER_H_
#define _CPARTYMANAGER_H_
#pragma once

#include "Party.h"

#define PARTYMGR USINGTON(CPartyManager)

class CPartyManager 
{
	CParty m_Party;
	void SetPartyInfo(PARTY_INFO* pmsg);
	DWORD m_RequestPartyID;
	BOOL m_bIsProcessing;

	int m_PartyOption ;
	int m_DiceGrade;

public:
	CPartyManager();
	virtual ~CPartyManager();

	void Init();
	void NetworkMsgParse(BYTE Protocol, LPVOID);
	void SetPickupItem(int option);
	int GetPickupItem() const { return m_PartyOption; }
	void SetDiceGrade(int option);
	int GetDiceGrade() const {return m_DiceGrade;}
	void AddPartyMemberSyn(DWORD AddPartymemeberID);
	void InvitePartyMemberByNameSyn( char* szCharName );						// 090525 ShinJS --- 캐릭터이름으로 파티초대하기
	void DelPartyMemberSyn();
	void BanPartyMemberSyn(DWORD DelPartymemberID);
	void ChangeMasterPartySyn(DWORD FromMemberID, DWORD ToMemberID);
	void BreakUpSyn();
	void PartyInviteAccept();
	void PartyInviteDeny();
	void SetRequestPartyID(DWORD id) { m_RequestPartyID = id; }
	DWORD GetRequestPartyID() const { return m_RequestPartyID; }
	void CreatePartyResult(MSG_DWORDBYTE2* pMsg);
	void AddPartyMemberResult(SEND_PARTY_MEMBER_INFO* pMsg); 
	void DelPartyMemberResult(MSG_DWORD* pMsg);
	void BanPartyMemberResult(MSG_DWORD* pMsg);
	void ChangeMasterPartyResult(MSG_DWORD* pMsg);
	void BreakUpPartyResult();
	void UserLogIn(SEND_PARTY_MEMBER_INFO*);
	void UserLogOut(MSG_DWORD*);
	PARTY_MEMBER* GetPartyMemberInfo(int i);
	const PARTY_MEMBER& GetMember(LPCTSTR name);
	DWORD GetMasterID() { return m_Party.GetMasterIdx(); }
	char* GetPartyMemberName(DWORD PlayerID) { return m_Party.GetMemberName(PlayerID); }
	DWORD GetPartyMemberID(int n) { return m_Party.GetMemberID(n); }
	BOOL IsPartyMember(DWORD PlayerID);
	BOOL PartyChat(char* ChatMsg, char* SenderName);
	BOOL CanActivate();
	
	void SetIsProcessing(BOOL val) { m_bIsProcessing = val;	}
	BOOL IsProcessing() const { return m_bIsProcessing;	}
	CParty& GetParty() { return m_Party; }
	void SetMemberLevel(DWORD playerIndex, LEVELTYPE);

private:
	BOOL m_MasterChanging;

private:
	void SetMasterChanging(BOOL val) { m_MasterChanging = val;	}
	BOOL IsMasterChanging();
	void Party_Info(LPVOID);
	void Party_Add_Invite(LPVOID);
	void Party_Add_Ack(LPVOID);
	void Party_Add_Nack(LPVOID);
	void Party_Del_Ack(LPVOID);
	void Party_Del_Nack(LPVOID);
	void Party_Create_Ack(LPVOID);
	void Party_Create_Nack(LPVOID);
	void Party_Ban_Ack(LPVOID);
	void Party_Member_Login(LPVOID);
	void Party_Member_Logout(LPVOID);
	void Party_ChangeMaster_Ack(LPVOID);
	void Party_Breakup_Ack(LPVOID);
	void Party_Breaup_Nack(LPVOID);
	void Party_MemberLife(LPVOID);
	void Party_MemberMana(LPVOID);
	void Party_SendPos(LPVOID);
	void Party_RevivePos(LPVOID);
	void Party_Invite_Accept_Nack(LPVOID);
	void Party_Member_LoginMsg(LPVOID);
	void Party_Monster_Obtain_Notify(LPVOID);
	void Party_Monster_ObtainByDice_Notify(LPVOID);
	void Party_Change_PickupItem_Ack(LPVOID);
	void Party_Change_PickupItem_Nack(LPVOID);
	void Party_Change_DiceGrade_Ack(LPVOID);
	void Party_Change_DiceGrade_Nack(LPVOID);
	void Party_InviteByName_Nack(LPVOID);
};
EXTERNGLOBALTON(CPartyManager);

#endif

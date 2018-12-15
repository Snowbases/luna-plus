#pragma once 
#define PARTYMGR CPartyManager::GetInstance()

class CPlayer;
class CParty;
struct PARTYMEMBER;


class CPartyManager 
{
	// 090923 ONS 메모리풀 교체
	CPool< CParty >* m_partyRoomPool;
	CYHHashTable<CParty> m_partyRoomHashTable;	
	CYHHashTable<CParty> m_tempPartyRoomHashTable;	

	CPartyManager();
	void ChangeMasterRandomly(CPlayer&);
	void ChangeMasterOneself(CPlayer&);

public:
	GETINSTANCE(CPartyManager);

	virtual ~CPartyManager();

	CParty* GetParty(DWORD partyIDx); 
	
	CParty* RegistParty(DWORD PartyIDX);
	void CreatePartyQuery(CPlayer * pMaster, BYTE bOption, BYTE bSubOption);
	void CreatePartyResult(CPlayer * pMaster, DWORD PartyIDX, BYTE Option, BYTE SubOption);
	void CreateParty(CPlayer* pMaster, BYTE bOption, BYTE bSubOption) ;
	void AddMember(DWORD PartyID, DWORD TargetPlayerID);
	void AddMemberResult(CParty*, CPlayer* pNewMember);
	void DelMember(DWORD PlayerID,DWORD PartyID);
	void DelMemberSyn(DWORD PlayerID, DWORD PartyID);
	void RemoveParty(DWORD DeletedPlayerID, DWORD PartyID);
	void BanParty(DWORD PartyID,DWORD PlayerID,DWORD TargetPlayerID);
	void ChangeMasterParty(DWORD PartyID,DWORD PlayerID,DWORD TargetPlayerID);
	void DoChangeMasterParty(CParty* pParty, DWORD TargetID);
	void BreakupParty(DWORD PartyID,DWORD PlayerID);

	void UserLogIn(CPlayer* pPlayer,BOOL bNotifyUserLogin);
	void UserLogOut(CPlayer* pPlayer);

	void AddPartyInvite(DWORD MasterID, DWORD TargetID);
	void NotifyChangesOtherMapServer(DWORD TargetPlayerID,CParty* pParty, BYTE Protocol, DWORD PartyIDX, LEVELTYPE Level=0);
	void NotifyCreateParty(MSG_PARTY_CREATE* pmsg);							// 090526 ShinJS --- Master를 등록할수 있도록 수정
	void NotifyAddParty(SEND_PARTY_MEMBER_INFO*);
	void NotifyBanParty(SEND_PARTY_MEMBER_INFO*);
	void NotifyChangeMasterParty(SEND_PARTY_MEMBER_INFO*);
	void NotifyBreakupParty(SEND_PARTY_MEMBER_INFO*);
	void NotifyDelParty(SEND_PARTY_MEMBER_INFO*);
	void NotifyDelSyn(MSG_DWORD2*);
	void NotifyUserLogIn(SEND_PARTY_MEMBER_INFO*);
	void NotifyUserLogOut(SEND_PARTY_MEMBER_INFO*);
	void NotifyMemberLoginMsg(DWORD PartyID, DWORD PlayerID);
	
	void SendErrMsg(DWORD PlayerID, int ErrKind, BYTE Protocol);
	void SendOptionMsg(MSG_DWORD2*);
	void MemberLevelUp(DWORD PartyIDX, DWORD PlayerIDX, LEVELTYPE);
	void DoMemberLevelUp(DWORD PartyIDX, DWORD PlayerIDX, LEVELTYPE);
	void SendObtainItemMsg(CPlayer*, DWORD ObtainIdx);
	void SendObtainItemByDiceMsg(CPlayer*, DWORD ObtainIdx, DWORD Num);
	DWORD GetRandomMember(DWORD PartyID);

	// 인스턴트파티 (DB에 기록을 하지않는 로컬맵 전용파티)
	struct stInstantPartyReservation
	{
		DWORD dwPlayerID;
		DWORD dwMasterID;
	};
	CIndexGenerator							m_InstantPartyIDXGenerator;
	CPool<stInstantPartyReservation>*		m_InstantPartyReservationPool;
	CYHHashTable<stInstantPartyReservation> m_InstantPartyReservationList;

	BOOL CanUseInstantPartyMap(MAPTYPE mapNum);
	DWORD CreateInstantParty(DWORD dwMasterIdx);
	DWORD GenerateInstantPartyIdx();
	void ReleaseInstantPartyIdx(DWORD dwInstantPartyIdx);

	void AddReservationInstantPartyMember(DWORD dwPlayerIndex, DWORD dwMasterIndex);
	DWORD AutoJoinToInstantParty(DWORD dwPlayerIndex, DWORD dwMasterIndex);
	CParty* GetPartyFromMasterID(DWORD dwMasterID);

	void ProcessReservationList(DWORD dwPlayerIndex);
	void BreakupInstantParty(DWORD PartyID,DWORD PlayerID);
	void DelMemberInstantParty(DWORD PlayerID,DWORD PartyID);
};


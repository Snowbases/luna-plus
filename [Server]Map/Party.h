// Party.h: interface for the CParty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTY_H__87E3B123_4418_45C7_B287_81730C428C79__INCLUDED_)
#define AFX_PARTY_H__87E3B123_4418_45C7_B287_81730C428C79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPlayer;

class CParty
{	
	//DB에 저장될 인덱스 값
	DWORD m_PartyIDx;
	PARTY_MEMBER m_Member[MAX_PARTY_LISTNUM];
	BYTE m_Option;
	BYTE m_DiceGrade;
	void Init(DWORD id, DWORD MasterID, char* strMasterName, CPlayer*, BYTE option, BYTE DiceGrade, LEVELTYPE = 0);
	void SendPartyInfo(CPlayer*);
	BOOL m_MasterChanging;
	BYTE m_byWinnerIdx;
	DWORD m_dwPartyMemberNum;

public:
	CParty();
	virtual ~CParty();
	void AddPartyMember(DWORD AddMemberID, LPCTSTR Name,CPlayer*, LEVELTYPE);
	void RemovePartyMember(DWORD MemberID);
	BOOL ChangeMaster(DWORD FromID,DWORD ToID);
	BOOL IsPartyMember(DWORD PlayerID);
	BOOL IsMemberLogIn(int n);
	void SetWinnerIdx(BYTE idx) { m_byWinnerIdx = idx; }
	BYTE GetWinnerIdx() const { return m_byWinnerIdx; }
	void SetMaster(CPlayer* pMaster);
	void SetOption(BYTE Option);
	BYTE GetOption();
	void SetDiceGradeOpt(BYTE Grade);
	BYTE GetDiceGradeOpt();
	void UserLogIn(CPlayer* pPlayer,BOOL bNotifyUserLogin);
	void UserLogOut(DWORD PlayerID);
	void SendMsgUserLogOut(DWORD PlayerID);
	
	void SendMsgLoginMemberInfo(SEND_PARTY_MEMBER_INFO*, DWORD GridID);

	void SendMsgToAll(MSGBASE* msg, int size);
	void SendMsgExceptOne(MSGBASE* msg, int size, DWORD PlayerID);
	void SendMsgToMember(MSG_DWORD2*);
	DWORD GetPartyIdx() const { return m_PartyIDx;	}
	DWORD GetMemberID(int n) const { return m_Member[n].dwMemberID; }
	DWORD GetMasterID()	const { return m_Member[0].dwMemberID; }
	char* GetMemberName(DWORD MemberID);
	void BreakUp();
	// 091127 LUJ, 최신 정보로 갱신한다
	const PARTY_MEMBER& GetMember(DWORD memberIndex);
	void NotifyUserLogIn(SEND_PARTY_MEMBER_INFO*);
	void SendMemberLogInMsg(DWORD PlayerID);
	void NotifyMemberLogin(DWORD PlayerID);
	void SendPlayerInfoToOtherMembers(int n, DWORD GridID);
	int SetMemberInfo(DWORD MemberID, char* strName, CPlayer* pPlayer, LEVELTYPE lvl, BOOL bLog, MAPTYPE);
	void SetMember(DWORD index, const PARTY_MEMBER&);
	// 090529 ShinJS --- 정보가 없는 경우에도 멤버 정보를 갱신할수 있도록 함수추가
	void SetMemberInfo( BYTE nPos, DWORD MemberID, char* strName, LEVELTYPE lvl, MAPTYPE map);
	void SetMasterChanging(BOOL val);
	BOOL IsMasterChanging(CPlayer* pPlayer, BYTE Protocol);
	void SendMsgExceptOneinChannel(MSGBASE* msg, int size, DWORD PlayerID, DWORD GridID);

	friend class CPartyManager;
	
	//level
	void SetMemberLevel(DWORD PlayerID, LEVELTYPE lvl);
	void SendMemberPos(DWORD MoverID, BASEMOVE_INFO* pMoveInfo);
	void NotifyChangedOption(DWORD dwOption) ;
	void SetPartyMemberNum(DWORD size) { m_dwPartyMemberNum = size; }
	DWORD GetPartyMemberNum() const { return m_dwPartyMemberNum; }
	DWORD GetRandomMember();
	void SetMasterTipInfo( MAPTYPE MapType, BYTE JobGrade, BYTE* Job, BYTE Race );
};

#endif // !defined(AFX_PARTY_H__87E3B123_4418_45C7_B287_81730C428C79__INCLUDED_)

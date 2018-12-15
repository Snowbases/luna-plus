#pragma once

#include "GuildWarehouse.h"

enum RTGChangeRank
{
	eGRankPos_1,			//@부문주
	eGRankPos_2,			//@장로1
	eGRankPos_3,			//@장로2
	eGRankPos_Max,
	eGRankPos_Err,
};

class CGuild
{
	GUILDINFO m_GuildInfo;
	cPtrList m_MemberList;
	MARKNAMETYPE m_MarkName;
	
	DWORD m_RankMemberIdx[eGRankPos_Max]; // m_RankMemberIdx[0] : vice master, m_RankMemberIdx[1],m_RankMemberIdx[2] : seniors  
	CGuildWarehouse m_GuildWare;
	DWORD m_GTBattleID;
	DWORD m_nStudentCount;
	DWORD mPlayerInWarehouse;

public:
	CGuild( const GUILDINFO& pInfo, MONEYTYPE GuildMoney);
	virtual ~CGuild();

	DWORD GetIdx() const { return m_GuildInfo.GuildIdx; }
	void InitGuildItem(const ITEMBASE&);
	BOOL IsMember(DWORD MemberIDX);
	BOOL IsViceMaster(DWORD PlayerIDX);
	
	BOOL AddMember( GUILDMEMBERINFO* );
	BOOL DeleteMember(DWORD PlayerIDX, BYTE bType);
	char* GetGuildName() { return m_GuildInfo.GuildName; }
	
	void BreakUp();
	const GUILDINFO& GetInfo() const { return m_GuildInfo; }
	GUILDINFO& GetInfo() { return m_GuildInfo; }

	void SendMsgToAll(MSGBASE* msg, int size);
	void SendMsgToAllExceptOne(MSGBASE* msg, int size, DWORD CharacterIdx);
	void SendDeleteMsg(DWORD PlayerIDX, BYTE bType);
	
	void SendNote(CPlayer *pPlayer, char* toName, char* note);
	void SendAllNote(CPlayer *pPlayer, char* note);
	void SendUnionNote(CPlayer *pPlayer, char* note);
	
	MARKNAMETYPE GetMarkName() const { return m_MarkName; }
	void SetMarkName(MARKNAMETYPE type) { m_MarkName = type; }
	DWORD GetMasterIdx() const { return m_GuildInfo.MasterIdx; }
	void SetMasterIndex(DWORD masterIndex) { m_GuildInfo.MasterIdx = masterIndex; }
	LPCTSTR GetMasterName() const { return m_GuildInfo.MasterName; }
	int GetMemberNum() { return m_MemberList.GetCount(); }
	void GetTotalMember(GUILDMEMBERINFO*);
	LEVELTYPE GetLevel() const { return m_GuildInfo.GuildLevel; }
	GUILDMEMBERINFO* GetMemberInfo(DWORD MemberIdx);
	int IsVacancy(BYTE ToRank);
	
	BOOL CanAddMember() const;
	
	BOOL ChangeMemberRank(DWORD MemberIdx, BYTE ToRank);
	
	void ResetRankMemberInfo(DWORD MemberIdx, BYTE Rank);
	CItemSlot* GetSlot();
	void SetMemberLevel(DWORD PlayerIdx, LEVELTYPE PlayerLvl);
	void SetLogInfo(DWORD PlayerIdx, BOOL vals, MAPTYPE);
	BOOL SetMember(const GUILDMEMBERINFO&);
	void SetLevel(LEVELTYPE level) { m_GuildInfo.GuildLevel = level; }
	void SendGTRewardToAll(DWORD gtcount, DWORD itemIdx, DWORD count, BOOL isStack, DWORD endtime);

	void SetGTBattleID( DWORD BattleID )			{	m_GTBattleID = BattleID;	}
	DWORD GetGTBattleID()							{	return m_GTBattleID;		}

	void	SetGuildUnionInfo( DWORD dwGuildUnionIdx, char* pGuildUnionName, DWORD dwGuildUnionMarkIdx, BOOL isMaster );
	void	SetGuildUnionIdx( DWORD dwGuildUnionIdx )	{ m_GuildInfo.UnionIdx = dwGuildUnionIdx; }
	void	SetUnionMarkIndex( DWORD dwMarkIdx );
	DWORD	GetUnionIndex()							{ return m_GuildInfo.UnionIdx; }
	BOOL	CanAddStudent() const;
	void	SetZeroMoney();
	void	ClearItem();
	void	Update( const GUILDINFO& info ) { m_GuildInfo = info; }

	// 080417 LUJ, 길드 스킬 관리
private:
	// 080417 LUJ, 보유한 스킬을 담는다. 키: 스킬 인덱스, 값: 스킬 레벨
	typedef stdext::hash_map< DWORD, LEVELTYPE > SkillMap;
	SkillMap mSkillMap;

public:
	void AddSkill(DWORD index, LEVELTYPE);
	void RemoveSkill(DWORD index);
	inline void RemoveAllSkill() { mSkillMap.clear(); }
	void SendSkill(CObject&) const;
	LEVELTYPE GetSkillLevel(DWORD index) const;
	inline DWORD GetSkillSize() const { return mSkillMap.size(); }
	void UpdateSkill(DWORD playerIndex);
	void SetPlayerInWarehouse(DWORD playerIndex) { mPlayerInWarehouse = playerIndex; }
	DWORD GetPlayerInWarehouse() const { return mPlayerInWarehouse; }
};

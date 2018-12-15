#pragma once

enum RTChangeRank
{
	eRankPos_1,
	eRankPos_2,
	eRankPos_3,
	eRankPos_Max,
	eRankPos_Err,
};

class CGuild  
{
	DWORD m_GuildIdx;
	LEVELTYPE m_GuildLvl;
	char m_MasterName[MAX_NAME_LENGTH+1];
	char m_GuildName[MAX_GUILD_NAME+1];
	//char m_GuildNotice[MAX_GUILD_NOTICE+1];
	DWORD m_UnionIdx;

	DWORD m_RankMemberIdx[eRankPos_Max];
	
	cPtrList m_MemberList;

	typedef BYTE RANK;
	std::multimap< RANK, GUILDMEMBERINFO* > m_MemberMultiMap;
	DWORD	m_nStudentCount;	//문하생 숫자 카운터

public:
	CGuild();
	virtual ~CGuild();
	
	void Init(GUILDINFO* pInfo);
	void Release();

	BOOL DeleteMember(DWORD PlayerIDX);
	BOOL AddMember(GUILDMEMBERINFO* pMemberInfo);
	BOOL IsMember(DWORD PlayerIDX);
	void SetTotalInfo(GUILDINFO* pInfo, GUILDMEMBERINFO* pMemberInfo, int MemberNum);
	
	DWORD GetGuildIdx()	const	{ return m_GuildIdx; }
	char* GetGuildName() 		{ return m_GuildName; }
	LEVELTYPE GetLevel() const	{ return m_GuildLvl; }
	char* GetMasterName()		{ return m_MasterName; }
	BYTE GetMemberSize()			{ return static_cast<BYTE>(m_MemberList.GetCount()); }
	cPtrList& GetMemberList()	{ return m_MemberList; }
	GUILDMEMBERINFO* GetMemberInfo(DWORD PlayerIDX);
	char* GetMemberName(DWORD MemberID);
	DWORD GetMemberIdx(int num);
	void SetLevel(LEVELTYPE level) { m_GuildLvl = level; }
	
	int IsVacancy(BYTE ToRank);
	BOOL ChangeRank(DWORD MemberID, BYTE Rank);
	void ResetRankMemberInfo(DWORD MemberIdx, BYTE Rank);
	void SetMemberLevel(DWORD MemberIdx, DWORD MemberLvl);	
	void SetLogInfo(DWORD MemberIdx, BOOL vals);
	DWORD GetStudentNum() { return m_nStudentCount; }
	BOOL CanAddStudent();
};
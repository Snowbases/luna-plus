#pragma once

enum EObjectKind;

class CUserTable : public CYHHashTable<CObject>
{
public:
	struct Team
	{
		typedef DWORD ObjectIndex;
		ObjectIndex mLeaderObjectIndex;
		typedef std::set< ObjectIndex > ObjectIndexContainer;
		ObjectIndexContainer mMemberContainer;

		Team() :
		mLeaderObjectIndex(0)
		{}
	};

private:
	DWORD m_dwUserCount;
	DWORD m_dwMonsterCount;
	DWORD m_dwNpcCount;
	DWORD m_dwExtraCount;
	// 채널별 유저 인덱스 관리
	typedef std::set<DWORD>					UserIndexSet;
	typedef std::map<DWORD, UserIndexSet>	UserIndexListInChannel;		// 채널별로 분리된 유저인덱스를 복사해둠. 트리거에서 사용.
	UserIndexSet							m_SearchUserIndexSet;
	UserIndexSet::iterator					m_SearchUserIndexIterator;
	UserIndexListInChannel					m_UserIndexListInChannel;

	typedef DWORD ObjectIndex;
	typedef std::string AliasName;
	typedef std::map< AliasName, ObjectIndex > AliasContainer;
	AliasContainer mAliasContainer;

	typedef std::string TeamName;
	typedef std::map< TeamName, Team > TeamContainer;
	TeamContainer mTeamContainer;

public:
	CUserTable();
	virtual ~CUserTable();

	void Init(DWORD dwBucket);
	CObject * FindUser(DWORD dwKey);
	CObject * FindUserForName(LPCTSTR);
	BOOL AddUser(CObject* pObject,DWORD dwKey);
	CObject * RemoveUser(DWORD dwKey);
	void SetObjectIndex(LPCTSTR, ObjectIndex);
	ObjectIndex GetObjectIndex(LPCTSTR) const;
	void GetTargetInRange(VECTOR3* pPivotPos,float Radius,CTargetList* pTList,DWORD GridID,float SafeRange = 0);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	typedef std::set< EObjectKind > ObjectKindSet;
	// 080708 LUJ, 특정 오브젝트 종류를 타겟으로 잡는다
	void GetTargetInRange(VECTOR3* pPivotPos,float Radius,CTargetList* pTList,DWORD GridID,float SafeRange, const ObjectKindSet& );
	DWORD GetUserCount() const { return m_dwUserCount; }
	DWORD GetMonsterCount() const { return m_dwMonsterCount; }
	DWORD GetNpcCount() const { return m_dwNpcCount; }
	DWORD GetExtraCount() const { return m_dwExtraCount; }
	void SetPositionUserHead() {  CYHHashTable<CObject>::SetPositionHead(); }
	CObject * GetUserData() { return CYHHashTable<CObject>::GetData(); }
	// 091113 NYJ, 인스턴스던전과 같이 채널별로 오브젝트개체수가 많은경우
	// 			SetPositionUserHead() & GetUserData() 대신 사용하기 위함
	void SetPositionUserHeadChannel(DWORD dwChannel);
	CObject * GetUserDataChannel(DWORD dwChannel);
	const Team& GetTeam(LPCTSTR) const;
	void AddTeamMember(LPCTSTR, ObjectIndex);
	void SetTeamLeader(LPCTSTR, ObjectIndex);
};

extern CUserTable * g_pUserTable;
// Battle.h: interface for the CBattle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BATTLE_H__5D88F50D_0E8C_46C3_86EA_B0F0D5CE11E0__INCLUDED_)
#define AFX_BATTLE_H__5D88F50D_0E8C_46C3_86EA_B0F0D5CE11E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPlayer;
class CMonster;
class CBattleTeam;

#define ToEachTeam(ValuableName)	{	\
	CBattleTeam* ValuableName;			\
	for(int TeamNumber=0;TeamNumber<eBattleTeam_Max;++TeamNumber)	\
	{									\
		ValuableName = m_TeamArray[TeamNumber];	
		
#define EndToEachTeam	}}

enum
{
	eBattleTeam1 = 0,
	eBattleTeam2,
	eBattleTeam_Max,
};

enum
{
	eBATTLE_STATE_READY = 0,
	eBATTLE_STATE_FIGHT,
	eBATTLE_STATE_RESULT,
};

#define BATTLE_FLAG_NONE			0x00000000
#define BATTLE_FLAG_LOGINBATTLE		0x00000001

class CBattle  
{
protected:
	BATTLE_INFO_BASE m_BattleInfo;
	union{
		struct{
			CBattleTeam* m_Team1;
			CBattleTeam* m_Team2;
			//CBattleTeam* m_TeamObserver;	<-- 여기 왜 있지?
		};
		CBattleTeam* m_TeamArray[eBattleTeam_Max];
	};

	CBattleTeam* m_TeamObserver; //Union에서 따로 뺐다 KES

	DWORD m_BattleFlag;
	int m_WinnerTeam;
	
	BOOL m_bDestroyFlag;

public:
	CBattle();
	virtual ~CBattle();

	inline BYTE  GetBattleKind()	{ return m_BattleInfo.BattleKind; }
	inline DWORD GetBattleID()		{ return m_BattleInfo.BattleID;	}
	inline DWORD GetBattleFlag()	{ return m_BattleFlag; }
	inline BYTE  GetBattleState()	{ return m_BattleInfo.BattleState; }

	virtual void Initialize(BATTLE_INFO_BASE* pCreateInfo, CBattleTeam* pTeam1, CBattleTeam* pTeam2);
	virtual void Release();
	
#ifdef _MAPSERVER_
	void SendBattleInfo(CPlayer* pPlayer);

	virtual BOOL AddObjectToBattle(CObject* pObject);
	virtual BOOL DeleteObjectFromBattle(CObject* pObject);
	BOOL IsAddableBattle(CObject* pObject);
#endif

	virtual void StartBattle();
	void SetDestroyFlag()		{	m_bDestroyFlag = TRUE;	}

	BOOL IsDestroyed()	{	return m_bDestroyFlag;	}
	
	//////////////////////////////////////////////////////////////////////////
	// { start virtual func 반드시 오버라이딩 해야함

	// Battle 정보 관련
#ifdef _MAPSERVER_
	virtual void GetBattleInfo(char* pInfo,WORD* size);
	virtual DWORD GetTeamMemberNum(int i);
#endif

	// 적,아군 구별
	virtual BOOL IsEnemy(CObject* pOperator,CObject* pTarget);
 	//virtual BOOL IsFriend(CObject* pOperator,CObject* pTarget);

	// 080807 LYW --- Battle : 공성전 중일 때, 적/아군을 구별하는 함수.
	BYTE IsEnemy_NormalTimes(CObject* pOperator,CObject* pTarget) ;			// 공성전 중이 아닌 평상시에 적/아군을 판단 하는 함수.
	BYTE IsEnemy_SiegeWarning(CObject* pOperator,CObject* pTarget) ;
	
	// event func
	virtual void OnCreate(BATTLE_INFO_BASE* pCreateInfo, CBattleTeam* pTeam1, CBattleTeam* pTeam2);
	virtual void OnFightStart();
	virtual void OnDestroy();
	virtual void OnTeamMemberAdd(int Team,DWORD MemberID,char* Name) {}
	virtual BOOL OnTeamMemberDelete(int Team,DWORD MemberID,char* Name) { return FALSE; }
	virtual BOOL OnTeamMemberDie(int Team,DWORD VictimMemberID,DWORD KillerID) { return FALSE; }
	virtual void OnTeamMemberRevive(int Team,CObject* pReviver) {}
	virtual void OnMonsterDistribute(CMonster*,CObject* pKiller);
	virtual void OnTick() {}
	virtual BOOL Judge() { return FALSE; }
	virtual void Victory(int WinnerTeamNum,int LoserTeamNum);
	virtual void Draw();

	// Guild Tournament
	virtual DWORD GetBattleTeamID( CObject* pObject );			// 1 = Battleteam1, 2 = Battleteam2
	virtual VECTOR3* GetRevivePoint(DWORD dwTeam) { return NULL;}
#ifdef _CLIENT_
	virtual void Render() {}
#endif
};

#endif // !defined(AFX_BATTLE_H__5D88F50D_0E8C_46C3_86EA_B0F0D5CE11E0__INCLUDED_)


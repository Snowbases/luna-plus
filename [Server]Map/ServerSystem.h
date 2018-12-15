// ServerSystem.h: interface for the CServerSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_)
#define AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MHMap.h"
//#include "PtrList.h"
class CObject;
class CGridSystem;

class CPlayer;
class CMonster;
class CNpc;
class cSkillObject;
class CBossMonster;
class CMapObject;
class CPet;
// 필드보스 - 05.12 이영준
class CFieldBossMonster;
class CFieldSubMonster;

enum eNATION
{
	eNATION_KOREA,
	eNATION_CHINA,
};

enum{ePET_FROM_DB, ePET_FROM_ITEM};		//DB로부터 펫생성, 소환아이템 첫사용으로 팻생성
enum EObjectKind;

class CServerSystem  
{
//	CYHHashTable<CItemObject> m_ItemObjectTable;
	CGridSystem* m_pGridSystem;
	CMHMap m_Map;
	
	friend class CCharMove;
	BOOL m_start;
	WORD 	m_wMapNum;
	eNATION	m_Nation;
	BOOL 	m_bTestServer;
	DWORD	m_dwQuestTime;
	BOOL	m_bQuestTime;

	BOOL	m_bCompletionChrUpdate;
//--check process time
	DWORD	m_dwMainProcessTime;

	// 프로세스타임 체크용
	BOOL			m_bCheckProcessTime;
	LARGE_INTEGER	m_freq;
	LARGE_INTEGER	m_ObjLoopProc[2];
	LARGE_INTEGER	m_TriggerProc[2];
	LARGE_INTEGER	m_DungeonProc[2];
	double			m_fTimeObjLoopProc;
	double			m_fTimeTriggerProc;
	double			m_fTimeDungeonProc;
	double			m_fAvrObjLoopProc;
	double			m_fAvrTriggerProc;
	double			m_fAvrDungeonProc;
	DWORD			m_dwProcessCount;

public:
	void ToggleCheckProcessTime();
	DWORD GetMainProcessTime() { return m_dwMainProcessTime; }

public:

	CServerSystem();
	virtual ~CServerSystem();
	void SetNation();
	eNATION GetNation() { return m_Nation; }

	BOOL IsTestServer() { return m_bTestServer; }

	void _5minProcess();
	void Process();
	void Start(WORD ServerNum);
	void End();
	CMHMap* GetMap() { return &m_Map; }
	WORD GetMapNum() const { return m_wMapNum; }
	BOOL GetStart()	const { return m_start; }
	void SetStart(BOOL);
	CPlayer* AddPlayer(DWORD dwPlayerID, DWORD dwAgentNum,DWORD UniqueIDinAgent,int ChannelNum, eUSERLEVEL);
	CPlayer* InitPlayerInfo(BASEOBJECT_INFO*, CHARACTER_TOTALINFO*, HERO_TOTALINFO*);
	void CreateNewPet(CPlayer* pMaster, DWORD dwItemIdx, DWORD SummonItemDBIdx, WORD wPetKind, WORD PetGrade = 1);
	CMonster* AddMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos,WORD wObjectKind = 32);
	CBossMonster* AddBossMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos);
	
	// 필드보스 - 05.12 이영준
	CFieldBossMonster* AddFieldBossMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos);
	CFieldSubMonster* AddFieldSubMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos);
	
	CNpc* AddNpc(BASEOBJECT_INFO* pBaseObjectInfo,NPC_TOTALINFO* pTotalInfo,VECTOR3* pPos);
	cSkillObject* AddSkillObject(cSkillObject* pSkillObj,VECTOR3* pPos);
	CMapObject* AddMapObject(DWORD Kind, BASEOBJECT_INFO* pBaseObjectInfo, MAPOBJECT_INFO* pMOInfo, VECTOR3* pPos);
	CPet* AddPet(BASEOBJECT_INFO&, const PET_OBJECT_INFO&, CPlayer* pPlayer);
	void RemovePlayer(DWORD dwPlayerID, BOOL bRemoveFromUserTable = TRUE);
	// 091214 ONS 펫 맵이동시 소환/봉인 관련 메세지를 출력하지 않도록 처리.
	void RemovePet(DWORD dwPetObjectID, BOOL bSummoned = TRUE);
	void RemoveMonster(DWORD dwPlayerID);
	void RemoveBossMonster(DWORD dwMonster);
	void RemoveNpc(DWORD dwPlayerID);
	void RemoveItemObject(DWORD ItemID);
	void RemoveSkillObject(DWORD SkillObjectID);
	void RemoveMapObject( DWORD MapObjID );

	
	inline CGridSystem* GetGridSystem()	{	return m_pGridSystem;	}

	void SendToOne(CObject* pObject,void* pMsg,int MsgLen);
	
	void ReloadResourceData();
	// 091127 LUJ, 소환 가능한 맵인지 반환한다
	BOOL IsNoRecallMap(CObject&);
	void	SetCharUpdateCompletion(BOOL bVal)	{ m_bCompletionChrUpdate = bVal;	}
	BOOL	GetCharUpdateCompletion()	{	return m_bCompletionChrUpdate;	}
	void	RemoveServerForKind( WORD wSrvKind );
	void	HandlingBeforeServerEND();

	void	LoadHackCheck();
	// 091106 LUJ, 채널별 몬스터
	void RemoveMonsterInGrid(DWORD gridIndex);
	DWORD GetMonsterCount(DWORD gridIndex) const;
	DWORD AddGameRoom(LPCTSTR address, LPCTSTR name, LPCTSTR upperIndex, LPCTSTR lowerIndex);
	void UpdateFiniteStateMachine(CMonster&, DWORD subIndex) const;

private:
	void AddMonsterCount(CObject&);
	void RemoveMonsterCount(CObject&);
	BOOL IsUncountable(EObjectKind) const;
	ULONGLONG GetGameRoomKey(LPCTSTR address) const;
	// 091106 LUJ, 키: 몬스터 오브젝트 번호
	typedef stdext::hash_set< DWORD > MonsterSet;
	// 091106 LUJ, 키: 채널 번호. 채널별 몬스터 오브젝트 셋이 저장된다
	typedef stdext::hash_map< DWORD, MonsterSet > ChannelMonsterMap;
	ChannelMonsterMap mChannelMonsterMap;
	// 091228 LUJ, PC방 정보
	struct GameRoom
	{
		DWORD mIndex;
		// 091228 LUJ, 고유번호가 DB에서 VARCHAR(20) 형식이며, 영문자가 포함될 수 있다
		TCHAR mDbIndex[21];
		TCHAR mName[MAX_NAME_LENGTH + 1];
		// 100216 ShinJS --- DB Load 시각 저장
		DWORD mDBLoadTime;
	};
	// 091228 LUJ, 키: IPv4 혹은 v6 정보를 점을 뺀 상태의 수치값으로 저장한다(예: 192.168.1.130 -> 192168001130)
	//			쿼리 재요청을 막기 위해, PC방이 아닌 경우에도 저장한다. 이때는 mIndex가 0이다.
	typedef stdext::hash_map< ULONGLONG, GameRoom > GameRoomContainer;
	GameRoomContainer mGameRoomContainer;
};

void __stdcall OnConnectServerSuccess(DWORD dwIndex, void* pVoid);
void __stdcall OnConnectServerFail(void* pVoid);

extern CServerSystem * g_pServerSystem;
BOOL LoadEventRate(char* strFileName);


#endif // !defined(AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_)

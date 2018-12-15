// SiegeWarfareMgr.h: interface for the CSiegeWarfareMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_)
#define AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

// 081017 LYW --- SiegeWarfareMgr : 워터시드 사용 연출 버그 수정 - 다른 Player가 각인할 때 연출이 안되는 현상 수정.
class CPlayer ;
class cSkillEffect ;

enum eSiegeWarfare_Type
{
	eSiegeWarfare_Type_Nera,
	eSiegeWarfare_Type_Max,
};

enum eSiegeWarfare_State
{
	eSiegeWarfare_State_Before,
	eSiegeWarfare_State_Start,
	eSiegeWarfare_State_First,
	eSiegeWarfare_State_End,
};

enum eSiegeWarfare_Map_Type
{
	eNeraCastle_Village,					// 공성 맵 마을
	eNeraCastle_UnderGround,				// 공성 지역 지하 맵
	eNeraCastle_Zebin,						// 공성 제뷘지역
	eNeraCastle_Lusen,						// 공성 루센지역
	eSiegeWarfare_Map_Max,
};

enum eSiegeWarfare_WaterSeed
{
	eSiegeWarfare_WaterSeed_Using,
	eSiegeWarfare_WaterSeed_Using_Complete,
	eSiegeWarfare_WaterSeed_Cancel,
};

struct SiegeWarfareKindMapInfo
{
	DWORD MapNum;
	WORD WarfareType;
	WORD MapType;
	float PosX;
	float PosZ;
};

enum eSiegeWarfare_Notify
{
	eSiegeWarfare_Notify_Befor_30m,
	eSiegeWarfare_Notify_Befor_5m,
	eSiegeWarfare_Notify_Begin,
	eSiegeWarfare_Notify_End,
	eSiegeWarfare_Notify_End_Before_10m,
	eSiegeWarfare_Notify_Max,
};

enum eSiegeWarfare_Info_Kind
{
//	eSiegeWarfare_Info_Schedule,
	eSiegeWarfare_Info_Damage_Ratio,
	eSiegeWarfare_Info_Heal_Ratio,
	eSiegeWarfare_Info_Critical_Rate,
	eSiegeWarfare_Info_WarterSeed_GuildLevel,
	eSiegeWarfare_Info_WarterSeed_Time,
	// 081017 LYW --- SiegeWarfareMgr : 각인을 허용하는 거리 정보 분류 추가.
	eSiegeWarfare_Info_Craving_Distance,
	eSiegeWarfare_Info_VilageInfo,
	eSiegeWarfare_Info_Max,
};

// 081009 LYW --- SiegeWarfareMgr : 공성 소스 머지.
enum eSiegeWarfare_WarterSeed_Error
{
	eSiegeWarfare_WarterSeed_Error_NotState = 1,
	eSiegeWarfare_WarterSeed_Error_UsingPlayer,
	eSiegeWarfare_WarterSeed_Error_NotGuildJoin,
	eSiegeWarfare_WarterSeed_Error_CastleGuld,
	eSiegeWarfare_WarterSeed_Error_GuildLevel,
	// 081017 LYW --- SiegeWarfareMgr : 워터시드 사용 거리체크 에러 분류 추가.
	eSiegeWarfare_WarterSeed_Error_SoFarDistance,
	eSiegeWarfare_WarterSeed_Error_Max,
};

// 081006 LYW --- SiegeWarfareMgr : 공성 집행위원이 사용할 맵이동 포인트 구조체 추가.
struct st_GUILDMAPMOVEINFO
{
	MAPTYPE mapNum ;

	float fXpos ;
	float fZpos ;
} ;

// 081015 LYW --- SiegeWarfareMgr : 워터시드 각인 스텝 분류.
enum eStep_CravingDirection
{
	eStep_Start = 0,
	eStep_Continue,
	eStep_End,
} ;

#define SIEGEWARFAREMGR CSiegeWarfareMgr::GetInstance()

#define WATERSEED_EFFECTNUM						301

class CSiegeWarfareMgr 
{
	CSiegeWarfareMgr();
	~CSiegeWarfareMgr();

	DWORD					m_dwMapType;	// 공성맵의 타입
	WORD					m_wState;		// 맵 상태

	DWORD					m_dwWaterSeedUsingGuildLevelLimit;				// 워터시드를 사용하기 위한 길드 레벨
	DWORD					m_dwWaterSeedUsingSecond;						// 워터시드를 시용하는 시간(초)

	// 081017 LYW --- SiegeWarfareMgr : 워터시드를 사용할 수 있는 거리를 담을 변수를 추가한다.
	WORD					m_wCravingDistance ;

	SYSTEMTIME				m_stReviveTime;

	BOOL					m_bRevive;

	WORD					m_wReviveMessageCount;
	DWORD					m_dwStartTime;

	std::list<SiegeWarfareKindMapInfo>	m_SiegeWarfareKindMapList;					// 공성전 관련 맵 리스트
	std::list<DWORD>					m_ForbiddenSkillList;						// 사용불가 스킬 리스트
	std::list<DWORD>					m_ForbiddenItemList;						// 사용불가 아이템 리스트

	// 080829 LYW --- SiegeWarfareMgr : 버프 스킬 비용을 담을 변수 선언.
	DWORD					m_dwBuffSkillPay ;

	// 081019 LYW --- SiegeWarfareMgr : 우물npc 위치를 담을 map 컨테이너 추가.
	typedef std::map<DWORD, VECTOR3>	M_WELLPOS ;
	M_WELLPOS							m_MWellPos ;

	// 091211 pdy 워터시드 연출 타겟설정이 안되는 버그 수정
	cSkillEffect*						m_WaterSeedEffect;

public:
	MAKESINGLETON( CSiegeWarfareMgr );

	void					Process();

	// 관련 정보를 가져온다.
	void					LoadInfo();
	// 정보를 초기화 한다.
	void					Init();

	void					SetReviveTime();

	void					SetRevive(BOOL bRevive)		{ m_bRevive = bRevive;	}

	// 사용 금지된 스킬 인가?
	BOOL					IsFobiddenSkill(DWORD SkillIdx);
	// 사용 금지된 아이템 인가?
	BOOL					IsFobiddenItem(DWORD ItemIdx);
	// 공성지역인가?
	BOOL					IsSiegeWarfareZone(DWORD MapNum);
	BOOL					IsSiegeWarfareZone();
	// 공성과 관련이 있는 맵인가?
	BOOL					IsSiegeWarfareKindMap(DWORD MapNum);

	BOOL					IsRevive();

	bool					UseCandleStand();

	// 공성 맵의 상태 가져오기
	WORD					GetState()	{ return m_wState; }

	// 네트워크 메시지 파서
	void					NetworkMsgParse(BYTE Protocol,void* pMsg );
	// 공성전 상황을 요청한다.
	void					CM_RequestState();
	// 공성전 상황 요청을 받는다.
	void					MMC_SiegeWarfareState(WORD State);

	// 080829 LYW --- SiegeWarfareMgr : 워터시드의 사용이 완료되었는지 서버에 요청하는 함수.
	void					IsEndUsing_WaterSeed() ;

	// 080829 LYW --- SiegeWarfareMgr : 버프 스킬 비용을 설정/반환하는 함수 추가.
	void					Set_BuffSkillPay(DWORD dwPay) { m_dwBuffSkillPay = dwPay ; }
	DWORD					Get_BuffSkillPay() { return m_dwBuffSkillPay ; }

	// 080904 LYW --- SiegeWarfareMgr : 현재 맵이 공성맵에서 스케쥴을 관리하는 맵인지 체크한다.
	BOOL					Is_CastleMap() ;

	// 080907 LYW --- SiegeWarfareMgr : 워터시드 사용 시간 반환 함수 추가.
	DWORD					Get_WaterSeedUsingSecond() { return m_dwWaterSeedUsingSecond ; }

	// 081009 LYW --- SiegeWarfareMgr : 공성소스 머지.
	DWORD					Get_WaterSeedLimitGuildLevel() { return m_dwWaterSeedUsingGuildLevelLimit; }

	// 081006 LYW --- SiegeWarfareMgr : 성 내 길드 집행위원이 사용할 맵이동 포인트 정보를 담을 리스트 추가.
	typedef std::list< st_GUILDMAPMOVEINFO >	L_GUILDMAPMOVEINFO ;
	L_GUILDMAPMOVEINFO		m_List_MapMove_Rushen ;
	L_GUILDMAPMOVEINFO		m_List_MapMove_Zevyn ;
    
	// 081006 LYW --- SiegeWarfareMgr : 집행위원이 사용할 맵이동 정보 로드/반환 함수 추가.
	void Load_GuildMapMoveInfo() ;
	void Get_GuildMapMoveInfo_Rushen(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE map) ;
	void Get_GuildMapMoveInfo_Zevyn(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE map) ;

	L_GUILDMAPMOVEINFO& Get_MapMoveList_Rushen()	{ return  m_List_MapMove_Rushen ; }
	L_GUILDMAPMOVEINFO& Get_MapMoveList_Zevyn()		{ return  m_List_MapMove_Zevyn ; }

	// 081009 LYW --- SiegeWarfareMgr : 워터시드 사용 길드 레벨 제한을 반환하는 함수 추가.
	DWORD GetGuildLevelLimit_UseWaterSeed()			{ return m_dwWaterSeedUsingGuildLevelLimit ; }
	DWORD GetUseWaterSeedSecond()					{ return m_dwWaterSeedUsingSecond ; }

	// 081015 LYW --- SiegeWarfareMgr : 워터시드 각인 연출을 하는 함수 추가.
	// 081017 LYW --- SiegeWarfareMgr : 워터시드 각인 연출 수정 - 다른 Player가 각인하는게 안보이는 현상 수정.
	//void Start_CravingDirection(BYTE byStep) ;
	void Start_CravingDirection(CPlayer* pPlayer, BYTE byStep) ;

	// 081017 LYW --- SiegeWarfareMgr : 워터시드 각인을 허용하는 수치를 반환하는 함수 추가.
	WORD Get_CravingDistance()						{ return m_wCravingDistance ; }

	// 081019 LYW --- SiegeWarfareMgr : 공성 맵 타입을 반환하는 함수 추가.
	DWORD Get_SiegeMapType()						{ return m_dwMapType ; }

	// 081019 LYW --- SiegeWarfareMgr : 우물 위치 로드 함수 추가.
	void Load_WellPos() ;

	// 081019 LYW --- SiegeWarfareMgr : 우물 위치 반환 함수 추가.
	VECTOR3* Get_WellPos(DWORD dwUniqueIdx) ;
};

#endif // !defined(AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_)


/*
L_GUILDMAPMOVEINFO& info = Get_MapMoveList_Rushen();
*/
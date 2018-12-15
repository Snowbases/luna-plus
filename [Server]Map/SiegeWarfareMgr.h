// SiegeWarfareMgr.h: interface for the CSiegeWarfareMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_)
#define AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SIEGEWARFAREMGR CSiegeWarfareMgr::GetInstance()

// 080901 LYW --- SiegeWarfareMgr : 소환 몬스터 체크 딜레이 수치 정의.
#define CHECK_SUMMON_TIME	3000

class CGuild;
class CPlayer;

enum eSiegeWarfare_State
{
	eSiegeWarfare_State_Before,
	eSiegeWarfare_State_Start,
	eSiegeWarfare_State_First,
	eSiegeWarfare_State_End,
};

enum eSiegeWarfare_Type
{
	eSiegeWarfare_Type_Nera,
	eSiegeWarfare_Type_Max,
};

enum eSiegeWarfare_Map_Type
{
	eNeraCastle_Village,					// 공성 맵 마을
	eNeraCastle_UnderGround,				// 공성 지역 지하 맵
	eNeraCastle_Zebin,						// 공성 제뷘지역
	eNeraCastle_Lusen,						// 공성 루센지역
	eSiegeWarfare_Map_Max,
};

enum eSiegeWarfare_Info_Kind
{
	eSiegeWarfare_Info_Damage_Ratio,
	eSiegeWarfare_Info_Heal_Ratio,
	eSiegeWarfare_Info_Critical_Rate,
	eSiegeWarfare_Info_WarterSeed_GuildLevel,
	eSiegeWarfare_Info_WarterSeed_Time,
	// 081017 LYW --- SiegeWarfareMgr : 각인을 허용하는 거리 정보 분류 추가.
	eSiegeWarfare_Info_Craving_Distance,
	eSiegeWarfare_Info_VilageInfo,
	// 081210 LYW --- SiegeWarfareMgr : 길드 던전 내 루쉔/제뷘 길드 부활위치 분류 추가.
	eSiegeWarfare_Info_ReviveInfo_Rushen,
	eSiegeWarfare_Info_ReviveInfo_Zevyn,
	eSiegeWarfare_Info_Max,
};

// 공성전 관련 메시지
enum eSiegeWarfare_Notify
{
	eSiegeWarfare_Notify_Befor_30m,
	eSiegeWarfare_Notify_Befor_5m,
	eSiegeWarfare_Notify_Begin,
	eSiegeWarfare_Notify_End,
	eSiegeWarfare_Notify_End_Before_10m,
	eSiegeWarfare_Notify_Max,
};

enum eSiegeWarfare_WaterSeed
{
	eSiegeWarfare_WaterSeed_Using,
	eSiegeWarfare_WaterSeed_Using_Complete,
	eSiegeWarfare_WaterSeed_Cancel,
};

enum eSiegeWarfare_State_Message_Type
{
	eSiegeWarfare_State_Message_Single,
	eSiegeWarfare_State_Message_All,
	eSiegeWarfare_State_Message_Max,
};

// 081009 LYW ---  공성소스 머지.
enum eSiegeWarfare_WarterSeed_Error
{
	eSiegeWarfare_WarterSeed_Error_NotState = 1,
	eSiegeWarfare_WarterSeed_Error_UsingPlayer,
	eSiegeWarfare_WarterSeed_Error_NotGuildJoin,
	eSiegeWarfare_WarterSeed_Error_CastleGuld,
	eSiegeWarfare_WarterSeed_Error_GuildLevel,
	// 081017 LYW --- SiegeWarfareMgr : 워터시드 사용 거리체크 에러 분류 추가.
	eSiegeWarfare_WarterSeed_Error_SoFarDistance,
	// 090706 pdy --- 클라이언트 워터시드 시간조작 에러 분류 추가.
	eSiegeWarfare_WarterSeed_Error_WarterSeedTime,
	eSiegeWarfare_WarterSeed_Error_Max,
};

struct VillageWarp
{
	DWORD	MapNum;
	float	PosX;
	float	PosZ;
};

struct SiegeWarfareSkillInfo
{
	float	DecreaseDamageRatio;								// 데미지 감소량(%)
	float	DecreaseHealRatio;									// 힐 감소량(%)
	float	DecreaseCriticalRatio;								// 크리티컬 감소량(%)
};

struct SiegeWarfareKindMapInfo
{
	DWORD MapNum;
	WORD WarfareType;
	WORD MapType;
	float PosX;
	float PosZ;
};

// 081006 LYW --- SiegeWarfareMgr : 공성 집행위원이 사용할 맵이동 포인트 구조체 추가.
struct st_GUILDMAPMOVEINFO
{
	MAPTYPE mapNum ;

	float fXpos ;
	float fZpos ;
} ;

// 080919 LYW --- cMapMoveDialog : 다이얼로그 용도 분류.
enum eMapMoveDialogUseCase
{
	eNormalStype = 0,														// 일반적으로 사용하는 용도.
	eCastle_Rushen,															// 루쉔 길드 위원회에서 사용하는 용도.
	eCastle_Zevyn,															// 제뷘 길드 위원회에서 사용하는 용도.
} ;

class CSiegeWarfareMgr  
{
	CSiegeWarfareMgr();
	~CSiegeWarfareMgr();

	DWORD					m_dwMapType;									// 공성맵의 타입
	DWORD					m_dwCastleGuildIdxList[eSiegeWarfare_Map_Max];	// 성 길드 인덱스
	VillageWarp				m_stCastleVilage;								// 성 마을
	SiegeWarfareSkillInfo	m_stSkillInfo;									// 성 정보
	SYSTEMTIME				m_stWaterSeedUsingCompleteTime;					// 워터시드 사용 완료 시간
	WORD					m_wState;										// 맵 상태
	DWORD					m_dwWarfareType;								// 공선전 Type
	WORD					m_wNotifyStep;									// 알림 상태 (일부만 사용됨)

	DWORD					m_dwWaterSeedUsingGuildLevelLimit;				// 워터시드를 사용하기 위한 길드 레벨
	DWORD					m_dwWaterSeedUsingSecond;						// 워터시드를 시용하는 시간(초)
	BOOL					m_bWaterSeedUsing;								// 워터시드를 사용중인지 체크
	DWORD					m_dwWaterSeedUsingPlayerIdx;					// 워터시드를 사용중인 플레이어 인덱스

	BOOL					m_bSummonFlag;

	// 081009 LYW --- SiegeWarfareMgr : 공성소스머지.
	BOOL					m_bWaterSeedFlag;

	std::list<SiegeWarfareKindMapInfo>	m_SiegeWarfareKindMapList;					// 공성전 관련 맵 리스트
	std::list<DWORD>					m_ForbiddenSkillList;						// 사용불가 스킬 리스트
	std::list<DWORD>					m_ForbiddenItemList;						// 사용불가 아이템 리스트

	std::list<SYSTEMTIME>				m_ScheduleStartTime[eSiegeWarfare_Type_Max];
	std::list<SYSTEMTIME>				m_ScheduleEndTime[eSiegeWarfare_Type_Max];

	// 080829 LYW --- SiegeWarfareMgr : 버프 스킬 비용을 담을 변수 선언.
	DWORD					m_dwBuffSkillPay ;

	// 080901 LYW --- SiegeWarfareMgr : 맵서버 시작 시, 기존에 존재하던 몹이 있는지, 있으면 다시 로딩 요청을 하기 위한 변수 추가.
	BYTE					m_IsLoadedSummon ;

	// 081017 LYW --- SiegeWarfareMgr : 워터시드를 사용할 수 있는 거리를 담을 변수를 추가한다.
	WORD					m_wCravingDistance ;

	// 081210 LYW --- SiegeWarfareMgr : 길드 던전 내 루쉔/제뷘 부활(안전지대) 위치를 담을 구조체.
	VillageWarp				m_stDGRP_Rushen ;
	VillageWarp				m_stDGRP_Zevyn ;

public:
	MAKESINGLETON( CSiegeWarfareMgr );
	// 성 마을 맵번호를 설정
	VOID					SetVilageMapInfo(DWORD MapNum, float PosX, float PosZ)	{	m_stCastleVilage.MapNum = MapNum;
																						m_stCastleVilage.PosX = PosX;
																						m_stCastleVilage.PosZ = PosZ;	}
	// 감소된 데미지량을 가져온다.
	float					GetDecreaseDamageRatio()	{	return m_stSkillInfo.DecreaseDamageRatio;	}
	// 감소된 힐량을 가져온다.
	float					GetDecreaseHealRatio()		{	return m_stSkillInfo.DecreaseHealRatio;	}
	// 감소된 크리티컬수치를 가져온다.
	float					GetDecreaseCriticalRatio()	{	return m_stSkillInfo.DecreaseCriticalRatio;	}

	// 성 마을 정보를 가지고 온다.
	VillageWarp*				GetVilageInfo()		{	return &m_stCastleVilage;	}

	// 성을 소유 하고 있는 길드 인덱스 설정
	VOID					SetCastleGuildIdx(DWORD MapType,DWORD	GuildIdx)	{	m_dwCastleGuildIdxList[MapType] = GuildIdx;	}
	// 성을 소유 하고 있는 길드 인덱스를 얻어온다.
	DWORD					GetCastleGuildIdx(DWORD MapType)	{	return m_dwCastleGuildIdxList[MapType];	}

	// 성의 속성을 설정하여 준다.
	VOID					SetMapInfo(DWORD MapType)	{	m_dwMapType = MapType;	}
	// 성의 속성을 가져온다.
	DWORD					GetMapInfo()	{	return m_dwMapType;	}

	VOID					SetCastleGuildIndex(DWORD MapType, DWORD GuildIDX)	{	m_dwCastleGuildIdxList[MapType] = GuildIDX;	}

	VOID					SetSummonFlag(BOOL bFlag)	{	m_bSummonFlag = bFlag;	}

	// 공성지역인가?
	BOOL					IsSiegeWarfareZone(DWORD MapNum,BOOL bCheckWarState = TRUE );
	// 현재 맵이 공성지역 인가?
	BOOL					IsSiegeWarfareZone();

	// 맵 정보를 읽어 온다.
	VOID					LoadMapInfo();

	// 공성전에 참여한 플레이어중 성 길드가 아닌 플레이어를 성 마을로 워프 시킨다.
	VOID					WarpToVilage();
	// 마을로 플레이어 한명을 보내기 위한 함수
	VOID					WarpToVilagePlayer(CPlayer* pPlayer);

	// 유저를 아지트로 워프 시키기 위한 함수
	BOOL					WarpToAgit(CPlayer* pPlayer);

	// 080922 LYW --- SiegeWarfareMgr : 집행위원을 통해 맵 이동을 하는 함수 추가.
	VOID					ChangeMap(CPlayer* pPlayer, WORD wType, WORD wMapNum) ;

	// 메인 루프
	VOID					Process();

	// 초기화 데이터 셋팅 함수
	VOID					Init();

	// 네트워크 메시지 파서
	VOID					NetworkMsgParse( DWORD dwConnectionIndex, BYTE Protocol,void* pMsg );	

	// 공성 맵의 상태 가져오기
	WORD					GetState()	{ return m_wState; }
	// State를 셋팅해 주는 동시에 패킷을 보낸다. MessageType에 따라 결정됨.
	VOID					SetState(WORD State, WORD MessageType);
	// State만 셋팅해준다.
	VOID					SetState(WORD State)		
	{	
		m_wState = State;	

		// 081028 LYW --- SiegeWarfareMgr : 공성 상태 바뀜을 로그로 남긴다.
		char szLog[1024] = {0, } ;
		sprintf( szLog, "SelpMapNum : %d \t State : %d", g_pServerSystem->GetMapNum(), m_wState ) ;
		WriteSiegeSchedule( szLog ) ;
	}

	// 공성이 시작되었는지 체크 하는 함수. 이 함수 내부에서 공성 30분전, 5분전 메시지를 같이 처리 한다.
	// eNeraCastle_Village 의 속성을 가지고 있는 맵만 체크 한다. 이 함수를 통화한 경우에 다른 관련 공성 맵에
	// 공성이 시작되었음을 알려준다.
	BOOL					IsBeginSiegeWarfare();
	// 공성이 완료 되었는가?
	BOOL					IsEndSiegeWarfare();

	// 워터시드를 사용하고 있는 중인가?
	BOOL					IsWaterSeedUsing()		{	return m_bWaterSeedUsing;	}
	// 워터시드를 사용중인 플레이어인가?
	BOOL					IsWaterSeedUsingPlayer(DWORD PlayerIdx);

	// 사용 금지된 스킬 인가?
	BOOL					IsFobiddenSkill(DWORD SkillIdx);
	// 사용 금지된 아이템 인가?
	BOOL					IsFobiddenItem(DWORD ItemIdx);

	// 워터시드 중지
	VOID					CancelWaterSeedUsing(CPlayer* pPlayer);

	// 혹시나 하는 마음에 만든 함수 ㅡ_ㅡ;; 공성 시간중에 다시 네라성 마을의 맵이 시작 되었을 경우에 사용될수 있음.
	VOID					CheckBeforeState();

	// 080829 LYW --- SiegeWarfareMgr : 버프 스킬 비용을 설정/반환하는 함수 추가.
	VOID					Set_BuffSkillPay(DWORD dwPay) { m_dwBuffSkillPay = dwPay ; }
	DWORD					Get_BuffSkillPay() { return m_dwBuffSkillPay ; }

	VOID Set_LoadSummonFlag(BYTE byFlag) { m_IsLoadedSummon = byFlag; }
	BOOL Is_LoadedSummon() { return m_IsLoadedSummon; }
	BOOL Is_CastleMap() const { return eNeraCastle_Village == m_dwMapType; }

	// 081006 LYW --- SiegeWarfareMgr : 성 내 길드 집행위원이 사용할 맵이동 포인트 정보를 담을 리스트 추가.
	typedef std::list< st_GUILDMAPMOVEINFO >	L_GUILDMAPMOVEINFO ;
	L_GUILDMAPMOVEINFO		m_List_MapMove_Rushen ;
	L_GUILDMAPMOVEINFO		m_List_MapMove_Zevyn ;
    
	void Load_GuildMapMoveInfo();
	void Get_GuildMapMoveInfo_Rushen(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE);
	void Get_GuildMapMoveInfo_Zevyn(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE);
	BYTE Is_ValidDistance_FromWell(CPlayer*);

	L_GUILDMAPMOVEINFO& Get_MapMoveList_Rushen() { return  m_List_MapMove_Rushen; }
	L_GUILDMAPMOVEINFO& Get_MapMoveList_Zevyn()	{ return  m_List_MapMove_Zevyn; }
	void WriteSiegeSchedule( char* pMsg );

	VOID VillageProcess();
	VOID UnderGroundProcess();
	VOID EmblemProcess();
	VOID WaterSeedProcess();
	WORD MC_InstallWaterSeed(CPlayer*);
	VOID MMC_SiegeWarfareState(WORD State);
	BYTE CM_RequestCastleBuff(CPlayer*, WORD wUniqueNpcIndex);

	VOID LoadSiegeWarfareInfo();
	VOID UpdateSiegeWarfareInfo( BOOL bInit = FALSE );
	
	VillageWarp* GetDGRP_Rushen() { return &m_stDGRP_Rushen ; }
	VillageWarp* GetDGRP_Zevyn() { return &m_stDGRP_Zevyn ; }
	void WriteLog(char* pMsg) ;

	VOID UpdateWaterSeedComplateSynToDB( DWORD dwPlayerIdx , DWORD dwGuildidx );
	VOID UpdateWaterSeedComplateAckFromDB( DWORD dwResult , DWORD dwPlayerIdx ,DWORD dwMapType ,DWORD dwGuildIdx);
	void GuildBreakUp( DWORD dwGuildidx );
};

#endif // !defined(AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_)

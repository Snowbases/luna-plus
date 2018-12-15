#pragma once

#define HOUSINGMGR USINGTON(HousingMgr)
#define HOUSINGMAPNUM		99

struct stHouseReservation
{
	DWORD dwCharIndex;

	DWORD dwHouseOwnerIndex;
	DWORD dwChannelID;
	DWORD dwRegistTime;
	CHAR  cVisitKind;

	void Clear()
	{
		memset(this, 0, sizeof(stHouseReservation));
	}

	stHouseReservation()
	{
		Clear();
	}
};

struct stHouseVoteInfo
{
	DWORD dwPlayerIndex;		//추천자
	DWORD dwVoteHouseUserIndex;	//추천집 OwnerUserIndex

	void Clear()
	{
		memset(this, 0, sizeof(stHouseVoteInfo));
	}

	stHouseVoteInfo()
	{
		Clear();
	}
};

struct stHouseRiderInfo
{
	DWORD dwPlayerIndex;			//탑승자
	DWORD dwFurnitureObjectIndex;	//탑승물의 ObjectIndex
	WORD  wSlot;					//탑승위치 최대4

	void Clear()
	{
		memset(this, 0, sizeof(stHouseRiderInfo));
	}

	stHouseRiderInfo()
	{
		Clear();
	}
};

class HousingMgr
{
private:
	SYSTEMTIME							m_LastCheckTime;
	// 090923 ONS 메모리풀 교체
	CPool<stHouse>*						m_HousePool;
	CPool<stFurniture>*					m_FurniturePool;
	CPool<stHouseReservation>*			m_ReservationPool;
	CPool<stHouseVoteInfo>*				m_VotePool;
	CPool<stHouseRiderInfo>*			m_RiderPool;

	CYHHashTable<stHouse>				m_HouseList;
	CYHHashTable<stHouseVoteInfo>		m_VoteList;
	CYHHashTable<stHouseRiderInfo>		m_RiderList;
	CYHHashTable<stHouseRankNPC>		m_RankNpcList;

	CYHHashTable<stHouse>				m_LoadingHouseList;			// DB로부터 데이터를 수신중인 하우스 : 등록되어 있는 하우스는 입장불가
	CYHHashTable<stHouseReservation>	m_ReservationList;			// 하우스 방문객의 입장예약 목록
	stHousingSettingInfo				m_HousingSettingInfo;

	BOOL								m_bLoadRank;
	stHouseRank							m_HouseRank;				// 하우스랭킹 기록
	DWORD								m_dwHouseRankSetTime;


	CIndexGenerator						m_FurnitureIDGenerator;

public:
	HousingMgr(void);
	virtual ~HousingMgr(void);

	// 시스템
	void	Init();
	void	Process();
	void	NetworkMsgParser(DWORD dwConnectionIndex, BYTE Protocol, void* pMsg, DWORD dwLength);

	void	SendHouseMsgToPlayer(stHouse* pHouse, CPlayer* pPlayer);
	void	ChangeMapFromHouse(DWORD dwCharIndex, MAPTYPE changemap=0); // changemap==0 은 LoginMap

	void	SetHouseRank(stHouseRank* pRank);
	stHouseRank* GetHouseRank();
	void	SetLoadRank(BOOL bVal)			{m_bLoadRank = bVal;}
	void	RecallRankNPC();

	DWORD	GetLoadHouseNum()				{return m_HouseList.GetDataNum();}
	stHouseRiderInfo* GetRiderInfo(DWORD dwCharIndex)	{return m_RiderList.GetData(dwCharIndex);}

	// 리소스
	stHousingSettingInfo* GetHousingSettingInfo() {return &m_HousingSettingInfo;}
	void	LoadHousing_Setting();
	DWORD	GetDecoUsePoint(BOOL bRanker=FALSE);
	DWORD	GetFurnitureIndexFromItemIndex(DWORD dwItemIndex);
	DWORD	GetRemainTimeFromItemIndex(DWORD dwItemIndex);

	stDynamicHouseNpcMapInfo* GetDynimiHouseNpcMapInfo(DWORD dwMapIndex);
	stDynamicHouseNpcInfo*	  GetDynimiHouseNpcInfo(DWORD dwMapIndex, BYTE byRank, BYTE byExterioKind);

	// 하우스 생성 & 해제 & 검색
	DWORD	GetNewHouseChannel();
	void	RemoveHouseChannel(DWORD dwChannelID);

	stHouse* CreateNewHouse();
	void	RemoveHouseFromPool(stHouse* pHouse);
	BOOL	CreateHouse(stHouse* pHouse);
	void	DeleteHouse(DWORD dwUserIndex);
	void	CloseHouse(DWORD dwUserIndex);

	BOOL	IsLoadingHouse(DWORD dwOwnerUserIndex);
	void	RemoveLoadingHouse(DWORD dwUserIndex);

	stHouse* GetHouseByUserIndex(DWORD dwUserIndex);
	stHouse* GetHouseByChannelID(DWORD dwChannelID);
	DWORD	GetChannelIDByUserIndex(DWORD dwUserIndex);
	DWORD	GetPlayerNumInHouse(DWORD dwChannelID);
	// 하우스 방문 & 퇴장
	DWORD	GetChannelIDFromReservationList(DWORD dwCharIndex);
	VECTOR3*  GetStartPosFromeservationList(DWORD dwCharIndex);
	void	ReservationHouse(DWORD dwOwnerIndex, DWORD dwChannelID, DWORD dwCharIndex, CHAR cVisitKind);
	BOOL	EntranceHouse(DWORD dwCharIndex);
	void	ExitHouse(DWORD dwCharIndex, BOOL bVisitHouse);

	// 하우스 가구관리
	DWORD	GeneraterFurnitureID();
	void	ReleaseFurnitureID(DWORD id);

	int		CheckExtendHouse(DWORD dwCharIndex);
	void	ExtendHouse(DWORD dwCharIndex, DWORD dwItemDBIndex, DWORD dwItemIndex, const ITEM_INFO* pItemInfo);
	BOOL	ChangeHouseName(DWORD dwCharIndex, char* pHouseName);

	VECTOR3* GetStartPos(stHouse* pHouse);

	stFurniture* CreateNewFurniture();

	BOOL	IsDecoMode(DWORD dwOwnerUserIndex, DWORD dwCharIndex);
	BOOL	CanDecoMode(DWORD dwOwnerUserIndex, DWORD dwCharIndex);
	BOOL	IsInstall(DWORD dwOwnerUserIndex, DWORD dwObjectIndex);
	BOOL	CanInstall(DWORD dwOwnerUserIndex, DWORD dwObjectIndex);
	BOOL	CanUpdate(DWORD dwOwnerUserIndex, DWORD dwObjectIndex);
	BOOL	IsKeeping(DWORD dwOwnerUserIndex, DWORD dwObjectIndex);
	BOOL	CanKeeping(stHouse* pHouse, DWORD dwItemIndex, BYTE bySlot);
	BOOL	CanDestroy(DWORD dwOwnerUserIndex, DWORD dwObjectIndex);

	BOOL	ChangeMaterial(DWORD dwCharIndex, DWORD dwItemDBIndex, DWORD dwItemIndex, const ITEM_INFO* pItemInfo);
	DWORD	GetDecoPoint(stFurniture* pFurniture);
	void	UseDecoBonus(DWORD dwChannelID, DWORD dwPlayerID, DWORD dwBonusIndex);

	stFurniture* GetFurniture(DWORD dwOwnerIndex, DWORD dwObjectIndex);
	void	AddFurnitureToHouse(stFurniture* pFurniture, stHouse* pHouse);				// 정보가 채워져있는 가구를 하우스에삽입
	void	AddNewFurniture(stFurniture* pFurniture, stHouse* pHouse);					// 새가구(FurnitureIndex, slot, state는 유효해야함)를 하우스에삽입
	void	UpdateFurniture(stFurniture* pFurniture, stHouse* pHouse, BOOL bAddDecoPoint=FALSE);					// 정보가 채워져있는 가구를 하우스에갱신
	void	RemoveFurniture(DWORD dwObjectIndex, stHouse* pHouse, BOOL bExtend=FALSE);	// 가구를 하우스에서 제거
	void	RemoveFurnitureFromPool(stFurniture* pFurniture);

	int		GetEmptySlot(stHouse* pHouse, DWORD dwHighCategory);
};

EXTERNGLOBALTON(HousingMgr)
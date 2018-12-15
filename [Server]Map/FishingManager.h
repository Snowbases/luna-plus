#pragma once
#include "stdafx.h"

class CPlayer;

#define FISHINGMGR USINGTON(CFishingManager)
#define FISHINGRATE_SCALE		1000.0f							// 소수점 이하 3자리까지 유효.

#define MAX_FISHINGPLACE		10
#define MAX_FISHITEM			10
#define FISHINGTIME_LATENCY		500								// 네트웍지연시간

enum {
	eFISHINGRATE_ITEM_UNKNOWN	= 0,
	eFISHINGRATE_ITEM_UTILITY	= 1,
	eFISHINGRATE_ITEM_BAIT		= 2,
};

enum {
	eFISHMISSION_STATE_READY	= 0,	// 미션주기전
	eFISHMISSION_STATE_STANDBY	= 1,	// 미션주고 응답대기
	eFISHMISSION_STATE_NEW		= 2,	// 미션주기
	eFISHMISSION_STATE_END		= 3,	// 미션끝내기
};

struct stFishingMissionInfo;

extern DWORD g_FishingMissionCode[MAX_FISHINGMISSIONCODE];		// 물고기코드
extern std::map<DWORD, stFishingMissionInfo> g_mapMissionInfo;	// 미션정보

struct stFishItemInfo
{
	DWORD dwItemIndex;											// 아이템Index
	float fRate;												// 확률
};

struct stFishRateInfo
{
	int   nGrade;												// 물고기등급
	float fRate;												// 확률
};

struct stFishingMissionInfo
{
	DWORD dwCode;												// 미션코드
	DWORD dwRewardItem;											// 보상아이템
	int   dwRewardItemNum;										// 보상아이템 개수
};

struct stFishingPlaceInfo
{
	char  cNpcKind;												// NPCLIST.bin에 저장된 NpcKind필드
	VECTOR3 vPos;												// 위치
	DWORD dwLifeTime;											// 지속시간.
	DWORD dwProcessTime;										// 게이지 진행시간. (난이도)
	int   nRepeatCount;											// 게이지 반복횟수. (난이도)
	stFishItemInfo			m_FishItem[MAX_FISHITEM];			// 물고기정보
	std::vector<stFishItemInfo> m_vecPlaceEff;					// 낚시터에 따른 적용효과
	std::vector<stFishItemInfo> m_vecWeatherEff[eWS_Max];		// 날씨에 따른 적용효과
	DWORD dwBaseDelay;											// 생성주기 기본
	DWORD dwRandDelay;											// 생성주기 랜덤
	char  cFishItemNum;											// 물고기아이템 수
	float fHitAddRate[4];										// [0]공통, [1]피라미, [2]붕어, [3]잉어 ; 컨트롤 적중시 추가확률

	stFishingPlaceInfo()	{Clear();}
	void Clear()
	{
		cNpcKind=cFishItemNum = 0;
		vPos.x=vPos.y=vPos.z=0.0f;
		dwLifeTime=dwProcessTime = 0;
		nRepeatCount = 0;
		memset(m_FishItem, 0, sizeof(stFishItemInfo)*MAX_FISHITEM);
		dwBaseDelay=dwRandDelay = 0;
		m_vecPlaceEff.clear();
		int i;
		for(i=0; i<eWS_Max; i++)
			m_vecWeatherEff[i].clear();

		for(i=0; i<4; i++)
			fHitAddRate[i] = 1.0f;
	}
};

struct stFishingPlaceInst
{
	BYTE  byInfoIndex;											// m_FishingPlaceInfo[]에서 참조할 index
	DWORD dwObjectIndex;										// g_pUserTable에 등록된 index
	DWORD dwGenTime;											// 생성시간.
	DWORD dwDelTime;											// 소멸시간.

	stFishingPlaceInst()	{Clear();}
	void Clear()
	{
		byInfoIndex = 0;
		dwObjectIndex=dwGenTime=dwDelTime = 0;
	}
};

struct stFishingRate
{
	DWORD dwItemIndex;
	stFishRateInfo FishList[MAX_FISHITEM];

	stFishingRate()			{Clear();}
	void Clear()
	{
		dwItemIndex = 0;
		int i;
		for(i=0; i<MAX_FISHITEM; i++)
		{
			FishList[i].nGrade = -1;
			FishList[i].fRate = 0.0f;
		}
	}
};

struct stFishingEvent
{
	BOOL bOnEvent;				// 이벤트구동 플래그
	WORD wPoint[3];				// [0]Perfect환산점수, [1]Great환산점수, [2]Good환산점수
	WORD wGoalPoint[3];			// 목표점수
	DWORD dwRewardItemIdx[3];	// 보상아이템

	stFishingEvent()
	{
		bOnEvent = FALSE;

		memset(wPoint, 0, sizeof(wPoint));
		memset(wGoalPoint, 0, sizeof(wGoalPoint));
		memset(dwRewardItemIdx, 0, sizeof(dwRewardItemIdx));
	}
};

class CFishingManager
{
public:
	CFishingManager(void);
	virtual ~CFishingManager(void);

	void Init();
	void NetworkMsgParse( BYTE Protocol, void* pMsg, DWORD dwLength );

	void Fishing_Ready_Syn(void* pMsg);
	void Fishing_GetFish_Syn(void* pMsg);
	void Fishing_Cancel_Syn(void* pMsg);
	void Fishing_FPChange_Syn(void* pMsg);

	void Process();

	BOOL				m_bActive;
	BOOL				m_bInit;
	BOOL				m_bUseMission;
	stFishingPlaceInfo	m_FishingPlaceInfo[MAX_FISHINGPLACE];
	stFishingPlaceInst	m_FishingPlaceInst[MAX_CHANNEL_NUM][MAX_FISHINGPLACE];
	CYHHashTable< stFishingPlaceInst*>	m_FishingGabagePlace;	//매 루프시 소멸된 낚시터Index 임시저장.

	stFishingEvent		m_FishingEventInfo;

	BOOL AddFishingPlace(stFishingPlaceInfo* pFishingPlaceInfo, DWORD dwChannel, BYTE byInfoIndex);
	void DelFishingPlace(stFishingPlaceInst* pInst);
	void SendGetFish(CPlayer* pPlayer, WORD wResCode, void* pMsg=NULL);

	stFishingPlaceInst* GetFishingPlaceInst(DWORD dwPlaceIndex);
	
	void SetPlaceEffect();
	void SetWeatherEffect();

	DWORD GetItemIdxFromFM_Code(int nCode);	// 낚시미션 코드로 아이템Index얻기
	void GetFishCodeFromFM_Code(DWORD dwMissionCode, WORD* pFishCode);	// 낚시미션 코드로 물고기코드 얻기
	void SendFishingMissionInfo(CPlayer* pPlayer, BOOL bNew);

	stFishingRate* GetFishingUtilityRate(DWORD dwItemIndex);
	stFishingRate* GetFishingBaitRate(DWORD dwItemIndex);


	BOOL GetActive() {return m_bActive;}
	char CheckMissionState();
	void ChangeMissionState();
	void ProcessPlayer(CPlayer* pPlayer);

private:
	int m_nChannelNum;
	int m_nFishingPlaceNum;

	int m_nPlaceEff[MAX_FISHINGPLACE][eFishItem_Max];
	int m_nWeatherEff[MAX_FISHINGPLACE][eWS_Max][eFishItem_Max];

	char  m_cMissionState;
	DWORD m_dwMissionSendTime;		// 낚시 시작후 미션보내기까지 대기시간 (Pulling 까지의 시간)
	DWORD m_dwMissionDelayTime;		// 미션부여 간격
	DWORD m_dwMissionEndTime;		// 미션 완료를 기다리는 시간.

	std::map<DWORD, stFishingRate*> m_mapUtilRateList;
	std::map<DWORD, stFishingRate*> m_mapBaitRateList;
};

EXTERNGLOBALTON(CFishingManager)
// GuildTournamentMgr.h: interface for the CGuildTournamentMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUILDTOURNAMENTMGR_H__21A71231_1238_4D1F_99B6_D53A80C23436__INCLUDED_)
#define AFX_GUILDTOURNAMENTMGR_H__21A71231_1238_4D1F_99B6_D53A80C23436__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\[CC]BattleSystem\GTournament\Battle_GTournament.h"

#define GTMGR CGuildTournamentMgr::GetInstance()
#define GTMAPNUM		94
#define GTRETURNMAPNUM	52

#define GT_NOTIFY_STARTTIME			(60000 * 5)
#define GT_NOTIFY_INTERVAL			60000

#define GT_CONVERT_DAY2MS(day)		(60000 * 60 * 24 * day)
#define GT_CONVERT_HOUR2MS(hour)	(60000 * 60 * hour)
#define GT_CONVERT_MIN2MS(min)		(60000 * min)
#define GT_CONVERT_SEC2MS(sec)		(1000 * sec)

#define GT_WAITING_NONE			0x00000000
#define GT_WAITING_INFOLOAD		0x00000001
#define GT_WAITING_INFOLOADALL	0x00000010
#define GT_WAITING_PLAYERLOAD	0x00000100

enum {
	eGTTime_OP_Add = 0,
	eGTTime_OP_Sub,
};

enum {
	eGTRewardReceiver_Master = 0,
	eGTRewardReceiver_Players,
	eGTRewardReceiver_GuildMember,
};

class CGuild;
class CPlayer;

struct GTRewardItem
{
	BYTE	nReceiver;
	DWORD	dwItemIndex;
	WORD	wNum;
};

struct GTRewardInfo
{
	DWORD	dwPoint;
	CYHHashTable<GTRewardItem>	RewardItemList;
	// 091113 ONS 골드보상 추가
	DWORD	dwGold;
};

struct GTLevel_Value
{
	WORD wMinLevel;
	WORD wMaxLevel;
	DWORD dwValue;

	BOOL IsInRange(WORD nLevel)
	{
		if(wMinLevel<=nLevel && nLevel<=wMaxLevel)
			return TRUE;

		return FALSE;
	};
};

struct GTImmortalInfo
{
	DWORD	dwClassCode;
	DWORD	dwTime;
};

struct GTGeneralInfo
{
	BOOL	bObAllow;				//옵저버허용여부
	DWORD	dwFee[2];				//참가비   : [0]길드포인트 [1]참가비(42억이하)
	WORD	wLimit_Team[2];			//제한팀수 : [0]최소참가팀, [1]최대참가팀
	WORD	wLimit_Guild[2];		//제한길드 : [0]참가길드레벨, [1]참가길드멤버수
	DWORD	dwEntryStart;			//신청기간 : 신청시작
	DWORD	dwEntryEnd;				//신청기간 : 신청마감
	DWORD	dwStartTime;			//경기시작 : 시작시간
	WORD	wTableOpen;				//마감후 대진표공개시간 (분단위)
	WORD	wPlayRule[2];			//경기방식 : [0]경기타입, [1]경기목표
	WORD	wPlayTime[2];			//경기시간 : [0]라운드시간, [1]라운드사이의 입장대기시간 (분단위)
	GTRewardInfo	RewardInfo[2];	//보상정보 : [0]1위보상, [2]2위보상

	int		nRespawnListSize;		//리스폰시간 리스트
	GTLevel_Value* pRespawnList;

	CYHHashTable<GTImmortalInfo> ImmortalList;	//무적시간 테이블

	GTGeneralInfo()
	{
		RewardInfo[0].RewardItemList.Initialize(10);
		RewardInfo[1].RewardItemList.Initialize(10);
		ImmortalList.Initialize(60);
		pRespawnList = NULL;
	};

	~GTGeneralInfo()
	{
		Clear();
	}

	void Clear()
	{
		DWORD dwSize;

		int i;
		for(i=0; i<2; i++)
		{
			dwSize = RewardInfo[i].RewardItemList.GetDataNum();
			if(0 < dwSize)
			{
				RewardInfo[i].RewardItemList.SetPositionHead();
				GTRewardItem* pItem;

				while((pItem = RewardInfo[i].RewardItemList.GetData())!= NULL)
				{
					delete pItem;
					pItem = NULL;
				}
			}
			RewardInfo[i].RewardItemList.RemoveAll();
		}

		dwSize = nRespawnListSize;
		if(0 < dwSize)
		{
			delete [] pRespawnList;
		}

		dwSize = ImmortalList.GetDataNum();
		if(0 < dwSize)
		{
			ImmortalList.SetPositionHead();
			GTImmortalInfo* pInfo;

			while((pInfo = ImmortalList.GetData())!= NULL)
			{
				delete pInfo;
				pInfo = NULL;
			}
		}
		ImmortalList.RemoveAll();
	};
};

struct GTPlayerInfo
{
	DWORD dwPlayerID;
	DWORD dwScore;
	char name[MAX_NAME_LENGTH+1];

	GTPlayerInfo() {dwPlayerID=dwScore=0; memset(name, 0, sizeof(name));}
};

struct GTInfo
{
	CGuild* pGuild;
	WORD	wRound;
	WORD	Position;

	CYHHashTable<GTPlayerInfo>	PlayerList;

	void ClearPlayerList()
	{
		GTPlayerInfo* pPlayerInfo = NULL;

		PlayerList.SetPositionHead();
		while((pPlayerInfo = PlayerList.GetData())!= NULL)
		{
			if(pPlayerInfo)
				delete pPlayerInfo;

			pPlayerInfo = NULL;
		}

		PlayerList.RemoveAll();
	}

	void ResetPlayerScore()
	{
		GTPlayerInfo* pPlayerInfo = NULL;

		PlayerList.SetPositionHead();
		while((pPlayerInfo = PlayerList.GetData())!= NULL)
		{
			if(pPlayerInfo)
				pPlayerInfo->dwScore = 0;
		}
	}

	GTInfo() {PlayerList.Initialize(20);}
	~GTInfo() {ClearPlayerList();}
};


class CGuildTournamentMgr
{
	GTGeneralInfo			m_GeneralInfo;						// 토너먼트 정보
	CYHHashTable<GTInfo>	m_pEntryList;						// 토너먼트 등록 길드
	BATTLE_INFO_GTOURNAMENT	m_BattleInfo[MAXGUILD_INTOURNAMENT];// 배틀생성을 위한 정보
	DWORD					m_InitBattleTable[MAXGUILD_INTOURNAMENT];	// 최초 배틀대진표
	DWORD					m_CurBattleTable[MAXGUILD_INTOURNAMENT];	// 현재 배틀대진표

	BOOL					m_bIgnoreSchedule;					// 수동으로 진행중
	BOOL					m_bStartGame;						// 게임시작했나?
	BOOL					m_bShuffleBattleTable;				// 대진표 혼합했나?
	BOOL					m_bNoMatchRound;					// 이번라운드 유효경기가 없음.

	WORD					m_wTournamentCount;					// 토너먼트 개최 수
	WORD					m_wCurBattleCount;					// 이 라운드에 생성된 경기수
	WORD					m_wInTournamentTeam;				// 이 라운드에 경기중인 팀
	WORD					m_wEndTournamentTeam;				// 이 라운드에 경기종료된 팀

	WORD					m_wCurRound;						// 현재 열리는 라운드 16강->8강->4강->결승
	WORD					m_wMaxRound;						// 전체 라운드수
	WORD					m_wInitRound;						// 시작 라운드수 16강=1라운드, 8강=2라운드
	WORD					m_wTournamentState;					// 현재 경기의 상태 (참가신청전, 참가신청중, 입장전, 입장중, 진행중, 마침)
	
	DWORD					m_dwRemainTime;						// 현재 프로세스를 즉시처리하지 않고 이시간만큼 대기한 후 처리
	DWORD					m_dwLastNotifyTime;					// 이전에 공지를 한 시간.


	// 재부팅시 세팅에 필요한 변수
	BOOL					m_bNeedAdjust;
	WORD					m_wLastState;
	WORD					m_wLastRound;
	DWORD					m_dwWaitingFlag;
	DWORD					m_dwWaitingForDBLoad;
	

public:
	MAKESINGLETON( CGuildTournamentMgr );

	CGuildTournamentMgr();
	virtual ~CGuildTournamentMgr();

	void Init();
	void ReadyToNewTournament();
	void ReadyToNewMatch();
	void Process();

	void Process_BeforeRegist();
	void Process_Regist();
	void Process_BeforeEntrance();
	void Process_Entrance();
	void Process_Process();
	void Process_Leave();
	
	void PlayerRegist(CGuild* pGuild, DWORD* pPlayerList);
	DWORD RegistGuild( CGuild* pGuild);
	DWORD ForceRegistGuild( CGuild* pGuild, DWORD Round, DWORD Position );

	void MakeBattleTable();
	void CreateBattle();
	void StartTournament();

	void MoveToBattleMap_Syn(DWORD dwConnectionIndex, void* pMsg);
	void NetworkMsgParse( DWORD dwConnectionIndex, BYTE Protocol,void* pMsg );

	int GetBattleID( CGuild* pGuild );
	void SetResult( DWORD GuildIdx, BOOL bWin, BOOL bUnearnedWin=FALSE );

	void FillLastGTInfo(DWORD dwGuildIdx, WORD wPosition, WORD wRound);
	void FillPlayerInfo(WORD wGTCount, DWORD dwGuildIdx, DWORD* pPlayers);
	GTInfo* GetGTEntryInfo(DWORD dwGuildIdx) {return m_pEntryList.GetData(dwGuildIdx);}
	void RemoveGTEntryInfo(DWORD dwGuildIdx);// {m_pEntryList.Remove(dwGuildIdx);}

	DWORD GetRespawnByLevel(int nLevel);
	DWORD GetImmortalTimeByClass(WORD wClassCode);
	WORD GetTournamentCount()				{return m_wTournamentCount;}
	void SetTournamentCount(WORD wCount)	{m_wTournamentCount = wCount;}
	WORD GetTournamentState()				{return m_wTournamentState;}
	void SetTournamentState(WORD wState)	{m_wTournamentState = wState;}
	void SetCurRound(WORD wRound)			{m_wCurRound = wRound;}
	// 081027 LUJ, 현재 라운드 반환
	WORD GetCurRound() const				{return m_wCurRound;};

	void SetLastState(WORD wState)			{m_wLastState = wState;}
	void SetLastRound(WORD wRound)			{m_wLastRound = wRound;}

	void SetPlayerScore(DWORD dwGuildIdx, DWORD dwPlayerIdx, DWORD dwScore);
	DWORD GetPlayerScore(DWORD dwGuildIdx, DWORD dwPlayerIdx);

	DWORD GetFeePoint() {return m_GeneralInfo.dwFee[0];}
	DWORD GetFeeMoney() {return m_GeneralInfo.dwFee[1];}
	WORD GetGoal()		{return m_GeneralInfo.wPlayRule[1];}
	WORD GetInitRound() {return m_wInitRound;}
	void GetPlayTypeObject(DWORD& dwType, DWORD& dwObject);

	void SetStateToRegist(WORD wTournamentCount);
	WORD AdjustedState();

	// SendToMsg
	void SendReward(WORD wRank, DWORD dwGuildIdx);
	void Notify_Send2Guild(DWORD dwGuildIdx, DWORD dwNotifyType, DWORD dwParam1, DWORD dwParam2=0, DWORD dwParma3=0);
	void Notify_Send2GTPlayer(DWORD dwGuildIdx, int nNotifyType, DWORD dwParam1, DWORD dwParam2=0, DWORD dwParma3=0);
	void Notify_Send2AllGTPlayer(int nNotifyType, DWORD dwParam1, DWORD dwParam2=0, DWORD dwParma3=0);
	void Notify_Send2AllUser(int nNotifyType, DWORD dwParam1, DWORD dwParam2=0);

	// Utility
	void GetTimeFromGTTime(DWORD dwSrcTime, WORD& wD, WORD& wH, WORD& wM);
	DWORD GetGTTimeOP(DWORD dwSrcTime, DWORD dwOPTime, BYTE OPType);
	DWORD GetRemaindTime(DWORD dwDestTime);

	void AddWaitingFlag(DWORD dwWait)		{m_dwWaitingFlag |= dwWait;}
	void RemoveWiatingFlag(DWORD dwWait)	{m_dwWaitingFlag &= ~dwWait;}
	void SetNeedAdjust(BOOL bVal)			{m_bNeedAdjust = bVal;}


	// 아래 함수 사용시 반드시 함수 상호간의 순서에 주의해야함.
	// 1)IsRegistDay() , 2)IsBeforeEntrace(), 3)IsBeforeRegist() 의 순서대로 사용되어야 합니다.
	BOOL IsRegistDay();
	BOOL IsBeforeEntrace();
	BOOL IsBeforeRegist();
};

#endif // !defined(AFX_GUILDTOURNAMENTMGR_H__21A71231_1238_4D1F_99B6_D53A80C23436__INCLUDED_)

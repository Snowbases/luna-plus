#pragma once
#include "stdafx.h"

#include "./EFFECT/EffectManager.h"

#define FISHINGMGR USINGTON(CFishingManager)

extern DWORD g_FishingMissionCode[MAX_FISHINGMISSIONCODE];
extern char  g_FishingExpGrade[MAX_FISHING_LEVEL][32];

class CFishingManager
{
public:
	CFishingManager(void);
	virtual ~CFishingManager(void);

	void Init();
	void NetworkMsgParse(BYTE Protocol,void* pMsg);

	void Fishing_Ready_Ack(void* pMsg);
	void Fishing_Ready_Nack(void* pMsg);
	void Fishing_GetFish_Ack(void* pMsg);
	void Fishing_GetFish_Nack(void* pMsg);
	void Fishing_Mission_Update(void* pMsg);
	void Fishing_Mission_Reward(void* pMsg);
	void Fishing_Exp_Ack(void* pMsg);
	void Fishing_Point_Ack(void* pMsg);
	void Fishing_LevelUp_Ack(void* pMsg);
	void Fishing_FPChange_Ack(void* pMsg);
	void Fishing_FPChange_Nack(void* pMsg);

	void SendFishing_Ready(DWORD dwBaitIdx, POSTYPE BaitPos);
	void SendFishing_Cancel();

	void Process();

protected:
	BOOL  m_bActive;						// 낚시활성 플래그
	BOOL  m_bPulling;						// 낚시질 플래그
	DWORD m_dwFishingPlaceIdx;				// 낚시터NPC Instance Index
	DWORD m_dwFishingStartTime;				// 시작시간

	BOOL  m_bUseMission;

	WORD  m_wFishingLevel;
	DWORD m_dwFishingExp;
	DWORD m_dwFishPoint;

	// 게이지 제어관련
	float m_fGaugeStartPos;					// 게이지 시작위치
	float m_fGaugeBarSpeed;					// 게이지 이동속도
	int   m_nRepeatCount;					// 게이지 반복횟수
	DWORD m_dwProcessTime;					// 게이지 진행시간
	DWORD m_dwGaugeStartTime;				// 게이지 이동시작시간


	DWORD m_dwUpdateTime;					// 게이지 갱신시간(ms단위)

	int	  m_nGaugeBarDir;

public:
	void FishingInfoClear();				// 낚시관련정보 한방에 클리어.

	DWORD GetItemIdxFromFM_Code(int nCode);	// 낚시미션 코드로 아이템Index얻기

	BOOL IsActive()											{return m_bActive;}
	BOOL IsPulling()										{return m_bPulling;}
	BOOL IsMission()										{return m_bUseMission;}

	void SetFishingPlace(DWORD dwIndex)						{m_dwFishingPlaceIdx = dwIndex;}
	DWORD GetFishingPlace()									{return m_dwFishingPlaceIdx;}

	void SetFishingStartTime(DWORD dwStartTime)				{m_dwFishingStartTime = dwStartTime;}
	DWORD GetFishingStartTime()								{return m_dwFishingStartTime;}

	int GetGaugeRepeatCount()								{return m_nRepeatCount;}
	DWORD GetUpdateTime()									{return m_dwUpdateTime;}

	float GetGaugeBarSpeed()								{return m_fGaugeBarSpeed;}
	int   GetGaugeBarDir()									{return m_nGaugeBarDir;}
	void  SetReverseDir()									{m_nGaugeBarDir *= -1;}

	WORD  GetFishingLevel()									{return m_wFishingLevel;}
	DWORD GetFishingExp()									{return m_dwFishingExp;}
	DWORD GetFishPoint()									{return m_dwFishPoint;}
};

EXTERNGLOBALTON(CFishingManager)
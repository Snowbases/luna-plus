// BossRewardsManager.cpp: implementation of the CBossRewardsManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BossRewardsManager.h"
#include "MHFile.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBossRewardsManager::CBossRewardsManager()
{
	m_BossRewardsTable.Initialize(40);
}

CBossRewardsManager::~CBossRewardsManager()
{
	Release();
}

void CBossRewardsManager::Release()
{
	m_BossRewardsTable.SetPositionHead();
	BOSSREWARDS * pInfo;
	while((pInfo = m_BossRewardsTable.GetData())!= NULL)
	{
		delete pInfo;
	}
	m_BossRewardsTable.RemoveAll();
}

void CBossRewardsManager::LoadBossRewardsInfo()
{
	CMHFile file;	

	if(!file.Init("System/Resource/BossReward.bin", "rb"))
	{
		ASSERTMSG(0, "Not found BossReward.bin file!");
		return ;
	}

	WORD MonsterKind;
	while(1)
	{
		if(file.IsEOF() != FALSE)
			break;
		
		BOSSREWARDS * pInfo = new BOSSREWARDS;
		MonsterKind = file.GetWord();
		pInfo->MaxExpPoint = file.GetDword();
		file.GetDword();
		pInfo->MaxMoney = file.GetDword();
		pInfo->EventRewardItem = file.GetDword();

		m_BossRewardsTable.Add(pInfo, MonsterKind);
	}

	file.Release();
}

void CBossRewardsManager::GetBossRewardsInfo(WORD MonsterKind, EXPTYPE& rExp, MONEYTYPE& rMoney)
{
	BOSSREWARDS * pInfo = m_BossRewardsTable.GetData(MonsterKind);
	ASSERT(pInfo);
	if(pInfo)
	{
		rExp = pInfo->MaxExpPoint;
		rMoney = pInfo->MaxMoney;
	}
}

DWORD CBossRewardsManager::GetBossEventRewardItem(WORD MonsterKind)
{
	BOSSREWARDS * pInfo = m_BossRewardsTable.GetData(MonsterKind);
	if(pInfo)
		return pInfo->EventRewardItem;

	return 0;
}

EXPTYPE CBossRewardsManager::GetExpOfBossMonster(WORD MonsterKind)
{
	BOSSREWARDS * pInfo = NULL ;
	pInfo = m_BossRewardsTable.GetData(MonsterKind) ;

	ASSERT(pInfo) ;

	if(!pInfo) return 0 ;

	return pInfo->MaxExpPoint ;
}

MONEYTYPE CBossRewardsManager::GetMoneyOfBossMonster(WORD MonsterKind)
{
	BOSSREWARDS * pInfo = NULL ;
	pInfo = m_BossRewardsTable.GetData(MonsterKind) ;

	ASSERT(pInfo) ;

	if(!pInfo) return 0 ;

	return pInfo->MaxMoney ;
}
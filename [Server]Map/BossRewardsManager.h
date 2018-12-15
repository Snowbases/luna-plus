// BossRewardsManager.h: interface for the CBossRewardsManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOSSREWARDSMANAGER_H__101AE7AB_7992_493A_B636_1B3E323F55B0__INCLUDED_)
#define AFX_BOSSREWARDSMANAGER_H__101AE7AB_7992_493A_B636_1B3E323F55B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BOSSREWARDSMGR CBossRewardsManager::GetInstance()
struct BOSSREWARDS
{
	EXPTYPE MaxExpPoint;
	MONEYTYPE MaxMoney;

	DWORD	EventRewardItem;	// 081030 NYJ - 보스를 타격한 모든사람에게 주는 보상아이템 수량은 1개로 제한.
};

class CBossRewardsManager  
{
	CBossRewardsManager();
	CYHHashTable<BOSSREWARDS> m_BossRewardsTable;
public:	
	GETINSTANCE(CBossRewardsManager);
	
	virtual ~CBossRewardsManager();

	void Release();
	void LoadBossRewardsInfo();
	void GetBossRewardsInfo(WORD MonsterKind, EXPTYPE&, MONEYTYPE&);
	DWORD GetBossEventRewardItem(WORD MonsterKind);

	// 080212 LYW --- BossRewardsManager : 경험치와 돈을 리턴하는 함수 추가.
	EXPTYPE GetExpOfBossMonster(WORD MonsterKind) ;
	MONEYTYPE GetMoneyOfBossMonster(WORD MonsterKind) ;
};

#endif // !defined(AFX_BOSSREWARDSMANAGER_H__101AE7AB_7992_493A_B636_1B3E323F55B0__INCLUDED_)

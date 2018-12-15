// LootingManager.h: interface for the CLootingManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOOTINGMANAGER_H__4CC47E46_1F53_463C_8A19_C021E23BCAF2__INCLUDED_)
#define AFX_LOOTINGMANAGER_H__4CC47E46_1F53_463C_8A19_C021E23BCAF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define LOOTINGMGR CLootingManager::GetInstance()

class CPlayer;
class CLootingRoom;

class CLootingManager  
{
protected:
	CYHHashTable<CLootingRoom>		m_htLootingRoom;
	// 090923 ONS 메모리풀 교체
	CPool<CLootingRoom>*			m_mpLootingRoom;

	CYHHashTable<CLootingRoom>		m_htDeleteRoom;

public:
	CLootingManager();
	virtual ~CLootingManager();
	MAKESINGLETON( CLootingManager );

	BOOL IsLootingSituation( CPlayer* pDiePlayer, CPlayer* pAttackPlayer );

	void CreateLootingRoom( CPlayer* pDiePlayer, CPlayer* pAttackPlayer );
	CLootingRoom* GetLootingRoom( DWORD dwDiePlayer );
	BOOL IsLootedPlayer( DWORD dwPlayerIdx );

	void Loot( DWORD dwDiePlayer, int nArrayNum );
	int GetLootingChance( CPlayer* pDiePlayer, CPlayer* pAttackPlayer );
	int GetLootingItemNum( CPlayer* pDiePlayer, CPlayer* pAttackPlayer );
	int GetWearItemLootingRatio( DWORD dwBadFame );

	void CloseLootingRoom( DWORD dwDiePlayer, BOOL bMsgToClient );

	void DiePlayerExitPanelty( CPlayer* pPlayer );
	void LootingCancel( CPlayer* pPlayer );

	void ProcessTimeOutCheck();

	//자동루팅 (for guildfieldwar)
	int AutoLooting( CPlayer* pDiePlayer, CPlayer* pAttackPlayer );

};

#endif // !defined(AFX_LOOTINGMANAGER_H__4CC47E46_1F53_463C_8A19_C021E23BCAF2__INCLUDED_)

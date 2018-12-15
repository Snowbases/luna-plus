// ItemDrop.cpp: implementation of the CItemDrop class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemDrop.h"
#include "Player.h"
#include "time.h"
#include "ItemManager.h"
#include "Monster.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "MHFile.h"
#include "PartyManager.h"
#include "Party.h"
#include "UserTable.h"

// 080212 LYW --- ItemDrop : Include Boss rewards manager.
#include "BossRewardsManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemDrop::CItemDrop()
{}

CItemDrop::~CItemDrop()
{}

MONEYTYPE CItemDrop::DropMoney(const BASE_MONSTER_LIST& baseMonsterList, int nPenaltyType)
{
	MONEYTYPE ResultMoney	= 0;											// 획득 머니.
	MONEYTYPE MaxMoney		= 0 ; 

	if( baseMonsterList.ObjectKind == eObjectKind_BossMonster )				// 보스 몬스터이면, 
	{
		WORD wMonsterKind = 0 ;
		wMonsterKind = baseMonsterList.MonsterKind ;

		MaxMoney = BOSSREWARDSMGR->GetMoneyOfBossMonster(wMonsterKind) ;	// 보스 몬스터 머니를 받는다.
	}
	else																	// 일반 몬스터이면,
	{
		MaxMoney = baseMonsterList.dropMoney ;								// 몬스터가 드랍할 수 있는 최대 머니를 받는다.
	}

	DWORD dwDropRateValue = baseMonsterList.dwMoneyDropRate ;				// 몬스터의 머니 드랍률을 받는다.

	WORD wDropRate1 = WORD( random(0, 10000) ) ;									// 뽑기 확률1을 구한다.
	WORD wDropRate2 = WORD( random(0, 10000) ) ;									// 뽑기 확률2를 구한다.

	DWORD dwDropRate = (wDropRate1 * 10000) + wDropRate2 ;					// 억단위 뽑기 확률을 구한다.

	float pickupPercent = random(50.0f, 100.0f) ;							// 획득할 수 있는 확률을 구한다.
	float fObtainMoney = 0.0f ;												// 획득할 머니를 임시로 담을 변수.

	if( dwDropRateValue >= dwDropRate)										// 몬스터 드랍 확률과 뽑기 확률 비교.
	{
		fObtainMoney = MaxMoney * (pickupPercent/100) ;						// 획득 머니를 세팅한다.
	}

	ResultMoney = (DWORD)fObtainMoney ;

	// 패널티 3에대해서만 처리하도록 한다.
	if( nPenaltyType == 3 )
	{
		ResultMoney = 1 ;
	}

	return ResultMoney;
}

BOOL CItemDrop::DropItem(WORD MonsterKind, const MONSTER_DROPITEM& dropItem, CPlayer* pPlayer, int nPenaltyType)
{
	if(0 == pPlayer)
	{
		return FALSE;
	}

	if( dropItem.wItemIdx == 0 ) return FALSE ;								// 드랍 아이템의 인덱스를 체크한다.

	if( gEventRate[eEvent_ItemRate] < 0 )										// 혹시라도 음수가 들어오면, 
	{
		gEventRate[eEvent_ItemRate] = 1 ;										// 아래 확률이 큰일난다.
	}

	DWORD dwDropRateValue = 0 ;
	dwDropRateValue = (DWORD)(gEventRate[eEvent_ItemRate] * dropItem.dwDropPercent) ;	// 드랍률 변경.

	// 071204 -- KTH 드랍율 상승 Buff 적용
	const float fIncreaseDropRate = (pPlayer->GetRateBuffStatus()->IncreaseDropRate + pPlayer->GetRatePassiveStatus()->IncreaseDropRate) / 100.f;
	dwDropRateValue += DWORD( dwDropRateValue * fIncreaseDropRate );

	// stdlib의 rand 함수는 최대 값이 32767까지 나오기 때문에, random 함수를 쓰기로 한다.

	DWORD dwDropRate1 = random(0, 99) ;										// 앞자리 뽑기 확률을 구한다.
	DWORD dwDropRate2 = random(0, 99) ;										// 뒷자리 뽑기 확률을 구한다.
	DWORD dwDropRate3 = random(0, 99) ;										// 뒷자리 뽑기 확률을 구한다.
	DWORD dwDropRate4 = random(0, 99) ;										// 뒷자리 뽑기 확률을 구한다.

	DWORD dwDropRate = (dwDropRate1 * 1000000) + (dwDropRate2 * 10000) + (dwDropRate3 * 100) + dwDropRate4 ;					// 억단위 뽑기 확률을 구한다.

	if( dwDropRateValue > dwDropRate)											// 드랍 확률과 뽑기 확률을 비교한다.
	{
		ITEM_INFO * pItemInfo	= ITEMMGR->GetItemInfo( dropItem.wItemIdx );
		if(pItemInfo)
		{
			// 고급아이템은 랜덤분배~
			CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx());
			if( pParty && pItemInfo->kind > pParty->GetDiceGradeOpt() )
			{
				DWORD dwItemGetPlayerID = PARTYMGR->GetRandomMember(pPlayer->GetPartyIdx());
				if(dwItemGetPlayerID)
				{
					CPlayer* pItemGetPlayer = (CPlayer*)g_pUserTable->FindUser(dwItemGetPlayerID);
					if(pItemGetPlayer)
					{
						ITEMMGR->MonsterObtainItem(pItemGetPlayer, dropItem.wItemIdx, MonsterKind, (WORD)dropItem.byCount);
						PARTYMGR->SendObtainItemByDiceMsg(pItemGetPlayer, dropItem.wItemIdx, dropItem.byCount);
					}
				}
				return TRUE;
			}
		}

		ITEMMGR->MonsterObtainItem(pPlayer, dropItem.wItemIdx, MonsterKind, (WORD)dropItem.byCount);

		return TRUE ;
	}

	return FALSE ;
}
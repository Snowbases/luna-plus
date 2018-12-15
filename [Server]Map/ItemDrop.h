#pragma once

#define ITEMDROP_OBJ CItemDrop::GetInstance()

struct MONSTER_ITEM
{
#ifdef _DEBUG
	char Name[MAX_ITEMNAME_LENGTH+1];			// 아이템 이름
#endif
	DWORD wItemIdx;							// 해당 몬스터가 사망했을 때 드랍하는 아이템 - "0"값 입력 시 드랍 아이템 없음
	DWORD dwDropPercent;					// 해당몬스터가 사망했을 때 드랍하는 아이템의 드랍 확률 - 박만분율 수치로 입력 예) 75000 = 7.5%
	BYTE byCount ;							// 해당 몬스터가 사망했을 때 드랍하는 아이템의 최대 개수 1~최대개수 랜덤하게 드랍.
};

#define MAX_DROP_ITEM_PERCENT 10000

struct MONSTER_DROP_ITEM
{
	WORD			wItemDropIndex;
	WORD			wTotalNum;
	DWORD			dwCurTotalRate;
	DWORD			dwCurMonsterDropItemPercent[MAX_DROPITEM_NUM];
	MONSTER_ITEM	MonsterDropItemPercent[MAX_DROPITEM_NUM];
};

class CItemDrop  
{	
public:
	GETINSTANCE(CItemDrop);
 	
	CItemDrop();
	virtual ~CItemDrop();
	MONEYTYPE DropMoney(const BASE_MONSTER_LIST&, int nPenaltyType);
	BOOL DropItem(WORD MonsterKind, const MONSTER_DROPITEM&, CPlayer*, int nPenaltyType);
};
#pragma once


#include "MunpaWareSlot.h"
#include "Purse.h"

class CGuildWarehouse : public CMunpaWareSlot  
{
	cPtrList m_WarehousePlayerList[TAB_GUILDWAREHOUSE_NUM]; //창고 보고 있는 사람들
	ITEMBASE	m_TotalItemBase[SLOT_GUILDWAREHOUSE_NUM];
	SLOTINFO	m_TotalSlotInfo[SLOT_GUILDWAREHOUSE_NUM];
	void AddWarehousePlayer(DWORD PlayerID, BYTE TabNum);
	void RemoveWarehousePlayer(DWORD PlayerID, BYTE TabNum);
	void SendtoWarehousePlayer(BYTE TabNum, MSGBASE* msg, int size, DWORD ExceptID=0);
	
	friend class CMunpaWarePurse;
	CPurse m_GuildWarePurse;
	
public:
	CGuildWarehouse();
	virtual ~CGuildWarehouse();
	virtual ERROR_ITEM InsertItemAbs(CPlayer*, POSTYPE, ITEMBASE* pItem, WORD state=SS_NONE);
	virtual ERROR_ITEM DeleteItemAbs(CPlayer*, POSTYPE, ITEMBASE* pItemOut, WORD state=SS_NONE);
	virtual BOOL IsEmpty(POSTYPE);
	virtual ITEMBASE* GetItemInfoAbs(POSTYPE);
	virtual ERROR_ITEM UpdateItemAbs(CPlayer * pPlayer, POSTYPE whatAbsPos, DWORD dwDBIdx, WORD wItemIdx, POSTYPE position, POSTYPE quickPosition, DURTYPE Dur, WORD flag=UB_ALL, WORD state=SS_NONE);
	void Init(MONEYTYPE);
	BOOL InitGuildItem(ITEMBASE*);
	MONEYTYPE SetMoney(MONEYTYPE , BYTE bOper, BYTE MsgFlag);
	MONEYTYPE GetMaxMoney();
	BOOL IsEmptyAndMoney();
	void AddItemInfoWaitingMember(CPlayer*, BYTE TabNum);
};
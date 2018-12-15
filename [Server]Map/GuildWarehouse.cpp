#include "stdafx.h"
#include "GuildWarehouse.h"
#include "Player.h"
#include "UserTable.h"
#include "ItemManager.h"
#include "GuildManager.h"


CGuildWarehouse::CGuildWarehouse()
{
}

CGuildWarehouse::~CGuildWarehouse()
{
	for(int i=0;i<TAB_GUILDWAREHOUSE_NUM;++i)
	{
		m_WarehousePlayerList[i].RemoveAll();
	}

	m_GuildWarePurse.Release();
}

void CGuildWarehouse::Init(MONEYTYPE GuildMoney)
{
	ZeroMemory( m_TotalItemBase, sizeof( m_TotalItemBase ) );
	ZeroMemory( m_TotalSlotInfo, sizeof( m_TotalSlotInfo ) );

	CMunpaWareSlot::Init(TP_GUILDWAREHOUSE_START, SLOT_GUILDWAREHOUSE_NUM, m_TotalItemBase, m_TotalSlotInfo);
	CreatePurse(&m_GuildWarePurse, this, GuildMoney, 4000000000);
}

void CGuildWarehouse::AddWarehousePlayer(DWORD PlayerID, BYTE TabNum)
{
	// 같은 아이디가 있으면 추가하지 않는다.
	PTRLISTPOS pos = m_WarehousePlayerList[TabNum].GetHeadPosition();
	while(pos)
	{
		DWORD PID = (DWORD)m_WarehousePlayerList[TabNum].GetAt(pos);
		if(PID == PlayerID)
		{
			return;
		}

		m_WarehousePlayerList[TabNum].GetNext(pos);
	}

	m_WarehousePlayerList[TabNum].AddTail((void*)PlayerID);	
}

void CGuildWarehouse::RemoveWarehousePlayer(DWORD PlayerID, BYTE TabNum)
{
	PTRLISTPOS pos = m_WarehousePlayerList[TabNum].GetHeadPosition();
	while(pos)
	{
		DWORD PID = (DWORD)m_WarehousePlayerList[TabNum].GetAt(pos);
		if(PID == PlayerID)
		{
			m_WarehousePlayerList[TabNum].RemoveAt(pos);
			break;
		}

		m_WarehousePlayerList[TabNum].GetNext(pos);
	}
}


void CGuildWarehouse::SendtoWarehousePlayer(BYTE TabNum, MSGBASE* msg, int size, DWORD ExceptID)
{
	PTRLISTPOS pos = m_WarehousePlayerList[TabNum].GetHeadPosition();
	while(pos)
	{
		DWORD PlayerID = (DWORD)m_WarehousePlayerList[TabNum].GetAt(pos);
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(PlayerID);
		if(!pPlayer)
			m_WarehousePlayerList[TabNum].RemoveAt(pos);
		else if(PlayerID != ExceptID)
			pPlayer->SendMsg(msg, size);
		m_WarehousePlayerList[TabNum].GetNext(pos);
	}
}

BOOL CGuildWarehouse::InitGuildItem(ITEMBASE* pInfo)
{
	return CMunpaWareSlot::InsertItemAbs(NULL, pInfo->Position,pInfo);
}

BOOL CGuildWarehouse::IsEmpty(POSTYPE absPos)
{
	return CMunpaWareSlot::IsEmpty(absPos);
}


ERROR_ITEM CGuildWarehouse::InsertItemAbs(CPlayer * pPlayer, POSTYPE absPos, ITEMBASE * pItem, WORD state)
{
	ERROR_ITEM rt = CMunpaWareSlot::InsertItemAbs(pPlayer, absPos,pItem, state);
	if(rt != EI_TRUE)
		return rt;
	if(pPlayer)
	{
		MSG_ITEM_WITH_OPTION msg;
		ZeroMemory( &msg, sizeof( msg ) );
		
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_ADDTO_GUILDWARE_NOTIFY;
		msg.ItemInfo = *pItem;
		msg.OptionInfo	= ITEMMGR->GetOption( *pItem );

		BYTE TabNum = CMunpaWareSlot::GetTabNum(absPos);
		SendtoWarehousePlayer(TabNum, &msg, sizeof( msg ), pPlayer->GetID());
	}
	return rt;
}

ERROR_ITEM CGuildWarehouse::DeleteItemAbs(CPlayer * pPlayer, POSTYPE absPos, ITEMBASE * pItemOut, WORD state)
{
	ERROR_ITEM rt = CMunpaWareSlot::DeleteItemAbs(pPlayer, absPos,pItemOut);
	if(rt != EI_TRUE)
		return rt;
	if(pPlayer)
	{
		MSG_ITEM msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_DELETEFROM_GUILDWARE_NOTIFY;
		msg.ItemInfo = *pItemOut;
		
		BYTE TabNum = CMunpaWareSlot::GetTabNum(absPos);
		SendtoWarehousePlayer(TabNum,&msg,sizeof(msg),pPlayer->GetID());
	}
	return rt;
}

ERROR_ITEM CGuildWarehouse::UpdateItemAbs(CPlayer * pPlayer, POSTYPE whatAbsPos, DWORD dwDBIdx, WORD wItemIdx, POSTYPE position, POSTYPE quickPosition, DURTYPE Dur, WORD flag, WORD state)
{
	return CMunpaWareSlot::UpdateItemAbs(pPlayer, whatAbsPos, dwDBIdx, wItemIdx, position, quickPosition, Dur, flag, state);
}

ITEMBASE* CGuildWarehouse::GetItemInfoAbs(POSTYPE absPos)
{
	if( !CMunpaWareSlot::IsPosIn(absPos) )	return NULL;

	return  &m_ItemBaseArray[absPos-m_StartAbsPos];	
}

MONEYTYPE CGuildWarehouse::GetMaxMoney()
{
	return m_pPurse->GetMaxMoney();
}

MONEYTYPE CGuildWarehouse::SetMoney( MONEYTYPE ChangeValue, BYTE bOper, BYTE MsgFlag )
{
	CPurse* pPurse = GetPurse();
	if( !pPurse) return FALSE;

	if( bOper == MONEY_SUBTRACTION )
	{
		return pPurse->Subtraction( ChangeValue, MsgFlag );
	}
	else
	{
		return pPurse->Addition( ChangeValue, MsgFlag );
	}

}

BOOL CGuildWarehouse::IsEmptyAndMoney()
{
	if( m_pPurse->GetPurseCurMoney() )	return FALSE;

	for( int i = 0; i < SLOT_GUILDWAREHOUSE_NUM; ++i )
	{
		if( m_TotalItemBase[i].dwDBIdx )
			return FALSE;
	}
	return TRUE;
}

void CGuildWarehouse::AddItemInfoWaitingMember( CPlayer* pPlayer, BYTE TabNum )
{
	AddWarehousePlayer(pPlayer->GetID(),TabNum);
}
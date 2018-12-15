#include "stdafx.h"
#include "MunpaWareSlot.h"
#include "ItemManager.h"
#include "GuildManager.h"
#include "Player.h"
//#include "MapDBMsgParser.h"


CMunpaWareSlot::CMunpaWareSlot()
{}


CMunpaWareSlot::~CMunpaWareSlot()
{}


void CMunpaWareSlot::Init(POSTYPE startAbsPos, POSTYPE slotNum, ITEMBASE * pInfo, SLOTINFO * pSlotInfo)
{
	CItemSlot::Init(startAbsPos, slotNum, pInfo, pSlotInfo);
}


ERROR_ITEM CMunpaWareSlot::InsertItemAbs(CPlayer * pPlayer, POSTYPE absPos, ITEMBASE * pItem, WORD state)
{
	if( !IsPosIn( absPos ) )
	{
		return EI_OUTOFPOS;
	}

	// 071212 LUJ CItemSlot::InsertItemAbs() 참조
	//if( !IsEmptyInner(absPos) )
	if( ! IsEmpty( absPos ) )
	{
		return EI_EXISTED;
	}
	else if( !( SS_LOCKOMIT & state ) && IsLock(absPos) )
	{
		return EI_LOCKED;
	}
	else if((state & SS_PREINSERT) == FALSE)
	{
		// 091215 ONS 아이템Stack최대수를 아이템별로 지정된 값으로 처리한다.
		const WORD ItemStackNum = ITEMMGR->GetItemStackNum( m_ItemBaseArray[absPos - m_StartAbsPos].wIconIdx );
		if( ITEMMGR->IsDupItem(m_ItemBaseArray[absPos - m_StartAbsPos].wIconIdx) && m_ItemBaseArray[absPos - m_StartAbsPos].Durability > ItemStackNum )
			return EI_NOTEQUALDATA;
	}
	//////////////////////////////////////////////////////////////////////////

	pItem->Position = absPos;

	m_ItemBaseArray[absPos - m_StartAbsPos] = *pItem;

	//---KES ItemDivide Fix 071020
	//---DBIDX체크 추가
	m_SlotInfoArray[absPos - m_StartAbsPos].wState = ( state & ~(SS_LOCKOMIT|SS_CHKDBIDX) );
	//----------------------------

	m_SlotInfoArray[absPos - m_StartAbsPos].bLock = FALSE;

	return EI_TRUE;
}

ERROR_ITEM CMunpaWareSlot::UpdateItemAbs(CPlayer * pPlayer, POSTYPE whatAbsPos, DWORD dwDBIdx, WORD wItemIdx, POSTYPE position, POSTYPE quickPosition, DURTYPE Dur, WORD flag, WORD state)
{
	if( !IsPosIn(whatAbsPos) )	return EI_OUTOFPOS;
	if( (flag & UB_ABSPOS) && (position != whatAbsPos) )
	{
		//YH
		//return EI_NOTEQUALDATA;
		position = whatAbsPos;
	}
	if( !( SS_LOCKOMIT & state ) && IsLock(whatAbsPos) )  return EI_LOCKED;
	
	ITEMBASE& item = m_ItemBaseArray[whatAbsPos - m_StartAbsPos];

	if(flag & UB_ICONIDX)
		item.wIconIdx = wItemIdx;
	if(flag & UB_QABSPOS)
		item.QuickPosition = quickPosition;
	if(flag & UB_ABSPOS)
		item.Position = position;

	//////////////////////////////////////////////////////////////////////////
	//itemmaxcheck
	// 091215 ONS 아이템Stack최대수를 아이템별로 지정된 값으로 처리한다.
	const WORD wItemStackNum = ITEMMGR->GetItemStackNum( item.wIconIdx );
	if( ITEMMGR->IsDupItem(item.wIconIdx) && item.Durability > wItemStackNum )
		return EI_NOTEQUALDATA;
	//////////////////////////////////////////////////////////////////////////

	if(flag & UB_DURA)
		item.Durability = Dur;

	//---KES ItemDivide Fix 071020
	//---DBIDX체크 추가
	m_SlotInfoArray[whatAbsPos - m_StartAbsPos].wState = ( state & ~(SS_LOCKOMIT|SS_CHKDBIDX) );
	//----------------------------

	m_SlotInfoArray[whatAbsPos - m_StartAbsPos].bLock = FALSE;

	return EI_TRUE;
}


ERROR_ITEM CMunpaWareSlot::DeleteItemAbs(CPlayer * pPlayer, POSTYPE absPos, ITEMBASE * pItemOut, WORD state)
{
	if( !IsPosIn(absPos) )
	{
		return EI_OUTOFPOS;
	}
	//else if( IsEmptyInner(absPos) )
	else if( IsEmpty( absPos ) )
	{
		return EI_NOTEXIST;
	}
	else if(	SS_LOCKOMIT != state &&
		IsLock(absPos) )
	{
		return EI_LOCKED;
	}

	ITEMBASE& item = m_ItemBaseArray[ absPos - m_StartAbsPos ];

	if( pItemOut )
	{
		*pItemOut = item;
	}

	SLOTINFO& slot = m_SlotInfoArray[ absPos - m_StartAbsPos ];

	ZeroMemory( &item, sizeof( item ) );
	ZeroMemory( &slot, sizeof( slot ) );
	return EI_TRUE;
}


BYTE CMunpaWareSlot::GetTabNum(POSTYPE absPos)
{
	return BYTE((absPos - m_StartAbsPos) / TABCELL_GUILDWAREHOUSE_NUM);
}


void CMunpaWareSlot::GetItemInfoTab(BYTE TabNum, ITEMBASE* pRtItemArray)
{
	int StartPos = TABCELL_GUILDWAREHOUSE_NUM * TabNum;
	for(int n=0;n<TABCELL_GUILDWAREHOUSE_NUM;++n)
	{
		pRtItemArray[n] = m_ItemBaseArray[StartPos+n];
	}
}

BOOL CMunpaWareSlot::IsEmpty( POSTYPE absPos )
{
	return CItemSlot::IsEmpty( absPos - m_StartAbsPos );
}


BOOL CMunpaWareSlot::IsLock(POSTYPE absPos)
{
	return CItemSlot::IsLock( absPos - m_StartAbsPos );
}

void CMunpaWareSlot::SetLock(POSTYPE absPos, BOOL val)
{
	CItemSlot::SetLock( absPos - m_StartAbsPos, val );
}
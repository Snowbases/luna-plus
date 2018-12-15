// InventorySlot.cpp: implementation of the CInventorySlot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InventorySlot.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInventorySlot::CInventorySlot()
{
	m_ExtraSlotCount = 0;
}

CInventorySlot::~CInventorySlot()
{

}

WORD CInventorySlot::GetEmptyCell(WORD * EmptyCellPos, WORD NeedCount)
{
	if( NeedCount == 0 ) return 0;

	POSTYPE EndPos = m_StartAbsPos + m_SlotNum;
	WORD EmptyCellNum = 0;

	for( WORD i = m_StartAbsPos ; i < EndPos ; ++i )
	{
		//const ITEMBASE * pItemBase = GetItemInfoAbs(i);

		if( IsEmpty(i) )
		{
			if( EmptyCellPos )
				EmptyCellPos[EmptyCellNum++] = i;

			if( EmptyCellNum == NeedCount ) break;
		}
	}

	return EmptyCellNum;
}


BOOL CInventorySlot::CheckQPosForItemIdx( WORD ItemIdx )
{
	POSTYPE EndPos = m_StartAbsPos + m_SlotNum;
	
	for( WORD i = m_StartAbsPos ; i < EndPos ; ++i )
	{
		const ITEMBASE * pItemBase = GetItemInfoAbs(i);

		if( pItemBase->wIconIdx == ItemIdx && pItemBase->QuickPosition != 0 )
			return FALSE;
	}

	return TRUE;
}

BOOL CInventorySlot::CheckItemLockForItemIdx( WORD ItemIdx )
{
	POSTYPE EndPos = m_StartAbsPos + m_SlotNum;

	for( WORD i = m_StartAbsPos ; i < EndPos ; ++i )
	{
		const ITEMBASE * pItemBase = GetItemInfoAbs(i);

		if( pItemBase->wIconIdx == ItemIdx && IsLock(i) )
			return FALSE;
	}

	return TRUE;
}



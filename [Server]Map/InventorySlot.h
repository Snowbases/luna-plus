// InventorySlot.h: interface for the CInventorySlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVENTORYSLOT_H__33A85797_3371_4D1B_B3EA_5A442AD4D1A9__INCLUDED_)
#define AFX_INVENTORYSLOT_H__33A85797_3371_4D1B_B3EA_5A442AD4D1A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ItemSlot.h"

class CInventorySlot : public CItemSlot  
{
	DWORD		m_ExtraSlotCount;
	
public:
	CInventorySlot();
	virtual ~CInventorySlot();

	WORD GetEmptyCell(WORD * EmptyCellPos, WORD NeedCount = 1);
	BOOL CheckQPosForItemIdx( WORD ItemIdx );
	BOOL CheckItemLockForItemIdx( WORD ItemIdx );

};

#endif // !defined(AFX_INVENTORYSLOT_H__33A85797_3371_4D1B_B3EA_5A442AD4D1A9__INCLUDED_)

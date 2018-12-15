#pragma once


#include "ItemSlot.h"


class CWearSlot : public CItemSlot  
{
public:
	CWearSlot();
	virtual ~CWearSlot();
	virtual ERROR_ITEM UpdateItemAbs( CPlayer*, POSTYPE whatAbsPos, DWORD dwDBIdx, DWORD wItemIdx, POSTYPE position, POSTYPE quickPosition, DURTYPE Dur, WORD flag=UB_ALL, WORD state=SS_NONE);
	virtual ERROR_ITEM InsertItemAbs( CPlayer*, POSTYPE absPos, ITEMBASE*, WORD state=SS_NONE);
	virtual ERROR_ITEM DeleteItemAbs( CPlayer*, POSTYPE absPos, ITEMBASE*, WORD state=SS_NONE);

	
private:
	virtual BOOL IsAddable( POSTYPE, ITEMBASE* ) { return TRUE; }
	//void ApplyItem( CPlayer*);
};
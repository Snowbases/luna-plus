#pragma once


#include "interface/cTabDialog.h"


class CItem;
class cStatic;


class CGuildWarehouseDialog : public cTabDialog  
{
	BOOL m_Lock;
	BOOL m_InitInfo;
	USHORT	mTabSize;

public:
	CGuildWarehouseDialog();
	virtual ~CGuildWarehouseDialog();
	virtual void SetActive(BOOL val);
	virtual void Render();
	virtual void Add(cWindow * window);
	
	virtual BOOL FakeMoveIcon(LONG x, LONG y, cIcon * icon);
	void OnActionEvent(LONG lId, void * p, DWORD we);

	void Linking();
	BOOL FakeMoveItem( LONG mouseX, LONG mouseY, CItem * pFromItem );
	BOOL FakeGeneralItemMove( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem );
	BOOL GetPositionForXYRef(BYTE param, LONG x, LONG y, WORD& pos);
	CItem * GetItemForPos(POSTYPE absPos);
	
	BOOL	AddItem( const ITEMBASE* );
	BOOL	AddItem(CItem* pItem);
	BOOL	DeleteItem(POSTYPE Pos,CItem** ppItem);
	POSTYPE GetRelativePosition( POSTYPE absPos);
	WORD GetTabIndex( POSTYPE absPos );

	void SetLock(BOOL val);
	BOOL GetLock() const;
	void SetTabSize( USHORT size );

	void AddItem( const MSG_GUILD_ITEM* );
};
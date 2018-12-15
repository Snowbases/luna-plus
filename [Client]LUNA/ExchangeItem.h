#ifndef _EXCHANGEITEM_H
#define _EXCHANGEITEM_H


#include "./interface/cIcon.h"

class CItem;

class CExchangeItem : public cIcon
{

protected:
	POSTYPE	m_Position;
	POSTYPE	m_LinkPos;
	CItem* m_pLinkItem;
	DURTYPE m_Durability;
	DWORD m_dwDBId;
	RECT m_rect;
	
public:
	CExchangeItem();
	virtual ~CExchangeItem();
	void	InitItem( CItem* pItem, POSTYPE Pos, LONG id );
	 
	void	SetPosition( POSTYPE Pos )				{ m_Position = Pos; }
	POSTYPE GetPosition()							{ return m_Position; }
	POSTYPE GetLinkPos()							{ return m_LinkPos; }

	void	SetLinkItem( CItem* pItem )				{ m_pLinkItem = pItem; }
	CItem*	GetLinkItem()							{ return m_pLinkItem; }
	virtual void SetLock( BOOL bLock );

	inline DURTYPE	GetDurability()					{ return m_Durability; }
	inline void		SetDurability( DURTYPE dura )	{ m_Durability = dura; }
	
	void SetDBId( DWORD id )						{ m_dwDBId = id; }
	DWORD GetDBId()									{ return m_dwDBId; }

	char* GetItemName();

	virtual void Render();
};



#endif
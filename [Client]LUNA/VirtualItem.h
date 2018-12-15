#pragma once


#include "BaseItem.h"


class CVirtualItem : public CBaseItem  
{
public:
	CVirtualItem();
	virtual ~CVirtualItem();
	void Init();
	DWORD GetSrcItemIdx()			{ return m_pItemLink->GetItemIdx();		}
	POSTYPE GetSrcPosition()		{ return m_pItemLink->GetPosition();	}
//	eITEM_KINDBIT GetSrcItemKind();
	
	DWORD	GetRareness()			{   return m_pItemLink->GetRareness();	}
	DURTYPE	GetSrcDurability()		{	return m_pItemLink->GetDurability();	}
	void SetLinkItem(CBaseItem * pItem);
	CBaseItem * GetLinkItem()
	{
		return m_pItemLink;
	}
	virtual void Render();
	virtual void SetDurability( DURTYPE );

protected:
	CBaseItem * m_pItemLink;

private:
	DURTYPE	mDurability;
};
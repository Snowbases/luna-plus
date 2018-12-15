#pragma once


#include "INTERFACE\cIcon.h"

class cQuickItem;

class CBaseItem : public cIcon  
{
	cPtrList mQuickList;
public:
	CBaseItem();
	virtual ~CBaseItem();

	virtual inline DWORD	GetDBIdx(){ return 0; }
	virtual inline DWORD	GetItemIdx(){ return 0; }
	virtual inline POSTYPE	GetPosition(){ return 0; }
	virtual inline void		SetPosition(POSTYPE pos){}

	virtual inline DWORD	GetRareness()					{   return 0;	}
	virtual inline DURTYPE	GetDurability()					{	return 0;	}
	virtual inline void		SetDurability(DURTYPE dur)		{}

	
	virtual inline void		SetExpPoint(EXPTYPE point){}
	virtual inline EXPTYPE	GetExpPoint()			{	return 0;	}
	virtual inline void		SetSung(BYTE sung)		{}
	virtual inline BYTE		GetSung()				{	return 0;	}

	void AddQuick( cQuickItem* pQuick ) { mQuickList.AddTail( pQuick ); }
	void RemoveQuick( cQuickItem* pQuick ) { mQuickList.Remove( ( void* )pQuick ); }
	BOOL IsQuickLink() { return mQuickList.GetCount(); }
	void RemoveQuickLink();


	// 070714 웅주, 개수 표시 여부를 결정한다.
public:
	virtual BOOL IsVisibleDurability() const;
	virtual void SetVisibleDurability( BOOL );

protected:
	BOOL	mIsVisibleDurability;
};
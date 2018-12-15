#pragma once

#include "GameResourceManager.h"
#include "./BaseItem.h"


class CExchangeItem;


class CItem : public CBaseItem
{
protected:
	ITEMBASE m_ItemBaseInfo;
	
	POSTYPE			m_LinkPos;	//KES 030919 교환창 및 노점상 연결위치
	CExchangeItem*	m_pLinkItem;

	DWORD			m_Param;

//---KES 071202
	BOOL	m_bNoDeffenceTooltip;

public:
	BOOL m_bAdded;

	CItem( const ITEMBASE* );
	virtual ~CItem();

	virtual inline DWORD	GetDBIdx()					{	return m_ItemBaseInfo.dwDBIdx;	}
	virtual	void			SetDBIdx( DWORD itemDbIndex )	{ m_ItemBaseInfo.dwDBIdx = itemDbIndex; }
	virtual inline DWORD	GetItemIdx()				{	return m_ItemBaseInfo.wIconIdx;	}
	virtual inline POSTYPE	GetPosition()				{	return m_ItemBaseInfo.Position;	}
	virtual inline void		SetPosition(POSTYPE pos)	{	m_ItemBaseInfo.Position = pos;	}

	virtual inline DURTYPE	GetDurability()					{	return m_ItemBaseInfo.Durability;	}
	virtual inline void		SetDurability(DURTYPE dur)		{	m_ItemBaseInfo.Durability = dur;		}

//KES 030919 교환창 및 노점상 연결위치	
	inline POSTYPE GetLinkPosition() { return m_LinkPos; }
	inline void SetLinkPosition(POSTYPE pos)	{ m_LinkPos = pos; }
	void SetLinkItem( CExchangeItem* pItem )	{ m_pLinkItem = pItem; }
	CExchangeItem* GetLinkItem()				{ return m_pLinkItem; }

	const ITEMBASE&	GetItemBaseInfo() const { return m_ItemBaseInfo; }
	
	void SetItemBaseInfo( const ITEMBASE& );
	
	virtual void SetLock( BOOL bLock );
	

//	eITEM_KINDBIT GetItemKind();
	
	const ITEM_INFO* GetItemInfo();
	
	virtual void Render();
	
	virtual BOOL CanMoveToDialog( cDialog* ptargetdlg );
	virtual BOOL CanDelete();
	
	void SetItemParam(ITEMPARAM Param)		{ m_ItemBaseInfo.ItemParam = Param;	}
	DWORD GetItemParam()					{ return m_ItemBaseInfo.ItemParam;	}
	void SetUseParam( DWORD Param )			{ m_Param = Param;	}
	DWORD GetUseParam()						{ return m_Param;	}

	// 071124 LYW --- Item : 이벤트 함수 추가.
	virtual DWORD ActionEvent(CMouse* mouseInfo);

	// 071128 LYW --- Item : 마지막 체크 시간 설정 함수 추가.
	void SetLastCheckTime(DWORD dwTime) { m_ItemBaseInfo.LastCheckTime = dwTime ; }
	void SetRemainTime(int nRemainTime) { m_ItemBaseInfo.nRemainSecond = nRemainTime ; }

public:
	//---KES 071202
	BOOL IsNoDefenceTooltip() { return m_bNoDeffenceTooltip; }
	void SetDeffenceTooltip( BOOL bNoDeffenceTooltip ) { m_bNoDeffenceTooltip = bNoDeffenceTooltip; }
	void ForcedUnseal() { m_ItemBaseInfo.nSealed = eITEM_TYPE_SEAL_NORMAL; }
};
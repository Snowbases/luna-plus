#include "stdafx.h"
#include "ExchangeItem.h"
#include "interface/cFont.h"
#include "ItemManager.h"

CExchangeItem::CExchangeItem()
{
	m_pLinkItem	= NULL;
	m_LinkPos = POSTYPE(-1);
	m_Durability = 1;
	m_type = WT_EXCHANGEITEM;
}

CExchangeItem::~CExchangeItem()
{

}


void CExchangeItem::InitItem( CItem* pItem, POSTYPE Pos, LONG id )
{
	cImage tmpImage;
	Init( 0, 0, DEFAULT_ICONSIZE, DEFAULT_ICONSIZE,
		  ITEMMGR->GetIconImage( pItem->GetItemIdx(), &tmpImage ), id );

	m_pLinkItem = pItem;
	m_Position	= Pos;
	m_LinkPos	= pItem->GetPosition();
	SetIconType( eIconType_LinkedItem );
	SetData( pItem->GetItemIdx() );
	SetDurability( pItem->GetDurability() );

	ITEMMGR->AddToolTip( this );
}


void CExchangeItem::SetLock( BOOL bLock )
{
	cIcon::SetLock( bLock );
	SetMovable( !bLock );
}

void CExchangeItem::Render()
{
	if( m_bActive == FALSE ) return;
	
	cIcon::Render();

	if( ITEMMGR->IsDupItem( m_dwData ) )
	{
		static char nums[3];
		wsprintf(nums,"%2d", m_Durability);

		if( IsMovedWnd() )
		{
			if( m_Durability > 99 )
			{
				m_rect.left		= (LONG)m_absPos.x+16 ;
			}
			else
			{
				m_rect.left		= (LONG)m_absPos.x+22 ;
			}
			m_rect.top		= (LONG)m_absPos.y+23 ;
			m_rect.right	= 1 ;
			m_rect.bottom	= 1 ;
		}

		CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_rect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
	}

	if( IsMovedWnd() ) SetMovedWnd( FALSE ) ;
}

char* CExchangeItem::GetItemName()
{
	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(GetData());
	if(pItemInfo)
		return pItemInfo->ItemName;
	return NULL;
}

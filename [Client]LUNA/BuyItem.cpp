// BuyItem.cpp: implementation of the CBuyItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BuyItem.h"
#include "ItemManager.h"
#include "./Interface/cFont.h"
#include "ChatManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuyItem::CBuyItem()
{
	m_ItemIdx = 0;
	m_nVolume = 0;
	m_nMoney = 0;
	m_dwParam = 0;
	m_wSeal = 0;
}

CBuyItem::~CBuyItem()
{

}

void CBuyItem::InitItem(ITEMBASE ItemInfo, WORD Volume, DWORD Money)
{
	m_ItemIdx = ItemInfo.wIconIdx;
	m_nVolume = Volume;
	m_nMoney = Money;
	//m_dwParam = ItemInfo.ItemParam;
	// 080220 KTH -- 수정 봉인 정보를 넣어준다.
	m_dwParam = ItemInfo.nSealed;
	m_wSeal = WORD(ItemInfo.nSealed);

	if( ItemInfo.nSealed == eITEM_TYPE_SEAL )
		this->SetSeal(TRUE);
	else
		SetSeal(FALSE);

//	DWORD ItemKind = GetItemKind(m_ItemIdx);

	m_bVolume = ( ITEMMGR->GetItemInfo( m_ItemIdx ) )->Stack;//!(( ItemKind > eEQUIP_ITEM) && ( ItemKind < eEXTRA_ITEM) && !ITEMMGR->IsDupItem(m_ItemIdx));
			
	cImage tmpImage;
	Init( 0, 0, DEFAULT_ICONSIZE, DEFAULT_ICONSIZE,
		ITEMMGR->GetIconImage( ItemInfo.wIconIdx, &tmpImage ) );
	
	SetIconType( eIconType_LinkedItem );
	SetData( ItemInfo.wIconIdx );	

	ITEMMGR->AddToolTip( this );
	//ITEMMGR->AddToolTip(m_pItem);
}

void CBuyItem::Render()
{
	if( m_bActive == FALSE ) return;

	if( this->IsSeal() )
	{
		cDialog::Render();

		m_SealImage.RenderSprite( NULL, NULL, 0, &m_absPos, RGBA_MERGE(0xffffff, 255));
	}
	else
		cIcon::Render();

	if(m_bVolume && m_nVolume > 0)
	{
		static char nums[2];
		wsprintf(nums,"%2d", m_nVolume);
		if( IsMovedWnd() )
		{
			m_rect.left		= (LONG)m_absPos.x+27 ;
			m_rect.top		= (LONG)m_absPos.y+29 ;
			m_rect.right	= 1 ;
			m_rect.bottom	= 1 ;
		}
		CFONT_OBJ->RenderFont(0,nums,strlen(nums),&m_rect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));		// color hard coding : taiyo 
	}

	if( IsMovedWnd() ) SetMovedWnd( FALSE ) ;
}

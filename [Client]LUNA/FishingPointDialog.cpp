#include "stdafx.h"
#include "fishingpointdialog.h"
#include "./Interface/cIconGridDialog.h"
#include "WindowIDEnum.h"
#include "ItemManager.h"
#include "GameIn.h"
#include "FishingManager.h"
#include "InventoryExDialog.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"

CFishingPointDialog::CFishingPointDialog(void)
{
	m_dwAddFishPoint = 0;
}

CFishingPointDialog::~CFishingPointDialog(void)
{
}

void CFishingPointDialog::Linking()
{
	m_pIconFish = (cIconGridDialog*)GetWindowForID(FISHING_ICON_FISH);
	m_pIconFish->SetDisable(TRUE);
	m_pAddStatic = (cStatic*)GetWindowForID(FISHING_STATIC_ADDFISHPOINT);
	m_pAddStatic->SetDisable(TRUE);
	m_pCurStatic = (cStatic*)GetWindowForID(FISHING_STATIC_CURFISHPOINT);
	m_pCurStatic->SetDisable(TRUE);
}

void CFishingPointDialog::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	CHero* hero = OBJECTMGR->GetHero();
	if(!hero)		return;

	if(val == TRUE)
	{
		char buf[256] = {0,};
		sprintf( buf, "%d", FISHINGMGR->GetFishPoint());
		m_pCurStatic->SetStaticText(buf);
		m_pAddStatic->SetStaticText("");

		m_dwAddFishPoint = 0;

		OBJECTSTATEMGR->StartObjectState( hero, eObjectState_Deal );
		// 인벤토리도 열자
	}
	else
	{
		ClearFishIcon();

		OBJECTSTATEMGR->EndObjectState( hero, eObjectState_Deal );
	}
	cDialog::SetActive(val);
}

BOOL CFishingPointDialog::FakeMoveIcon(LONG x, LONG y, cIcon * pOrigIcon)
{
	if( WT_ITEM != pOrigIcon->GetType() || pOrigIcon->IsLocked() || m_bDisable)
	{
		return FALSE;
	}

	CItem * pOrigItem = (CItem *)pOrigIcon;

	// 인벤토리의 아이템만 허용된다
	if( ! ITEMMGR->IsEqualTableIdxForPos(eItemTable_Inventory, pOrigItem->GetPosition() ) )
	{
		return FALSE;
	}

	const DWORD				itemIndex	= pOrigItem->GetItemIdx();
	const ITEM_INFO*		info		= ITEMMGR->GetItemInfo( itemIndex );


	// 물고기만 처리
	if(ITEM_KIND_FISHING_FISH == info->SupplyType)
	{
		WORD wPos;
		m_pIconFish->GetPositionForXYRef(x, y, wPos);

		if(0<=wPos && wPos<MAX_FISHLIST_FORPOINT)
		{
			CBaseItem* pPrevItem = (CBaseItem*)m_pIconFish->GetIconForIdx(wPos);
			if(pPrevItem)
			{
				CItem* pItem = (CItem*)m_pFishList[wPos].GetLinkItem();
				if(pItem)
				{
					m_dwAddFishPoint -= pItem->GetItemInfo()->SupplyValue * pItem->GetDurability();
					pItem->SetLock(FALSE);
				}

				SetFishItem(wPos, NULL);
			}

			if(!m_pIconFish->IsAddable(wPos))
				return FALSE;

			SetFishItem(wPos, pOrigItem);

			ITEMMGR->AddToolTip( &m_pFishList[wPos] );
			m_pIconFish->AddIcon( wPos, &m_pFishList[wPos]);

			m_dwAddFishPoint += pOrigItem->GetItemInfo()->SupplyValue * pOrigItem->GetDurability();
			pOrigItem->SetLock(TRUE);

			char buf[256] = {0,};
			sprintf( buf, "%d", m_dwAddFishPoint);
			m_pAddStatic->SetStaticText(buf);

			return FALSE;
		}
	}
	
	return FALSE;
}

void CFishingPointDialog::SetPointText(char* pText)
{
	if(m_pCurStatic)
	{
		m_pCurStatic->SetStaticText(pText);
	}
}

void CFishingPointDialog::SetFishItem(int nIdx, CBaseItem* pBaseItem)
{
	m_pFishList[nIdx].SetData(nIdx);
	m_pFishList[nIdx].SetLinkItem(NULL);
	m_pIconFish->DeleteIcon(
		WORD(nIdx),
		0);

	if(pBaseItem)
	{
		m_pFishList[nIdx].SetData(pBaseItem->GetItemIdx());
		m_pFishList[nIdx].SetLinkItem(pBaseItem);
		m_pFishList[nIdx].SetDurability(pBaseItem->GetDurability());

		ITEMMGR->AddToolTip( &m_pFishList[nIdx] );
		m_pIconFish->AddIcon(
			WORD(nIdx),
			&m_pFishList[nIdx]);
	}
}

void CFishingPointDialog::ShowDealDialog(BOOL val)
{
	SetAbsXY( (LONG)m_relPos.x, (LONG)m_relPos.y ) ;

	SetActive( val ) ;

	CInventoryExDialog* pWindow = GAMEIN->GetInventoryDialog() ;

	VECTOR2 vPos = {0, } ;
	vPos.x = pWindow->GetAbsX() ;
	vPos.y = pWindow->GetAbsY() ;

	memcpy( pWindow->GetPrevPos(), &vPos, sizeof(VECTOR2) ) ;

	if( val )
	{
		pWindow->SetAbsXY((LONG)(m_relPos.x + 10) + GetWidth(), (LONG)m_relPos.y ) ;

		if( !pWindow->IsActive() )pWindow->SetActive( TRUE ) ;
	}
}

void CFishingPointDialog::ClearFishIcon()
{
	int i;
	for(i=0; i<MAX_FISHLIST_FORPOINT; i++)
	{
		CItem* pItem = (CItem*)m_pFishList[i].GetLinkItem();
		if(pItem)
			pItem->SetLock(FALSE);

		SetFishItem(i, NULL);
	}

	m_dwAddFishPoint = 0;
	m_pAddStatic->SetStaticText("");
}

void CFishingPointDialog::SendFishPoint()
{
	MSG_FISHING_FPCHANGE_SYN msg;
	msg.Category = MP_FISHING;
	msg.Protocol = MP_FISHING_FPCHANGE_SYN;
	msg.dwObjectID = HEROID;

	int i;
	BOOL bCheck = FALSE;
	for(i=0; i<MAX_FISHLIST_FORPOINT; i++)
	{
		CItem* pItem = (CItem*)m_pFishList[i].GetLinkItem();
		if(pItem)
		{
			msg.FishList[i].dwItemIdx = pItem->GetItemIdx();
			msg.FishList[i].pos = pItem->GetPosition();
			msg.FishList[i].dur = pItem->GetDurability();
			bCheck = TRUE;
		}
	}

	msg.dwAddFishPoint = m_dwAddFishPoint;

	if(bCheck)
	{
		NETWORK->Send(&msg, sizeof(msg));

		//SetDisable( TRUE );
		GAMEIN->GetInventoryDialog()->SetDisable( TRUE );
	}
}
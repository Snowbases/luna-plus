// ItemShopDialog.cpp: implementation of the CItemShopDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemShopDialog.h"
#include "WindowIDEnum.h"
#include "./Interface/cIcon.h"
#include "./Interface/cScriptManager.h"
#include "./Interface/cPushupButton.h"
#include "./Interface/cIconGridDialog.h"
#include "./Interface/cWindowManager.h"
#include "ItemShopGridDialog.h"
#include "ItemManager.h"
#include "./Input/UserInput.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "ChatManager.h"
#include "cMsgBox.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemShopDialog::CItemShopDialog()
{
	m_type = WT_ITEMSHOPDIALOG;
}

CItemShopDialog::~CItemShopDialog()
{}

// 100609 ONS PC방아이템 시간설정 기능 추가
DWORD CItemShopDialog::ActionEvent(CMouse * mouseInfo)
{
	return cDialog::ActionEvent( mouseInfo );	
}

void CItemShopDialog::SetActive( BOOL val )
{
	if( val == FALSE)
	{
		if( HERO->GetState() == eObjectState_Deal)
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
	}
	
	cDialog::SetActive( val );
}



void CItemShopDialog::Linking()
{
	m_pItemGridDialog = (CItemShopGridDialog*)GetWindowForID( ITMALL_GRID );
}

void CItemShopDialog::SetItemInfo(SEND_SHOPITEM_INFO* pItemInfo)
{
	for(POSTYPE i = 0; i < SLOT_SHOPITEM_NUM; ++i)
	{
		CItem* pItem = m_pItemGridDialog->GetItemForPos(
			POSTYPE(TP_SHOPITEM_START + i));

		if( pItem )
		{
			cIcon* pIcon = 0;
			m_pItemGridDialog->DeleteIcon( i, &pIcon );
		}
	}
	
	BOOL bPresent = FALSE;
	for(i=0; i<pItemInfo->ItemCount; i++)
	{
		if( pItemInfo->Item[i].dwDBIdx != 0 )
		{
			if( pItemInfo->Item[i].ItemParam & ITEM_PARAM_PRESENT )
			{
				bPresent = TRUE;
				pItemInfo->Item[i].ItemParam ^= ITEM_PARAM_PRESENT;
			}

			m_pItemGridDialog->AddItem( &pItemInfo->Item[i] );

//			dwtab = GetTabNumber(pItemInfo->Item[i].Position);
//			m_pItemGridDialog[dwtab]->AddItem( &pItemInfo->Item[i] );			
		}
	}

	if( bPresent )
		WINDOWMGR->MsgBox( MBI_PRESENT_NOTIFY, MBT_OK, CHATMGR->GetChatMsg(1194) );
}

BOOL CItemShopDialog::AddItem(CItem* pItem)
{
	return m_pItemGridDialog->AddItem( &( pItem->GetItemBaseInfo() ) );
}

BOOL CItemShopDialog::DeleteItem(POSTYPE Pos,CItem** ppItem)
{
	return m_pItemGridDialog->DeleteItem(Pos, ppItem);
}

CItem * CItemShopDialog::GetItemForPos(POSTYPE absPos)
{
	if(TP_SHOPITEM_START <= absPos && absPos < TP_SHOPITEM_END)
	{
		return (CItem *)m_pItemGridDialog->GetIconForIdx(WORD(absPos - TP_SHOPITEM_START));
	}
	else
		return NULL;
}
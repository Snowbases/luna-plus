// WearedExDialog.cpp: implementation of the CWearedExDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WearedExDialog.h"

#include "ObjectManager.h"
#include "AppearanceManager.h"
#include "ItemManager.h"
#include "CharacterCalcManager.h"
#include "GameIn.h"

#include "CharacterDialog.h"

#include "ChatManager.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWearedExDialog::CWearedExDialog()
{
	m_type = WT_WEAREDDIALOG;
	m_nIconType = WT_ITEM;
}

CWearedExDialog::~CWearedExDialog()
{}


BOOL CWearedExDialog::AddItem(WORD relPos, cIcon * InIcon)
{	
	CHero* pHero = OBJECTMGR->GetHero();

	if( ! AddIcon(relPos, InIcon) )
	{
		return FALSE;
	}

	CItem * item = (CItem * )InIcon;

	pHero->SetWearedItemIdx(relPos, item->GetItemIdx());
	pHero->SetCurComboNum(SKILL_COMBO_NUM);

	APPEARANCEMGR->AddCharacterPartChange(pHero->GetID(), eCHAPPEAR_INIT);
	
	CHARCALCMGR->AddItem( pHero, item->GetItemBaseInfo());
	//CHARCALCMGR->Initialize( pHero );
	GAMEIN->GetCharacterDialog()->RefreshInfo();
	RefreshSetItem(
		item->GetItemBaseInfo());

	DWORD	dwArmor	=	HERO->GetWearedItemIdx( eWearedItem_Dress );
	DWORD	dwBoots	=	HERO->GetWearedItemIdx( eWearedItem_Shoes );
	DWORD	dwGlove	=	HERO->GetWearedItemIdx( eWearedItem_Glove );
	DWORD	dwItem	=	item->GetItemIdx();

	if( dwArmor == dwItem || dwBoots == dwItem || dwGlove == dwItem )
		HERO->DoGetDefenseRate();

	RefreshArmorItem();
	//----------------------------------

	return TRUE;
}

BOOL CWearedExDialog::DeleteItem(WORD relPos, cIcon ** outIcon)
{
	CHero* pHero = OBJECTMGR->GetHero();
	//KES 031128 ¹«°ø¾ÆÀÌÅÛ ÀåÂø°¡´É ÅøÆÁ º¯°æ
	if( ! DeleteIcon(relPos, outIcon) )
	{
		return FALSE;
	}
	
	pHero->SetWearedItemIdx(relPos, 0);
	pHero->SetCurComboNum(SKILL_COMBO_NUM);
	
	APPEARANCEMGR->AddCharacterPartChange(pHero->GetID(), eCHAPPEAR_INIT);
	
	CItem* item	= ( CItem* )( *outIcon );

	if( ! item )
	{
		return FALSE;
	}
	
	CHARCALCMGR->RemoveItem( pHero, item->GetItemBaseInfo() );
	//CHARCALCMGR->Initialize( pHero );
	GAMEIN->GetCharacterDialog()->RefreshInfo();
	RefreshSetItem(
		item->GetItemBaseInfo());

	item->SetImageRGB(
		ICONCLR_USABLE);
	item->SetDeffenceTooltip(
		FALSE);

	HERO->DoGetDefenseRate();

	RefreshArmorItem();
	//----------------------------------

	return TRUE;
}

void CWearedExDialog::RefreshSetItem( const ITEMBASE& item )
{
	const SetScript* script = GAMERESRCMNGR->GetSetScript( item.wIconIdx );

	if( ! script )
	{
		return;
	}

	const SetScript::Item&	itemSet = script->mItem;
	CHero*					hero	= OBJECTMGR->GetHero();

	for(POSTYPE part = TP_WEAR_START ; part < TP_WEAR_END ; ++part )
	{
		const ITEMBASE* itemBase = ITEMMGR->GetItemInfoAbsIn( hero, part );

		if( itemBase )
		{
			CItem* item = ITEMMGR->GetItem( itemBase->dwDBIdx );
			ASSERT( item );

			if( itemSet.end() != itemSet.find( item->GetItemIdx() ) )
			{
				ITEMMGR->RefreshItem( item );
			}
		}
	}
}

//---KES 방어력 적용 여부 071128
void CWearedExDialog::RefreshArmorItem()
{
	if( !HERO ) return;

	for( WORD i = 0 ; i < 16 ; ++i )	//아바타 빼고 16개.... 디파인 어디 되어 있나?
	{
		cIcon* pIcon = GetIconForIdx( i );
		if( !pIcon ) continue;

		if( pIcon->GetType() != WT_ITEM ) return;

		CItem* pItem = (CItem*)pIcon;

		if( HERO->IsNoDeffenceFromArmor() )
		{
			pItem->SetImageRGB( ICONCLR_DISABLE );
			pItem->SetDeffenceTooltip( TRUE );
		}
		else
		{
			pIcon->SetImageRGB( ICONCLR_USABLE );
			pItem->SetDeffenceTooltip( FALSE );
		}

        ITEMMGR->AddToolTip( pItem );
	}
}
//--------------------------------
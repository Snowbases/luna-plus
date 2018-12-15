#include "stdafx.h"
#include ".\petweareddialog.h"
#include "cicon.h"
#include "item.h"
#include "chatmanager.h"
#include "objectmanager.h"
#include "appearancemanager.h"
#include "Interface/cScriptManager.h"
#include "itemmanager.h"
#include "quickmanager.h"
#include "PetManager.h"

CPetWearedDialog::CPetWearedDialog(void)
{
	m_type = WT_PET_WEARED_DLG;

	SCRIPTMGR->GetImage( 140, &mBlockImage );
}

CPetWearedDialog::~CPetWearedDialog(void)
{
}

void CPetWearedDialog::Linking()
{
}

void CPetWearedDialog::SetActive( BOOL val )
{
	cIconDialog::SetActive( val );
}

BOOL CPetWearedDialog::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	if( icon->GetParent() != this )
	{
		if( icon->GetType() != WT_ITEM )
		{
			return FALSE;
		}

		CItem* pItem = ( CItem* )icon;

		const ITEM_INFO* pItemInfo = pItem->GetItemInfo();

		if( pItemInfo->EquipType != eEquipType_Pet )
		{
			return FALSE;
		}
		else if(pItemInfo->EquipSlot < PET_EQUIP_SLOT_START)
		{
			return FALSE;
		}

		// 090625 pdy 펫 기간제 아이템 착용처리 추가
		// 090713 pdy 조건 채크를 eITEM_TYPE_SEAL로 수정 
		if( eITEM_TYPE_SEAL == pItem->GetItemBaseInfo().nSealed )
		{
			return FALSE;
		}

		if( pItemInfo->LimitLevel > HEROPET->GetLevel() )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(363) );
			return FALSE;
		}

		// 090202 LYW --- PetWeardDialog : 펫 장비창의 슬롯 번호를 얻는다.
		WORD slot = 0;
		BOOL rt = FALSE ;
		RECT Rect ;

		POINT mousePoint ;
		mousePoint.x = x ;
		mousePoint.y = y ;

		WORD wCellNum = GetCellNum() ;

		for( WORD i = 0 ; i < wCellNum ; ++i )
		{
			GetIconCellRect( i, Rect ) ;

			if( !PtInRect( &Rect, mousePoint ) )
			continue ;

			slot = i ;

			rt = TRUE ;
		}

		if( pItemInfo->SupplyType == ITEM_KIND_PET_SKILL )
		{
			if( rt )
			{
				if( slot < ePetEquipSlot_Skill1 || slot >= ePetEquipSlot_Skill1 + HEROPET->GetSkillSlot() )
				{
					BOOL chk = FALSE;

					for( WORD i = ePetEquipSlot_Skill1; i < ePetEquipSlot_Skill1 + HEROPET->GetSkillSlot(); i++ )
					{
						if( IsAddable( i ) )
						{
							slot = i;
							chk = TRUE;
							break;
						}
					}

					if( !chk )
						return FALSE;
				}
			}
			else
			{
				// 090202 LYW --- PetWeardDialog : 상위 펫 장비창의 슬롯 번호를 얻지 못하면, 
				// 아이템을 아이템을 제대로 이동하지 않은 것으로 보고 리턴 처리한다.
				return FALSE;
			}
		}
		else
		{
			slot = WORD(pItemInfo->EquipSlot - PET_EQUIP_SLOT_START);
		}

		MSG_PET_ITEM_MOVE_SYN msg;
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_ITEM_MOVE_PLAYER_SYN;
		msg.dwObjectID = HEROID;
		msg.PlayerPos = pItem->GetPosition();
		msg.PlayerItemIdx = pItem->GetItemIdx();
		msg.PetPos = slot;

		CItem* target = ( CItem* )GetIconForIdx( slot );

		if( target )
		{
			msg.PetItemIdx = target->GetItemIdx();
		}
		else
		{
			msg.PetItemIdx = 0;
		}

		NETWORK->Send( &msg, sizeof( msg ) );
	}
	
	return FALSE;
}

BOOL CPetWearedDialog::AddItem(WORD relPos, cIcon * InIcon)
{
	if( ! AddIcon(relPos, InIcon) )
	{
		return FALSE;
	}

	HEROPET->SetWearedItem( relPos, ((CItem*)InIcon)->GetItemIdx() );

	HEROPET->CalcStats();
	
	APPEARANCEMGR->InitAppearance( HEROPET );
	return TRUE;
}

BOOL CPetWearedDialog::DeleteItem(WORD relPos, cIcon ** outIcon)
{
	if( ! DeleteIcon(relPos, outIcon) )
	{
		return FALSE;
	}

	HEROPET->SetWearedItem( relPos, 0 );

	HEROPET->CalcStats();

	APPEARANCEMGR->InitAppearance( HEROPET );
	return TRUE;
}

void CPetWearedDialog::Render()
{
	cIconDialog::Render();

	VECTOR2 Scale;
	Scale.x = Scale.y = 1;
	VECTOR2 ScreenPos;

	PET_INFO* pPetInfo = PETMGR->GetPetInfo(HEROPET->GetKind());

	for( BYTE slot = 6; slot > HEROPET->GetSkillSlot(); slot-- )
	{
		// 090720 ONS 고정스킬이 설정된 펫의 경우6번째 슬롯을 열어둔다.
		if(slot == 6 && pPetInfo->StaticItemIndex > 0) 
			continue;

		RECT rt = {0};
		GetIconCellRect(
			WORD(ePetEquipSlot_Tail + slot),
			rt);

		ScreenPos.x = rt.left + m_absPos.x;
		ScreenPos.y = rt.top + m_absPos.y;
		mBlockImage.RenderSprite(&Scale, NULL, 0, &ScreenPos, 0xffffffff);	
	}

	if( !pPetInfo )
		return;

	if( !pPetInfo->Costume )
	{
		int pos = ePetEquipSlot_Face;

		while( ePetEquipSlot_Tail >= pos )
		{
			RECT rt = {0};
			GetIconCellRect(
				WORD(pos),
				rt);

			ScreenPos.x = rt.left + m_absPos.x;
			ScreenPos.y = rt.top + m_absPos.y;
			mBlockImage.RenderSprite(&Scale, NULL, 0, &ScreenPos, 0xffffffff);

			pos++;
		}
	}
}

void CPetWearedDialog::FakeDelete( cIcon* icon )
{
	if( icon->GetType() != WT_ITEM )
		return;
	
	if( icon->GetParent() != this )
		return;

	CItem* pItem = ( CItem* )icon;

	mDeleteItem = pItem->GetItemBaseInfo();
}

void CPetWearedDialog::SendDelete()
{
	if( mDeleteItem.dwDBIdx == 0 )
		return;

	MSG_DWORDBYTE msg;
	msg.Category = MP_PET;
	msg.Protocol = MP_PET_ITEM_DISCARD_SYN;
	msg.dwObjectID = HEROID;
	msg.bData = (BYTE)mDeleteItem.Position;
	msg.dwData = mDeleteItem.dwDBIdx;

	NETWORK->Send( &msg, sizeof( msg ) );
}

void CPetWearedDialog::DeleteAck( BYTE pos )
{
	cIcon* pIcon;

	if( !DeleteItem( pos, &pIcon ) )
		return;

	if( pIcon )
	{
		CItem* pItem = ( CItem* )pIcon;

		QUICKMGR->DeleteLinkdedQuickItem(pItem->GetDBIdx()) ;
		QUICKMGR->RefreshQickItem();

		if( pItem->IsQuickLink() )
		{
			pItem->RemoveQuickLink();
		}
		
		ITEMMGR->ItemDelete( pItem );
	}
}

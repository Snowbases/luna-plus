#include "stdafx.h"
#include "WindowIDEnum.h"
#include "./Interface/cIconGridDialog.h"
#include "./Interface/cStatic.h"
#include ".\quickslotdlg.h"
#include "..\[CC]Skill\Client\\Info\SkillInfo.h"
#include "ObjectManager.h"
#include "cSkillBase.h"
// 070216 LYW --- QuickSlotDlg : Include header file of cConductBase.
#include "ConductBase.h"
#include "ConductInfo.h"

#include "cQuickItem.h"
#include "QuickManager.h"
#include "GameIn.h"

#include "ItemManager.h"

#include "InventoryExDialog.h"
#include "./Input/Mouse.h"

#include "ChatManager.h"
#include "MacroManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"

// 080707 LYW --- QuickSlotDlg : 확장 슬롯 추가 처리.
#include "../Interface/cWindowManager.h"

cQuickSlotDlg::cQuickSlotDlg(void)
{
	mCurSheet = 0;
}

cQuickSlotDlg::~cQuickSlotDlg(void)
{
}

void cQuickSlotDlg::Add(cWindow * window)
{
	if(window->GetType() == WT_ICONGRIDDIALOG )
	{
		// 080702 LYW --- QuickSlotDlg : 확장 슬롯 추가에 따른 최대 페이지 수를 변경.
		//if( mCurSheet > 7 )
		if( mCurSheet > MAX_SLOTPAGE-1 )
		{
			return;
		}

		mpSheet[ mCurSheet++ ] = ( cIconGridDialog* )window;

	}
	
	cDialog::Add(window);
}

void cQuickSlotDlg::Linking()
{
	cQuickItem* pItem = NULL;

	for( WORD sheet = 0; sheet < MAX_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			if(GetID() == QI1_QUICKSLOTDLG)
			{
				pItem = QUICKMGR->NewQuickItem( sheet, pos );
			}
			else
			{
				pItem = QUICKMGR->NewQuickItem( ( (sheet+MAX_SLOTPAGE) + 0 ), pos );
			}

			mpSheet[ sheet ]->AddIcon( pos, pItem );
		}
	}
}

void cQuickSlotDlg::OnActionEvnet(LONG lId, void * p, DWORD we)
{
	if( we & WE_BTNCLICK )
	{
		if( lId == QI_PREVPAGEBTN || lId == QI2_PREVPAGEBTN)
		{
			SelectPage( mCurSheet - 1 );
		}
		else if( lId == QI_NEXTPAGEBTN || lId == QI2_NEXTPAGEBTN )
		{
			SelectPage( mCurSheet + 1 );
		}
	}
	else if( we & WE_LBTNDBLCLICK )
	{
		// 080704 LYW --- QuickSlotDlg : 확장 슬롯 추가 처리.
		//QUICKMGR->UseQuickItem( mCurShee, ( WORD )( mpSheet[ mCurSheet ]->GetCurSelCellPos() ) );
		if(this->GetID() == QI1_QUICKSLOTDLG)
		{
			QUICKMGR->UseQuickItem( mCurSheet, ( WORD )( mpSheet[ mCurSheet ]->GetCurSelCellPos() ) );
		}
		else
		{
			QUICKMGR->UseQuickItem( (mCurSheet+MAX_SLOTPAGE) + 0, ( WORD )( mpSheet[ mCurSheet ]->GetCurSelCellPos() ) );
		}
	}
}

void cQuickSlotDlg::SelectPage( WORD page )
{
	// 080702 LYW --- QuickSlotDlg : 확장 슬롯 추가에 따른 최대 슬롯 페이지를 변경한다.
	//if( page > 8 )
	//{
	//	page = 7;
	//}
	//if( page > 7 )
	//{
	//	page = 0;
	//}

	if( page > MAX_SLOTPAGE )
	{
		page = MAX_SLOTPAGE-1;
	}
	if( page > MAX_SLOTPAGE-1 )
	{
		page = 0;
	}

	mCurSheet = page; 

	SetActive( IsActive() );
}

void cQuickSlotDlg::SetActive( BOOL val )
{
	cDialog::SetActive( val );

	if( val )
	{
		// 080702 LYW --- QuickSkotDlg : 확장 슬롯 추가에 따른 최대 페이지 수를 변경한다.
		//for( WORD i = 0; i < 8; i++ )
		for( WORD i = 0; i < MAX_SLOTPAGE; i++ )
		{
			if( i == mCurSheet )
			{
				mpSheet[ i ]->SetActive( TRUE );
				continue;
			}

			mpSheet[ i ]->SetActive( FALSE );
		}
	}
}

// 080703 LYW --- QuickSlotDlg : 확장 슬롯 추가에 따른 함수 변경.
//BOOL cQuickSlotDlg::FakeMoveIcon(LONG mouseX, LONG mouseY, cIcon * icon)
BOOL cQuickSlotDlg::FakeMoveIcon(cDialog* pFromDlg, LONG mouseX, LONG mouseY, cIcon * icon)
{
	// 080702 LYW --- QuickSlotDlg : 확장 슬롯 추가에 따른 최대 페이지를 변경.
	//if( mCurSheet > 8 )
	if( mCurSheet > MAX_SLOTPAGE )
	{
		return FALSE;
	}
	else if( icon->GetType() == WT_STALLITEM || icon->GetType() == WT_EXCHANGEITEM )
	{
		return FALSE;
	}
	else if(FALSE == CanEquip(icon))
	{
		return FALSE;
	}

	switch( icon->GetType() )
	{
	case WT_QUICKITEM:
		{
			// 080703 LYW --- QuickSlotDlg : 확장 슬롯 추가에 따른 처리.
			//FakeMoveQuickItem(mouseX, mouseY, (cQuickItem *)icon);
			FakeMoveQuickItem(pFromDlg, mouseX, mouseY, (cQuickItem *)icon);
		}
		break;
	case WT_ITEM:
		{
			FakeAddItem(mouseX, mouseY, (CItem *)icon);
		}
		break;
	case WT_SKILL:
		{
			FakeAddSkill(mouseX, mouseY, (cSkillBase *)icon);
		}
		break;
	// 070216 LYW --- QuickSlotDlg : Add case part for conduct.
	case WT_CONDUCT :
		{
			FakeAddConduct(mouseX, mouseY, (cConductBase*)icon) ;
		}
		break ;
	}
	return FALSE;
}

// 080703 LYW --- QuickSlotDlg : 확장슬롯 추가에 따른 처리.
//void cQuickSlotDlg::FakeMoveQuickItem(LONG x, LONG y, cQuickItem * icon)
//{
//	WORD tab = 0;
//	WORD pos1 = 0;
//	WORD pos2 = 0;
//	
//	if( !( mpSheet[ mCurSheet ]->GetPositionForXYRef( x, y, pos2 ) ) )
//	{
//		return;
//	}
//
//	icon->GetPosition( tab, pos1 );
//
//	MSG_QUICK_CHANGE_SYN msg;
//	msg.Category = MP_QUICK;
//	msg.Protocol = MP_QUICK_CHANGE_SYN;
//	msg.dwObjectID = HEROID;
//	msg.tabNum1 = ( BYTE )tab;
//	msg.pos1 = ( BYTE )pos1;
//	msg.tabNum2 = ( BYTE )( mCurShee );
//	msg.pos2 = ( BYTE )pos2;
//
//	NETWORK->Send( &msg, sizeof( msg ) );
//}

void cQuickSlotDlg::FakeMoveQuickItem(cDialog* pFromDlg, LONG x, LONG y, cQuickItem * icon)
{
	// 이전 슬롯을 체크.
	if(!pFromDlg) return ;

	// 아이콘 정보를 체크.
	if(!icon) return ;


	// 이동 처리에 필요한 임시 변수들을 선언한다.
	WORD wThisTab	= 0 ;
	WORD wBeforeTab = 0 ;
	WORD wThisPos	= 0 ;
	WORD wBeforePos = 0 ;


	// 이전 탭 / 위치를 받는다.
	icon->GetPosition( wBeforeTab, wBeforePos );

	wThisTab = mCurSheet;
	
	// 현재 위치를 받는다.
	if( !( mpSheet[ mCurSheet ]->GetPositionForXYRef( x, y, wThisPos ) ) )
	{
		return;
	}

	if(GetID() == QI2_QUICKSLOTDLG)
	{
		wThisTab += MAX_SLOTPAGE;
	}

	if( wBeforeTab == wThisTab && wBeforePos == wThisPos )
		return;

	// 이동 정보를 서버로 보낸다.
	MSG_QUICK_CHANGE_SYN msg;
	msg.Category = MP_QUICK;
	msg.Protocol = MP_QUICK_CHANGE_SYN;
	msg.dwObjectID = HEROID;
	msg.tabNum1 = ( BYTE )wBeforeTab;
	msg.pos1 = ( BYTE )wBeforePos;
	msg.tabNum2 = ( BYTE )wThisTab;
	msg.pos2 = ( BYTE )wThisPos;

	NETWORK->Send( &msg, sizeof( msg ) );
}

void cQuickSlotDlg::FakeAddItem(LONG x, LONG y, CItem * icon)
{
	WORD pos = 0;
	if( !( mpSheet[ mCurSheet ]->GetPositionForXYRef( x, y, pos ) ) )
	{
		return;
	}

	MSG_QUICK_ADD_SYN msg;

	msg.Category = MP_QUICK;
	msg.Protocol = MP_QUICK_ADD_SYN;
	msg.dwObjectID = HEROID;

	if(this->GetID() == QI1_QUICKSLOTDLG)
	{
		msg.tabNum = ( BYTE )(mCurSheet);
	}
	else
	{
		msg.tabNum = ( BYTE )(mCurSheet+MAX_SLOTPAGE);
	}

	msg.pos = ( BYTE )pos;
	msg.slot.dbIdx = icon->GetDBIdx();
	msg.slot.idx = icon->GetItemIdx();

	if( icon->GetItemInfo()->EquipType == eEquipType_Pet )
	{
		if( icon->GetItemInfo()->SupplyType == ITEM_KIND_PET_SKILL )
		{
			msg.slot.data = 0;
			msg.slot.kind = QuickKind_PetSkill;
		}
		else
		{
			msg.slot.data = 0;
			msg.slot.kind = QuickKind_PetItem;
		}
	}
	else if( ITEMMGR->IsDupItem( icon->GetItemIdx() ) )
	{
		msg.slot.data = 0;
		msg.slot.kind = QuickKind_MultiItem;
	}
	else
	{
		msg.slot.data = 0;
		msg.slot.kind = QuickKind_SingleItem;
	}

	NETWORK->Send( &msg, sizeof( msg ) );
}

void cQuickSlotDlg::FakeAddSkill(LONG x, LONG y, cSkillBase * icon)
{
	WORD pos = 0;
	if( !( mpSheet[ mCurSheet ]->GetPositionForXYRef( x, y, pos ) ) )
	{
		return;
	}

	MSG_QUICK_ADD_SYN msg;

	msg.Category = MP_QUICK;
	msg.Protocol = MP_QUICK_ADD_SYN;
	msg.dwObjectID = HEROID;
	
	if(this->GetID() == QI1_QUICKSLOTDLG)
	{
		msg.tabNum = ( BYTE )mCurSheet;
	}
	else
	{
		msg.tabNum = ( BYTE )(mCurSheet+MAX_SLOTPAGE);
	}

	msg.pos = ( BYTE )pos;
	msg.slot.kind = QuickKind_Skill;
	msg.slot.dbIdx = icon->GetDBIdx();
	msg.slot.idx = icon->GetSkillIdx();
	msg.slot.data = icon->GetLevel();

	NETWORK->Send( &msg, sizeof( msg ) );
}

// 070216 LYW --- QuickSlotDlg : Add function to add conduct to quick slot.
void cQuickSlotDlg::FakeAddConduct( LONG x, LONG y, cConductBase* icon )
{
	WORD pos = 0 ;

	if( !( mpSheet[ mCurSheet ]->GetPositionForXYRef( x, y, pos ) ) )
	{
		return ;
	}

	MSG_QUICK_ADD_SYN msg ;

	msg.Category = MP_QUICK;
	msg.Protocol = MP_QUICK_ADD_SYN;
	msg.dwObjectID = HEROID;
	
	// 080702 LYW --- QuickSlotDlg : 확장 슬롯 추가 처리.
	//msg.tabNum = ( BYTE )( mCurShee );
	if(this->GetID() == QI1_QUICKSLOTDLG)
	{
		msg.tabNum = ( BYTE )mCurSheet;
	}
	else
	{
		msg.tabNum = ( BYTE )(mCurSheet+MAX_SLOTPAGE);
	}

	msg.pos = ( BYTE )pos;
	msg.slot.kind = QuickKind_Conduct;
	msg.slot.dbIdx = icon->m_pConductInfo->GetConductIdx() ;
	msg.slot.idx = icon->m_pConductInfo->GetConductIdx() ;
	msg.slot.data = 0;

	NETWORK->Send( &msg, sizeof( msg ) );

}

void cQuickSlotDlg::FakeDeleteQuickItem()
{
	MSG_QUICK_REMOVE_SYN msg;

	msg.Category = MP_QUICK;
	msg.Protocol = MP_QUICK_REMOVE_SYN;
	msg.dwObjectID = HEROID;
	
	if(this->GetID() == QI1_QUICKSLOTDLG)
	{
		msg.tabNum = ( BYTE )mCurSheet;
	}
	else
	{
		msg.tabNum = ( BYTE )(mCurSheet+MAX_SLOTPAGE);
	}
	msg.pos = ( BYTE )( mpSheet[ mCurSheet ]->GetCurSelCellPos() );

	NETWORK->Send( &msg, sizeof( msg ) );
}

void cQuickSlotDlg::SetCoolTime( DWORD itemIndex, DWORD miliSecond )
{
	const DWORD groupIndex = ITEMMGR->GetCoolTimeGroupIndex( itemIndex );

	for( WORD page = 0; page < MAX_SLOTPAGE; ++page )
	{
		cIconGridDialog* tab = mpSheet[ page ];
		
		for( WORD iconIndex = 0; tab->GetCellNum() > iconIndex; ++iconIndex )
		{
			cQuickItem* icon = ( cQuickItem* )tab->GetIconForIdx( iconIndex );

			if( ! icon )
			{
				continue;
			}
			else if(QuickKind_MultiItem != icon->GetSlotInfo().kind	&&
				QuickKind_SingleItem != icon->GetSlotInfo().kind)
			{
				continue;
			}
			else if(icon->GetSlotInfo().idx == itemIndex ||
				(groupIndex && groupIndex == ITEMMGR->GetCoolTimeGroupIndex(icon->GetSlotInfo().idx)))
			{
				icon->SetCoolTime( miliSecond );
			}
		}
	}
}


void cQuickSlotDlg::SetCoolTimeAbs( DWORD itemIndex, DWORD dwBegin, DWORD dwEnd, DWORD miliSecond )
{
	const DWORD groupIndex = ITEMMGR->GetCoolTimeGroupIndex( itemIndex );

	for( WORD page = 0; page < MAX_SLOTPAGE; ++page )
	{
		cIconGridDialog* tab = mpSheet[ page ];
		
		for( WORD iconIndex = 0; tab->GetCellNum() > iconIndex; ++iconIndex )
		{
			cQuickItem* icon = ( cQuickItem* )tab->GetIconForIdx( iconIndex );

			if( ! icon )
			{
				continue;
			}
			else if(QuickKind_MultiItem	!= icon->GetSlotInfo().kind &&
				QuickKind_SingleItem != icon->GetSlotInfo().kind)
			{
				continue;
			}

			if(icon->GetSlotInfo().idx == itemIndex ||
				(groupIndex && groupIndex == ITEMMGR->GetCoolTimeGroupIndex(icon->GetSlotInfo().idx)))
			{
				icon->SetCoolTimeAbs( dwBegin, dwEnd, miliSecond );
			}
		}
	}
}

BOOL cQuickSlotDlg::CanEquip(cIcon* pIcon)
{
	if(pIcon->GetType() == WT_QUICKITEM)
	{
		return TRUE;
	}
	else if(pIcon->GetType() == WT_ITEM)
	{
		CItem * pItem = (CItem *)pIcon;

		if( pItem->GetItemInfo()->Category == eItemCategory_Equip )
		{
			return TRUE;
		}

		if( pItem->GetItemInfo()->Category == eItemCategory_Expend )
		{
			return TRUE;
		}

		return FALSE;
	}
	else if( pIcon->GetType() == WT_SKILL )
	{
		cSkillBase * pSkill = (cSkillBase *)pIcon;
		cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo(
			pSkill->GetSkillIdx());

		if( pSkillInfo->GetKind() == SKILLKIND_PASSIVE || pSkillInfo->GetKind() == SKILLKIND_BUFF )
		{
			return FALSE;
		}

		return TRUE;
	}
	// 070216 LYW --- QuickManager : Add conduct part to CanEquip function.
	else if( pIcon->GetType() == WT_CONDUCT )
	{
		return TRUE ;
	}

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(352) );
	return FALSE;
}


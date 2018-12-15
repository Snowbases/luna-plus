#include "stdafx.h"
#include ".\petresurrectiondialog.h"
#include "WindowIDEnum.h"
#include "ChatManager.h"
#include "interface/cIconDialog.h"
#include "Interface/cScriptManager.h"
#include "Interface/cWindowManager.h"
#include "interface/cStatic.h"
#include "InventoryExDialog.h"
#include "Item.h"
#include "VirtualItem.h"
#include "ItemManager.h"
#include "ObjectManager.h"
#include "ProgressDialog.h"
#include "GameIn.h"
#include "QuickSlotDlg.h"
#include "InventoryExDialog.h"
#include "petmanager.h"
#include "./interface/cResourceManager.h"


CPetResurrectionDialog::CPetResurrectionDialog(void)
{
	mSourceItem = new CVirtualItem;
}

CPetResurrectionDialog::~CPetResurrectionDialog(void)
{

}

void CPetResurrectionDialog::Linking()
{
	mIconDialog		= ( cIconDialog* )	GetWindowForID( PET_RES_DLG_ICON );
}

BOOL CPetResurrectionDialog::FakeMoveIcon( LONG x, LONG y, cIcon* icon )
{
	if( WT_ITEM != icon->GetType() ||
		icon->IsLocked()			||
		m_bDisable )
	{
		return FALSE;
	}

	CItem* pItem = (CItem *)icon;

	if( pItem->GetItemInfo()->SupplyType != ITEM_KIND_PET )
	{
		return FALSE;
	}

	PET_OBJECT_INFO* pPetObjectInfo = PETMGR->GetPetObjectInfo( pItem->GetDBIdx() );

	if( !pPetObjectInfo )
	{
		return FALSE;
	}

	if( pPetObjectInfo->State != ePetState_Die )
	{
		return FALSE;
	}

	CItem* old = ( CItem* )( mSourceItem->GetLinkItem() );
	if( old )
		old->SetLock( FALSE );

	mSourceItem->SetData( pItem->GetItemIdx() );
	mSourceItem->SetLinkItem( pItem );

	ITEMMGR->AddToolTip( mSourceItem );

	mIconDialog->AddIcon( 0, mSourceItem );

	pItem->SetLock( TRUE );

	return FALSE;
}

void CPetResurrectionDialog::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if( lId == PET_RES_DLG_CANCEL )
	{
		SetActive( FALSE );

		// 081021 LYW --- PetResurrectionDialog : 아이템의 락을 풀어준다.
		if( mUsedItem ) mUsedItem->SetLock( FALSE ) ;
	}

	if( lId == PET_RES_DLG_OK )
	{
		CItem* pItem = ( CItem* )( mSourceItem->GetLinkItem() );
		if( !pItem )
			return;

		mMessage.Category = MP_PET;
		mMessage.Protocol = MP_PET_RESURRECTION_SYN;
		mMessage.dwObjectID = HEROID;
		mMessage.dwData1 = pItem->GetDBIdx();
		mMessage.dwData2 = mUsedItem->GetPosition();
		mMessage.dwData3 = mUsedItem->GetItemIdx();
		
		GAMEIN->GetInventoryDialog()->SetCoolTime( mUsedItem->GetItemIdx(), WAITMILISECOND );

		{
			cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
			ASSERT( dialog );

			//dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond );
			dialog->SetCoolTime( mUsedItem->GetItemIdx(), WAITMILISECOND );
		}

		{
			CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
			ASSERT( dialog );

			// 081112 LYW --- PetResurrectionDialog : 펫을 부활할 때, 
			// 프로그레스 바의 출력 메시지를 부활중이라는 메시지로 수정 함.(정진문)
			dialog->SetText( CHATMGR->GetChatMsg( 1740 ) ) ;

			dialog->Wait( CProgressDialog::eActionPetResurrection );
		}
	}
}

void CPetResurrectionDialog::SetActive( BOOL val )
{
	cDialog::SetActive( val );

	if( mUsedItem )
		mUsedItem->SetLock( val );

	if( !val )
	{
		cIcon* pIcon;

		mIconDialog->DeleteIcon( 0, &pIcon );
		
		CItem* old = ( CItem* )( mSourceItem->GetLinkItem() );
		if( old )
			old->SetLock( FALSE );
	}
}

void CPetResurrectionDialog::Send()
{
	MSG_DWORD3 message( mMessage );

	NETWORK->Send( &message, sizeof( message ) );
}
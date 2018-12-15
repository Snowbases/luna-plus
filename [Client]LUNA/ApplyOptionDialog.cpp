#include "stdafx.h"
#include "ApplyOptionDialog.h"
#include "WindowIDEnum.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "cScriptManager.h"
#include "cWindowManager.h"
#include "interface/cIconDialog.h"
#include "interface/cStatic.h"
#include "ItemManager.h"
#include "ProgressDialog.h"
#include "FishingManager.h"
#include "GameIn.h"
#include "FishingDialog.h"
#include "FishingPointDialog.h"


CApplyOptionDialog::CApplyOptionDialog() :
mIconDialog( 0 ),
mResultStatic( 0 ),
mTargetItem( 0 ),
mSourceItem( 0 )
{
	// 대상 아이템 아이콘
	{
		cImage image;

		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );

		ITEMBASE base;
		ZeroMemory( &base, sizeof( base ) );

		mTargetItem = new CItem( &base );

		mTargetItem->SetToolTip( "", RGB_HALF( 255, 255, 255), &image );
		mTargetItem->SetMovable( FALSE );
	}
}


CApplyOptionDialog::~CApplyOptionDialog()
{
	mIconDialog->DeleteIcon( 0, 0 );

	SAFE_DELETE( mTargetItem );
}


void CApplyOptionDialog::Linking()
{
	mIconDialog		= ( cIconDialog* )	GetWindowForID( APPLY_OPTION_ICON_DIALOG );
	mResultStatic	= ( cStatic* )		GetWindowForID( APPLY_OPTION_RESULT_STATIC );
}


void CApplyOptionDialog::SetSourceItem( CItem& item )
{
	mSourceItem = &item;
}


BOOL CApplyOptionDialog::FakeMoveIcon(LONG x, LONG y, cIcon* targetIcon )
{
	if( !	targetIcon							||
			WT_ITEM != targetIcon->GetType()	||
			targetIcon->IsLocked()				||
			IsDisable()							||
		!	mSourceItem )
	{
		return FALSE;
	}

	CItem*				targetItem = ( CItem* )targetIcon;
	const ITEM_INFO*	info = targetItem->GetItemInfo();

	if( !	info ||
			info->Stack )
	{
		return FALSE;
	}
	// 인벤토리의 아이템만 허용된다
	else if( ! ITEMMGR->IsEqualTableIdxForPos( eItemTable_Inventory, targetItem->GetPosition() ) )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 787 ) );
		return FALSE;
	}
	
	const LONGLONG itemKey = g_CGameResourceManager.GetItemKey(
		info->EquipSlot,
		info->WeaponType,
		info->ArmorType,
		info->WeaponAnimation);
	const ApplyOptionScript& script	= g_CGameResourceManager.GetApplyOptionScript(
		mSourceItem->GetItemIdx());

	if(script.mLevel.mMin > info->LimitLevel ||
		script.mLevel.mMax < info->LimitLevel ||
		script.mTargetTypeSet.end() == script.mTargetTypeSet.find(itemKey))
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1421 ) );
		return FALSE;
	}
	
	// 아이템 칸에 이미 아이템이 있다면 복구한다
	{
		cIcon* icon = mIconDialog->GetIconForIdx( 0 );

		if( icon == mTargetItem )
		{
			CItem* item = ITEMMGR->GetItem( mTargetItem->GetDBIdx() );

			if( item )
			{
				item->SetLock( FALSE );
			}
		}
	}

	// 창에 아이콘을 표시한다
	{
		cImage image;

		mTargetItem->Init(
			0,
			0,
			DEFAULT_ICONSIZE,
			DEFAULT_ICONSIZE,
			ITEMMGR->GetIconImage( targetItem->GetItemIdx(), &image ),
			0 );

		mTargetItem->SetItemBaseInfo( targetItem->GetItemBaseInfo() );

		ITEMMGR->AddToolTip( mTargetItem );

		mIconDialog->DeleteIcon( 0, 0 );
		mIconDialog->AddIcon( 0, mTargetItem );
	}
	
	targetItem->SetLock( TRUE );

	return FALSE;
}


void CApplyOptionDialog::OnActionEvent(LONG lId, void * p, DWORD we)
{
	switch( lId )
	{
	case APPLY_OPTION_SUBMIT_BUTTON:
		{
			if( mTargetItem != mIconDialog->GetIconForIdx( 0 ) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, "Empty" );
				break;
			}
			
			const ITEM_INFO* info = mTargetItem->GetItemInfo();

			if( !	mSourceItem	||
				!	info		||
					info->Stack )
			{
				return;
			}

			mMessage.Category	= MP_ITEM;
			mMessage.Protocol	= MP_ITEM_APPLY_OPTION_SYN;
			mMessage.dwObjectID	= HERO->GetID();
			mMessage.dwData1	= mSourceItem->GetPosition();
			mMessage.dwData2	= mTargetItem->GetPosition();

			ITEMMGR->SetDisableDialog( TRUE, eItemTable_Inventory );
			SetDisable( TRUE );

			// 진행 표시가 끝난 후 서버에 작업을 요청할 것이다
			{
				CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
				ASSERT( dialog );

				dialog->Wait( CProgressDialog::eActionApplyOption );
			}
			
			break;
		}
	case APPLY_OPTION_CANCEL_BUTTON:
	case APPLY_OPTION_DIALOG:
		{
			if( m_bDisable )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 992 ) );
				break;
			}

			SetActive( FALSE );

			{
				cIcon* icon = mIconDialog->GetIconForIdx( 0 );

				if( icon == mTargetItem )
				{
					CItem* item = ITEMMGR->GetItem( mTargetItem->GetDBIdx() );

					if( item )
					{
						item->SetLock( FALSE );
					}
				}
			}

			if( mSourceItem )
			{
				mSourceItem->SetLock( FALSE );
			}

			break;
		}
	}
}


void CApplyOptionDialog::SetActive( BOOL isActive )
{
	if( isActive )
	{
		if(		IsActive() ||
			!	mSourceItem )
		{
			return;
		}

		// 다른 작업을 처리 중인 경우에는 창을 띄울 수 없다
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
			ASSERT( dialog );

			if( dialog->IsDisable() &&
				dialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
		}

		// NPC 상점을 연 상태에서도 불가. 
		// 판매 상점을 연 상태에서 열 수 없음: 아이템이 겹쳐지나, 아이템의 잠금 처리가 클라이언트에 저장되지 않으므로 원천적으로 아이템에 대한
		// 동시 접근 시도를 차단해야 한다.
		// 거래 중/창고 사용일 때는 불가함. 위와 마찬가지 이유
		{
			cDialog* npcScriptDialog	= WINDOWMGR->GetWindowForID( NSI_SCRIPTDLG );
			cDialog* npcShopDialog		= WINDOWMGR->GetWindowForID( DE_DEALERDLG );
			cDialog* stallShopDialog	= WINDOWMGR->GetWindowForID( SSI_STALLDLG );
			cDialog* exchangeDialog		= WINDOWMGR->GetWindowForID( XCI_DIALOG );
			cDialog* privateWarehouseDialog		= WINDOWMGR->GetWindowForID( PYO_STORAGEDLG );
			cDialog* guildWarehouseDialog		= WINDOWMGR->GetWindowForID( GDW_WAREHOUSEDLG );
			cDialog* itemMallWarehouseDialog	= WINDOWMGR->GetWindowForID( ITMALL_BASEDLG );

			ASSERT( npcScriptDialog && npcShopDialog && stallShopDialog && exchangeDialog );
			ASSERT( privateWarehouseDialog && guildWarehouseDialog && itemMallWarehouseDialog );

			if( npcScriptDialog->IsActive() )
			{
				return;
			}
			else if( npcShopDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );
				return;
			}
			else if( stallShopDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1078 ) );
				return;
			}
			else if( exchangeDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1079 ) );
				return;
			}
			// 071211 LUJ 창고가 표시된 경우 사용 불가
			else if(	privateWarehouseDialog->IsActive()	||
				guildWarehouseDialog->IsActive()	||
				itemMallWarehouseDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
			// 080429 NYJ --- 낚시중 사용불가
			else if(GAMEIN->GetFishingDlg()->IsPushedStartBtn())
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
			// 080429 NYJ --- 낚시포인트교환중 사용불가
			else if(GAMEIN->GetFishingPointDlg()->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
		}

		// 강화/인챈트/조합/분해/노점판매 창이 동시에 뜨는 것을 막는다
		{
			cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			cDialog* DissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
			cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
			
			ASSERT( mixDialog && DissoloveDialog && reinforceDialog );

			mixDialog->SetActive( FALSE );
			DissoloveDialog->SetActive( FALSE );
			reinforceDialog->SetActive( FALSE );
		}

		// 인벤토리와 함께 중앙에 표시시킨다
		{
			cDialog* inventory = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( inventory );

			const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
			const DWORD			space	= 140;
			const DWORD			x		= ( screen.dwWidth - m_width - inventory->GetWidth() - space ) / 2;
			const DWORD			y		= ( screen.dwHeight - max( inventory->GetHeight(), m_height ) ) / 2;

			SetAbsXY( x + space + inventory->GetWidth(), y );

			inventory->SetAbsXY( x, y );
			inventory->SetActive( TRUE );
		}

		// 초기화
		{
			mIconDialog->DeleteIcon( 0, 0 );
			mResultStatic->SetStaticText( "" );
		}

		// 081024 LUJ, 검사가 완료된 후 아이템을 잠궈야한다
		mSourceItem->SetLock( TRUE );
	}
	else
	{
		if( mSourceItem )
		{
			mSourceItem->SetLock( FALSE );
		}

		{
			CItem* item = ITEMMGR->GetItem( mTargetItem->GetDBIdx() );

			if( item )
			{
				item->SetLock( FALSE );
			}
		}

		{
			CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
			ASSERT( dialog );

			if(  dialog->IsDisable() )
			{
				return;
			}
		}
	}

	cDialog::SetActive( isActive );
}


void CApplyOptionDialog::Succeed()
{
	ITEMMGR->AddToolTip( mTargetItem );

	CItem* item = ITEMMGR->GetItem( mTargetItem->GetDBIdx() );

	if( item )
	{
		ITEMMGR->AddToolTip( item );
	}

	mResultStatic->SetStaticText( CHATMGR->GetChatMsg( 1422 ) );

	Restore();

	// 원료로 사용된 아이템 수량을 조정한다
	if( mSourceItem->GetItemInfo()->Stack &&
		1 < mSourceItem->GetDurability())
	{
		mSourceItem->SetDurability( mSourceItem->GetDurability() - 1 );

		ITEMMGR->AddToolTip( mSourceItem );
	}
	else
	{
		{
			CItem* item = 0;

			ITEMMGR->DeleteItem( mSourceItem->GetPosition(), &item );

			mSourceItem = 0;
		}		

		{
			CItem* item = ITEMMGR->GetItem( mTargetItem->GetDBIdx() );

			if( item )
			{
				item->SetLock( FALSE );
			}
		}

		SetActive( FALSE );
	}

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1419 ), mTargetItem->GetItemInfo()->ItemName );
}


void CApplyOptionDialog::Fail()
{
	mResultStatic->SetStaticText( CHATMGR->GetChatMsg( 1423 ) );

	Restore();

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1420 ), mTargetItem->GetItemInfo()->ItemName );
}


void CApplyOptionDialog::Restore()
{
	SetDisable( FALSE );

	ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );
}


void CApplyOptionDialog::Send()
{
	NETWORK->Send( &mMessage, sizeof( mMessage ) );
}
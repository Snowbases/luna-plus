#include "stdafx.h"
#include "EnchantDialog.h"
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
#include "FishingManager.h"
#include "GameIn.h"
#include "FishingDialog.h"
#include "FishingPointDialog.h"
#include "input/Mouse.h"
#include "CommonCalcFunc.h"


CEnchantDialog::CEnchantDialog() :
mIconDialog( 0 ),
mSourceItem( new CVirtualItem ),
// 080228 LUJ, 일반/보호 모드 표시위한 컨트롤 초기화
mTitle( 0 ),
mProtectTitle( 0 ),
mProtectSymbol( 0 )
{
	cImage image;

	SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );

	// 인챈트할 대상 아이템 아이콘
	{
		ITEMBASE base;
		ZeroMemory( &base, sizeof( base ) );

		mMaterialItem = new CItem( &base );

		mMaterialItem->SetToolTip( "", RGB_HALF( 255, 255, 255), &image );
		mMaterialItem->SetMovable( FALSE );
	}

	// 주문서 아이콘
	{
		mSourceItem->SetToolTip( "", RGB_HALF( 255, 255, 255), &image );
		mSourceItem->SetMovable(FALSE);
	}

	// 080228 LUJ, 보호 아이템 정보 초기화
	ZeroMemory( &mProtectionItem, sizeof( mProtectionItem ) );

	m_pEnchantCost = NULL;
}


CEnchantDialog::~CEnchantDialog()
{
	SAFE_DELETE( mSourceItem );
	SAFE_DELETE( mMaterialItem );
}


void CEnchantDialog::Linking()
{
	mIconDialog		= ( cIconDialog* )	GetWindowForID( ITD_UPGRADEGRID );
	mResult			= ( cStatic* )		GetWindowForID( ITD_UPGRADE_RESULT );
	
	// 080228 LUJ, 일반/보호 모드 표시 용 인터페이스 링크
	mTitle			= GetWindowForID( ITD_UPGRADE_TITLE );
	mProtectTitle	= GetWindowForID( ITD_UPGRADE_PROTECT_TITLE );
	mProtectSymbol	= GetWindowForID( ITD_UPGRADE_PROTECT_SYMBOL );

	// 090714 ShinJS --- 인챈트시 소비자금 출력용 Static
	m_pEnchantCost	= (cStatic*)GetWindowForID( ITD_UPGRADE_COST );
}


BOOL CEnchantDialog::FakeMoveIcon(LONG x, LONG y, cIcon * pOrigIcon)
{
	if( WT_ITEM != pOrigIcon->GetType() ||
		pOrigIcon->IsLocked()			||
		m_bDisable )
	{
		return FALSE;
	}

	CItem * pOrigItem = (CItem *)pOrigIcon;

	// 인벤토리의 아이템만 허용된다
	if( ! ITEMMGR->IsEqualTableIdxForPos(eItemTable_Inventory, pOrigItem->GetPosition() ) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 787 ) );
		return FALSE;
	}

	// 090714 ShinJS --- 골드 소비 초기화
	m_pEnchantCost->SetStaticText( "" );

	const DWORD				itemIndex	= pOrigItem->GetItemIdx();
	const ITEM_INFO*		info		= ITEMMGR->GetItemInfo( itemIndex );
	
	// 인챈트 가능한 아이템이 올라오면
	if( info->IsEnchant )
	{
		// 080228 LUJ, 보호 모드일 때 인챈트 가능한 아이템인지 체크한다
		if( mProtectionItem.dwDBIdx )
		{
			const ITEM_OPTION& option = ITEMMGR->GetOption(
				pOrigItem->GetItemBaseInfo());
			const EnchantProtection& protection = g_CGameResourceManager.GetEnchantProtection();
			EnchantProtection::ItemMap::const_iterator it = protection.mItemMap.find(
				mProtectionItem.wIconIdx);

			if( protection.mItemMap.end() != it )
			{
				const EnchantProtection::Data& data = it->second;

				if( option.mEnchant.mLevel < data.mEnchantLevelRange.mMin ||
					option.mEnchant.mLevel > data.mEnchantLevelRange.mMax )
				{
					CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1453 ) );
					return FALSE;
				}
				// 080328 LUJ, 보호 가능한 아이템 장착 레벨 체크
				else if(	info->LimitLevel < data.mItemLimitLevelRange.mMin ||
							info->LimitLevel > data.mItemLimitLevelRange.mMax )
				{
					CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1421 ) );
					return FALSE;
				}
			}
		}		

		CVirtualItem* item = ( CVirtualItem* )mIconDialog->GetIconForIdx( 1 );

		if(item &&
			0 == IsEnchantable(ITEMMGR->GetItemInfoAbsIn(HERO, pOrigItem->GetPosition()), g_CGameResourceManager.GetEnchantScript( item->GetItemIdx())))
		{
			return FALSE;
		}
		else
		{
			if( ! mIconDialog->IsAddable( 0 ) )
			{
				CItem* old = ( CItem* )( mSourceItem->GetLinkItem() );

				if( ! old )
				{
					return FALSE;
				}

				ITEMMGR->RefreshItem( old );

				old->SetLock( FALSE );

				mIconDialog->DeleteIcon( 0, 0 );
			}

			// 정보 설정 및 툴팁 달자...
			{
				mSourceItem->SetData( pOrigItem->GetItemIdx() );
				mSourceItem->SetLinkItem( pOrigItem );

				ITEMMGR->AddToolTip( mSourceItem );

				mIconDialog->AddIcon( 0, mSourceItem );
			}	

			//pOrigItem->SetUseParam( 1 );
			pOrigItem->SetLock( TRUE );
		}
	}
	// 인챈트 불가능 Item이 올라온 경우
	else
	{	
		const EnchantScript* const script = g_CGameResourceManager.GetEnchantScript(
			itemIndex);

		if( ! script )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 906 ) );
			return FALSE;
		}

		{
			CVirtualItem* item = ( CVirtualItem* )mIconDialog->GetIconForIdx( 0 );
			
			if(		item &&
				!	IsEnchantable(	ITEMMGR->GetItemInfoAbsIn( HERO, item->GetSrcPosition() ),
									script ) )
			{
				return FALSE;
			}
		}

		// 이미 주문서 칸에 아이템이 등록되어 있으면 복구
		if( ! mIconDialog->IsAddable( 1 ) )
		{
			CItem* original = ITEMMGR->GetItem( mMaterialItem->GetDBIdx() );

			if( original )
			{
				ITEMMGR->RefreshItem( original );

				original->SetLock( FALSE );
			}

			mIconDialog->DeleteIcon( 1, 0 );
		}

		// 주문서 등록
		{
			pOrigItem->SetLock( TRUE );

			if( ITEMMGR->IsDupItem( itemIndex ) )
			{
				pOrigItem->SetDurability(
					pOrigItem->GetDurability());
				pOrigItem->SetVisibleDurability(
					TRUE);
			}
			else
			{
				pOrigItem->SetVisibleDurability( FALSE );
			}

			{
				ITEMBASE itemBase;
				ZeroMemory( &itemBase, sizeof( itemBase ) );
				itemBase.wIconIdx	= itemIndex;
				itemBase.Position	= pOrigItem->GetPosition();
				itemBase.Durability	= (ITEMMGR->IsDupItem( itemIndex ) ? pOrigItem->GetDurability() : 0);
				itemBase.dwDBIdx	= pOrigItem->GetDBIdx();

				cImage image;

				mMaterialItem->Init(
					0,
					0,
					DEFAULT_ICONSIZE,
					DEFAULT_ICONSIZE,
					ITEMMGR->GetIconImage( itemIndex, &image ),
					0 );

				mMaterialItem->SetIconType( eIconType_AllItem );
				mMaterialItem->SetData( itemIndex );
				mMaterialItem->SetItemBaseInfo( itemBase );
				mMaterialItem->SetVisibleDurability( ITEMMGR->IsDupItem( itemIndex ) );
				
				ITEMMGR->AddToolTip( mMaterialItem );
				mIconDialog->AddIcon( 1, mMaterialItem );
			}
		}
	}

	// 090714 ShinJS --- 아이템이 모두 등록된 경우 소모골드 출력
	if( !mIconDialog->IsAddable( 0 ) &&
		!mIconDialog->IsAddable( 1 ) )
	{
		const ITEM_INFO* const sourceItemInfo = ITEMMGR->GetItemInfo(
			mSourceItem->GetSrcItemIdx());

		if(0 == sourceItemInfo)
		{
			return FALSE;
		}

		m_pEnchantCost->SetStaticText(
			AddComma(g_CGameResourceManager.GetEnchantCost(*sourceItemInfo)));

		// 090917 ShinJS --- 보호아이템이 없고 아이템이 모두 등록된 경우 인챈트 실패시의 경고 메세지 출력
		if( mProtectionItem.dwDBIdx == 0 )
		{
			CItem* pSourceItem = ( CItem* )mSourceItem->GetLinkItem();
			if( !pSourceItem )			return FALSE;
			const ITEM_INFO* pItemInfo = pSourceItem->GetItemInfo();
			if( !pItemInfo )		return FALSE;
			const ITEM_OPTION& itemOption = ITEMMGR->GetOption( pSourceItem->GetItemBaseInfo() );
			const stEnchantFailSetting* const pFailSetting = g_CGameResourceManager.GetEnchantFailSetting(
				pItemInfo->kind);

			if(0 == pFailSetting)
			{
				return FALSE;
			}

			if( pFailSetting->dwDeleteItemMinLv <= itemOption.mEnchant.mLevel && itemOption.mEnchant.mLevel <= pFailSetting->dwDeleteItemMaxLv )
			{
				// 아이템 삭제되는 경우
				CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1932 ) );
			}
			else if( pFailSetting->dwZeroMinLv <= itemOption.mEnchant.mLevel && itemOption.mEnchant.mLevel <= pFailSetting->dwZeroMaxLv )
			{
				// 인챈트 레벨 0 적용되는 경우
				CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1933 ) );
			}
			else if( pFailSetting->dwManusMinLv <= itemOption.mEnchant.mLevel && itemOption.mEnchant.mLevel <= pFailSetting->dwManusMaxLv )
			{
				// 인챈트 레벨이 감소하는 경우
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1934 ) );
			}
		}
	}
	
	return FALSE;
}


void CEnchantDialog::OnActionEvent(LONG lId, void * p, DWORD we)
{
	switch( lId )
	{
	case ITD_UPGRADE_OKBTN:
		{
			if( m_bDisable )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 992 ) );
				break;
			}
			else if( mIconDialog->IsAddable( 0 ) )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 905 ) );
				break;
			}
			else if( mIconDialog->IsAddable( 1 ) )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 906 ) );
				break;
			}

			const ITEM_INFO* const sourceItemInfo = ITEMMGR->GetItemInfo(
				mSourceItem->GetSrcItemIdx());

			if(0 == sourceItemInfo)
			{
				break;
			}
			else if(HERO->GetMoney() < g_CGameResourceManager.GetEnchantCost(*sourceItemInfo))
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 324 ) );
				break;
			}

			// 090917 ShinJS --- 인챈트 시도 전 ItemOption 정보 저장
			CItem* pSourceItem = ( CItem* )( mSourceItem->GetLinkItem() );
			if( !pSourceItem )
				break;

			const ITEM_OPTION& option = ITEMMGR->GetOption( pSourceItem->GetItemBaseInfo() );
			m_PrevOption = option;


			// 080228 LUJ, 초기화
			ZeroMemory( &mMessage, sizeof( mMessage ) );
			
			mMessage.Category			= MP_ITEM;
			mMessage.Protocol			= MP_ITEM_ENCHANT_SYN;
			mMessage.dwObjectID			= HEROID;
			mMessage.ItemPos			= mSourceItem->GetSrcPosition();
			mMessage.wItemIdx			= mSourceItem->GetSrcItemIdx();
			mMessage.MaterialItemPos	= mMaterialItem->GetPosition();
			mMessage.wMaterialItemIdx	= mMaterialItem->GetItemIdx();

			// 080228 LUJ, 보호 아이템 정보 설정
			if( mProtectionItem.wIconIdx )
			{
				ICONBASE& protectionItem = mMessage.mProtectionItem;

				protectionItem.dwDBIdx	= mProtectionItem.dwDBIdx;
				protectionItem.wIconIdx	= mProtectionItem.wIconIdx;
				protectionItem.Position	= mProtectionItem.Position;
			}

			ITEMMGR->SetDisableDialog( TRUE, eItemTable_Inventory );
			SetDisable( TRUE );

			// 진행 표시가 끝난 후 서버에 작업을 요청할 것이다
			{
				CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
				ASSERT( dialog );

				dialog->Wait( CProgressDialog::eActionEnchant );
			}

			// 090921 ShinJS --- 결과 메세지 초기화
			mResult->SetStaticText( "" );
			
			break;
		}
	case ITD_UPGRADE_CANCELBTN:
		{
			if( m_bDisable )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 992 ) );
				break;
			}

			SetActive( FALSE );
			break;
		}
	}
}


BOOL CEnchantDialog::IsEnchantable( const ITEMBASE* item, const EnchantScript* script ) const
{
	const DWORD			itemIndex	= item->wIconIdx;
	const ITEM_INFO*	info		= ITEMMGR->GetItemInfo( itemIndex );

	if( !	info							||
		!	info->IsEnchant					||
			ITEMMGR->IsDupItem( itemIndex )	||
		!	script )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 907 ) );
		return FALSE;
	}

	const ITEM_OPTION& option = ITEMMGR->GetOption( *item );

	// 인챈트 최고 레벨에 도달했으면 불가
	if( option.mEnchant.mLevel == script->mEnchantMaxLevel )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 908 ) );
		return FALSE;
	}
	// 아이템이 인챈트 할 수 있는 범위인지 체크
	else if(	script->mItemLevel.mMin > info->LimitLevel ||
				script->mItemLevel.mMax	< info->LimitLevel )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 907 ) );
		return FALSE;
	}
	else if( script->mSlot.end() == script->mSlot.find( info->EquipSlot ) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 907 ) );
		return FALSE;
	}

	return TRUE;
}


void CEnchantDialog::SetActive( BOOL isActive )
{
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

	if( isActive )
	{
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
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1077 ) );
				return;
			}
			else if( stallShopDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1078 ) );
				return;
			}
			else if( exchangeDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1079 ) );
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

		// 인벤토리와 함께 표시시킨다
		// 091223 ShinJS --- 위치를 고정하지 않도록 수정.
		{
			cDialog* inventory = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( inventory );
			inventory->SetActive( TRUE );
		}

		// 초기화
		{
			mResult->SetStaticText( "" );
		}

		// 080228 LUJ, 일반 모드 표시
		{
			if( mTitle )
			{
				mTitle->SetActive( TRUE );
			}

			if( mProtectTitle )
			{
				mProtectTitle->SetActive( FALSE );
			}

			if( mProtectSymbol )
			{
				mProtectSymbol->SetActive( FALSE );
			}
		}

		// 090714 ShinJS --- 골드 소비 초기화
		m_pEnchantCost->SetStaticText( "" );
	}
	
	{
		if( ! mIconDialog->IsAddable( 0 ) )
		{
			CItem* item = ( CItem* )( mSourceItem->GetLinkItem() );
			ASSERT( item );

			if( ! item )
			{
				return;
			}

			item->SetLock( FALSE );
			
			mIconDialog->DeleteIcon( 0, 0 );
		}

		// 재료 아이템 복구
		if( ! mIconDialog->IsAddable( 1 ) )
		{
			if(CItem* CONST original = ITEMMGR->GetItem(mMaterialItem->GetDBIdx()))
			{
				original->SetLock( FALSE );
			}

			mIconDialog	->DeleteIcon( 1, 0 );
		}

		// 080228 LUJ, 보호 아이템을 초기화한다
		{
			CItem* item = ITEMMGR->GetItem( mProtectionItem.dwDBIdx );

			if( item )
			{
				item->SetLock( FALSE );

				ZeroMemory( &mProtectionItem, sizeof( mProtectionItem ) );
			}
		}
	}

	cDialog::SetActive( isActive );
}


// 080228 LUJ, 보호 아이템이 있을 경우의 성공 처리
void CEnchantDialog::Succeed( const MSG_ITEM_ENCHANT_ACK& message )
{
	Restore();

	ITEMMGR->AddOption( message.OptionInfo );
	RemoveMaterial();

	ITEMMGR->AddToolTip(
		mSourceItem);
	ITEMMGR->AddToolTip(
		(CItem*)mSourceItem->GetLinkItem());

	const ITEM_INFO* const info = ITEMMGR->GetItemInfo(
		mSourceItem->GetLinkItem()->GetItemIdx());

	CHATMGR->AddMsg(
		CTC_OPERATEITEM,
		CHATMGR->GetChatMsg(913),
		info->ItemName,
		message.OptionInfo.mEnchant.mLevel );

	mResult->SetStaticText(
		CHATMGR->GetChatMsg(974));

	if(CItem* item = ITEMMGR->GetItem(mMessage.mProtectionItem.dwDBIdx))
	{
		if(FALSE == item->GetItemInfo()->Stack ||
			1 ==  item->GetDurability())
		{
			CItem* deletedItem = 0;

			ITEMMGR->DeleteItem(
				item->GetPosition(),
				&deletedItem);

			SetActive(
				FALSE);
			mSourceItem->GetLinkItem()->SetLock(
				FALSE);

			if(CItem* const item = ITEMMGR->GetItem(mMaterialItem->GetDBIdx()))
			{
				item->SetLock(
					FALSE);
			}

			ZeroMemory( &mProtectionItem, sizeof( mProtectionItem ) );
		}
		else
		{
			item->SetDurability(
				item->GetDurability() - 1);
		}

		EnchantProtection::ItemMap::const_iterator it = g_CGameResourceManager.GetEnchantProtection().mItemMap.find(
			mProtectionItem.wIconIdx);

		// 대상 아이템의 보호 구간을 넘었는지 판단하여, 자동 해제시킨다
		if(g_CGameResourceManager.GetEnchantProtection().mItemMap.end() != it)
		{
			const EnchantProtection::Data& data = it->second;
			const ITEM_OPTION& itemOption = ITEMMGR->GetOption(
				mSourceItem->GetLinkItem()->GetDBIdx());

			if(itemOption.mEnchant.mLevel < data.mEnchantLevelRange.mMin ||
				itemOption.mEnchant.mLevel > data.mEnchantLevelRange.mMax)
			{
				mIconDialog->DeleteIcon(
					0,
					0);
				mSourceItem->GetLinkItem()->SetLock(
					FALSE);
			}
		}
	}
}


// 080228 LUJ, 보호 아이템이 있을 경우의 실패 처리 추가
// 080320 LUJ, 인챈트 제외 보호 모드(인챈트 수치가 초기화됨) 처리
void CEnchantDialog::Fail()
{
	Restore();
	RemoveMaterial();
	
	// 080228 LUJ, 보호 아이템을 사용한 경우 창을 닫는다
	if(CItem* item = ITEMMGR->GetItem( mMessage.mProtectionItem.dwDBIdx ))
	{
		if(FALSE == item->GetItemInfo()->Stack ||
			1 ==  item->GetDurability())
		{
			CItem* deletedItem = 0;

			ITEMMGR->DeleteItem(
				item->GetPosition(),
				&deletedItem);

			SetActive(
				FALSE);
			mSourceItem->GetLinkItem()->SetLock(
				FALSE);

			if(CItem* const item = ITEMMGR->GetItem(mMaterialItem->GetDBIdx()))
			{
				item->SetLock(
					FALSE);
			}

			ZeroMemory( &mProtectionItem, sizeof( mProtectionItem ) );
		}
		else
		{
			item->SetDurability(
				item->GetDurability() - 1);
		}

		CHATMGR->AddMsg(
			CTC_OPERATEITEM,
			CHATMGR->GetChatMsg(1450));
		ITEMMGR->AddToolTip(
			mSourceItem);
		ITEMMGR->AddToolTip(
			(CItem*)mSourceItem->GetLinkItem());
	}
	else
	{
		// 090917 ShinJS --- 실패한 경우 실패 적용정보를 읽어와서 해당 레벨에 맞는 작업을 진행한다.
		const ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(
			mSourceItem->GetLinkItem()->GetItemIdx());

		if(0 == pItemInfo)
		{
			return;
		}
		else if(const stEnchantFailSetting* const pFailSetting = g_CGameResourceManager.GetEnchantFailSetting(pItemInfo->kind))
		{
			if( pFailSetting->dwDeleteItemMinLv <= m_PrevOption.mEnchant.mLevel && m_PrevOption.mEnchant.mLevel <= pFailSetting->dwDeleteItemMaxLv )
			{
				// 아이템 삭제
				mIconDialog->DeleteIcon( 0, 0 );

				CItem* deletedItem = 0;
				ITEMMGR->DeleteItem( mSourceItem->GetSrcPosition(), &deletedItem );

				if(const ITEM_INFO* info = deletedItem->GetItemInfo())
				{
					CHATMGR->AddMsg(
						CTC_OPERATEITEM,
						CHATMGR->GetChatMsg(914),
						info->ItemName);
				}
			}
			else
			{
				if( pFailSetting->dwZeroMinLv <= m_PrevOption.mEnchant.mLevel && m_PrevOption.mEnchant.mLevel <= pFailSetting->dwZeroMaxLv )
				{
					// 인챈트 레벨 0 적용
					CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1935 ), pItemInfo->ItemName );
				}
				else if( pFailSetting->dwManusMinLv <= m_PrevOption.mEnchant.mLevel && m_PrevOption.mEnchant.mLevel <= pFailSetting->dwManusMaxLv )
				{
					// 인첸트 레벨 감소
					if( m_PrevOption.mEnchant.mLevel > 0 )
					{
						CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1936 ), m_PrevOption.mEnchant.mLevel - 1 );
					}
					else
					{
						CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1935 ), pItemInfo->ItemName );
					}
					
				}

				ITEMMGR->AddToolTip(
					mSourceItem);
				ITEMMGR->AddToolTip(
					(CItem*)mSourceItem->GetLinkItem());
			}
		}
		else
		{
			mIconDialog->DeleteIcon( 0, 0 );

			CItem* deletedItem = 0;
			ITEMMGR->DeleteItem( mSourceItem->GetSrcPosition(), &deletedItem );

			const ITEM_INFO* info = deletedItem->GetItemInfo();
			if( info )
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 914 ), info->ItemName );
		}
	}

	mResult->SetStaticText( CHATMGR->GetChatMsg( 975 ) );
}


void CEnchantDialog::Restore()
{
	SetDisable( FALSE );

	ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );
}


void CEnchantDialog::RemoveMaterial()
{
	CInventoryExDialog* const inventoryDialog = (CInventoryExDialog*)WINDOWMGR->GetWindowForID(
		IN_INVENTORYDLG);
	
	if(CItem* item = inventoryDialog->GetItemForPos(mMaterialItem->GetPosition()))
	{
		if(FALSE == ITEMMGR->IsDupItem(item->GetItemIdx()) ||
			1 == item->GetDurability())
		{
			CItem* deletedItem = 0;
			
			ITEMMGR->DeleteItem(
				mMaterialItem->GetPosition(),
				&deletedItem);

			mIconDialog->DeleteIcon(
				1,
				0);
		}
		else
		{
			item->SetDurability(
				item->GetDurability() - 1);
			mMaterialItem->SetDurability(
				mMaterialItem->GetDurability() - 1);
		}
	}
}


void CEnchantDialog::Send()
{
	// 080228 LUJ, 보호 아이템 사용 정보가 서버에서 전송하지 않으므로 클라이언트 정보를 활용하도록 복사해서 쓴다
	MSG_ITEM_ENCHANT_SYN message( mMessage );

	NETWORK->Send( &message, sizeof( message ) );
}


// 080228 LUJ, 보호 아이템 설정
void CEnchantDialog::SetProtectedActive( CItem& item )
{
	// 080228 LUJ, 보호 모드 표시
	{
		if( mTitle )
		{
			mTitle->SetActive( FALSE );
		}

		if( mProtectTitle )
		{
			mProtectTitle->SetActive( TRUE );
		}

		if( mProtectSymbol )
		{
			mProtectSymbol->SetActive( TRUE );
		}
	}

	mProtectionItem = item.GetItemBaseInfo();
	item.SetLock( TRUE );
}

DWORD CEnchantDialog::ActionEvent(CMouse* mouse)
{
	const DWORD windowEvent = cDialog::ActionEvent(
		mouse);

	if(FALSE == (WE_LBTNCLICK & windowEvent))
	{
		return windowEvent;
	}

	const POINT point = {mouse->GetMouseX(), mouse->GetMouseY()};

	for(POSTYPE position = 0; mIconDialog->GetCellNum() > position; ++position)
	{
		cIcon* const icon = mIconDialog->GetIconForIdx(
			position);

		if(0 == icon)
		{
			continue;
		}

		RECT rect = {0};
		rect.left = LONG(icon->GetAbsX());
		rect.top = LONG(icon->GetAbsY());
		rect.right = LONG(icon->GetAbsX()) + icon->GetWidth();
		rect.bottom = LONG(icon->GetAbsY()) + icon->GetHeight();

		if(FALSE == PtInRect(&rect, point))
		{
			continue;
		}
		else if(mSourceItem == icon)
		{
			mSourceItem->GetLinkItem()->SetLock(
				FALSE);
		}
		else if(mMaterialItem == icon)
		{
			if(CItem* const item = ITEMMGR->GetItem(mMaterialItem->GetDBIdx()))
			{
				item->SetLock(
					FALSE);
			}
		}

		mIconDialog->DeleteIcon(
			position,
			0);
		break;
	}

	return windowEvent;
}
#include "stdafx.h"
#include "ReinforceDlg.h"
#include "Item.h"
#include "VirtualItem.h"
#include "INTERFACE\cIconDialog.h"
#include "INTERFACE\cIconGridDialog.h"
#include "WindowIDEnum.h"
#include "ItemManager.h"
#include "ChatManager.h"
#include "Interface/cScriptManager.h"
#include "Interface/cWindowManager.h"
#include "ObjectManager.h"
#include "Interface/cStatic.h"
#include "InventoryExDialog.h"
#include "ProgressDialog.h"
#include "cMsgBox.h"
#include "GameIn.h"
#include "FishingDialog.h"

CReinforceDlg::CReinforceDlg() :
mItemIcon		( new CVirtualItem ),
mBaseDialog		( 0 ),
mIconGridDialog	( 0 ),
mTextQuantity	( 0 ),
mQuantity		( 0 ),
mMaxQuantity	( 100 ),
mIsReinforcing	( FALSE ),
// 080228 LUJ, 일반/보호 모드 표시위한 컨트롤 초기화
mTitle( 0 ),
mProtectTitle( 0 ),
mProtectSymbol( 0 )
{
	// 분해 대상 아이콘
	{
		cImage image;

		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );

		mItemIcon->SetToolTip( "", RGB_HALF( 255, 255, 255), &image );
		mItemIcon->SetMovable(FALSE);
	}

	// 080228 LUJ, 보호 아이템 초기화
	ZeroMemory( &mProtectionItem, sizeof( mProtectionItem ) );

	// 080228 LUJ, 강화 전 수치 초기화
	ZeroMemory( &mProtectedOption, sizeof( mProtectedOption ) );
	// 080929 LUJ, 보조 아이템 초기화
	ZeroMemory( &mSupportItem, sizeof( mSupportItem ) );
}


CReinforceDlg::~CReinforceDlg()
{
	Release();

	SAFE_DELETE( mItemIcon );
}


void CReinforceDlg::Linking()
{
	Release();

	mBaseDialog		= ( cIconDialog* )		GetWindowForID( ITR_REINFORCEICONDLG );
	mIconGridDialog = ( cIconGridDialog* )	GetWindowForID( ITR_REINFORCEICONGRIDDLG );

	{
		mTextQuantity	= (cStatic*)GetWindowForID( ITR_REINFORCEGRAVITY );
	
		cImage image;
		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );
		mTextQuantity->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &image, TTCLR_DEFAULT );
	}	

	// 아이콘 할당
	{
		int size = mIconGridDialog->GetCellNum();

		mMaterialIcon.reserve( size );

		ITEMBASE base;
		ZeroMemory( &base, sizeof( base ) );

		cImage image;

		while( size-- )
		{
			CItem* item = new CItem( &base );

			mMaterialIcon.push_back( item );
		}
	}

	{
		char line[ MAX_PATH ];
		sprintf( line, "%3d/%3d", mQuantity, mMaxQuantity );

		mTextQuantity->SetStaticText( line );
	}

	// 080228 LUJ, 일반/보호 모드 표시 용 인터페이스 링크
	{		
		mTitle			= GetWindowForID( ITR_REINFORCETITLE );
		mProtectTitle	= GetWindowForID( ITR_REINFORCE_PROTECT_TITLE );
		mProtectSymbol	= GetWindowForID( ITR_REINFORCE_PROTECT_SYMBOL );
	}
}


// 080228 LUJ,	보호 아이템을 사용한 경우, 강화 속성이 하나인 무기만 위치시킬 수 있도록 함.
//				보호 아이템을 사용한 경우, 한 종류의 보석만 배치할 수 있도록 함
BOOL CReinforceDlg::FakeMoveIcon(LONG x, LONG y, cIcon* icon)
{
	if( !	icon						||
			WT_ITEM != icon->GetType() ||
			mIsReinforcing )
	{
		return FALSE;
	}
	
	CItem* item = ( CItem* )icon;

	// 인벤토리이외에 아이템 FALSE
	if( ! ITEMMGR->IsEqualTableIdxForPos( eItemTable_Inventory, item->GetPosition() ) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 787 ) );
		return FALSE;
	}

	const ITEM_INFO* info = item->GetItemInfo();

	if( ! info )
	{
		return FALSE;
	}

	if( ITEMMGR->IsDupItem( item->GetItemIdx() ) )
	{
		AddMaterial( *item );
	}
	else if( info->Improvement )
	{
		// 080228 LUJ, 보호 아이템을 쓴 경우 속성이 하나만 강화된 장비만 올릴 수 있다
		if( mProtectionItem.dwDBIdx )
		{
			ITEM_OPTION option = ITEMMGR->GetOption( item->GetItemBaseInfo() );

			// 080228 LUJ, 인챈트된 값이 있을 경우 빼줘야 한다
			if(const EnchantScript* enchantScript = g_CGameResourceManager.GetEnchantScript(option.mEnchant.mIndex))
			{
				g_CGameResourceManager.AddEnchantValue(
					option,
					enchantScript->mAbility,
					-1 * info->EnchantDeterm * option.mEnchant.mLevel);
			}

			const ITEM_OPTION::Reinforce& protectedOption = option.mReinforce;

			// 080228 LUJ, 보호할 옵션이 없는 경우 강화하지 않는다
			{
				const ITEM_OPTION::Reinforce emptyOption = { 0 };

				if( ! memcmp( &emptyOption, &protectedOption, sizeof( emptyOption ) ) )
				{
					CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1453 ) );
					return FALSE;
				}
			}

			int count = 0;

			count = ( protectedOption.mStrength			? ++count : count );
			count = ( protectedOption.mDexterity		? ++count : count );
			count = ( protectedOption.mVitality			? ++count : count );
			count = ( protectedOption.mIntelligence		? ++count : count );
			count = ( protectedOption.mWisdom			? ++count : count );
			count = ( protectedOption.mLife				? ++count : count );
			count = ( protectedOption.mMana				? ++count : count );
			count = ( protectedOption.mManaRecovery		? ++count : count );
			count = ( protectedOption.mLifeRecovery		? ++count : count );
			count = ( protectedOption.mPhysicAttack		? ++count : count );
			count = ( protectedOption.mPhysicDefence	? ++count : count );
			count = ( protectedOption.mMagicAttack		? ++count : count );
			count = ( protectedOption.mMagicDefence		? ++count : count );	
			count = ( protectedOption.mCriticalRate		? ++count : count );
			count = ( protectedOption.mCriticalDamage	? ++count : count );
			count = ( protectedOption.mMoveSpeed		? ++count : count );
			count = ( protectedOption.mEvade			? ++count : count );
			count = ( protectedOption.mAccuracy			? ++count : count );

			if( 1 < count )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1457 ) );
				return FALSE;
			}			
		}

		// 080929 LUJ, 보조 아이템 사용 시 가능한 재료 정보를 읽어, 대상이 타당한지 검사한다
		{
			const ReinforceSupportScript* supportScript = g_CGameResourceManager.GetReinforceSupportScript( mSupportItem.wIconIdx );
			
			if( supportScript )
			{
				BOOL isValid = FALSE;

				for(	ReinforceSupportScript::MaterialSet::const_iterator it = supportScript->mMaterialSet.begin();
						supportScript->mMaterialSet.end() != it;
						++it )
				{
					const ReinforceScript* const script = g_CGameResourceManager.GetReinforceScript( *it );

					if( ! script )
					{
						continue;
					}

					const BOOL isSameRareType	= ( ITEMMGR->IsRare( info ) == script->mForRare );
					const BOOL isReinforceable	= ( script->mSlot.end() != script->mSlot.find(info->EquipSlot) );
					
					if( ! isSameRareType	||
						! isReinforceable )
					{
						continue;
					}

					isValid = TRUE;
					break;
				}

				if( ! isValid )
				{
					const ITEM_INFO*	supportItemInfo = ITEMMGR->GetItemInfo( mSupportItem.wIconIdx );
					const char*			text			= CHATMGR->GetChatMsg( 1718 );

					WINDOWMGR->MsgBox(
						MBI_NOTICE,
						MBT_OK,
						const_cast< char* >( text ),
						supportItemInfo ? supportItemInfo->ItemName : "?" );
					return FALSE;
				}
			}
		}

		RemoveMaterial();

		// 이미 등록된 아템이 있으면 복구
		if( ! mBaseDialog->IsAddable( 0 ) )
		{
			CItem* old = ( CItem* )( mItemIcon->GetLinkItem() );

			// 080108 LUJ, 플레이어 상태에 따라 초기화해야한다
			ITEMMGR->RefreshItem( old );

			old->SetLock( FALSE );
		}

		mBaseDialog->DeleteIcon( 0, 0 );

		mItemIcon->SetData( item->GetItemIdx()  );
		mItemIcon->SetLinkItem( item );
		
		ITEMMGR->AddToolTip( mItemIcon );

		mBaseDialog->AddIcon( 0, mItemIcon );

		//item->SetUseParam( 1 );
		item->SetLock(TRUE);
		
		ITEMMGR->AddToolTip( mItemIcon );
	}
	else
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(809) );
	}
	
	return FALSE;
}


void CReinforceDlg::OnActionEvent(LONG lId, void * p, DWORD we)
{
	switch( lId )
	{
	case ITR_REINFORCEOKBTN:
		{
			Submit( TRUE );
			break;
		}
	case ITR_REINFORCECANCELBTN:
		{
			SetActive( FALSE );
			break;
		}
	case ITR_RFGUIDEBTN:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( RFGUIDE_DLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
	}
}


void CReinforceDlg::Release()
{
	for(	MaterialIcon::iterator it = mMaterialIcon.begin();
			mMaterialIcon.end() != it;
			++it )
	{
		CItem* item = *it;

		SAFE_DELETE( item );
	}

	mMaterialIcon.clear();
}


// 080228 LUJ, 널포인터를 막기 위해 참조형으로 변경
// 080228 LUJ, 보호 아이템 사용 시 한 종류의 아이템만 등록하게 함
void CReinforceDlg::AddMaterial( CItem& item )
{
	// 강화할 아이템이 없으면 등록되지 않는다
	CItem* targetItem = 0;
	{
		cIcon* icon = mBaseDialog->GetIconForIdx( 0 );

		if( !	icon ||
				mItemIcon != icon )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(802) );
			return;
		}

		targetItem = ( CItem* )( mItemIcon->GetLinkItem() );

		if( !	targetItem &&
				WT_ITEM != targetItem->GetType())
		{
			return;
		}
	}

	const ReinforceScript* script = g_CGameResourceManager.GetReinforceScript( item.GetItemIdx() );

	if( ! script )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(803) );
		return;
	}

	const ReinforceSupportScript* supportScript = g_CGameResourceManager.GetReinforceSupportScript( mSupportItem.wIconIdx );

	// 080929 LUJ, 보조 아이템이 사용될 경우, 강화 보조 스크립트에서 재료가 사용 가능한지 체크한다
	if( supportScript )
	{
		if( supportScript->mMaterialSet.end() == supportScript->mMaterialSet.find( item.GetItemIdx() ) )
		{
			const ITEM_INFO*	supportItemInfo = ITEMMGR->GetItemInfo( mSupportItem.wIconIdx );
			const char*			text			= CHATMGR->GetChatMsg( 1718 );

			WINDOWMGR->MsgBox(
				MBI_NOTICE,
				MBT_OK,
				const_cast< char* >( text ),
				supportItemInfo ? supportItemInfo->ItemName : "?" );
			return;
		}
	}
	// 일반 아이템일 때는 일반 강화 재료가, 레어 아이템일 때는 레어 강화 재료가 등록되어야 한다.
	// 080929 LUJ, 보조 아이템이 사용되지 않는 경우에는 따로 처리한다
	else
	{
		CItem*		source = ( CItem* )( mItemIcon->GetLinkItem() );
		const BOOL	isRare = ITEMMGR->IsRare( source->GetItemInfo() );

		if( isRare && ! script->mForRare )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(804) );
			return;
		}

		if( ! isRare && script->mForRare )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 911 ) );
			return;
		}
	}

	// 해당 재료로 강화할 수 있는 아이템인지 검사
	{
		const ITEM_INFO* info = ITEMMGR->GetItemInfo( mItemIcon->GetSrcItemIdx() );
		
		if( script->mSlot.end() == script->mSlot.find( info->EquipSlot ) )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(803) );
			return;
		}
	}

	// 080228 LUJ, 보호 아이템을 사용한 경우, 한 종류의 재료만 올릴 수 있다
	if( mProtectionItem.dwDBIdx )
	{
		cIcon* icon = mIconGridDialog->GetIconForIdx( 0 );

		if( icon &&
			icon->GetData() != item.GetItemIdx() )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1458 ) );
			return;
		}

		// 080228 LUJ, 보호 상태일 경우 각각의 강화 옵션은 현재 위치한 보석이 강화시키는 속성과 일치해야함
		{
			// 080228 LUJ, 인챈트된 값이 있을 경우 빼줘야 한다
			ITEM_OPTION::Reinforce protectedOption = { 0 };
			{
				ITEM_OPTION	option = ITEMMGR->GetOption(
					targetItem->GetItemBaseInfo());
				const EnchantScript* enchantScript = g_CGameResourceManager.GetEnchantScript(
					option.mEnchant.mIndex);
				const ITEM_INFO* info = targetItem->GetItemInfo();

				if( info &&
					enchantScript )
				{
					g_CGameResourceManager.AddEnchantValue(
						option,
						enchantScript->mAbility,
						-1 * info->EnchantDeterm * option.mEnchant.mLevel);
				}

				protectedOption = option.mReinforce;

				// 080228 LUJ, 이름을 초기화하지 않으면, 비교할 때 항상 실패한다
				ZeroMemory( &( protectedOption.mMadeBy ), sizeof( protectedOption.mMadeBy ) );
			}

			// 080228 LUJ, 해당 보석이 강화하는 속성을 옵션 정보에 담고, 그 속성만 강화되었는지 확인한다
			{
				ITEM_OPTION::Reinforce option = { 0 };

				switch( script->mType )
				{
				case ReinforceScript::eTypeStrength:
					{
						option.mStrength = protectedOption.mStrength;
						break;
					}
				case ReinforceScript::eTypeDexterity:
					{
						option.mDexterity = protectedOption.mDexterity;
						break;
					}
				case ReinforceScript::eTypeVitality:
					{
						option.mVitality = protectedOption.mVitality;
						break;
					}
				case ReinforceScript::eTypeWisdom:
					{
						option.mWisdom = protectedOption.mWisdom;
						break;
					}
				case ReinforceScript::eTypeIntelligence:
					{
						option.mIntelligence = protectedOption.mIntelligence;
						break;
					}
				case ReinforceScript::eTypeLife:
					{
						option.mLife = protectedOption.mLife;
						break;
					}
				case ReinforceScript::eTypeLifeRecovery:
					{
						option.mLifeRecovery = protectedOption.mLifeRecovery;
						break;
					}
				case ReinforceScript::eTypeMana:
					{
						option.mMana = protectedOption.mMana;
						break;
					}
				case ReinforceScript::eTypeManaRecovery:
					{
						option.mManaRecovery = protectedOption.mManaRecovery;
						break;
					}
				case ReinforceScript::eTypePhysicAttack:
					{
						option.mPhysicAttack = protectedOption.mPhysicAttack;
						break;
					}
				case ReinforceScript::eTypePhysicDefence:
					{
						option.mPhysicDefence = protectedOption.mPhysicDefence;
						break;
					}
				case ReinforceScript::eTypeMagicAttack:
					{
						option.mMagicAttack = protectedOption.mMagicAttack;
						break;
					}
				case ReinforceScript::eTypeMagicDefence:
					{
						option.mMagicDefence = protectedOption.mMagicDefence;
						break;
					}
				case ReinforceScript::eTypeMoveSpeed:
					{
						option.mMoveSpeed = protectedOption.mMoveSpeed;
						break;
					}
				case ReinforceScript::eTypeEvade:
					{
						option.mEvade = protectedOption.mEvade;
						break;
					}
				case ReinforceScript::eTypeAccuracy:
					{
						option.mAccuracy = protectedOption.mAccuracy;
						break;
					}
				case ReinforceScript::eTypeCriticalRate:
					{
						option.mCriticalRate = protectedOption.mCriticalRate;
						break;
					}
				case ReinforceScript::eTypeCriticalDamage:
					{
						option.mCriticalDamage = protectedOption.mCriticalDamage;
						break;
					}
				}

				if( memcmp( &option, &protectedOption, sizeof( option ) ) )
				{
					CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1459 ) );
					return;
				}
			}
		}
	}

	for( WORD i = 0; i < mMaterialIcon.size(); ++i )
	{
		if( ! mIconGridDialog->IsAddable( i ) )
		{
			continue;
		}

		// 등록 개수 변경
		{
			if( mMaxQuantity < mQuantity + int( item.GetDurability() ) )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(805) );
				return;
			}

			mQuantity += int( item.GetDurability() );

			char line[ MAX_PATH ] = { 0 };
			sprintf( line, "%3d/%d", mQuantity, mMaxQuantity );

			mTextQuantity->SetStaticText( line );
		}

		// 재료 아이콘 등록		
		{
			CItem* material = mMaterialIcon[ i ];
			cImage image;

			material->Init(
				0,
				0,
				DEFAULT_ICONSIZE,
				DEFAULT_ICONSIZE,
				ITEMMGR->GetIconImage( item.GetItemIdx(), &image ),
				0 );

			material->SetIconType( eIconType_AllItem );
			material->SetData( item.GetItemIdx() );
			material->SetItemBaseInfo( item.GetItemBaseInfo() );

			// 080228 LUJ, 아이콘 이동을 금지시킨다
			material->SetMovable( FALSE );

			ITEMMGR->AddToolTip( material );
			mIconGridDialog->AddIcon( i, material );
		}

		// 원본 아이템에도 등록되었음을 알리기 위해 개수를 0으로 하고 잠금 표시한다
		{
			if( ITEMMGR->IsDupItem( item.GetItemIdx() ) )
			{
				item.SetDurability( 0 );
			}

			//item->SetUseParam( 1 );
			item.SetLock( TRUE );
		}

		return;
	}

	CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(806) );
}


void CReinforceDlg::Submit( BOOL isCheck )
{
	ZeroMemory( &mMessage, sizeof( mMessage ) );

	if( mIsReinforcing			||
		mBaseDialog->IsAddable( 0 ) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 802 ) );
		return;
	}
	
	{
		CBaseItem* item = mItemIcon->GetLinkItem();

		if( ! item )
		{
			return;
		}

		mMessage.Category				= MP_ITEM;
		mMessage.Protocol				= MP_ITEM_REINFORCE_SYN;
		mMessage.dwObjectID				= HEROID;
		mMessage.mSourceItemIndex		= item->GetItemIdx();
		mMessage.mSourceItemPosition	= item->GetPosition();
		mMessage.mSourceItemDbIndex		= item->GetDBIdx();
		mMessage.mSize					= 0;
		mMessage.mSupportItem			= mSupportItem;
	}	

	// 루프 돌아서 메시지에 저장한다(위치와 개수)
	for( WORD i = 0; i < mMaterialIcon.size(); ++i )
	{
		if( mIconGridDialog->IsAddable( i ) )
		{
			break;
		}

		// 데이터 추가
		{
			CItem* material = mMaterialIcon[ i ];

			mMessage.mItem[ mMessage.mSize++ ] = material->GetItemBaseInfo();
		}
	}

	if( ! mMessage.mSize )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 912 ) );

		ZeroMemory( &mMessage, sizeof( mMessage ) );
		return;
	}

	ITEMMGR->SetDisableDialog( TRUE, eItemTable_Inventory );
	SetDisable( TRUE );

	// 이미 강화 옵션이 있는 경우 메시지창을 통해 경고를 표시하도록 한다.
	{
		const ITEMBASE* item = ITEMMGR->GetItemInfoAbsIn( HERO, mItemIcon->GetSrcPosition() );

        if(		item									&&
			!	ITEMMGR->IsDupItem( item->wIconIdx )	&&
				item->Durability )
		{
			const ITEM_OPTION& option = ITEMMGR->GetOption(
				*item);
			const EnchantScript* enchantScript = g_CGameResourceManager.GetEnchantScript(
				option.mEnchant.mIndex);
			ITEM_OPTION	dummyOption	= option;

			if( enchantScript )
			{
				const ITEM_INFO* info = ITEMMGR->GetItemInfo( item->wIconIdx );

				if( ! info )
				{
					return;
				}
				
				g_CGameResourceManager.AddEnchantValue(
					dummyOption,
					enchantScript->mAbility,
					-1 * info->EnchantDeterm * option.mEnchant.mLevel);
			}

			if( isCheck )
			{
				const ITEM_OPTION::Reinforce emptyReinforce	= { 0 };

				if( memcmp( &emptyReinforce, &dummyOption.mReinforce, sizeof( emptyReinforce ) ) )
				{
					WINDOWMGR->MsgBox( MBI_ITEM_REINFORCE_CONFIRM, MBT_YESNO, CHATMGR->GetChatMsg( 1118 ) );
					return;
				}
			}				
		}
	}

	// 080228 LUJ, 보호 아이템 정보 설정
	if( mProtectionItem.dwDBIdx )
	{
		ICONBASE& protectionItem = mMessage.mProtectionItem;

		protectionItem.dwDBIdx	= mProtectionItem.dwDBIdx;
		protectionItem.wIconIdx	= mProtectionItem.wIconIdx;
		protectionItem.Position	= mProtectionItem.Position;
	}
	
	// 진행 표시가 끝난 후 서버에 작업을 요청할 것이다
	if( isCheck )
	{
		CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
		ASSERT( dialog );

		dialog->Wait( CProgressDialog::eActionReinforce );
	}
}


void CReinforceDlg::SetActive( BOOL isActive )
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
			// 080429 NYJ  낚시포인트교환중 사용불가
			// 080929 LUJ, 구체 클래스를 참조하지 않도록 수정
			{
				cDialog* dialog = WINDOWMGR->GetWindowForID( FISHINGPOINTDLG );

				if( dialog &&
					dialog->IsActive() )
				{
					CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
					return;
				}
			}
		}

		// 강화/인챈트/조합/분해/노점판매 창이 동시에 뜨는 것을 막는다
		{
			cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			cDialog* DissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
			cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
			
			ASSERT( mixDialog && DissoloveDialog && enchantDialog );

			mixDialog->SetActive( FALSE );
			DissoloveDialog->SetActive( FALSE );
			enchantDialog->SetActive( FALSE );
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

		// 080228 LUJ, 보호 모드 표시
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
	}
	// 080929 LUJ, 창이 닫힐 때 복구 처리
	else	
	{
		// 원본 아이템 복구
		if( ! mBaseDialog->IsAddable( 0 ) )
		{
			mBaseDialog->DeleteIcon( 0, 0 );

			CItem* item = ( CItem* )( mItemIcon->GetLinkItem() );
			
			if( item )
			{
				item->SetLock( FALSE );
			}
		}

		// 등록된 재료를 원상 복구시킨다
		RemoveMaterial();
		
		// 정보창 끄자
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( RFGUIDE_DLG );
			ASSERT( dialog );

			dialog->SetActive( FALSE );
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

		// 080925 LUJ, 보조 아이템 복구
		{
			CItem* item = ITEMMGR->GetItem( mSupportItem.dwDBIdx );

			if( item )
			{
				item->SetLock( FALSE );
			}

			ZeroMemory( &mSupportItem, sizeof( mSupportItem ) );
		}
	}

	cDialog::SetActive( isActive );
}


// 080228 LUJ, 보호 아이템이 적용된 경우 소모한다
void CReinforceDlg::Succeed( const MSG_ITEM_REINFORCE_OPTION_ACK& message )
{
	Restore();

	// 080228 LUJ, 옵션 보호 여부를 나타냄
	BOOL isProtected = FALSE;

	// 080228 LUJ, 보호 아이템 사용 시 처리
	if( mMessage.mProtectionItem.dwDBIdx )
	{
		// 080228 LUJ, 보호 아이템을 사용한 경우 창을 닫는다
		{
			CItem* item = ITEMMGR->GetItem( mMessage.mProtectionItem.dwDBIdx );

			if( item )
			{
				SetActive( FALSE );
			}
		}

		// 080228 LUJ, 옵션이 보호된 경우인지 조사한다
		{
			CItem* item = ITEMMGR->GetItem( mMessage.mSourceItemDbIndex );

			if( item )
			{
				ITEM_OPTION	option = ITEMMGR->GetOption(
					item->GetItemBaseInfo());
				const ITEM_INFO* const info = item->GetItemInfo();
				const EnchantScript* const enchantScript = g_CGameResourceManager.GetEnchantScript(
					option.mEnchant.mIndex);

				// 080228 LUJ, 인챈트된 값이 있을 경우 빼줘야 한다
				if( info &&
					enchantScript )
				{
					g_CGameResourceManager.AddEnchantValue(
						option,
						enchantScript->mAbility,
						-1 * info->EnchantDeterm * option.mEnchant.mLevel);
				}

				if( ! memcmp( &( option.mReinforce ), &( mProtectedOption.mReinforce ), sizeof( option.mReinforce ) ) )
				{
					isProtected = TRUE;
				}
			}
		}
	}
	
	// 080228 LUJ, 보호된 경우와 아닌 경우에 다른 메시지를 표시한다
	CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( isProtected ? 1451 : 807 ) );

	// 아이템의 옵션을 갱신해준다
	{
		ASSERT( ! mBaseDialog->IsAddable( 0 ) );

		CInventoryExDialog* inventory	= ( CInventoryExDialog* )( WINDOWMGR->GetWindowForID( IN_INVENTORYDLG ) ) ;
		CItem*				item		= inventory->GetItemForPos( mItemIcon->GetSrcPosition() );
		ASSERT( item );
		
		item->SetDBIdx( message.mOption.mItemDbIndex );
		
		ITEMMGR->AddToolTip( item );
		ITEMMGR->AddToolTip( mItemIcon );
	}
	
	{
		char line[ MAX_PATH ] = { 0 };
		sprintf( line, "%3d/%3d", mQuantity = 0, mMaxQuantity );

		mTextQuantity->SetStaticText( line );
	}

	// 080929 LUJ, 보조 아이템을 쓴 경우 창을 닫는다
	if( mSupportItem.wIconIdx )
	{
		SetActive( FALSE );
	}
}


// 080228 LUJ, 보호 모드 처리 추가
void CReinforceDlg::Fail()
{
	Restore();

	// 080228 LUJ, 보호 아이템을 사용한 경우 창을 닫는다
	if( mMessage.mProtectionItem.dwDBIdx )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1451 ) );

		CItem* item = ITEMMGR->GetItem( mMessage.mProtectionItem.dwDBIdx );

		if( item )
		{
			SetActive( FALSE );
		}
	}
	else
	{
		char line[ MAX_PATH ];
		sprintf( line, "%3d/%3d", mQuantity = 0, mMaxQuantity );

		mTextQuantity->SetStaticText( line );

		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 808 ) );
	}

	// 080929 LUJ, 보조 아이템을 쓴 경우 창을 닫는다
	if( mSupportItem.wIconIdx )
	{
		SetActive( FALSE );
	}
}


void CReinforceDlg::RemoveMaterial()
{
	// 재료 복구
	for( WORD i = 0; i < mMaterialIcon.size(); ++i )
	{
		if( mIconGridDialog->IsAddable( i ) )
		{
			break;
		}

		CItem*	material = mMaterialIcon[ i ];
		CItem*	original = ITEMMGR->GetItem( material->GetDBIdx() );

		if( original )
		{
			if( ITEMMGR->IsDupItem( material->GetItemIdx() ) )
			{
				original->SetDurability( material->GetDurability() + original->GetDurability() );
			}
			
			//original->SetUseParam( 0 );
			original->SetLock( FALSE );
		}

		mIconGridDialog->DeleteIcon( i, 0 );
	}

	{
		char line[ MAX_PATH ];
		sprintf( line, "%3d/%3d", mQuantity = 0, mMaxQuantity );

		mTextQuantity->SetStaticText( line );
	}
}


void CReinforceDlg::Restore()
{
	mIsReinforcing	= FALSE;

	ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );

	if( HERO->GetState() == eObjectState_Die )
	{
		SetActive( FALSE );
	}

	SetDisable( FALSE );
}


void CReinforceDlg::Send()
{
	Submit( FALSE );

	static const MSG_ITEM_REINFORCE_SYN emptyMessage;

	if( ! memcmp( &emptyMessage, &mMessage, sizeof( emptyMessage ) ) )
	{
		return;
	}

	// 080228 LUJ, 보호되는 경우 해당 상태의 메시지 출력을 위해 옵션을 저장해둔다
	{
		mProtectedOption = ITEMMGR->GetOption( mMessage.mSourceItemDbIndex );

		const EnchantScript* enchantScript = g_CGameResourceManager.GetEnchantScript(
			mProtectedOption.mEnchant.mIndex);
		const ITEM_INFO* info = ITEMMGR->GetItemInfo(
			mMessage.mSourceItemIndex);

		if( info &&
			enchantScript )
		{
			g_CGameResourceManager.AddEnchantValue(
				mProtectedOption,
				enchantScript->mAbility,
				-1 * info->EnchantDeterm * mProtectedOption.mEnchant.mLevel);
		}
	}

	MSG_ITEM_REINFORCE_SYN message( mMessage );

	NETWORK->Send( &message, message.GetSize() );

	SetDisable( FALSE );

	RemoveMaterial();
}


// 080228 LUJ, 보호 아이템 설정
void CReinforceDlg::SetProtectedActive( CItem& item )
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

	CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1455 ) );
}

// 080929 LUJ, 보조 아이템을 등록한다
void CReinforceDlg::SetSupportItem( const ITEMBASE& item )
{
	mSupportItem.wIconIdx	= item.wIconIdx;
	mSupportItem.dwDBIdx	= item.dwDBIdx;
	mSupportItem.Position	= item.Position;
}
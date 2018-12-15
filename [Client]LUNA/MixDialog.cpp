#include "stdafx.h"
#include "MixDialog.h"
#include "ChatManager.h"
#include "interface/cIconGridDialog.h"
#include "WindowIDEnum.h"
#include "Item.h"
#include "itemShow.h"
#include "ItemManager.h"
#include "InventoryExDialog.h"
#include "cIconDialog.h"
#include "interface/cStatic.h"
#include "Interface/cScriptManager.h"
#include "ObjectManager.h"
#include "Interface/cWindowManager.h"
#include "objectstatemanager.h"
#include "StreetStallManager.h"
#include "interface/cListDialog.h"
#include "cResourceManager.h"
#include "cDivideBox.h"
#include "cMsgBox.h"
#include "ProgressDialog.h"
#include "FishingManager.h"
#include "GameIn.h"
#include "FishingDialog.h"
#include "FishingPointDialog.h"
#include "interface/cSpin.h"
#include "interface/cComboBox.h"
#include "cIMEEX.h"

const DWORD DisableColor( RGBA_MAKE( 255, 100, 100, 0 ) );
const DWORD EnableColor( RGBA_MAKE( 255, 255, 255, 0 ) );


CMixDialog::CMixDialog() :
mDescription	( 0 ),
mResultIconDialog		( 0 ),
mIconGridDialog	( 0 ),
// 080228 LUJ, 일반/보호 모드 표시위한 컨트롤 초기화
mTitle( 0 ),
mProtectTitle( 0 ),
mProtectSymbol( 0 )
{
	// 080228 LUJ, 보호 아이템 정보 초기화
	ZeroMemory( &mProtectionItem, sizeof( mProtectionItem ) );
	// 080925 LUJ, 보조 아이템 정보 초기화
	ZeroMemory( &mSupportItem, sizeof( mSupportItem ) );

	// 090812 ShinJS --- 조합 인터페이스 수정, Category 정보 초기화
	for( int i=0 ; i<eMixDlgCategoryType_Max ; ++i )
	{
		m_CategoryDlg[ i ] = NULL;
		m_SelectedCategory[ i ] = 1;
		m_bChangedCategory[ i ] = TRUE;
	}

	m_pCntSpin = NULL;								// 개수 cSpin 초기화
	m_pInvenMoney = NULL;							// 소지금액 cStatic 초기화
	m_pMixCost = NULL;								// 조합비용 cStatic 초기화

	mResultIcon = NULL;

	m_pSearchNameEdit = NULL;
	m_pSearchMinLv = NULL;
	m_pSearchMaxLv = NULL;

	m_htCategoryInfo.Initialize( 20 );				// Category List Info Initialize
	m_htEqualSelectedMixResult.Initialize( 10 );	// 선택된 조합 아이템의 스크립트 Table (기본 재료아이템과 결과 아이템이 같고 조건이 다른 스크립트들 저장) 초기화
	m_pSelectedMixResult = NULL;					// 선택된 조합 아이템의 스크립트 초기화
	m_MixItemCnt = 1;								// 선택된 조합 아이템의 조합 개수 초기화
	m_eMixItemSortType = eMixItemSortType_ByName;	// 조합 아이템 List 정렬 방식 초기화
	m_eMixDialogState = eMixDialogState_Normal;		// 조합 Dialog 상태
	m_pSearchConditionInitBTN	=	NULL;			// 조건 초기화버튼

	m_dwMixResultCnt	=	0;
	m_bIsEnoughMoney	=	FALSE;
}


CMixDialog::~CMixDialog()
{
	// don't call Release() 
	// pre delete item 

	Release();
}


void CMixDialog::SetActiveRecursive(BOOL val)
{
	if( (STREETSTALLMGR->GetStallKind() == eSK_BUY && OBJECTSTATEMGR->GetObjectState(HERO) == eObjectState_StreetStall_Owner) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(789) );
		return;
	}
}

DWORD CMixDialog::ActionKeyboardEvent( CKeyboard* keyInfo )
{
	DWORD	we	=	WE_NULL;

	if( !m_bActive )
		return we;

	if( m_pSearchNameEdit->IsFocus() )
	{
		const char*	pSearchText	=	m_pSearchNameEdit->GetEditText();
		if( strlen( pSearchText ) >= LESSSTRINGLEN )
			SearchMixResultItem();
	}
	//조합창 조합개수 입력시 바로 필요/보유 개수및 가격 구함
	else if( m_pCntSpin->IsFocus() )
	{
		char	strItemCount[128]	=	{0, };
		strcpy( strItemCount, RemoveComma( m_pCntSpin->GetEditText() ) );
		DWORD	ItemCount	=	(DWORD)atoi( strItemCount );

		if( ItemCount > MAX_YOUNGYAKITEM_DUPNUM )
		{
			ItemCount = MAX_YOUNGYAKITEM_DUPNUM;
			m_pCntSpin->SetValue( ItemCount );
		}
		
		if( ItemCount != m_MixItemCnt )
		{
			m_MixItemCnt = ItemCount;
			UpdateOwnStuff();
			UpdateCostInfo();
		}
	}

	return	we;
}

void CMixDialog::Linking()
{
	Release();

	// 090812 ShinJS --- Category List Info Load
	LoadMixDlgCategory();

	mDescription		= (cListDialog*)GetWindowForID( ITMD_MIXDESCRIPTION );
	mResultIconDialog	= (cIconDialog*)GetWindowForID(ITMD_MIXICONDLG);
	mIconGridDialog		= (cIconGridDialog*)GetWindowForID(ITMD_MIXICONGRIDDLG);

	// 090812 ShinJS --- Category Dialog Link
	m_CategoryDlg[ eMixDlgCategoryType_1 ]	= (cListDialog*)GetWindowForID( ITMD_MIXCATEGORY1 );
	m_CategoryDlg[ eMixDlgCategoryType_2 ]	= (cListDialog*)GetWindowForID( ITMD_MIXCATEGORY2 );
	m_CategoryDlg[ eMixDlgCategoryType_3 ]	= (cListDialog*)GetWindowForID( ITMD_MIXCATEGORY3 );
	m_CategoryDlg[ eMixDlgCategoryType_4 ]	= (cListDialog*)GetWindowForID( ITMD_MIXCATEGORY4 );

	// 조합 개수 입력용 cSpin
	m_pCntSpin							= (cSpin*)GetWindowForID( ITMD_MIXCNTSPIN );
	if( m_pCntSpin )
	{
		m_pCntSpin->SetUnit( 1 );
		m_pCntSpin->SetValidCheck( VCM_NUMBER );
	}
	m_pInvenMoney						= (cStatic*)GetWindowForID( ITMD_INVEN_MONEY );
	m_pMixCost							= (cStatic*)GetWindowForID( ITMD_MIX_COST );
	m_pSortTypeCombo					= (cComboBox*)GetWindowForID(ITMD_SORT_TYPE_COMBOBOX);
	// 이름 정렬로 초기화
	SetSortType( eMixItemSortType_ByLevel );
	m_pSortTypeCombo->SelectComboText( eMixItemSortType_ByLevel );

	// 조건 초기화버튼	
	m_pSearchConditionInitBTN			= (cButton*)GetWindowForID( ITMD_MIXRESULT_SEARCH_RESET_BTN );

	for( int i=0 ; i<eMixDlgCategoryType_Max ; ++i )
	{
		// Category List Item 선택 가능 설정
		if( m_CategoryDlg[ i ] )
			m_CategoryDlg[ i ]->SetShowSelect( TRUE );
	}

	// 아이콘을 초기화한다
	{
		cImage imgToolTip;
		SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );

		// 결과 아이콘
		{
			ITEMBASE base;
			ZeroMemory( &base, sizeof( base ) );

			mResultIcon = new CItem( &base );
			mResultIcon->SetToolTip( "", RGB_HALF( 255, 255, 255), &imgToolTip );
			mResultIcon->SetMovable(FALSE);
		}

		// 재료 아이콘
		{
			int size = mIconGridDialog->GetCellNum();

			mStuffIcon.reserve( size );

			while( size-- )
			{
				CItemShow* item = new CItemShow;

				item->SetToolTip( "", RGB_HALF( 255, 255, 255), &imgToolTip );
				item->SetMovable(FALSE);
				item->SetVisibleDurability( FALSE );

				mStuffIcon.push_back( item );
			}
		}
	}

	// 080228 LUJ, 일반/보호 모드 표시위한 컨트롤 링크
	{
		mTitle			= GetWindowForID( ITMD_MIXICONTITLE );
		mProtectTitle	= GetWindowForID( ITMD_MIX_PROTECT_TITLE );
		mProtectSymbol	= GetWindowForID( ITMD_MIX_PROTECT_SYMBOL );
	}

	// 090812 ShinJS --- 조합 인터페이스 수정
	// Category ListDlg 초기화
	InitCategory();

	// 091124 ShinJS --- 검색 기능 추가
	m_pSearchNameEdit		= (cEditBox*)GetWindowForID( ITMD_MIXRESULT_SEARCH_NAME_EDIT );
	m_pSearchMinLv			= (cEditBox*)GetWindowForID( ITMD_MIXRESULT_SEARCH_LV_MIN_EDIT );
	m_pSearchMaxLv			= (cEditBox*)GetWindowForID( ITMD_MIXRESULT_SEARCH_LV_MAX_EDIT );

	// 091201 ShinJS --- Comma 없는 숫자 설정
	m_pSearchMinLv->SetValidCheck( VCM_NORMAL_NUMBER );
	m_pSearchMaxLv->SetValidCheck( VCM_NORMAL_NUMBER );
	m_pSearchMinLv->SetCheckMethodBaseStr( "" );
	m_pSearchMaxLv->SetCheckMethodBaseStr( "" );

}

BOOL CMixDialog::FakeMoveIcon( LONG x, LONG y, cIcon* icon )
{
	if( ! icon ||
		WT_ITEM != icon->GetType()	||
		icon->IsLocked() )
	{
		return FALSE;
	}

	CItem* item = ( CItem* )icon;

	// 인벤토리이외에 아이템 FALSE
	if( ! ITEMMGR->IsEqualTableIdxForPos( eItemTable_Inventory, item->GetPosition() ) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1168 ) );
		return FALSE;
	}

	// 조합 결과정보 제거
	RemoveResult();

	// 조합 Script로 Category Update
	m_dwDragItemIdx = item->GetItemIdx();
	UpdateCategoryByItemIdx( m_dwDragItemIdx );

	// 조합창을 최상위 Dialog로 만든다.
	WINDOWMGR->SetWindowTop( this );

	// Item Drag 상태로 설정
	m_eMixDialogState = eMixDialogState_Drag;

	return FALSE;
}

void CMixDialog::PutDescription( const ItemMixResult* result )
{
	// 090911 ShinJS --- 인자 확인 추가
	if( !result )
		return;

	const ITEM_INFO* info = ITEMMGR->GetItemInfo( result->mItemIndex );
	ASSERT( info );

	const std::string prefixProduct	( RESRCMGR->GetMsg( 626 ) ); // "제작품목: ";
	const std::string prefixLevel	( RESRCMGR->GetMsg( 627 ) ); // "제한레벨: ";
	const std::string prefixMaterial( RESRCMGR->GetMsg( 628 ) ); // "필요재료: ";
	const std::string prefixMoney	( RESRCMGR->GetMsg( 629 ) ); // "제작비용: ";
	const std::string prefixSuccess	( CHATMGR->GetChatMsg(1464) );
	const std::string postfixList	( RESRCMGR->GetMsg( 630 ) ); // "개";

	CHero*		hero = OBJECTMGR->GetHero();
	const DWORD textColor( RGBA_MAKE( 255, 255, 255, 0 ) );
	char		line[ MAX_TEXT_SIZE ];

	mDescription->RemoveAll();

	// 제작 품목 표시
	{
		sprintf( line, "%s%s", prefixProduct.c_str(), info->ItemName );

		mDescription->AddItemAutoLine( line, ITEMMGR->GetItemNameColor( *info ) );
	}

	// 레벨 표시
	{
		if( result->mRequiredLevel > hero->GetLevel() )
		{
			sprintf( line, "%s%d", prefixLevel.c_str(), result->mRequiredLevel );
			mDescription->AddItemAutoLine( line, DisableColor );
		}
	}

	// 비용 표시
	{
		sprintf( line, "%s%s", prefixMoney.c_str(), AddComma( result->mMoney ) );

		mDescription->AddItemAutoLine( line, ( result->mMoney > hero->GetMoney() ? DisableColor : EnableColor ) );
	}

	// 080925 LUJ, 성공율이 0~100% 범위로 표시되도록 한다
	{
		const MixSupportScript* script				= ITEMMGR->GetMixSupportScript( mSupportItem.wIconIdx );
		const float				bonusSuccessRate	= ( script ? script->mBonusSucessRate : 0 );
		sprintf( line, prefixSuccess.c_str(), min( 100.0f, max( 0, ( bonusSuccessRate + result->mSuccessRate ) * 100.0f ) ) );

		mDescription->AddItemAutoLine( line, textColor );
	}

	typedef std::list< std::string >	Description;
	Description							description;

	// 결과 아이템
	{
		sprintf( line, "%s", prefixMaterial.c_str() );

		mDescription->AddItemAutoLine( line, textColor );

		std::string tab;

		// 재료 표시의 절반 만큼을 여백으로 한다
		for( int i = strlen( prefixMaterial.c_str() ) / 2; i--; )
		{
			tab += " ";
		}

		for(ItemMixResult::Material::const_iterator it = result->mMaterial.begin();
			result->mMaterial.end() != it;
			++it )
		{
			const DWORD itemIndex	= it->first;
			const DWORD quantity	= it->second;

			const ITEM_INFO* materialInfo = ITEMMGR->GetItemInfo( itemIndex );

			sprintf( line, "%s%s %d%s", tab.c_str(), materialInfo->ItemName, quantity, postfixList.c_str() );

			mDescription->AddItemAutoLine( line, textColor );
		}
	}

	mDescription->SetPosition( 0 );
}

void CMixDialog::RemoveResult()
{
	mDescription->RemoveAll();

	for(WORD i = 0 ; i < mIconGridDialog->GetCellNum() ; ++i )
	{
		mIconGridDialog->DeleteIcon( i, 0 );
	}

	mResultIconDialog->DeleteIcon( 0, 0 );
}


void CMixDialog::Submit( DURTYPE mixSize, BOOL isCheck )
{
	if( ! mixSize )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1085 ) );
		return;
	}

	UpdateCostInfo();

	// 090817 ShinJS --- 선택된 조합 스크립트가 없는 경우
	if( !m_pSelectedMixResult )
	{
		// 조합 아이템 선택 요청 메세지 출력
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 791 ) );
		return ;
	}

	const DWORD sourceItemIndex = m_pSelectedMixResult->mBaseMaterial;
	CItem* sourceItem = GAMEIN->GetInventoryDialog()->GetItemForIdx(
		sourceItemIndex,
		WORD(-1));
	const POSTYPE inventoryStartPosition = TP_INVENTORY_START;
	const POSTYPE inventoryEndPosition = POSTYPE(TP_INVENTORY_END + TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount());

	// 기본 아이템 존재?
	if( ! sourceItem )
	{
		// 재료 부족 메세지 출력
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 784 ) );
		return ;
	}

	CHero* hero = OBJECTMGR->GetHero();
	ASSERT( hero );

	if( m_pSelectedMixResult->mMoney * mixSize > HERO->GetMoney() )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 324 ) );
		return ;
	}
	else if( hero->GetLevel() < m_pSelectedMixResult->mRequiredLevel )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 792 ) );
		return;
	}

	ZeroMemory( &mMessage, sizeof( mMessage ) );

	const DWORD					sourceItemDBIndex	= sourceItem->GetDBIdx();
	MSG_ITEM_MIX_SYN::Material*	material			= mMessage.mMaterial;
	ItemMixResult::Material		requiredMaterial;

	// 필요 개수를 채우자... 
	for(
		ItemMixResult::Material::const_iterator it = m_pSelectedMixResult->mMaterial.begin();
		m_pSelectedMixResult->mMaterial.end() != it;
		++it )
	{
		const DWORD		itemIndex	= it->first;
		const DURTYPE	quantity	= it->second;

		requiredMaterial[ itemIndex ] = quantity * mixSize;

		if( ! ITEMMGR->GetItemInfo( itemIndex ) )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1271 ) );

			ZeroMemory( &mMessage, sizeof( mMessage ) );
			return;
		}
	}

	CInventoryExDialog* inventory = ( CInventoryExDialog* )WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
	ASSERT( inventory );

	// 재료 정보를 수집한다
	for(
		POSTYPE position = inventoryStartPosition;
		inventoryEndPosition > position;		
		++position )
	{
		if( requiredMaterial.empty() )
		{
			break;
		}

		CItem* item = inventory->GetItemForPos( position );

		if( ! item )
		{
			continue;
		}

		const DWORD	itemIndex	= item->GetItemIdx();
		const BOOL	isDuplicate = ITEMMGR->IsDupItem( itemIndex );
		const DWORD quantity	= ( isDuplicate ? item->GetDurability() : 1 );

		if( 1 > quantity													||
			requiredMaterial.end() == requiredMaterial.find( itemIndex )	||
			item->IsLocked() )
		{
			continue;
		}

		DWORD& remainedQuantity = requiredMaterial[ itemIndex ];

		// 남은 개수에서 현재 슬롯의 개수를 뺀다
		// 남은 개수가 0이하인 경우 슬롯의 개수를 이전 남은 개수로 한다.
		{
			const DWORD previousRemainedQuantity = remainedQuantity;

			remainedQuantity		= max( 0, int( remainedQuantity - quantity ) );

			//material->mPosition		= item->GetPosition();
			material->mPosition		= position;
			material->mItemIndex	= itemIndex;
			material->mQuantity		= ( isDuplicate ? min( previousRemainedQuantity, quantity ) : 1 );

			if( sourceItemDBIndex == item->GetDBIdx() )
			{
				mMessage.mSourceIndex = material - mMessage.mMaterial;

				if( isDuplicate )
				{
					++material->mQuantity;
				}
			}

			++mMessage.mSourceSize;
			++material;
		}

		if( ! remainedQuantity )
		{
			requiredMaterial.erase( itemIndex );
		}
	}

	// 저장 위치를 계산한다.
	{
		typedef MSG_ITEM_MIX_SYN::Material	Material;
		typedef std::map< DWORD, Material > InventoryMap;
		InventoryMap						inventoryMap;

		// 인벤토리 정보 복사
		for(
			POSTYPE position = inventoryStartPosition;
			inventoryEndPosition > position;
			++position )
		{
			CItem* item = inventory->GetItemForPos( position );

			if( item )
			{
				const BOOL isDuplicate = ITEMMGR->IsDupItem( item->GetItemIdx() );

				Material& slot = inventoryMap[ position ];

				slot.mItemIndex	= item->GetItemIdx();
				slot.mPosition	= position;
				slot.mQuantity	= ( isDuplicate ? item->GetDurability() : 1 );
			}
		}

		// 재료 정보를 조회해가면서 인벤토리 정보를 갱신한다(소요된 재료를 차감한다)
		for( DWORD i = mMessage.mSourceSize; i--; )
		{
			const Material& source		= mMessage.mMaterial[ i ];
			const POSTYPE	position	= source.mPosition;

			if( inventoryMap.end() == inventoryMap.find( position ) )
			{
				continue;
			}

			Material& slot = inventoryMap[ position ];
			ASSERT( slot.mQuantity >= source.mQuantity );

			slot.mQuantity -= source.mQuantity;

			if( slot.mQuantity ||
				slot.mPosition == sourceItem->GetPosition() )
			{
				continue;
			}

			inventoryMap.erase( position );
		}

		MSG_ITEM_MIX_SYN::Material*	resultSlot			= mMessage.mMaterial + mMessage.mSourceSize;
		int							resultSize			= int( mixSize );
		const BOOL					isResultDuplicate	= ITEMMGR->IsDupItem( m_pSelectedMixResult->mItemIndex );
		// 091215 ONS 아이템Stack최대수를 아이템별로 지정된 값으로 처리한다.
		const WORD wItemStackNum = ITEMMGR->GetItemStackNum( m_pSelectedMixResult->mItemIndex );

		// 겹치는 아이템이면
		if( isResultDuplicate )
		{
			// 일단 같은 아이템 인덱스를 찾아 여유공간만큼 때려 넣는다
			for(
				POSTYPE position = inventoryStartPosition;
				inventoryEndPosition > position;
				++position )
			{
				if( ! resultSize )
				{
					break;
				}
				else if( inventoryMap.end() == inventoryMap.find( position ) )
				{
					continue;
				}

				const Material& slot		= inventoryMap[ position ];
				const int		enableSize	= int( wItemStackNum - slot.mQuantity );

				// 071213 LUJ,	재료 아이템인 경우 결과를 업데이트할 곳으로 선정하지 않는다
				//				DB 랙으로 삽입/삭제가 뒤바뀌어 진행될 경우 성공해도 아이템을 삭제하는 경우가 발생한다
				if(	m_pSelectedMixResult->mItemIndex != slot.mItemIndex	||
					0 >=	enableSize						||
					position == sourceItem->GetPosition() )
				{
					continue;
				}

				resultSlot->mItemIndex	= m_pSelectedMixResult->mItemIndex;
				resultSlot->mQuantity	= min( resultSize, enableSize );
				resultSlot->mPosition	= position;

				resultSize	-= resultSlot->mQuantity;

				++resultSlot;
			}
		}

		// 겹치는 아이템이 아니거나, 남은 겹치는 아이템이 있을 경우 채운다.
		for(
			POSTYPE position = inventoryStartPosition;
			inventoryEndPosition > position;
			++position )
		{
			if( ! resultSize )
			{
				break;
			}
			else if( inventoryMap.end() != inventoryMap.find( position ) )
			{
				continue;
			}

			resultSlot->mItemIndex	= m_pSelectedMixResult->mItemIndex;
			resultSlot->mQuantity	= ( isResultDuplicate ? min( resultSize, wItemStackNum ) : 1 );
			resultSlot->mPosition	= position;

			resultSize				-= resultSlot->mQuantity;
			++resultSlot;
		}

		if( resultSize )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 910 ) );

			ZeroMemory( &mMessage, sizeof( mMessage ) );
			return;
		}

		mMessage.mResultSize = resultSlot - ( mMessage.mMaterial + mMessage.mSourceSize );
	}

	if( ! requiredMaterial.empty() )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 784 ) );

		ZeroMemory( &mMessage, sizeof( mMessage ) );
		return;
	}

	{
		// 090812 ShinJS --- MixScript List에서 현재 선택된 MixResult의 위치를 구한다
		const DWORD dwMixScriptPos = ITEMMGR->GetMixResultPosInfo( m_pSelectedMixResult->mBaseMaterial, m_pSelectedMixResult );
		if( dwMixScriptPos == (DWORD)-1 )
			return;

		mMessage.Category			= MP_ITEM;
		mMessage.Protocol			= MP_ITEM_MIX_SYN;
		mMessage.dwObjectID			= gHeroID;
		mMessage.mResultIndex		= dwMixScriptPos;
		mMessage.mMixSize			= mixSize;
	}

	// 080228 LUJ, 보호 아이템 정보 설정
	if( mProtectionItem.wIconIdx )
	{
		ICONBASE& protectionItem = mMessage.mProtectionItem;

		protectionItem.dwDBIdx	= mProtectionItem.dwDBIdx;
		protectionItem.wIconIdx	= mProtectionItem.wIconIdx;
		protectionItem.Position	= mProtectionItem.Position;
	}

	if( isCheck )
	{
		if( 1 < mixSize )
		{
			const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo( m_pSelectedMixResult->mItemIndex );
			ASSERT( itemInfo );

			WINDOWMGR->MsgBox( MBI_ITEM_MIX_CONFIRM, MBT_YESNO, CHATMGR	->GetChatMsg( 1086 ), itemInfo->ItemName, mixSize );
		}
		// 진행 표시가 끝난 후 서버에 작업을 요청할 것이다
		else
		{
			CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
			ASSERT( dialog );

			dialog->Wait( CProgressDialog::eActionMix );
		}

		SetDisable( TRUE );
	}
}

void CMixDialog::OnActionEvent( LONG lId, void * p, DWORD we )
{
	CatagoryScrollEnd();
	switch( lId )
	{
		// 조합 시도
	case ITMD_MIXOKBTN:
		{
			// 090817 ShinJS --- 선택된 조합 아이템이 없는 경우
			if( !m_pSelectedMixResult )
			{
				// 조합 아이템 선택 요청 메세지 출력
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 791 ) );
				return;
			}

			// 080925 LUJ, 보조 아이템이 사용된 경우 무기/방어구/액세서리만 조합 가능하다
			if( mSupportItem.wIconIdx )
			{
				const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo( m_pSelectedMixResult->mItemIndex );

				if( ! itemInfo )
				{
					return;
				}
				else if(	( eEquipType_Accessary	!= itemInfo->EquipType ) &&
							( eEquipType_Weapon		!= itemInfo->EquipType ) &&
							( eEquipType_Armor		!= itemInfo->EquipType ) )
				{
					WINDOWMGR->MsgBox(
						MBI_NOTICE,
						MBT_OK,
						CHATMGR->GetChatMsg( 1714 ) );
					return;
				}
			}

			// 090817 ShinJS --- 조합 개수 설정
			if( m_pCntSpin )
			{
				DURTYPE mixItemCnt = (DURTYPE)m_pCntSpin->GetValue();
				if( mixItemCnt > MAX_YOUNGYAKITEM_DUPNUM )
					mixItemCnt = MAX_YOUNGYAKITEM_DUPNUM;
				m_pCntSpin->SetValue( mixItemCnt );
				m_MixItemCnt = mixItemCnt;
			}
			
			Submit( m_MixItemCnt, TRUE );
		}
		break;
		// 080925 LUJ, 닫기 버튼 처리 추가
	case ITMD_MIXDLG:
	case ITMD_MIXCANCELBTN:
		{
			//CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 785 ) );

			SetActive( FALSE );
		}
		break;		

		// 090812 ShinJS --- 카테고리 선택
	case ITMD_MIXCATEGORY1:
	case ITMD_MIXCATEGORY2:
	case ITMD_MIXCATEGORY3:
	case ITMD_MIXCATEGORY4:
		{
			if( we == WE_ROWCLICK )
			{
				ITEM* pItem = GetSelectedItemFromListDlg( lId );
				if( !pItem )
					break;

				UpdateCategory();
			}
		}
		break;

	case ITMD_MIXCNTSPIN:
		{
			// 조합 아이템 개수가 변경된 경우 재료현황 Update
			DURTYPE mixItemCnt = (DURTYPE)m_pCntSpin->GetValue();
			if( mixItemCnt > MAX_YOUNGYAKITEM_DUPNUM )
			{
				mixItemCnt = MAX_YOUNGYAKITEM_DUPNUM;
				m_pCntSpin->SetValue( mixItemCnt );
			}
			
			if( mixItemCnt != m_MixItemCnt )
			{
				m_MixItemCnt = mixItemCnt;
				UpdateOwnStuff();
				UpdateCostInfo();
			}
		}
		break;

	case ITMD_SORT_TYPE_COMBOBOX:
		{
			if( we != WE_COMBOBOXSELECT )
				break;

			// 정렬 Type 설정			
			SetSortType( eMixItemSortType( m_pSortTypeCombo->GetCurSelectedIdx() ) );
		}
		break;

	case ITMD_MIXRESULT_SEARCH_RESET_BTN:
		{
			// 검색 초기화
			if( we != WE_BTNCLICK )
				break;
			
			DWORD	dwMixCost	=	0;
			m_pSearchNameEdit->SetEditText( "" );
			m_pSearchMinLv->SetEditText( "" );
			m_pSearchMaxLv->SetEditText( "" );
			m_pCntSpin->SetValue(0);
			m_pMixCost->SetStaticText( AddComma( dwMixCost ) );

			// 결과 제거
			RemoveResult();

			// 카테고리 변경시 선택된 스크립트 초기화
			ClearSelectedCategoryInfo();

			// 카테고리 초기화
			m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetCurSelectedRowIdx( -1 );
			m_CategoryDlg[ eMixDlgCategoryType_2 ]->RemoveAll();
			m_CategoryDlg[ eMixDlgCategoryType_3 ]->RemoveAll();
			m_CategoryDlg[ eMixDlgCategoryType_4 ]->RemoveAll();

			m_SelectedCategory[ eMixDlgCategoryType_1 ] = -1;
			m_SelectedCategory[ eMixDlgCategoryType_2 ] = -1;
			m_SelectedCategory[ eMixDlgCategoryType_3 ] = -1;
			m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;
		}
		break;

	case ITMD_MIXRESULT_SEARCH_NAME_EDIT:
	case ITMD_MIXRESULT_SEARCH_LV_MIN_EDIT:
	case ITMD_MIXRESULT_SEARCH_LV_MAX_EDIT:
	case ITMD_MIXRESULT_SEARCH_BTN:
		{
			// 아이템 검색
			if( !((we & WE_RETURN) || (we & WE_BTNCLICK)) )
				break;

			// 검색 상태로 설정
			m_eMixDialogState = eMixDialogState_Search;

			SearchMixResultItem();
		}
		break;

	case	ITMD_MIXCATEGORY1_BTN:
		{
			PushCatagory1BTN();
		}
		break;
	case	ITMD_MIXCATEGORY2_BTN:
		{
			PushCatagory2BTN();
		}
		break;
	}
}

void CMixDialog::Release()
{
	{
		for(std::vector<CItemShow*>::iterator it = mStuffIcon.begin();
			mStuffIcon.end() != it;
			++it )
		{
			CItemShow* item = *it;

			SAFE_DELETE( item );
		}

		mStuffIcon.clear();
	}

	SAFE_DELETE( mResultIcon );

	m_htCategoryInfo.SetPositionHead();

	for(stCategoryInfo* pInfo = m_htCategoryInfo.GetData();
		0 < pInfo;
		pInfo = m_htCategoryInfo.GetData())
	{
		SAFE_DELETE( pInfo );
	}
	m_htCategoryInfo.RemoveAll();

	m_htEqualSelectedMixResult.RemoveAll();
}

// 080228 LUJ, 보호 아이템이 적용된 경우 소모한다
void CMixDialog::Succeed( const MSG_ITEM_MIX_ACK& message )
{
	Restore();

	// 080228 LUJ, 보호 아이템을 사용한 경우 창을 닫는다
	if( mMessage.mProtectionItem.dwDBIdx )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1452 ) );

		CItem* item = ITEMMGR->GetItem( mMessage.mProtectionItem.dwDBIdx );

		if( item )
		{
			SetActive( FALSE );
		}
	}

	// 080925 LUJ, 보조 아이템을 사용한 경우 창을 닫는다
	if( mSupportItem.wIconIdx )
	{
		SetActive( FALSE );
	}
}


// 080228 LUJ, 보호 모드 처리 추가
void CMixDialog::Fail( const MSG_ITEM_MIX_ACK& message )
{
	Restore();

	// 080228 LUJ, 보호 아이템을 사용한 경우 창을 닫는다
	if( mMessage.mProtectionItem.dwDBIdx )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1449 ) );

		CItem* item = ITEMMGR->GetItem( mMessage.mProtectionItem.dwDBIdx );

		if( item )
		{
			SetActive( FALSE );
		}
	}
	else
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 812 ) );
	}

	// 080925 LUJ, 보조 아이템을 사용한 경우 창을 닫는다
	if( mSupportItem.wIconIdx )
	{
		SetActive( FALSE );
	}
}


void CMixDialog::SetActive( BOOL isActive )
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
			cDialog* npcScriptDialog			= WINDOWMGR->GetWindowForID( NSI_SCRIPTDLG );
			cDialog* npcShopDialog				= WINDOWMGR->GetWindowForID( DE_DEALERDLG );
			cDialog* stallShopDialog			= WINDOWMGR->GetWindowForID( SSI_STALLDLG );
			cDialog* exchangeDialog				= WINDOWMGR->GetWindowForID( XCI_DIALOG );
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
			cDialog* DissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
			cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
			cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );

			ASSERT( DissoloveDialog && enchantDialog && reinforceDialog );

			DissoloveDialog->SetActive( FALSE );
			enchantDialog->SetActive( FALSE );
			reinforceDialog->SetActive( FALSE );
		}

		mIconGridDialog->SetCurSelCellPos( -1 );

		// 인벤토리와 함께 중앙에 표시시킨다
		{
			cDialog* inventory = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( inventory );

			const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
			const int			space	=  screen.dwWidth > DWORD(m_width + inventory->GetWidth() + 140) ? 140 :screen.dwWidth - m_width - inventory->GetWidth();
			const DWORD			x		= ( screen.dwWidth - m_width - inventory->GetWidth() - space ) / 2;
			const DWORD			y		= ( screen.dwHeight - max( inventory->GetHeight(), m_height ) ) / 2;

			SetAbsXY( x + space + inventory->GetWidth(), y );

			inventory->SetAbsXY( x, y );
			inventory->SetActive( TRUE );
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

		// 090817 ShinJS --- 선택된 조합 아이템의 조합 개수 초기화
		m_MixItemCnt = 1;
		m_pCntSpin->SetValue( 1 );

		InitCategory();
		UpdateCostInfo();
	}
	// 080925 LUJ, 창이 닫힐 때 처리
	else
	{
		// 080925 LUJ, 보조 아이템 복구
		{
			CItem* item = ITEMMGR->GetItem( mSupportItem.dwDBIdx );

			if( item )
			{
				item->SetLock( FALSE );
			}

			ZeroMemory( &mSupportItem, sizeof( mSupportItem ) );
		}

		RemoveResult();

		mResultIconDialog->DeleteIcon( 0, 0 );

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
	
	m_pSearchNameEdit->SetEditText( "" );

	cDialog::SetActive( isActive );
}

void CMixDialog::Restore()
{
	//mIsMixing	 = FALSE;

	ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );

	if( HERO->GetState() == eObjectState_Die )
	{
		SetActive( FALSE );
	}

	SetDisable( FALSE );
}

void CMixDialog::Send()
{
	Submit( mMessage.mMixSize, FALSE );

	static const MSG_ITEM_MIX_SYN emptyMessage;

	if( ! memcmp( &emptyMessage, &mMessage, sizeof( emptyMessage ) ) )
	{
		return;
	}

	MSG_ITEM_MIX_SYN2 message;
	{
		ZeroMemory( &message, sizeof( message ) );

		if( !m_pSelectedMixResult )
			return;

		const DWORD dwSoruceItemIdx = m_pSelectedMixResult->mBaseMaterial;
		CItem* sourceItem = GAMEIN->GetInventoryDialog()->GetItemForIdx(
			dwSoruceItemIdx,
			WORD(-1));

		if( ! sourceItem )
		{
			// 재료 부족 메세지 출력
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 784 ) );
			return ;
		}

		message.Category			= MP_ITEM;
		message.Protocol			= MP_ITEM_MIX_SYN;
		message.dwObjectID			= HEROID;
		message.mSourceItemPosition	= sourceItem->GetPosition();
		message.mSourceItemDbIndex	= sourceItem->GetDBIdx();
		message.mMixSize			= mMessage.mMixSize;
		message.mResultIndex		= mMessage.mResultIndex;
		message.mProtectionItem		= mMessage.mProtectionItem;
		// 080925 LUJ, 보조 아이템 정보 설정
		message.mSupportItem		= mSupportItem;
	}

	NETWORK->Send( &message, sizeof( message ) );	

	{
		SetDisable( FALSE );
	}	
}

void CMixDialog::Refresh()
{
	if( !IsActive() )
		return;

	// 히어로의 소지금/레벨 정보가 변경될때마다 갱신이 필요하다
	// 설명과 소지금 정보만 갱신, 재료 현황은 갱신하지 않도록 한다.
	PutDescription( m_pSelectedMixResult );

	UpdateCostInfo();
}

// 080227 LUJ, 조합 결과를 갱신한다
void CMixDialog::RefreshResult( const ITEMBASE& itemBase )
{
	if( mResultIcon )
	{
		mResultIcon->SetItemBaseInfo( itemBase );

		ITEMMGR->AddToolTip( mResultIcon );

		mResultIconDialog->AddIcon( 0, mResultIcon );
	}
}

// 080228 LUJ, 보호 아이템 설정
void CMixDialog::SetProtectedActive( CItem& item )
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

	CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1456 ) );
}

// 080925 LUJ, 조합 보조 아이템 설정
void CMixDialog::SetSupportItem( const ITEMBASE& itemBase )
{
	mSupportItem.wIconIdx	= itemBase.wIconIdx;
	mSupportItem.dwDBIdx	= itemBase.dwDBIdx;
	mSupportItem.Position	= itemBase.Position;
}

// 090812 ShinJS --- 조합 인터페이스 수정, 조합 아이템 Category 리스트 Load, Init, Update
void CMixDialog::LoadMixDlgCategory()
{
	CMHFile file;
	if( !file.Init( "./Data/Interface/Windows/MixDialogCategory.bin", "rb" ) )
		return;

	eMixDlgCategoryType eCurCategoryType = eMixDlgCategoryType_Max;

	while( !file.IsEOF() )
	{
		char buf[ MAX_PATH ] = {0,};

		file.GetString( buf );

		if( buf[0] == '/' && buf[1] == '/' )
		{
			file.GetLineX( buf, MAX_PATH );
			continue;
		}
		else if( buf[0] == '}' )
		{
			eCurCategoryType = eMixDlgCategoryType_Max;
		}
		else if( strcmp( buf, "$Category1" ) == 0 )
		{
			eCurCategoryType = eMixDlgCategoryType_1;
		}
		else if( strcmp( buf, "$Category2" ) == 0 )
		{
			eCurCategoryType = eMixDlgCategoryType_2;
		}
		else if( strcmp( buf, "#AddList" ) == 0 )
		{
			stCategoryInfo* pInfo = new stCategoryInfo;

			switch( eCurCategoryType )
			{
			case eMixDlgCategoryType_1:
				{
					pInfo->categoryType				= eMixDlgCategoryType_1;
					pInfo->wID						= file.GetWord();						// ID
					pInfo->wHighCategoryID			= WORD( -1 );
					file.GetString( pInfo->szName );										// Name	
					
					// Item Type
					char txt[MAX_PATH]={0,};
					file.GetLine( txt, MAX_PATH );
					char* delimit = " ,\t";
					char* token = strtok( txt, delimit );
                    while( token != NULL )
					{
						pInfo->setItemType.insert( DWORD(atoi( token )) );
						token = strtok( 0, delimit );
					}
				}
				break;
			case eMixDlgCategoryType_2:
				{
					pInfo->categoryType				= eMixDlgCategoryType_2;
					pInfo->wID						= WORD( m_htCategoryInfo.GetDataNum() );// ID
					pInfo->wHighCategoryID			= file.GetWord();						// 상위Type
					file.GetString( pInfo->szName );										// Name
					
					// Item TypeDetail
					char txt[MAX_PATH]={0,};
					file.GetLine( txt, MAX_PATH );
					char* delimit = " ,\t";
					char* token = strtok( txt, delimit );
                    while( token != NULL )
					{
						pInfo->setItemTypeDetail.insert( DWORD(atoi( token )) );
						token = strtok( 0, delimit );
					}
				}
				break;
			}

			m_htCategoryInfo.Add( pInfo, pInfo->wHighCategoryID );
		}
	}
}

ITEM* CMixDialog::GetSelectedItemFromListDlg( eMixDlgCategoryType eCategory, BOOL bCancelOpt )
{
	if( !m_CategoryDlg[ eCategory ] )
		return NULL;

	int nSelectedRow = m_CategoryDlg[ eCategory ]->GetClickedRowIdx();
	if( nSelectedRow < 0 )
		return NULL;

	PTRLISTPOS pos = m_CategoryDlg[ eCategory ]->GetListItem()->FindIndex( nSelectedRow );
	if( !pos )
		return NULL;

	ITEM* pItem = (ITEM*)m_CategoryDlg[ eCategory ]->GetListItem()->GetAt(pos);
	if( !pItem )
		return NULL;

	if( nSelectedRow != m_SelectedCategory[ eCategory ] )
	{
		m_bChangedCategory[ eCategory ] = TRUE;

		// 선택 위치 저장
		m_SelectedCategory[ eCategory ] = nSelectedRow;
	}
	return pItem;
}

ITEM* CMixDialog::GetSelectedItemFromListDlg( LONG lId )
{
	eMixDlgCategoryType categoryType;
	switch( lId )
	{
	case ITMD_MIXCATEGORY1:
		categoryType = eMixDlgCategoryType_1;
		break;
	case ITMD_MIXCATEGORY2:
		categoryType = eMixDlgCategoryType_2;
		break;
	case ITMD_MIXCATEGORY3:
		categoryType = eMixDlgCategoryType_3;
		break;
	case ITMD_MIXCATEGORY4:
		categoryType = eMixDlgCategoryType_4;
		break;
	default:
		return NULL;
	}

	return GetSelectedItemFromListDlg( categoryType, TRUE );
}

stCategoryInfo* CMixDialog::GetSelectedCategoryInfo( eMixDlgCategoryType eCategory )
{
	if( !m_CategoryDlg[ eCategory ] ||
		m_SelectedCategory[ eCategory ] < 0 ||
		eCategory > eMixDlgCategoryType_2 )
		return NULL;

	PTRLISTPOS pos = m_CategoryDlg[ eCategory ]->GetListItem()->FindIndex( m_SelectedCategory[ eCategory ] );
	if( !pos )
		return NULL;

	ITEM* pItem = (ITEM*)m_CategoryDlg[ eCategory ]->GetListItem()->GetAt(pos);
	if( !pItem )
		return NULL;

	DWORD dwMultiDataKey = (eCategory > eMixDlgCategoryType_1 ? m_SelectedCategory[ eCategory - 1 ] : (WORD)-1);
	
	m_htCategoryInfo.StartGetMultiData( dwMultiDataKey );

	for(stCategoryInfo* pInfo = m_htCategoryInfo.GetMultiData();
		0 < pInfo;
		pInfo = m_htCategoryInfo.GetMultiData())
	{
		if( strcmp( pItem->string, pInfo->szName ) == 0 )
			return pInfo;
	}

	return NULL;
}

void CMixDialog::InitCategory()
{
	for( int i=0 ; i<eMixDlgCategoryType_Max ; ++i )
		if( !m_CategoryDlg[ i ] )
			return;

	// Category1 List Setting
	m_CategoryDlg[ eMixDlgCategoryType_1 ]->RemoveAll();

	m_htCategoryInfo.SetPositionHead();

	for(stCategoryInfo* pInfo = m_htCategoryInfo.GetData();
		0 < pInfo;
		pInfo = m_htCategoryInfo.GetData())
	{
		if( pInfo->categoryType == eMixDlgCategoryType_1 )
		{
			m_CategoryDlg[ eMixDlgCategoryType_1 ]->AddItem( pInfo->szName, RGB_HALF( 255, 255, 255 ) );
		}
	}

	// 초기 설정
	m_SelectedCategory[ eMixDlgCategoryType_1 ] = 0;
	m_SelectedCategory[ eMixDlgCategoryType_2 ] = 0;
	m_SelectedCategory[ eMixDlgCategoryType_3 ] = -1;
	m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;

	m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetCurSelectedRowIdx( 0 );

	m_bChangedCategory[ eMixDlgCategoryType_1 ] = TRUE;

	m_dwDragItemIdx = 0;
	m_eMixDialogState = eMixDialogState_Normal;

	UpdateCategory();
}

void CMixDialog::UpdateCategory()
{
	// 결과 정보를 모두 제거한다.
	RemoveResult();

	// Category1 의 선택이 변경된 경우 Category2 List 변경
	if( m_bChangedCategory[ eMixDlgCategoryType_1 ] )
	{
		m_bChangedCategory[ eMixDlgCategoryType_1 ] = FALSE;
		
		// 카테고리 변경시 선택된 스크립트 초기화
		ClearSelectedCategoryInfo();

		// 기본 상태로 설정
		m_eMixDialogState = eMixDialogState_Normal;

		UpdateCategory( eMixDlgCategoryType_1 );

		// 하위 카테고리 변경
		m_bChangedCategory[ eMixDlgCategoryType_2 ] = TRUE;
	}

	// Category2 의 선택이 변경된 경우 Category3 List 변경
	if( m_bChangedCategory[ eMixDlgCategoryType_2 ] )
	{
		m_bChangedCategory[ eMixDlgCategoryType_2 ] = FALSE;

		// 카테고리 변경시 선택된 스크립트 초기화
		ClearSelectedCategoryInfo();

		// 기본 상태로 설정
		m_eMixDialogState = eMixDialogState_Normal;

		UpdateCategory( eMixDlgCategoryType_2 );

		// 하위 카테고리 변경
		m_bChangedCategory[ eMixDlgCategoryType_3 ] = TRUE;
	}

	// Category3 의 선택이 변경된 경우 조합식 정보 설정
	if( m_bChangedCategory[ eMixDlgCategoryType_3 ] )
	{
		m_bChangedCategory[ eMixDlgCategoryType_3 ] = FALSE;

		// 카테고리 변경시 선택된 스크립트 초기화
		ClearSelectedCategoryInfo();

		UpdateCategory( eMixDlgCategoryType_3 );

		// 하위 카테고리 변경
		m_bChangedCategory[ eMixDlgCategoryType_4 ] = TRUE;
	}

	// Category4 의 선택이 변경된 경우 조합 결과 정보 설정
	if( m_bChangedCategory[ eMixDlgCategoryType_4 ] )
	{
		m_bChangedCategory[ eMixDlgCategoryType_4 ] = FALSE;

		UpdateCategory( eMixDlgCategoryType_4 );
	}
}

void CMixDialog::UpdateCategoryByItemIdx( DWORD dwItemIdx )
{
	const MixScript* pMixScript = ITEMMGR->GetMixScript( dwItemIdx );
	if( !pMixScript )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 782 ) );
		return;
	}

	if( pMixScript->empty() )
		return;

	// 카테고리 초기화
	m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetCurSelectedRowIdx( -1 );
	m_CategoryDlg[ eMixDlgCategoryType_2 ]->RemoveAll();
	m_CategoryDlg[ eMixDlgCategoryType_3 ]->RemoveAll();
	m_CategoryDlg[ eMixDlgCategoryType_4 ]->RemoveAll();

	m_SelectedCategory[ eMixDlgCategoryType_1 ] = -1;
	m_SelectedCategory[ eMixDlgCategoryType_2 ] = -1;
	m_SelectedCategory[ eMixDlgCategoryType_3 ] = -1;
	m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;

	ClearSelectedCategoryInfo();

	// 결과 아이템을 설정한다.
	std::set<DWORD> addedResultItemIdx;
	for( MixScript::const_iterator it = pMixScript->begin() ; pMixScript->end() != it ; ++it )
	{
		const ItemMixResult&	result		= *it;
		const DWORD				itemIndex	= result.mItemIndex;

		// 이미 추가된 Item인 경우 추가하지 않는다.
		if( addedResultItemIdx.find( itemIndex ) != addedResultItemIdx.end() )
			continue;

		ITEM_INFO* pIteminfo = ITEMMGR->GetItemInfo( itemIndex );
		if( !pIteminfo )
			continue;

		switch( m_eMixItemSortType )
		{
		case eMixItemSortType_ByName:
			{
				m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx );
			}
			break;
		case eMixItemSortType_ByLevel:
			{
				if( pIteminfo->LimitLevel )
				{
					char txt[ MAX_LISTITEM_SIZE ] = {0,};
					sprintf( txt, RESRCMGR->GetMsg( 1156 ), pIteminfo->ItemName, pIteminfo->LimitLevel );
					m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( txt, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
				}
				else
					m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
			}
			break;
		}

		addedResultItemIdx.insert( itemIndex );
	}

	ITEM_INFO* pSourceItemInfo	=	ITEMMGR->GetItemInfo( dwItemIdx );

	m_pSearchNameEdit->SetEditText( pSourceItemInfo->ItemName );
		
	m_CategoryDlg[ eMixDlgCategoryType_3 ]->SetTopListItemIdx( 0 );
}

void CMixDialog::UpdateCategory( eMixDlgCategoryType eCategory )
{
	switch( eCategory )
	{
	case eMixDlgCategoryType_1:
		{
			// eMixDlgCategoryType_2 List 변경
			// 하위 카테고리 List 제거
			m_CategoryDlg[ eMixDlgCategoryType_2 ]->RemoveAll();
			m_SelectedCategory[ eMixDlgCategoryType_2 ] = -1;

			// Category2 List Setting
			stCategoryInfo* pInfo = NULL;
			m_htCategoryInfo.StartGetMultiData( m_SelectedCategory[ eMixDlgCategoryType_1 ] );
			while( (pInfo = m_htCategoryInfo.GetMultiData()) != NULL )
			{
				m_CategoryDlg[ eMixDlgCategoryType_2 ]->AddItem( pInfo->szName, RGB_HALF( 255, 255, 255 ) );
			}

			m_CategoryDlg[ eMixDlgCategoryType_2 ]->SetTopListItemIdx( 0 );
		}
		break;
	case eMixDlgCategoryType_2:
		{
			// eMixDlgCategoryType_3 List 변경

			// 조합 Dialog 상태에 따라 List 설정방법을 달리 한다
			switch( m_eMixDialogState )
			{
			case eMixDialogState_Normal:
				{
					// 하위 카테고리 List 제거
					m_CategoryDlg[ eMixDlgCategoryType_3 ]->RemoveAll();
					m_SelectedCategory[ eMixDlgCategoryType_3 ] = -1;

					stCategoryInfo* pCategoryInfo = GetSelectedCategoryInfo( eMixDlgCategoryType_2 );
					if( pCategoryInfo )
					{
						CYHHashTable<stMixResultItemInfo>& htMixInfoForScript = ITEMMGR->GetMixInfoForScriptTable();

						htMixInfoForScript.SetPositionHead();

						for(stMixResultItemInfo* pMixInfo = htMixInfoForScript.GetData();
							0 < pMixInfo;
							pMixInfo = htMixInfoForScript.GetData())
						{
							// 091012 ShinJS --- 중복된 결과물인 경우 출력하지 않는다.
							if( pMixInfo->bOverlappedResult )
								continue;

							ITEM_INFO* pIteminfo = ITEMMGR->GetItemInfo( pMixInfo->dwResultItemIdx );
							if( !pIteminfo )
								continue;

							// 결과 아이템의 Type이 Category와 맞지 않는 경우
							if( pCategoryInfo->setItemTypeDetail.find( pIteminfo->dwTypeDetail ) == pCategoryInfo->setItemTypeDetail.end() )
								continue;

							// 정렬 방법에 따라 추가시 정보를 다르게 넣는다.
							switch( m_eMixItemSortType )
							{
							case eMixItemSortType_ByName:
								{
									m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx );
								}
								break;
							case eMixItemSortType_ByLevel:
								{
									if( pIteminfo->LimitLevel )
									{
										char txt[ MAX_LISTITEM_SIZE ] = {0,};
										sprintf( txt, RESRCMGR->GetMsg( 1156 ), pIteminfo->ItemName, pIteminfo->LimitLevel );
										m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( txt, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
									}
									else
										m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
								}
								break;
							}
						}
					}

					m_CategoryDlg[ eMixDlgCategoryType_3 ]->SetTopListItemIdx( 0 );
				}
				break;
			case eMixDialogState_Drag:
				{
					UpdateCategoryByItemIdx( m_dwDragItemIdx );
				}
				break;
			case eMixDialogState_Search:
				{
					SearchMixResultItem();
				}
				break;
			}
		}
		break;
	case eMixDlgCategoryType_3:
		{
			// 하위 카테고리 List 제거
			m_CategoryDlg[ eMixDlgCategoryType_4 ]->RemoveAll();
			m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;

			// 조합식 정보 Update
			UpdateMixInfo();
		}
		break;
	case eMixDlgCategoryType_4:
		{
			// 조합식 선택에 대한 조합 정보 설정
			UpdateMixScript();
		}
		break;
	}
}

void CMixDialog::UpdateHighCategory( DWORD dwResultItemIdx )
{
	// 결과아이템에 해당하는 CategorInfo를 구한다.
	const ITEM_INFO* pResultItemInfo = ITEMMGR->GetItemInfo( dwResultItemIdx );
	stCategoryInfo* pCategoryInfo = NULL;
	stCategoryInfo* pResultCategory1Info = NULL;
	stCategoryInfo* pResultCategory2Info = NULL;
	
	// eMixDlgCategoryType_2 의 정보를 구한다.
	m_htCategoryInfo.SetPositionHead();
	while( (pCategoryInfo = m_htCategoryInfo.GetData()) != NULL )
	{
		if( pCategoryInfo->setItemTypeDetail.find( pResultItemInfo->dwTypeDetail ) != pCategoryInfo->setItemTypeDetail.end() )
		{
			pResultCategory2Info = pCategoryInfo;
			break;
		}
	}

	if( !pResultCategory2Info )
		return;

	// eMixDlgCategoryType_1 의 정보를 구한다.
	m_htCategoryInfo.StartGetMultiData( (WORD)-1 );
	while( (pCategoryInfo = m_htCategoryInfo.GetMultiData()) != NULL )
	{
		if( pCategoryInfo->wID == pResultCategory2Info->wHighCategoryID )
		{
			pResultCategory1Info = pCategoryInfo;
			break;
		}
	}

	if( !pResultCategory1Info )
		return;

	// eMixDlgCategoryType_1 선택
	int nCategoryIndex = GetCategoryIndexFromName( eMixDlgCategoryType_1, pResultCategory1Info->szName );
	m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetCurSelectedRowIdx( nCategoryIndex );
	m_SelectedCategory[ eMixDlgCategoryType_1 ] = nCategoryIndex;

	// eMixDlgCategoryType_2의 List 설정
	UpdateCategory( eMixDlgCategoryType_1 );


	// eMixDlgCategoryType_2 선택
	nCategoryIndex = GetCategoryIndexFromName( eMixDlgCategoryType_2, pResultCategory2Info->szName );
	m_CategoryDlg[ eMixDlgCategoryType_2 ]->SetCurSelectedRowIdx( nCategoryIndex );
	m_SelectedCategory[ eMixDlgCategoryType_2 ] = nCategoryIndex;
}

void CMixDialog::UpdateMixInfo()
{
	ITEM* pCategoryItem = GetSelectedItemFromListDlg( eMixDlgCategoryType_3 );
	if( !pCategoryItem )
		return;

	// 결과 Item으로 MixScript 모두 구하기
	const DWORD dwItemIdx = pCategoryItem->dwData;
	m_htEqualSelectedMixResult.RemoveAll();
	ITEMMGR->GetMixResultFromResultItemIdx( dwItemIdx, &m_htEqualSelectedMixResult );
	if( m_htEqualSelectedMixResult.GetDataNum() <= 0 )
		return;

	// 결과 아이템이 선택된 경우 하위 조합식 Category 초기화/Update
	if( m_SelectedCategory[ eMixDlgCategoryType_3 ] >= 0 )
	{
		// 하위 카테고리 List 설정
		DWORD dwMixResultCnt = 1;

		m_htEqualSelectedMixResult.SetPositionHead();

		for(ItemMixResult* pMixResult = m_htEqualSelectedMixResult.GetData();
			0 < pMixResult;
			pMixResult = m_htEqualSelectedMixResult.GetData())
		{
			char szItemName[ 256 ] = {0,};
			sprintf( szItemName, RESRCMGR->GetMsg( 1153 ), dwMixResultCnt++ );
			m_CategoryDlg[ eMixDlgCategoryType_4 ]->AddItem( szItemName, RGB_HALF( 255, 255, 255 ) );
		}

		m_CategoryDlg[ eMixDlgCategoryType_4 ]->SetCurSelectedRowIdx( 0 );
		m_SelectedCategory[ eMixDlgCategoryType_4 ] = 0;
	}
	else
	{
		m_CategoryDlg[ eMixDlgCategoryType_4 ]->SetCurSelectedRowIdx( -1 );
		m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;
	}

	m_CategoryDlg[ eMixDlgCategoryType_4 ]->SetTopListItemIdx( 0 );

	// 상위 카테고리 변경이 필요한 경우(검색, Drag 상태인 경우)
	if( m_eMixDialogState != eMixDialogState_Normal )
	{
		UpdateHighCategory( dwItemIdx );
	}
}

void CMixDialog::UpdateMixScript()
{
	// 선택된 것이 없거나 조합식이 없는 경우
	if( m_SelectedCategory[ eMixDlgCategoryType_4 ] < 0 ||
		m_htEqualSelectedMixResult.GetDataNum() <= 0 )
		return;

	// 선택된 조합 아이템의 스크립트 저장
	m_pSelectedMixResult = m_htEqualSelectedMixResult.GetData( m_SelectedCategory[ eMixDlgCategoryType_4 ] );
	// 조합식 정보가 잘못된 경우
	if( !m_pSelectedMixResult )
		return;

	// 결과 아이템 Icon Update
	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( m_pSelectedMixResult->mItemIndex );
	if( !pItemInfo )
		return;	
	// 결과 아이템 위치 저장
	LONG	absX	=	(LONG)mResultIcon->GetAbsX();
	LONG	absY	=	(LONG)mResultIcon->GetAbsY();

	cImage image;
	mResultIcon->Init( absX, absY, DEFAULT_ICONSIZE, DEFAULT_ICONSIZE, ITEMMGR->GetIconImage( pItemInfo->ItemIdx, &image ), 0 );
	ITEMBASE itembase;
	memset( &itembase, 0, sizeof(ITEMBASE) );
	itembase.wIconIdx = pItemInfo->ItemIdx;

	mResultIcon->SetItemBaseInfo( itembase );
	mResultIcon->SetIconType( eIconType_AllItem );
	mResultIcon->SetData( pItemInfo->ItemIdx );
	mResultIcon->SetVisibleDurability( FALSE );

	ITEMMGR->AddToolTip( mResultIcon );
	mResultIconDialog->AddIcon( 0, mResultIcon );


	// 조합 설명 Update
	PutDescription( m_pSelectedMixResult );

	// 조합 아이템이 겹치지 않는경우 spin btn/edit가 적용되지 않도록 한다
	if( ITEMMGR->IsDupItem( m_pSelectedMixResult->mItemIndex ) )
	{
		m_pCntSpin->SetUnit( 1 );
		m_pCntSpin->SetReadOnly( FALSE );
	}
	else
	{
		m_pCntSpin->SetUnit( 0 );
		m_pCntSpin->SetReadOnly( TRUE );
	}
	m_pCntSpin->SetValue( 1 );
	m_MixItemCnt = 1;

	// 소지 재료 Update
	UpdateOwnStuff();

	m_pCntSpin->SetValue( m_dwMixResultCnt );

	// 소지금과 조합 비용 Update
	UpdateCostInfo();
}

void CMixDialog::UpdateOwnStuff()
{
	if( !m_pSelectedMixResult )
		return;

	CInventoryExDialog* pInvenDlg = (CInventoryExDialog*)WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
	ASSERT( pInvenDlg );

	DWORD index	= 0;

	int		nMinResultCnt	=	0;
	int		nCurrentMinCnt	=	0;

	//초기값 설정
	ItemMixResult::Material::const_iterator firstIter = m_pSelectedMixResult->mMaterial.begin();
	const DWORD dwStuffItemIdx = firstIter->first;
	const DURTYPE stuffItemNeedCntFirst = firstIter->second * m_MixItemCnt;
	const DURTYPE curItemCntFirst = pInvenDlg->GetTotalItemDurability( dwStuffItemIdx );

	if( stuffItemNeedCntFirst )
		nMinResultCnt	=	curItemCntFirst	/	stuffItemNeedCntFirst;


	for( ItemMixResult::Material::const_iterator iter = m_pSelectedMixResult->mMaterial.begin() ; iter != m_pSelectedMixResult->mMaterial.end() ; ++iter, ++index )
	{
		if( mStuffIcon.size() <= index )
		{
			ASSERT( 0 && "재료 개수가 그리드 내 아이콘보다 많음" );
			break;
		}

		CItemShow*				itemShow	= mStuffIcon[ index ];
		const DWORD dwStuffItemIdx = iter->first;
		const DURTYPE stuffItemNeedCnt = iter->second * m_MixItemCnt;
		const DURTYPE curItemCnt = pInvenDlg->GetTotalItemDurability( dwStuffItemIdx );
		//인벤토리에 보유한 아이템 개수
		const DURTYPE ownItemCount	=	GAMEIN->GetInventoryDialog()->GetTotalItemDurability( dwStuffItemIdx );

		itemShow->SetData( dwStuffItemIdx );
		itemShow->Init( dwStuffItemIdx, curItemCnt );
		itemShow->SetVisibleDurability( TRUE );

		// 재료 개수에 대한 Lock 처리
		if( curItemCnt >= stuffItemNeedCnt && curItemCnt != 0 ) //|| stuffItemNeedCnt <= 0 )
			itemShow->SetLock( false );
		else 
			itemShow->SetLock( true );
		

		ITEMMGR->AddToolTip( itemShow );

		mIconGridDialog->AddIcon( WORD( index ), itemShow );
		if( ownItemCount != 0 && stuffItemNeedCnt != 0 )
			nCurrentMinCnt	=	ownItemCount / stuffItemNeedCnt;
		else
			nCurrentMinCnt	=	0;

		
		if( nMinResultCnt > nCurrentMinCnt )
		{
			nMinResultCnt = nCurrentMinCnt;
		}

		// 재료 필요 개수 ToolTip 추가	//	필요개수 / 보유개수로 변경	오버할경우 노란색

		char szToolTip[ MAX_PATH ]	=	{ 0, };
		DWORD	dwFontColor;
		
		sprintf( szToolTip, RESRCMGR->GetMsg( 1152 ), ownItemCount, iter->second * m_MixItemCnt );
			if(	stuffItemNeedCnt <= ownItemCount && ownItemCount != 0 )	//아이템 보유수가 필요수 넘을경우
			dwFontColor	=	RGBA_MAKE( 255, 255, 0, 255 );
		else
			dwFontColor =	RGBA_MAKE( 255, 0, 0, 255 );
		itemShow->AddToolTipLine( szToolTip, dwFontColor );
	}
	// 겹쳐지지 않는 아이템의 경우 조합가능한 최대수 1로 조정
	if( !(ITEMMGR->IsDupItem( m_pSelectedMixResult->mItemIndex )) && nMinResultCnt >= 1 )
		m_dwMixResultCnt	=	1;
	else
		m_dwMixResultCnt	=	nMinResultCnt;
}

void CMixDialog::UpdateCostInfo()
{
	if( !m_pInvenMoney || !m_pMixCost )
		return;

	DURTYPE mixItemCnt = (DURTYPE)m_pCntSpin->GetValue();

	// 소지 금액 Update
	m_pInvenMoney->SetStaticText( AddComma( HERO->GetMoney() ) );

	if( !m_pSelectedMixResult )
		return;

	DWORD	dwMixCost	=	m_pSelectedMixResult->mMoney * mixItemCnt;


	// 조합 비용 Update
	m_pMixCost->SetStaticText( AddComma( dwMixCost ) );

	if( dwMixCost < HERO->GetMoney() )
		m_bIsEnoughMoney	=	TRUE;
	else
		m_bIsEnoughMoney	=	FALSE;

}

void CMixDialog::SetSortType( eMixItemSortType eSortType )
{
	switch( eSortType )
	{
	case eMixItemSortType_ByName:
		{
			m_CategoryDlg[ eMixDlgCategoryType_3 ]->SetSortType( eListItemSortType_StringGreater );
		}
		break;
	case eMixItemSortType_ByLevel:
		{
			m_CategoryDlg[ eMixDlgCategoryType_3 ]->SetSortType( eListItemSortType_SortDataGreater );
		}
		break;
	}

	// 정렬 방식이 변경된 경우 List 다시 생성
	if( m_eMixItemSortType != eSortType )
	{
		// 정렬 Type 저장
		m_eMixItemSortType = eSortType;

		// 정렬 설정 후 Update
		UpdateCategory( eMixDlgCategoryType_2 );
	}
}

void CMixDialog::ClearSelectedCategoryInfo()
{
	m_pSelectedMixResult = NULL;

	// 선택된 조합 아이템의 스크립트 Table (기본 재료아이템과 결과 아이템이 같고 조건이 다른 스크립트들 저장) 제거	
	m_htEqualSelectedMixResult.RemoveAll();
}

int CMixDialog::GetCategoryIndexFromName( eMixDlgCategoryType eCategory, const char* szItemName )
{
	int nCategoryCnt = m_CategoryDlg[ eCategory ]->GetListItem()->GetCount();
	for( int index = 0 ; index < nCategoryCnt ; ++index )
	{
		PTRLISTPOS pos = m_CategoryDlg[ eCategory ]->GetListItem()->FindIndex( index );
		if( !pos )
			continue;

		ITEM* pItem = (ITEM*)m_CategoryDlg[ eCategory ]->GetListItem()->GetAt( pos );
		if( !pItem )
			continue;

		if( strcmp( pItem->string, szItemName ) == 0 )
			return index;
	}

	return -1;
}

int CMixDialog::GetCategoryIndexFromItemIdx( eMixDlgCategoryType eCategory, DWORD dwItemIdx )
{
	int nCategoryCnt = m_CategoryDlg[ eCategory ]->GetListItem()->GetCount();
	for( int index = 0 ; index < nCategoryCnt ; ++index )
	{
		PTRLISTPOS pos = m_CategoryDlg[ eCategory ]->GetListItem()->FindIndex( index );
		if( !pos )
			continue;

		ITEM* pItem = (ITEM*)m_CategoryDlg[ eCategory ]->GetListItem()->GetAt( pos );
		if( !pItem )
			continue;

		if( pItem->dwData == dwItemIdx )
			return index;
	}

	return -1;
}

void CMixDialog::SearchMixResultItem()
{
	char	tempSearchName[MAX_PATH]	=	{ 0, };
	char	tempItemName[MAX_PATH]		=	{ 0, };
	// 카테고리 초기화
	m_CategoryDlg[ eMixDlgCategoryType_3 ]->RemoveAll();
	m_CategoryDlg[ eMixDlgCategoryType_4 ]->RemoveAll();

	m_SelectedCategory[ eMixDlgCategoryType_3 ] = -1;
	m_SelectedCategory[ eMixDlgCategoryType_4 ] = -1;

	ClearSelectedCategoryInfo();

	// 검색 방법에 대한 설정
	stCategoryInfo* pCategoryInfo = NULL;
	stCategoryInfo* pInfo = GetSelectedCategoryInfo( eMixDlgCategoryType_1 );
	if( pInfo )
		pCategoryInfo = pInfo;

	pInfo = GetSelectedCategoryInfo( eMixDlgCategoryType_2 );
	if( pInfo && pCategoryInfo->wID != 0 )	// 1차 카테고리가 전체 일경우 2차카테고리는 설정하지 않음
		pCategoryInfo = pInfo;


	// 검색 레벨 설정
	WORD wMinLv = WORD( atoi(m_pSearchMinLv->GetEditText()) );
	WORD wMaxLv = WORD( atoi(m_pSearchMaxLv->GetEditText()) );

	// Lv Text 설정
	if( wMinLv < 1 )
		m_pSearchMinLv->SetEditText( "" );
	if( wMaxLv < 1 )
		m_pSearchMaxLv->SetEditText( "" );

	wMaxLv = (wMaxLv > 0 ? wMaxLv : WORD(-1));

	if( wMinLv > wMaxLv )
	{
		// 검색 레벨을 정확하게 입력해주세요
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1989 ) );
		return;
	}

	// 검색어 설정
	const char* szSearchName = m_pSearchNameEdit->GetEditText();

	CYHHashTable<stMixResultItemInfo>& htMixInfoForScript = ITEMMGR->GetMixInfoForScriptTable();

	htMixInfoForScript.SetPositionHead();

	for(stMixResultItemInfo* pMixInfo = htMixInfoForScript.GetData();
		0 < pMixInfo;
		pMixInfo = htMixInfoForScript.GetData())
	{
		// 091012 ShinJS --- 중복된 결과물인 경우 출력하지 않는다.
		if( pMixInfo->bOverlappedResult )
			continue;

		ITEM_INFO* pIteminfo = ITEMMGR->GetItemInfo( pMixInfo->dwResultItemIdx );
		if( !pIteminfo )
			continue;

		// 레벨 설정 확인
		if( wMinLv > pIteminfo->LimitLevel || pIteminfo->LimitLevel > wMaxLv )
			continue;

		// 카테고리 정보 확인
		if( pCategoryInfo )
		{
			switch( pCategoryInfo->categoryType )
			{
			case eMixDlgCategoryType_1:
				{
					if( pCategoryInfo->setItemType.find( pIteminfo->dwType ) == pCategoryInfo->setItemType.end() )
						continue;
				}
				break;
			case eMixDlgCategoryType_2:
				{
					if( pCategoryInfo->setItemTypeDetail.find( pIteminfo->dwTypeDetail ) == pCategoryInfo->setItemTypeDetail.end() )
						continue;
				}
				break;
			}
		}

		strcpy( tempSearchName, szSearchName );
		strcpy( tempItemName, pIteminfo->ItemName );

		//대문자로 변환
		strupr( tempSearchName );
		strupr( tempItemName );


		// 검색어 확인
		if( strstr( tempItemName, tempSearchName ) == NULL )
			continue;

		// 검색 아이템 추가, 정렬 방법에 따라 추가시 정보를 다르게 넣는다.
		switch( m_eMixItemSortType )
		{
		case eMixItemSortType_ByName:
			{
				m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx );
			}
			break;
		case eMixItemSortType_ByLevel:
			{
				if( pIteminfo->LimitLevel )
				{
					char txt[ MAX_LISTITEM_SIZE ] = {0,};
					sprintf( txt, RESRCMGR->GetMsg( 1156 ), pIteminfo->ItemName, pIteminfo->LimitLevel );
					m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( txt, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
				}
				else
					m_CategoryDlg[ eMixDlgCategoryType_3 ]->AddItem( pIteminfo->ItemName, RGB_HALF( 255, 255, 255 ), -1, pIteminfo->ItemIdx, pIteminfo->LimitLevel );
			}
			break;
		}
	}
	
	m_CategoryDlg[ eMixDlgCategoryType_3 ]->SetTopListItemIdx( 0 );

	char txt[ MAX_LISTITEM_SIZE ] = {0,};
	sprintf( txt, CHATMGR->GetChatMsg( 1988 ), m_CategoryDlg[ eMixDlgCategoryType_3 ]->GetItemCount() ); // 검색결과 %d 개
	m_CategoryDlg[ eMixDlgCategoryType_4 ]->AddItem( txt, RGB_HALF( 255, 255, 255 ), -1 );
}

void CMixDialog::PushCatagory1BTN()
{
	m_CategoryDlg[ eMixDlgCategoryType_2 ]->SetActive( FALSE );
	m_pSortTypeCombo->SetActive( FALSE );
	m_pSearchConditionInitBTN->SetActive( FALSE );
}

void CMixDialog::PushCatagory2BTN()
{
	m_pSortTypeCombo->SetActive( FALSE );
	m_pSearchConditionInitBTN->SetActive( FALSE );
}

void CMixDialog::CatagoryScrollEnd()
{
	if( m_pSortTypeCombo &&m_pSearchConditionInitBTN )
	{
		m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetActive( TRUE );
		m_CategoryDlg[ eMixDlgCategoryType_2 ]->SetActive( TRUE );

		m_CategoryDlg[ eMixDlgCategoryType_1 ]->SetShowScroll( FALSE );
		m_CategoryDlg[ eMixDlgCategoryType_2 ]->SetShowScroll( FALSE );

		m_pSortTypeCombo->SetActive( TRUE );
		m_pSearchConditionInitBTN->SetActive( TRUE );
	}
}
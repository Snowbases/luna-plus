#include "stdafx.h"
#include "ComposeDialog.h"
#include "ItemManager.h"
#include "WindowIDEnum.h"
#include "cWindowManager.h"
#include "interface/cIconGridDialog.h"
#include "interface/cIconDialog.h"
#include "interface/cListDialog.h"
#include "interface/cButton.h"
#include "interface/cStatic.h"
#include "cMsgBox.h"
#include "cScriptManager.h"
#include "ChatManager.h"
#include "ProgressDialog.h"
#include "../[CC]Header/GameResourceManager.h"

const WORD firstSourcePosition	= 0;
const WORD secondSourcePosition	= 1;
const WORD resultPosition		= 2;

CComposeDialog::CComposeDialog() :
mOptionListDialog( 0 ),
mSourceIconDialog( 0 ),
mResultIconGridDialog( 0 )
{
	ZeroMemory( &mKeyIconBase,		sizeof( mKeyIconBase ) );
	ZeroMemory( &mComposeMessage,	sizeof( mComposeMessage ) );
}

CComposeDialog::~CComposeDialog()
{
	Release();
}

void CComposeDialog::Linking()
{
	Release();

	mPageStatic				= ( cStatic*			)GetWindowForID( COMPOSE_PAGE_STATIC );
	mPagePreviousButton		= ( cButton*			)GetWindowForID( COMPOSE_PAGE_PREV_BUTTON );
	mPageNextButton			= ( cButton*			)GetWindowForID( COMPOSE_PAGE_NEXT_BUTTON );
	mSubmitButton			= ( cButton*			)GetWindowForID( COMPOSE_SUBMIT_BUTTON );
	mOptionListDialog		= ( cListDialog*		)GetWindowForID( COMPOSE_OPTION_LISTDLG );
	mSourceIconDialog		= ( cIconDialog*		)GetWindowForID( COMPOSE_SOURCE_ICONDLG );
	mResultIconGridDialog	= ( cIconGridDialog*	)GetWindowForID( COMPOSE_RESULT_ICONGRIDDLG );

	if( ! mPageStatic			||
		! mPagePreviousButton	||
		! mPageNextButton		||
		! mSubmitButton			||
		! mOptionListDialog		||
		! mSourceIconDialog		||
		! mResultIconGridDialog )
	{
		cprintf( "linking is failed in CComposeDialog\n" );
		return;
	}

	if( mOptionListDialog )
	{
		mOptionListDialog->SetShowSelect( TRUE );
	}

	// 080916 LUJ, 아이콘을 할당하고 창에 연결시킨다.
	{
		ITEMBASE emptyItemBase;
		ZeroMemory( &emptyItemBase, sizeof( emptyItemBase ) );

		cImage image;
		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );		

		for(WORD i = 0; i < mSourceIconDialog->GetCellNum(); ++i )
		{
			CItem* item = new CItem( &emptyItemBase );
			item->SetToolTip( "", RGB_HALF( 255, 255, 255 ), &image );
			item->SetMovable( FALSE );
			
			mSourceIconDialog->AddIcon( i, item );
			mItemList.push_back( item );
		}

		for(WORD i = 0; i < mResultIconGridDialog->GetCellNum(); ++i )
		{
			CItem* item = new CItem( &emptyItemBase );
			item->SetToolTip( "", RGB_HALF( 255, 255, 255 ), &image );
			item->SetMovable( FALSE );
			
			mResultIconGridDialog->AddIcon( i, item );
			mItemList.push_back( item );
		}
	}
}

void CComposeDialog::Release()
{
	{
		for(	ItemList::iterator it = mItemList.begin();
				mItemList.end() != it;
				++it )
		{
			SAFE_DELETE( *it );
		}

		mItemList.clear();
	}
}

BOOL CComposeDialog::FakeMoveIcon( LONG x, LONG y, cIcon* fakeMoveIcon )
{
	CItem* fakeMoveItem = ( CItem* )fakeMoveIcon;

	if( !	fakeMoveItem				||
			fakeMoveItem->IsLocked()	||
			WT_ITEM != fakeMoveItem->GetType() )
	{
		return FALSE;
	}
	// 080916 LUJ, 인벤토리이외에 아이템 FALSE
	else if( ! ITEMMGR->IsEqualTableIdxForPos( eItemTable_Inventory, fakeMoveItem->GetPosition() ) )
	{
		WINDOWMGR->MsgBox(
			MBI_NOTICE,
			MBT_OK,
			CHATMGR->GetChatMsg( 1678 ) );
		return FALSE;
	}
	else if( ITEMMGR->IsDupItem( fakeMoveItem->GetItemIdx() ) )
	{
		WINDOWMGR->MsgBox(
			MBI_NOTICE,
			MBT_OK,
			CHATMGR->GetChatMsg( 1679 ) );
		return FALSE;
	}

	const ComposeScript* script = GAMERESRCMNGR->GetComposeScript( fakeMoveItem->GetItemIdx() );

	if( ! script )
	{
		WINDOWMGR->MsgBox(
			MBI_NOTICE,
			MBT_OK,
			CHATMGR->GetChatMsg( 1680 ) );
		return FALSE;
	}
	else if( script->mKeyItemIndex != mKeyIconBase.wIconIdx )
	{
		WINDOWMGR->MsgBox(
			MBI_NOTICE,
			MBT_OK,
			CHATMGR->GetChatMsg( 1681 ) );
		return FALSE;
	}

	// 080916 LUJ, 재료 아이템이 하나 등록된 상태에서, 드래그한 재료를 다른 위치에 놓으려는 경우에 스크립트 일치 검사를 한다
	{
		CItem* firstSourceItem	= ( CItem* )mSourceIconDialog->GetIconForIdx( firstSourcePosition );
		CItem* secondSourceItem	= ( CItem* )mSourceIconDialog->GetIconForIdx( secondSourcePosition );

		if( !	firstSourceItem							||
				firstSourceItem->GetType()	!= WT_ITEM	||
			!	secondSourceItem						||
				secondSourceItem->GetType()	!= WT_ITEM )
		{
			return FALSE;
		}
		
		WORD position = WORD(-1);
		mSourceIconDialog->GetPositionForXYRef( x, y, position );

		// 080916 LUJ, 재료로 등록하는 아이템 두 개가 같은 스크립트를 사용해야한다. 다음 경우에만 체크하면 된다
		//			1. 한 슬롯에 이미 재료가 있고, 빈 슬롯에 넣으려고 시도할 경우
		//			2. 두 슬롯 모두 재료가 있는 상태에서, 기존 슬롯에 넣으려고 시도할 경우	
		const BOOL isNeedCheck = (
			( firstSourceItem->GetItemIdx()		&& secondSourceItem->GetItemIdx() )			||
			( firstSourceItem->GetItemIdx()		&& ( firstSourcePosition != position ) )	||
			( secondSourceItem->GetItemIdx()	&& ( secondSourcePosition != position ) ) );

		if( isNeedCheck )
		{
			for( WORD i = 0; i < mSourceIconDialog->GetCellNum(); ++i )
			{
				CItem* item = ( CItem* )mSourceIconDialog->GetIconForIdx( i );

				if( !	item ||
						item->GetType() != WT_ITEM	||
					!	item->GetDBIdx() )
				{
					continue;
				}
				else if( script != GAMERESRCMNGR->GetComposeScript( item->GetItemIdx() ) )
				{
					WINDOWMGR->MsgBox(
						MBI_NOTICE,
						MBT_OK,
						CHATMGR->GetChatMsg( 1681 ) );
					return FALSE;
				}
			}
		}
	}

	// 080916 LUJ, 첫번째 혹은 두번째 아이콘에 소스 아이템을 연결시킨다
	{
		WORD position = 0;

		if( !	mSourceIconDialog->GetPositionForXYRef( x, y, position )	||
				position == resultPosition )
		{
			return FALSE;
		}

		// 080916 LUJ, 결과로 나온 아이템을 초기화하자
		Clear( mSourceIconDialog->GetIconForIdx( resultPosition ) );

		CItem* updateItem = ( CItem* )mSourceIconDialog->GetIconForIdx( position );

		if( !	updateItem ||
				updateItem->GetType() != WT_ITEM )
		{
			return FALSE;
		}
		else
		{
			CItem* item = ITEMMGR->GetItem( updateItem->GetDBIdx() );

			if( item )
			{
				item->SetLock( FALSE );
			}
		}

		UpdateItem( *updateItem, fakeMoveItem->GetItemBaseInfo() );

		// 080916 LUJ, 위치시킨 아이템은 사용할 수 없도록 잠근다
		fakeMoveItem->SetLock( TRUE );
	}

	DWORD sourceItemSize = 0;

	for( WORD i = 0; i < mSourceIconDialog->GetCellNum(); ++i )
	{
		CItem* item = ( CItem* )mSourceIconDialog->GetIconForIdx( i );

		if( !	item						||
				item->GetType() != WT_ITEM	||
			!	item->GetItemIdx() )
		{
			continue;
		}

		++sourceItemSize;
	}

	// 080916 LUJ, 합성 가능한 개수이면 버튼을 활성화한다.
	if( sourceItemSize == script->mSourceSize )
	{
		mSubmitButton->SetActive( TRUE );
	}
	// 080916 LUJ, 아이템이 하나만 등록된 경우에만 목록을 갱신한다
	else if( 1 == sourceItemSize )
	{
		// 080916 LUJ, 옵션 리스트에 가능한 옵션을 등록
		{
			mOptionListDialog->RemoveAll();

			const ComposeScript::OptionMap& optionMap = script->mOptionMap;

			for(	ComposeScript::OptionMap::const_iterator it = optionMap.begin();
					optionMap.end() != it;
					++it )
			{
				const ITEM_OPTION::Drop::Key&	key		= it->first;
				const ComposeScript::Option&	option	= it->second;

				char text[ MAX_PATH ] = { 0 };

				switch( key )
				{
				case ITEM_OPTION::Drop::KeyPlusStrength:
				case ITEM_OPTION::Drop::KeyPlusIntelligence:
				case ITEM_OPTION::Drop::KeyPlusDexterity:
				case ITEM_OPTION::Drop::KeyPlusWisdom:
				case ITEM_OPTION::Drop::KeyPlusVitality:
				case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
				case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
				case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
				case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
				case ITEM_OPTION::Drop::KeyPlusCriticalRate:
				case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
				case ITEM_OPTION::Drop::KeyPlusAccuracy:
				case ITEM_OPTION::Drop::KeyPlusEvade:
				case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
				case ITEM_OPTION::Drop::KeyPlusLife:
				case ITEM_OPTION::Drop::KeyPlusMana:
				case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
				case ITEM_OPTION::Drop::KeyPlusManaRecovery:
					{
						sprintf(
							text,
							"%s %+0.1f",
							ITEMMGR->GetName( key ),
							option.mValue );
						break;
					}
				case ITEM_OPTION::Drop::KeyPercentStrength:
				case ITEM_OPTION::Drop::KeyPercentIntelligence:
				case ITEM_OPTION::Drop::KeyPercentDexterity:
				case ITEM_OPTION::Drop::KeyPercentWisdom:
				case ITEM_OPTION::Drop::KeyPercentVitality:
				case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
				case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
				case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
				case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
				case ITEM_OPTION::Drop::KeyPercentCriticalRate:
				case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
				case ITEM_OPTION::Drop::KeyPercentAccuracy:
				case ITEM_OPTION::Drop::KeyPercentEvade:
				case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
				case ITEM_OPTION::Drop::KeyPercentLife:
				case ITEM_OPTION::Drop::KeyPercentMana:
				case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
				case ITEM_OPTION::Drop::KeyPercentManaRecovery:
					{
						sprintf(
							text,
							"%s %+0.1f%%",
							ITEMMGR->GetName( key ),
							option.mValue * 100.0f );
						break;
					}
				}

				mOptionListDialog->AddItemAutoLine( text, CTC_OPERATEITEM );
			}
		}

		UpdateResult( *script, 0 );
	}
	
	return FALSE;
}

void CComposeDialog::OnActionEvent( LONG id, void * p, DWORD event )
{
	switch( id )
	{
	case COMPOSE_SUBMIT_BUTTON:
		{
			CItem* item = ( CItem* )mSourceIconDialog->GetIconForIdx( firstSourcePosition );

			if( !	item	||
					item->GetType() != WT_ITEM )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, "Source item is not found" );
				break;
			}

			const ComposeScript* script = GAMERESRCMNGR->GetComposeScript( item->GetItemIdx() );

			if( ! script )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, "Script is not found" );
				break;
			}

			const int selectedRow = mOptionListDialog->GetClickedRowIdx();

			if( 0 > selectedRow )
			{
				WINDOWMGR->MsgBox(
					MBI_NOTICE,
					MBT_OK,
					CHATMGR->GetChatMsg( 1682 ) );
				break;
			}
			else if( int( script->mOptionMap.size() ) <= selectedRow )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, "It's invalid option" );
				break;
			}

			ZeroMemory( &mComposeMessage, sizeof( mComposeMessage ) );
			mComposeMessage.Category	= MP_ITEM;
			mComposeMessage.Protocol	= MP_ITEM_COMPOSE_SYN;
			mComposeMessage.dwObjectID	= HEROID;

			// 080916 LUJ, 선택한 옵션을 넣어준다
			{
				ComposeScript::OptionMap::const_iterator it = script->mOptionMap.begin();			
				std::advance( it, selectedRow );

				mComposeMessage.mOptionKey = it->first;
			}

			// 080916 LUJ, 선택한 결과를 넣어준다
			{
				if( script->mResultSet.empty() )
				{
					break;
				}

				const char* textPage = mPageStatic->GetStaticText();

				if( ! textPage )
				{
					break;
				}

				const LONG		resultPosition	= ( mResultIconGridDialog->GetCurSelCellPos() + max( 0, atoi( textPage ) - 1 ) * mResultIconGridDialog->GetCellNum() );
				const BOOL		isHidden		= ( script->mResultSet.size() != script->mResultMap.size() );
				const DWORD		resultSize		= script->mResultSet.size() + ( isHidden ? 1 : 0 );

				CItem* item = GetResultItem( POSTYPE( mResultIconGridDialog->GetCurSelCellPos() ) );

				if( !	item	||
						resultSize <= DWORD( resultPosition ) )
				{
					WINDOWMGR->MsgBox(
						MBI_NOTICE,
						MBT_OK,
						CHATMGR->GetChatMsg( 1683 ) );
					break;
				}
				// 080916 LUJ, 아이템 번호가 없으면 랜덤 결과를 선택했다는 뜻
				else if( ! item->GetItemIdx() )
				{
					mComposeMessage.mIsRandomResult = TRUE;					
				}
				else
				{
					ICONBASE& icon = mComposeMessage.mResultItem;
					icon.wIconIdx	= item->GetItemIdx();
					icon.dwDBIdx	= item->GetDBIdx();
					icon.Position	= item->GetPosition();
				}
			}

			// 080917 LUJ, 키 아이템을 설정한다
			mComposeMessage.mKeyItem = mKeyIconBase;

			// 080916 LUJ, 재료를 넣어주자
			for( WORD i = 0; i < script->mSourceSize; ++i )
			{
				CItem* item = ( CItem* )mSourceIconDialog->GetIconForIdx( i );

				if( !	item	||
						item->GetType() != WT_ITEM )
				{
					continue;
				}

				ICONBASE& icon = mComposeMessage.mUsedItem[ ( mComposeMessage.mUsedItemSize )++ ];
				icon.wIconIdx	= item->GetItemIdx();
				icon.dwDBIdx	= item->GetDBIdx();
				icon.Position	= item->GetPosition();
			}

			{
				CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );

				if( dialog )
				{
					dialog->Wait( CProgressDialog::eActionCompose );
				}
			}

			SetDisable( TRUE );
			break;
		}
	case COMPOSE_CANCEL_BUTTON:
	case CMI_CLOSEBTN:
		{
			SetActive( FALSE );
			break;
		}
	case COMPOSE_PAGE_PREV_BUTTON:
	case COMPOSE_PAGE_NEXT_BUTTON:
		{
			for( WORD i = 0; i < mSourceIconDialog->GetCellNum(); ++i )
			{
				CItem* sourceItem = ( CItem* )mSourceIconDialog->GetIconForIdx( i );

				if( !	sourceItem	||
						sourceItem->GetType() != WT_ITEM )
				{
					continue;
				}

				const ComposeScript* script = GAMERESRCMNGR->GetComposeScript( sourceItem->GetItemIdx() );

				if( ! script )
				{
					continue;
				}

				const char* page = mPageStatic->GetStaticText();

				if( ! page )
				{
					break;
				}

				// 080916 LUJ, 페이지 시작은 1부터 표시되나 실제는 0부터이다. 그래서 1을 빼준다
				UpdateResult(
					*script,
					-1 + atoi( page ) + ( id == COMPOSE_PAGE_PREV_BUTTON ? -1 : 1 ) );
				break;
			}

			break;
		}
	}
}

void CComposeDialog::SetActive( BOOL isActive )
{
	if( isActive )
	{
		Clear( *mResultIconGridDialog );
		Clear( *mSourceIconDialog );

		mOptionListDialog->RemoveAll();
		mSubmitButton->SetActive( FALSE );

		mPageStatic->SetStaticText( "0" );
		mPageNextButton->SetActive( FALSE );
		mPagePreviousButton->SetActive( FALSE );

		// 080916 LUJ, 인벤토리와 함께 지정 위치에 배치된다
		{
			cDialog* inventory = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( inventory );

			const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
			const DWORD			space	= 150;
			const DWORD			x		= ( screen.dwWidth - m_width - inventory->GetWidth() - space ) / 2;
			const DWORD			y		= ( screen.dwHeight - max( inventory->GetHeight(), m_height ) ) / 2;

			SetAbsXY( x + space + inventory->GetWidth(), y );

			inventory->SetAbsXY( x, y );
			inventory->SetActive( TRUE );
		}
	}
	// 080916 LUJ, 키 아이템과 등록된 재료들의 잠금을 해제한다
	else
	{
		CItem* keyItem = ITEMMGR->GetItem( mKeyIconBase.dwDBIdx );

		if( keyItem )
		{
			keyItem->SetLock( FALSE );
		}

		// 080916 LUJ, 등록된 아이템의 잠금을 해제한다
		for( WORD i = 0; i < mSourceIconDialog->GetCellNum(); ++i )
		{
			CItem* sourceItem = ( CItem* )mSourceIconDialog->GetIconForIdx( i );

			if( !	sourceItem	||
					sourceItem->GetType() != WT_ITEM )
			{
				continue;
			}

			CItem* inventoryItem = ITEMMGR->GetItem( sourceItem->GetDBIdx() );

			if( ! inventoryItem )
			{
				continue;
			}

			inventoryItem->SetLock( FALSE );
		}
	}

	cDialog::SetActive( isActive );
}

void CComposeDialog::SetKeyItem( const ITEMBASE& keyItem )
{
	mKeyIconBase.wIconIdx	= keyItem.wIconIdx;
	mKeyIconBase.dwDBIdx	= keyItem.dwDBIdx;
	mKeyIconBase.Position	= keyItem.Position;
}

// 080916 LUJ, 컨트롤을 초기화한다
void CComposeDialog::Clear( cIconDialog& dialog ) const
{
	for( WORD i = 0; i < dialog.GetCellNum(); ++i )
	{
		Clear( dialog.GetIconForIdx( i ) );
	}

	dialog.SetCurSelCellPos( -1 );
}

// 080916 LUJ, 컨트롤을 초기화한다
void CComposeDialog::Clear( cIconGridDialog& dialog ) const
{
	for( WORD i = 0; i < dialog.GetCellNum(); ++i )
	{
		Clear( dialog.GetIconForIdx( i ) );
	}

	dialog.SetCurSelCellPos( -1 );
}

// 080916 LUJ, 컨트롤을 초기화한다
void CComposeDialog::Clear( cIcon* icon ) const
{
	if( !	icon	||
			icon->GetType() != WT_ITEM )
	{
		return;
	}

	CItem& item = *( ( CItem* )icon );

	cImage image;
	item.SetBasicImage( &image );

	ITEMBASE emptyItemBase;
	ZeroMemory( &emptyItemBase, sizeof( emptyItemBase ) );	
	item.SetItemBaseInfo( emptyItemBase);
	item.SetToolTip( "" );
}

CItem* CComposeDialog::GetResultItem( POSTYPE position )
{
	CItem* item = ( CItem* )mResultIconGridDialog->GetIconForIdx( WORD( position ) );

	if( !	item	||
			item->GetType() != WT_ITEM )
	{
		return 0;
	}

	return item;
}

// 080916 LUJ, 합성 성공 메시지 처리
void CComposeDialog::Proceed( const MSG_ITEM_COMPOSE_ACK& message )
{
	// 080916 LUJ, 결과 아이템을 갱신하자(서버에서 재료를 결과로 바꾸었음)
	{
		// 090122 LUJ, ITEMBASE 타입으로 변경
		const ITEMBASE& resultItem = message.mResultItem;

		CItem* item = ITEMMGR->GetItem( resultItem.dwDBIdx );

		if( ! item )
		{
			cprintf( "error: result item is removed by unknown reason" );
			return;
		}

		item->SetItemBaseInfo( resultItem );
		cImage iconImage;
		item->SetBasicImage( ITEMMGR->GetIconImage( resultItem.wIconIdx, &iconImage ) );
		item->SetLock( FALSE );
		ITEMMGR->RefreshItem( item );
	}

	// 080916 LUJ, 소모된 재료를 삭제하자
	for( DWORD i = 0; i < message.mUsedItemSize; ++i )
	{
		const ICONBASE& iconBase	= message.mUsedItem[ i ];
		CItem*			item		= 0;

		ITEMMGR->DeleteItem( iconBase.Position, &item );
	}

	// 080916 LUJ, 소모된 키 아이템을 갱신하자
	{
		CItem*				keyItem		= ITEMMGR->GetItem( message.mUsedKeyItem.dwDBIdx );
		const ITEM_INFO*	keyItemInfo	= ITEMMGR->GetItemInfo( message.mUsedKeyItem.wIconIdx );

		if( ! keyItem ||
			! keyItemInfo )
		{
			return;
		}

		// 080916 LUJ, 중복되고 수량이 2개 이상 있을 경우, 수량을 수정한다
		if( keyItemInfo->Stack &&
			keyItem->GetDurability() > 1 )
		{
			ITEMBASE itemBase = keyItem->GetItemBaseInfo();
			--( itemBase.Durability );

			keyItem->SetItemBaseInfo( itemBase );
			ITEMMGR->RefreshItem( keyItem );
		}
		else
		{
			CItem* deletedItem = 0;
			ITEMMGR->DeleteItem( keyItem->GetPosition(), &deletedItem );
		}
	}

	// 080916 LUJ, 결과를 합성창에 표시하자
	{
		// 080916 LUJ, 초기화
		{
			Clear( *mResultIconGridDialog );
			Clear( *mSourceIconDialog );

			mOptionListDialog->RemoveAll();
			mSubmitButton->SetActive( FALSE );

			mPageStatic->SetStaticText( "" );
			mPageNextButton->SetActive( FALSE );
			mPagePreviousButton->SetActive( FALSE );
		}

		CItem* resultItem = ( CItem* )mSourceIconDialog->GetIconForIdx( resultPosition );

		if( !	resultItem	||
				resultItem->GetType() != WT_ITEM )
		{
			return;
		}

		// 090122 LUJ, 타입이 변경되어 직접 인자로 사용
		UpdateItem( *resultItem, message.mResultItem );
	}

	SetDisable( FALSE );
	// 090122 LUJ, 처리 후에는 재료가 없으므로 합성 버튼를 표시하지 않도록 하자
	mSubmitButton->SetActive( FALSE );

	// 080916 LUJ, 키 아이템이 없는 경우 창을 닫자. 주의: disable상태에서는 SetActive()가 작동하지 않는다
	{
		CItem* item = ITEMMGR->GetItem( mKeyIconBase.dwDBIdx );

		if( ! item )
		{
			SetActive( FALSE );
		}
	}
}

// 080916 LUJ, 합성 실패 메시지 처리
void CComposeDialog::Proceed( const MSG_ITEM_COMPOSE_NACK& message )
{
	switch( message.mType )
	{
	case MSG_ITEM_COMPOSE_NACK::TypeInvalidUsedItem:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by invalid used item" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeNotMatchedScript:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by not matched script" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeNotExistedScript:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by not existed script" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeNotExistedOption:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by not existed option" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeWrongUsedItemSize:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by wrong item size" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeInvalidResult:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by invalid result" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeEmptyResult:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by empty result" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeInvalidKeyItem:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by invalid key item" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeFailedUpdateMemory:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by memory update failure on server" );
			break;
		}
	default:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by unknown reason" );
			break;
		}
	}

	SetDisable( FALSE );
}

// 080916 LUJ, 주어진 ITEMBASE 정보로 아이콘을 갱신한다
void CComposeDialog::UpdateItem( CItem& item, const ITEMBASE& itemBase ) const
{
	cImage image;
	item.SetBasicImage( ITEMMGR->GetIconImage( itemBase.wIconIdx, &image ) );	
	item.Init(
		LONG( item.GetAbsX() ),
		LONG( item.GetAbsY() ),
		DEFAULT_ICONSIZE,
		DEFAULT_ICONSIZE,
		ITEMMGR->GetIconImage( itemBase.wIconIdx, &image ) );
	item.SetItemBaseInfo( itemBase );

	ITEMMGR->AddToolTip( &item	);
}

// 080916 LUJ, 결과를 페이지 단위로 갱신한다
void CComposeDialog::UpdateResult( const ComposeScript& script, int pageIndex )
{
	Clear( *mResultIconGridDialog );

	WORD iconIndex = 0;

	// 080916 LUJ, 입수 가능한 모든 결과는 mResultMap에, 표시되는 결과는 resultSet에 있다.
	//				개수가 다르면 숨겨진 아이템이 있다는 뜻
	const DWORD isHiddenResult = ( script.mResultMap.size() != script.mResultSet.size() );

	// 080916 LUJ, 첫번째 페이지이고, 숨겨진 아이템이 있을 경우 물음표를 표시해야한다.	
	if( !	pageIndex &&
			isHiddenResult )
	{
		CItem* item = GetResultItem( iconIndex++ );

		if( ! item )
		{
			return;
		}

		ITEMBASE itemBase;
		ZeroMemory( &itemBase, sizeof( itemBase ) );
		UpdateItem( *item, itemBase );		
		
		cImage backgroundImage;
		SCRIPTMGR->GetImage( 0, &backgroundImage, PFT_HARDPATH );
		item->SetToolTip(
			"",
			RGBA_MAKE( 255, 255, 255, 255 ),
			&backgroundImage,
			TTCLR_ITEM_CANEQUIP );
		item->AddToolTipLine( CHATMGR->GetChatMsg( 1684 ) );
	}

	// 080916 LUJ, 반복자가 이동할 위치를 정한다
	DWORD position = mResultIconGridDialog->GetCellNum() * pageIndex;
	
	if( script.mResultSet.size() <= position )
	{
		return;
	}
	else if( 0 < pageIndex && isHiddenResult )
	{
		// 100517 ONS 숨겨진 아이템이 있을경우, 
		// 두번째 페이지부터 시작 포지션을 -1해야 정상적으로 아이템이 출력된다. ( pos : 30 -> 29 )
		position -= 1; 
	}

	ComposeScript::ResultSet::const_iterator it = script.mResultSet.begin();
	std::advance( it, position );

	// 080916 LUJ, 결과 아이콘을 갱신한다
	for(	;
			it != script.mResultSet.end();
			++it )
	{
		CItem* item = GetResultItem( iconIndex );

		if( ! item )
		{
			continue;
		}

		ITEMBASE itemBase;
		ZeroMemory( &itemBase, sizeof( itemBase ) );
		itemBase.wIconIdx = *it;
		UpdateItem( *item, itemBase );

		if( mResultIconGridDialog->GetCellNum() <= ++iconIndex )
		{
			break;
		}
	}

	// 080916 LUJ, 페이지 번호 표시
	{
		char text[ MAX_PATH ] = { 0 };
		sprintf( text, "%d", pageIndex + 1 );
		mPageStatic->SetStaticText( text );
	}	

	// 080916 LUJ, 페이지 이동 버튼 표시
	{
		const DWORD resultSize	= script.mResultSet.size() + ( isHiddenResult ? 1 : 0 );
		const int pageSize	= ( resultSize / mResultIconGridDialog->GetCellNum() ) + ( 0 < ( resultSize % mResultIconGridDialog->GetCellNum() ) ? 1 : 0 );

		// 080916 LUJ, 한 페이지 이하면 페이지 버튼을 표시할 필요가 없다
		if( pageSize == 1 )
		{
			mPageNextButton->SetActive( FALSE );
			mPagePreviousButton->SetActive( FALSE );
			return;
		}
		else if( pageSize == pageIndex + 1 )
		{
			mPagePreviousButton->SetActive( TRUE );
			mPageNextButton->SetActive( FALSE );
		}
		else if( ! pageIndex )
		{
			mPagePreviousButton->SetActive( FALSE );
			mPageNextButton->SetActive( TRUE );
		}
		else
		{
			mPageNextButton->SetActive( TRUE );
			mPagePreviousButton->SetActive( TRUE );
		}
	}
}

void CComposeDialog::Send()
{
	NETWORK->Send( &mComposeMessage, mComposeMessage.GetSize() );
	// 080916 LUJ, 전송 후에는 메모리가 깨진다
	ZeroMemory( &mComposeMessage, sizeof( mComposeMessage ) );

	SetDisable( TRUE );
}
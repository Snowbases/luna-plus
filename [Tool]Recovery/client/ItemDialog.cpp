// ItemDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "ItemDialog.h"
#include "ItemAddDialog.h"
#include "DateDialog.h"
#include "QuantityDialog.h"
#include "ItemPositionDialog.h"
// 080403 LUJ, 로그 처리
#include "LogDialog.h"
#include "ItemDropOptionLogCommand.h"
#include "ItemOptionLogCommand.h"
#include "ItemLogCommand.h"
// 080716 LUJ, 펫 로그 처리
#include "PetLogCommand.h"
#include ".\itemdialog.h"


// CItemDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CItemDialog, CDialog)
CItemDialog::CItemDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CItemDialog::IDD, pParent)
	, mSelectedItemDbIndex( 0 )
	, mApplication( application )
	, mStorageSize( 0 )
	, mExtendedInventorySize( 0 )
	, mPlayerIndex( 0 )	
	, mUserIndex( 0 )
{
	EnableAutomation();
}

CItemDialog::~CItemDialog()
{
}


void CItemDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CItemDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ITEM_INVENTORY_LIST, mInventoryListCtrl);
	DDX_Control(pDX, IDC_ITEM_DATA_FRAME2, mDataFrame);
	DDX_Control(pDX, IDC_ITEM_END_TIME_STATIC, mEndTimeStatic);
	DDX_Control(pDX, IDC_ITEM_REMAIN_TIME_STATIC, mRemainTimeStatic);
	DDX_Control(pDX, IDC_ITEM_STORAGE_SIZE_COMBO, mStorageSizeCombo);
	DDX_Control(pDX, IDC_ITEM_STORAGE_MONEY_EDIT, mStorageMoneyEdit);
	DDX_Control(pDX, IDC_ITEM_TAB, mValueTabCtrl);
	DDX_Control(pDX, IDC_ITEM_ADD_BUTTON, mItemAddButton);
	DDX_Control(pDX, IDC_ITEM_STORAGE_UPDATE_BUTTON, mStorageUpdateButton);
	DDX_Control(pDX, IDC_ITEM_SAFE_MOVE_CHECK_BOX, mSafeMoveCheckBox);
	DDX_Control(pDX, IDC_ITEM_DIALOG_SEAL_RADIO, mSealingRadioButton );
	DDX_Control(pDX, IDC_ITEM_END_TIME_BUTTON, mRemainTimeButton);
	DDX_Control(pDX, IDC_ITEM_REMAIN_TIME_BUTTON, mEndTimeButton);
	DDX_Control(pDX, IDC_ITEM_DATA_UPDATE_BUTTON, mUpdateDataButton);
	DDX_Control(pDX, IDC_ITEM_PET_INVENTORY_LIST, mPetInventoryListCtrl);
	DDX_Control(pDX, IDC_ITEM_DIALOG_INVENTORY_TAB, mInventoryTabCtrl);
	DDX_Control(pDX, IDC_ITEM_PET_SUMMON_BUTTON, mPetSummonButton);
	DDX_Control(pDX, IDC_ITEM_HOUSE_INVENTORY_LIST, mHouseInventoryListCtrl);
}


BEGIN_MESSAGE_MAP(CItemDialog, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_INVENTORY, OnLvnItemchangedItemDialogInventory)
	//ON_BN_CLICKED(IDC_ITEM_DATA_UPDATE, OnBnClickedItemDialogDataUpdate)
	ON_BN_CLICKED(IDC_ITEM_DATA_UPDATE_BUTTON, OnBnClickedItemDialogDataUpdateButton)
	//ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_STORAGE, OnLvnItemchangedItemStorage)
	ON_BN_CLICKED(IDC_ITEM_DATA_DELETE_BUTTON, OnBnClickedItemDataDeleteButton)
	ON_BN_CLICKED(IDC_ITEM_INVENTORY_LOG_BUTTON, OnBnClickedItemInventoryLogButton)
	//ON_BN_CLICKED(IDC_ITEM_WAREHOUSE_LOG_BUTTON2, OnBnClickedItemWarehouseLogButton2)
	ON_BN_CLICKED(IDC_ITEM_ADD_BUTTON, OnBnClickedItemAddButton)
	//ON_EN_SETFOCUS(IDC_ITEM_END_TIME_STATIC, OnEnSetfocusItemEndTimeStatic)
	//ON_EN_SETFOCUS(IDC_ITEM_REMAIN_TIME_STATIC, OnEnSetfocusItemRemainTimeStatic)
	ON_BN_CLICKED(IDC_ITEM_END_TIME_BUTTON, OnBnClickedItemEndTimeButton)
	ON_BN_CLICKED(IDC_ITEM_REMAIN_TIME_BUTTON, OnBnClickedItemRemainTimeButton)
	ON_NOTIFY(TCN_SELCHANGE, IDC_ITEM_TAB, OnTcnSelchangeItemTab)
	ON_BN_CLICKED(IDC_ITEM_STORAGE_UPDATE_BUTTON, OnBnClickedItemStorageUpdateButton)
	ON_BN_CLICKED(IDC_ITEM_HELP_BUTTON, OnBnClickedItemHelpButton)
	ON_BN_CLICKED(IDC_ITEM_DIALOG_SEAL_RADIO, OnBnClickedItemDialogSealRadio)
	ON_NOTIFY(TCN_SELCHANGE, IDC_ITEM_DIALOG_INVENTORY_TAB, OnTcnSelchangeItemDialogInventoryTab)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_PET_INVENTORY_LIST, OnLvnItemchangedItemPetInventoryList)
	ON_BN_CLICKED(IDC_ITEM_PET_SUMMON_BUTTON, OnBnClickedItemPetSummonButton)
	ON_BN_CLICKED(IDC_ITEM_COPY_CLIPBOARD, OnBnClickedItemCopyClipboard)
	ON_BN_CLICKED(IDC_ITEM_SAVE_EXCEL_BUTTON, OnBnClickedItemSaveExcelButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CItemDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IItemDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {50406DEA-E1CA-484E-AF7D-0C8310FCB000}
static const IID IID_IItemDialog =
{ 0x50406DEA, 0xE1CA, 0x484E, { 0xAF, 0x7D, 0xC, 0x83, 0x10, 0xFC, 0xB0, 0x0 } };

BEGIN_INTERFACE_MAP(CItemDialog, CDialog)
	INTERFACE_PART(CItemDialog, IID_IItemDialog, Dispatch)
END_INTERFACE_MAP()


void CItemDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
	case MP_RM_ITEM_GET_ACK:
		{
			const MSG_RM_ITEM* m = ( MSG_RM_ITEM* )message;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const Item& item = m->mData[ i ];

				if( mItemMap.end() == mItemMap.find( item.dwDBIdx ) )
				{
					mItemMap.insert( std::make_pair( item.dwDBIdx, item ) );
				}
				else
				{
					ASSERT( 0 );
				}
			}

			{
				mValueTabCtrl.EnableWindow( TRUE );
				mInventoryListCtrl.EnableWindow( TRUE );
				mItemAddButton.EnableWindow( TRUE );
			}			

			// 080716 LUJ, 펫 인벤토리가 표시된 경우 삭제하자
			if( mInventoryTabCtrl.GetItemCount() == 2 )
			{
				mInventoryTabCtrl.DeleteItem(2);

				OnTcnSelchangeItemDialogInventoryTab( 0, 0 );
			}

			// 080611 LUJ, 버튼 초기화
			{
				mSealingRadioButton.ShowWindow( SW_HIDE );

				mEndTimeButton.EnableWindow( FALSE );
				mEndTimeStatic.SetWindowText( _T( "" ) );
				mRemainTimeButton.EnableWindow( FALSE );
				mRemainTimeStatic.SetWindowText( _T( "" ) );
			}

			typedef std::multimap< POSTYPE, Item >	DataMap;
			DataMap									dataMap;

			// 위치 별로 정렬하기 위해 다시 컨테이너에 담는다
			for( ItemMap::const_iterator it = mItemMap.begin(); mItemMap.end() != it; ++it )
			{
				const Item& item = it->second;

				// 080716 LUJ, 펫의 아이템일 경우 여기서 표시하지 않는다
				if( item.mPetIndex )
				{
					continue;
				}

				dataMap.insert( std::make_pair( item.Position, item ) );
			}

			typedef std::map< POSTYPE, int >	DuplicatedMap;
			DuplicatedMap						duplicatedMap;

			mInventoryListCtrl.DeleteAllItems();

			for(
				DataMap::const_iterator it = dataMap.begin();
				dataMap.end() != it;
				++it )
			{
				const Item& item = it->second;

				PutItem( item, mInventoryListCtrl, true );

				// 캐쉬 창고의 아이템 위치는 모두 0으로 DB에 세팅되어 있으므로, 중복 위치를 계산하지 않는다
				if( Item::AreaCashStorage == item.mArea )
				{
					continue;
				}

				++duplicatedMap[ item.Position ];
			}

			CString text;

			for(
				DuplicatedMap::const_iterator it = duplicatedMap.begin();
				duplicatedMap.end() != it;
				++it )
			{
				const POSTYPE	position	= it->first;
				const int		count		= it->second;

				if( 1 < count )
				{
					CString number;

					number.Format( _T( "[%d]" ), position );

					text += number;
				}
			}

			if( ! text.IsEmpty() )
			{
				CString textPositionIsDuplicated;
				textPositionIsDuplicated.LoadString( IDS_STRING105 );

				MessageBox( textPositionIsDuplicated + text, _T( "" ), MB_ICONERROR | MB_OK );
			}

			break;
		}
	case MP_RM_ITEM_GET_OPTION_ACK:
		{
			const MSG_RM_ITEM_OPTION* m = ( MSG_RM_ITEM_OPTION* )message;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const ITEM_OPTION& option = m->mData[ i ];

				if( mOptionMap.end() == mOptionMap.find( option.mItemDbIndex ) )
				{
					mOptionMap[ option.mItemDbIndex ] = option;
				}
				else
				{
					ASSERT( 0 && "duplicated option index found" );
				}
			}

			break;
		}	
	case MP_RM_ITEM_SET_ACK:
		{
			const MSG_RM_ITEM_UPDATE*	m		= ( MSG_RM_ITEM_UPDATE* )message;
			Item&						item	= GetItem( m->mItem.dwDBIdx );

			if( ! item.dwDBIdx )
			{
				ASSERT( 0 );
				break;
			}

			{
				const ITEMBASE& source = m->mItem;

				item.Durability		= source.Durability;
				item.dwDBIdx		= source.dwDBIdx;
				item.ItemParam		= source.ItemParam;
				item.LastCheckTime	= source.LastCheckTime;
				item.nRemainSecond	= source.nRemainSecond;
				item.nSealed		= source.nSealed;
				item.Position		= source.Position;
				item.QuickPosition	= source.QuickPosition;
				item.wIconIdx		= source.wIconIdx;
			}

			{
				CString textUpdateSucceed;
				textUpdateSucceed.LoadString( IDS_STRING28 );

				MessageBox( textUpdateSucceed, _T( "" ), MB_OK );
			}

			// 리스트 컨트롤 업데이트
			PutItem( item );
			PutData( item.dwDBIdx );
			break;
		}
	case MP_RM_ITEM_SET_OPTION_ACK:
		{
			const MSG_RM_ITEM_UPDATE_OPTION* m = ( MSG_RM_ITEM_UPDATE_OPTION* )message;

			const ITEM_OPTION&	option	= m->mOption;
			Item&				item	= GetItem( mSelectedItemDbIndex );

			if( ! item.dwDBIdx )
			{
				ASSERT( 0 );
				break;
			}

			item.Durability = option.mItemDbIndex;

			if( mApplication.IsDuplicateItem( item.wIconIdx ) )
			{
				ASSERT( 0 && "Client and server data is not mismatched" );

				return;
			}

			// 이전 아이템 옵션이 있다면 지우자	
			mOptionMap.erase( item.Durability );

			ASSERT( mOptionMap.end() == mOptionMap.find( option.mItemDbIndex ) );
			mOptionMap[ item.dwDBIdx ] = option;

			PutItem( item );
			PutData( item.dwDBIdx );

			{
				CString textUpdateSucceed;
				textUpdateSucceed.LoadString( IDS_STRING28 );

				MessageBox( textUpdateSucceed, _T( "" ), MB_OK );
			}

			break;
		}
	case MP_RM_ITEM_DELETE_ACK:
		{
			const MSG_DWORD*	m			= ( MSG_DWORD* )message;
			const DWORD			itemDbIndex = m->dwData;
			
			{
				mOptionMap.erase( itemDbIndex );
				mItemMap.erase( itemDbIndex );
			}

			// 080731 LUJ, 해당 아이템을 사용할 수 없는 상태로 한다
			{
				class
				{
				public:
					void operator()( DWORD itemDbIndex, CListCtrl& listCtrl ) const
					{
						for( int row = 0; row < listCtrl.GetItemCount(); ++row )
						{
							const CString textItemDbIndex = listCtrl.GetItemText( row, 0 );

							if( _ttoi( textItemDbIndex ) == itemDbIndex )
							{
								listCtrl.DeleteItem( row );
								break;
							}
						}
					}
				}
				DeleteItem;

				DeleteItem( itemDbIndex, mInventoryListCtrl );
				DeleteItem( itemDbIndex, mPetInventoryListCtrl );
			}

			PutData( 0 );

			{
				CString textItemWasRemoved;
				textItemWasRemoved.LoadString( IDS_STRING64 );

				MessageBox( textItemWasRemoved, _T( "" ), MB_OK );
			}
			
			break;
		}
	case MP_RM_ITEM_DELETE_OPTION_ACK:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			{
				const DWORD itemDbIndex	= m->dwData;

				OptionMap::iterator it = mOptionMap.find( itemDbIndex );

				if( mOptionMap.end() != it )
				{
					ITEM_OPTION& option = it->second;

					ZeroMemory( &option.mReinforce, sizeof( option.mReinforce ) );
					ZeroMemory( &option.mMix,		sizeof( option.mMix ) );
					ZeroMemory( &option.mEnchant,	sizeof( option.mEnchant ) );
				}

				const Item& item = GetItem( itemDbIndex );

				if( ! item.dwDBIdx )
				{
					break;
				}

				PutItem( item );
				PutData( item.dwDBIdx );
			}

			{
				CString textOptionIsCleared;
				textOptionIsCleared.LoadString( IDS_STRING65 );

				MessageBox( textOptionIsCleared, _T( "" ), MB_OK );
			}

			break;
		}
	case MP_RM_ITEM_DELETE_DROP_OPTION_ACK:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			{
				const DWORD itemDbIndex	= m->dwData;

				OptionMap::iterator it = mOptionMap.find( itemDbIndex );

				if( mOptionMap.end() != it )
				{
					ITEM_OPTION& option = it->second;

					ZeroMemory( &option.mDrop, sizeof( option.mDrop ) );
				}

				const Item& item = GetItem( itemDbIndex );

				if( ! item.dwDBIdx )
				{
					break;
				}

				PutItem( item );
				PutData( item.dwDBIdx );
			}

			{
				CString textOptionIsCleared;
				textOptionIsCleared.LoadString( IDS_STRING65 );

				MessageBox( textOptionIsCleared, _T( "" ), MB_OK );
			}

			break;
		}
	case MP_RM_ITEM_GET_OPTION_NACK:
		{
			// 정보가 없을 수 있음
			break;
		}
	case MP_RM_ITEM_SET_NACK:
	case MP_RM_ITEM_SET_OPTION_NACK:
	case MP_RM_ITEM_DELETE_OPTION_NACK:
		{
			CString textUpdateIsFailed;
			textUpdateIsFailed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateIsFailed, _T( "" ), MB_OK );
			break;
		}
	case MP_RM_ITEM_ADD_ACK:
		{
			const MSG_RM_ITEM_ADD* m = ( MSG_RM_ITEM_ADD* )message;

			const Item& item = m->mItem;

			{
				PutItem( item, mInventoryListCtrl, true );

				ASSERT( mItemMap.end() == mItemMap.find( item.dwDBIdx ) );

				mItemMap[ item.dwDBIdx ] = item;
			}

			{
				CString textItemAdd;
				textItemAdd.LoadString( IDS_STRING344 );

				// 080304 LUJ, 로그
				mApplication.Log(
					textItemAdd,
					mPlayerName,
					mPlayerIndex,
					mApplication.GetItemPositionType( item.mArea, item.Position ),
					item.Position,
					mApplication.GetItemName( item.wIconIdx ),
					item.dwDBIdx,
					max( 1, item.Durability ) );
			}

			{
				CString textCreatingItemIsSucceed;
				textCreatingItemIsSucceed.LoadString( IDS_STRING66 );

				MessageBox( textCreatingItemIsSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			}
			
			break;
		}
	case MP_RM_ITEM_ADD_NACK:
		{
			CString textCreatingItemIsFailed;
			textCreatingItemIsFailed.LoadString( IDS_STRING67 );

			MessageBox( textCreatingItemIsFailed, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_ITEM_UPDATE_REMAIN_TIME_ACK:
		{
			const MSG_INT2* m				= ( MSG_INT2* )message;
			const DWORD		itemDbIndex		= m->nData1;
			const int		remainedSecond	= m->nData2;

			Item& item = GetItem( itemDbIndex );

			if( ! item.dwDBIdx )
			{
				break;
			}

			mRemainTimeStatic.SetWindowText( mApplication.GetDateString( item.nRemainSecond	= remainedSecond ) );
			break;
		}
	case MP_RM_ITEM_UPDATE_REMAIN_TIME_NACK:
		{
			CString textUpdateIsFailed;
			textUpdateIsFailed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateIsFailed, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_ITEM_UPDATE_END_TIME_ACK:
		{
			const MSG_INT2* m				= ( MSG_INT2* )message;
			const DWORD		itemDbIndex		= m->nData1;
			const int		remainedSecond	= m->nData2;

			Item& item = GetItem( itemDbIndex );

			if( ! item.dwDBIdx )
			{
				break;
			}

			mEndTimeStatic.SetWindowText( mApplication.GetDateString( item.nRemainSecond = remainedSecond ) );
			break;
		}
	case MP_RM_ITEM_UPDATE_END_TIME_NACK:
		{
			CString textUpdateIsFailed;
			textUpdateIsFailed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateIsFailed, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_ITEM_GET_STORAGE_ACK:
		{
			{
				const MSG_DWORD3* m = ( MSG_DWORD3* )message;

				CString text;
				text.Format( _T( "%d" ), m->dwData2 );

				mStorageMoneyEdit.SetWindowText( text );
				mStorageSizeCombo.SetCurSel( m->dwData1 );

				mStorageSize			= m->dwData1;
				mExtendedInventorySize	= m->dwData3;
			}			

			// 잠궜던 컨트롤을 활성화시킨다
			{
				mStorageSizeCombo.EnableWindow( TRUE );
				mStorageMoneyEdit.EnableWindow( TRUE );
				mStorageUpdateButton.EnableWindow( TRUE );
			}

			break;
		}
	case MP_RM_ITEM_SET_STORAGE_ACK:
		{
			const MSG_DWORD2* m = ( MSG_DWORD2* )message;

			mStorageSizeCombo.SetCurSel( m->dwData1 );

			CString text;
			text.Format( _T( "%d" ), m->dwData2 );

			mStorageSize = m->dwData1;

			mStorageMoneyEdit.SetWindowText( text );

			{
				CString textUpdateIsSucceed;
				textUpdateIsSucceed.LoadString( IDS_STRING28 );

				MessageBox( textUpdateIsSucceed, _T( "" ), MB_OK | MB_ICONINFORMATION );
			}
			
			break;
		}
	case MP_RM_ITEM_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
		// 080716 LUJ, 펫 정보를 저장한다
	case MP_RM_ITEM_GET_PET_ACK:
		{
			const MSG_RM_PET* m = ( MSG_RM_PET* )message;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const Pet& pet = m->mData[ i ];

				mPetMap.insert( std::make_pair( pet.mItemDbIndex, pet ) );

				{
					const Item& item = GetItem( pet.mItemDbIndex );

					if( ! item.dwDBIdx )
					{
						continue;
					}

					PutItem( item );

					if( mSelectedItemDbIndex == pet.mItemDbIndex )
					{
						PutData( mSelectedItemDbIndex );
					}
				}
			}

			break;
		}
	case MP_RM_ITEM_SET_PET_ACK:
		{
			CString textUpdateIsSucceed;
			textUpdateIsSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateIsSucceed, _T( "" ), MB_OK | MB_ICONINFORMATION );
			PutData( mSelectedItemDbIndex );
			break;
		}
		// 080731 LUJ, 펫 추가 성공
	case MP_RM_ITEM_ADD_PET_ACK:
		{
			const MSGBASE*	m			= ( MSGBASE* )message;
			const DWORD		itemDbIndex = m->dwObjectID;

			Item& item = GetItem( itemDbIndex );

			if( ! item.dwDBIdx )
			{
				ASSERT( 0 );
				break;
			}

			item.nSealed = eITEM_TYPE_SEAL_NORMAL;
			PutItem( item );
			break;
		}
		// 080731 LUJ, 펫 추가 실패
	case MP_RM_ITEM_ADD_PET_NACK:
		{
			CString textUpdateIsSucceed;
			textUpdateIsSucceed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateIsSucceed, _T( "" ), MB_OK | MB_ICONINFORMATION );
			break;
		}
		// 080731 LUJ, 펫 삭제
	case MP_RM_ITEM_REMOVE_PET_ACK:
		{
			const MSGBASE*	m			= ( MSGBASE* )message;
			const DWORD		itemDbIndex	= m->dwObjectID;

			mPetMap.erase( itemDbIndex );

			Item& item = GetItem( itemDbIndex );

			if( ! item.dwDBIdx )
			{
				break;
			}

			item.nSealed = eITEM_TYPE_SEAL;

			PutItem( item );
			PutData( mSelectedItemDbIndex );

			// 080731 LUJ, 성공 메시지
			{
				CString textPet;
				textPet.LoadString( IDS_STRING280 );
				CString	textMessage;
				textMessage.LoadString( IDS_STRING28 );

				MessageBox( textMessage, textPet, MB_OK | MB_ICONINFORMATION );
			}

			break;
		}
	case MP_RM_ITEM_GET_FURNITURE_ACK:
		{
			mHouseInventoryListCtrl.DeleteAllItems();

			const MSG_RM_FURNITURE* const receivedMessage = (MSG_RM_FURNITURE*)message;

			for(DWORD i = 0; i < receivedMessage->mSize; ++i)
			{
				const MSG_RM_FURNITURE::Furniture& furniture = receivedMessage->mFurniture[i];

				TCHAR buffer[MAX_PATH] = {0};
				int column = -1;

				mHouseInventoryListCtrl.InsertItem(
					i,
					mApplication.GetText(furniture.mCategory),
					++column);
				mHouseInventoryListCtrl.SetItemText(
					i,
					++column,
					_itot(furniture.mItemIndex, buffer, 10));
				mHouseInventoryListCtrl.SetItemText(
					i,
					++column,
					mApplication.GetItemName(furniture.mItemIndex));

				mHouseInventoryListCtrl.SetItemText(
					i,
					++column,
					mApplication.GetText(furniture.mState));

				_stprintf(
					buffer,
					_T("%s"),
					furniture.mIsUnableRemove ? _T("o") : _T("x"));
				mHouseInventoryListCtrl.SetItemText(
					i,
					++column,
					buffer);

				mHouseInventoryListCtrl.SetItemText(
					i,
					++column,
					CString(furniture.mDate));
				_stprintf(
					buffer,
					_T("%0.0f, %0.0f, %0.0f"),
					furniture.mPosition.x,
					furniture.mPosition.y,
					furniture.mPosition.z);
				mHouseInventoryListCtrl.SetItemText(
					i,
					++column,
					buffer);
				
				_stprintf(
					buffer,
					_T("%0.0f"),
					furniture.mAngle);
				mHouseInventoryListCtrl.SetItemText(
					i,
					++column,
					buffer);

				mHouseInventoryListCtrl.SetItemText(
					i,
					++column,
					_itot(furniture.mSlot, buffer, 10));
			}

			break;
		}
	case MP_RM_ITEM_ADD_FURNITURE_ACK:
	case MP_RM_ITEM_REMOVE_FURNITURE_ACK:
		{
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


// 080320 LUJ, 드롭 옵션 그리드 추가
BOOL CItemDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CDataExchange dx( this, TRUE );
	DoDataExchange( &dx );

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	// list control initialization
	{
		// 080716 LUJ, 아이템을 동일한 형식으로 표시하는 리스트를 설정하기 위해 함수로 분리
		struct
		{
			void operator() ( CListCtrl& listCtrl ) const
			{
				CRect rect;
				const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;

				int step = -1;

				listCtrl.GetWindowRect( &rect );

				CString textDbIndex;
				textDbIndex.LoadString( IDS_STRING13 );
				listCtrl.InsertColumn( ++step, textDbIndex, LVCFMT_LEFT, 0 );

				CString textIndex;
				textIndex.LoadString( IDS_STRING22 );
				listCtrl.InsertColumn( ++step, textIndex, LVCFMT_LEFT, 0 );

				CString textType;
				textType.LoadString( IDS_STRING4 );
				listCtrl.InsertColumn( ++step, textType, LVCFMT_LEFT, int( rect.Width() * 0.14f ) );

				CString textPosition;
				textPosition.LoadString( IDS_STRING68 );
				listCtrl.InsertColumn( ++step, textPosition, LVCFMT_LEFT, int( rect.Width() * 0.13f ) );

				CString textName;
				textName.LoadString( IDS_STRING20 );
				listCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 0.44f ) );

				CString textQuantity;
				textQuantity.LoadString( IDS_STRING42 );
				listCtrl.InsertColumn( ++step, textQuantity, LVCFMT_LEFT, int( rect.Width() * 0.12f ) );

				CString textInformation;
				textInformation.LoadString( IDS_STRING69 );
				listCtrl.InsertColumn( ++step, textInformation, LVCFMT_LEFT, int( rect.Width() * 0.3f ) );

				listCtrl.SetExtendedStyle( listCtrl.GetExtendedStyle() | addStyle );
			}
		}
		SetListCtrl;

		SetListCtrl( mInventoryListCtrl );
		SetListCtrl( mPetInventoryListCtrl );
	}

	// 하우스 인벤토리
	{
		mHouseInventoryListCtrl.SetExtendedStyle(
			mHouseInventoryListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		CRect rect;
		mHouseInventoryListCtrl.GetWindowRect(
			&rect);

		int step = -1;

		CString text;
		text.LoadString(
			IDS_STRING394);
		mHouseInventoryListCtrl.InsertColumn(
			++step,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.2f));

		text.LoadString(
			IDS_STRING189);
		mHouseInventoryListCtrl.InsertColumn(
			++step,
			text,
			LVCFMT_LEFT,
			0);
		mHouseInventoryListCtrl.InsertColumn(
			++step,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.5f));

		text.LoadString(
			IDS_STRING399);
		mHouseInventoryListCtrl.InsertColumn(
			++step,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.2f));

		text.LoadString(
			IDS_STRING502);
		mHouseInventoryListCtrl.InsertColumn(
			++step,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.2f));

		text.LoadString(
			IDS_STRING102);
		mHouseInventoryListCtrl.InsertColumn(
			++step,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.3f));

		text.LoadString(
			IDS_STRING68);
		mHouseInventoryListCtrl.InsertColumn(
			++step,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.4f));

		text.LoadString(
			IDS_STRING501);
		mHouseInventoryListCtrl.InsertColumn(
			++step,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.2f));

		text.LoadString(
			IDS_STRING479);
		mHouseInventoryListCtrl.InsertColumn(
			++step,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.2f));
	}

	// grid control initialization
	{
		
		CRect frameWindowRect;
		mDataFrame.GetWindowRect(
			frameWindowRect);
		ScreenToClient(
			frameWindowRect);

		CRect gridRect = frameWindowRect;
		const int titleColumn = int( gridRect.Width() * 0.6f );
		const int valueColumn = int( gridRect.Width() * 0.4f );
		const int columnWidth = titleColumn + valueColumn;

		{
			CString textQuantity;
			textQuantity.LoadString( IDS_STRING42 );

			CString textPosition;
			textPosition.LoadString( IDS_STRING68 );

			const CString title[] = {
				textQuantity,
				textPosition,
				_T( "" )	// it must end with empty string 
			};

			mApplication.CreateGrid( this, &mItemGrid, IDC_ITEM_OPTION_GRID, gridRect, title, FALSE );

			mItemGrid.SetColumnWidth( 0, titleColumn );
			mItemGrid.SetColumnWidth( 1, valueColumn );
			//mItemGrid.SetFixedColumnCount( mItemGrid.GetColumnCount() );

			//mItemGrid.ExpandToFit();
		}

		{
			CString textStrength;
			textStrength.LoadString( IDS_STRING70 );

			CString textDexterity;
			textDexterity.LoadString( IDS_STRING71 );

			CString textHealth;
			textHealth.LoadString( IDS_STRING72 );
	
			CString textIntelligence;
			textIntelligence.LoadString( IDS_STRING73 );
			CString textWisdom;
			textWisdom.LoadString( IDS_STRING74 );

			CString textLife;
			textLife.LoadString( IDS_STRING75 );

			CString textMana;
			textMana.LoadString( IDS_STRING76 );

			CString textLifeRecovery;
			textLifeRecovery.LoadString( IDS_STRING77 );

			CString textManaRecovery;
			textManaRecovery.LoadString( IDS_STRING78 );

			CString textPhysicalAttack;
			textPhysicalAttack.LoadString( IDS_STRING79 );

			CString textPhysicalDefence;
			textPhysicalDefence.LoadString( IDS_STRING80 );

			CString textMagicalAttack;
			textMagicalAttack.LoadString( IDS_STRING81 );

			CString textMagicalDefence;
			textMagicalDefence.LoadString( IDS_STRING82 );

			CString textMoveSpeed;
			textMoveSpeed.LoadString( IDS_STRING83 );

			CString textEvade;
			textEvade.LoadString( IDS_STRING84 );

			CString textAccuracy;
			textAccuracy.LoadString( IDS_STRING85 );

			CString textCriticalRate;
			textCriticalRate.LoadString( IDS_STRING86 );

			CString textCriticalValue;
			textCriticalValue.LoadString( IDS_STRING87 );

			CString textMaker;
			textMaker.LoadString( IDS_STRING88 );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );

			CString textReinforce;
			textReinforce.LoadString( IDS_STRING89 );

			CString textMix;
			textMix.LoadString( IDS_STRING90 );

			CString textEnchant;
			textEnchant.LoadString( IDS_STRING91 );
			CString textIndex;
			textIndex.LoadString(IDS_STRING22);

			const CString title[] = {
				textReinforce	+ _T( ": " ) + textStrength			+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textDexterity		+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textHealth			+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textIntelligence		+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textWisdom			+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textLife				+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textMana				+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textLifeRecovery		+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textManaRecovery		+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textPhysicalAttack	+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textPhysicalDefence	+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textMagicalAttack	+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textMagicalDefence	+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textMoveSpeed		+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textEvade			+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textAccuracy			+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textCriticalValue	+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textCriticalRate		+ _T( "*" ),
				textReinforce	+ _T( ": " ) + textMaker			+ _T( "*" ),
				textMix			+ _T( ": " ) + textStrength			+ _T( "*" ),
				textMix			+ _T( ": " ) + textIntelligence		+ _T( "*" ),
				textMix			+ _T( ": " ) + textDexterity		+ _T( "*" ),
				textMix			+ _T( ": " ) + textWisdom			+ _T( "*" ),
				textMix			+ _T( ": " ) + textHealth			+ _T( "*" ),
				textMix			+ _T( ": " ) + textMaker			+ _T( "*" ),
				textEnchant		+ _T( ": " ) + textIndex			+ _T( "*" ),
				textEnchant		+ _T( ": " ) + textLevel			+ _T( "*" ),
				textEnchant		+ _T( ": " ) + textMaker			+ _T( "*" ),
				_T( "" )	// it must end with empty string
			};

			const int titleColumn = int( gridRect.Width() * 0.65f );
			const int valueColumn = int( gridRect.Width() * 0.35f );
			
			mApplication.CreateGrid( this, &mOptionGrid, IDC_ITEM_DATA_GRID, gridRect, title, FALSE );

			mOptionGrid.SetColumnWidth( 0, titleColumn );
			mOptionGrid.SetColumnWidth( 1, valueColumn );
			mOptionGrid.SetFixedColumnCount( mOptionGrid.GetColumnCount() );
			mOptionGrid.ExpandToFit();
		}

		// 080320 LUJ, 드롭 옵션 그리드 추가
		{
			CString textKey;
			textKey.LoadString( IDS_STRING151 );
			CString textValue;
			textValue.LoadString( IDS_STRING152 );

			const CString title[] = {
				textKey		+ _T( " 1" ),
				textValue	+ _T( " 1" ),
				textKey		+ _T( " 2" ),
				textValue	+ _T( " 2" ),
				textKey		+ _T( " 3" ),
				textValue	+ _T( " 3" ),
				textKey		+ _T( " 4" ),
				textValue	+ _T( " 4" ),
				textKey		+ _T( " 5" ),
				textValue	+ _T( " 5" ),
				_T( "" )	// it must end with empty string
			};

			const int titleColumn = int( gridRect.Width() * 0.75f );
			const int valueColumn = int( gridRect.Width() * 0.25f );
			
			mApplication.CreateGrid( this, &mDropOptionGrid, IDC_ITEM_DATA_GRID, gridRect, title, FALSE );

			mDropOptionGrid.SetColumnWidth( 0, titleColumn );
			mDropOptionGrid.SetColumnWidth( 1, valueColumn );
			mDropOptionGrid.SetFixedColumnCount( mDropOptionGrid.GetColumnCount() );
		}

		// 080716 LUJ, 펫 그리드 추가
		{
			CString textKind;
			textKind.LoadString( IDS_STRING394 );
			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );
			CString textGrade;
			textGrade.LoadString( IDS_STRING395 );
			CString textSkillSlot;
			textSkillSlot.LoadString( IDS_STRING396 );
			CString textExperience;
			textExperience.LoadString( IDS_STRING9 );
			CString textFriendship;
			textFriendship.LoadString( IDS_STRING397 );
			CString textAritificialIntelligence;
			textAritificialIntelligence	.LoadString( IDS_STRING398 );
			CString textState;
			textState.LoadString( IDS_STRING399 );
			CString textType;
			textType.LoadString( IDS_STRING4 );
			CString textHealth;
			textHealth.LoadString( IDS_STRING75 );
			CString textMana;
			textMana.LoadString( IDS_STRING76 );

			// 080731 LUJ, 등급/종류를 수정할 수 있도록 함
			const CString title[] = {
				textKind,
				textLevel + _T( "*" ),
				textGrade + _T( "*" ),
				textSkillSlot + _T( "*" ),
				textExperience + _T( "*" ),
				textFriendship + _T( "*" ),
				textAritificialIntelligence,
				textState,
				textType + _T( "*" ),
				textHealth,
				textMana,
				_T( "" )	// it must end with empty string
			};

			const int titleColumn = int( gridRect.Width() * 0.55f );
			const int valueColumn = int( gridRect.Width() * 0.45f );
			
			mApplication.CreateGrid( this, &mPetGrid, IDC_ITEM_DATA_GRID, gridRect, title, FALSE );

			mPetGrid.SetColumnWidth( 0, titleColumn );
			mPetGrid.SetColumnWidth( 1, valueColumn );
			mPetGrid.SetFixedColumnCount( mPetGrid.GetColumnCount() );
		}
	}

	// 콤보 박스
	{
		CString		text;

		for( DWORD i = 0; i <= TAB_STORAGE_NUM; ++i )
		{
			text.Format( _T( "%d" ), i );

			mStorageSizeCombo.AddString( text );
		}

		CRect rect;
		mStorageSizeCombo.SetCurSel( 0 );
		mStorageSizeCombo.GetWindowRect( rect );
		mStorageSizeCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
	}

	// 에디트 컨트롤
	{
		mStorageMoneyEdit.SetWindowText( _T( "0" ) );
	}

	// 080716 LUJ, 값 탭 컨트롤. 펫 정보는 펫이 연결된 아이템이 선택될 때만 표시시킬 것이다.
	{
		CString textInformation;
		textInformation.LoadString( IDS_STRING69 );
		mValueTabCtrl.InsertItem( mValueTabCtrl.GetItemCount(), textInformation );

		CString textOption;
		textOption.LoadString( IDS_STRING55 );
		mValueTabCtrl.InsertItem( mValueTabCtrl.GetItemCount(), textOption );

		CString textDropOption;
		textDropOption.LoadString( IDS_STRING354 );
		mValueTabCtrl.InsertItem( mValueTabCtrl.GetItemCount(), textDropOption );

		CString textPet;
		textPet.LoadString( IDS_STRING280 );
		mValueTabCtrl.InsertItem( mValueTabCtrl.GetItemCount(), textPet );
	}

	// 080716 LUJ, 인벤토리 탭 컨트롤. 펫이 가진 아이템 정보는 펫이 연결된 아이템이 선택될 때만 표시시킬 것이다.
	{
		CString text;
		text.LoadString(
			IDS_STRING54);
		mInventoryTabCtrl.InsertItem(
			mInventoryTabCtrl.GetItemCount(),
			text);

		text.LoadString(
			IDS_STRING476);
		mInventoryTabCtrl.InsertItem(
			mInventoryTabCtrl.GetItemCount(),
			text);

		mPetInventoryListCtrl.ShowWindow(
			SW_HIDE);
	}

	mHouseInventoryListCtrl.ShowWindow(
		SW_HIDE);
	mSafeMoveCheckBox.SetCheck(
		TRUE);
	mPetSummonButton.ShowWindow(
		SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CItemDialog::OnLvnItemchangedItemDialogInventory(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 리스트 컨트롤의 아이템을 선택했을 때 들어온다

	switch(mInventoryTabCtrl.GetCurSel())
	{
		// 인벤토리
	case 0:
		// 펫 인벤토리
	case 2:
		{
			const LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
			mSelectedItemDbIndex = _ttoi(
				mInventoryListCtrl.GetItemText(pNMLV->iItem, 0));

			PutData(
				mSelectedItemDbIndex);
			OnTcnSelchangeItemTab(
				0,
				0);
			break;
		}
		// 가구
	case 1:
		{
			PutData(
				0);
			OnTcnSelchangeItemTab(
				0,
				0);
			break;
		}
	}
}


// 080320 LUJ, 드롭 옵션 그리드 값 설정
void CItemDialog::OnBnClickedItemDialogDataUpdateButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	switch( mValueTabCtrl.GetCurSel() )
	{
	// item update
	case 0:
		{
			const Item& item = GetItem( mSelectedItemDbIndex );

			if( ! item.dwDBIdx )
			{
				CString textThereIsNoTarget;
				textThereIsNoTarget.LoadString( IDS_STRING31 );

				MessageBox( textThereIsNoTarget, _T( "" ), MB_OK );
				return;
			}

			POSTYPE position = _ttoi( mItemGrid.GetItemText( 1, 1 ) );

			// 위치 중복 검사
			// 080401 LUJ, 유료 아이템은 무조건 이동 위치를 0으로 설정한다
			if( Item::AreaCashStorage == item.mArea )
			{
				CString textCashItemWarningForMovement;
				textCashItemWarningForMovement.LoadString( IDS_STRING361 );

				if( position	&&
					IDCANCEL == MessageBox( textCashItemWarningForMovement, _T( "" ), MB_OKCANCEL | MB_ICONWARNING ) )
				{
					return;
				}

				position = 0;
			}
			// 080401 LUJ, 유료 아이템이 아닌 경우 위치가 중복될 경우 표시되지 않는다. 따라서 중복 위치 검사를 해줘야한다.
			else
			{
				for( int row = 0; row < mInventoryListCtrl.GetItemCount(); ++row )
				{
					const DWORD itemDbIndex = _ttoi( mInventoryListCtrl.GetItemText( row, 0 ) );

					if( itemDbIndex == item.dwDBIdx )
					{
						continue;
					}

					if( position == _ttoi( mInventoryListCtrl.GetItemText( row, 3 ) ) )
					{
						CString textPositionIsDuplicated;
						textPositionIsDuplicated.LoadString( IDS_STRING105 );

						MessageBox( textPositionIsDuplicated, _T( "" ), MB_OK | MB_ICONERROR );
						return;
					}
				}
			}

			// 080320 LUJ, 이동 상태를 두 가지로 분류함
			if( position != item.Position )
			{				
				// 080320 LUJ, 현재 위치에 기반하여 안전하게 이동할 수 있는 상태
				if( mSafeMoveCheckBox.GetCheck() )
				{
					CString textPositionIsOver;
					textPositionIsOver.LoadString( IDS_STRING106 );

					if( !	(	TP_INVENTORY_START	<= position			&& TP_INVENTORY_END		> position		 )	&&
							(	TP_INVENTORY_START	<= item.Position	&& TP_INVENTORY_END		> item.Position ) )
					{
						MessageBox( textPositionIsOver, _T( "" ), MB_OK | MB_ICONERROR );
						return;
					}
					else if( !	(	TP_STORAGE_START	<= position			&& TABCELL_STORAGE_NUM * mStorageSize		> position			)	&&
								(	TP_STORAGE_START	<= item.Position	&& TABCELL_STORAGE_NUM * mStorageSize		> item.Position	) )
					{
						MessageBox( textPositionIsOver, _T( "" ), MB_OK | MB_ICONERROR );
						return;
					}
					else if( !	(	TP_EXTENDED_INVENTORY1_START	<= position			&& TP_EXTENDED_INVENTORY1_END		> position		 )	&&
								(	TP_EXTENDED_INVENTORY1_START	<= item.Position	&& TP_EXTENDED_INVENTORY1_END		> item.Position ) )
					{
						MessageBox( textPositionIsOver, _T( "" ), MB_OK | MB_ICONERROR );
						return;
					}
					else if( !	(	TP_EXTENDED_INVENTORY2_START	<= position			&& TP_EXTENDED_INVENTORY2_END		> position		 )	&&
								(	TP_EXTENDED_INVENTORY2_START	<= item.Position	&& TP_EXTENDED_INVENTORY2_END		> item.Position ) )
					{
						MessageBox( textPositionIsOver, _T( "" ), MB_OK | MB_ICONERROR );
						return;
					}
					else if( !	(	TP_WEAR_START	<= position			&& TP_WEAR_END		> position		 )	&&
								(	TP_WEAR_START	<= item.Position	&& TP_WEAR_END		> item.Position ) )
					{
						MessageBox( textPositionIsOver, _T( "" ), MB_OK | MB_ICONERROR );
						return;
					}
					else if( !	(	TP_SHOPITEM_START	<= position			&& TP_SHOPITEM_END		> position		 )	&&
								(	TP_SHOPITEM_START	<= item.Position	&& TP_SHOPITEM_END		> item.Position ) )
					{
						MessageBox( textPositionIsOver, _T( "" ), MB_OK | MB_ICONERROR );
						return;
					}
				}
				// 080320 LUJ, 이동 범위에 제한이 없는 상태. 운영자에게 책임이 있음
				else
				{
					CString textWarningForNonSafeMove;
					textWarningForNonSafeMove.LoadString( IDS_STRING352 );

					CString text;
					text.Format( textWarningForNonSafeMove, item.Position, position );

					if( IDCANCEL == MessageBox( text, _T( "" ), MB_OKCANCEL | MB_ICONWARNING ) )
					{
						return;
					}
				}
			}

			MSG_RM_ITEM_UPDATE message;
			message.Category			= MP_RM_ITEM;
			message.Protocol			= MP_RM_ITEM_SET_SYN;
			message.dwObjectID			= mPlayerIndex;

			{
				ITEMBASE& dest = message.mItem;

				dest.Durability		= item.Durability;
				dest.dwDBIdx		= item.dwDBIdx;
				dest.ItemParam		= item.ItemParam;
				dest.LastCheckTime	= item.LastCheckTime;
				dest.nRemainSecond	= item.nRemainSecond;
				dest.nSealed		= item.nSealed;
				dest.Position		= item.Position;
				dest.QuickPosition	= item.QuickPosition;
				dest.wIconIdx		= item.wIconIdx;
			}
			
			message.mItem.Position		= position;	

			if( mApplication.IsDuplicateItem( item.wIconIdx ) )
			{
				message.mItem.Durability	= min( _ttoi( mItemGrid.GetItemText( 0, 1 ) ), MAX_YOUNGYAKITEM_DUPNUM );

				if( ! message.mItem.Durability )
				{
					CString textWarningAboutMinimumQuantity;
					textWarningAboutMinimumQuantity.LoadString( IDS_STRING353 );
					MessageBox( textWarningAboutMinimumQuantity, _T( "" ), MB_OK | MB_ICONERROR );
					return;
				}
			}

			if( ! memcmp( &item, &message.mItem, sizeof( item ) ) )
			{
				// 변경된 내역이 없음
				return;
			}

			// 080304 LUJ, 로그
			{
				CString textUpdateItem;
				textUpdateItem.LoadString( IDS_STRING345 );

				mApplication.Log(
					textUpdateItem,
					mPlayerName,
					mPlayerIndex,
					mApplication.GetItemName( message.mItem.wIconIdx ),
					message.mItem.dwDBIdx,
					message.mItem.Durability,
					message.mItem.Position );
			}

			mApplication.Send( mServerIndex, message, sizeof( message ) );
			
			break;
		}
	// option update
	case 1:
		{
			const Item& item = GetItem( mSelectedItemDbIndex );

			if( ! item.dwDBIdx )
			{
				CString textThereIsNoTarget;
				textThereIsNoTarget.LoadString( IDS_STRING31 );

				MessageBox( textThereIsNoTarget, _T( "" ), MB_OK );
				return;
			}

			const DWORD		quantity	= _ttoi( mItemGrid.GetItemText( 0, 1 ) );

			if( mApplication.IsDuplicateItem( item.wIconIdx ) )
			{
				CString textItemCannotHaveOption;

				CString textItIsInvalidTarget;
				textItIsInvalidTarget.LoadString( IDS_STRING103 );

				MessageBox( textItIsInvalidTarget, _T( "" ), MB_OK );
				return;
			}

			MSG_RM_ITEM_UPDATE_OPTION message;
			{
				ZeroMemory( &message, sizeof( message ) );

				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_SET_OPTION_SYN;
				message.dwObjectID	= mPlayerIndex;
			}

			if( mOptionMap.end() != mOptionMap.find( item.dwDBIdx ) )
			{
				message.mOption = mOptionMap[ item.dwDBIdx ];
			}

			// 값 저장
			{
				int cell = -1;

				message.mOption.mItemDbIndex = item.dwDBIdx;

				{
					ITEM_OPTION::Reinforce& data = message.mOption.mReinforce;

					data.mStrength			= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mDexterity			= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mVitality			= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mIntelligence		= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mWisdom			= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mLife				= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mMana				= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mLifeRecovery		= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mManaRecovery		= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mPhysicAttack		= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mPhysicDefence		= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mMagicAttack		= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mMagicDefence		= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mMoveSpeed			= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mEvade				= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mAccuracy			= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mCriticalRate		= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mCriticalDamage	= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );

					StringCopySafe( data.mMadeBy, CW2AEX< MAX_PATH >( mOptionGrid.GetItemText( ++cell, 1 ) ), sizeof( data.mMadeBy ) );
				}

				{
					ITEM_OPTION::Mix& data = message.mOption.mMix;

					data.mStrength			= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mIntelligence		= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mDexterity			= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mWisdom			= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mVitality			= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );

					StringCopySafe( data.mMadeBy, CW2AEX< MAX_PATH >( mOptionGrid.GetItemText( ++cell, 1 ) ), sizeof( data.mMadeBy ) );
				}

				// 아이템에 적용가능한 인챈트 인덱스를 얻어내고 레벨이 적당한지 체크해야한다
				{
					ITEM_OPTION::Enchant& data = message.mOption.mEnchant;
					data.mIndex	= _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) );
					data.mLevel	= BYTE( _ttoi( mOptionGrid.GetItemText( ++cell, 1 ) ) );
					StringCopySafe( data.mMadeBy, CW2AEX< MAX_PATH >( mOptionGrid.GetItemText( ++cell, 1 ) ), sizeof( data.mMadeBy ) );

					if(-1 == data.mIndex)
					{
						CString textMessage;
						textMessage.LoadString(
							IDS_STRING540);

						if(IDNO == MessageBox(textMessage, 0, MB_YESNO))
						{
							return;
						}
					}
					else
					{
						if(const EnchantScript* script = mApplication.GetEnchantScript(data.mIndex))
						{
							if( data.mLevel > script->mEnchantMaxLevel )
							{
								CString textItIsUnableToUpdateMoreEnchantLevel;
								textItIsUnableToUpdateMoreEnchantLevel.LoadString( IDS_STRING104 );

								CString text;
								text.Format( textItIsUnableToUpdateMoreEnchantLevel, script->mEnchantMaxLevel );

								MessageBox( text, 0, MB_OK );

								data.mLevel = BYTE( script->mEnchantMaxLevel );
							}
						}
						else if(data.mIndex > 0)
						{
							CString textItIsInvalidTarget;
							textItIsInvalidTarget.LoadString( IDS_STRING103 );

							MessageBox( textItIsInvalidTarget, _T( "" ), MB_OK );
							return;
						}
					}
				}
			}

			// 변경된 내역이 있는지 검사
			{
				OptionMap::const_iterator it = mOptionMap.find( item.dwDBIdx );

				if( mOptionMap.end() != it )
				{
					const ITEM_OPTION& previousOption = it->second;

					if( ! memcmp( &message.mOption, &previousOption, sizeof( previousOption ) ) )
					{
						return;
					}
				}
			}

			// 080304 LUJ, 로그
			{
				const ITEM_OPTION& option = message.mOption;

				const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
				const ITEM_OPTION::Mix&			mix			= option.mMix;
				const ITEM_OPTION::Enchant&		enchant		= option.mEnchant;
				const ITEM_OPTION::Drop&		drop		= option.mDrop;

				CString textItemOptionUpdate;
				textItemOptionUpdate.LoadString( IDS_STRING347 );

				const CString reinforcerName	( CA2WEX< MAX_PATH >( reinforce.mMadeBy ) );
				const CString enchanterName		( CA2WEX< MAX_PATH >( enchant.mMadeBy	) );
				const CString mixerName			( CA2WEX< MAX_PATH >( mix.mMadeBy		) );

				mApplication.Log(
					textItemOptionUpdate,
					mPlayerName,
					mPlayerIndex,
					mApplication.GetItemName( item.wIconIdx ),
					item.dwDBIdx,
					reinforce.mStrength,
					reinforce.mDexterity,
					reinforce.mVitality,
					reinforce.mIntelligence,
					reinforce.mWisdom,
					reinforce.mLife,
					reinforce.mMana,
					reinforce.mLifeRecovery,
					reinforce.mManaRecovery,
					reinforce.mPhysicAttack,
					reinforce.mPhysicDefence,
					reinforce.mMagicAttack,
					reinforce.mMagicDefence,
					reinforce.mMoveSpeed,
					reinforce.mEvade,
					reinforce.mAccuracy,
					reinforce.mCriticalRate,
					reinforce.mCriticalDamage,
					reinforcerName.IsEmpty() ? _T( "*" ) : reinforcerName,
					mix.mStrength,
					mix.mIntelligence,
					mix.mDexterity,
					mix.mWisdom,
					mix.mVitality,
					mixerName.IsEmpty() ? _T( "*" ) : mixerName,
					enchant.mIndex,
					enchant.mLevel,
					enchanterName.IsEmpty() ? _T( "*" ) : enchanterName,
					drop.mValue[ 0 ].mKey,
					drop.mValue[ 0 ].mValue,
					drop.mValue[ 1 ].mKey,
					drop.mValue[ 1 ].mValue,
					drop.mValue[ 2 ].mKey,
					drop.mValue[ 2 ].mValue,
					drop.mValue[ 3 ].mKey,
					drop.mValue[ 3 ].mValue,
					drop.mValue[ 4 ].mKey,
					drop.mValue[ 4 ].mValue );
			}			

			mApplication.Send( mServerIndex, message, sizeof( message ) );
			break;
		}
	// 080320 LUJ, 드롭 옵션 그리드값 초기화
	case 2:
		{
			const Item& item = GetItem( mSelectedItemDbIndex );

			if( ! item.dwDBIdx )
			{
				CString textThereIsNoTarget;
				textThereIsNoTarget.LoadString( IDS_STRING31 );

				MessageBox( textThereIsNoTarget, _T( "" ), MB_OK );
				return;
			}

			const DWORD	quantity = _ttoi( mItemGrid.GetItemText( 0, 1 ) );

			if( mApplication.IsDuplicateItem( item.wIconIdx ) )
			{
				CString textItemCannotHaveOption;

				CString textItIsInvalidTarget;
				textItIsInvalidTarget.LoadString( IDS_STRING103 );

				MessageBox( textItIsInvalidTarget, _T( "" ), MB_OK );
				return;
			}

			MSG_RM_ITEM_UPDATE_OPTION message;
			{
				ZeroMemory( &message, sizeof( message ) );

				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_SET_OPTION_SYN;
				message.dwObjectID	= mPlayerIndex;
			}

			if( mOptionMap.end() != mOptionMap.find( item.dwDBIdx ) )
			{
				message.mOption = mOptionMap[ item.dwDBIdx ];
			}

			// 값 저장
			{
				int cell = -1;

				message.mOption.mItemDbIndex = item.dwDBIdx;

				{
					ITEM_OPTION::Drop& data = message.mOption.mDrop;

					data.mValue[ 0 ].mKey	= ITEM_OPTION::Drop::Key(	_ttoi( mDropOptionGrid.GetItemText( ++cell, 1 ) ) );
					data.mValue[ 0 ].mValue	=					float(	_tstof( mDropOptionGrid.GetItemText( ++cell, 1 ) ) );
					data.mValue[ 1 ].mKey	= ITEM_OPTION::Drop::Key(	_ttoi( mDropOptionGrid.GetItemText( ++cell, 1 ) ) );
					data.mValue[ 1 ].mValue	= 					float(	_tstof( mDropOptionGrid.GetItemText( ++cell, 1 ) ) );
					data.mValue[ 2 ].mKey	= ITEM_OPTION::Drop::Key(	_ttoi( mDropOptionGrid.GetItemText( ++cell, 1 ) ) );
					data.mValue[ 2 ].mValue	= 					float(	_tstof( mDropOptionGrid.GetItemText( ++cell, 1 ) ) );
					data.mValue[ 3 ].mKey	= ITEM_OPTION::Drop::Key(	_ttoi( mDropOptionGrid.GetItemText( ++cell, 1 ) ) );
					data.mValue[ 3 ].mValue	= 					float(	_tstof( mDropOptionGrid.GetItemText( ++cell, 1 ) ) );
					data.mValue[ 4 ].mKey	= ITEM_OPTION::Drop::Key(	_ttoi( mDropOptionGrid.GetItemText( ++cell, 1 ) ) );
					data.mValue[ 4 ].mValue	= 					float(	_tstof( mDropOptionGrid.GetItemText( ++cell, 1 ) ) );
				}
			}

			// 변경된 내역이 있는지 검사
			{
				OptionMap::const_iterator it = mOptionMap.find( item.dwDBIdx );

				if( mOptionMap.end() != it )
				{
					const ITEM_OPTION& previousOption = it->second;

					if( ! memcmp( &message.mOption, &previousOption, sizeof( previousOption ) ) )
					{
						return;
					}
				}
			}

			// 080304 LUJ, 로그
			{
				const ITEM_OPTION& option = message.mOption;

				const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
				const ITEM_OPTION::Mix&			mix			= option.mMix;
				const ITEM_OPTION::Enchant&		enchant		= option.mEnchant;
				const ITEM_OPTION::Drop&		drop		= option.mDrop;

				CString textItemOptionUpdate;
				textItemOptionUpdate.LoadString( IDS_STRING347 );

				mApplication.Log(
					textItemOptionUpdate,
					mPlayerName,
					mPlayerIndex,
					mApplication.GetItemName( item.wIconIdx ),
					item.dwDBIdx,
					reinforce.mStrength,
					reinforce.mDexterity,
					reinforce.mVitality,
					reinforce.mIntelligence,
					reinforce.mWisdom,
					reinforce.mLife,
					reinforce.mMana,
					reinforce.mLifeRecovery,
					reinforce.mManaRecovery,
					reinforce.mPhysicAttack,
					reinforce.mPhysicDefence,
					reinforce.mMagicAttack,
					reinforce.mMagicDefence,
					reinforce.mMoveSpeed,
					reinforce.mEvade,
					reinforce.mAccuracy,
					reinforce.mCriticalRate,
					reinforce.mCriticalDamage,
					CString( "" ) == reinforce.mMadeBy ? "*" : reinforce.mMadeBy,
					mix.mStrength,
					mix.mIntelligence,
					mix.mDexterity,
					mix.mWisdom,
					mix.mVitality,
					CString( "" ) == mix.mMadeBy ? "*" : mix.mMadeBy,
					enchant.mIndex,
					enchant.mLevel,
					CString( "" ) == enchant.mMadeBy ? "*" : enchant.mMadeBy,
					drop.mValue[ 0 ].mKey,
					drop.mValue[ 0 ].mValue,
					drop.mValue[ 1 ].mKey,
					drop.mValue[ 1 ].mValue,
					drop.mValue[ 2 ].mKey,
					drop.mValue[ 2 ].mValue,
					drop.mValue[ 3 ].mKey,
					drop.mValue[ 3 ].mValue,
					drop.mValue[ 4 ].mKey,
					drop.mValue[ 4 ].mValue );
			}

			mApplication.Send( mServerIndex, message, sizeof( message ) );
			break;
		}
		// 080716 LUJ, 펫 정보
	case 3:
		{
			const PetMap::const_iterator it = mPetMap.find( mSelectedItemDbIndex );

			if( mPetMap.end() == it )
			{
				break;
			}

			MSG_RM_PET message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_ITEM;
			message.Protocol	= MP_RM_ITEM_SET_PET_SYN;
			message.dwObjectID	= mUserIndex;
			message.mSize		= 1;
			message.mData[ 0 ]	= it->second;
			
			{
				Pet& pet = message.mData[ 0 ];

				pet.mLevel		=			_ttoi( mPetGrid.GetItemText( 1, 1 ) );
				pet.mGrade		=			_ttoi( mPetGrid.GetItemText( 2, 1 ) );
				pet.mSkillSlot	=			_ttoi( mPetGrid.GetItemText( 3, 1 ) );
				pet.mExperience	= 			_ttoi( mPetGrid.GetItemText( 4, 1 ) );
				pet.mFriendship	=			_ttoi( mPetGrid.GetItemText( 5, 1 ) );
				// 080731 LUJ, 진화 종류를 변경하도록 값을 받는다
				pet.mType		= ePetType(	_ttoi( mPetGrid.GetItemText( 8, 1 ) ) );

				// 080731 LUJ, 펫의 레벨에 따라 가질 수 있는 상태가 정해져있다. 이를 위반했는지 검사하자
				{
					const CclientApp::PetTypeSet& typeSet = mApplication.GetPetType( pet.mLevel );

					if( typeSet.end() == typeSet.find( pet.mType ) )
					{
						// 080731 LUJ, 선택할 수 있는 것이 하나 뿐일 때는 값이 정해짐을 알린다.
						if( 1 == typeSet.size() )
						{
							const ePetType type = *( typeSet.begin() );

							CString textPet;
							textPet.LoadString( IDS_STRING280 );
							CString textMessage;
							textMessage.LoadString( IDS_STRING403 );

							if( IDNO == MessageBox( textMessage + _T( ": " ) + mApplication.GetText( type ), textPet, MB_YESNO ) )
							{
								PutData( mSelectedItemDbIndex );
								break;
							}

							pet.mType = type;
						}
						else
						{
							CString typeText;

							// 080731 LUJ, 선택 가능 종류 문자열 조립
							for(
								CclientApp::PetTypeSet::const_iterator it = typeSet.begin();
								typeSet.end() != it;
								++it )
							{
								const ePetType type = *it;

								CString text;
								text.Format(
									_T( "%s(%d) " ),
									mApplication.GetText( type ),
									type );

								typeText += text;
							}

							CString textPet;
							textPet.LoadString( IDS_STRING280 );
							CString textMessage;
							textMessage.LoadString( IDS_STRING404 );

							MessageBox( textMessage + _T( ": " ) + typeText , textPet, MB_OK );
							PutData( mSelectedItemDbIndex );
							break;
						}
					}
				}

				// 080731 LUJ, 펫의 종류/등급에 따라 가질 수 있는 정보가 있다. 정보가 있는지 검사하자
				{
					const CclientApp::PetBodyScript&	body		= mApplication.GetPetBodyScript( pet.mType, pet.mGrade );
					const CclientApp::PetBodyScript		emptyBody	= { 0 };

					if( ! memcmp( &body, &emptyBody, sizeof( body ) ) )
					{
						CString textPet;
						textPet.LoadString( IDS_STRING280 );
						CString textMessage;
						textMessage.LoadString( IDS_STRING405 );

						MessageBox( textMessage, textPet, MB_OK );
						PutData( mSelectedItemDbIndex );
						break;
					}
				}

				// 080731 LUJ, 설정 조건 체크
				{
					const CclientApp::PetSystemScript& script = mApplication.GetPetSystemScript();

					if( script.mMaxGrade < pet.mGrade )
					{
						CString textPet;
						textPet.LoadString( IDS_STRING280 );
						CString textMessage;
						textMessage.LoadString( IDS_STRING407 );
						CString text;
						text.Format( textMessage, script.mMaxGrade );

						MessageBox( text, textPet, MB_OK );
						PutData( mSelectedItemDbIndex );
						break;
					}
					else if( script.mMaxLevel < pet.mLevel )
					{
						CString textPet;
						textPet.LoadString( IDS_STRING280 );
						CString textMessage;
						textMessage.LoadString( IDS_STRING408 );
						CString text;
						text.Format( textMessage, script.mMaxLevel );

						MessageBox( text, textPet, MB_OK );
						PutData( mSelectedItemDbIndex );
						break;
					}
				}

				if( ! memcmp( &pet, &( it->second ), sizeof( pet ) ) )
				{
					break;
				}

				// 080731 LUJ, 변경한 정보를 저장하자
				mPetMap[ mSelectedItemDbIndex ] = pet;
			}

			{
				const Pet& pet = message.mData[ 0 ];

				CString textPet;
				textPet.LoadString( IDS_STRING280 );
				CString textUpdate;
				textUpdate.LoadString( IDS_STRING295 );

				mApplication.Log(
					_T( "%s: index: %d, item DB index: %d, kind: %d, level: %d, grade: %d, skill slot: %d, experience: %d, friendship: %d, ariticial intelligence: %d, state: %d, type: %d" ),
					textPet + _T( " " ) + textUpdate,
					pet.mIndex,
					pet.mItemDbIndex,
					pet.mKind,
					pet.mLevel,
					pet.mGrade,
					pet.mSkillSlot,
					pet.mExperience,
					pet.mFriendship,
					pet.mArtificialIntelligence,
					pet.mState,
					pet.mType );
			}

			mApplication.Send( mServerIndex, message, message.GetSize() );
			break;
		}
	}
}


void CItemDialog::PutData( DWORD itemDbIndex )
{
	// 080731 LUJ, 그리드 초기화
	{
		// 아이템 그리드 클리어
		{
			mItemGrid.ClearCells( CCellRange( 0, 1, mItemGrid.GetRowCount() - 1, 1 ) );
			mItemGrid.SetFixedColumnCount( mItemGrid.GetColumnCount() );
			mItemGrid.SetEditable( FALSE);
			mItemGrid.ShowWindow( SW_HIDE );
		}

		// 옵션 그리드 클리어
		{
			mOptionGrid.ClearCells( CCellRange( 0, 1, mOptionGrid.GetRowCount() - 1, 1 ) );
			mOptionGrid.SetFixedColumnCount( mOptionGrid.GetColumnCount() );
			mOptionGrid.SetEditable( FALSE );
			mOptionGrid.ShowWindow( SW_HIDE );
		}

		// 080320 LUJ, 드롭 옵션 그리드 초기화
		{
			mDropOptionGrid.ClearCells( CCellRange( 0, 1, mDropOptionGrid.GetRowCount() - 1, 1 ) );
			mDropOptionGrid.SetFixedColumnCount( mDropOptionGrid.GetColumnCount() );
			mDropOptionGrid.SetEditable( FALSE );
			mDropOptionGrid.ShowWindow( SW_HIDE );
		}

		// 080716 LUJ, 펫 그리드 초기화
		{
			mPetGrid.ClearCells( CCellRange( 0, 1, mDropOptionGrid.GetRowCount() - 1, 1 ) );
			mPetGrid.SetFixedColumnCount( mPetGrid.GetColumnCount() );
			mPetGrid.SetEditable( FALSE );
			mPetGrid.ShowWindow( SW_HIDE );
		}
	}

	const Item& item = GetItem( itemDbIndex );

	if( ! item.dwDBIdx )
	{
		return;
	}

	// 080716 LUJ, 기본적으로 정보가 표시되도록 한다
	mValueTabCtrl.SetCurSel( 0 );

	const BOOL isDuplicated = mApplication.IsDuplicateItem( item.wIconIdx );

	// 아이템 기본 정보 처리
	{
		mItemGrid.SetEditable( TRUE);
		mItemGrid.SetFixedColumnCount( 1 );

		const DWORD	style	= DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
		GV_ITEM		cell	= { 0 };
		{
			cell.nFormat	= style;
			cell.col		= 1;
			cell.mask		= GVIF_TEXT;
		}

		CclientApp::SetCellValue( item.Durability, mItemGrid, cell, isDuplicated ? GVIS_MODIFIED : GVIS_READONLY );
		++cell.row;

		CclientApp::SetCellValue( item.Position, mItemGrid, cell, GVIS_MODIFIED );
		++cell.row;
	}

	// 080611 LUJ, 봉인 정보 초기화
	{
		mSealingRadioButton.ShowWindow( SW_HIDE );

		mRemainTimeButton.EnableWindow( FALSE );
		mEndTimeButton.EnableWindow( FALSE );
	}

	mOptionGrid.SetFixedColumnCount( 1 );
	mOptionGrid.SetEditable( TRUE );

	// 080320 LUJ, 드롭 옵션 초기화
	mDropOptionGrid.SetFixedColumnCount( 1 );
	mDropOptionGrid.SetEditable( TRUE );

	const ItemScript& info = mApplication.GetItemScript( item.wIconIdx );

	if( ! info.ItemIdx )
	{
		return;
	}

	// 080611 LUJ, 봉인 정보 갱신
	switch( item.nSealed )
	{
	case eITEM_TYPE_SEAL:
		{
			CString textUnseal;
			textUnseal.LoadString( IDS_STRING272 );

			mSealingRadioButton.SetWindowText( textUnseal );
			mSealingRadioButton.ShowWindow( SW_SHOW );
			mSealingRadioButton.SetCheck( 0 );

			mRemainTimeButton.EnableWindow( FALSE );
			mEndTimeButton.EnableWindow( FALSE );
			break;
		}
	case eITEM_TYPE_UNSEAL:
		{
			CString textSeal;
			textSeal.LoadString( IDS_STRING101 );

			mSealingRadioButton.SetWindowText( textSeal );
			mSealingRadioButton.ShowWindow( SW_SHOW );
			mSealingRadioButton.SetCheck( 1 );

			mRemainTimeButton.EnableWindow( TRUE );
			mEndTimeButton.EnableWindow( TRUE );
			break;
		}
	}

	switch( info.nTimeKind )
	{
	case eKIND_REALTIME:
		{
			mEndTimeStatic.SetWindowText( mApplication.GetDateString( item.nRemainSecond ) );
			mRemainTimeStatic.SetWindowText( _T( "" ) );
			break;
		}
	case eKIND_PLAYTIME:
		{
			mEndTimeStatic.SetWindowText( _T( "" ) );
			mRemainTimeStatic.SetWindowText( mApplication.GetDateString( item.LastCheckTime ) );
			break;
		}
	default:
		{
			ASSERT( 0 );

			mEndTimeStatic.SetWindowText( _T( "" ) );
			mRemainTimeStatic.SetWindowText( _T( "" ) );
			break;
		}
	}

	OptionMap::const_iterator it = mOptionMap.find( item.dwDBIdx );

	if( mOptionMap.end() != it )
	{
		const ITEM_OPTION& option = it->second;

		const DWORD	style	= DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
		GV_ITEM		cell	= { 0 };
		cell.nFormat		= style;
		cell.col			= 1;
		cell.mask			= GVIF_TEXT;
		
		// 강화 옵션 정보
		{
			const ITEM_OPTION::Reinforce& data = option.mReinforce;

			CclientApp::SetCellValue( data.mStrength, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mDexterity, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mVitality, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mIntelligence, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mWisdom, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mLife, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mMana, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mLifeRecovery, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mManaRecovery, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mPhysicAttack, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mPhysicDefence, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mMagicAttack, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mMagicDefence, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mMoveSpeed, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mEvade, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mAccuracy, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mCriticalRate, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mCriticalDamage, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( CString( CA2WEX< sizeof( data.mMadeBy ) >( data.mMadeBy ) ), mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;
		}

		// 조합 옵션
		{
			const ITEM_OPTION::Mix& data = option.mMix;

			CclientApp::SetCellValue( data.mStrength, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mIntelligence, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mDexterity, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mWisdom, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mVitality, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( CString( CA2WEX< sizeof( data.mMadeBy ) >( data.mMadeBy ) ), mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;
		}

		// 인챈트 옵션
		{
			const ITEM_OPTION::Enchant& data = option.mEnchant;

			CclientApp::SetCellValue( data.mIndex, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mLevel, mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( CString( CA2WEX< sizeof( data.mMadeBy ) >( data.mMadeBy ) ), mOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;
		}

		// 080320 LUJ, 드롭 옵션은 옵션과는 다른 컨트롤에 표시되므로 초기화해야한다
		cell.row = 0;

		// 080320 LUJ, 드롭 옵션
		{
			const ITEM_OPTION::Drop& data = option.mDrop;

			const ITEM_OPTION::Drop::Value& value1 = data.mValue[ 0 ];
			const ITEM_OPTION::Drop::Value& value2 = data.mValue[ 1 ];
			const ITEM_OPTION::Drop::Value& value3 = data.mValue[ 2 ];
			const ITEM_OPTION::Drop::Value& value4 = data.mValue[ 3 ];
			const ITEM_OPTION::Drop::Value& value5 = data.mValue[ 4 ];

			CclientApp::SetCellValue( value1.mKey, mDropOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( value1.mValue, mDropOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( value2.mKey, mDropOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( value2.mValue, mDropOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( value3.mKey, mDropOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( value3.mValue, mDropOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( value4.mKey, mDropOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( value5.mValue, mDropOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( value5.mKey, mDropOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( data.mValue[ 4 ].mValue, mDropOptionGrid, cell, GVIS_MODIFIED );
			++cell.row;
		}
	}

	// 080716 LUJ, 펫 정보 처리
	if( Item::AreaPet != item.mArea )
	{
		const PetMap::const_iterator it = mPetMap.find( itemDbIndex );

		if( mPetMap.end() == it )
		{
			if( mInventoryTabCtrl.GetItemCount() == 3 &&
				mInventoryTabCtrl.GetCurSel() == 0)
			{
				mInventoryTabCtrl.DeleteItem( 2 );
				mInventoryTabCtrl.SetCurSel( 0 );

				OnTcnSelchangeItemDialogInventoryTab( 0, 0 );
			}
		}
		else
		{
			// 080716 LUJ, 펫 정보가 자동으로 표시되도록 한다
			mValueTabCtrl.SetCurSel( 3 );

			mPetGrid.SetFixedColumnCount( 1 );
			mPetGrid.SetEditable( TRUE );

			// 080716 LUJ, 펫이 가진 아이템을 표시할 수 있도록 탭을 추가한다
			if( mInventoryTabCtrl.GetItemCount() == 2 )
			{
				CString textPet;
				textPet.LoadString( IDS_STRING280 );
				mInventoryTabCtrl.InsertItem( mInventoryTabCtrl.GetItemCount(), textPet );
				mInventoryTabCtrl.SetCurSel( 0 );
			}

			const Pet& pet = it->second;

			// 080716 LUJ, 펫 보유 아이템 리스트를 갱신한다
			{
				mPetInventoryListCtrl.DeleteAllItems();

				for(
					ItemMap::const_iterator it = mItemMap.begin();
					mItemMap.end() != it;
					++it )
				{
					const Item& item = it->second;

					if( pet.mIndex != item.mPetIndex )
					{
						continue;
					}

					PutItem( item, mPetInventoryListCtrl, true );
				}
			}

			const DWORD	style	= DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
			GV_ITEM		cell	= { 0 };
			cell.nFormat		= style;
			cell.col			= 1;
			cell.mask			= GVIF_TEXT;
			
			CclientApp::SetCellValue( mApplication.GetPetName( pet.mKind ), mPetGrid, cell, GVIS_READONLY );
			++cell.row;

			CclientApp::SetCellValue( pet.mLevel, mPetGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( pet.mGrade, mPetGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( pet.mSkillSlot, mPetGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( pet.mExperience, mPetGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( pet.mFriendship, mPetGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( mApplication.GetPetAritificialIntelligenceText( pet.mArtificialIntelligence ), mPetGrid, cell, GVIS_MODIFIED );
			++cell.row;

			CclientApp::SetCellValue( mApplication.GetText( pet.mState ), mPetGrid, cell, GVIS_READONLY );
			++cell.row;

			CclientApp::SetCellValue( pet.mType, mPetGrid, cell, GVIS_MODIFIED );
			++cell.row;

			cell.strText.Format( _T( "%d/%d" ), pet.mHealth, pet.mMaxHealth );
			CclientApp::SetCellValue( cell.strText, mPetGrid, cell, GVIS_READONLY );
			++cell.row;

			cell.strText.Format( _T( "%d/%d" ), pet.mMana, pet.mMaxMana );
			CclientApp::SetCellValue( cell.strText, mPetGrid, cell, GVIS_READONLY );
			++cell.row;
		}
	}

	OnTcnSelchangeItemTab( 0, 0 );
}


// 080401 LUJ, 반환 타입을 Item으로 변경
// 080731 LUJ, 참조로 반환하도록 함
Item& CItemDialog::GetItem( DWORD itemDbIndex )
{
	ItemMap::iterator it = mItemMap.find( itemDbIndex );

	if( mItemMap.end() == it )
	{
		static Item item;

		return item;
	}

	return it->second;
}


void CItemDialog::OnBnClickedItemDataDeleteButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	switch( mValueTabCtrl.GetCurSel() )
	{
	// item delete
	case 0:
		{
			const Item& item = GetItem( mSelectedItemDbIndex );

			if( ! item.dwDBIdx )
			{
				CString textThereIsNoTarget;
				textThereIsNoTarget.LoadString( IDS_STRING31 );

				MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONWARNING | MB_OK );
				return;
			}

			CString textDoYouDeleteItem;
			textDoYouDeleteItem.LoadString( IDS_STRING108 );

			if( IDNO == MessageBox( textDoYouDeleteItem, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
			{
				return;
			}

			// 080304 LUJ, 로그
			{
				CString textItemRemove;
				textItemRemove.LoadString( IDS_STRING348 );

				mApplication.Log(
					textItemRemove,
					mPlayerName,
					mPlayerIndex,
					mApplication.GetItemName( item.wIconIdx ),
					item.dwDBIdx );
			}

			// 080320 LUJ, 삭제될 옵션 표시
			{
				OptionMap::const_iterator it = mOptionMap.find( item.dwDBIdx );

				if( mOptionMap.end() != it )
				{
					const ITEM_OPTION& option = it->second;

					const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
					const ITEM_OPTION::Mix&			mix			= option.mMix;
					const ITEM_OPTION::Enchant&		enchant		= option.mEnchant;
					const ITEM_OPTION::Drop&		drop		= option.mDrop;

					CString textItemOptionUpdate;
					textItemOptionUpdate.LoadString( IDS_STRING347 );

					mApplication.Log(
						textItemOptionUpdate,
						mPlayerName,
						mPlayerIndex,
						mApplication.GetItemName( item.wIconIdx ),
						item.dwDBIdx,
						reinforce.mStrength,
						reinforce.mDexterity,
						reinforce.mVitality,
						reinforce.mIntelligence,
						reinforce.mWisdom,
						reinforce.mLife,
						reinforce.mMana,
						reinforce.mLifeRecovery,
						reinforce.mManaRecovery,
						reinforce.mPhysicAttack,
						reinforce.mPhysicDefence,
						reinforce.mMagicAttack,
						reinforce.mMagicDefence,
						reinforce.mMoveSpeed,
						reinforce.mEvade,
						reinforce.mAccuracy,
						reinforce.mCriticalRate,
						reinforce.mCriticalDamage,
						CString( "" ) == reinforce.mMadeBy ? "*" : reinforce.mMadeBy,
						mix.mStrength,
						mix.mIntelligence,
						mix.mDexterity,
						mix.mWisdom,
						mix.mVitality,
						CString( "" ) == mix.mMadeBy ? "*" : mix.mMadeBy,
						enchant.mIndex,
						enchant.mLevel,
						CString( "" ) == enchant.mMadeBy ? "*" : enchant.mMadeBy,
						drop.mValue[ 0 ].mKey,
						drop.mValue[ 0 ].mValue,
						drop.mValue[ 1 ].mKey,
						drop.mValue[ 1 ].mValue,
						drop.mValue[ 2 ].mKey,
						drop.mValue[ 2 ].mValue,
						drop.mValue[ 3 ].mKey,
						drop.mValue[ 3 ].mValue,
						drop.mValue[ 4 ].mKey,
						drop.mValue[ 4 ].mValue );
				}
			}

			MSG_DWORD message;
			{
				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_DELETE_SYN;
				message.dwObjectID	= mPlayerIndex;
				message.dwData		= item.dwDBIdx;
			}			
			
			mApplication.Send( mServerIndex, message, sizeof( message ) );
			break;
		}
	// option delete
	case 1:
		{
			const Item& item = GetItem( mSelectedItemDbIndex );

			if( ! item.dwDBIdx )
			{
				ASSERT( 0 );
				return;
			}

			if(	mApplication.IsDuplicateItem( item.wIconIdx )	||
				mOptionMap.end() == mOptionMap.find( item.dwDBIdx ) )
			{
				return;
			}

			CString textDoYouClearOption;
			textDoYouClearOption.LoadString( IDS_STRING107 );

			if( IDNO == MessageBox( textDoYouClearOption, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
			{
				return;
			}

			// 080304 LUJ, 로그
			{
				CString textItemOptionRemove;
				textItemOptionRemove.LoadString( IDS_STRING349 );

				mApplication.Log(
					textItemOptionRemove,
					mPlayerName,
					mPlayerIndex,
					mApplication.GetItemName( item.wIconIdx ),
					item.dwDBIdx );
			}

			// 080320 LUJ, 삭제될 옵션 표시
			{
				OptionMap::const_iterator it = mOptionMap.find( item.dwDBIdx );

				if( mOptionMap.end() != it )
				{
					const ITEM_OPTION& option = it->second;

					// 080320 LUJ, 옵션 표시
					{
						const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
						const ITEM_OPTION::Mix&			mix			= option.mMix;
						const ITEM_OPTION::Enchant&		enchant		= option.mEnchant;
						const ITEM_OPTION::Drop&		drop		= option.mDrop;

						CString textItemOptionUpdate;
						textItemOptionUpdate.LoadString( IDS_STRING347 );

						mApplication.Log(
							textItemOptionUpdate,
							mPlayerName,
							mPlayerIndex,
							mApplication.GetItemName( item.wIconIdx ),
							item.dwDBIdx,
							reinforce.mStrength,
							reinforce.mDexterity,
							reinforce.mVitality,
							reinforce.mIntelligence,
							reinforce.mWisdom,
							reinforce.mLife,
							reinforce.mMana,
							reinforce.mLifeRecovery,
							reinforce.mManaRecovery,
							reinforce.mPhysicAttack,
							reinforce.mPhysicDefence,
							reinforce.mMagicAttack,
							reinforce.mMagicDefence,
							reinforce.mMoveSpeed,
							reinforce.mEvade,
							reinforce.mAccuracy,
							reinforce.mCriticalRate,
							reinforce.mCriticalDamage,
							CString( "" ) == reinforce.mMadeBy ? "*" : reinforce.mMadeBy,
							mix.mStrength,
							mix.mIntelligence,
							mix.mDexterity,
							mix.mWisdom,
							mix.mVitality,
							CString( "" ) == mix.mMadeBy ? "*" : mix.mMadeBy,
							enchant.mIndex,
							enchant.mLevel,
							CString( "" ) == enchant.mMadeBy ? "*" : enchant.mMadeBy,
							drop.mValue[ 0 ].mKey,
							drop.mValue[ 0 ].mValue,
							drop.mValue[ 1 ].mKey,
							drop.mValue[ 1 ].mValue,
							drop.mValue[ 2 ].mKey,
							drop.mValue[ 2 ].mValue,
							drop.mValue[ 3 ].mKey,
							drop.mValue[ 3 ].mValue,
							drop.mValue[ 4 ].mKey,
							drop.mValue[ 4 ].mValue );
					}
				}
			}

			MSG_DWORD message;
			{
				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_DELETE_OPTION_SYN;
				message.dwObjectID	= mPlayerIndex;
				message.dwData		= item.dwDBIdx;
			}			
			
			mApplication.Send( mServerIndex, message, sizeof( message ) );
			break;
		}
	// 080320 LUJ, 드롭 옵션 삭제
	case 2:
		{
			const Item& item = GetItem( mSelectedItemDbIndex );

			if( ! item.dwDBIdx )
			{
				ASSERT( 0 );
				return;
			}

			if(	mApplication.IsDuplicateItem( item.wIconIdx )	||
				mOptionMap.end() == mOptionMap.find( item.dwDBIdx ) )
			{
				return;
			}

			CString textDoYouClearOption;
			textDoYouClearOption.LoadString( IDS_STRING107 );

			if( IDNO == MessageBox( textDoYouClearOption, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
			{
				return;
			}

			OptionMap::const_iterator it = mOptionMap.find( item.dwDBIdx );
			
			// 080320 LUJ, 드롭 옵션 표시
			if( mOptionMap.end() != it )
			{
				const ITEM_OPTION& option = it->second;

				const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
				const ITEM_OPTION::Mix&			mix			= option.mMix;
				const ITEM_OPTION::Enchant&		enchant		= option.mEnchant;
				const ITEM_OPTION::Drop&		drop		= option.mDrop;

				CString textItemOptionUpdate;
				textItemOptionUpdate.LoadString( IDS_STRING347 );

				mApplication.Log(
					textItemOptionUpdate,
					mPlayerName,
					mPlayerIndex,
					mApplication.GetItemName( item.wIconIdx ),
					item.dwDBIdx,
					reinforce.mStrength,
					reinforce.mDexterity,
					reinforce.mVitality,
					reinforce.mIntelligence,
					reinforce.mWisdom,
					reinforce.mLife,
					reinforce.mMana,
					reinforce.mLifeRecovery,
					reinforce.mManaRecovery,
					reinforce.mPhysicAttack,
					reinforce.mPhysicDefence,
					reinforce.mMagicAttack,
					reinforce.mMagicDefence,
					reinforce.mMoveSpeed,
					reinforce.mEvade,
					reinforce.mAccuracy,
					reinforce.mCriticalRate,
					reinforce.mCriticalDamage,
					CString( "" ) == reinforce.mMadeBy ? "*" : reinforce.mMadeBy,
					mix.mStrength,
					mix.mIntelligence,
					mix.mDexterity,
					mix.mWisdom,
					mix.mVitality,
					CString( "" ) == mix.mMadeBy ? "*" : mix.mMadeBy,
					enchant.mIndex,
					enchant.mLevel,
					CString( "" ) == enchant.mMadeBy ? "*" : enchant.mMadeBy,
					drop.mValue[ 0 ].mKey,
					drop.mValue[ 0 ].mValue,
					drop.mValue[ 1 ].mKey,
					drop.mValue[ 1 ].mValue,
					drop.mValue[ 2 ].mKey,
					drop.mValue[ 2 ].mValue,
					drop.mValue[ 3 ].mKey,
					drop.mValue[ 3 ].mValue,
					drop.mValue[ 4 ].mKey,
					drop.mValue[ 4 ].mValue );
			}

			MSG_DWORD message;
			{
				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_DELETE_DROP_OPTION_SYN;
				message.dwObjectID	= mPlayerIndex;
				message.dwData		= item.dwDBIdx;
			}

			mApplication.Send( mServerIndex, message, sizeof( message ) );
			break;
		}
		// 080731 LUJ, 애완동물 삭제
	case 3:
		{
			if( mPetMap.end() == mPetMap.find( mSelectedItemDbIndex ) )
			{
				break;
			}

			// 080731 LUJ, 삭제 확인
			{
				CString textPet;
				textPet.LoadString( IDS_STRING280 );
				CString textMessage;
				textMessage.LoadString( IDS_STRING108 );

				if( IDNO == MessageBox( textMessage, textPet, MB_YESNO | MB_ICONWARNING ) )
				{
					break;
				}
			}

			// 080731 LUJ, 로그
			{
				const Pet& pet = mPetMap[ mSelectedItemDbIndex ];

				CString textPet;
				textPet.LoadString( IDS_STRING280 );
				CString textRemove;
				textRemove.LoadString( IDS_STRING253 );

				mApplication.Log(
					_T( "%s: index: %d, item DB index: %d, kind: %d, level: %d, grade: %d, skill slot: %d, experience: %d, friendship: %d, ariticial intelligence: %d, state: %d, type: %d" ),
					textPet + _T( " " ) + textRemove,
					pet.mIndex,
					pet.mItemDbIndex,
					pet.mKind,
					pet.mLevel,
					pet.mGrade,
					pet.mSkillSlot,
					pet.mExperience,
					pet.mFriendship,
					pet.mArtificialIntelligence,
					pet.mState,
					pet.mType );
			}

			MSG_DWORD2 message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_ITEM;
			message.Protocol	= MP_RM_ITEM_REMOVE_PET_SYN;
			message.dwObjectID	= mUserIndex;
			message.dwData1		= mSelectedItemDbIndex;

			mApplication.Send( mServerIndex, message, sizeof( message ) );
			break;
		}
	}
}


void CItemDialog::Request( DWORD serverIndex, const CString& playerName, DWORD playerIndex, DWORD userIndex )
{
	mServerIndex	= serverIndex;
	mPlayerName		= playerName;
	mPlayerIndex	= playerIndex;
	mUserIndex		= userIndex;
	
	// 아이템 정보창은 피호출 처지이므로 누구에 의해 호출되었는지 알 수 없다. 타이틀을 직접 세팅해주자
	{
		CString textTitle;
		textTitle.LoadString( IDS_STRING109 );

		CString	text;
		text.Format( textTitle, mApplication.GetServerName( serverIndex ), playerName, playerIndex );

		CDialog* dialog = mApplication.GetItemDialog();
		ASSERT( dialog );

		dialog->SetWindowText( text );
	}

	mItemMap.clear();
	mOptionMap.clear();
	mPetMap.clear();

	PutData( 0 );

	// 컨트롤을 잠가놓고, 결과가 오면 활성화시킨다
	{
		mItemAddButton.EnableWindow( FALSE );
		mInventoryListCtrl.EnableWindow( FALSE );
		mValueTabCtrl.EnableWindow( FALSE );
		mStorageSizeCombo.EnableWindow( FALSE );
		mStorageMoneyEdit.EnableWindow( FALSE );
		mStorageUpdateButton.EnableWindow( FALSE );
		// 080611 LUJ, 봉인 토글 버튼
		mSealingRadioButton.ShowWindow( SW_HIDE );
	}

	{
		MSG_DWORD message;

		message.Category	= MP_RM_ITEM;
		message.Protocol	= MP_RM_ITEM_GET_SYN;
		message.dwObjectID	= playerIndex;
		message.dwData		= userIndex;
		
		mApplication.Send( serverIndex, message, sizeof( message ) );
	}
}


void CItemDialog::OnBnClickedItemInventoryLogButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( ! mInventoryListCtrl.GetItemCount() )
	{
		return;
	}

	switch( mValueTabCtrl.GetCurSel() )
	{
	// item log
	case 0:
		{
			POSITION	position	= mInventoryListCtrl.GetFirstSelectedItemPosition();
			const DWORD	row			= mInventoryListCtrl.GetNextSelectedItem( position );

			const CString	itemDbIndex	= mInventoryListCtrl.GetItemText( row, 0 );

			CLogDialog* dialog = mApplication.GetLogDialog();

			if( !	itemDbIndex.IsEmpty() &&
					dialog )
			{
				CString textItem;
				textItem.LoadString( IDS_STRING189 );

				CString textLog;
				textLog.LoadString( IDS_STRING10 );

				const CString textName = mInventoryListCtrl.GetItemText( row, 4 );
				
				const CString title( textItem + _T( " " ) + textLog + _T( ": " ) + textName + _T( "(" ) + itemDbIndex + _T( ")" ) );

				CItemLogCommand::Configuration configuration = { 0 };
				configuration.mItemDbIndex	= _ttoi( itemDbIndex );

				dialog->DoModal(
					mServerIndex,
					CItemLogCommand( mApplication, title, configuration ) );
			}

			break;
		}
	// option log
	case 1:
		{
			POSITION	position	= mInventoryListCtrl.GetFirstSelectedItemPosition();
			const DWORD	row			= mInventoryListCtrl.GetNextSelectedItem( position );

			const CString itemDbIndex	= mInventoryListCtrl.GetItemText( row, 0 );
			const CString itemName		= mInventoryListCtrl.GetItemText( row, 4 );

			CLogDialog* dialog = mApplication.GetLogDialog();

			if( !	itemDbIndex.IsEmpty() &&
					dialog )
			{
				CString textItemOption;
				textItemOption.LoadString( IDS_STRING314 );

				CString textLog;
				textLog.LoadString( IDS_STRING10 );

				const CString title( textItemOption + _T( " " ) + textLog + _T( ": " ) + itemName + _T( "(" ) + itemDbIndex + _T( ")" ) );

				dialog->DoModal(
					mServerIndex,
					CItemOptionLogCommand( mApplication, title, _ttoi( itemDbIndex ) ) );
			}

			break;
		}
	// 080320 LUJ, 드롭 옵션 로그
	case 2:
		{
			POSITION	position	= mInventoryListCtrl.GetFirstSelectedItemPosition();
			const DWORD	row			= mInventoryListCtrl.GetNextSelectedItem( position );

			const CString itemDbIndex	= mInventoryListCtrl.GetItemText( row, 0 );
			const CString itemName		= mInventoryListCtrl.GetItemText( row, 4 );

			CLogDialog* dialog = mApplication.GetLogDialog();

			if( !	itemDbIndex.IsEmpty()	&&
					dialog )
			{
				CString textItem;
				textItem.LoadString( IDS_STRING189 );

				CString textDropOption;
				textDropOption.LoadString( IDS_STRING354 );

				CString textLog;
				textLog.LoadString( IDS_STRING10 );

				const CString title( textItem + _T( " " ) + textDropOption + _T( " " ) + textLog + _T( ": " ) + itemName + _T( "(" ) + itemDbIndex + _T( ")" ) );

				dialog->DoModal(
					mServerIndex,
					CItemDropOptionLogCommand( mApplication, title, _ttoi( itemDbIndex ) ) );
			}

			break;
		}
		// 080716 LUJ, 펫 로그 창 표시
	case 3:
		{
			POSITION		position	= mInventoryListCtrl.GetFirstSelectedItemPosition();
			const DWORD		row			= mInventoryListCtrl.GetNextSelectedItem( position );
			const CString	itemDbIndex	= mInventoryListCtrl.GetItemText( row, 0 );

			const PetMap::const_iterator it = mPetMap.find( _ttoi( itemDbIndex ) );

			if( mPetMap.end() == it )
			{
				break;
			}

			CLogDialog* dialog = mApplication.GetLogDialog();

			if( ! dialog )
			{
				break;
			}

			const Pet& pet = it->second;

			CString textPet;
			textPet.LoadString( IDS_STRING280 );
			CString textLog;
			textLog.LoadString( IDS_STRING10 );
			CString	textPetIndex;
			textPetIndex.Format( _T( "%d" ), pet.mIndex );
			const CString textName = mApplication.GetPetName( pet.mKind );

			const CString title( textPet + _T( " " ) + textLog + _T( ": " ) + textName + _T( "(" ) + textPetIndex + _T( ")" ) );

			CPetLogCommand::Configuration configuration = { 0 };
			configuration.mPetIndex = pet.mIndex;

			dialog->DoModal(
				mServerIndex,
				CPetLogCommand( mApplication, title, configuration ) );
			break;
		}
	}
}


void CItemDialog::OnBnClickedItemAddButton()
{
	Item item;
	ZeroMemory( &item, sizeof( item ) );

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	{
		POSTYPE itemInventoryPosition		= USHRT_MAX;
		POSTYPE itemPrivateStoragePosition	= USHRT_MAX;
		POSTYPE itemCashStoragePosition		= USHRT_MAX;

		std::set< DWORD > positionSet;

		for(
			int row = 0;
			row < mInventoryListCtrl.GetItemCount();
			++row )
		{
			const CString index = mInventoryListCtrl.GetItemText( row, 3 );

			positionSet.insert( _ttoi( index ) );
		}

		for(
			POSTYPE i = TP_INVENTORY_START;
			i < TP_INVENTORY_END + TABCELL_INVENTORY_NUM * mExtendedInventorySize;
			++i )
		{
			if( positionSet.end() == positionSet.find( i ) )
			{
				itemInventoryPosition = i;
				break;
			}
		}

		for(
			POSTYPE i = TP_STORAGE_START;
			i < TP_STORAGE_START + TABCELL_STORAGE_NUM * mStorageSize;
			++i )
		{
			if( positionSet.end() == positionSet.find( i ) )
			{
				itemPrivateStoragePosition = i;
				break;
			}
		}

		for(
			POSTYPE i = TP_SHOPITEM_START;
			i < TP_SHOPITEM_END;
			++i )
		{
			if( positionSet.end() == positionSet.find( i ) )
			{
				itemCashStoragePosition = i;
				break;
			}
		}

		if( USHRT_MAX == itemInventoryPosition		&&
			USHRT_MAX == itemPrivateStoragePosition	&&
			USHRT_MAX == itemCashStoragePosition )
		{
			CString textInventoryIsFull;
			textInventoryIsFull.LoadString( IDS_STRING110 );

			MessageBox( textInventoryIsFull, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}

		CItemPositionDialog dialog;

		if( IDCANCEL == dialog.DoModal( itemInventoryPosition, itemPrivateStoragePosition, itemCashStoragePosition ) )
		{
			return;
		}

		if( Item::AreaNone == dialog.GetAreaType() )
		{
			CString textInventoryIsFull;
			textInventoryIsFull.LoadString( IDS_STRING110 );

			MessageBox( textInventoryIsFull, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}

		item.mArea = dialog.GetAreaType();

		switch( dialog.GetAreaType() )
		{
		case Item::AreaInventory:
			{
				item.Position	= itemInventoryPosition;
				break;
			}
		case Item::AreaPrivateStorage:
			{
				item.Position	= itemPrivateStoragePosition;
				break;
			}
		case Item::AreaCashStorage:
			{
				item.Position	= itemCashStoragePosition;
				break;
			}
		}		
	}
	
	{
		CItemAddDialog dialog( mApplication );

		if( IDCANCEL == dialog.DoModal() )
		{
			return;
		}

		item.wIconIdx = dialog.GetSelectedItemIndex();
	}

	const ItemScript& info = mApplication.GetItemScript( item.wIconIdx );

	if( ! info.ItemIdx )
	{
		return;
	}

	// 080611 LUJ, 시간 제한이 걸린 아이템은 생성 시 봉인시킨다
	item.nSealed = ITEM_SEAL_TYPE(info.wSeal | ( 0 < info.dwUseTime ? eITEM_TYPE_SEAL : eITEM_TYPE_SEAL_NORMAL));

	if( info.Stack )
	{
		CQuantityDialog dialog;

		if( IDOK != dialog.DoModal() )
		{
			return;
		}

		item.Durability = min( MAX_YOUNGYAKITEM_DUPNUM, dialog.GetQuantity() );
	}

	{
		CString textTitle;
		textTitle.LoadString( IDS_STRING111 );

		CString text;
		text.Format(
			textTitle,
			item.Position,
			mApplication.GetItemName( item.wIconIdx ) );

		if( IDNO == MessageBox( text, _T( "" ), MB_ICONINFORMATION | MB_YESNO ) )
		{
			return;
		}
	}	

	{
		MSG_RM_ITEM_ADD message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category		= MP_RM_ITEM;
		message.Protocol		= MP_RM_ITEM_ADD_SYN;
		message.mPlayerIndex	= mPlayerIndex;
		message.mUserIndex		= mUserIndex;
		message.mItem			= item;

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}	
}

void CItemDialog::OnBnClickedItemEndTimeButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	const Item& item = GetItem( mSelectedItemDbIndex );

	if( ! item.dwDBIdx )
	{
		return;
	}

	const ITEM_INFO& info = mApplication.GetItemScript( item.wIconIdx );

	if( ! info.ItemIdx )
	{
		return;
	}
	else if( eKIND_REALTIME != info.nTimeKind )
	{
		return;
	}

	CDateDialog dialog;

	if( IDCANCEL == dialog.DoModal( item.nRemainSecond, mApplication.GetDateString( item.nRemainSecond ) ) )
	{
		return;
	}
	else if( dialog.GetSecond() == item.nRemainSecond )
	{
		return;
	}
	
	// 080304 LUJ, 로그
	{
		CString textItemValidTimeUpdate;
		textItemValidTimeUpdate.LoadString( IDS_STRING346 );

		mApplication.Log(
			textItemValidTimeUpdate,
			mPlayerName,
			mPlayerIndex,
			mApplication.GetItemName( item.wIconIdx ),
			item.dwDBIdx,
			dialog.GetSecond() );
	}

	PutItem( item );
	
	{
		MSG_INT2 message;
		message.Category	= MP_RM_ITEM;
		message.Protocol	= MP_RM_ITEM_UPDATE_END_TIME_SYN;
		message.nData1		= item.dwDBIdx;
		message.nData2		= dialog.GetSecond();

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}

void CItemDialog::OnBnClickedItemRemainTimeButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	const Item& item = GetItem( mSelectedItemDbIndex );

	if( ! item.dwDBIdx )
	{
		return;
	}

	const ItemScript& info = mApplication.GetItemScript( item.wIconIdx );

	if( ! info.ItemIdx )
	{
		return;
	}
	else if( eKIND_PLAYTIME != info.nTimeKind )
	{
		return;
	}

	CDateDialog dialog;

	if( IDCANCEL == dialog.DoModal( item.LastCheckTime, mApplication.GetDateString( item.LastCheckTime ) ) )
	{
		return;
	}
	else if( dialog.GetSecond() == item.LastCheckTime )
	{
		return;
	}

	// 080304 LUJ, 로그
	{
		CString textItemEndTimeUpdate;
		textItemEndTimeUpdate.LoadString( IDS_STRING350 );

		mApplication.Log(
			textItemEndTimeUpdate,
			mPlayerName,
			mPlayerIndex,
			mApplication.GetItemName( item.wIconIdx ),
			item.dwDBIdx,
			dialog.GetSecond() );
	}

	PutItem( item );
	
	{
		MSG_INT2 message;
		message.Category	= MP_RM_ITEM;
		message.Protocol	= MP_RM_ITEM_UPDATE_REMAIN_TIME_SYN;
		message.nData1		= item.dwDBIdx;
		message.nData2		= dialog.GetSecond();

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}



// 080320 LUJ, 드롭 옵션 탭 표시 추가
void CItemDialog::OnTcnSelchangeItemTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( pResult )
	{
		*pResult = 0;
	}

	mItemGrid.ShowWindow( SW_HIDE );
	mOptionGrid.ShowWindow( SW_HIDE );
	mDropOptionGrid.ShowWindow( SW_HIDE );
	mPetGrid.ShowWindow( SW_HIDE );

	switch( mValueTabCtrl.GetCurSel() )
	{
	case 0:
		{
			mItemGrid.ShowWindow( SW_SHOW );
			break;
		}
	case 1:
		{
			mOptionGrid.ShowWindow( SW_SHOW );
			break;
		}
	case 2:
		{
			mDropOptionGrid.ShowWindow( SW_SHOW );
			break;
		}
		// 080716 LUJ, 펫 그리드를 표시한다
	case 3:
		{
			if( mPetMap.end() == mPetMap.find( mSelectedItemDbIndex ) )
			{
				mPetSummonButton.ShowWindow( SW_SHOW );
				break;
			}

			mPetSummonButton.ShowWindow( SW_HIDE );
			mPetGrid.ShowWindow( SW_SHOW );
			break;
		}
	}
}


void CItemDialog::OnBnClickedItemStorageUpdateButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	MSG_DWORD2 message;
	{
		ZeroMemory( &message, sizeof( message ) );

		DWORD size;
		{
			CString text;
			mStorageSizeCombo.GetLBText( mStorageSizeCombo.GetCurSel(), text );

			size = min( TAB_STORAGE_NUM, _ttoi( text ) );
		}
		
		DWORD money;
		{
			CString text;
			mStorageMoneyEdit.GetWindowText( text );

			money = _ttoi( text );			
		}		

		message.Category	= MP_RM_ITEM;
		message.Protocol	= MP_RM_ITEM_SET_STORAGE_SYN;
		message.dwObjectID	= mPlayerIndex;
		message.dwData1		= size;
		message.dwData2		= money;

		// 080304 LUJ, 로그
		{
			CString textStorageUpdate;
			textStorageUpdate.LoadString( IDS_STRING351 );

			mApplication.Log(
				textStorageUpdate,
				mPlayerName,
				mPlayerIndex,
				size,
				money );
		}	
	}

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


// 080328 LUJ, 도움말 표시
void CItemDialog::OnBnClickedItemHelpButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	switch( mValueTabCtrl.GetCurSel() )
	{
		// 080328 LUJ, 아이템 정보
	case 0:
		{
			CString textHelp;
			textHelp.LoadString( IDS_STRING355 );
			CString textItem;
			textItem.LoadString( IDS_STRING189 );

			CString textArticle1;
			textArticle1.LoadString( IDS_STRING356 );
			CString textArticle2;
			textArticle2.LoadString( IDS_STRING357 );

			mApplication.Put( _T( "" ) );
			mApplication.Put( _T( "[%s] %s" ), textHelp, textItem );
			mApplication.Put( _T( "" ) );
			mApplication.Put( textArticle1 );
			mApplication.Put( textArticle2 );
			mApplication.Put( _T( "" ) );
			break;
		}
		// 080328 LUJ, 아이템 옵션 정보
	case 1:
		{
			CString textHelp;
			textHelp.LoadString( IDS_STRING355 );
			CString textItemOption;
			textItemOption.LoadString( IDS_STRING314 );

			CString textArticle1;
			textArticle1.LoadString( IDS_STRING358 );

			mApplication.Put( _T( "" ) );
			mApplication.Put( _T( "[%s] %s" ), textHelp, textItemOption );
			mApplication.Put( _T( "" ) );
			mApplication.Put( textArticle1 );
			mApplication.Put( _T( "" ) );
			break;
		}
		// 080328 LUJ, 아이템 드롭 옵션 정보
	case 2:
		{
			CString textHelp;
			textHelp.LoadString( IDS_STRING355 );
			CString textDropOption;
			textDropOption.LoadString( IDS_STRING354 );

			mApplication.Put( _T( "" ) );
			mApplication.Put( _T( "[%s] %s" ), textHelp, textDropOption );
			mApplication.Put( _T( "" ) );

			struct
			{
				void operator() ( CclientApp& application, ITEM_OPTION::Drop::Key key ) const
				{
					application.Put( _T( "\t %d: %s" ), key, application.GetText( key ) );
				}
			}
			PutHelp;

			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusStrength );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusIntelligence );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusDexterity );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusWisdom );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusVitality );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusPhysicalAttack );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusPhysicalDefence );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusMagicalAttack );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusMagicalDefence );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusCriticalRate );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusCriticalDamage );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusAccuracy );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusEvade );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusMoveSpeed );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusLife );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusMana );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusLifeRecovery );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPlusManaRecovery );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentStrength );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentIntelligence );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentDexterity );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentWisdom );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentVitality );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentPhysicalAttack );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentPhysicalDefence );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentMagicalAttack );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentMagicalDefence );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentCriticalRate );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentCriticalDamage );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentAccuracy );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentEvade );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentMoveSpeed );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentLife );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentMana );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentLifeRecovery );
			PutHelp( mApplication, ITEM_OPTION::Drop::KeyPercentManaRecovery );

			mApplication.Put( _T( "" ) );
			break;
		}
		// 080731 LUJ, 펫 도움말 표시
	case 3:
		{
			CString textHelp;
			textHelp.LoadString( IDS_STRING355 );
			CString textPet;
			textPet.LoadString( IDS_STRING280 );
			CString textType;
			textType.LoadString( IDS_STRING4 );
			CString textGrade;
			textGrade.LoadString( IDS_STRING395 );

			mApplication.Put( _T( "" ) );
			mApplication.Put( _T( "[%s] %s" ), textHelp, textPet );

			mApplication.Put( _T( "" ) );
			mApplication.Put( textGrade );

			// 080731 LUJ, 추가될 것을 대비해서 루프를 크게 잡고, 대신 정의되지 않은 문자열이면 종료시킨다
			for( int i = 0; i < 100; ++i )
			{
				const CString text = mApplication.GetPetGradeText( i );

				if( text.IsEmpty() )
				{
					break;
				}

				mApplication.Put( _T( "\t%d: %s" ),
					i,
					text );
			}

			mApplication.Put( _T( "" ) );
			mApplication.Put( textType );
			
			for( int i = 0; i < ePetType_Max; ++i )
			{
				mApplication.Put( _T( "\t%d: %s" ),
					i,
					mApplication.GetText( ePetType( i ) ) );
			}

			{
				mApplication.Put( _T( "" ) );

				CString textCondition;
				textCondition.LoadString( IDS_STRING406 );
				mApplication.Put( textCondition );

				CString text;
				mApplication.GetPetBodyConditionText( text );	
				mApplication.Put( text );
			}

			break;
		}
	}

	{
		CString textGuide;
		textGuide.LoadString( IDS_STRING359 );

		MessageBox( textGuide, _T( "" ) );
	}	
}

// 080611 LUJ, 봉인/봉인 해제를 토글한다
void CItemDialog::OnBnClickedItemDialogSealRadio()
{
	const Item& item = GetItem( mSelectedItemDbIndex );

	if( ! item.dwDBIdx )
	{
		ASSERT( 0 );
		return;
	}

	const TCHAR* itemName = mApplication.GetItemName( item.wIconIdx );

	ITEM_SEAL_TYPE sealType = eITEM_TYPE_SEAL_NORMAL;
	
	// 080611 LUJ, 아이템을 봉인 해제하려고 함
	switch( item.nSealed )		
	{
	case eITEM_TYPE_SEAL:
		{
			CString textFormat;
			textFormat.LoadString( IDS_STRING381 );

			CString text;
			text.Format(
				textFormat,
				itemName );

			if( IDNO == MessageBox( text, _T( "" ), MB_YESNO ) )
			{
				mSealingRadioButton.SetCheck( item.nSealed );
				return;
			}

			{
				CString textUnsealItem;
				textUnsealItem.LoadString( IDS_STRING383 );

				mApplication.Log(
					textUnsealItem,
					itemName,
					item.dwDBIdx );
			}

			// 080731 LUJ, 펫 아이템을 개봉할 경우 소환하도록 한다
			{
				const ItemScript& script = mApplication.GetItemScript( item.wIconIdx );

				if( ITEM_KIND_PET == script.SupplyType &&
					mPetMap.end() == mPetMap.find( item.dwDBIdx ) )
				{
					OnBnClickedItemPetSummonButton();
					return;
				}
			}

			sealType = eITEM_TYPE_UNSEAL;
			break;
		}
		// 080611 LUJ, 아이템을 봉인하려고 함
	case eITEM_TYPE_UNSEAL:
		{
			CString textFormat;
			textFormat.LoadString( IDS_STRING382 );

			CString text;
			text.Format(
				textFormat,
				itemName );

			if( IDNO == MessageBox( text, _T( "" ), MB_YESNO ) )
			{
				mSealingRadioButton.SetCheck( item.nSealed );
				return;
			}

			{
				CString textSealItem;
				textSealItem.LoadString( IDS_STRING384 );

				mApplication.Log(
					textSealItem,
					itemName,
					item.dwDBIdx );
			}

			sealType = eITEM_TYPE_SEAL;
			break;
		}
	default:
		{
			ASSERT( 0 );
			return;
		}
	}

	PutItem( item );

	MSG_RM_ITEM_UPDATE message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_ITEM;
	message.Protocol	= MP_RM_ITEM_SET_SYN;
	message.dwObjectID	= mPlayerIndex;

	{
		ITEMBASE& dest = message.mItem;

		dest.Durability		= item.Durability;
		dest.dwDBIdx		= item.dwDBIdx;
		dest.ItemParam		= item.ItemParam;
		dest.LastCheckTime	= item.LastCheckTime;
		dest.nRemainSecond	= ( eITEM_TYPE_SEAL == sealType ? 0 : item.nRemainSecond );
		dest.nSealed		= sealType;
		dest.Position		= item.Position;
		dest.QuickPosition	= item.QuickPosition;
		dest.wIconIdx		= item.wIconIdx;
	}

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


// 080716 LUJ, 인벤토리 탭이 바뀔 때마다 표시될 리스트 컨트롤을 결정한다
void CItemDialog::OnTcnSelchangeItemDialogInventoryTab(NMHDR *pNMHDR, LRESULT *pResult)
{	
	if( pResult )
	{
		*pResult = 0;
	}	

	mInventoryListCtrl.ShowWindow(
		SW_HIDE);
	mPetInventoryListCtrl.ShowWindow(
		SW_HIDE);
	mHouseInventoryListCtrl.ShowWindow(
		SW_HIDE);

	switch( mInventoryTabCtrl.GetCurSel() )
	{
	case 0:
		{
			mInventoryListCtrl.ShowWindow( SW_SHOW );
			break;
		}
	case 1:
		{
			mHouseInventoryListCtrl.ShowWindow(SW_SHOW);
			break;
		}
	case 2:
		{
			mPetInventoryListCtrl.ShowWindow( SW_SHOW );
			break;
		}
	}
}


void CItemDialog::OnLvnItemchangedItemPetInventoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ¿ⓒ±a¿¡ AAÆ®·N ¾E¸² A³¸®±a AUμa¸| Aß°¡CO´I´U.
	*pResult = 0;

	mSelectedItemDbIndex = _ttoi( mPetInventoryListCtrl.GetItemText( pNMLV->iItem, 0 ) );

	PutData( mSelectedItemDbIndex );
}


// 080731 LUJ, 모든 인벤토리에서  아이템 정보를 새로 고친다
void CItemDialog::PutItem( const Item& item, bool isAdd )
{
	PutItem( item, mInventoryListCtrl, isAdd );
	PutItem( item, mPetInventoryListCtrl, isAdd );
}


// 080731 LUJ, 특정 인벤토리에서 아이템 정보를 새로 고친다
void CItemDialog::PutItem( const Item& item, CListCtrl& listCtrl, bool isAdd )
{
	int row = -1;

	if( isAdd )
	{
		row = listCtrl.GetItemCount();

		CString text;
		text.Format( _T( "%d" ), item.dwDBIdx );
		listCtrl.InsertItem( row, text, 0 );
	}
	else
	{
		// 080731 LUJ, 해당 아이템에 표시된 정보 중 [애완동물] 항목을 빼낸다
		for( int i = 0; i < listCtrl.GetItemCount(); ++i )
		{
			const CString itemDbIndex = listCtrl.GetItemText( i, 0 );

			if( _ttoi( itemDbIndex ) == item.dwDBIdx )
			{
				row = i;
				break;
			}
		}

		if( 0 > row )
		{
			return;
		}
	}

	int			step	= 0;
	CString		text;

	text.Format( _T( "%d" ), item.wIconIdx );
	mInventoryListCtrl.SetItemText( row, ++step, text );

	CString infomation;

	if(	Item::AreaCashStorage == item.mArea )
	{
		CString textBuying;
		textBuying.LoadString( IDS_STRING98 );
		infomation += textBuying;
	}

	listCtrl.SetItemText( row, ++step, mApplication.GetItemPositionType( item.mArea, item.Position ) );

	text.Format( _T( "%d" ), item.Position );
	listCtrl.SetItemText( row, ++step, text );
	listCtrl.SetItemText( row, ++step, mApplication.GetItemName( item.wIconIdx ) );

	const BOOL isDuplicated = mApplication.IsDuplicateItem( item.wIconIdx );

	{
		++step;

		if( isDuplicated )
		{
			text.Format( _T( "%d" ), item.Durability );

			listCtrl.SetItemText( row, step, text );
		}	
	}	

	{
		++step;

		const OptionMap::const_iterator option_it = mOptionMap.find( item.dwDBIdx );

		if( mOptionMap.end() != option_it )
		{
			// 081201 LUJ, 옵션 정보도 엑셀로 추출하기 위해 저장한다
			infomation += MakeText( option_it->second );
		}

		switch( item.nSealed )
		{
		case eITEM_TYPE_SEAL:
			{
				CString textSealing;
				textSealing.LoadString( IDS_STRING100 );
				infomation += _T( "[" ) + textSealing + _T( "]" );
				break;
			}
		case eITEM_TYPE_UNSEAL:
			{
				CString textUnsealing;
				textUnsealing.LoadString( IDS_STRING101 );
				infomation += _T( "[" ) + textUnsealing + _T( "]" );
				break;
			}
		}

		if( item.nRemainSecond || item.LastCheckTime )
		{
			CString textTime;
			textTime.LoadString( IDS_STRING102 );
			infomation += _T( "[" ) + textTime + _T( "]" );
		}

		// 080716 LUJ, 펫 소환이 되는 아이템인 경우 표시한다
		if( mPetMap.end() != mPetMap.find( item.dwDBIdx ) )
		{
			CString textPet;
			textPet.LoadString( IDS_STRING280 );
			infomation += _T( "[" ) + textPet + _T( "]" );
		}

		listCtrl.SetItemText( row, step, infomation );
	}
}


void CItemDialog::OnBnClickedItemPetSummonButton()
{
	const Item& item = GetItem( mSelectedItemDbIndex );

	if( ! item.dwDBIdx )
	{
		return;
	}

	const ItemScript& itemScript = mApplication.GetItemScript( item.wIconIdx );
	
	if( ITEM_KIND_PET != itemScript.SupplyType )
	{
		return;
	}	

	{
		CString textPet;
		textPet.LoadString( IDS_STRING280 );
		CString textMessage;
		textMessage.LoadString( IDS_STRING201 );

		if( IDNO == MessageBox( textMessage, textPet, MB_YESNO | MB_ICONWARNING ) )
		{
			return;
		}
	}

	MSG_RM_PET message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_ITEM;
	message.Protocol	= MP_RM_ITEM_ADD_PET_SYN;
	message.dwObjectID	= mUserIndex;
	message.mSize		= 1;
	{
		const CclientApp::PetSystemScript&	systemScript	= mApplication.GetPetSystemScript();
		const CclientApp::PetStatusScript&	statusScript	= mApplication.GetPetStatusScript( ePetType_Basic, 1 );
		const CclientApp::PetBodyScript&	bodyScript		= mApplication.GetPetBodyScript( ePetType_Basic, 0 );

		MSG_RM_PET::Pet& pet = message.mData[ 0 ];

		pet.mItemDbIndex	= mSelectedItemDbIndex;
		pet.mSkillSlot		= systemScript.mSkillSlot.mMinSize;
		pet.mKind			= itemScript.SupplyValue;
		// 080731 LUJ, CPet::CalcStats()에서 인용. 변경될 시에 바꿔주어야 함
		pet.mMaxHealth		= DWORD( statusScript.mVitality * ( 10 + 0.05f * bodyScript.mHealth ) );
		pet.mMaxMana		= DWORD( statusScript.mWisdom * ( 6 + 0.05f * bodyScript.mMana ) );
	}

	mApplication.Send( mServerIndex, message, message.GetSize() );
}

void CItemDialog::OnBnClickedItemCopyClipboard()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CclientApp::SaveToClipBoard( mInventoryListCtrl, TRUE );
}

void CItemDialog::OnBnClickedItemSaveExcelButton()
{
	CString textInventory;
	textInventory.LoadString( IDS_STRING281 );

	CclientApp::SaveToExcel(
		textInventory + _T( "-" ) + mApplication.GetServerName( mServerIndex ) + _T( ", " ) + mPlayerName,
		textInventory,
		mInventoryListCtrl );
}

// 081201 LUJ, 옵션을 문자열로 출력한다
CString CItemDialog::MakeText( const ITEM_OPTION& option ) const
{
	class
	{
	public:
		CString operator()( ReinforceScript::eType type, DWORD value ) const
		{
			if( ! value )
			{
				return _T( "" );
			}

			CString textTitle;

			switch( type )
			{
			case ReinforceScript::eTypeStrength:
				{
					CString textStrength;
					textStrength.LoadString( IDS_STRING70 );
					textTitle = textStrength;
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					CString textDexterity;
					textDexterity.LoadString( IDS_STRING71 );
					textTitle = textDexterity;
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					CString textHealth;
					textHealth.LoadString( IDS_STRING72 );
					textTitle = textHealth;
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					CString textIntelligence;
					textIntelligence.LoadString( IDS_STRING73 );
					textTitle = textIntelligence;
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					CString textWisdom;
					textWisdom.LoadString( IDS_STRING74 );
					textTitle = textWisdom;
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					CString textLife;
					textLife.LoadString( IDS_STRING75 );
					textTitle = textLife;
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					CString textMana;
					textMana.LoadString( IDS_STRING76 );
					textTitle = textMana;
					break;
				}
			case ReinforceScript::eTypeLifeRecovery:
				{
					CString textLifeRecovery;
					textLifeRecovery.LoadString( IDS_STRING77 );
					textTitle = textLifeRecovery;
					break;
				}
			case ReinforceScript::eTypeManaRecovery:
				{
					CString textManaRecovery;
					textManaRecovery.LoadString( IDS_STRING78 );
					textTitle = textManaRecovery;
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					CString textPhysicalAttack;
					textPhysicalAttack.LoadString( IDS_STRING79 );
					textTitle = textPhysicalAttack;
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					CString textPhysicalDefence;
					textPhysicalDefence.LoadString( IDS_STRING80 );
					textTitle = textPhysicalDefence;
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					CString textMagicalAttack;
					textMagicalAttack.LoadString( IDS_STRING81 );
					textTitle = textMagicalAttack;
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					CString textMagicalDefence;
					textMagicalDefence.LoadString( IDS_STRING82 );
					textTitle = textMagicalDefence;
					break;
				}
			case ReinforceScript::eTypeMoveSpeed:
				{
					CString textMoveSpeed;
					textMoveSpeed.LoadString( IDS_STRING83 );
					textTitle = textMoveSpeed;
					break;
				}
			case ReinforceScript::eTypeEvade:
				{
					CString textEvade;
					textEvade.LoadString( IDS_STRING84 );
					textTitle = textEvade;
					break;
				}
			case ReinforceScript::eTypeAccuracy:
				{
					CString textAccuracy;
					textAccuracy.LoadString( IDS_STRING85 );
					textTitle = textAccuracy;
					break;
				}
			case ReinforceScript::eTypeCriticalRate:
				{
					CString textCriticalRate;
					textCriticalRate.LoadString( IDS_STRING86 );
					textTitle = textCriticalRate;
					break;
				}
			case ReinforceScript::eTypeCriticalDamage:
				{
					CString textCriticalValue;
					textCriticalValue.LoadString( IDS_STRING87 );
					textTitle = textCriticalValue;
					break;
				}
			default:
				{
					textTitle = _T( "?" );
					break;
				}
			}

            CString text;
			text.Format(
				_T( "%s %+d " ),
				textTitle,
				value );
			return text;
		}

		CString operator()( MixSetting::eAbility ability, DWORD value ) const
		{
			if( ! value )
			{
				return _T( "" );
			}

			CString textTitle;

			switch( ability )
			{
			case MixSetting::eAbilityStrength:
				{
					CString textStrength;
					textStrength.LoadString( IDS_STRING70 );
					textTitle = textStrength;
					break;
				}
			case MixSetting::eAbilityDexterity:
				{
					CString textDexterity;
					textDexterity.LoadString( IDS_STRING71 );
					textTitle = textDexterity;
					break;
				}
			case MixSetting::eAbilityIntelligence:
				{
					CString textHealth;
					textHealth.LoadString( IDS_STRING72 );
					textTitle = textHealth;
					break;
				}
			case MixSetting::eAbilityVitality:
				{
					CString textIntelligence;
					textIntelligence.LoadString( IDS_STRING73 );
					textTitle = textIntelligence;
					break;
				}
			case MixSetting::eAbilityWisdom:
				{
					CString textWisdom;
					textWisdom.LoadString( IDS_STRING74 );
					textTitle = textWisdom;
					break;
				}
			default:
				{
					textTitle = _T( "?" );
					break;
				}
			}

			CString text;
			text.Format(
				_T( "%s %+d " ),
				textTitle,
				value );
			return text;
		}

		CString operator()( const CclientApp& application, ITEM_OPTION::Drop::Key key, float value ) const
		{
			if(		ITEM_OPTION::Drop::KeyNone == key ||
				!	value )
			{
				return _T( "" );
			}

			CString text;
			text.Format(
				_T( "%s %+0.1f " ),
				application.GetText( key ),
				value );
			return text;
		}
	}
	GetOptionText;

	const ITEM_OPTION emptyOption = { 0 };

	CString text;

	// 081201 LUJ, 강화 옵션 문자열
	{
		const BOOL reinforced = ( memcmp( &( emptyOption.mReinforce ), &( option.mReinforce ), sizeof( emptyOption.mReinforce ) ) );

		if( reinforced )
		{
			CString textTitle;
			textTitle.LoadString( IDS_STRING89 );
			text += CString( _T( "[" ) ) + textTitle + _T( "] " );

			const ITEM_OPTION::Reinforce& reinforce	= option.mReinforce;

			text += GetOptionText( ReinforceScript::eTypeStrength,			reinforce.mStrength	);
			text += GetOptionText( ReinforceScript::eTypeDexterity,			reinforce.mDexterity );
			text += GetOptionText( ReinforceScript::eTypeVitality,			reinforce.mVitality );
			text += GetOptionText( ReinforceScript::eTypeIntelligence,		reinforce.mIntelligence	);
			text += GetOptionText( ReinforceScript::eTypeWisdom,			reinforce.mWisdom );
			text += GetOptionText( ReinforceScript::eTypeLife,				reinforce.mLife	);
			text += GetOptionText( ReinforceScript::eTypeMana,				reinforce.mMana	);
			text += GetOptionText( ReinforceScript::eTypeLifeRecovery,		reinforce.mLifeRecovery	);
			text += GetOptionText( ReinforceScript::eTypeManaRecovery,		reinforce.mManaRecovery	);
			text += GetOptionText( ReinforceScript::eTypePhysicAttack,		reinforce.mPhysicAttack	);
			text += GetOptionText( ReinforceScript::eTypePhysicDefence,		reinforce.mPhysicDefence );
			text += GetOptionText( ReinforceScript::eTypeMagicAttack,		reinforce.mMagicAttack );
			text += GetOptionText( ReinforceScript::eTypeMagicDefence,		reinforce.mMagicDefence );
			text += GetOptionText( ReinforceScript::eTypeMoveSpeed,			reinforce.mMoveSpeed );
			text += GetOptionText( ReinforceScript::eTypeEvade,				reinforce.mEvade );
			text += GetOptionText( ReinforceScript::eTypeCriticalDamage,	reinforce.mCriticalDamage );
			text += GetOptionText( ReinforceScript::eTypeCriticalRate,		reinforce.mCriticalRate	);
			text += CString( reinforce.mMadeBy ) + _T( "  " );
		}
	}

	// 081201 LUJ, 조합 옵션 문자열
	{
		const BOOL mixed = ( memcmp( &( emptyOption.mMix ), &( option.mMix ), sizeof( emptyOption.mMix ) ) );

		if( mixed )
		{
			CString textTitle;
			textTitle.LoadString( IDS_STRING90 );
			text += CString( _T( "[" ) ) + textTitle + _T( "] " );

			const ITEM_OPTION::Mix& mix = option.mMix;

			text += GetOptionText( MixSetting::eAbilityStrength,		mix.mStrength );
			text += GetOptionText( MixSetting::eAbilityDexterity,		mix.mDexterity );
			text += GetOptionText( MixSetting::eAbilityIntelligence,	mix.mIntelligence );
			text += GetOptionText( MixSetting::eAbilityVitality,		mix.mVitality );
			text += GetOptionText( MixSetting::eAbilityWisdom,			mix.mWisdom );
			text += CString( mix.mMadeBy ) + _T( "  " );
		}
	}

	// 081201 LUJ, 인챈트 옵션 문자열
	{
		const BOOL enchanted = ( memcmp( &( emptyOption.mEnchant ),	&( option.mEnchant ), sizeof( emptyOption.mEnchant ) ) );

		if( enchanted )
		{
			CString textTitle;
			textTitle.LoadString( IDS_STRING91 );
			text += CString( _T( "[" ) ) + textTitle + _T( "] " );

			const ITEM_OPTION::Enchant& enchant = option.mEnchant;

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );

			CString textTemporary;
			textTemporary.Format(
				_T( "%d(%d %s)" ),
				enchant.mIndex,
				enchant.mLevel,
				textLevel );

			text += textTemporary +_T( " " ) + CString( enchant.mMadeBy ) + _T( "  " );
		}
	}	

	// 081201 LUJ, 드롭 옵션 문자열
	{
		const BOOL dropped	= ( memcmp( &( emptyOption.mDrop ),	&( option.mDrop ), sizeof( emptyOption.mDrop ) ) );

		if( dropped )
		{
			CString textTitle;
			textTitle.LoadString( IDS_STRING354 );
			text += CString( _T( "[" ) ) + textTitle + _T( "] " );

			const ITEM_OPTION::Drop& drop = option.mDrop;

            const size_t dropOptionSize = ( sizeof( drop.mValue ) / sizeof( *drop.mValue ) );

			for(	size_t i = 0;
					dropOptionSize > i;
					++i )
			{
				const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

				text += GetOptionText( mApplication, value.mKey, value.mValue );
			}
		}
	}

	return text;
}
#include "stdafx.h"
#include "ItemManager.h"
#include "ItemShow.h"
#include "cQuickItem.h"
#include "DealDialog.h"
#include "ExchangeDialog.h"
#include "MixDialog.h"
#include "ProgressDialog.h"
#include "GameIn.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"
#include "Interface/cScriptManager.h"
#include "cResourceManager.h"
#include "cDivideBox.h"
#include "ObjectManager.h"
#include "AppearanceManager.h"
#include "QuickManager.h"
#include "ExchangeItem.h"
#include "ChatManager.h"
#include "mhFile.h"
#include "GuildWarehouseDialog.h"
#include "InventoryExDialog.h"
#include "StorageDialog.h"
#include "EnchantDialog.h"
#include "ReinforceDlg.h"
#include "ApplyOptionDialog.h"
#include "MHMap.h"
#include "DissolveDialog.h"
#include "QuickSlotDlg.h"
#include "cSkillTreeManager.h"
#include "../[cc]skill/client/manager/Skillmanager.h"
#include "ItemShopDialog.h"
#include "ComposeDialog.h"
#include "NpcImageDlg.h"
#include "NpcScriptDialog.h"
#include "../hseos/Farm/SHFarmManageDlg.h"
#include "TutorialManager.h"
#include "cMapMoveDialog.h"
#include "../CharacterDialog.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/ActiveSkillInfo.h"
#include "cMsgBox.h"
#include "../ConductInfo.h"
#include "ConductManager.h"
#include "cChangeNameDialog.h"
#include "../[CC]Header/FilteringTable.h"
#include "interface/cEditBox.h"
#include "Interface/cFont.h"
#include "../hseos/Farm/FarmAnimalDlg.h"
#include "BuyItem.h"
#include "ShoutDialog.h"
#include "FishingDialog.h"
#include "FishingManager.h"
#include "CommonCalcFunc.h"
#include "../hseos/Monstermeter/SHMonstermeterDlg.h"
#include "ItemPopupDlg.h"
#include "petweareddialog.h"
#include "petmanager.h"
#include "./GuildManager.h"
#include "./ObjectStateManager.h"
#include "./SiegeWarfareMgr.h"
#include "cCookMgr.h"
#include "cHousingMgr.h"
#include "cHousingStoredIcon.h"
#include "MoveManager.h"
#include "ChangeClassDlg.h"
#include "Questquickviewdialog.h"
#include "QuestDialog.h"
#include "QuestManager.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "ConsignmentDlg.h"
#include "QuickDlg.h"
#include "interface/cListDialog.h"

struct TextUnit
{
	TCHAR mText[MAX_PATH];
	DWORD mColor;
	BOOL mIsAttach;
};

CItemManager* CItemManager::GetInstance()
{
	static CItemManager instance;

	return &instance;
}

CItemManager::CItemManager()
{
	m_ItemHash.Initialize(128);
	m_ItemInfoList.Initialize(MAX_ITEM_NUM);
	mEffectList.Initialize( 100 );
	mEventInfoTable.Initialize( 3 );

	m_IconIndexCreator.Init(MAX_ITEMICON_NUM, IG_ITEM_STARTINDEX);
	m_bAddPrice = FALSE;
	//m_RareItemInfoTable.Initialize(64);
	memset(&m_TempDeleteItem, 0, sizeof(ITEMBASE));
	m_nSealItemDelPos = -1;

	m_htMixResultItemInfo.Initialize(MAX_ITEM_NUM);
	m_ENchantNotifyCheckTime = 0;

	LoadMixScript();
	LoadDissolveScript();
	LoadCoolTimeScript();
	LoadChangeItem();
	LoadBodyChangeScript();
	LoadItemList();
	LoadItemToolTipList();
	LoadItemEffectList();
	LoadGiftEvent();
}

CItemManager::~CItemManager()
{
	//m_GroundHash.R
	Release();
	m_IconIndexCreator.Release();

	ITEM_INFO * pInfo = NULL;
	m_ItemInfoList.SetPositionHead();
	while((pInfo = m_ItemInfoList.GetData()) != NULL)
		delete pInfo;
	m_ItemInfoList.RemoveAll();
	
	m_ToolTipTextMap.clear();

	EnchantEffect * pEffect = NULL;
	mEffectList.SetPositionHead();
	while((pEffect = mEffectList.GetData()) != NULL)
		delete pEffect;
	mEffectList.RemoveAll();

	EVENTINFO* pEvent = NULL;
	mEventInfoTable.SetPositionHead();
	while((pEvent = mEventInfoTable.GetData()) != NULL)
	{
		delete[] pEvent->Msg;
		delete pEvent;
	}
	mEventInfoTable.RemoveAll();

	m_GiftEventTime = 0;

	m_htMixResultItemInfo.SetPositionHead();
	while(stMixResultItemInfo* pMixInfo = m_htMixResultItemInfo.GetData())
	{
		SAFE_DELETE( pMixInfo );
	}
	m_htMixResultItemInfo.RemoveAll();
}


cImage * CItemManager::GetIconImage(DWORD ItemIdx, cImage * pImageGet)
{
	// 080916 LUJ, 널 포인터 체크 추가
	const ITEM_INFO* itemInfo = GetItemInfo( ItemIdx );
	SCRIPTMGR->GetImage( itemInfo ? itemInfo->Image2DNum : 0, pImageGet, PFT_ITEMPATH );
	return pImageGet;
}

CItem* CItemManager::MakeNewItem( const ITEMBASE* pBaseInfo, const char* strKind)
{
	ITEM_INFO* pItemInfo = GetItemInfo(pBaseInfo->wIconIdx);
	
	if(pItemInfo == NULL)
	{
		char temp[256];
		sprintf(temp,"NoItemInfo %d,  %d,  %s",gHeroID,pBaseInfo->wIconIdx,strKind);
		ASSERTMSG(pItemInfo, temp);
		return NULL;
	}
	
	if(pBaseInfo->dwDBIdx == 0)
		return NULL;

	CItem* pItem = NULL;
	pItem = GetItem(pBaseInfo->dwDBIdx);

	if(!pItem)
	{
		pItem = new CItem(pBaseInfo);
		m_ItemHash.Add(pItem,pItem->GetDBIdx());						// HASH¿¡ ¾ÆAIAU ³O±a
		cImage tmpIconImage;
		pItem->Init(0,0,DEFAULT_ICONSIZE,DEFAULT_ICONSIZE,
					GetIconImage(pItem->GetItemIdx(), &tmpIconImage),
					m_IconIndexCreator.GenerateIndex());
		pItem->SetIconType(eIconType_AllItem);
		pItem->SetData(pItem->GetItemIdx());
		pItem->SetLinkPosition( 0 );

		WINDOWMGR->AddWindow(pItem);
		RefreshItem( pItem );
	}
	else
	{
		// 080923 LUJ, 창고를 두번 이상 열때 기존 아이콘을 재활용한다. 이때 잔여시간이 갱신되지 않으면
		//				쿨타임이 처리되지 않은 것처럼 보인다. 시간을 갱신해주기 위해 정보를 다시 설정한다
		pItem->SetItemBaseInfo( *pBaseInfo );
	}

	return pItem;
}


void CItemManager::AddToolTip( cWindow* window, DWORD toolTipIndex, DWORD color )
{
	if( ! window )
	{
		return;
	}

	const std::vector< std::string >& cutToolTipList = GetToolTipMsg( toolTipIndex );
	for( std::vector< std::string >::const_iterator cutToolTipListIter = cutToolTipList.begin() ; cutToolTipListIter != cutToolTipList.end() ; ++cutToolTipListIter )
	{
		const std::string& strTooltip = *cutToolTipListIter;
		window->AddToolTipLine( strTooltip.c_str(), color );
	}
}


void CItemManager::AddToolTip( CVirtualItem* icon )
{
	if( ! icon )
	{
		return;
	}

	CBaseItem* linkedItem = icon->GetLinkItem();

	if( ! linkedItem )
	{
		return;
	}

	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );

		item.wIconIdx	= linkedItem->GetItemIdx();
		item.dwDBIdx	= linkedItem->GetDBIdx();
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );
}


void CItemManager::AddToolTip( cIcon* icon )
{
	if( ! icon )
	{
		return;
	}

	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );

		item.wIconIdx	= icon->GetData();
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );
}


void CItemManager::AddToolTip( CItemShow* icon )
{
	if( ! icon )
	{
		return;
	}

	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );

		item.wIconIdx	= icon->GetItemIdx();
		item.dwDBIdx	= icon->GetDBIdx();
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );

}


void CItemManager::AddToolTip( CItem* icon )
{
	if( ! icon )
	{
		return;
	}

	const ITEMBASE& item = icon->GetItemBaseInfo();

	BOOL bFirst = TRUE ;

	if( eITEM_TYPE_UNSEAL == item.nSealed )
	{
		bFirst = FALSE ;
		icon->SetLastCheckTime( gCurTime );
	}
		
	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );

	// 071115 LYW --- ItemManager : 남은 사용시간 툴팁 추가.
	AddRemainTimeToolTip( *icon, item, bFirst );

	if( icon->IsNoDefenceTooltip() )
	{
		icon->AddToolTipLine( "" );
		icon->AddToolTipLine( CHATMGR->GetChatMsg( 1195 ), ICONCLR_DISABLE );
		icon->AddToolTipLine( CHATMGR->GetChatMsg( 1196 ), ICONCLR_DISABLE );
	}
}


void CItemManager::AddToolTip( cQuickItem* icon )
{
	if( !	icon  )
	{
		return;
	}

	if(icon->GetSlotInfo().kind == QuickKind_Skill)
	{
		return ;
	}
	else if(icon->GetSlotInfo().kind == QuickKind_Conduct )
	{
		cConductInfo* pConductInfo = CONDUCTMGR->GetConductInfo(icon->GetSlotInfo().idx ) ;

		if( !pConductInfo ) return ;

		cImage image;
		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );

		icon->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &image, TTCLR_ITEM_CANEQUIP );
		icon->AddToolTipLine( pConductInfo->GetConductName(), RGB_HALF( 255, 255, 255 ) );
		return ;
	}

	ITEMBASE item;
	ZeroMemory( &item, sizeof( item ) );
	item.wIconIdx = icon->GetSlotInfo().idx;

	if(CItem* const linkedItem = GetItem(icon->GetSlotInfo().dbIdx))
	{
		item.dwDBIdx = linkedItem->GetDBIdx();
		item.nSealed = ITEM_SEAL_TYPE(linkedItem->IsSeal());
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddUseableTimeToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );
}


void CItemManager::AddToolTip( CExchangeItem* icon )
{
	if( ! icon  )
	{
		return;
	}

	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );

		if( icon->GetLinkItem() )
		{
			item = icon->GetLinkItem()->GetItemBaseInfo();
		}
		else
		{
			item.wIconIdx	= icon->GetData();
			item.dwDBIdx	= icon->GetDBId();
			item.nSealed	= ITEM_SEAL_TYPE(icon->IsSeal());
		}		
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip(* icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );

	if( item.nSealed )
	{
		AddUseableTimeToolTip( *icon, item ) ;
	}
}


void CItemManager::AddTimeToolTip( cIcon* icon, const ITEM_OPTION& option, BOOL bFirst)
{
	if( ! icon )
	{
		ASSERT( 0 );
		return;
	}
	
	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );

		if( WT_ITEM == icon->GetType() )
		{
			item = ( ( CItem* )icon )->GetItemBaseInfo();
		}
		else
		{
			item.wIconIdx	= icon->GetData();
		}		
	}

	const ITEM_INFO* info = GetItemInfo( icon->GetData() );

	if( ! info )
	{
		return;
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );			// 090430 pdy 하우징아이템 툴팁 추가 
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip( *icon, item );
	AddFunctionToolTip( *icon, item );					
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );			// 090430 pdy 하우징아이템 툴팁 추가 

	if( icon->GetType() == WT_HOUSING_STORED_ICON )
	{
		// 090430 pdy 하우징아이템 툴팁 추가 
		AddFurnitureStateToolTip(*icon, item);
	}

	// 071115 LYW --- ItemManager : 남은 시간 출력.
	// 090430 pdy WT_HOUSING_STORED_ICON 추가 
	if( icon->GetType() == WT_ITEM ||
		icon->GetType() == WT_HOUSING_STORED_ICON )
	{
		AddRemainTimeToolTip( *icon, item, bFirst) ;
	}	
}

void CItemManager::AddToolTip(CBuyItem* icon)
{
	if( ! icon  )
	{
		return;
	}

	ITEMBASE item;
	{
		ZeroMemory( &item, sizeof( item ) );
		item.wIconIdx	= icon->GetItemIdx();
		item.nSealed	= ITEM_SEAL_TYPE(icon->GetSeal());
	}

	AddNametoolTip( *icon, item );
	AddFurnitureFunctionToolTip( *icon, item );
	AddPetToolTip( *icon, item );
	AddOptionToolTip( *icon, item );
	AddDescriptionToolTip( *icon, item );
	AddSetToolTip(* icon, item );
	AddFunctionToolTip( *icon, item );
	AddMakerToolTip( *icon, item );
	AddFurnitureOptionToolTip( *icon, item );

	if( item.nSealed )
	{
		AddUseableTimeToolTip( *icon, item ) ;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
// 071126 LYW --- ItemManager : 남은 사용시간 툴팁 추가.
//---------------------------------------------------------------------------------------------------------------------------
void CItemManager::AddUseableTimeToolTip(cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* pItemInfo = GetItemInfo( item.wIconIdx ) ;

	if( !pItemInfo ||
		pItemInfo->wSeal != eITEM_TYPE_SEAL ||
		pItemInfo->SupplyType == ITEM_KIND_PET )
	{
		return ;
	}

	const DWORD dwRemainTime = pItemInfo->dwUseTime ;

	const int nDayValue		= 86400 ;
	const int nHourValue	= 3600 ;

	int nDay = dwRemainTime/(nDayValue) ;

	int nHour = ( dwRemainTime - (nDay * nDayValue) ) / nHourValue ;
	
	int nMinute = ( (dwRemainTime - (nDay * nDayValue)) - (nHour * nHourValue) ) / 60;

	char tempBuf[512] = {0, } ;

	icon.AddToolTipLine(CHATMGR->GetChatMsg( 1184 ), RGB_HALF( 143, 205, 217 )) ;

	if( nDay <= 0 )
	{
		if( nHour <= 0 )
		{
			if( nMinute <= 0 ) nMinute = 1 ;
			sprintf(tempBuf, CHATMGR->GetChatMsg( 1185 ), nMinute) ;
		}
		else
		{
			sprintf(tempBuf, CHATMGR->GetChatMsg( 1186 ), nHour, nMinute) ;
		}
	}
	else
	{
		sprintf(tempBuf, CHATMGR->GetChatMsg( 1187 ), nDay, nHour, nMinute) ;
	}

	icon.AddToolTipLine(tempBuf, RGB_HALF( 143, 205, 217 )) ;
}


//---------------------------------------------------------------------------------------------------------------------------
// 071115 LYW --- ItemManager : 남은 사용시간 툴팁 추가.
//---------------------------------------------------------------------------------------------------------------------------
void CItemManager::AddRemainTimeToolTip( cIcon& icon, const ITEMBASE& item, BOOL bFirst)
{
	const ITEM_INFO* pItemInfo = GetItemInfo( item.wIconIdx );

	if( !pItemInfo )
	{
		return;
	}

	if( pItemInfo->SupplyType == ITEM_KIND_PET )
	{
		return;
	}

	DWORD dwRemainTime = 0;
	char tempBuf[512] = { 0 } ;

	switch( item.nSealed )
	{		
	case eITEM_TYPE_UNSEAL:
		{
			icon.AddToolTipLine( "" );

			if( bFirst )
			{
				dwRemainTime = pItemInfo->dwUseTime ;
			}
			else
			{
				dwRemainTime = item.nRemainSecond ;
			}

			int nDayValue = 86400 ;
			int nHourValue = 3600 ;

			int nDay = dwRemainTime/(nDayValue) ;
			int nHour = ( dwRemainTime - (nDay * nDayValue) ) / nHourValue ;		
			int nMinuteValue = (dwRemainTime - (nDay * nDayValue)) - (nHour * nHourValue) ;		
			int nMinute = nMinuteValue / 60;
			int nSecond = nMinuteValue % 60;

			if( nSecond > 0 )
			{
				++nMinute ;
			}

			icon.AddToolTipLine(CHATMGR->GetChatMsg( 1179 ), RGB_HALF( 255, 255, 0 )) ;

			if( nDay <= 0 )
			{
				if( nHour <= 0 )
				{
					if( nMinute <= 0 ) 
					{
						sprintf(tempBuf, CHATMGR->GetChatMsg( 1181 ), 1) ;
					}
					else
					{
						sprintf(tempBuf, CHATMGR->GetChatMsg( 1181 ), nMinute) ;
					}

				}
				else
				{
					sprintf(tempBuf, CHATMGR->GetChatMsg( 1182 ), nHour, nMinute) ;
				}
			}
			else
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1183 ), nDay, nHour, nMinute) ;
			}

			icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			break;
		}
	case eITEM_TYPE_SEAL:
		{
			icon.AddToolTipLine( "" );

			dwRemainTime = pItemInfo->dwUseTime ;

			int nDayValue = 86400 ;
			int nHourValue = 3600 ;

			int nDay = dwRemainTime/(nDayValue) ;

			int nHour = ( dwRemainTime - (nDay * nDayValue) ) / nHourValue ;

			int nMinute = ( (dwRemainTime - (nDay * nDayValue)) - (nHour * nHourValue) ) / 60;

			icon.AddToolTipLine(CHATMGR->GetChatMsg( 1184 ), RGB_HALF( 143, 205, 217 )) ;

			if( nDay <= 0 )
			{
				if( nHour <= 0 )
				{
					if( nMinute <= 0 ) nMinute = 1 ;
					sprintf(tempBuf, CHATMGR->GetChatMsg( 1185 ), nMinute) ;
				}
				else
				{
					sprintf(tempBuf, CHATMGR->GetChatMsg( 1186 ), nHour, nMinute) ;
				}
			}
			else
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1187 ), nDay, nHour, nMinute) ;
			}

			icon.AddToolTipLine(tempBuf, RGB_HALF( 143, 205, 217 )) ;
			break;
		}
	}

	//090430 pdy 하우징 아이템 시간툴팁 추가
	if( pItemInfo->SupplyType == ITEM_KIND_FURNITURE )
	{
		if( pItemInfo->dwUseTime >= 1500000000 )		//090604 pdy 하우징 아이템 시간설정이 15억이상이면 띄우지 않는다. (기획팀 요청)
			return;

		//가구아이템일경우 봉인이 아니지만 시간툴팁을 띄워줘야한다.
		BOOL	bInstalled = FALSE; 
		dwRemainTime = pItemInfo->dwUseTime ;			//디폴트로 아이템의 기본사용기간을 넣는다

		if( icon.GetType() == WT_HOUSING_STORED_ICON )
		{
			//보관 아이콘일 상태 
			cHousingStoredIcon* pHS_StoredIcon = (cHousingStoredIcon*) &icon;
			stFurniture* pstFurniture = pHS_StoredIcon->GetLinkFurniture();
			if(! pstFurniture )
				return;

			if( pstFurniture->wState != eHOUSEFURNITURE_STATE_KEEP )
			{
				//보관 아이콘일때 한번이라도 설치가 되었으면 가구의 남은시간을 가져오자 
				dwRemainTime = pstFurniture->dwRemainTime;
				bInstalled = TRUE;
			}
		}

		//icon.AddToolTipLine( "" );
		int nDayValue = 86400 ;
		int nHourValue = 3600 ;
		int nDay = dwRemainTime/(nDayValue) ;
		int nHour = ( dwRemainTime - (nDay * nDayValue) ) / nHourValue ;		
		int nMinuteValue = (dwRemainTime - (nDay * nDayValue)) - (nHour * nHourValue) ;		
		int nMinute = nMinuteValue / 60;

		if( bInstalled == TRUE )		 
		{
			icon.AddToolTipLine(CHATMGR->GetChatMsg( 1179 ), RGB_HALF( 255, 255, 0 )) ;
			//아이템이 한번이라도 설치된 상태면 [남은시간] 출력
			if( nDay > 0)
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1183 ), nDay, nHour, nMinute) ;
			}
			else if( nHour > 0 )
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1182 ), nHour, nMinute) ;
			}
			else if( nMinute > 0 ) 
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1181 ), nMinute) ;
			}
			else
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1181 ), 1) ;
			}
			icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
		}
		else 
		{
			icon.AddToolTipLine(CHATMGR->GetChatMsg( 1184 ), RGB_HALF( 143, 205, 217 )) ;
			//아이템이 한번이라도 설치되지않은 상태면 [설치후시간] 출력
			if( nDay > 0)
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1843 ), nDay, nHour, nMinute) ;
			}
			else if( nHour > 0 )
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1842), nHour, nMinute) ;
			}
			else if( nMinute > 0 )
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1843 ), nMinute) ;
			}
			else 
			{
				sprintf(tempBuf, CHATMGR->GetChatMsg( 1843 ), 1) ;
			}
			icon.AddToolTipLine(tempBuf, RGB_HALF( 143, 205, 217 )) ;
		}
	}
}


void CItemManager::AddMakerToolTip( cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	// 080226 LUJ, 장착 불가능한 아이템은 제작자 정보를 표시하지 않는다
	if( !	info ||
			eEquipType_None == info->EquipType )
	{
		return;
	}

	const ITEM_OPTION& option = GetOption( item.dwDBIdx );

	const char* reinforceMaker	= option.mReinforce.mMadeBy;
	const char* mixMaker		= option.mMix.mMadeBy;
	const char* enchantMaker	= option.mEnchant.mMadeBy;

	if( ! *reinforceMaker	&&
		! *mixMaker			&&
		! *enchantMaker )
	{
		return;
	}

	char text[ MAX_PATH ];

	icon.AddToolTipLine( "" );

	if( *reinforceMaker )
	{
		sprintf( text, CHATMGR->GetChatMsg( 1170 ), reinforceMaker );
		icon.AddToolTipLine( text, RGB_HALF( 255, 255, 0 ) );
	}

	if( *mixMaker )
	{
		sprintf( text, CHATMGR->GetChatMsg( 1169 ), mixMaker );
		icon.AddToolTipLine( text, RGB_HALF( 255, 255, 0 ) );
	}

	if( *enchantMaker )
	{
		sprintf( text, CHATMGR->GetChatMsg( 1171 ), enchantMaker );
		icon.AddToolTipLine( text, RGB_HALF( 255, 255, 0 ) );
	}
}

void CItemManager::AddFunctionToolTip( cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	// 091012 pdy 기획팀 요청으로 FunctionToolTip 툴팁 방식 변경 
	// 가능 = 노랑색 , 불가능 = 빨간색 

	// 강화 판매 교환 보관 
	icon.AddToolTipLine( "" );

	// 판매 
	if( info->Sell )
	{
		icon.AddToolTipPartialLine( TRUE, CHATMGR->GetChatMsg( 1958 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( TRUE, CHATMGR->GetChatMsg( 1958 ) , RGB_HALF( 255,0,0) );
	}

	icon.AddToolTipPartialLine( FALSE ," ");

	// 교환
	if( info->Trade )
	{
		if( item.nSealed != eITEM_TYPE_SEAL_NORMAL && item.nSealed !=  eITEM_TYPE_SEAL )
		{
			// 봉인 아이템인 경우 봉인이 해제 되어있으면 교환 불가 
			icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1957 ) , RGB_HALF( 255,0,0) );
		}
		else
		{
			icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1957 ) , RGB_HALF( 255,255,0) );
		}
	}
	else
	{
		// 교환 불가
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1957 ) , RGB_HALF( 255,0,0) );
	}

	icon.AddToolTipPartialLine( FALSE ," ");

	// 창고보관 
	if( info->Deposit )
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1959 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1959 ) , RGB_HALF( 255,0,0) );
	}

	icon.AddToolTipPartialLine( FALSE ," ");

	// 강화 
	if( info->Improvement )
	{
		icon.AddToolTipPartialLine( FALSE ,  CHATMGR->GetChatMsg( 1965 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( FALSE ,  CHATMGR->GetChatMsg( 1965 ) , RGB_HALF( 255,0,0) );
	}


	// 조합 분해 인챈트

	// 조합
	if( GetMixScript( info->ItemIdx ) )
	{
		icon.AddToolTipPartialLine( TRUE , CHATMGR->GetChatMsg( 1966 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( TRUE , CHATMGR->GetChatMsg( 1966 ) , RGB_HALF( 255,0,0) );
	}

	icon.AddToolTipPartialLine( FALSE ," ");

	// 분해
	if( GetDissolveScript( info->ItemIdx ) )
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1967 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1967 ) , RGB_HALF( 255,0,0) );
	}

	icon.AddToolTipPartialLine( FALSE ," ");

	// 인챈트
	if( info->IsEnchant )
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1968 ) , RGB_HALF( 255,255,0) );
	}
	else
	{
		icon.AddToolTipPartialLine( FALSE , CHATMGR->GetChatMsg( 1968 ) , RGB_HALF( 255,0,0) );
	}
}


void CItemManager::AddNametoolTip( cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	// 초기화
	{
		cImage image;
		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );

		icon.SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &image, TTCLR_ITEM_CANEQUIP );
	}	

	char line[ MAX_PATH ] = { 0 };

	// 이름 표시
	{
		char isSealing[ MAX_PATH ] = { 0 };

		DWORD color = GetItemNameColor( *info );

		if( //eIconType_AllItem	== icon.GetIconType() &&
			eITEM_TYPE_SEAL		== item.nSealed )
		{
			sprintf( isSealing, "[%s] ", CHATMGR->GetChatMsg( 1393 ) );

			color = RGB_HALF( 255, 0, 0 );
		}

		char isEnchant[ MAX_PATH ] = { 0 };

		// 080601 LUJ, 중첩되지 않는 아이템만 인챈트 정보를 표시한다
		if( ! info->Stack )
		{
			const ITEM_OPTION& option = GetOption( item.dwDBIdx );

			if( option.mEnchant.mLevel )
			{
				sprintf( isEnchant, "+%d ", option.mEnchant.mLevel );
			}
		}

		sprintf( line, "%s%s%s", isSealing, isEnchant, info->ItemName );

		icon.AddToolTipLine( line, color );
	}	

	// 장비 종류
	switch( eEquipType( info->EquipType ) )
	{
	case eEquipType_Weapon:
		{
			switch( eWeaponType( info->WeaponType ) )
			{
			case eWeaponType_Sword:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 660 ) );
					break;
				}				
			case eWeaponType_Mace:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 662 ) );
					break;
				}				
			case eWeaponType_Axe:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 661 ) );
					break;
				}
			case eWeaponType_Dagger:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 663 ) );
					break;
				}
			case eWeaponType_Staff:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 665 ) );
					break;
				}
			case eWeaponType_Bow:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 664 ) );
					break;
				}
			case eWeaponType_FishingPole:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 1542 ) );
					break;
				}
			case eWeaponType_CookUtil:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 1803 ) );
					break;
				}
				// 080902 LUJ, 총 추가
			case eWeaponType_Gun:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 1677 ) );
					break;
				}
				// 090528 ONS 신규종족 무기속성추가
			case eWeaponType_BothDagger:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 1924 ) );
					break;
				}
				// 080902 LUJ, 미지정 아이템은 '기타'로 표시
			default:
				{
					sprintf( line, "%s", CHATMGR->GetChatMsg( 48 ) );
					break;
				}
			}

			switch( eWeaponAnimationType( info->WeaponAnimation ) )
			{
			case eWeaponAnimationType_OneHand:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 658 ) );
					break;
				}
			case eWeaponAnimationType_TwoHand:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 659 ) );
					break;
				}
				//양손 스태프 표시
			case eWeaponAnimationType_Staff:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 659 ) );
					break;
				}

			}

			icon.AddToolTipLine( line, TTTC_ITEM_NAME );
			break;
		}
	case eEquipType_Armor:
		{
			if(eWeaponType( info->WeaponType ) == eWeaponType_CookUtil)
			{
				// 요리도구는 메시지 지정
				sprintf( line, "%s", CHATMGR->GetChatMsg( 1803 ) );
			}
			else
			{
				sprintf( line, "%s", GetName(info->EquipSlot));
			}

			// D : 종류
			switch( eArmorType( info->ArmorType ) )
			{
			case eArmorType_Robe:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 692 ) );
					break;
				}
				break;
			case eArmorType_Leather:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 693 ) );
					break;
				}
			case eArmorType_Metal:
				{
					sprintf( line, "%s / %s", line, CHATMGR->GetChatMsg( 694 ) );
					break;
				}				
			}

			icon.AddToolTipLine( line, TTTC_ITEM_NAME );
			break;
		}
			// 091008 pdy 기획팀 요청 악세사리 아이템은 슬롯위치로 판단하여 툴팁을 띄워준다. 
		case eEquipType_Accessary:
			{
				switch(info->EquipSlot)
				{
				case eWearedItem_Ring1:
				case eWearedItem_Ring2:
				case eWearedItem_Necklace:
				case eWearedItem_Earring1:
				case eWearedItem_Earring2:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1943 ) );
					}
					break;
				case eWearedItem_Belt:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1947 ) );
					}
					break;
				case eWearedItem_Band:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1951 ) );
					}
					break;
				case eWearedItem_Glasses:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1950 ) );
					}
					break;
				case eWearedItem_Mask:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1949 ) );
					}
					break;
				case eWearedItem_Wing:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1948 ) );
					}
					break;
				case eWearedItem_Costume_Head:
				case eWearedItem_Costume_Dress:
				case eWearedItem_Costume_Glove:
				case eWearedItem_Costume_Shoes:
					{
						sprintf( line, "%s", CHATMGR->GetChatMsg( 1952 ) );
					}
					break;

				}

				switch(info->EquipSlot)
				{
				case eWearedItem_Ring1:
				case eWearedItem_Ring2:
					{
						// 악세사리 / 반지 
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1946 ) );
					}
					break;
				case eWearedItem_Necklace:
					{
						// 악세사리 / 목걸이
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1944 ) );
					}
					break;
				case eWearedItem_Earring1:
				case eWearedItem_Earring2:
					{
						// 악세사리 / 귀걸이
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1945 ) );
					}
					break;
				case eWearedItem_Costume_Head:
					{
						// 코스튬 / 헤어
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1953 ) );
					}
					break;
				case eWearedItem_Costume_Dress:
					{
						// 코스튬 / 상의
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1954 ) );
					}
					break;
				case eWearedItem_Costume_Glove:
					{
						// 코스튬 / 장갑
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1955 ) );
					}
					break;
				case eWearedItem_Costume_Shoes:
					{
						// 코스튬 / 신발
						sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1956 ) );
					}
					break;
				}

				icon.AddToolTipLine( line, TTTC_ITEM_NAME );
			}
			break;

			// 091008 pdy 기획팀 요청 팻 아이템은 슬롯위치로 판단하여 툴팁을 띄워준다. 
			case eEquipType_Pet:
			{
				switch( ePetEquipSlot( info->EquipSlot - PET_EQUIP_SLOT_START ) )
				{
					// 펫 장비
					case ePetEquipSlot_Helmet:	
					case ePetEquipSlot_Armor:		
					case ePetEquipSlot_Glove:
					case ePetEquipSlot_Accessaries:
					case ePetEquipSlot_Weapon:
						{
							sprintf( line, "%s", CHATMGR->GetChatMsg( 1960 ) );
						}
						break;
					// 펫 코스튬
					case ePetEquipSlot_Face:
					case ePetEquipSlot_Head:
					case ePetEquipSlot_Back:	
					case ePetEquipSlot_Tail:	
						{
							sprintf( line, "%s", CHATMGR->GetChatMsg( 1962 ) );
						}
						break;

					case ePetEquipSlot_Skill:
						{
							sprintf( line, "%s", CHATMGR->GetChatMsg( 1960 ) );
						}
						break;
				}

				switch( ePetEquipSlot( info->EquipSlot - PET_EQUIP_SLOT_START ) )
				{
					// 펫 장비
					case ePetEquipSlot_Helmet:
						{
							// 펫 장비 / 투구
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 684 ) );
						}
						break;
					case ePetEquipSlot_Armor:	
						{
							// 펫 장비 / 갑옷
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 683 ) );
						}
						break;
					case ePetEquipSlot_Glove:
						{
							// 펫 장비 / 장갑
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 685 ) );
						}
						break;
					case ePetEquipSlot_Accessaries:
						{
							// 펫 장비 / 악세서리
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1943 ) );
						}
						break;
					case ePetEquipSlot_Weapon:
						{
							// 펫 장비 / 무기
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1961 ) );
						}
						break;
					// 펫 코스튬
					case ePetEquipSlot_Face:
						{
							// 펫 장비 / 얼굴장식
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1950 ) );
						}
						break;
					case ePetEquipSlot_Head:
						{
							// 펫 코스튬 / 머리장식
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1951 ) );
						}
						break;
					case ePetEquipSlot_Back:	
						{
							// 펫 장비 / 등
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1948 ) );
						}
						break;
					case ePetEquipSlot_Tail:	
						{
							// 펫 장비 / 꼬리
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1964 ) );
						}
						break;
					case ePetEquipSlot_Skill:
						{
							// 펫 장비 / 스킬
							sprintf( line, "%s / %s", line , CHATMGR->GetChatMsg( 1998 ) );
						}
						break;
				}

				icon.AddToolTipLine( line, TTTC_ITEM_NAME );
			}
			break;
	}
}


void CItemManager::AddDescriptionToolTip( cIcon& icon, const ITEMBASE& item, DWORD color )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	const std::vector< std::string >& cutToolTipList = GetToolTipMsg( info->ItemTooltipIdx );

	// 080227 LUJ, 옵션이 있거나 장비 아이템인 경우 한 줄 띄도록 한다
	if( !cutToolTipList.empty() && 
		( GetOption( item.dwDBIdx ).mItemDbIndex || eEquipType_None != info->EquipType ) )
	{
		icon.AddToolTipLine( "" );
	}

	for( std::vector< std::string >::const_iterator cutToolTipListIter = cutToolTipList.begin() ; cutToolTipListIter != cutToolTipList.end() ; ++cutToolTipListIter )
	{
		const std::string& strTooltip = *cutToolTipListIter;
		icon.AddToolTipLine( strTooltip.c_str(), color );
	}
}

// 090430 pdy 하우징아이템 툴팁 추가 
void CItemManager::AddFurnitureStateToolTip	( cIcon& icon, const ITEMBASE& item)
{
	const ITEM_INFO* pItemInfo = GetItemInfo( item.wIconIdx );

	if( !pItemInfo )
	{
		return;
	}

	if( pItemInfo->SupplyType != ITEM_KIND_FURNITURE )
		return;

	char tempBuf[512] = {0,};

	if( icon.GetType() == WT_HOUSING_STORED_ICON )
	{
		//보관 아이콘일 상태 
		cHousingStoredIcon* pHS_StoredIcon = (cHousingStoredIcon*) &icon;
		if(! pHS_StoredIcon )
			return;

		stFurniture* pstFurniture = pHS_StoredIcon->GetLinkFurniture();
		if(! pstFurniture )
			return;

		switch(pstFurniture->wState)
		{
			case eHOUSEFURNITURE_STATE_KEEP:
				{
					//회수가능 
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1846 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 255 )) ;

					//설치가능 
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1844 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 255 )) ;
				}
				break;
			case eHOUSEFURNITURE_STATE_INSTALL:
				{
					//회수불가
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1847 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 0, 0 )) ;

					//설치중
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1845 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 0, 0 )) ;
				}
				break;
			case eHOUSEFURNITURE_STATE_UNINSTALL:
				{
					//회수불가 
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1847 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 0, 0 )) ;

					//설치가능 
					sprintf(tempBuf,CHATMGR->GetChatMsg( 1844 )) ;
					icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 255 )) ;
				}
				break;
		}
	}
}


// 090430 pdy 하우징아이템 툴팁 추가 
void CItemManager::AddFurnitureOptionToolTip	( cIcon& icon, const ITEMBASE& item)
{
	const ITEM_INFO* pItemInfo = GetItemInfo( item.wIconIdx );

	if( !pItemInfo )
	{
		return;
	}

	if( pItemInfo->SupplyType != ITEM_KIND_FURNITURE )
		return;

	char tempBuf[512] = {0,};

	stFunitureInfo* pstFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pItemInfo->SupplyValue);
	if(! pstFurnitureInfo )
		return;

	icon.AddToolTipLine( "" );
	//꾸미기 포인트 
	sprintf(tempBuf,CHATMGR->GetChatMsg( 1848 ),pstFurnitureInfo->dwDecoPoint) ;
	icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;

	//쌓기 가능
	if( pstFurnitureInfo->bStackable )
	{
		sprintf(tempBuf,CHATMGR->GetChatMsg( 1849 )) ;
		icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
	}

	//회전 가능
	if( pstFurnitureInfo->bRotatable )
	{
		sprintf(tempBuf,CHATMGR->GetChatMsg( 1850 )) ;
		icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
	}
	icon.AddToolTipLine( "" );

}

// 090430 pdy 하우징아이템 툴팁 추가 
void CItemManager::AddFurnitureFunctionToolTip	( cIcon& icon, const ITEMBASE& item)
{
	const ITEM_INFO* pItemInfo = GetItemInfo( item.wIconIdx );

	if( !pItemInfo )
	{
		return;
	}

	if( pItemInfo->SupplyType != ITEM_KIND_FURNITURE )
		return;

	char tempBuf[512] = {0,};


	DWORD dwFunction = Get_Function(pItemInfo->SupplyValue);

	switch(dwFunction)
	{
		case eHOUSE_Function_None:
			{
				//일반 데코레이션 
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1851 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
		case eHOUSE_Function_Wall :
			{
				//고정외벽 
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1852 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
		case eHOUSE_Function_Floor :
			{
				//고정바닥
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1853 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
		case eHOUSE_Function_Door:
			{
				//문
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1854 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
		case eHOUSE_Function_Start:
			{
				//시작위치
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1855 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
		case eHOUSE_Function_Action:
			{
				//기능성
				sprintf(tempBuf,CHATMGR->GetChatMsg( 1856 )) ;
				icon.AddToolTipLine(tempBuf, RGB_HALF( 255, 255, 0 )) ;
			}
			break;
	}
}

void CItemManager::ItemDelete(CItem* pItem)
{
	m_ItemHash.Remove(pItem->GetDBIdx());	
	//ICFreeIndex(&m_IconIndexCreator,pItem->GetID()-START_ITEMTCON_ID);
	m_IconIndexCreator.ReleaseIndex(pItem->GetID());
	
//KES EXCHANGE 031002
	if( pItem->GetLinkItem() )
	{
		GAMEIN->GetExchangeDialog()->DelItem( 0, pItem->GetLinkItem()->GetPosition() );
	}
//

//	WINDOWMGR->DeleteWindowForID(pItem->GetID());	//KES 즉시지우지마시오. 031128
	WINDOWMGR->AddListDestroyWindow( pItem );
}


void CItemManager::Release()
{
	CItem* pItem;
	m_ItemHash.SetPositionHead();
	while((pItem = m_ItemHash.GetData()) != NULL)
	{
		ItemDelete(pItem);
		//ICFreeIndex(&m_IconIndexCreator,pItem->GetID()-START_ITEMTCON_ID);		
	}
	m_ItemHash.RemoveAll();

	{
		for(
			OptionMap::iterator it = mOptionMap.begin();
			mOptionMap.end() != it;
			++it )
		{
			SAFE_DELETE( it->second );
		}

		mOptionMap.clear();
	}
}


CItem* CItemManager::GetItem(DWORD dwDBIdx)
{
	return m_ItemHash.GetData( dwDBIdx );
}


void CItemManager::CombineItem( MSG_ITEM_COMBINE_ACK * pMsg )
{	
	const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);
	const eITEMTABLE ToTableIdx = GetTableIdxForAbsPos(pMsg->ToPos);

	CItem * pFromItem = GetItemofTable(FromTableIdx, pMsg->FromPos);
	CItem * pToItem = GetItemofTable(ToTableIdx, pMsg->ToPos);

	if( !pFromItem || !pToItem )
	{
		ASSERTMSG(0, "Item Combine Failed : pToItem=NULL or pFromItem=NULL");
		return;
	}
	if( ( pFromItem->GetItemIdx() != pToItem->GetItemIdx() ) || ( pFromItem->GetItemIdx() != pMsg->wItemIdx) )
//	if( ( pFromItem->GetItemIdx() != pToItem->GetItemIdx() ) || ( pFromItem->GetItemIdx() != pMsg->CombineItemIdx ) )
	{
		ASSERTMSG(0, "Item Combine Failed : FromItemIdx != ToItemIdx");
		return;
	}

	if(eITEM_TYPE_UNSEAL == pToItem->GetItemBaseInfo().nSealed)
	{
		const DWORD totalRemainSecond = pFromItem->GetItemBaseInfo().nRemainSecond * (pFromItem->GetDurability() - pMsg->FromDur) + pToItem->GetItemBaseInfo().nRemainSecond * pToItem->GetDurability();
		const DWORD combinedRemainSecond = totalRemainSecond / (pFromItem->GetDurability() - pMsg->FromDur + pToItem->GetDurability());

		pToItem->SetRemainTime(
			combinedRemainSecond);
#ifdef _GMTOOL_
		OutputDebug(
			"Items are merged with time");
		OutputDebug(
			"\tfrom: second: %d, count: %d, server count: %d",
			pFromItem->GetItemBaseInfo().nRemainSecond,
			pFromItem->GetDurability(),
			pMsg->FromDur);
		OutputDebug(
			"\tto: second: %d, count: %d, server count: %d",
			pToItem->GetItemBaseInfo().nRemainSecond,
			pToItem->GetDurability(),
			pMsg->ToDur);
		OutputDebug(
			"\tfrom: %d, to: %d. second",
			pFromItem->GetItemBaseInfo().nRemainSecond * (pFromItem->GetDurability() - pMsg->FromDur),
			pToItem->GetItemBaseInfo().nRemainSecond * pToItem->GetDurability());
		OutputDebug(
			"\ttotal: %d, count: %d, result: %d",
			totalRemainSecond,
			pFromItem->GetDurability() - pMsg->FromDur + pToItem->GetDurability(),
			combinedRemainSecond);
#endif
	}

	pFromItem->SetDurability( pMsg->FromDur );
	pToItem->SetDurability( pMsg->ToDur );

	// 100422 ShinJS --- 인벤토리 아이템 변경시 퀘스트 정보를 갱신한다.
	GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
	GAMEIN->GetQuestDialog()->RefreshQuestList();
	QUESTMGR->ProcessNpcMark();
	
	if( pMsg->FromDur == 0 )
	{
		DeleteItemofTable(
			FromTableIdx,
			pFromItem->GetPosition(),
			TRUE);
	}

	if( pMsg->ToDur == 0 )
	{
		DeleteItemofTable(
			FromTableIdx,
			pFromItem->GetPosition(),
			TRUE);
	}
}

void CItemManager::MoveItem( MSG_ITEM_MOVE_ACK * pMsg )
{
	const eITEMTABLE tableIdx = GetTableIdxForAbsPos(pMsg->ToPos);

	if(tableIdx == eItemTable_Storage)
		MoveItemToStorage(pMsg);
	else if(tableIdx == eItemTable_MunpaWarehouse)
		MoveItemToGuild(pMsg);
	else if(tableIdx == eItemTable_Shop)
		MoveItemToShop(pMsg);
	else if(tableIdx == eItemTable_Inventory || tableIdx == eItemTable_Weared)
	{
		CInventoryExDialog * pInven = GAMEIN->GetInventoryDialog();
		CStorageDialog* pStorage = GAMEIN->GetStorageDialog();
		CGuildWarehouseDialog* pGuild = GAMEIN->GetGuildWarehouseDlg();
		CItemShopDialog* pShop = GAMEIN->GetItemShopDialog();
		
		CItem * FromItem = NULL;
		CItem * ToItem = NULL;
		
		pInven->DeleteItem(pMsg->ToPos, &ToItem);

		const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);

		if(FromTableIdx == eItemTable_Storage)
		{
			pStorage->DeleteItem( pMsg->FromPos, &FromItem );
		}
		else if(FromTableIdx == eItemTable_MunpaWarehouse)
		{
			pGuild->DeleteItem( pMsg->FromPos, &FromItem );
		}
		else if(FromTableIdx == eItemTable_Shop)
		{
			// 후에 샵아이템 인벤에만 들어가도록 추가

			pShop->DeleteItem( pMsg->FromPos, &FromItem );
		}
		else 
			pInven->DeleteItem( pMsg->FromPos , &FromItem );

		if(ToItem)
		{
			// 080820 LUJ, 획득 시 쿨타임을 가진 아이템 정보를 변경한다
			UpdateCoolTimeItem( *ToItem );

			ToItem->SetPosition( pMsg->FromPos );

			if(FromTableIdx == eItemTable_Storage)
			{
				if( ToItem->IsQuickLink() )
				{
					ToItem->RemoveQuickLink();
				}
				pStorage->AddItem(ToItem);
			}
			else if(FromTableIdx == eItemTable_MunpaWarehouse)
			{
				if( ToItem->IsQuickLink() )
				{
					ToItem->RemoveQuickLink();
				}
				pGuild->AddItem(ToItem);
				QUICKMGR->RefreshQickItem();
			}
			else
				pInven->AddItem(ToItem);
		}
		
		if(FromItem == 0)
		{
			ASSERT(0);
			return;
		}

		// 080820 LUJ, 획득 시 쿨타임을 가진 아이템 정보를 변경한다
		UpdateCoolTimeItem( *FromItem );

		FromItem->SetPosition(pMsg->ToPos);
		pInven->AddItem(FromItem); 
		QUICKMGR->RefreshQickItem();
	}
}

void CItemManager::PetMoveItem( MSG_ITEM_MOVE_ACK * pMsg )
{
	CPetWearedDialog* pPetDlg = GAMEIN->GetPetWearedDlg();
	CInventoryExDialog * pInven = GAMEIN->GetInventoryDialog();
	CStorageDialog* pStorage = GAMEIN->GetStorageDialog();

	CItem * FromItem = NULL;
	cIcon * buf = NULL;
	CItem * ToItem = NULL;

	pPetDlg->DeleteItem(pMsg->ToPos, &buf);
	ToItem = ( CItem* )buf;

	const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);

	if(FromTableIdx == eItemTable_Storage)
	{
		pStorage->DeleteItem( pMsg->FromPos, &FromItem );
	}
	else 
		pInven->DeleteItem( pMsg->FromPos , &FromItem );

	if(ToItem)
	{
		ToItem->SetPosition( pMsg->FromPos );

		ITEM_INFO* pinfo = GetItemInfo( ToItem->GetItemBaseInfo().wIconIdx );
		if( pinfo )
		{
			if(FromTableIdx == eItemTable_Storage)
			{
				if( ToItem->IsQuickLink() )
				{
					ToItem->RemoveQuickLink();
				}
				pStorage->AddItem(ToItem);
			}
			else
				pInven->AddItem(ToItem);
		}
	}	

	if(FromItem)
	{
		FromItem->SetPosition( pMsg->ToPos );
		pPetDlg->AddItem( pMsg->ToPos, (cIcon*)FromItem );
	}

	QUICKMGR->RefreshQickItem();
}

void CItemManager::MoveItemToGuild( MSG_ITEM_MOVE_ACK* pMsg )
{	
	CGuildWarehouseDialog * pGuildDlg	= GAMEIN->GetGuildWarehouseDlg();
	CInventoryExDialog * pInvenDlg = GAMEIN->GetInventoryDialog();
	const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);
	CItem * FromItem = NULL;
	CItem * ToItem = NULL;

	pGuildDlg->DeleteItem(pMsg->ToPos , &ToItem);

	if(FromTableIdx == eItemTable_MunpaWarehouse)
	{
		pGuildDlg->DeleteItem( pMsg->FromPos, &FromItem);
	}
	else if(FromTableIdx == eItemTable_Inventory || FromTableIdx == eItemTable_Weared)
	{
		pInvenDlg->DeleteItem( pMsg->FromPos, &FromItem);
	}
	else 
		ASSERT(0);
	
	if(ToItem)
	{
		// 080820 LUJ, 획득 시 쿨타임 아이템은 이동 시마다 갱신해야한다
		UpdateCoolTimeItem( *ToItem );

		ToItem->SetPosition( pMsg->FromPos );
		if(FromTableIdx == eItemTable_Inventory)
			pInvenDlg->AddItem(ToItem);
		else
			pGuildDlg->AddItem(ToItem);
	}

	ReLinkQuickPosition(FromItem);
	if( FromItem->IsQuickLink() )
	{
		FromItem->RemoveQuickLink();
	}
	ASSERT(FromItem);

	// 080820 LUJ, 획득 시 쿨타임 아이템은 이동 시마다 갱신해야한다
	UpdateCoolTimeItem( *FromItem );

	FromItem->SetPosition(pMsg->ToPos);
	pGuildDlg->AddItem(FromItem);
	
	QUICKMGR->RefreshQickItem();

	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
}

void CItemManager::MoveItemToStorage(MSG_ITEM_MOVE_ACK* pMsg)
{
	CInventoryExDialog * pInven = GAMEIN->GetInventoryDialog();
	CStorageDialog* pStorage = GAMEIN->GetStorageDialog();
	
	CItem * FromItem = NULL;
	CItem * ToItem = NULL;

	pStorage->DeleteItem(pMsg->ToPos, &ToItem);
	
	const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);

	if(FromTableIdx == eItemTable_Inventory || FromTableIdx == eItemTable_Weared)
	{
		pInven->DeleteItem( pMsg->FromPos, &FromItem );				
	}
		
	else if(FromTableIdx == eItemTable_Storage)
	{
		pStorage->DeleteItem( pMsg->FromPos, &FromItem );
	}
	else
	{
		ASSERT(0);
		return;
	}
		
	if(ToItem)
	{
		// 080820 LUJ, 획득 시 쿨타임 아이템은 이동 시마다 갱신해야한다
		UpdateCoolTimeItem( *ToItem );

		ToItem->SetPosition( pMsg->FromPos );
		if(FromTableIdx == eItemTable_Inventory || FromTableIdx == eItemTable_Weared)
			pInven->AddItem(ToItem);
		else
			pStorage->AddItem(ToItem);
		if( ToItem->IsQuickLink() )
		{
			ToItem->RemoveQuickLink();
		}
	}

	ReLinkQuickPosition(FromItem);
	
	if( FromItem->IsQuickLink() )
	{
		FromItem->RemoveQuickLink();
	}
	ASSERT(FromItem);

	// 071125 LYW --- ItemManager : 시간 아이템 이동 처리 추가.
	const ITEMBASE& pItemBase = FromItem->GetItemBaseInfo() ;

	if( pItemBase.nSealed == eITEM_TYPE_UNSEAL )
	{
		const ITEM_OPTION& pOption = GetOption( FromItem->GetItemBaseInfo() ) ;

		AddTimeToolTip(((CItem*)FromItem), pOption, FALSE );
	}

	// 080820 LUJ, 획득 시 쿨타임 아이템은 이동 시마다 갱신해야한다
	UpdateCoolTimeItem( *FromItem );

	FromItem->SetPosition(pMsg->ToPos);
	pStorage->AddItem(FromItem);
	
	QUICKMGR->RefreshQickItem();
}



void CItemManager::MoveItemToShop(MSG_ITEM_MOVE_ACK* pMsg)
{
	CItemShopDialog* pShop = GAMEIN->GetItemShopDialog();
	
	CItem * FromItem = NULL;
	CItem * ToItem = NULL;

	pShop->DeleteItem(pMsg->ToPos, &ToItem);

	const eITEMTABLE FromTableIdx = GetTableIdxForAbsPos(pMsg->FromPos);

	if( FromTableIdx == eItemTable_Shop )
	{
		pShop->DeleteItem( pMsg->FromPos, &FromItem );				
	}
		
	if(ToItem)
	{
		// 080820 LUJ, 획득 시 쿨타임 아이템은 이동 시마다 갱신해야한다
		UpdateCoolTimeItem( *ToItem );

		ToItem->SetPosition( pMsg->FromPos );		

		if( FromTableIdx == eItemTable_Shop )		
			pShop->AddItem(ToItem);	

		if( ToItem->IsQuickLink() )
		{
			ToItem->RemoveQuickLink();
		}
	}

	ReLinkQuickPosition(FromItem);
	if( FromItem->IsQuickLink() )
	{
		FromItem->RemoveQuickLink();
	}
	ASSERT(FromItem);

	// 080820 LUJ, 획득 시 쿨타임 아이템은 이동 시마다 갱신해야한다
	UpdateCoolTimeItem( *FromItem );

	FromItem->SetPosition(pMsg->ToPos);
	pShop->AddItem(FromItem);

	QUICKMGR->RefreshQickItem();
}

void CItemManager::DeleteItem( POSTYPE absPos, CItem** ppItemOut )
{
	switch(GetTableIdxForAbsPos(absPos))
	{
	case eItemTable_Inventory:
	case eItemTable_Weared:
		{
			CInventoryExDialog * pInven = GAMEIN->GetInventoryDialog();
			pInven->DeleteItem( absPos, ppItemOut );

		}break;
	case eItemTable_Storage:
		{
			CStorageDialog* pStorage = GAMEIN->GetStorageDialog();
			pStorage->DeleteItem( absPos, ppItemOut );
			
		}break;
	case eItemTable_MunpaWarehouse:
		{
			CGuildWarehouseDialog* pGuild = GAMEIN->GetGuildWarehouseDlg();
			pGuild->DeleteItem( absPos, ppItemOut );
		}break;
	}

	if((*ppItemOut))
	{
		RemoveOption((*ppItemOut)->GetDBIdx());
		
		if( (*ppItemOut)->IsQuickLink() )
		{
			(*ppItemOut)->RemoveQuickLink();
		}

		ItemDelete( (*ppItemOut) );
	}
		
}

void CItemManager::DeleteItemofTable(eITEMTABLE TableIdx, POSTYPE absPos, BOOL bSellDel)
{
	CItem * pItemOut = NULL;
	
	switch(TableIdx)
	{
	case eItemTable_Inventory:
	case eItemTable_Weared:
		{
			CInventoryExDialog * pDlg = GAMEIN->GetInventoryDialog();
			if( !pDlg->DeleteItem( absPos, &pItemOut ) )
			{
				ASSERTMSG(0, "Item Delete Failed : DeleteItem return FALSE");
				return;
			}

			const ITEMBASE& pItemBase = pItemOut->GetItemBaseInfo() ;
			
			if( pItemBase.nSealed == eITEM_TYPE_UNSEAL && !bSellDel )
			{
				if( m_nSealItemDelPos != (int)pItemBase.Position )
				{
					CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1192 ), CHATMGR->GetChatMsg( 1189 ), pItemOut->GetItemInfo()->ItemName );
				}
			}

			// 071127 LYW --- ItemManager : 삭제 아이템 ( 봉인 관련 ) 위치 저장 변수 추가.
			m_nSealItemDelPos = -1 ;
		}
		break;
	case eItemTable_Storage:
		{
			CStorageDialog * pDlg = GAMEIN->GetStorageDialog();
			if( !pDlg->DeleteItem( absPos, &pItemOut ) )
			{
				ASSERTMSG(0, "Item Delete Failed : DeleteItem return FALSE");
				return;
			}

			// 071124 LYW --- ItemManager : 시간 완료 아이템 삭제 메시지 추가.
			const ITEMBASE& pItemBase = pItemOut->GetItemBaseInfo() ;
			
			if( pItemBase.nSealed == eITEM_TYPE_UNSEAL )
			{
				if( m_nSealItemDelPos != (int)pItemBase.Position )
				{
					CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1192 ), CHATMGR->GetChatMsg( 1188 ), pItemOut->GetItemInfo()->ItemName );
				}
			}

			// 071127 LYW --- ItemManager : 삭제 아이템 ( 봉인 관련 ) 위치 저장 변수 추가.
			m_nSealItemDelPos = -1 ;
		}
		break;
	case eItemTable_MunpaWarehouse:
		{
			CGuildWarehouseDialog * pDlg = GAMEIN->GetGuildWarehouseDlg();
			if( !pDlg->DeleteItem( absPos, &pItemOut ) )
			{
				ASSERTMSG(0, "Item Delete Failed : DeleteItem return FALSE");
				return;
			}			
		}
		break;
	case eItemTable_Shop:
		{
			CItemShopDialog* pDlg = GAMEIN->GetItemShopDialog();
			if( !pDlg->DeleteItem( absPos, &pItemOut ) )
			{
				ASSERTMSG(0, "Item Delete Failed : DeleteItem return FALSE");
				return;
			}
		}
		break;
	}
	if(pItemOut)
	{
		RemoveOption(pItemOut->GetDBIdx() );

		if( pItemOut->IsQuickLink() )
		{
			pItemOut->RemoveQuickLink();
		}
		ItemDelete( pItemOut );
	}
}


void CItemManager::RemoveOption( DWORD dwOptionIdx )
{
	OptionMap::iterator it = mOptionMap.find( dwOptionIdx );

	if( mOptionMap.end() == it )
	{
		return;
	}

	SAFE_DELETE( it->second );

	mOptionMap.erase( it );
}


void CItemManager::AddOption( const ITEM_OPTION& option )
{
	OptionMap::iterator it = mOptionMap.find( option.mItemDbIndex );

	// 이미 할당 공간이 있으면 해당 메모리에 복사해버린다
	if( mOptionMap.end() != it )
	{
		*( it->second ) = option;
		return;
	}

	mOptionMap.insert( std::make_pair( option.mItemDbIndex, new ITEM_OPTION( option ) ) );
}



const ITEM_OPTION& CItemManager::GetOption( const ITEMBASE& item ) const
{
	return GetOption( item.dwDBIdx );
}


const ITEM_OPTION& CItemManager::GetOption( DWORD itemDbIndex ) const
{
	OptionMap::const_iterator it = mOptionMap.find( itemDbIndex );

	if( mOptionMap.end() == it )
	{
		const static ITEM_OPTION emptyOption = {0};

		return emptyOption;
	}

	return *( it->second );
}


void CItemManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_ITEM_TOTALINFO_LOCAL:					Item_TotalInfo_Local( pMsg ) ;					break;
	case MP_ITEM_STORAGE_ITEM_INFO_ACK:				Item_Storage_Item_Info_Ack( pMsg ) ;			break;
	case MP_ITEM_MOVE_ACK:							Item_Move_Ack( pMsg ) ;							break;
	case MP_ITEM_COMBINE_ACK:						Item_Combine_Ack( pMsg ) ;						break;
	case MP_ITEM_DIVIDE_ACK:						Item_Divide_Ack( pMsg ) ;						break;
	case MP_ITEM_MONSTER_OBTAIN_NOTIFY:				Item_Monster_Obtain_Notify( pMsg ) ;			break;
	case MP_ITEM_MOVE_NACK:							Item_Move_Nack( pMsg ) ;						break;
	case MP_ITEM_DIVIDE_NACK:						Item_Divite_Nack( pMsg ) ;						break;
	case MP_ITEM_COMBINE_NACK:						Item_Combine_Nack( pMsg ) ;						break;
	case MP_ITEM_DISCARD_NACK:						Item_Discard_Nack( pMsg ) ;						break;
	case MP_ITEM_DISCARD_ACK:						Item_Discard_Ack( pMsg ) ;						break;
	case MP_ITEM_USE_NOTIFY:						Item_Use_Notify( pMsg ) ;						break;
	case MP_ITEM_USE_ACK:							Item_Use_Ack( pMsg ) ;							break;
	case MP_ITEM_REINFORCE_OPTION_ACK:				Item_Reinforce_Option_Ack( pMsg );				break;
	case MP_ITEM_REINFORCE_NACK:					Item_ReInForce_Nack( pMsg ) ;					break;
	case MP_ITEM_REINFORCE_SUCCEED_ACK:
	case MP_ITEM_REINFORCE_FAILED_ACK:
		{
			Item_ReInForce_Ack( pMsg );
			break;
		}	
	case MP_ITEM_COOK_ACK:
		{
			Item_Cook_Ack( pMsg);
			break;
		}
	case MP_ITEM_HOUSE_RESTORED:
		{
			Item_HouseRestored( pMsg ) ;
			break;
		}
	case MP_ITEM_HOUSE_BONUS:
		{
			Item_HouseBonus( pMsg ) ;
			break;
		}
	case MP_ITEM_MIX_SUCCEED_ACK:
	case MP_ITEM_MIX_FAILED_ACK:
	case MP_ITEM_MIX_BIGFAILED_ACK:
		{
			Item_Mix_Ack( pMsg );
			break;
		}
	case MP_ITEM_MIX_NACK:							Item_Mix_Nack( pMsg ) ;							break;
	case MP_ITEM_MIX_GETITEM:						Item_Mix_GetItem( pMsg );						break;
	case MP_ITEM_COOK_GETITEM:						Item_Cook_GetItem( pMsg );						break;
	case MP_ITEM_ENCHANT_SUCCESS_ACK:				Item_Enchant_Success_Ack( pMsg ) ;				break;
	case MP_ITEM_ENCHANT_NACK:						Item_Enchant_Nack( pMsg ) ;						break;
	case MP_ITEM_ENCHANT_FAILED_ACK:				Item_Enchant_Failed_Ack( pMsg );				break;
	case MP_ITEM_WORKING_SUCCESS:					Item_Working_Success( pMsg ) ;					break;
	case MP_ITEM_WORKING_FAIL:						Item_Working_Fail( pMsg ) ;						break;
	case MP_ITEM_WORKING_START:						Item_Working_Start( pMsg ) ;					break;
	case MP_ITEM_WORKING_STARTEX:					Item_Working_StartEx( pMsg ) ;					break;
	case MP_ITEM_WORKING_STOP:						Item_Working_Stop( pMsg ) ;						break;
	case MP_ITEM_USE_NACK:							Item_Use_Nack( pMsg ) ;							break;
	case MP_ITEM_SELL_ACK:							Item_Sell_Ack( pMsg ) ;							break;
	case MP_ITEM_SELL_NACK:							Item_Sell_Nack( pMsg ) ;						break;
	case MP_ITEM_BUY_ACK:							Item_Buy_Ack( pMsg ) ;							break;
	case MP_ITEM_BUY_NACK:							Item_Buy_Nack( pMsg ) ;							break;
	case MP_ITEM_BUY_BY_COIN:						Item_Buy_By_Coin( pMsg ) ;						break;			// 090227 ShinJS --- 암상인 추가작업 : Coin Item 구입시 Coin 제거 작업
	case MP_ITEM_DEALER_ACK:						Item_Dealer_Ack( pMsg ) ;						break;
	case MP_ITEM_APPEARANCE_ADD:					Item_Appearance_Add( pMsg );					break;
	case MP_ITEM_APPEARANCE_REMOVE:					Item_Appearance_Remove( pMsg );					break;
	case MP_ITEM_MONEY:								Item_Money( pMsg ) ;							break;
	case MP_ITEM_MONEY_ERROR:		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(115) );		break;
	case MP_ITEM_OBTAIN_MONEY:						Item_Obtain_Money( pMsg ) ;						break;
	case MP_ITEM_DIVIDE_NEWITEM_NOTIFY:				Item_Divide_NewItem_Notify( pMsg ) ;			break;
	case MP_ITEM_GUILD_MOVE_ACK:					Item_Guild_Move_Ack( pMsg ) ;					break;
	case MP_ITEM_GUILD_MOVE_NACK:					Item_Guild_Move_Nack( pMsg ) ;					break;
	case MP_ITEM_DELETEFROM_GUILDWARE_NOTIFY:		Item_DeleteFrom_GuildWare_Notify( pMsg ) ;		break;
	case MP_ITEM_ADDTO_GUILDWARE_NOTIFY:			Item_AddTo_GuildWare_Notify( pMsg ) ;			break;
	case MP_ITEM_STORAGE_MOVE_ACK:					Item_Storage_Move_Ack( pMsg ) ;					break;
	case MP_ITEM_DISSOLVE_GETITEM:					Item_Dissolve_GetItem( pMsg ) ;					break;
	case MP_ITEM_DISSOLVE_ACK:						Item_Dissolve_Ack( pMsg ) ;						break;
	case MP_ITEM_DISSOLVE_NACK:						Item_Dissolve_Nack( pMsg ) ;					break;	
	case MP_ITEM_ERROR_NACK:						Item_Error_Nack( pMsg ) ;						break;
	case MP_ITEM_CHANGEITEM_NACK:					Item_ChangeItem_Nack( pMsg ) ;					break;
	case MP_ITEM_USE_CHANGEITEM_ACK:				Item_Use_ChangeItem_Ack( pMsg ) ;				break;
	case MP_ITEM_TIMELIMT_ITEM_ONEMINUTE :			Item_TimeLimit_Item_OneMinute( pMsg ) ;			break;
	case MP_ITEM_SHOPITEM_USE_ACK:					Item_ShopItem_Use_Ack( pMsg ) ;					break;
	case MP_ITEM_EVENTITEM_USE:						Item_EventItem_Use( pMsg ) ;					break;
	case MP_ITEM_SHOPITEM_INFO_ACK:					Item_ShopItem_Info_Ack( pMsg ) ;				break;
	case MP_ITEM_SKILL_ACK :						Item_Skill_Ack( pMsg ) ;						break;
	case MP_ITEM_RESET_STATS_ACK :					Item_Reset_Stats_Ack( pMsg ) ;					break;
	case MP_ITEM_RESET_STATS_NACK :					Item_Reset_Stats_Nack( pMsg ) ;					break;
	case MP_ITEM_MAPMOVE_SCROLL_NACK :				Item_MapMoveScroll_Nack( pMsg ) ;				break;
	case MP_ITEM_INCREASE_INVENTORY_ACK :			Item_Increase_Inventory_Ack( pMsg ) ;			break;
	case MP_ITEM_INCREASE_INVENTORY_NACK :			Item_Increase_Inventory_Nack( pMsg ) ;			break;
	case MP_ITEM_CHANGE_CHARACTER_NAME_ACK :		Item_Change_Character_Name_Ack( pMsg ) ;		break;
	case MP_ITEM_CHANGE_CHARACTER_NAME_NACK :		Item_Change_Character_Name_Nack( pMsg ) ;		break;
	case MP_ITEM_GIFT_EVENT_NOTIFY:					Item_Gift_Event_Notify( pMsg );					break;

	// 080124 LUJ, 옵션 추가
	case MP_ITEM_OPTION_ADD_ACK:
		{
			const MSG_ITEM_OPTION* message = ( MSG_ITEM_OPTION* )pMsg;

			for( DWORD i = 0; i < message->mSize; ++i )
			{
				const ITEM_OPTION& option = message->mOption[ i ];

				AddOption( option );
			}
			break;
		}
	// 080218 LUJ, 아이템 옵션 부여
	case MP_ITEM_APPLY_OPTION_ACK:
		{
			CApplyOptionDialog* dialog = ( CApplyOptionDialog* )WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG );

			if( dialog )
			{
				dialog->Succeed();
			}

			break;
		}
	// 080218 LUJ, 아이템 옵션 부여
	case MP_ITEM_APPLY_OPTION_NACK:
		{
			CApplyOptionDialog* dialog = ( CApplyOptionDialog* )WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG );

			if( dialog )
			{
				dialog->Fail();
			}

			break;
		}

	// 080317 NYJ --- 전체외치기 아이템 사용실패
	case MP_ITEM_SHOPITEM_SHOUT_NACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1461 ) );
			break;
		}

	case MP_ITEM_ENCHANT_EFFECT_NOTIFY:
		{
			MSG_DWORDBYTE* pmsg = ( MSG_DWORDBYTE* )pMsg;

			CObject* pTarget = OBJECTMGR->GetObject( pmsg->dwData );

			if( !pTarget || pTarget->GetObjectKind() != eObjectKind_Player )
				break;

			CPlayer* pPlayer = ( CPlayer* )pTarget;

			pPlayer->GetCharacterTotalInfo()->WeaponEnchant = pmsg->bData;
		}
		break;
	case MP_ITEM_PET_MOVE_ACK:	Item_Pet_Move_Ack( pMsg ) ;	break;

		// 080916 LUJ, 아이템 합성 성공
	case MP_ITEM_COMPOSE_ACK:
		{
			CComposeDialog* dialog = GAMEIN->GetComposeDialog();

			if( dialog )
			{
				const MSG_ITEM_COMPOSE_ACK* message = ( MSG_ITEM_COMPOSE_ACK* )pMsg;
				dialog->Proceed( *message );
			}
		}
		break;
		// 080916 LUJ, 아이템 합성 실패
	case MP_ITEM_COMPOSE_NACK:
		{
			CComposeDialog* dialog = GAMEIN->GetComposeDialog();

			if( dialog )
			{
				const MSG_ITEM_COMPOSE_NACK* message = ( MSG_ITEM_COMPOSE_NACK* )pMsg;
				dialog->Proceed( *message );
			}
		}
		break;

	case MP_ITEM_CHANGESIZE_ACK:
		{
			MSG_DWORD2INT* pmsg = (MSG_DWORD2INT*)pMsg;
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData1 );
			if( !pPlayer ) return;

			float fSize = (float)pmsg->dwData2 / 100.f;
			if(fSize < .3f)
				fSize = 0.3f;
			else if(fSize > 3.0f)
				fSize = 3.0f;

			if(pPlayer->GetID() == gHeroID)
			{
				int nDiffSize = pmsg->nData;

				if(nDiffSize < -70)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1735));
				else if(-70<=nDiffSize && nDiffSize<-40)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1734));
				else if(-40<=nDiffSize && nDiffSize<-10)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1733));
				else if(-10<=nDiffSize && nDiffSize<11)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1736));
				else if(11<=nDiffSize && nDiffSize<41)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1730));
				else if(41<=nDiffSize && nDiffSize<71)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1731));
				else if(71 < nDiffSize)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1732));
			}

			
			pPlayer->SetCharacterSize( fSize );
		}
		break;

	case MP_ITEM_USEEFFECT_ACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData1 );
			if( !pPlayer ) return;

			OBJECTEFFECTDESC desc;
			desc.SetDesc((WORD)pmsg->dwData2);
			pPlayer->RemoveObjectEffect( ITEM_SUMMON_EFFECTID );
			pPlayer->AddObjectEffect( ITEM_SUMMON_EFFECTID, &desc, 1 );
		}
		break;
	// 100517 ONS 전직초기화 아이템 처리 추가
	case MP_ITEM_CHANGE_CLASS_ACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2314 ) );
		}
		break;
	case MP_ITEM_CHANGE_CLASS_NACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2226 ) );
		}
		break;
	case MP_ITEM_ENCHANT_LEVEL_NOTIFY:
		{
			EnchantNotifyAdd( pMsg );
		}
		break;
	}
}

BOOL CItemManager::IsEqualTableIdxForPos(eITEMTABLE TableIdx, POSTYPE absPos )
{
	if(TP_INVENTORY_START <= absPos && absPos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		return (TableIdx == eItemTable_Inventory);
	}
	else if(TP_WEAR_START <= absPos && absPos < TP_WEAR_END)
	{
		return (TableIdx == eItemTable_Weared);
	}
	else if(TP_GUILDWAREHOUSE_START <= absPos && absPos < TP_GUILDWAREHOUSE_END)
	{
		return (TableIdx == eItemTable_MunpaWarehouse);
	}
	else if(TP_STORAGE_START <= absPos && absPos < TP_STORAGE_END)
	{
		return (TableIdx == eItemTable_Storage);
	}
	
	return FALSE;
}

eITEMTABLE CItemManager::GetTableIdxForAbsPos(POSTYPE absPos)
{
	if(TP_INVENTORY_START <= absPos && absPos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		return eItemTable_Inventory;
	}
	else if(TP_WEAR_START <= absPos && absPos < TP_WEAR_END)
	{
		return eItemTable_Weared;
	}
	else if(TP_GUILDWAREHOUSE_START <= absPos && absPos < TP_GUILDWAREHOUSE_END)
	{
		return eItemTable_MunpaWarehouse;
	}
	else if(TP_STORAGE_START <= absPos && absPos < TP_STORAGE_END)
	{
		return eItemTable_Storage;
	}
	else if(TP_SHOPITEM_START <= absPos && absPos < TP_SHOPITEM_END)
	{
		return eItemTable_Shop;
	}
	
	return eItemTable_Max;
}


void CItemManager::RefreshAllItem()
{
	CItem* pItem;
	m_ItemHash.SetPositionHead();
	
	while((pItem = m_ItemHash.GetData()) != NULL)
	{
		// 080901 LUJ, 획득 시 쿨타임 아이템은 프로세스 타임마다 쿨타임을 처리하지 않는다.
		//				따라서 새로고침 전에 갱신해주지 않으면, 레벨업 등 때 처음 값으로 재설정된다
		UpdateCoolTimeItem( *pItem );

		RefreshItem( pItem );
	}
}

void CItemManager::RefreshItem( CItem* pItem )
{
	if(pItem->GetItemInfo()->Category == eItemCategory_Equip)
	{
		// 090114 LYW --- ItemManager : 아이템 착용 가능 체크 함수 수정에 따른 처리.
		if(CanEquip(pItem->GetItemIdx()) == eEquip_Able)
		{
			pItem->SetUseParam( 0 );
//			pItem->SetToolTipImageRGB( TTCLR_ITEM_CANEQUIP );
		}
		else
		{
			pItem->SetUseParam( 1 );
//			pItem->SetToolTipImageRGB( TTCLR_ITEM_CANNOTEQUIP );
		}
	}
	else if(pItem->GetItemInfo()->Category == eItemCategory_Expend)
	{
		if(CanUse(pItem->GetItemIdx()))
		{
			pItem->SetUseParam( 0 );
		}
		else
		{
			pItem->SetUseParam( 1 );
		}
	}

	AddToolTip( pItem );

	if( m_bAddPrice )
	{
		char buf[128];
		DWORD SellPrice = pItem->GetItemInfo()->SellPrice;
		pItem->AddToolTipLine( "" );
		wsprintf( buf, CHATMGR->GetChatMsg(35), AddComma(SellPrice) );
		pItem->AddToolTipLine( buf, TTTC_SELLPRICE );
	}	

	// 080820 LUJ, 지원 타입에 따른 처리 추가
	switch( pItem->GetItemInfo()->SupplyType )
	{
		// 080820 LUJ, 획득 시에 쿨타임을 갖고 있는 아이템
	case ITEM_KIND_COOLTIME:
		{
			const ITEMBASE& item			= pItem->GetItemBaseInfo();
			const DWORD		remainedTick	= item.nRemainSecond * 1000;

			// 080820 LUJ, 밀리세컨드 오차는 무시한다
			if( remainedTick / 100 <= pItem->GetRemainedCoolTime() / 100 )
			{
				break;
			}

			const DWORD maxTick		= pItem->GetItemInfo()->SupplyValue * 1000;
			const DWORD	currentTick	= GetTickCount();

			// 080820 LUJ, 남은 시간/최대 시간의 모습으로 쿨타임을 표시해주기 위해, 이전에 시작된 것처럼 설정한다
			pItem->SetCoolTimeAbs(
				currentTick - ( maxTick - remainedTick ),
				currentTick + remainedTick,
				maxTick );
			break;
		}
	}
}

void CItemManager::RefreshItemToolTip( DWORD ItemDBIdx )
{
	CItem* pItem = GetItem(ItemDBIdx);

	ASSERT(pItem);

	AddToolTip( pItem );
}

void CItemManager::SetPriceToItem( BOOL bAddPrice )
{
	CItem* pItem;
	m_ItemHash.SetPositionHead();

	if( bAddPrice )
	{
		char buf[128];

		while((pItem = m_ItemHash.GetData()) != NULL)
		{
			DWORD SellPrice = pItem->GetItemInfo()->SellPrice;
			pItem->AddToolTipLine( "" );
			wsprintf( buf, CHATMGR->GetChatMsg(35), AddComma(SellPrice) );
			pItem->AddToolTipLine( buf, TTTC_SELLPRICE );			
		}
	}
	else
	{
		while((pItem = m_ItemHash.GetData()) != NULL)
		{
			AddToolTip( pItem );
		}
	}

	m_bAddPrice = bAddPrice;
}

RESULT_CANEQUIP CItemManager::CanEquip(DWORD wItemIdx)
{
	ITEM_INFO * pInfo = GetItemInfo(wItemIdx);

	if( !pInfo ) return eEquip_Disable ;

	if(pInfo->LimitRace)
	{
		// 090708 ONS 신규종족 추가에 따른 종족별 아이템 착용처리 수정
		BYTE byRace = HERO->GetRace();
		if( (pInfo->LimitRace == eLimitRace_HumanElf) && (byRace == RaceType_Devil) ||
			(pInfo->LimitRace == eLimitRace_Devil)	  && (byRace != RaceType_Devil)	)
				return eEquip_RaceFail ;
	}


	// [경고] %s 아이템은 성별이 맞지 않아 착용할 수 없습니다.
	if(pInfo->LimitGender)
	{
		if(pInfo->LimitGender != HERO->GetGender()+1) return eEquip_GenderFail ;
	}
	

	// [경고] %s 아이템은 레벨이 낮아 착용할 수 없습니다.
	if(pInfo->LimitLevel > HERO->GetLevel())
	{
		return eEquip_LevelFail;
	}


	// 낚시대는 숙련등급을 비교함.
	WORD wFishingLevel = FISHINGMGR->GetFishingLevel();
	if(pInfo->WeaponType==eWeaponType_FishingPole && wFishingLevel<pInfo->kind)
		return eEquip_FishingLevelFail ;

	// 요리도구는 등급을 비교함.
	WORD wCookingLevel = COOKMGR->GetCookLevel();
	if(pInfo->WeaponType==eWeaponType_CookUtil && wCookingLevel<pInfo->Grade)
		return eEquip_CookingLevelFail ;


	// 착용 가능 결과를 리턴한다.
	return eEquip_Able ;
}

BOOL CItemManager::CanUse(DWORD wItemIdx)
{
	ITEM_INFO * pInfo = GetItemInfo(wItemIdx);


	// 미끼는 숙련등급을 비교함.
	if(pInfo->SupplyType == ITEM_KIND_FISHING_BAIT)
	{
		WORD wFishingLevel = FISHINGMGR->GetFishingLevel();

		if(wFishingLevel < pInfo->kind)
		{
			return FALSE;
		}
	}
	// 요리레시피는 레벨, 요리등급을 비교함.
	else if(pInfo->SupplyType == ITEM_KIND_ADDRECIPE)
	{
		WORD wRecipeLevel = (WORD)COOKMGR->GetCookLevelFromRecipe(pInfo->SupplyValue);

		if(pInfo->LimitLevel>HERO->GetLevel() || wRecipeLevel>COOKMGR->GetCookLevel())
		{
			return FALSE;
		}
	}
	// 요리는 레벨을 비교함
	else if(pInfo->SupplyType == ITEM_KIND_FOOD)
	{
		if(pInfo->LimitLevel>HERO->GetLevel())
		{
			return FALSE;
		}
	}
	// 요리스킬북은 숙련도와 레벨을 비교함
	else if(pInfo->SupplyType == ITEM_KIND_FOODSKILLBOOK)
	{
		if(pInfo->LimitLevel>HERO->GetLevel())
		{
			return FALSE;
		}

		if(pInfo->SupplyValue-COOKMGR->GetCookLevel() > 1)
		{
			return FALSE;
		}
	}

	return TRUE;
}

ITEM_INFO * CItemManager::GetItemInfo(DWORD wItemIdx)
{
	return m_ItemInfoList.GetData(wItemIdx);
}

LPCTSTR CItemManager::GetName(EWEARED_ITEM slot) const
{
	switch( slot )
	{
	case eWearedItem_Shield:
		{
			return CHATMGR->GetChatMsg( 695 );
		}
	case eWearedItem_Dress:
		{
			return CHATMGR->GetChatMsg( 683 );
		}
	case eWearedItem_Hat:
		{
			return CHATMGR->GetChatMsg( 684 );
		}
	case eWearedItem_Glove:
		{
			return CHATMGR->GetChatMsg( 685 );
		}
	case eWearedItem_Shoes:
		{
			return CHATMGR->GetChatMsg( 686 );
		}
	case eWearedItem_Ring1:
		{
			return CHATMGR->GetChatMsg( 687 );
		}
	case eWearedItem_Necklace:
		{
			return CHATMGR->GetChatMsg( 688 );
		}
	case eWearedItem_Earring1:
		{
			return CHATMGR->GetChatMsg( 689 );
		}
	case eWearedItem_Belt:
		{
			return CHATMGR->GetChatMsg( 690 );
		}
	case eWearedItem_Band:
		{
			return CHATMGR->GetChatMsg( 691 );
		}
	case eWearedItem_Glasses:
		{
			return CHATMGR->GetChatMsg( 832 );
		}
	case eWearedItem_Mask:
		{
			return CHATMGR->GetChatMsg( 833 );
		}
	case eWearedItem_Wing:
		{
			return CHATMGR->GetChatMsg( 834 );
		}
	case eWearedItem_Costume_Head:
		{
			return CHATMGR->GetChatMsg( 835 );
		}
	case eWearedItem_Costume_Dress:
		{
			return CHATMGR->GetChatMsg( 836 );
		}
	case eWearedItem_Costume_Glove:
		{
			return CHATMGR->GetChatMsg( 837 );
		}
	case eWearedItem_Costume_Shoes:
		{
			return CHATMGR->GetChatMsg( 838 );
		}
	}

	return "?";
}

// 080916 LUJ, 옵션 이름을 반환받는다
const char* CItemManager::GetName( ITEM_OPTION::Drop::Key key ) const
{
	switch( key )
	{
	case ITEM_OPTION::Drop::KeyPlusStrength:
		{
			return CHATMGR->GetChatMsg( 157 );
		}
	case ITEM_OPTION::Drop::KeyPlusIntelligence:
		{
			return CHATMGR->GetChatMsg( 160 );
		}
	case ITEM_OPTION::Drop::KeyPlusDexterity:
		{
			return CHATMGR->GetChatMsg( 158 );
		}
	case ITEM_OPTION::Drop::KeyPlusWisdom:
		{
			return CHATMGR->GetChatMsg( 161 );
		}
	case ITEM_OPTION::Drop::KeyPlusVitality:
		{
			return CHATMGR->GetChatMsg( 678 );
		}
	case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
		{
			return CHATMGR->GetChatMsg( 152 );
		}
	case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
		{
			return CHATMGR->GetChatMsg( 153 );
		}
	case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
		{
			return CHATMGR->GetChatMsg( 155 );
		}
	case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
		{
			return CHATMGR->GetChatMsg( 156 );
		}
	case ITEM_OPTION::Drop::KeyPlusCriticalRate:
		{
			return CHATMGR->GetChatMsg( 842 );
		}
	case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
		{
			return CHATMGR->GetChatMsg( 843 );
		}
	case ITEM_OPTION::Drop::KeyPlusAccuracy:
		{
			return CHATMGR->GetChatMsg( 162 );
		}
	case ITEM_OPTION::Drop::KeyPlusEvade:
		{
			return CHATMGR->GetChatMsg( 841 );
		}
	case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
		{
			return CHATMGR->GetChatMsg( 840 );
		}
	case ITEM_OPTION::Drop::KeyPlusLife:
		{
			return CHATMGR->GetChatMsg( 164 );
		}
	case ITEM_OPTION::Drop::KeyPlusMana:
		{
			return CHATMGR->GetChatMsg( 165 );
		}
	case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
		{
			return CHATMGR->GetChatMsg( 846 );
		}
	case ITEM_OPTION::Drop::KeyPlusManaRecovery:
		{
			return CHATMGR->GetChatMsg( 847 );
		}
	case ITEM_OPTION::Drop::KeyPercentStrength:
		{
			return CHATMGR->GetChatMsg( 157 );
		}
	case ITEM_OPTION::Drop::KeyPercentIntelligence:
		{
			return CHATMGR->GetChatMsg( 160 );
		}
	case ITEM_OPTION::Drop::KeyPercentDexterity:
		{
			return CHATMGR->GetChatMsg( 158 );
		}
	case ITEM_OPTION::Drop::KeyPercentWisdom:
		{
			return CHATMGR->GetChatMsg( 161 );
		}
	case ITEM_OPTION::Drop::KeyPercentVitality:
		{
			return CHATMGR->GetChatMsg( 678 );
		}
	case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
		{
			return CHATMGR->GetChatMsg( 152 );
		}
	case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
		{
			return CHATMGR->GetChatMsg( 153 );
		}
	case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
		{
			return CHATMGR->GetChatMsg( 155 );
		}
	case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
		{
			return CHATMGR->GetChatMsg( 156 );
		}
	case ITEM_OPTION::Drop::KeyPercentCriticalRate:
		{
			return CHATMGR->GetChatMsg( 842 );
		}
	case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
		{
			return CHATMGR->GetChatMsg( 843 );
		}
	case ITEM_OPTION::Drop::KeyPercentAccuracy:
		{
			return CHATMGR->GetChatMsg( 162 );
		}
	case ITEM_OPTION::Drop::KeyPercentEvade:
		{
			return CHATMGR->GetChatMsg( 841 );
		}
	case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
		{
			return CHATMGR->GetChatMsg( 840 );
		}
	case ITEM_OPTION::Drop::KeyPercentLife:
		{
			return CHATMGR->GetChatMsg( 164 );
		}
	case ITEM_OPTION::Drop::KeyPercentMana:
		{
			return CHATMGR->GetChatMsg( 165 );
		}
	case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
		{
			return CHATMGR->GetChatMsg( 846 );
		}
	case ITEM_OPTION::Drop::KeyPercentManaRecovery:
		{
			return CHATMGR->GetChatMsg( 847 );
		}
	}

	return "?";
}



void CItemManager::LoadItemToolTipList()
{
	CMHFile file;
	if( !file.Init( "Data/Interface/Windows/ToolTipMsg.bin", "rb" ) )
		return;

	char buf[512];	//툴팁의 길이제한...22		//256까지 지원하고..
	DWORD dwIndex;
	while( 1 )
	{
		if( file.IsEOF() ) break;

		SafeStrCpy( buf, file.GetString(), sizeof(buf) );

		if( strcmp( buf, "#Msg" ) == 0 )
		{
			dwIndex = file.GetDword();	//msg index
			SafeStrCpy( buf, file.GetString(), sizeof(buf) );
			if( strcmp( buf, "{" ) == 0 )
			{
				// ToolTipMsg 를 모두 읽은 후 예약어를 변환한다.
				std::string strToolTipMsg;
				file.GetLine( buf, sizeof(buf) );
				strToolTipMsg.append( buf );

				file.GetLine( buf, sizeof(buf) );
				while( *buf != '}' )
				{
					strToolTipMsg += "\n";
					strToolTipMsg.append( buf );
					file.GetLine( buf, sizeof(buf) );
				}

				// 100312 ShinJS --- 툴팁 메세지 예약어 변환
 				SKILLMGR->ConvertToolTipMsgKeyWordFormText( dwIndex, strToolTipMsg );

				m_ToolTipTextMap.insert( std::make_pair( dwIndex, strToolTipMsg ) );
			}
		}		
	}	
}

#define TOOLTIPINFO_EXTENT		144
const std::vector< std::string >& CItemManager::GetToolTipMsg( DWORD dwIndex )
{
	static std::vector< std::string > emptyTooltipList;

	// 적당히 잘린 ToolTip 검색
	std::map< DWORD, std::vector< std::string > >::const_iterator cutToolTipIter = m_CutToolTipText.find( dwIndex );

	// 존재하지 않는 경우 생성
	if( cutToolTipIter == m_CutToolTipText.end() )
	{
		const std::string& totalToolTip = GetTotalToolTipMsg( dwIndex );
		if( totalToolTip.empty() )
			return emptyTooltipList;

		// 이름출력시의 길이로 자른다.
		int nToolTipWidth = TOOLTIPINFO_EXTENT;
		cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( dwIndex );
		ITEM_INFO* pItemInfo = GetItemInfo( dwIndex );
		if( pSkillInfo )
		{
			char buf[512]={0,};
			sprintf( buf, CHATMGR->GetChatMsg( 2094 ), pSkillInfo->GetName(), pSkillInfo->GetLevel() );
			int nNameWidth = CFONT_OBJ->GetTextExtentEx( 0, buf, strlen( buf ) );
			nToolTipWidth = ( nNameWidth > TOOLTIPINFO_EXTENT ? nNameWidth : TOOLTIPINFO_EXTENT );
		}
		else if( pItemInfo )
		{
			int nNameWidth = CFONT_OBJ->GetTextExtentEx( 0, pItemInfo->ItemName, strlen( pItemInfo->ItemName ) );
			nToolTipWidth = ( nNameWidth > TOOLTIPINFO_EXTENT ? nNameWidth : TOOLTIPINFO_EXTENT );
		}

		std::vector< std::string >& cutToolTipList = m_CutToolTipText[ dwIndex ];
		CutToolTipMsg( totalToolTip, cutToolTipList, nToolTipWidth );

		return cutToolTipList;
	}

	return cutToolTipIter->second;
}

const std::string& CItemManager::GetTotalToolTipMsg( DWORD dwIndex )
{
	static std::string emptyTooltip;

	std::map< DWORD, std::string >::const_iterator tooltipMapIter = m_ToolTipTextMap.find( dwIndex );
	if( tooltipMapIter == m_ToolTipTextMap.end() )
        return emptyTooltip;

	return tooltipMapIter->second;
}

void CItemManager::CutToolTipMsg( const std::string totalToolTip, std::vector< std::string >& cutToolTipList, const int nCutWidth )
{
	char* str = (char*)totalToolTip.c_str();
	int nLen = totalToolTip.length();
	std::string cutToolTip;

#ifdef _TL_LOCAL_
	BYTE brk[512];
	int nBrkNum = g_TBreak.FindThaiWordBreak( str, nLen, brk, 512, FTWB_SEPARATESYMBOL );
	int nCutPos = 0;

	for( int i = 0 ; i < nBrkNum ; ++i )
	{
		nCutPos += brk[i];
		if( CFONT_OBJ->GetTextExtentEx(0, str, nCutPos ) > nCutWidth )
		{
			nCutPos -= brk[i];
			cutToolTip.clear();
			cutToolTip.append( str, nCutPos );

			--i;
			str += nCutPos;
			nCutPos = 0;

			if( *str == ' ' )
			{
				++str;	//라인 첫글자가 스페이스가 아니도록...
				++i;
			}

			cutToolTipList.push_back( cutToolTip );
		}
	}

#else

	int nCutPos = 0;
	int nLenPos = 0;
	while( nLenPos < nLen )
	{
		if( IsDBCSLeadByte( *(str + nCutPos) ) )
		{
			++nCutPos;
			++nLenPos;
		}

		++nCutPos;
		++nLenPos;

		if( CFONT_OBJ->GetTextExtentEx(0, str, nCutPos ) > nCutWidth )
		{
			cutToolTip.clear();
			cutToolTip.append( str, nCutPos );

			str += nCutPos;
			nCutPos = 0;

			if( *str == ' ' )
			{
				++str;	//라인 첫글자가 스페이스가 아니도록...
				++nLenPos;
			}

			cutToolTipList.push_back( cutToolTip );
		}
	}
#endif

	if( nCutPos > 0 )
	{
		cutToolTip.clear();
		cutToolTip.append( str );

		cutToolTipList.push_back( cutToolTip );
	}
}

void CItemManager::LoadItemList()
{
	{
		CMHFile file;

		if( ! file.Init("System/Resource/ItemList.bin", "rb" ) )
		{
			return;
		}

		while( ! file.IsEOF() )
		{
			const DWORD dwItemIndex = file.GetDword();
			if( dwItemIndex == 0 )
			{
				char buf[MAX_PATH]={0,};
				file.GetLine( buf, sizeof(buf) );
				continue;
			}

			ITEM_INFO* pInfo = new ITEM_INFO;
			if( !pInfo )
				continue;

			pInfo->ItemIdx = dwItemIndex;

#ifdef _GMTOOL_
			if (m_ItemInfoList.GetData(pInfo->ItemIdx))
			{
				char szText[256];
				sprintf(szText, "이미 존재하는 아이템ID: %d", pInfo->ItemIdx);
				char	szCaption[512];														// 아래 두줄 살짝 추가. -_-; 2007.05.28 이진영.
				sprintf( szCaption, "%s, %s() %d", __FILE__, __FUNCTION__, __LINE__); 
				MessageBox(NULL, szText, szCaption, NULL);
			}
#endif
			// E ASSERT수정 추가 added by hseos 2007.05.22

			SafeStrCpy( pInfo->ItemName, file.GetString(), MAX_ITEMNAME_LENGTH+1 );

			pInfo->ItemTooltipIdx = file.GetDword();
			pInfo->Image2DNum = file.GetWord();	
			pInfo->Part3DType = file.GetWord();
			pInfo->Part3DModelNum = file.GetWord();	

			pInfo->Shop = file.GetWord(); // 아이템몰 여부
			pInfo->Category = eItemCategory(file.GetWord());
			pInfo->Grade = file.GetWord();
			pInfo->LimitRace = LIMIT_RACE(file.GetWord());
			pInfo->LimitGender = file.GetWord();		
			pInfo->LimitLevel = (WORD)file.GetLevel();	

			pInfo->Stack = file.GetWord();
			pInfo->Trade = file.GetWord();
			pInfo->Deposit = file.GetWord();
			pInfo->Sell = file.GetWord();
			pInfo->Decompose = file.GetWord();

			pInfo->DecomposeIdx = file.GetDword();
			pInfo->Time = file.GetDword();

			pInfo->BuyPrice = file.GetDword();			
			pInfo->SellPrice = file.GetDword();	

#ifdef _GMTOOL_
			if( pInfo->BuyPrice < pInfo->SellPrice )
			{
				char szText[256];
				sprintf(szText, "판매금액이 구매금액보다 높은 아이템ID: %d", pInfo->ItemIdx);
				char	szCaption[512];														// 아래 두줄 살짝 추가. -_-; 2007.05.28 이진영.
				sprintf( szCaption, "%s, %s() %d", __FILE__, __FUNCTION__, __LINE__); 
				MessageBox(NULL, szText, szCaption, NULL);
			}
#endif
			pInfo->EquipType = eEquipType(file.GetWord());
			pInfo->EquipSlot = EWEARED_ITEM(file.GetWord() - 1);
			pInfo->Durability = file.GetDword();
			file.GetDword();

			pInfo->Repair = file.GetWord();
			pInfo->WeaponAnimation = eWeaponAnimationType(file.GetWord());
			pInfo->WeaponType = eWeaponType(file.GetWord());
			pInfo->PhysicAttack = file.GetInt();
			pInfo->MagicAttack = file.GetInt();
			pInfo->ArmorType = eArmorType(file.GetWord());
			pInfo->AccessaryType = eAccessaryType(file.GetWord());
			pInfo->PhysicDefense = file.GetInt();
			pInfo->MagicDefense = file.GetInt();

			pInfo->IsEnchant = file.GetBool();
			pInfo->Improvement = file.GetWord();

			pInfo->EnchantValue		= file.GetDword();
			pInfo->EnchantDeterm	= file.GetDword();

			pInfo->ImprovementStr = file.GetInt();			
			pInfo->ImprovementDex = file.GetInt();			
			pInfo->ImprovementVit = file.GetInt();		//10
			pInfo->ImprovementInt = file.GetInt();
			pInfo->ImprovementWis = file.GetInt();			
			pInfo->ImprovementLife = file.GetInt();			
			pInfo->ImprovementMana = file.GetInt();			

			pInfo->ImprovementSkill = file.GetDword();
			pInfo->Skill		= file.GetDword();
			pInfo->SupplyType	= ITEM_KIND(file.GetWord());
			pInfo->SupplyValue	= file.GetDword();
			pInfo->kind			= ITEM_INFO::eKind( file.GetDword() );

			pInfo->RequiredBuff = file.GetDword();
			pInfo->Battle = file.GetBool();
			pInfo->Peace = file.GetBool();
			pInfo->Move = file.GetBool();
			pInfo->Stop = file.GetBool();
			pInfo->Rest = file.GetBool();
			pInfo->wSeal = ITEM_SEAL_TYPE(file.GetWord());
			pInfo->nTimeKind = ITEM_TIMEKIND(file.GetInt());
			pInfo->dwUseTime = file.GetDword();
			pInfo->dwBuyFishPoint = file.GetDword();
			pInfo->wPointType = eItemPointType(file.GetWord());
			pInfo->dwPointTypeValue1 = file.GetDword();
			pInfo->dwPointTypeValue2 = file.GetDword();
			pInfo->dwType =  file.GetDword(); // Item 종류
			pInfo->dwTypeDetail =  file.GetDword();	// Item 세부종류

			switch(pInfo->SupplyType)
			{
			case ITEM_KIND_FARM_PLANT:
				{
					CSHFarmManageGardenDlg::SetCropImgInfo( ITEM_KIND( pInfo->SupplyValue ), pInfo->Image2DNum );
					break;
				}
			case ITEM_KIND_ANIMAL:
				{
					GAMEIN->GetAnimalDialog()->SetAnimalImgInfo( ITEM_KIND( pInfo->SupplyValue ), pInfo->Image2DNum );
					break;
				}
			}

			m_ItemInfoList.Add(pInfo, pInfo->ItemIdx);
		}
	}
}

BOOL CItemManager::IsDupItem( DWORD wItemIdx )
{
	return 0 < GetItemStackNum(
		wItemIdx);
}

CItem* CItemManager::GetItemofTable(eITEMTABLE TableIDX, POSTYPE absPos)
{
	CItem * pItem = NULL;
	switch(TableIDX)
	{
	case eItemTable_Weared:
	case eItemTable_Inventory:
		{
			CInventoryExDialog * pDlg = GAMEIN->GetInventoryDialog();
			pItem = pDlg->GetItemForPos( absPos );			
		}
		break;
	case eItemTable_Storage:
		{
			CStorageDialog * pDlg = GAMEIN->GetStorageDialog();
			pItem = pDlg->GetItemForPos( absPos );			
		}
		break;
	case eItemTable_MunpaWarehouse:
		{
			CGuildWarehouseDialog * pDlg = GAMEIN->GetGuildWarehouseDlg();
			pItem = pDlg->GetItemForPos( absPos );			
		}
		break;
	case eItemTable_Shop:
		{
			CItemShopDialog* pDlg = GAMEIN->GetItemShopDialog();
			pItem = pDlg->GetItemForPos( absPos );
		}
		break;
	}
	return pItem;
}

// yunho StatsCalcManager 공유를 위해
const ITEMBASE* CItemManager::GetItemInfoAbsIn( CPlayer*, POSTYPE absPos)
{
	const eITEMTABLE TableIdx = GetTableIdxForAbsPos(absPos);

	if(eItemTable_Max == TableIdx)
	{
		return NULL;
	}

	CItem* pItem = GetItemofTable(
		TableIdx,
		absPos);
	
	return pItem ? &( pItem->GetItemBaseInfo() ) : 0;
}

void CItemManager::SetDisableDialog(BOOL val, BYTE TableIdx)
{
	switch(TableIdx)
	{
	case eItemTable_Weared:
	case eItemTable_Inventory:
		{
			CInventoryExDialog * pDlg = GAMEIN->GetInventoryDialog();
			if( pDlg )
				pDlg->SetDisable( val );
		}
		break;
	case eItemTable_Storage:
		{
			CStorageDialog * pDlg = GAMEIN->GetStorageDialog();
			if( pDlg )
				pDlg->SetDisable( val );			
		}
		break;
	case eItemTable_MunpaWarehouse:
		{
			CGuildWarehouseDialog * pDlg = GAMEIN->GetGuildWarehouseDlg();
			if( pDlg )
				pDlg->SetDisable( val );
		}
		break;
	case eItemTable_Deal:
		{
			CDealDialog * pDlg = GAMEIN->GetDealDialog();
			if( pDlg )
				pDlg->SetDisable( val );
		}
		break;
	case eItemTable_Shop:
		{
			CItemShopDialog* pDlg = GAMEIN->GetItemShopDialog(); 
			if( pDlg )
				pDlg->SetDisable( val );
		}
		break;
	default:
		{
			ASSERTMSG(0, "테이블 인덱스가 벗어났음다.DisableDlg()-Taiyo. Invalid Table Index.");
			break;
		}
	}
}

void CItemManager::DivideItem(ITEMOBTAINARRAY * pMsg )
{
	ITEMOBTAINARRAY * pmsg = (ITEMOBTAINARRAY *)pMsg;
	
	CItem* pItem = NULL;
	CItem* pPreItem = NULL;
	for(WORD i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		DeleteItem(pmsg->GetItem(i)->Position, &pPreItem);

		pItem = MakeNewItem(pmsg->GetItem(i),"DivideItem");	
	//	BOOL rt = GAMEIN->GetInventoryDialog()->AddItem(pItem);
		if(!AddItem(pItem))
		{
			ASSERT(0);
		}
	}
}

BOOL CItemManager::AddItem(CItem* pItem)
{
	ASSERT( pItem );

	switch( GetTableIdxForAbsPos( pItem->GetPosition() ) )
	{
	case eItemTable_Inventory:
	case eItemTable_Weared:
	case eItemTable_Storage:
		return GAMEIN->GetStorageDialog()->AddItem(pItem);
	case eItemTable_MunpaWarehouse:
		return GAMEIN->GetGuildWarehouseDlg()->AddItem(pItem);
	}
	
	return FALSE;
}

ITEM_INFO* CItemManager::FindItemInfoForName( char* strName )
{
	m_ItemInfoList.SetPositionHead();
	ITEM_INFO* pInfo = NULL;
	while( (pInfo = m_ItemInfoList.GetData()) != NULL)
	{
		if( strcmp( strName, pInfo->ItemName ) == 0 )
		{
			return pInfo;
		}
	}

	return NULL;
}

void CItemManager::ReLinkQuickPosition(CItem* pItem)
{
	if(IsDupItem(pItem->GetItemIdx()))
	{
		QUICKMGR->RefreshQickItem();
	}
}

void CItemManager::SetPreItemData(sPRELOAD_INFO* pPreLoadInfo, int* Level, int Count)
{
	ITEM_INFO* pItemInfo = NULL;
	int ItemIndex = 0;

	m_ItemInfoList.SetPositionHead();
	while( (pItemInfo = m_ItemInfoList.GetData()) != NULL)
	{
		for(int i=0; i<Count; i++)
		{
			if(pItemInfo->LimitLevel == Level[i])
			{
				// 나머지가 있으면 기본무기에 +가 붙은 아이템이다. 그래서 패스~
				if( pItemInfo->ItemIdx%10 )			continue;
				
				ItemIndex = pPreLoadInfo->Count[ePreLoad_Item];
				pPreLoadInfo->Kind[ePreLoad_Item][ItemIndex] = pItemInfo->ItemIdx;
				++pPreLoadInfo->Count[ePreLoad_Item];
				
				// Max를 넘어가면 그마안~~
				if(pPreLoadInfo->Count[ePreLoad_Item] >= MAX_KIND_PERMAP)		return;
			}
		}
	}
}

void CItemManager::ItemDropEffect( DWORD wItemIdx )
{
	const ITEM_INFO* const itemInfo = GetItemInfo(
		wItemIdx);

	if(0 == itemInfo)
	{
		return;
	}

	switch(itemInfo->kind)
	{
	case ITEM_INFO::eKindRare:
		{
			EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Rare);
		}
		break;
	case ITEM_INFO::eKindUnique:
		{
			EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Unique);
		}
		break;
	}

	if(itemInfo->Category == eItemCategory_Equip )
	{
		switch(GetItemInfo(wItemIdx)->EquipType)
		{
		case eEquipType_Weapon:
			{
				EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Weapon);
			}
			break;
		case eEquipType_Armor:
			{
				EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Dress);
			}
			break;
		}
	}
	else
	{
		EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Youngyak);
	}
}

void CItemManager::Process()
{
	ProcessCoolTime();

	// 100609 ONS 5초간격으로 체크하여 큐에 데이터가 존재하면 공지를 띄운다.
	if(false == m_EnchantNotifyQueue.empty())
	{
		if(m_ENchantNotifyCheckTime > gCurTime)
		{
			return;
		}

		const EnchantNotifyInfo Info = m_EnchantNotifyQueue.front();
		m_EnchantNotifyQueue.pop();

		const ITEM_INFO* pInfo = GetItemInfo( Info.ItemIndex );
		if( NULL == pInfo )
		{
			return;
		}

        char buf[128] = {0};
		sprintf( buf, Info.IsSuccessed ? CHATMGR->GetChatMsg(2285) : CHATMGR->GetChatMsg(2321), 
						Info.Name, 
						pInfo->ItemName, 
						Info.EnchantLevel );
		GAMEIN->GetItemPopupDlg()->SetText( buf );
		GAMEIN->GetItemPopupDlg()->SetActive( TRUE );

		m_ENchantNotifyCheckTime = gCurTime + 5000;
	}
}

void CItemManager::FakeDeleteItem(CItem* pItem)
{
	m_TempDeleteItem.Durability = pItem->GetDurability();
	m_TempDeleteItem.Position = pItem->GetPosition();
	m_TempDeleteItem.wIconIdx = pItem->GetItemIdx();
}

void CItemManager::SendDeleteItem()
{
	if( m_TempDeleteItem.wIconIdx == 0 ) return;

	CItem* pItem = GetItemofTable(GetTableIdxForAbsPos(m_TempDeleteItem.Position), m_TempDeleteItem.Position);
	if((pItem == 0) || (pItem->GetItemIdx() != m_TempDeleteItem.wIconIdx))
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(153) );
		GAMEIN->GetInventoryDialog()->SetDisable(FALSE);
		return;
	}

	// 071127 LYW --- ItemManager : 삭제 아이템 ( 봉인 관련 ) 위치 저장 변수 추가.
	m_nSealItemDelPos = m_TempDeleteItem.Position ;
			
	MSG_ITEM_DISCARD_SYN msg;
	msg.Category = MP_ITEM;
	msg.Protocol = MP_ITEM_DISCARD_SYN;
	msg.dwObjectID = gHeroID;
	msg.TargetPos =m_TempDeleteItem.Position;
	msg.wItemIdx = m_TempDeleteItem.wIconIdx;
	msg.ItemNum = m_TempDeleteItem.Durability;

	NETWORK->Send(&msg,sizeof(msg));
	memset(&m_TempDeleteItem, 0, sizeof(ITEMBASE));
}

void CItemManager::Item_TotalInfo_Local( void* pMsg )
{
	if( !pMsg ) return ;

	ITEM_TOTALINFO * msg = (ITEM_TOTALINFO *)pMsg;

	for(int i = 0 ; i < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )) ; ++i)
	{
		GAMEIN->GetInventoryDialog()->AddItem(&msg->Inventory[i]);
	}

	for(DWORD i = 0 ; i < eWearedItem_Max ; ++i)
	{
		GAMEIN->GetInventoryDialog()->AddItem(&msg->WearedItem[i]);
	}

	QUICKMGR->RefreshQickItem();
}


void CItemManager::Item_Storage_Item_Info_Ack( void* pMsg )
{
	SEND_STORAGE_ITEM_INFO * pmsg = (SEND_STORAGE_ITEM_INFO*)pMsg;

	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		switch(AddInfoKind)
		{
		case(CAddableInfoList::ItemOption):
			{
				ITEM_OPTION OptionInfo[SLOT_STORAGE_NUM];
				iter.GetInfoData(&OptionInfo);
				
				for( WORD i = 0; i < pmsg->wOptionCount; ++i )
				{
					ITEM_OPTION& option = OptionInfo[ i ];

					AddOption( option );
				}
			}
			break;
		}
		iter.ShiftToNextData();
	}

	for(int i=0; i<SLOT_STORAGE_NUM; ++i)
	{
		if(pmsg->StorageItem[i].dwDBIdx != 0)
			GAMEIN->GetStorageDialog()->AddItem(&pmsg->StorageItem[i]);
	}
	GAMEIN->GetStorageDialog()->SetStorageMoney(pmsg->money);		
	
	GAMEIN->GetStorageDialog()->SetItemInit(TRUE);
	GAMEIN->GetStorageDialog()->ShowStorageDlg(TRUE) ;

	if(HERO->GetStorageNum())
	{
		GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageWare1);
		GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageWare1);
		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// 스크립트 다이얼로그를 비활성화 한다.
	}
}

void CItemManager::Item_Reinforce_Option_Ack( void* pMsg )
{
	const MSG_ITEM_REINFORCE_OPTION_ACK* message = ( MSG_ITEM_REINFORCE_OPTION_ACK* )pMsg;
	
	AddOption( message->mOption );

	{
		CReinforceDlg* dialog = GAMEIN->GetReinforceDialog();
		ASSERT( dialog );

		dialog->Succeed( *message );
	}

	QUICKMGR->RefreshQickItem();
}


void CItemManager::Item_ReInForce_Ack( void* pMsg )
{
	const MSG_ITEM_REINFORCE_ACK*	message		= ( MSG_ITEM_REINFORCE_ACK* )pMsg;
	
	if( MP_ITEM_REINFORCE_FAILED_ACK == message->Protocol )
	{
		CReinforceDlg* dialog = GAMEIN->GetReinforceDialog();
		ASSERT( dialog );

		dialog->Fail();
	}
	// 071226 LUJ, 강화 옵션이 붙을 때까지만 달아놓자...
	else
	{
		CItem* item = GetItem( message->mSourceItemDbIndex );

		if( item )
		{
			// 080108 LUJ, 서버 응답이 올때까지 다시 강화할 수 없게 한다
			item->SetLock( TRUE );
		}
	}

	CItem*				deletedItem		= 0;
	CInventoryExDialog*	inventoryDialog = GAMEIN->GetInventoryDialog();
	ASSERT( inventoryDialog );

	// 아이템 업데이트
	for( DWORD i = 0; i < message->mSize; ++i )
	{
		const ITEMBASE& updatedItem = message->mItem[ i ];

		CItem* item	= GetItem( updatedItem.dwDBIdx );

		if( item )
		{
			if( updatedItem.wIconIdx )
			{
				cImage image;

				item->SetBasicImage( GetIconImage( updatedItem.wIconIdx, &image ) );
				item->SetItemBaseInfo( updatedItem );
				item->SetVisibleDurability( IsDupItem( updatedItem.wIconIdx ) );
				item->SetData( updatedItem.wIconIdx );

				AddToolTip( item );
			}
			else
			{
				DeleteItem( item->GetPosition(), &deletedItem );

				//inventoryDialog->DeleteItem( updatedItem.Position, &deletedItem );
			}

			item->SetLock( FALSE );
		}
		// 071218 LUJ,	클라이언트에 없는 아이템이면 임시에 생성해 놓고, DB 인덱스가 서버에서 올때까지 잠궈놓는다
		//				DB랙으로 인해 아이템이 사라졌다고 오해하거나, 해당 공간을 사용하는 것을 막기 위함이다
		else
		{
			inventoryDialog->AddItem(
				MakeNewItem(&updatedItem, ""));
		}
	}
}


void CItemManager::Item_ReInForce_Nack( void* pMsg )
{
	{
		CReinforceDlg* dialog = GAMEIN->GetReinforceDialog();
		ASSERT( dialog );

		dialog->Restore();
	}

	const MSG_ITEM_ERROR* message = ( MSG_ITEM_ERROR* )pMsg;

	switch( message->ECode )
	{
	case eLog_ItemReinforceDataHaveTooManyMaterials:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Data is too big" );
			break;
		}
	case eLog_ItemReinforceMaterialIsInvalid:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Material data is invalid" );
			break;
		}
	case eLog_ItemReinforceWhichMaterialHasZero:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Some material has zero" );
			break;
		}
	case eLog_ItemReinfoceYouCannotOver100ToReinforce:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: You cannot over 100 materials to reinforce" );
			break;
		}
	case eLog_ItemReinforceSourceItemNotExist:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Source item not exist" );
			break;
		}
	case eLog_ItemRerinfoceServerHasNoReinforceScript:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Server has no reinforce script" );
			break;
		}
	case eLog_ItemReinforceMaterialSourceItemCannnotReinforce:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Source item cannot reinforce" );
			break;
		}
	case eLog_ItemReinforceSourceItemCanDuplicate:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Source item can duplicate" );
			break;
		}
	case eLog_ItemReinforceRemovingMaterialIsFailed:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Removing material is failed" );
			break;
		}
	case eLog_ItemReinforceUpdatingItemIsFailed:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Update item is failed" );
			break;
		}
		// 080929 LUJ, 보조 아이템이 잘못됨
	case eLog_ItemReinforceInvalidSupportItem:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Support item is invalid" );
			break;
		}
		// 080929 LUJ, 보조 스크립트가 없음
	case eLog_ItemReinforceNoSupportScript:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "There is no support script" );
			break;
		}
		// 080929 LUJ, 보조 아이템에 적절한 재료가 아님
	case eLog_ItemReinforceInvalidMaterialForSupportItem:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Some Material is invalid for support item" );
			break;
		}
	default:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Undefined result detected" );
			break;
		}
	}
}


void CItemManager::Item_Move_Ack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	MoveItem( (MSG_ITEM_MOVE_ACK *)pMsg );			
	
	WINDOWMGR->DragWindowNull();
}

void CItemManager::Item_Pet_Move_Ack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	PetMoveItem( (MSG_ITEM_MOVE_ACK *)pMsg );			
	
	WINDOWMGR->DragWindowNull();
}

void CItemManager::Item_Combine_Ack( void* pMsg )
{
	CombineItem( (MSG_ITEM_COMBINE_ACK *)pMsg );	

	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	
	WINDOWMGR->BackDragWindow();
}


void CItemManager::Item_Divide_Ack( void* pMsg )
{
	CInventoryExDialog * pInven = GAMEIN->GetInventoryDialog();
	SetDisableDialog(FALSE, eItemTable_Inventory); 
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);

	ITEMOBTAINARRAY * pmsg = (ITEMOBTAINARRAY *)pMsg;

	for(WORD i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		ITEMBASE* const dividedItem = pmsg->GetItem(i);

		if(const CItem* const originalItem = GetItem(dividedItem->ItemParam))
		{
			dividedItem->nRemainSecond = originalItem->GetItemBaseInfo().nRemainSecond;
		}

		pInven->AddItem(
			MakeNewItem(dividedItem, "MP_ITEM_DIVIDE_ACK"));
	}
			
	//---KES ItemDivide Fix 071020
	GAMEIN->GetInventoryDialog()->UpdateItemDurabilityAdd( GetDivideItemInfoPos(), -GetDivideItemInfoDur() );
	//----------------------------

	// QuickDur다시 계산
	QUICKMGR->RefreshQickItem();

	WINDOWMGR->DragWindowNull();
}


void CItemManager::Item_Monster_Obtain_Notify( void* pMsg )
{
	ITEMOBTAINARRAY* const pmsg	= (ITEMOBTAINARRAY*)pMsg;
	CItem* pItem = NULL;
	
	for(int i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		ITEMBASE& itemBase = pmsg->ItemInfo[ i ];
		const ITEM_INFO* const pInfo = GetItemInfo(
			itemBase.wIconIdx);

		if(NULL == pInfo)
		{
			continue;
		}

		pItem = GAMEIN->GetInventoryDialog()->GetItemForPos( itemBase.Position );
		
		ITEMBASE prevItemBase;
		ZeroMemory( &prevItemBase, sizeof( prevItemBase ) );

		if( pItem )
		{
			if( pItem->GetDBIdx() == itemBase.dwDBIdx )
			{
				prevItemBase = pItem->GetItemBaseInfo();
			}
			
			pItem->SetItemBaseInfo( itemBase ) ;
		}
		else
		{
			pItem = MakeNewItem( &itemBase, "MP_ITEM_MONSTER_OBTAIN_NOTIFY" ) ;

			// 서버에서 전송된 쓰레기 값으로 인해 생성되지 않을 경우가 있다. ITEMOBTAINARRAY가 초기화되어있지 않은 경우라고 추측하고 있음
			if(0 == pItem)
			{
				continue;
			}
			else if( pInfo->wSeal == eITEM_TYPE_SEAL )
			{
				itemBase.nSealed = eITEM_TYPE_SEAL ;
				itemBase.nRemainSecond = pInfo->dwUseTime ;

				pItem->SetItemBaseInfo( itemBase ) ;
			}

			GAMEIN->GetInventoryDialog()->AddItem(pItem);
		}

		// 071220 LUJ, 개수가 음수로 나오는 문제 수정
		if( IsDupItem( itemBase.wIconIdx ) )
		{
			const DURTYPE quantity = abs( int( prevItemBase.Durability ) - int( itemBase.Durability ) );

			// 080526 LUJ, 획득 개수가 0인 경우 표시하지 않도록 함
			if( quantity )
			{
				CHATMGR->AddMsg(
					CTC_GETITEM,
					CHATMGR->GetChatMsg(1112),
					pInfo->ItemName,
					quantity);
			}
		}
		else
		{
			CHATMGR->AddMsg(
				CTC_GETITEM,
				CHATMGR->GetChatMsg(101),
				pInfo->ItemName);
		}
	}

	if( pItem )
	{
		ItemDropEffect( pItem->GetItemIdx() );
	}

	//나누기창이 있으면 지운다.
	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_DIVIDE_INV );
	if( pDlg )
	{
		((cDivideBox*)pDlg)->ExcuteDBFunc( 0 );	//취소버튼 누르기
	}

	QUICKMGR->RefreshQickItem();

	//071022 LYW --- ItemManager : Check item tutorial.
	TUTORIALMGR->Check_GetItem(pItem) ;
}

void CItemManager::Item_HouseRestored( void* pMsg )
{
	//하우징 보관해제시 아이템이 생성되어 온다 그에대한 처리 
	ITEMOBTAINARRAY *	pmsg	= (ITEMOBTAINARRAY *)pMsg;
	CItem*				pItem	= NULL;
		
	for(int i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		ITEMBASE& itemBase = pmsg->ItemInfo[ i ];
		
		ITEMBASE prevItemBase;
		ZeroMemory( &prevItemBase, sizeof( prevItemBase ) );

		pItem = MakeNewItem( &itemBase, "MP_ITEM_HOUSE_RESTORED" ) ;

		// 071127 LYW --- ItemManager : 봉인 아이템 처리.
		const ITEM_INFO* pInfo = GetItemInfo( pItem->GetItemIdx() ) ;

		if( !pInfo ) continue;

		if( !GAMEIN->GetInventoryDialog()->AddItem(pItem) )
		{
			ASSERT(0) ;
		}

		//090527 pdy 하우징 시스템메세지 [보관해제]
		CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 1864 ), pInfo->ItemName );	//1864 "%s가 인벤토리로 이동 하였습니다."
	}

	if( pItem )
	{
		ItemDropEffect( pItem->GetItemIdx() );
	}

	QUICKMGR->RefreshQickItem();
}

//090618 pdy 하우징 꾸미기 보너스 아이템보상 추가 
void CItemManager::Item_HouseBonus( void* pMsg ) 
{
	//하우징 꾸미기 보너스 사용시 아이템보상 타입일경우 아이템이 날라온다. 
	ITEMOBTAINARRAY *	pmsg	= (ITEMOBTAINARRAY *)pMsg;
	CItem*				pItem	= NULL;
		
	for(int i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		ITEMBASE& itemBase = pmsg->ItemInfo[ i ];
		
		ITEMBASE prevItemBase;
		ZeroMemory( &prevItemBase, sizeof( prevItemBase ) );

		pItem = MakeNewItem( &itemBase, "MP_ITEM_HOUSE_BONUS" ) ;

		// 071127 LYW --- ItemManager : 봉인 아이템 처리.
		const ITEM_INFO* pInfo = GetItemInfo( pItem->GetItemIdx() ) ;

		if( !pInfo ) continue;

		if( !GAMEIN->GetInventoryDialog()->AddItem(pItem) )
		{
			ASSERT(0) ;
		}

		CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 101 ), pInfo->ItemName );	
	}

	if( pItem )
	{
		ItemDropEffect( pItem->GetItemIdx() );
	}

	QUICKMGR->RefreshQickItem();
}

void CItemManager::Item_Move_Nack( void* pMsg )
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	
	switch(pmsg->wData)
	{
	case 7:
		break;
	default:
		ASSERT(pmsg->wData == 3);	
	}
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	
	WINDOWMGR->BackDragWindow();
}


void CItemManager::Item_Divite_Nack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	WINDOWMGR->BackDragWindow();
}


void CItemManager::Item_Combine_Nack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	
	WINDOWMGR->BackDragWindow();
}


void CItemManager::Item_Discard_Nack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);

	GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
	MSG_ITEM_ERROR * pmsg = (MSG_ITEM_ERROR*)pMsg;
	if(pmsg->ECode != 4)
		WINDOWMGR->BackDragWindow();
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(799));
}


void CItemManager::Item_Discard_Ack( void* pMsg )
{
	MSG_ITEM_DISCARD_SYN* const pmsg = (MSG_ITEM_DISCARD_SYN*)pMsg;
	const ITEMBASE* pItemBase = GetItemInfoAbsIn(
		HERO,
		pmsg->TargetPos);

	if(0 == pItemBase)
	{
		return;
	}

	QUICKMGR->DeleteLinkdedQuickItem(
		pItemBase->dwDBIdx);
	DeleteItemofTable(
		GetTableIdxForAbsPos(pmsg->TargetPos),
		pmsg->TargetPos,
		FALSE);
	SetDisableDialog(
		FALSE,
		eItemTable_Inventory);
	SetDisableDialog(
		FALSE,
		eItemTable_Storage);
	SetDisableDialog(
		FALSE,
		eItemTable_MunpaWarehouse);
	QUICKMGR->RefreshQickItem();
}


void CItemManager::Item_Use_Notify( void* pMsg )
{
	MSG_DWORDBYTE* pmsg = (MSG_DWORDBYTE*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject( pmsg->dwData );
	if( pObject == NULL ) return;
	if( pObject->GetObjectKind() != eObjectKind_Player ) return;

	CPlayer* pPlayer = (CPlayer*)pObject;

	switch( pmsg->bData )
	{
	case 1:	//생명력 회복 이펙트
		{
			OBJECTEFFECTDESC desc((WORD)FindEffectNum("ItemUSE_PotionHP_little.beff"));
			pPlayer->AddObjectEffect( LIFE_RECOVER_EFFECTID, &desc, 1 );
		}
		break;
	case 2:	//내력 회복 이펙트
		{
			OBJECTEFFECTDESC desc((WORD)FindEffectNum("ItemUSE_PotionMP_little.beff"));
			pPlayer->AddObjectEffect( MANA_RECOVER_EFFECTID, &desc, 1 );
		}
		break;
	case 3:	//올 회복 이펙트
		{
			//---KES 나중엔 이펙트 하나로 만들어서 그걸로 뿌려주자!
			OBJECTEFFECTDESC desc1((WORD)FindEffectNum("ItemUSE_PotionHP_little.beff"));
			pPlayer->AddObjectEffect( LIFE_RECOVER_EFFECTID, &desc1, 1 );
			OBJECTEFFECTDESC desc2((WORD)FindEffectNum("ItemUSE_PotionMP_little.beff"));
			pPlayer->AddObjectEffect( MANA_RECOVER_EFFECTID, &desc2, 1 );
//			OBJECTEFFECTDESC desc(FindEffectNum("ItemUSE_PotionHP_little.beff"));
//			pPlayer->AddObjectEffect( ALL_RECOVER_EFFECTID, &desc, 1 );
		}
		break;
	}
}


void CItemManager::Item_Use_Ack( void* pMsg )
{
	MSG_ITEM_USE_ACK* const message = (MSG_ITEM_USE_ACK*)pMsg;
	const POSTYPE itemPosition = message->TargetPos;
	const eItemUse_Err itemResult = message->eResult;
	
	CItem* const item = GAMEIN->GetInventoryDialog()->GetItemForPos(
		itemPosition);
	
	if( !item )
	{
		return;
	}

	if( item->IsLocked() ) 
	{
		// 090811 ShinJS --- 낚시Dialog에 등록된 미끼 아이템인 경우 Lock을 해제하지 않는다
		CFishingDialog* pFishingDlg = GAMEIN->GetFishingDlg();
		ASSERT( pFishingDlg )
		if( pFishingDlg->GetBaitItemIdx() != item->GetItemIdx() )
			item->SetLock( FALSE ) ;
	}

	if(eItemUseSuccess == itemResult)
	{
		if(AddCoolTime(gHeroID, item->GetItemIdx()))
		{
			CoolTimeScript::ItemData::const_iterator item_it = mCoolTimeScript.mItemData.find( item->GetItemIdx() );

			if( mCoolTimeScript.mItemData.end() == item_it )
			{
				return;
			}

			const CoolTimeScript::Unit& unit = item_it->second;
			const DWORD	itemIndex = item->GetItemIdx();

			CoolTimeScript::GroupData::const_iterator group_it = mCoolTimeScript.mGroupData.find( GetCoolTimeGroupIndex( itemIndex ) );

			if( mCoolTimeScript.mGroupData.end() == group_it )
			{
				return;
			}

			GAMEIN->GetInventoryDialog()->SetCoolTime(
				itemIndex,
				unit.mTime);
			GAMEIN->GetQuickDlg()->GetSlotDlg(0)->SetCoolTime(
				itemIndex,
				unit.mTime);
			GAMEIN->GetQuickDlg()->GetSlotDlg(1)->SetCoolTime(
				itemIndex,
				unit.mTime);
		}
	}

	const ITEM_INFO* info = item->GetItemInfo();

	if(0 == info)
	{
		return;
	}
	else if( eItemCategory_Expend == info->Category )
	{
		//090527 pdy 하우징 가구류는 아이템 사용메세지가 없다.
		if( info->SupplyType != ITEM_KIND_FURNITURE )
			CHATMGR->AddMsg( CTC_EXPENDEDITEM, CHATMGR->GetChatMsg( 1111 ), info->ItemName );
	}
	
	switch(GetTableIdxForAbsPos(itemPosition))
	{
	case eItemTable_Inventory:
	case eItemTable_Weared:
		{
			BOOL bDelItem = FALSE;

			switch(info->Category)
			{
			// LUJ, 재료를 스킬 발동 시 소모한다. 이렇게 해야 소비 처리가 된다
			case eItemCategory_Material:
			case eItemCategory_Expend:
				{
					if( info->SupplyType == ITEM_KIND_PET )
					{
						break;
					}
					else if(eItemUseUnsealed == itemResult)
					{
						break;
					}
					else if(info->Stack)
					{
						item->SetDurability(
							item->GetDurability() - 1);

						if(0 == item->GetDurability())
						{
							CItem* pOutItem = 0;
							DeleteItem(
								itemPosition,
								&pOutItem);

							if( pOutItem )
							{
								bDelItem = TRUE;
								ReLinkQuickPosition( pOutItem );
							}
						}
					}
					else
					{
						const ITEMBASE& pItemBase = item->GetItemBaseInfo() ;
						
						if( pItemBase.nSealed == eITEM_TYPE_UNSEAL )
						{
							QUICKMGR->RefreshQickItem();
							return ;
						}

						CItem* pOutItem = 0;
						DeleteItem(
							itemPosition,
							&pOutItem);

						if( pOutItem )
						{
							bDelItem = TRUE;
							ReLinkQuickPosition( pOutItem );
						}
					}

					switch(info->SupplyType)
					{
					case ITEM_KIND_HP_RECOVER:
					case ITEM_KIND_HP_RECOVER_INSTANT:
						{
							EFFECTMGR->StartHeroEffectProcess(
								FindEffectNum("ItemUSE_PotionHP_little.beff"));
							break;
						}
					case ITEM_KIND_MP_RECOVER:
					case ITEM_KIND_MP_RECOVER_INSTANT:
						{
							EFFECTMGR->StartHeroEffectProcess(
								FindEffectNum("ItemUSE_PotionMP_little.beff"));
							break;
						}
					case ITEM_KIND_HPMP_RECOVER:
					case ITEM_KIND_HPMP_RECOVER_INSTANT:
						{
							EFFECTMGR->StartHeroEffectProcess(
								FindEffectNum("ItemUSE_PotionHP_little.beff"));
							EFFECTMGR->StartHeroEffectProcess(
								FindEffectNum("ItemUSE_PotionMP_little.beff"));
							break;
						}
					case ITEM_KIND_FISHING_BAIT:
						{
							if(bDelItem)
							{
								if(CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForIdx(item->GetItemIdx()))
									GAMEIN->GetFishingDlg()->UpdateBaitItem();
								else
									GAMEIN->GetFishingDlg()->SetBaitItem(NULL);
							}
							else
							{
								GAMEIN->GetFishingDlg()->UpdateBaitItem();
							}

							break;
						}
					case ITEM_KIND_SUMMON_EFFECT:
						{
							EFFECTMGR->StartHeroEffectProcess(
								info->SupplyValue);
							break;
						}
					}
				}
				break;
			default:
				{	
					if( info->SupplyType != ITEM_KIND_COOLTIME )
					{	
						break;
					}	
	
					CItem* deletedItem = 0;
					DeleteItem( item->GetPosition(), &deletedItem );
				}	
				break;
			}	
		}
		break;
	}
	QUICKMGR->RefreshQickItem();
}

void CItemManager::Item_Cook_Ack( void* pMsg )
{
	const MSG_COOK_ACK* message		= ( MSG_COOK_ACK *)pMsg;
	
	CItem*				deletedItem		= 0;
	CInventoryExDialog*	inventoryDialog = GAMEIN->GetInventoryDialog();

	for(size_t i = 0; i < message->mUpdateResult.mSize; ++i)
	{
		const ITEMBASE& updatedItem = message->mUpdateResult.mItem[i];
		CItem* item	= GetItem( updatedItem.dwDBIdx );
		
		if( item )
		{
			const BOOL isDuplicated = IsDupItem( updatedItem.wIconIdx );

			if( updatedItem.wIconIdx )
			{
				cImage image;

				item->SetBasicImage( GetIconImage( updatedItem.wIconIdx, &image ) );
				item->SetItemBaseInfo( updatedItem );
				item->SetVisibleDurability( isDuplicated );
				item->SetData( updatedItem.wIconIdx );

				RefreshItem( item );
			}
			else
			{
				DeleteItem( item->GetPosition(), &deletedItem );
			}
		}
		// 071218 LUJ,	클라이언트에 없는 아이템이면 임시에 생성해 놓고, DB 인덱스가 서버에서 올때까지 잠궈놓는다
		//				DB랙으로 인해 아이템이 사라졌다고 오해하거나, 해당 공간을 사용하는 것을 막기 위함이다
		else
		{
			inventoryDialog->AddItem(
				MakeNewItem(&updatedItem, ""));
		}
	}
}

void CItemManager::Item_Cook_GetItem( void* p )
{
	ITEMOBTAINARRAY*	message = ( ITEMOBTAINARRAY* )p;

	// 인벤토리 갱신
	{
		CInventoryExDialog* dialog	= GAMEIN->GetInventoryDialog();
		ASSERT( dialog );

		for( WORD i = 0; i < message->ItemNum ; ++i )
		{
			const ITEMBASE* itemBase = message->GetItem( i );

			{
				CItem* item = dialog->GetItemForPos( itemBase->Position );

				if( item )
				{
					CItem*	deletedItem	= 0;
					DeleteItem( item->GetPosition(), &deletedItem );
				}
			}

			{
				CItem* item = MakeNewItem( itemBase, "" );

				if( ! dialog->AddItem( item ) )
				{
					ASSERT( 0 );
				}

				item->SetLock( FALSE );
			}
		}	
	}
}

void CItemManager::Item_Mix_Ack( void* pMsg ) 
{
	const MSG_ITEM_MIX_ACK* message		= ( MSG_ITEM_MIX_ACK *)pMsg;
	const BOOL				isFailed	= ( message->Protocol != MP_ITEM_MIX_SUCCEED_ACK );
	CMixDialog*				mixDialog	= GAMEIN->GetMixDialog();

	ASSERT( mixDialog );

	// 결과 알림
	if( isFailed )
	{
		mixDialog->Fail( *message );

		QUICKMGR->RefreshQickItem();
	}
	else
	{
		CItem* item	= GetItem( message->mSourceItemDbIndex );

		if( ! item )
		{
			ASSERT( 0 );
			return;
		}

		const ItemMixResult*	mixResult	= GetMixResult( item->GetItemIdx(), message->mResultIndex );
		const ITEM_INFO*		info		= GetItemInfo( mixResult->mItemIndex );
		ASSERT( info );

		if( info )
		{
			CHATMGR->AddMsg(
				CTC_OPERATEITEM,
				CHATMGR->GetChatMsg( 814 ),
				info->ItemName,
				message->mMixSize );

			QUICKMGR->RefreshQickItem();
		}

		mixDialog->Succeed( *message );
	}
	
	CItem* deletedItem = 0;
	
	for(DWORD i = 0; i < message->mUpdateResult.mSize; ++i)
	{
		const ITEMBASE& updatedItem = message->mUpdateResult.mItem[i];
		CItem* item	= GetItem( updatedItem.dwDBIdx );
		
		if( item )
		{
			const BOOL isDuplicated = IsDupItem( updatedItem.wIconIdx );

			if( updatedItem.wIconIdx )
			{
				cImage image;

				item->SetBasicImage( GetIconImage( updatedItem.wIconIdx, &image ) );
				item->SetItemBaseInfo( updatedItem );
				item->SetVisibleDurability( isDuplicated );
				item->SetData( updatedItem.wIconIdx );

				RefreshItem( item );

				// 100422 ShinJS --- 인벤토리 아이템 변경시 퀘스트 정보를 갱신한다.
				GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
				GAMEIN->GetQuestDialog()->RefreshQuestList();
				QUESTMGR->ProcessNpcMark();
			}
			else
			{
				DeleteItem( item->GetPosition(), &deletedItem );
			}
		}
		// 071218 LUJ,	클라이언트에 없는 아이템이면 임시에 생성해 놓고, DB 인덱스가 서버에서 올때까지 잠궈놓는다
		//				DB랙으로 인해 아이템이 사라졌다고 오해하거나, 해당 공간을 사용하는 것을 막기 위함이다
		else
		{
			GAMEIN->GetInventoryDialog()->AddItem(
				MakeNewItem(&updatedItem, ""));
		}
	}

	// 090818 ShinJS --- 창이 열려있는 경우 정보를 갱신한다
	if( mixDialog->IsActive() )
	{
		mixDialog->UpdateCategory( eMixDlgCategoryType_4 );
	}
}


void CItemManager::Item_Mix_GetItem( void* p )
{
	ITEMOBTAINARRAY*	message = ( ITEMOBTAINARRAY* )p;

	// 인벤토리 갱신
	{
		CInventoryExDialog* dialog	= GAMEIN->GetInventoryDialog();
		ASSERT( dialog );

		for( WORD i = 0; i < message->ItemNum ; ++i )
		{
			const ITEMBASE* itemBase = message->GetItem( i );

			{
				CItem* item = dialog->GetItemForPos( itemBase->Position );

				if( item )
				{
					CItem*	deletedItem	= 0;

					//dialog->DeleteItem( itemBase->Position, 0 );
					DeleteItem( item->GetPosition(), &deletedItem );
				}
			}

			{
				CItem* item = MakeNewItem( itemBase, "" );

				if( ! dialog->AddItem( item ) )
				{
					ASSERT( 0 );
				}

				item->SetLock( FALSE );
			}
		}	
	}

	// 080227 LUJ, 조합창 갱신. 결과를 한 종류만 처리할 수 있도록 설계되어 있음을 유의하자
	{
		CMixDialog* dialog = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );

		if( dialog )
		{
			const ITEMBASE* itemBase = message->GetItem( 0 );

			if( itemBase )
			{
				dialog->RefreshResult( *itemBase );
			}
		}
	}
}

void CItemManager::Item_Mix_Nack( void* pMsg )
{
	const MSG_DWORD*	message = ( MSG_DWORD* )	pMsg;		

	switch( eLogitemmoney( message->dwData ) )
	{
	case eLog_ItemMixInsufficentSpace:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 910 ) );
			break;
		}
	case eLog_ItemMixSystemFault:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	case eLog_ItemMixNotFound:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 784 ) );
			break;
		}
	case eLog_itemMixBadRequest:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1276 ) );
			break;
		}
	case eLog_ItemMixBadCondition:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1277 ) );
			break;
		}
	case eLog_ItemMixBadQuantity:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1278 ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	}
}


void CItemManager::Item_Working_Success( void* pMsg ) 
{
	const MSG_DWORD*	message		= ( MSG_DWORD* )pMsg;
	const DWORD			playerIndex = message->dwData;
	CObject*			object		= OBJECTMGR->GetObject( playerIndex );

	// 090911 ShinJS --- 이동중인 경우 이펙트, 모션 출력하지 않도록 수정
	if( object && !MOVEMGR->IsMoving( object ) && !HERO->IsGetOnVehicle() )
	{
		OBJECTEFFECTDESC desc( 28 );

		object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
		object->RemoveObjectEffect( ITEM_WORKING_SUCCESS_EFFECT );
		object->AddObjectEffect( ITEM_WORKING_SUCCESS_EFFECT, &desc, 1 );
	}
}

void CItemManager::Item_Working_Fail( void* pMsg )
{
	const MSG_DWORD*	message		= ( MSG_DWORD* )pMsg;
	const DWORD			playerIndex = message->dwData;
	CObject*			object		= OBJECTMGR->GetObject( playerIndex );

	// 090911 ShinJS --- 이동중인 경우 이펙트, 모션 출력하지 않도록 수정
	if( object && !MOVEMGR->IsMoving( object ) && !HERO->IsGetOnVehicle() )
	{
		OBJECTEFFECTDESC desc( 29 );

		object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
		object->RemoveObjectEffect( ITEM_WORKING_FAIL_EFFECT );
		object->AddObjectEffect( ITEM_WORKING_FAIL_EFFECT, &desc, 1 );
	}
}

void CItemManager::Item_Working_Start( void* pMsg )
{
	const MSG_DWORD*	message		= ( MSG_DWORD* )pMsg;
	const DWORD			playerIndex = message->dwData;
	CObject*			object		= OBJECTMGR->GetObject( playerIndex );

	// 090911 ShinJS --- 이동중인 경우 이펙트, 모션 출력하지 않도록 수정
	if( object && !MOVEMGR->IsMoving( object ) && !HERO->IsGetOnVehicle() )
	{
		OBJECTEFFECTDESC desc( 30 );

		object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
		object->AddObjectEffect( ITEM_WORKING_EFFECT, &desc, 1 );
	}
}

void CItemManager::Item_Working_StartEx( void* pMsg )
{
	const MSG_DWORD2*	message		= ( MSG_DWORD2* )pMsg;
	const DWORD			playerIndex = message->dwData1;
	CObject*			object		= OBJECTMGR->GetObject( playerIndex );

	// 090911 ShinJS --- 이동중인 경우 이펙트, 모션 출력하지 않도록 수정
	if( object && !MOVEMGR->IsMoving( object ) && !HERO->IsGetOnVehicle() )
	{
		OBJECTEFFECTDESC desc;

		switch(message->dwData2)
		{
		case CProgressDialog::eActionExSummonEffect:
				desc.SetDesc(/*FindEffectNum("E_W_stable_01.beff")*/ 35);
				object->RemoveObjectEffect( ITEM_SUMMON_EFFECTACTIONID );
				object->AddObjectEffect( ITEM_SUMMON_EFFECTACTIONID, &desc, 1 );
			break;

		case CProgressDialog::eActionExCooking:
				desc.SetDesc(/*FindEffectNum("E_M_cook_01.beff")*/ 37);
				object->RemoveObjectEffect( COOKING_EFFECT );
				object->AddObjectEffect( COOKING_EFFECT, &desc, 1 );

				OBJECTSTATEMGR->StartObjectState(object, eObjectState_Cooking);
			break;

		// 091106 ShinJS --- 탈것 소환/해제시 다른 이펙트 적용
		case CProgressDialog::eActionVehicleSummon:
		case CProgressDialog::eActionVehicleSeal:
				desc.SetDesc( 9454 );
				object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
				object->AddObjectEffect( ITEM_WORKING_EFFECT, &desc, 1 );
			break;
		default:
				desc.SetDesc(30);
				object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
				object->AddObjectEffect( ITEM_WORKING_EFFECT, &desc, 1 );
			break;
		}
	}
}

void CItemManager::Item_Working_Stop( void* pMsg )
{
	const MSG_DWORD*	message		= ( MSG_DWORD* )pMsg;
	const DWORD			playerIndex = message->dwData;
	CObject*			object		= OBJECTMGR->GetObject( playerIndex );

	if( object )
	{
		object->RemoveObjectEffect( ITEM_WORKING_EFFECT );
	}
}

void CItemManager::Item_TimeLimit_Item_OneMinute( void* pMsg )
{
	if( !pMsg ) return ;
	
	MSG_DWORD2* pmsg = NULL ;
	pmsg = (MSG_DWORD2*)pMsg ;

	if( !pmsg ) return ;

	ITEM_INFO* pItemInfo = NULL ;
	pItemInfo = GetItemInfo( pmsg->dwData1 ) ;

	if( !pItemInfo ) return ;

	POSTYPE pos = (POSTYPE)pmsg->dwData2 ;

	const ITEMBASE* pItemBase = GetItemInfoAbsIn( HERO, pos ) ;

	if( pItemBase )
	{
		CItem* pItem = NULL ;
		pItem = GetItem( pItemBase->dwDBIdx ) ;

		if( pItem )
		{
			pItem->SetItemBaseInfo( *pItemBase ) ;
			pItem->SetLastCheckTime(gCurTime) ;
			pItem->SetRemainTime( 50 ) ;
		}
	}

	const eITEMTABLE wTableIdx = GetTableIdxForAbsPos( pos ) ;

	if( wTableIdx == eItemTable_Inventory )
	{
		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1191 ), CHATMGR->GetChatMsg( 1189 ), pItemInfo->ItemName ) ;
	}
	else if( wTableIdx == eItemTable_Storage )
	{
		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1191 ), CHATMGR->GetChatMsg( 1188 ), pItemInfo->ItemName ) ;
	}
	else if( wTableIdx == eItemTable_Weared )
	{
		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1191 ), CHATMGR->GetChatMsg( 1190 ), pItemInfo->ItemName ) ;
	}
}

void CItemManager::Item_ShopItem_Use_Ack( void* pMsg )
{
	MSG_ITEM* pmsg = (MSG_ITEM*)pMsg;
	CItem* const pItem = GAMEIN->GetInventoryDialog()->GetItemForPos(
		pmsg->ItemInfo.Position);

	if(0 == pItem)
	{
		return;
	}

	pItem->SetItemBaseInfo(
		pmsg->ItemInfo);
	pItem->SetLastCheckTime(
		gCurTime);

	AddToolTip(
		pItem);
	QUICKMGR->UnsealQuickItem(
		pmsg->ItemInfo.dwDBIdx);
	QUICKMGR->RefreshQickItem();

	CHATMGR->AddMsg(
		CTC_CHEAT_1,
		CHATMGR->GetChatMsg(366));
	CHATMGR->AddMsg(
		CTC_CHEAT_1, 
		CHATMGR->GetChatMsg(1176),
		pItem->GetItemInfo()->ItemName);
}

void CItemManager::Item_Enchant_Success_Ack( void* pMsg )
{
	const MSG_ITEM_ENCHANT_ACK* message = ( MSG_ITEM_ENCHANT_ACK* )pMsg;

	{
		CEnchantDialog* dialog = ( CEnchantDialog* )WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	);
		ASSERT( dialog );

		dialog->Succeed( *message );
	}

	QUICKMGR->RefreshQickItem();
}


void CItemManager::Item_Enchant_Nack( void* pMsg )
{
	const MSG_DWORD* message = ( MSG_DWORD* )pMsg;

	switch( eLogitemmoney( message->dwData ) )
	{
	case eLog_ItemEnchantItemInvalid:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, "Report to LUNA homepage, please: Enchanting item is invalid" );
			break;
		}
	case eLog_ItemEnchantServerError:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	case eLog_ItemEnchantHacking:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, "Report to LUNA homepage, please: your apply to server was abnormal" );
			break;
		}
	case eLog_ItemEnchantItemUpdateFailed:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, "Report to LUNA homepage, please: After enchanting update data is failed" );
			break;
		}
	// 090713 ShinJS --- 인챈트 골드소비 추가에 대한 소지금 부족 오류
	case eLog_ItemEnchantNotEnoughMoney:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 324 ) );
			break;
		}
	default:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	}

	{
		CEnchantDialog* dialog = ( CEnchantDialog* )WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	);
		ASSERT( dialog );

		dialog->Restore();
	}
}


void CItemManager::Item_Enchant_Failed_Ack( void* pMsg )
{
	CEnchantDialog* dialog = ( CEnchantDialog* )WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	);
	ASSERT( dialog );

	dialog->Fail();
}


void CItemManager::Item_Use_Nack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);

	const MSG_ITEM_ERROR* const pmsg = (MSG_ITEM_ERROR*)pMsg;

	switch(pmsg->ECode)
	{
	case eItemUseErr_PreInsert :
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1279));
		}
		break ;

	case eItemUseErr_FullInven : 
		{
			// 091214 ONS Full인벤관련 에러 메세지번호 변경
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1975) ) ;
		}
		break ;

		// 080904 LYW --- ItemManager : 공성중 사용할 수 없다는 에러 처리.
   	case eItemUseErr_SiegeWarfareForbiddenItem :
   		{
   			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(91)) ;
   		}
   		break ;
	case eItemUseErr_ForbiddenSummon:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1737)) ;
		}
		break;
	case eItemUseErr_InvalidCookGrade:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1794)) ;
		}
		break;
	case eItemUseErr_LowerLevelForEat:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1795)) ;
		}
		break;
	case eItemUseErr_AlreadyLearnRecipe:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1796)) ;
		}
		break;
	case eItemUseErr_FullLearnRecipe:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1797)) ;
		}
		break;
	// 100218 ONS 주민등록 초기화 아이템 사용 실패 에러 추가
	case eItemUseErr_NotRegist:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1859)) ;
		}
		break;
	// 100607 플레이어 스턴 상태일때 아이템 사용시 에러추가
	case eltemUseErr_Stun:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2264) );
		}
		break;
	case eItemUseErr_PlayerIsGod:
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1350));
		}
		break;
	case eItemUseErr_SkillCanNotStart:
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1351));
		}
		break;
	default :
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(772));
#ifdef _GMTOOL_
			CHATMGR->AddMsg(CTC_SYSMSG, "Item use error. rt:%d", pmsg->ECode);
#endif
		}
		break ;
	}
}


void CItemManager::Item_Sell_Ack( void* pMsg )
{
	const MSG_ITEM_SELL_ACK* const pmsg = (MSG_ITEM_SELL_ACK*)pMsg;
	CItem* pItem = NULL;
	
	switch(GetTableIdxForAbsPos( pmsg->TargetPos ))
	{
	case eItemTable_Inventory :
		{
			// 인벤토리 정보를 받는다.
			CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;

			// 인벤토리 유효성 체크.
			if( !pInvenDlg )
			{
				#ifdef _GMTOOL_
				MessageBox( NULL, "Invalid inventory dialog!!", __FUNCTION__, MB_OK ) ;
				#endif //_GMTOOL_
				return ;
			}

			// 아이템 정보를 받는다.
			pItem = pInvenDlg->GetItemForPos( pmsg->TargetPos ) ;
		}
		break ;

	case eItemTable_Storage :
		{
			// 창고 정보를 받는다.
			CStorageDialog* pStorageDlg = GAMEIN->GetStorageDialog() ;

			// 창고 유효성 체크.
			if( !pStorageDlg )
			{
				#ifdef _GMTOOL_
				MessageBox( NULL, "Invalid storage dialog!!", __FUNCTION__, MB_OK ) ;
				#endif //_GMTOOL_
				return ;
			}

			// 아이템 정보를 받는다.
			pItem = pStorageDlg->GetItemForPos( pmsg->TargetPos ) ;
		}
		break ;

	default : return ;
	}


	// 아이템의 유효성을 확인한다.
	if( !pItem )
	{
		#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid item position!!", __FUNCTION__, MB_OK ) ;
		#endif //_GMTOOL_
		return ;
	}


	// 아이템 정보를 받는다.
	const ITEM_INFO* pSellItemInfo = pItem->GetItemInfo() ;

	if( !pSellItemInfo )
	{
		#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid item info!!", __FUNCTION__, MB_OK ) ;
		#endif //_GMTOOL_
		return ;
	}


	// 스택 아이템인지, 단일 아이템인지 확인한다.
	if( IsDupItem( pmsg->wSellItemIdx ) && ( pItem->GetDurability() - pmsg->SellItemNum ) > 0 )
	{
		// 1. 아이템 수를 업데이트한다.
		pItem->SetDurability( pItem->GetDurability() - pmsg->SellItemNum ) ;
		DWORD SellPrice = pSellItemInfo->SellPrice;

		CHATMGR->AddMsg( CTC_SELLITEM, CHATMGR->GetChatMsg( 39 ), pSellItemInfo->ItemName, pmsg->SellItemNum, AddComma( SellPrice*pmsg->SellItemNum ) ) ;

		// 4. 퀵슬롯을 새로고침한다.
		QUICKMGR->RefreshQickItem() ;
	}
	else
	{
		// 1. 판매 가격을 받는다.
		DWORD SellPrice = pSellItemInfo->SellPrice;

		// 2. 판매 메시지를 출력한다.
		if( pmsg->SellItemNum == 0 || ! IsDupItem( pItem->GetItemIdx() ) )
		{
			CHATMGR->AddMsg( CTC_SELLITEM, CHATMGR->GetChatMsg( 38 ), pSellItemInfo->ItemName, AddComma( SellPrice ) );
		}
		else
		{
			CHATMGR->AddMsg( CTC_SELLITEM, CHATMGR->GetChatMsg( 39 ), pSellItemInfo->ItemName, pmsg->SellItemNum, AddComma( SellPrice*pmsg->SellItemNum ) );
		}

		// 3. 아이템 삭제 처리를 한다.
		// 071202 LYW --- ItemManager : 봉인 아이템이 삭제 될 때, 순수 삭제될 때와 상점에 팔 때에도 
		// 같은 이 함수를 사용하기 때문에, 함수 뒤쪽 인자로 팔때인지 삭제인지 여부를 체크하도록 수정함.
		DeleteItemofTable( GetTableIdxForAbsPos( pmsg->TargetPos ), pmsg->TargetPos, TRUE ) ;
		QUICKMGR->DeleteLinkdedQuickItem( pItem->GetDBIdx() ) ;
	}


	// 강제 Disable 시켰던 다이얼로그들을 해제한다.
	
	// 1. 상점 다이얼로그를 체크.
	CDealDialog* pDealDlg = GAMEIN->GetDealDialog() ;

	if( !pDealDlg )
	{
		#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid deal dialog!!", __FUNCTION__, MB_OK ) ;
		#endif //_GMTOOL_
		return ;
	}

	if( pDealDlg->IsActive() ) pDealDlg->SetDisable( FALSE ) ;

	
	// 2. 인벤토리 다이얼로그 체크.
	CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;

	if( !pInvenDlg )
	{
		#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid inventory dialog!!", __FUNCTION__, MB_OK ) ;
		#endif //_GMTOOL_
		return ;
	}

	if( pInvenDlg->IsActive() ) pInvenDlg->SetDisable( FALSE ) ;

	// 3. 창고 다이얼로그 체크.
	CStorageDialog* pStorageDlg = GAMEIN->GetStorageDialog() ;

	// 창고 유효성 체크.
	if( !pStorageDlg )
	{
		#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid storage dialog!!", __FUNCTION__, MB_OK ) ;
		#endif //_GMTOOL_
		return ;
	}

	if( pStorageDlg->IsActive() ) pStorageDlg->SetDisable( FALSE ) ;


	// 퀵 슬롯을 새로고침한다.
	QUICKMGR->RefreshQickItem() ;
}


void CItemManager::Item_Sell_Nack( void* pMsg )
{
	// 090107 LYW --- ItemManager : 아이템 판매 실패 처리 메시지 추가.


	// 함수 인자 확인.
	if( !pMsg ) return ;
    

	// 원본 메시지 변환.
	MSG_ITEM_ERROR* pmsg = (MSG_ITEM_ERROR*)pMsg ;
	if( !pmsg ) return ;


	// 에러 메시지 확인
	switch( pmsg->ECode )
	{
	case ERR_NOTSELLITEM :
	case ERR_INVALSLOT :
	case ERR_INVALITEMINFO :
	case ERR_INVALPLAYER :
	case ERR_LOOTPLAYER :
	case ERR_INVALNPC :
	case ERR_INVALIDENTITY :
	case ERR_OVERSTACKCOUNT :
	case ERR_FAILDISCARD :
		break ;

	case ERR_MAXMONEY :
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(459) ) ;
		break ;

	case ERR_NOTINVENITEM :
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1809 ) ) ;
		break ;
	}


	// 활성화 중인 창들이 Disable되어 있으면 해제한다.

	// 인벤토리 다이얼로그 처리.
	CInventoryExDialog* pInven = GAMEIN->GetInventoryDialog() ;
	if( !pInven )
	{
#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid inventory dialog!!", __FUNCTION__, MB_OK ) ;
#endif //_GMTOOL_
	}
	else
	{
		if( pInven->IsActive() ) pInven->SetDisable( FALSE ) ;
	}

	// 상점 다이얼로그 처리.
	CDealDialog* pDealDlg = GAMEIN->GetDealDialog() ;
	if( !pDealDlg )
	{
#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid deal dialog!!", __FUNCTION__, MB_OK ) ;
#endif //_GMTOOL_
	}
	else
	{
		if( pDealDlg->IsActive() ) pDealDlg->SetDisable( FALSE ) ;
	}

	// 창고 소환 처리 예외 추가.
	CStorageDialog* pStorageDlg = GAMEIN->GetStorageDialog() ;
	if( !pStorageDlg )
	{
#ifdef _GMTOOL_
		MessageBox( NULL, "Invalid storage dialog!!", __FUNCTION__, MB_OK ) ;
#endif //_GMTOOL_
	}
	else
	{
		if( pStorageDlg->IsActive() ) pStorageDlg->SetDisable( FALSE ) ;
	}

}


void CItemManager::Item_Buy_Ack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Deal);
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);

	ITEMOBTAINARRAY * pmsg = (ITEMOBTAINARRAY *)pMsg;

	CItem* pItem = NULL;
	CItem* pPreItem = NULL;
	for(WORD i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		if( (pPreItem = GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->GetItem(i)->Position )) != NULL)
		{
			DeleteItem(pmsg->GetItem(i)->Position, &pPreItem);
			ASSERT(pPreItem);
		}
		pItem = MakeNewItem(pmsg->GetItem(i),"MP_ITEM_BUY_ACK");	
		BOOL rt = GAMEIN->GetInventoryDialog()->AddItem(pItem);
		if(!rt)
		{
			ASSERT(0);					
		}

		// 090811 ShinJS --- 낚시 미끼 아이템인 경우 낚시Dialog에 등록되어 있는지 판단하여 Update 해준다
		CFishingDialog* pFishingDlg = GAMEIN->GetFishingDlg();
		ASSERT( pFishingDlg )
		if( pFishingDlg->GetBaitItemIdx() == pItem->GetItemIdx() )
		{
			pFishingDlg->RefreshBaitItem();
		}
		
	}
	MONEYTYPE BuyPrice;
	BuyPrice = pItem->GetItemInfo()->BuyPrice;

	if( pmsg->wObtainCount > 1 )
	{
		CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(37), pItem->GetItemInfo()->ItemName, pmsg->wObtainCount,
						AddComma( BuyPrice * pmsg->wObtainCount ) );	//confirm
	}
	else
	{
		CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(36), pItem->GetItemInfo()->ItemName, AddComma( BuyPrice ) );
	}

	QUICKMGR->RefreshQickItem();
}

// 070329 LYW --- ItemManager : Add function to make string.
void CItemManager::CompositString( char* inputStr, char* str, ... )
{
	if( inputStr == NULL ) return ;
	if( str == NULL ) return;

	va_list argList;

	va_start(argList, str);
	vsprintf(inputStr,str,argList);
	va_end(argList);
}


void CItemManager::Item_Buy_Nack( void* pMsg )
{
	MSG_ITEM_ERROR* pmsg = (MSG_ITEM_ERROR*)pMsg;

	//most of all, inventory full process : taiyo
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	SetDisableDialog(FALSE, eItemTable_Deal);
	//GAMEIN->GetDealDialog()->SetDisable( FALSE );

	switch(pmsg->ECode)
	{
	case NOT_MONEY:	
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(324) );
		break;
	case NOT_SPACE:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(474) );
		break;
	case NOT_EXIST:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(476) );
		break;
	case NOT_PLAYER:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(477) );
		break;
	case NOT_FISHINGPOINT:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1533 ) );
		break;
	// 090227 ShinJS --- Coin 부족으로 인한 구입 실패
	case NOT_COIN:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1919 ) );

	// 100104 ShinJS --- PC방 포인트 부족으로 인한 구입 실패
	case NOT_PCROOMPOINT:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2011 ) );
		break;
	// 100105 ShinJS --- PC방 등록업소가 아닌 경우
	case NOT_PCROOM:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2013 ) );
		break;
	default:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(478) );
		break;
	}

	//ASSERT(0);
}

// 090227 ShinJS --- 암상인 추가작업 : Coin Item 구입시 Coin 제거 작업
void CItemManager::Item_Buy_By_Coin( void* pMsg )
{
	if( !pMsg )		return;

	MSG_ITEM_BUY_BY_COIN* pmsg	= (MSG_ITEM_BUY_BY_COIN*)pMsg;
	CItem* deletedItem			= 0;

	// 아이템 업데이트
	for( DWORD i = 0; i < pmsg->mSize; ++i )
	{
		const ITEMBASE& updatedItem = pmsg->mItem[ i ];

		CItem* item	= GetItem( updatedItem.dwDBIdx );
		
		if( item )
		{
			const BOOL isDuplicated = IsDupItem( updatedItem.wIconIdx );

			// Coin Item 수량 변화
			if( updatedItem.wIconIdx )
			{
				cImage image;

				item->SetBasicImage( GetIconImage( updatedItem.wIconIdx, &image ) );
				item->SetItemBaseInfo( updatedItem );
				item->SetVisibleDurability( isDuplicated );
				item->SetData( updatedItem.wIconIdx );

				RefreshItem( item );
			}
			// Coin Item 제거
			else
			{
				DeleteItem( item->GetPosition(), &deletedItem );
			}
		}
	}
}

void CItemManager::Item_Dealer_Ack( void* pMsg )
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg; 
	if( HERO->GetState() != eObjectState_Die)
		GAMEIN->GetDealDialog()->ShowDealer(pmsg->wData);

	// 070329 LYW --- ItemManager : Setting position deal dialog and inventory dialog.
	GAMEIN->GetDealDialog()->ShowDealDialog(TRUE) ;
}

void CItemManager::Item_Appearance_Add( void* p )
{
	const MSG_APPERANCE_ADD*	message		= ( MSG_APPERANCE_ADD* )p;
	const DWORD					playerIndex = message->dwObjectID;

	CPlayer* player = ( CPlayer* )OBJECTMGR->GetObject( playerIndex );

	if( player &&
		player->GetObjectKind() == eObjectKind_Player )
	{
		player->SetWearedItemIdx( message->mSlotIndex, message->mItemIndex );
		APPEARANCEMGR->AddCharacterPartChange( playerIndex );
	}
	else
	{
		ASSERT( 0 );
	}
}


void CItemManager::Item_Appearance_Remove( void* p )
{
	const MSG_APPERANCE_REMOVE*	message		= ( MSG_APPERANCE_REMOVE* )p;
	const DWORD					playerIndex = message->dwObjectID;

	CPlayer* player = ( CPlayer* )OBJECTMGR->GetObject( playerIndex );

	if( player &&
		player->GetObjectKind() == eObjectKind_Player )
	{
		player->SetWearedItemIdx( message->mSlotIndex, 0 );
		APPEARANCEMGR->AddCharacterPartChange( playerIndex );
	}
}


void CItemManager::Item_Money( void* pMsg )
{
	MSG_MONEY* pmsg = (MSG_MONEY*)pMsg;
			
	MONEYTYPE OriMoney = HERO->GetMoney();
	MONEYTYPE PrintMoney = 0;

	switch(pmsg->bFlag)
	{
	case 64:
		{
			PrintMoney = pmsg->dwTotalMoney - OriMoney;
			if( PrintMoney )
			{
				// 070126 LYW --- GETMONEY : Modified message number.
				//CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 120 ), AddComma( PrintMoney ) );
				CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 301 ), AddComma( PrintMoney ) );
				EFFECTMGR->StartHeroEffectProcess(eEffect_GetMoney);
			}
		}
		break;
	case MF_NOMAL:
		{
			break;
		}
	case MF_OBTAIN: // Obtain Money;
		{
			PrintMoney = pmsg->dwTotalMoney - OriMoney;
			if( PrintMoney )
			{
				// 070126 LYW --- GETMONEY : Modified message number.
				//CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 120 ), AddComma( PrintMoney ) );
				CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 301 ), AddComma( PrintMoney ) );
				EFFECTMGR->StartHeroEffectProcess(eEffect_GetMoney);
			}
		}
		break;
	case MF_LOST:
		{
			PrintMoney = OriMoney - pmsg->dwTotalMoney;
			if( PrintMoney )
			{
				// 070126 LYW --- GETMONEY : Modified message number.
				//CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 118 ), AddComma( PrintMoney) );
				CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 378 ), AddComma( PrintMoney) );
			}
		}
		break;
	case MF_FEE:
		{
			PrintMoney = OriMoney - pmsg->dwTotalMoney;
			if( PrintMoney )
			{
				CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg(1601), AddComma( PrintMoney) );
			}
		}
		break;
	default:
		break;
	}

	HERO->SetMoney(pmsg->dwTotalMoney);

	// 조합은 히어로의 소지금/레벨에 영향받는데, 현재 상태에 기반해서 표시해주는 항목이 있으므로 정보가 변경될때마다 갱신이 필요하다
	{
		CMixDialog* dialog = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
		ASSERT( dialog );

		dialog->Refresh();
	}
}


void CItemManager::Item_Obtain_Money( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	DWORD OriMoney = HERO->GetMoney();
	DWORD PrintMoney = pmsg->dwData - OriMoney;

	if( PrintMoney )
	{
		CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 301 ), PrintMoney );
		EFFECTMGR->StartHeroEffectProcess(eEffect_GetMoney);
	}
	
	HERO->SetMoney(pmsg->dwData);
}


void CItemManager::Item_Divide_NewItem_Notify( void* pMsg )
{
	MSG_ITEM* pmsg = (MSG_ITEM*)pMsg;
	CItem* pItem = MakeNewItem(&pmsg->ItemInfo,"MP_ITEM_DIVIDE_NEWITEM_NOTIFY");
	int tbidx = GetTableIdxForAbsPos(pItem->GetItemBaseInfo().Position);

	if(tbidx == eItemTable_Inventory)
		GAMEIN->GetInventoryDialog()->AddItem(pItem);
	else if(tbidx == eItemTable_Storage)
		GAMEIN->GetStorageDialog()->AddItem(pItem);
	else if(tbidx == eItemTable_MunpaWarehouse)
		GAMEIN->GetGuildWarehouseDlg()->AddItem(pItem);

	CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 123 ));
}


void CItemManager::Item_Guild_Move_Ack( void* pMsg )
{
	MoveItemToGuild((MSG_ITEM_MOVE_ACK*)pMsg);
	WINDOWMGR->DragWindowNull();
}


void CItemManager::Item_Guild_Move_Nack( void* pMsg )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	WINDOWMGR->BackDragWindow();
}


void CItemManager::Item_DeleteFrom_GuildWare_Notify( void* pMsg )
{
	MSG_ITEM * pItembase = (MSG_ITEM*)pMsg;
	CItem* pItem = NULL;
	if(GAMEIN->GetGuildWarehouseDlg()->DeleteItem(pItembase->ItemInfo.Position, &pItem) == TRUE)
		ItemDelete(pItem);
	else
	{
		ASSERT(0);
	}
}


void CItemManager::Item_AddTo_GuildWare_Notify( void* pMsg )
{
	const MSG_ITEM_WITH_OPTION * pItem = (MSG_ITEM_WITH_OPTION*)pMsg;

	AddOption( pItem->OptionInfo );
	
	GAMEIN->GetGuildWarehouseDlg()->AddItem(&pItem->ItemInfo);
}


void CItemManager::Item_Storage_Move_Ack( void* pMsg )
{
	MoveItemToStorage( (MSG_ITEM_MOVE_ACK*)pMsg );
	WINDOWMGR->DragWindowNull();
}


void CItemManager::Item_Dissolve_GetItem( void* pMsg )
{
	ITEMOBTAINARRAY* const message = (ITEMOBTAINARRAY*)pMsg;
	
	for(WORD i = 0 ; i < message->ItemNum ; ++i)
	{
		const ITEMBASE* const itemBase = message->GetItem(i);

		CItem* deletedItem = 0;
		DeleteItem(
			itemBase->Position,
			&deletedItem);

		GAMEIN->GetInventoryDialog()->AddItem(
			MakeNewItem(itemBase, ""));
	}
}


void CItemManager::Item_Dissolve_Ack( void* pMsg )
{
	const MSG_ITEM_DISSOLVE_ACK* message = ( MSG_ITEM_DISSOLVE_ACK *)pMsg;
	GAMEIN->GetDissolveDialog()->Succeed( *message );
	
	for(DWORD i = 0; i < message->mUpdateResult.mSize; ++i)
	{
		const ITEMBASE& itemBase = message->mUpdateResult.mItem[i];

		RemoveOption(
			itemBase.dwDBIdx);

		CItem* const inventoryItem = GetItem(
			itemBase.dwDBIdx);

		if(0 == inventoryItem)
		{
			CItem* const item = MakeNewItem(
				&itemBase,
				"");

			if(0 == item)
			{
				continue;
			}

			item->SetLock(
				TRUE);

			GAMEIN->GetInventoryDialog()->AddItem(
				item);
			continue;
		}
		else if(0 == itemBase.wIconIdx)
		{
			CItem* deletedItem = 0;
			DeleteItem(
				inventoryItem->GetPosition(),
				&deletedItem);
			continue;
		}

		cImage image;
		inventoryItem->SetBasicImage(
			GetIconImage(itemBase.wIconIdx, &image));
		inventoryItem->SetItemBaseInfo(
			itemBase);
		inventoryItem->SetVisibleDurability(
			IsDupItem(itemBase.wIconIdx));
		inventoryItem->SetDurability(
			itemBase.Durability);
		inventoryItem->SetData(
			itemBase.wIconIdx);
		inventoryItem->SetLock(
			FALSE);

		AddToolTip(
			inventoryItem);
	}
}


void CItemManager::Item_Dissolve_Nack( void* pMsg )
{
	const MSG_DWORD* message = ( MSG_DWORD* )pMsg;

	switch( eLogitemmoney( message->dwData ) )
	{
	case eLog_ItemDissolvePlayerNotExist:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Server have no such player data" );
			break;
		}
	case eLog_ItemDissolveItemDataNotMatched:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Server item data do not matched with yours" );
			break;
		}
	case eLog_ItemDissolveServerHaveNoDissolvingData:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Server has no data about it's dissolution" );
			break;
		}
	case eLog_ItemDissolveItNeedMoreLevel:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: You need more level to dissolve it" );
			break;
		}
	case eLog_ItemDissolveRemovingItemIsFailed:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Removing item is failed" );
			break;
		}
	case eLog_ItemDissolveAddingItemIsFailed:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: Adding item is failed" );
			break;
		}
	case eLog_ItemDissolveInsufficentSpace:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 839 ) );
			break;
		}
	case eLog_ItemDissolveNoResult:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: No result remained" );
			break;
		}
	case eLog_ItemDissolvePlayserHasNoInventory:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: You have no inventory" );
			break;
		}
	case eLog_ItemDissolveScriptInvalid:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "Report to LUNA homepage, please: dissolving script is invalid" );
			break;
		}
	case eLog_ItemDissolveServerError:
	case eLog_ItemDissolveSourceItemNotExist:
	default:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1275 ) );
			break;
		}
	}
}


void CItemManager::Item_Error_Nack( void* p )
{
	SetDisableDialog(FALSE, eItemTable_Inventory);
	SetDisableDialog(FALSE, eItemTable_Storage);
	SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);

	WINDOWMGR->BackDragWindow();

	const MSG_ITEM_ERROR* message = ( MSG_ITEM_ERROR* )p;

	switch( message->ECode )
	{
	case eItemUseErr_PreInsert:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1279 ) );
			break;
		}
	case eItemUseErr_Discard:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1280 ) );
			break;
		}
	case eItemUseErr_AlreadyUse:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1281 ) );
			break;
		}
#ifdef _GMTOOL_
	default:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Item error. rt: %d",
				message->ECode);
			break;
		}
#endif
	}
}


void CItemManager::Item_ChangeItem_Nack( void* pMsg )
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(354) );
}


void CItemManager::Item_Use_ChangeItem_Ack( void* pMsg )
{
	ITEMOBTAINARRAY* pmsg = (ITEMOBTAINARRAY *)pMsg;
		
	CItem* pItem = NULL;

	for(WORD i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		pItem								= GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->GetItem(i)->Position );
		const DURTYPE	previousQuantity	= ( pItem ? pItem->GetDurability() : 0 );

		// 071127 LYW --- ItemManager : 아이템 습득 처리에 봉인 관련 처리 추가.
		ITEMBASE* pItemBase = pmsg->GetItem(i) ;

		if( !pItemBase ) return ;

		if( pItem )
		{
			pItem->SetItemBaseInfo(*pItemBase);
		}
		else
		{
			pItem = MakeNewItem(pItemBase,"MP_ITEM_MONSTER_OBTAIN_NOTIFY");	

			ITEM_INFO* pInfo = GetItemInfo( pItem->GetItemIdx() ) ;

			if( !pInfo ) continue ;

			if( pInfo->wSeal == eITEM_TYPE_SEAL )
			{
				pItemBase->nSealed = eITEM_TYPE_SEAL ;
				pItemBase->nRemainSecond = pInfo->dwUseTime ;

				pItem->SetItemBaseInfo( *pItemBase ) ;
			}
			
			if(! GAMEIN->GetInventoryDialog()->AddItem(pItem) )
			{
				ASSERT(0);
			}
		}

		if( IsDupItem( pItem->GetItemIdx() ) )
		{
			CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 1112 ), pItem->GetItemInfo()->ItemName, pItem->GetDurability() - previousQuantity );
		}
		else
		{
			CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 101 ), pItem->GetItemInfo()->ItemName );
		}
	}

	if( pItem )
	{
		ItemDropEffect( pItem->GetItemIdx() );
	}
	
	//³ª´ⓒ±aA￠AI AOA¸¸e Ao¿i´U.
	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_DIVIDE_INV );
	if( pDlg )
	{
		((cDivideBox*)pDlg)->ExcuteDBFunc( 0 );	//Ae¼O¹oÆ° ´ⓒ¸￡±a
	}

	QUICKMGR->RefreshQickItem();
}


void CItemManager::Item_EventItem_Use( void* pMsg )
{
#ifdef TAIWAN_LOCAL
	if( pmsg->dwData2 == 53074 )
	{
		EFFECTMGR->StartPlayerEffectProcess( pPlayer, FindEffectNum("m_ba_079.beff") );
	}

	// 06. 03. 화이트데이 이벤트
	if( pmsg->dwData2 = 53151 )
	{
		EFFECTMGR->StartPlayerEffectProcess( pPlayer, FindEffectNum("m_ba_082.beff") );
	}
#else
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwData1);

	// 06. 03. 화이트데이 이벤트
	if( pmsg->dwData2 == 53151 )
	{
		EFFECTMGR->StartPlayerEffectProcess( pPlayer, FindEffectNum("m_ba_082.beff") );
	}
#endif
}


// 071128 LYW --- ItemManager : 아이템몰 창고 추가 처리.
void CItemManager::Item_ShopItem_Info_Ack( void* pMsg ) 
{
	SEND_SHOPITEM_INFO* pmsg = (SEND_SHOPITEM_INFO*)pMsg;

	if( pmsg->ItemCount )
	{
		GAMEIN->GetItemShopDialog()->SetItemInfo(pmsg);
	}
	else
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1193) );
	}

	GAMEIN->GetItemShopDialog()->SetActive(TRUE);
	GAMEIN->GetInventoryDialog()->SetActive( TRUE );
}





// 071203 LYW --- ItemManager
void CItemManager::Item_Skill_Ack( void* pMsg )
{
	if( !pMsg ) return ;

	Item_Use_Ack(pMsg);
}

// 071204 LYW --- ItemManager :
void CItemManager::Item_Reset_Stats_Ack( void* pMsg )
{
	if( !pMsg ) return ;
			
	SEND_DEFAULT_STATS* pmsg = NULL ;
	pmsg = (SEND_DEFAULT_STATS*)pMsg ;

	if( !pmsg ) return ;

	if( !HERO ) return ;

	HERO->SetStrength((DWORD)pmsg->nStr) ;
	HERO->SetDexterity((DWORD)pmsg->nDex) ;
	HERO->SetVitality((DWORD)pmsg->nVit) ;
	HERO->SetIntelligence((DWORD)pmsg->nInt) ;
	HERO->SetWisdom((DWORD)pmsg->nWis) ;
	GAMEIN->GetCharacterDialog()->SetPointLeveling(TRUE, (WORD)pmsg->nPoint);
	CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1283 ) );
}


// 071208 LYW --- ItemManager : 스탯 초기화 실패 처리.
void CItemManager::Item_Reset_Stats_Nack( void* pMsg ) 
{
	WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg( 1282 ) );	
}


// 071208 LYW --- ItemManager : 맵 이동 실패 처리.
void CItemManager::Item_MapMoveScroll_Nack( void* pMsg )
{
	WINDOWMGR->MsgBox(
		MBI_MAPMOVE_NOTICE,
		MBT_OK,
		CHATMGR->GetChatMsg(56));
}


// 080312 NYJ --- ItemManager : 전체외치기 아이템사용(클릭) 처리.
void CItemManager::UseItem_WorldShout(CItem* pItem)
{
	if( !pItem ) return ;

	pItem->SetLock(TRUE) ;

	CShoutDialog* pDlg = NULL ;
	pDlg = GAMEIN->GetShoutDlg();

	if( !pDlg ) return ;
	pDlg->SetItem(pItem);
	pDlg->SetActive(TRUE) ;
}

// 080319 LUJ, 추가 인챈트 수치를 표시함
// 080320 LUJ, 추가 인챈트 수치를 함수로 가져옴
void CItemManager::AddOptionToolTip( cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* info = GetItemInfo( item.wIconIdx );

	if( !	info ||
			eEquipType_None == info->EquipType )
	{
		return;
	}

	// 성별제한
	switch( info->LimitGender )
	{
	case 1:
		{
			icon.AddToolTipLine( CHATMGR->GetChatMsg( 666 ), TTTC_ITEM_NAME );
			break;
		}
	case 2:
		{
			icon.AddToolTipLine( CHATMGR->GetChatMsg( 667 ), TTTC_ITEM_NAME );
			break;
		}
	}

	// 090708 ONS 신규종족 추가에 따른 아이템 종족제한 처리 수정
	// 종족제한
	switch( info->LimitRace )
	{
	case 0:
		{
			// 공용 아이템
			icon.AddToolTipLine( CHATMGR->GetChatMsg( 1927 ), TTTC_ITEM_NAME );
			break;
		}
	case 1:
		{
			// 휴먼/엘프 전용
			icon.AddToolTipLine( CHATMGR->GetChatMsg( 668 ), TTTC_ITEM_NAME );
			break;
		}
	case 2:
		{
			// 마족 전용
			icon.AddToolTipLine( CHATMGR->GetChatMsg( 669 ), TTTC_ITEM_NAME );
			break;
		}
	}

	char line[ MAX_PATH ];

	// 레벨제한
	if( info->LimitLevel )
	{
		sprintf( line, "%s%d", CHATMGR->GetChatMsg( 670 ), info->LimitLevel );
		icon.AddToolTipLine( line, TTTC_ITEM_NAME );
	}

	// 081231 LUJ, 퍼센트 스탯을 표시하기 위해 구조체 ITEM_OPTION -> PlayerStat으로 변경
	PlayerStat base;
	PlayerStat enchantStat;
	ITEM_OPTION option	= GetOption( item.dwDBIdx );

	// 인챈트된 수치는 괄호안에 표시되지 않는다. 또한 저장될 때는 메모리 절약 및 계산 속도 향상을 위해 옵션 정보와 같이 저장된다.
	// 따라서 일단 수치를 별도로 저장한다. 그리고, 인챈트되었으면 그 만큼을 기본 수치에 더해주고, 옵션 수치에서는 빼준다.
	{
		SetValue( base.mPhysicAttack,	info->PhysicAttack,										SetValueTypeAdd );
		SetValue( base.mPhysicDefense,	info->PhysicDefense,									SetValueTypeAdd );
		SetValue( base.mMagicAttack,	info->MagicAttack,										SetValueTypeAdd );
		SetValue( base.mMagicDefense,	info->MagicDefense,										SetValueTypeAdd );
		SetValue( base.mStrength,		info->ImprovementStr	+ option.mMix.mStrength,		SetValueTypeAdd );
		SetValue( base.mDexterity,		info->ImprovementDex	+ option.mMix.mDexterity,		SetValueTypeAdd );
		SetValue( base.mVitality,		info->ImprovementVit	+ option.mMix.mVitality,		SetValueTypeAdd );
		SetValue( base.mIntelligence,	info->ImprovementInt	+ option.mMix.mIntelligence,	SetValueTypeAdd );
		SetValue( base.mWisdom,			info->ImprovementWis	+ option.mMix.mWisdom,			SetValueTypeAdd );
		SetValue( base.mLife,			info->ImprovementLife,									SetValueTypeAdd );
		SetValue( base.mMana,			info->ImprovementMana,									SetValueTypeAdd );

		if(const EnchantScript* const script = g_CGameResourceManager.GetEnchantScript(option.mEnchant.mIndex))
		{
			const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo(
				option.mEnchant.mLevel);
			const int value = info->EnchantDeterm * option.mEnchant.mLevel;

			g_CGameResourceManager.AddEnchantValue(
				enchantStat,
				script->mAbility,
				value);
			g_CGameResourceManager.AddEnchantValue(
				option,
				script->mAbility,
				-value);

			for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
				script->mAbility.end() != it;
				++it )
			{
				switch( EnchantScript::eType( *it ) )
				{
				case ReinforceScript::eTypeStrength:
					{
						SetValue(
							enchantStat.mStrength,
							GetBonusEnchantValue( *info, option, info->ImprovementStr, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeDexterity:
					{
						SetValue(
							enchantStat.mDexterity,
							GetBonusEnchantValue( *info, option, info->ImprovementDex, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeVitality:
					{
						SetValue(
							enchantStat.mVitality,
							GetBonusEnchantValue( *info, option, info->ImprovementVit, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeWisdom:
					{
						SetValue(
							enchantStat.mWisdom,
							GetBonusEnchantValue( *info, option, info->ImprovementWis, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeIntelligence:
					{
						SetValue(
							enchantStat.mIntelligence,
							GetBonusEnchantValue( *info, option, info->ImprovementInt, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeLife:
					{
						SetValue(
							enchantStat.mLife,
							GetBonusEnchantValue( *info, option, info->ImprovementLife, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeMana:
					{
						SetValue(
							enchantStat.mMana,
							GetBonusEnchantValue( *info, option, info->ImprovementMana, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypePhysicAttack:
					{
						SetValue(
							enchantStat.mPhysicAttack,
							GetBonusEnchantValue( *info, option, info->PhysicAttack, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypePhysicDefence:
					{
						SetValue(
							enchantStat.mPhysicDefense,
							GetBonusEnchantValue( *info, option, info->PhysicDefense, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeMagicAttack:
					{
						SetValue(
							enchantStat.mMagicAttack,
							GetBonusEnchantValue( *info, option, info->MagicAttack, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				case ReinforceScript::eTypeMagicDefence:
					{
						SetValue(
							enchantStat.mMagicDefense,
							GetBonusEnchantValue( *info, option, info->MagicDefense, pEnchantLvBonusRate ),
							SetValueTypeAdd );
						break;
					}
				}
			}
		}
		else if(const EnchantScript* const script = g_CGameResourceManager.GetEnchantScript(info->EquipSlot))
		{
			int value = 0;

			if(const BOOL isEnchantedOnDrop = (UINT_MAX == option.mEnchant.mIndex))
			{
				value = info->EnchantDeterm * option.mEnchant.mLevel;
			}

			g_CGameResourceManager.AddEnchantValue(
				enchantStat,
				script->mAbility,
				value);
			g_CGameResourceManager.AddEnchantValue(
				option,
				script->mAbility,
				-value);
		}		
	}

	typedef std::list< TextUnit > TextList;
	TextList firstTextList;
	
	// 081231 LUJ, 형식에 맞게 툴팁을 표시한다
	class ToolTip
	{
	public:
		void Format(TextList& list, const char* title, const PlayerStat::Value& baseValue, int optionValue, DWORD baseColor, LPCTSTR baseValueFormat = _T(" %+0.f"), LPCTSTR optionValueFormat = _T(" %+d"))
		{
			Format(
				list,
				title,
				baseValue,
				optionValue,
				baseColor,
				baseColor,
				baseValueFormat,
				optionValueFormat);
		}

		void Format(TextList& list, const char* title, const PlayerStat::Value& baseValue, int optionValue, DWORD baseColor, DWORD optionColor, LPCTSTR baseValueFormat = _T(" %+0.f"), LPCTSTR optionValueFormat = _T(" %+d"))
		{
			TCHAR textOptionValue[ MAX_PATH ] = { 0 };

			// 081231 LUJ, 표시할 옵션 값을 문자열 버퍼에 저장한다
			if( optionValue )
			{
				_stprintf(
					textOptionValue,
					_T( " %+d" ),
					optionValue );
			}

			TCHAR textBaseValue[ MAX_PATH ]	= { 0 };

			// 081231 LUJ, 표시할 기본 값을 문자열 버퍼에 저장한다
			if( baseValue.mPlus ||
				baseValue.mPercent )
			{
				TCHAR textPlus[ MAX_PATH ]		= { 0 };
				TCHAR textPercent[ MAX_PATH ]	= { 0 };
				_stprintf(
					textPlus,
					baseValue.mPlus ? baseValueFormat : _T( "" ),
					baseValue.mPlus );
				_stprintf(
					textPercent,
					baseValue.mPercent ? _T( " %+0.f%%" ) : _T( "" ),
					baseValue.mPercent * 100.0f );
				// 081231 LUJ, 값이 두 개 있을 경우에 쉼표를 사용한다
				_stprintf(
					textBaseValue,
					_T( "%s%s%s" ),
					textPlus,
					( baseValue.mPlus && baseValue.mPercent ) ? _T( "," ) : _T( "" ),
					textPercent );
			}

			const BOOL hasValue = ( _tcslen( textOptionValue ) || _tcslen( textBaseValue ) );

			if( hasValue )
			{
				TextUnit textUnit = {0};
				textUnit.mColor = baseColor;
				_stprintf(
					textUnit.mText,
					_T( "%s%s" ),
					title,
					textBaseValue);
				list.push_back(
					textUnit);

				textUnit.mColor = optionColor;
				textUnit.mIsAttach = TRUE;
				_stprintf(
					textUnit.mText,
					optionValue ? optionValueFormat : "",
					optionValue);
				list.push_back(
					textUnit);
			}
		}
	}
	toolTip;

	// 081231 LUJ, 물리/마법 공격/마법의 기본 수치는 부호 없이 표시된다
	toolTip.Format(
		firstTextList,
		CHATMGR->GetChatMsg(671),
		base.mPhysicAttack,
		int(enchantStat.mPhysicAttack.mPlus),
		TTTC_ITEM_NAME,
		NAMECOLOR_WANTED,
		_T(" %0.f"),
		_T(" (%+d)"));
	toolTip.Format(
		firstTextList,
		CHATMGR->GetChatMsg(673),
		base.mPhysicDefense,
		int(enchantStat.mPhysicDefense.mPlus),
		TTTC_ITEM_NAME,
		NAMECOLOR_WANTED,
		_T(" %0.f"),
		_T(" (%+d)"));
	toolTip.Format(
		firstTextList,
		CHATMGR->GetChatMsg(672),
		base.mMagicAttack,
		int(enchantStat.mMagicAttack.mPlus),
		TTTC_ITEM_NAME,
		NAMECOLOR_WANTED,
		_T(" %0.f"),
		_T(" (%+d)"));
	toolTip.Format(
		firstTextList,
		CHATMGR->GetChatMsg(674),
		base.mMagicDefense,
		int(enchantStat.mMagicDefense.mPlus),
		TTTC_ITEM_NAME,
		NAMECOLOR_WANTED,
		_T(" %0.f"),
		_T(" (%+d)"));

	// 드롭 옵션은 항목을 분리해서 표시
	{
		const ITEM_OPTION empytOption = { 0 };

		if( memcmp( &empytOption.mDrop, &option.mDrop, sizeof( empytOption.mDrop ) ) )
		{
			TextUnit textUnit = {0};
			textUnit.mColor = TTTC_DEFAULT;

			firstTextList.push_back(
				textUnit);
		}

		const DWORD size = sizeof( option.mDrop.mValue ) / sizeof( *option.mDrop.mValue );

		for( DWORD i = 0; i < size; ++i )
		{
			const ITEM_OPTION::Drop::Value& value = option.mDrop.mValue[ i ];

			switch( value.mKey )
			{
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
					TextUnit textUnit = {0};
					textUnit.mColor = RGB_HALF(57, 187, 225);
					sprintf(
						textUnit.mText,
						"%s %+.1f%%",
						GetName(value.mKey),
						value.mValue * 100.0f);

					firstTextList.push_back(
						textUnit);
					break;	
				}	
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
					TextUnit textUnit = {0};
					textUnit.mColor = RGB_HALF(57, 187, 225);
					sprintf(
						textUnit.mText,
						"%s %+.1f",
						GetName(value.mKey),
						value.mValue);

					firstTextList.push_back(
						textUnit);
					break;	
				}
			}
		}
	}

	TextList secondTextList;
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(671),
		PlayerStat::Value(),
		option.mReinforce.mPhysicAttack,
		NAMECOLOR_WANTED);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(673),
		PlayerStat::Value(),
		option.mReinforce.mPhysicDefence,
		NAMECOLOR_WANTED);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(672),
		PlayerStat::Value(),
		option.mReinforce.mMagicAttack,
		NAMECOLOR_WANTED);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(674),
		PlayerStat::Value(),
		option.mReinforce.mMagicDefence,
		NAMECOLOR_WANTED);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(676),
		base.mStrength,
		option.mReinforce.mStrength,
		option.mReinforce.mStrength ? NAMECOLOR_WANTED : (option.mMix.mStrength ? RGB_HALF(255, 255, 0) : TTTC_ITEM_VALUE));
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(677),
		base.mDexterity,
		option.mReinforce.mDexterity,
		option.mReinforce.mDexterity ? NAMECOLOR_WANTED : (option.mMix.mDexterity ? RGB_HALF(255, 255, 0) : TTTC_ITEM_VALUE));
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(678),
		base.mVitality,
		option.mReinforce.mVitality,
		option.mReinforce.mVitality ? NAMECOLOR_WANTED : (option.mMix.mVitality ? RGB_HALF(255, 255, 0) : TTTC_ITEM_VALUE));
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(679),
		base.mIntelligence,
		option.mReinforce.mIntelligence,
		option.mReinforce.mIntelligence ? NAMECOLOR_WANTED : (option.mMix.mIntelligence ? RGB_HALF(255, 255, 0) : TTTC_ITEM_VALUE));
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(680),
		base.mWisdom,
		option.mReinforce.mWisdom,
		option.mReinforce.mWisdom ? NAMECOLOR_WANTED : (option.mMix.mWisdom ? RGB_HALF(255, 255, 0) : TTTC_ITEM_VALUE));
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(681),
		base.mLife,
		option.mReinforce.mLife,
		option.mReinforce.mLife ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(682),
		base.mMana,
		option.mReinforce.mMana,
		option.mReinforce.mMana ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(846),
		base.mRecoveryLife,
		option.mReinforce.mLifeRecovery,
		option.mReinforce.mLifeRecovery ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(847),
		base.mRecoveryMana,
		option.mReinforce.mManaRecovery,
		option.mReinforce.mManaRecovery ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList, 
		CHATMGR->GetChatMsg(840),
		base.mMoveSpeed,
		option.mReinforce.mMoveSpeed,
		option.mReinforce.mMoveSpeed ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(841),
		base.mEvade,
		option.mReinforce.mEvade,
		option.mReinforce.mEvade ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(162),
		base.mAccuracy,	
		option.mReinforce.mAccuracy,
		option.mReinforce.mAccuracy ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(842),
		base.mCriticalRate,	
		option.mReinforce.mCriticalRate,
		option.mReinforce.mCriticalRate ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);
	toolTip.Format(
		secondTextList,
		CHATMGR->GetChatMsg(843),
		base.mCriticalDamage,
		option.mReinforce.mCriticalDamage,
		option.mReinforce.mCriticalDamage ? NAMECOLOR_WANTED : TTTC_ITEM_VALUE);

	// 081231 LUJ, 아이템 추가 능력이 설정된 경우 표시하기 위해 더해준다
	{
		const SetScript::Element&	itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );
		const PlayerStat&			itemStat	= itemElement.mStat;

		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 671 ),
			itemStat.mPhysicAttack,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 673 ),
			itemStat.mPhysicDefense,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 672 ),
			itemStat.mMagicAttack,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 674 ),
			itemStat.mMagicDefense,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 676 ),
			itemStat.mStrength,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 677 ),
			itemStat.mDexterity,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 678 ),
			itemStat.mVitality,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 679 ),
			itemStat.mIntelligence,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 680 ),
			itemStat.mWisdom,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 681 ),
			itemStat.mLife,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 682 ),
			itemStat.mMana,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 846 ),
			itemStat.mRecoveryLife,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 847 ),
			itemStat.mRecoveryMana,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList, 
			CHATMGR->GetChatMsg( 840 ),
			itemStat.mMoveSpeed,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 841 ),
			itemStat.mEvade,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 162 ),
			itemStat.mAccuracy,	
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 842 ),
			itemStat.mCriticalRate,	
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);
		toolTip.Format(
			secondTextList,
			CHATMGR->GetChatMsg( 843 ),
			itemStat.mCriticalDamage,
			0,
			TTTC_ITEM_VALUE,
			TTTC_ITEM_VALUE);

		// 081231 LUJ, 스킬을 표시한다
		const SetScript::Element::Skill& skillMap = itemElement.mSkill;

		for(	SetScript::Element::Skill::const_iterator skill_it = skillMap.begin();
				skillMap.end() != skill_it;
				++skill_it )
		{
			const BYTE			skillLevel	= skill_it->second;
			const DWORD			skillIndex	= skill_it->first / 100 * 100 + skillLevel;	
			cSkillInfo* const	skillInfo	= SKILLMGR->GetSkillInfo( skillIndex );
			
			if( ! skillInfo )
			{
				continue;
			}
			else if( skillInfo->GetLevel() )
			{
				TextUnit textUnit = {0};
				textUnit.mColor = TTTC_ITEM_VALUE;
				sprintf(
					textUnit.mText,
					"[%s] %s +%d ",
					skillInfo->GetName(),
					RESRCMGR->GetMsg( 636 ),
					skillInfo->GetLevel() );

				secondTextList.push_back(
					textUnit);
			}
			else
			{
				TextUnit textUnit = {0};
				textUnit.mColor = TTTC_ITEM_VALUE;
				sprintf(
					textUnit.mText,
					"[%s] ",
					skillInfo->GetName());

				secondTextList.push_back(
					textUnit);
			}
		}
	}

	// 아이콘에 툴팁 표시
	{
		for(TextList::const_iterator it = firstTextList.begin();
			firstTextList.end() != it;
			++it )
		{
			const TextUnit& textUnit = *it;

			if(textUnit.mIsAttach)
			{
				icon.AddToolTipPartialLine(
					FALSE,
					textUnit.mText,
					textUnit.mColor);
			}
			else
			{
				icon.AddToolTipLine(
					textUnit.mText,
					textUnit.mColor);
			}
		}

		if( ! secondTextList.empty() )
		{
			icon.AddToolTipLine( "" );
		}

		for(TextList::const_iterator it = secondTextList.begin();
			secondTextList.end() != it;
			++it )
		{
			const TextUnit& textUnit = *it;

			if(textUnit.mIsAttach)
			{
				icon.AddToolTipPartialLine(
					FALSE,
					textUnit.mText,
					textUnit.mColor);
			}
			else
			{
				icon.AddToolTipLine(
					textUnit.mText,
					textUnit.mColor);
			}
		}
	}	
}

void CItemManager::AddPetToolTip( cIcon& icon, const ITEMBASE& item )
{
	PET_OBJECT_INFO* pPetObjectInfo = PETMGR->GetPetObjectInfo( item.dwDBIdx );

	if( !pPetObjectInfo )
		return;

	icon.AddToolTipLine( "" );

	char buf[256] = {0,};

	if( pPetObjectInfo->State == ePetState_Die )
	{
		cImage dieimage;
		SCRIPTMGR->GetImage( 141, &dieimage );
		icon.SetStateImage( &dieimage );
	}
	else
	{
		icon.ClearStateImage();
	}

	sprintf( buf, CHATMGR->GetChatMsg( 1586 ), pPetObjectInfo->Level );
	icon.AddToolTipLine( buf );

	if( pPetObjectInfo->Type == ePetType_Basic )
	{
		sprintf( buf, CHATMGR->GetChatMsg( 1587 ), RESRCMGR->GetMsg( 1005 ) );
		icon.AddToolTipLine( buf );
	}
	else
	{
		sprintf( buf, CHATMGR->GetChatMsg( 1587 ), RESRCMGR->GetMsg( pPetObjectInfo->Type + 996 ) );
		icon.AddToolTipLine( buf );
	}

	sprintf( buf, CHATMGR->GetChatMsg( 1588 ), RESRCMGR->GetMsg( pPetObjectInfo->Grade + 1000 ) );			
	icon.AddToolTipLine( buf );

	sprintf( buf, CHATMGR->GetChatMsg( 1589 ), pPetObjectInfo->SkillSlot );
	icon.AddToolTipLine( buf );
}

void CItemManager::AddSetToolTip( cIcon& icon, const ITEMBASE& item )
{
	const ITEM_INFO* info	= GetItemInfo( item.wIconIdx );
	const SetScript* script = GAMERESRCMNGR->GetSetScript( item.wIconIdx );

	if( ! info ||
		! script )
	{
		return;
	}

	icon.AddToolTipLine( "" );

	CHero* hero = OBJECTMGR->GetHero();

	// 보유한 아이템을 여기에 넣는다
	typedef	std::set< DWORD >	ItemIndexSet;
	ItemIndexSet				itemIndexSet;

	// 현재 보유한 아이템 인덱스를 셋에 넣는다.
	{
		BOOL isWearItem = FALSE;
		
		for( POSTYPE position = TP_WEAR_START; position < TP_WEAR_END; ++position )
		{		
			const ITEMBASE* equippedItem = GetItemInfoAbsIn( hero, position );

			if( equippedItem  )
			{
				itemIndexSet.insert( equippedItem->wIconIdx );
				
				if( position == item.Position )
				{
					isWearItem = TRUE;
				}
			}
		}

		// 착용하지 않은 아이템은 세트 아이템 목록만 보여주도록 장비 셋을 클리어한다.
		if( ! isWearItem )
		{
			itemIndexSet.clear();
		}
	}

	// 갖춰진 세트 아이템 개수
	int currentSize = 0;
	// 081231 LUJ, 아이템의 세트 능력 여부를 나타내는 플래그
	const BOOL itemHaveSetAbility = ( 0 < script->mAbility.size() );

	// 완성된 세트 구성을 가져오고, 장비 유무에 따라 표시한다.
	{
		typedef std::list< TextUnit >			TextList;
		TextList								textList;

		// 080916 LUJ, 개별 아이템 표시 방식
		if( script->mSlotNameMap.empty() )
		{	
			for(	SetScript::Item::const_iterator it = script->mItem.begin();
					script->mItem.end() != it;
					++it )
			{	
				const ITEM_INFO* itemInfo = GetItemInfo( *it );
	
				if( ! itemInfo )
				{
					continue;
				}
	
				const std::string name( std::string( "  " ) + itemInfo->ItemName );
	
				if( itemIndexSet.end() == itemIndexSet.find( itemInfo->ItemIdx ) )
				{
					TextUnit textUnit = {0};
					textUnit.mColor = TTTC_ITEM_LIMIT;
					SafeStrCpy(
						textUnit.mText,
						name.c_str(),
						_countof(textUnit.mText));

					textList.push_back(
						textUnit);
				}
				else
				{
					TextUnit textUnit = {0};
					textUnit.mColor = RGB_HALF(0, 255, 0);
					SafeStrCpy(
						textUnit.mText,
						name.c_str(),
						_countof(textUnit.mText));
	
					textList.push_back(
						textUnit);

					++currentSize;
				}
			}	
		}
		// 080916 LUJ, 슬롯 명칭 재지정 방식
		else
		{	
			typedef std::set< EWEARED_ITEM > EquipSlotSet;
			EquipSlotSet equipSlotSet;

			// 080912 LUJ, 아이템 세트가 필요한 슬롯들을 얻어낸다.
			for(	SetScript::Item::const_iterator it = script->mItem.begin();
					script->mItem.end() != it;
					++it )
			{	
				const ITEM_INFO* itemInfo = GetItemInfo( *it );
	
				if(itemInfo)
				{
					equipSlotSet.insert(
						itemInfo->EquipSlot);
				}
			}	
	
			EquipSlotSet lackedEquipSlotSet( equipSlotSet );
	
			// 080912 LUJ, 장착 아이템 중 존재하지 않는 슬롯을 남겨둔다
			for(	ItemIndexSet::const_iterator it = itemIndexSet.begin();
					itemIndexSet.end() != it;
					++it )
			{	
				const ITEM_INFO* itemInfo = GetItemInfo( *it );
	
				if( ! itemInfo )
				{
					continue;
				}

				// 090129 LUJ, 세트 아이템이 아닌 것은 표시하지 않는다
				{
					const SetScript::Item::const_iterator item_it = script->mItem.find( itemInfo->ItemIdx );

					if( script->mItem.end() == item_it )
					{
						continue;
					}
				}
	
				lackedEquipSlotSet.erase(itemInfo->EquipSlot);
			}	
	
			// 080912 LUJ, 구성된 아이템 인덱스의 파트를 조사하여 일치 여부에 따라 다른 색으로 표시한다
			for(	EquipSlotSet::const_iterator it = equipSlotSet.begin();
					equipSlotSet.end() != it;
					++it )
			{	
				const EWEARED_ITEM equipSlot = *it;	
				const SetScript::SlotNameMap::const_iterator name_it = script->mSlotNameMap.find( equipSlot );
				// 090129 LUJ, 따로 지정한 슬롯 명이 없는 경우 일반 명칭을 표시한다
				const std::string slotName( script->mSlotNameMap.end() == name_it ? "" : name_it->second );
				const std::string name( std::string( "  " ) + ( slotName.empty() ? GetName( equipSlot ) : slotName ) );
	
				if( lackedEquipSlotSet.end() != lackedEquipSlotSet.find( equipSlot ) )
				{
					TextUnit textUnit = {0};
					textUnit.mColor = TTTC_ITEM_LIMIT;
					SafeStrCpy(
						textUnit.mText,
						name.c_str(),
						_countof(textUnit.mText));
	
					textList.push_back(
						textUnit);
				}
				else
				{
					TextUnit textUnit = {0};
					textUnit.mColor = RGB_HALF(0, 255, 0);
					SafeStrCpy(
						textUnit.mText,
						name.c_str(),
						_countof(textUnit.mText));

					textList.push_back(
						textUnit);

					++currentSize;
				}
			}
		}

		// 세트 구성 개수도 저장한다. 
		// 081231 LUJ, 세트 능력이 있을 때만 표시한다
		// 090129 LUJ, 그룹 세트일 경우 개수를 그룹 개수로 표시한다
		if( itemHaveSetAbility )
		{
			const DWORD setItemSize = ( script->mSlotNameMap.empty() ? script->mItem.size() : script->mSlotNameMap.size() );

			{
				TextUnit textUnit = {0};
				textUnit.mColor = RGB_HALF(255, 255, 0);
				sprintf(
					textUnit.mText,
					"%s(%d/%d)",
					script->mName.c_str(),
					currentSize,
					setItemSize);

				textList.push_front(
					textUnit);
			}

			for(TextList::const_iterator it = textList.begin();
				textList.end() != it; 
				++it )
			{
				const TextUnit& textUnit = *it;

				if(textUnit.mIsAttach)
				{
					icon.AddToolTipPartialLine(
						FALSE,
						textUnit.mText,
						textUnit.mColor);
				}
				else
				{
					icon.AddToolTipLine(
						textUnit.mText,
						textUnit.mColor);
				}
			}
		}
	}

	// 능력을 표시한다.
	// 081231 LUJ, 세트 능력이 있을 때만 표시한다
	if( itemHaveSetAbility )
	{
		icon.AddToolTipLine( "" );

		char buffer	[ MAX_TEXT_SIZE ];
		char line	[ MAX_TEXT_SIZE ] = { 0, };
		
		/// 세트효과 글자 만큼 태핑할 수 있도록 공백 문자열을 만든다
		std::string tabString;

		// 메시지 [0]세트 효과
		for(
			size_t tabSize = strlen( RESRCMGR->GetMsg( 632 ) );
			tabSize--;
			)
		{
			tabString += " ";
		}

		for(
			SetScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it )
		{
			const int					size	= it->first;
			const SetScript::Element&	element = it->second;
			const DWORD					color	= size > currentSize ? TTTC_ITEM_LIMIT : RGB_HALF( 0, 255, 0 );
			
			//sprintf( buffer, "[%d]세트효과: ", size );
			sprintf( buffer, RESRCMGR->GetMsg( 633 ), size );
			const char* prefix = buffer;

			// 물리 공격
			{
				const PlayerStat::Value& value = element.mStat.mPhysicAttack;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 671 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 671 ), value.mPlus );
					strcat( line, " " );	// 파싱 버그일까? 웬일인지 위의 sprintf에서 뒤의 여백은 무시가 된다...
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}
			
			// 마법 공격
			{
				const PlayerStat::Value& value = element.mStat.mMagicAttack;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 672 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 672 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 물리 방어
			{
				const PlayerStat::Value& value = element.mStat.mPhysicDefense;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 673 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f ", prefix, CHATMGR->GetChatMsg( 673 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 마법 방어
			{
				const PlayerStat::Value& value = element.mStat.mMagicDefense;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 674 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 674 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 힘
			{
				const PlayerStat::Value& value = element.mStat.mStrength;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 676 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 676 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 민첩
			{
				const PlayerStat::Value& value = element.mStat.mDexterity;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 677 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 677 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 생명
			{
				const PlayerStat::Value& value = element.mStat.mVitality;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 678 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 678 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 지능
			{
				const PlayerStat::Value& value = element.mStat.mIntelligence;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 679 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 679 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 지혜
			{
				const PlayerStat::Value& value = element.mStat.mWisdom;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 680 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 680 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 생명
			{
				const PlayerStat::Value& value = element.mStat.mLife;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 681 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 681 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 마나
			{
				const PlayerStat::Value& value = element.mStat.mMana;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 682 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 682 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 생명 회복
			{
				const PlayerStat::Value& value = element.mStat.mRecoveryLife;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, RESRCMGR->GetMsg( 634 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, RESRCMGR->GetMsg( 634 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 마나 회복
			{
				const PlayerStat::Value& value = element.mStat.mRecoveryMana;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, RESRCMGR->GetMsg( 635 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, RESRCMGR->GetMsg( 635 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 명중
			{
				const PlayerStat::Value& value = element.mStat.mAccuracy;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, RESRCMGR->GetMsg( 282 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, RESRCMGR->GetMsg( 282 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 회피
			{
				const PlayerStat::Value& value = element.mStat.mEvade;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, RESRCMGR->GetMsg( 281 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, RESRCMGR->GetMsg( 281 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 크리티컬 확률
			{
				const PlayerStat::Value& value = element.mStat.mCriticalRate;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 842 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 842 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 크리티컬 피해
			{
				const PlayerStat::Value& value = element.mStat.mCriticalDamage;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 843 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 843 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			// 080226 LUJ, 이동 속도
			{
				const PlayerStat::Value& value = element.mStat.mMoveSpeed;

				if( value.mPercent )
				{
					sprintf( line, "%s%s %+.0f%% ", prefix, CHATMGR->GetChatMsg( 840 ), value.mPercent * 100 );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
				else if( value.mPlus )
				{
					sprintf( line, "%s%s %+.0f%", prefix, CHATMGR->GetChatMsg( 840 ), value.mPlus );
					strcat( line, " " );
					icon.AddToolTipLine( line, color );

					prefix = tabString.c_str();
				}
			}

			for(
				SetScript::Element::Skill::const_iterator it = element.mSkill.begin();
				element.mSkill.end() != it;
				++it )
			{
				const short	level	= it->second;
				const DWORD	index	= it->first / 100 * 100 + level;	
				cSkillInfo* info	=  SKILLMGR->GetSkillInfo( index );
				
				if( ! info )
				{
					continue;
				}
				else if( info->GetLevel() )
				{
					sprintf( line, "%s[%s] %s +%d ", prefix, info->GetName(), RESRCMGR->GetMsg( 636 ), info->GetLevel() );
				}
				else
				{
					sprintf( line, "%s[%s] ", prefix, info->GetName() );
				}

				icon.AddToolTipLine( line, color );
				
				prefix = tabString.c_str();
			}
		}
	}
}


int CItemManager::GetSetItemSize( CPlayer* player, const SetScript* script )
{
	if( ! script || ! player )
	{
		ASSERT( 0 );
		return 0;
	}

	int size = 0;

	for( POSTYPE part = TP_WEAR_START; part < TP_WEAR_END; ++part )
	{
		const ITEMBASE * itemBase = GetItemInfoAbsIn(player, part );

		if( itemBase &&
			itemBase->dwDBIdx )
		{
			const ITEM_INFO * itemInfo = GetItemInfo( itemBase->wIconIdx );

			if( itemInfo &&
				script->mItem.end() != script->mItem.find( itemInfo->ItemIdx ) )
			{
				++size;
			}
		}
	}

	return size;
}

void CItemManager::LoadMixScript()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/ItemMix.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	char buffer[ MAX_PATH * 5 ];

	enum Type
	{
		TypeNone,
		TypeProtection,
		TypeResult,
		// 080925 LUJ, 조합 보조
		TypeSupport,
	}
	type = TypeNone;

	while( ! file.IsEOF() )
	{
		file.GetLine( buffer, sizeof( buffer ) );

		const int length = _tcslen( buffer );

		if( ! length )
		{
			continue;
		}

		// 중간의 주석 제거
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = buffer[ i ];
			const char b = buffer[ i + 1 ];

			if( '/' == a &&
				'/' == b )
			{
				buffer[ i ] = 0;
				break;
			}
		}

		const char*		separator	= "~\n\t(), ";
		const char*		token		= strtok( buffer, separator );

		if( ! token )
		{
			continue;
		}
		else if( ! stricmp( "#protection", token ) )
		{
			type = TypeProtection;
			continue;
		}
		else if( ! stricmp( "#result", token ) )
		{
			type = TypeResult;
			continue;
		}
		// 080925 LUJ, 조합 보조
		else if( ! stricmp( "#support", token ) )
		{
			type = TypeSupport;
			continue;
		}

		switch( type )
		{
		case TypeProtection:
			{
				const DWORD itemIndex = atoi( token );

				mMixProtectionItemSet.insert( itemIndex );
				break;
			}
		case TypeResult:
			{
				const DWORD		itemIndex	= atoi( token );
				MixScript&		data		= mMixScriptMap[ itemIndex ];

				data.push_back( ItemMixResult() );

				ItemMixResult& result = data.back();

				result.mMaterial[ itemIndex ] = 1;

				{
					// 091012 ShinJS --- 기본 재료 아이템 Index 저장
					result.mBaseMaterial	= itemIndex;
					result.mItemIndex		= strtoul( strtok( 0, separator ), 0, 10 );
					result.mRequiredLevel	= WORD( atoi( strtok( 0, separator ) ) );
					result.mMoney			= strtoul( strtok( 0, separator ), 0, 10 );

					const float rate		= float( atof( strtok( 0, separator ) ) / 10000 );
					result.mSuccessRate		= min( 1.0f, rate );
				}

				while( true )
				{
					const char* token1 = strtok( 0, separator );
					const char* token2 = strtok( 0, separator );

					if( ! token1 || ! token2 )
					{
						break;
					}

					const DWORD index		= strtoul( token1, 0, 10 );
					const DWORD	quantity	= strtoul( token2, 0, 10 );

					if( 0 < quantity )
					{
						result.mMaterial[ index ] += quantity;
					}
				}

				// 090812 ShinJS --- 조합: 결과 Item으로 script를 구하기 위한 정보 추가
				// 091012 ShinJS --- 결과 아이템이 같더라도 재료아이템이 다른경우 정보 추가
				BOOL bExist = FALSE;
				BOOL bOverlapResult = FALSE;
				stMixResultItemInfo* pMixInfo = NULL;
				if( m_htMixResultItemInfo.GetData( result.mItemIndex ) )
				{
					bOverlapResult = TRUE;
					m_htMixResultItemInfo.StartGetMultiData( result.mItemIndex );
					while( (pMixInfo = m_htMixResultItemInfo.GetMultiData()) != NULL )
					{
						if( pMixInfo->dwBaseItemIdx == itemIndex )
						{
							bExist = TRUE;
							break;
						}
					}
				}

				// 같은 정보가 존재하는 경우 추가하지 않는다.
				if( bExist )
					break;

				pMixInfo = new stMixResultItemInfo;
				pMixInfo->dwResultItemIdx		= result.mItemIndex;
				pMixInfo->dwBaseItemIdx			= itemIndex;
				pMixInfo->bOverlappedResult		= bOverlapResult;

				m_htMixResultItemInfo.Add( pMixInfo, result.mItemIndex );

				break;
			}
			// 080925 LUJ, 조합 보조
		case TypeSupport:
			{
				MixSupportScript& script = mMixSupportScriptMap[ atoi( token ) ];

				while( true )
				{
					const char* key		= strtok( 0, separator );
					const char* rate	= strtok( 0, separator );

					if(	! key ||
						! rate )
					{
						break;
					}

					// 080925 LUJ, 확률을 누적하여 구간을 설정하는 내부 클래스
					class
					{
					public:
						void operator()( MixSetting::Ability& abilityMap, const char* rate, MixSetting::eAbility ability ) const
						{
							const float lastRate = ( abilityMap.empty() ? 0 : ( *( abilityMap.rbegin() ) ).first );

							abilityMap[ lastRate + float( atof( rate ) ) ] = ability;
						}
					}
					Accumulate;

					if( ! stricmp( key, "success" ) )
					{
						script.mBonusSucessRate	= float( atof( rate ) / 100.0f );
					}
					else if( ! stricmp( key, "strength" ) )
					{
						Accumulate( script.mAbiltyMap, rate, MixSetting::eAbilityStrength );
					}
					else if( ! stricmp( key, "dexterity" ) )
					{
						Accumulate( script.mAbiltyMap, rate, MixSetting::eAbilityDexterity );
					}
					else if( ! stricmp( key, "vitality" ) )
					{
						Accumulate( script.mAbiltyMap, rate, MixSetting::eAbilityVitality );
					}
					else if( ! stricmp( key, "intelligence" ) )
					{
						Accumulate( script.mAbiltyMap, rate, MixSetting::eAbilityIntelligence );
					}
					else if( ! stricmp( key, "wisdom" ) )
					{
						Accumulate( script.mAbiltyMap, rate, MixSetting::eAbilityWisdom );
					}
				}
			}
		}
	}
}


const MixScript* CItemManager::GetMixScript( DWORD itemIndex ) const
{
	MixScriptMap::const_iterator it = mMixScriptMap.find( itemIndex );

	return mMixScriptMap.end() == it ? 0 : &( it->second );
};


const ItemMixResult* CItemManager::GetMixResult( DWORD itemIndex, DWORD level ) const
{
	MixScriptMap::const_iterator it = mMixScriptMap.find( itemIndex );

	if( mMixScriptMap.end() != it )
	{
		const MixScript&			mixScript	= it->second;
		MixScript::const_iterator	inner		= mixScript.begin();

		if( mixScript.size() > level )
		{
			std::advance( inner, level );

			const ItemMixResult& mixResult = *inner;

			return &mixResult;
		}
	}

	return 0;
}

void CItemManager::GetMixResultFromResultItemIdx( DWORD resultItemIdx, CYHHashTable<ItemMixResult>* pEqualMixScript )
{
	if( !pEqualMixScript )
		return;

	// 결과 아이템에 해당하는 모든 스크립트를 저장한다.	
	stMixResultItemInfo* pMixResultItemInfo = NULL;
	m_htMixResultItemInfo.StartGetMultiData( resultItemIdx );
	while( (pMixResultItemInfo = m_htMixResultItemInfo.GetMultiData()) != NULL )
	{
		MixScriptMap::const_iterator it = mMixScriptMap.find( pMixResultItemInfo->dwBaseItemIdx );
		if( mMixScriptMap.end() == it )
			return;
		
		const MixScript& mixScript = it->second;
		MixScript::const_iterator iterList;
		for( iterList = mixScript.begin(); iterList != mixScript.end() ; ++iterList )
		{
			const ItemMixResult& mixResult = *iterList;
			if( mixResult.mItemIndex == resultItemIdx )
				pEqualMixScript->Add( const_cast<ItemMixResult*>( &mixResult ), pEqualMixScript->GetDataNum() );
		}
	}
}

const DWORD CItemManager::GetMixResultPosInfo( DWORD BaseItemIdx, ItemMixResult* pMixResult )
{
	MixScriptMap::const_iterator it = mMixScriptMap.find( BaseItemIdx );
	if( mMixScriptMap.end() == it )
		return (DWORD)-1;

	const MixScript& mixScript = it->second;
	MixScript::const_iterator iterList;
	DWORD dwPos = 0;
	for( iterList = mixScript.begin() ; iterList != mixScript.end() ; ++iterList, ++dwPos )
	{
		if( &(*iterList) == pMixResult )
		{
			return dwPos;
		}
	}

	return (DWORD)-1;
}

DWORD CItemManager::GetItemNameColor( const ITEM_INFO& info ) const
{
	switch( info.kind )
	{
	case ITEM_INFO::eKindNormal:
		{
			return RGB( 254, 254, 254 );
		}
	case ITEM_INFO::eKindHighClass:
		{
			return RGB( 0, 255, 0 );
		}
	case ITEM_INFO::eKindUnique:
		{
			return RGB( 50, 135, 245 );
		}
	case ITEM_INFO::eKindRare:
		{
			return RGB( 254, 150, 20 );
		}
	case ITEM_INFO::eKindLegend:
		{
			return RGB( 230, 50, 230 );
		}
	default:
		{
			ASSERTMSG( 0, "지정되지 않은 종류가 있습니다. 일단 툴팁 색상을 백색으로 표시합니다" );
			return RGB( 254, 254, 254 );
		}
	}
}


const DissolveScript* CItemManager::GetDissolveScript( DWORD itemIndex ) const
{
	DissolveScriptMap::const_iterator it = mDissolveScriptMap.find( itemIndex );

	return mDissolveScriptMap.end() == it ? 0 : &( it->second );
}


void CItemManager::LoadDissolveScript()
{
	CMHFile file;
	file.Init( "System/Resource/itemDissolve.bin", "rb" );

	char buffer[ 1024 ];

	while( ! file.IsEOF() )
	{
		file.GetLine( buffer, sizeof( buffer ) );

		const char* separator	= "\t\n ,";
		const char* token		= strtok( buffer, separator );
		const DWORD itemIndex	= strtoul( token, 0, 10 );

		DissolveScript& script = mDissolveScriptMap[ itemIndex ];

		script.mLevel			= strtoul( strtok( 0, separator ), 0, 10 ) ;

        const DWORD staticResultSize = 5;

        for( DWORD i = 0; i < staticResultSize; ++i )
		{
			const char* token1		= strtok( 0, separator );
			const char* token2		= strtok( 0, separator );
			
			ASSERT( token1 && token2 );

			const DWORD index		= strtoul( token1, 0, 10 );
			const DWORD quantity	= strtoul( token2, 0, 10 );

			if( index && quantity )
			{
				script.mStaticResult.push_back( DissolveScript::Result() );

				DissolveScript::Result& result = script.mStaticResult.back();

				result.mItemIndex	= index;
				result.mQuantity	= quantity;
			}
		}

		// 확률에 따라 아이템이 출현하는 동적 결과는 처리하기 조금 까다롭다.
		// 다음과 같이 진행할 것이다. 먼저 파싱 결과를 저장한다. 그 값을 바탕으로 
		// 루프를 돌면서 맵에 누적시킨 값들을 저장한다(값이 10000이 넘지 않도록 주의).
		// 예를 들어 10%, 30%, 60% 확률이 있다면, 맵에 1000, 4000, 10000으로 저장한다.
		// 그리고 결과를 얻을 때는 랜덤값을 얻은 후 맵의 lower_bound()로 해당되는 값을 찾아낸다.
		{
			const DWORD maxRate = 10000;

			for( DWORD rate = 0; rate < maxRate; )
			{
				const char* token1	= strtok( 0, separator );
				const char* token2	= strtok( 0, separator );
				const char* token3	= strtok( 0, separator );

				if( ! token1 || ! token2 || ! token3 )
				{
					break;
				}

				const DWORD itemIndex	= strtoul( token1, 0, 10 );
				const DWORD quantity	= strtoul( token2, 0, 10 );
				const DWORD percent		= strtoul( token3, 0, 10 );

				if( ! itemIndex || ! quantity || ! percent )
				{
					break;
				}

				rate = min( maxRate, rate + percent );
				
				DissolveScript::Result& result = script.mDynamicResult[ rate ];

				result.mItemIndex	= itemIndex;
				result.mQuantity	= quantity;
			}

			if( !	script.mDynamicResult.empty()	&&
					script.mDynamicResult.end() == script.mDynamicResult.find( maxRate ) )
			{
				ASSERT( 0 && "분해 스크립트의 확률 값의 합이 10000이 되지 않습니다. 올바른 결과를 보장할 수 없습니다" );
			}
		}
	}
}

BOOL CItemManager::IsRare( const ITEM_INFO* info ) const
{
	if( info )
	{
		switch( info->kind )
		{
		case ITEM_INFO::eKindRare:
		case ITEM_INFO::eKindUnique:
		case ITEM_INFO::eKindLegend:
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CItemManager::LoadCoolTimeScript()
{
	CMHFile file;
	file.Init( "system/resource/itemCoolTime.bin", "rb" );

	int		openCount	= 0;
	DWORD	groupIndex	= 1;

	std::list< std::string >	text;

	for(TCHAR line[ MAX_PATH ];
		! file.IsEOF();
		ZeroMemory( line, sizeof( line ) ) )
	{
		file.GetLine( line, sizeof( line ) / sizeof( TCHAR ) );

		const int length = _tcslen( line );

		if( ! length )
		{
			continue;
		}

		// 중간의 주석 제거
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = line[ i ];
			const char b = line[ i + 1 ];

			if( '/' == a &&
				'/' == b )
			{
				line[ i ] = 0;
				break;
			}
		}

		text.push_back( line );

		const TCHAR* separator	= _T( " ~\n\t=,+\"()%" );
		const TCHAR* token		= _tcstok( line, separator );
		const TCHAR* markBegin	= _T( "{" );
		const TCHAR* markEnd	= _T( "}" );

		if( 0 == token ||
			0 == _tcsicmp( "group", token ) )
		{
			text.pop_back();
		}
		else if( 0 == _tcsnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			text.pop_back();

			++openCount;
		}
		else if( 0 == _tcsnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			text.pop_back();

			if( ! --openCount )
			{
				for(std::list< std::string >::const_iterator it = text.begin();
					text.end() != it;
					++it )
				{
					char buffer[ MAX_PATH ] = {0};

					_tcscpy( buffer, it->c_str() );

					const DWORD itemIndex	= strtoul( _tcstok( buffer, separator ), 0, 10 );
                    const DWORD coolTime	= atoi( _tcstok( 0, separator ) ) * 1000;

					CoolTimeScript::Unit& unit = mCoolTimeScript.mItemData[ itemIndex ];

					unit.mIndex	= groupIndex;
					unit.mTime	= coolTime;

					{
						CoolTimeScript::ItemIndex& indexSet =  mCoolTimeScript.mGroupData[ groupIndex ];

						indexSet.insert( itemIndex );
					}
				}

				text.clear();
				++groupIndex;
			}
			else
			{
				ASSERT( 0 );
				return;
			}
		}
	}
}


BOOL CItemManager::IsCoolTime( DWORD playerIndex, DWORD itemIndex ) const
{
	UsingCoolTimeGroup::const_iterator it =  mUsingCoolTimeGroup.find( playerIndex );

	if( mUsingCoolTimeGroup.end() == it )
	{
		return FALSE;
	}

	const CoolTimeGroup& group = it->second;

	return group.end() != group.find( GetCoolTimeGroupIndex( itemIndex ) );
}


BOOL CItemManager::AddCoolTime( DWORD playerIndex, DWORD itemIndex )
{
	const DWORD groupIndex = GetCoolTimeGroupIndex( itemIndex );

	if( ! groupIndex )
	{
		return FALSE;
	}

	// 해당 쿨타임 그룹을 사용중으로 처리한다.
	{
		CoolTimeGroup& group = mUsingCoolTimeGroup[ playerIndex ];

		group.insert( groupIndex );
	}

	// 큐에 넣어 사용 가능 시점이 자동으로 설정되도록 한다.
	{
		CoolTimeScript::ItemData::const_iterator it = mCoolTimeScript.mItemData.find( itemIndex );

		if( mCoolTimeScript.mItemData.end() == it )
		{
			ASSERT( 0 );
			return FALSE;
		}

		const CoolTimeScript::Unit& unit = it->second;

		mCoolTimeQueue.push( CoolTimeSort::Tick() );

		CoolTimeSort::Tick& tick		= mCoolTimeQueue.top();
		const DWORD			tickCount	= GetTickCount();

		tick.mCoolTimeGroup	= groupIndex;
		tick.mBegin			= tickCount;
		tick.mEnd			= tickCount + unit.mTime;
		tick.mPlayerIndex	= playerIndex;


		std::pair<DWORD, CoolTimeSort::Tick> pairData;
		pairData.first = groupIndex;
		pairData.second = tick;
		m_mapCoolTime.insert(pairData);
	}

	return TRUE;
}


DWORD CItemManager::GetCoolTimeGroupIndex( DWORD itemIndex ) const
{
	CoolTimeScript::ItemData::const_iterator it = mCoolTimeScript.mItemData.find( itemIndex );

	if( mCoolTimeScript.mItemData.end() == it )
	{
		return 0;
	}

	const CoolTimeScript::Unit& unit = it->second;

	return unit.mIndex;
}


void CItemManager::ProcessCoolTime()
{
	// 쿨타임 체크. 남은 시간 별로 정렬되어 있으므로 가장 상위에 있는 것만 체크한다.
	if( mCoolTimeQueue.empty() )
	{
		return;
	}

	const DWORD					tickCount	= GetTickCount();
	const CoolTimeSort::Tick&	tick		= mCoolTimeQueue.top();

	// 남은 시간 별로 정렬된 우선순위 큐의 가장 상위에 있는 유닛의 시간이 만료되었는지 체크하자
	if( ( tick.mBegin > tick.mEnd && tickCount < tick.mBegin && tickCount > tick.mEnd ) ||	// 오버플로된 상황
		( tick.mBegin < tick.mEnd && tick.mEnd < tickCount ) )								// 일반적인 경우
	{
		UsingCoolTimeGroup::iterator it = mUsingCoolTimeGroup.find( tick.mPlayerIndex );

		if( mUsingCoolTimeGroup.end() != it )
		{
			CoolTimeGroup& group = it->second;

			group.erase( tick.mCoolTimeGroup );

			if( group.empty() )
			{
				mUsingCoolTimeGroup.erase( it );
			}
		}

		if(m_mapCoolTime.end() != m_mapCoolTime.find(tick.mCoolTimeGroup))
		{
			m_mapCoolTime.erase(tick.mCoolTimeGroup);
		}

		mCoolTimeQueue.pop();
	}
}


void CItemManager::RemoveCoolTime( DWORD playerIndex )
{
	mUsingCoolTimeGroup.erase( playerIndex );
}

void CItemManager::UpdateDlgCoolTime(DWORD itemIndex)
{
	if(IsCoolTime(gHeroID, itemIndex))
	{
		CoolTimeScript::ItemData::const_iterator item_it = mCoolTimeScript.mItemData.find( itemIndex );

		if( mCoolTimeScript.mItemData.end() == item_it )
		{
			ASSERT( 0 );
			return;
		}

		const CoolTimeScript::Unit& unit		= item_it->second;

		CoolTimeScript::GroupData::const_iterator group_it = mCoolTimeScript.mGroupData.find( GetCoolTimeGroupIndex( itemIndex ) );

		if( mCoolTimeScript.mGroupData.end() == group_it )
		{
			ASSERT( 0 );
			return;
		}

		//const CoolTimeScript::ItemIndex& indexSet = group_it->second;

		std::map<DWORD, CoolTimeSort::Tick>::iterator iter;
		iter = m_mapCoolTime.find(GetCoolTimeGroupIndex( itemIndex ));

		if(iter != m_mapCoolTime.end())
		{
			// 인벤토리 아이콘 쿨타임 처리
			{
				CInventoryExDialog* dialog = ( CInventoryExDialog* )WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
				ASSERT( dialog );
				dialog->SetCoolTimeAbs( itemIndex, iter->second.mBegin, iter->second.mEnd, unit.mTime);
			}

			// 단축창 아이콘 쿨타임 처리
			{
				cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
				ASSERT( dialog );
				dialog->SetCoolTimeAbs( itemIndex, iter->second.mBegin, iter->second.mEnd, unit.mTime);

				// 080706 LYW --- ItemManager : 확장 퀵 슬롯 추가 처리.
				cQuickSlotDlg* dialog2 = NULL ;
				dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
				ASSERT( dialog2 ) ;

				if(dialog2)
				{
					dialog2->SetCoolTimeAbs( itemIndex, iter->second.mBegin, iter->second.mEnd, unit.mTime) ;
				}
			}
		}
	}
}


void CItemManager::LoadChangeItem()
{
	CMHFile file;

	if( ! file.Init( "system/resource/changeitem.bin", "rb") )
	{
		ASSERT( 0 );
		return;
	}

	char line[ MAX_PATH ];

	while( ! file.IsEOF() )
	{
		file.GetLine( line, sizeof( line ) );

		const int length = _tcslen( line );

		if( ! length )
		{
			continue;
		}

		// 중간의 주석 제거
		{
			for( int i = 0; i < length - 1; ++i )
			{
				const char a = line[ i ];
				const char b = line[ i + 1 ];

				if( '/' == a &&
					'/' == b )
				{
					line[ i ] = 0;
					break;
				}
			}
		}

		const char* separator	= " \t";
		const char* token		= strtok( line, separator );

		if (	token						&&
				stricmp( token, "money" )	&&
				stricmp( token, "item" )	&&
				stricmp( token, "end_kind" ) )
		{
			mChangeItemSet.insert( atoi( token ) );
		}
	}
}


BOOL CItemManager::IsChangeItem( DWORD itemIndex ) const
{
	return mChangeItemSet.end() != mChangeItemSet.find( itemIndex );
}


//=========================================================================
//	NAME : UseItem_ResetStats
//	DESC : 071203 LYW --- 스탯 초기화 아이템 처리.
//=========================================================================
void CItemManager::UseItem_ResetStats(CItem* pItem)
{
	if( !pItem ) return ;																// 아이템의 무결성을 체크한다.

	MSG_ITEM_USE_SYN msg ;																// 메시지 구조체를 선언하고 초기화 한다.
	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN)) ;

	msg.Category	= MP_ITEM ;															// 카테고리, 프로토콜, 오브젝트 아이디 세팅.
	msg.Protocol	= MP_ITEM_USE_SYN ;
	msg.dwObjectID	= gHeroID ;

	msg.wItemIdx	= pItem->GetItemIdx() ;												// 아이템 인덱스와 포지션을 세팅한다.
	msg.TargetPos	= pItem->GetItemBaseInfo().Position ;

	NETWORK->Send( &msg, sizeof(MSG_ITEM_USE_SYN) ) ;									// 메시지 전송.
}





//=========================================================================
//	NAME : UseItem_ResetSkill
//	DESC : 071203 LYW --- 스킬 초기화 아이템 처리.
//=========================================================================
void CItemManager::UseItem_ResetSkill(CItem* pItem)
{
	if( !pItem ) return ;																// 아이템의 무결성 체크.

	MSG_ITEM_USE_SYN msg ;																// 메시지 구조체를 선언하고 초기화 한다.
	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN)) ;

	msg.Category	= MP_ITEM ;															// 카테고리, 프로토콜, 오브젝트 아이디를 세팅한다.
	msg.Protocol	= MP_ITEM_USE_SYN ;
	msg.dwObjectID	= gHeroID ;

	msg.wItemIdx	= pItem->GetItemIdx() ;												// 아이템의 인덱스와 포지션을 세팅한다.
	msg.TargetPos	= pItem->GetItemBaseInfo().Position ;

	NETWORK->Send( &msg, sizeof(MSG_ITEM_USE_SYN) ) ;									// 메시지 전송.
}

// 080218 KTH -- Reset RegidentRegist
void CItemManager::UseItem_ResetRegistResident(CItem* pItem)
{
	if( !pItem ) return;

	MSG_ITEM_USE_SYN msg;

	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN));

	msg.Category	= MP_ITEM;
	msg.Protocol	= MP_ITEM_USE_SYN;
	msg.dwObjectID	= gHeroID;
	msg.wItemIdx	= pItem->GetItemIdx();
	msg.TargetPos	= pItem->GetItemBaseInfo().Position;

	NETWORK->Send( &msg, sizeof(MSG_ITEM_USE_SYN) );
}


//=========================================================================
//	NAME : UseItem_MapMoveScroll
//	DESC : 071203 LYW --- 맵 이동 스크롤 처리.
//=========================================================================
void CItemManager::UseItem_MapMoveScroll(CItem* pItem)
{
	if( !pItem ) return ;																// 아이템의 무결성 체크.

	cMapMoveDialog* pDlg = NULL ;														// 맵 이동 창을 받는다.
	pDlg = GAMEIN->GetMapMoveDlg() ;

	if( !pDlg ) return ;																// 맵 이동 창의 무결성 체크.

	CItem* pDeleteItem = NULL ;
	pDeleteItem = pDlg->GetItemInfo() ;													// 사용 중이던 아이템 정보를 받는다.

	if( pDeleteItem )																	// 사용 중이던 아이템이 있다면,
	{
		pDeleteItem->SetLock(FALSE) ;													// 사용 중이던 아이템의 잠김을 해제한다.
	}

	pItem->SetLock(TRUE) ;																// 사용할 아이템을 잠근다.
    
	pDlg->SetItemInfo(pItem) ;															// 맵 이동창에 아이템 정보를 세팅한다.

	// 080919 LYW --- ItemManager : 사용 용도를 설정한다.
	pDlg->SetUseCase(eNormalStyle) ;

	pDlg->SetActive(TRUE) ;																// 맵 이동창을 활성화한다.

	pDlg->GetMapListDlg()->ResetGuageBarPos() ;											// 맵 리스트를 다시 정렬한다.
}





//=========================================================================
//	NAME : UseItem_ExtendStorage
//	DESC : 071206 LYW --- 창고 확장 아이템 처리.
//=========================================================================
void CItemManager::UseItem_ExtendStorage(CItem* pItem)
{
	if( !pItem ) return ;																// 아이템의 무결성 체크.

	CInventoryExDialog* pDlg = NULL ;													// 인벤토리를 얻어온다.
	pDlg = GAMEIN->GetInventoryDialog() ;

	if( !pDlg ) return ;																// 인벤토리 무결성 체크.

	MSG_ITEM_USE_SYN msg ;																// 메시지 구조체를 선언하고 초기화 한다.
	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN)) ;

	msg.Category = MP_ITEM;																// 카테고리 및, 기타 메시지 구조체를 세팅한다.
	msg.Protocol = MP_ITEM_USE_SYN;
	msg.dwObjectID = gHeroID;
	msg.TargetPos = pItem->GetPosition();
	msg.wItemIdx = pItem->GetItemIdx();

	if( IsChangeItem( pItem->GetItemIdx() ) )											// 변환 아이템이면,
	{
		pDlg->SetDisable( TRUE );														// 인벤토리를 고정시킨다.

		const DWORD changeSecond = WAITMILISECOND;										// 변환 시간을 세팅한다.

		pDlg->SetCoolTime( pItem->GetItemIdx(), changeSecond );							// 아이템에 쿨타임을 세팅한다.

		{
			cQuickSlotDlg* dialog = NULL ;												// 퀵 슬롯 다이얼로그를 받는다.
			dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG ) ;

			ASSERT( dialog ) ;															// 퀵 슬롯 다이얼로그의 무결성 체크.

			dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond ) ;					// 퀵 슬롯에 쿨타임을 세팅한다.

			// 080706 LYW --- ItemManager : 확장 퀵 슬롯 추가 처리.
			cQuickSlotDlg* dialog2 = NULL ;
			dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
			ASSERT( dialog2 ) ;

			if(dialog2)
			{
				dialog2->SetCoolTime( pItem->GetItemIdx(), changeSecond ) ;
			}
		}
	}
	else																				// 변환아이템이 아니면, 
	{
		NETWORK->Send( &msg, sizeof( msg ) ) ;											// 메시지를 전송한다.
	}
}

//=========================================================================
//	NAME : GetSkillPointByItem
//	DESC : 080611 LYW --- 스킬 포인트 추가 주문서 처리.
//=========================================================================
void CItemManager::GetSkillPointByItem(CItem* pItem)
{
	ASSERT(pItem) ;
	if( !pItem ) return ;																// 아이템의 무결성 체크.

	MSG_ITEM_USE_SYN msg ;																// 메시지 구조체를 선언하고 초기화 한다.
	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN)) ;

	msg.Category	= MP_ITEM ;															// 카테고리, 프로토콜, 오브젝트 아이디를 세팅한다.
	msg.Protocol	= MP_ITEM_USE_SYN ;
	msg.dwObjectID	= gHeroID ;

	msg.wItemIdx	= pItem->GetItemIdx() ;												// 아이템의 인덱스와 포지션을 세팅한다.
	msg.TargetPos	= pItem->GetItemBaseInfo().Position ;

	NETWORK->Send( &msg, sizeof(MSG_ITEM_USE_SYN) ) ;									// 메시지 전송.
}





//=========================================================================
//	NAME : IsUsingItem
//	DESC : 071206 LYW --- 이미 사용하여 버프가 활성화 중인지 체크하는 함수 추가.
//=========================================================================
BOOL CItemManager::IsUsingItem(CItem* pItem)
{
	const ITEM_INFO* const pInfo = GetItemInfo(
		pItem->GetItemIdx());

	if(NULL == pInfo)
	{
		return FALSE;
	}

	cActiveSkillInfo* const pActiveSkillInfo = (cActiveSkillInfo*)SKILLMGR->GetSkillInfo(
		pInfo->SupplyValue);

	if(NULL == pActiveSkillInfo)
	{
		return FALSE;
	}

	return STATUSICONDLG->IsHasBuff(
		pActiveSkillInfo->GetSkillInfo()->Buff[0]);
}





//=========================================================================
//	NAME : UseItem_ItemMallItem
//	DESC : 071206 LYW --- 아이템몰 아이템 사용 함수 추가.
//=========================================================================
void CItemManager::UseItem_ItemMallItem(CItem* pItem, CObject* pTarget)
{
	if( !pItem ) return ;																// 아이템의 무결성 체크.
	if(!pTarget) return;

	MAINTARGET MainTarget;																// 주 타겟을 선언한다.

	CActionTarget Target ;																// 액션타겟을 HERO로 하고, 주 타겟으로 처리한다.
	Target.InitActionTarget(/*HERO*/pTarget, NULL) ;		
	Target.GetMainTarget( &MainTarget ) ;

	DWORD dwSupplyValue = 0 ;
	dwSupplyValue = pItem->GetItemInfo()->SupplyValue ;									// 아이템이 지원하는 스킬 인덱스를 받는다.

	ROTATEINFO* pRotateInfo = HERO->GetBaseRotateInfo();
	if( !pRotateInfo )	return;
	const float fOperatorAngle = (pRotateInfo->bRotating == TRUE ? pRotateInfo->Angle.GetTargetAngleRad() : HERO->GetAngle());
	const VECTOR3 directionVector = GetRadToVector( fOperatorAngle );

	MSG_ITEM_SKILL_START_SYN msg ;														
	memset(&msg, 0, sizeof(MSG_ITEM_SKILL_START_SYN)) ;									// 메시지 구조체를 선언하고 초기화 한다.

	msg.Category	= MP_ITEM ;
	msg.Protocol	= MP_ITEM_SKILL_SYN ;												// 카테도리 등, 각 메시지 정보를 세팅한다.
	msg.dwObjectID	= gHeroID ;
	msg.Idx			= pItem->GetItemIdx() ;
	msg.Pos			= pItem->GetItemBaseInfo().Position ;

	msg.SkillStartSyn.InitMsg( dwSupplyValue, &MainTarget, directionVector, gHeroID ) ;		// 스킬 시작 요청 메시지를 초기화한다.

	NETWORK->Send(&msg,sizeof(MSG_ITEM_SKILL_START_SYN)) ;								// 메시지를 전송한다.
}





//=========================================================================
//	NAME : UseItem_ExtendInventory
//	DESC : 071210 LYW --- 인벤토리 확장 아이템 처리.
//=========================================================================
void CItemManager::UseItem_ExtendInventory(CItem* pItem)
{
	if( !pItem ) return ;																// 아이템의 무결성을 체크한다.

	CInventoryExDialog* pDlg = NULL ;
	pDlg = GAMEIN->GetInventoryDialog() ;												// 인벤토리를 받고, 인벤토리의 무결성을 체크한다.

	if( !pDlg ) return ;

	MSG_ITEM_USE_SYN msg ;																// 메시지 구조체를 선언하고 초기화 한다.
	memset(&msg, 0, sizeof(MSG_ITEM_USE_SYN)) ;

	msg.Category = MP_ITEM;																// 카테고리 등, 각 메시지를 세팅한다.
	msg.Protocol = MP_ITEM_INCREASE_INVENTORY_SYN ;
	msg.dwObjectID = gHeroID;
	msg.TargetPos = pItem->GetPosition();
	msg.wItemIdx = pItem->GetItemIdx();

	pDlg->SetDisable( TRUE );															// 인벤토리를 고정시킨다.

	if( IsChangeItem( pItem->GetItemIdx() ) )											// 변환 가능 아이템이면,
	{
		const DWORD changeSecond = WAITMILISECOND;										// 변환 시간을 세팅한다.

		pDlg->SetCoolTime( pItem->GetItemIdx(), changeSecond );							// 인벤토리에 쿨타임을 세팅한다.

		{
			cQuickSlotDlg* dialog = NULL ;
			dialog = (cQuickSlotDlg*)WINDOWMGR->GetWindowForID(QI1_QUICKSLOTDLG);		// 퀵 슬롯 다이얼로그를 받는다.
			ASSERT( dialog );

			dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond );					// 퀵 슬롯 다이얼로그에 쿨타임을 적용한다.

			// 080706 LYW --- ItemManager : 확장 퀵 슬롯 추가 처리.
			cQuickSlotDlg* dialog2 = NULL ;
			dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
			ASSERT( dialog2 ) ;

			if(dialog2)
			{
				dialog2->SetCoolTime( pItem->GetItemIdx(), changeSecond ) ;
			}
		}
	}
	else																				// 변환 불가능 아이템이면,
	{
		NETWORK->Send( &msg, sizeof( msg ) ) ;											// 메시지를 전송한다.
	}
}





//=========================================================================
//	NAME : Item_Increase_Inventory_Ack
//	DESC : 071210 LYW --- 인벤토리 확장 성공 함수 추가.
//=========================================================================
void CItemManager::Item_Increase_Inventory_Ack( void* pMsg )
{
	if( !pMsg ) return ;																// 메시지의 무결성을 체크한다.

	MSG_DWORD* pmsg = NULL ;															// 메시지를 받는다.
	pmsg = (MSG_DWORD*)pMsg ;

	if( !pmsg ) return ;																// 메시지의 무결성 체크.

	if( pmsg->dwObjectID != gHeroID ) return ;											// HERO와 무관한지 체크.

	CInventoryExDialog* pDlg = NULL ;													
	pDlg = GAMEIN->GetInventoryDialog() ;												// 인벤토리를 받고, 무결성 체크를 한다.

	if( !pDlg ) return ;

	switch(pmsg->dwData)																// 메시지 타입이 1이면,
	{
	case 1 : 
		{
			pDlg->ShowTab(2,TRUE) ;														// 인벤토리 한칸 확장 처리를 한다.
			HERO->Set_HeroExtendedInvenCount(1) ;
		}
		break ;
	case 2 :																			// 메시지 타입이 2이면,
		{
			pDlg->ShowTab(2,TRUE) ; 
			pDlg->ShowTab(3,TRUE) ; 
																						// 인벤토리 두칸 확장 처리를 한다.
			HERO->Set_HeroExtendedInvenCount(2) ;
		}
		break ;
	default : break ;
	}

	WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1249) ) ;						// 인벤토리 확장을 알린다.

	// 071212 LYW --- ItemManager : 인벤토리 확장 방어코드.
	pDlg->SetDisable(FALSE) ;
}





//=========================================================================
//	NAME : Item_Increase_Inventory_Nack
//	DESC : 071210 LYW --- 인벤토리 확장 실패 함수 추가.
//=========================================================================
void CItemManager::Item_Increase_Inventory_Nack( void* pMsg )
{
	if( !pMsg ) return ;																// 메시지의 무결성 체크.

	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;															// 메시지를 받는다.

	if( !pmsg ) return ;

	switch(pmsg->dwData)
	{
	case 0 :																			// 0은 아이템 사용이 실패 하였을때 보낸다.
		{
			WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1250) ) ;
		}
		break ;

	case 1 :																			// 1은 더이상 늘릴 아이템이 없을 시에 보낸다.
		{
			WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1248) ) ;
		}
		break ;
	}

	// 071212 LYW --- ItemManager : 인벤토리 확장 방어코드.
	CInventoryExDialog* pDlg = NULL ;													// 인벤토리를 받는다.
	pDlg = GAMEIN->GetInventoryDialog() ;

	if( !pDlg ) return ;

	pDlg->SetDisable(FALSE) ;
}


//=========================================================================
//	NAME : UseItem_ChangeName
//	DESC : 이름 변경 아이템 사용을 처리하는 함수. 071226 LYW
//=========================================================================
void CItemManager::UseItem_ChangeName(char* pName)
{
	if( !pName ) return ;																// 이름의 무결성 체크.

	int nNameLength = 0 ;																// 이름의 길이를 받는다.
	nNameLength = strlen(pName) ;

	// 080124 KTH -- 피케이 모드 시에는 사용할수 없다.
	if(HERO->IsPKMode())
	{
		WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1263) ) ;					// 에러 메시지를 출력한다.
		return;
	}

	// 080610 LYW --- ItemManager : 캐릭명 변경이 8글자가 안된다고 하는 버그 수정.
	//if( nNameLength < 4 || nNameLength >= MAX_NAME_LENGTH )								// 이름 길이의 유효성을 체크.
	if( nNameLength < 4 || nNameLength >= MAX_NAME_LENGTH + 1 )								// 이름 길이의 유효성을 체크.
	{
		cEditBox * pNameBox = NULL ;
		pNameBox = (cEditBox *)WINDOWMGR->GetWindowForIDEx(NAME_BOX) ;					// 이름 입력 박스를 받는다.

		if( pNameBox )
		{
			pNameBox->SetEditText("") ;													// 공란으로 세팅한다.
		}

		WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(19) ) ;					// 에러 메시지를 출력한다.
		return ;
	}

	if( FILTERTABLE->IsInvalidCharInclude((unsigned char*)pName) )						// 잘못 된 이름인지 필터링 체크를 한다.
	{
		cEditBox * pNameBox = NULL ;
		pNameBox = (cEditBox *)WINDOWMGR->GetWindowForIDEx(NAME_BOX) ;					

		if( pNameBox )
		{
			pNameBox->SetEditText("") ;													// 공란으로 세팅한다.
		}

		WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(14) ) ;					// 에러 메시지를 출력한다.
		return ;
	}

	if( !FILTERTABLE->IsUsableName(pName) )												// 사용 가능한 이름인지 체크한다.
	{
		cEditBox * pNameBox = NULL ;
		pNameBox = (cEditBox *)WINDOWMGR->GetWindowForIDEx(NAME_BOX) ;

		if( pNameBox )																	// 이름 입력란을 공란으로 세팅한다.
		{
			pNameBox->SetEditText("") ;
		}

		WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(14) ) ;					// 에러 메시지를 출력한다.
		return ;
	}

	CInventoryExDialog* pDlg = NULL ;
	pDlg = GAMEIN->GetInventoryDialog() ;												// 인벤토리를 받는다.

	if( !pDlg ) return ;																// 인벤토리의 무결성 체크.

	CItem* pItem = NULL ;
	pItem = pDlg->Get_QuickSelectedItem() ;												// 아이템을 받는다.

	if( !pItem ) return ;																// 아이템의 무결성 체크.

	MSG_CHANGE_CHARACTER_NAME_SYN msg ;
	memset(&msg, 0, sizeof(MSG_CHANGE_CHARACTER_NAME_SYN)) ;							// 메시지 구조체를 선언하고 초기화 한다.

	msg.Category = MP_ITEM ;
	msg.Protocol = MP_ITEM_CHANGE_CHARACTER_NAME_SYN ;
	msg.dwObjectID = gHeroID ;															// 카테고리, 등의 정보를 세팅한다.

	strcpy(msg.changeName, pName) ;														// 이름을 복사한다.

	msg.TargetPos = pItem->GetPosition() ;												// 아이템 정보를 세팅한다.
	msg.wItemIdx  = pItem->GetItemIdx() ;

	pItem->SetLock(FALSE) ;
	pDlg->Set_QuickSelectedItem(NULL) ;													// 아이템의 잠김을 해제한다.

	NETWORK->Send(&msg, sizeof(msg)) ;													// 메시지를 전송한다.

	cChangeNameDialog* pNameBox = NULL ;
	pNameBox = GAMEIN->GetChangeNameDlg() ;												// 이름 변경창을 받는다.

	if( !pNameBox ) return ;															// 이름 변경차의 무결성 체크.

	pNameBox->SetActive(FALSE) ;														// 이름 변경창을 비활성화 한다.
}

//=========================================================================
//	NAME : Item_Change_Character_Name_Ack
//	DESC : 이름 변경 아이템 사용 성공을 처리하는 함수. 071226 LYW
//=========================================================================
void CItemManager::Item_Change_Character_Name_Ack( void* pMsg )
{
	WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1254) ) ;				// 이름 변경을 위한 로그아웃을 알린다.
}


//=========================================================================
//	NAME : Item_Change_Character_Name_Nack
//	DESC : 이름 변경 아이템 사용 실패를 처리하는 함수. 071226 LYW
//=========================================================================
void CItemManager::Item_Change_Character_Name_Nack( void* pMsg ) 
{
	if( !pMsg ) return ;																// 메시지 무결성 체크.

	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;															// 메시지를 받는다.

	if( !pmsg ) return ;																// 메시지의 무결성 체크.

	int nMsgNum = 0 ;																	// 메시지 번호를 담을 변수 선언.

	switch( pmsg->dwData )																// 메시지의 타입에 따라, 
	{
	case 0 :nMsgNum = 1255 ;	break ;													// 이름 변경 실패,
	case 2 :nMsgNum = 1259 ;	break ;													// (패밀리) 변경 실패
	case 3 :nMsgNum = 1258 ;	break ;													// (파티) 변경 실패
	case 4 :nMsgNum = 1257 ;	break ;													// (길드) 변경 실패
	case 5 :nMsgNum = 1256 ;	break ;													// (중복) 변경 실패
	case 6 :nMsgNum = 2329 ;	break ;													// 판매대행
	case 7 :nMsgNum = 2330 ;	break ;													// 우편소포
	default : return ;
	}

	WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg( nMsgNum ) ) ;					// 에러 메시지를 띄운다.
}


// 080228 LUJ, 조합 보호 아이템 여부
BOOL CItemManager::IsMixProtectionItem( const ITEMBASE& item ) const
{
	return mMixProtectionItemSet.end() != mMixProtectionItemSet.find( item.wIconIdx );
}

void CItemManager::LoadItemEffectList()
{
	CMHFile file;

	if( ! file.Init("Data/Script/Direct/EnchantSetting.bin", "rb" ) )
	{
		return;
	}

	while( ! file.IsEOF() )
	{
		EnchantEffect* pEffect = new EnchantEffect;

		pEffect->ItemIdx = file.GetDword();
		
		if ( mEffectList.GetData( pEffect->ItemIdx ) )
		{
			continue;
		}

		for( int i =0; i < 13; i++ )
		{
			for( int j = 0; j < 3; j++ )
			{
				pEffect->Effect[ i ][ j ] = file.GetWord();
			}
		}

		mEffectList.Add( pEffect, pEffect->ItemIdx );
	}
}

EnchantEffect* CItemManager::GetItemEffect( DWORD ItemIdx )
{
	return (EnchantEffect*)mEffectList.GetData( ItemIdx );
}

// 080414 LUJ, 외양 변환 스크립트를 읽는다
void CItemManager::LoadBodyChangeScript()
{
	CMHFile file;

	if( ! file.Init( "system/resource/ItemBodyChange.bin", "rb" ) )
	{
		return;
	}

	char line[ MAX_PATH * 5 ] = { 0 };

	enum ScriptState
	{
		StateNone,
		StateItem,
		StateRace,
		StatePart,
		StateGender,
	}
	scriptState = StateNone;
	
	int						openCount	= 0;
	BodyChangeScriptList*	scriptList	= 0;
	BodyChangeScript		script		= { 0 };

	typedef std::list< std::string >	TextList;
	TextList							textList;

	while( ! file.IsEOF() )
	{
		file.GetLine( line, sizeof( line ) );

		const int length = strlen( line );

		// 중간의 주석 제거
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = line[ i ];
			const char b = line[ i + 1 ];

			if( '/' == a &&
				'/' == b )
			{
				line[ i ] = 0;
				break;
			}
		}

		textList.push_back( line );

		const char* separator	= " \n\t=,+\"~()";
		const char* token		= strtok( line, separator );
		const char* markBegin	= "{";
		const char* markEnd		= "}";

		if( ! token )
		{
			textList.pop_back();
		}
		// 새로운 클로저가 열리면 헤드를 파싱한다
		else if( 0 == strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			textList.pop_back();

			switch( scriptState )
			{
			case StateNone:
				{
					++openCount;

					scriptState = StateItem;

					for(
						TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it )
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "item" ) )
						{
							const char* itemIndex = strtok( 0, separator );

							scriptList = &( mBodyChangeScriptListMap[ atoi( itemIndex ) ] );
						}
					}

					break;
				}
			case StateItem:
				{
					for(
						TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it )
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "elf" ) )
						{
							scriptState		= StateRace;
							script.mRace	= RaceType_Elf;
						}
						else if( ! stricmp( token, "human" ) )
						{
							scriptState		= StateRace;
							script.mRace	= RaceType_Human;
						}
						// 090520 ONS 신규종족 관련 처리 추가
						else if( ! stricmp( token, "devil" ) )
						{
							scriptState		= StateRace;
							script.mRace	= RaceType_Devil;
						}
					}

					break;
				}
			case StateRace:
				{
					for(
						TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it )
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "hair" ) )
						{
							scriptState		= StatePart;							
							script.mPart	= ePartType_Hair;
						}
						else if( ! stricmp( token, "face" ) )
						{
							scriptState		= StatePart;							
							script.mPart	= ePartType_Face;
						}
					}

					break;
				}
			case StatePart:
				{
					for(
						TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it )
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "female" ) )
						{
							scriptState		= StateGender;							
							script.mGender	= GENDER_FEMALE;
						}
						else if( ! stricmp( token, "male" ) )
						{
							scriptState		= StateGender;							
							script.mGender	= GENDER_MALE;
						}
					}

					break;
				}				
			}

			// 080511 LUJ, 스크립트가 올바로 파싱되도록 수정
			textList.clear();
		}
		else if( 0 == strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			textList.pop_back();

			--openCount;

			switch( scriptState )
			{
			case StateItem:
				{
					ZeroMemory( &script, sizeof( script ) );
					scriptList	= 0;

					scriptState	= StateNone;					
					break;
				}
			case StateRace:
				{
					scriptState	= StateItem;
					break;
				}
			case StatePart:
				{
					scriptState	= StateRace;
					break;
				}
			case StateGender:
				{
					scriptState	= StatePart;

					for(
						TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it )
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						for(
							const char* key = strtok( line, separator );
							key;
							key = strtok( 0, separator ) )
						{
							const char* value = strtok( 0, separator );

							if( ! value )
							{
								continue;
							}

							if( ! stricmp( "value", key ) )
							{
								script.mValue	= atoi( value );
							}
							else if( ! stricmp( "text", key ) )
							{
								script.mText	= atoi( value );
							}
						}

						if( scriptList )
						{
							scriptList->push_back( script );
						}						
					}
					
					break;
				}
			}

			textList.clear();
		}
	}

	if( openCount )
	{
		ASSERT( 0 );
	}
}

// 080414 LUJ, 아이템에 해당하는 외양 변환 스크립트를 반환한다.
const CItemManager::BodyChangeScriptListMap& CItemManager::GetBodyChangeScriptListMap() const
{
	return mBodyChangeScriptListMap;
}

void CItemManager::LoadGiftEvent()
{
	CMHFile file;
	if(!file.Init("System/Resource/Event_get_list.bin", "rb"))
		return;

	char Token[256];
	char* buf;
	int len;

	while(1)
	{
		if(file.IsEOF())
			break;

		file.GetString( Token );
		if( strcmp(Token,"#EventInfo") == 0 )
		{
			while(1)
			{
				if(file.IsEOF())
					break;

				EVENTINFO* pInfo = new EVENTINFO;

				pInfo->Index = file.GetDword();
				pInfo->Delay = file.GetDword();
				pInfo->Next  = file.GetDword();
				pInfo->Item  = file.GetDword();
				pInfo->Count = file.GetDword();

				file.GetString();

				buf = file.GetStringInQuotation();
				len = strlen( buf );
				pInfo->Msg = new char[ len + 1];
				strcpy( pInfo->Msg, buf );
				pInfo->Msg[ len ] = 0;

				mEventInfoTable.Add( pInfo, pInfo->Index );
			}
		}
	}
}

void CItemManager::Item_Gift_Event_Notify( void* pMsg )
{
	MSG_DWORD2* pmsg = ( MSG_DWORD2* )pMsg;

	DWORD eventidx = pmsg->dwData1;
	DWORD state = pmsg->dwData2;

	EVENTINFO* pInfo = mEventInfoTable.GetData( eventidx );

	if( !pInfo )
		return;

	switch( state )
	{
	case 1:	// 이벤트 시작
		{
			if(CSHMonstermeterDlg* const dialog = GAMEIN->GetMonstermeterDlg())
			{
				m_GiftEventTime = pInfo->Delay * 1000;

				dialog->SetGiftEvent(
					TRUE);
				dialog->SetGiftEventTime(
					m_GiftEventTime);
			}
		}
		break;
	case 2: // 이벤트 완료 아이템 지급
		{
			//CHATMGR->AddMsg(CTC_SYSMSG, pInfo->Msg) ;

			GAMEIN->GetItemPopupDlg()->SetText( pInfo->Msg );
			GAMEIN->GetItemPopupDlg()->SetActive( TRUE );
		}
		break;
	}
}

// 080820 LUJ, 획득 시 쿨타임을 가진 아이템은 이동 시에 시간을 업데이트해줘야한다. 서버는 알아서 갱신한다
void CItemManager::UpdateCoolTimeItem( CItem& item )
{
	const ITEM_INFO* info = GetItemInfo( item.GetItemIdx() );

	if( !	info ||
			info->SupplyType != ITEM_KIND_COOLTIME )
	{
		return;
	}

	item.SetRemainTime( item.GetRemainedCoolTime() / 1000 );
}

const MixSupportScript* CItemManager::GetMixSupportScript( DWORD itemIndex ) const
{
	const MixSupportScriptMap::const_iterator it = mMixSupportScriptMap.find( itemIndex );

	return mMixSupportScriptMap.end() == it ? 0 : &( it->second );
}

void CItemManager::UseItem_WarterSeed(CItem* pItem)
{
	if(!pItem)
	{
#ifdef _GMTOOL_
   		MessageBox( NULL, "Invalid item!!", __FUNCTION__, MB_OK ) ;
#endif //_GMTOOL_
   		return ;
	}
   	else if( GUILDMGR->GetLevel() <  SIEGEWARFAREMGR->Get_WaterSeedLimitGuildLevel() )
   	{
   		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1693 ) ) ;
   		return ;
   	}
   
   	CObject* pTarget = OBJECTMGR->GetSelectedObject() ;
   
   	if( !pTarget )
   	{
   		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg(1689) ) ;
   		return ;
   	}
   
   	WORD wNpcJob = ((CNpc*)pTarget)->GetNpcJob() ;
   
   	if( pTarget->GetObjectKind() != eObjectKind_Npc || wNpcJob != SIEGEWELL_ROLE )
   	{
   		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg(1043) ) ;
   		return ;
   	}
   
   
	// 081017 LYW --- ItemManager : 워터시드 사용 시, 우물과의 거리를 체크하는 부분 추가.
	// HERO의 위치를 받는다.
	VECTOR3 HeroPos = HERO->GetCurPosition() ;


	// 워터시드의 위치를 받는다.
	VECTOR3 TargetPos = pTarget->GetCurPosition() ;


	// 거리를 체크한다.
	float fDistance = 0.0f ;
	fDistance = CalcDistanceXZ( &HeroPos, &TargetPos ) ;

	if( fDistance > SIEGEWARFAREMGR->Get_CravingDistance() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1720 ) ) ;
		return ;
	}

	// 090706 pdy 클라이언트 워터시드 시간 조작 문제로 수정 
   	MSG_SIEGEWARFARE_USE_WATERSEED_SYN msg ;
   	memset(&msg, 0, sizeof(MSG_SIEGEWARFARE_USE_WATERSEED_SYN)) ;
   
   	msg.Category	= MP_SIEGEWARFARE ;
   	msg.Protocol	= MP_SIEGEWARFARE_USE_WATERSEED_SYN ;
   
   	msg.dwObjectID	= gHeroID ;
   
   	msg.wItemIdx	= pItem->GetItemIdx() ;
   	msg.TargetPos	= pItem->GetItemBaseInfo().Position ;

	//클라이언트의 워터시드 시간도 함께 보넨다.
	msg.dwWaterSeedUsingSecond = SIEGEWARFAREMGR->Get_WaterSeedUsingSecond();
   
   	NETWORK->Send( &msg, sizeof(MSG_SIEGEWARFARE_USE_WATERSEED_SYN) ) ;
   
   
   	// 캐릭터의 상태를 Deal 상태로 세팅한다.
   	OBJECTSTATEMGR->StartObjectState( HERO, eObjectState_Deal ) ;
}

// 091215 ONS 아이템Stack최대수를 아이템별로 지정된 값으로 처리하기위해 
// 해당 아이템의 stack수를 반환한다.
WORD CItemManager::GetItemStackNum( DWORD dwItemIndex )
{
	const ITEM_INFO* pInfo = GetItemInfo( dwItemIndex );

	return 0 == pInfo ? 0 : pInfo->Stack;
}

// 100517 ONS 전직초기화 아이템 처리 추가
void CItemManager::UseItem_ChangeClass( CItem* pItem )
{
	if( !pItem )
	{
		return;
	}

	CChangeClassDlg* pDlg = GAMEIN->GetChangeClassDlg();
	if( !pDlg )
	{
		return;
	}

	MSG_ITEM_CHANGE_CLASS msg;														
	ZeroMemory(&msg, sizeof(msg));

	msg.Category	= MP_ITEM;														
	msg.Protocol	= MP_ITEM_CHANGE_CLASS_SYN;
	msg.dwObjectID	= gHeroID;

	msg.wItemIdx	= pItem->GetItemIdx();											
	msg.TargetPos	= pItem->GetItemBaseInfo().Position;

	CHARACTER_TOTALINFO ChaInfo;
	OBJECTMGR->GetHero()->GetCharacterTotalInfo(&ChaInfo);
	for( WORD Stage = 0; Stage < ChaInfo.JobGrade - 1; Stage++)
	{
		int nIndex = pDlg->GetCurSelectedIdx(Stage);
		if( nIndex > -1 )
		{
			const ITEM* pSelectItem = pDlg->GetItem(Stage, nIndex);
			if( !pSelectItem ) break;

			msg.ClassStage[Stage] = (BYTE)pSelectItem->line + 1;
		}
		else
		{
			msg.ClassStage[Stage] = ChaInfo.Job[Stage+1];
		}
	}

	NETWORK->Send( &msg, sizeof(MSG_ITEM_CHANGE_CLASS) );									
}

// 100609 ONS 인챈트 +10이상 공지를 큐에 저장한다.
void CItemManager::EnchantNotifyAdd( void* pMsg )
{
	const MSG_NAME_DWORD3* pmsg = (MSG_NAME_DWORD3*)pMsg;
	const DWORD dwItemIdx		= pmsg->dwData1;
	const DWORD dwEnchantLevel	= pmsg->dwData2;
	const DWORD  bIsSuccessed	= pmsg->dwData3;
	const char* const pName		= pmsg->Name;

	if(  0 == dwItemIdx		||
		10 > dwEnchantLevel	||
		NULL == pName		)
	{
		return;
	}

	EnchantNotifyInfo Info;
	ZeroMemory(&Info, sizeof(EnchantNotifyInfo));

	Info.ItemIndex		= dwItemIdx;
	Info.EnchantLevel	= dwEnchantLevel;
	Info.IsSuccessed	= (BYTE)bIsSuccessed;
	SafeStrCpy( Info.Name, pName, MAX_NAME_LENGTH + 1 );

	m_EnchantNotifyQueue.push( Info );
}

WORD CItemManager::GetItemList(const char* pKeyWord, const char* pCategory1, const char* pCategory2, int nRareLevel, WORD wMinLevel, WORD wMaxLevel, WORD wNum, DWORD* pItemList)
{
	if(0==wNum || 0==pItemList)
		return 0;

	if(strlen(pKeyWord) < 4)
	{
		return 0;
	}

	CConsignmentDlg* pDlg = GAMEIN->GetConsignmentDlg();
	if(! pDlg)
		return 0;

	ITEM_INFO* pInfo;
	m_ItemInfoList.SetPositionHead();

	WORD wCount = 0;
	DWORD dwCategory1 = 0;
	DWORD dwCategory2 = 0;
	const WORD wMaxListDialogCount = 99;

	DWORD dwItemCategory1 = 0;
	DWORD dwItemCategory2 = 0;
	BOOL bOverCount = FALSE;
	while(NULL != (pInfo = m_ItemInfoList.GetData()))
	{
		if(wMaxListDialogCount < wCount)
		{
			return wMaxListDialogCount;
		}
		else if(wNum <= wCount)
		{
			bOverCount = TRUE;
		}

		if(FALSE==pInfo->Trade || FALSE==pInfo->Sell)
			continue;

		dwCategory1 = dwCategory2 = 0;
		dwItemCategory1 = dwItemCategory2 = 0;

		if(NULL != pCategory1)
			dwCategory1 = pDlg->GetCategory1Value(pCategory1);

		if(NULL != pCategory2)
			dwCategory2 = pDlg->GetCategory2Value(pCategory2);

		pDlg->GetCategoryValueByItemIndex(pInfo->ItemIdx, dwItemCategory1, dwItemCategory2);
		if(0!=dwCategory1 && dwItemCategory1!=dwCategory1)
			continue;

		if(0!=dwCategory2 && dwItemCategory2!=dwCategory2)
			continue;

		if(-1!=nRareLevel && nRareLevel!=pInfo->kind)
			continue;

		if(pInfo->LimitLevel<wMinLevel || wMaxLevel<pInfo->LimitLevel)
			continue;

		char keyword[256] = {0,};
		SafeStrCpy(keyword, pKeyWord, 256-1);
		if(NULL!=pKeyWord && NULL==strstr(_strupr(pInfo->ItemName), _strupr(keyword)))
			continue;

		if(FALSE == bOverCount)
			pItemList[wCount] = pInfo->ItemIdx;

		wCount++;

		DWORD dwColor = GetItemNameColor( *pInfo );
		pDlg->AddItemToKeywordList(pInfo->ItemName, dwColor);
	}

	return wCount;
}

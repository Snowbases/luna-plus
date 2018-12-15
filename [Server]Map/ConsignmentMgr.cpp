#include "stdafx.h"
#include "Consignmentmgr.h"
#include "Protocol.h"
#include "ItemManager.h"
#include "Player.h"
#include "UserTable.h"
#include "MapDBMsgParser.h"
#include "LootingManager.h"
#include "CheckRoutine.h"

GLOBALTON(CConsignmentMgr)

CConsignmentMgr::CConsignmentMgr(void)
{
}

CConsignmentMgr::~CConsignmentMgr(void)
{
}

void CConsignmentMgr::NetworkMsgParser(DWORD dwConnectionIndex, BYTE Protocol, void* pMsg, DWORD dwLength)
{
	switch( Protocol )
	{
	case MP_CONSIGNMENT_GETLIST_SYN:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(! pPlayer)
				return;

			Consignment_GetList(pPlayer->GetID());
		}
		break;

	case MP_CONSIGNMENT_REGIST_SYN:
		{
			MSG_CONSIGNMENT_REGIST* pmsg = (MSG_CONSIGNMENT_REGIST*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(! pPlayer)
				return;

			CItemSlot * pFromSlot	= pPlayer->GetSlot(pmsg->FromPos);
			if(! pFromSlot)
				return;

			if(pFromSlot->IsLock(pmsg->FromPos))
				return;

			const ITEM_OPTION ItemOption = ITEMMGR->GetOption(pmsg->ItemInfo);

			// 등록가능한지 체크
			const ITEMBASE * pItemBase = pFromSlot->GetItemInfoAbs(pmsg->FromPos);
			if(! pItemBase)
				return;

			ITEM_INFO* pItem = ITEMMGR->GetItemInfo(pmsg->ItemInfo.wIconIdx);
			if(! pItem)
				return;

			DWORD dwErrorKind = eConsignmentERR_None;

			// 1초 이내에 들어오는 요청은 무시
			if(pPlayer->GetConsignmentTick() + 1000 > gCurTime)
				dwErrorKind = eConsignmentERR_FastRequest;

			if( LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) )
				dwErrorKind = eConsignmentERR_WrongState;

			if(!CHKRT->ItemOf(pPlayer, pmsg->FromPos, pmsg->ItemInfo.wIconIdx,0,0,CB_EXIST|CB_ICONIDX))
				dwErrorKind = eConsignmentERR_WrongItem;

			if( pItemBase->nSealed == eITEM_TYPE_UNSEAL )
				dwErrorKind = eConsignmentERR_WrongItem;

			if( FALSE==pItem->Trade || FALSE==pItem->Sell)
				dwErrorKind = eConsignmentERR_WrongItem;

			DWORDEX dwMoneyCheck = pItemBase->Durability ? pItemBase->Durability * pmsg->dwPrice : pmsg->dwPrice;
			if( dwMoneyCheck >= MAX_INVENTORY_MONEY)
				dwErrorKind = eConsignmentERR_GoldOver;

			DWORD dwDurability = pItemBase->Durability ? pItemBase->Durability : 1;
			if( pmsg->dwDeposit > pPlayer->GetMoney() )
				dwErrorKind = eConsignmentERR_PoorMoney;

			float fDeposit = (float)((pmsg->dwPrice * dwDurability) * CONSIGNMENT_DEPOSIT_RATE);
			fDeposit = ceil(fDeposit);
			float fCommition = (float)(pmsg->dwPrice * CONSIGNMENT_COMMISSION_RATE);
			fCommition = ceil(fCommition);
			if((DWORD)fDeposit!=pmsg->dwDeposit || (DWORD)fCommition!=pmsg->dwCommission)
				dwErrorKind = eConsignmentERR_WrongParam;

			if(IsCharInString(pmsg->szItemName, "'"))
				dwErrorKind = eConsignmentERR_WrongParam;

			if(dwErrorKind != eConsignmentERR_None)
			{
				MSG_DWORD5 msg;
				msg.Category = MP_CONSIGNMENT;
				msg.Protocol = MP_CONSIGNMENT_REGIST_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData1 = dwErrorKind;
				msg.dwData2 = pmsg->ItemInfo.dwDBIdx;	// ItemDBIndex
				msg.dwData3 = pmsg->ItemInfo.wIconIdx;	// ItemIndex
				msg.dwData4 = 0;						// 소모된비용
				msg.dwData5 = pmsg->FromPos;			// 아이템슬롯
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}

			pFromSlot->SetLock(pmsg->FromPos, TRUE);
			pPlayer->SetConsignmentTick();
			
			// DB에 등록요청
			Consignment_Regist(pPlayer->GetID(), pmsg->ItemInfo.dwDBIdx, pmsg->ItemInfo.wIconIdx, pPlayer->GetObjectName(), pmsg->szItemName,
				(int)pmsg->dwCategory1Value, (int)pmsg->dwCategory2Value,
				pItem->kind, pItem->LimitLevel, ItemOption.mEnchant.mLevel,
				pmsg->dwDeposit, pmsg->dwCommission, pmsg->dwPrice, pmsg->FromPos, pItem->Stack);
		}
		break;

	case MP_CONSIGNMENT_SEARCH_SYN:
		{
			MSG_CONSIGNMENT_SEARCH_REQUEST* pmsg = (MSG_CONSIGNMENT_SEARCH_REQUEST*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(! pPlayer)
				return;

			DWORD dwErrorKind = eConsignmentERR_None;

			// 1초 이내에 들어오는 요청은 무시
			if(pPlayer->GetConsignmentTick() + 1000 > gCurTime)
				dwErrorKind = eConsignmentERR_FastRequest;

			if( LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) )
				dwErrorKind = eConsignmentERR_WrongState;

			// 파마미터 값체크 하자....
			if(dwErrorKind != eConsignmentERR_None)
			{
				MSG_DWORD msg;
				msg.Category = MP_CONSIGNMENT;
				msg.Protocol = MP_CONSIGNMENT_SEARCH_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = dwErrorKind;
			}

			pPlayer->SetConsignmentTick();

			// DB에 검색요청
			Consignment_Search(pPlayer->GetID(), pmsg->wSortType, pmsg->wStartPage, pmsg->nCategory1, pmsg->nCategory2, pmsg->nMinLevel, pmsg->nMaxLevel, pmsg->nRareLevel);
		}
		break;

	case MP_CONSIGNMENT_SEARCHINDEX_SYN:
		{
			MSG_CONSIGNMENT_SEARCHINDEX_REQUEST* pmsg = (MSG_CONSIGNMENT_SEARCHINDEX_REQUEST*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(! pPlayer)
				return;

			DWORD dwErrorKind = eConsignmentERR_None;

			// 1초 이내에 들어오는 요청은 무시
			if(pPlayer->GetConsignmentTick() + 1000 > gCurTime)
				dwErrorKind = eConsignmentERR_FastRequest;

			if( LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) )
				dwErrorKind = eConsignmentERR_WrongState;

			// 파마미터 값체크 하자....
			if(dwErrorKind != eConsignmentERR_None)
			{
				MSG_DWORD msg;
				msg.Category = MP_CONSIGNMENT;
				msg.Protocol = MP_CONSIGNMENT_SEARCH_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = dwErrorKind;
			}

			pPlayer->SetConsignmentTick();

			// DB에 검색요청
			Consignment_SearchIndex(pPlayer->GetID(), pmsg->wSortType, pmsg->wStartPage, pmsg->dwItemList);
		}
		break;

	case MP_CONSIGNMENT_UPDATE_SYN:
		{
			MSG_CONSIGNMENT_UPDATE* pmsg = (MSG_CONSIGNMENT_UPDATE*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(! pPlayer)
				return;

			ITEM_SEAL_TYPE wSeal = eITEM_TYPE_SEAL_NORMAL;
			ITEM_INFO* pItemInfo = NULL;
			int nErrorKind = eConsignmentERR_None;

			// 1초 이내에 들어오는 요청은 무시
			if(pPlayer->GetConsignmentTick() + 1000 > gCurTime)
				nErrorKind = eConsignmentERR_FastRequest;

			if(pmsg->wUpdateKind == eConsignmentUPDATEKIND_Buy)
			{
				// 빈슬롯체크
				CItemSlot* pSlot = pPlayer->GetSlot(eItemTable_Inventory);
				if(NULL == pSlot) ASSERTMSG(0,"올바른 아이템 슬롯을 얻을 수 없습니다.");

				if( LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) )
					nErrorKind = eConsignmentERR_WrongState;

				// 골드체크
				if(pmsg->dwBuyTotalPrice > pPlayer->GetMoney())
					nErrorKind = eConsignmentERR_PoorMoney;

				if(0==pmsg->wBuyDurability || 0==pmsg->dwBuyTotalPrice)
					nErrorKind = eConsignmentERR_WrongState;

				pItemInfo = ITEMMGR->GetItemInfo(pmsg->dwBuyItemIndex);
				if(!pItemInfo)
					nErrorKind = eConsignmentERR_WrongItem;

				// 스택되지 않는 아이템은 수량을 0으로 재조정
				if(! pItemInfo->Stack)
					pmsg->wBuyDurability = 0;

				wSeal = pItemInfo->wSeal;
			}

			pPlayer->SetConsignmentTick();


			if(nErrorKind != eConsignmentERR_None)
			{
				MSG_CONSIGNMENT_UPDATERESULT msg;
				ZeroMemory(&msg, sizeof(msg));

				msg.Category = MP_CONSIGNMENT;
				msg.Protocol = MP_CONSIGNMENT_UPDATE_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.nResult = nErrorKind;
				pPlayer->SendMsg( &msg, sizeof(msg) );
				return;
			}

			Consignment_Update(pPlayer->GetID(), pmsg->dwConsignmentIndex, pmsg->wUpdateKind, pmsg->wBuyDurability, wSeal);
		}
		break;
	}
}

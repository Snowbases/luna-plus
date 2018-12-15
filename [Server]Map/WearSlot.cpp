#include "stdafx.h"
#include "WearSlot.h"
#include "CharacterCalcManager.h"
#include "PackedData.h"
#include "UserTable.h"

#include "MapDBMsgParser.h"
#include "Player.h"
#include "ObjectStateManager.h"
//#include "ItemManager.h"
//#include "..\[cc]header\CommonStruct.h"


CWearSlot::CWearSlot()
{}

CWearSlot::~CWearSlot()
{}


//BOOL CWearSlot::IsAddable(POSTYPE absPos, ITEMBASE * pItemBase)
//{
//	return TRUE;
//}


//void CWearSlot::ApplyItem(CPlayer* pPlayer)
//{
//	// 횄횜?청횊짯 횉횎쩔채!
//	// Charactertable쩔징 ?횜쨈횂 횁짚쨘쨍 횁철쩔챵
//	CharacterTotalInfoUpdate(pPlayer);
//
//	// 쩐횈?횑횇횤 쩍쨘횇횦 쨘짱째챈
//	CHARCALCMGR->CalcItemStats(pPlayer);
//
//	// 횄횜?청횊짯 횉횎쩔채!
//	APPEARANCE_INFO msg;
//	msg.Category = MP_ITEM;
//	msg.Protocol = MP_ITEM_APPEARANCE_CHANGE;
//	msg.PlayerID = pPlayer->GetID();
//	for(int n=0;n<eWearedItem_Max;++n)
//	{
//		msg.WearedItem[n] = pPlayer->GetWearedItemIdx(n);
//	}
//
//	PACKEDDATA_OBJ->QuickSendExceptObjectSelf(pPlayer, &msg, sizeof(msg));
//}


ERROR_ITEM CWearSlot::InsertItemAbs(CPlayer * pPlayer, POSTYPE absPos, ITEMBASE* pItem, WORD state)
{
	const ERROR_ITEM rt = CItemSlot::InsertItemAbs(pPlayer, absPos, pItem);

	if(rt != EI_TRUE)	
	{
		return rt;
	}

	if( pPlayer->GetState() == eObjectState_Immortal )
	{
		OBJECTSTATEMGR_OBJ->EndObjectState( pPlayer, eObjectState_Immortal );
	}

	pPlayer->SetWearedItemIdx(absPos - m_StartAbsPos, pItem->wIconIdx);
	//ApplyItem(pPlayer);

	CHARCALCMGR->AddItem( pPlayer, *pItem );

	CharacterTotalInfoUpdate(pPlayer);

	{
		MSG_APPERANCE_ADD message;
		message.Category	= MP_ITEM;
		message.Protocol	= MP_ITEM_APPEARANCE_ADD;
		message.dwObjectID	= pPlayer->GetID();
		message.mSlotIndex	= absPos - m_StartAbsPos;
		message.mItemIndex	= pItem->wIconIdx;

		PACKEDDATA_OBJ->QuickSendExceptObjectSelf( pPlayer, &message, sizeof( message ) );
	}

	return EI_TRUE;
}

ERROR_ITEM CWearSlot::UpdateItemAbs(CPlayer * pPlayer, POSTYPE whatAbsPos, DWORD dwDBIdx, DWORD wItemIdx, POSTYPE position, POSTYPE quickPosition, DURTYPE Dur, WORD flag, WORD state)
{
	// 아이템 분할, 병합, 강화 등 정보가 변경되었을 때 호출됨. 인벤토리에 든 아이템은 호출되지 않음. 
	// 인벤토리에 든 아이템에서 이 메소드가 호출될 경우는 정상적으로 플레이어 상태가 갱신되지 않을 것이다... 주의바람

	const ERROR_ITEM rt = CItemSlot::UpdateItemAbs(pPlayer, whatAbsPos, dwDBIdx, wItemIdx, position, quickPosition, Dur, flag, state);

	if( rt != EI_TRUE )
	{
		if( pPlayer->GetState() == eObjectState_Immortal )
		{
			OBJECTSTATEMGR_OBJ->EndObjectState( pPlayer, eObjectState_Immortal );
		}

		if(UB_ICONIDX & flag)
		{
			pPlayer->SetWearedItemIdx(whatAbsPos - m_StartAbsPos, wItemIdx);

			//ApplyItem(pPlayer);
		}
	}

	return rt;
}


ERROR_ITEM CWearSlot::DeleteItemAbs(CPlayer * pPlayer, POSTYPE absPos, ITEMBASE * pItemOut, WORD state)
{
	const ERROR_ITEM rt = CItemSlot::DeleteItemAbs(pPlayer, absPos, pItemOut);

	if(rt != EI_TRUE)
	{
		return rt;
	}

	if( pPlayer->GetState() == eObjectState_Immortal )
	{
		OBJECTSTATEMGR_OBJ->EndObjectState( pPlayer, eObjectState_Immortal );
	}

	pPlayer->SetWearedItemIdx(absPos - m_StartAbsPos, 0);

	//ApplyItem(pPlayer);

	CHARCALCMGR->RemoveItem( pPlayer, *pItemOut );

	CharacterTotalInfoUpdate(pPlayer);

	{
		MSG_APPERANCE_REMOVE message;
		message.Category	= MP_ITEM;
		message.Protocol	= MP_ITEM_APPEARANCE_REMOVE;
		message.mSlotIndex	= absPos - m_StartAbsPos;
		message.dwObjectID	= pPlayer->GetID();
		
		PACKEDDATA_OBJ->QuickSendExceptObjectSelf( pPlayer, &message, sizeof( message ) );
	}
	
	return EI_TRUE;
}
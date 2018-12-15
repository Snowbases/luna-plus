#include "cCookManager.h"
#include "MHFile.h"
#include "UserTable.h"
#include "Player.h"
#include "Npc.h"
#include "ItemManager.h"
#include "ItemSlot.h"
#include "ItemContainer.h"
#include "PackedData.h"
#include "MapDBMsgParser.h"
#include "..\[CC]Header\GameResourceManager.h"

#define MAX_COOK_DELAY 2000

cCookManager::cCookManager(void)
{
	m_CookerList.Initialize(100);
	m_FireNpcList.Initialize(5);

	Init();
}

cCookManager::~cCookManager(void)
{
	DWORD* pIndex = NULL;
	m_CookerList.SetPositionHead();
	while((pIndex = m_CookerList.GetData()) != NULL)
	{
		delete pIndex;
		pIndex = NULL;
	}

	m_FireNpcList.SetPositionHead();
	while((pIndex = m_FireNpcList.GetData()) != NULL)
	{
		delete pIndex;
		pIndex = NULL;
	}

	std::map<DWORD, stRecipeInfo*>::iterator iter;

	for(iter=m_mapRecipeLv1.begin(); iter!=m_mapRecipeLv1.end(); iter++)
	{
		if(iter->second)
			SAFE_DELETE(iter->second);
	}
	m_mapRecipeLv1.clear();

	for(iter=m_mapRecipeLv2.begin(); iter!=m_mapRecipeLv2.end(); iter++)
	{
		if(iter->second)
			SAFE_DELETE(iter->second);
	}
	m_mapRecipeLv2.clear();

	for(iter=m_mapRecipeLv3.begin(); iter!=m_mapRecipeLv3.end(); iter++)
	{
		if(iter->second)
			SAFE_DELETE(iter->second);
	}
	m_mapRecipeLv3.clear();

	for(iter=m_mapRecipeLv4.begin(); iter!=m_mapRecipeLv4.end(); iter++)
	{
		if(iter->second)
			SAFE_DELETE(iter->second);
	}
	m_mapRecipeLv4.clear();
}

cCookManager* cCookManager::GetInstance()
{
	static cCookManager instance;

	return &instance;
}

void cCookManager::Init()
{
	CMHFile file;
	char filename[256] = {0,};
	char string[256] = {0,};
	int nLevel = 0;
	
	sprintf(filename,"system/Resource/Cooking.bin");

	if( file.Init( filename, "rb" ) )
	{
		while(!file.IsEOF())
		{
			file.GetString(string);

			if(string[0] == '@')
			{
				file.GetLineX(string, 256);
				continue;
			}
			else if(string[0] == '{')
			{
				continue;
			}
			else if(string[0] == '}')
			{
				continue;
			}
			else if(0==strcmp(string, "#FIRENPCIDX"))
			{
				DWORD* pFireNpcIdx = new DWORD;
				if(pFireNpcIdx)
				{
					*pFireNpcIdx = file.GetDword();
					m_FireNpcList.Add(pFireNpcIdx, *pFireNpcIdx);
				}
			}
			else if(0==strcmp(string, "#COOKCOUNT"))
			{
				m_wMaxCookCount[0] = file.GetWord();
				m_wMaxCookCount[1] = file.GetWord();
				m_wMaxCookCount[2] = file.GetWord();
				m_wMaxCookCount[3] = file.GetWord();
			}
			else if(0==strcmp(string, "#EATCOUNT"))
			{
				m_wMaxEatCount[0] = file.GetWord();
				m_wMaxEatCount[1] = file.GetWord();
				m_wMaxEatCount[2] = file.GetWord();
				m_wMaxEatCount[3] = file.GetWord();
			}
			else if(0==strcmp(string, "#FIRECOUNT"))
			{
				m_wMaxFireCount[0] = file.GetWord();
				m_wMaxFireCount[1] = file.GetWord();
				m_wMaxFireCount[2] = file.GetWord();
				m_wMaxFireCount[3] = file.GetWord();
			}
			else if(0==strcmp(string, "#REWARD"))
			{
				m_dwLevelUpReward[0] = file.GetDword();
				m_dwLevelUpReward[1] = file.GetDword();
				m_dwLevelUpReward[2] = file.GetDword();
				m_dwLevelUpReward[3] = file.GetDword();
			}
			else if(0==strcmp(string, "#RECIPE_LV1"))
			{
				nLevel = eCOOKLEVEL1;
			}
			else if(0==strcmp(string, "#RECIPE_LV2"))
			{
				nLevel = eCOOKLEVEL2;
			}
			else if(0==strcmp(string, "#RECIPE_LV3"))
			{
				nLevel = eCOOKLEVEL3;
			}
			else if(0==strcmp(string, "#RECIPE_LV4"))
			{
				nLevel = eCOOKLEVEL4;
			}
			else if(0==strcmp(string, "#RECIPE"))
			{				
				stRecipeInfo* pAddInfo = new stRecipeInfo;
				if(pAddInfo)
				{
					DWORD dwIndex = file.GetDword();
					if(nLevel != GetCookLevelFromRecipe(dwIndex))
					{
						SAFE_DELETE(pAddInfo);
						continue;
					}

					pAddInfo->dwRecipeIdx = dwIndex;
					pAddInfo->dwExpertPointMin = file.GetDword();
					pAddInfo->dwExpertPointMax = file.GetDword();
					pAddInfo->dwRemainTime = file.GetDword();
					pAddInfo->dwFoodItemIdx = file.GetDword();

					int i;
					for(i=0; i<MAX_INGREDIENT_LIST; i++)
					{
						pAddInfo->Ingredients[i].dwItemIdx = file.GetDword();
						pAddInfo->Ingredients[i].wQuantity = file.GetWord();
					}
				}

				if(eCOOKLEVEL1 == nLevel)
				{
					m_mapRecipeLv1.insert(std::make_pair(pAddInfo->dwRecipeIdx, pAddInfo));
				}
				else if(eCOOKLEVEL2 == nLevel)
				{
					m_mapRecipeLv2.insert(std::make_pair(pAddInfo->dwRecipeIdx, pAddInfo));
				}
				else if(eCOOKLEVEL3 == nLevel)
				{
					m_mapRecipeLv3.insert(std::make_pair(pAddInfo->dwRecipeIdx, pAddInfo));
				}
				else if(eCOOKLEVEL4 == nLevel)
				{
					m_mapRecipeLv4.insert(std::make_pair(pAddInfo->dwRecipeIdx, pAddInfo));
				}
			}
		}
	}
	file.Release();
}

void cCookManager::Cook_Syn(void* pMsg)
{
	WORD wResult = eCookError_None;
	MSG_COOK_SYN* pmsg = (MSG_COOK_SYN*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	
	if(pPlayer)
	{
		// 스피드핵 체크
		if(0 < pPlayer->GetLastCookTime())
		{
			wResult=eCookError_InvaildState;
			goto COOK_NACK;
		}

		// 요리도구확인
		WORD wCookUtilLevel = CheckCookUtil(pPlayer);
		if(!wCookUtilLevel)
		{
			wResult=eCookError_InvaildUtil;			
			goto COOK_NACK;
		}

		// 레시피확인
		stRecipeInfo* pRecipe = GetRecipeInfo(pmsg->dwRecipeIdx);
		if(!pRecipe)
		{
			wResult=eCookError_InvaildRecipe;
			goto COOK_NACK;
		}

		// 요리도구-레시피 레벨확인
		if(wCookUtilLevel<eCOOKLEVEL3 && wCookUtilLevel<COOKMGR->GetCookLevelFromRecipe(pRecipe->dwRecipeIdx))
		{
			wResult=eCookError_LowerUtil;
			goto COOK_NACK;
		}

		// 모닥불확인
		CNpc* pFire = (CNpc*)g_pUserTable->FindUser(pmsg->dwFireNpcIdx);
		if(!pFire || eObjectKind_Npc!=pFire->GetObjectKind())
		{
			wResult=eCookError_InvaildFire;
			goto COOK_NACK;
		}

		// 제작수량확인
		if(pmsg->wMakeNum<1 || 99<pmsg->wMakeNum)
		{
			wResult=eCookError_InvaildMakeNum;
			goto COOK_NACK;
		}

		// 거리제한
		VECTOR3 vHeroPos, vPlacePos;
		pPlayer->GetPosition(&vHeroPos) ;
		pFire->GetPosition(&vPlacePos);
		float fDistance = CalcDistanceXZ(&vHeroPos, &vPlacePos);
		if(MAX_COOKING_DISTANCE < fDistance)
		{
			wResult = eCookError_OverDistance;
			goto COOK_NACK;
		}

		CItemSlot* pInvenSlot = pPlayer->GetSlot(eItemTable_Inventory);
		if(0 == ITEMMGR->GetTotalEmptySlotNum(pInvenSlot, pPlayer))
		{
			wResult = eCookError_InvenFull;
			goto COOK_NACK;
		}

		// 요리재료확인 및 소모
		if(!CheckIngredients(pPlayer, pRecipe, pmsg->wMakeNum))
		{
			wResult = eCookError_NeedIngredient;
			goto COOK_NACK;
		}

		// 숙련도확인 및 증가
		WORD wCookCount = pPlayer->GetCookCount(); 
		WORD wAddPoint = 0;
		if(pPlayer->GetCookLevel() == GetCookLevelFromRecipe(pRecipe->dwRecipeIdx))
		{
			WORD wExpertPointMin = (WORD)pRecipe->dwExpertPointMin;
			WORD wExpertPointMax = (WORD)pRecipe->dwExpertPointMax;
			if(pPlayer->GetCookCount() < wExpertPointMin)
			{
				wResult = eCookError_LowerExpertPoint;
				goto COOK_NACK;
			}
			
			if(wExpertPointMax < wCookCount)
				wAddPoint = 0;
			else if(wExpertPointMax <= wCookCount+pmsg->wMakeNum)
				wAddPoint = wExpertPointMax - wCookCount;
			else
				wAddPoint = pmsg->wMakeNum;

			if(wCookCount+wAddPoint < COOKMGR->GetMaxCookCount(pPlayer->GetCookLevel()))
				pPlayer->SetCookCount(wCookCount + wAddPoint);
			else
				pPlayer->SetCookCount(COOKMGR->GetMaxCookCount(pPlayer->GetCookLevel()));
		}

		Cooking_Update(pPlayer);
		Cooking_Log(pPlayer->GetID(), eCookingLog_CookCount, pRecipe->dwRecipeIdx, wCookCount, wAddPoint, pPlayer->GetCookCount());

		DWORD* pPlayerIdx = new DWORD;
		if(pPlayerIdx)
		{
			*pPlayerIdx = pPlayer->GetID();
			pPlayer->SetLastCookTime(gCurTime);
			m_CookerList.Add(pPlayerIdx, *pPlayerIdx);
		}

		MSG_DWORD2 msgAck;
		msgAck.Category = MP_COOK;
		msgAck.Protocol = MP_COOK_ACK;
		msgAck.dwObjectID = pPlayer->GetID();
		msgAck.dwData1 = pRecipe->dwRecipeIdx;
		msgAck.dwData2 = pmsg->wMakeNum;
		pPlayer->SendMsg(&msgAck, sizeof(msgAck));

		MSG_DWORD4 msg;
		msg.Category = MP_COOK;
		msg.Protocol = MP_COOK_STATE;
		msg.dwObjectID = pPlayer->GetID();
		msg.dwData1 = pPlayer->GetCookLevel();
		msg.dwData2 = pPlayer->GetCookCount();
		msg.dwData3 = pPlayer->GetEatCount();
		msg.dwData4 = pPlayer->GetFireCount();
		pPlayer->SendMsg(&msg, sizeof(msg));

		return;
	}


COOK_NACK:
	MSG_DWORD msg;
	msg.Category = MP_COOK;
	msg.Protocol = MP_COOK_NACK;
	msg.dwObjectID = pPlayer->GetID();
	msg.dwData = wResult;
	PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
}

void cCookManager::NetworkMsgParse( BYTE Protocol, void* pMsg, DWORD dwLength )
{
	switch(Protocol)
	{
	case MP_COOK_SYN:
			Cook_Syn(pMsg);	
		break;
	case MP_COOK_CHEAT:
		{
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			int nCheck = 0;
			if(0<pmsg->dwData1 && pmsg->dwData1<5)
				nCheck++;
			if(0<=pmsg->dwData2 && pmsg->dwData2<=m_wMaxCookCount[pmsg->dwData1])
				nCheck++;
			if(0<=pmsg->dwData3 && pmsg->dwData3<=m_wMaxEatCount[pmsg->dwData1])
				nCheck++;
			if(0<=pmsg->dwData4 && pmsg->dwData4<=m_wMaxFireCount[pmsg->dwData1])
				nCheck++;

			if(nCheck==4)
			{
				pPlayer->SetCookLevel((WORD)pmsg->dwData1);
				pPlayer->SetCookCount((WORD)pmsg->dwData2);
				pPlayer->SetEatCount((WORD)pmsg->dwData3);
				pPlayer->SetFireCount((WORD)pmsg->dwData4);

				MSG_DWORD4 msg;
				msg.Category = MP_COOK;
				msg.Protocol = MP_COOK_STATE;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData1 = pPlayer->GetCookLevel();
				msg.dwData2 = pPlayer->GetCookCount();
				msg.dwData3 = pPlayer->GetEatCount();
				msg.dwData4 = pPlayer->GetFireCount();
				pPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
		break;
	default:
		break;
	}
}

void cCookManager::Process()
{
	m_CookerList.SetPositionHead();

	while(DWORD* pPlayerIdx = m_CookerList.GetData())
	{
		CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(*pPlayerIdx);

		if(pPlayer || pPlayer->GetLastCookTime()+MAX_COOK_DELAY > gCurTime)
		{
			m_CookerList.Remove(*pPlayerIdx);
			pPlayer->SetLastCookTime(0);

			SAFE_DELETE(pPlayerIdx);
			continue;
		}
	}
}

bool cCookManager::CanLevelUp(CPlayer* pPlayer, WORD wNextLevel)
{
	WORD wCurLevel = pPlayer->GetCookLevel();
	if(wNextLevel<=wCurLevel || wCurLevel<eCOOKLEVEL1 || eCOOKLEVEL4<=wCurLevel)
		return false;

	if(pPlayer->GetCookCount() < m_wMaxCookCount[wCurLevel])
		return false;

	if(pPlayer->GetEatCount() < m_wMaxEatCount[wCurLevel])
		return false;

	if(pPlayer->GetFireCount() < m_wMaxFireCount[wCurLevel])
		return false;

	if(2 <= wNextLevel-wCurLevel)
		return false;

	return true;
}

void cCookManager::SetLevel(CPlayer* pPlayer, WORD wLevel)
{
	if(wLevel<eCOOKLEVEL1 || eCOOKLEVEL4<wLevel)
		return;

	pPlayer->SetCookLevel(wLevel);
	pPlayer->SetCookCount(0);
	pPlayer->SetEatCount(0);
	pPlayer->SetFireCount(0);
}

bool cCookManager::IsFireNpc(DWORD dwNpcIndex)
{
	DWORD* pFireNpcIdx = m_FireNpcList.GetData(dwNpcIndex);
	if(pFireNpcIdx)
		return true;

	return false;
}

stRecipeInfo* cCookManager::GetRecipeInfo(DWORD dwRecipeIdx)
{
	int nLevel = GetCookLevelFromRecipe(dwRecipeIdx);

	std::map<DWORD, stRecipeInfo*>::iterator iter;
	
	switch(nLevel)
	{
	case eCOOKLEVEL1:
		iter = m_mapRecipeLv1.find(dwRecipeIdx);
		if(iter != m_mapRecipeLv1.end())
			return (*iter).second;
		break;
	case eCOOKLEVEL2:
		iter = m_mapRecipeLv2.find(dwRecipeIdx);
		if(iter != m_mapRecipeLv2.end())
			return (*iter).second;
		break;
	case eCOOKLEVEL3:
		iter = m_mapRecipeLv3.find(dwRecipeIdx);
		if(iter != m_mapRecipeLv3.end())
			return (*iter).second;
		break;
	case eCOOKLEVEL4:
		iter = m_mapRecipeLv4.find(dwRecipeIdx);
		if(iter != m_mapRecipeLv4.end())
			return (*iter).second;
		break;
	}

	return NULL;
}

WORD cCookManager::CheckCookUtil(CPlayer* pPlayer)
{
	WORD wHighGrade = 0;
	if(!pPlayer)
		return 0;

	const ITEMBASE*		leftItem		= ITEMMGR->GetItemInfoAbsIn( pPlayer, TP_WEAR_START + eWearedItem_Weapon );
	const ITEM_INFO*	leftItemInfo	= ITEMMGR->GetItemInfo( leftItem ? leftItem->wIconIdx : 0 );
	const ITEMBASE*		rightItem		= ITEMMGR->GetItemInfoAbsIn( pPlayer, TP_WEAR_START + eWearedItem_Shield );
	const ITEM_INFO*	rightItemInfo	= ITEMMGR->GetItemInfo( rightItem ? rightItem->wIconIdx : 0 );

	if(!leftItem || !leftItemInfo || !rightItem || !rightItemInfo)
		return 0;
	
	if((eWeaponType_CookUtil != leftItemInfo->WeaponType) || (eWeaponType_CookUtil != rightItemInfo->WeaponType))
		return 0;

	if(rightItemInfo->Part3DType != ePartType_Shield)
		return 0;

	leftItemInfo->Grade>rightItemInfo->Grade ? wHighGrade=leftItemInfo->Grade : wHighGrade=rightItemInfo->Grade;

	return wHighGrade;
}

bool cCookManager::CheckIngredients(CPlayer* pPlayer, stRecipeInfo* pRecipe, WORD wMakeNum)
{
	if(!pPlayer || !pRecipe || !wMakeNum)
		return false;


	CItemSlot* slot = pPlayer->GetSlot( eItemTable_Inventory );
	if(!slot)
		return false;
	// 인벤토리 정보를 보관해두었다가. 복구할 때 사용한다
	CBackupSlot backupSlot( *slot );

	CItemSlot* playerSlot = pPlayer->GetSlot( eItemTable_Inventory );
	if(!playerSlot)
		return false;

	// 빈슬롯 체크
	WORD emptyPosition[SLOT_MAX_INVENTORY_NUM] = {0,};
	WORD emptyCount = playerSlot->GetEmptyCell( emptyPosition, 1 );
	if(emptyCount < 1)
		return false;

	// 필요한 재료수량 계산
	DWORD dwRequierdItemNum[MAX_INGREDIENT_LIST] = {0,};
	int i;
	for(i=0; i<MAX_INGREDIENT_LIST; i++)
	{
		if(pRecipe->Ingredients[i].dwItemIdx)
			dwRequierdItemNum[i] = pRecipe->Ingredients[i].wQuantity * wMakeNum;
	}

	const POSTYPE	inventoryStartPosition	= POSTYPE( TP_INVENTORY_START );
	const POSTYPE	inventoryEndPosition	= POSTYPE( TP_INVENTORY_END + pPlayer->GetInventoryExpansionSize() );


	// 재료소모
	POSTYPE position = 0;
	for(position=inventoryStartPosition; position<inventoryEndPosition; position++)
	{
		const ITEMBASE* item = playerSlot->GetItemInfoAbs( position );
		if(!item ||	!item->dwDBIdx)
			continue;
		
		for(i=0; i<MAX_INGREDIENT_LIST; i++)
		{
			if(dwRequierdItemNum[i] && item->wIconIdx==pRecipe->Ingredients[i].dwItemIdx)
			{
				if(ITEMMGR->IsDupItem(item->wIconIdx))
				{
					const DWORD quantity = item->Durability;

					if( item->Durability > dwRequierdItemNum[i] )
					{
						if( EI_TRUE != playerSlot->UpdateItemAbs(
							pPlayer,
							item->Position,
							item->dwDBIdx,
							item->wIconIdx,
							item->Position,
							item->QuickPosition,
							item->Durability - dwRequierdItemNum[i] ) )
						{
							continue;
						}
					}
					else
					{
						if( EI_TRUE != playerSlot->DeleteItemAbs(
							pPlayer,
							item->Position,
							0 ) )
						{
							continue;
						}
					}

					dwRequierdItemNum[i] -= min( quantity, dwRequierdItemNum[i] );
				}
				else
				{
					if( EI_TRUE != playerSlot->DeleteItemAbs( pPlayer, item->Position, 0 ) )
					{
						continue;
					}

					dwRequierdItemNum[i]--;
				}
			}
		}
	}


	// 남은재료 있는지 확인
	bool bSuccess = true;
	for(i=0; i<MAX_INGREDIENT_LIST; i++)
	{
		if(0 < dwRequierdItemNum[i])
		{
			bSuccess = false;
			break;
		}
	}

	const ITEM_INFO* const recipeItemInfo = ITEMMGR->GetItemInfo(
		pRecipe->dwFoodItemIdx);

	if(0 == recipeItemInfo)
	{
		return false;
	}

	// 만들어진 음식 지급
	bool bStacked = false;
	for(position=inventoryStartPosition; position<inventoryEndPosition; position++)
	{
		const ITEMBASE* item = playerSlot->GetItemInfoAbs( position );
		
		if(item->wIconIdx != pRecipe->dwFoodItemIdx)
		{
			continue;
		}
		else if(item->Durability + wMakeNum >= recipeItemInfo->Stack)
		{
			continue;
		}
		else if(item->nSealed != recipeItemInfo->wSeal)
		{
			continue;
		}

		if( EI_TRUE != playerSlot->UpdateItemAbs(
				pPlayer,
				item->Position,
				item->dwDBIdx,
				item->wIconIdx,
				item->Position,
				item->QuickPosition,
				item->Durability + wMakeNum,
				UB_DURA,
				SS_CHKDBIDX ) )
		{
			bSuccess = false;
		}

		bStacked = true;
		break;
	}

	if(!bStacked)
	{
		ITEMBASE fooditem;
		ZeroMemory( &fooditem, sizeof( fooditem ) );
		fooditem.wIconIdx	= pRecipe->dwFoodItemIdx;
		fooditem.Position	= emptyPosition[0];
		fooditem.Durability = wMakeNum;
		fooditem.dwDBIdx = UINT_MAX;
		fooditem.nSealed = recipeItemInfo->wSeal;

		if( EI_TRUE != playerSlot->InsertItemAbs(
			pPlayer,
			emptyPosition[0],
			&fooditem,
			SS_PREINSERT ) )
		{
			bSuccess = false;
		}
	}

	// 실패
	if(!bSuccess)
	{
		backupSlot.Restore();
		return false;
	}

	MSG_COOK_ACK message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_ITEM;
	message.Protocol = MP_ITEM_COOK_ACK;
	message.dwResultIdx = pRecipe->dwFoodItemIdx;

	ICONBASE emptyIconBase = {0};
	ITEMMGR->UpdateResult(
		*pPlayer,
		*playerSlot,
		backupSlot,
		message.mUpdateResult,
		MP_ITEM_COOK_GETITEM,
		eLog_ItemCookUse,
		eLog_ItemCookSuccess,
		emptyIconBase);
	pPlayer->SendMsg(
		&message,
		message.GetSize());

	return true;
}
#include "cCookMgr.h"
#include "MHFile.h"
#include "ItemManager.h"
#include "CookDlg.h"
#include "GameIn.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "InventoryExDialog.h"
#include "QuickManager.h"
#include "item.h"

GLOBALTON(cCookMgr)
cCookMgr::cCookMgr(void)
{
	m_wCookCnt = m_wEatCnt = m_wFireCnt = 0;
	m_wLevel = eCOOKLEVEL1;
	memset(m_MasterRecipe, 0, sizeof(m_MasterRecipe));
}

cCookMgr::~cCookMgr(void)
{
	std::map<DWORD, stRecipeInfo*>::iterator iter;

	for(iter=m_mapRecipeLv1.begin(); iter!=m_mapRecipeLv1.end(); iter++)
	{
		SAFE_DELETE(iter->second);
	}
	m_mapRecipeLv1.clear();

	for(iter=m_mapRecipeLv2.begin(); iter!=m_mapRecipeLv2.end(); iter++)
	{
		SAFE_DELETE(iter->second);
	}
	m_mapRecipeLv2.clear();

	for(iter=m_mapRecipeLv3.begin(); iter!=m_mapRecipeLv3.end(); iter++)
	{
		SAFE_DELETE(iter->second);
	}
	m_mapRecipeLv3.clear();

	for(iter=m_mapRecipeLv4.begin(); iter!=m_mapRecipeLv4.end(); iter++)
	{
		SAFE_DELETE(iter->second);
	}
	m_mapRecipeLv4.clear();
}

void cCookMgr::Init()
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

	// 요리다이얼로그에 레시피목록 삽입.
	ITEM_INFO* pItemInfo = NULL;
	std::map<DWORD, stRecipeInfo*>::iterator iter;
	
	for(iter=m_mapRecipeLv1.begin(); iter!=m_mapRecipeLv1.end(); iter++)
	{
		pItemInfo = ITEMMGR->GetItemInfo(iter->second->dwFoodItemIdx);
		if(pItemInfo && iter->second->dwRemainTime==0)
			GAMEIN->GetCookDlg()->AddRecipe(eCOOKLEVEL1, iter->first, pItemInfo->ItemName);
	}

	for(iter=m_mapRecipeLv2.begin(); iter!=m_mapRecipeLv2.end(); iter++)
	{
		pItemInfo = ITEMMGR->GetItemInfo(iter->second->dwFoodItemIdx);
		if(pItemInfo && iter->second->dwRemainTime==0)
			GAMEIN->GetCookDlg()->AddRecipe(eCOOKLEVEL2, iter->first, pItemInfo->ItemName);
	}

	for(iter=m_mapRecipeLv3.begin(); iter!=m_mapRecipeLv3.end(); iter++)
	{
		pItemInfo = ITEMMGR->GetItemInfo(iter->second->dwFoodItemIdx);
		if(pItemInfo && iter->second->dwRemainTime==0)
			GAMEIN->GetCookDlg()->AddRecipe(eCOOKLEVEL3, iter->first, pItemInfo->ItemName);
	}

	for(iter=m_mapRecipeLv4.begin(); iter!=m_mapRecipeLv4.end(); iter++)
	{
		pItemInfo = ITEMMGR->GetItemInfo(iter->second->dwFoodItemIdx);
		if(pItemInfo && iter->second->dwRemainTime==0)
			GAMEIN->GetCookDlg()->AddRecipe(eCOOKLEVEL4, iter->first, pItemInfo->ItemName);
	}
}

void cCookMgr::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_COOK_ACK:
		{
			GAMEIN->GetCookDlg()->SetDlgLock(FALSE);

			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			WORD wLevel = WORD(GetCookLevelFromRecipe(pmsg->dwData1));
			stRecipeInfo* pRecipe = GetRecipeInfo(wLevel, pmsg->dwData1);
			if(pRecipe)
			{
				ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(pRecipe->dwFoodItemIdx);
				if(pItemInfo)
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1801), pItemInfo->ItemName, pmsg->dwData2);
				}
			}

			if(pmsg->dwObjectID == HEROID)
			{
				CObject*			object		= OBJECTMGR->GetObject( pmsg->dwObjectID );
				object->RemoveObjectEffect( COOKING_EFFECT );

				OBJECTSTATEMGR->StartObjectState(object, eObjectState_None);
				QUICKMGR->RefreshQickItem();
			}
		}
		break;

	case MP_COOK_STATE:
		{
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

			bool bChangeLevel = false;
			if(m_wLevel != pmsg->dwData1)
				bChangeLevel = true;

			if(HERO->GetState()!=eObjectState_Immortal && !bChangeLevel)
			{
				if(m_wCookCnt < pmsg->dwData2)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1969), pmsg->dwData2, pmsg->dwData2-m_wCookCnt);

				if(m_wEatCnt < pmsg->dwData3)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1970), pmsg->dwData3, pmsg->dwData3-m_wEatCnt);

				if(m_wFireCnt < pmsg->dwData4)
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1971), pmsg->dwData4, pmsg->dwData4-m_wFireCnt);
			}

			m_wLevel = (WORD)pmsg->dwData1;
			m_wCookCnt = (WORD)pmsg->dwData2;
			m_wEatCnt = (WORD)pmsg->dwData3;
			m_wFireCnt = (WORD)pmsg->dwData4;

			GAMEIN->GetCookDlg()->UpdateDlg();

			if(bChangeLevel)
				GAMEIN->GetInventoryDialog()->RefreshInvenItem();
		}
		break;

	case MP_COOK_NACK:
		{
			GAMEIN->GetCookDlg()->SetDlgLock(FALSE);

			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CObject* pObject = OBJECTMGR->GetObject(pmsg->dwObjectID);
			if(!pObject)		return;

			OBJECTSTATEMGR->EndObjectState(pObject, eObjectState_Fishing);
			OBJECTSTATEMGR->StartObjectState(pObject, eObjectState_None);

			if(pObject == HERO)	// 본인실패 처리
			{
				switch(pmsg->dwData)
				{
				case eCookError_InvaildRecipe:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1798) );
					break;
				case eCookError_InvaildFire:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1664) );
					break;
				case eCookError_InvaildMakeNum:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1799) );
					break;
				case eCookError_InvaildUtil:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1660) );
					break;
				case eCookError_LowerUtil:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1662) );
					break;
				case eCookError_LowerExpertPoint:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1661) );
					break;
				case eCookError_InvalidGrade:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1794) );
					break;
				case eCookError_InvaildState:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1802) );
					break;
				case eCookError_OverDistance:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1663) );
					break;
				case eCookError_InvenFull:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1672) );
					break;
				case eCookError_NeedIngredient:
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1670) );
					break;
				}

				QUICKMGR->RefreshQickItem();
			}
		}
		break;

	case MP_COOK_LEVELUP_ACK:
		{
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
			m_wLevel = (WORD)pmsg->dwData1;
			m_wCookCnt = (WORD)pmsg->dwData2;
			m_wEatCnt = (WORD)pmsg->dwData3;
			m_wFireCnt = (WORD)pmsg->dwData4;

			GAMEIN->GetCookDlg()->UpdateDlg();
			
			EFFECTMGR->StartEffectProcess(eEffect_LevelUpSentence, HERO, NULL, 0, 0);
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1800));
		}
		break;

	case MP_COOK_UPDATERECIPE:
		{
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
			DWORD dwSlot = pmsg->dwData3;
			if(dwSlot<0 && MAX_RECIPE_LV4_LIST<=dwSlot)
				return;

			int nRecipeLevel = GetCookLevelFromRecipe(pmsg->dwData2);
			stRecipeInfo* pRecipeInfo = GetRecipeInfo(
				WORD(nRecipeLevel),
				pmsg->dwData2);
			ITEM_INFO* pItemInfo = NULL;	// FoodItem
			if(pRecipeInfo)
				pItemInfo = ITEMMGR->GetItemInfo(pRecipeInfo->dwFoodItemIdx);

			switch(pmsg->dwData1)
			{
			case eCOOKRECIPE_ADD:
				{
					m_MasterRecipe[dwSlot].dwRecipeIdx = pmsg->dwData2;
					m_MasterRecipe[dwSlot].dwRemainTime = pmsg->dwData4;
					
					if(pItemInfo)
					{
						char buf[256] = {0,};
						char temp[256] = {0,};
						DWORD dwMin = pmsg->dwData4/60000;
						if(dwMin)
						{
							sprintf(buf, "%s ", pItemInfo->ItemName);
							sprintf(temp, CHATMGR->GetChatMsg(1804), dwMin);
							strcat(buf, temp);
						}
						else
							sprintf(buf, "%s %s", pItemInfo->ItemName, CHATMGR->GetChatMsg(1805));

						GAMEIN->GetCookDlg()->AddRecipe(eCOOKLEVEL4, pmsg->dwData2, buf);
					}
				}
				break;
				
			case eCOOKRECIPE_DEL:
				{
					if(pItemInfo)
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1806), pItemInfo->ItemName);

					m_MasterRecipe[dwSlot].dwRecipeIdx = 0;
					m_MasterRecipe[dwSlot].dwRemainTime = 0;

					const int nPos = GAMEIN->GetCookDlg()->GetRecipePos(
						WORD(nRecipeLevel),
						pmsg->dwData2);

					if(0<=nPos && nPos<MAX_RECIPE_LV4_LIST)
					{
						GAMEIN->GetCookDlg()->DelRecipe(
							WORD(nRecipeLevel),
							nPos,
							TRUE);
					}
				}
				break;

			case eCOOKRECIPE_UPDATE:
				{
					m_MasterRecipe[dwSlot].dwRecipeIdx = pmsg->dwData2;
					m_MasterRecipe[dwSlot].dwRemainTime = pmsg->dwData4;

					if(pItemInfo)
					{
						char buf[256] = {0,};
						char temp[256] = {0,};
						DWORD dwMin = pmsg->dwData4/60000;
						if(dwMin)
						{
							sprintf(buf, "%s ", pItemInfo->ItemName);
							sprintf(temp, CHATMGR->GetChatMsg(1804), dwMin);
							strcat(buf, temp);
						}
						else
							sprintf(buf, "%s %s", pItemInfo->ItemName, CHATMGR->GetChatMsg(1805));

						int nPos = GAMEIN->GetCookDlg()->GetRecipePos(
							WORD(nRecipeLevel),
							pmsg->dwData2);

						if(0<=nPos && nPos<MAX_RECIPE_LV4_LIST)
						{
							GAMEIN->GetCookDlg()->DelRecipe(
								WORD(nRecipeLevel),
								nPos);
							GAMEIN->GetCookDlg()->AddRecipe(
								WORD(nRecipeLevel),
								pmsg->dwData2,
								buf);
						}
					}
				}
				break;
			}

			GAMEIN->GetCookDlg()->UpdateDlg();
		}
		break;
	}
}

stRecipeInfo* cCookMgr::GetRecipeInfo(WORD wLevel, DWORD dwIndex)
{
	std::map<DWORD, stRecipeInfo*>::iterator iter;

	switch(wLevel)
	{
	case eCOOKLEVEL1:
		iter = m_mapRecipeLv1.find(dwIndex);
		if(iter != m_mapRecipeLv1.end())
			return iter->second;
		break;

	case eCOOKLEVEL2:
		iter = m_mapRecipeLv2.find(dwIndex);
		if(iter != m_mapRecipeLv2.end())
			return iter->second;
		break;

	case eCOOKLEVEL3:
		iter = m_mapRecipeLv3.find(dwIndex);
		if(iter != m_mapRecipeLv3.end())
			return iter->second;
		break;

	case eCOOKLEVEL4:
		iter = m_mapRecipeLv4.find(dwIndex);
		if(iter != m_mapRecipeLv4.end())
			return iter->second;
		break;
	}

	return NULL;
}
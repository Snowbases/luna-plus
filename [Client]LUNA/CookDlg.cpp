#include "stdafx.h"
#include "Cookdlg.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"
#include "interface/cListDialog.h"
#include "interface/cStatic.h"
#include "cResourceManager.h"
#include "cCookMgr.h"
#include "MoveManager.h"
#include "GameIn.h"
#include "ItemManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "ProgressDialog.h"
#include "../[cc]skill/client/info/SkillInfo.h"
#include "../[cc]skill/client/manager/Skillmanager.h"
#include "InventoryExDialog.h"
#include "../[cc]skill/client/info/BuffSkillInfo.h"

CCookDlg::CCookDlg(void)
{
}

CCookDlg::~CCookDlg(void)
{
	int i;
	for(i=0; i<MAX_COOKING_LEVEL;i++)
		m_lstRecipeIndex[i].clear();
}

void CCookDlg::Add( cWindow* window )
{
	WORD wWindowType = window->GetType() ;																		// 윈도우 타입을 받아온다.

	if( wWindowType == WT_PUSHUPBUTTON )																		// 윈도우 타입이 푸쉬업 버튼이면,
	{
		AddTabBtn( curIdx1++, (cPushupButton*)window ) ;														// 현재 인덱스1로 탭 버튼을 추가한다.
	}
	else if( wWindowType == WT_DIALOG )																			// 윈도우 타입이 다이얼로그이면,
	{
		AddTabSheet( curIdx2++, window ) ;																		// 현재 인덱스2로 쉬트를 추가한다.
	}
	else																										// 이도 저도 아니면,
	{
		cTabDialog::Add( window ) ;																				// 윈도우를 추가한다.
	}
}

void CCookDlg::Linking()
{
	m_pTitle	= (cStatic*)((cTabDialog*)GetTabSheet(0))->GetWindowForID( COOK_TITLE );
	m_pLevel	= (cStatic*)((cTabDialog*)GetTabSheet(0))->GetWindowForID( COOK_LEVEL );
	m_pCookCount= (cStatic*)((cTabDialog*)GetTabSheet(0))->GetWindowForID( COOK_COOKCOUNT );
	m_pEatCount	= (cStatic*)((cTabDialog*)GetTabSheet(0))->GetWindowForID( COOK_EATCOUNT );
	m_pFireCount= (cStatic*)((cTabDialog*)GetTabSheet(0))->GetWindowForID( COOK_FIRECOUNT );

	m_pBtnStart = (cButton*)GetWindowForID( COOK_START );
	m_pBtnStart->SetDisable(TRUE);
	m_pBtnClose = (cButton*)GetWindowForID( COOK_CLOSE );
	m_pMakeNum = (cEditBox*)GetWindowForID( COOK_MAKENUM );

	m_pTabBtn[0] = (cPushupButton*)GetWindowForID( COOK_TAB_1 );
	m_pTabBtn[1] = (cPushupButton*)GetWindowForID( COOK_TAB_2 );
	m_pTabBtn[2] = (cPushupButton*)GetWindowForID( COOK_TAB_3 );
	m_pTabBtn[3] = (cPushupButton*)GetWindowForID( COOK_TAB_4 );

	m_pRecipeList[0] = (cListDialog*)((cTabDialog*)GetTabSheet(1))->GetWindowForID( COOK_RECIPELIST_1 );
	m_pRecipeList[0]->SetShowSelect( TRUE );
	m_pRecipeList[1] = (cListDialog*)((cTabDialog*)GetTabSheet(2))->GetWindowForID( COOK_RECIPELIST_2 );
	m_pRecipeList[1]->SetShowSelect( TRUE );
	m_pRecipeList[2] = (cListDialog*)((cTabDialog*)GetTabSheet(3))->GetWindowForID( COOK_RECIPELIST_3 );
	m_pRecipeList[2]->SetShowSelect( TRUE );
	m_pRecipeList[3] = (cListDialog*)((cTabDialog*)GetTabSheet(4))->GetWindowForID( COOK_RECIPELIST_4 );
	m_pRecipeList[3]->SetShowSelect( TRUE );

	m_pIngredientList[0] = (cListDialog*)((cTabDialog*)GetTabSheet(1))->GetWindowForID( COOK_INGREDIENTLIST_1 );
	m_pIngredientList[1] = (cListDialog*)((cTabDialog*)GetTabSheet(2))->GetWindowForID( COOK_INGREDIENTLIST_2 );
	m_pIngredientList[2] = (cListDialog*)((cTabDialog*)GetTabSheet(3))->GetWindowForID( COOK_INGREDIENTLIST_3 );
	m_pIngredientList[3] = (cListDialog*)((cTabDialog*)GetTabSheet(4))->GetWindowForID( COOK_INGREDIENTLIST_4 );
}

void CCookDlg::SetActive(BOOL val )
{
	if( m_bDisable ) return;

	if(val == TRUE)
	{
		m_pSelectedRecipe = NULL;
		m_dwSelectedRecipe = 0;
		m_dwSelectedFire = 0;
		m_wMakeNum = 0;

		int i;
		for(i=0; i<eCOOKLEVEL4; i++)
		{
			m_pRecipeList[i]->SetCurSelectedRowIdx(-1);
			m_pIngredientList[i]->RemoveAll();
			m_pIngredientList[i]->SetCurSelectedRowIdx(-1);
		}

		m_pMakeNum->SetEditText("");
		UpdateDlg();
	}
	else
	{
	}

	cDialog::SetActive(val);
}

void CCookDlg::OnActionEvent(LONG lId, void * p, DWORD we)
{
	if( !p ) return ;

	if(we == WE_BTNCLICK)
	{
		switch(lId)
		{
		case COOK_START:
			{
				m_dwSelectedFire = 0;
				m_wMakeNum = 0;

				// 장비확인
				WORD wCookUtilLevel = CheckCookUtil();
				if(!wCookUtilLevel)
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1660) );
					return;
				}
				
				// 레시피확인
				if(!m_dwSelectedRecipe || !m_pSelectedRecipe)
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1798) );
					return;
				}
				else if(wCookUtilLevel<eCOOKLEVEL3 && wCookUtilLevel<COOKMGR->GetCookLevelFromRecipe(m_dwSelectedRecipe))
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1662) );
					return;
				}

				const WORD wCookLevel = WORD(COOKMGR->GetCookLevelFromRecipe(m_dwSelectedRecipe));
				stRecipeInfo* pRecipeInfo = COOKMGR->GetRecipeInfo(wCookLevel, m_dwSelectedRecipe);
				if(!pRecipeInfo)
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1798) );
					return;
				}

				if(COOKMGR->GetCookLevel()<=wCookLevel && COOKMGR->GetCookCount()<pRecipeInfo->dwExpertPointMin)
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1661) );
					return;
				}

				m_wMakeNum = WORD(atoi(m_pMakeNum->GetEditText()));

				if(m_wMakeNum<1 || 99<m_wMakeNum)
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1799) );
					return;
				}

				// 모닥불확인
				CObject* pObject = OBJECTMGR->GetSelectedObject();
				if(!pObject || eObjectKind_Npc!=pObject->GetObjectKind())
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1664) );
					return;
				}
				else
				{
					CNpc* pNpc = (CNpc*)pObject;
					if(pNpc->GetNpcJob()!=CAMPFIRE_ROLE || pNpc->IsDied())
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1664) );
						return;
					}

					m_dwSelectedFire = pNpc->GetID();
				}

				// 거리제한
				VECTOR3 vHeroPos;
				HERO->GetPosition(&vHeroPos) ;
				float fDistance = CalcDistanceXZ(&vHeroPos, &pObject->GetBaseMoveInfo()->CurPosition) ;
				if(MAX_COOKING_DISTANCE < fDistance) 
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1663)) ;
					return;
				}

				// 재료확인
				if(! CheckIngredients())
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1670) );
					return;
				}


				// 방향전환
				VECTOR3 pos;
				pObject->GetPosition(&pos);
				MOVEMGR->SetLookatPos(HERO,&pos,0,gCurTime);


				CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
				ASSERT( dialog );

				SetDlgLock(TRUE);
				dialog->Wait( CProgressDialog::eActionExCooking );
			}
			break;

		case COOK_CLOSE:
			{
				m_pSelectedRecipe = NULL;
				m_dwSelectedRecipe = 0;
				m_dwSelectedFire = 0;
				m_wMakeNum = 0;

				int i;
				for(i=0; i<MAX_COOKING_LEVEL; i++)
				{
					m_pRecipeList[i]->SetCurSelectedRowIdx(-1);
					m_pIngredientList[i]->RemoveAll();
					m_pIngredientList[i]->SetCurSelectedRowIdx(-1);
				}
				SetActive(FALSE);
			}
			break;
		}
	}
}

DWORD CCookDlg::ActionEvent(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;

	we = cTabDialog::ActionEvent(mouseInfo);

	if(we & WE_LBTNCLICK)
	{
		BYTE CurTab = GetCurTabNum();

		if(0 == CurTab)
		{
			int a;
			a = 10;
		}
		else
		{
			if(m_pRecipeList[CurTab-1]->IsDisable())
				return we;

			int nCurSelIdx = m_pRecipeList[CurTab-1]->GetCurSelectedRowIdx();

			if(-1 < nCurSelIdx)
			{
				int i;
				DWORD dwRecipeIdx = 0;
				m_pBtnStart->SetDisable(TRUE);
				std::list<DWORD>::iterator iter;
				for(iter=m_lstRecipeIndex[CurTab-1].begin(), i=0; iter!=m_lstRecipeIndex[CurTab-1].end(); iter++, i++)
				{
					if(i == nCurSelIdx)
					{
						dwRecipeIdx = (*iter);
						break;
					}
				}

				if(0!=dwRecipeIdx && dwRecipeIdx!=m_dwSelectedRecipe)
				{
					m_pSelectedRecipe = NULL;
					m_pIngredientList[CurTab-1]->RemoveAll();

					m_pSelectedRecipe = COOKMGR->GetRecipeInfo(CurTab, dwRecipeIdx);
					if(m_pSelectedRecipe)
					{
						char buf[512] = {0,};

						// 효과
						ITEM_INFO* pFoodItem = ITEMMGR->GetItemInfo(m_pSelectedRecipe->dwFoodItemIdx);
						if(pFoodItem)
						{
							cSkillInfo* pSkill = SKILLMGR->GetSkillInfo(pFoodItem->SupplyValue);
							if(pSkill)
							{
								const std::string& strToolTip = ITEMMGR->GetTotalToolTipMsg( pSkill->GetTooltip() );
								if( !strToolTip.empty() )
								{
									sprintf( buf, "%s %s", RESRCMGR->GetMsg(1072), strToolTip.c_str() );
									m_pIngredientList[CurTab-1]->AddItem(buf, RGB(0,255,0));
								}

								// 요리숙련도
								BOOL bCookable = TRUE;
								if(COOKMGR->GetCookLevel() < CurTab)
									bCookable = FALSE;
								else if(COOKMGR->GetCookLevel() == CurTab &&
									COOKMGR->GetCookCount() < m_pSelectedRecipe->dwExpertPointMin)
									bCookable = FALSE;

								char szMinExpert[128] = {0,};
								char szMaxExpert[128] = {0,};
								SafeStrCpy(szMinExpert, RESRCMGR->GetMsg(1164), 128-1);
								SafeStrCpy(szMaxExpert, RESRCMGR->GetMsg(1165), 128-1);
								sprintf(buf, "%s(%d)-%s(%d)", szMinExpert, m_pSelectedRecipe->dwExpertPointMin, szMaxExpert, m_pSelectedRecipe->dwExpertPointMax);
								if(bCookable)
									m_pIngredientList[CurTab-1]->AddItem(buf, RGB(0,255,255));
								else
									m_pIngredientList[CurTab-1]->AddItem(buf, RGB(0,0,255));

								// 재료
								for(i=0; i<MAX_INGREDIENT_LIST; i++)
								{
									ITEM_INFO* pIngredientItem = ITEMMGR->GetItemInfo(m_pSelectedRecipe->Ingredients[i].dwItemIdx);
									if(pIngredientItem)
									{
										sprintf(buf, "%s x%d", pIngredientItem->ItemName, m_pSelectedRecipe->Ingredients[i].wQuantity);
										m_pIngredientList[CurTab-1]->AddItem(buf, RGB(255,255,0));
									}
								}
								m_pIngredientList[CurTab-1]->SetCurSelectedRowIdx(-1);

								m_dwSelectedRecipe = dwRecipeIdx;
								m_pBtnStart->SetDisable(FALSE);
							}
						}
					}
				}
			}
		}
	}

	return we;
}

void CCookDlg::UpdateDlg()
{
	int i;
	char buf[64] = {0,};

	for(i=0; i<MAX_COOKING_LEVEL; i++)
		m_pTabBtn[i]->SetDisable(TRUE);

	switch(COOKMGR->GetCookLevel())
	{
		case eCOOKLEVEL1:
			sprintf(buf, "%s", RESRCMGR->GetMsg(1032));		m_pTitle->SetStaticText(buf);
			sprintf(buf, "%s", RESRCMGR->GetMsg(1020));		m_pLevel->SetStaticText(buf);
			break;
		case eCOOKLEVEL2:
			sprintf(buf, "%s", RESRCMGR->GetMsg(1033));		m_pTitle->SetStaticText(buf);
			sprintf(buf, "%s", RESRCMGR->GetMsg(1021));		m_pLevel->SetStaticText(buf);
			break;
		case eCOOKLEVEL3:
			sprintf(buf, "%s", RESRCMGR->GetMsg(1034));		m_pTitle->SetStaticText(buf);
			sprintf(buf, "%s", RESRCMGR->GetMsg(1022));		m_pLevel->SetStaticText(buf);
			break;
		case eCOOKLEVEL4:
			sprintf(buf, "%s", RESRCMGR->GetMsg(1073));		m_pTitle->SetStaticText(buf);
			sprintf(buf, "%s", RESRCMGR->GetMsg(1023));		m_pLevel->SetStaticText(buf);
			break;
	}

	for(i=0; i<COOKMGR->GetCookLevel(); i++)
		m_pTabBtn[i]->SetDisable(FALSE);

	DWORD FullColor = 0xffffffff;
	DWORD HighrigtColor = 0xffffff00;
	WORD wCurCount, wMaxCount;

	wCurCount = COOKMGR->GetCookCount();
	wMaxCount = COOKMGR->GetMaxCookCount();
	wCurCount==wMaxCount ? m_pCookCount->SetFGColor(HighrigtColor) : m_pCookCount->SetFGColor(FullColor);
	sprintf(buf, "%d/%d", wCurCount, wMaxCount);
	m_pCookCount->SetStaticText(buf);

	wCurCount = COOKMGR->GetEatCount();
	wMaxCount = COOKMGR->GetMaxEatCount();
	wCurCount==wMaxCount ? m_pEatCount->SetFGColor(HighrigtColor) : m_pEatCount->SetFGColor(FullColor);
	sprintf(buf, "%d/%d", wCurCount, wMaxCount);
	m_pEatCount->SetStaticText(buf);

	wCurCount = COOKMGR->GetFireCount();
	wMaxCount = COOKMGR->GetMaxFireCount();
	wCurCount==wMaxCount ? m_pFireCount->SetFGColor(HighrigtColor) : m_pFireCount->SetFGColor(FullColor);
	sprintf(buf, "%d/%d", wCurCount, wMaxCount);
	m_pFireCount->SetStaticText(buf);
}

void CCookDlg::ClearRecipe(WORD wLevel)
{
	if(wLevel<eCOOKLEVEL1 || eCOOKLEVEL4<wLevel)
		return;

	m_lstRecipeIndex[wLevel-1].clear();
	m_pRecipeList[wLevel-1]->RemoveAll();
	m_pIngredientList[wLevel-1]->RemoveAll();
}

void CCookDlg::AddRecipe(WORD wLevel, DWORD dwIndex, char* pRecipeName)
{
	if(wLevel<eCOOKLEVEL1 || eCOOKLEVEL4<wLevel)
		return;

	m_pRecipeList[wLevel-1]->AddItem(pRecipeName, RGB( 255, 255, 255 ));
	m_lstRecipeIndex[wLevel-1].push_back(dwIndex);
}

void CCookDlg::DelRecipe(WORD wLevel, DWORD dwPos, bool bDeselect)
{
	if(wLevel<eCOOKLEVEL1 || eCOOKLEVEL4<wLevel)
		return;

	if(bDeselect &&
		int(dwPos) == m_pRecipeList[wLevel-1]->GetClickedRowIdx())
	{
		m_pRecipeList[wLevel-1]->SetCurSelectedRowIdx(-1);
		m_pIngredientList[wLevel-1]->RemoveAll();
		m_dwSelectedRecipe = 0;
	}

	m_pRecipeList[wLevel-1]->RemoveItem(dwPos);
	
	std::list<DWORD>::iterator iter = m_lstRecipeIndex[wLevel-1].begin();

	for(DWORD nPos = 0; nPos<MAX_RECIPE_LV4_LIST; nPos++, iter++)
	{
		if(nPos == dwPos)
		{
			m_lstRecipeIndex[wLevel-1].erase(iter);
			break;
		}
	}
}

int CCookDlg::GetRecipePos(WORD wLevel, DWORD dwRecipeIdx)
{
	if(!dwRecipeIdx || wLevel<eCOOKLEVEL1 || eCOOKLEVEL4<wLevel)
		return -1;

	int nPos;
	std::list<DWORD>::iterator iter;
	for(iter=m_lstRecipeIndex[wLevel-1].begin(), nPos=0; iter!=m_lstRecipeIndex[wLevel-1].end(); iter++, nPos++)
	{
		if((*iter) == dwRecipeIdx)
			return nPos;
	}

	return nPos;
}

void CCookDlg::Send()
{
	MSG_COOK_SYN msg;
	msg.Category = MP_COOK;
	msg.Protocol = MP_COOK_SYN;
	msg.dwObjectID = HEROID;
	msg.dwRecipeIdx = m_dwSelectedRecipe;
	msg.wMakeNum = m_wMakeNum;
	msg.dwFireNpcIdx = m_dwSelectedFire;

	NETWORK->Send(&msg, sizeof(msg));
}

WORD CCookDlg::CheckCookUtil()
{
	WORD wLowGrade = 0;
	const ITEMBASE*		leftItem		= ITEMMGR->GetItemInfoAbsIn( HERO, TP_WEAR_START + eWearedItem_Weapon );
	const ITEM_INFO*	leftItemInfo	= ITEMMGR->GetItemInfo( leftItem ? leftItem->wIconIdx : 0 );
	const ITEMBASE*		rightItem		= ITEMMGR->GetItemInfoAbsIn( HERO, TP_WEAR_START + eWearedItem_Shield );
	const ITEM_INFO*	rightItemInfo	= ITEMMGR->GetItemInfo( rightItem ? rightItem->wIconIdx : 0 );

	if(!leftItem || !leftItemInfo || !rightItem || !rightItemInfo)
		return 0;
	
	if((eWeaponType_CookUtil != leftItemInfo->WeaponType) || (eWeaponType_CookUtil != rightItemInfo->WeaponType))
		return 0;

	if(rightItemInfo->Part3DType != ePartType_Shield)
		return 0;

	leftItemInfo->Grade<rightItemInfo->Grade ? wLowGrade=leftItemInfo->Grade : wLowGrade=rightItemInfo->Grade;

	return wLowGrade;
}

bool CCookDlg::CheckIngredients()
{
	if(!m_dwSelectedRecipe || !m_pSelectedRecipe || !m_wMakeNum)
		return false;

	CInventoryExDialog* inventory = ( CInventoryExDialog* )WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
	ASSERT( inventory );

	DWORD dwRequierdItemNum[MAX_INGREDIENT_LIST] = {0,};
	int i;
	for(i=0; i<MAX_INGREDIENT_LIST; i++)
	{
		if(m_pSelectedRecipe->Ingredients[i].dwItemIdx)
			dwRequierdItemNum[i] = m_pSelectedRecipe->Ingredients[i].wQuantity * m_wMakeNum;
	}

	const POSTYPE inventoryStartPosition = TP_INVENTORY_START;
	const POSTYPE inventoryEndPosition = POSTYPE(TP_INVENTORY_END + TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount());
	POSTYPE position = 0;
	for(position=inventoryStartPosition; position<inventoryEndPosition; position++)
	{
		CItem* item = inventory->GetItemForPos( position );
		if(!item)				continue;
		if(item->IsLocked())	continue;

		const DWORD	itemIndex	= item->GetItemIdx();
		
		for(i=0; i<MAX_INGREDIENT_LIST; i++)
		{
			if(itemIndex == m_pSelectedRecipe->Ingredients[i].dwItemIdx)
			{
				const BOOL	isDuplicate = ITEMMGR->IsDupItem( itemIndex );
				const DWORD quantity	= ( isDuplicate ? item->GetDurability() : 1 );
				dwRequierdItemNum[i] -= min( quantity, dwRequierdItemNum[i] );

				break;
			}
		}
	}

	for(i=0; i<MAX_INGREDIENT_LIST; i++)
	{
		if(0 < dwRequierdItemNum[i])
			return false;
	}

	return true;
}

void CCookDlg::SetDlgLock(BOOL bVal)
{
	for(int i=0; i<MAX_COOKING_LEVEL; ++i)
		m_pRecipeList[i]->SetDisable(bVal);
}
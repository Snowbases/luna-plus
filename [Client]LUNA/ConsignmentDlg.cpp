#include "stdafx.h"
#include "Consignmentdlg.h"
#include "WindowIDEnum.h"
#include "cIMEex.h"
#include "MHFile.h"

#include "ObjectManager.h"

#include "Item.h"
#include ".\interface\cIconDialog.h"
#include "ItemManager.h"
#include "Interface/cScriptManager.h"
#include "ChatManager.h"

#include ".\interface\cStatic.h"
#include ".\interface\cPushupButton.h"
#include ".\interface\cEditBox.h"
#include ".\interface\cComboBox.h"
#include ".\interface\cListDialog.h"
#include "cRitemEx.h"

#include "ConsignmentMgr.h"
#include "cMsgBox.h"
#include "cWindowManager.h"
#include "ExchangeItem.h"
#include "cDivideBox.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "./Input/Keyboard.h"
#include "cResourceManager.h"

CConsignmentDlg::CConsignmentDlg(void)
{
	m_type = WT_CONSIGNMENTDLG;	
	m_RegistIcon = NULL;

	for(int i=0; i<DISPLAYNUM_PER_PAGE; i++)
	{
		m_RegistedIcon[i] = NULL;
		m_SearchIcon[i] = NULL;
	}
	m_bHold = FALSE;


	m_htCategory1Info.Initialize(20);
	m_htCategory2Info.Initialize(100);
}

CConsignmentDlg::~CConsignmentDlg(void)
{
	m_RegistControlListArray.RemoveAll();
	m_BuyControlListArray.RemoveAll();
	SAFE_DELETE( m_RegistIcon );
	for(int i=0; i<DISPLAYNUM_PER_PAGE; i++)
	{
		SAFE_DELETE( m_RegistedIcon[i] );
		SAFE_DELETE( m_SearchIcon[i] );
	}

	stCategoryInfo1* pInfo1;
	m_htCategory1Info.SetPositionHead();
	while(NULL != (pInfo1 = m_htCategory1Info.GetData()))
	{
		SAFE_DELETE(pInfo1);
	}
	m_htCategory1Info.RemoveAll();

	stCategoryInfo2* pInfo2;
	m_htCategory2Info.SetPositionHead();
	while(NULL != (pInfo2 = m_htCategory2Info.GetData()))
	{
		SAFE_DELETE(pInfo2);
	}
	m_htCategory2Info.RemoveAll();
}

void CConsignmentDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	m_type = WT_CONSIGNMENTDLG;	
	m_nMode = eConsignment_Mode_Regist;

	LoadCategoryInfo();
	m_dwLastSearchTime = 0;
}

void CConsignmentDlg::Linking()
{
	m_PageBuyBtn	= (cPushupButton*)GetWindowForID(CONSIGNMENT_PUSHBTN_BUY);
	m_PageRegistBtn	= (cPushupButton*)GetWindowForID(CONSIGNMENT_PUSHBTN_REGIST);
	m_pClose = (cButton*)GetWindowForID(CONSIGNMENT_BTN_CLOSE);
	m_pHelp = (cButton*)GetWindowForID(CONSIGNMENT_BTN_HELP);
	m_pHaveMoney = (cStatic*)GetWindowForID(CONSIGNMENT_HAVEMONEY);

	// 등록Page
	m_pReg_ItemName		= (cStatic*)GetWindowForID(CONSIGNMENT_REG_ITEMNAME);
	m_pReg_ItemLevel	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_ITEMLEVEL);
	m_pReg_RemainTime	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_REMAINTIME);
	m_pReg_Price		= (cStatic*)GetWindowForID(CONSIGNMENT_REG_PRICE);
	m_pReg_Cancel		= (cStatic*)GetWindowForID(CONSIGNMENT_REG_CANCEL);
	m_pReg_FilterTop	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_FILTER_BG_TOP);
	m_pReg_FilterMiddle	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_FILTER_BG_MIDDLE);
	m_pReg_FilterBottom	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_FILTER_BG_BOTTOM);
	m_pReg_TotalPriceBG	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_TOTALPRICE_BG);
	m_pReg_TotalPrice	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_TOTALPRICE);
	m_pReg_DepositBG	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_DEPOSIT_BG);
	m_pReg_Deposit		= (cStatic*)GetWindowForID(CONSIGNMENT_REG_DEPOSIT);
	m_pReg_CommissionBG	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_COMMISSION_BG);
	m_pReg_Commission	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_COMMISSION);
	m_pReg_Icon			= (cStatic*)GetWindowForID(CONSIGNMENT_REG_ICON);
	m_pReg_PriceLine	= (cStatic*)GetWindowForID(CONSIGNMENT_REG_PRICELINE);
	m_pReg_Dollar1		= (cStatic*)GetWindowForID(CONSIGNMENT_REG_DOLLAR1);
	m_pReg_Text1		= (cStatic*)GetWindowForID(CONSIGNMENT_REG_TEXT1);
	m_pReg_Text2		= (cStatic*)GetWindowForID(CONSIGNMENT_REG_TEXT2);
	m_pReg_Text3		= (cStatic*)GetWindowForID(CONSIGNMENT_REG_TEXT3);
	m_pReg_Text4		= (cStatic*)GetWindowForID(CONSIGNMENT_REG_TEXT4);

	m_pReg_Cancel1Btn	= (cButton*)GetWindowForID(CONSIGNMENT_REG_CANCEL1_BTN);
	m_pReg_Cancel2Btn	= (cButton*)GetWindowForID(CONSIGNMENT_REG_CANCEL2_BTN);
	m_pReg_Cancel3Btn	= (cButton*)GetWindowForID(CONSIGNMENT_REG_CANCEL3_BTN);
	m_pReg_Cancel4Btn	= (cButton*)GetWindowForID(CONSIGNMENT_REG_CANCEL4_BTN);
	m_pReg_Cancel5Btn	= (cButton*)GetWindowForID(CONSIGNMENT_REG_CANCEL5_BTN);
	m_pReg_RegistBtn	= (cButton*)GetWindowForID(CONSIGNMENT_REG_REGIST_BTN);
	m_pReg_UnitPriceEdit = (cEditBox*)GetWindowForID(CONSIGNMENT_REG_UNITPRICE);

	m_pReg_ItemDlg			= (cIconDialog*)GetWindowForID(CONSIGNMENT_REG_ICONDLG);
	for(int i=0; i<5; i++)
		m_pReg_RegistedItemDlg[i]	= (cIconDialog*)GetWindowForID(CONSIGNMENT_REG_REGISTED_ICON1 + i);

	m_pReg_ItemList			= (cListDialog*)GetWindowForID(CONSIGNMENT_REG_ITEMLIST);
	m_pReg_ItemList_Lv		= (cListDialog*)GetWindowForID(CONSIGNMENT_REG_ITEMLIST_LV);
	m_pReg_ItemList_Time	= (cListDialog*)GetWindowForID(CONSIGNMENT_REG_ITEMLIST_TIME);
	m_pReg_ItemList_Price	= (cListDialog*)GetWindowForID(CONSIGNMENT_REG_ITEMLIST_PRICE);



	m_RegistControlListArray.AddTail(m_pReg_ItemName);
	m_RegistControlListArray.AddTail(m_pReg_ItemLevel);
	m_RegistControlListArray.AddTail(m_pReg_RemainTime);
	m_RegistControlListArray.AddTail(m_pReg_Price);
	m_RegistControlListArray.AddTail(m_pReg_Cancel);
	m_RegistControlListArray.AddTail(m_pReg_FilterTop);
	m_RegistControlListArray.AddTail(m_pReg_FilterMiddle);
	m_RegistControlListArray.AddTail(m_pReg_FilterBottom);
	m_RegistControlListArray.AddTail(m_pReg_TotalPriceBG);
	m_RegistControlListArray.AddTail(m_pReg_TotalPrice);
	m_RegistControlListArray.AddTail(m_pReg_DepositBG);
	m_RegistControlListArray.AddTail(m_pReg_Deposit);
	m_RegistControlListArray.AddTail(m_pReg_CommissionBG);
	m_RegistControlListArray.AddTail(m_pReg_Commission);
	m_RegistControlListArray.AddTail(m_pReg_Icon);
	m_RegistControlListArray.AddTail(m_pReg_PriceLine);
	m_RegistControlListArray.AddTail(m_pReg_Dollar1);
	m_RegistControlListArray.AddTail(m_pReg_Text1);
	m_RegistControlListArray.AddTail(m_pReg_Text2);
	m_RegistControlListArray.AddTail(m_pReg_Text3);
	m_RegistControlListArray.AddTail(m_pReg_Text4);
	m_RegistControlListArray.AddTail(m_pReg_Cancel1Btn);
	m_RegistControlListArray.AddTail(m_pReg_Cancel2Btn);
	m_RegistControlListArray.AddTail(m_pReg_Cancel3Btn);
	m_RegistControlListArray.AddTail(m_pReg_Cancel4Btn);
	m_RegistControlListArray.AddTail(m_pReg_Cancel5Btn);
	m_RegistControlListArray.AddTail(m_pReg_RegistBtn);
	m_RegistControlListArray.AddTail(m_pReg_UnitPriceEdit);
	m_RegistControlListArray.AddTail(m_pReg_ItemDlg);
	for(int i=0; i<5; i++)
		m_RegistControlListArray.AddTail(m_pReg_RegistedItemDlg[i]);

	m_RegistControlListArray.AddTail(m_pReg_ItemList);
	m_RegistControlListArray.AddTail(m_pReg_ItemList_Lv);
	m_RegistControlListArray.AddTail(m_pReg_ItemList_Time);
	m_RegistControlListArray.AddTail(m_pReg_ItemList_Price);

	// 구입Page
	m_pBuy_SearchBG			= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_SEARCH_BG);
	m_pBuy_FilterTop		= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_FILTER_BG_TOP);
	m_pBuy_FilterMiddle		= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_FILTER_BG_MIDDLE);
	m_pBuy_FilterBottom		= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_FILTER_BG_BOTTOM);
	m_pBuy_PageTop			= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_PAGE_BG_TOP);
	m_pBuy_PageBottom		= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_PAGE_BG_BOTTOM);
	m_pBuy_SellerBG			= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_SELLER_BG);
	m_pBuy_Category1		= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_CATEGORY1_BG);
	m_pBuy_Category2		= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_CATEGORY2_BG);
	m_pBuy_RareItem			= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_RAREITEM_BG);
	m_pBuy_LevelMinBG		= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_LEVELMIN_BG);
	m_pBuy_LevelMaxBG		= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_LEVELMAX_BG);
	m_pBuy_Text1			= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_TEXT1);
	m_pBuy_Text2			= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_TEXT2);
	m_pBuy_PagePrint		= (cStatic*)GetWindowForID(CONSIGNMENT_BUY_PAGE_PRINT);

	m_pBuy_PageLeftBtn		= (cButton*)GetWindowForID(CONSIGNMENT_BUY_PAGELEFT_BTN);
	m_pBuy_PageRightBtn		= (cButton*)GetWindowForID(CONSIGNMENT_BUY_PAGERIGHT_BTN);
	m_pBuy_SortNameBtn		= (cButton*)GetWindowForID(CONSIGNMENT_BUY_SORTNAME_BTN);
	m_pBuy_SortLevelBtn		= (cButton*)GetWindowForID(CONSIGNMENT_BUY_SORTLEVEL_BTN);
	m_pBuy_SortPriceBtn		= (cButton*)GetWindowForID(CONSIGNMENT_BUY_SORTPRICE_BTN);
	m_pBuy_BuyBtn			= (cButton*)GetWindowForID(CONSIGNMENT_BUY_BUY_BTN);
	m_pBuy_SearchBtn		= (cButton*)GetWindowForID(CONSIGNMENT_BUY_SEARCH_BTN);
	m_pBuy_ResetBtn			= (cButton*)GetWindowForID(CONSIGNMENT_BUY_RESET_BTN);
	m_pBuy_SearchEdit		= (cEditBox*)GetWindowForID(CONSIGNMENT_BUY_SEARCH_EDT);
	m_pBuy_LevelMinEdit		= (cEditBox*)GetWindowForID(CONSIGNMENT_BUY_LEVELMIN_EDT);
	m_pBuy_LevelMaxEdit		= (cEditBox*)GetWindowForID(CONSIGNMENT_BUY_LEVELMAX_EDT);
	m_pBuy_RareItemCombo	= (cComboBox*)GetWindowForID(CONSIGNMENT_BUY_RAREITEM_CMB);
	for(int i=0; i<5; i++)
		m_pBuy_ListItemDlg[i]	= (cIconDialog*)GetWindowForID(CONSIGNMENT_BUY_LIST_ICON1 + i);

	m_pBuy_KeywordList		= (cListDialog*)GetWindowForID(CONSIGNMENT_BUY_KEYWORD);
	m_pBuy_Category1List	= (cListDialog*)GetWindowForID(CONSIGNMENT_BUY_CATEGORY1_LIST);
	m_pBuy_Category2List	= (cListDialog*)GetWindowForID(CONSIGNMENT_BUY_CATEGORY2_LIST);
	m_pBuy_ItemList			= (cListDialog*)GetWindowForID(CONSIGNMENT_BUY_ITEMLIST);
	m_pBuy_ItemList_Lv		= (cListDialog*)GetWindowForID(CONSIGNMENT_BUY_ITEMLIST_LV);
	m_pBuy_ItemList_Seller	= (cListDialog*)GetWindowForID(CONSIGNMENT_BUY_ITEMLIST_SELLER);
	m_pBuy_ItemList_Price	= (cListDialog*)GetWindowForID(CONSIGNMENT_BUY_ITEMLIST_PRICE);


	m_BuyControlListArray.AddTail(m_pBuy_SearchBG);
	m_BuyControlListArray.AddTail(m_pBuy_FilterTop);
	m_BuyControlListArray.AddTail(m_pBuy_FilterMiddle);
	m_BuyControlListArray.AddTail(m_pBuy_FilterBottom);
	m_BuyControlListArray.AddTail(m_pBuy_PageTop);
	m_BuyControlListArray.AddTail(m_pBuy_PageBottom);
	m_BuyControlListArray.AddTail(m_pBuy_SellerBG);
	m_BuyControlListArray.AddTail(m_pBuy_Category1);
	m_BuyControlListArray.AddTail(m_pBuy_Category2);
	m_BuyControlListArray.AddTail(m_pBuy_RareItem);
	m_BuyControlListArray.AddTail(m_pBuy_LevelMinBG);
	m_BuyControlListArray.AddTail(m_pBuy_LevelMaxBG);
	m_BuyControlListArray.AddTail(m_pBuy_Text1);
	m_BuyControlListArray.AddTail(m_pBuy_Text2);
	m_BuyControlListArray.AddTail(m_pBuy_PagePrint);
	m_BuyControlListArray.AddTail(m_pBuy_PageLeftBtn);
	m_BuyControlListArray.AddTail(m_pBuy_PageRightBtn);
	m_BuyControlListArray.AddTail(m_pBuy_SortNameBtn);
	m_BuyControlListArray.AddTail(m_pBuy_SortLevelBtn);
	m_BuyControlListArray.AddTail(m_pBuy_SortPriceBtn);
	m_BuyControlListArray.AddTail(m_pBuy_BuyBtn);
	m_BuyControlListArray.AddTail(m_pBuy_SearchBtn);
	m_BuyControlListArray.AddTail(m_pBuy_ResetBtn);
	m_BuyControlListArray.AddTail(m_pBuy_SearchEdit);
	m_BuyControlListArray.AddTail(m_pBuy_LevelMinEdit);
	m_BuyControlListArray.AddTail(m_pBuy_LevelMaxEdit);
	m_BuyControlListArray.AddTail(m_pBuy_RareItemCombo);
	for(int i=0; i<5; i++)
		m_BuyControlListArray.AddTail(m_pBuy_ListItemDlg[i]);

	m_BuyControlListArray.AddTail(m_pBuy_Category1List);
	m_BuyControlListArray.AddTail(m_pBuy_Category2List);
	m_BuyControlListArray.AddTail(m_pBuy_ItemList);
	m_BuyControlListArray.AddTail(m_pBuy_ItemList_Lv);
	m_BuyControlListArray.AddTail(m_pBuy_ItemList_Seller);
	m_BuyControlListArray.AddTail(m_pBuy_ItemList_Price);




	// 링크 후 처리 (초기화등..)
	m_pReg_UnitPriceEdit->SetValidCheck( VCM_NUMBER );
	m_pReg_UnitPriceEdit->SetAlign( TXT_RIGHT );
	m_pReg_UnitPriceEdit->SetReadOnly( FALSE );
	m_pReg_UnitPriceEdit->SetEditText( "0" );

	m_pReg_ItemList->SetShowSelect( TRUE );
	m_pReg_ItemList_Lv->SetShowSelect( TRUE );
	m_pReg_ItemList_Time->SetShowSelect( TRUE );
	m_pReg_ItemList_Price->SetShowSelect( TRUE );

	m_pBuy_ItemList->SetShowSelect( TRUE );
	m_pBuy_ItemList_Lv->SetShowSelect( TRUE );
	m_pBuy_ItemList_Seller->SetShowSelect( TRUE );
	m_pBuy_ItemList_Price->SetShowSelect( TRUE );

	DWORD dwColor = RGBA_MAKE( 255, 255, 255, 0 );

	stCategoryInfo1* pInfo1;
	m_htCategory1Info.SetPositionHead();
	while(NULL != (pInfo1 = m_htCategory1Info.GetData()))
	{
		m_pBuy_Category1List->AddItem(pInfo1->szCategory1Name, dwColor);
	}
	m_pBuy_Category1List->SetClickedRowIdx(0);
	UpdateCategory2List();

	m_pBuy_RareItemCombo->SetCurSelectedIdx(0);
	m_pBuy_KeywordList->SetActive(FALSE);


	PTRLISTSEARCHSTART(m_BuyControlListArray, cWindow*,pWin)
	pWin->SetActive(FALSE);
	PTRLISTSEARCHEND


	cImage imgToolTip;
	SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );

	ZeroMemory( &m_EmptyItemBase, sizeof( m_EmptyItemBase ) );
	m_RegistIcon = new CItem( &m_EmptyItemBase );
	m_RegistIcon->SetToolTip( "", RGB_HALF( 255, 255, 255), &imgToolTip );
	m_RegistIcon->SetMovable(FALSE);

	for(int i=0; i<DISPLAYNUM_PER_PAGE; i++)
	{
		m_RegistedIcon[i] = new CItem( &m_EmptyItemBase );
		m_RegistedIcon[i]->SetToolTip( "", RGB_HALF( 255, 255, 255), &imgToolTip );
		m_RegistedIcon[i]->SetMovable(FALSE);

		m_SearchIcon[i] = new CItem( &m_EmptyItemBase );
		m_SearchIcon[i]->SetToolTip( "", RGB_HALF( 255, 255, 255), &imgToolTip );
		m_SearchIcon[i]->SetMovable(FALSE);
	}

	m_pBuy_PagePrint->SetStaticText("0 / 0");
}

void CConsignmentDlg::Render()
{
	if(FALSE == IsActive())
	{
		return;
	}

	static DWORD color = TTTC_ONE;
	static DWORDEX dwTotalPrice = 0;
	static double fDeposit = 0;
	static double fCommition = 0;

	switch(m_nMode)
	{
	case eConsignment_Mode_Regist:
		{
			if( m_bActive )
			{
				DWORD durability = m_RegistIcon->GetDurability();
				if(! durability)
					durability = 1;

				DWORDEX money = _atoi64(RemoveComma(m_pReg_UnitPriceEdit->GetEditText()));
				if( money )
				{
					color = GetMoneyColor((DWORD)money);

					if(m_RegistIcon->GetItemIdx())
					{
						dwTotalPrice = money * durability;
						m_pReg_TotalPrice->SetStaticText(AddComma(dwTotalPrice));
					}

					fDeposit = (float)(dwTotalPrice * CONSIGNMENT_DEPOSIT_RATE);
					fDeposit = ceil(fDeposit);
					fCommition = (float)(money * CONSIGNMENT_COMMISSION_RATE) * durability ;
					fCommition = ceil(fCommition);

					m_pReg_Deposit->SetStaticText(AddComma((DWORD)fDeposit));
					m_pReg_Commission->SetStaticText(AddComma((DWORD)fCommition));
				}
			}
			
			m_pReg_UnitPriceEdit->SetNonactiveTextColor( color );
			m_pReg_UnitPriceEdit->SetActiveTextColor( color );
		}
		break;

	case eConsignment_Mode_Buy:
		{
			if(m_pBuy_KeywordList->IsShowScrool())
			{
				m_pBuy_Category1->SetActive(FALSE);
				m_pBuy_Category1List->SetActive(FALSE);
				m_pBuy_Category2->SetActive(FALSE);
				m_pBuy_Category2List->SetActive(FALSE);
				m_pBuy_RareItem->SetActive(FALSE);
				m_pBuy_RareItemCombo->SetActive(FALSE);
				m_pBuy_LevelMinBG->SetActive(FALSE);
				m_pBuy_LevelMinEdit->SetActive(FALSE);
				m_pBuy_LevelMaxBG->SetActive(FALSE);
				m_pBuy_LevelMaxEdit->SetActive(FALSE);
				m_pBuy_Text2->SetActive(FALSE);
			}
			else if(m_pBuy_Category1List->IsShowScrool())
			{
				m_pBuy_Category2->SetActive(FALSE);
				m_pBuy_Category2List->SetActive(FALSE);
				m_pBuy_RareItem->SetActive(FALSE);
				m_pBuy_RareItemCombo->SetActive(FALSE);
				m_pBuy_LevelMinBG->SetActive(FALSE);
				m_pBuy_LevelMinEdit->SetActive(FALSE);
				m_pBuy_LevelMaxBG->SetActive(FALSE);
				m_pBuy_LevelMaxEdit->SetActive(FALSE);
				m_pBuy_Text2->SetActive(FALSE);
			}
			else if(m_pBuy_Category2List->IsShowScrool())
			{
				m_pBuy_RareItem->SetActive(FALSE);
				m_pBuy_RareItemCombo->SetActive(FALSE);
				m_pBuy_LevelMinBG->SetActive(FALSE);
				m_pBuy_LevelMinEdit->SetActive(FALSE);
				m_pBuy_LevelMaxBG->SetActive(FALSE);
				m_pBuy_LevelMaxEdit->SetActive(FALSE);
				m_pBuy_Text2->SetActive(FALSE);
			}
			else
			{
				m_pBuy_Category1->SetActive(TRUE);
				m_pBuy_Category1List->SetActive(TRUE);
				m_pBuy_Category2->SetActive(TRUE);
				m_pBuy_Category2List->SetActive(TRUE);
				m_pBuy_RareItem->SetActive(TRUE);
				m_pBuy_RareItemCombo->SetActive(TRUE);
				m_pBuy_LevelMinBG->SetActive(TRUE);
				m_pBuy_LevelMinEdit->SetActive(TRUE);
				m_pBuy_LevelMaxBG->SetActive(TRUE);
				m_pBuy_LevelMaxEdit->SetActive(TRUE);
				m_pBuy_Text2->SetActive(TRUE);
			}

		}
		break;
	}

	m_pHaveMoney->SetStaticText(AddComma(HERO->GetMoney()));
	

	cDialog::Render();
}

void CConsignmentDlg::SetActive( BOOL val )
{
	cDialog::SetActive( val );

	ClearControls(m_nMode);

	if(! val )
	{
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
	}
}

DWORD CConsignmentDlg::ActionEvent(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;
	we = cDialog::ActionEvent(mouseInfo);

	return we;
}

void CConsignmentDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN)
	{
		if(m_bHold)
			return;

 		switch( lId )
		{
			case CONSIGNMENT_PUSHBTN_BUY:
				SetMode(eConsignment_Mode_Buy);
			break;

			case CONSIGNMENT_PUSHBTN_REGIST:
				ResetFilter();
				SetMode(eConsignment_Mode_Regist);
				CONSIGNMENTMGR->LoadRegistedList();
			break;
		}
	}
	else if( we & WE_BTNCLICK)
	{
		if(m_bHold)
			return;

		switch( lId )
		{
			case CONSIGNMENT_BTN_CLOSE:
				SetActive(FALSE);
				break;

			case CONSIGNMENT_BTN_HELP:
				{
					cDialog* pDlg = WINDOWMGR->GetWindowForID( CONSIGNMENTGUIDE_DLG );
					if(pDlg)
					{
						pDlg->SetActive( ! pDlg->IsActive() );
					}
				}
				break;

			case CONSIGNMENT_REG_REGIST_BTN:
				{
					SendRegistMsg();
				}
				break;

			case CONSIGNMENT_BUY_SEARCH_BTN:
				{
					if(FALSE == CheckSearchTime())		
						return;

					m_wCurSelPage = 1;
					SendSearchMsg();
				}break;

			case CONSIGNMENT_BUY_SORTNAME_BTN:
				{
					/*
					if(FALSE == CheckSearchTime())		
						return;

					if(m_wSortType == eConsignmentSORT_NameAsc)
						m_wSortType = eConsignmentSORT_NameDesc;
					else
						m_wSortType = eConsignmentSORT_NameAsc;

					SendSearchMsg();
					*/
				}
				break;

			case CONSIGNMENT_BUY_SORTLEVEL_BTN:
				{
					if(FALSE == CheckSearchTime() || 0==m_SearchList.ItemList[0].dwConsignmentIdx)
						return;

					if(m_wSortType == eConsignmentSORT_LevelAsc)
					{
						m_wSortType = eConsignmentSORT_LevelDesc;
						m_pBuy_SortLevelBtn->SetText(RESRCMGR->GetMsg(1473), RGB_HALF(255,230,20));
					}
					else
					{
						m_wSortType = eConsignmentSORT_LevelAsc;
						m_pBuy_SortLevelBtn->SetText(RESRCMGR->GetMsg(1472), RGB_HALF(255,230,20));
					}
					
					m_wCurSelPage = 1;
					SendSearchMsg();
				}
				break;

			case CONSIGNMENT_BUY_SORTPRICE_BTN:
				{
					if(FALSE == CheckSearchTime() || 0==m_SearchList.ItemList[0].dwConsignmentIdx)		
						return;

					if(m_wSortType == eConsignmentSORT_PriceAsc)
					{
						m_wSortType = eConsignmentSORT_PriceDesc;
						m_pBuy_SortPriceBtn->SetText(RESRCMGR->GetMsg(1475), RGB_HALF(255,230,20));
					}
					else
					{
						m_wSortType = eConsignmentSORT_PriceAsc;
						m_pBuy_SortPriceBtn->SetText(RESRCMGR->GetMsg(1474), RGB_HALF(255,230,20));
					}

					m_wCurSelPage = 1;
					SendSearchMsg();
				}
				break;

			case CONSIGNMENT_BUY_RESET_BTN:
				{
					ResetFilter();
				}
				break;

			case CONSIGNMENT_BUY_BUY_BTN:
				{
					DWORD dwConsignmentIndex = GetConsignmentIndex();
					if(dwConsignmentIndex == 0)
						return;

					ConfirmBuy(dwConsignmentIndex);
				}
				break;

			case CONSIGNMENT_BUY_PAGERIGHT_BTN:
				{
					if(m_wMaxSearchPage > m_wCurSelPage)
					{
						if(FALSE == CheckSearchTime())		
							return;

						m_wCurSelPage++;
						SendSearchMsg();
					}
				}
				break;

			case CONSIGNMENT_BUY_PAGELEFT_BTN:
				{
					if(m_wCurSelPage > 1)
					{
						if(FALSE == CheckSearchTime())		
							return;

						m_wCurSelPage--;
						SendSearchMsg();
					}
				}
				break;

			case CONSIGNMENT_REG_CANCEL1_BTN:
			case CONSIGNMENT_REG_CANCEL2_BTN:
			case CONSIGNMENT_REG_CANCEL3_BTN:
			case CONSIGNMENT_REG_CANCEL4_BTN:
			case CONSIGNMENT_REG_CANCEL5_BTN:
				ConfirmCancel((WORD)lId);
				break;
		}
	}
	else if(we & WE_ROWCLICK)
	{
		if(m_bHold)
			return;

		switch( lId )
		{
		case CONSIGNMENT_REG_ITEMLIST:
			{
				int nClickIdx = m_pReg_ItemList->GetClickedRowIdx();
				m_pReg_ItemList_Lv->SetCurSelectedRowIdxWithoutRowMove( nClickIdx );
				m_pReg_ItemList_Time->SetCurSelectedRowIdxWithoutRowMove( nClickIdx );
				m_pReg_ItemList_Price->SetCurSelectedRowIdxWithoutRowMove( nClickIdx );

				UpdateRegIconList();
			}
			break;

		case CONSIGNMENT_BUY_ITEMLIST:
			{
				int nClickIdx = m_pBuy_ItemList->GetClickedRowIdx();
				m_pBuy_ItemList_Lv->SetCurSelectedRowIdxWithoutRowMove( nClickIdx );
				m_pBuy_ItemList_Seller->SetCurSelectedRowIdxWithoutRowMove( nClickIdx );
				m_pBuy_ItemList_Price->SetCurSelectedRowIdxWithoutRowMove( nClickIdx );
				UpdateBuyIconList();
			}
			break;

		case CONSIGNMENT_BUY_CATEGORY1_LIST:
			{
				UpdateCategory2List();
				SearchOnClient();
			}
			break;

		case CONSIGNMENT_BUY_CATEGORY2_LIST:
			{
				SearchOnClient();
			}
			break;
		case CONSIGNMENT_BUY_KEYWORD:
			{
				char buf[256] = {0,};
				strcpy(buf, m_pBuy_KeywordList->GetClickedItem());

				m_pBuy_SearchEdit->SetEditText(buf);
				m_pBuy_KeywordList->SetActive(FALSE);

				SetControlByItemIndex(buf);
			}
			break;
		}
	}
	else if(we & WE_COMBOBOXSELECT)
	{
		if(m_bHold)
			return;
		
		switch( lId )
		{
			case CONSIGNMENT_BUY_RAREITEM_CMB:
			{
				SearchOnClient();
			}
			break;
		}

	}
	else if(we & WE_TOPROWCHANGE)
	{
		if(m_bHold)
			return;

		switch( lId )
		{
		case CONSIGNMENT_REG_ITEMLIST:
			{
				int nTopListItemIdx = m_pReg_ItemList->GetTopListItemIdx();
				m_pReg_ItemList_Lv->SetTopListItemIdx( nTopListItemIdx );
				m_pReg_ItemList_Time->SetTopListItemIdx( nTopListItemIdx );
				m_pReg_ItemList_Price->SetTopListItemIdx( nTopListItemIdx );

				UpdateRegIconList();
			}
			break;

		case CONSIGNMENT_BUY_ITEMLIST:
			{
				int nTopListItemIdx = m_pBuy_ItemList->GetTopListItemIdx();
				m_pBuy_ItemList_Lv->SetTopListItemIdx( nTopListItemIdx );
				m_pBuy_ItemList_Seller->SetTopListItemIdx( nTopListItemIdx );
				m_pBuy_ItemList_Price->SetTopListItemIdx( nTopListItemIdx );
				UpdateBuyIconList();
			}
			break;
		}
	}
}

DWORD CConsignmentDlg::ActionKeyboardEvent( CKeyboard * keyInfo )
{
	DWORD we = WE_NULL ;

	if( !m_bActive ) return we ;

	we |= cDialog::ActionKeyboardEvent(keyInfo) ;

	// 포커스 있으면,
	if(GetMode()==eConsignment_Mode_Buy && m_pBuy_SearchEdit->IsFocus())
	{
		if( keyInfo->GetKeyPressed(KEY_RETURN) || keyInfo->GetKeyPressed(KEY_PADENTER) )
		{
			SendSearchMsg();
		}
		else if( FALSE == keyInfo->IsEmptyKeyboardEvent() )
		{
			SearchOnClient();
		}
	}
	else if(GetMode()==eConsignment_Mode_Regist && m_pReg_UnitPriceEdit->IsFocus())
	{
		if( keyInfo->GetKeyPressed(KEY_RETURN) || keyInfo->GetKeyPressed(KEY_PADENTER) )
		{
			SendRegistMsg();
		}
	}

	return we ;
}

void CConsignmentDlg::SetMode(int nMode)
{
	m_nMode = nMode;
	PTRLISTSEARCHSTART(m_BuyControlListArray, cWindow*,pWin)
	pWin->SetActive(FALSE);
	PTRLISTSEARCHEND

	PTRLISTSEARCHSTART(m_RegistControlListArray, cWindow*,pWin)
	pWin->SetActive(FALSE);
	PTRLISTSEARCHEND

	ClearControls(nMode);

	switch(nMode)
	{
	case eConsignment_Mode_Regist:
		{
			ZeroMemory(&m_RegistedList, sizeof(m_RegistedList));
			m_dwBuyDurability = 0;

			m_PageBuyBtn->SetPush(FALSE);
			m_PageRegistBtn->SetPush(TRUE);

			PTRLISTSEARCHSTART(m_RegistControlListArray, cWindow*,pWin)
			pWin->SetActive(TRUE);
			PTRLISTSEARCHEND
		}
		break;

	case eConsignment_Mode_Buy:
		{
			m_pBuy_Category1List->SetClickedRowIdx(0);
			UpdateCategory2List();
			m_pBuy_RareItemCombo->SetCurSelectedIdx(0);

			ZeroMemory(&m_SearchList, sizeof(m_SearchList));
			m_dwBuyDurability = 0;

			m_wSortType = eConsignmentSORT_LevelAsc;
			m_wCurSelPage = 1;
			m_wMaxSearchPage = 1;

			m_PageRegistBtn->SetPush(FALSE);
			m_PageBuyBtn->SetPush(TRUE);

			PTRLISTSEARCHSTART(m_BuyControlListArray, cWindow*,pWin)
			pWin->SetActive(TRUE);
			PTRLISTSEARCHEND
		}
		break;
	}
}

BOOL CConsignmentDlg::FakeMoveIcon(LONG x, LONG y, cIcon * pOrigIcon)
{
	if(m_nMode!=eConsignment_Mode_Regist || m_bHold)
		return FALSE;

	if( WT_ITEM != pOrigIcon->GetType() || pOrigIcon->IsLocked() || m_bDisable)
	{
		return FALSE;
	}

	CItem * pOrigItem = (CItem *)pOrigIcon;

	// 인벤토리의 아이템만 허용된다
	if( ! ITEMMGR->IsEqualTableIdxForPos(eItemTable_Inventory, pOrigItem->GetPosition() ) )
	{
		return FALSE;
	}

	ITEMBASE BaseInfo = pOrigItem->GetItemBaseInfo();
	ITEM_OPTION OptionInfo = ITEMMGR->GetOption(BaseInfo);
	const ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(BaseInfo.wIconIdx);
	if(! pItemInfo)
		return FALSE;

	if(BaseInfo.nSealed == eITEM_TYPE_UNSEAL ||
		! pItemInfo->Trade ||
		! pItemInfo->Sell)
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2256) );
		return FALSE;
	}
	
	if(m_RegistIcon->GetLinkItem())
		m_RegistIcon->GetLinkItem()->SetLock(FALSE);

	WINDOWMGR->SetWindowTop( this );

	SetRegistItem(&BaseInfo, &OptionInfo, pOrigIcon);
	
	return FALSE;
}

void CConsignmentDlg::SetRegistItem(ITEMBASE* pBaseInfo, ITEM_OPTION* pOptionInfo, cIcon* pOrigIcon)
{
	m_RegistIcon->SetData(0);
	m_pReg_ItemDlg->DeleteIcon(0, NULL);

	if(! pBaseInfo)
		return;

	if( m_RegistIcon )
	{
		m_RegistIcon->SetLinkItem((CExchangeItem*) pOrigIcon);
		pOrigIcon->SetLock(TRUE);

		ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( pBaseInfo->wIconIdx );
		if( !pItemInfo )
			return;	

		cImage image;
		m_RegistIcon->Init( 0, 0, DEFAULT_ICONSIZE, DEFAULT_ICONSIZE, ITEMMGR->GetIconImage( pItemInfo->ItemIdx, &image ), 0 );
		m_RegistIcon->SetItemBaseInfo( *pBaseInfo );
		ITEMMGR->AddOption(*pOptionInfo);
		m_RegistIcon->SetIconType( eIconType_AllItem );
		m_RegistIcon->SetData( pItemInfo->ItemIdx );

		ITEMMGR->AddToolTip( m_RegistIcon );
		m_pReg_ItemDlg->AddIcon( 0, m_RegistIcon );

		m_pReg_RegistBtn->SetDisable(FALSE);

		if(m_dwLastRegistItemIdx == m_RegistIcon->GetItemIdx())
			m_pReg_UnitPriceEdit->SetEditText(AddComma(m_dwLastRegistItemUnitPrice));
		else
			m_pReg_UnitPriceEdit->SetEditText("");

		m_pReg_UnitPriceEdit->SetFocusEdit(TRUE);
	}
}

void CConsignmentDlg::SendRegistMsg()
{
	if(0 == m_RegistIcon->GetItemIdx())
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2277) );
		return;
	}

	DWORDEX dwUnitPrice = _atoi64(RemoveComma(m_pReg_UnitPriceEdit->GetEditText()));
	if(dwUnitPrice < 100)
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2241) );
		return;
	}
	else if(dwUnitPrice >= MAX_INVENTORY_MONEY)
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2279) );
		return;
	}

	MSG_CONSIGNMENT_REGIST msg;
	msg.Category = MP_CONSIGNMENT;
	msg.Protocol = MP_CONSIGNMENT_REGIST_SYN;
	msg.dwObjectID = HEROID;
	msg.ItemInfo = m_RegistIcon->GetItemBaseInfo();
	const char* pStaticText = m_pReg_Deposit->GetStaticText();
	char buf[256] = {0,};
	strcpy(buf, pStaticText);
	strcpy(msg.szItemName, m_RegistIcon->GetItemInfo()->ItemName);
	msg.dwDeposit = (DWORD)_atoi64(RemoveComma(buf));
	float fCommission = (float)dwUnitPrice * (float)CONSIGNMENT_COMMISSION_RATE;	// 서버에 전송시는 "개당수수료"를 보낸다.
	fCommission = ceil(fCommission);
	msg.dwCommission = (DWORD)fCommission;
	msg.dwPrice = (DWORD)dwUnitPrice;
	msg.FromPos = m_RegistIcon->GetPosition();

	GetCategoryValueByItemIndex(m_RegistIcon->GetItemIdx(), msg.dwCategory1Value, msg.dwCategory2Value);
	if(0==msg.dwCategory1Value || 0==msg.dwCategory2Value)
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2249) );
		return;
	}

	NETWORK->Send( &msg, sizeof(msg) ) ;

	m_dwLastRegistItemIdx = m_RegistIcon->GetItemIdx();
	m_dwLastRegistItemUnitPrice = (DWORD)dwUnitPrice;

	SetHold(TRUE);
}

void CConsignmentDlg::ClearControls(int nMode)
{
	switch(nMode)
	{
	case eConsignment_Mode_Regist:
		{
			if(m_RegistIcon->GetLinkItem())
			{
				m_RegistIcon->SetData(0);
				m_RegistIcon->SetItemBaseInfo(m_EmptyItemBase);
				m_RegistIcon->GetLinkItem()->SetLock(FALSE);
				m_RegistIcon->SetLinkItem(NULL);
				m_pReg_ItemDlg->DeleteIcon(0, NULL);
			}

			for(int i=0; i<DISPLAYNUM_PER_PAGE; i++)
			{
				m_RegistedIcon[i]->SetData(0);
				m_pReg_RegistedItemDlg[i]->DeleteIcon(0, NULL);
			}

			m_pReg_UnitPriceEdit->SetEditText( "0" );
			m_pReg_TotalPrice->SetStaticText("0");
			m_pReg_Deposit->SetStaticText("0");
			m_pReg_Commission->SetStaticText("0");

			m_pReg_ItemList->RemoveAll();
			m_pReg_ItemList_Lv->RemoveAll();
			m_pReg_ItemList_Time->RemoveAll();
			m_pReg_ItemList_Price->RemoveAll();
		}
		break;

	case eConsignment_Mode_Buy:
		{
			if(m_RegistIcon->GetLinkItem())
			{
				m_RegistIcon->SetData(0);
				m_RegistIcon->SetItemBaseInfo(m_EmptyItemBase);
				m_RegistIcon->GetLinkItem()->SetLock(FALSE);
				m_RegistIcon->SetLinkItem(NULL);
				m_pReg_ItemDlg->DeleteIcon(0, NULL);
			}

			for(int i=0; i<DISPLAYNUM_PER_PAGE; i++)
			{
				m_SearchIcon[i]->SetData(0);
				m_pBuy_ListItemDlg[i]->DeleteIcon(0, NULL);
			}

			m_pBuy_SearchEdit->SetEditText("");
			m_pBuy_LevelMinEdit->SetEditText(SEARCH_MIN_LEVEL);
			m_pBuy_LevelMaxEdit->SetEditText(SEARCH_MAX_LEVEL);

			m_pBuy_Category1List->SetCurSelectedRowIdx(0);
			m_pBuy_Category2List->SetCurSelectedRowIdx(0);
			m_pBuy_RareItemCombo->SetCurSelectedIdx(0);

			m_pBuy_ItemList->RemoveAll();
			m_pBuy_ItemList_Lv->RemoveAll();
			m_pBuy_ItemList_Seller->RemoveAll();
			m_pBuy_ItemList_Price->RemoveAll();
		}
		break;
	}
}

DWORD CConsignmentDlg::GetConsignmentIndex()
{
	int nCount = -1;
	DWORD dwConsignmentIndex = 0;

	switch(m_nMode)
	{
	case eConsignment_Mode_Regist:
		nCount = m_pReg_ItemList->GetClickedRowIdx();
		if(nCount >= 0)
			dwConsignmentIndex = m_RegistedList.ItemList[nCount].dwConsignmentIdx;
		break;
	case eConsignment_Mode_Buy:
		nCount = m_pBuy_ItemList->GetClickedRowIdx();
		if(nCount >= 0)
			dwConsignmentIndex = m_SearchList.ItemList[nCount].dwConsignmentIdx;
		break;
	}

	return dwConsignmentIndex;
}

void CConsignmentDlg::LoadCategoryInfo()
{
	CMHFile file;
	if( !file.Init( "./Data/Interface/Windows/ConsignmentCategory.bin", "rb" ) )
		return;

	eConsignmentCategoryType CategoryType = eConsignmentCategory_Max;

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
			CategoryType = eConsignmentCategory_Max;
		}
		else if( strcmp( buf, "$Category1" ) == 0 )
		{
			CategoryType = eConsignmentCategory1;
		}
		else if( strcmp( buf, "$Category2" ) == 0 )
		{
			CategoryType = eConsignmentCategory2;
		}
		else if( strcmp( buf, "#AddList" ) == 0 )
		{
			switch( CategoryType )
			{
			case eConsignmentCategory1:
				{
					stCategoryInfo1* pInfo = new stCategoryInfo1;
					pInfo->dwCategory1Value			= file.GetDword();
					file.GetString(pInfo->szCategory1Name);

					m_htCategory1Info.Add(pInfo, pInfo->dwCategory1Value);
				}
				break;
			case eConsignmentCategory2:
				{
					stCategoryInfo2* pInfo = new stCategoryInfo2;
					pInfo->dwCategory1Value			= file.GetDword();
					pInfo->dwCategory2Value			= file.GetDword();
					file.GetString(pInfo->szCategory2Name);
					pInfo->dwItemTypeDetail			= file.GetDword();

					m_htCategory2Info.Add(pInfo, pInfo->dwItemTypeDetail);
				}
				break;
			}
		}
	}
}

DWORD CConsignmentDlg::GetCategory1Value(const char* pName)
{
	stCategoryInfo1* pInfo1;
	m_htCategory1Info.SetPositionHead();
	while(NULL != (pInfo1 = m_htCategory1Info.GetData()))
	{
		if(0 == strcmp(pName, pInfo1->szCategory1Name))
			return pInfo1->dwCategory1Value;
	}

	return 0;
}

DWORD CConsignmentDlg::GetCategory2Value(const char* pName)
{
	stCategoryInfo2* pInfo2;
	m_htCategory2Info.SetPositionHead();
	while(NULL != (pInfo2 = m_htCategory2Info.GetData()))
	{
		if(0 == strcmp(pName, pInfo2->szCategory2Name))
			return pInfo2->dwCategory2Value;
	}

	return 0;
}

DWORD CConsignmentDlg::GetCategory2Value(DWORD dwItemTypeDetail)
{
	stCategoryInfo2* pInfo2 = m_htCategory2Info.GetData(dwItemTypeDetail);
	if(pInfo2)
		return pInfo2->dwCategory2Value;

	return 0;
}

void CConsignmentDlg::GetCategoryValueByItemIndex(DWORD dwItemIndex, DWORD& dwCategory1, DWORD& dwCategory2)
{
	dwCategory1 = 0;
	dwCategory2 = 0;
	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( dwItemIndex );
	if(! pItemInfo)
		return;
	
	stCategoryInfo2* pInfo2 = m_htCategory2Info.GetData(pItemInfo->dwTypeDetail);
	if(! pInfo2)
		return;

	dwCategory1 = pInfo2->dwCategory1Value;
	dwCategory2 = pInfo2->dwCategory2Value;
}

void CConsignmentDlg::UpdateCategory2List()
{
	m_pBuy_Category2List->RemoveAll();

	const char* pName = m_pBuy_Category1List->GetClickedItem();
	DWORD dwCategory1Value = GetCategory1Value(pName);
	DWORD dwLastAddedValue = DWORD(-1);

	DWORD dwColor = RGBA_MAKE( 255, 255, 255, 0 );
	stCategoryInfo2* pInfo2;
	m_htCategory2Info.SetPositionHead();
	while(NULL != (pInfo2 = m_htCategory2Info.GetData()))
	{
		if(dwCategory1Value == pInfo2->dwCategory1Value &&
			dwLastAddedValue != pInfo2->dwCategory2Value)
		{
			m_pBuy_Category2List->AddItem(pInfo2->szCategory2Name, dwColor);
			dwLastAddedValue = pInfo2->dwCategory2Value;
		}
	}
	m_pBuy_Category2List->SetClickedRowIdx(0);
	m_pBuy_Category2List->SetCurSelectedRowIdx(0);

	m_pBuy_Category1List->SetDisableScroll();
	m_pBuy_Category2List->SetDisableScroll();
}

void CConsignmentDlg::ConfirmCancel(WORD nBtnIndex)
{
	if(m_nMode != eConsignment_Mode_Regist)
		return;

	int nCount = m_pReg_ItemList->GetTopListItemIdx() + (nBtnIndex - CONSIGNMENT_REG_CANCEL1_BTN);
	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( m_RegistedList.ItemList[nCount].dwItemIndex );
	if( !pItemInfo )
		return;	

	m_pReg_ItemList->SetClickedRowIdx(nCount);
	m_pReg_ItemList_Lv->SetCurSelectedRowIdxWithoutRowMove( nCount );
	m_pReg_ItemList_Time->SetCurSelectedRowIdxWithoutRowMove( nCount );
	m_pReg_ItemList_Price->SetCurSelectedRowIdxWithoutRowMove( nCount );

	char text[256] = {0,};
	if(m_RegistedList.ItemList[nCount].wEnchant > 0)
		sprintf(text, "+%d %s", m_RegistedList.ItemList[nCount].wEnchant, pItemInfo->ItemName);
	else
		sprintf(text, "%s", pItemInfo->ItemName);

	WINDOWMGR->MsgBox( MBI_CONSIGNMENT_CANCEL_CONFIRM, MBT_YESNO, CHATMGR->GetChatMsg(2242), text) ;
}

void CConsignmentDlg::UpdateRegIconList()
{
	if(m_nMode != eConsignment_Mode_Regist)
		return;

	cButton* pBtn = NULL;
	for(int i=0; i<DISPLAYNUM_PER_PAGE; i++)
	{
		m_RegistedIcon[i]->SetData(0);
		m_pReg_RegistedItemDlg[i]->DeleteIcon(0, NULL);

		pBtn = (cButton*)GetWindowForID(CONSIGNMENT_REG_CANCEL1_BTN + i);
		if(pBtn)
			pBtn->SetActive(FALSE);
	}

	int nCount = m_pReg_ItemList->GetTopListItemIdx();
	int nMax = nCount + DISPLAYNUM_PER_PAGE;
	if(nMax > MAX_ITEMNUM_PER_PAGE)
		nMax = MAX_ITEMNUM_PER_PAGE;

	for(int i=nCount, nIconNum=0; i<nMax; i++, nIconNum++)
	{
		if( m_RegistedIcon[nIconNum] )
		{
			ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( m_RegistedList.ItemList[i].dwItemIndex );
			if( !pItemInfo )
				break;	

			cImage image;
			m_RegistedIcon[nIconNum]->Init( 0, 0, DEFAULT_ICONSIZE, DEFAULT_ICONSIZE, ITEMMGR->GetIconImage( pItemInfo->ItemIdx, &image ), 0 );
			m_RegistedIcon[nIconNum]->SetItemBaseInfo( m_RegistedList.ItemInfo[i] );
			ITEMMGR->AddOption( m_RegistedList.OptionInfo[i] );
			m_RegistedIcon[nIconNum]->SetIconType( eIconType_AllItem );
			m_RegistedIcon[nIconNum]->SetData( pItemInfo->ItemIdx );

			ITEMMGR->AddToolTip( m_RegistedIcon[nIconNum] );
			m_pReg_RegistedItemDlg[nIconNum]->AddIcon( 0, m_RegistedIcon[nIconNum] );

			pBtn = (cButton*)GetWindowForID(CONSIGNMENT_REG_CANCEL1_BTN + nIconNum);
			if(pBtn)
				pBtn->SetActive(TRUE);
		}
	}
}

void CConsignmentDlg::UpdateRegList(int nNum, MSG_CONSIGNMENT_SEARCH_RESULT* pSearchResult)
{
	m_pReg_ItemList->RemoveAll();
	m_pReg_ItemList_Lv->RemoveAll();
	m_pReg_ItemList_Time->RemoveAll();
	m_pReg_ItemList_Price->RemoveAll();

	DWORD dwColor = RGBA_MAKE( 255, 255, 255, 0 );
	char text[256] = {0,};
	DWORD dwDay = 0;
	DWORD dwHour = 0;
	DWORD dwMin = 0;
	ITEM_INFO* pItemInfo;
	stConsignmentItemInfo* pItem;

	for(int cnt=0; cnt<nNum; cnt++)
	{
		if( !pSearchResult->ItemList[cnt].dwConsignmentIdx )
			continue;

		pItemInfo = ITEMMGR->GetItemInfo( pSearchResult->ItemInfo[cnt].wIconIdx );
		if( !pItemInfo )
			continue;

		if( 0 == pSearchResult->ItemList[cnt].dwRemainMinute )
			continue;

		pItem = &pSearchResult->ItemList[cnt];
		pItem->dwItemDBIdx		= pSearchResult->ItemInfo[cnt].dwDBIdx;
		pItem->dwItemIndex		= pSearchResult->ItemInfo[cnt].wIconIdx;
		pItem->dwCategory1		= pItemInfo->dwType;
		pItem->dwCategory2		= pItemInfo->dwTypeDetail;
		pItem->wRareLevel		= (WORD)pItemInfo->kind;
		pItem->wUseLevel		= pItemInfo->LimitLevel;
		pItem->wEnchant			= pSearchResult->OptionInfo[cnt].mEnchant.mLevel;
		pItem->wInitDurability	= pSearchResult->ItemList[cnt].wInitDurability;
		pItem->dwDeposit		= pSearchResult->ItemList[cnt].dwDeposit;
		pItem->dwCommission		= pSearchResult->ItemList[cnt].dwCommission;
		pItem->dwUnitPrice		= pSearchResult->ItemList[cnt].dwUnitPrice;

		if(pItem->wEnchant > 0)
			sprintf(text, "+%d %s", pItem->wEnchant, pItemInfo->ItemName);
		else
			sprintf(text, "%s", pItemInfo->ItemName);

		DWORD dwNameColor = ITEMMGR->GetItemNameColor( *pItemInfo );
		m_pReg_ItemList->AddItem(text, dwNameColor);

		sprintf(text, "%d", pItem->wUseLevel);
		m_pReg_ItemList_Lv->AddItem(text, dwColor);

		dwDay = pItem->dwRemainMinute / (24*60);
		dwHour = pItem->dwRemainMinute / 60;
		dwMin = pItem->dwRemainMinute;

		if(dwDay)
			sprintf(text, "%d%s", dwDay, CHATMGR->GetChatMsg(2259));
		else if(dwHour)
			sprintf(text, "%d%s", dwHour, CHATMGR->GetChatMsg(2260));
		else if(dwMin)
			sprintf(text, "%d%s", dwMin, CHATMGR->GetChatMsg(2261));
		m_pReg_ItemList_Time->AddItem(text, dwColor);

		sprintf(text, "%11s", AddComma(pItem->dwUnitPrice * (pSearchResult->ItemInfo[cnt].Durability ? pSearchResult->ItemInfo[cnt].Durability : 1)));
		m_pReg_ItemList_Price->AddItem(text, dwColor);
	}

	memcpy(&m_RegistedList, pSearchResult, sizeof(MSG_CONSIGNMENT_SEARCH_RESULT));

	m_pReg_ItemList->SetCurSelectedRowIdx(-1);
	m_pReg_ItemList_Lv->SetCurSelectedRowIdx(-1);
	m_pReg_ItemList_Time->SetCurSelectedRowIdx(-1);
	m_pReg_ItemList_Price->SetCurSelectedRowIdx(-1);

	UpdateRegIconList();
}

void CConsignmentDlg::ConfirmBuy(DWORD dwConsignmentIndex)
{
	if(m_nMode != eConsignment_Mode_Buy)
		return;

	int nCount = m_pBuy_ItemList->GetClickedRowIdx();
	if(nCount < 0)
		return;

	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( m_SearchList.ItemList[nCount].dwItemIndex );
	if( !pItemInfo )
		return;	

	m_pBuy_ItemList_Lv->SetCurSelectedRowIdxWithoutRowMove(nCount);
	m_pBuy_ItemList_Seller->SetCurSelectedRowIdxWithoutRowMove(nCount);
	m_pBuy_ItemList_Price->SetCurSelectedRowIdxWithoutRowMove(nCount);

	if(m_SearchList.ItemList[nCount].wEnchant > 0)
		sprintf(m_szBuyItemName, "+%d %s", m_SearchList.ItemList[nCount].wEnchant, pItemInfo->ItemName);
	else
		sprintf(m_szBuyItemName, "%s", pItemInfo->ItemName);


	if(m_SearchList.ItemInfo[nCount].Durability)
	{
		cDivideBox * pDivideBox = WINDOWMGR->DivideBox( MBI_CONSIGNMENT_DIVIDEBUYITEM, 100, 100, OnBuyItem, OnCancelBuyItem,
															this, m_szBuyItemName, CHATMGR->GetChatMsg(2243) );
		pDivideBox->SetValue(m_SearchList.ItemInfo[nCount].Durability);
		SetDisable(TRUE);
	}
	else
	{
		WINDOWMGR->MsgBox( MBI_CONSIGNMENT_BUY_CONFIRM, MBT_YESNO, CHATMGR->GetChatMsg(2244), m_szBuyItemName) ;
	}
}

DWORD CConsignmentDlg::GetBuyItemIndex(DWORD dwConsignmentIndex)
{
	int nCount = -1;

	if(m_nMode && eConsignment_Mode_Buy)
	{
		nCount = m_pBuy_ItemList->GetClickedRowIdx();
		if(nCount >= 0)
			return dwConsignmentIndex = m_SearchList.ItemList[nCount].dwItemIndex;
	}

	return 0;
}

DWORD CConsignmentDlg::GetBuyDurability(DWORD dwConsignmentIndex)
{
	return m_dwBuyDurability ? m_dwBuyDurability : 1;
}

DWORD CConsignmentDlg::GetBuyPrice(DWORD dwConsignmentIndex)
{
	int nCount = -1;

	if(m_nMode && eConsignment_Mode_Buy)
	{
		nCount = m_pBuy_ItemList->GetClickedRowIdx();
		if(nCount >= 0)
			return m_SearchList.ItemList[nCount].dwUnitPrice;
	}

	return 0;
}

void CConsignmentDlg::SendSearchMsg()
{
	char* buf = m_pBuy_SearchEdit->GetEditText();
	int nLen = strlen(buf);

	if(0 == nLen && (0==m_pBuy_Category1List->GetClickedRowIdx() || 0==m_pBuy_Category2List->GetClickedRowIdx()))
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2298));
		m_pBuy_SearchEdit->SetFocusEdit(TRUE);
		return;
	}
	else if(0 == nLen && m_pBuy_Category1List->GetClickedRowIdx()>0)
	{
		;
	}
	else if(m_bOverCount)
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2299));
		m_pBuy_SearchEdit->SetFocusEdit(TRUE);
		return;
	}
	else if(m_dwSearchItemList[0] > 0)
	{
		SendSearchIndexMsg();
		return;
	}
	else
	{
#ifdef _GMTOOL_
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2300));
		return;
#endif
	}

	MSG_CONSIGNMENT_SEARCH_REQUEST msg;
	msg.Category = MP_CONSIGNMENT;
	msg.Protocol = MP_CONSIGNMENT_SEARCH_SYN;
	msg.dwObjectID = HEROID;
	msg.wSortType = m_wSortType;
	msg.wStartPage = m_wCurSelPage;

	const char* pCategory1 = m_pBuy_Category1List->GetClickedItem();
	const char* pCategory2 = m_pBuy_Category2List->GetClickedItem();
	DWORD dwCategory1Value = GetCategory1Value(pCategory1);
	DWORD dwCategory2Value = GetCategory2Value(pCategory2);
	msg.nCategory1 = (int)dwCategory1Value;
	msg.nCategory2 = (int)dwCategory2Value;

	msg.nMinLevel = atoi(m_pBuy_LevelMinEdit->GetEditText());
	msg.nMaxLevel = atoi(m_pBuy_LevelMaxEdit->GetEditText());
	
	msg.nRareLevel = m_pBuy_RareItemCombo->GetCurSelectedIdx() - 1;
	NETWORK->Send( &msg, sizeof(msg) ) ;

	SetHold(TRUE);

	m_dwLastSearchTime = gCurTime;
}

void CConsignmentDlg::SendSearchIndexMsg()
{
	MSG_CONSIGNMENT_SEARCHINDEX_REQUEST msg;
	ZeroMemory(&msg, sizeof(msg));
	msg.Category = MP_CONSIGNMENT;
	msg.Protocol = MP_CONSIGNMENT_SEARCHINDEX_SYN;
	msg.dwObjectID = HEROID;
	msg.wSortType = m_wSortType;
	msg.wStartPage = m_wCurSelPage;
	memcpy(msg.dwItemList, m_dwSearchItemList, sizeof(msg.dwItemList));

	NETWORK->Send( &msg, sizeof(msg) ) ;

	SetHold(TRUE);

	m_dwLastSearchTime = gCurTime;
}

void CConsignmentDlg::UpdateBuyIconList()
{
	if(m_nMode != eConsignment_Mode_Buy)
		return;

	for(int i=0; i<DISPLAYNUM_PER_PAGE; i++)
	{
		m_SearchIcon[i]->SetData(0);
		m_pBuy_ListItemDlg[i]->DeleteIcon(0, NULL);
	}

	int nCount = m_pBuy_ItemList->GetTopListItemIdx();
	int nMax = nCount + DISPLAYNUM_PER_PAGE;
	if(nMax > MAX_ITEMNUM_PER_PAGE)
		nMax = MAX_ITEMNUM_PER_PAGE;

	for(int i=nCount, nIconNum=0; i<nMax; i++, nIconNum++)
	{
		if( m_SearchIcon[nIconNum] )
		{
			ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( m_SearchList.ItemList[i].dwItemIndex );
			if( !pItemInfo )
				return;	

			cImage image;
			m_SearchIcon[nIconNum]->Init( 0, 0, DEFAULT_ICONSIZE, DEFAULT_ICONSIZE, ITEMMGR->GetIconImage( pItemInfo->ItemIdx, &image ), 0 );
			m_SearchIcon[nIconNum]->SetItemBaseInfo( m_SearchList.ItemInfo[i] );
			ITEMMGR->AddOption( m_SearchList.OptionInfo[i] );
			m_SearchIcon[nIconNum]->SetIconType( eIconType_AllItem );
			m_SearchIcon[nIconNum]->SetData( pItemInfo->ItemIdx );

			ITEMMGR->AddToolTip( m_SearchIcon[nIconNum] );
			m_pBuy_ListItemDlg[nIconNum]->AddIcon( 0, m_SearchIcon[nIconNum] );
		}
	}
}

void CConsignmentDlg::UpdateBuyList(int nNum, int nCurPage, int nTotalPage, MSG_CONSIGNMENT_SEARCH_RESULT* pSearchResult)
{
	m_pBuy_ItemList->RemoveAll();
	m_pBuy_ItemList_Lv->RemoveAll();
	m_pBuy_ItemList_Seller->RemoveAll();
	m_pBuy_ItemList_Price->RemoveAll();

	DWORD dwColor = RGBA_MAKE( 255, 255, 255, 0 );
	char text[256] = {0,};
	ITEM_INFO* pItemInfo;
	stConsignmentItemInfo* pItem;

	m_wCurSelPage = (WORD)nCurPage;
	m_wMaxSearchPage = (WORD)nTotalPage;
	{
		if(m_wCurSelPage < 1)
			m_wCurSelPage = 1;

		if(m_wMaxSearchPage < 1)
			m_wMaxSearchPage = 1;

		sprintf(text, "%d / %d", m_wCurSelPage, m_wMaxSearchPage);
		m_pBuy_PagePrint->SetStaticText(text);
	}

	for(int cnt=0; cnt<nNum; cnt++)
	{
		if( !pSearchResult->ItemList[cnt].dwConsignmentIdx )
			continue;

		pItemInfo = ITEMMGR->GetItemInfo( pSearchResult->ItemInfo[cnt].wIconIdx );
		if( !pItemInfo )
			continue;

		if( 0 == pSearchResult->ItemList[cnt].dwRemainMinute )
			continue;

		pItem = &pSearchResult->ItemList[cnt];
		pItem->dwItemDBIdx		= pSearchResult->ItemInfo[cnt].dwDBIdx;
		pItem->dwItemIndex		= pSearchResult->ItemInfo[cnt].wIconIdx;
		pItem->dwCategory1		= pItemInfo->dwType;
		pItem->dwCategory2		= pItemInfo->dwTypeDetail;
		pItem->wRareLevel		= (WORD)pItemInfo->kind;
		pItem->wUseLevel		= pItemInfo->LimitLevel;
		pItem->wEnchant			= pSearchResult->OptionInfo[cnt].mEnchant.mLevel;
		pItem->wInitDurability	= pSearchResult->ItemList[cnt].wInitDurability;
		pItem->dwDeposit		= pSearchResult->ItemList[cnt].dwDeposit;
		pItem->dwCommission		= pSearchResult->ItemList[cnt].dwCommission;
		pItem->dwUnitPrice		= pSearchResult->ItemList[cnt].dwUnitPrice;

		// 툴팁
		cListDialog::ToolTipTextList toolTipList;
		char line[MAX_PATH] = {0};
		sprintf(line, "%s %s", CHATMGR->GetChatMsg(2322), AddComma(pSearchResult->ItemList[cnt].dwAverageSellPrice));
		toolTipList.push_back(line);

		sprintf(line, "%s %s", CHATMGR->GetChatMsg(2323), AddComma(pSearchResult->ItemList[cnt].dwUnitPrice));
		toolTipList.push_back(line);
		/////////////////

		if(pItem->wEnchant > 0)
			sprintf(text, "+%d %s", pItem->wEnchant, pItemInfo->ItemName);
		else
			sprintf(text, "%s", pItemInfo->ItemName);

		DWORD dwNameColor = ITEMMGR->GetItemNameColor( *pItemInfo );
		m_pBuy_ItemList->AddItem(text, dwNameColor, toolTipList);

		sprintf(text, "%d", pItem->wUseLevel);
		m_pBuy_ItemList_Lv->AddItem(text, dwColor, toolTipList);
		sprintf(text, "%s", pSearchResult->ItemList[cnt].szPlayerName);
		m_pBuy_ItemList_Seller->AddItem(text, dwColor, toolTipList);
		sprintf(text, "%s", AddComma(pItem->dwUnitPrice * (pSearchResult->ItemInfo[cnt].Durability ? pSearchResult->ItemInfo[cnt].Durability : 1)));
		m_pBuy_ItemList_Price->AddItem(text, dwColor, toolTipList);
	}

	memcpy(&m_SearchList, pSearchResult, sizeof(MSG_CONSIGNMENT_SEARCH_RESULT));

	m_pBuy_ItemList->SetCurSelectedRowIdx(-1);
	m_pBuy_ItemList_Lv->SetCurSelectedRowIdx(-1);
	m_pBuy_ItemList_Seller->SetCurSelectedRowIdx(-1);
	m_pBuy_ItemList_Price->SetCurSelectedRowIdx(-1);

	UpdateBuyIconList();
}

BOOL CConsignmentDlg::CheckSearchTime()
{
	if(gCurTime < m_dwLastSearchTime + 1000)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2284) );
		return FALSE;
	}

	return TRUE;
}

void CConsignmentDlg::OnBuyItem( LONG iId, void* p, DWORD param1, void * vData1, void* vData2 )
{
	CConsignmentDlg* pDlg = (CConsignmentDlg*)vData1;

	char* pText = (char*)vData2;
	pDlg->m_dwBuyDurability = param1;

	pDlg->SetDisable(FALSE);

	WINDOWMGR->MsgBox( MBI_CONSIGNMENT_BUY_CONFIRM, MBT_YESNO, CHATMGR->GetChatMsg(2245), pText, pDlg->m_dwBuyDurability) ;
}

void CConsignmentDlg::OnCancelBuyItem( LONG iId, void* p, DWORD param1, void * vData1, void* vData2 )
{
	CConsignmentDlg* pDlg = (CConsignmentDlg*)vData1;

	pDlg->SetDisable(FALSE);
}

void CConsignmentDlg::SearchOnClient()
{
	const char* pKeyWord = m_pBuy_SearchEdit->GetEditText();
	if(strlen(pKeyWord) < 4)
	{
		m_pBuy_KeywordList->SetActive(FALSE);
		return;
	}

	m_pBuy_KeywordList->RemoveAll();
	m_pBuy_KeywordList->SetActive(TRUE);
	
	m_bOverCount = FALSE;
	ZeroMemory(m_dwSearchItemList, sizeof(m_dwSearchItemList));

	const char* pCategory1 = m_pBuy_Category1List->GetClickedItem();
	const char* pCategory2 = m_pBuy_Category2List->GetClickedItem();
	int nRareLevel = m_pBuy_RareItemCombo->GetCurSelectedIdx() - 1;
	WORD wMinLevel = (WORD)atoi(m_pBuy_LevelMinEdit->GetEditText());
	WORD wMaxLevel = (WORD)atoi(m_pBuy_LevelMaxEdit->GetEditText());

	WORD wSearchNum = ITEMMGR->GetItemList(pKeyWord, pCategory1, pCategory2, nRareLevel, wMinLevel, wMaxLevel, CONSIGNMENT_SEARCHINDEX_NUM, m_dwSearchItemList);

	if(0 == wSearchNum)
		m_pBuy_KeywordList->SetActive(FALSE);
	else if(wSearchNum >= 99)
	{
		m_pBuy_KeywordList->SetActive(FALSE);
	}
	else
	{
		m_pBuy_KeywordList->SetCurSelectedRowIdx(0);
	}

	if(wSearchNum > CONSIGNMENT_SEARCHINDEX_NUM)
	{
		m_bOverCount = TRUE;
		ZeroMemory(m_dwSearchItemList, sizeof(m_dwSearchItemList));
	}
}

void CConsignmentDlg::AddItemToKeywordList(const char* pItemName, DWORD dwColor)
{
	m_pBuy_KeywordList->AddItem(pItemName, dwColor);
}

void CConsignmentDlg::SetControlByItemIndex(char* pItemName)
{
	if(NULL == pItemName)
		return;

	if(strlen(pItemName) < 4)
		return;

	ITEM_INFO* pItemInfo = ITEMMGR->FindItemInfoForName(pItemName);

	if(NULL == pItemInfo)
		return;

	DWORD dwCategory1 = 0;
	DWORD dwCategory2 = 0;
	GetCategoryValueByItemIndex(pItemInfo->ItemIdx, dwCategory1, dwCategory2);

	if(0==dwCategory1 || 0==dwCategory2)
		return;

	ZeroMemory(m_dwSearchItemList, sizeof(m_dwSearchItemList));
	m_dwSearchItemList[0] = pItemInfo->ItemIdx;
	m_bOverCount = FALSE;
}

void CConsignmentDlg::ResetFilter()
{
	m_pBuy_SearchEdit->SetEditText("");
	m_pBuy_Category1List->SetClickedRowIdx(0);
	m_pBuy_Category1List->SetCurSelectedRowIdx(0);
	UpdateCategory2List();

	m_pBuy_RareItemCombo->SetCurSelectedIdx(0);
	m_pBuy_RareItemCombo->SelectComboText(0);

	m_pBuy_LevelMinEdit->SetEditText(SEARCH_MIN_LEVEL);
	m_pBuy_LevelMaxEdit->SetEditText(SEARCH_MAX_LEVEL);

	if(m_pBuy_KeywordList->IsShowScrool())
	{
		m_pBuy_Category1->SetActive(TRUE);
		m_pBuy_Category1List->SetActive(TRUE);
		m_pBuy_Category2->SetActive(TRUE);
		m_pBuy_Category2List->SetActive(TRUE);
		m_pBuy_RareItem->SetActive(TRUE);
		m_pBuy_RareItemCombo->SetActive(TRUE);
		m_pBuy_LevelMinBG->SetActive(TRUE);
		m_pBuy_LevelMinEdit->SetActive(TRUE);
		m_pBuy_LevelMaxBG->SetActive(TRUE);
		m_pBuy_LevelMaxEdit->SetActive(TRUE);
		m_pBuy_Text2->SetActive(TRUE);

		m_pBuy_KeywordList->SetActive(FALSE);
	}
	else if(m_pBuy_Category1List->IsShowScrool())
		m_pBuy_Category1List->SetShowScroll(FALSE);
	else if(m_pBuy_Category2List->IsShowScrool())
		m_pBuy_Category2List->SetShowScroll(FALSE);

	m_pBuy_SearchEdit->SetFocusEdit(TRUE);
}
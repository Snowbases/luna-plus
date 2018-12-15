// DealDialog.cpp: implementation of the CDealDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DealDialog.h"
#include "DealItem.h"
#include "ItemManager.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cIconGridDialog.h"
#include "./Interface/cStatic.h"
#include "./interface/cTabDialog.h"
#include "./interface/cPushupButton.h"
#include "GameIn.h"
#include "ObjectStateManager.h"
#include "cDivideBox.h"
#include "InventoryExDialog.h"
#include "mhFile.h"
#include "cMsgBox.h"
#include "ChatManager.h"
#include "MHMap.h"
#include "FishingManager.h"
#include "PCRoomManager.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void BuyItem(LONG lId, void * p, DWORD we);

CDealDialog::CDealDialog()
{
	m_type = WT_DEALDIALOG;
	m_pMoneyEdit = NULL;
	m_CurSelectedItemIdx = 0;
	m_lCurSelItemPos = -1;
	m_DealerTable.Initialize(32);			// #define MAX_DEALER_NUM 100
	m_fShow = FALSE;

	m_DealerIdx = 0;
}

CDealDialog::~CDealDialog()
{
	Release();
}


void CDealDialog::Add(cWindow * window)
{
	if(window->GetType() == WT_PUSHUPBUTTON)
		AddTabBtn(curIdx1++, (cPushupButton * )window);
	else if(window->GetType() == WT_ICONGRIDDIALOG)
		AddTabSheet(curIdx2++, window);
	else 
		cDialog::Add(window);
}

void CDealDialog::ShowDealer(WORD DealerKey)
{
	if(m_fShow) HideDealer();
	SelectTab(0);
	SetMoney(0, 2);

	DealerData * pDealer = m_DealerTable.GetData(DealerKey);
	if(pDealer == NULL)
	{
		//070126 LYW --- NPCShop : Modified message number.
		//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(197) );
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(338) );
		if( HERO->GetState() == eObjectState_Deal )
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		return;
	}

	SetActive(TRUE);
	
	for(BYTE i = 0; i < m_bTabNum; ++i )
	{
		cPushupButton* Btn = GetTabBtn(i);
		Btn->SetActive(FALSE);
	}

	CDealItem* pItem = NULL;
	cImage tmpImage;
	m_DealerIdx = DealerKey;
	DWORD DealIdx = 0;
	POS pos = pDealer->m_DealItemList.GetFirstPos();

	for(DealerItem* pDealItem = pDealer->m_DealItemList.GetNextPos(pos);
		0 < pDealItem;
		pDealItem = pDealer->m_DealItemList.GetNextPos(pos))
	{
		pItem = new CDealItem;
		pItem->Init(0,0,DEFAULT_ICONSIZE,DEFAULT_ICONSIZE, ITEMMGR->GetIconImage(pDealItem->ItemIdx, &tmpImage),IG_DEALITEM_START+DealIdx++);
		pItem->SetData(pDealItem->ItemIdx);
		pItem->SetItemIdx(pDealItem->ItemIdx);
		//pItem->SetMovable(FALSE);

		ITEMMGR->AddToolTip( pItem );
		
		char buf[MAX_PATH] = {0};
		pItem->AddToolTipLine("");

		if(pItem->GetBuyPrice())
		{
			wsprintf(
				buf,
				CHATMGR->GetChatMsg(35),
				AddComma(pItem->GetBuyPrice()));
			pItem->AddToolTipLine(
				buf,
				TTTC_BUYPRICE);
		}

		if(0 < pItem->GetBuyFishPoint())
		{
			wsprintf( buf, CHATMGR->GetChatMsg(1540), AddComma(pItem->GetBuyFishPoint()) );
			pItem->AddToolTipLine( buf, TTTC_BUYPRICE );
		}

		// 100104 ShinJS --- Item Point Type에 대한 툴팁 추가
		pItem->AddPointTypeToolTip();

		// 090114 LYW --- DealDialog : 아이템 착용 가능 체크 함수 수정에 따른 처리.
		if( ITEMMGR->CanEquip(pDealItem->ItemIdx) == eEquip_Disable )
			pItem->SetToolTipImageRGB( TTCLR_ITEM_CANNOTEQUIP ) ;

		WINDOWMGR->AddWindow(pItem);

		cIconGridDialog * dlg = (cIconGridDialog *)GetTabSheet(pDealItem->Tab);
		dlg->AddIcon(pDealItem->Pos, pItem);

		//SW050819 
		cPushupButton* Btn = GetTabBtn(pDealItem->Tab);
		if( !Btn->IsActive() )
		{
			Btn->SetActive(TRUE);
		}
	}
	GAMEIN->GetInventoryDialog()->SetActive(TRUE);	// taiyo : 유저가 불편할 수 있지 않은가?

	m_CurSelectedItemIdx	= 0;
	m_lCurSelItemPos		= -1;
	m_fShow = TRUE;

	ITEMMGR->SetPriceToItem( TRUE );
}

void CDealDialog::HideDealer()
{
	if(!m_fShow) return;

	
	cIcon* pIcon;

	for(BYTE tab=0;tab<m_bTabNum;++tab)
	{
		cIconGridDialog * dlg = (cIconGridDialog *)GetTabSheet(tab);

		for(WORD n=0;n<dlg->m_nCol*dlg->m_nRow;++n)
		{
			pIcon = dlg->GetIconForIdx(n);
			if(pIcon != NULL)
			{
				dlg->DeleteIcon(pIcon);
				//delete pIcon;
				pIcon->SetActive( FALSE );
				WINDOWMGR->AddListDestroyWindow( pIcon );
				pIcon = NULL;
			}
			
		}
	}
	m_DealerIdx = 0;

//KES 위에 WINDOMGR->AddListDestroyWindow( pIcon ) 로 대체.하였음
//process 중에 바로 delete했을시에 문제가 생김.
//	for(DWORD n=0;n<m_DealIdx;++n)
//	{
//		WINDOWMGR->DeleteWindowForID(IG_DEALITEM_START+n);
//	}

//죽은 상태에서 들어올 수도 있다. 체크하는것 추가 필요
	if( HERO->GetState() != eObjectState_Die )
	{
		if( HERO->GetState() == eObjectState_Deal  )
		OBJECTSTATEMGR->EndObjectState(HERO,eObjectState_Deal);
	}

	m_fShow = FALSE;

	ITEMMGR->SetPriceToItem( FALSE );
}


void CDealDialog::LoadDealerItem(CMHFile* fp)
{
	char buff[2048];
	char mapname[256];
	char npcname[256];
	DWORD ItemIdx;
	
	DealerItem* pItem;
	DealerData* pCurDealer = NULL;
	char seps[]   = "\t\n";
	char* token;
	int count = 0;
	while(1)
	{
		if(fp->IsEOF())
		{
			break;
		}
		count++;
		fp->GetWord(); // map index
		fp->GetString(mapname);
		fp->GetWord(); // npc kind
		fp->GetString(npcname);
		WORD npc_index = fp->GetWord();
		fp->GetWord(); // point x
		fp->GetWord(); // point y
		fp->GetWord(); // angle
		BYTE tabnum = fp->GetByte();
		BYTE Pos = 0;
		
		fp->GetLine(buff,2048);

		token = strtok( buff, seps );
		if(token == NULL)
			continue;
		token = strtok( NULL, seps );
		ItemIdx = atoi(token);

		pCurDealer = m_DealerTable.GetData(npc_index);
		if(pCurDealer == NULL)
		{
			pCurDealer = new DealerData;
			m_DealerTable.Add(pCurDealer,npc_index);
		}
		

		if( ItemIdx != 0 )
		{
			pItem = new DealerItem;
			ASSERT(tabnum)
			pItem->Tab = tabnum-1;
			pItem->Pos = Pos++;
			pItem->ItemIdx = ItemIdx;
			pCurDealer->m_DealItemList.AddTail(pItem);
		}
		else
		{
			Pos++;
		}

		while( 1 )
		{
			token = strtok( NULL, seps );
			if(token == NULL)
				break;
			token = strtok( NULL, seps );
			ItemIdx = atoi(token);
			
			if( ItemIdx != 0 )
			{
				pItem = new DealerItem;
				pItem->Tab = tabnum-1;
				pItem->Pos = Pos++;
				pItem->ItemIdx = ItemIdx;
				pCurDealer->m_DealItemList.AddTail(pItem);
			}
			else
			{
				Pos++;
			}
		}		
	}
}
void CDealDialog::Linking()
{
	m_pMoneyEdit = (cStatic *)GetWindowForID(DE_MONEYEDIT);
	m_pMoneyEdit->SetTextXY( 4, 4 );
	m_pMoneyEdit->SetAlign( TXT_RIGHT );

	// 100104 ShinJS --- PC방 포인트 출력 Static 추가
	m_pPointImage = (cStatic *)GetWindowForID(DE_POINTIMG);
	m_pPointImage->SetActive( FALSE );

	m_pPointEdit = (cStatic *)GetWindowForID(DE_POINTEDIT);
	m_pPointEdit->SetActive( FALSE );
}
void CDealDialog::Release()
{
	m_DealerTable.SetPositionHead();

	for(DealerData* pDealer = m_DealerTable.GetData();
		0 < pDealer;
		pDealer = m_DealerTable.GetData())
	{
		POS pos = pDealer->m_DealItemList.GetFirstPos();

		for(DealerItem* pDealerItem = pDealer->m_DealItemList.GetNextPos( pos );
			0 < pDealerItem;
			pDealerItem = pDealerItem = pDealer->m_DealItemList.GetNextPos( pos ))
		{
			delete pDealerItem;
		}

		pDealer->m_DealItemList.DeleteAll();
		delete pDealer;
	}
	m_DealerTable.RemoveAll();
}

void CDealDialog::OnSelectedItem()
{	
	cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum());
	CDealItem* pItem = (CDealItem*)gridDlg->GetIconForIdx((WORD)gridDlg->GetCurSelCellPos());
	if(!pItem)
	{
		SetMoney(0, 2);
		m_CurSelectedItemIdx	= 0;
		m_lCurSelItemPos		= -1;
		return;
	}

	m_CurSelectedItemIdx	= pItem->GetItemIdx();
	m_lCurSelItemPos		= gridDlg->GetCurSelCellPos();
	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(m_CurSelectedItemIdx);
	
	SetMoney(pItemInfo->BuyPrice, 1);
}
void CDealDialog::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}

void CDealDialog::SetMoney(DWORD value, BYTE colorType)
{
	if(colorType == 0)		//파는가격표시
		m_pMoneyEdit->SetFGColor(RGB_HALF(255,0,0));
	else if(colorType == 1)	//사는가격표시
		// 080401 LYW --- DealDialog : Change text color.
		//m_pMoneyEdit->SetFGColor(TTTC_BUYPRICE);
		m_pMoneyEdit->SetFGColor(RGB_HALF(10, 10, 10));
	else
		// 080401 LYW --- DealDialog : change text color.
		//m_pMoneyEdit->SetFGColor(RGB_HALF(255,255,255));
		m_pMoneyEdit->SetFGColor(RGB_HALF(255, 0, 0));

	m_pMoneyEdit->SetStaticText( AddComma( value ) );
}

void CDealDialog::SellItem(CItem* pItem)
{
	if(0 == pItem)
	{
		return;
	}
	else if(pItem->IsLocked())
	{
		return;
	}

	const ITEM_INFO* const pItemInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() );

	if(0 == pItemInfo)
	{
		return;
	}
	else if(FALSE == pItemInfo->Sell)
	{
		return;
	}

	SetDisable( TRUE );
	GAMEIN->GetInventoryDialog()->SetDisable( TRUE );

	if(pItemInfo->Stack && pItem->GetDurability() > 1)
	{
		m_sellMsg.Category = MP_ITEM;
		m_sellMsg.Protocol = MP_ITEM_SELL_SYN;
		m_sellMsg.dwObjectID = HEROID;
		m_sellMsg.TargetPos	= pItem->GetPosition();
		m_sellMsg.SellItemNum = WORD(pItem->GetDurability());
		m_sellMsg.wSellItemIdx = pItem->GetItemIdx();
		m_sellMsg.wDealerIdx = m_DealerIdx;

		cDivideBox * pDivideBox = WINDOWMGR->DivideBox(
			DBOX_SELL, 
			LONG(pItem->GetAbsX()),
			LONG(pItem->GetAbsY()),
			OnFakeSellItem,
			OnCancelSellItem,
			this,
			pItem,
			CHATMGR->GetChatMsg(26));

		pDivideBox->SetValue(
			pItem->GetDurability());
		pDivideBox->SetMaxValue(
			pItemInfo->Stack);
	}
	else
	{
		m_sellMsg.Category = MP_ITEM;
		m_sellMsg.Protocol = MP_ITEM_SELL_SYN;
		m_sellMsg.dwObjectID = HEROID;
		m_sellMsg.TargetPos	= pItem->GetPosition();
		m_sellMsg.SellItemNum = WORD(pItem->GetDurability());
		m_sellMsg.wSellItemIdx = pItem->GetItemIdx();
		m_sellMsg.wDealerIdx = m_DealerIdx;

		WINDOWMGR->MsgBox(
			MBI_SELLITEM,
			MBT_YESNO, 
			CHATMGR->GetChatMsg(339),
			pItemInfo->ItemName,
			AddComma(pItemInfo->SellPrice));
	}
}

BOOL CDealDialog::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	// 090106 LYW --- DealDialog : 창고 소환 아이템을 사용하여, 
	// 창고에서 아이템을 바로 판매 할수 있는 버그를 수정한다.


	// 함수인자 확인.
	if( !icon ) return FALSE ;


	// 아이템 타입인지 확인.
	if( icon->GetType() != WT_ITEM ) return FALSE ;


	// 아이템 정보를 받는다.
	CItem * pItem = ( CItem * )icon ;
	if( !pItem ) return FALSE ;

    ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( ((CItem*)icon)->GetItemIdx() ) ;	
	if( !pItemInfo ) return FALSE ;


	// 판매 가능 아이템인지 확인한다.
	if( !pItemInfo->Sell ) return FALSE ;
	
	const eITEMTABLE tableIdx = ITEMMGR->GetTableIdxForAbsPos(
		pItem->GetPosition());

	if( tableIdx != eItemTable_Inventory )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1809 ) ) ;
		return FALSE ;
	}


	// 판매 메시지를 설정한다.
	m_sellMsg.Category			= MP_ITEM ;
	m_sellMsg.Protocol			= MP_ITEM_SELL_SYN ;
	m_sellMsg.dwObjectID		= HEROID ;

	m_sellMsg.TargetPos			= pItem->GetPosition() ;
	m_sellMsg.SellItemNum		= (WORD)pItem->GetDurability() ;
	m_sellMsg.wSellItemIdx		= pItem->GetItemIdx() ;
	m_sellMsg.wDealerIdx		= m_DealerIdx ;


	// 스택아이템이고 하나 이상의 아이템을 판매 하려는 경우,
	if( ITEMMGR->IsDupItem( pItem->GetItemIdx() ) && pItem->GetDurability() > 1 )
	{
		// 판매 개수 입력 창을 활성화 한다.
		cDivideBox * pDivideBox = WINDOWMGR->DivideBox( DBOX_SELL, (LONG)pItem->GetAbsX(), (LONG)pItem->GetAbsY(),
			OnFakeSellItem, OnCancelSellItem, this, pItem, CHATMGR->GetChatMsg(26) ) ;
		pDivideBox->SetValue( m_sellMsg.SellItemNum ) ;

		// 091215 ONS 아이템Stack최대수를 아이템별로 지정된 값으로 처리한다.
		pDivideBox->SetMaxValue( pItemInfo->Stack ) ;
	}
	// 단일 아이템이라면, 판매 하겠는지 확인하는 메세지 창을 활성화 한다.
	else
	{	
		WINDOWMGR->MsgBox(
			MBI_SELLITEM,
			MBT_YESNO,
			CHATMGR->GetChatMsg(339),
			pItemInfo->ItemName,
			AddComma( pItemInfo->SellPrice ));
	}


	// 상점창을 Disable 시킨다.
	SetDisable( TRUE ) ;


	// 인벤토리를 Disable 시킨다.
	CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;
	if( pInvenDlg )
	{
		pInvenDlg->SetDisable( TRUE ) ;
	}

	return FALSE ;
}

void CDealDialog::OnFakeSellItem( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
	CDealDialog * tDlg = ( CDealDialog * )vData1;
	CItem* pItem = (CItem*)vData2;
	ASSERT( pItem );

	if( param1 == 0 ) 
	{
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Deal);
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
		return;
	}

	tDlg->m_sellMsg.SellItemNum = (WORD)((tDlg->m_sellMsg.SellItemNum > param1 ? param1 : tDlg->m_sellMsg.SellItemNum));

	WORD SellNum = tDlg->m_sellMsg.SellItemNum;
	if(! ITEMMGR->IsDupItem( tDlg->m_sellMsg.wSellItemIdx ))
		SellNum = 1;

	char temp[32] = {0};
	SafeStrCpy(temp, AddComma(pItem->GetItemInfo()->SellPrice), 32);
	WINDOWMGR->MsgBox(
		MBI_SELLITEM,
		MBT_YESNO,
		CHATMGR->GetChatMsg(340),
		pItem->GetItemInfo()->ItemName,
		SellNum,
		temp,
		AddComma(pItem->GetItemInfo()->SellPrice * SellNum));
}

void CDealDialog::OnCancelSellItem( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
	CDealDialog * tDlg = ( CDealDialog * )vData1;

	tDlg->SetDisable( FALSE );
	GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
}


void CDealDialog::SendSellItemMsg()
{
	const eITEMTABLE TargetTableIdx = ITEMMGR->GetTableIdxForAbsPos(m_sellMsg.TargetPos);
	CItem* pTargetItem = ITEMMGR->GetItemofTable(TargetTableIdx, m_sellMsg.TargetPos);

	if(!pTargetItem)
	{
		OnCancelSellItem(0, NULL, 0, this, NULL);
		return ;
	}

	if(m_sellMsg.wSellItemIdx != pTargetItem->GetItemIdx())
	{
		OnCancelSellItem(0, NULL, 0, this, NULL);
		return ;
	}
	if(m_sellMsg.SellItemNum > pTargetItem->GetDurability())
	{
		OnCancelSellItem(0, NULL, 0, this, NULL);
		return ;
	}

	NETWORK->Send( &m_sellMsg, sizeof(m_sellMsg) );
}

void CDealDialog::OnSellPushed()
{
	if(CItem* const pItem = GAMEIN->GetInventoryDialog()->GetCurSelectedItem(eItemTable_Inventory))
	{
		FakeMoveIcon(
			LONG(pItem->GetAbsX() + 20),
			LONG(pItem->GetAbsY() + 20),
			pItem);
	}
}

void CDealDialog::OnFakeBuyItem( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
/*	// 살수 있는지 체크 필
	tDlg->m_buyMsg.BuyItemNum	= param1;
	NETWORK->Send( &tDlg->m_buyMsg, sizeof(tDlg->m_buyMsg) );
*/
	if( param1 == 0 )
	{
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Deal);
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
		return;
	}

	CDealDialog * tDlg = ( CDealDialog * )vData1;
	CDealItem* pItem = (CDealItem*)vData2;
	ASSERT( pItem );

	tDlg->m_buyMsg.BuyItemNum	= (WORD)param1;

	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() );
	if( !pItemInfo )
		return;

	switch( pItemInfo->wPointType )
	{
		// PC방 포인트 구매
	case eItemPointType_PCRoomPoint:
		{
			DWORD dwNeedPoint = pItemInfo->dwPointTypeValue1 * tDlg->m_buyMsg.BuyItemNum;

			char totalPrice[32], totalPoint[32];
			SafeStrCpy( totalPrice, AddComma( pItem->GetBuyPrice() * tDlg->m_buyMsg.BuyItemNum ), 32 );
			SafeStrCpy( totalPoint, AddComma( dwNeedPoint ), 32 );

			WINDOWMGR->MsgBox(
				MBI_BUYITEM,
				MBT_YESNO,
				CHATMGR->GetChatMsg( 2015 ),
				pItem->GetItemName(),
				tDlg->m_buyMsg.BuyItemNum,
				AddComma(pItem->GetBuyPrice()),
				totalPrice,
				totalPoint );
		}
		break;
	default:
		{
			char temp[32] = {0};
			SafeStrCpy(temp, AddComma(pItem->GetBuyPrice()), 32);
			WINDOWMGR->MsgBox(
				MBI_BUYITEM,
				MBT_YESNO,
				CHATMGR->GetChatMsg(341),
				pItem->GetItemName(),
				tDlg->m_buyMsg.BuyItemNum,
				temp,
				AddComma(pItem->GetBuyPrice() * tDlg->m_buyMsg.BuyItemNum));
		}
		break;
	}

	
	
}

void CDealDialog::OnCancelBuyItem( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
	CDealDialog * tDlg = ( CDealDialog * )vData1;
	tDlg->SetDisable( FALSE );
	GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
}

void CDealDialog::OnBuyPushed()
{
	if(m_lCurSelItemPos == -1)
		return;
	
	cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum());
	CDealItem* pItem = (CDealItem*)gridDlg->GetIconForIdx((WORD)m_lCurSelItemPos);

	if( pItem )
	{
		FakeBuyItem( (LONG)(pItem->GetAbsX()+20), (LONG)(pItem->GetAbsY()+20), pItem);
	}
}


void CDealDialog::SendBuyItemMsg()
{
	// 100104 ShinJS --- Item Point Type 적용
	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo( m_buyMsg.wBuyItemIdx );
	if( !pItemInfo )
		return;
    
	switch( pItemInfo->wPointType )
	{
		// Coin 구매 Item
	case eItemPointType_Item:
		{
			// Coin 보유량이 구매량보다 부족한 경우
			DWORD dwCoinItemIdx = pItemInfo->dwPointTypeValue1;
			DWORD dwNeedCoinCnt = pItemInfo->dwPointTypeValue2;
			if( !CanBuyCoinItem( m_buyMsg.wBuyItemIdx, dwCoinItemIdx, dwNeedCoinCnt * m_buyMsg.BuyItemNum ) )
			{
				SetDisable( FALSE );
				GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
				return;
			}
		}
		break;

		// PC방 포인트 구매
	case eItemPointType_PCRoomPoint:
		{
			DWORD dwNeedPoint = pItemInfo->dwPointTypeValue1 * m_buyMsg.BuyItemNum;

			// 포인트 부족
			if( !PCROOMMGR->CanBuyItem( m_buyMsg.wBuyItemIdx, dwNeedPoint ) )
			{
				SetDisable( FALSE );
				GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
				return;
			}
		}
		break;
	}
	

	NETWORK->Send( &m_buyMsg, sizeof(m_buyMsg) );
}

void CDealDialog::CancelBuyItem()
{
	SetDisable( FALSE );
	GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
}

void CDealDialog::FakeBuyItem(LONG x, LONG y, CDealItem* pItem)
{
	DWORD itemIdx = pItem->GetItemIdx();
	if( itemIdx == 0 ) return;

	if( ITEMMGR->IsDupItem( itemIdx ) )
	{
		// 나누기 창 띄우기
		m_buyMsg.Category		= MP_ITEM;
		m_buyMsg.Protocol		= MP_ITEM_BUY_SYN;
		m_buyMsg.dwObjectID		= HEROID;
		m_buyMsg.wBuyItemIdx	= itemIdx;
		m_buyMsg.wDealerIdx		= m_DealerIdx;
		//m_buyMsg.BuyItemNum		= 0;

		cDivideBox * pDivideBox = WINDOWMGR->DivideBox( DBOX_BUY, x, y, OnFakeBuyItem, OnCancelBuyItem,
			//070126 LYW --- NPCShop : Modified message number.
														//this, pItem, CHATMGR->GetChatMsg(187) );
														this, pItem, CHATMGR->GetChatMsg(27) );
		if( !pDivideBox )
			return;

		pDivideBox->SetMaxValue( MAX_ITEMBUY_NUM );
//		pDivideBox->SetValue(1);

		SetDisable( TRUE );
		GAMEIN->GetInventoryDialog()->SetDisable( TRUE );

	}
	else
	{
		if( HERO->GetMoney() < pItem->GetBuyPrice() )
		{
			//돈이 부족합니다.
			//070126 LYW --- NPCShop : Modified message number.
			//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(117) );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(324) );
			return;			
		}

		if( 0<pItem->GetBuyFishPoint() && FISHINGMGR->GetFishPoint() < pItem->GetBuyFishPoint())
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1533 ) );
			return;			
		}

		// 100104 ShinJS --- Item Point Type 적용
		ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo( itemIdx );
		if( !pItemInfo )
			return;

		switch( pItemInfo->wPointType )
		{
			// Coin 구매
		case eItemPointType_Item:
			{
				// Coin 보유량이 구매량보다 부족한 경우
				DWORD dwCoinItemIdx = pItemInfo->dwPointTypeValue1;
				DWORD dwNeedCoinCnt = pItemInfo->dwPointTypeValue2;
				if( !CanBuyCoinItem( itemIdx, dwCoinItemIdx, dwNeedCoinCnt ) )
				{
					return;
				}
			}
			break;

			// PC방 포인트 구매
		case eItemPointType_PCRoomPoint:
			{
				DWORD dwNeedPoint = pItemInfo->dwPointTypeValue1;

				// 포인트 부족
				if( !PCROOMMGR->CanBuyItem( itemIdx, dwNeedPoint ) )
				{
					return;
				}
			}
			break;
		}
		
		SetDisable( TRUE );
		GAMEIN->GetInventoryDialog()->SetDisable( TRUE );

		m_buyMsg.Category		= MP_ITEM;
		m_buyMsg.Protocol		= MP_ITEM_BUY_SYN;
		m_buyMsg.dwObjectID		= HEROID;
		m_buyMsg.wBuyItemIdx	= itemIdx;
		m_buyMsg.wDealerIdx		= m_DealerIdx;
		m_buyMsg.BuyItemNum		= 1;

		const MONEYTYPE BuyPrice = pItem->GetBuyPrice();
		char buf[256] = {0,};
		DWORD dwBuyFishPoint = pItem->GetBuyFishPoint();
		if(0 < dwBuyFishPoint)
		{
			sprintf(buf, "%s %s", CHATMGR->GetChatMsg( 342 ), CHATMGR->GetChatMsg( 1534 ));

			char temp[32];
			SafeStrCpy(temp, AddComma(BuyPrice), 32 );

			WINDOWMGR->MsgBox( MBI_BUYITEM, MBT_YESNO, buf, 
				pItem->GetItemName(), temp, AddComma( dwBuyFishPoint ));
		}
		else
		{
			// 100105 ShinJS --- PointType 별 구매 Msg 변경
			switch( pItemInfo->wPointType )
			{
				// PC방 포인트 구매
			case eItemPointType_PCRoomPoint:
				{
					DWORD dwNeedPoint = pItemInfo->dwPointTypeValue1;

					char buf[MAX_PATH] = {0,};
					char temp[32];
					sprintf( buf, "%s %s", CHATMGR->GetChatMsg( 342 ), CHATMGR->GetChatMsg( 2014 ) );	// "[%s]^n%s원에 사시겠습니까? (PC방 포인트 %s 소모)"
					SafeStrCpy( temp, AddComma( dwNeedPoint ), 32 );

					WINDOWMGR->MsgBox( MBI_BUYITEM, MBT_YESNO, buf, 
						pItem->GetItemName(), AddComma( BuyPrice ), temp );
				}
				break;
			default:
				{
					WINDOWMGR->MsgBox( MBI_BUYITEM, MBT_YESNO, CHATMGR->GetChatMsg( 342 ), 
						pItem->GetItemName(), AddComma( BuyPrice ) );
				}
				break;
			}
			
		}

	}
}

void CDealDialog::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(val == FALSE)
	{
		HideDealer();
	}
	cTabDialog::SetActive(val);

	// 070326 LYW --- StorageDialog : Close inventory.
	if( !val )
	{
		CInventoryExDialog* pWindow = GAMEIN->GetInventoryDialog() ;

		if( pWindow->IsActive() )
		{
			VECTOR2* pPos = pWindow->GetPrevPos() ;
			pWindow->SetAbsXY( (LONG)pPos->x, (LONG)pPos->y ) ;

			pWindow->SetActive( FALSE ) ;
		}
	}

	SetActivePointInfo( FALSE, 0 );
}
void CDealDialog::OnActionEvnet(LONG lId, void * p, DWORD we)
{
	if(/*we == WE_RBTNCLICK ||*/ we == WE_LBTNDBLCLICK)
	{
		OnBuyPushed();
	}
}

// 070329 LYW --- DealDialog : Add function to setting positio.
void CDealDialog::ShowDealDialog( BOOL val )
{
	SetAbsXY( (LONG)m_relPos.x, (LONG)m_relPos.y ) ;

	SetActive( val ) ;

	CInventoryExDialog* pWindow = GAMEIN->GetInventoryDialog() ;

	VECTOR2 vPos = {0, } ;
	vPos.x = pWindow->GetAbsX() ;
	vPos.y = pWindow->GetAbsY() ;

	memcpy( pWindow->GetPrevPos(), &vPos, sizeof(VECTOR2) ) ;

	if( val )
	{
		pWindow->SetAbsXY((LONG)(m_relPos.x + 10) + GetWidth(), (LONG)m_relPos.y ) ;

		if( !pWindow->IsActive() )pWindow->SetActive( TRUE ) ;
	}
}

// 090227 ShinJS --- Coin Item의 구입가능여부 판단함수
BOOL CDealDialog::CanBuyCoinItem( DWORD dwBuyItemIdx, DWORD dwCoinType, DWORD dwCoinCnt ) const
{
	ITEM_INFO * pBuyItemInfo = ITEMMGR->GetItemInfo( dwBuyItemIdx );
	ITEM_INFO * pCoinItemInfo = ITEMMGR->GetItemInfo( dwCoinType );
	if( !pBuyItemInfo || !pCoinItemInfo )
		return FALSE;

	const POSTYPE inventoryStartPosition = TP_INVENTORY_START;
	const POSTYPE inventoryEndPosition = POSTYPE(TP_INVENTORY_END + TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount());
	CInventoryExDialog* inventory = ( CInventoryExDialog* )WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
	if( !inventory )		return FALSE;

	DWORD dwCoinCntOfInventory = 0;

	// 인벤토리를 검사하여 해당 Coin의 소지갯수를 구한다
	for( POSTYPE position = inventoryStartPosition ; inventoryEndPosition > position ; ++position )
	{
		CItem* item = inventory->GetItemForPos( position );
		if( !item )		continue;

		const DWORD	itemIndex	= item->GetItemIdx();

		// 해당 Coin인 경우
		if( itemIndex == dwCoinType )
		{
			const BOOL	isDuplicate = ITEMMGR->IsDupItem( itemIndex );
			const DWORD quantity	= ( isDuplicate ? item->GetDurability() : 1 );

			dwCoinCntOfInventory += quantity;
		}
	}

	// 코인이 부족한 경우
	if( dwCoinCntOfInventory < dwCoinCnt )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1908), pBuyItemInfo->ItemName, pCoinItemInfo->ItemName, dwCoinCnt - dwCoinCntOfInventory );
		return FALSE;
	}

	return TRUE;
}

void CDealDialog::SetActivePointInfo( BOOL bShow, DWORD dwPoint )
{
	m_pPointEdit->SetActive( bShow );
	m_pPointImage->SetActive( bShow );

	m_pPointEdit->SetStaticValue( LONG( dwPoint ) );
}

BOOL CDealDialog::IsActivePointInfo()
{
	return m_pPointEdit->IsActive();
}
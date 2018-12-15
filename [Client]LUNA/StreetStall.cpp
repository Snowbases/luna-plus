// StreetStall.cpp: implementation of the CStreetStall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StallKindSelectDlg.h"
#include "StreetStall.h"
#include "WindowIDEnum.h"
#include "ObjectManager.h"
#include "GameIn.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cSpin.h"
#include "./Interface\cIcon.h"
#include "./Interface\cIconGridDialog.h"
#include "./Interface\cEditBox.h"
#include "./Interface\cTextArea.h"
#include "./Interface\cButton.h"

#include "StreetStallManager.h"

#include "item.h"
#include "itemmanager.h"
#include "ExchangeItem.h"
#include "StreetStallTitleTip.h"
#include "cIMEex.h"
#include "cDivideBox.h"
#include "cMoneyDivideBox.h"
#include "ChatManager.h"
#include "./Input/Mouse.h"

// 061220 LYW --- Delete this code.
//#include "MainBarDialog.h"
#include "QuickManager.h"

// 071012 LYW --- StreetStall : Include static control.
#include "./interface/cStatic.h"

// 071012 LYW --- StreetStall : Include resource manager.
#include "../Interface/cResourceManager.h"

// 071125 LYW --- ExchangeDialog : 메시지 박스 헤더 포함.
#include "cMsgBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStreetStall::CStreetStall()
{
	m_type = WT_STREETSTALLDIALOG;
	m_DlgState = eSDS_NOT_OPENED;

	memset(m_MoneyArray, 0, sizeof(m_MoneyArray));
	m_nCurSelectedItem = WORD(-1);
	m_dwCurSelectedItemDBIdx = 0;
	m_pData = NULL;
}

CStreetStall::~CStreetStall()
{

}

void CStreetStall::Linking()
{
	// LBS 추가 03.09.18
	m_pStallGrid = (cIconGridDialog*)GetWindowForID(SSI_ICONGRID);

	m_pTitleEdit = (cEditBox *)GetWindowForID(SSI_TITLEEDIT);
	m_pTitleEdit->GetIME()->SetEnterAllow(FALSE);
	m_pTitleEdit->GetIME()->SetLimitLine(2);
//	m_pTitle	= (cTextArea*)GetWindowForID(SSI_TITLE);
//	m_pTitle->SetEnterAllow( FALSE );
//	m_pTitleEdit->SetEnterAllow( FALSE );
	m_pEnterBtn	= (cButton*)GetWindowForID(SSI_ENTER);
	m_pMoneyEdit = (cEditBox *)GetWindowForID(SSI_MONEYEDIT);
	m_pMoneyEdit->SetValidCheck( VCM_NUMBER );
	m_pMoneyEdit->SetAlign( TXT_RIGHT );
	m_pMoneyEdit->SetReadOnly( TRUE );
	m_pBuyBtn = (cButton *)GetWindowForID(SSI_BUYBTN);
//	m_pRegistBtn = (cButton *)GetWindowForID(SSI_REGISTBTN);
	m_pEditBtn = (cButton *)GetWindowForID(SSI_EDITBTN);

//	m_pTitle->SetScriptText( DEFAULT_TITLE_TEXT );
//	strcpy( m_OldTitle, m_pTitle->GetScriptText() );
//	m_pTitle->GetScriptText( m_OldTitle );

	m_pTitleEdit->SetEditText( DEFAULT_TITLE_TEXT );
	strcpy( m_OldTitle, m_pTitleEdit->GetEditText() );
	
	m_pMoneyEdit->SetEditText( DEFAULT_MONEY_TEXT );

	// 071012 LYW --- StreetStall : Add static control for owner name of stall dialog.
	m_pOwnerName = (cStatic*)GetWindowForID(SSI_OWNERTITLE) ;				// 상점명 입력, 오너 이름 출력용 스테틱을 받는다.
	m_pOwnerName->SetStaticText(RESRCMGR->GetMsg(TEXTNUM_OWNERTITLE)) ;
}

void CStreetStall::SetDisable( BOOL val )
{
	cDialog::SetDisable( val );

	if( m_DlgState == eSDS_BUY )
	{
		m_pEnterBtn->SetDisable( TRUE );
	}
}

void CStreetStall::OnCloseStall(BOOL bDelOption)
{
	STREETSTALLMGR->SetStallKind(eSK_NULL);

	SetDisable(FALSE);
	cDialog::SetActive(FALSE);

	m_pMoneyEdit->SetReadOnly( TRUE );
	m_pMoneyEdit->SetFocusEdit( FALSE );

	DeleteItemAll(bDelOption);

	// 각 변수 초기화
	m_pStallGrid->SetCurSelCellPos(-1);
	m_nCurSelectedItem = WORD(-1);
	m_dwCurSelectedItemDBIdx = 0;
	memset(m_MoneyArray, 0, SLOT_STREETSTALL_NUM*sizeof(DWORD));

	m_pMoneyEdit->SetEditText( DEFAULT_MONEY_TEXT );
	m_pTitleEdit->SetEditText( DEFAULT_TITLE_TEXT );
//	m_pTitle->SetScriptText( DEFAULT_TITLE_TEXT );


	m_pStallGrid->SetCurSelCellPos( -1 );
//	m_pStallGrid->SetShowGrid( FALSE );
	strcpy( m_OldTitle, m_pTitleEdit->GetEditText() );
//	m_pTitle->GetScriptText( m_OldTitle );

	m_DlgState = eSDS_NOT_OPENED;
	m_dwOwnnerId = gHeroID;

	if(m_pData)
	{
		CItem* item = (CItem*)m_pData;
		item->SetLock( FALSE );
	}
	m_pData = NULL;

	// 나누기 창이 등록 되어 있는경우 지운다.
	cDivideBox * pDivideBox = (cDivideBox *)WINDOWMGR->GetWindowForID( DBOX_DIVIDE_STREETSTALL );
	if( pDivideBox )
	{
		pDivideBox->SetDisable(TRUE);
		pDivideBox->SetActive(FALSE);
		WINDOWMGR->AddListDestroyWindow( pDivideBox );
	}

	cMoneyDivideBox *pMoneyDivideBox = (cMoneyDivideBox *)WINDOWMGR->GetWindowForID( DBOX_INPUT_MONEY );
	if( pMoneyDivideBox )
	{
		pMoneyDivideBox->SetDisable(TRUE);
		pMoneyDivideBox->SetActive(FALSE);
		WINDOWMGR->AddListDestroyWindow( pMoneyDivideBox );
	}
}

void CStreetStall::ShowSellStall()
{
	SetActive(TRUE);

	m_DlgState = eSDS_OPENED;

	m_pBuyBtn->SetActive(FALSE);
	m_pEditBtn->SetActive(TRUE);
//	m_pRegistBtn->SetActive(TRUE);

	m_pTitleEdit->SetReadOnly(FALSE);
//	m_pTitle->SetReadOnly(FALSE);

	m_pEnterBtn->SetDisable(FALSE);
}

void CStreetStall::ShowBuyStall()
{
	SetActive(TRUE);

	m_DlgState = eSDS_BUY;

	m_pBuyBtn->SetActive(TRUE);
	m_pEditBtn->SetActive(FALSE);
//	m_pRegistBtn->SetActive(FALSE);

	m_pTitleEdit->SetReadOnly(TRUE);
//	m_pTitle->SetFocusEdit( FALSE );
//	m_pTitle->SetReadOnly(TRUE);
	m_pEnterBtn->SetDisable( TRUE );

	//m_pMoneyEdit->SetReadOnly(TRUE);
	//m_pMoneyEdit->SetFocusEdit( FALSE );

	ResetDlgData();

	return;
}

int CStreetStall::AddItem( cIcon* pItem )
{
	for(POSTYPE i=0;i<m_pStallGrid->GetCellNum();++i)
	{
		if( !m_pStallGrid->m_pIconGridCell[i].use )
			break;
	}

	POSTYPE pos = i;
	if( pos == m_pStallGrid->GetCellNum() ) return -1;

	m_pStallGrid->AddIcon( pos, pItem );

	return pos;
}

cIcon* CStreetStall::GetItem( DWORD dwDBIdx )
{
	CExchangeItem* pItem;
	for(int i=0;i<m_pStallGrid->GetCellNum();++i)
	{
		pItem = ( CExchangeItem* )m_pStallGrid->m_pIconGridCell[i].icon;
		if(pItem == NULL)
			return NULL; //하지도 않은 노점 메시지가 날라와서 뻑나게 되서 추가 hs 
		if( pItem->GetDBId() == dwDBIdx )
			return pItem;
	}

	return NULL;
}


void CStreetStall::DeleteItem( ITEMBASE* pbase )
{
	CExchangeItem* pItem;
	POSTYPE pos;

	for(POSTYPE i=0;i<m_pStallGrid->GetCellNum();++i)
	{
		pItem = (CExchangeItem*)m_pStallGrid->m_pIconGridCell[i].icon;

		if( pItem == NULL ) 
		{
//			ASSERTMSG(0, "서버에 있는 정보가 클라이언트에는 없음..");
			continue;
		}

		if( pbase->dwDBIdx == pItem->GetDBId() )
			break;
	}
	
	pos = i;
	
	if( pos == m_pStallGrid->GetCellNum() ) return;

	cIcon* pIcon = NULL;
	if( m_pStallGrid->DeleteIcon( pos, &pIcon ) )
	{
		if( m_pData && pIcon )
		{
			if( ((CItem*)m_pData)->GetItemBaseInfo().dwDBIdx == ((CExchangeItem*)pIcon)->GetDBId() )
			{
				m_pData = NULL;
				m_dwCurSelectedItemDBIdx = 0;
			}
		}

		if( m_DlgState == eSDS_BUY )
		{
			if( pIcon ) WINDOWMGR->AddListDestroyWindow( pIcon );
		}
		else
		{
			if( pIcon ) STREETSTALLMGR->UnlinkItem( (CExchangeItem*)pIcon );
		}

		m_pStallGrid->DeleteIcon( pos, &pIcon );

		for( POSTYPE i = pos ; i < SLOT_STREETSTALL_NUM-1 ; ++i )
		{
			m_pStallGrid->DeleteIcon( i+1, &pIcon );
			if( !pIcon ) break;

			((CExchangeItem*)pIcon)->SetPosition( i );

			if( ((CExchangeItem*)pIcon)->GetLinkItem() )	//사는 사람은 링크가 없다.
				((CExchangeItem*)pIcon)->GetLinkItem()->SetLinkPosition( i );

			m_pStallGrid->AddIcon( i, pIcon );
			
			m_MoneyArray[i] = m_MoneyArray[i+1];
		}
		m_MoneyArray[i] = 0;
	}

	ResetDlgData();
}


CExchangeItem* CStreetStall::FindItem( DWORD dwDBIdx )
{
	CExchangeItem* pItem = NULL;
	CExchangeItem* pFindItem = NULL;

	for( int i = 0 ; i < m_pStallGrid->GetCellNum() ; ++i )
	{
		pItem = (CExchangeItem*)m_pStallGrid->m_pIconGridCell[i].icon;

		if( pItem == NULL )
			break;

		if( dwDBIdx == pItem->GetDBId() )
		{
			pFindItem = pItem;
			break;
		}
	}
	
	return pFindItem;	
}

//아이템의 갯수가 바뀔경우( 상대방이 아이템 구입해감 )
void CStreetStall::ResetItemInfo( DWORD dwDBIdx, DWORD count )
{
	for(int i=0;i<m_pStallGrid->GetCellNum();++i)
	{
		CExchangeItem* pItem = (CExchangeItem*)m_pStallGrid->m_pIconGridCell[i].icon;
		
		if( pItem == NULL ) 
		{
			ASSERTMSG(0, "서버에 있는 정보가 클라이언트에는 없음..");
			continue;
		}
		//100603 갯수가 바뀐 아이템의 개수를 갱신한다.
		if( dwDBIdx == pItem->GetDBId() )
		{
			pItem->SetDurability( count );
			break;
		}
	}
}


void CStreetStall::DeleteItemAll(BOOL bDelOption)
{
	cIcon* pIcon;

	for( POSTYPE i=0;i<SLOT_STREETSTALL_NUM;++i)
	{
		m_pStallGrid->DeleteIcon( i, &pIcon );
		
		if( bDelOption && pIcon )
		{
			CItem* pItem = ((CExchangeItem*)pIcon)->GetLinkItem();

			if( pItem &&
				!( pItem->GetItemInfo()->Shop ) )				
			{
				ITEMMGR->RemoveOption( ((CExchangeItem*)pIcon)->GetDBId() );			
			}
		}

		if( m_DlgState == eSDS_BUY )
		{
			if( pIcon ) WINDOWMGR->AddListDestroyWindow( pIcon );
		}
		else
		{
			if( pIcon ) STREETSTALLMGR->UnlinkItem( (CExchangeItem*)pIcon );
		}
	}
}

BOOL CStreetStall::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	//if( m_bDisable == TRUE || m_pData ) return FALSE;
	if( m_bDisable ) return FALSE ;
	if( GetDlgState() != eSDS_OPENED ) return FALSE;


    ////////////////////////////////////////////////////////////////////////////////////////////
	//// 아이템에대한 유효성 체크
	if( icon->GetType() != WT_ITEM )
		return FALSE;

	// 090105 ShinJS --- ITEM_INFO의 NULL 체크 추가
	if( ((CItem*)icon)->GetItemInfo() == NULL )
		return FALSE;

	if( ( GetTableIdxPosition( ((CItem*)icon)->GetPosition() ) != eItemTable_Inventory ))
	{
		if( GetTableIdxPosition( ((CItem*)icon)->GetPosition() ) == eItemTable_Weared )
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(479) );

		return FALSE;
	}

	if( !((CItem*)icon)->GetItemInfo()->Sell )
	{
		return FALSE;
	}

	// 퀵다이얼로그에 링크가 있는지 체크한다.
	if( ((CItem*)icon)->GetItemInfo()->Category == eItemCategory_Expend )
	{
		if( QUICKMGR->CheckQPosForItemIdx(((CItem*)icon)->GetItemIdx()) == FALSE )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(62) );
			return FALSE;
		}
	}
	else
	{
		if( QUICKMGR->CheckQPosForDBIdx(((CItem*)icon)->GetDBIdx()) == FALSE )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(62) );
			return FALSE;
		}
	}

	// 링크아이템인지 확인한다.
	if( icon->GetIconType() == eIconType_LinkedItem ) return FALSE;

	if( !m_pStallGrid->PtInWindow( x, y ) ) return FALSE;
	////////////////////////////////////////////////////////////////////////////////////////////

	CItem* item = (CItem*)icon;
	if( item->GetLinkPosition() ) return FALSE; // 아이템의 링크상태 확인
	item->SetLock( TRUE );
	
//	if( m_pData ) // 등록 중인 기존 아이템에 대해서 LOCK을 푼다.
//	{
//		CItem* pCurItem = (CItem*)m_pData;
//		pCurItem->SetLock(FALSE);
//	}
	m_pData = item;

	if( STREETSTALLMGR->GetLinkedItemNum() >= SLOT_STREETSTALL_NUM )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(469) );
		item->SetLock( FALSE );
		m_pData = NULL;
	}
	else
	{
		// 071125 LYW --- StreetStallManager : 봉인 해제 된 아이템 판매 제한 처리 추가.
		const ITEMBASE& itemBase = item->GetItemBaseInfo() ;

		if( itemBase.nSealed == eITEM_TYPE_UNSEAL )
		{
			WINDOWMGR->MsgBox(MBI_SEALITEM_STREETSTALL, MBT_OK, CHATMGR->GetChatMsg(1175));
			item->SetLock( FALSE );

			return FALSE ;
		}

		// 아이템을 가격과 함께 등록 한다.
		STREETSTALLMGR->RegistItemEx( item, 0 );
		m_dwCurSelectedItemDBIdx = item->GetDBIdx();
		SetDisable( TRUE );
		
//		pStall->SetData( NULL );
//		pStall->SetDisable( FALSE );
//		ShowDivideBox(eSO_INPUTMONEY, x, y);
	}

	return FALSE;
}

void CStreetStall::FakeDeleteItem( POSTYPE pos )
{
	CExchangeItem* pExItem = (CExchangeItem*)m_pStallGrid->GetIconForIdx( pos );
	if( pExItem == NULL ) return;
	if( pExItem->GetLinkItem() == NULL ) return;

	MSG_LINKITEM msg;
	msg.Category	= MP_STREETSTALL;
	msg.Protocol	= MP_STREETSTALL_DELETEITEM_SYN;
	msg.dwObjectID	= gHeroID;
	msg.ItemInfo	= pExItem->GetLinkItem()->GetItemBaseInfo();
	msg.wAbsPosition = pos;

	NETWORK->Send( &msg, sizeof( MSG_LINKITEM ) );
}

// LBS 추가  03.09.22
DWORD CStreetStall::ActionEventWindow(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;

	if( m_bDisable ) return we;
	we = cDialog::ActionEventWindow( mouseInfo );

	if( mouseInfo->LButtonDown() )
	if( !PtInWindow( mouseInfo->GetMouseX(), mouseInfo->GetMouseY() ) )
	{
		m_pStallGrid->SetShowGrid( FALSE );
//		m_pStallGrid->SetCurSelCellPos(-1);
//		m_nCurSelectedItem = -1;
//		m_dwCurSelectedItemDBIdx = 0;
	}

	return we;
}

// LBS 추가 03.09.18
void CStreetStall::OnActionEvnet(LONG lId, void * p, DWORD we)
{
	if( we == WE_LBTNCLICK || we == WE_LBTNDBLCLICK )
	{
		POSTYPE pos = (POSTYPE)m_pStallGrid->GetCurSelCellPos();
		CExchangeItem * pItem = (CExchangeItem*)m_pStallGrid->GetIconForIdx( pos );

		if( !pItem ) // 선택된 곳에 아이템이 없는경우
		{
			m_nCurSelectedItem = WORD(-1);
			m_dwCurSelectedItemDBIdx = 0;
			return;
		}
		else
		{
			m_nCurSelectedItem = pos;
			m_dwCurSelectedItemDBIdx = GetCurSelectedItemDBidx();

//			m_pMoneyEdit->SetEditText( DEFAULT_MONEY_TEXT ); 

			if( m_DlgState == eSDS_BUY )
			{
				if( !pItem->IsLocked() && we == WE_LBTNDBLCLICK ) // DoubleClick으로 물건 구입
				{
					// 070430 LYW --- StreetStall : IsDupItem.
					//if( ITEMMGR->IsDupItem( (WORD)pItem->GetData() ) )
					if( ITEMMGR->IsDupItem( pItem->GetData() ) )
					{
						if( pItem->GetDurability() == 1)
							STREETSTALLMGR->BuyItem();
						else
							ShowDivideBox();
					}
					else
					{
						STREETSTALLMGR->BuyItem();
					}
				}
			}

			m_pStallGrid->SetShowGrid( TRUE );
		}
	}
	else if( we & WE_BTNCLICK && lId == SSI_ENTER )
	{
		STREETSTALLMGR->EditTitle();
	}
	else if( we & WE_BTNCLICK && lId == SSI_CLOSEBTN )
	{
		SetActive( FALSE );	
	}
}

void CStreetStall::SetActive( BOOL val )
{
	if( m_bDisable )
	{
		return;
	}

	if( val )
	{
		// 강화/인챈트/조합/분해/노점판매 창이 동시에 뜨는 것을 막는다
		cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
		cDialog* DissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
		cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
		cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
		
		ASSERT( mixDialog && DissoloveDialog && enchantDialog && reinforceDialog );

		mixDialog->SetActive( FALSE );
		DissoloveDialog->SetActive( FALSE );
		enchantDialog->SetActive( FALSE );
		reinforceDialog->SetActive( FALSE );
	}

	if( m_bActive != val )
	{
		if(!val) STREETSTALLMGR->CloseStreetStall();

		cDialog::SetActiveRecursive( val );
	}
}

void CStreetStall::RegistMoney()
{
	//m_MoneyArray[m_nCurSelectedItem] = (DWORD)atoi( RemoveComma( m_pMoneyEdit->GetEditText() ));
	if( _atoi64( RemoveComma( m_pMoneyEdit->GetEditText() ) ) > MAX_INVENTORY_MONEY )
		m_MoneyArray[m_nCurSelectedItem] = MAX_INVENTORY_MONEY;

	
}

void CStreetStall::RegistMoney( POSTYPE pos, DWORD dwMoney )
{
	m_MoneyArray[pos] = dwMoney;
}

void CStreetStall::RegistTitle( char* title, BOOL bSucess )
{
	if( bSucess )
	{
		HERO->ShowStreetStallTitle( TRUE, title );
		strcpy( m_OldTitle, title );
	}
	else
	{
		m_pTitleEdit->SetEditText( m_OldTitle );
//		m_pTitle->SetScriptText( m_OldTitle );
	}
}

void CStreetStall::ChangeItemStatus( POSTYPE pos, DWORD money, BOOL bLock )
{
	CExchangeItem * pItem = (CExchangeItem*)m_pStallGrid->GetIconForIdx( pos );
	if( !pItem ) return;
	char buf[128] = {0,};
	char buf2[128] = {0,};

	m_MoneyArray[pos] = money;

	if( m_DlgState == eSDS_BUY )
	{
		pItem->SetLock( !bLock );
		pItem->SetMovable( FALSE );
//		m_pMoneyEdit->SetReadOnly( TRUE );
//		m_pMoneyEdit->SetFocusEdit( FALSE );
	}
	else
	{
		pItem->SetLock( bLock );
		pItem->SetMovable( !bLock );
//		m_pMoneyEdit->SetReadOnly( bLock );
	}

	ITEMMGR->AddToolTip( pItem );

	if( bLock )
	{
		// 070430 LYW --- StreetStall : IsDupItem.
		//if( ITEMMGR->IsDupItem((WORD)pItem->GetData()) )
		if( ITEMMGR->IsDupItem(pItem->GetData()) )
		{
			wsprintf( buf, CHATMGR->GetChatMsg(30), AddComma( money ) );
			wsprintf( buf2, CHATMGR->GetChatMsg(504), AddComma( money * pItem->GetDurability() ));
		}
		else
			wsprintf( buf, CHATMGR->GetChatMsg(35), AddComma( money ) );
	}
	else
	{
		wsprintf( buf, CHATMGR->GetChatMsg(503) );
	}

	pItem->AddToolTipLine( "" );

	DWORD color = 0xffffffff;

	// 091112 ONS 금액표시 색상 설정
	color = GetMoneyColor(money);


	pItem->AddToolTipLine( buf, color );

	if(buf2[0])
	{
		DWORD color = 0xffffffff;
		money = money * pItem->GetDurability();

		// 091112 ONS 금액표시 색상 설정
		color = GetMoneyColor(money);

		pItem->AddToolTipLine( buf2, color );
	}

//	if( bLock )
//		m_pMoneyEdit->SetFocusEdit( FALSE );

	// 가격표에 대한 처리 
	ResetDlgData();
	 
	// 100603 아이템에 대한 Lock을 풀어준다.
	pItem->SetLock( FALSE );
	pItem->SetMovable( FALSE );
}

void CStreetStall::ChangeItemStatus( ITEMBASE* pbase, DWORD money, BOOL bLock, int nDivideKind )
{
	//090212 NYJ - 구입직전에 아이템가격이 변동되면 구입창을 닫아버린다.
	cMsgBox* pBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_STREETBUYMSG );
	if(pBox)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1812) );
		pBox->ForceClose();
	}

	CExchangeItem* pItem = NULL;
	POSTYPE pos;
	char buf[64] = {0,};
	char buf2[64] = {0,};

	for(POSTYPE i=0;i<m_pStallGrid->GetCellNum();++i)
	{
		if( m_pStallGrid->m_pIconGridCell[i].use )
		{
			pItem = (CExchangeItem*)m_pStallGrid->m_pIconGridCell[i].icon;
			if( pItem->GetDBId() == pbase->dwDBIdx )
				break;
		}
	}

	pos = i;
	if( pos == m_pStallGrid->GetCellNum() ) return;

	m_nCurSelectedItem = pos;
	m_pStallGrid->SetCurSelCellPos( pos );

	m_MoneyArray[pos] = money;

	if( m_DlgState == eSDS_BUY )
	{
		pItem->SetLock( !bLock );
		pItem->SetMovable( FALSE );
//		m_pMoneyEdit->SetReadOnly( TRUE );
//		m_pMoneyEdit->SetFocusEdit( FALSE );
	}
	else
	{
		if( !bLock )
		{
			m_pData = pItem->GetLinkItem();
			if( ITEMMGR->IsDupItem( pItem->GetLinkItem()->GetItemIdx() ) )
				ShowDivideBox( eSO_INPUTMONEY_DUP, (int)(pItem->GetAbsX() + 20), (int)(pItem->GetAbsY() + 20), nDivideKind );
			else
				ShowDivideBox( eSO_INPUTMONEY, (int)(pItem->GetAbsX() + 20), (int)(pItem->GetAbsY() + 20), nDivideKind );
		
			pItem->SetLock( TRUE );
			pItem->SetMovable( FALSE );
		}
		else
		{
			pItem->SetLock( FALSE );
			pItem->SetMovable( FALSE );
			m_pData = NULL;
		}
//		pItem->SetLock( bLock );
//		pItem->SetMovable( !bLock );
//		m_pMoneyEdit->SetReadOnly( bLock );
	}

	ITEMMGR->AddToolTip( pItem );

	if( bLock )
	{
		// 070430 LYW --- StreetStall : IsDupItem.
		//if( ITEMMGR->IsDupItem((WORD)pItem->GetData()) )
		if( ITEMMGR->IsDupItem(pItem->GetData()) )
		{
			wsprintf( buf, CHATMGR->GetChatMsg(30), AddComma( money ) );
			wsprintf( buf2, CHATMGR->GetChatMsg(504), AddComma( money * pItem->GetDurability() ));
		}
		else
			wsprintf( buf, CHATMGR->GetChatMsg(35), AddComma( money ) );
	}
	else
	{
		wsprintf( buf, CHATMGR->GetChatMsg(503) );
	}
			
	pItem->AddToolTipLine( "" );
	
	DWORD color = 0xffffffff;

	// 091112 ONS 금액표시 색상 설정
	color = GetMoneyColor(money);

	pItem->AddToolTipLine( buf, color );

	if(buf2[0])
	{
		DWORD color = 0xffffffff;
		money = money * pItem->GetDurability();

		// 091112 ONS 금액표시 색상 설정
		color = GetMoneyColor(money);

		pItem->AddToolTipLine( buf2, color );
	}
	
	//	if( bLock )
//		m_pMoneyEdit->SetFocusEdit( FALSE );

	// 가격표에 대한 처리 
	ResetDlgData();

	// 100603 아이템에 대한 Lock을 풀어준다.
	pItem->SetLock( FALSE );
	pItem->SetMovable( FALSE );
}

// Lock상태와 겹치기아이템인지를 확인할 수 있다.
BOOL CStreetStall::SelectedItemCheck()
{
	if(cIcon* const icon = m_pStallGrid->GetIconForIdx(m_nCurSelectedItem))
	{
		return FALSE == icon->IsLocked();
	}

	return FALSE;
}

void CStreetStall::ResetDlgData()
{
	m_pStallGrid->SetCurSelCellPos( m_nCurSelectedItem );
	m_pStallGrid->SetShowGrid( TRUE );
	
	m_TotalMoney = 0;
	
	for(int i=0;i<m_pStallGrid->GetCellNum();++i)
	{
		CExchangeItem* pItem = (CExchangeItem*)m_pStallGrid->m_pIconGridCell[i].icon;
		DWORD Durability;
		
		if(pItem == NULL)
			continue;

		// 070430 LYW --- StreetStall : IsDupItem.
		//if( ITEMMGR->IsDupItem((WORD)pItem->GetData()) )
		if( ITEMMGR->IsDupItem(pItem->GetData()) )
			Durability = pItem->GetDurability();
		else
			Durability = 1;
		
		if(Durability == 0)
			Durability = 1;
		
		m_TotalMoney += (m_MoneyArray[i] * Durability);
	}
		
	DWORD color = TTTC_ONDERMILLION;
	DWORD money = m_TotalMoney;

	// 091112 ONS 금액표시 색상 설정
	color = GetMoneyColor(money);


	m_pMoneyEdit->SetNonactiveTextColor( color );
	m_pMoneyEdit->SetEditText( AddComma( m_TotalMoney ) ); 
}

DWORD CStreetStall::GetCurSelectedItemDBidx()
{
	return ((CExchangeItem*)m_pStallGrid->GetIconForIdx(m_nCurSelectedItem))->GetDBId();
}

DWORD CStreetStall::GetCurSelectedItemDur()
{
	return ((CExchangeItem*)m_pStallGrid->GetIconForIdx(m_nCurSelectedItem))->GetDurability();
}


void CStreetStall::ShowDivideBox(DWORD dwOption, int x, int y, int nDivideKind )
{
	switch( dwOption )
	{
	case eSO_DIVE:
		{
			CExchangeItem* pExItem = (CExchangeItem*)m_pStallGrid->GetIconForIdx(m_nCurSelectedItem);

			if( pExItem == NULL ) return;

			cDivideBox * pDivideBox = WINDOWMGR->DivideBox( DBOX_DIVIDE_STREETSTALL, (LONG)pExItem->GetAbsX(), (LONG)pExItem->GetAbsY(), OnDivideItem, OnDivideItemCancel, this, NULL, CHATMGR->GetChatMsg(27) );
			if( !pDivideBox )
			{
				pDivideBox = (cDivideBox *)WINDOWMGR->GetWindowForID( DBOX_DIVIDE_STREETSTALL );
			}
			pDivideBox->ChangeKind( nDivideKind );
			
			// 091215 ONS 아이템Stack최대수를 아이템별로 지정된 값으로 처리한다.
			const ITEM_INFO* pInfo = ITEMMGR->GetItemInfo(pExItem->GetData()) ;            
			if( NULL == pInfo ) return;
			pDivideBox->SetMaxValue( pInfo->Stack );
			//pDivideBox->SetValue( pExItem->GetDurability() );
			pDivideBox->SetValue(1);	//KES
//SS_BUG
//			pDivideBox->SetPos( m_nCurSelectedItem );

			m_pData = pExItem;

			pDivideBox->m_pSpin->SetFocusEdit(TRUE);
			//SetDisable(TRUE);
		}
		break;
	case eSO_INPUTMONEY:
		{
			cMoneyDivideBox * pMoneyDivideBox = WINDOWMGR->MoneyDivideBox( DBOX_INPUT_MONEY, x, y, OnFakeRegistItem, OnRegistItemCancel, this, NULL, CHATMGR->GetChatMsg(540) );
			if( !pMoneyDivideBox )
				pMoneyDivideBox = (cMoneyDivideBox *)WINDOWMGR->GetWindowForID( DBOX_INPUT_MONEY );

			pMoneyDivideBox->ChangeKind( nDivideKind );
			//pDivideBox->SetValue(0);
			//pDivideBox->m_pSpin->SetEditText("0");
			pMoneyDivideBox->m_pSpin->SetFocusEdit( TRUE );
			pMoneyDivideBox->m_pSpin->SetEditText( AddComma( m_MoneyArray[m_nCurSelectedItem] ) );
			SetDisable( TRUE );
		}
		break;
	case eSO_INPUTMONEY_DUP:
		{
			cMoneyDivideBox * pMoneyDivideBox = WINDOWMGR->MoneyDivideBox( DBOX_INPUT_MONEY, x, y, OnFakeRegistItem, OnRegistItemCancel, this, NULL, CHATMGR->GetChatMsg(541) );
			if( !pMoneyDivideBox )
				pMoneyDivideBox = (cMoneyDivideBox *)WINDOWMGR->GetWindowForID( DBOX_INPUT_MONEY );
			pMoneyDivideBox->ChangeKind( nDivideKind );
			//pDivideBox->SetValue(0);
			//pDivideBox->m_pSpin->SetEditText("0");
			pMoneyDivideBox->m_pSpin->SetFocusEdit( TRUE );
			pMoneyDivideBox->m_pSpin->SetEditText( AddComma( m_MoneyArray[m_nCurSelectedItem] ) );
			SetDisable( TRUE );
		}
		break;
	}
}

void CStreetStall::OnDivideItem( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{

	CStreetStall* pStall = (CStreetStall*)vData1;

	// 변수의 초기화
	if( param1 == 0 )
	{
		pStall->SetDisable(FALSE);
		return; // 갯수가 0이면 
	}

	cDivideBox* pDivideBox = (cDivideBox*)p;

	if( pDivideBox == NULL )
	{
		pStall->SetDisable(FALSE);
		return;
	}

	CExchangeItem* pExItem = pStall->FindItem( pStall->GetCheckDBIdx() );
	if( pExItem == NULL )
	{
		OnDivideItemCancel( iId, p, param1, vData1, vData2 );
		//메세지 표시해주기
		return;		
	}

	if( pExItem->GetDurability() < param1 )
		param1 = pExItem->GetDurability();

	POSTYPE pos = pExItem->GetPosition();

// 매니져에 영약 아이템 사기 호출 ..
	STREETSTALLMGR->BuyItemDur( pos, param1 );

//	pStall->SetData( NULL );
//	pStall->SetDisable( FALSE );
}

void CStreetStall::OnDivideItemCancel( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
	CStreetStall* pStall = (CStreetStall*)vData1;
	if( pStall == NULL ) return;

	pStall->SetData( NULL );
	pStall->SetDisable( FALSE );
}

void CStreetStall::OnFakeRegistItem( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
	CStreetStall* pStall = (CStreetStall*)vData1;

	pStall->SetDisable( FALSE );
	
//	if( pStall->GetCurSelectedItemNum() == -1 ) return;
//	if( pStall->SelectedItemCheck() ) return; 

	if( param1 == 0 )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(542) );
		OnRegistItemCancel( iId, p, param1, vData1, vData2 );
		return;
	}
	else
	{
		CExchangeItem* pExItem = pStall->FindItem( pStall->GetCheckDBIdx() );
		if( pExItem == NULL )
		{
			OnRegistItemCancel( iId, p, param1, vData1, vData2 );
			return;
		}

		//pStall->RegistMoney( pStall->GetCurSelectedItemNum(), param1 );//이러면 꼬일수도.
		pStall->RegistMoney( pExItem->GetPosition(), param1 );
		STREETSTALLMGR->ItemStatus( pExItem, TRUE );
	}
}

void CStreetStall::OnRegistItemCancel( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
	CStreetStall* pStall = (CStreetStall*)vData1;
	CItem* item = (CItem*)pStall->GetData();

	item->SetLock( FALSE );
	pStall->SetData( NULL );
	pStall->SetDisable( FALSE );

//KES
//	if( item->GetLinkPosition() != 0 )
	{
		CExchangeItem* pExItem = (CExchangeItem*)pStall->GetGridDialog()->GetIconForIdx( item->GetLinkPosition() );
		if( pExItem == NULL ) return;
		if( pExItem->GetLinkItem() == NULL ) return;
		
		MSG_LINKITEM msg;
		msg.Category	= MP_STREETSTALL;
		msg.Protocol	= MP_STREETSTALL_DELETEITEM_SYN;
		msg.dwObjectID	= gHeroID;
		msg.ItemInfo	= pExItem->GetLinkItem()->GetItemBaseInfo();
		msg.wAbsPosition = item->GetLinkPosition();
		
		NETWORK->Send( &msg, sizeof( MSG_LINKITEM ) );
	}
}

int CStreetStall::GetGridposForItemIdx(WORD wIdx)
{
	const WORD num = m_pStallGrid->GetCellNum();

	for(WORD i=0;i<num;++i)
	{
		CExchangeItem* pItem = (CExchangeItem*)m_pStallGrid->GetIconForIdx(i);
		if( wIdx ==  pItem->GetDBId() )
			return i;
	}

	return -1;
}

void CStreetStall::GetTitle( char* pStrTitle )
{
//	m_pTitle->GetScriptText( pStrTitle );
	strcpy(pStrTitle, m_pTitleEdit->GetEditText());
}

void CStreetStall::EditTitle( char* title )
{
//	m_pTitle->SetScriptText( title );
	m_pTitleEdit->SetEditText( title );
}

cIcon* CStreetStall::GetCurSelectedItem() 
{ 
	if(m_nCurSelectedItem == -1) return NULL; 
	return m_pStallGrid->GetIconForIdx(m_nCurSelectedItem); 
}


DWORD CStreetStall::GetCurSelectedItemIdx() 
{ 
	if(m_nCurSelectedItem == -1) return 0; 
	return m_pStallGrid->GetIconForIdx(m_nCurSelectedItem)->GetData(); 
}

cIcon* CStreetStall::GetItem( POSTYPE pos )
{
	return m_pStallGrid->GetIconForIdx( pos );
}

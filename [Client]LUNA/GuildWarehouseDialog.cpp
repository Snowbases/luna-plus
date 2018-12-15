#include "stdafx.h"
#include "GuildWarehouseDialog.h"
#include "WindowIDEnum.h"
#include "Interface/cIconGridDialog.h"
#include "Interface/cPushupButton.h"
#include "Interface/cStatic.h"
#include "ItemManager.h"
#include "ChatManager.h"
//#include "GuildManager.h"
#include "Input/UserInput.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "GameIn.h"
//#include "NpcScriptDialog.h"
#include "interface/cWindowManager.h"
#include "cMsgBox.h"

#include "InventoryExDialog.h"



DWORD ConvertTextToValue( const char* text )
{
	char buffer[ MAX_PATH ];
	char* p = buffer;

	while( *text )
	{
		if( ',' != *text )
		{
			*p++ = *text;
		}

		++text;
	}

	*p = 0;

	return strtoul( buffer, 0, 10 );
}



CGuildWarehouseDialog::CGuildWarehouseDialog()
{
	m_type = WT_GUILDWAREHOUSEDLG;
	m_Lock = FALSE;
	m_InitInfo = FALSE;
}


CGuildWarehouseDialog::~CGuildWarehouseDialog()
{}


void CGuildWarehouseDialog::Linking()
{}

void CGuildWarehouseDialog::SetActive( BOOL val )
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}

	const BOOL lastIsActive = m_bActive;

	cTabDialog::SetActive(val);

	if( val )
	{
		OBJECTSTATEMGR->StartObjectState( hero, eObjectState_Deal );

		//if( GetLock() )
		//{
		//	return;
		//}

		//SetLock(TRUE);

		//GUILDMGR->GuildWarehouseInfoSyn(GetCurTabNum());

		SetTabSize( mTabSize );

		// 인벤토리와 함께 중앙에 표시시킨다
		{
			cDialog* inventory = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( inventory );

			const DISPLAY_INFO& screen = GAMERESRCMNGR->m_GameDesc.dispInfo;

			const DWORD x = ( screen.dwWidth - m_width - inventory->GetWidth() ) / 2;
			const DWORD y = ( screen.dwHeight - m_height ) / 2;

			SetAbsXY( x, y );

			inventory->SetAbsXY( x + m_width, y );
			inventory->SetActive( TRUE );
		}

		//if(GAMEIN->GetInventoryDialog()->IsActive() == FALSE)
		//{
		//	GAMEIN->GetInventoryDialog()->SetActive(TRUE);
		//}			
	}
	else
	{
		OBJECTSTATEMGR->EndObjectState( hero, eObjectState_Deal );

		//if( ( HERO->GetState() == eObjectState_Deal ) && (GAMEIN->GetNpcScriptDialog()->IsActive() == FALSE))
		//{
		//	OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		//}

		//GUILDMGR->GuildWarehouseLeave(GetCurTabNum());

		if( lastIsActive )
		{
			MSGBASE message;
			message.Category	= MP_GUILD;
			message.Protocol	= MP_GUILD_CLOSE_WAREHOUSE;
			message.dwObjectID	= HEROID;

			NETWORK->Send( &message, sizeof( message ) );
		}

		// 입금 창도 꺼주자
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( DBOX_INPUT_MONEY );

			if( dialog )
			{
				dialog->SetActive(FALSE);

				WINDOWMGR->AddListDestroyWindow( dialog );
			}
		}

		// 인벤토리 창을 꺼주자
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( dialog );

			dialog->SetActive( FALSE );
		}
	}	
}

//DWORD CGuildWarehouseDialog::ActionEvent(CMouse * mouseInfo)
//{
//	// 아이템을 받는 중이면 기다림. 
//	// 아이템 다 받으면 락 풀기 
//	// 다른 버튼이 선택되면 현재 탭 MSG_BYTE로 MP_ITEM_GUILD_WAREHOUSE_LEAVE 보냄 
//	// 버튼 눌려지면 아이템 보내달라고 요청. 받기 전에 다른 탭 못 누르게 락 
//	//GUILDMGR->GuildWarehouseInfoSyn();
//	//if(GetLock())
//	//	return NULL;
//	return cTabDialog::ActionEvent( mouseInfo );
//}

void CGuildWarehouseDialog::OnActionEvent(LONG id, void * p, DWORD we)
{
	//switch( id )
	//{
	// 071218 LUJ, 길드 창고의 돈 입출금 기능 제거
	//case GDW_PUTINMONEYBTN:
	//	{
	//		cWindow* pInvenDlg		= WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
	//		cWindow* pGuildMoney	= WINDOWMGR->GetWindowForIDEx( GDW_MONEY );

	//		ASSERT( pInvenDlg && pGuildMoney );

	//		WINDOWMGR->DivideBox(
	//			DBOX_INPUT_MONEY,
	//			(LONG)(pGuildMoney->GetAbsX()-50),
	//			(LONG)(pGuildMoney->GetAbsY()-100),
	//			OnPutInMoneyGuildOk,
	//			NULL,
	//			pInvenDlg,
	//			this,
	//			CHATMGR->GetChatMsg( 40 ) );
	//		break;
	//	}
	//case GDW_PUTOUTMONEYBTN:
	//	{
	//		cWindow* pInvenDlg		= WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
	//		cWindow* pGuildMoney	= WINDOWMGR->GetWindowForIDEx( GDW_MONEY );

	//		ASSERT( pInvenDlg && pGuildMoney );

	//		WINDOWMGR->DivideBox(
	//			DBOX_INPUT_MONEY,
	//			(LONG)(pGuildMoney->GetAbsX()-50),
	//			(LONG)(pGuildMoney->GetAbsY()-100),
	//			OnPutOutMoneyGuildOk,
	//			NULL,
	//			pInvenDlg,
	//			this,
	//			CHATMGR->GetChatMsg( 41 ) );
	//		break;
	//	}
	//}
}

void CGuildWarehouseDialog::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}


void CGuildWarehouseDialog::Add(cWindow * window)
{
	if(window->GetType() == WT_PUSHUPBUTTON)
		AddTabBtn(curIdx1++, (cPushupButton * )window);
	else if(window->GetType() == WT_ICONGRIDDIALOG)
	{
		AddTabSheet(curIdx2++, window);
		((cIconGridDialog*)window)->SetDragOverIconType( WT_ITEM );
	}
	else 
		cDialog::Add(window);
}


BOOL CGuildWarehouseDialog::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	ASSERT(icon);
	if( m_bDisable )
	{
		return FALSE;
	}
	else if(icon->GetType() == WT_ITEM)
	{
		return FakeMoveItem(x, y, (CItem *)icon);
	}

	return FALSE;
}


BOOL CGuildWarehouseDialog::FakeMoveItem( LONG mouseX, LONG mouseY, CItem * pFromItem )
{
	// 071208 LYW --- GuildWarehouseDialog : 길드 창고에 봉인 해제 아이템 추가할 수 없도록 처리.
	if( !pFromItem ) return FALSE ;

	const ITEMBASE& pInfo = pFromItem->GetItemBaseInfo();

	if( pInfo.nSealed == eITEM_TYPE_UNSEAL )
	{
		WINDOWMGR->MsgBox(MBI_SEALITEM_STORAGE, MBT_OK, CHATMGR->GetChatMsg(1246)) ;
		return FALSE ;
	}


	// changeitem 중에 이동안되는거
	if( pFromItem->GetItemInfo()->Deposit == eITEM_TYPE_DEPOSIT_NONE )
	{
		return FALSE ;
	}

	if( pFromItem->GetItemInfo()->Deposit == eITEM_TYPE_DEPOSIT_ONLY_STORAGE  )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2035 ) );
		return FALSE;
	}

	WORD ToPos=0;
	
	if( !GetPositionForXYRef( 0, mouseX, mouseY, ToPos ) )	// 절대위치 넘어 옴
		return FALSE;
	if( pFromItem->IsLocked() ) 
		return FALSE;
	CItem * pToItem = GetItemForPos( ToPos );
	if( pToItem )
	{
		// 자동 교환 안됨
		return FALSE;
	}

	if(TP_GUILDWAREHOUSE_START <= pFromItem->GetPosition() && pFromItem->GetPosition() < TP_GUILDWAREHOUSE_END)
		return FakeGeneralItemMove( ToPos, pFromItem, pToItem );
	else if(TP_INVENTORY_START <= pFromItem->GetPosition() && pFromItem->GetPosition() < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() ))) 
		return FakeGeneralItemMove( ToPos, pFromItem, pToItem );
	else if(TP_WEAR_START <= pFromItem->GetPosition() && pFromItem->GetPosition() < TP_WEAR_END)
		return FakeGeneralItemMove( ToPos, pFromItem, pToItem );

	return FALSE;
}


BOOL CGuildWarehouseDialog::FakeGeneralItemMove( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem )
{
	const eITEMTABLE FromTable = ITEMMGR->GetTableIdxForAbsPos(
		pFromItem->GetPosition());

	if(FromTable >= eItemTable_Max) return FALSE;
	if( pToItem )
	{
		return FALSE;
	}
	
	// 080410 LUJ, 장비 아이템의 창고 적재 금지
	if( eItemTable_Storage	== FromTable	||
		eItemTable_Weared	== FromTable )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 76 ) );
		return FALSE;
	}

	const eITEMTABLE ToTable = ITEMMGR->GetTableIdxForAbsPos(
		ToPos);

	if(ToTable >= eItemTable_Max) return FALSE;


	MSG_ITEM_MOVE_SYN msg;
	msg.Category			= MP_ITEM;
	msg.Protocol			= MP_ITEM_GUILD_MOVE_SYN;
	msg.dwObjectID			= HEROID;

	msg.FromPos				= pFromItem->GetPosition();
	msg.wFromItemIdx		= pFromItem->GetItemIdx();
	msg.ToPos				= ToPos;
	msg.wToItemIdx			= (pToItem?pToItem->GetItemIdx():0);

	if( msg.FromPos == msg.ToPos )
		return FALSE;

	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Storage);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_MunpaWarehouse);

	NETWORK->Send( &msg, sizeof(msg) );
	return TRUE;
}

BOOL CGuildWarehouseDialog::GetPositionForXYRef(BYTE param, LONG x, LONG y, WORD& pos)
{
	WORD position=0;										// 상대 위치

	cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum());
	if(gridDlg->GetPositionForXYRef(x, y, position))
	{
		pos = WORD(position+TP_GUILDWAREHOUSE_START + TABCELL_GUILDWAREHOUSE_NUM * GetCurTabNum());		// -> 절대 위치로 바꿈
		return TRUE;
	}
	return FALSE;
}


CItem * CGuildWarehouseDialog::GetItemForPos(POSTYPE absPos)
{
	if(TP_GUILDWAREHOUSE_START <= absPos && absPos < TP_GUILDWAREHOUSE_END)
	{
		cIconGridDialog * Dlg = (cIconGridDialog *)GetTabSheet(
			GetCurTabNum());

		if(absPos < (TP_GUILDWAREHOUSE_START+TABCELL_GUILDWAREHOUSE_NUM * GetCurTabNum()))
			return NULL;

		return (CItem *)Dlg->GetIconForIdx(
			WORD(absPos-TP_GUILDWAREHOUSE_START-TABCELL_GUILDWAREHOUSE_NUM * GetCurTabNum()));
	}
	return NULL;
}


BOOL CGuildWarehouseDialog::AddItem( const ITEMBASE* pItemInfo)
{
	if(pItemInfo->dwDBIdx == 0)
	{
		ASSERTMSG(0,"길드창고-아이템 디비인덱스가 0. Invalid GuildWarehouse Index.");
		return FALSE;
	}
	CItem* pItem = ITEMMGR->GetItem(pItemInfo->dwDBIdx);
	if(pItem == 0)
		pItem = ITEMMGR->MakeNewItem( pItemInfo,"CGuildWarehouseDialog::AddItem");
	ASSERT(pItem);

	// 071207 LYW --- GuildWarehouseDialog : 길드 창고 아이템 봉인여부 세팅.
	pItem->SetSeal( pItemInfo->nSealed == eITEM_TYPE_SEAL );

	return AddItem( pItem );
}


BOOL CGuildWarehouseDialog::AddItem(CItem* pItem)
{
	ITEMMGR->RefreshItem(pItem);
	POSTYPE relPos = GetRelativePosition(pItem->GetPosition());
	int tabIndex = GetTabIndex( pItem->GetPosition() );
	cIconGridDialog * dlg = (cIconGridDialog *)GetTabSheet(
		BYTE(tabIndex));

	return dlg->AddIcon(relPos, pItem);
}


BOOL CGuildWarehouseDialog::DeleteItem(POSTYPE Pos,CItem** ppItem)
{
	int TabIndex = GetTabIndex( Pos );
	cIconGridDialog * Dlg = (cIconGridDialog *)GetTabSheet(
		BYTE(TabIndex));

	if(!Dlg->IsAddable(WORD(Pos - TP_GUILDWAREHOUSE_START-TABCELL_GUILDWAREHOUSE_NUM*TabIndex)))
	{
		return Dlg->DeleteIcon(
			WORD(Pos-TP_GUILDWAREHOUSE_START-TABCELL_GUILDWAREHOUSE_NUM * TabIndex),
			(cIcon **)ppItem);
	}

	return FALSE;

}


POSTYPE CGuildWarehouseDialog::GetRelativePosition( POSTYPE absPos)
{
	return POSTYPE(( absPos - TP_GUILDWAREHOUSE_START ) % TABCELL_GUILDWAREHOUSE_NUM);
}


WORD CGuildWarehouseDialog::GetTabIndex( POSTYPE absPos )
{
	return WORD(( absPos - TP_GUILDWAREHOUSE_START ) / TABCELL_GUILDWAREHOUSE_NUM);
}


void CGuildWarehouseDialog::SetLock(BOOL val)
{
	m_Lock = val;
}


BOOL CGuildWarehouseDialog::GetLock() const
{
	return m_Lock;
}

void CGuildWarehouseDialog::SetTabSize( USHORT size )
{
	ASSERT( size <= GetTabNum() );

	mTabSize = min( GetTabNum(), size );

	for( BYTE i = 0; i < size; ++i )
	{
		cWindow*		sheet	= GetTabSheet( i );
		cPushupButton*	button	= GetTabBtn( i );

		ASSERT( sheet && button );

		sheet->SetActive( TRUE );
		button->SetActive( TRUE );
	}

	for( USHORT i = size; i < GetTabNum(); ++i )
	{
		cWindow*		sheet	= GetTabSheet( BYTE( i ) );
		cPushupButton*	button	= GetTabBtn( BYTE( i ) );

		ASSERT( sheet && button );

		sheet->SetActive( FALSE );
		button->SetActive( FALSE );
	}
}


void CGuildWarehouseDialog::AddItem( const MSG_GUILD_ITEM* message )
{
	cIconGridDialog* sheet1 = ( cIconGridDialog* )GetTabSheet( 0 );
	ASSERT( sheet1 );

	// 클리어
	for( BYTE sheetIndex = 0; sheetIndex < GetTabNum(); ++sheetIndex )
	{
		cIconGridDialog* sheet = ( cIconGridDialog* )GetTabSheet( sheetIndex );
		ASSERT( sheet );

		for( WORD i = 0; i < sheet->GetCellNum(); ++i )
		{
			CItem* item;

			if(DeleteItem(POSTYPE(TP_GUILDWAREHOUSE_START + i + sheetIndex * sheet->GetCellNum()), &item))
			{
				ITEMMGR->ItemDelete( item );
			}
		}
	}

	for( DWORD i = 0; i < message->mSize; ++i )
	{
		const ITEMBASE& item = message->mItem[ i ];

		if( item.dwDBIdx )
		{
			AddItem( &item );
		}
	}	
}
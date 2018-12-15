#include "stdafx.h"
#include "StreetStall.h"
#include "usertable.h"
#include "Player.h"
#include "objectstatemanager.h"
#include "itemmanager.h"


cStreetStall::cStreetStall()
{
	Init();
}

cStreetStall::~cStreetStall()
{
	m_pOwner = NULL;
	m_GuestList.RemoveAll();
//	EmptyCellAll();
}

void cStreetStall::Init()
{
	m_pOwner = NULL;
	m_nCurRegistItemNum = 0;
	m_wStallKind = eSK_NULL;
	EmptyCellAll();
	DeleteGuestAll();
//	m_nUsable = DEFAULT_USABLE_INVENTORY;	
	m_nTotalMoney = 0;
}

BOOL cStreetStall::FillCell( ITEMBASE* pBase, DWORD money, BOOL bLock, DWORD Volume, WORD wAbsPosition )
{
	WORD pos = 0;
	
	if(!m_pOwner) 
	{
		return FALSE;
	}
	
	if(m_wStallKind == eSK_SELL)
	{
		CItemSlot* pSlot = m_pOwner->GetSlot(pBase->Position);
		
		if( !pSlot )
		{
			ASSERT(0);
			return TRUE;
		}
		
		pSlot->SetLock(pBase->Position, bLock);

		for(int n = 0; n<MAX_STREETSTALL_CELLNUM;++n)
		{
			if( !m_sArticles[n].bFill )
				break;
		}
		pos = WORD( n );
		if( pos == MAX_STREETSTALL_CELLNUM ) return FALSE;
		if( pBase->wIconIdx == 0 ) return FALSE;
	}
	else if(m_wStallKind == eSK_BUY)
	{	// 구매 노점의 경우 데이터가 있으면 덮어 씌운다
		pos = wAbsPosition;
		if(m_sArticles[pos].bFill)
			EmptyCell( pos );

	}

	m_sArticles[pos].bFill = TRUE;
	m_sArticles[pos].bLock = bLock;
	m_sArticles[pos].sItemBase = *pBase;
	m_sArticles[pos].dwMoney = money;
	m_sArticles[pos].wVolume = (WORD)Volume;

	++m_nCurRegistItemNum;	

	if(m_wStallKind == eSK_BUY)
	{
		m_nTotalMoney = 0;
		
		for(int n = 0; n<MAX_STREETBUYSTALL_CELLNUM;++n)
		{
			if( !m_sArticles[n].bFill )
				continue;
			
			m_nTotalMoney += m_sArticles[n].dwMoney * m_sArticles[n].wVolume;
		}
	}
	return TRUE;
}


void cStreetStall::EmptyCell( ITEMBASE* pBase, eITEMTABLE tableIdx )
{
	WORD pos;
	BOOL bSetItemSlot = TRUE;
	CItemSlot* pSlot = NULL;

	if( m_pOwner )
	{
		pSlot = m_pOwner->GetSlot(tableIdx);
		if( !pSlot )
		{
			ASSERT(0);
			bSetItemSlot = FALSE;
		}
	}
	else 
		bSetItemSlot = FALSE;

	for(int n = 0; n<MAX_STREETSTALL_CELLNUM;++n)
	{
		if( m_sArticles[n].bFill )
		{
			if(m_sArticles[n].sItemBase.dwDBIdx == pBase->dwDBIdx)
				break;
		}
	}

	pos = WORD( n );
	if(  pos == MAX_STREETSTALL_CELLNUM ) return;
	if( !m_sArticles[pos].bFill ) return; // 이미 비워져 있다..

	pSlot->SetLock(m_sArticles[pos].sItemBase.Position, FALSE);
	
	m_sArticles[pos].Init();

/*	Bugs code!!!!!!
	for( int i = pos ; i < MAX_STREETSTALL_CELLNUM-1 ; ++i )
	{
		if( !m_sArticles[i+1].bFill )
		{
			m_sArticles[i].Init();
			break;
		}
		m_sArticles[i] = m_sArticles[i+1];
	}
*/

	for( int i = pos ; i < MAX_STREETSTALL_CELLNUM-1 ; ++i )
	{
		if( !m_sArticles[i+1].bFill )
			break;

		m_sArticles[i] = m_sArticles[i+1];
		m_sArticles[i+1].Init();
	}

	--m_nCurRegistItemNum;
}

void cStreetStall::EmptyCell( POSTYPE pos )
{
	if(  pos == MAX_STREETSTALL_CELLNUM ) return;
	if( !m_sArticles[pos].bFill ) return; // 이미 비워져 있다..

	m_nTotalMoney -= (m_sArticles[pos].dwMoney * m_sArticles[pos].wVolume);
	m_sArticles[pos].Init();

	--m_nCurRegistItemNum;
}


void cStreetStall::UpdateCell( WORD pos, DURTYPE dur )
{
	if( pos > MAX_STREETSTALL_CELLNUM ) return;
	if( !m_sArticles[pos].bFill ) return;
	
	if(m_wStallKind == eSK_SELL)
		m_sArticles[pos].sItemBase.Durability = dur;
	else if(m_wStallKind == eSK_BUY)
		m_sArticles[pos].wVolume = (WORD)dur;
}


void cStreetStall::EmptyCellAll()
{
	BOOL bSetItemSlot = TRUE;
	CItemSlot* pSlot = NULL;

	if( m_pOwner )
	{
		pSlot = m_pOwner->GetSlot(eItemTable_Inventory);

		if( !pSlot)
		{
			ASSERT(0);
			bSetItemSlot = FALSE;
		}
	}
	else
		bSetItemSlot = FALSE;

	for(int i=0;i<MAX_STREETSTALL_CELLNUM;++i)
	{

		if( !m_sArticles[i].bFill )
		{
			if(m_wStallKind == eSK_SELL)
				break;
			if(m_wStallKind == eSK_BUY)
				continue;
		}

		if( bSetItemSlot )
		{
			if(m_wStallKind == eSK_SELL)
			{
				CItemSlot* pItemSlot = m_pOwner->GetSlot( m_sArticles[i].sItemBase.Position );
				if( pItemSlot )
					pSlot->SetLock(m_sArticles[i].sItemBase.Position, FALSE);
			}
//			pSlot->SetLock(m_sArticles[i].sItemBase.Position, FALSE);
//			pSlotShop->SetLock(m_sArticles[i].sItemBase.Position, FALSE);
		}
		m_sArticles[i].Init();
		m_sArticles[i].dwMoney = 0;
	}
	m_nCurRegistItemNum = 0;
}

void cStreetStall::SetMoney( WORD pos, DWORD money )
{
	if( pos > MAX_STREETSTALL_CELLNUM ) return; // 갯수를 초과한다??
	if( !m_sArticles[pos].bFill ) return; // 비워져 있다..
	if( m_sArticles[pos].bLock ) return;

	m_sArticles[pos].dwMoney = money;
}

void cStreetStall::SetVolume( WORD pos, WORD Volume )
{
	if( pos > MAX_STREETSTALL_CELLNUM ) return; // 갯수를 초과한다??
	if( !m_sArticles[pos].bFill ) return; // 비워져 있다..
	if( m_sArticles[pos].bLock ) return;
	
	m_sArticles[pos].wVolume = Volume;
}

void cStreetStall::ChangeCellState( WORD pos, BOOL bLock ) // (주의)등록과는 다르게 배열의 인덱스가 넘어온다.
{
	if(!m_pOwner) 
	{
		return;
	}

	if( pos > MAX_STREETSTALL_CELLNUM ) return; // 갯수를 초과한다??
	if( !m_sArticles[pos].bFill ) return; // 비워져 있다..
	if( m_sArticles[pos].bLock == bLock ) return;
	if( m_wStallKind == eSK_BUY ) return;

	CItemSlot* pSlot = m_pOwner->GetSlot(m_sArticles[pos].sItemBase.Position);
	if( !pSlot )
	{
		ASSERT(0);
		return;
	}

	pSlot->SetLock(m_sArticles[pos].sItemBase.Position, bLock);
	m_sArticles[pos].bLock = bLock;
}

void cStreetStall::AddGuest( CPlayer* pGuest )
{
	if( m_GuestList.Find(pGuest) ) return;

	pGuest->SetGuestStall( this );
	m_GuestList.AddTail( pGuest );
}

void cStreetStall::DeleteGuest( CPlayer* pGuest )
{
	pGuest->SetGuestStall( NULL );

	if( m_GuestList.Find(pGuest) == FALSE ) return;
	m_GuestList.Remove( pGuest );
}

void cStreetStall::DeleteGuestAll()
{
	PTRLISTPOS pos =  m_GuestList.GetHeadPosition();

	while(pos)
	{
		CPlayer* pGuest = (CPlayer*)m_GuestList.GetNext( pos );
		pGuest->SetGuestStall( NULL );
	}
	m_GuestList.RemoveAll();
}

void cStreetStall::GetStreetStallInfo( STREETSTALL_INFO& stall )
{
	ITEM_OPTION OptionInfo[SLOT_STREETSTALL_NUM];
	WORD OptionNum = 0;
	stall.StallKind = m_wStallKind;
	
	for(int i=0;i<MAX_STREETSTALL_CELLNUM;++i)
	{
		const ITEMBASE& item = m_sArticles[i].sItemBase;

		stall.Item[i].wIconIdx		= item.wIconIdx;
		stall.Item[i].dwDBIdx		= item.dwDBIdx;
		stall.Item[i].Durability	= item.Durability;
		stall.Item[i].money			= m_sArticles[i].dwMoney;
		stall.Item[i].Locked		= char( m_sArticles[i].bLock );
		stall.Item[i].Fill			= char( m_sArticles[i].bFill );
		stall.Item[i].ItemParam		= item.ItemParam;
		stall.Item[i].wVolume		= m_sArticles[i].wVolume;
		// 071215 LYW --- StreetStall : 봉인 여부를 세팅한다.
		stall.Item[i].nSeal			= item.nSealed ;
		
		if( stall.Item[i].Fill )
		{			
			const ITEM_OPTION& option = ITEMMGR->GetOption( item );

			if( option.mItemDbIndex )
			{
				OptionInfo[ OptionNum++ ] = option;
			}
		}
	}

	if( OptionNum )
	{
		stall.AddableInfo.AddInfo( CAddableInfoList::ItemOption, sizeof( OptionInfo[ 0 ] )*OptionNum, OptionInfo, __FUNCTION__ );
		stall.count = OptionNum;
	}
	
	if( m_pOwner )
		m_pOwner->GetStreetStallTitle(stall.Title);
}

BOOL cStreetStall::CheckItemDBIdx(DWORD idx)
{
	for(int i=0;i<MAX_STREETSTALL_CELLNUM;++i)
	{
		if( m_sArticles[i].bFill )
		{
			if( m_sArticles[i].sItemBase.dwDBIdx == idx)
				return FALSE;
		}
	}

	return TRUE;
}

BOOL cStreetStall::CheckItemIdx(DWORD idx)
{
	for(int i=0;i<MAX_STREETSTALL_CELLNUM;++i)
	{
		if( m_sArticles[i].bFill )
		{
			if( m_sArticles[i].sItemBase.wIconIdx == idx)
				return FALSE;
		}
	}
	
	return TRUE;
}

BOOL cStreetStall::IsFull()
{
	for(int i=0;i<MAX_STREETSTALL_CELLNUM;++i)
	{
		if( !m_sArticles[i].bFill )
			return FALSE;
	}
	
	return TRUE;
}


/* 손님의 상태 변경 Flag 추가 03.12.26 */
void cStreetStall::SendMsgGuestAll( MSGBASE* pMsg, int nMsgLen, BOOL bChangeState )
{
	PTRLISTPOS pos =  m_GuestList.GetHeadPosition();

	while(pos)
	{
		CPlayer* pGuest = (CPlayer*)m_GuestList.GetNext( pos );
		if( !pGuest )
			continue;

		pGuest->SendMsg( pMsg, nMsgLen);

		if( bChangeState )
		{
			if( pGuest->GetState() == eObjectState_StreetStall_Guest)
				OBJECTSTATEMGR_OBJ->EndObjectState(pGuest, eObjectState_StreetStall_Guest);
		}
	}
}
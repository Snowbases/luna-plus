#include "stdafx.h"
#include "QuickManager.h"
#include "ObjectManager.h"
#include "InventoryExDialog.h"
#include "GameIn.h"
#include "cQuickItem.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cScriptManager.h"
#include "ItemManager.h"
#include "cSkillTreeManager.h"
#include "./Interface/cIcon.h"
#include "./Input/UserInput.h"
#include "ChatManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/skillinfo.h"
#include "QuickDlg.h"
#include "ConductManager.h"
#include "ConductInfo.h"
#include "../[cc]skill/client/info/activeskillinfo.h"
#include "cSkillBase.h"
#include "StorageDialog.h"

GLOBALTON(CQuickManager);

CQuickManager::CQuickManager()
{
	m_IconIndexCreator.Init(MAX_ITEMICON_NUM, IG_QUICKITEM_STARTINDEX);
}

CQuickManager::~CQuickManager()
{
	m_IconIndexCreator.Release();
}

// 080702 LYW --- QuickManager : 확장슬롯 추가 처리.
cQuickItem* CQuickManager::NewQuickItem( WORD tab, WORD pos )
{
	cQuickItem* pQuickItem;
	pQuickItem = new cQuickItem( tab, pos );

	pQuickItem->Init(0,0,DEFAULT_ICONSIZE,DEFAULT_ICONSIZE, NULL, m_IconIndexCreator.GenerateIndex());
	WINDOWMGR->AddWindow(pQuickItem);	

	mQuickItemTable[ tab ][ pos ] = pQuickItem;

	return pQuickItem;
}

// 080702 LYW --- QuickManager : 확장 슬롯 추가 처리.
void CQuickManager::AddQuickItem( WORD tab, WORD pos, SLOT_INFO* pSlot )
{
	cQuickItem* pQuickItem = NULL ;
	pQuickItem = mQuickItemTable[ tab ][ pos ];

	if(!pQuickItem) return ;

	CItem* pItem	=	ITEMMGR->GetItem( pSlot->dbIdx );

	if( CheckItemFromStorage( pItem ) )
		return;

	pQuickItem->ClearLink();
	pQuickItem->SetSlotInfo(
		*pSlot);

	switch( pSlot->kind )
	{
	case QuickKind_None:
		{
		}
		break;
	case QuickKind_SingleItem:
		{
			CItem* pItem = ITEMMGR->GetItem( pSlot->dbIdx );

			// 071126 LYW --- QuickManager : 봉인 아이템 처리.
			if( !pItem ) return ;

			if( pItem )
			{
				pQuickItem->SetCoolTime( pItem->GetRemainedCoolTime() );
			}

			const ITEMBASE& pItemBase = pItem->GetItemBaseInfo() ;

			if( pItemBase.nSealed == eITEM_TYPE_SEAL )
			{
				pQuickItem->SetSeal(TRUE) ;
			}
		}
		break;
	case QuickKind_MultiItem:
		{
			CItem* item = ITEMMGR->GetItem( pSlot->dbIdx );

			if( item )
			{
				pQuickItem->SetCoolTime( item->GetRemainedCoolTime() );
			}

			WORD dur = ( WORD )( GAMEIN->GetInventoryDialog()->GetTotalItemDurability( pSlot->idx ) );
			pQuickItem->SetSlotData( dur );
		}
		break;
	case QuickKind_Skill:
		{
			const LEVELTYPE level = pSlot->data;
			const DWORD	index = pSlot->idx;
			cSkillBase* const pSkillBase = SKILLTREEMGR->GetSkill(
				index);

			if(0 == pSkillBase )
			{
				break;
			}

			// 080303 ggomgrak --- SlotData Update
			pQuickItem->SetSlotData(
				level);
			pQuickItem->SetData(
				pSkillBase->GetSkillIdx());

			SKILLTREEMGR->SetToolTipIcon(
				pQuickItem,
				index);
		}
		break;
	}

	GetImage( pQuickItem );
	ITEMMGR->AddToolTip( pQuickItem );
}

void CQuickManager::RemoveQuickItem( WORD tab, WORD pos )
{
	cQuickItem* pQuickItem = mQuickItemTable[ tab ][ pos ];

	if( !pQuickItem ) return ;

	pQuickItem->SetSeal(FALSE) ;

	pQuickItem->ClearLink();
	pQuickItem->SetCoolTime( 0 );
}

void CQuickManager::ChangeQuickItem( WORD tab1, WORD pos1, WORD tab2, WORD pos2 )
{
	cQuickItem* pQuickItem1 = mQuickItemTable[ tab1 ][ pos1 ];

	if(!pQuickItem1) return ;

	cQuickItem* pQuickItem2 = mQuickItemTable[ tab2 ][ pos2 ];

	SLOT_INFO info1 = pQuickItem1->GetSlotInfo();
	SLOT_INFO info2 = pQuickItem2->GetSlotInfo();

	RemoveQuickItem( tab1, pos1 );
	RemoveQuickItem( tab2, pos2 );

	AddQuickItem( tab1, pos1, &info2 );
	AddQuickItem( tab2, pos2, &info1 );
}

void CQuickManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_QUICK_INFO:
		{
			Quick_Info( pMsg ) ;
		}
		break;
	case MP_QUICK_ADD_ACK:
		{
			Quick_Add_Ack( pMsg ) ;
		}
		break;
	case MP_QUICK_REMOVE_ACK:
		{
			Quick_Remove_Ack( pMsg ) ;
		}
		break;
	case MP_QUICK_CHANGE_ACK:
		{
			Quick_Change_Ack( pMsg ) ;
		}
		break;
	}
}


void CQuickManager::UseQuickItem( WORD tab, WORD pos )
{
	cQuickItem* pQuickItem = mQuickItemTable[ tab ][ pos ];
	
	CItem* pItem	=	ITEMMGR->GetItem( pQuickItem->GetSlotInfo().dbIdx );

	if( CheckItemFromStorage( pItem ) )
	{
		DeleteLinkdedQuickItem( pItem->GetItemBaseInfo().dwDBIdx );
		return;
	}	
	
	if(!pQuickItem) return;

	switch( pQuickItem->GetSlotInfo().kind )
	{
	case QuickKind_SingleItem:
		{
			CItem* pItem = ITEMMGR->GetItem( pQuickItem->GetSlotInfo().dbIdx );

			if( pItem )
			{
				GAMEIN->GetInventoryDialog()->UseItem( pItem );
			}
		}
		break;
	case QuickKind_MultiItem:
		{
			CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForIdx( pQuickItem->GetSlotInfo().idx );

			if( pItem )
			{
				GAMEIN->GetInventoryDialog()->UseItem( pItem );
			}

		}
		break;
	case QuickKind_Skill:
		{
#ifdef _TESTCLIENT_
			cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( pQuickItem->GetSlotInfo().idx );

			if( pSkillInfo )
			{
				SKILLMGR->OnSkillCommand( pSkillInfo->GetIndex() + pSkillInfo->GetLevel() );
			}
#else
			const cSkillBase* const pSkillBase = SKILLTREEMGR->GetSkill(
				pQuickItem->GetSlotInfo().idx);

			if( pSkillBase )
			{
				SKILLMGR->OnSkillCommand(
					pSkillBase->GetSkillIdx());
			}
#endif
		}
		break;

		// 070220 LYW --- QuickManager : Add case part to use conduct item.
	case QuickKind_Conduct :
		{
			CONDUCTMGR->OnConductCommand( pQuickItem->GetSlotInfo().idx ) ;
		}
		break ;
	case QuickKind_PetSkill:
		{
			if( !HEROPET )
				return;
			
			CItem* pItem = ITEMMGR->GetItem( pQuickItem->GetSlotInfo().dbIdx );
			if( !pItem )
				return;

			ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( pQuickItem->GetSlotInfo().idx );
			if( !pInfo )
				return;

			DWORD target = OBJECTMGR->GetSelectedObjectID();

			DWORD skillindex = HEROPET->GetRealSkillIndex( pInfo->SupplyValue );

			if( target )
			{
				SKILLMGR->OnPetSkillCommand( skillindex );
			}
		}
		break;

	}
}

void CQuickManager::Release()
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ];
			ReleaseQuickItem( pQuick );
			mQuickItemTable[ sheet ][ pos ] = NULL;
		}
	}
}

void CQuickManager::RefreshQickItem()
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ];

			if(!pQuick) continue ;

			if(QuickKind_MultiItem == pQuick->GetSlotInfo().kind)
			{
				const WORD quantity = WORD(GAMEIN->GetInventoryDialog()->GetTotalItemDurability(
					pQuick->GetSlotInfo().idx));

				pQuick->SetZeroCount(
					quantity <= 0);
				pQuick->SetSlotData(
					quantity);
			}

			ITEMMGR->AddToolTip(
				pQuick);
		}
	}
}

// 071215 LYW --- QuickManager : 퀵슬롯에 등록 된 아이템이 인벤에서 삭제될 때, 퀵 아이템을 삭제하는 함수 추가.
//
// 퀵 슬롯에 등록 된 아이템이 인벤토리에서 삭제될 때, 퀵슬롯에 등록된 퀵 아이템도 삭제해야 한다. 
// 봉인관련 아이템이나, 인챈트, 강화에 의해 기본 아이템정보에서 변경이 된 아이템들의 툴팁이 뿌려지고 있는상황에서, 
// 인벤에서만 아이템이 삭제될 경우, 이전에 진행하고 있던 처리과정의 정보들을 얻지 못해, 에러가 발생하기 때문이다.
//
void CQuickManager::DeleteLinkdedQuickItem(DWORD dwDBIdx)
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ] ;

			if( !pQuick ) continue ;

			if( pQuick->GetSlotInfo().kind != QuickKind_SingleItem ) continue ; 			

			if(pQuick->GetSlotInfo().dbIdx != dwDBIdx) continue ;

			RemoveQuickItem(sheet, pos) ;
		}
	}
}

// 071215 LYW --- QuickManager : 봉인된 아이템이 링크가 걸려있을 경우,
// 봉인이 풀렸을 경우 봉인을 풀어주기 위한 함수 추가.
void CQuickManager::UnsealQuickItem(DWORD dwDBIdx)
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		// 첫번째 슬롯 봉인 해제 처리.
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ] ;

			if( !pQuick ) continue ;

			if( pQuick->GetSlotInfo().kind != QuickKind_SingleItem ) continue ; 			

			if(pQuick->GetSlotInfo().dbIdx != dwDBIdx) continue ;

			pQuick->SetSeal(FALSE) ;
		}
	}
}

void CQuickManager::ReleaseQuickItem(cQuickItem * quick)
{
	if( quick == NULL )
	{
		ASSERT(0);
		return;
	}
	m_IconIndexCreator.ReleaseIndex(quick->GetID());
	WINDOWMGR->AddListDestroyWindow( quick );
}

void CQuickManager::GetImage( cQuickItem * pQuickItem )
{
	cImage lowImage;
	cImage highImage;

	switch(pQuickItem->GetSlotInfo().kind)
	{
	case QuickKind_None:
		{
			pQuickItem->SetImage2( NULL, NULL );
		}
		break;
	case QuickKind_SingleItem:
	case QuickKind_MultiItem:
	case QuickKind_PetItem:
	case QuickKind_PetSkill:
		{
			SCRIPTMGR->GetImage(
				ITEMMGR->GetItemInfo(pQuickItem->GetSlotInfo().idx)->Image2DNum,
				&lowImage,
				PFT_ITEMPATH);
			pQuickItem->SetImage2(
				&lowImage,
				0);
		}
		break;
	case QuickKind_Skill:
		{
			const cSkillBase* const skillBase = SKILLTREEMGR->GetSkill(
				pQuickItem->GetSlotInfo().idx);

			if(0 == skillBase)
			{
				break;
			}

			const cSkillInfo* const skillInfo = SKILLMGR->GetSkillInfo(
				skillBase->GetSkillIdx());

			if(0 == skillInfo)
			{
				break;
			}

			SCRIPTMGR->GetImage(
				skillInfo->GetImage(),
				&highImage,
				PFT_SKILLPATH);
			pQuickItem->SetImage2(
				&lowImage,
				&highImage );
		}
		break;
		// 070216 LYW --- QuickManager : Add case part for conduct to GetImage function.
	case QuickKind_Conduct :
		{
			cConductInfo* pConductInfo = CONDUCTMGR->GetConductInfo(
				pQuickItem->GetSlotInfo().idx ) ;

			if( pConductInfo == NULL )
			{
				ASSERT(0) ;
				return ;
			}

			if( pConductInfo->GetHighImage() != -1 )
				SCRIPTMGR->GetImage( pConductInfo->GetHighImage(), &highImage, PFT_CONDUCTPATH ) ;

			pQuickItem->SetImage2( &lowImage, &highImage ) ;
		}
		break ;
	}
}

BOOL CQuickManager::CheckQPosForItemIdx( DWORD ItemIdx )
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ];

			if( pQuick->GetSlotInfo().idx == ItemIdx )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


BOOL CQuickManager::CheckQPosForDBIdx( DWORD DBIdx )
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ];

			if( pQuick->GetSlotInfo().dbIdx == DBIdx )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

void CQuickManager::Quick_Info( void* pMsg )
{
	MSG_QUICK_INFO* pmsg = ( MSG_QUICK_INFO* )pMsg;

	for( WORD tab = 0; tab < TOTAL_SLOTPAGE; tab++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			if(tab < MAX_SLOTPAGE)
			{
				AddQuickItem( tab, pos, &( pmsg->tab[ tab ].slot[ pos ] ) );
			}
			else
			{
				AddQuickItem( tab, pos, &( pmsg->tab[ tab ].slot[ pos ] ) );
			}
		}
	}
}

void CQuickManager::Quick_Add_Ack( void* pMsg )
{
	MSG_QUICK_ADD_SYN* pmsg = ( MSG_QUICK_ADD_SYN* )pMsg;

	if(pmsg->tabNum < MAX_SLOTPAGE)
	{
		AddQuickItem( pmsg->tabNum, pmsg->pos, &( pmsg->slot ) );
	}
	else
	{
		AddQuickItem( pmsg->tabNum, pmsg->pos, &( pmsg->slot ) );
	}
}

void CQuickManager::Quick_Remove_Ack( void* pMsg )
{
	MSG_QUICK_REMOVE_SYN* pmsg = ( MSG_QUICK_REMOVE_SYN* )pMsg;

	RemoveQuickItem( pmsg->tabNum, pmsg->pos );
}

void CQuickManager::Quick_Change_Ack( void* pMsg )
{
	MSG_QUICK_CHANGE_SYN* pmsg = ( MSG_QUICK_CHANGE_SYN* )pMsg;

	ChangeQuickItem( pmsg->tabNum1, pmsg->pos1, pmsg->tabNum2, pmsg->pos2 );
}

void CQuickManager::AddToolTip( cQuickItem* pQuickItem )
{
	switch(pQuickItem->GetSlotInfo().kind)
	{
	case QuickKind_Skill:
		{
			const cSkillBase* const skillBase = SKILLTREEMGR->GetSkill(
				pQuickItem->GetSlotInfo().idx);

			if(0 == skillBase)
			{
				break;
			}

			SKILLTREEMGR->SetToolTipIcon(
				pQuickItem,
				skillBase->GetSkillIdx());
			break;
		}
	default:
		{
			ITEMMGR->AddToolTip(
				pQuickItem);
			break;
		}
	}
}

void CQuickManager::RefreshSkillItem( DWORD skillIndex )
{
	for( WORD i = 0; i < TOTAL_SLOTPAGE; ++i )
	{
		for( WORD j = 0; j < MAX_SLOTNUM; ++j )
		{
			cQuickItem* item = mQuickItemTable[ i ][ j ];

			if(0 == item)
			{
				continue;
			}
			else if( QuickKind_Skill != item->GetSlotInfo().kind )
			{
				continue;
			}

			const cSkillBase* const quickSkill = SKILLTREEMGR->GetSkill(
				item->GetData());
			const cSkillBase* const refreshSkill = SKILLTREEMGR->GetSkill(
				skillIndex);

			if(0 == quickSkill)
			{
				RemoveQuickItem(
					i,
					j);
			}
			else if(quickSkill == refreshSkill)
			{
				item->SetData(
					refreshSkill->GetSkillIdx());
				GetImage(
					item);
			}
		}
	}
}

BOOL CQuickManager::CheckItemFromStorage( CItem* pItem )
{
	if( !pItem )
		return FALSE;
	
	CStorageDialog* pStorageDlg	=	GAMEIN->GetStorageDialog();

	int iStorageNum	=	pStorageDlg->GetStorageNum( pItem->GetItemBaseInfo().Position );

	if( iStorageNum < 0 )
		return FALSE;

	return	TRUE;
}
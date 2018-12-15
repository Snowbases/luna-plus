// QuestExecute_Item.cpp: implementation of the CQuestExecute_Item class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestExecute_Item.h"													// 퀘스트 실행 아이템 헤더를 불러온다.

#include "QuestScriptLoader.h"													// 퀘스트 스크립트 로더 헤더를 불러온다.

#ifdef _MAPSERVER_																// 맵 서버가 선언되어 있다면,

#include "Player.h"																// 플레이어 헤더를 불러온다.

#include "QuestGroup.h"															// 퀘스트 그룹 헤더를 불러온다.

#include "ItemManager.h"														// 아이템 매니져 헤더를 불러온다.

#include "Quest.h"																// 퀘스트 헤더를 불러온다.

#include "SubQuestInfo.h"
#include "QuestTrigger.h"
#include "QuestManager.h"

#include "..\[CC]Header\GameResourceManager.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 생성자 함수.
CQuestExecute_Item::CQuestExecute_Item( DWORD dwExecuteKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
: CQuestExecute( dwExecuteKind, pTokens, dwQuestIdx, dwSubQuestIdx )
{
	m_dwItemKind = m_dwItemNum = m_dwItemProbability = m_dwWeaponKind = 0;		// 아이템 종류, 아이템 숫자, 아이템 속석, 무기 타입 등을 0으로 세팅한다.

	switch( dwExecuteKind )														// 실행 종류를 확인한다.
	{
	case eQuestExecute_GiveItem:												// 아이템을 주는 실행 이거나,
	case eQuestExecute_GiveQuestItem:											// 퀘스트 아이템을 주는 실행이라면,
		{
			GetScriptParam( m_dwItemKind, pTokens );							// 아이템 종류를 받는다.
			GetScriptParam( m_dwItemNum, pTokens );								// 아이템 수를 받는다.
		}
		break;

	case eQuestExecute_GiveMoney:												// 머니를 주는 실행이거나,
	case eQuestExecute_TakeMoney:												// 머니를 받는 실행이거나,
	case eQuestExecute_TakeExp:													// 경험치를 받는 실행이거나,
	case eQuestExecute_TakeSExp:												// SP를 받는 실행이라면,
		{
			GetScriptParam( m_dwItemNum, pTokens );								// 아이템 수를 받는다.
		}
		break;

	case eQuestExecute_TakeQuestItem:											// 퀘스트 아이템을 받는 실행이거나,
	case eQuestExecute_TakeItem:												// 아이템을 받는 실행이라면,
		{
			GetScriptParam( m_dwItemKind, pTokens );							// 아이템 종류를 받는다.
			GetScriptParam( m_dwItemNum, pTokens );								// 아이템 수를 받는다.
			GetScriptParam( m_dwItemProbability, pTokens );						// 아이템 속성을 받는다.
		}
		break;

	case eQuestExecute_TakeQuestItemFQW:										// 퀘스트 특정 무기를 받는 실행이거나,
	case eQuestExecute_TakeQuestItemFW:											// 퀘스트 무기를 받는 실행이면,
		{
			GetScriptParam( m_dwItemKind, pTokens );							// 아이템 종류를 받는다.
			GetScriptParam( m_dwItemNum, pTokens );								// 아이템 수를 받는다.
			GetScriptParam( m_dwItemProbability, pTokens );						// 아이템 속성을 받는다.
			GetScriptParam( m_dwWeaponKind, pTokens );							// 무기 종류를 받는다.
		}
		break;
		
	case eQuestExecute_TakeMoneyPerCount:										// 카운트당 머니를 받는 실행이면,
		{
			GetScriptParam( m_dwItemKind, pTokens );							// 아이템 종류를 받는다.
			GetScriptParam( m_dwItemNum, pTokens );								// 아이템 수를 받는다.
		}
		break;
	}
}

// 소멸자 함수.
CQuestExecute_Item::~CQuestExecute_Item()										
{
}

// 실행 함수.
BOOL CQuestExecute_Item::Execute( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )
{
#ifdef _MAPSERVER_																// 맵 서버가 선언되어 있다면,

	switch( m_dwExecuteKind )													// 실행 종류를 확인한다.
	{
	case eQuestExecute_TakeQuestItem:											// 퀘스트 아이템을 받는 실행이면,
		{
			pQuestGroup->TakeQuestItem( pPlayer, m_dwQuestIdx, m_dwSubQuestIdx, // 퀘스트 그룹의 퀘스트 아이템을 받는 함수를 호출한다.
				m_dwItemKind, m_dwItemNum, m_dwItemProbability );
		}
		break;

	case eQuestExecute_GiveQuestItem:											// 퀘스트 아이템을 주는 실행이면,
		{
			//pQuestGroup->GiveQuestItem( pPlayer, m_dwQuestIdx, m_dwItemKind,	// 퀘스트 그룹의 퀘스트 아이템을 주는 함수를 호출한다.
			//	m_dwItemNum );

			if( !pQuestGroup->GiveQuestItem( pPlayer, m_dwQuestIdx, m_dwItemKind, m_dwItemNum )) return FALSE ;
		}
		break;

	case eQuestExecute_GiveItem:												// 아이템을 주는 실행이면,
		{
			//if( !pQuestGroup->GiveItem( pPlayer, m_dwItemKind, m_dwItemNum ) )	// 아이템 반납이 실패하면,
			//{
			//	return FALSE ;													// false를 리턴한다.
			//}

			if( !pQuestGroup->GiveItem( pPlayer, m_dwItemKind, m_dwItemNum, pQuest->GetQuestIdx()) )
			{
				return FALSE ;
			}
		}
		break;

	case eQuestExecute_GiveMoney:												// 머니를 주는 실행이면,
		{
			pQuestGroup->GiveMoney( pPlayer, m_dwItemNum );						// 퀘스트 그룹의 머니를 주는 함수를 호출한다.
		}
		break;

	case eQuestExecute_TakeItem:												// 아이템을 받는 실행이면,
		{
			return pQuestGroup->TakeItem( pPlayer, m_dwQuestIdx, m_dwSubQuestIdx, m_dwItemKind,			// 퀘스트 그룹의 아이템을 받는 함수를 호출하여 결과를 리턴한다.
				m_dwItemNum, m_dwItemProbability );
		}
		break;

	case eQuestExecute_TakeMoney:												// 머니를 받는 실행이면,
		{
			pQuestGroup->TakeMoney( pPlayer, m_dwItemNum );						// 퀘스트 그룹의 머니를 받는 함수를 호출한다.
		}
		break;

	case eQuestExecute_TakeExp:													// 경험치를 받는 실행이면,
		{
			pQuestGroup->TakeExp( pPlayer, m_dwItemNum );						// 퀘스트 그룹의 경험치를 받는 함수를 호출한다.
		}
		break;

	case eQuestExecute_TakeSExp:												// SP를 받는 실행이면,
		{
			pQuestGroup->TakeSExp( pPlayer, m_dwItemNum );						// 퀘스트 그룹의 SP를 받는 함수를 호출한다.
		}
		break;

	case eQuestExecute_TakeQuestItemFQW:										// 퀘스트 특정 무기를 받는 실행이면,
		{
			pQuestGroup->TakeQuestItemFromQWeapon( pPlayer, m_dwQuestIdx,		// 특정 무기로 부터 퀘스트 아이템을 받는 함수를 호출한다.
				m_dwSubQuestIdx, m_dwItemKind, m_dwItemNum, m_dwItemProbability, 
				m_dwWeaponKind );
		}
		break;
	case eQuestExecute_TakeMoneyPerCount:										// 카운트당 머니를 받는 실행이면,
		{
			pQuestGroup->TakeMoneyPerCount( pPlayer, m_dwQuestIdx, m_dwSubQuestIdx, m_dwItemKind, m_dwItemNum );	// 카운트 당 머니를 받는 함수를 호출한다.
		}
		break;
	}
#endif
	return TRUE;
}





int CQuestExecute_Item::CheckCondition( PLAYERTYPE* pPlayer, 
							CQuestGroup* pQuestGroup, CQuest* pQuest )			// 퀘스트 이벤트를 실행하기 위한 조건을 만족하는지 체크하는 함수.
{
	int nErrorCode = e_EXC_ERROR_NO_ERROR ;										// 에러 코드를 담을 변수를 선언하고 e_EXE_ERROR_NO_ERROR로 세팅한다.

	if( !pPlayer )																// 플레이어 정보가 유효하지 않으면, 
	{
		nErrorCode = e_EXC_ERROR_NO_PLAYERINFO ;								// 플레이어 정보가 없다는 에러 코드를 세팅한다.
		return nErrorCode ;														// 에러 코드를 return 처리를 한다.
	}

	if( !pQuestGroup )															// 퀘스트 그룹 정보가 유효하지 않으면,
	{
		nErrorCode = e_EXC_ERROR_NO_QUESTGROUP ;								// 퀘스트 그룹 정보가 없다는 에러 코드를 세팅한다.
		return nErrorCode ;														// 에러 코드를 return 처리를 한다.
	}

	if( !pQuest )																// 퀘스트 정보가 유효하지 않다면,
	{
		nErrorCode = e_EXC_ERROR_NO_QUEST ;										// 퀘스트 정보가 없다는 에러 코드를 세팅한다.
		return nErrorCode ;														// 에러 코드를 return 처리를 한다.
	}

#ifdef _MAPSERVER_																// 맵 서버가 선언되어 있다면,

	switch(m_dwExecuteKind)														// 실행 종류를 확인한다.
	{
	case eQuestExecute_TakeQuestItem :											// 퀘스트 아이템을 얻는 실행문이면,
		{
			return nErrorCode ;													// 에러 코드를 return 처리를 한다.

			// 현재 플레이어의 퀘스트 아이템 슬롯의 정보를 받아오는 부분이 
			// 없으므로, 우선은 에러 없음으로 처리를 하자.

			//CItemSlot* pSlot = NULL ;											// 아이템 슬롯 정보를 받을 포인터를 선언하고 null 처리를 한다.
			//pSlot = pPlayer->GetSlot(eItemTable_Inventory) ;					// 플레이어의 인벤토리 스롯 정보를 받는다.

			//if( !pSlot )														// 인벤토리 정보가 유효하지 않으면,
			//{
			//	nErrorCode = e_EXC_ERROR_NO_INVENTORYINFO ;						// 인벤토리 정보가 없다는 에러코드를 세팅한다.
			//	return nErrorCode ;												// 에러 코드를 return 처리를 한다.
			//}

			//WORD EmptyCellPos[255];											// 빈 슬롯 위치를 받을 변수를 선언한다.
			//WORD EmptyCellNum;												// 빈 슬롯 개수를 받을 변수를 선언한다.

			//WORD wResult = 0 ;												// 빈 슬롯의 체크 결과 값을 받을 변수를 선언하고 0으로 세팅한다.

			//wResult = ITEMMGR->GetCanBuyNumInSpace(pPlayer, pSlot,			// 아이템을 받을 공간이 충분한지 결과 값을 받는다.
			//	m_dwItemKind, m_dwItemNum, EmptyCellPos, EmptyCellNum) ;

			//if( wResult == 0 )												// 결과 값이 0과 같으면,
			//{
			//	nErrorCode = e_EXC_ERROR_NO_EXTRASLOT ;							// 에러 코드를 인벤토리 여분이 없다는 에러 코드를 세팅한다.
			//	return nErrorCode ;												// 에러 코드를 return 처리를 한다.
			//}
		}
		break ;

	case eQuestExecute_GiveQuestItem:											// 퀘스트 아이템을 반납 실행이면,
		{
			QUESTITEM* pQuestItem = NULL ;										// 퀘스트 아이템 정보를 받을 포인터를 선언하고 null 처리를 한다.
			pQuestItem = pQuestGroup->GetQuestItem(m_dwItemKind) ;				// 퀘스트 아이템 인덱스로 퀘스트 아이템 정보를 받는다.

			if( !pQuestItem )													// 퀘스트 아이템 정보가 유효하지 않다면,
			{
				nErrorCode = e_EXC_ERROR_NO_QUESTITEM ;							// 퀘스트 아이템이 없다는 에러 코드를 세팅한다.
				return nErrorCode ;												// 에러 코드를 return 처리를 한다.
			}

			if( pQuestItem->dwItemNum < m_dwItemNum )							// 아이템의 개수가, 목표 수량보다 작으면,
			{
				nErrorCode = e_EXC_ERROR_NOT_ENOUGH_QUESTITEM ;					// 퀘스트 아이템이 부족하다는 에러 코드를 세팅한다.
				return nErrorCode ;												// 에러 코드를 return 처리를 한다.
			}
		}
		break;

	case eQuestExecute_GiveItem:												// 아이템을 반납하는 실행이면,
		{
			int count = 0;
			DWORD nGiveItemCount = 0;

			ITEM_TOTALINFO ItemInfo;
			ZeroMemory(
				&ItemInfo,
				sizeof(ItemInfo));
			pPlayer->GetItemtotalInfo(
				ItemInfo,
				GETITEM_FLAG_INVENTORY | GETITEM_FLAG_WEAR );

			int nTotalInvenCount = (int)(SLOT_INVENTORY_NUM + pPlayer->GetInventoryExpansionSize());
			BOOL bStackItem = ITEMMGR->IsDupItem( m_dwItemKind ) ;

			for( count = 0 ; count < nTotalInvenCount ; ++count )				
			{
				if( ItemInfo.Inventory[count].wIconIdx == m_dwItemKind )		// 인자로 넘어온 아이템 인덱스와 같은 아이템이 있다면,
				{
					if( bStackItem )											// 스택아이템이면, 
					{
						nGiveItemCount += ItemInfo.Inventory[count].Durability ;// 반납 가능한 아이템 수를 추가한다.
					}
					else														// 일반아이템이면, 
					{
						++nGiveItemCount ;										// 반납 아이템 수를 증가한다.
					}
					
				}
			}

			for( count = 0 ; count < SLOT_WEAR_NUM ; ++count )					// 장비 창의 슬롯 수 만큼 for문을 돌린다.
			{
				if( ItemInfo.WearedItem[count].wIconIdx == m_dwItemKind )		// 인자로 넘어온 아이템 인덱스와 같은 아이템이 있다면,
				{
					if( bStackItem )											// 스택아이템이면, 
					{
						nGiveItemCount += ItemInfo.Inventory[count].Durability ;// 반납 가능한 아이템 수를 추가한다.
					}
					else														// 일반아이템이면, 
					{
						++nGiveItemCount ;										// 반납 아이템 수를 증가한다.
					}
				}
			}

			if( nGiveItemCount == 0 )											// 반납 가능한 아이템 수가 0개면,
			{
				nErrorCode = e_EXC_ERROR_NO_GIVEITEM ;							// 반납할 아이템이 없다는 에러를 세팅한다.
				return nErrorCode ;												// 에러 코드를 return 처리를 한다.
			}

			if( nGiveItemCount < m_dwItemNum )
			{
				nErrorCode = e_EXC_ERROR_NOT_ENOUGH_GIVEITEM ;					// 반납할 아이템이 부족하다는 에러를 세팅한다.
				return nErrorCode ;												// 에러 코드를 return 처리를 한다.
			}
		}
		break;

	case eQuestExecute_GiveMoney:												// 머니를 반납하는 실행이면,
		{
			MONEYTYPE curMoney = pPlayer->GetMoney() ;							// 플레이어의 현재 머니를 받는다.

			if( curMoney < m_dwItemNum )										// 현재 플레이어의 머니가 반납 할 머니보다 부족하면,
			{
				nErrorCode = e_EXC_ERROR_NOT_ENOUGH_GIVEMONEY ;					// 머니가 부족하다는 에러를 세팅한다.
				return nErrorCode ;												// 에러 코드를 return 처리를 한다.
			}
		}
		break;

	case eQuestExecute_TakeItem :
		{
			// 080515 LYW --- QuestExecute_Item : 아이템 획득 버그 수정.
			// 기존 체크 방식은, 트리거에 물려있는 실행 정보 하나를 각각 여분 슬롯을 체크 했기 때문에, 
			// 트리거 내에서, 여러개의 아이템을 줄때, (A, B, C, D)아이템을 하나씩 보상한다고 하면, 
			// 빈슬롯 하나만 있을경우, 여분 슬롯 체크가 성공되었다.
			// 지금은 트리거에 물린 실행 정보를 모드 체크하도록 수정.
			// 에러 코드 세팅.
			nErrorCode = e_EXC_ERROR_NO_EXTRASLOT ;



			// 퀘스트를 받는다.
			CQuest* pQuest = NULL ;
			pQuest = pQuestGroup->GetQuest(m_dwQuestIdx) ;

			if(!pQuest) return nErrorCode ;



			// 서브 퀘스트 정보를 받는다.
			CSubQuestInfo* pSubQuestInfo = NULL ;
			pSubQuestInfo = pQuest->GetSubQuestInfo(m_dwSubQuestIdx) ;

			if(!pSubQuestInfo) return nErrorCode ;

			cPtrList& pTriggerList = pSubQuestInfo->GetTriggerList() ;

			// 퀘스트 트리거 정보를 받는다.
			CQuestTrigger* pTrigger ;
			cPtrList* pExeList = NULL ;

			PTRLISTPOS triPos = pTriggerList.GetHeadPosition() ;
			while(triPos)
			{
				pTrigger = (CQuestTrigger*)pTriggerList.GetNext(triPos) ;

				if(!pTrigger) continue ;

				pExeList = pTrigger->GetExeList() ;
				break ;
			}

			if(!pExeList) return nErrorCode ;



			// 아이템 슬롯 정보를 받는다.
			CItemSlot* pSlot = NULL ;
			pSlot = pPlayer->GetSlot(eItemTable_Inventory) ;

			if( !pSlot )
			{
				nErrorCode = e_EXC_ERROR_NO_INVENTORYINFO ;
				return nErrorCode ;
			}



			// 비어있는 아이템 cell 카운트를 받는다.
			POSTYPE startPos = pSlot->GetStartPos() ;
			POSTYPE EndPos = startPos + pSlot->GetSlotNum() ;

			BYTE byEmptyCellCount = 0 ;
			for( WORD count = startPos ; count < EndPos ; ++count )
			{
				if(!pSlot->IsEmpty(count)) continue ;

				++byEmptyCellCount ;
			}



			// 퀘스트 실행 정보를 받는다.
			CQuestExecute* pExecute ;
			PTRLISTPOS exePos = pExeList->GetHeadPosition() ;

			BYTE byDifferrentItemCount = 0 ;
			while(exePos)
			{
				pExecute = NULL ;
				pExecute = (CQuestExecute*)pExeList->GetNext(exePos) ;

				if(!pExecute) continue ;

				if(pExecute->GetQuestExecuteKind() != eQuestExecute_TakeItem) continue ;

				// 아이템 정보를 받는다.
				DWORD dwItemIdx		= ((CQuestExecute_Item*)pExecute)->GetItemKind() ;
				DURTYPE dwItemNum	= ((CQuestExecute_Item*)pExecute)->GetItemNum() ;

				ITEM_INFO* ItemInfo = NULL ;
				ItemInfo = ITEMMGR->GetItemInfo( dwItemIdx ) ;

				if(!ItemInfo)
				{
					// 아이템 정보가 유효하지 않으면, 로그 남기기.
					char tempMsg[128] = {0,} ;
					sprintf(tempMsg, "------ %s ----- TakeItem Failed!!", __FUNCTION__) ;
					QUESTMGR->WriteLog(tempMsg) ;

					memset(tempMsg, 0, strlen(tempMsg)) ;
					sprintf(tempMsg, "QuestID:%d, SubQuestID:%d, ItemID:%d PlayerID:%d", m_dwQuestIdx, m_dwSubQuestIdx, dwItemIdx, pPlayer->GetID()) ;
					QUESTMGR->WriteLog(tempMsg) ;

					return nErrorCode ;
				}

				// 여분의 슬롯/같은 아이템 등을 체크한다.
				if(ITEMMGR->IsDupItem(dwItemIdx))
				{
					// 091215 ONS 아이템Stack최대수를 아이템별로 지정된 값으로 처리한다.
					const WORD wItemStackNum = ITEMMGR->GetItemStackNum( dwItemIdx );
					for( WORD count = startPos ; count < EndPos ; ++count )
					{
						if( dwItemNum == 0 ) break ;

						const ITEMBASE * pItemBase = pSlot->GetItemInfoAbs(count) ;

						if( !pSlot->IsEmpty(count) && !pSlot->IsLock(count) && pItemBase->wIconIdx == dwItemIdx && pItemBase->Durability < wItemStackNum )
						{
							if( pItemBase->Durability + dwItemNum > wItemStackNum )
							{
								dwItemNum = dwItemNum + pItemBase->Durability - wItemStackNum;
							}
							else
							{
								dwItemNum = 0 ;
							}
						}
					}

					for( WORD count = startPos ; count < EndPos ; ++count )
					{
						if( dwItemNum == 0 ) break;

						if(pSlot->IsEmpty(count))
						{
							if( dwItemNum > wItemStackNum )
							{
								dwItemNum -= wItemStackNum;
							}
						}
					}

					if(dwItemNum != 0) ++byDifferrentItemCount ;
				}
				else ++byDifferrentItemCount ;
			}



			// 아이템 분배가 가능한지 체크한다.
			if(byDifferrentItemCount > byEmptyCellCount) return nErrorCode ;
			else return e_EXC_ERROR_NO_ERROR;
		}
		break ;

	case eQuestExecute_TakeQuestItemFQW:										// 특정 무기를 체크해서 퀘스트 아이템을 받는 실행이면,
		{
			if( pPlayer->GetWearedWeapon() != m_dwWeaponKind )					// 플레이어가 착용하고 있는 무기와 비교대상 무기가 같지 않으면,
			{
				nErrorCode = e_EXC_ERROR_NOT_SAME_WEAPONKIND ;					// 무기 종류가 달라서 퀘스트 아이템을 받을 수 없다는 에러를 세팅한다.
				return nErrorCode ;												// 에러 코드를 return 처리를 한다.
			}
		}
		break;
	case eQuestExecute_TakeMoney:												// 머니를 지급받는 실행이면,
	case eQuestExecute_TakeMoneyPerCount:										// 카운트당 머니를 받는 실행이면,
		{
			MONEYTYPE curMoney = pPlayer->GetMoney() ;							// 플레이어의 현재 머니를 받는다.

			if( curMoney >= 4200000000 )
			{
				nErrorCode = e_EXC_ERROR_CANT_RECEIVE_MONEY ;					// 플레이어는 더이상 머니를 받을 수 없다는 에러 코드를 세팅한다.
				return nErrorCode ;													// 에러 코드를 return 처리를 한다.
			}
		}
	}

#endif //_MAPSERVER_

	return nErrorCode ;															// 기본 값을 리턴한다.
}











#include "stdafx.h"
#include "ConsignmentMgr.h"
#include "ConsignmentDlg.h"
#include "ChatManager.h"
#include "ItemManager.h"
#include "GameIn.h"
#include "InventoryExDialog.h"
#include "QuickManager.h"

GLOBALTON(CConsignmentMgr)

CConsignmentMgr::CConsignmentMgr(void)
{
}

CConsignmentMgr::~CConsignmentMgr(void)
{
}

void CConsignmentMgr::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	CConsignmentDlg* pDlg = GAMEIN->GetConsignmentDlg();
	if(! pDlg)
		return;

	switch(Protocol)
	{
	case MP_CONSIGNMENT_GETLIST_ACK:
		{
			MSG_CONSIGNMENT_SEARCH_RESULT* pmsg = (MSG_CONSIGNMENT_SEARCH_RESULT*)pMsg;
			pDlg->UpdateRegList(MAX_ITEMNUM_PER_PAGE, pmsg);
		}
		break;
	case MP_CONSIGNMENT_GETLIST_NACK:
		{
		}
		break;
	case MP_CONSIGNMENT_REGIST_ACK:
		{
			MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;

			pDlg->ClearControls(CConsignmentDlg::eConsignment_Mode_Regist);

			CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;
			if( !pInvenDlg )
				return ;

			// 아이템 정보를 받는다.
			CItem * pItem = NULL ;
			pItem = pInvenDlg->GetItemForPos( (POSTYPE)pmsg->dwData5 ) ;
			const ITEM_INFO* pItemInfo = pItem->GetItemInfo();

			ITEMMGR->DeleteItemofTable( ITEMMGR->GetTableIdxForAbsPos( (POSTYPE)pmsg->dwData5 ), (POSTYPE)pmsg->dwData5, TRUE ) ;
			QUICKMGR->DeleteLinkdedQuickItem( pItem->GetDBIdx() ) ;

			if( pInvenDlg->IsActive() ) pInvenDlg->SetDisable( FALSE ) ;
			QUICKMGR->RefreshQickItem() ;


			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2246), pItemInfo->ItemName );
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2247), pmsg->dwData4 );
			pDlg->SetHold(FALSE);

			// 등록했으니 리스트 새로요청하자.
			LoadRegistedList();
		}
		break;
	case MP_CONSIGNMENT_REGIST_NACK:
		{
			MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;

			switch(pmsg->dwData1)
			{
			case eConsignmentDBResult_RegistedOver:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2248) );
				}
				break;

			case eConsignmentERR_PoorMoney:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2283) );
				}
				break;

			default:
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2249) );
				break;
			}

			pDlg->SetHold(FALSE);
		}
		break;
	case MP_CONSIGNMENT_SEARCH_ACK:
		{
			MSG_CONSIGNMENT_SEARCH_RESULT* pmsg = (MSG_CONSIGNMENT_SEARCH_RESULT*)pMsg;

			pDlg->UpdateBuyList(MAX_ITEMNUM_PER_PAGE, pmsg->dwStartPage, pmsg->dwEndPage, pmsg);

			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2253), pmsg->dwCount );
			pDlg->SetHold(FALSE);
		}
		break;
	case MP_CONSIGNMENT_SEARCH_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			switch(pmsg->dwData)
			{
			case eConsignmentERR_FastRequest:
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2284) );
				break;

			case eConsignmentERR_WrongState:
				break;
			}
			pDlg->SetHold(FALSE);

#ifdef _GMTOOL_
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2257), pmsg->dwData );
#endif
		}
		break;
	case MP_CONSIGNMENT_UPDATE_ACK:
		{
			MSG_CONSIGNMENT_UPDATERESULT* pmsg = (MSG_CONSIGNMENT_UPDATERESULT*)pMsg;

			switch(pmsg->nResult)
			{
			case eConsignmentDBResult_UserCancel_Ack:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2250), pmsg->szItemName );

					if(pDlg->GetMode() == CConsignmentDlg::eConsignment_Mode_Regist)
						LoadRegistedList();
				}
				break;

			case eConsignmentDBResult_TimeCancel_Ack:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2251), pmsg->szItemName );

					if(pDlg->IsActive() && pDlg->GetMode() == CConsignmentDlg::eConsignment_Mode_Regist)
						LoadRegistedList();
				}
				break;

			case eConsignmentDBResult_Buy_Ack:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2252), pmsg->szItemName, pmsg->dwTotalPrice );

					if(pDlg->IsActive() && pDlg->GetMode() == CConsignmentDlg::eConsignment_Mode_Buy)
						pDlg->SendSearchMsg();
				}
				break;

			default:
				break;
			}
		}
		break;
	case MP_CONSIGNMENT_UPDATE_NACK:
		{
			MSG_CONSIGNMENT_UPDATERESULT* pmsg = (MSG_CONSIGNMENT_UPDATERESULT*)pMsg;

			switch(pmsg->nResult)
			{
			case eConsignmentERR_PoorMoney:
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1333) );
				break;

			default:
				break;
			}
#ifdef _GMTOOL_
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2258), pmsg->nResult );
#endif
		}
		break;
	}
}

void CConsignmentMgr::LoadRegistedList()
{
	MSG_DWORD msg;
	msg.Category = MP_CONSIGNMENT;
	msg.Protocol = MP_CONSIGNMENT_GETLIST_SYN;
	msg.dwObjectID = HEROID;
	NETWORK->Send( &msg, sizeof(msg) ) ;
}

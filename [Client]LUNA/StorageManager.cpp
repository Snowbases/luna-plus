// CStorageManager.cpp: implementation of the StorageManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StorageManager.h"
#include "GameIn.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "./interface/cWindowManager.h"
#include "WindowIdEnum.h"
#include "cDivideBox.h"

// 070326 LYW --- StorageManager : Include messagebox.
#include "cMsgBox.h"

// 070329 LYW --- NpcScriptManager : Include NpcImageDlg.
#include "NpcImageDlg.h"



#include "StorageDialog.h"
#include "CharacterDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CStorageManager);
CStorageManager::CStorageManager()
{
}

CStorageManager::~CStorageManager()
{

}

void CStorageManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol) 
	{
	case MP_STORAGE_BUY_ACK:
		{
			MSG_BYTE * pmsg = (MSG_BYTE*)pMsg;
			HERO->SetStorageNum(pmsg->bData);
			
			if(pmsg->bData == 1)
			{
				GAMEIN->GetStorageDialog()->SetItemInit(TRUE);
				GAMEIN->GetStorageDialog()->SetStorageMoney(0);
			}
		//	GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageWare1+pmsg->bData-1);
		//	GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageWare1+pmsg->bData-1);
					
			GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageListInfo);
			GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageListInfo);
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(532) );
		}
		break;
	case MP_STORAGE_PUTIN_MONEY_ACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			GAMEIN->GetStorageDialog()->SetStorageMoney(pmsg->dwData);
		}
		break;
	case MP_STORAGE_PUTOUT_MONEY_ACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			GAMEIN->GetStorageDialog()->SetStorageMoney(pmsg->dwData);
		}
		break;
	case MP_STORAGE_PUTOUT_MONEY_NACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1098 ), AddComma( MAX_INVENTORY_MONEY ) );
		}
		break;
		
		//////////////////////////////////////////////////////////////////////////
		//nack 
	case MP_STORAGE_BUY_NACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(533) );
		}
		break;
	case MP_STORAGE_PUTIN_MONEY_NACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(534) );
		}
		break;
	}
}

void CStorageManager::BuyStorageSyn()
{
	int rt = GAMEIN->GetStorageDialog()->CanBuyStorage();

	if( rt != 0 )
	{
		if( rt == 1 )	//µ·¾ø´Ù
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(533) );
		}
		else if( rt == 2 ) //Ã¢°í ²ËÃ¡´Ù
		{
			// 070325 LYW --- StarageManager : Modified message.
			//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(308) );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(535)  );
		}
		return;
	}

	MSGBASE msg;
	msg.Category = MP_STORAGE;
	msg.Protocol = MP_STORAGE_BUY_SYN;
	msg.dwObjectID = HEROID;
	
	NETWORK->Send(&msg, sizeof(msg));
}

void CStorageManager::DelStorageSyn(BYTE num)
{
	MSG_BYTE msg;
	msg.Category = MP_STORAGE;
	msg.Protocol = MP_STORAGE_DEL_SYN;
	msg.dwObjectID = HEROID;
	msg.bData = num;

	NETWORK->Send(&msg, sizeof(msg));
}

void CStorageManager::PutInOutMoney(int vals)
{	
	CStorageDialog* pStorageDlg = GAMEIN->GetStorageDialog();
	CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog();

	cStatic* pStorageMoney = (cStatic*)WINDOWMGR->GetWindowForIDEx(PYO_MONEY);

	if(vals)
	{
		WINDOWMGR->DivideBox( DBOX_INPUT_MONEY, (LONG)(pStorageMoney->GetAbsX()-50),
			(LONG)(pStorageMoney->GetAbsY()-100), OnPutInMoneyStorageOk, OnPutInMoneyStorageCancel,
			pInvenDlg, pStorageDlg, CHATMGR->GetChatMsg(536));
	}
	else
	{
		WINDOWMGR->DivideBox( DBOX_INPUT_MONEY, (LONG)(pStorageMoney->GetAbsX()-50),
			(LONG)(pStorageMoney->GetAbsY()-100), OnPutOutMoneyStorageOk, OnPutInMoneyStorageCancel, pInvenDlg,
			pStorageDlg, CHATMGR->GetChatMsg(537));
	}
}


void CStorageManager::OnPutInMoneyStorageOk( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
	if(param1 == 0)	return;

	MSG_DWORD msg;
	msg.Category = MP_STORAGE;
	msg.Protocol = MP_STORAGE_PUTIN_MONEY_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData = param1;
	
	NETWORK->Send(&msg, sizeof(msg));
}

void CStorageManager::OnPutInMoneyStorageCancel( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
}

void CStorageManager::PutInOutMoneyResult(MSG_DWORD3 * msg)
{
	if(HEROID == msg->dwData1)
	{
		HERO->SetMoney(msg->dwData2);
		GAMEIN->GetCharacterDialog()->UpdateData();
		
	}
//	GAMEIN->GetMunpaDialog()->SetMoney(msg->dwData3);
}

void CStorageManager::OnPutOutMoneyStorageOk( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
	if(param1 == 0)	return;

	MSG_DWORD msg;
	msg.Category = MP_STORAGE;
	msg.Protocol = MP_STORAGE_PUTOUT_MONEY_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData = param1;
	
	NETWORK->Send(&msg, sizeof(msg));
}

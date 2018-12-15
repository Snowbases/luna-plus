// NoteManager.cpp: implementation of the CNoteManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NoteManager.h"
#include "MiniNoteDialog.h"
#include "MiniNoteDialogRead.h"
#include "NoteDialog.h"
#include "GameIn.h"
// 061220 LYW --- Delete this code.
//#include "MainBarDialog.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include "ItemManager.h"
#include "InventoryExDialog.h"
#include "QuickManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//GLOBALTON(CNoteManager)
CNoteManager::CNoteManager()
{

}

CNoteManager::~CNoteManager()
{

}

void CNoteManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
		case MP_NOTE_RECEIVENOTE:
		{
			if(GAMEIN->GetNoteDialog()->IsActive() == TRUE)
			{
				NoteListSyn(GAMEIN->GetNoteDialog()->GetSelectedNotePge());
			}
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(933));						
		}
		break;
	case MP_NOTE_NEW_NOTE:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(934));
			if(GAMEIN->GetNoteDialog()->IsActive() == TRUE)
				NoteListSyn(1);
		}
		break;
	case MP_NOTE_SENDNOTE_ACK:
		{
			MSG_NAME *pmsg = (MSG_NAME*) pMsg;
			GAMEIN->GetMiniNoteDialogWrite()->SetActive(FALSE);
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(935), pmsg->Name );
		}
		break;
	case MP_NOTE_NOTELIST_ACK:
		{
			MSG_FRIEND_NOTE_LIST* pmsg = (MSG_FRIEND_NOTE_LIST*) pMsg;
			GAMEIN->GetNoteDialog()->SetNoteList(pmsg);
		}
		break;
	case MP_NOTE_DELNOTE_ACK:
		{
			MSG_FRIEND_DEL_NOTE* pmsg = (MSG_FRIEND_DEL_NOTE*)pMsg;
			if(pmsg->bLast)
			{
				NoteListSyn(GAMEIN->GetNoteDialog()->GetSelectedNotePge(), GAMEIN->GetNoteDialog()->GetMode());
			}
			
			if(pmsg->NoteID == GAMEIN->GetMiniNoteDialogRead()->GetNoteID())
			{
				GAMEIN->GetMiniNoteDialogRead()->SetActive(FALSE);
			}

			GAMEIN->GetMiniNoteDialogWrite()->SetActive(FALSE);
		}
		break;
	case MP_NOTE_SAVENOTE_ACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			if(pmsg->dwData2)
			{
				NoteListSyn(GAMEIN->GetNoteDialog()->GetSelectedNotePge(), GAMEIN->GetNoteDialog()->GetMode());
			}
			GAMEIN->GetMiniNoteDialogWrite()->SetActive(FALSE);
		}
		break;
	case MP_NOTE_READNOTE_ACK:
		{
			MSG_FRIEND_READ_NOTE * pmsg = (MSG_FRIEND_READ_NOTE*)pMsg;

			{
				CMiniNoteDialogRead* dialog = GAMEIN->GetMiniNoteDialogRead();
				ASSERT( dialog );

				// desc_hseos_농장시스템_01
				// S 농장시스템 추가 added by hseos 2008.01.16
				if (stricmp(pmsg->FromName, "<SYSTEM>") == 0)
				{
					int nMsgID = atoi(pmsg->Note);
					if (nMsgID && 
						CHATMGR->GetChatMsg(nMsgID))
					{
						
						SafeStrCpy(pmsg->Note, CHATMGR->GetChatMsg(nMsgID), MAX_NOTE_LENGTH - 1);
					}
				}
				// E 농장시스템 추가 added by hseos 2008.01.16

				char* pSendDate = NULL;
				char* pTitle = NULL;
				CNoteDialog* noteDialog = GAMEIN->GetNoteDialog();
				if(noteDialog)
				{
					pSendDate = noteDialog->GetSendDate(pmsg->NoteID);
					pTitle = noteDialog->GetTitle(pmsg->NoteID);
				}

				dialog->SetMiniNote(pTitle, pSendDate, pmsg->FromName, pmsg->Note, pmsg->ItemIdx);
				dialog->SetPackageInfo(pmsg->dwPackageMoney, NULL, NULL);
				dialog->SetNoteID(pmsg->NoteID);
				dialog->SetActive(TRUE);

				noteDialog->Refresh( pmsg->NoteID );
			}
		}
		break;
	case MP_NOTE_READNOTE_WITH_PACKAGE_ACK:
		{
			MSG_FRIEND_READ_NOTE_WITH_PACKAGE * pmsg = (MSG_FRIEND_READ_NOTE_WITH_PACKAGE*)pMsg;

			CMiniNoteDialogRead* dialog = GAMEIN->GetMiniNoteDialogRead();
			ASSERT( dialog );

			char* pSendDate = NULL;
			char* pTitle = NULL;
			CNoteDialog* noteDialog = GAMEIN->GetNoteDialog();
			if(noteDialog)
			{
				pSendDate = noteDialog->GetSendDate(pmsg->NoteID);
				pTitle = noteDialog->GetTitle(pmsg->NoteID);
			}

			dialog->SetMiniNote(pTitle, pSendDate, pmsg->FromName, pmsg->Note, pmsg->ItemIdx);
			dialog->SetPackageInfo(pmsg->PackageMoney, &pmsg->ItemInfo, &pmsg->OptionInfo);
			dialog->SetNoteID(pmsg->NoteID);
			dialog->SetActive(TRUE);

			noteDialog->Refresh( pmsg->NoteID );
		}
		break;
		
	case MP_NOTE_GETPACKAGE_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			switch(pmsg->dwData)
			{
			case eConsignmentERR_PoorSlot:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1520) );
				}
				break;

			case eConsignmentERR_GoldOver:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(475) );
				}
				break;
			}

		}
		break;

	case MP_NOTE_GETPACKAGE_ACK:
		{
			ITEMOBTAINARRAY * pmsg = (ITEMOBTAINARRAY *)pMsg;												// 아이템 획득 정보를 담을 포인터를 선언하고 메시지를 받는다.

			CItem* pItem = NULL;																			// 아이템 정보를 담을 포인터를 선언하고 null처리를 한다.

			for(WORD i = 0; i < pmsg->ItemNum; ++i )
			{
				ITEMBASE& itemBase = pmsg->ItemInfo[ i ];
				pItem = GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->GetItem(i)->Position);

				ITEMBASE prevItemBase;
				ZeroMemory( &prevItemBase, sizeof( prevItemBase ) );

				if(pItem)
				{
					if( pItem->GetDBIdx() == itemBase.dwDBIdx )
					{
						prevItemBase = pItem->GetItemBaseInfo();
					}

					pItem->SetItemBaseInfo( itemBase );												// 아이템 기본 정보를 세팅한다.
				}
				else																						// 아이템 정보가 유효하지 않으면,
				{
					pItem = ITEMMGR->MakeNewItem(pmsg->GetItem(i),"MP_NOTE_GETPACKAGE_ACK");					// 새로 아이템을 생성해 포인터로 받는다.
					
					const ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() ) ;
					if( !pInfo ) continue;

					if( pInfo->wSeal == eITEM_TYPE_SEAL )
					{
						itemBase.nSealed = eITEM_TYPE_SEAL ;
						itemBase.nRemainSecond = pInfo->dwUseTime ;

						pItem->SetItemBaseInfo( itemBase ) ;
					}

					GAMEIN->GetInventoryDialog()->AddItem(pItem);
				}
			}

			if( pItem )
			{
				ITEMMGR->ItemDropEffect( pItem->GetItemIdx() );
			}

			QUICKMGR->RefreshQickItem();
		}
		break;

	case MP_NOTE_GETPACKAGE_NOTIFY:
		{
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
			const ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( pmsg->dwData3 ) ;
			if(pInfo)
			{
				if( pInfo->Stack )
					CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 1112 ), pInfo->ItemName, pmsg->dwData4 );
				else
					CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 101 ), pInfo->ItemName );
			}

			DWORD dwNoteID = 0;
			CMiniNoteDialogRead* pReadDlg = GAMEIN->GetMiniNoteDialogRead();
			if(pReadDlg)
			{
				dwNoteID = pReadDlg->GetNoteID();
				pReadDlg->SetPackageInfo(0, NULL, NULL);
			}

			CNoteDialog* pNoteDlg = GAMEIN->GetNoteDialog();
			if(pNoteDlg)
			{
				pNoteDlg->ClearPackageInfo(dwNoteID);
				pNoteDlg->Refresh(dwNoteID);
			}
		}
		break;
		
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//nack 	
	case MP_NOTE_SENDNOTE_NACK:
		{
			MSG_NAME_WORD * pmsg = (MSG_NAME_WORD *)pMsg;
			if(pmsg->wData == 2) //invalid user
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(936) );
			else if(pmsg->wData == 3) //full list
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(937), pmsg->Name );
		}
		break;
	}

}

void CNoteManager::WriteNote(char* Note, char* Title, char* ToName)
{
	MSG_FRIEND_SEND_NOTE msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_SENDNOTE_SYN;
	msg.FromId = HEROID;
	SafeStrCpy(msg.ToName, ToName, MAX_NAME_LENGTH+1);
	SafeStrCpy(msg.FromName, HERO->GetObjectName(), MAX_NAME_LENGTH+1);
	SafeStrCpy(msg.Title, Title, MAX_NOTE_TITLE - 1);
	SafeStrCpy(msg.Note, Note, MAX_NOTE_LENGTH - 1);	
	
	
	NETWORK->Send(&msg, msg.GetMsgLength());
}

void CNoteManager::WriteNoteID(char* Note, DWORD TargetID)
{
	MSG_FRIEND_SEND_NOTE_ID msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_SENDNOTEID_SYN;
	msg.dwObjectID = HEROID;
	SafeStrCpy(msg.FromName, HERO->GetObjectName(), MAX_NAME_LENGTH+1);
	msg.TargetID = TargetID;
	SafeStrCpy(msg.Note, Note, MAX_NOTE_LENGTH - 1);

	NETWORK->Send(&msg, msg.GetMsgLength());
}

void CNoteManager::DelNoteSyn(DWORD NoteID, BOOL bLast)
{
	CNoteDialog* pNoteDlg = GAMEIN->GetNoteDialog();
	if(! pNoteDlg)
		return;

	if(pNoteDlg->IsPackageNote(NoteID))
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2281));
		return;
	}

	MSG_FRIEND_DEL_NOTE msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_DELNOTE_SYN;
	msg.NoteID = NoteID;
	msg.dwObjectID = HEROID;
	msg.bLast = bLast;

	NETWORK->Send(&msg, sizeof(msg));
}

void CNoteManager::SaveNoteSyn(DWORD NoteID, BOOL bLast)
{
	MSG_DWORD2 msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_SAVENOTE_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = NoteID;
	msg.dwData2 = bLast;

	NETWORK->Send(&msg, sizeof(msg));
}

void CNoteManager::DelAllNoteSyn()
{
	MSGBASE msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_DELALLNOTE_SYN;
	msg.dwObjectID = HEROID;
	NETWORK->Send(&msg, sizeof(msg));
}

void CNoteManager::NoteListSyn(WORD SelectedPage, WORD Mode)
{
	if(HERO->IsInited() == FALSE)
		return;
	if(HEROID > 10000000)
	{
		char buf[256];
		sprintf(buf, "HeroId: %d, HeroName: %s, HeroSim: %d", HEROID, HERO->GetObjectName(), HERO->GetWisdom());
		ASSERTMSG(0, buf);
	}
	MSG_WORD2 msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_NOTELIST_SYN;
	msg.wData1 = SelectedPage;
	msg.wData2 = Mode;
	msg.dwObjectID = HEROID;

	NETWORK->Send(&msg, sizeof(msg));

	GAMEIN->GetNoteDialog()->SetSelectedNotePge(SelectedPage);
}

void CNoteManager::ReadNote(DWORD NoteID, BOOL IsFront)
{
	MSG_DWORD2 msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_READNOTE_SYN;
	msg.dwData1 = NoteID;
	msg.dwData2 = IsFront;
	msg.dwObjectID = HEROID;

	NETWORK->Send(&msg, sizeof(msg));
}
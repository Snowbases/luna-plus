#include "stdafx.h"
#include ".\mininotedialogread.h"

#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"
#include "cImeEx.h"
#include ".\interface\cTextArea.h"
#include ".\interface\cEditBox.h"
#include ".\interface\cStatic.h"
#include ".\interface\cButton.h"
#include ".\interface\cIconDialog.h"
#include "Interface/cScriptManager.h"

#include "ItemManager.h"
#include "ChatManager.h"
#include "ConsignmentDlg.h"
#include "NoteDialog.h"
#include "GameIn.h"

CMiniNoteDialogRead::CMiniNoteDialogRead(void)
{
	m_SelectedNoteID = 0;
	m_pItemPackage = NULL;
}

CMiniNoteDialogRead::~CMiniNoteDialogRead(void)
{
	m_MinNoteCtlListArray.RemoveAll();
	SAFE_DELETE( m_pItemPackage );
}

void CMiniNoteDialogRead::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
}

void CMiniNoteDialogRead::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	PTRLISTSEARCHSTART(m_MinNoteCtlListArray, cWindow*,pWin)
	pWin->SetActive(val);
	PTRLISTSEARCHEND

	cDialog::SetActive(val);
}

void CMiniNoteDialogRead::Linking()
{
	m_pRTitleStc	= (cStatic*)GetWindowForID(NOTE_MRTITLE);	//제목

	//읽기 
	m_pRNoteText = (cTextArea*)GetWindowForID(NOTE_MRNOTETEXTREA);
	m_pRNoteText->SetEnterAllow(FALSE);
	m_pSendDateStc =  (cStatic*)GetWindowForID(NOTE_MSENDEDATE);
	m_pSenderStc = (cStatic*)GetWindowForID(NOTE_MSENDERSTC);
	m_pPackageMoenyStc = (cStatic*)GetWindowForID(NOTE_MPACKAGEMONEY);
	m_pReplayBtn = (cButton*)GetWindowForID(NOTE_MREPLYBTN);
	m_pGetPackageBtn = (cButton*)GetWindowForID(NOTE_MGETPACKAGEBTN);
	m_pDeleteBtn = (cButton*)GetWindowForID(NOTE_MDELETEBTN);
	m_pItemPackageDlg = (cIconDialog*)GetWindowForID(NOTE_ITEM_PACKAGE);
	m_pItemPackageDlg->SetDisable(TRUE);

	m_MinNoteCtlListArray.AddTail(m_pRTitleStc);
	m_MinNoteCtlListArray.AddTail(m_pRNoteText);
	m_MinNoteCtlListArray.AddTail(m_pSendDateStc);
	m_MinNoteCtlListArray.AddTail(m_pSenderStc);
	m_MinNoteCtlListArray.AddTail(m_pReplayBtn);
	m_MinNoteCtlListArray.AddTail(m_pDeleteBtn);
	m_MinNoteCtlListArray.AddTail(m_pItemPackageDlg);

	cImage imgToolTip;
	SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );

	ITEMBASE base;
	ZeroMemory( &base, sizeof( base ) );
	m_pItemPackage = new CItem( &base );
	m_pItemPackage->SetToolTip( "", RGB_HALF( 255, 255, 255), &imgToolTip );
	m_pItemPackage->SetMovable(FALSE);
}

void CMiniNoteDialogRead::SetMiniNote(char* Title, char* Date, char* Sender, char* Note, WORD ItemIdx)
{
	char buf[MAX_NOTE_LENGTH] = { 0, };	
	
	if( ItemIdx > 0 )
	{
		ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( ItemIdx );

		if( pItemInfo )
			sprintf( buf, CHATMGR->GetChatMsg(938), pItemInfo->ItemName );
	}

	strcat( buf, Note );

	CNoteDialog* noteDialog = GAMEIN->GetNoteDialog();
	if(! noteDialog)
		return;

	char* pTitle = noteDialog->GetCovertedTitleFromFormNote(Title);
	if(pTitle)
		m_pRTitleStc->SetStaticText(pTitle);
	else
		m_pRTitleStc->SetStaticText(Title);

	m_pSendDateStc->SetStaticText(Date);
	
	char szPreFix[16] = {0,};
	strncpy(szPreFix, Sender, 4);
	if( stricmp(Sender, "<CONSIGN>") == 0 )
	{
		m_pSenderStc->SetStaticText(CHATMGR->GetChatMsg(2236));

		// 판매대행 영수증처리 양식에 따른 조작이 필요함.
		char tokens[32] = {","};
		char* pBuf = Note;

		char name[256];
		DWORD kind, initNum, curNum, deposit, commission, price;

		kind		= atoi( strtok(pBuf, tokens) );
		strcpy(name, strtok(NULL, ","));
		initNum		= atoi( strtok(NULL, ",") );
		curNum		= atoi( strtok(NULL, ",") );
		deposit		= atoi( strtok(NULL, ",") );
		commission	= atoi( strtok(NULL, ",") );
		price		= atoi( strtok(NULL, ",") );


		char buf2[256] = {0,};
		strcpy(buf, "");

		switch(kind)
		{
		case CConsignmentDlg::eConsignment_Reciept_UserCancel:
			{
				sprintf(buf2, CHATMGR->GetChatMsg(2271));
				strcat(buf, buf2);

				if(initNum && initNum!=curNum)
				{
					// 2265	"%s %d개가 판매 되었습니다.^n"
					sprintf(buf2, CHATMGR->GetChatMsg(2265), name, initNum - curNum);
					strcat(buf, buf2);
				}
				// 2266	"%s %d개가 판매 취소되었습니다.^n"
				sprintf(buf2, CHATMGR->GetChatMsg(2266), name, initNum ? curNum : 1);
				strcat(buf, buf2);

				if(initNum-curNum > 0)
				{
					// 2268	"판매액 = %d * %d = +%d골드^n"
					sprintf(buf2, CHATMGR->GetChatMsg(2268), price, initNum ? initNum-curNum : 0, price*(initNum ? initNum-curNum : 0));
					strcat(buf, buf2);

					// 2269	"수수료 = %d * %d = %s%d골드^n"
					sprintf(buf2, CHATMGR->GetChatMsg(2270), commission, initNum ? initNum-curNum : 0, commission*(initNum ? initNum-curNum : 0));
					strcat(buf, buf2);
				}

				// 2269	"보증금 = %s%d골드^n"
				sprintf(buf2, CHATMGR->GetChatMsg(2269), 0);
				strcat(buf, buf2);
			}
			break;
		case CConsignmentDlg::eConsignment_Reciept_TimeCancel:
			{
				sprintf(buf2, CHATMGR->GetChatMsg(2272));
				strcat(buf, buf2);

				if(initNum && initNum!=curNum)
				{
					// 2265	"%s %d개가 판매 되었습니다.^n"
					sprintf(buf2, CHATMGR->GetChatMsg(2265), name, initNum - curNum);
					strcat(buf, buf2);
				}
				// 2267	"%s %d개가 반송 되었습니다.^n"
				sprintf(buf2, CHATMGR->GetChatMsg(2267), name, initNum ? curNum : 1);
				strcat(buf, buf2);

				if(initNum-curNum > 0)
				{
					// 2268	"판매액 = %d * %d = +%d골드^n"
					sprintf(buf2, CHATMGR->GetChatMsg(2268), price, initNum ? initNum-curNum : 0, price*(initNum ? initNum-curNum : 0));
					strcat(buf, buf2);

					// 2269	"수수료 = %d * %d = %s%d골드^n"
					sprintf(buf2, CHATMGR->GetChatMsg(2270), commission, initNum ? initNum-curNum : 0, commission*(initNum ? initNum-curNum : 0));
					strcat(buf, buf2);
				}

				// 2269	"보증금 = %s%d골드^n"
				sprintf(buf2, CHATMGR->GetChatMsg(2269), deposit);
				strcat(buf, buf2);
			}
			break;
		case CConsignmentDlg::eConsignment_Reciept_Buy:
			{
				sprintf(buf2, CHATMGR->GetChatMsg(2273));
				strcat(buf, buf2);

				// ????	"%s %d개를 구입 하였습니다.^n"
				sprintf(buf2, CHATMGR->GetChatMsg(2274), name, curNum ? curNum : 1);
				strcat(buf, buf2);

				// ???	"구입비 = %d.^n"
				sprintf(buf2, CHATMGR->GetChatMsg(2275), curNum ? price*curNum : price);
				strcat(buf, buf2);
			}
			break;
		case CConsignmentDlg::eConsignment_Reciept_SoldOut:
			{
				sprintf(buf2, CHATMGR->GetChatMsg(2276));
				strcat(buf, buf2);

				if( initNum )
				{
					// 2265	"%s %d개가 판매 되었습니다.^n"
					sprintf(buf2, CHATMGR->GetChatMsg(2265), name, initNum);
					strcat(buf, buf2);
				}
				else
				{
					// 2265	"%s %d개가 판매 되었습니다.^n"
					sprintf(buf2, CHATMGR->GetChatMsg(2265), name, 1);
					strcat(buf, buf2);
				}

				// 2268	"판매액 = %d * %d = +%d골드^n"
				sprintf(buf2, CHATMGR->GetChatMsg(2268), price, initNum ? initNum : 1, price*(initNum ? initNum : 1));
				strcat(buf, buf2);

				// 2269	"보증금 = %s%d골드^n"
				sprintf(buf2, CHATMGR->GetChatMsg(2269), deposit);
				strcat(buf, buf2);

				// 2269	"수수료 = %d * %d = %s%d골드^n"
				sprintf(buf2, CHATMGR->GetChatMsg(2270), commission, initNum ? initNum : 1, commission*(initNum ? initNum : 1));
				strcat(buf, buf2);
			}
			break;
		}

		m_pRNoteText->SetEnterAllow(TRUE);
	}
	else if(0 == strcmp("<NPC", szPreFix))
	{
		// NPC로부터 온 양식있는 우편이다.
		// 파싱해서 본문을 작성하자.
		char tokens[32] = {","};
		char* pBuf = Note;

		DWORD kind = 0;
		DWORD SysMsgID = 0;
		if(strlen(pBuf) > 2)
		{
			kind		= atoi( strtok(pBuf, tokens) );
			SysMsgID	= atoi( strtok(NULL, ",") );

			switch(kind)
			{
			case eNoteParsing_FishingLevelUp:
			case eNoteParsing_CookLevelUp:
				strcpy(buf, CHATMGR->GetChatMsg(SysMsgID));
				break;
			}
		}

		// 이름세팅
		char* pName = NULL;
		CNoteDialog* noteDialog = GAMEIN->GetNoteDialog();
		if(! noteDialog)
			return;

		pName = noteDialog->GetCovertedNameFromFormNote(Sender);
		if(pName)
			m_pSenderStc->SetStaticText(pName);
		else
			m_pSenderStc->SetStaticText(Sender);
	}
	else
	{
		char* pName = NULL;
		CNoteDialog* noteDialog = GAMEIN->GetNoteDialog();
		if(! noteDialog)
			return;

		pName = noteDialog->GetCovertedNameFromFormNote(Sender);
		if(pName)
			m_pSenderStc->SetStaticText(pName);
		else
			m_pSenderStc->SetStaticText(Sender);
	}
	
	m_pRNoteText->SetCaretMoveFirst();
	m_pRNoteText->SetScriptText(buf);
	m_pRNoteText->SetEnterAllow(FALSE);
	m_pRNoteText->SetCaretMoveFirst();
	m_pItemPackageDlg->DeleteIcon(0, 0);
}

void CMiniNoteDialogRead::SetPackageInfo(DWORD dwMoney, ITEMBASE* pBaseInfo, ITEM_OPTION* pOptionInfo)
{
	m_dwPackageMoney = 0;
	m_pPackageMoenyStc->SetStaticText("0");
	m_pItemPackage->SetData(0);

	ITEMBASE emptyItem;
	ZeroMemory(&emptyItem, sizeof(emptyItem));
	m_pItemPackage->SetItemBaseInfo(emptyItem);
	m_pItemPackageDlg->DeleteIcon(0, NULL);
	m_pGetPackageBtn->SetDisable(TRUE);

	if(dwMoney)
	{
		m_dwPackageMoney = dwMoney;

		DWORD color = GetMoneyColor(dwMoney);
		m_pPackageMoenyStc->SetStaticText(AddComma(dwMoney));
		m_pPackageMoenyStc->SetFGColor(color);
	}

	if( pBaseInfo && pOptionInfo && m_pItemPackage )
	{
		ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( pBaseInfo->wIconIdx );
		if( !pItemInfo )
			return;	

		cImage image;
		m_pItemPackage->Init( 0, 0, DEFAULT_ICONSIZE, DEFAULT_ICONSIZE, ITEMMGR->GetIconImage( pItemInfo->ItemIdx, &image ), 0 );
		m_pItemPackage->SetItemBaseInfo( *pBaseInfo );
		ITEMMGR->AddOption(*pOptionInfo);
		m_pItemPackage->SetIconType( eIconType_AllItem );
		m_pItemPackage->SetData( pItemInfo->ItemIdx );

		ITEMMGR->AddToolTip( m_pItemPackage );
		m_pItemPackageDlg->AddIcon( 0, m_pItemPackage );
	}

	if(m_pItemPackage || m_dwPackageMoney)
		m_pGetPackageBtn->SetDisable(FALSE);
}

const char* CMiniNoteDialogRead::GetSenderName() const
{
	return m_pSenderStc->GetStaticText();
}

void CMiniNoteDialogRead::SendGetPackageMsg()
{
	DWORD dwPackageItemIndex = m_pItemPackage->GetItemBaseInfo().wIconIdx;
	DWORD dwPackageItemDBIdx = m_pItemPackage->GetItemBaseInfo().dwDBIdx;
	DWORD dwDurabiliry = m_pItemPackage->GetItemBaseInfo().Durability;
	if( !dwPackageItemDBIdx && !m_dwPackageMoney )
		return;

	// 소포획득
	MSG_DWORD5 msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_GETPACKAGE_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = GetNoteID();			// NoteID
	msg.dwData2 = dwPackageItemIndex;	// ItemIndex
	msg.dwData3 = dwPackageItemDBIdx;	// ItemDBIndex
	msg.dwData4 = dwDurabiliry;			// Durability
	msg.dwData5 = m_dwPackageMoney;		// Money
	NETWORK->Send(&msg, sizeof(msg));
}
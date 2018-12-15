#include "stdafx.h"
#include "ShoutDialog.h"
#include "WindowIDEnum.h"
#include ".\interface\cTextArea.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "FilteringTable.h"
#include "Item.h"

CShoutDialog::CShoutDialog()
{
	m_type = WT_SHOUT_DLG;
	m_pItem = NULL;
}

CShoutDialog::~CShoutDialog()
{}

void CShoutDialog::Linking()
{
	m_pMsgText = (cTextArea*)GetWindowForID(SHOUT_TEXTAREA_MSG);
	m_pMsgText->SetEnterAllow(FALSE);
	m_pMsgText->SetScriptText("");
}

void CShoutDialog::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(val == TRUE)
	{
		m_pMsgText->SetScriptText("");
		m_pMsgText->SetFocusEdit(TRUE);
	}
	else
	{
		m_pItem->SetLock(FALSE);
		m_pItem = NULL;
		m_pMsgText->SetScriptText("");
		m_pMsgText->SetFocusEdit(FALSE);
	}
	cDialog::SetActive(val);
}

void CShoutDialog::OnActionEvent(LONG windowIndex, LPVOID windowControl, DWORD windowEvent)
{
	switch(windowIndex)
	{
	case SHOUT_BTN_OK:
		{
			char szFullText[MAX_PATH] = {0};

			if(m_pMsgText)
			{
				m_pMsgText->GetScriptText(szFullText);
			}

			if(0 == _tcslen(szFullText))
			{
				break;
			}
			else if( FILTERTABLE->FilterChat(szFullText))
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(1225));
				break;
			}

			SEND_SHOUTBASE_ITEMINFO message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_ITEM;
			message.Protocol = MP_ITEM_SHOPITEM_SHOUT_SYN;
			message.dwObjectID = gHeroID;
			message.dwItemIdx = m_pItem->GetItemIdx();
			message.dwItemPos = POSTYPE(m_pItem->GetPosition());
			SafeStrCpy(
				message.Name,
				HERO->GetObjectName(),
				_countof(message.Name));

			WCHAR wideTextBuffer[MAX_PATH] = {0};
			mbstowcs(
				wideTextBuffer,
				szFullText,
				_countof(wideTextBuffer));
			wcstombs(
				message.Msg,
				wideTextBuffer,
				_countof(message.Msg));

			NETWORK->Send(
				&message,
				message.GetMsgLength());
			SetActive(
				FALSE);
			break;
		}
	case SHOUT_BTN_CANCEL:
		{
			SetActive(
				FALSE);
			break;
		}
	}
}
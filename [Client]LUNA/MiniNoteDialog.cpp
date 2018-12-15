// MiniNoteDialog.cpp: implementation of the CMiniNoteDialogWrite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiniNoteDialog.h"
#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"
#include "cImeEx.h"
#include ".\interface\cTextArea.h"
#include ".\interface\cEditBox.h"
#include ".\interface\cStatic.h"

#include "ItemManager.h"
#include "ChatManager.h"

#include "MHFile.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMiniNoteDialogWrite::CMiniNoteDialogWrite()
{
	m_type = WT_MININOTEDLG;
	m_CurMiniNoteMode = -1;
}

CMiniNoteDialogWrite::~CMiniNoteDialogWrite()
{
	m_MinNoteCtlListArray.RemoveAll();
}

void CMiniNoteDialogWrite::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	m_type = WT_MININOTEDLG;
}

void CMiniNoteDialogWrite::Linking()
{
	m_pTitleEdit = (cEditBox*)GetWindowForID(NOTE_MWTITLE);
	m_pTitleEdit->SetEditText("");

	//쓰기
	m_pWNoteText = (cTextArea*)GetWindowForID(NOTE_MWNOTETEXTREA);
	m_pWNoteText->SetEnterAllow(FALSE);
	m_pReceiver = (cStatic*)GetWindowForID(NOTE_MRECEIVER); //받는이 :
	

	m_pMReceiverEdit = (cEditBox*)GetWindowForID(NOTE_MRECEIVEREDIT); //아무개
	m_pMReceiverEdit->SetValidCheck( VCM_CHARNAME );
	m_pMReceiverEdit->SetEditText("");
	
	m_pSendOkBtn = (cButton*)GetWindowForID(NOTE_MSENDOKBTN);
	
	m_MinNoteCtlListArray.AddTail(m_pTitleEdit);
	m_MinNoteCtlListArray.AddTail(m_pWNoteText);
	m_MinNoteCtlListArray.AddTail(m_pMReceiverEdit);
	m_MinNoteCtlListArray.AddTail(m_pSendOkBtn);
	m_MinNoteCtlListArray.AddTail(m_pReceiver);
}

void CMiniNoteDialogWrite::SetMiniNote(char* Sender, char* Note, WORD ItemIdx)
{
	char buf[300] = { 0, };	
	
	if( ItemIdx > 0 )
	{
		ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( ItemIdx );

		if( pItemInfo )
			sprintf( buf, CHATMGR->GetChatMsg(938), pItemInfo->ItemName );
	}

	strcat( buf, Note );
	m_pMReceiverEdit->SetEditText(Sender);
	m_pWNoteText->SetScriptText(buf);
}


int CMiniNoteDialogWrite::GetCurMode()
{
	return m_CurMiniNoteMode;
}

void CMiniNoteDialogWrite::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(val == TRUE)
	{
		m_pTitleEdit->SetEditText("");
		m_pWNoteText->SetScriptText("");
		m_pMReceiverEdit->SetEditText("");
	}
	else
	{
		m_pMReceiverEdit->SetFocusEdit(FALSE);
		m_pWNoteText->SetFocusEdit(FALSE);
	}

	PTRLISTSEARCHSTART(m_MinNoteCtlListArray, cWindow*,pWin)
	pWin->SetActive(val);
	PTRLISTSEARCHEND

	cDialog::SetActive(val);
}
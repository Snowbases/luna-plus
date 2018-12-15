// NoteDialog.cpp: implementation of the CNoteDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NoteDialog.h"
#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"
#include "NoteManager.h"
#include "GameIn.h"
// 061220 LYW --- Delete this code.
//#include "MainBarDialog.h"
#include "cRitemEx.h"

// 071022 LYW --- NoteDialog : Include header file of tutorial manager.
#include "TutorialManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNoteDialog::CNoteDialog()
{
	m_type = WT_NOTEDLG;
	m_SelectedNotePge = 1;
	m_CurNoteMode = eMode_NormalNote;
	m_CurNoteID = 0;
}

CNoteDialog::~CNoteDialog()
{

}

void CNoteDialog::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	m_type = WT_NOTEDLG;	
}

void CNoteDialog::Linking()
{
	//////////////////////////////////////////////////////////////////////////
	//eNoteMode_List
	m_pWriteNoteBtn = (cButton*)GetWindowForID(NOTE_WRITENOTEBTN);
	m_pDelNoteBtn = (cButton*)GetWindowForID(NOTE_DELNOTEBTN);
	m_pSaveNoteBtn = (cButton*)GetWindowForID(NOTE_SAVENOTEBTN);
	m_pNoteBtn = (cPushupButton*)GetWindowForID(NOTE_TABBTN1);
	m_pPsNoteBtn = (cPushupButton*)GetWindowForID(NOTE_TABBTN2);

	m_pNoteListLCtrl = (cListCtrl*)GetWindowForID(NOTE_NOTELISTLCTL);
	for(int i=0; i<NOTENUM_PER_PAGE; ++i)
	{
		m_pNoteChk[i] = (cCheckBox*)GetWindowForID(NOTE_NOTELISTCHK1+i);
		m_pPackage[i] = (cStatic*)GetWindowForID(NOTE_NOTEPACKAGE1+i);
		m_pPackage[i]->SetActive(FALSE);
	}
	m_pNoteChkAll = (cCheckBox*)GetWindowForID(NOTE_NOTELISTCHK12);

	for(i=0; i<MAX_NOTE_PAGE; ++i)
	{
		m_pNotePageBtn[i] = (cPushupButton*)GetWindowForID(NOTE_PAGE1BTN+i);
		m_pNotePageBtn[i]->SetActive(TRUE);
	}

	m_pOverAlert = (cStatic*)GetWindowForID(NOTE_NOTEOVERALERT);
	m_pOverAlert->SetActive(FALSE);

	SetMode( eMode_NormalNote );

	//////////////////////////////////////////////////////////////////////////
}

void CNoteDialog::SetActive( BOOL val )
{
	cDialog::SetActive( val );
			
	if( !val )
	{
		SetMode( eMode_NormalNote );
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
	}
	else
	{
		if( m_pNoteChkAll )
			m_pNoteChkAll->SetChecked( FALSE );
	}

	TUTORIALMGR->Check_OpenDialog(this->GetID(), val) ;
}


void CNoteDialog::SetMode(WORD mode)
{
	m_CurNoteMode = mode;
	
	if( mode == eMode_NormalNote )
	{
#ifndef TAIWAN_LOCAL
		m_pNoteBtn->SetPush( TRUE );
		m_pPsNoteBtn->SetPush( FALSE );
#endif
	}
	else
	{
#ifndef TAIWAN_LOCAL
		m_pNoteBtn->SetPush( FALSE );
		m_pPsNoteBtn->SetPush( TRUE );
#endif
	}
}


void CNoteDialog::SetNoteList(MSG_FRIEND_NOTE_LIST* pmsg)
{
	// 쪽지를 읽을 때 새로고침하기 위해 저장한다
	mNoteList = *pmsg;

	ShowNotePageBtn(pmsg->TotalPage);

	if(pmsg->TotalMsgNum > MAX_NOTENUM)
		m_pOverAlert->SetActive(TRUE);
	else
		m_pOverAlert->SetActive(FALSE);

	m_pNoteListLCtrl->DeleteAllItems();
	
	for(int i=0; i<NOTENUM_PER_PAGE; ++i)
	{
		m_pNoteChk[i]->SetChecked(FALSE);
		m_pNoteChk[i]->SetActive(FALSE);
		m_pPackage[i]->SetActive(FALSE);

		if(pmsg->NoteList[i].NoteID != 0)
		{
			cRITEMEx *ritem = new cRITEMEx(3);

			// 제목
			char Title[MAX_NOTE_TITLE+1] = {0,};
			int Len = strlen( pmsg->NoteList[i].SendTitle );
			if( Len > 24 )
			{
				strncpy( Title, pmsg->NoteList[i].SendTitle, 24 );
				strcat( Title, "...\n");
				strcpy(ritem->pString[0],Title);
			}
			else if(Len < 2)
			{
				strcpy(ritem->pString[0], CHATMGR->GetChatMsg(2238));
				strcpy(mNoteList.NoteList[i].SendTitle, ritem->pString[0]);
			}
			else
			{
				char* pTitle = GetCovertedTitleFromFormNote(pmsg->NoteList[i].SendTitle);
				if(pTitle)
					strcpy(ritem->pString[0], pTitle);
				else
					strcpy(ritem->pString[0], pmsg->NoteList[i].SendTitle);
			}
			
			//이름 6글자 까지 보여준다. 
			char Name[13] = {0,};
			Len = strlen( pmsg->NoteList[i].FromName ); 
			if(Len > 12)
			{
				strncpy( Name, pmsg->NoteList[i].FromName, 12 );
				Name[12] = 0;
				strcpy(ritem->pString[2],Name);
			}
			else 
			{
				char* pName = GetCovertedNameFromFormNote(pmsg->NoteList[i].FromName);
				if(pName)
					strcpy(ritem->pString[2], pName);
				else
					strcpy(ritem->pString[2], pmsg->NoteList[i].FromName);
			}

			char Date[12] = {0,};
			strncpy(Date, pmsg->NoteList[i].SendDate, 10);
			Date[11] = 0;
			strcpy(ritem->pString[1], Date);

			
			DWORD color = RGBA_MAKE( 255, 255, 255, 0 );
			if( pmsg->NoteList[i].bIsRead )
				color = RGBA_MAKE( 255, 255, 0, 0 );

			for( int r=0; r<3; r++)
				ritem->rgb[r] = color;

			ritem->dwID = pmsg->NoteList[i].NoteID; 
			m_pNoteListLCtrl->InsertItem(i, ritem);
			m_pNoteChk[i]->SetActive(TRUE);

			if(pmsg->NoteList[i].PackageItemIdx || pmsg->NoteList[i].PackageMoney)
				m_pPackage[i]->SetActive(TRUE);
		}
	}

	SetActive(TRUE);
}


void CNoteDialog::Refresh( DWORD noteID )
{
	m_pNoteListLCtrl->DeleteAllItems();

	for(int i=0; i<NOTENUM_PER_PAGE; ++i)
	{
		cCheckBox* checkBox = m_pNoteChk[i];

		FRIEND_NOTE& note = mNoteList.NoteList[ i ];

		if( ! note.NoteID )
		{
			checkBox->SetChecked(FALSE);
			checkBox->SetActive(FALSE);

			break;
		}
		else if( note.NoteID == noteID )
		{
			note.bIsRead = TRUE;
		}

		checkBox->SetActive(TRUE);
		m_pPackage[i]->SetActive(FALSE);

		cRITEMEx *ritem = new cRITEMEx(3);

		// 제목
		char Title[MAX_NOTE_TITLE+1] = {0,};
		int Len = strlen( note.SendTitle );
		if( Len > 24 )
		{
			strncpy( Title, note.SendTitle, 24 );
			strcat( Title, "...\n");
			strcpy(ritem->pString[0],Title);
		}
		else if(Len < 2)
		{
			strcpy(ritem->pString[0], CHATMGR->GetChatMsg(2238));
			strcpy(note.SendTitle, ritem->pString[0]);
		}
		else
		{
			char* pTitle = GetCovertedTitleFromFormNote(note.SendTitle);
			if(pTitle)
				strcpy(ritem->pString[0], pTitle);
			else
				strcpy(ritem->pString[0], note.SendTitle);
		}

		char Name[13] = {0,};
		Len = strlen( note.FromName ); 
		if(Len > 12)
			{
				strncpy( Name, note.FromName, 12 );
				Name[12] = 0;
				strcpy(ritem->pString[2],Name);
			}
			else 
			{
				char* pName = GetCovertedNameFromFormNote(note.FromName);
				if(pName)
					strcpy(ritem->pString[2], pName);
				else
					strcpy(ritem->pString[2], note.FromName);
			}

		char Date[12] = {0,};
		strncpy(Date, note.SendDate, 10);
		Date[11] = 0;
		strcpy(ritem->pString[1], Date);

		{
			const DWORD color = note.bIsRead ? RGBA_MAKE( 255, 255 ,0 ,0 ) : RGBA_MAKE( 255, 255, 255, 0 );

			ritem->rgb[ 0 ] = color;
			ritem->rgb[ 1 ] = color;
			ritem->rgb[ 2 ] = color;
		}

		ritem->dwID = note.NoteID; 
		m_pNoteListLCtrl->InsertItem( i, ritem);

		if(note.PackageItemIdx || note.PackageMoney)
			m_pPackage[i]->SetActive(TRUE);
	}
}

char* CNoteDialog::GetSendDate(DWORD dwNoteID)
{
	for(int i=0; i<NOTENUM_PER_PAGE; ++i)
	{
		FRIEND_NOTE& note = mNoteList.NoteList[ i ];
		if(dwNoteID == note.NoteID)
		{
			return note.SendDate;
		}
	}
	
	return NULL;
}

char* CNoteDialog::GetTitle(DWORD dwNoteID)
{
	for(int i=0; i<NOTENUM_PER_PAGE; ++i)
	{
		FRIEND_NOTE& note = mNoteList.NoteList[ i ];
		if(dwNoteID == note.NoteID)
		{
			return note.SendTitle;
		}
	}

	return NULL;
}

void CNoteDialog::CheckedNoteID()
{
	DWORD IDs[NOTENUM_PER_PAGE];
	SelectedNoteIDChk(NOTENUM_PER_PAGE,NOTE_NOTELISTCHK1,m_pNoteListLCtrl,IDs);	
	
	int lastidx = 0;
	for(int i=0;i<NOTENUM_PER_PAGE;++i)
	{
		if(IDs[i] != 0)
			lastidx = i;
	}
	
	for(i=0;i<NOTENUM_PER_PAGE;++i)
	{
		if(IDs[i] != 0)
		{
			BOOL bLast;
			if(i==lastidx)
				bLast = TRUE;
			else
				bLast = FALSE;
			NOTEMGR->DelNoteSyn(IDs[i],bLast);
		}
	} 
}

void CNoteDialog::SaveNoteID()
{
	if(GetMode() == eMode_StoredNote)
	{
		return;
	}

	DWORD IDs[NOTENUM_PER_PAGE];
	SelectedNoteIDChk(NOTENUM_PER_PAGE,NOTE_NOTELISTCHK1,m_pNoteListLCtrl,IDs);	
	
	int lastidx = 0;
	BOOL bHasUnRead = FALSE;
	BOOL bHasPackage = FALSE;
	for(int i=0;i<NOTENUM_PER_PAGE;++i)
	{
		if(IDs[i] != 0)
			lastidx = i;

		if( IDs[i] && FALSE==mNoteList.NoteList[i].bIsRead )
		{
			bHasUnRead = TRUE;
			break;
		}

		if( (IDs[i] && 0!=mNoteList.NoteList[i].PackageItemIdx) ||
			(IDs[i] && 0!=mNoteList.NoteList[i].PackageMoney) )
		{
			bHasPackage = TRUE;
			break;
		}
	}

	if(bHasUnRead)
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2239) );
		return;
	}
	else if(bHasPackage)
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(2240) );
		return;
	}
	
	for(i=0;i<NOTENUM_PER_PAGE;++i)
	{
		if(IDs[i] != 0)
		{
			BOOL bLast;
			if(i==lastidx)
				bLast = TRUE;
			else
				bLast = FALSE;

			NOTEMGR->SaveNoteSyn(IDs[i],bLast);
		}
	} 
}

void CNoteDialog::SelectedNoteIDChk(int count, LONG ChkBtnIDStar,cListCtrl* pListCtrl,DWORD* IDs)
{
	cCheckBox* pChkArray[NOTENUM_PER_PAGE];
	for(int i=0; i<count; ++i)
	{
		pChkArray[i] = (cCheckBox*)GetWindowForID(ChkBtnIDStar+i);
	}
	GetCheckedNoteIDs(count,pChkArray,pListCtrl,IDs);
}


void CNoteDialog::GetCheckedNoteIDs(int count,cCheckBox** pChkBtnArray,cListCtrl* pListCtrl,DWORD* pOutIDs)
{
	memset(pOutIDs,0,sizeof(DWORD)*count);
	for(int i=0; i<count; ++i)
	{	
		cCheckBox* Chk = pChkBtnArray[i];
		
		if(Chk->IsChecked())
		{
			cRITEMEx * rItem = (cRITEMEx *)pListCtrl->GetRItem(i);
			if(!rItem) return;
			if(rItem->dwID)
			{
				pOutIDs[i] = rItem->dwID;
			}
		}
	}
}


DWORD CNoteDialog::ActionEvent(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;
	we = cDialog::ActionEvent(mouseInfo);
	
	if(we & WE_ROWCLICK)
	{
		cRITEMEx * pRItem = (cRITEMEx *)m_pNoteListLCtrl->GetRItem(m_pNoteListLCtrl->GetSelectedRowIdx());
		if(pRItem != NULL)
		{	
			NOTEMGR->ReadNote(pRItem->dwID, 1);
		}
	}
	return we;
}

void CNoteDialog::ShowNotePageBtn(BYTE TotalPage)
{
	// 080121 LYW --- NoteManager : 선택 된 탭만 푸쉬 상태로 처리 함.
	int nSelectedTab = m_SelectedNotePge-1 ;

	for( int count = 0 ;  count < MAX_NOTE_PAGE ; ++count )
	{
		if( !m_pNotePageBtn[count] ) continue ;

		if( nSelectedTab == count )
		{
			m_pNotePageBtn[count]->SetPush(TRUE) ;
		}
		else
		{
			m_pNotePageBtn[count]->SetPush(FALSE) ;
		}
	}
}

BOOL CNoteDialog::IsPackageNote(DWORD dwNoteID)
{
	for(int i=0; i<NOTENUM_PER_PAGE; i++)
	{
		if(mNoteList.NoteList[i].NoteID == dwNoteID)
		{
			if(mNoteList.NoteList[i].PackageItemIdx || mNoteList.NoteList[i].PackageMoney)
				return TRUE;
		}
	}

	return FALSE;
}

void CNoteDialog::ClearPackageInfo(DWORD dwNoteID)
{
	for(int i=0; i<NOTENUM_PER_PAGE; i++)
	{
		if(mNoteList.NoteList[i].NoteID == dwNoteID)
		{
			mNoteList.NoteList[i].PackageItemIdx = 0;
			mNoteList.NoteList[i].PackageMoney = 0;
			break;
		}
	}
}

void CNoteDialog::SetChkAll()
{
	if( m_pNoteChkAll == NULL ) return;

	BOOL Chk = m_pNoteChkAll->IsChecked();
	
	for(int i=0; i<NOTENUM_PER_PAGE; ++i)
	{
		if( m_pNoteChk[i]->IsActive() )		
			m_pNoteChk[i]->SetChecked( Chk );
	}
}

char* CNoteDialog::GetCovertedNameFromFormNote(char* pName)
{
	int nLen = strlen(pName);
	char szPreFix[16] = {0,};
	char szIndex[16] = {0,};
	int nMsgIndex = 0;
	strncpy(szPreFix, pName, 4);

	if(0 == strcmp("<CONSIGN>", pName))
	{
		return CHATMGR->GetChatMsg(2236);
	}
	else if(0 == strcmp("<NPC", szPreFix))
	{
		strncpy(szIndex, &pName[4], nLen-5);
		nMsgIndex = atoi(szIndex);
		return CHATMGR->GetChatMsg(nMsgIndex);
	}

	return NULL;
}

char* CNoteDialog::GetCovertedTitleFromFormNote(char* pTitle)
{
	int nLen = strlen(pTitle);
	char szPreFix[16] = {0,};
	char szIndex[16] = {0,};
	int nMsgIndex = 0;
	strncpy(szPreFix, pTitle, 4);

	if(0==strncmp("<RECEIPT>", pTitle, 9))
	{
		return CHATMGR->GetChatMsg(2235);
	}
	else if(0 == strcmp("<MSG", szPreFix))
	{
		strncpy(szIndex, &pTitle[4], nLen-5);
		nMsgIndex = atoi(szIndex);
		return CHATMGR->GetChatMsg(nMsgIndex);
	}

	return NULL;
}
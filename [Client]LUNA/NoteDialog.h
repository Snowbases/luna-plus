#pragma once


#include "INTERFACE\cDialog.h"
#include ".\interface\cWindowHeader.h"
#include ".\interface\cTextArea.h"


class CNoteDialog : public cDialog  
{
	cListCtrl* m_pNoteListLCtrl;
	
	cPushupButton* m_pNotePageBtn[MAX_NOTE_PAGE];
	cButton* m_pWriteNoteBtn;
	cButton* m_pDelNoteBtn;
	cButton* m_pSaveNoteBtn;
	cPushupButton* m_pNoteBtn;
	cPushupButton* m_pPsNoteBtn;
	
	cCheckBox* m_pNoteChk[NOTENUM_PER_PAGE];
	cCheckBox* m_pNoteChkAll;

	cStatic* m_pPackage[NOTENUM_PER_PAGE];
	cStatic* m_pOverAlert;

	WORD m_SelectedNotePge;
	WORD m_CurNoteMode;

	DWORD m_CurNoteID;

public:
	CNoteDialog();
	virtual ~CNoteDialog();
	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	virtual void SetActive( BOOL val );

	void SetNoteList(MSG_FRIEND_NOTE_LIST* pmsg);
	virtual DWORD ActionEvent(CMouse * mouseInfo);

	void Linking();
	void CheckedNoteID();
	void SaveNoteID();

	WORD GetSelectedNotePge() { return m_SelectedNotePge;	}
	void SetSelectedNotePge(WORD FPge) { m_SelectedNotePge = FPge;	}

	void GetCheckedNoteIDs(int count,cCheckBox** pChkBtnArray,cListCtrl* pListCtrl,DWORD* pOutIDs);
	void SelectedNoteIDChk(int count, LONG ChkBtnIDStar,cListCtrl* pListCtrl,DWORD* IDs);

	void SetMode(WORD mode);
	WORD GetMode()			{ return m_CurNoteMode;	}
	void ShowNotePageBtn(BYTE TotalPage);
	
	DWORD GetCurNoteID() { return m_CurNoteID; };
	void SetCurNoteID(DWORD id) { m_CurNoteID = id; };

	BOOL IsPackageNote(DWORD dwNoteID);
	void ClearPackageInfo(DWORD dwNoteID);

	void SetChkAll();

	// 읽은 쪽지의 색상 처리를 변경한다
	void Refresh( DWORD noteID );

	char* GetSendDate(DWORD dwNoteID);
	char* GetTitle(DWORD dwNoteID);

	char* GetCovertedNameFromFormNote(char* pName);
	char* GetCovertedTitleFromFormNote(char* pTitle);

private:
	MSG_FRIEND_NOTE_LIST	mNoteList;
};
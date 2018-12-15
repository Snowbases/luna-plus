#pragma once
#include "INTERFACE\cDialog.h"
#include "Item.h"
#include "ItemShow.h"

class cStatic;
class cTextArea;
class cButton;
class cEditBox;

class CMiniNoteDialogRead :
	public cDialog
{
	cStatic *	m_pRTitleStc;
	cTextArea * m_pRNoteText;
	cButton *	m_pReplayBtn;
	cButton *	m_pGetPackageBtn;
	cButton *	m_pDeleteBtn;
	cStatic *	m_pSendDateStc;
	cStatic *	m_pSenderStc;
	cStatic *	m_pPackageMoenyStc;

	cIconDialog*		m_pItemPackageDlg;
	CItem*				m_pItemPackage;

	DWORD m_SelectedNoteID;
	DWORD m_dwPackageMoney;
	cPtrList m_MinNoteCtlListArray;

public:
	CMiniNoteDialogRead(void);
	virtual ~CMiniNoteDialogRead(void);

	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	virtual void SetActive(BOOL val);
	void Linking();

	void SetMiniNote(char* Title, char* Date, char* Sender, char* Note, WORD ItemIdx);
	void SetPackageInfo(DWORD dwMoney, ITEMBASE* pBaseInfo, ITEM_OPTION* pOptionInfo);
	void SetNoteID(DWORD NoteID) { m_SelectedNoteID = NoteID;	}
	DWORD GetNoteID() { return m_SelectedNoteID; }
	const char* GetSenderName() const;

	void SendGetPackageMsg();
};

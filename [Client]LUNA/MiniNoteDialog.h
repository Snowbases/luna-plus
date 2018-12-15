// MiniNoteDialog.h: interface for the CMiniNoteDialogWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MININOTEDIALOG_H__11F671E3_42AB_4140_945A_71A1FEABEDDA__INCLUDED_)
#define AFX_MININOTEDIALOG_H__11F671E3_42AB_4140_945A_71A1FEABEDDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INTERFACE\cDialog.h"

class cStatic;
class cTextArea;
class cButton;
class cEditBox;

class CMiniNoteDialogWrite : public cDialog  
{
	cTextArea * m_pWNoteText;
	cButton *	m_pSendOkBtn;
	cButton *	m_pSendCancelBtn;
	
	cEditBox *	m_pTitleEdit;
	cEditBox *	m_pMReceiverEdit;
	cStatic *	m_pReceiver;

	cPtrList m_MinNoteCtlListArray;

	int m_CurMiniNoteMode;


public:
	CMiniNoteDialogWrite();
	virtual ~CMiniNoteDialogWrite();
	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	virtual void SetActive(BOOL val);
	void Linking();
	void SetMiniNote(char* Sender, char* Note, WORD ItemIdx);
	int GetCurMode();
};

#endif // !defined(AFX_MININOTEDIALOG_H__11F671E3_42AB_4140_945A_71A1FEABEDDA__INCLUDED_)

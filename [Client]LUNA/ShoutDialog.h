// ShoutDialog.h: interface for the CShoutDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHOUTDIALOG_H__2851FDE7_5AC8_4835_8F46_900FC49814D7__INCLUDED_)
#define AFX_SHOUTDIALOG_H__2851FDE7_5AC8_4835_8F46_900FC49814D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cDialog.h"

class cTextArea;
class CItem;

class CShoutDialog : public cDialog
{
	cTextArea* m_pMsgText;
	CItem* m_pItem;	

public:
	CShoutDialog();
	virtual ~CShoutDialog();
	virtual void OnActionEvent(LONG windowIndex, LPVOID windowControl, DWORD windowEvent);
	void Linking();
	virtual void SetActive(BOOL);
	void SetItem(CItem* pItem) { m_pItem = pItem; }
};

#endif // !defined(AFX_SHOUTDIALOG_H__2851FDE7_5AC8_4835_8F46_900FC49814D7__INCLUDED_)

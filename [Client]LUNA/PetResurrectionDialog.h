#pragma once
#include "./interface/cdialog.h"

class cIconDialog;
class CVirtualItem;
class CItem;

class CPetResurrectionDialog :
	public cDialog
{
	cIconDialog*	mIconDialog;
	CVirtualItem*	mSourceItem;
	CItem*			mUsedItem;

	MSG_DWORD3		mMessage;

public:
	CPetResurrectionDialog(void);
	virtual ~CPetResurrectionDialog(void);
	
	void Linking();
	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );

	void SetUsedItem( CItem* pItem ) { mUsedItem = pItem; }
	
	virtual void OnActionEvent(LONG lId, void* p, DWORD we);
	virtual void SetActive( BOOL val );

	void Send();
};

#pragma once
#include ".\interface\cdialog.h"

class cQuickSlotDlg;
class CQuickItem;

class cQuickDlg :
	public cDialog
{
	cQuickSlotDlg* mpSlot;
	cQuickSlotDlg* mpExSlot;

public:
	cQuickDlg(void);
	virtual ~cQuickDlg(void);

	void Linking();

	void SetActiveSheet( WORD page1, WORD page2 );

	void OnActionEvnet(LONG lId, void * p, DWORD we);

	void AddItem( WORD tab, WORD pos, CQuickItem* pItem );
	void RemoveItem( WORD tab, WORD pos );
	void ChangeItem( WORD tab1, WORD pos1, WORD tab2, WORD pos2 );
	cQuickSlotDlg* GetSlotDlg(POSTYPE);
	void Toggle_ExSlot() ;
};

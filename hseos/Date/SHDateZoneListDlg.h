#ifndef _SHDATEZONELISTDLG
#define _SHDATEZONELISTDLG

#include "./Interface/cDialog.h"
#include "./Interface/cListCtrl.h"

class CSHDateZoneListDlg : public cDialog
{
private:
	cListCtrl*		m_pcsDateZoneList;
	int				m_nSelDateZone;

public:
	CSHDateZoneListDlg() ;
	~CSHDateZoneListDlg() ;

	void		 	Linking();
	virtual void 	SetActive( BOOL val );
	virtual void 	Render();
	virtual DWORD	ActionEvent(CMouse* mouseInfo);
	void			OnActionEvent( LONG lId, void* p, DWORD we );

	void			SelectDateZone(int nRowIndex);
};



#endif
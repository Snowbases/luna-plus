#pragma once

#include "./Interface/cDialog.h"

class cEditBox;

class CStoreSearchDlg : public cDialog
{
protected:
	cEditBox*		m_pEBSearch;

public:
	CStoreSearchDlg(void);
	~CStoreSearchDlg(void);

	void Linking();
	virtual void SetActive( BOOL val );
	void OnActionEvent(LONG lId, void * p, DWORD we);
};

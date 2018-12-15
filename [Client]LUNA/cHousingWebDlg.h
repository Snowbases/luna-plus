#pragma once

#include "interface/cDialog.h"

class cBrowser;

class cHousingWebDlg :
	public cDialog
{
protected:
	cBrowser* m_pBrowser;

public:
	cHousingWebDlg(void);
	virtual ~cHousingWebDlg(void);

	void Linking();
	void SetActive( BOOL val );

	void Navigate( char* pURL );
	
	void OpenMiniHomePage(DWORD dwOwnerUserIndex);
};

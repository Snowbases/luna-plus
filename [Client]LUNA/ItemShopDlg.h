#pragma once


#include "./Interface/cDialog.h"

class cBrowser;

class CItemShopDlg : public cDialog
{
protected:
	cBrowser* m_pBrowser;

public:
	CItemShopDlg();
	virtual ~CItemShopDlg();

	void Linking();
	void SetActive( BOOL val );

	void Navigate( char* pURL );
};

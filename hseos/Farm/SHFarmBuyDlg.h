#ifndef _FARMBUY_H
#define _FARMBUY_H

#include "./Interface/cDialog.h"
#include "ImageNumber.h"

class cStatic;

class CSHFarmBuyDlg : public cDialog
{
protected:

public:
	CSHFarmBuyDlg() ;
	~CSHFarmBuyDlg() ;

	void		 	Linking();
	virtual void 	SetActive( BOOL val );
	virtual void 	Render();
};



#endif
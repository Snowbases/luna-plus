#pragma once
#include "cdividebox.h"

class cMoneyDivideBox :
	public cDivideBox
{
	DWORD	mMoney;
	DWORD	mColor;

public:
	cMoneyDivideBox(void);
	virtual ~cMoneyDivideBox(void);

	virtual void Render();
	virtual void CreateDivideBox( LONG x, LONG y, LONG ID, cbDivideFUNC cbFc1, cbDivideFUNC cbFc2, void * vData1, void * vData2, char* strTitle );
};

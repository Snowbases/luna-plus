#pragma once
#include "./interface/ctabdialog.h"

class CPetInvenDialog;
class CPetStateDialog;

class CPetInfoDialog :
	public cTabDialog
{
	CPetInvenDialog*	mpPetInvenDialog;
	CPetStateDialog*	mpPetStateDialog;

	BYTE				mCurTab;

public:
	CPetInfoDialog(void);
	virtual ~CPetInfoDialog(void);
	virtual void Add(cWindow*);
	virtual void Render();
	void Linking();
	virtual BOOL FakeMoveIcon(LONG x, LONG y, cIcon*);
};

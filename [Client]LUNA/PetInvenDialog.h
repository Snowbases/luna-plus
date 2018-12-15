#pragma once
#include "./interface/cdialog.h"

class CPetWearedDialog;

class CPetInvenDialog :
	public cDialog
{
	CPetWearedDialog*	mpPetWearedDialog;

public:
	CPetInvenDialog(void);
	virtual ~CPetInvenDialog(void);

	virtual void Add(cWindow * window);
	void Linking();

	virtual BOOL FakeMoveIcon(LONG x, LONG y, cIcon * icon);
	virtual void SetActive( BOOL val );
};

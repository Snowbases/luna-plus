#pragma once
#include "./interface/cicondialog.h"

class CPetWearedDialog :
	public cIconDialog
{
	cImage	mBlockImage;

	ITEMBASE	mDeleteItem;

public:
	CPetWearedDialog(void);
	virtual ~CPetWearedDialog(void);

	void Linking();

	virtual BOOL FakeMoveIcon(LONG x, LONG y, cIcon * icon);
	virtual void SetActive( BOOL val );

	virtual void Render();

	BOOL AddItem(WORD relPos, cIcon * InIcon);
	BOOL DeleteItem(WORD relPos, cIcon ** outIcon);

	void FakeDelete( cIcon* icon );
	void SendDelete();
	void DeleteAck( BYTE pos );
};

#pragma once


#include "Interface/cDialog.h"


class CGuildRevivalDialog : public cDialog  
{
public:
	CGuildRevivalDialog();
	virtual ~CGuildRevivalDialog();

	void Linking();
	virtual void OnActionEvent( LONG id, void* p, DWORD event );
};
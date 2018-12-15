#pragma once


#include "Interface/cDialog.h"


class cTextArea;


class CGuildNoticeDlg : public cDialog
{
public:
	CGuildNoticeDlg(void);
	virtual ~CGuildNoticeDlg(void);

	void Linking();
	virtual void OnActionEvent( LONG id, void* p, DWORD event );
	virtual void SetActive(BOOL val);

	void SetNotice( const char* );


private:
	cTextArea* mNotice;

	std::string mPreviousNotice;
};
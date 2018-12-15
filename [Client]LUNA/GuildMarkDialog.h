#pragma once

#include "interface/cDialog.h"


class cEditBox;
class cStatic;


class CGuildMarkDialog : public cDialog  
{
	cWindow*	mGuildMarkSubmit;
	cWindow*	mUnionMarkSubmit;
	cEditBox*	pMarkName;
	cStatic*	mTitle;

public:
	CGuildMarkDialog();
	virtual ~CGuildMarkDialog();
	virtual void SetActive(BOOL val);
	virtual void OnActionEvent( LONG id, void* p, DWORD event );

	void Linking();
	
	void	ShowGuildMark();
	void	ShowGuildUnionMark();
	
};
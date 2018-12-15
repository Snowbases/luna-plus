#pragma once


#include "INTERFACE\cDialog.h"

class cTextArea;
class cEditBox;
class cStatic;


class CGuildNickNameDialog : public cDialog  
{
	cTextArea*	m_pNickMsg;
	cEditBox*	m_pNickName;
	cStatic*	mTitle;

	GUILDMEMBERINFO	mSelectedMember;

public:
	CGuildNickNameDialog();
	virtual ~CGuildNickNameDialog();
	virtual void SetActive(BOOL val);
	void Linking();
	//void SetNickMsg( const char* );
	virtual void OnActionEvent( LONG id, void* p, DWORD event );
};
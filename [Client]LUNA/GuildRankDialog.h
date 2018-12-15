#pragma once


#include "INTERFACE\cDialog.h"

class cTextArea;
class cComboBox;
class cButton;
class cStatic;


class CGuildRankDialog  : public cDialog  
{
	cTextArea*	m_pRankMemberName;
	cComboBox*	m_pRankComboBox;
	cStatic*	mTitle;
	GUILDMEMBERINFO	mSelectedMember;
	

public:
	CGuildRankDialog();
	virtual ~CGuildRankDialog();
	virtual void SetActive(BOOL val);
	void Linking();
	
	virtual void OnActionEvent( LONG id, void* p, DWORD event );
};
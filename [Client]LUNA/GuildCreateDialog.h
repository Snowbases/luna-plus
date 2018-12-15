#pragma once


#include "interface/cDialog.h"


//class cStatic;
class cEditBox;
class cTextArea;


class CGuildCreateDialog : public cDialog  
{
public:
	CGuildCreateDialog();
	virtual ~CGuildCreateDialog();
	void Linking();
	virtual void SetActive( BOOL );
	virtual void OnActionEvent( LONG id, void* p, DWORD event );

private:
	
	//cStatic*	m_pLocation;
	cEditBox*	m_pGuildName;
};


class CGuildUnionCreateDialog : public cDialog
{
protected:
	cEditBox*	m_pNameEdit;
	cTextArea*	m_pText;
	cButton*	m_pOkBtn;
	
public:
	CGuildUnionCreateDialog();
	virtual ~CGuildUnionCreateDialog();

	virtual void	SetActive( BOOL val );
	virtual void	OnActionEvent( LONG id, void* p, DWORD event );
	void			Linking();
};
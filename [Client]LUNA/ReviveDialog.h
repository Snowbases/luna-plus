#pragma once


#include "./Interface/cDialog.h"


class cTextArea;


class CReviveDialog : public cDialog  
{
	cButton*	m_pPresentBtn;
	cButton*	m_pLoginBtn;
	cButton*	m_pVillageBtn;

	// 070112 LYW --- Add textarea control.
	cTextArea*	m_pTextArea ;

public:
	CReviveDialog();
	virtual ~CReviveDialog();

	void Linking();
	virtual void SetActive( BOOL val );
};
//=================================================================================================
//	FILE		: CertificateDlg.h
//	PURPOSE		: Header file of certificate interface.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 18, 2006
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================
#pragma once

#ifndef __CCERTIFICATE_H__
#define __CCERTIFICATE_H__


//=================================================================================================
//	INCLUDE PART
//=================================================================================================
#include ".\interface\cdialog.h"

class cButton ;
class cStatic ;
class cEditBox ;
class cTextArea ;


//=================================================================================================
//	DEFINE PART
//=================================================================================================


//=================================================================================================
//	THE OTHERS PART
//=================================================================================================


//=================================================================================================
// NAME			: CCertificateDlg Class.
// PURPOSE		: The class for certificate part.
// ATTENTION	:
//=================================================================================================
class CCertificateDlg : public cDialog
{
protected :
	/// Buttons for choice ok or cancel.
	cButton*	m_pBtnOk ;
	cButton*	m_pBtnCancel ;

	/// Static for notice random number.
	cStatic*	m_pRanNum ;

	/// Editbox for input text from user.
	cEditBox*	m_pEditBox ;

	/// Text area for notice text.
	cTextArea*	m_pText ;

	// 070117 LYW --- Add static control for title.
	// 070118 LYW --- Delete this control.
	//cStatic*	m_pTitle ;


	/// variable for random number.
	int m_nRanNum  ;
public:
	CCertificateDlg(void);
	virtual ~CCertificateDlg(void);

	/// The function for linking components to window manager.
	void Linking() ;

	/// The function for process event.
	void OnActionEvent( LONG lId, void* p, DWORD we ) ;

	/// Make random number for certificate some condition.
	void MakeRandomNumber() ;
};
#endif // __CCERTIFICATE_H__
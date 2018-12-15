// FamilyMarkDialog.h: interface for the CFamilyMarkDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FAMILYMARKDIALOG_H__E389FD12_6720_460C_97EE_5EE57C901DAD__INCLUDED_)
#define AFX_FAMILYMARKDIALOG_H__E389FD12_6720_460C_97EE_5EE57C901DAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INTERFACE\cDialog.h"
class cTextArea;
class cButton;
class CFamilyMarkDialog : public cDialog  
{
	cTextArea * m_pInfoText;
	cButton*	m_pFamilyMarkBtn;
	cButton*	m_pFamilyUnionMarkBtn;

public:
	CFamilyMarkDialog();
	virtual ~CFamilyMarkDialog();
	virtual void SetActive(BOOL val);
	void Linking();
	
	void	ShowFamilyMark();
	void	ShowFamilyUnionMark();
};

#endif // !defined(AFX_FAMILYMARKDIALOG_H__E389FD12_6720_460C_97EE_5EE57C901DAD__INCLUDED_)

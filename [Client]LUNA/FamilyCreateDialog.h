// FamilyCreateDialog.h: interface for the CFamilyCreateDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FAMILYCREATEDIALOG_H__280DECC6_0507_4E41_9FC8_5F2FCB9417C5__INCLUDED_)
#define AFX_FAMILYCREATEDIALOG_H__280DECC6_0507_4E41_9FC8_5F2FCB9417C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INTERFACE\cDialog.h"
class cStatic;
class cEditBox;
class cTextArea;

class CFamilyCreateDialog : public cDialog  
{
	cStatic * m_pLocation;
	cEditBox * m_pFamilyName;

public:
	CFamilyCreateDialog();
	virtual ~CFamilyCreateDialog();
	void Linking();
	virtual void SetActive(BOOL val);
};

#endif // !defined(AFX_FAMILYCREATEDIALOG_H__280DECC6_0507_4E41_9FC8_5F2FCB9417C5__INCLUDED_)

// FamilyRankDialog.h: interface for the CFamilyRankDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FAMILYRANKDIALOG_H__34228850_CFC8_4FAE_9A7F_DCAD9D68A695__INCLUDED_)
#define AFX_FAMILYRANKDIALOG_H__34228850_CFC8_4FAE_9A7F_DCAD9D68A695__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INTERFACE\cDialog.h"

class cTextArea;
class cComboBox;
class cButton;

enum FamilyRankmode
{
	eFamilyRankMode_Dan,
	eFamilyRankMode_Family,
	eFamilyRankMode_Max,
};

class CFamilyRankDialog  : public cDialog  
{
	cTextArea * m_pRankMemberName;
	cComboBox * m_pRankComboBox;
	cComboBox * m_pDRankComboBox;
	cButton * m_pOkBtn;
	cButton * m_pDOkBtn;

	cPtrList m_FamilyRankCtrlList[eFamilyRankMode_Max];
	BYTE m_CurFamilyRankMode;
public:
	CFamilyRankDialog();
	virtual ~CFamilyRankDialog();
	virtual void SetActive(BOOL val);
	void Linking();
	void ShowFamilyRankMode(BYTE FamilyLvl);
	void SetActiveFamilyRankMode(int showmode,BOOL bActive);
	void SetName(char * name);
};

#endif // !defined(AFX_FAMILYRANKDIALOG_H__34228850_CFC8_4FAE_9A7F_DCAD9D68A695__INCLUDED_)

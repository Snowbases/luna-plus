// PartyCreateDlg.h: interface for the CPartyCreateDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTYCREATEDLG_H__B6BE7855_C5FF_443C_8017_CE52284E25F3__INCLUDED_)
#define AFX_PARTYCREATEDLG_H__B6BE7855_C5FF_443C_8017_CE52284E25F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INTERFACE\cDialog.h"
class cButton;
class cComboBox;

class CPartyCreateDlg : public cDialog  
{

	cButton* m_pOKBtn;
	cButton* m_pCancelBtn;
	cComboBox* m_pDistribute;
	cComboBox* m_pDiceGrade;

public:
	CPartyCreateDlg();
	virtual ~CPartyCreateDlg();
	void Linking();
	

};

#endif // !defined(AFX_PARTYCREATEDLG_H__B6BE7855_C5FF_443C_8017_CE52284E25F3__INCLUDED_)

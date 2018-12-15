// PartyCreateDlg.cpp: implementation of the CPartyCreateDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PartyCreateDlg.h"
#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cButton.h"
#include "./Interface/cComboBox.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPartyCreateDlg::CPartyCreateDlg()
{
	m_type = WT_PARTYCREATEDLG;
}

CPartyCreateDlg::~CPartyCreateDlg()
{

}

void CPartyCreateDlg::Linking()
{
	m_pDistribute =	(cComboBox*)GetWindowForID(PA_CREATECOMBOX);
	m_pDiceGrade = (cComboBox*)GetWindowForID(PA_DICEGRADECOMBOX);
	m_pOKBtn = (cButton*)GetWindowForID(PA_CREATEOK);
	m_pCancelBtn = (cButton*)GetWindowForID(PA_CREATECANCEL);
}
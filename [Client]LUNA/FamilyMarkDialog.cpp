// FamilyMarkDialog.cpp: implementation of the CFamilyMarkDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FamilyMarkDialog.h"
#include "NpcScriptDialog.h"
#include "WindowIDEnum.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"
#include "GameIn.h"
#include "ChatManager.h"
#include ".\interface\cTextArea.h"
#include "./interface/cEditBox.h"
#include "./interface/cButton.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFamilyMarkDialog::CFamilyMarkDialog()
{
	m_type = WT_FAMILYMARKDLG;
}

CFamilyMarkDialog::~CFamilyMarkDialog()
{

}

void CFamilyMarkDialog::Linking()
{
	m_pFamilyMarkBtn = (cButton*)GetWindowForID(FDM_REGISTOKBTN);
	m_pFamilyUnionMarkBtn = (cButton*)GetWindowForID(FUM_REGISTOKBTN);
}

void CFamilyMarkDialog::SetActive(BOOL val)
{
 	if(TRUE == val)
	{
		cEditBox* const pMarkName = (cEditBox*)GetWindowForID(
			FDM_NAMEEDIT);
		pMarkName->SetEditText(
			"");
		pMarkName->SetFocusEdit(
			TRUE);
	}
	cDialog::SetActive(val);
}

void CFamilyMarkDialog::ShowFamilyMark() 
{
	SetActive( TRUE );
	m_pFamilyMarkBtn->SetActive( TRUE );
	m_pFamilyUnionMarkBtn->SetActive( FALSE );
}

void CFamilyMarkDialog::ShowFamilyUnionMark()
{
	SetActive( TRUE );
	m_pFamilyMarkBtn->SetActive( FALSE );
	m_pFamilyUnionMarkBtn->SetActive( TRUE );
}
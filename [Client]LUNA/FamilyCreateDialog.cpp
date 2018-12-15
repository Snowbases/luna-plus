// FamilyCreateDialog.cpp: implementation of the CFamilyCreateDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FamilyCreateDialog.h"
#include "./Interface/cWindowManager.h"
#include "WindowIDEnum.h"
#include "./interface/cStatic.h"
#include "./interface/cEditBox.h"
#include "./interface/cTextArea.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "GameIn.h"
#include "NpcScriptDialog.h"
#include "ChatManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFamilyCreateDialog::CFamilyCreateDialog()
{
	m_type = WT_FAMILYCREATEDLG;
}

CFamilyCreateDialog::~CFamilyCreateDialog()
{

}

void CFamilyCreateDialog::Linking()
{
	m_pFamilyName = (cEditBox*)GetWindowForID(FD_CNAME);
}

void CFamilyCreateDialog::SetActive(BOOL val)
{
	if(val == TRUE)
	{
		m_pFamilyName->SetEditText("");
	}
	else
	{
		m_pFamilyName->SetFocusEdit(FALSE);
		if(HERO == 0)
			return;
		if( ( HERO->GetState() == eObjectState_Deal ) && (GAMEIN->GetNpcScriptDialog()->IsActive() == FALSE))
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
	}
	cDialog::SetActive(val);
}
// PartyBtnDlg.cpp: implementation of the CPartyBtnDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PartyBtnDlg.h"
#include "WindowIDEnum.h"
#include "Interface/cButton.h"
#include "Interface/cStatic.h"
#include "Interface/cComboBox.h"
#include "PartyManager.h"
#include "ObjectManager.h"

#include "./Interface/cCheckBox.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPartyBtnDlg::CPartyBtnDlg()
{
	m_bSetTopOnActive = FALSE;	//KESÃß°¡;
	// 070416 LYW --- PartyBtnDlg : Default setting to FALSE.
	m_bOption = IsActive();

	m_pSecedeBtn= NULL ;
	m_pTransferBtn= NULL ;
	m_pForcedSecedeBtn= NULL ;
	m_pAddMemberBtn= NULL ;
	m_pBreakUpBtn= NULL ;
	m_pWarSuggestBtn= NULL ;
	m_pMemberBtn= NULL ;
}

CPartyBtnDlg::~CPartyBtnDlg()
{
}

void CPartyBtnDlg::Linking()
{
	m_pSecedeBtn = (cButton*)GetWindowForID(PA_SECEDEBTN);
	m_pTransferBtn = (cButton*)GetWindowForID(PA_TRANSFERBTN);
	m_pForcedSecedeBtn = (cButton*)GetWindowForID(PA_FORCEDSECEDEBTN);
	m_pAddMemberBtn = (cButton*)GetWindowForID(PA_ADDPARTYMEMBERBTN);
	m_pBreakUpBtn = (cButton*)GetWindowForID(PA_BREAKUPBTN);
	m_pWarSuggestBtn = (cButton*)GetWindowForID(PA_WARSUGGESTBTN);
	m_pDistributeCombo = (cComboBox*)GetWindowForID(PA_DISTRIBUTE);
	m_pDiceGradeCombo = (cComboBox*)GetWindowForID(PA_DICEGRADE);
}

void CPartyBtnDlg::RefreshDlg()
{
	if(FALSE == PARTYMGR->IsPartyMember(gHeroID))
	{
		ShowNonPartyDlg();
		return;
	}
	else if(PARTYMGR->GetMasterID() == gHeroID)
	{
		ShowPartyMasterDlg();
		return;
	}

	ShowPartyMemberDlg();
}

void CPartyBtnDlg::ShowNonPartyDlg()
{
	m_pSecedeBtn->SetActive(TRUE);
	m_pTransferBtn->SetActive(TRUE);
	m_pForcedSecedeBtn->SetActive(TRUE);
	m_pAddMemberBtn->SetActive(TRUE);
	m_pBreakUpBtn->SetActive(TRUE);
	m_pWarSuggestBtn->SetActive(TRUE);
}

void CPartyBtnDlg::ShowPartyMasterDlg()
{
	m_pSecedeBtn->SetActive(m_bOption);
	m_pTransferBtn->SetActive(m_bOption);
	m_pForcedSecedeBtn->SetActive(m_bOption);
	m_pAddMemberBtn->SetActive(m_bOption);
	m_pBreakUpBtn->SetActive(m_bOption);
	m_pWarSuggestBtn->SetActive(m_bOption);

	m_pSecedeBtn->SetImageRGB( ICONCLR_DISABLE );
	m_pTransferBtn->SetImageRGB( ICONCLR_USABLE );
	m_pForcedSecedeBtn->SetImageRGB( ICONCLR_USABLE );
	m_pAddMemberBtn->SetImageRGB( ICONCLR_USABLE );
	m_pBreakUpBtn->SetImageRGB( ICONCLR_USABLE );
	m_pWarSuggestBtn->SetImageRGB( ICONCLR_USABLE );

	m_pSecedeBtn->SetDisable( TRUE );
	m_pTransferBtn->SetDisable( FALSE );
	m_pForcedSecedeBtn->SetDisable( FALSE );
	m_pAddMemberBtn->SetDisable( FALSE );
	m_pBreakUpBtn->SetDisable( FALSE );
	m_pWarSuggestBtn->SetDisable( FALSE );
}

void CPartyBtnDlg::ShowPartyMemberDlg()
{
	m_pSecedeBtn->SetActive(m_bOption);
	m_pTransferBtn->SetActive(m_bOption);
	m_pForcedSecedeBtn->SetActive(m_bOption);
	m_pAddMemberBtn->SetActive(m_bOption);
	m_pBreakUpBtn->SetActive(m_bOption);
	m_pWarSuggestBtn->SetActive(m_bOption);
	
	m_pSecedeBtn->SetImageRGB( ICONCLR_USABLE );
	m_pTransferBtn->SetImageRGB( ICONCLR_DISABLE );
	m_pForcedSecedeBtn->SetImageRGB( ICONCLR_DISABLE );
	m_pAddMemberBtn->SetImageRGB( ICONCLR_USABLE );
	m_pBreakUpBtn->SetImageRGB( ICONCLR_DISABLE );
	m_pWarSuggestBtn->SetImageRGB( ICONCLR_DISABLE );

	m_pSecedeBtn->SetDisable( FALSE );
	m_pTransferBtn->SetDisable( TRUE );
	m_pForcedSecedeBtn->SetDisable( TRUE );
	m_pAddMemberBtn->SetDisable( FALSE );
	m_pBreakUpBtn->SetDisable( TRUE );
	m_pWarSuggestBtn->SetDisable( TRUE );
}

void CPartyBtnDlg::ShowOption( BOOL bOption )
{
	m_bOption = bOption;

	m_pSecedeBtn->SetActive(m_bOption);
	m_pTransferBtn->SetActive(m_bOption);
	m_pForcedSecedeBtn->SetActive(m_bOption);
	m_pAddMemberBtn->SetActive(m_bOption);
	m_pBreakUpBtn->SetActive(m_bOption);
	m_pWarSuggestBtn->SetActive(m_bOption);
}

void CPartyBtnDlg::Render()
{
	// 070209 LYW --- CPartyBtnDlg : Modified Render part.
	if( IsMovedWnd() )
	{
		SetMovedWnd( FALSE ) ;
	}

	cDialog::Render();
}

void CPartyBtnDlg::SetDistribute(int option)
{
	m_pDistributeCombo->SetCurSelectedIdx(option);
	m_pDistributeCombo->SelectComboText(
		WORD(option));
}

void CPartyBtnDlg::SetDiceGrade(int option)
{
	m_pDiceGradeCombo->SetCurSelectedIdx(option);
	m_pDiceGradeCombo->SelectComboText(
		WORD(option));
}
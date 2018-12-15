#include "stdafx.h"
#include "WindowIDEnum.h"
#include "./Interface/cStatic.h"
#include ".\quickdlg.h"
#include ".\quickslotdlg.h"
#include "./interface/cWindowManager.h"

cQuickDlg::cQuickDlg(void)
{
	m_type = WT_QUICKDIALOG;
}

cQuickDlg::~cQuickDlg(void)
{}

void cQuickDlg::Linking()
{
	mpSlot = ( cQuickSlotDlg* )GetWindowForID( QI1_QUICKSLOTDLG );

	WINDOWMGR->AddWindow( ( cWindow* )mpSlot );

	m_pComponentList.Remove( ( void* )mpSlot );

	mpSlot->Linking();

	// 080702 LYW --- QuickDlg : 확장 슬롯 추가 처리.
	mpExSlot = ( cQuickSlotDlg* )GetWindowForID( QI2_QUICKSLOTDLG ) ;

	WINDOWMGR->AddWindow( ( cWindow* )mpExSlot ) ;

	m_pComponentList.Remove( (void*)mpExSlot ) ;

	mpExSlot->Linking() ;
}

void cQuickDlg::SetActiveSheet( WORD page1, WORD page2 )
{
	mpSlot->SelectPage( page1 );

	// 080702 LYW --- QuickDlg : 확장 슬롯 추가 처리.
	mpExSlot->SelectPage( page2 ) ;
}

void cQuickDlg::OnActionEvnet(LONG lId, void * p, DWORD we)
{
	if( ((cQuickSlotDlg*)p)->GetID() == QI1_QUICKSLOTDLG )
	{
		mpSlot->OnActionEvnet( lId, p, we );
	}
	else if( ((cQuickSlotDlg*)p)->GetID() == QI2_QUICKSLOTDLG )
	{
		mpExSlot->OnActionEvnet( lId, p, we );
	}
	
	if(lId == QI_QUICK2BTN)
	{
		Toggle_ExSlot() ;
	}
}

cQuickSlotDlg* cQuickDlg::GetSlotDlg(POSTYPE position)
{
	switch(position)
	{
	case 0:
		{
			return mpSlot;
		}
	case 1:
		{
			return mpExSlot;
		}
	}

	return 0;
}

void cQuickDlg::Toggle_ExSlot()
{
	mpExSlot->SetAbsXY(
		LONG(mpSlot->GetAbsX()),
		LONG(mpSlot->GetAbsY() - mpSlot->GetHeight()));
	mpExSlot->SetActive(
		! mpExSlot->IsActive());
}

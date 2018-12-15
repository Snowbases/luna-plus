#include "stdafx.h"
#include ".\petinfodialog.h"
#include ".\petinvendialog.h"
#include ".\petstatedialog.h"
#include "gamein.h"

CPetInfoDialog::CPetInfoDialog(void)
{
}

CPetInfoDialog::~CPetInfoDialog(void)
{
}

void CPetInfoDialog::Add(cWindow * window)
{
	if(window->GetType() == WT_PUSHUPBUTTON)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx1 ;

		AddTabBtn(curIdx1++, (cPushupButton * )window);
	}
	else if(window->GetType() == WT_PET_INVEN_DLG)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx2 ;

		AddTabSheet(curIdx2++, window);
		mpPetInvenDialog = ( CPetInvenDialog* )window;
		GAMEIN->SetPetInvenDlg( mpPetInvenDialog );
	}
	else if(window->GetType() == WT_PET_STATE_DLG)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx2 ;

		AddTabSheet(curIdx2++, window);
		mpPetStateDialog = ( CPetStateDialog* )window;
		GAMEIN->SetPetStateDlg( mpPetStateDialog );
	}
	else 
		cDialog::Add(window);
}

void CPetInfoDialog::Linking()
{
	mpPetInvenDialog->Linking();
	mpPetStateDialog->Linking();
}

void CPetInfoDialog::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}

BOOL CPetInfoDialog::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	if( GetTabSheet( GetCurTabNum() )->GetType() == WT_PET_INVEN_DLG )
	{
		return mpPetInvenDialog->FakeMoveIcon( x, y, icon );
	}
	else
		return FALSE;
}
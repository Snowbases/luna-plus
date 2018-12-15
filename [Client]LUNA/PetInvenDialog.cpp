#include "stdafx.h"
#include ".\petinvendialog.h"
#include ".\petweareddialog.h"
#include "gamein.h"

CPetInvenDialog::CPetInvenDialog(void)
{
	m_type = WT_PET_INVEN_DLG;
}

CPetInvenDialog::~CPetInvenDialog(void)
{
}

void CPetInvenDialog::Add(cWindow * window)
{
	if(window->GetType() == WT_PET_WEARED_DLG)
	{
		mpPetWearedDialog = ( CPetWearedDialog* )window;
		GAMEIN->SetPetWearedDlg( mpPetWearedDialog );
	}

	cDialog::Add(window);
}

void CPetInvenDialog::Linking()
{
	mpPetWearedDialog->Linking();
}

void CPetInvenDialog::SetActive( BOOL val )
{
	cDialog::SetActive( val );

	mpPetWearedDialog->SetActive( val );
}

BOOL CPetInvenDialog::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	// 090202 LYW --- PetInvenDialog : ¸¶¿ì½º ÁÂÇ¥¸¦ Æê Ã¢¿¡ ¸Â°Ô ²û ¼öÁ¤ÇÑ´Ù.
	return mpPetWearedDialog->FakeMoveIcon(
		LONG(x - GetAbsX()),
		LONG(y - GetAbsY()),
		icon);
}
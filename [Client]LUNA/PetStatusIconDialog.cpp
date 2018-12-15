#include "stdafx.h"
#include ".\petstatusicondialog.h"
#include "GAMEIN.h"
#include "cdialog.h"

GLOBALTON(CPetStatusIconDialog)

CPetStatusIconDialog::CPetStatusIconDialog(void)
{
}

CPetStatusIconDialog::~CPetStatusIconDialog(void)
{
}

void CPetStatusIconDialog::Render()
{
	if(m_pObject == NULL)
		return;
	if( GAMEIN->GetPetUIDlg() )
	{
		m_DrawPosition.x = GAMEIN->GetPetUIDlg()->GetAbsX() + 169;
		m_DrawPosition.y = GAMEIN->GetPetUIDlg()->GetAbsY();
	}
	CStatusIconDlg::Render();
}

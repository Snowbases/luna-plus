#include "stdafx.h"
#include "SHFarmBuyDlg.h"
#include "GameIn.h"
#include "GameResourceManager.h"
#include "ObjectManager.h"
#include "WindowIDEnum.h"
#include "cStatic.h"
#include "..\effect\DamageNumber.h"

CSHFarmBuyDlg::CSHFarmBuyDlg()
{
}

CSHFarmBuyDlg::~CSHFarmBuyDlg()
{
}
void CSHFarmBuyDlg::SetActive( BOOL val )
{
	cDialog::SetActive( val );
}

void CSHFarmBuyDlg::Linking()
{
}

void CSHFarmBuyDlg::Render()
{
 	if( !IsActive() ) return;

	cDialog::Render();
}

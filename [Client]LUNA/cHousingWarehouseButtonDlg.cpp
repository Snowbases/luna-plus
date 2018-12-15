#include "stdafx.h"
#include "cHousingWarehouseButtonDlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "cHousingWarehouseDlg.h"
#include "cHousingMgr.h"
#include "GameIn.h"


cHousingWarehouseButtonDlg::cHousingWarehouseButtonDlg()
{
	m_type = WT_HOUSING_WAREHOUSE_BUTTON_DLG;
}

cHousingWarehouseButtonDlg::~cHousingWarehouseButtonDlg()
{

}

void cHousingWarehouseButtonDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
}

void cHousingWarehouseButtonDlg::Linking()
{
	 m_pWarehouseBtn =  ( cButton* )GetWindowForID( HOUSING_WH_BT );
}

void cHousingWarehouseButtonDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(! HOUSINGMGR->IsHouseOwner() )
	{
		//집주인이 아니면 닫자  
		val = FALSE;
	}

	cDialog::SetActive(val);
	cDialog::SetActiveRecursive(val);
}

void cHousingWarehouseButtonDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if(! HOUSINGMGR->IsHouseInfoLoadOk())
	{
		//하우스 로딩이 되어있지 않으면 리턴 
		return;
	}

	switch(lId)
	{
		case HOUSING_WH_BT :
			{
				// 091214 하우징 창고버튼 열기 닫기 둘다 가능하도록 수정 
				BOOL bActived = GAMEIN->GetHousingWarehouseDlg()->IsActive() ;
				//내집창고를 열자 
				GAMEIN->GetHousingWarehouseDlg()->SetActive( !bActived );
			}
			break;
	}
}
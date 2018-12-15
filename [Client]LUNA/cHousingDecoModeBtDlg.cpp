#include "stdafx.h"
#include "cHousingDecoModeBtDlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "cHousingWarehouseDlg.h"
#include "cHousingMgr.h"
#include "GameIn.h"
#include "cMsgBox.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"
#include "ChatManager.h"


cHousingDecoModeBtDlg::cHousingDecoModeBtDlg()
{
	m_type = WT_HOUSING_DECOMODE_BT_DLG;
}

cHousingDecoModeBtDlg::~cHousingDecoModeBtDlg()
{

}

void cHousingDecoModeBtDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
}

void cHousingDecoModeBtDlg::Linking()
{
	 m_pDecoMode_On_Btn		=	( cButton* )GetWindowForID( HOUSING_DECOMODE_ON_BT );
	 m_pDecoMode_Off_Btn	=	( cButton* )GetWindowForID( HOUSING_DECOMODE_OFF_BT );
}

void cHousingDecoModeBtDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(! HOUSINGMGR->IsHouseOwner() )
	{
		//집주인이 아니면 닫자  
		val = FALSE;
	}

	cDialog::SetActive(val);
	cDialog::SetActiveRecursive(val);

	if( val == TRUE )
	{
		if( HOUSINGMGR->IsDecorationMode() )
		{
			if( m_pDecoMode_On_Btn )
				m_pDecoMode_On_Btn->SetActive( FALSE );
		}
		else
		{
			if( m_pDecoMode_Off_Btn )
				m_pDecoMode_Off_Btn->SetActive( FALSE );
		}
	}
}

void cHousingDecoModeBtDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if(! HOUSINGMGR->IsHouseInfoLoadOk())
	{
		//하우스 로딩이 되어있지 않으면 리턴 
		return;
	}

	switch(lId)
	{
		case HOUSING_DECOMODE_ON_BT :
		case HOUSING_DECOMODE_OFF_BT :
			{ 
				if(  HERO->GetState() != eObjectState_Housing )
				{
					if( OBJECTSTATEMGR->GetObjectState(HERO) != eObjectState_None) 
					{
						WINDOWMGR->MsgBox( MBI_MAPMOVE_NOTICE, MBT_OK, CHATMGR->GetChatMsg( 358 ) ) ;
						return;
					}

					HOUSINGMGR->RequestDecoRationMode();
				}
				else
				{
					HOUSINGMGR->RequestDecoRationMode();
				}
			}
			break;
	}
}

void cHousingDecoModeBtDlg::OnChangeDecoMode( BOOL bDecemode)
{
	if( bDecemode )
	{
		if( m_pDecoMode_On_Btn )
			m_pDecoMode_On_Btn->SetActive( FALSE );

		if( m_pDecoMode_Off_Btn )
			m_pDecoMode_Off_Btn->SetActive( TRUE );
	}
	else
	{
		if( m_pDecoMode_On_Btn )
			m_pDecoMode_On_Btn->SetActive( TRUE );

		if( m_pDecoMode_Off_Btn )
			m_pDecoMode_Off_Btn->SetActive( FALSE );
	}
}
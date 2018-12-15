#include "cHousingActionPopupMenuDlg.h"
#include "WindowIDEnum.h"
#include "cStatic.h"
#include "cButton.h"
#include "ObjectManager.h"
#include "GameResourceManager.h"
#include "cHousingMgr.h"
#include "./Input/UserInput.h"

cHousingActionPopupMenuDlg::cHousingActionPopupMenuDlg(void)
{
	m_type = WT_HOUSING_ACTIONPOPUPMENUDLG;
}

cHousingActionPopupMenuDlg::~cHousingActionPopupMenuDlg(void)
{
}

void cHousingActionPopupMenuDlg::Linking()
{
	m_PopupMenuSlotArr[0].m_pBtAction = ( cButton* )GetWindowForID( HOUSING_ACTIONPOPUPMENU_BUTTON_1 );
	m_PopupMenuSlotArr[1].m_pBtAction = ( cButton* )GetWindowForID( HOUSING_ACTIONPOPUPMENU_BUTTON_2 );
	m_PopupMenuSlotArr[2].m_pBtAction = ( cButton* )GetWindowForID( HOUSING_ACTIONPOPUPMENU_BUTTON_3 );
	m_PopupMenuSlotArr[3].m_pBtAction = ( cButton* )GetWindowForID( HOUSING_ACTIONPOPUPMENU_BUTTON_4 );
	m_PopupMenuSlotArr[4].m_pBtAction = ( cButton* )GetWindowForID( HOUSING_ACTIONPOPUPMENU_BUTTON_5 );
}

void cHousingActionPopupMenuDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	switch(lId)
	{
		case HOUSING_ACTIONPOPUPMENU_BUTTON_1 :
		case HOUSING_ACTIONPOPUPMENU_BUTTON_2 :
		case HOUSING_ACTIONPOPUPMENU_BUTTON_3 :
		case HOUSING_ACTIONPOPUPMENU_BUTTON_4 :
		case HOUSING_ACTIONPOPUPMENU_BUTTON_5 :
			{
				LONG lSlotIndex =  lId - HOUSING_ACTIONPOPUPMENU_BUTTON_1;

				if( HOUSINGMGR->FakeUseAction( m_pstSelectedFurniture , m_PopupMenuSlotArr[lSlotIndex].m_pActionInfo) )
				{
					SetActive(FALSE);
				}
			}
			break;
	}
}

void cHousingActionPopupMenuDlg::SetActive(BOOL val)
{
	cDialog::SetActive( val );

	if( val )
	{
		//액션 팝업이 Active되면 선택된 가구의 액션정보를 가져와 m_PopupMenuSlotArr에 링크해둔다.
		CObject* pObject = OBJECTMGR->GetSelectedObject();

		if( ! pObject || pObject->GetObjectKind() != eObjectKind_Furniture )
			goto FAILED_ACTIVE;

		stFurniture* pstFurniture = ( (CFurniture*) pObject)->GetFurnitureInfo();
		stFunitureInfo* pstFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFurniture->dwFurnitureIndex);

		if ( pstFurnitureInfo->dwActionIndex == 0) 
			goto FAILED_ACTIVE;

		stHouseActionGroupInfo* pstActionGroup = GAMERESRCMNGR->GetHouseActionGroupInfo(pstFurnitureInfo->dwActionIndex);

		if( ! pstActionGroup || pstActionGroup->byActionNum == 0 )
			goto FAILED_ACTIVE;

		for( int i=0; i < MAX_HOUSE_ACTION_SLOT ; i++)
		{
			if( i >= pstActionGroup->byActionNum )
			{
				// 09091125 pdy 하우징 액션 버튼 텍스트가 남아있는 버그 수정
				if( m_PopupMenuSlotArr[i].m_pBtAction )
				{
					m_PopupMenuSlotArr[i].m_pBtAction->SetText("",RGBA_MAKE(255,255,255,255));
				}
				m_PopupMenuSlotArr[i].m_pActionInfo = NULL;
				continue;
			}

			stHouseActionInfo*	pActionInfo = GAMERESRCMNGR->GetHouseActionInfo( pstActionGroup->dwActionIndexList[i] ); 
			
			if( ! pActionInfo )
				goto FAILED_ACTIVE;

			m_PopupMenuSlotArr[i].m_pActionInfo = pActionInfo;

			m_PopupMenuSlotArr[i].m_pBtAction->SetText( pActionInfo->szName ,
				m_PopupMenuSlotArr[i].m_pBtAction->GetBasicColor() , 
				m_PopupMenuSlotArr[i].m_pBtAction->GetOverColor() ,
				m_PopupMenuSlotArr[i].m_pBtAction->GetPressColor() );

			if( m_PopupMenuSlotArr[i].m_pActionInfo->dwActionType == eHOUSE_ACTION_TYPE_RIDE )
			{
				//어태치값이 참이면 현재 마우스 포인트에서 가까운 본을 찾아 해당 노드번호를 저장해 놓는다. 
				LONG x = MOUSE->GetMouseX();
				LONG y = MOUSE->GetMouseY();

				if(! ( (CFurniture*) pObject)->GetRideNodeNumByMousePoint(x,y,m_bySortedNearBoneNumArr) )
				{
					goto FAILED_ACTIVE;
				}
			}
		}

		m_pstSelectedFurniture = pstFurniture;
	}
	else
	{
		for( int i=0; i < MAX_HOUSE_ACTION_SLOT ; i++)
			m_PopupMenuSlotArr[i].m_pActionInfo = NULL;

		m_pstSelectedFurniture = NULL;
	}

	return;

FAILED_ACTIVE:
	SetActive(FALSE);
}

DWORD cHousingActionPopupMenuDlg::ActionEvent(CMouse *mouseInfo)
{
	if( ! m_bActive || m_bDisable )
		return NULL;

	DWORD rt = cDialog::ActionEvent(mouseInfo) ;
	
	if( !rt && ( mouseInfo->LButtonDown() || mouseInfo->RButtonDown() || mouseInfo->MButtonDown() ) )
	{
		SetActive( FALSE );
	}
	return rt;
}

#include "stdafx.h"
#include "RidePopupMenuDlg.h"
#include "WindowIDEnum.h"
#include "cButton.h"
#include "../input/Mouse.h"
#include "ObjectManager.h"
#include "InventoryExDialog.h"
#include "GameIn.h"
#include "../Interface/cWindowManager.h"
#include "VehicleManager.h"
#include "MoveManager.h"

CRidePopupMenuDlg::CRidePopupMenuDlg()
{}

CRidePopupMenuDlg::~CRidePopupMenuDlg()
{
}

void CRidePopupMenuDlg::Linking()
{
	m_GetOn = (cButton*)GetWindowForID(
		ID_RIDEPOPUPMENU_BUTTON_GETON);
	m_GetOff = (cButton*)GetWindowForID(
		ID_RIDEPOPUPMENU_BUTTON_GETOFF);
	m_Seal = (cButton*)GetWindowForID(
		ID_RIDEPOPUPMENU_BUTTON_SEAL);
}

void CRidePopupMenuDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	CObject* pObject = OBJECTMGR->GetSelectedObject();
	if( !pObject || pObject->GetObjectKind() != eObjectKind_Vehicle )
	{
		// 잘못된 데이터가 선택된경우 팝업창을 닫는다
		SetActive( FALSE );
		return;
	}

	CVehicle* pVehicle = (CVehicle*)pObject;

	if( we & WE_BTNCLICK )
	{
 		switch( lId )																												
		{
		case ID_RIDEPOPUPMENU_BUTTON_GETON:
			{
				CActionTarget Target;
				Target.InitActionTarget( pVehicle, NULL );

				CAction MoveAction;
				float fDistance = NPC_TALKING_DISTANCE;

				MoveAction.InitAction( eActionKind_VehicleGetOn, NULL, fDistance, &Target );
				MOVEMGR->SetHeroActionMove( &Target, &MoveAction );		
			}
			break;
		case ID_RIDEPOPUPMENU_BUTTON_GETOFF:
			{
				// 090423 ShinJS --- 하차할 수 있는 상태인지 검사
				if( !pVehicle->CanGetOff( HERO ) )
				{
					break;
				}

				// 090316 LUJ, 서버에 요청한다
				MSGBASE message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category = MP_VEHICLE;
				message.Protocol = MP_VEHICLE_DISMOUNT_SYN;
				message.dwObjectID = HEROID;
				NETWORK->Send( &message, sizeof( message ) );
			}
			break;
		case ID_RIDEPOPUPMENU_BUTTON_SEAL:
			{
				// 090423 ShinJS --- 탈것 봉인 가능여부 확인
				if( !HERO->CanSealVehicle() )
					break;

				CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
				if( !pDlg )		break;

				pDlg->StartVehicleSeal( pVehicle->GetID() );	// 탈것봉인 Progress 시작
			}
			break;
		case TB_STATE_PUSHUP1:
			{
				cDialog* const dialog = WINDOWMGR->GetWindowForID(
					TB_STATE_PUSHUP2);

				if(0 == dialog)
				{
					break;
				}

				dialog->SetActive(
					! dialog->IsActive());
			}
			break;
		}

		SetActive( FALSE );
	}
}

void CRidePopupMenuDlg::SetActive(BOOL val)
{
	cDialog::SetActive( val );

	if( val )
	{
		CObject* pObject = OBJECTMGR->GetSelectedObject();

		// 탈것이 아닌경우 Return
		if( !pObject || pObject->GetObjectKind() != eObjectKind_Vehicle )
		{
			SetActive( FALSE );
			return;
		}

		CVehicle* pVehicle = (CVehicle*)pObject;

		if( HERO->GetVehicleID() != pVehicle->GetID() &&
			HERO->IsGetOnVehicle() )
		{
			SetActive( FALSE );
			return;
		}

		const DWORD enableColor = RGB(255, 255, 255);
		const DWORD unableColor = RGB(190, 190, 190);

		m_GetOn->SetDisable(
			TRUE);
		m_GetOn->SetText(
			m_GetOn->GetButtonText(),
			unableColor,
			unableColor);
		m_GetOff->SetDisable(
			TRUE);
		m_GetOff->SetText(
			m_GetOff->GetButtonText(),
			unableColor,
			unableColor);
		m_Seal->SetDisable(
			TRUE);
		m_Seal->SetText(
			m_Seal->GetButtonText(),
			unableColor,
			unableColor);

		if( pVehicle->IsGetOn( HERO ) )
		{
			m_GetOff->SetDisable(
				FALSE);
			m_GetOff->SetText(
				m_GetOff->GetButtonText(),
				enableColor,
				enableColor);
		}
		else
		{
			m_GetOn->SetDisable(
				FALSE);
			m_GetOn->SetText(
				m_GetOn->GetButtonText(),
				enableColor,
				enableColor);
		}

		const BOOL isMyVehicle = (pVehicle->GetOwnerIndex() == gHeroID);

		if(isMyVehicle)
		{
			m_Seal->SetDisable(
				FALSE);
			m_Seal->SetText(
				m_Seal->GetButtonText(),
				enableColor,
				enableColor);
		}

		cButton* const optionButton = (cButton*)GetWindowForID(
			TB_STATE_PUSHUP1);

		if(optionButton &&
			WT_BUTTON == optionButton->GetType())
		{
			const DWORD color = (isMyVehicle ? enableColor : unableColor);

			optionButton->SetDisable(
				!isMyVehicle);
			optionButton->SetText(
				optionButton->GetButtonText(),
				color,
				color);
		}
	}
}

DWORD CRidePopupMenuDlg::ActionEvent(CMouse *mouseInfo)
{
	DWORD rt = cDialog::ActionEvent(mouseInfo) ;
	
	if( !rt && ( mouseInfo->LButtonDown() || mouseInfo->RButtonDown() || mouseInfo->MButtonDown() ) )
	{
		SetActive( FALSE );
	}

	return rt;
}
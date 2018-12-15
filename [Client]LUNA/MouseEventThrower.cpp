#include "stdafx.h"
#include "MouseEventThrower.h"
#include "MouseEventReceiver.h"
#include "interface\cWindowManager.h"

#include "MousePointer.h"
#include "ObjectManager.h"
#include "gamein.h"
#include "MouseCursor.h"
#include "MHMap.h"
#include "WindowIDEnum.h"

#include "MonsterGuageDlg.h"

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.09.18
#include "../hseos/Farm/SHFarmManager.h"
// E 농장시스템 추가 added by hseos 2007.09.18

// 080618 KTH add
#include "../hseos/Farm/FarmAnimalDlg.h"

#include "MoveManager.h"
// 090317 NYJ - 하우징
#include "cHousingMgr.h"
#include "Input/UserInput.h"

CMouseEventThrower::CMouseEventThrower()
{
	m_pCurrentReceiver	= NULL;
	m_pDefaultReceiver	= NULL;

	m_bCancelSelect		= FALSE;
	m_bAllowMouseClick	= TRUE;
}

CMouseEventThrower::~CMouseEventThrower()
{

}

void CMouseEventThrower::SetDefaultReceiver(CMouseEventReceiver* pDefaultReceiver)
{
	m_pDefaultReceiver = pDefaultReceiver;
}

void CMouseEventThrower::SetCurrentReceiver(CMouseEventReceiver* pCurrentReceiver)
{
	m_pCurrentReceiver = pCurrentReceiver;
}

void CMouseEventThrower::SetReceiverToDefault()
{
	ASSERT(m_pDefaultReceiver);
	m_pCurrentReceiver = m_pDefaultReceiver;
}

void CMouseEventThrower::Follow()
{
	static DWORD dwLastFollowTick = 0;

	if( HERO )
	if( HERO->GetFollowPlayer() )
	{
		if( gCurTime - dwLastFollowTick >= FOLLOW_TICK )
		{
			dwLastFollowTick = gCurTime;

			CObject* pObject = OBJECTMGR->GetObject( HERO->GetFollowPlayer() );
			if( pObject )
			{
				if( pObject->GetObjectKind() == eObjectKind_Player )
				{
					VECTOR3 vMyPos, vTargetPos;

					HERO->GetPosition( &vMyPos );
					pObject->GetPosition( &vTargetPos );

					float dist = CalcDistanceXZ( &vMyPos, &vTargetPos );

					if( dist > 300.0f )
					{
						vTargetPos = ( vTargetPos - vMyPos ) * ( dist - 200.0f ) / dist + vMyPos;
						m_pCurrentReceiver->OnClickGround(MOUSEEVENT_LPRESS, &vTargetPos);
					}
				}
			}
			else
			{
				HERO->SetFollowPlayer( 0 );
				MOVEMGR->HeroMoveStop();
			}
		}
	}
}
//--------------

void CMouseEventThrower::Process(CMouse* Mouse)
{
	static DWORD dwMouseMoveTick = 0;
	static DWORD dwMouseOnTick = 0;
	static DWORD dwMousePressTick = 0;
	static DWORD previousMouseOverObjectIndex = 0;
	CObject* const previousMouseOverObject = OBJECTMGR->GetObject(
		previousMouseOverObjectIndex);

	if( !m_bAllowMouseClick )
		return;

	if( WINDOWMGR->IsMouseInputProcessed() || WINDOWMGR->IsDragWindow() )
	{
		if(previousMouseOverObject)
		{
			m_pCurrentReceiver->OnMouseLeave(
				previousMouseOverObject);
			previousMouseOverObjectIndex = 0;
		}
		
		dwMousePressTick = gCurTime;
		return;	//윈도우 드래그중일때는 마우스 이벤트 체크가 불필요.
	}

	DWORD MouseEvent = MOUSEEVENT_NONE;

	// 090406 ShinJS --- 마우스 오른쪽 더블클릭에 대한 Event 제거
	if( Mouse->LButtonDoubleClick() )
		MouseEvent = MOUSEEVENT_DBLCLICK;
	else
	{
		if( Mouse->LButtonDown() )
			MouseEvent = MOUSEEVENT_LCLICK;
		else if( Mouse->RButtonDown() )
			MouseEvent = MOUSEEVENT_RCLICK;
		// 090613 ONS 캐릭터 우클릭 팝업메뉴 생성 처리 변경 
		else if(Mouse->RButtonUp())
			MouseEvent = MOUSEEVENT_RBUTTONUP;

		if( Mouse->LButtonPressed() )
			MouseEvent |= MOUSEEVENT_LPRESS;
	}

//---캐릭터 선택 해제관련
	if( Mouse->RButtonDown() )	m_bCancelSelect = TRUE;
	if( Mouse->RButtonDrag() )	m_bCancelSelect = FALSE;

	if( HOUSINGMGR->IsHousingMap() ) 
	if( HERO->GetState() == eObjectState_Housing )
	{
		//090525 꾸미기모드일땐 가구 설치관련 마우스 이벤트만 처리한다.   
		HOUSINGMGR->ProcessMouseEvent_DecorationMode(Mouse,MouseEvent);
		return;
	}

	if( OBJECTMGR->GetSelectedObjectID() && Mouse->RButtonUp() && m_bCancelSelect )
	{
		m_bCancelSelect = FALSE;
	}

	CObject* pOverObject = NULL;

	// ShinJS 091105 --- Alt 클릭시 Object 클릭 무효화
	if( ! KEYBOARD->GetKeyPressed(KEY_MENU) )
	{
		int nMouseX, nMouseY;
		if( MouseEvent == MOUSEEVENT_NONE )
		{
			nMouseX = Mouse->GetMouseX();
			nMouseY = Mouse->GetMouseY();
		}
		else
		{
			nMouseX = Mouse->GetMouseEventX();
			nMouseY = Mouse->GetMouseEventY();
		}

		//하우징맵일경우 액션가능한 가구만 선택되게끔..& SortFlag추가 
		if( HOUSINGMGR->IsHousingMap() )
			pOverObject = (CObject*)HOUSINGMGR->GetSelectedObjectFromHouseMap( nMouseX, nMouseY, PICK_TYPE_PER_FACE, /*bSort = TRUE*/ TRUE );
		else
			pOverObject = (CObject*)GetSelectedObject( nMouseX, nMouseY, PICK_TYPE_PER_COLLISION_MESH, TRUE);

		// 080111 KTH -- 보스 몬스터 일 경우에만 PICK_TYPE_PER_FACE 호출
		// 091022 ShinJS --- 탈것 Face 검사 추가
		if( pOverObject && 
			(pOverObject->GetObjectKind() == eObjectKind_BossMonster ||
			pOverObject->GetObjectKind() == eObjectKind_Vehicle) )
		{
			pOverObject = (CObject*)GetSelectedObject( nMouseX, nMouseY, PICK_TYPE_PER_FACE );
		}
	}
	
	if( MouseEvent == MOUSEEVENT_NONE )
	{
		if( gCurTime - dwMouseOnTick > MOUSE_ON_TICK )
		{
			dwMouseOnTick = gCurTime;

			if(previousMouseOverObject != pOverObject )
			{
				m_pCurrentReceiver->OnMouseLeave(
					previousMouseOverObject);
				m_pCurrentReceiver->OnMouseOver(
					pOverObject);

				previousMouseOverObjectIndex = (pOverObject ? pOverObject->GetID() : 0);
			}
			//---마우스가 타일 위로
			if( pOverObject == NULL )
			{
				CURSOR->SetCursor( eCURSOR_DEFAULT );
			}
		}

		// 090422 ShinJS --- 탈것 설치를 위한 마우스위치로 설치Object 이동시키기
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( VEHICLE_INSTALL_ID );
		if( pVehicle )
			pVehicle->SetPosToMousePos();
	}
//---마우스 클릭
	else
	{
		// 프로그레스 창이 떠 있으면 움직이지 못하도록 한다.
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
			ASSERT( dialog );

			if( dialog->IsDisable() )
			{
				return;
			}
		}
		
		// desc_hseos_농장시스템_01
		// S 농장시스템 추가 added by hseos 2007.09.18
		// ..농장 모션 중이면 마우스 입력을 받지 않는다.
		if (g_csFarmManager.CLI_GetIngCharMotion())	return;
		// E 농장시스템 추가 added by hseos 2007.09.18
		
		if( pOverObject )
		{
			switch( pOverObject->GetEngineObjectType() )
			{
			case eEngineObjectType_Character:
				{
					m_pCurrentReceiver->OnClickPlayer(MouseEvent,(CPlayer*)pOverObject);
					break;
				}
			case eEngineObjectType_Monster:
				{
					m_pCurrentReceiver->OnClickMonster(MouseEvent,(CMonster*)pOverObject);
					break;
				}
			case eEngineObjectType_Npc:
				{
					m_pCurrentReceiver->OnClickNpc(MouseEvent,(CNpc*)pOverObject);
					break;
				}
			case eEngineObjectType_SkillObject:
				{
					m_pCurrentReceiver->OnClickSkillObject(MouseEvent,(cSkillObject*)pOverObject);
					break;
				}
			case eEngineObjectType_Effect:
				{
					ASSERT(0);
					break;
				}
			case eEngineObjectType_FarmRenderObj:
				{
					m_pCurrentReceiver->OnClickFarmRenderObj(MouseEvent,(CSHFarmRenderObj*)pOverObject);

					// 080618 KTH -- not select target
					if ( GAMEIN->GetAnimalDialog()->IsActive() )
						return;
					break;
				}
			// 090422 ShinJS --- 탈것 추가작업
			case eEngineObjectType_Vehicle:
				{
					m_pCurrentReceiver->OnClickVehicle(MouseEvent,(CVehicle*)pOverObject);
				}
				break;
			case eEngineObjectType_Furniture:
				{
					m_pCurrentReceiver->OnClickFurniture(MouseEvent,(CFurniture*)pOverObject );
					break;
				}
			default:
				{
					ASSERT( 0 );
					break;
				}
			}
			
			if( MouseEvent & MOUSEEVENT_DBLCLICK )
			{
				if( pOverObject->GetEngineObjectType() == eEngineObjectType_Character )
				{
					m_pCurrentReceiver->OnDbClickPlayer( MouseEvent,(CPlayer*)pOverObject );
				}
				else if( pOverObject->GetEngineObjectType() == eEngineObjectType_Monster )
				{
					m_pCurrentReceiver->OnDbClickMonster( MOUSEEVENT_LCLICK, (CMonster*)pOverObject );
				}
				else if( pOverObject->GetEngineObjectType() == eEngineObjectType_Pet )
				{
					m_pCurrentReceiver->OnDbClickPet( MOUSEEVENT_LCLICK, (CPet*)pOverObject );
				}
				else if( pOverObject->GetEngineObjectType() == eEngineObjectType_Vehicle )
				{
					m_pCurrentReceiver->OnDbClickVehicle( MouseEvent, (CVehicle*)pOverObject );
				}
			}

			if( MouseEvent & MOUSEEVENT_LCLICK && pOverObject != HERO )	//자신은 클릭이 안된다.
			{
				int ObjectType = pOverObject->GetEngineObjectType() ;

				if( ObjectType == eEngineObjectType_Monster )
				{
					pOverObject->ShowObjectName( TRUE, RGB_HALF( 255, 141, 39 ) );
				}
				else
				{
					pOverObject->ShowObjectName( TRUE, NAMECOLOR_SELECTED );
				}

				OBJECTMGR->SetSelectedObject( pOverObject );
			}
		}
		else 
		{
			if( MouseEvent & MOUSEEVENT_LCLICK )
			{
				if( gCurTime - dwMouseMoveTick >= MOUSE_MOVE_TICK )
				{
					dwMouseMoveTick = gCurTime;
					dwMousePressTick = gCurTime;

					VECTOR3* TargetPos = GetPickedPosition(Mouse->GetMouseEventX(),Mouse->GetMouseEventY());
					if(!TargetPos)
						return;

					m_pCurrentReceiver->OnClickGround(MouseEvent,TargetPos);

					//---KES 따라가기
					if(HERO)
						HERO->SetFollowPlayer( 0 );
					//---------------
				}
			}
			else if( MouseEvent & MOUSEEVENT_LPRESS )
			{
				if( gCurTime - dwMousePressTick >= MOUSE_PRESS_TICK )
				{
					dwMousePressTick = gCurTime;
					VECTOR3* TargetPos = GetPickedPosition(Mouse->GetMouseX(), Mouse->GetMouseY());
					if(!TargetPos)
						return;
					m_pCurrentReceiver->OnClickGround(MouseEvent,TargetPos);

					//---KES 따라가기
					if(HERO)
						HERO->SetFollowPlayer( 0 );
					//---------------
				}				
			}
		}
	}
}

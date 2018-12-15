// ObjectStateManager.cpp: implementation of the CObjectStateManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"

GLOBALTON(CObjectStateManager);
CObjectStateManager::CObjectStateManager()
{

}

CObjectStateManager::~CObjectStateManager()
{

}

void CObjectStateManager::InitObjectState(CObject* pObject) 
{
	const EObjectState State = pObject->m_BaseObjectInfo.ObjectState;

	if( eObjectState_Die != State )
	{
		pObject->OnStartObjectState( State );
		pObject->SetState( State );
		return;
	}

	switch( pObject->GetObjectKind() )
	{
	case eObjectKind_Monster:
		{
			//----KES ETC 071021
			//---enum문을 수정했다.
			pObject->GetEngineObject()->ChangeMotion( eMonsterMotion_Died, FALSE );
			//------------
			pObject->GetEngineObject()->DisablePick();
			break;
		}
	case eObjectKind_Player:
		{
			CPlayer* player	= ( CPlayer* )	pObject;

			// 시체 포즈
			{
				const eWeaponAnimationType weapon =	player->GetWeaponAniType();
				const WORD motion =	CHARACTER_MOTION[ eCharacterMotion_Died1 ][ weapon ];

				player->ChangeMotion( motion, FALSE );
				player->ChangeBaseMotion( motion );
			}
			
			// 눈 감기
			{
				const CHARACTER_TOTALINFO* info = player->GetCharacterTotalInfo();

				player->SetFlag( TRUE );
				player->SetFaceShape( info->FaceType * 2 + 1 + 72 );
			}
			
			break;
		}
	}
}

BOOL CObjectStateManager::StartObjectState(CObject* pObject, EObjectState State) 
{
	if( !pObject )			return FALSE;

	switch(pObject->GetState())
	{
	case eObjectState_SkillSyn:
	case eObjectState_Die:
		{
			return FALSE;
		}
	case eObjectState_Exit:
	case eObjectState_HouseRiding:
	case eObjectState_Move:
		{
			EndObjectState(pObject,pObject->GetState());
		}
		break;
	case eObjectState_Exchange:
	case eObjectState_StreetStall_Owner:
	case eObjectState_StreetStall_Guest:
	case eObjectState_Deal:
	case eObjectState_TiedUp:
	case eObjectState_Fishing:
	case eObjectState_FishingResult:
		{ 
			if( State != eObjectState_Die &&
				State != eObjectState_Exit &&
				pObject->GetID() == gHeroID)
			{
				return FALSE;
			}
		}
		break;
	case eObjectState_Society:
		{
			if( pObject->EndSocietyAct() == FALSE )
				return FALSE;
			
			EndObjectState( pObject, pObject->GetState() );
		}
		break;
	}

	pObject->OnStartObjectState(State);
	pObject->SetState(State);

	return TRUE;
}

void CObjectStateManager::EndObjectState(CObject* pObject, EObjectState State, DWORD EndStateCount)
{
	if( !pObject )			return;

	if(pObject->GetState() != State) 
	{
		if( pObject->GetState() == eObjectState_Die )		//KESÃß°¡
		{
			return;
		}
	}

	if(EndStateCount == 0)	// Áö±Ý Áï½Ã ³¡³½´Ù
	{
        pObject->SetState(eObjectState_None);
		pObject->OnEndObjectState(State);		//setstate()ÇÏ±â?ü¿¡ È£ÃâÇØ¾ßÇÑ´Ù.
	}
	else
	{
		pObject->m_ObjectState.State_End_Time = gCurTime + EndStateCount;
		pObject->m_ObjectState.bEndState = TRUE;
	}
}

void CObjectStateManager::StateProcess(CObject* pObject)
{
	if(pObject->m_ObjectState.bEndState)
	{
		if(pObject->m_ObjectState.State_End_Time < gCurTime)
		{
			EndObjectState(pObject,pObject->m_BaseObjectInfo.ObjectState);
		}
		else
		{
			// ³¡³»±â µ¿?Û?¸·Î ?üÈ¯
			DWORD RemainTime = pObject->m_ObjectState.State_End_Time - gCurTime;
			if(RemainTime <= pObject->m_ObjectState.State_End_MotionTime)
			{
				pObject->m_ObjectState.State_End_MotionTime = 0;
				if(pObject->m_ObjectState.State_End_Motion != -1)
					pObject->ChangeMotion(pObject->m_ObjectState.State_End_Motion,FALSE);
			}
		}
	}
}

EObjectState CObjectStateManager::GetObjectState(CObject* pObject) const
{
	return pObject->GetState();
}

BOOL CObjectStateManager::IsEndStateSetted(CObject* pObject)
{
	return pObject->m_ObjectState.bEndState;
}

BOOL CObjectStateManager::CheckAvailableState(CObject* pObject,eCHECKAVAILABLESTATE cas)
{
	if( pObject->GetState() == eObjectState_Die )
	{
		//Á×?º »óÅÂ¿¡¼­´Â ?åÂø?» º¯°æÇÒ ¼ö ¾ø½?´Ï´Ù.
		return FALSE;
	}

	if(eCAS_EQUIPITEM != cas)
	{
		return TRUE;
	}

	switch(pObject->GetState())
	{
		case eObjectState_SkillStart:
		case eObjectState_SkillSyn:
		case eObjectState_SkillBinding:
		case eObjectState_SkillUsing:
		case eObjectState_SkillDelay:
			{
				return FALSE;
			}
	}

	return TRUE;
}

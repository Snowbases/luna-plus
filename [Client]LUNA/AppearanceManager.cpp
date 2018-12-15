// AppearanceManager.cpp: implementation of the CAppearanceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppearanceManager.h"

#include "Player.h"
#include "Monster.h"
#include "BossMonster.h"
#include "Npc.h"
#include ".\Engine\EngineObject.h"
#include ".\Engine\EngineEffect.h"
#include "ObjectStateManager.h"

#include "OptionManager.h"
#include "ObjectManager.h"
#include "GameResourceManager.h"
#include "ItemManager.h"
#include "MotionManager.h"
#include "CharMakeManager.h"
#include "MainGame.h"
#include "MHMap.h"
#include "MapObject.h"
#include "StreetStallManager.h"

#include "MHCamera.h"

// 070121 LYW --- Include header file.
#include "ObjectBalloon.h"
#include "OBalloonName.h"
#include "battle.h"
#include "BattleSystem_Client.h"
#include "..\[CC]BattleSystem\GTournament\Battle_GTournament.h"

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.10.05
#include "../hseos/Farm/SHFarmManager.h"
// E 농장시스템 추가 added by hseos 2007.10.05
#include "InventoryExDialog.h"
#include "GameIn.h"

#include "PetManager.h"

#include "Furniture.h"
//090521 pdy 하우징
#include "cHousingMgr.h"
#include "SiegeWarfareMgr.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CAppearanceManager)

CAppearanceManager::CAppearanceManager()
{
	// 090205 LYW --- AppearanceManager : Appear 처리에 사용할 변수 초기화 처리.
	memset( &m_AppearPlayer, 0, sizeof(CAppearPlayer) ) ;
}

CAppearanceManager::~CAppearanceManager()
{
	Release();
}

void CAppearanceManager::Init()
{
	GAMERESRCMNGR->LoadModFileList(GAMERESRCMNGR->m_ModFileList);
	GAMERESRCMNGR->LoadHairModFileList(GAMERESRCMNGR->m_HairModList);
	GAMERESRCMNGR->LoadFaceModFileList(GAMERESRCMNGR->m_FaceModList);
	GAMERESRCMNGR->LoadPetModList();

	m_bUseReservation = FALSE;
	m_pReserItemPlayer = new RESERVATION_ITEM_PLAYER[MAX_RESERVATION_ITEM_PLAYER];

	for (int i = 0; i < MAX_RESERVATION_ITEM_PLAYER; ++i)
	{
		m_lstFreeReserItemPlayer.AddTail(&m_pReserItemPlayer[i]);
	}

	m_dwUsedReserItemPlayerNum	= 0;
}

void CAppearanceManager::Release()
{	
	m_lstFreeReserItemPlayer.RemoveAll();

	SAFE_DELETE_ARRAY(
		m_pReserItemPlayer);
}

void CAppearanceManager::InitAppearanceToEffect(CObject* pObject,CEngineEffect* pEffect)
{
	if(pObject->GetObjectKind() == eObjectKind_Player)
	{
		CPlayer* pPlayer = (CPlayer*)pObject;
		SetCharacterAppearanceToEffect(pPlayer,pEffect);
	}
}

void CAppearanceManager::RenewalAppear(const BOOL bForEffect, CPlayer* pPlayer, CEngineObject* pEngineObject, DWORD changeFlag )
{
	// 함수 인자를 확인한다.
	if( !pPlayer || !pEngineObject ) return ;


	// 갱신시킬 Player 정보를 설정한다.
	m_AppearPlayer.SetPlayer( pPlayer ) ;
	m_AppearPlayer.SetEngineObject( pEngineObject ) ;

	// @@@ Iros 옵션 검사  LOD 최하 -> 박스졸라맨 으로 쓸 부분;
	if (MAINGAME->GetCurStateNum() == eGAMESTATE_GAMEIN && OPTIONMGR->GetGameOption()->nLODMode && pPlayer != HERO)
	{
		m_AppearPlayer.SetBaseAppearPlayer();
	}
	else
	{
		m_AppearPlayer.SetForEffect( bForEffect ) ;

		// Player의 모습을 갱신한다.
		if (changeFlag == eCHAPPEAR_INIT)
			m_AppearPlayer.RenewalAll();
		else
			m_AppearPlayer.Renewal(changeFlag) ;
	}
}

void CAppearanceManager::ShowWeapon(CPlayer* pPlayer)
{
	// 090205 LYW --- AppearanceManager : 호출 루틴 수정.
	// @@@ Iros 최적화 필요
	RenewalAppear( FALSE, pPlayer, &pPlayer->m_EngineObject, eCHAPPEAR_INIT ) ;
	//ShowWeapon(pPlayer, pPlayer->GetEngineObject());			
	
	if(pPlayer->GetCharacterTotalInfo()->bVisible == FALSE)	
	{
		// 090818 pdy HideWeapon ,ShowWeapon시 HideLevel처리가 빠져있어 수정
		if( pPlayer->GetCharacterTotalInfo()->HideLevel == 0 )
		{
#ifdef _GMTOOL_
		if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM && CAMERA->GetCameraMode() != eCM_EyeView )
			pPlayer->GetEngineObject()->SetAlpha( 0.3f );
		else
#endif
			pPlayer->GetEngineObject()->HideWithScheduling();
		}
		else
		{
			OBJECTMGR->SetHide( pPlayer->GetID(), pPlayer->GetCharacterTotalInfo()->HideLevel );
		}
	}

	OBJECTMGR->ApplyShadowOption(pPlayer);

	// 090116 ShinJS --- 캐릭터 안보이기 옵션에 대한 적용을 위해 실행
	OBJECTMGR->ApplyOverInfoOption(pPlayer);
}





void CAppearanceManager::HideWeapon(CPlayer* pPlayer)
{
	// 090205 LYW --- AppearanceManager : 호출 루틴 수정.
	// @@@ Iros 최적화 필요.
	//RenewalAppear( FALSE, pPlayer, &pPlayer->m_EngineObject, eCHAPPEAR_INIT ) ;

	HideWeapon(pPlayer, pPlayer->GetEngineObject());
	
	if(pPlayer->GetCharacterTotalInfo()->bVisible == FALSE)	
	{
		// 090818 pdy HideWeapon ,ShowWeapon시 HideLevel처리가 빠져있어 수정
		if( pPlayer->GetCharacterTotalInfo()->HideLevel == 0 )
		{
#ifdef _GMTOOL_
		if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM && CAMERA->GetCameraMode() != eCM_EyeView )
			pPlayer->GetEngineObject()->SetAlpha( 0.3f );
		else
#endif
			pPlayer->GetEngineObject()->HideWithScheduling();
		}
		else
		{
			OBJECTMGR->SetHide( pPlayer->GetID(), pPlayer->GetCharacterTotalInfo()->HideLevel );
		}
	}

	OBJECTMGR->ApplyShadowOption(pPlayer);

	// 090116 ShinJS --- 캐릭터 안보이기 옵션에 대한 적용을 위해 실행
	OBJECTMGR->ApplyOverInfoOption(pPlayer);
}





void CAppearanceManager::SetCharacterAppearance(CPlayer* pPlayer, DWORD changeFlag)
{
	// 090205 LYW --- AppearanceManager : 호출 루틴 수정.
	RenewalAppear( FALSE, pPlayer, &pPlayer->m_EngineObject, changeFlag ) ;

	if(pPlayer->GetState() == eObjectState_None)
		OBJECTSTATEMGR->EndObjectState(pPlayer,eObjectState_None);
	if( pPlayer->GetState() == eObjectState_Move &&
		pPlayer->m_MoveInfo.KyungGongIdx == 0)
		pPlayer->SetState(eObjectState_Move);
	if( pPlayer->GetState() == eObjectState_Rest )
	{	
		pPlayer->SetState(eObjectState_Rest);
	}

	// 090213 ShinJS --- 캐릭터 Apperance후 옵션 적용
	const CHARACTER_TOTALINFO* pInfo = pPlayer->GetCharacterTotalInfo();
	if( !pInfo )	return;

	// 캐릭터 정보가 Hide인 경우
    if( pInfo->bVisible == FALSE )
	{
		if( pInfo->HideLevel == 0 )
		{
#ifdef _GMTOOL_
		if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM && CAMERA->GetCameraMode() != eCM_EyeView )
			pPlayer->GetEngineObject()->SetAlpha( 0.3f );
		else
#endif
			pPlayer->GetEngineObject()->HideWithScheduling();
		}
		else
		{
			OBJECTMGR->SetHide( pPlayer->GetID(), pInfo->HideLevel );
		}
	}

	// 091123 공성전 워터시드 사용중 플레그 추가 
	if( pInfo->bUsingWaterSeed == TRUE )
	{
		SIEGEWARFAREMGR->Start_CravingDirection(pPlayer, eStep_Start) ;
	}
	
	// 옵션을 적용한다
	OBJECTMGR->ApplyOverInfoOption( pPlayer );
	OBJECTMGR->ApplyShadowOption( pPlayer );

	if( HOUSINGMGR->IsHousingMap() )
	{
		if( pPlayer->GetRideFurnitureID() )
		{
			HOUSINGMGR->RideOnPlayerToFuniture(pPlayer);
			APPEARANCEMGR->HideWeapon( pPlayer ) ;
		}
	}
}

void CAppearanceManager::SetBossMonsterAppearance(CBossMonster* pMonster)
{
	pMonster->m_EngineObject.Release();
	
	WORD mkind = pMonster->GetMonsterKind();
	BASE_MONSTER_LIST* pList = GAMERESRCMNGR->GetMonsterListInfo(mkind);
	
	pMonster->m_EngineObject.Init(pList->ChxName, pMonster, eEngineObjectType_Monster);
	pMonster->m_EngineObject.ApplyHeightField(TRUE);
	
	//////////////////////////////////////////////////////////////////////////
	// 몬스터 크기 적용
	VECTOR3 Scale;
	// taiyo test
	Scale.x = Scale.y = Scale.z = pList->Scale;
	pMonster->m_EngineObject.SetScale(&Scale);

	// desc_hseos_라이트효과01
	// S 라이트효과 추가 added by hseos 2007.06.13
	pMonster->GetEngineObject()->EnableSelfIllumin();
	// E 라이트효과 추가 added by hseos 2007.06.13
}

void CAppearanceManager::SetMonsterAppearance(CMonster* pMonster)
{
	pMonster->m_EngineObject.Release();
	
	WORD mkind = pMonster->GetMonsterKind();

	BASE_MONSTER_LIST* pList = GAMERESRCMNGR->GetMonsterListInfo(mkind);
	pMonster->m_EngineObject.Init(pList->ChxName,pMonster,eEngineObjectType_Monster);
	pMonster->m_EngineObject.ApplyHeightField(TRUE);


	/////////////////////////////////////////////////////////////////////////
	// taiyo 
	// 모션 정보 로드

	MOTIONMGR->SetMotionInfo(pMonster, 0);

	//////////////////////////////////////////////////////////////////////////
	// 몬스터 크기 적용
	VECTOR3 Scale;
	// taiyo test
	Scale.x = Scale.y = Scale.z = pList->Scale;
	pMonster->m_EngineObject.SetScale(&Scale);

	// desc_hseos_라이트효과01
	// S 라이트효과 추가 added by hseos 2007.06.13
	pMonster->GetEngineObject()->EnableSelfIllumin();
	// E 라이트효과 추가 added by hseos 2007.06.13
}
void CAppearanceManager::SetNpcAppearance(CNpc* pNpc)
{
	pNpc->m_EngineObject.Release();
		
	WORD nkind = pNpc->GetNpcKind();
	NPC_LIST* pList = GAMERESRCMNGR->GetNpcInfo(nkind);
	int ChxNum = pList->ModelNum;
	char* pNpcChxName = GAMERESRCMNGR->GetNpcChxName(ChxNum);
	
	pNpc->m_EngineObject.Init(pNpcChxName,pNpc,eEngineObjectType_Npc);
	pNpc->SetPosition(&pNpc->m_MoveInfo.CurPosition);
	pNpc->m_EngineObject.ApplyHeightField(TRUE);
	pNpc->SetAngle(pNpc->m_RotateInfo.Angle.ToRad());
	
	pNpc->m_EngineObject.ChangeMotion(1);

	pNpc->m_EngineObject.RandMotionFrame();
	
	if( pNpc->GetNpcJob() == 0 )
		pNpc->m_EngineObject.DisablePick();
	else if( pNpc->GetNpcKind() == 31 || pNpc->GetNpcJob() == MAP_MOVE_ROLE ||
		pNpc->GetNpcJob() == DUNGEON_WARP_ROLE)
	{
		pNpc->m_EngineObject.DisablePick() ;
	}

	//////////////////////////////////////////////////////////////////////////
	// Npc 크기 적용
	VECTOR3 Scale;
	//
	Scale.x = Scale.y = Scale.z = pList->Scale;
	pNpc->m_EngineObject.SetScale(&Scale);

	// desc_hseos_라이트효과01
	// S 라이트효과 추가 added by hseos 2007.06.13
	pNpc->GetEngineObject()->EnableSelfIllumin();
	// E 라이트효과 추가 added by hseos 2007.06.13
}

void CAppearanceManager::SetPetAppearance(CPet* pPet)
{
	int PartType;
	int PartModelNum;
	BOOL rt;

//	pPet->m_EngineObject.Release();
	
	DWORD kind = pPet->GetKind();

	PET_INFO* pInfo = PETMGR->GetPetInfo( kind );
	MOD_LIST* pModList = &GAMERESRCMNGR->mPetModList;
	
	pPet->m_EngineObject.Init( pInfo->ModelName, pPet, eEngineObjectType_Pet );
	pPet->m_EngineObject.ApplyHeightField(TRUE);
	
	for( int i = ePetEquipSlot_Face; i <= ePetEquipSlot_Tail; i++ ) 
	{
		DWORD itemidx = pPet->mWearedItem[ i ];
		ITEM_INFO* pInfo = ITEMMGR->GetItemInfo(itemidx);

		/// 아이템 정보 없다...
		if(!pInfo)
			continue;

		PartType = pInfo->Part3DType;
		PartModelNum = pInfo->Part3DModelNum;

		/// 3D 파트 정보가 없으면 아이콘만 표시되는 녀석들이다.
		if( PartType == ePartType_None )
			continue;
		if( PartModelNum == 65535 )
			continue;

		switch( PartType )
		{
		case ePartType_Pet_Face:
			{
				CEngineObject* pCostume = NULL;
				pCostume = new CEngineObject;
				pCostume->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Pet);
				rt = pPet->m_EngineObject.AttachDress(pCostume,"Bone02");	
			}
			break;
		case ePartType_Pet_Head:
			{
				CEngineObject* pCostume = NULL;
				pCostume = new CEngineObject;
				pCostume->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Pet);
				rt = pPet->m_EngineObject.AttachDress(pCostume,"Bone24");	
			}
			break;
		case ePartType_Pet_Wing:
			{
				CEngineObject* pCostume = NULL;
				pCostume = new CEngineObject;
				pCostume->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Pet);
				rt = pPet->m_EngineObject.AttachDress(pCostume,"Bone01");	
			}
			break;
		case ePartType_Pet_Tail:
			{
				CEngineObject* pCostume = NULL;
				pCostume = new CEngineObject;
				pCostume->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Pet);
				rt = pPet->m_EngineObject.AttachDress(pCostume,"Bone07");	
			}
			break;
		}
	}

	// 091110 ONS 펫 애니메이션 추가 : motionList.bin의 인덱스를 전달한다.
	MOTIONMGR->SetMotionInfo(pPet, 1);
	
	const float addScale = ( pPet->GetLevel() / 5 ) * 0.05f;

	VECTOR3 Scale = {0};
	Scale.x = Scale.y = Scale.z = 1.0f + addScale + pInfo->Scale;
	pPet->m_EngineObject.SetScale(
		&Scale);
	pPet->GetEngineObject()->EnableSelfIllumin();
	// E 라이트효과 추가 added by hseos 2007.06.13
	pPet->SetPosition(&pPet->m_MoveInfo.CurPosition);
	pPet->SetAngle(pPet->m_RotateInfo.Angle.ToRad());
}

// 090422 ShinJS --- 탈것추가작업
void CAppearanceManager::SetVehicleAppearance(CVehicle* pVehicle)
{
	pVehicle->m_EngineObject.Release();

	WORD wkind = pVehicle->GetMonsterKind();
	BASE_MONSTER_LIST* pList = GAMERESRCMNGR->GetMonsterListInfo( wkind );
	if( !pList )	return;

	pVehicle->m_EngineObject.Init( pList->ChxName, pVehicle, eEngineObjectType_Vehicle );
	pVehicle->m_EngineObject.ApplyHeightField( TRUE );
	pVehicle->SetPosition( &pVehicle->m_MoveInfo.CurPosition );

	MOTIONMGR->SetMotionInfo( pVehicle, 0 );
	
	pVehicle->m_EngineObject.ChangeMotion( 1 );

	// desc_hseos_라이트효과01
	// S 라이트효과 추가 added by hseos 2007.06.13
	pVehicle->GetEngineObject()->EnableSelfIllumin();
	// E 라이트효과 추가 added by hseos 2007.06.13
}

void CAppearanceManager::SetFurnitureAppearance(CFurniture* pObject)
{
	pObject->m_EngineObject.Release();

	//090324 pdy 
	DWORD FurnitureIdx = pObject->GetFurnitureInfo()->dwFurnitureIndex;
	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(FurnitureIdx);

	if(! stFurnitureInfo )
		return;

	if( ! stFurnitureInfo->szModelFileName[0] )
		return;

	pObject->m_EngineObject.Init( stFurnitureInfo->szModelFileName, pObject, eEngineObjectType_Furniture) ;

	//090325 pdy 가구는 공중에 떠있을수도 있다
	pObject->GetEngineObject()->ApplyHeightField(FALSE);
	pObject->SetPosition( &pObject->m_MoveInfo.CurPosition );
	pObject->GetEngineObject()->ApplyHeightField(TRUE);
	pObject->SetAngle( pObject->GetFurnitureInfo()->fAngle  );
	pObject->GetEngineObject()->EnableSelfIllumin();

	//090507 pdy 머터리얼 적용 추가 
	pObject->GetEngineObject()->SetMaterialIndex(pObject->GetFurnitureInfo()->nMaterialIndex);
}

void CAppearanceManager::SetMapObjectAppearance(CMapObject* pMapObject, char* strData, float fScale )
{
	pMapObject->m_EngineObject.Release();

	pMapObject->m_EngineObject.Init( strData, pMapObject, eEngineObjectType_MapObject) ;

	pMapObject->SetPosition( &pMapObject->m_MoveInfo.CurPosition );
	pMapObject->m_EngineObject.ApplyHeightField(TRUE);
	pMapObject->SetAngle( pMapObject->m_RotateInfo.Angle.ToRad() );

	VECTOR3 Scale;
	Scale.x = Scale.y = Scale.z = fScale;
	pMapObject->m_EngineObject.SetScale(&Scale);
}

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.05.07
void CAppearanceManager::SetFarmRenderObjAppearance(CSHFarmRenderObj* pFarmRenderObj)
{
	pFarmRenderObj->m_EngineObject.Release();
		
	WORD nKind = pFarmRenderObj->GetInfo()->nKind;
	CSHFarmManager::stFARMRENDEROBJLIST* pList = g_csFarmManager.CLI_GetFarmRenderObjList(nKind);

	// 090806 LUJ, 널 포인터 참조를 막도록 검사한다
	if( 0 == pList )
	{
		return;
	}
	
	pFarmRenderObj->m_EngineObject.Init(pList->szChxName, pFarmRenderObj, eEngineObjectType_FarmRenderObj);
	pFarmRenderObj->SetPosition(&pFarmRenderObj->m_MoveInfo.CurPosition);
	pFarmRenderObj->m_EngineObject.ApplyHeightField(TRUE);
	pFarmRenderObj->SetAngle(pFarmRenderObj->m_RotateInfo.Angle.ToRad());
	
	pFarmRenderObj->m_EngineObject.ChangeMotion(1);

	pFarmRenderObj->m_EngineObject.RandMotionFrame();
	
	//////////////////////////////////////////////////////////////////////////
	// Npc 크기 적용
	VECTOR3 Scale;
	//
	Scale.x = Scale.y = Scale.z = pList->nScale;
	pFarmRenderObj->m_EngineObject.SetScale(&Scale);
	pFarmRenderObj->GetEngineObject()->EnableSelfIllumin();
}
// E 농장시스템 추가 added by hseos 2007.05.07

void CAppearanceManager::SetCharacterAppearanceToEffect(CPlayer* pPlayer,CEngineEffect* pRtEffect)
{
	// 090205 LYW --- AppearanceManager : 호출 루틴 수정.
	RenewalAppear( TRUE, pPlayer, &pPlayer->m_EngineObject, eCHAPPEAR_INIT );
}

void CAppearanceManager::InitAppearance(CObject* pObject)
{
	if(pObject->GetObjectKind() == eObjectKind_Player)
	{
		SetCharacterAppearance((CPlayer*)pObject, eCHAPPEAR_INIT);
	}
	else if(pObject->GetObjectKind() == eObjectKind_Monster)
	{
		SetMonsterAppearance((CMonster*)pObject);
	}
	else if(pObject->GetObjectKind() == eObjectKind_Npc)
	{
		SetNpcAppearance((CNpc*)pObject);
	}
	else if(pObject->GetObjectKind() == eObjectKind_BossMonster)
	{
		SetBossMonsterAppearance((CBossMonster*)pObject);
	}
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.05.07
	else if (pObject->GetObjectKind() == eObjectKind_FarmObj)
	{
		SetFarmRenderObjAppearance((CSHFarmRenderObj*)pObject);
	}
	// E 농장시스템 추가 added by hseos 2007.05.07
	else if( pObject->GetObjectKind() == eObjectKind_Pet )
	{
		SetPetAppearance((CPet*)pObject);
	}
	// 090422 ShinJS --- 탈것추가작업
	else if( pObject->GetObjectKind() == eObjectKind_Vehicle )
	{
		SetVehicleAppearance((CVehicle*)pObject);
	}
	else if( pObject->GetObjectKind() == eObjectKind_Furniture )
	{
		SetFurnitureAppearance((CFurniture*)pObject);
	}
}

void CAppearanceManager::SetScalebyGuageBar(DWORD PlayerID)
{
	CObject* pPlayer;	
	pPlayer = OBJECTMGR->GetObject(PlayerID);
	if(pPlayer == NULL)
	{
		ASSERT(0);
		return;
	}
	ASSERT(pPlayer->GetObjectKind() == eObjectKind_Player);

}

void CAppearanceManager::AddCharacterPartChange(DWORD PlayerID, DWORD changeFlag/* = eCHAPPEAR_INIT*/)
{
	ChangeHashMap::iterator it;
	it = m_ChangeHashMap.find( PlayerID );

	if (it == m_ChangeHashMap.end())
	{
		APPEAR_CHANGE change;
		change.ChangeFlag = changeFlag;

		m_ChangeHashMap.insert(std::make_pair( PlayerID, change )) ;
	}
	else
		m_ChangeHashMap[PlayerID].ChangeFlag |= changeFlag;
}

void CAppearanceManager::ProcessAppearance()
{
	CObject* pPlayer;

	ChangeHashMap::iterator it ;
	for (it=m_ChangeHashMap.begin(); it!=m_ChangeHashMap.end(); ++it)
	{
		pPlayer = OBJECTMGR->GetObject((*it).first);

		if (pPlayer != NULL)
		{
			ASSERT(pPlayer->GetObjectKind() == eObjectKind_Player);
			SetCharacterAppearance((CPlayer*)pPlayer, (*it).second.ChangeFlag);
		}
	}
	m_ChangeHashMap.clear();
}


void CAppearanceManager::ShowWeapon(CPlayer* pPlayer,CEngineObject* pEngineObject)
{
	HideWeapon(
		pPlayer,pEngineObject);

	MOD_LIST& pModList = GAMERESRCMNGR->m_ModFileList[pPlayer->m_CharacterInfo.Race][pPlayer->m_CharacterInfo.Gender];	

	if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(pPlayer->GetWearedItemIdx(eWearedItem_Weapon)))
	{
		CEngineObject* pWeapon = new CEngineObject;

		pWeapon->Init(
			pModList.ModFile[itemInfo->Part3DModelNum],
			NULL,
			eEngineObjectType_Weapon);

		pEngineObject->AttachWeapon(
			pWeapon,
			eWeaponType_Bow == itemInfo->WeaponType ? LEFT_HAND : RIGHT_HAND);
	}

	if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(pPlayer->GetWearedItemIdx(eWearedItem_Shield)))
	{
		CEngineObject* pShield = new CEngineObject;

		pShield->Init(
			pModList.ModFile[itemInfo->Part3DModelNum],
			NULL,
			eEngineObjectType_Weapon);

		pEngineObject->AttachWeapon(
			pShield,
			LEFT_HAND);
	}
}

void CAppearanceManager::HideWeapon(CPlayer* pPlayer,CEngineObject* pEngineObject)
{
	// 함수 인자 확인.
	if( !pPlayer || !pEngineObject ) return ;

	// 무기 이펙트를 강제 종료 시킨다.
	EFFECTMGR->ForcedEndEffect( pPlayer->mWeaponEffect[0] ) ;
	EFFECTMGR->ForcedEndEffect( pPlayer->mWeaponEffect[1] ) ;
	EFFECTMGR->ForcedEndEffect( pPlayer->mWeaponEffect[2] ) ;

	pPlayer->mWeaponEffect[0] = 0 ;
	pPlayer->mWeaponEffect[1] = 0 ;
	pPlayer->mWeaponEffect[2] = 0 ;

	pEngineObject->RemoveAllAttachedWeapon();
}

VOID CAppearanceManager::CancelReservation(CObject* pObject)
{
	if (FALSE == m_bUseReservation)
	{
		return;
	}

	if (NULL == pObject)
	{
		return;
	}

	if (pObject->GetObjectKind() == eObjectKind_Player)
	{
		RESERVATION_ITEM_PLAYER* pReservItem = (RESERVATION_ITEM_PLAYER*)pObject->GetEngineObject()->GetCacheListData();

		if (NULL != pReservItem)
		{
			m_lstReserveToAddPlayerList.Remove((void*)pReservItem);
		}

		pObject->GetEngineObject()->SetCacheListData(NULL);
	}
}

VOID CAppearanceManager::CancelAlphaProcess(CObject* pObject)
{
	if (FALSE == m_bUseReservation)
	{
		return;
	}

	if (NULL == pObject)
	{
		return;
	}

	if (pObject->GetObjectKind() == eObjectKind_Player)
	{
		pObject->GetEngineObject()->SetAlpha(1.0f);
		m_lstAlphaProcessing.Remove(pObject);
	}
}

BOOL CAppearanceManager::ReserveToAppearObject(CObject* pObject, void* pMsg)
{
	if (FALSE == m_bUseReservation)
	{
		InitAppearance(pObject);
		return FALSE;
	}
		
	if(pObject->GetObjectKind() == eObjectKind_Player)
	{	

		RESERVATION_ITEM_PLAYER* pReservItem = AllocReservationItem();
		if (NULL != pReservItem)
		{
			pReservItem->pObject	= (CPlayer*)pObject;
			pReservItem->Msg		= *(SEND_CHARACTER_TOTALINFO*)pMsg;
			m_lstReserveToAddPlayerList.AddTail((void*)pReservItem);	
			
			pObject->GetEngineObject()->SetCacheListData((void*)pReservItem);			
		}
		else
		{
			pObject->GetEngineObject()->SetCacheListData(NULL);
		}
		
	}
	else
	{
		InitAppearance(pObject);
	}
	
	return TRUE;
}


#define REACTION_PROCESS_RESERVATION_TIME	200
#define LOAD_OBJECT_NUMBER_PER_PROCESS		3


VOID CAppearanceManager::ProcessReservation(void)
{
	if (FALSE == m_bUseReservation)
	{		
		return;
	}	
	
	DWORD dwCurrentTick = GetTickCount();

	if (dwCurrentTick >= m_dwLastProcessReservationTime + REACTION_PROCESS_RESERVATION_TIME)
	{	
		for(int i=0; i<LOAD_OBJECT_NUMBER_PER_PROCESS; ++i)
		{
			if (0 < m_lstReserveToAddPlayerList.GetCount())
			{
				RESERVATION_ITEM_PLAYER* pReservItem = NULL;
				pReservItem = (RESERVATION_ITEM_PLAYER*)m_lstReserveToAddPlayerList.RemoveHead();

				if (NULL != pReservItem)
				{
					CPlayer* pPlayer = pReservItem->pObject;

					if (NULL != pPlayer)
					{
						InitAppearance(pPlayer);
						
						PostProcessAddPlayer(pPlayer, &pReservItem->Msg);

						// 090316 LUJ, 지연되어 초기화된 오브젝트를 탑승 처리한다
						{
							CVehicle* const vehicle = ( CVehicle* )OBJECTMGR->GetObject( pPlayer->GetVehicleID() );

							if( vehicle && vehicle->GetObjectKind() == eObjectKind_Vehicle )
							{
								vehicle->GetOn( pPlayer, pPlayer->GetVehicleSeatPos() );
							}
						}
						
						if( pPlayer->GetCharacterTotalInfo()->bVisible )
						{
							pPlayer->GetEngineObject()->SetAlpha(0.2f);
							m_lstAlphaProcessing.AddTail(pPlayer);
						}
					}

					FreeReservationItem(pReservItem);
					pReservItem = NULL;

					pPlayer->GetEngineObject()->SetCacheListData(NULL);

				}
			}
		}

		m_dwLastProcessReservationTime = dwCurrentTick;
	}
}



RESERVATION_ITEM_PLAYER* CAppearanceManager::AllocReservationItem(VOID)
{

	if (m_dwUsedReserItemPlayerNum < MAX_RESERVATION_ITEM_PLAYER)
	{
		++m_dwUsedReserItemPlayerNum;
		return (RESERVATION_ITEM_PLAYER*)m_lstFreeReserItemPlayer.RemoveHead();
	}

	return NULL;
}

BOOL CAppearanceManager::FreeReservationItem(RESERVATION_ITEM_PLAYER* pItem)
{
	if (NULL == pItem)
	{
		return FALSE;
	}

	if (0 < m_dwUsedReserItemPlayerNum)
	{
		--m_dwUsedReserItemPlayerNum;
		m_lstFreeReserItemPlayer.AddTail((void*)pItem);
	}

	return TRUE;
}


VOID CAppearanceManager::ProcessAlphaAppearance(VOID)
{		
	float fAlpha = 0.0f;

	cPtrList removelist;

	PTRLISTPOS pos = m_lstAlphaProcessing.GetHeadPosition();
	
	while( pos )
	{
		CObject* pObject = (CObject*)m_lstAlphaProcessing.GetNext(pos);
		
		if( pObject )
		{
			fAlpha = pObject->GetEngineObject()->GetAlpha();			
			
			if (0.9f <= fAlpha)
			{
				pObject->GetEngineObject()->SetAlpha(1.0f);					
				
				//m_lstAlphaProcessing.GetNext(pos);
				//m_lstAlphaProcessing.Remove(pObject);
				removelist.AddTail(pObject);
			}
			else
			{
				fAlpha += 0.025f;
				
				pObject->GetEngineObject()->SetAlpha(fAlpha);			
			}
		}
	}

	pos = removelist.GetHeadPosition();
	while( pos )
	{
		CObject* pObject = (CObject*)removelist.GetNext(pos);
		if( pObject )
			m_lstAlphaProcessing.Remove( pObject );
	}
	removelist.RemoveAll();
}

void PostProcessAddPlayer(CPlayer* pPlayer, SEND_CHARACTER_TOTALINFO* pmsg)
{
	if (NULL == pPlayer)
	{
		return;
	}

	if (NULL == pmsg)
	{
		return;
	}


	OBJECTSTATEMGR->InitObjectState(pPlayer);
	OBJECTMGR->ApplyOverInfoOption( pPlayer );
	OBJECTMGR->ApplyShadowOption( pPlayer );

	///


	BOOL bShowdown = FALSE;
	BOOL bGTournament = FALSE;
	int wGTournamentTeam = 0;
	WORD StallKind = 0;
	COMPRESSEDPOS ShowdownPos;
	static char StreetStallTitle[MAX_STREETSTALL_TITLELEN+1] = {0,};

	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)			
			yCASE(CAddableInfoList::ShowdownInfo)
				bShowdown = TRUE;
				iter.GetInfoData(&ShowdownPos);
			yCASE(CAddableInfoList::StreetStall)
				StallKind = eSK_SELL;
				iter.GetInfoData(StreetStallTitle);
			yCASE(CAddableInfoList::StreetBuyStall)
				StallKind = eSK_BUY;
				iter.GetInfoData(StreetStallTitle);
			yCASE(CAddableInfoList::GTournament)
				bGTournament = TRUE;
				iter.GetInfoData( &wGTournamentTeam );
		yENDSWITCH
		
		iter.ShiftToNextData();
	}

	if( pmsg->TotalInfo.bPKMode )
	{
		//OBJECTEFFECTDESC desc(FindEffectNum("maintain_PK_S.beff"));
		OBJECTEFFECTDESC desc( 39 );	//---KES PK
		pPlayer->AddObjectEffect( PK_EFFECT_ID, &desc, 1 );
	}

	if(pmsg->bLogin)
	{
		TARGETSET set;
		set.pTarget = pPlayer;


		if( bGTournament && 2==wGTournamentTeam)
			;
		else
			EFFECTMGR->StartEffectProcess(eEffect_NewCharacter,pPlayer,&set,0,pPlayer->GetID());


		pPlayer->ChangeBaseMotion( pPlayer->GetStandardMotion() );
	}
	else
	{
		if( pmsg->BaseObjectInfo.ObjectState == eObjectState_StreetStall_Owner )
		{
			if(StallKind == eSK_SELL)
				pPlayer->ShowStreetStallTitle( TRUE, StreetStallTitle );
			else if(StallKind == eSK_BUY)
				pPlayer->ShowStreetBuyStallTitle( TRUE, StreetStallTitle );
		}
	}

	if( bGTournament )
	{		
		if(2 == wGTournamentTeam)
		{
			((CBattle_GTournament*)BATTLESYSTEM->GetBattle())->SetGTName( pPlayer, eGTNameType_Max );

			// Observer
			pPlayer->GetCharacterTotalInfo()->bVisible = false;
			if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM && CAMERA->GetCameraMode() != eCM_EyeView )
				pPlayer->GetEngineObject()->SetAlpha( 0.3f );
			else
				pPlayer->GetEngineObject()->HideWithScheduling();
		}
		else if(HERO->GetBattleTeam() == wGTournamentTeam)
		{
			((CBattle_GTournament*)BATTLESYSTEM->GetBattle())->SetGTName( pPlayer, eGTNameType_OurGuild );
		}
		else
		{
			((CBattle_GTournament*)BATTLESYSTEM->GetBattle())->SetGTName( pPlayer, eGTNameType_Enemy );
		}
	}
}
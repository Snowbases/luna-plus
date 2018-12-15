// ----------------------------- 
// 090422 ShinJS --- 탈것 Class
// ----------------------------- 
#include "stdafx.h"
#include "Vehicle.h"

#include "GameResourceManager.h"
#include "ObjectManager.h"
#include "TileManager.h"
#include "MoveManager.h"
#include "input/UserInput.h"
#include "MHMap.h"
#include "AppearanceManager.h"
#include "MOTIONDESC.h"
#include "ChatManager.h"
#include "ObjectStateManager.h"
#include "MHCamera.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"
#include "VehicleManager.h"
#include "PetStateDialog.h"
#include "GameIn.h"
#include "../[cc]skill/client/manager/SkillManager.h"
#include "../[cc]skill/client/info/SkillInfo.h"
#include "CharacterDialog.h"

CVehicle::CVehicle()
{
	m_htRiderInfo.Initialize( VehicleScript::Seat::MaxSize );
}

CVehicle::~CVehicle()
{
	Release();
}

void CVehicle::InitVehicle()
{
	m_bInstalled = FALSE;
	m_dwLastDistCheckTime = 0;
	m_dwRequestGetOnPlayerID = 0;
	ZeroMemory( &mUsedItemToSummon, sizeof( mUsedItemToSummon ) );
	memset( m_MasterName, 0, MAX_NAME_LENGTH+1 );
	m_dwExecutedEffectHandle = 0;
	m_nExecutedEffectNum = -1;
	m_bFollow = TRUE;
}

void CVehicle::Release()
{
	if( HasRider() )
	{
		// Release에 들어오는 경우 자기자신이 ObjectManager의 ObjectTable에 이미 제거된 상태에서 들어오기 때문에
		// GetOff()를 실행하는 경우 원하는 결과를 얻을 수 없다. 직접 Detach 작업을 실행해줘야 한다
		m_htRiderInfo.SetPositionHead();

		for(DWORD dwPlayerID = DWORD(m_htRiderInfo.GetData());
			0 < dwPlayerID;
			dwPlayerID = DWORD(m_htRiderInfo.GetData()))
		{
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( dwPlayerID );

			if( pPlayer && pPlayer->GetObjectKind() == eObjectKind_Player )
			{
				// 탑승해제 시킨다
				m_EngineObject.DetachRider( pPlayer->GetEngineObject() );

				// Player의 Vehicle정보 초기화
				pPlayer->SetVehicleID( 0 );											// 탈것 ID 초기화
				pPlayer->SetVehicleSeatPos( 0 );									// 탑승 위치 초기화
				pPlayer->SetBaseMotion();											// 기본모션 재설정
				MOVEMGR->MoveStop( pPlayer, NULL );									// 내린 자리에 멈춤

				APPEARANCEMGR->InitAppearance( pPlayer );
			}
		}
		m_htRiderInfo.RemoveAll();
	}

	// 091110 ShinJS --- 현재 실행중인 Effect 종료
	if( m_dwExecutedEffectHandle )
	{
		while( EFFECTMGR->ForcedEndEffect( m_dwExecutedEffectHandle ) == FALSE )
		{}
		m_dwExecutedEffectHandle = NULL;
	}
	
	CObject::Release();
}

void CVehicle::Process()
{
	// 설치되지 않은 상태
	if( !m_bInstalled )
		return;

	CMonster::Process();
	
	if( DoGetMoveSpeed() <= 0.0f ||
		GetOwnerIndex() != gHeroID ||
		m_bDieFlag ||
		IsShocked() )
		return;

	CPlayer* pMaster = (CPlayer*)OBJECTMGR->GetObject(
		GetOwnerIndex());

	if( !pMaster || pMaster->GetObjectKind() != eObjectKind_Player )
	{
		return;
	}

	// Master의 좌석위치 구하기
	DWORD dwMasterSeatPos = pMaster->GetVehicleSeatPos();

	if( m_bFollow &&
		dwMasterSeatPos == 0 &&
		gCurTime - m_dwLastDistCheckTime > 1000 )
	{
		VECTOR3 vecMasterPos, vecVehiclePos;

		pMaster->GetPosition( &vecMasterPos );
		GetPosition( &vecVehiclePos );
		float fDist = CalcDistanceXZ( &vecMasterPos, &vecVehiclePos );

		// 마지막 거리 확인시각 갱신
		m_dwLastDistCheckTime = gCurTime;

		BASE_MONSTER_LIST* pList = GAMERESRCMNGR->GetMonsterListInfo( GetMonsterKind() );
		ASSERT( pList );
		if( !pList )		return;

		const float fVehicleRadius = float( pList->MonsterRadius );			// 탈것 반지름
		fDist -= fVehicleRadius;

		// 따라가기가 필요한 경우
		if( fDist > CVehicle::eFollowExecuteDist )
		{
			MOVE_INFO* pMasterMoveInfo = pMaster->GetBaseMoveInfo();

			if( !pMasterMoveInfo )	return;

			// 따라가기 위치 구하기			
			VECTOR3 vecDir, vecFollowPos, vecUp, vecCross;
			float fRandDist;

			// Master가 바라보는 반대방향에 위치할수 있도록 한다
			vecDir = pMasterMoveInfo->Move_Direction * -1.0f;
			Normalize( &vecDir, &vecDir );

			fRandDist = float( rand() % CVehicle::eFollowRandomDist );

			vecUp.x = 0.0f;		vecUp.y = 1.0f;		vecUp.z = 0.0f;
			CrossProduct( &vecCross, &vecUp, &vecDir );

			// Master의 반대방향과 탈것의 반지름위치, 
			vecDir = vecDir * ( fVehicleRadius + fRandDist ) + vecCross * fRandDist * float( rand()%2 ? 1 : -1 );
			vecFollowPos = vecMasterPos + vecDir;

			if( vecFollowPos.x < 0 )				vecFollowPos.x = 0;
			else if( vecFollowPos.x > 51199 )		vecFollowPos.x = 51199;

			if( vecFollowPos.z < 0 )				vecFollowPos.z = 0;
			else if( vecFollowPos.z > 51199 )		vecFollowPos.z = 51199;

			vecFollowPos.x = ((DWORD)(vecFollowPos.x/TILECOLLISON_DETAIL)*TILECOLLISON_DETAIL);
			vecFollowPos.z = ((DWORD)(vecFollowPos.z/TILECOLLISON_DETAIL)*TILECOLLISON_DETAIL);
			vecFollowPos.y = 0.0f;

			// 순간이동이 필요한 경우(너무 멀어진 경우)
			if( fDist > CVehicle::eRecallExecuteDist )
			{
				MOVE_POS message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category = MP_MOVE;
				message.Protocol = MP_MOVE_VEHICLE_RECALL_SYN;
				message.dwObjectID = pMaster->GetID();
				message.dwMoverID = GetID();
				message.cpos.Compress( &vecFollowPos );
				NETWORK->Send( &message,sizeof( MOVE_POS ) );
			}
			// 따라가기
			else
			{
				MOVE_ONETARGETPOS message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category = MP_MOVE;
				message.Protocol = MP_MOVE_VEHICLE_BEGIN_SYN;
				message.dwObjectID = pMaster->GetID();
				message.dwMoverID = GetID();
				message.SetStartPos( &GetCurPosition() );
				message.SetTargetPos( &vecFollowPos );
				NETWORK->Send( &message, sizeof( message ) );
			}
		}
	}
}

void CVehicle::SetMotionInState(EObjectState State)
{
	// 091110 ShinJS --- 모션을 Beff 파일을 이용하여 변경
	m_ObjectState.State_Start_Motion = -1;
	m_ObjectState.State_Ing_Motion = -1;
	m_ObjectState.State_End_Motion = -1;
	m_ObjectState.State_End_MotionTime = 0;
	m_ObjectState.MotionSpeedRate = 1.0f;

	// 설치되지 않은 상태
	if( !m_bInstalled )
		return;

	switch(State)
	{
	case eObjectState_Move:
		{
			CObject* const pObject = OBJECTMGR->GetObject(
				GetOwnerIndex());

			if( !pObject )		break;

			switch( pObject->GetBaseMoveInfo()->MoveMode )
			{
			case eMoveMode_Walk:
				{
					ChangeBeff( VehicleScript::Motion::MoveObjectWalk );
				}
				break;

			case eMoveMode_Run:
				{
					ChangeBeff( VehicleScript::Motion::MoveObjectRun );
				}
				break;
			}
		}
		break;
	case eObjectState_None:
		{
			ChangeBeff( VehicleScript::Motion::MoveObjectIdle );
		}
		break;
	}
}

float CVehicle::DoGetMoveSpeed()
{
	const BASE_MONSTER_LIST* const pList = GAMERESRCMNGR->GetMonsterListInfo( GetMonsterKind() );

	if( 0 == pList )
	{
		return 0;
	}

	float moveSpeed = 0;

	switch( GetBaseMoveInfo()->MoveMode )
	{
	case eMoveMode_Walk:
		{
			moveSpeed = pList->WalkMove;
			break;
		}
	case eMoveMode_Run:
		{
			moveSpeed = pList->RunMove;
			break;
		}
	}

	return moveSpeed;
}

void CVehicle::SetPosToMousePos()
{
	// 설치되지 않은 상태
	if( !m_bInstalled )
	{
		CObject* pOverObject = (CObject*)GetSelectedObject( MOUSE->GetMouseX(), MOUSE->GetMouseY() );

		// 선택된 오브젝트가 존재하고 현재 설치할 탈것Object가 아닌경우 그리지 않는다
		if( pOverObject && pOverObject != this )
		{
			if( m_EngineObject.IsShow() )
				m_EngineObject.Hide();
			return;
		}

		VECTOR3 *vecPickPos;
		vecPickPos = GetPickedPosition( MOUSE->GetMouseX(), MOUSE->GetMouseY() );
		if( vecPickPos )
		{	
			if( !m_EngineObject.IsShow() )
				m_EngineObject.Show();

			MOVEMGR->SetPosition( this, vecPickPos );		// 이동정보에 위치를 세팅한다.
		}

		return;
	}
}

BOOL CVehicle::IsValidPosForInstall()
{
	// 탈것 Object의 맵 타일 검사
	COLLISION_MESH_OBJECT_DESC ColMeshDescVehicle;
	g_pExecutive->GXOGetCollisionMesh( GetEngineObject()->GetGXOHandle(), &ColMeshDescVehicle);
	
	VECTOR3 vecVehicleMin, vecVehicleMax;
	vecVehicleMin.x = vecVehicleMin.z = 3.402823466e+38F;	// FLT_MAX
	vecVehicleMax.x = vecVehicleMax.z = 1.175494351e-38F;	// FLT_MIN
	vecVehicleMin.y = vecVehicleMax.y = 0.0f;

	for( int i=0 ; i<8 ; ++i )
	{
		if( ColMeshDescVehicle.boundingBox.v3Oct[i].x < vecVehicleMin.x )		vecVehicleMin.x = ColMeshDescVehicle.boundingBox.v3Oct[i].x;
		if( ColMeshDescVehicle.boundingBox.v3Oct[i].x > vecVehicleMax.x )		vecVehicleMax.x = ColMeshDescVehicle.boundingBox.v3Oct[i].x;

		if( ColMeshDescVehicle.boundingBox.v3Oct[i].z < vecVehicleMin.z )		vecVehicleMin.z = ColMeshDescVehicle.boundingBox.v3Oct[i].z;
		if( ColMeshDescVehicle.boundingBox.v3Oct[i].z > vecVehicleMax.z )		vecVehicleMax.z = ColMeshDescVehicle.boundingBox.v3Oct[i].z;
	}

	BOUNDING_SPHERE sphere;
	sphere.fRs = TILECOLLISON_DETAIL;
	sphere.v3Point.y = ColMeshDescVehicle.boundingSphere.v3Point.y;
	for( float x=vecVehicleMin.x ; x < vecVehicleMax.x ; x += TILECOLLISON_DETAIL )
	{
		for( float z=vecVehicleMin.z ; z < vecVehicleMax.z ; z += TILECOLLISON_DETAIL )
		{
			sphere.v3Point.x = x;
			sphere.v3Point.z = z;

			// 체크해야하는 Tile 인 경우
			if( IsCollisionSphereAndBox( &sphere, &ColMeshDescVehicle.boundingBox ) )
			{
				// Tile 검사
				if( MAP->CollisionTilePos( int(sphere.v3Point.x), int(sphere.v3Point.z), MAP->GetMapNum(), this ) )
					return FALSE;
			}
		}
	}

	// NPC와 충돌 검사
	VECTOR3 vecVehiclePos, vecObjPos;
	CYHHashTable<CObject>& htObjectTable = OBJECTMGR->GetObjectTable();

	htObjectTable.SetPositionHead();

	for(CObject* pObject = htObjectTable.GetData();
		0 < pObject;
		pObject = htObjectTable.GetData())
	{
		if( pObject->GetObjectKind() == eObjectKind_Npc )
		{
			// 090604 LUJ, 충돌박스가 지정되어 있지 않은 NPC가 있을 경우가 있으므로 검사한다
			COLLISION_MESH_OBJECT_DESC ColMeshDesc = { 0 };
			CEngineObject* const engineObject = pObject->GetEngineObject();

			if( 0 == engineObject )
			{
				continue;
			}
			else if( 0 == engineObject->GetGXOHandle() )
			{
				continue;
			}

			g_pExecutive->GXOGetCollisionMesh( engineObject->GetGXOHandle(), &ColMeshDesc);

			VECTOR3 vecObjMin, vecObjMax;
			vecObjMin.x = vecObjMin.z = 3.402823466e+38F;	// FLT_MAX
			vecObjMax.x = vecObjMax.z = 1.175494351e-38F;	// FLT_MIN
			vecObjMin.y = vecObjMax.y = 0.0f;

			for( int i=0 ; i<8 ; ++i )
			{
				if( ColMeshDesc.boundingBox.v3Oct[i].x < vecObjMin.x )		vecObjMin.x = ColMeshDesc.boundingBox.v3Oct[i].x;
				if( ColMeshDesc.boundingBox.v3Oct[i].x > vecObjMax.x )		vecObjMax.x = ColMeshDesc.boundingBox.v3Oct[i].x;

				if( ColMeshDesc.boundingBox.v3Oct[i].z < vecObjMin.z )		vecObjMin.z = ColMeshDesc.boundingBox.v3Oct[i].z;
				if( ColMeshDesc.boundingBox.v3Oct[i].z > vecObjMax.z )		vecObjMax.z = ColMeshDesc.boundingBox.v3Oct[i].z;
			}
			
			VECTOR3 vecTmp;
			vecObjPos = ColMeshDesc.boundingSphere.v3Point;
			vecVehiclePos = ColMeshDescVehicle.boundingSphere.v3Point;
			vecObjPos.y = vecVehiclePos.y = 0.0f;
			vecTmp = vecObjPos - vecVehiclePos;
			float fObjDist = VECTOR3Length( &vecTmp );					// Object와 탈것과의 거리

			vecTmp = vecObjMax - vecObjMin;
			float fObjRadius = VECTOR3Length( &vecTmp ) / 2.0f;			// Object의 반지름

			vecTmp = vecVehicleMax - vecVehicleMin;
			float fVehicleRadius = VECTOR3Length( &vecTmp ) / 2.0f;		// Vehicle의 반지름

			if( fObjDist < fVehicleRadius + fObjRadius )
				return FALSE;
		}
	}
	return TRUE;
}

void CVehicle::GetOn( CPlayer* pPlayer, DWORD dwSeatPos )
{
	if( 0 == pPlayer )
	{
		return;
	}

	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );

	if( dwSeatPos == 0 ||
		dwSeatPos > script.mSeatSize )
	{
		return;
	}
	// 탑승인원 초과
	else if( m_htRiderInfo.GetDataNum() >= script.mSeatSize )
	{
		return;
	}

	// 090316 LUJ, 오브젝트가 지연되어 초기화될 수 있다. 이후에도 탑승 처리가 되도록 값은 설정한다.
	pPlayer->SetVehicleID( GetID() );
	pPlayer->SetVehicleSeatPos( dwSeatPos );

	// 090424 ShinJS --- 탑승자 추가(중복되지 않도록 조건이 일치할때 Attach 한다)
	if( 0 == m_htRiderInfo.GetData( dwSeatPos ) &&			// 해당 좌석에 탑승자가 없고
		pPlayer->GetEngineObject()->IsInited() &&			// 탑승자와
		TRUE == GetEngineObject()->IsInited() )				// 탈것의 CEngineObject가 Init되어 있는경우(Attach가능)
	{
		// 090422 ShinJS --- 탈것이 고정형이고 탑승자가 없다가 탑승한 경우 애니메이션을 실행해준다
		if( DoGetMoveSpeed() <= 0.0f )
		{
			// 탑승자를 추가전에 탑승자가 없었는지 판단
			if( !HasRider() )
			{
				// 없었던 경우 애니메이션 실행
				// 091110 ShinJS --- 모션을 Beff 파일을 이용하여 변경
				ChangeBeff( VehicleScript::Motion::FixObjectPlay );
			}

			// 091215 ShinJS --- 고정형인 경우 Effect를 숨긴다
			pPlayer->HideEffect( TRUE );

			// 091215 ShinJS --- 고정형인 경우 펫의 AI 상태를 Stand로 변경한다.
			if( pPlayer->GetID() == HEROID && HEROPET )
			{
				HEROPET->SetAI( ePetAI_Stand );
			}
		}
		// 100326 ShinJS --- 탑승 완료후 현재상태의 모션 실행
		else
		{	
			SetMotionInState( m_BaseObjectInfo.ObjectState );
		}

		m_htRiderInfo.Add( ( DWORD* )pPlayer->GetID(), dwSeatPos );			// 탑승자 추가
		
		m_EngineObject.AttachRider(
			pPlayer->GetEngineObject(),
			const_cast< char* >( script.mSeat[ dwSeatPos - 1 ].mAttachedName ) );

		pPlayer->SetBaseMotion();
		OBJECTSTATEMGR->InitObjectState(pPlayer);
		// 090428 ShinJS --- 탈것의 방향으로 탑승자의 방향을 변경한다
		pPlayer->SetAngle( GetAngle() );

		// 091105 ShinJS --- 무기 제거
		APPEARANCEMGR->HideWeapon( pPlayer );
	}

	if( pPlayer->GetID() == HEROID )
	{
		GAMEIN->GetCharacterDialog()->RefreshInfo();
	}
}

void CVehicle::GetOffAll()
{
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );

	for( DWORD seatIndex = 1; seatIndex <= script.mSeatSize; ++seatIndex )
	{
		CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( DWORD(m_htRiderInfo.GetData( seatIndex )) );
		GetOff( pPlayer, seatIndex );
	}
}

BOOL CVehicle::GetOff( CPlayer* pPlayer, DWORD dwSeatPos )
{
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );

	// 인자 확인 (지정된 좌석번호 확인)
	if( dwSeatPos == 0 || dwSeatPos > script.mSeatSize )		
		return FALSE;

	// 090427 ShinJS --- Player 확인
	DWORD dwPlayerID = (DWORD)m_htRiderInfo.GetData( dwSeatPos );
	if( !pPlayer || 
		pPlayer->GetObjectKind() != eObjectKind_Player || 
		pPlayer->GetID() != dwPlayerID )
		return FALSE;

	m_EngineObject.DetachRider( pPlayer->GetEngineObject() );
	
	// 탑승자 제거
	m_htRiderInfo.Remove( dwSeatPos );

	// Master가 아닌경우 탈것ID정보 초기화
	if(dwPlayerID != GetOwnerIndex())
		pPlayer->SetVehicleID( 0 );										// 탈것 ID 초기화
	pPlayer->SetVehicleSeatPos( 0 );									// 탑승 위치 초기화
	pPlayer->SetBaseMotion();											// 기본모션 재설정

	MOVEMGR->MoveStop( pPlayer, NULL );									// 내린 자리에 멈춤

	// 091105 ShinJS --- 무기 보이기
	APPEARANCEMGR->ShowWeapon( pPlayer );

	APPEARANCEMGR->InitAppearance( pPlayer );							// 탑승해제 후 다른 Player의 경우 보이지 않아 보이게 함

	// 090422 ShinJS --- 탑승자가 HERO인 경우 카메라 FollowView로 한번 실행한다
    if( pPlayer->GetID() == HEROID )
	{
		CAMERA->ExecuteOnceFollowView();
		GAMEIN->GetCharacterDialog()->RefreshInfo();

		// 버프효과가 있으면 메시지출력.
		DWORD dwSkillIndex = 0;
		for(int i=0; i<VehicleScript::Condition::BuffMaxSize; i++)
		{
			dwSkillIndex = script.mCondition.mBuffSkillIndex[i];
			cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo(dwSkillIndex);
			if(pSkillInfo)
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2004 ), pSkillInfo->GetName());
		}

		GAMEIN->GetCharacterDialog()->RefreshInfo();
	}

	// 090422 ShinJS --- 탈것이 고정형이고 탑승자가 존재하지 않는 상태가 된 경우 애니메이션을 정지시킨다
	if( DoGetMoveSpeed() <= 0.0f )
	{
		// 탑승자를 제거후에 탑승자가 없는지 판단
		if( !HasRider() )
		{
			// 없는 경우 애니메이션 Stop로 변경
			// 091110 ShinJS --- 모션을 Beff 파일을 이용하여 변경
			ChangeBeff( VehicleScript::Motion::FixObjectStop );
		}

		// 091215 ShinJS --- 고정형인 경우 Effect 숨김상태를 해제한다.(캐릭터가 Hide 상태가 아닌경우 해제한다)
		if( pPlayer->GetCharacterTotalInfo()->bVisible != FALSE )
			pPlayer->HideEffect( FALSE );

		// 091215 ShinJS --- 고정형인 경우 HEROPET의 AI상태를 원래대로 되돌린다.
		if( pPlayer->GetID() == HEROID && HEROPET )
		{
			if(CPetStateDialog* pPetStateDialog = GAMEIN->GetPetStateDlg())
			{
				HEROPET->SetAI(
					ePetAI(pPetStateDialog->GetAISelectedIdx()));
			}
		}
	}

	return TRUE;
}

BOOL CVehicle::IsGetOn( CPlayer* pPlayer )
{	
	ASSERT( pPlayer );
	if( !pPlayer )		return FALSE;

	DWORD dwSeatPos = pPlayer->GetVehicleSeatPos();
	if( DWORD(m_htRiderInfo.GetData( dwSeatPos )) == pPlayer->GetID() )
		return TRUE;

	return FALSE;
}

DWORD CVehicle::CanGetOn( CPlayer* pPlayer )
{
	// Player 상태 검사
	switch( pPlayer->GetState() )
	{
	case eObjectState_Rest:
	case eObjectState_Exchange:
	case eObjectState_StreetStall_Owner:
	case eObjectState_StreetStall_Guest:
	case eObjectState_PrivateWarehouse:
	case eObjectState_Munpa:

	case eObjectState_SkillStart:
	case eObjectState_SkillSyn:
	case eObjectState_SkillBinding:
	case eObjectState_SkillUsing:
	case eObjectState_SkillDelay:
	
	case eObjectState_TiedUp_CanMove:
	case eObjectState_TiedUp_CanSkill:
	case eObjectState_TiedUp:
	
	case eObjectState_Die:
	case eObjectState_Fishing:
	case eObjectState_FishingResult:

	case eObjectState_Cooking:
	case eObjectState_ItemUse:
	case eObjectState_Society:
	case eObjectState_Exit:
	case eObjectState_BattleReady:
	case eObjectState_Deal:
		return 0;
	}

	// 091106 ShinJS --- PK 모드상태 탑승 불가
	if( pPlayer->IsPKMode() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1263 ) );
		return 0;
	}

	// 탈것의 탑승가능 레벨과 플레이어 레벨 비교 검사
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );
	
	if( pPlayer->GetLevel() < script.mCondition.mPlayerMinimumLevel )
	{
		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			CHATMGR->GetChatMsg(2210),
			script.mCondition.mPlayerMinimumLevel,
			script.mCondition.mPlayerMaximumLevel);
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			text);
		return 0;
	}
	else if( pPlayer->GetLevel() > script.mCondition.mPlayerMaximumLevel )
	{
		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			CHATMGR->GetChatMsg(2210),
			script.mCondition.mPlayerMinimumLevel,
			script.mCondition.mPlayerMaximumLevel);
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			text);
		return 0;
	}
	else if( pPlayer->GetID() != GetOwnerIndex() &&
			pPlayer->GetVehicleID() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1834 ) );
		return 0;
	}

	// 이미 탑승중인 경우 (팝업과 매크로키의 조작시 가능)
	if( IsGetOn( pPlayer ) )
	{
		// 메세지 없이 처리 하지 않도록 한다
		return 0;
	}

	// 090922 ShinJS --- HERO인 경우 조합/분해/인챈/강화 다이얼로그가 활성화 중인 경우 탑승하지 못하도록 한다.
	if( pPlayer->GetID() == HEROID )
	{
		cDialog* pMixDialog				= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );			// 조합
		cDialog* pDissolutioniDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );	// 분해
		cDialog* pReinforceDialog		= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );	// 강화
		cDialog* pUpgradeDialog			= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );		//인첸트

		if( !pMixDialog				|| 
			!pDissolutioniDialog	|| 
			!pReinforceDialog		|| 
			!pUpgradeDialog	) 
		{
			return 0;
		}

		if(	pMixDialog->IsActive()			|| 
			pDissolutioniDialog->IsActive() || 
			pReinforceDialog->IsActive()	|| 
			pUpgradeDialog->IsActive()		)
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1104 ) );
			return 0;
		}
	}

	// 탈것과 HERO의 거리 확인	
	VECTOR3 vecPlayerPos, vecVehiclePos;
	pPlayer->GetPosition( &vecPlayerPos );
	GetPosition( &vecVehiclePos );
	float fDist = CalcDistanceXZ( &vecPlayerPos, &vecVehiclePos );

	if( fDist > script.mCondition.mSummonRange )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1825 ) );
		return 0;
	}

	// 091013 ShinJS --- 시간 검사 여부 확인
	if( VEHICLEMGR->IsCheckTime() )
	{
		// 탈것 재탑승 가능 시간 확인
		DWORD dwGetOffElapsedTime = gCurTime - pPlayer->GetLastVehicleGetOffTime();
		if( dwGetOffElapsedTime < CVehicle::eGetOnLimitTime )
		{
			float fGetOffElapsedTime = float( CVehicle::eGetOnLimitTime - dwGetOffElapsedTime ) / 1000.0f;
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1822 ), fGetOffElapsedTime );
			return 0;
		}
	}

	// 090428 ShinJS --- 탈것의 위치가 유효한지 확인
	if( MAP->CollisionTilePos( int(vecVehiclePos.x), int(vecVehiclePos.z), MAP->GetMapNum(), this ) )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1840 ) );
		return 0;
	}

	// 090422 ShinJS --- 비어있는 좌석 구하기
	return GetEmptySeatPos( pPlayer->GetID() );
}

// 090423 ShinJS --- 하차 가능 여부 확인
BOOL CVehicle::CanGetOff( CPlayer* pPlayer )
{
	// Player 상태 검사
	switch( pPlayer->GetState() )
	{
	case eObjectState_Rest:
	case eObjectState_Exchange:
	case eObjectState_StreetStall_Owner:
	case eObjectState_StreetStall_Guest:
	case eObjectState_PrivateWarehouse:
	case eObjectState_Munpa:

	case eObjectState_SkillStart:
	case eObjectState_SkillSyn:
	case eObjectState_SkillBinding:
	case eObjectState_SkillUsing:
	case eObjectState_SkillDelay:
	
	case eObjectState_TiedUp_CanMove:
	case eObjectState_TiedUp_CanSkill:
	case eObjectState_TiedUp:
	
	case eObjectState_Die:
	case eObjectState_Fishing:
	case eObjectState_FishingResult:

	case eObjectState_Cooking:
	case eObjectState_ItemUse:
	case eObjectState_Society:
	case eObjectState_Exit:
	case eObjectState_BattleReady:
	case eObjectState_Deal:
		return FALSE;
	}

	// 탑승중이지 않은 경우
	if( !IsGetOn( pPlayer ) )
	{
		return FALSE;
	}

	return TRUE;
}

DWORD CVehicle::GetEmptySeatPos( DWORD dwPlayerID )
{
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );
	
	// Master인 경우 Control 좌석 설정
	if( GetOwnerIndex() == dwPlayerID )
	{
		for( DWORD dwSeatPos=1 ; dwSeatPos <= script.mSeatSize ; ++dwSeatPos )
		{
			if( script.mSeat[ dwSeatPos - 1 ].mIsEnableControl )
				return dwSeatPos;
		}
	}
    
	// 탑승인원 초과
	if( m_htRiderInfo.GetDataNum() >= script.mSeatSize )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1824 ) );		// 탑승인원 초과 메세지
		return 0;
	}

	std::vector<DWORD> vEmptySeatPos;
	DWORD dwEmptyPos = 0;
	BYTE byPassengerCnt = 0;

	// 비어있는 좌석 확인
	for( dwEmptyPos=1 ; dwEmptyPos <= script.mSeatSize ; ++dwEmptyPos )
	{
		// Control 좌석이 아니고 좌석위치에 탑승자가 없는경우
		if( !script.mSeat[ dwEmptyPos - 1 ].mIsEnableControl )
		{
			// 091214 ShinJS --- Control 좌석이 아닌 좌석 Count
			++byPassengerCnt;

			if(	DWORD(m_htRiderInfo.GetData( dwEmptyPos )) == 0 )
				vEmptySeatPos.push_back( dwEmptyPos );
		}
	}

	// 잘못된 데이터 정보
	if( vEmptySeatPos.empty() )
	{
		if( byPassengerCnt > 0 )
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1824 ) );		// 탑승인원 초과 메세지
		else
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2003 ) );		// "소유주만 탑승 하실 수 있습니다."

		return 0;
	}

	// 탑승위치를 랜덤하게 구한다
	int nRandPos = rand() % vEmptySeatPos.size();
	dwEmptyPos = vEmptySeatPos[ nRandPos ];
	vEmptySeatPos.clear();
	
	return dwEmptyPos;
}

BOOL CVehicle::CanControl( CPlayer* pPlayer )
{
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );
	
	// 좌석 정보가 있는 경우 Script의 Control 정보를 반환한다
	const DWORD dwSeatPos = pPlayer->GetVehicleSeatPos();
	if( dwSeatPos )
		return script.mSeat[ dwSeatPos - 1 ].mIsEnableControl;

	return FALSE;
}

void CVehicle::Move_OneTarget( MOVE_ONETARGETPOS_FROMSERVER* pMsg )
{
	// Vehicle 이동처리
	pMsg->dwObjectID = GetID();						// 이동 Object 설정
	MOVEMGR->Move_OneTarget( (void*)pMsg );				// 이동

	m_htRiderInfo.SetPositionHead();

	for(DWORD dwPlayerID = DWORD(m_htRiderInfo.GetData());
		0 < dwPlayerID;
		dwPlayerID = DWORD(m_htRiderInfo.GetData()))
	{
		// Master인 경우 이동처리를 직접한다
		if( dwPlayerID == GetOwnerIndex() )
			continue;

		CObject* pObject = OBJECTMGR->GetObject( dwPlayerID );
		ASSERT( pObject );
		if( !pObject || pObject->GetObjectKind() != eObjectKind_Player )
			continue;

		// 탑승자 이동처리
		pMsg->dwObjectID = dwPlayerID;				// 이동 Object 설정
		MOVEMGR->Move_OneTarget( (void*)pMsg );		// 이동
	}
}

void CVehicle::StartMove( VECTOR3* pTargetPos )
{
	MOVE_INFO* pMoveInfo = GetBaseMoveInfo();
	pMoveInfo->InitTargetPosition();
	pMoveInfo->SetTargetPosition( 0, *pTargetPos );
	pMoveInfo->SetMaxTargetPosIdx( 1 );

	MOVEMGR->StartMoveEx( this, NULL, gCurTime );

	m_htRiderInfo.SetPositionHead();

	for(DWORD dwPlayerID = DWORD(m_htRiderInfo.GetData());
		0 < dwPlayerID;
		dwPlayerID = DWORD(m_htRiderInfo.GetData()))
	{
		// Master인 경우 이동처리를 직접한다
		if( dwPlayerID == GetOwnerIndex() )
			continue;

		CObject* pObject = OBJECTMGR->GetObject( dwPlayerID );
		if( !pObject || pObject->GetObjectKind() != eObjectKind_Player )
			continue;

		// 탑승자 이동처리
		pMoveInfo = pObject->GetBaseMoveInfo();
		pMoveInfo->InitTargetPosition();
		pMoveInfo->SetTargetPosition( 0, *pTargetPos );
		pMoveInfo->SetMaxTargetPosIdx( 1 );

		MOVEMGR->StartMoveEx( pObject, NULL, gCurTime );
	}
}

void CVehicle::MoveStop( VECTOR3* pVecPos )
{
	// Vehicle Stop 처리
	MOVEMGR->MoveStop( this, pVecPos );

	m_htRiderInfo.SetPositionHead();

	for(DWORD dwPlayerID = DWORD(m_htRiderInfo.GetData());
		0 < dwPlayerID;
		dwPlayerID = DWORD(m_htRiderInfo.GetData()))
	{
		// Master인 경우 이동처리를 직접한다
		if( dwPlayerID == GetOwnerIndex() )
			continue;

		CObject* pObject = OBJECTMGR->GetObject( dwPlayerID );
		ASSERT( pObject );
		if( !pObject || pObject->GetObjectKind() != eObjectKind_Player )
			continue;

		// 탑승자 Stop처리
		MOVEMGR->MoveStop( pObject, pVecPos );
	}

}

DWORD CVehicle::GetTollMoney()
{
	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() );
	
	return script.mCondition.mTollMoney;
}

// 091109 ShinJS --- 탈것 모션종류에 대한 Beff파일이름을 반환
const char* CVehicle::GetBeffFileNameFromMotion( DWORD dwMotionType )
{
	VehicleScript& script = const_cast<VehicleScript&>( GAMERESRCMNGR->GetVehicleScript( GetMonsterKind() ) );
	typedef std::multimap<DWORD, std::string>::iterator BeffFileIter;
	std::pair<BeffFileIter, BeffFileIter> EqualRange;

	// 선택된 MotionType (Stop, Play, Idle, Walk, Run)의 BeffFile 이름들을 구한다.
    EqualRange = script.mMotion.mBeffFileList.equal_range( dwMotionType );

	// BeffFile 이름이 없는 경우
	if( EqualRange.first == script.mMotion.mBeffFileList.end() &&
		EqualRange.second == script.mMotion.mBeffFileList.end() )
		return "";

	// BeffFile이 1개인 경우 해당 BeffFile 이름 반환
	int nFileCnt = (int)std::distance( EqualRange.first, EqualRange.second );
	if( nFileCnt == 1 )
		return (EqualRange.first)->second.c_str();

	// 2개 이상인 경우 랜덤하게 BeffFile 이름 반환
	int nRandPos = rand() % nFileCnt;
	std::advance( EqualRange.first, nRandPos );

	return (EqualRange.first)->second.c_str();
}

void CVehicle::ChangeBeff( DWORD dwMotionType )
{
	// MotionType에 대한 EffectNum를 구한다.
	int nEffectNum = FindEffectNum( (char*)GetBeffFileNameFromMotion( dwMotionType ) );

	// 현재 실행된 Effect가 아닌경우
	if( m_nExecutedEffectNum != nEffectNum )
	{
		// 현재 실행중인 Effect 종료
		if( m_dwExecutedEffectHandle )
			EFFECTMGR->ForcedEndEffect( m_dwExecutedEffectHandle );

		// Effect 실행
		TARGETSET set;
		set.pTarget = this;
		m_dwExecutedEffectHandle = EFFECTMGR->StartEffectProcess( nEffectNum, this, &set, 1, GetID() );

		// 실행 Effect 저장
		m_nExecutedEffectNum = nEffectNum;
	}
}
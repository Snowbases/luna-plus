#include "StdAfx.h"
#include "Vehicle.h"
#include "Player.h"
#include "UserTable.h"
#include "CharMove.h"
#include "ObjectStateManager.h"
#include "..\[CC]Skill\Server\Manager\SkillManager.h"
#include "..\[CC]Skill\Server\Info\BuffSkillInfo.h"
#include "PackedData.h"
#include "MapNetworkMsgParser.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "MapDBMsgParser.h"
#include "PartyManager.h"
#include "Party.h"

CVehicle::CVehicle()
{}

CVehicle::~CVehicle()
{}

BOOL CVehicle::Init( EObjectKind kind, DWORD agentNum, BASEOBJECT_INFO* baseObjectInfo )
{
	if(FALSE == CMonster::Init( kind, agentNum, baseObjectInfo))
	{
		return FALSE;
	}

	ZeroMemory( mSeat, sizeof( mSeat ) );
	ZeroMemory( &mUsedItemToSummon, sizeof( mUsedItemToSummon ) );
	mMountedSeatSize = 0;
	mRunSpeed = mWalkSpeed = 0;
	mMoveSyncedSeatIndex = 0;
	return TRUE;
}

// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
DWORD CVehicle::SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*& sendMessage)
{
	static SEND_VEHICLE_TOTALINFO message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_VEHICLE_ADD;
	message.dwObjectID = dwReceiverID;
	GetSendMoveInfo( &message.MoveInfo, &message.AddableInfo );
	message.BaseObjectInfo = m_BaseObjectInfo;
	message.TotalInfo = m_MonsterInfo;
	message.bLogin = BYTE(isLogin);
	message.usedItemPosition = mUsedItemToSummon.Position;

	// 090316 LUJ, 좌석 정보를 갱신한다
	{
		const DWORD seatMaxSize = sizeof( message.mSeat ) / sizeof( *( message.mSeat ) );

		for( DWORD seatIndex = 1; seatIndex <= seatMaxSize; ++seatIndex )
		{
			message.mSeat[ seatIndex - 1 ] = GetPlayerInSeat( seatIndex );
		}
	}
	
	if(CObject* const pOwner = g_pUserTable->FindUser( GetOwnerIndex() ))
	{
		SafeStrCpy(
			message.MasterName,
			pOwner->GetObjectName(),
			_countof(message.MasterName));
	}

	sendMessage = &message;
	return message.GetMsgLength();
}

BOOL CVehicle::DoAttack(DWORD)
{
	return TRUE;
}

MSG_VEHICLE_ERROR::Error CVehicle::Mount( DWORD playerIndex )
{
	return Mount( playerIndex, GetOwnerIndex() == playerIndex ? GetEmptySeatForDriver() : GetEmptySeatForPassenger() );
}

MSG_VEHICLE_ERROR::Error CVehicle::Mount( const DWORD playerIndex, const DWORD seatIndex )
{
	const BOOL thereIsNoSeat( 0 == seatIndex );

	if( thereIsNoSeat )
	{
		return MSG_VEHICLE_ERROR::ErrorMountByFullSeat;
	}
	else if( GetScript().mSeatSize < seatIndex )
	{
		return MSG_VEHICLE_ERROR::ErrorMountByOverSeatIndex;
	}
	else if( GetPlayerInSeat( seatIndex ) )
	{
		return MSG_VEHICLE_ERROR::ErrorMountBySitAlready;
	}
	else if( GetOwnerIndex() != playerIndex )
	{
		const VehicleScript::Seat& seat = GetScript().mSeat[ seatIndex - 1 ];

		if( TRUE == seat.mIsEnableControl )
		{
			return MSG_VEHICLE_ERROR::ErrorMountByNoOwner;
		}
	}

	CPlayer* const playerObject = ( CPlayer* )g_pUserTable->FindUser( playerIndex );

	if( playerObject &&
		playerObject->GetObjectKind() == eObjectKind_Player )
	{
		playerObject->SetMountedVehicle( GetID() );
	}

	SetSeatMountedPlayer( playerIndex, seatIndex );
	++mMountedSeatSize;

	return MSG_VEHICLE_ERROR::ErrorNone;
}

MSG_VEHICLE_ERROR::Error CVehicle::Dismount( const DWORD playerIndex, const BOOL needBuff )
{
	const DWORD seatIndex = GetMountedSeat( playerIndex );
	
	if( 0 == seatIndex )
	{
		return MSG_VEHICLE_ERROR::ErrorDismountByNotSit;
	}

	CPlayer* const playerObject = ( CPlayer* )g_pUserTable->FindUser( playerIndex );
	if( playerObject == NULL || playerObject->GetObjectKind() != eObjectKind_Player )
		return MSG_VEHICLE_ERROR::ErrorDismountByNotSit;

	playerObject->SetMountedVehicle( 0 );
	SetSeatMountedPlayer( 0, seatIndex );
	--mMountedSeatSize;

	if( needBuff )
	{
		AddBuff( playerIndex );
	}

	return MSG_VEHICLE_ERROR::ErrorNone;
}

// 090316 LUJ, 해당 플레이어가 탑승한 좌석 번호를 얻는다
DWORD CVehicle::GetMountedSeat( const DWORD playerIndex ) const
{
	for( DWORD seatIndex = 1; seatIndex <= GetScript().mSeatSize; ++seatIndex )
	{
		const DWORD mountedPlayerIndex = GetPlayerInSeat( seatIndex );

		if( mountedPlayerIndex == playerIndex )
		{
			return seatIndex;
		}
	}

	return 0;
}

// 090316 LUJ, 빈 좌석 번호를 얻는다
DWORD CVehicle::GetEmptySeatForPassenger() const
{
	for( DWORD i = 0; i < GetScript().mSeatSize; ++i )
	{
		const DWORD seatIndex = i + 1;

		const BOOL anyoneDoNotMount = ( 0 == GetPlayerInSeat( seatIndex ) );
		const VehicleScript::Seat& seat = GetScript().mSeat[ i ];

		if( FALSE == seat.mIsEnableControl &&
			anyoneDoNotMount )
		{
			return seatIndex;
		}
	}

	return 0;
}

// 090316 LUJ, 빈 좌석 번호를 얻는다
DWORD CVehicle::GetEmptySeatForDriver()
{
	for( DWORD i = 0; i < GetScript().mSeatSize; ++i )
	{
		const DWORD seatIndex = i + 1;

		const BOOL anyoneDoNotMount = ( 0 == GetPlayerInSeat( seatIndex ) );
		const VehicleScript::Seat& seat = GetScript().mSeat[ i ];
		const BOOL bIsControlCheck = ( GetMoveSpeed() <= 0.f || seat.mIsEnableControl );

		// 100318 ShinJS --- 고정형인 경우 조정여부를 판단하지 않는다.
		if( bIsControlCheck &&
			anyoneDoNotMount )
		{
			return seatIndex;
		}
	}

	return 0;
}

// 090316 LUJ, 해당 좌석에 있는 플레이어 번호를 구한다
DWORD CVehicle::GetPlayerInSeat( DWORD seatIndex ) const
{
	return IsInvalidSeatIndex( seatIndex ) ? 0 : mSeat[ seatIndex - 1 ];
}

BOOL CVehicle::IsInvalidSeatIndex( const DWORD seatIndex ) const
{
	return 0 == seatIndex || GetScript().mSeatSize < seatIndex;
}

// 090316 LUJ, 좌석에 플레이어를 설정한다
void CVehicle::SetSeatMountedPlayer( const DWORD playerIndex, const DWORD seatIndex )
{
	// 090316 LUJ, 범위가 잘못된 경우 원래 값을 대입한다. 즉 아무 동작도 취하지 않는다
	mSeat[ seatIndex - 1 ] = ( IsInvalidSeatIndex( seatIndex ) ? mSeat[ seatIndex - 1 ] : playerIndex );
}

void CVehicle::StateProcess()
{
	CMonster::StateProcess();

	if(IsInfiniteSummon())
	{
		return;
	}

	CObject* const ownerObject = g_pUserTable->FindUser(GetOwnerIndex());

	if( 0 == ownerObject ||
		ownerObject->GetState() == eObjectState_Die )
	{
		DoDie( 0 );
		return;
	}

	switch( GetState() )
	{
	case eObjectState_Move:
		{
			MOVE_INFO* ownerMoveInfo = ( MOVE_INFO* )( ownerObject->GetMoveInfo() );
			CObject* const passengerObject = g_pUserTable->FindUser( GetPlayerInSeat( ++mMoveSyncedSeatIndex ) );

			if( GetScript().mSeatSize < mMoveSyncedSeatIndex )
			{
				mMoveSyncedSeatIndex = 0;
			}
			else if( 0 != passengerObject )
			{
				MOVE_INFO* const vehicleMoveInfo = ( MOVE_INFO* )( GetMoveInfo() );

				// 090424 ShinJS --- 탑승자의 이동정보 Sync 수정
				// 소유주인 경우 탈것의 이동정보를 소유주로 맞춰준다
				if( GetOwnerIndex() == passengerObject->GetID() )
				{
					*vehicleMoveInfo = *ownerMoveInfo;
				}
				// 소유주가 아닌 경우 탑승자의 이동정보를 탈것으로 맞춰준다
				else
				{
					MOVE_INFO* const passengerMoveInfo = ( MOVE_INFO* )( passengerObject->GetMoveInfo() );
					*passengerMoveInfo = *vehicleMoveInfo;

					// 파티가 있으면 파티원에게 이동정보를 알려주자.
					CParty* pParty = PARTYMGR->GetParty(((CPlayer*)passengerObject)->GetPartyIdx());
					if(pParty)
					{
						pParty->SendMemberPos(((CPlayer*)passengerObject)->GetID(), ((CPlayer*)passengerObject)->GetMoveInfo());
					}
				}
			}	

			break;
		}
	}
}

// 090316 LUJ, 탑승한 모든 플레이어를 하차시킨다
void CVehicle::DismountAll()
{
	for( DWORD seatIndex = 1; seatIndex <= GetScript().mSeatSize; ++seatIndex )
	{
		const DWORD objectIndex = GetPlayerInSeat( seatIndex );
		Dismount( objectIndex, TRUE );
	}
}

// 100121 ShinJS --- 특정 플레이어 이외의 모든 플레이어를 강제 하차시킨다
void CVehicle::DismountAllExceptOne( DWORD dwExceptIndex )
{
	for( DWORD seatIndex = 1; seatIndex <= GetScript().mSeatSize; ++seatIndex )
	{
		const DWORD objectIndex = GetPlayerInSeat( seatIndex );
		if( dwExceptIndex == objectIndex )
			continue;

		CPlayer* const player = ( CPlayer* )g_pUserTable->FindUser( objectIndex );
		if( !player || player->GetObjectKind() != eObjectKind_Player )
			continue;

		const MSG_VEHICLE_ERROR::Error error = Dismount( objectIndex, TRUE );
		if( MSG_VEHICLE_ERROR::ErrorNone != error )
		{
			MSG_VEHICLE_ERROR message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category = MP_VEHICLE;
			message.Protocol = MP_VEHICLE_ERROR_ACK;
			message.mError = error;
			player->SendMsg( &message, sizeof( message ) );
			continue;
		}

		RemoveVehiclePassengerToDb( objectIndex );

		MSG_DWORD3 message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_VEHICLE;
		message.Protocol = MP_VEHICLE_DISMOUNT_ACK;
		message.dwData1 = objectIndex;
		message.dwData2 = GetID();
		message.dwData3 = seatIndex;
		PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
	}
}

const VehicleScript& CVehicle::GetScript() const
{
	return GAMERESRCMNGR->GetVehicleScript(
		GetMonsterKind());
}

void CVehicle::DoDie( CObject* attacker )
{
	if( eObjectState_Die == GetState() )
	{
		return;
	}

	CMonster::DoDie( attacker );
	
	MSG_DWORD2 message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_VEHICLE_REMOVE;
	message.dwData1 = GetOwnerIndex();
	message.dwData2 = GetID();
	PACKEDDATA_OBJ->QuickSend( this, &message, sizeof( message ) );

	DismountAll();
}

MSG_VEHICLE_ERROR::Error CVehicle::Move( const DWORD askedPlayerIndex, VECTOR3& startPosition, VECTOR3& targetPosition )
{
	if( GetOwnerIndex() != askedPlayerIndex )
	{
		return MSG_VEHICLE_ERROR::ErrorMoveByNoOwner;
	}

	// 090316 LUJ, 탈것을 이동시킨다
	CCharMove::StartMoveEx(
		this,
		gCurTime,
		&startPosition,
		&targetPosition );
	OBJECTSTATEMGR_OBJ->StartObjectState(
		this,
		eObjectState_Move,
		0 );

	if( GetMountedSeat( GetOwnerIndex() ) )
	{
		CObject* const ownerObject = g_pUserTable->FindUser( GetOwnerIndex() );

		if( ownerObject )
		{
			const MOVE_INFO* const ownerMoveInfo = ( MOVE_INFO* )ownerObject->GetMoveInfo();
			MOVE_INFO* const vehicleMoveInfo = ( MOVE_INFO* )GetMoveInfo();

			*vehicleMoveInfo = *ownerMoveInfo;
		}
	}

	// 090316 LUJ, 승객도 이동시킨다
	for( DWORD seatIndex = 1; seatIndex <= GetScript().mSeatSize; ++seatIndex )
	{
		const DWORD passengerObjectIndex = GetPlayerInSeat( seatIndex );
		CObject* const passengerObject = g_pUserTable->FindUser( passengerObjectIndex );

		if( 0 == passengerObject )
		{
			continue;
		}

		CCharMove::StartMoveEx(
			passengerObject,
			gCurTime,
			&startPosition,
			&targetPosition );
		OBJECTSTATEMGR_OBJ->StartObjectState(
			passengerObject, 
			eObjectState_Move,
			0 );
	}

	return MSG_VEHICLE_ERROR::ErrorNone;
}

MSG_VEHICLE_ERROR::Error CVehicle::EndMove( const DWORD askedPlayerIndex, VECTOR3& position )
{
	if( GetOwnerIndex() != askedPlayerIndex )
	{
		return MSG_VEHICLE_ERROR::ErrorMoveByNoOwner;
	}

	CCharMove::EndMove( this, gCurTime, &position );
	OBJECTSTATEMGR_OBJ->EndObjectState( this, eObjectState_Move );

	// 090316 LUJ, 승객도 이동시킨다
	for( DWORD seatIndex = 1; seatIndex <= GetScript().mSeatSize; ++seatIndex )
	{
		const DWORD mountedObjectIndex = GetPlayerInSeat( seatIndex );
		CObject* const object = g_pUserTable->FindUser( mountedObjectIndex );

		if( object )
		{
			CCharMove::EndMove( object, gCurTime, &position );
			OBJECTSTATEMGR_OBJ->EndObjectState( object, eObjectState_Move );
		}
	}

	return MSG_VEHICLE_ERROR::ErrorNone;
}

DWORD CVehicle::Damage( CObject*, RESULTINFO* )
{
	return GetLife();
}

void CVehicle::AddBuff( const DWORD playerIndex )
{
	// 090316 LUJ, 탑승자에게 버프를 제공한다
	for( DWORD i = 0; i < VehicleScript::Condition::BuffMaxSize; ++i )
	{
		const DWORD buffSkillIndex = GetScript().mCondition.mBuffSkillIndex[ i ];
		const cBuffSkillInfo* const buffSkillInfo = SKILLMGR->GetBuffInfo( buffSkillIndex );

		if(0 == buffSkillInfo ||
			SKILLKIND_BUFF != buffSkillInfo->GetKind())
		{
			break;
		}

		SKILLMGR->BuffSkillStart(
			playerIndex,
			buffSkillIndex,
			0,
			buffSkillInfo->GetInfo().Count );
	}
}

BOOL CVehicle::Teleport( const DWORD askedPlayerIndex, const MAPCHANGE_INFO* const mapChangeInfo )
{
	if( mapChangeInfo == NULL ||
		0 == GetMoveSpeed() ||
		GetOwnerIndex() != askedPlayerIndex )
	{
		return FALSE;
	}

	char textQuery[ MAX_PATH ] = { 0 };

	for( DWORD seatIndex = 1; seatIndex <= GetScript().mSeatSize; ++seatIndex )
	{
		const DWORD playerIndex = GetPlayerInSeat( seatIndex );

		if( 0 == playerIndex )
		{
			continue;
		}

		char textVariable[ MAX_PATH ] = { 0 };
		sprintf( textVariable, ", %d, %d", seatIndex, playerIndex );

		const size_t textSize = strlen( textQuery ) + strlen( textVariable );
		const size_t textMaxSize = sizeof( textQuery ) / sizeof( *textQuery );

		if( textMaxSize < textSize )
		{
			break;
		}
		else if(FALSE == Teleport(
			playerIndex,
			*mapChangeInfo))
		{
			continue;
		}

		strcat(
			textQuery,
			textVariable);
	}

	// 090316 LUJ, 소유주가 탑승하고 있지 않더라도 맵 이동시켜야 한다
	if( 0 == GetMountedSeat( GetOwnerIndex() ) )
	{
		Teleport(
			askedPlayerIndex,
			*mapChangeInfo);
	}

	// 091231 LUJ, 같은 맵으로 이동 시 종종 오브젝트가 사라지지 않는다. 소환 해제하도록 처리했다
	DoDie(0);

	g_DB.FreeMiddleQuery(
		0,
		0,
		"EXEC dbo.MP_VEHICLE_PASSENGER_UPDATE %d, %d%s",
		GetOwnerIndex(),
		mapChangeInfo->MoveMapNum,
		textQuery );
	return TRUE;
}

void CVehicle::DoStand()
{}

void CVehicle::DoRest( BOOL )
{}

BOOL CVehicle::DoWalkAround()
{
	return TRUE;
}

BOOL CVehicle::DoPursuit( CObject* )
{
	return TRUE;
}

BOOL CVehicle::Recall( const VECTOR3& position )
{	
	CObject* const ownerObject = g_pUserTable->FindUser( GetOwnerIndex() );

	if( 0 == ownerObject )
	{
		return FALSE;
	}
	else if( 0 == GetMoveSpeed() )
	{
		return FALSE;
	}

	VECTOR3 recallPosition = position;
	recallPosition.x = max( recallPosition.x, 0 );
	recallPosition.x = min( recallPosition.x, 51199 );
	recallPosition.z = max( recallPosition.z, 0 );
	recallPosition.z = min( recallPosition.z, 51199 );
	CCharMove::SetPosition( this, &recallPosition );

	// 090316 LUJ, 승객 위치도 보정한다
	for( DWORD seatIndex = 1; seatIndex <= GetScript().mSeatSize; ++seatIndex )
	{
		CObject* const passengerObject = g_pUserTable->FindUser( GetPlayerInSeat( seatIndex ) );
		CCharMove::SetPosition( passengerObject, &recallPosition );
	}

	MOVE_POS message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_MOVE;
	message.Protocol = MP_MOVE_WARP;
	message.dwMoverID = GetID();
	message.cpos.Compress( &recallPosition );
	PACKEDDATA_OBJ->QuickSend( ownerObject, &message, sizeof( message ) );
	return TRUE;
}

BOOL CVehicle::Teleport(DWORD playerIndex, const MAPCHANGE_INFO& mapChangeInfo)
{
	CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(playerIndex);

	if(0 == player)
	{
		return FALSE;
	}
	else if(eObjectKind_Player != player->GetObjectKind())
	{
		return FALSE;
	}

	player->SetMapMoveInfo(
		mapChangeInfo.MoveMapNum,
		DWORD(mapChangeInfo.MovePoint.x),
		DWORD(mapChangeInfo.MovePoint.z));	
	player->UpdateLogoutToDB(FALSE);

	const DWORD maintainCurrentChannel = WORD( -1 );
	MSG_WORD2 message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_MAP_OUT_WITHMAPNUM;
	message.dwObjectID = playerIndex;
	message.wData1 = mapChangeInfo.MoveMapNum;
	message.wData2 = maintainCurrentChannel;
	player->SendMsg(
		&message,
		sizeof(message));

	g_pServerSystem->RemovePlayer(
		playerIndex);
	return TRUE;
}
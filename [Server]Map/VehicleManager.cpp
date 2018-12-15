#include "StdAfx.h"
#include "VehicleManager.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "ItemManager.h"
#include "Player.h"
#include "UserTable.h"
#include "RegenManager.h"
#include "Vehicle.h"
#include "PackedData.h"
#include "MapDBMsgParser.h"
#include "CharMove.h"
#include "HousingMgr.h"
#include "..\hseos\Date\SHDateManager.h"
#include "ChannelSystem.h"
#include <fstream>

CVehicleManager::CVehicleManager()
{
	_stprintf(
		mFileName,
		_T("system\\vehicle.%03d.txt"),
		g_pServerSystem->GetMapNum());

	Load();
}

CVehicleManager::~CVehicleManager()
{}

CVehicleManager& CVehicleManager::GetInstance()
{
	static CVehicleManager manager;
	return manager;
}

void CVehicleManager::NetworkMsgParse( const DWORD connectionIndex, const MSGBASE* const receivedMessage, const DWORD receivedMessageSize )
{
	switch( receivedMessage->Protocol )
	{
		// 090316 LUJ, 탈것 소환
	case MP_VEHICLE_SUMMON_SYN:
		{
			const MSG_VEHICLE_SUMMON* const message = ( MSG_VEHICLE_SUMMON* )receivedMessage;
			const DWORD playerIndex = message->dwObjectID;
			const ICONBASE& iconBase = message->IconBase;
			VECTOR3 vehiclePosition = { 0 };
			message->SummonPos.Decompress( &vehiclePosition );
			Summon( playerIndex, iconBase, vehiclePosition, TRUE );
			break;
		}
		// 090316 LUJ, 소환 해제
	case MP_VEHICLE_UNSUMMON_SYN:
		{
			const MSGBASE* const message = ( MSGBASE* )receivedMessage;
			const DWORD playerIndex = message->dwObjectID;
			Unsummon( playerIndex, TRUE );
			break;
		}
		// 090316 LUJ, 승차 시도
	case MP_VEHICLE_MOUNT_ASK_SYN:
		{
			const MSG_DWORD2* const message = ( MSG_DWORD2* )receivedMessage;
			const DWORD askedPlayerIndex = message->dwObjectID;
			const DWORD vehicleIndex = message->dwData1;
			const DWORD key = message->dwData2;
			MountAsk(
				askedPlayerIndex,
				vehicleIndex,
				key);
			break;
		}
		// 090316 LUJ, 승차 허락
	case MP_VEHICLE_MOUNT_ALLOW_SYN:
		{
			const MSG_DWORD4* const message = (MSG_DWORD4*)receivedMessage;
			const DWORD playerIndex = message->dwData1;
			const DWORD vehicleIndex = message->dwData2;
			const DWORD seatIndex = message->dwData3;
			const DWORD key = message->dwData4;
			const MSG_VEHICLE_ERROR::Error error = MountToll(
				playerIndex,
				vehicleIndex,
				key);

			if(MSG_VEHICLE_ERROR::ErrorNone != error)
			{
				MSG_DWORD sendMessage;
				ZeroMemory(
					&sendMessage,
					sizeof(sendMessage));
				sendMessage.Category = MP_VEHICLE;
				sendMessage.Protocol = MP_VEHICLE_MOUNT_REJECT_SYN;
				sendMessage.dwData = playerIndex;
				NetworkMsgParse(
					connectionIndex,
					&sendMessage,
					sizeof(sendMessage));

				SendErrorMessage(
					message->dwObjectID,
					error);
				break;
			}

			MountAllow(
				playerIndex,
				vehicleIndex,
				seatIndex,
				TRUE);
			break;
		}
		// 090316 LUJ, 승차를 허락하지 않음
	case MP_VEHICLE_MOUNT_REJECT_SYN:
		{
			const MSG_DWORD* const message = ( MSG_DWORD* )receivedMessage;
			const DWORD askedPlayerIndex = message->dwData;
			CObject* const object = g_pUserTable->FindUser(
				askedPlayerIndex);

			if(0 == object)
			{
				break;
			}

			MSGBASE sendMessage;
			ZeroMemory(
				&sendMessage,
				sizeof(sendMessage));
			sendMessage.Category = MP_VEHICLE;
			sendMessage.Protocol = MP_VEHICLE_MOUNT_REJECT_ACK;
			object->SendMsg(
				&sendMessage,
				sizeof(sendMessage));
			break;
		}
		// 090316 LUJ, 하차 시도
	case MP_VEHICLE_DISMOUNT_SYN:
		{
			const MSGBASE* const message = ( MSGBASE* )receivedMessage;
			const DWORD playerIndex = message->dwObjectID;
			Dismount( playerIndex, TRUE );
			break;
		}
	}
}

void CVehicleManager::SetOption(DWORD playerIndex, BOOL isNeedAllow, MONEYTYPE chargedMoney)
{
	Option& option = mOptionContainer[playerIndex];
	option.mKey = gCurTime;
	option.mIsNeedAllow = isNeedAllow;
	option.mChargedMoney = min(
		chargedMoney,
		MAX_INVENTORY_MONEY);
}

const CVehicleManager::Option& CVehicleManager::GetOption(DWORD playerIndex) const
{
	OptionContainer::const_iterator iterator = mOptionContainer.find(
		playerIndex);

	const static Option emptyOption = {0};
	return mOptionContainer.end() == iterator ? emptyOption : iterator->second;
}

void CVehicleManager::SendErrorMessage(DWORD objectIndex, MSG_VEHICLE_ERROR::Error error, DWORD dwData) const
{
	CObject* const object = g_pUserTable->FindUser(
		objectIndex);

	if(0 == object)
	{
		return;
	}

	SendErrorMessage(
		*object,
		error);
}

void CVehicleManager::SendErrorMessage(CObject& object, MSG_VEHICLE_ERROR::Error error, DWORD dwData) const
{
	MSG_VEHICLE_ERROR message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_VEHICLE;
	message.Protocol = MP_VEHICLE_ERROR_ACK;
	message.mError = error;
	message.dwData = dwData;
	object.SendMsg( &message, sizeof( message ) );
}

void CVehicleManager::Summon( const DWORD playerIndex, const ICONBASE& iconBase, VECTOR3& vehiclePosition, BOOL summonByPlayer ) const
{
	CPlayer* const player = GetPlayer( playerIndex );

	if( 0 == player )
	{
		return;
	}
	else if( player->GetSummonedVehicle() )
	{
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorSummonAlready );
		return;
	}
	else if( player->GetMountedVehicle() )
	{
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorSummonByMount );
		return;
	}
	else if( IsInvalidState( EObjectState( player->GetState() ) ) )
	{
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorSummonByState );
		return;
	}
	else if( HOUSINGMAPNUM == g_pServerSystem->GetMapNum())
	{
		// 하우스맵에선 실패
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorInHouse );
		return;
	}

	const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo( iconBase.wIconIdx );

	if( 0 == itemInfo )
	{
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorSummonByItem );
		return;
	}
	else if( ITEM_KIND_SUMMON_VEHICLE != itemInfo->SupplyType )
	{
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorSummonByItem );
		return;
	}

	const DWORD monsterKind = itemInfo->SupplyValue;
	const VehicleScript& vehicleScript = GAMERESRCMNGR->GetVehicleScript( monsterKind );

	VECTOR3 playerPosition = { 0 };
	player->GetPosition( &playerPosition );
	
	if( vehicleScript.mCondition.mSummonRange < CalcDistanceXZ( &playerPosition, &vehiclePosition ) )
	{
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorSummonByDistance, iconBase.Position );
		return;
	}
	else if( summonByPlayer )
	{
		// 090316 LUJ, 아이템 사용은 최종적으로 이뤄지도록 하자. 소모가 될 경우 돌이킬 수 없기 때문
		if( eItemUseSuccess != ITEMMGR->UseItem( player, iconBase.Position, iconBase.wIconIdx ) )
		{
			SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorSummonByUsingItem, iconBase.Position );
			return;
		}
	}

	CVehicle* const vehicle = REGENMGR->RegenVehicle(
		player,
		monsterKind,
		vehiclePosition);

	if( 0 == vehicle )
	{
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorSummonByCreateVehicle, iconBase.Position );
		return;
	}

	LogItemMoney(
		player->GetID(),
		player->GetObjectName(),
		0,
		"*summon",
		eLog_ItemVehicleSummon,
		player->GetMoney(),
		0,
		0,
		iconBase.wIconIdx,
		iconBase.dwDBIdx,
		iconBase.Position,
		0,
		0,
		0);
	UpdateVehicleToDb( 
		playerIndex,
		iconBase, 
		0 );
	vehicle->SetUseItem( iconBase );
	player->SetSummonedVehicle( vehicle->GetID() );

	MSG_DWORD4 message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_VEHICLE;
	message.Protocol = MP_VEHICLE_SUMMON_ACK;
	message.dwObjectID = playerIndex;
	message.dwData1 = vehicle->GetID();
	message.dwData2 = playerIndex;
	message.dwData3 = iconBase.Position;
	message.dwData4 = summonByPlayer;
	PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
}

void CVehicleManager::Unsummon( const DWORD playerIndex, const BOOL unsummonByPlayer ) const
{
	CPlayer* const player = GetPlayer( playerIndex );

	if( 0 == player )
	{
		return;
	}
	else if( IsInvalidState( EObjectState( player->GetState() ) ) )
	{
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorUnsummonByState );
		return;
	}

	CVehicle* const vehicle = GetVehicle( player->GetSummonedVehicle() );

	if( 0 == vehicle )
	{
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorUnsummonByNoMine );
		return;
	}
	else if( unsummonByPlayer )
	{
		if( IsInvalidDistance( *player, *vehicle ) )
		{
			SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorUnsummonByDistance );
			return;
		}
	}

	LogItemMoney(
		player->GetID(),
		player->GetObjectName(),
		0,
		"*unsummon",
		eLog_ItemVehicleUnsummon,
		player->GetMoney(),
		0,
		0,
		vehicle->GetUseItem().wIconIdx,
		vehicle->GetUseItem().dwDBIdx,
		vehicle->GetUseItem().Position,
		0,
		0,
		0);
	UpdateVehicleToDb(
		playerIndex,
		vehicle->GetUseItem(),
		0 );
	vehicle->Die( 0 );
	player->SetSummonedVehicle( 0 );

	MSG_DWORD message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_VEHICLE;
	message.Protocol = MP_VEHICLE_UNSUMMON_ACK;
	message.dwObjectID = playerIndex;
	message.dwData = vehicle->GetID();
	PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
}

void CVehicleManager::MountAsk(DWORD askedPlayerIndex, DWORD vehicleIndex, DWORD key)
{
	CPlayer* const askedPlayer = GetPlayer( askedPlayerIndex );
	CVehicle* const vehicle = GetVehicle( vehicleIndex );

	if( 0 == askedPlayer || 0 == vehicle )
	{
		return;
	}
	else if(vehicle->IsInfiniteSummon())
	{
		MountAllow(
			askedPlayerIndex,
			vehicleIndex,
			vehicle->GetEmptySeatForPassenger(),
			TRUE);
		return;
	}

	CPlayer* const ownedPlayer = GetPlayer( vehicle->GetOwnerIndex() );

	if( 0 == ownedPlayer )
	{
		return;
	}

	const MSG_VEHICLE_ERROR::Error checkError = MountCheck( *ownedPlayer, *askedPlayer, *vehicle );

	if( MSG_VEHICLE_ERROR::ErrorNone != checkError )
	{
		SendErrorMessage( *askedPlayer, checkError );
		return;
	}
	else if(vehicle->GetOwnerIndex() == askedPlayerIndex)
	{
		MountAllow(
			askedPlayerIndex,
			vehicleIndex,
			vehicle->GetEmptySeatForDriver(),
			TRUE);
		return;
	}

	const Option& option = GetOption(
		ownedPlayer->GetID());

	if(0 == key &&
		0 < option.mChargedMoney)
	{
		MSG_DWORD3 message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_VEHICLE;
		message.Protocol = MP_VEHICLE_GET_OPTION_ACK;
		message.dwData1 = vehicleIndex;
		message.dwData2 = option.mChargedMoney;
		message.dwData3 = option.mKey;
		askedPlayer->SendMsg(
			&message,
			sizeof(message));
		return;
	}
	else if(option.mIsNeedAllow)
	{
		MSG_DWORD4 message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_VEHICLE;
		message.Protocol = MP_VEHICLE_MOUNT_REQUEST_SYN;
		message.dwData1 = askedPlayerIndex;
		message.dwData2 = vehicleIndex;
		message.dwData3 = option.mChargedMoney;
		message.dwData4 = key;
		ownedPlayer->SendMsg(
			&message,
			sizeof(message));
		return;
	}

	MSG_DWORD4 message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_VEHICLE;
	message.Protocol = MP_VEHICLE_MOUNT_ALLOW_SYN;
	message.dwData1 = askedPlayerIndex;
	message.dwData2 = vehicleIndex;
	message.dwData3 = vehicle->GetEmptySeatForPassenger();
	message.dwData4 = key;
	NetworkMsgParse(
		0,
		&message,
		sizeof(message));
}

MSG_VEHICLE_ERROR::Error CVehicleManager::MountToll(DWORD askedPlayerIndex, DWORD vehicleIndex, DWORD key) const
{
	CVehicle* const vehicle = GetVehicle(
		vehicleIndex);

	if(0 == vehicle)
	{
		return MSG_VEHICLE_ERROR::ErrorMountByNoVehicle;
	}
	else if(vehicle->GetOwnerIndex() == askedPlayerIndex)
	{
		return MSG_VEHICLE_ERROR::ErrorNone;
	}
	else if(0 == vehicle->GetEmptySeatForPassenger())
	{
		return MSG_VEHICLE_ERROR::ErrorTollByFullSeat;
	}

	const Option& option = GetOption(
		vehicle->GetOwnerIndex());

	if(0 == option.mChargedMoney)
	{
		return MSG_VEHICLE_ERROR::ErrorNone;
	}
	else if(option.mKey != key)
	{
		return MSG_VEHICLE_ERROR::ErrorTollByInvalidKey;
	}

	CPlayer* const ownedPlayer = GetPlayer(
		vehicle->GetOwnerIndex());

	if(0 == ownedPlayer)
	{
		return MSG_VEHICLE_ERROR::ErrorTollByNoOwner;
	}
	else if(FALSE == ownedPlayer->IsEnoughAdditionMoney(option.mChargedMoney))
	{
		return MSG_VEHICLE_ERROR::ErrorTollByOwnerHasFullMoney;
	}

	CPlayer* const askedPlayer = GetPlayer(
		askedPlayerIndex);

	if(0 == askedPlayer)
	{
		return MSG_VEHICLE_ERROR::ErrorTollByNoPassenger;
	}
	else if(askedPlayer->GetMoney() < option.mChargedMoney)
	{
		return MSG_VEHICLE_ERROR::ErrorTollByPassengerHasInsufficientMoney;
	}

	ownedPlayer->SetMoney(
		option.mChargedMoney,
		MONEY_ADDITION,
		MF_NOMAL,
		eItemTable_Inventory,
		eMoneyLog_TollForVehicle );
	askedPlayer->SetMoney(
		option.mChargedMoney,
		MONEY_SUBTRACTION,
		MF_NOMAL,
		eItemTable_Inventory,
		eMoneyLog_TollForVehicle );

	const ICONBASE& useItem = vehicle->GetUseItem();
	LogItemMoney(
		askedPlayerIndex,
		askedPlayer->GetObjectName(),
		ownedPlayer->GetID(),
		ownedPlayer->GetObjectName(),
		eLog_ItemVehicleToll,
		askedPlayer->GetMoney(),
		ownedPlayer->GetMoney(),
		option.mChargedMoney,
		useItem.wIconIdx,
		useItem.dwDBIdx,
		useItem.Position,
		0,
		0,
		0);
	return MSG_VEHICLE_ERROR::ErrorNone;
}

void CVehicleManager::MountAllow( const DWORD playerIndex, const DWORD vehicleIndex, const DWORD seatIndex, const BOOL needBuff ) const
{
	CVehicle* const vehicle = GetVehicle( vehicleIndex );

	if( 0 == vehicle )
	{
		return;
	}

	CPlayer* const askedPlayer = GetPlayer( playerIndex );

	if(0 == askedPlayer)
	{
		return;
	}
	else if(FALSE == vehicle->IsInfiniteSummon())
	{
		CPlayer* const ownedPlayer = GetPlayer(
			vehicle->GetOwnerIndex());

		if( 0 == ownedPlayer)
		{
			return;
		}

		const MSG_VEHICLE_ERROR::Error checkError = MountCheck( *ownedPlayer, *askedPlayer, *vehicle );

		if( MSG_VEHICLE_ERROR::ErrorNone != checkError )
		{
			SendErrorMessage( *askedPlayer, checkError );
			return;
		}
		else if( 0 == ownedPlayer->GetSummonedVehicle() )
		{
			SendErrorMessage( *askedPlayer, MSG_VEHICLE_ERROR::ErrorMountByNoVehicle );
			return;
		}
	}
	
	// 090316 LUJ, 소환한 상태에서 다른 플레이어의 탈것에 탑승할 수 없다
	if( 0 < askedPlayer->GetSummonedVehicle() &&
		askedPlayer->GetSummonedVehicle() != vehicleIndex )
	{
		SendErrorMessage( *askedPlayer, MSG_VEHICLE_ERROR::ErrorMountBySummonedAlready );
		return;
	}
	else if( IsInvalidDistance( *askedPlayer, *vehicle ) )
	{
		SendErrorMessage( *askedPlayer, MSG_VEHICLE_ERROR::ErrorMountByDistance );
		return;
	}
	// 090316 LUJ, 레벨이 낮으면 탑승할 수 없다.
	else if( askedPlayer->GetLevel() < vehicle->GetScript().mCondition.mPlayerMinimumLevel )
	{
		SendErrorMessage( *askedPlayer, MSG_VEHICLE_ERROR::ErrorMountByPlayerLevel );
		return;
	}
	else if( askedPlayer->GetLevel() > vehicle->GetScript().mCondition.mPlayerMaximumLevel )
	{
		SendErrorMessage( *askedPlayer, MSG_VEHICLE_ERROR::ErrorMountByPlayerLevel );
		return;
	}
	else
	{
		VECTOR3 collidePosition = { 0 };
		VECTOR3 askedPlayerPosition = { 0 };
		askedPlayer->GetPosition( &askedPlayerPosition );
		const BOOL isCollide = g_pServerSystem->GetMap()->CollisionCheck(
			&askedPlayerPosition,
			&askedPlayerPosition,
			&collidePosition,
			askedPlayer );

		// 090316 LUJ, 탑승 불가능한 위치일 때 탑승하면 맵 오브젝트에 끼어 이동할 수 없는 경우가 있다.
		if( isCollide )
		{
			SendErrorMessage( *askedPlayer, MSG_VEHICLE_ERROR::ErrorMountByCollision );
			return;
		}
	}

	const MSG_VEHICLE_ERROR::Error mountError = vehicle->Mount( playerIndex, seatIndex );

	if( MSG_VEHICLE_ERROR::ErrorNone != mountError )
	{
		SendErrorMessage( *askedPlayer, mountError );
		return;
	}

	// 탑승시 탈것의 위치에 탑승자를 이동시킨다
	VECTOR3 vecVehiclePos = *( CCharMove::GetPosition( vehicle ) );
	CCharMove::SetPosition( askedPlayer, &vecVehiclePos );

	// 090316 LUJ, 탑승자 위치 보정
	{
		VECTOR3 vehiclePosition = { 0 };
		vehicle->GetPosition( &vehiclePosition );
		askedPlayer->GetMoveInfo()->CurPosition = vehiclePosition;
	}

	MSG_VEHICLE_ALLOW message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_VEHICLE;
	message.Protocol = MP_VEHICLE_MOUNT_ALLOW_ACK;
	message.mOwnerObjectIndex = vehicle->GetOwnerIndex();
	message.mMountedObjectIndex = askedPlayer->GetID();
	message.mMountedSeatIndex = vehicle->GetMountedSeat( playerIndex );
	SafeStrCpy( message.mMountedObjectName, askedPlayer->GetObjectName(), sizeof( message.mMountedObjectName ) / sizeof( *( message.mMountedObjectName ) ) );
	message.mVehicleObjectIndex = vehicle->GetID();
	SafeStrCpy( message.mVehicleObjectName, vehicle->GetObjectName(), sizeof( message.mVehicleObjectName ) / sizeof( *( message.mVehicleObjectName ) ) );
	message.mTollMoney = ( needBuff ? vehicle->GetScript().mCondition.mTollMoney : 0 );
	PACKEDDATA_OBJ->QuickSend( askedPlayer, &message, sizeof( message ) );

	if(FALSE == vehicle->IsInfiniteSummon())
	{
		CPlayer* const ownedPlayer = GetPlayer(
			vehicle->GetOwnerIndex());

		if( 0 == ownedPlayer)
		{
			return;
		}

		const GridPosition* const askedPlayerGridPosition = askedPlayer->GetGridPosition();
		const GridPosition* const ownedPlayerGridPosition = ownedPlayer->GetGridPosition();

		// 090316 LUJ, 탑승자와 소유주의 그리드 위치가 다를 경우 탑승료 갱신을 위해 직접 전송해야 한다
		if( askedPlayerGridPosition->x != ownedPlayerGridPosition->x ||
			askedPlayerGridPosition->z != ownedPlayerGridPosition->z )
		{
			ownedPlayer->SendMsg( &message, sizeof( message ) );
		}
	}
}

MSG_VEHICLE_ERROR::Error CVehicleManager::MountCheck( CPlayer& ownedPlayer, CPlayer& askedPlayer, CVehicle& vehicle ) const
{
	if( vehicle.GetOwnerIndex() != ownedPlayer.GetID() )
	{
		return MSG_VEHICLE_ERROR::ErrorMountByNoVehicle;
	}
	// 090316 LUJ, 소환한 상태에서 다른 플레이어의 탈것에 탑승할 수 없다
	else if( 0 < askedPlayer.GetSummonedVehicle() &&
		askedPlayer.GetSummonedVehicle() != vehicle.GetID() )
	{
		return MSG_VEHICLE_ERROR::ErrorMountBySummonedAlready;
	}
	// 091106 ShinJS --- PK 모드상태 탑승 불가
	else if( askedPlayer.IsPKMode() )
	{
		return MSG_VEHICLE_ERROR::ErrorMountByState;
	}
	else if( askedPlayer.IsShowdown() )
	{
		return MSG_VEHICLE_ERROR::ErrorMountByState;
	}
	else if( IsInvalidDistance( askedPlayer, vehicle ) )
	{
		return MSG_VEHICLE_ERROR::ErrorMountByDistance;
	}
	// 090316 LUJ, 레벨이 낮으면 탑승할 수 없다.
	else if( askedPlayer.GetLevel() < vehicle.GetScript().mCondition.mPlayerMinimumLevel )
	{
		return MSG_VEHICLE_ERROR::ErrorMountByPlayerLevel;
	}
	else if( askedPlayer.GetLevel() > vehicle.GetScript().mCondition.mPlayerMaximumLevel )
	{
		return MSG_VEHICLE_ERROR::ErrorMountByPlayerLevel;
	}
	else if( IsInvalidState( EObjectState( askedPlayer.GetState() ) ) )
	{
		return MSG_VEHICLE_ERROR::ErrorMountByState;
	}

	return MSG_VEHICLE_ERROR::ErrorNone;
}

BOOL CVehicleManager::Dismount( const DWORD playerIndex, const BOOL dismountByPlayer ) const
{
	CPlayer* const player = GetPlayer( playerIndex );

	if( 0 == player )
	{
		return FALSE;
	}
	else if( IsInvalidState( EObjectState( player->GetState() ) ) )
	{
		SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorDismountByState );
		return FALSE;
	}
	
	CVehicle* const vehicle = GetVehicle( player->GetMountedVehicle() );

	if( 0 == vehicle )
	{
		return FALSE;
	}
	else if( dismountByPlayer )
	{
		if( IsInvalidDistance( *player, *vehicle ) )
		{
			SendErrorMessage( *player, MSG_VEHICLE_ERROR::ErrorDismountbyDistance );
			return FALSE;
		}
	}

	const DWORD dismountedSeatIndex = vehicle->GetMountedSeat( playerIndex );

	const MSG_VEHICLE_ERROR::Error error = vehicle->Dismount( playerIndex, dismountByPlayer );
	if( MSG_VEHICLE_ERROR::ErrorNone != error )
	{
		SendErrorMessage( *player, error );
		return FALSE;
	}

	RemoveVehiclePassengerToDb( playerIndex );

	MSG_DWORD3 message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_VEHICLE;
	message.Protocol = MP_VEHICLE_DISMOUNT_ACK;
	message.dwData1 = playerIndex;
	message.dwData2 = vehicle->GetID();
	message.dwData3 = dismountedSeatIndex;
	PACKEDDATA_OBJ->QuickSend( player, &message, sizeof( message ) );
	return TRUE;
}

CPlayer* CVehicleManager::GetPlayer( DWORD playerIndex ) const
{
	CPlayer* const player = ( CPlayer* )g_pUserTable->FindUser( playerIndex );

	return ( player && player->GetObjectKind() == eObjectKind_Player ) ? player : 0;
}

CVehicle* CVehicleManager::GetVehicle( DWORD vehicleIndex ) const
{
	CVehicle* const vehicle = ( CVehicle* )g_pUserTable->FindUser( vehicleIndex );

	return ( vehicle && vehicle->GetObjectKind() == eObjectKind_Vehicle ) ? vehicle : 0;
}

BOOL CVehicleManager::IsInvalidDistance( CObject& object, CVehicle& vehicle ) const
{
	const VehicleScript& vehicleScript = GAMERESRCMNGR->GetVehicleScript( vehicle.GetMonsterKind() );

	VECTOR3 objectPosition = { 0 };
	object.GetPosition( &objectPosition );
	VECTOR3 vehiclePosition = { 0 };
	vehicle.GetPosition( &vehiclePosition );

	return vehicleScript.mCondition.mSummonRange < CalcDistanceXZ( &objectPosition, &vehiclePosition );
}

BOOL CVehicleManager::Teleport( const DWORD playerIndex, const MAPTYPE mapType ) const
{
	const MAPCHANGE_INFO* mapChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum(WORD(mapType));

	if(0 == mapChangeInfo)
		return FALSE;

	MAPCHANGE_INFO	changeMapInfo = *mapChangeInfo;
	
	changeMapInfo.MovePoint = *GAMERESRCMNGR->GetRevivePoint( changeMapInfo.MoveMapNum );
	
	return Teleport( playerIndex, &changeMapInfo );
}

BOOL CVehicleManager::Teleport( const DWORD playerIndex, const MAPCHANGE_INFO* const mapChangeInfo ) const
{
	CPlayer* const player = GetPlayer( playerIndex );

	if( 0 == player )
	{
		return FALSE;
	}
	else if(g_csDateManager.IsChallengeZone(mapChangeInfo->MoveMapNum))
	{
		return FALSE;
	}
	else if(HOUSINGMAPNUM == mapChangeInfo->MoveMapNum)
	{
		return FALSE;
	}

	CVehicle* const vehicle = GetVehicle( player->GetSummonedVehicle() );

	if( 0 == vehicle )
	{
		return FALSE;
	}
	else if( FALSE == vehicle->Teleport( playerIndex, mapChangeInfo ) )
	{
		return FALSE;
	}

	UpdateVehicleToDb(
		vehicle->GetOwnerIndex(),
		vehicle->GetUseItem(),
		mapChangeInfo->MoveMapNum );
	return TRUE;
}

BOOL CVehicleManager::IsInvalidState( const EObjectState objectState ) const
{
	switch( objectState )
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
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CVehicleManager::IsInvalidShowdown( CPlayer& player )
{
	CObject* const vehicleObject = g_pUserTable->FindUser( player.GetMountedVehicle() );

	if( 0 == vehicleObject )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CVehicleManager::Recall( const DWORD playerIndex )
{
	CPlayer* const playerObject = GetPlayer( playerIndex );

	if( 0 == playerObject )
	{
		return FALSE;
	}

	CVehicle* const vehicleObject = GetVehicle( playerObject->GetSummonedVehicle() );

	if( 0 == vehicleObject )
	{
		return FALSE;
	}

	VECTOR3 playerPosition = { 0 };
	playerObject->GetPosition( &playerPosition );

	return vehicleObject->Recall( playerPosition );
}

void CVehicleManager::Summon(DWORD monsterKind, VECTOR3& position, DWORD playerIndex)
{
	if(mLocationContainer.end() != mLocationContainer.find(position))
	{
		CObject* const object = g_pUserTable->FindUser(
			playerIndex);

		if(object)
		{
			MSGBASE message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_CHEAT;
			message.Protocol = MP_CHEAT_VEHICLE_SUMMON_NACK;

			object->SendMsg(
				&message,
				sizeof(message));
		}

		return;
	}

	const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript(
		monsterKind);

	if(0 == script.mMonsterKind)
	{
		return;
	}

	for(WORD channel = 0; CHANNELSYSTEM->GetChannelCount() > channel; ++channel)
	{
		CVehicle* const vehicle = REGENMGR->RegenVehicle(
			channel + 1,
			script.mMonsterKind,
			position);

		if(0 == vehicle)
		{
			break;
		}

		mLocationContainer[position].insert(
			vehicle->GetID());
	}
}

void CVehicleManager::Unsummon(const VECTOR3& position)
{
	const LocationContainer::iterator locationIterator = mLocationContainer.find(
		position);

	if(mLocationContainer.end() == locationIterator)
	{
		return;
	}

	const ObjectIndexContainer& objectIndexContainer = locationIterator->second;

	for(ObjectIndexContainer::const_iterator objectIndexIterator = objectIndexContainer.begin();
		objectIndexContainer.end() != objectIndexIterator;
		++objectIndexIterator)
	{
		CObject* const object = g_pUserTable->FindUser(
			*objectIndexIterator);

		if(0 == object)
		{
			continue;
		}

		object->DoDie(
			0);
	}

	mLocationContainer.erase(
		locationIterator);
}

void CVehicleManager::Save() const
{
	std::ofstream file(
		mFileName);

	if(! file)
	{
		return;
	}

	file
		<< "kind\t x\t y\n"
		<< "....\t .\t .\n";

	for(LocationContainer::const_iterator iterator = mLocationContainer.begin();
		mLocationContainer.end() != iterator;
		++iterator)
	{
		const ObjectIndexContainer& objectIndexContainer = iterator->second;
		const ObjectIndex objectIndex = (objectIndexContainer.empty() ? 0 : *objectIndexContainer.begin());
		CVehicle* const vehicleObject = (CVehicle*)g_pUserTable->FindUser(
			objectIndex);

		if(0 == vehicleObject)
		{
			continue;
		}
		else if(eObjectKind_Vehicle != vehicleObject->GetObjectKind())
		{
			continue;
		}

		VECTOR3 position = {0};
		vehicleObject->GetPosition(
			&position);

		file
			<< vehicleObject->GetMonsterKind() << '\t'
			<< DWORD(position.x) << '\t'
			<< DWORD(position.z) << '\n';
	}
}

void CVehicleManager::Load()
{
	std::ifstream file(
		mFileName);

	if(! file)
	{
		return;
	}

	while(false == file.eof())
	{
		TCHAR text[MAX_PATH] = {0};
		file.getline(
			text,
			sizeof(text) / sizeof(*text));

		LPCTSTR textSeperator = _T("\t");
		LPCTSTR textMonsterKind = _tcstok(
			text,
			textSeperator);
		LPCTSTR textPositionX = _tcstok(
			0,
			textSeperator);
		LPCTSTR textPositionZ = _tcstok(
			0,
			textSeperator);

		const DWORD monsterKind = _ttoi(
			textMonsterKind ? textMonsterKind : "");
		VECTOR3 position = {0};
		position.x = float(
			_tstof(textPositionX ? textPositionX : ""));
		position.z = float(
			_tstof(textPositionZ ? textPositionZ : ""));

		Summon(
			monsterKind,
			position,
			0);
	}
}

void CVehicleManager::GetSummonedVehicle(DWORD playerIndex) const
{
	CObject* const object = g_pUserTable->FindUser(
		playerIndex);

	if(0 == object)
	{
		return;
	}

	MSG_SKILL_LIST message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_CHEAT;
	message.Protocol = MP_CHEAT_VEHICLE_GET_ACK;	

	for(LocationContainer::const_iterator locationIterator = mLocationContainer.begin();
		mLocationContainer.end() != locationIterator;
		++locationIterator)
	{
        const VECTOR3& position = locationIterator->first;
		const ObjectIndexContainer& objectIndexContainer = locationIterator->second;
		const ObjectIndex objectIndex = (objectIndexContainer.empty() ? 0 : *objectIndexContainer.begin());

		CVehicle* const vehicleObject = (CVehicle*)g_pUserTable->FindUser(
			objectIndex);

		if(0 == vehicleObject)
		{
			continue;
		}
		else if(eObjectKind_Vehicle != vehicleObject->GetObjectKind())
		{
			continue;
		}

		SKILL_BASE& data = message.mData[message.mSize++];
		data.dwDBIdx = MAKELPARAM(
			position.x,
			position.z);
		data.wSkillIdx = vehicleObject->GetMonsterKind();
	}

	object->SendMsg(
		&message,
		message.GetSize());
}

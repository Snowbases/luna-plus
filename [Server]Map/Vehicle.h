/*
090316 LUJ, 탈것 클래스
*/
#pragma once
#include "monster.h"

class CVehicle : public CMonster
{
public:
	CVehicle(void);
	virtual ~CVehicle(void);
	virtual BOOL Init( EObjectKind, DWORD agentNum, BASEOBJECT_INFO* );
	virtual void DoStand();
	virtual void DoRest( BOOL bStart );
	virtual BOOL DoWalkAround();
	virtual BOOL DoPursuit( CObject* = 0 );
	virtual BOOL DoAttack( DWORD attackNum );
	// 090316 LUJ, 프로세스 처리
	virtual void StateProcess();
	virtual void DoDie( CObject* attacker );
	virtual DWORD SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*&);
	// 091212 ShinJS --- 걷기/뛰기 속도 분류
	virtual float DoGetMoveSpeed() { return (m_MoveInfo.MoveMode == eMoveMode_Run ? mRunSpeed : mWalkSpeed); }
	virtual DWORD Damage( CObject*, RESULTINFO* );
	DWORD GetEmptySeatForPassenger() const;
	DWORD GetEmptySeatForDriver();
	inline DWORD GetMountedSeatSize() const { return mMountedSeatSize; }
	// 090316 LUJ, 사용한 아이템을 설정한다
	inline void SetUseItem( const ICONBASE& iconBase ) { mUsedItemToSummon = iconBase; }
	inline const ICONBASE& GetUseItem() const { return mUsedItemToSummon; }
	// 091212 ShinJS --- 걷기/뛰기 속도 분류
	inline void SetMoveSpeed( float runSpeed, float walkSpeed ) { mRunSpeed = runSpeed;		mWalkSpeed = walkSpeed; }
	MSG_VEHICLE_ERROR::Error Mount( DWORD playerIndex );
	MSG_VEHICLE_ERROR::Error Mount( DWORD playerIndex, DWORD seatIndex );
	// 090316 LUJ, 좌석에서 내린다. 내린 좌석 번호가 반환된다
	MSG_VEHICLE_ERROR::Error Dismount( const DWORD playerIndex, const BOOL needBuff );
	// 090316 LUJ, 해당 플레이어가 탑승한 좌석 번호를 얻는다
	DWORD GetMountedSeat( const DWORD playerIndex ) const;
	const VehicleScript& GetScript() const;
	// 090316 LUJ, 해당 좌석에 있는 플레이어 번호를 구한다
	DWORD GetPlayerInSeat( const DWORD seatIndex ) const;
	MSG_VEHICLE_ERROR::Error Move( const DWORD askedPlayerIndex, VECTOR3& startPosition, VECTOR3& targetPosition );
	MSG_VEHICLE_ERROR::Error EndMove( const DWORD askedPlayerIndex, VECTOR3& position );	
	BOOL Teleport( const DWORD askedPlayerIndex, const MAPCHANGE_INFO* const mapChangeInfo );
	// 090316 LUJ, 특정 플레이어에게 스크립트에 정의된 버프를 적용한다
	void AddBuff( const DWORD playerIndex );
	// 090408 ShinJS, Owner와의 거리가 멀어진 경우 순간이동 시킨다
	BOOL Recall( const VECTOR3& );
	// 100121 ShinJS --- 특정 플레이어 이외의 모든 플레이어를 하차시킨다
	void DismountAllExceptOne( DWORD dwExceptIndex );
	BOOL IsInfiniteSummon() const { return UINT_MAX == m_MonsterInfo.OwnedObjectIndex; }

private:
	// 090316 LUJ, 좌석 번호가 정상인지 검사한다
	BOOL IsInvalidSeatIndex( const DWORD seatIndex ) const;
	// 090316 LUJ, 좌석에 플레이어를 설정한다
	void SetSeatMountedPlayer( const DWORD playerIndex, const DWORD seatIndex );
	void SetSeatSize( const VehicleScript& );
	// 090316 LUJ, 탑승한 모든 플레이어를 하차시킨다
	void DismountAll();
	BOOL Teleport(DWORD playerIndex, const MAPCHANGE_INFO&);

private:
	// 090316 LUJ, 좌석 번호별로 탑승 중인 플레이어 번호가 들어있다. [인덱스 = 좌석 번호 - 1]
	DWORD mSeat[VehicleScript::Seat::MaxSize];
	ICONBASE mUsedItemToSummon;
	// 090316 LUJ, 
	DWORD mMountedSeatSize;
	// 091212 ShinJS --- Run/Walk 구분
	float mRunSpeed;
	float mWalkSpeed;
	// 090316 LUJ, 이동 정보가 동기화된 좌석 번호. 한 프로세스 타임에 하나씩 동기화시킨다
	DWORD mMoveSyncedSeatIndex;
};
// ----------------------------- 
// 090422 ShinJS --- 탈것 Class
// ----------------------------- 
#pragma once

#include "Object.h"
#include "Monster.h"

class CVehicle : public CMonster
{
	BOOL		m_bInstalled;											// 설치 여부(장소지정 전 판단)
	char		m_MasterName[ MAX_NAME_LENGTH+1 ];						// 소유주의 이름
	DWORD		m_dwLastDistCheckTime;									// 따라가기시 마지막 거리확인 시각
	DWORD		m_dwRequestGetOnPlayerID;								// 탑승 요청을 한 Player ID 저장

	CYHHashTable<DWORD> m_htRiderInfo;									// 탑승자 정보 저장 테이블 [ 탑승위치(1~mSeatSize)-Key, CObject ID-Data ]
	// 090316 LUJ, 소환된 아이템 정보
	ICONBASE mUsedItemToSummon;
	DWORD		m_dwExecutedEffectHandle;
	int			m_nExecutedEffectNum;
	BOOL		m_bFollow;

public:
	enum{	
		eFollowRandomDist	= 100,										// 따라가기 실행시 랜덤하게 더해줄 최대 거리 (위치 = Master위치 + 탈것 반지름+ FollowRandomDist)
		eFollowExecuteDist	= 300,										// 따라가기 실행가능 거리
		eGetOnLimitTime		= 2000,										// 재탑승 제한시간(2초)
		eRecallExecuteDist	= 3000,										// 순간이동 실행가능 거리
		eSummonLimitTime	= 5000,										// 소환/봉인 제한시간(5초)
	};

	CVehicle();
	virtual ~CVehicle();

	void InitVehicle();
	void Release();

	virtual void Process();
	virtual void SetMotionInState(EObjectState);
	virtual float DoGetMoveSpeed();

	void SetInstall( BOOL bInstall ) { m_bInstalled = bInstall; }											// 설치 여부 설정	
	void SetMasterName( const char* name ) { SafeStrCpy( m_MasterName, name, MAX_NAME_LENGTH+1 ); }			// 소유주 이름 설정
	void SetRequestGetOnPlayerID( DWORD dwPlayerID ) { m_dwRequestGetOnPlayerID = dwPlayerID; }				// 탑승 요청을 한 Player ID 설정
	BOOL IsInstalled() const { return m_bInstalled; }
	BOOL HasRider() { return m_htRiderInfo.GetDataNum() > 0; }
	virtual DWORD GetOwnerIndex() const { return GetMonsterTotalInfo().OwnedObjectIndex; }
	const char* GetMasterName() const { return m_MasterName; }
	DWORD GetRequestGetOnPlayerID() const { return m_dwRequestGetOnPlayerID; }								// 탑승 요청을 한 Player ID 구하기

	void SetPosToMousePos();																				// 마우스에 해당하는 위치로 탈것 이동(설치전)
	BOOL IsValidPosForInstall();																			// 설치하기 적절한 위치인지 판단

	void GetOn( CPlayer*, DWORD dwSeatPos  );																// 탑승
	BOOL GetOff( CPlayer* pPlayer, DWORD dwSeatPos );														// 탑승해제
	// 090316 LUJ, 모든 승객을 내리게 한다
	void GetOffAll();
	BOOL IsGetOn( CPlayer* pPlayer );																		// 탑승여부 확인
	DWORD CanGetOn( CPlayer* pPlayer );																		// 탑승 가능 여부 확인, 가능시 탑승 위치 반환
	BOOL CanGetOff( CPlayer* pPlayer );																		// 하차 가능 여부 확인

	DWORD GetRiderID( DWORD dwSeatPos ) { return DWORD(m_htRiderInfo.GetData( dwSeatPos )); }				// 좌석정보로 탑승자 ID 구하기
	DWORD GetEmptySeatPos( DWORD dwPlayerID );																// 비어있는 좌석번호 구하기

	BOOL CanControl( CPlayer* pPlayer );																	// 이동을 할수 있는지 판단
	void Move_OneTarget( MOVE_ONETARGETPOS_FROMSERVER* pMsg );												// 위치로 이동(다인승의 경우 탑승자 이동)
	void StartMove( VECTOR3* pTargetPos );																	// 이동시작(다인승의 경우 탑승자 이동)
	void MoveStop( VECTOR3* pVecPos );																		// 이동 멈춤(다인승의 경우 탑승자 이동멈춤)

	DWORD GetTollMoney();																					// 탈것의 탑승요금 구하기
	// 090316 LUJ, 소환에 필요한 아이템 정보
	inline const ICONBASE& GetUsedItem() const { return mUsedItemToSummon; }
	inline void SetUsedItem( const ICONBASE& usedItem ) { mUsedItemToSummon = usedItem; }
	const char* GetBeffFileNameFromMotion(DWORD dwMotionType);											// 모션종류에 대한 Beff파일이름을 반환
	void ChangeBeff( DWORD dwMotionType );
	void SetFollow( BOOL bFollow ) { m_bFollow = bFollow; }
};

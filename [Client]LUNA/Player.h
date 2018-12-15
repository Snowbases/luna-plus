#pragma once

#include "Object.h"

struct MOD_LIST;
class cStreetStallTitleTip;
class cImageSelf;
struct SetScript;


class CPlayer : public CObject  
{
protected: 
	CHARACTER_TOTALINFO	m_CharacterInfo;
	cStreetStallTitleTip* m_pSSTitle;
	cTimeDelay m_FacialTime;
	BOOL m_bStartCheck;
	cTimeDelay m_EyeTime;
	BOOL m_bActiveEye;
	BOOL m_bClosed;
	BOOL m_bRunningEmotion;
	CEngineObject* m_pWingObject;
	DWORD m_ChangedPartIdx[eWearedItem_Max];

private:
	int m_StandardMotion;
	int	m_Move_Start_Motion;
	int	m_Move_Ing_Motion;
	int	m_Move_End_Motion;
	int	m_ItemUseMotion;
	int	m_ItemUseEffect;
	CEngineObject* m_pFaceShapeObject;
	stFACEACTION m_stFaceAction;
	cImageSelf*	m_pcsFamilyEmblem;
	DWORD m_nFarmMotionTimeTick;
	DWORD m_nFarmMotionTime;
	DWORD m_dwImmortalTimeOnGTMAP;

public:
	DWORD mWeaponEffect[3];
	DWORD mJobEffect;

	CPlayer();
	virtual	~CPlayer();

private:
	void InitPlayer(CHARACTER_TOTALINFO*);
	void ProcFarmMotion();
	void ProcessFaceAction();

public:
	/// 관련 클래스 및 함수
	friend class			CObjectManager;
	friend class			CAppearanceManager;
	friend void				CS_BtnFuncEnter( LONG lId, void* p, DWORD we );
	void GetCharacterTotalInfo(CHARACTER_TOTALINFO*);
	CHARACTER_TOTALINFO* GetCharacterTotalInfo() { return &m_CharacterInfo; }
	virtual void SetLevel(LEVELTYPE level) { m_CharacterInfo.Level = level; }
	LEVELTYPE GetLevel() const { return m_CharacterInfo.Level; }
	BYTE GetGender() const { return m_CharacterInfo.Gender; }
	BYTE GetRace() const { return m_CharacterInfo.Race; }
	virtual void SetStage( BYTE grade, BYTE index );
	BYTE GetStage() const { return m_CharacterInfo.JobGrade; }
	void SetCharChangeInfo(CHARACTERCHANGE_INFO*);
	void SetCharacterSize(float fSize);
	float GetCharacterSize() const { return m_CharacterInfo.Height; }

	virtual DWORD GetLife() { return m_CharacterInfo.Life; }
	virtual void SetMaxLife(DWORD maxlife) { m_CharacterInfo.MaxLife = maxlife; }
	virtual void SetLife( DWORD life, BYTE type = 1 );
	virtual DWORD DoGetMaxLife();
	virtual float DoGetMoveSpeed();
	BYTE GetLifePercent();

	void SetGuildMark();
	void SetNickName();
	void SetFamilyMark();
	virtual void			ClearGuildInfo();
	void					SetGuildIdx( DWORD GuildIdx );
	DWORD					GetGuildIdx() { return m_CharacterInfo.MunpaID; }
	void					SetGuildIdxRank(DWORD GuildIdx, BYTE Rank); 
	void					SetGuildMemberRank(BYTE rank) { m_CharacterInfo.PositionInMunpa = rank; } //문파에서의 자신의 지위 설정
	BYTE					GetGuildMemberRank() { return m_CharacterInfo.PositionInMunpa; } //문파에서의 자신의 지위
	void					SetGuildMarkName( MARKNAMETYPE MarkName );
	MARKNAMETYPE			GetGuildMarkName();
	void					SetGuildName( char* GuildName );
	char*					GetGuildName();
	void					SetNickName( char* pNickName );
	char*					GetNickName();

	// 동맹 길드
	void					SetGuildUnionIdx( DWORD dwUnionIdx ) { m_CharacterInfo.dwGuildUnionIdx = dwUnionIdx; }
	void					SetGuildUnionName( char* pName ) { strncpy( m_CharacterInfo.sGuildUnionName, pName, MAX_GUILD_NAME+1 ); }
	void					SetGuildUnionMarkIdx( DWORD dwMarkIdx )	{ m_CharacterInfo.dwGuildUnionMarkIdx = dwMarkIdx; }
	DWORD					GetGuildUnionIdx() { return m_CharacterInfo.dwGuildUnionIdx; }
	char*					GetGuildUnionName() { return m_CharacterInfo.sGuildUnionName; }
	DWORD					GetGuildUnionMarkIdx() { return m_CharacterInfo.dwGuildUnionMarkIdx; }

	// 패밀리
	virtual void			ClearFamilyInfo();
	void					SetFamilyIdx( DWORD FamilyIdx );
	DWORD					GetFamilyIdx() { return m_CharacterInfo.FamilyID; }
	void					SetFamilyMarkName(MARKNAMETYPE);
	MARKNAMETYPE			GetFamilyMarkName();
	void					SetFamilyName( char* FamilyName );
	char*					GetFamilyName();
	void					SetFamilyNickName( char* pNickName );
	char*					GetFamilyNickName();
	// 길드토너먼트
	void					SetImmortalTimeOnGTMAP(DWORD dwTime) {m_dwImmortalTimeOnGTMAP = dwTime;}
	DWORD					GetImmortalTime() {return m_dwImmortalTimeOnGTMAP;}
	void					SetGTName(DWORD NameType);
	void SetPKMode(BOOL bPKMode);
	BOOL IsPKMode() const { return m_CharacterInfo.bPKMode; }
	virtual void SetBadFame(FAMETYPE val);
	FAMETYPE GetBadFame() const { return m_CharacterInfo.BadFame; }
	virtual void SetMotionInState(EObjectState);
	void SetRunningEmotion(BOOL val) { m_bRunningEmotion  = val ; }

	// 070509 LYW --- Player : Add function to return to check emotion.
	BOOL IsRunningEmotion() { return m_bRunningEmotion ; }

	void					SetBaseMotion();
	int						GetStandardMotion() { return m_StandardMotion; }

	/// 전투/평화모드
	void					SetPeaceMode( bool bPeace )	{ m_CharacterInfo.bPeace = bPeace; }

	/// 행동
	virtual BOOL			StartSocietyAct( int nStartMotion, int nIngMotion = -1, int nEndMotion = -1, BOOL bHideWeapon = FALSE );

	/// 이펙트
	BOOL					OnEndEffectProc( HEFFPROC hEff );

	/// 아이템 사용 연출
	void					SetItemUseMotionIdx( int Idx ) { m_ItemUseMotion = Idx;	}
	void					SetItemUseEffectIdx( int Idx ) { m_ItemUseEffect = Idx;	}

	///--------------------------------------------------------------------/
	/// 프로세스 관련 함수-------------------------------------------------/

	/// 프로세스
	void					Process();
	virtual void			OnStartObjectState(EObjectState);
	virtual void			OnEndObjectState(EObjectState);
	virtual void			Die( CObject* pAttacker, BOOL bFatalDamage, BOOL bCritical, BOOL bDecisive );
	// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
	virtual void			Damage( CObject* pAttacker, BYTE DamageKind, DWORD Damage, const RESULTINFO&);
	virtual void			ManaDamage( CObject* pAttacker, BYTE DamageKind, DWORD Damage, const RESULTINFO&);
	virtual void			Heal( CObject* pHealer, BYTE HealKind, DWORD HealVal );
	virtual void			Recharge( CObject* pHealer, BYTE RechargeKind, DWORD RechargeVal );

	void SetWearedItemIdx(DWORD WearedPosition, DWORD ItemIdx);
	DWORD GetWearedItemIdx(DWORD WearedPosition) const { return m_CharacterInfo.WearedItemIdx[WearedPosition]; }
	eWeaponType	GetWeaponEquipType();
	eWeaponAnimationType GetWeaponAniType();
	eWeaponAnimationType GetWeaponAniType(EWEARED_ITEM);
	eArmorType GetArmorType(EWEARED_ITEM );
	DWORD GetChangePart(DWORD WearedPosition);
	void ClearChangeParts();

	void ShowStreetStallTitle(BOOL bShow, char* strTitle, DWORD dwFontColor = RGB_HALF(10,10,10));
	void SetStreetStallTitle(char* strTitle);
	void ShowStreetBuyStallTitle(BOOL bShow, char* strTitle, DWORD dwFontColor = RGB_HALF(10,10,10));
	void SetStreetBuyStallTitle(char* strTitle);
	void RefreshStreetStallTitle();
	void ChangeFace(BYTE faceNum);
	void ReturnFace();
	void PlayEmoticon_Syn(BYTE EmoticonNum);
	void PlayEmoticon(int nEmoticonNum);
	void CloseEye();
	void InitializeEye();
	MOD_LIST* GetModList();
	cTimeDelay* GetEyeTime() { return &m_EyeTime; }
	void SetResidentRegistInfo(DATE_MATCHING_INFO* pInfo) { m_CharacterInfo.DateMatching = *pInfo; }
	void SetFaceObject(CEngineObject* pObject) { m_pFaceShapeObject = pObject; }
	CEngineObject* GetFaceObject() { return m_pFaceShapeObject ; }
	void SetFaceAction(int nAction, int nShape);
	void SetFaceShape(int nShape) { if(m_pFaceShapeObject)m_pFaceShapeObject->SetMaterialIndex(nShape); }
	cImageSelf*	GetFamilyEmblem() { return m_pcsFamilyEmblem; }
	void SetFamilyEmblem(cImageSelf* pcsEmblem) { m_pcsFamilyEmblem = pcsEmblem; }
	virtual void Release();
	void StartFarmMotion(int nMotionNum, DWORD nMotionTime);

	BOOL CheckHeart(CPlayer*);
	const DATE_MATCHING_INFO& GetDateMatchingInfo() const;
	void SetHeart( BOOL );
	virtual void SetObjectBattleState(eObjectBattleState eBattleState);

protected:
	// 하트 이펙트를 그리고 있는지 여부
	BOOL m_IsDrawHeart;
	

	// 웅주, 070705 세트 아이템 능력 관리
public:
	typedef std::map< const SetScript*, int >	SetItemLevel;

	const SetItemLevel&	GetSetItemLevel() const;
	SetItemLevel&		GetSetItemLevel();

private:
	SetItemLevel			mSetItemLevel;

public:
	void SetWingObject( CEngineObject* pObj ) { m_pWingObject = pObj; }
	void SetWingObjectMotion(eWingMotion);
	void SetWingObjectBaseMotion(eWingMotion);
	void RemoveEffects();
	void StartJobEffect();
	void EndJobEffect();

	// 090422 ShinJS --- 탈것 추가작업
private:
	DWORD m_dwVehicleID;										// 탈것에 가지고 있는 경우 탈것 ID 저장 (값이 0면 없는것으로 간주)
	DWORD m_dwVehicleSeatPos;									// 탈것에 탑승되어 있는 위치 (1번부터, 0:타지 않은 상태)
	DWORD m_dwLastVehicleSummonTime;							// 마지막으로 탈것 소환/봉인한 시각저장 (소환/봉인 딜레이시 사용)
	DWORD m_dwLastVehicleGetOffTime;							// 마지막으로 탑승해제한 시각(재탑승 가능시간 체크시 사용)

public:
	void SetVehicleID( DWORD dwVehicleID )			{ m_dwVehicleID = dwVehicleID; }
	void SetVehicleSeatPos( DWORD dwVehicleSeatPos ){ m_dwVehicleSeatPos = dwVehicleSeatPos; }
	void SetLastVehicleSummonTime( DWORD dwTime )	{ m_dwLastVehicleSummonTime = dwTime; }
	void SetLastVehicleGetOffTime( DWORD dwTime )	{ m_dwLastVehicleGetOffTime = dwTime; }

	DWORD GetVehicleID() const						{ return m_dwVehicleID; }
	DWORD GetVehicleSeatPos() const					{ return m_dwVehicleSeatPos; }
	DWORD GetLastVehicleSummonTime() const			{ return m_dwLastVehicleSummonTime; }
	DWORD GetLastVehicleGetOffTime() const			{ return m_dwLastVehicleGetOffTime; }

	// 탑승여부 확인
	BOOL IsGetOnVehicle() const						{ return m_dwVehicleID && m_dwVehicleSeatPos; }

	// 090423 ShinJS --- 소환/봉인 가능 여부 검사
	BOOL CanSummonVehicle();
	BOOL CanSealVehicle();

private:
	DWORD m_dwRideFurnitureMotionIndex;
	DWORD m_dwRideFurnitureID;										
	DWORD m_dwRideFurnitureSeatPos;	

	//090603 pdy 하우징 탑승버그 수정 m_bRideFurniture 추가 
	BOOL  m_bRideFurniture;

public:
	void SetRideFurnitureMotion(DWORD dwRideMotionIndex)		{ m_dwRideFurnitureMotionIndex = dwRideMotionIndex;}
	void SetRideFurnitureID( DWORD dwFurnitureID )				{ m_dwRideFurnitureID = dwFurnitureID; }
	void SetRideFurnitureSeatPos( DWORD dwFurnitureSeatPos )	{ m_dwRideFurnitureSeatPos = dwFurnitureSeatPos; }

	DWORD GetRideFurnitureID() const								{ return m_dwRideFurnitureID; }
	DWORD GetRideFurnitureSeatPos() const							{ return m_dwRideFurnitureSeatPos; }

	//090603 pdy 하우징 탑승버그 수정 m_bRideFurniture 추가 
	void SetRideFurniture(BOOL bRide)							{ m_bRideFurniture = bRide; }
	BOOL IsRideFurniture()										{return m_bRideFurniture;}

public:
	// 091123 공성전 워터시드 사용중 플레그 추가 
	void SetUsingWaterSeedFlag( BOOL bFlag ) { m_CharacterInfo.bUsingWaterSeed = bFlag ; }
};

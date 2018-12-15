#pragma once

#include "Player.h"
#include "CAction.h"

class CSHFamily;
class CWayPointManager;
class CSound;

class CHero : public CPlayer  
{
	// 기본 정보
	HERO_TOTALINFO 	m_HeroTotalInfo;
	EXPTYPE	m_maxExpPoint;
	PlayerStat	mCharStat;
	PlayerStat	mItemBaseStat;
	PlayerStat	mItemOptionStat;

	// 080313 LUJ, 세트 아이템 스탯 분리
	PlayerStat	mSetItemStat;

	DWORD mWeaponEnchantLevel;
	DWORD mPhysicDefenseEnchantLevel;
	DWORD mMagicDefenseEnchantLevel;
	// 액션
	CAction	m_MovingAction;
	CAction	m_NextAction;
	CAction	m_SkillStartAction;
	// 사회행동
	DWORD m_dwLastSocietyAction;

	DWORD		mAccuracy;
	DWORD		mAvoid;
	DWORD		mBlock;
	DWORD		mPhysicAttackMax;
	DWORD		mPhysicAttackMin;
	DWORD		mMagicAttack;
	DWORD		mPhysicDefense;
	DWORD		mMagicDefense;
	DWORD		mCriticalRate;
	DWORD		mCriticalDamage;
	DWORD		mLifeRecover;
	DWORD		mManaRecover;
	DWORD		mMaxLife;
	DWORD		mMaxMana;

	BYTE		m_StorageNum;
	/// 자동공격	
	BOOL				m_bIsAutoAttackMode;
	BOOL				m_bIsAutoAttacking;
	CActionTarget		m_AutoAttackTarget;

	/// 기본공격
	DWORD				m_CurComboNum;
	DWORD				mSkillDelay;
	DWORD				mSkillCancelDelay;
	DWORD				mCurrentSkillObjectIndex;

	AbnormalStatus mAbnormalStatus;
	Status				mBuffStatus;
	Status				mRateBuffStatus;
	Status				mPassiveStatus;
	Status				mRatePassiveStatus;
	CSHFamily* m_pcsFamily;
	stMONSTERMETER m_stMonstermeterInfo;
	BOOL m_bNoDeffenceFromArmor;
	BYTE m_byExtendedInvenCount;
	CSound* mMoveSound;

public:
	CHero();
	virtual ~CHero();
	void				InitHero( HERO_TOTALINFO*);
	void				GetHeroTotalInfo(HERO_TOTALINFO*);
	HERO_TOTALINFO*		GetHeroTotalInfo() { return &m_HeroTotalInfo; }

	/// 경험치
	EXPTYPE				GetExpPoint() { return m_HeroTotalInfo.ExpPoint; }
	EXPTYPE				GetMaxExpPoint() { return m_maxExpPoint; }
	void				SetExpPoint( EXPTYPE dwPoint, BYTE type=1 );
	void				SetMaxExpPoint( EXPTYPE dwPoint );
	virtual void		SetLevel( LEVELTYPE level );

	void				SetSkillPoint( DWORD point );
	void				SetHeroLevelupPoint( LEVELTYPE dwPoint ) { m_HeroTotalInfo.LevelUpPoint = dwPoint; }
	DWORD				GetHeroLevelUpPoint() { return m_HeroTotalInfo.LevelUpPoint; }
	/// 상태 알림
	void				HeroStateNotify( BYTE State );

	void				SetWeaponEnchantLevel( DWORD level )		{ mWeaponEnchantLevel = level; }
	void				SetPhysicDefenseEnchantLevel( DWORD level )	{ mPhysicDefenseEnchantLevel = level; }
	void				SetMagicDefenseEnchantLevel( DWORD level )	{ mMagicDefenseEnchantLevel = level; }

	const DWORD			GetWeaponEnchantLevel()	const				{ return mWeaponEnchantLevel; }
	const DWORD			GetPhysicDefenseEnchantLevel() const		{ return mPhysicDefenseEnchantLevel; }
	const DWORD			GetMagicDefenseEnchantLevel() const			{ return mMagicDefenseEnchantLevel; }
	/// 스텟 계산 구조체
	PlayerStat&	GetCharStats() { return mCharStat; }
	PlayerStat&	GetItemBaseStats() { return mItemBaseStat; }
	PlayerStat&	GetItemOptionStats() { return mItemOptionStat; }

	// 080313 LUJ, 세트 아이템 스탯 반환
	PlayerStat& GetSetItemStats()	{ return mSetItemStat; }

	/// 기본 스텟
	void				SetStrength( DWORD val );
	void				SetDexterity( DWORD val );
	void				SetVitality( DWORD val );
	void				SetWisdom( DWORD val );
	void				SetIntelligence( DWORD val );

	DWORD				GetVitality()				const;
	DWORD				GetWisdom()					const;
	DWORD				GetStrength()				const;
	DWORD				GetDexterity()				const;
	DWORD				GetIntelligence()			const;

	/// 생명력 / 마나
	virtual void		SetMaxLife( DWORD maxlife );
	virtual void		SetLife( DWORD life, BYTE type = 1 );
	void				ChangeLife( int changeval );

	virtual void		SetMaxMana( DWORD val );
	virtual void		SetMana( DWORD val, BYTE type = 1 );
	virtual DWORD		GetMana() { return m_HeroTotalInfo.Mana; }
	virtual DWORD		DoGetMaxMana();
	BYTE				GetManaPercent();
	void				ChangeMana(int changeval);

	DWORD				GetMaxLifeWithItem();
	DWORD				GetMaxManaWithItem();
	virtual DWORD		DoGetAttackRate();
	virtual DWORD		DoGetDefenseRate();
	virtual DWORD		DoGetMagAttackRate();
	virtual DWORD		DoGetMagDefenseRate();
	virtual DWORD		DoGetAccuracyRate();
	virtual DWORD		DoGetEvasionRate();
	virtual DWORD		DoGetCriticalRate();		
	virtual DWORD		DoGetMagicCriticalRate();		
	virtual DWORD		DoGetMoveSpeedRate();		
	virtual DWORD		DoGetCastingSpeedRate();		
	virtual DWORD		DoGetAttackSpeedRate();		
	virtual void		SetBadFame(FAMETYPE);
	DWORD				GetAccuracy();
	DWORD				GetAvoid();
	DWORD				GetBlock();
	DWORD				GetPhysicAttackMax();
	DWORD				GetPhysicAttackMin();
	DWORD				GetMagicAttack();
	DWORD				GetPhysicDefense();
	DWORD				GetMagicDefense();
	DWORD				GetCriticalRate();
	DWORD				GetCriticalDamage();
	DWORD				GetLifeRecover();
	DWORD				GetManaRecover();
	/// 파티
	DWORD				GetPartyIdx() { return m_HeroTotalInfo.PartyID; }
	void				SetPartyIdx( DWORD PartyId ) { m_HeroTotalInfo.PartyID = PartyId;	}
	virtual void		ClearGuildInfo();
	void				SetGuildName( char* Name );
	char*				GetGuildName();
	void				SetGuildEntryDate( char* day );
	char*				GetGuildEntryDate();
	void				SetFamilyName( char* Name );
	char*				GetFamilyName();
	void				SetFamilyEntryDate( char* day );
	char*				GetFamilyEntryDate();

	/// 사회 행동
	virtual BOOL		StartSocietyAct( int nStartMotion, int nIngMotion = -1, int nEndMotion = -1, BOOL bHideWeapon = FALSE );
	virtual BOOL		EndSocietyAct();
	void				StartSocietyActForQuest( int nStartMotion, int nIngMotion = -1, int nEndMotion = -1, BOOL bHideWeapon = FALSE );
	BOOL				CanSocietyAction( DWORD curTime );

	/// 기본 행동
	void				Die( CObject* pAttacker, BOOL bFatalDamage, BOOL bCritical, BOOL bDecisive );
	// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
	void				Damage( CObject* pAttacker, BYTE DamageKind, DWORD Damage, const RESULTINFO&);
	void				Revive( VECTOR3* pRevivePos );
	void				Heal( CObject* pHealer,BYTE HealKind,DWORD HealVal );

	void				SetMovingAction( CAction* pAction ) { m_MovingAction.CopyFrom( pAction ); }
	void				SetNextAction( CAction* pAction ) { m_NextAction.CopyFrom( pAction );	}
	CAction*			GetNextAction()	{ return &m_NextAction; }
	void				SetSkillStartAction( CAction* pAction ) { m_SkillStartAction.CopyFrom( pAction ); }

	void				Process();
	virtual void		OnStartObjectState(EObjectState);
	virtual void		OnEndObjectState(EObjectState);
	void				ApplyInterface();
	void				SetMoney( MONEYTYPE Money );
	MONEYTYPE			GetMoney() { return m_HeroTotalInfo.Money; }

	/// 창고
	BYTE				GetStorageNum()	{ return m_StorageNum; }
	void				SetStorageNum( BYTE num ) { m_StorageNum = num; }

	friend class		CAttackManager;
	/// 자동공격
	BOOL				IsAutoAttacking() { return m_bIsAutoAttacking; }
	CActionTarget*		GetAutoAttackTarget() { return &m_AutoAttackTarget; }
	void				EnableAutoAttack( CActionTarget* pTarget )
	{	
		if( m_bIsAutoAttackMode && pTarget->GetTargetKind() == eActionTargetKind_Object )
		{
			m_bIsAutoAttacking = TRUE;	
			m_AutoAttackTarget.CopyFrom( pTarget ); 
		}
	}
	void				DisableAutoAttack() 
	{ 
		m_bIsAutoAttacking = FALSE; 
		m_AutoAttackTarget.CopyFrom( NULL ); 
	}
	void				ToggleAutoAttackMode() { m_bIsAutoAttackMode = !m_bIsAutoAttackMode; }

	DWORD				GetCurComboNum() { return m_CurComboNum < MAX_COMBO_NUM ? m_CurComboNum : 0; }
	void				SetCurComboNum( DWORD num )	{ m_CurComboNum = num; }


	/// 타겟 정보
	void				ClearTarget( CObject* pObject );
	void				OnHitTarget( CObject* pMainTarget );
	DWORD				GetSkillDelay() const { return mSkillDelay; }
	void				SetSkillDelay(DWORD skillDelay) { mSkillDelay = skillDelay; }
	void				StartSkillProgressBar( DWORD time );

	Status*				GetBuffStatus() { return &mBuffStatus; }
	Status*				GetRateBuffStatus() { return &mRateBuffStatus; }
	Status*				GetPassiveStatus() { return &mPassiveStatus; }
	Status*				GetRatePassiveStatus() { return &mRatePassiveStatus; }

	AbnormalStatus*	GetAbnormalStatus() { return &mAbnormalStatus; }

	// 090204 LUJ, 패시브 상태에서 조건부 적용하는 스킬을 체크한다
public:
	// 090204 LUJ, 특수 스킬을 추가한다
	void AddSpecialSkill( cBuffSkillInfo* pInfo );
	// 090204 LUJ, 특수 스킬을 제거한다
	void RemoveSpecialSkill( cBuffSkillInfo* pInfo );
private:
	// 090204 LUJ, 특수 스킬을 프로세스 타임에 체크한다
	void ProcessSpecialSkill();
private:
	struct SpecialSkillData
	{
		cBuffSkillInfo*	mBuffSkillInfo;
		BOOL			mIsTurnOn;
	};
	typedef std::list< SpecialSkillData >	SpecialSkillList;
	SpecialSkillList						mSpecialSkillList;

public:
	void AddMatch();
	void RemoveMatch();

	// 그리드 내의 전체 플레이어 상대로 하트 재계산. 히어로의 매칭 정보가 바뀌었을 때 호출되어야 한다.
	void CheckAllHeart();

private:
	// 하트 매칭된 플레이어 수
	int m_matchedQuantity;

public:
	void SetFamily(CSHFamily*);
	CSHFamily* GetFamily() { return m_pcsFamily; }
	void SetMonstermeterPlaytime(DWORD nTime, DWORD nTimeTotal)		{ m_stMonstermeterInfo.nPlayTime = nTime; m_stMonstermeterInfo.nPlayTimeTotal = nTimeTotal; }
	void SetMonstermeterKillMonNum(DWORD nNum, DWORD nNumTotal)		{ m_stMonstermeterInfo.nKillMonsterNum = nNum; m_stMonstermeterInfo.nKillMonsterNumTotal = nNumTotal; }
	void ProcMonstermeterPlayTime();

	// 이동속도 정보 반환
public:
	virtual float DoGetMoveSpeed();
	DWORD GetSkillCancelDelay() const { return mSkillCancelDelay; }
	void SetSkillCancelDelay(DWORD delay) { mSkillCancelDelay = delay; }
	void SetCurrentSkill( const DWORD skillIndex ) { mCurrentSkillObjectIndex = skillIndex; }
	const DWORD GetCurrentSkill() const { return mCurrentSkillObjectIndex; }

//---KES 장비 방어력 적용 여부 071128
	BOOL	IsNoDeffenceFromArmor()		{ return m_bNoDeffenceFromArmor; }
//----------------------------------

	// 071210 LYW --- Hero : HERO의 확장 인벤토리 수를 설정 및 반환하는 함수 추가.
	void Set_HeroExtendedInvenCount(BYTE count) ;
	BYTE Get_HeroExtendedInvenCount() { return m_byExtendedInvenCount ; }

	void PassiveSkillCheckForWeareItem();

//---KES 따라가기
protected:
	DWORD	m_dwFollowPlayerIdx;
public:
	void SetFollowPlayer( DWORD PlayerIdx )		{ m_dwFollowPlayerIdx = PlayerIdx; }
	DWORD GetFollowPlayer()						{ return m_dwFollowPlayerIdx; }
//---------------

	// 090526 ShinJS --- 이름으로 파티초대 Error 누적초과시 작업시각 저장, 경과시간으로 재시도 가능 판단
protected:
	DWORD	m_dwInvitePartyByNameLastTime;				// 작업시각 설정 
	DWORD	m_dwInvitePartyByNameLimitTime;				// 제한시간 설정
public:
	void	SetInvitePartyByNameLastTime( DWORD dwTime )	{ m_dwInvitePartyByNameLastTime = dwTime; }
	void	SetInvitePartyByNameLimitTime( DWORD dwTime )	{ m_dwInvitePartyByNameLimitTime = dwTime; }
	DWORD	GetInvitePartyByNameLastTime() const			{ return m_dwInvitePartyByNameLastTime; }
	DWORD	GetInvitePartyByNameLimitTime() const			{ return m_dwInvitePartyByNameLimitTime; }
	BOOL IsNoEquip(eArmorType, eWeaponType, eWeaponAnimationType);
	BOOL IsEnable(const BUFF_SKILL_INFO&);
	virtual float GetBonusRange() const;
	virtual void SetObjectBattleState(eObjectBattleState eBattleState);

	// 090907 ShinJS --- 5팀 길찾기 Source 적용
protected:	
	CWayPointManager*	m_WayPointManager;																				// 길찾기용 WayPointManager;

public:
	void ClearWayPoint();																								// WayPoint 초기화
    BOOL Move_UsePath( VECTOR3* pDestPos, BOOL bSimpleMode, BOOL bMousePointDisplay, BOOL bIsCollisionPosMove=FALSE );	// 목표 지점 길찾기
	BOOL Move_Simple( VECTOR3* pTargetPos );																			// 직선 이동가능시 목표 지점으로 이동처리
	BOOL NextMove();																									// 길찾기로 저장된 다음 WayPoint로 이동
};
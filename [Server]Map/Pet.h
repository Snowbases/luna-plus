#pragma once
#include "object.h"

class CMonster;

class CPet :
	public CObject
{
	PET_OBJECT_INFO	mpPetObjectInfo;
	DWORD				mTargetID;

	PlayerStat			mPetStat;
	PlayerStat			mItemStat;
	PlayerStat			mItemOptionStat;

	float		m_fPhysicAttackMax;
	float		m_fPhysicAttackMin;
	float		m_fMagicAttackMax;
	float		m_fMagicAttackMin;

	Status				mPassiveStatus;
	Status				mRatePassiveStatus;

	friend class		CCharacterCalcManager;

	ITEMBASE mItemSlot[ePetEquipSlot_Max];

	DWORD				mCheckTime;

	BOOL				mbBuffSkillLoad;
	DWORD				mGravity;
	RECOVER_TIME		m_ManaRecoverTime;
	YYRECOVER_TIME		m_YYManaRecoverTime;

	typedef DWORD ObjectIndex;
	typedef std::set< ObjectIndex > ObjectIndexContainer;
	ObjectIndexContainer mAggroObjectContainer;
	CYHHashTable<CMonster> m_FollowMonsterList;

	// 081022 LYW --- Pet : 펫 버그/기타 테스트를 위하여 Pet에도 GOD 모드를 적용한다.
	BYTE				m_byGod ;
	// 091214 ONS 펫 맵이동시 소환/봉인 관련 메세지를 출력하지 않도록 처리.
	BOOL				m_bIsSummonFlag;
	DWORD mOwnedObjectIndex;

protected:	
	void UpdateGravity();

public:
	CPet(void);
	virtual ~CPet(void);
	void SetObjectInfo(const PET_OBJECT_INFO& pInfo) { mpPetObjectInfo = pInfo; }
	PET_OBJECT_INFO& GetObjectInfo() { return mpPetObjectInfo; }
	virtual BOOL Init(EObjectKind kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo);
	virtual void Release();
	// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
	virtual DWORD SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*&);
	virtual DWORD GetGravity() { return mGravity; }	
	virtual void OnStartObjectState(EObjectState, DWORD dwParam) {}
	virtual void OnEndObjectState(EObjectState);
	virtual void StateProcess();	

	virtual void DoDie(CObject* pAttacker);
	virtual DWORD Damage( CObject* pAttacker, RESULTINFO* pDamageInfo );
	virtual LEVELTYPE GetLevel() { return mpPetObjectInfo.Level; };
	virtual DWORD GetLife() { return mpPetObjectInfo.HP; }
	virtual void SetLife(DWORD Life,BOOL bSendMsg = TRUE);	
	virtual DWORD GetMana(){ return mpPetObjectInfo.MP; }
	virtual void SetMana(DWORD val,BOOL bSendMsg = TRUE);	

	void AddToAggroed(DWORD monsterObjectIndex) { mAggroObjectContainer.insert(monsterObjectIndex); }
	void RemoveFromAggroed(DWORD montserObjectIndex) { mAggroObjectContainer.erase(montserObjectIndex); }
	void RemoveAllAggroed();
	void AddAggroToMyMonsters(int nAggroAdd, DWORD targetObjectIndex, DWORD skillIndex);
	BOOL	AddFollowList( CMonster * pMob );
	void	RemoveFollowList( DWORD ID );
	BOOL	RemoveFollowAsFarAs( DWORD GAmount, CObject* pObject );

	virtual DWORD DoGetMaxLife()	{return mpPetObjectInfo.MAXHP;}
	virtual DWORD DoGetMaxMana()	{return mpPetObjectInfo.MAXMP;}
	virtual DWORD DoGetPhyDefense()	{return (DWORD)mPetStat.mPhysicDefense.mPlus;}
	virtual DWORD DoGetPhyAttackPowerMin()	{return (DWORD)mPetStat.mPhysicAttack.mPlus;}
	virtual DWORD DoGetPhyAttackPowerMax()	{return (DWORD)mPetStat.mPhysicAttack.mPlus;}
	virtual DWORD DoGetMagicAttackPower()	{return (DWORD)mPetStat.mMagicAttack.mPlus;}
	virtual DWORD DoGetMagicDefense()	{return (DWORD)mPetStat.mMagicAttack.mPlus;}
	virtual DWORD DoGetShieldDefense()	{return 0;}
	virtual DWORD DoGetCritical()	{return (DWORD)mPetStat.mCriticalRate.mPlus;}
	virtual DWORD DoGetAccuracy()	{return (DWORD)mPetStat.mAccuracy.mPlus;}
	virtual DWORD DoGetAvoid()	{return (DWORD)mPetStat.mEvade.mPlus;}

	virtual float DoGetMoveSpeed();
	virtual DWORD GetOwnerIndex() const { return mOwnedObjectIndex; }
	void SetMaxLife(DWORD val);
	void SetMaxMana(DWORD val);

	Status&	GetPassiveStatus() { return mPassiveStatus; }
	Status&	GetRatePassiveStatus() { return mRatePassiveStatus; }
	PlayerStat& GetItemStat() { return mItemStat; }
	void SetTarget( DWORD id ) { mTargetID = id; }
	DWORD GetTarget() { return mTargetID; }

	void AddExp();
	void LevelUp();
	void GradeUp();
	void SetType(ePetType type) { mpPetObjectInfo.Type = type; }
	void FriendlyUp( BYTE val );
	void SetLevel( BYTE level );

	ITEMBASE* GetWearedItem( POSTYPE pos );
	void SetWearedItem( POSTYPE pos, ITEMBASE* pitem );
	void SetAllWearedItem( ITEMBASE* info );

	void CalcStats();
	PlayerStat&	GetStat() { return mPetStat; }
	PlayerStat& GetItemOptionStat() { return mItemOptionStat; }
	void SetPetObjectState(ePetState state) { mpPetObjectInfo.State = state; }
	void FriendlyProcess();

	void DBInfoUpdate();
	void SetGod(BYTE byGod)	{ m_byGod = byGod ; }
	BYTE IsGod() const { return m_byGod ; }
	BOOL IsHaveSkill( DWORD skillIndex ) const;
	void ProcessTimeCheckItem( BOOL bDBUpdate , DWORD dwElapsedMili );
	// 091214 ONS 펫 맵이동시 소환/봉인 관련 메세지를 출력하지 않도록 처리.
	void SetSummonFlag(BOOL bFlag) { m_bIsSummonFlag = bFlag; }
	BOOL GetSummonFlag() const { return m_bIsSummonFlag; }
	void SetOwnerIndex(DWORD objectIndex) { mOwnedObjectIndex = objectIndex; }

	float	GetPhysicAttackMax()	{	return m_fPhysicAttackMax; }
	float	GetPhysicAttackMin()	{	return m_fPhysicAttackMin; }
	float	GetMagicAttackMax()		{	return m_fMagicAttackMax; }
	float	GetMagicAttackMin()		{	return m_fMagicAttackMin; }

	float	CalcPhysicAttack( float fAttackPoint );
	float	CalcMagicAttack( float fAttackPoint );
	virtual DWORD GetAggroNum() { return mAggroObjectContainer.size(); }
	virtual void ProceedToTrigger();
};

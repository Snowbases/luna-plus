#pragma once

#include "GridUnit.h"

class CBattle;
class cBuffSkillInfo;
// 080616 LUJ, 이벤트 발생 시 버프 중인 스킬이 액션을 취하기 위해 선언
class CEvent;
class cBuffSkillObject;

enum EObjectKind
{
	// 080616 LUJ, 초기화용 값 추가
	eObjectKind_None,
	eObjectKind_Player = 1,
	eObjectKind_Npc		=	2,
	eObjectKind_Item	=	4,
	eObjectKind_SkillObject=16,
	eObjectKind_Monster	=	32,
	eObjectKind_BossMonster=33,
	eObjectKind_SpecialMonster=34,
	
	// 필드보스 - 05.12 이영준
	eObjectKind_FieldBossMonster=35,
	eObjectKind_FieldSubMonster=36,
	eObjectKind_ToghterPlayMonster=37,
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.23
	eObjectKind_ChallengeZoneMonster = 38,
	// E 데이트 존 추가 added by hseos 2007.11.23
	// 080616 LUJ, 함정 추가
	eObjectKind_Trap = 39,
	// 090316 LUJ, 탈것
	eObjectKind_Vehicle		= 40,
	eObjectKind_MapObject	= 64,
	eObjectKind_Pet			= 128,
};

class CObject : public CGridUnit
{
	BOOL m_bInited;
	friend class CObjectFactory;
	friend class CCharMove;
	friend class CObjectStateManager;

	
protected:
	CObject();
	virtual ~CObject();

	DWORD m_AgentNum;
	EObjectKind m_ObjectKind;
	BASEOBJECT_INFO m_BaseObjectInfo;
	MOVE_INFO m_MoveInfo;
	STATE_INFO	m_ObjectState;

	RECOVER_TIME m_LifeRecoverTime;
	YYRECOVER_TIME m_YYLifeRecoverTime;

	// 100624 ONS HP업데이트를 위한 큐를 정의한다.
	std::queue< YYRECOVER_TIME >	m_YYLifeRecoverTimeQueue;
	// 100729 ONS 데미지를 HP로 변환할경우 즉시 적용한다.
	DWORD			m_LifeRecoverDirectlyAmount;

	AbnormalStatus mAbnormalStatus;

	virtual void DoDamage(CObject*,RESULTINFO*,DWORD) {}
	virtual void DoManaDamage(CObject*,RESULTINFO*,DWORD) {}
	void RemoveSummonedAll();
	friend class CCharacterCalcManager;

	CYHHashTable< cBuffSkillObject > m_BuffSkillList;
	Status				mBuffStatus;
	Status				mRateBuffStatus;
	DWORD				mCurrentSkillID;
	// 웅주, 트리거에서 별칭 지정위해 사용함
	DWORD				mAlias;
	DWORD				mGroupAlias;
	WORD				mEffectMoveCount;
	// 091113 LUJ, 다음 트리거 메시지를 보낼 시각
	DWORD mNextCheckedTick;
	BOOL mDieFlag;

	// 090216 LUJ, 컨테이너의 목적에 맞게 이름 변경
	// 100309 ShinJS --- 이벤트 스킬 소지여부를 판단하기 위한 변수 추가
	typedef std::list< std::pair< DWORD, eStatusKind > > EventSkillObjectIndexList;
	EventSkillObjectIndexList mEventSkillObjectIndexList;
	typedef std::map< eStatusKind, int > EventSkillKindCountMap;
	EventSkillKindCountMap m_pEventSkillKindCountMap;

	typedef DWORD SkillIndex;
	typedef DWORD ObjectIndex;
	typedef std::set< ObjectIndex > ObjectIndexContainer;
	typedef std::map< SkillIndex, ObjectIndexContainer > SummonObjectContainer;
	SummonObjectContainer mSummonObjectContainer;

	DWORD m_dwShockedTime;

public:
	DWORD CurCastingSkillID;

	const STATE_INFO& GetStateInfo() const { return m_ObjectState; }
	virtual void SetState(EObjectState);
	virtual CObject* GetTObject() const { return 0; }
	void AddSummoned(const ACTIVE_SKILL_INFO&, DWORD objectIndex);
	void RemoveSummoned(const ACTIVE_SKILL_INFO&);
	void RemoveSummonedOldest(const ACTIVE_SKILL_INFO&);
	int GetSummondSize(const ACTIVE_SKILL_INFO&);
	virtual DWORD GetOwnerIndex() const { return 0; }
	BYTE GetBattleTeam() const { return m_BaseObjectInfo.BattleTeam; }
	void SetBattleTeam( DWORD BattleTeam ) { m_BaseObjectInfo.BattleTeam = (BYTE)BattleTeam; }
	DWORD GetBattleID()	const {	return m_BaseObjectInfo.BattleID; }
	void SetBattleID(DWORD BattleID) { m_BaseObjectInfo.BattleID = BattleID; }
	CBattle* GetBattle();
	void SetBattle( DWORD BattleID, BYTE Team, DWORD Param=0 );
	
	virtual void SendMsg(MSGBASE* pMsg,int MsgLen);
	inline DWORD GetAgentNum() const { return m_AgentNum; }
	virtual float GetBonusRange() const { return 0; }
	virtual BOOL Init(EObjectKind kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo);
	virtual void Release();

	inline EObjectKind GetObjectKind() const { return m_ObjectKind;	}
	inline void SetObjectKind(EObjectKind kind)	{ m_ObjectKind = kind; }
	inline LPTSTR GetObjectName() { return m_BaseObjectInfo.ObjectName; }
	inline EObjectState GetState() const {return m_BaseObjectInfo.ObjectState; }
	void GetPosition(VECTOR3*);
	BASEMOVE_INFO* GetMoveInfo() { return &m_MoveInfo; }
	void SetInited() { m_bInited = TRUE; }
	void SetNotInited() { m_bInited = FALSE; }
	BOOL GetInited() const { return m_bInited; }
	inline DWORD GetID() const { return m_BaseObjectInfo.dwObjectID; }
	inline DWORD GetUserID() const { return m_BaseObjectInfo.dwUserID; }

	void GetBaseObjectInfo(BASEOBJECT_INFO* pRtInfo);
	virtual void GetSendMoveInfo(SEND_MOVEINFO* pRtInfo,CAddableInfoList* pAddInfoList);
	// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
	virtual DWORD SetAddMsg(DWORD receivedObjectIndex, BOOL isLogin, MSGBASE*& message) { return 0; }
	virtual DWORD SetRemoveMsg(DWORD dwReceiverID, MSGBASE*&);
	// 플레이어는 반지름이 지정되어 있지 않다. 아래 수치면 무난하다.
	virtual float GetRadius() const { return 100.0f; }
	virtual void OnStartObjectState(EObjectState, DWORD dwParam) {}
	virtual void OnEndObjectState(EObjectState) {}
	virtual void StateProcess() {}
	virtual void ProceedToTrigger() {}
	virtual int GetObjectTileSize();
	
	//////////////////////////////////////////////////////////////////////////
	// 데미지 죽음 관련
	void Die(CObject* pAttacker);
	virtual void DoDie(CObject* pAttacker)	{}
	virtual void CalcRealDamage(CObject* pOperator,WORD PhyDamage,WORD AttrDamage,RESULTINFO* pDamageInfo);
	virtual DWORD Damage(CObject* pAttacker,RESULTINFO*);
	virtual DWORD ManaDamage(CObject* pAttacker,RESULTINFO*);
	virtual LEVELTYPE GetLevel() { return 0; };
	virtual void SetLevel(LEVELTYPE level) {};
	virtual DWORD GetLife() { return 0; }
	virtual void SetLife(DWORD Life,BOOL bSendMsg = TRUE) {};
	virtual DWORD GetMana() { return 0; }
	virtual void SetMana(DWORD val,BOOL bSendMsg = TRUE) {};
	virtual DWORD GetRecoverLife() { return 0; }
	virtual DWORD GetRecoverMana() { return 0; }
	DWORD GetMaxLife() { return DoGetMaxLife(); }
	DWORD GetMaxMana() { return DoGetMaxMana(); }
	DWORD GetPhyDefense() { return DoGetPhyDefense(); }
	DWORD GetPhyAttackPowerMin() { return DoGetPhyAttackPowerMin(); }
	DWORD GetPhyAttackPowerMax() { return DoGetPhyAttackPowerMax(); }
	DWORD GetMagicAttackPower() { return DoGetMagicAttackPower(); }
	DWORD GetMagicDefense() { return DoGetMagicDefense(); }
	DWORD GetShieldDefense() { return DoGetShieldDefense(); }
	DWORD GetCritical() { return DoGetCritical(); }
	DWORD GetAccuracy() { return DoGetAccuracy(); }
	DWORD GetAvoid() { return DoGetAvoid(); }

	virtual DWORD DoGetMaxLife()	{return 0;}
	virtual DWORD DoGetMaxMana()	{return 0;}
	virtual DWORD DoGetPhyDefense()	{return 0;}
	virtual DWORD DoGetPhyAttackPowerMin()	{return 0;}
	virtual DWORD DoGetPhyAttackPowerMax()	{return 0;}
	virtual DWORD DoGetMagicAttackPower()	{return 0;}
	virtual DWORD DoGetMagicDefense()	{return 0;}
	virtual DWORD DoGetShieldDefense()	{return 0;}
	virtual DWORD DoGetCritical()	{return 0;}
	virtual DWORD DoGetAccuracy()	{return 0;}
	virtual DWORD DoGetAvoid()	{return 0;}


	float GetMoveSpeed() { return DoGetMoveSpeed(); }
	float GetUngiPlusRate() { return DoGetUngiPlusRate(); }
	float GetAddAttackRange() { return DoGetAddAttackRange(); }
	float GetDodgeRate() { return DoGetDodgeRate(); }
	
	virtual float DoGetMoveSpeed()	{return 0;}
	virtual float DoGetUngiPlusRate()	{return 0;}
	virtual float DoGetAddAttackRange()	{return 0;}
	virtual float DoGetDodgeRate()	{return 0;}
	virtual float GetAttDefense(WORD) { return 0; }
	virtual float DoGetAttDefense(WORD)	{return 0;}
	virtual void SetMaxLife(WORD life) {};
	virtual void SetMaxMana(WORD val) {};
	void AddLife(DWORD val, DWORD* realAddLife,BOOL bSendMsg = TRUE);
	void ReduceLife(DWORD val);
	void AddMana(DWORD val,DWORD* realAddMana);
	void ReduceMana(DWORD val);
	
	void CheckLifeValid();
	void CheckManaValid();

	virtual DWORD GetGravity() { return 0; }
	CYHHashTable< cBuffSkillObject >& GetBuffList() { return m_BuffSkillList; }
	AbnormalStatus*	GetAbnormalStatus() { return &mAbnormalStatus; }

	Status*	GetBuffStatus() { return &mBuffStatus; }
	Status*	GetRateBuffStatus() { return &mRateBuffStatus; }
	// 090204 LUJ, 버프가 유지되도록 하는 검사 값을 감소시킨다
	virtual void RemoveBuffCount( eBuffSkillCountType, int count );
	// 090204 LUJ, 파생 객체에서 오버라이드할 수 있도록 가상 함수로 만든다
	virtual void AddSpecialSkill( cBuffSkillInfo* ) {}
	virtual void RemoveSpecialSkill( cBuffSkillInfo* ) {}
	void	EndSkill();
	void	EndAllSkill();
	inline void SetCurrentSkill(DWORD idx) { mCurrentSkillID = idx; }
	inline DWORD GetCurrentSkill() const { return mCurrentSkillID; }
	void EndBuffSkillByStatus(WORD Status);
	virtual	void SetObjectBattleState(eObjectBattleState state ) { m_BaseObjectInfo.ObjectBattleState = state; }
	eObjectBattleState GetObjectBattleState() const { return m_BaseObjectInfo.ObjectBattleState; }
	void SetEffectMove() { ++mEffectMoveCount; }
	BOOL IsEffectMove();
	virtual DWORD GetAggroNum() { return 0; }
	void SetAlias(DWORD alias) { mAlias = alias; }
	DWORD GetAlias() const { return mAlias; }
	void SetGroupAlias(DWORD alias) { mGroupAlias = alias; }
	DWORD GetGroupAlias() const { return mGroupAlias; }
	virtual void Execute(const CEvent&);
	void AddEventSkill(cBuffSkillObject&);
	BOOL HasEventSkill( eStatusKind status );
	BOOL GetDieFlag() const { return mDieFlag; }
	void SetShockTime( DWORD dwTime );
	BOOL IsShocked();
	// 100624 ONS 큐에 정보를 저장 / HP업데이트처리 추가
	virtual void AddLifeRecoverTime( const YYRECOVER_TIME& recoverTime );
	virtual void UpdateLife();
	// 100729 ONS 데미지를 HP로 변환할경우 즉시 적용한다.
	virtual void SetLifeRecoverDirectly( DWORD recoverUnitAmout ) { m_LifeRecoverDirectlyAmount = recoverUnitAmout; }
};
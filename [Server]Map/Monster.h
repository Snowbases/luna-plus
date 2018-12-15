#pragma once

#include "Object.h"

class cDelay;
class CPlayer;
class CDistributer;

namespace FiniteStateMachine { class CMachine; }

class CMonster : public CObject
{
protected:
	DWORD m_DropItemId;
	DWORD m_dwDropItemRatio;
	DWORD m_SubID;
	MONSTER_TOTALINFO m_MonsterInfo;
	std::auto_ptr< CDistributer > m_Distributer;
	monster_stats m_mon_stats;
	BOOL m_bEventMob;
	cDelay* mpBattleStateDelay;
	DWORD m_DieTime;
	DWORD mKillerObjectIndex;
	VECTOR3 mDomainPosition;
	BOOL m_bNoCheckCollision;
	CObject* m_pTObject;
	BOOL m_bForeAttFlag;
	const std::auto_ptr< FiniteStateMachine::CMachine > mFiniteStateMachine;

	typedef DWORD ObjectIndex;
	typedef int AggroPoint;
	typedef std::map< ObjectIndex, AggroPoint > AggroContainer;
	AggroContainer mAggroContainer;

	BOOL m_bIsFixedPosition;

public:
	StateParameter mStateParamter;
	static void InitAggroMemoryPool();
	CMonster();
	virtual ~CMonster();
	virtual BOOL Init(EObjectKind kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo);
	virtual void InitMonster(MONSTER_TOTALINFO*);
	void RemoveFromAggro( DWORD dwObjectID );
	void RemoveAllAggro();
	virtual DWORD GetAggroNum() const { return mAggroContainer.size(); }
	void AddAggro(ObjectIndex, AggroPoint, DWORD skillIndex);
	void AddToAggroed(ObjectIndex);
	void ThinkAggro();
	virtual DWORD GetOwnerIndex() const { return m_MonsterInfo.OwnedObjectIndex; }
	virtual DWORD Damage(CObject*, RESULTINFO*);
	void SetNoCheckCollision( BOOL bNoCheck ) { m_bNoCheckCollision = bNoCheck; }
	BOOL IsNoCheckCollision() { return m_bNoCheckCollision; }
	void SetEventMob(BOOL bEventMob) { m_bEventMob = bEventMob; }
	void SetDropItemID( DWORD DropItemID, DWORD dwDropRatio = 100 )	{ m_DropItemId = DropItemID; m_dwDropItemRatio = dwDropRatio; }
	virtual void Release();
	inline MONSTER_TOTALINFO& GetMonsterTotalInfo() { return m_MonsterInfo; }
	virtual void GetSendMoveInfo(SEND_MOVEINFO* pRtInfo,CAddableInfoList* pAddInfoList);
	const BASE_MONSTER_LIST& GetSMonsterList() const;
	inline WORD GetMonsterKind() const { return m_MonsterInfo.MonsterKind; }
	void AddDamageObject(CPlayer* pPlayer, DWORD damage, DWORD plusdamage);
	void Drop();
	virtual DWORD SetAddMsg(DWORD dwReceiverID, BOOL bLogin, MSGBASE*&);	
	void SetSubID(DWORD id) { m_SubID = id;	}
	virtual BOOL SetTObject(CObject*);
	virtual CObject* GetTObject() const { return m_pTObject; }
	BOOL IsInvalidTarget(CObject&) const;
	virtual DWORD GetGravity();
	void MoveStop();
	virtual void OnStop();
	virtual void OnMove(VECTOR3 * pPos);
	virtual void DoStand();
	virtual void DoRest(BOOL bStart);
	virtual BOOL DoAttack( DWORD attackNum );
	virtual CObject* DoSearch();
	virtual BOOL DoWalkAround();
	virtual BOOL DoPursuit();
	virtual CObject* OnCollisionObject();
	virtual CMonster* DoFriendSearch(DWORD Range);
	virtual WORD DoFriendGroupSearch(DWORD* pFriendsList);
	void AddChat(LPCTSTR);
	void AddSpeech(DWORD SpeechType, DWORD SpeechIdx);
	monster_stats* GetMonsterStats() { return &m_mon_stats; }
	virtual WORD GetMonsterGroupNum() const { return WORD(m_MonsterInfo.Group); }		
	virtual void OnStartObjectState(EObjectState, DWORD dwParam);
	virtual void OnEndObjectState(EObjectState);
	virtual float GetRadius() const;	
	virtual void StateProcess();
	int GetObjectTileSize();	
	void DistributePerDamage();
	void DistributeItemPerDamage();
	void DistributeDamageInit();
	void DistributeDeleteDamagedPlayer(DWORD CharacterID);
	virtual void DoDie(CObject* pAttacker);
	virtual LEVELTYPE GetLevel();
	virtual void SetLevel(LEVELTYPE level) {}
	virtual DWORD GetLife() { return m_MonsterInfo.Life; }
	virtual void SetLife(DWORD Life,BOOL bSendMsg = TRUE);	
	virtual DWORD DoGetMaxLife();
	virtual void SetMaxLife(DWORD life)	{}
	virtual DWORD GetMana() { return 0; }
	virtual void SetMana(DWORD val,BOOL bSendMsg = TRUE) {}
	virtual DWORD DoGetMaxMana(){ return 0; }
	virtual void SetMaxMana(DWORD val) {}
	virtual DWORD DoGetPhyDefense() { return m_mon_stats.PhysicalDefense; }
	virtual DWORD DoGetPhyAttackPowerMin();
	virtual DWORD DoGetPhyAttackPowerMax();
	virtual float DoGetMoveSpeed();
	virtual	void SetObjectBattleState(eObjectBattleState);
	void SetForeAttack(BOOL bForeAttack) { m_bForeAttFlag = bForeAttack;}
	BOOL IsForeAttack() const {return m_bForeAttFlag;}
	void SetDieTime(DWORD time);
	virtual void Process();
	virtual void ProceedToTrigger();
	FiniteStateMachine::CMachine& GetFiniteStateMachine() { return *(mFiniteStateMachine.get()); }
	void SetDomainPosition( VECTOR3 pos ) { mDomainPosition = pos; }
	void SetFixedPosition( BOOL val ) { m_bIsFixedPosition = val; }

protected:
	void RequestHelp(const RESULTINFO&);
	virtual void DoDamage(CObject* pAttacker,RESULTINFO* pDamageInfo,DWORD beforeLife);
};
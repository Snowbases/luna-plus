// Object.h: interface for the CObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECT_H__A8AFB488_5BB5_45E5_8482_EE4EE2A55DDD__INCLUDED_)
#define AFX_OBJECT_H__A8AFB488_5BB5_45E5_8482_EE4EE2A55DDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Engine/EngineObject.h"
#include "./Engine/EngineEffect.h"
#include "Angle.h"
#include "EFFECT/EffectManager.h"
#include "../[cc]skill/client/delay/timedelay.h"

class CMOTIONDESC;
class cSkillObject;
class CObjectBalloon;
class cBuffSkillInfo;

enum EObjectKind
{
	// 080616 LUJ, 초기화용 값 추가
	eObjectKind_None,
	eObjectKind_Player = 1,
	eObjectKind_Npc		=	2,
	eObjectKind_Item	=	4,
	eObjectKind_Tactic	=	8,
	eObjectKind_SkillObject=16,
	eObjectKind_Monster	=	32,
	eObjectKind_BossMonster=33,
	eObjectKind_SpecialMonster=34,
	// 필드보스 - 05.12 이영준
	eObjectKind_FieldBossMonster=35,
	eObjectKind_FieldSubMonster=36,
	eObjectKind_ToghterPlayMonster=37,
	// 080616 LUJ, 함정 추가
	eObjectKind_Trap = 39,
	// 090422 ShinJS --- 탈것 추가(생성시 Monster로 생성, ObjectKind로 탈것 구분하기때문에 리소스와 서버의 eObjectKind_Vehicle를 맞추어야함)
	eObjectKind_Vehicle = 40,
	eObjectKind_Pet = 64,
	eObjectKind_MapObject = 128,
	// S 농장시스템 추가 added by hseos 2007.05.07
	eObjectKind_FarmObj= 130,
	// E 농장시스템 추가 added by hseos 2007.05.07

	// 090527 pdy 하우징
	eObjectKind_Furniture = 131,
};

struct ROTATEINFO
{
	BOOL bRotating;
	CAngle Angle;
	DWORD Rotate_StartTime;
	float EstimatedRotateTime;
};

#define NAMECOLOR_DEFAULT	RGB_HALF( 240, 240, 240 ) //KES 색수정 조금 밝게  //RGB_HALF( 230, 230, 230 )
#define NAMECOLOR_SELECTED	RGB_HALF( 255, 255, 0 )
#define NAMECOLOR_MONSTER	NAMECOLOR_DEFAULT
#define NAMECOLOR_PLAYER	NAMECOLOR_DEFAULT
#define NAMECOLOR_NPC		RGB_HALF( 255, 214, 0 )

#define NAMECOLOR_PARTY		RGB_HALF( 157, 204, 58 )
#define NAMECOLOR_MUNPA		RGB_HALF( 126, 156, 180 )

#define NAMECOLOR_PKMODE	RGB_HALF( 255, 0, 0 )
#define NAMECOLOR_WANTED	RGB_HALF( 234, 0, 255 )

#define NAMECOLOR_GM		RGB_HALF( 28, 233, 151 )


struct OBJECTEFFECTDESC
{
	OBJECTEFFECTDESC() { SetDesc(0); }
	OBJECTEFFECTDESC(DWORD EffectNum,DWORD dwFlag = EFFECT_FLAG_NORMAL,VECTOR3* pPos = 0)
	{	SetDesc(EffectNum,dwFlag,pPos);	}
	DWORD Effect;
	DWORD Flag;
	VECTOR3 Position;
	void SetDesc(DWORD EffectNum,DWORD dwFlag = EFFECT_FLAG_NORMAL,VECTOR3* pPos = 0)
	{
		Effect = EffectNum;
		Flag = dwFlag;
		if(pPos)
			Position = *pPos;
		else
			Position.x = Position.y = Position.z = 0;
	}
};


class CObject : public CObjectBase
{
	int m_bMoveSkipCount;
	CYHHashTable<void> m_StateEffectList;
	BOOL m_bIsYMoving;

protected:
	CObject();
	virtual ~CObject();

	DWORD m_DiedTime;
	CEngineEffect m_ShadowObj;
	BOOL m_bInited;
	BOOL m_bSelected;
	EObjectKind m_ObjectKind;
	BASEOBJECT_INFO m_BaseObjectInfo;
	MOVE_INFO m_MoveInfo;
	ROTATEINFO m_RotateInfo;

	cTimeDelay	mBattleStateCheckDelay;
	
	STATE_INFO	m_ObjectState;

	CMOTIONDESC	* m_pMotionDESC;			// 모션 정보  : taiyo
	CEngineObject m_EngineObject;
	
	BOOL m_bDieFlag;
	
	// overInfo 관련 --------------------------------------------------------------
	CObjectBalloon * m_pObjectBalloon;
	BYTE m_bObjectBalloonBits;

	DWORD m_dwShockedTime;

	typedef std::list< SKILLOBJECT_INFO* >	BuffSkillList;
	BuffSkillList		m_BuffSkillList;

	friend class CAppearanceManager;
	friend class CMoveManager;
	friend class CMHCamera;
	friend class CObjectStateManager;
	friend class CObjectActionManager;
	friend class CMotionManager;


	virtual BOOL Init(EObjectKind kind,BASEOBJECT_INFO* pBaseObjectInfo);
	void InitMove(BASEMOVE_INFO* pMoveInfo);
	virtual void Release();
	int	GetMotionIDX(int mainMotion, int subMotion = 0);
public:
	// overInfo 관련 --------------------------------------------------------------
	void InitObjectBalloon(BYTE bitFlag);

	// 070121 LYW --- Add function to return balloon.
	CObjectBalloon* GetObjectBalloon(){ return m_pObjectBalloon ; }
	void SetOverInfoOption( DWORD dwOption );
	void SetGuildMark();
	void SetNickName();
	void SetFamilyMark();
	void ShowObjectName( BOOL bShow, DWORD dwColor = NAMECOLOR_DEFAULT );
	void ShowChatBalloon( BOOL bShow, char* chatMsg, DWORD dwColor = RGB_HALF(70,70,70), DWORD dwAliveTime = 5000 );

	//090116 pdy 클라이언트 최적화작업 ( Hide 노점Title)
	void HideStreetStallTitle( BOOL bHide );
	// overInfo 관련 --------------------------------------------------------------

	//SW050913 수정
	void AddObjectEffect(DWORD KeyValue,OBJECTEFFECTDESC* EffectNumArray,WORD NumOfEffect,CObject* pSkillOperator=NULL);
	void RemoveObjectEffect(DWORD KeyValue);
	void RemoveAllObjectEffect();
	
	BYTE GetBattleTeam()						{	return m_BaseObjectInfo.BattleTeam;	}
	void SetBattleTeam( DWORD BattleTeam )		{	m_BaseObjectInfo.BattleTeam = (BYTE)BattleTeam;	}
	DWORD GetBattleID()				{	return m_BaseObjectInfo.BattleID;	}
	void SetBattle(DWORD BattleID,BYTE Team);
	inline EObjectKind GetObjectKind() const { return m_ObjectKind;	}
	inline void SetObjectKind(EObjectKind kind)	{ m_ObjectKind = kind; }
	virtual LPTSTR GetObjectName() { return m_BaseObjectInfo.ObjectName; }
	inline DWORD GetID() const { return m_BaseObjectInfo.dwObjectID; }

	void GetBaseObjectInfo(BASEOBJECT_INFO* pRtInfo);
	void GetBaseMoveInfo(BASEMOVE_INFO* pRtInfo);
	MOVE_INFO* GetBaseMoveInfo() {	return &m_MoveInfo;	}
	// 090206 LYW --- Object : Rotate 정보 반환함수 추가.
	ROTATEINFO*GetBaseRotateInfo() { return &m_RotateInfo ; }

	virtual void Process();

	inline CEngineObject* GetEngineObject()	{	return &m_EngineObject;		}


	friend class CObjectManager;

	inline BOOL IsInited()		{	return m_bInited;	}
	VECTOR3& GetCurPosition() { return m_MoveInfo.CurPosition; }
	float GetAngleDeg()	{ return m_RotateInfo.Angle.ToDeg(); }
	inline BOOL IsDied() const { return m_bDieFlag; }
	inline DWORD GetDiedTime() const { return m_DiedTime; }
	void SetDiedTime(DWORD time) { m_DiedTime = time; }
	void SetDieFlag();
	void SetFlag(BOOL val);
	virtual DWORD GetOwnerIndex() const { return 0; }
	virtual void SetMotionInState(EObjectState) {};
	virtual BOOL StartSocietyAct( int nStartMotion, int nIngMotion = -1, int nEndMotion = -1, BOOL bHideWeapon = FALSE )	{ return FALSE; }
	virtual BOOL EndSocietyAct()	{ return TRUE; }

	BuffSkillList&	GetBuffSkillList()	{ return	m_BuffSkillList; }
	
	void InsertBuffSkill( SKILLOBJECT_INFO* pBuffSkill );
	void RemoveBuffSkill( DWORD dwSkillObjectIdx );
	void ReleaseBuffSkillList();
	void UpdateTargetBuffSkill();

protected:	
	void SetState(EObjectState);
public:
	EObjectState GetState() const { return m_BaseObjectInfo.ObjectState; }
	DWORD GetStateStartTime() const { return m_ObjectState.State_Start_Time; }
	virtual void Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive);
	// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
	virtual void Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO&) {}
	virtual void ManaDamage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO&) {}
	virtual void Heal(CObject* pHealer,BYTE HealKind,DWORD HealVal) {}
	virtual void Recharge(CObject* pHealer,BYTE RechargeKind,DWORD RechargeVal) {}	
	virtual void Revive(VECTOR3* pRevivePos);
	virtual void OnStartObjectState(EObjectState) {}
	virtual void OnEndObjectState(EObjectState)	{}
	virtual void SetLife(DWORD life, BYTE type = 1);
	virtual DWORD GetLife() { return 0; }
	virtual float GetWeight() {	return 1.f;	}
	virtual float GetRadius() {	return 0.f;	}
	virtual DWORD GetMana()	{	return 0; }
	virtual void SetMana(DWORD val, BYTE type = 1) {}
//////////////////////////////////////////////////////////////////////////
// 
#define GET_STATUS(var_type,func)						\
var_type	func ## ()									\
{														\
	var_type Ori = Do ## func();						\
	return Ori;											\
};														\
virtual var_type	Do ## func ## ()	{	return 0; }		
//	
	GET_STATUS(DWORD,GetMaxLife);
	GET_STATUS(DWORD,GetMaxMana);
	GET_STATUS(DWORD,GetPhyDefense);
	GET_STATUS(DWORD,GetPhyAttackPowerMin);
	GET_STATUS(DWORD,GetPhyAttackPowerMax);
	GET_STATUS(DWORD,GetCritical);
	GET_STATUS(DWORD,GetDecisive);
	GET_STATUS(float,GetMoveSpeed);
	GET_STATUS(float,GetAddAttackRange);

	// 070411 LYW --- Object : Add physical part.
	GET_STATUS(DWORD,GetAttackRate);
	GET_STATUS(DWORD,GetDefenseRate);
	GET_STATUS(DWORD,GetMagAttackRate);
	GET_STATUS(DWORD,GetMagDefenseRate); 
	GET_STATUS(DWORD,GetAccuracyRate);
	GET_STATUS(DWORD,GetEvasionRate);
	GET_STATUS(DWORD,GetCriticalRate);		
	GET_STATUS(DWORD,GetMagicCriticalRate);		
	GET_STATUS(DWORD,GetMoveSpeedRate);		
	GET_STATUS(DWORD,GetAttackSpeedRate);		
	GET_STATUS(DWORD,GetCastingSpeedRate);		
	virtual float DoGetAttDefense(WORD Attrib)	{return 0;}
//////////////////////////////////////////////////////////////////////////
	
	virtual void SetPosition(VECTOR3* pPos);
	virtual void GetPosition(VECTOR3* pPos);
	virtual void SetAngle(float AngleRad);
	virtual float GetAngle();
	virtual DIRINDEX GetDirectionIndex();	


	virtual void ChangeMotion(int motion,BOOL bRoop, float rate = 1.0f);
	virtual void ChangeBaseMotion(int motion);
	void ChangeEmotion( int emotion );
	virtual float GetBonusRange() const { return 0; }
	void OnSelected();
	void OnDeselected();
	BOOL IsNpcMark(DWORD dwValue);
	virtual void SetObjectBattleState(eObjectBattleState);
	eObjectBattleState GetObjectBattleState() const { return m_BaseObjectInfo.ObjectBattleState; }

protected:
	BOOL m_bHideEffect ; 
public:
	void HideEffect( BOOL bHide ) ;
	BOOL IsHideEffect() { return m_bHideEffect ; }
	void SetShockTime( DWORD dwTime );
	BOOL IsShocked();
	void ApplyGravity( BOOL bApplyGravity, float fGravityAcceleration, DWORD dwValidTime );
};

#endif // !defined(AFX_OBJECT_H__A8AFB488_5BB5_45E5_8482_EE4EE2A55DDD__INCLUDED_)

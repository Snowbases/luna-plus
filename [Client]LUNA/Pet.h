#pragma once
#include "object.h"

class CPet :
	public CObject
{
protected:
	PET_INFO* mpPetInfo;
	DWORD mID;
	DWORD mMasterIdx;
	char mMasterName[MAX_NAME_LENGTH+1];
	DWORD mKind;
	BYTE mGrade;
	ePetState mState;
	BYTE mLevel;
	DWORD mWearedItem[ePetEquipSlot_Max];

	DWORD		mHP;
	DWORD		mMaxHP;
	
	friend class CAppearanceManager;

public:
	CPet(void);
	virtual ~CPet(void);
	LPTSTR GetObjectName();
	virtual void InitPet(PET_OBJECT_INFO*);
	virtual void Release();
	void Process();
	DWORD GetKind() const { return mKind; }
	BYTE GetGrade() const { return mGrade; }
	virtual void LevelUp() { mLevel++; }
	void GradeUp() { mGrade++; }
	void SetMaster(DWORD master) { mMasterIdx = master; }
	CPlayer* GetMaster();
	char* GetMasterName() { return mMasterName; }
	virtual DWORD GetOwnerIndex() const { return mMasterIdx; }
	virtual float DoGetMoveSpeed();
	virtual void SetMotionInState(EObjectState);
	virtual void SetLevel( BYTE level ) { mLevel = level; }
	BYTE	GetLevel() { return mLevel; }
	virtual void SetWearedItem( ITEMBASE* pItemInfo );
	virtual void SetWearedItem( WORD pos, DWORD idx ) { mWearedItem[ pos ] = idx; }

	virtual void	Die( CObject* pAttacker, BOOL bFatalDamage, BOOL bCritical, BOOL bDecisive );
	// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
	virtual void	Damage( CObject* pAttacker, BYTE DamageKind, DWORD Damage, const RESULTINFO&);
	virtual void	Heal( CObject* pHealer, BYTE HealKind, DWORD HealVal );
	virtual void	Recharge( CObject* pHealer, BYTE RechargeKind, DWORD RechargeVal );

	virtual void	SetMaxLife( DWORD maxlife ) { mMaxHP = maxlife; }
	virtual DWORD GetLife() { return mHP; }
	virtual void SetLife(DWORD life, BYTE type = 1);
	virtual DWORD DoGetMaxLife();
};

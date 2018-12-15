#pragma once

#include "GameResourceStruct.h"

class CPlayer;
class CPet;

#define PETMGR USINGTON(CPetManager)

class CPetManager
{
	BYTE	mMaxGrade;
	BYTE	mMaxLevel;
	BYTE*	mGradeUpLevel;
	BYTE*	mSummonPlayerLevel;
	WORD*	mExp;
	BYTE	mExpPenaltyLevel;
	BYTE	mShopBaseSkillSlot;
	BYTE	mMinBaseSkillSlot;
	BYTE	mMaxBaseSkillSlot;
	BYTE*	mExtendedSkillSlot;
	WORD	mMaxDistance;
	WORD	mAreaDistance;
	DWORD	mDistanceCheckTime;
	WORD	mHatchEffect;
	WORD	mSummonEffect;
	WORD	mSealEffect;
	WORD	mAppearEffect;
	WORD	mDisappearEffect;
	WORD	mLevelUpEffect;
	WORD	mGradeUpEffect;
	DWORD	mHatchDelay;
	DWORD	mSummonDelay;
	DWORD	mSealDelay;
	DWORD	mSummonPenaltyDelay;
	DWORD	mResummonPenaltyDelay;
	DWORD	mClosedSkillIndex;
	DWORD	mRangeSkillIndex;
	WORD	mNameHight;
	
	BYTE	mRunAnimation;
	BYTE	mWalkAnimation;
	BYTE	mIdleAnimation;
	BYTE	mDamageAnimation;

	CYHHashTable<PET_INFO>				mPetInfoTable;
	CYHHashTable<PET_OBJECT_INFO>		mPetObjectInfoTable;

	CYHHashTable<PET_STATUS_INFO>		mPetStatusInfoTable[ ePetType_Max ];
	CYHHashTable<PET_FRIENDLY_PENALTY>	mPetFriendlyPenaltyTable;
	CYHHashTable<cPtrList>				mPetSpeechInfoTable;
	CYHHashTable<cPtrList>				mPetEmoticonInfoTable;
	CYHHashTable<PET_FRIENDLY_STATE>	mFriendlyStateTable;

	DWORD	mLastSummonTime;

public:
	CPetManager(void);
	virtual ~CPetManager(void);
	
	void LoadPetInfo();

	void NetworkMsgParse(BYTE Protocol,void* pMsg);

	PET_INFO* GetPetInfo( DWORD kind ) { return mPetInfoTable.GetData( kind ); }
	PET_STATUS_INFO* GetPetStatusInfo(ePetType type, BYTE level)	{ return mPetStatusInfoTable[ type ].GetData( level ); }
	DWORD GetDistanceCheckTime() const { return mDistanceCheckTime; }
	WORD GetMaxDistance() const { return mMaxDistance; }
	WORD GetAreaDistance() const { return mAreaDistance; }
	WORD GetExp(BYTE grade) const { return mExp[grade]; }
	BYTE GetSummonPlayerLevel(BYTE grade) const { return mSummonPlayerLevel[grade]; }
	BYTE	GetRunAnimation() const { return mRunAnimation; }
	BYTE	GetWalkAnimation() const { return mWalkAnimation; }
	BYTE	GetIdleAnimation()	const { return mIdleAnimation; }
	BYTE	GetDamageAnimation() const { return mDamageAnimation; }
	DWORD	GetClosedSkillIndex() const { return mClosedSkillIndex; }
	DWORD	GetRangeSkillIndex() const { return mRangeSkillIndex; }	
	BYTE	GetGradeUpLevel( BYTE grade ) const { return grade >= mMaxGrade ? 0 : mGradeUpLevel[grade]; }
	float	GetFriendlyPenalty(BYTE friendy);
	PET_OBJECT_INFO* GetPetObjectInfo( DWORD ItemDBIdx ) { return mPetObjectInfoTable.GetData(ItemDBIdx); }
	DWORD	GetLastSummonTime() const { return mLastSummonTime; }
	void	SetLastSummonTime() { mLastSummonTime = gCurTime; }
	DWORD	GetSummonPenaltyDelay() const { return mSummonPenaltyDelay; }
	WORD	GetSummonEffect() const { return mSummonEffect; }
	WORD	GetSealEffect() const { return mSealEffect; }
	WORD	GetLevelUpEffect() const { return mLevelUpEffect; }
	WORD	GetGradeUpEffect() const { return mGradeUpEffect; }
	WORD	GetNameHeight() const { return mNameHight; }
	cPtrList* GetPetSpeechInfoTable( DWORD index ) { return mPetSpeechInfoTable.GetData( index ); }
	cPtrList* GetPetEmoticonInfoTable( DWORD index ) { return mPetEmoticonInfoTable.GetData( index ); }
	PET_FRIENDLY_STATE*	 GetFriendlyStateTable( BYTE friendly )	{ return mFriendlyStateTable.GetData( friendly ); }

	void	AddPetItemOptionStat( const ITEMBASE& petItemBase );
	void	RemovePetItemOptionStat( const ITEMBASE& petItemBase );
};

EXTERNGLOBALTON(CPetManager);

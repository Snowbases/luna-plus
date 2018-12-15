#pragma once

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
	DWORD	mHatchDelay;
	DWORD	mSummonDelay;
	DWORD	mSealDelay;
	DWORD	mSummonPenaltyDelay;
	DWORD	mResummonPenaltyDelay;
	DWORD	mClosedSkillIndex;
	DWORD	mRangeSkillIndex;
	DWORD	mFriendlyReduceTime;
	BYTE	mFriendlyReducePoint;

	CYHHashTable<PET_INFO>				mPetInfoTable;
	CYHHashTable<PET_STATUS_INFO>		mPetStatusInfoTable[ ePetType_Max ];
	CYHHashTable<PET_HP_MP_INFO>		mPetHPMPInfoTable;

	CYHHashTable<PET_FRIENDLY_PENALTY>	mPetFriendlyPenaltyTable;
	// 091214 ONS 펫 맵이동시 소환/봉인 관련 메세지를 출력하지 않도록 처리.
	std::set<DWORD>						m_setSummonPlayerIndex;

	typedef DWORD ItemDbIndex;
	typedef stdext::hash_map< ItemDbIndex, PET_OBJECT_INFO > PetInfoContainer;
	PetInfoContainer mPreloadedPetInfoContainer;
	typedef DWORD ObjectIndex;
	typedef stdext::hash_map< ItemDbIndex, ObjectIndex > PetIndexContainer;
	PetIndexContainer mPetIndexContainer;

public:
	CPetManager(void);
	virtual ~CPetManager(void);

	void LoadPetInfo();
	void UsePetItem(CPlayer*, DWORD ItemDBIdx);
	void NetworkMsgParse( BYTE Protocol, void* pMsg, DWORD dwLength );
	PET_INFO* GetInfo( DWORD kind ) { return mPetInfoTable.GetData( kind ); }
	PET_STATUS_INFO* GetPetStatusInfo(ePetType type, BYTE level) { return mPetStatusInfoTable[ type ].GetData( level ); }
	void SealPet(CPet*);
	void RemovePet(CPet*);
	void SummonPet(CPlayer*, DWORD ItemDBIdx, BOOL bIsSummoned = TRUE);
	const PET_OBJECT_INFO& GetObjectInfo(DWORD itemDbIndex);
	void Update(const PET_OBJECT_INFO&);
	void AddObject(DWORD itemDbIndex, DWORD objectIndex);
	void RemoveObject(DWORD itemDbIndex);
	BOOL SetTarget( CPet* pPet, DWORD id );
	BOOL PetAttack( CPet* pPet );
	BOOL PetSkillUse( CPet* pPet, DWORD idx );
	BOOL PetRecall( CPet* pPet );

	BOOL HatchPet( CPlayer* pPlayer, const ITEMBASE* pItem );

	WORD GetMaxEXP( BYTE grade );
	BYTE GetGradeUpLevel( BYTE grade );
	BYTE GetMaxGrade() { return mMaxGrade; }
	BYTE GetMaxLevel() { return mMaxLevel; }
	WORD GetAreaDistance() { return mAreaDistance; }

	PET_HP_MP_INFO*	GetHPMPInfo( BYTE idx ) { return mPetHPMPInfoTable.GetData( idx ); }

	DWORD GetFriendlyReduceTime() { return mFriendlyReduceTime; }
	BYTE GetFriendlyReducePoint() { return mFriendlyReducePoint; }

	float GetFriendlyPenalty( CPet* pPet );
	BYTE GetExtendedSkillSlot( BYTE grade ) { return mExtendedSkillSlot[ grade ]; }
	BYTE GetExpPenaltyLevel() { return mExpPenaltyLevel; }
	// 090720 ONS 펫스킬아이템인지 여부를 판단한다.
	BOOL IsPetStaticSkill(PET_INFO* pPetInfo, WORD pos);

	// 091214 ONS 펫 맵이동시 소환/봉인 관련 메세지를 출력하지 않도록 처리.
	BOOL IsSummonPlayerIndex( DWORD dwIndex  );
	CPet* GetPet(DWORD itemDbIndex);

	void	AddPetItemOptionStat( const ITEMBASE& petItemBase, CPet* pPet );
	void	RemovePetItemOptionStat( const ITEMBASE& petItemBase, CPet* pPet );
};

EXTERNGLOBALTON(CPetManager);

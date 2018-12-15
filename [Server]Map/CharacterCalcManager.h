#pragma once


#define CHARCALCMGR CCharacterCalcManager::GetInstance()


class CPlayer;
class CPet;

struct SetScript;
struct ITEM_INFO;

struct JobSkillInfo
{
	WORD	wClassIndex;
	DWORD	dwSkillIndex;
	BYTE	byPlusLevel;
};

class CCharacterCalcManager  
{
	cPtrList	m_HPMPDeterminateList ;
	std::list<JobSkillInfo>	m_JobSkillList;
	//std::hash_map<WORD, JobSkillInfo> m_JobSkillMap;

public:
	static CCharacterCalcManager* GetInstance();
	CCharacterCalcManager();
	virtual ~CCharacterCalcManager();

	void CalcMaxLife( CPlayer* );
	void CalcMaxMana( CPlayer* );

	BOOL LoadHPMPDeterminateList();

	// 081022 KTH -- Load File Job Skill
	BOOL LoadJobSkillList();

	void Init_HPMP_Point(CPlayer*);
	BOOL Get_HPMP_Point(CPlayer*, BYTE* hpPoint, BYTE* mpPoint);

	cPtrList* GetHPMPDetermiateList() { return &m_HPMPDeterminateList ; }

	void CalcCharPhyAttack( CPlayer* );
	void CalcCharPhyDefense( CPlayer* );

	void CalcCharMagAttack( CPlayer* );
	void CalcCharMagDefense( CPlayer* );

	void CalcCharAccuracy( CPlayer* );

	void CalcCharEvasion( CPlayer* );

	void CalcCharCriticalRate( CPlayer* );
	void CalcCharCriticalDamage( CPlayer* );

	void CalcCharAddDamage( CPlayer* );
	void CalcCharReduceDamage( CPlayer* );
	
	// 080910 LUJ, 플레이어의 방패 방어력을 계산
	void CalcCharShieldDefense( CPlayer* );

	static void ProcessLife(CPlayer*);
	static void ProcessMana(CPlayer*);
	
	static void ProcessPetLife(CPet*);
	static void ProcessPetMana(CPet*);
	static void ProcessLifeBoss(CBossMonster*);
	static void General_LifeCount(CPlayer*, DWORD curTime, DWORD life);
	static void General_ManaCount(CPlayer*, DWORD curTime, DWORD life);
	static void General_PetLifeCount(CPet*, DWORD curTime, DWORD life);
	static void General_PetManaCount(CPet*, DWORD curTime, DWORD life);
	static void General_LifeCountBoss(CBossMonster * pBoss, DWORD curTime, DWORD life, DWORD maxlife);
	static void UpdateLife( CObject* );
	static void StartUpdateLife( CObject*, DWORD plusLife, BYTE recoverInterval=RECOVERINTERVALUNIT, DWORD recoverTime=RECOVERTIMEUNIT);

	static void UpdateMana(CPlayer*);
	static void StartUpdateMana(CPlayer*, DWORD plusMana, BYTE recoverInterval=RECOVERINTERVALUNIT, DWORD recoverTime=RECOVERTIMEUNIT);

	void Initialize(CPlayer*);
	void AddItem(CPlayer*, const ITEMBASE&);
	void RemoveItem(CPlayer*, const ITEMBASE&);
	void CalcCharStats(CPlayer*);
	void ArrangeCharLevelPoint(CPlayer*, WORD type);
	void AddPlayerJobSkill(CPlayer*);

private:
	void AddStat	( const ITEM_INFO&,		PlayerStat& );

	// 080319 LUJ, 추가 인챈트 수치를 부여함
	void AddStat	( const ITEM_INFO&,		const ITEM_OPTION&,	PlayerStat& );
	void AddStat	( const PlayerStat&,	PlayerStat& );
	void RemoveStat	( const ITEM_INFO&,		PlayerStat& );

	// 080319 LUJ, 추가 인챈트 수치를 제거함
	void RemoveStat	( const ITEM_INFO&,		const ITEM_OPTION&,	PlayerStat& );
	void RemoveStat	( const PlayerStat&,	PlayerStat& );

	void AddSetItemStats	( CPlayer*, const SetScript&, int setItemSize );
	void RemoveSetItemStats	( CPlayer*, const SetScript&, int setItemSize );

	void AddItemBaseStat	( const ITEM_INFO&,		const ITEM_OPTION&,	PlayerStat& );
	void RemoveItemBaseStat	( const ITEM_INFO&,		const ITEM_OPTION&,	PlayerStat& );

	void AddItemOptionStat	( const ITEM_INFO&,		const ITEM_OPTION&,	PlayerStat& );
	void RemoveItemOptionStat	( const ITEM_INFO&,		const ITEM_OPTION&,	PlayerStat& );

	// 100226 ShinJS --- 계열에 맞지 않은 장비를 착용했는지 검사(천/라이트/헤비)
	BOOL IsInvalidEquip( CPlayer* pPlayer ) const;
};

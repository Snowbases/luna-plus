#pragma once


#include "Object.h"
#include "ExchangeManager.h"
#include "ItemContainer.h"
#include "InventorySlot.h"
#include "StorageSlot.h"
#include "WearSlot.h"
#include "Purse.h"
#include "..\[CC]Quest\QuestDefines.h"
#include "QuestGroup.h"
#include "QuickSlot.h"

struct sGAMEOPTION
{
	BOOL bNoDeal;
	BOOL bNoParty;
	BOOL bNoShowdown;
};

struct stFARMPLAYERINFO																		// 플레이어 정보
{
	UINT				nCropPlantRetryTime;												// ..다시 농작물을 심는데 필요한 시간
	DWORD				nCropPlantRetryTimeTick;											// ..다시 농작물을 심는데 필요한 시간 틱
	UINT				nCropManureRetryTime;												// ..다시 농작물에 비료를 주는데 필요한 시간
	DWORD				nCropManureRetryTimeTick;											// ..다시 농작물에 비료를 주는데 필요한 시간 틱
	UINT				nAnimalFeedRetryTime;												// ..다시 가축에 사료를 주는데 필요한 시간
	DWORD				nAnimalFeedRetryTimeTick;											// ..다시 가축에 사료를 주는데 필요한 시간 틱
	UINT				nAnimalCleanRetryTime;												// ..다시 가축을 씻기는대 필요한 시간
	DWORD				nAnimalCleanRetryTimeTick;											// ..다시 가축을 씻기는대 필요한 시간 틱
};

class cDelay;
class cStreetStall;
class CMonster;
class CQuestBase;
class CQuestScript;
class cSkillObject;
class cSkillTree;
struct SetScript;
struct ITEMOBTAINARRAYINFO;

enum
{
	PLAYERINITSTATE_ONLY_ADDED			= 1 << 0,
	PLAYERINITSTATE_HERO_INFO			= 1 << 1,
	PLAYERINITSTATE_SKILL_INFO			= 1 << 2,
	PLAYERINITSTATE_ITEM_INFO			= 1 << 3,
	PLAYERINITSTATE_QUICK_INFO			= 1 << 4,
	PLAYERINITSTATE_STORAGE_INFO		= 1 << 5,
	PLAYERINITSTATE_QEUST_INFO			= 1 << 6,
	PLAYERINITSTATE_FARM_INFO			= 1 << 7,
	PLAYERINITSTATE_CHALLENGEZONE_INFO	= 1 << 8,
	PLAYERINITSTATE_OPTION_INFO			= 1 << 9,
};

enum
{
	MONEY_ADDITION = 0,
	MONEY_SUBTRACTION,
};

#define GETITEM_FLAG_INVENTORY	0x00000001
#define GETITEM_FLAG_WEAR		0x00000002

class CPlayer : public CObject  
{
private:
	DWORD m_UniqueIDinAgent;
	DWORD m_dwCurChannel;
	DWORD m_ChannelID;
	eUSERLEVEL m_dwUserLevel;
	DWORD m_ContinuePlayTime;
	int m_initState;
	CHARACTER_TOTALINFO m_HeroCharacterInfo;
	HERO_TOTALINFO m_HeroInfo;
	sGAMEOPTION	m_GameOption;
	cQuickSlot m_QuickSlot[8];
	BOOL m_God;
	ReviveFlag mReviveFlag;
	PlayerStat m_charStats;
	PlayerStat m_SetItemStats;
	PlayerStat m_itemBaseStats;
	PlayerStat m_itemOptionStats;
	Status mPassiveStatus;
	Status mRatePassiveStatus;
	DWORD mGravity;
	friend class CCharacterCalcManager;

private:
	/// 대전 관련 정보
	BOOL		m_bShowdown;
	DWORD		m_dwOpPlayerID;

	/// PK모드 시간 정보
	DWORD		m_dwPKModeStartTime;	//with gCurTime
	DWORD		m_dwPKContinueTime;

	//---KES PK 071202
	BOOL	m_bNoExpPenaltyByPK;

	// 080318 LUJ, PK에 의한 경험치 비손실 상태인지 반환
public:
	BOOL	IsNoExpPenaltyByPK() const	{ return m_bNoExpPenaltyByPK; }

private:
	void		LogOnRelease();
	void		AddBadFameReduceTime();
	/// 자신을 죽인 대상 정보
	WORD		m_MurdererKind;
	DWORD		m_MurdererIDX;
	BOOL		m_bPenaltyByDie;
	BOOL		m_bReadyToRevive; //죽은 상태에서만 씀
	CYHHashTable<CMonster> m_FollowMonsterList;
	
	/// 전투 능력 수치
	float		mAccuracy;
	float		mAvoid;
	float		mBlock;
	float		mPhysicAttackMax;
	float		mPhysicAttackMin;
	float		mMagicAttackMax;
	float		mMagicAttackMin;
	float		mPhysicDefense;
	float		mMagicDefense;
	float		mCriticalRate;
	float		mCriticalDamagePlus;
	float		mCriticalDamageRate;
	float		mMagicCriticalRate;
	float		mMagicCriticalDamagePlus;
	float		mMagicCriticalDamageRate;
	float		mLifeRecover;
	float		mManaRecover;
	DWORD		mShieldDefense;

	float		mAddDamage;
	float		mReduceDamage;
	float		mRateAddDamage;
	float		mRateReduceDamage;

	BOOL		m_bDieForGFW;
	DWORD 		mPetIndex;
	WORD		m_wGTReturnMapNum;
	int			m_ObserverBattleIdx;


	// 길드토너먼트
	DWORD		m_dwKillCountFromGT;	// 길드토너먼트에서 킬 횟수
	DWORD		m_dwKillPointFromGT;	// 길드토너먼트에서 획득한 킬포인트
	DWORD		m_dwRespawnTimeOnGTMAP;	// 길드토너먼트에서 죽은경우 부활대기시간
	DWORD		m_dwImmortalTimeOnGTMAP;// 길드토너먼트에서 부활한경우 무적시간
	VECTOR3		m_vStartPosOnGTMAP;		// 길드토너먼트맵에서의 시작위치

	/// 퀘스트 정보
	CQuestGroup	m_QuestGroup;
public:
	BOOL		m_bNeedRevive;

	DWORD		mWeaponEnchantLevel;
	DWORD		mPhysicDefenseEnchantLevel;
	DWORD		mMagicDefenseEnchantLevel;

	cDelay*		mpBattleStateDelay;
	/// 전투/행동 정보 관련 변수-------------------------------------------/
	///--------------------------------------------------------------------/


	///--------------------------------------------------------------------/
	/// 프로세스 관련 변수-------------------------------------------------/
private:
	/// 로그 프로세스 시간
	DWORD			m_dwProgressTime;

	/// 스피드핵 체크
	int				m_nHackCount;			//skill + move
	DWORD			m_dwHackStartTime;

	/// 회복프로세스 관련 정보
	RECOVER_TIME	m_ManaRecoverTime;
	YYRECOVER_TIME	m_YYManaRecoverTime;

	// 100624 ONS MP업데이트를 위한 큐를 정의한다.
	std::queue< YYRECOVER_TIME >	m_YYManaRecoverTimeQueue;

	DWORD			m_ManaRecoverDirectlyAmount;


	/// 종료 정보
	BOOL			m_bExit;	//종료플래그
	DWORD			m_dwExitStartTime;
	BOOL			m_bNormalExit;
	BOOL			m_bWaitExitPlayer;

	BOOL			mIsEmergency;
	MAPTYPE			mTargetMap;
	DWORD			mTargetPosX;
	DWORD			mTargetPosZ;

	/// 프로세스 관련 변수-------------------------------------------------/
	///--------------------------------------------------------------------/

	///--------------------------------------------------------------------/
	/// 아이템 정보 관련 변수----------------------------------------------/
	/// 아이템으로 인한 케릭터 수치 정보는 수치 정보 관련 변수에 분류
private:
	/// 각종 인벤토리
	CItemContainer						m_ItemContainer;
	CInventorySlot						m_InventorySlot;
	CWearSlot							m_WearSlot;
	CStorageSlot						m_StorageSlot;
	CItemSlot							m_ShopItemSlot;			// ItemMall용	
	CInventoryPurse						m_InventoryPurse;
	CPurse								m_StoragePurse;
	/// 인벤토리 관련 클래스
	friend class						CInventoryPurse;
	
	/// 초기화 관련 변수
	BOOL								m_bGotWarehouseItems;
	sEXCHANGECONTAINER					m_ExchangeContainer;

	cStreetStall* m_pGuetStall;
	char m_StreetStallTitle[MAX_STREETSTALL_TITLELEN+1];
	cSkillTree*	m_SkillTree;
	DWORD m_CurComboNum;
	WORD m_SkillFailCount;
	DATE_MATCHING_INFO m_DateMatching;
	stMONSTERMETER m_stMonstermeterInfo;
	char*									m_pcFamilyEmblem;
	stFARMPLAYERINFO						m_stFarmInfo;
	ObjectIndexContainer mAggroObjectContainer;
	UINT									m_nChallengeZoneEnterFreq;
	UINT									m_nChallengeZoneEnterBonusFreq;
	UINT									m_nChallengeZonePartnerID;
	UINT									m_nChallengeZoneSection;
	UINT									m_nChallengeZoneStartState;
	DWORD									m_nChallengeZoneStartTimeTick;
	UINT									m_nChallengeZoneMonsterNum;
	UINT									m_nChallengeZoneKillMonsterNum;
	BOOL									m_bChallengeZoneNeedSaveEnterFreq;
	BOOL									m_bChallengeZoneCreateMonRightNow;
	UINT									m_nChallengeZoneMonsterNumTillNow;
	DWORD									m_nChallengeZoneClearTime;
	// 091123 ONS 챌린지존 경험치 분배 처리 추가
	DWORD									m_nChallengeZoneExpRate;
	BYTE m_byHP_Point;
	BYTE m_byMP_Point;
	EXPTYPE	m_dweFamilyRewardExp;
	BYTE m_byCurFamilyMemCnt;
	CYHHashTable< CQuestBase > m_QuestList;

public:
	/// 생성자/소멸자
	CPlayer();
	virtual ~CPlayer();
	void SetReviveFlag(ReviveFlag flag) { mReviveFlag = flag; }
	virtual BOOL	Init( EObjectKind kind, DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo );
	virtual void	Release();
	void			InitBaseObjectInfo( BASEOBJECT_INFO* pBaseInfo );
	void			InitClearData();
	void			InitCharacterTotalInfo(CHARACTER_TOTALINFO*);
	void			InitHeroTotalInfo(HERO_TOTALINFO*);
	virtual void	SetInitedGrid();

	/// 유저 관리용
	/// Agent서버 정보
	void			SetUniqueIDinAgent( DWORD uniqueid ) { m_UniqueIDinAgent = uniqueid; }
	DWORD			GetUniqueIDinAgent() { return m_UniqueIDinAgent; }

	/// 채널 정보
	void			SetChannelID( DWORD id ) { m_ChannelID = id; }
	DWORD			GetChannelID() { return m_ChannelID; }

	void			SetCurChannel(DWORD dwChannel) {m_dwCurChannel = dwChannel;}
	DWORD			GetCurChannel() {return m_dwCurChannel;}
	void			SetUserLevel(eUSERLEVEL dwUserLevel) { m_dwUserLevel = dwUserLevel; }
	eUSERLEVEL		GetUserLevel() const { return m_dwUserLevel; }
	DWORD SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*&);
	void 			GetCharacterTotalInfo(CHARACTER_TOTALINFO* info) { *info = m_HeroCharacterInfo; }
	CHARACTER_TOTALINFO& GetCharacterTotalInfo() { return m_HeroCharacterInfo; }
	const CHARACTER_TOTALINFO& GetCharacterTotalInfo() const { return m_HeroCharacterInfo; }
	void			GetHeroTotalInfo(HERO_TOTALINFO* info) { *info = m_HeroInfo; }
	HERO_TOTALINFO*	GetHeroTotalInfo() { return &m_HeroInfo; }
	int				GetGender() { return m_HeroCharacterInfo.Gender; }

	/// 게임 옵션
	sGAMEOPTION*	GetGameOption() { return &m_GameOption; }
	void			SetGameOption( sGAMEOPTION* pGameOption ) { m_GameOption = *pGameOption; }

	/// 상태
	int				GetInitState(){ return m_initState; }
	void			SetInitState( int initstate, DWORD protocol );
	virtual void	OnEndObjectState(EObjectState);
	void			SetPeaceMode( BOOL bPeace ) { m_HeroCharacterInfo.bPeace = bPeace; }
	BOOL			GetPeaceMode() const { return m_HeroCharacterInfo.bPeace; }
	BOOL			IsVisible() const { return m_HeroCharacterInfo.bVisible; }
	void			SetVisible(BOOL val) { m_HeroCharacterInfo.bVisible = val; }
	void			SetStage(BYTE grade, BYTE index);
	BYTE			GetStage() const { return m_HeroCharacterInfo.JobGrade ; }

	BYTE			GetJop(BYTE idx) const { return m_HeroCharacterInfo.Job[idx]; }
	BYTE			GetRacial() const { return m_HeroCharacterInfo.Race; }
	BYTE			GetJopGrade() const { return m_HeroCharacterInfo.JobGrade; }
	WORD			GetJobCodeForGT();
	void			SetJob(BYTE jobGrade, BYTE jobIdx);

	WORD			GetPlayerLoginPoint() const { return m_HeroCharacterInfo.LoginPoint_Index; }
	WORD			GetLoginMapNum() const {return m_HeroCharacterInfo.LoginMapNum;}
	void			SetLoginPoint(WORD LoginPoint) { m_HeroCharacterInfo.LoginPoint_Index = LoginPoint; }
	void			InitMapChangePoint() { m_HeroCharacterInfo.MapChangePoint_Index = 0; }
	void			SetMapChangePoint(WORD point) { m_HeroCharacterInfo.MapChangePoint_Index = point; }	// YH
	WORD			GetMapChangePoint() { return m_HeroCharacterInfo.MapChangePoint_Index;	} //hs for party
	void			SendPlayerToMap(MAPTYPE, float xpos, float zpos) ;
	BOOL			AddQuick( BYTE sheet, WORD pos, SLOT_INFO* pSlot );
	SLOT_INFO*		GetQuick( BYTE sheet, WORD pos );

	void			SetGod( BOOL val ) { m_God = val; }
	BOOL			IsGod() const { return m_God; }
	LEVELTYPE		GetPlayerLevelUpPoint() const { return m_HeroInfo.LevelUpPoint; }
	void			SetPlayerLevelUpPoint(LEVELTYPE);
	void SetSkillPoint( DWORD point, BYTE byForced ) ;

	DWORD				GetSkillPoint();
	/// 레벨 관련
	LEVELTYPE			GetMaxLevel() { return m_HeroInfo.MaxLevel; }
	virtual				LEVELTYPE GetLevel();
	virtual void		SetLevel( LEVELTYPE level );

    /// 경험치
	EXPTYPE				GetPlayerExpPoint() { return m_HeroInfo.ExpPoint; }
	DOUBLE				GetPlayerTotalExpPoint();
	void				SetPlayerExpPoint(EXPTYPE);
	void				AddPlayerExpPoint(EXPTYPE);
	void				ReduceExpPoint( EXPTYPE minusExp, BYTE bLogType );
	FAMETYPE			GetBadFame() const { return m_HeroCharacterInfo.BadFame; }
	void				SetBadFame(FAMETYPE val) { m_HeroCharacterInfo.BadFame = val; }

	/// 기본 스텟 정보
	PlayerStat&	GetCharStats() { return m_charStats; }

	// 080313 LUJ, 세트 스탯 분리
	PlayerStat& GetSetItemStats() { return m_SetItemStats; };

	PlayerStat&	GetItemBaseStats() { return m_itemBaseStats; }
	PlayerStat&	GetItemOptionStats() { return m_itemOptionStats; }


	// 070410 LYW --- Player : Add functions to return hero info.
	DWORD				GetRealVitality()		{ return m_HeroInfo.Vit ; }
	DWORD				GetRealWisdom()			{ return m_HeroInfo.Wis ; }
	DWORD				GetRealStrength()		{ return m_HeroInfo.Str ; }
	DWORD				GetRealDexterity()		{ return m_HeroInfo.Dex ; }
	DWORD				GetRealIntelligence()	{ return m_HeroInfo.Int ; }

	DWORD				GetVitality();
	DWORD				GetWisdom();
	DWORD				GetStrength();
	DWORD				GetDexterity();
	DWORD				GetIntelligence();
	
	void				SetVitality( DWORD val );
	void				SetWisdom( DWORD val );
	void				SetStrength( DWORD val );
	void				SetDexterity( DWORD val );
	void				SetIntelligence( DWORD val );

	void				AddStrength( int val );
	void				AddWisdom( int val );
	void				AddDexterity( int val );
	void				AddVitality( int val );
	void				AddIntelligence( int val );

	/// 생명력/마나 정보
	BYTE				GetLifePercent();
	BYTE				GetManaPercent();
	void				SetMaxLife( DWORD val );
	void				SetMaxMana( DWORD val );

	// 080625 LYW --- Player : 생명력 세팅 함수 수정.
	// 캐릭터가 죽은 상태에서 강종이나 튕김 현상이 발생 할 때, 
	// 캐릭터의 생명력/마나 30%를 복구해 주어야 한다. 그런데 상태가 죽은 상태면, 
	// 기존 함수는 return 처리를 하므로, 강제로 복구 여부를 세팅할 수 있는 함수를 추가한다.
	void SetLifeForce( DWORD Life, BYTE byForce, BOOL bSendMsg = TRUE) ;
	void SetManaForce(DWORD Mana, BYTE byForce, BOOL bSendMsg = TRUE) ;

	virtual DWORD		GetLife();
	virtual void		SetLife( DWORD Life, BOOL bSendMsg = TRUE );

	virtual DWORD		GetMana();
	virtual void		SetMana( DWORD valb, BOOL SendMsg = TRUE );	

	virtual DWORD		DoGetMaxLife();
	virtual DWORD		DoGetMaxMana();
	virtual DWORD		DoGetPhyAttackPowerMin();
	virtual DWORD		DoGetPhyAttackPowerMax();

	/// 사정거리
	virtual float		DoGetAddAttackRange() { return GetDexterity() / 3.f; }
	/// 크리티컬 확률
	virtual DWORD		DoGetCritical();
	virtual DWORD		DoGetDecisive();

	/// 이동속도 계산
	virtual float		DoGetMoveSpeed();
	/// 스텟 재계산
	void				CalcState();
	void				ClearMurderIdx();
	DWORD				GetMurderIDX() const { return m_MurdererIDX; }
	void						SetPenaltyByDie( BOOL bVal ) { m_bPenaltyByDie = bVal; }	//방파 비무 시 상대에 의해 죽었을 시 TRUE //강제종료 시 패널티 체크를 위한 변수
	BOOL						IsPenaltyByDie() { return m_bPenaltyByDie; }
	BOOL						IsReadyToRevive() { return m_bReadyToRevive;	}
	void						SetReadyToRevive( BOOL val ) { m_bReadyToRevive = val;	}
	// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
	//DWORD						RevivePenalty(BOOL bAdditionPenalty) ;
	EXPTYPE						RevivePenalty(BOOL bAdditionPenalty) ;
	void						RevivePresentSpot();
	void						ReviveLogIn();
	void						ReviveAfterShowdown( BOOL bSendMsg = TRUE );
	void						ReviveLogInPenelty();
	// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
	//DWORD						ReviveBySkill();
	void						ReviveBySkill( cSkillObject* pSkillObject );

	// 081210 LYW --- Player : 공성전 길드 던전에서의 부활 문제로 인해 두가지 함수를 추가한다.
	void						ReviveLogIn_Normal() ;
	void						ReviveLogIn_GuildDungeon() ;
	DWORD						GetPartyIdx() const { return m_HeroInfo.PartyID; }
	void						SetPartyIdx(DWORD PartyIDx);
	void						SendLifeToParty(DWORD life);
	void						SendManaToParty(DWORD mana);
	DWORD						GetGuildIdx() const { return m_HeroCharacterInfo.MunpaID;	}
	void						SetGuildIdx( DWORD GuildIdx ) { m_HeroCharacterInfo.MunpaID = GuildIdx; }
	void						SetGuildInfo( DWORD GuildIdx, BYTE Grade, const char* GuildName, MARKNAMETYPE MarkName );
	BYTE						GetGuildMemberRank() { return m_HeroCharacterInfo.PositionInMunpa; 	}
	void						SetGuildMemberRank( BYTE rank ) { m_HeroCharacterInfo.PositionInMunpa = rank; }
	void						SetGuildName( char* GuildName ) { SafeStrCpy(m_HeroCharacterInfo.GuildName, GuildName, _countof(m_HeroCharacterInfo.GuildName)); }
	void						SetGuildMarkName( MARKNAMETYPE MarkName );
	char*						GetGuildCanEntryDate();
	void						SetGuildCanEntryDate( char* date );
	void						ClrGuildCanEntryDate();
	void						SetNickName( char* NickName );

	/// 동맹 길드
	DWORD						GetGuildUnionIdx() { return m_HeroCharacterInfo.dwGuildUnionIdx; }
	void						InitGuildUnionInfo( DWORD dwGuildUnionIdx, char* pGuildUnionName, DWORD dwMarkIdx );
	void						SetGuildUnionInfo( DWORD dwGuildUnionIdx, char* pGuildUnionName, DWORD dwMarkIdx );
	void						SetGuildUnionMarkIdx( DWORD dwMarkIdx )	{ m_HeroCharacterInfo.dwGuildUnionMarkIdx = dwMarkIdx; }

	/// 길드 정보
	DWORD						GetFamilyIdx() const { return m_HeroCharacterInfo.FamilyID;	}
	void						SetFamilyIdx( DWORD FamilyIdx ) { m_HeroCharacterInfo.FamilyID = FamilyIdx; }
	void						SetFamilyNickName( char* NickName );

	void						AddQuest(CQuestBase*);
	BOOL						SetQuestState(DWORD QuestIdx, QSTATETYPE);
	CQuestGroup&				GetQuestGroup()	{ return m_QuestGroup; }
	
	/// 몬스터
	BOOL						AddFollowList( CMonster * pMob );
	void						RemoveFollowList( DWORD ID );
	BOOL						RemoveFollowAsFarAs( DWORD GAmount, CObject* pObject );
	WORD						m_wKillMonsterLevel;
	void						SetKillMonsterLevel( WORD wMonLevel ) { m_wKillMonsterLevel = wMonLevel; }
	WORD						GetKillMonsterLevel() { return m_wKillMonsterLevel; }
	virtual	DWORD				GetGravity() { return mGravity; }
	void						AddToAggroed(DWORD objectIndex);
	void						RemoveFromAggroed(DWORD objectIndex) { mAggroObjectContainer.erase(objectIndex); }
	void						RemoveAllAggroed();
	// 어그로가 적용된 대상 중 하나를 랜덤으로 반환한다
	virtual CObject*			GetTObject() const;
	virtual DWORD				GetAggroNum() { return mAggroObjectContainer.size(); }
	void						AddAggroToMyMonsters(int nAggroAdd, DWORD targetObjectIndex, DWORD skillIndex);
	void 						SetShowdown( BOOL bShowdown ) { m_bShowdown = bShowdown; }
	BOOL 						IsShowdown() { return m_bShowdown; }
	void 						SetShowdownOpPlayerID( DWORD dwOpPlayerID ) { m_dwOpPlayerID = dwOpPlayerID; }
	DWORD 						GetShowdownOpPlayerID()	{ return m_dwOpPlayerID; }

	/// PK
	int  						PKModeOn();
	BOOL  						PKModeOff();
	void  						PKModeOffForce();
	BOOL  						IsPKMode()							{ return m_HeroCharacterInfo.bPKMode; }
	void  						SetPKStartTimeReset()				{ m_dwPKModeStartTime = gCurTime; }
	//---KES PK 071202
	DWORD						GetPKContinueTime()					{ return m_dwPKContinueTime; }
	void  						SetPKContinueTime( DWORD dwTime )	{ m_dwPKContinueTime = dwTime; }
	//----------------
	void  						SetPKModeEndtime();

	/// 길드토너먼트
	void  						SetReturnMapNum( WORD num )	{ m_wGTReturnMapNum = num; }
	WORD  						GetReturnMapNum() { return m_wGTReturnMapNum; }
	void  						SetObserverBattleIdx( int Idx ) { m_ObserverBattleIdx = Idx; }
	int	 						GetObserverBattleIdx() { return m_ObserverBattleIdx; }
	void						AddKillCountFromGT() {m_dwKillCountFromGT++;}
	DWORD						GetKillCountFromGT() {return m_dwKillCountFromGT;}
	void						AddKillPointFromGT(WORD point) {m_dwKillPointFromGT += point;}
	DWORD						GetKillPointFromGT() {return m_dwKillPointFromGT;}
	void						SetRespawnTime(DWORD dwTime) {m_dwRespawnTimeOnGTMAP = dwTime;}
	void						SetImmortalTime(DWORD dwTime) {m_dwImmortalTimeOnGTMAP = dwTime;}
	DWORD						GetImmortalTime()				{return m_dwImmortalTimeOnGTMAP;}
	void						CheckImmortalTime();
	void						SetStartPosOnGTMAP(VECTOR3* pPos)	{m_vStartPosOnGTMAP = *pPos;}
	void						GetStartPosOnGTMAP(VECTOR3& pos)	{pos = m_vStartPosOnGTMAP;}
	void						SetDieForGFW(BOOL bDie) { m_bDieForGFW = bDie; }
	BOOL						IsDieFromGFW() const { return m_bDieForGFW; }
	/// 전투 능력 수치
	float						GetAccuracy();
	float						GetAvoid();
	float						GetBlock();
	float						GetPhysicAttackMax();
	float						GetPhysicAttackMin();
	float						GetPhysicAttack();
	float						GetMagicAttackMax();
	float						GetMagicAttackMin();
	float						GetMagicAttack();
	float						GetPhysicDefense();
	float						GetMagicDefense();
	float						GetCriticalRate();
	float						GetCriticalDamageRate();
	float						GetCriticalDamagePlus();
	float						GetMagicCriticalRate();
	float						GetMagicCriticalDamageRate();
	float						GetMagicCriticalDamagePlus();
	float						GetLifeRecover();
	float						GetManaRecover();
	DWORD GetShieldDefense();
	void SetShieldDefence(DWORD);
	void SpeedHackCheck();

	/// 기본 상태 프로세스
	virtual void	DoDie( CObject* pAttacker );
	virtual void	DoDamage( CObject* pAttacker, RESULTINFO* pDamageInfo, DWORD beforeLife );
	virtual DWORD	Damage( CObject* pAttacker, RESULTINFO* pDamageInfo );
	virtual void	DoManaDamage( CObject* pAttacker, RESULTINFO* pDamageInfo, DWORD beforeMana );
	virtual DWORD	ManaDamage( CObject* pAttacker, RESULTINFO* pDamageInfo );

	virtual void	StateProcess();
	
	/// 퀘스트 프로세스
	void			QuestProcess();

	/// 종료
	void			SetWaitExitPlayer( BOOL bWait ) { m_bWaitExitPlayer = bWait; }
	BOOL			IsWaitExitPlayer() { return m_bWaitExitPlayer; }
	BOOL			IsExitStart() { return m_bExit; }
	int				CanExitStart();
	void			SetExitStart( BOOL bExit );
	int				CanExit();
	void			ExitCancel();
	void			SetNormalExit() { m_bNormalExit = TRUE; }
	BOOL			IsNormalExit() { return m_bNormalExit; }
	void			UpdateLogoutToDB(BOOL val = TRUE);
	
	void			SetEmergency() { mIsEmergency = TRUE; }
	BOOL			IsEmergency() { return mIsEmergency; }
	
	void			SetMapMoveInfo( MAPTYPE map, DWORD pos_x, DWORD pos_z )
	{
		mTargetMap = map;
		mTargetPosX = pos_x;
		mTargetPosZ = pos_z;
	}

	void			GetMapMoveInfo( MAPTYPE &map, DWORD &pos_x, DWORD &pos_z )
	{
		map = mTargetMap;
		pos_x =	mTargetPosX;
        pos_z =	mTargetPosZ;
	}

	ITEMOBTAINARRAYINFO*				GetArray( WORD id );
	void								AddArray( ITEMOBTAINARRAYINFO * pInfo );
	void								RemoveArray( DWORD id );
	CYHHashTable<ITEMOBTAINARRAYINFO>	m_ItemArrayList;

	/// 각종 인벤토리
	CItemSlot *							GetSlot( POSTYPE absPos );
	CItemSlot *							GetSlot( eITEMTABLE tableIdx );

	void								AddStorageItem( ITEMBASE * pStorageItem );
	void								SetStorageNum( BYTE n );
	BYTE								GetStorageNum();

	void								InitItemTotalInfo(ITEM_TOTALINFO*);
	void								InitStorageInfo(BYTE storagenum, MONEYTYPE);
	BOOL								IsGotWarehouseItems() const { return m_bGotWarehouseItems; }
	void								SetGotWarehouseItems(BOOL bGot) { m_bGotWarehouseItems = bGot; }
	void								InitShopItemInfo(SEND_SHOPITEM_INFO&);
	void								GetItemtotalInfo(ITEM_TOTALINFO&, DWORD flag);
	DWORD								GetWearedItemIdx( EWEARED_ITEM WearedPosition ) const { return m_HeroCharacterInfo.WearedItemIdx[ WearedPosition ]; }
	void								SetWearedItemIdx( DWORD WearedPosition, DWORD ItemIdx );
	DWORD								GetWearedWeapon() const { return m_HeroCharacterInfo.WearedItemIdx[eWearedItem_Weapon]; }
	eWeaponType							GetWeaponEquipType();
	// 080703 LUJ, 양손에 장비한 무기를 종합하여 타입을 반환한다
	eWeaponAnimationType				GetWeaponAniType();
	eArmorType							GetArmorType( EWEARED_ITEM ) const;
	
	BOOL								RemoveItem(DWORD nItemID, DWORD nItemNum, eLogitemmoney eLogKind);
	
	MONEYTYPE							SetMoney( MONEYTYPE ChangeValue, BYTE bOper, BYTE MsgFlag = 0, eITEMTABLE tableIdx = eItemTable_Inventory, BYTE LogType = 0, DWORD TargetIdx = 0 );
	MONEYTYPE							GetMoney( eITEMTABLE tableIdx = eItemTable_Inventory );
	MONEYTYPE							GetMaxPurseMoney( eITEMTABLE TableIdx );
	void								SetMaxPurseMoney( eITEMTABLE TableIdx, DWORD MaxMoney );
	BOOL								IsEnoughAdditionMoney( MONEYTYPE money, eITEMTABLE tableIdx = eItemTable_Inventory );
	void								RSetMoney( MONEYTYPE money, BYTE flag );	
	void								SetExchangeContainer( sEXCHANGECONTAINER* ExchangeContainer ) { m_ExchangeContainer = *ExchangeContainer; }
	sEXCHANGECONTAINER*					GetExchangeContainer() { return &m_ExchangeContainer; }
	
	/// 노점상
	void								SetGuestStall( cStreetStall* pStall ) { m_pGuetStall = pStall; }
	cStreetStall*						GetGuestStall() { return m_pGuetStall; }
	void								SetStreetStallTitle( char* title );
	void								GetStreetStallTitle( char* title );
	
	/// 아이템 정보 관련 함수----------------------------------------------/
	///--------------------------------------------------------------------/

	
	///--------------------------------------------------------------------/
	/// 스킬 정보 관련 함수------------------------------------------------/

	// 080509 LUJ, 스킬 쿨타임 체크
public:	
	// 080509 LUJ, 스킬 쿨타임이 지나지 않았으면 참을 반환한다
	// 080516 LUJ, 실패 카운트 체크를 저장하기 위해 상수 선언 제거
	BOOL IsCoolTime( const ACTIVE_SKILL_INFO& );
	// 080511 LUJ, 스킬 쿨타임을 설정해놓는다
	void SetCoolTime( const ACTIVE_SKILL_INFO& );
	void ResetCoolTime( const ACTIVE_SKILL_INFO& );
	BOOL IsCanCancelSkill();
	const DWORD GetSkillCancelDelay() const;

private:
	// 080509 LUJ, 키: 스킬 인덱스, 값: 만료될 시점의 틱카운트
	typedef stdext::hash_map< DWORD, DWORD > SkillCoolTimeMap;
	SkillCoolTimeMap mSkillCoolTimeMap;

	// 080514 LUJ, 스킬 애니메이션 타임 체크. 애니메이션 시간 동안 다른 스킬이 발동할 수 없도록 채쿠
	typedef stdext::hash_map< DWORD, DWORD > SkillAnimTimeMap;
	SkillAnimTimeMap mSkillAnimTimeMap;

	// 080515 LUJ, 쿨타임 체크 실패 관리용 구조체
	struct CheckCoolTime
	{
		// 080515 LUJ, 쿨타임 체크가 실패한 최초 틱카운트
		DWORD mCheckedTick;
		// 080515 LUJ, 쿨타임 체크 실패 회수
		DWORD mFailureCount;
	}
	mCheckCoolTime;

	// 100617 ShinJS 스킬 취소에 대한 제한처리
	enum eSkillCancelLimit
	{
		eSkillCancelLimit_Count = 3,
		eSkillCancelLimit_CheckTime = 5000,
	};

	DWORD m_dwSkillCancelCount;
	DWORD m_dwSkillCancelLastTime;

public:
	cSkillTree& GetSkillTree();
	void SetCurComboNum(DWORD combonum) { m_CurComboNum = combonum; }
	DWORD GetCurComboNum() const { return m_CurComboNum; }
	BOOL CanSkillState();
	void CancelCurrentCastingSkill( BOOL bUseSkillCancelRate );
	Status* GetPassiveStatus() { return &mPassiveStatus; }
	Status*	GetRatePassiveStatus() { return &mRatePassiveStatus; }
	// 090217 LUJ, 목적에 맞도록 함수 이름 변경
	void						SetHideLevel( WORD level );
	void						SetDetectLevel( WORD level );
	WORD						GetHideLevel() const { return m_HeroCharacterInfo.HideLevel; }
	WORD						GetDetectLevel() const { return m_HeroCharacterInfo.DetectLevel; }

	void						SetMonstermeterPlaytime(DWORD nTime, DWORD nTimeTotal)		{ m_stMonstermeterInfo.nPlayTime = nTime; m_stMonstermeterInfo.nPlayTimeTotal = nTimeTotal; }
	void						SetMonstermeterKillMonNum(DWORD nNum, DWORD nNumTotal)		{ m_stMonstermeterInfo.nKillMonsterNum = nNum; m_stMonstermeterInfo.nKillMonsterNumTotal = nNumTotal; }
	void						SetMonstermeterPlaytimeTick(DWORD nTick)					{ m_stMonstermeterInfo.nPlayTimeTick = nTick; }

	stMONSTERMETER*				GetMonstermeterInfo()										{ return &m_stMonstermeterInfo; }

	void						ProcMonstermeterPlayTime();
	void						ProcMonstermeterKillMon();
	DATE_MATCHING_INFO*			GetResidentRegistInfo()	{ return &m_DateMatching; }
	void						SetResidentRegistInfo(DATE_MATCHING_INFO* info) { m_DateMatching = *info; }

	void		SetDateMatchingSerchTimeTick(DWORD nTick)			{ m_DateMatching.nSerchTimeTick = nTick; }
	void		SetDateMatchingChatPlayerID(DWORD nChatPlayerID)	{ m_DateMatching.nChatPlayerID = nChatPlayerID; }
	void		SetDateMatchingChatState(DWORD nState)				{ m_DateMatching.nRequestChatState = nState; }
	void		SetDateMatchingChatTimeTick(DWORD nTick)			{ m_DateMatching.nRequestChatTimeTick = nTick; }
	// E 주민등록 추가 added by hseos 2007.06.04	2007.06.09

	// desc_hseos_패밀리01
	// S 패밀리 추가 added by hseos 2007.07.06	2007.07.17	2007.11.22
	void						SetFamilyEmblem(char* pEmblem)			{ m_pcFamilyEmblem = pEmblem; }
	char*						GetFamilyEmblem()						{ return m_pcFamilyEmblem; }
	void						SetFamilyEmblemChangedFreq(DWORD nFreq)	{ m_HeroCharacterInfo.nFamilyEmblemChangedFreq = nFreq; }
	DWORD						GetFamilyEmblemChangedFreq()			{ return m_HeroCharacterInfo.nFamilyEmblemChangedFreq; }
	// E 패밀리 추가 added by hseos 2007.07.06	2007.07.17	2007.11.22

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.08.23
	stFARMPLAYERINFO*				GetFarmInfo()							{ return &m_stFarmInfo; }
	void							SetFarmCropPlantRetryTime(UINT nTime)	{ m_stFarmInfo.nCropPlantRetryTime = nTime; m_stFarmInfo.nCropPlantRetryTimeTick = gCurTime; }
	void							SetFarmCropManureRetryTime(UINT nTime)	{ m_stFarmInfo.nCropManureRetryTime = nTime; m_stFarmInfo.nCropManureRetryTimeTick = gCurTime; }
	void							ProcFarmTime();
	// E 농장시스템 추가 added by hseos 2007.08.23

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.27	2007.12.05	2008.01.22
	UINT							GetChallengeZoneEnterFreq()						{ return m_nChallengeZoneEnterFreq; }
	UINT							GetChallengeZoneEnterBonusFreq()				{ return m_nChallengeZoneEnterBonusFreq; }
	void							IncreaseChallengeZoneEnterFreq()				{ m_nChallengeZoneEnterFreq++; }
	void							SetChallengeZoneEnterFreq(UINT nFreq)			{ m_nChallengeZoneEnterFreq = nFreq; }
	void							SetChallengeZoneEnterBonusFreq(UINT nFreq)		{ m_nChallengeZoneEnterBonusFreq = nFreq; }

	UINT							GetChallengeZonePartnerID()						{ return m_nChallengeZonePartnerID; }
	void							SetChallengeZonePartnerID(DWORD nPlayerID)		{ m_nChallengeZonePartnerID = nPlayerID; }

	UINT							GetChallengeZoneSection()						{ return m_nChallengeZoneSection; }
	void							SetChallengeZoneSection(UINT nSection)			{ m_nChallengeZoneSection = nSection; }
	UINT							GetChallengeZoneStartState()					{ return m_nChallengeZoneStartState; }
	void							SetChallengeZoneStartState(UINT nState)			{ m_nChallengeZoneStartState = nState; }
	UINT							GetChallengeZoneStartTimeTick()					{ return m_nChallengeZoneStartTimeTick; }
	void							SetChallengeZoneStartTimeTick(UINT nTimeTick)	{ m_nChallengeZoneStartTimeTick = nTimeTick; }

	UINT							GetChallengeZoneMonsterNum()					{ return m_nChallengeZoneMonsterNum; }
	void							SetChallengeZoneMonsterNum(UINT nNum)			{ m_nChallengeZoneMonsterNum = nNum; }
	UINT							GetChallengeZoneKillMonsterNum()				{ return m_nChallengeZoneKillMonsterNum; }
	void							SetChallengeZoneKillMonsterNum(UINT nNum)		{ m_nChallengeZoneKillMonsterNum = nNum; }
	void							IncreaseChallengeZoneKillMonsterNum()			{ m_nChallengeZoneKillMonsterNum++; }

	UINT							GetChallengeZoneNeedSaveEnterFreq()				{ return m_bChallengeZoneNeedSaveEnterFreq; }
	void							SetChallengeZoneNeedSaveEnterFreq(BOOL bNeed)	{ m_bChallengeZoneNeedSaveEnterFreq = bNeed; }

	UINT							IsChallengeZoneCreateMonRightNow()				{ return m_bChallengeZoneCreateMonRightNow; }
	void							SetChallengeZoneCreateMonRightNow(BOOL bSet)	{ m_bChallengeZoneCreateMonRightNow = bSet; }

	UINT							GetChallengeZoneMonsterNumTillNow()				{ return m_nChallengeZoneMonsterNumTillNow; }
	void							SetChallengeZoneMonsterNumTillNow(UINT nNum)	{ m_nChallengeZoneMonsterNumTillNow = nNum; }

	DWORD							GetChallengeZoneClearTime()						{ return m_nChallengeZoneClearTime; }
	void							SetChallengeZoneClearTime(DWORD nClearTime)		{ m_nChallengeZoneClearTime = nClearTime; }

	// 091123 ONS 챌린지존 경험치 분배 처리 추가
	DWORD							GetChallengeZoneExpRate()						{ return m_nChallengeZoneExpRate; }
	void							SetChallengeZoneExpRate(DWORD nExpRate)			{ m_nChallengeZoneExpRate = nExpRate; }
	// 070813 웅주, 인벤토리 위치 체크
	BOOL IsInventoryPosition( POSTYPE );

	// 080507 KTH --
	void							SetFarmAnimalFeedRetryTime(UINT nTime)	{ m_stFarmInfo.nAnimalFeedRetryTime = nTime; m_stFarmInfo.nAnimalFeedRetryTimeTick = gCurTime; }
	void							SetFarmAnimalCleanRetryTime(UINT nTime)	{ m_stFarmInfo.nAnimalCleanRetryTime = nTime; m_stFarmInfo.nAnimalCleanRetryTimeTick = gCurTime; }
	
	// 070705 세트 아이템 능력 관리
public:
	// 071207 LUJ 세트 정보 초기화
	void	ResetSetItemStatus();

	typedef std::map< const SetScript*, int >	SetItemLevel;

	const SetItemLevel&	GetSetItemLevel() const;
	SetItemLevel&		GetSetItemLevel();
private:
	SetItemLevel mSetItemLevel;

	// 070719 세트 스킬 관리
	// 웅주, 070719 세트 패시브 스킬 적용
public:
	void AddSetSkill(DWORD skillIndex, LEVELTYPE);
	void RemoveSetSkill(DWORD skillIndex, LEVELTYPE);
	void AddJobSkill( DWORD skillIndex, BYTE level );
	void ClearJobSkill();
private:
	typedef std::list<SKILL_BASE> JobSkillList;
	JobSkillList m_JobSkillList;
	
	// 071005 웅주, 쿨타임
public:
	BOOL AddCoolTime(DWORD coolTimeGroupIndex, DWORD coolTime);
	void RemoveCoolTime( DWORD coolTimeGroupIndex);
	
private:
	void MoneyUpdate(MONEYTYPE);
	void UpdateGravity();
	void ProcCoolTime();
	virtual	void SetObjectBattleState(eObjectBattleState);

	struct CoolTime
	{
		DWORD mBeginTick;
		DWORD mEndTick;
	};

	// 키워드: 쿨타임 그룹 인덱스
	typedef std::map< DWORD, CoolTime > CoolTimeMap;
	CoolTimeMap mCoolTimeMap;
	DWORD m_dwLastTimeCheckItemDBUpdate;
	DWORD m_dwLastTimeCheckItem;
public:
	void ProcessTimeCheckItem( BOOL bForceDBUpdate );

	// 071128 LYW --- Player : HPMP 적용.
	void SetHP_Point( BYTE hpPoint ) { m_byHP_Point = hpPoint ; }
	BYTE GetHP_Point() { return m_byHP_Point ; }
	void SetMP_Point( BYTE mpPoint ) { m_byMP_Point = mpPoint ; }
	BYTE GetMP_Point() { return m_byMP_Point ; }

	// 080213 KTH -- ClearInventory
	BOOL ClearInventory();
	
	// 071206 KTH -- 스킬과 스텟이 초기화 중인지 검사.
	BOOL	IsResetSkill()	{	return m_bResetSkill;	}
	void	SetResetSkill( BOOL bResetSkill )	{	m_bResetSkill = bResetSkill;	}
	BOOL	IsResetStat()	{	return m_bResetStat;	}
	void	SetResetStat( BOOL bResetStat )	{	m_bResetStat = bResetStat;	}

private:
	BOOL	m_bResetSkill;
	BOOL	m_bResetStat;

protected:
	DWORD	m_dwReturnSkillMoney;	//돌려줄 돈 임시 보관 장소
public:
	void	SetRetrunSkillMoney( DWORD ReturnSkillMoney )	{	m_dwReturnSkillMoney = ReturnSkillMoney;	}
	DWORD	GetReturnSkillMoney()							{	return m_dwReturnSkillMoney;	}

	// 071210 KTH -- 확장인벤토리 기능
	WORD	GetInventoryExpansion()							{ return m_HeroCharacterInfo.wInventoryExpansion; }
	// 071225 KTH -- (Fix) 확장 인벤토리의 갯수가 변경될 경우 확장된 수만큼 슬롯의 갯수를 다시 셋팅하여 준다.
	void	SetInventoryExpansion(WORD InventoryExpansion)	{ 
																m_HeroCharacterInfo.wInventoryExpansion = InventoryExpansion; 
																m_InventorySlot.SetSlotNum( (POSTYPE) ( SLOT_INVENTORY_NUM + GetInventoryExpansionSize() ));
															}
	BOOL	IncreaseInventoryExpansion();
	// 071213 KTH -- 확장인벤토리의 크기 가져오기
	WORD	GetInventoryExpansionSize()						{	return (WORD) ( m_HeroCharacterInfo.wInventoryExpansion * TABCELL_INVENTORY_NUM ) ;	}

	// 071226 KTH -- 이름 변경.
	VOID	SetCharacterName(char* name)					{	strcpy(m_BaseObjectInfo.ObjectName, name);	}
	virtual void ProceedToTrigger();
	void PassiveSkillCheckForWeareItem();

//---KES AUTONOTE
protected:
	DWORD m_dwAutoNoteIdx;
	DWORD m_dwLastActionTime;

	// 090105 ShinJS --- 오토노트 적발 실패시 재실행 판단을 위한 시간 변수 추가
	DWORD m_dwAutoNoteLastExecuteTime;			// 오토노트 마지막 실행 시간(적발 실패시 저장)
public:
	DWORD GetAutoNoteIdx() const { return m_dwAutoNoteIdx; }
	void SetAutoNoteIdx(DWORD AutoNoteIdx) { m_dwAutoNoteIdx = AutoNoteIdx; }

	void SetCharacterSize(float fSize) { m_HeroCharacterInfo.Width = m_HeroCharacterInfo.Height = fSize; }
	float GetCharacterSize() const { return m_HeroCharacterInfo.Height; }
	DWORD GetLastActionTime() const { return m_dwLastActionTime; }
	void SetLastActionTime( DWORD dwLastActionTime ) { m_dwLastActionTime = dwLastActionTime; }

	// 090105 ShinJS --- 오토노트 실행시간 조작 함수 추가
	DWORD GetAutoNoteLastExecuteTime() const { return m_dwAutoNoteLastExecuteTime; }
	void SetAutoNoteLastExecuteTime() { m_dwAutoNoteLastExecuteTime = gCurTime; }

	void SetWeaponEnchant( BYTE level )
	{
		m_HeroCharacterInfo.WeaponEnchant = level;
	}

// 080411 NYJ - 낚시
protected:
	DWORD m_dwFishingPlaceIdx;				// 낚시터NPC Instance Index
	DWORD m_dwFishingStartTime;				// 시작시간
	DWORD m_dwFishingProcessTime;			// 진행시간
	DWORD m_dwFishingBaitIdx;				// 사용한 미끼 Item index

	float m_fFishItemRate[eFishItem_Max];	// 장비에 따른 낚시확률 추가변수
	float m_fFishBaitRate[eFishItem_Max];	// 미끼에 따른 낚시확률 추가변수


	DWORD m_dwFM_MissionCode;				// 진행중인 미션
	DWORD m_dwFM_StartTime;					// 미션 시작 시간

	// 낚시관련데이타
	WORD  m_wFishingLevel;					// 낚시숙련등급
	// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
	//DWORD m_dwFishingExp;					// 낚시숙련도
	EXPTYPE m_dwFishingExp;					// 낚시숙련도

	DWORD m_dwFishPoint;					// 물고기포인트
	
public:
	std::list<DWORD> m_lstGetFishList;		// 미션중 최근 낚은 3마리 고기, FM(FishingMission)

	void FishingInfoClear();				// 낚시관련정보 한방에 클리어.

	void SetFishingPlace(DWORD dwIndex)						{m_dwFishingPlaceIdx = dwIndex;}
	DWORD GetFishingPlace()									{return m_dwFishingPlaceIdx;}

	void SetFishingStartTime(DWORD dwTime)					{m_dwFishingStartTime = dwTime;}
	DWORD GetFishingStartTime()								{return m_dwFishingStartTime;}

	void SetFishingProcessTime(DWORD dwTime)				{m_dwFishingProcessTime = dwTime;}
	DWORD GetFishingProcessTime()							{return m_dwFishingProcessTime;}

	void SetFishingBait(DWORD dwIndex)						{m_dwFishingBaitIdx = dwIndex;}
	DWORD GetFishingBait()									{return m_dwFishingBaitIdx;}

	float* GetFishItemRate()								{return m_fFishItemRate;}
	float GetFishItemRate_Grade(int nGrade)					{if(0<=nGrade && nGrade<eFishItem_Max)	return m_fFishItemRate[nGrade];	return 0.0f;}

	float* GetFishBaitRate()								{return m_fFishBaitRate;}
	float GetFishBaitRate_Grade(int nGrade)					{if(0<=nGrade && nGrade<eFishItem_Max)	return m_fFishBaitRate[nGrade];	return 0.0f;}


	void SetFM_StartTime(DWORD dwStartTime)					{m_dwFM_StartTime = dwStartTime;}
	DWORD GetFM_StartTime()									{return m_dwFM_StartTime;}

	// 낚시미션관련
	void SetFM_Mission(DWORD dwCode)						{m_dwFM_MissionCode = dwCode;}
	DWORD GetFM_Mission()									{return m_dwFM_MissionCode;}

	// 낚시관련데이타
	void SetFishingLevel(WORD wLevel)						{m_wFishingLevel = wLevel;}
	WORD GetFishingLevel()									{return m_wFishingLevel;}
	void SetFishingExp(EXPTYPE);
	EXPTYPE GetFishingExp() const {return m_dwFishingExp;}
	void SetFishPoint(DWORD dwPoint) {m_dwFishPoint = dwPoint; if(MAX_FISHPOINT<m_dwFishPoint) m_dwFishPoint=MAX_FISHPOINT;}
	DWORD GetFishPoint() const {return m_dwFishPoint;}
	BOOL IsNoEquip(eArmorType, eWeaponType, eWeaponAnimationType);
	BOOL IsEnable(const BUFF_SKILL_INFO&);
	virtual float GetBonusRange() const;

	void SetPetItemDbIndex(DWORD itemDbIndex) { mPetIndex = itemDbIndex; }
	DWORD GetPetItemDbIndex() const { return mPetIndex; }
	virtual void AddSpecialSkill(const cBuffSkillInfo*);
	virtual void RemoveSpecialSkill(const cBuffSkillInfo*);
private:
	// 090204 LUJ, 특수 스킬을 프로세스 타임에 체크한다
	void ProcSpecialSkill();

	struct SpecialSkillData
	{
		const cBuffSkillInfo* mBuffSkillInfo;
		BOOL mIsTurnOn;
	};
	typedef std::list< SpecialSkillData > SpecialSkillList;
	SpecialSkillList mSpecialSkillList;

	// 081119 NYJ - 요리
protected:
	WORD					m_wCookLevel;
	WORD					m_wCookCount;
	WORD					m_wEatCount;
	WORD					m_wFireCount;
	DWORD					m_dwLastCookTime;
	stRecipeLv4Info			m_MasterRecipe[MAX_RECIPE_LV4_LIST];
public:
	void					SetCookLevel(WORD wLevel)	{m_wCookLevel = wLevel;}
	void					SetCookCount(WORD wCount)	{m_wCookCount = wCount;}
	void					SetEatCount(WORD wCount)	{m_wEatCount = wCount;}
	void					SetFireCount(WORD wCount)	{m_wFireCount = wCount;}
	void					SetLastCookTime(DWORD dwTime){m_dwLastCookTime = dwTime;}
	void					SetMasterRecipe(POSTYPE pos, DWORD dwRecipe, DWORD dwRemainTime);

	WORD					GetCookLevel()				{return m_wCookLevel;}
	WORD					GetCookCount()				{return m_wCookCount;}
	WORD					GetEatCount()				{return m_wEatCount;}
	WORD					GetFireCount()				{return m_wFireCount;}
	DWORD					GetLastCookTime()			{return m_dwLastCookTime;}
	stRecipeLv4Info*		GetMasterRecipe(POSTYPE pos);
	
	int						CanAddRecipe(DWORD dwRecipe);
	void					ProcessRecipeTimeCheck(DWORD dwElapsedTime);


	/// 090213 LYW --- Player : 접속 중인 패밀리 멤버 수에 따른 추가 보상 처리.
public :
	void							SetFamilyRewardExp( EXPTYPE rewardExp )	{ m_dweFamilyRewardExp = rewardExp ; }
	void							SetCurFamilyMemCnt( BYTE byCount )		{ m_byCurFamilyMemCnt = byCount ; }
	BYTE							GetCurFamilyMemCnt() const				{ return m_byCurFamilyMemCnt ; }

	// 090316 LUJ, 탈것
public:
	inline DWORD GetSummonedVehicle() const { return mSummonedVehicleIndex; }
	inline void SetSummonedVehicle( DWORD vehicleIndex ) { mSummonedVehicleIndex = vehicleIndex; }
	inline DWORD GetMountedVehicle() const { return mMountedVehicleIndex; }
	inline void SetMountedVehicle( DWORD vehicleIndex ) { mMountedVehicleIndex = vehicleIndex; }
private:
	DWORD mSummonedVehicleIndex;
	DWORD mMountedVehicleIndex;

public:
	// 091123 공성전 워터시드 사용중 플레그 추가 
	void SetUsingWaterSeedFlag( BOOL bFlag ) { m_HeroCharacterInfo.bUsingWaterSeed = bFlag ; }


	// NYJ, 하우스
private:
	char				m_szHouseName[MAX_HOUSING_NAME_LENGTH];

public:
	void				SetHouseName(char* pHouseName)	{strcpy(m_szHouseName, pHouseName);}
	char*				GetHouseName()					{return m_szHouseName;}
	
	// 100211 ONS 부활대상자가 수락한 경우 부활처리를 실행한다.
private:
	DWORD		m_dwCurrentResurrectIndex;					// 현재 부활스킬 응답중일경우, 해당 스킬 인덱스를 저장한다.
public:
    void		SetCurResurrectIndex( DWORD dwIndex ) { if( !m_dwCurrentResurrectIndex ) m_dwCurrentResurrectIndex = dwIndex; }
	DWORD		GetCurResurrectIndex(){ return m_dwCurrentResurrectIndex; }
	EXPTYPE		OnResurrect();

	// NYJ, 인던모니터링
private:
	BOOL		m_bDungeonObserver;
	VECTOR3		m_DungeonObserverPos;
public:
	void		SetDungeonObserver(BOOL bVal)		{m_bDungeonObserver = bVal;}
	BOOL		IsDungeonObserver()					{return m_bDungeonObserver;}
	void		SetDungeonObserverPos(VECTOR3* pPos){memcpy(&m_DungeonObserverPos, pPos, sizeof(VECTOR3));}
	VECTOR3*	GetDungeonObserverPos()				{return &m_DungeonObserverPos;}

private:
	DWORD		m_dwConsignmentTick;
public:
	void		SetConsignmentTick()				{m_dwConsignmentTick = gCurTime;}
	DWORD		GetConsignmentTick()				{return m_dwConsignmentTick;}

	// 100624 ONS 큐에 정보를 저장 / HP업데이트처리 추가
	void AddLifeRecoverTime( const YYRECOVER_TIME& recoverTime );
	void AddManaRecoverTime( const YYRECOVER_TIME& recoverTime );
	void UpdateLife();
	void UpdateMana();
	void SetManaRecoverDirectly( DWORD recoverUnitAmout ) { m_ManaRecoverDirectlyAmount = recoverUnitAmout; }

// 100611 ONS 플레이어에 채팅금지 처리 추가.
private:
	__time64_t	ForbidChatTime;
public:
	void		SetForbidChatTime( __time64_t time ) { ForbidChatTime = time; }
	__time64_t	GetForbidChatTime() const { return ForbidChatTime; }
	BOOL		IsForbidChat() const;

};

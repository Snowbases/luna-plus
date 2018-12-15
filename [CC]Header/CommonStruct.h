#pragma once

#include "AddableInfo.h"
#include <hash_map>
#include <hash_set>
#include <list>
#include <map>
#include <set>
#include <string>

#pragma pack(push,1)
typedef unsigned long long DWORDEX;
typedef WORD MAPTYPE;
typedef DWORDEX EXPTYPE;
typedef WORD LEVELTYPE;
typedef DWORD DURTYPE;
typedef WORD POSTYPE;
typedef DWORD ITEMPARAM;
typedef int FAMETYPE;
typedef DWORD MARKNAMETYPE;
typedef DWORD MONEYTYPE;
#define MAXMONEY MAXULONG_PTR // 0xffffffff(4294967295)
#define MAX_JOB_GRADE 6

struct MSGROOT
{
	BYTE CheckSum;
#ifdef _CRYPTCHECK_ 
	MSGROOT():Code(0){}
	char Code;
#endif
	BYTE Category;
	BYTE Protocol;
};
struct MSGBASE :public MSGROOT
{
	DWORD dwObjectID;
};

struct COMPRESSEDPOS
{
	WORD wx;
	WORD wz;
	void Compress(VECTOR3* pPos)
	{
		wx = WORD(pPos->x);
		wz = WORD(pPos->z);
	}
	// 090316 LUJ, 상수형 함수로 선언
	void Decompress(VECTOR3* pRtPos) const
	{
		pRtPos->x = wx;
		pRtPos->z = wz;
		pRtPos->y = 0;
	}
};
struct COMPRESSEDTARGETPOS
{
	void Init()
	{
		PosNum = 0;
		memset( this, 0, sizeof(COMPRESSEDTARGETPOS) );
	}
	void AddTargetPos(VECTOR3 * pPos)
	{
		if( PosNum > MAX_CHARTARGETPOSBUF_SIZE )
		{
			ASSERT(0);
			return;
		}
		pos[PosNum++].Compress(pPos);
	}
	int GetSize()
	{
		return sizeof(COMPRESSEDTARGETPOS) - sizeof(COMPRESSEDPOS)*(MAX_CHARTARGETPOSBUF_SIZE-PosNum);
	}
	int GetSurplusSize()
	{
		return sizeof(COMPRESSEDPOS)*(MAX_CHARTARGETPOSBUF_SIZE-PosNum);
	}
	BYTE PosNum;
	COMPRESSEDPOS pos[MAX_CHARTARGETPOSBUF_SIZE];
};

struct SEND_MOVEINFO
{
	COMPRESSEDPOS CurPos;
	eMoveMode MoveMode;
	WORD KyungGongIdx;
	float AddedMoveSpeed;
};

struct BASEMOVE_INFO
{
	VECTOR3 CurPosition;
private:
	BYTE CurTargetPositionIdx;
	BYTE MaxTargetPositionIdx;
	VECTOR3 TargetPositions[MAX_CHARTARGETPOSBUF_SIZE];
public:
	bool bMoving;
	eMoveMode MoveMode;
	float AddedMoveSpeed;

	WORD KyungGongIdx;			// 경공Idx 0: 경공중이 아니다  !0: 사용중인 경공번호
	
	void SetFrom(SEND_MOVEINFO* pInfo)
	{
		bMoving = FALSE;
		pInfo->CurPos.Decompress(&CurPosition);
		MoveMode = pInfo->MoveMode;
		KyungGongIdx = pInfo->KyungGongIdx;
		AddedMoveSpeed = pInfo->AddedMoveSpeed;
	}
	BYTE GetCurTargetPosIdx() { return CurTargetPositionIdx; }
	BYTE GetMaxTargetPosIdx() { return MaxTargetPositionIdx; }
	BYTE & GetMaxTargetPosIdxRef() { return MaxTargetPositionIdx; }
	void SetCurTargetPosIdx(BYTE idx) { CurTargetPositionIdx=idx; }
	void SetMaxTargetPosIdx(BYTE idx) { MaxTargetPositionIdx=idx; }

	VECTOR3 * GetTargetPosition(BYTE idx) { return &TargetPositions[idx]; }
	VECTOR3 * GetTargetPositionArray() { return TargetPositions; }
	void InitTargetPosition()
	{
		CurTargetPositionIdx = 0;
		MaxTargetPositionIdx = 0;
		memset( TargetPositions, 0, sizeof( VECTOR3 ) * MAX_CHARTARGETPOSBUF_SIZE );
	}
	void SetTargetPosition( VECTOR3* pos )	{ memcpy( TargetPositions, pos, sizeof(VECTOR3)*MAX_CHARTARGETPOSBUF_SIZE );	}
	void SetTargetPosition( BYTE idx, VECTOR3 pos )	{ TargetPositions[idx] = pos;	}
};

struct MOVE_INFO : public BASEMOVE_INFO
{
	BOOL m_bLastMoving; // 클라이언트에서만 쓰임
	BOOL m_bEffectMoving;

	DWORD m_dwMoveCalculateTime;
	BOOL m_bMoveCalculateTime;

	// 마지막으로 계산한 시간.
	// 마지막으로 계산한 시간이 얼마되지 않았으면 또다시 계산하지 않는다
	DWORD Move_LastCalcTime;

	DWORD Move_StartTime;
	VECTOR3 Move_StartPosition;
	VECTOR3 Move_Direction;

	float Move_EstimateMoveTime;

	BOOL bApplyGravity;				// 중력 적용여
	DWORD dwValidGravityTime;		// 중력 적용 유효 시각
	float fGravityAcceleration;		// 중력 가속
	float fGravitySpeed;			// 현재 중력 적용 속도
};

struct STATE_INFO
{
	int State_Start_Motion;
	int State_Ing_Motion;
	int State_End_Motion;
	DWORD State_End_MotionTime;
	DWORD State_End_Time;
	DWORD State_Start_Time;
	BOOL bEndState;
	float MotionSpeedRate;
};


struct HERO_TOTALINFO
{
	DWORD	Str;							// Strength
	DWORD	Dex;						// 민첩
	DWORD	Vit;						// 체력
	DWORD	Int;
	DWORD	Wis;						// 심맥

	DWORD	Mana;						// 내력
	DWORD	MaxMana;					// 최대 내력
	EXPTYPE	ExpPoint;						// 경험치
	LEVELTYPE	LevelUpPoint;				// 레벨업포인트		//??????
	MONEYTYPE	Money;						// 돈

	DWORD	PartyID;						// 파티 인덱스 	
	DWORD	Playtime;						// Play Time
	DWORD	LastPKModeEndTime;				// 마지막 살기모드 off시간
	LEVELTYPE MaxLevel;						// 올랐었던 최고 레벨
	char	MunpaCanEntryDate[11];			// can create guild/be member left time 
	char	FamilyCanEntryDate[11];

	DWORD	SkillPoint;
	// 080703 LUJ, 이도류 사용 가능 여부
	BOOL	bUsingTwoBlade;
};

struct MONSTER_TOTALINFO
{
	DWORD Life;
	WORD MonsterKind;
	// 091113 LUJ, 그룹 별칭을 설정할 수 있도록 변경
	DWORD Group;
	MAPTYPE MapNum;
	// 090316 LUJ, 몬스터를 소유한 오브젝트 번호
	DWORD OwnedObjectIndex;
	TCHAR mScriptName[MAX_PATH];
};

struct NPC_TOTALINFO
{
	WORD NpcKind;
	WORD Group;
	MAPTYPE MapNum;
	WORD NpcUniqueIdx;
	WORD NpcJob;				// NPC_ROLE로 바꿔야 함! : taiyo

	// 080822 LYW --- CommonStruct : 소환을 하는 Npc일 경우, NpcRecallInfo.bin의 인덱스를 사용한다.
	DWORD dwRecallNpcIdx ;

	DWORD dwSummonerUserIndex;	// 소환한 사람의 UserIndex
	char szSummonerName[MAX_NAME_LENGTH + 1];

	NPC_TOTALINFO()
	{
		ZeroMemory(this, sizeof(NPC_TOTALINFO));
	}
};

struct STATIC_NPCINFO
{
	MAPTYPE	MapNum;
	WORD	wNpcJob;
	WORD	wNpcUniqueIdx;
	VECTOR3	vPos;
	WORD	wDir;				// 090227 ShinJS --- 캐릭터의 방향 추가
	char NpcName[MAX_NAME_LENGTH+1];
};

struct QUESTNPCINFO
{
	DWORD	dwObjIdx;
	MAPTYPE	wMapNum;
	WORD	wNpcKind;
	char	sName[MAX_NAME_LENGTH+1];
	WORD	wNpcIdx;
	VECTOR3	vPos;
	float	fDir;	
};

struct PET_INFO
{
	DWORD	Index;
	char	Name[MAX_FILE_NAME + 1];
	char	ModelName[MAX_FILE_NAME + 1];
	DWORD	Image;
	DWORD	Sight;
	BYTE	Costume;
	// 090720 ONS 펫의 고정스킬 인덱스
	DWORD	StaticItemIndex;
	float	Scale;
};

struct PET_FRIENDLY_PENALTY
{
	BYTE	Friendly;
	float	Penalty;
};

struct PET_STATUS_INFO
{
	BYTE	Level;
	DWORD	STR;
	DWORD	DEX;
	DWORD	VIT;
	DWORD	INT;
	DWORD	WIS;
};

struct PET_HP_MP_INFO
{
	BYTE	Index;
	BYTE	HP;
	BYTE	MP;
};

struct PET_OBJECT_INFO
{
	DWORD	ID;
	DWORD	ItemDBIdx;
	DWORD	MasterIdx;
	char	MasterName[MAX_NAME_LENGTH+1];
	DWORD	Kind;
	BYTE	Level;
	BYTE	Grade;
	BYTE	SkillSlot;
	WORD	Exp;
	BYTE	Friendly;
	DWORD	HP;
	DWORD	MAXHP;
	DWORD	MP;
	DWORD	MAXMP;
	ePetAI	AI;
	ePetState State;
	ePetType Type;
	DWORD	FriendlyCheckTime;
};

struct ICONBASE
{
	DWORD	dwDBIdx;
	DWORD	wIconIdx;		// 아이템->itemIdx, 무공->skillIdx
	POSTYPE Position;		// 
};

struct ITEM_OPTION
{
	// 080130 LUJ, 옵션 인덱스 대신 아이템 DB 인덱스를 사용
	DWORD	mItemDbIndex;
	// DWORD mIndex;

	// 강화로 얻는 수치
	struct Reinforce
	{
		DWORD	mStrength;
		DWORD	mDexterity;
		DWORD	mVitality;
		DWORD	mIntelligence;
		DWORD	mWisdom;
		DWORD	mLife;
		DWORD	mMana;
		DWORD	mManaRecovery;
		DWORD	mLifeRecovery;
		DWORD	mPhysicAttack;
		DWORD	mPhysicDefence;
		DWORD	mMagicAttack;
		DWORD	mMagicDefence;	
		DWORD	mCriticalRate;
		DWORD	mCriticalDamage;
		DWORD	mMoveSpeed;
		DWORD	mEvade;
		DWORD	mAccuracy;
		char	mMadeBy[ MAX_NAME_LENGTH + 1 ];
	}
	mReinforce;

	// 조합시에 랜덤하게 획득되는 수치
	struct Mix
	{
		DWORD	mStrength;
		DWORD	mIntelligence;
		DWORD	mDexterity;
		DWORD	mWisdom;
		DWORD	mVitality;
		char	mMadeBy[ MAX_NAME_LENGTH + 1 ];
	}
	mMix;

	// 인챈트로 얻는 수치. 이 수치는 계산 가능하다.
	struct Enchant
	{
		DWORD	mIndex;
		BYTE	mLevel;
		char	mMadeBy[ MAX_NAME_LENGTH + 1 ];
	}
	mEnchant;
	
	// 080130 LUJ, 드롭 옵션 추가
	struct Drop
	{
		enum Key
		{
			KeyNone,
			KeyPlusStrength			= 100,
			KeyPlusIntelligence,
			KeyPlusDexterity,
			KeyPlusWisdom,
			KeyPlusVitality,
			KeyPlusPhysicalAttack,
			KeyPlusPhysicalDefence,
			KeyPlusMagicalAttack,
			KeyPlusMagicalDefence,
			KeyPlusCriticalRate,
			KeyPlusCriticalDamage,
			KeyPlusAccuracy,
			KeyPlusEvade,
			KeyPlusMoveSpeed,
			KeyPlusLife,
			KeyPlusMana,
			KeyPlusLifeRecovery,
			KeyPlusManaRecovery,
			KeyPercentStrength		= 200,
			KeyPercentIntelligence,
			KeyPercentDexterity,
			KeyPercentWisdom,
			KeyPercentVitality,
			KeyPercentPhysicalAttack,
			KeyPercentPhysicalDefence,
			KeyPercentMagicalAttack,
			KeyPercentMagicalDefence,
			KeyPercentCriticalRate,
			KeyPercentCriticalDamage,
			KeyPercentAccuracy,
			KeyPercentEvade,
			KeyPercentMoveSpeed,
			KeyPercentLife,
			KeyPercentMana,
			KeyPercentLifeRecovery,
			KeyPercentManaRecovery,
		};

        struct Value
		{
			Key		mKey;
			float	mValue;
		}
		mValue[ 5 ];
	}
	mDrop;
};

struct PlayerStat
{
	struct Value
	{
		float mPlus;
		float mPercent;
		Value() { mPlus = 0; mPercent = 0; }
	};

	Value mStrength;
	Value mDexterity;
	Value mVitality;
	Value mIntelligence;
	Value mWisdom;
	Value mPhysicAttack;
	Value mPhysicDefense;
	Value mMagicAttack;
	Value mMagicDefense;
	Value mMana;
	Value mLife;
	Value mRecoveryLife;
	Value mRecoveryMana;
	Value mCriticalRate;
	Value mCriticalDamage;
	Value mMagicCriticalRate;
	Value mMagicCriticalDamage;
	Value mMoveSpeed;
	Value mAccuracy;
	Value mEvade;
	Value mAddDamage;
	Value mReduceDamage;
};

struct monster_stats
{
	WORD PhysicalAttackMin1;
	WORD PhysicalAttackMax1;
	WORD PhysicalAttackMin2;
	WORD PhysicalAttackMax2;
	WORD AttributeAttackMin1;
	WORD AttributeAttackMax1;
	WORD AttributeAttackMin2;
	WORD AttributeAttackMax2;
	WORD PhysicalDefense;
};

struct ComposeScript
{
	ComposeScript() :
	mSourceSize( 0 )
	{}

	// 080916 LUJ, 
	DWORD mKeyItemIndex;
	// 080916 LUJ, 재료 개수
	DWORD mSourceSize;

	struct Result
	{
		DWORD	mItemIndex;
		BOOL	mIsHidden;
	};

	// 080916 LUJ, 확률 별 구간을 점유하고 있다. 키: 확률
	typedef std::map< float, Result >	ResultMap;
	ResultMap							mResultMap;

	// 080916 LUJ, 결과 중에 아이템이 있는지 조회하는데 쓰인다. 숨겨진 아이템은 여기에 정보가 없다.
	//				그러므로 숨겨진 아이템은 플레이어가 직접 결과로 지정할 수 없다.
	typedef std::set< DWORD >	ResultSet;
	ResultSet					mResultSet;

	// 080916 LUJ, 결과에 부여될 옵션
	// 090122 LUJ, 히든 보너스도 속성을 가지도록 함
	struct Option
	{
		Option()
		{
			ZeroMemory( this, sizeof( *this ) );
		}

		float						mValue;
		ITEM_OPTION::Drop::Value	mHiddenBonus;
	};

	typedef std::map< ITEM_OPTION::Drop::Key, Option >	OptionMap;
	OptionMap											mOptionMap;
};

// 세트 아이템 기본 자료 구조
// 080925 LUJ, ItemManager.h에서 이동시킴
struct SetScript
{
	std::string mName;

	// 세트로 구성되는 아이템
	typedef stdext::hash_set< DWORD >	Item;
	Item								mItem;

	struct Element
	{
		PlayerStat	mStat;

		// 키: 스킬 인덱스, 값: 레벨 증감
		typedef std::map< DWORD, BYTE >	Skill;
		Skill							mSkill;
	};

	// 키: 능력이 발현되는 세트 아이템 구성 개수
	typedef stdext::hash_map< int, Element >	Ability;
	Ability										mAbility;
	// 080916 LUJ, 세트 아이템이 차지할 슬롯 이름을 대체할 이름을 갖고 있는 자료구조
	//				여기에 값이 있으면, 세트를 구성하는 아이템 이름 대신 자료구조에
	//				지정된 이름이 세트 아이템 툴팁에 표시됨
	typedef std::map< EWEARED_ITEM, std::string > SlotNameMap;
	SlotNameMap	mSlotNameMap;
	// 081231 LUJ, 아이템의 속성을 추가해준다
	typedef stdext::hash_map< DWORD, Element >	ItemElementMap;
	ItemElementMap								mItemElementMap;
};

// 080925 LUJ, ItemManager.h에서 이동시킴
struct DissolveScript
{
	typedef struct
	{
		DWORD mItemIndex;
		DWORD mQuantity;
	}
	Result;

	// 분해시 무조건 출현하는 아이템
	typedef std::list< Result > StaticResult;
	StaticResult				mStaticResult;

	// 분해시 확률에 따라 출현하는 아이템. lower_bound()를 쓰므로 hash_map을 쓰면 안된다.
	typedef std::map< DWORD, Result >	DynamicResult;
	DynamicResult						mDynamicResult;

	// 분해가능한 최소 레벨
	DWORD mLevel;
};

// 080925 LUJ, ItemManager.h에서 이동시킴
struct ApplyOptionScript
{
	struct
	{
		int	mMin;
		int mMax;
	}
	mLevel;

	// 키: 확률, 값: 부여될 옵션 개수
	typedef std::map< float, DWORD >		OptionSizeMap;
	OptionSizeMap							mOptionSizeMap;
	// 적용될 아이템 종류
	typedef stdext::hash_set< LONGLONG >	TargetTypeSet;
	TargetTypeSet							mTargetTypeSet;

	struct Option
	{
		ITEM_OPTION::Drop::Key	mKey;
		float					mBeginValue;
		float					mEndValue;
	};

	// 확률마다 선택될 옵션 종류와 수치 범위를 담고 있다. 키: 선택 확률
	typedef std::map< float, Option >	OptionTypeMap;
	OptionTypeMap						mOptionTypeMap;
};

struct DropOptionScript
{
	// 키: 확률, 값: 인챈트 수치
	typedef std::map< float, DWORD > EnchantLevelMap;
	EnchantLevelMap	mEnchantLevelMap;
	// 키: 확률, 값: 부여될 옵션 개수
	typedef std::map< float, DWORD > OptionSizeMap;
	OptionSizeMap mOptionSizeMap;

	typedef ApplyOptionScript::Option Option;
	// 확률마다 선택될 옵션 종류와 수치 범위를 담고 있다
	// 키: 선택 확률
	typedef std::map< float, Option > OptionMap;
	// 키: 아이템 장착 레벨별로 적용할 옵션에 대한 확률 맵을 갖고 있다
	typedef std::map< DWORD, OptionMap > LevelMap;
	LevelMap mLevelMap;
};

struct ReinforceScript
{
	enum eType
	{
		eTypeNone,
		eTypeStrength,
		eTypeDexterity,
		eTypeVitality,
		eTypeIntelligence,
		eTypeWisdom,
		eTypeLife,
		eTypeMana,
		eTypeLifeRecovery,
		eTypeManaRecovery,
		eTypePhysicAttack,
		eTypePhysicDefence,
		eTypeMagicAttack,
		eTypeMagicDefence,
		eTypeMoveSpeed,
		eTypeEvade,
		eTypeAccuracy,
		eTypeCriticalRate,
		eTypeCriticalDamage,
		eTypeMax
	};

	eType	mType;		// 강화로 상승되는 능력 종류
	DWORD	mMax;		// 강화로 올릴 수 있는 최대 수치
	float	mBias;		// 강화를 위한 값
	BOOL	mForRare;	// 레어 아이템 강화 가능 여부

	// 해당 재료로 강화할 수 있는 아이템 종류(장착 슬롯으로 구분)
	typedef stdext::hash_set< EWEARED_ITEM > Slot;
	Slot mSlot;
};

// 080929 LUJ, 강화 보조 스크립트
struct ReinforceSupportScript
{
	ReinforceSupportScript() :
	mBias( 0 )
	{
		ZeroMemory( &mValueRange, sizeof( mValueRange ) );
		ZeroMemory( &mBonusRange, sizeof( mBonusRange ) );
	}

	struct Range
	{
		float mMin;
		float mMax;
	};
	// 080929 LUJ, 강화 수치 범위
	Range	mValueRange;
	// 080929 LUJ, 보너스 수치 범위
	Range	mBonusRange;
	// 080929 LUJ, 재료 1개 당 적용되는 수치
	float	mBias;
	// 080929 LUJ, 사용 가능한 재료. 키: 아이템 번호
	typedef std::set< DWORD >	MaterialSet;
	MaterialSet					mMaterialSet;
};
// 080929 LUJ, 강화 보조 스크립트 컨테이너. 키: 보조 아이템 인덱스
typedef stdext::hash_map< DWORD, ReinforceSupportScript > ReinforceSupportScriptMap;

// 070810 웅주, 인챈트 스크립트
// 080925 LUJ, ItemManager.h에서 이동시킴
struct EnchantScript
{
	// 인챈트를 부여하는 아이템(인챈트 스크롤 등)이 가지는 인덱스
	DWORD mItemIndex;

	enum eType
	{
		eTypeNone,
		eTypeStrength,
		eTypeDexterity,
		eTypeVitality,
		eTypeIntelligence,
		eTypeWisdom,
		eTypeLife,
		eTypeMana,
		eTypeLifeRecovery,
		eTypeManaRecovery,
		eTypePhysicAttack,
		eTypePhysicDefence,
		eTypeMagicAttack,
		eTypeMagicDefence,
		eTypeMoveSpeed,
		eTypeEvade,
		eTypeAccuracy,
		eTypeCriticalRate,
		eTypeCriticalDamage,
		eTypeMax
	};

	// 인챈 가능한 아이템 착용 제한 레벨 범위
	struct
	{
		DWORD mMin;
		DWORD mMax;
	}
	mItemLevel;

	DWORD mEnchantMaxLevel;	// 최대로 인챈트할 수 있는 단계

	typedef stdext::hash_set< eType >	Ability;
	Ability								mAbility;
	// 해당 재료로 인챈트할 수 있는 아이템 종류(장착 슬롯으로 구분)
	typedef stdext::hash_set< EWEARED_ITEM > Slot;
	Slot mSlot;
};

struct EnchantProtection
{
	struct Data
	{
		struct Range
		{
			DWORD mMin;
			DWORD mMax;
		};
		Range mEnchantLevelRange;
		Range mItemLimitLevelRange;
		DWORD mMinimumEnchantLevel;
		int mAddedPercent;
		typedef std::map< float, int > BonusMap;
		BonusMap mPlusBonusMap;
		BonusMap mMinusBonusMap;
	};

	typedef stdext::hash_map< DWORD, Data >	ItemMap;
	ItemMap	mItemMap;
};

// 080925 LUJ, ItemManager.h에서 이동시킴
struct MixSetting
{
	enum eAbility
	{
		eAbilityNone,
		eAbilityStrength,
		eAbilityDexterity,
		eAbilityIntelligence,
		eAbilityVitality,
		eAbilityWisdom,
	};

	// 각 구간 별 범위값이 들어있다. 힘 20%, 민첩이 80%라 하자. 이때 100까지의 랜덤값을 얻어
	// map의 lower_bound() 메소드를 이용하여 적용할 능력치를 얻어낸다.
	typedef std::map< float, eAbility >	Ability;
	typedef std::map< eStatusKind, Ability > Equippment;
	Equippment mEquippment;

	typedef struct Range
	{
		DWORD mMin;
		DWORD mMax;
	};

	// 키: 레벨 값. 값: 적용할 수치 범위
	typedef std::map< DWORD, Range >	RangeMap;
	RangeMap							mRangeMap;
};

// 조합 아이템 기본 자료구조
// 080925 LUJ, ItemManager.h에서 이동
struct ItemMixResult
{
	DWORD	mItemIndex;
	DWORD	mBaseMaterial;				// 091012 ShinJS --- 기본 재료 아이템 인덱스 추가
	WORD	mRequiredLevel;
	DWORD	mMoney;
	float	mSuccessRate;

	// 키: 아이템 인덱스, 값: 소요 개수
	typedef stdext::hash_map< DWORD, DWORD >	Material;
	Material									mMaterial;
};

// 080925 LUJ, ItemManager.h에서 이동
typedef std::list< ItemMixResult >	MixScript;

// 080925 LUJ, 조합 보조 스크립트
struct MixSupportScript
{
	MixSupportScript() :
	mBonusSucessRate( 0 )
	{}

	// 080925 LUJ, 성공 확률 보너스
	float				mBonusSucessRate;
	// 080925 LUJ, 옵션을 선택하는 확률 구간. 조합 보조 아이템을 사용할 때는 이 확률 구간을 사용한다
	typedef	MixSetting::Ability	Ability;
	Ability						mAbiltyMap;
};

// 081203 LUJ, 기존 스킬 리스트의 기능을 확장할 수 있는 스크립트
struct SkillScript
{
	enum Type
	{
		TypeNone,
		TypeStrength,
		TypeDexterity,
		TypeVitality,
		TypeIntelligence,
		TypeWisdom,
		TypeLife,
		TypeMana,
		TypeLifeRecovery,
		TypeManaRecovery,
		TypePhysicAttack,
		TypePhysicDefence,
		TypeMagicAttack,
		TypeMagicDefence,
		TypeMoveSpeed,
		TypeEvade,
		TypeAccuracy,
		TypeCriticalRate,
		TypeCriticalDamage,
	};

	struct Value
	{
		Type	mType;
		float	mPlus;
		float	mPercent;

		Value()
		{
			ZeroMemory( this, sizeof( *this ) );
		}
	};
	typedef std::list< Value >	ValueList;
	// 081203 LUJ, 스탯에 적용될 보너스가 담겨있다
	ValueList					mStatusBonusList;
	// 081203 LUJ, 유지 시간에 적용될 보너스가 담겨있다
	ValueList					mDurationBonusList;

	struct Buff
	{
		eStatusKind	mKind;
		float		mValue;

		Buff()
		{
			ZeroMemory( this, sizeof( *this ) );
		}
	};
	// 081203 LUJ, 적용할 속성 및 값이 담겨있다
	typedef std::list< Buff >	BuffList;
	BuffList					mBuffList;
	// 090226 LUJ, 스킬 검사 여부
	BOOL mIsNeedCheck;

	// 090630 ONS 스킬 그룹 속성 추가
	DWORD mSkillGroupNum;

	SkillScript() :
	mIsNeedCheck( TRUE ),
	mSkillGroupNum( 0 )
	{}
};
// 081203 LUJ, 스킬 스크립트를 저장하는 자료 구조. 키: 스킬 번호
typedef stdext::hash_map< DWORD, const SkillScript* > SkillScriptMap;

// 090316 LUJ, 탈것 스크립트
struct VehicleScript
{
	// 090316 LUJ, MonsterList.bin에서 정보를 읽어온다
	WORD mMonsterKind;
	// 090316 LUJ, 좌석 정보
	struct Seat
	{
		enum { MaxSize = 20 };
		BOOL mIsEnableControl;
		char mAttachedName[ MAX_NAME_LENGTH ];
	}
	mSeat[ Seat::MaxSize ];
	// 090316 LUJ, 시트 개수
	DWORD mSeatSize;
	// 090316 LUJ, 탈것에서의 플레이어 동작
	struct Motion
	{
		DWORD mMount;				// 탑승시의 플레이어 Motion Index

		// 091109 ShinJS --- Beff 적용
		enum
		{
			FixObjectPlay,
			FixObjectStop,
			MoveObjectIdle,
			MoveObjectWalk,
			MoveObjectRun,
		};

		typedef std::pair<DWORD, std::string> MotionInfo;
		std::multimap< DWORD, std::string > mBeffFileList;
	}
	mMotion;
	// 090316 LUJ, 탑승 조건
	struct Condition
	{
		// 090316 LUJ, 탑승 시 소요되는 금액
		MONEYTYPE mTollMoney;
		LEVELTYPE mPlayerMinimumLevel;
		LEVELTYPE mPlayerMaximumLevel;
		// 090316 LUJ, 소환 시 플레이어와 탈것이 멀어질 수 있는 최대 거리
		float mSummonRange;
		enum { BuffMaxSize = 10 };
		DWORD mBuffSkillIndex[ BuffMaxSize ];

		Condition() :
		mTollMoney(0),
		mPlayerMinimumLevel(0),
		mPlayerMaximumLevel(SHRT_MAX),
		mSummonRange(0)
		{
			ZeroMemory(
				mBuffSkillIndex,
				sizeof(mBuffSkillIndex));
		}
	}
	mCondition;
	// 090316 LUJ, 초기화를 수행할 생성자
	VehicleScript()
	{
		mMonsterKind = 0;
		mSeatSize = 0;
		mMotion.mMount = 0;
		ZeroMemory( mSeat, sizeof( Seat ) * Seat::MaxSize );
	}
};

// 081119 NYJ - 요리
struct stIngredient
{
	DWORD dwItemIdx;
	WORD wQuantity;

	stIngredient()
	{
		dwItemIdx = wQuantity = 0;
	}
};

struct stRecipeInfo
{
	DWORD dwRecipeIdx;
	DWORD dwExpertPointMin;
	DWORD dwExpertPointMax;
	DWORD dwRemainTime;
	DWORD dwFoodItemIdx;
	stIngredient Ingredients[MAX_INGREDIENT_LIST];

	stRecipeInfo()
	{
		dwRecipeIdx = dwExpertPointMin = dwExpertPointMax = dwRemainTime = dwFoodItemIdx = 0;
		memset(Ingredients, 0, sizeof(stIngredient)*MAX_INGREDIENT_LIST); 
	}
};

struct stRecipeLv4Info	// 달인레시피정보
{
	DWORD dwRecipeIdx;
	DWORD dwRemainTime;

	stRecipeLv4Info()
	{
		dwRecipeIdx = dwRemainTime = 0;
	}
};


struct stMonsterEventRewardItemInfo
{
	WORD wMonsterType;
	DWORD dwItemIndex;

	stMonsterEventRewardItemInfo()
	{
		wMonsterType = 0;
		dwItemIndex = 0;
	}
};


// 090316 NYJ - 하우징
struct stGeneralFunitureInfo
{
	DWORD		dwItemIndex;
	VECTOR3		vWorldPos;
	float		fAngle;

	stGeneralFunitureInfo()
	{
		memset(this, 0, sizeof(stGeneralFunitureInfo));
	}
};

//집모양 외관하우스 Npc정보 
struct stDynamicHouseNpcInfo
{
	DWORD		dwRankTypeIndex;		//랭킹(1~)*100 + 외관타입(1~99)   
	DWORD		dwNpcKind;				//Npc 카인드
	float		fWorldPosX;		
	float		fWorldPosZ;		
	float		fAnlge;
	float		fDistance;				//입장요청 유효거리

	stDynamicHouseNpcInfo()
	{
		memset(this, 0, sizeof(stDynamicHouseNpcInfo) );
	}
};

struct stDynamicHouseNpcMapInfo
{
	DWORD MapIndex;
	CYHHashTable<stDynamicHouseNpcInfo>	pDynamicHouseNpcList;

	stDynamicHouseNpcMapInfo()
	{
		Clear();
		pDynamicHouseNpcList.Initialize(100);
	}

	void Clear()
	{
		MapIndex = 0;
		stDynamicHouseNpcInfo* pNpcInfo = NULL;
		pDynamicHouseNpcList.SetPositionHead();	
		while((pNpcInfo = pDynamicHouseNpcList.GetData() )!= NULL)
		{
			SAFE_DELETE(pNpcInfo);
		}
		pDynamicHouseNpcList.RemoveAll();
	}

	~stDynamicHouseNpcMapInfo()
	{
		Clear();
	}
};

struct stHousingSettingInfo
{
	float fDecoPoint_Weight;	//꾸미기 점수 가중치
	float fRecommend_Weight;	//추천수 점수 가중치
	float fVisitor_Weight;		//방문자 점수 가중치 

	DWORD dwGeneralFunitureNumArr[MAX_HOUSE_EXTEND_LEVEL];
	stGeneralFunitureInfo* pGeneralFunitureList[MAX_HOUSE_EXTEND_LEVEL];

	DWORD	dwStarPoint_Nomal;
	DWORD	dwStarPoint_Ranker;

	DWORD	dwRankingDay;

	CYHHashTable<stDynamicHouseNpcMapInfo>	m_DynamicHouseNpcMapInfoList;	//동적생성 하우스Npc 맵정보 리스트 

	stHousingSettingInfo()
	{
		memset(pGeneralFunitureList , 0 , sizeof(stGeneralFunitureInfo*) * MAX_HOUSE_EXTEND_LEVEL );
		m_DynamicHouseNpcMapInfoList.Initialize( 100 );
		Clear();
	}

	void Clear()
	{
		for(int i=0; i<MAX_HOUSE_EXTEND_LEVEL ;i++)
			SAFE_DELETE_ARRAY(pGeneralFunitureList[i]);

		dwStarPoint_Nomal = dwStarPoint_Ranker = dwRankingDay = 0;
		memset(dwGeneralFunitureNumArr , 0 , sizeof(DWORD) * MAX_HOUSE_EXTEND_LEVEL );

		stDynamicHouseNpcMapInfo* pMapInfo;
		m_DynamicHouseNpcMapInfoList.SetPositionHead();	
		while((pMapInfo = m_DynamicHouseNpcMapInfoList.GetData())!= NULL)
		{
			pMapInfo->Clear();
			SAFE_DELETE(pMapInfo);
		}
		m_DynamicHouseNpcMapInfoList.RemoveAll();
	}

	~stHousingSettingInfo()
	{ 
		Clear();
	}
};

struct stFurnitureFileInfo
{
	DWORD dwFurnitureIndex;						//가구인덱스
	char szNameCHX[MAX_HOUSING_NAME_LENGTH];	//CHX [애니메이션 보유]
	char szNameCHR[MAX_HOUSING_NAME_LENGTH];	//CHR [Material 보유]
	char szNameMOD[MAX_HOUSING_NAME_LENGTH];	//MOD [모델만 보유]

	stFurnitureFileInfo()
	{
		dwFurnitureIndex = 0;
		strcpy(szNameCHX, "");
		strcpy(szNameCHR, "");
		strcpy(szNameMOD, "");
	}
};

struct stFurniture	// DB,서버,클라가 주고받는 가구구조체
{
	DWORD dwOwnerUserIndex;		// 소유주의 UserIndex
	DWORD dwFurnitureIndex;		// 가구인덱스
	DWORD dwLinkItemIndex;		// 아이템인덱스
	DWORD dwObjectIndex;		// 고유인덱스:서버->클라전송시 고유Index 할당
	int nMaterialIndex;			// 현재사용중인 MaterialIndex
	VECTOR3 vPosition;			// 위치
	float fAngle;				// 각도
	BYTE wCategory;				// 내집창고 - 카테고리
	BYTE wSlot;					// 내집창고 - 슬롯
	BYTE wState;				// 내집창고 - 상태(eHOUSEFURNITURE_STATE_UNKEEP, eHOUSEFURNITURE_STATE_KEEP, eHOUSEFURNITURE_STATE_INSTALL, eHOUSEFURNITURE_STATE_UNINSTALL)
	BOOL bNotDelete;			// 기본설치품목 (삭제불가)
	DWORD dwRemainTime;			// 남은시간

	DWORD dwRidingPlayer[MAX_HOUSE_ATTATCH_SLOT];		// 탑승슬롯 (탑승자 CharIndex보관)

	void Clear()
	{
		memset(this, 0, sizeof(stFurniture));
		nMaterialIndex = -1;
	}

	stFurniture()
	{
		Clear();
	}
};

struct stFunitureInfo	// 서버,클라가 스크립트에서 읽어오는 정보
{
	DWORD dwFurnitureIndex;		// 가구인덱스
	char szName[MAX_HOUSING_NAME_LENGTH];
	DWORD dwItemIndex;			// 보관전 ItemIndex
	DWORD dwActionIndex;		// 액션Index
	DWORD dwDecoPoint;			// 꾸미기포인트
	DWORD dwRemainTime;			// 사용기간
	BOOL bStackable;			// 위에쌓기여부
	BOOL bRotatable;			// 회전여부
	BOOL bTileDisable;			// 이동불가
	BOOL bMeshTest;				// Mesh충돌 Test
	BYTE byMaxMaterialNum;		// 최대 Matrial 개수
	BYTE byModelFileFlag;		// 모델파일종류 0: MOD, 1: CHX, 2:CHR
	char szModelFileName[256];	// 모델파일 이름
	BYTE byAttachBoneNum;		// 보유 AttachBone갯수
	BOOL bHasAlpha;				// 자체알파 보유여부 (알파소팅시 렌더링 순서 여부때문에 클라이언트에서만 쓴다)

	stFunitureInfo()
	{
		memset(this, 0, sizeof(stFunitureInfo));
	}
};

struct stHouseBonusInfo	// 꾸미기효과
{
	DWORD dwBonusIndex;			// 보너스인덱스 ( 10000~19999 기본보너스 , 20000 ~ 29999 랭커용보너스) 
	DWORD dwDecoPoint;			// 꾸미기 포인트 				
	char szName[MAX_HOUSING_NAME_LENGTH];
	DWORD dwTooltipIndex;		//보너스 툴팁인덱스
	BYTE byKind;				//0 범용보너스 , 1랭커용 보너스 	
	DWORD dwBonusType;			//0 버프타입 ,1 맵이동  
	DWORD dwBonusTypeValue;		//스킬인덱스 , 맵번호  
	BYTE byTargetType;			//0 집주인	 , 집안에있는 사람
	DWORD dwUsePoint;			//사용포인트 

	stHouseBonusInfo()
	{
		memset(this, 0, sizeof(stHouseBonusInfo));
	}
};

struct stHouseInfo
{
	DWORD dwOwnerUserIndex;
	char szHouseName[MAX_HOUSING_NAME_LENGTH + 1];
	BYTE ExteriorKind;
	BYTE ExtendLevel;
	DWORD dwTotalVisitCount;
	DWORD dwDailyVisitCount;
	DWORD dwHousePoint;			// DecoPoint + VotePoint + VisitPoint
	DWORD dwDecoUsePoint;

	void Clear()
	{
		memset(this, 0, sizeof(stHouseInfo));
	}

	stHouseInfo()
	{
		Clear();
	}
};

// 서버,클라가 스크립트에서 읽어오는 액션그룹정보
struct stHouseActionGroupInfo
{
	DWORD dwGroupIndex;		//그룹인덱스 
	BYTE  byActionNum;		//보유액션수 
	DWORD dwActionIndexList[MAX_HOUSE_ACTION_SLOT];

	stHouseActionGroupInfo()
	{
		memset(this, 0, sizeof(stHouseActionGroupInfo));
	}
};

// 서버,클라가 스크립트에서 읽어오는 액션정보
struct stHouseActionInfo
{
	DWORD	dwActionIndex;						//액션인덱스
	char	szName[MAX_HOUSING_NAME_LENGTH];	//액션 이름
	DWORD	dwPlayer_MotionIndex;				//플레이어 모션인덱스
	DWORD	dwPlayer_EffectIndex;				//플레이어 이펙트인덱스

	DWORD	dwFurniture_MotionIndex;			//가구모션인덱스
	DWORD	dwFurniture_EffectIndex;			//가구이펙트인덱스
	BOOL	bUseAttach;							//어태치여부 							

	DWORD	dwActionType;						//액션타입
	DWORD	dwActionValue;						//액션타입에 따른 추가 값

	stHouseActionInfo()
	{
		memset(this, 0, sizeof(stHouseActionInfo));
	}
};

struct stHouseRankNPC
{
	DWORD dwHouseUserIndex;
	DWORD dwNpcIndex;

	stHouseRankNPC()
	{
		memset(this, 0, sizeof(stHouseRankNPC));
	}
};

struct stHouseRank
{
	DWORD	dwRank_1_UserIndex;
	char	szRank_1_HouseName[MAX_HOUSING_NAME_LENGTH + 1];
	BYTE	byRank_1_UserExterioKind;
	DWORD	dwRank_1_HousePoint;
	DWORD	dwRank_2_UserIndex;
	char	szRank_2_HouseName[MAX_HOUSING_NAME_LENGTH + 1];
	BYTE	byRank_2_UserExterioKind;
	DWORD	dwRank_2_HousePoint;
	DWORD	dwRank_3_UserIndex;
	char	szRank_3_HouseName[MAX_HOUSING_NAME_LENGTH + 1];
	BYTE	byRank_3_UserExterioKind;
	DWORD	dwRank_3_HousePoint;

	WORD	wCalcYear;
	WORD	wCalcMonth;
	WORD	wCalcDay;

	stHouseRank()
	{
		memset(this, 0, sizeof(stHouseRank));
	}
};

struct stHouse
{
	// DB저장필요
	stHouseInfo	HouseInfo;

	//서버에서 따로 옴 
	DWORD m_dwCategoryNum[MAX_HOUSING_CATEGORY_NUM];
	DWORD m_dwFurnitureList[MAX_HOUSING_CATEGORY_NUM][MAX_HOUSING_SLOT_NUM];

	// 런타임시 필요
	DWORD dwOwnerCharIndex;		// 방문중인 주인의 캐릭터Index;
	DWORD dwDecoPoint;			// 현재설치된 오브젝트의 꾸미기점수
	DWORD dwCurDoorIndex;		// 현재설치된 문의 ObjectIndex
	DWORD dwChannelID;
	DWORD dwJoinPlayerNum;
	CYHHashTable<stFurniture>	pFurnitureList;
	CYHHashTable<stFurniture>	pNotDeleteFurnitureList;	// pFurnitureList에 등록되는 객체중 기본설치물만 등록. 메모리중복해제조심(pFurnitureList만 해제하면 됨)
	VECTOR3 vStartPos;

	void Clear()
	{
		HouseInfo.Clear();

		ZeroMemory(m_dwCategoryNum, sizeof(m_dwCategoryNum));
		ZeroMemory(m_dwFurnitureList, sizeof(m_dwFurnitureList));
		dwOwnerCharIndex = dwDecoPoint = dwChannelID = dwJoinPlayerNum = 0;
		dwCurDoorIndex = 0;

		pFurnitureList.RemoveAll();
		pNotDeleteFurnitureList.RemoveAll();
	}

	stHouse()
	{
		Clear();
	}

	~stHouse()
	{
		stFurniture* pFurniture;
		pFurnitureList.SetPositionHead();
		while((pFurniture = pFurnitureList.GetData())!= NULL)
		{
			SAFE_DELETE(pFurniture);
		}
	}
};

struct stDungeonKey
{
	DWORD dwIndex;
	WORD  wMapNum;
	eDIFFICULTY difficulty;
	DWORD dwEntranceNpcJobIndex;
	DWORD dwPosX;
	DWORD dwPosZ;
	DWORD dwTooltipIndex;

	WORD wMinLevel;		// 최소레벨
	WORD wMaxLevel;		// 최대레벨
	WORD wMaxCount;		// 최대횟수
	WORD wDelayMin;		// 입장딜레이시간(분)

	stDungeonKey()
	{
		ZeroMemory(this, sizeof(stDungeonKey));
	}
};

struct stWarpInfo
{
	DWORD dwIndex;
	WORD  wMapNum;
	WORD  wNpcJob;
	char  name[MAX_MONSTER_NAME_LENGTH + 1];
	WORD  wRadius;
	WORD  wCurPosX;
	WORD  wCurPosZ;
	WORD  wDstPosX;
	WORD  wDstPosZ;
	BOOL  bActive;

	stWarpInfo()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stWarpInfo));
	}
};

struct stWarpState
{
	DWORD dwIndex;
	DWORD dwObjectIndex;
	BOOL  bActive;

	stWarpState()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stWarpState));
	}
};

struct stSwitchNpcInfo
{
	WORD  wIndex;
	WORD  wMapNum;
	WORD  wNpcJob;
	char  name[MAX_NAME_LENGTH + 1];
	WORD  wRadius;
	WORD  wPosX;
	WORD  wPosZ;
	BOOL  bActive;

	stSwitchNpcInfo()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stSwitchNpcInfo));
	}
};

struct stSwitchNpcState
{
	DWORD dwObjectIndex;
	WORD  wIndex;
	WORD  wEffectIndex;
	BOOL  bActive;

	stSwitchNpcState()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stSwitchNpcState));
	}
};

struct stBossMonsterState
{
	DWORD	dwObjectIndex;
	WORD	wIndex;
	WORD	wReturnPosX;
	WORD	wReturnPosZ;

	stBossMonsterState()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stBossMonsterState));
	}
};

#define MAX_DUNGEON_SWITCH		40
#define MAX_DUNGEON_WARP		40

struct stDungeon
{
	DWORD dwIndex;
	DWORD dwPartyIndex;
	DWORD dwChannelID;
	DWORD dwStartTime;
	DWORD dwPoint;
	DWORD dwJoinPlayerNum;
	eDIFFICULTY difficulty;

	int			mSwitch[MAX_DUNGEON_SWITCH];
	WORD		m_WarpEmptyPos;
	WORD		m_SwitchNpcEmptyPos;

	DWORD		m_TimerAlias;
	stWarpState			m_WarpState[MAX_DUNGEON_WARP];
	stSwitchNpcState	m_SwitchNpcState[MAX_DUNGEON_SWITCH];

	stBossMonsterState	m_CurBossMonsterState;

	stDungeon()
	{
		Clear();
	}

	~stDungeon()
	{
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(stDungeon));
	}

	stWarpState* GetWarp(DWORD dwIndex)
	{
		for(WORD i=0; i<MAX_DUNGEON_WARP; i++)
		{
			if(m_WarpState[i].dwIndex == dwIndex)
				return &m_WarpState[i];
		}

		return NULL;
	}

	stSwitchNpcState* GetSwitchNpc(WORD wIndex)
	{
		for(WORD i=0; i<MAX_DUNGEON_SWITCH; i++)
		{
			if(m_SwitchNpcState[i].wIndex == wIndex)
				return &m_SwitchNpcState[i];
		}
		return NULL;
	}
};

struct SKILL_BASE
{
	DWORD dwDBIdx;
	DWORD wSkillIdx;
	LEVELTYPE Level;
	LEVELTYPE mLearnedLevel;
};

enum ITEM_TIMEKIND
{
	eKIND_REALTIME = 0,		// 실제 시간 기준으로 계속 시간을 처리.
	eKIND_PLAYTIME ,		// 게임을 플레이 했을 때 시간을 처리.
} ;

//////////////////////////////////////////////////////////////////////////

// 071123 KTH --- CommonStruct : 아이템의 봉인 상태 enum 추가.
enum ITEM_SEAL_TYPE
{
	eITEM_TYPE_SEAL_NORMAL = 0,
	eITEM_TYPE_SEAL,
	eITEM_TYPE_UNSEAL,
	eITEM_TYPE_GET_UNSEAL,
};

// 100223 pdy 아이템 보관 플레그 확장 작업 ( 개인 창고만 보관가능 추가 )
enum ITEM_DEPOSIT_TYPE
{
	eITEM_TYPE_DEPOSIT_NONE = 0,				// 보관불가능
	eITEM_TYPE_DEPOSIT_ALL,						// 모든 보관가능
	eITEM_TYPE_DEPOSIT_ONLY_STORAGE,			// 개인창고만 가능
};

struct ITEMBASE : public ICONBASE
{
	DURTYPE Durability;
	POSTYPE	QuickPosition;
	ITEMPARAM ItemParam;
	ITEM_SEAL_TYPE nSealed;
	int nRemainSecond;
	DWORD LastCheckTime;
};

struct SLOTINFO
{
	BOOL	bLock;
	WORD	wState;
};

struct ITEM_TOTALINFO
{
	ITEMBASE Inventory[SLOT_MAX_INVENTORY_NUM];
	ITEMBASE WearedItem[eWearedItem_Max];
};

struct ScriptCheckValue
{
	ScriptCheckValue()
	{
		ZeroMemory( this, sizeof( *this ) );
	}

	LONGLONG mValue;
};

struct PARTY_MEMBER
{
	DWORD dwMemberID;
	BOOL bLogged;
	BYTE LifePercent;
	BYTE ManaPercent;
	char Name[MAX_NAME_LENGTH+1];
	LEVELTYPE Level;
	VECTOR3 mPosition;
	// 091127 LUJ, 접속한 맵 번호
	MAPTYPE mMapType;
	BYTE mJobGrade;
	BYTE mJob[MAX_JOB_GRADE];
	BYTE mRace;
};

struct PARTY_INFO : public MSGBASE 
{
	DWORD PartyDBIdx;
	BYTE Option;
	BYTE SubOption;
	PARTY_MEMBER Member[MAX_PARTY_LISTNUM];
};

struct SEND_PARTY_MEMBER_INFO : public MSGBASE 
{
	PARTY_MEMBER MemberInfo;
	DWORD PartyID;
	BOOL mIsMaster;
};

struct CHARACTERMAKEINFO : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];			// 캐릭터 이름
	DWORD	UserID;
	BYTE	RaceType;
	BYTE	SexType;
	BYTE	HairType;
	BYTE	FaceType;
	BYTE	JobType;
	BYTE    StandingArrayNum;				// pjs 케릭터의 위치 순서 [5/22/2003]
	BOOL	bDuplCheck;
};

struct MSG_INT : public MSGBASE
{
	int nData;
};

struct MSG_INT2 : public MSGBASE
{
	int nData1;
	int nData2;
};

struct MSG_DWORD : public MSGBASE
{
	DWORD dwData;
};

// 080602 LYW --- CommonStruct : DWORDEX 타입의 메시지 추가.
struct MSG_DWORDEX : public MSGBASE
{
	DWORDEX dweData ;
} ;

struct MSG_DWORDEX2 : public MSGBASE
{
	DWORDEX dweData1 ;
	DWORDEX dweData2 ;
} ;

struct MSG_DWORDEX3 : public MSGBASE
{
	DWORDEX dweData1 ;
	DWORDEX dweData2 ;
	DWORDEX dweData3 ;
} ;

struct MSG_DWORDEX4 : public MSGBASE
{
	DWORDEX dweData1 ;
	DWORDEX dweData2 ;
	DWORDEX dweData3 ;
	DWORDEX dweData4 ;
} ;

struct MSG_DWORD2 : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
};

struct MSG_DWORDBYTE : public MSGBASE
{
	DWORD dwData;
	BYTE bData;
};

struct MSG_DWORD2BYTE : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	BYTE bData;
};

struct MSG_DWORD2INT : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	int nData;
};

struct MSG_DWORDBYTE2 : public MSGBASE
{
	DWORD dwData;
	BYTE bData1;
	BYTE bData2;
};

struct MSG_DWORD3 : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;
};

struct MSG_DWORD4 : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;
	DWORD dwData4;
};

struct MSG_DWORD5 : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;
	DWORD dwData4;
	DWORD dwData5;
};

struct MSG_DWORD6 : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;
	DWORD dwData4;
	DWORD dwData5;
	DWORD dwData6;
};

struct MSG_DWORD3BYTE2 : public MSGBASE
{
	DWORD	dwData1;
	DWORD	dwData2;
	DWORD	dwData3;
	BYTE	bData1;
	BYTE	bData2;
};

struct MSG_WORD : public MSGBASE
{
	WORD wData;
};

struct MSG_WORD2 : public MSGBASE
{
	WORD wData1;
	WORD wData2;
};
struct MSG_WORD3 : public MSGBASE
{
	WORD wData1;
	WORD wData2;
	WORD wData3;
};
struct MSG_WORD4 : public MSGBASE
{
	WORD wData1;
	WORD wData2;
	WORD wData3;
	WORD wData4;
};

struct MSG_DWORD_WORD : public MSGBASE
{
	DWORD dwData;
	WORD wData;
};

struct MSG_DWORD_NAME : public MSGBASE
{
	DWORD dwData;
	char Name[MAX_NAME_LENGTH+1];
};

struct MSG_DWORD2_NAME : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	char Name[MAX_NAME_LENGTH+1];
};

// 091204 ONS 농장 세금납부 정보전달을 위해 메세지추가.
struct MSG_DWORD3_NAME : public MSGBASE
{
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;
	char Name[MAX_NAME_LENGTH+1];
};

struct MSG_BYTE : public MSGBASE
{
	BYTE bData;
};

// 070416 LYW --- CommonStruct : Add struct for two byte.
struct MSG_BYTE2 : public MSGBASE
{
	BYTE bData1 ;
	BYTE bData2 ;
} ;
// 081011 LYW --- CommonStruct : Add struct for three byte.
struct MSG_BYTE3 : public MSG_BYTE2
{
	BYTE bData3 ;
} ;

struct MSG_BYTE4 : public MSG_BYTE3
{
	BYTE bData4 ;
} ;

struct MSG_BYTE5 : public MSG_BYTE4
{
	BYTE bData5 ;
} ;

struct MSG_GUILDNOTICE : public MSGBASE
{
	DWORD dwGuildId;
	char Msg[MAX_GUILD_NOTICE+1];
	int GetMsgLength() { return sizeof(MSG_GUILDNOTICE) - MAX_GUILD_NOTICE + strlen(Msg); }
};

struct TESTMSG : public MSGBASE
{
	char Msg[MAX_CHAT_LENGTH+1];
	int GetMsgLength() { return sizeof(TESTMSG) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct MSG_USER_ADD_ITEM : public MSGBASE
{
	char	CharacterName[32];
	DWORD	dwIconIdx;
	//int GetMsgLength() { return sizeof(MSG_USER_ADD_ITEM) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct TESTMSGID : public MSGBASE
{
	DWORD dwSenderID;
	char Msg[MAX_CHAT_LENGTH+1];
	int GetMsgLength() { return sizeof(TESTMSGID) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct MSG_EVENT_MONSTERREGEN : public MSGBASE
{
	WORD	MonsterKind;
	BYTE	cbMobCount;		//mob count
	WORD	wMap;			//map
	BYTE	cbChannel;		//channel	= 0:all
	VECTOR3 Pos;
	WORD	wRadius;
	DWORD	ItemID;
	DWORD	dwDropRatio;	//item drop ratio
//	BYTE	bBoss;			//필요한가?
};

struct MSG_EVENT_NPCSUMMON : public MSGBASE
{
	WORD	NpcKind;
	BYTE	cbNpcCount;		//mob count
	WORD	wMap;			//map
	BYTE	cbChannel;		//channel	= 0:all
	VECTOR3 Pos;
	WORD	wRadius;
};


struct MSG_FAME : public MSGBASE
{
	FAMETYPE Fame;
};


struct MSG_LOGIN_SYN : public MSGBASE
{
	DWORD AuthKey;
#ifdef _TW_LOCAL_
	char id[21];
	char pw[21];
#else
	char id[MAX_NAME_LENGTH+1];
	char pw[MAX_NAME_LENGTH+1];
#endif
	char Version[16];

	// 080109 LYW --- CommonStruct : 보안비밀번호를 선언한다.
	// 예) (ABCD) 0895 -- 실제로 담기는 숫자는 895 이다.
	// 08/1000 = A, 08/100 = B, 95/10 = C, 95/1 = D 로 데이터를 인증하도록 한다.
	char strSecurityPW[16+1] ;

	ScriptCheckValue Check;
	char mLoginKey[MAX_PATH];
};

struct MSG_USE_DYNAMIC_SYN : public MSGBASE
{
	char id[MAX_NAME_LENGTH+1];
};

struct MSG_USE_DYNAMIC_ACK : public MSGBASE
{
	char mat[32];
};

struct MSG_LOGIN_DYNAMIC_SYN : public MSGBASE
{
	DWORD AuthKey;
	char id[MAX_NAME_LENGTH+1];
	char pw[MAX_NAME_LENGTH+1];
	char Version[16];
};

struct MSG_LOGIN_ACK : public MSGBASE
{
	char agentip[16];
	WORD agentport;
	DWORD userIdx;
	BYTE cbUserLevel;
};

struct MOVE_ONETARGETPOS : public MSGBASE
{
	DWORD			dwMoverID;
	COMPRESSEDPOS	sPos;
	COMPRESSEDPOS	tPos;

	void SetStartPos( VECTOR3 * pos )
	{
		sPos.Compress(pos);
	}
	void SetTargetPos( VECTOR3* pos )
	{
		tPos.Compress( pos );
	}
	void GetStartPos( VECTOR3 * pos )
	{
		sPos.Decompress( pos );
	}
	void GetTargetInfo(MOVE_INFO * pMoveInfo)
	{
		pMoveInfo->SetCurTargetPosIdx(0);
		pMoveInfo->SetMaxTargetPosIdx(1);
		tPos.Decompress(pMoveInfo->GetTargetPosition(0));
	}
	int GetSize()
	{
		return sizeof(MOVE_ONETARGETPOS);
	}
};

struct MOVE_ONETARGETPOS_FROMSERVER : public MSGBASE
{
	COMPRESSEDPOS	sPos;
	COMPRESSEDPOS	tPos;
	float AddedMoveSpeed;

	void SetStartPos( VECTOR3 * pos )
	{
		sPos.Compress(pos);
	}
	void GetStartPos( VECTOR3 * pos )
	{
		sPos.Decompress( pos );
	}
	void SetTargetPos( VECTOR3* pos )
	{
		tPos.Compress( pos );
	}
	void GetTargetInfo(MOVE_INFO * pMoveInfo)
	{
		pMoveInfo->SetCurTargetPosIdx(0);
		pMoveInfo->SetMaxTargetPosIdx(1);
		tPos.Decompress(pMoveInfo->GetTargetPosition(0));
	}
	int GetSize()
	{
		return sizeof(MOVE_ONETARGETPOS_FROMSERVER);
	}
};

struct MOVE_TARGETPOS : public MSGBASE
{
	DWORD dwMoverID;
	float AddedMoveSpeed;
	COMPRESSEDPOS spos;
	COMPRESSEDTARGETPOS tpos;
	void Init()
	{
		tpos.Init();
	}
	void SetStartPos(VECTOR3 * pos)
	{
		spos.Compress(pos);
	}
	void AddTargetPos(VECTOR3 * pos)
	{
		tpos.AddTargetPos(pos);
	}
	void GetStartPos(VECTOR3 * pos)
	{
		spos.Decompress(pos);
	}
	void GetTargetInfo(MOVE_INFO * pMoveInfo)
	{
		pMoveInfo->SetCurTargetPosIdx(0);//CurTargetPositionIdx = 0;
		pMoveInfo->SetMaxTargetPosIdx(tpos.PosNum);//pMoveInfo->MaxTargetPositionIdx = tpos.PosNum;
		ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
		for(int i = 0 ; i < tpos.PosNum ; ++i)
			tpos.pos[i].Decompress(pMoveInfo->GetTargetPosition(static_cast<BYTE>(i)));//&pMoveInfo->TargetPositions[i]);
	}
	int GetSize()
	{
		return sizeof(MOVE_TARGETPOS) - tpos.GetSurplusSize();
	}
};


struct MOVE_POS : public MSGBASE
{
	DWORD dwMoverID;
	COMPRESSEDPOS cpos;
};

// 091119 ONS 인던 패이드무브 메세지 추가
struct MSG_DUNGEON_FADE_MOVE : public MOVE_POS
{
	DWORD dwWarpIndex;
};


struct MOVE_POS_USER : public MSGBASE
{
	char	Name[MAX_NAME_LENGTH+1];
	COMPRESSEDPOS cpos;
};


struct MSG_WHERE_INFO : public MSGBASE
{
	WORD	wMapNum;		//맵번호
	BYTE	bChannel;		//채널번호
	COMPRESSEDPOS cpos;		//위치
};

// 100312 ONS PC방버프 관련 메세지 추가
struct MSG_PCROOM_BUFF_INFO : public MSGBASE
{
	DWORD dwBuffIndex[5];
	WORD  wStage;
	WORD  wDay;
	BYTE  bIsChanged;
};


struct BASEOBJECT_INFO
{
	BASEOBJECT_INFO()
	{
		BattleID = 0;
		BattleTeam = 0;
	}
	DWORD dwObjectID;
	DWORD dwUserID;
	// 080218 LYW --- CommonStruct : 오브젝트 이름의 길이를 늘리는 작업.
	//char ObjectName[MAX_NAME_LENGTH+1];
	char ObjectName[32+1];
	DWORD BattleID;
	BYTE BattleTeam;
	EObjectState ObjectState;
	eObjectBattleState ObjectBattleState;
};

#define		MAX_INTRODUCE_LEN	512			// 090317 ShinJS --- 리소스의 TextArea 크기와 맞추기 위해 수정(일본버그226)

// 데이트 매칭. CPlayer에서 쓰인다.
// 주민증으로도 쓰일 수 있다. certificate of residence
// desc_hseos_주민등록01
// S 주민등록 추가 added by hseos 2007.06.09
enum																	// 채팅 신청 상태
{
	DATE_MATCHING_CHAT_REQUEST_STATE_NONE = 0,							// 아무상태 아님
	DATE_MATCHING_CHAT_REQUEST_STATE_REQUEST,							// 누군가에게 대화 신청 상태
	DATE_MATCHING_CHAT_REQUEST_STATE_RECV_REQUEST,						// 누군가에게 대화 신청을 받은 상태
	DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING,							// 채팅 중
};

typedef enum															// 채팅 결과
{
	DATE_MATCHING_CHAT_RESULT_REQUEST_AND_WAIT = 0,						// ..신청자에게 신청처리와 대기를 알림
	DATE_MATCHING_CHAT_RESULT_REQUESTED,								// ..대상자에게 누군가의 채팅 신청을 알림
	DATE_MATCHING_CHAT_RESULT_CANCEL,									// ..신청자기 신청 취소
	DATE_MATCHING_CHAT_RESULT_BUSY,										// ..대상자가 이미 채팅 중 or 채팅 신청중 or 채팅 신청받는 중 
	DATE_MATCHING_CHAT_RESULT_TIMEOUT,									// ..시간 초과
	DATE_MATCHING_CHAT_RESULT_REFUSE,									// ..거절함
	DATE_MATCHING_CHAT_RESULT_START_CHAT,								// ..승락하여 채팅 시작
	DATE_MATCHING_CHAT_RESULT_END_CHAT,									// ..채팅 끝
	DATE_MATCHING_CHAT_RESULT_START_CHALLENGEZONE,						// 100107 ONS 대상자가 챌린지존에 있어서 채팅 거부
} DATE_MATCHING_CHAT_RESULT;

// E 주민등록 추가 added by hseos 2007.06.09

struct DATE_MATCHING_INFO						// date matching 2007/03/28
{
	BYTE	bIsValid;							// 주민증 발급 받았는가? 여부.
	BYTE	bActive;							// 활성화 됬는가.? 클라이언트에서 'search' 하는 순간 활성화 된다.
	BYTE	byGender;							// 성별.	GENDER_MALE = 0, GENDER_FEMALE = 1
	DWORD	dwAge;								// 나이.
	// desc_hseos_주민등록01
	// S 주민등록 추가 added by hseos 2007.06.06	2007.06.09
	// DWORD	dwAgeIndex;						// 나이의 분류코드.			// 이거 제대로 쓰이는지 체크요.
	DWORD	nRace;								// 종족
	// E 주민등록 추가 added by hseos 2007.06.06	2007.06.09
	DWORD	dwRegion;							// 사는 지역					enum DATE_MATCHING_REGION 로 선언.
	DWORD	dwGoodFeelingDegree;				// 호감 지수.
	DWORD	pdwGoodFeeling[3];					// 좋아하는 성향				enum DATE_MATCHING_FEELING 로 선언.
	DWORD	pdwBadFeeling[2];					// 싫어하는 성향
	DWORD	dwClass;							// 100113 ONS 직업 정보를 저장한다.
	char	szIntroduce[MAX_INTRODUCE_LEN+1];	// 소개. 일단 char에 300자(한글 150자)한정.
	char	szName[MAX_NAME_LENGTH+1];			// 캐릭터 이름. BASEOBJECT_INFO::ObjectName 과 같다.
	// desc_hseos_주민등록01
	// S 주민등록 추가 added by hseos 2007.06.06	2007.06.09	2007.12.18
	DWORD	nSerchTimeTick;						// 검색을 요청했을 때의 시간 틱. 일정시간 간격으로 검색 허용.
	DWORD	nChatPlayerID;						// 채팅 대상자 ID
	DWORD	nRequestChatState;					// 채팅 신청 상태
	DWORD	nRequestChatTimeTick;				// 채팅 신청 시의 시간 틱
	DWORD	nChatPlayerAgentID;					// 채팅 대상자 에이전트 ID
	BYTE	nMatchingPoint;						// 매칭포인트	
	// E 주민등록 추가 added by hseos 2007.06.06	2007.06.09	2007.12.18
};

struct CHARACTER_TOTALINFO
{
	DWORD	Life;							// 생명력			//?
	DWORD	MaxLife;						// 최대 생명력		//?
	
	BYTE	Race;
	BYTE	Gender;
	BYTE	FaceType;
	BYTE	HairType;
	DWORD	WearedItemIdx[eWearedItem_Max];
	DWORD	HideFlag;
	BYTE	WeaponEnchant;

	BYTE	JobGrade;
	BYTE	Job[ MAX_JOB_GRADE ];
	LEVELTYPE	Level;						// 레벨				//?
	MAPTYPE	CurMapNum;						// 현재맵			//?
	MAPTYPE	LoginMapNum;					// 로그인맵			//?
	BOOL	bPeace;
	WORD	MapChangePoint_Index;			// 맵전환시 사용	//?
	WORD	LoginPoint_Index;				// 로그인시 사용	//?

	DWORD	MunpaID;						// 문파 인덱스
	BYTE	PositionInMunpa;				// 문파 직위 

	DWORD   FamilyID;
	DWORD	nFamilyEmblemChangedFreq;
	// E 패밀리 추가 added by hseos 2007.11.22

	MARKNAMETYPE MarkName;					// guild mark name	
	MARKNAMETYPE FamilyMarkName;					// guild mark name	
	BOOL bVisible;
	bool bPKMode;
	FAMETYPE BadFame;
	float	Height;							// 키
	float	Width;							// 체격

	char	NickName[MAX_GUILD_NICKNAME+1]; // 길드에서의 호칭
	char	GuildName[MAX_GUILD_NAME+1];	// 문파 이름

	char    FamilyNickName[MAX_GUILD_NICKNAME+1];
	char	FamilyName[MAX_GUILD_NAME+1];
	
	DWORD	dwGuildUnionIdx;					// 동맹
	char	sGuildUnionName[MAX_GUILD_NAME+1];	// 동맹이름
	DWORD	dwGuildUnionMarkIdx;				// 동맹마크
	WORD	HideLevel;
	WORD	DetectLevel;
	WORD wInventoryExpansion;
	BOOL bUsingWaterSeed;
	DATE_MATCHING_INFO DateMatching;

};

struct SEND_MONSTER_TOTALINFO	:	public MSGBASE
{
	BASEOBJECT_INFO BaseObjectInfo;
	MONSTER_TOTALINFO TotalInfo;
	SEND_MOVEINFO MoveInfo;

	BYTE bLogin;
	
	CAddableInfoList AddableInfo;

	WORD GetMsgLength()	{	return sizeof(SEND_MONSTER_TOTALINFO) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();	}
};

// 090316 LUJ, 탈것 정보
struct SEND_VEHICLE_TOTALINFO :	public MSGBASE
{
	BASEOBJECT_INFO BaseObjectInfo;
	MONSTER_TOTALINFO TotalInfo;
	SEND_MOVEINFO MoveInfo;
	BYTE bLogin;
	// 090316 LUJ, 좌석 번호별로 탑승한 플레이어 번호가 들어있다
	DWORD mSeat[ VehicleScript::Seat::MaxSize ];
	// 091105 ShinJS --- 소유주의 이름정보
	char MasterName[ MAX_NAME_LENGTH + 1 ];
	DWORD usedItemPosition;

	CAddableInfoList AddableInfo;

	WORD GetMsgLength()	{ return sizeof( *this ) - sizeof( AddableInfo ) + AddableInfo.GetInfoLength();	}
};

struct SEND_NPC_TOTALINFO	:	public MSGBASE
{
	BASEOBJECT_INFO BaseObjectInfo;
	NPC_TOTALINFO TotalInfo;
	SEND_MOVEINFO MoveInfo;
	
	float Angle;
	BYTE bLogin;

	CAddableInfoList AddableInfo;

	WORD GetMsgLength()	{	return sizeof(SEND_NPC_TOTALINFO) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();	}
};

struct SEND_CHARACTER_TOTALINFO : public MSGBASE
{
	BASEOBJECT_INFO BaseObjectInfo;
	CHARACTER_TOTALINFO TotalInfo;	
	SEND_MOVEINFO MoveInfo;
	struct Vehicle
	{
		DWORD mVehicleIndex;
		DWORD mSeatIndex;
	}
	mMountedVehicle;
	struct stHouseRideInfo
	{
		DWORD dwFurnitureObjectIndex;
		WORD wSlot;
	} mRiderInfo;

	BYTE bLogin;
	
	CAddableInfoList AddableInfo;

	WORD GetMsgLength()	{	return sizeof(SEND_CHARACTER_TOTALINFO) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();	}
};

struct SEND_HERO_TOTALINFO : public MSGBASE
{
	BASEOBJECT_INFO BaseObjectInfo;
	CHARACTER_TOTALINFO ChrTotalInfo;
	HERO_TOTALINFO HeroTotalInfo;
	SEND_MOVEINFO SendMoveInfo;
	DWORD UniqueIDinAgent;
	WORD	SkillNum;
	ITEM_TOTALINFO ItemTotalInfo;
	WORD OptionNum;
	WORD PetNum;
	SYSTEMTIME ServerTime; //캐릭터 서버 로그인 시간
	DWORD StorageSize;
	CAddableInfoList AddableInfo;

	WORD GetMsgLength()
	{
		return sizeof( SEND_HERO_TOTALINFO ) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();
	}
};

// 071129 LYW --- CommonStruct : 스킬 초기화 정보 전송 구조체 추가.
struct SEND_SKILL_RESETINFO : public MSGBASE	
{
	CAddableInfoList AddableInfo ;
	WORD SkillNum ;
	int nSkillPoint ;
} ;


struct SEND_CHARSELECT_INFO : public MSGBASE
{
#ifdef _CRYPTCHECK_ 
	HselInit eninit;
	HselInit deinit;
#endif
	int CharNum;
	WORD StandingArrayNum[MAX_CHARACTER_NUM];				// pjs 케릭터의 위치 순서 [5/22/2003]
	BASEOBJECT_INFO BaseObjectInfo[MAX_CHARACTER_NUM];
	CHARACTER_TOTALINFO ChrTotalInfo[MAX_CHARACTER_NUM];
};

struct MSG_NAME : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
};

struct MSG_NAME2 : public MSGBASE
{
	char str1[MAX_NAME_LENGTH+1];
	char str2[MAX_NAME_LENGTH+1];
};

struct MSG_NAME_DWORD : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
	DWORD dwData;
};

struct MSG_NAME_WORD : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
	WORD wData;
};

struct MSG_NAME_DWORD2 : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
	DWORD dwData1;
	DWORD dwData2;
};

struct MSG_HOUSENAME : public MSGBASE
{
	char Name[MAX_HOUSING_NAME_LENGTH];
	DWORD dwData;
};

//횈횗 쨉짜?횑횇횒 째첬쨌횄 ////////////////////////////////////////////////
struct MSG_NAME_DWORD3 : public MSGBASE
{
	char	Name[MAX_NAME_LENGTH+1];
	DWORD	dwData1;
	DWORD	dwData2;
	DWORD	dwData3;
};

struct MSG_NAME_DWORD4 : public MSGBASE
{
	char	Name[MAX_NAME_LENGTH+1];
	DWORD	dwData1;
	DWORD	dwData2;
	DWORD	dwData3;
	DWORD	dwData4;
};

struct MSG_NAME_DWORD5 : public MSGBASE
{
	char	Name[MAX_NAME_LENGTH+1];
	DWORD	dwData1;
	DWORD	dwData2;
	DWORD	dwData3;
	DWORD	dwData4;
	DWORD	dwData5;
};

struct MSG_NAME2_DWORD : public MSGBASE
{
	char	Name1[MAX_NAME_LENGTH+1];
	char	Name2[MAX_NAME_LENGTH+1];
	DWORD	dwData;
};

// desc_hseos_데이트 존_01
// S 데이트 존 추가 added by hseos 2008.01.23
struct MSG_NAME2_DWORD2 : public MSGBASE
{
	char	Name1[MAX_NAME_LENGTH+1];
	char	Name2[MAX_NAME_LENGTH+1];
	DWORD	dwData1;
	DWORD	dwData2;
};

// 091124 ONS 챌린지존 시작 알림 메세지 추가
struct MSG_NAME2_DWORD3 : public MSGBASE
{
	char	Name1[MAX_NAME_LENGTH+1];
	char	Name2[MAX_NAME_LENGTH+1];
	DWORD	dwData1;
	DWORD	dwData2;
	DWORD	dwData3;
};
// E 데이트 존 추가 added by hseos 2008.01.23

//팩 데이터 과련 ////////////////////////////////////////////////
struct SEND_PACKED_DATA : public MSGBASE
{
	WORD wRealDataSize;
	WORD wReceiverNum;
	char Data[MAX_PACKEDDATA_SIZE];
};
struct SEND_PACKED_TOMAPSERVER_DATA : public MSGBASE
{
	MAPTYPE ToMapNum;
	WORD wRealDataSize;
	char Data[MAX_PACKEDDATA_SIZE];
	WORD GetMsgLength()
	{
		return sizeof(SEND_PACKED_TOMAPSERVER_DATA) - MAX_PACKEDDATA_SIZE + wRealDataSize;
	}
};
// 아이템 관련 네트웤 메시지 ////////////////////////////////////

struct MSG_ITEM : public MSGBASE
{
	ITEMBASE ItemInfo;
};

struct MSG_ITEM_WITH_OPTION : public MSGBASE
{
	ITEMBASE ItemInfo;
	ITEM_OPTION OptionInfo;
};

struct MSG_CONSIGNMENT_REGIST : public MSG_ITEM
{
	DWORD dwCategory1Value;
	DWORD dwCategory2Value;

	char szItemName[MAX_ITEMNAME_LENGTH + 1];
	DWORD dwDeposit;
	DWORD dwCommission;
	DWORD dwPrice;
	POSTYPE FromPos;
};

struct MSG_CONSIGNMENT_SEARCH_REQUEST : public MSGBASE
{
	WORD wSortType;
	WORD wStartPage;
	int nCategory1;
	int nCategory2;
	int nMinLevel;
	int nMaxLevel;
	int nRareLevel;
};

struct MSG_CONSIGNMENT_SEARCHINDEX_REQUEST : public MSGBASE
{
	WORD wSortType;
	WORD wStartPage;
	DWORD dwItemList[CONSIGNMENT_SEARCHINDEX_NUM];
};

struct MSG_CONSIGNMENT_UPDATE : public MSGBASE
{
	DWORD dwConsignmentIndex;
	WORD wUpdateKind;

	DWORD dwBuyItemIndex;
	DWORD dwBuyTotalPrice;
	WORD wBuyDurability;
};

struct MSG_CONSIGNMENT_UPDATERESULT : public MSGBASE
{
	int nResult;
	DWORD dwTotalPrice;
	char szItemName[MAX_ITEMNAME_LENGTH + 1];
};

#define MAX_CONSIGNMENT_REGISTNUM 10
struct stConsignmentItemInfo
{
	DWORD dwConsignmentIdx;
	DWORD dwItemDBIdx;
	DWORD dwItemIndex;
	DWORD dwPlayerIndex;
	char szPlayerName[MAX_NAME_LENGTH + 1];
	DWORD dwCategory1;
	DWORD dwCategory2;
	WORD wRareLevel;
	WORD wUseLevel;
	WORD wEnchant;
	DWORD dwRemainMinute;
	WORD wInitDurability;
	DWORD dwDeposit;
	DWORD dwCommission;
	DWORD dwUnitPrice;
	DWORD dwAverageSellPrice;
};

struct MSG_CONSIGNMENT_SEARCH_RESULT : public MSGBASE
{
	DWORD dwCount;
	DWORD dwStartPage;
	DWORD dwEndPage;

	stConsignmentItemInfo ItemList[MAX_CONSIGNMENT_REGISTNUM];
	ITEMBASE ItemInfo[MAX_CONSIGNMENT_REGISTNUM];
	ITEM_OPTION OptionInfo[MAX_CONSIGNMENT_REGISTNUM];
};

struct MSG_CONSIGNMENT_LIST : public MSGBASE
{
	DWORD dwCount;
	DWORD dwStartPage;
	DWORD dwTotalPage;
	stConsignmentItemInfo ItemList[MAX_CONSIGNMENT_REGISTNUM];
};


// 080130 LUJ, 아이템 옵션을 전송하는 메시지 구조체
struct MSG_ITEM_OPTION : public MSGBASE
{
	DWORD		mSize;
	ITEM_OPTION mOption[ 100 ];

	DWORD GetSize() const
	{
		return sizeof( MSG_ITEM_OPTION ) - ( sizeof( mOption ) / sizeof( *mOption ) - mSize ) * sizeof( *mOption );
	}
};

//새로 작업
struct ITEMOBTAINARRAY : public MSGBASE
{
	ITEMOBTAINARRAY()
	{
		ItemNum = 0;
	}
	void Init(MP_CATEGORY cg, BYTE pt, DWORD dwID)
	{
		Category = BYTE(cg);
		Protocol = pt;
		dwObjectID = dwID;
	}
	void AddItem(DWORD DBIdx, DWORD ItemIdx, DURTYPE Durability, POSTYPE bPosition, POSTYPE QuickPosition, ITEMPARAM Param, DWORD RareIdx, ITEM_SEAL_TYPE Sealed, int RemainSecond = 0)
	{
		ItemInfo[ItemNum].dwDBIdx		= DBIdx;
		ItemInfo[ItemNum].wIconIdx		= ItemIdx;
		ItemInfo[ItemNum].Durability	= Durability;
		ItemInfo[ItemNum].Position		= bPosition;
		ItemInfo[ItemNum].QuickPosition	= QuickPosition;
		ItemInfo[ItemNum].ItemParam		= Param;
		//SW050920 RareItem
		// 071127 KTH -- 봉인과 유효시간을 넣어준다.
		ItemInfo[ItemNum].nSealed		= Sealed;
		ItemInfo[ItemNum].nRemainSecond = RemainSecond;
		ItemNum++;
	}
	void AddItem( WORD idx, ITEMBASE * item)
	{
		memcpy( &ItemInfo[idx], item, sizeof( ITEMBASE ) );
		ItemNum++;
	}
	ITEMBASE * GetItem( WORD idx ) { return &ItemInfo[idx]; }
	int GetSize()
	{
		//return ( sizeof( ITEMOBTAINARRAY ) - ( sizeof( ITEMBASE ) * ( SLOT_INVENTORY_NUM - ItemNum ) ) );
		// 071213 KTH --
		return ( sizeof( ITEMOBTAINARRAY ) - ( sizeof( ITEMBASE ) * ( SLOT_MAX_INVENTORY_NUM - ItemNum ) ) );
	}
	void Clear()
	{
		memset( this, 0, sizeof( ITEMOBTAINARRAY ) );
		ItemNum = 0;
	}
	BYTE		ItemNum;
	WORD		wObtainCount;
	//ITEMBASE	ItemInfo[SLOT_INVENTORY_NUM];
	// 071213 KTH --
	ITEMBASE	ItemInfo[SLOT_MAX_INVENTORY_NUM];
};

struct MSG_ITEM_ERROR : public MSGBASE
{
	int ECode;
};

struct MSG_ITEM_MOVE_SYN : public MSGBASE
{
	POSTYPE FromPos;
	DWORD wFromItemIdx;
	POSTYPE ToPos;
	DWORD wToItemIdx;
};

struct MSG_PET_ITEM_MOVE_SYN : public MSGBASE
{
	POSTYPE PetPos;
	DWORD PetItemIdx;
	POSTYPE PlayerPos;
	DWORD PlayerItemIdx;
};

struct MSG_ITEM_MOVE_ACK : public MSG_ITEM_MOVE_SYN
{
};
struct MSG_ITEM_COMBINE_SYN : public MSGBASE
{
	DWORD wItemIdx;
	POSTYPE FromPos;
	POSTYPE ToPos;
	DURTYPE FromDur;
	DURTYPE ToDur;
};
struct MSG_ITEM_COMBINE_ACK : public MSG_ITEM_COMBINE_SYN
{
};
struct MSG_ITEM_DIVIDE_SYN : public MSGBASE
{
	DWORD wItemIdx;
	POSTYPE FromPos;
	POSTYPE ToPos;
	DURTYPE FromDur;
	DURTYPE ToDur;
};

struct MSG_ITEM_DISCARD_SYN : public MSGBASE
{
	POSTYPE TargetPos;
	DWORD wItemIdx;
	DURTYPE ItemNum;
};

struct MSG_ITEM_BUY_SYN : public MSGBASE
{
	DWORD wBuyItemIdx;
	WORD BuyItemNum;
	WORD wDealerIdx;
};

struct MSG_ITEM_SELL_SYN : public MSGBASE
{
	POSTYPE TargetPos;
	DWORD	wSellItemIdx;
	WORD	SellItemNum;
	WORD	wDealerIdx;
};
struct MSG_ITEM_SELL_ACK : public MSG_ITEM_SELL_SYN
{
};


struct MSG_ITEM_ENCHANT_SYN : public MSGBASE
{
	POSTYPE ItemPos;
	DWORD	wItemIdx;

	POSTYPE MaterialItemPos;
	DWORD	wMaterialItemIdx;

	// 080228 LUJ, 보호 아이템
	ICONBASE mProtectionItem;
};


struct MSG_ITEM_ENCHANT_ACK : public MSGBASE
{
	POSTYPE		TargetPos;
	ITEM_OPTION	OptionInfo;
};


// 071217 LUJ, 클라이언트 검사용으로만 사용
struct MSG_ITEM_MIX_SYN : public MSGBASE
{
	DWORD	mResultIndex;
	DWORD	mSourceIndex;	// 배열에서 기본 아이템이 몇번째 배열에 위치해있는지 알려준다
	DWORD	mMixSize;		// 조합 성공시 결과로 얻는 개수
	DWORD	mSourceSize;	// 재료가 차지하는 슬롯 개수
	DWORD	mResultSize;	// 결과가 차지하는 슬롯 개수

	// 080228 LUJ, 보호 아이템
	ICONBASE mProtectionItem;

	struct Material
	{
		DWORD	mItemIndex;
		POSTYPE	mPosition;
		DWORD	mQuantity;
	}
	mMaterial[ SLOT_MAX_INVENTORY_NUM * 2 ];	// 재료 슬롯과 결과 슬롯이 모두 필요하므로 인벤토리 개수의 두배를 잡는다

	int GetSize()
	{
		return sizeof( MSG_ITEM_MIX_SYN ) - ( sizeof( mMaterial ) / sizeof( Material ) - ( mSourceSize + mResultSize ) ) * sizeof( Material );
	}
};



// 071217 LUJ, 계산을 서버에서도 별도로 수행하므로 필요 정보만 넘긴다
struct MSG_ITEM_MIX_SYN2 : public MSGBASE
{
	POSTYPE	mSourceItemPosition;
	DWORD	mSourceItemDbIndex;
	DWORD	mMixSize;
	DWORD	mResultIndex;

	// 080228 LUJ, 보호 아이템
	ICONBASE mProtectionItem;
	// 080925 LUJ, 보조 아이템
	ICONBASE mSupportItem;
};

struct ITEM_UPDATE_RESULT
{
	DWORD mSize;
	ITEMBASE mItem[SLOT_MAX_INVENTORY_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mItem) / sizeof(*mItem) - mSize) * sizeof(*mItem);
	}
};

// 071217 LUJ
// 여기에는 시간 순으로 처리할 아이템 정보의 복사가 들어간다. 처음부터 읽어서 개수가 0이면 삭제하고 2이면 2로 업데이트하면 된다.
// 주의: 비중복 아이템인 경우 Durability = UINT_MAX일 경우 삭제할 아이템임
// 주의: 줄이는 정보가 먼저 나온다. 따라서 늘일 정보를 얻으려면 mMaterlal + mRemoveSize의 포인터를 조회한다
struct MSG_ITEM_MIX_ACK	: public MSGBASE
{
	POSTYPE		mSourceItemPosition;
	DWORD		mSourceItemDbIndex;
	DWORD		mMixSize;
	DWORD		mResultIndex;
	ITEM_UPDATE_RESULT mUpdateResult;

	size_t GetSize() const
	{
		return sizeof(*this) - sizeof(mUpdateResult) + mUpdateResult.GetSize();
	}
};

struct MSG_ITEM_USE_SYN : public MSGBASE
{
	POSTYPE TargetPos;
	DWORD	wItemIdx;
};

// 100517 ONS 전직초기화 아이템 처리 추가
struct MSG_ITEM_CHANGE_CLASS : public MSGBASE
{
	POSTYPE TargetPos;
	DWORD	wItemIdx;
	BYTE	ClassStage[MAX_CLASS_LEVEL - 1];
};

struct MSG_ITEM_USE_ACK : public MSGBASE
{
	POSTYPE			TargetPos;
	DWORD			dwItemIdx;
	eItemUse_Err	eResult;
};

struct MSG_ITEM_DISSOLVE_SYN : public MSGBASE
{
	size_t mSourceSize;
	size_t mSize;

	struct Item
	{
		enum Type
		{
			TypeNone,
			TypeSource,
			TypeResult,
		}
		mType;
		DWORD mIndex;
		DWORD mDbIndex;
		POSTYPE	mPosition;
		DURTYPE	mQuantity;
	}
	mItem[SLOT_MAX_INVENTORY_NUM];

	BOOL Add(const Item& item)
	{
		if(mSize >= SLOT_MAX_INVENTORY_NUM)
		{
			return FALSE;
		}

		mItem[mSize++] = item;
		mSourceSize += (Item::TypeSource == item.mType ? 1 : 0);
		return TRUE;
	}

	size_t GetSize() const
	{
		return sizeof(MSG_ITEM_DISSOLVE_SYN) - (sizeof(mItem) / sizeof(*mItem) - mSize) * sizeof(Item);
	}
};

struct MSG_ITEM_DISSOLVE_ACK : public MSGBASE
{
	ITEM_UPDATE_RESULT mUpdateResult;

	size_t GetSize() const
	{
		return sizeof(*this) - sizeof(mUpdateResult) + mUpdateResult.GetSize();
	}
};

struct MSG_ITEM_REINFORCE_SYN : public MSGBASE
{
	DWORD	mSourceItemIndex;		// 강화할 대상 아이템 번호
	POSTYPE mSourceItemPosition;	// 강화할 대상 아이템 위치
	DWORD	mSourceItemDbIndex;
	WORD	mSize;					// 재료 개수

	// 080228 LUJ, 보호 아이템
	ICONBASE mProtectionItem;
	// 080929 LUJ, 보조 아이템
	ICONBASE mSupportItem;
	ITEMBASE mItem[ SLOT_MAX_INVENTORY_NUM ];	// 071213 KTH --

	DWORD GetSize() const
	{
		return sizeof(MSG_ITEM_REINFORCE_SYN) - ( sizeof( mItem ) / sizeof( *mItem ) - mSize ) * sizeof( *mItem );
	}	
};


struct MSG_ITEM_REINFORCE_OPTION_ACK : public MSGBASE
{
	DWORD		mItemDbIndex;
	ITEM_OPTION mOption;
};


struct MSG_ITEM_REINFORCE_ACK : public MSGBASE
{
	DWORD		mSourceItemDbIndex;
	
	DWORD		mSize;
	ITEMBASE	mItem[ SLOT_MAX_INVENTORY_NUM ];

	DWORD GetSize() const
	{
		return sizeof( MSG_ITEM_REINFORCE_ACK ) - ( sizeof( mItem ) / sizeof( *mItem ) - mSize ) * sizeof( *mItem );
	}
};

// 081119 NYJ - 요리
struct MSG_COOK_SYN : public MSGBASE
{
	DWORD	dwRecipeIdx;
	DWORD	dwFireNpcIdx;
	WORD	wMakeNum;
};

struct MSG_COOK_ACK : public MSGBASE
{
	DWORD dwResultIdx;
	ITEM_UPDATE_RESULT mUpdateResult;

	size_t GetSize() const
	{
		return sizeof(*this) - sizeof(mUpdateResult) + mUpdateResult.GetSize();
	}
};

// 단축창
struct SLOT_INFO
{
	BYTE	kind;
	DWORD	dbIdx;
	DWORD	idx;
	WORD	data;
};

struct TAB_INFO
{
	BYTE		tabNum;
	SLOT_INFO	slot[10];
};

struct MSG_QUICK_INFO : public MSGBASE
{
	TAB_INFO	tab[8];
};

struct MSG_QUICK_ADD_SYN : public MSGBASE
{
	BYTE		tabNum;
	BYTE		pos;
	SLOT_INFO	slot;
};

struct MSG_QUICK_REMOVE_SYN : public MSGBASE
{
	BYTE		tabNum;
	BYTE		pos;
};

struct MSG_QUICK_CHANGE_SYN : public MSGBASE
{
	BYTE	tabNum1;
	BYTE	pos1;
	BYTE	tabNum2;
	BYTE	pos2;
};

struct MSG_SKILL_UPDATE : public MSGBASE
{
	SKILL_BASE SkillBase;
};

// 080417 LUJ, 스킬 정보 전송 구조체
struct MSG_SKILL_LIST : public MSGBASE
{
	DWORD		mSize;
	SKILL_BASE	mData[ 100 ];

	DWORD GetSize() const
	{
		return sizeof( MSG_SKILL_LIST ) - ( sizeof( mData ) / sizeof( *mData ) - mSize ) * sizeof( *mData );
	}
};

struct MSG_LINKITEM : public MSG_ITEM
{
	POSTYPE		wAbsPosition;
};

//SW050920 Rare
struct MSG_LINKITEM_ADD : public MSG_LINKITEM	//교환시 옵션정보를 넘겨줄때 쓴다.
{
	BYTE				bPetInfo;
	ITEM_OPTION			sOptionInfo;
};

struct MSG_LINKITEMEX : public MSG_LINKITEM
{
	MONEYTYPE	dwMoney;
};

struct MSG_LINKBUYITEMEX : public MSG_LINKITEMEX
{
	WORD wVolume;
};

struct MSG_ITEMEX : public MSG_ITEM
{
	BYTE PetInfoCount;
	ITEM_OPTION	sOptionInfo;
	MSG_ITEMEX() {memset(this, 0, sizeof(MSG_ITEMEX));}
};

struct MSG_BUYITEM : public MSG_ITEMEX
{
	DWORD dwData;
};

struct MSG_LOOTINGIEM : public MSG_LINKITEM
{
	DWORD				dwDiePlayerIdx;
	BYTE				bPetSummonItem;
	ITEM_OPTION			sOptionInfo;
};

struct SEND_LINKITEM_TOTALINFO : public MSG_LINKITEMEX
{
	BYTE PetInfoCount;
	ITEM_OPTION sOptionInfo;
};

struct SEND_LINKBUYITEM_TOTALINFO : public SEND_LINKITEM_TOTALINFO
{
	WORD wVolume;
};

struct MSG_EXCHANGE_REMOVEITEM : public MSGBASE
{
	POSTYPE		wAbsPosition;
};

// 071026 LYW --- CommonStruct : Add struct to change item count.
struct MSG_EXCHANGE_DECREASEITEM : public MSGBASE
{
	POSTYPE		wAbsPosition ;
	int			nItemCount ;
} ;

struct MSG_ITEM_DESTROY : public MSGBASE
{
	POSTYPE		wAbsPosition;
	BYTE		cbReason;
};

// LBS 노점상 관련..
struct MSG_STREETSTALL_TITLE : public MSGBASE
{
	char Title[MAX_STREETSTALL_TITLELEN+1];
	WORD StallKind;
};

struct MSG_STREETSTALL_ITEMSTATUS : public MSG_ITEM
{
	POSTYPE		wAbsPosition;
	MONEYTYPE		dwMoney;
	WORD	wVolume;
};


struct STREETSTALLITEM
{
	WORD wVolume; // 구매노점용 수량
	DWORD wIconIdx;
	DWORD dwDBIdx;
	DURTYPE Durability;
	MONEYTYPE money;
	char Locked; // 상태
	char Fill;
	ITEM_SEAL_TYPE nSeal;
	ITEMPARAM ItemParam;
};

struct STREETSTALL_INFO : public MSGBASE
{
	DWORD StallOwnerID;
	char Title[MAX_STREETSTALL_TITLELEN+1];
	STREETSTALLITEM Item[SLOT_STREETSTALL_NUM];
	WORD StallKind;

	WORD count;
	WORD PetItemCount;
	
	CAddableInfoList AddableInfo;

	WORD GetSize()
	{
		return sizeof( STREETSTALL_INFO ) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();
	}
};

struct STREETSTALLTITLE_INFO : public MSGBASE
{
	char Title[MAX_STREETSTALL_TITLELEN+1];
};

struct STREETSTALL_BUYINFO : public MSG_ITEM
{
	DWORD	StallOwnerID;
	POSTYPE StallPos;
	DWORD	dwBuyPrice;
};

// LBS StreetStall 03.11.27
struct MSG_REGISTITEM : public MSG_ITEM
{
	DWORD dwData;
};

struct MSG_REGISTITEMEX : public MSG_REGISTITEM
{
	MONEYTYPE dwMoney;
};

struct MSG_REGISTBUYITEMEX : public MSG_REGISTITEMEX
{
	WORD wVolume;
	WORD wAbsPosition;
};

struct MSG_SELLITEM : public MSG_REGISTITEM
{
	DWORD count;
};

struct MSG_STALLMSG	: public MSGBASE
{
	DWORD	SellerID;
	DWORD	BuyerID;
	DWORD	ItemIdx;
	WORD	count;
	DWORD	money;
};

struct ITEM_SELLACK_INFO : public MSGBASE
{
	//DWORD ItemDBIdx;			// check를 위해  임시 삭제
	MONEYTYPE Money;
	POSTYPE Pos;
};

//struct APPEARANCE_INFO : public MSGBASE
struct MSG_APPERANCE_ADD : public MSGBASE
{
	//DWORD	PlayerID;
	DWORD	mSlotIndex;
	DWORD	mItemIndex;

	//DWORD WearedItem[eWearedItem_Max];
};


struct MSG_APPERANCE_REMOVE : public MSGBASE
{
	DWORD	mSlotIndex;
};

struct MSG_LEVEL : public MSGBASE
{
	enum EXPPOINTKIND
	{
		EXPPOINTKIND_ACQUIRE,
		EXPPOINTKIND_DIE,
	};
	LEVELTYPE Level;
	EXPTYPE CurExpPoint;
	EXPTYPE MaxExpPoint;
};
struct MSG_EXPPOINT : public MSGBASE
{
	enum EXPPOINTKIND
	{
		EXPPOINTKIND_ACQUIRE,
		EXPPOINTKIND_DIE,
	};
	EXPTYPE ExpPoint;
	BYTE ExpKind;
};
struct MSG_SKILLEXP : public MSGBASE
{
	EXPTYPE ExpPoint;
	POSTYPE absPos;
};
struct M2C_DAMAGE_MSG : public MSGBASE
{
	DWORD dwDamage;
};


//////////////////////////////////////////////////////////////////////////
// Skill 관련
struct MAINTARGET
{
	enum
	{
		MAINTARGETKIND_OBJECTID,
		MAINTARGETKIND_POS,
	};
	BYTE MainTargetKind;
	union {
		DWORD dwMainTargetID;		
		COMPRESSEDPOS cpTargetPos;
	};
	void SetMainTarget(DWORD id)
	{	MainTargetKind = MAINTARGETKIND_OBJECTID;	dwMainTargetID = id;	}
	void SetMainTarget(VECTOR3* pp)
	{	MainTargetKind = MAINTARGETKIND_POS;	cpTargetPos.Compress(pp);	}
	MAINTARGET()	{}
	MAINTARGET(DWORD id)	{	SetMainTarget(id);	}
	MAINTARGET(VECTOR3* pp){	SetMainTarget(pp);	}
};

struct SKILLOBJECT_INFO
{
	DWORD SkillObjectIdx;
	DWORD SkillIdx;

	VECTOR3 Pos;
	DWORD StartTime;
	VECTOR3 Direction;
	BYTE SkillLevel;
	DWORD Operator;

	MAINTARGET MainTarget;

	DWORD BattleID;
	WORD Option;
	float Rate;
	DWORD RemainTime;
	// 090204 LUJ, 수치 범위를 확장함
	int Count;
	bool IsNew;
};

enum SKILLKIND
{
	SKILLKIND_PHYSIC,
	SKILLKIND_MAGIC,
	SKILLKIND_PASSIVE,
	SKILLKIND_ONOFF,
	SKILLKIND_BUFF,
	SKILLKIND_MAX
};

enum UNITKIND
{
	UNITKIND_NONE,
	UNITKIND_PHYSIC_ATTCK,
	UNITKIND_MAGIC_ATTCK,
	UNITKIND_LIFE_RECOVER,
	UNITKIND_RESURRECTION,
	UNITKIND_DEBUFF,
	UNITKIND_FAKE_DAMAGE,
	UNITKIND_DECOY,
	UNITKIND_MANA_RECOVER,
	UNITKIND_RETURN,
	UNITKIND_SPELLBREAK,
	UNITKIND_SUMMON,
	// 091127 LUJ, 플레이어 소환
	UNITKIND_RECALL,
	UNITKIND_TAME,
	UNITKIND_PHYSIC_FAKE_DAMAGE,
	UNITKIND_MAGIC_FAKE_DAMAGE,
	UNITKIND_MAX,
};

#define MAX_BUFF_COUNT		5
#define MAX_STATUS_COUNT	3

// 070215 LYW --- CommonStruct : Add struct for conduct part.
struct CONDUCT_INFO
{
	char	ConductName[MAX_NAME_LENGTH+1] ;

	WORD	ConductIdx ;
	WORD	ConductTootipIdx ;
	WORD	ConductKind ;
	WORD	ConductPos ;

	int		HighImage ;

	int		MotionIdx ;

	// 091022 ShinJS --- 탈것 탑승시의 제한 여부
	BOOL	bInvalidOnVehicle;
} ;

struct SKILL_INFO
{
	DWORD Index;
	TCHAR Name[MAX_SKILL_NAME_LENGTH+1];
	/// 툴팁 인덱스
	DWORD Tooltip;
	int	Image;
	SKILLKIND Kind;
	/// 스킬 레벨
	LEVELTYPE Level;
	// 071224 KTH -- 파티중에 사용 불가능한 버프 인가?
	BOOL Party;
	DWORD Money;
	// 080616 LUJ, 마나/HP 소요가 가감뿐 아니라 퍼센트로도 가능하게 한다
	struct Value
	{
		float mPercent;
		float mPlus;
	};
	Value mLife;
	Value mMana;
	// 091211 LUJ, 장비 제한
	eWeaponType mWeaponType;
	eWeaponAnimationType mWeaponAnimationType;
	eArmorType mArmorType;
};

struct BUFF_SKILL_INFO : public SKILL_INFO
{
	DWORD DelayTime;
	int SkillEffect;
	// 080618 LUJ, 어떤 형태의 버프 스킬인지 나타냄. enum형으로 변경
	eStatusKind	Status;
	// 080616 LUJ, 값을 어떤 형태로 처리하는지 나타냄. enum형으로 변경
	enum DataType
	{
		StatusTypeNone,
		StatusTypeAdd,
		StatusTypePercent,
	}
	StatusDataType;
	float StatusData;
	// 090204 LUJ, 조건에 따라 버프 적용 여부를 켜거나 끄는 조건
	struct Condition
	{
		// 090204 LUJ, 비교 대상
		enum Type
		{
			TypeNone,
			TypeLifePercent,
			TypeLife,
			TypeLifeMax,
			TypeLifeRecovery,
			TypeManaPercent,
			TypeMana,
			TypeManaMax,
			TypeManaRecovery,
			TypeStrength,
			TypeIntelligence,
			TypeDexterity,
			TypeWisdom,
			TypeVitality,
			TypePhysicalAttack,
			TypePhysicalDefense,
			TypeMagicalAttack,
			TypeMagicalDefense,
			TypeCriticalRate,
			TypeCriticalDamage,
			TypeAccuracy,
			TypeEvade,
			TypeMoveSpeed,
		}
		mType;
		// 090204 LUJ, 비교 방법
		enum Operation
		{
			OperationNone,
			OperationMoreEqual,	// >=
			OperationLessEqual,	// <=
			OperationMore,		// >
			OperationLess,		// <
			OperationEqual,		// =
		}
		mOperation;
		// 090204 LUJ, 비교 값
		float mValue;
	}
	mCondition;
	// 090204 LUJ, 수치 범위를 확장함
	int Count;
	eBuffSkillActiveRule mRuleBattle;
	eBuffSkillActiveRule mRulePeace;
	eBuffSkillActiveRule mRuleMove;
	eBuffSkillActiveRule mRuleStop;
	eBuffSkillActiveRule mRuleRest;
	BOOL Die;
	BOOL NoUpdate;	//DB에 업데이트 하지 않는다. 맵이동시 제거되는 버프등에 사용.

	// 090204 LUJ, 타입을 명확히 함
	eBuffSkillCountType CountType;
	// 080219 LUJ, 기간제 스킬 여부
	BOOL IsEndTime;
	// 080616 LUJ, 이벤트로 발동될 스킬
	DWORD mEventSkillIndex;
	// 080819 LUJ, 캐쉬 스킬 여부
	BOOL IsCash;
	/// 소모 품목
	DWORD Item[2];
	WORD ItemCount[2];
};

struct ACTIVE_SKILL_INFO : public SKILL_INFO
{
	int	OperatorEffect;
	int	TargetEffect;
	int	SkillEffect;
	DWORD AnimationTime;
	DWORD CastingTime;
	WORD DelayType;
	DWORD DelayTime;
	DWORD CoolTime;
	WORD Target;
	WORD Range;
	TARGET_KIND	Area;
	eSkillAreaTarget AreaTarget;
	WORD AreaData;
	UNITKIND Unit;
	WORD UnitDataType;
	DWORD UnitData;
	/// 스킬 확률
	WORD Success;
	WORD Cancel;
	WORD TrainPoint;
	MONEYTYPE TrainMoney;
	DWORD Buff[ MAX_BUFF_COUNT ];
	WORD BuffRate[ MAX_BUFF_COUNT ];
	DWORD RequiredBuff;
	BOOL IsMove;
	LEVELTYPE RequiredPlayerLevel;
	// 080602 LUJ, 스킬 사용 시 소모되는 아이템
	ITEMBASE mConsumeItem;
	// 080602 LUJ, 스킬 사용 시 소모되는 포인트
	struct ConsumePoint
	{
		// 080602 LUJ, 소모되는 포인트 종류
		enum Type
		{
			TypeNone,
			TypeGuild,
			TypeFamily,
			TypePlayerKill,
			TypeExperience,
		}
		mType;
		DWORD mValue;
	}
	mConsumePoint;
	BOOL mIsBattle;
	BOOL mIsPeace;
	BOOL mIsMove;
	BOOL mIsStop;
};

#define MAX_SKILL_TREE 255

struct SKILL_TREE_INFO
{
	WORD	ClassIndex;
	WORD	Count;
	DWORD	SkillIndex[ MAX_SKILL_TREE ];
};

#include ".\TargetList\TargetList.h"
#include ".\TargetList\TargetListIterator.h"
struct MSG_SKILL_START_SYN : public MSGBASE
{
	DWORD SkillIdx;
	DWORD Operator;
	// 080602 LUJ, 사용할 아이템 정보. 서버에서 조회하려면 시간이 많이 걸리므로 클라이언트에서 한다
	ITEMBASE	mConsumeItem;
	VECTOR3 SkillDir;
	MAINTARGET MainTarget;
	CTargetList TargetList;

	void InitMsg(DWORD wSkillIdx,MAINTARGET* pMainTarget,const VECTOR3& dir,DWORD pOperator)
	{
		Category = MP_SKILL;
		Protocol = MP_SKILL_START_SYN;
		SkillIdx = wSkillIdx;
		TargetList.Clear();
		SkillDir = dir;
        Operator = pOperator;
		memcpy(&MainTarget,pMainTarget,sizeof(MAINTARGET));
	}
	WORD GetMsgLength()
	{
		return sizeof(MSG_SKILL_START_SYN) - sizeof(CTargetList) + TargetList.GetTotalMsgLen();
	}
};

// 071203 KTH --- 스킬을 사용하는 아이템을 사용할때 받는다.
struct MSG_ITEM_SKILL_START_SYN : public MSGBASE
{
	MSG_SKILL_START_SYN	SkillStartSyn;
	POSTYPE	Pos;
	DWORD	Idx;
};

// 080602 LUJ, 전서버 대상 스킬 발동
struct MSG_ITEM_SKILL_START_TO_MAP : public MSGBASE
{
	SKILL_BASE	mSkill;
	DWORD		mGuildIndex;
	DWORD		mFamilyIndex;
};

// 090109 LUJ, 스크립트를 검사하여 전송하는 구조체
struct MSG_SCRIPT_CHECK : public MSGBASE
{
	ScriptCheckValue mCheckValue;
};

struct MSG_SKILLOBJECT_ADD2 : public MSGBASE
{
	SKILLOBJECT_INFO SkillObjectInfo;
	bool bCreate;

	void InitMsg(SKILLOBJECT_INFO* pSkillObjectInfo,bool Create)
	{
		Category = MP_SKILL;
		Protocol = MP_SKILL_SKILLOBJECT_ADD;
		memcpy(&SkillObjectInfo,pSkillObjectInfo,sizeof(SkillObjectInfo));
		SkillObjectInfo.StartTime = gCurTime - SkillObjectInfo.StartTime;
		bCreate = Create;
	}
};

struct MSG_SKILL_RESULT : public MSGBASE
{
	DWORD SkillObjectID;
	bool SkillDamageKind;
	CTargetList TargetList;

	void InitMsg(DWORD SOID)
	{
		Category = MP_SKILL;
		Protocol = MP_SKILL_SKILL_RESULT;
		SkillObjectID = SOID;
		TargetList.Clear();
	}
	WORD GetMsgLength()
	{
		// 080804 LUJ, 구조체 크기를 올바로 반환할 수 있도록 수정
		return sizeof(MSG_SKILL_RESULT) - sizeof(CTargetList) + TargetList.GetTotalMsgLen();
	}

};

struct MSG_SKILL_UPDATE_TARGET : public MSGBASE
{
	DWORD SkillObjectID;
	CTargetList TargetList;

	void InitMsg(BYTE bProtocol, DWORD SOID)
	{
		Category = MP_SKILL;
		Protocol = bProtocol;
		SkillObjectID = SOID;
		TargetList.Clear();
	}
	WORD GetMsgLength()
	{
		// 080804 LUJ, 구조체 크기를 올바로 반환할 수 있도록 수정
		return sizeof(MSG_SKILL_UPDATE_TARGET) - sizeof(CTargetList) + TargetList.GetTotalMsgLen();
	}
};

struct MUNPA_WAREHOUSE_ITEM_INFO
{
//	ITEMBASE WarehouseItem[MAX_MUNPA_WAREITEM_PERTAB_NUM];
	ITEMBASE WarehouseItem[TABCELL_GUILDWAREHOUSE_NUM];
	MONEYTYPE money;
};

struct SEND_MUNPA_WAREHOUSE_ITEM_INFO : public MSGBASE
{
	MUNPA_WAREHOUSE_ITEM_INFO info;
	WORD wOptionCount;
	WORD wRareOptionCount;
	WORD wPetInfoCount;
	BYTE bTabNum;

	CAddableInfoList AddableInfo;

	WORD GetSize()
	{
		return static_cast<WORD>(sizeof( SEND_MUNPA_WAREHOUSE_ITEM_INFO ) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength());
	}
};

struct STORAGELIST
{
	DWORD MaxCellNum;
	MONEYTYPE MaxMoney;
	MONEYTYPE BuyPrice;
};

struct SEND_STORAGE_ITEM_INFO : public MSGBASE
{
	MONEYTYPE money;
	ITEMBASE StorageItem[SLOT_STORAGE_NUM];

	WORD wOptionCount;
	WORD wRareOptionCount;
	WORD wPetInfoCount;

	CAddableInfoList AddableInfo;

	WORD GetSize()
	{
		return static_cast<WORD>(sizeof( SEND_STORAGE_ITEM_INFO ) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength());
	}
	
};

struct MSG_FRIEND_MEMBER_ADDDELETEID : public MSGBASE
{
	DWORD PlayerID;
	char Name[MAX_NAME_LENGTH+1];
};

struct FRIEND
{
	char Name[MAX_NAME_LENGTH+1];
	DWORD Id;
	BOOL IsLoggIn;
};

struct FRIEND_LIST
{
	FRIEND Friend[MAX_FRIEND_LIST];
	int totalnum;
};

struct MSG_FRIEND_LIST_DLG : public MSGBASE
{
	BYTE count;
	FRIEND FriendList[MAX_FRIEND_NUM];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_FRIEND_LIST_DLG) - (sizeof(FRIEND) * (MAX_FRIEND_NUM - count)));
	}
};

struct MSG_FRIEND_SEND_NOTE : public MSGBASE
{
	DWORD FromId;
	char FromName[MAX_NAME_LENGTH+1];
	char ToName[MAX_NAME_LENGTH+1];
	char Title[MAX_NOTE_TITLE+1];
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_FRIEND_SEND_NOTE) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
struct PCROOM_MEMBER
{
	DWORD	CharIndex;
	DWORD	CharClass;
	char	Name[MAX_NAME_LENGTH+1];
};

struct PCROOM_MEMBER_LIST
{
	int totalnum;
	PCROOM_MEMBER Member[MAX_PCROOM_MEMBER_LIST];
};

struct MSG_PCROOM_MEMBER_LIST : public MSGBASE
{	
	BYTE				count;
	WORD				PartyStage;
	WORD				StageMemberMin;
	PCROOM_MEMBER		PCRoomMemList[MAX_PCROOM_MEMBER_NUM];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_PCROOM_MEMBER_LIST) - (sizeof(PCROOM_MEMBER) * (MAX_PCROOM_MEMBER_NUM - count)));
	}
};

struct MSG_GUILD_SEND_NOTE : public MSGBASE
{
	DWORD FromId;
	char FromName[MAX_NAME_LENGTH+1];
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_GUILD_SEND_NOTE) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

struct MSG_FRIEND_SEND_NOTE_ID : public MSGBASE
{
	DWORD TargetID;
	char FromName[MAX_NAME_LENGTH+1];
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_FRIEND_SEND_NOTE_ID) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

struct FRIEND_NOTE
{
	//note는 따로 불러온다. 
	char FromName[MAX_NAME_LENGTH+1];
	DWORD NoteID;
	char SendDate[MAX_SENDDATE_LENGTH+1];
	BYTE bIsRead;

	char SendTitle[MAX_NOTE_TITLE+1];
	DWORD PackageItemIdx;
	DWORD PackageMoney;
};

struct MSG_FRIEND_NOTE_LIST : public MSGBASE
{
	FRIEND_NOTE NoteList[NOTENUM_PER_PAGE];
	BYTE TotalPage;
	WORD TotalMsgNum;
};

struct MSG_FRIEND_DEL_NOTE : public MSGBASE
{
	DWORD NoteID;
	BOOL bLast;
};

struct MSG_FRIEND_READ_NOTE : public MSGBASE
{
	char FromName[MAX_NAME_LENGTH+1];
	
	DWORD NoteID;
	WORD ItemIdx;
	DWORD dwPackageMoney;
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_FRIEND_READ_NOTE) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

struct MSG_FRIEND_READ_NOTE_WITH_PACKAGE : public MSG_ITEM_WITH_OPTION
{
	char FromName[MAX_NAME_LENGTH+1];
	
	DWORD NoteID;
	WORD ItemIdx;
	DWORD PackageItem;
	DWORD PackageMoney;
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_FRIEND_READ_NOTE_WITH_PACKAGE) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

//////////////////////////////////////////////////////////////////////////
//채널
struct MSG_CHANNEL_INFO : public MSGBASE
{
	WORD PlayerNum[MAX_CHANNEL_NUM];
	char ChannelName[MAX_CHANNEL_NAME+1];
	DWORD dwUniqueIDinAgent;
	BYTE Count;
};

struct MSG_CHANNEL_INFO_MORNITOR : public MSGBASE
{
	MSG_CHANNEL_INFO	ChannelInfo;
	char				sServerIP[MAX_IPADDRESS_SIZE];
	WORD				wServerPort;
};
//////////////////////////////////////////////////////////////////////////


//------------ AUTOPATCH 관련 메시지 ----------------------
typedef struct tagServerTraffic : public MSGROOT
{
	LONG UserNum;
	int Returnkey;
	
}ServerTraffic;


//////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 채팅관련메세지 //////////////////////////////////////

struct MSG_CHAT : public MSGBASE
{
	char	Name[MAX_NAME_LENGTH+1];
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(MSG_CHAT) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct MSG_CHAT_WITH_SENDERID : public MSGBASE
{
	DWORD	dwSenderID;
	char	Name[MAX_NAME_LENGTH+1];
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(MSG_CHAT_WITH_SENDERID) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct MSG_WHISPER : public MSGBASE
{
	DWORD	dwReceiverID;
	char	SenderName[MAX_NAME_LENGTH+1];
	char	ReceiverName[MAX_NAME_LENGTH+1];
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(MSG_WHISPER) - MAX_CHAT_LENGTH + strlen(Msg); }
};

struct SEND_PARTY_CHAT : public MSGBASE
{
	DWORD	MemberID[MAX_PARTY_LISTNUM];
	BYTE	MemberNum;

	char	Name[MAX_NAME_LENGTH+1];
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(SEND_PARTY_CHAT) - MAX_CHAT_LENGTH + strlen(Msg); }
};

// 070105 LYW --- Add struct for chatting of family system.
struct SEND_FAMILY_CHAT : public MSGBASE
{
	DWORD	MemberID[ MAX_FAMILY_LISTNUM ] ;
	BYTE	MemberNum ;

	char	Name[ MAX_NAME_LENGTH + 1 ] ;
	char	Msg[ MAX_CHAT_LENGTH + 1 ] ;
	int		GetMsgLength() { return sizeof( SEND_FAMILY_CHAT ) - MAX_CHAT_LENGTH + strlen( Msg ) ; }
} ;

struct MSG_CHAT_WORD : public MSGBASE
{
	WORD	wData;
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(MSG_CHAT_WORD) - MAX_CHAT_LENGTH + strlen(Msg); }
};

enum eChatError
{
	CHATERR_ERROR,
	CHATERR_NO_NAME,
	CHATERR_NOT_CONNECTED,
	CHATERR_OPTION_NOWHISPER,
};

struct BATTLE_TEAMMEMBER_INFO
{
	DWORD MemberID;
	char MemberName[MAX_NAME_LENGTH+1];
	void set(DWORD id,char* name)
	{
		MemberID = id;
//		strcpy(MemberName,name);
		strncpy( MemberName, name, MAX_NAME_LENGTH );
		MemberName[MAX_NAME_LENGTH] = 0;

		MemberKillNum = 0;
		MemberDieNum = 0;
	}
	BYTE MemberKillNum;
	BYTE MemberDieNum;
};

struct BATTLE_INFO_BASE
{
	DWORD BattleID;
	BYTE BattleKind;
	BYTE BattleState;
	DWORD BattleTime;
};

#define MAX_BATTLE_INFO_LENGTH	2048
struct MSG_BATTLE_INFO : public MSGBASE
{
	BYTE BattleKind;
	char info[MAX_BATTLE_INFO_LENGTH];
	WORD msglen;

	int GetMsgLength()
	{
		return sizeof(MSGBASE) + msglen + sizeof(BYTE);
	}
};

struct MSG_BATTLE_TEAMMEMBER_ADDDELETE : public MSGBASE

{
	BATTLE_TEAMMEMBER_INFO Member;
	BYTE Team;
};


//==============
//For Showdown
struct MSG_BATTLE_SHOWDOWN_CREATESTAGE : public MSGBASE
{
	DWORD	dwBattleID;
	VECTOR3 vPosStage;		//이것 줄이자. 아래로 바꾸자.
//	COMPRESSEDPOS vPosStage
};
//===============


struct MSG_MONEY : public MSGBASE
{
	MONEYTYPE dwTotalMoney;		// 전체 금액
	BYTE bFlag;					// MsgFlag
};

struct SEND_GUILD_CREATE_SYN : public MSGBASE
{
	char GuildName[MAX_GUILD_NAME+1];
	char Intro[MAX_GUILD_INTRO+1];
	int GetMsgLength()
	{
		return sizeof(SEND_GUILD_CREATE_SYN) - (MAX_GUILD_INTRO+1) + strlen(Intro)+1;
	}
};
struct GUILDINFO
{
	DWORD GuildIdx;
	DWORD MasterIdx;
	char MasterName[MAX_NAME_LENGTH+1];
	char GuildName[MAX_GUILD_NAME+1];
	DWORD UnionIdx;
	BOOL mIsUnionMaster;
	LEVELTYPE GuildLevel;
	MARKNAMETYPE MarkName;
};


struct GUILDMEMBERINFO
{
	DWORD MemberIdx;
	char MemberName[MAX_NAME_LENGTH+1];
	LEVELTYPE Memberlvl;
	BYTE Rank;
	BOOL bLogged;
    // 081031 LUJ, 맵 번호
	MAPTYPE	mMapType;
	// 080225 LUJ, 직업 정보 보관
	BYTE	mJobGrade;
	BYTE	mJob[ MAX_JOB_GRADE ];
	BYTE	mRace;
};


struct SEND_GUILD_CREATE_ACK : public MSGBASE
{
	DWORD GuildIdx;
	char GuildName[MAX_GUILD_NAME+1];
};


struct SEND_GUILD_CREATE_NOTIFY : public MSGBASE
{
	GUILDINFO		GuildInfo;
	LEVELTYPE		MasterLvl;

	// 080225 LUJ, 설정된 회원 정보
	GUILDMEMBERINFO	mMaster;
};


struct SEND_GUILD_MEMBER_INFO : public MSGBASE
{
	DWORD GuildIdx;
	char GuildName[MAX_GUILD_NAME+1];	
	GUILDMEMBERINFO MemberInfo;
};
struct SEND_GUILD_TOTALINFO : public MSGBASE
{
	GUILDINFO GuildInfo;
	//SW060719 문파포인트
	//GUILDPOINT_INFO PointInfo;
	int membernum;
	GUILDMEMBERINFO MemberInfo[MAX_GUILD_MEMBER];
	void InitGuildInfo( const GUILDINFO& pInfo)
	{
		GuildInfo.GuildIdx = pInfo.GuildIdx;
		GuildInfo.GuildLevel = pInfo.GuildLevel;
		strcpy(GuildInfo.GuildName, pInfo.GuildName);
		GuildInfo.MasterIdx = pInfo.MasterIdx;
		// 06. 03. 문파공지 - 이영준
		//strcpy(GuildInfo.GuildNotice, pInfo.GuildNotice);
		GuildInfo.UnionIdx = pInfo.UnionIdx;
		strcpy(GuildInfo.MasterName, pInfo.MasterName);
		GuildInfo.MarkName = pInfo.MarkName;
	}
	//void InitPointInfo(GUILDPOINT_INFO* pInfo)
	//{
	//	memcpy(&PointInfo, pInfo, sizeof(GUILDPOINT_INFO));
	//}
	int GetMsgLength() { return sizeof(SEND_GUILD_TOTALINFO) - (MAX_GUILD_MEMBER-membernum)*sizeof(GUILDMEMBERINFO);	}
};
struct SEND_GUILD_INVITE : public MSGBASE
{
	DWORD MasterIdx;
	char MasterName[MAX_NAME_LENGTH+1];
	char GuildName[MAX_GUILD_NAME+1];
};

struct SEND_GUILD_NICKNAME : public MSGBASE
{
	DWORD TargetIdx;
	char NickName[MAX_GUILD_NICKNAME+1];
};

struct SEND_GUILD_NAME : public MSGBASE
{
	char GuildName[MAX_GUILD_NAME+1];
};

struct SEND_GUILDMEMBER_LOGININFO : public MSGBASE
{
	DWORD MemberIdx;
	BOOL bLogIn;
	// 081031 LUJ, 맵 번호
	MAPTYPE	mMapType;
};

struct SEND_GUILD_HUNTEDMOB_INFO : public MSG_DWORD2
{};

#define GUILDMARKBPP	2
#define GUILDMARKWIDTH	16
#define GUILDMARKHEIGHT	16
#define GUILDMARK_BUFSIZE	(GUILDMARKBPP*GUILDMARKWIDTH*GUILDMARKHEIGHT)
#define GUILDUNIONMARKHEIGHT	16
#define GUILDUNIONMARK_BUFSIZE	(GUILDMARKBPP*GUILDMARKWIDTH*GUILDUNIONMARKHEIGHT)

struct MSG_GUILDMARK_IMG : public MSGBASE
{
	DWORD GuildIdx;
	MARKNAMETYPE MarkName;
	char imgData[GUILDMARK_BUFSIZE];
};

struct MSG_GUILDUNIONMARK_IMG : public MSGBASE
{
	DWORD	dwMarkIdx;
	DWORD	dwGuildUnionIdx;
	char	imgData[GUILDUNIONMARK_BUFSIZE];
};

// 071015 웅주, 동맹간의 전쟁이 가능해짐에 따라 동맹 이름을 알려주도록 MSG_GUILDWAR_INFO가 확장됨
struct MSG_GUILD_LIST : public MSGBASE
{
	DWORD mSize;

	struct Data : public GUILDINFO
	{
		char mUnionName[ MAX_NAME_LENGTH + 1 ];
	}
	mData[ 100 ];

	DWORD GetSize() const
	{
		return sizeof( MSG_GUILD_LIST ) - ( sizeof( mData ) / sizeof( Data ) - mSize ) * sizeof( Data );
	};
};


// 071017 웅주, 길드전의 결과를 다른 맵서버로 전파한다.
struct MSG_GUILD_WAR_RESULT : public MSGBASE
{
	struct Data
	{
		DWORD	mGuildIndex;
		DWORD	mScoreValue;		// 최신 점수
		float	mScoreVariation;	// 변화된 값
	};
	
	Data	mWinner;
	Data	mLoser;
};

// 070801 웅주. 플레이어 접속 시, 길드의 상세 정보를 쿼리하여 담는다
struct MSG_GUILD_DATA : public MSGBASE
{
	DWORD	mIndex;	
	DWORD	mLevel;
	MAPTYPE	mLocation;
	DWORD	mUnionIndex;
	DWORD	mScore;
	DWORD	mWarehouseSize;
	DWORD	mWarehouseRank;
	char	mName	[ MAX_GUILD_NAME	+ 1 ];
	char	mIntro	[ MAX_GUILD_INTRO	+ 1 ];
	char	mMark	[ MAX_NAME_LENGTH	+ 1 ];	// TODO: 웅주 마크에 대해서 좀더 검토 필요
	char	mNotice	[ MAX_GUILD_NOTICE  + 1 ];

	DWORD			mMemberSize;
	GUILDMEMBERINFO	mMember[ MAX_GUILD_MEMBER ];

	DWORD GetSize() const
	{
		return sizeof( MSG_GUILD_DATA ) - ( sizeof( mMember ) / sizeof( GUILDMEMBERINFO ) - mMemberSize ) * sizeof( GUILDMEMBERINFO );
	}
};

struct MSG_GUILD_OPEN_WAREHOUSE : public MSGBASE
{
	DWORD	mPlayerIndex;
	char	mName[ MAX_NAME_LENGTH ];
	DWORD	mMoney;
	USHORT	mWarehouseSize;	// 길드 창고 크기
	BYTE	mWarehouseRank;	// 길드 창고 접근 권한
};


struct MSG_GUILD_SET_WAREHOUSE_RANK : public MSGBASE
{
	DWORD	mGuildIndex;
	BYTE	mRank;			// 창고 접근 가능 권한
};


// 070808 웅주, 길드 아이템 정보
struct MSG_GUILD_ITEM : public MSGBASE
{
	DWORD		mSize;
	ITEMBASE	mItem[ SLOT_GUILDWAREHOUSE_NUM ];

	DWORD GetSize() const
	{
		return sizeof( MSG_GUILD_ITEM ) - ( sizeof( mItem ) / sizeof( ITEMBASE ) - mSize ) * sizeof( ITEMBASE );
	}
};


// 070808 웅주, 길드 아이템의 옵션 정보
struct MSG_GUILD_ITEM_OPTION : public MSGBASE
{
	DWORD		mSize;
	ITEM_OPTION	mOption[ SLOT_GUILDWAREHOUSE_NUM ];

	DWORD GetSize() const
	{
		return sizeof( MSG_GUILD_ITEM_OPTION ) - ( sizeof( mOption ) / sizeof( ITEM_OPTION ) - mSize ) * sizeof( ITEM_OPTION );
	}
};

// 081031 LUJ, 소환 요청
struct MSG_RECALL_SYN : public MSGBASE
{
	struct Player
	{
		DWORD	mIndex;
		char	mName[ MAX_NAME_LENGTH + 1 ];
		MAPTYPE	mMapType;
	};
	Player	mRecallPlayer;
	Player	mTargetPlayer;

	DWORD	mChannelIndex;
	DWORD	mRandomPositionRange;
	DWORD	mLimitMiliSecond;
	DWORD	mKey;
	DWORD	mSkillIndex;
	
	MSG_RECALL_SYN( MP_CATEGORY category = MP_MAX, BYTE protocol = 0 )
	{
		ZeroMemory( this, sizeof( *this ) );
		Category	= static_cast<BYTE>(category);
		Protocol	= protocol;
	}
};

// 081031 LUJ, 소환 시 오류 메시지
struct MSG_RECALL_NACK : public MSGBASE
{
	enum Type
	{
		TypeNone,
		TypePlayerIsNotExist,
		TypePlayerIsNoMember,
		TypePlayerLocateOnBlockedMap,
		TypePlayerHasNoSkill,
		TypePlayerIsInCoolTimeForSkill,
		TypePlayerFailedUsingSkill,
		TypePlayerCannotRecallOneself,
		TypePlayerDied,
		TypePlayerIsDealing,
		TypePlayerIsExchanging,
		TypePlayerIsGuestInStall,
		TypePlayerIsOwnerInStall,
		TypePlayerIsInPlayerKillMode,
		TypePlayerIsOnShowdown,
		TypeTargetIsNotExist,
		TypeTargetIsNoMember,
		TypeTargetLocateOnBlockedMap,
		TypeTargetIsInPlayerKillMode,
		TypeTargetIsLooting,
		TypeTargetDoAutoNote,
		TypeTargetIsDealing,
		TypeTargetIsExchanging,
		TypeTargetDied,
		TypeTargetIsGuestInStall,
		TypeTargetIsOwnerInStall,
		TypeTargetIsOnShowdown,
		TypeTargetIsOnBattle,
		TypeSkillIsNotExist,
		TypeSkillNeedsMoreGuildScore,
		TypeSkillNeedsMoreFamilyPoint,
		TypeTimeOver,
	}
	mType;

	DWORD mKey;
	TCHAR mRecallPlayerName[ MAX_NAME_LENGTH + 1 ];
	TCHAR mTargetPlayerName[ MAX_NAME_LENGTH + 1 ];

	// 081203 LUJ, 경고를 피하기 위해 타입을 변경함
	MSG_RECALL_NACK( MP_CATEGORY category, BYTE protocol, Type type, DWORD objectIndex, DWORD key )
	{
		ZeroMemory( this, sizeof( *this ) );
		Category	= static_cast<BYTE>(category);
		Protocol	= protocol;
		mType		= type;
		dwObjectID	= objectIndex;
		mKey		= key;
	}
};

#define FAMILYMARKBPP	2
#define FAMILYMARKWIDTH	16
#define FAMILYMARKHEIGHT	12
#define FAMILYMARK_BUFSIZE	(FAMILYMARKBPP*FAMILYMARKWIDTH*FAMILYMARKHEIGHT)

struct MSG_FAMILYMARK_IMG : public MSGBASE
{
	DWORD FamilyIdx;
	MARKNAMETYPE MarkName;
	char imgData[FAMILYMARK_BUFSIZE];
};

#pragma pack(1)

typedef DWORD	QSTATETYPE;
#define MAX_BIT ((sizeof(QSTATETYPE))*(8))

#define YEARTOSECOND	31536000
#define MONTHTOSECOND	2592000
#define DAYTOSECOND		86400
#define HOURTOSECOND	3600
#define MINUTETOSECOND	60

extern DWORD DayOfMonth[];
extern DWORD DayOfMonth_Yundal[];

//┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//┃stTIME														  ┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
struct stTIME{
	QSTATETYPE		value;

	stTIME():value(0) {}

	void SetTime(DWORD time)	{	value = time;	}
	void SetTime(DWORD year, DWORD month, DWORD day, DWORD hour, DWORD minute, DWORD second)
	{
		value=0;
		QSTATETYPE ch=0;

		ch = year;
		value = (value | (ch<<28));
		ch = month;
		value = (value | (ch<<24));
		ch = day;
		value = (value | (ch<<18));
		ch = hour;
		value = (value | (ch<<12));
		ch = minute;
		value = (value | (ch<<6));
		ch = second;
		value = (value | ch);
	}

	//
	DWORD GetYear()		{	return value>>28;	}
	DWORD GetMonth()	{	DWORD msk = value<<4;		return msk>>28;		}
	DWORD GetDay()		{	DWORD msk = value<<8;		return msk>>26;		}
	DWORD GetHour()		{	DWORD msk = value<<14;		return msk>>26;		}
	DWORD GetMinute()	{	DWORD msk = value<<20;		return msk>>26;		}
	DWORD GetSecond()	{	DWORD msk = value<<26;		return msk>>26;		}
	inline BOOL	operator >(const stTIME& time);
	inline void operator +=(const stTIME& time);
	inline void operator -=(const stTIME& time);
	inline void operator =(const stTIME& time);
};

inline BOOL	 stTIME::operator >(const stTIME& time)
{
	BOOL bResult = FALSE;
	stTIME ctime = time;

	if( this->GetYear() == ctime.GetYear() )
	{
		if( this->GetMonth() > ctime.GetMonth() )
			bResult = TRUE;
		else if( this->GetMonth() == ctime.GetMonth() )
		{
			if( this->GetDay() > ctime.GetDay() )
				bResult = TRUE;
			else if( this->GetDay() == ctime.GetDay() )
			{
				if( this->GetHour() > ctime.GetHour() )
					bResult = TRUE;
				else if( this->GetHour() == ctime.GetHour() )
				{
					if( this->GetMinute() > ctime.GetMinute() )
						bResult = TRUE;
					else if( this->GetMinute() == ctime.GetMinute() )
						if( this->GetSecond() > ctime.GetSecond() )
							bResult = TRUE;
				}						
			}
		}
	}
	else if( this->GetYear() > ctime.GetYear() )
		bResult = TRUE;

	return bResult;
}

//---KES ETC 701020
inline void	 stTIME::operator +=(const stTIME& time)
{
	stTIME atime = time;
	int year, month, day, hour, minute, second, calcmonth;
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	year = this->GetYear() + atime.GetYear();
	month = this->GetMonth() + atime.GetMonth();
	day = this->GetDay() + atime.GetDay();
	hour = this->GetHour() + atime.GetHour();
	minute = this->GetMinute() + atime.GetMinute();
	second = this->GetSecond() + atime.GetSecond();

	if( this->GetMonth() == 1 )			calcmonth = 11;
	else								calcmonth = this->GetMonth()-1;

	if(second >= 60)
	{
		++minute;
		second -= 60;
	}
	if(minute >= 60)
	{
		++hour;
		minute -= 60;
	}
	if(hour >= 24)
	{
		++day;
		hour -= 24;
	}
	if( (systime.wYear%4) == 0 )
	{
		if(day > (int)(DayOfMonth_Yundal[calcmonth]))
		{
			++month;
			day -= DayOfMonth_Yundal[calcmonth];
		}
	}
	else
	{
		if(day > (int)(DayOfMonth[calcmonth]))
		{
			++month;
			day -= DayOfMonth[calcmonth];
		}
	}
	if(month > 12)
	{
		++year;
		month -= 12;
	}
	
	this->SetTime(year, month, day, hour, minute, second);
}
inline void	 stTIME::operator -=(const stTIME& time)
{
	stTIME atime = time;
	int year, month, day, hour, minute, second, calcmonth;
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	year = this->GetYear() - atime.GetYear();
	month = this->GetMonth() - atime.GetMonth();
	day = this->GetDay() - atime.GetDay();
	hour = this->GetHour() - atime.GetHour();
	minute = this->GetMinute() - atime.GetMinute();
	second = this->GetSecond() - atime.GetSecond();

	if( this->GetMonth() == 1 )			calcmonth = 11;
	else								calcmonth = this->GetMonth()-2;

	if(second < 0)
	{
		--minute;
		second += 60;
	}
	if(minute < 0)
	{
		--hour;
		minute += 60;
	}
	if(hour < 0)
	{
		--day;
		hour += 24;
	}
	if( (systime.wYear%4) == 0 )
	{
		if(day < 0)
		{
			--month;
			day += DayOfMonth_Yundal[calcmonth]; 
		}		
	}
	else
	{
		if(day < 0)
		{
			--month;
			day += DayOfMonth[calcmonth]; 
		}
	}
	if(month <= 0)
	{
		--year;
		month += 12;
	}
	
	this->SetTime(year, month, day, hour, minute, second);
}
//-------------------
inline void	 stTIME::operator =(const stTIME& time)
{
	stTIME atime = time;
	this->SetTime(atime.GetYear(), atime.GetMonth(), atime.GetDay(), atime.GetHour(), atime.GetMinute(), atime.GetSecond());
}
//



struct stPlayTime {
	DWORD value;

	stPlayTime::stPlayTime()
	{
		value = 0;
	}
	void GetTime(int& Year, int& Day, int& Hour, int& Minute, int& Second)
	{
		int mv = 0;

		Year = value/YEARTOSECOND;
		mv = value%YEARTOSECOND;

		Day = mv/DAYTOSECOND;
		mv = mv%DAYTOSECOND;

		Hour = mv/HOURTOSECOND;
		mv = mv%HOURTOSECOND;

		Minute = mv/MINUTETOSECOND;
		Second = mv%MINUTETOSECOND;		
	}
};


// LBS 04.01.06----------------------------------
typedef struct tagField{
	DWORD b0:1; 
	DWORD b1:1;
	DWORD b2:1;
	DWORD b3:1;
	DWORD b4:1;
	DWORD b5:1;
	DWORD b6:1;
	DWORD b7:1;

	DWORD b8:1;
	DWORD b9:1;
	DWORD b10:1;
	DWORD b11:1;
	DWORD b12:1;
	DWORD b13:1;
	DWORD b14:1;
	DWORD b15:1;

	DWORD b16:1;
	DWORD b17:1;
	DWORD b18:1;
	DWORD b19:1;
	DWORD b20:1;
	DWORD b21:1;
	DWORD b22:1;
	DWORD b23:1;

	DWORD b24:1;
	DWORD b25:1;
	DWORD b26:1;
	DWORD b27:1;
	DWORD b28:1;
	DWORD b29:1;
	DWORD b30:1;
	DWORD b31:1;

} stFIELD, *pFIELD;

typedef union tagQuestFlag{
	QSTATETYPE	value;
	stFIELD		flag;

	tagQuestFlag():value(0) {}

	void SetField(BYTE bit, BOOL bSetZero=FALSE) // nBit의 유효 숫자는 DWORD인경우1 ~ 32 이다
	{
		if( bit  < 1 || bit > MAX_BIT ) return;

		QSTATETYPE ch;
		bSetZero ? ch=1 : ch=0;
		value = (value | (ch<<(MAX_BIT- bit)));
	}

	BOOL IsSet(BYTE bit)
	{
		// 쓰레기 값들어오는 경우 현재 적용되었다고 알린다.
		if( bit  < 1 || bit > MAX_BIT ) return TRUE;

		QSTATETYPE ch = 1;
		return (value & (ch<<(MAX_BIT-bit)))? TRUE:FALSE;
	}

} QFLAG, *pQFLAG;


struct QBASE
{
	WORD		QuestIdx;
	QSTATETYPE	state;
};

struct QMBASE
{
	WORD		QuestIdx;	
	QFLAG		state;
	DWORD		EndParam;
	__time64_t	registTime;
	BYTE		CheckType;
	DWORD		CheckTime;
};

// 071023 LYW --- CommonStruct : Add struct for tutorial info.
struct TIBASE
{
	int			nTutorialIdx ;
	BOOL		IsComplete ;
} ;

struct QSUBASE
{
	WORD		QuestIdx;
	WORD		SubQuestIdx;
	QSTATETYPE	state;
	QSTATETYPE	time;
};

struct QITEMBASE
{
	DWORD		ItemIdx;
	WORD		Count;
	WORD		QuestIdx;
};

struct SEND_QUEST_IDX : public MSGBASE
{
	WORD		MainQuestIdx;
	WORD		SubQuestIdx;
	DWORD		dwFlag;
	DWORD		dwData;
	__time64_t	registTime;
};

// 100414 ONS  퀘스트 이벤트 메세지에 선택보상아이템 정보를 추가한다.
struct SEND_QUEST_REQUITAL_IDX : public SEND_QUEST_IDX
{
	DWORD		dwRequitalIdx;
	DWORD		dwRequitalCount;
};

struct SEND_QUESTITEM_IDX : public MSGBASE
{
	DWORD	dwItemIdx;
	DWORD	dwItemNum;
};

struct SEND_QUEST_TOTALINFO : public MSGBASE
{
	int GetSize() {
		return (sizeof(SEND_QUEST_TOTALINFO) - sizeof(QBASE)*(50 - wCount));
	}

	WORD	wCount;
	QBASE	QuestList[50];
};

struct SEND_MAINQUEST_DATA : public MSGBASE
{
	int GetSize() {
		return (sizeof(SEND_MAINQUEST_DATA) - sizeof(QMBASE)*(100 - wCount));
	}

	WORD	wCount;
	QMBASE	QuestList[100];
};

// 071023 LYW --- CommonStruct : Add struct for tutorial data.
struct SEND_TUTORIAL_DATA : public MSGBASE
{
	int GetSize() {
		return (sizeof(SEND_TUTORIAL_DATA) - sizeof(TIBASE)*(100 - wCount));
	}

	WORD	wCount;
	TIBASE	TutorialList[100];
};

// 071106 LYW --- CommonStruct : Add structure for send message about update to default stats.
struct SEND_DEFAULT_STATS : public MSGBASE
{
	int nStr ;
	int nDex ;
	int nVit ;
	int nInt ;
	int nWis ;
	int nPoint ;

	int nErrorCode ;
};

struct SEND_SUBQUEST_DATA : public MSGBASE
{
	int GetSize() {
		return (sizeof(SEND_SUBQUEST_DATA) -sizeof(QSUBASE)*(100-wCount));
	}

	WORD	wCount;
	QSUBASE	QuestList[100];
};

struct SEND_SUBQUEST_UPDATE : public MSGBASE
{
	WORD	wQuestIdx;
	WORD	wSubQuestIdx;
	DWORD	dwMaxCount;
	DWORD	dwData;
	DWORD	dwTime;
};

struct SEND_QUESTITEM : public MSGBASE
{
	int GetSize() {
		return (sizeof(SEND_QUESTITEM)-sizeof(QITEMBASE)*(100-wCount));
	}

	WORD		wCount;
	QITEMBASE	ItemList[100];
};

struct QUESTINFO {
	DWORD	m_dwIdx;		// Quest idx
	QFLAG	m_flag;			// Quest State Value
};

// PartyIcon
struct MoveCoord
{
	WORD posX;
	WORD posZ;

	MoveCoord::MoveCoord()
	{
		posX = posZ = 0;
	}
};

struct PARTYICON_MOVEINFO
{
	DWORD	dwMoverID;
	BYTE	tgCount;
	WORD	KyungGongIdx;
	float	fSpeed;
	MoveCoord	cPos;
	MoveCoord	tgPos[MAX_CHARTARGETPOSBUF_SIZE];

	PARTYICON_MOVEINFO::PARTYICON_MOVEINFO()
	{
		dwMoverID	= 0;
		tgCount		= 0;
		KyungGongIdx = 0;
		fSpeed = 0.0f;
		memset(&cPos, 0, sizeof(MoveCoord));
		memset(tgPos, 0, sizeof(MoveCoord)*MAX_CHARTARGETPOSBUF_SIZE);
	}

	int GetSize()
	{
		return sizeof(PARTYICON_MOVEINFO)-sizeof(MoveCoord)*(MAX_CHARTARGETPOSBUF_SIZE-tgCount);
	}
};

struct SEND_PARTYICON_MOVEINFO : public MSGBASE
{
	PARTYICON_MOVEINFO MoveInfo;

	int GetSize()
	{
		return sizeof(MSGBASE)+MoveInfo.GetSize();
	}
};

struct SEND_PARTYICON_REVIVE : public MSGBASE
{
	DWORD		dwMoverID;
	MoveCoord	Pos;
};

struct SEND_SHOPITEM_INFO : public MSGBASE
{
	WORD		ItemCount;
	ITEMBASE	Item[SLOT_SHOPITEM_NUM];

	SEND_SHOPITEM_INFO::SEND_SHOPITEM_INFO() {
		ItemCount = 0;
		memset(Item, 0, sizeof(ITEMBASE)*SLOT_SHOPITEM_NUM);
	}
	int GetSize() {
		return (sizeof(SEND_SHOPITEM_INFO) - sizeof(ITEMBASE)*(SLOT_SHOPITEM_NUM-ItemCount));
	}
};

struct CHARACTERCHANGE_INFO
{
	BYTE	Gender;
	BYTE	FaceType;
	BYTE	HairType;
	float	Height;
	float	Width;

	CHARACTERCHANGE_INFO::CHARACTERCHANGE_INFO()
	{
		Gender = 0;
		FaceType = 0;
		HairType = 0;
		Height = 0;
		Width = 0;
	}
};

// 080414 LUJ, 외양 변경 메시지
struct MSG_CHARACTER_CHANGE : public MSGBASE
{
	CHARACTERCHANGE_INFO	mInfo;
	ITEMBASE				mItem;
};

struct SEND_SHOUTBASE_ITEMINFO : public MSGBASE
{	
	DWORD dwItemIdx;
	DWORD dwItemPos;
	char Name[MAX_NAME_LENGTH+1];
	char Msg[MAX_NAME_LENGTH+MAX_SHOUT_LENGTH+1];

	SEND_SHOUTBASE_ITEMINFO::SEND_SHOUTBASE_ITEMINFO()
	{
		ZeroMemory(
			this,
			sizeof(*this));
	}

	int	GetMsgLength() { return sizeof(SEND_SHOUTBASE_ITEMINFO) - MAX_SHOUT_LENGTH + strlen(Msg); }
};

enum eCheatEvent
{
	eEvent_None=0,
	eEvent_ExpRate,
	eEvent_ItemRate,
	eEvent_MoneyRate,
	eEvent_DamageReciveRate,
	eEvent_DamageRate,
	eEvent_ManaRate,
	eEvent_UngiSpeed,
	eEvent_PartyExpRate,
	eEvent_AbilRate,
	eEvent_GetMoney,
	eEvent_SkillExp,

	eEvent_Max,
};


struct MSG_GM_MOVE : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwGMIndex;
	char strGMID[MAX_NAME_LENGTH+1];
};

struct MSG_EVENTNOTIFY_ON : public MSGBASE
{
	char strTitle[32];
	char strContext[128];
	BYTE EventList[eEvent_Max];
};

struct GAMEEVENT 
{
	void Init(BYTE kind, DWORD data)
	{
		bEventKind = kind;
		dwData = data;
	}

	BYTE bEventKind;			// 이벤트의 종류
	DWORD dwData;				// 데이터1
};

struct REGISTEDGUILDINFO
{
	DWORD			GuildIdx;
	char			ImageData[GUILDMARK_BUFSIZE];
	char			GuildName[MAX_GUILD_NAME+1];
	DWORD			MarkExist;
	BYTE			Position;
	BYTE			Ranking;
	BYTE			ProcessTournament;
	DWORD			BattleID;
};

struct GUILDRANKING
{
	DWORD			GuildIdx;
	BYTE			Ranking;
};


struct GTTEAM_MEMBERINFO
{
	DWORD		TeamGuildIdx[2];
	DWORD		TeamLevel[2];
	WORD		TeamMember[2];
	WORD		TeamDieMember[2];
};

struct SEND_GTWINLOSE : public MSGBASE
{
	WORD		TournamentCount;
	DWORD		GuildIdx;
	char		GuildName[MAX_GUILD_NAME+1];
	BYTE		bWin;
	DWORD		Param;			// 어떻게 이기고 졌는가, client-몇강전 경기인가
	BYTE		bUnearnedWin;	// 부전승인가
};

struct SEND_BATTLEJOIN_INFO : public MSGBASE
{
	DWORD		AgentIdx;
	DWORD		UserLevel;
	DWORD		GuildIdx;
	DWORD		BattleIdx;
	DWORD		ReturnMapNum;
};


struct SEND_BATTLESCORE_INFO : public MSGBASE
{
	char		GuildName[2][MAX_GUILD_NAME+1];
	DWORD		Players[2][MAX_GTOURNAMENT_PLAYER];
	DWORD		PlayerScore[2][MAX_GTOURNAMENT_PLAYER];
	DWORD		Score[2];
	DWORD		FightTime;
	DWORD		EntranceTime;
	DWORD		State;
	BYTE		Team;
	WORD		GoalScore;

	SEND_BATTLESCORE_INFO::SEND_BATTLESCORE_INFO()
	{
		ZeroMemory(
			this,
			sizeof(*this));
	}
};


struct SEND_REGISTEDGUILDINFO : public MSGBASE
{
	BYTE				MaxTeam;
	BYTE				Count;
	WORD				CurTournament;
	WORD				CurRound;
	WORD				CurState;
	DWORD				PlayerID;
	DWORD				RemainTime;
	REGISTEDGUILDINFO	GInfo[MAXGUILD_INTOURNAMENT];

	SEND_REGISTEDGUILDINFO::SEND_REGISTEDGUILDINFO()
	{
		Count = 0;
		CurTournament = 0;
		PlayerID = 0;
		memset( GInfo, 0, sizeof(REGISTEDGUILDINFO)*MAXGUILD_INTOURNAMENT );
	}
	DWORD SEND_REGISTEDGUILDINFO::GetSize()
	{
		return (sizeof(SEND_REGISTEDGUILDINFO) - sizeof(REGISTEDGUILDINFO)*(MAXGUILD_INTOURNAMENT-Count));
	}
};

struct SEND_GUILDRANKING : public MSGBASE
{
	BYTE			Count;
	GUILDRANKING	Ranking[MAXGUILD_INTOURNAMENT];

	SEND_GUILDRANKING::SEND_GUILDRANKING()
	{
		Count = 0;
		memset( Ranking, 0, sizeof(GUILDRANKING)*MAXGUILD_INTOURNAMENT );
	}
	DWORD SEND_GUILDRANKING::GetSize()
	{
		return (sizeof(SEND_GUILDRANKING) - sizeof(GUILDRANKING)*(MAXGUILD_INTOURNAMENT-Count));
	}
};


struct GTBATTLE_INFO
{
	BYTE	Group;		// A, B, C, D
	DWORD	BattleID;
	char	GuildName1[MAX_GUILD_NAME+1];
	char	GuildName2[MAX_GUILD_NAME+1];
};


struct SEND_GTBATTLE_INFO : public MSGBASE
{
	BYTE			Count;
	BYTE			PlayOff;		// 예선-1, 본선-2
	GTBATTLE_INFO	BattleInfo[MAXGUILD_INTOURNAMENT/2];
	
	SEND_GTBATTLE_INFO::SEND_GTBATTLE_INFO()
	{
		Count = 0;
		memset( BattleInfo, 0, sizeof(GTBATTLE_INFO)*(MAXGUILD_INTOURNAMENT/2) );
	}
	DWORD SEND_GTBATTLE_INFO::GetSize()
	{
		return (sizeof(SEND_GTBATTLE_INFO) - sizeof(GTBATTLE_INFO)*(MAXGUILD_INTOURNAMENT/2-Count));
	}
};


struct GTBATTLE_INFOBROAD
{
	DWORD	BattleID;
	DWORD	Group;			// 몇강인지
	DWORD	GuildIdx[2];
};


struct SEND_GTBATTLE_INFOBROAD : public MSGBASE
{
	BYTE					Count;
	DWORD					CurTournament;
	GTBATTLE_INFOBROAD		BattleInfo[MAXGUILD_INTOURNAMENT/2];
	
	SEND_GTBATTLE_INFOBROAD::SEND_GTBATTLE_INFOBROAD()
	{
		Count = 0;
		memset( BattleInfo, 0, sizeof(GTBATTLE_INFOBROAD)*MAXGUILD_INTOURNAMENT/2 );
	}
	DWORD SEND_GTBATTLE_INFOBROAD::GetSize()
	{
		return (sizeof(SEND_GTBATTLE_INFOBROAD) - sizeof(GTBATTLE_INFOBROAD)*(MAXGUILD_INTOURNAMENT/2-Count));
	}
};


struct MSG_GTEDIT_PLAYERS : public MSGBASE
{
	DWORD dwRemainTime;
	DWORD dwPlayers[MAX_GTOURNAMENT_PLAYER];

	MSG_GTEDIT_PLAYERS(){dwRemainTime=0; memset(dwPlayers, 0, sizeof(dwPlayers));}
};

struct MSG_GTEDIT_PLAYERS_DWORD3 : public MSGBASE
{
	DWORD dwRemainTime;
	DWORD dwPlayers[MAX_GTOURNAMENT_PLAYER];
	DWORD dwData1;
	DWORD dwData2;
	DWORD dwData3;

	MSG_GTEDIT_PLAYERS_DWORD3(){dwRemainTime=0; memset(dwPlayers, 0, sizeof(dwPlayers));}
};


#define		MAX_GUILD_UNION_NUM		7

struct sGUILDIDXNAME
{
	DWORD	dwGuildIdx;
	char	sGuildName[MAX_GUILD_NAME+1];
};

struct SEND_GUILD_UNION_INFO : public MSGBASE
{
	DWORD			dwGuildUnionIdx;
	char			sGuildUnionName[MAX_GUILD_NAME+1];
	DWORD			dwGuildUnionMarkIdx;
	int				nMaxGuildNum;
	sGUILDIDXNAME	GuildInfo[MAX_GUILD_UNION_NUM];
	
	SEND_GUILD_UNION_INFO()
	{
		dwGuildUnionIdx = 0;
		memset( sGuildUnionName, 0, MAX_GUILD_NAME+1 );
		dwGuildUnionMarkIdx = 0;
		memset( GuildInfo, 0, sizeof(sGUILDIDXNAME)*MAX_GUILD_UNION_NUM );
		nMaxGuildNum = 0;
	}
	void AddGuildInfo( DWORD dwGuildIdx, char* pGuildName )
	{
		GuildInfo[nMaxGuildNum].dwGuildIdx = dwGuildIdx;
		strncpy( GuildInfo[nMaxGuildNum].sGuildName, pGuildName, MAX_GUILD_NAME+1 );
		++nMaxGuildNum;
	}
	int GetSize()
	{
		return ( sizeof(SEND_GUILD_UNION_INFO) - sizeof(sGUILDIDXNAME)*(MAX_GUILD_UNION_NUM-nMaxGuildNum) );
	}
};

struct SEND_CHEAT_ITEM_OPTION : public MSGBASE
{
	SEND_CHEAT_ITEM_OPTION(){memset(this, 0, sizeof(SEND_CHEAT_ITEM_OPTION));}
	POSTYPE bPosition;
	DWORD wItemIdx;
	WORD wOptionKind;

	ITEM_OPTION		OptionInfo;
};

struct MAPOBJECT_INFO
{
	DWORD	Life;
	DWORD	MaxLife;
	DWORD	PhyDefence;
	float	Radius;
};

struct AbnormalStatus
{
	// 080807 LUJ, 이상 상태 중첩될 때, 해제되면 일괄로 해제되는 문제를 수정하기 위해 카운팅하도록 함
	int	IsParalysis;
	int	IsStun;
	int	IsSlip;
	int	IsFreezing;
	int	IsStone;
	int	IsSilence;
	int	IsGod;
	int	IsBlockAttack;
	int	IsPoison;
	int	IsShield;
	int	IsBleeding;
	int	IsBurning;
	int	IsHolyDamage;
	int	IsMoveStop;
	int IsUnableUseItem;
	int IsUnableBuff;
	float Attract;
	float Poison;
	float Shield;
	float Bleeding;
	float Burning;
	float HolyDamage;
};

struct Status
{
	float	Str;
	float	Dex;
	float	Vit;
	float	Int;
	float	Wis;

	float	PhysicAttack;
	float	PhysicDefense;
	float	MagicAttack;
	float	MagicDefense;
	
	float	Accuracy;
	float	Avoid;
	float	CriticalRate;
	float	Range;
	float	MagicCriticalRate;
	float	CriticalDamage;
	float	MagicCriticalDamage;
	float	MoveSpeed;
	float	Block;
	float	CoolTime;
	float	CastingProtect;

	float	AddDamage;
	float	ReduceDamage;

	float	MaxLife;
	float	MaxMana;
	float	LifeRecoverRate;
	float	ManaRecoverRate;
	float	LifeRecover;
	float	ManaRecover;

	float	Reflect;
	float	Absorb;
	float	DamageToLife;
	float	DamageToMana;
	float	GetLife;
	float	GetMana;
	float	GetExp;
	float	GetGold;
	
	float	Attrib_None;
	float	Attrib_Earth;
	float	Attrib_Water;
	float	Attrib_Divine;
	float	Attrib_Wind;
	float	Attrib_Fire;
	float	Attrib_Dark;
	
	float	Sword;
	float	Mace;
	float	Axe;
	float	Staff;
	float	Bow;
	float	Gun;
	float	Dagger;
	float	Spear;
	float	OneHanded;
	float	TwoHanded;
	float	TwoBlade;
	float	Robe;
	float	LightArmor;
	float	HeavyArmor;
	float	Shield;

	float	NormalSpeedRate;
	float	PhysicSkillSpeedRate;
	float	MagicSkillSpeedRate;

	// 071203 KTH -- 스킬 사용시 마나 감소 %
	float	DecreaseManaRate;
	// 071204 KTH -- 드랍율 상승 %
	float	IncreaseDropRate;
	BOOL	IsProtectExp;

	Status() {memset( this, 0, sizeof( Status ) );}
};

struct	DATE_MATCHING_SEARCH_OUTPUT
{
	char		szName[MAX_NAME_LENGTH+1];									// 검색된 유저.
	DWORD		dwGoodFeelingScore;						// 호감지수
	DWORD		dwScore;								// 검색 결과 점수.
	DWORD		dwAge;									// 나이.
	DWORD		dwUserID;								// 유저 ID
};

#define	DATE_MATCHING_LIST_MAX	20

struct	MSG_DATEMATCHING_SEARCH_RESULT: public MSGBASE				// s2c 데이트 상대 리스트를 보낸다.
{
	DWORD	dwResultCount;
	DATE_MATCHING_SEARCH_OUTPUT		pList[DATE_MATCHING_LIST_MAX];
	int	GetMsgLength(){ return	sizeof(MSG_DATEMATCHING_SEARCH_RESULT) - (sizeof(DATE_MATCHING_SEARCH_OUTPUT)*DATE_MATCHING_LIST_MAX) + sizeof(DATE_MATCHING_SEARCH_OUTPUT)*dwResultCount;}
};

struct	MSG_DATEMATCHING_REQUEST_TARGET_INFO: public MSGBASE	// c2s
{
//	char	szTargetName[MAX_CHAT_LENGTH+1];
	DWORD	dwTargetUserID;							// 찾으려는 UserID;
};

struct	MSG_DATEMATCHING_TARGET_INFO: public MSGBASE			// s2c
{
	DATE_MATCHING_INFO	TargetInfo;
};

struct	MSG_DATEMATCHING_MODIFY_INTRODUCE:	public MSGBASE		// c2s
{
	// 내 소개 변경을 서버측에 갱신.
	char	szIntroduce[MAX_INTRODUCE_LEN+1];
};

// desc_hseos_몬스터미터01
// S 몬스터미터 추가 added by hseos 2007.05.23
typedef struct
{
	DWORD			nPlayTimeTick;
	DWORD			nPlayTime;
	DWORD			nKillMonsterNum;
	DWORD			nPlayTimeTotal;
	DWORD			nKillMonsterNumTotal;
} stMONSTERMETER;
// E 몬스터미터 추가 added by hseos 2007.05.23

// desc_hseos_얼굴교체01
// S 얼굴교체 추가 added by hseos 2007.06.07	2007.06.08
enum
{
	FACE_ACTION_KIND_EYE_BLINK = 0,
	FACE_ACTION_KIND_EYE_CHATTING_CMD, 
};

enum
{
	PLAYER_FACE_SHAPE_EYE_OPEN = 0,
	PLAYER_FACE_SHAPE_EYE_CLOSE,
};

typedef struct
{
	DWORD	nFaceShapeTimeTick;					// 얼굴 모양 시간 틱
	int		nFaceShapeState;					// 얼굴 모양 상태 ( 애니메이션시 필요. - 눈깜빡임)
	int		nFaceActionKind;					// 얼굴 동작 종류 ( 0: 기본 눈깜빡임, 1: 채팅창 명령어 얼굴 변경)
} stFACEACTION;
// E 얼굴교체 추가 added by hseos 2007.06.07	2007.06.08

// desc_hseos_패밀리01
// S 패밀리 추가 added by hseos 2007.07.09
// ..공용으로 사용할 결과값 정의
enum RESULT
{
	RESULT_OK = 0,								// 성공
	RESULT_OK_02,
	RESULT_OK_03,
	RESULT_OK_04,
	RESULT_OK_05,
	RESULT_OK_06,

	RESULT_FAIL_INVALID,						// 실패(시스템 범위를 벗어나는 오류)

	RESULT_FAIL_01,								// 실패01
	RESULT_FAIL_02,								// 실패02
	RESULT_FAIL_03,								// 실패03
	RESULT_FAIL_04,								// 실패04
	RESULT_FAIL_05,								// 실패05
	RESULT_FAIL_06,								// 실패06
	RESULT_FAIL_07,								// 실패07
	RESULT_FAIL_08,								// 실패08
	RESULT_FAIL_09,								// 실패09
	RESULT_FAIL_10,								// 실패10
	RESULT_FAIL_11,								// 실패11
	RESULT_FAIL_12,								// 실패12
	RESULT_FAIL_13,								// 실패13
	RESULT_FAIL_14,								// 실패14
};
// E 패밀리 추가 added by hseos 2007.07.09


//---KES Distribute Encrypt 071003
struct MSG_DIST_CONNECTSUCCESS : public MSG_DWORD
{
	HselInit eninit;
	HselInit deinit;
};

// 071226 KTH -- 이름 변경 아이템 사용
struct MSG_CHANGE_CHARACTER_NAME_SYN : public MSG_ITEM_USE_SYN
{
	char	changeName[17];
};

struct MSG_CHANGE_CHARACTER_NAME_ACK : public MSG_CHANGE_CHARACTER_NAME_SYN
{
};

struct MSG_CHANGE_CHARACTER_NAME_AGENT : public MSGBASE
{
	char changeName[17];
	int CharacterIdx;
	int UserIdx;
};

struct MSG_AUTOLIST_ADD : public MSGBASE
{
	char Name[MAX_NAME_LENGTH+1];
	char Date[20];		//2008.01.25 00:00:00  (19글자)
};

struct MSG_AUTOLIST_ALL : public MSGBASE
{
	struct AUTOLIST_ROW
	{
		char Name[MAX_NAME_LENGTH+1];
		char Date[20];
	};

	int nCount;
	AUTOLIST_ROW	row[100];

	int	GetMsgLength()
	{
		return sizeof(MSG_AUTOLIST_ALL) - ( sizeof(AUTOLIST_ROW) * ( 100-nCount ) );
	}
};

struct MSG_AUTONOTE_IMAGE : public MSGBASE
{
	BYTE Image[128*32*3];
};





// 080404 LYW --- CommonStruct : 채팅방 시스템에 필요한 데이터 구조체들.

#define MAX_USER_BUCKETCNT			100								// 유저 관리 해쉬 테이블의 최대 버킷 수.
#define MAX_ROOM_COUNT				250								// 최대 생성 채팅방 수.
#define MAX_USER_PER_ROOM			20								// 한 채팅방 내 최대 참여자 수.

#define TITLE_SIZE					64								// 제목 사이즈.

#define ROOM_COUNT_PER_PAGE			19								// 페이지 당 채팅방 수.

#define CHATROOM_DELAY_TIME			1500							// 서버에 접근 할 수 있는 딜레이 시간.

#define SECRET_CODE_SIZE			20								// 암호 사이즈.

struct ST_CR_USER													// 유저 정보.
{
	BYTE byIdx ;													// 참여중인 채팅방 인덱스.

	DWORD dwUserID ;												// 유저 인덱스.
	DWORD dwPlayerID ;												// 캐릭터 인덱스.

	//WORD  wClassIdx ;												// 클래스 인덱스.

	BYTE  byLevel ;													// 캐릭터 레벨.
	BYTE  byMapNum ;												// 현재 캐릭터 맵.

	char  name[ MAX_NAME_LENGTH+1 ] ;								// 캐릭터 이름.

	DWORD dwConnectionIdx ;
} ;

struct MSG_CR_USER : public MSGBASE									// 유저 정보 전송용 메시지.
{
	ST_CR_USER user ;												// 유저 정보.
} ;

struct ST_CR_ROOM_SRV												// 서버 관리용 채팅방 정보.
{
	BYTE byIdx ;													// 채팅방 인덱스.

	char code[SECRET_CODE_SIZE+1] ;									// 채팅방 암호.

	DWORD dwOwnerIdx ;												// 방장 인덱스.

	BYTE bySecretMode ;												// 공개/비공개 모드.
	BYTE byRoomType ;												// 채팅방 분류.

	BYTE byCurGuestCount ;											// 현재 참여 유저 수.
	BYTE byTotalGuestCount ;										// 최대 참여 유저 수.

	char title[ TITLE_SIZE+1 ] ;									// 채팅방 제목.

	DWORD dwUser[MAX_USER_PER_ROOM] ;								// 참여자 아이디.
} ;

struct ST_CR_ROOM_CLT												// 클라이언트 전용 채팅방 정보.
{
	BYTE byIdx ;													// 채팅방 인덱스.

	BYTE bySecretMode ;												// 공개/비공개 모드.
	BYTE byRoomType ;												// 채팅방 분류.

	BYTE byCurGuestCount ;											// 현재 참여 유저 수.
	BYTE byTotalGuestCount ;										// 최대 참여 유저 수.

	char title[ TITLE_SIZE+1 ] ;									// 채팅방 제목.
} ;

struct MSG_CR_ROOMINFO : public MSGBASE								// 하나의 채팅방 정보를 담는 구조체.
{
	BYTE byCount ;													// 유저 카운트.

	ST_CR_ROOM_SRV room ;											// 방정보.
	ST_CR_USER user[MAX_USER_PER_ROOM] ;							// 참여자 정보.

	int GetMsgSize()
	{
		return sizeof(MSG_CR_ROOMINFO) - (MAX_USER_PER_ROOM-byCount)*sizeof(ST_CR_USER) ;
	}
} ;

struct MSG_CR_ROOMLIST : public MSGBASE								// 채팅방을 담을 리스트 정보.
{
	BYTE byCurPage ;												// 현재 페이지.
	BYTE byTotalPage ;												// 총 페이지.

	BYTE byRoomType ;												// 채팅방 타입.

	BYTE byCount ;													// 채팅방 개수.

	ST_CR_ROOM_CLT room[ROOM_COUNT_PER_PAGE] ;						// 클라이언트 전용 채팅방.

	int GetMsgSize()												// 사이즈 반환 함수.
	{
		return sizeof(MSG_CR_ROOMLIST) - (ROOM_COUNT_PER_PAGE-byCount)*sizeof(ST_CR_ROOM_CLT) ;
	}
} ;

struct MSG_CR_ELEMENT : public MSGBASE								// 방 구성 요소 정보.
{
	BYTE byIdx ;													// 방 인덱스.

	char code[SECRET_CODE_SIZE+1] ;									// 채팅방 암호.

	BYTE bySecretMode ;												// 공개/비공개.
	BYTE byRoomType ;												// 채팅방 분류.

	BYTE byTotalGuestCount ;										// 최대 수용 인원.

	char title[ TITLE_SIZE+1 ] ;									// 채팅방 제목.
} ;

struct MSG_CR_CHANGE_OWNER : public MSGBASE							// 방장 위임 처리를 하는 메시지.
{
	DWORD dwOwnerID ;												// 방장 아이디.
	char  name[ MAX_NAME_LENGTH+1 ] ;								// 방장 이름.
} ;	

struct MSG_CR_MSG : public MSGBASE									// 한 사람에게 메시지를 전송할 때 사용하는 구조체.
{
	char  name[ MAX_NAME_LENGTH+1 ] ;								// 이름.
	char Msg[MAX_CHAT_LENGTH+1] ;									// 메시지.
} ;

struct MSG_CR_MSG_BROADCAST : public MSGBASE						// 채팅방에 접속한 사람들에게 메시지를 전송할 때 사용하는 구조체.
{
	char  name[ MAX_NAME_LENGTH+1 ] ;								// 보낸사람 이름.

	BYTE byCount ;													// 참여자 수.
	DWORD dwUser[MAX_USER_PER_ROOM] ;								// 참여자 아이디.

	char Msg[MAX_CHAT_LENGTH+1] ;									// 메시지.
} ;

struct MSG_CR_ROOM_USER : public MSGBASE							// 참여자 정보만 필요할 때 사용하는 메시지.
{
	BYTE byCount ;													// 참여자 카운트.
	DWORD dwUser[MAX_USER_PER_ROOM] ;								// 참여자 아이디.
} ;

struct MSG_CR_IDNAME : public MSG_CR_ROOM_USER						// 기본 참여자 정보에, 특별한 아이디, 이름이 추가 된 메시지.
{
	DWORD dwID ;													// 특별한 아이디.
	char  name[ MAX_NAME_LENGTH+1 ] ;								// 이름.
} ;

struct MSG_CR_MSG_RESULT : public MSGBASE							// 기본 참여자 정보에, 채팅방 내, 전송할 메시지.
{
	char  name[ MAX_NAME_LENGTH+1 ] ;								// 이름.
	char Msg[MAX_CHAT_LENGTH+1] ;									// 메시지.
} ;

struct MSG_CR_ROOM_NOTICE : public MSG_CR_ROOM_USER					// 기본 참여자 정보에, 채팅방 내 공지를 위한 메시지.
{
	ST_CR_ROOM_SRV room ;											// 채팅방 정보.
} ;

struct MSG_CR_JOIN_NOTICE	: public MSG_CR_ROOM_USER				// 기본 참여자 정보에, 채팅방 참여를 위한 메시지.
{
	ST_CR_USER user ;												// 참여한 사람의 정보.
} ;

struct MSG_CR_KICK_ACK : public MSG_CR_ROOMLIST
{
	DWORD dwKickPlayer ;
} ;

struct MSG_CR_SEARCH_NAME : public MSGBASE							// 이름 검색용 구조체.
{
	BYTE byRoomIdx ;												// 방인덱스.
	char name[MAX_NAME_LENGTH+1] ;									// 이름.
} ;

struct MSG_CR_SEARCH_TITLE : public MSGBASE							// 제목 검색용 구조체.
{
	BYTE byRoomIdx ;												// 방인덱스.
	char title[ TITLE_SIZE+1 ] ;									// 제목.
} ;

struct MSG_CR_JOIN_SYN : public MSGBASE								// 채팅방에 참여할 때 사용할 메시지.
{
	BYTE byRoomIdx ;
	char code[SECRET_CODE_SIZE+1] ;									// 채팅방 암호.
} ;

struct MSG_CR_ROOMINFO_TO_NOTICE : public MSGBASE					// 서버가 클라이언트로 정보를 전송할 때 쓰는 구조체.
{
	ST_CR_ROOM_SRV room ;
} ;

struct MSG_FISHING_READY_SYN : public MSGBASE
{
	DWORD dwPlaceIdx;
	DWORD dwPoleItemIdx;
	DWORD dwBaitItemIdx;
	WORD wBaitItemPos;
};

struct MSG_FISHING_GETFISH_SYN : public MSGBASE
{
	DWORD dwPlaceIdx;
	DWORD dwPoleItemIdx;
	WORD  wPerpectCnt;
	WORD  wGreatCnt;
	WORD  wGoodCnt;
	WORD  wMissCnt;
};

struct MSG_FISHING_GETFISH_ACK : public MSGBASE
{
	WORD wResCode;
	DWORD dwItemIdx;
};

struct MSG_FISHING_MISSION_CODE : public MSGBASE
{
	DWORD dwItem1;
	DWORD dwItem2;
	DWORD dwItem3;
	DWORD dwProcessTime;
};

struct FISHITEMINFO
{
	DWORD dwItemIdx;
	DURTYPE dur;
	POSTYPE pos;
};

struct MSG_FISHING_FPCHANGE_SYN : public MSGBASE
{
	DWORD dwAddFishPoint;
	FISHITEMINFO FishList[12];
};

struct MSG_PET_ADD : public MSGBASE
{
	DWORD	MasterIdx;
	BASEOBJECT_INFO BaseObjectInfo;
	SEND_MOVEINFO MoveInfo;
	PET_OBJECT_INFO PetObjectInfo;
	ITEMBASE ItemInfo[ ePetEquipSlot_Max ];
	BOOL	IsLogin;
	BOOL	IsSummoned;							// 091214 ONS 아이템소환/맵이동을 판단하는 플래그.
	CAddableInfoList AddableInfo;

	WORD GetMsgLength()	{	return sizeof(SEND_CHARACTER_TOTALINFO) - sizeof(CAddableInfoList) + AddableInfo.GetInfoLength();	}
};

struct MSG_PET_INFO : public MSGBASE
{
	PET_OBJECT_INFO PetObjectInfo;
};

struct MSG_PET_ITEM_INFO : public MSGBASE
{
	DWORD	ID;
	ITEMBASE	Item[ePetEquipSlot_Max];
};

// 080916 LUJ, 아이템 합성 요청
struct MSG_ITEM_COMPOSE_SYN : public MSGBASE
{
	// 080916 LUJ, 합성 시 부여될 옵션 종류
	ITEM_OPTION::Drop::Key	mOptionKey;
	// 080916 LUJ, 합성 결과 중에는 '랜덤'이 있다. 그 때 이 플래그가 참 값이다
	BOOL					mIsRandomResult;
	// 080916 LUJ, 합성 시작에 필요한 아이템
	ICONBASE				mKeyItem;
	// 080916 LUJ, 합성 결과로 선택한 아이템
	ICONBASE				mResultItem;
	// 080916 LUJ, 소요되는 아이템 정보
	DWORD					mUsedItemSize;
	ICONBASE				mUsedItem[ MAX_ITEMBUY_NUM ];
	
	DWORD GetSize() const
	{
		return sizeof( MSG_ITEM_COMPOSE_SYN ) - ( sizeof( mUsedItem ) / sizeof( *mUsedItem ) - mUsedItemSize ) * sizeof( *mUsedItem );
	}
};

// 080916 LUJ, 아이템 합성 실패
struct MSG_ITEM_COMPOSE_NACK : public MSGBASE
{
	enum Type
	{
		TypeNone,
		TypeInvalidUsedItem,
		TypeNotMatchedScript,
		TypeNotExistedScript,
		TypeNotExistedOption,
		TypeWrongUsedItemSize,
		TypeInvalidResult,
		TypeInvalidKeyItem,
		TypeEmptyResult,
		TypeFailedUpdateMemory,
	}
	mType;
};

// 080916 LUJ, 아이템 합성 결과
struct MSG_ITEM_COMPOSE_ACK : public MSGBASE
{
	// 090122 LUJ, 클라이언트 측 변경이 쉽도록 ITEMBASE 타입으로 변경
	ITEMBASE	mResultItem;
	ICONBASE	mUsedKeyItem;
	DWORD		mUsedItemSize;
	ICONBASE	mUsedItem[ MAX_ITEMBUY_NUM ];

	DWORD GetSize() const
	{
		return sizeof( MSG_ITEM_COMPOSE_ACK ) - ( sizeof( mUsedItem ) / sizeof( *mUsedItem ) - mUsedItemSize ) * sizeof( *mUsedItem );
	}
};

enum SIEGERECALLMGR_COMMAND_KIND
{
	e_CK_ADD_THEME = 0,
	e_CK_ADD_THEME_IDX,
	e_CK_ADD_MAP,
	e_CK_ADD_STEP,
	e_CK_ADD_OBJECT,
	e_CK_START_TERM,
	e_CK_END_TERM,
	e_CK_COMMAND,
	e_CK_DIE_RECALL_OBJ,
	e_CK_CHECK_DIE_OBJ,
	e_CK_USE_ITEM,

	e_CK_MAX,
} ;

// Enumerate a terms to need recall and destroy or object.
enum SIEGERECALLMGR_TERM_KINDS
{
	e_TERM_NONE = 0,

	e_TERM_DIE_OBJ,
	e_TERM_USE_ITEM,

	e_TERM_MAX,									// 최대 수치 체크용.
} ;

// 
enum SIEGERECALLMGR_COMMAND_KINDS
{
	e_COMMAND_NONE = 0,

	e_COMMAND_RECALL_DIE_OBJ,

	e_COMMAND_MAX,
} ;

// 스텝 종류를 정의 한다.
enum SIEGERECALLMGR_STEP_KINDS
{
	e_STEP_ADD_OBJ = 0,

	e_STEP_MAX,									// 최대 수치 체크용.
} ;

// 
enum SIEGEBASEKIND
{
	e_SIEGE_BASEKIND_EMPTY = 0,
	e_SIEGE_BASEKIND_START_TERM,
	e_SIEGE_BASEKIND_END_TERM,
	e_SIEGE_BASEKIND_COMMAND,

	e_SIEGE_BASEKIND_MAX,
} ;

// 
enum SIEGERECALL_REMOVETYPE
{
	e_SIEGERECALL_REMOVE_ALL_MAP = 0,
	e_SIEGERECALL_REMOVE_SPECIFY_MAP,

	e_SIEGERECALL_REMOVE_ALL_OBJ,
	e_SIEGERECALL_REMOVE_SPECIFY_OBJ,
} ;

enum SIEGERECALL_OBJTYPE
{
	e_ObjParent = 0 ,
	e_ObjChild,
} ;

struct st_SIEGEOBJ
{
	WORD wThemeIdx ;

	MAPTYPE	mapNum ;

	BYTE	byStepIdx ;

	DWORD	dwObjectIdx ;

	float	fXpos ;
	float	fZpos ;

	BYTE	byUseRandomPos ;

	BYTE	byRadius ;

	BYTE	byAddObjIdx ;

	BYTE	byComKind ;

	BYTE	byComIndex ;

	WORD	wParentMap ;
} ;

// 
//struct st_SIEGEOBJ
//{
//	MAPTYPE	mapNum ;
//	WORD	wObjectKind ;
//	DWORD	dwObjectIdx ;
//
//	float	fXpos ;
//	float	fZpos ;
//} ;

// 
struct st_DIE_RECALL_OBJ
{
	BYTE byIdx ;

	st_SIEGEOBJ objInfo ;
} ;

//
struct st_CHECK_OBJ_DIE
{
	BYTE byIdx ;

	MAPTYPE	mapNum ;

	BYTE byObjectIdx ;
} ;

struct st_CHECK_USE_ITEM
{
	BYTE byIdx ;

	DWORD dwItemIdx ;

	WORD wUseCount ;
} ;

struct MSG_SIEGERECALL_OBJINFO : public MSGBASE
{
	WORD wExValue ;

	st_SIEGEOBJ siegeObj ;
} ;

struct MSG_SIEGERECALL_PARENTINFO : public MSGBASE
{
	MAPTYPE p_mapNum ;

	BYTE p_byStepIdx ;

	//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
	//BYTE p_byChildIdx ;
	DWORD p_dwObjectIdx ;
} ;

struct MSG_SIEGERECALL_CHECK_OBJ_DIE : public MSG_SIEGERECALL_PARENTINFO
{
	BYTE byCheckKind ;

	MAPTYPE check_mapNum ;
	BYTE check_ObjIdx ;
} ;

struct MSG_SIEGERECALL_CHECK_USE_ITEM : public MSG_SIEGERECALL_PARENTINFO
{
	BYTE byCheckKind ;

	DWORD dwItemIdx ;
	WORD wItemCount ;
} ;

struct MSG_SIEGERECALL_COMMAND_DIE_RECALL_OBJ : public MSG_SIEGERECALL_PARENTINFO
{
	BYTE byCommandKind ;

	st_DIE_RECALL_OBJ dieRecallObj ;
} ;

struct MSG_SIEGERECALL_LOADOBJINFO : public MSGBASE
{
	BYTE	ThemeIdx ;
	WORD	MapIdx ;
	WORD	RecallMapIdx ;
	BYTE	StepIdx ;
	BYTE	AddObjIdx ;
	BYTE	ComKind ;
	BYTE	ComIndex ;
} ;

// 090706 pdy 워터시드 사용 요청 
struct MSG_SIEGEWARFARE_USE_WATERSEED_SYN : public MSG_ITEM_USE_SYN 
{
	DWORD dwWaterSeedUsingSecond;
};

// 080805 KTH -- 워터시드 사용 완료
struct MSG_WATERSEED_COMPLETE : public MSGBASE
{
	DWORD	dwMapType;
	DWORD	dwGuildIdx;
	char	szGuildName[MAX_NAME_LENGTH+1];
};

// 080819 LYW --- CommonStruct : NPC 소환 관련 정보를 정의한다.
enum NPCRECALL_TYPE													// NPC 소환 조건 분류.
{
	eNR_None = 0,													// 소환 조건이 없는 기본 상태.
	eNR_ItSelf,														// 스스로 소환되는 타입.
	eNR_RemainTime,													// 한번 소환 후, 일정 시간이 지나면 사라지는 타입.
	eNR_Moveable,													// 스스로 이동이 가능한 타입.

	eNR_NpcRecall_Max,												// 소환조건 제한 체크용.
} ;

enum RECALL_NPC_ERROR_TYPE											// NPC 소환실패 에러 분류 정의.
{
	e_RNET_INVALID_NPC_TYPE =0,										// 잘못 된 NPC 타입으로 인해 실패.
	e_RNET_FAILED_RECEIVE_REMAINNPC,								// 일정 시간동안 소환/소멸되는 NPC 정보를 받는데 실패.

	e_RNET_FAILED_RECEIVE_STATICNPCINFO,							// STATICNPC 리스트에서 NPC 정보를 받는데 실패.
	e_RNET_FAILED_CREATE_NPC,										// NPC 생성 실패.
	e_RNET_FAILED_RECEIVE_NPCLIST,									// NPC 리스트 받기 실패.
	e_RNET_FAILED_RECEIVE_NPC_FROM_USERTABLE,						// 맵 서버의 유저테이블에서, NPC 정보 받는데 실패.
	e_RNET_FAILED_RECEIVE_SERVERPORT,								// 서버 포트 정보 받는데 실패.
	e_RNET_FAILED_RECEIVE_NPCINFO,									// NPC 정보를 받는데 실패.
	e_RNET_FAILED_RECEIVE_USERINFO,									// 유저 정보를 받는데 실패.
	e_RNET_FAILED_CHANGEMAP_INVALID_STATE,							// 맵이동을 할 수 없는 상태이기 때문에, 맵 이동 실패.
	e_RNET_FAILED_ALREADY_RECALLED,									// 이미 소환 된 NPC라 소환 실패.
	e_RNET_FAILED_NOT_SIEGEWARTIME,									// 공성전 중이 아니라 이동할 수 없다.
	e_RNET_FAILED_CHECK_HACK_RECALLNPC_CHECK,						// 서버 NPC 인증에서 실패 

	e_RNET_MAX,														// 에러 제한 체크용.
} ;

struct MSG_NPCRECALL_INFO : public MSGBASE
{
	DWORD		dwCreatedIdx ;										// 맵 서버에서 소환(생성)된 아이디 저장용 변수.

	BYTE		byRecallCodition ;									// 소환 조건.

	DWORD		dwNpcIdx ;											// Npc 인덱스. StaticNpcList.bin의 Unique index를 사용한다.
	WORD		wNpcKind ;											// Npc 종류. NpcList.bin의 Kind를 참조한다.

	MAPTYPE		RecallMapNum ;										// 소환 맵 번호.

	float		fXpos ;												// 좌표 X.
	float		fZpos ;												// 좌표 Z.

	BYTE		byActive ;											// 활성화 여부를 담는 변수.
} ;

struct MSG_NPCRECALL_CHANGEMAP : public MSGBASE
{
	DWORD	dwNpcIdx ;												// 이동을 하게 하는 npc 인덱스.

	MAPTYPE CurMapNum ;												// 현재 맵 번호.
	MAPTYPE ChangeMapNum ;											// 이동할 맵 번호.

	BYTE    ChannelNum ;											// 채널 번호.

	float	fChangeMapXPos ;										// 이동할 맵의 X좌표.
	float	fChangeMapZPos ;										// 이동할 맵의 Z좌표.
} ;

// 090227 ShinJS --- Coin Item 구입시 Coin 제거 정보
struct MSG_ITEM_BUY_BY_COIN : public MSGBASE
{
	DWORD		mSize;												// 변경되는 Coin Item의 개수
	ITEMBASE	mItem[ SLOT_MAX_INVENTORY_NUM ];					// 변경되는 Coin Item 정보

	DWORD GetSize() const
	{
		return sizeof( MSG_ITEM_BUY_BY_COIN ) - ( sizeof( mItem ) / sizeof( *mItem ) - mSize ) * sizeof( *mItem );
	}
};

// 090317 ONS 길드 채팅 메세지추가
struct MSG_GUILD_CHAT : public MSGBASE
{
	DWORD dwGuildIdx;
	DWORD dwSenderIdx;
	char Msg[MAX_CHAT_LENGTH+1];
	int GetMsgLength() { return sizeof(MSG_GUILD_CHAT) - MAX_CHAT_LENGTH + strlen(Msg); }
};

// 090317 ONS 길드연합 채팅 메세지추가
struct MSG_GUILDUNION_CHAT : public MSGBASE
{
	DWORD	dwGuildUnionIdx;
	char	Name[MAX_NAME_LENGTH+1];
	char	Msg[MAX_CHAT_LENGTH+1];
	int		GetMsgLength() { return sizeof(MSG_GUILDUNION_CHAT) - MAX_CHAT_LENGTH + strlen(Msg); }
};

// 090317 ONS 길드, 연합 노트 메세지추가
struct MSG_GUILD_UNION_SEND_NOTE : public MSGBASE
{
	DWORD dwGuildUnionIdx;
	char FromName[MAX_NAME_LENGTH+1];
	char Note[MAX_NOTE_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_GUILD_UNION_SEND_NOTE) - (MAX_NOTE_LENGTH+1) + strlen(Note)+1);
	}
};

// 090422 ShinJS --- 탈것 소환 정보
struct MSG_VEHICLE_SUMMON : public MSGBASE
{
	ICONBASE IconBase;				// 소환 Item 정보
	COMPRESSEDPOS SummonPos;		// 소환 위치
};

struct MSG_VEHICLE_ERROR : public MSG_DWORD
{
	enum Error
	{
		ErrorNone,
		ErrorSummonByItem,
		ErrorSummonByDistance,
		ErrorSummonByUsingItem,
		ErrorSummonByCreateVehicle,
		ErrorSummonAlready,
		ErrorSummonByMount,
		ErrorSummonByState,
		ErrorUnsummonByDistance,
		ErrorUnsummonByNoMine,
		ErrorUnsummonByPassenger,
		ErrorUnsummonByState,
		ErrorMountByDistance,
		ErrorMountByFullSeat,
		ErrorMountBySitAlready,
		ErrorMountByNoVehicle,
		ErrorMountByOverSeatIndex,
		ErrorMountByNoOwner,
		ErrorMountByPlayerLevel,
		ErrorMountBySummonedAlready,
		ErrorMountByState,
		ErrorMountByCollision,
		ErrorDismountbyDistance,
		ErrorDismountByNotSit,
		ErrorDismountByState,
		ErrorMoveByNoOwner,
		ErrorTollByFullSeat,
		ErrorTollByNoOwner,
		ErrorTollByNoPassenger,
		ErrorTollByOwnerHasFullMoney,
		ErrorTollByPassengerHasInsufficientMoney,
		ErrorTollByInvalidKey,
		ErrorInHouse,
	}
	mError;
};

// 090316 LUJ, 좌석 번호별로 탑승한 플레이어 번호가 들어있다
struct MSG_VEHICLE_PASSENGER : public MSGBASE
{
	DWORD mSeat[ VehicleScript::Seat::MaxSize ];
};

// 090316 LUJ, 탑승 허락
struct MSG_VEHICLE_ALLOW : public MSGBASE
{
	DWORD mOwnerObjectIndex;
	DWORD mMountedObjectIndex;
	DWORD mMountedSeatIndex;
	char mMountedObjectName[ MAX_NAME_LENGTH ];
	DWORD mVehicleObjectIndex;
	char mVehicleObjectName[ MAX_NAME_LENGTH ];
	MONEYTYPE mTollMoney;
};

struct MSG_LIMITDUNGEON_ERROR : public MSGBASE
{
	enum Error
	{
		ErrorNone,
		ErrorMapMove,
		ErrorRoom,
		ErrorLevel,
		ErrorItem,
	}
	mError;
};

struct MSG_HOUSE_CREATE : public MSGBASE
{
	DWORD dwPlayerID;
	char szHouseName[MAX_HOUSING_NAME_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_HOUSE_CREATE) - (MAX_HOUSING_NAME_LENGTH+1) + strlen(szHouseName)+1);
	}
};

struct MSG_HOUSE_CREATESRV : public MSGBASE
{
	DWORD dwPlayerID;
	DWORD dwPlayerMapNum;
	DWORD dwConnectionIndex;
	char szHouseName[MAX_HOUSING_NAME_LENGTH+1];
	WORD GetMsgLength()
	{
		return static_cast<WORD>(sizeof(MSG_HOUSE_CREATE) - (MAX_HOUSING_NAME_LENGTH+1) + strlen(szHouseName)+1);
	}
};

struct MSG_HOUSE_HOUSEINFO : public MSGBASE
{
	BOOL				bInit;			// 최초여부
	CHAR				cRank;			// 1,2,3위만 기록되고 나머지는 0
	DWORD				dwChannel;		// 하우스 채널
	DWORD				dwDecoPoint;	// DecoPoint != (HousePoint와는 다름)
	stHouseInfo			HouseInfo;		// 하우징 정보
};

struct MSG_HOUSE_FURNITURELIST : public MSGBASE
{
	WORD wCategory;
	WORD wNum;
	stFurniture Furniture[MAX_HOUSING_SLOT_NUM];

	WORD GetMsgLength()
	{
		return sizeof(MSG_HOUSE_FURNITURELIST) - sizeof(stFurniture)*(MAX_HOUSING_SLOT_NUM-wNum);
	}
};

struct MSG_HOUSE_FURNITURE : public MSGBASE
{
	WORD wState;
	DWORD dwDecoPoint;
	stFurniture Furniture;
};

struct MSG_HOUSE_FURNITURE_INSTALL : public MSGBASE
{
	DWORD dwChannel;
	DWORD dwFurnitureObjectIndex;
	WORD wSlot;

	float fAngle;
	VECTOR3 vPos;
};

struct stHouse_CheatInfo
{
	DWORD dwChannelID;
	DWORD dwOwnerUserIndex;
	DWORD dwVisiterNum;
	char szHouseName[MAX_HOUSING_NAME_LENGTH + 1];
};

struct stHouse_CheatInfoDetail
{
	DWORD dwChannelID;
	DWORD dwOwnerUserIndex;
	DWORD dwVisiterNum;
	char szHouseName[MAX_HOUSING_NAME_LENGTH + 1];
	char szOwnerName[MAX_HOUSING_NAME_LENGTH + 1];

	DWORD m_dwFurnitureNum[MAX_HOUSING_CATEGORY_NUM];
	DWORD m_dwInstalledNum[MAX_HOUSING_CATEGORY_NUM];

	stHouse_CheatInfoDetail()
	{
		strcpy(szHouseName, "");
		strcpy(szOwnerName, "");
		ZeroMemory(m_dwFurnitureNum, sizeof(m_dwFurnitureNum));
		ZeroMemory(m_dwInstalledNum, sizeof(m_dwInstalledNum));
	};
};

struct MSG_HOUSE_CHEATINFO_ONE : public MSGBASE
{
	stHouse_CheatInfoDetail HouseInfo;

	WORD GetMsgLength()
	{
		return sizeof(MSG_HOUSE_CHEATINFO_ONE);
	}
};

struct MSG_HOUSE_CHEATINFO_ALL : public MSGBASE
{
	DWORD dwHouseNum;
	DWORD dwUserNum;
	DWORD dwReservationNum;
	stHouse_CheatInfo HouseInfo[MAX_HOUSE_NUM];

	WORD GetMsgLength()
	{
		return (WORD)(sizeof(MSG_HOUSE_CHEATINFO_ALL) - sizeof(stHouse_CheatInfo)*(MAX_HOUSE_NUM-dwHouseNum));
	}
};

struct MSG_HOUSE_RANK_UPDATE : public MSGBASE
{
	stHouseRank HouseRank;
};

struct MSG_HOUSE_EXTEND : public MSGBASE
{
	WORD wExtendLevel;
	WORD wUnInstallNum;
	WORD wInstallNum;
	DWORD dwDecoPoint;

	stFurniture UnInstall[MAX_HOUSING_SLOT_NUM];
	stFurniture Install[MAX_HOUSING_SLOT_NUM];

	WORD GetMsgLength()
	{
		return sizeof(MSG_HOUSE_EXTEND) - sizeof(UnInstall)*(MAX_HOUSING_SLOT_NUM-wUnInstallNum) - sizeof(Install)*(MAX_HOUSING_SLOT_NUM-wInstallNum);
	}
};

struct MSG_HOUSE_VISIT : public MSGBASE
{
	CHAR cKind;			//eHouseVisitByCheat=0, eHouseVisitByLink=1, eHouseVisitByItem=3
	DWORD dwValue1;		//eHouseVisitByLink(Npc인덱스), eHouseVisitByItem(아이템Index)
	DWORD dwValue2;		//eHouseVisitByItem(아이템Slot)

	char Name[MAX_NAME_LENGTH+1];
};

struct MSG_HOUSE_VISITSRV : public MSGBASE
{
	CHAR cKind;
	DWORD dwValue1;
	DWORD dwValue2;
	DWORD dwConnectionIndex;
	DWORD dwMapNum;

	char Name[MAX_NAME_LENGTH+1];
};

struct MSG_DUNGEON_INFO : public MSGBASE
{
	DWORD dwIndex;
	DWORD dwPartyIndex;
	DWORD dwChannelID;
	DWORD dwPoint;
	DWORD dwJoinPlayerNum;
	eDIFFICULTY difficulty;
	stWarpState warpState[MAX_DUNGEON_WARP];
	stSwitchNpcState SwitchNpcState[MAX_DUNGEON_SWITCH];
};

struct stDungeonSummary
{
	DWORD dwChannel;
	DWORD dwPartyIndex;
	DWORD dwJoinPlayerNum;
	DWORD dwPoint;
	eDIFFICULTY	difficulty;
};

struct MSG_DUNGEON_INFO_ALL : public MSGBASE
{
	DWORD dwDungeonNum;
	DWORD dwUserNum;
	stDungeonSummary DungeonInfo[MAX_DUNGEON_NUM];

	WORD GetMsgLength()
	{
		return (WORD)(sizeof(MSG_DUNGEON_INFO_ALL) - sizeof(stDungeonSummary)*(MAX_DUNGEON_NUM-dwDungeonNum));
	}
};

struct stDungeonDetail
{
	stDungeonSummary DungeonSummary;
	
	PARTY_MEMBER	PartyMember[MAX_PARTY_LISTNUM];
	stWarpState warpState[MAX_DUNGEON_WARP];
	int			switchState[MAX_DUNGEON_SWITCH];
};

struct MSG_DUNGEON_INFO_ONE : public MSGBASE
{
	stDungeonDetail DungeonInfo;
};

// 100601 ONS 채팅금지 설정 정보 구조체
struct MSG_FORBID_CHAT : public MSGBASE
{
	DWORD	dwData;
	char	Name[MAX_NAME_LENGTH + 1];
	char	Reason[MAX_CHAT_LENGTH + 1];
};

// 100722 ONS 파티생성시 Master의 Tip정보 등록처리 추가.
struct MSG_PARTY_CREATE : public MSGBASE
{
	BYTE PartyOpt;
	BYTE JobGrade;
	BYTE Race;
	BYTE DiceGrade;
	MAPTYPE MapType;
	DWORD PartyIDX;
	DWORD MasterID;
	DWORD MasterLv;

	BYTE Job[MAX_JOB_GRADE];
	char Name[MAX_NAME_LENGTH + 1];
};


struct stEnchantGoldRate
{
	float	fLevelRate;		// 아이템레벨 상수
	float	fGradeRate;		// 아이템등급 상수
	float	fBasicRate;		// 기본 상수
};

// 090713 ShinJS --- 인챈트 레벨에 대한 보너스적용 수치 정보
struct stEnchantLvBonusRate
{
	LEVELTYPE minLevel;			// 최소 레벨
	LEVELTYPE maxLevel;			// 최대 레벨	
	float fEnchantLvRate1;		// 인챈트 레벨 상수1
	float fEnchantLvRate2;		// 인챈트 레벨 상수2
};

// 090909 ONS 스킬트리 정보
struct SkillData
{
	DWORD	index;
	BYTE	level;
};

struct LimitDungeonScript
{
	DWORD mNameIndex;
	MAPTYPE mMapType;
	// 091106 LUJ, 컨텐츠 종류
	DWORD mChannel;
	TCHAR mMonsterScript[MAX_PATH];
	struct Date
	{
		// 091106 LUJ, 0~6. 일요일~토요일
		int mDayOfWeek;
		int mStartHour;
		int mStartmMinute;
		int mPlayTime;
	};
	typedef std::list< Date > DateList;
	DateList mDateList;
	struct Level
	{
		LEVELTYPE mMin;
		LEVELTYPE mMax;
	}
	mLevel;
	struct Item
	{
		DWORD mItemIndex;
		DURTYPE mQuantity;
	}
	mItem;
	struct Home
	{
		MAPTYPE mMapType;
		VECTOR3 mPosition;
	}
	mHome;
	struct Alarm
	{
		__time64_t mTime;
		DWORD mMessage;
	};
	typedef std::list< Alarm > AlarmList;
	AlarmList mAlarmList;
	typedef std::list< DWORD > DescriptionList;
	DescriptionList mDescriptionList;

	LimitDungeonScript() :
	mNameIndex(0),
	mMapType(0),
	mChannel(0)
	{
		const Level emptyLevel = {0, USHRT_MAX};
		mLevel = emptyLevel;
		ZeroMemory(mMonsterScript, sizeof(mMonsterScript));
		ZeroMemory(&mItem, sizeof(mItem));
		ZeroMemory(&mHome, sizeof(mHome));
	}
};

struct stTime64t : public MSGBASE
{
	__time64_t time64t;
};

#pragma pack(pop)

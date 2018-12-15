#ifndef __GAMERESOURCESTRUCT_H__
#define __GAMERESOURCESTRUCT_H__

class cActiveSkillInfo;
enum EObjectKind;

class RECALLINFO
{
public:
	DWORD	RecallType;
	DWORD	RecallID;
	DWORD	RecallMonsterNum;
	DWORD	RecallRange;
};

class CRecallInfo
{
public:
	RECALLINFO * pRecallInfo;

	CRecallInfo(){ memset(this, 0, sizeof(CRecallInfo)); }
	virtual ~CRecallInfo()
	{
		if(pRecallInfo)
		delete [] pRecallInfo;
	}
	void Init(DWORD nRecall)
	{
		MaxRecallNum = nRecall;
		pRecallInfo = new RECALLINFO[MaxRecallNum];
		memset(pRecallInfo, 0, sizeof(RECALLINFO)*MaxRecallNum);
	}
	
	RECALLINFO * GetRecallInfo(DWORD dwIdx)
	{
		ASSERT(dwIdx < MaxRecallNum);
		return &pRecallInfo[dwIdx];
	}
	void SetRecallType(DWORD dwIdx, DWORD dwType)
	{
		pRecallInfo[dwIdx].RecallType = dwType;
	}
	void SetRecallID(DWORD dwIdx, DWORD dwID)
	{
		pRecallInfo[dwIdx].RecallID = dwID;
	}
	void SetRecallMonsterNum(DWORD dwIdx, DWORD dwNum)
	{
		pRecallInfo[dwIdx].RecallMonsterNum = dwNum;
	}
	void SetRecallRange(DWORD dwIdx, DWORD dwRange)
	{
		pRecallInfo[dwIdx].RecallRange = dwRange;
	}
protected:
	DWORD	MaxRecallNum;
	
};

struct MONSTER_DROPITEM
{
	DWORD wItemIdx;
	DWORD dwDropPercent;
	BYTE byCount;
};

struct BASE_MONSTER_LIST
{
	WORD		MonsterKind;			// 번호
	EObjectKind	ObjectKind;				// Object번호 32 일반 33 보스 35 필드보스 36 필드보스부하
	char		Name[MAX_MONSTER_NAME_LENGTH +1];				// 몬스터이름	
	LEVELTYPE	Level;					// 몬스터레벨	
	char		ChxName[32];			// 몬스터 CHX_Index	
	float		Scale;					// 몬스터 크기	
	DWORD		Gravity;				// 몬스터 비중
	float		Weight;					// 몬스터 무게	
	DWORD		MonsterRadius;
	DWORD		Life;					// 생명력	
	DWORD		ExpPoint ;
	WORD		Tall;					// 키
	WORD		Accuracy;
	WORD		Avoid;
	WORD		AttackPhysicalMin;// 공격 최소물리 공격력	
	WORD		AttackPhysicalMax;// 공격 최대물리 공격력
	WORD		AttackMagicalMin;// 공격 최소마법 공격력	
	WORD		AttackMagicalMax;// 공격 최대마법 공격력
	WORD		CriticalPercent;// 크리티컬 퍼센트
	WORD		PhysicalDefense;// 물리 방어력	
	WORD		MagicalDefense;// 마법 방어력	
	WORD		WalkMove;// 몬스터걷기이동력	
	WORD		RunMove;// 몬스터뛰기이동력	
	WORD		RunawayMove;// 몬스터도망이동력

	int			Damage1DramaNum;// 맞음 1 연출 번호	
	int			Die1DramaNum;// 죽음 1 연출 번호	
	int			Die2DramaNum;// 죽음 2 연출 번호	

	DWORD		StandTime;
	BYTE		StandRate;
	BYTE		WalkRate;
	BYTE		RunRate;
	DWORD		DomainRange;					// 영역 범위 : 반지름
	DWORD		PursuitForgiveTime;
	DWORD		PursuitForgiveDistance;	

	BOOL		bForeAttack;					// 선공격
	DWORD		SearchPeriodicTime;				// 탐색 주기
	WORD		TargetSelect;					// 타겟 선택 : FIND_CLOSE, FIND_FIRST
	DWORD		SearchRange;
	DWORD		SearchAngle;
	WORD		TargetChange;
	DWORD		AttackNum;
	DWORD		AttackIndex[5];
	DWORD		AttackRate[5];
	cActiveSkillInfo* SkillInfo[5];
	// 100104 LUJ, 몬스터가 주변에 도움을 요청한다
	enum HelpType
	{
		HelpNone,
		HelpLifeIsUnder50Percent,
		HelpLifeIsUnder30Percent,
		HelpDie,
		HelpAlways,
	}
	mHelpType;
	BOOL		bDetectSameRace;
	BOOL		bHearing;
	DWORD		HearingDistance;

	WORD		SpecialType;
	WORD		MonsterType;
	WORD		MonsterRace;
	WORD		MonsterAttribute;

	BYTE		BaseReinforce;
	BYTE		PhysicAttackReinforce;
	BYTE		MagicAttackReinforce;
	BYTE		PhysicDefenseReinforce;
	BYTE		MagicDefenseReinforce;

	MONEYTYPE dropMoney;
	DWORD dwMoneyDropRate;
	MONSTER_DROPITEM dropItem[eDROPITEMKIND_MAX - 1];
	DWORD		dwMonsterTargetType;	//몬스터 GuageTargetDlg Type
};

typedef struct _CHXLIST
{
	StaticString FileName;
}CHXLIST;

//-----------------------------------------------------------------------
// 맵전환 좌표기억 구조체
//-----------------------------------------------------------------------
typedef struct _MAPCHANGE_INFO
{
	WORD Kind;

	// 071214 LUJ, 맵 이름이 MAX_NAME_LENGTH 길이를 초과하여 길이를 새로 지정 
	char CurMapName[ MAX_MONSTER_NAME_LENGTH + 1 ];
	char ObjectName[ MAX_MONSTER_NAME_LENGTH + 1 ];
	//char CurMapName[MAX_NAME_LENGTH+1];
	//char ObjectName[MAX_NAME_LENGTH+1];

	WORD CurMapNum;
	WORD MoveMapNum;
	VECTOR3 CurPoint;
	VECTOR3 MovePoint;
	WORD chx_num;
	
	// 방향설정
}MAPCHANGE_INFO;

//-----------------------------------------------------------------------
// LogIn 좌표기억 구조체
//-----------------------------------------------------------------------
typedef struct _LOGINPOINT_INFO
{
	WORD Kind;
	char MapName[MAX_NAME_LENGTH+1];
	WORD MapNum;
	BYTE MapLoginPointNum;
	VECTOR3 CurPoint[10];
	WORD chx_num;

	// 방향설정
}LOGINPOINT_INFO;

//-----------------------------------------------------------------------
// NPC 정보
//-----------------------------------------------------------------------
struct NPC_LIST
{
	WORD NpcKind;			// Npc 종류
	char Name[MAX_NPC_NAME_LENGTH+1];			// Npc 이름
	WORD ModelNum;			// MonsterChxList에서의 번호
	WORD JobKind;			// 직업종류별구분
	float Scale;			// Npc 크기
	WORD Tall;				// 키
	BOOL ShowJob;
};

//-----------------------------------------------------------------------
// NPC 리젠
//-----------------------------------------------------------------------
typedef struct _NPC_REGEN
{

	_NPC_REGEN():dwObjectID(0),NpcKind(0)
	{
		Pos.x = Pos.y = Pos.z = 0;
	}
	DWORD	dwObjectID;		// 신규 추가 taiyo
	MAPTYPE	MapNum;			// Map번호
	WORD	NpcKind;		// NpcKind
	char	Name[MAX_NAME_LENGTH+1];		// Npc 이름
	WORD	NpcIndex;		// NpcIndex(고유번호로 사용)
	VECTOR3 Pos;			// 몬스터 위치
	float	Angle;			// 몬스터 각도
}NPC_REGEN;


//-----------------------------------------------------------------------
// MOD 리스트 
//-----------------------------------------------------------------------
struct MOD_LIST
{
	MOD_LIST()
	{
		MaxModFile = 0;
		ModFile = NULL;
	}
	~MOD_LIST()
	{
		if(MaxModFile == 0)
			return;
		SAFE_DELETE_ARRAY(ModFile);
	}
	DWORD MaxModFile;
	StaticString* ModFile;
	StaticString BaseObjectFile;	
};

struct ITEM_INFO
{
	DWORD ItemIdx;
	char ItemName[MAX_ITEMNAME_LENGTH + 1];
	DWORD ItemTooltipIdx;
	WORD Image2DNum;
	WORD Part3DType;
	WORD Part3DModelNum;
	LIMIT_RACE LimitRace;
	WORD LimitGender;
	LEVELTYPE LimitLevel;
	WORD Shop;
	WORD Stack;
	WORD Trade;
	WORD Deposit;
	WORD Sell;
	WORD Decompose;
	WORD Repair;
	BOOL IsEnchant;		// 업그레이드 가능 여부
	WORD Improvement;

	DWORD EnchantValue;	// 업글 성공 확률 변수
	DWORD EnchantDeterm;	// 업글 단계마다 부여되는 수치

	DWORD DecomposeIdx;
	DWORD Time;
	MONEYTYPE BuyPrice;
	MONEYTYPE SellPrice;
	LEVELTYPE Grade;
	DURTYPE Durability;
	int PhysicAttack;
	int MagicAttack;
	int PhysicDefense;
	int MagicDefense;
	eItemCategory Category;
	eEquipType EquipType;
	EWEARED_ITEM EquipSlot;
	eWeaponAnimationType WeaponAnimation;
	eWeaponType WeaponType;
	eArmorType ArmorType;
	eAccessaryType AccessaryType;
	int ImprovementStr;			
	int ImprovementDex;			
	int ImprovementVit;			
	int ImprovementInt;
	int ImprovementWis;			
	int ImprovementLife;			
	int ImprovementMana;		
	int ImprovementSkill;

	DWORD Skill;
	ITEM_KIND SupplyType;
	DWORD SupplyValue;
	// 상황별 예외
	BOOL Battle;
	BOOL Peace;
	BOOL Move;
	BOOL Stop;
	BOOL Rest;
	ITEM_SEAL_TYPE wSeal;
	ITEM_TIMEKIND nTimeKind;
	DWORD dwUseTime;
	DWORD RequiredBuff;

	enum eKind
	{
		eKindNormal,
		eKindHighClass,
		eKindRare,
		eKindUnique,
		eKindLegend,
	}
	kind;
	DWORD dwBuyFishPoint;
	eItemPointType wPointType;
	DWORD dwPointTypeValue1;
	DWORD dwPointTypeValue2;
	DWORD dwType; // Item 종류
	DWORD dwTypeDetail; // Item 세부종류
};

struct stEnchantFailSetting
{
	ITEM_INFO::eKind dwItemKind;
	LEVELTYPE dwManusMinLv;	// 인챈트레벨 마이너스 적용 최소 Lv
	LEVELTYPE dwManusMaxLv;	// 인챈트레벨 마이너스 적용 최대 Lv
	LEVELTYPE dwZeroMinLv; // 인챈트레벨 0 적용 최소 Lv
	LEVELTYPE dwZeroMaxLv; // 인챈트레벨 0 적용 최대 Lv
	LEVELTYPE dwDeleteItemMinLv; // 인챈트 시도 아이템 제거 최소 Lv
	LEVELTYPE dwDeleteItemMaxLv; // 인챈트 시도 아이템 제거 최대 Lv
};

#ifdef _CLIENT_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//

typedef struct _PRELOAD
{
	StaticString FileName;
}PRELOAD;

#ifdef _TESTCLIENT_

//trustpak
struct TESTCHARACTER_INFO
{
	BYTE byGender;
	float fX;
	float fY;
	float fZ;
	float fAngle;
};

#define MAX_TEST_CHARACTER_NO 8
//


struct TESTCLIENTINFO {
	TESTCLIENTINFO()
	{
		//trustpak
		memset(this, 0, sizeof(TESTCLIENTINFO));
		//

		LightEffect = 1;
		Effect = 0;
		SkillIdx = 0;
		WeaponIdx = 0;
		Gender = GENDER_MALE;
		x = 30000.f;
		z = 30000.f;
		MonsterEffect = 1;
		MonsterNum = 1;
		MonsterKind = 13;
		Map = 17;

		DressIdx = 0;
		HatIdx = 0;
		ShoesIdx = 0;
		HairType = 0;
		FaceType = 0;
		

		CharacterNum = 0;
		BossMonsterNum = 0;
		BossMonsterKind = 1000;
		
	}
	float x,z;

	WORD DressIdx;
	WORD HatIdx;
	WORD ShoesIdx;
	WORD ShieldIdx;

	BYTE HairType;
	BYTE FaceType;

	WORD MonsterKind;

	WORD BossMonsterKind;
	WORD BossMonsterNum;
	char BossInfofile[17];

	int Effect;
	WORD SkillIdx;
	WORD WeaponIdx;
	BYTE Race;
	BYTE Gender;
	BYTE Job;
	int MonsterEffect;
	WORD MonsterNum;
	WORD CharacterNum;
	MAPTYPE Map;

	BOOL LightEffect;
	
	float Width;
	float Height;

	//trustpak
	DWORD				dwTestCharacterNo;
	TESTCHARACTER_INFO	aTestCharacterInfo[MAX_TEST_CHARACTER_NO];
	
	//

};
#endif

//-----------------------------------------------------------------------
// game config 정보 
//-----------------------------------------------------------------------
struct GAMEDESC_INI
{
	GAMEDESC_INI()
	{
		strcpy(DistributeServerIP,"211.233.35.36");
		DistributeServerPort = 400;

		AgentServerPort = 100;
		
		bShadow = MHSHADOW_CIRCLE;
		
		dispInfo.dwWidth = 1024;		//WINDOW SIZE
		dispInfo.dwHeight = 768;
		dispInfo.dwBPS = 4;
		dispInfo.dispType = WINDOW_WITH_BLT;
		//dispInfo.bWindowed = TRUE;
		dispInfo.dwRefreshRate = 70;

		MaxShadowTexDetail = 256;
		MaxShadowNum = 4;

		FramePerSec = 30;
		TickPerFrame = 1000/(float)FramePerSec;

		MasterVolume = 1;
		SoundVolume = 1;
		BGMVolume = 1;
		
		CameraMinAngleX = 0;
		CameraMaxAngleX = 89.f;
		CameraMinDistance = 200.f;
		CameraMaxDistance = 1000.f;

		LimitDay = 0;
		LimitID[0] = 0;
		LimitPWD[0] = 0;

		strWindowTitle[0] = 0;
	}

	float MasterVolume;
	float SoundVolume;
	float BGMVolume;

	char DistributeServerIP[32];
	WORD DistributeServerPort;
	
	WORD AgentServerPort;
	
	DISPLAY_INFO dispInfo;

	BYTE bShadow;
	DWORD MaxShadowNum;
	DWORD MaxShadowTexDetail;

	DWORD FramePerSec;
	float TickPerFrame;

	char MovePoint[64];

	float CameraMinAngleX;
	float CameraMaxAngleX;
	float CameraMinDistance;
	float CameraMaxDistance;

	DWORD LimitDay;
	char LimitID[MAX_NAME_LENGTH+1];
	char LimitPWD[MAX_NAME_LENGTH+1];

	char strWindowTitle[128];
};

struct SEVERLIST
{
	char	DistributeIP[16];
	WORD	DistributePort;
	char	ServerName[64];
	WORD	ServerNo;
	BOOL	bEnter;

	SEVERLIST()
	{
		ServerNo = 1;
		strcpy( DistributeIP, "211.233.35.36" );
		DistributePort = 400;
		strcpy( ServerName, "Test" );
		bEnter = TRUE;
	}
};

struct PET_FRIENDLY_STATE
{
	BYTE	Friendly;
	DWORD	SpeechIndex;
	BYTE	SpeechRate;
	DWORD	EmoticonIndex;
	BYTE	EmoticonRate;
};
//-----------------------------------------------------------------------------------------------------------//
#endif //_CLIENT_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------------------------//
//		서버만 사용하는 부분
#ifdef _SERVER_RESOURCE_FIELD_

//-----------------------------------------------------------------------------------------------------------//
#include "ServerTable.h"
//-----------------------------------------------------------------------
// 캐릭터 IN/OUT POINT 설정
//-----------------------------------------------------------------------
typedef struct _CHARACTERINOUTPOINT
{
	WORD MapNum;
	VECTOR3 MapInPoint[MAX_MAP_NUM];
	VECTOR3 MapOutPoint[MAX_MAP_NUM];
}CHARACTERINOUTPOINT;


//---KES 스킬초기화돈	071207
struct SKILL_MONEY
{
	DWORD SkillIdx;
	DWORD SP;
	DWORD Money;
};

//----------------------------


//-----------------------------------------------------------------------------------------------------------//
#endif //_SERVER_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//
#endif //__GAMERESOURCESTRUCT_H__

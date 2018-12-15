#ifndef __COMMONGAMEDEFINE_H__
#define __COMMONGAMEDEFINE_H__


enum RaceType
{
	RaceType_Human,
	RaceType_Elf,
	// 090504 ONS 신규종족 추가작업
	RaceType_Devil,

	RaceType_Max,

	RaceType_GM,
};

//for GMPower
enum eGM_POWER
{
	eGM_POWER_MASTER = 0,
	eGM_POWER_MONITOR,
	eGM_POWER_PATROLLER,
	eGM_POWER_AUDITOR,
	eGM_POWER_EVENTER,
	eGM_POWER_QA,
	eGM_POWER_MAX,
};

#define EXIT_COUNT	5
enum eEXITCODE
{
	eEXITCODE_OK,
	eEXITCODE_PKMODE,
	eEXITCODE_LOOTING,
	eEXITCODE_SPEEDHACK,
	eEXITCODE_DIEONEXIT,
	eEXITCODE_NOT_ALLOW_STATE,
};
//

#define SPEEDHACK_CHECKTIME	60000	//1¡§¡þ¡§¡E

//for PK
#define PKLOOTING_ITEM_NUM			20
#define PKLOOTING_DLG_DELAY_TIME	2000
#define PKLOOTING_LIMIT_TIME		10000

enum ePKCODE
{
	ePKCODE_OK,
	ePKCODE_ALREADYPKMODEON,
	ePKCODE_SHOWDOWN,
	ePKCODE_STATECONFLICT,
	ePKCODE_NOTALLAW,
};

enum eLOOTINGERROR
{
	eLOOTINGERROR_OK,
	eLOOTINGERROR_NOMORE_CHANCE,
	eLOOTINGERROR_NOMORE_ITEMLOOTCOUNT,
	eLOOTINGERROR_INVALID_POSITION,
	eLOOTINGERROR_ALREADY_SELECTED,
	eLOOTINGERROR_OVER_DISTANCE,
	eLOOTINGERROR_NO_LOOTINGROOM,
};

#define NPC_TALKING_DISTANCE 700
#define PK_LOOTING_DISTANCE	1000
#define eDROPITEMKIND_MAX 16

enum eUSERLEVEL
{
	eUSERLEVEL_GOD = 1,
	eUSERLEVEL_PROGRAMMER,
	eUSERLEVEL_DEVELOPER,
	eUSERLEVEL_GM,
	eUSERLEVEL_SUPERUSER,
	eUSERLEVEL_USER,
	eUSERLEVEL_SUBUSER,	
};

// 091125 ONS 아이템취급 최대갯수 100 -> 1000으로 수정
#define MAX_ITEMBUY_NUM		1000
#define MAX_INVENTORY_MONEY	4000000000

enum eSTATUSICON
{
	eStatusIcon_None,
	eStatusIcon_QuestTime,
	eStatusIcon_Max,
};

enum eBOSSMONSTERKIND
{
	eBOSSKIND_TARINTUS = 261,
	eBOSSKIND_KIERRA = 267,
	eBOSSKIND_ARACH = 280,
	eBOSSKIND_LEOSTEIN = 721,
	eBOSSKIND_DRAGONIAN = 762,
};

#define RECALL_GROUP_ID			-1
#define EVENTRECALL_GROUP_ID	-2
#define QUESTRECALL_GROUP_ID	-3
// desc_hseos_데이트 존_01
// S 데이트 존 추가 added by hseos 2007.11.29
#define CHALLENGERECALL_GROUP_ID	-4
// E 데이트 존 추가 added by hseos 2007.11.29

#define ITEMRECALL_GROUP_ID	1001	// 몬스터소환 아이템 사용
#define DUNGEON_MONSTER_GROUP_ID	2001	// 인스턴스던전 보스

// 100603 ONS 탈것 그룹아이디 추가
#define VEHICLE_RECALL_GROUP_ID	3001

#define eOPT_HIDEPARTS_NONE				0x00000000
#define eOPT_HIDEPARTS_EQUIP_BAND		0x00000001
#define eOPT_HIDEPARTS_EQUIP_GLASSES	0x00000002
#define eOPT_HIDEPARTS_EQUIP_WING		0x00000004
#define eOPT_HIDEPARTS_COSTUME_HEAD		0x00000008
#define eOPT_HIDEPARTS_COSTUME_DRESS	0x00000010
#define eOPT_HIDEPARTS_COSTUME_GLOVE	0x00000020
#define eOPT_HIDEPARTS_COSTUME_SHOES	0x00000040
#define eOPT_HIDEPARTS_ALL				( eOPT_HIDEPARTS_EQUIP_BAND			|\
											eOPT_HIDEPARTS_EQUIP_GLASSES	|\
											eOPT_HIDEPARTS_EQUIP_WING		|\
											eOPT_HIDEPARTS_COSTUME_HEAD		|\
											eOPT_HIDEPARTS_COSTUME_DRESS	|\
											eOPT_HIDEPARTS_COSTUME_GLOVE	|\
											eOPT_HIDEPARTS_COSTUME_SHOES )
#define MAXPLAYER_IN_CHANNEL	300

enum eBATTLE_KIND
{
	eBATTLE_KIND_NONE = 0,		// Player vs Monster ¡§￠®o¡§uOAC ￠®ic¨IⓒªECO¡EO￠®¡¿AC Battle
	eBATTLE_KIND_SHOWDOWN,
	eBATTLE_KIND_GTOURNAMENT,
	eBATTLE_KIND_MAX,
};

enum GRID_TYPE
{
	eGridType_General,
	eGridType_Murim
};

enum Friend_ERROR_CODE
{
	//add friend
	eFriend_AddSuccess=0,
	eFriend_AddInvalidUser,
	eFriend_AddFullList,
	eFriend_AddDeny,
	eFriend_NotLogin,
	eFriend_AlreadyRegist,
//	eFriend_RegistSelf,
	//delete friend
	eFriend_DelInvaliedUser,
//----
	eFriend_OptionNoFriend,
};

enum {
LOGIN_SUCCESS = 0,
LOGIN_ERROR_INVALID_VERSION,
LOGIN_ERROR_OVERLAPPEDLOGIN,
LOGIN_ERROR_NOAGENTSERVER,
LOGIN_ERROR_NODISTRIBUTESERVER,
LOGIN_ERROR_INVALIDUSERLEVEL,
LOGIN_ERROR_WRONGIDPW,
LOGIN_ERROR_INVALIDDISTAUTHKEY,
LOGIN_ERROR_DISTSERVERISBUSY,
LOGIN_ERROR_AGENTSERVERISBUSY,
LOGIN_ERROR_AGENTMAXCONNECTION,
LOGIN_ERROR_BLOCKUSERLEVEL,
LOGIN_ERROR_INVALID_IP,	//ipcheck
LOGIN_ERROR_DISTCONNET_ERROR,
LOGIN_ERROR_MAXUSER,
LOGIN_ERROR_OVERLAPPEDLOGININOTHERSERVER,
LOGIN_ERROR_MINOR_INADULTSERVER,
LOGIN_ERROR_NOT_CLOSEBETAWINNER,
LOGIN_ERROR_SECEDEDACCOUNT,
LOGIN_ERROR_NOREMAINTIME,
LOGIN_ERROR_NOIPREGEN,
LOGIN_ERROR_NONACCADMINIP,
// 080111 LYW --- CommonGameDefine : 로그인 보안 비밀번호 관련 에러 처리 추가.
LOGIN_ERROR_INVALID_SEC_PASS,				// 보안 비밀번호 인증에 실패했다는 에러메시지.
LOGIN_ERROR_CHANGE_SEC_PASS,				// 잘못된 보안 비빌번호, 홈페이지에서 보안 비밀번호를 재 등록 하시오.
LOGIN_ERROR_FILE_CRC,
LOGIN_ERROR_LOGINSESSION_INVALID,
};

#define MHSHADOW_OFF	0
#define MHSHADOW_CIRCLE	1
#define MHSHADOW_DETAIL	2
// E¡E¡þ¡§¡þ¨Io ¡§oA￠®¨¡￠®I
#define RECOVERTIMEUNIT					3000
// E¡E¡þ¡§¡þ¨Io ￠®¨¡￠®I￠®¨¡Y
#define RECOVERINTERVALUNIT				10

// A¡E¡þAI ￠®icAIAi(CN A¡E¡þAIAC A¡§I￠®¨ua)
#define fTILE_SIZE 50.f

#define SKILLAREA_ATTR_DAMAGE			0x00000001
#define SKILLAREA_ATTR_BLOCK			0x00000002
#define SKILLAREA_ATTR_SAFETYZONE		0x00000003
#define SKILLAREA_ATTR_OTHEREFFECT		0x00000004

typedef BYTE AREATILE;
#define GetAreaTile	GetByte

#define MAX_ITEM_NUM 3000	//confirm
#define MAX_ITEM_OPTION_NUM	90

// INDEXGENERATER ID START INDEX
#define MAX_ITEMICON_NUM	999

// 080203 LYW --- 행동 관련 아이콘 최대 개수 정의.
#define MAX_CONDUCTICON_NUM	999

#define IG_ITEM_STARTINDEX				10000
#define IG_MSGBOX_STRARTINDEX			12000
#define IG_STREETSTALLITEM_STARTINDEX	13000	//LBS 03.09.19
#define IG_EXCHANGEITEM_STARTINDEX		14000	//KES EXCHANGE 030920
#define IG_MENUSLOTITEM_STARTINDEX		15000	//KES 040510	//Ao￠®¨uY ¡§uE¡§u¡E¡I¡E¡IU. Ao¡E?i￠®¨ui.
#define IG_PKLOOTINGDLG_START			16000	//KES PKLOOTING 040720
#define IG_DEBUG_START					20000	//KES DEBUG 040720
#define IG_QUESTITEM_STARTINDEX			25000	//RaMa
#define IG_SHOPITEM_STARTINDEX			28000	//RaMa 04.11.17
#define IG_DEALITEM_START				30000
#define IG_MAINBARICON_START			40000
//#define IG_ABILITYICON_START			31000
//#define IG_ABILITYPOINT_START			40000	//RaMa 특기치 재분배
#define IG_QUICKITEM_STARTINDEX			60000
// 070216 LYW --- CommonGameDefine : Define start index of conduct part.
#define IG_CONDUCT_STARTINDEX			70000

// 070608 LYW --- CommonGameDefine : Define start index of matching favor icon part.
#define IG_FAVOR_STARTINDEX				150000
#define IG_IDENTIFICATION_STARTINDEX	151000
#define IG_MATCH_MYINFO_STARTINDEX		152000
#define IG_MATCH_PARTNERINFO_STARTINDEX	153000
#define IG_MATCH_ENDINDEX				154000

//090423 pdy 하우징 아이콘 인덱스 
#define IG_HS_STOREDICON_START 155000
#define HOUSING_FURNITURE_STARTINDEX	(UINT_MAX - 2000000)
#define MONSTER_INDEX_SIZE				200000
#define MONSTER_STARTINDEX				HOUSING_FURNITURE_STARTINDEX - MONSTER_INDEX_SIZE
#define VEHICLE_INSTALL_ID				MONSTER_STARTINDEX - 1

enum NPC_ROLE
{
	OBJECT_ROLE = 0,
	DEALER_ROLE = 1,
	MUNPA_ROLE = 3,
	CHANGGO_ROLE = 4,
	FYOKUK_ROLE = 5,
	TALKER_ROLE = 6,
	MAP_MOVE_ROLE = 8,
	FAMILY_ROLE = 15,
 	// 080901 LYW --- CommonGameDefine :
	CANDLESTAND_ROLE = 20,
	// 080821 LYW --- CommonGameDefine : 맵 이동을 하는데, 확인 메시지 박스를 띄우는 타입이다.(공성전에서 사용)
	PORTAL_ROLE = 21,
	// 080901 LYW --- CommonGameDefine : 
	SIEGEWELL_ROLE = 22,
	BOMUL_ROLE	= 23,
	// 080901 LYW --- CommonGameDeinfe : 버프 NPC 직업 추가.
	BUFFNPC_ROLE = 24,
	// 080903 LYW --- CommonGameDefine : 공성 깃발 추가.
	SIEGEFLAGROLE_RUSHEN = 25,
	SIEGEFLAGROLE_ZEVYN = 26,
	MAPCHANGE_ROLE = 27,	
	FISHINGPLACE_ROLE = 28,
	CAMPFIRE_ROLE = 29,
	// 070605 LYW --- CommonGameDefine : Add identification role.
	IDENTIFICATION_ROLE = 38,

	// 081125 LYW --- CommonGameDefine : 리미트 던전 입장 전용 직업 추가.
	LIMITDUNGEON_ROLE	= 40,

	//090522 pdy  --- CommonGameDefine : 하우징 NPC 직업 추가.
	HOUSING_ROLE		= 50,
	HOUSING_RANK1_ROLE	= 51,
	HOUSING_RANK2_ROLE	= 52,
	HOUSING_RANK3_ROLE	= 53,
	HOUSING_PORTAL_ROLE	= 54,
	DUNGEON_WARP_ROLE	= 55,
	DUNGEON_SWITCH_ROLE = 56,
	
	NOTE_ROLE			= 60,	// 우편함
	CONSIGNMENT_ROLE	= 61,	// 판매대행
};


enum eMoveMode
{
	eMoveMode_Run = 0,
	eMoveMode_Walk = 1,
};

#define MAX_PACKEDDATA_SIZE 60000 // for SEND_PACKED_DATA

enum eMONSTER_ACTION
{
	eMA_STAND,
	eMA_WALKAROUND,
	eMA_PERSUIT,
	eMA_RUNAWAY,
	eMA_WALKAWAY,
	eMA_ATTACK,
	eMA_PAUSE,
	eMA_SCRIPT_RUN,
	eMA_SCRIPT_WALK,
};

enum LIMIT_RACE
{
	eLimitRace_Common = 0,
	eLimitRace_HumanElf,
	eLimitRace_Devil,
};

enum ITEM_KIND
{
	ITEM_KIND_HP_RECOVER		= 1,	// 생명력 회복제
	ITEM_KIND_MP_RECOVER		= 2,	// 기력 회복제
//---KES 
	ITEM_KIND_HP_RECOVER_INSTANT	= 3,	// 생명력 회복제
	ITEM_KIND_MP_RECOVER_INSTANT	= 4,	// 기력 회복제
	ITEM_KIND_HPMP_RECOVER			= 5,	// 생명기력 동시 회복제
	ITEM_KIND_HPMP_RECOVER_INSTANT	= 6,	// 생명기력 동시 회복제
//-----
	ITEM_KIND_FARM_PLANT		= 7,	// 농장의 농작물 심기
	ITEM_KIND_FARM_MANURE		= 8,	// 농장의 농작물 비료 주기
	ITEM_KIND_BUFF_ITEMS		= 11,	// 버프의 기능들을 사용한 유료 아이템들.
	ITEM_KIND_FARM_HARVEST		= 12,	// 농장의 농작물 수확
	ITEM_KIND_COOLTIME			= 13,	// 080820 LUJ, 획득 시에 쿨타임을 갖고 있는 아이템

	ITEM_KIND_CHANGESIZE_UPWEIGHT	= 20,	// 캐릭터 키변경 - 커질확률 가중치
	ITEM_KIND_CHANGESIZE_DNWEIGHT	= 21,	// 캐릭터 키변경 - 작아질확률 가중치
	ITEM_KIND_CHANGESIZE_DEFALUT	= 22,	// 캐릭터 키변경 - 원래대로

	ITEM_KIND_ADDRECIPE			= 23,	// 요리 - 레시피배우기
	//ITEM_KIND_DELRECIPE			= 24,	// 요리 - 레시피지우기
	ITEM_KIND_FOODSKILLBOOK		= 25,	// 요리 - 스킬북
	ITEM_KIND_FOOD				= 26,	// 요리 - 요리

	//090527 pdy 하우징 
	ITEM_KIND_FURNITURE						= 30,
	ITEM_KIND_FURNITURE_WALLPAPER			= 31,
	ITEM_KIND_FURNITURE_FLOORPAPER			= 32,
	ITEM_KIND_FURNITURE_CEILINGPAPER		= 33,
	ITEM_KIND_VISIT_HOUSE					= 34,	// 하우스방문
	ITEM_KIND_VISIT_HOME					= 35,	// my하우스입장
	ITEM_KIND_EXTEND_HOUSE					= 36,	// 하우스 확장

	ITEM_KIND_CHANGE_CLASS		= 49,	// 100511 ONS 전직초기화 아이템	
	ITEM_KIND_RESET_STATS		= 50,	// 스탯 초기화 아이템
	ITEM_KIND_RESET_SKILL		= 51,	// 스킬 초기화 아이템

	// 080611 LYW --- CommonGameDefine : 아이템 타입 추가.
	ITEM_KIND_GET_SKILLPOINT	= 52,	// 스킬 포인트 획득.

	ITEM_KIND_PROTECT_EXP		= 53,	// 경험치 보호 아이템
	ITEM_KIND_RECALL_MOVE		= 54,	// 기억이동 주문서
	ITEM_KIND_EXTEND_INVEN		= 55,	// 인벤토리 확장 아이템
	ITEM_KIND_EXTEND_STORAGE	= 56,	// 창고 확장 아이템
	ITEM_KIND_MEMORY_STONE		= 57,	// 기억석

	// 080607 LYW --- CommonGameDefine : 아이템 타입 추가.
	ITEM_KIND_OPENSTORAGE_BYITEM = 58,	// 창고 소환 아이템.

	ITEM_KIND_CHANGE_NAME		= 66,	// 이름 변경 아이템
	ITEM_KIND_MAP_MOVE_SCROLL	= 67,	// 071201 맵이동 주문서
	ITEM_KIND_GTBUFF_ITEMS		= 76,		// 길드토너먼트 전용 버프아이템 (타겟대상에게만 쓸 수 있다)
	ITEM_KIND_JOIN_EVENT		= 77,	// 080204 KTH -- 이벤트 응모 관련
	ITEM_KIND_GTHP_RECOVER_INSTANT	= 78,	// 길드토너먼트 전용 HP포션
	ITEM_KIND_GTMP_RECOVER_INSTANT	= 79,	// 길드토너먼트 전용 MP포션
	ITEM_KIND_WORLDSHOUT		= 80,	// 080312 NYJ -- 전체외치기(월드확성기) 아이템
	ITEM_KIND_FISHING_BAIT		= 81,	// 080410 NYJ -- 낚시 미끼아이템
	ITEM_KIND_FISHING_FISH		= 82,	// 080410 NYJ -- 낚시 물고기
	ITEM_KIND_RESET_REGIDENT_REGIST	= 88,// 080215 KTH -- 주민등록 초기화 아이템
	ITEM_KIND_RETURN_SCROLL		= 99,	// 귀환 주문서

 	// 080826 LYW --- CommonGameDefine : 워터시드 아이템 타입 추가.
	ITEM_KIND_SIEGEWARFARE_WATERSEED = 100,

	ITEM_KIND_CHALLENGEZONE_ENTER_FREQ	= 200,			// 챌린지 존 입장 횟수 증가
	ITEM_KIND_ANIMAL			= 500,	// 080415 KTH -- 가축
	ITEM_KIND_ANIMAL_FOOD		= 501,	// 080507 KTH -- 먹이
	ITEM_KIND_ANIMAL_CLEANER	= 502,	// 080514 KTH -- 축사 청소 세재
	ITEM_KIND_PET							= 600,
	ITEM_KIND_PET_SKILL						= 601,
	ITEM_KIND_PET_COSTUME					= 602,
	ITEM_KIND_PET_EQUIP						= 603,

	ITEM_KIND_PET_EGG						= 700,
	ITEM_KIND_PET_FOOD						= 701,
	ITEM_KIND_PET_NORMAL_RESURRECTION		= 702,
	ITEM_KIND_PET_SPECIAL_RESURRECTION		= 703,
	ITEM_KIND_PET_HP_RECOVER				= 704,
	ITEM_KIND_PET_MP_RECOVER				= 705,
	ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE		= 706,
	ITEM_KIND_PET_EVOLUTION_MAGIC_TYPE		= 707,
	ITEM_KIND_PET_EVOLUTION_SUPPORT_TYPE	= 708,
	ITEM_KIND_SUMMON_NPC					= 800,
	ITEM_KIND_SUMMON_MONSTER				= 801,
	ITEM_KIND_SUMMON_EFFECT					= 802,
	ITEM_KIND_SUMMON_VEHICLE				= 803,
	ITEM_KIND_DUNGEON_KEY					= 900,
	ITEM_KIND_LEVEL_UP						= 910,
	ITEM_KIND_GUILDPOINT_UP					= 911,
	ITEM_KIND_FAMILYPOINT_UP				= 912,
	ITEM_KIND_EXP_UP						= 913,
	ITEM_KIND_PCROOMPOINT_UP				= 914,
	ITEM_KIND_FISHINGEXP_UP					= 915,
	ITEM_KIND_FISHPOINT_UP					= 916,
	ITEM_KIND_COOKLEVEL_UP					= 917,
	ITEM_KIND_DECOUSEPOINT_UP				= 918,
	ITEM_KIND_PKPOINT_UP					= 919,
	ITEM_KIND_PKPOINT_DN					= 920,
};
// E 농장시스템 추가 added by hseos 2007.05.14

/// 아이템 구분 재정의
enum eItemCategory
{
	eItemCategory_Etc,
	eItemCategory_Equip,
	eItemCategory_Expend,
	eItemCategory_Material,
	eItemCategory_Quest,
	eItemCategory_Max,
};

enum eEquipType
{
	eEquipType_None,
	eEquipType_Weapon,
	eEquipType_Armor,
	eEquipType_Accessary,
	eEquipType_Pet,
	eEquipType_Max,
};

enum ePetEquipSlot
{
	ePetEquipSlot_Helmet,
	ePetEquipSlot_Armor,
	ePetEquipSlot_Glove,
	ePetEquipSlot_Accessaries,
	ePetEquipSlot_Weapon,
	
	ePetEquipSlot_Face,
	ePetEquipSlot_Head,
	ePetEquipSlot_Back,
	ePetEquipSlot_Tail,
	
	ePetEquipSlot_Skill,

	ePetEquipSlot_Skill1 = ePetEquipSlot_Skill,
	ePetEquipSlot_Skill2,
	ePetEquipSlot_Skill3,
	ePetEquipSlot_Skill4,
	ePetEquipSlot_Skill5,
	ePetEquipSlot_Skill6,


	ePetEquipSlot_Max,
};

enum eWeaponAnimationType
{
	eWeaponAnimationType_None,
	eWeaponAnimationType_OneHand,
	eWeaponAnimationType_TwoHand,
	eWeaponAnimationType_Dagger,
	eWeaponAnimationType_Staff,
	eWeaponAnimationType_Bow,	
	eWeaponAnimationType_Gun,
	eWeaponAnimationType_TwoBlade,
	// 090528 ONS 마족 양손무기 속성 추가
	eWeaponAnimationType_BothDagger,
	// 090204 LUJ, 속성 추가
	eWeaponAnimationType_Spear,
	eWeaponAnimationType_Max,
};

enum eWeaponType
{
	eWeaponType_None,
	eWeaponType_Sword,
	eWeaponType_Mace,
	eWeaponType_Axe,
	eWeaponType_Dagger,
	eWeaponType_Staff,
	eWeaponType_Bow,
	eWeaponType_Gun,
	eWeaponType_FishingPole,
	eWeaponType_CookUtil,
	// 090528 ONS 신규종족 무기 타입 속성 추가
	eWeaponType_BothDagger,
	eWeaponType_Max,
};

enum eArmorType
{
	eArmorType_None,
	eArmorType_Robe,
	eArmorType_Leather,
	eArmorType_Metal,
	// 090819 ShinJS --- 조합 인터페이스 수정, 방패/헬멧 구분을 위해 Armor Type 추가
	eArmorType_Helmet,
	eArmorType_Shield,
	eArmorType_Max,
};

enum eAccessaryType
{
	eAccessaryType_None,
	eAccessaryType_Ring,
	// 090820 ShinJS --- 조합 인터페이스 수정, 목걸이와 귀걸이가 아이템리스트의 값과 서로 바뀌어 있어 수정
	eAccessaryType_Necklace,
	eAccessaryType_Earring,
	eAccessaryType_Belt,
	eAccessaryType_Band,
	eAccessaryType_Max,
};

// 100104 ShinJS --- 아이템 Point Type
enum eItemPointType
{
	eItemPointType_None,
	eItemPointType_Item,
	eItemPointType_PCRoomPoint,
	eItemPointType_GuildPoint,
	eItemPointType_FamilyPoint,
	eItemPointType_Exp,
	eItemPointType_PKPoint,
	eItemPointType_FishPoint,
};

#define PET_EQUIP_SLOT_START 70
#define MAX_JOB	3
#define MAX_NPC_NUM 200
#define	MAX_MOVENPC_NUM	10						// 090227 ShinJS --- 이동NPC 최대 개수 
#define MAX_DROPITEM_NUM 20
#define MAX_CHARACTER_LEVEL_NUM		150			// A¨Iⓒª¡E¡þ?AI ￠®￠´¨Iiⓒo¡Ei¨u￠®¡¿ ¡§uo

#define MAX_PLAYERLEVEL_NUM	151
#define MAX_MONSTERLEVELPOINTRESTRICT_NUM	9
#define MONSTERLEVELRESTRICT_LOWSTARTNUM	6

#define MAX_COMBO_NUM		3
#define SKILL_COMBO_NUM		100
#define SKILLRESULTKIND_NONE 0

enum ReviveFlag
{
	ReviveFlagNone,
	ReviveFlagHere,
	ReviveFlagTown,
};

enum eItemUse_Err
{
	eItemUseSuccess,
	eItemUseSuccess_ResetRegidentCard,
	eItemUseSuccess_ChangeItem,
	// E 체인지아이템 추가 added by hseos 2007.08.29

	eItemUseErr_PreInsert,
	eItemUseErr_Move,
	eItemUseErr_Combine,
	eItemUseErr_Divide,
	eItemUseErr_Discard,
	eItemUseErr_Quest,
	eItemUseErr_Err,
	eItemUseErr_Upgrade,
	eItemUseErr_Lock,
	eItemUseErr_AlreadyUse,
	eItemUseErr_Unabletime,
	eItemUseErr_AlreadyPoint,
	eItemUseErr_AlredyChange,
	eItemuseErr_DontUseToday,
	eItemUseErr_DontDupUse,
	eItemUseErr_UseFull,
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.08.29
	eItemUseErr_FullInven,
	eItemUseErr_WithNoneMsg,
	// E 농장시스템 추가 added by hseos 2007.08.29

	eItemUseUnsealed,	//080317 NYJ 봉인해제
	eItemUseErr_NoSummonInfo,

 	// 080607 LYW --- CommonGameDefine : 창고 소환 아이템 사용 실패 에러 추가.
	eItemUse_OpenStorage_byItem_Failed_Storage_IsZero,
	// 080609 LYW --- CommonGameDefine : 창고 소환 아이템 사용 성공 타입 추가.
	eItemUse_OpenStorage_Success,
	// 080902 LYW --- CommonGameDefine : 
	eItemUseErr_WaterseedErr,

	// 080904 LYW --- CommonGameDefine : 공성 중 사용할 수 없는 아이템 메시지 출력.
	eItemUseErr_SiegeWarfareForbiddenItem,

	eItemUseErr_ForbiddenSummon,	// 081027 NYJ 소환금지

	eItemUseErr_InvalidCookGrade,
	eItemUseErr_LowerLevelForEat,
	eItemUseErr_AlreadyLearnRecipe,
	eItemUseErr_FullLearnRecipe,

	eItemUseErr_HaveNoHouse,

	// 100218 ONS 주민등록 초기화 아이템 사용 실패 에러 추가
	eItemUseErr_NotRegist,
	eItemUseErr_PlayerIsGod,
	eItemUseErr_SkillCanNotStart,
	eltemUseErr_Stun,
};

/// GameResource¡E?¡E￠c ￠®ic¡E?e¡IiC¡E¡IA define ///////////////////////////////////////////////
// 091125 ONS 아이템취급 최대갯수 100 -> 1000으로 수정
#define MAX_YOUNGYAKITEM_DUPNUM			1000
// 061211 LYW --- Adjust max character count.
//#define MAX_CHARACTER_NUM	4
#define MAX_CHARACTER_NUM	5
#define MAX_STREETSTALL_TITLELEN 66

#define MAX_FILE_NAME		32
enum { STR_POINT, WIS_POINT, DEX_POINT, VIT_POINT, INT_POINT, MAX_BTN_POINT};

#define GENDER_MALE 0
#define GENDER_FEMALE 1
#define GENDER_MAX 2

#define MAX_PARTY_LISTNUM 7
#define MAX_PARTY_LOAD_NUM 100

// 070105 LYW --- Define max count for family member.
#define MAX_FAMILY_LISTNUM 5
#define MAX_BASESTORAGELIST_NUM 2
#define MAX_STORAGELIST_NUM 5
#define MAX_FRIEND_NUM 30
#define MAX_FRIEND_LIST 10
#define MAX_FRIEND_PAGE MAX_FRIEND_NUM/MAX_FRIEND_LIST // 30/10 = 10

// 100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
#define MAX_PCROOM_MEMBER_NUM 30
#define MAX_PCROOM_MEMBER_LIST 10

#define MAX_NOTENUM 88 //40
#define MAX_NOTE_PAGE 8 //4
#define NOTENUM_PER_PAGE MAX_NOTENUM/MAX_NOTE_PAGE // 30/3 = 10
#define MAX_NOTE_LENGTH 600//255

#define MAX_CHANNEL_NUM 10
#define MAX_CHANNEL_NAME 16

enum EWEARED_ITEM
{
	eWearedItem_Weapon,
	eWearedItem_Shield,
	eWearedItem_Dress,
	eWearedItem_Hat,
	eWearedItem_Glove,
	eWearedItem_Shoes,

	eWearedItem_Ring1,
	eWearedItem_Ring2,
	eWearedItem_Necklace,
	eWearedItem_Earring1,
	eWearedItem_Earring2,
	eWearedItem_Belt,
	eWearedItem_Band,

	eWearedItem_Glasses,
	eWearedItem_Mask,
	eWearedItem_Wing,

	eWearedItem_Costume_Head,
	eWearedItem_Costume_Dress,
	eWearedItem_Costume_Glove,
	eWearedItem_Costume_Shoes,

	eWearedItem_Max,
};

enum ERROR_GUILD
{
	eGuildErr_None	= 0,
	eGuildErr_Err	= 1,
			
	eGuildErr_Create_Invalid_Name,
	eGuildErr_Create_Already,
	eGuildErr_Create_Low_Level,
	eGuildErr_Create_Insufficient_Money,
	eGuildErr_Create_Unable_Entry_Data,
	
	eGuildErr_BreakUp,
	
	eGuildErr_DeleteMember,
	eGuildErr_DeleteMember_NothaveAuthority,
	eGuildErr_DeleteMember_NotMember,
	
	eGuildErr_AddMember_NotPlayer,
	eGuildErr_AddMember_OtherGuild,
	eGuildErr_AddMember_AlreadyMember,
	eGuildErr_AddMember_FullMember,	
	eGuildErr_AddMember_TargetNotDay,
	eGuildErr_AddMember_NothaveAuthority,
	
	eGuildErr_InviteApt_NoMaster,

	eGuildErr_Mark_NoGuild,
	eGuildErr_Mark_NoMark,

	eGuildErr_LevelUp_NotAbil,
	eGuildErr_LevelUp_NotMoney,
	eGuildErr_LevelUp_Complete,

	eGuildErr_ChangeRank_NoGuild,
	eGuildErr_ChangeRank_NotMember,
	eGuildErr_ChangeRank_LowerLevel,
	eGuildErr_ChangeRank_Master,
	eGuildErr_ChangeRank_AlreadyHave,
	eGuildErr_ChangeRank_NoAuthority,
	eGuildErr_ChangeRank_NoGraduatedStudent,
	eGuildErr_ChangeRank_Student,

	eGuildErr_NoGuild,

	eGuildErr_Nick_Filter,
	eGuildErr_Nick_NotMember,
	eGuildErr_Nick_NotAvailableName,
	eGuildErr_Nick_NotLogIn,

	eGuildErr_AddStudent_TooHighLevelAsStudent,
	eGuildErr_AddStudent_Full,

	// 080417 LUJ, 길드 스킬 추가 에러
	eGuildErr_InsufficentMoney	=	100,
	eGuildErr_InsufficentScore,
	eGuildErr_NoMaster,
	eGuildErr_NoScript,
	eGuildErr_Skill_Max,
	eGuildErr_Skill_InvalidLevel,
	eGuildErr_Skill_NotExist,

	// 090317 NYJ - 길드공지에 필터링문자 검출
	eGuildErr_Notice_FilterCharacter,
};
enum
{
	eGuild_Delete,
	eGuild_Secede,		
};

enum ERROR_FAMILY
{
	eFamilyErr_Err = 1,
			
	eFamilyErr_Create_Name,
	eFamilyErr_Create_Intro,
	
	eFamilyErr_BreakUp,
	
	eFamilyErr_DeleteMember,
	eFamilyErr_DeleteMember_NothaveAuthority,
	eFamilyErr_DeleteMember_NotMember,
	
	eFamilyErr_AddMember_NotPlayer,
	eFamilyErr_AddMember_OtherFamily,
	eFamilyErr_AddMember_AlreadyMember,
	eFamilyErr_AddMember_FullMember,	
	eFamilyErr_AddMember_TargetNotDay,
	eFamilyErr_AddMember_NothaveAuthority,

	eFamilyErr_InviteApt_NoMaster,

	eFamilyErr_Mark_NoFamily,
	eFamilyErr_Mark_NoMark,

	eFamilyErr_LevelUp_NotAbil,
	eFamilyErr_LevelUp_NotMoney,
	eFamilyErr_LevelUp_Complete,

	eFamilyErr_ChangeRank_NoFamily,
	eFamilyErr_ChangeRank_NotMember,

	eFamilyErr_NoFamily,

	eFamilyErr_Nick_Filter,
	eFamilyErr_Nick_NotMember,
	eFamilyErr_Nick_NotAvailableName,
	eFamilyErr_Nick_NotLogIn,

	eFamilyErr_AddStudent_TooHighLevelAsStudent,
};
enum
{
	eFamily_Delete,
	eFamily_Secede,		
};

//--------¡E?¡E￠c￠®￠´? ¡E¡þ¡§¡ⓒ¡§oAAo----------------------------------------------------------
// 090227 ShinJS --- Coin부족 에러 추가(NOT_COIN)
enum DEAL_BUY_ERROR { NOT_ERROR, NOT_MONEY = 101, NOT_SPACE, NOT_EXIST, NOT_PLAYER, NOT_OWNERMONEY, NOT_SAMEPRICE, NOT_SAMEINFO, NOT_FISHINGPOINT, OVER_INVENMONEY, NOT_COIN, NOT_PCROOMPOINT, NOT_PCROOM };

// 090107 LYW --- CommonGameDefine : 아이템 상점 판매 에러 분류 추가.
enum DEAL_SELL_ERROR
{
	ERR_NONE = 0,			// 에러아님.
	ERR_NOTSELLITEM,		// 비매품 아이템.
	ERR_NOTINVENITEM,		// 인벤토리의 아이템이 아님.
	ERR_INVALSLOT,			// 비정상 SLOT.
	ERR_INVALITEMINFO,		// 비정상 ITEM.
	ERR_INVALPLAYER,		// 비정상 PLAYER.
	ERR_LOOTPLAYER,			// 루팅중인 PLAYER.
	ERR_INVALNPC,			// 비정상 NPC.
	ERR_INVALIDENTITY,		// 아이템 동일성 체크 실패.
	ERR_OVERSTACKCOUNT,		// 스택 카운트 오버.
	ERR_MAXMONEY,			// 소유가능금액 오버.
	ERR_FAILDISCARD,		// 아이템 삭제처리 실패.
} ;

enum ERROR_MUNPABOARDCONTENTS {
	ERROR_MUNPABOARDCONTENTS_NOCONTENTS = 1,
	ERROR_MUNPABOARDCONTENTS_NOAUTHORITY,
};

enum ERROR_PARTY 
{
	eErr_Create_DB,
	eErr_Add_NoPlayer,
	eErr_Add_AlreadyinParty,
	eErr_Add_OptionNoParty,
	eErr_Add_DiedPlayer,
	eErr_ChangingMaster,
	eErr_Add_Full, //can not join cause party is full!
	eErr_BrokenParty, // Party has already broken.
	eErr_Create_MapHasParty, //map has party but client doesn't have party
	eErr_Add_NotOurGuild,		// During Guild Tournament
	// 070110 LYW --- Add error message for party.
	eErr_Fail_Change_PickupItem,
	eErr_Success_Change_PickupItem,
	eErr_Fail_Change_DiceGrade,
	eErr_Success_Change_DiceGrade,
	// 090528 ShinJS --- 이름검색으로 파티초대시 에러 추가
	eErr_AlreadySearchingByName,	// 검색중인 경우
	eErr_Add_NotConnectedPlayer,	// 초대대상이 접속중이지 않은 경우
	eErr_SettingLimitTime,			// 최대 초대요청 초과시 제한시간 설정
};

enum PARTY_OPTION
{
	ePartyOpt_Damage,
	ePartyOpt_Sequence,
};

#define PARTY_DICEGRADE_DEFAULT 3

// 090528 ShinJS --- 이름으로 파티초대요청에 대한 제한설정
enum PARTY_INVITE_BYNAME_SETTING
{
	ePartyInvite_LimitCnt = 2,									// 최대 초대요청 누적회수 (초과시 eOverCntLimitTime 만큼 기능제한)
	ePartyInvite_CheckSearchTimeNoPlayer = 5000,				// 이름검색시 검사시간 - 플레이어가 존재하지 않는경우
	ePartyInvite_CheckSearchTimeNotConnectPlayer = 5000,		// 이름검색시 검사시간 - 플레이어가 접속하지 않은경우
	ePartyInvite_CheckSearchTimeAlreadyInParty = 5000,			// 이름검색시 검사시간 - 플레이어가 이미 다른파티에 속한 경우
	ePartyInvite_CheckSearchTimeSuccess = 5000,					// 이름검색시 검사시간 - 검색에 성공한 경우
	ePartyInvite_OverCntLimitTimeNoPlayer = 5000,				// 누적 초과시 제한시간 설정 - 플레이어가 존재하지 않는경우
	ePartyInvite_OverCntLimitTimeAlreadyInParty = 5000,			// 누적 초과시 제한시간 설정 - 플레이어가 이미 다른파티에 속한 경우
	ePartyInvite_OverCntLimitTimeNotConnectPlayer = 5000,		// 누적 초과시 제한시간 설정 - 플레이어가 접속하지 않은경우
	ePartyInvite_OverCntLimitTimeSuccess = 5000,				// 누적 초과시 제한시간 설정 - 검색에 성공한 경우
};

//--------¡E?¡E￠c￠®￠´? ¡E¡þ¡§¡ⓒ¡§oAAo----------------------------------------------------------

enum TABNUMBER
{
	TAB_INVENTORY_NUM		= 2,
	TAB_MUNPAWAREHOUSE_NUM	= 7,
	TAB_BASE_STORAGE_NUM	= 2,
	TAB_STORAGE_NUM			= TAB_BASE_STORAGE_NUM + 3,
	TAB_GUILDWAREHOUSE_NUM	= 2,
};

enum CELLPERTAB
{
	TABCELL_INVENTORY_NUM		= 20,
	TABCELL_MUNPAWAREHOUSE_NUM	= 45,
	TABCELL_STORAGE_NUM			= 36,
	TABCELL_GUILDWAREHOUSE_NUM	= 36,
};

enum
{
	SLOT_INVENTORY_NUM			= TAB_INVENTORY_NUM * TABCELL_INVENTORY_NUM,
	SLOT_EXTENDED_INVENTORY1_NUM = TABCELL_INVENTORY_NUM,
	SLOT_EXTENDED_INVENTORY2_NUM = TABCELL_INVENTORY_NUM,
	SLOT_MAX_INVENTORY_NUM		= SLOT_INVENTORY_NUM + SLOT_EXTENDED_INVENTORY1_NUM + SLOT_EXTENDED_INVENTORY2_NUM,	// 80
	SLOT_WEAR_NUM				= 20,
	SLOT_MUNPAWAREHOUSE_NUM		= TAB_MUNPAWAREHOUSE_NUM * TABCELL_MUNPAWAREHOUSE_NUM,	//315
	SLOT_STORAGE_NUM			= TAB_STORAGE_NUM * TABCELL_STORAGE_NUM,				//180
	SLOT_GUILDWAREHOUSE_NUM		= TAB_GUILDWAREHOUSE_NUM * TABCELL_GUILDWAREHOUSE_NUM,	//72
	SLOT_SHOPITEM_NUM			= 36,
	SLOT_STREETSTALL_NUM		= 24,
	SLOT_STREETBUYSTALL_NUM		= 5,	
};

enum
{
	TP_INVENTORY_START				= 0,
	TP_INVENTORY_END				= TP_INVENTORY_START		+ SLOT_INVENTORY_NUM,		// 40
	TP_EXTENDED_INVENTORY1_START	= TP_INVENTORY_END,
	TP_EXTENDED_INVENTORY1_END		= TP_EXTENDED_INVENTORY1_START	+ SLOT_EXTENDED_INVENTORY1_NUM,	// 60
	TP_EXTENDED_INVENTORY2_START	= TP_EXTENDED_INVENTORY1_END,
	TP_EXTENDED_INVENTORY2_END		= TP_EXTENDED_INVENTORY2_START	+ SLOT_EXTENDED_INVENTORY2_NUM,	// 80
	TP_WEAR_START					= TP_EXTENDED_INVENTORY2_END,
	TP_WEAR_END						= TP_WEAR_START				+ SLOT_WEAR_NUM,			// 100
	TP_STORAGE_START				= TP_WEAR_END,											// 100
	TP_STORAGE_END					= TP_STORAGE_START			+ SLOT_STORAGE_NUM,			// 280
	TP_SHOPITEM_START				= TP_STORAGE_END,										// 280
	TP_SHOPITEM_END					= TP_SHOPITEM_START			+ SLOT_SHOPITEM_NUM,		// 316
	TP_MAX							= TP_SHOPITEM_END,										// 316
	TP_GUILDWAREHOUSE_START			= 500,
	TP_GUILDWAREHOUSE_END			= TP_GUILDWAREHOUSE_START + SLOT_GUILDWAREHOUSE_NUM,	// 572
};

enum eITEMTABLE
{
	eItemTable_Inventory,
	eItemTable_Weared,
	eItemTable_Storage,
	eItemTable_Shop,
	eItemTable_TableMax,
	eItemTable_MunpaWarehouse,
	eItemTable_Deal,
	eItemTable_Max,
};

enum MONEY_FLAG {
	MF_NOMAL = 0, MF_OBTAIN, MF_LOST, MF_NONE, MF_PUTINGUILD = 20, MF_PUTOUTGUILD, MF_PUTINFAMILY, MF_PUTOUTFAMILY, MF_FEE,
};

enum EAPPEAR_PART
{
	eAppearPart_Body,
	eAppearPart_Hair,
	eAppearPart_Face,
	eAppearPart_Hand,
	eAppearPart_Foot,
	eAppearPart_Weapon,
	
	eAppearPart_Max
};

#define WALKSPEED		225
#define RUNSPEED		450


#define MOVE_LIMIT_ERRORDISTANCE	1000
#define MAX_NAME_LENGTH				16
#define MAX_SKILL_NAME_LENGTH		32
#define MAX_NPC_NAME_LENGTH		32
#define MAX_MAP_NAME_LENGTH		32
#define MAX_MONSTER_NAME_LENGTH		60
#define MAX_CHAT_LENGTH				127
#define	MAX_CHANNELTITLE_LENGTH		64
#define	MAX_PLAYROOMTITLE_LENGTH	64
#define MAX_ITEMNAME_LENGTH			32
#define MAX_TARGET_NUM		300

#define MAX_SENDDATE_LENGTH		16
#define MAX_NOTE_TITLE			36


#define MIN_CHAR_CHANGESIZE	0.3f
#define MAX_CHAR_CHANGESIZE	3.0f
#define POINT_VALID_DISTANCE		6500


#define REMOVEFADE_START_TIME		2500
#define MONSTERREMOVE_TIME			5000
#define PLAYERREVIVE_TIME			5000
// 낚시관련 Define
#define MAX_FISHING_LEVEL			4			// 낚시숙련도 단계
#define MAX_FISHING_MISSION			3			// 낚시미션개수
#define MAX_FISHLIST_FORPOINT		12			// 물고기리스트 : 포인트변환시 사용
#define MAX_FISHINGMISSIONCODE		99			// 
#define MAX_FISHING_DISTANCE		2300.0f		// 낚시 가능한 거리
#define MAX_FISHPOINT				9999999		// 최대 물고기포인트 9,999,999
#define FISHINGTIME_STARTPULLING	2000		// 낚시질 시작시간
#define FISHINGMISSION_INVALID		1000000		// 미션코드는 000000 ~ 999999 범위이므로 이를 초과하면 무효값.

enum eCOOKLEVEL{eCOOKLEVEL1=1, eCOOKLEVEL2=2, eCOOKLEVEL3=3, eCOOKLEVEL4=4};
enum eCOOKRECIPE{eCOOKRECIPE_ADD=1, eCOOKRECIPE_DEL=2, eCOOKRECIPE_UPDATE=3};
#define ERR_FULLLEANRN_RECIPE		-2
#define ERR_ALREADYLEARN_RECIPE		-1
#define MAX_COOKING_LEVEL			4					// 최대 요리숙련단계 4개
#define MAX_INGREDIENT_LIST			5					// 최대 재료목록 5개
#define MAX_RECIPE_LV4_LIST			10					// DB에 등록되는 최대 달인레시피목록 10개
#define MAX_RECIPE_DISPLIST			99					// UI에 출력되는 최대 레시피목록 99개
#define MAX_COOKING_DISTANCE		300.0f				// 요리 가능한 거리
#define LV4FOOD_EAT_LEVEL			75					// 달인 요리를 먹을 수 있는 레벨

// 하우징관련 Define
enum {eHOUSEFURNITURE_ADD=1, eHOUSEFURNITURE_DEL=2, eHOUSEFURNITURE_UPDATE=3};
enum eHOUSEFURNITURE_STATE
{
	eHOUSEFURNITURE_STATE_UNKEEP	= 0,		// 인벤토리에 보관중 (RemainTime 소진없음)
	eHOUSEFURNITURE_STATE_KEEP		= 1,		// 내집창고에 보관중 (RemainTime 소진없음)
	eHOUSEFURNITURE_STATE_INSTALL	= 2,		// 하우스에 설치상태 (RemainTime 소진됨)
	eHOUSEFURNITURE_STATE_UNINSTALL	= 3,		// 하우스에 설치했다가 내집창고에 보관중 (RemainTime 소진됨)
};
enum {
	eHOUSEERR_NONE			= 0,	// 성공(에러없음)
	eHOUSEERR_NOOWNER		= 1,	// 소유자없음
	eHOUSEERR_NOHOUSE		= 2,	// 하우스없음
	eHOUSEERR_HAVEHOUSE		= 3,	// 하우스소유
	eHOUSEERR_ENTRANCEFAIL	= 4,	// 입장실패
	eHOUSEERR_LOADING		= 5,	// DB에서 데이터로딩중
	eHOUSEERR_FULL			= 6,	// 하우스생성초과
	eHOUSEERR_DONOT_HOUSE	= 7,	// 하우스에서 처리불가능
	eHOUSEERR_DO_HOUSE		= 8,	// 하우스에서 가능
	eHOUSEERR_NOTOWNER		= 9,	// 소유자가 아님
	eHOUSEERR_MAX_EXTEND	= 10,	// 하우스확장 최대
	eHOUSEERR_DECOMODE		= 11,	// 꾸미기모드중 불가
	eHOUSEERR_NOTENOUGHPOINT = 12,	// DecoUse포인트 부족
	eHOUSEERR_INVAILDSTATE	= 13,	// 유효하지 않은 상태
	eHOUSEERR_NOFURNITURE	= 14,	// 가구가없음
	eHOUSEERR_HASRIDER		= 15,	// 탑승자 있음
	eHOUSEERR_NOTRIDING		= 16,	// 탑승상태 아님
	eHOUSEERR_RIDING		= 17,	// 탑승중
	eHOUSEERR_RESERVATING	= 18,	// 방문예약중
	eHOUSEERR_DISTANCE_FAR	= 19,	// 거리가 멀다
	eHOUSEERR_NOTREGIST		= 20,	// 주민등록 안했음.
	eHOUSEERR_HOME2HOME_FAIL = 21,	// 홈->홈 이동실패
	eHOUSEERR_ONVEHICLE		= 22,	// 탈것소환 or 탑승중
	eHOUSEERR_NORANKER		=23,	// 랭커가 아님 		

	eHOUSEERR_ERROR			   ,
};

enum eHouseLog
{
	// 아이템(인벤)<->가구(내집창고)
	eHouseLog_Keep	=	1,
	eHouseLog_UnKepp,

	// 내집창고
	eHouseLog_Install,
	eHouseLog_UnInstall,
	eHouseLog_EndTime,	//시간소멸
	eHouseLog_Destroy,

	// 하우징관련 아이템사용(인벤)
	eHouseLog_WallPaper,
	eHouseLog_FloorPaper,
	eHouseLog_CeillingPaper,
	eHouseLog_Extend,
	eHouseLog_ChangeHouseName,
};

enum {eHouseVisitByCheat=0, eHouseVisitByLink=1, eHouseVisitByItemRandom=2, eHouseVisitByItem=3, eHouseVisitByName=4};
enum {eHOUSEEXTEND_NONE=0, eHOUSEEXTEND_LV1=1, eHOUSEEXTEND_LV2=2, eHOUSEEXTEND_LV3=3, eHOUSEEXTEND_LV4=4};
#define MAX_HOUSING_CATEGORY_NUM	8		// 카테고리 : [0]임시 [1]가구 [2]가전 [3]문 [4]벽 [5]장판 [6]소품 [7]장식
#define MAX_HOUSING_SLOT_NUM		60		// 내집창고 슬롯갯수
#define MAX_HOUSING_NAME_LENGTH		32		// 하우징관련 이름
#define MAX_FURNITURE_STORAGE_NUM	(MAX_HOUSING_CATEGORY_NUM * MAX_HOUSING_SLOT_NUM)	// 내집창고 최대보관목록
//#define MAX_BONUS_LIST				20		// 최대 꾸미기효과 개수
#define MAX_HOUSE_DECOUSEPOINT		100		// 꾸미기 소모포인트

#define MAX_HOUSE_NUM				1000	// 열릴 수 있는 최대 하우스개수 (실제로는 MAX_HOUSE_NUM - 1 만큼 열린다)

#define HOUSE_INSTALL_MARGIN		1
#define	CHECK_PECENT				50.0f
#define	MAX_HOUSE_EXTEND_LEVEL		eHOUSEEXTEND_LV4
#define MAX_HOUSE_ACTION_SLOT		5
#define MAX_HOUSE_ATTATCH_SLOT		5
#define MAX_HOUSE_ACTION_DISTANCE	500.0f
#define MAX_START_FURNITURE_HEIGHT	6.0f

#define HOUSE_ACTIONINDEX_GETOFF	7		// 내리기 액션인덱스 

enum HOUSE_ACTION_TYPE
{
	eHOUSE_ACTION_TYPE_INVALID	= 0,		// 에러검출용
	eHOUSE_ACTION_TYPE_RIDE		= 1,		// 탑승
	eHOUSE_ACTION_TYPE_GETOFF	= 2,		// 내리기
	eHOUSE_ACTION_TYPE_BUFF		= 3,		// 버프
	eHOUSE_ACTION_TYPE_NORMAL	= 4,		// 애니,이펙트
	eHOUSE_ACTION_TYPE_STORAGE	= 5,		// 창고

	eHOUSE_ACTION_TYPE_DOOR_EXITHOUSE	= 1000,
	eHOUSE_ACTION_TYPE_DOOR_VOTEHOUSE	= 1001,

	// 091105 pdy 하우징 가구 액션추가 ( 미니홈피 웹 브라우저 링크 ) 
	eHOUSE_ACTION_TYPE_OPEN_HOMEPAGE = 2000,	
};

// 하우징파일 네이밍 룰
enum HOUSE_HIGHCATEGORY
{
	eHOUSE_HighCategory_Furniture		= 1,
	eHOUSE_HighCategory_Electric		= 2,
	eHOUSE_HighCategory_Door			= 3,
	eHOUSE_HighCategory_Wall			= 4,
	eHOUSE_HighCategory_Carpet			= 5,
	eHOUSE_HighCategory_Properties		= 6,
	eHOUSE_HighCategory_Decoration		= 7,
	
	eFN_HighCatefory_Max,
};

enum HOUSE_LOWCATEGORY_FURNITURE
{
	eHOUSE_LowCategory_Furniture_Etc		= 1,
	eHOUSE_LowCategory_Furniture_Table		= 2,
	eHOUSE_LowCategory_Furniture_Chair		= 3,
	eHOUSE_LowCategory_Furniture_Sofa		= 4,
	eHOUSE_LowCategory_Furniture_Closet		= 5,
	eHOUSE_LowCategory_Furniture_Chest		= 6,
	eHOUSE_LowCategory_Furniture_Drawer		= 7,
	eHOUSE_LowCategory_Furniture_Bed		= 8,
	
	eHOUSE_LowCategory_Furniture_Max,
};

enum HOUSE_LOWCATEGORY_ELECTRIC
{
	eHOUSE_LowCategory_Electric_Etc		= 1,
	eHOUSE_LowCategory_Electric_Ref		= 2,
	eHOUSE_LowCategory_Electric_Tv		= 3,
	
	eHOUSE_LowCategory_Electric_Max,
};

enum HOUSE_LOWCATEGORY_DOOR
{
	eHOUSE_LowCategory_Door_Etc			= 1,
	eHOUSE_LowCategory_Door_Frame		= 2,
	eHOUSE_LowCategory_Door_Window		= 3,
	eHOUSE_LowCategory_Door_Watch		= 4,
	eHOUSE_LowCategory_Door_Door		= 5,

	eHOUSE_LowCategory_Door_Max,
};

enum HOUSE_LOWCATEGORY_WALL
{
	eHOUSE_LowCategory_Wall_Etc			= 1,
	eHOUSE_LowCategory_Wall_Wall		= 2,

	eHOUSE_LowCategory_Wall_Max,
};

enum HOUSE_LOWCATEGORY_CARPET
{
	eHOUSE_LowCategory_Carpet_Etc		= 1,
	eHOUSE_LowCategory_Carpet_Tile		= 2,

	eHOUSE_LowCategory_Carpet_Max,
};

enum HOUSE_LOWCATEGORY_PROPERTIES
{
	eHOUSE_LowCategory_Properties_Etc		= 1,
	eHOUSE_LowCategory_Properties_Frame		= 2,
	eHOUSE_LowCategory_Properties_FlowerPot	= 3,
	eHOUSE_LowCategory_Properties_Cushion	= 4,
	eHOUSE_LowCategory_Properties_Clock		= 5,
	eHOUSE_LowCategory_Properties_Doll		= 6,
	eHOUSE_LowCategory_Properties_Basket	= 7,
	
	eHOUSE_LowCategory_Properties_Max,
};

enum HOUSE_LOWCATEGORY_DECORATION
{
	eHOUSE_LowCategory_Decoration_Etc		= 1,
	eHOUSE_LowCategory_Decoration_FirePlace	= 2,
	eHOUSE_LowCategory_Decoration_Mark		= 3,
	eHOUSE_LowCategory_Decoration_Lamp		= 4,
	eHOUSE_LowCategory_Decoration_Bronze	= 5,
	eHOUSE_LowCategory_Decoration_CCover	= 6,
	
	eHOUSE_LowCategory_Decoration_Max,
};

enum HOUSE_WORKINDEX
{
	eHOUSE_WorkIndex_A0		= 1,
	eHOUSE_WorkIndex_B0		= 2,
	eHOUSE_WorkIndex_C0		= 3,
	eHOUSE_WorkIndex_D0		= 4,
	eHOUSE_WorkIndex_E0		= 5,
	eHOUSE_WorkIndex_F0		= 6,

	eHOUSE_WorkIndex_Max,
};

enum HOUSE_FUNCTION
{
	eHOUSE_Function_None	= 0,
	eHOUSE_Function_Wall	= 1,
	eHOUSE_Function_Floor	= 2,
	eHOUSE_Function_Door	= 3,
	eHOUSE_Function_Start	= 4,
	eHOUSE_Function_Action	= 5,

	eHOUSE_Function_Max,
};

enum HOUSE_FURNITUREINDEX_PARTS
{
	eHOUSE_FurnitureIndex_Part_HighCategory,
	eHOUSE_FurnitureIndex_Part_LowCategory,
	eHOUSE_FurnitureIndex_Part_Work,
	eHOUSE_FurnitureIndex_Part_Index,
	eHOUSE_FurnitureIndex_Part_Action,
	
	eHOUSE_FurnitureIndex_Part_Max,
};

#define HOUSE_HIGHCATEGORY_CHAR_FURINITURE			"FU"
#define HOUSE_HIGHCATEGORY_CHAR_ELECTRIC			"EL"
#define HOUSE_HIGHCATEGORY_CHAR_DOOR				"DO"
#define HOUSE_HIGHCATEGORY_CHAR_WALL				"WA"
#define HOUSE_HIGHCATEGORY_CHAR_CARPET				"CA"
#define HOUSE_HIGHCATEGORY_CHAR_PROPERTIES			"PR"
#define HOUSE_HIGHCATEGORY_CHAR_DECORATION			"DE"

#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_ETC		"ETC"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_TABLE		"TABLE"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_CHAIR		"CHAIR"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_SOFA		"SOFA"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_CLOSET		"CLOSET"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_CHEST		"CHEST"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_DRAWER		"DRAWER"
#define HOUSE_LOWCATEGORY_CHAR_FURNITURE_BED		"BED"

#define HOUSE_LOWCATEGORY_CHAR_ELECTRIC_ETC			"ETC"
#define HOUSE_LOWCATEGORY_CHAR_ELECTRIC_REF			"REF"
#define HOUSE_LOWCATEGORY_CHAR_ELECTRIC_TV			"TV"

#define HOUSE_LOWCATEGORY_CHAR_DOOR_ETC				"ETC"
#define HOUSE_LOWCATEGORY_CHAR_DOOR_FRAME			"FRAME"
#define HOUSE_LOWCATEGORY_CHAR_DOOR_WINDOWS			"WINDOW"//"WINDOWS"
#define HOUSE_LOWCATEGORY_CHAR_DOOR_WATCH			"WATCH"
#define HOUSE_LOWCATEGORY_CHAR_DOOR_DOOR			"DOOR"

#define HOUSE_LOWCATEGORY_CHAR_WALL_ETC				"ETC"
#define HOUSE_LOWCATEGORY_CHAR_WALL_WALL			"WALL"

#define HOUSE_LOWCATEGORY_CHAR_CARPET_ETC			"ETC"
#define HOUSE_LOWCATEGORY_CHAR_CARPET_TILE			"TILE"

#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_ETC		"ETC"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_FRAME		"FRAME"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_FLOWERPOT	"FLOWERPOT"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_CUSHION	"CUSHION"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_CLOCK		"CLOCK"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_DOLL		"DOLL"
#define HOUSE_LOWCATEGORY_CHAR_PROPERTIES_BASKET	"BASKET"

#define HOUSE_LOWCATEGORY_CHAR_DECORATION_ETC		"ETC"
#define HOUSE_LOWCATEGORY_CHAR_DECORATION_FIREPLACE	"FIREPLACE"
#define HOUSE_LOWCATEGORY_CHAR_DECORATION_MARK		"MARK"
#define HOUSE_LOWCATEGORY_CHAR_DECORATION_LAMP		"LAMP"
#define HOUSE_LOWCATEGORY_CHAR_DECORATION_BRONZE	"BRONZE"
#define HOUSE_LOWCATEGORY_CHAR_DECORATION_CCOVER	"CCOVER"

#define HOUSE_WORK_CHAR_A0							"A0"
#define HOUSE_WORK_CHAR_B0							"B0"
#define HOUSE_WORK_CHAR_C0							"C0"
#define HOUSE_WORK_CHAR_D0							"D0"
#define HOUSE_WORK_CHAR_E0							"E0"
#define HOUSE_WORK_CHAR_F0							"F0"

#define HOUSE_FUNCTION_CHAR_NONE					"N"
#define HOUSE_FUNCTION_CHAR_WALL					"W"
#define HOUSE_FUNCTION_CHAR_FLOOR					"F"
#define HOUSE_FUNCTION_CHAR_DOOR					"D"
#define HOUSE_FUNCTION_CHAR_START					"S"
#define HOUSE_FUNCTION_CHAR_ACTION					"A"

enum eDIFFICULTY
{
	eDIFFICULTY_NONE	= 0,
	eDIFFICULTY_EASY	= 1,
	eDIFFICULTY_NORMAL	= 2,
	eDIFFICULTY_HARD	= 3,
	eDIFFICULTY_MAX,
};


#define MAX_DUNGEON_NUM	100

enum {
	eDungeonERR_None			= 0,	// 성공(에러없음)
	eDungeonERR_NoKeyInfo		= 1,	// 키정보없음
	eDungeonERR_NoParty			= 2,	// 파티없음
	eDungeonERR_NotEnoughLevel	= 3,	// 레벨미달
	eDungeonERR_OverLevel		= 4,	// 레벨초과
	eDungeonERR_EntranceDelay	= 5,	// 입장딜레이시간
	eDungeonERR_OverCount		= 6,	// 입장횟수초과
	eDungeonERR_DungeonMapOff	= 7,	// 던전맵서버 Off
	eDungeonERR_DungeonFull		= 8,	// 던전맵 Full
	eDungeonERR_NoNPCInfo		= 9,	// NPC 정보 없음

	eDungeonERR_WrongParam		= 10,	// 파라미터 잘못
	eDungeonERR_NoChannel		= 11,	// 채널 없음

	eDungeonERR_WrongDiffculty	= 12,	// 다른 난이도의 인던입장
	eDungeonERR_CreateFailed	= 13,	// 던전 생성 실패

	eDungeonERR_ERROR			= 99,	// 미지정 에러
};


enum eTimerType
{
	eTimerType_CountDown	= 0,
	eTimerType_CountUp		= 1,
};

enum eTimerState
{
	// 동작과 관련있는 상태는 Ready, Run, Pause, Stop의 네가지이다.
	eTimerState_Ready		= 0,
	eTimerState_Run			= 1,

	// 동작을 변경하기 위해 의뢰되는 상태이다. 결과는 Run, Pause, Stop으로 나타난다.
	eTimerState_Pause		= 2,	// 결과 : Pause
	eTimerState_Resume		= 3,	// 결과 : Run
	eTimerState_Stop		= 4,	// 결과 : Stop
};

#define CONSIGNMENT_DEPOSIT_RATE		0.001
#define CONSIGNMENT_COMMISSION_RATE		0.03
#define CONSIGNMENT_SEARCHINDEX_NUM		10

enum {
	eConsignmentERR_None			= 0,	// 성공(에러없음)
	eConsignmentERR_WrongState		= 1,	// 잘못된 상태
	eConsignmentERR_WrongItem		= 2,	// 잘못된 아이템
	eConsignmentERR_WrongParam		= 3,	// 잘못된 인자
	eConsignmentERR_PoorMoney		= 4,	// 골드부족
	eConsignmentERR_PoorSlot		= 5,	// 인벤부족
	eConsignmentERR_FastRequest		= 6,	// 빠른요청
	eConsignmentERR_GoldOver		= 7,	// 골드초과

	eConsignmentERR_ERROR			= 99,	// 미지정 에러
};

enum {
	eConsignmentSORT_NameAsc		= 0,
	eConsignmentSORT_NameDesc		= 1,
	eConsignmentSORT_LevelAsc		= 2,
	eConsignmentSORT_LevelDesc		= 3,
	eConsignmentSORT_PriceAsc		= 4,
	eConsignmentSORT_PriceDesc		= 5,
};

enum {
	eConsignmentUPDATEKIND_CancelByUser	= 0,
	eConsignmentUPDATEKIND_CancelByTime	= 1,
	eConsignmentUPDATEKIND_Buy			= 2,
};

enum {
	eConsignmentDBResult_RegistedOver		= -4, // 등록초과
	eConsignmentDBResult_DurabilityErr		= -3, // 수량불일치
	eConsignmentDBResult_PoorMoney			= -2, // 골드부족
	eConsignmentDBResult_Err				= -1, // 에러
	eConsignmentDBResult_None				= 0,
	eConsignmentDBResult_SoldOut			= 1, // 모두판매됨
	eConsignmentDBResult_UserCancel_Ack		= 2, // 유저취소성공
	eConsignmentDBResult_UserCancel_Nack	= 3, // 유저취소실패
	eConsignmentDBResult_TimeCancel_Ack		= 4, // 기간취소성공
	eConsignmentDBResult_TimeCancel_Nack	= 5, // 기간취소실패
	eConsignmentDBResult_Buy_Ack			= 6, // 구매성공
	eConsignmentDBResult_Buy_Nack			= 7, // 구매실패
};

enum eConsignmentLog
{
	eConsignmentLog_Regist			= 0, // 등록
	eConsignmentLog_Buy				= 1, // 구입
	eConsignmentLog_SellUpdate		= 2, // 갱신 (부분판매시 원본아이템의 수량변경)
	eConsignmentLog_SoldOut			= 3, // 판매완료
	eConsignmentLog_UserCancel		= 4, // 유저취소
	eConsignmentLog_TimeCancel		= 5, // 기간취소
	eConsignmentLog_GetPackage		= 6, // 소포획득
	eConsignmentLog_GetPackageFail	= 7, // 소포획득실패
	eConsignmentLog_DelByDate		= 8, // 소포기간삭제
};

enum BASICEFFECT
{
	eEffect_NewCharacter = 1,	//A¨Iⓒª¡E¡þ?AI ¡IiiAa
	eEffect_Standard = 2,		//A¨Iⓒª¡E¡þ?AI ￠®¨ua¡§¡þ￠®i(¨Io￠®i￠®¨ua¡§u¨I¨￡AI)
	eEffect_Walk,	//￠®¨¡E￠®¨ua (¨Io￠®i￠®¨ua ¡§u¨I¨￡AI)
	eEffect_Run,	//¡EOU￠®¨ua (¨Io￠®i￠®¨ua¡§u¨I¨￡AI)
	eEffect_BattleWalk_Gum,		//AuAo ￠®¨¡E￠®¨ua(￠®¨¡E_¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleWalk_Gwun,		//AuAo ￠®¨¡E￠®¨ua(￠®¨uC¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleWalk_Do,		//AuAo ￠®¨¡E￠®¨ua(¡Ii¡Ii_¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleWalk_Chang,		//AuAo ￠®¨¡E￠®¨ua(A￠®E_¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleWalk_Gung,		//AuAo ￠®¨¡E￠®¨ua(￠®¨uA_¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleWalk_Amgi,		//AuAo ￠®¨¡E￠®¨ua(¡§uI￠®¨ua_¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleRun_Gum,		//AuAo ¡EOU￠®¨ua(￠®¨¡E_¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleRun_Gwon,		//AuAo ¡EOU￠®¨ua(￠®¨uC_¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleRun_Do,		//AuAo ¡EOU￠®¨ua(¡Ii¡Ii_¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleRun_Chang,		//AuAo ¡EOU￠®¨ua(A￠®E¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleRun_Gung,		//AuAo ¡EOU￠®¨ua(￠®¨uA_¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_BattleRun_Amgi,		//AuAo ¡EOU￠®¨ua(¡§uI￠®¨ua_¨Io￠®i￠®¨uaAaA¨I¨￡)
	eEffect_UngijosikStart,	//¡E?i￠®¨uav¡§oA(¡§uE¡E¡IA¡Ii¡E?AU)
	eEffect_Ungijosik,	//¡E?i￠®¨uav¡§oA
	eEffect_Ungijosik_End,	//¡E?i￠®¨uav¡§oA (¨Iⓒª¡E￠c¨Iⓒª￠®¨IEA)
	eEffect_UngijosikDamage,	//¡E?i￠®¨uav¡§oA A¨I¡þ¡E?¡E￠c ¡Ii￠®I¨IoIAo
	eEffect_StallStart,	//¨Iⓒªea￠®io ¡Ii¡E?AU1 (¡§uE￠®¨ua)
	eEffect_Stall,	//¨Iⓒªea￠®io ¡Ii¡E?AU1 (¡§uE¡§u¡§￠® AO￠®¨ua)
	eEffect_StallEnd,	//¨Iⓒªea￠®io ¡Ii¡E?AU1 (¡§u￠®¨I￠®¨ua)
	eEffect_NormalDamage = 13,		//¡Ii￠®I¨IoIAo1 (AA)
	eEffect_CriticalDamage = 14,		//¡Ii￠®I¨IoIAo2 (¡E?i)
	eEffect_Die1 = 16,		//A￠®￠?= 1
	eEffect_Died1 = 16,		//A￠®￠?¡§ui¡§u￠®¨I ¡E¡I¡§I¡E?o AO¡E¡IA ¡E¡þ¨I¡I¡§oA 1
	eEffect_Die2 = 15,		//A￠®￠?= 2
	eEffect_Died2 = 15,		//A￠®￠?¡§ui¡§u￠®¨I ¡E¡I¡§I¡E?o AO¡E¡IA ¡E¡þ¨I¡I¡§oA 2
	eEffect_LevelUpSentence = 3,	//￠®￠´¨Iiⓒo¡Ei¨u￠®¡¿¡§u￠®A ￠®¨uUAU
	eEffect_MouseMovePoint,	//¡E¡þ¡EO¡E?i¡§o¡§¡þ AI¡Ii¡E?¡§￠®￠®AAI¡§￠®¡Ec
	
	eEffect_GetMoney = 5,	// ¡E¡þ￠®AA￠®￠?¡E?¡§I¡§u￠®¨I ¡Ii￠®￠´ ¡§uo￠®¨ua
	eEffect_GetItem_Accessory = 25,	// ¡E¡þ￠®AA￠®￠?¡E?¡§I¡§u￠®¨I ¡§u¡§￠®AIA¡§￠® ¡§uo￠®¨ua

	eEffect_Emoticon_Bow,	//AI￠®ic
	eEffect_Emoticon_Thanks,	//￠®¨¡￠®¡×￠®ic
	eEffect_Emoticon_Yes,	//￠®¨uad 
	eEffect_Emoticon_No,	//¡§¡þId
	eEffect_Emoticon_Pleasure,	//￠®¨ua￠®iY
	eEffect_Emoticon_Sadness,	//¡§o¡§oCA
	eEffect_Emoticon_Love,	//￠®ic¡EOu
	eEffect_Emoticon_Hostility,	//Au¡E¡Ie
	eEffect_Emoticon_Disappoint,	//¡§oC¡E¡þA
	eEffect_Emoticon_Angry,	//¡§¡þ¡§¡E¨Iⓒªe
	eEffect_Emoticon_Suprised,	//¨Iⓒªi¡EO￠®A
	eEffect_Emoticon_Evasion,	//E¡E¡þCC
	eEffect_Emoticon_Declare,	//¡§u￠®¨u¡§u¨I¡I
	eEffect_Defence_Gum,		//¨Iiⓒo|i¡¾¡§ui¡Ii¡E?AU(￠®¨¡E)
	eEffect_Defence_Gwun,		//¨Iiⓒo|i¡¾¡§ui¡Ii¡E?AU(￠®¨uC)
	eEffect_Defence_Do,		//¨Iiⓒo|i¡¾¡§ui¡Ii¡E?AU(¡Ii¡Ii)
	eEffect_Defence_Chang,		//¨Iiⓒo|i¡¾¡§ui¡Ii¡E?AU(A￠®E)
	eEffect_Defence_Gung,		//¨Iiⓒo|i¡¾¡§ui¡Ii¡E?AU(￠®¨uA)
	eEffect_Defence_Amgi,		//¨Iiⓒo|i¡¾¡§ui¡Ii¡E?AU(¡§uI￠®¨ua)

	eEffect_GetItem_Weapon = 7,		//¨Iiⓒo|i¡¾¡§ui¡Ii¡E?AU(¡§uI￠®¨ua)
	eEffect_GetItem_Dress = 8,
	eEffect_GetItem_Quest = 10,
	eEffect_GetItem_Youngyak = 6,

	eEffect_MonRegen = 80,				// 앞으로 몬스터 리젠은 eEffect_NewCharacter 이펙트를 사용함
	eEffect_ChangeStage_Hwa = 98,
	eEffect_ChangeStage_Geuk = 99,
	eEffect_PetSummon = 3151,			// 펫소환
	eEffect_PetSeal		= 3152,			// 펫봉인
	eEffect_FeedUp		= 3153,			// 펫먹이
	eEffect_PetWarp		= 3154,			// 펫워프

	eEffect_ShopItem_Life = 3200,		// 금창약	
	eEffect_ShopItem_NaeRuyk,			// 요상단	
	eEffect_ShopItem_Revive,			// 부활주문서
	eEffect_ShopItem_Boom1,				// 폭죽(나비)
	eEffect_ShopItem_Boom2,				// 폭죽(별)
	eEffect_ShopItem_Hair1,				// 가발1	
	eEffect_ShopItem_Hair2,				// 가발2	
	eEffect_ShopItem_Hair3,				// 가발3	
	eEffect_ShopItem_Teleport1,			// 텔레포트1
	eEffect_ShopItem_Teleport2,			// 텔레포트2
	eEffect_ShopItem_Teleport3,			// 텔레포트3
	eEffect_ShopItem_MaxLife,			// 생명의 물약
	eEffect_ShopItem_MaxNaeRuyk,		// 내력의 물약

	// 070528 LYW CommonGameDefine : Add effect of emoticon part.
	eEffect_Emoticon1 = 51, 
	eEffect_Emoticon2 = 52, 
	eEffect_Emoticon3 = 53, 
	eEffect_Emoticon4 = 54, 
	eEffect_Emoticon5 = 55, 
	eEffect_Emoticon6 = 56, 

	eEffect_GetItem_Unique	= 40,
	eEffect_GetItem_Rare	= 41,

	// 071206 LUJ 길드 졸업 효과
	eEffect_Guild_Graduate	= 49,
};

enum MapName
{
	MapNameNone,
	Alker_Farm = 2,
	// 090921 LUJ, 네라 농장 추가
	Nera_Farm = 3,
	Moon_Blind_Swamp = 13,
	Red_Orc_Outpost,	
	Moon_Blind_Forest,	
	Haunted_Mine_1F = 16,
	Haunted_Mine_2F = 17,
	The_Gate_Of_Alker = 19,
	Alker_Harbor,	

	Ruins_Of_Draconian,	
	Zakandia,	
	Tarintus,	
	Mont_Blanc_Port = 25,	
	Dryed_Gazell_Fall,		
	Zakandia_Outpost,		
	The_Dark_Portal,		
	Distorted_crevice = 29,
	The_Way_To_The_Howling_Ravine = 31,
	Howling_Ravine = 32,
	Howling_Cave_1F = 33,
	Howling_Cave_2F = 34,
	Ghost_Tree_Swamp = 41, 
	Lair_of_Kierra = 42,
	The_Valley_Of_Fairy = 51,
	The_Town_of_Nera_Castle = 52,
	The_Great_Garden = 53,
	// 080714 LUJ, "기사단의 묘지" 맵 추가
	TheKnightsGrave	= 54,
	A_Harbor_of_Nera = 55,
	Temple_of_Greed = 56,
	Broken_Shipwrecked = 57,
	The_tower_of_wizardly_Low = 60,
	The_tower_of_wizardly_Middle = 61,
	The_tower_of_wizardly_High = 62,
	The_Death_tower = 63,
	The_Spirit_Forest = 64,
	The_Cristal_Empire = 65,
	TreeOfKenai = 67,
	TheCryingTavern = 68,
	TheUndergroundPassage = 69,
	SahelZone = 71,
	MirageInTheParasus = 72,
	RuinsOfAncientElpis = 73,
	DragoniansTomb1 = 74,
	DragoniansTomb2 = 75,
 	// 080905 LYW --- CommonGameDefine : 공성 관련 맵 이름 추가.
	The_Castle_of_Rushen = 81,
	The_Culvert = 82,
	The_Castle_of_Zevyn = 83,
 	Advanced_Dungeon = 84,

	DateDungeon_Cemetery = 91,
	DateDungeon_BrokenHouse = 92,
	DateDungeon_DarkCave = 93,

	GuildTournamentStadium = 94,
	// 090609 LUJ, 신규 지역 추가
	DesolatedGorge = 96,

	DateDungeon_CookieGarden = 97,

	AlkerResidence = 99,
	
	// 090626 NYJ 최대 맵 갯수 200개로 확장
	MAX_MAP_NUM = 200,
	//MAX_MAP_NUM,
};

enum EObjectState
{
	eObjectState_None,						// ¡§u¡§￠®¨Io￠®i￠®ioAA ¡§u¡§￠®¡E¡IO..
	eObjectState_Enter,						// A¨Iⓒª¡E¡þ?AI¨Iⓒª¡§¡I ¡E¡þo¡§o¡§¡þAI￠®¨¡¡E￠c A¨Iⓒª= ¡IiiAaCO¡EO￠®¡¿ AC ￠®ioAA
	eObjectState_Move,						// AAC￠®I AI¡Ii¡E? ￠®ioAA
	eObjectState_Rest,						// ¡E¡þo¡§o¡§¡þAIAC E¡§¡ⓒ¡§oA ￠®ioAA
	
//	eObjectState_Speech,					// Npc¡E?I ¡E¡IeE￠®¨I ￠®ioAA
	eObjectState_Deal,						// ￠®ioa Open ￠®ioAA

	eObjectState_Exchange,					// ￠®¨u¨IⓒªE? ￠®ioAA
	eObjectState_StreetStall_Owner,			// ¨Iⓒªea￠®io Open ￠®ioAA
	eObjectState_StreetStall_Guest,			// ¨Iⓒªea￠®io ¡§uO¡E¡IO ￠®ioAA
	eObjectState_PrivateWarehouse,			// ￠®¨¡¨IⓒªAIA￠®E￠®¨¡i 10
	eObjectState_Munpa,						//¨Io¡Ec¡§￠®A ¡E¡þ¡Ec¡§iⓒo¡Ei¨u¡§￠®¡Ec¨Iⓒª¡§¡I ¨Io¡Ec¡§￠®AA￠®E

	eObjectState_SkillStart,				// ¡§o¡§¡þA¨Iⓒª ¡§oAAu ￠®ioAA
	eObjectState_SkillSyn,					//
	eObjectState_SkillBinding,				//
	eObjectState_SkillUsing,				//
	eObjectState_SkillDelay,				// A¡§¡ⓒ¡§¡þ¡E¡þ￠®¨¡¡E￠c ¨I¡A￠®!¨¡a; ¡EO￠®¡¿, ¨Io￠®i￠®¨¡¨I㎕鈐??¨I¡A￠®¨¡i¨Iⓒª￠®¨IEAAC ¡Iio￠®￠´¨IoAI
	
	eObjectState_TiedUp_CanMove,			// ￠®¨¡a¨IoU ￠®ioAA
	eObjectState_TiedUp_CanSkill,
	eObjectState_TiedUp,
	
	eObjectState_Die,						// A￠®￠?: ￠®ioAA 20	
	
	eObjectState_BattleReady,
	eObjectState_Exit,						//KES 040707 A¨Iⓒª¡E¡þ?AI￠®¨¡¡E￠c ¨Iⓒª¡§¡I￠®¨¡￠®I¡EO￠®¡¿
	eObjectState_Immortal,					//¡¤I¡¾¡¿AI, ￠￢EAuE?, AuAa'A￠® ¡ii¨u¨¡ⓒø¨￡¡¾a¨oA ⓒo¡iAu¡ioAA
	eObjectState_ImmortalDefense,			//

	eObjectState_Society,					//KES A¿¹A´IÆ¼ μ¿AU
	
	eObjectState_ItemUse,

	eObjectState_TournamentReady,
	eObjectState_TournamentProcess,
	eObjectState_TournamentEnd,
	eObjectState_TournamentDead,

	eObjectState_Engrave,

	// 070503 LYW --- CommonGameDefine : Add enum to setting rest damage.
	eObjectState_RestDamage,
	eObjectState_Connect,					// 웅주 070823

	// 080428 NYJ --- 낚시상태
	eObjectState_Fishing,
	eObjectState_FishingResult,

	eObjectState_Cooking,
	eObjectState_Housing,
	eObjectState_HouseRiding,
	
	eObjectState_MAX,	
};

enum eObjectBattleState
{
	eObjectBattleState_Peace,
	eObjectBattleState_Battle,
	eObjectBattleState_Max,
};

#define FARM_RENDER_OBJ_ID_START	0x8fffffff
// E 농장시스템 추가 added by hseos 2007.05.07
#define STATICNPC_ID_START		0xa0000000
#define STATICNPC_ID_MAX		0x10000000
#define SKILLOBJECT_ID_START	0xc0000000
#define SKILLOBJECT_ID_MAX		0x0000ffff
#define BATTLE_ID_START			0xd0000000
#define BATTLE_ID_MAX			0x0000ffff
#define TEMP_SKILLOBJECT_ID		0xffffffff
#define BATTLE_TEAMEFFECT_ID	0xfffffffe
#define BATTLE_STAGEEFFECT_ID	0xfffffffd
#define STATE_EFFECT_ID			0xfffffffc
#define PK_EFFECT_ID			0xfffffffb
#define NPCMARK_1_ID			0xfffffffa		// !
#define NPCMARK_2_ID			0xfffffff9		// ?
#define BATTLE_GTTEAM1_EFFECTID	0xfffffff7
#define BATTLE_GTTEAM2_EFFECTID	0xfffffff6
#define ENGRAVE_EFFECTID		0xfffffff4
#define ENGRAVE_ACK_EFFECTID	0xfffffff3
#define ENGRAVE_NACK_EFFECTID	0xfffffff2
#define LIFE_RECOVER_EFFECTID	0xfffffff1
#define MANA_RECOVER_EFFECTID	0xfffffff0
#define BATTLE_GTTEAM3_EFFECTID	0xffffffef
#define ALL_RECOVER_EFFECTID	0xffffffee
#define DETECT_EFFECT_ID		0xffffffea

#define HELMET_EFFECT			0xffffffe9
#define SHIELD_EFFECT			0xffffffe8
#define HEART_EFFECT			0xffffffe7
#define ITEM_WORKING_SUCCESS_EFFECT	0xffffffe6
#define ITEM_WORKING_FAIL_EFFECT	0xffffffe5
#define ITEM_WORKING_EFFECT			0xffffffe4

#define FISHING_START_EFFECT		0xffffffe3
#define FISHING_SUCCESS_EFFECT		0xffffffe2
#define FISHING_FAIL_EFFECT			0xffffffe1

#define BATTLE_GTIMMORTAL_ID		0xffffffe0

// 080827 LYW --- CommonGameDefine : 워터시드 사용 이펙트 번호를 지정한다.
#define SIEGEWARFARE_USING_WATERSEED	0xffffffdf

#define ITEM_SUMMON_EFFECTACTIONID	0xffffffde
#define ITEM_SUMMON_EFFECTID		0xffffffdd

#define COOKING_EFFECT				0xffffffdc
#define HOUSE_DECOMODE_EFFECT		0xffffffdb
// 100406 ShinJS --- 시간제한 퀘스트 !, ? Effect ID
#define NPCMARK_3_ID			0xffffffda		// !
#define NPCMARK_4_ID			0xffffffd9		// ?
typedef BYTE DIRINDEX;
#define MAX_DIRECTION_INDEX		32
#define DEGTODIR(angleDeg)	(BYTE)((angleDeg)/360*MAX_DIRECTION_INDEX)
#define DIRTODEG(DirIndex)	(((float)DirIndex)*360/MAX_DIRECTION_INDEX)
#define DIRTORAD(DirIndex)	(DEGTORAD(DIRTODEG(DirIndex)))
#define RADTODIR(angleRad)	(DEGTODIR(RADTODEG(angleRad)))

// A￠®i¡E¡þ¡§¡ⓒ¡EOo ¡§u¨Iⓒªd
#define CAMERA_NEAR		100
#define MAX_CHARTARGETPOSBUF_SIZE	15

//system
#define MAX_IPADDRESS_SIZE	16

// type : 0->¡§oA￠®¨¡￠®I¡EO￠®¡¿¡§¡þ￠®¨¡ ¨Iⓒª¨I¡A￠®¨ue. 1~100->AId￠®￠´¡Ec AI￠®io¡§¡þ?E￠®¨I¡§oA ¨Iⓒª¨I¡A￠®¨ue(￠®¨¡¡E￠c￠®ie), 101~200->(￠®¨¡￠®¡×￠®ie)
// 1->￠®ioa¡E?¡E￠c ¡§￠®C¡E¡þA, 2->¨Iⓒªea¡E?¡E￠c¡§u￠®¨I ¡§￠®C¡E¡þA, 3->¡E¡þo¡§o¡§¡þAI¡E?¡E￠c￠®¨¡O E¨Io¡Ii¨I￠®, 4->￠®¨u¨IⓒªE?, 5->A￠®E￠®¨¡i¡E?¡E￠c¡§u￠®¨I ￠®i￠®ⓒ­¡EO￠®¡¿, 6->¨Io¡Ec¡§￠®AA￠®E￠®¨¡i¡E?¡E￠c¡§u￠®¨I ￠®i￠®ⓒ­¡EO￠®¡¿
// 101->￠®ioa¡E?¡E￠c¡§u￠®¨I ￠®ii¡EO￠®¡¿, 102->¨Iⓒªea¡E?¡E￠c¡§u￠®¨I ￠®ii¡EO￠®¡¿, 103->A￠®￠?¡§uu;¡EO￠®¡¿, 104->￠®¨u¨IⓒªE?, 105->A￠®E￠®¨¡i¡E?¡E￠c ¨IⓒªO=, 106->¨Io¡Ec¡§￠®AA￠®E￠®¨¡i¡E?¡E￠c ¨IⓒªO=, 107->C￠®I￠®¨u¨Io ￠®ii¡EO￠®¡¿
enum eMoneyLogtype
{
	eMoneyLog_Time				= 0,
// ￠®¨¡¡E￠c￠®ie
	eMoneyLog_GetStall			= 1,
	eMoneyLog_GetStreetStall	= 2,
	eMoneyLog_GetMonster		= 3,
	eMoneyLog_GetExchange		= 4,
	eMoneyLog_GetStorage			= 5,
	eMoneyLog_GetGuild			= 6,
	eMoneyLog_GetWantedPrize	= 7,
	eMoneyLog_GetPKLooting		= 8,
	eMoneyLog_GetSuryunFail		= 9,
	eMoneyLog_GetPrize			= 10,
	eMoneyLog_GetSWProfit		= 11,
	eMoneyLog_GetFromQuest		= 12,
	eMoneyLog_GetGMTool			= 13,
	eMoneyLog_GetFamily,
	// desc_hseos_몬스터미터01
	// S 몬스터미터 추가 added by hseos 2007.07.30
	eMoneyLog_GetMonstermeter,
	// E 몬스터미터 추가 added by hseos 2007.07.30

	
// ￠®¨¡￠®¡×￠®ie
	eMoneyLog_LoseStall			= 101,
	eMoneyLog_LoseStreetStall	= 102,
	eMoneyLog_LoseDie			= 103,
	eMoneyLog_LoseExchange		= 104,
	eMoneyLog_LoseStorage		= 105,
	eMoneyLog_LoseGuild			= 106,
	eMoneyLog_LoseStorageBuy		= 107,
	eMoneyLog_LoseWantedRegist	= 108,
	eMoneyLog_LoseWantedBuyRight = 109,
	eMoneyLog_LosePKLooted		= 110,
	eMoneyLog_LosePKModeExitPanelty = 111,
	eMoneyLog_LoseBadFrameBail	= 112,
	eMoneyLog_LoseSuryunFee		= 113,
	eMoneyLog_LoseGuildLevelUp	= 114,
	eMoneyLog_LoseChangeLocation	= 115,
	eMoneyLog_LoseGMTool		= 116,
	eMoneyLog_LoseFamily,
	eMoneyLog_LoseFamilyLevelUp,
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.08.23	2007.09.10	2008.01.15
	eMoneyLog_LoseFarmBuy,
	eMoneyLog_LoseFarmUpgrade,
	eMoneyLog_LoseFarmTax,
	eMoneyLog_TollForVehicle,
	eMoneyLog_Enchant,
	eMoneyLog_TriggerAction,
	eMoneyLog_Skill,
	eMoneyLog_GetCheatMoney = 201,

	eMoneyLog_Consignment_Charge,
	eMoneyLog_MoneyFromNote,
};

enum eLogSkill
{
	eLog_SkillLearn = 0,
	eLog_SkillDiscard,
	eLog_SkillDestroyByWanted,
	eLog_SkillDestroyByBadFame,
	eLog_SkillLevelup,
	eLog_SkillCheatLevelup,
	eLog_SkillLevelDownByWanted,
	eLog_SkillLevelDownByBadFame,
	eLog_SkillDestroyByGetJulCho,
	eLog_SkillDestroyByBadFameJulChoDown,
	eLog_SkillDestroyByGetNextLevel,		// for japan

	////////////////////////////////////////////////////////////////////////////////
	// 06. 06. 2차 전직 - 이영준
	// 무공 변환
	eLog_SkillOption,
	eLog_SkillOptionClear,
	eLog_SkillDestroyByOptionClear,
	////////////////////////////////////////////////////////////////////////////////

	eLog_Skill_RM	= 100,
	eLog_SkillLearn_RM,
	eLog_SkillLevel_RM,
	eLog_SkillRemove_RM,

	eLog_SkillGetPoint,
	// 080819 LUJ, 캐쉬 스킬 로그
	eLog_SkillCashAdd,
	eLog_SkillCashRemove,
};


enum eFamilyLog
{
	// 포인트
	eLog_FamilyPoint	= 0,
	eLog_FamilyPointAdd,
	// 080610 LUJ, 포인트 변경
	eLog_FamilyPointUpdate,

	// 농장
	eLog_FamilyFarm		= 100,
	eLog_FamilyFarmPutCrop,		// 파종
	eLog_FamilyFarmDieCrop,		// 고사
	eLog_FamilyFarmGrowCrop,	// 생장
	eLog_FamilyFarmCareCrop,	// 비료
	eLog_FamilyFarmHarvestCrop,	// 수확
	eLog_FamilyFarmLostRight,	// 권리 상실(패밀리 탈퇴 등으로 인한 권리 상실)
	eLog_FamilyFarmDisband,		// 농장 해체(패밀리 해산으로 인한)
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2008.01.16
	eLog_FamilyFarmTax,			// 농장 해체(관리비 미납)
	eLog_FamilyFarmMystery,		// 농장 해체(농장 해체 시 로그를 추가하지 않았을 경우)
	// E 농장시스템 추가 added by hseos 2008.01.16
	// 080425 KTH -- 축사 관련 로그 추가 --------------
	eLog_FamilyFarmInstallAnimal,
	eLog_FamilyFarmDieAnimal,
	eLog_FamilyFarmFeedAnimal,
	eLog_FamilyFarmCleanAnimal,
	eLog_FamilyFarmAnimalReward,
	// 081205 LUJ, 패밀리 행동
	eLog_FamilyCreate = 200,
	eLog_FamilyAddMember,
	eLog_FamilyChangeNick,
	eLog_FamilyRegistEmblem,
	eLog_FamilyLeaveMember,
	eLog_FamilyExpelMember,
	eLog_FamilyBreakup,
	// 091111 ONS 패밀리 문장 삭제
	eLog_FamilyDelEmelem,
	// 091126 ONS 패밀리장 이양 기능 추가
	eLog_FamilyChangeMaster,
};

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2008.01.23
enum FARM_LOG_KIND																			// 농장 로그 종류
{
	FARM_LOG_KIND_BUY = 0,																	// ..구입
	FARM_LOG_KIND_UPGRADE,																	// ..업그레이드
	FARM_LOG_KIND_PAY_TAX,																	// ..관리비 납부
	FARM_LOG_KIND_DEL_FAMILY_BREAKUP,														// ..삭제 - 패밀리 해체
	FARM_LOG_KIND_DEL_TAX_ARREARAGE,														// ..삭제 - 관리비 체납
	FARM_LOG_KIND_MYSTERY = 9999,
};

enum FARM_UPGRADE_LOG_KIND																	// 농장 업그레이드 로그 종류
{
	FARM_UPGRADE_LOG_KIND_GARDEN = 0,														// ..텃밭
	FARM_UPGRADE_LOG_KIND_FENCE,															// ..울타리
	FARM_UPGRADE_LOG_KIND_HOUSE,															// ..집
	FARM_UPGRADE_LOG_KIND_WAREHOUSE,														// ..창고
	FARM_UPGRADE_LOG_KIND_ANIMALCAGE,														// ..축사
	FARM_UPGRADE_LOG_KIND_MYSTERY = 9999,
};

enum eLogitemmoney
{
	eLog_ItemNone		= 0,
	eLog_ItemDiscard	= 1,
	eLog_PetItemDiscard,
	
	//item move
	eLog_ItemMoveInvenToStorage = 100,
	eLog_ItemMoveStorageToInven,
	eLog_ItemMoveStorageToStorage,
	eLog_ItemMoveInvenToGuild,
	eLog_ItemMoveInvenToInven,
	eLog_ItemMoveGuildToInven,
	eLog_ItemMoveGuild,
	eLog_MoneyDepositToGuild,
	eLog_MoneyWithdrwaGuild,
	// 080716 LUJ, 펫<->플레이어 이동
	eLog_ItemMoveFromPlayerToPet,
	eLog_ItemMoveFromPetToPlayer,
	
	//item obtain/destory
	eLog_ItemObtainBuy = 200,
	eLog_ItemObtainDivide,
	eLog_ItemObtainCheat,
	eLog_ItemObtainMonster,
	eLog_ItemObtainQuest,
	eLog_ItemObtainPK,
	eLog_ItemObtainDissolve,
	eLog_ItemObtainMix,
	eLog_ItemObtainFromChangeItem,
	eLog_ItemRareObtainCheat,
	eLog_ItemOptionObtainCheat,
	eLog_ItemObtainMonstermeter,
	eLog_ItemObtainMarriage,
	eLog_ItemObtainFishing,
	eLog_ItemObtainGift,
	eLog_ItemObtainMerge,
	eLog_ItemObtainGTReward,	// 길드토너먼트 우승상품

	eLog_ItemObtainHouseRestore, // 하우스 아이템보관해제
	eLog_ItemObtainHouseBonus,	// 하우스 보너스(아이템획득)

	eLog_ItemSell = 300,
	eLog_ItemUse,
	
	eLog_StreetStallBuyAll = 400,
	eLog_StreetStallBuyDivide,
	eLog_StreetStallSellDivide,

	eLog_Exchange = 500,

	eLog_ItemReinforceSuccess = 600,
	eLog_ItemReinforceFail,
	eLog_ItemReinforceDataHaveTooManyMaterials,
	eLog_ItemReinforceSourceInvalid,
	eLog_ItemReinforceMaterialIsInvalid,
	eLog_ItemReinforceWhichMaterialHasZero,
	eLog_ItemReinfoceYouCannotOver100ToReinforce,
	eLog_ItemReinforceSourceItemNotExist,
	eLog_ItemRerinfoceServerHasNoReinforceScript,
	eLog_ItemReinforceMaterialSourceItemCannnotReinforce,
	eLog_ItemReinforceSourceItemCanDuplicate,
	eLog_ItemReinforceRemovingMaterialIsFailed,
	eLog_ItemReinforceUpdatingItemIsFailed,
	// 080929 LUJ, 강화 보조 아이템 오류 추가
	eLog_ItemReinforceInvalidSupportItem,
	eLog_ItemReinforceNoSupportScript,
	eLog_ItemReinforceInvalidMaterialForSupportItem,

	eLog_ItemMixSuccess = 700,
	eLog_ItemMixFail,
	eLog_ItemMixBigFail,
	eLog_ItemMixInsufficentSpace,
	eLog_ItemMixServerError,
	eLog_ItemMixUse,
	
	eLog_ItemDissolveSuccess,
	eLog_ItemDissolvePlayerNotExist,
	eLog_ItemDissolveSourceItemNotExist,
	eLog_ItemDissolveServerHaveNoDissolvingData,
	eLog_ItemDissolveItemDataNotMatched,
	eLog_ItemDissolveItNeedMoreLevel,
	eLog_ItemDissolveRemovingItemIsFailed,
	eLog_ItemDissolveAddingItemIsFailed,
	eLog_ItemDissolvePlayserHasNoInventory,
	eLog_ItemDissolveInsufficentSpace,
	eLog_ItemDissolveNoResult,
	eLog_ItemDissolveServerError,
	eLog_ItemDissolveScriptInvalid,
	eLog_ItemDissolveInsert,		// 빈 슬롯에 아이템을 넣음
	eLog_ItemDissolveAddDuplicated,	// 분해시 아이템이 든 슬롯에 겹쳐지는 아이템을 추가함

	eLog_ItemEnchantSuccess,
	eLog_ItemEnchantItemInvalid,
	eLog_ItemEnchantServerError,
	eLog_ItemEnchantHacking,
	eLog_ItemEnchantItemUpdateFailed,
	eLog_ItemEnchantUse,
	eLog_ItemEnchantDestroy,
	eLog_ItemEnchantNotEnoughMoney,	// 090713 ShinJS --- 인챈트시 소지금이 부족한 경우
	eLog_ItemEnchantLevelChanged,
	eLog_ItemEnchantProtected,

	eLog_ItemMixSystemFault,	// 조합 시 시스템 오류
	eLog_ItemMixNotFound,		// 조합할 대상을 찾지 못함
	eLog_itemMixBadRequest,		// 잘못된 요청
	eLog_ItemMixBadCondition,	// 조합할 조건이 되지 않음
	eLog_ItemMixBadQuantity,	// 개수가 잘못됨
		
	eLog_ItemDestroyMix = 800,
	eLog_ItemDestroyReinforce,
	eLog_ItemDestroyUpgrade,
	eLog_ItemDestroyByWanted, // eLog_ItemDestroyByBadFame¸| AIAEA¸·I ³≫·A¾ßAo
	eLog_ItemDestroyByBadFame, //die panelty with bad fame.
	eLog_ItemDestroyDissolution, // dissolve base item
	eLog_ItemDestroyGiveQuest, // by complete quest 
	eLog_ItemDestroyDeleteQuest,
	eLog_ItemDestroybyChangeItem,
	eLog_ItemDestroyReinforceWithShopItem,

	// 090207 LUJ, 합성 로그
	eLog_ItemComposeSuccess = 900,
	eLog_ItemComposeSuccessWithConsume,
	eLog_ItemComposeFailureByMemoryMismatch,
	eLog_ItemComposeFailureByWrongScript,
	eLog_ItemComposeFailureByWrongUseItemSize,
	eLog_ItemComposeFailureByInvalidResult,
	eLog_ItemComposeFailureByInvalidKeyItem,
	eLog_ItemComposeFailureByNoExistedOption,
	eLog_ItemComposeFailureByNoResultItemInfo,

	eLog_MoneyObtainPK = 1000,
	//exp obtain
	eLog_ExpObtainPK,

	eLog_ShopItemUse = 1500,	
	eLog_ShopItemUseEnd,
	eLog_ShopItemMoveToInven,
	eLog_ShopItemProtectAll,
	eLog_ShopItemSeal,
	eLog_ShopItemUnseal,
	eLog_ShopItemUnseal_Failed,
	eLog_ShopItem_ReinforceReset,
	eLog_ShopItemGetCheat,
	// 090625 pdy 펫 기간제 아이템 착용처리 추가 
	eLog_ShopItemUseEndFromPetInven,

	eLog_ShopItemUse_MixSuccess=1600,

	// 080602 LUJ, 스킬 사용 시 소모
	eLog_SkillItemUse	= 1700,
	eLog_SkillMoneyUse,
	
	eLog_Item = 2000,
	eLog_ItemOptionAdd_RM,
	eLog_ItemOptionRemove_RM,
	eLog_ItemAdd,
	eLog_ItemRemove,
	eLog_ItemRemoveMarriageDivorce,
	eLog_ItemDropOptionAdd,
		
	eLog_ItemMoveInvenToPetInven = 2100,
	eLog_ItemMovePetInvenToInven,

	eLog_ItemSkillLearn	= 2200,

	eLog_GTournamentReg = 2300,		// 길드토너먼트 등록비
	// 081205 LUJ, 농장
	eLog_FarmTaxPaid = 2400,

	eLog_ItemCookUse,
	eLog_ItemCookSuccess,
	eLog_ItemVehicleToll,
	eLog_ItemHouseKeep,
	eLog_ItemHouseUnKepp,
	eLog_ItemUseHouseItem,

	// 090227 ShinJS --- Coin Item의 수량 변경/삭제
	eLog_CoinDecreaseByItemBuy,					// Coin 변경사항(Update시)
	eLog_CoinDestroyByItemBuy,					// Coin 변경사항(Delete시)
	// 트리거로 인한 아이템 생성/추가
	eLog_AddItemByTrigger,
	eLog_RemoveItemByTrigger,
	eLog_ItemChangeClass,


	// 판매대행
	eLog_Consignment_Regist = 2412,			// 판매대행등록
	// 우편함
	eLog_Note_Consignmnet_Buy,			// 판매대행구매
	eLog_Note_Consignmnet_SellUpdate,	// 판매대행갱신 (부분판매시 원본아이템의 수량변경)
	eLog_Note_Consignmnet_Receipt,		// 판매대행영수증
	eLog_Note_Consignmnet_UserCancel,	// 판매대행취소(유저)
	eLog_Note_Consignmnet_TimeCancel,	// 판매대행취소(기간)
	eLog_Note_CombineDelete,			// 우편받기삭제(수량성 기존아이템에 합쳐져서 제거함)
	eLog_Note_Obtain_Success,			// 우편받기지급(아이템)
	eLog_Note_Obtain_Money,				// 우편받기지급(골드)
	eLog_Note_UserDelete,				// 우편유저삭제
	eLog_Note_TimeDelete,				// 우편기간삭제
	eLog_ItemVehicleSummon,
	eLog_ItemVehicleUnsummon,
	eLog_DungeonBegin,
	eLog_DungeonEnd,
	eLog_LimitDungeonBegin,
	eLog_LimitDungeonEnd,
	eLog_DateMatchBegin,
	eLog_DateMatchEnd,
	eLog_Note_Obtain_Fail,				// 우편받기실패
	eLog_Note_SendItemFromNPC,			// NPC가 우편아이템발송

	eLog_ItemUse_GTBUFFITEM,			// 스킬이 시전되는 아이템
	eLog_Max,
};


enum eGuildLog
{
	// member
	eGuildLog_MemberSecede = 1,
	eGuildLog_MemberBan,
	eGuildLog_MemberAdd,
	eGuildLog_StudentAdd,

	// master 
	eGuildLog_MasterChangeRank = 100,

	// guild
	eLogGuild_GuildCreate = 200,
	eGuildLog_GuildBreakUp,
	eGuildLog_GuildLevelUp,
	eGuildLog_GuildLevelDown,
	eGuildLog_Retire,
	eGuildLog_SkillAdd,

	// guildunion
	eGuildLog_CreateUnion = 400,
	eGuildLog_DestroyUnion,
	eGuildLog_AddUnion,
	eGuildLog_RemoveUnion,	
	eGuildLog_SecedeUnion,

	// 점수
	eGuildLog_Score	= 500,
	eGuildLog_TimeScoreAdd,
	eGuildLog_ScoreAddByKill,		// 길드전 상대 제거
	eGuildLog_ScoreRemoveByMurder,	// 길드전에서 사망
	eGuildLog_ScoreAddByWin,		// 길드전 승리
	eGuildLog_ScoreRemoveByDefeat,	// 길드전 패배
	eGuildLog_ScoreCheat,			// 치트
	eGuildLog_ScoreLevelUp,			// 레벨업에 사용
	eGuildLog_ScoreSkillUse,		// 080602 LUJ, 스킬 발동에 소모
	eGuildLog_ScoreRemoveByGTFee,	// 길드토너먼트 등록소모
	eGuildLog_ScoreAddByGTWin,		// 길드토너먼트 승리보상
	eGuildLog_ScoreAddByItem,		// 길드포인트획득 아이템

	// gm 툴
	eGuildLog_GM	= 600,
	eGuildLog_GM_ChangeRank,
	eGuildLog_GM_KIck_Member,
	// 081027 LUJ, 길드 토너먼트 로그
	eGuildLog_TournamentAdd,
};


enum eQuestLog
{
	eQuestLog_None	= 0,

	eQuestLog_GM	= 1,

	eQuestLog_User	= 100,
	eQuestLog_Main_Begin,
	eQuestLog_Sub_Begin,
	eQuestLog_Main_End,
	eQuestLog_Sub_End,
	eQuestLog_Main_Delete,
	eQuestLog_Sub_Delete,
	eQuestLOg_ItemTake,
	eQuestLOg_ItemGive,
	eQuestLOg_ItemUpdate,
};


enum eLogExppoint
{
	eExpLog_Time,
		
	eExpLog_LevelUp = 10,
	// 081027 LUJ, 경험치 획득
	eExpLog_Get,

	eExpLog_LosebyBadFame = 50, //reduce 
	eExpLog_LosebyReviveLogIn,
	eExpLog_LosebyRevivePresent,
	eExpLog_LosebyLooting,
	eExpLog_LosebyBreakupGuild,
	eExpLog_LosebyReviveExpProtect,
	eExpLog_LosebyReviveReviveOther,
	eExpLog_LosebyGuildFieldWar,
	eExpLog_LosebyBreakupFamily,
	// desc_hseos_패밀리01
	// S 패밀리 추가 added by hseos 2007.10.11
	eExpLog_LosebyLeaveFamily,
	eExpLog_LosebyExpleFamilyMember,
	// E 패밀리 추가 added by hseos 2007.10.11
	eExpLog_LoseByPKMode,
	// 080602 LUJ, 스킬 사용으로 인한 경험치 소모
	eExpLog_LoseBySkillUse,

	eExpLog_GetbyQuest = 100,
	eExpLog_SExpGetbyQuest = 101,
	eExpLog_GetbyItem = 102,
	
	eExpLog_ProtectExp = 150,
	eExpLog_ReviveExp,
};

enum eLogToolType	// Tool A¾·u
{
	eToolLog_GM = 1,
	eToolLog_RM,
	eToolLog_Server,
};

enum eGMLogKind		// GMTool ·I±× A¾·u
{
	eGMLog_LogIn = 1,
	eGMLog_Move,	
	eGMLog_Item,
	eGMLog_Money,
	eGMLog_Summon,
	eGMLog_SummonDelete,
	eGMLog_MonsterKill,
	eGMLog_PKAllow,
	eGMLog_Disconnect_Map,
	eGMLog_Disconnect_User,
	eGMLog_Block,
	eGMLog_Event,
	eGMLog_PlusTime,

	eGMLog_Skill,
	eGMLog_Ability,
	eGMLog_Chat_Map,
	eGMLog_Chat_User,
	eGMLog_PK,
	eGMLog_Regen,
};

enum eFishingLog
{
	eFishingLog_SetPointFromCheat = 1,	// 치트로부터 물고기포인트 설정
	eFishingLog_GetPointFromFish,		// 물고기로부터 물고기포인트 획득
	eFishingLog_UsePointForBuyItem,		// 아이템을 사기위해 물고기포인트 사용
	eFishingLog_SetLevel,				// 080808 LUJ, 낚시 레벨 변경
	eFishingLog_Regular,				// 080808 LUJ, 정기 로그 저장
	eFishingLog_GetExpFromItem,
	eFishingLog_GetPointFromItem,
};

enum eCookingLog
{
	eCookingLog_CookCount = 1,			// 요리하기
	eCookingLog_EatCount,				// 요리먹기
	eCookingLog_FireCount,				// 모닥불 피우기

	eCookingLog_RecipeReg,				// 레시피 등록
	eCookingLog_RecipeDel_by_Time,		// 레시피 삭제 (시간종료 자동삭제)
	eCookingLog_RecipeDel_by_User,		// 레시피 삭제 (유저 직접삭제)
};

enum eHousingLog
{
	eHousingLog_Keep = 1,			// 보관
	eHougingLog_UnKeep,				// 보관해제
	eHougingLog_Install,			// 설치
	eHougingLog_UnInstall,			// 설치해제
	eHougingLog_Destroy,			// 파괴
	eHougingLog_EndTime,			// 사용시간종료

	eHougingLog_WallPaper,			// 벽지사용
	eHougingLog_FloorPaper,			// 바닥지사용
	eHougingLog_CeilingPaper,		// 천장지사용
	eHougingLog_Extend,				// 확장
	eHougingLog_Visit,				// 방문
	eHougingLog_Exterior,			// 외형사용
};

// 081028 LUJ, 공성 로그. 이후 
enum eSiegeLog
{
	eSiegeLogNone,
	eSiegeLogChangeState,
	eSiegeLogDestoryCatapult,
	eSiegeLogGetCastle,
	eSiegeLogUseSeed,
	eSiegeLogSetSeedSucceed,
	eSiegeLogSetSeedFailed,
	eSiegeLogComplete,
};


// 080716 LUJ, 펫 로그
enum ePetLog
{
	ePetLogSummon,
	ePetLogDie,
	ePetLogRevive,
	ePetLogChangeGrade,
	ePetLogChangeLevel,
	ePetLogRegular,		// 080716 LUJ, 일정 간격으로 상태 저장
};


// 길드토너먼트 로그
enum eGTLogKind
{
	eGTLogKind_None=0,
	eGTLogKind_Regist,				// 등록
	eGTLogKind_Win,					// 승
	eGTLogKind_Lose,				// 패
};


// 공성전 로그
enum eSWLogKind
{
	eSWLogKind_None=0,
	eSWLogKind_DefenceProposal,			// 수성참여 신청
	eSWLogKind_Attack,					// 공성문파
	eSWLogKind_Defence,					// 수성측 문파
	eSWLogKind_DefenceAccept,			// 수성참여 허락으로 수성문파 참여
	eSWLogKind_DefenceUnion,			// 동맹결성으로 수성문파 참여
	eSWLogKind_CancelByCastleGuild,		// 성문파가 동맹탈퇴로 다른성문파의 수성참여 취소
	eSWLogKind_CancelByUnionSecession,	// 성문파와의 동맹 탈퇴로 수성참여 취소
	eSWLogKind_StartSiegeWar,			// 공성시작
	eSWLogKind_SucceedCastleGuild,		// 공성성공으로 성을 차지한 문파
	eSWLogKind_SucceedMoveToDefence,	// 공성성공으로 성을 차지한 문파의 동맹은 수성으로 이동
	eSWLogKind_SucceedAttack,			// 공성성공 공성은 공성 그대로인 문파
	eSWLogKind_SucceedMoveToAttack,		// 공성측의 공성성공으로 수성->공성으로 옮겨간 문파
	eSWLogKind_EndCastleGuild,			// 공성종료후 성을 차지한 문파
	eSWLogKind_EndDefenceUnionGuild,	// 공성종료후 동맹이어서 수성으로 등록되는 문파
	eSWLogKind_EngraveSyn,				// 각인시도
	eSWLogKind_EngraveNack,				// 각인실패 맞아 죽음
	eSWLogKind_EngraveNackCancel,		// 각인실패 취소
	eSWLogKind_EngraveNackLogOut,		// 각인실패 로그아웃
	eSWLogKind_SiegeWarSucceed,			// 각인성공
	eSWLogKind_EndSiegeWar,				// 공성종료
};

// 090609 ShinJS --- AutoNote Log 추가
enum eAutoNoteLogKind
{
	eAutoNoteLogKind_None,
	eAutoNoteLogKind_Report,			// 신고
	eAutoNoteLogKind_Regist,			// Auto 등록
	eAutoNoteLogKind_Ban,				// Ban 등록
};

#define MAX_KIND_PERMAP		150

enum
{
	ePreLoad_Monster,
	ePreLoad_Item,

	ePreLoad_Max,
};


struct sPRELOAD_INFO
{
	int		MapNum;
	int		Count[ePreLoad_Max];
	int		Kind[ePreLoad_Max][MAX_KIND_PERMAP];

	sPRELOAD_INFO()
	{
		MapNum		= 0;
		memset(Count, 0, sizeof(int)*ePreLoad_Max);
		memset(Kind, 0, sizeof(int)*ePreLoad_Max*MAX_KIND_PERMAP);
	}
};

#define MAX_GUILD_NAME 16
#define MAX_GUILD_INTRO		60
// 06. 03. 문파공지 - 이영준
#define MAX_GUILD_NOTICE	150
#define MAX_GUILD_UNION		7
#define MAX_GUILD_NICKNAME	16
#define GUILD_MASTER		60
#define GUILD_VICEMASTER	50
#define GUILD_SENIOR		40
#define GUILD_JUNIOR		30
#define GUILD_MEMBER		20
#define GUILD_STUDENT		10
#define GUILD_NOTMEMBER		0

#define GUILD_STUDENT_MAX_LEVEL	40
#define MAX_GUILD_MEMBER	80 //60 + 20(수련생)
#define MAX_SHOUT_LENGTH 60

enum
{
	eMode_NormalNote = 0,
	eMode_StoredNote,
};

#define MAXGUILD_INTOURNAMENT			32		//길드토너먼트 최대참가팀
#define MAX_GTOURNAMENT_PLAYER			14		//길드토너먼트 참가선수
#define GT_INVALID_BATTLEID				99999	//길드토너먼트 배틀ID가 없을경우

enum
{
	eGTError_NOERROR=0,					// No Error
	eGTError_NOTREGISTDAY,				// Not a Regist-day
	eGTError_NOTREGISTCANCELDAY,		// Not a Registcancel-day
	eGTError_DONTFINDGUILDINFO,			// Don't find a GuildInfo
	eGTError_MAXGUILDOVER,				// Over MAXGUILD_INTOURNAMENT
	eGTError_ALREADYREGISTE,			// Already Registed GuildTournament
	eGTError_ALREADYCANCELREGISTE,		// Already Cancel Registed GuildTournament
	eGTError_UNDERLIMITEMEMBER,			// Under LIMITMEMBER_GUILDTOURNAMENT
	eGTError_UNDERLEVEL,				// Under LIMITLEVEL_GUILDTOURNAMENT
	eGTError_DELETEAPPLIED,				// CancelError Regited GuildTournament
	eGTError_NOGUILDMASTER,				// Not a GuildMaster
	eGTError_DONTFINDPOSITION,			// GuildTournament-Position Find Error
	eGTError_DONTFINDBATTLE,			// Don't find a Battle	
	eGTError_BATTLEJOINFAIL,			// Failed a join in Battle
	eGTError_NOTENTERTIME,				// Not a time of enter the battle;
	eGTError_STUDENTCANTENTER,			// a guild student can't enter the battle
	eGTError_FAILTOREGIST,				// Fail to Regist
	eGTError_INVALIDROUND,
	eGTError_ALREADYSETMATCH,
	eGTError_NOTALLOWOBSERVER,
	eGTError_INVALIDDAY,
	eGTError_READYTONEXTMATCH,
	eGTError_NOTENOUGHFEE,
	eGTError_ERROR,
};

enum
{
	eGTState_BeforeRegist=0,
	eGTState_Regist,
	eGTState_BeforeEntrance,
	eGTState_Entrance,
	eGTState_Process,
	eGTState_Leave,

	eGTState_AutoSchedule,
	
	eGTState_Max,
};

enum
{
	eGTStanding_1ST = 1,
	eGTStanding_2ND = 2,
	eGTStanding_3RD = 3,
	eGTStanding_4TH = 4,
	eGTStanding_8TH = 8,
	eGTStanding_16TH = 16,
	eGTStanding_32TH = 32,
};

enum
{
	eGTFight_32		= 0,
	eGTFight_16		= 1,
	eGTFight_8		= 2,
	eGTFight_4		= 3,
	eGTFight_2		= 4,
	eGTFight_1		= 5,
	eGTFight_End,

	eGTT_MAX,
};

enum
{
	eGTNotify_RegistStart = 0,
	eGTNotify_RegistEnd,
	eGTNotify_TournamentStart,
	eGTNotify_TournamentEnd,
	eGTNotify_RoundStartforPlayer,
	eGTNotify_RoundStart,
	eGTNotify_RoundEnd,
	
	eGTNotify_EnteranceRemain,
	eGTNotify_RegistRemain,
	eGTNotify_BattleTableOpen,

	eGTNotify_WinLose,

	eGTNotify_Max,
};

enum
{
	eGTDay_SUNDAY=0,
	eGTDay_MONDAY,
	eGTDay_TUESDAY,
	eGTDay_WEDNESDAY,
	eGTDay_THURSDAY,
	eGTDay_FRIDAY,
	eGTDay_SATURDAY,
};

enum
{
	eGTWin_None = 0,
	eGTWin_Unearned,		// 부전승
	eGTWin_RemainPlayer,	// 남은사람수
	eGTWin_LowLevel,		// 레벨합이 낮다
	eGTWin_LowExp,			// 경험치가 낮다.
};

enum
{
	eNpcParam_ShowpStorage=1,
};

#define ITEM_PARAM_PRESENT	0x00000100

enum eITEM_PARAM
{
	eITEM_PARAM_NORMAL,
	eITEM_PARAM_SEAL,
	eITEM_PARAM_UNSEAL,
};

// 070414 LYW --- CommonGameDefine : Modified eCharacterStage.
enum eCharacterStage
{
	eStage_H_Fighter			= 1111,	// 파이터
	eStage_H_Guard				= 1121,	// 가드
	eStage_H_Warrior			= 1122,	// 전사
	eStage_H_InfantryMan		= 1131,	// 보병
	eStage_H_SwordMan			= 1132,	// 소드맨
	eStage_H_Mercenary			= 1133,	// 용병
	eStage_H_Phalanx			= 1141, // 팔랑크스
	eStage_H_Knight				= 1142, // 기사
	eStage_H_Gladiator			= 1143, // 검투사
	eStage_H_RunicKnight		= 1144, // 룬 나이트
	eStage_H_Paladin			= 1151, // 팰러딘
	eStage_H_Panzer				= 1152, // 기갑부대원
	eStage_H_Crusader			= 1153, // 십자군전사
	eStage_H_Destroyer			= 1154, // 파괴자
	eStage_H_SwordMaster		= 1155, // 소드마스터
	eStage_H_Magners			= 1156, // 매그너스
	eStage_H_Lord				= 1161, // 로드
	eStage_H_DeathKNight		= 1162, // 데스나이트
	eStage_H_ArcTemplar			= 1163, // 아크템플러

	eStage_E_Fighter			= 1211,	// 파이터
	eStage_E_Guard				= 1221,	// 가드
	eStage_E_Warrior			= 1222,	// 전사
	eStage_E_InfantryMan		= 1231,	// 보병
	eStage_E_SwordMan			= 1232,	// 소드맨
	eStage_E_Mercenary			= 1233,	// 용병
	eStage_E_Phalanx			= 1241, // 팔랑크스
	eStage_E_Knight				= 1242, // 기사
	eStage_E_Gladiator			= 1243, // 검투사
	eStage_E_RunicKnight		= 1244, // 룬 나이트
	eStage_E_Paladin			= 1251, // 팰러딘
	eStage_E_Panzer				= 1252, // 기갑부대원
	eStage_E_Crusader			= 1253, // 십자군전사
	eStage_E_Destroyer			= 1254, // 파괴자
	eStage_E_SwordMaster		= 1255, // 소드마스터
	eStage_E_Magners			= 1256, // 매그너스
	eStage_E_Lord				= 1261, // 로드
	eStage_E_DeathKNight		= 1262, // 데스나이트
	eStage_E_ArcTemplar			= 1263, // 아크템플러
	

	eStage_H_Rogue				= 2111, // 방랑자
	eStage_H_Voyager			= 2121, // 항해자
	eStage_H_Ruffian			= 2122, // 무법자
	eStage_H_Archer				= 2131, // 궁수
	eStage_H_Chief				= 2132, // 시프
	eStage_H_Scout				= 2133, // 정찰병
	eStage_H_Ranger				= 2141, // 부대원
	eStage_H_TreasureHunter 	= 2142, // 보물사냥꾼
	eStage_H_Assassin			= 2143, // 암살자
	eStage_H_RunicWorker		= 2144, // 룬워커
	eStage_H_Sniper				= 2151, // 저격병
	eStage_H_Intraper			= 2152, // 
	eStage_H_BladeTaker			= 2153, //
	eStage_H_TemperMaster		= 2154, //

	// 080709 LYW --- CommonGameDefine : 아크레인저 직업을 추가한다.
	eState_H_ArcRanger			= 2155,	// 아크레인저

	eStage_H_SoulEye			= 2161, //
	eStage_H_BloodStalker		= 2162, //
	eStage_H_ArcBridger			= 2163, //

	eStage_E_Rogue				= 2211, // 방랑자
	eStage_E_Voyager			= 2221, // 항해자
	eStage_E_Ruffian			= 2222, // 무법자
	eStage_E_Archer				= 2231, // 궁수
	eStage_E_Chief				= 2232, // 시프
	eStage_E_Scout				= 2233, // 정찰병
	eStage_E_Ranger				= 2241, // 부대원
	eStage_E_TreasureHunter 	= 2242, // 보물사냥꾼
	eStage_E_Assassin			= 2243, // 암살자
	eStage_E_RunicWorker		= 2244, // 룬워커
	eStage_E_Sniper				= 2251, // 저격병		
	eStage_E_Intraper			= 2252, // 
	eStage_E_BladeTaker			= 2253, //
	eStage_E_TemperMaster		= 2254, //

	// 080709 LYW --- CommonGameDefine : 아크레인저 직업을 추가한다.
	eState_E_ArcRanger			= 2255,	// 아크레인저

	eStage_E_SoulEye			= 2261, //
	eStage_E_BloodStalker		= 2262, //
	eStage_E_ArcBridger			= 2263, //

	eStage_H_Mage				= 3111, //
	eStage_H_Cleric				= 3121, //
	eStage_H_Wiard				= 3122, //
	eStage_H_Priest				= 3131, //
	eStage_H_Sorcerer			= 3132, //
	eStage_H_Monk				= 3133, //
	eStage_H_Bishop				= 3141, //
	eStage_H_Warlock			= 3142, //
	eStage_H_Inquirer			= 3143, //
	eStage_H_ElementalMaster	= 3144, //
	eStage_H_Cardinal			= 3151, //
	eStage_H_SoulArbiter		= 3152, //
	eStage_H_GrandMaster		= 3153, //
	eStage_H_Necromancer		= 3154, //
	eStage_H_RunicMaster		= 3155, //
	eStage_H_Saint				= 3161, //
	eStage_H_DarkArchon			= 3162, //
	eStage_H_ArcCelebrant		= 3163, //

	eStage_E_Mage				= 3211, //
	eStage_E_Cleric				= 3221, //
	eStage_E_Wiard				= 3222, //
	eStage_E_Priest				= 3231, //
	eStage_E_Sorcerer			= 3232, //
	eStage_E_Monk				= 3233, //
	eStage_E_Bishop				= 3241, //
	eStage_E_Warlock			= 3242, //
	eStage_E_Inquirer			= 3243, //
	eStage_E_ElementalMaster	= 3244, //
	eStage_E_Cardinal			= 3251, //
	eStage_E_SoulArbiter		= 3252, //
	eStage_E_GrandMaster		= 3253, //
	eStage_E_Necromancer		= 3254, //
	eStage_E_RunicMaster		= 3255, //
	eStage_E_Saint				= 3261, //
	eStage_E_DarkArchon			= 3262, //
	eStage_E_ArcCelebrant		= 3263, //

};

enum eGuildUnion_Err
{
	eGU_Not_ValidName,
	eGU_Not_InGuild,
	eGU_Not_GuildMaster,
	eGU_Not_Level,
	eGU_Not_GuildUnionMaster,
	eGU_Not_InGuildUnion,
	eGU_Aleady_InGuildUnion,
	eGU_Time,
	eGU_Other_Not_InGuild,
	eGU_Other_Not_InGuildUnion,
	eGU_No_Lone_Union,
	eGU_Other_Not_GuildMaster,
	eGU_Other_Aleady_InGuildUnion,
	eGU_Other_Time,
	eGU_Is_Full,	
	eGU_Is_GuildFieldWar,
	eGU_Not_ValidMap,
};

// 기상상태
enum eWeatherState
{
	//eWS_None,	// 0
	//eWS_Snow,	// 1
	eWS_Clear,	// 맑음
	eWS_Cloudy,	// 흐림
	eWS_Rainy,	// 비
	eWS_Windy,	// 바람
	eWS_Foggy,	// 안개
	eWS_Typoon,	// 태풍

	eWS_Max,	
};

// 080410 NYJ --- 낚시관련 에러코드 
enum eFishingErrorCode
{
	eFishingError_None = 0,		// NotError, Success
	eFishingError_InvaildHero,	// 유효하지 않은 플레이어
	eFishingError_InvaildPlace,	// 유효하지 않은 낚시터
	eFishingError_NoPole,		// 낚시대 미착용
	eFishingError_NoBait,		// 미끼 미사용
	eFishingError_OverTime,		// 시간초과
	eFishingError_UserCancel,	// 유저의 취소요청
	eFishingError_InvenFull,	// 인벤이 가득찬상태
	eFishingError_InvalidGrade,	// 숙련등급 불일치
	eFishingError_EarlyTime,	// 시간미충족
	eFishingError_InvaildState,	// 유효하지 않은 상태
	eFishingError_OverMaxFishPoint,	// 최대 물고기포인트 초과
	eFishingError_FailFishPoint,// 물고기포인트변환 실패
	eFishingError_OverDistance,	// 거리초과
};

// 080414 NYJ --- 낚시관련 물고기아이템타입
enum eFishItemType
{
	eFishItem_Poor = 0,		// [0]피라미
	eFishItem_Common,		// [1]붕어
	eFishItem_UnCommon,		// [2]잉어
	eFishItem_Rare,			// [3]황금잉어, 황금자라
	eFishItem_Unique,		// [4]빛나는 쓰레기
	eFishItem_Event,		// [5]친환경미끼
	eFishItem_Reserve1,		// [6]예약1
	eFishItem_Reserve2,		// [7]예약1
	eFishItem_Reserve3,		// [8]예약1
	eFishItem_Reserve4,		// [9]예약1
	
	eFishItem_Max,
};

enum eCookErrorCode
{
	eCookError_None = 0,		// NotError, Success
	eCookError_InvaildRecipe,	// 유효하지 않은 레시피
	eCookError_InvaildFire,		// 유효하지 않은 모닥불
	eCookError_InvaildMakeNum,	// 유효하지 않은 제작수량
	eCookError_InvaildUtil,		// 유효하지 않은 요리도구
	eCookError_LowerUtil,		// 레시피보다 등급이 낮은 요리도구
	eCookError_LowerExpertPoint,// 레시피보다 점수가 낮은 숙련도
	eCookError_InvalidGrade,	// 숙련등급 불일치
	eCookError_InvaildState,	// 유효하지 않은 상태
	eCookError_OverDistance,	// 거리초과
	eCookError_InvenFull,		// 인벤이 가득찬상태
	eCookError_NeedIngredient,	// 재료부족
};

enum PART_TYPE
{
	ePartType_None,
	ePartType_OneHand,
	ePartType_TwoHand,
	ePartType_TwoBlade,
	ePartType_Shield,

	ePartType_Body,
	ePartType_Hand = 7,
	ePartType_Foot,

	ePartType_Hair,
	ePartType_Face,
	ePartType_Head,

	ePartType_Helmet = 15,
	ePartType_Glasses,
	ePartType_Mask,

	ePartType_Head_Chicken = 20,
	ePartType_Head_Panda,
	ePartType_Head_Tights,
	ePartType_Head_Bag,
	ePartType_Head_Mask,
	//090506 pdy 파츠 추가 
	ePartType_Head_CatEar,		

	ePartType_Costume_Body = 30,
	ePartType_Costume_Foot,
	ePartType_Costume_Hand,

	ePartType_Wing = 40,
	ePartType_Wing2,

	//100406 pdy 파츠추가 : Bip01 Pelvis에 Attach
	ePartType_Pelvis,

	//090506 pdy 파츠추가 : Bip01에 Attach
	ePartType_Tail	= 50, 


	ePartType_Pet_Face = 70,
	ePartType_Pet_Head,
	ePartType_Pet_Wing,
	ePartType_Pet_Tail,
	
	ePartType_Job_Effect = 100,

	ePartType_Max,
};

enum	DATE_MATCHING_REGION				// 데이트 매칭 시스템에서 사는 지역 선언. 로컬라이징 하려면 파일형식으로 밖으로 빼야할 듯.?
{
	eDATE_MATCHING_REGION_KYUNGKI,			// 경기,
	eDATE_MATCHING_REGION_SEOUL,			// 서울
	eDATE_MATCHING_REGION_INCHON,			// 인천
	eDATE_MATCHING_REGION_KANGWON,			// 강원
	eDATE_MATCHING_REGION_CHUNGNAM,			// 충남
	eDATE_MATCHING_REGION_CHUNGBUK,			// 충북
	eDATE_MATCHING_REGION_DAEJEON,			// 대전
	eDATE_MATCHING_REGION_KYUNGBUK,			// 경북
	eDATE_MATCHING_REGION_KYUNGNAM,			// 경남
	eDATE_MATCHING_REGION_DAEGU,			// 대구
	eDATE_MATCHING_REGION_BUSAN,			// 부산
	eDATE_MATCHING_REGION_ULSAN,			// 울산
	eDATE_MATCHING_REGION_JEONBUK,			// 전북
	eDATE_MATCHING_REGION_JEONNAM,			// 전남
	eDATE_MATCHING_REGION_KWANGJU,			// 광주
	eDATE_MATCHING_REGION_JEJU,				// 제주
	eDATE_MATCHING_REGION_OVERSEAS,			// 해외
};

enum	DATE_MATCHING_FEELING				// 데이트 매칭 시스템의 성향.
{
	eDATE_MATCHING_FEELING_BLACKHAIR,		// 검은 머리
	eDATE_MATCHING_FEELING_DYEDHAIR,			// 염색한 머리
	eDATE_MATCHING_FEELING_FORMALDRESS,		// 정장
	eDATE_MATCHING_FEELING_CASUALWEAR,		// 캐주얼 복장
	eDATE_MATCHING_FEELING_MAKEUP,			// 화장
	eDATE_MATCHING_FEELING_PERFUME,			// 향수
	eDATE_MATCHING_FEELING_THIN,				// 마름
	eDATE_MATCHING_FEELING_FAT,				// 뚱뚱함
	eDATE_MATCHING_FEELING_BEARD,			// 수염
	eDATE_MATCHING_FEELING_EYEGLASS,			// 안경
	eDATE_MATCHING_FEELING_HAT,				// 모자
	eDATE_MATCHING_FEELING_ACCESSORIES,		// 액세서리
};

enum eStatusKind
{
	eStatusKind_None,
	eStatusKind_Str,
	eStatusKind_Dex,
	eStatusKind_Vit,
	eStatusKind_Int,
	eStatusKind_Wis,
	eStatusKind_All,
	eStatusKind_PhysicAttack = 11,
	eStatusKind_PhysicDefense,
	eStatusKind_MagicAttack,
	eStatusKind_MagicDefense,
	eStatusKind_Accuracy,
	eStatusKind_Avoid,
	eStatusKind_CriticalRate,
	eStatusKind_Range,
	eStatusKind_MagicCriticalRate,
	eStatusKind_CriticalDamage = 22,
	eStatusKind_MoveSpeed,
	eStatusKind_Block,
	eStatusKind_CoolTime,
	eStatusKind_CastingProtect,

	eStatusKind_Hide,
	eStatusKind_Detect,

	eStatusKind_IncreaseDropRate = 29,
	eStatusKind_DecreaseManaRate = 30,
	eStatusKind_MaxLife = 31,
	eStatusKind_MaxMana,
	eStatusKind_LifeRecoverRate,
	eStatusKind_ManaRecoverRate,
	eStatusKind_LifeRecover,
	eStatusKind_ManaRecover,
	eStatusKind_Poison = 40,
	eStatusKind_Paralysis,
	eStatusKind_Stun,
	eStatusKind_Slip,
	eStatusKind_Freezing,
	eStatusKind_Stone,
	eStatusKind_Silence,
	eStatusKind_BlockAttack,
	eStatusKind_God,
	eStatusKind_Shield,
	eStatusKind_Bleeding,
	eStatusKind_Burning,
	eStatusKind_HolyDamage,
	eStatusKind_MoveStop,
	eStatusKind_Attract,
	// 090204 LUJ, 피격 시 MP를 감소시킨다
	eStatusKind_ManaBurn,

	eStatusKind_Reflect = 61,
	eStatusKind_Absorb,
	eStatusKind_DamageToLife,
	eStatusKind_DamageToMana,
	eStatusKind_GetLife,
	eStatusKind_GetMana,
	eStatusKind_GetExp,
	eStatusKind_GetGold,
	// 071217 KTH --- 경험치 보호
	eStatusKind_ProtectExp = 70,
	eStatusKind_Attrib_None = 71,
	eStatusKind_Attrib_Earth,
	eStatusKind_Attrib_Water,
	eStatusKind_Attrib_Divine,
	eStatusKind_Attrib_Wind,
	eStatusKind_Attrib_Fire,
	eStatusKind_Attrib_Dark,
	eStatusKind_Sword = 81,
	eStatusKind_Mace,
	eStatusKind_Axe,
	eStatusKind_Staff,
	eStatusKind_Bow,
	eStatusKind_Gun,
	eStatusKind_Dagger,
	eStatusKind_Spear,
	eStatusKind_TwoHanded,
	eStatusKind_TwoBlade,
	eStatusKind_OneHanded,

	eStatusKind_RobeArmor,
	eStatusKind_LightArmor,
	eStatusKind_HeavyArmor,
	eStatusKind_ShieldArmor,
	eStatusKind_BothDagger,
	eStatusKind_FishingPole,
	eStatusKind_CookUtil,
	
	eStatusKind_NormalSpeedRate = 100,
	eStatusKind_PhysicSkillSpeedRate,
	eStatusKind_MagicSkillSpeedRate,	
	eStatusKind_NormalPhysicSkillSpeedRate = 105,
	// 080616 LUJ, 5차 스킬 추가
	eStatusKind_ConvertHealthWithGivedDamage	= 106,
	eStatusKind_ConvertManaWithGivedDamage		= 107,
	eStatusKind_EventSkillToVictim				= 108,
	eStatusKind_EventSkillToKiller				= 109,
	// 080703 LUJ, 이도류 사용 가능
	eStatusKind_EnableTwoBlade					= 110,
	// 081203 LUJ, 아이템 사용 가능 여부
	eStatusKind_UnableUseItem					= 111,
	// 081203 LUJ, 버프 사용 가능 여부
	eStatusKind_UnableBuff						= 112,
	// 071204 KTH -- 경험치 증가 %
	eStatusKind_IncreaseExpRate = 120,
	// 100221 ShinJS --- 데미지를 마나 데미지로 전환시킨다
	eStatusKind_DamageToManaDamage = 121,

	// Push 기능추가
	eStatusKind_Push_Target,
	eStatusKind_Push_Operator,
	eStatusKind_PushPull_Target,
	eStatusKind_PushPull_Operator,

	eStatusKind_Max,
};

enum eSkillTargetKind
{
	eSkillTargetKind_None,
	eSkillTargetKind_OnlyOthers,
	eSkillTargetKind_Ground,
	eSkillTargetKind_OnlySelf,
	eSkillTargetKind_AllObject,
};

// 080602 LUJ, 스킬 타겟 대상. Target.h에서 옮겨옴
enum TARGET_KIND
{
	TARGET_KIND_SINGLE,
	TARGET_KIND_MULTI,
	TARGET_KIND_SINGLE_CIRCLE,
	TARGET_KIND_MULTI_CIRCLE,
	TARGET_KIND_MULTI_RECT,
	TARGET_KIND_MULTI_ARC,
	// 080602 LUJ, 세계 범위 스킬
	TARGET_KIND_WORLD,
	// 080708 LUJ, 몬스터 대상 범위 스킬
	TARGET_KIND_MULTI_MONSTER,
	// 090204 LUJ, 전체(몬스터/플레이어) 대상 범위 스킬
	TARGET_KIND_MULTI_ALL,
	TARGET_KIND_MAX,
};

// 080602 LUJ, 스킬 범위 유형 타겟. Target.h에서 옮겨옴
enum eSkillAreaTarget
{
	eSkillAreaTargetNone,
	eSkillAreaTargetMonster,
	eSkillAreaTargetParty,
	eSkillAreaTargetFamily,
	eSkillAreaTargetGuild,
	eSkillAreaTargetAll,
	eSkillAreaTargetAllExceptMe,
};

// 080602 LUJ, 스킬 결과. 클라이언트에 실패 이유를 나타내기 위해 [cc]skill/server/manager/skillManager.h에서 옮겨옴
enum SKILL_RESULT
{
	SKILL_SUCCESS = 0,
	SKILL_OPERATOR_INVALUED,
	SKILL_INFO_INVALUED,
	SKILL_LEVEL_INVALUED,
	SKILL_STATE_INVALUED,
	SKILL_TARGET_INVALUED,
	SKILL_TARGET_IMMORTALED,
	SKILL_RANGE_NOTENOUGH,
	SKILL_COOLING,
	SKILL_FAIL_ETC,
	SKILL_DESTROY,
};

enum eBuffSkillActiveRule
{
	eBuffSkillActiveRule_Active,
	eBuffSkillActiveRule_Delete,
	eBuffSkillActiveRule_Stop,
};

// 090204 LUJ, 속성 변경
enum eBuffSkillCountType
{
	eBuffSkillCountType_None,
	eBuffSkillCountType_Dead,
	// 090204 LUJ, 모든 유형의 피해
	eBuffSkillCountType_AnyDamageVolumn = 10,
	eBuffSkillCountType_AnyDamage,
	eBuffSkillCountType_AnyHitVolumn,
	eBuffSkillCountType_AnyHit,
	// 090204 LUJ, 물리 피해
	eBuffSkillCountType_PhysicalDamageVolumn = 20,
	eBuffSkillCountType_PhysicalDamage,
	eBuffSkillCountType_PhysicalHitVolumn,
	eBuffSkillCountType_PhysicalHit,
	// 090204 LUJ, 마법 피해
	eBuffSkillCountType_MagicalDamageVolumn = 30,
	eBuffSkillCountType_MagicalDamage,
	eBuffSkillCountType_MagicalHitVolumn,
	eBuffSkillCountType_MagicalHit,
	// 090204 LUJ, 힐
	eBuffSkillCountType_HealVolumn = 40,
	eBuffSkillCountType_Heal,
	eBuffSkillCountType_BeHealedVolumn,
	eBuffSkillCountType_BeHealed,
};

enum eAutoNoteError
{
	eAutoNoteError_None,
	eAutoNoteError_CantFind,
	eAutoNoteError_NotProperState,
	eAutoNoteError_AlreadyAsked,
	eAutoNoteError_AlreadyAsking,
	eAutoNoteError_CantUseMap,
};

enum ePetAI
{
	ePetAI_Aggressive,
	ePetAI_Support,
	ePetAI_Stand,
};

enum ePetType
{
	ePetType_Basic,
	ePetType_Physic,
	ePetType_Magic,
	ePetType_Support,
	ePetType_Max,
};

enum ePetState
{
	ePetState_None,
	ePetState_Summon,
	ePetState_Die,
};

// 090701 ShinJS --- 던전 종류 구분
enum eLIMITDUNGEON_TYPE
{
	eLIMITDUNGEON_TYPE_LOW,			// 초급 던전
	eLIMITDUNGEON_TYPE_MIDDLE,		// 중급 던전
	eLIMITDUNGEON_TYPE_HIGH,		// 고급 던전
};


// 090909 ShinJS --- 길찾기 추가시 기존 필요정보 Common Header 로 이동
#define MOUSE_MOVE_TICK		400
#define MOUSE_PRESS_TICK	400
#define MOUSE_ON_TICK		200
#define FOLLOW_TICK			500		//---KES 따라가기
 

//  신규종족 생성 가능한 기존캐릭터의 레벨수치
#define MAKE_DEVIL_LIMIT_LEVEL	50

// 100322 ONS 퀘스트알리미 자동등록 개수제한 변경
#define MAX_QUICKVIEW_QUEST 10
#define MAX_QUEST_COUNT 20

#define MAX_CLASS_LEVEL		6
enum ENUM_CLASS
{
	eClass_FIGHTER = 0,
	eClass_ROGUE,
	eClass_MAGE, 
	eClass_DEVIL,
	eClass_MAXCLASS
} ;

enum eNoteParsingKind
{
	eNoteParsing_None = 0,
	eNoteParsing_FishingLevelUp,
	eNoteParsing_CookLevelUp,
};

enum eNoteError
{
	eNoteErr_None = 0,
	eNoteErr_WrongState,
	eNoteErr_InsertFail,
};

#define DEFAULT_MONEY_TEXT	"0"
#define DEFAULT_VOLUME_TEXT	"1"

enum eBUFF_START_RESULT
{
	eBUFF_START_RESULT_SUCCESS,
	eBUFF_START_RESULT_BATTLE_RULE_DELETE,
	eBUFF_START_RESULT_PEACE_RULE_DELETE,
	eBUFF_START_RESULT_MOVE_RULE_DELETE,
	eBUFF_START_RESULT_STOP_RULE_DELETE,
	eBUFF_START_RESULT_REST_RULE_DELETE,
};

#define MIN_MAGIC_CASTING_TIME	700.f

#endif //__COMMONGAMEDEFINE_H__

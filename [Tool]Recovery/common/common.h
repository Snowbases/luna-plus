#pragma once
#define MAX_DATE_SIZE	30

// 080401 LUJ,	서버에서 전송 가능한 최대 로그 개수. 너무 많은 쿼리가 반환될 경우 서버에 무한 대기 현상이 발생할 수 있어 이를 막기 위함.
//				한번에 쿼리되는 최대 레코드가 100이므로 100단위로만 지정할 것. 
#define MAX_LOG_SIZE	1000

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

// 080311 LUJ, 문자열 안전 복사
inline void StringCopySafe( wchar_t* const lhs, const wchar_t* const rhs, size_t size )
{
	wcsncpy( lhs, rhs, size );

	lhs[ size - 1 ] = 0;
}

// 080328 LUJ, 문자열 안전 복사
inline void StringCopySafe( char* const lhs, const char* const rhs, size_t size )
{
	strncpy( lhs, rhs, size );

	lhs[ size - 1 ] = 0;
}

// 081117 LUJ, 멀티바이트 문자열을 유니코드 형식으로 변환하며 복사
inline void StringCopySafe( wchar_t* const lhs, const char* const rhs, size_t size )
{
	//size = size / sizeof( *lhs );

	::mbstowcs(
		lhs,
		rhs,
		size );
}

struct Item : public ITEMBASE
{
	enum Area
	{
		AreaNone,
		AreaInventory,
		AreaCashStorage,
		AreaPrivateStorage,
		AreaPet,
	}
	mArea;

	// 080716 LUJ, 아이템을 가진 펫 번호
	DWORD mPetIndex;
};


// 080430 LUJ, [Client]Luna\Object.h에서 복사해옴
enum EObjectKind
{
	eObjectKind_Player					= 1,
	eObjectKind_Npc						= 2,
	eObjectKind_Item					= 4,
	eObjectKind_SkillObject				= 16,
	eObjectKind_Monster					= 32,
	eObjectKind_BossMonster				= 33,
	eObjectKind_SpecialMonster			= 34,
	eObjectKind_FieldBossMonster		= 35,
	eObjectKind_FieldSubMonster			= 36,
	eObjectKind_ToghterPlayMonster		= 37,
	eObjectKind_ChallengeZoneMonster	= 38,
	eObjectKind_MapObject				= 64,
	eObjectKind_CastleGate				= 65,
	eObjectKind_Pet						= 128,
};


// 080401 LUJ, 유니코드 형식의 이름을 표시하기 위해 상속받음
struct ItemScript : public ITEM_INFO
{
	TCHAR mUnicodeName[ MAX_ITEMNAME_LENGTH + 1 ];
};


#pragma pack(push,1)

enum RecoveryCategory
{
	MP_RM_CONNECT,
	MP_RM_SEARCH,
	MP_RM_PLAYER,
	MP_RM_ITEM,
	MP_RM_ITEM_FIND,
	MP_RM_ITEM_LOG,
	MP_RM_ITEM_OPTION_LOG,
	MP_RM_SKILL,
	MP_RM_SKILL_LOG,
	MP_RM_FAMILY,
	MP_RM_QUEST,
	MP_RM_QUEST_LOG,
	MP_RM_GUILD,
	MP_RM_GUILD_LOG,
	MP_RM_GUILD_WAREHOUSE_LOG,
	MP_RM_EXP_LOG,	
	MP_RM_STAT_LOG,	
	MP_RM_USER,
	MP_RM_JOB_LOG,
	MP_RM_GUILD_SCORE_LOG,
	MP_RM_FAMILY_CROP_LOG,
	MP_RM_FAMILY_POINT_LOG,
	MP_RM_PERMISSION,
	MP_RM_OPERATOR,
	MP_RM_OPERATOR_LOG,
	MP_RM_PASSWORD,	
	MP_RM_NAME_LOG,
	// 080320 LUJ, 드롭 옵션 로그
	MP_RM_ITEM_DROP_OPTION_LOG,
	// 080401 LUJ, 농장 로그
	MP_RM_FARM_LOG,
	// 080403 LUJ, 유료 아이템 구입 로그
	MP_RM_ITEM_SHOP_LOG,
	// 080523 LUJ, 낚시 로그
	MP_RM_FISH_LOG,
	// 080630 LUJ, 가축 로그
	MP_RM_LIVESTOCK_LOG,
	// 080716 LUJ, 펫 로그
	MP_RM_PET_LOG,
	// 080716 LUJ, 전역 아이템 로그 검색
	MP_RM_ITEM_GLOBAL_LOG,
	// 081021 LUJ, 권한 관리
	MP_RM_AUTHORITY,
	// 081027 LUJ, 길드 토너먼트
	MP_RM_TOURNAMENT,
	// 081027 LUJ, 공성 로그
	MP_RM_SIEGE_LOG,
	// 081205 LUJ, 채팅 로그
	MP_RM_CHAT_LOG,
	// 081205 LUJ, 패밀리 로그
	MP_RM_FAMILY_LOG,
	// 090122 LUJ, 스크립트 변조 로그
	MP_RM_SCRIPT_HACK_LOG,
	// 090406 LUJ, 돈 로그
	MP_RM_MONEY_LOG,
	// 090609 ShinJS, AutoNote 로그
	MP_RM_AUTONOTE_LOG,
	MP_RM_HOUSE_LOG,
	MP_RM_COOK_LOG,
	MP_RM_CONSIGN_LOG,
	MP_RM_GAMEROOM_POINT_LOG,

	// 100701 ONS 채팅금지 로그
	MP_RM_FORBID_CHAT_LOG,
};

// 081021 LUJ, 권한 종류
enum AuthorityType
{
	AuthorityTypeNone,
	AuthorityTypeVersion,
	AuthorityTypePlayerLevel	= 100,
	AuthorityTypePlayerCreate,
	AuthorityTypePlayerDelete,
	AuthorityTypePlayerRecover,
	AuthorityTypePlayerLog,
	AuthorityTypePlayerUpdate,
	AuthorityTypeSkill			= 200,
	AuthorityTypeQuest			= 300,
	AuthorityTypeGuild			= 400,
	AuthorityTypeFamily			= 500,
	AuthorityTypeOperator		= 600,
	AuthorityTypeAllowIp		= 700,
	AuthorityTypeOperatorLog	= 800,
	// 090219 LUJ, 아이템 권한 관리
	AuthorityTypeItem			= 900,
};

enum
{
	MP_RM_CONNECT_SYN,
	MP_RM_CONNECT_ACK,
	MP_RM_CONNECT_NACK,
	MP_RM_CONNECT_NACK_BY_IP,	// 한 아이피에서 중복 접속 시도
	MP_RM_CONNECT_NACK_BY_ID,	// 한 아이디로 중복 접속 시도
	MP_RM_CONNECT_NACK_BY_AUTH,

	// 080311 LUJ, 버전 차이로 인해 접속 못할 경우
	MP_RM_CONNECT_NACK_BY_VERSION,
};


enum
{
	MP_RM_SEARCH_SYN,
	MP_RM_SEARCH_ACK,
	MP_RM_SEARCH_NACK,
	MP_RM_SEARCH_NACK_BY_AUTH,
	MP_RM_SEARCH_USER_NAME_ACK,
	//MP_RM_SEARCH_NO_RESULT_ACK,	// 속도 저하 요인으로 차단
};


enum
{
	MP_RM_PLAYER_NACK_BY_AUTH,

	MP_RM_PLAYER_GET_USER_SYN,
	MP_RM_PLAYER_GET_USER_ACK,
	MP_RM_PLAYER_GET_USER_NACK,
	
	MP_RM_PLAYER_GET_DATA_SYN,
	MP_RM_PLAYER_GET_DATA_ACK,
	MP_RM_PLAYER_GET_EXTENDED_DATA_ACK,
	MP_RM_PLAYER_GET_DATA_NACK,

	MP_RM_PLAYER_GET_LOGIN_ID_NACK,
	MP_RM_PLAYER_GET_LOGIN_ID_ACK,

	MP_RM_PLAYER_SET_DATA_SYN,
	MP_RM_PLAYER_SET_DATA_ACK,
	MP_RM_PLAYER_SET_DATA_NACK,

	MP_RM_PLAYER_SET_EXTENDED_DATA_SYN,
	MP_RM_PLAYER_SET_EXTENDED_DATA_ACK,
	MP_RM_PLAYER_SET_EXTENDED_DATA_NACK,

	MP_RM_PLAYER_GET_LICENSE_SYN,
	MP_RM_PLAYER_GET_LICENSE_ACK,
	MP_RM_PLAYER_GET_LICENSE_NACK,

	MP_RM_PLAYER_SET_LICENSE_SYN,
	MP_RM_PLAYER_SET_LICENSE_ACK,
	MP_RM_PLAYER_SET_LICENSE_NACK,

	// 080328 LUJ, 플레이어의 패밀리 정보를 가져옴
	MP_RM_PLAYER_GET_FAMILY_ACK,

	// 080526 LUJ, 낚시 정보
	MP_RM_PLAYER_GET_FISHING_ACK,
	MP_RM_PLAYER_SET_FISHING_SYN,
	MP_RM_PLAYER_SET_FISHING_ACK,
	MP_RM_PLAYER_SET_FISHING_NACK,
	MP_RM_PLAYER_GET_COOK_ACK,
	MP_RM_PLAYER_GET_COOK_RECIPE_ACK,
	MP_RM_PLAYER_SET_COOK_SYN,
	MP_RM_PLAYER_SET_COOK_ACK,
	MP_RM_PLAYER_ADD_COOK_RECIPE_SYN,
	MP_RM_PLAYER_ADD_COOK_RECIPE_ACK,
	MP_RM_PLAYER_REMOVE_COOK_RECIPE_SYN,
	MP_RM_PLAYER_REMOVE_COOK_RECIPE_ACK,
	MP_RM_PLAYER_GET_HOUSE_ACK,
	MP_RM_PLAYER_SET_HOUSE_SYN,
	MP_RM_PLAYER_SET_HOUSE_ACK,
	MP_RM_PLAYER_GET_NOTE_LIST_ACK,
	MP_RM_PLAYER_GET_NOTE_SYN,
	MP_RM_PLAYER_GET_NOTE_ACK,
	MP_RM_PLAYER_GET_PARTY_ACK,
};

enum
{
	MP_RM_ITEM_NACK_BY_AUTH,

	MP_RM_ITEM_GET_SYN,
	MP_RM_ITEM_GET_ACK,
	//MP_RM_ITEM_GET_NACK,

	//MP_RM_ITEM_GET_OPTION_SYN,
	MP_RM_ITEM_GET_OPTION_ACK,
	MP_RM_ITEM_GET_OPTION_NACK,

	MP_RM_ITEM_SET_SYN,
	MP_RM_ITEM_SET_ACK,
	MP_RM_ITEM_SET_NACK,

	MP_RM_ITEM_SET_OPTION_SYN,
	MP_RM_ITEM_SET_OPTION_ACK,
	MP_RM_ITEM_SET_OPTION_NACK,

	MP_RM_ITEM_DELETE_SYN,
	MP_RM_ITEM_DELETE_ACK,
	MP_RM_ITEM_DELETE_NACK,

	MP_RM_ITEM_DELETE_OPTION_SYN,
	MP_RM_ITEM_DELETE_OPTION_ACK,
	MP_RM_ITEM_DELETE_OPTION_NACK,

	MP_RM_ITEM_GET_STORAGE_ACK,

	MP_RM_ITEM_SET_STORAGE_SYN,
	MP_RM_ITEM_SET_STORAGE_ACK,

	MP_RM_ITEM_GET_EXTENDED_INVENTORY_SYN,

	MP_RM_ITEM_ADD_SYN,
	MP_RM_ITEM_ADD_ACK,
	MP_RM_ITEM_ADD_NACK,

	MP_RM_ITEM_UPDATE_END_TIME_SYN,
	MP_RM_ITEM_UPDATE_END_TIME_ACK,
	MP_RM_ITEM_UPDATE_END_TIME_NACK,

	MP_RM_ITEM_UPDATE_REMAIN_TIME_SYN,
	MP_RM_ITEM_UPDATE_REMAIN_TIME_ACK,
	MP_RM_ITEM_UPDATE_REMAIN_TIME_NACK,
	
	// 080320 LUJ, 드롭 옵션 삭제
	MP_RM_ITEM_DELETE_DROP_OPTION_SYN,
	MP_RM_ITEM_DELETE_DROP_OPTION_ACK,
	MP_RM_ITEM_DELETE_DROP_OPTION_NACK,	

	// 080716 LUJ, 펫 정보
	MP_RM_ITEM_GET_PET_ACK,
	MP_RM_ITEM_SET_PET_SYN,
	MP_RM_ITEM_SET_PET_ACK,
	MP_RM_ITEM_SET_PET_NACK,
	// 080731 LUJ, 펫 추가
	MP_RM_ITEM_ADD_PET_SYN,
	MP_RM_ITEM_ADD_PET_ACK,
	MP_RM_ITEM_ADD_PET_NACK,
	// 080731 LUJ, 펫 삭제
	MP_RM_ITEM_REMOVE_PET_SYN,
	MP_RM_ITEM_REMOVE_PET_ACK,
	// 가구 관리
	MP_RM_ITEM_GET_FURNITURE_ACK,
	MP_RM_ITEM_ADD_FURNITURE_SYN,
	MP_RM_ITEM_ADD_FURNITURE_ACK,
	MP_RM_ITEM_REMOVE_FURNITURE_SYN,
	MP_RM_ITEM_REMOVE_FURNITURE_ACK,
};


enum
{
	MP_RM_ITEM_FIND_OWNER_SYN,
	MP_RM_ITEM_FIND_OWNER_ACK,
	MP_RM_ITEM_FIND_OWNER_NACK,
	MP_RM_ITEM_FIND_OWNER_NACK_BY_AUTH,
};


enum
{
	MP_RM_ITEM_LOG_NACK_BY_AUTH,

	//MP_RM_ITEM_LOG_GET_SYN,
	MP_RM_ITEM_LOG_GET_ACK,
	//MP_RM_ITEM_LOG_GET_NACK,

	MP_RM_ITEM_LOG_GET_SIZE_SYN,
	MP_RM_ITEM_LOG_GET_SIZE_ACK,
	MP_RM_ITEM_LOG_GET_SIZE_NACK,

	MP_RM_ITEM_LOG_GET_STOP_SYN,
	MP_RM_ITEM_LOG_GET_STOP_ACK,
	//MP_RM_ITEM_LOG_GET_STOP_NACK,
};


enum
{
	MP_RM_ITEM_OPTION_LOG_NACK_BY_AUTH,

	//MP_RM_ITEM_OPTION_LOG_GET_SYN,
	MP_RM_ITEM_OPTION_LOG_GET_ACK,
	//MP_RM_ITEM_OPTION_LOG_GET_NACK,

	MP_RM_ITEM_OPTION_LOG_SIZE_SYN,
	MP_RM_ITEM_OPTION_LOG_SIZE_ACK,
	MP_RM_ITEM_OPTION_LOG_SIZE_NACK,

	MP_RM_ITEM_OPTION_LOG_STOP_SYN,
	MP_RM_ITEM_OPTION_LOG_STOP_ACK,
	//MP_RM_ITEM_OPTION_LOG_STOP_NACK,
};


// 080320 LUJ, 드롭 옵션 로그 프로토콜
enum
{
	MP_RM_ITEM_DROP_OPTION_LOG_NACK_BY_AUTH,

	//MP_RM_ITEM_DROP_OPTION_LOG_GET_SYN,
	MP_RM_ITEM_DROP_OPTION_LOG_GET_ACK,
	//MP_RM_ITEM_DROP_OPTION_LOG_GET_NACK,

	MP_RM_ITEM_DROP_OPTION_LOG_SIZE_SYN,
	MP_RM_ITEM_DROP_OPTION_LOG_SIZE_ACK,
	MP_RM_ITEM_DROP_OPTION_LOG_SIZE_NACK,

	MP_RM_ITEM_DROP_OPTION_LOG_STOP_SYN,
	MP_RM_ITEM_DROP_OPTION_LOG_STOP_ACK,
	//MP_RM_ITEM_DROP_OPTION_LOG_STOP_NACK,
};


enum
{
	MP_RM_SKILL_NACK_BY_AUTH,

	MP_RM_SKILL_GET_ALL_SYN,
	MP_RM_SKILL_GET_ALL_ACK,
	MP_RM_SKILL_GET_ALL_NACK,

	MP_RM_SKILL_SET_LEVEL_SYN,
	MP_RM_SKILL_SET_LEVEL_ACK,
	MP_RM_SKILL_SET_LEVEL_NACK,

	MP_RM_SKILL_ADD_SYN,
	MP_RM_SKILL_ADD_ACK,
	MP_RM_SKILL_ADD_NACK,

	MP_RM_SKILL_REMOVE_SYN,
	MP_RM_SKILL_REMOVE_ACK,
	MP_RM_SKILL_REMOVE_NACK,

	// 080731 LUJ, 버프 스킬
	MP_RM_SKILL_BUFF_ACK,
	// 090204 LUJ, 버프 삭제
	MP_RM_SKILL_BUFF_REMOVE_SYN,
};


enum
{
	MP_RM_SKILL_LOG_NACK_BY_AUTH,

	//MP_RM_SKILL_LOG_GET_SYN,
	MP_RM_SKILL_LOG_GET_ACK,
	//MP_RM_SKILL_LOG_GET_NACK,

	MP_RM_SKILL_LOG_SIZE_SYN,
	MP_RM_SKILL_LOG_SIZE_ACK,
	MP_RM_SKILL_LOG_SIZE_NACK,

	MP_RM_SKILL_LOG_STOP_SYN,
	MP_RM_SKILL_LOG_STOP_ACK,
	//MP_RM_SKILL_LOG_STOP_NACK,
};


enum
{
	MP_RM_FAMILY_NACK_BY_AUTH,

	MP_RM_FAMILY_GET_LIST_SYN,
	MP_RM_FAMILY_GET_LIST_ACK,
	MP_RM_FAMILY_GET_LIST_NACK,

	MP_RM_FAMILY_GET_DATA_SYN,
	MP_RM_FAMILY_GET_DATA_ACK,
	MP_RM_FAMILY_GET_DATA_NACK,

	MP_RM_FAMILY_SET_DATA_SYN,
	MP_RM_FAMILY_SET_DATA_ACK,
	MP_RM_FAMILY_SET_DATA_NACK,

	MP_RM_FAMILY_GET_FARM_SYN,
	MP_RM_FAMILY_GET_FARM_ACK,
	MP_RM_FAMILY_GET_FARM_NACK,

	MP_RM_FAMILY_GET_MEMBER_ACK,
	MP_RM_FAMILY_GET_CROP_ACK,
	// 080731 LUJ, 동물 목록 가져옴
	MP_RM_FAMILY_GET_LIVESTOCK_ACK,
};


enum
{
	MP_RM_QUEST_NACK_BY_AUTH,

	MP_RM_QUEST_GET_MAIN_DATA_SYN,
	MP_RM_QUEST_GET_MAIN_DATA_ACK,
	MP_RM_QUEST_GET_MAIN_DATA_NACK,

	MP_RM_QUEST_GET_SUB_DATA_SYN,
	MP_RM_QUEST_GET_SUB_DATA_ACK,
	MP_RM_QUEST_GET_SUB_DATA_NACK,

	MP_RM_QUEST_GET_ITEM_DATA_SYN,
	MP_RM_QUEST_GET_ITEM_DATA_ACK,
	MP_RM_QUEST_GET_ITEM_DATA_NACK,

	MP_RM_QUEST_REMOVE_SYN,
	MP_RM_QUEST_REMOVE_ACK,
	MP_RM_QUEST_REMOVE_NACK,

	MP_RM_QUEST_ITEM_ADD_SYN,
	MP_RM_QUEST_ITEM_ADD_ACK,
	MP_RM_QUEST_ITEM_ADD_NACK,

	MP_RM_QUEST_ITEM_UPDATE_SYN,
	MP_RM_QUEST_ITEM_UPDATE_ACK,
	MP_RM_QUEST_ITEM_UPDATE_NACK,

	MP_RM_QUEST_ITEM_DELETE_SYN,
	MP_RM_QUEST_ITEM_DELETE_ACK,
	MP_RM_QUEST_ITEM_DELETE_NACK,

	MP_RM_QUEST_FINISH_SUB_SYN,
	MP_RM_QUEST_FINISH_SUB_ACK,
	MP_RM_QUEST_FINISH_SUB_NACK,
};


enum
{
	MP_RM_QUEST_LOG_NACK_BY_AUTH,

	MP_RM_QUEST_LOG_SIZE_SYN,
	MP_RM_QUEST_LOG_SIZE_ACK,
	MP_RM_QUEST_LOG_SIZE_NACK,

	MP_RM_QUEST_LOG_STOP_SYN,
	MP_RM_QUEST_LOG_STOP_ACK,
	//MP_RM_QUEST_LOG_STOP_NACK,

	//MP_RM_QUEST_LOG_GET_SYN,
	MP_RM_QUEST_LOG_GET_ACK,
	//MP_RM_QUEST_LOG_GET_NACK,
};


enum
{
	MP_RM_GUILD_NACK_BY_AUTH,

	MP_RM_GUILD_GET_LIST_SYN,
	MP_RM_GUILD_GET_LIST_ACK,
	MP_RM_GUILD_GET_LIST_NACK,

	MP_RM_GUILD_GET_DATA_SYN,
	MP_RM_GUILD_GET_DATA_ACK,
	MP_RM_GUILD_GET_DATA_NACK,

	//MP_RM_GUILD_GET_STORE_SYN,
	MP_RM_GUILD_GET_STORE_ACK,
	//MP_RM_GUILD_GET_STORE_NACK,

	//MP_RM_GUILD_GET_MEMBER_SYN,
	MP_RM_GUILD_GET_MEMBER_ACK,
	//MP_RM_GUILD_GET_MEMBER_NACK,

	MP_RM_GUILD_GET_FRIENDLY_ACK,
	MP_RM_GUILD_GET_ENEMY_ACK,

	MP_RM_GUILD_SET_DATA_SYN,
	MP_RM_GUILD_SET_DATA_ACK,
	MP_RM_GUILD_SET_DATA_NACK,

	MP_RM_GUILD_SET_RANK_SYN,
	MP_RM_GUILD_SET_RANK_ACK,
	MP_RM_GUILD_SET_RANK_NACK,

	MP_RM_GUILD_KICK_SYN,
	MP_RM_GUILD_KICK_ACK,
	MP_RM_GUILD_KICK_NACK,
	MP_RM_GUILD_KICK_NACK_BY_NO_GUILD,
	MP_RM_GUILD_KICK_NACK_BY_ONE_MEMBER,

	// 080425 LUJ, 길드 스킬
	MP_RM_GUILD_GET_SKILL_ACK,

	MP_RM_GUILD_REMOVE_SKILL_SYN,
	MP_RM_GUILD_REMOVE_SKILL_ACK,
	MP_RM_GUILD_REMOVE_SKILL_NACK,
};


enum
{
	MP_RM_GUILD_SCORE_LOG_NACK_BY_AUTH,

	//MP_RM_GUILD_SCORE_LOG_GET_SYN,
	MP_RM_GUILD_SCORE_LOG_GET_ACK,
	//MP_RM_GUILD_SCORE_LOG_GET_NACK,

	MP_RM_GUILD_SCORE_LOG_SIZE_SYN,
	MP_RM_GUILD_SCORE_LOG_SIZE_ACK,
	MP_RM_GUILD_SCORE_LOG_SIZE_NACK,

	MP_RM_GUILD_SCORE_LOG_STOP_SYN,
	MP_RM_GUILD_SCORE_LOG_STOP_ACK,
	//MP_RM_GUILD_SCORE_LOG_STOP_NACK,
};


enum
{
	MP_RM_GUILD_LOG_NACK_BY_AUTH,

	//MP_RM_GUILD_LOG_GET_SYN,
	MP_RM_GUILD_LOG_GET_ACK,
	//MP_RM_GUILD_LOG_GET_NACK,

	MP_RM_GUILD_LOG_SIZE_SYN,
	MP_RM_GUILD_LOG_SIZE_ACK,
	MP_RM_GUILD_LOG_SIZE_NACK,

	MP_RM_GUILD_LOG_STOP_SYN,
	MP_RM_GUILD_LOG_STOP_ACK,
	//MP_RM_GUILD_LOG_STOP_NACK,
};


enum
{
	MP_RM_GUILD_WAREHOUSE_LOG_NACK_BY_AUTH,

	//MP_RM_GUILD_WAREHOUSE_LOG_GET_SYN,
	MP_RM_GUILD_WAREHOUSE_LOG_GET_ACK,
	//MP_RM_GUILD_WAREHOUSE_LOG_GET_NACK,

	MP_RM_GUILD_WAREHOUSE_LOG_SIZE_SYN,
	MP_RM_GUILD_WAREHOUSE_LOG_SIZE_ACK,
	MP_RM_GUILD_WAREHOUSE_LOG_SIZE_NACK,

	MP_RM_GUILD_WAREHOUSE_LOG_STOP_SYN,
	MP_RM_GUILD_WAREHOUSE_LOG_STOP_ACK,
	//MP_RM_GUILD_WAREHOUSE_LOG_STOP_NACK,
};


enum
{
	MP_RM_EXP_LOG_NACK_BY_AUTH,

	//MP_RM_EXT_GET_SYN,
	MP_RM_EXP_LOG_GET_ACK,
	MP_RM_EXP_LOG_GET_NACK,

	MP_RM_EXP_LOG_GET_STOP_SYN,
	MP_RM_EXP_LOG_GET_STOP_ACK,
	MP_RM_EXP_LOG_GET_STOP_NACK,

	MP_RM_EXP_LOG_GET_SIZE_SYN,
	MP_RM_EXP_LOG_GET_SIZE_ACK,
	MP_RM_EXP_LOG_GET_SIZE_NACK,
};


enum
{
	MP_RM_STAT_LOG_NACK_BY_AUTH,

	//MP_RM_STAT_LOG_GET_SYN,
	MP_RM_STAT_LOG_GET_ACK,
	//MP_RM_STAT_LOG_GET_NACK,

	MP_RM_STAT_LOG_SIZE_SYN,
	MP_RM_STAT_LOG_SIZE_ACK,
	MP_RM_STAT_LOG_SIZE_NACK,

	MP_RM_STAT_LOG_STOP_SYN,
	MP_RM_STAT_LOG_STOP_ACK,
	//MP_RM_STAT_LOG_STOP_NACK,
};


enum
{
	MP_RM_USER_NACK_BY_AUTH,

	//MP_RM_USER_GET_DATA_BY_PLAYER_INDEX_SYN,
	MP_RM_USER_GET_DATA_SYN,
	MP_RM_USER_GET_DATA_ACK,
	MP_RM_USER_GET_DATA_NACK,

	MP_RM_USER_GET_PLAYER_LIST_ACK,

	MP_RM_USER_SET_DATA_SYN,
	MP_RM_USER_SET_DATA_ACK,
	MP_RM_USER_SET_DATA_NACK,

	MP_RM_USER_CREATE_SYN,
	MP_RM_USER_CREATE_ACK,
	MP_RM_USER_CREATE_NACK,
	MP_RM_USER_CREATE_NACK_BY_DUPLICATED_NAME,
	MP_RM_USER_CREATE_NACK_BY_GUILD,
	MP_RM_USER_CREATE_NACK_BY_FULL,

	MP_RM_USER_RESTORE_SYN,
	MP_RM_USER_RESTORE_ACK,
	MP_RM_USER_RESTORE_NACK,
	MP_RM_USER_RESTORE_NACK_BY_DUPLICATED_NAME,
	MP_RM_USER_RESTORE_NACK_BY_GUILD,
	MP_RM_USER_RESTORE_NACK_BY_FAMILY,
	MP_RM_USER_RESTORE_NACK_BY_FULL,
	MP_RM_USER_RESTORE_NACK_BY_NOT_EXIST,

	MP_RM_USER_DELETE_SYN,
	MP_RM_USER_DELETE_ACK,
	MP_RM_USER_DELETE_NACK,
	MP_RM_USER_DELETE_NACK_BY_FAMILY,
	MP_RM_USER_DELETE_NACK_BY_PARTY,
	MP_RM_USER_DELETE_NACK_BY_GUILD,

	// 090618 ShinJS --- Add User Punish Data Update
	MP_RM_USER_UPDATE_PUNISH_DATA,
	MP_RM_USER_GAMEROOM_GET_ACK,
	MP_RM_USER_GAMEROOM_SET_SYN,
	MP_RM_USER_GAMEROOM_SET_ACK,
};

enum
{
	MP_RM_JOB_LOG_NACK_BY_AUTH,

	//MP_RM_JOB_LOG_GET_SYN,
	MP_RM_JOB_LOG_GET_ACK,
	//MP_RM_JOB_LOG_GET_NACK,

	MP_RM_JOB_LOG_SIZE_SYN,
	MP_RM_JOB_LOG_SIZE_ACK,
	MP_RM_JOB_LOG_SIZE_NACK,

	MP_RM_JOB_LOG_STOP_SYN,
	MP_RM_JOB_LOG_STOP_ACK,
	//MP_RM_JOB_LOG_STOP_NACK,
};

	
enum
{
	MP_RM_FAMILY_CROP_LOG_NACK_BY_AUTH,

	//MP_RM_FAMILY_CROP_LOG_GET_SYN,
	MP_RM_FAMILY_CROP_LOG_GET_ACK,
	//MP_RM_FAMILY_CROP_LOG_GET_NACK,

	MP_RM_FAMILY_CROP_LOG_SIZE_SYN,
	MP_RM_FAMILY_CROP_LOG_SIZE_ACK,
	MP_RM_FAMILY_CROP_LOG_SIZE_NACK,

	MP_RM_FAMILY_CROP_LOG_STOP_SYN,
	MP_RM_FAMILY_CROP_LOG_STOP_ACK,
	//MP_RM_FAMILY_CROP_LOG_STOP_NACK,
};


enum
{
	MP_RM_FAMILY_POINT_LOG_NACK_BY_AUTH,

	//MP_RM_FAMILY_POINT_LOG_GET_SYN,
	MP_RM_FAMILY_POINT_LOG_GET_ACK,
	//MP_RM_FAMILY_POINT_LOG_GET_NACK,

	MP_RM_FAMILY_POINT_LOG_SIZE_SYN,
	MP_RM_FAMILY_POINT_LOG_SIZE_ACK,
	MP_RM_FAMILY_POINT_LOG_SIZE_NACK,

	MP_RM_FAMILY_POINT_LOG_STOP_SYN,
	MP_RM_FAMILY_POINT_LOG_STOP_ACK,
	//MP_RM_FAMILY_POINT_LOG_STOP_NACK,
};


enum
{
	MP_RM_PERMISSION_NACK_BY_AUTH,

	MP_RM_PERMISSION_ADD_SYN,
	MP_RM_PERMISSION_ADD_ACK,
	MP_RM_PERMISSION_ADD_NACK,

	MP_RM_PERMISSION_REMOVE_SYN,
	MP_RM_PERMISSION_REMOVE_ACK,
	MP_RM_PERMISSION_REMOVE_NACK,
};


enum
{
	MP_RM_OPERATOR_NACK_BY_AUTH,

	MP_RM_OPERATOR_GET_SYN,
	MP_RM_OPERATOR_GET_ACK,
	
	MP_RM_OPERATOR_ADD_SYN,
	MP_RM_OPERATOR_ADD_ACK,
	MP_RM_OPERATOR_ADD_NACK_BY_ID,

	MP_RM_OPERATOR_REMOVE_SYN,
	MP_RM_OPERATOR_REMOVE_ACK,
	MP_RM_OPERATOR_REMOVE_NACK,

	MP_RM_OPERATOR_SET_SYN,
	MP_RM_OPERATOR_SET_ACK,
	MP_RM_OPERATOR_SET_NACK,

	MP_RM_OPERATOR_IP_GET_ACK,

	MP_RM_OPERATOR_IP_ADD_SYN,
	MP_RM_OPERATOR_IP_ADD_ACK,
	MP_RM_OPERATOR_IP_ADD_NACK,

	MP_RM_OPERATOR_IP_REMOVE_SYN,
	MP_RM_OPERATOR_IP_REMOVE_ACK,
	MP_RM_OPERATOR_IP_REMOVE_NACK,
};


enum
{
	MP_RM_OPERATOR_LOG_NACK_BY_AUTH,

	//MP_RM_OPERATOR_LOG_SYN,
	MP_RM_OPERATOR_LOG_ACK,
	//MP_RM_OPERATOR_LOG_NACK,

	MP_RM_OPERATOR_LOG_STOP_SYN,

	MP_RM_OPERATOR_LOG_SIZE_SYN,
	MP_RM_OPERATOR_LOG_SIZE_ACK,
	MP_RM_OPERATOR_LOG_SIZE_NACK,
};


enum
{
	MP_RM_PASSWORD_SET_SYN,
	MP_RM_PASSWORD_SET_ACK,
	MP_RM_PASSWORD_SET_NACK,
};


enum
{
	MP_RM_NAME_LOG_GET_SYN,
	MP_RM_NAME_LOG_GET_ACK,
	MP_RM_NAME_LOG_GET_NACK,
};


// 080401 LUJ, 농장 로그
enum
{
	MP_RM_FARM_LOG_NACK_BY_AUTH,

	MP_RM_FARM_LOG_ACK,
	
	MP_RM_FARM_LOG_SIZE_SYN,
	MP_RM_FARM_LOG_SIZE_ACK,
	MP_RM_FARM_LOG_SIZE_NACK,

	MP_RM_FARM_LOG_STOP_SYN,
	MP_RM_FARM_LOG_STOP_ACK,
};


// 080403 LUJ, 유료 아이템 구입 로그
enum
{
	MP_RM_ITEM_SHOP_LOG_NACK_BY_AUTH,

	MP_RM_ITEM_SHOP_LOG_ACK,

	MP_RM_ITEM_SHOP_LOG_SIZE_SYN,
	MP_RM_ITEM_SHOP_LOG_SIZE_ACK,
	MP_RM_ITEM_SHOP_LOG_SIZE_NACK,

	MP_RM_ITEM_SHOP_LOG_STOP_SYN,
	MP_RM_ITEM_SHOP_LOG_STOP_ACK,
};


// 080523 LUJ, 낚시 로그
enum
{
	MP_RM_FISH_LOG_NACK_BY_AUTH,

	MP_RM_FISH_LOG_ACK,

	MP_RM_FISH_LOG_SIZE_SYN,
	MP_RM_FISH_LOG_SIZE_ACK,
	MP_RM_FISH_LOG_SIZE_NACK,

	MP_RM_FISH_LOG_STOP_SYN,
	MP_RM_FISH_LOG_STOP_ACK,
};


// 080630 LUJ, 가축 로그
enum
{
	MP_RM_LIVESTOCK_LOG_NACK_BY_AUTH,

	MP_RM_LIVESTOCK_LOG_ACK,

	MP_RM_LIVESTOCK_LOG_SIZE_SYN,
	MP_RM_LIVESTOCK_LOG_SIZE_ACK,
	MP_RM_LIVESTOCK_LOG_SIZE_NACK,

	MP_RM_LIVESTOCK_LOG_STOP_SYN,
	MP_RM_LIVESTOCK_LOG_STOP_ACK,
};


// 080716 LUJ, 펫 로그
enum
{
	MP_RM_PET_LOG_NACK_BY_AUTH,

	MP_RM_PET_LOG_ACK,

	MP_RM_PET_LOG_SIZE_SYN,
	MP_RM_PET_LOG_SIZE_ACK,
	MP_RM_PET_LOG_SIZE_NACK,

	MP_RM_PET_LOG_STOP_SYN,
	MP_RM_PET_LOG_STOP_ACK,
};


// 080716 LUJ, 전역 아이템 로그 검색
enum
{
	MP_RM_ITEM_GLOBAL_LOG_NACK_BY_AUTH,

	MP_RM_ITEM_GLOBAL_LOG_READY_ACK,
	MP_RM_ITEM_GLOBAL_LOG_SYN,
	MP_RM_ITEM_GLOBAL_LOG_ACK,
	MP_RM_ITEM_GLOBAL_LOG_END_ACK,
	MP_RM_ITEM_GLOBAL_LOG_STOP_SYN,
	MP_RM_ITEM_GLOBAL_LOG_STOP_ACK,	
};

// 081021 LUJ, 권한 관리
enum
{
	MP_RM_AUTHORITY_GET_SYN,
	MP_RM_AUTHORITY_GET_ACK,
	MP_RM_AUTHORITY_GET_NACK,
	MP_RM_AUTHORITY_SET_SYN,
	MP_RM_AUTHORITY_SET_ACK,
	MP_RM_AUTHORITY_SET_NACK,
	MP_RM_AUTHORITY_NACK_BY_AUTH,
};

// 081027 LUJ, 길드 토너먼트
enum
{
	// 081027 LUJ, 최대 회차 정보를 가져온다
	MP_RM_TOURNAMENT_GET_ROUND_SYN,
	MP_RM_TOURNAMENT_GET_ROUND_ACK,
	MP_RM_TOURNAMENT_GET_ROUND_NACK_BY_AUTH,
	// 081027 LUJ, 특정 회차의 토너먼트 정보를 가져온다
	MP_RM_TOURNAMENT_GET_DATA_SYN,
	MP_RM_TOURNAMENT_GET_DATA_PLAYER_ACK,
	MP_RM_TOURNAMENT_GET_DATA_GUILD_ACK,
	MP_RM_TOURNAMENT_GET_DATA_NACK,
	// 081027 LUJ, 대진표 정보를 가져온다
	MP_RM_TOURNAMENT_GET_TABLE_SYN,
	MP_RM_TOURNAMENT_GET_TABLE_ACK,
	MP_RM_TOURNAMENT_GET_TABLE_NACK,
};

// 081027 LUJ, 공성 로그
enum
{
	MP_RM_SIEGE_LOG_NACK_BY_AUTH,
	MP_RM_SIEGE_LOG_ACK,
	MP_RM_SIEGE_LOG_SIZE_SYN,
	MP_RM_SIEGE_LOG_SIZE_ACK,
	MP_RM_SIEGE_LOG_SIZE_NACK,
	MP_RM_SIEGE_LOG_STOP_SYN,
	MP_RM_SIEGE_LOG_STOP_ACK,
};

// 081205 LUJ, 채팅 로그
enum
{
	MP_RM_CHAT_LOG_NACK_BY_AUTH,
	MP_RM_CHAT_LOG_ACK,
	MP_RM_CHAT_LOG_SIZE_SYN,
	MP_RM_CHAT_LOG_SIZE_ACK,
	MP_RM_CHAT_LOG_SIZE_NACK,
	MP_RM_CHAT_LOG_STOP_SYN,
	MP_RM_CHAT_LOG_STOP_ACK,
};

// 081205 LUJ, 패밀리 로그
enum
{
	MP_RM_FAMILY_LOG_NACK_BY_AUTH,
	MP_RM_FAMILY_LOG_ACK,
	MP_RM_FAMILY_LOG_SIZE_SYN,
	MP_RM_FAMILY_LOG_SIZE_ACK,
	MP_RM_FAMILY_LOG_SIZE_NACK,
	MP_RM_FAMILY_LOG_STOP_SYN,
	MP_RM_FAMILY_LOG_STOP_ACK,
};

// 090122 LUJ, 스크립트 해킹 로그
enum
{
	MP_RM_SCRIPT_HACK_LOG_NACK_BY_AUTH,
	MP_RM_SCRIPT_HACK_LOG_ACK,
	MP_RM_SCRIPT_HACK_LOG_SIZE_SYN,
	MP_RM_SCRIPT_HACK_LOG_SIZE_ACK,
	MP_RM_SCRIPT_HACK_LOG_SIZE_NACK,
	MP_RM_SCRIPT_HACK_LOG_STOP_SYN,
	MP_RM_SCRIPT_HACK_LOG_STOP_ACK,
};

// 090406 LUJ, 돈 로그
enum
{
	MP_RM_MONEY_LOG_NACK_BY_AUTH,
	MP_RM_MONEY_LOG_ACK,
	MP_RM_MONEY_LOG_SIZE_SYN,
	MP_RM_MONEY_LOG_SIZE_ACK,
	MP_RM_MONEY_LOG_SIZE_NACK,
	MP_RM_MONEY_LOG_STOP_SYN,
	MP_RM_MONEY_LOG_STOP_ACK,
};

// 090609 ShinJS, AutoNote 로그
enum
{
	MP_RM_AUTONOTE_LOG_NACK_BY_AUTH,
	MP_RM_AUTONOTE_LOG_ACK,
	MP_RM_AUTONOTE_LOG_SIZE_SYN,
	MP_RM_AUTONOTE_LOG_SIZE_ACK,
	MP_RM_AUTONOTE_LOG_SIZE_NACK,
	MP_RM_AUTONOTE_LOG_STOP_SYN,
	MP_RM_AUTONOTE_LOG_STOP_ACK,
};

enum
{
	MP_RM_HOUSE_LOG_NACK_BY_AUTH,
	MP_RM_HOUSE_LOG_ACK,
	MP_RM_HOUSE_LOG_SIZE_SYN,
	MP_RM_HOUSE_LOG_SIZE_ACK,
	MP_RM_HOUSE_LOG_SIZE_NACK,
	MP_RM_HOUSE_LOG_STOP_SYN,
	MP_RM_HOUSE_LOG_STOP_ACK,
};

enum
{
	MP_RM_COOK_LOG_NACK_BY_AUTH,
	MP_RM_COOK_LOG_ACK,
	MP_RM_COOK_LOG_SIZE_SYN,
	MP_RM_COOK_LOG_SIZE_ACK,
	MP_RM_COOK_LOG_SIZE_NACK,
	MP_RM_COOK_LOG_STOP_SYN,
	MP_RM_COOK_LOG_STOP_ACK,
	MP_RM_COOK_RECIPE_LOG_NACK_BY_AUTH,
	MP_RM_COOK_RECIPE_LOG_ACK,
	MP_RM_COOK_RECIPE_LOG_SIZE_SYN,
	MP_RM_COOK_RECIPE_LOG_SIZE_ACK,
	MP_RM_COOK_RECIPE_LOG_SIZE_NACK,
	MP_RM_COOK_RECIPE_LOG_STOP_SYN,
	MP_RM_COOK_RECIPE_LOG_STOP_ACK,
};

enum
{
	MP_RM_CONSIGN_LOG_NACK_BY_AUTH,
	MP_RM_CONSIGN_LOG_ACK,
	MP_RM_CONSIGN_LOG_SIZE_SYN,
	MP_RM_CONSIGN_LOG_SIZE_ACK,
	MP_RM_CONSIGN_LOG_SIZE_NACK,
	MP_RM_CONSIGN_LOG_STOP_SYN,
	MP_RM_CONSIGN_LOG_STOP_ACK,
};

enum
{
	MP_RM_GAMEROOM_POINT_LOG_NACK_BY_AUTH,
	MP_RM_GAMEROOM_POINT_LOG_ACK,
	MP_RM_GAMEROOM_POINT_LOG_SIZE_SYN,
	MP_RM_GAMEROOM_POINT_LOG_SIZE_ACK,
	MP_RM_GAMEROOM_POINT_LOG_SIZE_NACK,
	MP_RM_GAMEROOM_POINT_LOG_STOP_SYN,
	MP_RM_GAMEROOM_POINT_LOG_STOP_ACK,
};

enum
{
	MP_RM_FORBID_CHAT_LOG_NACK_BY_AUTH,
	MP_RM_FORBID_CHAT_LOG_ACK,
	MP_RM_FORBID_CHAT_LOG_SIZE_SYN,
	MP_RM_FORBID_CHAT_LOG_SIZE_ACK,
	MP_RM_FORBID_CHAT_LOG_SIZE_NACK,
	MP_RM_FORBID_CHAT_LOG_STOP_SYN,
	MP_RM_FORBID_CHAT_LOG_STOP_ACK,
};

enum RecoveryLog
{
	// 플레이어
	RecoveryLogPlayer				= 100,
	RecoveryLogSetPlayer,
	RecoveryLogSetExtendedPlayer,
	RecoveryLogSetLicense,
	RecoveryLogSetFishing,

	// 아이템
	RecoveryLogItem					= 200,
	RecoveryLogSetItem,
	RecoveryLogSetItemOption,
	RecoveryLogRemoveItem,
	RecoveryLogRemoveItemOption,
	RecoveryLogAddItem,
	RecoveryLogUpdateItemEndTime,
	RecoveryLogUpdateItemRemainTime,
	RecoveryLogSetItemStorage,
	RecoveryLogRemoveItemdDropOption,	// 080320 LUJ, 드롭 옵션 삭제
	RecoveryLogUpdatePet,				// 080716 LUJ, 펫 정보 변경
	RecoveryLogRemovePet,				// 080731 LUJ, 펫 삭제
	RecoveryLogAddPet,					// 080731 LUJ, 펫 추가
	
	// 스킬
	RecoveryLogSkill				= 300,
	RecoveryLogAddSkill,
	RecoveryLogRemoveSkill,
	RecoveryLogSetSkill,
	RecoveryLogRemoveBuff,
	// 패밀리
	RecoveryLogFamily				= 400,
	RecoveryLogSetFamily,

	// 퀘스트
	RecoveryLogQuest				= 500,
	RecoveryLogRemoveQuest,
	RecoveryLogFinishSubQuest,
	RecoveryLogSetQuestItem,
	RecoveryLogRemoveQuestItem,
	RecoveryLogAddQuestItem,

	// 길드
	RecoveryLogGuild				= 600,
	RecoveryLogSetGuild,
	RecoveryLogSetGuildRank,
	RecoveryLogKickGuildMember,
	RecoveryLogRemoveGuildSkill,

	// 사용자
	RecoveryLogUser					= 700,
	RecoveryLogSetUser,
	RecoveryLogAddPlayerToUser,
	RecoveryLogRemovePlayerFromUser,
	RecoveryLogRestorePlayerToUser,
	RecoveryLogUpdatePunishData,

	// 차단한 IP대역내에서 특정 IP 허용
	RecoveryPermission				= 800,
	RecoveryPermissionAddIp,
	RecoveryPermissionRemoveIp,

	// 운영
	RecoveryOperator				= 900,
	RecoveryOperatorAdd,
	RecoveryOperatorRemove,
	RecoveryOperatorSet,
	RecoveryOperatorAddIp,
	RecoveryOperatorRemoveIp,
};


// 080403 LUJ, 유료 아이템 구입 로그
enum ItemShopLog
{
	ItemShopLogBuy			= 1,	// 아이템 구입
	ItemShopLogLuckyBuy		= 2,	// 행운상자
	ItemShopLogLucky77Buy	= 3,	// 행운상자 77
	ItemShopLogRegular		= 4,	// 정기 획득
	ItemShopLogVote			= 5,	// 081013 LUJ, 설문조사
	ItemShopLogEvent		= 6,	// 081015 LUJ, 이벤트로 획득
};


// 키워드로 유저 목록을 요청한다.
struct MSG_RM_OPEN_SYN : public MSGBASE
{
	enum Type
	{
		LoginId,		// 로그인 ID로 검색
		RealName,		// 이름으로 검색
		PlayerIndex,	// 플레이어 DB 번호로 검색
		PlayerName,		// 캐릭터 명으로 검색
		UserIndex,		// 사용자 번호로 검색
	}
	mType;

	char mKeyword[ MAX_NAME_LENGTH + 1 ];
};


// 검색된 유저 결과를 반환한다
struct MSG_RM_OPEN_ACK : public MSGBASE
{
	DWORD	mSize;
	BOOL	mIsMore;	// 검색결과가 용량보다 많을 경우 세팅됨

	struct Data
	{
		DWORD	mUserIndex;
		char	mUserName[ MAX_NAME_LENGTH + 1 ];
		DWORD	mPlayerIndex;
		char	mPlayerName[ MAX_NAME_LENGTH + 1 ];
		DWORD	mStandIndex;
	}
	mData[ MAX_ROW_NUM ];

	int GetSize() const
	{
		return sizeof( MSG_RM_OPEN_ACK ) - ( sizeof( mData ) / sizeof( *mData ) - mSize ) * sizeof( *mData );
	}
};


// 플레이어의 상세 정보를 요청한다
struct MSG_RM_GET_PLAYER_SYN : public MSGBASE
{
	DWORD	mPlayerIndex;
	char	mPlayerName[ MAX_NAME_LENGTH + 1 ];
};


// 해당 플레이어의 상세 정보를 반환한다
// 이 구조체에는 숫자 정보만 담는다. 문자열 등의 정보는 쿼리 타입이 다르므로
// MSG_RM_GET_PLAYER_DATA_EXTENDED를 사용한다.
struct MSG_RM_PLAYER_DATA : public MSGBASE
{
	struct Player
	{
		char	mName[ MAX_NAME_LENGTH + 1 ];
		DWORD	mIndex;
		DWORD	mStrength;
		DWORD	mDexterity;
		DWORD	mVitality;
		DWORD	mIntelligence;
		DWORD	mWisdom;
		DWORD	mLife;
		DWORD	mMana;
		DWORD	mGrade;
		DWORD	mMaxGrade;
		DWORD	mExp;
		DWORD		mGradeUpPoint;
		MONEYTYPE	mMoney;
		DWORD	mPlayTime;

		DWORD	mSkillPoint;
		DWORD	mRace;
		DWORD	mExtendedInventorySize;
		DWORD	mBadFame;
	}
	mPlayer;

	struct Position
	{
		MAPTYPE	mMap;
		DWORD	mX;
		DWORD	mY;
	}
	mPosition;

	struct Job
	{
		DWORD	mIndex;
		DWORD	mData1;
		DWORD	mData2;
		DWORD	mData3;
		DWORD	mData4;
		DWORD	mData5;
		DWORD	mData6;
	}
	mJob;

	struct Guild
	{
		DWORD	mIndex;		
		DWORD	mRank;
		char	mName[ MAX_GUILD_NAME + 1 ];
	}
	mGuild;
};


//typedef MSG_RM_GET_PLAYER_DATA MSG_RM_SET_PLAYER_DATA;


// 플레이어의 확장 정보를 반환한다. MIDDLE QUERY를 이용하므로 필드가 20개를 넘으면 안된다
struct MSG_RM_PLAYER_EXTENDED_DATA : public MSGBASE
{
	struct Player
	{
		DWORD mIndex;
		char mName[MAX_NAME_LENGTH + 1];
		char mGuildNick[MAX_NAME_LENGTH + 1];
		char mFamilyNick[MAX_NAME_LENGTH + 1];
		char mLastPlayedTime[MAX_CHAT_LENGTH + 1];
		BOOL mGender;
		RaceType mRace;
	}
	mPlayer;

	struct History
	{
		BOOL	mIsEnable;
		char	mCreateDate		[ MAX_CHAT_LENGTH + 1 ];
		char	mCreateIp		[ MAX_NAME_LENGTH + 1 ];
		char	mDeletedDate	[ MAX_CHAT_LENGTH + 1 ];
		char	mDeletedIp		[ MAX_NAME_LENGTH + 1 ];
		char	mRecoveredDate	[ MAX_DATE_SIZE + 1 ];
		char	mRecoveredIp	[ MAX_NAME_LENGTH + 1 ];
		// 081020 LUJ, 몬스터 미터 정보 쿼리
		DWORD	mMonsterKillCount;
		DWORD	mPlayTimeTotal;
	}
	mHistory;	
};


struct MSG_RM_PLAYER_LOVE : public MSGBASE
{
	DATE_MATCHING_INFO	mDateMatching;
};


// 아이템 정보를 반환한다
struct MSG_RM_ITEM : public MSGBASE
{
	DWORD		mSize;
	Item		mData[ MAX_ROW_NUM  ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_ITEM ) - ( sizeof( mData ) / sizeof( *mData ) - mSize ) * sizeof( *mData );
	}
};


// 옵션 정보를 가져온다
struct MSG_RM_ITEM_OPTION : public MSGBASE
{
	size_t		mSize;
	// 081021 LUJ, 오버플로 문제로 MAX_ITEM_OPTION_NUM -> MAX_ROW_NUM을 사용
	ITEM_OPTION mData[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_ITEM_OPTION ) - ( sizeof( mData ) / sizeof( ITEM_OPTION ) - mSize ) * sizeof( ITEM_OPTION );
	}
};


// 아이템 정보를 갱신한다
struct MSG_RM_ITEM_UPDATE : public MSGBASE
{
	ITEMBASE mItem;
};


// 아이템의 옵션 정보를 갱신한다(추가/삭제/변경)
struct MSG_RM_ITEM_UPDATE_OPTION : public MSGBASE
{
	ITEM_OPTION	mOption;
};


struct MSG_RM_ITEM_OWNER : public MSGBASE
{
	DWORD mItemIndex;
	size_t mSize;

	struct Player
	{
		DWORD mIndex;
		char mName[ MAX_NAME_LENGTH + 1 ];
		DWORD mSlotSize;
		DURTYPE mQuantity;
		DWORD mUserIndex;
	}
	mPlayer[ MAX_ROW_NUM ];	

	size_t GetSize() const
	{
		return sizeof( MSG_RM_ITEM_OWNER ) - ( sizeof( mPlayer ) / sizeof( *mPlayer ) - mSize ) * sizeof( *mPlayer );
	}
};


struct MSG_RM_SKILL : public MSGBASE
{
	DWORD		mSize;
	SKILL_BASE	mData[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_SKILL ) - ( sizeof( mData ) / sizeof( *mData ) - mSize ) * sizeof( *mData );
	}
};


// 080731 LUJ, 버프를 보관한다
struct MSG_RM_SKILL_BUFF : public MSGBASE
{
	DWORD mSize;

	struct Buff
	{
		DWORD	mSkillIndex;
		DWORD	mRemainTime;
		DWORD	mRemainCount;
		char	mEndDate[ MAX_DATE_SIZE + 1 ];
	}
	mBuff[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_SKILL_BUFF ) - ( sizeof( mBuff ) / sizeof( *mBuff ) - mSize ) * sizeof( *mBuff );
	}
};


struct MSG_RM_FAMILY_LIST : public MSGBASE
{
	size_t mSize;

	struct Family
	{
		DWORD	mIndex;
		char	mName[ MAX_NAME_LENGTH + 1 ];
		DWORD	mSize;
		DWORD	mMasterIndex;
	}
	mFamily[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_FAMILY_LIST ) - ( sizeof( mFamily ) / sizeof( *mFamily ) - mSize ) * sizeof( *mFamily );
	}
};


struct MSG_RM_FAMILY_DATA : public MSGBASE
{
	DWORD	mIndex;
	char	mName[ MAX_NAME_LENGTH + 1 ];
	DWORD	mHonorPoint;
	BOOL	mIsEnableNick;
	char	mBuildDate[ MAX_DATE_SIZE + 1 ];
};


struct MSG_RM_FAMILY_MEMBER : public MSGBASE
{
	size_t mSize;

	struct Player
	{
		DWORD	mIndex;
		char	mNick[ MAX_NAME_LENGTH + 1 ];
		char	mName[ MAX_NAME_LENGTH + 1 ];
		DWORD	mLevel;
		BOOL	mIsMaster;
	}
	mPlayer[ 50 ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_FAMILY_MEMBER ) - ( sizeof( mPlayer ) / sizeof( Player ) - mSize ) * sizeof( Player );
	}
};


struct MSG_RM_FAMILY_FARM : public MSGBASE
{
	DWORD	mZone;
	DWORD	mFarmIndex;
	DWORD	mState;
	DWORD	mOwnerIndex;
	DWORD	mGardenGrade;
	DWORD	mHouseGrade;
	DWORD	mWarehouseGrade;
	DWORD	mCageGrade;
	DWORD	mFenceGrade;
	char	mBuildDate[ MAX_DATE_SIZE + 1 ];
};


struct MSG_RM_FAMILY_CROP : public MSGBASE
{
	size_t mSize;

	struct Crop
	{
		DWORD	mIndex;
        DWORD	mOwnerIndex;
		char	mOwnerName[ MAX_NAME_LENGTH + 1 ];
		DWORD	mKind;
		DWORD	mStep;
		DWORD	mLife;
		DWORD	mNextStepTime;
		DWORD	mSeedGrade;
	}
	mCrop[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_FAMILY_CROP ) - ( sizeof( mCrop ) / sizeof( Crop ) - mSize ) * sizeof( Crop );
	}
};


struct MSG_RM_GUILD_LIST : public MSGBASE
{
	size_t mSize;

	struct Guild
	{
		DWORD	mIndex;
		char	mName[ MAX_NAME_LENGTH + 1 ];
		DWORD	mSize;
		DWORD	mLevel;
		DWORD	mMoney;	// 선포금
	}
	mGuild[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_GUILD_LIST ) - ( sizeof( mGuild ) / sizeof( Guild ) - mSize ) * sizeof( Guild );
	}
};


struct MSG_RM_GUILD_DATA : public MSGBASE
{	
	DWORD	mGuildIndex;
	DWORD	mLevel;
	DWORD	mScore;
	DWORD	mMoney;
	char	mGuildName[ MAX_GUILD_NAME + 1 ];
	CHAR	mBuiltDate[ MAX_DATE_SIZE + 1 ];
	char	mAllianceName[ MAX_GUILD_NAME + 1 ];
	DWORD	mAllianceEntryDate;
};


struct MSG_RM_GUILD_STORE : public MSGBASE
{
	size_t mSize;

	struct Item
	{
		DWORD	mDbIndex;
		DWORD	mIndex;
		DWORD	mQuantity;
	}
	mItem[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_GUILD_STORE ) - ( sizeof( mItem ) / sizeof( Item ) - mSize ) * sizeof( Item );
	}
};


struct MSG_RM_GUILD_MEMBER : public MSGBASE
{
	size_t mSize;

	struct Player
	{
		DWORD	mIndex;
		char	mName[ MAX_NAME_LENGTH + 1 ];
		DWORD	mLevel;
		DWORD	mRank;
		char	mDate[ MAX_DATE_SIZE ];
	}
	mPlayer[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_GUILD_MEMBER ) - ( sizeof( mPlayer ) / sizeof( Player ) - mSize ) * sizeof( Player );
	}
};


struct MSG_RM_LOG_REQUEST : public MSGBASE
{
	// 클라이언트가 로그를 요청한 시간. 이 수치가 일치하지 않는 로그는 클라이언트에서 무시한다
	DWORD	mRequestTick;
	char	mBeginDate[ MAX_DATE_SIZE ];
	char	mEndDate[ MAX_DATE_SIZE ];
};

struct MSG_RM_CONSIGN_LOG_REQUEST : public MSG_RM_LOG_REQUEST
{
	DWORD	mItemDBIndex;
	DWORD	mConsignIndex;
};


struct MSG_RM_LOG_SIZE : public MSGROOT
{
	DWORD	mRequestTick;
	size_t	mSize;
};


struct MSG_RM_EXP_LOG : public MSGBASE
{
	size_t mSize;

	struct Log
	{
		DWORD			mLogIndex;
		// 080416 LUJ, 경험치 타입으로 변경
		// 080716 LUJ, 잘못된 타입을 수정
		eLogExppoint	mType;
		DWORD			mGrade;
		DWORD			mVariation;
		DWORD			mKillerKind;
		DWORD			mKillerIndex;
		double			mExperience;
		char			mDate[ MAX_DATE_SIZE ];
		// 080716 LUJ, 경험치 보호 회수 저장
		DWORD			mCount;
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_EXP_LOG ) - ( sizeof( mLog ) / sizeof( Log ) - mSize ) * sizeof( Log );
	}
};


struct MSG_RM_ITEM_LOG_REQUEST : public MSG_RM_LOG_REQUEST
{
	DWORD	mItemIndex;
	DWORD	mItemDbIndex;
	DWORD	mPlayerIndex;
};


struct MSG_RM_ITEM_LOG : public MSGBASE
{
	DWORD	mRequestTick;
	size_t	mSize;

	struct Player
	{
		DWORD	mIndex;
		char	mName[ MAX_NAME_LENGTH + 1 ];
		DWORD	mMoney;
	};

	struct Item
	{
		DWORD	mIndex;
		DWORD	mDbIndex;
		DWORD	mQuantity;
		DWORD	mFromPosition;
		DWORD	mToPosition;
	};

	struct Log
	{
		DWORD	mLogIndex;

		eLogitemmoney	mType;
		
		Player	mFromPlayer;
		Player	mToPlayer;
		
		DWORD	mMoneyVariation;

		Item	mItem;

		char	mDate[ MAX_DATE_SIZE + 1 ];
		DWORD	mExperience;
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_ITEM_LOG ) - ( sizeof( mLog ) / sizeof( Log ) - mSize ) * sizeof( Log );
	}
};


// 080716 LUJ, 전역 아이템 로그 검색 결과가 담기는 자료형
struct MSG_RM_ITEM_GLOBAL_LOG : public MSGBASE
{
	DWORD	mRequestTick;
	size_t	mSize;

	typedef MSG_RM_ITEM_LOG::Player	Player;
	typedef MSG_RM_ITEM_LOG::Item	Item;
	
	struct Log : public MSG_RM_ITEM_LOG::Log
	{
		// 080716 LUJ, 중복되는 쿼리를 막기 위해 로그 인덱스와 함께 키로 사용한다
		DWORD mKey;
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_ITEM_GLOBAL_LOG ) - ( sizeof( mLog ) / sizeof( Log ) - mSize ) * sizeof( Log );
	}
};


struct MSG_RM_STAT_LOG : public MSGBASE
{
	size_t mSize;

	struct Log
	{
		DWORD	mLogIndex;
		DWORD	mLevel;
		DWORD	mStrength;
		DWORD	mDexterity;
		DWORD	mVitality;
		DWORD	mIntelligence;
		DWORD	mWisdom;
		DWORD	mExperience;
		char	mDate[ MAX_DATE_SIZE + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_STAT_LOG ) - ( sizeof( mLog ) / sizeof( Log ) - mSize ) * sizeof( Log );
	}
};


struct MSG_RM_SKILL_LOG : public MSGBASE
{
	size_t mSize;

	struct Log
	{
		DWORD		mLogIndex;
		eLogSkill	mType;
		DWORD		mDbIndex;
		DWORD		mIndex;
		DWORD		mLevel;
		DWORD		mPoint;
		char		mDate[ MAX_DATE_SIZE + 1 ];
		char		mMemo[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_SKILL_LOG ) - ( sizeof( mLog ) / sizeof( Log ) - mSize ) * sizeof( Log );
	}
};


struct MSG_RM_ITEM_OPTION_LOG : public MSGBASE
{
	size_t mSize;

	struct Log
	{
		DWORD			mLogIndex;
		DWORD			mItemDbIndex;
		eLogitemmoney	mType;

		ITEM_OPTION	mOption;

		char		mDate[ MAX_DATE_SIZE + 1 ];
		char		mMemo[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_ITEM_OPTION_LOG ) - ( sizeof( mLog ) / sizeof( Log ) - mSize ) * sizeof( Log );
	}
};

// 081205 LUJ, 길드 로그를 플레이어 번호로도 검색할 수 있도록 요청 구조체 추가
struct MSG_RM_GUILD_LOG_REQUEST : public MSG_RM_LOG_REQUEST
{
	DWORD mGuildIndex;
	DWORD mPlayerIndex;
};

struct MSG_RM_GUILD_LOG : public MSGBASE
{
	DWORD	mSize;

	struct Log
	{
		DWORD		mLogIndex;
		eGuildLog	mType;
		DWORD		mPlayerIndex;
		DWORD		mValue1;
		DWORD		mValue2;
		char		mDate[ MAX_DATE_SIZE + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_GUILD_LOG ) - ( sizeof( mLog ) / sizeof( Log ) - mSize ) * sizeof( Log );
	}
};


struct MSG_RM_GUILD_WAREHOUSE_LOG : public MSGBASE
{
	size_t mSize;

	struct Log
	{
		DWORD			mLogIndex;
		DWORD			mPlayerIndex;
		POSTYPE			mItemFromPosition;
		ITEMBASE		mItem;
		MONEYTYPE		mMoney;
		eLogitemmoney	mType;
		char			mDate[ MAX_DATE_SIZE + 1 ];
		char			mMemo[ MAX_CHAT_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_GUILD_WAREHOUSE_LOG ) - ( sizeof( mLog ) / sizeof( Log ) - mSize ) * sizeof( Log );
	}
};


struct MSG_RM_USER_DATA : public MSGBASE
{
	DWORD	mIndex;
	char	mId[ MAX_NAME_LENGTH + 1 ];
	char	mEnpangName[MAX_NAME_LENGTH + 1];
	char	mName[ MAX_NAME_LENGTH + 1 ];
	char	mJoinedDate[ MAX_NAME_LENGTH + 1 ];
	char	mLogedoutDate[ MAX_NAME_LENGTH + 1 ];
	DWORD	mPlayTime;
	DWORD	mLevel;
	// 080725 LUJ, 상태 값 추가
	DWORD	mState;
	// 090618 ShinJS --- Auto Punish 상태 추가
	DWORD	mAutoPunishKind;
	char	mAutoPunishStartDate[ MAX_NAME_LENGTH + 1 ];
	char	mAutoPunishEndDate[ MAX_NAME_LENGTH + 1 ];
	DWORD	mAutoPunishTime;
	
	struct Connection
	{
		char	mIp[ MAX_NAME_LENGTH + 1 ];
		DWORD	mAgent;
	}
	mConnection;
};


struct MSG_RM_USER_PLAYER_LIST : public MSGBASE
{
	size_t mSize;

	struct Player
	{
		DWORD	mIndex;
		DWORD	mLevel;
		char	mName[ MAX_NAME_LENGTH + 1 ];
	}
	mPlayer[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_USER_PLAYER_LIST ) - ( sizeof( mPlayer ) / sizeof( mPlayer[ 0 ] ) - mSize ) * sizeof( mPlayer[ 0 ] );
	}
};


struct MSG_RM_JOB_LOG : public MSGBASE
{
	size_t mSize;

	struct Log
	{
		DWORD	mLogIndex;
		DWORD	mClassIndex;
		DWORD	mJobGrade;
		DWORD	mJobIndex;
		char	mDate[ MAX_DATE_SIZE + 1 ];
		char	mMemo[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_JOB_LOG ) - ( sizeof( mLog ) / sizeof( mLog[ 0 ] ) - mSize ) * sizeof( mLog[ 0 ] );
	}
};


struct MSG_RM_GUILD_SCORE_LOG : public MSGBASE
{
	size_t mSize;

	struct Log
	{
		DWORD	mLogIndex;
		eGuildLog	mType;
		DWORD	mScore;
		//DWORD	mLoginMember;
		//DWORD	mMonsterHunt;
		char	mDate[ MAX_DATE_SIZE + 1 ];
		char	mMemo[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_GUILD_SCORE_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


struct MSG_RM_FAMILY_POINT_LOG : public MSGBASE
{
	size_t mSize;

	struct Log
	{
		DWORD		mLogIndex;
		eFamilyLog	mType;
		DWORD		mPoint;
		DWORD		mMonsterExp;
		DWORD		mLoginMember;
		char		mDate[ MAX_DATE_SIZE + 1 ];
		char		mMemo[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_FAMILY_POINT_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


struct MSG_RM_FAMILY_CROP_LOG : public MSGBASE
{
	size_t mSize;

	struct Log
	{
		DWORD		mLogIndex;
		eFamilyLog	mType;
		DWORD		mFarmIndex;
		DWORD		mOwnerIndex;
		DWORD		mCropIndex;
		DWORD		mCropKind;
		DWORD		mCropStep;
		DWORD		mCropLife;
		DWORD		mCropSeedGrade;
		char		mDate[ MAX_DATE_SIZE + 1 ];
		char		mMemo[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_FAMILY_CROP_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


struct MSG_RM_QUEST_LOG : public MSGBASE
{
	size_t mSize;

	struct Log
	{
		DWORD		mLogIndex;
		eQuestLog	mType;
		DWORD		mMainQuestIndex;
		DWORD		mSubQuestIndex;
		int			mValue1;		// 몬스터 혹은 아이템 번호
		int			mValue2;		// 몬스터 사냥 수 혹은 아이템 개수
		char		mDate[ MAX_DATE_SIZE + 1 ];
		char		mMemo[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_QUEST_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


struct MSG_RM_OPERATOR : public MSGBASE
{
	size_t mSize;

	struct Operator
	{
		DWORD		mIndex;
		char		mId				[ MAX_NAME_LENGTH + 1 ];
		char		mName			[ MAX_NAME_LENGTH + 1 ];
		char		mIp				[ MAX_NAME_LENGTH + 1 ];
		char		mRegistedDate	[ MAX_NAME_LENGTH + 1 ];
		eGM_POWER	mPower;
	}
	mOperator[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_OPERATOR ) - ( sizeof( mOperator ) / sizeof( *mOperator ) - mSize ) * sizeof( *mOperator );
	}
};


struct MSG_RM_OPERATOR_IP : public MSGBASE
{
	size_t mSize;

	struct Address
	{
		char mIp[ MAX_NAME_LENGTH + 1 ];
	}
	mAddress[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_OPERATOR_IP ) - ( sizeof( mAddress ) / sizeof( *mAddress ) - mSize ) * sizeof( *mAddress );
	}
};


struct MSG_RM_OPERATOR_LOG : public MSGROOT
{
	DWORD	mRequestTick;
	size_t	mSize;
	
	struct Log
	{
		int			mIndex;
		RecoveryLog	mType;
		char		mDate		[ MAX_NAME_LENGTH + 1 ];
		char		mOperator	[ MAX_NAME_LENGTH + 1 ];
		char		mKey1		[ MAX_NAME_LENGTH + 1 ];
		char		mValue1		[ MAX_NAME_LENGTH + 1 ];
		char		mKey2		[ MAX_NAME_LENGTH + 1 ];
		char		mValue2		[ MAX_NAME_LENGTH + 1 ];
		char		mKey3		[ MAX_NAME_LENGTH + 1 ];
		char		mValue3		[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_OPERATOR_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


struct MSG_RM_NAME_LOG : public MSGROOT
{
	DWORD mRequestTick;
	DWORD mSize;
	
	struct Log
	{
		DWORD	mIndex;
		char	mPreviousName	[ MAX_NAME_LENGTH + 1 ];
		char	mName			[ MAX_NAME_LENGTH + 1 ];
		char	mDate			[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_NAME_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


// 080303 LUJ, 아이템 추가 메시지 형식
struct MSG_RM_ITEM_ADD : public MSGROOT
{
	DWORD	mUserIndex;
	DWORD	mPlayerIndex;
	Item	mItem;
};


// 080311 LUJ, 로그인 메시지
struct MSG_RM_LOGIN : public MSGROOT
{
	char	mId		[ MAX_NAME_LENGTH + 1 ];
	char	mPass	[ MAX_NAME_LENGTH + 1 ];
	
	struct Version
	{
		int mMajor;
		int mMinor;
	}
	mVersion;
};


// 080401 LUJ, 농장 로그
struct MSG_RM_FARM_LOG : public MSGROOT
{
	DWORD mRequestTick;
	DWORD mSize;

	struct Log
	{
		DWORD		mIndex;
		DWORD		mZone;
		DWORD		mFarmIndex;
		eFamilyLog	mKind;
		DWORD		mPlayerIndex;
		DWORD		mValue1;
		DWORD		mValue2;
		char		mDate[ MAX_DATE_SIZE + 1 ];
	}
	mLog[ MAX_ROW_NUM ];
	
	size_t GetSize() const
	{
		return sizeof( MSG_RM_FARM_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


// 080403 LUJ, 유료 아이템 구입 로그
struct MSG_RM_ITEM_SHOP_LOG : public MSGROOT
{
	DWORD mRequestTick;
	DWORD mSize;

	struct Log
	{
		DWORD		mIndex;
		ItemShopLog	mType;
		char		mDate[ MAX_DATE_SIZE + 1 ];
		
		struct User
		{
			DWORD	mIndex;
			char	mId[ MAX_NAME_LENGTH + 1 ];
		}
		mUser;

		struct Item
		{
			DWORD mIndex;
			DWORD mDbIndex;
			DWORD mQuantity;
		}
		mItem;
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_ITEM_SHOP_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


// 080523 LUJ, 낚시 로그
struct MSG_RM_FISH_LOG : public MSGROOT
{
	DWORD mRequestTick;
	DWORD mSize;

	struct Log
	{
		DWORD		mIndex;
		eFishingLog	mType;
		char		mDate[ MAX_DATE_SIZE + 1 ];
		DWORD		mPlayerIndex;
		int			mVariant;
		int			mPoint;
		DWORD		mBuyItemIndex;
		DWORD		mUseItemIndex;
		// 080808 LUJ, 경험치/레벨 추가
		EXPTYPE		mExperience;
		DWORD		mLevel;
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_FISH_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


// 080630 LUJ, 가축 로그
struct MSG_RM_LIVESTOCK_LOG : public MSGBASE
{
	DWORD mRequestTick;
	DWORD mSize;

	struct Log
	{
		DWORD		mIndex;
		eFamilyLog	mType;
		DWORD		mFarmIndex;
		char		mDate[ MAX_DATE_SIZE + 1 ];
		char		mMemo[ MAX_NAME_LENGTH + 1 ];

		struct Animal
		{
			DWORD mOwnerIndex;
			DWORD mIndex;
			DWORD mKind;
			DWORD mStep;
			DWORD mLife;
			DWORD mContentment;
			DWORD mInterest;
		}
		mAnimal;
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_LIVESTOCK_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


// 080731 LUJ, 가축 정보
struct MSG_RM_LIVESTOCK : public MSGBASE
{
	DWORD	mSize;

	struct Animal : public MSG_RM_LIVESTOCK_LOG::Log::Animal
	{
		char	mOwnerName[ MAX_NAME_LENGTH + 1 ];
		DWORD	mNextStepTime;
	}
	mAnimal[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_LIVESTOCK ) - ( sizeof( mAnimal ) / sizeof( *mAnimal ) - mSize ) * sizeof( *mAnimal );
	}
};


// 080716 LUJ, 펫 정보 목록
struct MSG_RM_PET : public MSGBASE
{
	DWORD mSize;

	struct Pet
	{
		DWORD		mIndex;
		DWORD		mItemDbIndex;
		DWORD		mKind;
		DWORD		mLevel;
		DWORD		mGrade;
		DWORD		mSkillSlot;
		DWORD		mExperience;
		DWORD		mFriendship;
		DWORD		mHealth;
		DWORD		mMaxHealth;
		DWORD		mMana;
		DWORD		mMaxMana;
		DWORD		mArtificialIntelligence;
		ePetState	mState;
		ePetType	mType;
	}
	mData[ MAX_MIDDLEROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_PET ) - ( sizeof( mData ) / sizeof( *mData ) - mSize ) * sizeof( *mData );
	}
};


// 080716 LUJ, 펫 로그
struct MSG_RM_PET_LOG : public MSGBASE
{
	DWORD mRequestTick;
	DWORD mSize;

	struct Pet
	{
		DWORD		mID;
		DWORD		mExperience;
		DWORD		mFriendship;
		DWORD		mGrade;
		DWORD		mLevel;
		DWORD		mKind;
		ePetState	mState;
		DWORD		mSkillSlot;
	};

	struct Log
	{
		DWORD	mIndex;
		ePetLog	mType;
		Pet		mPet;
		char	mDate[ MAX_DATE_SIZE + 1 ];
		char	mMemo[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_PET_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};


// 080716 LUJ, 펫 로그 요청
struct MSG_RM_PET_LOG_REQUEST : public MSG_RM_LOG_REQUEST
{
	DWORD	mPetIndex;
};


// 080716 LUJ, 아이템 로그를 하루 단위가 아닌 기간으로 검색하는 요청
struct MSG_RM_ITEM_GLOBAL_LOG_REQUEST : public MSG_RM_LOG_REQUEST
{
	eLogitemmoney	mLogType;
	DWORD			mItemIndex;
	DWORD			mItemDbIndex;
	DWORD			mPlayerIndex;
};

// 081021 LUJ, 권한 설정 요청
struct MSG_RM_AUTHORITY : public MSGBASE
{
	DWORD mSize;

	struct Authority
	{
		eGM_POWER		mPower;
		AuthorityType	mType;
		bool			mReadable;
		bool			mWriteable;
	}
	mAuthority[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_AUTHORITY ) - ( sizeof( mAuthority ) / sizeof( *mAuthority ) - mSize ) * sizeof( *mAuthority );
	}
};

// 081027 LUJ, 토너먼트 참가한 길드 목록
struct MSG_RM_TOURNAMENT_GUILD : public MSGBASE
{
	DWORD	mTournamentCount;
	DWORD	mSize;

	struct Guild
	{
		DWORD	mIndex;
		char	mName[ MAX_GUILD_NAME + 1 ];
		DWORD	mPosition;
		DWORD	mRound;
	}
	mGuild[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_TOURNAMENT_GUILD ) - ( sizeof( mGuild ) / sizeof( *mGuild ) - mSize ) * sizeof( *mGuild );
	}
};

// 081027 LUJ, 토너먼트 참가한 플레이어 목록
struct MSG_RM_TOURNAMENT_PLAYER : public MSGBASE
{
	DWORD	mGuildIndex;
	DWORD	mTournamentCount;
	DWORD	mSize;
	
	struct Player
	{
		DWORD	mIndex;
		char	mName[ MAX_NAME_LENGTH + 1 ];
	}
	mPlayer[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_TOURNAMENT_PLAYER ) - ( sizeof( mPlayer ) / sizeof( *mPlayer ) - mSize ) * sizeof( *mPlayer );
	}
};

// 081027 LUJ, 공성 로그
struct MSG_RM_SIEGE_LOG : public MSGBASE
{
	DWORD mRequestTick;
	DWORD mSize;

	struct Log
	{
		DWORD		mIndex;
		eSiegeLog	mType;
		DWORD		mGuildIndex;
		DWORD		mPlayerIndex;
		char		mMemo[ MAX_NAME_LENGTH + 1 ];
		char		mDate[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( MSG_RM_SIEGE_LOG ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};

// 081205 LUJ, 채팅 로그
struct MSG_RM_CHAT_LOG : public MSGBASE
{
	DWORD mRequestTick;
	DWORD mSize;

	struct Log
	{
		DWORD	mIndex;
		DWORD	mPlayerIndex;
		char	mChat[ MAX_CHAT_LENGTH + 1 ];
		char	mDate[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_MIDDLEROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( *this ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};

// 081205 LUJ, 패밀리 로그 요청
struct MSG_RM_FAMILY_LOG_REQUEST : public MSG_RM_LOG_REQUEST
{
	DWORD mFamilyIndex;
	DWORD mPlayerIndex;
};

// 081205 LUJ, 패밀리 로그
struct MSG_RM_FAMILY_LOG : public MSGBASE
{
	DWORD mRequestTick;
	DWORD mSize;

	struct Log
	{
		DWORD		mIndex;
		eFamilyLog	mType;
		DWORD		mFamilyIndex;
		DWORD		mPlayerIndex;
		char		mDate[ MAX_NAME_LENGTH + 1 ];
		char		mMemo[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( *this ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};

// 090122 LUJ, 스크립트 해킹 로그
struct MSG_RM_SCRIPT_HACK_LOG : public MSGBASE
{
	DWORD mRequestTick;
	DWORD mSize;

	struct Log
	{
		DWORD	mIndex;
		DWORD	mUserIndex;
		DWORD	mPlayerIndex;
		char	mDate[ MAX_NAME_LENGTH + 1 ];
		char	mScriptName[ MAX_NAME_LENGTH + 1 ];
		char	mMemo[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( *this ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};

// 090122 LUJ, 이름 변경 로그 검색 요청
struct MSG_RM_RENAME_LOG_REQUEST : public MSG_RM_LOG_REQUEST
{
	char	mKeyword[ MAX_NAME_LENGTH + 1 ];
};

// 090406 LUJ, 돈 로그
struct MSG_RM_MONEY_LOG : public MSGBASE
{
	DWORD mRequestTick;
	DWORD mSize;
	struct Player
	{
		DWORD mIndex;
		MONEYTYPE mInventoryMoney;
	};
	struct Log
	{
		DWORD mIndex;
		// 090826 ShinJS --- 돈 로그 Type 오류 수정
		eMoneyLogtype mType;
		MONEYTYPE mMarginMoney;
		Player mSourcePlayer;
		Player mTargetPlayer;
		char mDate[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( *this ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};

// 090609 ShinJS, AutoNote 로그
struct MSG_RM_AUTONOTE_LOG : public MSGBASE
{
	DWORD mRequestTick;
	DWORD mSize;
	
	struct Log
	{
		DWORD mIndex;
		eAutoNoteLogKind mType;
		char mReportPlayer[ MAX_NAME_LENGTH + 1 ];
		DWORD mReportPlayerIndex;
		char mAutoPlayer[ MAX_NAME_LENGTH + 1 ];
		DWORD mAutoPlayerIndex;
		char mDate[ MAX_NAME_LENGTH + 1 ];
	}
	mLog[ MAX_ROW_NUM ];

	size_t GetSize() const
	{
		return sizeof( *this ) - ( sizeof( mLog ) / sizeof( *mLog ) - mSize ) * sizeof( *mLog );
	}
};

// 090618 ShinJS --- Add User Punish Data Update
struct MSG_RM_PUNISH_DATA : public MSGBASE
{
	DWORD	mIndex;
	char	mAutoPunishStartDate[ MAX_NAME_LENGTH + 1 ];
	char	mAutoPunishEndDate[ MAX_NAME_LENGTH + 1 ];
};

struct MSG_RM_HOUSE_LOG : public MSGBASE
{
	DWORD mRequestTick;
	size_t mSize;

	struct Log
	{
		DWORD mIndex;
		DWORD mUser;
		char mPlayer[MAX_NAME_LENGTH + 1];
		eHouseLog mType;
		struct Item
		{
			DWORD mIndex;
			DWORD mDbIndex;
		}
		mItem;
		DWORD mFurniture;
		DWORD mSlot;
		char mHouse[MAX_NAME_LENGTH + 1];
		DWORD mDecorationPoint;
		DWORD mHousePoint;
		char mDate[MAX_NAME_LENGTH + 1];
	}
	mLog[MAX_ROW_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mLog) / sizeof(*mLog) - mSize) * sizeof(*mLog);
	}
};

struct MSG_RM_COOK_LOG : public MSGBASE
{
	DWORD mRequestTick;
	size_t mSize;

	struct Log
	{
		DWORD mIndex;
		DWORD mPlayerIndex;
		eCookingLog mType;
		DWORD mItemIndex;
		DWORD mValue;
		char mDate[MAX_NAME_LENGTH + 1];
	}
	mLog[MAX_ROW_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mLog) / sizeof(*mLog) - mSize) * sizeof(*mLog);
	}
};

struct MSG_RM_COOK_RECIPE : public MSGBASE
{
	DWORD mRequestTick;
	size_t mSize;

	struct Recipe
	{
		DWORD mIndex;
		DWORD mSlot;
		DWORD mRemainTime;
	}
	mRecipe[MAX_ROW_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mRecipe) / sizeof(*mRecipe) - mSize) * sizeof(*mRecipe);
	}
};

struct MSG_RM_RECIPE_LOG : public MSGBASE
{
	DWORD mRequestTick;
	size_t mSize;

	struct Log
	{
		DWORD mLogIndex;
		DWORD mPlayerIndex;
		DWORD mSlot;
		DWORD mRecipeIndex;
		eCOOKRECIPE mType;
		DWORD mRemainTime;
		char mDate[MAX_DATE_SIZE + 1];
	}
	mLog[MAX_ROW_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mLog) / sizeof(*mLog) - mSize) * sizeof(*mLog);
	}
};

struct MSG_RM_HOUSE : public MSGBASE
{
	DWORD mUserIndex;
	char mHouseName[MAX_PATH];
	LEVELTYPE mExteriorKind;
	LEVELTYPE mExtendLevel;
	DWORD mTotalVisitCount;
	DWORD mDailyVisitCount;
	DWORD mDecorationPoint;
	DWORD mHousePoint;
    char mDate[MAX_DATE_SIZE + 1];
};

struct MSG_RM_FURNITURE : public MSGBASE
{
	size_t mSize;

	struct Furniture
	{
		DWORD mFurnitureIndex;
		int mItemIndex;
		VECTOR3 mPosition;
		float mAngle;
		HOUSE_HIGHCATEGORY mCategory;
		POSTYPE mSlot;
		eHOUSEFURNITURE_STATE mState;
		BOOL mIsUnableRemove;
		char mDate[MAX_DATE_SIZE];
	}
	mFurniture[MAX_ROW_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mFurniture) / sizeof(*mFurniture) - mSize) * sizeof(*mFurniture);
	}
};

struct MSG_RM_NOTE_LIST : public MSGBASE
{
	size_t mSize;

	struct Note
	{
		DWORD mIndex;
		char mTitle[MAX_NOTE_TITLE];
		char mDate[MAX_DATE_SIZE];
	}
	mNote[MAX_ROW_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mNote) / sizeof(*mNote) - mSize) * sizeof(*mNote);
	}
};

struct MSG_RM_NOTE : public MSGBASE
{
	DWORD mNoteIndex;
	char mSendedPlayer[MAX_NAME_LENGTH];
	char mNote[MAX_NOTE_LENGTH];
	BOOL mIsRead;
};

struct MSG_RM_CONSIGN_LOG : public MSGBASE
{
	DWORD mRequestTick;
	size_t mSize;

	struct Log
	{
		DWORD mLogIndex;
		eConsignmentLog mType;
		DWORD mPlayerIndex;
		DWORD mItemDbIndex;
		DWORD mItemIndex;
		DURTYPE mQuantity;
		MONEYTYPE mPrice;
		char mDate[MAX_DATE_SIZE];
		DWORD mConsignIndex;
	}
	mLog[MAX_ROW_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mLog) / sizeof(*mLog) - mSize) * sizeof(*mLog);
	}
};

struct MSG_RM_GAMEROOM_POINT_LOG : public MSGBASE
{
	DWORD mRequestTick;
	size_t mSize;

	struct Log
	{
		DWORD mLogIndex;
		eLogitemmoney mType;
		DWORD mUserIndex;
		DWORD mPoint;
		char mDate[MAX_DATE_SIZE];
	}
	mLog[MAX_ROW_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mLog) / sizeof(*mLog) - mSize) * sizeof(*mLog);
	}
};

struct MSG_RM_PARTY : public MSGBASE
{
	size_t mSize;

	struct Member
	{
		DWORD mPlayerIndex;
		DWORD mRank;
		char mName[MAX_NAME_LENGTH + 1];
	}
	mMember[MAX_MIDDLEROW_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mMember) / sizeof(*mMember) - mSize) * sizeof(*mMember);
	}
};

struct MSG_RM_FORBID_CHAT_LOG : public MSGBASE
{
	DWORD mRequestTick;
	size_t mSize;

	struct Log
	{
		DWORD mPlayerIndex;
		char m_OperatorName[MAX_NAME_LENGTH];
		char mRegisterDate[MAX_DATE_SIZE];
		char mReleaseDate[MAX_DATE_SIZE];
		char mReason[MSG_BUF_SIZE];
	}
	mLog[MAX_ROW_NUM];

	size_t GetSize() const
	{
		return sizeof(*this) - (sizeof(mLog) / sizeof(*mLog) - mSize) * sizeof(*mLog);
	}
};

#pragma pack(pop)

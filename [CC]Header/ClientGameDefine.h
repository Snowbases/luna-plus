#ifndef __CLIENTGAMEDEFINE_H__
#define __CLIENTGAMEDEFINE_H__

//------------------------------------------------------------------------------------
//
//	ClientGameDefine.h: 클라이언트 게임시스템에서만 사용하는 define, typedef, enum
//						global variable extern 도 포함한다.
//	사용 원칙:
//	1. comment를 한다
//	2. 이 파일에 사용하기에 적당한지 다시 한번 생각한다
//
//------------------------------------------------------------------------------------


enum eGameInInitKind
{
	eGameInInitKind_Login,
	eGameInInitKind_MapChange,
	eGameInInitKind_MovePos,
	eGameInInitKind_GTEnter,
	eGameInInitKind_GTLeave,
	eGameInInitKind_GTObserverEnter,
	eGameInInitKind_DungeonObserverEnter,
};

typedef WORD	SNDIDX;

#define HEROID gHeroID
extern DWORD gHeroID;
extern DWORD gUserID;

extern DWORD gCurTime;
extern DWORD gTickTime;
extern float gTickPerFrame;
extern float gAntiGravity;
extern int gChannelNum;
extern int gCheatMove;

#define MAX_PLAYEROBJECT_NUM	300
#define MAX_MONSTEROBJECT_NUM	200
#define MAX_NPCOBJECT_NUM		50
#define MAX_PETOBJECT_NUM		100
#define MAX_MAPOBJECT_NUM		20
// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.05.07
#define MAX_FARM_RENDER_OBJ_NUM	500
// E 농장시스템 추가 added by hseos 2007.05.07
// 090422 ShinJS --- 탈것 추가
#define MAX_VEHICLEOBJECT_NUM	100
#define WEAPON_ATTACH_BONENAME_R	"Bone_R"
#define WEAPON_ATTACH_BONENAME_L	"Bone_L"
#define WEAPON_ATTACH_BONENAME_BACK	"Bone_Back"

#define LEFT_HAND			"Lhand"
#define RIGHT_HAND			"Rhand"
#define LEFT_HAND_BONE		"Lhand_bone"
#define RIGHT_HAND_BONE		"Rhand_bone"
#define HEAD_BONE			"head_bone"
#define FACE_BONE			"face_bone"
#define MAX_QUEST_PAGEBTN		6

// 100611 레벨업 버튼 활성화 최소 스킬 & 스텟 포인트
#define MINIMUM_LEVELUP_POINT 1
#define MINIMUM_SKILL_POINT 20

enum
{
	eDamageKind_Front = 1,
	eDamageKind_Left,
	eDamageKind_Right,
	
	eDamageKind_Counter,
	eDamageKind_ContinueDamage,
};

enum
{
	eRangeDamageKind_SameTime = 1,
	eRangeDamageKind_FrontToBack,
	eRangeDamageKind_LeftToRight,
	eRangeDamageKind_RightToLeft,
	eRangeDamageKind_CenterToEdge,
};

// LBS 노점상 관련 03.09.30
#define STREETSTALL_ENTER_DISTANCE		200

#define _CLIENT_RESOURCE_FIELD_			// 리소스의 구분적 사용을 위해

//#define SIGNAL_COMMON_TEXT	"공지"		//KES : msg로 하는것이 일괄적?
//#define SIGNAL_ONEUSER_TEXT "알림"
//#define SIGNAL_SYSTEM_TEXT	"시스템"

//DirectInputVersion
#define DIRECTINPUT_VERSION		0x0800

enum GTournament_NameType
{
	eGTNameType_OurGuild = 0,
	eGTNameType_Enemy,
	eGTNameType_Max,
};

struct DIALOGUE
{
	WORD wType;
	DWORD dwColor;
	TCHAR str[MAX_PATH];
	WORD wLine;
};

#endif //__CLIENTGAMEDEFINE_H__

#ifndef __SERVERGAMEDEFINE_H__
#define __SERVERGAMEDEFINE_H__



#define MAX_TOTAL_PLAYER_NUM		50
#define MAX_TOTAL_PET_NUM			20
#define MAX_TOTAL_MONSTER_NUM		200
#define MAX_TOTAL_BOSSMONSTER_NUM	1
#define MAX_TOTAL_NPC_NUM			50
#define MAX_MAPOBJECT_NUM			20



#define GRID_BIT 5

extern DWORD gCurTime;
extern DWORD gTickTime;
extern float gEventRate[];
extern float gEventRateFile[];

#define _SERVER_RESOURCE_FIELD_			// 쩌짯쨔철쨍쨍?쨩 짹쨍쨘횖횉횕쨈횂 쩔쨉쩔짧

#define START_LOGIN			0
#define CHANGE_LOGIN		1

enum SERVER_KIND
{
	ERROR_SERVER,
	DISTRIBUTE_SERVER,
	AGENT_SERVER,
	MAP_SERVER,
	CHAT_SERVER,
	MURIM_SERVER,
	MONITOR_AGENT_SERVER,
	MONITOR_SERVER,
	MAX_SERVER_KIND,
};

enum CHEAT_LOG
{
	eCHT_Item,
	eCHT_Money,
	eCHT_Hide,
	eCHT_AddSkill,
	eCHT_SkillLevel,
	eCHT_LevelUp,
	eCHT_Str,
	eCHT_Dex,
	eCHT_Vit,
	eCHT_Wis,
	eCHT_Int,
};

enum ebossstate
{	
	//////////////////////////////////////////////////////////////////////////
	//NORMAL STATE
	eBossState_Stand,
	eBossState_WalkAround,
	eBossState_Pursuit,
	eBossState_RunAway,
	eBossState_Attack,

	//////////////////////////////////////////////////////////////////////////
	//EVENT STATE
	eBossState_Recover,
	eBossState_Summon,
			
	eBossState_Max,
};

enum ebossaction
{
	eBOSSACTION_RECOVER = 1,
	eBOSSACTION_SUMMON,
};

enum ebosscondition
{
	eBOSSCONDITION_LIFE = 1,	
};

//---KES PUNISH  //주의: 아래의 두 enum의 번호값은 DB로 저장이 된다. 순서를 바꾸지 마시오.
enum ePunishKind			
{
	ePunish_Login,
	ePunish_Chat,
	ePunish_Trade,
	ePunish_AutoNote,		//---오토노트 사용제한
	ePunish_Max,
};

enum ePunishCountKind
{
	ePunishCount_NoManner,
	ePunishCount_TradeCheat,		//---교환 노점등 사기
	ePunishCount_AutoUse,		//---오토사용
	ePunishCount_Max,
};

// 100104 LUJ, 몬스터 AI 이벤트
enum eStateEvent
{
	eSEVENT_DUMMY = -1,
	eSEVENT_NULL,
	eSEVENT_Process,
	eSEVENT_Message,
	eSEVENT_Enter,
	eSEVENT_Leave,
};

// 100104 LUJ, 몬스터가 도움을 요청할 때 전달하는 메시지 종류
enum enumMESSAGEKINDS
{
	eMK_HelpRequest,
	eMK_HelpShout,
};

#endif //__SERVERGAMEDEFINE_H__

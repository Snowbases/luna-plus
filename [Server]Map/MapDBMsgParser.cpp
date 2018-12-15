#include "stdafx.h"
#include "QuickSlot.h"
#include "CommonDBMsgParser.h"
#include "MapDBMsgParser.h"
#include "UserTable.h"
#include "Network.h"
#include "ItemManager.h"
#include "SkillTreeManager.h"
#include "PartyManager.h"
#include "Party.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "CharMove.h"
#include "GuildManager.h"
#include "StreetStallManager.h"
#include "CQuestBase.h"
#include "TileManager.h"
/*어빌리티 삭제 - 이영준 #include "AbilityManager.h"*/
#include "StorageManager.h"
#include "GuildFieldWarMgr.h"
#include "GuildManager.h"
#include "GuildScore.h"
#include "QuestManager.h"
#include "GuildTournamentMgr.h"
#include "GuildUnionManager.h"
#include "../[cc]skill/server/tree/SkillTree.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "Guild.h"
#include "PackedData.h"
// 081031 LUJ, 리콜위해 참조
#include "MoveManager.h"
// 081008 LYW --- MapDBMsgParser : 공성 소환매니져 호출.
#include "./SiegeRecallMgr.h"
#include "VehicleManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "../hseos/Date/SHDateManager.h"
#include "QuickManager.h"
#include "CharacterCalcManager.h"
#include "../[cc]skill/Server/Info/ActiveSkillInfo.h"
#include "../[cc]skill/Server/Object/ActiveSkillObject.h"
#include "../[cc]skill/Server/Object/BuffSkillObject.h"
#include "Trap.h"
#include "PetManager.h"
#include "pet.h"
#include "cCookManager.h"
#include "HousingMgr.h"
#include "Dungeon/DungeonMgr.h"
#include "../[cc]skill/server/info/buffskillinfo.h"
#include "./SiegeWarfareMgr.h"
#include "..\hseos\ResidentRegist\SHResidentRegistManager.h"
#include "../[CC]SiegeDungeon/SiegeDungeonMgr.h"
#include "Trigger/common.h"
#include "Trigger/Manager.h"
#include "./NpcRecallMgr.h"
#include "PCRoomManager.h"
#include "..\[CC]Quest\QuestInfo.h"
#include "ItemSlot.h"

extern int g_nServerSetNum;

DBMsgFunc g_DBMsgFunc[MaxQuery] =
{
	RCharacterNumSendAndCharacterInfo,
	RCharacterSkillTreeInfo,
	RCharacterItemInfo,
	RCharacterShopItemInfo,				// eShopItemInfoQuery
	//	RCharacterPetInfo,

	RSSItemInsert,
	
	/* New Item Work */
	NULL,						// ItemCombineUpdateToDB
	NULL,						// ItemMoveUpdateToDB
	NULL,						// ItemMoveStorageUpdate
	NULL,						// ItemMoveGuildUpdate
	RItemInsert,				// ItemInsertToDB
	NULL,						// ItemDeleteToDB
	NULL,						// RegistLoginMapInfo
	NULL,						// UnRegistLoginMapInfo

	NULL,						// SkillUpdateToDB
	RSkillInsert,				// SkillInsertToDB
	NULL,						// SkillDeleteToDB
	NULL,
	NULL,			/// HeroInfo
	NULL,			/// TotalInfo
	NULL,			// eBadFameUpdate,
	RPartyLoad,
	RPartyInfoByUserLogin,
	RPartyCreate,
	NULL,
	RPartyAddMember,	// 090529 ShinJS --- add Func : ePartyAddMember
	RPartyDelMember,	// PartyDelMember
	RPartyChangeMaster, //PartyChangeMaster

	NULL,
	RSaveMapChangePointReturn,

	NULL,
	RCharacterStorageInfo,			// eCharacterStorageInfo
	RCharacterStorageItemInfo,		// eCharacterStorageItemInfo	
	NULL,					// StorageMoneyUpdateToDB

	RFriendNotifyLogouttoClient,	
	NULL,					//eBadFameCharacterUpdate
	NULL,					//ePKCharacterUpdate
	RQuestTotalInfo,		// eQuestTotalInfo                         
	NULL,					// eQuestUpdate
	RQuestMainQuestLoad,	// eQuestMainQuestLoad
	RQuestSubQuestLoad,		// eQuestSubQuestLoad
	RQuestItemLoad,			// eQuestItemLoad
	NULL,					// eQuestMainQuestInsert
	NULL,					// eQuestSubQuestInsert
	NULL,					// eQuestItemInsert
	NULL,					// eQuestMainQuestUpdate
	NULL,					// eQuestSubQuestUpdate
	NULL,					// eQuestItemUpdate
	NULL,					// eQuestMainQuestDelete
	NULL,					// eQuestSubQuestDelete
	NULL,					// eQuestSubQuestDeleteAll	
	NULL,					// eQuestItemDelete
	NULL,					// eQuestItemDeleteAll
	NULL,					// eQuestMainQuestUpdateCheckTime
	NULL,					// eCharacterInfoByTime,
	NULL,					// eSkillExpByTime,
	NULL,					// eSpeedHackCheck,

	RGuildSelectForReward,	// eGuildSelectForReward
	RGuildItemSelect,		// eGuildItemSelect
	
	RGuildLoadGuild,		// eGuildLoadGuild, 
	RGuildLoadMember,		// eGuildLoadMember,
	RGuildBreakUp,			// eGuildBreakUp,
	RGuildDeleteMember,		// eGuildDeleteMember,
	RGuildSecedeMember,		// eGuildSecedeMember
	NULL,					// eGuildChangeMemberRank
	NULL,					// eGuildGiveNickName
	RGuildAddScore,				//eGuildAddScore
	RGuildUpdateScore,			//eGuildUpdateScore 
	RGuildMarkDelete,			//eGuildMarkDelete // 091111 ONS 길드문장 삭제
	RConfirmUserOut,		// eConfirmUserOut
	RLoadGuildWarRecord,	// eLoadGuildWarRecord	
	NULL,					// eGuildWarRecordDelete
	RLoadGuildWar,			// eLoadGuildWar
	NULL,					// eInsertGuildFieldWar
	NULL,					// eDeleteGuildFieldWar
	NULL,					// eUpdateGuildFieldWarRecord
	RGuildUnionLoad,					// eGuildUnionLoad
	RGuildUnionCreate,					// eGuildUnionCreate
	NULL,								// eGuildUnionDestroy	
	NULL,								// eGuildUnionAddGuild
	NULL,								// eGuildUnionRemoveGuild
	NULL,								// eGuildUnionSecedeGuild
	NULL,								// eChangeCharacterAttr
	NULL,								// eGuildUpdateNotice,

	RQuestEventCheck,					// eQuestEventCheck
	RQuickInfo,							// eQuickInfo
	NULL,

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.04.23	2007.09.12	2008.01.16
	RFarm_LoadFarmState,				// eFarm_LoadFarmState
	RFarm_LoadCropInfo,					// eFarm_LoadCropInfo
	RFarm_LoadTimeDelay,				// eFarm_LoadTimeDelay
	NULL,								// eFarm_SetFarmState
	NULL,								// eFarm_SetCropInfo
	NULL,								// eFarm_SetTimeDelay
	NULL,								// eFarm_SetTaxInfo
	// 091204 ONS 패밀리다이얼로그에 농장세금납부기능 추가 : 개별 농장 정보를 DB에서 가져오기위해 추가
	RFarm_GetFarmInfo,
	// E 농장시스템 추가 added by hseos 2007.04.23	2007.09.12	2008.01.16

	// desc_hseos_몬스터미터01
	// S 몬스터미터 추가 added by hseos 2007.05.28
	RMonsterMeter_LoadInfo,				// eMonsterMeter_LoadInfo
	NULL,								// eMonsterMeter_SetInfo
	// E 몬스터미터 추가 added by hseos 2007.05.28

	NULL,
	NULL,

	NULL,

	// 090413 ShinJS --- 챌린지 존에서 나가는 경우
	NULL,								// eCharacterLogoutPointUpdate_ToChallengeSavedPoint
	
	// 070918 LYW --- MapDBMsgParser : Add function pointer.
	RSaveNpcMapChangePointReturn,

	// 071023 LYW --- MapDBMsgParser : 튜토리얼 로드 결과처리를 할 함수 포인터.
	RTutorialLoad,

	// 071023 LYW --- MapDBMsgParser : 튜토리얼 업데이트 결과 처리를 할 함수 포인터.
	RTutorialUpdate,

	// 071106 LYW --- MapDBMsgParser : 기본 스탯으로 업데이트 하는 함수 포인터.
	RUpdateToDefaultStats,

	// 071117 LYW --- MapDBMsgParser : 퀘스트 관련 추가 함수.
	NULL,	//eEndQuest_New,
	NULL,	//eEndSubQuest_New,
	NULL,	//eDeleteQuest_New,

	RCharacterBuffLoad,					//eCharacterBuffLoad
	NULL,								//eCharacterBuffAdd
	NULL,								//eCharacterBuffRemove
	NULL,								//eCharacterBuffUpdate
	RSkill_Reset,
	NULL,	// eUpdateAccumulateSkillpoint,

	NULL,

	RIncreaseCharacterInventory,		// eIncreaseCharacterInventory
	NULL,								// eResetCharacterInventory
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.26		2008.01.23
	NULL,								// eChallengeZone_SavePos_SetInfo

	RChallengeZone_EnterFreq_Load,		// eChallengeZone_EnterFreq_LoadInfo
	NULL,								// eChallengeZone_EnterFreq_SetInfo

	NULL,								// eChallengeZone_Success_SetInfo

	// 091124 ONS 챌린지존 경험치분배/로드 처리추가
	NULL,
	RChallengeZone_ExpRate_Load,		// eChallengeZone_ExpRate_Load
	RChangeCharacterName,				// eCharacterChangeName
//---KES AUTONOTE
		RAutoNoteListLoad,					// eAutoNoteList_Load,	//---자신이 잡은 오토
		RAutoNoteListAdd,					// eAutoNoteList_Add,	//---자신이 잡은 오토
//---------------
		NULL,
	NULL, //RResidentRegist_ResetResult,				// eResidentRegist_Reset

	RGiftItemInsert,	//eGiftItemInsert
	
	RPetInfoLoad,
	RPetInfoAdd,
	NULL,
	NULL,
	NULL,

	NULL,
	RPetItemInfoLoad,

	NULL, //eFarm_SetAnimalInfo,
	RFarm_LoadAnimalInfo,	//eFarm_LoadAnimalInfo

	// 080813 LYW --- MapDBMsgParser : 소환 오브젝트 정보를 추가하는 분류 추가.
  	RSiegeREcallInsert,
  	// 080811 LYW --- MapDBMsgParser : 공성용 소환 오브젝트 정보를 업데이트 하는 분류 추가.
  	NULL,	// eSiegeRecallUpdate
  	// 080811 LYW --- MapDBMsgParser : 공성용 소환 오브젝트 정보를 삭제하는 분류 추가.
  	NULL,	// eSiegeRecallRemove
  	//// 080812 LYW --- MapDBMsgParser : 공성용 소환 오브젝트 정보를 로드하는 분류 추가.
  	RSiegeRecallLoad,

	// 081108 LYW --- MapDBMsgParser : 소환 된 몹 정보를 초기화 하는 분류 추가.
	NULL,	//eSiegeRecallReset,
  
  	// 080814 KTH --- MapDBMsgParser : 공성 관련 추가.
   	RSiegeWarfareInfoLoad,		// eSiegeWarfareInfoLoad
  	NULL,						// eSiegeWarfareInfoUpdate
	RSiegeWarWarterSeedComplete,// eSiegeWarWarterSeedComplete
	NULL,
  
  	// 080822 LYW --- MapDBMsgParser : 소환 된 npc 정보를 DB에서 로드하는 분류 추가.
  	RNpcRemainTime_Load,
	NULL,
	NULL,
	NULL,
  	NULL,						// eSavePointSiegeWarfare,
	RRequuestKillCount,

	// 090406 NYJ - 하우징 가구목록
	RHouseFurnitureLoad,
	// NYJ - 트리거목록
	RLoadTriggerPlayerFromDb,
	RLoadTriggerServerFromDb,

	// 091113 ONS 길드토너먼트 우승/준우승 골드 지급분류 추가.
	RGTRewardMoneyInsert,

	// 091230 ShinJS --- PC방 아이템 지급, eProvidePCRoomItem
	RProvidePCRoomItem,
	RGetPCRoomInfo,
	
	RConsignment_GetList,
	RConsignment_Search,
	RNote_GetPackage,

	RForbidChatUpdate,
	RForbidChatLoad
};


char txt[512];

void CharacterNumSendAndCharacterInfo(DWORD CharacterIDX, DWORD Protocol)
{
	_sntprintf(
		txt,
		_countof(txt),
		"EXEC dbo.MP_CHARACTER_SELECTBYCHARACTERIDX %d",
		CharacterIDX);
	g_DB.Query(
		eQueryType_FreeQuery,
		eCharacterInfoQuery,
		Protocol,
		txt,
		CharacterIDX);
	Option_Load(
		CharacterIDX);
}

void CharacterSkillTreeInfo(DWORD CharacterIDX, DWORD Protocol)
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d", STORED_MAPCHANGE_SKILLTREEINFO, CharacterIDX);
	g_DB.Query(eQueryType_FreeQuery, eCharacterSkillTreeQuery, Protocol, txt, CharacterIDX);
}

void CharacterItemInfo(DWORD CharacterIDX, DWORD Protocol)
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d", STORED_CHARACTER_ITEMINFO, CharacterIDX);
	g_DB.Query(eQueryType_FreeQuery, eCharacterItemQuery, Protocol, txt, CharacterIDX);
}

void	CharacterShopItemInfo(DWORD CharacterIDX, DWORD StartDBIdx)
{
	sprintf(txt, "EXEC %s %d, %d", STORED_SHOPITEM_INFO, CharacterIDX, StartDBIdx);
	g_DB.Query(eQueryType_FreeQuery, eShopItemInfoQuery, CharacterIDX, txt, CharacterIDX);
}

void SSItemInsert(DWORD CharacterIdx, DWORD wItemIdx, DURTYPE Durability, POSTYPE bPosition, DWORD FromChrIdx, ITEM_SEAL_TYPE Sealed, int remainTime)
{
	sprintf(
		txt,
		"EXEC %s %d, %d, %d, %d, %d, %d",
		STORED_ITEM_INSERT_INVEN,
		CharacterIdx,
		wItemIdx,
		Durability,
		bPosition,
		Sealed,
		0,
		remainTime);
	g_DB.Query(
		eQueryType_FreeQuery,
		eSSItemInsert,
		FromChrIdx,
		txt,
		CharacterIdx);
}

void ItemUpdateToDB(DWORD CharacterIdx, DWORD dwDBIdx, DWORD wItemIdx, DURTYPE Durability, POSTYPE bPosition, WORD qPosition, DWORD itemParam)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		CharacterIdx,
		"EXEC dbo.MP_ITEM_UPDATE %d, %d, %d, %d, %d, %d, %d", 
		CharacterIdx,
		dwDBIdx,
		wItemIdx,
		Durability,
		bPosition,
		qPosition,
		itemParam);
}

/* bDel:0 fromItem Update, bDel:1 fromItem Delete */
void ItemCombineUpdateToDB(DWORD dwfromDBIdx, DURTYPE fromDur, DWORD dwToDBIdx, DURTYPE toDur, DWORD CharacterIdx)
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d, %d, %d, %d", STORED_ITEM_COMBINEUPDATE, dwfromDBIdx, fromDur, dwToDBIdx, toDur);
	g_DB.Query(eQueryType_FreeQuery, eItemCombineUpdate, 0, txt, CharacterIdx);
}

void ItemMoveUpdateToDB(DWORD CharacterIDX, DWORD dwfromDBIdx, POSTYPE frompos, DWORD dwtoDBIdx, POSTYPE topos, DWORD UserIdx, DWORD MunpaIdx)
{
	// 080410 LUJ, 영역 정보가 바뀌어도 관계없도록 인자로 받도록 함
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(
		txt,
		"EXEC dbo.MP_ITEM_MOVEUPDATE %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
		dwfromDBIdx,
		frompos,
		dwtoDBIdx,
		topos,
		CharacterIDX,
		UserIdx,
		MunpaIdx,
		TP_INVENTORY_START,
		// 080611 LUJ, 장비칸을 고려하지 않아 DB 갱신이 실패하는 현상을 수정
		TP_INVENTORY_END + SLOT_EXTENDED_INVENTORY1_NUM + SLOT_EXTENDED_INVENTORY2_NUM + SLOT_WEAR_NUM - 1,
		TP_STORAGE_START,
		TP_STORAGE_END - 1,
		TP_GUILDWAREHOUSE_START,
		TP_GUILDWAREHOUSE_END - 1,
		TP_SHOPITEM_START,
		TP_SHOPITEM_END - 1 );

	g_DB.Query(eQueryType_FreeQuery, eItemMoveUpdate, 0, txt, CharacterIDX);
}

void ItemMoveStorageUpdateToDB(DWORD CharacterIDX, DWORD UserIDX, DWORD dwfromDBIdx, POSTYPE frompos, DWORD dwtoDBIdx, POSTYPE topos)
{
	// 080410 LUJ, 영역 정보가 바뀌어도 관계없도록 인자로 받도록 함
	sprintf(
		txt,
		"EXEC dbo.MP_ITEM_MOVEUPDATESTORAGE %d, %d, %d, %d, %d, %d, %d, %d",
		CharacterIDX,
		UserIDX,
		dwfromDBIdx,
		frompos,
		dwtoDBIdx,
		topos,
		TP_INVENTORY_START,
		TP_INVENTORY_END + SLOT_EXTENDED_INVENTORY1_NUM + SLOT_EXTENDED_INVENTORY2_NUM - 1 );
	
	g_DB.Query(eQueryType_FreeQuery, eItemMoveStorageUpdate, 0, txt, CharacterIDX);
}

void ItemMoveGuildUpdateToDB(DWORD CharacterIDX, DWORD MunpaIDX, DWORD dwfromDBIdx, POSTYPE frompos, DWORD dwtoDBIdx, POSTYPE topos)
{
	// 080410 LUJ, 영역 정보가 바뀌어도 관계없도록  인자로 받도록 함
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(
		txt,
		"EXEC dbo.MP_ITEM_MOVEUPDATEMUNPA %d, %d, %d, %d, %d, %d, %d, %d",
		CharacterIDX,
		MunpaIDX,
		dwfromDBIdx,
		frompos,
		dwtoDBIdx,
		topos,
		TP_INVENTORY_START,
		TP_INVENTORY_END + SLOT_EXTENDED_INVENTORY1_NUM + SLOT_EXTENDED_INVENTORY2_NUM - 1 );

	g_DB.Query(eQueryType_FreeQuery, eItemMoveGuildUpdate, 0, txt, CharacterIDX);
}

void ItemInsertToDB(DWORD CharacterIdx, DWORD wItemIdx, DURTYPE Durability, POSTYPE bPosition, DWORD dwKey, ITEM_SEAL_TYPE wSeal)
{
	const ITEM_INFO* info = ITEMMGR->GetItemInfo( wItemIdx );

	if( ! info )
	{
		return;
	}
	
	switch( info->SupplyType )
	{
		// 080820 LUJ, 획득 시 쿨타임을 가진 아이템 처리
	case ITEM_KIND_COOLTIME:
		{
			sprintf(txt, "EXEC %s %d, %d, %d, %d, %d, 0, %d", STORED_ITEM_INSERT_INVEN, CharacterIdx, wItemIdx, Durability, bPosition, wSeal, info->SupplyValue );break;
			break;
		}
	default:
		{
			sprintf(txt, "EXEC %s %d, %d, %d, %d, %d ", STORED_ITEM_INSERT_INVEN, CharacterIdx, wItemIdx, Durability, bPosition, wSeal);			
			break;
		}
	}

	g_DB.Query(eQueryType_FreeQuery, eItemInsert, dwKey, txt, CharacterIdx);
}


void ItemDeleteToDB(DWORD dwDBIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		0,
		"EXEC dbo.MP_ITEM_DELETE %d",
		dwDBIdx );

	ITEMMGR->RemoveOption( dwDBIdx );
}

// 080402 LUJ, 인자 이름을 아이템 DB인덱스로 변경
void ItemOptionDelete( DWORD dwDBIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		0,
		"EXEC dbo.MP_ITEM_OPTION_DELETE %d",
		dwDBIdx );
}


// 071231 KTH -- CharacterIDX add
void SkillUpdateToDB( SKILL_BASE* msg, DWORD CharacterIDX )
{
	sprintf(txt, "EXEC %s %d, %d, %d", STORED_SKILL_UPDATE, msg->dwDBIdx, msg->wSkillIdx, msg->mLearnedLevel);
	g_DB.Query(eQueryType_FreeQuery, eSkillUpdate2, 0, txt, CharacterIDX);
}

void SkillInsertToDB(DWORD CharacterIDX, DWORD SkillIDX, LEVELTYPE level)
{
	sprintf(txt, "EXEC %s %u, %u, %d", STORED_SKILL_INSERT, CharacterIDX, SkillIDX, level);
	g_DB.Query(eQueryType_FreeQuery, eSkillInsert, CharacterIDX, txt, CharacterIDX);
}

void RegistLoginMapInfo(DWORD CharacterIDX, char* CharacterName, BYTE MapNum, WORD MapPort)
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(CharacterName, "'"))	return;

	//	char txt[128];
	sprintf(txt, "EXEC %s %d, \'%s\', %d, %d", STORED_CHARACTER_REGISTLOGINMAPINFO, CharacterIDX, CharacterName, MapNum, MapPort);
	g_DB.Query(eQueryType_FreeQuery, eRegistLoginMapInfo, 0, txt, CharacterIDX);
}

void UnRegistLoginMapInfo(DWORD CharacterIDX)
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d", STORED_CHARACTER_UNREGISTLOGINMAPINFO, CharacterIDX);
	g_DB.Query(eQueryType_FreeQuery, eUnRegistLoginMapInfo, 0, txt, CharacterIDX);
}

//////////////////////////////////////////////////////////////////////////
// ¨¡A¨¡¨u ¡Æu¡¤A DBAo￠￢￠c
void PartyLoad(DWORD PartyIDX)
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d ", STORED_PARTY_REGIST_MAPSERVER, PartyIDX);
	g_DB.Query(eQueryType_FreeQuery, ePartyRegistMapServer, 0, txt);
}

void PartyInfoByUserLogin(DWORD PartyIDX, DWORD CharacterIDX) 
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d ", STORED_PARTY_INFO_BY_USERLOGIN, PartyIDX);
	g_DB.Query(eQueryType_FreeQuery, ePartyInfoByUserLogin, CharacterIDX, txt, CharacterIDX);
}
void PartyBreakup(DWORD PartyIDX, DWORD CharacterIDX) 
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d", STORED_PARTY_BREAK, PartyIDX);
	g_DB.Query(eQueryType_FreeQuery, ePartyBreakup, PartyIDX, txt, CharacterIDX);
}
void PartyCreate(DWORD MasterIDX, BYTE Option, BYTE SubOption) 
{
	sprintf(txt, "EXEC %s %d, %d, %d", STORED_PARTY_CREATE, MasterIDX, Option, SubOption);
	g_DB.Query(eQueryType_FreeQuery, ePartyCreate, MasterIDX, txt, MasterIDX);
}

void GuildLoadGuild(DWORD StartGuildIdx)
{
	sprintf(txt, "EXEC %s %u", STORED_GUILD_LOADGUILD, StartGuildIdx);
	g_DB.Query(eQueryType_FreeQuery, eGuildLoadGuild, 0, txt);
}

void GuildUpdateNotice( DWORD guildIndex, const char* notice, DWORD CharacterIDX )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	// 081130 LUJ, 일본어 반각문자가 따옴표와 결합하여 문자열이 닫히지 않는 현상 발생.
	//			반각문자가 후속 문자와 결합하여 다른 문자로 변하는 현상이 낮은 확률로 발생한다고 함
	//			일단 변하는 확률을 낮추기 위해 문자열 뒤에 공백을 한칸 추가함.
	sprintf( txt, "EXEC dbo.MP_GUILD_NOTICE_UPDATE %d, \'%s \'", guildIndex, notice, CharacterIDX );

	g_DB.FreeQuery( eGuildUpdateNotice, 0, txt);
}

void GuildLoadMember( DWORD StartCharacterIdx )
{
	g_DB.FreeQuery(
		eGuildLoadMember,
		0,
		"EXEC MP_GUILD_LoadMember %u",
		StartCharacterIdx );
}

void GuildLoadMark(DWORD StartMarkName)
{
	char txt[128];
	sprintf(txt, "EXEC %s %u", STORED_GUILD_LOADMARK, StartMarkName);
	g_DB.FreeLargeQuery(RGuildLoadMark, 0, txt);
}

void GuildCreate(DWORD CharacterIdx, const char* CharacterName, LEVELTYPE CharacterLvl, const char* GuildName, const char* Intro, int masterLevel )
{
	// 090325 ONS 필터링문자체크
	char szBufName[MAX_NAME_LENGTH+1]		= { 0 };
	char szBufGuildname[MAX_GUILD_NAME+1]	= { 0 };
	char szBufGuildIntro[MAX_GUILD_INTRO+1]	= { 0 };

	SafeStrCpy(szBufName, CharacterName, MAX_NAME_LENGTH+1);
	SafeStrCpy(szBufGuildname, GuildName, MAX_GUILD_NAME+1);
	SafeStrCpy(szBufGuildIntro, Intro, MAX_GUILD_INTRO+1);

	if(IsCharInString(szBufName, "'") || IsCharInString(szBufGuildname, "'") || IsCharInString(szBufGuildIntro, "'") )	return;

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RGuildCreate,
		CharacterIdx,
		"EXEC dbo.MP_GUILD_CREATE2 %d, \'%s\', %d, \'%s\', \'%s\', %d, %d",
		CharacterIdx,
		CharacterName,
		CharacterLvl,
		GuildName,
		Intro,
		GAMERESRCMNGR->GetLoadMapNum(),
		masterLevel,
		CharacterIdx );
}


void RGuildCreate( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD				playerIndex = dbMessage->dwID;
	CPlayer*				player		= ( CPlayer* )g_pUserTable->FindUser( playerIndex );

	if( ! player			||
		! dbMessage->dwResult )
	{
		ASSERT( 0 );
		return;
	}	

	const MIDDLEQUERYST&	record		= query[ 0 ];

	if( ! atoi( ( char* )record.Data[ 0 ] ) )
	{
		if( player )
		{
			GUILDMGR->SendNackMsg( player, MP_GUILD_CREATE_NACK, eGuildErr_Create_Invalid_Name );
		}

		return;
	}

	// 창립 자금을 빼낸다
	{
		const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( 1 );

		if( ! setting )
		{
			ASSERT( 0 );
			return;
		}

		player->SetMoney( setting->mRequiredMoney, MONEY_SUBTRACTION, MF_LOST, eItemTable_Inventory, 0, 0  );
	}

	{
		GUILDINFO data;

		ZeroMemory( &data, sizeof( data ) );
		data.GuildIdx	= atoi( ( char* )record.Data[ 0 ] );
		data.MasterIdx	= playerIndex;
		data.GuildLevel = 1;
		data.UnionIdx	= 0;
		data.MarkName	= 0;
		SafeStrCpy(data.GuildName, ( char* )record.Data[ 2 ], sizeof( data.GuildName ) );
		SafeStrCpy(data.MasterName, ( char* )record.Data[ 3 ], sizeof( data.MasterName ) );

		GUILDMGR->CreateGuildResult( LEVELTYPE( atoi( ( char* )record.Data[ 4 ] ) ), data );

		GuildReadData( playerIndex, data.GuildIdx );
	}
}


void GuildBreakUp(DWORD GuildIdx, DWORD MasterIdx)
{
	sprintf(txt, "EXEC %s %d", STORED_GUILD_BREAKUP, GuildIdx);
	g_DB.Query(eQueryType_FreeQuery, eGuildBreakUp, MasterIdx, txt, MasterIdx);

	//공성전 성점령 정보도 초기화
	sprintf(txt, "EXEC %s %d", "MP_SIEGEWARFARE_GUILD_BreakUp", GuildIdx);
	g_DB.Query(eQueryType_FreeQuery, eSiegeWarfareGuildBreakUp, MasterIdx, txt, MasterIdx);
}


void GuildDeleteMember(DWORD GuildIdx, DWORD MemberIDX)
{
	sprintf(txt, "EXEC %s %d, %d", STORED_GUILD_DELETEMEMBER, GuildIdx, MemberIDX);
	g_DB.Query(eQueryType_FreeQuery, eGuildDeleteMember, GuildIdx, txt, MemberIDX);
}


void GuildSecedeMember(DWORD GuildIdx, DWORD PlayerIdx)
{
	sprintf(txt, "EXEC %s %d, %d", STORED_GUILD_DELETEMEMBER, GuildIdx, PlayerIdx);
	g_DB.Query(eQueryType_FreeQuery, eGuildSecedeMember, GuildIdx, txt, PlayerIdx);
}

void GuildAddMember(DWORD guildIndex, DWORD playerIndex, const char* guildName, BYTE rank )
{
	// 090325 ONS 필터링문자체크
	char szBufGuildname[MAX_GUILD_NAME+1]	= { 0 };
	SafeStrCpy(szBufGuildname, guildName, MAX_GUILD_NAME+1);
	if(IsCharInString(szBufGuildname, "'"))	return;

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RGuildAddMember,
		playerIndex,
		"EXEC MP_GUILD_ADDMEMBER %u, %u, \'%s\', %d",
		guildIndex,
		playerIndex,
		guildName,
		rank,
		playerIndex);
}


void RGuildAddMember( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	if( 1 == dbMessage->dwResult )
	{
		CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( dbMessage->dwID );

		if( player )
		{
			GUILDMGR->AddPlayer( player );
		}
	}	
}


void GuildMarkRegist(DWORD GuildIdx, char* ImgData, DWORD CharacterIdx)
{
	char buf[ 2048 ];
	sprintf( buf, "EXEC %s %d, 0x", STORED_GUILD_MARKREGIST,GuildIdx);

	int curpos = strlen(buf);
	for(int n=0;n<GUILDMARK_BUFSIZE;++n)
	{
		sprintf(&buf[curpos],"%02x",(BYTE)ImgData[n]);
		curpos += 2;
	}

	g_DB.FreeLargeQuery(RGuildMarkRegist, CharacterIdx,	
		buf, CharacterIdx);
}

// 091111 ONS 길드문장 삭제 처리 추가
void GuildMarkDelete(DWORD GuildIdx, DWORD CharacterIdx)
{
	sprintf(txt, "EXEC %s %u, %u", STORED_GUILD_MARKDELETE, GuildIdx, CharacterIdx);
	g_DB.Query(eQueryType_FreeQuery, eGuildMarkDelete, 0, txt, CharacterIdx);
}

void RGuildMarkDelete( LPQUERY query, LPDBMESSAGE message )
{
	if( !message->dwResult )
		return;

	const DWORD dwGuildIdx = atoi( (char*)query->Data[0] );
	const DWORD dwMarkIdx  = atoi( (char*)query->Data[1] );
	const DWORD dwPlayerIdx  = atoi( (char*)query->Data[2] );

	if(dwGuildIdx)
	{
		GUILDMGR->DeleteMarkResult(dwPlayerIdx, dwMarkIdx, dwGuildIdx);
	}
}

void GuildChangeMemberRank(DWORD GuildIdx, DWORD MemberIdx, BYTE Rank)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		0,
		"EXEC MP_GUILD_CHANGERANK %u, %u, %d",
		GuildIdx,
		MemberIdx,
		Rank,
		MemberIdx);
}

void GuildGiveMemberNickName(DWORD TargetId, char * NickName)
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(NickName, "'"))	return;

	sprintf(txt, "EXEC %s %u, \'%s\'", STORED_GUILD_GIVENICKNAME, TargetId, NickName);
	g_DB.Query(eQueryType_FreeQuery, eGuildGiveNickName, 0, txt, TargetId);
}

void StorageBuyStorage(DWORD PlayerIDX)
{
	sprintf(txt, "EXEC %s %d", STORED_STORAGE_BUYSTORAGE, PlayerIDX);
	g_DB.Query(eQueryType_FreeQuery, eStorageBuyStorage, PlayerIDX, txt, PlayerIDX);
}
// 071231 KTH -- CharacterIDX Add
void StorageMoneyUpdateToDB(DWORD UserIdx, MONEYTYPE StorageMoney, DWORD CharacterIDX)
{
	sprintf(txt, "EXEC %s %u, %u", STORED_STORAGE_MONEYUPDATE, UserIdx, StorageMoney);
	g_DB.Query(eQueryType_FreeQuery, eStorageMoneyUpdate, 0, txt, CharacterIDX);
}

void CharacterStorageInfo(DWORD UserIdx, DWORD CharacterIdx)
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d", STORED_STORAGE_INFO, UserIdx);
	g_DB.Query(eQueryType_FreeQuery, eCharacterStorageInfo, CharacterIdx, txt, CharacterIdx);
}

//void CharacterStorageItemInfo(DWORD CharacterIdx, DWORD StartDBIdx)
void CharacterStorageItemInfo(DWORD CharacterIdx,DWORD UserIdx, DWORD StartDBIdx)
{
	//	char txt[128];
	//sprintf(txt, "EXEC %s %d, %d", STORED_STORAGE_ITEMINFO, CharacterIdx, StartDBIdx);
	//g_DB.Query(eQueryType_FreeQuery, eCharacterStorageItemInfo, CharacterIdx, txt);
	sprintf(txt, "EXEC %s %d, %d", STORED_STORAGE_ITEMINFO, UserIdx, StartDBIdx);
	g_DB.Query(eQueryType_FreeQuery, eCharacterStorageItemInfo, CharacterIdx, txt, CharacterIdx);
}

void FriendNotifyLogouttoClient(DWORD PlayerID)
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d", STORED_FRIEND_NOTIFYLOGOUT, PlayerID);
	g_DB.Query(eQueryType_FreeQuery, eFriendNotifyLogout, PlayerID, txt, PlayerID);
}

void RGuildLoadMember(LPQUERY pData, LPDBMESSAGE pMessage)
{
	GUILDMEMBERINFO_ID member = {0,};

	for( DWORD i = 0; i < pMessage->dwResult; ++i )
	{
		ZeroMemory( &member, sizeof( member ) );

		QUERYST&			query	= pData[ i ];
		GUILDMEMBERINFO&	info	= member.MemberInfo;

		info.MemberIdx	= atoi( ( char* )query.Data[ 0 ] );
		SafeStrCpy( info.MemberName, (char* )query.Data[ 1 ], sizeof( info.MemberName ) );

		info.Memberlvl	= LEVELTYPE( atoi( ( char* )query.Data[ 2 ] ) );
		member.GuildIdx = atoi( ( char* )query.Data[ 3 ] );
		info.Rank		= BYTE( atoi( ( char* )query.Data[ 4 ] ) );

		// 080225 LUJ, 쿼리 정보 추가
		info.mJobGrade	= BYTE( atoi( ( char* )query.Data[ 5 ] ) );
		info.mJob[ 0 ]	= BYTE( atoi( ( char* )query.Data[ 6 ] ) );
		info.mJob[ 1 ]	= BYTE( atoi( ( char* )query.Data[ 7 ] ) );
		info.mJob[ 2 ]	= BYTE( atoi( ( char* )query.Data[ 8 ] ) );
		info.mJob[ 3 ]	= BYTE( atoi( ( char* )query.Data[ 9 ] ) );
		info.mJob[ 4 ]	= BYTE( atoi( ( char* )query.Data[ 10 ] ) );
		info.mJob[ 5 ]	= BYTE( atoi( ( char* )query.Data[ 11 ] ) );
		info.mRace		= BYTE( atoi( ( char* )query.Data[ 12 ] ) );

		GUILDMGR->LoadMembers(&member) ;
	}

	if( MAX_ROW_NUM == pMessage->dwResult )
	{
		GuildLoadMember( member.MemberInfo.MemberIdx );
	}
	else
	{
		GuildLoadMark(0);
	}	
}	

void CharacterHeroInfoUpdate(CPlayer* pPlayer)
{
	if( !pPlayer->GetInited() )	return;

	BASEOBJECT_INFO baseinfo;
	HERO_TOTALINFO	heroinfo;

	pPlayer->GetBaseObjectInfo(&baseinfo);
	pPlayer->GetHeroTotalInfo(&heroinfo);

	// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
	char txt[512] = {0,} ;
	//sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d, %d, %d, %u, %d, %u, %d, %d, %d, %d, %d",
	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d, %d, %d, %0.f, %d, %u, %d, %d, %d, %d, %d",
		STORED_CHARACTER_HEROINFOUPDATE,
		baseinfo.dwObjectID, 
		heroinfo.Str, 
		heroinfo.Mana, 
		heroinfo.Dex, 
		heroinfo.Vit, 
		heroinfo.Int, 
		heroinfo.Wis, 
		0,
		0,
		(float)heroinfo.ExpPoint, 
		heroinfo.LevelUpPoint, 
		heroinfo.Money, 
		0,
		0,/*어빌리티 삭제 - 이영준 pPlayer->GetPlayerAbilityExpPoint(), */
		heroinfo.Playtime, 
		heroinfo.LastPKModeEndTime,
		heroinfo.MaxLevel);
	g_DB.Query(eQueryType_FreeQuery, eHeroInfoUpdate, 0, txt, pPlayer->GetID());
}

void CharacterTotalInfoUpdate(CPlayer* pPlayer)
{
	if( !pPlayer->GetInited() )	return;

	CHARACTER_TOTALINFO totinfo;
	pPlayer->GetCharacterTotalInfo(&totinfo);

	// 06. 06 - 이영준 중요!!!
	//MP_CHARACTER_TotalInfoUpdate 수정 필요함
		char txt[512];
	sprintf(txt, "EXEC  %s %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u",
		STORED_CHARACTER_TOTALINFOUPDATE,
		pPlayer->GetID(), totinfo.Gender,
		totinfo.Life,
		totinfo.Level,
		totinfo.LoginMapNum, totinfo.FaceType, totinfo.HairType, totinfo.WearedItemIdx[eWearedItem_Hat],
		totinfo.WearedItemIdx[eWearedItem_Weapon], totinfo.WearedItemIdx[eWearedItem_Dress],
		totinfo.WearedItemIdx[eWearedItem_Shoes], 0/*모자*/,
		0/*옷*/, totinfo.WearedItemIdx[eWearedItem_Glove],
		0/*신발*/, 0/*장갑*/,
		0/*마스크*/, totinfo.WearedItemIdx[eWearedItem_Shield],
		totinfo.BadFame, 0/*안경*/, totinfo.WearedItemIdx[eWearedItem_Band] );
	g_DB.Query(eQueryType_FreeQuery, eTotalInfoUpdate, 0, txt, pPlayer->GetID());
}

void PartyAddMember(DWORD PartyIDX, DWORD TargetID)
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d, %d ", STORED_PARTY_ADDMEMBER, PartyIDX, TargetID);
	g_DB.Query(eQueryType_FreeQuery, ePartyAddMember, TargetID, txt, TargetID);
}

// 090529 ShinJS --- 파티멤버 추가시 파티원 정보 갱신 추가
void RPartyAddMember(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if( !pMessage->dwResult )
		return;

	const DWORD dwTargetID = pMessage->dwID;
	const DWORD dwPartyIdx = atoi( (char*)pData->Data[0] );
	CPlayer* pNewMember = (CPlayer*)g_pUserTable->FindUser(dwTargetID);
	CParty* pParty = PARTYMGR->GetParty( dwPartyIdx );

	if( !pNewMember || !pParty )
		return;

	for(DWORD i = 0 ; i < pMessage->dwResult ; ++i)
	{
		const DWORD dwPlayerID		= atoi( (char*)pData[i].Data[2] );
		char* strName				= (char*)pData[i].Data[3];
		const LEVELTYPE Lv			= (LEVELTYPE)( atoi( (char*)pData[i].Data[4] ) );
		//100105 NYJ
		const MAPTYPE map			= (MAPTYPE)( atoi( (char*)pData[i].Data[5] ) );

		pParty->SetMemberInfo( (BYTE)i, dwPlayerID, strName, Lv, map );
	}

	pParty->SetPartyMemberNum(pMessage->dwResult);

	for(DWORD i = pMessage->dwResult; i < MAX_PARTY_LISTNUM; ++i)
	{
		const PARTY_MEMBER emptyMember = {0};
		pParty->SetMember(
			i,
			emptyMember);
	}

	PARTYMGR->AddMemberResult(pParty, pNewMember);
	PARTYMGR->UserLogIn(pNewMember, TRUE);
}

void PartyDelMember(DWORD PartyIDX, DWORD TargetID)
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d, %d ", STORED_PARTY_DELMEMBER, PartyIDX, TargetID);
	g_DB.Query(eQueryType_FreeQuery, ePartyDelMember, PartyIDX, txt, TargetID);
}

void PartyChangeMaster(DWORD PartyID, DWORD PlayerID, DWORD TargetPlayerID)
{
	//	char txt[128];
	sprintf(txt, "EXEC %s %d, %d, %d", STORED_PARTY_CHANGEMASTER, PartyID, PlayerID, TargetPlayerID);
	g_DB.Query(eQueryType_FreeQuery, ePartyChangeMaster, PartyID, txt, PlayerID);
}

void SavePointUpdate(DWORD CharacterIDX, WORD LoginPoint_Idx, WORD MapNum)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d, %d",	STORED_SAVE_LOGINPOINT, CharacterIDX, LoginPoint_Idx, MapNum);
	g_DB.Query(eQueryType_FreeQuery, eSavePoint, 0, txt, CharacterIDX);
}

void SaveMapChangePointUpdate(DWORD CharacterIDX, WORD MapChangePoint_Idx)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d",	STORED_SAVE_MAPCHAGEPOINT, CharacterIDX, MapChangePoint_Idx);
	g_DB.Query(eQueryType_FreeQuery, eSavePoint, 0, txt, CharacterIDX);
}

// 081006 LYW --- MapDBMsgParser : 맵 이동 정보 업데이트 함수 추가.
void SaveMapChangePointUpdate_SiegeWarfare( DWORD CharacterIDX, WORD MapNum, float fXpos, float fZpos )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	// 081103 LYW --- MapDBMsgParser : db 쿼리문 에러를 수정한다.
	//sprintf( txt, "EXEC %s %d %d %d %d", "dbo.MP_CHARACTER_MAPCHANGEPOINTUPDATE_SIEGEWARFARE", CharacterIDX, MapNum, fXpos, fZpos ) ;
	sprintf( txt, "EXEC %s %d, %d, %d, %d", "dbo.MP_CHARACTER_MAPCHANGEPOINTUPDATE_SIEGEWARFARE", CharacterIDX, MapNum, fXpos, fZpos ) ;
	g_DB.Query( eQueryType_FreeQuery, eSavePointSiegeWarfare, 0, txt, CharacterIDX ) ;
}

// 081218 LUJ, 플레이어 소지금 추가
void SaveMapChangePointReturn(DWORD CharacterIDX, WORD MapChangePoint_Idx, DWORD dwConnectionIdx, LEVELTYPE lvl, EXPTYPE ExpPoint, MONEYTYPE inventoryMoney, MONEYTYPE StorageMoney)
{
	sprintf(txt, "EXEC  %s %u, %d, %u, %d, %0.f, %u, %u", STORED_SAVE_MAPCHAGEPOINTRET, CharacterIDX, MapChangePoint_Idx, dwConnectionIdx, lvl, (float)ExpPoint, inventoryMoney, StorageMoney);
	g_DB.Query(eQueryType_FreeQuery, eSaveMapChangePoint, CharacterIDX, txt, CharacterIDX);
}

// 070917 LYW --- MapDBMsgParser : Add function to save map change point to return.
// 081218 LUJ, 플레이어 소지금 추가
void SaveNpcMapChangePointReturn(DWORD CharacterIDX, WORD MapChangePoint_Idx, DWORD dwConnectionIdx, LEVELTYPE lvl, EXPTYPE ExpPoint, MONEYTYPE inventoryMoney, MONEYTYPE StorageMoney)
{
	// 081219 LUJ, 인자가 하나 빠져서 추가
	sprintf(txt, "EXEC  %s %u, %d, %u, %d, %0.f, %u, %u",	STORED_SAVE_MAPCHAGEPOINTRET, CharacterIDX, MapChangePoint_Idx, dwConnectionIdx, lvl, (float)ExpPoint, inventoryMoney, StorageMoney);
	g_DB.Query(eQueryType_FreeQuery, eSaveNpcMapChangePoint, CharacterIDX, txt, CharacterIDX);
}

void BadFameCharacterUpdate(DWORD PlayerIDX, FAMETYPE val)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d",	STORED_BADFAME_CHARACTERUPDATE, PlayerIDX, val);
	g_DB.Query(eQueryType_FreeQuery, eBadFameCharacterUpdate, 0, txt, PlayerIDX);
}

void PKCharacterUpdate(DWORD PlayerIDX, DWORD val)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d",	STORED_PK_CHARACTERUPDATE, PlayerIDX, val);
	g_DB.Query(eQueryType_FreeQuery, ePKCharacterUpdate, 0, txt, PlayerIDX);
}

void QuestTotalInfo(DWORD PlayerID)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d",	"MP_QUEST_TOTALINFO", PlayerID);
	g_DB.Query(eQueryType_FreeQuery, eQuestTotalInfo, PlayerID, txt, PlayerID);
}

void QuestUpdateToDB(DWORD PlayerID, DWORD QuestIdx, QSTATETYPE State, BYTE bEnd)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d, %d, %d",	"MP_QUEST_UPDATE", PlayerID, QuestIdx, State, bEnd);
	g_DB.Query(eQueryType_FreeQuery, eQuestUpdate, 0, txt, PlayerID);
}

// RaMa
void QuestMainQuestLoad(DWORD PlayerID, int QuestIdx )
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d", STORED_QUEST_MAINQUEST_LOAD, PlayerID, QuestIdx );
	g_DB.Query(eQueryType_FreeQuery, eQuestMainQuestLoad, PlayerID, txt, PlayerID);
}

void QuestMainQuestInsert(DWORD PlayerID, DWORD mQuestIdx, QSTATETYPE State, __time64_t registTime, BOOL repeat)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d, %d, %I64d, %d", STORED_QUEST_MAINQUEST_INSERT, PlayerID, mQuestIdx, State, registTime, repeat);
	g_DB.Query(eQueryType_FreeQuery, eQuestMainQuestInsert, PlayerID, txt, PlayerID);
}

void QuestMainQuestDelete(DWORD PlayerID, DWORD mQuestIdx)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d", STORED_QUEST_MAINQUEST_DELETE, PlayerID, mQuestIdx);
	g_DB.Query(eQueryType_FreeQuery, eQuestMainQuestDelete, PlayerID, txt, PlayerID);
}

void QuestMainQuestUpdateToDB(DWORD PlayerID, DWORD mQuestIdx, QSTATETYPE State, DWORD EndParam, __time64_t registTime)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %I64d", STORED_QUEST_MAINQUEST_UPDATE, PlayerID, mQuestIdx, State, EndParam, registTime);
	g_DB.Query(eQueryType_FreeQuery, eQuestMainQuestUpdate, PlayerID, txt, PlayerID);
}

void QuestSubQuestLoad(DWORD PlayerID, int QuestIdx)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d", STORED_QUEST_SUBQUEST_LOAD, PlayerID, QuestIdx);
	g_DB.Query(eQueryType_FreeQuery, eQuestSubQuestLoad, PlayerID, txt, PlayerID);
}

// 071117 LYW --- MapDBMsgParser : 퀘스트 관련 추가 함수.
void EndQuest_New(DWORD dwCharacter_Idx, DWORD dwQuest_Idx, DWORD dwSubQuest_Do, DWORD dwEnd_Param, __time64_t registTime)
{
	sprintf
	(
		txt
	,	"EXEC  %s %d, %d, %d, %d, %I64d"
	,	STORED_QUEST_ENDQUEST_NEW
	,	dwCharacter_Idx
	,	dwQuest_Idx
	,	dwSubQuest_Do
	,	dwEnd_Param
	,	registTime
	) ;

	g_DB.Query(eQueryType_FreeQuery, eEndQuest_New, dwCharacter_Idx, txt, dwCharacter_Idx) ;
}

void EndSubQuest_New(DWORD dwCharacter_Idx, DWORD dwQuest_Idx, DWORD dwSubQuestIdx, DWORD dwSubQuest_Do, __time64_t registTime)
{
	sprintf
	(
		txt
	,	"EXEC  %s %d, %d, %d, %d, %I64d"
	,	STORED_QUEST_ENDSUBQUEST_NEW
	,	dwCharacter_Idx
	,	dwQuest_Idx
	,	dwSubQuestIdx
	,	dwSubQuest_Do
	,	registTime
	) ;

	g_DB.Query(eQueryType_FreeQuery, eEndSubQuest_New, dwCharacter_Idx, txt, dwCharacter_Idx) ;
}

void DeleteQuest_New(DWORD dwCharacter_Idx, DWORD dwQuest_Idx, DWORD param)
{
	sprintf(txt, "EXEC  %s %d, %d, %d", STORED_QUEST_DELETEQUEST_NEW, dwCharacter_Idx, dwQuest_Idx, param) ;
	g_DB.Query(eQueryType_FreeQuery, eDeleteQuest_New, dwQuest_Idx, txt, dwCharacter_Idx) ;
}

void QuestItemload(DWORD PlayerID, int QuestIdx )
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d", STORED_QUEST_ITEM_LOAD, PlayerID, QuestIdx );
	g_DB.Query(eQueryType_FreeQuery, eQuestItemLoad, PlayerID, txt, PlayerID);
}

void QuestItemInsert(DWORD PlayerID, DWORD dwQuestIdx, DWORD ItemIdx, DWORD dwItemNum )
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d, %d, %d", STORED_QUEST_ITEM_INSERT, PlayerID, ItemIdx, dwItemNum, dwQuestIdx );
	g_DB.Query(eQueryType_FreeQuery, eQuestItemInsert, PlayerID, txt, PlayerID);
}

void QuestItemDelete(DWORD PlayerID, DWORD ItemIdx)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d", STORED_QUEST_ITEM_DELETE, PlayerID, ItemIdx);
	g_DB.Query(eQueryType_FreeQuery, eQuestItemDelete, PlayerID, txt, PlayerID);
}

void QuestItemDeleteAll(DWORD PlayerID, DWORD dwQuestIdx)
{
	sprintf(txt, "EXEC  %s %d, %d", STORED_QUEST_ITEM_DELETE_ALL, PlayerID, dwQuestIdx);
	g_DB.Query(eQueryType_FreeQuery, eQuestItemDeleteAll, PlayerID, txt, PlayerID);
}

void QuestItemUpdate(DWORD PlayerID, DWORD dwQuestIdx, DWORD ItemIdx, DWORD ItemNum)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d, %d, %d", STORED_QUEST_ITEM_UPDATE, PlayerID, ItemIdx, ItemNum, dwQuestIdx );
	g_DB.Query(eQueryType_FreeQuery, eQuestItemUpdate, PlayerID, txt, PlayerID);
}

void QuestSubQuestDelete(DWORD PlayerID, DWORD mQuestIdx, DWORD sQuestIdx)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d, %d", STORED_QUEST_SUBQUEST_DELETE, PlayerID, mQuestIdx, sQuestIdx);
	g_DB.Query(eQueryType_FreeQuery, eQuestSubQuestDelete, PlayerID, txt, PlayerID);
}

void QuestSubQuestDeleteAll( DWORD PlayerID, DWORD mQuestIdx )
{
	sprintf(txt, "EXEC  %s %d, %d", STORED_QUEST_SUBQUEST_DELETE_ALL, PlayerID, mQuestIdx);
	g_DB.Query(eQueryType_FreeQuery, eQuestSubQuestDeleteAll, PlayerID, txt, PlayerID);
}

void QuestSubQuestInsert(DWORD PlayerID, DWORD mQuestIdx, DWORD sQuestIdx, QSTATETYPE Data, QSTATETYPE Time)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d", STORED_QUEST_SUBQUEST_INSERT, PlayerID, mQuestIdx, sQuestIdx, Data, Time);
	g_DB.Query(eQueryType_FreeQuery, eQuestSubQuestInsert, PlayerID, txt, PlayerID);
}

void QuestSubQuestUpdateToDB(DWORD PlayerID, DWORD mQuestIdx, DWORD sQuestIdx, QSTATETYPE Data, QSTATETYPE Time)
{
	//	char txt[128];
	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d", STORED_QUEST_SUBQUEST_UPDATE, PlayerID, mQuestIdx, sQuestIdx, Data, Time);
	g_DB.Query(eQueryType_FreeQuery, eQuestSubQuestUpdate, PlayerID, txt, PlayerID);
}

void QuestMainQuestUpdateCheckTime( DWORD dwCharIdx, DWORD dwQuestIdx, DWORD dwCheckType, DWORD dwCheckTime )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "dbo.MP_MAINQUEST_UPDATECHECKTIME %d, %d, %d, %d", dwCharIdx, dwQuestIdx, dwCheckType, dwCheckTime);
	g_DB.Query(eQueryType_FreeQuery, eQuestMainQuestUpdateCheckTime, dwCharIdx, txt, dwCharIdx);
}

// 071023 LYW --- MapDBMsgParser : Add Load query function.
void TutorialLoad(DWORD dwPlayerID, int tutorialIdx, BOOL isComplete)
{
	sprintf(txt, "EXEC	%s %d, %d, %d", STORED_TUTORIAL_LOAD, dwPlayerID, tutorialIdx, isComplete ) ;
	g_DB.Query(eQueryType_FreeQuery, eTutorialLoad, dwPlayerID, txt, dwPlayerID ) ;
}

// 071023 LYW --- MapDBMsgParser : Add Update query function.
void TutorialUpdateToDB(DWORD dwPlayerID, int tutorialIdx, BOOL isComplete)
{
	sprintf(txt, "EXEC	%s %d, %d, %d", STORED_TUTORIAL_UPDATE, dwPlayerID, tutorialIdx, isComplete) ;
	g_DB.Query(eQueryType_FreeQuery, eTutorialUpdate, dwPlayerID, txt, dwPlayerID) ;
}

// 071106 LYW --- MapDBMsgParser : 기본 스탯으로 업데이트 하는 함수.
void DB_UpdateToDefaultStats(DWORD dwPlayerID, int nStr, int nDex, int nVit, int nInt, int nWis, int nRemainPoint)
{
	sprintf(txt, "EXEC %s %d, %d, %d, %d, %d, %d, %d", 
			STORED_DEFAULT_STATS, dwPlayerID, nStr, nDex, nVit, nInt, nWis, nRemainPoint) ;

	g_DB.Query(eQueryType_FreeQuery, eUpdateToDefaultStats, dwPlayerID, txt, dwPlayerID) ;
}

// 071129 LYW --- MapDBMsgParser : 스킬 초기화 쿼리 함수 추가.
void DB_ResetToDefaultSkill(DWORD dwPlayerID)
{
	sprintf( txt, "EXEC %s %d", STORED_DEFAULT_SKILL, dwPlayerID ) ;
	g_DB.Query(eQueryType_FreeQuery, eSkill_Reset, dwPlayerID, txt, dwPlayerID) ;
}

// 071129 LYW --- MapDBMsgParser : 누적 스킬 포인트 업데이트 함수 추가.
void DB_UpdateAccumulateSkillPoint(DWORD dwPlayerID, BOOL bForce, int nPoint)
{
	sprintf( txt, "EXEC %s %d, %d, %d", STORED_UPDATE_ACCSKILLPOINT, dwPlayerID, bForce, nPoint ) ;
	g_DB.Query( eQueryType_FreeQuery, eUpdateAccumulateSkillpoint, dwPlayerID, txt, dwPlayerID ) ;
}


void InsertLogCharacter( DWORD charIdx, LEVELTYPE level, HERO_TOTALINFO* pTotalInfo )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_CHARACTERLOG %d, %d, %d, %d, %d, %d, %d, %d",
		charIdx,
		level,
		pTotalInfo->Str,
		pTotalInfo->Dex,
		pTotalInfo->Vit,
		pTotalInfo->Int,
		pTotalInfo->Wis,
		pTotalInfo->LevelUpPoint );
}


void InsertLogExp( BYTE bType, DWORD charIdx, LEVELTYPE level, EXPTYPE changeValue, EXPTYPE NowExp, WORD MurdererKind, DWORD MurdererIdx, DWORD CurAbilPoint)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_EXPPOINTLOG %d, %d, %d, %0.f, %0.f, %d, %d, %d",
		bType,
		charIdx,
		level,
		(float)changeValue,
		(float)NowExp,
		MurdererKind,
		MurdererIdx,
		CurAbilPoint );
}


void InsertLogMoney( BYTE type, DWORD charIdx, MONEYTYPE InvenMoney, MONEYTYPE ChangeValue, DWORD TargetID, MONEYTYPE TargetMoney )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_MONEYLOG %d, %d, %u, %u, %d, %u",	
		type,
		charIdx,
		InvenMoney,
		ChangeValue,
		TargetID,
		TargetMoney );
}

void InsertLogWorngMoney( BYTE type, DWORD charIdx, MONEYTYPE changeValue, MONEYTYPE InvenMoney, MONEYTYPE StorageMoney, DWORD targetIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_WRONGMONEYLOG %d, %d, %u, %u, %u, %d",
		type,
		charIdx,
		changeValue,
		InvenMoney,
		StorageMoney,
		targetIdx );	
}

void LogItemMoney(DWORD FromChrIdx, char* FromChrName, DWORD ToChrIdx, char* ToChrName,
				  eLogitemmoney LogType, MONEYTYPE FromTotalMoney, MONEYTYPE ToTotalMoney, MONEYTYPE ChangeMoney,
				  DWORD ItemIdx, DWORD ItemDBIdx, POSTYPE ItemFromPos, POSTYPE ItemToPos,
				  DURTYPE ItemDur, EXPTYPE ExpPoint)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_ITEMMONEYLOG %d, \'%s\', %d, \'%s\', %d, %u, %u, %u, %d, %d, %d, %d, %u, %0.f",
		FromChrIdx,
		FromChrName,
		ToChrIdx,
		ToChrName, 
		LogType,
		FromTotalMoney,
		ToTotalMoney,
		ChangeMoney,
		ItemIdx,
		ItemDBIdx,
		ItemFromPos,
		ItemToPos,
		ItemDur,
		// 아이템 로그에는 경험치를 보여줄 필요가 없다하여, 0으로 처리함.
		0 );
}

// 080111 KTH -- 케릭터 이름이 변경된 내용을 로그에 남긴다.
void LogChangeName(DWORD CharacterIDX, char* beforeName, char* afterName)
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(beforeName, "'") || IsCharInString(afterName, "'") )	return;

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_CHARRENAME_LOG_INSERT %d, \'%s\', \'%s\'",
		CharacterIDX,
		beforeName,
		afterName );
}

void LogCheat(DWORD Character_Idx, BYTE LogType, MONEYTYPE Param, DWORD ItemIdx, DWORD ItemDBIdx)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_CHEATE_LOG %d, %d, %d, %d, %d",
		Character_Idx,
		LogType,
		Param,
		ItemIdx,
		ItemDBIdx );
}

// 091102 LUJ, 값이 있는 옵션만 로그로 남긴다
void LogItemOption(DWORD playerIndex, DWORD itemDbIndex, const ITEM_OPTION& option, eLogitemmoney type )
{
	const ITEM_OPTION emptyOption = {0};

	if(0 == memcmp(&emptyOption, &option, sizeof(option)))
	{
		return;
	}

	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;
	const ITEM_OPTION::Enchant&		enchant		= option.mEnchant;

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.TP_ITEM_OPTION_LOG_INSERT %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, \'\'",
		playerIndex,
		itemDbIndex,
		reinforce.mStrength,
		reinforce.mDexterity,
		reinforce.mVitality,
		reinforce.mIntelligence,
		reinforce.mWisdom,
		reinforce.mLife,
		reinforce.mMana,
		reinforce.mLifeRecovery,
		reinforce.mManaRecovery,
		reinforce.mPhysicAttack,
		reinforce.mPhysicDefence,
		reinforce.mMagicAttack,
		reinforce.mMagicDefence,
		reinforce.mCriticalRate,
		reinforce.mCriticalDamage,
		reinforce.mMoveSpeed,
		reinforce.mEvade,
		reinforce.mAccuracy,
		mix.mStrength,
		mix.mIntelligence,
		mix.mDexterity,
		mix.mWisdom,
		mix.mVitality,
		enchant.mIndex,
		enchant.mLevel,
		type );
}

// 080327 LUJ, 드롭 옵션 로그 저장
void LogItemDropOption( const ITEM_OPTION& option, eLogitemmoney type )
{
	const ITEM_OPTION::Drop::Value& value1 = option.mDrop.mValue[ 0 ];
	const ITEM_OPTION::Drop::Value& value2 = option.mDrop.mValue[ 1 ];
	const ITEM_OPTION::Drop::Value& value3 = option.mDrop.mValue[ 2 ];
	const ITEM_OPTION::Drop::Value& value4 = option.mDrop.mValue[ 3 ];
	const ITEM_OPTION::Drop::Value& value5 = option.mDrop.mValue[ 4 ];

	g_DB.LogMiddleQuery(
		0,
		0,
		"INSERT INTO TB_ITEM_DROP_OPTION_LOG ( TYPE, ITEM_DBIDX, KEY1, VALUE1, KEY2, VALUE2, KEY3, VALUE3, KEY4, VALUE4, KEY5, VALUE5 ) VALUES ( %d, %d, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f )",
		type,
		option.mItemDbIndex,
		value1.mKey,
		value1.mValue,
		value2.mKey,
		value2.mValue,
		value3.mKey,
		value3.mValue,
		value4.mKey,
		value4.mValue,
		value5.mKey,
		value5.mValue );
}

void LogGuild(DWORD CharacterIdx, DWORD GuildIdx, WORD LogType, DWORD Param1, DWORD Param2)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_GUILDLOG %d, %d, %d, %d, %d",
		CharacterIdx,
		GuildIdx,
		LogType,
		Param1,
		Param2 );
}

void InsertLogSkill( CPlayer* player, const SKILL_BASE* skill, DWORD type )
{
	if( ! player ||
		! skill )
	{
		ASSERT( 0 );
		return;
	}

	// type의 값은 CommonGameDefine.h의 eLogSkill 참조할 것
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery( 0, 0, "EXEC dbo.TP_SKILL_LOG_INSERT %d, %d, %d, %d, %d, %d, \'\'",
		player->GetID(),
		player->GetSkillPoint(),
		skill->dwDBIdx,
		skill->mLearnedLevel,
		skill->wSkillIdx,
		type );
}


void InsertLogGuildWarehouse( CPlayer* player, POSTYPE previousPosition, const ITEMBASE* item, MONEYTYPE money, eLogitemmoney type )
{
	if( ! player  )
	{
		ASSERT( 0 );
		return;
	}

	if( item )
	{
		// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
		g_DB.LogMiddleQuery( 0, 0, "EXEC dbo.TP_GUILD_WAREHOUSE_LOG_INSERT %d, %d, %d, %d, %d, %d, %d, %u, %d, \'\'",
			player->GetID(),
			player->GetGuildIdx(),
			previousPosition,
			item->Position,
			item->wIconIdx,
			item->dwDBIdx,
			item->Durability,
			money,
			type );
	}
	else
	{
		// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
		g_DB.LogMiddleQuery( 0, 0, "EXEC dbo.TP_GUILD_WAREHOUSE_LOG_INSERT %d, %d, %d, %d, %d, %d, %d, %u, %d, \'\'",
			player->GetID(),
			player->GetGuildIdx(),
			previousPosition,
			0,
			0,
			0,
			0,
			money,
			type );
	}	
}


void InsertLogJob( CPlayer* player, DWORD classIndex, DWORD jobGrade, DWORD jobIndex, char* jobMemo )
{
	if( ! player )
	{
		ASSERT( 0 );
		return;
	}

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.TP_JOB_LOG_INSERT %d, %d, %d, %d, \'%s\'",
		player->GetID(),
		classIndex,
		jobGrade,
		jobIndex,
		jobMemo);
}


void InsertLogGuildScore( DWORD guildIndex, float score, eGuildLog type )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.TP_GUILD_SCORE_LOG_INSERT %d, %0.5f, %d, \'""\'",
		guildIndex,
		score,
		type );
}


void InsertLogFarmCrop( CSHFarm* farm, CSHCrop* crop, eFamilyLog type )
{
	if( ! farm	||
		! crop )
	{
		ASSERT( 0 );
		return;
	}

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.TP_FARM_CROP_LOG_INSERT %d, %d, %d, %d, %d, %d, %d, %d, \'""\'",
		farm->GetID(),
		crop->GetOwner(),
		crop->GetID(),
		crop->GetKind(),
		crop->GetStep(),
		crop->GetLife(),
		crop->GetSeedGrade(),
		type );
}

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2008.01.22
void InsertLogFarm( DWORD nZone, DWORD nFarm, DWORD nKind, DWORD nPlayerID, DWORD nValue01, DWORD nValue02)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.TP_FARM_LOG_INSERT %d, %d, %d, %d, %d, %d",
		nZone,
		nFarm,
		nKind,
		nPlayerID,
		nValue01,
		nValue02);
}

void InsertLogQuest( CPlayer* player, DWORD mainQuestIndex, DWORD subQuestIndex, int index, int variation, eQuestLog type )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.TP_QUEST_LOG_INSERT %d, %d, %d, %d, %d, %d, \'\'",
		player->GetID(),
		mainQuestIndex,
		subQuestIndex,
		index,
		variation,
		type );
}


void InsertLogSkillExp( DWORD charIdx, EXPTYPE changeValue, DWORD skillIdx, DWORD skillDBIdx, LEVELTYPE skillLevel, EXPTYPE skillExp, POSTYPE skillPos ) 
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_MUNGONGEXPLOG %d, %0.f, %d, %d, %d, %0.f, %d",
		charIdx,
		(float)changeValue,
		skillIdx,
		skillDBIdx,
		skillLevel,
		(float)skillExp,
		skillPos );
}

void UpdateCharacterInfoByTime(DWORD charIdx, EXPTYPE exp, /*어빌리티 삭제 - 이영준 EXPTYPE abilityExp,*/ MONEYTYPE money, DWORD time, DWORD LastPKEndTime )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC  %s %d, %0.f, %d, %u, %d, %d",	"dbo.MP_CHARACTER_UPDATEINFOBYTIME", 
		charIdx, (float)exp, 0, money, time, LastPKEndTime );
	// E 프로시저정리 추가 added by hseos 2007.06.21
	g_DB.Query(eQueryType_FreeQuery, eCharacterInfoByTime, 0, txt, charIdx);
}

void UpdateSkillExpByTime(DWORD charIdx, DWORD DBIdx, EXPTYPE exp)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC  %s %d, %d, %0.f",	"dbo.MP_SKILLTREE_UPDATEEXPBYTIME", 
		charIdx, DBIdx, (float)exp);
	g_DB.Query(eQueryType_FreeQuery, eSkillExpByTime, 0, txt, charIdx);
}

void InsertSpeedHackCheck(DWORD charIdx, char* charName, DWORD Distance, DWORD skillDelay)
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(charName, "'"))	return;

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC  %s %d, \'%s\', %d, %d",	"dbo.UP_SPEEDHACKCHECK", 
		charIdx, charName, Distance, skillDelay);
	g_DB.Query(eQueryType_FreeQuery, eSpeedHackCheck, 0, txt, charIdx);
}


void InsertLogTool( DWORD dwLogType, DWORD dwLogKind, DWORD dwOperIdx, char* sOperName, DWORD dwTargetIdx, char* sTargetName,
				   DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4 )
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(sOperName, "'") || IsCharInString(sTargetName, "'"))	return;

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_TOOLLOG %d, %d, %d, \'%s\', %d, \'%s\', %d, %d, %d, %d",
		dwLogType,
		dwLogKind,
		dwOperIdx,
		sOperName,
		dwTargetIdx,
		sTargetName,
		dwParam1,
		dwParam2,
		dwParam3,
		dwParam4 );
}


// 071122 LYW --- MapDBMsgParser : 봉인 해제 처리.
void UnSealItemByRealTime(DWORD dwPlayerID, DWORD dwDBIdx, DWORD dwTime)
{
	g_DB.FreeMiddleQuery(
		0,
		dwPlayerID,
		"EXEC dbo.MP_ITEM_UNSEALINGBYREALTIME %d, %d",
		dwDBIdx,
		dwTime);
}

void UnSealItemByGameTime(DWORD dwPlayerID, DWORD dwDBIdx, DWORD dwTime)
{
	g_DB.FreeMiddleQuery(
		0,
		dwPlayerID,
		"EXEC dbo.MP_ITEM_UNSEALINGBYGAMETIME %d, %d",
		dwDBIdx,
		dwTime);
}

void UpdateRemainTime(DWORD dwPlayerID, DWORD dwDBIdx, ITEM_SEAL_TYPE bySealState, DWORD dwTime)
{
	g_DB.FreeMiddleQuery(
		0,
		dwPlayerID,
		"EXEC dbo.MP_ITEM_UPDATEREMAINTIME %d, %d, %d",
		dwDBIdx,
		bySealState,
		dwTime);
}

void RCharacterNumSendAndCharacterInfo(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if(0 == pMessage->dwResult)
	{
		return;
	}

	BASEOBJECT_INFO Objinfo;
	ZeroMemory(
		&Objinfo,
		sizeof(Objinfo));
	CHARACTER_TOTALINFO Totalinfo = {0};
	HERO_TOTALINFO Heroinfo = {0};
	BASEMOVE_INFO Moveinfo;
	ZeroMemory(
		&Moveinfo,
		sizeof(Moveinfo));

	enum CHSelectInfo
	{
		eCS_ObjectID = 0, eCS_UserID, eCS_PartyID,eCS_Gender, eCS_Str, eCS_Life, eCS_Energe, eCS_Dex, eCS_Vit, eCS_Int, eCS_Wis, eCS_Fame,
		eCS_Name, eCS_Repute, eCS_Grade, eCS_Expoint, eCS_GradeUpPoint, eCS_Money,
		eCS_Wing, eCS_Vitality, eCS_Map, eCS_FaceType, eCS_HeadType, eCS_Hat, eCS_Weapon, eCS_Dress, eCS_Shoes,
		eCS_Costume_Hat, eCS_Costume_Dress, eCS_Glove, eCS_Costume_Shoes, eCS_Costume_Glove, eCS_Mask, eCS_MunpaIDX, eCS_BadFame,
		eCS_MapChangePoint, eCS_LoginPoint, eCS_Inventory, eCS_Pos_X, eCS_Pos_Y, eCS_Playtime, eCS_LastPKEndTime, eCS_MaxLevel, eCS_MarkName, eCS_MunpaCanEntryDate,
		eCS_NickName, eCS_Race, eCS_JobGrade, eCS_Job1, eCS_Job2, eCS_Job3, eCS_Job4, eCS_Job5, eCS_Job6, eCS_Shield, eCS_Glasses, eCS_Band,
		eCS_SkillPoint, eCS_Size, eCS_FamilyNickName,
	};

	Objinfo.dwObjectID = atoi((char*)pData->Data[eCS_ObjectID]);
	Objinfo.dwUserID =  atoi((char*)pData->Data[eCS_UserID]);
	Heroinfo.PartyID = atoi((char*)pData->Data[eCS_PartyID]);

	Totalinfo.Gender = BYTE( atoi((char*)pData->Data[eCS_Gender]) );
	Heroinfo.Str = atoi((char*)pData->Data[eCS_Str]);
	Totalinfo.Life = atoi((char*)pData->Data[eCS_Life]);
	Totalinfo.MaxLife = Totalinfo.Life;
	Heroinfo.Mana = atoi((char*)pData->Data[eCS_Energe]);
	Heroinfo.Dex = atoi((char*)pData->Data[eCS_Dex]);
	Heroinfo.Vit = atoi((char*)pData->Data[eCS_Vit]);
	Heroinfo.Int= atoi((char*)pData->Data[eCS_Int]);
	Heroinfo.Wis = atoi((char*)pData->Data[eCS_Wis]);
	Totalinfo.FamilyID = atoi((char*)pData->Data[eCS_Fame]);
	SafeStrCpy( Objinfo.ObjectName, (char*)pData->Data[eCS_Name], MAX_NAME_LENGTH + 1 );
	Totalinfo.Level = LEVELTYPE( atoi((char*)pData->Data[eCS_Grade]) );

	Heroinfo.ExpPoint = _atoi64((char*)pData->Data[eCS_Expoint]);
	Heroinfo.LevelUpPoint = LEVELTYPE( atoi((char*)pData->Data[eCS_GradeUpPoint]) );
	Heroinfo.Money = atoi((char*)pData->Data[eCS_Money]);
	Totalinfo.CurMapNum = GAMERESRCMNGR->GetLoadMapNum();
	Totalinfo.LoginMapNum = MAPTYPE( atoi((char*)pData->Data[eCS_Map]) );
	Totalinfo.FaceType = BYTE( atoi((char*)pData->Data[eCS_FaceType]) );
	Totalinfo.HairType = BYTE( atoi((char*)pData->Data[eCS_HeadType]) );
	Totalinfo.WearedItemIdx[eWearedItem_Hat] = atoi((char*)pData->Data[eCS_Hat]);
	Totalinfo.WearedItemIdx[eWearedItem_Weapon] = atoi((char*)pData->Data[eCS_Weapon]);
	Totalinfo.WearedItemIdx[eWearedItem_Dress] = atoi((char*)pData->Data[eCS_Dress]);
	Totalinfo.WearedItemIdx[eWearedItem_Shoes] = atoi((char*)pData->Data[eCS_Shoes]);
	Totalinfo.WearedItemIdx[eWearedItem_Glove] = atoi((char*)pData->Data[eCS_Glove]);
	Totalinfo.WearedItemIdx[eWearedItem_Shield] = atoi((char*)pData->Data[eCS_Shield]);
	Totalinfo.WearedItemIdx[eWearedItem_Band] = atoi((char*)pData->Data[eCS_Band]);
	Totalinfo.WearedItemIdx[eWearedItem_Costume_Head] = atoi((char*)pData->Data[eCS_Costume_Hat]);
	Totalinfo.WearedItemIdx[eWearedItem_Costume_Dress] = atoi((char*)pData->Data[eCS_Costume_Dress]);
	Totalinfo.WearedItemIdx[eWearedItem_Costume_Glove] = atoi((char*)pData->Data[eCS_Costume_Glove]);
	Totalinfo.WearedItemIdx[eWearedItem_Costume_Shoes] = atoi((char*)pData->Data[eCS_Costume_Shoes]);
	Totalinfo.WearedItemIdx[eWearedItem_Glasses] = atoi((char*)pData->Data[eCS_Glasses]);
	Totalinfo.WearedItemIdx[eWearedItem_Mask] = atoi((char*)pData->Data[eCS_Mask]);
	Totalinfo.WearedItemIdx[eWearedItem_Wing] = atoi((char*)pData->Data[eCS_Wing]);

	Heroinfo.SkillPoint = (DWORD)atoi((char*)pData->Data[eCS_SkillPoint]);

	Totalinfo.MunpaID = atoi((char*)pData->Data[eCS_MunpaIDX]);
	
	if(CGuild* const pGuild = GUILDMGR->GetGuild(Totalinfo.MunpaID))
	{
		SafeStrCpy(
			Totalinfo.GuildName,
			pGuild->GetGuildName(),
			_countof(Totalinfo.GuildName));

		if(const GUILDMEMBERINFO* const member = pGuild->GetMemberInfo(Objinfo.dwObjectID))
		{
			Totalinfo.PositionInMunpa = member->Rank;
		}
	}

	SafeStrCpy(Totalinfo.NickName, (char*)pData->Data[eCS_NickName], sizeof( Totalinfo.NickName ) );
	Totalinfo.BadFame = atoi((char*)pData->Data[eCS_BadFame]);


	Totalinfo.MapChangePoint_Index = WORD( atoi((char*)pData->Data[eCS_MapChangePoint]) );
	Totalinfo.LoginPoint_Index = WORD( atoi((char*)pData->Data[eCS_LoginPoint]) );
	Heroinfo.Playtime = atoi((char*)pData->Data[eCS_Playtime]);
	Heroinfo.LastPKModeEndTime = atoi((char*)pData->Data[eCS_LastPKEndTime]);

	Heroinfo.MaxLevel = LEVELTYPE( atoi((char*)pData->Data[eCS_MaxLevel]) );
	Totalinfo.MarkName	= atoi((char*)pData->Data[eCS_MarkName]);
	SafeStrCpy(Heroinfo.MunpaCanEntryDate, (char*)pData->Data[eCS_MunpaCanEntryDate], 11);

	Totalinfo.Race = BYTE( atoi((char*)pData->Data[eCS_Race]) );
	Totalinfo.JobGrade = BYTE( atoi((char*)pData->Data[eCS_JobGrade]) );
	Totalinfo.Job[0] = BYTE( atoi((char*)pData->Data[eCS_Job1]) );
	Totalinfo.Job[1] = BYTE( atoi((char*)pData->Data[eCS_Job2]) );
	Totalinfo.Job[2] = BYTE( atoi((char*)pData->Data[eCS_Job3]) );
	Totalinfo.Job[3] = BYTE( atoi((char*)pData->Data[eCS_Job4]) );
	Totalinfo.Job[4] = BYTE( atoi((char*)pData->Data[eCS_Job5]) );
	Totalinfo.Job[5] = BYTE( atoi((char*)pData->Data[eCS_Job6]) );
	Totalinfo.wInventoryExpansion = WORD( atoi((char*)pData->Data[eCS_Inventory]) );
	SafeStrCpy(
		Totalinfo.FamilyNickName,
		LPCTSTR(pData->Data[eCS_FamilyNickName]),
		_countof(Totalinfo.FamilyNickName));
	Totalinfo.Height = Totalinfo.Width = max(
		MIN_CHAR_CHANGESIZE,
		min(MAX_CHAR_CHANGESIZE, float(_tstof(LPCTSTR(pData->Data[eCS_Size]))) / 100.0f));

	if(Heroinfo.MaxLevel < Totalinfo.Level)
		Heroinfo.MaxLevel = Totalinfo.Level;

	CPlayer* pPlayer = g_pServerSystem->InitPlayerInfo(&Objinfo, &Totalinfo, &Heroinfo);
	if(pPlayer == NULL)
		return;

	g_pUserTable->SetObjectIndex(
		pPlayer->GetObjectName(),
		pPlayer->GetID());

	pPlayer->GetMoveInfo()->CurPosition.x = (float)atoi((char*)pData->Data[eCS_Pos_X]);
  	pPlayer->GetMoveInfo()->CurPosition.z = (float)atoi((char*)pData->Data[eCS_Pos_Y]);

	if(g_pServerSystem->GetMapNum() == HOUSINGMAPNUM)
	{
		if(VECTOR3* const pStartPos = HOUSINGMGR->GetStartPosFromeservationList(pPlayer->GetID()))
		{
			pPlayer->GetMoveInfo()->CurPosition = *pStartPos;
		}
	}
	else if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
	{
		if(VECTOR3* const pStartPos = DungeonMGR->GetStartPosFromeservationList(pPlayer->GetID()))
		{
			pPlayer->GetMoveInfo()->CurPosition = *pStartPos;
		}
		else if(pPlayer->IsDungeonObserver())
		{
			pPlayer->SetHideLevel(
				1);
			pPlayer->GetMoveInfo()->CurPosition = *pPlayer->GetDungeonObserverPos();
		}
	}
	else if(g_pServerSystem->GetMapNum() == GTMAPNUM)
	{
		pPlayer->GetStartPosOnGTMAP(
			pPlayer->GetMoveInfo()->CurPosition);
	}
	else if(g_csDateManager.IsChallengeZone(g_pServerSystem->GetMapNum()))
	{
		if(MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfo(Totalinfo.MapChangePoint_Index))
		{
			pPlayer->GetMoveInfo()->CurPosition = ChangeInfo->MovePoint;
		}
	}
	else
	{
		VECTOR3 RandPos = {0};
		RandPos.x = pPlayer->GetMoveInfo()->CurPosition.x + rand() % 500 - 250;
		RandPos.z = pPlayer->GetMoveInfo()->CurPosition.z + rand() % 500 - 250;

		const float fXpos = (RandPos.x/TILECOLLISON_DETAIL) * TILECOLLISON_DETAIL;
		const float fZpos = (RandPos.z/TILECOLLISON_DETAIL) * TILECOLLISON_DETAIL;

		if(FALSE == g_pServerSystem->GetMap()->CollisionTilePos(fXpos,fZpos,g_pServerSystem->GetMapNum()))
		{
			pPlayer->GetMoveInfo()->CurPosition = RandPos;
		}
	}

	// 100811 ShinJS 좌표가 비정상적인 경우 안전지대로 이동
	if( abs( pPlayer->GetMoveInfo()->CurPosition.x ) < TILECOLLISON_DETAIL &&
		abs( pPlayer->GetMoveInfo()->CurPosition.z ) < TILECOLLISON_DETAIL )
	{
		VECTOR3* pRevivePos = GAMERESRCMNGR->GetRevivePoint();
		VECTOR3 pos={0};
		pos.x = pRevivePos->x + (rand() % 500 - 250);
		pos.z = pRevivePos->z + (rand() % 500 - 250);

		// 안전지대의 좌표가 이상한경우
		if( abs( pos.x ) < TILECOLLISON_DETAIL &&
			abs( pos.z ) < TILECOLLISON_DETAIL )
		{
			FILE* fpLog = NULL;
			fpLog = fopen( "./Log/LoginPositionError.txt", "a+" );
			if( fpLog )
			{
				SYSTEMTIME sysTime;
				GetLocalTime( &sysTime );

				fprintf( fpLog, "[%04d-%02d-%02d %02d:%02d:%02d] Map:d, PlayerID:%d, PlayerName:%s, MapChangePoint:%d, RevivePoint:(%.2f,%.2f)\n", 
					sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
					GAMERESRCMNGR->GetLoadMapNum(),
					pPlayer->GetID(),
					pPlayer->GetObjectName(),
					Totalinfo.MapChangePoint_Index,
					pRevivePos->x,
					pRevivePos->z );

				fclose( fpLog );
			}
		}

		pPlayer->GetMoveInfo()->CurPosition.x = pos.x;
		pPlayer->GetMoveInfo()->CurPosition.z = pos.z;
	}


	// guildunion info setting
	GUILDUNIONMGR->SetInfoToPlayer( pPlayer, Totalinfo.MunpaID );

	pPlayer->SetInitState(PLAYERINITSTATE_HERO_INFO,pMessage->dwID);

//---KES AUTONOTE
	AutoNoteListLoad( Objinfo.dwObjectID );

//---------------

	// 081022 KTH --
	CHARCALCMGR->AddPlayerJobSkill(pPlayer);
}

void RCharacterSkillTreeInfo(LPQUERY pData, LPDBMESSAGE pMessage)
{

	DWORD count = pMessage->dwResult;

	if((count == 1) && (atoi((char*)pData->Data[0]) == 0))
	{
		// ￥i¡IAIAI¡Æ￠® ¨uⓒª¨ui¨u¡ⓒ PlayerID￠￢| ⓒoYE?CO ¡Æⓒ¡￠?i¡ÆE¡ic
		CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(atoi((char*)pData->Data[1]));
		if(pPlayer == NULL)
			return;

		pPlayer->SetInitState(PLAYERINITSTATE_SKILL_INFO,pMessage->dwID);
	}
	else
	{
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(atoi((char*)pData[0].Data[0]));

		if(pPlayer == NULL)
			return;

		for( DWORD i = 0; i < count; ++i )
		{
			const QUERYST& record = pData[i];

			SKILL_BASE skill = {0};
			skill.dwDBIdx = atoi(LPCTSTR(record.Data[1]));
			skill.wSkillIdx = atoi(LPCTSTR(record.Data[2]));
			skill.mLearnedLevel = LEVELTYPE(atoi(LPCTSTR(record.Data[3])));
			skill.Level = skill.mLearnedLevel;

			const SKILL_BASE* const oldSkill = pPlayer->GetSkillTree().GetData(
				skill.wSkillIdx);

			if(oldSkill)
			{
				skill.Level = skill.Level + oldSkill->Level;
			}

			pPlayer->GetSkillTree().Update(
				skill);
		}

		pPlayer->SetInitState(PLAYERINITSTATE_SKILL_INFO,pMessage->dwID);
	}
}

void RCharacterItemInfo(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if( ! pMessage->dwResult )
	{
		return;
	}

	const DWORD playerIndex = atoi( ( char* )pData->Data[ 0 ] );

	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser( playerIndex );

	if( ! pPlayer)
	{
		return;
	}

	if( 1 == pMessage->dwResult  &&
		atoi((char*)pData->Data[0]) == 0 )
	{
		ITEM_TOTALINFO Iteminfo;
		memset(&Iteminfo, 0, sizeof(ITEM_TOTALINFO));
		pPlayer->InitItemTotalInfo(&Iteminfo);
		return;
	}

	MSG_ITEM_OPTION optionMessage;
	{
		ZeroMemory( &optionMessage, sizeof( optionMessage ) );

		optionMessage.Category	= MP_ITEM;
		optionMessage.Protocol	= MP_ITEM_OPTION_ADD_ACK;
	}

	//진짜 주소를 가지고 와야.. InitItemTotalInfo() 함수 안에서의 for문 100번을 막을 수 있다.
	ITEM_TOTALINFO Iteminfo;
	ZeroMemory( &Iteminfo, sizeof( Iteminfo ) );

	const int maxoverlap = 100;
	int overlapcount = 0;
	ITEMBASE OverLapItem[maxoverlap];
	memset(&OverLapItem, 0, sizeof(ITEMBASE)*maxoverlap);
	memset(&Iteminfo, 0, sizeof(ITEM_TOTALINFO));
	for(DWORD  i = 0; i < pMessage->dwResult; i++)
	{
		POSTYPE ItemPos = POSTYPE( atoi((char*)pData[i].Data[3]) );
		ITEMBASE* pItemBase = NULL;
		// ¡¾¡¿￠￢￠c￥ia(AI¨￢¡IAa￠￢￠c)¨u¨¡AIAU ¨uA¨¡A
		//if(ItemPos >= TP_INVENTORY_START && ItemPos < TP_INVENTORY_END)
		// 071210 KTH -- Fix "TP_INVENTORY_END에서 TP_EXTENDED_INVENTORY2_END로 사이즈를 늘린다."
		if(ItemPos >= TP_INVENTORY_START && ItemPos < TP_EXTENDED_INVENTORY2_END)
		{
			ItemPos -= TP_INVENTORY_START;
			pItemBase = &Iteminfo.Inventory[ItemPos];
		}
		// AaA짢횕짢짙징짠u징짠짖짰AIAU 징짠uA징짠짖짰A
		else if(ItemPos >= TP_WEAR_START && ItemPos < TP_WEAR_END)
		{				
			POSTYPE ItemGrid = POSTYPE( ItemPos - TP_WEAR_START );
			pItemBase = &Iteminfo.WearedItem[ItemGrid];
		}

		if(pItemBase == NULL)
		{
			// ￠?¨I¡¾a￠?￠® ￥ie¨ui￠?8e ¨u¨¡AIAU ¨¡¡AAo¨uC ￠?￠®¡¤?AO
			ASSERT(0);
			continue;
		}

		//아이템 포지션이 겹쳤다.
		BOOL bOverlap = FALSE;

		if(pItemBase->dwDBIdx != 0 && overlapcount < maxoverlap)
		{
			pItemBase = &OverLapItem[overlapcount];
			++overlapcount;

			bOverlap = TRUE;
		}
		
		pItemBase->dwDBIdx = atoi((char*)pData[i].Data[1]);
		pItemBase->wIconIdx = atoi((char*)pData[i].Data[2]);
		pItemBase->Position = POSTYPE( atoi((char*)pData[i].Data[3]) );
		pItemBase->QuickPosition = POSTYPE( atoi((char*)pData[i].Data[4]) );
		pItemBase->Durability = atoi((char*)pData[i].Data[5]);
		pItemBase->ItemParam = atoi((char*)pData[i].Data[6]);
		pItemBase->nSealed = ITEM_SEAL_TYPE(atoi((char*)pData[i].Data[7]));

		ITEM_INFO* ItemInfo = ITEMMGR->GetItemInfo( pItemBase->wIconIdx );

		if( ItemInfo == NULL )
			continue;

		// 080820 LUJ, 획득 시 쿨타임을 갖는 아이템은 itemParam에 쿼리한 시간을 설정해서, 경과된 시간을 계산한다
		if( ItemInfo &&
			ItemInfo->SupplyType == ITEM_KIND_COOLTIME )
		{
			pItemBase->ItemParam 		= gCurTime;
			pItemBase->nRemainSecond 	= (int)atoi((char*)pData[i].Data[8]);
		}
		
		// 091110 pdy eITEM_TYPE_GET_UNSEAL에 대한 처리추가
		if( pItemBase->nSealed  == eITEM_TYPE_GET_UNSEAL )
		{
			DWORD dwRemainTime = 0;

			if( ItemInfo->nTimeKind == eKIND_REALTIME )
			{
				// 최초 DBInsert경과 시간을 구한다. 
				DWORD dwRateDBInsertTime = (DWORD) abs( atoi((char*)pData[i].Data[8]) );

				// DBInsert 경과 시간이 사용기간보다 크거나 같을 경우엔 이미 기간이 지났음으로 삭제
				if( dwRateDBInsertTime >= ItemInfo->dwUseTime )
				{
					// 071231 KTH -- GetID Add
					ItemDeleteToDB(pItemBase->dwDBIdx );
					
					LogItemMoney(pPlayer->GetID(), pPlayer->GetObjectName(), 0, "",
						eLog_ShopItemUseEnd, pPlayer->GetMoney(eItemTable_Inventory), 0, 0,
						pItemBase->wIconIdx , pItemBase->dwDBIdx, pItemBase->Position, 0, pItemBase->Durability, pPlayer->GetPlayerExpPoint());

					memset(pItemBase, 0, sizeof(ITEMBASE));

					if( bOverlap )
						--overlapcount;

					// 아이템을 삭제 했으니 건너뛴다.
					continue;
				}

				// 사용기간에서 최초 DBInsert경과 시간을 빼준다.
				dwRemainTime = ItemInfo->dwUseTime - dwRateDBInsertTime;

				UnSealItemByRealTime(pPlayer->GetID(), pItemBase->dwDBIdx, dwRemainTime );
			}
			else if( ItemInfo->nTimeKind ==	eKIND_PLAYTIME )
			{
				dwRemainTime = ItemInfo->dwUseTime;

				UnSealItemByGameTime(pPlayer->GetID(), pItemBase->dwDBIdx, dwRemainTime );
			}

			pItemBase->nSealed = eITEM_TYPE_UNSEAL ;
			pItemBase->nRemainSecond = dwRemainTime;
		}
		else if( pItemBase->nSealed == eITEM_TYPE_UNSEAL && ItemInfo->nTimeKind == eKIND_REALTIME ) 
		{
			pItemBase->nRemainSecond = (int)atoi((char*)pData[i].Data[8]) ;	//DB 에서 음수값이 넘어올 수도 있다.

			if( pItemBase->nRemainSecond <= 0)
			{
				// 071231 KTH -- GetID Add
				ItemDeleteToDB(pItemBase->dwDBIdx );
				
				LogItemMoney(pPlayer->GetID(), pPlayer->GetObjectName(), 0, "",
					eLog_ShopItemUseEnd, pPlayer->GetMoney(eItemTable_Inventory), 0, 0,
					pItemBase->wIconIdx , pItemBase->dwDBIdx, pItemBase->Position, 0, pItemBase->Durability, pPlayer->GetPlayerExpPoint());

				memset(pItemBase, 0, sizeof(ITEMBASE));

				if( bOverlap )
					--overlapcount;

				// 아이템을 삭제 했으니 건너뛴다.
				continue;
			}
		}
		else
		{
			pItemBase->nRemainSecond = (int)atoi((char*)pData[i].Data[9]) ;
		}

		/*
		080130 LUJ, 아이템 정보를 가져오는 작업은 (1) 옵션 가져오기 (2) 아이템 정보
					가져오기, 이렇게 두 부분으로 구성되었었다. 테이블이 플레이어나 
					유저에 따라 분리되어 있지 않기 때문에, 부득이하게 매우 큰 크기인
					TB_ITEM 테이블을 두 번 스캔해야했다. 이를 피하기 위해 TB_ITEM과
					TB_ITEM_OPTION을 외부 조인으로 결합시켜 스캔 회수를 1번으로 줄였다.
					한편, 드롭 아이템에 랜덤으로 붙은 TB_ITEM_DROP_OPTION도 조인으로
					결합시켜 가져온다
		*/
		{
			const QUERYST& record = pData[ i ];

			ITEM_OPTION& option = optionMessage.mOption[ optionMessage.mSize ];

			{
				ITEM_OPTION::Reinforce& data = option.mReinforce;

				data.mStrength			= atoi( ( char* )record.Data[ 10 ] );
				data.mDexterity			= atoi( ( char* )record.Data[ 11 ] );
				data.mVitality			= atoi( ( char* )record.Data[ 12 ] );
				data.mIntelligence		= atoi( ( char* )record.Data[ 13 ] );
				data.mWisdom			= atoi( ( char* )record.Data[ 14 ] );
				data.mLife				= atoi( ( char* )record.Data[ 15 ] );
				data.mMana				= atoi( ( char* )record.Data[ 16 ] );
				data.mLifeRecovery		= atoi( ( char* )record.Data[ 17 ] );
				data.mManaRecovery		= atoi( ( char* )record.Data[ 18 ] );
				data.mPhysicAttack		= atoi( ( char* )record.Data[ 19 ] );
				data.mPhysicDefence		= atoi( ( char* )record.Data[ 20 ] );
				data.mMagicAttack		= atoi( ( char* )record.Data[ 21 ] );
				data.mMagicDefence		= atoi( ( char* )record.Data[ 22 ] );
				data.mMoveSpeed			= atoi( ( char* )record.Data[ 23 ] );
				data.mEvade				= atoi( ( char* )record.Data[ 24 ] );
				data.mAccuracy			= atoi( ( char* )record.Data[ 25 ] );
				data.mCriticalRate		= atoi( ( char* )record.Data[ 26 ] );
				data.mCriticalDamage	= atoi( ( char* )record.Data[ 27 ] );
				SafeStrCpy( option.mReinforce.mMadeBy, ( char* )record.Data[ 28 ], sizeof( option.mReinforce.mMadeBy ) );
			}

			{
				ITEM_OPTION::Mix& data = option.mMix;

				data.mStrength		= atoi( ( char* )record.Data[ 29 ] );
				data.mIntelligence	= atoi( ( char* )record.Data[ 30 ] );
				data.mDexterity		= atoi( ( char* )record.Data[ 31 ] );
				data.mWisdom		= atoi( ( char* )record.Data[ 32 ] );
				data.mVitality		= atoi( ( char* )record.Data[ 33 ] );
				SafeStrCpy( option.mMix.mMadeBy, ( char* )record.Data[ 34 ], sizeof( option.mMix.mMadeBy ) );
			}

			{
				ITEM_OPTION::Enchant& data = option.mEnchant;

				data.mIndex	= atoi( ( char* )record.Data[ 35 ] );
				data.mLevel	= BYTE( atoi( ( char* )record.Data[ 36 ] ) );
				SafeStrCpy( option.mEnchant.mMadeBy, ( char* )record.Data[ 37 ], sizeof( option.mEnchant.mMadeBy ) );				
			}

			{
				ITEM_OPTION::Drop& data = option.mDrop;

				data.mValue[ 0 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 38 ] ) );
				data.mValue[ 0 ].mValue	= float( atof( ( char* )record.Data[ 39 ] ) );

				data.mValue[ 1 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 40 ] ) );
				data.mValue[ 1 ].mValue	= float( atof( ( char* )record.Data[ 41 ] ) );

				data.mValue[ 2 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 42 ] ) );
				data.mValue[ 2 ].mValue	= float( atof( ( char* )record.Data[ 43 ] ) );

				data.mValue[ 3 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 44 ] ) );
				data.mValue[ 3 ].mValue	= float( atof( ( char* )record.Data[ 45 ] ) );

				data.mValue[ 4 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 46 ] ) );
				data.mValue[ 4 ].mValue	= float( atof( ( char* )record.Data[ 47 ] ) );
			}

			const ITEM_OPTION emptyOption = { 0 };

			if( memcmp( &emptyOption, &option, sizeof( emptyOption ) ) )
			{
				option.mItemDbIndex	= pItemBase->dwDBIdx;
				ITEMMGR->AddOption( option );

				++optionMessage.mSize;
			}
		}

		// 081202 LUJ, 수량성 아이템의 개수가 0인 경우, 삭제한다
		if(		ItemInfo->Stack &&
			!	pItemBase->Durability )
		{
			LogItemMoney(
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				0,
				"*invalid",
				eLog_ItemDiscard,
				pPlayer->GetMoney(),
				0,
				0,
				pItemBase->wIconIdx,
				pItemBase->dwDBIdx,
				pItemBase->Position,
				0,
				pItemBase->Durability,
				0 );
			ItemDeleteToDB( pItemBase->dwDBIdx );
			ZeroMemory( pItemBase, sizeof( *pItemBase ) );
		}
	}

	//중복된 아이템이 있으면 빈칸에 넣어준다.
	for(int n=0;n<overlapcount;++n)
	{
		// 071210 KTH -- 확장인벤토리까지 체크한다.
		for(int m=TP_INVENTORY_START;m<TP_EXTENDED_INVENTORY2_END;++m)
		{
			//빈칸이면 넣어준다.
			if(Iteminfo.Inventory[m].dwDBIdx == 0)
			{
				Iteminfo.Inventory[m] = OverLapItem[n];
				Iteminfo.Inventory[m].Position = POSTYPE( m );
				break;
			}
		}

		//만약 빈칸이 없으면 더이상 처리하지 않음
		//if(m==TP_INVENTORY_END)
		// 071210 KTH -- 확장인벤토리를 넘으면 break;
		if(m==TP_EXTENDED_INVENTORY2_END)
			break;
	}

	pPlayer->SendMsg( &optionMessage, optionMessage.GetSize() );
	pPlayer->InitItemTotalInfo(&Iteminfo);
	pPlayer->SetInitState(PLAYERINITSTATE_ITEM_INFO, MP_USERCONN_GAMEIN_SYN );
}

void RSSItemInsert(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if((atoi((char*)pData->Data[0]) == 0))
	{
		// Insert ¨oC¨¡¨￠

	}
	else
	{
		DWORD dwPlayerID = atoi((char*)pData->Data[0]);
		ITEMBASE info;
		info.dwDBIdx = atoi((char*)pData->Data[1]);
		info.wIconIdx = atoi((char*)pData->Data[2]);
		info.Position = POSTYPE(atoi((char*)pData->Data[3]));
		info.QuickPosition = POSTYPE( atoi((char*)pData->Data[4]));
		info.Durability = atoi((char*)pData->Data[5]);
		info.nSealed = ITEM_SEAL_TYPE(atoi((char*)pData->Data[8]));
		info.nRemainSecond = atoi((char*)pData->Data[9]);

		STREETSTALLMGR->CreateDupItem( dwPlayerID, &info, pMessage->dwID);
	}
}

void ItemOptionInsert(const ITEM_OPTION& option, DWORD CharacterIdx )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;
	const ITEM_OPTION::Enchant&		enchant		= option.mEnchant;

	// 090325 ONS 필터링문자체크
	char szBufReinforceName[MAX_NAME_LENGTH+1]	= { 0 };
	char szBufmixName[MAX_NAME_LENGTH+1]		= { 0 };
	char szBufEnchantName[MAX_NAME_LENGTH+1]	= { 0 };

	SafeStrCpy(szBufReinforceName, reinforce.mMadeBy, MAX_NAME_LENGTH+1);
	SafeStrCpy(szBufmixName, mix.mMadeBy, MAX_NAME_LENGTH+1);
	SafeStrCpy(szBufEnchantName, enchant.mMadeBy, MAX_NAME_LENGTH+1);

	if(IsCharInString(szBufReinforceName, "'") || IsCharInString(szBufmixName, "'") || IsCharInString(szBufEnchantName, "'"))	return;
	
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		CharacterIdx,
		"EXEC dbo.MP_ITEM_OPTION_INSERT %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, \'%s\', %d, %d, %d, %d, %d, \'%s\', %d, %d, \'%s\'",
		option.mItemDbIndex,
		reinforce.mStrength,
		reinforce.mDexterity,
		reinforce.mVitality,
		reinforce.mIntelligence,
		reinforce.mWisdom,
		reinforce.mLife,
		reinforce.mMana,
		reinforce.mLifeRecovery,
		reinforce.mManaRecovery,
		reinforce.mPhysicAttack,
		reinforce.mPhysicDefence,
		reinforce.mMagicAttack,
		reinforce.mMagicDefence,
		reinforce.mMoveSpeed,
		reinforce.mEvade,
		reinforce.mAccuracy,
		reinforce.mCriticalRate,
		reinforce.mCriticalDamage,
		reinforce.mMadeBy,
		mix.mStrength,
		mix.mIntelligence,
		mix.mDexterity,
		mix.mWisdom,
		mix.mVitality,
		mix.mMadeBy,
		enchant.mIndex,
		enchant.mLevel,
		enchant.mMadeBy );
}


void ItemDropOptionInsert( const ITEM_OPTION& option, DWORD CharacterIdx )
{
	const ITEM_OPTION::Drop::Value& value1 = option.mDrop.mValue[ 0 ];
	const ITEM_OPTION::Drop::Value& value2 = option.mDrop.mValue[ 1 ];
	const ITEM_OPTION::Drop::Value& value3 = option.mDrop.mValue[ 2 ];
	const ITEM_OPTION::Drop::Value& value4 = option.mDrop.mValue[ 3 ];
	const ITEM_OPTION::Drop::Value& value5 = option.mDrop.mValue[ 4 ];

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		CharacterIdx,
		"EXEC dbo.MP_ITEM_DROP_OPTION_INSERT %d, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f",
		option.mItemDbIndex,
		value1.mKey,
		value1.mValue,
		value2.mKey,
		value2.mValue,
		value3.mKey,
		value3.mValue,
		value4.mKey,
		value4.mValue,
		value5.mKey,
		value5.mValue );
}


/* New Item Return Func */
void RItemInsert(LPQUERY pData, LPDBMESSAGE pMessage)
{
	WORD lastNo = LOWORD(pMessage->dwID);
	WORD ArrayID = HIWORD(pMessage->dwID);

	if( pMessage->dwResult )
	{
		DWORD dwDBIdx = atoi((char*)pData->Data[1]);
		DWORD dwItemIdx = atoi((char*)pData->Data[2]);
		DWORD dwPos = atoi((char*)pData->Data[3]);
		DWORD dwDura = atoi((char*)pData->Data[5]);

		ITEM_SEAL_TYPE nSealed = ITEM_SEAL_TYPE(atoi((char*)pData->Data[7]));
		int nRemainTime = atoi((char*)pData->Data[8]);

		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(atoi((char*)pData->Data[0]));
		if(pPlayer == NULL)
			return;
		if(pPlayer->GetInited() == FALSE)
			return;

		ITEMOBTAINARRAYINFO * pItemArrayInfo = pPlayer->GetArray(ArrayID);
		if(pItemArrayInfo == NULL)
			return;

		const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo( dwItemIdx );

		if( ! itemInfo )
		{
			return;
		}

		switch(itemInfo->wSeal)
		{
		case eITEM_TYPE_GET_UNSEAL:
			{
				if( itemInfo->nTimeKind == eKIND_REALTIME )
				{
					UnSealItemByRealTime(pPlayer->GetID(), dwDBIdx, itemInfo->dwUseTime);
				}
				else if( itemInfo->nTimeKind ==	eKIND_PLAYTIME )
				{
					UnSealItemByGameTime(pPlayer->GetID(), dwDBIdx, itemInfo->dwUseTime );
				}

				nSealed = eITEM_TYPE_UNSEAL;
				nRemainTime = itemInfo->dwUseTime;
			}
			break;
		}

		pItemArrayInfo->ItemArray.AddItem( atoi((char*)pData->Data[1]), 
			atoi((char*)pData->Data[2]), 
			atoi((char*)pData->Data[5]), 
			POSTYPE(atoi((char*)pData->Data[3])),
			POSTYPE(atoi((char*)pData->Data[4])),
			// LUJ, 쿨타임을 가진 아이템은 ItemParam 필드를 최근 체크한 시각을 넣는데 사용한다.
			ITEM_KIND_COOLTIME == itemInfo->SupplyType ? gCurTime : atoi((char*)pData->Data[6]),
			0,
			nSealed,
			nRemainTime);

		if( lastNo == pItemArrayInfo->ItemArray.ItemNum )
		{
			pItemArrayInfo->CallBack( pPlayer, ArrayID );
			//ITEMMGR->ObtainItemDBResult( pPlayer, ArrayID);
		}
		else
		{
			LogItemMoney(
				0,
				"",
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				eLogitemmoney( pItemArrayInfo->wType ),
				0,
				pPlayer->GetMoney(),
				0,
				dwItemIdx,
				dwDBIdx,
				0,
				POSTYPE( dwPos ),
				dwDura,
				pPlayer->GetPlayerExpPoint() );
		}
	}
	else
	{
		ASSERT(0);
	}
}

/* New Skill Return Func */
void RSkillInsert(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if(1 != pMessage->dwResult)
	{
		return;
	}

	const QUERYST& record = pData[0];
	CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(
		pMessage->dwID);

	if(0 == player)
	{
		return;
	}
	else if(eObjectKind_Player != player->GetObjectKind())
	{
		return;
	}

	SKILL_BASE Skill = {0};
	Skill.dwDBIdx = _ttoi(LPCTSTR(record.Data[1]));
	Skill.wSkillIdx = _ttoi(LPCTSTR(record.Data[2]));
	Skill.mLearnedLevel = BYTE(_ttoi(LPCTSTR(record.Data[3])));
	Skill.Level = Skill.mLearnedLevel;

	const SKILL_BASE* oldSkill = player->GetSkillTree().GetData(
		Skill.wSkillIdx);

	if(oldSkill)
	{
		Skill.Level = Skill.Level + oldSkill->Level;
	}

	SKILLTREEMGR->AddSkillDBResult(
		player,
		&Skill);
	InsertLogSkill(
		player,
		&Skill,
		eLog_SkillLearn);
}

void RPartyLoad(LPQUERY pData, LPDBMESSAGE pMessage)
{
	DWORD PartyIDX = 0;
	for(DWORD i=0; i<pMessage->dwResult;++i)
	{
		PartyIDX = atoi((char*)pData[i].Data[0]);
		PARTYMGR->RegistParty(PartyIDX);
	}
	if(pMessage->dwResult == MAX_PARTY_LOAD_NUM)
	{
		PartyLoad(PartyIDX);
	}
}

void RPartyCreate(LPQUERY pData, LPDBMESSAGE pMessage) 
{
	const DWORD CreateErr = atoi((char*)pData->Data[0]);
	const DWORD MasterIdx = atoi((char*)pData->Data[2]);
	const BYTE Option = BYTE(atoi((char*)pData->Data[3]));
	const BYTE SubOption = BYTE(atoi((char*)pData->Data[4]));
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(MasterIdx);
	if(!pPlayer)
		return;

	if(CreateErr == NULL)
	{
		PARTYMGR->SendErrMsg(MasterIdx, eErr_Create_DB, MP_PARTY_CREATE_NACK);
		return;	
	}
	
	const DWORD PartyIdx = atoi((char*)pData->Data[1]);	
	PARTYMGR->CreatePartyResult(pPlayer,PartyIdx,Option, SubOption);
}

void RPartyInfoByUserLogin(LPQUERY pData, LPDBMESSAGE pMessage) 
{
	CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);

	if(0 == player)
	{
		return;
	}

	CParty* party = PARTYMGR->GetParty(player->GetPartyIdx());

	if(0 == party)
	{
		party = PARTYMGR->RegistParty(player->GetPartyIdx());
	}

	PARTY_MEMBER member[MAX_PARTY_LISTNUM] = {0};

	for(DWORD i = 0;i < pMessage->dwResult; ++i)
	{
		PARTY_MEMBER& lhs = member[i];
		const QUERYST& record = pData[i];
		lhs.dwMemberID = _ttoi(LPCTSTR(record.Data[0]));
		SafeStrCpy(
			lhs.Name,
			LPCTSTR(record.Data[1]),
			sizeof(lhs.Name) / sizeof(*lhs.Name));
		
		const MAPTYPE map = MAPTYPE(_ttoi(LPCTSTR(record.Data[2])));
		lhs.bLogged = (0 < map);
		lhs.mMapType = map;
		lhs.Level = LEVELTYPE(_ttoi(LPCTSTR(record.Data[3])));
		const BYTE option = BYTE(_ttoi(LPCTSTR(record.Data[4])));

		lhs.mJobGrade	= BYTE(_ttoi(LPCTSTR(record.Data[5])));
		lhs.mJob[ 0 ]	= BYTE(_ttoi(LPCTSTR(record.Data[6])));
		lhs.mJob[ 1 ]	= BYTE(_ttoi(LPCTSTR(record.Data[7])));
		lhs.mJob[ 2 ]	= BYTE(_ttoi(LPCTSTR(record.Data[8])));
		lhs.mJob[ 3 ]	= BYTE(_ttoi(LPCTSTR(record.Data[9])));
		lhs.mJob[ 4 ]	= BYTE(_ttoi(LPCTSTR(record.Data[10])));
		lhs.mJob[ 5 ]	= BYTE(_ttoi(LPCTSTR(record.Data[11])));
		lhs.mRace		= BYTE(_ttoi(LPCTSTR(record.Data[12])));

		party->SetMember(
			i,
			lhs);
		party->SetOption(option);
	}

	// 091127 LUJ, 잘못된 정보가 있을 수 있으므로 멤버 이외의 정보들도 초기화해준다
	for(DWORD i = pMessage->dwResult; i < MAX_PARTY_LISTNUM; ++i)
	{
		const PARTY_MEMBER emptyMember = {0};
		party->SetMember(
			i,
			emptyMember);
	}

	party->SetPartyMemberNum(
		pMessage->dwResult);
	party->UserLogIn(
		player,
		TRUE);
}

void RPartyChangeMaster(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CParty* pParty = PARTYMGR->GetParty(pMessage->dwID);
	if(!pParty)
	{
		ASSERT(0);
		return;
	}

	CPlayer* pMaster = (CPlayer*)g_pUserTable->FindUser(pParty->GetMasterID());

	if(atoi((char*)pData->Data[0]) == 0)
	{
		const DWORD targetObjectIndex = atoi((char*)pData->Data[1]);

		PARTYMGR->DoChangeMasterParty(
			pParty,
			targetObjectIndex);
		pParty->SetMasterChanging(
			FALSE);
		PARTYMGR->NotifyChangesOtherMapServer(
			targetObjectIndex,
			pParty,
			MP_PARTY_NOTIFYCHANGEMASTER_TO_MAPSERVER,
			pMessage->dwID);
	}
	else //change master nack
	{
		if(!pMaster)
		{
			pParty->SetMasterChanging(FALSE);
			return;
		}

		MSGBASE msg;
		msg.Category = MP_PARTY;
		msg.Protocol = MP_PARTY_CHANGEMASTER_NACK;
		pMaster->SendMsg(&msg, sizeof(msg));
	}
}

void RPartyDelMember(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CParty* pParty = PARTYMGR->GetParty(pMessage->dwID);
	if(!pParty)
	{
		ASSERT(0);
		return;
	}
	if(atoi((char*)pData->Data[0]) == 0)
	{
		PARTYMGR->DelMember(atoi((char*)pData->Data[1]), pMessage->dwID);
	}
	else //del member nack
	{
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(atoi((char*)pData->Data[1]));

		if(!pPlayer)
			return;
		MSG_INT msg;
		msg.Category = MP_PARTY;
		msg.Protocol = MP_PARTY_DEL_NACK;
		msg.nData = atoi((char*)pData->Data[0]);
		pPlayer->SendMsg(&msg, sizeof(msg));
	}
}

void RCharacterStorageInfo(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer)
		return;

	BYTE StorageNum = 0;

	if( pMessage->dwResult > 0 )
	{
		StorageNum = BYTE( atoi((char*)pData->Data[0]) );
	}

	pPlayer->InitStorageInfo(
		StorageNum,
		atoi((char*)pData->Data[1]));
	pPlayer->SetInitState(
		PLAYERINITSTATE_STORAGE_INFO,
		MP_USERCONN_GAMEIN_SYN);
}

void RCharacterStorageItemInfo(LPQUERY pData, LPDBMESSAGE pMessage)
{
	const DWORD playerIndex = pMessage->dwID;
	CPlayer*	pPlayer		= (CPlayer*)g_pUserTable->FindUser( playerIndex );

	if( ! pPlayer ||
		! pPlayer->GetInited() )
	{
		return;
	}

	CItemSlot* const itemSlot = pPlayer->GetSlot(
		eItemTable_Storage);

	if(0 == itemSlot)
	{
		return;
	}

	for( BYTE i = 0 ; i < pMessage->dwResult ; ++i )
	{	
		ITEMBASE item;
		ZeroMemory(
			&item,
			sizeof(item));
		item.dwDBIdx = atoi((char*)pData[i].Data[1]);
		item.wIconIdx = atoi((char*)pData[i].Data[2]);
		item.Position = POSTYPE( atoi((char*)pData[i].Data[3]));
		item.QuickPosition = POSTYPE( atoi((char*)pData[i].Data[4]));
		item.Durability	= atoi((char*)pData[i].Data[5]);
		
		const ITEM_INFO* const ItemInfo = ITEMMGR->GetItemInfo( item.wIconIdx );

		if(NULL == ItemInfo)
		{
			continue;
		}

		item.nSealed = ITEM_SEAL_TYPE(atoi((char*)pData[i].Data[7]));

		if(eITEM_TYPE_GET_UNSEAL == item.nSealed)
		{
			DWORD dwRemainTime = 0;

			if( ItemInfo->nTimeKind == eKIND_REALTIME )
			{
				// 최초 DBInsert경과 시간을 구한다. 
				DWORD dwRateDBInsertTime = (DWORD) abs(  (int)atoi((char*)pData[i].Data[8]) );

				// DBInsert 경과 시간이 설정시간보다 크거나 같을 경우엔 이미 기간이 지났음으로 삭제
				if(  dwRateDBInsertTime >= ItemInfo->dwUseTime )
				{
					ItemDeleteToDB( item.dwDBIdx );

					LogItemMoney(pPlayer->GetID(), pPlayer->GetObjectName(), 0, "",
						eLog_ShopItemUseEnd, pPlayer->GetMoney(eItemTable_Inventory), 0, 0,
						item.wIconIdx , item.dwDBIdx, item.Position, 0, 
						item.Durability, pPlayer->GetPlayerExpPoint());
					continue;
				}

				// 사용기간에서 최초 DBInsert경과 시간을 빼준다.
				dwRemainTime = ItemInfo->dwUseTime - dwRateDBInsertTime;

				UnSealItemByRealTime(pPlayer->GetID(), item.dwDBIdx, dwRemainTime);
			}
			else if( ItemInfo->nTimeKind ==	eKIND_PLAYTIME )
			{
				dwRemainTime = ItemInfo->dwUseTime ;
				UnSealItemByGameTime(pPlayer->GetID(), item.dwDBIdx, dwRemainTime );
			}

			item.nSealed = eITEM_TYPE_UNSEAL ;
			item.nRemainSecond = dwRemainTime;
		}
		else if( item.nSealed == eITEM_TYPE_UNSEAL && ItemInfo->nTimeKind == eKIND_REALTIME )
		{
			item.nRemainSecond = (int)atoi((char*)pData[i].Data[8]) ;	//DB 에서 음수값이 넘어올 수도 있다.

			// 071125 KTH --- MapDBMsgParser : Item Over Time delete "아이템의 사용시간이 지났으로 Discard 해준다."
			if( item.nRemainSecond <= 0 )
			{
				ItemDeleteToDB( item.dwDBIdx );

				LogItemMoney(pPlayer->GetID(), pPlayer->GetObjectName(), 0, "",
					eLog_ShopItemUseEnd, pPlayer->GetMoney(eItemTable_Inventory), 0, 0,
					item.wIconIdx , item.dwDBIdx, item.Position, 0, 
					item.Durability, pPlayer->GetPlayerExpPoint());
				continue;
			}
		}
		else 
		{
			item.nRemainSecond = (int)atoi((char*)pData[i].Data[9]) ;
		}

		// 080820 LUJ, 획득 시 쿨타임을 갖는 아이템은 시간 계산을 위해 쿼리된 시간을 설정해줘야 한다
		if( ITEM_KIND_COOLTIME == ItemInfo->SupplyType )
		{
			item.ItemParam = gCurTime;
		}

		// 옵션 로딩
		{
			const QUERYST& record = pData[ i ];

			ITEM_OPTION option = { 0 };

			{
				ITEM_OPTION::Reinforce& data = option.mReinforce;

				data.mStrength			= atoi( ( char* )record.Data[ 10 ] );
				data.mDexterity			= atoi( ( char* )record.Data[ 11 ] );
				data.mVitality			= atoi( ( char* )record.Data[ 12 ] );
				data.mIntelligence		= atoi( ( char* )record.Data[ 13 ] );
				data.mWisdom			= atoi( ( char* )record.Data[ 14 ] );
				data.mLife				= atoi( ( char* )record.Data[ 15 ] );
				data.mMana				= atoi( ( char* )record.Data[ 16 ] );
				data.mLifeRecovery		= atoi( ( char* )record.Data[ 17 ] );
				data.mManaRecovery		= atoi( ( char* )record.Data[ 18 ] );
				data.mPhysicAttack		= atoi( ( char* )record.Data[ 19 ] );
				data.mPhysicDefence		= atoi( ( char* )record.Data[ 20 ] );
				data.mMagicAttack		= atoi( ( char* )record.Data[ 21 ] );
				data.mMagicDefence		= atoi( ( char* )record.Data[ 22 ] );
				data.mMoveSpeed			= atoi( ( char* )record.Data[ 23 ] );
				data.mEvade				= atoi( ( char* )record.Data[ 24 ] );
				data.mAccuracy			= atoi( ( char* )record.Data[ 25 ] );
				data.mCriticalRate		= atoi( ( char* )record.Data[ 26 ] );
				data.mCriticalDamage	= atoi( ( char* )record.Data[ 27 ] );
				SafeStrCpy( option.mReinforce.mMadeBy, ( char* )record.Data[ 28 ], sizeof( option.mReinforce.mMadeBy ) );
			}

			{
				ITEM_OPTION::Mix& data = option.mMix;

				data.mStrength		= atoi( ( char* )record.Data[ 29 ] );
				data.mIntelligence	= atoi( ( char* )record.Data[ 30 ] );
				data.mDexterity		= atoi( ( char* )record.Data[ 31 ] );
				data.mWisdom		= atoi( ( char* )record.Data[ 32 ] );
				data.mVitality		= atoi( ( char* )record.Data[ 33 ] );
				SafeStrCpy( option.mMix.mMadeBy, ( char* )record.Data[ 34 ], sizeof( option.mMix.mMadeBy ) );
			}

			{
				ITEM_OPTION::Enchant& data = option.mEnchant;

				data.mIndex	= atoi( ( char* )record.Data[ 35 ] );
				data.mLevel	= BYTE( atoi( ( char* )record.Data[ 36 ] ) );
				SafeStrCpy( option.mEnchant.mMadeBy, ( char* )record.Data[ 37 ], sizeof( option.mEnchant.mMadeBy ) );
			}

			{
				ITEM_OPTION::Drop& data = option.mDrop;

				data.mValue[ 0 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 38 ] ) );
				data.mValue[ 0 ].mValue	= float( atof( ( char* )record.Data[ 39 ] ) );

				data.mValue[ 1 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 40 ] ) );
				data.mValue[ 1 ].mValue	= float( atof( ( char* )record.Data[ 41 ] ) );

				data.mValue[ 2 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 42 ] ) );
				data.mValue[ 2 ].mValue	= float( atof( ( char* )record.Data[ 43 ] ) );

				data.mValue[ 3 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 44 ] ) );
				data.mValue[ 3 ].mValue	= float( atof( ( char* )record.Data[ 45 ] ) );

				data.mValue[ 4 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 46 ] ) );
				data.mValue[ 4 ].mValue	= float( atof( ( char* )record.Data[ 47 ] ) );
			}

			const ITEM_OPTION emptyOption = { 0 };

			// 081117 LUJ, 창고에 든 아이템의 제작자 이름이 표시되지 않는 문제를 수정하기 위해 검사 위치를 이동
			if( memcmp( &emptyOption, &option, sizeof( emptyOption ) ) )
			{
				option.mItemDbIndex	= item.dwDBIdx;
				ITEMMGR->AddOption( option );
			}
		}

		// 081202 LUJ, 수량성 아이템의 개수가 0인 경우, 삭제한다
		if(ItemInfo->Stack &&
			0 == item.Durability)
		{
			LogItemMoney(
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				0,
				"*invalid",
				eLog_ItemDiscard,
				pPlayer->GetMoney(),
				0,
				0,
				item.wIconIdx,
				item.dwDBIdx,
				item.Position,
				0,
				item.Durability,
				0 );
			ItemDeleteToDB( item.dwDBIdx );
			continue;
		}

        itemSlot->InsertItemAbs(
			pPlayer,
			item.Position,
			&item);
	}

	if( pMessage->dwResult >= MAX_ROW_NUM)
	{
		const QUERYST&	record		= pData[ MAX_ROW_NUM - 1 ];
		const DWORD		itemDbIndex = atoi( ( char* )record.Data[ 1 ] );

		CharacterStorageItemInfo(
			playerIndex,
			pPlayer->GetUserID(),
			itemDbIndex );
	}
	else //￠￥U A¨￠¨ui￠?8e A¡þ￠OoAI¨uⓒ￡¨¡￠c￠?￠® ¨￢￠￢ⓒø¡iAU. 
	{
		STORAGEMGR->SendStorageItemInfo(pPlayer);
	}
}

void RFriendNotifyLogouttoClient(LPQUERY pData, LPDBMESSAGE pMessage)
{
	for(DWORD i=0; i<pMessage->dwResult; ++i)
	{
		const DWORD ID = atoi((char*)pData[i].Data[0]);
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(ID);

		if(!pPlayer)
		{
			MSG_NAME_DWORD cmsg;
			cmsg.Category = MP_FRIEND;
			cmsg.Protocol = MP_FRIEND_LOGOUT_NOTIFY_TO_AGENT;
			SafeStrCpy(
				cmsg.Name,
				(char*)pData[0].Data[1],
				_countof(cmsg.Name));
			cmsg.dwData = pMessage->dwID;
			cmsg.dwObjectID = atoi((char*)pData[i].Data[0]);

			g_Network.Broadcast2AgentServer((char*)&cmsg,sizeof(cmsg));
		}
		else
		{
			MSG_NAME_DWORD msg;
			msg.Category = MP_FRIEND;
			msg.Protocol = MP_FRIEND_LOGOUT_NOTIFY_TO_CLIENT;
			SafeStrCpy(
				msg.Name,
				(char*)pData[0].Data[1],
				_countof(msg.Name));
			msg.dwData = pMessage->dwID;			
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
	}
}

void RGuildLoadGuild(LPQUERY pData, LPDBMESSAGE pMessage)
{
	for( DWORD i = 0; i<pMessage->dwResult; ++i )
	{
		const QUERYST& record = pData[ i];

		GUILDINFO info = { 0 };

		info.GuildIdx	= atoi( ( char* )record.Data[ 0 ] );
		info.MasterIdx	= atoi( ( char* )record.Data[ 1 ] );
		SafeStrCpy( info.MasterName, ( char* )record.Data[ 2 ], sizeof( info.MasterName ) );
		SafeStrCpy( info.GuildName, ( char* )record.Data[ 3 ], sizeof( info.GuildName ) );
		info.GuildLevel	= BYTE( atoi( ( char* )record.Data[ 4 ] ) );
		info.UnionIdx	= atoi( ( char* )record.Data[ 6 ] );

		const MONEYTYPE	money = atoi( ( char* )record.Data[ 7 ] );

		info.MarkName	= atoi( ( char* )record.Data[ 8 ] );

		const DWORD time = atoi( ( char* )record.Data[ 9 ] );

		GUILDMGR->RegistGuild( info, money );

		if( time )
		{
			GUILDUNIONMGR->AddEntryTime( info.GuildIdx, time );
		}
	}

	if(pMessage->dwResult >= MAX_ROW_NUM)
	{
		const DWORD lastGuildIndex = atoi( ( char* )pData[ MAX_ROW_NUM - 1 ].Data[ 0 ] );

		GuildLoadGuild( lastGuildIndex );
	}
	else
	{
		GuildLoadMember(0);

		// 길드토너먼트 지난 경기의 정보를 가져온다.
		// 길드매니저에 길드정보가 모두 로드된 이후에 수행되어야 한다.
		if(g_pServerSystem->GetMapNum() == GTMAPNUM)
			GTInfoLoad();
	}
}

void RQuestTotalInfo(LPQUERY pData, LPDBMESSAGE pMessage)
{
	SEND_QUEST_TOTALINFO msg;
	memset(&msg.QuestList, 0, sizeof(msg.QuestList));

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer ) return;

	msg.Category = MP_QUEST;
	msg.Protocol = MP_QUEST_TOTALINFO;
	msg.dwObjectID = pMessage->dwID;

	if( pMessage->dwResult == 0 )
	{
		msg.wCount = 0;
		pPlayer->SendMsg( &msg, msg.GetSize() );
	}
	else
	{
		CQuestBase* pQuest;
		QFLAG flag;
		WORD QuestIdx;
		WORD count = 0;

		for( DWORD i = 0 ; i < pMessage->dwResult ; ++i )
		{	
			QuestIdx = (WORD)atoi((char*)pData[i].Data[0]);
			flag.value = (QSTATETYPE)atoi((char*)pData[i].Data[1]);

			if( !flag.IsSet(1) )
			{
				pQuest = new CQuestBase;
				pQuest->Init(QuestIdx, flag.value);

				pPlayer->AddQuest( pQuest );

				msg.QuestList[count].QuestIdx = QuestIdx;
				msg.QuestList[count].state = flag.value;
				count++;
			}
		}

		msg.wCount = count;
		pPlayer->SendMsg( &msg, msg.GetSize() );
	}

	//KES 퀘스트 쿼리 연계
	QuestMainQuestLoad( pMessage->dwID );
} 

void RQuestMainQuestLoad(LPQUERY pData, LPDBMESSAGE pMessage)
{
	SEND_MAINQUEST_DATA msg;
	memset(&msg.QuestList, 0, sizeof(msg.QuestList));
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer ) return;

	msg.Category = MP_QUEST;
	msg.Protocol = MP_QUEST_MAINDATA_LOAD;
	msg.dwObjectID = pMessage->dwID;

	if( pMessage->dwResult == 0 )
	{
		//msg.wCount = 0;
		//pPlayer->SendMsg( &msg, msg.GetSize() );

		//---KES 메인을 다 받고 서브를 연결해 받자.
		QuestSubQuestLoad( pMessage->dwID );
	}
	else
	{
		DWORD state;
		WORD QuestIdx=0;
		DWORD EndParam=0;
		__time64_t registTime=0, curTime=0;
		BYTE bCheckType = 0;
		DWORD dwCheckTime = 0;
		WORD count = 0;

		typedef std::map< DWORD, DWORD >	QuestMap ;
		QuestMap							questIdx ;

		_time64( &curTime );

		for( DWORD i = 0 ; i < pMessage->dwResult ; ++i )
		{			
			QuestIdx = (WORD)atoi((char*)pData[i].Data[0]);
			state = (QSTATETYPE)atoi((char*)pData[i].Data[1]);
			EndParam = atoi((char*)pData[i].Data[2]);
			registTime = _atoi64((char*)pData[i].Data[3]);
			bCheckType = (BYTE)atoi((char*)pData[i].Data[4]);
			dwCheckTime = atoi((char*)pData[i].Data[5]);

			CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo( QuestIdx );
			if( !pQuestInfo )
				continue;

			// 활성화 상태의 일반퀘스트가 종료된 경우 정보를 제거한다.
			if( pQuestInfo->HasDateLimit() )
			{
				__time64_t nextEndTime = ( registTime > 0 ? pQuestInfo->GetNextEndDateTime( registTime ) : 0 );
				if( pQuestInfo->GetEndParam() == 0 &&
					EndParam &&
					curTime > nextEndTime )
				{
					QUESTMGR->DeleteQuestForcedly( *pPlayer, QuestIdx, FALSE );
					continue;
				}
			}	

			++questIdx[QuestIdx] ;

			msg.QuestList[count].QuestIdx = QuestIdx;
			msg.QuestList[count].state.value = state;
			msg.QuestList[count].EndParam = EndParam;
			msg.QuestList[count].registTime = registTime;
			msg.QuestList[count].CheckType = bCheckType;
			msg.QuestList[count].CheckTime = dwCheckTime;
			++count;

			QUESTMGR->SetMainQuestData( pPlayer, QuestIdx, state, EndParam, registTime, bCheckType, dwCheckTime );
		}

		for( QuestMap::const_iterator it = questIdx.begin(); questIdx.end() != it; ++it )
		{
			const DWORD count = it->second;

			if(count > 1)
			{
				SYSTEMTIME time;
				char szFile[256] = {0,};
				GetLocalTime( &time );

				sprintf( szFile, "./Log/MAIN_QUEST_LOAD%02d_%4d%02d%02d.txt", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay ) ;

				FILE* fp = fopen(szFile,"a+") ;

				fprintf(fp,"PlayerID : %d - QuestID : %d\n", pPlayer->GetID(), QuestIdx) ;

				fclose(fp) ;
			}
		}

		msg.wCount = count;
		pPlayer->SendMsg( &msg, msg.GetSize() );		

		if( count >= 100 )
		{
			QuestMainQuestLoad( pMessage->dwID, (int)QuestIdx );
		}
		else
		{
			//---KES 메인을 다 받고 서브를 연결해 받자.
			QuestSubQuestLoad( pMessage->dwID );
		}
	}	
}


void RQuestSubQuestLoad(LPQUERY pData, LPDBMESSAGE pMessage)
{
	SEND_SUBQUEST_DATA msg;
	memset(&msg.QuestList, 0, sizeof(msg.QuestList));

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer ) return;

	// client￠?￠® ¨￢￠￢ⓒø¨u ¡¾￠￢vA¨u ¡iy¨u¨￢..
	msg.Category = MP_QUEST;
	msg.Protocol = MP_QUEST_SUBDATA_LOAD;
	msg.dwObjectID = pMessage->dwID;
	msg.wCount = WORD(pMessage->dwResult);

	if( pMessage->dwResult == 0 )
	{
		//msg.wCount = 0;
		//pPlayer->SendMsg( &msg, msg.GetSize() );
		QuestItemload( pMessage->dwID );
	}
	else
	{
		for(DWORD i = 0 ; i < pMessage->dwResult ; ++i)
		{
			const QUERYST& record = pData[i];
			QSUBASE& questBase = msg.QuestList[i];

			questBase.QuestIdx = WORD(atoi((char*)record.Data[0]));
			questBase.SubQuestIdx = WORD(atoi((char*)record.Data[1]));
			questBase.state = atoi((char*)record.Data[2]);
			questBase.time = atoi((char*)record.Data[3]);

			QUESTMGR->SetSubQuestData(
				pPlayer,
				questBase.QuestIdx,
				questBase.SubQuestIdx,
				questBase.state,
				questBase.time);
		}

		pPlayer->SendMsg( &msg, msg.GetSize() );

		if(MAX_ROW_NUM <= pMessage->dwResult)
		{
			const QSUBASE& questBase = msg.QuestList[pMessage->dwResult - 1];

			QuestSubQuestLoad(
				pMessage->dwID,
				questBase.QuestIdx);
		}
		else
		{
			QuestItemload( pMessage->dwID );
		}
	}	
}


void RQuestItemLoad(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer ) return;

	SEND_QUESTITEM msg;
	memset(&msg.ItemList, 0, sizeof(msg.ItemList));
	// client징횏?징횏짖챌 징짠징첸징횏징첸짢횕짤짧징짠u 짖짰짢첬징횏징첸vA징짠u 짖짰iy징짠u징짠징첸..
	msg.Category = MP_QUEST;
	msg.Protocol = MP_QUEST_ITEM_LOAD;
	msg.dwObjectID = pMessage->dwID;
	msg.wCount = WORD(pMessage->dwResult);

	if( pMessage->dwResult == 0 )
	{
		//msg.wCount = 0;
		//pPlayer->SendMsg( &msg, msg.GetSize() );
		TutorialLoad( pMessage->dwID );
	}
	else
	{
		for(DWORD i = 0 ; i < pMessage->dwResult ; ++i)
		{
			const QUERYST& record = pData[i];
			QITEMBASE& itemBase = msg.ItemList[i];

			itemBase.ItemIdx = atoi((char*)record.Data[0]);
			itemBase.Count = WORD(atoi((char*)record.Data[1]));
			itemBase.QuestIdx = WORD(atoi((char*)record.Data[2]));
			
			QUESTMGR->SetQuestItem(
				pPlayer,
				itemBase.QuestIdx,
				itemBase.ItemIdx,
				itemBase.Count);
		}

		pPlayer->SendMsg( &msg, msg.GetSize() );

		if(MAX_ROW_NUM <= pMessage->dwResult)
		{
			const QITEMBASE& itemBase = msg.ItemList[pMessage->dwResult - 1];

			QuestItemload(
				pMessage->dwID,
				itemBase.QuestIdx);
		}
		else
		{
			TutorialLoad( pMessage->dwID );
		}
	}
}

void RSaveMapChangePointReturn(LPQUERY pData, LPDBMESSAGE pMessage)
{
	DWORD dwConnectionIndex = (DWORD)atoi((char*)pData->Data[0]);
	DWORD Map = (DWORD)atoi((char*)pData->Data[1]);

	MSG_DWORD msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_MAP_OUT;
	msg.dwObjectID = pMessage->dwID;
	msg.dwData = Map;

	g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(msg));
}

void RCharacterShopItemInfo(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer ) return;

	if(pPlayer->GetInited() == FALSE)
		return;
	if(pMessage->dwResult == 0)	
		return;

	SEND_SHOPITEM_INFO ShopItem;
	ZeroMemory(
		&ShopItem,
		sizeof(ShopItem));
	ShopItem.Category = MP_ITEM;
	ShopItem.Protocol = MP_ITEM_SHOPITEM_INFO_ACK;
	ShopItem.ItemCount = WORD(pMessage->dwResult);

	for(DWORD i = 0; i < pMessage->dwResult; ++i)
	{
		if(i >= _countof(ShopItem.Item))
		{
			break;
		}

		const QUERYST& record = pData[i];

		ITEMBASE& itemBase = ShopItem.Item[i];
		itemBase.Position = POSTYPE(TP_SHOPITEM_START + i);
		itemBase.dwDBIdx = atoi((char*)record.Data[0]);
		itemBase.wIconIdx = atoi((char*)record.Data[1]);
		itemBase.Durability = atoi((char*)record.Data[2]);
		itemBase.ItemParam = atoi((char*)record.Data[3]);
		itemBase.nSealed = ITEM_SEAL_TYPE(atoi((char*)record.Data[4]));

		const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo(
			itemBase.wIconIdx);

		if(0 == itemInfo)
		{
			continue;
		}
		else if(eITEM_TYPE_GET_UNSEAL == itemBase.nSealed)
		{
			DWORD dwRemainTime = 0;

			if(eKIND_REALTIME == itemInfo->nTimeKind)
			{
				const DWORD dwRateDBInsertTime = atoi((char*)record.Data[5]);

				// DBInsert 경과 시간이 사용기간보다 크거나 같을 경우엔 이미 기간이 지났음으로 삭제
				if(dwRateDBInsertTime >= itemInfo->dwUseTime)
				{
					ItemDeleteToDB(
						itemBase.dwDBIdx);
					LogItemMoney(
						pPlayer->GetID(),
						pPlayer->GetObjectName(),
						0,
						"",
						eLog_ShopItemUseEnd,
						pPlayer->GetMoney(eItemTable_Inventory),
						0,
						0,
						itemBase.wIconIdx,
						itemBase.dwDBIdx,
						itemBase.Position,
						0,
						itemBase.Durability,
						pPlayer->GetPlayerExpPoint());
					ZeroMemory(
						&itemBase,
						sizeof(itemBase));
					continue;
				}

				// 사용기간에서 최초 DBInsert경과 시간을 빼준다.
				dwRemainTime = itemInfo->dwUseTime - dwRateDBInsertTime;

				UnSealItemByRealTime(
					pPlayer->GetID(),
					itemBase.dwDBIdx,
					dwRemainTime);
			}
			else if(eKIND_PLAYTIME == itemInfo->nTimeKind)
			{
				dwRemainTime = itemInfo->dwUseTime;

				UnSealItemByGameTime(
					pPlayer->GetID(),
					itemBase.dwDBIdx,
					dwRemainTime);
			}

			itemBase.nSealed = eITEM_TYPE_UNSEAL;
			itemBase.nRemainSecond = dwRemainTime;
		}
		else if(eKIND_REALTIME == itemInfo->nTimeKind)
		{
			itemBase.nRemainSecond = atoi((char*)record.Data[5]);
		}
		else
		{
			itemBase.nRemainSecond = atoi((char*)record.Data[6]);
		}

		if(itemInfo->Stack &&
			0 == itemBase.Durability)
		{
			LogItemMoney(
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				0,
				"*invalid",
				eLog_ItemDiscard,
				pPlayer->GetMoney(),
				0,
				0,
				itemBase.wIconIdx,
				itemBase.dwDBIdx,
				itemBase.Position,
				0,
				itemBase.Durability,
				0);
			ItemDeleteToDB(
				itemBase.dwDBIdx);
			ZeroMemory(
				&itemBase,
				sizeof(itemBase));
		}
	}

	pPlayer->InitShopItemInfo(
		ShopItem);
	pPlayer->SendMsg(
		&ShopItem,
		ShopItem.GetSize());
}

void RGuildLoadMark(LPLARGEQUERY pData, LPDBMESSAGE pMessage)
{
	for(DWORD i=0; i<pMessage->dwResult; ++i)
	{
		MARKNAMETYPE	MarkName	= atoi( ( char* )pData->Data[ 0 ] );
		char*			pImgData	=		( char* )pData->Data[ 1 ];

		GUILDMGR->LoadMark( MarkName, pImgData );
	}
	if(pMessage->dwResult)
	{
		GuildLoadMark(atoi((char*)pData->Data[0]));
	}
	else
	{
		LoadGuildWarRecord( 0 );
	}
}


void RGuildMarkRegist(LPLARGEQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if(pPlayer == 0) return;
	//markidx, guildidx, imgdate

	// 091111 ONS 길드문장 등록시 길드원 전체에 메세지를 보내도록 수정.
	GUILDMGR->RegistMarkResult(atoi((char*)pData->Data[1]), atoi((char*)pData->Data[0]), (char*)pData->Data[2]);
}

void ConfirmUserOut( DWORD dwConnectIdx, DWORD dwUserIdx, DWORD dwCharIdx, DWORD dwKind )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC RP_CONFIRMUSEROUT %d, %d, %d", dwUserIdx, dwCharIdx, dwKind );
	g_DB.Query(eQueryType_FreeQuery, eConfirmUserOut, dwConnectIdx, txt, dwCharIdx);
}

void RConfirmUserOut( LPQUERY pData, LPDBMESSAGE pMessage )
{
	DWORD dwConnectIdx = pMessage->dwID;
	DWORD dwUserIdx = (DWORD)atoi((char*)pData[0].Data[0]);
	DWORD dwCharIdx = (DWORD)atoi((char*)pData[0].Data[1]);
	DWORD dwKind = (DWORD)atoi((char*)pData[0].Data[2]);

	switch( dwKind )
	{
	case 1:		// 캐릭선택창
		{
			MSGBASE msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_BACKTOCHARSEL_ACK;
			msg.dwObjectID = dwCharIdx;

			g_Network.Send2Server(dwConnectIdx, (char*)&msg, sizeof(msg));
		}
		break;
	case 2:
		{
			MSGBASE msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_LOGINCHECK_DELETE;
			msg.dwObjectID = dwUserIdx;

			g_Network.Send2Server(dwConnectIdx, (char*)&msg, sizeof(msg));
		}
		break;
	}	
}

void RGuildBreakUp( LPQUERY pData, LPDBMESSAGE pMessage )
{
	CPlayer* pMaster = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	GUILDMGR->BreakUpGuildResult(pMaster, atoi((char*)pData->Data[0]), (char*)pData->Data[1]);

	DeleteGuildWarRecord( (DWORD)atoi((char*)pData->Data[0]) );
}


void RGuildDeleteMember( LPQUERY pData, LPDBMESSAGE pMessage )
{
	if(atoi((char*)pData->Data[0]) == 0) //err
	{
		CPlayer * pMaster = (CPlayer*)g_pUserTable->FindUser( atoi((char*)pData->Data[1]) );
		if(pMaster)
			GUILDMGR->SendNackMsg(pMaster, MP_GUILD_DELETEMEMBER_NACK, eGuildErr_Err);
	}
	else
		GUILDMGR->DeleteMemberResult(pMessage->dwID, atoi((char*)pData->Data[0]), (char*)pData->Data[1]);
}

void RGuildSecedeMember( LPQUERY pData, LPDBMESSAGE pMessage )
{
	if(atoi((char*)pData->Data[0]) == 0) //err
	{
		CPlayer * pPlayer = (CPlayer*)g_pUserTable->FindUser(atoi((char*)pData->Data[1]));
		if(pPlayer)
			GUILDMGR->SendNackMsg(pPlayer, MP_GUILD_SECEDE_NACK, eGuildErr_Err);
	}
	else
		GUILDMGR->SecedeResult(pMessage->dwID, atoi((char*)pData->Data[0]), (char*)pData->Data[1]);
}


void LoadGuildWarRecord( DWORD dwIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC dbo.MP_GUILDWARRECORD_LOAD %d", dwIdx );
	g_DB.Query(eQueryType_FreeQuery, eLoadGuildWarRecord, 0, txt);
}

void RLoadGuildWarRecord( LPQUERY pData, LPDBMESSAGE pMessage )
{
	int nStartIdx = 0;
	for( DWORD i = 0; i < pMessage->dwResult; ++i )
	{
		nStartIdx = atoi((char*)pData[i].Data[0]);
		DWORD dwGuildIdx = (DWORD)atoi((char*)pData[i].Data[1]);
		DWORD dwGFWVictory = (DWORD)atoi((char*)pData[i].Data[2]);
		DWORD dwGFWDraw = (DWORD)atoi((char*)pData[i].Data[3]);
		DWORD dwGFWLoose = (DWORD)atoi((char*)pData[i].Data[4]);

		GUILDWARMGR->RegistGuildWarFromRecord( dwGuildIdx, dwGFWVictory, dwGFWDraw, dwGFWLoose );
	}

	if( pMessage->dwResult >= MAX_ROW_NUM )
	{
		LoadGuildWarRecord( nStartIdx );
	}	
	else
	{
		LoadGuildWar( 0 );
	}
}

void DeleteGuildWarRecord( DWORD dwGuildIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC dbo.MP_GUILDWARRECORD_DELETE %d", dwGuildIdx );
	g_DB.Query(eQueryType_FreeQuery, eDeleteGuildWarRecord, 0, txt);
}

void LoadGuildWar( DWORD dwIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC dbo.MP_GUILDFIELDWAR_LOAD %d", dwIdx );
	g_DB.Query(eQueryType_FreeQuery, eLoadGuildWar, 0, txt);
}

void RLoadGuildWar( LPQUERY pData, LPDBMESSAGE pMessage )
{
	int nStartIdx = 0;
	for( DWORD i = 0; i < pMessage->dwResult; ++i )
	{
		nStartIdx = atoi((char*)pData[i].Data[0]);
		DWORD dwGuildIdx1 = (DWORD)atoi((char*)pData[i].Data[1]);
		DWORD dwGuildIdx2 = (DWORD)atoi((char*)pData[i].Data[2]);
		DWORD dwMoney = (DWORD)atoi((char*)pData[i].Data[3]);

		GUILDWARMGR->RegistGuildWar( dwGuildIdx1, dwGuildIdx2, dwMoney );
	}

	if( pMessage->dwResult >= MAX_ROW_NUM )
	{
		LoadGuildWar( nStartIdx );
	}	
	else
	{
		GuildUnionLoad( 0 );
	}
}

void InsertGuildWar( DWORD dwGuildIdx1, DWORD dwGuildIdx2, DWORD dwMoney )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC dbo.MP_GUILDFIELDWAR_INSERT %d, %d, %u", dwGuildIdx1, dwGuildIdx2, dwMoney );
	g_DB.Query(eQueryType_FreeQuery, eInsertGuildWar, 0, txt);
}

void DeleteGuildWar( DWORD dwGuildIdx1, DWORD dwGuildIdx2 )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC dbo.MP_GUILDFIELDWAR_DELETE %d, %d", dwGuildIdx1, dwGuildIdx2 );
	g_DB.Query(eQueryType_FreeQuery, eDeleteGuildWar, 0, txt);
}

void UpdateGuildWarRecordToDB( DWORD dwGuildIdx, DWORD dwVitory, DWORD dwDraw, DWORD dwLoose )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC dbo.MP_GUILDFIELDWAR_RECORD %d, %d, %d, %d", dwGuildIdx, dwVitory, dwDraw, dwLoose );
	g_DB.Query(eQueryType_FreeQuery, eUpdateGuildWarRecord, 0, txt);
}

// 081006 LUJ, 연합 정보를 읽는다
void GuildUnionLoad( DWORD dwStartGuildUnionIdx )
{
	sprintf( txt, "EXEC dbo.MP_GUILD_UNION_LOAD %d", dwStartGuildUnionIdx );
	g_DB.Query( eQueryType_FreeQuery, eGuildUnionLoad, 0, txt );
}

void RGuildUnionLoad( LPQUERY pData, LPDBMESSAGE pMessage )
{
	DWORD dwStartGuildIdx = 0;
	for( DWORD i = 0; i < pMessage->dwResult; ++i )
	{
		dwStartGuildIdx = (DWORD)atoi((char*)pData[i].Data[0]);
		GUILDUNIONMGR->LoadGuildUnion( (DWORD)atoi((char*)pData[i].Data[0])		// guildidx
			, (char*)pData[i].Data[1]					// name
			, (DWORD)atoi((char*)pData[i].Data[2])		// 0
				, (DWORD)atoi((char*)pData[i].Data[3])		// 1
				, (DWORD)atoi((char*)pData[i].Data[4])		// 2
				, (DWORD)atoi((char*)pData[i].Data[5])		// 3
				, (DWORD)atoi((char*)pData[i].Data[6])		// 4
				, (DWORD)atoi((char*)pData[i].Data[7])		// 5
				, (DWORD)atoi((char*)pData[i].Data[8])		// 6
				, (DWORD)atoi((char*)pData[i].Data[9]) );	// mark
	}
	if( pMessage->dwResult >= MAX_ROW_NUM )
	{
		GuildUnionLoad( dwStartGuildIdx );
	}
	else
	{
		GuildUnionLoadMark( 0 );
	}
}

void GuildUnionLoadMark( DWORD dwMarkIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_GUILD_UNION_LOADMARK %d", dwMarkIdx );
	g_DB.FreeLargeQuery( RGuildUnionLoadMark, 0, txt );
}

void RGuildUnionLoadMark( LPLARGEQUERY pData, LPDBMESSAGE pMessage )
{
	for( DWORD i = 0; i < pMessage->dwResult; ++i )
	{
		GUILDUNIONMGR->LoadGuildUnionMark( (DWORD)atoi((char*)pData->Data[0]), (DWORD)atoi((char*)pData->Data[1]), (char*)pData->Data[2] );
	}
	if( pMessage->dwResult )
		GuildUnionLoadMark( (DWORD)atoi((char*)pData->Data[0]) );
}

void GuildUnionCreate( DWORD dwCharacterIdx, DWORD dwGuildIdx, char* sUnionName )
{
	g_DB.FreeQuery(
		eGuildUnionCreate,
		dwCharacterIdx,
		"EXEC dbo.MP_GUILD_UNION_CREATE %d, \'%s\'",
		dwGuildIdx,
		sUnionName);
}

void RGuildUnionCreate( LPQUERY pData, LPDBMESSAGE pMessage )
{
	if( !pMessage->dwResult )	return;
	if( atoi((char*)pData->Data[0]) == 0 )
	{
		CPlayer* pPlayer = ( CPlayer* )g_pUserTable->FindUser( pMessage->dwID );
		if( pPlayer )
			GUILDUNIONMGR->SendNackMsg( pPlayer, MP_GUILD_UNION_CREATE_NACK, eGU_Not_ValidName );
		return;
	}
	else
		GUILDUNIONMGR->GuildUnionCreateResult( (DWORD)atoi((char*)pData->Data[0]), (char*)pData->Data[1], (DWORD)atoi((char*)pData->Data[2]) );
}

void GuildUnionDestroy( DWORD dwGuildUnionIdx, DWORD dwMarkIdx, DWORD dwMasterIdx, DWORD dwTime )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_GUILD_UNION_DESTROY %d, %d, %d, %d", dwGuildUnionIdx, dwMarkIdx, dwMasterIdx, dwTime );
	g_DB.Query( eQueryType_FreeQuery, eGuildUnionDestroy, 0, txt, dwMasterIdx );
}

void GuildUnionAddGuild( DWORD dwGuildUnionIdx, int nIndex, DWORD dwGuildIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_GUILD_UNION_ADDGUILD %d, %d, %d", dwGuildUnionIdx, nIndex, dwGuildIdx );
	g_DB.Query( eQueryType_FreeQuery, eGuildUnionAddGuild, 0, txt );
}

void GuildUnionRemoveGuild( DWORD dwGuildUnionIdx, int nIndex, DWORD dwGuildIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_GUILD_UNION_REMOVEGUILD %d, %d, %d", dwGuildUnionIdx, nIndex, dwGuildIdx );
	g_DB.Query( eQueryType_FreeQuery, eGuildUnionRemoveGuild, 0, txt );
}

void GuildUnionSecedeGuild( DWORD dwGuildUnionIdx, int nIndex, DWORD dwGuildIdx, DWORD dwTime )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_GUILD_UNION_SECEDEGUILD %d, %d, %d, %d", dwGuildUnionIdx, nIndex, dwGuildIdx, dwTime );
	g_DB.Query( eQueryType_FreeQuery, eGuildUnionSecedeGuild, 0, txt );
}

void GuildUnionMarkRegist( DWORD dwCharacterIdx, BOOL isUpdate, DWORD dwGuildUnionIdx, char* pImgData )
{
	char buf[2048] = {0};
	sprintf(
		buf,
		"EXEC dbo.MP_GUILD_UNION_MARKREGIST %d, %d, %d, 0x",
		dwCharacterIdx,
		isUpdate,
		dwGuildUnionIdx);

	int curpos = strlen(buf);
	for( int n = 0; n < GUILDUNIONMARK_BUFSIZE; ++n )
	{
		sprintf( &buf[curpos], "%02x", (BYTE)pImgData[n] );
		curpos += 2;
	}

	g_DB.FreeLargeQuery( RGuildUnionMarkRegist, 0, buf, dwCharacterIdx );
}

void RGuildUnionMarkRegist( LPLARGEQUERY pData, LPDBMESSAGE pMessage )
{
	if( !pMessage->dwResult )	return;
	GUILDUNIONMGR->GuildUnionMarkRegistResult( (DWORD)atoi((char*)pData->Data[0]), (DWORD)atoi((char*)pData->Data[1]),
		(DWORD)atoi((char*)pData->Data[2]), (char*)pData->Data[3] );
}

void ChangeCharacterAttr( DWORD dwPlayerIdx, DWORD dwAttr )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_CHARACTER_CHANGEATTR_JP %d, %d", dwPlayerIdx, dwAttr );
	g_DB.Query( eQueryType_FreeQuery, eChangeCharacterAttr, 0, txt, dwPlayerIdx );
}

void QuestEventCheck( DWORD dwUserIdx, DWORD dwCharacterIdx, DWORD dwNpcIdx, DWORD dwQuestIdx, char* pTime )
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(pTime, "'"))	return;
	
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_QUEST_EVENTCHECK %d, %d, %d, %d, \'%s\'", dwUserIdx, dwCharacterIdx, dwNpcIdx, dwQuestIdx, pTime );
	g_DB.Query( eQueryType_FreeQuery, eQuestEventCheck, dwCharacterIdx, txt, dwCharacterIdx );
}

void RQuestEventCheck( LPQUERY pData, LPDBMESSAGE pMessage )
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pMessage->dwID );
	if( !pPlayer )	return;

	MSG_DWORD2 Msg;
	Msg.Category = MP_QUEST; 

	DWORD dwNpcIdx = (DWORD)atoi((char*)pData[0].Data[0]);
	DWORD dwQuestIdx = (DWORD)atoi((char*)pData[0].Data[1]);
	DWORD dwRet = (DWORD)atoi((char*)pData[0].Data[2]);
	switch( dwRet )
	{
	case 0:		// 할수있다
		{
			Msg.Protocol = MP_QUESTEVENT_NPCTALK_ACK;

			// quest event
			CQuestEvent qe;
			qe.SetValue( eQuestEvent_NpcTalk, dwNpcIdx, dwQuestIdx );
			QUESTMGR->AddQuestEvent( pPlayer, &qe );
		}
		break;
	case 1:		// 이미 했다
		{
			Msg.Protocol = MP_QUESTEVENT_NPCTALK_NACK;
		}
		break;
	case 2:		// 할 수 없다
		{
			Msg.Protocol = MP_QUESTEVENT_NPCTALK_NACK;
		}
		break;
	default:
		return;
	}

	Msg.dwData1 = dwQuestIdx;
	Msg.dwData2 = dwRet;
	pPlayer->SendMsg( &Msg, sizeof(Msg) );
}

void QuickInfo( DWORD characterIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_QUICK_INFO %d", characterIdx );
	g_DB.Query( eQueryType_FreeQuery, eQuickInfo, characterIdx, txt, characterIdx );
}

void RQuickInfo( LPQUERY pData, LPDBMESSAGE pMessage )
{
	if(pMessage->dwResult > 8)
	{
		/// 단축창 탭정보가 8개보다 많다
	}
	else if((pMessage->dwResult == 1) && ( atoi( ( char* )pData->Data[ 0 ] ) == 0 ) )
	{
		/// 단축창 정보가 없다.
		/// 기본 셋팅
		CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(atoi((char*)pData->Data[1]));
		if(pPlayer == NULL)
			return;

		for( BYTE sheet = 0; sheet < 8; ++sheet )
		{
			for( WORD j = 0; j < 10; ++j )
			{
				SLOT_INFO info;

				info.kind = 0;
				info.dbIdx = 0;
				info.idx = 0;
				info.data = 0;

				pPlayer->AddQuick( (BYTE)sheet, j, &info );
			}
		}

		pPlayer->SetInitState(PLAYERINITSTATE_QUICK_INFO, pMessage->dwID);

	}
	else
	{
		const DWORD characterIdx = atoi( ( char* )pData[ 0 ].Data[ 0 ] );

		CPlayer* pPlayer = ( CPlayer * )g_pUserTable->FindUser( characterIdx );
		if( pPlayer == NULL )
			return;

		for( DWORD i = 0; i < pMessage->dwResult; i++ )
		{
			const BYTE sheet = ( BYTE )atoi( ( char* )pData[ i ].Data[ 1 ] );

			for( WORD j = 0; j < 10; j++ )
			{
				SLOT_INFO info;

				info.kind = ( BYTE )atoi( ( char* )pData[ i ].Data[ 2 + ( j * 4 ) ] );
				info.dbIdx = atoi( ( char* )pData[ i ].Data[ 3 + ( j * 4 ) ] );
				info.idx = atoi( ( char* )pData[ i ].Data[ 4 + ( j * 4 ) ] );
				info.data = ( WORD )atoi( ( char* )pData[ i ].Data[ 5 + ( j * 4 ) ] );

				pPlayer->AddQuick( sheet, j, &info );
			}
		}

		pPlayer->SetInitState(PLAYERINITSTATE_QUICK_INFO, pMessage->dwID);
	}
}

void QuickUpdate( DWORD characterIdx, BYTE sheet, SLOT_INFO* pSlot )
{
	char buf[ 10 ][ 40 ];
	for( int i = 0; i < 10; i++ )
	{
		sprintf( buf[i], "%d, %u, %d, %d", pSlot[i].kind, pSlot[i].dbIdx, pSlot[i].idx, pSlot[i].data );
	}

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_QUICK_UPDATE %d, %d, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",
		characterIdx,
		sheet,
		buf[ 0 ],
		buf[ 1 ],
		buf[ 2 ],
		buf[ 3 ],
		buf[ 4 ],
		buf[ 5 ],
		buf[ 6 ],
		buf[ 7 ],	
		buf[ 8 ],
		buf[ 9 ] );

	g_DB.Query( eQueryType_FreeQuery, eQuickUpdate, characterIdx, txt, characterIdx );
}

// 081017 LUJ, 농작물 정보를 일괄로 쿼리하도록 수정
void RFarm_LoadFarmState( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const WORD zoneIndex = WORD( dbMessage->dwID );

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST& record = query[ i ];

		CSHFarmManager::PACKET_FARM_STATE_WITH_TAX packet;
		ZeroMemory( &packet, sizeof( packet ) );
		packet.nFarmZone			= WORD( zoneIndex );
		packet.nFarmID				= WORD( atoi( ( char* )record.Data[ 0 ] ) );
		packet.nFarmState			= WORD( atoi( ( char* )record.Data[ 1 ] ) );
		packet.nFarmOwner			= DWORD( atoi( ( char* )record.Data[ 2 ] ) );
		packet.nGardenGrade			= WORD( atoi( ( char* )record.Data[ 3 ] ) );
		packet.nHouseGrade			= WORD( atoi( ( char* )record.Data[ 4 ] ) );
		packet.nWarehouseGrade		= WORD( atoi( ( char* )record.Data[ 5 ] ) );
		packet.nAnimalCageGrade		= WORD( atoi( ( char* )record.Data[ 6 ] ) );
		packet.nFenceGrade			= WORD( atoi( ( char* )record.Data[ 7 ] ) );
		packet.nTaxArrearageFreq	= WORD( atoi( ( char* )record.Data[ 8 ] ) );
		SafeStrCpy( packet.szTaxPayPlayerName,  (char* )record.Data[ 9 ], sizeof( packet.szTaxPayPlayerName ) );

		g_csFarmManager.SRV_SetFarmStateFromDBQueue( 0, &packet );
	}
}

// 081017 LUJ, 농작물 정보를 일괄로 쿼리하도록 수정
void RFarm_LoadCropInfo( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const WORD zoneIndex = WORD( dbMessage->dwID );

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST& record = query[ i ];

		CSHFarmManager::PACKET_FARM_GARDEN_CROP_PLANT packet;
		ZeroMemory( &packet, sizeof( packet ) );
		packet.nFarmZone			= WORD( zoneIndex );
		packet.nFarmID				= WORD( atoi( ( char* )record.Data[ 0 ] ) );
		packet.nCropID				= WORD( atoi( ( char* )record.Data[ 1 ] ) );
		packet.nCropOwner			= DWORD( atoi( ( char* )record.Data[ 2 ] ) );
		packet.nCropKind			= WORD( atoi( ( char* )record.Data[ 3 ] ) );
		packet.nCropStep			= WORD( atoi( ( char* )record.Data[ 4 ] ) );
		packet.nCropLife			= WORD( atoi( ( char* )record.Data[ 5 ] ) );
		packet.nCropNextStepTime	= WORD( atoi( ( char* )record.Data[ 6 ] ) );
		packet.nCropSeedGrade		= WORD( atoi( ( char* )record.Data[ 7 ] ) );

		g_csFarmManager.SRV_SetCropInfoFromDBQueue( 0, &packet );
	}

	// 081017 LUJ, 최대 개수일 때 추가로 쿼리 요청한다
	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ dbMessage->dwResult - 1 ];

		const DWORD lastFarmIndex	= atoi( ( char* )record.Data[ 0 ] );
		const DWORD lastCropIndex	= atoi( ( char* )record.Data[ 1 ] );

		g_DB.FreeQuery(
			eFarm_LoadCropInfo,
			zoneIndex,
			"EXEC dbo.MP_FARM_CROP_SELECT %d, %d, %d",
			zoneIndex,
			lastFarmIndex,
			lastCropIndex );
	}
}

// 080416 KTH -- 가축 정보
// 081017 LUJ, 농작물 정보를 일괄로 쿼리하도록 수정
void RFarm_LoadAnimalInfo( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const WORD zoneIndex = WORD( dbMessage->dwID );

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST& record = query[ i ];

		CSHFarmManager::PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT packet;
		ZeroMemory( &packet, sizeof( packet ) );
		packet.nFarmZone			= zoneIndex;
		packet.nFarmID				= WORD( atoi( ( char* )record.Data[ 0 ] ) );
		packet.nAnimalID			= WORD( atoi( ( char* )record.Data[ 1 ] ) );
		packet.nAnimalOwner			= DWORD( atoi( ( char* )record.Data[ 2 ] ) );
		packet.nAnimalKind			= WORD( atoi( ( char* )record.Data[ 3 ] ) );
		packet.nAnimalStep			= WORD( atoi( ( char* )record.Data[ 4 ] ) );
		packet.nAnimalLife			= WORD( atoi( ( char* )record.Data[ 5 ] ) );
		packet.nAnimalNextStepTime	= WORD( atoi( ( char* )record.Data[ 6 ] ) );
		packet.nAnimalContentment	= WORD( atoi( ( char* )record.Data[ 7 ] ) );
		packet.nAnimalInterest		= WORD( atoi( ( char* )record.Data[ 8 ] ) );
		
		g_csFarmManager.SRV_SetAnimalInfoFromDBQueue( 0, &packet );
	}

	// 081017 LUJ, 최대 개수일 때 추가로 쿼리 요청한다
	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ dbMessage->dwResult - 1 ];

		const DWORD lastFarmIndex	= atoi( ( char* )record.Data[ 0 ] );
		const DWORD lastAnimalIndex	= atoi( ( char* )record.Data[ 1 ] );

		g_DB.FreeQuery(
			eFarm_LoadAnimalInfo,
			zoneIndex,
			"EXEC dbo.MP_FARM_ANIMAL_SELECT %d, %d, %d",
			zoneIndex,
			lastFarmIndex,
			lastAnimalIndex );
	}
}

void Farm_LoadTimeDelay(DWORD nPlayerID)
{
	sprintf(txt, "EXEC  %s %d", STORED_FARM_LOAD_DELAYTIME, nPlayerID);
	g_DB.Query(eQueryType_FreeQuery, eFarm_LoadTimeDelay, nPlayerID, txt, nPlayerID);
}

void Farm_SetTimeDelay(DWORD nPlayerID, DWORD nKind, DWORD nRemainSecond)
{
	sprintf(txt, "EXEC  %s %d, %d, %d", STORED_FARM_SET_DELAYTIME, nPlayerID, nKind, nRemainSecond);
	g_DB.Query(eQueryType_FreeQuery, eFarm_SetTimeDelay, 0, txt, nPlayerID);
}

void RFarm_LoadTimeDelay(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer ) return;

	for(DWORD i=0; i<pMessage->dwResult; i++)
	{
		int nKind = atoi((char*)pData[i].Data[0]);
		if (nKind == CSHFarmManager::FARM_TIMEDELAY_KIND_PLANT)
		{
			// 수치 데이터가 변경되었을 경우에 이미 기록이 되어 있는 경우에는 변경된 수치가 적용되지 않고 UI에도 남은 시간이 표시되지 않는다.
			// 그래서 수치 데이터와 DB값을 체크. 남은 시간을 기록해서 그럼..
			DWORD nTime = atoi((char*)pData[i].Data[0]);
			if (nTime > CSHCrop::CROP_PLANT_RETRY_TIME)
				nTime = CSHCrop::CROP_PLANT_RETRY_TIME;
			pPlayer->SetFarmCropPlantRetryTime(atoi((char*)pData[i].Data[1]));
		}
		else if (nKind == CSHFarmManager::FARM_TIMEDELAY_KIND_MANURE)
		{
			// 수치 데이터가 변경되었을 경우에 이미 기록이 되어 있는 경우에는 변경된 수치가 적용되지 않고 UI에도 남은 시간이 표시되지 않는다.
			// 그래서 수치 데이터와 DB값을 체크. 남은 시간을 기록해서 그럼..
			DWORD nTime = atoi((char*)pData[i].Data[1]);
			if (nTime > CSHCrop::CROP_MANURE_RETRY_TIME)
				nTime = CSHCrop::CROP_MANURE_RETRY_TIME;
			pPlayer->SetFarmCropManureRetryTime(atoi((char*)pData[i].Data[1]));
		}
		// 080508 KTH -- 가축 사료주기 딜레이 추가
		else if (nKind == CSHFarmManager::FARM_TIMEDELAY_KIND_FEED)
		{
			DWORD nTime = atoi((char*)pData[i].Data[1]);
			if(nTime > CAnimal::ANIMAL_FEED_DELAY_TIME)
				nTime = CAnimal::ANIMAL_FEED_DELAY_TIME;
			pPlayer->SetFarmAnimalFeedRetryTime(atoi((char*)pData[i].Data[1]));
		}
		else if (nKind == CSHFarmManager::FARM_TIMEDELAY_KIND_CLEAN )
		{
			DWORD nTime = atoi((char*)pData[i].Data[1]);
			if( nTime > CAnimal::ANIMAL_CLEANING_RETRY_TIME)
				nTime = CAnimal::ANIMAL_CLEANING_RETRY_TIME;
			pPlayer->SetFarmAnimalCleanRetryTime(atoi((char*)pData[i].Data[1]));
		}
	}

	//---
	MonsterMeter_Load( pMessage->dwID );

}

void Farm_SetTaxInfo(DWORD nFarmZone, DWORD nFarmID, DWORD nTaxArrearageFreq, DWORD nTaxPayPlayerID)
{
	sprintf(txt, "EXEC  %s %d, %d, %d, %d", STORED_FARM_SET_TAXINFO, nFarmZone, nFarmID, nTaxArrearageFreq, nTaxPayPlayerID);
	g_DB.Query(eQueryType_FreeQuery, eFarm_SetTaxInfo, 0, txt);
}

// 091204 ONS 패밀리 다이얼로그에 농장세금납부 기능 추가: 해당 농장정보를 DB에서 가져온다.
void Farm_GetFarmInfo(DWORD dwPlyaerID, DWORD dwFarmZone, DWORD dwFarmID)
{
	sprintf(txt, "EXEC dbo.MP_FARM_LOADFARMSTATE %d, %d", dwFarmZone, dwFarmID);
	g_DB.Query(eQueryType_FreeQuery, eFarm_GetFarmInfo, dwPlyaerID, txt, dwPlyaerID);
}

// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가.
void RFarm_GetFarmInfo(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer ) return;

	for( DWORD i = 0; i < pMessage->dwResult; ++i )
	{
		const QUERYST& record = pData[ i ];

		CSHFarmManager::stFARMTAXSTATE stFarmState;
		ZeroMemory(&stFarmState, sizeof(stFarmState));

		stFarmState.nFarmZone				= WORD( atoi( ( char* )record.Data[ 0 ] ) );
		stFarmState.nFarmID					= WORD( atoi( ( char* )record.Data[ 1 ] ) );
		stFarmState.nFarmState				= WORD( atoi( ( char* )record.Data[ 2 ] ) );
		stFarmState.nFarmOwner				= DWORD( atoi( ( char* )record.Data[ 3 ] ) );
		stFarmState.nGardenGrade			= WORD( atoi( ( char* )record.Data[ 4 ] ) );
		stFarmState.nHouseGrade				= WORD( atoi( ( char* )record.Data[ 5 ] ) );
		stFarmState.nWarehouseGrade			= WORD( atoi( ( char* )record.Data[ 6 ] ) );
		stFarmState.nAnimalCageGrade		= WORD( atoi( ( char* )record.Data[ 7 ] ) );
		stFarmState.nFenceGrade				= WORD( atoi( ( char* )record.Data[ 8 ] ) );
		stFarmState.nTaxArrearageFreq		= WORD( atoi( ( char* )record.Data[ 9 ] ) );
		SafeStrCpy( stFarmState.szTaxPayPlayerName,  (char* )record.Data[ 10 ], MAX_NAME_LENGTH+1 );

		g_csFarmManager.SRV_SendTaxToClient( pPlayer, &stFarmState );
	}
}
// E 농장시스템 추가 added by hseos 2007.04.23	2007.09.12	2008.01.16

// desc_hseos_몬스터미터01
// S 몬스터미터 추가 added by hseos 2007.05.28
void MonsterMeter_Load(DWORD nPlayerID)
{
	sprintf(txt, "EXEC  %s %d", STORED_MONSTERMETER_LOAD_INFO, nPlayerID);
	g_DB.Query(eQueryType_FreeQuery, eMonsterMeter_LoadInfo, nPlayerID, txt, nPlayerID);
}

void MonsterMeter_Save(DWORD nPlayerID, DWORD nPlayTime, DWORD nKillMonNum, DWORD nPlayTimeTotal, DWORD nKillMonNumTotal)
{
	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d", STORED_MONSTERMETER_SET_INFO, nPlayerID, nPlayTime, nKillMonNum, nPlayTimeTotal, nKillMonNumTotal);
	g_DB.Query(eQueryType_FreeQuery, eMonsterMeter_SetInfo, 0, txt, nPlayerID);
}

void RMonsterMeter_LoadInfo(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer ) return;

	if( pMessage->dwResult == 0 )
	{
		g_Console.LOG(4, "MonsterMeter Load Error. Inited Data..");
	}
	else
	{
		pPlayer->SetMonstermeterPlaytime(atoi((char*)pData[0].Data[0]), atoi((char*)pData[0].Data[2]));
		pPlayer->SetMonstermeterKillMonNum(atoi((char*)pData[0].Data[1]), atoi((char*)pData[0].Data[3]));
	}

	ResidentRegist_LoadInfo( pMessage->dwID );
}
// E 몬스터미터 추가 added by hseos 2007.05.28

// desc_hseos_주민등록01
// S 주민등록 추가 added by hseos 2007.06.04
void ResidentRegist_LoadInfo(DWORD nPlayerID)
{
	sprintf(txt, "EXEC  %s %d", STORED_RESIDENTREGIST_LOAD_INFO, nPlayerID);
	g_DB.FreeMiddleQuery(RResidentRegist_LoadInfo, nPlayerID, txt, nPlayerID);
}

void ResidentRegist_SaveInfo(DWORD nPlayerID, void* _pInfo, int nIntroduction)
{
	// 주민 등록. 등록시에 자기소개는 안 함
	if (nIntroduction == 0)
	{
		CSHResidentRegistManager::stRegistInfo* pInfo = (CSHResidentRegistManager::stRegistInfo*)_pInfo;

		sprintf(txt, "EXEC  %s %d, %s, %d, %d, %d, %d, %d, %d, %d, %d, %d", STORED_RESIDENTREGIST_SAVE_INFO, 
			nPlayerID,
			"XXX",
			pInfo->nSex, 
			pInfo->nAge,
			pInfo->nLocation,
			pInfo->nFavor,
			pInfo->nPropensityLike01,
			pInfo->nPropensityLike02,
			pInfo->nPropensityLike03,
			pInfo->nPropensityDisLike01,
			pInfo->nPropensityDisLike02);
	}
	// 자기소개
	else if (nIntroduction == 1)
	{
		char* pszIntroduction = (char*)_pInfo;

		sprintf(txt, "EXEC  %s %d, '%s'", STORED_RESIDENTREGIST_UPDATE_INTRODUCTION, nPlayerID, pszIntroduction);
	}

	g_DB.FreeMiddleQuery(NULL, 0, txt, nPlayerID);
}
// 080215 KTH -- Reset Resident Regist
void ResidentRegist_Reset(DWORD nPlayerID)
{
	sprintf(txt, "EXEC %s %d", STORED_RESIDENTREGIST_RESET, nPlayerID);
	g_DB.Query(eQueryType_FreeQuery, eResidentRegist_Reset, nPlayerID, txt, nPlayerID);

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(nPlayerID);

	pPlayer->UpdateLogoutToDB();
	PARTYMGR->UserLogOut(pPlayer);
	FriendNotifyLogouttoClient(pPlayer->GetID());
	GUILDMGR->UserLogOut(pPlayer);

	ConfirmUserOut(
		pPlayer->GetAgentNum(),
		pPlayer->GetUserID(),
		nPlayerID,
		1);
	// 마지막에 실행되어야 한다
	g_pServerSystem->RemovePlayer(
		nPlayerID);
}

void RResidentRegist_LoadInfo(LPMIDDLEQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer ) return;

	if( pMessage->dwResult == 0 )
	{
	}
	else
	{
		CSHResidentRegistManager::stRegistInfo stInfo;

		SafeStrCpy(stInfo.szNickName, (char*)pData[0].Data[0], MAX_NAME_LENGTH+1);
		stInfo.nSex					= WORD( atoi((char*)pData[0].Data[1]) );
		stInfo.nAge					= WORD( atoi((char*)pData[0].Data[2]) );
		stInfo.nLocation			= WORD( atoi((char*)pData[0].Data[3]) );
		stInfo.nFavor				= WORD( atoi((char*)pData[0].Data[4]) );
		stInfo.nPropensityLike01	= WORD( atoi((char*)pData[0].Data[5]) );
		stInfo.nPropensityLike02	= WORD( atoi((char*)pData[0].Data[6]) );
		stInfo.nPropensityLike03	= WORD( atoi((char*)pData[0].Data[7]) );
		stInfo.nPropensityDisLike01	= WORD( atoi((char*)pData[0].Data[8]) );
		stInfo.nPropensityDisLike02	= WORD( atoi((char*)pData[0].Data[9]) );

		DATE_MATCHING_INFO stInfo2;
		ZeroMemory(&stInfo2, sizeof(stInfo2));

		SafeStrCpy(stInfo2.szName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
		stInfo2.bIsValid			= TRUE;
		stInfo2.byGender			= (BYTE)stInfo.nSex;
		stInfo2.dwAge				= stInfo.nAge;

		CHARACTER_TOTALINFO stExInfo;
		pPlayer->GetCharacterTotalInfo(&stExInfo);

		// 100113 ONS 직업 정보를 저장한다.
		WORD idx = 1;
		if( stExInfo.JobGrade > 1 )
		{
			idx = stExInfo.Job[stExInfo.JobGrade - 1];
		}
		stInfo2.dwClass				= ( stExInfo.Job[0] * 1000 ) + ( ( stExInfo.Race + 1 ) * 100 ) + ( stExInfo.JobGrade * 10 ) + idx;
		stInfo2.nRace				= stExInfo.Race;
		stInfo2.dwRegion			= stInfo.nLocation;
		stInfo2.dwGoodFeelingDegree	= stInfo.nFavor;
		stInfo2.pdwGoodFeeling[0]	= stInfo.nPropensityLike01;
		stInfo2.pdwGoodFeeling[1]	= stInfo.nPropensityLike02;
		stInfo2.pdwGoodFeeling[2]	= stInfo.nPropensityLike03;
		stInfo2.pdwBadFeeling[0]	= stInfo.nPropensityDisLike01;
		stInfo2.pdwBadFeeling[1]	= stInfo.nPropensityDisLike02;
		SafeStrCpy(stInfo2.szIntroduce, (char*)pData[0].Data[10], MAX_INTRODUCE_LEN+1);
		pPlayer->SetResidentRegistInfo(&stInfo2);
	}

	pPlayer->SetInitState( PLAYERINITSTATE_FARM_INFO, MP_USERCONN_GAMEIN_SYN );
}
// E 주민등록 추가 added by hseos 2007.06.04

void SkillPointUpdate(DWORD CharacterIdx, DWORD SkillPoint)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC dbo.MP_SKILLTREE_SKILLPOINT_UPDATE %u, %u", CharacterIdx, SkillPoint);

	g_DB.Query(eQueryType_FreeQuery, eSkillPointUpdate, 0, txt, CharacterIdx);
}

void CharacterJobUpdate( DWORD CharacterIdx, WORD Grade, WORD Job1, WORD Job2, WORD Job3, WORD Job4, WORD Job5, WORD Job6 )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC dbo.MP_CHARACTER_JOB_UPDATE %u, %d, %d, %d, %d, %d, %d, %d", CharacterIdx, Grade, Job1, Job2, Job3, Job4, Job5, Job6);

	g_DB.Query(eQueryType_FreeQuery, eJobUpdate, 0, txt, CharacterIdx);

}

void GuildAddScore( DWORD guildIndex, float score )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_GUILD_ADD_SCORE %d, %f", guildIndex, score );

	g_DB.Query( eQueryType_FreeQuery, eGuildAddScore, guildIndex, txt );
}


void RGuildAddScore( LPQUERY query, LPDBMESSAGE message )
{
	if( 1 != message->dwResult )
	{
		ASSERT( 0 );
		return;
	}

	// 다른 맵서버는 알아서 주기적으로 업데이트할 것이다. 자신만 업데이트하면 된다.
	{
		const DWORD	guildIndex	= message->dwID;
		const DWORD	score		= atoi( ( char* )query[ 0 ].Data[ 0 ] );

		MSG_DWORD2 message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_SCORE_UPDATE;
		message.dwData1		= guildIndex;
		message.dwData2		= score;
		
		CGuildScore* guild = GUILDMGR->GetGuildScore( guildIndex );

		if( guild )
		{
			guild->Send( &message, sizeof( message ) );
		}
	}
}

// 080417 LUJ, 길드 스킬 획득
void RGuildAddSkill( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD	playerIndex	= dbMessage->dwID;
	
	CPlayer* player	= ( CPlayer* )g_pUserTable->FindUser( playerIndex );

	if( ! player )
	{
		return;
	}

	CGuild* guild = GUILDMGR->GetGuild( player->GetGuildIdx() );

	if( ! guild )
	{
		return;
	}

	const DWORD		skillIndex	=			atoi( ( char* )query[ 0 ].Data[ 0 ] );
	const BYTE		skillLevel	=			BYTE( atoi( ( char* )query[ 0 ].Data[ 1 ] ) );
	const MONEYTYPE	money		= MONEYTYPE(atoi( ( char* )query[ 0 ].Data[ 2 ] ) );
	const float		guildScore	= float(	atof( ( char* )query[ 0 ].Data[ 3 ] ) );

	// 080417 LUJ, 스킬 인덱스가 0으로 반환되면 포인트가 부족해서 스킬 획득 실패
	if( ! skillIndex )
	{
		GUILDMGR->SendNackMsg( player, MP_GUILD_ADD_SKILL_NACK, eGuildErr_InsufficentScore );
		return;
	}
	else if( player->GetMoney() < money )
	{
		GUILDMGR->SendNackMsg( player, MP_GUILD_ADD_SKILL_NACK, eGuildErr_InsufficentMoney );
		return;
	}

	player->SetMoney( money, MONEY_SUBTRACTION );
	
	// 080417 LUJ, 로그
	{
		InsertLogGuildScore(
			guild->GetIdx(),
			guildScore,
			eGuildLog_SkillAdd );

		LogGuild(
			player->GetID(),
			guild->GetIdx(),
			eGuildLog_SkillAdd,
			skillIndex,
			skillLevel );
	}

	// 080417 LUJ, 다른 맵에도 전파한다
	{
		MSG_SKILL_UPDATE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category				= MP_GUILD;
		message.Protocol				= MP_GUILD_ADD_SKILL_ACK_TO_MAP;
		message.dwObjectID				= guild->GetIdx();
		message.SkillBase.wSkillIdx		= skillIndex;
		message.SkillBase.Level			= skillLevel;

		g_Network.Broadcast2AgentServer( ( char* )&message, sizeof( message ) );

		// 080417 LUJ, 현재 맵에도 전파한다
		GUILDMGR->NetworkMsgParse( MP_GUILD_ADD_SKILL_ACK_TO_MAP, &message );
	}	
}

// 080417 LUJ, 길드 스킬 로딩
void RGuildGetSkill( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD guildIndex = dbMessage->dwID;

	CGuild* guild = GUILDMGR->GetGuild( guildIndex );

	if( ! guild )
	{
		return;
	}

	MSG_SKILL_LIST message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_GET_SKILL_ACK;
		message.mSize		= dbMessage->dwResult;
	}
	
	for( DWORD row = 0; row < dbMessage->dwResult; ++row )
	{
		const MIDDLEQUERYST& record = query[ row ];

		const DWORD index	=		atoi( ( char* )record.Data[ 0 ] );
		const BYTE	level	= BYTE( atoi( ( char* )record.Data[ 1 ] ) );

		guild->AddSkill( index, level );

		{
			SKILL_BASE& data = message.mData[ row ];

			data.wSkillIdx	= index;
			data.Level		= level;
		}
	}

	// 080417 LUJ, 접속된 플레이어에게 스킬 정보를 전파한다. 플레이어는 직위에 따라 알아서 설정할 것이다
	guild->SendMsgToAll( &message, message.GetSize() );

	// 080417 LUJ, 길드 스킬을 최대로 읽어들였으면 DB에 다시 요청한다
	if( MAX_MIDDLEROW_NUM == dbMessage->dwResult )
	{
		const MIDDLEQUERYST& record			= query[ MAX_MIDDLEROW_NUM - 1 ];
		const DWORD			 lastSkillIndex = atoi( ( char* )record.Data[ 0 ] );

		g_DB.FreeMiddleQuery(
			RGuildGetSkill,
			guildIndex,
			"SELECT TOP %d SKILL_IDX, SKILL_LEVEL FROM TB_GUILD_SKILL WHERE GUILD_IDX = %d AND SKILL_IDX > %d ORDER BY SKILL_IDX",
			MAX_MIDDLEROW_NUM,
			guildIndex,
			lastSkillIndex );
	}
}

void GuildUpdateScore( CPlayer* killer, CPlayer* corpse, float killerPoint, float corpsePoint )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_GUILD_UPDATE_SCORE %d, %d, %.0f, %d, %d, %.0f",
		killer->GetID(),
		killer->GetGuildIdx(),
		killerPoint,
		corpse->GetID(),
		corpse->GetGuildIdx(),
		corpsePoint );

	g_DB.Query( eQueryType_FreeQuery, eGuildUpdateScore, 0, txt, killer->GetID() );
}


void RGuildUpdateScore( LPQUERY query, LPDBMESSAGE message )
{
	// 레코드 두개가 반환되어야 정상적으로 업데이트된 경우
	if( 2 != message->dwResult )
	{
		ASSERT( 0 );
		return;
	}	

	// 길드 점수가 0인 경우 차감할 길드 점수가 부족하여 갱신하지 못한 경우임.
	// 클라이언트가 이를 판단하여 적절한 메시지를 표시하면 해결됨
	{
		const QUERYST&	killerRecord		= query[ 0 ];
		const QUERYST&	corpseRecord		= query[ 1 ];
		const DWORD		killerPlayerIndex	= atoi( ( char* )killerRecord.Data[ 0 ] );	// 살해한 길드원 플레이어 인덱스
		const DWORD		corpsePlayerIndex	= atoi( ( char* )corpseRecord.Data[ 0 ] );	// 살해당한 길드원 플레이어 인덱스
		

		MSG_DWORD3 message;
		{
			message.Category	= MP_GUILD;
			message.dwData1		= killerPlayerIndex;
			message.dwData2		= corpsePlayerIndex;
		}

		// 첫번째 레코드는 다른 길드의 멤버를 살해하여 갱신된 길드 포인트
		{
			// 갱신이 실패할 경우 -1이 DB에서 전달되고, guildScore에는 UINT_MAX 값이 들어간다
			const DWORD			guildScore	= atoi( ( char* )killerRecord.Data[ 1 ] );
			const DWORD			guildIndex	= atoi( ( char* )killerRecord.Data[ 2 ] );
			const CGuildScore*	guild		= GUILDMGR->GetGuildScore( guildIndex );
			
			if( guild )
			{
				message.Protocol	= MP_GUILD_SCORE_UPDATE_BY_KILL;
				message.dwData3		= guildScore;
				
				guild->Send( &message, sizeof( message ) );
			}
		}

		// 두번째 레코드는 다른 길드원에게 살해당해 갱신된 길드 포인트
		{
			// 갱신이 실패할 경우 -1이 DB에서 전달되고, guildScore에는 UINT_MAX 값이 들어간다
			const DWORD			guildScore	= atoi( ( char* )corpseRecord.Data[ 1 ] );
			const DWORD			guildIndex	= atoi( ( char* )corpseRecord.Data[ 2 ] );
			const CGuildScore*	guild		= GUILDMGR->GetGuildScore( guildIndex );

			if( guild )
			{
				message.Protocol	= MP_GUILD_SCORE_UPDATE_BY_DEAD;
				message.dwData3		= guildScore;
				
				guild->Send( &message, sizeof( message ) );
			}
		}
	}
}

void GuildReadData( DWORD playerIndex, DWORD guildIndex )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_GUILD_SELECT %d", guildIndex );

	g_DB.FreeMiddleQuery( RGuildReadData, playerIndex, txt );
}


void RGuildReadData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	if( 1 != dbMessage->dwResult )
	{
		ASSERT( 0 );
		return;
	}

	const DWORD	playerIndex = dbMessage->dwID;
	CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(
		playerIndex);

	if(NULL == player)
	{
		return;
	}
	else if(eObjectKind_Player != player->GetObjectKind())
	{
		return;
	}

	// 길드 정보의 동기화를 위해 DB에 의존하기 때문에 메모리에 담지 않고 전송한다
	{
		const MIDDLEQUERYST& record = query[ 0 ];
		MSG_GUILD_DATA message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_GUILD;
		message.Protocol = MP_GUILD_DATA_READ;
		message.mIndex = atoi( ( char* )record.Data[ 0 ] );
		message.mLevel = atoi( ( char* )record.Data[ 1 ] );
		message.mUnionIndex = atoi( ( char* )record.Data[ 2 ] );
		message.mScore = atoi( ( char* )record.Data[ 3 ] );
		message.mLocation = POSTYPE( atoi( ( char* )record.Data[ 4 ] ) );
		message.mWarehouseSize = atoi( ( char* )record.Data[ 5 ] );
		message.mWarehouseRank = atoi( ( char* )record.Data[ 6 ] );
		
		const DWORD masterIndex	= atoi( ( char* )record.Data[ 7 ] );

		SafeStrCpy( message.mName,	( char* )record.Data[ 8 ], sizeof( message.mName ) );
		SafeStrCpy( message.mMark,	( char* )record.Data[ 9 ], sizeof( message.mMark ) );
		SafeStrCpy( message.mIntro, ( char* )record.Data[ 10 ], sizeof( message.mIntro ) );
		SafeStrCpy( message.mNotice, ( char* )record.Data[ 11 ], sizeof( message.mNotice ) );
		
		if(CGuild* guild = GUILDMGR->GetGuild(message.mIndex))
		{
			guild->SetMasterIndex(
				masterIndex);
			guild->GetTotalMember(
				&message.mMember[0]);
			guild->SetLevel(
				LEVELTYPE(message.mLevel));
			guild->SetGuildUnionIdx(
				message.mUnionIndex);

			message.mMemberSize	= guild->GetMemberNum();
		}

		player->SendMsg(
			&message,
			message.GetSize());
		GUILDMGR->UserLogIn(
			player);
	}
}

void RGuildSetLevel( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{	
	// 길드 점수가 부족했을 때, 퀘스트 조건이 맞지 않을때 실패한다
	if(0 == dbMessage->dwResult)
	{
		if(CObject* const object = g_pUserTable->FindUser( dbMessage->dwID ))
		{
			MSGBASE message;
			message.Category = MP_GUILD;
			message.Protocol = MP_GUILD_LEVELUP_NACK;
			object->SendMsg(
				&message,
				sizeof(message));
		}
		
		return;
	}

	const MIDDLEQUERYST& record	= query[0];
	const DWORD guildIndex = atoi(LPCTSTR(record.Data[0]));
	const DWORD guildScore = atoi(LPCTSTR(record.Data[1]));
	const MONEYTYPE usedMoney = atoi(LPCTSTR(record.Data[2]));
	const float losingScore = float(atof(LPCTSTR(record.Data[3])));
	const LEVELTYPE guildLevel = LEVELTYPE(atoi(LPCTSTR(record.Data[4])));

	if(CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(dbMessage->dwID))
	{
		player->SetMoney(
			usedMoney,
			MONEY_SUBTRACTION,
			MF_LOST,
			eItemTable_Inventory,
			0,
			0);
	}

	MSG_DWORD3 message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_GUILD;
	message.Protocol = MP_GUILD_LEVELUP_NOTIFY_TO_MAP;
	message.dwData1 = guildIndex;
	message.dwData2 = guildScore;
	message.dwData3 = guildLevel;

	GUILDMGR->NetworkMsgParse(
		message.Protocol,
		LPVOID(&message));
	g_Network.Send2AgentServer(
		(char*)&message,
		sizeof(message));

	LogGuild(
		dbMessage->dwID,
		guildIndex,
		eGuildLog_ScoreLevelUp,
		guildLevel,
		usedMoney);
	InsertLogGuildScore(
		guildIndex,
		losingScore,
		eGuildLog_ScoreLevelUp);
}


void GuildResetWarehouse( MAPTYPE mapIndex )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		0,
		"EXEC dbo.MP_GUILD_WAREHOUSE_RESET %d",
		mapIndex );
}


void GuildOpenWarehouse( DWORD guildIndex, DWORD playerIndex, MAPTYPE mapIndex )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RGuildOpenWarehouse,
		playerIndex,
		"EXEC dbo.MP_GUILD_WAREHOUSE_OPEN %d, %d, %d",
		guildIndex,
		mapIndex,
		playerIndex );
}


void RGuildOpenWarehouse( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD playerIndex = dbMessage->dwID;
	CObject*	object		= g_pUserTable->FindUser( playerIndex );

	if( !	object ||
			dbMessage->dwResult != 1 )
	{
		ASSERT( 1 == dbMessage->dwResult );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_GUILD_OPEN_WAREHOUSE message;

	{
		message.Category		= MP_GUILD;
		message.mWarehouseSize	= USHORT(	atoi( ( char* )record.Data[ 0 ] ) );
		message.mWarehouseRank	= BYTE(		atoi( ( char* )record.Data[ 1 ] ) );
		message.mMoney			= atoi( ( char* )record.Data[ 2 ] );
		
		SafeStrCpy( message.mName, ( char* )record.Data[ 3 ], sizeof( message.mName ) );
	}

	const DWORD	openedPlayerIndex = atoi( ( char* )record.Data[ 4 ] );
	const DWORD	guildIndex = atoi( ( char* )record.Data[ 5 ] );
	CGuild*	guild = GUILDMGR->GetGuild( guildIndex );

	if( ! guild )
	{
		ASSERT( 0 );
		return;		
	}

	// 길드 창고를 열었으면 아이템 정보도 가져온다. 일단 옵션 읽고 아이템 정보 읽어야한다
	if( message.mWarehouseSize &&
		playerIndex == openedPlayerIndex )
	{
		guild->SetPlayerInWarehouse(
			playerIndex);
		GuildItemSelect(
			playerIndex,
			guildIndex,
			0);

		message.Protocol = MP_GUILD_OPEN_WAREHOUSE_ACK;
	}
	else
	{
		message.Protocol = MP_GUILD_OPEN_WAREHOUSE_NACK;
	}

	object->SendMsg( &message, sizeof( message ) );
}


void GuildCloseWarehouse( DWORD guildIndex, DWORD playerIndex, MAPTYPE mapIndex )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RGuildCloseWarehouse,
		guildIndex,
		"EXEC dbo.MP_GUILD_WAREHOUSE_CLOSE %d, %d, %d",
		guildIndex,
		mapIndex,
		playerIndex );
}


void RGuildCloseWarehouse( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	// 길드마다 창고는 한명만 사용 가능하므로, 창고가 닫히면 다시 접근할 수 있도록 초기화해준다
	const DWORD guildIndex = dbMessage->dwID;

	if(CGuild* const guild = GUILDMGR->GetGuild(guildIndex))
	{
		guild->SetPlayerInWarehouse(
			0);
	}
}


void GuildItemSelect( DWORD playerIndex, DWORD guildIndex, DWORD beginItemDbIndex )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_GUILD_ITEM_SELECT %d, %d", guildIndex, beginItemDbIndex );

	g_DB.FreeQuery( eGuildItemSelect, playerIndex, txt, playerIndex );
}


void RGuildItemSelect( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD playerIndex = dbMessage->dwID;
	CPlayer*	player		= ( CPlayer* )g_pUserTable->FindUser( playerIndex );

	if( ! player )
	{
		return;
	}

	if( SLOT_GUILDWAREHOUSE_NUM < dbMessage->dwResult )
	{
		ASSERT( 0 );
		return;
	}

	CGuild* guild = GUILDMGR->GetGuild( player->GetGuildIdx() );

	if( ! guild )
	{
		return;
	}

	guild->ClearItem();

	MSG_ITEM_OPTION optionMessage;
	{
		ZeroMemory( &optionMessage, sizeof( optionMessage ) );

		optionMessage.Category	= MP_ITEM;
		optionMessage.Protocol	= MP_ITEM_OPTION_ADD_ACK;
	}

	MSG_GUILD_ITEM message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_ITEM_READ;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&	record	= query[ i ];
		ITEMBASE&		item	= message.mItem[ i ];

		item.dwDBIdx		= atoi( ( char* )record.Data[ 1 ] );
		item.wIconIdx		= atoi( ( char* )record.Data[ 2 ] );
		item.Position		= POSTYPE( atoi( ( char* )record.Data[ 3 ] ) );
		item.Durability		= atoi( ( char* )record.Data[ 5 ] );
		item.nSealed		= ITEM_SEAL_TYPE(atoi( ( char* )record.Data[ 7 ]));
		item.nRemainSecond	= atoi( ( char* )record.Data[ 9 ] );

		const ITEM_INFO* itemInfo = ITEMMGR->GetItemInfo( item.wIconIdx );

		if( ! itemInfo )
		{
			continue;
		}

		// 091110 pdy eITEM_TYPE_GET_UNSEAL에 대한 처리추가	
		if ( item.nSealed == eITEM_TYPE_GET_UNSEAL )
		{
			DWORD dwRemainTime = 0;

			if( itemInfo->nTimeKind == eKIND_REALTIME )
			{
				// 최초 DBInsert경과 시간을 구한다. 
				DWORD dwRateDBInsertTime = (DWORD) abs( atoi( ( char* )record.Data[ 8 ] ) );

				// DBInsert 경과 시간이 사용기간보다 크거나 같을 경우엔 이미 기간이 지났음으로 삭제
				if( dwRateDBInsertTime >= itemInfo->dwUseTime )
				{
					// 071231 KTH -- GetID Add
					ItemDeleteToDB(item.dwDBIdx );
					
					LogItemMoney(player->GetID(), player->GetObjectName(), 0, "",
						eLog_ShopItemUseEnd, player->GetMoney(eItemTable_Inventory), 0, 0,
						item.wIconIdx , item.dwDBIdx, item.Position, 0, item.Durability, player->GetPlayerExpPoint());

					ZeroMemory( &item , sizeof( ITEMBASE ) );
					continue;
				}

				// 사용기간에서 최초 DBInsert경과 시간을 빼준다.
				dwRemainTime = itemInfo->dwUseTime - dwRateDBInsertTime;

				UnSealItemByRealTime(player->GetID(), item.dwDBIdx, dwRemainTime);
			}
			else if( itemInfo->nTimeKind ==	eKIND_PLAYTIME )
			{
				dwRemainTime = itemInfo->dwUseTime;

				UnSealItemByGameTime(player->GetID(), item.dwDBIdx, itemInfo->dwUseTime );
			}

			item.nSealed = eITEM_TYPE_UNSEAL ;
			item.nRemainSecond = dwRemainTime;
		}

		// 080820 LUJ, 획득 시 쿨타임을 갖는 아이템은 경과 시간을 검사하기 위해 쿼리한 시간을 설정해야한다
		item.ItemParam		= ( ITEM_KIND_COOLTIME == itemInfo->SupplyType ) ? gCurTime : 0;

		guild->InitGuildItem( item );

		// 옵션 로딩
		{
			const QUERYST& record = query[ i ];

			ITEM_OPTION& option = optionMessage.mOption[ optionMessage.mSize ];

			{
				ITEM_OPTION::Reinforce& data = option.mReinforce;

				data.mStrength			= atoi( ( char* )record.Data[ 10 ] );
				data.mDexterity			= atoi( ( char* )record.Data[ 11 ] );
				data.mVitality			= atoi( ( char* )record.Data[ 12 ] );
				data.mIntelligence		= atoi( ( char* )record.Data[ 13 ] );
				data.mWisdom			= atoi( ( char* )record.Data[ 14 ] );
				data.mLife				= atoi( ( char* )record.Data[ 15 ] );
				data.mMana				= atoi( ( char* )record.Data[ 16 ] );
				data.mLifeRecovery		= atoi( ( char* )record.Data[ 17 ] );
				data.mManaRecovery		= atoi( ( char* )record.Data[ 18 ] );
				data.mPhysicAttack		= atoi( ( char* )record.Data[ 19 ] );
				data.mPhysicDefence		= atoi( ( char* )record.Data[ 20 ] );
				data.mMagicAttack		= atoi( ( char* )record.Data[ 21 ] );
				data.mMagicDefence		= atoi( ( char* )record.Data[ 22 ] );
				data.mMoveSpeed			= atoi( ( char* )record.Data[ 23 ] );
				data.mEvade				= atoi( ( char* )record.Data[ 24 ] );
				data.mAccuracy			= atoi( ( char* )record.Data[ 25 ] );
				data.mCriticalRate		= atoi( ( char* )record.Data[ 26 ] );
				data.mCriticalDamage	= atoi( ( char* )record.Data[ 27 ] );
				SafeStrCpy( option.mReinforce.mMadeBy, ( char* )record.Data[ 28 ], sizeof( option.mReinforce.mMadeBy ) );				
			}

			{
				ITEM_OPTION::Mix& data = option.mMix;

				data.mStrength		= atoi( ( char* )record.Data[ 29 ] );
				data.mIntelligence	= atoi( ( char* )record.Data[ 30 ] );
				data.mDexterity		= atoi( ( char* )record.Data[ 31 ] );
				data.mWisdom		= atoi( ( char* )record.Data[ 32 ] );
				data.mVitality		= atoi( ( char* )record.Data[ 33 ] );
				SafeStrCpy( option.mMix.mMadeBy, ( char* )record.Data[ 34 ], sizeof( option.mMix.mMadeBy ) );				
			}

			{
				ITEM_OPTION::Enchant& data = option.mEnchant;
				data.mIndex	= atoi((char*)record.Data[35]);
				data.mLevel	= BYTE(atoi((char*)record.Data[36]));
				SafeStrCpy(option.mEnchant.mMadeBy, (char*)record.Data[37], sizeof(option.mEnchant.mMadeBy));
			}

			{
				ITEM_OPTION::Drop& data = option.mDrop;

				data.mValue[ 0 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 38 ] ) );
				data.mValue[ 0 ].mValue	= float( atof( ( char* )record.Data[ 39 ] ) );

				data.mValue[ 1 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 40 ] ) );
				data.mValue[ 1 ].mValue	= float( atof( ( char* )record.Data[ 41 ] ) );

				data.mValue[ 2 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 42 ] ) );
				data.mValue[ 2 ].mValue	= float( atof( ( char* )record.Data[ 43 ] ) );

				data.mValue[ 3 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 44 ] ) );
				data.mValue[ 3 ].mValue	= float( atof( ( char* )record.Data[ 45 ] ) );

				data.mValue[ 4 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 46 ] ) );
				data.mValue[ 4 ].mValue	= float( atof( ( char* )record.Data[ 47 ] ) );
			}

			const ITEM_OPTION emptyOption = { 0 };

			if( memcmp( &emptyOption, &option, sizeof( emptyOption ) ) )
			{
				option.mItemDbIndex	= item.dwDBIdx;
				ITEMMGR->AddOption( option );

				++optionMessage.mSize;
			}
		}

		// 081202 LUJ, 수량성 아이템의 개수가 0인 경우, 삭제한다
		if(		itemInfo->Stack &&
			!	item.Durability )
		{
			LogItemMoney(
				player->GetID(),
				player->GetObjectName(),
				0,
				"*invalid",
				eLog_ItemDiscard,
				player->GetMoney(),
				0,
				0,
				item.wIconIdx,
				item.dwDBIdx,
				item.Position,
				0,
				item.Durability,
				0 );
			ItemDeleteToDB( item.dwDBIdx );
			ZeroMemory( &item, sizeof( item ) );
		}
	}

	player->SendMsg( &optionMessage, optionMessage.GetSize() );
	player->SendMsg( &message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record			= query[ MAX_ROW_NUM - 1 ];
		const DWORD		lastItemDbIndex	= atoi( ( char* )record.Data[ 1 ] );

		GuildItemSelect( playerIndex, player->GetGuildIdx(), lastItemDbIndex );
	}
}


void GuildSetWarehouseRank( DWORD guildIndex, DWORD playerIndex, BYTE rank )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RGuildSetWarehouseRank,
		playerIndex,
		"EXEC dbo.MP_GUILD_WAREHOUSE_RANK_UPDATE %d, %d, %d",
		guildIndex,
		playerIndex,
		rank, playerIndex );
}


void RGuildSetWarehouseRank( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	if( 1 != dbMessage->dwResult )
	{
		ASSERT( 0 );
		return;
	}

	CObject* object = g_pUserTable->FindUser( dbMessage->dwID );

	if( object )
	{
		const MIDDLEQUERYST& record = query[ 0 ];

		MSG_GUILD_SET_WAREHOUSE_RANK message;

		message.Category	= MP_GUILD;
        message.Protocol	= MP_GUILD_SET_WAREHOUSE_RANK_ACK;
		message.mRank		= BYTE( atoi( ( char* )record.Data[ 0 ] ) );
        		
		object->SendMsg( &message, sizeof( message ) );
	}
}


void GuildChangeMaster(DWORD guildIndex, DWORD newMasterPlayerIndex, BYTE masterRank, BYTE exMasterRank )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RGuildChangeMaster,
		guildIndex,
		 "EXEC dbo.MP_GUILD_CHANGE_MASTER %d, %d, %d, %d",
		 guildIndex,
		 newMasterPlayerIndex,
		 masterRank,
		 exMasterRank, newMasterPlayerIndex );
}


void RGuildChangeMaster( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		ASSERT( 0 );
		return;
	}

	const MIDDLEQUERYST&	record			= query[ 0 ];
	const DWORD				newMasterIndex	= atoi( ( char* )record.Data[ 0 ] );
	const DWORD				guildIndex		= dbMessage->dwID;

	if(CGuild* const guild = GUILDMGR->GetGuild(guildIndex))
	{
		LogGuild(
			guild->GetMasterIdx(),
			guildIndex,
			eGuildLog_Retire,
			newMasterIndex,
			0);
	}

	MSG_DWORD2 message;
	message.Category = MP_GUILD;
	message.Protocol = MP_GUILD_CHANGE_MASTER_TO_AGENT;
	message.dwData1	= guildIndex;
	message.dwData2	= newMasterIndex;

	g_Network.Send2AgentServer(
		(char*)&message,
		sizeof(message));
	GUILDMGR->NetworkMsgParse(
		message.Protocol,
		LPVOID(&message));
}


void CharacterLogoutPointUpdate(DWORD playerIndex, MAPTYPE mapType, float positionX, float positionZ)
{
	g_DB.FreeQuery(
		eCharacterLogoutPointUpdate,
		playerIndex,
		"EXEC dbo.MP_CHARACTER_LOGOUTPOINTUPDATE %d, %d, %0.0f, %0.0f",
		playerIndex,
		mapType,
		positionX,
		positionZ);
}

// 070917 LYW --- MapDBMsgParser : Add function to change map point to return result.
void RSaveNpcMapChangePointReturn(LPQUERY pData, LPDBMESSAGE pMessage)
{
	DWORD dwConnectionIndex = (DWORD)atoi((char*)pData->Data[0]);
	DWORD Map = (DWORD)atoi((char*)pData->Data[1]);

	MSG_DWORD msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_NPC_MAP_OUT; 
	msg.dwObjectID = pMessage->dwID;
	msg.dwData = Map;

	g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(msg));
}


void GuildSelectForRewardByWar( DWORD winnerGuildIndex, DWORD winnerUnionIndex, DWORD loserGuildIndex, DWORD loserUnionIndex )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.MP_GUILDFIELDWAR_SELECT_FOR_REWARD %d, %d, %d, %d",
		winnerGuildIndex,
		winnerUnionIndex,
		loserGuildIndex,
		loserUnionIndex );

	g_DB.FreeQuery( eGuildSelectForReward, 0, txt );
}


void RGuildSelectForReward( LPQUERY query, LPDBMESSAGE dbMessage )
{
	if( ! dbMessage->dwResult )
	{
		return;
	}

	// 주의:
	// 점수를 얻은 맵이 아닌 다른 맵은 점수를 즉각적으로 갱신하지 않으므로 비동기 현상이 나타난다.
	// 이에 따라 변화된 정도(variation)도 함께 전달해줘야 한다.

	// first: 갱신할 점수값, second: 변화된 값
	typedef std::pair< float, float >	Score;

	// 키: 길드 인덱스, 값: 길드 점수
	typedef std::map< DWORD, Score >	Group;
	Group								winnerGroup;
	Group								loserGroup;

	// 각각을 그룹으로 만든다. 편의상 하나를 승자 그룹, 하나를 패자 그룹으로 나눈다.
	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&	record		= query[ i ];
		const BOOL		isWinner	= atoi( ( char* )record.Data[ 0 ] );
		const DWORD		guildIndex	= atoi( ( char* )record.Data[ 1 ] );
		const float		score		= float( atof( ( char* )record.Data[ 2 ] ) );

		if( isWinner )
		{
			winnerGroup[ guildIndex ].first = score;
		}
		else
		{
			loserGroup[ guildIndex ].first	= score;
		}
	}

	float losedScore = 0;

	// 패자 그룹의 길드 점수를 규칙에 따라 차감하도록 DB에 요청한다.
	// 차감한 합계를 승리 그룹에게 더해줘야 하므로 차감한 합을 저장하자
	for( Group::iterator it = loserGroup.begin(); loserGroup.end() != it; ++it )
	{
		const DWORD guildIndex	= it->first;
		Score&		score		= it->second;
		//const float score		= it->second;
		const float losingScore = max( -1.0f * score.first, GUILDMGR->GetLosingScore( score.first ) );

		if( ! losingScore )
		{
			continue;
		}

		// losingScore가 음수이고, 뺄 값을 얻어야한다는 점을 유의하자. 가진 점수 이상 빼면 음수가 되는 것도 주의
		GuildAddScore( guildIndex, losingScore );

		// 071114 웅주, 로그
		InsertLogGuildScore( guildIndex, losingScore, eGuildLog_ScoreRemoveByDefeat );

		losedScore += losingScore;

		score.first		+= losingScore;
		score.second	= abs( losingScore );

		//loserGroup[ guildIndex ] = score + losingScore;
	}

	// 승자 그룹의 길드 점수를 규칙에 따라 더하도록 DB에 요청한다
	const CGuildManager::ScoreSetting&	setting		= GUILDMGR->GetScoreSetting();
	const float							addingScore	= abs( losedScore ) * setting.mWinScore.mPercent / winnerGroup.size();

	if( addingScore )
	{
		for( Group::iterator it = winnerGroup.begin(); winnerGroup.end() != it; ++it )
		{
			const DWORD guildIndex	= it->first;
			//const float score		= it->second;
			Score&		score		= it->second;

			GuildAddScore( guildIndex, addingScore );

			// 071114 웅주, 로그
			InsertLogGuildScore( guildIndex, addingScore, eGuildLog_ScoreAddByWin );

			//winnerGroup[ guildIndex ] = score + addingScore;

			score.first		+= addingScore;
			score.second	= addingScore;
		}
	}
	
	// 메시지 전송
	{
		MSG_DWORD4 messageToClient;
		messageToClient.Category = MP_GUILD_WAR;
		messageToClient.Protocol = MP_GUILD_WAR_SURRENDER;

		for( Group::const_iterator it = winnerGroup.begin(); winnerGroup.end() != it; ++it )
		{
			const DWORD		winnerGuildIndex	= it->first;
			//const DWORD winnerScore			= DWORD( it->second );
			const Score&	winnerScore			= it->second;
			CGuild*			winner				= GUILDMGR->GetGuild( winnerGuildIndex );

			if( ! winner )
			{
				ASSERT( 0 );
				continue;
			}

			for( Group::const_iterator inner = loserGroup.begin(); loserGroup.end() != inner; ++inner )
			{
				const DWORD		loserGuildIndex	= inner->first;
				//const DWORD loserScore		= DWORD( inner->second );
				const Score&	loserScore		= inner->second;
				CGuild*			loser			= GUILDMGR->GetGuild( loserGuildIndex );

				if( ! loser )
				{
					ASSERT( 0 );
					continue;
				}

				{
					messageToClient.dwData1 = 0;	// 0 = surrender
					messageToClient.dwData2 = winnerGuildIndex;
					//messageToClient.dwData3	= loserScore;
					messageToClient.dwData3	= DWORD( loserScore.first );	// 바뀔 값
					messageToClient.dwData4	= DWORD( loserScore.second );	// 잃은 정도

					loser->SendMsgToAll( &messageToClient, sizeof( messageToClient ) );
				}

				{
					messageToClient.dwData1 = 1;	// 1 = win
					messageToClient.dwData2 = loserGuildIndex;
					//messageToClient.dwData3	= winnerScore;
					messageToClient.dwData3	= DWORD( winnerScore.first );	// 바뀔 값
					messageToClient.dwData4	= DWORD( winnerScore.second );	// 얻은 정도

					winner->SendMsgToAll( &messageToClient, sizeof( messageToClient ) );
				}

				{
					MSG_GUILD_WAR_RESULT message;
					message.Category	= MP_GUILD_WAR;
					message.Protocol	= MP_GUILD_WAR_SURRENDER_NOTIFY_TOMAP;

					{
						MSG_GUILD_WAR_RESULT::Data& data = message.mWinner;

						data.mGuildIndex		= winnerGuildIndex;
						data.mScoreValue		= DWORD( winnerScore.first );
						data.mScoreVariation	= winnerScore.second;
					}

					{
						MSG_GUILD_WAR_RESULT::Data& data = message.mLoser;

						data.mGuildIndex		= loserGuildIndex;
						data.mScoreValue		= DWORD( loserScore.first );
						data.mScoreVariation	= loserScore.second;
					}

					g_Network.Send2AgentServer( ( char* )&message, sizeof( message ) );
				}

				DeleteGuildWar( winnerGuildIndex, loserGuildIndex );
				GUILDWARMGR->UnregistGuildWar( loserGuildIndex, winnerGuildIndex );

				GUILDWARMGR->UpdateGuildWarRecord( TRUE, 0, loserGuildIndex );
				GUILDWARMGR->UpdateGuildWarRecord( TRUE, 1, winnerGuildIndex );

				//GUILDWARMGR->SendToAllUser( 2, winner->GetGuildName(), loser->GetGuildName() );
			}
		}
	}
}

// 071023 LYW --- MapDBMsgParser : Add function pointer for tutorial.
void RTutorialLoad(LPQUERY pData, LPDBMESSAGE pMessage)
{
	SEND_TUTORIAL_DATA msg;													// 튜토리얼 데이터를 담을 구조체를 선언한다.
	memset(&msg.TutorialList, 0, sizeof(msg.TutorialList));					// 튜토리얼 데이터 구조체를 초기화 한다.

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);	// 넘어온 메시지 아이디로 플레이어 정보를 받는다.

	if( !pPlayer ) return;													// 플레이어 정보가 유효하지 않으면, return 처리를 한다.

	msg.Category = MP_TUTORIAL ;											// 카테고리를 튜토리얼로 세팅한다.				
	msg.Protocol = MP_TUTORIAL_MAINDATALOAD ;								// 프로토콜을 튜토리얼 메인 데이터 로딩으로 세팅한다.
	msg.dwObjectID = pMessage->dwID ;										// 오브젝트 아이디를 세팅한다.

	DWORD count = 0;

	for( count = 0 ; count < pMessage->dwResult ; ++count )					// 메시지의 결과값보다 count 가 작을 동안 for문을 돌린다.
	{	
		msg.TutorialList[count].nTutorialIdx = (int)atoi((char*)pData[count].Data[0]);
		msg.TutorialList[count].IsComplete	 = (BOOL)atoi((char*)pData[count].Data[1]);
	}

	msg.wCount = (WORD)count;												// 메시지의 카운트를 세팅한다.

	pPlayer->SendMsg( &msg, msg.GetSize() );								// 플레이어에게 메시지를 전송한다.

	//---KES 퀘스트 DB 결과 완료
	pPlayer->SetInitState( PLAYERINITSTATE_QEUST_INFO, MP_USERCONN_GAMEIN_SYN );
}

// 071023 LYW --- MapDBMsgParser : 튜토리얼 업데이트 쿼리문 처리에 대한 결과 처리를 하기 위한 함수.
void RTutorialUpdate(LPQUERY pData, LPDBMESSAGE pMessage)
{
	SEND_TUTORIAL_DATA msg;													// 튜토리얼 데이터를 담을 구조체를 선언한다.
	memset(&msg.TutorialList, 0, sizeof(msg.TutorialList));					// 튜토리얼 데이터 구조체를 초기화 한다.

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);	// 넘어온 메시지 아이디로 플레이어 정보를 받는다.

	if( !pPlayer ) return;													// 플레이어 정보가 유효하지 않으면, return 처리를 한다.

	msg.Category = MP_TUTORIAL ;											// 카테고리를 튜토리얼로 세팅한다.				
	msg.Protocol = MP_TUTORIAL_UPDATE_ACK ;									// 프로토콜을 튜토리얼 메인 데이터 로딩으로 세팅한다.
	msg.dwObjectID = pMessage->dwID ;										// 오브젝트 아이디를 세팅한다.

	if( pMessage->dwResult == 0 ) return ;									// 메시지의 결과 값이 0이면 return 처리를 한다.

	DWORD	count ;															// for문을 돌리기 위한 카운트를 선언한다.

	for( count = 0 ; count < pMessage->dwResult ; ++count )					// 메시지의 결과값보다 count 가 작을 동안 for문을 돌린다.
	{	
		msg.TutorialList[count].nTutorialIdx = (int)atoi((char*)pData[count].Data[0]) ;
		msg.TutorialList[count].IsComplete	 = (BOOL)atoi((char*)pData[count].Data[1]) ;
	}

	msg.wCount = (WORD)count;												// 메시지의 카운트를 세팅한다.

	pPlayer->SendMsg( &msg, msg.GetSize() );	
}






// 071207 LYW --- MapDBMsgParser : 스탯 초기화 프로시저 실행 결과 함수.
void RUpdateToDefaultStats(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);	// 넘어온 메시지 아이디로 플레이어 정보를 받는다.

	if( !pPlayer ) return;													// 플레이어 정보가 유효하지 않으면, return 처리를 한다.

	// 071207 LYW --- MapDBMsgParser : 캐릭터 로그아웃 처리.
	pPlayer->UpdateLogoutToDB();
	PARTYMGR->UserLogOut(pPlayer);
	FriendNotifyLogouttoClient(pPlayer->GetID());
	GUILDMGR->UserLogOut(pPlayer);

	ConfirmUserOut(
		pPlayer->GetAgentNum(),
		pPlayer->GetUserID(),
		pMessage->dwID,
		1);
	// 마지막에 실행되어야 한다
	g_pServerSystem->RemovePlayer(
		pMessage->dwID);
}





// 071129 LYW --- MapDBMsgParser : 스킬 초기화 프로시저 실행 결과 함수.
// 080408 LUJ, 보유 중인 버프 스킬을 해제하도록 수정
void RSkill_Reset(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);

	if(0 == pPlayer)
	{
		return;
	}

	if( pMessage->dwResult == 0 ) return ;

	pPlayer->GetBuffList().SetPositionHead();
	cPtrList templist;

	while(cBuffSkillObject* const pSObj = pPlayer->GetBuffList().GetData())
	{
		if(!pSObj->GetInfo().Die)
		{
			pSObj->SetEndState();
			pSObj->EndState();
			templist.AddTail(pSObj);
		}
	}

	PTRLISTPOS pos = templist.GetHeadPosition();

	while(cSkillObject* const skillObject = (cSkillObject*)templist.GetNext(pos))
	{
		pPlayer->GetBuffList().Remove(
			skillObject->GetSkillIdx());
	}

	templist.RemoveAll();
	pPlayer->GetSkillTree().RemoveAll();

	for(DWORD count = 0 ; count < pMessage->dwResult ; ++count)
	{
		SKILL_BASE skill = {0};
		skill.dwDBIdx = atoi((char*)pData[count].Data[0]);
		skill.wSkillIdx = atoi((char*)pData[count].Data[1]);
		skill.mLearnedLevel = LEVELTYPE( atoi((char*)pData[count].Data[2]) ) ;
		skill.Level = skill.mLearnedLevel;
		pPlayer->GetHeroTotalInfo()->SkillPoint = atoi((char*)pData[count].Data[3]);

		pPlayer->GetSkillTree().Update(
			skill);
	}

	for( BYTE count = 0 ; count < 8 ; ++count )
	{
		for( BYTE count2 = 0 ; count2 < 10 ; ++count2 )
		{
			SLOT_INFO* pSlot = pPlayer->GetQuick( count, count2 ) ;

			if( !pSlot ) continue ;

			if( pSlot->kind == QuickKind_Skill )
			{
				QUICKMNGR->RemoveQuickInfo( pPlayer, count, count2) ;
			}
		}
	}

	pPlayer->SetMoney(
		pPlayer->GetReturnSkillMoney(),
		MONEY_ADDITION,
		MF_NOMAL);
	pPlayer->SetRetrunSkillMoney(
		0);
	pPlayer->SetResetSkill(
		FALSE);
	pPlayer->UpdateLogoutToDB();

	PARTYMGR->UserLogOut(
		pPlayer);
	FriendNotifyLogouttoClient(
		pPlayer->GetID());
	
	ConfirmUserOut(
		pPlayer->GetAgentNum(),
		pPlayer->GetUserID(),
		pMessage->dwID,
		1);
	// 마지막에 실행되어야 한다
	g_pServerSystem->RemovePlayer(
		pMessage->dwID);
}

// 080602 LUJ, 스킬 사용 시 소모 조건이 부합되는지 DB에서 검사하고 값을 바꾼 상태이다.
//				DB에서 성공으로 전달되었다면 서버 메모리만 갱신하자.
void RSkillCheckUsable( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD skillObjectIndex = dbMessage->dwID;
	const MIDDLEQUERYST& record = query[0];
	const DWORD	playerIndex	= atoi((char*)record.Data[0]);
	const DWORD	skillIndex = atoi((char*)record.Data[1]);

	CPlayer* player = (CPlayer*)g_pUserTable->FindUser(
		playerIndex);

	if(0 ==	player ||
		player->GetObjectKind() != eObjectKind_Player)
	{
		return;
	}
	else if(const BOOL isFailed = (0 == skillIndex))
	{
		cActiveSkillObject* const skillObject = (cActiveSkillObject*)SKILLMGR->GetSkillObject(
			skillObjectIndex);

		if(skillObject &&
			skillObject->GetType() == cSkillObject::TypeActive)
		{
			skillObject->RemoveAllBuffSkillObject();

			MSG_DWORD message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category = MP_SKILL;
			message.Protocol = MP_SKILL_CANCEL_SYN;
			message.dwObjectID = player->GetID();
			message.dwData = skillObjectIndex;

			SKILLMGR->NetworkMsgParse(
				message.Protocol,
				&message);
		}

		return;
	}

	const cActiveSkillInfo* const info = SKILLMGR->GetActiveInfo( skillIndex );

	if( ! info )
	{
		return;
	}

	switch( info->GetInfo().mConsumePoint.mType )
	{
	case ACTIVE_SKILL_INFO::ConsumePoint::TypeGuild:
		{
			// 080602 LUJ, 0점을 더하면 새로고침과 동일하다
			GuildAddScore( player->GetGuildIdx(), 0 );

			InsertLogGuildScore(
				player->GetGuildIdx(),
				-1.0f * info->GetInfo().mConsumePoint.mValue,
				eGuildLog_ScoreSkillUse );

			// 080602 LUJ, 길드 포인트 변경은 전 서버에 반영되어야 한다. 그렇지 않으면 클라이언트에서는 
			//				충분한 점수가 있는 것처럼 보여도, 서버에서 사용 불가능한 경우가 생긴다
			{
				MSGBASE message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_SCORE_UPDATE_TO_MAP;
				message.dwObjectID	= player->GetGuildIdx();

				g_Network.Broadcast2AgentServer( ( char* )&message, sizeof( message ) );
			}

			break;
		}
	case ACTIVE_SKILL_INFO::ConsumePoint::TypeFamily:
		{
			MSG_DWORD message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_FAMILY;
			message.Protocol	= MP_FAMILY_HONOR_POINT;
			// 080602 LUJ, 주의! 에이전트에서 int로 캐스팅해서 가감처리를 한다. 개선이 필요하나 서비스 코드라서 건드리지는 않겠음...
			message.dwData		= -1 * info->GetInfo().mConsumePoint.mValue;

			player->SendMsg( &message, sizeof( message ) );

			// 080602 LUJ, 클라이언트측 점수 변경은 에이전트에서 처리한다. 별도로 재전송하지 않는다.
			break;
		}
	}
}

// 080602 LUJ, 패밀리 멤버 목록을 쿼리해서 스킬을 적용한다. 패밀리 정보가 맵 서버에 없고
//				에이전트에 있기 때문에 번거롭지만 DB에서 조회하는 편이 편하다.
//
//				주의: 미들 쿼리이므로 최대 20개밖에 쿼리하지 못한다. 그러나 패밀리 최대 인원이 다섯명이므로 사용한다
void RSkillAddToFamily( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const cActiveSkillInfo* const info = SKILLMGR->GetActiveInfo( dbMessage->dwID );

	if( ! info )
	{
		return;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const MIDDLEQUERYST&	record		= query[ i ];
		const DWORD				playerIndex = atoi( ( char* )record.Data[ 0 ] );
		CPlayer*				player		= ( CPlayer* )g_pUserTable->FindUser( playerIndex );

		if( !	player ||
				player->GetObjectKind() != eObjectKind_Player )
		{
			continue;
		}

		SKILLMGR->AddBuffSkill( *player, info->GetInfo() );
	}
}

void CharacterBuffLoad( DWORD characteridx )
{
	sprintf(txt, "EXEC dbo.MP_CHARACTER_BUFF_LOAD %d", characteridx);
	g_DB.Query(eQueryType_FreeQuery, eCharacterBuffLoad, characteridx, txt, characteridx);
}

// 080819 LUJ, 종료된 기간제 버프 스킬 정보를 읽고, 이를 로그로 남긴다
// 080904 LUJ, 펫의 버프도 이곳으로 들어오므로, 종료 로그 남길 때만 플레이어 체크하도록 수정
void RCharacterBuffLoad(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CObject* object = g_pUserTable->FindUser( pMessage->dwID );

	if( ! object  )
	{
		return;
	}

	for( DWORD count = 0 ; count < pMessage->dwResult ; ++count )	
	{
		DWORD	skillidx	= ( DWORD )atoi( ( char* )pData[ count ].Data[ 0 ] );
		DWORD	remaintime	= ( DWORD )atoi( ( char* )pData[ count ].Data[ 1 ] );
		// 090204 LUJ, 수치 범위를 확장함
		int		remaincount = atoi( ( char* )pData[ count ].Data[ 2 ] );

		// 080219 LUJ, 기간제 스킬의 기간이 긴 경우 오버플로될 수 있으므로 별도로 가져온다
		{
			const char* endDate = ( char* )pData[ count ].Data[ 3 ];

			if( strcmp( "0", endDate ) )
			{
				remaintime	= DWORD( atoi( endDate ) ) * 1000;
			}
			// 080819 LUJ, 기간제 유료 스킬인지 검사하여 로그를 남기고 다음 레코드를 처리한다.
			//				종료된 스킬 삭제는 DB에서 처리하므로 별도로 할 필요없다.
			//				기간제 유료 스킬이 아닐 경우에는 버프가 시작되어야한다.
			else
			{
				const cBuffSkillInfo* const skillInfo = SKILLMGR->GetBuffInfo( skillidx );

				if( !	skillInfo ||
						skillInfo->GetKind() != SKILLKIND_BUFF )
				{
					continue;
				}
				else if(skillInfo->GetInfo().IsCash	&&
					skillInfo->GetInfo().IsEndTime &&
					object->GetObjectKind() != eObjectKind_Player )
				{
					SKILL_BASE skill = { 0 };
					skill.wSkillIdx	= skillidx;

					InsertLogSkill(
						( CPlayer* )object,
						&skill,
						eLog_SkillCashRemove );

					continue;
				}
			}
		}

		SKILLMGR->BuffSkillStart( pMessage->dwID, skillidx, remaintime, remaincount );
	}
}

// 090204 LUJ, 수치 범위를 확장함
void CharacterBuffAdd( DWORD characteridx, DWORD skillidx, DWORD remaintime, int count, BOOL isEndDate )
{
	// 패밀리보상버프는 DB에 저장하지 않는다.
	if(g_csFamilyManager.IsFamilyRewardBuff(skillidx))
		return;

	sprintf(txt, "EXEC dbo.MP_CHARACTER_BUFF_ADD %d, %d, %u, %d, %d", characteridx, skillidx, remaintime, count, isEndDate );
	g_DB.Query(eQueryType_FreeQuery, eCharacterBuffAdd, characteridx, txt, characteridx);
}

// 090204 LUJ, 수치 범위를 확장함
void CharacterBuffUpdate( DWORD characteridx, DWORD skillidx, DWORD remaintime, int count  )
{
	// 패밀리보상버프는 DB에 저장하지 않는다.
	if(g_csFamilyManager.IsFamilyRewardBuff(skillidx))
		return;

	sprintf(txt, "EXEC dbo.MP_CHARACTER_BUFF_UPDATE %d, %d, %u, %d, %d", characteridx, skillidx, remaintime, count );
	g_DB.Query(eQueryType_FreeQuery, eCharacterBuffUpdate, characteridx, txt, characteridx);
}

void CharacterBuffRemove( DWORD characteridx, DWORD skillidx )
{
	sprintf(txt, "EXEC dbo.MP_CHARACTER_BUFF_REMOVE %d, %d", characteridx, skillidx);
	g_DB.Query(eQueryType_FreeQuery, eCharacterBuffRemove, characteridx, txt, characteridx);
}

void ConnectionCheck()
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(txt, "EXEC dbo.MP_CONNECTIONCHECK");
	g_DB.Query(eQueryType_FreeQuery, eConnectionCheck, 0, txt);
}

void ChallengeZone_EnterFreq_Save(DWORD nPlayerID, DWORD nEnterFreq, DWORD nEnterBonusFreq)
{
	sprintf(txt, "EXEC  %s %d, %d, %d", STORED_CHALLENGEZONE_ENTERFREQ_SAVE_INFO, nPlayerID, nEnterFreq, nEnterBonusFreq);
	g_DB.Query(eQueryType_FreeQuery, eChallengeZone_EnterFreq_SetInfo, nPlayerID, txt, nPlayerID);
}

void ChallengeZone_EnterFreq_Load(DWORD nPlayerID)
{
	sprintf(txt, "EXEC  %s %d", STORED_CHALLENGEZONE_ENTERFREQ_LOAD_INFO, nPlayerID);
	g_DB.Query(eQueryType_FreeQuery, eChallengeZone_EnterFreq_LoadInfo, nPlayerID, txt, nPlayerID);
}

void RChallengeZone_EnterFreq_Load(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer ) return;

	// 챌린지 존 맵에 입장할 때 입장 회수를 증가시킨다. 그런데 DB로 부터의 로드가 그 이후에 이루어지기 때문에
	// 입장 회수가 증가됐는지 체크해서 증가된 만큼 + 설정하고, 다시 DB에 저장한다.
	UINT nAleadyIncreaseFreq = pPlayer->GetChallengeZoneEnterFreq();

	if( pMessage->dwResult == 0 )
	{
	}
	else
	{
		// DB에 저장되어 있던 값을 설정
		pPlayer->SetChallengeZoneEnterFreq(atoi((char*)pData[0].Data[0]));
		pPlayer->SetChallengeZoneEnterBonusFreq(atoi((char*)pData[0].Data[1]));
		// 챌린지 존에 입장했나?
		if (nAleadyIncreaseFreq)
		{
			// 하루 이용 가능 회수를 초과했는지 체크.
			if (nAleadyIncreaseFreq + pPlayer->GetChallengeZoneEnterFreq() > CSHDateManager::ENTER_CHALLENGE_ZONE_FREQ_PER_DAY)
			{
				// 초과했다면 보너스 회수가 있다는 뜻이다. 보너스 회수를 -1 한다.
				// ..정말 Bonus 회수가 있는지 확인 체크
				if (pPlayer->GetChallengeZoneEnterBonusFreq())
				{
					pPlayer->SetChallengeZoneEnterBonusFreq(pPlayer->GetChallengeZoneEnterBonusFreq()-1);
				}
				else
				{
					// Bonus 회수가 없다? 뭔가 잘못됐지만 일단 그냥 입장시킴.
				}
			}
			else
			{
				// 하루 이용 가능 회수를 초과하지 않았으니까 회수 증가
				pPlayer->IncreaseChallengeZoneEnterFreq();
			}
		}
	}

	// 챌린지 존에 입장을 했다면 위의 루틴에 의해서 입장 가능 회수나, 보너스 회수가 변경되었기에 이를 DB에 저장.
	// ..다른 맵에서 챌린지 존에 입장을 할 때 처리하지 않고, 챌린지 존에 입장하는 순간에 처리하기 때문에
	// ..일반적인 DB Load 와는 조금 다름. 
	// ..다른 맵에서 챌린지 존에 입장할 때 회수 값을 저장시키면 이렇게 처리하지 않아도 되지만, 어떤 문제로 인하여
	// ..챌린지 존에 접속하지 못했을 경우에도 회수값이 설정되기 때문에 실제로 챌린지 존에 접속했음을 확인하기 위해 이렇게 처리했음.
	if (nAleadyIncreaseFreq)
	{
		ChallengeZone_EnterFreq_Save(pPlayer->GetID(), pPlayer->GetChallengeZoneEnterFreq(), pPlayer->GetChallengeZoneEnterBonusFreq());
	}

	pPlayer->SetInitState( PLAYERINITSTATE_CHALLENGEZONE_INFO, MP_USERCONN_GAMEIN_SYN );
}

void ChallengeZone_Success_Save(DWORD nPlayerID, DWORD nPartnerID, UINT nSection, UINT nClearTime)
{
	sprintf(txt, "EXEC  %s %d, %d, %d, %d", STORED_CHALLENGEZONE_SUCCESS_SAVE_INFO, nPlayerID, nPartnerID, nSection, nClearTime);
	g_DB.Query(eQueryType_FreeQuery, eChallengeZone_Success_SetInfo, nPlayerID, txt, nPlayerID);
}

// 091124 ONS 챌린지존 경험치분배 저장/로드 처리추가
void ChallengeZone_ExpRate_Save(DWORD nPlayerID, DWORD dwExpRate)
{
	sprintf(txt, "EXEC  %s %d, %d", STORED_CHALLENGEZONE_EXP_RATE_SAVE, nPlayerID, dwExpRate);
	g_DB.Query(eQueryType_FreeQuery, eChallengeZone_ExpRate_Save, 0, txt, 0);
}

void ChallengeZone_ExpRate_Load(DWORD nPlayerID, DWORD nPartnerID, UINT nSection)
{
	sprintf(txt, "EXEC  %s %d, %d, %d", STORED_CHALLENGEZONE_EXP_RATE_LOAD, nPlayerID, nPartnerID, nSection);
	g_DB.Query(eQueryType_FreeQuery, eChallengeZone_ExpRate_Load, nPlayerID, txt, nPlayerID);
}

void RChallengeZone_ExpRate_Load(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( !pPlayer )
	{
		return;
	}

	if( 0 == pMessage->dwResult )
	{
		return;
	}
	
	char PlayerName[MAX_NAME_LENGTH+1]="";
	char PartnerName[MAX_NAME_LENGTH+1]="";
	strcpy(PlayerName, (char*)pData->Data[0]);
	strcpy(PartnerName, (char*)pData->Data[1]);
	DWORD dwPlyaerExpRate	= atoi((char*)pData[0].Data[2]);
	DWORD dwPartnerExpRate	= atoi((char*)pData[0].Data[3]);
	UINT  nSection			= atoi((char*)pData[0].Data[4]);

	// 클라이언트로 파티원 정보 전달
	g_csDateManager.SRV_NotifyStartChallengeZone(pPlayer, PlayerName, PartnerName, dwPlyaerExpRate, dwPartnerExpRate, nSection);
	// 해당 플레이어의 경험치습득비율 설정
	pPlayer->SetChallengeZoneExpRate(dwPlyaerExpRate);
}
// E 데이트 존 추가 added by hseos 2007.11.26	2007.11.27	2007.12.01	2008.01.23

// 071210 KTH -- 케릭터의 인벤토리를 확장한다.
void IncreaseCharacterInventory( DWORD characteridx )
{
	sprintf(txt, "EXEC %s %d", STORED_INCREASE_CHARACTER_INVENTORY, characteridx);
	g_DB.Query(eQueryType_FreeQuery, eIncreaseCharacterInventory, characteridx, txt, characteridx);
}

void RIncreaseCharacterInventory(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);

	if( !pPlayer ) return;

	if( pMessage->dwResult != 0 )
	{
		if( pPlayer->GetInventoryExpansion() >= 2 )
			return;

		pPlayer->SetInventoryExpansion(pPlayer->GetInventoryExpansion() + 1);

		MSG_DWORD msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_INCREASE_INVENTORY_ACK;
		// 071220 LYW --- MapDBMsgParser : 오브젝트 아이디 세팅.
		msg.dwObjectID = pPlayer->GetID() ;
		msg.dwData = (DWORD)pPlayer->GetInventoryExpansion();

		pPlayer->SendMsg(&msg, sizeof(msg));
	}
}

// 071212 KTH -- 확장 인벤토리의 정보를 초기화 하여준다.
void ResetCharacterInventory( DWORD characteridx )
{
	sprintf(txt, "EXEC %s %d", STORED_RESET_CHARACTER_INVENTORY, characteridx);
	g_DB.Query(eQueryType_FreeQuery, eResetCharacterInventory, characteridx, txt, characteridx);
}

// 071224 KTH -- 케릭터 이름 변경
void ChangeCharacterName( char* szName, int characterIdx, DWORD itemIdx, POSTYPE pos)
{
	g_DB.FreeQuery(
		eChangeCharacterName,
		characterIdx,
		"EXEC dbo.MP_CHARACTER_CHANGE_NAME \'%s\', %d, %d, %d",
		szName,
		characterIdx,
		itemIdx,
		pos);
}
// 071226 KTH -- 케릭터의 이름 변경 결과를 가져온다. 아이템의 사용 여부도 여기서 판단한다.
void RChangeCharacterName(LPQUERY pData, LPDBMESSAGE pMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);

	char text[ MAX_NAME_LENGTH + 1 ] = { 0 };

	if( !pPlayer )
		return;

	strcpy( text, pPlayer->GetObjectName() );

	if( atoi((char*)pData->Data[0]) == 0 )
	{
		char changeName[17];
		strcpy(changeName, (char*)pData->Data[1]);
		// 케릭터의 이름을 바꿔준다.
		pPlayer->SetCharacterName(changeName);

		DWORD itemIdx	=	atoi((char*)pData->Data[2]);
		POSTYPE	pos		=	(POSTYPE)atoi((char*)pData->Data[3]);

		ITEMBASE previousItem;
		ZeroMemory(
			&previousItem,
			sizeof(previousItem));
		const ITEMBASE* item = ITEMMGR->GetItemInfoAbsIn( pPlayer, pos );

		if( item )
		{
			previousItem = *item;
		}

		if( ITEMMGR->UseItem(pPlayer, pos, itemIdx) != eItemUseSuccess)
		{
			MSG_DWORD msg;
			msg.Category = MP_ITEM;
			msg.Protocol = MP_ITEM_CHANGE_CHARACTER_NAME_NACK;
			msg.dwData = 0; // 0은 아이템 사용이 실패 하였을때 보낸다.
			pPlayer->SendMsg(&msg, sizeof(msg));
			return;
		}

		MSG_CHANGE_CHARACTER_NAME_ACK	msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_CHANGE_CHARACTER_NAME_ACK;
		//msg.dwObjectID = pPlayer->GetID();
		msg.TargetPos = pos;
		msg.wItemIdx = itemIdx;
		strcpy(msg.changeName, pPlayer->GetObjectName());
		pPlayer->SendMsg(&msg, sizeof(msg));

		{
			LogItemMoney(
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				0,
				text,
				eLog_ItemUse,
				pPlayer->GetMoney(),
				0,
				0,
				previousItem.wIconIdx,
				previousItem.dwDBIdx,
				previousItem.Position,
				0,
				previousItem.Durability - 1,
				0 );
		}
		{
			// 0801
			LogChangeName(pPlayer->GetID(), text, pPlayer->GetObjectName());
		}

		// 080121 KTH -- 케릭터 이름 변경을 에이전트에 알려준다.
		// charIDX, userIDX, characterName, ServerNum <- 요건 에이전트에서 처리 ㅎ;;
		MSG_CHANGE_CHARACTER_NAME_AGENT msg2;
		msg2.Category = MP_ITEM;
		msg2.Protocol = MP_ITEM_CHANGE_CHARACTER_NAME_SYN;
		msg2.dwObjectID = pPlayer->GetID();
		strcpy(msg2.changeName, changeName);
		msg2.CharacterIdx = pPlayer->GetID();
		msg2.UserIdx = pPlayer->GetUserID();

		pPlayer->SendMsg(&msg2, sizeof(MSG_CHANGE_CHARACTER_NAME_AGENT));

		// 071231 LYW --- MapDBMsgParser : 캐릭터 로그아웃 처리.
		pPlayer->UpdateLogoutToDB();
		FriendNotifyLogouttoClient(pPlayer->GetID());

		ConfirmUserOut(
			pPlayer->GetAgentNum(),
			pPlayer->GetUserID(),
			pMessage->dwID,
			1);
		// 마지막에 실행되어야 한다
		g_pServerSystem->RemovePlayer(
			pMessage->dwID);
	}
	else
	{
		MSG_DWORD msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_CHANGE_CHARACTER_NAME_NACK;
		msg.dwData = atoi((char*)pData->Data[0]);
		pPlayer->SendMsg(&msg, sizeof(msg));
		return;
	}
}

void AutoNoteListAdd( DWORD dwCharacterIdx, char* pStrAutoName, DWORD dwAutoCharacterIdx, DWORD dwAutoUserIdx )
{
	sprintf( txt, "EXEC dbo.MP_AUTONOTELIST_ADD %u, '%s', %u, %u", dwCharacterIdx, pStrAutoName, dwAutoCharacterIdx, dwAutoUserIdx );
	g_DB.Query( eQueryType_FreeQuery, eAutoNoteList_Add, dwCharacterIdx, txt );	

	// 090609 ShinJS --- Add AutoNote Log
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( dwCharacterIdx );
	if( !pPlayer || pPlayer->GetObjectKind() != eObjectKind_Player )
		return;

	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_AUTONOTELOG %d, %d, \'%s\', %d, \'%s\'", 
		eAutoNoteLogKind_Regist, 
		dwCharacterIdx,
		pPlayer->GetObjectName(),
		dwAutoCharacterIdx,
		pStrAutoName );
}

void RAutoNoteListAdd( LPQUERY pData, LPDBMESSAGE pMessage )
{
	if( pMessage->dwResult == 0 )
		return;

	MSG_AUTOLIST_ADD msg;
	msg.Category	= MP_AUTONOTE;
	msg.Protocol	= MP_AUTONOTE_LIST_ADD;
	msg.dwObjectID	= pMessage->dwID;

	SafeStrCpy( msg.Name, (char*)pData[0].Data[0], MAX_NAME_LENGTH+1 );
	SafeStrCpy( msg.Date, (char*)pData[0].Data[1]+2, 20 ); //+2는 2008을 08로 표시하기 위해

	g_Network.Broadcast2AgentServer( (char*)&msg, sizeof(msg) );	//에이젼트로 브로드캐스팅
}

void AutoNoteListLoad( DWORD dwCharacterIdx  )
{
	sprintf( txt, "EXEC dbo.MP_AUTONOTELIST_LOAD %u", dwCharacterIdx );
	g_DB.Query( eQueryType_FreeQuery, eAutoNoteList_Load, dwCharacterIdx, txt, dwCharacterIdx );	
}

void RAutoNoteListLoad( LPQUERY pData, LPDBMESSAGE pMessage )
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( pPlayer == NULL ) return;

	int nCount = pMessage->dwResult;
	if( nCount == 0 ) return;

	MSG_AUTOLIST_ALL msg;
	msg.Category	= MP_AUTONOTE;
	msg.Protocol	= MP_AUTONOTE_LIST_ALL;
	msg.dwObjectID	= pMessage->dwID;
	msg.nCount		= nCount;

	for( int i = 0 ; i < nCount ; ++i )
	{
		SafeStrCpy( msg.row[i].Name, (char*)pData[i].Data[0], MAX_NAME_LENGTH+1 );
		SafeStrCpy( msg.row[i].Date, (char*)pData[i].Data[1]+2, 20 ); //+2는 2008을 08로 표시하기 위해
	}

	pPlayer->SendMsg( &msg, msg.GetMsgLength() );
}
//--------------
// 080204 KTH -- 이벤트 관련 쿼리
void UpdateEventFromGame( DWORD dwCharacterIdx, char* pCharacterName )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC dbo.UP_EVENT_FROM_GAME_INSERT %d, '%s', %d", dwCharacterIdx, pCharacterName, g_nServerSetNum );
	g_DB.LoginQuery(eQueryType_FreeQuery, eJoinEvent, dwCharacterIdx, txt);
}

void GiftItemInsert( DWORD characterIdx, DWORD useridx, DWORD eventIdx, DWORD itemIdx, DWORD count, BOOL isStack )
{
	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d", "MP_GIFT_ITEM_INSERT", useridx, eventIdx, itemIdx, count, isStack );
	g_DB.Query(eQueryType_FreeQuery, eGiftItemInsert, characterIdx, txt);

	// 080716 LUJ, 로그. 숫자 4는 복구툴에 정의된 eItemShopLog의 enum 상수값
	g_DB.LogMiddleQuery(
		0,
		0,
		"INSERT TB_ITEM_SHOP_LOG ( TYPE, USER_IDX, USER_ID, ITEM_IDX, ITEM_DBIDX, SIZE ) VALUES ( 4, %d, '', %d, 0, %d )",
		useridx,
		itemIdx,
		count );
}

void RGiftItemInsert( LPQUERY pData, LPDBMESSAGE pMessage )
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pMessage->dwID);
	if( pPlayer == NULL ) return;

	DWORD eventidx = atoi((char*)pData[0].Data[0]);

	MSG_DWORD2 msg;
	msg.Category = MP_ITEM;
	msg.Protocol = MP_ITEM_GIFT_EVENT_NOTIFY;
	msg.dwData1 = eventidx;
	msg.dwData2 = 2;

	pPlayer->SendMsg( &msg, sizeof( msg ) );	
}

void FishingData_Load(DWORD dwPlayerID)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RFishingData_Load,
		dwPlayerID,
		"EXEC dbo.MP_FISHINGDATA_LOAD %d",
		dwPlayerID );
}

void RFishingData_Load(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD playerIndex = dbMessage->dwID;
	CPlayer*	pPlayer		= (CPlayer*)g_pUserTable->FindUser( playerIndex );

	if( !	pPlayer ||
			dbMessage->dwResult != 1 )
	{
		ASSERT( 1 == dbMessage->dwResult );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	WORD wLevel = WORD(atoi((char*)record.Data[0]));

	// 080605 LYW --- MapDBMsgParser : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
	//DWORD dwExp = DWORD(atoi((char*)record.Data[1]));
	DWORDEX dwExp = _atoi64((char*)record.Data[1]);

	DWORD dwPoint = DWORD(atoi((char*)record.Data[2]));

	pPlayer->SetFishingLevel(wLevel);
	pPlayer->SetFishingExp(dwExp);
	pPlayer->SetFishPoint(dwPoint);

}

// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
//void FishingData_Update(DWORD dwPlayerID, WORD wFishingLevel, DWORD dwFishingExp, DWORD dwFishPoint)
void FishingData_Update(DWORD dwPlayerID, WORD wFishingLevel, EXPTYPE dwFishingExp, DWORD dwFishPoint)
{
	// 090805 pdy 낚시포인트 초기화 버그 수정 모든 수치가 초기값 이라면 업데이트 하지 말자. 
	if( wFishingLevel == 1 && dwFishingExp == 0 && dwFishPoint == 0 )
		return ;

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		NULL,
		dwPlayerID,
		"EXEC  dbo.MP_FISHINGDATA_UPDATE %d, %d, %0.f, %u",
		dwPlayerID, wFishingLevel, (float)dwFishingExp, dwFishPoint);
}

// 080808 LUJ, 낚시 경험치/레벨 추가, 더 이상 낚시 포인트만 저장하는 로그가 아니므로 함수명을 변경
void Log_Fishing(DWORD dwPlayerID, CHAR cLogType, DWORD dwDiffPoint, DWORD dwTotalPoint, DWORD dwBuyItemIdx, DWORD dwUseFish, EXPTYPE dwExp, DWORD dwLevel )
{
	// 080808 LUJ, 낚시 경험치/레벨 추가
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_FISHING_LOG_INSERT %d, %d, %u, %u, %d, %d, %0.0f, %d",
		dwPlayerID,
		cLogType,
		dwDiffPoint,
		dwTotalPoint,
		dwBuyItemIdx,
		dwUseFish,
		float( dwExp ),
		dwLevel );
}

void CookingData_Load(DWORD dwPlayerID)
{
	g_DB.FreeMiddleQuery(
		RCookingData_Load,
		dwPlayerID,
		"EXEC dbo.MP_COOKING_LOAD %d",
		dwPlayerID );
}

void RCookingData_Load(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD playerIndex = dbMessage->dwID;
	CPlayer*	pPlayer		= (CPlayer*)g_pUserTable->FindUser( playerIndex );

	if( !	pPlayer ||
			dbMessage->dwResult != 1 )
	{
		ASSERT( 1 == dbMessage->dwResult );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	WORD wLevel = WORD(atoi((char*)record.Data[0]));
	WORD wCookCount = WORD(atoi((char*)record.Data[1]));
	WORD wEatCount = WORD(atoi((char*)record.Data[2]));
	WORD wFireCount = WORD(atoi((char*)record.Data[3]));

	pPlayer->SetCookLevel(wLevel);
	pPlayer->SetCookCount(wCookCount);
	pPlayer->SetEatCount(wEatCount);
	pPlayer->SetFireCount(wFireCount);
}

void Cooking_Update(CPlayer* pPlayer) 
{
	if(!pPlayer)	return;

	DWORD wCookLevel, wCookCount, wEatCount, wFireCount;

	wCookLevel = pPlayer->GetCookLevel();
	wCookCount = pPlayer->GetCookCount();
	wEatCount = pPlayer->GetEatCount();
	wFireCount = pPlayer->GetFireCount();

	if(wCookLevel<=1 &&
		wCookCount == 0 &&
		wEatCount == 0 &&
		wFireCount == 0)
		return;

	g_DB.FreeMiddleQuery(
		NULL,
		pPlayer->GetID(),
		"EXEC dbo.MP_COOKING_UPDATE %d, %d, %d, %d, %d",
		pPlayer->GetID(), wCookLevel, wCookCount, wEatCount, wFireCount);
}

void Cooking_Recipe_Load(DWORD dwPlayerID)
{
	g_DB.FreeMiddleQuery(
		RCooking_Recipe_Load,
		dwPlayerID,
		"EXEC dbo.MP_COOKRECIPE_LOAD %d",
		dwPlayerID );
}

void RCooking_Recipe_Load(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD playerIndex = dbMessage->dwID;
	CPlayer*	pPlayer		= (CPlayer*)g_pUserTable->FindUser( playerIndex );

	if(!pPlayer)		return;
	int nCount = dbMessage->dwResult;

	int i;
	for(i=0; i<nCount; i++)
	{
		const MIDDLEQUERYST& record = query[ 0 ];
		DWORD dwRecipeIdx	= DWORD(atoi((char*)record.Data[1]));
		DWORD dwSlot		= DWORD(atoi((char*)record.Data[2]));
		DWORD dwReaminTime	= DWORD(atoi((char*)record.Data[3]));

		pPlayer->SetMasterRecipe((POSTYPE)dwSlot, dwRecipeIdx, dwReaminTime);
	}
}

void Cooking_Recipe_Update(DWORD dwPlayerID, int nKind, DWORD dwRecipe, POSTYPE pos, DWORD dwRemainTime)
{
	g_DB.FreeMiddleQuery(
		NULL,
		dwPlayerID,
		"EXEC dbo.MP_COOKRECIPE_UPDATE %d, %d, %d, %d, %d",
		dwPlayerID, nKind, dwRecipe, pos, dwRemainTime);
}

void Cooking_Log(DWORD dwPlayerID, int nKind, DWORD dwItemIdx, DWORD dwBeforeValue, DWORD dwDiffValue, DWORD dwAfterValue)
{
	g_DB.LogMiddleQuery(
		NULL,
		dwPlayerID,
		"EXEC dbo.TP_COOKING_LOG %d, %d, %d, %d, %d, %d",
		dwPlayerID, nKind, dwItemIdx, dwBeforeValue, dwDiffValue, dwAfterValue);
}

void CookRecipe_Log(DWORD dwPlayerID, DWORD dwRecipeIdx, POSTYPE slot, int nKind, DWORD dwRemainTime)
{
	g_DB.LogMiddleQuery(
		NULL,
		dwPlayerID,
		"EXEC dbo.TP_COOKRECIPE_LOG %d, %d, %d, %d, %d",
		dwPlayerID, dwRecipeIdx, slot, nKind, dwRemainTime);
}

void HouseData_Load(DWORD dwPlayerID)
{
	g_DB.FreeMiddleQuery(
		RHouseData_Load,
		dwPlayerID,
		"EXEC dbo.MP_HOUSEDATA_LOAD %d",
		dwPlayerID );
}

void RHouseData_Load(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD playerIndex = dbMessage->dwID;
	CPlayer*	pPlayer		= (CPlayer*)g_pUserTable->FindUser( playerIndex );

	if( !	pPlayer ||
			dbMessage->dwResult != 1 )
	{
		ASSERT( 1 == dbMessage->dwResult );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	char* pHouseName = (char*)record.Data[0];
	pPlayer->SetHouseName(pHouseName);
}

void HouseCreate(DWORD dwConnectionIndex, DWORD dwPlayerMapNum, DWORD dwPlayerID, char* pCreateName)
{
	if(IsCharInString(pCreateName, "'"))
	{
 		return;
	}

	DWORD dwDecoUsePoint = HOUSINGMGR->GetDecoUsePoint();

	g_DB.FreeMiddleQuery(
		RHouseCreate,
		dwPlayerID,
		"EXEC dbo.MP_HOUSECREATE %d, %d, %d, \'%s\', %d",
		dwConnectionIndex, dwPlayerMapNum, dwPlayerID, pCreateName, dwDecoUsePoint);
}

void RHouseCreate(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD playerIndex = dbMessage->dwID;

	if(1 != dbMessage->dwResult)
	{
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];
	DWORD dwResult = DWORD(atoi((char*)record.Data[0]));
	DWORD dwPlayerMapNum = DWORD(atoi((char*)record.Data[2]));

	if(eHOUSEERR_NOOWNER == dwResult)
		return;
	else if(eHOUSEERR_HAVEHOUSE == dwResult)
	{
		// 이미보유 통보
		MSG_DWORD msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_CREATE_FROM_SERVER_NACK;
		msg.dwObjectID = playerIndex;
		msg.dwData = eHOUSEERR_HAVEHOUSE;
		PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
		return;
	}

	DWORD dwOwnerUserIndex = DWORD(atoi((char*)record.Data[3]));
	char* pHouseName = (char*)record.Data[4];

	// 초기오브젝트 추가
	DWORD dwCategorySlot[MAX_HOUSING_CATEGORY_NUM] = {0,};

	stHousingSettingInfo* pSettingInfo = HOUSINGMGR->GetHousingSettingInfo();
	if(pSettingInfo)
	{		
		DWORD i;
		for(i=0; i<pSettingInfo->dwGeneralFunitureNumArr[eHOUSEEXTEND_NONE]; i++)
		{
			stGeneralFunitureInfo* pInitFurnitureInfo = &pSettingInfo->pGeneralFunitureList[eHOUSEEXTEND_NONE][i];

			if(!pInitFurnitureInfo)
				continue;

			DWORD dwFurnitureIndex = HOUSINGMGR->GetFurnitureIndexFromItemIndex(pInitFurnitureInfo->dwItemIndex);
			DWORD dwHiCategory = Get_HighCategory(dwFurnitureIndex);

			// DB에 처음목록추가
			DWORD dwEndTime = HOUSINGMGR->GetRemainTimeFromItemIndex(pInitFurnitureInfo->dwItemIndex); // 50 * 365 * 24 * 60 * 60;		//기본설치물은 50년간 유지 
			HouseFurnitureUpdate(playerIndex, eHOUSEFURNITURE_ADD, dwOwnerUserIndex, dwFurnitureIndex, pInitFurnitureInfo->dwItemIndex, 0, 
				pInitFurnitureInfo->vWorldPos.x, pInitFurnitureInfo->vWorldPos.y, pInitFurnitureInfo->vWorldPos.z, pInitFurnitureInfo->fAngle,
				(BYTE)dwCategorySlot[dwHiCategory], eHOUSEFURNITURE_STATE_INSTALL, dwEndTime, TRUE);

			dwCategorySlot[dwHiCategory]++;
		}
	}

	// 생성 통보
	// DB에 생성만 했음.
	// 유저가 하우스 이동시 하우스매니저에 생성해야 함.
	MSG_HOUSENAME msgHouseName;
	msgHouseName.Category = MP_HOUSE;
	msgHouseName.Protocol = MP_HOUSE_CREATE_FROM_SERVER_ACK;
	strcpy(msgHouseName.Name, pHouseName);
	msgHouseName.dwObjectID = playerIndex;
	msgHouseName.dwData = dwOwnerUserIndex;
	PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msgHouseName, sizeof(msgHouseName));
}

void HouseUpdate(stHouseInfo* pHouseInfo)
{
	if(IsCharInString(pHouseInfo->szHouseName, "'"))
	{
 		return;
	}

	g_DB.FreeMiddleQuery(
		NULL,
		0,
		"EXEC dbo.MP_HOUSEUPDATE %d, %d, %d, %d, %d, %d, %d, \'%s\'",
		pHouseInfo->dwOwnerUserIndex, pHouseInfo->ExteriorKind, 
		pHouseInfo->ExtendLevel, pHouseInfo->dwTotalVisitCount, pHouseInfo->dwDailyVisitCount, pHouseInfo->dwDecoUsePoint, pHouseInfo->dwHousePoint,
		pHouseInfo->szHouseName);
}

void HouseEntrance(DWORD dwConnectionIndex, DWORD dwPlayerMapNum, CHAR cKind, DWORD dwValue1, DWORD dwValue2, DWORD dwPlayerID, char* pOwnerName)
{
	char szHouseOwnerName[MAX_HOUSING_NAME_LENGTH + 1] = {0,};
	SafeStrCpy(szHouseOwnerName, pOwnerName, MAX_HOUSING_NAME_LENGTH+1);

	if(IsCharInString(szHouseOwnerName, "'"))
	{
 		return;
	}

	g_DB.FreeMiddleQuery(
		RHouseEntrance,
		dwPlayerID,
		"EXEC dbo.MP_HOUSEENTRANCE %d, %d, %d, %d, %d, %d, '%s'", dwConnectionIndex, dwPlayerMapNum, cKind, dwValue1, dwValue2, dwPlayerID, szHouseOwnerName);
}

void RHouseEntrance(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD playerIndex = dbMessage->dwID;

	if(1 != dbMessage->dwResult)
	{
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];
	DWORD dwResult = DWORD(atoi((char*)record.Data[0]));
	DWORD dwConnectionIndex = DWORD(atoi((char*)record.Data[1]));
	DWORD dwPlayerMapNum = DWORD(atoi((char*)record.Data[2]));
	CHAR  cKind = CHAR(atoi((char*)record.Data[3]));
	DWORD dwValue1 = DWORD(atoi((char*)record.Data[4]));
	DWORD dwValue2 = DWORD(atoi((char*)record.Data[5]));
	DWORD dwOwnerUserIndex = DWORD(atoi((char*)record.Data[7]));

	if(eHOUSEERR_NOOWNER == dwResult)
	{
		return;
	}
	else if(eHOUSEERR_NOHOUSE == dwResult)
	{
		// 하우스 미존재 통보
		MSG_DWORD4 msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_ENTRANCE_FROM_SERVER_NACK;
		msg.dwObjectID = playerIndex;
		msg.dwData1 = eHOUSEERR_NOHOUSE;
		msg.dwData2 = cKind;
		msg.dwData3 = dwValue1;
		msg.dwData4 = dwValue2;
		PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
		return;
	}

	if(HOUSINGMGR->IsLoadingHouse(dwOwnerUserIndex))
	{
		// 하우스 로딩중 통보
		MSG_DWORD4 msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_ENTRANCE_FROM_SERVER_NACK;
		msg.dwObjectID = playerIndex;
		msg.dwData1 = eHOUSEERR_LOADING;
		msg.dwData2 = cKind;
		msg.dwData3 = dwValue1;
		msg.dwData4 = dwValue2;
		PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
		return;
	}

	if(0 != HOUSINGMGR->GetChannelIDFromReservationList(playerIndex))
	{
		// 하우스 예약중 통보
		MSG_DWORD4 msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_ENTRANCE_FROM_SERVER_NACK;
		msg.dwObjectID = playerIndex;
		msg.dwData1 = eHOUSEERR_RESERVATING;
		msg.dwData2 = cKind;
		msg.dwData3 = dwValue1;
		msg.dwData4 = dwValue2;
		PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
		return;
	}

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(playerIndex);
	if(pPlayer)
	{
		stHouse* pHome = HOUSINGMGR->GetHouseByUserIndex(dwOwnerUserIndex);
		if(pHome && pHome->dwChannelID == pPlayer->GetChannelID())
		{
			// 홈->홈 이동하는 경우 실패처리
			MSG_DWORD4 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ENTRANCE_FROM_SERVER_NACK;
			msg.dwObjectID = playerIndex;
			msg.dwData1 = eHOUSEERR_HOME2HOME_FAIL;
			msg.dwData2 = cKind;
			msg.dwData3 = dwValue1;
			msg.dwData4 = dwValue2;
			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
			return;
		}
	}

	stHouse* pHouse = NULL;
	if(!HOUSINGMGR->GetHouseByUserIndex(dwOwnerUserIndex))
	{
		pHouse = HOUSINGMGR->CreateNewHouse();
		if(!pHouse)
		{
			MSG_DWORD4 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ENTRANCE_FROM_SERVER_NACK;
			msg.dwObjectID = playerIndex;
			msg.dwData1 = eHOUSEERR_ENTRANCEFAIL;
			msg.dwData2 = cKind;
			msg.dwData3 = dwValue1;
			msg.dwData4 = dwValue2;
			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
			return;
		}

		pHouse->HouseInfo.dwOwnerUserIndex = dwOwnerUserIndex;
		char* pHouseName = (char*)record.Data[8];
		strcpy(pHouse->HouseInfo.szHouseName, pHouseName);
		pHouse->HouseInfo.ExteriorKind = BYTE(atoi((char*)record.Data[9]));
		pHouse->HouseInfo.ExtendLevel = BYTE(atoi((char*)record.Data[10]));
		pHouse->HouseInfo.dwTotalVisitCount = DWORD(atoi((char*)record.Data[11]));
		pHouse->HouseInfo.dwDailyVisitCount = DWORD(atoi((char*)record.Data[12]));
		pHouse->HouseInfo.dwDecoUsePoint = DWORD(atoi((char*)record.Data[13]));
		pHouse->HouseInfo.dwHousePoint = DWORD(atoi((char*)record.Data[14]));
		int nDiffDay = DWORD(atoi((char*)record.Data[15]));

		if(nDiffDay < 0)
		{
			pHouse->HouseInfo.dwDailyVisitCount = 0;
			pHouse->HouseInfo.dwDecoUsePoint = HOUSINGMGR->GetDecoUsePoint();
			HouseUpdate(&pHouse->HouseInfo);
		}

		// 하우스메니저에 추가
		if(!HOUSINGMGR->CreateHouse(pHouse))
		{
			HOUSINGMGR->RemoveHouseFromPool(pHouse);

			MSG_DWORD4 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ENTRANCE_FROM_SERVER_NACK;
			msg.dwObjectID = playerIndex;
			msg.dwData1 = eHOUSEERR_FULL;
			msg.dwData2 = cKind;
			msg.dwData3 = dwValue1;
			msg.dwData4 = dwValue2;
			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
			return;
		}

		// 하우스 가구목록 DB에서 받아오기
		HouseFurnitureLoad(pHouse->HouseInfo.dwOwnerUserIndex, 0, dwConnectionIndex, dwPlayerMapNum, cKind, dwValue1, dwValue2, playerIndex);
	}
	else
	{
		pHouse = HOUSINGMGR->GetHouseByUserIndex(dwOwnerUserIndex);
		if(!pHouse)
		{
			// 하우스 없음
			MSG_DWORD4 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ENTRANCE_FROM_SERVER_NACK;
			msg.dwObjectID = playerIndex;
			msg.dwData1 = eHOUSEERR_ENTRANCEFAIL;
			msg.dwData2 = cKind;
			msg.dwData3 = dwValue1;
			msg.dwData4 = dwValue2;
			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
			return;
		}

		if(HOUSINGMGR->IsLoadingHouse(dwOwnerUserIndex))
		{
			// DB에서 데이터로딩중 입장실패
			MSG_DWORD4 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ENTRANCE_FROM_SERVER_NACK;
			msg.dwObjectID = playerIndex;
			msg.dwData1 = eHOUSEERR_LOADING;
			msg.dwData2 = cKind;
			msg.dwData3 = dwValue1;
			msg.dwData4 = dwValue2;
			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
			return;
		}

		// 하우스 입장예약 등록
		HOUSINGMGR->ReservationHouse(dwOwnerUserIndex, pHouse->dwChannelID, playerIndex, cKind);

		// 맵이동 명령 보내기
		MSG_DWORD6 msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_ENTRANCE_ACK;
		msg.dwObjectID = playerIndex;
		msg.dwData1 = 1;
		msg.dwData2 = (DWORD)pHouse->vStartPos.x;
		msg.dwData3 = (DWORD)pHouse->vStartPos.z;
		msg.dwData4 = cKind;
		msg.dwData5 = dwValue1;
		msg.dwData6 = dwValue2;
		PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
	}
}

void HouseFurnitureLoad(DWORD dwOwnerIndex, DWORD dwStartIndex, DWORD dwQueryServerConnectionIndex, DWORD dwPlayerMapNum, CHAR cKind, DWORD dwValue1, DWORD dwValue2, DWORD dwQueryPlayerID)
{
	sprintf(txt, "EXEC dbo.MP_HOUSE_FURNITURE_LOAD %d, %d, %d, %d, %d, %d, %d, %d",
		dwOwnerIndex, dwStartIndex, dwQueryServerConnectionIndex, dwPlayerMapNum, cKind, dwValue1, dwValue2, dwQueryPlayerID);
	g_DB.Query(eQueryType_FreeQuery, eHouseFurniture_Load, 0, txt);
}

void RHouseFurnitureLoad(LPQUERY pData, LPDBMESSAGE pMessage)
{
	QUERYST& record = pData[0];
	DWORD dwQueryServerConnectionIndex = (DWORD)(atoi((char*)record.Data[0]));	// 요청한 플레이어의 서버ConnectionIndex
	DWORD dwQueryPlayerMapNum = (DWORD)(atoi((char*)record.Data[1]));			// 요청한 플레이어Map Num
	CHAR cKind = (CHAR)(atoi((char*)record.Data[2]));							// 요청한 플레이어의 방문 종류
	DWORD dwValue1 = (DWORD)(atoi((char*)record.Data[3]));			//
	DWORD dwValue2 = (DWORD)(atoi((char*)record.Data[4]));			//
	DWORD dwQueryPlayerIndex = (DWORD)(atoi((char*)record.Data[5]));			// 요청한 플레이어Index
	DWORD dwUserIndex = (DWORD)(atoi((char*)pData[0].Data[6]));
	stHouse* pHouse = HOUSINGMGR->GetHouseByUserIndex(dwUserIndex);

	if(!pHouse)
		return;

	long nRemainTime = 0;
	// 가구목록 100개씩 로드
	DWORD i;
	for(i=0; i<pMessage->dwResult; i++)
	{
		record = pData[i];

		stFurniture* pFurniture = HOUSINGMGR->CreateNewFurniture();
		if(! pFurniture)
		{
			// 가구메모리 할당 실패했다!!
			__asm int 3;
		}

		pFurniture->dwOwnerUserIndex = dwUserIndex;
		pFurniture->dwFurnitureIndex = (DWORD)(atoi((char*)record.Data[7]));
		pFurniture->dwLinkItemIndex = (DWORD)(atoi((char*)record.Data[8]));
		pFurniture->nMaterialIndex = (int)atoi((char*)record.Data[9]);
		pFurniture->vPosition.x = (float)atof((char*)record.Data[10]);
		pFurniture->vPosition.y = (float)atof((char*)record.Data[11]);
		pFurniture->vPosition.z = (float)atof((char*)record.Data[12]);
		pFurniture->fAngle = (float)atof((char*)record.Data[13]);
		pFurniture->wCategory = (BYTE)(atoi((char*)record.Data[14]));
		pFurniture->wSlot = (BYTE)(atoi((char*)record.Data[15]));
		pFurniture->wState = (BYTE)(atoi((char*)record.Data[16]));
		pFurniture->bNotDelete = (BOOL)(atoi((char*)record.Data[17]));
		nRemainTime = (int)atoi((char*)record.Data[18]);

		if(!pFurniture->bNotDelete && (pFurniture->wState==eHOUSEFURNITURE_STATE_INSTALL || pFurniture->wState==eHOUSEFURNITURE_STATE_UNINSTALL))
		{	
			if(nRemainTime <= 0)
			{
				// 날짜경과 DB에서 지우자.
				HouseFurnitureUpdate(0, eHOUSEFURNITURE_DEL, pFurniture->dwOwnerUserIndex, pFurniture->dwFurnitureIndex, pFurniture->dwLinkItemIndex, 0, 
					0, 0, 0, 0,
					pFurniture->wSlot, eHOUSEFURNITURE_STATE_UNINSTALL, 0);
				HOUSINGMGR->RemoveFurnitureFromPool(pFurniture);

				// 내집창고 가구삭제 로그
				// DB에서 가구목록을 로드하는도중 삭제되는 가구는 CHARNAME필드에 "*HS_LoadEndTime"로 기록하고,
				// DECO_POINT, HOUSE_POINT는 '0'으로 기록한다.
				Log_Housing(0, "*HS_LoadEndTime", eHouseLog_EndTime, 0, 0, 0,
					pFurniture->dwFurnitureIndex, pFurniture->wSlot, pHouse->HouseInfo.szHouseName, 0, 0);

				continue;
			}

			pFurniture->dwRemainTime = (DWORD)nRemainTime;
		}
		else
		{
			pFurniture->dwRemainTime = HOUSINGMGR->GetRemainTimeFromItemIndex(pFurniture->dwLinkItemIndex);
		}

		HOUSINGMGR->AddFurnitureToHouse(pFurniture, pHouse);
	}

	if(pMessage->dwResult >= MAX_ROW_NUM)
	{
		const DWORD dwOwnerUserIndex = dwUserIndex;
		const DWORD lastFurnitureIndex = (DWORD)atoi((char*)pData[MAX_ROW_NUM-1].Data[7]);

		HouseFurnitureLoad(dwOwnerUserIndex, lastFurnitureIndex, dwQueryServerConnectionIndex, dwQueryPlayerMapNum, cKind, dwValue1, dwValue2, dwQueryPlayerIndex);
	}
	else
	{
		// 로딩완료
		// 로딩중인 하우스목록 제거
		HOUSINGMGR->RemoveLoadingHouse(dwUserIndex);

		// 하우스 입장예약 등록
		HOUSINGMGR->ReservationHouse(dwUserIndex, pHouse->dwChannelID, dwQueryPlayerIndex, cKind);

		// 맵이동 명령 보내기
		MSG_DWORD6 msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_ENTRANCE_ACK;
		msg.dwObjectID = dwQueryPlayerIndex;
		msg.dwData1 = 1;
		msg.dwData2 = (DWORD)pHouse->vStartPos.x;
		msg.dwData3 = (DWORD)pHouse->vStartPos.z;
		msg.dwData4 = cKind;
		msg.dwData5 = dwValue1;
		msg.dwData6 = dwValue2;
		PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwQueryPlayerMapNum, &msg, sizeof(msg));
	}
}

void HouseFurnitureUpdate(DWORD dwPlayerID, int nKind, DWORD dwUserIndex, DWORD dwFurnitureIndex, DWORD dwLinkItemIndex, BYTE byMtrlIndex,
						  float fPosX, float fPosY, float fPosZ, float fAngle, BYTE bySlot, BYTE byState, DWORD dwEndTime, BOOL bNotDelete)
{
	// nKind : [1]추가 [2]삭제 [3]갱신
	DWORD dwHiCategory = Get_HighCategory(dwFurnitureIndex);

	g_DB.FreeMiddleQuery(
		NULL,
		dwPlayerID,
		"EXEC dbo.MP_HOUSE_FURNITURE_UPDATE %d, %d, %d, %d, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d",
		nKind, dwUserIndex, dwFurnitureIndex, dwLinkItemIndex, byMtrlIndex,
		fPosX, fPosY, fPosZ, fAngle,
		(BYTE)dwHiCategory, bySlot, byState, bNotDelete, dwEndTime);
}

void HouseDelete(DWORD dwConnectionIndex, DWORD dwPlayerMapNum, DWORD dwPlayerID, char* pOwnerName)
{
	char szHouseOwnerName[MAX_HOUSING_NAME_LENGTH + 1] = {0,};
	SafeStrCpy(szHouseOwnerName, pOwnerName, MAX_HOUSING_NAME_LENGTH+1);

	if(IsCharInString(szHouseOwnerName, "'"))
	{
 		return;
	}

	g_DB.FreeMiddleQuery(
		RHouseDelete,
		dwPlayerID,
		"EXEC dbo.MP_HOUSEDELETE %d, %d, %d, \'%s\'",
		dwConnectionIndex,
		dwPlayerMapNum,
		dwPlayerID,
		szHouseOwnerName);
}

void RHouseDelete(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD playerIndex = dbMessage->dwID;

	if(1 != dbMessage->dwResult)
	{
		return;
	}


	const MIDDLEQUERYST& record = query[ 0 ];
	DWORD dwResult = DWORD(atoi((char*)record.Data[0]));
	DWORD dwConnectionIndex = DWORD(atoi((char*)record.Data[1]));
	DWORD dwPlayerMapNum = DWORD(atoi((char*)record.Data[2]));
	DWORD dwOwnerUserIndex = DWORD(atoi((char*)record.Data[4]));

	if(eHOUSEERR_NOOWNER == dwResult)
	{
		return;
	}
	else if(eHOUSEERR_NOHOUSE == dwResult)
	{
		// 하우스 미존재 통보
		MSG_DWORD msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_CHEAT_DELETE_NACK;
		msg.dwObjectID = playerIndex;
		msg.dwData = eHOUSEERR_NOHOUSE;
		g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(msg));
		return;
	}

	if(HOUSINGMGR->IsLoadingHouse(dwOwnerUserIndex))
	{
		// 하우스 로딩중 통보
		MSG_DWORD msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_CHEAT_DELETE_NACK;
		msg.dwObjectID = playerIndex;
		msg.dwData = eHOUSEERR_LOADING;
		g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(msg));
		return;
	}

	HOUSINGMGR->DeleteHouse(dwOwnerUserIndex);

	// 하우스 삭제통보
	MSG_DWORD msg;
	msg.Category = MP_HOUSE;
	msg.Protocol = MP_HOUSE_CHEAT_DELETE_ACK;
	msg.dwObjectID = playerIndex;
	PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwPlayerMapNum, &msg, sizeof(msg));
}

void HouseRankLoad()
{
	g_DB.FreeMiddleQuery(
		RHouseRank,
		NULL,
		"EXEC dbo.MP_HOUSERANK_LOAD");
}

void HouseRankUpdate()
{
	g_DB.FreeMiddleQuery(
		RHouseRank,
		NULL,
		"EXEC dbo.MP_HOUSERANK");
}

void RHouseRank(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const MIDDLEQUERYST& record = query[ 0 ];

	HOUSINGMGR->SetLoadRank(TRUE);

	BOOL bResult = BOOL(atoi((char*)record.Data[0]));	//bResult : [0]=순위로딩, [1]=순위갱신

	stHouseRank* pRank = HOUSINGMGR->GetHouseRank();
	if(pRank)
	{
		char* pHouseName;

		pRank->dwRank_1_UserIndex = DWORD(atoi((char*)record.Data[1]));
		pHouseName = (char*)record.Data[2];
		strcpy(pRank->szRank_1_HouseName, pHouseName);
		pRank->byRank_1_UserExterioKind = BYTE(atoi((char*)record.Data[3]));
		pRank->dwRank_1_HousePoint = DWORD(atoi((char*)record.Data[4]));

		pRank->dwRank_2_UserIndex = DWORD(atoi((char*)record.Data[5]));
		pHouseName = (char*)record.Data[6];
		strcpy(pRank->szRank_2_HouseName, pHouseName);
		pRank->byRank_2_UserExterioKind = BYTE(atoi((char*)record.Data[7]));
		pRank->dwRank_2_HousePoint = DWORD(atoi((char*)record.Data[8]));

		pRank->dwRank_3_UserIndex = DWORD(atoi((char*)record.Data[9]));
		pHouseName = (char*)record.Data[10];
		strcpy(pRank->szRank_3_HouseName, pHouseName);
		pRank->byRank_3_UserExterioKind = BYTE(atoi((char*)record.Data[11]));
		pRank->dwRank_3_HousePoint = DWORD(atoi((char*)record.Data[12]));

		char* pTemp = (char*)record.Data[13];
		if(strlen(pTemp) >= 10)
		{
			char seps[] = "-";
			char* pCalcYear = strtok(pTemp, seps);
			char* pCalcMonth = strtok(NULL, seps);
			char* pCalcDay = strtok(NULL, seps);

			pRank->wCalcYear = WORD( atoi(pCalcYear) );
			pRank->wCalcMonth = WORD( atoi(pCalcMonth) );
			pRank->wCalcDay = WORD( atoi(pCalcDay) );
		}

		if(bResult && HOUSINGMAPNUM==g_pServerSystem->GetMapNum())
		{
			// 순위가 갱신되었음 전체서버에 통보
			MSG_HOUSE_RANK_UPDATE msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_RANKINFO_FORSERVER;
			memcpy(&msg.HouseRank, pRank, sizeof(msg.HouseRank));
			g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) );	//에이젼트로 브로드캐스팅
			return;
		}

		HOUSINGMGR->SetHouseRank(pRank);
	}
}

void Log_Housing(DWORD dwOwnerIndex, char* pCharName, CHAR cLogType, DWORD dwItemDBIndex, DWORD dwItemIndex, DWORD dwItemValue, 
				 DWORD dwFurnitureIndex, BYTE bySlot, char* pHouseName, DWORD dwDecoPoint, DWORD dwHousePoint)
{
	DWORD dwHighCategory = Get_HighCategory(dwFurnitureIndex);

	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.TP_HOUSING_LOG %d, '%s', %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d",
		dwOwnerIndex,
		pCharName,
		cLogType,
		dwItemDBIndex,
		dwItemIndex,
		dwItemValue,
		dwFurnitureIndex,
		dwHighCategory,
		bySlot,
		pHouseName,
		dwDecoPoint,
		dwHousePoint );
}

void DungeonEntrance(DWORD dwConnectionIndex, DWORD dwPlayerMapNum, DWORD dwPlayerIndex, DWORD dwPartyIndex, DWORD dwKeyIndex, DWORD dwItemIndex, DWORD dwItemSLot, WORD wDungeonMapNum, eDIFFICULTY difficulty)
{
	g_DB.FreeMiddleQuery(
		RDungeonEntrance,
		dwPlayerIndex,
		"EXEC dbo.MP_DUNGEONENTRANCE %d, %d, %d, %d, %d, %d, %d, %d, %d", dwConnectionIndex, dwPlayerMapNum, dwPlayerIndex, dwPartyIndex, dwKeyIndex, dwItemIndex, dwItemSLot, wDungeonMapNum, difficulty);
}

void RDungeonEntrance(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	if(1 != dbMessage->dwResult)
	{
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];
	DWORD dwPlayerMapNum = DWORD(atoi((char*)record.Data[2]));
	DWORD dwPlayerIndex = DWORD(atoi((char*)record.Data[3]));
	DWORD dwKeyIndex = DWORD(atoi((char*)record.Data[5]));
	DWORD dwItemIndex = DWORD(atoi((char*)record.Data[6]));
	DWORD dwItemSlot = DWORD(atoi((char*)record.Data[7]));
	DWORD wDungeonMapNum = WORD(atoi((char*)record.Data[8]));
	DWORD dwTotalVisit = DWORD(atoi((char*)record.Data[10]));
	DWORD dwDailyVisit = DWORD(atoi((char*)record.Data[11]));
	DWORD dwElapsedMinute = DWORD(atoi((char*)record.Data[12]));

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if(! pPlayer)
		return;

	DWORD dwError = eDungeonERR_None;

	stDungeonKey* pKey = GAMERESRCMNGR->GetDungeonKey(dwKeyIndex);
	if(! pKey)
		return;

	if(pPlayer->GetLevel() < pKey->wMinLevel)
		dwError = eDungeonERR_NotEnoughLevel;
	else if(pKey->wMaxLevel < pPlayer->GetLevel())
		dwError = eDungeonERR_OverLevel;

	// 하루초과
	if(dwElapsedMinute > 1440)
		dwDailyVisit = 0;

	// 입장딜레이
	if(dwElapsedMinute <= pKey->wDelayMin)
		dwError = eDungeonERR_EntranceDelay;

	if(dwDailyVisit >= pKey->wMaxCount)
		dwError = eDungeonERR_OverCount;

	if(eDungeonERR_None != dwError)
	{
		MSG_DWORD	msg;
		msg.Category = MP_DUNGEON;
		msg.Protocol = MP_DUNGEON_ENTRANCE_NACK;
		msg.dwObjectID = dwPlayerIndex;
		msg.dwData = dwError;
		pPlayer->SendMsg(&msg, sizeof(msg));
		return;
	}

	DungeonUpdate(dwPlayerIndex, wDungeonMapNum, ++dwTotalVisit, ++dwDailyVisit);

	MSG_DWORD5 msg;
	msg.Category = MP_DUNGEON;
	msg.Protocol = MP_DUNGEON_ENTRANCE_FROM_SERVER_SYN;
	msg.dwObjectID = dwPlayerIndex;
	msg.dwData1 = dwKeyIndex;
	msg.dwData2 = dwPlayerMapNum;
	msg.dwData3 = pPlayer->GetPartyIdx();
	msg.dwData4 = dwItemIndex;
	msg.dwData5 = dwItemSlot;
	PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)wDungeonMapNum, &msg, sizeof(msg));
}

void DungeonUpdate(DWORD dwPlayerIndex, DWORD wDungeonMapNum, DWORD wTotalVisit, DWORD wDailyVisit, BYTE byDoRollBack)
{
	g_DB.FreeMiddleQuery(
		0,
		dwPlayerIndex,
		"EXEC dbo.MP_DUNGEONUPDATE %d, %d, %d, %d, %d", dwPlayerIndex, wDungeonMapNum, wTotalVisit, wDailyVisit, byDoRollBack);
}


void RGTCheckRegist(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const MIDDLEQUERYST& record = query[ 0 ];

	DWORD dwPlayerID = DWORD(atoi((char*)record.Data[0]));
	DWORD dwMapNum = DWORD(atoi((char*)record.Data[1]));
	DWORD dwGuildID = DWORD(atoi((char*)record.Data[3]));
	DWORD dwFeePoint = DWORD(atoi((char*)record.Data[4]));
	DWORD dwFeeMoney = DWORD(atoi((char*)record.Data[5]));
	DWORD dwResult = DWORD(atoi((char*)record.Data[6]));

	if(dwResult)
	{
		CGuild* pGuild = GUILDMGR->GetGuild(dwGuildID);
		if(!pGuild)		return;

		MSG_NAME_DWORD5 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_REGIST_ACK;
		SafeStrCpy(msg.Name, pGuild->GetGuildName(), sizeof(msg.Name));
		msg.dwData1 = dwPlayerID;
		msg.dwData2 = GTMGR->GetTournamentCount();
		msg.dwData3 = dwFeePoint;
		msg.dwData4 = dwFeeMoney;
		msg.dwData5 = dwGuildID;
		PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwMapNum, &msg, sizeof(msg));
	}
	else
	{
		GTMGR->RemoveGTEntryInfo(dwGuildID);

		MSG_DWORD2 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_REGIST_NACK;
		msg.dwData1 = dwPlayerID;
		msg.dwData2 = eGTError_NOTENOUGHFEE;
		PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)dwMapNum, &msg, sizeof(msg));
		return;
	}
}

void GTRewardItemInsert(DWORD characterIdx, DWORD gtcount, DWORD itemIdx, DWORD count, BOOL isStack, DWORD endtime)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RGTRewardItemInsert,
		characterIdx,
		"EXEC dbo.MP_GT_REWARD_INSERT %d, %d, %d, %d, %d, %d",
		characterIdx,
		gtcount,
		itemIdx,
		count,
		isStack,
		endtime );
}

void RGTRewardItemInsert(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD playerIndex = dbMessage->dwID;

	const MIDDLEQUERYST& record = query[ 0 ];

	char* pPlayerName = (char*)(record.Data[1]);
	DWORD dwItemIdx = DWORD(atoi((char*)record.Data[2]));
	DWORD dwCount = DWORD(atoi((char*)record.Data[3]));

	// 아이템 지급완료 로그저장
	LogItemMoney(
				playerIndex,
				pPlayerName,
				0,
				"*GT_Reward",
				eLog_ItemObtainGTReward,
				0, //pPlayer->GetMoney(),
				0,
				0,
				dwItemIdx,
				0,
				0,
				0,
				dwCount,
				0 );

	// 아이템 받을놈이 어디 있는지 모르니 에이전트로 브로드캐스팅.
	MSG_DWORD msg;
	ZeroMemory(&msg, sizeof(MSG_DWORD));

	msg.Category = MP_GTOURNAMENT;
	msg.Protocol = MP_GTOURNAMENT_REWARD;
	msg.dwObjectID = playerIndex;
	// 091113 ONS 우승/준우승시 획득한 아이템명을 전달한다.
	msg.dwData		= dwItemIdx;

	g_Network.Broadcast2AgentServer((char*)&msg,sizeof(msg));
}

// 091201 ONS 길드토너먼트 우승(준우승) 상금지급 처리
void GTRewardMoneyInsert( DWORD CharacterIdx, DWORD dwMoney )
{
	sprintf(txt, "EXEC dbo.MP_GT_REWARD_MONEY %d, %d", CharacterIdx, dwMoney);
	g_DB.Query(eQueryType_FreeQuery, eGTRewardMoneyInsert, CharacterIdx, txt, CharacterIdx);
}

void RGTRewardMoneyInsert(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if( !pMessage->dwResult )
		return;


	DWORD dwPlayerID = (DWORD)atoi((char*)pData->Data[0]);
	DWORD dwMoney = (DWORD)atoi((char*)pData->Data[1]);

	MSG_DWORD msg;
	ZeroMemory(&msg, sizeof(MSG_DWORD));
	msg.Category	= MP_GTOURNAMENT;
	msg.Protocol	= MP_GTOURNAMENT_REWARD_MONEY_TO_MAP;
	msg.dwObjectID	= dwPlayerID;
	msg.dwData		= dwMoney;

	g_Network.Broadcast2AgentServer((char*)&msg,sizeof(msg));
}

void GTInfoLoad()
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RGTInfoLoad,
		g_pServerSystem->GetMapNum(),
		"EXEC dbo.MP_GT_INFOLOAD" );

	GTMGR->AddWaitingFlag(GT_WAITING_INFOLOAD);
}

void RGTInfoLoad(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const MIDDLEQUERYST& record = query[ 0 ];

	WORD wGTCount = WORD(atoi((char*)record.Data[0]));
	WORD wState = WORD(atoi((char*)record.Data[1]));
	WORD wRound = WORD(atoi((char*)record.Data[2]));
	GTMGR->SetTournamentCount(wGTCount);
	GTMGR->SetLastState(wState);
	GTMGR->SetLastRound(wRound);

	if(0 < wGTCount)
		GTInfoLoadAll(wGTCount);

	GTMGR->RemoveWiatingFlag(GT_WAITING_INFOLOAD);
}

void GTInfoLoadAll(WORD wGTCount)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RGTInfoLoadAll,
		g_pServerSystem->GetMapNum(),
		"EXEC dbo.MP_GT_INFOLOADALL %d",
		wGTCount);

	GTMGR->AddWaitingFlag(GT_WAITING_INFOLOADALL);
}

void RGTInfoLoadAll(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	for(DWORD i = 0; i<dbMessage->dwResult; ++i)
	{
		const MIDDLEQUERYST& record = query[i];

		WORD wGTCount = WORD(atoi((char*)record.Data[0]));
		DWORD dwGuildIdx = DWORD(atoi((char*)record.Data[1]));
		WORD wPosition = WORD(atoi((char*)record.Data[3]));
		WORD wRound = WORD(atoi((char*)record.Data[4]));

		if(0 < dwGuildIdx)
		{
			GTMGR->FillLastGTInfo(dwGuildIdx, wPosition, wRound);
			GTPlayerLoad(wGTCount, dwGuildIdx);
		}
	}

	GTMGR->RemoveWiatingFlag(GT_WAITING_INFOLOADALL);
}

void GTInfoInsert(WORD wGTCount, DWORD dwGuildIdx, char* pGuildName, WORD wPosition, WORD wRound)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		g_pServerSystem->GetMapNum(),
		"EXEC dbo.MP_GT_INFOINSERT %d, %d, '%s', %d, %d",
		wGTCount,
		dwGuildIdx,
		pGuildName,
		wPosition,
		wRound);
}

void GTInfoUpdate(WORD wGTCount, DWORD dwGuildIdx, WORD wPosition, WORD wRound)
{
	// dwGuildIdx == 0 인경우는 진행상태를 갱신합니다.
	// 이때,
	// wPosition의 값은 TournamentState를 전달하고,
	// wRound의 값은 다음진행될 Round를 전달합니다.
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		g_pServerSystem->GetMapNum(),
		"EXEC dbo.MP_GT_INFOUPDATE %d, %d, %d, %d",
		wGTCount,
		dwGuildIdx,
		wPosition,
		wRound);
}

void GTPlayerInsert(WORD wGTCount, DWORD dwGuildIdx, DWORD* pPlayers)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		g_pServerSystem->GetMapNum(),
		"EXEC dbo.MP_GT_PLAYERINSERT %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
		wGTCount,
		dwGuildIdx,
		pPlayers[0], pPlayers[1], pPlayers[2], pPlayers[3], pPlayers[4], pPlayers[5], pPlayers[6],
		pPlayers[7], pPlayers[8], pPlayers[9], pPlayers[10], pPlayers[11], pPlayers[12], pPlayers[13]);
}

void GTPlayerLoad(WORD wGTCount, DWORD dwGuildIdx)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		RGTPlayerLoad,
		g_pServerSystem->GetMapNum(),
		"EXEC dbo.MP_GT_PLAYERLOAD %d, %d",
		wGTCount,
		dwGuildIdx);

	GTMGR->AddWaitingFlag(GT_WAITING_PLAYERLOAD);
}

void RGTPlayerLoad(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const MIDDLEQUERYST& record = query[0];

	WORD wGTCount;
	DWORD dwGuildIdx;
	DWORD dwPlayer[MAX_GTOURNAMENT_PLAYER];

	wGTCount = WORD(atoi((char*)record.Data[0]));
	dwGuildIdx = DWORD(atoi((char*)record.Data[1]));

	for(int i=0; i<MAX_GTOURNAMENT_PLAYER; i++)
		dwPlayer[i] = DWORD(atoi((char*)record.Data[i+2]));

	GTMGR->FillPlayerInfo(wGTCount, dwGuildIdx, &dwPlayer[0]);

	GTMGR->RemoveWiatingFlag(GT_WAITING_PLAYERLOAD);
}

void GTInfoReset(WORD wGTCount, WORD wRount)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeMiddleQuery(
		0,
		g_pServerSystem->GetMapNum(),
		"EXEC dbo.MP_GT_INFORESET %d, %d",
		wGTCount,
		wRount);
}

void PetInfoLoad( DWORD characterIdx, DWORD userIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC  %s %d", "dbo.MP_PET_INFO_LOAD", userIdx );
	g_DB.Query( eQueryType_FreeQuery, ePetInfoLoad, characterIdx, txt, characterIdx );
}

void RPetInfoLoad( LPQUERY pData, LPDBMESSAGE pMessage )
{
	CPlayer* const playerObject = (CPlayer*)g_pUserTable->FindUser(
		pMessage->dwID);

	if(0 == playerObject)
	{
		return;
	}
	else if(eObjectKind_Player != playerObject->GetObjectKind())
	{
		return;
	}
	
	for(DWORD i = 0; i < pMessage->dwResult; ++i)
	{
		MSG_PET_INFO message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_PET;
		message.Protocol = MP_PET_OBJECT_INFO;

		const QUERYST& record = pData[i];
		message.PetObjectInfo.ID = _ttoi(
			LPCTSTR(record.Data[0]));
		message.PetObjectInfo.ItemDBIdx = _ttoi(
			LPCTSTR(record.Data[1]));
		message.PetObjectInfo.MasterIdx = _ttoi(
			LPCTSTR(record.Data[2]));
		message.PetObjectInfo.Kind = _ttoi(
			LPCTSTR(record.Data[3]));
		message.PetObjectInfo.Level = BYTE(_ttoi(
			LPCTSTR(record.Data[4])));
		message.PetObjectInfo.Grade = BYTE(_ttoi(
			LPCTSTR(record.Data[5])));
		message.PetObjectInfo.SkillSlot = BYTE(_ttoi(
			LPCTSTR(record.Data[6])));
		message.PetObjectInfo.Exp = WORD(_ttoi(
			LPCTSTR(record.Data[7])));
		message.PetObjectInfo.Friendly = BYTE(_ttoi(
			LPCTSTR(record.Data[8])));
		message.PetObjectInfo.HP = _ttoi(
			LPCTSTR(record.Data[9]));
		message.PetObjectInfo.MAXHP = _ttoi(
			LPCTSTR(record.Data[10]));
		message.PetObjectInfo.MP = _ttoi(
			LPCTSTR(record.Data[11]));
		message.PetObjectInfo.MAXMP = _ttoi(
			LPCTSTR(record.Data[12]));
		message.PetObjectInfo.AI = ePetAI(_ttoi(
			LPCTSTR(record.Data[13])));
		message.PetObjectInfo.State = ePetState(_ttoi(
			LPCTSTR(record.Data[14])));
		message.PetObjectInfo.Type = ePetType(_ttoi(
			LPCTSTR(record.Data[15])));
		message.PetObjectInfo.FriendlyCheckTime = _ttoi(
			LPCTSTR(record.Data[16]));
		SafeStrCpy(
			message.PetObjectInfo.MasterName,
			playerObject->GetObjectName(),
			_countof(message.PetObjectInfo.MasterName));

		playerObject->SendMsg(
			&message,
			sizeof(message));
		PETMGR->Update(
			message.PetObjectInfo);

		const DWORD ownedPlayerIndex = _ttoi(
			LPCTSTR(record.Data[17]));

		if(playerObject->GetID() != ownedPlayerIndex)
		{
			continue;
		}
		else if(ePetState_Summon != message.PetObjectInfo.State)
		{
			continue;
		}

		// 091214 ONS 펫 맵이동시 소환/봉인 관련 메세지를 출력하지 않도록 처리.
		// 아이템 소환과 구분하기 위해 플래그를 전달한다.
		PETMGR->SummonPet(
			playerObject,
			message.PetObjectInfo.ItemDBIdx,
			FALSE);
	}
}

void RPetInfoAdd( LPQUERY pData, LPDBMESSAGE pMessage )
{
	CPlayer* playerObject = (CPlayer*)g_pUserTable->FindUser(
		pMessage->dwID);

	if(0 == playerObject)
	{
		return;
	}
	else if(eObjectKind_Player != playerObject->GetObjectKind())
	{
		return;
	}
	else if(0 == pMessage->dwResult)
	{
		return;
	}

	const QUERYST& record = pData[0];

	MSG_PET_INFO message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_PET;
	message.Protocol = MP_PET_HATCH;
	message.PetObjectInfo.ID = _ttoi(
		LPCTSTR(record.Data[0]));
	message.PetObjectInfo.ItemDBIdx = _ttoi(
		LPCTSTR(record.Data[1]));
	message.PetObjectInfo.MasterIdx = _ttoi(
		LPCTSTR(record.Data[2]));
	message.PetObjectInfo.Kind = _ttoi(
		LPCTSTR(record.Data[3]));
	message.PetObjectInfo.Level = BYTE(_ttoi(
		LPCTSTR(record.Data[4])));
	message.PetObjectInfo.Grade = BYTE(_ttoi(
		LPCTSTR(record.Data[5])));
	message.PetObjectInfo.SkillSlot = BYTE(_ttoi(
		LPCTSTR(record.Data[6])));
	message.PetObjectInfo.Exp = WORD(_ttoi(
		LPCTSTR(record.Data[7])));
	message.PetObjectInfo.Friendly = BYTE(_ttoi(
		LPCTSTR(record.Data[8])));
	message.PetObjectInfo.HP = _ttoi(
		LPCTSTR(record.Data[9]));
	message.PetObjectInfo.MAXHP = _ttoi(
		LPCTSTR(record.Data[10]));
	message.PetObjectInfo.MP = _ttoi(
		LPCTSTR(record.Data[11]));
	message.PetObjectInfo.MAXMP = _ttoi(
		LPCTSTR(record.Data[12]));
	message.PetObjectInfo.AI = ePetAI(_ttoi(
		LPCTSTR(record.Data[13])));
	message.PetObjectInfo.State = ePetState(_ttoi(
		LPCTSTR(record.Data[14])));
	message.PetObjectInfo.Type = ePetType(_ttoi(
		LPCTSTR(record.Data[15])));
	message.PetObjectInfo.FriendlyCheckTime = _ttoi(
		LPCTSTR(record.Data[16]));
	SafeStrCpy(
		message.PetObjectInfo.MasterName,
		playerObject->GetObjectName(),
		_countof(message.PetObjectInfo.MasterName));

	PETMGR->Update(
		message.PetObjectInfo);
	playerObject->SendMsg(
		&message,
		sizeof(message));
}

// 090720 ONS 펫 고정스킬정보를 프로시져에 추가한다.
void PetInfoAdd( DWORD characterIdx, DWORD userIdx, DWORD itemDBIdx, WORD kind, BYTE SkillSlot, DWORD SkillIdx, WORD Position )
{
	sprintf( txt, "EXEC  %s %d, %d, %d, %d, %d, %d", "dbo.MP_PET_INFO_ADD", userIdx, itemDBIdx, kind, SkillSlot, SkillIdx, Position );
	g_DB.Query( eQueryType_FreeQuery, ePetInfoAdd, characterIdx, txt, characterIdx );
}

void PetInfoRemove( DWORD characterIdx, DWORD itemDBIdx )
{
	sprintf( txt, "EXEC  %s %d", "dbo.MP_PET_INFO_REMOVE", itemDBIdx );
	g_DB.Query( eQueryType_FreeQuery, ePetInfoRemove, characterIdx, txt, characterIdx );
}

void PetInfoUpdate( DWORD characterIdx, DWORD itemDBIdx, BYTE level, BYTE grade, ePetType type, BYTE slot, WORD exp, BYTE friendy, DWORD maxhp, DWORD hp, DWORD maxmp, DWORD mp, ePetAI ai, ePetState state, DWORD time)
{
	sprintf( txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", "dbo.MP_PET_INFO_UPDATE", itemDBIdx, level, grade, type, slot, exp, friendy, maxhp, hp, maxmp, mp, ai, state, time );
	g_DB.Query( eQueryType_FreeQuery, ePetInfoUpdate, characterIdx, txt, characterIdx );
}

void PetMasterUpdate( DWORD characterIdx, DWORD userIdx, DWORD itemDBIdx )
{
	sprintf( txt, "EXEC  %s %d, %d", "dbo.MP_PET_MASTER_UPDATE", userIdx, itemDBIdx );
	g_DB.Query( eQueryType_FreeQuery, ePetMasterUpdate, characterIdx, txt, characterIdx );
}

// 080425 KTH -- 가축 로그 --------------------------------------------------------------------------------------------------
void InsertLogFarmAnimal( CSHFarm* pFarm, CAnimal* pAnimal,  eFamilyLog type )
{
	if( !pFarm ||
		!pAnimal )
	{
		ASSERT(0);
		return;
	}

	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.TP_FARM_ANIMAL_LOG_INSERT %d, %d, %d, %d, %d, %d, %d, %d, %d, \'""\'",
		pFarm->GetID(),
		pAnimal->GetOwner(),
		pAnimal->GetID(),
		pAnimal->GetKind(),
		pAnimal->GetStep(),
		pAnimal->GetLife(),
		pAnimal->GetContentment(),
		pAnimal->GetInterest(),
		type );
}

void ItemMoveUpdatePetInven( DWORD characterIdx, DWORD userIdx, DWORD petIdx, DWORD fromItemIdx, DWORD fromItemPos, DWORD toItemIdx, DWORD toItemPos, BYTE param )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d, %d", "dbo.MP_ITEM_MOVEUPDATEPETINVEN", characterIdx, userIdx, petIdx, fromItemIdx, fromItemPos, toItemIdx, toItemPos, param );
	g_DB.Query( eQueryType_FreeQuery, eItemMoveUpdatePetInven, characterIdx, txt, characterIdx );
}

void PetItemInfoLoad( DWORD characterIdx, DWORD petIdx)
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( txt, "EXEC  %s %d", "dbo.MP_PET_ITEMINFO", petIdx );
	g_DB.Query( eQueryType_FreeQuery, ePetItemInfoLoad, characterIdx, txt, characterIdx );
}

void RPetItemInfoLoad( LPQUERY pData, LPDBMESSAGE pMessage )
{
	CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(
		pMessage->dwID);

	if(0 == pPlayer)
	{
		return;
	}
	else if(eObjectKind_Player != pPlayer->GetObjectKind())
	{
		return;
	}

	const PET_OBJECT_INFO& petObjectInfo = PETMGR->GetObjectInfo(
		pPlayer->GetPetItemDbIndex());
	PET_INFO* const petInfo = PETMGR->GetInfo(
		petObjectInfo.Kind);

	if(0 == petInfo)
	{
		return;
	}

	ITEMBASE buf[ePetEquipSlot_Max];
	ZeroMemory(
		buf,
		sizeof(buf));

	for( DWORD i = 0; i < pMessage->dwResult; ++i )
	{
		const QUERYST& record = pData[i];
		POSTYPE pos = POSTYPE( atoi((char*)record.Data[3]) );

		if( pos >= ePetEquipSlot_Max )
		{
			continue;
		}

		const DWORD itemDbIndex = _ttoi(
			LPCTSTR(record.Data[1]));
		const DWORD itemIndex = _ttoi(
			LPCTSTR(record.Data[2]));
		const ITEM_SEAL_TYPE isSealed = ITEM_SEAL_TYPE(_ttoi(
			LPCTSTR(record.Data[4])));

		if( ( pos >= ePetEquipSlot_Skill1 + petObjectInfo.SkillSlot )	&&
			// 090720 ONS 펫 고정스킬의 슬롯(ePetEquipSlot_Skill6)에 아이템을 설정한다. 
			!PETMGR->IsPetStaticSkill(petInfo, pos))
		{
			ItemDeleteToDB( atoi((char*)record.Data[1]) );

			LogItemMoney(
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				0, 
				"",
				eLog_PetItemDiscard,
				pPlayer->GetMoney(eItemTable_Inventory),
				0,
				0,
				atoi((char*)record.Data[2]),
				itemDbIndex,
				pos,
				0,
				0,
				pPlayer->GetPlayerExpPoint());
			
			continue;
		}

		ITEMBASE& itemBase = buf[pos];
		itemBase.dwDBIdx = itemDbIndex;
		itemBase.wIconIdx = itemIndex;
		itemBase.Position = pos;
		itemBase.nSealed = isSealed;

		const ITEM_INFO* const ItemInfo = ITEMMGR->GetItemInfo(
			itemIndex);

		if(0 == ItemInfo)
		{
			continue;
		}

		if(ItemInfo->SupplyType == ITEM_KIND_COOLTIME )
		{
			itemBase.ItemParam  = gCurTime;
			itemBase.nRemainSecond = atoi((char*)record.Data[5]);
		}

		if(ItemInfo->nTimeKind == eKIND_REALTIME &&
			itemBase.nSealed == eITEM_TYPE_UNSEAL )
		{
			itemBase.nRemainSecond = atoi((char*)record.Data[5]);
			
			if( itemBase.nRemainSecond <= 0 )
			{
				ItemDeleteToDB(
					itemDbIndex);
				LogItemMoney(
					pPlayer->GetID(),
					pPlayer->GetObjectName(),
					0,
					"",
					eLog_ShopItemUseEndFromPetInven,
					pPlayer->GetMoney(eItemTable_Inventory),
					0,
					0,
					itemIndex,
					itemDbIndex,
					pos,
					0,
					itemBase.Durability,
					pPlayer->GetPlayerExpPoint());
				ZeroMemory(
					&itemBase,
					sizeof(itemBase));
			}
		}
		else
		{
			itemBase.nRemainSecond = atoi((char*)record.Data[6]) ;
		}
	}

	VECTOR3* pPlayerPos = CCharMove::GetPosition( pPlayer );
	
	int ran=0;
	ran = rand();
	float dx = float(ran%PETMGR->GetAreaDistance()) * (ran%2?1:-1);
	ran = rand();
	float dz = float(ran%PETMGR->GetAreaDistance()) * (ran%2?1:-1);

	VECTOR3 pos = {
		pPlayerPos->x + dx,
		0,
		pPlayerPos->z + dz
	};

	if( pos.x < 0 )				pos.x = 0;
	else if( pos.x > 51199 )	pos.x = 51199;

	if( pos.z < 0 )				pos.z = 0;
	else if( pos.z > 51199 )	pos.z = 51199;

	BASEOBJECT_INFO info;
	ZeroMemory(
		&info,
		sizeof(info));
	info.dwObjectID = petObjectInfo.ID;
	info.dwUserID = petObjectInfo.MasterIdx;
	SafeStrCpy(
		info.ObjectName,
		petInfo->Name,
		_countof(info.ObjectName));
	info.BattleID = pPlayer->GetBattleID();
	info.BattleTeam = pPlayer->GetBattleTeam();

	CPet* const pPet = g_pServerSystem->AddPet(
		info,
		petObjectInfo,
		pPlayer);

	if(0 == pPet)
	{
		return;
	}

	pPet->SetAllWearedItem(
		buf);

	CCharMove::InitMove(
		pPet,
		&pos);
	pPet->SetSummonFlag(
		FALSE);	
}

// 080716 LUJ, 펫 로그
void LogPet( const PET_OBJECT_INFO& info, ePetLog log )
{
	g_DB.LogMiddleQuery(
		0,
		0,
		"INSERT INTO TB_PET_LOG ( TYPE, USER_IDX, PET_IDX, EXPERIENCE, FRIENDSHIP, GRADE, LEVEL, KIND, STATE, SKILL_SLOT ) VALUES ( %d, %d, %d, %d, %d, %d, %d, %d, %d, %d )",
		log,
		info.MasterIdx,
		info.ID,
		info.Exp,
		info.Friendly,
		info.Grade,
		info.Level,
		info.Kind,
		info.State,
		info.SkillSlot);
}

void Option_Load(DWORD dwPlayerIdx)
{
	g_DB.FreeMiddleQuery(
		ROption_Load,
		dwPlayerIdx,
		"EXEC dbo.MP_OPTIONLOAD %d",
		dwPlayerIdx );
}

void ROption_Load(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(
		dbMessage->dwID);

	if(0 ==	pPlayer ||
		dbMessage->dwResult != 1)
	{
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];
	pPlayer->GetCharacterTotalInfo().HideFlag = atoi((char*)record.Data[0]);
	pPlayer->SetInitState(
		PLAYERINITSTATE_OPTION_INFO,
		MP_USERCONN_GAMEIN_SYN);
}

void Option_Update(CPlayer& pPlayer)
{
	g_DB.FreeMiddleQuery(
		NULL,
		pPlayer.GetID(),
		"EXEC dbo.MP_OPTIONUPDATE %d, %d",
		pPlayer.GetID(),
		pPlayer.GetCharacterTotalInfo().HideFlag);
}

void CharSize_Update(CPlayer* pPlayer)
{
	if(!pPlayer)	return;

	// 길토맵에선 키정보를 업데이트 하지 않는다.
	if( g_pServerSystem->GetMapNum()==GTMAPNUM )
	{
		return;
	}
	// 공성 맵에서도 키정보를 업데이트 하지 않는다.
	else if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE && SIEGEWARFAREMGR->IsSiegeWarfareZone() )
	{
		return;
	}


	DWORD dwSize = (DWORD)(pPlayer->GetCharacterSize() * 100.0f);

	g_DB.FreeMiddleQuery(
		NULL,
		pPlayer->GetID(),
		"EXEC  DBO.MP_CHARSIZEUPDATE %d, %d",
		pPlayer->GetID(), dwSize);
}

//-------------------------------------------------------------------------------------------------
//	NAME		: RSiegeRecallLoad
//	DESC		: 공성용 소환 오브젝트 정보를 로드하는 분류 추가.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 12, 2008
//-------------------------------------------------------------------------------------------------
void RSiegeRecallLoad( LPQUERY pData, LPDBMESSAGE pMessage )
{
	for( DWORD count = 0 ;  count < pMessage->dwResult ; ++count )
	{
		MSG_SIEGERECALL_LOADOBJINFO msg;
		ZeroMemory(
			&msg,
			sizeof(msg));
		msg.Category	 = MP_SIEGERECALL ;
		msg.Protocol	 = MP_SIEGERECALL_REQUEST_OBJINFO_SYN ;
		msg.ThemeIdx 	 = (BYTE)atoi((char*)pData[count].Data[0]) ;
		msg.MapIdx		 = (WORD)atoi((char*)pData[count].Data[1]) ;
		msg.RecallMapIdx = (WORD)atoi((char*)pData[count].Data[2]) ;
		msg.StepIdx		 = (BYTE)atoi((char*)pData[count].Data[3]) ;
		msg.AddObjIdx	 = (BYTE)atoi((char*)pData[count].Data[4]) ;
		msg.ComKind		 = (BYTE)atoi((char*)pData[count].Data[5]) ;
		msg.ComIndex	 = (BYTE)atoi((char*)pData[count].Data[6]) ;

		g_Network.Send2AgentServer( (char*)&msg, sizeof(MSG_SIEGERECALL_LOADOBJINFO) ) ;
	}
}


// 공성 지역 정보를 가져온다.
void RSiegeWarfareInfoLoad(LPQUERY pData, LPDBMESSAGE pMessage)
{
	DWORD count = pMessage->dwResult;

	if( count == 0 ) return;
	 
	// 080901 LYW --- MapDBMsgParser : 맵 타입이 0이어도, 공성 상태는 받아야 할 것 같다.
	// 우선 0이더라도, 상태값은 세팅할 수 있도록, 잠시 아래 라인 주석 처리 한다.
	// KTH오면 물어볼것.
	//if( atoi( ( char* )pData->Data[ 0 ] ) == 0 ) return;

	DWORD dwMapType, dwCastleGuildIdx;
	WORD wState;

	for( DWORD i = 0; i < count; i++ )
	{
		dwMapType   = (DWORD)atoi((char*)pData[i].Data[0]);
		dwCastleGuildIdx = (DWORD)atoi((char*)pData[i].Data[1]);
		wState    = (WORD)atoi((char*)pData[i].Data[2]);

		SIEGEWARFAREMGR->SetCastleGuildIndex(dwMapType, dwCastleGuildIdx);

		if( SIEGEWARFAREMGR->IsSiegeWarfareZone() && SIEGEWARFAREMGR->GetMapInfo() == dwMapType)
		{
			SIEGEWARFAREMGR->SetState(wState);
		}
	}

	SIEGEWARFAREMGR->CheckBeforeState();
	// 여기 쯤에서 만약 엠블럼 쪽이라면 워터시드 상태로 바꿔줄 것인가를 결정 해야함... 용스 홧팅!
}

void RSiegeWarWarterSeedComplete(LPQUERY pData, LPDBMESSAGE pMessage)
{
	DWORD dwResult = (DWORD)atoi((char*)pData->Data[0]);
	DWORD dwPlayerIdx = (DWORD)atoi((char*)pData->Data[1]);
	DWORD dwMaptype = (DWORD)atoi((char*)pData->Data[2]);
	DWORD dwGuildIdx = (DWORD)atoi((char*)pData->Data[3]);

	OutputDebug(
		"%s result: %d, player: %d, map: %d, guild: %d",
		__FUNCTION__,
		dwResult,
		dwPlayerIdx,
		dwMaptype,
		dwGuildIdx);

	SIEGEWARFAREMGR->UpdateWaterSeedComplateAckFromDB( dwResult , dwPlayerIdx , dwMaptype , dwGuildIdx ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RNpcRemainTime_Load
//	DESC		: 소환 된 npc 로드 결과 처리 함수 추가.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 22, 2008
//-------------------------------------------------------------------------------------------------
void RNpcRemainTime_Load(LPQUERY pData, LPDBMESSAGE pMessage)
{
	// 로드 결과를 확인한다.
	for( DWORD count = 0 ;  count < pMessage->dwResult ; ++count )
	{
		DWORD dwRecallidx	= (DWORD)atoi((char*)pData[count].Data[0]) ;
		DWORD dwChenelidx	= (DWORD)atoi((char*)pData[count].Data[1]) ;
		DWORD dwremaintick = (DWORD)atoi((char*)pData[count].Data[2]) ;

		NPCRECALLMGR->OnLoadNpc_FromDB( dwRecallidx ,dwChenelidx , dwremaintick ) ;
	}
}

void RSiegeREcallInsert(LPQUERY pData, LPDBMESSAGE pMessage)
{
	BYTE byResult = 0 ;
	byResult = (BYTE)atoi((char*)pData->Data[0]) ;

	if( byResult )	SIEGERECALLMGR->SetReadyToUseWaterSeed(TRUE) ;
	else SIEGERECALLMGR->SetReadyToUseWaterSeed(FALSE) ;
}





// 081222 LYW --- MapDBMsgParser : 캐터펄트 킬 카운트 결과를 받는 함수.
//-------------------------------------------------------------------------------------------------
//	NAME		: RRequuestKillCount
//	DESC		: 캐터펄트 킬 카운트 결과를 받는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 22, 2008
//-------------------------------------------------------------------------------------------------
void RRequuestKillCount(LPQUERY pData, LPDBMESSAGE pMessage)
{
	int nResult = (int)atoi((char*)pData->Data[0]) ;

	SIEGERECALLMGR->CheckKillCount( nResult ) ;
}

// 081027 LUJ, 공성 로그
void InsertLogSiege( eSiegeLog type, DWORD guildIndex, DWORD playerIndex, const char* memo )
{
	char text[ MAX_NAME_LENGTH + 1 ] = { 0 };
	SafeStrCpy( text, memo, sizeof( text ) );

	// 090325 ONS 필터링문자체크
	if(IsCharInString(text, "'"))	return;

	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.TP_SIEGE_LOG_INSERT %d, %d, %d, \'%s\'",
		type,
		guildIndex,
		playerIndex,
		text );
}

// 081031 LUJ, 리콜을 위해 DB에서 가져와야하는 정보를 쿼리한다
void RRecallDataLoad( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	if( 1 != dbMessage->dwResult )
	{
		return;
	}

	const DWORD				key			= dbMessage->dwID;
	const MIDDLEQUERYST&	record		= query[ 0 ];
	const DWORD				guildScore	= _ttoi( ( char* )record.Data[ 0 ] );
	const DWORD				familyPoint	= _ttoi( ( char* )record.Data[ 1 ] );

	MOVEMGR->Recall(
		key,
		guildScore,
		familyPoint );
}

// 081218 LUJ, 맵 이동 전에 필수 정보를 업데이트시킨다
void UpdatePlayerOnMapOut( DWORD playerIndex, DWORD userIndex, MONEYTYPE inventoryMoney, MONEYTYPE storageMoney, DWORD mapIndex, WORD channelIndex )
{
	g_DB.FreeMiddleQuery(
		RUpdatePlayerOnMapOut,
		playerIndex,
		// 081219 LUJ, 소유주 추
		"EXEC dbo.MP_CHARACTER_UPDATE_ON_MAPOUT %u, %u, %u, %u, %u, %d",
		playerIndex,
		userIndex,
		inventoryMoney,
		storageMoney,
		mapIndex,
		channelIndex );
}

// 081218 LUJ, 필수 정보가 처리되었으면 맵 이동시킨다
void RUpdatePlayerOnMapOut( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD				playerIndex		= dbMessage->dwID;
	const MIDDLEQUERYST&	record			= query[ 0 ];
	const BOOL				isSuccess		= atoi( ( char* )record.Data[ 0 ] );

	// 081218 LUJ, 업데이트에 실패한 경우, 맵이동을 하지 않는 편이 낫다
	if( ! isSuccess )
	{
		// 081219 LUJ, 처리 오류 시 로그를 표시하도록 함
		g_Console.LOG(
			4,
			"Error Message: MP_CHARACTER_UPDATE_ON_MAPOUT is failed. player index is %u",
			playerIndex );
		return;
	}

	const WORD	mapIndex		= WORD( atoi( ( char* )record.Data[ 1 ] ) );
	const WORD	channelIndex	= WORD( atoi( ( char* )record.Data[ 2 ] ) );

	MSG_WORD2 message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_USERCONN;
	message.Protocol	= MP_USERCONN_MAP_OUT_WITHMAPNUM;
	message.dwObjectID	= playerIndex;
	message.wData1		= mapIndex;
	message.wData2		= channelIndex;

	g_Network.Broadcast2AgentServer(
		( char* )&message,
		sizeof( message ) );
}

// 090115 LUJ, 스크립트 해킹 로그를 작성한다
void LogScriptHack( CObject& object, const TCHAR* scriptName, const TCHAR* memo )
{
	if( eObjectKind_Player != object.GetObjectKind() )
	{
		return;
	}

	// 090115 LUJ, DB에서 정의된 크기보다 긴 텍스트가 프로시저 인수가 될 경우 오류가 발생하므로 잘라준다
	char text1[ MAX_NAME_LENGTH + 1 ] = { 0 };
	SafeStrCpy(
		text1,
		scriptName,
		sizeof( text1 ) / sizeof( *text1 ) );
	char text2[ MAX_NAME_LENGTH + 1 ] = { 0 };
	SafeStrCpy(
		text2,
		memo,
		sizeof( text2 ) / sizeof( *text2 ) );

	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.TP_SCRIPT_HACK_LOG_INSERT %d, %d, '%s', '%s'",
		object.GetUserID(),
		object.GetID(),
		text1,
		text2 );
}

// 090413 ShinJS --- 챌린지 존에서 나가는 경우
void CharacterLogoutPointUpdate_ToChallengeSavedPoint( DWORD dwCharacterIdx )
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeQuery(
		eCharacterLogoutPointUpdate_ToChallengeSavedPoint,
		dwCharacterIdx,
		"EXEC dbo.MP_CHARACTER_LOGOUTPOINTUPDATE_TOCHALLENGESAVEDPOINT %d", dwCharacterIdx );
}

void UpdateVehicleToDb( const DWORD playerIndex, const ICONBASE& iconBase, const MAPTYPE mapType )
{
	g_DB.FreeMiddleQuery(
		0,
		0,
		"EXEC dbo.MP_VEHICLE_UPDATE %d, %d, %d, %d",
		playerIndex,
		iconBase.wIconIdx,
		iconBase.Position,
		mapType );
}

void LoadVehicleFromDb( const DWORD playerIndex, const MAPTYPE mapType )
{
	g_DB.FreeMiddleQuery(
		RLoadVehicleFromDb,
		playerIndex,
		"EXEC dbo.MP_VEHICLE_LOAD %d, %d",
		playerIndex,
		mapType );
}

void RLoadVehicleFromDb( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	// 090316 LUJ, 승객은 탈것 정보와 관계없이 탑승 정보를 쿼리 요청해야한다.
	const DWORD playerIndex = dbMessage->dwID;
	LoadVehiclePassengerFromDb( playerIndex, g_pServerSystem->GetMapNum() );

	if( 1 != dbMessage->dwResult )
	{
		return;
	}

	CPlayer* const player = ( CPlayer* )g_pUserTable->FindUser( playerIndex );

	if( 0 == player || player->GetObjectKind() != eObjectKind_Player )
	{
		return;
	}

	CItemSlot* const itemSlot = player->GetSlot( eItemTable_Inventory );

	if( 0 == itemSlot )
	{
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];
	const DWORD itemIndex = atoi( ( char* )record.Data[ 0 ] );
	const POSTYPE itemPosition = POSTYPE( atoi( ( char* )record.Data[ 1 ] ) );
	const DWORD passengerCount = atoi( ( char* )record.Data[ 2 ] );
	const ITEMBASE* usedItemBase = itemSlot->GetItemInfoAbs( itemPosition );
	ICONBASE iconBase = { 0 };
	iconBase.wIconIdx = itemIndex;
	iconBase.Position = itemPosition;

	// 090316 LUJ, 기간제 아이템으로 소환한 탈 것은, 해당 아이템이 없을 경우 소환하지 않는다.
	if( 0 == usedItemBase ||
		usedItemBase->wIconIdx != itemIndex )
	{
		const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo( itemIndex );

		// 090316 LUJ, 기간제 아이템으로 소환된 경우, 아이템이 없으면 소환하지 않는다.
		//			다른 슬롯에 동일한 기간제 아이템이 있을 수 있지만, 편의를 위해 고려하지 않는다
		if( 0 == itemInfo ||
			0 < itemInfo->dwUseTime )
		{
			return;
		}

		for( POSTYPE position = itemSlot->GetStartPos(); position < itemSlot->GetSlotNum(); ++position )
		{
			const ITEMBASE* const itemBase = itemSlot->GetItemInfoAbs( position );

			if( 0 == itemBase ||
				eITEM_TYPE_UNSEAL != itemBase->nSealed )
			{
				continue;
			}
			else if( itemBase->wIconIdx == itemIndex )
			{
				iconBase.dwDBIdx = itemBase->dwDBIdx;
				iconBase.Position = itemBase->Position;
				break;
			}
		}
	}
	else
	{
		iconBase.dwDBIdx = usedItemBase->dwDBIdx;
	}

	VECTOR3 playerPosition = { 0 };
	player->GetPosition( &playerPosition );

	// 091231 LUJ, 비탑승 상태에서, 탈것과 겹쳐있으면 어색해보인다
	//			이를 보정하기 위해 임의 조정한다
	if(0 == passengerCount)
	{
		playerPosition.x += float(rand() % 500 - 250);
		playerPosition.z += float(rand() % 500 - 250);
	}

	VEHICLEMGR->Summon( playerIndex, iconBase, playerPosition, FALSE );
}

void LoadVehiclePassengerFromDb( const DWORD playerIndex, const MAPTYPE mapType )
{
	g_DB.FreeMiddleQuery(
		RLoadVehiclePassengerFromDb,
		playerIndex,
		"EXEC dbo.MP_VEHICLE_PASSENGER_LOAD %d, %d",
		playerIndex,
		mapType );
}

void RLoadVehiclePassengerFromDb( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	if( 0 == dbMessage->dwResult )
	{
		return;
	}

	const DWORD autoMountedplayerIndex = dbMessage->dwID;
	CPlayer* const autoMountedPlayer = ( CPlayer* )g_pUserTable->FindUser( autoMountedplayerIndex );

	if( 0 == autoMountedPlayer || autoMountedPlayer->GetObjectKind() != eObjectKind_Player )
	{
		return;
	}

	const MIDDLEQUERYST& firstRecord = query[ 0 ];
	const DWORD vehicleOwnedPlayerIndex = atoi( ( char* )firstRecord.Data[ 0 ] );
	CPlayer* const vehicleOwnedPlayer = ( CPlayer* )g_pUserTable->FindUser( vehicleOwnedPlayerIndex );

	if( 0 == vehicleOwnedPlayer || vehicleOwnedPlayer->GetObjectKind() != eObjectKind_Player )
	{
		return;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const MIDDLEQUERYST& record = query[ i ];
		const DWORD passengerPlayerIndex = atoi( ( char* )record.Data[ 1 ] );
		const DWORD mountedSeatIndex = atoi( ( char* )record.Data[ 2 ] );

		VEHICLEMGR->MountAllow(
			passengerPlayerIndex,
			vehicleOwnedPlayer->GetSummonedVehicle(),
			mountedSeatIndex,
			FALSE );
	}
}

void ResetVehicle( MAPTYPE mapType )
{
	g_DB.FreeMiddleQuery(
		0,
		0,
		"EXEC dbo.MP_VEHICLE_RESET %d",
		mapType );
}

void RemoveVehiclePassengerToDb( const DWORD playerIndex )
{
	g_DB.FreeMiddleQuery(
		0,
		0,
		"EXEC dbo.MP_VEHICLE_PASSENGER_REMOVE %d",
		playerIndex );
}

// 090609 ShinJS --- Add AutoNote Log
void LogAutoNote( eAutoNoteLogKind LogType, CPlayer* pFromPlayer, CPlayer* pToPlayer )
{
	if( !pFromPlayer || !pToPlayer )
		return;

	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_AUTONOTELOG %d, %d, \'%s\', %d, \'%s\'", 
		LogType, 
		pFromPlayer->GetID(), 
		pFromPlayer->GetObjectName(), 
		pToPlayer->GetID(),
		pToPlayer->GetObjectName() );
}

void LoadTriggerPlayerFromDb(DWORD ownerIndex, DWORD startindex)
{
	sprintf(
		txt,
		"EXEC DBO.MP_TRIGGER_SELECT %d, %d, %d, %u",
		Trigger::eOwner_Player,
		ownerIndex,
		g_pServerSystem->GetMapNum(),
		startindex);
	g_DB.Query(eQueryType_FreeQuery, eLoadTriggerPlayerList, ownerIndex, txt);
}

void RLoadTriggerPlayerFromDb(LPQUERY pData, LPDBMESSAGE pMessage)
{
	TRIGGERMGR->CreateTrigger(
		Trigger::eOwner_Player,
		pMessage,
		pData);
}

void LoadTriggerServerFromDb(DWORD channedIndex, DWORD startindex)
{
	sprintf(
		txt,
		"EXEC DBO.MP_TRIGGER_SELECT %d, %d, %d, %u",
		Trigger::eOwner_Server,
		channedIndex,
		g_pServerSystem->GetMapNum(),
		startindex);
	g_DB.Query(eQueryType_FreeQuery, eLoadTriggerServerList, channedIndex, txt);
}

void RLoadTriggerServerFromDb(LPQUERY pData, LPDBMESSAGE pMessage)
{
	TRIGGERMGR->CreateTrigger(
		Trigger::eOwner_Server,
		pMessage,
		pData);
}

void RGetTriggerRepeatCount(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD triggerIndex = dbMessage->dwID;
	DWORD repeatCount = 0;
	struct tm regTime = {0};

	if(1 == dbMessage->dwResult)
	{
		const MIDDLEQUERYST& record = query[0];
		repeatCount = atoi(LPCTSTR(record.Data[0]));
		// MSDN 2005 이상에서는 자정(00:00:00)으로 나와 있으나 현재(09.10.28) 테스트(.Net) 결과 09:00:00 기준.
		__time64_t time = _atoi64( (LPCTSTR)record.Data[1] );
		regTime = *_localtime64(&time);
	}

	TRIGGERMGR->RestoreRepeatCount(
		triggerIndex,
		repeatCount,
		regTime);
}

void RCheckHaveFarmNumToDB(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	DWORD dwBuyOwnerIdx = dbMessage->dwID ; 
	DWORD dwResult = dbMessage->dwResult ;

	g_csFarmManager.SRV_OnCheckHaveFarmNumToDB( dwBuyOwnerIdx, dwResult );
}

// 091230 ShinJS --- PC방 아이템 지급
void RProvidePCRoomItem(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if( !pMessage->dwResult )
		return;

	WORD wResult = WORD( atoi( (char*)pData->Data[0] ) );
	switch( wResult )
	{
		// 아이템 지급
	case 1:
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pMessage->dwID );
			if( !pPlayer || pPlayer->GetObjectKind() != eObjectKind_Player )
				return;

			DWORD dwUserIdx = DWORD( atoi( (char*)pData->Data[1] ) );

			DWORD dwItemIdx[ MAX_PCROOM_PROVIDE_ITEM_NUM ] = {0,};
			DWORD dwItemCnt[ MAX_PCROOM_PROVIDE_ITEM_NUM ] = {0,};
			DWORD dwItemDBIdx[ MAX_PCROOM_PROVIDE_ITEM_NUM ] = {0,};

			dwItemIdx[0] = DWORD( atoi( (char*)pData->Data[2] ) );
			dwItemIdx[1] = DWORD( atoi( (char*)pData->Data[3] ) );
			dwItemIdx[2] = DWORD( atoi( (char*)pData->Data[4] ) );
			dwItemIdx[3] = DWORD( atoi( (char*)pData->Data[5] ) );
			dwItemIdx[4] = DWORD( atoi( (char*)pData->Data[6] ) );

			dwItemCnt[0] = DWORD( atoi( (char*)pData->Data[7] ) );
			dwItemCnt[1] = DWORD( atoi( (char*)pData->Data[8] ) );
			dwItemCnt[2] = DWORD( atoi( (char*)pData->Data[9] ) );
			dwItemCnt[3] = DWORD( atoi( (char*)pData->Data[10] ) );
			dwItemCnt[4] = DWORD( atoi( (char*)pData->Data[11] ) );

			dwItemDBIdx[0] = DWORD( atoi( (char*)pData->Data[12] ) );
			dwItemDBIdx[1] = DWORD( atoi( (char*)pData->Data[13] ) );
			dwItemDBIdx[2] = DWORD( atoi( (char*)pData->Data[14] ) );
			dwItemDBIdx[3] = DWORD( atoi( (char*)pData->Data[15] ) );
			dwItemDBIdx[4] = DWORD( atoi( (char*)pData->Data[16] ) );

			MSG_DWORD5 msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_PCROOM;
			msg.Protocol = MP_PCROOM_PROVIDE_ITEM_ACK;
			// 지급 아이템 정보
			msg.dwData1 = dwItemIdx[0];
			msg.dwData2 = dwItemIdx[1];
			msg.dwData3 = dwItemIdx[2];
			msg.dwData4 = dwItemIdx[3];
			msg.dwData5 = dwItemIdx[4];

			pPlayer->SendMsg( &msg, sizeof(msg) );

			for( int i=0 ; i<MAX_PCROOM_PROVIDE_ITEM_NUM ; ++i )
			{
				if( dwItemIdx[i] )
				{
					// 080716 LUJ, 로그. 숫자 4는 복구툴에 정의된 eItemShopLog의 enum 상수값
					g_DB.LogMiddleQuery(
						0,
						0,
						"INSERT TB_ITEM_SHOP_LOG ( TYPE, USER_IDX, USER_ID, ITEM_IDX, ITEM_DBIDX, SIZE ) VALUES ( 4, %d, '', %d, %d, %d )",
						dwUserIdx,
						dwItemIdx[i],
						dwItemDBIdx[i],
						dwItemCnt[i] );
				}
			}
		}
		break;
	}
}

void RLoadPCRoomPoint(LPMIDDLEQUERY pData, LPDBMESSAGE pMessage)
{
	if( !pMessage->dwResult )
		return;

	DWORD dwPlayerIdx		= pMessage->dwID;
	DWORD dwPCRoomPoint		= DWORD( atoi( (char*)pData->Data[0] ) );
	BOOL  bCanProvideItem	= (atoi( (char*)pData->Data[1] ) > 0 ? TRUE : FALSE);
	WORD  dwMemberCount		= WORD(atoi((char*)pData->Data[2]));
	BOOL  bProvidedBuff		= (atoi( (char*)pData->Data[3] ) > 0 ? TRUE : FALSE);
	BYTE  byDay				= BYTE( atoi( (char*)pData->Data[4] ) );
	DWORD userIndex = _ttoi(LPCTSTR(pData->Data[5]));

	PCROOMMGR->AddPlayer(
		dwPlayerIdx,
		userIndex,
		dwPCRoomPoint,
		dwMemberCount,
		bCanProvideItem,
        bProvidedBuff,
		byDay);
}

// 100105 ONS PC방buff정보 DBUpdate
// 100305 ONS 해당 맵번호를 DB에 업데이트한다. 
void DBUpdatePCRoomBuff( DWORD dwPlayerIdx, WORD wBuffState, WORD wBuffStage, BYTE byDay, WORD byLoginMap )
{
	g_DB.FreeMiddleQuery(
		0,
		dwPlayerIdx,
		"EXEC dbo.MP_GAMEROOM_UPDATE_BUFF %d, %d, %d, %d, %d",
		dwPlayerIdx,
		wBuffState,
		wBuffStage,
		byDay,
		byLoginMap);
}

//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
void FriendGetPCRoomInfo( DWORD dwPlayerIdx, DWORD dwPCRoomIdx )
{
	char txt[1024] = {0,};
	sprintf(txt, "EXEC dbo.MP_FRIEND_GETPCROOMINFO %d", dwPCRoomIdx );
	g_DB.Query( eQueryType_FreeQuery, eGetPCRoomInfo, dwPlayerIdx, txt, dwPlayerIdx );
}

// 100118 ONS 친구관리 다이얼로그에 PC방정보관련 DB처리 추가
void RGetPCRoomInfo(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if( !pMessage->dwResult )
	{
		return;
	}

	MSG_PCROOM_MEMBER_LIST msg;
	ZeroMemory(
		&msg,
		sizeof(msg));
	msg.Category = MP_PCROOM;
	msg.Protocol = MP_PCROOM_INFO_ACK;
	msg.count = BYTE(min(
		MAX_PCROOM_MEMBER_NUM,
		pMessage->dwResult));

	for( DWORD i = 0; i < msg.count; ++i )
	{
		msg.PCRoomMemList[i].CharIndex = atol((char*)pData[i].Data[0]);
		msg.PCRoomMemList[i].CharClass = atol((char*)pData[i].Data[2]);
		SafeStrCpy( msg.PCRoomMemList[i].Name, (char*)pData[i].Data[1], MAX_NAME_LENGTH + 1 );
	}

	const GameRoomType GameRoomType = PCROOMMGR->GetUserGameRoomType( pMessage->dwID );
	if( GameRoomIsInvalid == GameRoomType ) 
	{
		return;
	}

	WORD wPartyStage = PCROOMMGR->GetPartyBuffStage( msg.count, GameRoomType );
	msg.PartyStage = wPartyStage + 1;
	msg.StageMemberMin = PCROOMMGR->GetMinimumOfStage( wPartyStage, GameRoomType );

	for( DWORD nCnt = 0; nCnt < pMessage->dwResult; ++nCnt )
	{
		CObject* const object = g_pUserTable->FindUser( msg.PCRoomMemList[nCnt].CharIndex );

		if(0 == object)
		{
			continue;
		}

		object->SendMsg(
			&msg,
			sizeof(msg));
	}

	// 다른맵에 있는 PC방파티원의 리스트를 갱신한다.
	msg.Protocol	= MP_PCROOM_INFO_SYN_OTHERMAP;
	PACKEDDATA_OBJ->SendToBroadCastMapServer( &msg, sizeof(msg) );
}

// 웹이벤트
void WebEvent( DWORD dwUserIdx, DWORD type )
{
	g_DB.FreeMiddleQuery(
		0,
		dwUserIdx,
		"EXEC dbo.MP_WEB_EVENT %d, '%d'",
		dwUserIdx,
		type);
}

void Consignment_CheckDate(DWORD dwPlayerID)
{
	g_DB.FreeMiddleQuery(
		RConsignment_CheckDate,
		dwPlayerID,
		"EXEC dbo.MP_CONSIGNMENT_CHECKDATE %d",
		dwPlayerID );
}

void RConsignment_CheckDate(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD dwPlayerIndex = dbMessage->dwID;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if( ! pPlayer )
	{
		return;
	}

	DWORD dwConsignmentIndex = 0;
	for(WORD i=0; i<dbMessage->dwResult; i++)
	{
		dwConsignmentIndex = (DWORD)atoi( ( char* )query[i].Data[0] );
		Consignment_Update(pPlayer->GetID(), dwConsignmentIndex, eConsignmentUPDATEKIND_CancelByTime, 0, eITEM_TYPE_SEAL_NORMAL);
	}
}

void Consignment_GetList(DWORD dwPlayerID)
{
	sprintf(txt, "EXEC %s %d", "dbo.MP_CONSIGNMENT_GETLIST",
		dwPlayerID);
	g_DB.Query(eQueryType_FreeQuery, eConsignment_GetList, dwPlayerID, txt, dwPlayerID);
}

void RConsignment_GetList(LPQUERY pData, LPDBMESSAGE pMessage)
{
	const DWORD dwPlayerIndex = pMessage->dwID;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if( ! pPlayer )
	{
		return;
	}

	MSG_CONSIGNMENT_SEARCH_RESULT msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category = MP_CONSIGNMENT;
	msg.Protocol = MP_CONSIGNMENT_GETLIST_ACK;
	msg.dwObjectID = pPlayer->GetID();
	msg.dwCount = pMessage->dwResult;

	int nRemainMinute = 0;
	for( DWORD row = 0; row < pMessage->dwResult; ++row )
	{
		const QUERYST& record = pData[ row ];
		msg.ItemList[row].dwConsignmentIdx	= (DWORD)atoi( ( char* )record.Data[ 0 ] );
		strcpy(msg.ItemList[row].szPlayerName, ( char* )record.Data[ 1 ]);
		nRemainMinute = (DWORD)atoi( ( char* )record.Data[ 2 ] );
		if(nRemainMinute < 0)
		{
			// 날짜 지난놈은 지우자
			Consignment_Update(pPlayer->GetID(), msg.ItemList[row].dwConsignmentIdx, eConsignmentUPDATEKIND_CancelByTime, 0, eITEM_TYPE_SEAL_NORMAL);

			msg.ItemList[row].dwConsignmentIdx = 0;
			strcpy(msg.ItemList[row].szPlayerName, "");
			msg.ItemList[row].dwRemainMinute = 0;
			continue;
		}
		msg.ItemList[row].dwRemainMinute	= nRemainMinute;
		msg.ItemList[row].wInitDurability	= (WORD)atoi( ( char* )record.Data[ 3 ] );
		msg.ItemList[row].dwDeposit			= (DWORD)atoi( ( char* )record.Data[ 4 ] );
		msg.ItemList[row].dwCommission		= (DWORD)atoi( ( char* )record.Data[ 5 ] );
		msg.ItemList[row].dwUnitPrice		= (DWORD)atoi( ( char* )record.Data[ 6 ] );
		msg.dwStartPage						= (DWORD)atoi( ( char* )record.Data[ 7 ] );
		msg.dwEndPage						= (DWORD)atoi( ( char* )record.Data[ 8 ] );

		msg.ItemInfo[row].dwDBIdx			= (DWORD)atoi( ( char* )record.Data[ 9 ] );
		msg.ItemInfo[row].wIconIdx			= (DWORD)atoi( ( char* )record.Data[ 10 ] );
		msg.ItemInfo[row].Durability		= (DWORD)atoi( ( char* )record.Data[ 11 ] );
		msg.ItemInfo[row].ItemParam			= (DWORD)atoi( ( char* )record.Data[ 12 ] );
		msg.ItemInfo[row].nSealed			= ITEM_SEAL_TYPE(atoi( ( char* )record.Data[ 13 ]));
		msg.ItemInfo[row].nRemainSecond		= (int)atoi( ( char* )record.Data[ 14 ] );

		{
			ITEM_OPTION::Reinforce& data = msg.OptionInfo[row].mReinforce;
			data.mStrength			= atoi( ( char* )record.Data[ 15 ] );
			data.mDexterity			= atoi( ( char* )record.Data[ 16 ] );
			data.mVitality			= atoi( ( char* )record.Data[ 17 ] );
			data.mIntelligence		= atoi( ( char* )record.Data[ 18 ] );
			data.mWisdom			= atoi( ( char* )record.Data[ 19 ] );
			data.mLife				= atoi( ( char* )record.Data[ 20 ] );
			data.mMana				= atoi( ( char* )record.Data[ 21 ] );
			data.mLifeRecovery		= atoi( ( char* )record.Data[ 22 ] );
			data.mManaRecovery		= atoi( ( char* )record.Data[ 23 ] );
			data.mPhysicAttack		= atoi( ( char* )record.Data[ 24 ] );
			data.mPhysicDefence		= atoi( ( char* )record.Data[ 25 ] );
			data.mMagicAttack		= atoi( ( char* )record.Data[ 26 ] );
			data.mMagicDefence		= atoi( ( char* )record.Data[ 27 ] );
			data.mMoveSpeed			= atoi( ( char* )record.Data[ 28 ] );
			data.mEvade				= atoi( ( char* )record.Data[ 29 ] );
			data.mAccuracy			= atoi( ( char* )record.Data[ 30 ] );
			data.mCriticalRate		= atoi( ( char* )record.Data[ 31 ] );
			data.mCriticalDamage	= atoi( ( char* )record.Data[ 32 ] );
			SafeStrCpy( msg.OptionInfo[row].mReinforce.mMadeBy, ( char* )record.Data[ 33 ], sizeof( msg.OptionInfo[row].mReinforce.mMadeBy ) );
		}

		{
			ITEM_OPTION::Mix& data = msg.OptionInfo[row].mMix;
			data.mStrength		= atoi( ( char* )record.Data[ 34 ] );
			data.mIntelligence	= atoi( ( char* )record.Data[ 35 ] );
			data.mDexterity		= atoi( ( char* )record.Data[ 36 ] );
			data.mWisdom		= atoi( ( char* )record.Data[ 37 ] );
			data.mVitality		= atoi( ( char* )record.Data[ 38 ] );
			SafeStrCpy( msg.OptionInfo[row].mMix.mMadeBy, ( char* )record.Data[ 39 ], sizeof( msg.OptionInfo[row].mMix.mMadeBy ) );
		}

		{
			ITEM_OPTION::Enchant& data = msg.OptionInfo[row].mEnchant;

			data.mIndex	= atoi( ( char* )record.Data[ 40 ] );
			data.mLevel	= BYTE( atoi( ( char* )record.Data[ 41 ] ) );
			SafeStrCpy( msg.OptionInfo[row].mEnchant.mMadeBy, ( char* )record.Data[ 42 ], sizeof( msg.OptionInfo[row].mEnchant.mMadeBy ) );				
		}

		{
			ITEM_OPTION::Drop& data = msg.OptionInfo[row].mDrop;

			data.mValue[ 0 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 43 ] ) );
			data.mValue[ 0 ].mValue	= float( atof( ( char* )record.Data[ 44 ] ) );

			data.mValue[ 1 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 45 ] ) );
			data.mValue[ 1 ].mValue	= float( atof( ( char* )record.Data[ 46 ] ) );

			data.mValue[ 2 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 47 ] ) );
			data.mValue[ 2 ].mValue	= float( atof( ( char* )record.Data[ 48 ] ) );

			data.mValue[ 3 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 49 ] ) );
			data.mValue[ 3 ].mValue	= float( atof( ( char* )record.Data[ 50 ] ) );

			data.mValue[ 4 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 51 ] ) );
			data.mValue[ 4 ].mValue	= float( atof( ( char* )record.Data[ 52 ] ) );
		}

		msg.OptionInfo[row].mItemDbIndex = msg.ItemInfo[row].dwDBIdx;
	}
	pPlayer->SendMsg( &msg, sizeof(msg) );
}

void Consignment_Regist(DWORD dwPlayerID, DWORD dwItemDBIdx, DWORD dwItemIndex, char* pPlayerName, char* pItemName, int nCategory1, int nCategory2, int nRareLevel, int nItemLevel, int nEnchant, DWORD dwDeposit, DWORD dwCommission, DWORD dwPrice, POSTYPE FromPos, WORD wStack)
{
	g_DB.FreeMiddleQuery(
		RConsignment_Regist,
		dwPlayerID,
		"EXEC dbo.MP_CONSIGNMENT_INSERT %d, %d, %d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
		dwPlayerID, dwItemDBIdx, dwItemIndex, pPlayerName, pItemName, nCategory1, nCategory2, nRareLevel, nItemLevel, nEnchant, dwDeposit, dwCommission, dwPrice, FromPos, wStack );
}

void RConsignment_Regist(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD dwPlayerIndex = dbMessage->dwID;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if( ! pPlayer )
	{
		return;
	}

	int nError = atoi( (char*)query[0].Data[0] );
	MSG_DWORD5 msg;
	
	if(nError < 0)
	{
		msg.Category = MP_CONSIGNMENT;
		msg.Protocol = MP_CONSIGNMENT_REGIST_NACK;
		msg.dwObjectID = pPlayer->GetID();
		msg.dwData1 = (DWORD)atoi( ( char* )query[0].Data[0] );	// 성공시 ConsignmentIndex, 실패시 ErrorIndex
		msg.dwData2 = (DWORD)atoi( ( char* )query[0].Data[1] );	// ItemDBIndex
		msg.dwData3 = (DWORD)atoi( ( char* )query[0].Data[2] );	// ItemIndex
		msg.dwData4 = (DWORD)atoi( ( char* )query[0].Data[3] );	// 소모된비용
		msg.dwData5 = (DWORD)atoi( ( char* )query[0].Data[4] );	// 아이템슬롯

		CItemSlot * pFromSlot	= pPlayer->GetSlot((POSTYPE)msg.dwData5);
		if(pFromSlot)
			pFromSlot->SetLock((POSTYPE)msg.dwData5, FALSE);

		pPlayer->SendMsg( &msg, sizeof(msg) );
		return;
	}

	msg.Category = MP_CONSIGNMENT;
	msg.Protocol = MP_CONSIGNMENT_REGIST_ACK;
	msg.dwObjectID = pPlayer->GetID();
	msg.dwData1 = (DWORD)atoi( ( char* )query[0].Data[0] );	// 성공시 ConsignmentIndex, 실패시 ErrorIndex
	msg.dwData2 = (DWORD)atoi( ( char* )query[0].Data[1] );	// ItemDBIndex
	msg.dwData3 = (DWORD)atoi( ( char* )query[0].Data[2] );	// ItemIndex
	msg.dwData4 = (DWORD)atoi( ( char* )query[0].Data[3] );	// 소모된비용
	msg.dwData5 = (DWORD)atoi( ( char* )query[0].Data[4] );	// 아이템슬롯
	DWORD dwUnitPrice = (DWORD)atoi( ( char* )query[0].Data[5] );	// 아이템(개당)가격

	if(msg.Protocol == MP_CONSIGNMENT_REGIST_ACK)
	{
		// 아이템처리, 비용소모시키자.
		DURTYPE durability = 0;
		ITEMBASE DelFromItem;

		CItemSlot * pFromSlot	= pPlayer->GetSlot((POSTYPE)msg.dwData5);
		if(pFromSlot)
		{
			const ITEMBASE * pItem;
			pItem = pFromSlot->GetItemInfoAbs((POSTYPE)msg.dwData5);

			durability = pItem->Durability;
			ITEM_INFO* pInfo;
			pInfo = ITEMMGR->GetItemInfo( msg.dwData3 );
			
			ZeroMemory( &DelFromItem, sizeof( DelFromItem ) );
			
			pFromSlot->DeleteItemAbs(pPlayer, (POSTYPE)msg.dwData5, &DelFromItem, SS_LOCKOMIT);
		}

		DelFromItem.QuickPosition = 0;


		// 로그
		LogItemMoney(pPlayer->GetID(), pPlayer->GetObjectName(), 0, "*CONSIGNMENT",
			eLog_Consignment_Regist, pPlayer->GetMoney(), 0, msg.dwData4,
			msg.dwData3, msg.dwData2, (POSTYPE)msg.dwData5, 0, durability, 0 );

		DWORD dwConsignmentIdx = (DWORD)atoi( ( char* )query[0].Data[0] );
		Consignment_Log(dwConsignmentIdx, pPlayer->GetID(), eConsignmentLog_Regist, msg.dwData2, msg.dwData3, durability, dwUnitPrice);

		pPlayer->SetMoney( msg.dwData4, MONEY_SUBTRACTION, 0, eItemTable_Inventory, eMoneyLog_Consignment_Charge, 0);
	}

	pPlayer->SendMsg( &msg, sizeof(msg) );
}

void Consignment_SearchIndex(DWORD dwPlayerID, WORD wSortType, WORD wStartPage, DWORD* pItemList)
{
	if(0 == dwPlayerID || NULL==pItemList)
		return;

	sprintf(txt, "EXEC %s %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", "dbo.MP_CONSIGNMENT_SEARCHINDEX",
		dwPlayerID, wSortType, wStartPage, 
		pItemList[0], pItemList[1], pItemList[2], pItemList[3], pItemList[4], pItemList[5], pItemList[6], pItemList[7], pItemList[8], pItemList[9]);
	g_DB.Query(eQueryType_FreeQuery, eConsignment_Search, dwPlayerID, txt, dwPlayerID);
}

void Consignment_Search(DWORD dwPlayerID, WORD wSortType, WORD wStartPage, int nCategory1, int nCategory2, int nMinLevel, int nMaxLevel, int nRareLevel)
{
	sprintf(txt, "EXEC %s %d, %d, %d, %d, %d, %d, %d, %d", "dbo.MP_CONSIGNMENT_SEARCH",
		dwPlayerID, wSortType, wStartPage, nCategory1, nCategory2, nMinLevel, nMaxLevel, nRareLevel);
	g_DB.Query(eQueryType_FreeQuery, eConsignment_Search, dwPlayerID, txt, dwPlayerID);
}

void RConsignment_Search(LPQUERY pData, LPDBMESSAGE pMessage)
{
	const DWORD dwPlayerIndex = pMessage->dwID;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if( ! pPlayer )
	{
		return;
	}

	MSG_CONSIGNMENT_SEARCH_RESULT msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category = MP_CONSIGNMENT;
	msg.Protocol = MP_CONSIGNMENT_SEARCH_ACK;
	msg.dwObjectID = pPlayer->GetID();

	int nRemainMinute = 0;
	for( DWORD row = 0; row < pMessage->dwResult; ++row )
	{
		const QUERYST& record = pData[ row ];
		msg.ItemList[row].dwConsignmentIdx	= (DWORD)atoi( ( char* )record.Data[ 0 ] );
		strcpy(msg.ItemList[row].szPlayerName, ( char* )record.Data[ 1 ]);
		nRemainMinute = (DWORD)atoi( ( char* )record.Data[ 2 ] );
		msg.ItemList[row].dwRemainMinute	= nRemainMinute;
		msg.ItemList[row].wInitDurability	= (WORD)atoi( ( char* )record.Data[ 3 ] );
		msg.ItemList[row].dwDeposit			= (DWORD)atoi( ( char* )record.Data[ 4 ] );
		msg.ItemList[row].dwCommission		= (DWORD)atoi( ( char* )record.Data[ 5 ] );
		msg.ItemList[row].dwUnitPrice		= (DWORD)atoi( ( char* )record.Data[ 6 ] );
		msg.dwStartPage						= (DWORD)atoi( ( char* )record.Data[ 7 ] );
		msg.dwEndPage						= (DWORD)atoi( ( char* )record.Data[ 8 ] );

		msg.ItemInfo[row].dwDBIdx			= (DWORD)atoi( ( char* )record.Data[ 9 ] );
		msg.ItemInfo[row].wIconIdx			= (DWORD)atoi( ( char* )record.Data[ 10 ] );
		msg.ItemInfo[row].Durability		= (DWORD)atoi( ( char* )record.Data[ 11 ] );
		msg.ItemInfo[row].ItemParam			= (DWORD)atoi( ( char* )record.Data[ 12 ] );
		msg.ItemInfo[row].nSealed			= ITEM_SEAL_TYPE(atoi( ( char* )record.Data[ 13 ]));
		msg.ItemInfo[row].nRemainSecond		= (int)atoi( ( char* )record.Data[ 14 ] );

		{
			ITEM_OPTION::Reinforce& data = msg.OptionInfo[row].mReinforce;
			data.mStrength			= atoi( ( char* )record.Data[ 15 ] );
			data.mDexterity			= atoi( ( char* )record.Data[ 16 ] );
			data.mVitality			= atoi( ( char* )record.Data[ 17 ] );
			data.mIntelligence		= atoi( ( char* )record.Data[ 18 ] );
			data.mWisdom			= atoi( ( char* )record.Data[ 19 ] );
			data.mLife				= atoi( ( char* )record.Data[ 20 ] );
			data.mMana				= atoi( ( char* )record.Data[ 21 ] );
			data.mLifeRecovery		= atoi( ( char* )record.Data[ 22 ] );
			data.mManaRecovery		= atoi( ( char* )record.Data[ 23 ] );
			data.mPhysicAttack		= atoi( ( char* )record.Data[ 24 ] );
			data.mPhysicDefence		= atoi( ( char* )record.Data[ 25 ] );
			data.mMagicAttack		= atoi( ( char* )record.Data[ 26 ] );
			data.mMagicDefence		= atoi( ( char* )record.Data[ 27 ] );
			data.mMoveSpeed			= atoi( ( char* )record.Data[ 28 ] );
			data.mEvade				= atoi( ( char* )record.Data[ 29 ] );
			data.mAccuracy			= atoi( ( char* )record.Data[ 30 ] );
			data.mCriticalRate		= atoi( ( char* )record.Data[ 31 ] );
			data.mCriticalDamage	= atoi( ( char* )record.Data[ 32 ] );
			SafeStrCpy( msg.OptionInfo[row].mReinforce.mMadeBy, ( char* )record.Data[ 33 ], sizeof( msg.OptionInfo[row].mReinforce.mMadeBy ) );
		}

		{
			ITEM_OPTION::Mix& data = msg.OptionInfo[row].mMix;
			data.mStrength		= atoi( ( char* )record.Data[ 34 ] );
			data.mIntelligence	= atoi( ( char* )record.Data[ 35 ] );
			data.mDexterity		= atoi( ( char* )record.Data[ 36 ] );
			data.mWisdom		= atoi( ( char* )record.Data[ 37 ] );
			data.mVitality		= atoi( ( char* )record.Data[ 38 ] );
			SafeStrCpy( msg.OptionInfo[row].mMix.mMadeBy, ( char* )record.Data[ 39 ], sizeof( msg.OptionInfo[row].mMix.mMadeBy ) );
		}

		{
			ITEM_OPTION::Enchant& data = msg.OptionInfo[row].mEnchant;

			data.mIndex	= atoi( ( char* )record.Data[ 40 ] );
			data.mLevel	= BYTE( atoi( ( char* )record.Data[ 41 ] ) );
			SafeStrCpy( msg.OptionInfo[row].mEnchant.mMadeBy, ( char* )record.Data[ 42 ], sizeof( msg.OptionInfo[row].mEnchant.mMadeBy ) );				
		}

		{
			ITEM_OPTION::Drop& data = msg.OptionInfo[row].mDrop;

			data.mValue[ 0 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 43 ] ) );
			data.mValue[ 0 ].mValue	= float( atof( ( char* )record.Data[ 44 ] ) );

			data.mValue[ 1 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 45 ] ) );
			data.mValue[ 1 ].mValue	= float( atof( ( char* )record.Data[ 46 ] ) );

			data.mValue[ 2 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 47 ] ) );
			data.mValue[ 2 ].mValue	= float( atof( ( char* )record.Data[ 48 ] ) );

			data.mValue[ 3 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 49 ] ) );
			data.mValue[ 3 ].mValue	= float( atof( ( char* )record.Data[ 50 ] ) );

			data.mValue[ 4 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 51 ] ) );
			data.mValue[ 4 ].mValue	= float( atof( ( char* )record.Data[ 52 ] ) );
		}

		msg.dwCount = (DWORD)atoi( ( char* )record.Data[ 53 ] );
		msg.OptionInfo[row].mItemDbIndex = msg.ItemInfo[row].dwDBIdx;
		msg.ItemList[row].dwAverageSellPrice = (DWORD)atoi( ( char* )record.Data[ 54 ] );
	}
	pPlayer->SendMsg( &msg, sizeof(msg) );
}

void Consignment_Update(DWORD dwPlayerID, DWORD dwConsignmentIdx, WORD wKind, DWORD dwDurability, ITEM_SEAL_TYPE wSeal)
{
	g_DB.FreeMiddleQuery(
		RConsignment_Update,
		dwPlayerID,
		"EXEC dbo.MP_CONSIGNMENT_UPDATE %d, %d, %d, %d, %d",
		dwPlayerID, dwConsignmentIdx, wKind, dwDurability, wSeal);
}

void RConsignment_Update(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD dwPlayerIndex = dbMessage->dwID;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if( ! pPlayer )
	{
		return;
	}

	int nResult = atoi( (char*)query[0].Data[0] );

	MSG_CONSIGNMENT_UPDATERESULT msg;
	ZeroMemory(&msg, sizeof(msg));

	switch(nResult)
	{
	case eConsignmentDBResult_UserCancel_Ack:
		{
			DWORD dwItemDBIdx = (DWORD)atoi( (char*)query[0].Data[2] );
			DWORD dwItemIndex = (DWORD)atoi( (char*)query[0].Data[3] );
			DURTYPE Durability = (DURTYPE)atoi( (char*)query[0].Data[4] );
			DWORD dwUnitPrice = (DWORD)atoi( (char*)query[0].Data[5] );
			DWORD dwConsignmentIdx = (DWORD)atoi( (char*)query[0].Data[6] );

			// 로그 (유저캔슬)
			LogItemMoney(pPlayer->GetID(), pPlayer->GetObjectName(), 0, "*CONSIGNMENT",
			eLog_Note_Consignmnet_UserCancel, 0, 0, 0,
			dwItemIndex, dwItemDBIdx, 0, 0, Durability, 0 );

			Consignment_Log(dwConsignmentIdx, pPlayer->GetID(), eConsignmentLog_UserCancel, dwItemDBIdx, dwItemIndex, Durability, dwUnitPrice);

			msg.Category = MP_CONSIGNMENT;
			msg.Protocol = MP_CONSIGNMENT_UPDATE_ACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.nResult = nResult;
			SafeStrCpy(msg.szItemName, (char*)query[0].Data[1], sizeof(msg.szItemName));
			pPlayer->SendMsg( &msg, sizeof(msg) );

			// DB에서 우편으로 발송했다. 우편발송 메시지 보내자.
			MSGBASE msgNote;
			msgNote.Category = MP_NOTE;
			msgNote.Protocol = MP_NOTE_RECEIVENOTE;
			msgNote.dwObjectID = pPlayer->GetID();
			g_Network.Broadcast2AgentServer( (char*)&msgNote, sizeof(msgNote) );
		}
		break;

	case eConsignmentDBResult_TimeCancel_Ack:
		{
			DWORD dwItemDBIdx = (DWORD)atoi( (char*)query[0].Data[2] );
			DWORD dwItemIndex = (DWORD)atoi( (char*)query[0].Data[3] );
			DURTYPE Durability = (DURTYPE)atoi( (char*)query[0].Data[4] );
			DWORD dwUnitPrice = (DWORD)atoi( (char*)query[0].Data[5] );
			DWORD dwConsignmentIdx = (DWORD)atoi( (char*)query[0].Data[6] );

			// 로그 (기간경과)
			LogItemMoney(pPlayer->GetID(),  pPlayer->GetObjectName(), 0, "*CONSIGNMENT",
			eLog_Note_Consignmnet_TimeCancel, 0, 0, 0,
			dwItemIndex, dwItemDBIdx, 0, 0, Durability, 0 );

			Consignment_Log(dwConsignmentIdx, pPlayer->GetID(), eConsignmentLog_TimeCancel, dwItemDBIdx, dwItemIndex, Durability, dwUnitPrice);

			msg.Category = MP_CONSIGNMENT;
			msg.Protocol = MP_CONSIGNMENT_UPDATE_ACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.nResult = nResult;
			SafeStrCpy(msg.szItemName, (char*)query[0].Data[1], sizeof(msg.szItemName));
			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
		break;

	case eConsignmentDBResult_Buy_Ack:
		{
			// 원본ItemIndex, 원본 ItemDBIdx, 구매자이름, 구매수량, (개당)구매금액, 완전판매여부
			DWORD dwSrcItemIndex	= (DWORD)atoi( (char*)query[0].Data[1] );
			DWORD dwSrcItemDBidx	= (DWORD)atoi( (char*)query[0].Data[2] );
			SafeStrCpy(msg.szItemName, (char*)query[0].Data[3], sizeof(msg.szItemName));
			DWORD dwDurability		= (DWORD)atoi( (char*)query[0].Data[4] );
			DWORD dwUnitPrice		= (DWORD)atoi( (char*)query[0].Data[5] );
			DWORD dwSellCharIndex	= (DWORD)atoi( (char*)query[0].Data[6] );
			DWORD dwSoldOut			= (DWORD)atoi( (char*)query[0].Data[7] );
			DWORD dwNewDBIndex		= (DWORD)atoi( (char*)query[0].Data[8] ); // 부분판매시 우편발송되는 새아이템의 DB인덱스
			char szSellCharName[MAX_NAME_LENGTH + 1] = {0,};
			SafeStrCpy(szSellCharName, (char*)query[0].Data[9], sizeof(szSellCharName));
			DWORD dwObtainMoney		= (DWORD)atoi( (char*)query[0].Data[10] );
			DWORD dwConsignmentIdx = (DWORD)atoi( (char*)query[0].Data[11] );

			if(dwSoldOut)
			{
				// 로그 (구입)
				LogItemMoney(pPlayer->GetID(),  pPlayer->GetObjectName(), 0, "*CONSIGNMENT", 
					eLog_Note_Consignmnet_Buy, 0, pPlayer->GetMoney(), dwDurability ?  dwDurability * dwUnitPrice : dwUnitPrice,
					dwSrcItemIndex, dwSrcItemDBidx, 0, 0, (DURTYPE)dwDurability, 0 );

				Consignment_Log(dwConsignmentIdx, pPlayer->GetID(), eConsignmentLog_Buy, dwSrcItemDBidx, dwSrcItemIndex, dwDurability, dwUnitPrice);


				// 로그 (영수증)
				LogItemMoney(dwSellCharIndex, szSellCharName, 0, "*CONSIGNMENT",
					eLog_Note_Consignmnet_Receipt, 0, 0, dwObtainMoney,
					0, 0, 0, 0, (DURTYPE)dwDurability, 0 );

				Consignment_Log(dwConsignmentIdx, dwSellCharIndex, eConsignmentLog_SoldOut, dwSrcItemDBidx, dwSrcItemIndex, dwDurability, dwUnitPrice);

				// for 구매자
				msg.Category = MP_CONSIGNMENT;
				msg.Protocol = MP_CONSIGNMENT_UPDATE_ACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.nResult = nResult;
				msg.dwTotalPrice = dwDurability ?  dwDurability * dwUnitPrice : dwUnitPrice;
				pPlayer->SendMsg( &msg, sizeof(msg) );

				// 모두판매 - 소스아이템을 우편으로 발송했다.
				MSGBASE msgNote;
				msgNote.Category = MP_NOTE;
				msgNote.Protocol = MP_NOTE_RECEIVENOTE;
				msgNote.dwObjectID = dwPlayerIndex;
				g_Network.Broadcast2AgentServer( (char*)&msgNote, sizeof(msgNote) );

				// for 판매자
				MSGBASE msgReceipt;
				msgReceipt.Category = MP_NOTE;
				msgReceipt.Protocol = MP_NOTE_RECEIVENOTE;
				msgReceipt.dwObjectID = dwSellCharIndex;
				g_Network.Broadcast2AgentServer( (char*)&msgReceipt, sizeof(msgReceipt) );
			}
			else
			{
				// 로그 (부분구입)
				LogItemMoney(dwSellCharIndex, szSellCharName, 0, "*CONSIGNMENT",
					eLog_Note_Consignmnet_SellUpdate, 0, 0, dwDurability ?  dwDurability * dwUnitPrice : dwUnitPrice,
					dwSrcItemIndex, dwSrcItemDBidx, 0, 0, (DURTYPE)dwDurability, 0 );

				Consignment_Log(dwConsignmentIdx, dwSellCharIndex, eConsignmentLog_SellUpdate, dwSrcItemDBidx, dwSrcItemIndex, dwDurability, dwUnitPrice);

				LogItemMoney(pPlayer->GetID(),  pPlayer->GetObjectName(), 0, "*CONSIGNMENT", 
					eLog_Note_Consignmnet_Buy, 0, pPlayer->GetMoney(), dwDurability ?  dwDurability * dwUnitPrice : dwUnitPrice,
					dwSrcItemIndex, dwNewDBIndex, 0, 0, (DURTYPE)dwDurability, 0 );

				Consignment_Log(dwConsignmentIdx, pPlayer->GetID(), eConsignmentLog_Buy, dwNewDBIndex, dwSrcItemIndex, dwDurability, dwUnitPrice);

				// for 구매자
				msg.Category = MP_CONSIGNMENT;
				msg.Protocol = MP_CONSIGNMENT_UPDATE_ACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.nResult = nResult;
				msg.dwTotalPrice = dwDurability ?  dwDurability * dwUnitPrice : dwUnitPrice;
				pPlayer->SendMsg( &msg, sizeof(msg) );

				// 부분판매 - 새 아이템을 생성해서 우편으로 발송했다.
				// 옵션은 없는 수량성 아이템이다.
				MSGBASE msgNote;
				msgNote.Category = MP_NOTE;
				msgNote.Protocol = MP_NOTE_RECEIVENOTE;
				msgNote.dwObjectID = dwPlayerIndex;
				g_Network.Broadcast2AgentServer( (char*)&msgNote, sizeof(msgNote) );
			}

			// 구입금액소모
			pPlayer->SetMoney(dwUnitPrice*(dwDurability ? dwDurability : 1), MONEY_SUBTRACTION, 0, eItemTable_Inventory, eMoneyLog_Consignment_Charge, 0);

		}
		break;

	case eConsignmentDBResult_RegistedOver:
	case eConsignmentDBResult_DurabilityErr:
	case eConsignmentDBResult_PoorMoney:
	case eConsignmentDBResult_Err:
		{
			msg.Category = MP_CONSIGNMENT;
			msg.Protocol = MP_CONSIGNMENT_UPDATE_NACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.nResult = nResult;
			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
		break;
	default:
		{
			msg.Category = MP_CONSIGNMENT;
			msg.Protocol = MP_CONSIGNMENT_UPDATE_NACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.nResult = nResult;
			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
		break;
	}
}

void Note_CheckDate(DWORD dwPlayerID)
{
	g_DB.FreeMiddleQuery(
		RNote_CheckDate,
		dwPlayerID,
		"EXEC dbo.MP_NOTE_CHECKDATE %d",
		dwPlayerID );
}

void RNote_CheckDate(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD dwPlayerIndex = dbMessage->dwID;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if( ! pPlayer )
	{
		return;
	}

	DWORD dwNoteIndex = 0;
	DWORD dwPackageItemIdx = 0;
	DWORD dwPackageMoney = 0;
	DWORD dwConsignmentIdx = 0;
	char szTitle[40] = {0,};
	
	for(WORD i=0; i<dbMessage->dwResult; i++)
	{
		dwNoteIndex = (DWORD)atoi( ( char* )query[i].Data[0] );
		dwPackageItemIdx = (DWORD)atoi( ( char* )query[i].Data[1] );
		dwPackageMoney = (DWORD)atoi( ( char* )query[i].Data[2] );
		// 우편제목에서 판매대행인덱스만 추출하자.
		SafeStrCpy( szTitle, ( char* )query[i].Data[3], sizeof( szTitle ) );
		char* pIndex = &szTitle[9];
		dwConsignmentIdx = (DWORD)atoi(pIndex);

		Note_DelByDate(pPlayer->GetID(), dwNoteIndex, dwPackageItemIdx, dwPackageMoney);
		Consignment_Log(dwConsignmentIdx, pPlayer->GetID(), eConsignmentLog_DelByDate, dwPackageItemIdx, 0, 0, dwPackageMoney);
	}
}

void Note_DelByDate(DWORD dwPlayerID, DWORD dwNoteIndex, DWORD dwPackageItemIdx, DWORD dwPackageMoney)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerID);
	if( ! pPlayer )
	{
		return;
	}

	g_DB.FreeMiddleQuery(
		0,
		0,
		"EXEC dbo.MP_NOTE_DELNOTE %d, %d, %d, %d",
		dwPlayerID, FALSE, dwNoteIndex, 0);

	if(dwPackageItemIdx || dwPackageMoney)
	{
		// 지워지는 우편중 소포가 있는경우만 로그를 남긴다.
		// 지워지는 우편에 대한 아이템정보는 DBIndex밖에 모른다.
		// 아이템정보를 조회하려면 eLog_Note_Obtain_NewItem(우편함지급) 로그를 찾아보자.
		LogItemMoney(pPlayer->GetID(),  pPlayer->GetObjectName(), 0, "*NOTE", 
			eLog_Note_TimeDelete, 0, pPlayer->GetMoney(), dwPackageMoney,
			0, dwPackageItemIdx, 0, 0, 0, 0 );
	}
}

void Note_GetPackage(DWORD dwPlayerID, DWORD dwNoteID, WORD wEmptySlot, DWORD dwItemDBIdx, DWORD dwMoney)
{
	sprintf(txt, "EXEC %s %d, %d, %d, %d, %d", "dbo.[MP_NOTE_GETPACKAGE]",
		dwPlayerID, dwNoteID, wEmptySlot, dwItemDBIdx, dwMoney);
	g_DB.Query(eQueryType_FreeQuery, eNote_GetPackage, dwPlayerID, txt, dwPlayerID);
}

void RNote_GetPackage(LPQUERY pData, LPDBMESSAGE pMessage)
{
	const DWORD dwPlayerIndex = pMessage->dwID;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if( ! pPlayer )
	{
		return;
	}

	if(pMessage->dwResult != 1)
		return;

	DWORD dwMoney = 0;
	DWORD dwItemDBIndex = 0;
	DWORD dwItemIndex = 0;
	DWORD dwItemDurabilty = 0;
	DWORD dwNoteIndex = 0;
	DWORD dwCalcDurability = 0;

	char szTitle[40] = {0,};
	DWORD dwConsignmentIdx = 0;

	ITEMBASE ItemBase;
	ZeroMemory(&ItemBase, sizeof(ItemBase));
	ITEM_OPTION option;

	const QUERYST& record = pData[ 0 ];
	int nCheck = atoi( ( char* )record.Data[ 0 ] );
	if(nCheck < 0)
	{
		MSG_DWORD msg;
		msg.Category = MP_NOTE;
		msg.Protocol = MP_NOTE_GETPACKAGE_NACK;
		msg.dwObjectID = pPlayer->GetID();
		msg.dwData = (DWORD)nCheck;
		pPlayer->SendMsg(&msg, sizeof(msg));
		return;
	}
	else if(nCheck == 1)
	{
		dwMoney	= (DWORD)atoi( ( char* )record.Data[ 1 ] );
		ItemBase.dwDBIdx = atoi((char*)record.Data[2]);
		dwItemDBIndex = ItemBase.dwDBIdx;
		ItemBase.wIconIdx = atoi((char*)record.Data[3]);
		dwItemIndex = ItemBase.wIconIdx;
		ItemBase.Position = POSTYPE( atoi((char*)record.Data[4]) );
		ItemBase.QuickPosition = POSTYPE( atoi((char*)record.Data[5]) );
		ItemBase.Durability = atoi((char*)record.Data[6]);
		dwItemDurabilty = ItemBase.Durability;
		ItemBase.ItemParam = atoi((char*)record.Data[7]);
		ItemBase.nSealed = (ITEM_SEAL_TYPE)atoi((char*)record.Data[8]);

		ITEM_INFO* ItemInfo = ITEMMGR->GetItemInfo( ItemBase.wIconIdx );
		if( ItemInfo == NULL )
			return;

		if(0 == ItemInfo->Stack)
		{
			{
				ITEM_OPTION::Reinforce& data = option.mReinforce;

				data.mStrength			= atoi( ( char* )record.Data[ 10 ] );
				data.mDexterity			= atoi( ( char* )record.Data[ 11 ] );
				data.mVitality			= atoi( ( char* )record.Data[ 12 ] );
				data.mIntelligence		= atoi( ( char* )record.Data[ 13 ] );
				data.mWisdom			= atoi( ( char* )record.Data[ 14 ] );
				data.mLife				= atoi( ( char* )record.Data[ 15 ] );
				data.mMana				= atoi( ( char* )record.Data[ 16 ] );
				data.mLifeRecovery		= atoi( ( char* )record.Data[ 17 ] );
				data.mManaRecovery		= atoi( ( char* )record.Data[ 18 ] );
				data.mPhysicAttack		= atoi( ( char* )record.Data[ 19 ] );
				data.mPhysicDefence		= atoi( ( char* )record.Data[ 20 ] );
				data.mMagicAttack		= atoi( ( char* )record.Data[ 21 ] );
				data.mMagicDefence		= atoi( ( char* )record.Data[ 22 ] );
				data.mMoveSpeed			= atoi( ( char* )record.Data[ 23 ] );
				data.mEvade				= atoi( ( char* )record.Data[ 24 ] );
				data.mAccuracy			= atoi( ( char* )record.Data[ 25 ] );
				data.mCriticalRate		= atoi( ( char* )record.Data[ 26 ] );
				data.mCriticalDamage	= atoi( ( char* )record.Data[ 27 ] );
				SafeStrCpy( option.mReinforce.mMadeBy, ( char* )record.Data[ 28 ], sizeof( option.mReinforce.mMadeBy ) );
			}

			{
				ITEM_OPTION::Mix& data = option.mMix;

				data.mStrength		= atoi( ( char* )record.Data[ 29 ] );
				data.mIntelligence	= atoi( ( char* )record.Data[ 30 ] );
				data.mDexterity		= atoi( ( char* )record.Data[ 31 ] );
				data.mWisdom		= atoi( ( char* )record.Data[ 32 ] );
				data.mVitality		= atoi( ( char* )record.Data[ 33 ] );
				SafeStrCpy( option.mMix.mMadeBy, ( char* )record.Data[ 34 ], sizeof( option.mMix.mMadeBy ) );
			}

			{
				ITEM_OPTION::Enchant& data = option.mEnchant;

				data.mIndex	= atoi( ( char* )record.Data[ 35 ] );
				data.mLevel	= BYTE( atoi( ( char* )record.Data[ 36 ] ) );
				SafeStrCpy( option.mEnchant.mMadeBy, ( char* )record.Data[ 37 ], sizeof( option.mEnchant.mMadeBy ) );				
			}

			{
				ITEM_OPTION::Drop& data = option.mDrop;

				data.mValue[ 0 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 38 ] ) );
				data.mValue[ 0 ].mValue	= float( atof( ( char* )record.Data[ 39 ] ) );

				data.mValue[ 1 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 40 ] ) );
				data.mValue[ 1 ].mValue	= float( atof( ( char* )record.Data[ 41 ] ) );

				data.mValue[ 2 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 42 ] ) );
				data.mValue[ 2 ].mValue	= float( atof( ( char* )record.Data[ 43 ] ) );

				data.mValue[ 3 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 44 ] ) );
				data.mValue[ 3 ].mValue	= float( atof( ( char* )record.Data[ 45 ] ) );

				data.mValue[ 4 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 46 ] ) );
				data.mValue[ 4 ].mValue	= float( atof( ( char* )record.Data[ 47 ] ) );
			}

			option.mItemDbIndex	= ItemBase.dwDBIdx;
			ITEMMGR->AddOption( option );
		}
		dwNoteIndex = (DWORD)atoi( ( char* )record.Data[ 48 ] );
		// 우편제목에서 판매대행인덱스만 추출하자.
		SafeStrCpy( szTitle, ( char* )record.Data[ 49 ], sizeof( szTitle ) );
		char* pIndex = &szTitle[9];
		dwConsignmentIdx = (DWORD)atoi(pIndex);

		DWORD dwError = eNoteErr_None;
		{
			CItemSlot* slot = pPlayer->GetSlot( eItemTable_Inventory );
			if(NULL == slot)
				dwError = eNoteErr_WrongState;
			
			CBackupSlot backupSlot( *slot );

			// 아이템 삽입
			BOOL				bCombined		= FALSE;
			CItemSlot*			pSlot			= pPlayer->GetSlot(eItemTable_Inventory);
			const POSTYPE		StartPosition	= POSTYPE( TP_INVENTORY_START );
			const POSTYPE		EndPosition		= POSTYPE( TP_INVENTORY_END + pPlayer->GetInventoryExpansionSize() );
			const ITEM_INFO*	pInfo			= ITEMMGR->GetItemInfo( ItemBase.wIconIdx );
			if(NULL == pInfo)
				dwError = eNoteErr_WrongState;

			if(pInfo->Stack)
			{
				dwCalcDurability = ItemBase.Durability;
				const WORD wItemStackNum = ITEMMGR->GetItemStackNum( ItemBase.wIconIdx );

				for(POSTYPE i = StartPosition; i<EndPosition; i++)
				{
					if(0 == dwCalcDurability)
						break;

					const ITEMBASE* item = pSlot->GetItemInfoAbs(i);

					if( NULL==item || 0==item->dwDBIdx || item->wIconIdx!=ItemBase.wIconIdx	|| wItemStackNum==item->Durability )
						continue;
					else if(pInfo->wSeal != item->nSealed)
					{
						continue;
					}

					const DURTYPE enableSize = wItemStackNum - item->Durability;

					if( EI_TRUE != pSlot->UpdateItemAbs(pPlayer, item->Position, item->dwDBIdx, item->wIconIdx, item->Position, item->QuickPosition,
						item->Durability + min( enableSize, dwCalcDurability ), UB_DURA, SS_CHKDBIDX ) )
					{
						continue;
					}
					
					dwCalcDurability -= min( enableSize, dwCalcDurability );
				}

				if(dwCalcDurability)
				{
					ITEMBASE item;
					memcpy(&item, &ItemBase, sizeof(item));
					item.Durability = dwCalcDurability;

					if(EI_TRUE != pSlot->InsertItemAbs(pPlayer, item.Position, &item))
					{
						dwError = eNoteErr_InsertFail;
					}
				}
				else
				{
					bCombined = TRUE;
				}
			}
			else
			{
				if(EI_TRUE != pSlot->InsertItemAbs(pPlayer, ItemBase.Position, &ItemBase))
				{
					dwError = eNoteErr_InsertFail;
				}
			}

			if(eNoteErr_None < dwError)
			{
				// 에러 DB의 우편함 아이템 복구 시키자.
				backupSlot.Restore();

				g_DB.FreeMiddleQuery(
				0,
				0,
				"EXEC dbo.MP_NOTE_RESTOREPACKAGE %d, %d, %d, %d",
				pPlayer->GetID(), dwNoteIndex, dwItemDBIndex, dwMoney);

				// 실패로그
				LogItemMoney(pPlayer->GetID(),  pPlayer->GetObjectName(), 0, "*NOTE(Restored)",
					eLog_Note_Obtain_Fail, 0, pPlayer->GetMoney(), 0,
					ItemBase.wIconIdx, ItemBase.dwDBIdx, 0, 0, ItemBase.Durability, 0 );

				Consignment_Log(dwConsignmentIdx, pPlayer->GetID(), eConsignmentLog_GetPackageFail, dwItemDBIndex, dwItemIndex, dwItemDurabilty, dwMoney);
				return;
			}

			// 성공 : 아이템 갱신 or 추가
			if(0 == ItemInfo->Stack)
			{
				ItemUpdateToDB(pPlayer->GetID(), ItemBase.dwDBIdx, ItemBase.wIconIdx, ItemBase.Durability, ItemBase.Position, ItemBase.QuickPosition, ItemBase.ItemParam);
				// 성공로그
				LogItemMoney(pPlayer->GetID(),  pPlayer->GetObjectName(), 0, "*NOTE",
					eLog_Note_Obtain_Success, 0, pPlayer->GetMoney(), 0,
					ItemBase.wIconIdx, ItemBase.dwDBIdx, 0, ItemBase.Position, ItemBase.Durability, 0 );

				// 클라에 아이템기본정보 보내기
				ITEMOBTAINARRAYINFO* const pArrayInfo = ITEMMGR->Alloc(pPlayer, MP_NOTE, MP_NOTE_GETPACKAGE_ACK, pPlayer->GetID(), ItemBase.dwDBIdx, eLog_Note_Obtain_Success, (WORD)ItemBase.Durability, NULL);
				pArrayInfo->ItemArray.AddItem(ItemBase.dwDBIdx, ItemBase.wIconIdx, ItemBase.Durability, ItemBase.Position, ItemBase.QuickPosition, 0, 0, ItemBase.nSealed);
				pPlayer->SendMsg( &pArrayInfo->ItemArray, pArrayInfo->ItemArray.GetSize() );

				// 클라에 아이템옵션정보 보내기
				ITEMMGR->AddOption(option);
				MSG_ITEM_OPTION optionMessage;
				{
					ZeroMemory( &optionMessage, sizeof( optionMessage ) );

					optionMessage.Category	= MP_ITEM;
					optionMessage.Protocol	= MP_ITEM_OPTION_ADD_ACK;
					optionMessage.mSize		= 1;
					memcpy(&optionMessage.mOption[0], &option, sizeof(ITEM_OPTION));
				}
				pPlayer->SendMsg( &optionMessage, optionMessage.GetSize() );
				ITEMMGR->Free(pPlayer, pArrayInfo);
			}
			else
			{
				ITEMOBTAINARRAYINFO* const pArrayInfo = ITEMMGR->Alloc(pPlayer, MP_NOTE, MP_NOTE_GETPACKAGE_ACK, pPlayer->GetID(), ItemBase.dwDBIdx, eLog_Note_Obtain_Success, (WORD)ItemBase.Durability, NULL);

				for(POSTYPE i = StartPosition; i<EndPosition; i++)
				{
					const ITEMBASE*	item = pSlot->GetItemInfoAbs( i );
					const ITEMBASE*	backupItem = backupSlot.GetItem( i );

					if(NULL==item && NULL==backupItem)
						continue;
					else if(NULL == item)
					{
						continue;
					}
					else if(NULL==backupItem && item->dwDBIdx==0)
					{
						continue;
					}
					else if(NULL==backupItem && 0<item->dwDBIdx)
					{
						ItemUpdateToDB(pPlayer->GetID(), ItemBase.dwDBIdx, ItemBase.wIconIdx, dwCalcDurability, ItemBase.Position, ItemBase.QuickPosition, ItemBase.ItemParam);
						// 성공로그
						LogItemMoney(pPlayer->GetID(),  pPlayer->GetObjectName(), 0, "*NOTE",
							eLog_Note_Obtain_Success, 0, pPlayer->GetMoney(), 0,
							ItemBase.wIconIdx, ItemBase.dwDBIdx, 0, ItemBase.Position, dwCalcDurability, 0 );

						pArrayInfo->ItemArray.AddItem(ItemBase.dwDBIdx, ItemBase.wIconIdx, dwCalcDurability, ItemBase.Position, ItemBase.QuickPosition, 0, 0, ItemBase.nSealed);
					}
					else if(item->dwDBIdx==backupItem->dwDBIdx && item->Durability!=backupItem->Durability)
					{
						ItemUpdateToDB(pPlayer->GetID(), item->dwDBIdx, item->wIconIdx, item->Durability, item->Position, item->QuickPosition, item->ItemParam);
						// 성공로그
						LogItemMoney(pPlayer->GetID(),  pPlayer->GetObjectName(), 0, "*NOTE",
							eLog_Note_Obtain_Success, 0, pPlayer->GetMoney(), 0,
							item->wIconIdx, item->dwDBIdx, 0, item->Position, item->Durability, 0 );

						pArrayInfo->ItemArray.AddItem(item->dwDBIdx, item->wIconIdx, item->Durability, item->Position, item->QuickPosition, 0, 0, item->nSealed);
					}
					else
					{
						continue;
					}
				}

				if(bCombined)
				{
					// 합쳤으니 기존아이템 삭제
					ItemDeleteToDB(ItemBase.dwDBIdx);
					LogItemMoney(pPlayer->GetID(),  pPlayer->GetObjectName(), 0, "*NOTE",
						eLog_Note_CombineDelete, 0, pPlayer->GetMoney(), 0,
						ItemBase.wIconIdx, ItemBase.dwDBIdx, 0, ItemBase.Position, ItemBase.Durability, 0 );
				}

				pPlayer->SendMsg( &pArrayInfo->ItemArray, pArrayInfo->ItemArray.GetSize() );
				ITEMMGR->Free(pPlayer, pArrayInfo);
			}
		}
	}
	else if(nCheck == 2)
	{
		dwMoney	= (DWORD)atoi( ( char* )record.Data[ 1 ] );
		// 우편제목에서 판매대행인덱스만 추출하자.
		SafeStrCpy( szTitle, ( char* )record.Data[ 2 ], sizeof( szTitle ) );
		char* pIndex = &szTitle[9];
		dwConsignmentIdx = (DWORD)atoi(pIndex);
	}

	// 골드지급
	if(dwMoney)
	{
		// 로그
		LogItemMoney(pPlayer->GetID(),  pPlayer->GetObjectName(), 0, "*NOTE",
			eLog_Note_Obtain_Money, 0, pPlayer->GetMoney(), dwMoney,
			0, 0, 0, 0, 0, 0 );

		pPlayer->SetMoney( dwMoney, MONEY_ADDITION, MF_OBTAIN, eItemTable_Inventory, eMoneyLog_MoneyFromNote, 0);
	}

	if(dwConsignmentIdx)
	{
		Consignment_Log(dwConsignmentIdx, pPlayer->GetID(), eConsignmentLog_GetPackage, dwItemDBIndex, dwItemIndex, dwItemDurabilty, dwMoney);
	}

	MSG_DWORD4 msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_GETPACKAGE_NOTIFY;
	msg.dwObjectID = pPlayer->GetID();
	msg.dwData1 = dwItemDBIndex;
	msg.dwData2 = dwMoney;
	msg.dwData3 = dwItemIndex;
	msg.dwData4 = dwItemDurabilty;
	pPlayer->SendMsg(&msg, sizeof(msg));
}

void Consignment_Log(DWORD dwConsignmentIdx, DWORD dwPlayerID, int nKind, DWORD dwDBItemIdx, DWORD dwItemIndex, DWORD dwChangeDurability, DWORD dwPrice)
{
	g_DB.LogMiddleQuery(
		NULL,
		dwPlayerID,
		"EXEC dbo.TP_CONSIGNMENT_LOG %d, %d, %d, %d, %d, %d, %d",
		dwConsignmentIdx, dwPlayerID, nKind, dwDBItemIdx, dwItemIndex, dwChangeDurability, dwPrice);
}

void ItemInsertToNote(DWORD dwPlayerID, DWORD dwItemIndex, DWORD dwDurability, ITEM_SEAL_TYPE wSeal, DWORD dwMoney, WORD wKind, DWORD dwNameMsgIdx, DWORD dwTitleMsgIdx, char* pParam)
{
	ITEM_INFO* ItemInfo = ITEMMGR->GetItemInfo( dwItemIndex );
	if( ItemInfo == NULL )
			return;

	g_DB.FreeMiddleQuery(
		RItemInsertToNote,
		dwPlayerID,
		"EXEC dbo.MP_ITEM_INSERT_TO_NOTE %d, %d, %d, %d, %d, %d, %d, %d, '%s'",
		dwPlayerID, dwItemIndex, dwDurability, wSeal, dwMoney, wKind, dwNameMsgIdx, dwTitleMsgIdx, pParam );
}

void RItemInsertToNote(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD dwPlayerIndex = dbMessage->dwID;

	if(dbMessage->dwResult != 1)
		return;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerIndex);
	if( ! pPlayer )
	{
		return;
	}

	DWORD dwResult = (DWORD)atoi( ( char* )query[0].Data[0] );
	DWORD dwNewDBIndex = (DWORD)atoi( ( char* )query[0].Data[1] );
	DWORD dwItemIndex = (DWORD)atoi( ( char* )query[0].Data[2] );
	DURTYPE durability = (DWORD)atoi( ( char* )query[0].Data[3] );
	DWORD dwObtainMoney = (DWORD)atoi( ( char* )query[0].Data[4] );
	if(dwResult)
	{
		// 로그
		LogItemMoney(pPlayer->GetID(), pPlayer->GetObjectName(), 0, "*NOTE",
			eLog_Note_SendItemFromNPC, 0, pPlayer->GetMoney(), dwObtainMoney,
			dwItemIndex, dwNewDBIndex, 0, 0, durability, 0 );

		MSGBASE msgNote;
		msgNote.Category = MP_NOTE;
		msgNote.Protocol = MP_NOTE_RECEIVENOTE;
		msgNote.dwObjectID = dwPlayerIndex;
		g_Network.Broadcast2AgentServer( (char*)&msgNote, sizeof(msgNote) );
	}
}

// 100601 ONS 채팅금지 대상을 DB에 추가한다.
void ForbidChatUpdate( DWORD dwPlayerIdx, DWORD dwTargetIdx, const char* pReason, DWORD dwForbidTime, BOOL bIsForbidAdd )
{
	char txt[1024] = {0,};
	sprintf(txt, "EXEC dbo.MP_GM_FORBIDCHAT_UPDATE %d, %d, \'%s\', %d, %d", dwPlayerIdx, dwTargetIdx, pReason, dwForbidTime, (byte)bIsForbidAdd );
	g_DB.Query( eQueryType_FreeQuery, eForbidChatUpdate, dwPlayerIdx, txt, dwPlayerIdx );
}

void RForbidChatUpdate(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if( !pMessage->dwResult )
	{
		return;
	}

	BYTE	bIsForbidChat 	= BYTE( atoi( (char*)pData->Data[0] ) );
	DWORD	dwTargetIdx		= DWORD( atoi( (char*)pData->Data[1] ) );
	__time64_t	time		= _atoi64( (char*)pData->Data[2] );

	CPlayer* const pTarget = (CPlayer*)g_pUserTable->FindUser( dwTargetIdx );
	if( !pTarget )
	{
		return;		
	}

	// 해당 플레이어의 채팅금지 시간을 설정한다.
	pTarget->SetForbidChatTime( time );

	stTime64t msg;
	ZeroMemory( &msg, sizeof(msg) );
	msg.Category	= MP_CHAT;
	msg.Protocol	= (BYTE)(bIsForbidChat? MP_CHAT_FORBIDCHAT : MP_CHAT_PERMITCHAT);
	msg.dwObjectID	= pTarget->GetID();
	msg.time64t		= time;
	pTarget->SendMsg(&msg, sizeof(msg));
}

// 100601 ONS 채팅금지 대상을 로드한다.
void ForbidChatLoad( DWORD dwPlayerIdx )
{
	char txt[1024] = {0,};
	sprintf(txt, "EXEC dbo.MP_GM_FORBIDCHAT_LOAD %d", dwPlayerIdx );
	g_DB.Query( eQueryType_FreeQuery, eForbidChatLoad, dwPlayerIdx, txt, dwPlayerIdx );
}

void RForbidChatLoad(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if( !pMessage->dwID )
	{
		return;
	}

	// DB에 누적된 채팅금지 이력중에서 가장 최근의 정보를 로드한다.
	DWORD		dwTargetIdx	= DWORD( atoi( (char*)pData->Data[0] ) );
	__time64_t	ReleaseTime	= _atoi64( (char*)pData->Data[1] );

	CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser( dwTargetIdx );
	if( !pPlayer )
	{
		return;		
	}

	// 채팅금지 시간이 경과했을 경우 종료한다.
	__time64_t curTime;
	_time64( &curTime );
	if( curTime > ReleaseTime )
	{
		return;
	}

	// 채팅금지중이면 금지시간을 설정한다.
	pPlayer->SetForbidChatTime( ReleaseTime );

	// 해당 플레이어에게 통보한다.
	stTime64t msg;
	ZeroMemory( &msg, sizeof(msg) );
	msg.Category	= MP_CHAT;
	msg.Protocol	= MP_CHAT_FORBIDCHAT;
	msg.dwObjectID	= pPlayer->GetID();
	msg.time64t		= ReleaseTime;
	pPlayer->SendMsg(&msg, sizeof(msg));
}

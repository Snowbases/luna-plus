#include "StdAfx.h"
#include "ServerSystem.h"
#include "Network.h"
#include "MapNetworkMsgParser.h"
#include "Usertable.h"
#include "Servertable.h"
#include "ObjectFactory.h"
#include "UserTable.h"
#include "GridSystem.h"
#include "MapDBMsgParser.h"
#include "CharMove.h"
#include "Player.h"
#include "Monster.h"
#include "BossMonster.h"
#include "BossMonsterManager.h"
#include "BossRewardsManager.h"
#include "Npc.h"
#include "Object.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "MHTimeManager.h"
#include "ItemManager.h"
#include "TileManager.h"
#include "MHFile.h"
#include "CharacterCalcManager.h"
#include "PartyManager.h"
#include "ObjectStateManager.h"
#include "ItemDrop.h"
#include "MapDBMsgParser.h"
#include "AISystem.h"
#include "BattleSystem_Server.h"
#include "ChannelSystem.h"
#include "StreetSTallManager.h "
#include "StorageManager.h"
#include "BootManager.h"
#include "PathManager.h"
#include "RegenManager.h"
#include "ShowdownManager.h"
#include "PKManager.h"
#include "LootingManager.h"
#include "AIGroupManager.h"
#include "GuildFieldWarMgr.h"
#include "GuildTournamentMgr.h"
#include "QuestManager.h"
#include "..\[CC]Quest\QuestEvent.h"
#include "QuestRegenMgr.h"
#include "QuestMapMgr.h"
#include "GuildManager.h"
#include "cMonsterSpeechManager.h"
#include "MapObject.h"
#include "..\[CC]ServerModule\MiniDumper.h"
#include "WeatherManager.h"
#include "FieldBossMonsterManager.h"
#include "FieldBossMonster.h"
#include "FieldSubMonster.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "../[cc]skill/server/object/skillobject.h"
#include "../hseos/SHMain.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Monstermeter/SHMonstermeterManager.h"
#include "../hseos/Debug/SHDebug.h"
#include "../hseos/Date/SHDateManager.h"
#include "AutoNoteManager.h"
#include "FishingManager.h"
#include "cCookManager.h"
#include "Pet.h"
#include "PetManager.h"
#include "SiegeWarfareMgr.h"
#include "..\[CC]SiegeDungeon\SiegeDungeonMgr.h"
#include "./SiegeRecallMgr.h"
#include "./NpcRecallMgr.h"
#include "./LimitDungeonMgr.h"
#include "HousingMgr.h"
#include "Dungeon/DungeonMgr.h"
#include "Trigger/Manager.h"
#include "NPCMoveMgr.h"
#include "PCRoomManager.h"
#include "Party.h"
#include "Finite State Machine/Machine.h"

LPCTSTR g_SERVER_VERSION = "LTSV08070301";

void __stdcall ReceivedMsgFromServer(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength);
void __stdcall ReceivedMsgFromUser(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength);
void __stdcall OnAcceptServer(DWORD dwConnectionIndex);
void __stdcall OnDisconnectServer(DWORD dwConnectionIndex);
void __stdcall OnAcceptUser(DWORD dwConnectionIndex);
void __stdcall OnDisconnectUser(DWORD dwConnectionIndex);
void __stdcall ProcessServer(DWORD eventIndex);
void __stdcall ProcessGame(DWORD eventIndex);
void __stdcall ProcessCheck(DWORD eventIndex);
void ButtonProc1();
void ButtonProc2();
void ButtonProc3();
void ButtonProc4();
void ButtonToggleStatOfTrigger();
void ButtonTogglePeriodicMessageOfTrigger();
void ButtonToggleProcessTime();
void OnCommand(char* szCommand);

typedef void (*MSGPARSER)(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
MSGPARSER g_pServerMsgParser[MP_MAX];
MSGPARSER g_pUserMsgParser[MP_MAX];

BOOL g_bCloseWindow = FALSE;
HWND g_hWnd = 0;

int	g_nHackCheckNum = 15;
int	g_nHackCheckWriteNum = 15;

// 071218 LUJ, 3번 버튼 선택 시 프로세스 타임을 표시하기 위한 용도
struct ProcessTime
{
	DWORD	mCurrentTick;
	DWORD	mPreviousTick;
	DWORD	mMaxSpace;
	float	mAverageSpace;
	DWORD	mSpace;
	DWORD	mTotalSpace;
	DWORD	mCount;
}
processTime;


// taiyo 
CServerSystem * g_pServerSystem = NULL;

CServerSystem::CServerSystem()
{
	srand(GetTickCount());
	CoInitialize(NULL);
	
	g_pUserTable = new CUserTable;
	g_pUserTable->Init(2000);
	g_pServerTable = new CServerTable;
	g_pServerTable->Init(50);
	g_pObjectFactory = new CObjectFactory;
	g_pObjectFactory->Init();
	m_pGridSystem = new CGridSystem;
	m_pGridSystem->Init();

	m_Nation = eNATION_KOREA;
	m_dwQuestTime = 0;
	m_bQuestTime = FALSE;

	m_bCompletionChrUpdate = FALSE;

//-- for test
	m_dwMainProcessTime = 0;

	// 070809 웅주, 길드 정보를 모두 읽기 전까지 이 값이 FALSE로 되어있도록 되어 있으나.. 
	//				앞으로 길드 정보는 필요한 시점에만 로드할 것이다. 따라서 이 값은 초기화 후 참이 된다.
	//				길드 정보의 지연 로딩 작업이 모두 끝나면 이 플래그 자체가 필요없다.
	m_start	= TRUE;

	m_bTestServer = FALSE;
}

CServerSystem::~CServerSystem()
{
	CoUninitialize();

	delete g_pUserTable;
	delete g_pServerTable;
	delete g_pObjectFactory;
	delete m_pGridSystem;
}

void CServerSystem::Start(WORD ServerNum)
{	
	m_bCheckProcessTime = FALSE;
	m_dwProcessCount = 0;

	m_wMapNum = ServerNum;
//AO½A ·IAA
	SetNation();
	BOOTMNGR->AddSelfBootList(MAP_SERVER, ServerNum, g_pServerTable);

	MENU_CUSTOM_INFO menu[] = {
		{"Assert MsgBox", ButtonProc1},
		{"Load HackCheck", ButtonProc2},
		{"Put time", ButtonProc3},
		{"console", ButtonProc4},
		{"..toggle stat.", ButtonToggleStatOfTrigger},
		{"..toggle msg", ButtonTogglePeriodicMessageOfTrigger},
		{"..toggle Time", ButtonToggleProcessTime}
	};
	
	if(FALSE == g_Console.Init(sizeof(menu)/sizeof(*menu), menu, OnCommand))
	{
		MessageBox(
			0,
			"Console initializing Failed",
			0,
			0);
	}

	g_hWnd = GetActiveWindow();

	char TitleText[128];
	sprintf(TitleText, "MAP%d(%s)", ServerNum, GetMapName(ServerNum) );
	SetWindowText(g_hWnd, TitleText);

	// 080703 LYW --- ServerSystem : 서버 콘솔창에 처음으로 서버 버전부터 출력한다.
	g_Console.LOG(4, "　") ;
	g_Console.LOG(4, "[ Server Version : %s ]", g_SERVER_VERSION) ;
	g_Console.LOG(4, "　") ;
	
	//DWORD    dwProcessID = NULL;
    //DWORD    dwTreadID = ::GetWindowThreadProcessId(g_hWnd, &dwProcessID );

	// load hackcheck
	LoadHackCheck();

	FILE* fpstart = fopen("serverStart.txt","w");
#define STARTLOG(a)	fprintf(fpstart,#a);	a;
//	STARTLOG(CONDITIONMGR->Initial());
//	STARTLOG(CONDITIONMGR->LoadConditionList());
	STARTLOG(SKILLMGR->Init());

	STARTLOG(GAMERESRCMNGR->LoadMapChangeArea(GAMERESRCMNGR->m_MapChangeArea));
	STARTLOG(GAMERESRCMNGR->LoadLoginPoint(GAMERESRCMNGR->m_LoginPoint));
	STARTLOG(GAMERESRCMNGR->LoadMonsterList());
#ifdef _TESTCLIENT_
	STARTLOG(GAMERESRCMNGR->LoadSMonsterList());
#endif
	STARTLOG(BOSSMONMGR->LoadSummonInfoList());

	STARTLOG(GAMERESRCMNGR->LoadMonsterRewardList());

	STARTLOG(BOSSMONMGR->LoadBossMonsterInfoList());
	STARTLOG(BOSSREWARDSMGR->LoadBossRewardsInfo());
	
	// 필드보스 - 05.12 이영준
	STARTLOG(FIELDBOSSMONMGR->Init());
	STARTLOG(GAMERESRCMNGR->LoadNpcList());
	STARTLOG(GAMERESRCMNGR->LoadStaticNpc());
	// 06. 05 HIDE NPC - 이영준
	STARTLOG(GAMERESRCMNGR->LoadHideNpcList());	
	STARTLOG(GAMERESRCMNGR->LoadSkillMoney());	
	STARTLOG(GAMERESRCMNGR->LoadExpPoint());
	STARTLOG(GAMERESRCMNGR->LoadFishingExpPoint());
	STARTLOG(GAMERESRCMNGR->LoadPlayerxMonsterPoint());
	// 080826 KTH -- Load Npc Buff List
	STARTLOG(NPCRECALLMGR->LoadNPCBuffList());

	STARTLOG(STORAGEMGR->LoadStorageList());
	STARTLOG(ITEMMGR->LoadItemList());	
	STARTLOG(ITEMMGR->LoadDealerItem());

	STARTLOG(ITEMMGR->LoadMonSummonItemInfo());
	STARTLOG(ITEMMGR->LoadNpcSummonItemInfo());
	STARTLOG(ITEMMGR->LoadScriptFileDataChangeItem());	
	STARTLOG(MON_SPEECHMGR->LoadMonSpeechInfoList());
	STARTLOG(PETMGR->LoadPetInfo());
	STARTLOG(m_Map.InitMap(ServerNum));
	STARTLOG(PATHMGR->SetMap(&m_Map, ServerNum, m_Map.GetTileWidth(ServerNum)));
	STARTLOG(PKMGR->Init(m_Map.IsPKAllow()));
	STARTLOG(LoadEventRate("./System/Resource/droprate.txt"));

	// quest
	STARTLOG(QUESTMGR->LoadQuestScript());
	STARTLOG(QUESTREGENMGR->LoadData());	
	AUTONOTEMGR->Init();

	g_pServerMsgParser[MP_POWERUP] = MP_POWERUPMsgParser;
	g_pServerMsgParser[MP_CHAR] = MP_CHARMsgParser;
	g_pServerMsgParser[MP_ITEM] = MP_ITEMMsgParser;
	g_pServerMsgParser[MP_CHAT] = MP_CHATMsgParser;
	g_pServerMsgParser[MP_USERCONN] = MP_USERCONNMsgParser;
	g_pServerMsgParser[MP_MOVE] = MP_MOVEMsgParser;
	g_pServerMsgParser[MP_SKILLTREE] = MP_SKILLTREEMsgParser;
	g_pServerMsgParser[MP_CHEAT] = MP_CHEATMsgParser;
	g_pServerMsgParser[MP_QUICK] = MP_QUICKMsgParser;
	g_pServerMsgParser[MP_PARTY] = MP_PARTYMsgParser;
	g_pServerMsgParser[MP_SKILL] = MP_SkillMsgParser;
	g_pServerMsgParser[MP_STORAGE] = MP_STORAGEMsgParser;
	g_pServerMsgParser[MP_BATTLE] = MP_BattleMsgParser;
	g_pServerMsgParser[MP_CHAR_REVIVE] = MP_REVIVEMsgParser;
	g_pServerMsgParser[MP_EXCHANGE] = MP_EXCHANGEMsgParser;
	g_pServerMsgParser[MP_STREETSTALL] = MP_STREETSTALLMsgParser;
	g_pServerMsgParser[MP_NPC] = MP_NPCMsgParser;
	g_pServerMsgParser[MP_QUEST] = MP_QUESTMsgParser;
	g_pServerMsgParser[MP_MORNITORMAPSERVER] = MP_MonitorMsgParser;
	g_pServerMsgParser[MP_PK] = MP_PKMsgParser;
	g_pServerMsgParser[MP_HACKCHECK] = MP_HACKCHECKMsgParser;
	g_pServerMsgParser[MP_GUILD] = MP_GUILDMsgParser;
	g_pServerMsgParser[MP_GUILD_WAR] = MP_GUILDFIELDWARMsgParser;
	g_pServerMsgParser[MP_GTOURNAMENT] = MP_GTOURNAMENTMsgParser;
	g_pServerMsgParser[MP_GUILD_UNION] = MP_GUILUNIONMsgParser;
	g_pServerMsgParser[MP_FACIAL] = MP_FACIALMsgParser;
	g_pServerMsgParser[MP_EMOTION] = MP_EMOTIONMsgParser ;
	g_pServerMsgParser[MP_FAMILY] = MP_FAMILYMsgParser;
	g_pServerMsgParser[MP_FARM] = MP_FARM_MsgParser;
	g_pServerMsgParser[MP_RESIDENTREGIST] = MP_RESIDENTREGIST_MsgParser;
	g_pServerMsgParser[MP_TUTORIAL] = MP_TUTORIALMsgParser ;
	g_pServerMsgParser[MP_DATE] = MP_DATE_MsgParser;
	g_pServerMsgParser[MP_AUTONOTE] = MP_AUTONOTE_MsgParser;
	g_pServerMsgParser[MP_FISHING] = MP_FISHING_ServerMsgParser;
	g_pServerMsgParser[MP_PET] = MP_PET_MsgParser;
  	g_pServerMsgParser[MP_SIEGEWARFARE] = MP_SIEGEWARFAREMsgParser;
   	g_pServerMsgParser[MP_SIEGERECALL] = MP_SIEGERECALL_MsgParser ;
  	g_pServerMsgParser[MP_RECALLNPC] = MP_RECALLNPC_MsgParser ;
	g_pServerMsgParser[MP_LIMITDUNGEON] = MP_LIMITDUNGEON_MsgParser ;
	g_pServerMsgParser[MP_COOK] = MP_COOK_MsgParser;
	g_pServerMsgParser[MP_VEHICLE] = MP_VEHICLE_MsgParser;
	g_pServerMsgParser[MP_HOUSE] = MP_HOUSE_MsgParser;
	g_pServerMsgParser[MP_TRIGGER] = MP_TRIGGER_MsgParser;
	g_pServerMsgParser[MP_DUNGEON] = MP_DUNGEON_MsgParser;
	g_pServerMsgParser[MP_PCROOM] = MP_PCROOM_MsgParser;
	g_pServerMsgParser[MP_TRIGGER] = &(Trigger::CManager::NetworkMsgParser);
	g_pServerMsgParser[MP_CONSIGNMENT] = MP_CONSIGNMENT_MsgParser;
	g_pServerMsgParser[MP_NOTE] = MP_NOTE_MsgParser;
	STARTLOG( CHANNELSYSTEM->Init( ServerNum, m_Map.GetChannelNum() ) );

	STARTLOG(g_pAISystem.LoadAIGroupList());

	CUSTOM_EVENT customEvent[] = {
		{10, ProcessServer},
		{100, ProcessGame},
		{1000 * 60 * 5, ProcessCheck},
	};

	DESC_NETWORK desc = {0};
	desc.OnAcceptServer = OnAcceptServer;
	desc.OnDisconnectServer = OnDisconnectServer;
	desc.OnAcceptUser = OnAcceptUser;
	desc.OnDisconnectUser = OnDisconnectUser;
	desc.OnRecvFromServerTCP = ReceivedMsgFromServer;
	desc.OnRecvFromUserTCP = ReceivedMsgFromUser;
	desc.dwCustomDefineEventNum	= (sizeof(customEvent) / sizeof( *customEvent));
	desc.pEvent = customEvent;
	desc.dwMainMsgQueMaxBufferSize = 20480000;
	desc.dwMaxServerNum = 50;
	desc.dwMaxUserNum = 10;
	desc.dwServerBufferSizePerConnection = 512000;
	desc.dwServerMaxTransferSize = 65535;
	desc.dwUserBufferSizePerConnection = 65535;
	desc.dwUserMaxTransferSize = 65535;
	desc.dwConnectNumAtSameTime = 100;
	desc.dwFlag = 0;

	STARTLOG(g_Network.Init(&desc));
	
	if(!BOOTMNGR->StartServer(&g_Network, g_pServerTable->GetSelfServer()))
	{
		//ASSERT(0);
	}
	if(!BOOTMNGR->ConnectToMS(&g_Network, g_pServerTable))
	{
		SERVERINFO info;
		info.wServerKind = MONITOR_SERVER;
		OnConnectServerFail(&info);		
	}

	//////////////////////////////////////////////////////////////////////////
	// DB ¨uⓒød
	DWORD maxthread = g_pServerSystem->GetMap()->GetMaxDBThread();
	DWORD maxqueryinsametime = g_pServerSystem->GetMap()->GetMaxQueryInSameTime();

	if(g_DB.Init(maxthread,maxqueryinsametime,FALSE) == FALSE)
		MessageBox(NULL,"DataBase Initializing Failed",0,0);


	// guildfieldwar
	GUILDWARMGR->Init();
	SIEGEWARFAREMGR->LoadMapInfo();
  	SIEGEWARFAREMGR->Init() ;
  	SIEGEDUNGEONMGR->LoadInfo();
  	SIEGERECALLMGR->Initialieze() ;
	
	// Guild Tournament
	GTMGR->Init();

	// Weather System
	// 080328 NYJ --- 낚시시스템추가로 날씨관리자 사용
	WEATHERMGR->Init();
	FISHINGMGR->Init();
	GAMERESRCMNGR->LoadHousing_AllList();
	HOUSINGMGR->Init();
	DungeonMGR->Init();
	GAMERESRCMNGR->LoadDungeonKeyList();
	PartyLoad(0);
	GuildLoadGuild(0);
	// 090316 LUJ, 새로 시작된 맵에서는 자동 소환/탑승하지 않도록 초기화한다
	ResetVehicle( GetMapNum() );
	GuildResetWarehouse( m_wMapNum );
	// 100305 ONS 맵기동시 DB의 PC방 버프정보를 초기화한다.
	PCROOMMGR->Init();
	
	// S 농장시스템 추가 added by hseos 2007.04.30	2007.09.07
	// 081017 LUJ, 농장 정보를 일괄 쿼리하도록 수정
	{
		g_csFarmManager.Init( GetMapNum() );
		int farmZoneId = 0;
		
		// 090520 LUJ, 각각의 농장 정보를 읽어올 수 있도록 수정
		if( TRUE == g_csFarmManager.GetFarmZoneID( GetMapNum(), &farmZoneId ) )
		{
			CSHFarmZone* const farmZone = g_csFarmManager.GetFarmZone( WORD( farmZoneId ) );

			if( farmZone )
			{
				g_DB.FreeQuery(
					eFarm_LoadFarmState,
					farmZoneId,
					"EXEC dbo.MP_FARM_SELECT %d",
					farmZoneId );

				// 091126 pdy 0번 농장의 0번 작물,가축이 로드가 안되는 버그 수정   
				g_DB.FreeQuery(
					eFarm_LoadCropInfo,
					farmZoneId,
					"EXEC dbo.MP_FARM_CROP_SELECT %d, 0, -1",
					farmZoneId );

				g_DB.FreeQuery(
					eFarm_LoadAnimalInfo,
					farmZoneId,
					"EXEC dbo.MP_FARM_ANIMAL_SELECT %d, 0, -1",
					farmZoneId );
			}
		}
	}

	g_csDateManager.LoadChallengeMonsterInfo(m_wMapNum);
	NPCMOVEMGR->Init();

	m_dwQuestTime = gCurTime;
	m_bQuestTime = FALSE;
	NPCRECALLMGR->Initialize();

	MiniDumper md(MiniDumper::DUMP_LEVEL_0);

	g_Console.LOG(4, "-----------   MAP SERVER START  -----------------");
	g_Console.LOG(4, "-----------   Map Number %d     -----------------", GAMERESRCMNGR->GetLoadMapNum());

	g_bReady = TRUE;

	fclose(fpstart);
	g_Console.WaitMessage();
}

void CServerSystem::End()
{
	SetStart(FALSE);

	g_Network.Release();

	if(FALSE == GetCharUpdateCompletion())
	{
		HandlingBeforeServerEND();
	}

	SKILLMGR->Release();
	g_Console.Release();

	BATTLESYSTEM->Release();

	SAFE_DELETE( g_pUserTable );
	SAFE_DELETE( g_pServerTable );
	SAFE_DELETE( g_pObjectFactory );
	SAFE_DELETE( m_pGridSystem );
	AUTONOTEMGR->Release();
	m_Map.Release();
	CoFreeUnusedLibraries();
}

void CServerSystem::Process()
{
	QueryPerformanceFrequency(&m_freq);

	// 071218 LUJ
	processTime.mPreviousTick	= GetTickCount();

	if(g_bCloseWindow)
	{
		SendMessage(g_hWnd, WM_CLOSE, 0,0);
		g_bCloseWindow = FALSE;
		SetStart(FALSE);
	}

	if(GetStart() == FALSE)
		return;

	MHTIMEMGR_OBJ->Process();

	if(!g_pUserTable)
		return;

	static DWORD dwLastProcessTime = 0;

	if( gCurTime > m_dwQuestTime+60000 )
	{
		m_dwQuestTime = gCurTime;
		m_bQuestTime = TRUE;
	}

	// 080602 NYJ --- 날씨, 낚시 Process()
	WEATHERMGR->Process();
	FISHINGMGR->Process();
	COOKMGR->Process();
	
	// 091229 ShinJS --- PC방 이벤트 Process
	PCROOMMGR->Process();

	QueryPerformanceCounter(&m_ObjLoopProc[0]);
	g_pUserTable->SetPositionUserHead();
	CObject* pObject;
	while( (pObject = (CObject*)g_pUserTable->GetUserData()) != NULL )
	{
		if(pObject->GetInited() == FALSE)
			continue;
		
		pObject->StateProcess();
		pObject->ProceedToTrigger();

		if(TRUE == pObject->GetStateInfo().bEndState &&
			pObject->GetStateInfo().State_End_Time < gCurTime)
		{
			// EndObjectState() 안에서 CObject를 해제한다! 계속 진행할 경우 해제된 메모리를 액세스하게 된다
			OBJECTSTATEMGR_OBJ->EndObjectState(
				pObject,
				pObject->GetState());
			continue;
		}

		CCharMove::MoveProcess(pObject);

		CCharacterCalcManager::UpdateLife(pObject);

		if(pObject->GetObjectKind() == eObjectKind_Player)
		{
			CPlayer* pPlayer = (CPlayer*)pObject;
			CCharacterCalcManager::ProcessLife(pPlayer);
			CCharacterCalcManager::UpdateMana(pPlayer);	
			CCharacterCalcManager::ProcessMana(pPlayer);

			pPlayer->QuestProcess();
			pPlayer->CheckImmortalTime();
			if( m_bQuestTime )
			{
				// add quest event
				CQuestEvent QEvent( eQuestEvent_Time, 0, 0 );
				QUESTMGR->AddQuestEvent( pPlayer, &QEvent );
			}

			pPlayer->ProcessTimeCheckItem( FALSE );


			// 낚시관련처리 : FISHINGMGR::Process() 가 호출된 이후에 사용되어야 함.
			//                유저테이블을 중복순회 하지 않기 위해 이곳에서 처리함.
			FISHINGMGR->ProcessPlayer(pPlayer);
		}
		else if(pObject->GetObjectKind() & eObjectKind_Monster)
		{
			CMonster* pMonster = (CMonster*)pObject;
			pMonster->Process();
		}
		else if(pObject->GetObjectKind() == eObjectKind_Npc)
		{
			CNpc* pNpc = (CNpc*)pObject;
			pNpc->Process();
		}
		else if(pObject->GetObjectKind() == eObjectKind_Pet)
		{
			// 091110 ONS 펫 애니메이션 추가 : 펫이 죽었을 경우 빠져나간다.
			if(pObject->GetDieFlag() == TRUE)
				continue;

			CPet* pPet = (CPet*)pObject;
			CCharacterCalcManager::ProcessPetLife(pPet);
			CCharacterCalcManager::ProcessPetMana(pPet);
			pPet->FriendlyProcess();
		}
	}
	QueryPerformanceCounter(&m_ObjLoopProc[1]);

	FISHINGMGR->ChangeMissionState();	// 유저테이블 순회후에 미션상태 변경을 해야함.
	FIELDBOSSMONMGR->Process();

	SKILLMGR->Process();
	BATTLESYSTEM->Process();
	g_pAISystem.Process();
	LOOTINGMGR->ProcessTimeOutCheck();
	QUESTMGR->Process();
	GUILDMGR->Process();
	GTMGR->Process();

	SIEGEWARFAREMGR->Process();
	
	m_bQuestTime = FALSE;

//--- Test
	m_dwMainProcessTime = gCurTime - dwLastProcessTime;
	dwLastProcessTime = gCurTime;

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.04.12
	CSHMain::MainLoop();
	// E 농장시스템 추가 added by hseos 2007.04.12

//---KES AUTONOTE
	AUTONOTEMGR->Process();
//---------------

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.27
	// ..맨 아래에 이전 LocalTime 을 저장해 둔다. 꼭 맨 아래에 있어야 함. 그렇지 않고
	// ..이 함수보다 아래에 위치하는 함수가 있을 경우 그 함수에서 OldLocalTime 을 사용하면 OldLocalTime 값이 설정되지 않음.
	MHTIMEMGR_OBJ->ProcOldLocalTime();
	LIMITDUNGEONMGR->Process() ;
	
	QueryPerformanceCounter(&m_TriggerProc[0]);
	TRIGGERMGR->Process();
	QueryPerformanceCounter(&m_TriggerProc[1]);
	HOUSINGMGR->Process();
	QueryPerformanceCounter(&m_DungeonProc[0]);
	DungeonMGR->Process();
	QueryPerformanceCounter(&m_DungeonProc[1]);
	NPCRECALLMGR->Process() ;
	
	// 071218 LUJ
	{
		processTime.mCurrentTick	=	GetTickCount();
		processTime.mSpace			=	processTime.mCurrentTick - processTime.mPreviousTick;
		processTime.mMaxSpace		=	max( processTime.mSpace, processTime.mMaxSpace );
		processTime.mTotalSpace		+=	processTime.mSpace;
		processTime.mAverageSpace	=	float( processTime.mTotalSpace ) / ++processTime.mCount;
	}

	if(m_bCheckProcessTime)
	{
		static DWORD dwLastPutLogTime = gCurTime;

		double fTimeObjLoopProc = (double)(m_ObjLoopProc[1].QuadPart - m_ObjLoopProc[0].QuadPart) / m_freq.QuadPart;
		double fTimeTriggerProc = (double)(m_TriggerProc[1].QuadPart - m_TriggerProc[0].QuadPart) / m_freq.QuadPart;
		double fTimeDungeonProc = (double)(m_DungeonProc[1].QuadPart - m_DungeonProc[0].QuadPart) / m_freq.QuadPart;

		m_fAvrObjLoopProc += fTimeObjLoopProc;
		m_fAvrTriggerProc += fTimeTriggerProc;
		m_fAvrDungeonProc += fTimeDungeonProc;
		m_dwProcessCount++;

		if(m_fTimeObjLoopProc < fTimeObjLoopProc)
			m_fTimeObjLoopProc = fTimeObjLoopProc;

		if(m_fTimeTriggerProc < fTimeTriggerProc)
			m_fTimeTriggerProc = fTimeTriggerProc;

		if(m_fTimeDungeonProc < fTimeDungeonProc)
			m_fTimeDungeonProc = fTimeDungeonProc;

		
		if(gCurTime > dwLastPutLogTime + 5000)
		{
			if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
			{
				Trigger::PutLog("DungeonInfo : %dDungeons, %dUsers, %dMonsters, %dNpcs, %dExtras",
					DungeonMGR->GetDungeonNum(),
					DungeonMGR->GetPlayerNum(),
					DungeonMGR->GetMonsterNum(),
					DungeonMGR->GetNpcNum(),
					DungeonMGR->GetExtraNum());
			}

			Trigger::PutLog("	Object Proc (High:%f, Avr:%f)", m_fTimeObjLoopProc, m_fAvrObjLoopProc/=m_dwProcessCount);
			Trigger::PutLog("	TriggerProc (High:%f, Avr:%f)", m_fTimeTriggerProc, m_fAvrTriggerProc/=m_dwProcessCount);
			Trigger::PutLog("	DungeonProc (High:%f, Avr:%f)", m_fTimeDungeonProc, m_fAvrDungeonProc/=m_dwProcessCount);

			dwLastPutLogTime = gCurTime;
		}
	}
}

void CServerSystem::SetStart( BOOL state )
{
	m_start = state;
	if( m_start )
	{
		g_Console.LOG( 4, "-----------   MAP SERVER LOAD DATA FROM DB  -----------------");
	}
}

CPlayer* CServerSystem::AddPlayer(DWORD dwPlayerID,DWORD dwAgentNum,DWORD UniqueIDinAgent,int ChannelNum, eUSERLEVEL userLevel)
{
	CObject* pPreObj = g_pUserTable->FindUser(dwPlayerID);
	ASSERT(pPreObj == NULL);
	if(pPreObj)
	{
		pPreObj->SetNotInited();
		RemovePlayer(dwPlayerID);
	}

	CHANNELSYSTEM->SetChallengeZoneAgentChannelID(ChannelNum);

	enum Alert
	{
		AlertNone,
		AlertDungeon,
		AlertHouse,
	}
	alertType = AlertNone;

	VECTOR3* pHouseStartPos = NULL;
	DWORD num = CHANNELSYSTEM->GetChannelID(ChannelNum);

	if( QUESTMAPMGR->IsQuestMap() )
	{
		num = CHANNELSYSTEM->CreateQuestMapChannel(dwPlayerID);
	}
	else if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
	{
		num = DungeonMGR->GetChannelIDFromReservationList(dwPlayerID);
		if(0 == num)
		{
			// 옵저버인 경우 채널번호를 할당받고 들어온다.
			num = ChannelNum;

			if( ChannelNum == 0 )
				alertType = AlertDungeon;
		}
	}
	else if(HOUSINGMAPNUM == g_pServerSystem->GetMapNum())
	{
		num = HOUSINGMGR->GetChannelIDFromReservationList(dwPlayerID);

		if(0 == num)
		{
			alertType = AlertHouse;
		}
		else
		{
			pHouseStartPos = HOUSINGMGR->GetStartPosFromeservationList(dwPlayerID);
		}
	}

	if(0 == num)
	{
		if(eUSERLEVEL_USER <= userLevel)
		{
			return 0;
		}

		const int temporaryChannel = 1;
		num = temporaryChannel;
	}

	BASEOBJECT_INFO binfo;
	ZeroMemory(
		&binfo,
		sizeof(binfo));
	binfo.dwObjectID = dwPlayerID;

	CPlayer* pPlayer = (CPlayer*)g_pObjectFactory->MakeNewObject(eObjectKind_Player,dwAgentNum, &binfo);

	if(pHouseStartPos)
	{
		CCharMove::SetPosition(
		pPlayer,
		pHouseStartPos);
	}

	pPlayer->SetUniqueIDinAgent(
		UniqueIDinAgent);
	pPlayer->SetBattleID(
		num);
	pPlayer->SetGridID(
		num);
	pPlayer->SetChannelID(
		num);
	pPlayer->SetCurChannel(
		ChannelNum);
	pPlayer->InitClearData();
	pPlayer->SetInitState(
		PLAYERINITSTATE_ONLY_ADDED,
		0);
	pPlayer->SetUserLevel(
		userLevel);

	CHANNELSYSTEM->IncreasePlayerNum(pPlayer->GetChannelID());
	g_pUserTable->AddUser(pPlayer,pPlayer->GetID());

	switch(alertType)
	{
	case AlertDungeon:
		{
			TESTMSG message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_SIGNAL;
			message.Protocol = MP_SIGNAL_COMMONUSER;
			SafeStrCpy(
				message.Msg,
				"This is a dungeon which you need to have specific items to enter. For detailed info, please ask programmer about them",
				_countof(message.Msg));

			pPlayer->SendMsg(
				&message,
				message.GetMsgLength());
			

			FILE* fpLog = NULL;
			fpLog = fopen( "./Log/DungeonEnterLog.txt", "a+" );
			if( fpLog )
			{
				SYSTEMTIME sysTime;
				GetLocalTime( &sysTime );

				fprintf( fpLog, "[%04d-%02d-%02d %02d:%02d:%02d] %s - AlertDungeon [PlayerID : %d, AgentNum : %d, ChannelNum : %d, UserLv : %d] \n", 
					sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
					__FUNCTION__,
					dwPlayerID, dwAgentNum, num, userLevel );

				fclose( fpLog );
			}

			break;
		}
	case AlertHouse:
		{
			TESTMSG message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_SIGNAL;
			message.Protocol = MP_SIGNAL_COMMONUSER;
			SafeStrCpy(
				message.Msg,
				"This is a house which you need to have specific items to enter. For detailed info, please ask programmer about them",
				_countof(message.Msg));

			pPlayer->SendMsg(
				&message,
				message.GetMsgLength());
			break;
		}
	}

	return pPlayer;
}

CPlayer* CServerSystem::InitPlayerInfo(BASEOBJECT_INFO* pBaseObjectInfo,CHARACTER_TOTALINFO* pTotalInfo,HERO_TOTALINFO* pHeroInfo)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pBaseObjectInfo->dwObjectID);
	if(pPlayer == NULL)
		return NULL;
	pBaseObjectInfo->BattleID = pPlayer->GetBattleID();
	ASSERT(pPlayer->GetID() == pBaseObjectInfo->dwObjectID);
	pPlayer->Init(eObjectKind_Player,pPlayer->GetAgentNum(),pBaseObjectInfo);
	pPlayer->InitCharacterTotalInfo(pTotalInfo);
	pPlayer->InitHeroTotalInfo(pHeroInfo);

	return pPlayer;
}

CMonster* CServerSystem::AddMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos,WORD wObjectKind)
{
	EObjectKind objectkind = eObjectKind_Monster;
	
	switch( wObjectKind )
	{
	case eObjectKind_SpecialMonster:
	case eObjectKind_ToghterPlayMonster:
	case eObjectKind_Trap:
	case eObjectKind_Vehicle:
		{
			objectkind = EObjectKind( wObjectKind );
			break;
		}
	}

	CMonster* pMonster = (CMonster*)g_pObjectFactory->MakeNewObject(objectkind,0, pBaseObjectInfo);
	pMonster->SetSubID(dwSubID);
	pMonster->InitMonster(pTotalInfo);
	pMonster->SetGridID(pBaseObjectInfo->BattleID);

	UpdateFiniteStateMachine(
		*pMonster,
		dwSubID);

	pMonster->SetInited();

	CCharMove::InitMove(pMonster,pPos);

	g_pUserTable->AddUser(pMonster,pMonster->GetID());

	// 100614 ShinJS --- 탈것은 AI 등록안함
	if( objectkind != eObjectKind_Vehicle )
	{
		g_pAISystem.AddObject(
			pMonster);
	}
	
	CBattle* pBattle = BATTLESYSTEM->GetBattle(pMonster->GetBattleID());
	if(pBattle->GetBattleKind() != eBATTLE_KIND_NONE)
		BATTLESYSTEM->AddObjectToBattle(pBattle, pMonster);

	AddMonsterCount(*pMonster);
	return pMonster;
}

CBossMonster* CServerSystem::AddBossMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos)
{
	ASSERT(g_pUserTable->FindUser(pBaseObjectInfo->dwObjectID) == NULL);

	CBossMonster* pBossMonster = (CBossMonster*)g_pObjectFactory->MakeNewObject(eObjectKind_BossMonster,0, pBaseObjectInfo);
	pBossMonster->SetSubID(dwSubID);
	pBossMonster->InitMonster(pTotalInfo);
	pBossMonster->SetGridID(pBaseObjectInfo->BattleID);
	pBossMonster->SetInited();
	
	CCharMove::InitMove(pBossMonster,pPos);
	

	g_pUserTable->AddUser(pBossMonster,pBossMonster->GetID());
	g_pAISystem.AddObject(pBossMonster);
	BOSSMONMGR->SetBossInfo(pBossMonster);
	
	CBattle* pBattle = BATTLESYSTEM->GetBattle(pBossMonster->GetBattleID());
	if(pBattle->GetBattleKind() != eBATTLE_KIND_NONE)
		BATTLESYSTEM->AddObjectToBattle(pBattle, pBossMonster);

	AddMonsterCount(*pBossMonster);
	return pBossMonster;
}

// 필드보스 - 05.12 이영준
// 필드보스 추가 함수
CFieldBossMonster* CServerSystem::AddFieldBossMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos)
{
	ASSERT(g_pUserTable->FindUser(pBaseObjectInfo->dwObjectID) == NULL);

	CFieldBossMonster* pFieldBossMonster = (CFieldBossMonster*)g_pObjectFactory->MakeNewObject(eObjectKind_FieldBossMonster,0, pBaseObjectInfo);
	pFieldBossMonster->SetSubID(dwSubID);
	pFieldBossMonster->InitMonster(pTotalInfo);
	pFieldBossMonster->SetGridID(pBaseObjectInfo->BattleID);
	pFieldBossMonster->SetInited();
	
	CCharMove::InitMove(pFieldBossMonster,pPos);

	g_pUserTable->AddUser(pFieldBossMonster,pFieldBossMonster->GetID());
	g_pAISystem.AddObject(pFieldBossMonster); 
		
	CBattle* pBattle = BATTLESYSTEM->GetBattle(pFieldBossMonster->GetBattleID());
	if(pBattle->GetBattleKind() != eBATTLE_KIND_NONE)
		BATTLESYSTEM->AddObjectToBattle(pBattle, pFieldBossMonster);

	AddMonsterCount(*pFieldBossMonster);
	return pFieldBossMonster;
}

// 필드보스 - 05.12 이영준
// 필드보스 부하 추가 함수
CFieldSubMonster* CServerSystem::AddFieldSubMonster(DWORD dwSubID, BASEOBJECT_INFO* pBaseObjectInfo,MONSTER_TOTALINFO* pTotalInfo,VECTOR3* pPos)
{
	ASSERT(g_pUserTable->FindUser(pBaseObjectInfo->dwObjectID) == NULL);

	CFieldSubMonster* pFieldSubMonster = (CFieldSubMonster*)g_pObjectFactory->MakeNewObject(eObjectKind_FieldSubMonster,0, pBaseObjectInfo);
	pFieldSubMonster->SetSubID(dwSubID);
	pFieldSubMonster->InitMonster(pTotalInfo);
	pFieldSubMonster->SetGridID(pBaseObjectInfo->BattleID);
	pFieldSubMonster->SetInited();
	
	CCharMove::InitMove(pFieldSubMonster,pPos);

	g_pUserTable->AddUser(pFieldSubMonster,pFieldSubMonster->GetID());

	g_pAISystem.AddObject(pFieldSubMonster); 
		
	CBattle* pBattle = BATTLESYSTEM->GetBattle(pFieldSubMonster->GetBattleID());
	if(pBattle->GetBattleKind() != eBATTLE_KIND_NONE)
		BATTLESYSTEM->AddObjectToBattle(pBattle, pFieldSubMonster);

	AddMonsterCount(*pFieldSubMonster);
	return pFieldSubMonster;
}

CNpc* CServerSystem::AddNpc(BASEOBJECT_INFO* pBaseObjectInfo,NPC_TOTALINFO* pTotalInfo,VECTOR3* pPos)
{
	ASSERT(g_pUserTable->FindUser(pBaseObjectInfo->dwObjectID) == NULL);

	CNpc* pNpc = (CNpc*)g_pObjectFactory->MakeNewObject(eObjectKind_Npc,0, pBaseObjectInfo);

	// 081012 LYW --- ServerSystem : npc null 체크를 한다.
	if( !pNpc ) return NULL ;

	pNpc->InitNpc(pTotalInfo);
	pNpc->SetGridID(pBaseObjectInfo->BattleID);
	pNpc->SetInited();
	CCharMove::InitMove(pNpc,pPos);

	g_pUserTable->AddUser(pNpc,pNpc->GetID());
	
	CBattle* pBattle = BATTLESYSTEM->GetBattle(pNpc->GetBattleID());
	if(pBattle->GetBattleKind() != eBATTLE_KIND_NONE)
		BATTLESYSTEM->AddObjectToBattle(pBattle, pNpc);

	return pNpc;
}

cSkillObject* CServerSystem::AddSkillObject(cSkillObject* pSkillObj,VECTOR3* pPos)
{
	ASSERT(g_pUserTable->FindUser(pSkillObj->GetID()) == NULL);

	pSkillObj->SetInited();
	CCharMove::InitMove(pSkillObj,pPos);

	g_pUserTable->AddUser(pSkillObj,pSkillObj->GetID());
	
	return pSkillObj;
}

CMapObject* CServerSystem::AddMapObject(DWORD Kind, BASEOBJECT_INFO* pBaseObjectInfo, MAPOBJECT_INFO* pMOInfo, VECTOR3* pPos)
{
	ASSERT(g_pUserTable->FindUser(pBaseObjectInfo->dwObjectID) == NULL);

	CMapObject* pMapObject = (CMapObject*)g_pObjectFactory->MakeNewObject((EObjectKind)Kind,0, pBaseObjectInfo);
	
	pMapObject->InitMapObject( pMOInfo );
	pMapObject->SetGridID( pBaseObjectInfo->BattleID );
	pMapObject->SetInited();
	CCharMove::InitMove( pMapObject, pPos );

	g_pUserTable->AddUser( pMapObject, pMapObject->GetID() );
	
	CBattle* pBattle = BATTLESYSTEM->GetBattle(pMapObject->GetBattleID());
	if(pBattle->GetBattleKind() != eBATTLE_KIND_NONE)
		BATTLESYSTEM->AddObjectToBattle( pBattle, pMapObject );

	return pMapObject;
}

void CServerSystem::HandlingBeforeServerEND()
{
	//SetUnableProcess(TRUE);				//1. Process 리턴 & DBReturnMsg 처리 안함.

	g_Console.LOG(4, "[0/4]Start Working For Map Server Shutdown" );

	RemoveServerForKind(AGENT_SERVER);	//2. AgentSrv 끊기.
	g_Console.LOG(4, "[1/4]Agent Disconnecting...Done" );

	//3. =======Object 처리
	g_pUserTable->SetPositionHead();
	CObject* pObject;
	while( (pObject = g_pUserTable->GetData()) != NULL )
	{	
		if(pObject->GetObjectKind() == eObjectKind_Player)
			g_pServerSystem->RemovePlayer(pObject->GetID(), FALSE);
			//FALSE: 실제로 UserTable에서는 지우지 않는다.
		else
		{
			CCharMove::ReleaseMove(pObject);
			g_pObjectFactory->ReleaseObject(pObject);
			if(pObject->GetObjectKind() == eObjectKind_Monster ||
				pObject->GetObjectKind() == eObjectKind_BossMonster )
			{
				g_pAISystem.RemoveObject(pObject->GetID());
			}
		}
	}
	g_Console.LOG(4, "[2/4]Start Object Handling (Update/Delete)...Done" );

	g_pUserTable->RemoveAll();
	g_Console.LOG(4, "[3/4]Removing All Object on UserTable...Done" );

	g_Console.LOG(4, "[4/4]Start DB Release..." );
	g_DB.Release();
	g_Console.LOG(4, "[4/4]Release...Done" );

	SetCharUpdateCompletion(TRUE);
}


void CServerSystem::RemovePlayer(DWORD dwPlayerID, BOOL bRemoveFromUserTable )
{	
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerID);
	if(!pPlayer)
		return;
	ASSERT(pPlayer->GetObjectKind() == eObjectKind_Player);

	EXCHANGEMGR->UserLogOut(pPlayer);
	STREETSTALLMGR->UserLogOut(pPlayer);
	SHOWDOWNMGR->UserLogOut(pPlayer);
	LOOTINGMGR->LootingCancel(pPlayer);	//PKLOOT
	BOSSMONMGR->UserLogOut(pPlayer->GetID(), pPlayer->GetGridID());
	// 080725 KTH - 죽은걸로 처리하자;
	SIEGEWARFAREMGR->CancelWaterSeedUsing(pPlayer);

	// 071122
	pPlayer->ProcessTimeCheckItem( TRUE );

	if(pPlayer->GetInited() == TRUE)
	{
		if(pPlayer->GetState() == eObjectState_Die)
		{
			if( pPlayer->GetBattle()->GetBattleKind() == eBATTLE_KIND_NONE &&
				pPlayer->IsPenaltyByDie() )
			{
				pPlayer->ReviveLogInPenelty(); 
			}
			else
			{
				pPlayer->ReviveAfterShowdown( FALSE );	//°a°uA³¸®¸¸.. msgº¸³≫Ao ¾E´A´U.
			}
		}

		if( pPlayer->IsPKMode() )
			pPlayer->SetPKModeEndtime();

		CharacterHeroInfoUpdate(pPlayer);
		CharacterTotalInfoUpdate(pPlayer);
		Option_Update(*pPlayer);
		CCharMove::ReleaseMove(pPlayer);

		BATTLESYSTEM->DeleteObjectFromBattle(pPlayer);

		MonsterMeter_Save(pPlayer->GetID(), 0, 0, pPlayer->GetMonstermeterInfo()->nPlayTimeTotal, pPlayer->GetMonstermeterInfo()->nKillMonsterNumTotal);

		if(g_pServerSystem->GetMapNum() != GTMAPNUM && pPlayer->IsEmergency())
		{
			VECTOR3* ppos;
			VECTOR3 pos;

			ppos = GAMERESRCMNGR->GetRevivePoint();

			if(ppos)	// NYJ 포인터무효상태로 접근하면 다운.
			{
				int temp;
				temp = rand() % 500 - 250;
				pos.x = ppos->x + temp;
				temp = rand() % 500 - 250;
				pos.z = ppos->z + temp;
				pos.y = 0;

				pPlayer->SetMapMoveInfo( GAMERESRCMNGR->GetLoadMapNum(), ( DWORD )( pos.x ), ( DWORD )( pos.z ) );
			}
		}
	}
	else		
	{
		CCharMove::ReleaseMove(pPlayer);
	}

	CHANNELSYSTEM->DecreasePlayerNum(
		pPlayer->GetChannelID());
	DWORD playerSizeInChannel = CHANNELSYSTEM->GetPlayerNumInChannel(
		pPlayer->GetChannelID());

	// 100111 LUJ, 하우스/던젼 맵은 프로세스 상으로 카운트가 늦게 되서 감소처리 해줘야한다
	//			이후 채널 개수에 관계없이 처리할 수 있도록 수정 작업할 예정
	if(HOUSINGMAPNUM == GetMapNum())
	{
		--playerSizeInChannel;
	}
	else if(DungeonMGR->IsDungeon(GetMapNum()) &&
		! pPlayer->IsDungeonObserver())	// 던전옵저버는 인원체크 하지 않는다.
	{
		--playerSizeInChannel;
	}

	if(0 == playerSizeInChannel)
	{
		TRIGGERMGR->StopServerTrigger(pPlayer->GetChannelID());
	}

	UnRegistLoginMapInfo(
		dwPlayerID);
	QUESTMAPMGR->RemovePlayer(
		pPlayer);

	BOOL bUpdateLogPos = TRUE;
	MAPTYPE map;
	DWORD	pos_x;
	DWORD	pos_z;

	pPlayer->GetMapMoveInfo( map, pos_x, pos_z );

	BOOL bMapIsZeroValue = ( map == 0 ) ? TRUE : FALSE ;

	if( map == 0 && pos_x == 0 && pos_z == 0 )
	{
		VECTOR3 pos;
		
		pPlayer->GetPosition( &pos );

		map = GAMERESRCMNGR->GetLoadMapNum();
		
		pos_x = (DWORD)pos.x;
		pos_z = (DWORD)pos.z;
	}
	else
	{
		/// 맵 번호가 0인 경우 귀환지로 보내버리자?
		if( map == 0 )
		{
			LOGINPOINT_INFO* ReturnInfo = NULL;

			ReturnInfo = GAMERESRCMNGR->GetLoginPointInfo( pPlayer->GetPlayerLoginPoint() );

			/// 귀환지가 없다? 그럼 안전지대로...
			if( !ReturnInfo )
			{
				VECTOR3* ppos;
				VECTOR3 pos;

				ppos = GAMERESRCMNGR->GetRevivePoint();

				int temp;
				temp = rand() % 500 - 250;
				pos.x = ppos->x + temp;
				temp = rand() % 500 - 250;
				pos.z = ppos->z + temp;
				pos.y = 0;		

				map = GAMERESRCMNGR->GetLoadMapNum();
				pos_x = ( DWORD )( pos.x );
				pos_z = ( DWORD )( pos.z );
			}
			else
			{
				VECTOR3 RandPos;

				int temp;
				temp = rand() % 500 - 250;
				RandPos.x = ReturnInfo->CurPoint[0].x + temp;
				temp = rand() % 500 - 250;
				RandPos.z = ReturnInfo->CurPoint[0].z + temp;
				RandPos.y = 0;

				map = ReturnInfo->MapNum;
				pos_x = ( DWORD )( RandPos.x );
				pos_z = ( DWORD )( RandPos.z );
			}
		}

		/// 이동 좌표가 이상하다면... 안전지대로...
		if( pos_x == 0 || pos_x > 51200 || pos_z == 0 || pos_z > 51200 )
		{
			VECTOR3* ppos;
			VECTOR3 pos;

			ppos = GAMERESRCMNGR->GetRevivePoint();

			int temp;
			temp = rand() % 500 - 250;
			pos.x = ppos->x + temp;
			temp = rand() % 500 - 250;
			pos.z = ppos->z + temp;
			pos.y = 0;		

			map = GAMERESRCMNGR->GetLoadMapNum();
			pos_x = ( DWORD )( pos.x );
			pos_z = ( DWORD )( pos.z );
		}
	}

	if(g_pServerSystem->GetMapNum() == GTMAPNUM)
	{		
		VECTOR3* ppos = GAMERESRCMNGR->GetRevivePoint(GTRETURNMAPNUM);
		VECTOR3 pos = {0};
		int temp;
		temp = rand() % 500 - 250;
		pos.x = ppos->x + temp;
		temp = rand() % 500 - 250;
		pos.z = ppos->z + temp;
		
		CharacterLogoutPointUpdate(
			pPlayer->GetID(),
			GTRETURNMAPNUM,
			pos.x,
			pos.z);

		bUpdateLogPos = FALSE;
	}
	else if((SIEGEWARFAREMGR->IsSiegeWarfareZone(g_pServerSystem->GetMapNum(), FALSE) && 
				SIEGEWARFAREMGR->Is_CastleMap() == FALSE)
			|| SIEGEDUNGEONMGR->IsSiegeDungeon(g_pServerSystem->GetMapNum()))
	{
		const VillageWarp* const pVillageWarp = SIEGEWARFAREMGR->GetVilageInfo();

		// 100216 pdy 공성지역 이동좌표문제 수정
		if( map == pVillageWarp->MapNum || bMapIsZeroValue == TRUE || pPlayer->IsEmergency() )
		{
			if(pVillageWarp)
			{
				CharacterLogoutPointUpdate(
					pPlayer->GetID(),
					WORD(pVillageWarp->MapNum),
					pVillageWarp->PosX * 100.0f,
					pVillageWarp->PosZ * 100.0f);
			}

			bUpdateLogPos = FALSE;
		}
	}
	else if(g_csDateManager.IsChallengeZone(g_pServerSystem->GetMapNum()))
	{
		bUpdateLogPos = FALSE;
	}
	else if(g_pServerSystem->GetMapNum() == HOUSINGMAPNUM)
	{
		DWORD dwReserveChannel = HOUSINGMGR->GetChannelIDFromReservationList(pPlayer->GetID());
		if(dwReserveChannel == MAX_HOUSE_NUM+1)
		{
			HOUSINGMGR->ExitHouse(pPlayer->GetID(), FALSE);
		}
		else
		{
			// 하우징맵에서 다른 하우스 방문
			HOUSINGMGR->ExitHouse(pPlayer->GetID(), TRUE);
		}

		bUpdateLogPos = FALSE;
	}
	else if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
	{
		DungeonMGR->Exit(pPlayer->GetID());
	}
	
	if(PARTYMGR->CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
	{
		CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx());
		if(pParty)
		{
			if(pParty->GetMasterID() == pPlayer->GetID())
					PARTYMGR->BreakupInstantParty(pParty->GetPartyIdx(), pPlayer->GetID());
			else
				PARTYMGR->DelMemberInstantParty(pPlayer->GetID(), pParty->GetPartyIdx());
		}
	}

	if(map == HOUSINGMAPNUM || DungeonMGR->IsDungeon(map) || g_csDateManager.IsChallengeZone(map))
	{
		bUpdateLogPos = FALSE;
	}

	if(bUpdateLogPos)
	{
		CharacterLogoutPointUpdate(
			pPlayer->GetID(),
			map,
			float(pos_x),
			float(pos_z));
	}

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.29	2007.12.05
	// ..맵 이탈시 DB의 챌린지 존 입장 회수 초기화가 필요할 경우.. CSHDateManager::SRV_ProcChallengeZoneEnterFreq 함수의 설명 참조
	if (pPlayer->GetChallengeZoneNeedSaveEnterFreq())
	{
		ChallengeZone_EnterFreq_Save(pPlayer->GetID(), 0, pPlayer->GetChallengeZoneEnterBonusFreq());
		pPlayer->SetChallengeZoneNeedSaveEnterFreq(FALSE);
	}
	// ..챌린지 존 끝
	g_csDateManager.SRV_EndChallengeZone(pPlayer, CSHDateManager::CHALLENGEZONE_END_PARTNER_OUT);
	// E 데이트 존 추가 added by hseos 2007.11.29	2007.12.05

	// 091231 ShinJS --- PC방 데이터 제거
	PCROOMMGR->RemovePlayer( dwPlayerID );

	// 080804 LUJ, 객체 해제는 항상 최후에 수행해야 한다. pPlayer로 수행할 코드가 있을 경우 이 코드 앞에 넣어야 한다
	{
		g_pObjectFactory->ReleaseObject( pPlayer );

		if( bRemoveFromUserTable )
		{
			g_pUserTable->RemoveUser( dwPlayerID );
		}
	}
}


CPet* CServerSystem::AddPet(BASEOBJECT_INFO& pBaseObjectInfo, const PET_OBJECT_INFO& pPetObjectInfo, CPlayer* pPlayer)
{
	CPet* const previousPetObject = PETMGR->GetPet(
		pPlayer->GetPetItemDbIndex());

	if(previousPetObject)
	{
		previousPetObject->SetPetObjectState(
			ePetState_None);
		RemovePet(
			previousPetObject->GetID());
	}

	CPet* pPet = ( CPet* )g_pObjectFactory->MakeNewObject(
		eObjectKind_Pet,
		0,
		&pBaseObjectInfo);

	if(0 == pPet)
	{
		return 0;
	}

	pPet->SetGridID(
		pPlayer->GetGridID());
	pPet->SetOwnerIndex(
		pPlayer->GetID());
	pPet->SetObjectInfo(
		pPetObjectInfo);
	PETMGR->AddObject(
		pPetObjectInfo.ItemDBIdx,
		pPet->GetID());
	pPet->SetInited();

	g_pUserTable->AddUser(
		pPet,
		pPet->GetID());

	return pPet;
}

// 091214 ONS 펫 맵이동시 소환/봉인 관련 메세지를 출력하지 않도록 처리.
// 아이템소환/맵이동을 구분하기 위한 값을 클라이언트에 전달한다.
void CServerSystem::RemovePet( DWORD dwPetObjectID, BOOL bSummoned )
{
	CObject* pObject = g_pUserTable->FindUser(dwPetObjectID);
	
	if(!pObject)	return;

	CCharMove::ReleaseMove(pObject);
	CPet* pRemovePet =  ( CPet* )pObject;
	pRemovePet->DBInfoUpdate();
	PETMGR->RemoveObject(
		pRemovePet->GetObjectInfo().ItemDBIdx);
	PETMGR->Update(
		pRemovePet->GetObjectInfo());

	if(CObject* const ownerObject = g_pUserTable->FindUser(pRemovePet->GetOwnerIndex()))
	{
		MSG_DWORD_WORD  Msg;
		ZeroMemory( &Msg, sizeof(Msg) );
		Msg.Category = MP_USERCONN;
		Msg.Protocol = MP_USERCONN_HEROPET_REMOVE;
		Msg.dwData = pRemovePet->GetID();
		Msg.wData = WORD(bSummoned);

		ownerObject->SendMsg(
			&Msg,
			sizeof(Msg));
	}

	g_pObjectFactory->ReleaseObject(pObject);
	g_pUserTable->RemoveUser(dwPetObjectID);
}

void CServerSystem::RemoveMonster(DWORD dwMonster)
{
	CObject* pObject = g_pUserTable->FindUser(dwMonster);
	ASSERT(pObject);
	if(!pObject)	return;

	RemoveMonsterCount(*pObject);
	CCharMove::ReleaseMove(pObject);
	BATTLESYSTEM->DeleteObjectFromBattle(pObject);

	g_pAISystem.RemoveObject(dwMonster);
	g_pObjectFactory->ReleaseObject(pObject);
	g_pUserTable->RemoveUser(dwMonster);
}


void CServerSystem::RemoveBossMonster(DWORD dwMonster)
{
	CObject* pObject = g_pUserTable->FindUser(dwMonster);
	ASSERT(pObject);
	if(!pObject)	return;

	RemoveMonsterCount(*pObject);
	CCharMove::ReleaseMove(pObject);
	BATTLESYSTEM->DeleteObjectFromBattle(pObject);
	g_pObjectFactory->ReleaseObject(pObject);

	g_pAISystem.RemoveObject(dwMonster);
	g_pUserTable->RemoveUser(dwMonster);


//CONDITIONMGR->RemoveListToPool(dwMonster);

}

void CServerSystem::RemoveNpc(DWORD dwNpcID)
{
	CObject* pObject = g_pUserTable->FindUser(dwNpcID);
	ASSERT(pObject);
	if(!pObject)	return;
	
	CCharMove::ReleaseMove(pObject);
	BATTLESYSTEM->DeleteObjectFromBattle(pObject);
	
	if(!pObject)	return;

	g_pObjectFactory->ReleaseObject(pObject);
	g_pUserTable->RemoveUser(dwNpcID);
}

void CServerSystem::RemoveSkillObject(DWORD SkillObjectID)
{
	CObject* pObject = g_pUserTable->FindUser(SkillObjectID);
	//ASSERT(pObject);
	if(!pObject)	return;
	
	CCharMove::ReleaseMove(pObject);
	g_pObjectFactory->ReleaseObject(pObject);
	g_pUserTable->RemoveUser(SkillObjectID);
}

void CServerSystem::RemoveMapObject( DWORD MapObjID )
{
	CObject* pObject = g_pUserTable->FindUser(MapObjID);
	ASSERT(pObject);
	if(!pObject)	return;
	
	CCharMove::ReleaseMove(pObject);
	BATTLESYSTEM->DeleteObjectFromBattle(pObject);

	if(!pObject)	return;

	g_pObjectFactory->ReleaseObject(pObject);
	g_pUserTable->RemoveUser(MapObjID);
}


void CServerSystem::SendToOne(CObject* pObject,void* pMsg,int MsgLen)
{
	g_Network.Send2Server(pObject->GetAgentNum(),(char*)pMsg,MsgLen);
}

void CServerSystem::ReloadResourceData()	// case MP_CHEAT_RELOADING:
{
	SKILLMGR->Release();
	SKILLMGR->Init();
	GAMERESRCMNGR->LoadPlayerxMonsterPoint();
	ITEMMGR->ReloadItemList();

	g_pUserTable->SetPositionUserHead();
	CObject* pObject;
	while( (pObject = (CObject*)g_pUserTable->GetUserData()) != NULL )
	{
		if(pObject->GetObjectKind() == eObjectKind_Monster)
		{
			RemoveMonster(pObject->GetID());
		}
		else if(pObject->GetObjectKind() == eObjectKind_BossMonster)
		{
			RemoveBossMonster(pObject->GetID());
		}
		else if(pObject->GetObjectKind() == eObjectKind_Npc)
		{
			RemoveNpc(pObject->GetID());
		}
		
	}

	g_pAISystem.RemoveAllList();
	GAMERESRCMNGR->ResetMonsterList();
	GAMERESRCMNGR->LoadMonsterList();
	GAMERESRCMNGR->LoadMonsterRewardList();

	//SW050901
	MON_SPEECHMGR->LoadMonSpeechInfoList();
	g_pAISystem.LoadAIGroupList();

}

void CServerSystem::SetNation()
{
	CMHFile file;
	if( file.Init( "LocalizingInfo.txt", "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
	{

		if( strcmp( file.GetString(), "*NATION" ) == 0 )
		{
			if( strcmp( file.GetString(), "CHINA" ) == 0 )
			{
				m_Nation = eNATION_CHINA;
			}
		}

		file.Release();
	}

	CMHFile file2;
	if( file2.Init( "_TESTSERVER", "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
	{
		m_bTestServer = TRUE;
		file2.Release();
	}
}

void CServerSystem::RemoveServerForKind( WORD wSrvKind )
{
	g_pServerTable->RemoveServerForKind(wSrvKind);
}


void CServerSystem::LoadHackCheck()
{
	CMHFile file;
	if( !file.Init( "ServerSet/HackCheck.txt", "rt", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
	{
		g_Console.LOG(4, "HackCheckNum : %d, HackCheckWriteNum : %d", g_nHackCheckNum, g_nHackCheckWriteNum );
		return;
	}

	char temp[256] = {0, };
	while( 1 )
	{
		if( file.IsEOF() )	break;

		file.GetString( temp );
		if( strcmp( temp, "*HACKCHECKNUM" ) == 0 )
		{
			g_nHackCheckNum = file.GetInt();
		}
		else if( strcmp( temp, "*HACKCHECKWRITENUM" ) == 0 )
		{
			g_nHackCheckWriteNum = file.GetInt();
		}
		// 090205 LUJ, 광역 스킬 오차 범위
		else if( strcmp( temp, "*ALLOW_RANGE_FOR_WIDE_SKILL" ) == 0 )
		{
			cSkillObject::SetAllowRangeForWideSkill( file.GetFloat() );
		}
	}

	file.Release();

	g_Console.LOG(4, "HackCheckNum : %d, HackCheckWriteNum : %d", g_nHackCheckNum, g_nHackCheckWriteNum );
}

void CServerSystem::AddMonsterCount(CObject& object)
{
	if(IsUncountable(EObjectKind(object.GetObjectKind())))
	{
		return;
	}

	MonsterSet& monsterSet = mChannelMonsterMap[object.GetGridID()];
	monsterSet.insert(object.GetID());
}

void CServerSystem::RemoveMonsterCount(CObject& object)
{
	if(IsUncountable(EObjectKind(object.GetObjectKind())))
	{
		return;
	}

	MonsterSet& monsterSet = mChannelMonsterMap[object.GetGridID()];
	monsterSet.erase(object.GetID());
}

BOOL CServerSystem::IsUncountable(EObjectKind kind) const
{
	switch(kind)
	{
	case eObjectKind_Monster:
	case eObjectKind_BossMonster:
	case eObjectKind_SpecialMonster:
	case eObjectKind_FieldBossMonster:
	case eObjectKind_FieldSubMonster:
	case eObjectKind_ToghterPlayMonster:
	case eObjectKind_ChallengeZoneMonster:
		{
			return FALSE;
		}
	}

	return TRUE;
}

DWORD CServerSystem::GetMonsterCount(DWORD gridIndex) const
{
	const ChannelMonsterMap::const_iterator iter = mChannelMonsterMap.find(gridIndex);

	if(mChannelMonsterMap.end() == iter)
	{
		return 0;
	}

	const MonsterSet& monsterSet = iter->second;
	return monsterSet.size();
}

void CServerSystem::RemoveMonsterInGrid(DWORD gridIndex)
{
	const ChannelMonsterMap::const_iterator channelIterr = mChannelMonsterMap.find(gridIndex);

	if(mChannelMonsterMap.end() == channelIterr)
	{
		return;
	}

	const MonsterSet& monsterSet = channelIterr->second;

	for(MonsterSet::const_iterator monsterIter = monsterSet.begin();
		monsterSet.end() != monsterIter;
		++monsterIter)
	{
		const DWORD objectIndex = *monsterIter;
		CMonster* const monster = (CMonster*)g_pUserTable->FindUser(objectIndex);

		if(0 == monster)
		{
			continue;
		}

		const BOOL isNotMonster = !(monster->GetObjectKind() & eObjectKind_Monster);

		if(isNotMonster)
		{
			continue;
		}

		// 100204 LUJ, 몬스터를 강제로 삭제할 때에는 리젠되지 않도록 그룹 번호를 무효 값으로 설정한다
		MONSTER_TOTALINFO& totalInfo = monster->GetMonsterTotalInfo();
		totalInfo.Group = USHRT_MAX;
		monster->Die(0);
	}
}

void CServerSystem::ToggleCheckProcessTime()
{
	m_bCheckProcessTime = ! m_bCheckProcessTime;

	if(m_bCheckProcessTime)
	{
		m_fTimeObjLoopProc = 0.0f;
		m_fTimeTriggerProc = 0.0f;
		m_fTimeDungeonProc = 0.0f;

		m_fAvrObjLoopProc = 0.0f;
		m_fAvrTriggerProc = 0.0f;
		m_fAvrDungeonProc = 0.0f;
		m_dwProcessCount = 0;
	}
}

BOOL CServerSystem::IsNoRecallMap(CObject& object)
{
	if(GuildTournamentStadium == GetMapNum())
	{
		return TRUE;
	}
	else if(DungeonMGR->IsDungeon(GetMapNum()))
	{
		return TRUE;
	}
	else if(g_csDateManager.IsChallengeZone(GetMapNum()))
	{
		return TRUE;
	}

	const LimitDungeonScript& script = GAMERESRCMNGR->GetLimitDungeonScript(
		GetMapNum(),
		object.GetGridID());

	if(GetMapNum() == script.mMapType)
	{
		return TRUE;
	}

	return FALSE;
}

// 080813 LUJ, 수정된 inetwork 호출 형식에 따라 변경
void __stdcall OnConnectServerSuccess(DWORD dwConnectionIndex, void* pVoid)
{
	SERVERINFO* info = (SERVERINFO*)pVoid;
	info->dwConnectionIndex = dwConnectionIndex;
	if(info->wServerKind == MONITOR_SERVER)
	{
		BOOTMNGR->NotifyBootUpToMS(&g_Network);
		g_Console.LOG(4, "Connected to the MS : %s, %d, (%d)", info->szIPForServer, info->wPortForServer, dwConnectionIndex);
	}
	else
	{
		BOOTMNGR->SendConnectSynMsg(&g_Network, dwConnectionIndex, g_pServerTable);
		g_Console.LOG(4, "Connected to the Server : %s, %d, (%d)", info->szIPForServer, info->wPortForServer, dwConnectionIndex);
	}

	if(g_pServerTable->GetMaxServerConnectionIndex() < dwConnectionIndex)
			g_pServerTable->SetMaxServerConnectionIndex(dwConnectionIndex);
}

// 080813 LUJ, 수정된 inetwork 호출 형식에 따라 변경
void __stdcall OnConnectServerFail(void* pVoid)
{
	SERVERINFO* info = (SERVERINFO*)pVoid;
	if(info->wServerKind == MONITOR_SERVER)
	{
		BOOTMNGR->AddBootListINI(MAP_SERVER, GAMERESRCMNGR->GetLoadMapNum(), g_pServerTable);
		BOOTMNGR->BactchConnectToMap(&g_Network, g_pServerTable);
		g_Console.LOG(4, "Failed to Connect to the MS : %s, %d", info->szIPForServer, info->wPortForServer);
	}
	else
	{
		g_Console.LOG(4, "Failed to Connect to the Server : %s, %d", info->szIPForServer, info->wPortForServer);
		BOOTMNGR->RemoveBootList(g_pServerTable, info->wPortForServer);
	}	
}

// 080813 LUJ, 수정된 inetwork 호출 형식에 따라 변경
void __stdcall OnAcceptServer(DWORD dwConnectionIndex)
{
	g_Console.LOG(4, "Server Connected : ConnectionIndex %d", dwConnectionIndex);
}

// 080813 LUJ, 수정된 inetwork 호출 형식에 따라 변경
void __stdcall OnDisconnectServer(DWORD dwConnectionIndex)
{
	if( g_pServerSystem->GetStart() == FALSE ) return;

	g_Console.LOG(4, "Server Disconnected : ConnectionIndex %d", dwConnectionIndex);

	SERVERINFO * delInfo = g_pServerTable->RemoveServer(dwConnectionIndex);
	if(!delInfo)
	{
		ASSERT(0);
	}
	delete delInfo;


	// taiyo
	g_pUserTable->SetPositionUserHead();
	CObject * info = NULL;
	cPtrList list;

	while( (info = g_pUserTable->GetUserData()) != NULL )
	{
		if(info->GetObjectKind() == eObjectKind_Player)
		{
			CPlayer * ply = (CPlayer *)info;
			if(ply->GetAgentNum() == dwConnectionIndex)
			{
				SaveMapChangePointUpdate(ply->GetID(), 0);
				//g_pServerSystem->RemovePlayer(ply->GetID());
				list.AddTail( ply );
			}
		}
	}
	PTRLISTPOS pos = list.GetHeadPosition();
	while( pos )
	{
		CPlayer* p = (CPlayer*)list.GetNext(pos);
		g_pServerSystem->RemovePlayer(p->GetID());
	}
	list.RemoveAll();
}

// 080813 LUJ, 수정된 inetwork 호출 형식에 따라 변경
void __stdcall OnAcceptUser(DWORD dwConnectionIndex)
{
}

// 080813 LUJ, 수정된 inetwork 호출 형식에 따라 변경
void __stdcall OnDisconnectUser(DWORD dwConnectionIndex)
{

}

// 080813 LUJ, 수정된 inetwork 호출 형식에 따라 변경
void __stdcall ReceivedMsgFromServer(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength)
{
	MSGROOT* pTempMsg = reinterpret_cast<MSGROOT*>(pMsg);
	ASSERT(pTempMsg->Category < MP_MAX);
	
	if( g_pServerMsgParser[pTempMsg->Category] == NULL ||
		pTempMsg->Category >= MP_MAX ||
		pTempMsg->Category == 0)
		return;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			g_pServerMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
		}
		catch(...)
		{
			char szTxt[256];
			sprintf(szTxt, "M:%02d  ReceivedMsgFromServer, %u, %u, %u, %u", g_pServerSystem->GetMapNum(), dwConnectionIndex, pTempMsg->Category, pTempMsg->Protocol, dwLength);
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		g_pServerMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
	}
}

// 080813 LUJ, 수정된 inetwork 호출 형식에 따라 변경
void __stdcall ReceivedMsgFromUser(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength)
{
	MSGROOT* pTempMsg = reinterpret_cast<MSGROOT*>(pMsg);
	ASSERT(pTempMsg->Category < MP_MAX);
	ASSERT(g_pUserMsgParser[pTempMsg->Category]);
	
	if( g_pUserMsgParser[pTempMsg->Category] == NULL ||
		pTempMsg->Category >= MP_MAX ||
		pTempMsg->Category == 0)
		return;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			g_pUserMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
		}
		catch(...)
		{
			char szTxt[256];
			sprintf(szTxt, "M:%02d  ReceivedMsgFromUser, %u, %u, %u, %u", g_pServerSystem->GetMapNum(), dwConnectionIndex, pTempMsg->Category, pTempMsg->Protocol, dwLength);
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		g_pUserMsgParser[pTempMsg->Category](dwConnectionIndex, pMsg, dwLength);
	}
}

void __stdcall ProcessServer(DWORD)
{
	g_DB.ProcessingDBMessage();
}

void __stdcall ProcessGame(DWORD)
{
	g_pServerSystem->Process();
}

void __stdcall ProcessCheck(DWORD)
{
	ConnectionCheck();
	NPCMOVEMGR->Process();
}

void ButtonProc1()
{
	g_Console.LOG( 4, "QueryCount: %d, GetDB: %p", g_DB.GetQueryQueueCount(), g_DB.GetCurDB() );
}


extern BOOL g_bWriteQuery;
void ButtonProc2()
{
	g_pServerSystem->LoadHackCheck();
}

void ButtonProc3()
{
	if( g_pServerSystem )
	{
		g_Console.LOG(
			4,
			"%d: space: %dms(avg: %0.1fms, max: %dms) process time: %dms",
			processTime.mCount,
			processTime.mSpace,
			processTime.mAverageSpace,
			processTime.mMaxSpace,
			g_pServerSystem->GetMainProcessTime() );
	}
}

void ButtonProc4()
{
	static BOOL isUseConsole;

	if(isUseConsole)
	{
		FreeConsole();
	}
	else
	{
		// TODO: 콘솔에 Ctrl+C 메시지를 보내면 어플리케이션이 정지한다.. - -; 콘솔 핸들러로도 안된다. 왜 그럴까...
		AllocConsole();
		char text[MAX_PATH] = {0};
		sprintf(
			text,
			"MAP%02d(%s)",
			g_pServerSystem->GetMapNum(),
			GetMapName(g_pServerSystem->GetMapNum()));
		SetConsoleTitle(text);
	}

	isUseConsole = ! isUseConsole;
}

void ButtonToggleStatOfTrigger()
{
	TRIGGERMGR->ToggleStatistic();
}

void ButtonTogglePeriodicMessageOfTrigger()
{
	TRIGGERMGR->TogglePeriodicMessage();
}

void ButtonToggleProcessTime()
{
	g_pServerSystem->ToggleCheckProcessTime();
}

void OnCommand(char* szCommand)
{
	if (stricmp(szCommand, "MONSTERMETER_LOAD_REWARD_SCRIPT") == 0 || 
		stricmp(szCommand, "MM_LRS") == 0)
	{
		g_csMonstermeterManager.LoadScriptFileData();
		g_Console.LOG(4, "- Done Cmd:MONSTERMETER_LOAD_REWARD_SCRIPT -");
	}

	if (stricmp(szCommand, "CHECK_TRYCATCH") == 0)
	{
		CSHDebug::SetExceptionProcGrade(!CSHDebug::GetExceptionProcGrade());
		g_Console.LOG(4, "- TryCatchGrade: %d", CSHDebug::GetExceptionProcGrade());
	}

	if (stricmp(szCommand, "CHANGEITEM_LOAD") == 0)
	{
		ITEMMGR->LoadScriptFileDataChangeItem();
		g_Console.LOG(4, "- Done Cmd:CHANGEITEM_LOAD");
	}
}

BOOL LoadEventRate(char* strFileName)
{
	for(int i=0; i<eEvent_Max; ++i)	
	{
		gEventRate[i] = 1.f;
		gEventRateFile[i] = 1.f;
	}

	CMHFile file;
	// 080118 KTH -- 일반 모드로 파일을 연다.
	if( !file.Init( strFileName, "r", MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR) )
		return FALSE;

	char Token[256];
	
// RaMa -04.11.24
	while( !file.IsEOF() )
	{
		file.GetString(Token);

		if(strcmp(Token,"#EXP") == 0)	
		{			
			//gExpRate = file.GetFloat();
			gEventRateFile[eEvent_ExpRate] = file.GetFloat();
		}
		else if(strcmp(Token, "#ABIL") == 0)
		{
			//gAbilRate = file.GetFloat();
			gEventRateFile[eEvent_AbilRate] = file.GetFloat();
		}
		else if(strcmp(Token,"#ITEM") == 0)
		{
			//gItemRate = file.GetFloat();
			gEventRateFile[eEvent_ItemRate] = file.GetFloat();
		}
		else if(strcmp(Token,"#MONEY") == 0)
		{
			//gMoneyRate = file.GetFloat();
			gEventRateFile[eEvent_MoneyRate] = file.GetFloat();
		}
		else if(strcmp(Token,"#GETMONEY") == 0)
		{
			//gGetMoney = file.GetFloat();
			gEventRateFile[eEvent_GetMoney] = file.GetFloat();
		}
		else if(strcmp(Token,"#DAMAGERECIVE") == 0)
		{
			//gDamageReciveRate = file.GetFloat();
			gEventRateFile[eEvent_DamageReciveRate] = file.GetFloat();
		}
		else if(strcmp(Token,"#DAMAGERATE") == 0)
		{
			//gDamageRate = file.GetFloat();
			gEventRateFile[eEvent_DamageRate] = file.GetFloat();
		}
		else if(strcmp(Token,"#MANASPEND") == 0)
		{
			//gNaeRuykRate = file.GetFloat();
			gEventRateFile[eEvent_ManaRate] = file.GetFloat();
		}
		else if(strcmp(Token,"#UNGISPEED") == 0)
		{
			//gUngiSpeed = file.GetFloat();
			gEventRateFile[eEvent_UngiSpeed] = file.GetFloat();
		}
		else if(strcmp(Token,"#PARTYEXP") == 0)
		{
			//gPartyExpRate = file.GetFloat();
			gEventRateFile[eEvent_PartyExpRate] = file.GetFloat();
		}
		else if(strcmp(Token,"#SKILLEXPRATE") == 0)
		{
			gEventRateFile[eEvent_SkillExp] = file.GetFloat();
		}		
	}


	file.Release();

	for(i=0; i<eEvent_Max; ++i)	
		gEventRate[i] = gEventRateFile[i];

	return TRUE;
}

DWORD CServerSystem::AddGameRoom(LPCTSTR address, LPCTSTR name, LPCTSTR upperIndex, LPCTSTR lowerIndex)
{
	GameRoom& gameRoom = mGameRoomContainer[GetGameRoomKey(address)];
	ZeroMemory(
		&gameRoom,
		sizeof(gameRoom));
	SafeStrCpy(
		gameRoom.mName,
		name,
		sizeof(gameRoom.mName) / sizeof(*gameRoom.mName));
	_stprintf(
		gameRoom.mDbIndex,
		"%s%s",
		upperIndex,
		lowerIndex);

	if(0 < _tcslen(gameRoom.mDbIndex))
	{
		gameRoom.mIndex = TRIGGERMGR->GetHashCode(gameRoom.mDbIndex);
	}

	// 100216 ShinJS --- 다음 DB Load 시각 저장(1시간후)
	const DWORD dwCheckTime = 60*60*1000;
	gameRoom.mDBLoadTime = gCurTime + dwCheckTime;

	return gameRoom.mIndex;
}

ULONGLONG CServerSystem::GetGameRoomKey(LPCTSTR address) const
{
	TCHAR buffer[MAX_PATH] = {0};
	SafeStrCpy(
		buffer,
		address,
		sizeof(buffer) / sizeof(*buffer));
	ULONGLONG key = 0;
	LPCTSTR seperator = ".";

	// 091228 LUJ, 문자열을 수치로 변경한다(예: 192.168.1.130 -> 192168001130)
	for(LPCTSTR token = _tcstok(buffer, seperator);
		0 < token;
		token = _tcstok(0, seperator))
	{
		key = key * 1000;
		key = key + _ttoi(token);
	}

	return key;
}

void CServerSystem::UpdateFiniteStateMachine(CMonster& monster, DWORD subIndex) const
{
	CAIGroup* const aiGroup = GROUPMGR->GetGroup(
		monster.GetMonsterGroupNum(),
		monster.GetGridID());

	if(0 == aiGroup)
	{
		return;
	}

	const CAIGroup::Parameter* const parameter = aiGroup->GetRegenObject(
		subIndex);

	if(0 == parameter)
	{
		return;
	}
	else if(FALSE == monster.GetFiniteStateMachine().Initialize(
		parameter->mMachine,
		monster.GetID(),
		monster.GetGridID()))
	{
		return;
	}

	MONSTER_TOTALINFO& monsterTotalInfo = monster.GetMonsterTotalInfo();
	SafeStrCpy(
		monsterTotalInfo.mScriptName,
		parameter->mMachine,
		_countof(monsterTotalInfo.mScriptName));
}

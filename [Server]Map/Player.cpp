#include "stdafx.h"
#include "Player.h"
#include "PartyManager.h"
#include "Party.h"
#include "CharMove.h"
#include "PackedData.h"
#include "MapDBMsgParser.h"
#include "ItemManager.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "ObjectStateManager.h"
#include "streetstallmanager.h"
#include "CharacterCalcManager.h"
#include "GuildManager.h"
#include "SkillTreeManager.h"
#include "GridSystem.h"
#include "GridTable.h"
#include "../[cc]skill/Server/info/BuffSkillInfo.h"
#include "../[cc]skill/Server/info/ActiveSkillInfo.h"
#include "Showdown/Battle_Showdown.h"
#include "StateMachinen.h"
#include "MHError.h"
#include "CQuestBase.h"
#include "Quest.h"
#include "TileManager.h"
#include "mhtimemanager.h"
#include "LootingManager.h"
#include "PKManager.h"
#include "QuestManager.h"
#include "StorageManager.h"
#include "UserTable.h"
#include "GuildFieldWarMgr.h"
#include "QuestMapMgr.h"
#include "GuildTournamentMgr.h"
#include "StreetStall.h"
#include "QuickManager.h"
#include "FishingManager.h"
#include "../[CC]BattleSystem/BattleSystem_Server.h"
#include "../[cc]skill/server/tree/SkillTree.h"
#include "../[CC]Skill\Server\Delay\Delay.h"
#include "AttackManager.h"
#include "../hseos/Common/SHMath.h"
#include "../hseos/Monstermeter/SHMonstermeterManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "../hseos/Date/SHDateManager.h"
#include "../[cc]skill/server/Object\BuffSkillObject.h"
#include "../[cc]skill/server/Object/ActiveSkillObject.h"
#include "Guild.h"
#include "Network.h"
#include "MapNetworkMsgParser.h"
#include "Pet.h"
#include "petmanager.h"
#include "LimitDungeonMgr.h"
#include "./SiegeWarfareMgr.h"
#include "../[CC]SiegeDungeon/SiegeDungeonMgr.h"
#include "Vehicle.h"
#include "HousingMgr.h"
#include "Trigger\Manager.h"
#include "Trigger\Message.h"
#include "PCRoomManager.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "Dungeon\DungeonMgr.h"
#include "..\hseos\ResidentRegist\SHResidentRegistManager.h"

#define INSERTLOG_TIME			600000 // 10 min
#define INSERTLOG_TIME_CHINA	1800000 // 30 min
#define INSERTLOG_TIME_HK		1800000 // 30 min

extern int	g_nHackCheckNum;
extern int	g_nHackCheckWriteNum;

CPlayer::CPlayer() :
m_SkillTree(new cSkillTree)
{
	m_SkillFailCount = 0;
	mpBattleStateDelay = new cDelay;
	mpBattleStateDelay->Init( 10000 );

	m_God = FALSE;

	mIsEmergency = FALSE;

	mTargetMap = 0;
	mTargetPosX = 0;
	mTargetPosZ = 0;

	// 071128 LYW --- Player : HPMP 적용.
	m_byHP_Point = 0;
	m_byMP_Point = 0;

	m_bResetSkill	=	false;
	m_bResetStat	=	false;
	m_ItemArrayList.Initialize(10);
	m_FollowMonsterList.Initialize(5);
	m_QuestList.Initialize(30);
	m_dweFamilyRewardExp = 0;
	m_byCurFamilyMemCnt	 = 0;
	m_dwCurrentResurrectIndex = 0;
}

CPlayer::~CPlayer()
{
	delete mpBattleStateDelay;
	SAFE_DELETE(m_SkillTree);
}

void CPlayer::InitClearData()
{
	m_ItemContainer.Init();
	//m_ItemContainer.SetInit(eItemTable_Inventory,	TP_INVENTORY_START,		SLOT_INVENTORY_NUM,		&m_InventorySlot);
	m_ItemContainer.SetInit(eItemTable_Inventory,	TP_INVENTORY_START,		SLOT_MAX_INVENTORY_NUM,		&m_InventorySlot);

	m_ItemContainer.SetInit(eItemTable_Weared,		TP_WEAR_START,			SLOT_WEAR_NUM,			&m_WearSlot);
	m_ItemContainer.SetInit(eItemTable_Storage,		TP_STORAGE_START,		SLOT_STORAGE_NUM,		&m_StorageSlot);
	m_ItemContainer.SetInit(eItemTable_Shop,		TP_SHOPITEM_START,		SLOT_SHOPITEM_NUM,		&m_ShopItemSlot);	
	memset(&m_HeroCharacterInfo,0,sizeof(CHARACTER_TOTALINFO));
	memset(&m_HeroInfo,0,sizeof(HERO_TOTALINFO));
	// 090701 LUJ, 메모리풀이 생성자/소멸자를 호출하지 않기 때문에 Init()에서 초기화하지 않을 경우, 초기화가
	//		이뤄지지 않는다. purse는 그 동안 초기화를 하지 않았으므로 플레이어가 접속을 해제해도 정보(쓰레기값)를
	//		갖고 있다. 이를 이용해서 DB에서 값을 가져오기 전에 빠르게 맵 이동을 반복할 경우, 쓰레기값을 DB에 저장하게된다.
	//		이를 막기 위해 객체 초기화 순간에 값도 초기화시키도록 한다
	m_InventoryPurse.SetZeroMoney();
	m_StoragePurse.SetZeroMoney();

	m_QuestGroup.Initialize( this );
	m_wKillMonsterLevel = 0;
	m_bGotWarehouseItems = FALSE;
	m_SkillFailCount = 0;
	mGravity = 0;

	memset( &m_DateMatching, 0, sizeof(DATE_MATCHING_INFO));			// 데이트 매칭 주민증.
	// desc_hseos_주민등록01
	// S 주민등록 추가 added by hseos 2007.06.09
	m_DateMatching.nSerchTimeTick = gCurTime;
	m_DateMatching.nRequestChatTimeTick = gCurTime;
	// E 주민등록 추가 added by hseos 2007.06.09
	memset( &mPassiveStatus, 0, sizeof( Status ) );
	memset( &mBuffStatus, 0, sizeof( Status ) );
	memset( &mRatePassiveStatus, 0, sizeof( Status ) );
	memset( &mRateBuffStatus, 0, sizeof( Status ) );
	memset( &mAbnormalStatus, 0, sizeof( AbnormalStatus ) );

	// desc_hseos_몬스터미터01
	// S 몬스터미터 추가 added by hseos 2007.05.23
	ZeroMemory(&m_stMonstermeterInfo, sizeof(m_stMonstermeterInfo));
	m_stMonstermeterInfo.nPlayTimeTick = gCurTime;
	m_pcFamilyEmblem = NULL;
	ZeroMemory(&m_stFarmInfo, sizeof(m_stFarmInfo));
	m_stFarmInfo.nCropPlantRetryTimeTick  = gCurTime;
	m_stFarmInfo.nCropManureRetryTimeTick = gCurTime;
	// 080519 KTH
	m_stFarmInfo.nAnimalFeedRetryTimeTick = gCurTime;
	m_stFarmInfo.nAnimalCleanRetryTimeTick = gCurTime;
	m_nChallengeZoneEnterFreq = 0;
	m_nChallengeZoneEnterBonusFreq = 0;
	m_nChallengeZonePartnerID = 0;
	m_nChallengeZoneSection = 0;
	m_nChallengeZoneStartState = 0;
	m_nChallengeZoneStartTimeTick = 0;
	m_nChallengeZoneMonsterNum = 0;
	m_nChallengeZoneKillMonsterNum = 0;
	m_bChallengeZoneNeedSaveEnterFreq = FALSE;
	m_bChallengeZoneCreateMonRightNow = FALSE;
	m_nChallengeZoneMonsterNumTillNow = 0;
	m_nChallengeZoneClearTime = 0;
	m_nChallengeZoneExpRate   = 0;

	m_God = FALSE;
	mIsEmergency = FALSE;

	mTargetMap = 0;
	mTargetPosX = 0;
	mTargetPosZ = 0;

	m_bResetSkill	=	false;
	m_bResetStat	=	false;

	m_dwReturnSkillMoney = 0;

//---KES AUTONOTE
	m_dwAutoNoteIdx = 0;
	m_dwLastActionTime = 0;
//---------------

	FishingInfoClear();
	m_dwFM_MissionCode = 1000000;

	int i;
	for(i=0; i<eFishItem_Max; i++)
	{
		m_fFishItemRate[i] = 0.0f;
	}
	m_lstGetFishList.clear();
	

	m_wFishingLevel = 1;
	m_dwFishingExp = 0;
	m_dwFishPoint = 0;
	mReviveFlag = ReviveFlagNone;
	m_dwKillCountFromGT = 0;
	m_dwKillPointFromGT = 0;
	m_dwRespawnTimeOnGTMAP = 0;
	m_dwImmortalTimeOnGTMAP = 0;
	m_wCookLevel = 1;
	m_wCookCount = 0;
	m_wEatCount = 0;
	m_wFireCount = 0;
	m_dwLastCookTime = 0;
	memset(m_MasterRecipe, 0, sizeof(m_MasterRecipe));
	// 090316 LUJ, 탈 것 초기화
	SetSummonedVehicle( 0 );
	SetMountedVehicle( 0 );
	m_initState = 0;

	mWeaponEnchantLevel = 0;
	mPhysicDefenseEnchantLevel = 0;
	mMagicDefenseEnchantLevel = 0;

	strcpy(m_szHouseName, "");

	m_dwCurrentResurrectIndex = 0;
	mPetIndex = 0;
	m_bDungeonObserver = FALSE;

	m_dwConsignmentTick = 0;
	ForbidChatTime = 0;
}

BOOL CPlayer::Init(EObjectKind kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo)
{
	m_StreetStallTitle[0] = 0;
	m_MurdererIDX = 0;
	m_MurdererKind = 0;
	m_bPenaltyByDie = FALSE;

	m_bReadyToRevive = FALSE;
	m_bShowdown	= FALSE;
	m_bExit = FALSE;
	m_bNormalExit = FALSE;
//
	CObject::Init(kind, AgentNum, pBaseObjectInfo); //¿ⓒ±a¼­´A eObjectState_NoneA¸·I ¸¸μc´U.

//KES 040827
	OBJECTSTATEMGR_OBJ->StartObjectState( this, eObjectState_Immortal, 0 );
	// 06.08.29. RaMa.
	OBJECTSTATEMGR_OBJ->EndObjectState( this, eObjectState_Immortal, 30000 );
//

	m_CurComboNum = 0;
	m_pGuetStall = 0;
	m_bNeedRevive = TRUE;
	ZeroMemory(
		&m_GameOption,
		sizeof(m_GameOption));
	m_ContinuePlayTime = 0;

//HACK CHECK INIT
	m_nHackCount = 0;
	m_dwHackStartTime = gCurTime;
	m_wKillMonsterLevel = 0;
	m_bDieForGFW = FALSE;
	m_bWaitExitPlayer = TRUE;
	m_bWaitExitPlayer = TRUE;

//	m_SkillList.RemoveAll();
	m_SkillFailCount = 0;

	for( BYTE i = 0; i < 8; i++ )
	{
		m_QuickSlot[i].Init( GetID(), i );
	}

	m_SkillTree->Init( this );
	m_stMonstermeterInfo.nPlayTimeTick = gCurTime;
	mIsEmergency = FALSE;

	mTargetMap = 0;
	mTargetPosX = 0;
	mTargetPosZ = 0;

	m_dwLastTimeCheckItemDBUpdate = gCurTime;
	m_dwLastTimeCheckItem = gCurTime;

	m_bNoExpPenaltyByPK = FALSE;

	// 080515 LUJ, 쿨타임 체크용 구조체 초기화
	ZeroMemory( &mCheckCoolTime, sizeof( mCheckCoolTime ) );

	m_dwSkillCancelCount = 0;
	m_dwSkillCancelLastTime = 0;

	m_dwAutoNoteLastExecuteTime = 0;
	
	mWeaponEnchantLevel = 0;
	mPhysicDefenseEnchantLevel = 0;
	mMagicDefenseEnchantLevel = 0;

	m_dwCurrentResurrectIndex = 0;

	ZeroMemory( &m_YYManaRecoverTime, sizeof(m_YYManaRecoverTime) );
	m_ManaRecoverDirectlyAmount = 0;

	return TRUE;
}
void CPlayer::Release()
{
	RemoveAllAggroed();
//-------------------

	FishingData_Update(GetID(), GetFishingLevel(), GetFishingExp(), GetFishPoint());
	Cooking_Update(this);

	if(FISHINGMGR->GetActive())		// 낚시스크립트가 활성되어 있을때(==2번 농장맵)만 기록.
	{
		// 080808 LUJ, 낚시 정보를 로그로 저장
		Log_Fishing(
			GetID(),
			eFishingLog_Regular,
			0,
			GetFishPoint(),
			0,
			0,
			GetFishingExp(),
			GetFishingLevel() );
	}

	LogOnRelease();
	GUILDMGR->RemovePlayer( this );
	ITEMMGR->RemoveCoolTime( GetID() );
	mCoolTimeMap.clear();
	mSkillCoolTimeMap.clear();
	mSkillAnimTimeMap.clear();

	{
		for( POSTYPE position = 0; position < m_ItemContainer.GetSize(); ++position )
		{
			const ITEMBASE* item = m_ItemContainer.GetItemInfoAbs( position );

			if( ! item )
			{
				continue;
			}

			ITEMMGR->RemoveOption( item->dwDBIdx );
		}
	}

	{
		m_ItemArrayList.SetPositionHead();

		for(
			ITEMOBTAINARRAYINFO* pInfo;
			(pInfo = m_ItemArrayList.GetData())!= NULL; )
		{
			ITEMMGR->Free(this, pInfo);
		}

		m_ItemArrayList.RemoveAll();
	}

	m_FollowMonsterList.RemoveAll();
	
	{		
		m_QuestList.SetPositionHead();

		for(
			CQuestBase* pQuest;
			(pQuest = m_QuestList.GetData())!= NULL; )
		{
			delete pQuest;
		}

		m_QuestList.RemoveAll();
	}
	

	m_InventoryPurse.Release();
	m_StoragePurse.Release();
	m_QuestGroup.Release();
	CObject::Release();

	mSpecialSkillList.clear();
	m_SkillFailCount = 0;
	m_SkillTree->Release();

	SAFE_DELETE_ARRAY(m_pcFamilyEmblem);	

	{
		CPet* const petObject = PETMGR->GetPet(
			GetPetItemDbIndex());

		g_pServerSystem->RemovePet(
			petObject ? petObject->GetID() : 0,
			FALSE);
	}

	// 090316 LUJ, 하차 처리한다
	{
		CVehicle* const vehicleObject = ( CVehicle* )g_pUserTable->FindUser( GetMountedVehicle() );

		if( vehicleObject &&
			vehicleObject->GetObjectKind() == eObjectKind_Vehicle )
		{
			vehicleObject->Dismount( GetID(), TRUE );
		}
	}

	mWeaponEnchantLevel = 0;
	mPhysicDefenseEnchantLevel = 0;
	mMagicDefenseEnchantLevel = 0;
}

void CPlayer::UpdateGravity()
{
	mGravity = 0;
	m_FollowMonsterList.SetPositionHead();

	while(CObject* const pObject = m_FollowMonsterList.GetData())
	{
		mGravity += pObject->GetGravity();
	}
}

BOOL CPlayer::AddFollowList(CMonster * pMob)
{
	if( m_FollowMonsterList.GetDataNum() < 50 )		//max 50마리
	{
		m_FollowMonsterList.Add(pMob, pMob->GetID());
		UpdateGravity();
		return TRUE;
	}

	return FALSE;
}
BOOL CPlayer::RemoveFollowAsFarAs(DWORD GAmount, CObject* pMe )
{	
	VECTOR3 * ObjectPos	= CCharMove::GetPosition(this);
	BOOL bMe = FALSE;

	while(GAmount > 100)
	{	
		CMonster * MaxObject = NULL;
		float	MaxDistance	= -1;
		float	Distance	= 0;

		m_FollowMonsterList.SetPositionHead();
		while(CMonster* pObject = m_FollowMonsterList.GetData())
		{
			VECTOR3 * TObjectPos = CCharMove::GetPosition(pObject);
			if((Distance = CalcDistanceXZ( ObjectPos, TObjectPos )) > MaxDistance)
			{
				MaxDistance = Distance;
				MaxObject = pObject;
			}
		}

		if(MaxObject)
		{
			if(GAmount > MaxObject->GetGravity())
				GAmount -= MaxObject->GetGravity();
			else
				GAmount = 0;

			MaxObject->SetTObject(NULL);

			GSTATEMACHINE.SetState(MaxObject, eMA_WALKAROUND);

			if( pMe == MaxObject )
				bMe = TRUE;
		}
		else
		{
			MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("amount != 0"));
			GAmount = 0;
		}
	}

	return bMe;	
}

void CPlayer::RemoveFollowList(DWORD ID)
{
	m_FollowMonsterList.Remove(ID);
	UpdateGravity();
}

ITEMOBTAINARRAYINFO * CPlayer::GetArray(WORD id)
{
	return m_ItemArrayList.GetData(id);
}

void CPlayer::AddArray(ITEMOBTAINARRAYINFO * pInfo)
{
	m_ItemArrayList.Add(pInfo, pInfo->wObtainArrayID);
}
void CPlayer::RemoveArray(DWORD id)
{
	m_ItemArrayList.Remove(id);
}
void CPlayer::InitCharacterTotalInfo(CHARACTER_TOTALINFO* pTotalInfo)
{
	memcpy(&m_HeroCharacterInfo,pTotalInfo,sizeof(CHARACTER_TOTALINFO));
	if( GetUserLevel() == eUSERLEVEL_GM )
		m_HeroCharacterInfo.bVisible = FALSE;
	else
		m_HeroCharacterInfo.bVisible = TRUE;

	// 071226 KTH -- 인벤토리의 크기를 설정하여 준다.
	m_InventorySlot.SetSlotNum( POSTYPE( SLOT_INVENTORY_NUM + GetInventoryExpansionSize() ) );

}

void CPlayer::InitHeroTotalInfo(HERO_TOTALINFO* pHeroInfo)
{
	memcpy(&m_HeroInfo,pHeroInfo,sizeof(HERO_TOTALINFO));
	m_ItemContainer.GetSlot(eItemTable_Inventory)->CreatePurse(&m_InventoryPurse, this, m_HeroInfo.Money, MAX_INVENTORY_MONEY);
}

void CPlayer::InitItemTotalInfo(ITEM_TOTALINFO* pItemInfo)
{
	m_ItemContainer.GetSlot(eItemTable_Inventory)->SetItemInfoAll(pItemInfo->Inventory);	
	m_ItemContainer.GetSlot(eItemTable_Weared)->SetItemInfoAll(pItemInfo->WearedItem);
}

void CPlayer::AddStorageItem(ITEMBASE * pStorageItem)
{
	CStorageSlot * pSlot = (CStorageSlot *)m_ItemContainer.GetSlot(eItemTable_Storage);
	pSlot->InsertItemAbs(this, pStorageItem->Position, pStorageItem);
}

void CPlayer::InitStorageInfo(BYTE Storagenum,MONEYTYPE money)
{
	CStorageSlot * pSlot = (CStorageSlot *)m_ItemContainer.GetSlot(eItemTable_Storage);
	pSlot->SetStorageNum(Storagenum);

	MONEYTYPE maxmoney = 0;
	if(Storagenum)
	{		
		STORAGELISTINFO* pInfo = STORAGEMGR->GetStorageInfo(Storagenum);
		ASSERT(pInfo);
		maxmoney = pInfo ? pInfo->MaxMoney : 10;
	}
	else
	{
		ASSERT(money == 0);
		maxmoney = 0;
	}
	pSlot->CreatePurse(&m_StoragePurse, this, money, maxmoney);	
}

void CPlayer::InitShopItemInfo(SEND_SHOPITEM_INFO& message)
{
	CItemSlot* const pSlot = m_ItemContainer.GetSlot(
		eItemTable_Shop);

	for(DWORD i = 0; i < _countof(message.Item); ++i)
	{
		pSlot->ClearItemBaseAndSlotInfo(
			POSTYPE(TP_SHOPITEM_START + i));

		ITEMBASE& itemBase = message.Item[i];

		if(0 == itemBase.dwDBIdx)
		{
			continue;
		}

		pSlot->InsertItemAbs(
			this,
			itemBase.Position,
			&itemBase);
	}
}

void CPlayer::SetStorageNum(BYTE n)
{
	CStorageSlot * pSlot = (CStorageSlot *)m_ItemContainer.GetSlot(eItemTable_Storage);
	pSlot->SetStorageNum(n);
}

BYTE CPlayer::GetStorageNum()
{
	CStorageSlot * pSlot = (CStorageSlot *)m_ItemContainer.GetSlot(eItemTable_Storage);
	return pSlot->GetStorageNum();
}

MONEYTYPE CPlayer::GetMoney(eITEMTABLE tableIdx)
{
	CItemSlot* pSlot = m_ItemContainer.GetSlot(tableIdx);

	return pSlot ? pSlot->GetMoney() : 0;
}

void CPlayer::GetItemtotalInfo(ITEM_TOTALINFO& pRtInfo,DWORD dwFlag)
{
	if(dwFlag & GETITEM_FLAG_INVENTORY)
	{
		m_ItemContainer.GetSlot(eItemTable_Inventory)->GetItemInfoAll(
			pRtInfo.Inventory,
			_countof(pRtInfo.Inventory));
	}

	if(dwFlag & GETITEM_FLAG_WEAR)
	{
		m_ItemContainer.GetSlot(eItemTable_Weared)->GetItemInfoAll(
			pRtInfo.WearedItem,
			_countof(pRtInfo.WearedItem));
	}
}

// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
DWORD CPlayer::SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*& sendMessage)
{
	if(eUSERLEVEL_GM >= GetUserLevel() &&
		FALSE == IsVisible())
	{
		return 0;
	}

	static SEND_CHARACTER_TOTALINFO message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_CHARACTER_ADD;
	message.dwObjectID = dwReceiverID;
	GetSendMoveInfo( &message.MoveInfo,&message.AddableInfo );
	GetBaseObjectInfo( &message.BaseObjectInfo);
	GetCharacterTotalInfo( &message.TotalInfo );
	message.TotalInfo.DateMatching = m_DateMatching;
	message.bLogin = BYTE(isLogin);
	message.mMountedVehicle.mVehicleIndex = GetSummonedVehicle();

	// 090316 LUJ, 탑승 정보 복사
	{
		CVehicle* const vehicle = ( CVehicle* )g_pUserTable->FindUser( GetMountedVehicle() );

		if( vehicle && vehicle->GetObjectKind() == eObjectKind_Vehicle )
		{
			message.mMountedVehicle.mVehicleIndex = vehicle->GetID();
			message.mMountedVehicle.mSeatIndex = vehicle->GetMountedSeat( GetID() );
		}
	}

	// 하우징 탑승 정보 복사
	{
		const stHouseRiderInfo* const houseRiderInfo = HOUSINGMGR->GetRiderInfo(GetID());

		if(houseRiderInfo)
		{
			message.mRiderInfo.dwFurnitureObjectIndex = houseRiderInfo->dwFurnitureObjectIndex;
			message.mRiderInfo.wSlot = houseRiderInfo->wSlot;
		}
	}

	cStreetStall* pStall = STREETSTALLMGR->FindStreetStall(this);

	if( pStall != NULL)
	{
		char StallTitle[MAX_STREETSTALL_TITLELEN+1] = {0};
		GetStreetStallTitle(StallTitle);
		CAddableInfoList::AddableInfoKind kind = CAddableInfoList::None;

		switch(pStall->GetStallKind())
		{
		case eSK_SELL:
			{

				kind = CAddableInfoList::StreetStall;
				break;
			}
		case eSK_BUY:
			{
				kind = CAddableInfoList::StreetBuyStall;
				break;
			}
		}
		
		message.AddableInfo.AddInfo(
			BYTE(kind),
			WORD(strlen(StallTitle)+1),
			StallTitle,
			__FUNCTION__);
	}

	switch(GetBattle()->GetBattleKind())
	{
	case eBATTLE_KIND_SHOWDOWN:
		{
			COMPRESSEDPOS ShowdownPos;
			BATTLE_INFO_SHOWDOWN info;
			WORD wSize = 0;
			GetBattle()->GetBattleInfo( (char*)&info, &wSize );
			ShowdownPos.Compress(&info.vStgPos);
			message.AddableInfo.AddInfo(CAddableInfoList::ShowdownInfo,sizeof(COMPRESSEDPOS),&ShowdownPos, __FUNCTION__ );
			break;
		}
	case eBATTLE_KIND_GTOURNAMENT:
		{
			int nTeam = GetBattle()->GetBattleTeamID(this);
			message.AddableInfo.AddInfo(CAddableInfoList::GTournament, sizeof(nTeam), &nTeam, __FUNCTION__ );
			break;
		}
	}

	sendMessage = &message;
	return message.GetMsgLength();
}

void CPlayer::SetStreetStallTitle(char* title)
{
	SafeStrCpy( m_StreetStallTitle, title, MAX_STREETSTALL_TITLELEN+1 );
}

void CPlayer::GetStreetStallTitle(char* title)
{
	SafeStrCpy( title, m_StreetStallTitle, MAX_STREETSTALL_TITLELEN+1);
}

void CPlayer::CalcState()
{
	//CHARCALCMGR->CalcItemStats(this);
	CHARCALCMGR->CalcCharStats( this );

	SetLife(m_HeroCharacterInfo.Life);
	SetMana(m_HeroInfo.Mana);
}

void CPlayer::MoneyUpdate( MONEYTYPE money )
{	
	m_HeroInfo.Money = money;
}

void CPlayer::SetStrength(DWORD val)
{
	m_HeroInfo.Str = val;

	CHARCALCMGR->CalcCharStats(this);

	// DB￠?￠® ¨u¡A￥i¡IAI¨¡￠c
	CharacterHeroInfoUpdate(this);

	MSG_DWORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_STR_NOTIFY;
	msg.dwObjectID = GetID();
	msg.dwData = val;
	SendMsg(&msg,sizeof(msg));
}

void CPlayer::SetDexterity(DWORD val)
{
	m_HeroInfo.Dex = val;

	CHARCALCMGR->CalcCharStats(this);

	// DB￠?￠® ¨u¡A￥i¡IAI¨¡￠c
	CharacterHeroInfoUpdate(this);

	MSG_DWORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_DEX_NOTIFY;
	msg.dwObjectID = GetID();
	msg.dwData = val;
	SendMsg(&msg,sizeof(msg));
}
void CPlayer::SetVitality(DWORD val)
{
	m_HeroInfo.Vit = val;

	// ¡iy￠￢i¡¤A, ⓒoⓒ¡¨ui¡¤A; ￠￥U¨oA ¡Æe¡ie
	CHARCALCMGR->CalcCharStats(this);

	// DB￠?￠® ¨u¡A￥i¡IAI¨¡￠c
	CharacterHeroInfoUpdate(this);

	MSG_DWORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_VIT_NOTIFY;
	msg.dwObjectID = GetID();
	msg.dwData = val;
	SendMsg(&msg,sizeof(msg));
}
void CPlayer::SetWisdom(DWORD val)
{
	m_HeroInfo.Wis = val;

	// ⓒø¡i¡¤A; ￠￥U¨oA ¡Æe¡ie
	CHARCALCMGR->CalcCharStats(this);

	// DB￠?￠® ¨u¡A￥i¡IAI¨¡￠c
	CharacterHeroInfoUpdate(this);

	MSG_DWORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_WIS_NOTIFY;
	msg.dwObjectID = GetID();
	msg.dwData = val;
	SendMsg(&msg,sizeof(msg));
}

// 070411 LYW --- Player : Add function to setting intelligence.
void CPlayer::SetIntelligence( DWORD val )
{
	m_HeroInfo.Int = val ;

	CHARCALCMGR->CalcCharStats(this);

	CharacterHeroInfoUpdate(this);

	MSG_DWORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_INT_NOTIFY;
	msg.dwObjectID = GetID();
	msg.dwData = val;
	SendMsg(&msg,sizeof(msg));
}

CItemSlot * CPlayer::GetSlot(POSTYPE absPos)
{
	if( TP_INVENTORY_START <= absPos && absPos < TP_SHOPITEM_END )
	{
		return m_ItemContainer.GetSlot(absPos);
	}
	else if( TP_GUILDWAREHOUSE_START <= absPos && absPos < TP_GUILDWAREHOUSE_END )
	{
		return (CItemSlot*)GUILDMGR->GetSlot( GetGuildIdx());
	}
	else if( TP_SHOPITEM_START <= absPos && absPos < TP_SHOPITEM_END )
	{
		return m_ItemContainer.GetSlot(absPos);
	}

	return NULL;
}

CItemSlot * CPlayer::GetSlot(eITEMTABLE tableIdx)
{
	if(tableIdx < eItemTable_TableMax)
		return m_ItemContainer.GetSlot(tableIdx);
	else if( tableIdx == eItemTable_MunpaWarehouse)
	{
		return (CItemSlot*)GUILDMGR->GetSlot(GetGuildIdx());
	}
	return NULL;
}

//-------------------------------------------------------------------------------------------------
//	NAME : SetLifeForce
//	DESC : 080625 LYW 
//		   캐릭터가 죽은 상태에서 강종이나 튕김 현상이 발생 할 때, 
//		   캐릭터의 생명력 50%를 복구해 주어야 한다. 그런데 상태가 죽은 상태면, 
//		   기존 함수는 return 처리를 하므로, 강제로 복구 여부를 세팅할 수 있는 함수를 추가한다.
//-------------------------------------------------------------------------------------------------
void CPlayer::SetLifeForce(DWORD Life, BYTE byForce, BOOL bSendMsg) 
{
	// 강제 세팅 여부를 확인한다.
	if(byForce == FALSE)
	{
		// 캐릭터가 죽은 상태라면, return 처리를 한다.
		if(GetState() == eObjectState_Die) return ;
	}


	// 현재 캐릭터 레벨에 맞는 최대 생명력을 받는다.
	DWORD maxlife = 0 ;
	maxlife = GetMaxLife() ;


	// 생명력 수치 유효 체크.
	if(Life > maxlife) Life = maxlife ;


	// 인자로 넘어온 생명력이 기존 생명력 보다 작으면, return 처리를 한다.
	if( m_HeroCharacterInfo.Life >= Life ) return ;

	
	// 기존 생명력 / 인자로 넘어온 생명력이 같지 않으면,
	if(m_HeroCharacterInfo.Life != Life)
	{
		// 메시지 전송 하는 상황이라면,
		if(bSendMsg == TRUE)
		{
			//// 새로운 생명력을 계산한다.
			//DWORD dwNewLife = 0 ;
			//dwNewLife = Life - GetLife() ;

			// 메시지를 전송한다.
			MSG_INT msg ;
			msg.Category = MP_CHAR ;
			msg.Protocol = MP_CHAR_LIFE_ACK ;
			msg.dwObjectID = GetID() ;
			//msg.nData = dwNewLife ;
			msg.nData = Life ;

			SendMsg(&msg,sizeof(msg)) ;
		}
		
		SendLifeToParty(
			Life);
	}
		

	// 캐릭터의 생명력을 세팅한다.
	m_HeroCharacterInfo.Life = Life ;
}





void CPlayer::SetLife(DWORD val,BOOL bSendMsg) 
{
	if(GetState() == eObjectState_Die)
		return;

	DWORD maxlife = GetMaxLife();
	if(val > maxlife)
		val = maxlife;
	
	if(m_HeroCharacterInfo.Life != val)	// ￥i¡IⓒoIAo AI¡Æⓒ¡￠?i￠?￠®￠￥A ￠?¡þAa￠?￠®¨u¡ⓒ ¡i¨IA¨ª￠￥U.
	{
		if(bSendMsg == TRUE)
		{
			// To A¡þ￠OoAI¨uⓒ￡¨¡￠c -------------------------------
			MSG_INT msg;
			msg.Category = MP_CHAR;
			msg.Protocol = MP_CHAR_LIFE_BROADCAST;
			msg.dwObjectID = GetID();
			// 070419 LYW --- Player : Modified function SetLife.
			msg.nData = val - GetLife();
			PACKEDDATA_OBJ->QuickSend( this, &msg, sizeof( msg ) );
		}
		
		SendLifeToParty(
			val);
	}
		
	m_HeroCharacterInfo.Life = val;
}

void CPlayer::SendLifeToParty(DWORD val)
{	
	if(CParty* pParty = PARTYMGR->GetParty(GetPartyIdx()))
	{
		MSG_DWORD2 message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_PARTY;
		message.Protocol = MP_PARTY_MEMBERLIFE;
		message.dwData1 = GetID();
		message.dwData2 = val * 100 / GetMaxLife();

		pParty->SendMsgExceptOneinChannel(
			&message,
			sizeof(message),
			GetID(),
			GetGridID());
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetLifeForce
//	DESC : 080625 LYW 
//		   캐릭터가 죽은 상태에서 강종이나 튕김 현상이 발생 할 때, 
//		   캐릭터의 마나 30%를 복구해 주어야 한다. 그런데 상태가 죽은 상태면, 
//		   기존 함수는 return 처리를 하므로, 강제로 복구 여부를 세팅할 수 있는 함수를 추가한다.
//-------------------------------------------------------------------------------------------------
void CPlayer::SetManaForce(DWORD Mana, BYTE byForce, BOOL bSendMsg) 
{
	// 강제 세팅 여부를 확인한다.
	if(byForce == FALSE)
	{
		// 캐릭터가 죽은 상태라면, return 처리를 한다.
		if(GetState() == eObjectState_Die) return ;
	}


	// 캐릭터의 현재 레벨상 최대 마나 수치를 받는다.
	DWORD MaxMana = 0 ;
	MaxMana = GetMaxMana() ;


	// 인자로 넘어온 마나의 유효 범위를 체크.
	if(Mana > MaxMana) Mana = MaxMana ;


	// 인자로 넘어온 마나량이 기존 마나량 보다 작으면, return 처리를 한다.
	if( m_HeroInfo.Mana >= Mana ) return ;


	// 기존마나 / 인자로 넘어온 마나가 같지 않으면,
	if( m_HeroInfo.Mana != Mana)
	{
		// 메시지 전송 여부가 TRUE 이면,
		if(bSendMsg)
		{
			// 마나를 전송한다.
			MSG_DWORD msg ;
			msg.Category = MP_CHAR ;
			msg.Protocol = MP_CHAR_MANA_GET_ACK;
			msg.dwObjectID = GetID() ;
			msg.dwData = Mana ;
			SendMsg(&msg,sizeof(msg)) ;
		}
		
		SendManaToParty(
			Mana);
	}
	

	// 캐릭터의 마나를 세팅한다.
	m_HeroInfo.Mana = Mana ; 
}





void CPlayer::SetMana(DWORD val,BOOL bSendMsg)
{ 
	if(GetState() == eObjectState_Die)
		return;

	DWORD MaxMana = GetMaxMana();
	if(val > MaxMana)
		val = MaxMana;

	if( m_HeroInfo.Mana != val)
	{
		// 100223 ShinJS --- 마나 데미지 추가로 인한 마나 정보를 변화량으로 수정.
		if(bSendMsg)
		{
			MSG_INT msg;
			msg.Category = MP_CHAR;
			msg.Protocol = MP_CHAR_MANA_ACK;
			msg.dwObjectID = GetID();
			msg.nData = val - GetMana();
			SendMsg(&msg,sizeof(msg));
		}
		
		SendManaToParty(
			val);
	}
	
	m_HeroInfo.Mana = val; 
}

void CPlayer::SendManaToParty(DWORD mana)
{
	if(CParty* pParty = PARTYMGR->GetParty(GetPartyIdx()))
	{
		MSG_DWORD2 message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_PARTY;
		message.Protocol = MP_PARTY_MEMBERMANA;
		message.dwData1 = GetID();
		message.dwData2 = mana * 100 / GetMaxMana();

		pParty->SendMsgExceptOneinChannel(
			&message,
			sizeof(message),
			GetID(),
			GetGridID());
	}
}

void CPlayer::SetMaxLife(DWORD val)
{
	m_HeroCharacterInfo.MaxLife = val;

	// To A¡þ￠OoAI¨uⓒ￡¨¡￠c -------------------------------
	MSG_DWORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_MAXLIFE_NOTIFY;
	msg.dwObjectID = GetID();
	msg.dwData = val;
	PACKEDDATA_OBJ->QuickSend( this, &msg, sizeof( msg ) );
}

void CPlayer::SetMaxMana(DWORD val)
{
	m_HeroInfo.MaxMana= val;

	// To A¡þ￠OoAI¨uⓒ￡¨¡￠c -------------------------------
	MSG_DWORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_MAXMANA_NOTIFY;
	msg.dwObjectID = GetID();
	msg.dwData = val;
	SendMsg(&msg,sizeof(msg));
}
void CPlayer::AddStrength(int val)
{
	SetStrength(m_HeroInfo.Str+val);
}
void CPlayer::AddWisdom(int val)
{
	SetWisdom(m_HeroInfo.Wis+val);
}
void CPlayer::AddDexterity(int val)
{
	SetDexterity(m_HeroInfo.Dex+val);
}
void CPlayer::AddVitality(int val)
{
	SetVitality(m_HeroInfo.Vit+val);
}
// 070411 LYW --- Player : Add function to setting intelligence.
void CPlayer::AddIntelligence( int val )
{
	SetIntelligence( m_HeroInfo.Int+ val ) ;
}
void CPlayer::SetPlayerLevelUpPoint(LEVELTYPE val) 
{ 
	m_HeroInfo.LevelUpPoint=val;
	
	MSG_DWORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_LEVELUPPOINT_NOTIFY;
	msg.dwObjectID = GetID();
	msg.dwData = val;
	SendMsg(&msg,sizeof(msg));
}
/*****************************************************************/
/* 1. AE¡¾a￠?￠® Aⓒø￠￢?AI¡Æ￠® ¡iy¨u¨￢￥iC¨uu;￠O¡× from DBResultQuery
/* 2. SetPlayerExpPoint()￠?￠®¨u¡ⓒ AO￠￥e ¡Æⓒ¡CeA￠®￠￢| ⓒøN¨ui ¨uⓒo; ￠O¡× call￥iE
/*
/*
/*****************************************************************/

void CPlayer::SetLevel(LEVELTYPE level)
{
	if(level >= MAX_PLAYERLEVEL_NUM)
	{
		ASSERT(0);
		return;
	}

	if(m_HeroCharacterInfo.Level == level) return;

	LEVELTYPE AddPoint = 0;
	if(level > m_HeroCharacterInfo.Level)
	{
		if(m_HeroInfo.MaxLevel < level)
		{
			m_HeroInfo.MaxLevel = level;
			AddPoint += 5;

			DWORD skillpoint = (DWORD)( ceil( level / 10.f ) + 19 );

			if(m_HeroCharacterInfo.Race == RaceType_Devil)
			{
				skillpoint = (DWORD)( ceil( level / 10.f ) + 21 );
			}

			SetSkillPoint( skillpoint, FALSE );

			// 웹이벤트
			if( level == 10 )
			{
				WebEvent( GetUserID(), 1 );
			}
		}
	}

	SetLife(
		GetMaxLife());
	SetMana(
		GetMaxMana());

	m_HeroCharacterInfo.Level = level;
	CHARCALCMGR->CalcCharStats( this );

	MSG_LEVEL message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_CHAR;
	message.Protocol = MP_CHAR_LEVEL_NOTIFY;
	message.dwObjectID = GetID();
	message.Level = GetLevel();
	message.MaxExpPoint = GAMERESRCMNGR->GetMaxExpPoint(level);
	message.CurExpPoint = GetPlayerExpPoint();
	PACKEDDATA_OBJ->QuickSend(
		this,
		&message,
		sizeof(message));

	SetPlayerLevelUpPoint(GetPlayerLevelUpPoint() + AddPoint);

	CQuestEvent QEvent(
		eQuestEvent_Level,
		level,
		1);
	QUESTMGR->AddQuestEvent(
		this,
		&QEvent);

	PARTYMGR->MemberLevelUp(
		GetPartyIdx(),
		GetID(),
		GetLevel());
	GUILDMGR->MemberLevelUp(
		GetGuildIdx(),
		GetID(),
		GetLevel());
	g_csFamilyManager.SRV_UpdateMapPlayerInfo(
		this);
}

// 080611 LYW --- Player : 스킬포인트 업데이트 처리를 수정함.
// (아이템으로 스킬 포인트를 추가하는 기능이 생겼기 때문.)
//void CPlayer::SetSkillPoint( DWORD point )
void CPlayer::SetSkillPoint( DWORD point, BYTE byForced )
{
	// 강제 업데이트 여부 체크.
	ASSERT(byForced <= TRUE) ;
	if(byForced > TRUE) return ;


	// 스킬 포인트 업데이트.
	GetHeroTotalInfo()->SkillPoint += point;


	// 클라이언트 전송
	MSG_DWORD msg;

	msg.Category = MP_SKILLTREE;
	msg.Protocol = MP_SKILLTREE_POINT_NOTIFY;
	msg.dwObjectID = GetID();
	msg.dwData = point;

	SendMsg(&msg, sizeof(msg));


	// DB 업데이트
	SkillPointUpdate( GetID(), GetSkillPoint() );


	// 071129 LYW --- Player : 누적 스킬 포인트 업데이트.
	//DB_UpdateAccumulateSkillPoint(GetID(), FALSE, point) ;
	DB_UpdateAccumulateSkillPoint(GetID(), byForced, point) ;


	// 071114 웅주. 로그
	{
		const SKILL_BASE emptyData = { 0 };

		InsertLogSkill( this, &emptyData, eLog_SkillGetPoint );
	}
}

DWORD CPlayer::GetSkillPoint()
{
	return GetHeroTotalInfo()->SkillPoint;
}

void CPlayer::SetPlayerExpPoint(EXPTYPE point)
{
	// 071119 웅주, 한번에 여러 단계를 레벨업할 수 있도록 수정하고 코드를 단순화함

	const LEVELTYPE& level = m_HeroCharacterInfo.Level;
	
	ASSERT( level <= MAX_CHARACTER_LEVEL_NUM );

	if( level == MAX_CHARACTER_LEVEL_NUM )
	{
		const EXPTYPE& BeforeExp = m_HeroInfo.ExpPoint;
		// Max레벨일때 경험치 하락이 되지 않던 버그 수정
		// 경험치 상승일때만 리턴시킨다.
		if( point > BeforeExp )
		{
			return;
		}
	}	
	
	// 경험치가 다음 단계에서 요구하는 것보다 훨씬 많을 수 있으므로,
	// 계속 체크해서 레벨업하자
	{
		EXPTYPE nextPoint = GAMERESRCMNGR->GetMaxExpPoint( level );

		while( point >= nextPoint )
		{
			SetLevel( level + 1 );

			CharacterHeroInfoUpdate( this );
			CharacterTotalInfoUpdate( this );

			InsertLogCharacter( GetID(), m_HeroCharacterInfo.Level, &m_HeroInfo );
			InsertLogExp( eExpLog_LevelUp, GetID(), level, 0, GetPlayerExpPoint(), 0, 0, 0 );

			point		-=	nextPoint;
			nextPoint	=	GAMERESRCMNGR->GetMaxExpPoint( level );
		}

		m_HeroInfo.ExpPoint = point;
	}

	{
		MSG_EXPPOINT message;
		message.Category	= MP_CHAR;
		message.Protocol	= MP_CHAR_EXPPOINT_ACK;
		message.ExpPoint	= point;

		SendMsg( &message, sizeof( message ) );
	}
}
void CPlayer::AddPlayerExpPoint(EXPTYPE Exp)
{
	if( Exp == 0 ) return;

	EXPTYPE NewExp = 0 ;

	// 090213 LYW --- Player : 패밀리 멤버 접속에 따른 추가 경험치를 적용한다. ( 서버 적용 경험치 설정 )
	if( GetFamilyIdx() )
	{
		NewExp = GetPlayerExpPoint() + Exp + m_dweFamilyRewardExp ;
	}
	else
	{
		NewExp = GetPlayerExpPoint() + Exp ;
	}

	SetPlayerExpPoint(NewExp);

	MSG_DWORDEX2 msg ;

	msg.Category	= MP_USERCONN ;
	msg.Protocol	= MP_USERCONN_CHARACTER_APPLYEXP_NOTICE ;
	msg.dwObjectID	= GetID() ;
	msg.dweData1	= GetPlayerExpPoint() ;

	// 090213 LYW --- Player : 패밀리 멤버 접속에 따른 추가 경험치를 적용한다. ( 전송용 경험치 설정 )
	if( GetFamilyIdx() )
	{
		msg.dweData2	= Exp + m_dweFamilyRewardExp ;
	}
	else
	{
		msg.dweData2	= Exp ;
	}

	SendMsg(&msg, sizeof(msg)) ;
}

void CPlayer::ReduceExpPoint(EXPTYPE minusExp, BYTE bLogType)
{
	LEVELTYPE minuslevel = 0;

	// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
	//DWORD CurExp = GetPlayerExpPoint();
	EXPTYPE CurExp = GetPlayerExpPoint();

	if(GetLevel() <= 1 && CurExp < minusExp)	//·¹º§1Aº 0±iAo¸¸ ±iAI´U.
		minusExp = CurExp;

	InsertLogExp( bLogType, GetID(), GetLevel(), minusExp, GetPlayerExpPoint(), m_MurdererKind, m_MurdererIDX, 0/*어빌리티 삭제 - GetPlayerAbilityExpPoint()*/);
	
	while(1)
	{
		if(CurExp < minusExp)
		{
			minusExp -= CurExp;
			++minuslevel;
			CurExp = GAMERESRCMNGR->GetMaxExpPoint(GetLevel()-minuslevel) - 1;
			ASSERT(minuslevel<2);	//E¤½A³ª CØ¼­
			if(minuslevel > 3)		//E¤½A³ª CI´A... ¹≪CN·cCA ¹æAo¿e
				break;
		}
		else
		{
			CurExp -= minusExp;
			break;
		}
	}

	if(minuslevel > 0)
	{
		SetLevel(m_HeroCharacterInfo.Level-minuslevel);

		// character info log
		InsertLogCharacter( GetID(), m_HeroCharacterInfo.Level, &m_HeroInfo );
	}

	SetPlayerExpPoint(CurExp);
}

BYTE CPlayer::GetLifePercent()
{
	return BYTE(GetLife() / (float)GetMaxLife() * 100);
}

BYTE CPlayer::GetManaPercent()
{
	return BYTE(GetMana() / (float)GetMaxMana() * 100);
}

void CPlayer::OnEndObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Die:
		GetBattle()->OnTeamMemberRevive(GetBattleTeam(),this);
		m_bNeedRevive = TRUE;
		break;
	}

}

// 090204 LUJ, 타입을 명확히 함
eWeaponType CPlayer::GetWeaponEquipType()
{
	const ITEM_INFO* const pItemInfo = ITEMMGR->GetItemInfo( GetWearedWeapon() );

	return pItemInfo ? eWeaponType( pItemInfo->WeaponType ) : eWeaponType_None;
}

// 080703 LUJ, 반환 타입을 enum으로 변경
eWeaponAnimationType CPlayer::GetWeaponAniType()
{
	const ITEM_INFO* leftInfo	= ITEMMGR->GetItemInfo( GetWearedItemIdx( eWearedItem_Weapon ) );
	const ITEM_INFO* rightInfo	= ITEMMGR->GetItemInfo( GetWearedItemIdx( eWearedItem_Shield ) );

	const eWeaponAnimationType	leftType	= eWeaponAnimationType( leftInfo ? leftInfo->WeaponAnimation : eWeaponAnimationType_None );
	const eWeaponAnimationType	rightType	= eWeaponAnimationType( rightInfo ? rightInfo->WeaponAnimation : eWeaponAnimationType_None );

	// 080703 LUJ, 양손의 무기가 다르면 이도류가 아니다. 왼손에 무기를 안 들었을 경우도 마찬가지이다.
	if( leftType != rightType ||
		leftType == eWeaponAnimationType_None )
	{
		return leftType;
	}

	return eWeaponAnimationType_TwoBlade;
}

void CPlayer::ReviveAfterShowdown( BOOL bSendMsg )
{
	ClearMurderIdx();
	m_bNeedRevive = TRUE;

	if( bSendMsg )
	{
		MOVE_POS msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CHARACTER_REVIVE;
		msg.dwObjectID = GetID();
		msg.dwMoverID = GetID();
	
		msg.cpos.Compress(CCharMove::GetPosition(this));
		
		PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));
	}

	OBJECTSTATEMGR_OBJ->EndObjectState(this,eObjectState_Die);

	m_YYLifeRecoverTime.bStart = FALSE;
	m_YYManaRecoverTime.bStart = FALSE;
	SetLife( GetMaxLife() * 30 / 100 );	//¨uoA￠® ¨ui￠O¡i¡ÆO CO¡¾i.
}
	
// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
//DWORD CPlayer::RevivePenalty(BOOL bAdditionPenalty)								// 제자리 부활시 추가 경험치 하락을 처리하는 함수.
EXPTYPE CPlayer::RevivePenalty(BOOL bAdditionPenalty)								// 제자리 부활시 추가 경험치 하락을 처리하는 함수.
{
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.30
	// ..챌린지 존에서 죽어서 맵아웃일 때는 페널티 없음
	if (g_csDateManager.IsChallengeZoneHere())
	{
		return FALSE;
	}
	// E 데이트 존 추가 added by hseos 2007.11.30

	// 길트 토너먼트시 죽을경우 패널티 없음.
	if( g_pServerSystem->GetMapNum() == GTMAPNUM)
	{
		return FALSE;
	}

	DWORD PenaltyNum = 0 ;														// 추가 경험치 하락을 2%로 고정한다.

	if( bAdditionPenalty )
	{
		PenaltyNum = 1 ;
	}
	else
	{
		PenaltyNum = random(1, 3) ;
	}
	
	// 071217 KTH --- Status에 ProtectExp의 효과가 존재하면 경험치를 감소 시키지 않는다.//
	Status* pStatus;
	pStatus = this->GetBuffStatus();

	if( pStatus->IsProtectExp )
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////////////////

	LEVELTYPE CurLevel = GetLevel() ;											// 플레이어의 현재 레벨을 구한다.

	EXPTYPE CurExp	= GetPlayerExpPoint() ;										// 플레이어의 현재 경험치를 구한다.
	
	EXPTYPE GoalExp	= GAMERESRCMNGR->GetMaxExpPoint(CurLevel) ;					// 플레이어의 레벨업 경험치를 구한다. 

	//---KES CHECK : GoalExp는 매우 큰 수이다. * PeanltyNum을 했을 경우 DWORD를 넘어갈 수 있다.
	// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
	//DWORD dwExpA = GoalExp * PenaltyNum ;										// 패널티 수치를 구한다.
	EXPTYPE dwExpA = GoalExp * PenaltyNum ;										// 패널티 수치를 구한다.
	
	EXPTYPE PenaltyExp = (EXPTYPE)(dwExpA / 100) ;								// 패널티 경험치를 구한다.

	// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
	//DWORD dwExp = 0 ;															// 전송할 경험치 변수를 선언하고 0으로 세팅한다.
	EXPTYPE dwExp = 0 ;															// 전송할 경험치 변수를 선언하고 0으로 세팅한다.

	BOOL bLevelDown = FALSE ;													// 레벨 다운여부 변수를 선언하고 FALSE 세팅을 한다.

	if( CurExp >= PenaltyExp )													// 현재 경험치가 패널티 경험치 이상일 경우.
	{
		dwExp = CurExp - PenaltyExp ;											// 전송할 경험치를 세팅한다.

		ASSERT( dwExp >= 0 ) ;													// 경험치는 0이상이어야만 한다.

		SetPlayerExpPoint( dwExp ) ;

		MSG_DWORDEX3 msg ;

		msg.Category	= MP_USERCONN ;											// 카테고리를 MP_USERCONN로 세팅한다.
		msg.Protocol	= MP_USERCONN_CHARACTER_DOWNEXP_NOTICE ;				// 프로토콜을 경험치 하락으로 세팅한다.
		msg.dwObjectID	= GetID() ;												// 플레이어의 아이디를 세팅한다.
		msg.dweData1	= (DWORD)PenaltyNum ;									// 패널티 수치를 세팅한다.
		msg.dweData2	= dwExp ;												// 전송할 경험치를 세팅한다.

		if( bAdditionPenalty )
		{
			msg.dweData3		= TRUE ;												// 추가 경험치 하락 여부를 TRUE로 세팅한다.
		}
		else
		{
			msg.dweData3		= FALSE ;												// 추가 경험치 하락 여부를 FALSE로 세팅한다.
		}

		SendMsg(&msg, sizeof(msg)) ;											// 플레이어에게 메시지를 전송한다.
	}
	else																		// 현재 플레이어의 경험치가 패널티 경험치보다 작을경우.
	{
		bLevelDown = TRUE ;														// 레벨 다운 여부를 TRUE로 세팅한다.

		dwExp = PenaltyExp - CurExp ;											// 전송할 경험치를 세팅한다.

		ASSERT( dwExp >= 0 ) ;													// 경험치는 0이상이어야만 한다.

		GoalExp = GAMERESRCMNGR->GetMaxExpPoint(CurLevel-1) ;					// 한단계 낮은 레벨의 레벨업 목표 경험치를 구한다.
		SetLevel( CurLevel -1 ) ;												// 플레이어의 레벨을 다운한 레벨로 세팅한다.
		SetPlayerExpPoint(GoalExp-dwExp) ;										// 플레이어의 경험치를 세팅한다.

		MSG_DWORDEX4 msg ;

		msg.Category	= MP_USERCONN ;											// 카테고리를 MP_USERCONN로 세팅한다.
		msg.Protocol	= MP_USERCONN_CHARACTER_DOWNLEVEL_NOTICE ;				// 프로토콜을 경험치 하락으로 세팅한다.
		msg.dwObjectID	= GetID() ;												// 플레이어의 아이디를 세팅한다.
		msg.dweData1	= (DWORDEX)GetLevel() ;									// 플레이어의 레벨을 세팅한다.
		msg.dweData2	= GoalExp-dwExp ;										// 플레이어의 경험치를 세팅한다.
		msg.dweData3	= (DWORDEX)PenaltyNum ;									// 패널티 수치를 세팅한다.


		if( bAdditionPenalty )
		{
			msg.dweData4		= TRUE ;												// 추가 경험치 하락 여부를 TRUE로 세팅한다.
		}
		else
		{
			msg.dweData4		= FALSE ;												// 추가 경험치 하락 여부를 FALSE로 세팅한다.
		}

		SendMsg(&msg, sizeof(msg)) ;											// 플레이어에게 메시지를 전송한다.
	}

	// 080414 LUJ, 경험치 손실 때 손실된 경험치 만큼 로그를 남긴다
	InsertLogExp(
		eExpLog_LosebyRevivePresent,
		GetID(),
		GetLevel(),
		PenaltyExp,
		dwExp,
		0,
		0,
		0 );

	return PenaltyExp;
}


void CPlayer::RevivePresentSpot()
{	
	if(GetState() != eObjectState_Die)
	{
		ASSERT(0);
		MSG_BYTE msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE_NACK;
		msg.bData		= 1; //errorcode
		SendMsg( &msg, sizeof(msg) );

		return;
	}

	if( LOOTINGMGR->IsLootedPlayer( GetID() ) )	//¡¤c¨¡AAⓒ￢￠?¡I
	{
		MSG_BYTE msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE_NACK;
		msg.bData		= 2; //errorcode
		SendMsg( &msg, sizeof(msg) );

		return;
	}

	if( IsExitStart() )
	{
		MSG_BYTE msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE_NACK;
		msg.bData		= 4; //errorcode
		SendMsg( &msg, sizeof(msg) );

		return;
	}
	// 100111 LUJ, 부활 플래그에 따라 부활 불가능할 수도 있다
	else if(ReviveFlagTown == mReviveFlag)
	{
		MSG_BYTE message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_CHARACTER_REVIVE_NACK;
		message.bData = 3;
		SendMsg(
			&message,
			sizeof(message));
		return;
	}

	m_bNeedRevive = TRUE;
	MOVE_POS msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHARACTER_REVIVE;
	msg.dwObjectID = GetID();
	msg.dwMoverID = GetID();
	
	msg.cpos.Compress(CCharMove::GetPosition(this));
		
	PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));

	LEVELTYPE curLevel = GetLevel() ;

	if( curLevel >= 10 && m_bNoExpPenaltyByPK == FALSE )
	{
		RevivePenalty(FALSE) ;
		RevivePenalty(TRUE) ;
	
		if( !g_csDateManager.IsChallengeZoneHere() )
		{
			// 090204 LUJ, 감소 회수를 지정함
			RemoveBuffCount( eBuffSkillCountType_Dead, 1 );
		}
	}

	OBJECTSTATEMGR_OBJ->EndObjectState(this,eObjectState_Die);
	
	DWORD MaxLife = GetMaxLife();
	DWORD MaxMana = GetMaxMana();

	int nReviveVal = (int)(MaxLife*0.3) ;

	MSG_INT ReviveLife;
	ReviveLife.Category = MP_CHAR;
	ReviveLife.Protocol = MP_CHAR_LIFE_ACK;
	ReviveLife.dwObjectID = GetID();
	ReviveLife.nData = max(1, nReviveVal);
	SendMsg(&ReviveLife,sizeof(ReviveLife));			
		
	SendLifeToParty(
		nReviveVal);
		
	m_HeroCharacterInfo.Life = nReviveVal;

	// 070417 LYW --- Player : Modified setting mana when the character revived.
	DWORD dwManaRate = (DWORD)(MaxMana*0.3) ;
	if( GetMana() < dwManaRate )
	{
		SetMana(dwManaRate);
	}
	
	m_YYLifeRecoverTime.bStart = FALSE;
	m_YYManaRecoverTime.bStart = FALSE;
	ClearMurderIdx();

	m_bDieForGFW = FALSE;
}

// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
//DWORD CPlayer::ReviveBySkill()
void CPlayer::ReviveBySkill( cSkillObject* pSkillObject )
{	
	if( !pSkillObject )
		return;

	if(GetState() != eObjectState_Die)
	{
		ASSERT(0);
		MSG_BYTE msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE_NACK;
		msg.bData		= 1; //errorcode
		SendMsg( &msg, sizeof(msg) );

		return;
	}

	if( LOOTINGMGR->IsLootedPlayer( GetID() ) )	//¡¤c¨¡AAⓒ￢￠?¡I
	{
		MSG_BYTE msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE_NACK;
		msg.bData		= 2; //errorcode
		SendMsg( &msg, sizeof(msg) );

		return;
	}

	if( IsExitStart() )
	{
		MSG_BYTE msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE_NACK;
		msg.bData		= 4; //errorcode
		SendMsg( &msg, sizeof(msg) );

		return;
	}

	// 100211 ONS 부활대상에게 부활여부를 묻는다.
	// 부활스킬을 설정한다.
	SetCurResurrectIndex( pSkillObject->GetSkillIdx() );
	
	// 스킬Operator이름을 전송한다.
	CObject* pOperator = pSkillObject->GetOperator();
	if( !pOperator || 
		pOperator->GetObjectKind() != eObjectKind_Player )
	{
		return;
	}

	MSG_NAME msg;
	ZeroMemory(&msg, sizeof(MSG_NAME));
	msg.Category	= MP_SKILL;
	msg.Protocol	= MP_SKILL_RESURRECT_SYN;
	msg.dwObjectID	= GetID();
	SafeStrCpy(msg.Name, pOperator->GetObjectName(), MAX_NAME_LENGTH+1);
	SendMsg( &msg, sizeof(msg) );
}

// 100211 ONS 부활대상자가 수락한 경우 부활처리를 실행한다.
EXPTYPE CPlayer::OnResurrect()
{
	EXPTYPE exp = 0;
	m_bNeedRevive = TRUE;

	MOVE_POS msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHARACTER_REVIVE;
	msg.dwObjectID = GetID();
	msg.dwMoverID = GetID();
	
	msg.cpos.Compress(CCharMove::GetPosition(this));
		
	PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));

	LEVELTYPE curLevel = GetLevel() ;

	if( curLevel >= 10 && m_bNoExpPenaltyByPK == FALSE )
	{
		exp = RevivePenalty(FALSE) ;

		if( !g_csDateManager.IsChallengeZoneHere() && g_pServerSystem->GetMapNum()!=GTMAPNUM)
		{
			// 090204 LUJ, 감소 회수를 지정함
			RemoveBuffCount( eBuffSkillCountType_Dead, 1 );
		}
	}

	OBJECTSTATEMGR_OBJ->EndObjectState(this,eObjectState_Die);
	
	DWORD MaxLife = GetMaxLife();
	DWORD MaxMana = GetMaxMana();

	int nReviveVal = (int)(MaxLife*0.3) ;

	MSG_INT ReviveLife;
	ReviveLife.Category = MP_CHAR;
	ReviveLife.Protocol = MP_CHAR_LIFE_ACK;
	ReviveLife.dwObjectID = GetID();
	ReviveLife.nData = max(1, nReviveVal);
	SendMsg(&ReviveLife,sizeof(ReviveLife));			
		
	SendLifeToParty(
		nReviveVal);
		
	m_HeroCharacterInfo.Life = nReviveVal;

	// 070417 LYW --- Player : Modified setting mana when the character revived.
	DWORD dwManaRate = (DWORD)(MaxMana*0.3) ;
	if( GetMana() < dwManaRate )
	{
		SetMana(dwManaRate);
	}
	
	m_YYLifeRecoverTime.bStart = FALSE;
	m_YYManaRecoverTime.bStart = FALSE;
	ClearMurderIdx();

	m_bDieForGFW = FALSE;
	return exp;
}

void CPlayer::ReviveLogIn()
{	
	// 캐릭터가 죽은 상태가 아니면, 부활 실패 처리를 한다.
	if(GetState() != eObjectState_Die)
	{
		ASSERT(0) ;
		MSG_BYTE msg ;
		msg.Category	= MP_USERCONN ;
		msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE_NACK ;
		msg.bData		= 1 ; //errorcode
		SendMsg( &msg, sizeof(msg) ) ;

		return ;
	}
	

	// 루팅 상태라면, 부활 실패처리를 한다.
	if( LOOTINGMGR->IsLootedPlayer( GetID() ) )
	{
		MSG_BYTE msg ;
		msg.Category	= MP_USERCONN ;
		msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE_NACK ;
		msg.bData		= 2 ; //errorcode
		SendMsg( &msg, sizeof(msg) ) ;

		return ;
	}


	// 아웃처리가 시작 되었으면, 실패 처리를 한다.
	if( IsExitStart() )
	{
		MSG_BYTE msg ;
		msg.Category	= MP_USERCONN ;
		msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE_NACK ;
		msg.bData		= 4 ; //errorcode
		SendMsg( &msg, sizeof(msg) ) ;

		return ;
	}
	// 100111 LUJ, 부활 플래그에 따라 부활 불가능할 수도 있다
	else if(ReviveFlagHere == mReviveFlag)
	{
		MSG_BYTE message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_CHARACTER_REVIVE_NACK;
		message.bData = 3;
		SendMsg(
			&message,
			sizeof(message));
		return;
	}

	// 공성 길드 던전인지 확인한다.
	if( SIEGEDUNGEONMGR->IsSiegeDungeon(g_pServerSystem->GetMapNum()) )
	{
		ReviveLogIn_GuildDungeon() ;
	}
	else
	{
		ReviveLogIn_Normal() ;
	}
}





// 081210 LYW --- Player : 공성전 길드 던전에서의 부활 문제로 인해 두가지 함수를 추가한다.
//-------------------------------------------------------------------------------------------------
//	NAME		: ReviveLogIn_Normal
//	DESC		: 일반적인 안전지대 부활 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 10, 2008
//-------------------------------------------------------------------------------------------------
void CPlayer::ReviveLogIn_Normal()
{
	// 부활창이 필요하다고 세팅한다.
	m_bNeedRevive = TRUE ;


	// 부활 메시지를 세팅한다.
	MOVE_POS msg ;

	msg.Category	= MP_USERCONN ;
	msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE ;

	msg.dwObjectID	= GetID() ;
	msg.dwMoverID	= GetID() ;


	// 부활 위치를 세팅한다.
	VECTOR3* ppos ;
	VECTOR3 pos ;

	if(g_pServerSystem->GetMapNum() == GTMAPNUM)
	{
		const DWORD dwTeam = GetBattle()->GetBattleTeamID( this );
		ppos = GetBattle()->GetRevivePoint(dwTeam);
	}
	else
	{
		ppos = GAMERESRCMNGR->GetRevivePoint() ;
	}

	int temp ;
	temp	= rand() % 500 - 250 ;
	pos.x	= ppos->x + temp ;
	temp	= rand() % 500 - 250 ;
	pos.z	= ppos->z + temp ;
	pos.y	= 0 ;

	msg.cpos.Compress(&pos) ;
	
	CCharMove::SetPosition(this,&pos) ;

	PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg)) ;
		

	// Player의 죽음 상태를 해제한다.
	OBJECTSTATEMGR_OBJ->EndObjectState(this,eObjectState_Die) ;


	// 부활 패널티를 적용한다.
	const LEVELTYPE curLevel = GetLevel() ;
	
	if(	curLevel >= 10 && !m_bDieForGFW && m_bNoExpPenaltyByPK == FALSE )
	{
		RevivePenalty(FALSE) ;
		
		if( !g_csDateManager.IsChallengeZoneHere() && g_pServerSystem->GetMapNum()!=GTMAPNUM )
		{
			// 090204 LUJ, 감소 회수를 지정함
            RemoveBuffCount( eBuffSkillCountType_Dead, 1 );
		}
	}


	// 길드 토너먼트 예외 처리를 한다.
	m_bDieForGFW = FALSE ;
	m_dwRespawnTimeOnGTMAP = 0 ;
	m_dwImmortalTimeOnGTMAP = 0 ;
	

	// 상황에 따른 생명력을 세팅한다.
	DWORD CurLife = GetMaxLife() ;

	int nReviveVal = 0 ;
	if(g_pServerSystem->GetMapNum() == GTMAPNUM)
		nReviveVal = (int)(CurLife*1.0) ;
	else
		nReviveVal = (int)(CurLife*0.3) ;

	MSG_INT ReviveLife ;
	ReviveLife.Category = MP_CHAR ;
	ReviveLife.Protocol = MP_CHAR_LIFE_ACK ;
	ReviveLife.dwObjectID = GetID() ;
	ReviveLife.nData = max(1, nReviveVal);
	SendMsg(&ReviveLife,sizeof(ReviveLife)) ;

	m_HeroCharacterInfo.Life = nReviveVal ;


	// 상황에 따른 마나력을 세팅한다.
	DWORD MaxMana = GetMaxMana() ;

	DWORD dwManaRate = 0 ;
	if(g_pServerSystem->GetMapNum() == GTMAPNUM)
		SetMana(MaxMana) ;
	else
	{
		dwManaRate = (DWORD)(MaxMana*0.3) ;
		if( GetMana() < dwManaRate )
		{
			SetMana(dwManaRate) ;
		}
	}


	// 길드 토너먼트 예외를 처리한다.
	if(g_pServerSystem->GetMapNum() == GTMAPNUM)
	{
		WORD wCode = GetJobCodeForGT() ;
		m_dwImmortalTimeOnGTMAP = GTMGR->GetImmortalTimeByClass(wCode) ;
	}


	// 무적상태 처리를 한다.
	OBJECTSTATEMGR_OBJ->StartObjectState(this,eObjectState_Immortal,0) ;
	// 06.08.29. RaMa.
	OBJECTSTATEMGR_OBJ->EndObjectState( this, eObjectState_Immortal, 30000 ) ;
	
	m_YYLifeRecoverTime.bStart = FALSE ;
	m_YYManaRecoverTime.bStart = FALSE ;

	if(CParty* const pParty = PARTYMGR->GetParty(m_HeroInfo.PartyID))
	{
		SEND_PARTYICON_REVIVE msg;
		ZeroMemory(
			&msg,
			sizeof(msg));
		msg.Category = MP_PARTY;
		msg.Protocol = MP_PARTY_REVIVEPOS;
		msg.dwMoverID = m_BaseObjectInfo.dwObjectID;
		msg.Pos.posX = (WORD)pos.x;
		msg.Pos.posZ = (WORD)pos.z;

		pParty->SendMsgToAll(
			&msg,
			sizeof(msg));
		SendLifeToParty(
			nReviveVal);
	}

	ClearMurderIdx();
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ReviveLogIn_GuildDungeon
//	DESC		: 공성 길드던전에서의 안전지대 부활 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 10, 2008
//-------------------------------------------------------------------------------------------------
void CPlayer::ReviveLogIn_GuildDungeon()
{
	BYTE byCheckRevivePoint = TRUE ;

	// 부활창이 필요하다고 세팅한다.
	m_bNeedRevive = TRUE ;


	// 부활 메시지를 세팅한다.
	MOVE_POS msg ;

	msg.Category	= MP_USERCONN ;
	msg.Protocol	= MP_USERCONN_CHARACTER_REVIVE ;

	msg.dwObjectID	= GetID() ;
	msg.dwMoverID	= GetID() ;

	
	// Player가 루쉔성 길드 소속인지 제뷘성 길드 소속인지 확인한다.
	VillageWarp* pRevivePoint	= NULL ;

	DWORD dwGuildID				= GetGuildIdx() ;
	if( dwGuildID == 0 )
	{
		char szMsg[512] = {0, } ;
		sprintf( szMsg, "This player is not in guild! - %s,%d", GetObjectName(), GetID() ) ;
		SIEGEWARFAREMGR->WriteLog(szMsg) ;

		// 081217 LYW --- Player : 공성 길드 던전에서 길드 소속 없이 사망시, 캐릭터 선택 화면으로 이동하는 처리 추가.
		MSGBASE msg ;

		msg.Category	= MP_SIEGEWARFARE ;
		msg.Protocol	= MP_SIEGEWARFARE_DIEINGUILDDUNGEON_NOTICE ;

		msg.dwObjectID	= GetID() ;

		SendMsg( &msg, sizeof(MSGBASE) ) ;

		byCheckRevivePoint = FALSE ;
	}

	DWORD dwGuildID_Rushen		= SIEGEWARFAREMGR->GetCastleGuildIdx(eNeraCastle_Lusen) ;
	DWORD dwGuildID_Zevyn		= SIEGEWARFAREMGR->GetCastleGuildIdx(eNeraCastle_Zebin) ;

	// 루쉔성 길드라면,
	if( dwGuildID_Rushen == dwGuildID )
	{
		pRevivePoint = SIEGEWARFAREMGR->GetDGRP_Rushen() ;
	}
	// 제뷘성 길드라면,
	else
	{
		if( dwGuildID_Zevyn == dwGuildID )
		{
			pRevivePoint = SIEGEWARFAREMGR->GetDGRP_Zevyn() ;
		}
		else
		{
			char szMsg[512] = {0, } ;
			sprintf( szMsg, "Invalid guild idx! \n PLAYER_GUILD:%d / RUSHEN_GUILD:%d / ZEVYN_GUILD:%d", 
			dwGuildID, dwGuildID_Rushen, dwGuildID_Zevyn ) ;
			SIEGEWARFAREMGR->WriteLog(szMsg) ;

			// 081217 LYW --- Player : 공성 길드 던전에서 길드 소속 없이 사망시, 캐릭터 선택 화면으로 이동하는 처리 추가.
			MSGBASE msg ;

			msg.Category	= MP_SIEGEWARFARE ;
			msg.Protocol	= MP_SIEGEWARFARE_DIEINGUILDDUNGEON_NOTICE ;

			msg.dwObjectID	= GetID() ;

			SendMsg( &msg, sizeof(MSGBASE) ) ;

			byCheckRevivePoint = FALSE ;
		}
	}


	// 부활 위치를 세팅한다.
	VECTOR3 pos ;
	int temp ;

	if( byCheckRevivePoint )
	{
		if( !pRevivePoint )
		{
			char szMsg[512] = {0, } ;
			sprintf( szMsg, "Failed to receive dungeon revive point! \n PLAYER_GUILD:%d / RUSHEN_GUILD:%d / ZEVYN_GUILD:%d", 
				dwGuildID, dwGuildID_Rushen, dwGuildID_Zevyn ) ;
			SIEGEWARFAREMGR->WriteLog(szMsg) ;
			return ;
		}

		temp	= rand() % 100 ;
		pos.x	= (pRevivePoint->PosX*100) + temp ;
		temp	= rand() % 100 ;
		pos.z	= (pRevivePoint->PosZ*100) + temp ;
		pos.y	= 0 ;
	}
	else
	{
		pos = *CCharMove::GetPosition(this) ;
	}

	msg.cpos.Compress(&pos) ;
	
	CCharMove::SetPosition(this,&pos) ;

	PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg)) ;
		

	// Player의 죽음 상태를 해제한다.
	OBJECTSTATEMGR_OBJ->EndObjectState(this,eObjectState_Die) ;


	// 부활 패널티를 적용한다.
	const LEVELTYPE curLevel = GetLevel() ;
	
	if(	curLevel >= 10 && !m_bDieForGFW && m_bNoExpPenaltyByPK == FALSE )
	{
		RevivePenalty(FALSE) ;
		
		if( !g_csDateManager.IsChallengeZoneHere() && g_pServerSystem->GetMapNum()!=GTMAPNUM )
		{
			// 090204 LUJ, 감소 회수를 지정함
            RemoveBuffCount( eBuffSkillCountType_Dead, 1 );
		}
	}


	// 상황에 따른 생명력을 세팅한다.
	DWORD CurLife = GetMaxLife() ;
	int nReviveVal = (int)(CurLife*0.3) ;

	MSG_INT ReviveLife ;
	ReviveLife.Category = MP_CHAR ;
	ReviveLife.Protocol = MP_CHAR_LIFE_ACK ;
	ReviveLife.dwObjectID = GetID() ;
	ReviveLife.nData = max(1, nReviveVal);
	SendMsg(&ReviveLife,sizeof(ReviveLife)) ;

	m_HeroCharacterInfo.Life = nReviveVal ;


	// 상황에 따른 마나력을 세팅한다.
	DWORD MaxMana = GetMaxMana() ;

	DWORD dwManaRate = (DWORD)(MaxMana*0.3) ;
	if( GetMana() < dwManaRate )
	{
		SetMana(dwManaRate) ;
	}


	// 무적상태 처리를 한다.
	OBJECTSTATEMGR_OBJ->StartObjectState(this,eObjectState_Immortal,0) ;
	OBJECTSTATEMGR_OBJ->EndObjectState( this, eObjectState_Immortal, 30000 ) ;
	
	m_YYLifeRecoverTime.bStart = FALSE ;
	m_YYManaRecoverTime.bStart = FALSE ;

	if(CParty* const pParty = PARTYMGR->GetParty(m_HeroInfo.PartyID))
	{
		SEND_PARTYICON_REVIVE msg;
		ZeroMemory(
			&msg,
			sizeof(msg));
		msg.Category = MP_PARTY;
		msg.Protocol = MP_PARTY_REVIVEPOS;
		msg.dwMoverID = m_BaseObjectInfo.dwObjectID;
		msg.Pos.posX = (WORD)pos.x;
		msg.Pos.posZ = (WORD)pos.z;

		pParty->SendMsgToAll(
			&msg,
			sizeof(msg));
		SendLifeToParty(
			nReviveVal);
	}

	ClearMurderIdx();
}





void CPlayer::ReviveLogInPenelty()
{
	if( GetLevel() >= 10 && m_bNoExpPenaltyByPK == FALSE )
	{
		RevivePenalty(FALSE) ;
		RevivePenalty(TRUE) ;
		
		if( !g_csDateManager.IsChallengeZoneHere() )
		{
			// 090204 LUJ, 감소 회수를 지정?			RemoveBuffCount( eBuffSkillCountType_Dead, 1 );
		}
	}

	DWORD CurLife = GetMaxLife();
	DWORD CurMana = GetMaxMana();

	// 080625 LYW --- Player : 생명력 세팅을 하라고 하나, 캐릭터가 죽은 상태이기 때문에, 
	// 생명력 세팅 함수가 기능을 제대로 하지 않는다. 때문에, 강제로 생명력을 세팅하는 
	// 함수로 변경한다.
	//SetLife((DWORD)(CurLife*0.3));
	//SetMana(0);

	// 생명력 세팅.
	DWORD dwNewLife = 0 ;
	// 080710 LYW --- Player : 생명력 회복 50%로 수정.
	//dwNewLife = (DWORD)(CurLife * 0.3f) ;
	dwNewLife = (DWORD)(CurLife * 0.5f) ;

	SetLifeForce(dwNewLife, TRUE) ;

	// 마나 세팅.
	DWORD dwNewMana = 0 ;
	// 080710 LYW --- Player : 마나 회복 50%로 수정.
	//dwNewMana = (DWORD)(CurMana* 0.3f) ;
	dwNewMana = (DWORD)(CurMana* 0.5f) ;

	SetManaForce(dwNewMana, TRUE) ;

	m_bDieForGFW = FALSE;
}


void CPlayer::DoDie(CObject* pAttacker)
{
	OBJECTSTATEMGR_OBJ->StartObjectState(this,eObjectState_Die,pAttacker->GetID());

	m_bNoExpPenaltyByPK = FALSE;

	if( pAttacker->GetObjectKind() == eObjectKind_Pet )
	{
		CObject* const object = g_pUserTable->FindUser(
			pAttacker->GetOwnerIndex());

		if(0 == object)
		{
			return;
		}

		pAttacker = object;
	}

	if(pAttacker->GetObjectKind() == eObjectKind_Player)
	{
		CPlayer* pAttackPlayer = (CPlayer*)pAttacker;
		CBattle* pBattle = pAttacker->GetBattle();
		if(pBattle->GetBattleKind() == eBATTLE_KIND_NONE || pAttacker->GetBattleID() != GetBattleID() )
		{
			// for pk
			if( LOOTINGMGR->IsLootingSituation( this, pAttackPlayer ) )
			{
				LOOTINGMGR->CreateLootingRoom( this, (CPlayer*)pAttacker );
			}

			if(pAttackPlayer->IsPKMode() == TRUE)
			{
				m_MurdererIDX = pAttacker->GetID();
				m_MurdererKind = WORD(pAttacker->GetObjectKind());
				m_bNoExpPenaltyByPK = TRUE;
			}
			else if( GUILDWARMGR->JudgeGuildWar( this, (CPlayer*)pAttacker ) &&
				! m_bDieForGFW )
			{
				SetDieForGFW( TRUE );

				const CGuildManager::ScoreSetting& setting = GUILDMGR->GetScoreSetting();

				GuildUpdateScore( pAttackPlayer, this, setting.mKillerScore, setting.mCorpseScore );
			}

			SetReadyToRevive(TRUE);

		}
	}
	else if(pAttacker->GetObjectKind() & eObjectKind_Monster )
	{
		m_MurdererKind = ((CMonster*)pAttacker)->GetMonsterKind();
			
		// 080616 LUJ, 함정일 경우에는 페널티를 주지 않도록 한다
		if( pAttacker->GetObjectKind() != eObjectKind_Trap )
		{
			//SW060831 방파 비무 시 //강제 종료시 체크 변수
			SetPenaltyByDie(TRUE);
		}

		SetReadyToRevive(TRUE);

		// add quest event
		CQuestEvent QEvent( eQuestEvent_Die, g_pServerSystem->GetMapNum(), 0 );
		QUESTMGR->AddQuestEvent( this, &QEvent, 0 );
	}

	//KES EXIT
	ExitCancel();
	//KES EXCHANGE 031002
	EXCHANGEMGR->CancelExchange( this );
	STREETSTALLMGR->UserLogOut( this );
	//KES PKLOOTING
	LOOTINGMGR->LootingCancel( this );
	//KES PKPlayerPanelty
	PKMGR->DiePanelty( this, pAttacker );

//---KES Aggro 070918
//---어그로 리스트 릴리스
	RemoveAllAggroed();	//*주의: 아래 FollowMonsterList해제하기 이전에 해주어야, 다음 어그로를 타겟으로 잡을 수 있다.
//-------------------

	CMonster * pObject = NULL;
	m_FollowMonsterList.SetPositionHead();
	while((pObject = (CMonster *)m_FollowMonsterList.GetData())!= NULL)
	{
		pObject->SetTObject(NULL);
	}
	m_FollowMonsterList.RemoveAll();

	//---KES 죽으면 이동을 멈춰주어야 한다.
	if( CCharMove::IsMoving(this) )
	{
		VECTOR3 pos;
		GetPosition( &pos );
		CCharMove::EndMove( this, gCurTime, &pos );
	}

	QUESTMAPMGR->DiePlayer( this );

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.29
	g_csDateManager.SRV_EndChallengeZone(this, CSHDateManager::CHALLENGEZONE_END_ALL_DIE);
	// E 데이트 존 추가 added by hseos 2007.11.29

	// 080725 KTH
	SIEGEWARFAREMGR->CancelWaterSeedUsing(this);

	{
		CPet* const petObject = PETMGR->GetPet(
			GetPetItemDbIndex());

		if(petObject)
		{
			petObject->DoDie(
				pAttacker);
		}
	}

	// 081020 LYW --- Player : 공성중 캐릭터 사망시, A가 B를 죽였다는 맵 공지 브로드캐스팅 처리 추가. - 송가람.
	if( SIEGEWARFAREMGR->IsSiegeWarfareZone(g_pServerSystem->GetMapNum()) )
	{
		MSG_DWORD2 msg ;

		msg.Category	= MP_SIEGEWARFARE ;
		msg.Protocol	= MP_SIEGEWARFARE_NOTICE_KILLANDKILLER_SYN ;

		msg.dwObjectID	= 0 ;

		msg.dwData1		= pAttacker->GetID() ;
		msg.dwData2		= this->GetID() ;

		g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_DWORD2)) ;
	}

	SetSummonedVehicle( 0 );
	SetMountedVehicle( 0 );

	// 100621 ShinJS 사망시 현재 시전중인 스킬을 취소시킨다.
	CancelCurrentCastingSkill( FALSE );
}

float CPlayer::DoGetMoveSpeed()
{
	if( GetAbnormalStatus()->IsMoveStop )
	{
		return 0;
	}

	// 090422 ShinJS --- 탈것의 Master이고 탑승중인 경우 탈것의 이동속도 이용
	{
		CObject* const vehicleObject = g_pUserTable->FindUser( GetSummonedVehicle() );

		if( vehicleObject &&
			GetSummonedVehicle() == GetMountedVehicle() )
		{
			return vehicleObject->GetMoveSpeed();
		}
	}

	float speed = float( m_MoveInfo.MoveMode == eMoveMode_Run ? RUNSPEED : WALKSPEED );

	// 080630 LUJ, 세트 아이템 수치가 적용되도록 함
	float addrateval = ( GetRateBuffStatus()->MoveSpeed + GetRatePassiveStatus()->MoveSpeed ) / 100.f + m_itemBaseStats.mMoveSpeed.mPercent + m_itemOptionStats.mMoveSpeed.mPercent + m_SetItemStats.mMoveSpeed.mPercent;
	float addval = GetBuffStatus()->MoveSpeed + GetPassiveStatus()->MoveSpeed + m_itemBaseStats.mMoveSpeed.mPlus + m_itemOptionStats.mMoveSpeed.mPlus + m_SetItemStats.mMoveSpeed.mPlus;

	m_MoveInfo.AddedMoveSpeed = speed * addrateval + addval;

	speed += m_MoveInfo.AddedMoveSpeed;
	return max( 0, speed );
}


void CPlayer::SetInitedGrid()
{
	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_GRIDINIT;
	SendMsg(&msg,sizeof(msg));

	CGridUnit::SetInitedGrid();
	
	CBattle* pBattle = BATTLESYSTEM->GetBattle(this->GetBattleID());
	if(pBattle && pBattle->GetBattleKind() != eBATTLE_KIND_NONE)
		BATTLESYSTEM->AddObjectToBattle(pBattle, this);

	{
		CQuestEvent QEvent( eQuestEvent_GameEnter, 0, 1 );
		QUESTMGR->AddQuestEvent( this, &QEvent );
	}

	{
		CQuestEvent QEvent( eQuestEvent_MapChange, 0, g_pServerSystem->GetMapNum() );
		QUESTMGR->AddQuestEvent( this, &QEvent );
	}

	// 090316 LUJ, 탈것에 탑승한 채로 맵 이동한 경우 자동으로 태우기 위해 정보를 가져온다
	LoadVehicleFromDb( GetID(), g_pServerSystem->GetMapNum() );
}
// RaMa - 04.11.10    -> ShopItemOption 추가   AvatarOption추가(05.02.16)
DWORD CPlayer::DoGetCritical()
{	
	return (DWORD)mCriticalRate;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 06. 07 내공 적중(일격) - 이영준
DWORD CPlayer::DoGetDecisive()
{	
	return (DWORD)mCriticalRate;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CPlayer::DoGetPhyAttackPowerMin()
{
	return (DWORD)mPhysicAttackMin;
}

DWORD CPlayer::DoGetPhyAttackPowerMax()
{
	return (DWORD)mPhysicAttackMax;
}

void CPlayer::DoDamage(CObject* pAttacker,RESULTINFO* pDamageInfo,DWORD beforeLife)
{
	SetObjectBattleState( eObjectBattleState_Battle );
	if ( pAttacker )
	{
		pAttacker->SetObjectBattleState( eObjectBattleState_Battle );
		AddToAggroed(
			pAttacker->GetID());
	}

	if( GetAbnormalStatus()->IsSlip )
	{
		EndBuffSkillByStatus( eStatusKind_Slip );
	}

	// 090109 LUJ, 피격 시 캐스팅 중인 스킬이 취소될 수 있다
	CancelCurrentCastingSkill( TRUE );
}

void CPlayer::DoManaDamage( CObject* pAttacker, RESULTINFO* pDamageInfo, DWORD beforeMana )
{
	SetObjectBattleState( eObjectBattleState_Battle );
	if ( pAttacker )
	{
		pAttacker->SetObjectBattleState( eObjectBattleState_Battle );
	}

	if( GetAbnormalStatus()->IsSlip )
	{
		EndBuffSkillByStatus( eStatusKind_Slip );
	}

	// 090109 LUJ, 피격 시 캐스팅 중인 스킬이 취소될 수 있다
	CancelCurrentCastingSkill( TRUE );
}

void CPlayer::InitBaseObjectInfo(BASEOBJECT_INFO* pBaseInfo)
{
	ASSERT(GetInitState() == PLAYERINITSTATE_ONLY_ADDED);
	memcpy(&m_BaseObjectInfo,pBaseInfo,sizeof(BASEOBJECT_INFO));
}

/* ¨oC¨￢?E¡ⓒ¡¤￠c Return */
MONEYTYPE CPlayer::SetMoney( MONEYTYPE ChangeValue, BYTE bOper, BYTE MsgFlag, eITEMTABLE tableIdx, BYTE LogType, DWORD TargetIdx )
{
	CPurse* pPurse = m_ItemContainer.GetPurse(tableIdx);
	if( !pPurse) return 0;

	MONEYTYPE Real = 0;
	if( bOper == MONEY_SUBTRACTION )
	{
		Real = pPurse->Subtraction( ChangeValue, MsgFlag );
	}
	else
	{
		Real = pPurse->Addition( ChangeValue, MsgFlag );
	}

	if(tableIdx == eItemTable_Inventory)
	{
		if( Real >= 10000 )
			InsertLogWorngMoney( LogType, GetID(), Real, GetMoney(), GetMoney(eItemTable_Storage), TargetIdx );
	}

	return Real;
}

/* ￥i¡¤; ⓒøO; Aⓒ¡¨￢¨￠CN ¡Æⓒª¡Æ¡IAI AO￠￥AAo￠￢| ⓒo?￠￥A￠￥U.*/
BOOL CPlayer::IsEnoughAdditionMoney(MONEYTYPE money, eITEMTABLE tableIdx )
{
	CPurse* pPurse = m_ItemContainer.GetPurse(tableIdx);
	if(!pPurse) return FALSE;

	return pPurse->IsAdditionEnough( money );
}


MONEYTYPE CPlayer::GetMaxPurseMoney(eITEMTABLE TableIdx)
{
	CPurse* pPurse = m_ItemContainer.GetPurse(TableIdx);
	if( !pPurse) return FALSE;
	return pPurse->GetMaxMoney();
}

void CPlayer::SetMaxPurseMoney(eITEMTABLE TableIdx, DWORD MaxMoney)
{
	//C¡I¡¾ⓒo; ¡ic￠￢e ￥i¡¤ ¨￢￠￢¡Æu ￠￢¨¡¨o¨￢A￠®¡Æ￠® ￠￥A¨uiⓒø¡ⓒ￠￥U.
	if(TableIdx != eItemTable_Storage)
	{
		ASSERT(0);
		return;
	}
	CPurse* pPurse = m_ItemContainer.GetPurse(TableIdx);
	if( !pPurse) return;
	pPurse->SetMaxMoney(MaxMoney);
}

void CPlayer::AddQuest(CQuestBase* pQuest)
{
	if(CQuestBase* questBase = m_QuestList.GetData(pQuest->GetQuestIdx()))
	{
		m_QuestList.Remove(
			pQuest->GetQuestIdx());
		SAFE_DELETE(
			questBase);
	}
	m_QuestList.Add( pQuest, pQuest->GetQuestIdx() );
}

BOOL CPlayer::SetQuestState(DWORD QuestIdx, QSTATETYPE value)
{
	CQuestBase* pQuest;
	pQuest = m_QuestList.GetData(QuestIdx);

	if( !pQuest ) 
	{
//		char buff[256] = {0,};
//		sprintf(buff, "￠?a¡¾a￠￥A Au¨o¨￢¨¡￠c¡Æ￠® xAc ¨uECN￠￥Uⓒøⓒ￢ CI￠?¨I¡Æ¡I ⓒoy¨u￠c￠?￠®¡ÆO ¨uE¡¤AAa￠?¡Æ [QUEST ID : %d]", QuestIdx);
//		ASSERTMSG(0, buff);
		return FALSE;
	}

	pQuest->SetValue(value);
	BOOL bEnd = pQuest->IsComplete();

	// DB￠?￠® ¨uA¨¡ACN￠￥U.
	QuestUpdateToDB( GetID(), QuestIdx, value, (BYTE)bEnd );

	if( bEnd )
	{
		m_QuestList.Remove(QuestIdx);

		MSG_DWORD msg;
		msg.Category = MP_QUEST;
		msg.Protocol = MP_QUEST_REMOVE_NOTIFY;
		msg.dwObjectID = GetID();
		msg.dwData = QuestIdx;
		SendMsg(&msg, sizeof(msg));

		SAFE_DELETE(pQuest);
	}

	return TRUE;
}

void CPlayer::SetInitState(int initstate,DWORD protocol)
{
	m_initState |= initstate;


	// 091106 LUJ, 복잡한 if문 처리를 간결화
	if(FALSE == (m_initState & PLAYERINITSTATE_ONLY_ADDED))
	{
		return;
	}
	else if(FALSE == (m_initState & PLAYERINITSTATE_SKILL_INFO))
	{
		return;
	}
	else if(FALSE == (m_initState & PLAYERINITSTATE_ITEM_INFO))
	{
		return;
	}
	else if(FALSE == (m_initState & PLAYERINITSTATE_QUICK_INFO))
	{
		return;
	}
	else if(FALSE == (m_initState & PLAYERINITSTATE_HERO_INFO))
	{
		return;
	}
	else if(FALSE == (m_initState & PLAYERINITSTATE_QEUST_INFO))
	{
		return;
	}
	else if(FALSE == (m_initState & PLAYERINITSTATE_FARM_INFO))
	{
		return;
	}
	else if(FALSE == (m_initState & PLAYERINITSTATE_CHALLENGEZONE_INFO))
	{
		return;
	}
	else if(FALSE == (m_initState & PLAYERINITSTATE_STORAGE_INFO))
	{
		CharacterStorageInfo(GetUserID(), GetID());
		return;
	}
	else if(FALSE == (m_initState & PLAYERINITSTATE_OPTION_INFO))
	{
		return;
	}

	SetPeaceMode(g_pServerSystem->GetMap()->IsVillage());

	for(POSTYPE part = TP_WEAR_START ; part < TP_WEAR_END ; ++part )
	{
		const ITEMBASE * pTargetItemBase = ITEMMGR->GetItemInfoAbsIn(this, part);

		if( pTargetItemBase && 
			pTargetItemBase->dwDBIdx )
		{
			m_HeroCharacterInfo.WearedItemIdx[part-TP_WEAR_START] = pTargetItemBase->wIconIdx;
		}
		else
		{
			m_HeroCharacterInfo.WearedItemIdx[part-TP_WEAR_START] = 0;
		}
	}

	const ITEMBASE * pWeaponItemBase = ITEMMGR->GetItemInfoAbsIn(this, TP_WEAR_START + eWearedItem_Weapon );

	if( pWeaponItemBase && pWeaponItemBase->dwDBIdx )
	{
		const ITEM_OPTION& option = ITEMMGR->GetOption( *pWeaponItemBase );

		if( option.mItemDbIndex )
		{
			m_HeroCharacterInfo.WeaponEnchant = option.mEnchant.mLevel;
		}
	}

	m_dwProgressTime = gCurTime;

	// LUJ, 클라이언트에 전송하기 전에 계산해야한다
	CHARCALCMGR->Initialize(
		this);

	SEND_HERO_TOTALINFO msg;
	memset( &msg, 0, sizeof(msg) );
	GetBaseObjectInfo(&msg.BaseObjectInfo);
	GetCharacterTotalInfo(&msg.ChrTotalInfo);
	GetHeroTotalInfo(&msg.HeroTotalInfo);
	GetItemtotalInfo(
		msg.ItemTotalInfo,
		GETITEM_FLAG_INVENTORY | GETITEM_FLAG_WEAR);
	GetSendMoveInfo(&msg.SendMoveInfo,NULL);


	msg.ChrTotalInfo.CurMapNum = GAMERESRCMNGR->GetLoadMapNum();
	msg.UniqueIDinAgent = GetUniqueIDinAgent();

	SKILL_BASE SkillTreeInfo[MAX_SKILL_TREE] = {0};	
	m_SkillTree->SetPositionHead();

	for(SKILL_BASE* skill = m_SkillTree->GetData();
		0 < skill;
		skill = m_SkillTree->GetData())
	{
		if(msg.SkillNum > sizeof(SkillTreeInfo) / sizeof(*SkillTreeInfo))
		{
			break;
		}

		SkillTreeInfo[msg.SkillNum] = *skill;
		++msg.SkillNum;
	}

	msg.AddableInfo.AddInfo(
		CAddableInfoList::SkillTree,
		sizeof(*SkillTreeInfo) * msg.SkillNum,
		SkillTreeInfo,
		__FUNCTION__);
	msg.ChrTotalInfo.DateMatching = m_DateMatching;

	srand( GetTickCount());
	GetLocalTime(&msg.ServerTime);

	// 080827 LYW --- Player : 공성 상태를 유저에게(Client)로 전송한다.
	msg.Category	= MP_USERCONN;
	msg.Protocol	= MP_USERCONN_GAMEIN_ACK;

	// 071227 LUJ
	msg.StorageSize	= GetStorageNum();
	SendMsg( &msg, msg.GetMsgLength() );

	MSG_DWORDEX2 msgFishingExp;
	msgFishingExp.Category = MP_FISHING;
	msgFishingExp.Protocol = MP_FISHING_EXP_ACK;
	msgFishingExp.dweData1 = (DWORDEX)m_wFishingLevel;
	msgFishingExp.dweData2 = m_dwFishingExp;
	SendMsg( &msgFishingExp, sizeof(msgFishingExp) );

	// 080424 NYJ --- 물고기포인트
	MSG_DWORD msgFishPoint;
	msgFishPoint.Category = MP_FISHING;
	msgFishPoint.Protocol = MP_FISHING_POINT_ACK;
	msgFishPoint.dwData   = m_dwFishPoint;
	SendMsg( &msgFishPoint, sizeof(msgFishPoint) );

	// 요리숙련도
	MSG_DWORD4 msgCookState;
	msgCookState.Category = MP_COOK;
	msgCookState.Protocol = MP_COOK_STATE;
	msgCookState.dwData1 = GetCookLevel();
	msgCookState.dwData2 = GetCookCount();
	msgCookState.dwData3 = GetEatCount();
	msgCookState.dwData4 = GetFireCount();
	SendMsg( &msgCookState, sizeof(msgCookState) );

	// 요리달인레시피
	int i;
	for(i=0; i<MAX_RECIPE_LV4_LIST; i++)
	{
		MSG_DWORD4	msgRecipe;
		msgRecipe.Category = MP_COOK;
		msgRecipe.Protocol = MP_COOK_UPDATERECIPE;
		msgRecipe.dwData1 = eCOOKRECIPE_ADD;
		msgRecipe.dwData2 = m_MasterRecipe[i].dwRecipeIdx;
		msgRecipe.dwData3 = i;
		msgRecipe.dwData4 = m_MasterRecipe[i].dwRemainTime;
		SendMsg( &msgRecipe, sizeof(msgRecipe) );
	}

	VECTOR3 pos;
	msg.SendMoveInfo.CurPos.Decompress(&pos);

	MSG_WORD3 msgMapDesc;
	msgMapDesc.Category		= MP_USERCONN;
	msgMapDesc.Protocol		= MP_USERCONN_MAPDESC;
	msgMapDesc.wData1		= (WORD)g_pServerSystem->GetMap()->IsVillage();
	// 090824 ONS GM툴에서 PK제어시 설정된 PK허용값.
	msgMapDesc.wData2		= (WORD)PKMGR->IsPKAllow();
	msgMapDesc.wData3		= (WORD)GetCurChannel();
	SendMsg( &msgMapDesc, sizeof(msgMapDesc) );

	const MAPTYPE MapNum = GAMERESRCMNGR->GetLoadMapNum();

	RegistLoginMapInfo(
		GetID(),
		GetObjectName(),
		BYTE(GTMAPNUM == MapNum ? GTRETURNMAPNUM : MapNum),
		g_pServerTable->GetSelfServer()->wPortForServer);
	SetInited();
	if(PARTYMGR->CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
		PARTYMGR->ProcessReservationList(GetID());

	PARTYMGR->UserLogIn(
		this,
		MP_USERCONN_GAMEIN_SYN == protocol);
	QUICKMNGR->SendQuickInfo(
		this);

	// S 몬스터미터 추가 added by hseos 2007.05.29
	{
		MSG_DWORD2 msg;
		msg.Category	= MP_CHAR;
		msg.Protocol	= MP_CHAR_MONSTERMETER_PLAYTIME;
		msg.dwData1		= m_stMonstermeterInfo.nPlayTime;
		msg.dwData2		= m_stMonstermeterInfo.nPlayTimeTotal;
		SendMsg(&msg, sizeof(msg));

		msg.Category	= MP_CHAR;
		msg.Protocol	= MP_CHAR_MONSTERMETER_KILLMONSTER;
		msg.dwData1		= m_stMonstermeterInfo.nKillMonsterNum;
		msg.dwData2		= m_stMonstermeterInfo.nKillMonsterNumTotal;
		SendMsg(&msg, sizeof(msg));
		// 트리거를 읽어들인다
		TRIGGERMGR->LoadTrigger(*this);
	}

	g_csFarmManager.SRV_SendPlayerFarmInfo(this);
	g_csDateManager.SRV_SendChallengeZoneEnterFreq(this);
	// 091106 LUJ, 리미트 던젼에 등록시킨다
	LIMITDUNGEONMGR->AddPlayer(*this);
	GUILDMGR->AddPlayer( this );
	GUILDWARMGR->AddPlayer( this );
	PassiveSkillCheckForWeareItem();
	PetInfoLoad(
		GetID(),
		GetUserID());
	CharacterBuffLoad(
		GetID());

	CCharMove::InitMove(
		this,
		&m_MoveInfo.CurPosition);

	if(CGridTable* const gridTable = g_pServerSystem->GetGridSystem()->GetGridTable(this))
	{
		gridTable->AddObject(
			this,
			CCharMove::GetPosition(this));
	}

	// 100408 ShinJS --- 서버 시간 전송
	stTime64t serverTimeMsg;
	ZeroMemory( &serverTimeMsg, sizeof(serverTimeMsg) );
	serverTimeMsg.Category = MP_USERCONN;
	serverTimeMsg.Protocol = MP_USERCONN_GETSERVERTIME_ACK;
	serverTimeMsg.dwObjectID = GetID();
	_time64( &serverTimeMsg.time64t );

	SendMsg( &serverTimeMsg, sizeof(serverTimeMsg) );

	if(const DWORD gameRoomIndex = PCROOMMGR->GetGameRoomIndex(GetID()))
	{
		g_DB.FreeMiddleQuery(
			RLoadPCRoomPoint,
			GetID(),
			"EXEC dbo.MP_GAMEROOM_LOAD_POINT %d, %d, %d",
			GetID(),
			gameRoomIndex,
			g_pServerSystem->GetMapNum());
	}

	// 100525 NYJ - 판매대행 등록상품에 대해 시간경과체크 수행
	Consignment_CheckDate(GetID());
	Note_CheckDate(GetID());

	// 100611 ONS 로그인시 채팅금지 정보를 로드한다.
	ForbidChatLoad(GetID());
}

int CPlayer::CanExitStart()	//~¡¤av¡ÆC
{
//	if( GetState() != eObjectState_None && GetState() != eObjectState_Move )
//		return FALSE;
	if( IsPKMode() )
		return eEXITCODE_PKMODE;
	if( LOOTINGMGR->IsLootedPlayer(GetID()) )	//PK¡¤c¨¡A; ￠￥cCI￠￥A Aⓒ￢AI¡Æ￠®?
		return eEXITCODE_LOOTING;

	if( GetState() == eObjectState_Exchange )	//±³E?Aß¿￡ A¾·aCO ¼o ¾ø´U.
		return eEXITCODE_NOT_ALLOW_STATE;

	if( GetState() == eObjectState_StreetStall_Owner ||
		GetState() == eObjectState_StreetStall_Guest )	//³eA¡Aß¿￡ A¾·aCO ¼o ¾ø´U.
		return eEXITCODE_NOT_ALLOW_STATE;

	if( GetState() == eObjectState_Deal )	//≫oA¡ AI¿eAß¿￡ A¾·aCO ¼o ¾ø´U.
		return eEXITCODE_NOT_ALLOW_STATE;
	
	return eEXITCODE_OK;
}

void CPlayer::SetExitStart( BOOL bExit )
{
	m_bExit				= bExit;
	m_dwExitStartTime	= gCurTime;
}

int CPlayer::CanExit()
{
	DWORD lCurTime = MHTIMEMGR_OBJ->GetNewCalcCurTime();
	if( lCurTime - m_dwExitStartTime < EXIT_COUNT*1000 - 2000 )	//8.0	//버퍼를 충분히 잡자
		return eEXITCODE_SPEEDHACK;

	if( IsPKMode() )							//PK￠￢ⓒ￡￥iaAI¡Æ￠®?
		return eEXITCODE_PKMODE;
	if( LOOTINGMGR->IsLootedPlayer(GetID()) )	//PK¡¤c¨¡A; ￠￥cCI￠￥A Aⓒ￢AI¡Æ￠®?
		return eEXITCODE_LOOTING;

//---KES AUTONOTE
	if( GetAutoNoteIdx() )
		return eEXITCODE_NOT_ALLOW_STATE;
//---------------
	
	return eEXITCODE_OK;
}

void CPlayer::ExitCancel()
{
	if( IsExitStart() )
	{
		SetExitStart( FALSE );
		MSG_BYTE msgNack;
		msgNack.Category	= MP_CHAR;
		msgNack.Protocol	= MP_CHAR_EXIT_NACK;
		msgNack.bData		= eEXITCODE_DIEONEXIT;
		SendMsg(&msgNack, sizeof(msgNack));		
	}
}

int CPlayer::PKModeOn()
{
	if( IsPKMode() ) return ePKCODE_ALREADYPKMODEON;		//AIⓒoI PK￠￢ⓒ￡￥ia
	if( IsShowdown() ) return ePKCODE_SHOWDOWN;		//¨￢nⓒo¡iAⓒ￢AI￠￢e ¨uE￥iE￠￥U

	//pk¸ðμa A°¸e ¹≪Au≫oAA CØA|
	if( GetState() == eObjectState_Immortal )
		OBJECTSTATEMGR_OBJ->EndObjectState( this, eObjectState_Immortal );

	if( GetState() == eObjectState_Die )
		return ePKCODE_STATECONFLICT;	//￠￥U￠￢¡I¡ioAAAI￠O¡×￠￥A ¨uE￥iE￠￥U.
	
	m_HeroCharacterInfo.bPKMode = TRUE;
	m_dwPKModeStartTime			= gCurTime;

//---KES PK 071124
	m_dwPKContinueTime			= 20*60*1000 + ( GetBadFame() / 75 ) * 5*60*1000;	//기본 30분 + 악명치 75마다 5분
//----------------

	return ePKCODE_OK;
}

BOOL CPlayer::PKModeOff()
{
	if( !IsPKMode() ) return FALSE;

	if( gCurTime - m_dwPKModeStartTime >= m_dwPKContinueTime || ( GetUserLevel() <= eUSERLEVEL_GM && PKMGR->IsPKEvent() ) )
	{
		m_HeroCharacterInfo.bPKMode = FALSE;
		SetPKModeEndtime();
		PKCharacterUpdate( GetID(), m_HeroInfo.LastPKModeEndTime );
		return TRUE;
	}

	return FALSE;
}

void CPlayer::PKModeOffForce()
{
	m_HeroCharacterInfo.bPKMode = FALSE;
}

void CPlayer::StateProcess()
{
	switch( GetState() )		
	{
	case eObjectState_None:
		{
			if( m_BaseObjectInfo.ObjectBattleState )
			{
				if( !mpBattleStateDelay->Check() )
				{
					SetObjectBattleState( eObjectBattleState_Peace );
				}
			}

			//in Korea : per 10min
			//in China : per 30min
			DWORD dwInsertLogTime = INSERTLOG_TIME;

			if( gCurTime - m_dwProgressTime >= dwInsertLogTime )
			{
				{
					CPet* const petObject = (CPet*)g_pUserTable->FindUser(
						GetPetItemDbIndex());

					if(petObject &&
						eObjectKind_Pet == petObject->GetObjectKind())
					{
						LogPet(
							petObject->GetObjectInfo(),
							ePetLogRegular);
					}
				}

				InsertLogExp(
					eExpLog_Time,
					GetID(),
					GetLevel(),
					0,
					GetPlayerExpPoint(),
					0,
					0,
					0);
				InsertLogMoney(
					0,
					GetID(),
					GetMoney(),
					0,
					0,
					0);

				if( IsPKMode())
				{
					m_HeroInfo.LastPKModeEndTime = 0;
				}
				
				UpdateCharacterInfoByTime(
					GetID(),
					GetPlayerExpPoint(),
					GetMoney(),
					m_HeroInfo.Playtime,
					m_HeroInfo.LastPKModeEndTime);
				MonsterMeter_Save(
					GetID(),
					m_stMonstermeterInfo.nPlayTime,
					m_stMonstermeterInfo.nKillMonsterNum,
					m_stMonstermeterInfo.nPlayTimeTotal,
					m_stMonstermeterInfo.nKillMonsterNumTotal);
				FishingData_Update(
					GetID(),
					GetFishingLevel(),
					GetFishingExp(),
					GetFishPoint());

				m_dwProgressTime = gCurTime;
			}

			m_ContinuePlayTime += gTickTime;
			DWORD dwPT = m_ContinuePlayTime/1000;
			if(dwPT)
			{
				m_HeroInfo.Playtime += dwPT;
				m_ContinuePlayTime -= dwPT*1000;
			}
		}
		break;
	case eObjectState_Die:
		{
			//---KES PK 071202	죽은 경우 시간을 계속 리셋 (시간이 안가도록)
			SetPKStartTimeReset();
			
			if(FALSE == m_bNeedRevive )
			{
				break;
			}
			else if(g_pServerSystem->GetMapNum() == GTMAPNUM)
			{
				if(gTickTime < m_dwRespawnTimeOnGTMAP)
					m_dwRespawnTimeOnGTMAP -= gTickTime;
				else
				{
					MSGBASE message;
					message.Category	= MP_USERCONN;
					message.Protocol	= MP_USERCONN_READY_TO_REVIVE_BY_GFW;
					SendMsg( &message, sizeof( message ) );
					m_bNeedRevive = FALSE;
				}

				break;
			}

			DWORD dwElapsed = gCurTime - m_ObjectState.State_Start_Time;

			if( dwElapsed > PLAYERREVIVE_TIME )
			{
				if(TRUE == m_bDieForGFW)
				{
					MSGBASE message;
					message.Category	= MP_USERCONN;
					message.Protocol	= MP_USERCONN_READY_TO_REVIVE_BY_GFW;					
					SendMsg( &message, sizeof( message ) );

					m_bNeedRevive = FALSE;

					break;
				}

				if( LOOTINGMGR->IsLootedPlayer( GetID() ) )
					break;
				if( IsReadyToRevive() != TRUE )
					break;
				
				MSGBASE msg;
				msg.Category = MP_USERCONN;
				msg.Protocol = MP_USERCONN_READY_TO_REVIVE;
				SendMsg( &msg, sizeof(msg) );

				m_bNeedRevive = FALSE;
			}
		}
		break;
	}

	AddBadFameReduceTime();
	ProcMonstermeterPlayTime();
	g_csResidentRegistManager.SRV_ProcDateMatchingChatTimeOut(this);
	ProcFarmTime();
	g_csDateManager.SRV_Process(this);
	ProcCoolTime();
	ProcSpecialSkill();
}

void CPlayer::SetWearedItemIdx(DWORD WearedPosition,DWORD ItemIdx)
{	
	const size = sizeof( m_HeroCharacterInfo.WearedItemIdx ) / sizeof( DWORD );

	if( size > WearedPosition )
	{
		m_HeroCharacterInfo.WearedItemIdx[WearedPosition] = ItemIdx;
	}
	else
	{
		ASSERT( 0 );
	}	
}

void CPlayer::QuestProcess()
{
	m_QuestGroup.Process();
}


void CPlayer::SetPKModeEndtime()
{
	m_HeroInfo.LastPKModeEndTime = 0;
}


void CPlayer::AddBadFameReduceTime()
{
	if( g_pServerSystem->GetMap()->IsVillage() )	return;
	if( GetState() == eObjectState_Immortal )		return;
	if( IsPKMode() )								return;

	m_HeroInfo.LastPKModeEndTime += gTickTime;

	//---KES PK 071202
	if( m_HeroInfo.LastPKModeEndTime > 90*60*1000 )
	{
		m_HeroInfo.LastPKModeEndTime -= 30*60*1000;	//
		PKCharacterUpdate( GetID(), m_HeroInfo.LastPKModeEndTime );

        if( GetBadFame() > 5 )
		{
			SetBadFame( GetBadFame()-5 );
		}
		else
		{
			SetBadFame( 0 );
			m_HeroInfo.LastPKModeEndTime = 0;
		}

		BadFameCharacterUpdate( GetID(), GetBadFame() );
	}
}



void CPlayer::SpeedHackCheck()
{
	++m_nHackCount;

	if( gCurTime - m_dwHackStartTime >= 60*1000 )	//1분
	{
		if( m_nHackCount >= g_nHackCheckWriteNum )
		{
			InsertSpeedHackCheck( GetID(), GetObjectName(), m_nHackCount, 1 );
		}
		if( m_nHackCount >= g_nHackCheckNum )
		{
			MSGBASE Msg;
			Msg.Category = MP_HACKCHECK;
			Msg.Protocol = MP_HACKCHECK_BAN_USER_TOAGENT;
			SendMsg( &Msg, sizeof(Msg) );
		}
		m_nHackCount		= 1;
		m_dwHackStartTime	= gCurTime;
	}
}

void CPlayer::ClearMurderIdx()
{
	SetReadyToRevive(FALSE);
	m_MurdererKind = 0;
	m_MurdererIDX = 0;
	m_bPenaltyByDie = FALSE;
}


DWORD CPlayer::Damage(CObject* pAttacker,RESULTINFO* pDamageInfo)
{
	DWORD life = GetLife();
	DWORD beforelife = life;

	if(life > pDamageInfo->RealDamage)
	{
		life -= pDamageInfo->RealDamage;
	}
	else
	{
		if( GetUserLevel() == eUSERLEVEL_GM || m_God ) // && g_pServerSystem->GetNation() == eNATION_KOREA )
		{
			life = 1;		//gm은 데미지는 받아도 죽지 않도록
		}
		else
		{
			life = 0;
		}
	}
	
	SetLife(life,FALSE);
 
	DoDamage(pAttacker,pDamageInfo,beforelife);

	return life;
}

DWORD CPlayer::ManaDamage( CObject* pAttacker, RESULTINFO* pDamageInfo )
{
	DWORD mana = GetMana();
	DWORD beforemana = mana;
	mana = (mana > pDamageInfo->ManaDamage ? mana - pDamageInfo->ManaDamage : 0);
	
	SetMana( mana, FALSE );
 
	// 마나 데미지만 있는 경우
	if( pDamageInfo->RealDamage == 0 )
		DoManaDamage( pAttacker, pDamageInfo, beforemana );

	return mana;
}

void CPlayer::SetGuildInfo(DWORD GuildIdx, BYTE Grade, const char* GuildName, MARKNAMETYPE MarkName)
{
	m_HeroCharacterInfo.MunpaID = GuildIdx;
	m_HeroCharacterInfo.PositionInMunpa = Grade;
	SafeStrCpy(m_HeroCharacterInfo.GuildName, GuildName, sizeof( m_HeroCharacterInfo.GuildName ) );
	m_HeroCharacterInfo.MarkName = MarkName;
}

void CPlayer::RSetMoney( MONEYTYPE money, BYTE flag )
{
	CPurse* pPurse = m_ItemContainer.GetPurse(eItemTable_Inventory);
	if( !pPurse ) return;

	pPurse->RSetMoney( money, flag );
	m_HeroInfo.Money = money;
}

void CPlayer::SetGuildMarkName(MARKNAMETYPE MarkName)
{
	m_HeroCharacterInfo.MarkName = MarkName;
}

char* CPlayer::GetGuildCanEntryDate()
{ 
	return m_HeroInfo.MunpaCanEntryDate;
}

void CPlayer::SetGuildCanEntryDate(char* date)
{
	SafeStrCpy(m_HeroInfo.MunpaCanEntryDate, date, 11);
}

void CPlayer::ClrGuildCanEntryDate()
{
	SafeStrCpy(m_HeroInfo.MunpaCanEntryDate, "2004.01.01", 11);
}

void CPlayer::UpdateLogoutToDB(BOOL val)
{
	if(CPet* const petObject = PETMGR->GetPet(GetPetItemDbIndex()))
	{
		if(eObjectState_Die != petObject->GetState())
		{
			petObject->SetPetObjectState(
				val ? ePetState_None : ePetState_Summon);
		}
	}

	for(int i = 0; i < _countof(m_QuickSlot); ++i)
	{
		m_QuickSlot[i].DBUpdate();
	}
}

void CPlayer::CheckImmortalTime()
{
	if(g_pServerSystem->GetMapNum() != GTMAPNUM)
		return;

	if(gTickTime<m_dwImmortalTimeOnGTMAP)
		m_dwImmortalTimeOnGTMAP-=gTickTime;
	else
	{
		m_dwImmortalTimeOnGTMAP = 0;

		MSG_DWORD2 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_TEAMMEMBER_MORTAL;
		msg.dwObjectID = GetID();
		msg.dwData1 = GetID();
		msg.dwData2 = 0;
		
		PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));
	}
}

void CPlayer::SetNickName(char* NickName)
{
	SafeStrCpy(m_HeroCharacterInfo.NickName,NickName, MAX_GUILD_NICKNAME+1);
}

void CPlayer::SetFamilyNickName(char* NickName)
{
	SafeStrCpy(m_HeroCharacterInfo.FamilyNickName,NickName, sizeof( m_HeroCharacterInfo.FamilyNickName ) );
}

LEVELTYPE CPlayer::GetLevel()
{ 
	return m_HeroCharacterInfo.Level; 
}

DWORD CPlayer::GetLife() 
{ 
	return m_HeroCharacterInfo.Life; 
}

DWORD CPlayer::GetMana()
{ 
	return m_HeroInfo.Mana; 
}

DWORD CPlayer::DoGetMaxLife()
{ 
	return m_HeroCharacterInfo.MaxLife; 
}

DWORD CPlayer::DoGetMaxMana()
{ 
	return m_HeroInfo.MaxMana; 
}

void CPlayer::SetStage( BYTE grade, BYTE index )
{
	MSG_BYTE2 msg ;

	msg.Category	= MP_CHAR ;
	msg.Protocol	= MP_CHAR_STAGE_NOTIFY ;
	msg.dwObjectID	= GetID() ;
	msg.bData1		= grade ;
	msg.bData2		= index ;
	PACKEDDATA_OBJ->QuickSend( this, &msg, sizeof(msg) );

	CharacterTotalInfoUpdate( this );	
}

WORD CPlayer::GetJobCodeForGT ()
{
	WORD JobCategory = m_HeroCharacterInfo.Job[0];
	WORD JobGrade = m_HeroCharacterInfo.JobGrade;
	WORD JobIndex = m_HeroCharacterInfo.Job[JobGrade - 1];

	if(JobGrade == 1)
		JobIndex = 1;

	WORD JobCode = JobCategory*100 + JobGrade*10 + JobIndex;

	return JobCode;
}

// 070415 LYW --- Plsyer : Add function to setting job.
void CPlayer::SetJob( BYTE jobGrade, BYTE jobIdx )
{
	MSG_BYTE2 msg ;

	msg.Category	= MP_CHAR ;
	msg.Protocol	= MP_CHAR_STAGE_NOTIFY ;
	msg.dwObjectID	= GetID() ;
	msg.bData1		= jobGrade ;
	msg.bData2		= jobIdx ;

	// 070522 LYW --- Player : Modified send job notify.
	//PACKEDDATA_OBJ->QuickSend( this, &msg, sizeof(msg) );
	SendMsg( &msg, sizeof(msg) );

	m_HeroCharacterInfo.JobGrade = jobGrade ;
	m_HeroCharacterInfo.Job[jobGrade - 1] = jobIdx ;

	CharacterJobUpdate( GetID(), jobGrade,
						m_HeroCharacterInfo.Job[0],
						m_HeroCharacterInfo.Job[1],
						m_HeroCharacterInfo.Job[2],
						m_HeroCharacterInfo.Job[3],
						m_HeroCharacterInfo.Job[4],
						m_HeroCharacterInfo.Job[5] );


	// 071112 웅주, 클래스 로그를 남긴다
	InsertLogJob( this, m_HeroCharacterInfo.Job[0], jobGrade, jobIdx );

	// 081022 KTH -- 
	CHARCALCMGR->AddPlayerJobSkill(this);

	WebEvent( GetUserID(), 2 );


	// 100113 ONS 주민등록정보중 클래스정보가 변경되었을 경우 에이전트로 전달한다.
	CHARACTER_TOTALINFO TotalInfo;
	ZeroMemory(&TotalInfo, sizeof(TotalInfo));
	GetCharacterTotalInfo(&TotalInfo);

	WORD idx = 1;
	if( TotalInfo.JobGrade > 1 )
	{
		idx = TotalInfo.Job[TotalInfo.JobGrade - 1];
	}
	DWORD dwClass = ( TotalInfo.Job[0] * 1000 ) + ( ( TotalInfo.Race + 1 ) * 100 ) + ( TotalInfo.JobGrade * 10 ) + idx;

	MSG_DWORD Packet;
	ZeroMemory(&Packet, sizeof(Packet));
	Packet.Category	= MP_RESIDENTREGIST;
	Packet.Protocol	= MP_RESIDENTREGIST_REGIST_CHANGE;
	Packet.dwObjectID = GetID();
	Packet.dwData = dwClass;
	g_Network.Broadcast2AgentServer( ( char* )&Packet, sizeof( Packet ) );

	// 080225 LUJ, 길드 회원일 경우 직업 정보 변경을 전 서버에 전파해야한다
	{
		CGuild* guild = GUILDMGR->GetGuild( GetGuildIdx() );

		if( ! guild )
		{
			return;
		}

		GUILDMEMBERINFO* member = guild->GetMemberInfo( GetID() );

		if( ! member )
		{
			return;
		}

		member->mJobGrade	= jobGrade;
		member->mRace		= m_HeroCharacterInfo.Race;
		memcpy( member->mJob, m_HeroCharacterInfo.Job, sizeof( member->mJob ) );

		SEND_GUILD_MEMBER_INFO message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_SET_MEMBER_TO_MAP;
		message.GuildIdx	= GetGuildIdx();
		message.MemberInfo	= *member;

		g_Network.Send2AgentServer( ( char* )&message, sizeof( message ) );

		GUILDMGR->NetworkMsgParse( message.Protocol, &message );
	}	
}

void CPlayer::SendPlayerToMap(MAPTYPE mapNum, float xpos, float zpos)
{
	MSG_DWORD3 msg ;														// 메시지 구조체를 선언한다.
	memset(&msg, 0, sizeof(MSG_DWORD3)) ;									// 메시지 초기화.

	msg.Category	= MP_USERCONN ;
	msg.Protocol	= MP_USERCONN_QUEST_CHANGEMAP_SYN ;						// 카테고리와 프로토콜을 세팅한다.

	msg.dwObjectID	= GetID() ;												// 오브젝트 아이디를 세팅한다.

	msg.dwData1		= (DWORD)mapNum ;										// 도착 지역의 맵 번호를 세팅한다.
	msg.dwData2		= (DWORD)xpos ;											// X좌표를 세팅한다.
	msg.dwData3		= (DWORD)zpos ;											// Z좌표를 세팅한다.

	SendMsg( &msg, sizeof(MSG_DWORD3) ) ;									// 메시지를 전송한다.
}

DOUBLE CPlayer::GetPlayerTotalExpPoint()
{
	DOUBLE exp = 0;

	for(LEVELTYPE i=1; i<GetLevel(); ++i)
	{
		exp = exp + GAMERESRCMNGR->GetMaxExpPoint( i ) ;
	}

	exp += GetPlayerExpPoint();

	return exp;
}

void CPlayer::InitGuildUnionInfo( DWORD dwGuildUnionIdx, char* pGuildUnionName, DWORD dwMarkIdx )
{
	m_HeroCharacterInfo.dwGuildUnionIdx = dwGuildUnionIdx;
	strncpy( m_HeroCharacterInfo.sGuildUnionName, pGuildUnionName, MAX_GUILD_NAME+1 );
	m_HeroCharacterInfo.dwGuildUnionMarkIdx = dwMarkIdx;
}

void CPlayer::SetGuildUnionInfo( DWORD dwGuildUnionIdx, char* pGuildUnionName, DWORD dwMarkIdx )
{
	m_HeroCharacterInfo.dwGuildUnionIdx = dwGuildUnionIdx;
	strncpy( m_HeroCharacterInfo.sGuildUnionName, pGuildUnionName, sizeof( m_HeroCharacterInfo.sGuildUnionName ) );
	m_HeroCharacterInfo.dwGuildUnionMarkIdx = dwMarkIdx;

	MSG_NAME_DWORD3 Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_PLAYER_INFO;
	Msg.dwData1 = GetID();
	Msg.dwData2 = dwGuildUnionIdx;
	Msg.dwData3 = dwMarkIdx;
	strncpy( Msg.Name, pGuildUnionName, sizeof( Msg.Name ) );

	PACKEDDATA_OBJ->QuickSendExceptObjectSelf( this, &Msg, sizeof(Msg) );
}

float CPlayer::GetAccuracy()
{
	return mAccuracy;
}

float CPlayer::GetAvoid()
{
	return mAvoid;
}

// 100221 ShinJS --- 블럭공식 수정
float CPlayer::GetBlock()
{
	const float rate	= mRateBuffStatus.Block + mRatePassiveStatus.Block;
	float		bonus	= 0;

	switch( GetJop( 0 ) )
	{
		// 080910 LUJ, 파이터
	case 1:
		{
			bonus = 15.f;
			break;
		}
		// 080910 LUJ, 로그
	case 2:
		{
			bonus = 10.f;
			break;
		}
		// 080910 LUJ, 메이지
	case 3:
		{
			bonus = 5.f;
			break;
		}
		// 100218 ShinJS --- 마족
	case 4:
		{
			bonus = 9.f;
			break;
		}
	}

	return ( float( GetDexterity() / 27.f ) + rate + bonus );
}

float CPlayer::GetPhysicAttackMax()
{
	return mPhysicAttackMax;
}

float CPlayer::GetPhysicAttackMin()
{
	return mPhysicAttackMin;
}

float CPlayer::GetPhysicAttack()
{
	return float( random( mPhysicAttackMin, mPhysicAttackMax ) );;
}

float CPlayer::GetMagicAttackMax()
{
	return mMagicAttackMax;
}

float CPlayer::GetMagicAttackMin()
{
	return mMagicAttackMin;
}

float CPlayer::GetMagicAttack()
{
	return float( random( mMagicAttackMin, mMagicAttackMax ) );;
}

float CPlayer::GetPhysicDefense()
{
	return mPhysicDefense;
}

float CPlayer::GetMagicDefense()
{
	return mMagicDefense;
}

float CPlayer::GetCriticalRate()
{
	return mCriticalRate;
}

float CPlayer::GetCriticalDamageRate()
{
	return mCriticalDamageRate;
}

float CPlayer::GetCriticalDamagePlus()
{
	return mCriticalDamagePlus;
}

float CPlayer::GetMagicCriticalRate()
{
	return mMagicCriticalRate;
}

float CPlayer::GetMagicCriticalDamageRate()
{
	return mMagicCriticalDamageRate;
}

float CPlayer::GetMagicCriticalDamagePlus()
{
	return mMagicCriticalDamagePlus;
}

float CPlayer::GetLifeRecover()
{
	return mLifeRecover;
}

float CPlayer::GetManaRecover()
{
	return mManaRecover;
}

////////////////////////////////////////////////////////////////////////////
// 06. 07. 상태강제변경 - 이영준
// 일정이상 스킬 사용실패시 특별한 상태를 제외한 나머지 상태는 모두 초기화
// 정상적인 상태에서는 값을 초기화
// 실패값을 누적하지 않는다.
// 연속 실패시에만 상태해제
BOOL CPlayer::CanSkillState()
{
	//연속 5회 이상 실패시
	if(m_SkillFailCount >= 5)
	{
		switch(m_BaseObjectInfo.ObjectState)
		{
		// 스킬을 사용할수 있는 상태면 값을 초기화하고 TRUE 리턴
		case eObjectState_None:
		case eObjectState_Move:
		case eObjectState_TiedUp_CanSkill:
			{
				m_SkillFailCount = 0;
				return TRUE;
			}
			break;
		// 스킬을 사용할수 없는 상태중 풀어줘도 무방한 상태면 값을 초기화 하고
		// 상태를 초기화 한뒤 TRUE 리턴
		case eObjectState_SkillStart:
		case eObjectState_SkillSyn:	
		case eObjectState_SkillBinding:
		case eObjectState_SkillUsing:
		case eObjectState_SkillDelay:
		case eObjectState_Society:
		case eObjectState_Enter:
		case eObjectState_Rest:
			{
				m_SkillFailCount = 0;
				m_BaseObjectInfo.ObjectState = eObjectState_None;
				return TRUE;
			}
			break;
		// 그 외의 경우엔 값을 초기화 하고 FALSE 리턴
		default:
			{
				m_SkillFailCount = 0;
				return FALSE;
			}
			break;
		}
	}

	// 5회 이하 일때 스킬을 사용할수 없는 상태면
	// 값을 증가하고 FALSE 리턴
	if(m_BaseObjectInfo.ObjectState != eObjectState_None &&
	   m_BaseObjectInfo.ObjectState != eObjectState_Move &&
	   m_BaseObjectInfo.ObjectState != eObjectState_TiedUp_CanSkill )
	{
		m_SkillFailCount++;
		return FALSE;
	}

	// 정상 상태일때 값을 초기화하고 TRUE 리턴
	m_SkillFailCount = 0;
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////

BOOL CPlayer::AddQuick( BYTE sheet, WORD pos, SLOT_INFO* pSlot )
{
	return m_QuickSlot[ sheet ].Update( pos, pSlot );
}

SLOT_INFO*	CPlayer::GetQuick( BYTE sheet, WORD pos )
{
	return m_QuickSlot[ sheet ].GetQuick( pos );
}

// desc_hseos_몬스터미터01
// S 몬스터미터 추가 added by hseos 2007.05.23	2007.07.08
void CPlayer::ProcMonstermeterPlayTime()
{
	if (gCurTime - m_stMonstermeterInfo.nPlayTimeTick > SHMath_MINUTE(1))
	{
		m_stMonstermeterInfo.nPlayTimeTick = gCurTime;
		m_stMonstermeterInfo.nPlayTime++;
		m_stMonstermeterInfo.nPlayTimeTotal++;

		// DB에 저장
		//MonsterMeter_Save(GetID(), m_stMonstermeterInfo.nPlayTime, m_stMonstermeterInfo.nKillMonsterNum, m_stMonstermeterInfo.nPlayTimeTotal, m_stMonstermeterInfo.nKillMonsterNumTotal);

		// 플레이시간은 클라이언트에서도 계산 가능하므로 클라이언트에서 계산하도록 하고
		// 혹시 모를 오차떄문에 10분마다 서버의 수치를 보내준다.
		if ((m_stMonstermeterInfo.nPlayTime%10) == 0)
		{
			// 클라이언트에 알리기
			MSG_DWORD2 msg;
			msg.Category = MP_CHAR;
			msg.Protocol = MP_CHAR_MONSTERMETER_PLAYTIME;
			msg.dwObjectID = GetID();
			msg.dwData1 = m_stMonstermeterInfo.nPlayTime;
			msg.dwData2 = m_stMonstermeterInfo.nPlayTimeTotal;
			SendMsg(&msg, sizeof(msg));
		}

		if( m_stMonstermeterInfo.nPlayTimeTotal == 30 * 60 )
		{
			WebEvent( GetUserID(), 6 );
		}
		// 보상 처리
		g_csMonstermeterManager.ProcessReward(this, CSHMonstermeterManager::RBT_PLAYTIME, m_stMonstermeterInfo.nPlayTimeTotal);
	}
}
// E 몬스터미터 추가 added by hseos 2007.05.23	2007.07.08

// desc_hseos_몬스터미터01
// S 몬스터미터 추가 added by hseos 2007.05.23	2007.07.08
void CPlayer::ProcMonstermeterKillMon()
{
	m_stMonstermeterInfo.nKillMonsterNum++;
	m_stMonstermeterInfo.nKillMonsterNumTotal++;

	// DB에 저장
	//MonsterMeter_Save(GetID(), m_stMonstermeterInfo.nPlayTime, m_stMonstermeterInfo.nKillMonsterNum, m_stMonstermeterInfo.nPlayTimeTotal, m_stMonstermeterInfo.nKillMonsterNumTotal);

	// 클라이언트에 알리기
	MSG_DWORD2 msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_MONSTERMETER_KILLMONSTER;
	msg.dwObjectID = GetID();
	msg.dwData1 = m_stMonstermeterInfo.nKillMonsterNum;
	msg.dwData2 = m_stMonstermeterInfo.nKillMonsterNumTotal;
	SendMsg(&msg, sizeof(msg));

	if( m_stMonstermeterInfo.nKillMonsterNumTotal == 1000 )
	{
		WebEvent( GetUserID(), 9 );
	}

	// 보상 처리
	g_csMonstermeterManager.ProcessReward(this, CSHMonstermeterManager::RBT_MONSTERKILL, m_stMonstermeterInfo.nKillMonsterNumTotal);
}

void CPlayer::ProcFarmTime()
{
	if (m_stFarmInfo.nCropPlantRetryTime)
	{
		if (gCurTime - m_stFarmInfo.nCropPlantRetryTimeTick > SHMath_SECOND(1))
		{
			m_stFarmInfo.nCropPlantRetryTimeTick = gCurTime;
			m_stFarmInfo.nCropPlantRetryTime--;

			// DB에 저장
			Farm_SetTimeDelay(GetID(), CSHFarmManager::FARM_TIMEDELAY_KIND_PLANT, m_stFarmInfo.nCropPlantRetryTime);
		}
	}

	if (m_stFarmInfo.nCropManureRetryTime)
	{
		if (gCurTime - m_stFarmInfo.nCropManureRetryTimeTick > SHMath_SECOND(1))
		{
			m_stFarmInfo.nCropManureRetryTimeTick = gCurTime;
			m_stFarmInfo.nCropManureRetryTime--;

			// DB에 저?			Farm_SetTimeDelay(GetID(), CSHFarmManager::FARM_TIMEDELAY_KIND_MANURE, m_stFarmInfo.nCropManureRetryTime);
		}
	}

	// 080430 KTH Animal Delay Add  (분)단위는 여기서 수치를 Decrease 해주는군...
	if( m_stFarmInfo.nAnimalCleanRetryTime )
	{
		if( gCurTime - m_stFarmInfo.nAnimalCleanRetryTimeTick > SHMath_MINUTE(1) )
		{
			m_stFarmInfo.nAnimalCleanRetryTimeTick = gCurTime;
			m_stFarmInfo.nAnimalCleanRetryTime--;

			Farm_SetTimeDelay(GetID(), CSHFarmManager::FARM_TIMEDELAY_KIND_CLEAN, m_stFarmInfo.nAnimalCleanRetryTime);
		}
	}

	if( m_stFarmInfo.nAnimalFeedRetryTime )
	{
		if( gCurTime - m_stFarmInfo.nAnimalFeedRetryTimeTick > SHMath_SECOND(1) )
		{
			m_stFarmInfo.nAnimalFeedRetryTimeTick = gCurTime;
			m_stFarmInfo.nAnimalFeedRetryTime--;

			Farm_SetTimeDelay(GetID(), CSHFarmManager::FARM_TIMEDELAY_KIND_FEED, m_stFarmInfo.nAnimalFeedRetryTime);
		}
	}
}
// E 농장시스템 추가 added by hseos 2007.08.23

BOOL CPlayer::IsInventoryPosition( POSTYPE position )
{
	const CItemSlot* slot = GetSlot( eItemTable_Inventory );

    const POSTYPE begin = slot->GetStartPos();
	const POSTYPE end	= slot->GetSlotNum() + begin;

	return begin <= position && end >= position;
}


void CPlayer::ResetSetItemStatus()
{
	mSetItemLevel.clear();
	ZeroMemory(
		&m_SetItemStats,
		sizeof(m_SetItemStats));
}


const CPlayer::SetItemLevel& CPlayer::GetSetItemLevel() const
{
	return mSetItemLevel;
}


CPlayer::SetItemLevel& CPlayer::GetSetItemLevel()
{
	return mSetItemLevel;
}

void CPlayer::AddJobSkill( DWORD skillIndex, BYTE level )
{
	SKILL_BASE SkillBase;
	SkillBase.dwDBIdx = 0;
	SkillBase.wSkillIdx = skillIndex;
	SkillBase.Level = level;
	m_JobSkillList.push_back(SkillBase);

	AddSetSkill(skillIndex, level);
}

void CPlayer::ClearJobSkill()
{
	for(std::list< SKILL_BASE >::iterator iterator = m_JobSkillList.begin();
		iterator != m_JobSkillList.end();
		++iterator)
	{
		SKILL_BASE* skill = m_SkillTree->GetData(iterator->wSkillIdx);

		if( skill )
		{
			RemoveSetSkill(
				skill->wSkillIdx,
				skill->Level);
		}
	}

	m_JobSkillList.clear();
}

void CPlayer::AddSetSkill(DWORD skillIndex, LEVELTYPE level)
{
	SKILL_BASE skill = {0};
	skill.wSkillIdx = skillIndex;
	skill.Level = level;

	const SKILL_BASE* const oldSkill = m_SkillTree->GetData(
		skillIndex);

	if(oldSkill)
	{
		skill = *oldSkill;
		skill.Level = skill.Level + level;
	}

	m_SkillTree->Update(
		skill);
}


void CPlayer::RemoveSetSkill(DWORD skillIndex, LEVELTYPE level)
{
	SKILL_BASE* const oldSkill = m_SkillTree->GetData(
		skillIndex);

	if(0 == oldSkill)
	{
		return;
	}

	SKILL_BASE skill = *oldSkill;
	skill.Level = skill.Level - level;

	m_SkillTree->Update(
		skill);
}

// 090217 LUJ, 목적에 맞도록 함수 이름 변경
void CPlayer::SetHideLevel( WORD level )
{ 
	m_HeroCharacterInfo.HideLevel = level; 
	
	if( level )
	{
		m_HeroCharacterInfo.bVisible = false;
	}
	else
	{
		m_HeroCharacterInfo.bVisible = true;
	}

	MSG_WORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_HIDE_NOTIFY;
	msg.dwObjectID = GetID();
	msg.wData = level;

	PACKEDDATA_OBJ->QuickSend( this, &msg, sizeof( msg ) );	
}

// 090217 LUJ, 목적에 맞게 함수 이름 변경
void CPlayer::SetDetectLevel( WORD level )
{ 
	m_HeroCharacterInfo.DetectLevel = level; 
	
	MSG_WORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_DETECT_NOTIFY;
	msg.dwObjectID = GetID();
	msg.wData = level;

	PACKEDDATA_OBJ->QuickSend( this, &msg, sizeof( msg ) );	
}	

void CPlayer::RemoveAllAggroed()
{
	while(false == mAggroObjectContainer.empty())
	{
		const ObjectIndex objectIndex = *(mAggroObjectContainer.begin());
		mAggroObjectContainer.erase(
			objectIndex);

		CMonster* const monster = (CMonster*)g_pUserTable->FindUser(
			objectIndex);

		if(0 == monster ||
			FALSE == (eObjectKind_Monster & monster->GetObjectKind()))
		{
			continue;
		}

		monster->RemoveFromAggro(
			GetID());
		monster->GetAbnormalStatus()->Attract = 0;
	}
}

void CPlayer::AddAggroToMyMonsters(int nAggroAdd, DWORD targetObjectIndex, DWORD skillIndex)
{
	for(ObjectIndexContainer::const_iterator iterator = mAggroObjectContainer.begin();
		mAggroObjectContainer.end() != iterator;
		++iterator)
	{
		const ObjectIndex objectIndex = *iterator;
		CMonster* const monster = (CMonster*)g_pUserTable->FindUser(
			objectIndex);

		if(0 == monster ||
			FALSE == (eObjectKind_Monster & monster->GetObjectKind()))
		{
			continue;
		}

		monster->AddAggro(
			targetObjectIndex,
			nAggroAdd,
			skillIndex);
	}
}

// 080910 LUJ, 방패의 방어력을 반환한다
DWORD CPlayer::GetShieldDefense()
{
	return mShieldDefense;
}

// 080910 LUJ, 방패의 방어력을 설정한다
void CPlayer::SetShieldDefence( DWORD shieldDefense )
{
	mShieldDefense = shieldDefense;
}

BOOL CPlayer::AddCoolTime( DWORD coolTimeGroupIndex, DWORD coolTime )
{
	ProcCoolTime();

	if(mCoolTimeMap.end() != mCoolTimeMap.find( coolTimeGroupIndex ))
	{
		return FALSE;
	}

	CoolTime& time = mCoolTimeMap[ coolTimeGroupIndex ];
	const DWORD tick = GetTickCount();

	time.mBeginTick = tick;
	time.mEndTick	= tick + coolTime;

	return TRUE;
}

void CPlayer::RemoveCoolTime( DWORD coolTimeGroupIndex)
{
	mCoolTimeMap.erase(
		coolTimeGroupIndex);
}


void CPlayer::ProcCoolTime()
{
	if(true == mCoolTimeMap.empty())
	{
		return;
	}

	const DWORD tick = GetTickCount();

	std::set< DWORD > group;

	for(CoolTimeMap::iterator it = mCoolTimeMap.begin(); mCoolTimeMap.end() != it; ++it)
	{
		const CoolTime& time = it->second;

		if( ( time.mBeginTick < time.mEndTick && time.mEndTick < tick ) ||
			( time.mBeginTick > time.mEndTick && time.mBeginTick > tick && time.mEndTick < tick ) )
		{
			// 주의: 삭제한 이후에는 반복자가 잘못된다는 사실을 명심. 따라서 다음 프로세스에서 처리해야 한다.
			group.insert( it->first );
		}
	}

	for( std::set< DWORD > ::const_iterator it = group.begin(); group.end() != it; ++it )
	{
		mCoolTimeMap.erase(*it);
	}
}

DWORD CPlayer::GetVitality() 
{ 
	const float rate =
		mRatePassiveStatus.Vit +
		mRateBuffStatus.Vit +
		m_itemBaseStats.mVitality.mPercent +
		m_itemOptionStats.mVitality.mPercent +
		m_SetItemStats.mVitality.mPercent * 100.f;
	const float plus =
		mPassiveStatus.Vit +
		mBuffStatus.Vit +
		m_itemBaseStats.mVitality.mPlus +
		m_itemOptionStats.mVitality.mPlus +
		m_SetItemStats.mVitality.mPlus;
	float Result = m_charStats.mVitality.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	return (DWORD)( Round( Result, 1 ) );
}

DWORD CPlayer::GetWisdom() 
{ 
	const float	rate =
		mRatePassiveStatus.Wis +
		mRateBuffStatus.Wis +
		m_itemBaseStats.mWisdom.mPercent +
		m_itemOptionStats.mWisdom.mPercent +
		m_SetItemStats.mWisdom.mPercent * 100.f;
	const float plus =
		mPassiveStatus.Wis +
		mBuffStatus.Wis +
		m_itemBaseStats.mWisdom.mPlus +
		m_itemOptionStats.mWisdom.mPlus +
		m_SetItemStats.mWisdom.mPlus;
	float Result = m_charStats.mWisdom.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	return (DWORD)( Round( Result, 1 ) );
}

DWORD CPlayer::GetStrength() 
{
	const float	rate =
		mRatePassiveStatus.Str +
		mRateBuffStatus.Str +
		m_itemBaseStats.mStrength.mPercent +
		m_itemOptionStats.mStrength.mPercent +
		m_SetItemStats.mStrength.mPercent * 100.f;
	const float	plus =
		mPassiveStatus.Str +
		mBuffStatus.Str +
		m_itemBaseStats.mStrength.mPlus +
		m_itemOptionStats.mStrength.mPlus +
		m_SetItemStats.mStrength.mPlus;
	float Result = m_charStats.mStrength.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	return (DWORD)( Round( Result, 1 ) );
}

DWORD CPlayer::GetDexterity() 
{ 
	const float rate =
		mRatePassiveStatus.Dex +
		mRateBuffStatus.Dex +
		m_itemBaseStats.mDexterity.mPercent +
		m_itemOptionStats.mDexterity.mPercent +
		m_SetItemStats.mDexterity.mPercent * 100.f;
	const float plus =
		mPassiveStatus.Dex +
		mBuffStatus.Dex +
		m_itemBaseStats.mDexterity.mPlus +
		m_itemOptionStats.mDexterity.mPlus +
		m_SetItemStats.mDexterity.mPlus;
	float Result = m_charStats.mDexterity.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	return (DWORD)( Round( Result, 1 ) );
}

DWORD CPlayer::GetIntelligence() 
{
	const float rate =
		mRatePassiveStatus.Int +
		mRateBuffStatus.Int +
		m_itemBaseStats.mIntelligence.mPercent +
		m_itemOptionStats.mIntelligence.mPercent +
		m_SetItemStats.mIntelligence.mPercent * 100.f;
	const float plus =
		mPassiveStatus.Int +
		mBuffStatus.Int +
		m_itemBaseStats.mIntelligence.mPlus +
		m_itemOptionStats.mIntelligence.mPlus +
		m_SetItemStats.mIntelligence.mPlus;
	float Result = m_charStats.mIntelligence.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	return (DWORD)( Round( Result, 1 ) );
}

void CPlayer::SetObjectBattleState(eObjectBattleState state)
{ 
	m_BaseObjectInfo.ObjectBattleState = state; 

	if( state )	//eObjectBattleState_Battle
	{
		mpBattleStateDelay->Start();
	}
}

void CPlayer::ProcessTimeCheckItem( BOOL bForceDBUpdate )
{
	DWORD dwElapsedMili = gCurTime - m_dwLastTimeCheckItem;
	if(  dwElapsedMili < 60 * 1000 && !bForceDBUpdate )
		return;

	DWORD dwElapsedSecond = dwElapsedMili / 1000;

	m_dwLastTimeCheckItem = gCurTime - ( dwElapsedMili - dwElapsedSecond * 1000 ) ;

	BOOL bDBUpdate = bForceDBUpdate ? TRUE : FALSE;

	if( gCurTime - m_dwLastTimeCheckItemDBUpdate > 5 * 60 * 1000 )
	{
		bDBUpdate = TRUE;
		m_dwLastTimeCheckItemDBUpdate = gCurTime;
	}

	int iCheckItemMaxNum = TP_WEAR_END;
	if( IsGotWarehouseItems() )
	{
		iCheckItemMaxNum = TP_STORAGE_END;
	}

	for( POSTYPE i = TP_INVENTORY_START ; i <= iCheckItemMaxNum ; ++i )
	{
		ITEMBASE* pItemBase = (ITEMBASE*)m_ItemContainer.GetItemInfoAbs( i );
		const ITEM_INFO* info = ITEMMGR->GetItemInfo( pItemBase->wIconIdx );

		if(0 == info)
		{
			continue;
		}
		else if(info->SupplyType == ITEM_KIND_COOLTIME )
		{
			pItemBase->nRemainSecond = max( 0, int( pItemBase->nRemainSecond - dwElapsedSecond ) );
			pItemBase->ItemParam = gCurTime;

			UpdateRemainTime(
				GetID(),
				pItemBase->dwDBIdx,
				pItemBase->nSealed,
				pItemBase->nRemainSecond);
		}
		else if( pItemBase->nSealed == eITEM_TYPE_UNSEAL )
		{
			if( pItemBase->nRemainSecond > (int)dwElapsedSecond )
			{
				pItemBase->nRemainSecond -= dwElapsedSecond;

				// 071125 KTH --- Player "RemainSecond가 1분 미만일 경우 클라이언트에게 정보를 보내준다."
				if( pItemBase->nRemainSecond <= 60 )
				{
					MSG_DWORD2 msg;
					msg.Category = MP_ITEM;
					msg.Protocol = MP_ITEM_TIMELIMT_ITEM_ONEMINUTE;
					msg.dwData1 = pItemBase->wIconIdx;
					msg.dwData2 = pItemBase->Position;
	
					SendMsg(&msg, sizeof(msg));
				}

				if( bDBUpdate )
				{
					if( info->nTimeKind == eKIND_PLAYTIME )
					{
						UpdateRemainTime(
							GetID(),
							pItemBase->dwDBIdx,
							pItemBase->nSealed,
							pItemBase->nRemainSecond);
					}
				}
			}
			else
			{
				POSTYPE	position = pItemBase->Position;
				DWORD	iconIdx = pItemBase->wIconIdx;
				int returnValue = ITEMMGR->DiscardItem(
					this,
					pItemBase->Position,
					pItemBase->wIconIdx,
					pItemBase->Durability);

				if( EI_TRUE == returnValue )
				{
					CVehicle* const vehicle = ( CVehicle* )g_pUserTable->FindUser( GetSummonedVehicle() );

					// 090316 LUJ, 해당 아이템으로 소환된 탈 것을 소환 해제한다
					if( vehicle &&
						vehicle->GetObjectKind() == eObjectKind_Vehicle )
					{
						const ICONBASE& useItem = vehicle->GetUseItem();

						if( useItem.wIconIdx == iconIdx )
						{
							vehicle->DoDie( 0 );
						}
					}

					MSG_ITEM_DISCARD_SYN msg;
					ZeroMemory(&msg, sizeof(msg));
					msg.Category = MP_ITEM;
					msg.Protocol = MP_ITEM_DISCARD_ACK;
					msg.TargetPos = position;
					msg.wItemIdx = iconIdx;
					msg.ItemNum = 1;
					SendMsg(&msg, sizeof(msg));	

					LogItemMoney(
						GetID(),
						GetObjectName(),
						0,
						"",
						eLog_ShopItemUseEnd,
						GetMoney(),
						0,
						0,
						iconIdx,
						pItemBase->dwDBIdx,
						pItemBase->Position,
						0,
						1,
						GetPlayerExpPoint() );
				}
			}
		}
	}

	// NYJ - 시간제 요리레시피 시간소모
	ProcessRecipeTimeCheck(dwElapsedMili);

	{
		CPet* const petObject = PETMGR->GetPet(
			GetPetItemDbIndex());

		if(petObject)
		{
			petObject->ProcessTimeCheckItem(
				bDBUpdate,
				dwElapsedMili);
		}
	}

	// 100525 NYJ - 판매대행 등록상품에 대해 시간경과체크 수행
	Consignment_CheckDate(GetID());
	Note_CheckDate(GetID());
}

// desc_hseos_결혼_01
// S 결혼 추가 added by hseos 2008.01.29
BOOL CPlayer::RemoveItem(DWORD nItemID, DWORD nItemNum, eLogitemmoney eLogKind)
{
	int iCheckItemMaxNum = TP_WEAR_END;
	if( IsGotWarehouseItems() )
	{
		iCheckItemMaxNum = TP_STORAGE_END;
	}

	for( POSTYPE i = TP_INVENTORY_START ; i <= iCheckItemMaxNum ; ++i )
	{
		ITEMBASE* pItemBase = (ITEMBASE*)m_ItemContainer.GetItemInfoAbs( i );

		if( pItemBase->wIconIdx == nItemID )
		{
			BOOL dbidx = pItemBase->dwDBIdx;
			POSTYPE	position = pItemBase->Position;
			DWORD	iconIdx = pItemBase->wIconIdx;
			int nResult = ITEMMGR->DiscardItem( this, pItemBase->Position, pItemBase->wIconIdx, nItemNum );
			if (nResult != EI_TRUE)
			{
				return FALSE;
			}

			LogItemMoney(
				GetID(),
				GetObjectName(),
				0,
				"",
				eLogKind,
				GetMoney(),
				0,
				0,
				iconIdx,
				dbidx,
				position,
				0,
				nItemNum,
				0);

			MSG_ITEM_DISCARD_SYN msg;
			ZeroMemory(&msg, sizeof(msg));
			msg.Category = MP_ITEM;
			msg.Protocol = MP_ITEM_DISCARD_ACK;
			msg.TargetPos = position;
			msg.wItemIdx = iconIdx;
			msg.ItemNum = nItemNum;
			SendMsg(&msg, sizeof(msg));	
		}
	}

	return TRUE;
}
// E 결혼 추가 added by hseos 2008.01.29


BOOL CPlayer::IncreaseInventoryExpansion()
{
	if( GetInventoryExpansion() >= 2 )
		return FALSE;

	IncreaseCharacterInventory(this->GetID());

	return TRUE;
}

void CPlayer::PassiveSkillCheckForWeareItem()
{
	memset( &mPassiveStatus, 0, sizeof( Status ) );
	memset( &mRatePassiveStatus, 0, sizeof( Status ) );

	m_SkillTree->SetPositionHead();

	for(SKILL_BASE* pSkillBase = m_SkillTree->GetData();
		0 < pSkillBase;
		pSkillBase = m_SkillTree->GetData())
	{
		const DWORD skillLevel = min(
			pSkillBase->Level,
			SKILLMGR->GetSkillSize(pSkillBase->wSkillIdx));
		const cActiveSkillInfo* const pSkill = SKILLMGR->GetActiveInfo( 
			pSkillBase->wSkillIdx - 1 + skillLevel);
		
		if(0 == pSkill)
		{
			continue;
		}
		else if(SKILLKIND_PASSIVE != pSkill->GetKind())
		{
			continue;
		}

		for(DWORD index = 0; index < MAX_BUFF_COUNT; ++index)
		{
			const cBuffSkillInfo* const pSkillInfo = ( cBuffSkillInfo* )SKILLMGR->GetBuffInfo(
				pSkill->GetInfo().Buff[index]);

			if(0 == pSkillInfo)
			{
				break;
			}
			else if(FALSE == IsEnable(pSkillInfo->GetInfo()))
			{
				continue;
			}

			pSkillInfo->AddPassiveStatus(
				this);
		}
	}
}

BOOL CPlayer::ClearInventory()
{
	for( POSTYPE i = TP_INVENTORY_START ; i < TP_WEAR_START ; ++i )
	{
		ITEMBASE* pItemBase = (ITEMBASE*)m_ItemContainer.GetItemInfoAbs( i );
		if( pItemBase->dwDBIdx == 0 ) continue;

		int returnValue = ITEMMGR->DiscardItem( this, pItemBase->Position, pItemBase->wIconIdx, pItemBase->Durability );
		if( EI_TRUE == returnValue )
		{
			continue;
		}
	}
	return TRUE;
}

void CPlayer::FishingInfoClear()
{
	SetFishingPlace(0);
	SetFishingStartTime(0);
	SetFishingProcessTime(0);
	SetFishingBait(0);
	ZeroMemory(
		m_fFishBaitRate,
		sizeof(m_fFishBaitRate));
}

void CPlayer::SetFishingExp(EXPTYPE dwExp)
{
	const LEVELTYPE& level = m_wFishingLevel;

	if(MAX_FISHING_LEVEL <= level)
	{
		m_wFishingLevel = MAX_FISHING_LEVEL;
		m_dwFishingExp = 0;
		return;
	}
	
	// 경험치가 다음 단계에서 요구하는 것보다 훨씬 많을 수 있으므로,
	// 계속 체크해서 레벨업하자
	{
		EXPTYPE nextPoint = 0 ;
		nextPoint = GAMERESRCMNGR->GetFishingMaxExpPoint( level ) ;

		if(nextPoint == 0)
		{
			return ;
		}

		while( dwExp >= nextPoint )
		{
			if(nextPoint==0)
				break;

			m_wFishingLevel++;

			if(GetLevel() < GAMERESRCMNGR->GetFishingLevelUpRestrict(level))
			{
				m_wFishingLevel--;
				m_dwFishingExp = dwExp = nextPoint;

				MSG_WORD msg;
				msg.Category = MP_FISHING;
				msg.Protocol = MP_FISHING_LEVELUP_NACK;
				msg.wData = m_wFishingLevel;
				SendMsg(&msg, sizeof(msg));	
				break;
			}

			Log_Fishing(
				GetID(),
				eFishingLog_SetLevel,
				0,
				GetFishPoint(),
				0,
				0,
				dwExp - nextPoint,
				GetFishingLevel() );

			// 100607 NYJ 보조기술 레벨업시 아이템지급
			DWORD dwRewardItem = GAMERESRCMNGR->GetFishingLevelUpReward(level);
			if(dwRewardItem)
			{
				ITEM_INFO* pInfo = ITEMMGR->GetItemInfo(dwRewardItem);
				if(pInfo)
				{
					// 2286, 2287, "2288" 는 SystemMsg.bin의 인덱스
					ItemInsertToNote(GetID(), dwRewardItem, 1, pInfo->wSeal, 0, eNoteParsing_FishingLevelUp, 2286, 2287, "2288");
				}
			}

			if( m_wFishingLevel == 2 )
			{
				WebEvent( GetUserID(), 8 );
			}

			MSG_WORD msg;
			msg.Category = MP_FISHING;
			msg.Protocol = MP_FISHING_LEVELUP_ACK;
			msg.wData = m_wFishingLevel;
			SendMsg(&msg, sizeof(msg));	

			dwExp		-=	nextPoint;
			nextPoint	=	GAMERESRCMNGR->GetFishingMaxExpPoint( level );
		}

		m_dwFishingExp = dwExp;
	}
}

// 080509 LUJ, 스킬 쿨타임이 지나지 않았으면 참을 반환한다
// 080514 LUJ, 스킬 애니메이션 시간 체크
// 080515 LUJ, 쿨타임과 애니메이션 시간 체크로 인해 스킬이 얼마나 실패하는지 점검하기 위해 로그를 작성한다
// 080516 LUJ, 쿨타임 체크 실패가 허용 범위 이상일 때 접속을 종료시킴
BOOL CPlayer::IsCoolTime( const ACTIVE_SKILL_INFO& skill )
{
	// 080516 LUJ, 쿨타임 실패가 발생해도 일정 회수 이상은 허용한다. 그 이상이 발생하면 접속을 강제로 종료시킨다
	struct
	{
		void operator() ( CPlayer& player, CPlayer::CheckCoolTime& checkCoolTime )
		{
			const DWORD maxCheckTick = 1000 * 60;

			// 080516 LUJ, 쿨타임 실패가 발생한 지 일정한 시간이 지났으면 체크 데이터를 초기화한다
			// 080519 LUJ, maxCheckTick 내에 발생한 오류 체크를 하지 못하던 것 수정
			if( maxCheckTick < ( checkCoolTime.mCheckedTick - gCurTime ) )
			{
				// 080519 LUJ, 체크 시간을 지금부터 maxCheckTick동안으로 한다
				checkCoolTime.mCheckedTick	= gCurTime + maxCheckTick;
				checkCoolTime.mFailureCount	= 0;
			}

			const DWORD maxCheckCount = 10;

			// 080516 LUJ, 쿨타임 실패가 허용 회수 이하이면, 처리하지 않는다
			if( maxCheckCount > ++checkCoolTime.mFailureCount )
			{
				return;
			}

			// 080516 LUJ, 일정 회수이상 실패한 경우 접속을 종료시킨다
			{
				MSG_DWORD message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_USERCONN;
				message.Protocol	= MP_USERCONN_GAMEIN_NACK;
				message.dwData		= player.GetID();
				
				g_Network.Broadcast2AgentServer( (char*)&message, sizeof( message ) );

				// 100812 NYJ - 강제종료 콘솔로그를 남기자.
				g_Console.LOG(4, "Force KickOut!! (CoolTime Check Failed.) : ID: %d, NAME: %s",  player.GetID(), player.GetObjectName() );
			}
		}
	}
	Punish;

	// 080514 LUJ, 애니메이션이 표시되는 순간에는 스킬을 사용할 수 없다
	const SkillAnimTimeMap::const_iterator itAnim = mSkillAnimTimeMap.find( skill.Index );
	if(mSkillAnimTimeMap.end() != itAnim &&
		itAnim->second > gCurTime)
	{
		// 080516 LUJ, 쿨타임 체크가 일정 기준 이상 실패했을 때 벌칙을 부여한다
		Punish( *this, mCheckCoolTime );
		// 080519 LUJ, 쿨타임 체크 실패 때도 사용 가능하게 반환한다. 일정 회수 이상 실패 시 벌칙을 부여하기 때문
		return FALSE;
	}

	const SkillCoolTimeMap::const_iterator it = mSkillCoolTimeMap.find( skill.Index );

	if(mSkillCoolTimeMap.end() == it)
	{
		return FALSE;
	}

	const DWORD endTime		= it->second;
	const BOOL	isCoolTime	=  endTime > gCurTime;

	if( isCoolTime )
	{
		// 080516 LUJ, 쿨타임 체크가 일정 기준 이상 실패했을 때 벌칙을 부여한다
		Punish( *this, mCheckCoolTime );
	}
	
	// 080519 LUJ, 쿨타임 체크 실패 때도 사용 가능하게 반환한다. 일정 회수 이상 실패 시 벌칙을 부여하기 때문
	return FALSE;
}

// 080511 LUJ, 스킬 쿨타임을 추가한다
// 080514 LUJ, 스킬 애니메이션 종료 시간 설정
// 080605 LUJ, 스킬 종류에 따라 애니메이션 시간을 변경한다
void CPlayer::SetCoolTime( const ACTIVE_SKILL_INFO& skill )
{
	// 080605 LUJ, 애니메이션 타임을 스킬 종류에 따라 가변시키기 위해 변수 정의
	float animationTime = float( skill.AnimationTime );

	// 080605 LUJ, 스킬 종류에 따라 애니메이션 타임을 적절히 가감한다.
	{
		const Status* ratePassiveStatus = GetRatePassiveStatus();
		const Status* rateBuffStatus	= GetRateBuffStatus();

		if( rateBuffStatus &&
			ratePassiveStatus )
		{
			if( ( skill.Index / 100000 ) % 10 )
			{
				switch( skill.Unit )
				{
				case UNITKIND_MAGIC_ATTCK:
					{
						animationTime = animationTime * ( 1.0f - ( rateBuffStatus->MagicSkillSpeedRate + ratePassiveStatus->MagicSkillSpeedRate ) / 100.0f );
						break;
					}
				case UNITKIND_PHYSIC_ATTCK:
					{
						animationTime = animationTime * ( 1.0f - ( rateBuffStatus->PhysicSkillSpeedRate + ratePassiveStatus->PhysicSkillSpeedRate ) / 100.0f );
						break;
					}
				}
			}
			else
			{
				animationTime = animationTime * ( 1.0f - ( rateBuffStatus->NormalSpeedRate + ratePassiveStatus->NormalSpeedRate ) / 100.0f );
			}
		}
	}
	
	// 080514 LUJ, 애니메이션이 끝나는 시간을 설정한다. 네트워크 지연을 감안하여 0.1초 오차는 허용한다
	// 080520 LUJ, 테스트 결과로 0.1->0.3초로 허용 시간 연장
	// 080605 LUJ, 최소 0인 값만 허용한다. animationTime이 실수로 변경되어 오버플로 여지가 있기 때문
	mSkillAnimTimeMap[ skill.Index ] = DWORD( max( 0, animationTime ) ) + gCurTime - 300;
	mSkillCoolTimeMap[ skill.Index ] = skill.CoolTime + gCurTime - 300;	
}

void CPlayer::ResetCoolTime( const ACTIVE_SKILL_INFO& skill )
{
	mSkillAnimTimeMap[skill.Index] = 0;
	mSkillCoolTimeMap[skill.Index] = 0;
}

BOOL CPlayer::IsCanCancelSkill()
{
	// 100618 ShinJS 일정시간내에 특정회수(현재3회)이상 요청시 취소요청에 제한을 준다.
	if( m_dwSkillCancelLastTime > gCurTime )
	{
		if( ++m_dwSkillCancelCount >= eSkillCancelLimit_Count )
		{
			m_dwSkillCancelLastTime = gCurTime + eSkillCancelLimit_CheckTime;
			return FALSE;
		}

		return TRUE;
	}
	
	m_dwSkillCancelCount = 0;
	m_dwSkillCancelLastTime = gCurTime + eSkillCancelLimit_CheckTime;

	return TRUE;
}

const DWORD CPlayer::GetSkillCancelDelay() const
{
	return eSkillCancelLimit_CheckTime;
}

// 100621 ShinJS 현재 캐스팅중인 스킬 취소
void CPlayer::CancelCurrentCastingSkill( BOOL bUseSkillCancelRate )
{
	cActiveSkillObject* const activeSkillObject = ( cActiveSkillObject* )SKILLMGR->GetSkillObject( mCurrentSkillID );

	// 090109 LUJ, 액티브 스킬만 취소될 수 있다
	// 090109 LUJ, 캐스팅 중에만 취소될 수 있도록 체크한다
	if( ! activeSkillObject || 
		cSkillObject::TypeActive != activeSkillObject->GetType() ||
		! activeSkillObject->IsCasting() )
	{
		return;
	}

	if( bUseSkillCancelRate && 
		activeSkillObject->GetInfo().Cancel <= (rand() % 100) ) 
	{
		return;
	}

	MSG_DWORD message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_SKILL;
	message.Protocol	= MP_SKILL_CANCEL_NOTIFY;
	message.dwObjectID	= GetID();
	message.dwData		= mCurrentSkillID;
	PACKEDDATA_OBJ->QuickSend( this, &message, sizeof( MSG_DWORD ) );

	cSkillObject* const skillObject = SKILLMGR->GetSkillObject( mCurrentSkillID );
	if( skillObject )
	{
		skillObject->SetEndState();
		skillObject->EndState();
	}
}

eArmorType CPlayer::GetArmorType(EWEARED_ITEM wearType) const
{
	const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo( GetWearedItemIdx( wearType ) );

	return itemInfo ? eArmorType( itemInfo->ArmorType ) : eArmorType_None;
}

void CPlayer::AddSpecialSkill(const cBuffSkillInfo* buffSkillInfo)
{
	// 090204 LUJ, 처음에는 항상 적용한다. 프로세스 때 검사하면서 조건에 맞지 않을 경우 해제한다
	buffSkillInfo->AddBuffStatus( this );

	SpecialSkillData specialSkillData = { 0 };
	specialSkillData.mBuffSkillInfo	= buffSkillInfo;
	specialSkillData.mIsTurnOn = TRUE;
	mSpecialSkillList.push_back( specialSkillData );
}

void CPlayer::RemoveSpecialSkill(const cBuffSkillInfo* buffSkillInfo)
{
	SpecialSkillList::iterator it = mSpecialSkillList.end();

	for(; mSpecialSkillList.end() != it; ++it )
	{
		const SpecialSkillData& specialSkillData = *it;

		if( specialSkillData.mBuffSkillInfo == buffSkillInfo )
		{
			break;
		}
	}

	if(mSpecialSkillList.end() == it)
	{
		return;
	}

	const SpecialSkillData& specialSkillData = *it;

	// 090204 LUJ, 조건에 따라 미적용 상태일 수 있으므로 검사 후 취소해야 한다
	if( specialSkillData.mIsTurnOn )
	{
		buffSkillInfo->RemoveBuffStatus( this );
	}

	mSpecialSkillList.erase(it);
}

// 090204 LUJ, 특수 스킬을 프로세스 타임에 체크한다
void CPlayer::ProcSpecialSkill()
{
	if(true == mSpecialSkillList.empty())
	{
		return;
	}

	// 090204 LUJ, 효율성을 위해 컨테이너 맨 앞의 스킬을 검사한 후 맨 뒤로 돌린다
	SpecialSkillData specialSkillData = mSpecialSkillList.front();
	mSpecialSkillList.pop_front();

	const cBuffSkillInfo* const buffSkillInfo = specialSkillData.mBuffSkillInfo;
	const BOOL validCondition = IsEnable(buffSkillInfo->GetInfo());
	const BOOL isTurnOn = ( ! specialSkillData.mIsTurnOn && validCondition );
	const BOOL isTurnOff = ( specialSkillData.mIsTurnOn && ! validCondition );

	if( isTurnOn )
	{
		buffSkillInfo->AddBuffStatus( this );
		specialSkillData.mIsTurnOn = TRUE;
	}
	else if( isTurnOff )
	{
		buffSkillInfo->RemoveBuffStatus( this );
		specialSkillData.mIsTurnOn = FALSE;
	}

	// 090204 LUJ, 다음 검사를 위해 추가한다
	mSpecialSkillList.push_back(
		specialSkillData);
}

void CPlayer::SetMasterRecipe(POSTYPE pos, DWORD dwRecipe, DWORD dwRemainTime)
{
	if(pos<0 || MAX_RECIPE_LV4_LIST<=pos)	return;

	m_MasterRecipe[pos].dwRecipeIdx = dwRecipe;
	m_MasterRecipe[pos].dwRemainTime = dwRemainTime;
}

stRecipeLv4Info* CPlayer::GetMasterRecipe(POSTYPE pos)
{
	if(pos<0 || MAX_RECIPE_LV4_LIST<=pos)	return NULL;

	return &m_MasterRecipe[pos];
}

int CPlayer::CanAddRecipe(DWORD dwRecipe)
{
	int i, nEmptySlot = -1;
	for(i=0; i<MAX_RECIPE_LV4_LIST; i++)
	{
		if(nEmptySlot<0 && 0==m_MasterRecipe[i].dwRecipeIdx)
			nEmptySlot = i;

		if(m_MasterRecipe[i].dwRecipeIdx == dwRecipe)
			return -2;
	}

	return nEmptySlot;
}

void CPlayer::ProcessRecipeTimeCheck(DWORD dwElapsedTime)
{
	DWORD dwElapsedMili = dwElapsedTime;

	for(POSTYPE i =0; i<MAX_RECIPE_LV4_LIST; i++)
	{
		if(0!=m_MasterRecipe[i].dwRecipeIdx && 0!=m_MasterRecipe[i].dwRemainTime)
		{
			if(dwElapsedMili > m_MasterRecipe[i].dwRemainTime)
			{
				// 삭제
				DWORD dwRecipeIdx = m_MasterRecipe[i].dwRecipeIdx;
				SetMasterRecipe(i, 0, 0);
				Cooking_Recipe_Update(GetID(), eCOOKRECIPE_DEL, dwRecipeIdx, i, 0);
				CookRecipe_Log(GetID(), dwRecipeIdx, i, eCOOKRECIPE_DEL, 0);

				MSG_DWORD4 msg;
				msg.Category	= MP_COOK;
				msg.Protocol	= MP_COOK_UPDATERECIPE;
				msg.dwObjectID	= GetID();
				msg.dwData1		= eCOOKRECIPE_DEL;
				msg.dwData2		= dwRecipeIdx;
				msg.dwData3		= i;
				msg.dwData4		= 0;
				SendMsg(&msg, sizeof(msg));
			}
			else
			{
				// 갱신
				DWORD dwRemainTime = m_MasterRecipe[i].dwRemainTime - dwElapsedMili;
				SetMasterRecipe(i, m_MasterRecipe[i].dwRecipeIdx, dwRemainTime);
				Cooking_Recipe_Update(GetID(), eCOOKRECIPE_UPDATE, m_MasterRecipe[i].dwRecipeIdx, i, dwRemainTime);

				MSG_DWORD4 msg;
				msg.Category	= MP_COOK;
				msg.Protocol	= MP_COOK_UPDATERECIPE;
				msg.dwObjectID	= GetID();
				msg.dwData1		= eCOOKRECIPE_UPDATE;
				msg.dwData2		= m_MasterRecipe[i].dwRecipeIdx;
				msg.dwData3		= i;
				msg.dwData4		= dwRemainTime;
				SendMsg(&msg, sizeof(msg));
			}
		}
	}
}

void CPlayer::ProceedToTrigger()
{
	if(gCurTime < mNextCheckedTick)
	{
		return;
	}

	// 던전옵저버는 메시지를 발생시키지 않는다.
	if(m_bDungeonObserver)
		return;

	// 091116 LUJ, 주기적으로 발송하는 메시지 간격을 늘림(0.5 -> 1.0초)
	const DWORD stepTick = 1000;
	mNextCheckedTick = gCurTime + stepTick;
	// 091116 LUJ, 채널에 해당하는 메시지를 할당받도록 한다
	Trigger::CMessage* const message = TRIGGERMGR->AllocateMessage(GetGridID());
	message->AddValue(Trigger::eProperty_ObjectIndex, GetID());
	message->AddValue(Trigger::eProperty_ObjectKind, GetObjectKind());
	message->AddValue(Trigger::eProperty_Event, Trigger::eEvent_CheckSelf);
}

float CPlayer::GetBonusRange() const
{	
	const float value = mPassiveStatus.Range + mBuffStatus.Range;
	const float percent = mRatePassiveStatus.Range + mRateBuffStatus.Range;

	return value * (1.0f + percent);
}

BOOL CPlayer::IsNoEquip(eArmorType armorType, eWeaponType weaponType, eWeaponAnimationType weaponAnimationType)
{
	switch(armorType)
	{
	case eArmorType_Robe:
	case eArmorType_Leather:
	case eArmorType_Metal:
		{
			if(GetArmorType(eWearedItem_Dress) != armorType)
			{
				return TRUE;
			}

			break;
		}
	case eArmorType_Shield:
		{
			if(GetArmorType(eWearedItem_Shield) != armorType)
			{
				return TRUE;
			}

			break;
		}
	}

	if(eWeaponType_None != weaponType)
	{
		if(GetWeaponEquipType() != weaponType)
		{
			return TRUE;
		}
	}

	switch( weaponAnimationType )
	{
	case eWeaponAnimationType_None:
		break;
	case eWeaponAnimationType_OneHand:
		{
			switch( GetWeaponAniType() )
			{
			case eWeaponAnimationType_OneHand:
			case eWeaponAnimationType_Dagger:
			case eWeaponAnimationType_TwoBlade:
				break;
			default:
				return TRUE;
			}
		}
		break;

	case eWeaponAnimationType_TwoHand:
		{
			switch( GetWeaponAniType() )
			{
			case eWeaponAnimationType_TwoHand:
			case eWeaponAnimationType_Staff:
			case eWeaponAnimationType_Bow:
			case eWeaponAnimationType_Gun:
			case eWeaponAnimationType_Spear:
				break;
			default:
				return TRUE;
			}
		}
		break;

	default:
		{
			if( GetWeaponAniType() != weaponAnimationType )
				return TRUE;
		}
		break;
	}

	return FALSE;
}

BOOL CPlayer::IsEnable(const BUFF_SKILL_INFO& info)
{
	if(IsNoEquip(
		info.mArmorType,
		info.mWeaponType,
		info.mWeaponAnimationType))
	{
		return FALSE;
	}

	float checkValue = 0;
	const BUFF_SKILL_INFO::Condition& condition = info.mCondition;

	switch(condition.mType)
	{
	case BUFF_SKILL_INFO::Condition::TypeLifePercent:
		{
			checkValue = float(GetLifePercent());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLife:
		{
			checkValue = float(GetLife());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLifeMax:
		{
			checkValue = float(GetMaxLife());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLifeRecovery:
		{
			checkValue = float(GetLifeRecover());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaPercent:
		{
			checkValue = float(GetManaPercent());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMana:
		{
			checkValue = float(GetMana());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaMax:
		{
			checkValue = float(GetMaxMana());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaRecovery:
		{
			checkValue = float(GetManaRecover());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeStrength:
		{
			checkValue = float(GetStrength());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeIntelligence:
		{
			checkValue = float(GetIntelligence());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeDexterity:
		{
			checkValue = float(GetDexterity());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeWisdom:
		{
			checkValue = float(GetWisdom());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeVitality:
		{
			checkValue = float(GetVitality());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypePhysicalAttack:
		{
			checkValue = float(GetPhysicAttackMax());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMagicalAttack:
		{
			checkValue = float(GetMagicAttackMax());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypePhysicalDefense:
		{
			checkValue = float(GetPhysicDefense());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMagicalDefense:
		{
			checkValue = float(GetMagicDefense());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeCriticalRate:
		{
			checkValue = float(GetCriticalRate());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeCriticalDamage:
		{
			checkValue = float(GetCriticalDamageRate());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeAccuracy:
		{
			checkValue = float(GetAccuracy());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeEvade:
		{
			checkValue = float(GetAvoid());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMoveSpeed:
		{
			checkValue = float(GetMoveSpeed());
			break;
		}
	}

	BOOL isEnable = TRUE;

	switch(condition.mOperation)
	{
	case BUFF_SKILL_INFO::Condition::OperationMore:
		{
			isEnable = (checkValue > condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationMoreEqual:
		{
			isEnable = (checkValue >= condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationLess:
		{
			isEnable = (checkValue < condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationLessEqual:
		{
			isEnable = (checkValue <= condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationEqual:
		{
			isEnable = (checkValue == condition.mValue);
			break;
		}
	}

	return isEnable;
}

cSkillTree& CPlayer::GetSkillTree()
{
	return *m_SkillTree;
}

void CPlayer::SetPartyIdx( DWORD PartyIDx )
{
	m_HeroInfo.PartyID = PartyIDx; 

	if(m_HeroInfo.PartyID)
	{
		return;
	}

	// 파티 상태일 경우 특정 스킬을 삭제한다
	{
		cPtrList templist;
		m_BuffSkillList.SetPositionHead();

		while(cBuffSkillObject* pSObj = (cBuffSkillObject*)m_BuffSkillList.GetData())
		{
			if(GetPartyIdx() != 0 && pSObj->GetInfo().Party == 1 )
			{
				pSObj->SetEndState();
				pSObj->EndState();
				templist.AddTail( pSObj );
			}
		}

		PTRLISTPOS pos = templist.GetHeadPosition();
		while( pos )
		{
			cSkillObject* const pSObj = ( cSkillObject* )templist.GetNext( pos );
			m_BuffSkillList.Remove( pSObj->GetSkillIdx() );
		}
		templist.RemoveAll();
	}
}

CObject* CPlayer::GetTObject() const
{
	if(mAggroObjectContainer.empty())
	{
		return 0;
	}

	ObjectIndexContainer::const_iterator iterator = mAggroObjectContainer.begin();

	std::advance(
		iterator,
		rand() % mAggroObjectContainer.size());

	return g_pUserTable->FindUser(
		*iterator);
}

void CPlayer::AddToAggroed(DWORD objectIndex)
{
	// 버프로 데미지를 입힐 경우 공격자가 자기 자신이 된다. 자신을 어그로 컨테이너에 저장할 필요는 없다
	if(GetID() == objectIndex)
	{
		return;
	}

	CObject* const pObject = g_pUserTable->FindUser( objectIndex );
	if( !pObject )
		return;

	if( pObject->GetObjectKind() & eObjectKind_Monster )
	{
		// 100616 ShinJS --- 상대에게 자신을 등록하도록 하여 Die/Release시 어그로를 제거할수 있도록 한다.
		((CMonster*)pObject)->AddToAggroed( GetID() );
	}

	mAggroObjectContainer.insert(objectIndex);
}

void CPlayer::LogOnRelease()
{
	if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
	{
		TCHAR text[MAX_PATH] = {0};
		_sntprintf(
			text,
			_countof(text),
			"map:%d",
			g_pServerSystem->GetMapNum());
		LogItemMoney(
			GetID(),
			GetObjectName(),
			0,
			text,
			eLog_DungeonEnd,
			GetMoney(),
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0);
		return;
	}
	else if(g_csDateManager.IsChallengeZone(g_pServerSystem->GetMapNum()))
	{
		TCHAR text[MAX_PATH] = {0};
		_sntprintf(
			text,
			_countof(text),
			"map:%d",
			g_pServerSystem->GetMapNum());
		LogItemMoney(
			GetID(),
			GetObjectName(),
			0,
			text,
			eLog_DateMatchEnd,
			GetMoney(),
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0);
		return;
	}

	const LimitDungeonScript& script = g_CGameResourceManager.GetLimitDungeonScript(
		g_pServerSystem->GetMapNum(),
		GetChannelID());

	if(script.mMapType == g_pServerSystem->GetMapNum())
	{
		TCHAR text[MAX_PATH] = {0};
		_sntprintf(
			text,
			_countof(text),
			"%map:%d(%d)",
			g_pServerSystem->GetMapNum(),
			GetChannelID());
		LogItemMoney(
			GetID(),
			GetObjectName(),
			0,
			text,
			eLog_LimitDungeonEnd,
			GetMoney(),
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0);
	}
}

// 100624 ONS HP업데이트관련 처리 추가
void CPlayer::AddLifeRecoverTime( const YYRECOVER_TIME& recoverTime )
{
	m_YYLifeRecoverTimeQueue.push( recoverTime );
}

void CPlayer::UpdateLife()
{
	if( 0 != m_LifeRecoverDirectlyAmount )
	{
		AddLife( m_LifeRecoverDirectlyAmount, NULL );
		m_LifeRecoverDirectlyAmount = 0;
		return;
	}

	if( 0 == m_YYLifeRecoverTime.count )
	{
		if( FALSE == m_YYLifeRecoverTimeQueue.empty() )
		{
			// 하나씩 큐에서 꺼내서 업데이트시키도록 한다.
			m_YYLifeRecoverTime = m_YYLifeRecoverTimeQueue.front();
			m_YYLifeRecoverTimeQueue.pop();
		}
	}

	if( m_YYLifeRecoverTime.count > 0 )
	{
		if( m_YYLifeRecoverTime.lastCheckTime < gCurTime )
		{
			// HP가 꽉차면 큐에 저장된 데이터를 모두 삭제한다.
			if( GetMaxLife() <= GetLife() )
			{
				while( !m_YYLifeRecoverTimeQueue.empty() )
				{
					m_YYLifeRecoverTimeQueue.pop();
				}
				m_YYLifeRecoverTime.count = 0;
				return;
			}

			// HP업데이트한다.
			m_YYLifeRecoverTime.lastCheckTime = gCurTime + m_YYLifeRecoverTime.recoverDelayTime;
			AddLife( m_YYLifeRecoverTime.recoverUnitAmout, NULL );
			--m_YYLifeRecoverTime.count;
		}
	}
}

// 100624 ONS MP업데이트관련 처리 추가
void CPlayer::AddManaRecoverTime( const YYRECOVER_TIME& recoverTime )
{
	m_YYManaRecoverTimeQueue.push( recoverTime );
}

void CPlayer::UpdateMana()
{
	if( 0 != m_ManaRecoverDirectlyAmount )
	{
		AddMana( m_ManaRecoverDirectlyAmount, NULL );
		m_ManaRecoverDirectlyAmount = 0;
		return;
	}

	if( 0 == m_YYManaRecoverTime.count )
	{
		if( FALSE == m_YYManaRecoverTimeQueue.empty() )
		{
			m_YYManaRecoverTime = m_YYManaRecoverTimeQueue.front();
			m_YYManaRecoverTimeQueue.pop();
		}
	}

	if( m_YYManaRecoverTime.count > 0 )
	{
		if( m_YYManaRecoverTime.lastCheckTime < gCurTime )
		{
			if( GetMaxMana() <= GetMana() )
			{
				while( !m_YYManaRecoverTimeQueue.empty() )
				{
					m_YYManaRecoverTimeQueue.pop();
				}
				m_YYManaRecoverTime.count = 0;
				return;
			}

			m_YYManaRecoverTime.lastCheckTime = gCurTime + m_YYManaRecoverTime.recoverDelayTime;
			AddMana( m_YYManaRecoverTime.recoverUnitAmout, NULL );
			--m_YYManaRecoverTime.count;
		}
	}
}

// 100611 ONS 채팅금지상태 여부 판단.
BOOL CPlayer::IsForbidChat() const
{
	__time64_t time = 0;
	_time64( &time );

	if( time > ForbidChatTime || 0 == ForbidChatTime )
		return FALSE;

	return TRUE;
}
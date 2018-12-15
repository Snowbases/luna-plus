#include "stdafx.h"
#include "HousingMgr.h"
#include "UserTable.h"
#include "Player.h"
#include "Npc.h"
#include "MapDBMsgParser.h"
#include "MHFile.h"
#include "AISystem.h"
#include "../[CC]Header/GameResourceManager.h"
#include "ItemManager.h"
#include "Network.h"
#include "PackedData.h"
#include "LootingManager.h"
#include "BattleSystem_Server.h"
#include "CharMove.h"
#include "objectstatemanager.h"
#include "../[cc]skill/Server/Info/ActiveSkillInfo.h"
#include "../[cc]skill/Server/Object/ActiveSkillObject.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "ChannelSystem.h"
#include "StorageManager.h"

GLOBALTON(HousingMgr)

HousingMgr::HousingMgr(void)
{
	m_HousePool = NULL;
	m_FurniturePool = NULL;
	m_ReservationPool = NULL;
	m_VotePool = NULL;
	m_RiderPool = NULL;
}

HousingMgr::~HousingMgr(void)
{
	stHouse* pHouse = NULL;
	m_HouseList.SetPositionHead();
	while((pHouse = m_HouseList.GetData() )!= NULL)
	{
		m_HousePool->Free(pHouse);
	}
	m_HouseList.RemoveAll();
	m_LoadingHouseList.RemoveAll();

	stHouseVoteInfo* pVote = NULL;
	m_VoteList.SetPositionHead();
	while((pVote = m_VoteList.GetData())!= NULL)
	{
		m_VotePool->Free(pVote);
	}
	m_VoteList.RemoveAll();

	stHouseRiderInfo* pRider = NULL;
	m_RiderList.SetPositionHead();
	while((pRider = m_RiderList.GetData())!= NULL)
	{
		m_RiderPool->Free(pRider);
	}
	m_RiderList.RemoveAll();

	stHouseRankNPC* pRankNpc = NULL;
	m_RankNpcList.SetPositionHead();
	while((pRankNpc = m_RankNpcList.GetData())!= NULL)
	{
		SAFE_DELETE(pRankNpc);
	}
	m_RankNpcList.RemoveAll();

	stHouseReservation* pData;
	m_ReservationList.SetPositionHead();
	while((pData = m_ReservationList.GetData())!= NULL)
	{
		m_ReservationPool->Free(pData);
	}
	m_ReservationList.RemoveAll();

	SAFE_DELETE(m_HousePool);
	SAFE_DELETE(m_FurniturePool);
	SAFE_DELETE(m_ReservationPool);
	SAFE_DELETE(m_VotePool);
	SAFE_DELETE(m_RiderPool);

	m_FurnitureIDGenerator.Release();
}

void HousingMgr::Init()
{
	LoadHousing_Setting();

	DWORD dwChannelCount = CHANNELSYSTEM->GetChannelCount();
	m_RankNpcList.Initialize(dwChannelCount);

	if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
	{
		// 091026 LUJ, CPlayer::SetAddMsg()에서 비교문을 줄이고자 하우스 맵이 아니어도 초기화시킨다
		//			이 코드를 넣지 않으면 리스트가 초기화되지 않아 오류가 발생한다
		m_RiderList.Initialize(1);
		return;
	}

	const DWORD dwMaxFurnitureNum = MAX_HOUSE_NUM * MAX_HOUSING_CATEGORY_NUM * MAX_HOUSING_SLOT_NUM;
	m_FurnitureIDGenerator.Init(dwMaxFurnitureNum, HOUSING_FURNITURE_STARTINDEX);

	GetLocalTime(&m_LastCheckTime);

	m_HousePool = new CPool<stHouse>;
	m_HousePool->Init(50, 50, "HousePool");

	m_FurniturePool = new CPool<stFurniture>;
	m_FurniturePool->Init(100, 100, "FurniturePool");

	m_ReservationPool = new CPool<stHouseReservation>;
	m_ReservationPool->Init(100, 100, "ReservationPool");

	m_VotePool = new CPool<stHouseVoteInfo>;
	m_VotePool->Init(100, 100, "m_VotePool");

	m_RiderPool = new CPool<stHouseRiderInfo>;
	m_RiderPool->Init(100, 100, "m_RiderPool");

	m_HouseList.Initialize(MAX_HOUSE_NUM);
	m_VoteList.Initialize(1000);
	m_RiderList.Initialize(1000);

	m_LoadingHouseList.Initialize(MAX_HOUSE_NUM);
	m_ReservationList.Initialize(MAX_HOUSE_NUM);
}

void HousingMgr::Process()
{
	static BOOL bFirst = TRUE;
	if(60000<gCurTime && bFirst)
	{
		// DB에 하우스랭크 요청
		m_bLoadRank = FALSE;
		m_dwHouseRankSetTime = 0;
		HouseRankLoad();

		bFirst = FALSE;
	}

	if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
	{
		if(m_dwHouseRankSetTime)
		{
			if(m_dwHouseRankSetTime+60000 < gCurTime)
			{
				RecallRankNPC();
				m_dwHouseRankSetTime = 0;
			}
		}

		return;
	}

	static DWORD dwLastCheckTick = gCurTime;

	// 1분에 한번씩만 확인
	if(gCurTime < dwLastCheckTick+60000)
		return;

	if(!m_bLoadRank)
		return;

	BOOL bHouseUpdate = FALSE;
	SYSTEMTIME	st;
	GetLocalTime(&st);
	
	if(m_LastCheckTime.wDay != st.wDay)
	{
		// 날자가 변경되었으면 서버에 업데이트
		bHouseUpdate = TRUE;

		memcpy(&m_LastCheckTime, &st, sizeof(SYSTEMTIME));

		// 추천자목록 리셋
		stHouseVoteInfo* pVote = NULL;
		m_VoteList.SetPositionHead();
		while((pVote = m_VoteList.GetData())!= NULL)
		{
			m_VotePool->Free(pVote);
		}
		m_VoteList.RemoveAll();
	}

	// 대기자 목록 5분경과시 제거
	stHouseReservation* pReservation = NULL;

	m_ReservationList.SetPositionHead();
	while((pReservation = m_ReservationList.GetData())!= NULL)
	{
		if(pReservation->dwRegistTime+(60000*5) < gCurTime)
		{
			DWORD dwEmptyHouseChannel = pReservation->dwChannelID;

			m_ReservationList.Remove(pReservation->dwCharIndex);
			m_ReservationPool->Free(pReservation);

			stHouse* pEmptyHouse = GetHouseByChannelID(dwEmptyHouseChannel);
			if(pEmptyHouse && pEmptyHouse->dwJoinPlayerNum==0)
			{
				// 예약해놓고 5분간 입장이 없는 빈집은 삭제
				DeleteHouse(pEmptyHouse->HouseInfo.dwOwnerUserIndex);
			}
		}
	}


	// 시간경과 아이템 지우기
	stHouse* pHouse;
	stFurniture* pFurniture = NULL;
	
	static int nRemoveItemNum = 0;
	static stFurniture* pRemoveList[MAX_HOUSING_CATEGORY_NUM * MAX_HOUSING_SLOT_NUM] = {0,};

	m_HouseList.SetPositionHead();
	while((pHouse = m_HouseList.GetData())!= NULL)
	{
		nRemoveItemNum = 0;
		ZeroMemory(pRemoveList, sizeof(stFurniture*)*MAX_HOUSING_CATEGORY_NUM*MAX_HOUSING_SLOT_NUM);

		pHouse->pFurnitureList.SetPositionHead();
		while((pFurniture = pHouse->pFurnitureList.GetData())!= NULL)
		{
			if(!pFurniture->bNotDelete &&
				(pFurniture->wState==eHOUSEFURNITURE_STATE_INSTALL ||
				pFurniture->wState==eHOUSEFURNITURE_STATE_UNINSTALL))
			{
				if(pFurniture->dwRemainTime > 60)
				{
					pFurniture->dwRemainTime -= 60;
				}
				else
				{
					pRemoveList[nRemoveItemNum] = pFurniture;
					nRemoveItemNum++;
				}
			}
		}

		if(nRemoveItemNum > 0)
		{
			g_pUserTable->SetPositionUserHead();
			CObject* pObject = NULL ;
			while( (pObject = g_pUserTable->GetUserData() )!= NULL)
			{
				if( pObject->GetObjectKind() != eObjectKind_Player ) continue;
				
				CPlayer* pReceiver = (CPlayer*)pObject;
				if( pReceiver->GetChannelID() == pHouse->dwChannelID )
				{
					int i;
					for(i=0; i<nRemoveItemNum; i++)
					{
						DWORD dwRemoveObjectIndex = pRemoveList[i]->dwObjectIndex;
						// 시간경과 지우자~
						HouseFurnitureUpdate(0, eHOUSEFURNITURE_DEL, pRemoveList[i]->dwOwnerUserIndex, pRemoveList[i]->dwFurnitureIndex, pRemoveList[i]->dwLinkItemIndex, 0, 
						0, 0, 0, 0,
						pRemoveList[i]->wSlot, eHOUSEFURNITURE_STATE_UNINSTALL, 0);

						// 내집창고 가구삭제 로그
						// 시간경과로 삭제되는 가구는 CHARNAME필드에 "*HS_EndTime"로 기록한다.
						Log_Housing(0, "*HS_EndTime", eHouseLog_EndTime, 0, 0, 0,
							pRemoveList[i]->dwFurnitureIndex, pRemoveList[i]->wSlot, pHouse->HouseInfo.szHouseName, pHouse->dwDecoPoint, pHouse->HouseInfo.dwHousePoint);
						
						RemoveFurniture(pRemoveList[i]->dwObjectIndex, pHouse);

						// 같은 채널에 있는 유저에게통보
						MSG_DWORD3 msg;
						msg.Category = MP_HOUSE;
						msg.Protocol = MP_HOUSE_UNINSTALL_ACK;
						msg.dwObjectID = pHouse->HouseInfo.dwOwnerUserIndex;
						msg.dwData1 = dwRemoveObjectIndex;
						msg.dwData2 = 1;	// 가구제거						
						msg.dwData3 = pHouse->dwDecoPoint;
						pReceiver->SendMsg( &msg, sizeof(msg) );
					}
				}
			}
		}

		if(bHouseUpdate)
		{
			pHouse->HouseInfo.dwDailyVisitCount = 0;
			pHouse->HouseInfo.dwDecoUsePoint = HOUSINGMGR->GetDecoUsePoint();
			HouseUpdate(&pHouse->HouseInfo);

			// 집주인이 있으면 갱신통보
			CPlayer* pOwnerPlayer = (CPlayer*)g_pUserTable->FindUser(pHouse->dwOwnerCharIndex);
			if(pOwnerPlayer)
			{
				// 하우스정보 & 보너스 목록
				MSG_HOUSE_HOUSEINFO msgHouseInfo;
				msgHouseInfo.Category = MP_HOUSE;
				msgHouseInfo.Protocol = MP_HOUSE_INFO;
				msgHouseInfo.dwObjectID = pOwnerPlayer->GetID();
				msgHouseInfo.bInit = FALSE;
				msgHouseInfo.dwChannel = pHouse->dwChannelID;
				memcpy(&msgHouseInfo.HouseInfo, &pHouse->HouseInfo, sizeof(msgHouseInfo.HouseInfo));
				pOwnerPlayer->SendMsg( &msgHouseInfo, sizeof( msgHouseInfo ) );
			}
		}
	}


	// 매월 1일 순위계산
	WORD wCalcDay = (WORD)m_HousingSettingInfo.dwRankingDay;

	if(m_HouseRank.wCalcMonth != st.wMonth)
	{
		if(st.wDay == wCalcDay)
		{
			m_bLoadRank = FALSE;
			HouseRankUpdate();
		}
	}
	else if(m_HouseRank.wCalcMonth == st.wMonth)
	{
		if(m_HouseRank.wCalcDay!=wCalcDay &&
			st.wDay==wCalcDay)
		{
			m_bLoadRank = FALSE;
			HouseRankUpdate();
		}
	}

	dwLastCheckTick = gCurTime;
}

void HousingMgr::NetworkMsgParser(DWORD dwConnectionIndex, BYTE Protocol, void* pMsg, DWORD dwLength)
{
	CPlayer* pPlayer = NULL;

	switch(Protocol)
	{
	case MP_HOUSE_CREATE_SYN:
		{
			if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
			{
				MSG_HOUSE_CREATE* pmsg = (MSG_HOUSE_CREATE*)pMsg;

				pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
				if(pPlayer)
				{
					if(!pPlayer->GetResidentRegistInfo()->bIsValid)
					{
						// 주민등록 미신청
						MSG_DWORD msg;
						msg.Category = MP_HOUSE;
						msg.Protocol = MP_HOUSE_CREATE_NACK;
						msg.dwObjectID = pPlayer->GetID();
						msg.dwData = eHOUSEERR_NOTREGIST;
						g_Network.Send2Server(dwConnectionIndex, (char*)&msg, sizeof(msg));
						return;
					}

					MSG_HOUSE_CREATESRV msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_CREATE_FROM_SERVER_SYN;
					msg.dwObjectID = pmsg->dwObjectID;
					msg.dwPlayerID = pmsg->dwPlayerID;
					msg.dwPlayerMapNum = g_pServerSystem->GetMapNum();
					msg.dwConnectionIndex = dwConnectionIndex;
					SafeStrCpy(msg.szHouseName, pmsg->szHouseName, MAX_HOUSING_NAME_LENGTH+1);

					PACKEDDATA_OBJ->SendToMapServer(HOUSINGMAPNUM, &msg, sizeof(msg));
					return;
				}
			}
		}
		break;
	case MP_HOUSE_CREATE_FROM_SERVER_ACK:
		{
			if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
			{
				MSG_HOUSENAME* pmsg = (MSG_HOUSENAME*)pMsg;

				pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
				if(pPlayer)
				{
					pPlayer->SetHouseName(pmsg->Name);

					MSG_DWORD msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_CREATE_ACK;
					msg.dwObjectID = pmsg->dwObjectID;
					msg.dwData = pmsg->dwData;
					pPlayer->SendMsg( &msg, sizeof( msg ) );
					return;
				}
			}
		}
		break;
	case MP_HOUSE_CREATE_FROM_SERVER_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if(pPlayer)
			{
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_CREATE_NACK;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.dwData = pmsg->dwData;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}
		}
		break;
	case MP_HOUSE_CREATE_FROM_SERVER_SYN:
		{
			MSG_HOUSE_CREATESRV* pmsg = (MSG_HOUSE_CREATESRV*)pMsg;

			// DB에 하우스 생성요청
			char szCreateName[MAX_HOUSING_NAME_LENGTH] = {0,};
			SafeStrCpy(szCreateName, pmsg->szHouseName, MAX_HOUSING_NAME_LENGTH);
			HouseCreate(pmsg->dwConnectionIndex, pmsg->dwPlayerMapNum, pmsg->dwPlayerID, szCreateName);
		}
		break;
	case MP_HOUSE_ENTRANCE_SYN:
		{
			if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
			{
				//cKind(방문방식) dwValue1(ItemIndex or NPCIndex) dwValue2(ItemSlot)
				MSG_HOUSE_VISIT* pmsg = (MSG_HOUSE_VISIT*)pMsg;

				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
				// 탈것에 탑습중이면 실패
				if(pPlayer && (pPlayer->GetMountedVehicle() || pPlayer->GetSummonedVehicle()))
				{
					MSG_DWORD4 msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_ENTRANCE_NACK;
					msg.dwObjectID = pmsg->dwObjectID;
					msg.dwData1 = eHOUSEERR_ONVEHICLE;
					msg.dwData2 = pmsg->cKind;
					msg.dwData3 = pmsg->dwValue1;
					msg.dwData4 = pmsg->dwValue2;
					pPlayer->SendMsg( &msg, sizeof( msg ) );
					return;
				}

				MSG_HOUSE_VISITSRV msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ENTRANCE_FROM_SERVER_SYN;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.cKind = pmsg->cKind;
				msg.dwValue1 = pmsg->dwValue1;
				msg.dwValue2 = pmsg->dwValue2;
				msg.dwConnectionIndex = dwConnectionIndex;
				msg.dwMapNum = g_pServerSystem->GetMapNum();

				if(msg.cKind == eHouseVisitByLink)
				{
					if(0 == msg.dwValue2)
					{
						// 랭커 npc로 이동
						stHouseRankNPC* pRankNpc = m_RankNpcList.GetData(msg.dwValue1);
						if(pRankNpc)
						{
							msg.dwValue2 = pRankNpc->dwHouseUserIndex;
							SafeStrCpy(msg.Name, "HS_LINKVISIT", sizeof(msg.Name));
						}
					}
					else
					{
						// msg.dwValue2(=UserIndex)의 집으로 이동
						SafeStrCpy(msg.Name, "HS_LINKVISIT", sizeof(msg.Name));
					}
				}
				else if(msg.cKind == eHouseVisitByItemRandom)
				{
					SafeStrCpy(msg.Name, "HS_RANDOMVISIT", sizeof(pmsg->Name));
				}
				else
					SafeStrCpy(msg.Name, pmsg->Name, sizeof(msg.Name));

				PACKEDDATA_OBJ->SendToMapServer(HOUSINGMAPNUM, &msg, sizeof(msg));
				return;
			}
			
			
			MSG_HOUSE_VISIT* pmsg = (MSG_HOUSE_VISIT*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{	
				DWORD dwValue2 = pmsg->dwValue2;

				if(pmsg->cKind == eHouseVisitByLink)
				{
					if(0 == pmsg->dwValue2)
					{
						// 랭커 npc로 이동
						stHouseRankNPC* pRankNpc = m_RankNpcList.GetData(pmsg->dwValue1);
						if(pRankNpc)
						{
							pmsg->dwValue2 = pRankNpc->dwHouseUserIndex;
							SafeStrCpy(pmsg->Name, "HS_LINKVISIT", sizeof(pmsg->Name));
						}
					}
					else
					{
						// msg.dwValue2(=UserIndex)의 집으로 이동
						SafeStrCpy(pmsg->Name, "HS_LINKVISIT", sizeof(pmsg->Name));
					}
				}
				else if(pmsg->cKind == eHouseVisitByItemRandom)
				{
					SafeStrCpy(pmsg->Name, "HS_RANDOMVISIT", sizeof(pmsg->Name));
				}

				HouseEntrance(dwConnectionIndex, g_pServerSystem->GetMapNum(), pmsg->cKind, pmsg->dwValue1, dwValue2, pmsg->dwObjectID, pmsg->Name);
			}
		}
		break;
	case MP_HOUSE_ENTRANCE_ACK:
		{
			//dwData1(채널) dwData2(posX) dwData3(posZ), dwData4(VisitKind), dwData5(Value1), dwData6(Value2)
			MSG_DWORD6* pmsg = (MSG_DWORD6*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				if( pPlayer->IsPKMode() || LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) || pPlayer->GetAutoNoteIdx() )
				{
					MSG_DWORD msg;
					msg.Category	= MP_HOUSE;
					msg.Protocol	= MP_HOUSE_ENTRANCE_NACK;
					msg.dwObjectID  = pPlayer->GetID();
					msg.dwData		= eHOUSEERR_ERROR;
					pPlayer->SendMsg( &msg, sizeof( msg ) );
					return;
				}

				MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( HOUSINGMAPNUM );
				if( !ChangeInfo ) return;
				SaveMapChangePointUpdate( pPlayer->GetID(), ChangeInfo->Kind );

				// RaMa - 04.12.14
				pPlayer->UpdateLogoutToDB(FALSE);

				VECTOR3 vStartPos;
				vStartPos.x = (float)pmsg->dwData2;
				vStartPos.y = 0.0f;
				vStartPos.z = (float)pmsg->dwData3;

				pPlayer->SetMapMoveInfo( ChangeInfo->MoveMapNum, (DWORD)vStartPos.x, (DWORD)vStartPos.z);

				if(eHouseVisitByItem == pmsg->dwData4 ||
					eHouseVisitByItemRandom == pmsg->dwData4)
					ITEMMGR->UseItem(pPlayer, (WORD)pmsg->dwData6, pmsg->dwData5);

				const MAPTYPE	mapType			= HOUSINGMAPNUM;
				const WORD		channelIndex	= 0;
				const DWORD		playerIndex		= pPlayer->GetID();
				const DWORD		userIndex		= pPlayer->GetUserID();
				const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
				const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );

				g_pServerSystem->RemovePlayer( pPlayer->GetID() );

				// 081218 LUJ, 업데이트보다 맵 이동이 빨리 진행될 경우 업데이트되지 않은 정보가 오게된다.
				//			이를 막기 위해 프로시저 처리 완료 후 진행한다
				UpdatePlayerOnMapOut(
					playerIndex,
					userIndex,
					inventoryMoney,
					storageMoney,
					mapType,
					channelIndex );
			}
		}
		break;
	case MP_HOUSE_ENTRANCE_FROM_SERVER_NACK:
		{
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if(pPlayer)
			{
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ENTRANCE_NACK;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.dwData1 = pmsg->dwData1;
				msg.dwData2 = pmsg->dwData2;
				msg.dwData3 = pmsg->dwData3;
				msg.dwData4 = pmsg->dwData4;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}
		}
		break;
	case MP_HOUSE_ENTRANCE_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				pPlayer->SendMsg( (MSGBASE*)pMsg, dwLength );
			}
		}
		break;
	case MP_HOUSE_ENTRANCE_FROM_SERVER_SYN:
		{
			MSG_HOUSE_VISITSRV* pmsg = (MSG_HOUSE_VISITSRV*)pMsg;
			HouseEntrance(pmsg->dwConnectionIndex, pmsg->dwMapNum, pmsg->cKind, pmsg->dwValue1, pmsg->dwValue2, pmsg->dwObjectID, pmsg->Name);
		}
		break;
	case MP_HOUSE_INFO_SYN:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;

			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				if(! EntranceHouse(pPlayer->GetID()))
					ChangeMapFromHouse(pPlayer->GetID(), 0);
			}
		}
		break;
	case MP_HOUSE_RANKINFO_FORSERVER:
		{
			if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
			{
				MSG_HOUSE_RANK_UPDATE* pmsg = (MSG_HOUSE_RANK_UPDATE*)pMsg;
				SetHouseRank(&pmsg->HouseRank);
				SetLoadRank(TRUE);
				return;
			}
		}
		break;
	case MP_HOUSE_STORED_SYN:
		{
			//dwData1(채널) dwData2(아이템Index) dwData3(아이템Slot) dwData4(내집창고Slot)
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
			
			DWORD dwChannelID = pmsg->dwData1;
			DWORD dwItemIndex = pmsg->dwData2;
			WORD ItemSlot = (WORD)pmsg->dwData3;
			BYTE HouseSlot = (BYTE)pmsg->dwData4;

			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				stHouse* pHouse = GetHouseByChannelID(dwChannelID);
				if(pHouse &&
					pHouse->HouseInfo.dwOwnerUserIndex==pPlayer->GetUserID())
				{
					if(CanKeeping(pHouse, dwItemIndex, HouseSlot))
					{
						DWORD dwFurnitureIndex = GetFurnitureIndexFromItemIndex(dwItemIndex);
						if(dwFurnitureIndex)
						{
							stFunitureInfo* pInfo = GAMERESRCMNGR->GetFunitureInfo(dwFurnitureIndex);
							if(pInfo)
							{
								// 하우스에 추가
								stFurniture* pFurniture = m_FurniturePool->Alloc();
								if(pFurniture)
								{
									pFurniture->Clear();
									pFurniture->dwFurnitureIndex = dwFurnitureIndex;
									pFurniture->wSlot = HouseSlot;
									pFurniture->wState = eHOUSEFURNITURE_STATE_KEEP;
									pFurniture->dwLinkItemIndex = dwItemIndex;
									pFurniture->dwRemainTime = 0;
									AddNewFurniture(pFurniture, pHouse);

									// 아이템제거 & 로그
									ITEMBASE Item = *ITEMMGR->GetItemInfoAbsIn(pPlayer, ItemSlot);
									if(!ITEMMGR->DiscardItem(pPlayer, ItemSlot, dwItemIndex, 1))
									{
										// 인벤 아이템제거 로그
										LogItemMoney(pPlayer->GetID(), pPlayer->GetObjectName(), 0, "*HS_STORED",
											eLog_ItemHouseKeep, pPlayer->GetMoney(eItemTable_Inventory), 0, 0,
											Item.wIconIdx, Item.dwDBIdx,  Item.Position, 0,
											Item.Durability, pPlayer->GetPlayerExpPoint());

										// 내집창고 가구보관 로그
										Log_Housing(pPlayer->GetUserID(), pPlayer->GetObjectName(), eHouseLog_Keep, Item.dwDBIdx, dwItemIndex, 0,
											dwFurnitureIndex, HouseSlot, pHouse->HouseInfo.szHouseName, pHouse->dwDecoPoint, pHouse->HouseInfo.dwHousePoint);
									}

									// 아이템 제거통보
									MSG_ITEM_USE_ACK msgItemUse;
									msgItemUse.Category		= MP_ITEM;
									msgItemUse.Protocol		= MP_ITEM_USE_ACK;
									msgItemUse.dwObjectID	= pPlayer->GetID();
									msgItemUse.dwItemIdx	= dwItemIndex;
									msgItemUse.TargetPos	= ItemSlot;
									msgItemUse.eResult		= eItemUseSuccess;
									pPlayer->SendMsg( &msgItemUse, sizeof( msgItemUse ) );

									// 유저에게통보
									MSG_HOUSE_FURNITURE msg;
									msg.Category = MP_HOUSE;
									msg.Protocol = MP_HOUSE_STORED_ACK;
									msg.dwObjectID = pPlayer->GetID();
									msg.wState = eHOUSEFURNITURE_ADD;
									memcpy(&msg.Furniture, pFurniture, sizeof(msg.Furniture));
									pPlayer->SendMsg( &msg, sizeof( msg ) );								


									// DB보관
									HouseFurnitureUpdate(pPlayer->GetID(), eHOUSEFURNITURE_ADD, pPlayer->GetUserID(), dwFurnitureIndex, dwItemIndex, 0, 
									0, 0, 0, 0,
									HouseSlot, eHOUSEFURNITURE_STATE_KEEP, 0);

									return;
								}
							}
						}
					}
				}

				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_STORED_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = 0;
				pPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
		break;
	case MP_HOUSE_RESTORED_SYN:
		{
			//dwData1(채널) dwData2(가구ObjectIndex) dwData3(내집창고Slot)
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

			DWORD dwChannelID = pmsg->dwData1;
			DWORD dwFurnitureObjectIndex = pmsg->dwData2;
			BYTE HouseSlot = (BYTE)pmsg->dwData3;

			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				stHouse* pHouse = GetHouseByChannelID(dwChannelID);
				if(pHouse &&
					pHouse->HouseInfo.dwOwnerUserIndex==pPlayer->GetUserID())
				{
					if(IsKeeping(pHouse->HouseInfo.dwOwnerUserIndex, dwFurnitureObjectIndex))
					{
						stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwFurnitureObjectIndex);
						if(pFurniture)
						{
							// 아이템생성
							// 로그는CItemManager::ObtainItemDBResult() 에서 기록 DBIndex 확보때문에...
							ITEMMGR->ObtainGeneralItem(pPlayer, pFurniture->dwLinkItemIndex, 1, eLog_ItemObtainHouseRestore, MP_ITEM_HOUSE_RESTORED);

							// 내집창고에서 제거
							RemoveFurniture(dwFurnitureObjectIndex, pHouse);

							MSG_DWORD msg;
							msg.Category = MP_HOUSE;
							msg.Protocol = MP_HOUSE_RESTORED_ACK;
							msg.dwObjectID = pPlayer->GetID();
							msg.dwData = dwFurnitureObjectIndex;
							pPlayer->SendMsg( &msg, sizeof( msg ) );

							// DB삭제
							HouseFurnitureUpdate(pPlayer->GetID(), eHOUSEFURNITURE_DEL, pPlayer->GetUserID(), pFurniture->dwFurnitureIndex, pFurniture->dwLinkItemIndex, 0, 
								0, 0, 0, 0,
								HouseSlot, eHOUSEFURNITURE_STATE_UNKEEP, 0);

							return;
						}
					}
				}

				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_RESTORED_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = dwFurnitureObjectIndex;
				pPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
		break;

	case MP_HOUSE_DECOMODE_SYN:
		{
			//dwData1(채널), dwData2(On/Off)
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			DWORD dwChannelID = pmsg->dwData1;
			BOOL bDecoMode = (BOOL)pmsg->dwData2;

			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				stHouse* pHouse = GetHouseByChannelID(dwChannelID);
				if(pHouse &&
					pHouse->HouseInfo.dwOwnerUserIndex==pPlayer->GetUserID())
				{
					if(bDecoMode)
					{
						if(CanDecoMode(pHouse->HouseInfo.dwOwnerUserIndex, pPlayer->GetID()))
						{
							OBJECTSTATEMGR_OBJ->StartObjectState(pPlayer, eObjectState_Housing, 0);

							MSG_DWORD msg;
							msg.Category = MP_HOUSE;
							msg.Protocol = MP_HOUSE_DECOMODE_ACK;
							msg.dwObjectID = pPlayer->GetID();
							msg.dwData = 1;	// DecoMode On
							PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
							return;
						}
					}
					else
					{
						if(IsDecoMode(pHouse->HouseInfo.dwOwnerUserIndex, pPlayer->GetID()))
						{
							OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_Housing, 0);

							MSG_DWORD msg;
							msg.Category = MP_HOUSE;
							msg.Protocol = MP_HOUSE_DECOMODE_ACK;
							msg.dwObjectID = pPlayer->GetID();
							msg.dwData = 0;	// DecoMode Off
							PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
							return;
						}
					}
				}
			}

			MSG_DWORD msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_DECOMODE_NACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData = bDecoMode;
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
		break;

	case MP_HOUSE_INSTALL_SYN:
		{
			MSG_HOUSE_FURNITURE_INSTALL* pmsg = (MSG_HOUSE_FURNITURE_INSTALL*)pMsg;

			DWORD dwChannelID = pmsg->dwChannel;
			DWORD dwFurnitureObjectIndex = pmsg->dwFurnitureObjectIndex;
			BYTE HouseSlot = (BYTE)pmsg->wSlot;
			
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer && pPlayer->GetState()==eObjectState_Housing)
			{
				stHouse* pHouse = GetHouseByChannelID(dwChannelID);
				if(pHouse &&
					pHouse->HouseInfo.dwOwnerUserIndex==pPlayer->GetUserID())
				{
					if(CanInstall(pHouse->HouseInfo.dwOwnerUserIndex, dwFurnitureObjectIndex))
					{
						stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwFurnitureObjectIndex);
						if(pFurniture)
						{
							stFunitureInfo* pInfo = GAMERESRCMNGR->GetFunitureInfo(pFurniture->dwFurnitureIndex);
							if(pInfo)
							{
								// 갱신
								pFurniture->fAngle = pmsg->fAngle;
								pFurniture->vPosition.x = pmsg->vPos.x;
								pFurniture->vPosition.y = pmsg->vPos.y;
								pFurniture->vPosition.z = pmsg->vPos.z;

								// 최초설치시 시간을 넣어준다.
								DWORD dwEndTime = 0;
								if(pFurniture->wState==eHOUSEFURNITURE_STATE_KEEP)
								{
									dwEndTime = GetRemainTimeFromItemIndex(pFurniture->dwLinkItemIndex);
									pFurniture->dwRemainTime = dwEndTime;

									// 사용기간별 가중치
									DWORD dwWeight = 1;
									if(dwEndTime >= 2592000)
										dwWeight = 5;
									else if(dwEndTime >= 1296000)
										dwWeight = 3;

									pHouse->HouseInfo.dwHousePoint += (pInfo->dwDecoPoint + dwWeight) * 10;
									HouseUpdate(&pHouse->HouseInfo);
								}

								pFurniture->wState = eHOUSEFURNITURE_STATE_INSTALL;

								// DB갱신
								HouseFurnitureUpdate(pPlayer->GetID(), eHOUSEFURNITURE_UPDATE, pPlayer->GetUserID(), pFurniture->dwFurnitureIndex, pFurniture->dwLinkItemIndex, 0, 
									pFurniture->vPosition.x, pFurniture->vPosition.y, pFurniture->vPosition.z, pFurniture->fAngle,
									HouseSlot, eHOUSEFURNITURE_STATE_INSTALL, dwEndTime);

								// 하우스에 갱신
								UpdateFurniture(pFurniture, pHouse, TRUE);

								// 내집창고 가구설치 로그
								Log_Housing(pPlayer->GetUserID(), pPlayer->GetObjectName(), eHouseLog_Install, 0, 0, 0,
									pFurniture->dwFurnitureIndex, HouseSlot, pHouse->HouseInfo.szHouseName, pHouse->dwDecoPoint, pHouse->HouseInfo.dwHousePoint);

								// 같은 채널에 있는 유저에게통보
								MSG_HOUSE_FURNITURE msg;
								msg.Category = MP_HOUSE;
								msg.Protocol = MP_HOUSE_INSTALL_ACK;
								msg.dwObjectID = pPlayer->GetID();
								msg.wState = eHOUSEFURNITURE_ADD;
								msg.dwDecoPoint = pHouse->dwDecoPoint;
								memcpy(&msg.Furniture, pFurniture, sizeof(msg.Furniture));
								PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );

								// 문 설치시 기존문 설치해제.
								if(IsDoor(pFurniture->dwFurnitureIndex))
								{
									DWORD dwOldDoor = pHouse->dwCurDoorIndex;
									stFurniture* pOldDoor = pHouse->pFurnitureList.GetData(dwOldDoor);
									if(pOldDoor)
									{
										pOldDoor->wState = eHOUSEFURNITURE_STATE_UNINSTALL;
										// DB갱신
										HouseFurnitureUpdate(pPlayer->GetID(), eHOUSEFURNITURE_UPDATE, pPlayer->GetUserID(), pOldDoor->dwFurnitureIndex, pOldDoor->dwLinkItemIndex, 0, 
											pOldDoor->vPosition.x, pOldDoor->vPosition.y, pOldDoor->vPosition.z, pOldDoor->fAngle,
											pOldDoor->wSlot, eHOUSEFURNITURE_STATE_UNINSTALL, dwEndTime);

										// 하우스에 갱신
										UpdateFurniture(pOldDoor, pHouse);

										// 내집창고 가구설치해제 로그
										Log_Housing(pPlayer->GetUserID(), pPlayer->GetObjectName(), eHouseLog_UnInstall, 0, 0, 0,
											pOldDoor->dwFurnitureIndex, pOldDoor->wSlot, pHouse->HouseInfo.szHouseName, pHouse->dwDecoPoint, pHouse->HouseInfo.dwHousePoint);

										// 같은 채널에 있는 유저에게통보
										MSG_DWORD3 msg;
										msg.Category = MP_HOUSE;
										msg.Protocol = MP_HOUSE_UNINSTALL_ACK;
										msg.dwObjectID = pPlayer->GetUserID();
										msg.dwData1 = pOldDoor->dwObjectIndex;
										msg.dwData2 = 0; // 설치해제
										msg.dwData3 = pHouse->dwDecoPoint;
										PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
									}

									pHouse->dwCurDoorIndex = pFurniture->dwObjectIndex;
								}

								return;
							}
						}
					}
				}
			}

			MSG_DWORD msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_INSTALL_NACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData = 0;
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
		break;
	case MP_HOUSE_UNINSTALL_SYN:
		{
			//dwData1(채널) dwData2(가구ObjectIndex)
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			DWORD dwChannelID = pmsg->dwData1;
			DWORD dwFurnitureObjectIndex = pmsg->dwData2;

			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer && pPlayer->GetState()==eObjectState_Housing)
			{
				stHouse* pHouse = GetHouseByChannelID(dwChannelID);
				if(pHouse &&
					pHouse->HouseInfo.dwOwnerUserIndex==pPlayer->GetUserID())
				{
					if(IsInstall(pHouse->HouseInfo.dwOwnerUserIndex, dwFurnitureObjectIndex))
					{
						stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwFurnitureObjectIndex);
						if(pFurniture)
						{
							//기본품목은 설치해제 불가.
							if( pFurniture->bNotDelete )
								return;

							// 문은 설치해제 불가.
							if(IsDoor(pFurniture->dwFurnitureIndex))
								return;

							// 갱신
							pFurniture->wState = eHOUSEFURNITURE_STATE_UNINSTALL;

							// DB갱신
							HouseFurnitureUpdate(pPlayer->GetID(), eHOUSEFURNITURE_UPDATE, pPlayer->GetUserID(), pFurniture->dwFurnitureIndex, pFurniture->dwLinkItemIndex, 0, 
								pFurniture->vPosition.x, pFurniture->vPosition.y, pFurniture->vPosition.z, pFurniture->fAngle,
								pFurniture->wSlot, eHOUSEFURNITURE_STATE_UNINSTALL, 0);

							// 하우스에 갱신
							UpdateFurniture(pFurniture, pHouse);

							// 내집창고 가구설치해제 로그
							Log_Housing(pPlayer->GetUserID(), pPlayer->GetObjectName(), eHouseLog_UnInstall, 0, 0, 0,
								pFurniture->dwFurnitureIndex, pFurniture->wSlot, pHouse->HouseInfo.szHouseName, pHouse->dwDecoPoint, pHouse->HouseInfo.dwHousePoint);

							// 같은 채널에 있는 유저에게통보
							MSG_DWORD3 msg;
							msg.Category = MP_HOUSE;
							msg.Protocol = MP_HOUSE_UNINSTALL_ACK;
							msg.dwObjectID = pPlayer->GetUserID();
							msg.dwData1 = dwFurnitureObjectIndex;
							msg.dwData2 = 0; // 설치해제
							msg.dwData3 = pHouse->dwDecoPoint;
							PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
							return;
						}
					}
				}
			}

			MSG_DWORD msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_UNINSTALL_NACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData = dwFurnitureObjectIndex;
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
		break;
	case MP_HOUSE_UPDATE_SYN:
		{
			MSG_HOUSE_FURNITURE_INSTALL* pmsg = (MSG_HOUSE_FURNITURE_INSTALL*)pMsg;

			DWORD dwChannelID = pmsg->dwChannel;
			DWORD dwFurnitureObjectIndex = pmsg->dwFurnitureObjectIndex;
			BYTE HouseSlot = (BYTE)pmsg->wSlot;
			
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer && pPlayer->GetState()==eObjectState_Housing)
			{
				stHouse* pHouse = GetHouseByChannelID(dwChannelID);
				if(pHouse &&
					pHouse->HouseInfo.dwOwnerUserIndex==pPlayer->GetUserID())
				{
					if(CanUpdate(pHouse->HouseInfo.dwOwnerUserIndex, dwFurnitureObjectIndex))
					{
						stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwFurnitureObjectIndex);
						if(pFurniture)
						{
							// 갱신
							pFurniture->fAngle = pmsg->fAngle;
							pFurniture->vPosition.x = pmsg->vPos.x;
							pFurniture->vPosition.y = pmsg->vPos.y;
							pFurniture->vPosition.z = pmsg->vPos.z;

							// DB갱신
							HouseFurnitureUpdate(pPlayer->GetID(), eHOUSEFURNITURE_UPDATE, pPlayer->GetUserID(), pFurniture->dwFurnitureIndex, pFurniture->dwLinkItemIndex, 0, 
								pmsg->vPos.x, pmsg->vPos.y, pmsg->vPos.z, pmsg->fAngle,
								HouseSlot, eHOUSEFURNITURE_STATE_INSTALL, 0);

							// 하우스에 갱신
							UpdateFurniture(pFurniture, pHouse);

							// 같은 채널에 있는 유저에게통보
							MSG_HOUSE_FURNITURE msg;
							msg.Category = MP_HOUSE;
							msg.Protocol = MP_HOUSE_UPDATE_ACK;
							msg.dwObjectID = pPlayer->GetID();
							msg.wState = eHOUSEFURNITURE_UPDATE;
							memcpy(&msg.Furniture, pFurniture, sizeof(msg.Furniture));
							PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
							return;
						}
					}
				}
			}     

			MSG_DWORD msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_UPDATE_NACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData = 0;
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
		break;
	case MP_HOUSE_DESTROY_SYN:
		{
			// dwData1(채널), dwData2(가구Index)
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			DWORD dwChannelID = pmsg->dwData1;
			DWORD dwFurnitureObjectIndex = pmsg->dwData2;
			
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				stHouse* pHouse = GetHouseByChannelID(dwChannelID);
				if(pHouse &&
					pHouse->HouseInfo.dwOwnerUserIndex==pPlayer->GetUserID())
				{
					if(CanDestroy(pHouse->HouseInfo.dwOwnerUserIndex, dwFurnitureObjectIndex))
					{
						stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwFurnitureObjectIndex);
						if(pFurniture)
						{
							// DB삭제
							HouseFurnitureUpdate(pPlayer->GetID(), eHOUSEFURNITURE_DEL, pPlayer->GetUserID(), pFurniture->dwFurnitureIndex, pFurniture->dwLinkItemIndex, 0, 
									0, 0, 0, 0,
									pFurniture->wSlot, eHOUSEFURNITURE_STATE_UNINSTALL, 0);

							// 하우스에 갱신
							RemoveFurniture(dwFurnitureObjectIndex, pHouse);

							// 내집창고 가구파괴 로그
							Log_Housing(pPlayer->GetUserID(), pPlayer->GetObjectName(), eHouseLog_Destroy, 0, 0, 0,
								pFurniture->dwFurnitureIndex, pFurniture->wSlot, pHouse->HouseInfo.szHouseName, pHouse->dwDecoPoint, pHouse->HouseInfo.dwHousePoint);

							// 통보
							MSG_DWORD msg;
							msg.Category = MP_HOUSE;
							msg.Protocol = MP_HOUSE_DESTROY_ACK;
							msg.dwObjectID = pPlayer->GetID();
							msg.dwData = dwFurnitureObjectIndex;
							pPlayer->SendMsg(&msg, sizeof(msg));
							return;
						}
					}
				}

				// 통보
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_DESTROY_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = dwFurnitureObjectIndex;
				pPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
		break;
	case MP_HOUSE_ACTION_FORCE_GETOFF_SYN:
		{
			DWORD dwResult = eHOUSEERR_NONE;

			//dwData1(채널), dwData2(가구Index), dwData3(Attach슬롯), dwData4(액션Type)
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

			DWORD dwChannelID = pmsg->dwData1;
			DWORD dwFurnitureIndex = pmsg->dwData2;
			DWORD dwAttachSlot = pmsg->dwData3;
			DWORD dwActionIndex = pmsg->dwData4;
			DWORD dwActionType = eHOUSE_ACTION_TYPE_INVALID;
			DWORD dwActionValue = 0;

			// 클라이언트에서 요청한 액션
			stHouseActionInfo* pSynAction = GAMERESRCMNGR->GetHouseActionInfo(dwActionIndex);
			if(!pSynAction)
			{
				dwResult = eHOUSEERR_ERROR;
				break;
			}

			DWORD dwSynActionType = pSynAction->dwActionType;

			VECTOR3 vAdjustPos;
			vAdjustPos.x = vAdjustPos.y = vAdjustPos.z = 0;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(!pPlayer)	return;

			stHouse* pHouse = GetHouseByChannelID(dwChannelID);
			if(!pHouse)
			{
				dwResult = eHOUSEERR_NOHOUSE;
				break;
			}

			stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwFurnitureIndex);
			if(!pFurniture)
			{
				dwResult = eHOUSEERR_NOFURNITURE;
				break;
			}

			//090618 pdy 하우징 기능추가 액션 제한사항 변경 
			if( pPlayer->GetState() != eObjectState_None &&
				dwSynActionType != eHOUSE_ACTION_TYPE_GETOFF &&
				dwSynActionType != eHOUSE_ACTION_TYPE_STORAGE	)
			{
				// 히어로 상태가 NONE이아닐때 내리기,창고열기를 제외한 액션 사용불가.
				dwResult = eHOUSEERR_INVAILDSTATE ;
				break;
			}

			VECTOR3 vRiderPos, vFurniturePos;
			pPlayer->GetPosition(&vRiderPos);
			vRiderPos.y = 0.0f;
			vFurniturePos.x = pFurniture->vPosition.x;
			vFurniturePos.y = 0.0f;
			vFurniturePos.z = pFurniture->vPosition.z;
			float fDistance = CalcDistanceXZ(&vRiderPos, &vFurniturePos);

			//090618 pdy 하우징 기능추가 액션 제한사항 변경
			//문,창고열기 액션을 제외한 액션을 일정거리보다 떨어져있으면 사용할수 없다.
			if( ! IsDoor( pFurniture->dwFurnitureIndex )&& 
				dwSynActionType != eHOUSE_ACTION_TYPE_STORAGE &&
				dwSynActionType != eHOUSE_ACTION_TYPE_OPEN_HOMEPAGE)
			{
				if( fDistance>MAX_HOUSE_ACTION_DISTANCE	)
				{
					dwResult = eHOUSEERR_DISTANCE_FAR;	
					break;
				}
			}

			stFunitureInfo* pInfo = GAMERESRCMNGR->GetFunitureInfo(pFurniture->dwFurnitureIndex);
			if(!pInfo)
			{
				dwResult = eHOUSEERR_ERROR;
				break;
			}

			stHouseActionGroupInfo* pActionGroupInfo = GAMERESRCMNGR->GetHouseActionGroupInfo(pInfo->dwActionIndex);
			if(!pActionGroupInfo)
			{
				dwResult = eHOUSEERR_ERROR;
				break;
			}

			int i;
			for(i=0; i<pActionGroupInfo->byActionNum; i++)
			{
				stHouseActionInfo* pAction = GAMERESRCMNGR->GetHouseActionInfo(pActionGroupInfo->dwActionIndexList[i]);
				if(!pAction)
					continue;

				if(pAction->dwActionIndex == dwActionIndex)
				{
					dwActionType = pAction->dwActionType;
					dwActionValue = pAction->dwActionValue;
					break;
				}
			}

			if(eHOUSE_ACTION_TYPE_INVALID == dwActionType)
				dwResult = eHOUSEERR_ERROR;
			{
				stHouseRiderInfo* pRider = m_RiderList.GetData(pPlayer->GetID());
				if(!pRider)
				{
					dwResult = eHOUSEERR_NOTRIDING;
					break;
				}

				if(pRider->dwFurnitureObjectIndex != pFurniture->dwObjectIndex)
				{
					dwResult = eHOUSEERR_ERROR;
					break;
				}

				m_RiderList.Remove(pRider->dwPlayerIndex);
				m_RiderPool->Free(pRider);
				memcpy(&vAdjustPos, &pFurniture->vPosition, sizeof(VECTOR3));

				pFurniture->dwRidingPlayer[dwAttachSlot] = 0;

				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_HouseRiding, 0);
			}

			if(eHOUSEERR_NONE != dwResult)
			{
				// 실패 통보
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_FORCE_GETOFF_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = dwResult;
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}

			// 같은 채널에 있는 유저에게통보
			// dwData1(가구Index), dwData2(Attach슬롯), dwData3(액션Index), dwData4(x위치), dwData5(z위치)
			MSG_DWORD3 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ACTION_FORCE_GETOFF_ACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData1 = dwFurnitureIndex;
			msg.dwData2 = dwAttachSlot;
			msg.dwData3 = dwActionIndex;
			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );

			return;	
		}
		break;
	case MP_HOUSE_ACTION_SYN:
		{
			DWORD dwResult = eHOUSEERR_NONE;

			//dwData1(채널), dwData2(가구Index), dwData3(Attach슬롯), dwData4(액션Type)
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

			DWORD dwChannelID = pmsg->dwData1;
			DWORD dwFurnitureIndex = pmsg->dwData2;
			DWORD dwAttachSlot = pmsg->dwData3;
			DWORD dwActionIndex = pmsg->dwData4;
			DWORD dwActionType = eHOUSE_ACTION_TYPE_INVALID;
			DWORD dwActionValue = 0;

			// 클라이언트에서 요청한 액션
			stHouseActionInfo* pSynAction = GAMERESRCMNGR->GetHouseActionInfo(dwActionIndex);
			if(!pSynAction)
			{
				dwResult = eHOUSEERR_ERROR;
				break;
			}

			DWORD dwSynActionType = pSynAction->dwActionType;

			VECTOR3 vAdjustPos;
			vAdjustPos.x = vAdjustPos.y = vAdjustPos.z = 0;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(!pPlayer)	return;

			stHouse* pHouse = GetHouseByChannelID(dwChannelID);
			if(!pHouse)
			{
				dwResult = eHOUSEERR_NOHOUSE;
				break;
			}

			stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwFurnitureIndex);
			if(!pFurniture)
			{
				dwResult = eHOUSEERR_NOFURNITURE;
				break;
			}

			//090618 pdy 하우징 기능추가 액션 제한사항 변경 
			if( pPlayer->GetState() != eObjectState_None &&
				dwSynActionType != eHOUSE_ACTION_TYPE_GETOFF &&
				dwSynActionType != eHOUSE_ACTION_TYPE_STORAGE	)
			{
				// 히어로 상태가 NONE이아닐때 내리기,창고열기를 제외한 액션 사용불가.
				dwResult = eHOUSEERR_INVAILDSTATE ;
				break;
			}

			VECTOR3 vRiderPos, vFurniturePos;
			pPlayer->GetPosition(&vRiderPos);
			vRiderPos.y = 0.0f;
			vFurniturePos.x = pFurniture->vPosition.x;
			vFurniturePos.y = 0.0f;
			vFurniturePos.z = pFurniture->vPosition.z;
			float fDistance = CalcDistanceXZ(&vRiderPos, &vFurniturePos);

			//090618 pdy 하우징 기능추가 액션 제한사항 변경
			//문,창고열기 액션을 제외한 액션을 일정거리보다 떨어져있으면 사용할수 없다.
			if( ! IsDoor( pFurniture->dwFurnitureIndex )&& 
				dwSynActionType != eHOUSE_ACTION_TYPE_STORAGE &&
				dwSynActionType != eHOUSE_ACTION_TYPE_OPEN_HOMEPAGE)
			{
				if( fDistance>MAX_HOUSE_ACTION_DISTANCE	)
				{
					dwResult = eHOUSEERR_DISTANCE_FAR;	
					break;
				}
			}

			stFunitureInfo* pInfo = GAMERESRCMNGR->GetFunitureInfo(pFurniture->dwFurnitureIndex);
			if(!pInfo)
			{
				dwResult = eHOUSEERR_ERROR;
				break;
			}

			stHouseActionGroupInfo* pActionGroupInfo = GAMERESRCMNGR->GetHouseActionGroupInfo(pInfo->dwActionIndex);
			if(!pActionGroupInfo)
			{
				dwResult = eHOUSEERR_ERROR;
				break;
			}

			int i;
			for(i=0; i<pActionGroupInfo->byActionNum; i++)
			{
				stHouseActionInfo* pAction = GAMERESRCMNGR->GetHouseActionInfo(pActionGroupInfo->dwActionIndexList[i]);
				if(!pAction)
					continue;

				if(pAction->dwActionIndex == dwActionIndex)
				{
					dwActionType = pAction->dwActionType;
					dwActionValue = pAction->dwActionValue;
					break;
				}
			}

			if(eHOUSE_ACTION_TYPE_INVALID == dwActionType)
				dwResult = eHOUSEERR_ERROR;

			switch(dwActionType)
			{
			case eHOUSE_ACTION_TYPE_RIDE:
				{
					stHouseRiderInfo* pRider = m_RiderList.GetData(pPlayer->GetID());
					if(pRider)
					{
						dwResult = eHOUSEERR_RIDING;
						break;
					}

					if(pFurniture->dwRidingPlayer[dwAttachSlot])
					{
						dwResult = eHOUSEERR_HASRIDER;
						break;
					}

					pRider = m_RiderPool->Alloc();
					if(pRider)
					{
						pRider->Clear();
						pRider->dwPlayerIndex = pPlayer->GetID();
						pRider->dwFurnitureObjectIndex = pFurniture->dwObjectIndex;
						pRider->wSlot = (WORD)dwAttachSlot;
						m_RiderList.Add(pRider, pRider->dwPlayerIndex);
						memcpy(&vAdjustPos, &pFurniture->vPosition, sizeof(VECTOR3));

						pFurniture->dwRidingPlayer[dwAttachSlot] = pPlayer->GetID();

						OBJECTSTATEMGR_OBJ->StartObjectState(pPlayer, eObjectState_HouseRiding, 0);
					}
				}
				break;

			case eHOUSE_ACTION_TYPE_GETOFF:
				{
					stHouseRiderInfo* pRider = m_RiderList.GetData(pPlayer->GetID());
					if(!pRider)
					{
						dwResult = eHOUSEERR_NOTRIDING;
						break;
					}

					if(pRider->dwFurnitureObjectIndex != pFurniture->dwObjectIndex)
					{
						dwResult = eHOUSEERR_ERROR;
						break;
					}

					m_RiderList.Remove(pRider->dwPlayerIndex);
					m_RiderPool->Free(pRider);
					memcpy(&vAdjustPos, &pFurniture->vPosition, sizeof(VECTOR3));

					pFurniture->dwRidingPlayer[dwAttachSlot] = 0;

					OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_HouseRiding, 0);
				}
				break;

			case eHOUSE_ACTION_TYPE_BUFF:
				{
					if(const cActiveSkillInfo* const info = SKILLMGR->GetActiveInfo( dwActionValue ))
					{
						SKILLMGR->AddBuffSkill(
							*pPlayer,
							info->GetInfo());
					}
				}
				break;

			case eHOUSE_ACTION_TYPE_NORMAL:
				{
				}
				break;

			case eHOUSE_ACTION_TYPE_STORAGE:
				{
					// 예외처리
					if(pHouse->HouseInfo.dwOwnerUserIndex != pPlayer->GetUserID())
					{
						dwResult = eHOUSEERR_NOTOWNER;
						break;
					}

					// 창고열어주기
					if( ! pPlayer->GetStorageNum() )
					{	
						dwResult = eHOUSEERR_ERROR;
						break;
					}	
					else if( ! pPlayer->IsGotWarehouseItems() )
					{	
						CharacterStorageItemInfo( pPlayer->GetID(), pPlayer->GetUserID(), 0 ) ;
						pPlayer->SetGotWarehouseItems( TRUE );
					}
					else
					{	
						pPlayer->ProcessTimeCheckItem( TRUE );
						STORAGEMGR->SendStorageItemInfo( pPlayer );
					}
				}
				break;

			default:
				break;
			}

			if(eHOUSEERR_NONE != dwResult)
			{
				// 실패 통보
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_NACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = dwResult;
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}

			// 같은 채널에 있는 유저에게통보
			// dwData1(가구Index), dwData2(Attach슬롯), dwData3(액션Index), dwData4(x위치), dwData5(z위치)
			MSG_DWORD5 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ACTION_ACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData1 = dwFurnitureIndex;
			msg.dwData2 = dwAttachSlot;
			msg.dwData3 = dwActionIndex;
			msg.dwData4 = (DWORD)vAdjustPos.x;
			msg.dwData5 = (DWORD)vAdjustPos.z;
			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );

			if(eHOUSE_ACTION_TYPE_RIDE==dwActionType ||
				eHOUSE_ACTION_TYPE_GETOFF==dwActionType)
			{
				//위치 보정
				CCharMove::SetPosition( pPlayer, &vAdjustPos );
				CCharMove::CorrectPlayerPosToServer(pPlayer);
			}

			return;				
		}
		break;
	case MP_HOUSE_BONUS_SYN:
		{
			//dwData1(채널), dwData2(보너스index)
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			UseDecoBonus(pmsg->dwData1, pmsg->dwObjectID, pmsg->dwData2);
		}
		break;
	case MP_HOUSE_VOTE_SYN:
		{
			//dwData1(채널)
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			stHouse* pHouse = GetHouseByChannelID(pmsg->dwData);
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);

			if(pHouse && pPlayer)
			{
				//090618 pdy 하우징 기능추가 액션 제한사항 변경 
				if( pPlayer->GetState() != eObjectState_None )
					return;

				stHouseVoteInfo* pVote = m_VoteList.GetData(pPlayer->GetID());
				if(pVote)
				{
					// 이미 추천했음
					MSG_DWORD msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_VOTE_NACK;
					msg.dwObjectID = pPlayer->GetID();
					msg.dwData = pHouse->HouseInfo.dwOwnerUserIndex;
					pPlayer->SendMsg(&msg, sizeof(msg));
					return;
				}

				pVote = m_VotePool->Alloc();
				if(pVote)
				{
					pVote->Clear();
					pVote->dwPlayerIndex = pPlayer->GetID();
					pVote->dwVoteHouseUserIndex = pHouse->HouseInfo.dwOwnerUserIndex;
					m_VoteList.Add(pVote, pVote->dwPlayerIndex);
				}

				pHouse->HouseInfo.dwHousePoint += 20;
				HouseUpdate(&pHouse->HouseInfo);

				// 통보
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_VOTE_ACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = pHouse->HouseInfo.dwOwnerUserIndex;
				pPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
		break;
	case MP_HOUSE_EXIT_SYN:
		{
			if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
			return;

			//dwData1(채널)
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			//090618 pdy 액션 제한사항 변경 
			if( pPlayer)
			if( pPlayer->GetState() != eObjectState_None )
			{
				MSG_DWORD msg;
				msg.Category	= MP_HOUSE;
				msg.Protocol	= MP_HOUSE_EXIT_NACK;
				msg.dwObjectID  = pPlayer->GetID();
				msg.dwData	= eHOUSEERR_INVAILDSTATE;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}

			ChangeMapFromHouse(pmsg->dwObjectID);
		}
		break;
	case MP_HOUSE_KICK_SYN:
		{
			if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
				return;

			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			CPlayer* pDestPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwData);
			if(pPlayer && pDestPlayer && (pPlayer != pDestPlayer))
			{
				if(pPlayer->GetChannelID() == pDestPlayer->GetChannelID())
				{
					ChangeMapFromHouse(pDestPlayer->GetID());
				}

				// 통보
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_KICK_ACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = pDestPlayer->GetID();
				pPlayer->SendMsg(&msg, sizeof(msg));
				pDestPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
	case MP_HOUSE_CHEAT_CLOSE_SYN:
		{
			if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
			{
				MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

				MSG_DWORD2 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_CHEAT_CLOSE_SYN;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.dwData1 = pmsg->dwData1;
				msg.dwData2 = g_pServerSystem->GetMapNum();

				PACKEDDATA_OBJ->SendToMapServer(HOUSINGMAPNUM, &msg, sizeof(msg));
				return;
			}

			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			if(pmsg->dwData2 == 0)
				return;

			CloseHouse(pmsg->dwData1);
		}
		break;
	case MP_HOUSE_CHEAT_GETINFO_SYN:
		{
			if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
			{
				MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

				MSG_DWORD2 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_CHEAT_GETINFO_SYN;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.dwData1 = pmsg->dwData1;
				msg.dwData2 = g_pServerSystem->GetMapNum();

				PACKEDDATA_OBJ->SendToMapServer(HOUSINGMAPNUM, &msg, sizeof(msg));
				return;
			}

			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			stHouse* pHouse;

			if(pmsg->dwData1 == 0)
			{
				MSG_HOUSE_CHEATINFO_ALL msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_CHEAT_GETINFOALL_ACK;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.dwHouseNum = m_HouseList.GetDataNum();
				msg.dwUserNum = g_pUserTable->GetUserCount();
				msg.dwReservationNum = m_ReservationList.GetDataNum();

				m_HouseList.SetPositionHead();
				int nCnt = 0;
				while((pHouse = m_HouseList.GetData())!= NULL)
				{
					msg.HouseInfo[nCnt].dwChannelID = pHouse->dwChannelID;
					msg.HouseInfo[nCnt].dwOwnerUserIndex = pHouse->HouseInfo.dwOwnerUserIndex;
					msg.HouseInfo[nCnt].dwVisiterNum = pHouse->dwJoinPlayerNum;
					strcpy(msg.HouseInfo[nCnt].szHouseName, pHouse->HouseInfo.szHouseName);
					nCnt++;
				}

				if(0 == pmsg->dwData2)
				{
					CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
					if(!pPlayer)	return;

					pPlayer->SendMsg(&msg, msg.GetMsgLength());
				}
				else
				{
					PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData2, &msg, msg.GetMsgLength());
				}
			}
			else
			{
				MSG_HOUSE_CHEATINFO_ONE msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_CHEAT_GETINFOONE_ACK;
				msg.dwObjectID = pmsg->dwObjectID;

				pHouse = GetHouseByChannelID(pmsg->dwData1);
				if(pHouse)
				{
					msg.HouseInfo.dwChannelID = pHouse->dwChannelID;
					msg.HouseInfo.dwOwnerUserIndex = pHouse->HouseInfo.dwOwnerUserIndex;
					msg.HouseInfo.dwVisiterNum = pHouse->dwJoinPlayerNum;
					strcpy(msg.HouseInfo.szHouseName, pHouse->HouseInfo.szHouseName);
					
					CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pHouse->dwOwnerCharIndex);
					if(pPlayer)
						strcpy(msg.HouseInfo.szOwnerName, pPlayer->GetObjectName());

					stFurniture* pFurniture;
					pHouse->pFurnitureList.SetPositionHead();
					while((pFurniture = pHouse->pFurnitureList.GetData()) != NULL)
					{
						if(pFurniture->dwFurnitureIndex)
						{
							msg.HouseInfo.m_dwFurnitureNum[pFurniture->wCategory]++;

							if(pFurniture->wState == eHOUSEFURNITURE_STATE_INSTALL)
								msg.HouseInfo.m_dwInstalledNum[pFurniture->wCategory]++;
						}
					}
				}

				if(0 == pmsg->dwData2)
				{
					CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
					if(!pPlayer)	return;

					pPlayer->SendMsg(&msg, msg.GetMsgLength());
				}
				else
				{
					PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData2, &msg, msg.GetMsgLength());
				}
			}
		}
		break;
	case MP_HOUSE_CHEAT_GETINFOALL_ACK:
		{
			MSG_HOUSE_CHEATINFO_ALL* pmsg = (MSG_HOUSE_CHEATINFO_ALL*)pMsg;

			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
				pPlayer->SendMsg(pmsg, dwLength);
		}
		break;
	case MP_HOUSE_CHEAT_GETINFOONE_ACK:
		{
			MSG_HOUSE_CHEATINFO_ONE* pmsg = (MSG_HOUSE_CHEATINFO_ONE*)pMsg;

			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
				pPlayer->SendMsg(pmsg, dwLength);
		}
		break;
	case MP_HOUSE_CHEAT_CALCRANK:
		{
			m_bLoadRank = FALSE;
			HouseRankUpdate();
		}
		break;
	case MP_HOUSE_CHEAT_DELETE_SYN:
		{
			if(g_pServerSystem->GetMapNum() != HOUSINGMAPNUM)
			{
				MSG_DWORD_NAME* pmsg = (MSG_DWORD_NAME*)pMsg;

				MSG_DWORD2_NAME msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_CHEAT_DELETE_SERVER_SYN;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.dwData1 = dwConnectionIndex;
				msg.dwData2 = g_pServerSystem->GetMapNum();
				SafeStrCpy(msg.Name, pmsg->Name, sizeof(msg.Name));

				PACKEDDATA_OBJ->SendToMapServer(HOUSINGMAPNUM, &msg, sizeof(msg));
				return;
			}

			MSG_DWORD_NAME* pmsg = (MSG_DWORD_NAME*)pMsg;

			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				// 하우스내에서는 불가
				DWORD dwChannel = pPlayer->GetChannelID();
				if(GetHouseByChannelID(dwChannel))
				{
					MSG_DWORD msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_CHEAT_DELETE_NACK;
					msg.dwObjectID = pPlayer->GetID();
					msg.dwData = eHOUSEERR_DONOT_HOUSE;
					pPlayer->SendMsg( &msg, sizeof( msg ) );
					return;
				}
			}
		}
		break;
	case MP_HOUSE_CHEAT_DELETE_SERVER_SYN:
		{
			MSG_DWORD2_NAME* pmsg = (MSG_DWORD2_NAME*)pMsg;
			HouseDelete(pmsg->dwData1, pmsg->dwData2, pmsg->dwObjectID, pmsg->Name);
		}
		break;
	case MP_HOUSE_CHEAT_DELETE_ACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_CHEAT_DELETE_ACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = eHOUSEERR_NONE;
				pPlayer->SendMsg( &msg, sizeof( msg ) );
			}
		}
		break;
	}
}

void HousingMgr::SendHouseMsgToPlayer(stHouse* pHouse, CPlayer* pPlayer)
{
	if(!pPlayer)
		return;

	BOOL bVister = TRUE;
	
	if(pHouse->HouseInfo.dwOwnerUserIndex == pPlayer->GetUserID())
		bVister = FALSE;

	char cRank;
	if(m_HouseRank.dwRank_1_UserIndex == pPlayer->GetUserID())
		cRank = 1;
	else if(m_HouseRank.dwRank_2_UserIndex == pPlayer->GetUserID())
		cRank = 2;
	else if(m_HouseRank.dwRank_3_UserIndex == pPlayer->GetUserID())
		cRank = 3;
	else
		cRank = 0;

	// 하우스정보 & 보너스 목록
	MSG_HOUSE_HOUSEINFO msgHouseInfo;
	msgHouseInfo.Category = MP_HOUSE;
	msgHouseInfo.Protocol = MP_HOUSE_INFO;
	msgHouseInfo.dwObjectID = pPlayer->GetID();
	msgHouseInfo.bInit = TRUE;
	msgHouseInfo.cRank = cRank;
	msgHouseInfo.dwChannel = pHouse->dwChannelID;
	msgHouseInfo.dwDecoPoint = pHouse->dwDecoPoint;
	memcpy(&msgHouseInfo.HouseInfo, &pHouse->HouseInfo, sizeof(msgHouseInfo.HouseInfo));
	pPlayer->SendMsg( &msgHouseInfo, sizeof( msgHouseInfo ) );

	// 가구목록
	int i;
	for(i=0; i<MAX_HOUSING_CATEGORY_NUM; i++)
	{
		int nCnt = 0;

		MSG_HOUSE_FURNITURELIST msgFurnitureList;
		msgFurnitureList.Category = MP_HOUSE;
		msgFurnitureList.Protocol = MP_HOUSE_FURNITURELIST;
		msgFurnitureList.dwObjectID = pPlayer->GetID();
		msgFurnitureList.wCategory = (WORD)i;
		msgFurnitureList.wNum = 0;

		if(0 == pHouse->m_dwCategoryNum[i])
		{
			pPlayer->SendMsg( &msgFurnitureList, msgFurnitureList.GetMsgLength() );
			continue;
		}

		if(!bVister)
		{
			// 집주인은 모든 목록 전송
			msgFurnitureList.wCategory = (WORD)i;

			int j;
			for(j=0; j<MAX_HOUSING_SLOT_NUM; j++)
			{
				DWORD dwObjectIndex = pHouse->m_dwFurnitureList[i][j];
				stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwObjectIndex);
				if(pFurniture)
				{
					memcpy(&msgFurnitureList.Furniture[nCnt], pFurniture, sizeof(stFurniture));
				}
				nCnt++;
			}
		}
		else
		{
			// 방문객은 설치중인 목록만 전송
			msgFurnitureList.wCategory = (WORD)i;

			int j;
			for(j=0; j<MAX_HOUSING_SLOT_NUM; j++)
			{
				DWORD dwObjectIndex = pHouse->m_dwFurnitureList[i][j];
				stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwObjectIndex);
				if(pFurniture && pFurniture->wState==eHOUSEFURNITURE_STATE_INSTALL)
				{
					memcpy(&msgFurnitureList.Furniture[nCnt], pFurniture, sizeof(stFurniture));
					nCnt++;
				}
			}
		}

		msgFurnitureList.wNum = (WORD)nCnt;
		pPlayer->SendMsg( &msgFurnitureList, msgFurnitureList.GetMsgLength() );
	}
}

void HousingMgr::ChangeMapFromHouse(DWORD dwCharIndex, MAPTYPE changemap)
{
	// changemap이 0이면 LoginMap으로 귀환
	// 값이 있으면 해당 맵으로 이동

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwCharIndex);
	if(pPlayer)
	{
		if( pPlayer->IsPKMode() || LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) || pPlayer->GetAutoNoteIdx() )
		{
			MSG_DWORD msg;
			msg.Category	= MP_HOUSE;
			msg.Protocol	= MP_HOUSE_EXIT_NACK;
			msg.dwObjectID  = pPlayer->GetID();
			msg.dwData		= eHOUSEERR_ERROR;
			pPlayer->SendMsg( &msg, sizeof( msg ) );
			return;
		}

		MAPTYPE mapNum;
		if(changemap == 0)
			mapNum = pPlayer->GetLoginMapNum();
		else
			mapNum = changemap;

		MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( mapNum );
		if( !ChangeInfo ) return;
		SaveMapChangePointUpdate( pPlayer->GetID(), ChangeInfo->Kind );

		// RaMa - 04.12.14
		pPlayer->UpdateLogoutToDB(FALSE);

		VECTOR3 RandPos;
		int temp;
		temp = rand() % 500 - 250;
		RandPos.x = ChangeInfo->MovePoint.x + temp;
		temp = rand() % 500 - 250;
		RandPos.z = ChangeInfo->MovePoint.z + temp;
		RandPos.y = 0;

		pPlayer->SetMapMoveInfo( ChangeInfo->MoveMapNum, (DWORD)RandPos.x, (DWORD)RandPos.z );

		// 081218 LUJ, 플레이어를 해제하기 전에 DB에 업데이트할 주요 정보를 복사해놓는다
		const MAPTYPE	mapType			= mapNum;
		const WORD		channelIndex	= 0; //WORD( pmsg->dwData1 );
		const DWORD		playerIndex		= pPlayer->GetID();
		const DWORD		userIndex		= pPlayer->GetUserID();
		const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
		const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );

		g_pServerSystem->RemovePlayer( pPlayer->GetID() );

		// 081218 LUJ, 업데이트보다 맵 이동이 빨리 진행될 경우 업데이트되지 않은 정보가 오게된다.
		//			이를 막기 위해 프로시저 처리 완료 후 진행한다
		UpdatePlayerOnMapOut(
			playerIndex,
			userIndex,
			inventoryMoney,
			storageMoney,
			mapType,
			channelIndex );

		ExitHouse(pPlayer->GetID(), FALSE);
	}
}

void HousingMgr::SetHouseRank(stHouseRank* pRank)
{
	if(!pRank)
		return;

	stDynamicHouseNpcMapInfo* pInfo = GetDynimiHouseNpcMapInfo(g_pServerSystem->GetMapNum());
	if(!pInfo)
		return;

	CNpc* pRankerHouse;

	stHouseRankNPC* pRankNpc = NULL;
	m_RankNpcList.SetPositionHead();
	while((pRankNpc = m_RankNpcList.GetData())!= NULL)
	{
		if(pRankNpc)
		{
			pRankerHouse = (CNpc*)g_pUserTable->FindUser(pRankNpc->dwNpcIndex);
			if(pRankerHouse)
				pRankerHouse->SetDieTime(gCurTime + 10000);
		}
		SAFE_DELETE(pRankNpc);
	}
	m_RankNpcList.RemoveAll();


	memcpy(&m_HouseRank, pRank, sizeof(m_HouseRank));

	m_dwHouseRankSetTime = gCurTime;
}

stHouseRank* HousingMgr::GetHouseRank()
{
	return &m_HouseRank;
}

void HousingMgr::RecallRankNPC()
{
	// 새로운 랭킹으로 NPC생성하기
	MAPTYPE MapNum = g_pServerSystem->GetMapNum();
	stDynamicHouseNpcInfo* pNpcInfo = 0;

	DWORD dwChannelCount = (BYTE)CHANNELSYSTEM->GetChannelCount();
	DWORD dwChannel;
	for(dwChannel=0; dwChannel<dwChannelCount; dwChannel++)
	{
		// Rank1 HouseNPC
		pNpcInfo = GetDynimiHouseNpcInfo(MapNum, 1, m_HouseRank.byRank_1_UserExterioKind);
		if(pNpcInfo && m_HouseRank.dwRank_1_UserIndex)
		{
			VECTOR3 vOutPos;
			vOutPos.x = pNpcInfo->fWorldPosX;
			vOutPos.y = 0.0f;
			vOutPos.z = pNpcInfo->fWorldPosZ;

			vOutPos.x = ((DWORD)(vOutPos.x/50.0f)*50.0f);
			vOutPos.z = ((DWORD)(vOutPos.z/50.0f)*50.0f);
			
			BASEOBJECT_INFO Baseinfo;
			NPC_TOTALINFO NpcTotalInfo;
			NPC_LIST* pNpcList = GAMERESRCMNGR->GetNpcInfo((WORD)pNpcInfo->dwNpcKind);
			if(pNpcList == 0)
				continue;

			Baseinfo.dwObjectID = g_pAISystem.GeneraterMonsterID();
			Baseinfo.BattleID = dwChannel + 1;
			
			SafeStrCpy(Baseinfo.ObjectName, m_HouseRank.szRank_1_HouseName, MAX_NAME_LENGTH+1);
			NpcTotalInfo.Group = 0;
			NpcTotalInfo.MapNum = MapNum;
			NpcTotalInfo.NpcJob = pNpcList->JobKind;
			NpcTotalInfo.NpcKind = pNpcList->NpcKind;
			NpcTotalInfo.NpcUniqueIdx = 0;
			NpcTotalInfo.dwSummonerUserIndex = 0;

			CNpc* pNpc = g_pServerSystem->AddNpc(&Baseinfo, &NpcTotalInfo, &vOutPos);
			if(pNpc)
			{
				stHouseRankNPC* pRankNpc = new stHouseRankNPC;
				if(pRankNpc)
				{
					pRankNpc->dwHouseUserIndex = m_HouseRank.dwRank_1_UserIndex;
					pRankNpc->dwNpcIndex = pNpc->GetID();
					m_RankNpcList.Add(pRankNpc, pRankNpc->dwNpcIndex);
				}
				pNpc->SetNpcAngle(pNpcInfo->fAnlge);
				pNpc->SetDieTime(0);
			}
		}

		// Rank2 HouseNPC
		pNpcInfo = GetDynimiHouseNpcInfo(MapNum, 2, m_HouseRank.byRank_2_UserExterioKind);
		if(pNpcInfo && m_HouseRank.dwRank_2_UserIndex)
		{
			VECTOR3 vOutPos;
			vOutPos.x = pNpcInfo->fWorldPosX;
			vOutPos.y = 0.0f;
			vOutPos.z = pNpcInfo->fWorldPosZ;

			vOutPos.x = ((DWORD)(vOutPos.x/50.0f)*50.0f);
			vOutPos.z = ((DWORD)(vOutPos.z/50.0f)*50.0f);
			
			BASEOBJECT_INFO Baseinfo;
			NPC_TOTALINFO NpcTotalInfo;
			NPC_LIST* pNpcList = GAMERESRCMNGR->GetNpcInfo((WORD)pNpcInfo->dwNpcKind);
			if(pNpcList == 0)
				continue;

			Baseinfo.dwObjectID = g_pAISystem.GeneraterMonsterID();
			Baseinfo.BattleID = dwChannel + 1;
			
			SafeStrCpy(Baseinfo.ObjectName, m_HouseRank.szRank_2_HouseName, MAX_NAME_LENGTH+1);
			NpcTotalInfo.Group = 0;
			NpcTotalInfo.MapNum = MapNum;
			NpcTotalInfo.NpcJob = pNpcList->JobKind;
			NpcTotalInfo.NpcKind = pNpcList->NpcKind;
			NpcTotalInfo.NpcUniqueIdx = 0;
			NpcTotalInfo.dwSummonerUserIndex = 0;

			CNpc* pNpc = g_pServerSystem->AddNpc(&Baseinfo, &NpcTotalInfo, &vOutPos);
			if(pNpc)
			{
				stHouseRankNPC* pRankNpc = new stHouseRankNPC;
				if(pRankNpc)
				{
					pRankNpc->dwHouseUserIndex = m_HouseRank.dwRank_2_UserIndex;
					pRankNpc->dwNpcIndex = pNpc->GetID();
					m_RankNpcList.Add(pRankNpc, pRankNpc->dwNpcIndex);
				}
				pNpc->SetNpcAngle(pNpcInfo->fAnlge);
				pNpc->SetDieTime(0);
			}
		}

		// Rank3 HouseNPC
		pNpcInfo = GetDynimiHouseNpcInfo(MapNum, 3, m_HouseRank.byRank_3_UserExterioKind);
		if(pNpcInfo && m_HouseRank.dwRank_3_UserIndex)
		{
			VECTOR3 vOutPos;
			vOutPos.x = pNpcInfo->fWorldPosX;
			vOutPos.y = 0.0f;
			vOutPos.z = pNpcInfo->fWorldPosZ;

			vOutPos.x = ((DWORD)(vOutPos.x/50.0f)*50.0f);
			vOutPos.z = ((DWORD)(vOutPos.z/50.0f)*50.0f);
			
			BASEOBJECT_INFO Baseinfo;
			NPC_TOTALINFO NpcTotalInfo;
			NPC_LIST* pNpcList = GAMERESRCMNGR->GetNpcInfo((WORD)pNpcInfo->dwNpcKind);
			if(pNpcList == 0)
				continue;

			Baseinfo.dwObjectID = g_pAISystem.GeneraterMonsterID();
			Baseinfo.BattleID = dwChannel + 1;
			
			SafeStrCpy(Baseinfo.ObjectName, m_HouseRank.szRank_3_HouseName, MAX_NAME_LENGTH+1);
			NpcTotalInfo.Group = 0;
			NpcTotalInfo.MapNum = MapNum;
			NpcTotalInfo.NpcJob = pNpcList->JobKind;
			NpcTotalInfo.NpcKind = pNpcList->NpcKind;
			NpcTotalInfo.NpcUniqueIdx = 0;
			NpcTotalInfo.dwSummonerUserIndex = 0;

			CNpc* pNpc = g_pServerSystem->AddNpc(&Baseinfo, &NpcTotalInfo, &vOutPos);
			if(pNpc)
			{
				stHouseRankNPC* pRankNpc = new stHouseRankNPC;
				if(pRankNpc)
				{
					pRankNpc->dwHouseUserIndex = m_HouseRank.dwRank_3_UserIndex;
					pRankNpc->dwNpcIndex = pNpc->GetID();
					m_RankNpcList.Add(pRankNpc, pRankNpc->dwNpcIndex);
				}
				pNpc->SetNpcAngle(pNpcInfo->fAnlge);
				pNpc->SetDieTime(0);
			}
		}
	}
}

void HousingMgr::LoadHousing_Setting()
{
	m_HousingSettingInfo.Clear();

	CMHFile file;

	char StrBuf[256] = {0,};

	bool bPasing = FALSE;
	int	 iGroupCount = -1;
	DWORD dwCount = 0;
	const float f_1Radian = 0.01745f;
	const float fPI	  = 3.14159f;

	DWORD dwCurMapNum = 0;

	if( ! file.Init("System/Resource/Housing_Setting.bin", "rb" ) )
		return;

	//@@ 하우징순위산출 가중치 ( 임시 ) 
	//@@ Ex : RANKPOINT = (꾸미기포인트 * 가중치)+ (한달 간의 추천수 * 가중치) + (한달 간의 방문자수 * 가중치) 
	//@@ Ex : GENERALFUNITURE_NUM = 기본재공가구 배열크기
	//@@ Ex : GENERALFUNITURE = 가구인덱스, 월드x,월드y,월드z

	while( ! file.IsEOF() )
	{
		file.GetString(StrBuf);

		if(! bPasing)		
		{
			//Todo OneLine Passing 
			if( StrBuf[0] != '#' )
			{
				file.GetLine(StrBuf,256);
				continue;
			}
			else if( strcmp( StrBuf , "#RANKPOINT_WEIGHT" ) == 0 )
			{
				m_HousingSettingInfo.fDecoPoint_Weight	= file.GetFloat();
				m_HousingSettingInfo.fRecommend_Weight	= file.GetFloat();
				m_HousingSettingInfo.fVisitor_Weight	= file.GetFloat();
				continue;
			}
			else if( strcmp( StrBuf , "#STARPOINT_NOMAL" ) == 0 )
			{
				m_HousingSettingInfo.dwStarPoint_Nomal = file.GetDword();
			}
			else if( strcmp( StrBuf , "#STARPOINT_RANKER" ) == 0 )
			{
				m_HousingSettingInfo.dwStarPoint_Ranker = file.GetDword();

			}
			else if(strcmp( StrBuf , "#RANKINGDAY" ) == 0 )
			{
				m_HousingSettingInfo.dwRankingDay = file.GetDword();
			}
			else if( strcmp( StrBuf , "#GENERALFUNITURE_NUM" ) == 0 )
			{
				iGroupCount++;
				dwCount=0;

				DWORD dwNum = file.GetDword();
				m_HousingSettingInfo.dwGeneralFunitureNumArr[iGroupCount] = dwNum;
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ] = new stGeneralFunitureInfo[ dwNum ];
				continue;
			}
			else if( strcmp( StrBuf , "#GENERALFUNITURE" ) == 0 )
			{
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].dwItemIndex = file.GetDword();
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].vWorldPos.x = file.GetFloat();
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].vWorldPos.y = file.GetFloat();
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].vWorldPos.z = file.GetFloat();

				float fAngle = 0.0f;
				fAngle = file.GetFloat();
				if( fAngle != 0.0f )
				{
					//스크립트의 각도값은 -180~180도로 넣게 된다. 이것을 라디안 값으로 바꾸자.
					fAngle *= f_1Radian;

					//180도보다 
					if( fAngle > fPI )
						fAngle = fPI ; 

					if( fAngle < -fPI )
						fAngle = -fPI ;

				}

				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].fAngle	= fAngle;
				dwCount++;
				continue;
			}
			else if( strcmp( StrBuf , "#DynamicRankHouseNpcList" ) == 0 )
			{
				stDynamicHouseNpcMapInfo* pMapInfo = new stDynamicHouseNpcMapInfo();
				dwCurMapNum = pMapInfo->MapIndex = file.GetDword();
				m_HousingSettingInfo.m_DynamicHouseNpcMapInfoList.Add( pMapInfo , pMapInfo->MapIndex );
			}
			else if( strcmp( StrBuf , "#DynamicRankHouseNpc" ) == 0 ) 
			{
				stDynamicHouseNpcMapInfo* pMapInfo = m_HousingSettingInfo.m_DynamicHouseNpcMapInfoList.GetData( dwCurMapNum ) ;

				stDynamicHouseNpcInfo* pNpcInfo = new stDynamicHouseNpcInfo();
				pNpcInfo->dwRankTypeIndex = file.GetDword();
				pNpcInfo->dwNpcKind = file.GetDword();
				pNpcInfo->fWorldPosX = file.GetFloat();
				pNpcInfo->fWorldPosZ = file.GetFloat();

				float fAngle = 0.0f;
				fAngle = file.GetFloat();
				pNpcInfo->fAnlge = fAngle;

				pNpcInfo->fDistance  = file.GetFloat();	

				pMapInfo->pDynamicHouseNpcList.Add(pNpcInfo , pNpcInfo->dwRankTypeIndex );
			}
			else if( strcmp( StrBuf , "#DynamicRankHouseNpcEnd" ) == 0 )
			{
				dwCurMapNum = 0 ;
			}
		}
	}
}

DWORD HousingMgr::GetDecoUsePoint(BOOL bRanker)
{
	if(bRanker)
		return m_HousingSettingInfo.dwStarPoint_Ranker;
	
	return m_HousingSettingInfo.dwStarPoint_Nomal;
}

DWORD HousingMgr::GetFurnitureIndexFromItemIndex(DWORD dwItemIndex)
{
	ITEM_INFO* pInfo = ITEMMGR->GetItemInfo(dwItemIndex);
	if(pInfo && pInfo->SupplyType==ITEM_KIND_FURNITURE)
	{
		return pInfo->SupplyValue;
	}

	return 0;
}

DWORD HousingMgr::GetRemainTimeFromItemIndex(DWORD dwItemIndex)
{
	ITEM_INFO* pInfo = ITEMMGR->GetItemInfo(dwItemIndex);
	if(pInfo && pInfo->SupplyType==ITEM_KIND_FURNITURE)
	{
		return pInfo->dwUseTime;
	}

	return 0;
}

stDynamicHouseNpcMapInfo* HousingMgr::GetDynimiHouseNpcMapInfo(DWORD dwMapIndex)
{
	return m_HousingSettingInfo.m_DynamicHouseNpcMapInfoList.GetData(dwMapIndex);
}

stDynamicHouseNpcInfo* HousingMgr::GetDynimiHouseNpcInfo(DWORD dwMapIndex, BYTE byRank, BYTE byExterioKind)
{
	stDynamicHouseNpcMapInfo* pMapInfo = GetDynimiHouseNpcMapInfo( dwMapIndex );

	if( ! pMapInfo ) 
		return NULL;

	DWORD dwRankTypeIndex = byRank*100 + byExterioKind;
	return pMapInfo->pDynamicHouseNpcList.GetData( dwRankTypeIndex );
}

DWORD HousingMgr::GetNewHouseChannel()
{
	DWORD dwChannel = BATTLESYSTEM->CreateChannel();
	return dwChannel;
}

void HousingMgr::RemoveHouseChannel(DWORD dwChannelID)
{
	BATTLESYSTEM->DestroyChannel(dwChannelID);
	BATTLESYSTEM->ReleaseBattleID(dwChannelID);
}

stHouse* HousingMgr::CreateNewHouse()
{
	stHouse* pNewHouse = m_HousePool->Alloc();
	pNewHouse->Clear();

	return pNewHouse;
}

void HousingMgr::RemoveHouseFromPool(stHouse* pHouse)
{
	if(pHouse)
		m_HousePool->Free(pHouse);
}

BOOL HousingMgr::CreateHouse(stHouse* pHouse)
{
	if(!pHouse)
		return FALSE;

	if(m_HouseList.GetDataNum() >= MAX_HOUSE_NUM-1)
		return FALSE;

	pHouse->dwChannelID = GetNewHouseChannel();
	if(pHouse->dwChannelID)
	{
		m_HouseList.Add(pHouse, pHouse->HouseInfo.dwOwnerUserIndex);
		m_LoadingHouseList.Add(pHouse, pHouse->HouseInfo.dwOwnerUserIndex);

		pHouse->pFurnitureList.Initialize(MAX_HOUSING_SLOT_NUM);
		pHouse->pNotDeleteFurnitureList.Initialize(100);

		// 기본 시작위치
		pHouse->vStartPos.x = 3000.0f;
		pHouse->vStartPos.y = 0.0f;
		pHouse->vStartPos.z = 3000.0f;

		return TRUE;
	}

	return FALSE;
}

void HousingMgr::DeleteHouse(DWORD dwUserIndex)
{
	stHouse* pHouse = m_HouseList.GetData(dwUserIndex);
	if(pHouse)
	{
		DWORD dwChannelID = pHouse->dwChannelID;

		// 예약자명단에 남아있는 사람이 있다면 지우기
		stHouseReservation* pReservation;
		m_ReservationList.SetPositionHead();
		while((pReservation = m_ReservationList.GetData())!= NULL)
		{
			if(pReservation && pReservation->dwChannelID==dwChannelID)
			{
				m_ReservationList.Remove(pReservation->dwCharIndex);
				m_ReservationPool->Free(pReservation);
			}
		}

		// 가구 메모리 반환
		stFurniture* pFurniture;
		pHouse->pFurnitureList.SetPositionHead();	
		while((pFurniture = pHouse->pFurnitureList.GetData())!= NULL)
		{
			ReleaseFurnitureID(pFurniture->dwObjectIndex);
			RemoveFurnitureFromPool(pFurniture);
		}
		pHouse->pFurnitureList.RemoveAll();

		RemoveHouseChannel(dwChannelID);
	}

	m_HouseList.Remove(dwUserIndex);

	if(pHouse)
		m_HousePool->Free(pHouse);

	pHouse = m_LoadingHouseList.GetData(dwUserIndex);
	if( pHouse )
	{
		m_LoadingHouseList.Remove( dwUserIndex );
	}
}

void HousingMgr::CloseHouse(DWORD dwUserIndex)
{
	stHouse* pHouse = GetHouseByUserIndex(dwUserIndex);
	if(!pHouse)
		return;
	
	DWORD dwChannelID = pHouse->dwChannelID;
	m_LoadingHouseList.Remove(dwUserIndex);
	DeleteHouse(dwUserIndex);

	// 같은 채널에 남아있는 사람이 있다면 내보내기
	CObject* pObject;
	g_pUserTable->SetPositionHead();
	while((pObject = g_pUserTable->GetData())!= NULL)
	{
		if(pObject && pObject->GetObjectKind() == eObjectKind_Player)
		{
			CPlayer* pPlayer = (CPlayer*)pObject;
			if(pPlayer->GetChannelID() == dwChannelID)
			{
				ChangeMapFromHouse(pPlayer->GetID(), 0);
			}
		}
	} 
}

BOOL HousingMgr::IsLoadingHouse(DWORD dwOwnerUserIndex)
{
	stHouse* pHouse = m_LoadingHouseList.GetData(dwOwnerUserIndex);
	if(pHouse)
		return TRUE;

	return FALSE;
}

void HousingMgr::RemoveLoadingHouse(DWORD dwUserIndex)
{
	m_LoadingHouseList.Remove(dwUserIndex);
}

stHouse* HousingMgr::GetHouseByUserIndex(DWORD dwUserIndex)
{
	stHouse* pHouse = m_HouseList.GetData(dwUserIndex);
	if(pHouse)
		return pHouse;

	return NULL;
}

stHouse* HousingMgr::GetHouseByChannelID(DWORD dwChannelID)
{
	m_HouseList.SetPositionHead();
	
	stHouse* pHouse;
	while((pHouse = m_HouseList.GetData())!= NULL)
	{
		if(pHouse->dwChannelID == dwChannelID)
			return pHouse;
	}

	return NULL;
}

DWORD HousingMgr::GetChannelIDByUserIndex(DWORD dwUserIndex)
{
	stHouse* pHouse = m_HouseList.GetData(dwUserIndex);
	if(pHouse)
		return pHouse->dwChannelID;

	return 0;
}

DWORD HousingMgr::GetPlayerNumInHouse(DWORD dwChannelID)
{
	m_HouseList.SetPositionHead();
	
	stHouse* pHouse;
	while((pHouse = m_HouseList.GetData()) != NULL)
	{
		if(pHouse->dwChannelID == dwChannelID)
			return pHouse->dwJoinPlayerNum;
	}

	return 0;
}

DWORD HousingMgr::GetChannelIDFromReservationList(DWORD dwCharIndex)
{
	stHouseReservation* pReservation = m_ReservationList.GetData(dwCharIndex);
	if(pReservation)
	{
		stHouse* pHouse = m_HouseList.GetData(pReservation->dwHouseOwnerIndex);
		if(pHouse)
		{
			return pHouse->dwChannelID;
		}
	}

	return 0;
}

VECTOR3* HousingMgr::GetStartPosFromeservationList(DWORD dwCharIndex)
{
	stHouseReservation* pReservation = m_ReservationList.GetData(dwCharIndex);
	if(pReservation)
	{
		stHouse* pHouse = m_HouseList.GetData(pReservation->dwHouseOwnerIndex);
		if(pHouse)
		{
			return &pHouse->vStartPos;
		}
	}

	return NULL;
}

void HousingMgr::ReservationHouse(DWORD dwOwnerIndex, DWORD dwChannelID, DWORD dwCharIndex, CHAR cVisitKind)
{
	stHouseReservation* pReservation = NULL;

	pReservation = m_ReservationList.GetData(dwCharIndex);
	if(pReservation)
	{
		pReservation->dwCharIndex = dwCharIndex;
		pReservation->dwHouseOwnerIndex = dwOwnerIndex;
		pReservation->dwChannelID = dwChannelID;
		pReservation->dwRegistTime = gCurTime;
		pReservation->cVisitKind = cVisitKind;
	}
	else
	{
		pReservation = m_ReservationPool->Alloc();
		if(pReservation)
		{
			pReservation->Clear();
			pReservation->dwCharIndex = dwCharIndex;
			pReservation->dwHouseOwnerIndex = dwOwnerIndex;
			pReservation->dwChannelID = dwChannelID;
			pReservation->dwRegistTime = gCurTime;
			pReservation->cVisitKind = cVisitKind;

			m_ReservationList.Add(pReservation, dwCharIndex);
		}
	}
}

BOOL HousingMgr::EntranceHouse(DWORD dwCharIndex)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwCharIndex);
	if(!pPlayer)
		return FALSE;

	stHouseReservation* pReservation = m_ReservationList.GetData(dwCharIndex);
	if(pReservation)
	{
		stHouse* pHouse = m_HouseList.GetData(pReservation->dwHouseOwnerIndex);
		if(pHouse)
		{
			pHouse->dwJoinPlayerNum++;

			// 방문자와 집주인이 다르고 아이템사용이동이면 방문자카운트를 올려줌.
			if(pPlayer->GetUserID()!=pHouse->HouseInfo.dwOwnerUserIndex &&
				(eHouseVisitByItem==pReservation->cVisitKind ||
				eHouseVisitByItemRandom==pReservation->cVisitKind))
			{
				pHouse->HouseInfo.dwDailyVisitCount++;
				pHouse->HouseInfo.dwTotalVisitCount++;
				pHouse->HouseInfo.dwHousePoint++;
				HouseUpdate(&pHouse->HouseInfo);
			}

			m_ReservationList.Remove(dwCharIndex);
			m_ReservationPool->Free(pReservation);

			// 플레이어에게 가구목록 보내기
			SendHouseMsgToPlayer(pHouse, pPlayer);

			if(pHouse->HouseInfo.dwOwnerUserIndex == pPlayer->GetUserID())
				pHouse->dwOwnerCharIndex = pPlayer->GetID();

			// 시작위치로 이동
			CCharMove::SetPosition(pPlayer, &pHouse->vStartPos);
			CCharMove::CorrectPlayerPosToServer(pPlayer);

			// 같은 채널에 있는 유저에게통보
			MSG_DWORD msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_NOTIFY_VISIT;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData = pHouse->HouseInfo.dwTotalVisitCount;
			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );

			return TRUE;
		}
	}
	return FALSE;
}

void HousingMgr::ExitHouse(DWORD dwCharIndex, BOOL bVisitHouse)
{
	CObject* pObject = g_pUserTable->FindUser(dwCharIndex);
	if(pObject && pObject->GetObjectKind() == eObjectKind_Player)
	{
		CPlayer* pPlayer = (CPlayer*)pObject;
		DWORD dwChannel = pPlayer->GetChannelID();
		stHouse* pHouse = GetHouseByChannelID(dwChannel);
		if(pHouse)
		{
			if(pHouse->dwOwnerCharIndex == dwCharIndex)
				pHouse->dwOwnerCharIndex = 0;

			// 꾸미기 모드중 퇴장이면 꾸미기모드해제
			if(IsDecoMode(pHouse->HouseInfo.dwOwnerUserIndex, dwCharIndex))
			{
				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_Housing, 0);

				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_DECOMODE_ACK;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = 0;	// DecoMode Off
				PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
			}

			// 탑승중 퇴장이면 탑승해제
			stHouseRiderInfo* pRider = m_RiderList.GetData(pPlayer->GetID());
			if(pRider)
			{
				stFurniture* pFurniture = pHouse->pFurnitureList.GetData(pRider->dwFurnitureObjectIndex);
				if(pFurniture)
				{
					OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_HouseRiding, 0);
					pFurniture->dwRidingPlayer[pRider->wSlot] = 0;
					
					m_RiderList.Remove(pRider->dwPlayerIndex);
					m_RiderPool->Free(pRider);

					// 통보
					MSG_DWORD5 msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_ACTION_ACK;
					msg.dwObjectID = pPlayer->GetID();
					msg.dwData1 = pFurniture->dwObjectIndex;
					msg.dwData2 = pRider->wSlot;
					msg.dwData3 = eHOUSE_ACTION_TYPE_GETOFF;
					msg.dwData4 = (DWORD)pFurniture->vPosition.x;
					msg.dwData5 = (DWORD)pFurniture->vPosition.z;
					PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
				}
			}

			if(bVisitHouse)
			{
				// 같은 채널에 있는 유저에게통보
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_NOTIFY_EXIT;
				msg.dwObjectID = pPlayer->GetID();
				msg.dwData = 0;
				PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
			}

			if(pHouse->dwJoinPlayerNum <= 1)
				DeleteHouse(pHouse->HouseInfo.dwOwnerUserIndex);
			else
				pHouse->dwJoinPlayerNum--;

		}
	}
}

DWORD HousingMgr::GeneraterFurnitureID()
{
	DWORD id = m_FurnitureIDGenerator.GenerateIndex();
	ASSERT(id != 0);
	return id;
}

void HousingMgr::ReleaseFurnitureID(DWORD id)
{
	m_FurnitureIDGenerator.ReleaseIndex(id);
}

int HousingMgr::CheckExtendHouse(DWORD dwCharIndex)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwCharIndex);
	if(!pPlayer)
		return eHOUSEERR_ERROR;

	// 하우스소유주가 아님
	stHouse* pHouse = GetHouseByChannelID(pPlayer->GetChannelID());
	if(!pHouse || pHouse->HouseInfo.dwOwnerUserIndex!=pPlayer->GetUserID())
		return eHOUSEERR_NOTOWNER;

	// 하우스최대확장 상태
	if(pHouse->HouseInfo.ExtendLevel >= eHOUSEEXTEND_LV3)
		return eHOUSEERR_MAX_EXTEND;

	// 꾸미기상태에서 불가능
	if(IsDecoMode(pHouse->HouseInfo.dwOwnerUserIndex, dwCharIndex))
		return eHOUSEERR_DECOMODE;

	return eHOUSEERR_NONE;
}

void HousingMgr::ExtendHouse(DWORD dwCharIndex, DWORD dwItemDBIndex, DWORD dwItemIndex, const ITEM_INFO* pItemInfo)
{
	if(!pItemInfo)
		return;

	// 확장시 기존기본설치가구목록과 카테고리 '문'에 해당하는 가구를 설치해제한다.
	// 클라이언트에 설치해제 목록을 보내주지않으므로(네트웍트래픽문제 염려)
	// 클라이언트에서 해당패킷을 받을 때 설치해제한다.

	// 단, 신규기본설치가구목록은 ObjectIndex가 생성되므로 목록을 전송한다.

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwCharIndex);
	if(!pPlayer)
		return;

	stHouse* pHouse = GetHouseByChannelID(pPlayer->GetChannelID());
	if(!pHouse)
		return;

	if(pHouse->HouseInfo.dwOwnerUserIndex != pPlayer->GetUserID())
		return;

	stHousingSettingInfo* pSettingInfo = HOUSINGMGR->GetHousingSettingInfo();
	if(!pSettingInfo)
		return;

	BYTE CurExtendLevel = pHouse->HouseInfo.ExtendLevel;

	WORD wCnt;
	MSG_HOUSE_EXTEND msg;
	msg.Category = MP_HOUSE;
	msg.Protocol = MP_HOUSE_EXTEND;
	msg.wExtendLevel = CurExtendLevel + 1;

	stFurniture* pFurniture;

	// 기존 기본설치가구목록 제거
	pHouse->pNotDeleteFurnitureList.SetPositionHead();
	while((pFurniture = pHouse->pNotDeleteFurnitureList.GetData())!= NULL)
	{
		// DB갱신
		HouseFurnitureUpdate(0, eHOUSEFURNITURE_DEL, pFurniture->dwOwnerUserIndex, pFurniture->dwFurnitureIndex, pFurniture->dwLinkItemIndex, 0, 
						0, 0, 0, 0,
						pFurniture->wSlot, eHOUSEFURNITURE_STATE_UNINSTALL, 0);
		RemoveFurniture(pFurniture->dwObjectIndex, pHouse, TRUE);
	}
	pHouse->pNotDeleteFurnitureList.RemoveAll();
	pHouse->dwCurDoorIndex = 0;

	// 문 카테고리에 해당하는 가구 설치해제
	wCnt=0;
	pHouse->pFurnitureList.SetPositionHead();
	while((pFurniture = pHouse->pFurnitureList.GetData())!= NULL)
	{
		if(eHOUSE_HighCategory_Door == Get_HighCategory(pFurniture->dwFurnitureIndex) &&
			eHOUSEFURNITURE_STATE_INSTALL == pFurniture->wState)
		{
			// DB갱신
			HouseFurnitureUpdate(0, eHOUSEFURNITURE_UPDATE, pFurniture->dwOwnerUserIndex, pFurniture->dwFurnitureIndex, pFurniture->dwLinkItemIndex, 0, 
							0, 0, 0, 0,
							pFurniture->wSlot, eHOUSEFURNITURE_STATE_UNINSTALL, 0);

			pFurniture->wState = eHOUSEFURNITURE_STATE_UNINSTALL;
			UpdateFurniture(pFurniture, pHouse);

			memcpy(&msg.UnInstall[wCnt], pFurniture, sizeof(stFurniture));
			wCnt++;
		}
	}
	msg.wUnInstallNum = wCnt;

	// 확장된 기본가구 추가
	wCnt = 0;
	DWORD i;
	BYTE Slot;
	for(i=0; i<pSettingInfo->dwGeneralFunitureNumArr[CurExtendLevel + 1]; i++)
	{
		stGeneralFunitureInfo* pInitFurnitureInfo = &pSettingInfo->pGeneralFunitureList[CurExtendLevel + 1][i];

		if(!pInitFurnitureInfo)
			continue;

		DWORD dwFurnitureIndex = HOUSINGMGR->GetFurnitureIndexFromItemIndex(pInitFurnitureInfo->dwItemIndex);
		DWORD dwHiCategory = Get_HighCategory(dwFurnitureIndex);

		Slot = (BYTE)GetEmptySlot(pHouse, dwHiCategory);
		if(-1 < Slot)
		{
			// DB에 처음목록추가
			DWORD dwEndTime = HOUSINGMGR->GetRemainTimeFromItemIndex(pInitFurnitureInfo->dwItemIndex);
			HouseFurnitureUpdate(pPlayer->GetID(), eHOUSEFURNITURE_ADD, pPlayer->GetUserID(), dwFurnitureIndex, pInitFurnitureInfo->dwItemIndex, 0, 
				pInitFurnitureInfo->vWorldPos.x, pInitFurnitureInfo->vWorldPos.y, pInitFurnitureInfo->vWorldPos.z, pInitFurnitureInfo->fAngle,
				Slot, eHOUSEFURNITURE_STATE_INSTALL, dwEndTime, TRUE);

			stFurniture* pFurniture = HOUSINGMGR->CreateNewFurniture();
			if(pFurniture)
			{
				pFurniture->dwOwnerUserIndex = pPlayer->GetUserID();
				pFurniture->dwFurnitureIndex = dwFurnitureIndex;
				pFurniture->dwLinkItemIndex = pInitFurnitureInfo->dwItemIndex;
				pFurniture->nMaterialIndex = 0;
				pFurniture->vPosition.x = pInitFurnitureInfo->vWorldPos.x;
				pFurniture->vPosition.y = pInitFurnitureInfo->vWorldPos.y;
				pFurniture->vPosition.z = pInitFurnitureInfo->vWorldPos.z;
				pFurniture->fAngle = pInitFurnitureInfo->fAngle;
				pFurniture->wCategory = (BYTE)dwHiCategory;
				pFurniture->wSlot = Slot;
				pFurniture->wState = eHOUSEFURNITURE_STATE_INSTALL;
				pFurniture->bNotDelete = TRUE;
				pFurniture->dwRemainTime = dwEndTime;

				HOUSINGMGR->AddFurnitureToHouse(pFurniture, pHouse);

				memcpy(&msg.Install[wCnt], pFurniture, sizeof(stFurniture));
				wCnt++;
			}
		}
	}
	msg.wInstallNum = wCnt;
	msg.dwDecoPoint = pHouse->dwDecoPoint;

	pHouse->HouseInfo.ExtendLevel = CurExtendLevel + 1;
	HouseUpdate(&pHouse->HouseInfo);


	// 하우스 확장 로그
	Log_Housing(pPlayer->GetUserID(), pPlayer->GetObjectName(), eHouseLog_Extend, dwItemDBIndex, dwItemIndex, pItemInfo->SupplyValue,
				0, 0, pHouse->HouseInfo.szHouseName, pHouse->dwDecoPoint, pHouse->HouseInfo.dwHousePoint);


	// 같은 채널에 있는 유저에게통보
	PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
}

BOOL HousingMgr::ChangeHouseName(DWORD dwCharIndex, char* pHouseName)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwCharIndex);
	if(!pPlayer)
		return FALSE;

	stHouse* pHouse = GetHouseByChannelID(pPlayer->GetChannelID());
	if(!pHouse)
		return FALSE;

	if(pHouse->HouseInfo.dwOwnerUserIndex != pPlayer->GetUserID())
		return FALSE;

	if(IsCharInString(pHouseName, "'"))
 		return FALSE;

	SafeStrCpy(pHouse->HouseInfo.szHouseName, pHouseName, MAX_HOUSING_NAME_LENGTH+1);
	HouseUpdate(&pHouse->HouseInfo);

	// 하우스 이름변경 로그
	Log_Housing(pPlayer->GetUserID(), pPlayer->GetObjectName(), eHouseLog_ChangeHouseName, 0/*dwItemDBIndex*/, 0/*dwItemIndex*/, 0/*pItemInfo->SupplyValue*/,
				0, 0, pHouse->HouseInfo.szHouseName, pHouse->dwDecoPoint, pHouse->HouseInfo.dwHousePoint);

	return TRUE;
}

VECTOR3* HousingMgr::GetStartPos(stHouse* pHouse)
{
	if(pHouse)
		return &pHouse->vStartPos;

	return NULL;
}

stFurniture* HousingMgr::CreateNewFurniture()
{
	stFurniture* pNewFurniture = m_FurniturePool->Alloc();
	pNewFurniture->Clear();
	return pNewFurniture;
}

BOOL HousingMgr::IsDecoMode(DWORD dwOwnerUserIndex, DWORD dwCharIndex)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwCharIndex);
	if(pPlayer)
	{
		if(pPlayer->GetState() == eObjectState_Housing)
			return TRUE;
	}

	return FALSE;
}

BOOL HousingMgr::CanDecoMode(DWORD dwOwnerUserIndex, DWORD dwCharIndex)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwCharIndex);
	if(pPlayer)
	{
		stHouse* pHouse = m_HouseList.GetData(dwOwnerUserIndex);
		if(pHouse->HouseInfo.dwOwnerUserIndex == pPlayer->GetUserID())
		{
			if(pPlayer->GetState()!=eObjectState_Die				&&
				pPlayer->GetState()!=eObjectState_Deal				&&
				pPlayer->GetState()!=eObjectState_Exchange			&&
				pPlayer->GetState()!=eObjectState_StreetStall_Owner &&
				pPlayer->GetState()!=eObjectState_StreetStall_Guest &&
				pPlayer->GetState()!=eObjectState_Society			&&
				pPlayer->GetState()!=eObjectState_Cooking			&&
				pPlayer->GetState()!=eObjectState_Housing			&&
				pPlayer->GetState()!=eObjectState_HouseRiding
				)
				return TRUE;
		}
	}

	return TRUE;
}

BOOL HousingMgr::IsInstall(DWORD dwOwnerUserIndex, DWORD dwObjectIndex)
{
	stHouse* pHouse = m_HouseList.GetData(dwOwnerUserIndex);
	if(pHouse)
	{
		stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwObjectIndex);
		if(pFurniture && pFurniture->wState==eHOUSEFURNITURE_STATE_INSTALL)
			return TRUE;
	}

	return FALSE;
}

BOOL HousingMgr::CanInstall(DWORD dwOwnerUserIndex, DWORD dwObjectIndex)
{
	stHouse* pHouse = m_HouseList.GetData(dwOwnerUserIndex);
	if(pHouse)
	{
		stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwObjectIndex);
		if(pFurniture &&
			(pFurniture->wState==eHOUSEFURNITURE_STATE_KEEP ||
			pFurniture->wState==eHOUSEFURNITURE_STATE_UNINSTALL))
			return TRUE;
	}

	return FALSE;
}

BOOL HousingMgr::CanUpdate(DWORD dwOwnerUserIndex, DWORD dwObjectIndex)
{
	stHouse* pHouse = m_HouseList.GetData(dwOwnerUserIndex);
	if(pHouse)
	{
		stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwObjectIndex);
		if(pFurniture && pFurniture->wState==eHOUSEFURNITURE_STATE_INSTALL)
			return TRUE;
	}

	return FALSE;
}

BOOL HousingMgr::IsKeeping(DWORD dwOwnerUserIndex, DWORD dwObjectIndex)
{
	stHouse* pHouse = m_HouseList.GetData(dwOwnerUserIndex);
	if(pHouse)
	{
		stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwObjectIndex);
		if(pFurniture && pFurniture->wState==eHOUSEFURNITURE_STATE_KEEP)
			return TRUE;
	}

	return FALSE;
}

BOOL HousingMgr::CanKeeping(stHouse* pHouse, DWORD dwItemIndex, BYTE bySlot)
{
	if(!pHouse)
		return FALSE;

	DWORD dwFurnitureIndex = GetFurnitureIndexFromItemIndex(dwItemIndex);

	// 빈슬롯 있는지 검사
	DWORD dwHighCategory = Get_HighCategory(dwFurnitureIndex);
	if(pHouse->m_dwCategoryNum[dwHighCategory] >= MAX_HOUSING_SLOT_NUM)
		return FALSE;

	// 해당슬롯 검사
	if(pHouse->m_dwFurnitureList[dwHighCategory][bySlot])
		return FALSE;

	return TRUE;
}

BOOL HousingMgr::CanDestroy(DWORD dwOwnerUserIndex, DWORD dwObjectIndex)
{
	stHouse* pHouse = m_HouseList.GetData(dwOwnerUserIndex);
	if(pHouse)
	{
		stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwObjectIndex);
		if(pFurniture && !pFurniture->bNotDelete &&
			(pFurniture->wState==eHOUSEFURNITURE_STATE_KEEP || pFurniture->wState==eHOUSEFURNITURE_STATE_UNINSTALL))
			return TRUE;
	}

	return FALSE;
}
stFurniture* HousingMgr::GetFurniture(DWORD dwOwnerIndex, DWORD dwObjectIndex)
{
	stHouse* pHouse = m_HouseList.GetData(dwOwnerIndex);
	if(pHouse)
		return pHouse->pFurnitureList.GetData(dwObjectIndex);

	return NULL;
}

BOOL HousingMgr::ChangeMaterial(DWORD dwCharIndex, DWORD dwItemDBIndex, DWORD dwItemIndex, const ITEM_INFO* pItemInfo)
{
	if(!pItemInfo)
		return FALSE;

	const WORD wMtrlIndex = (WORD)pItemInfo->SupplyValue;

	// 머트리얼변경은 기본설치가구만 가능하다 (pHouse->pNotDeleteFurnitureList에 등록되어 있음)
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwCharIndex);
	if(!pPlayer)
		return FALSE;

	stHouse* pHouse = GetHouseByChannelID(pPlayer->GetChannelID());
	if(!pHouse)
		return FALSE;

	if(wMtrlIndex>10)
		return FALSE;

	CHAR cLogType = 0;
	BOOL bChange;
	stFurniture* pFurniture;
	pHouse->pNotDeleteFurnitureList.SetPositionHead();
	while((pFurniture = pHouse->pNotDeleteFurnitureList.GetData())!= NULL)
	{
		bChange = FALSE;

		if(ITEM_KIND_FURNITURE_WALLPAPER== pItemInfo->SupplyType &&
			IsExteriorWall(pFurniture->dwFurnitureIndex))
		{
			cLogType = eHouseLog_WallPaper;
			bChange = TRUE;
		}
		else if(ITEM_KIND_FURNITURE_FLOORPAPER== pItemInfo->SupplyType &&
			IsFloor(pFurniture->dwFurnitureIndex))
		{
			cLogType = eHouseLog_FloorPaper;
			bChange = TRUE;
		}
		else if(ITEM_KIND_FURNITURE_CEILINGPAPER== pItemInfo->SupplyType &&
			IsCeiling(pFurniture->dwFurnitureIndex))
		{
			cLogType = eHouseLog_CeillingPaper;
			bChange = TRUE;
		}

		if(bChange)
		{
			pFurniture->nMaterialIndex = (int)wMtrlIndex;

			// DB갱신
			HouseFurnitureUpdate(pPlayer->GetID(), eHOUSEFURNITURE_UPDATE, pPlayer->GetUserID(), pFurniture->dwFurnitureIndex, pFurniture->dwLinkItemIndex,(BYTE) pFurniture->nMaterialIndex, 
				pFurniture->vPosition.x, pFurniture->vPosition.y, pFurniture->vPosition.z, pFurniture->fAngle,
				(BYTE) pFurniture->wSlot , eHOUSEFURNITURE_STATE_INSTALL, 0);

			UpdateFurniture(pFurniture, pHouse);

			// 머터리얼변경(벽지) 로그
			Log_Housing(pPlayer->GetUserID(), pPlayer->GetObjectName(), cLogType, dwItemDBIndex, dwItemIndex, pItemInfo->SupplyValue,
						pFurniture->dwFurnitureIndex, pFurniture->wSlot, pHouse->HouseInfo.szHouseName, pHouse->dwDecoPoint, pHouse->HouseInfo.dwHousePoint);

			// 같은 채널에 있는 유저에게통보
			MSG_DWORD2	 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_UPDATE_MATERIAL_ACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData1	 = pFurniture->dwObjectIndex ;
			msg.dwData2	 = (DWORD)pFurniture->nMaterialIndex ;
			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
		}
	}

	return TRUE;
}

DWORD HousingMgr::GetDecoPoint(stFurniture* pFurniture)
{
	if(!pFurniture)
		return 0;

	stFunitureInfo* pInfo = GAMERESRCMNGR->GetFunitureInfo(pFurniture->dwFurnitureIndex);
	if(!pInfo)
		return 0;

	return pInfo->dwDecoPoint;
}

void HousingMgr::UseDecoBonus(DWORD dwChannelID, DWORD dwPlayerID, DWORD dwBonusIndex)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerID);
	if(!pPlayer)
		return;

	DWORD dwErr = eHOUSEERR_NONE;

	if(pPlayer->IsPKMode() || LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) || pPlayer->GetAutoNoteIdx())
		dwErr = eHOUSEERR_INVAILDSTATE;

	stHouse* pHouse = GetHouseByChannelID(dwChannelID);
	if(!pHouse || pHouse->HouseInfo.dwOwnerUserIndex!=pPlayer->GetUserID())
		dwErr = eHOUSEERR_NOTOWNER;

	if(IsDecoMode(pHouse->HouseInfo.dwOwnerUserIndex, pPlayer->GetID()))
		dwErr = eHOUSEERR_DECOMODE;

	stHouseBonusInfo* pBonusInfo = GAMERESRCMNGR->GetHouseBonusInfo(dwBonusIndex);
	if(pBonusInfo)
	{
		//090615 pdy 하우징 서버에서도 랭커채크 추가 
		if( 1 == pBonusInfo->byKind )									//랭커전용 보너스 일때 일반유저는 사용금지 
		{
			if( pPlayer->GetUserID() != m_HouseRank.dwRank_1_UserIndex &&
				pPlayer->GetUserID() != m_HouseRank.dwRank_2_UserIndex && 
				pPlayer->GetUserID() != m_HouseRank.dwRank_3_UserIndex	)		//요청한 유저ID가 1,2,3등이 아니라면 실패 
				dwErr = eHOUSEERR_NORANKER;
		}

		// 사용가능한 포인트가 작다.
		if(pHouse->HouseInfo.dwDecoUsePoint < pBonusInfo->dwUsePoint)
			dwErr = eHOUSEERR_NOTENOUGHPOINT;

		if(pHouse->dwDecoPoint < pBonusInfo->dwDecoPoint)
			dwErr = eHOUSEERR_NOTENOUGHPOINT;

		if(eHOUSEERR_NONE != dwErr)
		{
			// 실패 통보
			MSG_DWORD2 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_BONUS_NACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData1 = dwBonusIndex;
			msg.dwData2 = dwErr;
			pPlayer->SendMsg( &msg, sizeof( msg ) );
			return;
		}

		if(0 == pBonusInfo->dwBonusType)
		{
			if(const cActiveSkillInfo* const info = SKILLMGR->GetActiveInfo( pBonusInfo->dwBonusTypeValue ))
			{
				if(0 == pBonusInfo->byTargetType)
				{
					// 집주인만 버프
					SKILLMGR->AddBuffSkill( *pPlayer, info->GetInfo());
				}
				else if(1 == pBonusInfo->byTargetType)
				{
					// 방문자모두에게 버프
					CObject* pObject;
					g_pUserTable->SetPositionHead();
					while( (pObject = g_pUserTable->GetData()) != NULL)
					{
						if(pObject && pObject->GetObjectKind() == eObjectKind_Player)
						{
							CPlayer* pTarget = (CPlayer*)pObject;
							if(pTarget->GetChannelID() == pHouse->dwChannelID)
							{
								SKILLMGR->AddBuffSkill( *pTarget, info->GetInfo());
							}
						}
					}
				}
			}
		}
		else if(1 == pBonusInfo->dwBonusType)
		{
			MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( (WORD)pBonusInfo->dwBonusTypeValue );
			if( !ChangeInfo ) return;
			SaveMapChangePointUpdate( pPlayer->GetID(), ChangeInfo->Kind );

			// RaMa - 04.12.14
			pPlayer->UpdateLogoutToDB(FALSE);

			VECTOR3 RandPos;
			int temp;
			temp = rand() % 500 - 250;
			RandPos.x = ChangeInfo->MovePoint.x + temp;
			temp = rand() % 500 - 250;
			RandPos.z = ChangeInfo->MovePoint.z + temp;
			RandPos.y = 0;

			pPlayer->SetMapMoveInfo( ChangeInfo->MoveMapNum, (DWORD)RandPos.x, (DWORD)RandPos.z);

			// 081218 LUJ, 플레이어를 해제하기 전에 DB에 업데이트할 주요 정보를 복사해놓는다
			const MAPTYPE	mapType			= ChangeInfo->MoveMapNum;
			const WORD		channelIndex	= 0; //무조건 1번채널
			const DWORD		playerIndex		= pPlayer->GetID();
			const DWORD		userIndex		= pPlayer->GetUserID();
			const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
			const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );

			g_pServerSystem->RemovePlayer( pPlayer->GetID() );

			// 081218 LUJ, 업데이트보다 맵 이동이 빨리 진행될 경우 업데이트되지 않은 정보가 오게된다.
			//			이를 막기 위해 프로시저 처리 완료 후 진행한다
			UpdatePlayerOnMapOut(
				playerIndex,
				userIndex,
				inventoryMoney,
				storageMoney,
				mapType,
				channelIndex );
		}
		else if(2 == pBonusInfo->dwBonusType)
		{
			// 아이템 지급
			if(0 == pBonusInfo->byTargetType)
			{
				// 집주인만 지급
				ITEMMGR->ObtainGeneralItem(pPlayer, pBonusInfo->dwBonusTypeValue, 1, eLog_ItemObtainHouseBonus, MP_ITEM_HOUSE_BONUS);
			}
			else if(1 == pBonusInfo->byTargetType)
			{
				// 방문자모두에게 지급
				CObject* pObject;
				g_pUserTable->SetPositionHead();
				while( (pObject = g_pUserTable->GetData()) != NULL)
				{
					if(pObject && pObject->GetObjectKind() == eObjectKind_Player)
					{
						CPlayer* pTarget = (CPlayer*)pObject;
						if(pTarget->GetChannelID() == pHouse->dwChannelID)
						{
							ITEMMGR->ObtainGeneralItem(pTarget, pBonusInfo->dwBonusTypeValue, 1, eLog_ItemObtainHouseBonus, MP_ITEM_HOUSE_BONUS);
						}
					}
				}
			}
		}

		// 사용한 포인트차감
		pHouse->HouseInfo.dwDecoUsePoint -= pBonusInfo->dwUsePoint;
		HouseUpdate(&pHouse->HouseInfo);

		// 통보
		MSG_DWORD2 msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_BONUS_ACK;
		msg.dwObjectID = pPlayer->GetID();
		msg.dwData1 = dwBonusIndex;
		msg.dwData2 = pHouse->HouseInfo.dwDecoUsePoint;
		PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
	}
}

void HousingMgr::AddFurnitureToHouse(stFurniture* pFurniture, stHouse* pHouse)
{
	if(!pFurniture || !pHouse)
		return;

	pFurniture->dwObjectIndex = GeneraterFurnitureID();
	if(0==pFurniture->dwObjectIndex)
		return;

	if(IsStart(pFurniture->dwFurnitureIndex))
	{
		pHouse->vStartPos.x = pFurniture->vPosition.x;
		pHouse->vStartPos.y = pFurniture->vPosition.y;
		pHouse->vStartPos.z = pFurniture->vPosition.z;
	}

	DWORD dwHiCategory = Get_HighCategory(pFurniture->dwFurnitureIndex);
	WORD wSlot = pFurniture->wSlot;
	pHouse->m_dwFurnitureList[dwHiCategory][wSlot] = pFurniture->dwObjectIndex;
	pHouse->pFurnitureList.Add(pFurniture, pFurniture->dwObjectIndex);
	pHouse->m_dwCategoryNum[dwHiCategory]++;

	// 기본설치물목록에 추가
	if(pFurniture->bNotDelete)
		pHouse->pNotDeleteFurnitureList.Add(pFurniture, pFurniture->dwObjectIndex);

	// 설치상태 가구 처리
	if(eHOUSEFURNITURE_STATE_INSTALL == pFurniture->wState)
	{
		// 꾸미기포인트 추가
		pHouse->dwDecoPoint += GetDecoPoint(pFurniture);

		// 현재 설치된 문 등록
		if(IsDoor(pFurniture->dwFurnitureIndex))
		{
			if(0 != pHouse->dwCurDoorIndex)
			{
				// 헉!! 문이 두개닷!!!
				__asm int 3;
			}

			pHouse->dwCurDoorIndex = pFurniture->dwObjectIndex;
		}
	}
}

void HousingMgr::AddNewFurniture(stFurniture* pFurniture, stHouse* pHouse)
{
	if(!pFurniture || !pHouse)
		return;

	pFurniture->dwObjectIndex = GeneraterFurnitureID();
	if(0==pFurniture->dwObjectIndex)
		return;

	pFurniture->dwOwnerUserIndex = pHouse->HouseInfo.dwOwnerUserIndex;
	pFurniture->fAngle = 0.0f;
	pFurniture->nMaterialIndex = 0;
	pFurniture->vPosition.x = 0.0f;
	pFurniture->vPosition.y = 0.0f;
	pFurniture->vPosition.z = 0.0f;
	pFurniture->wCategory = (BYTE)Get_HighCategory(pFurniture->dwFurnitureIndex);

	AddFurnitureToHouse(pFurniture, pHouse);
}

void HousingMgr::UpdateFurniture(stFurniture* pFurniture, stHouse* pHouse, BOOL bAddDecoPoint)
{
	stFurniture* pOldFurniture = NULL;
	pOldFurniture = pHouse->pFurnitureList.GetData(pFurniture->dwObjectIndex);
	if(pOldFurniture)
		memcpy(pOldFurniture, pFurniture, sizeof(stFurniture));

	if(eHOUSEFURNITURE_STATE_INSTALL == pFurniture->wState)
	{
		if(bAddDecoPoint)
		{
			pHouse->dwDecoPoint += GetDecoPoint(pFurniture);

			if(pHouse->dwDecoPoint >= 100)
			{
				WebEvent( pHouse->HouseInfo.dwOwnerUserIndex, 7 );
			}
		}

		if(IsStart(pFurniture->dwFurnitureIndex))
		{
			// 시작위치인 경우 하우스정보도 갱신해주자.
			pHouse->vStartPos = pFurniture->vPosition;
		}

		//090603 pdy 가구탑승시 버그수정 (업데이트되었을시 탑승캐릭터 위치보정)
		int i;
		stHouseRiderInfo* pRider;		
		for(i=0; i<MAX_HOUSE_ATTATCH_SLOT; i++)
		{
			pRider = m_RiderList.GetData(pFurniture->dwRidingPlayer[i]);
			if(!pRider)
				continue;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pRider->dwPlayerIndex);
			if(!pPlayer)
				continue;

			VECTOR3 v2DPosition = pFurniture->vPosition;
			v2DPosition.y = 0.0f;

			//탑승 캐릭터 위치 보정
			CCharMove::SetPosition( pPlayer, &v2DPosition );
			CCharMove::CorrectPlayerPosToServer(pPlayer);
		}
		//--
	}
	else if(eHOUSEFURNITURE_STATE_UNINSTALL == pFurniture->wState)
	{
		pHouse->dwDecoPoint -= GetDecoPoint(pFurniture);

		int i;
		stHouseRiderInfo* pRider;		
		for(i=0; i<MAX_HOUSE_ATTATCH_SLOT; i++)
		{
			pRider = m_RiderList.GetData(pFurniture->dwRidingPlayer[i]);
			if(!pRider)
				continue;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pRider->dwPlayerIndex);
			if(!pPlayer)
				continue;

			OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_HouseRiding, 0);
			pFurniture->dwRidingPlayer[i] = 0;
			m_RiderList.Remove(pPlayer->GetID());
			m_RiderPool->Free(pRider);

			// 통보
			MSG_DWORD5 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ACTION_ACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData1 = pFurniture->dwObjectIndex;
			msg.dwData2 = pRider->wSlot;
			msg.dwData3 = HOUSE_ACTIONINDEX_GETOFF;
			msg.dwData4 = (DWORD)pFurniture->vPosition.x;
			msg.dwData5 = (DWORD)pFurniture->vPosition.z;
			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
		}

		pFurniture->fAngle = 0.0f;
		pFurniture->vPosition.x = 0.0f;
		pFurniture->vPosition.y = 0.0f;
		pFurniture->vPosition.z = 0.0f;
	}
}

void HousingMgr::RemoveFurniture(DWORD dwObjectIndex, stHouse* pHouse, BOOL bExtend)
{
	if(!bExtend)
	{
		stFurniture* pNotDeleteFurniture = pHouse->pNotDeleteFurnitureList.GetData(dwObjectIndex);
		if(pNotDeleteFurniture)
		{
			// 지워지면 안되는 가구!!
			__asm int 3;
		}
	}

	stFurniture* pFurniture = pHouse->pFurnitureList.GetData(dwObjectIndex);
	if(pFurniture)
	{
		DWORD dwHiCategory = Get_HighCategory(pFurniture->dwFurnitureIndex);
		pHouse->m_dwFurnitureList[dwHiCategory][pFurniture->wSlot] = 0;
		pHouse->m_dwCategoryNum[dwHiCategory]--;

		if(eHOUSEFURNITURE_STATE_INSTALL==pFurniture->wState)
			pHouse->dwDecoPoint -= GetDecoPoint(pFurniture);

		int i;
		stHouseRiderInfo* pRider;		
		for(i=0; i<MAX_HOUSE_ATTATCH_SLOT; i++)
		{
			pRider = m_RiderList.GetData(pFurniture->dwRidingPlayer[i]);
			if(!pRider)
				continue;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pRider->dwPlayerIndex);
			if(!pPlayer)
				continue;

			OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_HouseRiding, 0);
			pFurniture->dwRidingPlayer[i] = 0;
			m_RiderList.Remove(pPlayer->GetID());
			m_RiderPool->Free(pRider);

			// 통보
			MSG_DWORD5 msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ACTION_ACK;
			msg.dwObjectID = pPlayer->GetID();
			msg.dwData1 = pFurniture->dwObjectIndex;
			msg.dwData2 = pRider->wSlot;
			msg.dwData3 = HOUSE_ACTIONINDEX_GETOFF;
			msg.dwData4 = (DWORD)pFurniture->vPosition.x;
			msg.dwData5 = (DWORD)pFurniture->vPosition.z;
			PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
		}

		RemoveFurnitureFromPool(pFurniture);
		pHouse->pFurnitureList.Remove(dwObjectIndex);
	}
}

void HousingMgr::RemoveFurnitureFromPool(stFurniture* pFurniture)
{
	if(pFurniture)
		m_FurniturePool->Free(pFurniture);
}

int HousingMgr::GetEmptySlot(stHouse* pHouse, DWORD dwHighCategory)
{
	if(!pHouse)
		return -1;

	if(dwHighCategory<0 || dwHighCategory>MAX_HOUSING_CATEGORY_NUM)
		return -1;

	int i;
	for(i=0; i<MAX_HOUSING_SLOT_NUM; i++)
	{
		if(0 == pHouse->m_dwFurnitureList[dwHighCategory][i])
			return i;
	}

	return -1;
}

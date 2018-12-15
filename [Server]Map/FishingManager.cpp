#include "FishingManager.h"
#include "MHFile.h"
#include "ChannelSystem.h"
#include "AISystem.h"
#include "Npc.h"
#include "Player.h"
#include "../[CC]Header/GameResourceManager.h"
#include "UserTable.h"
#include "PackedData.h"
#include "ItemManager.h"
#include "WeatherManager.h"
#include "MapDBMsgParser.h"

DWORD g_FishingMissionCode[MAX_FISHINGMISSIONCODE] = {0,};
std::map<DWORD, stFishingMissionInfo> g_mapMissionInfo;

GLOBALTON(CFishingManager)
CFishingManager::CFishingManager(void)
{
	m_bInit = FALSE;
	m_bActive = FALSE;
	m_bUseMission = FALSE;
	m_nChannelNum = 0;
	m_nFishingPlaceNum = 0;

	m_cMissionState = eFISHMISSION_STATE_READY;
	m_dwMissionSendTime = FISHINGTIME_STARTPULLING;
	m_dwMissionDelayTime = 0;
	m_dwMissionEndTime = 0;

	m_FishingGabagePlace.Initialize(100);
}

CFishingManager::~CFishingManager(void)
{
	std::map<DWORD, stFishingRate*>::iterator iter;
	for(iter=m_mapUtilRateList.begin(); iter!=m_mapUtilRateList.end(); iter++)
	{
		if(iter->second)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}

	for(iter=m_mapBaitRateList.begin(); iter!=m_mapBaitRateList.end(); iter++)
	{
		if(iter->second)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
}

void CFishingManager::Init()
{
	CMHFile file;
	char filename[256] = {0,};
	// 낚시터 스크립트파일 존재하는지 확인 (로딩은 아래서 다시한다)
	sprintf(filename,"system/Resource/fishing_%02d.bin", g_pServerSystem->GetMapNum());
	if( ! file.Init( filename, "rb" ) )
		return;

	int i, j;
	m_nChannelNum = (int)CHANNELSYSTEM->GetChannelCount();
	
	for(i=0; i<MAX_FISHINGPLACE; i++)
		m_FishingPlaceInfo[i].Clear();

	ZeroMemory(m_FishingPlaceInst, sizeof(m_FishingPlaceInst));



	int nCount = 0;				// nCount==0 이거나
	BOOL bError = FALSE;		// bError==TRUE 이거나
	BOOL bOpenBlock = FALSE;	// bOpenBlock==TRUE 인채로 종료되면 비정상이므로 시스템은 구동되지 않는다.
	char string[256] = {0,};


	// 아이템 추가확률 스크립트파일 로딩
	char cItemType = eFISHINGRATE_ITEM_UNKNOWN;
	int  nItemCnt = 0;
	stFishingRate LoadRateData;

	sprintf(filename,"system/Resource/FishingRateDefine.bin");

	if( file.Init( filename, "rb" ) )
	{
		while(!file.IsEOF())
		{
			file.GetString(string);

			if(string[0] == '@')
			{
				file.GetLineX(string, 256);
				continue;
			}
			else if(string[0] == '{')
			{
				if(!bOpenBlock)
				{
					LoadRateData.Clear();
					bOpenBlock = TRUE;
					nItemCnt = 0;
				}
				else
				{
					bError = TRUE;
					break;
				}
				continue;
			}
			else if(string[0] == '}')
			{
				if(bOpenBlock)
				{
					//저장
					bOpenBlock = FALSE;
					if(cItemType == eFISHINGRATE_ITEM_UTILITY)
					{
						stFishingRate* pAddData = new stFishingRate;
						if(pAddData)
						{
							pAddData->dwItemIndex = LoadRateData.dwItemIndex;
							memcpy(pAddData->FishList, LoadRateData.FishList, sizeof(LoadRateData.FishList));

							std::pair< std::map<DWORD, stFishingRate*>::iterator, bool > pr;
							pr = m_mapUtilRateList.insert(std::make_pair(pAddData->dwItemIndex, pAddData));
							if(pr.second == FALSE)
							{
								delete pAddData;
							}
						}
					}
					else if(cItemType == eFISHINGRATE_ITEM_BAIT)
					{
						stFishingRate* pAddData = new stFishingRate;
						if(pAddData)
						{
							pAddData->dwItemIndex = LoadRateData.dwItemIndex;
							memcpy(pAddData->FishList, LoadRateData.FishList, sizeof(LoadRateData.FishList));

							std::pair< std::map<DWORD, stFishingRate*>::iterator, bool > pr;
							pr = m_mapBaitRateList.insert(std::make_pair(pAddData->dwItemIndex, pAddData));
							if(pr.second == FALSE)
							{
								delete pAddData;
							}
						}
					}

					cItemType = eFISHINGRATE_ITEM_UNKNOWN;
					nItemCnt = 0;
				}
				else
				{
					bError = TRUE;
					break;
				}
				continue;
			}
			else if(!bOpenBlock && 0==strcmp(string, "#UTILITY"))
			{
				cItemType = eFISHINGRATE_ITEM_UTILITY;
			}
			else if(!bOpenBlock && 0==strcmp(string, "#BAIT"))
			{
				cItemType = eFISHINGRATE_ITEM_BAIT;
			}
			else if(bOpenBlock && 0==strcmp(string, "#INDEX"))
			{
				LoadRateData.dwItemIndex = file.GetDword();
			}
			else if(bOpenBlock && 0==strcmp(string, "#EFFECT"))
			{
				if(MAX_FISHITEM<=nItemCnt)
				{
					bError = TRUE;
					break;
				}

				LoadRateData.FishList[nItemCnt].nGrade = file.GetInt();
				LoadRateData.FishList[nItemCnt].fRate = file.GetFloat();
				nItemCnt++;
			}
		}
	}

	file.Release();

	// 미션아이템-코드파일 로딩
	int nMissionItemCnt = 0;
	sprintf(filename,"system/Resource/FishingMissionCode.bin");

	if( file.Init( filename, "rb" ) )
	{
		while(!file.IsEOF())
		{
			file.GetString(string);

			if(string[0] == '@')
			{
				file.GetLineX(string, 256);
				continue;
			}
			else if(string[0] == '{')
			{
				if(!bOpenBlock)
				{
					bOpenBlock = TRUE;
					nMissionItemCnt = 0;
				}
				else
				{
					bError = TRUE;
					break;
				}
				continue;
			}
			else if(string[0] == '}')
			{
				if(bOpenBlock)
				{
					//저장
					bOpenBlock = FALSE;
				}
				else
				{
					bError = TRUE;
					break;
				}
				continue;
			}
			else if(bOpenBlock && 0==strcmp(string, "#CODE"))
			{
				int nCode = file.GetWord();
				if(0<=nCode && nCode<MAX_FISHINGMISSIONCODE)
					g_FishingMissionCode[nCode] = file.GetDword();
				
				nMissionItemCnt++;
			}
		}
	}

	file.Release();


	// 낚시터 스크립트파일 로딩
	sprintf(filename,"system/Resource/fishing_%02d.bin", g_pServerSystem->GetMapNum());

	if( ! file.Init( filename, "rb" ) )
		return;


	int nFishItemCnt = 0;
	stFishingPlaceInfo LoadData;

	while(!file.IsEOF())
	{
		file.GetString(string);

		if(string[0] == '@')
		{
			file.GetLineX(string, 256);
			continue;
		}
		else if(string[0] == '{')
		{
			if(!bOpenBlock && 0<nCount && nCount<=MAX_FISHINGPLACE)
			{
				LoadData.Clear();
				bOpenBlock = TRUE;
			}
			else
			{
				bError = TRUE;
				break;
			}
			continue;
		}
		else if(string[0] == '}')
		{
			if(bOpenBlock)
			{
				//저장
				m_FishingPlaceInfo[nCount-1].Clear();
				m_FishingPlaceInfo[nCount-1].cNpcKind = LoadData.cNpcKind;
				m_FishingPlaceInfo[nCount-1].vPos = LoadData.vPos;
				m_FishingPlaceInfo[nCount-1].dwLifeTime = LoadData.dwLifeTime;
				m_FishingPlaceInfo[nCount-1].dwProcessTime = LoadData.dwProcessTime;
				m_FishingPlaceInfo[nCount-1].nRepeatCount = LoadData.nRepeatCount;

				int i;
				for(i=0; i<MAX_FISHITEM; i++)
					m_FishingPlaceInfo[nCount-1].m_FishItem[i] = LoadData.m_FishItem[i];

				for(i=0; i<4; i++)
					m_FishingPlaceInfo[nCount-1].fHitAddRate[i] = LoadData.fHitAddRate[i];

				std::vector<stFishItemInfo>::iterator iter;
				for(iter=LoadData.m_vecPlaceEff.begin(); iter!=LoadData.m_vecPlaceEff.end(); iter++)
				{
					stFishItemInfo ItemInfo;
					ItemInfo.dwItemIndex = iter->dwItemIndex;
					ItemInfo.fRate = iter->fRate;
					m_FishingPlaceInfo[nCount-1].m_vecPlaceEff.push_back(ItemInfo);
				}

				for(i=0; i<eWS_Max; i++)
				{
					for(iter=LoadData.m_vecWeatherEff[i].begin(); iter!=LoadData.m_vecWeatherEff[i].end(); iter++)
					{
						stFishItemInfo ItemInfo;
						ItemInfo.dwItemIndex = iter->dwItemIndex;
						ItemInfo.fRate = iter->fRate;
						m_FishingPlaceInfo[nCount-1].m_vecWeatherEff[i].push_back(ItemInfo);
					}
				}

				m_FishingPlaceInfo[nCount-1].dwBaseDelay = LoadData.dwBaseDelay;
				m_FishingPlaceInfo[nCount-1].dwRandDelay = LoadData.dwRandDelay;
				m_FishingPlaceInfo[nCount-1].cFishItemNum = char( nFishItemCnt );

				bOpenBlock = FALSE;
				nFishItemCnt = 0;
			}
			else
			{
				bError = TRUE;
				break;
			}
			continue;
		}

		if(!bOpenBlock && 0==strcmp(string, "#FISHINGPLACE"))
		{
			m_nFishingPlaceNum = (int)file.GetDword();

			if(nCount<0 || MAX_FISHINGPLACE<nCount)
			{
				bError = FALSE;
				break;
			}
		}
		else if(!bOpenBlock && 0==strcmp(string, "#PLACEINFO"))
		{
			nCount = file.GetInt();

			if(nCount<0 || MAX_FISHINGPLACE<nCount)
			{
				bError = FALSE;
				break;
			}
		}
		else if(!bOpenBlock && 0==strcmp(string, "#MISSION"))
		{
			stFishingMissionInfo MissionInfo;
			MissionInfo.dwCode = file.GetDword();
			MissionInfo.dwRewardItem = file.GetDword();
			MissionInfo.dwRewardItemNum = file.GetInt();
			g_mapMissionInfo.insert(std::make_pair(MissionInfo.dwCode, MissionInfo));
		}
		else if(!bOpenBlock && 0==strcmp(string, "#MISSIONDELAY"))
		{
			m_dwMissionDelayTime = file.GetDword();
		}
		else if(!bOpenBlock && 0==strcmp(string, "#MISSIONEND"))
		{
			m_dwMissionEndTime = file.GetDword();
		}
		else if(!bOpenBlock && 0==strcmp(string, "#EVENTFISHPOINT"))
		{
			m_FishingEventInfo.wPoint[0] = file.GetWord();
			m_FishingEventInfo.wPoint[1] = file.GetWord();
			m_FishingEventInfo.wPoint[2] = file.GetWord();
			m_FishingEventInfo.bOnEvent = TRUE;
		}
		else if(!bOpenBlock && 0==strcmp(string, "#EVENTREWARD"))
		{
			static WORD wCount = 0;

			if(wCount < 3)
			{
				m_FishingEventInfo.wGoalPoint[wCount] = file.GetWord();
				m_FishingEventInfo.dwRewardItemIdx[wCount] = file.GetDword();
				wCount++;
			}
		}
		else if(bOpenBlock && 0==strcmp(string, "#NPCKIND"))
		{
			LoadData.cNpcKind = char( file.GetInt() );
		}
		else if(bOpenBlock && 0==strcmp(string, "#POS"))
		{
			LoadData.vPos.x = file.GetFloat();
			LoadData.vPos.y = 0.0f;
			LoadData.vPos.z = file.GetFloat();
		}
		else if(bOpenBlock && 0==strcmp(string, "#LIFE"))
		{
			LoadData.dwLifeTime = file.GetDword();
		}
		else if(bOpenBlock && 0==strcmp(string, "#DIFFICULTY"))
		{
			LoadData.dwProcessTime = file.GetDword();
			LoadData.nRepeatCount = file.GetInt();
		}
		else if(bOpenBlock && 0==strcmp(string, "#HITADDRATE"))
		{
			LoadData.fHitAddRate[0] = file.GetFloat();	// 공통
			LoadData.fHitAddRate[1] = file.GetFloat();	// 피라미급
			LoadData.fHitAddRate[2] = file.GetFloat();	// 붕어급
			LoadData.fHitAddRate[3] = file.GetFloat();	// 잉어급
		}
		else if(bOpenBlock && 0==strcmp(string, "#FISH"))
		{
			LoadData.m_FishItem[nFishItemCnt].dwItemIndex = file.GetDword();
			LoadData.m_FishItem[nFishItemCnt].fRate = file.GetFloat();
			nFishItemCnt++;
		}
		else if(bOpenBlock && 0==strcmp(string, "#EFFECT"))
		{
			stFishItemInfo ItemInfo;
			ItemInfo.dwItemIndex = file.GetDword();
			ItemInfo.fRate = file.GetFloat();
			LoadData.m_vecPlaceEff.push_back(ItemInfo);
		}
		else if(bOpenBlock && 0==strcmp(string, "#CLEAR"))
		{
			stFishItemInfo ItemInfo;
			ItemInfo.dwItemIndex = file.GetDword();
			ItemInfo.fRate = file.GetFloat();
			LoadData.m_vecWeatherEff[eWS_Clear].push_back(ItemInfo);
		}
		else if(bOpenBlock && 0==strcmp(string, "#CLOUDY"))
		{
			stFishItemInfo ItemInfo;
			ItemInfo.dwItemIndex = file.GetDword();
			ItemInfo.fRate = file.GetFloat();
			LoadData.m_vecWeatherEff[eWS_Cloudy].push_back(ItemInfo);
		}
		else if(bOpenBlock && 0==strcmp(string, "#RAINY"))
		{
			stFishItemInfo ItemInfo;
			ItemInfo.dwItemIndex = file.GetDword();
			ItemInfo.fRate = file.GetFloat();
			LoadData.m_vecWeatherEff[eWS_Rainy].push_back(ItemInfo);
		}
		else if(bOpenBlock && 0==strcmp(string, "#WINDY"))
		{
			stFishItemInfo ItemInfo;
			ItemInfo.dwItemIndex = file.GetDword();
			ItemInfo.fRate = file.GetFloat();
			LoadData.m_vecWeatherEff[eWS_Windy].push_back(ItemInfo);
		}
		else if(bOpenBlock && 0==strcmp(string, "#FOGGY"))
		{
			stFishItemInfo ItemInfo;
			ItemInfo.dwItemIndex = file.GetDword();
			ItemInfo.fRate = file.GetFloat();
			LoadData.m_vecWeatherEff[eWS_Foggy].push_back(ItemInfo);
		}
		else if(bOpenBlock && 0==strcmp(string, "#TYPOON"))
		{
			stFishItemInfo ItemInfo;
			ItemInfo.dwItemIndex = file.GetDword();
			ItemInfo.fRate = file.GetFloat();
			LoadData.m_vecWeatherEff[eWS_Typoon].push_back(ItemInfo);
		}
		else if(bOpenBlock && 0==strcmp(string, "#GENINFO"))
		{
			LoadData.dwBaseDelay = file.GetInt();
			LoadData.dwRandDelay = file.GetInt();
		}
	}

	if(bOpenBlock || nCount==0)
	{
		// 에러 : 스크립트 읽기가 정상종료되지 않음 - 관련정보 Clear
		int i;
		for(i=0; i<MAX_FISHINGPLACE; i++)
			m_FishingPlaceInfo[i].Clear();

		m_bActive = FALSE;
	}
	else
	{
		// 정상 : 낚시터 초기화
		m_bInit = TRUE;
		m_bActive = TRUE;

		for(i=0; i<m_nChannelNum; i++)
		{
			for(j=0; j<m_nFishingPlaceNum; j++)
			{
				m_FishingPlaceInst[i][j].byInfoIndex = BYTE( j );

				if(0 == m_FishingPlaceInfo[j].dwLifeTime)
				{
					// 정적생성 낚시터
					AddFishingPlace(&m_FishingPlaceInfo[j], i+1, BYTE( j ) );
				}
			}
		}
	}
	file.Release();

	if(0<nMissionItemCnt && 0<m_dwMissionDelayTime && 0<m_dwMissionEndTime)
		m_bUseMission = TRUE;

	SetPlaceEffect();
	SetWeatherEffect();
}

void CFishingManager::NetworkMsgParse( BYTE Protocol, void* pMsg, DWORD dwLength )
{
	switch(Protocol)
	{
	case MP_FISHING_READY_SYN:
		Fishing_Ready_Syn(pMsg);
		break;
	case MP_FISHING_GETFISH_SYN:
		Fishing_GetFish_Syn(pMsg);
		break;
	case MP_FISHING_CANCEL_SYN:
		Fishing_Cancel_Syn(pMsg);
		break;
	case MP_FISHING_FPCHANGE_SYN:
		Fishing_FPChange_Syn(pMsg);
		break;
	default:
		break;
	}
}

void CFishingManager::Fishing_Ready_Syn(void* pMsg)
{
	WORD wResult = eFishingError_None;
	CPlayer* pPlayer = NULL;

	MSG_DWORD4 msgAck;
	MSG_FISHING_READY_SYN* pmsg = (MSG_FISHING_READY_SYN*)pMsg;

	CObject* pObject = g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pObject)
	{
		wResult = eFishingError_InvaildHero;
		goto FISHING_READY_NACK;
	}
	pPlayer = (CPlayer*)pObject;
	
	pObject = NULL;
	pObject = g_pUserTable->FindUser(pmsg->dwPlaceIdx);
	if(!pObject)
	{
		wResult = eFishingError_InvaildPlace;
		goto FISHING_READY_NACK;
	}


	// 거리제한
	VECTOR3 vHeroPos, vPlacePos;
	pPlayer->GetPosition(&vHeroPos) ;
	pObject->GetPosition(&vPlacePos);
	float fDistance = CalcDistanceXZ(&vHeroPos, &vPlacePos);
	if(MAX_FISHING_DISTANCE < fDistance) 
	{
		wResult = eFishingError_OverDistance;
		goto FISHING_READY_NACK;
	}

	// 인벤확인
	ITEM_INFO * pItemInfo = NULL;
	CItemSlot* pInvenSlot = pPlayer->GetSlot(eItemTable_Inventory);
	if(0 == ITEMMGR->GetTotalEmptySlotNum(pInvenSlot, pPlayer))
	{
		wResult = eFishingError_InvenFull;
		goto FISHING_READY_NACK;
	}

	if(pPlayer->GetWearedWeapon() != pmsg->dwPoleItemIdx)
	{
		wResult = eFishingError_NoPole;
		goto FISHING_READY_NACK;
	}

	pItemInfo = ITEMMGR->GetItemInfo(pmsg->dwPoleItemIdx);
	if(pPlayer->GetFishingLevel() < pItemInfo->Grade)
	{
		wResult = eFishingError_InvalidGrade;
		goto FISHING_READY_NACK;
	}

	// 낚시중 확인
	if(0 < pPlayer->GetFishingStartTime())
	{
		wResult = eFishingError_InvaildState;
		goto FISHING_READY_NACK;
	}

	// 090423 ShinJS --- 탈것 탑승중인 경우낚시 불가
	if( pPlayer->GetMountedVehicle() != 0 )
	{
		wResult = eFishingError_InvaildState;
		goto FISHING_READY_NACK;
	}

	POSTYPE itempos = (POSTYPE)pmsg->wBaitItemPos;
	CItemSlot* pSlot = pPlayer->GetSlot(itempos);
	if(pSlot)
	{
		pItemInfo = ITEMMGR->GetItemInfo(pmsg->dwBaitItemIdx);
		if(pItemInfo->SupplyType != ITEM_KIND_FISHING_BAIT)
		{
			wResult = eFishingError_NoBait;
			goto FISHING_READY_NACK;
		}

		if(pPlayer->GetFishingLevel() < pItemInfo->Grade)
		{
			wResult = eFishingError_InvalidGrade;
			goto FISHING_READY_NACK;
		}

		if(eItemUseSuccess != ITEMMGR->UseItem(pPlayer, (WORD)pmsg->wBaitItemPos, pmsg->dwBaitItemIdx))
		{
			wResult = eFishingError_NoBait;
			goto FISHING_READY_NACK;
		}
		
		// 미끼 사용 통보
		MSG_ITEM_USE_ACK msgforUser;
		msgforUser.Category = MP_ITEM;
		msgforUser.Protocol = MP_ITEM_USE_ACK;
		msgforUser.dwObjectID = pmsg->dwObjectID;
		msgforUser.TargetPos = pmsg->wBaitItemPos;
		msgforUser.dwItemIdx = pmsg->dwBaitItemIdx;
		msgforUser.eResult	 = eItemUseSuccess;
		pPlayer->SendMsg(&msgforUser, sizeof(msgforUser));

		// 미끼 등급에 따른 처리는 여기서.
		float* pFishBaitRate = pPlayer->GetFishBaitRate();
		stFishingRate* pFishingRate = GetFishingBaitRate(pmsg->dwBaitItemIdx);
		if(pFishingRate)
		{
			int i;
			for(i=0; i<MAX_FISHITEM; i++)
			{
				if(pFishingRate->FishList[i].nGrade < 0)
					continue;

				pFishBaitRate[pFishingRate->FishList[i].nGrade] += pFishingRate->FishList[i].fRate;

				if(pFishBaitRate[pFishingRate->FishList[i].nGrade] < 0.0f)
					pFishBaitRate[pFishingRate->FishList[i].nGrade] = 0.0f;
			}
		}

		pPlayer->SetFishingBait(pmsg->dwBaitItemIdx);
	}

	pPlayer->SetFishingPlace(pmsg->dwPlaceIdx);
	pPlayer->SetFishingStartTime(gCurTime);

	stFishingPlaceInst* pPlaceInst = GetFishingPlaceInst(pmsg->dwPlaceIdx);
	if(!pPlaceInst)
		goto FISHING_READY_NACK;

	pPlayer->SetFishingProcessTime(m_FishingPlaceInfo[pPlaceInst->byInfoIndex].dwProcessTime + FISHINGTIME_STARTPULLING);


	msgAck.Category = MP_FISHING;
	msgAck.Protocol = MP_FISHING_READY_ACK;
	msgAck.dwObjectID = pPlayer->GetID();
	msgAck.dwData1 = pmsg->dwPlaceIdx;											//낚시터NPC 인덱스
	msgAck.dwData2 = rand()%4 * 30;												//게이지 시작위치
	msgAck.dwData3 = m_FishingPlaceInfo[pPlaceInst->byInfoIndex].dwProcessTime;	//게이지 진행시간
	msgAck.dwData4 = m_FishingPlaceInfo[pPlaceInst->byInfoIndex].nRepeatCount;	//게이지 반복횟수
	PACKEDDATA_OBJ->QuickSend( pPlayer, &msgAck, sizeof( msgAck ) );
	return;


FISHING_READY_NACK:
	if(!pPlayer || wResult==eFishingError_InvaildHero)
		return;

	pPlayer->FishingInfoClear();

	MSG_WORD msgNack;
	msgNack.Category = MP_FISHING;
	msgNack.Protocol = MP_FISHING_READY_NACK;
	msgNack.dwObjectID = pPlayer->GetID();
	msgNack.wData = wResult;
	pPlayer->SendMsg( &msgNack , sizeof( msgNack ) );
}

void CFishingManager::Fishing_GetFish_Syn(void* pMsg)
{
	WORD wResult = eFishingError_None;
	CPlayer* pPlayer = NULL;

	MSG_FISHING_GETFISH_SYN* pmsg = (MSG_FISHING_GETFISH_SYN*)pMsg;

	CObject* pObject = g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pObject)
		return;

	pPlayer = (CPlayer*)pObject;


	// 유효성확인
	if(0<pPlayer->GetFishingStartTime())
	{
		// 예상 완료시간보다 일찍 오면 오류
		if(gCurTime-pPlayer->GetFishingStartTime() < pPlayer->GetFishingProcessTime()-FISHINGTIME_LATENCY)
		{
			wResult = eFishingError_EarlyTime;
		}
	}
	else
	{
		wResult = eFishingError_InvaildState;
	}

	// 인벤확인
	CItemSlot* pSlot = pPlayer->GetSlot(eItemTable_Inventory);
	if(0 == ITEMMGR->GetTotalEmptySlotNum(pSlot, pPlayer))
		wResult = eFishingError_InvenFull;

	// 낚시대확인
	if(pPlayer->GetWearedWeapon() != pmsg->dwPoleItemIdx)
		wResult = eFishingError_NoPole;

	SendGetFish(pPlayer, wResult, pMsg);
}

void CFishingManager::Fishing_Cancel_Syn(void* pMsg)
{
	WORD wResult = eFishingError_UserCancel;
	CPlayer* pPlayer = NULL;

	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CObject* pObject = g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pObject)
		return;
	
	pPlayer = (CPlayer*)pObject;
	SendGetFish(pPlayer, wResult, pMsg);
}

void CFishingManager::Fishing_FPChange_Syn(void* pMsg)
{
	MSG_FISHING_FPCHANGE_SYN* pmsg = (MSG_FISHING_FPCHANGE_SYN*)pMsg;
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	if(!pPlayer)
		return;

	int i;
	BOOL bFail = FALSE;
	DWORD dwAddFishPoint = 0;
	DWORD dwFishPoint = 0;
	ITEM_INFO * pItemInfo = NULL;
	DWORD dwResult = eFishingError_None;

	// MAX_FISHPOINT 유효검사
	for(i=0; i<MAX_FISHLIST_FORPOINT; i++)
	{
		pItemInfo = ITEMMGR->GetItemInfo(pmsg->FishList[i].dwItemIdx);
		if(pItemInfo && pItemInfo->SupplyType==ITEM_KIND_FISHING_FISH)
		{
			ITEMBASE Item = *ITEMMGR->GetItemInfoAbsIn(pPlayer, pmsg->FishList[i].pos);
			dwFishPoint = pItemInfo->SupplyValue * Item.Durability;

			dwAddFishPoint += dwFishPoint;
		}
	}

	if(MAX_FISHPOINT < pPlayer->GetFishPoint() + dwAddFishPoint)
	{
		bFail = TRUE;
		dwResult = eFishingError_OverMaxFishPoint;
	}

	if(!bFail)
	{
		dwFishPoint = 0;
		dwAddFishPoint = 0;
		for(i=0; i<MAX_FISHLIST_FORPOINT; i++)
		{
			pItemInfo = ITEMMGR->GetItemInfo(pmsg->FishList[i].dwItemIdx);
			if(pItemInfo && pItemInfo->SupplyType==ITEM_KIND_FISHING_FISH)
			{
				ITEMBASE Item = *ITEMMGR->GetItemInfoAbsIn(pPlayer, pmsg->FishList[i].pos);
				dwFishPoint = pItemInfo->SupplyValue * Item.Durability;
				if(EI_TRUE != ITEMMGR->DiscardItem(pPlayer, pmsg->FishList[i].pos, pmsg->FishList[i].dwItemIdx, pmsg->FishList[i].dur))
				{
					bFail = TRUE;
					dwResult = eFishingError_FailFishPoint;
					break;
				}
				else
				{
					LogItemMoney(pmsg->dwObjectID, pPlayer->GetObjectName(), 0, "",
						eLog_ItemDiscard, pPlayer->GetMoney(eItemTable_Inventory), 0, 0,
						Item.wIconIdx, Item.dwDBIdx,  Item.Position, 0,
						Item.Durability, pPlayer->GetPlayerExpPoint());

					// 080808 LUJ, 낚시 경험치, 레벨 추가
					Log_Fishing(
						pPlayer->GetID(),
						eFishingLog_GetPointFromFish,
						dwFishPoint,
						pPlayer->GetFishPoint() + dwAddFishPoint + dwFishPoint,
						0,
						pmsg->FishList[i].dwItemIdx,
						pPlayer->GetFishingExp(),
						pPlayer->GetFishingLevel() );
				}

				dwAddFishPoint += dwFishPoint;
			}
		}
	}

	DWORD dwLastFishPoint = pPlayer->GetFishPoint();
	pPlayer->SetFishPoint(dwLastFishPoint+dwAddFishPoint);

	if(!bFail && dwAddFishPoint==pmsg->dwAddFishPoint)
	{
		MSG_DWORD msgFPChangeAck;
		msgFPChangeAck.Category = MP_FISHING;
		msgFPChangeAck.Protocol = MP_FISHING_FPCHANGE_ACK;
		msgFPChangeAck.dwData	= dwAddFishPoint;
		pPlayer->SendMsg(&msgFPChangeAck, sizeof(msgFPChangeAck));
	}
	else
	{
		MSG_DWORD msgFishPoint;
		msgFishPoint.Category = MP_FISHING;
		msgFishPoint.Protocol = MP_FISHING_FPCHANGE_NACK;
		msgFishPoint.dwData	= dwResult;
		pPlayer->SendMsg(&msgFishPoint, sizeof(msgFishPoint));
	}
}

void CFishingManager::Process()
{
	if(!m_bActive || gCurTime==0)
		return;

	int i,j;
	if(m_bInit)
	{
		// 동적생성 낚시터 초기화 : 1회만 동작
		for(i=0; i<m_nChannelNum; i++)
		{
			for(j=0; j<m_nFishingPlaceNum; j++)
			{
				if(0 != m_FishingPlaceInfo[j].dwLifeTime)
				{
					if(m_FishingPlaceInfo[j].dwLifeTime == 0)
					{
						DWORD dwTemp = m_FishingPlaceInfo[j].dwBaseDelay / 60000;
						dwTemp = (rand()%dwTemp) * 60000;

						m_FishingPlaceInst[i][j].dwDelTime = gCurTime + (m_FishingPlaceInfo[j].dwBaseDelay - dwTemp);
					}
				}
			}
		}

		m_bInit = FALSE;
	}

	stFishingPlaceInst* pInst = NULL;
	m_FishingGabagePlace.RemoveAll();

	// 동적생성 낚시터 프로세스
	for(i=0; i<m_nChannelNum; i++)
	{
		for(j=0; j<m_nFishingPlaceNum; j++)
		{
			pInst = &m_FishingPlaceInst[i][j];

			if(0 == m_FishingPlaceInfo[pInst->byInfoIndex].dwLifeTime)
				continue;
			else
			{
				if(0==pInst->dwObjectIndex && pInst->dwDelTime < gCurTime)
				{
					AddFishingPlace(&m_FishingPlaceInfo[pInst->byInfoIndex], i+1, pInst->byInfoIndex);
				}
				else if(0!=pInst->dwObjectIndex && (pInst->dwGenTime+m_FishingPlaceInfo[pInst->byInfoIndex].dwLifeTime)<gCurTime)
				{
					m_FishingGabagePlace.Add(&pInst, pInst->dwObjectIndex);
					DelFishingPlace(pInst);
				}
			}
		}
	}

	// 미션상태 변경
	CheckMissionState();
}

BOOL CFishingManager::AddFishingPlace(stFishingPlaceInfo* pFishingPlaceInfo, DWORD dwChannel, BYTE byInfoIndex)
{
	if(pFishingPlaceInfo)
	{
		BASEOBJECT_INFO Baseinfo;
		NPC_TOTALINFO NpcTotalInfo;

		NPC_LIST* pNpcList = GAMERESRCMNGR->GetNpcInfo(pFishingPlaceInfo->cNpcKind);
		if(pNpcList == 0)
			return FALSE;

		DWORD dwObjectID = g_pAISystem.GeneraterMonsterID();

		if(dwObjectID == 0)
		{
			m_FishingPlaceInst[dwChannel-1][byInfoIndex].byInfoIndex = byInfoIndex;
			m_FishingPlaceInst[dwChannel-1][byInfoIndex].dwObjectIndex = 0;
			m_FishingPlaceInst[dwChannel-1][byInfoIndex].dwGenTime = 0;
			return FALSE;
		}

		Baseinfo.dwObjectID = dwObjectID;
		Baseinfo.BattleID = dwChannel;
		
		SafeStrCpy(Baseinfo.ObjectName, pNpcList->Name, MAX_NAME_LENGTH+1);
		NpcTotalInfo.Group = 0;
		NpcTotalInfo.MapNum = g_pServerSystem->GetMapNum();
		NpcTotalInfo.NpcJob = pNpcList->JobKind;
		NpcTotalInfo.NpcKind = pNpcList->NpcKind;
		NpcTotalInfo.NpcUniqueIdx = 0;

		CNpc* pNpc = g_pServerSystem->AddNpc(&Baseinfo, &NpcTotalInfo, &pFishingPlaceInfo->vPos);
		if(pNpc)
		{
			pNpc->SetDieTime(0);	// 자동소멸되지 않도록.

			m_FishingPlaceInst[dwChannel-1][byInfoIndex].byInfoIndex = byInfoIndex;
			m_FishingPlaceInst[dwChannel-1][byInfoIndex].dwObjectIndex = dwObjectID;
			m_FishingPlaceInst[dwChannel-1][byInfoIndex].dwGenTime = gCurTime;
			return TRUE;
		}
	}

	return FALSE;
}

void CFishingManager::DelFishingPlace(stFishingPlaceInst* pInst)
{
	if(pInst== NULL)
		return;

	CNpc* pNpc = (CNpc*)g_pUserTable->FindUser(pInst->dwObjectIndex);
	if(pNpc == NULL)
		return;

	MSG_DWORD2 msg;
	msg.Category	= MP_USERCONN;

	if(pNpc->GetObjectKind() & eObjectKind_Npc)
	{
		msg.Protocol	= MP_USERCONN_NPC_DIE;

		msg.dwObjectID	= 0;
		msg.dwData1		= pInst->dwObjectIndex;
		msg.dwData2		= pNpc->GetID();
		
		PACKEDDATA_OBJ->QuickSend((CObject*)pNpc,&msg,sizeof(msg));
	}

	pNpc->Die(NULL);

	pInst->dwObjectIndex = 0;
	pInst->dwGenTime = 0;

	if(0 == m_FishingPlaceInfo[pInst->byInfoIndex].dwLifeTime)
	{
		pInst->dwDelTime = gCurTime;
	}
	else
	{
		pInst->dwDelTime = gCurTime + m_FishingPlaceInfo[pInst->byInfoIndex].dwLifeTime;
	}
}

void CFishingManager::SendGetFish(CPlayer* pPlayer, WORD wResCode, void* pMsg)
{
	if(!pPlayer)	return;


	// 성공
	if(wResCode == eFishingError_None)
	{
		DWORD dwTempTable[MAX_FISHITEM] = {0,};
		int nAccumulate = 0;
		DWORD dwFishingPlace = pPlayer->GetFishingPlace();
		
		stFishingPlaceInst* pPlaceInst = GetFishingPlaceInst(dwFishingPlace);
		stFishingPlaceInfo* pPlaceInfo = NULL;
		if(pPlaceInst)
		{
			pPlaceInfo = &m_FishingPlaceInfo[pPlaceInst->byInfoIndex];
			if(!pPlaceInfo)				goto FISHING_GETFISH_NACK;
		}
		else
			goto FISHING_GETFISH_NACK;

		int nItemRate[eFishItem_Max] = {0,};

		// 낚시터에 따른 추가확률
		nItemRate[eFishItem_Poor]		+= m_nPlaceEff[pPlaceInst->byInfoIndex][eFishItem_Poor];
		nItemRate[eFishItem_Common]		+= m_nPlaceEff[pPlaceInst->byInfoIndex][eFishItem_Common];
		nItemRate[eFishItem_UnCommon]	+= m_nPlaceEff[pPlaceInst->byInfoIndex][eFishItem_UnCommon];
		nItemRate[eFishItem_Rare]		+= m_nPlaceEff[pPlaceInst->byInfoIndex][eFishItem_Rare];
		nItemRate[eFishItem_Unique]		+= m_nPlaceEff[pPlaceInst->byInfoIndex][eFishItem_Unique];


		// 날씨에 따른 추가확률
		WORD wWeather = WEATHERMGR->GetWeatherState();
		nItemRate[eFishItem_Poor]		+= m_nWeatherEff[pPlaceInst->byInfoIndex][wWeather][eFishItem_Poor];
		nItemRate[eFishItem_Common]		+= m_nWeatherEff[pPlaceInst->byInfoIndex][wWeather][eFishItem_Common];
		nItemRate[eFishItem_UnCommon]	+= m_nWeatherEff[pPlaceInst->byInfoIndex][wWeather][eFishItem_UnCommon];
		nItemRate[eFishItem_Rare]		+= m_nWeatherEff[pPlaceInst->byInfoIndex][wWeather][eFishItem_Rare];
		nItemRate[eFishItem_Unique]		+= m_nWeatherEff[pPlaceInst->byInfoIndex][wWeather][eFishItem_Unique];


		// 컨트롤에 따른 추가확률
		MSG_FISHING_GETFISH_SYN* pmsg = (MSG_FISHING_GETFISH_SYN*)pMsg;
		if(!pmsg)						goto FISHING_GETFISH_NACK;

		if(pPlaceInfo->nRepeatCount < pmsg->wPerpectCnt + pmsg->wMissCnt + pmsg->wGoodCnt + pmsg->wGreatCnt)
										goto FISHING_GETFISH_NACK;

		DWORD dwEventRewardItemIdx = 0;
		if(m_FishingEventInfo.bOnEvent/* && pPlayer->GetUserLevel()<=eUSERLEVEL_GM //이벤트는 유저레벨체크*/)
		{
			WORD wPoint = 0;
			wPoint += pmsg->wPerpectCnt * m_FishingEventInfo.wPoint[0];
			wPoint += pmsg->wGreatCnt * m_FishingEventInfo.wPoint[1];
			wPoint += pmsg->wGoodCnt * m_FishingEventInfo.wPoint[2];

			if(m_FishingEventInfo.wGoalPoint[2] <= wPoint)
				dwEventRewardItemIdx = m_FishingEventInfo.dwRewardItemIdx[2];
			else if(m_FishingEventInfo.wGoalPoint[1] <= wPoint)
				dwEventRewardItemIdx = m_FishingEventInfo.dwRewardItemIdx[1];
			else if(m_FishingEventInfo.wGoalPoint[0] <= wPoint)
				dwEventRewardItemIdx = m_FishingEventInfo.dwRewardItemIdx[0];
		}
		
		// 실패는 잉어,붕어,피라미 확률축소
		nItemRate[eFishItem_Poor]		-= (int)((float)pmsg->wMissCnt		* FISHINGRATE_SCALE);
		nItemRate[eFishItem_UnCommon]	-= (int)((float)pmsg->wMissCnt		* FISHINGRATE_SCALE);
		nItemRate[eFishItem_Common]		-= (int)((float)pmsg->wMissCnt		* FISHINGRATE_SCALE);

		if(0 < pmsg->wPerpectCnt)
		{
			// Perpect는 잉어 확률증가
			nItemRate[eFishItem_UnCommon]	+= (int)((float)pmsg->wPerpectCnt	* FISHINGRATE_SCALE * pPlaceInfo->fHitAddRate[3]);
			nItemRate[eFishItem_Common]		+= (int)((float)pmsg->wPerpectCnt	* FISHINGRATE_SCALE * pPlaceInfo->fHitAddRate[0]);
			nItemRate[eFishItem_Poor]		+= (int)((float)pmsg->wPerpectCnt	* FISHINGRATE_SCALE * pPlaceInfo->fHitAddRate[0]);
		}
		if(0 < pmsg->wGreatCnt)
		{
			// Grate는 붕어 확률증가
			nItemRate[eFishItem_UnCommon]	+= (int)((float)pmsg->wGreatCnt		* FISHINGRATE_SCALE * pPlaceInfo->fHitAddRate[0]);
			nItemRate[eFishItem_Common]		+= (int)((float)pmsg->wGreatCnt		* FISHINGRATE_SCALE * pPlaceInfo->fHitAddRate[2]);
			nItemRate[eFishItem_Poor]		+= (int)((float)pmsg->wGreatCnt		* FISHINGRATE_SCALE * pPlaceInfo->fHitAddRate[0]);
		}
		if(0 < pmsg->wGoodCnt)
		{
			// Good은 피라미 확률증가
			nItemRate[eFishItem_UnCommon]	+= (int)((float)pmsg->wGoodCnt		* FISHINGRATE_SCALE * pPlaceInfo->fHitAddRate[0]);
			nItemRate[eFishItem_Common]		+= (int)((float)pmsg->wGoodCnt		* FISHINGRATE_SCALE * pPlaceInfo->fHitAddRate[0]);
			nItemRate[eFishItem_Poor]		+= (int)((float)pmsg->wGoodCnt		* FISHINGRATE_SCALE * pPlaceInfo->fHitAddRate[1]);
		}

		// 캐릭터착용 장비에 따른 추가확률
		nItemRate[eFishItem_Poor]		+= (int)(pPlayer->GetFishItemRate_Grade(eFishItem_Poor)		* FISHINGRATE_SCALE);
		nItemRate[eFishItem_Common]		+= (int)(pPlayer->GetFishItemRate_Grade(eFishItem_Common)	* FISHINGRATE_SCALE);
		nItemRate[eFishItem_UnCommon]	+= (int)(pPlayer->GetFishItemRate_Grade(eFishItem_UnCommon)	* FISHINGRATE_SCALE);
		nItemRate[eFishItem_Rare]		+= (int)(pPlayer->GetFishItemRate_Grade(eFishItem_Rare)		* FISHINGRATE_SCALE);
		nItemRate[eFishItem_Unique]		+= (int)(pPlayer->GetFishItemRate_Grade(eFishItem_Unique)	* FISHINGRATE_SCALE);

		// 사용 미끼에 따른 추가확률
		nItemRate[eFishItem_Poor]		+= (int)(pPlayer->GetFishBaitRate_Grade(eFishItem_Poor)		* FISHINGRATE_SCALE);
		nItemRate[eFishItem_Common]		+= (int)(pPlayer->GetFishBaitRate_Grade(eFishItem_Common)	* FISHINGRATE_SCALE);
		nItemRate[eFishItem_UnCommon]	+= (int)(pPlayer->GetFishBaitRate_Grade(eFishItem_UnCommon)	* FISHINGRATE_SCALE);
		nItemRate[eFishItem_Rare]		+= (int)(pPlayer->GetFishBaitRate_Grade(eFishItem_Rare)		* FISHINGRATE_SCALE);
		nItemRate[eFishItem_Unique]		+= (int)(pPlayer->GetFishBaitRate_Grade(eFishItem_Unique)	* FISHINGRATE_SCALE);

		int i;
		for(i=0; i<eFishItem_Max; i++)
		{
			if(nItemRate[i] < 0)
				nItemRate[i] = 0;
		}

		DWORD dwItemIdx = 0;
		for(i=0; i<pPlaceInfo->cFishItemNum; i++)
		{
			dwItemIdx = pPlaceInfo->m_FishItem[i].dwItemIndex;
			
			if(0 == dwItemIdx)
				break;

			ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(dwItemIdx);
			if(!pItemInfo)				goto FISHING_GETFISH_NACK;

			// 기본확률
			dwTempTable[i] = (int)(pPlaceInfo->m_FishItem[i].fRate * FISHINGRATE_SCALE);
			if(0<i)
				dwTempTable[i] += dwTempTable[i-1];
			
			// 등급별 추가확률 적용
			if(eFishItem_Poor<=pItemInfo->Grade && pItemInfo->Grade<eFishItem_Max)
			{
				dwTempTable[i] += nItemRate[pItemInfo->Grade];
				nItemRate[pItemInfo->Grade] = 0;
			}

			nAccumulate += dwTempTable[i];
		}

		// 보상아이템 결정
		int nHiRand = rand()%100;
		int nLoRand = rand()%1000;
		DWORD dwRand = nHiRand*1000 + nLoRand;



		//// 디버그용 확률테이블 출력
		//char buf[256] = {0,};
		//char buf2[128][MAX_FISHITEM] = {0,};
		//int j;
		//for(j=0; j<MAX_FISHITEM; j++)
		//{
		//	itoa(dwTempTable[j], buf2[j], 10);
		//}
		//sprintf(buf, "Rand[%d] Result : [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s]\n", dwRand,
		//	&buf2[0], &buf2[1], &buf2[2], &buf2[3], &buf2[4], &buf2[5], &buf2[6], &buf2[7], &buf2[8], &buf2[9]);
		//OutputDebugStringA(buf);


		DWORD dwFishItemIdx = 0;

		int nCnt = 0;
		while(1)
		{
			if( pPlaceInfo->cFishItemNum < nCnt)
				break;

			if(nCnt == 0)
			{
				if(0<=dwRand && dwRand<dwTempTable[nCnt])
				{
					dwFishItemIdx = pPlaceInfo->m_FishItem[nCnt].dwItemIndex;
					break;
				}
			}
			else
			{
				if(dwTempTable[nCnt-1]<=dwRand && dwRand<dwTempTable[nCnt])
				{
					dwFishItemIdx = pPlaceInfo->m_FishItem[nCnt].dwItemIndex;
					break;
				}
			}

			nCnt++;
		}

		if(dwEventRewardItemIdx)
		{
			ITEMMGR->ObtainGeneralItem(pPlayer, dwEventRewardItemIdx, 1, eLog_ItemObtainFishing, MP_ITEM_MONSTER_OBTAIN_NOTIFY);
		}

		if(0 < dwFishItemIdx)
		{
			ITEMMGR->ObtainGeneralItem(pPlayer, dwFishItemIdx, 1, eLog_ItemObtainFishing, MP_ITEM_MONSTER_OBTAIN_NOTIFY);

			if(MAX_FISHING_MISSION <= pPlayer->m_lstGetFishList.size())
			{
				while(1)
				{
					pPlayer->m_lstGetFishList.pop_front();
					if(pPlayer->m_lstGetFishList.size() <= 2)
						break;
				}
			}

			if(pPlayer->GetFM_Mission() != FISHINGMISSION_INVALID)
				pPlayer->m_lstGetFishList.push_back(dwFishItemIdx);

			if(MAX_FISHING_MISSION == pPlayer->m_lstGetFishList.size())
			{
				// 미션완료 했는지 확인!!
				WORD pFishCode[MAX_FISHING_MISSION];
				GetFishCodeFromFM_Code(pPlayer->GetFM_Mission(), pFishCode);

				int nSameCnt = 0;
				std::list<DWORD>::iterator iter;
				for(i=0, iter=pPlayer->m_lstGetFishList.begin(); iter!=pPlayer->m_lstGetFishList.end(); iter++, i++)
				{
					DWORD dwFishItem = *iter;
					if(dwFishItem == g_FishingMissionCode[pFishCode[i]])
						nSameCnt++;
				}

				if(MAX_FISHING_MISSION <= nSameCnt)
				{
					// 우왕ㅋ굳ㅋ 미션성공!!
					DWORD dwRewardItem = 0;
					int nRewardItemCnt = 0;

					std::map<DWORD, stFishingMissionInfo>::iterator iterInfo;
					iterInfo = g_mapMissionInfo.find(pPlayer->GetFM_Mission());
					if(iterInfo != g_mapMissionInfo.end())
					{
						dwRewardItem = iterInfo->second.dwRewardItem;
						nRewardItemCnt = iterInfo->second.dwRewardItemNum;
						ITEMMGR->ObtainGeneralItem(pPlayer, dwRewardItem, WORD( nRewardItemCnt ), eLog_ItemObtainFishing, MP_ITEM_MONSTER_OBTAIN_NOTIFY);
					}

					// 미션 완료했으므로 Clear
					pPlayer->m_lstGetFishList.clear();
					pPlayer->SetFM_Mission(FISHINGMISSION_INVALID);

					MSG_DWORD msgReward;
					msgReward.Category = MP_FISHING;
					msgReward.Protocol = MP_FISHING_MISSION_REWARD;
					msgReward.dwObjectID = pPlayer->GetID();
					msgReward.dwData = dwRewardItem;
					pPlayer->SendMsg(&msgReward, sizeof(msgReward));
				}
				else
				{
					// 미션 실패했으므로 Clear
					pPlayer->m_lstGetFishList.clear();
					pPlayer->SetFM_Mission(FISHINGMISSION_INVALID);
					SendFishingMissionInfo(pPlayer, FALSE);
				}
			}


			// 080424 NYJ --- 낚시숙련도
			// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
			//DWORD dwFishingExp = pPlayer->GetFishingExp() + 1;
			EXPTYPE dwFishingExp = 0 ;
			dwFishingExp = pPlayer->GetFishingExp() + 1;

			pPlayer->SetFishingExp(dwFishingExp);

			// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
			//MSG_DWORD_WORD msgFishingExp;
			//msgFishingExp.Category = MP_FISHING;
			//msgFishingExp.Protocol = MP_FISHING_EXP_ACK;
			//msgFishingExp.dwObjectID = pPlayer->GetID();
			//msgFishingExp.wData    = pPlayer->GetFishingLevel();
			//msgFishingExp.dwData   = pPlayer->GetFishingExp();
			//pPlayer->SendMsg( &msgFishingExp, sizeof( msgFishingExp ) );

			MSG_DWORDEX2 msgFishingExp;
			msgFishingExp.Category = MP_FISHING;
			msgFishingExp.Protocol = MP_FISHING_EXP_ACK;
			msgFishingExp.dwObjectID = pPlayer->GetID();
			msgFishingExp.dweData1    = pPlayer->GetFishingLevel();
			msgFishingExp.dweData2   = pPlayer->GetFishingExp();
			pPlayer->SendMsg( &msgFishingExp, sizeof( msgFishingExp ) );
		}

		MSG_FISHING_GETFISH_ACK msg;
		msg.Category = MP_FISHING;
		msg.Protocol = MP_FISHING_GETFISH_ACK;
		msg.dwObjectID = pPlayer->GetID();
		msg.wResCode = wResCode;
		msg.dwItemIdx = dwFishItemIdx;
		PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );


		pPlayer->FishingInfoClear();
		return;
	}

FISHING_GETFISH_NACK:	// 실패
	MSG_WORD msg;
	msg.Category = MP_FISHING;
	msg.Protocol = MP_FISHING_GETFISH_NACK;
	msg.dwObjectID = pPlayer->GetID();
	msg.wData = wResCode;

	PACKEDDATA_OBJ->QuickSend( pPlayer, &msg, sizeof( msg ) );
	pPlayer->FishingInfoClear();
}

stFishingPlaceInst* CFishingManager::GetFishingPlaceInst(DWORD dwPlaceIndex)
{
	if(!m_bActive || 0==dwPlaceIndex)		return NULL;

	int i, j;
	for(i=0; i<m_nChannelNum; i++)
	{
		for(j=0; j<m_nFishingPlaceNum; j++)
		{
			if(dwPlaceIndex == m_FishingPlaceInst[i][j].dwObjectIndex)
			{
				return &m_FishingPlaceInst[i][j];
			}
		}
	}

	return NULL;
}

void CFishingManager::SetPlaceEffect()
{
	int i;
	ITEM_INFO* pItemInfo = NULL;

	for(i=0; i<m_nFishingPlaceNum; i++)
	{
		std::vector<stFishItemInfo>::iterator iter;
		for(iter=m_FishingPlaceInfo[i].m_vecPlaceEff.begin(); iter!=m_FishingPlaceInfo[i].m_vecPlaceEff.end(); iter++)
		{
			DWORD dwItemIdx = iter->dwItemIndex;
			pItemInfo = ITEMMGR->GetItemInfo(dwItemIdx);
			if(pItemInfo)
			{
				switch(pItemInfo->Grade)
				{
					case eFishItem_Poor:		m_nPlaceEff[i][eFishItem_Poor] = (int)(iter->fRate * FISHINGRATE_SCALE);			break;
					case eFishItem_Common:		m_nPlaceEff[i][eFishItem_Common] = (int)(iter->fRate * FISHINGRATE_SCALE);			break;
					case eFishItem_UnCommon:	m_nPlaceEff[i][eFishItem_UnCommon] = (int)(iter->fRate * FISHINGRATE_SCALE);		break;
					case eFishItem_Rare:		m_nPlaceEff[i][eFishItem_Rare] = (int)(iter->fRate * FISHINGRATE_SCALE);			break;
					case eFishItem_Unique:		m_nPlaceEff[i][eFishItem_Unique] = (int)(iter->fRate * FISHINGRATE_SCALE);			break;
				}
			}
		}
	}
}

void CFishingManager::SetWeatherEffect()
{
	int i,j;
	std::vector<stFishItemInfo>::iterator iter;
	ITEM_INFO* pItemInfo = NULL;

	for(i=0; i<m_nFishingPlaceNum; i++)
	{
		for(j=0; j<eWS_Max; j++)
		{
			for(iter=m_FishingPlaceInfo[i].m_vecWeatherEff[j].begin(); iter!=m_FishingPlaceInfo[i].m_vecWeatherEff[j].end(); iter++)
			{
				DWORD dwItemIdx = iter->dwItemIndex;
				pItemInfo = ITEMMGR->GetItemInfo(dwItemIdx);
				if(pItemInfo)
				{
					switch(pItemInfo->Grade)
					{
						case eFishItem_Poor:		m_nWeatherEff[i][j][eFishItem_Poor] = (int)(iter->fRate * FISHINGRATE_SCALE);			break;
						case eFishItem_Common:		m_nWeatherEff[i][j][eFishItem_Common] = (int)(iter->fRate * FISHINGRATE_SCALE);		break;
						case eFishItem_UnCommon:	m_nWeatherEff[i][j][eFishItem_UnCommon] = (int)(iter->fRate * FISHINGRATE_SCALE);		break;
						case eFishItem_Rare:		m_nWeatherEff[i][j][eFishItem_Rare] = (int)(iter->fRate * FISHINGRATE_SCALE);			break;
						case eFishItem_Unique:		m_nWeatherEff[i][j][eFishItem_Unique] = (int)(iter->fRate * FISHINGRATE_SCALE);		break;
					}
				}
			}
		}
	}
}

DWORD CFishingManager::GetItemIdxFromFM_Code(int nCode)
{
	if(0<nCode && nCode<MAX_FISHINGMISSIONCODE)
		return g_FishingMissionCode[nCode];
	
	return 0;
}

void CFishingManager::GetFishCodeFromFM_Code(DWORD dwMissionCode, WORD* pFishCode)
{
	if(!pFishCode)	return;

	DWORD dwMod;
	pFishCode[0] = (WORD)(dwMissionCode / 10000);		dwMod = dwMissionCode % 10000;
	pFishCode[1] = (WORD)(dwMod	/ 100);					dwMod = dwMod % 100;
	pFishCode[2] = (WORD)(dwMod);
}

void CFishingManager::SendFishingMissionInfo(CPlayer* pPlayer, BOOL bNew)
{
	if(!pPlayer)	return;

	DWORD dwMissionCode = FISHINGMISSION_INVALID;
	pPlayer->SetFM_Mission(dwMissionCode);
	
	MSG_FISHING_MISSION_CODE msg;
	msg.Category = MP_FISHING;
	msg.Protocol = MP_FISHING_MISSION_UPDATE;
	msg.dwObjectID = pPlayer->GetID();
	msg.dwItem1 = 0;
	msg.dwItem2 = 0;
	msg.dwItem3 = 0;
	msg.dwProcessTime = 0;

	if(bNew)
	{
		int nSize = g_mapMissionInfo.size();
		int nRand = rand()%nSize;

		if(nRand < MAX_FISHINGMISSIONCODE)
		{
			std::map<DWORD, stFishingMissionInfo>::iterator iter;
			int i;
			
			for(i=0,iter=g_mapMissionInfo.begin(); iter!=g_mapMissionInfo.end(); i++,iter++)
			{
				if(i==nRand)
				{
					dwMissionCode = iter->second.dwCode;
					break;
				}
			}
			pPlayer->SetFM_Mission(dwMissionCode);

			DWORD dwMod;
			msg.dwItem1 = dwMissionCode / 10000;		dwMod = dwMissionCode % 10000;
			msg.dwItem2 = dwMod	/ 100;					dwMod = dwMod % 100;
			msg.dwItem3 = dwMod;
			msg.dwProcessTime = m_dwMissionEndTime;
		}
	}

	pPlayer->SendMsg(&msg, sizeof(msg));
}

stFishingRate* CFishingManager::GetFishingUtilityRate(DWORD dwItemIndex)
{
	std::map<DWORD, stFishingRate*>::iterator iter;
	iter = m_mapUtilRateList.find(dwItemIndex);
	if(iter != m_mapUtilRateList.end())
		return iter->second;

	return NULL;
}

stFishingRate* CFishingManager::GetFishingBaitRate(DWORD dwItemIndex)
{
	std::map<DWORD, stFishingRate*>::iterator iter;
	iter = m_mapBaitRateList.find(dwItemIndex);
	if(iter != m_mapBaitRateList.end())
		return iter->second;

	return NULL;
}

char CFishingManager::CheckMissionState()
{
	if(m_cMissionState==eFISHMISSION_STATE_READY && m_dwMissionSendTime+m_dwMissionDelayTime<=gCurTime)
	{
		m_cMissionState = eFISHMISSION_STATE_NEW;
	}
	else if(m_cMissionState==eFISHMISSION_STATE_STANDBY && m_dwMissionSendTime+m_dwMissionEndTime<=gCurTime)
	{
		m_cMissionState = eFISHMISSION_STATE_END;
	}

	return m_cMissionState;
}

void CFishingManager::ChangeMissionState()
{
	if(!m_bActive)
		return;

	if(m_cMissionState==eFISHMISSION_STATE_NEW)
	{
		// 미션 다 보냈다. 상태바꾸자~
		m_cMissionState = eFISHMISSION_STATE_STANDBY;
		m_dwMissionSendTime = gCurTime;
	}
	else if(m_cMissionState==eFISHMISSION_STATE_END)
	{
		// 미션 다 끝냈다. 상태바꾸자~
		m_cMissionState = eFISHMISSION_STATE_READY;
	}
}

void CFishingManager::ProcessPlayer(CPlayer* pPlayer)
{
	if(!m_bActive || !pPlayer)
		return;

	if(m_cMissionState==eFISHMISSION_STATE_NEW)
	{
		// 개별적으로 미션보냄
		SendFishingMissionInfo(pPlayer, TRUE);
	}
	else if(m_cMissionState==eFISHMISSION_STATE_END)
	{
		// 개별적으로 미션끝냄
		SendFishingMissionInfo(pPlayer, FALSE);
	}

	if(0==pPlayer->GetFishingPlace())
	{
		return;
	}
	else if(m_FishingGabagePlace.GetData(pPlayer->GetFishingPlace()))
	{
		// 낚시터인덱스가 유효하지 않으면 실패보냄
		WORD wResult = eFishingError_InvaildPlace;
		SendGetFish(pPlayer, wResult);
		return;
	}
}

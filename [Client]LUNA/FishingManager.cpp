#include "FishingManager.h"
#include "MHFile.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include "ObjectStateManager.h"
#include "MoveManager.h"
#include "GameIn.h"
#include "FishingDialog.h"
#include "FishingGaugeDialog.h"
#include "FishingPointDialog.h"
#include ".\interface\cGuageBar.h"
#include "ObjectGuagen.h"
#include "GameResourceManager.h"
#include "InventoryExDialog.h"
#include "QuickManager.h"
#include "ItemManager.h"

DWORD g_FishingMissionCode[MAX_FISHINGMISSIONCODE] = {0,};

GLOBALTON(CFishingManager)
CFishingManager::CFishingManager(void)
{
	m_bUseMission = FALSE;
	FishingInfoClear();

	m_nGaugeBarDir = 1;

	m_wFishingLevel = 1;
	m_dwFishingExp = 0;
	m_dwFishPoint = 0;
}

CFishingManager::~CFishingManager(void)
{
}

void CFishingManager::Init()
{
	// 091221 pdy 낚시 관련 정보들 초기화
	FishingInfoClear();

	CMHFile file;
	char filename[256] = {0,};
	BOOL bError = FALSE;		// bError==TRUE 이거나
	BOOL bOpenBlock = FALSE;	// bOpenBlock==TRUE 인채로 종료되면 비정상이므로 시스템은 구동되지 않는다.
	char string[256] = {0,};

	// 미션아이템-코드파일 로딩
	int  nItemCnt = 0;
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
				
				nItemCnt++;
			}
		}
	}

	file.Release();
}

void CFishingManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_FISHING_READY_ACK:				Fishing_Ready_Ack(pMsg);			break;
	case MP_FISHING_READY_NACK:				Fishing_Ready_Nack(pMsg);			break;
	case MP_FISHING_GETFISH_ACK:			Fishing_GetFish_Ack(pMsg);			break;
	case MP_FISHING_GETFISH_NACK:			Fishing_GetFish_Nack(pMsg);			break;
	case MP_FISHING_MISSION_UPDATE:			Fishing_Mission_Update(pMsg);		break;
	case MP_FISHING_MISSION_REWARD:			Fishing_Mission_Reward(pMsg);		break;
	case MP_FISHING_EXP_ACK:				Fishing_Exp_Ack(pMsg);				break;
	case MP_FISHING_POINT_ACK:				Fishing_Point_Ack(pMsg);			break;
	case MP_FISHING_LEVELUP_ACK:			Fishing_LevelUp_Ack(pMsg);			break;
	case MP_FISHING_LEVELUP_NACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1353) );
		}
		break;
	case MP_FISHING_FPCHANGE_ACK:			Fishing_FPChange_Ack(pMsg);			break;
	case MP_FISHING_FPCHANGE_NACK:			Fishing_FPChange_Nack(pMsg);		break;
	default:																	break;
	}
}

void CFishingManager::Fishing_Ready_Ack(void* pMsg)
{
	MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwObjectID);
	if(!pObject)
		return;

	if(pObject == HERO)
	{
		if(0==pmsg->dwData1 || 0==pmsg->dwData3 || 0==pmsg->dwData4)
			return;

		SetFishingStartTime(gCurTime);
		SetFishingPlace(pmsg->dwData1);

		m_fGaugeStartPos = (float)pmsg->dwData2/100.0f;
		m_dwProcessTime = pmsg->dwData3;
		m_nRepeatCount = pmsg->dwData4;

		LONG maxValue = 0;
		cGuageBar* pGaugeBar = GAMEIN->GetFishingGaugeDlg()->GetFishingGB();
		if(pGaugeBar)
			maxValue = pGaugeBar->GetMaxValue();

		m_fGaugeBarSpeed = ((float)m_nRepeatCount * maxValue) / (float)m_dwProcessTime;
		m_dwUpdateTime = (m_dwProcessTime / m_nRepeatCount) / maxValue;

		m_bActive = TRUE;
	}
	else
	{
		CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject(pmsg->dwData1);
		if(pNpc)
		{
			// 방향전환
			VECTOR3 pos;
			pNpc->GetPosition(&pos);
			MOVEMGR->SetLookatPos(pObject,&pos,0,gCurTime);
		}
	}


	OBJECTSTATEMGR->StartObjectState(pObject, eObjectState_Fishing);
	OBJECTEFFECTDESC desc( FindEffectNum("char_m_fishing_swing_A.beff") );

	pObject->RemoveObjectEffect( FISHING_START_EFFECT );
	pObject->AddObjectEffect( FISHING_START_EFFECT, &desc, 1 );
}

void CFishingManager::Fishing_Ready_Nack(void* pMsg)
{
	FishingInfoClear();
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;

	switch(pmsg->wData)
	{
	case eFishingError_InvaildHero:
		{
			goto FISHING_READY_NACK;
		}
		break;
	case eFishingError_InvaildPlace:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1519) );
			goto FISHING_READY_NACK;
		}
		break;
	case eFishingError_NoPole:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1532) );
			goto FISHING_READY_NACK;
		}
		break;
	case eFishingError_NoBait:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1527) );
			goto FISHING_READY_NACK;
		}
		break;
	case eFishingError_InvenFull:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1520) );
			goto FISHING_READY_NACK;
		}
		break;
	case eFishingError_InvalidGrade:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1530) );
			goto FISHING_READY_NACK;
		}
		break;
	case eFishingError_OverDistance:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1548) );
			goto FISHING_READY_NACK;
		}
		break;
	}
	return;

FISHING_READY_NACK:
	GAMEIN->GetFishingDlg()->SetPushStartBtn(FALSE);
	GAMEIN->GetFishingDlg()->SetLastFishingEndTime(0);
}

void CFishingManager::Fishing_GetFish_Ack(void* pMsg)
{
	MSG_FISHING_GETFISH_ACK* pmsg = (MSG_FISHING_GETFISH_ACK*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwObjectID);
	if(!pObject)		return;

	if(pObject == HERO)
	{
		GAMEIN->GetFishingDlg()->CheckMissionState(pmsg->dwItemIdx);
		FishingInfoClear();
	}

	OBJECTSTATEMGR->EndObjectState(pObject, eObjectState_Fishing);
	OBJECTSTATEMGR->StartObjectState(pObject, eObjectState_FishingResult);

	if(pmsg->dwItemIdx == 0)
	{
		OBJECTEFFECTDESC desc( FindEffectNum("char_m_fishing_failure_E.beff") );

		pObject->RemoveObjectEffect( FISHING_START_EFFECT );
		pObject->RemoveObjectEffect( FISHING_FAIL_EFFECT );
		pObject->AddObjectEffect( FISHING_FAIL_EFFECT, &desc, 1 );
	}
	else
	{	
		OBJECTEFFECTDESC desc( FindEffectNum("char_m_fishing_success_E.beff") );

		pObject->RemoveObjectEffect( FISHING_START_EFFECT );
		pObject->RemoveObjectEffect( FISHING_SUCCESS_EFFECT );
		pObject->AddObjectEffect( FISHING_SUCCESS_EFFECT, &desc, 1 );
	}
}

void CFishingManager::Fishing_GetFish_Nack(void* pMsg)
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwObjectID);
	if(!pObject)		return;


	OBJECTSTATEMGR->EndObjectState(pObject, eObjectState_Fishing);
	OBJECTSTATEMGR->StartObjectState(pObject, eObjectState_None);
	OBJECTEFFECTDESC desc( FindEffectNum("char_m_fishing_failure_E.beff") );

	pObject->RemoveObjectEffect( FISHING_START_EFFECT );
	pObject->RemoveObjectEffect( FISHING_FAIL_EFFECT );
	pObject->AddObjectEffect( FISHING_FAIL_EFFECT, &desc, 1 );

	if(pObject == HERO)
	{
		FishingInfoClear();

		switch(pmsg->wData)
		{
		case eFishingError_InvaildHero:
			{
				goto FISHING_GETFISH_NACK;
			}
			break;
		case eFishingError_InvaildPlace:
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1519) );
				goto FISHING_GETFISH_NACK;
			}
			break;
		case eFishingError_NoPole:	
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1532) );
				goto FISHING_GETFISH_NACK;
			}
			break;
		case eFishingError_NoBait:
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1527) );
				goto FISHING_GETFISH_NACK;
			}
			break;
		case eFishingError_UserCancel:
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1521) );
				goto FISHING_GETFISH_NACK;
			}
			break;
		case eFishingError_InvenFull:
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1520) );
				goto FISHING_GETFISH_NACK;
			}
			break;
		case eFishingError_EarlyTime:
		case eFishingError_InvaildState:
			goto FISHING_GETFISH_NACK;
			break;
		default:
			goto FISHING_GETFISH_NACK;
			break;
		}
	}

	return;

FISHING_GETFISH_NACK:
	GAMEIN->GetFishingDlg()->SetPushStartBtn(FALSE);
	GAMEIN->GetFishingDlg()->SetLastFishingEndTime(0);
	GAMEIN->GetFishingGaugeDlg()->SetActive(FALSE);
}

void CFishingManager::Fishing_Mission_Update(void* pMsg)
{
	MSG_FISHING_MISSION_CODE* pmsg = (MSG_FISHING_MISSION_CODE*)pMsg;

	DWORD dwMission[MAX_FISHING_MISSION] = {0};
	dwMission[0] = GetItemIdxFromFM_Code(pmsg->dwItem1);
	dwMission[1] = GetItemIdxFromFM_Code(pmsg->dwItem2);
	dwMission[2] = GetItemIdxFromFM_Code(pmsg->dwItem3);

	GAMEIN->GetFishingDlg()->SetMissionItem(dwMission);
	GAMEIN->GetFishingDlg()->SetProcessTimeText("");

	//090227 pdy 낚시 성공시 XXX표시 및 실패메시지 출력 버그 수정
	//미션 성공시에도, 미션이 끝나는 시간엔 항상 호출이 되므로 진짜 실패여부를 채크할 필요가 있다.
	BOOL bRealFailed = FALSE;

	if(0 < dwMission[0]+dwMission[1]+dwMission[2])
	{
		GAMEIN->GetFishingDlg()->SetMissionStartTime(gCurTime);
		GAMEIN->GetFishingDlg()->SetMissionProcessTime(pmsg->dwProcessTime);

		if(!m_bUseMission)
			m_bUseMission = TRUE;
	}
	else
	{
		GAMEIN->GetFishingDlg()->SetMissionStartTime(0);
		GAMEIN->GetFishingDlg()->SetMissionProcessTime(pmsg->dwProcessTime);

		if(m_bUseMission)	
		{
			m_bUseMission = FALSE;

			bRealFailed = TRUE;		//진짜 실패다.
		}
	}

	if(GAMEIN->GetFishingDlg()->IsActive())
	{
		if(0 < dwMission[0]+dwMission[1]+dwMission[2])
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1541 ) );
		}
		else if( bRealFailed )	//진짜 실패가 되었을때 
		{
			//090227 pdy 낚시 성공시 XXX표시 및 실패메시지 출력 버그 수정
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1523) );		
		}
	}
}

void CFishingManager::Fishing_Mission_Reward(void* pMsg)
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1522) );

	// 미션 성공했으므로 Clear
	DWORD dwMission[MAX_FISHING_MISSION] = {0,};
	GAMEIN->GetFishingDlg()->SetMissionItem(dwMission);
	GAMEIN->GetFishingDlg()->SetProcessTimeText("");

	//090227 pdy 낚시 성공시 XXX표시 및 실패메시지 출력 버그 수정
	m_bUseMission = FALSE;
}

void CFishingManager::Fishing_Exp_Ack(void* pMsg)
{
	// 080602 LYW --- FamilyDialog : 경험치 처리 (__int32)에서 (__int64)로 변경 처리.
	//MSG_DWORD_WORD* pmsg = (MSG_DWORD_WORD*)pMsg;
	MSG_DWORDEX2* pmsg = (MSG_DWORDEX2*)pMsg;

	BOOL bChangeLevel = FALSE;
	if(m_wFishingLevel != pmsg->dweData1)
		bChangeLevel = TRUE;

	m_wFishingLevel = (WORD)pmsg->dweData1;
	m_dwFishingExp = (DWORD)pmsg->dweData2;

	// 080530 LYW --- FishingManager : Modified a data type DWORD to EXPTYPE.
	//DWORD dwMaxExp = GAMERESRCMNGR->GetFishingMaxExpPoint(m_wFishingLevel);
	EXPTYPE dwMaxExp = 0 ;
	dwMaxExp = GAMERESRCMNGR->GetFishingMaxExpPoint(m_wFishingLevel) ;

	GAMEIN->GetFishingDlg()->SetExpValue((float)m_dwFishingExp/(float)dwMaxExp, 0);

	char szValue[50], szLevel[16];
	if(m_wFishingLevel == 1)
		strcpy(szLevel, CHATMGR->GetChatMsg(1544));
	else if(m_wFishingLevel == 2)
		strcpy(szLevel, CHATMGR->GetChatMsg(1545));
	else if(m_wFishingLevel == 3)
		strcpy(szLevel, CHATMGR->GetChatMsg(1546));
	else if(m_wFishingLevel == 4)
		strcpy(szLevel, CHATMGR->GetChatMsg(1547));
	else
		strcpy(szLevel, "");

	sprintf( szValue, "%d/%I64d (%s)", m_dwFishingExp, dwMaxExp, szLevel);
	GAMEIN->GetFishingDlg()->SetExpText(szValue);


	//낚시 등급이 바뀌었으므로 인벤토리에서 사용가능한 목록을 재조회 한다.
	if(bChangeLevel)
	{
		GAMEIN->GetInventoryDialog()->RefreshInvenItem();
	}
}

void CFishingManager::Fishing_Point_Ack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	m_dwFishPoint = pmsg->dwData;

	char szValue[50];
	sprintf( szValue, "%d", m_dwFishPoint);
	GAMEIN->GetFishingDlg()->SetPointText(szValue);
	GAMEIN->GetFishingPointDlg()->SetPointText(szValue);
}

void CFishingManager::Fishing_LevelUp_Ack(void* pMsg)
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	CPlayer * pPlayer = (CPlayer *)OBJECTMGR->GetObject(pmsg->dwObjectID);
	if(!pPlayer) return;
	
	m_wFishingLevel = pmsg->wData;

	EFFECTMGR->StartEffectProcess(eEffect_LevelUpSentence,pPlayer,NULL,0,0);


	//낚시 등급이 바뀌었으므로 인벤토리에서 사용가능한 목록을 재조회 한다.
	GAMEIN->GetInventoryDialog()->RefreshInvenItem();
}

void CFishingManager::Fishing_FPChange_Ack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	m_dwFishPoint += pmsg->dwData;

	CHATMGR->AddMsg( CTC_EXPENDEDITEM, CHATMGR->GetChatMsg( 1539 ), pmsg->dwData );

	int i;
	for(i=0; i<MAX_FISHLIST_FORPOINT; i++)
	{
		CVirtualItem* pItemList = (CVirtualItem*)GAMEIN->GetFishingPointDlg()->GetFishList(i);
		CItem* pItem = (CItem*)pItemList->GetLinkItem();
		if(pItem)
		{
			ITEMMGR->DeleteItemofTable(ITEMMGR->GetTableIdxForAbsPos(pItem->GetPosition()), pItem->GetPosition(), TRUE) ;
		}
	}

	char szValue[50];
	sprintf( szValue, "%d", m_dwFishPoint);
	GAMEIN->GetFishingDlg()->SetPointText(szValue);
	GAMEIN->GetFishingPointDlg()->SetPointText(szValue);

	GAMEIN->GetFishingPointDlg()->ClearFishIcon();
	GAMEIN->GetInventoryDialog()->SetDisable(FALSE);
	QUICKMGR->RefreshQickItem();
}

void CFishingManager::Fishing_FPChange_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	switch(pmsg->dwData)
	{
	case eFishingError_OverMaxFishPoint:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1549));
		break;

	case eFishingError_FailFishPoint:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1550));
		break;
	}

	GAMEIN->GetFishingPointDlg()->ClearFishIcon();
	GAMEIN->GetInventoryDialog()->SetDisable(FALSE);
}

void CFishingManager::SendFishing_Ready(DWORD dwBaitIdx, POSTYPE BaitPos)
{
	const EObjectState IsRestMode = OBJECTSTATEMGR->GetObjectState(HERO) ;

	if(IsRestMode == eObjectState_FishingResult)
	{
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_FishingResult);
		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_None);
	}
	else if(IsRestMode != eObjectState_None)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1524) );
		goto FISHING_SENDREADY_ERROR;
	}

	if(m_bActive)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1525) );
		goto FISHING_SENDREADY_ERROR;
	}

	CObject* pObject = OBJECTMGR->GetSelectedObject();
	if(pObject && pObject->GetObjectKind() == eObjectKind_Npc)
	{
		CNpc* pNpc = (CNpc*)pObject;
		if(pNpc->GetNpcJob() != FISHINGPLACE_ROLE)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1526) );
			goto FISHING_SENDREADY_ERROR;
		}

		if(HERO->GetWeaponEquipType() != eWeaponType_FishingPole)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1532) );
			goto FISHING_SENDREADY_ERROR;
		}

		if(pNpc->IsDied())
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1524) );
			goto FISHING_SENDREADY_ERROR;
		}

		if(0==dwBaitIdx)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1527) );
			goto FISHING_SENDREADY_ERROR;
		}

		// 거리제한
		VECTOR3 vHeroPos;
		HERO->GetPosition(&vHeroPos) ;
		float fDistance = CalcDistanceXZ(&vHeroPos, &pObject->GetBaseMoveInfo()->CurPosition) ;
		if(MAX_FISHING_DISTANCE < fDistance) 
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1548)) ;
			goto FISHING_SENDREADY_ERROR;
		}

		// 방향전환
		VECTOR3 pos;
		pNpc->GetPosition(&pos);
		MOVEMGR->SetLookatPos(HERO,&pos,0,gCurTime);

		GAMEIN->GetFishingDlg()->SetLastFishingEndTime(0);

		MSG_FISHING_READY_SYN msg;
		msg.Category = MP_FISHING;
		msg.Protocol = MP_FISHING_READY_SYN;
		msg.dwObjectID = HEROID;
		msg.dwPlaceIdx = pNpc->GetID();
		msg.dwPoleItemIdx = HERO->GetWearedItemIdx(eWearedItem_Weapon);
		msg.wBaitItemPos = BaitPos;
		msg.dwBaitItemIdx = dwBaitIdx;

		NETWORK->Send(&msg, sizeof(msg));
		return;
	}

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1526) );
	goto FISHING_SENDREADY_ERROR;

FISHING_SENDREADY_ERROR:
	GAMEIN->GetFishingDlg()->SetPushStartBtn(FALSE);
	GAMEIN->GetFishingDlg()->SetLastFishingEndTime(0);
}

void CFishingManager::SendFishing_Cancel()
{
	const EObjectState IsRestMode = OBJECTSTATEMGR->GetObjectState(HERO) ;

	if(IsRestMode==eObjectState_Fishing || IsRestMode==eObjectState_FishingResult)
	{
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Fishing);
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_FishingResult);

		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_None);

		MSG_DWORD msg;
		msg.Category = MP_FISHING;
		msg.Protocol = MP_FISHING_CANCEL_SYN;
		msg.dwObjectID = HEROID;
		msg.dwData = m_dwFishingPlaceIdx;

		NETWORK->Send(&msg, sizeof(msg));
	}

	FishingInfoClear();
}

void CFishingManager::Process()
{
	CFishingDialog* pFishingDlg = GAMEIN->GetFishingDlg();
	CFishingGaugeDialog* pFishingGaugeDlg = GAMEIN->GetFishingGaugeDlg();

	if(!pFishingDlg)
		return;


	// 자동재시작
	if(!m_bActive && 0<pFishingDlg->GetLastFishingEndTime() && pFishingDlg->IsPushedStartBtn())
	{
		if(pFishingDlg->GetLastFishingEndTime()+FISHING_AUTO_RETRY_DELAY < gCurTime)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1528) );
			pFishingDlg->Fishing_Start();
		}
	}

	if(!m_bActive || 0==m_dwFishingPlaceIdx || 0==m_dwFishingStartTime)		return;

	if(!m_bPulling && m_dwFishingStartTime+FISHINGTIME_STARTPULLING <= gCurTime)
	{
		// 게이지 출력.
		m_bPulling = TRUE;
		m_nGaugeBarDir = 1;
		CObjectGuagen* pTimeGauge = pFishingGaugeDlg->GetFishingTimeGauge();
		if(pTimeGauge)
			pTimeGauge->SetValue(1.0f, m_dwProcessTime);

		cGuageBar* pGaugeBar = pFishingGaugeDlg->GetFishingGB();
		if(pGaugeBar)
		{
			LONG maxValue = pGaugeBar->GetMaxValue();
			pGaugeBar->SetCurValue((LONG)(m_fGaugeStartPos * maxValue));
		}

		pFishingGaugeDlg->SetActive(TRUE);
		m_dwGaugeStartTime = gCurTime;
	}
	else if(m_bPulling && m_dwFishingStartTime+FISHINGTIME_STARTPULLING+m_dwProcessTime <= gCurTime)
	{
		m_bPulling = FALSE;
		SetFishingStartTime(0);

		// 게이지 제거.
		pFishingGaugeDlg->SetActive(FALSE);

		// 서버에 물고기요청
		MSG_FISHING_GETFISH_SYN msg;
		msg.Category = MP_FISHING;
		msg.Protocol = MP_FISHING_GETFISH_SYN;
		msg.dwObjectID = HEROID;
		msg.dwPlaceIdx = m_dwFishingPlaceIdx;
		msg.dwPoleItemIdx = HERO->GetWearedItemIdx(eWearedItem_Weapon);
		msg.wPerpectCnt = WORD(pFishingDlg->GetPointPerpect());
		msg.wGoodCnt = WORD(pFishingDlg->GetPointGood());
		msg.wGreatCnt = WORD(pFishingDlg->GetPointGreat());
		msg.wMissCnt = WORD(pFishingDlg->GetPointMiss());

		NETWORK->Send(&msg, sizeof(msg));
	}
}

void CFishingManager::FishingInfoClear()
{
	m_bActive = FALSE;
	m_bPulling = FALSE;
	SetFishingPlace(0);
	SetFishingStartTime(0);

	m_fGaugeStartPos = 0.0f;
	m_fGaugeBarSpeed = 0.0f;
	m_dwProcessTime = 0;
	m_dwGaugeStartTime = 0;
	m_nRepeatCount = 0;

	if(GAMEIN->GetFishingDlg())
	{
		GAMEIN->GetFishingDlg()->ClearPoint();
		GAMEIN->GetFishingDlg()->SetLastFishingEndTime(gCurTime);
	}
}

DWORD CFishingManager::GetItemIdxFromFM_Code(int nCode)
{
	if(0<nCode && nCode<MAX_FISHINGMISSIONCODE)
		return g_FishingMissionCode[nCode];
	
	return 0;
}
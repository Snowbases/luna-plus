#include "stdafx.h"
#include "DungeonMgr.h"
#include "..\[CC]BattleSystem\BattleSystem_Server.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "..\ItemManager.h"
#include "..\PartyManager.h"
#include "..\Party.h"
#include "..\PackedData.h"
#include "..\UserTable.h"
#include "..\LootingManager.h"
#include "..\MapDBMsgParser.h"
#include "..\Monster.h"
#include "..\Npc.h"
#include "..\RegenManager.h"
#include "..\AISystem.h"
#include "..\..\[CC]ServerModule\MHFile.h"
#include "..\Trigger\Manager.h"
#include "..\Trigger\Timer.h"
#include "DungeonProcessorBase.h"
#include "DungeonProcessorTempleofGreed.h"
#include "DungeonProcessorBrokenShipwrecked.h"
#include "CharMove.h"
#include "..\[cc]Skill\Server\Manager\SkillManager.h"
#include "..\Player.h"
#include "..\..\[cc]skill\Server\Info\BuffSkillInfo.h"
#include "CheckRoutine.h"
#include "Finite State Machine/Machine.h"

GLOBALTON(CDungeonMgr)

const stDungeonProcessor DungeonProcessorTable[] = 
{
	{Temple_of_Greed,			CDungeonProcessorTempleofGreed::Clone},
	{Broken_Shipwrecked,		CDungeonProcessorBrokenShipwrecked::Clone},
	{The_Death_tower,			CDungeonProcessorEmpty::Clone},
	{The_Spirit_Forest,			CDungeonProcessorEmpty::Clone},
	{The_Cristal_Empire,		CDungeonProcessorEmpty::Clone},
	{TreeOfKenai,				CDungeonProcessorEmpty::Clone},
	{TheCryingTavern,			CDungeonProcessorEmpty::Clone},
	{TheUndergroundPassage,		CDungeonProcessorEmpty::Clone},
};

CDungeonMgr::CDungeonMgr(void)
{
	m_DungeonPool = NULL;
	m_ReservationPool = NULL;
	m_NoPartyPool = NULL;
}

CDungeonMgr::~CDungeonMgr(void)
{
	SAFE_DELETE(m_pDungeonProcessor);

	stDungeon* pDungeon = NULL;
	m_DungeonList.SetPositionHead();
	while((pDungeon = m_DungeonList.GetData()) != NULL)
	{
		m_DungeonPool->Free(pDungeon);
	}
	m_DungeonList.RemoveAll();

	m_ChannelList.RemoveAll();

	stDungeonReservation* pReservation = NULL;
	m_ReservationList.SetPositionHead();
	while((pReservation = m_ReservationList.GetData()) != NULL)
	{
		m_ReservationPool->Free(pReservation);
	}
	m_ReservationList.RemoveAll();

	stDungeonNoPartyList* pNoPartyList;
	m_NoPartyList.SetPositionHead();
	while((pNoPartyList = m_NoPartyList.GetData()) != NULL)
	{
		m_NoPartyPool->Free(pNoPartyList);
	}
	m_NoPartyList.RemoveAll();

	stMonsterList* pMonsterListEasy;
	m_MonsterList_Easy.SetPositionHead();
	while((pMonsterListEasy = m_MonsterList_Easy.GetData()) != NULL)
	{
		SAFE_DELETE(pMonsterListEasy);
	}

	stMonsterList* pMonsterListNormal;
	m_MonsterList_Normal.SetPositionHead();
	while((pMonsterListNormal = m_MonsterList_Normal.GetData()) != NULL)
	{
		SAFE_DELETE(pMonsterListNormal);
	}

	stMonsterList* pMonsterListHard;
	m_MonsterList_Hard.SetPositionHead();
	while((pMonsterListHard = m_MonsterList_Hard.GetData()) != NULL)
	{
		SAFE_DELETE(pMonsterListHard);
	}

	stWarpInfo* pWarp;
	m_WarpList.SetPositionHead();
	while((pWarp = m_WarpList.GetData()) != NULL)
	{
		SAFE_DELETE(pWarp);
	}

	stSwitchNpcInfo* pSwitchNpc;
	m_SwitchNpcList.SetPositionHead();
	while((pSwitchNpc = m_SwitchNpcList.GetData()) != NULL)
	{
		SAFE_DELETE(pSwitchNpc);
	}
	SAFE_DELETE(m_DungeonPool);
	SAFE_DELETE(m_ReservationPool);
	SAFE_DELETE(m_NoPartyPool);
}

void CDungeonMgr::Init()
{
	if(FALSE == IsDungeon(g_pServerSystem->GetMapNum()))
	{
		return;
	}

	for(DWORD i=0; i< _countof(DungeonProcessorTable); ++i)
	{
		if(g_pServerSystem->GetMapNum() == DungeonProcessorTable[i].mapnum)
		{
			m_pDungeonProcessor = DungeonProcessorTable[i].processor();
			break;
		}
	}

	if(! m_pDungeonProcessor)
	{
		DUNGEON_ASSERTMSG(0, "m_pDungeonProcessor 생성실패!");
		return;
	}



	m_DungeonPool = new CMemoryPoolTempl<stDungeon>;
	m_DungeonPool->Init(50, 50, "m_DungeonPool");

	m_ReservationPool = new CMemoryPoolTempl<stDungeonReservation>;
	m_ReservationPool->Init(100, 100, "ReservationPool");

	m_NoPartyPool = new CMemoryPoolTempl<stDungeonNoPartyList>;
	m_NoPartyPool->Init(100, 100, "NoPartyList");

	m_DungeonList.Initialize(100);
	m_ChannelList.Initialize(100);
	m_ReservationList.Initialize(100);
	m_NoPartyList.Initialize(100);

	m_MonsterList_Easy.Initialize(200);
	m_MonsterList_Normal.Initialize(200);
	m_MonsterList_Hard.Initialize(200);

	m_WarpList.Initialize(40);
	m_SwitchNpcList.Initialize(30);

	// 인던맵일경우 로드하는 데이터들...
	// 2. 트리거
	// 인던정보 스크립트
	LoadDungeonInfo();
}

void CDungeonMgr::Process()
{
	static DWORD dwLastCheckTick = gCurTime;

	// 1분에 한번씩만 확인
	if(gCurTime >= dwLastCheckTick+60000)
	{
		// 대기자 목록 5분경과시 제거
		stDungeonReservation* pReservation = NULL;
		m_ReservationList.SetPositionHead();
		while((pReservation = m_ReservationList.GetData()) != NULL)
		{
			if(pReservation->dwRegistTime+(60000*5) < gCurTime)
			{
				m_ReservationList.Remove(pReservation->dwCharIndex);
				m_ReservationPool->Free(pReservation);

				stDungeon* pID = m_DungeonList.GetData(pReservation->dwChannelID);
				if(pID && 0 == pID->dwJoinPlayerNum)
					Remove(pID->dwChannelID);
			}
		}

		// 파티가 없는 사람 제거
		CPlayer* pPlayer = NULL;
		stDungeonNoPartyList* pList = NULL;
		m_NoPartyList.SetPositionHead();
		while((pList = m_NoPartyList.GetData()) != NULL)
		{
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pList->dwCharIndex);
			if(! pPlayer)
			{
				stDungeonNoPartyList* pNoPartyList = m_NoPartyList.GetData(pList->dwCharIndex);
				if(pNoPartyList)
				{
					m_NoPartyList.Remove(pList->dwCharIndex);
					m_NoPartyPool->Free(pNoPartyList);
				}
				continue;
			}
			
			if(pList->dwOutTime+30000<=gCurTime)
			{
				// 입장했던 파티가 아니다. 내쫓자!
				if(pList->dwOldPartyIndex != pPlayer->GetPartyIdx())					
					MoveToLoginMap(pList->dwCharIndex);

				stDungeonNoPartyList* pNoPartyList = m_NoPartyList.GetData(pList->dwCharIndex);
				if(pNoPartyList)
				{
					m_NoPartyList.Remove(pList->dwCharIndex);
					m_NoPartyPool->Free(pNoPartyList);
				}
			}
		}

		// 091104 ONS 1분마다 보스몬스터를 체크하여 
		// Aggro수치가 0이면 HP와 Position을 초기화한다.
		stDungeon* pDungeon;
		m_DungeonList.SetPositionHead();
		while((pDungeon = m_DungeonList.GetData()) != NULL)
		{
			DWORD dwObjectIndex = pDungeon->m_CurBossMonsterState.dwObjectIndex;
			if(!dwObjectIndex)
				continue;

			CMonster* pBossMonster = (CMonster*)g_pUserTable->FindUser( dwObjectIndex ) ;
			if(pBossMonster)
			{
				DWORD dwAggro = pBossMonster->GetAggroNum();
				if( 0 == dwAggro && !pBossMonster->GetObjectBattleState() )
				{
					WORD wIndex = pDungeon->m_CurBossMonsterState.wIndex;
					BASE_MONSTER_LIST* pMonsterList = GAMERESRCMNGR->GetMonsterListInfo(wIndex);
					if(!pMonsterList)
						continue;

					// 원래위치로 이동 및 HP회복
					DWORD dwLife = pMonsterList->Life;
					pBossMonster->SetLife(dwLife);

					VECTOR3 vPos;
					vPos.x = pDungeon->m_CurBossMonsterState.wReturnPosX;
					vPos.y = 0.0f;
					vPos.z = pDungeon->m_CurBossMonsterState.wReturnPosZ;
					CCharMove::SetPosition( pBossMonster, &vPos );

					MOVE_POS msg;
					msg.Category = MP_MOVE;
					msg.Protocol = MP_MOVE_WARP;
					msg.dwMoverID = pBossMonster->GetID();
					msg.cpos.Compress(&vPos);
	
					PACKEDDATA_OBJ->QuickSend(pBossMonster,&msg,sizeof(msg));
				}
			}
		}

		dwLastCheckTick = gCurTime;
	}

	if(m_pDungeonProcessor)
		m_pDungeonProcessor->Process();
}

void CDungeonMgr::NetworkMsgParser(DWORD dwConnectionIndex, BYTE Protocol, void* pMsg, DWORD dwLength)
{
	switch(Protocol)
	{
	case MP_DUNGEON_ENTRANCE_SYN:	// 입장요청
		{
			// dwData1(키인덱스), dwData2(인던맵번호), dwData3(아이템인덱스), dwData4(아이템슬롯)
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

			DWORD dwError = eDungeonERR_None;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(! pPlayer)
				return;

			stDungeonKey* pKey = GAMERESRCMNGR->GetDungeonKey(pmsg->dwData1);
			if(! pKey)
				dwError = eDungeonERR_NoKeyInfo;

			CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx());
			if(! pParty)
				dwError = eDungeonERR_NoParty;

			if(!CHKRT->ItemOf(pPlayer, (POSTYPE)pmsg->dwData4, pmsg->dwData3, 0, 0, CB_EXIST|CB_ICONIDX))
				dwError = eDungeonERR_ERROR;

			if(dwError != eDungeonERR_None)
			{
				MSG_DWORD	msg;
				msg.Category = MP_DUNGEON;
				msg.Protocol = MP_DUNGEON_ENTRANCE_NACK;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.dwData = dwError;
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}

			eDIFFICULTY difficulty = pKey->difficulty;

			DungeonEntrance(dwConnectionIndex, g_pServerSystem->GetMapNum(), pPlayer->GetID(), pParty->GetPartyIdx(), pmsg->dwData1, pmsg->dwData3, pmsg->dwData4, pKey->wMapNum, difficulty);
		}
		break;

	case MP_DUNGEON_PORTAL_ENTRANCE_SYN:	// 포털 입장 요청
		{
			// dwData1(키인덱스), dwData2(인던맵번호), dwData3(NPC ID), dwData4(NPC UniqueIndex)
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

			DWORD dwError = eDungeonERR_None;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(! pPlayer)
				return;

			stDungeonKey* pKey = GAMERESRCMNGR->GetDungeonKey(pmsg->dwData1);
			if(! pKey)
				dwError = eDungeonERR_NoKeyInfo;

			CNpc* pNpc = (CNpc*)g_pUserTable->FindUser(pmsg->dwData3);
			if( !pNpc )
			{
				STATIC_NPCINFO* pNpcInfo = GAMERESRCMNGR->GetStaticNpcInfo( (WORD)pmsg->dwData4 );
				if( pNpcInfo == NULL )
				{
					return;
				}

				VECTOR3 vecPlayerPos	= *CCharMove::GetPosition( pPlayer );
				float	fDistance		= CalcDistanceXZ( &vecPlayerPos, &pNpcInfo->vPos );
				if( fDistance > 3000.0f )
				{
					dwError = eDungeonERR_NoKeyInfo;
				}
			}
			else
			{
				// NPC와의 거리 검사
				VECTOR3 vecPlayerPos	= *CCharMove::GetPosition( pPlayer );
				VECTOR3 vecNpcPos		= *CCharMove::GetPosition( pNpc );
				float	fDistance		= CalcDistanceXZ( &vecPlayerPos, &vecNpcPos );
				if( pNpc->GetUniqueIdx() != pmsg->dwData4 ||
					fDistance > 3000.0f )
					dwError = eDungeonERR_NoNPCInfo;
			}

			CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx());
			if(! pParty)
				dwError = eDungeonERR_NoParty;

			if(dwError != eDungeonERR_None)
			{
				MSG_DWORD	msg;
				msg.Category = MP_DUNGEON;
				msg.Protocol = MP_DUNGEON_ENTRANCE_NACK;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.dwData = dwError;
				pPlayer->SendMsg(&msg, sizeof(msg));
				return;
			}

			eDIFFICULTY difficulty = pKey->difficulty;

			DungeonEntrance(dwConnectionIndex, g_pServerSystem->GetMapNum(), pPlayer->GetID(), pParty->GetPartyIdx(), pmsg->dwData1, 0, 0, pKey->wMapNum, difficulty);
		}
		break;
	case MP_DUNGEON_ENTRANCE_FROM_SERVER_SYN:
		{
			// dwData1(키인덱스), dwData2(요청한맵번호), dwData3(파티인덱스), dwData4(아이템인덱스), dwData5(아이템슬롯)
			MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;

			DWORD dwError = eDungeonERR_None;
			stDungeonKey* pKey = GAMERESRCMNGR->GetDungeonKey(pmsg->dwData1);
			if(! pKey)
				dwError = eDungeonERR_NoKeyInfo;

			if(m_DungeonList.GetDataNum() >= MAX_DUNGEON_NUM)
				dwError = eDungeonERR_DungeonFull;

			if(dwError != eDungeonERR_None)
			{
				// 맵이동 명령 보내기
				MSG_DWORD msgErr;
				msgErr.Category = MP_DUNGEON;
				msgErr.Protocol = MP_DUNGEON_ENTRANCE_NACK;
				msgErr.dwObjectID = pmsg->dwObjectID;
				msgErr.dwData = dwError;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData2, &msgErr, sizeof(msgErr));
			}

			DWORD dwResult = Entrance(pmsg->dwObjectID, pmsg->dwData3, pmsg->dwData1);
			if( eDungeonERR_None == dwResult )
			{
				// 맵이동 명령 보내기
				MSG_DWORD5 msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_DUNGEON;
				msg.Protocol = MP_DUNGEON_ENTRANCE_ACK;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.dwData1 = pmsg->dwData1;
				msg.dwData2 = pKey->dwPosX;
				msg.dwData3 = pKey->dwPosZ;
				msg.dwData4 = pmsg->dwData4;
				msg.dwData5 = pmsg->dwData5;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData2, &msg, sizeof(msg));
			}
			else
			{
				// DB 업데이트
				const BYTE DoRollback = 1;
				DungeonUpdate(pmsg->dwObjectID, (MAPTYPE)pKey->wMapNum, 0, 0, DoRollback);

				// 던전 입장 실패처리
				MSG_DWORD msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category	= MP_DUNGEON;
				msg.Protocol	= MP_DUNGEON_ENTRANCE_NACK;
				msg.dwObjectID	= pmsg->dwObjectID;
				msg.dwData		= dwResult;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData2, &msg, sizeof(msg));
			}
		}
		break;

	case MP_DUNGEON_ENTRANCE_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( !pPlayer )
			{
				return;
			}

			// 해당 플레이어에게 메세지 전송
			MSG_DWORD msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category	= MP_DUNGEON;
			msg.Protocol	= MP_DUNGEON_ENTRANCE_NACK;
			msg.dwObjectID  = pPlayer->GetID();
			msg.dwData		= pmsg->dwData;
			pPlayer->SendMsg( &msg, sizeof( msg ) );
		}
		break;

	case MP_DUNGEON_ENTRANCE_ACK:
		{
			//dwData1(채널) dwData2(posX) dwData3(posZ), dwData4(아이템인덱스), dwData5(아이템슬롯)
			MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;

			stDungeonKey* pKey = GAMERESRCMNGR->GetDungeonKey(pmsg->dwData1);
			if(! pKey)
				return;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				if( pPlayer->IsPKMode() || LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) || pPlayer->GetAutoNoteIdx() )
				{
					MSG_DWORD msg;
					msg.Category	= MP_DUNGEON;
					msg.Protocol	= MP_DUNGEON_ENTRANCE_NACK;
					msg.dwObjectID  = pPlayer->GetID();
					msg.dwData		= eHOUSEERR_ERROR;
					pPlayer->SendMsg( &msg, sizeof( msg ) );
					return;
				}

				MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( pKey->wMapNum );
				if( !ChangeInfo ) return;
				SaveMapChangePointUpdate( pPlayer->GetID(), ChangeInfo->Kind );

				// RaMa - 04.12.14
				pPlayer->UpdateLogoutToDB(FALSE);

				VECTOR3 vStartPos;
				vStartPos.x = (float)pmsg->dwData2;
				vStartPos.y = 0.0f;
				vStartPos.z = (float)pmsg->dwData3;

				pPlayer->SetMapMoveInfo( ChangeInfo->MoveMapNum, (DWORD)vStartPos.x, (DWORD)vStartPos.z);

				// 아이템소모
				ITEMMGR->UseItem(pPlayer, (WORD)pmsg->dwData5, pmsg->dwData4);

				// 081218 LUJ, 플레이어를 해제하기 전에 DB에 업데이트할 주요 정보를 복사해놓는다
				const MAPTYPE	mapType			= pKey->wMapNum;
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

				TCHAR text[MAX_PATH] = {0};
				_sntprintf(
					text,
					_countof(text),
					"map:%d(%d)",
					pKey->wMapNum,
					pKey->difficulty);
				LogItemMoney(
					pPlayer->GetID(),
					pPlayer->GetObjectName(),
					0,
					text,
					eLog_DungeonBegin,
					pPlayer->GetMoney(),
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
		break;

	case MP_DUNGEON_INFO_SYN:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			CObject* pObject = g_pUserTable->FindUser(pmsg->dwObjectID);
			if(! pObject || eObjectKind_Player != pObject->GetObjectKind())
				return;

			stDungeon* pDungeon = m_DungeonList.GetData(pObject->GetGridID());
			if(pDungeon)
			{
				MSG_DUNGEON_INFO msg;
				msg.Category = MP_DUNGEON;
				msg.Protocol = MP_DUNGEON_ENTRANCE_ACK;
				msg.dwObjectID = pmsg->dwObjectID;
				msg.dwIndex = pDungeon->dwIndex;
				msg.dwPartyIndex = pDungeon->dwPartyIndex;
				msg.dwChannelID = pDungeon->dwChannelID;
				msg.dwPoint = pDungeon->dwPoint;
				msg.dwJoinPlayerNum = pDungeon->dwJoinPlayerNum;
				msg.difficulty = pDungeon->difficulty;
				memcpy(msg.warpState, pDungeon->m_WarpState, sizeof(msg.warpState));
				memcpy(msg.SwitchNpcState, pDungeon->m_SwitchNpcState, sizeof(msg.SwitchNpcState));

				pObject->SendMsg( &msg, sizeof(msg) );

				Trigger::CTimer* pTimer = TRIGGERMGR->GetTimer(pDungeon->m_TimerAlias);
				if(pTimer)
				{
					// 타이머가 설정되어 있다. 유저에게 전송하자.
					MSG_DWORD4 msgTimer;
					msgTimer.Category = MP_TRIGGER;
					msgTimer.Protocol = MP_TRIGGER_TIMER_START;
					msgTimer.dwObjectID = pmsg->dwObjectID;
					msgTimer.dwData1 = pTimer->GetType();
					msgTimer.dwData2 = pTimer->GetState();
					if(eTimerType_CountDown == pTimer->GetType())
					{
						msgTimer.dwData3 = pTimer->GetDuration() - pTimer->GetElapsedTime();
						msgTimer.dwData4 = 0;
					}
					else
					{
						msgTimer.dwData3 = pTimer->GetDuration();
						msgTimer.dwData4 = pTimer->GetElapsedTime();
					}
					

					pObject->SendMsg(&msgTimer, sizeof(msgTimer));
				}

				// 100419 ONS 입장시 활성화되어 있는 버퍼를 적용시킨다.
				DungeonBuffMap::iterator iter = m_DungeonBuffMap.find( pObject->GetGridID() );
				if( iter != m_DungeonBuffMap.end() )
				{
					BuffVector& Buff = iter->second;
					BuffVector::iterator it;
					for( it = Buff.begin(); it != Buff.end(); ++it)
					{
						DWORD dwBuffIndex = *it;

						if(const cBuffSkillInfo* const skillInfo = SKILLMGR->GetBuffInfo(dwBuffIndex))
						{
							SKILLMGR->BuffSkillStart(
								pmsg->dwObjectID,
								dwBuffIndex,
								skillInfo->GetInfo().DelayTime,
								skillInfo->GetInfo().Count );
						}
					}
				}
				// 던전 처리자에게 전달.
				m_pDungeonProcessor->Info_Syn(pObject);
			}
		}
		break;
	// 091119 ONS 인던 패이드무브 메세지 처리추가
	case MP_DUNGEON_FADEMOVE_SYN:
		{
			MSG_DUNGEON_FADE_MOVE* pmsg = (MSG_DUNGEON_FADE_MOVE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwMoverID) ;
			if( !pPlayer ) return ;

			// 위치 변경하기.
			VECTOR3 pos = { 0 };
			pmsg->cpos.Decompress(&pos) ;

			if( !CheckValidPosition(pos) ) return ;

			CCharMove::SetPosition(pPlayer,&pos) ;
			
			pmsg->Protocol = MP_DUNGEON_FADEMOVE_ACK;
			pPlayer->SendMsg(pmsg, sizeof(MSG_DUNGEON_FADE_MOVE));		
		}
		break;

	default:
		break;
	}
}

stDungeon* CDungeonMgr::Create(DWORD dwPartyID, DWORD dwChannelID, eDIFFICULTY diffculty)
{
	stDungeon* pInst = m_DungeonPool->Alloc();

	if(pInst)
	{
		// 인던을 생성했다.
		pInst->Clear();
		pInst->dwPartyIndex = dwPartyID;
		pInst->dwChannelID = dwChannelID;
		pInst->difficulty = diffculty;
		
		m_ChannelList.Add(&pInst->dwChannelID, pInst->dwPartyIndex);
		m_DungeonList.Add(pInst, pInst->dwChannelID);


		if(m_pDungeonProcessor)
			m_pDungeonProcessor->Create(pInst);

		// Todo : 인던 초기화 (몹생성등....)
		SetMonsterList(dwChannelID, diffculty);
		InitWarpList(dwChannelID);
		InitSwitchNpcList(dwChannelID);
	}

	return pInst;
}

void CDungeonMgr::Remove(DWORD dwChannelID)
{
	stDungeon* pInst = m_DungeonList.GetData(dwChannelID);
	if(pInst)
	{
		CObject* pObject = NULL;
		g_pUserTable->SetPositionUserHeadChannel(dwChannelID);
		while((pObject = (CObject*)g_pUserTable->GetUserDataChannel(dwChannelID)) != NULL)
		{
			if(pObject->GetObjectKind() & eObjectKind_Monster)
			{
				g_pServerSystem->RemoveMonster(pObject->GetID());
			}
			else if(pObject->GetObjectKind() == eObjectKind_Player)
			{
				// 파티인덱스가 틀리다. 강제추방!
				if(pInst->dwPartyIndex != ((CPlayer*)pObject)->GetPartyIdx())
					MoveToLoginMap(((CPlayer*)pObject)->GetID());

				// 마지막 플레이어가 나갔을때 호출된다. 패스~
				continue;
			}
			else if(pObject->GetObjectKind() == eObjectKind_Npc)
			{
				g_pServerSystem->RemoveNpc(pObject->GetID());
			}
			else if(pObject->GetObjectKind() == eObjectKind_Pet)
			{
				// 펫은 플레이어 Remove처리에서 이루어진다.
				continue;
			}
			else if(pObject->GetObjectKind() == eObjectKind_Vehicle)
			{
				// 탈것은 플레이어 Remove처리에서 이루어진다.
				continue;
			}
			else if(pObject->GetObjectKind() == eObjectKind_SkillObject)
			{
				g_pServerSystem->RemoveSkillObject(pObject->GetID());
			}
			else if(pObject->GetObjectKind() == eObjectKind_MapObject)
			{
				g_pServerSystem->RemoveMapObject(pObject->GetID());
			}
			else
			{
				// 스킬오브젝트 패스
				cSkillObject* pSkill = SKILLMGR->GetSkillObject(pObject->GetID());
				if(pSkill)				
					continue;

				// 지워질 오브젝트가 있다.
				EObjectKind objectKind = eObjectKind_None;
				objectKind = (EObjectKind)pObject->GetObjectKind();

				char LogFile[256] = {0,};
				char buf[256] = {0,};

				sprintf(LogFile, "Log/Dungeon_%d.log", g_pServerSystem->GetMapNum());
				FILE* fp = fopen(LogFile, "a+");
				
				if(fp)
				{
					SYSTEMTIME st;
					GetLocalTime(&st);

					sprintf(buf, "%d_%d %d:%d : UnRemoved ObjectKind : %d\n", st.wMonth, st.wDay, st.wHour, st.wMinute, objectKind);
					fprintf(fp, buf);
					fflush(fp);
					fclose(fp);
				}
				continue;
			}
		}

		BATTLESYSTEM->DestroyChannel(pInst->dwChannelID);
		BATTLESYSTEM->ReleaseBattleID(pInst->dwChannelID);
		m_ChannelList.Remove(pInst->dwPartyIndex);
		m_DungeonList.Remove(pInst->dwChannelID);
		m_DungeonPool->Free(pInst);

		m_DungeonBuffMap.erase( pInst->dwChannelID );
	}
}

DWORD CDungeonMgr::Entrance(DWORD dwPlayerID, DWORD dwPartyID, DWORD dwKeyIndex)
{
	stDungeonKey* pKey = GAMERESRCMNGR->GetDungeonKey(dwKeyIndex);
	if(! pKey)
		return eDungeonERR_NoKeyInfo;

	DWORD* pChannelID = m_ChannelList.GetData(dwPartyID);
	if(! pChannelID)
	{
		DWORD dwNewChannel = BATTLESYSTEM->CreateChannel();
		stDungeon* pCreateDungeon = Create(dwPartyID, dwNewChannel, pKey->difficulty);
		if( !pCreateDungeon )
		{
			return eDungeonERR_CreateFailed;
		}

		ReservationDungeon(dwPlayerID, dwPartyID, dwNewChannel, dwKeyIndex);
	}
	else
	{
		stDungeon* pDungeon = m_DungeonList.GetData(*pChannelID);
		if( !pDungeon )
		{
			return eDungeonERR_NoChannel;
		}
		
		if( pKey->difficulty != pDungeon->difficulty )
		{
			// 난이도가 다른 던전에 입장할 수 없다.
			return eDungeonERR_WrongDiffculty;
		}

		ReservationDungeon(dwPlayerID, dwPartyID, *pChannelID, dwKeyIndex);
	}

	return eDungeonERR_None;
}

void CDungeonMgr::Exit(DWORD dwPlayerID)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwPlayerID);
	if(pPlayer && !pPlayer->IsDungeonObserver())
	{
		DWORD dwChannelID = pPlayer->GetChannelID();
		stDungeon* pDungeon = m_DungeonList.GetData(dwChannelID);

		if(pDungeon)
		{
			pDungeon->dwJoinPlayerNum--;

			if(0 == pDungeon->dwJoinPlayerNum)
			{
				Remove(dwChannelID);
			}
		}

		RemoveNoPartyList(dwPlayerID);
	}
}

void CDungeonMgr::ReservationDungeon(DWORD dwPlayerID, DWORD dwPartyID, DWORD dwChannelID, DWORD dwKeyIndex)
{
	stDungeonReservation* pReservation = NULL;
	stDungeonKey* pKey = GAMERESRCMNGR->GetDungeonKey(dwKeyIndex);
	if(! pKey)
		return;

	pReservation = m_ReservationList.GetData(dwPlayerID);
	if(pReservation)
	{
		pReservation->dwCharIndex = dwPlayerID;
		pReservation->dwPartyIndex = dwPartyID;
		pReservation->dwChannelID = dwChannelID;
		pReservation->vStartPos.x = (float)pKey->dwPosX;
		pReservation->vStartPos.y = 0.0f;
		pReservation->vStartPos.z = (float)pKey->dwPosZ;
		pReservation->dwRegistTime = gCurTime;
	}
	else
	{
		pReservation = m_ReservationPool->Alloc();
		if(pReservation)
		{
			pReservation->Clear();
			pReservation->dwCharIndex = dwPlayerID;
			pReservation->dwPartyIndex = dwPartyID;
			pReservation->dwChannelID = dwChannelID;
			pReservation->vStartPos.x = (float)pKey->dwPosX;
			pReservation->vStartPos.y = 0.0f;
			pReservation->vStartPos.z = (float)pKey->dwPosZ;
			pReservation->dwRegistTime = gCurTime;

			m_ReservationList.Add(pReservation, dwPlayerID);
		}
	}
}

void CDungeonMgr::PlayerEntered(DWORD dwPlayerID, DWORD dwChannelID)
{
	stDungeon* pDungeon = m_DungeonList.GetData(dwChannelID);
	if(pDungeon)
	{
		pDungeon->dwJoinPlayerNum++;
		stDungeonReservation* pReservation = m_ReservationList.GetData(dwPlayerID);
		if(pReservation)
		{
			m_ReservationList.Remove(dwPlayerID);
			m_ReservationPool->Free(pReservation);
		}
	}
}

DWORD CDungeonMgr::GetChannelIDFromReservationList(DWORD dwPlayerID)
{
	stDungeonReservation* pReservation = m_ReservationList.GetData(dwPlayerID);
	if(pReservation)
	{
		return pReservation->dwChannelID;
	}

	FILE* fpLog = NULL;
	fpLog = fopen( "./Log/DungeonEnterLog.txt", "a+" );
	if( fpLog )
	{
		SYSTEMTIME sysTime;
		GetLocalTime( &sysTime );

		fprintf( fpLog, "[%04d-%02d-%02d %02d:%02d:%02d] %s - not exist reservation data [PlayerID : %d] \n", 
			sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
			__FUNCTION__,
			dwPlayerID );

		fclose( fpLog );
	}

	return 0;
}

VECTOR3* CDungeonMgr::GetStartPosFromeservationList(DWORD dwPlayerID)
{
	stDungeonReservation* pReservation = m_ReservationList.GetData(dwPlayerID);
	if(pReservation)
	{
		PlayerEntered(dwPlayerID, pReservation->dwChannelID);
		return &pReservation->vStartPos;
	}

	return NULL;
}

DWORD CDungeonMgr::GetPlayerNumInDungeon(DWORD dwChannelID)
{
	stDungeon* pDungeon = m_DungeonList.GetData( dwChannelID );
	if( pDungeon != NULL )
		return pDungeon->dwJoinPlayerNum;

	return 0;
}

DWORD CDungeonMgr::GetPartyIndexFromDungeon(DWORD dwChannelID)
{
	stDungeon* pID = m_DungeonList.GetData(dwChannelID);
	if(pID)
		return pID->dwPartyIndex;

	return 0;
}

DWORD CDungeonMgr::GetDungeonNum()
{
	return m_DungeonList.GetDataNum();
}

DWORD CDungeonMgr::GetPlayerNum()
{
	return g_pUserTable->GetUserCount();
}

DWORD CDungeonMgr::GetMonsterNum()
{
	return g_pUserTable->GetMonsterCount();
}

DWORD CDungeonMgr::GetNpcNum()
{
	return g_pUserTable->GetNpcCount();
}

DWORD CDungeonMgr::GetExtraNum()
{
	return g_pUserTable->GetExtraCount();
}

void CDungeonMgr::AddPoint(DWORD dwChannelID, DWORD value)
{
	stDungeon* pID = m_DungeonList.GetData(dwChannelID);
	if(pID)
		pID->dwPoint += value;
}

void CDungeonMgr::SubtractPoint(DWORD dwChannelID, DWORD value)
{
	stDungeon* pID = m_DungeonList.GetData(dwChannelID);
	if(pID)
	{
		if(pID->dwPoint > value)
			pID->dwPoint -= value;
		else
			pID->dwPoint = 0;
	}
}

DWORD CDungeonMgr::GetPoint(DWORD dwChannelID)
{
	stDungeon* pID = m_DungeonList.GetData(dwChannelID);
	if(pID)
		return pID->dwPoint;

	return 0;
}

void CDungeonMgr::SendMsgToChannel(MSGBASE* pMsg, DWORD dwLength, DWORD dwChannelID)
{
	if(! pMsg)
		return;

	CObject* pObject;
	g_pUserTable->SetPositionHead();
	while((pObject = g_pUserTable->GetData()) != NULL)
	{
		if(pObject &&
			eObjectKind_Player == pObject->GetObjectKind() &&
			pObject->GetGridID() == dwChannelID)
		{
			PACKEDDATA_OBJ->QuickSend( pObject, pMsg, dwLength );
		}
	}
}

void CDungeonMgr::SetSwitch(DWORD dwChannelID, WORD num, BOOL val)
{
	if(FALSE == DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
		return;

	if(MAX_DUNGEON_SWITCH <= num)
		return;

	stDungeon* pID = m_DungeonList.GetData(dwChannelID);
	if(pID)
		pID->mSwitch[num] = val;

	m_pDungeonProcessor->SetSwitch(dwChannelID, num, val);
}

int CDungeonMgr::GetSwitch(DWORD dwChannelID, WORD num)
{
	if(FALSE == DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
		return -1;

	if(MAX_DUNGEON_SWITCH <= num)
		return -1;

	stDungeon* pID = m_DungeonList.GetData(dwChannelID);
	if(pID)
		return pID->mSwitch[num];

	return -1;
}

void CDungeonMgr::ResetSwitch(DWORD dwChannelID)
{
	if(FALSE == DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
		return;

	stDungeon* pID = m_DungeonList.GetData(dwChannelID);
	if(pID)
		ZeroMemory(pID->mSwitch, sizeof(pID->mSwitch));
}

void CDungeonMgr::AddNoPartyList(DWORD dwPlayerID, DWORD dwPartyID)
{
	if(FALSE == DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
		return;

	if(0 == g_pUserTable->FindUser(dwPlayerID))
		return;

	stDungeonNoPartyList* pList = m_NoPartyPool->Alloc();
	if(pList)
	{
		pList->dwCharIndex = dwPlayerID;
		pList->dwOldPartyIndex = dwPartyID;
		pList->dwOutTime = gCurTime;

		m_NoPartyList.Add(pList, pList->dwCharIndex);
	}
}

void CDungeonMgr::RemoveNoPartyList(DWORD dwPlayerID)
{
	m_NoPartyList.Remove(dwPlayerID);
}

void CDungeonMgr::MoveToLoginMap(DWORD dwCharIndex)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(dwCharIndex);
	if(pPlayer)
	{
		MAPTYPE mapNum = pPlayer->GetLoginMapNum();
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

		Exit(pPlayer->GetID());
	}
}

void CDungeonMgr::LoadMonsterList(CMHFile& file)
{
	m_MonsterList_Easy.RemoveAll();
	m_MonsterList_Normal.RemoveAll();
	m_MonsterList_Hard.RemoveAll();

	char StrBuf[256] = {0,};

	while(1)
	{
		if(file.IsEOF())
			break;

		file.GetString(StrBuf);
		if( StrBuf[0] != '#' )
		{
			file.GetLine(StrBuf,256);
			continue;
		}
		else if( strcmp( StrBuf , "#MONSTER" ) == 0 )
		{
			const eDIFFICULTY difficulty = (eDIFFICULTY)file.GetInt();
			stMonsterList* const pMonster = new stMonsterList;
			ZeroMemory(
				pMonster,
				sizeof(*pMonster));

			if(pMonster)
			{
				pMonster->dwIndex = file.GetDword();
				pMonster->dwPosX = file.GetWord();
				pMonster->dwPosZ = file.GetWord();
				strcpy(pMonster->Alias, file.GetString());
				strcpy(pMonster->GroupAlias, file.GetString());
				pMonster->bActive = file.GetBool();				// 091019 ONS 몬스터 소환 초기치 설정

				switch(difficulty)
				{
				case eDIFFICULTY_EASY:
					m_MonsterList_Easy.Add(pMonster, pMonster->dwIndex);
					break;

				case eDIFFICULTY_NORMAL:
					m_MonsterList_Normal.Add(pMonster, pMonster->dwIndex);
					break;

				case eDIFFICULTY_HARD:
					m_MonsterList_Hard.Add(pMonster, pMonster->dwIndex);
					break;
				}
			}
		}
		// 100331 ShinJS --- Regen Script를 사용할수 있도록 함.
		else if( strcmp( StrBuf , "#REGEN_FILE_EASY" ) == 0 )
		{
			char regenFilename[MAX_PATH]={0,};
			strncpy( regenFilename, file.GetString(), MAX_PATH );
			LoadMonsterRegenFile( regenFilename, m_MonsterList_Easy, eDIFFICULTY_EASY );
		}
		else if( strcmp( StrBuf , "#REGEN_FILE_NORMAL" ) == 0 )
		{
			char regenFilename[MAX_PATH]={0,};
			strncpy( regenFilename, file.GetString(), MAX_PATH );
			LoadMonsterRegenFile( regenFilename, m_MonsterList_Normal, eDIFFICULTY_NORMAL );
		}
		else if( strcmp( StrBuf , "#REGEN_FILE_HARD" ) == 0 )
		{
			char regenFilename[MAX_PATH]={0,};
			strncpy( regenFilename, file.GetString(), MAX_PATH );
			LoadMonsterRegenFile( regenFilename, m_MonsterList_Hard, eDIFFICULTY_HARD );
		}
		else if( strcmp( StrBuf , "#MONSTER_LIST_END" ) == 0 )
		{
			break;
		}
	}
}

void CDungeonMgr::LoadMonsterRegenFile( char* filename, CYHHashTable<stMonsterList>& monsterList, eDIFFICULTY eDiff )
{
	BOOL bIsComment = FALSE;

	char filepath[MAX_PATH]={0,};
	strcpy( filepath, "./System/Resource/" );
	strcat( filepath, filename );

	CMHFile file;
	file.Init( filepath, "rb" );
	BOOL bIsRegen = FALSE;

	char groupName[MAX_PATH]={0,};

	while( ! file.IsEOF() )
	{
		char txt[ MAX_PATH ] = {0,};
		file.GetLine( txt, MAX_PATH );

		int txtLen = _tcslen( txt );

		// 중간 주석 제거
		for( int i=0 ; i<txtLen-1 ; ++i )
		{
			if( txt[i] == '/' && txt[i+1] == '/' )
			{
				txt[i] = 0;
				break;
			}
			else if( txt[i] == '/' && txt[i+1] == '*' )
			{
				txt[i] = 0;
				bIsComment = TRUE;
			}
			else if( txt[i] == '*' && txt[i+1] == '/' )
			{
				txt[i] = ' ';
				txt[i+1] = ' ';
				bIsComment = FALSE;
			}
			else if( bIsComment )
			{
				txt[i] = ' ';
			}
		}

		char buff[ MAX_PATH ] = {0,};
		SafeStrCpy( buff, txt, MAX_PATH );
		char* delimit = " \n\t";
		char* token = strtok( buff, delimit );

		if( ! token )
		{
			continue;
		}
		else if( ! stricmp( "#GROUPNAME", token ) )
		{
			token = strtok( 0, delimit );
			if( !token )	continue;
			strncpy( groupName, token, MAX_PATH );
		}
		else if( ! stricmp( "#ADDCONDITION", token ) )
		{
			token = strtok( 0, delimit );
			if( !token )	continue;

			token = strtok( 0, delimit );
			if( !token )	continue;

			token = strtok( 0, delimit );
			if( !token )	continue;

			token = strtok( 0, delimit );
			if( !token )	continue;

			bIsRegen = BOOL( _ttoi( token ) );
		}
		else if ( ! stricmp( "#ADD", token ) )
		{
			WORD wPosX=0, wPosZ=0;
			DWORD dwMonsterIdx = 0;

			// object kind
			token = strtok( 0, delimit );
			if( !token )	continue;

			// regen index
			token = strtok( 0, delimit );
			if( !token )	continue;

			// monster kind
			token = strtok( 0, delimit );
			if( !token )	continue;
			dwMonsterIdx = DWORD( _ttoi( token ) );

			// pos x
			token = strtok( 0, delimit );
			if( !token )	continue;
			wPosX = WORD( _ttoi( token ) );

			// pos z
			token = strtok( 0, delimit );
			if( !token )	continue;
			wPosZ = WORD( _ttoi( token ) );

			LPCTSTR finiteStateMachine = _tcstok(
				NULL,
				delimit);

			stMonsterList* const pMonster = new stMonsterList;
			ZeroMemory(
				pMonster,
				sizeof(*pMonster));
			pMonster->difficulty = eDiff;
			pMonster->dwIndex = dwMonsterIdx;
			pMonster->dwPosX = wPosX;
			pMonster->dwPosZ = wPosZ;
			strncpy( pMonster->Alias, groupName, sizeof(pMonster->Alias) );
			strncpy( pMonster->GroupAlias, groupName, sizeof(pMonster->GroupAlias) );
			pMonster->bActive = bIsRegen;
			SafeStrCpy(
				pMonster->FiniteStateMachine,
				finiteStateMachine ? finiteStateMachine : "",
				_countof(pMonster->FiniteStateMachine));

			monsterList.Add( pMonster, pMonster->dwIndex );
		}
	}
}

void CDungeonMgr::SetMonsterList(DWORD dwChannelID, eDIFFICULTY difficulty)
{
	switch(difficulty)
	{
	case eDIFFICULTY_EASY:
		{
			stMonsterList* pMonster = NULL;
			m_MonsterList_Easy.SetPositionHead();
			while((pMonster = m_MonsterList_Easy.GetData()) != NULL)
			{
				// 091019 ONS 초기설정(InitActive == TRUE) 몬스터만 추가한다.
				if(pMonster->bActive)
				{
					AddMonster(pMonster, dwChannelID, eDIFFICULTY_EASY);
				}
			}
		}
		break;
	
	case eDIFFICULTY_NORMAL:
		{
			stMonsterList* pMonster = NULL;
			m_MonsterList_Normal.SetPositionHead();
			while((pMonster = m_MonsterList_Normal.GetData()) != NULL)
			{
				if(pMonster->bActive)
				{
					AddMonster(pMonster, dwChannelID, eDIFFICULTY_NORMAL);
				}
			}
		}
		break;

	case eDIFFICULTY_HARD:
		{
			stMonsterList* pMonster = NULL;
			m_MonsterList_Hard.SetPositionHead();
			while((pMonster = m_MonsterList_Hard.GetData()) != NULL)
			{
				if(pMonster->bActive)
				{
					AddMonster(pMonster, dwChannelID, eDIFFICULTY_HARD);
				}
			}
		}
		break;
	}
}

CMonster* CDungeonMgr::AddMonster(stMonsterList* pMonster, DWORD dwChannelID, eDIFFICULTY difficulty)
{
	if(! pMonster)
		return NULL;

	BASE_MONSTER_LIST* pMonsterList = GAMERESRCMNGR->GetMonsterListInfo(pMonster->dwIndex);
	if(! pMonsterList)
		return NULL;

	DWORD alias = 0;
	DWORD groupAlias = 0;


	if(0 != strcmp("0", pMonster->Alias))
	{
		// 별칭은 맵전용으로 등록한다.
		char buffer[MAX_PATH] = {0};
		sprintf(
			buffer,
			"%s.%d.%d.%d",
			pMonster->Alias,
			Trigger::eOwner_Server,
			dwChannelID,
			dwChannelID);
		alias = TRIGGERMGR->GetHashCode(buffer);
	}

	if(0 != strcmp("0", pMonster->GroupAlias))
	{
		// 그룹별칭은 맵전용으로 등록한다.
		char buffer[MAX_PATH] = {0};
		sprintf(
			buffer,
			"%s.%d.%d.%d",
			pMonster->GroupAlias,
			Trigger::eOwner_Server,
			dwChannelID,
			dwChannelID);
		groupAlias = TRIGGERMGR->GetHashCode(buffer);
	}

	VECTOR3 vPos = {0};
	vPos.x = (float)pMonster->dwPosX;
	vPos.z = (float)pMonster->dwPosZ;

	// 100104 LUJ, 서브 인덱스는 리젠 스크립트(Monster_%02d.bin)를 읽을 때
	//			생성되는 고유 번호이다. 따라서 리젠 스크립트에서 생성되지
	//			않는 몬스터는 0이어도 관계없다
	CMonster* pAddMonster = REGENMGR->RegenObject(
		g_pAISystem.GeneraterMonsterID(),
		0,
		dwChannelID,
		pMonsterList->ObjectKind,
		(WORD)pMonster->dwIndex,
		&vPos,
		groupAlias ? groupAlias : (DWORD)DUNGEON_MONSTER_GROUP_ID,
		0,
		0,
		FALSE);

	if(pAddMonster)
	{
		pAddMonster->SetAlias(
			alias);
		pAddMonster->SetGroupAlias(
			groupAlias);
		pAddMonster->SetDomainPosition(
			vPos);
		pAddMonster->SetFixedPosition(
			TRUE);
		pAddMonster->GetFiniteStateMachine().Initialize(
			pMonster->FiniteStateMachine,
			pAddMonster->GetID(),
			pAddMonster->GetGridID());
		SafeStrCpy(
			pAddMonster->GetMonsterTotalInfo().mScriptName,
			pMonster->FiniteStateMachine,
			_countof(pAddMonster->GetMonsterTotalInfo().mScriptName));
	}

	return pAddMonster;
}

void CDungeonMgr::LoadWarpList(CMHFile& file)
{
	m_WarpList.RemoveAll();

	char StrBuf[256] = {0,};

	while(1)
	{
		if(file.IsEOF())
			break;

		file.GetString(StrBuf);
		if( StrBuf[0] != '#' )
		{
			file.GetLine(StrBuf,256);
			continue;
		}
		else if( strcmp( StrBuf , "#WARP" ) == 0 )
		{
			if(m_WarpList.GetDataNum() >= MAX_DUNGEON_WARP)
			{
				// 에러 최대갯수보다 많다.
				return;
			}

			stWarpInfo* pWarp = new stWarpInfo;
			if(pWarp)
			{
				pWarp->dwIndex = file.GetDword();
				pWarp->wMapNum = file.GetWord();
				pWarp->wNpcJob = file.GetWord();
				strcpy(pWarp->name, file.GetString());
				pWarp->wRadius = file.GetWord();
				pWarp->wCurPosX = file.GetWord();
				pWarp->wCurPosZ = file.GetWord();
				pWarp->wDstPosX = file.GetWord();
				pWarp->wDstPosZ = file.GetWord();
				pWarp->bActive = file.GetBool();

				m_WarpList.Add(pWarp, pWarp->dwIndex);
			}
		}
		else if( strcmp( StrBuf , "#WARP_LIST_END" ) == 0 )
		{
			break;
		}
	}
}

void CDungeonMgr::LoadSwitchList(CMHFile& file)
{
	m_SwitchNpcList.RemoveAll();

	char StrBuf[256] = {0,};

	while(1)
	{
		if(file.IsEOF())
			break;

		file.GetString(StrBuf);
		if( StrBuf[0] != '#' )
		{
			file.GetLine(StrBuf,256);
			continue;
		}
		else if( strcmp( StrBuf , "#SWITCH" ) == 0 )
		{
			stSwitchNpcInfo* pSwitchNpc = new stSwitchNpcInfo;
			if(pSwitchNpc)
			{
				pSwitchNpc->wIndex = file.GetWord();
				pSwitchNpc->wMapNum = file.GetWord();
				pSwitchNpc->wNpcJob = file.GetWord();
				strcpy(pSwitchNpc->name, file.GetString());
				pSwitchNpc->wRadius = file.GetWord();
				pSwitchNpc->wPosX = file.GetWord();
				pSwitchNpc->wPosZ = file.GetWord();
				pSwitchNpc->bActive = file.GetBool();

				m_SwitchNpcList.Add(pSwitchNpc, pSwitchNpc->wIndex);
			}
		}
		else if( strcmp( StrBuf , "#SWITCH_LIST_END" ) == 0 )
		{
			break;
		}
	}
}

void CDungeonMgr::InitWarpList(DWORD dwChannelID)
{
	stDungeon* pDungeon = m_DungeonList.GetData(dwChannelID);
	if(! pDungeon)
		return;

	stWarpInfo* pWarp;
	m_WarpList.SetPositionHead();
	while((pWarp = m_WarpList.GetData()) != NULL)
	{
		pDungeon->m_WarpState[pDungeon->m_WarpEmptyPos].dwIndex = pWarp->dwIndex;
		pDungeon->m_WarpState[pDungeon->m_WarpEmptyPos].bActive = pWarp->bActive;
		pDungeon->m_WarpEmptyPos++;
	}
}

void CDungeonMgr::InitSwitchNpcList(DWORD dwChannelID)
{
	stDungeon* pDungeon = m_DungeonList.GetData(dwChannelID);
	if(! pDungeon)
		return;

	stSwitchNpcInfo* pSwitchNpc;
	m_SwitchNpcList.SetPositionHead();
	while((pSwitchNpc = m_SwitchNpcList.GetData()) != NULL)
	{

		pDungeon->m_SwitchNpcState[pDungeon->m_SwitchNpcEmptyPos].wIndex = pSwitchNpc->wIndex;
		pDungeon->m_SwitchNpcState[pDungeon->m_SwitchNpcEmptyPos].bActive = pSwitchNpc->bActive;
		if(pSwitchNpc->bActive)
		{
			DWORD dwNpcIndex = AddNpc( pSwitchNpc, dwChannelID );
			pDungeon->m_SwitchNpcState[pDungeon->m_SwitchNpcEmptyPos].dwObjectIndex = dwNpcIndex;
		}
		pDungeon->m_SwitchNpcEmptyPos++;
	}
}

void CDungeonMgr::SetWarp(DWORD dwChannelID, DWORD dwWarpIndex, BOOL bActive)
{
	stDungeon* pDungeon = m_DungeonList.GetData(dwChannelID);
	if(! pDungeon)
		return;

	stWarpState* pWarpState = pDungeon->GetWarp(dwWarpIndex);
	if(! pWarpState || pWarpState->bActive==bActive)
		return;

	pWarpState->bActive = bActive;

	// 클라이언트에 알려주자~
	MSG_DWORD3 msg;
	msg.Category = MP_DUNGEON;
	msg.Protocol = MP_DUNGEON_SETWARP_ACK;
	msg.dwData1 = dwChannelID;
	msg.dwData2 = dwWarpIndex;
	msg.dwData3 = bActive;
	SendMsgToChannel(&msg, sizeof(msg), dwChannelID);
}

void CDungeonMgr::SetSwitchNpc(DWORD dwChannelID, WORD wSwitchNpcIndex, BOOL bActive)
{
	stDungeon* pDungeon = m_DungeonList.GetData(dwChannelID);
	if(! pDungeon)
		return;

	stSwitchNpcState* pSwitchNpcState = pDungeon->GetSwitchNpc(wSwitchNpcIndex);
	if(! pSwitchNpcState || pSwitchNpcState->bActive==bActive)
		return;

	stSwitchNpcInfo* pInfo = m_SwitchNpcList.GetData(wSwitchNpcIndex);
	if(!pInfo)
		return;

	if(bActive)
	{
		const DWORD dwNpcIndex = AddNpc( pInfo, dwChannelID );
		pSwitchNpcState->dwObjectIndex = dwNpcIndex;
	}
	else
	{
		if(pSwitchNpcState->dwObjectIndex)
		{
			g_pServerSystem->RemoveNpc(pSwitchNpcState->dwObjectIndex);
			pSwitchNpcState->dwObjectIndex = 0;
		}
	}

	pSwitchNpcState->bActive = bActive;
}

void CDungeonMgr::SetBossMonster(DWORD dwChannelID)
{
	stDungeon* pDungeon = m_DungeonList.GetData(dwChannelID);
	if(pDungeon)
	{
		if(m_pDungeonProcessor)
			m_pDungeonProcessor->SetBossMonster(pDungeon);
		else
			DUNGEON_ASSERTMSG(0, "m_pDungeonProcessor->SetBossMonster() 실패!");
	}
}

void CDungeonMgr::AddTimer(DWORD dwChannelID, DWORD timerAlias)
{
	stDungeon* pDungeon = m_DungeonList.GetData(dwChannelID);
	if(pDungeon)
	{
		pDungeon->m_TimerAlias = timerAlias;
	}
}

void CDungeonMgr::RemoveTimer(DWORD dwChannelID)
{
	stDungeon* pDungeon = m_DungeonList.GetData(dwChannelID);
	if(pDungeon)
	{
		pDungeon->m_TimerAlias = 0;
	}
}

void CDungeonMgr::LoadDungeonInfo()
{
	char buf[MAX_PATH] = {0,};
	sprintf(buf, "%s%d", "system/resource/DungeonInfo_", g_pServerSystem->GetMapNum());
	strcat(buf, ".bin");
	
	CMHFile file;
	if(! file.Init( buf, "rb" ))
		return;

	char StrBuf[256] = {0,};

	while(1)
	{
		if(file.IsEOF())
			break;

		file.GetString(StrBuf);

		if( StrBuf[0] != '#' )
		{
			file.GetLine(StrBuf,256);
			continue;
		}
		else if( strcmp( StrBuf , "#WARP_LIST" ) == 0 )
		{
			LoadWarpList(file);
		}
		else if( strcmp( StrBuf , "#SWITCH_LIST" ) == 0 )
		{
			LoadSwitchList(file);
		}
		else if( strcmp( StrBuf , "#MONSTER_LIST" ) == 0 )
		{
			LoadMonsterList(file);
		}
	}
}

DWORD CDungeonMgr::GetElapsedSecondFromStartTime(DWORD dwChannelID)
{
	stDungeon* pID = m_DungeonList.GetData(dwChannelID);
	if(pID)
	{
		return (gCurTime - pID->dwStartTime) / 1000;
	}

	return 0;
}

eDIFFICULTY CDungeonMgr::GetDifficulty(DWORD dwChannelID)
{
	stDungeon* pID = m_DungeonList.GetData(dwChannelID);
	if(pID)
		return pID->difficulty;

	return eDIFFICULTY_NONE;
}

eDIFFICULTY CDungeonMgr::GetDifficultyFromKey(DWORD keyItemIndex)
{
	const ITEM_INFO* info = ITEMMGR->GetItemInfo( keyItemIndex );
	if(info && (ITEM_KIND_DUNGEON_KEY==info->SupplyType))
	{
		stDungeonKey* pKey = GAMERESRCMNGR->GetDungeonKey(info->SupplyValue);
		if(pKey)
			return pKey->difficulty;
	}

	return eDIFFICULTY_NONE;
}

BOOL CDungeonMgr::IsDungeon(MAPTYPE mapType) const
{
	for(size_t i = 0; i < _countof(DungeonProcessorTable); ++i)
	{
		const stDungeonProcessor& processor = DungeonProcessorTable[i];

		if(processor.mapnum == mapType)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CDungeonMgr::SummonMonsterGroup(DWORD dwAliasValue, DWORD dwChannelID, eAliasKind eAlias)
{
	// 트리거에서 지정한 몬스터(그룹) 별칭(dwGroupAlias) 과 DungeonInfo.bin의 MonsterList에
	// 지정한 Alias, GroupAlias를 비교하여 해당하는 몬스터를 소환한다.
	stDungeon* pInfo = m_DungeonList.GetData(dwChannelID);
	if(!pInfo)
		return;

	switch(pInfo->difficulty)
	{
	case eDIFFICULTY_EASY:
		{
			m_MonsterList_Easy.SetPositionHead();

			for(stMonsterList* pMonster = m_MonsterList_Easy.GetData();
				0 < pMonster;
				pMonster = m_MonsterList_Easy.GetData())
			{
				if (dwAliasValue == GetAliasHashCode(pMonster, dwChannelID, eAlias) && !pMonster->bActive )
				{
					AddMonster(pMonster, dwChannelID, eDIFFICULTY_EASY);
				}
			}
		}
		break;
	
	case eDIFFICULTY_NORMAL:
		{
			m_MonsterList_Normal.SetPositionHead();

			for(stMonsterList* pMonster = m_MonsterList_Normal.GetData();
				0 < pMonster;
				pMonster = m_MonsterList_Normal.GetData())
			{
				if( dwAliasValue == GetAliasHashCode(pMonster, dwChannelID, eAlias) && !pMonster->bActive )
				{
					AddMonster(pMonster, dwChannelID, eDIFFICULTY_NORMAL);
				}
			}
		}
		break;

	case eDIFFICULTY_HARD:
		{
			m_MonsterList_Hard.SetPositionHead();

			for(stMonsterList* pMonster = m_MonsterList_Hard.GetData();
				0 < pMonster;
				pMonster = m_MonsterList_Hard.GetData())
			{
				if( dwAliasValue == GetAliasHashCode(pMonster, dwChannelID, eAlias) && !pMonster->bActive )
				{
					AddMonster(pMonster, dwChannelID, eDIFFICULTY_HARD);
				}
			}
		}
		break;		
	}
}

DWORD CDungeonMgr::GetAliasHashCode( stMonsterList* pMonster, DWORD dwChannelID, eAliasKind eAlias)
{
	// 트리거에서 지정한 Alias, GroupAlias와 DungeonList.bin의 몬스터리스트에서 지정한
	// Alias, GroupAlias를 비교하기 위해서 해쉬코드를 반환한다.
	char* pAlias = NULL;
	if(eAlias == eAliasKind_Alias)
		pAlias = pMonster->Alias;
	else
		pAlias = pMonster->GroupAlias;

	if(0 == strcmp("0", pAlias)) 
		return 0;

	char buffer[MAX_PATH] = {0};
	sprintf(
		buffer,
		"%s.%d.%d.%d",
		pAlias,
		Trigger::eOwner_Server,
		dwChannelID,
		dwChannelID);

	return TRIGGERMGR->GetHashCode(buffer);
}

BOOL CDungeonMgr::CheckHackNpc( CPlayer* pPlayer, WORD wNpcIdx )
{
	stSwitchNpcInfo* pInfo = m_SwitchNpcList.GetData(wNpcIdx);
    if( pInfo == NULL )
	{
		return FALSE;
	}
	
	VECTOR3 vPos;
	VECTOR3 vNpcPos = {0,};
	vNpcPos.x = pInfo->wPosX;
	vNpcPos.z = pInfo->wPosZ;

	pPlayer->GetPosition( &vPos );
	float dist = CalcDistanceXZ( &vPos, &vNpcPos );
	if( dist > 3000.0f )
	{
		return FALSE;
	}

	return TRUE;
}

DWORD CDungeonMgr::GetNpcObjectIndex(DWORD dwChannelID, WORD dwNpcIndex)
{
	stDungeon* pDungeon = m_DungeonList.GetData(dwChannelID);
	if(! pDungeon)
		return 0;

	stSwitchNpcState* pSwitchNpcState = pDungeon->GetSwitchNpc(dwNpcIndex);
	if( !pSwitchNpcState )
		return 0;

	return pSwitchNpcState->dwObjectIndex;
}

DWORD CDungeonMgr::AddNpc( stSwitchNpcInfo* pSwitchNpc, DWORD dwChannelID )
{
	MAPTYPE MapNum = pSwitchNpc->wMapNum;

	VECTOR3 vOutPos = {0};
	vOutPos.x = pSwitchNpc->wPosX;
	vOutPos.z = pSwitchNpc->wPosZ;


	BASEOBJECT_INFO Baseinfo;
	NPC_TOTALINFO NpcTotalInfo;

	ZeroMemory(&Baseinfo, sizeof(BASEOBJECT_INFO));
	ZeroMemory(&NpcTotalInfo, sizeof(NPC_TOTALINFO));
	
	NPC_LIST* pNpcList = GAMERESRCMNGR->GetNpcInfo(pSwitchNpc->wNpcJob);
	if(pNpcList == 0)
		return 0;

	Baseinfo.dwObjectID =  g_pAISystem.GeneraterMonsterID();
	Baseinfo.BattleID = dwChannelID;
	
	SafeStrCpy(Baseinfo.ObjectName, pSwitchNpc->name, MAX_NAME_LENGTH+1);
	NpcTotalInfo.Group = 0;
	NpcTotalInfo.MapNum = MapNum;
	NpcTotalInfo.NpcJob = pNpcList->JobKind;
	NpcTotalInfo.NpcKind = pNpcList->NpcKind;
	NpcTotalInfo.NpcUniqueIdx = pSwitchNpc->wIndex;

	CNpc* pNpc = g_pServerSystem->AddNpc(&Baseinfo, &NpcTotalInfo, &vOutPos);
	if(pNpc)
	{
		pNpc->SetDieTime(0);	// 자동소멸되지 않도록.
	}
	else
		return 0;

	return pNpc->GetID();
}

const stDungeon& CDungeonMgr::GetDungeon(DWORD channelIndex)
{
	const stDungeon* const dungeon = m_DungeonList.GetData(channelIndex);

	if(0 == dungeon)
	{
		static const stDungeon emptyDungeon;
		return emptyDungeon;
	}

	return *dungeon;
}

void CDungeonMgr::SendInfoSummary(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;

	MSG_DUNGEON_INFO_ALL msg;
	msg.Category = MP_CHEAT;
	msg.Protocol = MP_CHEAT_DUNGEON_GETINFOALL_ACK;
	msg.dwObjectID = pmsg->dwObjectID;
	msg.dwDungeonNum = m_DungeonList.GetDataNum();
	msg.dwUserNum = g_pUserTable->GetUserCount();

	stDungeon* pDungeon;
	m_DungeonList.SetPositionHead();
	int nCnt = 0;
	while((pDungeon = m_DungeonList.GetData())!= NULL)
	{
		msg.DungeonInfo[nCnt].dwChannel = pDungeon->dwChannelID;
		msg.DungeonInfo[nCnt].dwJoinPlayerNum = pDungeon->dwJoinPlayerNum;
		msg.DungeonInfo[nCnt].dwPartyIndex = pDungeon->dwPartyIndex;
		msg.DungeonInfo[nCnt].dwPoint = pDungeon->dwPoint;
		msg.DungeonInfo[nCnt].difficulty = pDungeon->difficulty;

		nCnt++;
	}

	if(pmsg->wData2 != 0)
		PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->wData2, &msg, msg.GetMsgLength());
	else
	{
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
		if(pPlayer)
			pPlayer->SendMsg(&msg, sizeof(msg));
	}
}

void CDungeonMgr::SendInfoDetail(DWORD dwConnectionIndex, char* pMsg)
{
	MSG_WORD3* pmsg = (MSG_WORD3*)pMsg;

	MSG_DUNGEON_INFO_ONE msg;
	msg.Category = MP_CHEAT;
	msg.Protocol = MP_CHEAT_DUNGEON_GETINFOONE_ACK;
	msg.dwObjectID = pmsg->dwObjectID;

	stDungeon* pDungeon = m_DungeonList.GetData(pmsg->wData2);
	if(pDungeon)
	{	
		msg.DungeonInfo.DungeonSummary.dwChannel = pDungeon->dwChannelID;
		msg.DungeonInfo.DungeonSummary.dwJoinPlayerNum = pDungeon->dwJoinPlayerNum;
		msg.DungeonInfo.DungeonSummary.dwPartyIndex = pDungeon->dwPartyIndex;
		msg.DungeonInfo.DungeonSummary.dwPoint = pDungeon->dwPoint;
		msg.DungeonInfo.DungeonSummary.difficulty = pDungeon->difficulty;

		CParty* pParty = PARTYMGR->GetParty(pDungeon->dwPartyIndex);
		if(pParty)
		{
			for(int i=0; i<MAX_PARTY_LISTNUM; i++)
				msg.DungeonInfo.PartyMember[i] = pParty->GetMember(i);

			memcpy(msg.DungeonInfo.warpState, pDungeon->m_WarpState, sizeof(msg.DungeonInfo.warpState));
			memcpy(msg.DungeonInfo.switchState, pDungeon->mSwitch, sizeof(msg.DungeonInfo.switchState));

			//PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->wData3, &msg, sizeof(msg));
			if(pmsg->wData3 != 0)
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->wData3, &msg, sizeof(msg));
			else
			{
				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
				if(pPlayer)
					pPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
	}
}

// 100419 ONS 현재 인던내에 부여된 버프를 저장하여 관리한다.
void CDungeonMgr::StoreSwitchBuff( DWORD dwChannelID, DWORD dwBuffIdx )
{
	if( IsDungeon( g_pServerSystem->GetMapNum() ) == FALSE )
		return;

	stDungeon* pDungeon = m_DungeonList.GetData( dwChannelID );
	if( !pDungeon )
		return;
	
	BuffVector& buff = m_DungeonBuffMap[dwChannelID];

	BuffVector::iterator iter = find(buff.begin(), buff.end(), dwBuffIdx );
	if( iter == buff.end() )
		buff.push_back(dwBuffIdx);
}

// 100419 ONS 현재 인던내에 부여된 버프를 리스트에서 제거한다.
void CDungeonMgr::DelSwitchBuff( DWORD dwChannelID, DWORD dwBuffIdx )
{
	if( IsDungeon( g_pServerSystem->GetMapNum() ) == FALSE )
		return;

	stDungeon* pDungeon = m_DungeonList.GetData( dwChannelID );
	if( !pDungeon )
		return;
	
	BuffVector& buff = m_DungeonBuffMap[dwChannelID];

	BuffVector::iterator iter = find(buff.begin(), buff.end(), dwBuffIdx );
	if( iter != buff.end() )
		buff.erase( iter );
}
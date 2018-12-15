#include "stdafx.h"
#include "MHFile.h"
#include "SHDateManager.h"

#if defined(_AGENTSERVER)
	#include "Network.h"
	#include "AgentDBMsgParser.h"
	#include "AgentNetworkMsgParser.h"
#elif defined(_MAPSERVER_)
	#include "UserTable.h"
	#include "Player.h"
	#include "PackedData.h"
	#include "Network.h"
	#include "../[CC]Header/GameResourceManager.h"
	#include "MapDBMsgParser.h"
	#include "Party.h"
	#include "PartyManager.h"
	#include "MHTimeManager.h"
	#include "RecallManager.h"
#else
	#include "Player.h"
	#include "ChatManager.h"
	#include "GameIn.h"
	#include "ObjectManager.h"
	#include "WindowIDEnum.h"
	#include "cMsgBox.h"
	#include "interface/cWindowManager.h"
	#include "../ResidentRegist/SHResidentRegistManager.h"
	#include "PartyManager.h"
	#include "../DateMatchingDlg.h"
	#include "mhMap.h"
	#include "cStatic.h"
	#include "SHChallengeZoneClearNo1Dlg.h"
	#include "SHChallengeZoneListDlg.h"
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHDateManager
//
CSHDateManager g_csDateManager;


UINT	CSHDateManager::NOTICE_HEART_MATCHING_POING = 0;						// 하트 알리미 매칭 포인트
UINT	CSHDateManager::ENTER_CHALLENGE_ZONE_MATCHING_POINT = 0;				// 챌린지 존 입장 가능 매칭 포인트
UINT	CSHDateManager::ENTER_CHALLENGE_ZONE_FREQ_PER_DAY = 0;					// 챌린지 존 입장 가능 회수(하루)
UINT	CSHDateManager::ENTER_CHALLENGE_ZONE_SECTION_NUM = 0;					// 챌린지 존 구간 개수
DWORD	CSHDateManager::CHALLENGE_ZONE_START_DELAY_TIME = 0;					// 챌린지 존 시작 대기 시간
DWORD	CSHDateManager::CHALLENGE_ZONE_END_DELAY_TIME = 0;						// 챌린지 존 종료 대기 시간

UINT	CSHDateManager::CHALLENGE_ZONE_MOTION_NUM_START = 0;					// 챌린지 존 시작 모션 번호
UINT	CSHDateManager::CHALLENGE_ZONE_MOTION_NUM_SUCCESS = 0;					// 챌린지 존 성공 모션 번호
UINT	CSHDateManager::CHALLENGE_ZONE_MOTION_NUM_SUCCESS_LEAST_CLEAR_TIME = 0;	// 챌린지 존 최단 시간 클리어 성공 모션 번호
UINT	CSHDateManager::CHALLENGE_ZONE_MOTION_NUM_FAIL = 0;						// 챌린지 존 실패 모션 번호
// -------------------------------------------------------------------------------------------------------------------------------------
// CSHFamilyManager Method																										  생성자
//
CSHDateManager::CSHDateManager()
{
	m_bIsChallengeZone = FALSE;

	m_pnDateZoneMoveIndexList = NULL;
	m_nDateZoneMoveIndexNum = 0;

	m_pstChallengeZoneMoveIndexList = NULL;
	m_nChallengeZoneMoveIndexNum = 0;

	// 일반 기본 채널과의 식별을 위해 기본값으로 1000 을 설정
	m_nChallengeZoneEnterNum = 1000;

	m_pstChallengeZoneMonsterGroupSection = NULL;

	m_pstChallengeZoneSectionMonLevel = NULL;
	m_nChallengeZoneSectionNum = 0;

	m_nChallengeZoneState = 0;
	m_nChallengeZoneTime = 0;
	m_nChallengeZoneTimeTick = 0;

	m_bChallengeZoneStart = FALSE;
	m_nChallengeZoneFirstEnterPlayerID = 0;

	m_nChallengeZoneLeastClearTime = 0;
	LoadDateInfo();
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHFamilyManager Method																										  파괴자
//
CSHDateManager::~CSHDateManager()
{
	SAFE_DELETE_ARRAY(m_pnDateZoneMoveIndexList);
	SAFE_DELETE_ARRAY(m_pstChallengeZoneMoveIndexList);
	SAFE_DELETE_ARRAY(m_pstChallengeZoneSectionMonLevel);

	if (m_pstChallengeZoneMonsterGroupSection)
	{
		for(UINT i=0; i<ENTER_CHALLENGE_ZONE_SECTION_NUM; i++)
		{
			for(UINT j=0; j<m_pstChallengeZoneMonsterGroupSection[i].nGroupNum; j++)
			{
				SAFE_DELETE_ARRAY(m_pstChallengeZoneMonsterGroupSection[i].pstGroup[j].pstMonster);
			}
			SAFE_DELETE_ARRAY(m_pstChallengeZoneMonsterGroupSection[i].pstGroup);
		}
		SAFE_DELETE_ARRAY(m_pstChallengeZoneMonsterGroupSection);
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  Init Method																													  초기화
//
VOID CSHDateManager::Init()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (IsChallengeZone(MAP->GetMapNum()))
	{
		m_bIsChallengeZone = TRUE;
		return;
	}

	m_bIsChallengeZone = FALSE;
	m_nChallengeZoneState = 0;
	m_nChallengeZoneTime = 0;
	m_nChallengeZoneTimeTick = 0;
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  LoadDateInfo Method																										 데이터 읽기
//
BOOL CSHDateManager::LoadDateInfo()
{
	char szLine[MAX_PATH], szFile[MAX_PATH];
	int	 nKind = 0, nCnt = 0;
	CMHFile fp;

	sprintf(szFile, "./System/Resource/DateMatching.bin");
	fp.Init(szFile, "rb");
	if(!fp.IsInited()) return FALSE;

	while(FALSE == fp.IsEOF())
	{
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;			
		}
		else if (strstr(szLine, "END_KIND")) 
		{
			nCnt = 0;
			nKind++;
			continue;
		}

		switch(nKind)
		{
		case 1: m_nDateZoneMoveIndexNum++;			break;
		case 2: m_nChallengeZoneMoveIndexNum++;		break;
		case 4: ENTER_CHALLENGE_ZONE_SECTION_NUM++;	break;
		}
		
		nCnt++;
	}
	fp.Release();

	// 데이트 존
	SAFE_DELETE_ARRAY(m_pnDateZoneMoveIndexList);
	m_pnDateZoneMoveIndexList = new UINT[m_nDateZoneMoveIndexNum];
	ZeroMemory(m_pnDateZoneMoveIndexList, sizeof(*m_pnDateZoneMoveIndexList)*m_nDateZoneMoveIndexNum);
	// 챌린지 존
	SAFE_DELETE_ARRAY(m_pstChallengeZoneMoveIndexList);
	m_pstChallengeZoneMoveIndexList = new stCHALLENGEZONE_MAPINFO[m_nChallengeZoneMoveIndexNum];
	ZeroMemory(m_pstChallengeZoneMoveIndexList, sizeof(*m_pstChallengeZoneMoveIndexList)*m_nChallengeZoneMoveIndexNum);
	// 챌린지 존 구간
	SAFE_DELETE_ARRAY(m_pstChallengeZoneSectionMonLevel);
	m_pstChallengeZoneSectionMonLevel = new stCHALLENGEZONE_SECTION_MONLEVEL[ENTER_CHALLENGE_ZONE_SECTION_NUM];
	ZeroMemory(m_pstChallengeZoneSectionMonLevel, sizeof(*m_pstChallengeZoneSectionMonLevel)*ENTER_CHALLENGE_ZONE_SECTION_NUM);
	
	// 데이터 읽기
	nKind = 0;
	nCnt = 0;
	fp.Init(szFile, "rb");
	if(!fp.IsInited()) return FALSE;

	while(FALSE == fp.IsEOF())
	{
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;			
		}
		else if (strstr(szLine, "END_KIND")) 
		{
			nCnt = 0;
			nKind++;
			continue;
		}

		switch(nKind)
		{
		case 0:		sscanf(szLine, "%d %d",			&NOTICE_HEART_MATCHING_POING,
													&ENTER_CHALLENGE_ZONE_MATCHING_POINT);				break;

		case 1:		sscanf(szLine, "%d",			&m_pnDateZoneMoveIndexList[nCnt]);					break;
		case 2:		sscanf(szLine, "%d %d",			&m_pstChallengeZoneMoveIndexList[nCnt].nMapNum,
													&m_pstChallengeZoneMoveIndexList[nCnt].nMoveIndex);	break;

		case 3:		sscanf(szLine, "%d %d %d",		&ENTER_CHALLENGE_ZONE_FREQ_PER_DAY,
													&CHALLENGE_ZONE_START_DELAY_TIME,
													&CHALLENGE_ZONE_END_DELAY_TIME);					break;

		case 4:		sscanf(szLine, "%d %d %d",		&m_pstChallengeZoneSectionMonLevel[nCnt].nStart,
													&m_pstChallengeZoneSectionMonLevel[nCnt].nEnd,
													&m_pstChallengeZoneSectionMonLevel[nCnt].nLimitTime);	
			
					m_pstChallengeZoneSectionMonLevel[nCnt].nLimitTime += CHALLENGE_ZONE_START_DELAY_TIME;
																										break;
		case 5:		sscanf(szLine, "%d %d %d %d",	&CHALLENGE_ZONE_MOTION_NUM_START,
													&CHALLENGE_ZONE_MOTION_NUM_SUCCESS,
													&CHALLENGE_ZONE_MOTION_NUM_SUCCESS_LEAST_CLEAR_TIME,
													&CHALLENGE_ZONE_MOTION_NUM_FAIL);					break;
		}
		
		nCnt++;
	}
	fp.Release();

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  LoadChallengeMonsterInfo Method																			챌린지 존 몬스터 데이터 읽기
//
BOOL CSHDateManager::LoadChallengeMonsterInfo(int nMapNum)
{
#if defined(_MAPSERVER_)
	if (IsChallengeZone(nMapNum) == FALSE) return TRUE;
	m_bIsChallengeZone = TRUE;

	if (m_pstChallengeZoneMonsterGroupSection)
	{
		for(UINT i=0; i<ENTER_CHALLENGE_ZONE_SECTION_NUM; i++)
		{
			for(UINT j=0; j<m_pstChallengeZoneMonsterGroupSection[i].nGroupNum; j++)
			{
				SAFE_DELETE_ARRAY(m_pstChallengeZoneMonsterGroupSection[i].pstGroup[j].pstMonster);
			}
			SAFE_DELETE_ARRAY(m_pstChallengeZoneMonsterGroupSection[i].pstGroup);
		}
		SAFE_DELETE_ARRAY(m_pstChallengeZoneMonsterGroupSection);
	}

	// 섹션 개수만큼 생성
	m_pstChallengeZoneMonsterGroupSection = new stCHALLENGEZONE_MONSTER_GROUP_SECTION[ENTER_CHALLENGE_ZONE_SECTION_NUM];
	ZeroMemory(m_pstChallengeZoneMonsterGroupSection, sizeof(*m_pstChallengeZoneMonsterGroupSection)*ENTER_CHALLENGE_ZONE_SECTION_NUM);
	for(UINT nSection=0; nSection<ENTER_CHALLENGE_ZONE_SECTION_NUM; nSection++)
	{
		char szLine[MAX_PATH], szFile[MAX_PATH];
		int	 nKind = 0, nCnt = 0;
		CMHFile fp;

		sprintf(szFile, "./System/Resource/ChallengeZoneMonster%d%02d.bin", nMapNum, nSection+1);
		fp.Init(szFile, "rb");
		if(!fp.IsInited())
		{
			char szTmp[256];
			sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
			ASSERTMSG(0, szTmp);
			return FALSE;
		}

		// 그룹 개수 읽기
		while(1)
		{
			if(fp.IsEOF()) break;
			fp.GetLine(szLine, sizeof(szLine));
			if (strstr(szLine, "//") ||
				IsEmptyLine(szLine))
			{
				continue;			
			}
			else if (strstr(szLine, "END_KIND")) 
			{
				nCnt = 0;
				nKind++;
				m_pstChallengeZoneMonsterGroupSection[nSection].nGroupNum++;
				continue;
			}

			nCnt++;
		}
		fp.Release();

		m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup = new stCHALLENGEZONE_MONSTER_GROUP[m_pstChallengeZoneMonsterGroupSection[nSection].nGroupNum];
		ZeroMemory(m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup, sizeof(*m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup)*m_pstChallengeZoneMonsterGroupSection[nSection].nGroupNum);

		// 몬스터 개수 읽기
		nKind = 0;
		nCnt = 0;
		fp.Init(szFile, "rb");
		if(!fp.IsInited()) return FALSE;

		while(1)
		{
			if(fp.IsEOF()) break;
			fp.GetLine(szLine, sizeof(szLine));
			if (strstr(szLine, "//") ||
				IsEmptyLine(szLine))
			{
				continue;			
			}
			else if (strstr(szLine, "END_KIND")) 
			{
				m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nKind].nMonsterNum = nCnt;
				nCnt = 0;
				nKind++;
				continue;
			}

			nCnt++;
		}
		fp.Release();

		for(UINT i=0; i<m_pstChallengeZoneMonsterGroupSection[nSection].nGroupNum; i++)
		{
			m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[i].pstMonster = new stCHALLENGEZONE_MONSTER[m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[i].nMonsterNum];
			ZeroMemory(m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[i].pstMonster, sizeof(*m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[i].pstMonster)*m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[i].nMonsterNum);
		}

		//  최종 데이터 설정
		nKind = 0;
		nCnt = 0;
		fp.Init(szFile, "rb");
		if(!fp.IsInited()) return FALSE;

		while(1)
		{
			if(fp.IsEOF()) break;
			fp.GetLine(szLine, sizeof(szLine));
			if (strstr(szLine, "//") ||
				IsEmptyLine(szLine))
			{
				continue;			
			}
			else if (strstr(szLine, "END_KIND")) 
			{
				nCnt = 0;
				nKind++;
				continue;
			}

			sscanf(szLine, "%d %d %f %f %d",	&m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nKind].pstMonster[nCnt].nMonsterKind,
												&m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nKind].pstMonster[nCnt].nMonsterNum,
												&m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nKind].pstMonster[nCnt].nPosX,
												&m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nKind].pstMonster[nCnt].nPosZ,
												&m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nKind].pstMonster[nCnt].nDelayTime
												);
			// 총 몬스터 수 설정
			m_pstChallengeZoneMonsterGroupSection[nSection].nTotalMonsterNum += m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nKind].pstMonster[nCnt].nMonsterNum;
			// 처음 등장하는 몬스터의 딜레이 타임을 챌린지 시작 딜레이 타임으로 설정
			if (nKind == 0)
			{
				m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nKind].pstMonster[nCnt].nDelayTime += CHALLENGE_ZONE_START_DELAY_TIME;
			}
			// 딜레이 타임은 누적해서 계산한다.
			if (nKind > 0)
			{
				m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nKind].pstMonster[nCnt].nDelayTime += 
				m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nKind-1].pstMonster[0].nDelayTime;
			}
			nCnt++;
		}
		fp.Release();
	}
#endif

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  GetDateZoneMoveIndex Method																				  데이트 존 이동 인덱스 얻기
//
BOOL CSHDateManager::GetDateZoneMoveIndex(UINT nSelIndex, int* pnIndex)
{
	if (nSelIndex >= m_nDateZoneMoveIndexNum)
	{
		return FALSE;
	}

	*pnIndex = m_pnDateZoneMoveIndexList[nSelIndex];

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  GetChallengeZoneMoveIndex Method																		  챌린지 존 이동 인덱스 얻기
//
BOOL CSHDateManager::GetChallengeZoneMoveIndex(UINT nSelIndex, int* pnIndex)
{
	if (nSelIndex >= m_nChallengeZoneMoveIndexNum)
	{
		return FALSE;
	}

	*pnIndex = m_pstChallengeZoneMoveIndexList[nSelIndex].nMoveIndex;

	return TRUE;
}

MAPTYPE CSHDateManager::GetMapType(UINT index)
{
	if(index >= GetChallengeZoneMoveIndexNum())
	{
		return MAX_MAP_NUM;
	}

	return MAPTYPE(m_pstChallengeZoneMoveIndexList[index].nMapNum);
}

BOOL CSHDateManager::IsChallengeZone(int nMapNum)
{
	for(UINT i=0; i<m_nChallengeZoneMoveIndexNum; i++)
	{
		if (m_pstChallengeZoneMoveIndexList[i].nMapNum == (UINT)nMapNum)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  Process Method																									 
//
VOID CSHDateManager::MainLoop()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
  	switch(m_nChallengeZoneState)
	{
	case CHALLENGEZONE_START:
		if (gCurTime - m_nChallengeZoneTimeTick > 1000)
		{
			// 처음 한 번
			if (m_nChallengeZoneTime == 0)
			{
				// 이펙트 연출
				if (CHALLENGE_ZONE_MOTION_NUM_START)
					EFFECTMGR->StartEffectProcess(CHALLENGE_ZONE_MOTION_NUM_START, HERO, NULL, 0, 0);
			}

			m_nChallengeZoneTime += (gCurTime - m_nChallengeZoneTimeTick)/1000;
			if (m_nChallengeZoneTime >= CHALLENGE_ZONE_START_DELAY_TIME/1000)
			{
				m_nChallengeZoneState = NULL;
				m_nChallengeZoneTime = 0;
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1218));
				GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneStartTimeTick(gCurTime);
			}
			else
			{
				m_nChallengeZoneTimeTick = gCurTime;
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1219), CHALLENGE_ZONE_START_DELAY_TIME/1000 - m_nChallengeZoneTime);
			}
		}
		break;
	case CHALLENGEZONE_END:
		if (gCurTime - m_nChallengeZoneTimeTick > 1000)
		{
			m_nChallengeZoneTime += (gCurTime - m_nChallengeZoneTimeTick)/1000;
			if (m_nChallengeZoneTime >= CHALLENGE_ZONE_END_DELAY_TIME/1000)
			{
				m_nChallengeZoneState = NULL;
				m_nChallengeZoneTime = 0;
			}
			else
			{
				m_nChallengeZoneTimeTick = gCurTime;
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1220), CHALLENGE_ZONE_END_DELAY_TIME/1000 - m_nChallengeZoneTime);
			}
		}
		break;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_Process Method																							 챌린지 존 메인 프로세스					 
//
VOID CSHDateManager::SRV_Process(CPlayer* pPlayer)
{
#if defined(_MAPSERVER_)
	SRV_ProcChallengeZoneEnterFreq(pPlayer);
	SRV_ProcRegenMonster(pPlayer);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_StartChallengeZone Method																						  챌린지 존 시작					 
//
VOID CSHDateManager::SRV_StartChallengeZone(CPlayer* pPlayer, UINT nChannelNumFromAgent, UINT nSection)
{
#if defined(_MAPSERVER_)
	// 챌린지 존에 입장이라면 회수 증가
	// ..정상적인 챌린지 존으로의 이동인 경우 식별을 위해 에이전트에서 1000 이상으로 보냄
	if (nChannelNumFromAgent >= 1000)
	{
		pPlayer->IncreaseChallengeZoneEnterFreq();
		g_csDateManager.SRV_SendChallengeZoneEnterFreq(pPlayer);
		// 원래 여기서 DB에 저장해야 하지만 이 루틴 실행 후에 DB Load 루틴이 실행된다.
		// 그래서 Load 부분에서 입장 회수가 증가됐는지를 체크해서 증가되었다면 그 값을 DB에 저장한다.
	}

	if (m_bChallengeZoneStart == FALSE) return;

	// 먼저 들어와 있는 파트너 체크
	// ..파트너가 없거나 채널이 다르다면 뭔가 이상..
	CPlayer* pFirstEnterPlayer = (CPlayer*)g_pUserTable->FindUser(m_nChallengeZoneFirstEnterPlayerID);
	if(pFirstEnterPlayer && pPlayer->GetChannelID() == pFirstEnterPlayer->GetChannelID())
	{
		// 파트너 ID 설정
		pPlayer->SetChallengeZonePartnerID(pFirstEnterPlayer->GetID());
		pFirstEnterPlayer->SetChallengeZonePartnerID(pPlayer->GetID());
		// 몬스터 수 설정
		pPlayer->SetChallengeZoneMonsterNum(m_pstChallengeZoneMonsterGroupSection[nSection].nTotalMonsterNum);
		pPlayer->SetChallengeZoneMonsterNumTillNow(0);
		pPlayer->SetChallengeZoneKillMonsterNum(0);
		pPlayer->SetChallengeZoneCreateMonRightNow(FALSE);
		pFirstEnterPlayer->SetChallengeZoneMonsterNum(m_pstChallengeZoneMonsterGroupSection[nSection].nTotalMonsterNum);
		pFirstEnterPlayer->SetChallengeZoneMonsterNumTillNow(0);
		pFirstEnterPlayer->SetChallengeZoneKillMonsterNum(0);
		pFirstEnterPlayer->SetChallengeZoneCreateMonRightNow(FALSE);
		// ..구간 범위 체크. 만약 구간이 범위를 벗어난다면 그냥 맵 구경..ㅋ
		if (nSection < ENTER_CHALLENGE_ZONE_SECTION_NUM)
		{
			// 챌린지 시작 상태 설정
			// 시작 및 몬스터 설정은 1명이 담당한다.
			pPlayer->SetChallengeZoneStartState(1);
			// 구간 설정
			pPlayer->SetChallengeZoneSection(nSection);
			pFirstEnterPlayer->SetChallengeZoneSection(nSection);
			// 틱 카운트 설정
			pPlayer->SetChallengeZoneStartTimeTick(gCurTime);
			pFirstEnterPlayer->SetChallengeZoneStartTimeTick(gCurTime);

			// 091124 ONS 경험치 비율을 로드한다.
			ChallengeZone_ExpRate_Load(pFirstEnterPlayer->GetID(), pPlayer->GetID(), nSection);
			ChallengeZone_ExpRate_Load(pPlayer->GetID(), pFirstEnterPlayer->GetID(), nSection);
		}
	}
	else
	{
		if (g_pServerSystem->IsTestServer())
		{
			g_Console.LOG(4, "CHALLENGEZONE_ENTER_ERROR:%d", pPlayer->GetID());
		}
	}

	m_bChallengeZoneStart = FALSE;
	m_nChallengeZoneFirstEnterPlayerID = 0;
#endif
}
// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_NotifyStartChallengeZone Method																				 챌린지 존 시작 알림					 
//
// 091124 ONS 챌린지 존 시작 알림 - 이름, 경험치비율 정보를 각 클라이언트로 전송한다.
VOID CSHDateManager::SRV_NotifyStartChallengeZone(CPlayer* pPlayer, char* pszName1, char* pszName2, DWORD dwExpRate1, DWORD dwExpRate2, UINT nSection)
{
#if defined(_MAPSERVER_)
	MSG_NAME2_DWORD3 stPacket;

	stPacket.Category	= MP_DATE;
	stPacket.Protocol	= MP_DATE_CHALLENGEZONE_START;	
	
	SafeStrCpy(stPacket.Name1, pszName1, MAX_NAME_LENGTH+1);
	SafeStrCpy(stPacket.Name2, pszName2, MAX_NAME_LENGTH+1);
	stPacket.dwData1 = dwExpRate1;
	stPacket.dwData2 = dwExpRate2;
	stPacket.dwData3 = nSection;

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));

#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ProcChallengeEnterFreq Method																			챌린지 존 입장 회수 처리					 
//
VOID CSHDateManager::SRV_ProcChallengeZoneEnterFreq(CPlayer* pPlayer)
{
#if defined(_MAPSERVER_)
	// 날(day)이 변경되었으면(자정이 지나면 입장 회수 초기화)
	if (MHTIMEMGR_OBJ->GetOldLocalTime()->wDay != MHTIMEMGR_OBJ->GetCurLocalTime()->wDay)
	{
		if (pPlayer->GetChallengeZoneEnterFreq())
		{
			pPlayer->SetChallengeZoneEnterFreq(0);
			SRV_SendChallengeZoneEnterFreq(pPlayer);
			// 여기서 DB저장이 필요하지만, 저장하면 한 순간에 접속자 수 만큼의 DB쿼리가 발생한다. 무슨 일이 발생할지는....
			// 저장하는 게 확실하긴 하지만 저장은 안 해도 된다. 서버상에서의 값만 0 으로 해 주면 된다.
			// 챌린지 존으로 이동하거나, 로그아웃, 맵 이동을 할 때 DB Load 를 하면서 날짜 체크를 해서 
			// 날짜가 지났다면 DB에 저장되어 있는 값에 상관없이 0 으로 리턴하기 때문.
			// 그리고, 챌린지 존으로 이동할 때 0 으로 리턴된 값에 챌린지 존으로 들어가면서 +1 되어
			// DB에 저장을 하므로 DB 저장은 여기서 신경 안 써도 됨. (즉, 챌린지 존으로 이동하는 순간에만 DB 저장을 함)
			// ..날짜 체크를 이렇게 하지 않고, 패밀리 탈퇴처럼 챌랜지 존 이동을 시도할 때, 그 때 날짜를
			// ..체크해서 처리해도 되는데, 그럴 경우 클라이언트에 갱신된 정보를 보내주는 게 불가능해지고, 또한
			// ..서버에서 챌린지 존에 들어간 마지막 날짜를 갖고 있어야 하는데, 서버에서 갖고 있지 않고 처리할 수 있는
			// ..방법(DB 프로시저에서 처리할 수 있는 방법)을 우선시해서 이렇게 처리했음.
			//
			// 그런데 문제가 있을 수 있다. 게임서버의 시스템타임이 DB서버의 시스템타임보다 빠를 경우, 게임서버에서는
			// 회수가 0으로 초기화 된 후, 맵 이동을 하게 되었는 데, 그 때 DB서버의 시스템타임이 자정이 지나지 않았다면,
			// DB에 저장되어 있는 값을 읽어버려, DB서버의 시스템타임이 자정이 넘은 후 맵이동을 하기 전까지는 
			// 챌린지 존에 입장할 수 없게 된다. 게임서버의 시스템타임과 DB서버의 시스템타임의 오차가 관건~
			// 이 문제를 방지하기 위해 세이브 플래그를 하나 둬서 플래그가 세팅되어 있으면 맵 이탈시에 DB저장을 하도록 한다.
			pPlayer->SetChallengeZoneNeedSaveEnterFreq(TRUE);
			// 그런데 만약 맵 이탈할 때의 저장 DB쿼리보다 접속 맵의 로드 DB쿼리가 먼저 수행된다면..???  방법 없음..
			// DB서버의 시스템타임을 사용하지 않고 게임서버의 시스템타임을 사용한다고 해도, 모든 맵서버가 한 하드웨어에
			// 탑재되어 있지 않은 이상 시간오차를 피할 수는 없다. 문제 확률은 좀 줄어들겠지만..
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SendChallengeZoneEnterFreq Method																		챌린지 존 입장 회수 전송					 
//
VOID CSHDateManager::SRV_SendChallengeZoneEnterFreq(CPlayer* pPlayer)
{
#if defined(_MAPSERVER_)
	MSG_DWORD2 msg;

	msg.Category	= MP_DATE;
	msg.Protocol	= MP_DATE_CHALLENGEZONE_ENTER_FREQ;
	msg.dwData1		= pPlayer->GetChallengeZoneEnterFreq();
	msg.dwData2		= pPlayer->GetChallengeZoneEnterBonusFreq();
	pPlayer->SendMsg(&msg, sizeof(msg));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ProcRegenMonster Method																				  챌린지 존 몬스터 등장 처리					 
//
VOID CSHDateManager::SRV_ProcRegenMonster(CPlayer* pPlayer)
{
#if defined(_MAPSERVER_)
	// 챌린지 상태가 아니면..
	if (pPlayer->GetChallengeZoneStartState() == 0)
	{
		return;
	}

	// 챌린지가 끝났으면 대기 시간 후 귀환하기 전에 잠깐 딜레이.. 클라이언트에서 마지막 몬스터를 잡는 연출이 끝나는 시간을 위해..
	// 여기에서의 딜레이가 끝나면 종료 카운트 다운 들어감.
	if (pPlayer->GetChallengeZoneStartState() == CHALLENGEZONE_END_SUCCESS)
	{
		if (pPlayer->GetChallengeZoneStartTimeTick())
		{
			const DWORD END_DELAY = 5000;
			if (gCurTime - pPlayer->GetChallengeZoneStartTimeTick() > END_DELAY)
			{
				pPlayer->SetChallengeZoneStartState(CHALLENGEZONE_END_START_COUNTDOWN);
				pPlayer->SetChallengeZoneStartTimeTick(gCurTime);

				MSG_DWORD stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= MP_DATE_CHALLENGEZONE_END_SUCCESS;
				stPacket.dwObjectID			= pPlayer->GetID();
				stPacket.dwData				= 0;
				// 최단 시간 클리어 타임을 갱신했다면 클라이언트 연출을 위해 값 설정
				if (pPlayer->GetChallengeZoneClearTime() &&
					pPlayer->GetChallengeZoneClearTime() < m_pstChallengeZoneSectionMonLevel[pPlayer->GetChallengeZoneSection()].nLeastClearTime)
				{
					stPacket.dwData			= 1;
				}

				pPlayer->SendMsg(&stPacket, sizeof(stPacket));	
			}
		}
		return;
	}

	// 챌린지가 끝났으면 대기 시간 후 귀환
	if (pPlayer->GetChallengeZoneStartState() > CHALLENGEZONE_END)
	{
		if (pPlayer->GetChallengeZoneStartTimeTick())
		{
			if (gCurTime - pPlayer->GetChallengeZoneStartTimeTick() > CHALLENGE_ZONE_END_DELAY_TIME)
			{
				pPlayer->SetChallengeZoneStartTimeTick(0);

				MSG_DWORD stPacket;

				stPacket.Category 		= MP_DATE;
				stPacket.Protocol 		= MP_DATE_CHALLENGEZONE_RETURN;
				stPacket.dwObjectID		= pPlayer->GetID();;
				pPlayer->SendMsg(&stPacket, sizeof(stPacket) );
			}
		}
		return;
	}

	UINT nSection	= pPlayer->GetChallengeZoneSection();
	UINT i			= pPlayer->GetChallengeZoneStartState()-1;
	for(; i<m_pstChallengeZoneMonsterGroupSection[nSection].nGroupNum; i++)
	{
		// 등장 시간이 됐나? 또는 지금까지 나온 몬스터를 모두 해치웠으면 바로 다음 몬스터 등장
		if (gCurTime - pPlayer->GetChallengeZoneStartTimeTick() > m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[i].pstMonster[0].nDelayTime ||
			pPlayer->IsChallengeZoneCreateMonRightNow())
		{
			// 몬스터 소환
			for(UINT j=0; j<m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[i].nMonsterNum; j++)
			{
				VECTOR3 stPos;
				stPos.x = GetChallengeZoneMonsterGroup(nSection, i)->pstMonster[j].nPosX;
				stPos.z = GetChallengeZoneMonsterGroup(nSection, i)->pstMonster[j].nPosZ;
				RECALLMGR->ChallengeZoneRecall(g_csDateManager.GetChallengeZoneMonsterGroup(nSection, i)->pstMonster[j].nMonsterKind, 
											   (BYTE)g_csDateManager.GetChallengeZoneMonsterGroup(nSection, i)->pstMonster[j].nMonsterNum, 
											   pPlayer->GetChannelID(), 
											   &stPos,
											   0);
				/*
				if (g_pServerSystem->IsTestServer())
				{
					g_Console.LOG(4, "CHALLENGEZONE Monster: Channel:%d Group:%d Mon:%d,%d)", 
								  pPlayer->GetChannelID(),
								  i, 
								  g_csDateManager.GetChallengeZoneMonsterGroup(nSection, i)->pstMonster[j].nMonsterKind, 
								  g_csDateManager.GetChallengeZoneMonsterGroup(nSection, i)->pstMonster[j].nMonsterNum);
				}
				*/

				// 지금까지 나온 몬스터 수 설정
				UINT nNum = pPlayer->GetChallengeZoneMonsterNumTillNow() + g_csDateManager.GetChallengeZoneMonsterGroup(nSection, i)->pstMonster[j].nMonsterNum;
				pPlayer->SetChallengeZoneMonsterNumTillNow(nNum);
			}
			pPlayer->SetChallengeZoneStartState(pPlayer->GetChallengeZoneStartState()+1);
			pPlayer->SetChallengeZoneCreateMonRightNow(FALSE);
			break;
		}
	}

	// 타임아웃
	if (gCurTime - pPlayer->GetChallengeZoneStartTimeTick() > m_pstChallengeZoneSectionMonLevel[pPlayer->GetChallengeZoneSection()].nLimitTime)
	{
		SRV_EndChallengeZone(pPlayer, CHALLENGEZONE_END_TIMEOUT);
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ProcMonsterDie Method																				  챌린지 존 몬스터 죽음 처리					 
//
VOID CSHDateManager::SRV_ProcMonsterDie(CPlayer* pPlayer)
{
#if defined(_MAPSERVER_)
	pPlayer->IncreaseChallengeZoneKillMonsterNum();
	CPlayer* pPartner = (CPlayer*)g_pUserTable->FindUser(pPlayer->GetChallengeZonePartnerID());
	if (pPartner)
	{
		UINT nKillMonNum = pPlayer->GetChallengeZoneKillMonsterNum() + pPartner->GetChallengeZoneKillMonsterNum();
		// 몬스터를 모두 해치웠으면 성공!!
		if (nKillMonNum >= pPlayer->GetChallengeZoneMonsterNum())
		{
			SRV_EndChallengeZone(pPlayer,  CHALLENGEZONE_END_SUCCESS);
		}
		else
		{
			// 지금까지 나온 몬스터를 모두 해치웠으면 바로 다음 몬스터 등장 설정
			// ..몬스터 등장을 담당하는 플레이어를 체크해서 처리
			if (pPlayer->GetChallengeZoneStartState())
			{
				if (nKillMonNum  >= pPlayer->GetChallengeZoneMonsterNumTillNow())
				{
					pPlayer->SetChallengeZoneCreateMonRightNow(TRUE);
				}
			}
			else
			{
				if (nKillMonNum  >= pPartner->GetChallengeZoneMonsterNumTillNow())
				{
					pPartner->SetChallengeZoneCreateMonRightNow(TRUE);
				}
			}
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SetChallengeZoneEnterBonusFreq Method															 챌린지 존 입장 보너스 회수 증가				 
//
VOID CSHDateManager::SRV_SetChallengeZoneEnterBonusFreq(CPlayer* pPlayer, UINT nFreq)
{
#if defined(_MAPSERVER_)
	pPlayer->SetChallengeZoneEnterBonusFreq(pPlayer->GetChallengeZoneEnterBonusFreq()+nFreq);
	// DB에 저장
	ChallengeZone_EnterFreq_Save(pPlayer->GetID(), pPlayer->GetChallengeZoneEnterFreq(), pPlayer->GetChallengeZoneEnterBonusFreq());

	SRV_SendChallengeZoneEnterFreq(pPlayer);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_EndChallengeZone Method																						    챌린지 존을 끝냄					 
//
VOID CSHDateManager::SRV_EndChallengeZone(CPlayer* pPlayer, CHALLENGEZONE_STATE eEndState)
{
#if defined(_MAPSERVER_)
	if (IsChallengeZoneHere() == FALSE) return;
	
	switch(eEndState)
	{
	// 타임아웃
	case CHALLENGEZONE_END_TIMEOUT:
		if (pPlayer->GetChallengeZoneStartState() < CHALLENGEZONE_END &&
			pPlayer->GetChallengeZonePartnerID())
		{
			CPlayer* pPartner = (CPlayer*)g_pUserTable->FindUser(pPlayer->GetChallengeZonePartnerID());
			if (pPartner)
			{
				// 타임아웃이면 챌린지 존 끝.
				pPlayer->SetChallengeZoneStartState(eEndState);
				pPlayer->SetChallengeZoneStartTimeTick(gCurTime);

				pPartner->SetChallengeZoneStartState(eEndState);
				pPartner->SetChallengeZoneStartTimeTick(gCurTime);

				MSG_DWORD stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= MP_DATE_CHALLENGEZONE_END_TIMEOUT;
				stPacket.dwObjectID			= pPlayer->GetID();
				stPacket.dwData				= CHALLENGE_ZONE_END_DELAY_TIME;
				pPlayer->SendMsg(&stPacket, sizeof(stPacket));	

				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= MP_DATE_CHALLENGEZONE_END_TIMEOUT;
				stPacket.dwObjectID			= pPartner->GetID();
				stPacket.dwData				= CHALLENGE_ZONE_END_DELAY_TIME;
				pPartner->SendMsg(&stPacket, sizeof(stPacket));	
			}
			else
			{
				// 타임아웃인데, 파트너가 없다? 
				// 파트너가 나가면서 동시에 타임아웃일 경우 파트너가 나가면서 끝을 낸다.
				// 혼자 들어와있는 데 타임아웃? 비정상인적인 상황. 어쨌거나 쫓아내자
				pPlayer->SetChallengeZoneStartState(eEndState);
				pPlayer->SetChallengeZoneStartTimeTick(gCurTime);

				MSG_DWORD stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= MP_DATE_CHALLENGEZONE_END_TIMEOUT;
				stPacket.dwObjectID			= pPlayer->GetID();
				stPacket.dwData				= CHALLENGE_ZONE_END_DELAY_TIME;
				pPlayer->SendMsg(&stPacket, sizeof(stPacket));	
			}
		}
		break;
	// pPlayer 가 챌린지 존을 이탈하면 pPartner 에게 실패를 알린다.
	case CHALLENGEZONE_END_PARTNER_OUT:
		if (pPlayer->GetChallengeZoneStartState() < CHALLENGEZONE_END &&
			pPlayer->GetChallengeZonePartnerID())
		{
			CPlayer* pPartner = (CPlayer*)g_pUserTable->FindUser(pPlayer->GetChallengeZonePartnerID());
			if (pPartner)
			{
				pPartner->SetChallengeZoneStartState(eEndState);
				pPartner->SetChallengeZoneStartTimeTick(gCurTime);

				MSG_DWORD stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= MP_DATE_CHALLENGEZONE_END_PARTNER_OUT;
				stPacket.dwObjectID			= pPlayer->GetID();
				stPacket.dwData				= CHALLENGE_ZONE_END_DELAY_TIME;
				pPartner->SendMsg(&stPacket, sizeof(stPacket));	
			}
		}
		break;
	// pPlayer 가 죽었는 데, pPartner 도 죽어있다면 모두에게 실패를알린다.
	case CHALLENGEZONE_END_ALL_DIE:
		if (pPlayer->GetChallengeZoneStartState() < CHALLENGEZONE_END &&
			pPlayer->GetChallengeZonePartnerID())
		{
			CPlayer* pPartner = (CPlayer*)g_pUserTable->FindUser(pPlayer->GetChallengeZonePartnerID());
			if (pPartner)
			{
				// 둘 다 죽었으면 챌린지 존 끝.
				if (pPartner->GetState() == eObjectState_Die)
				{
					pPlayer->SetChallengeZoneStartState(eEndState);
					pPlayer->SetChallengeZoneStartTimeTick(gCurTime);

					pPartner->SetChallengeZoneStartState(eEndState);
					pPartner->SetChallengeZoneStartTimeTick(gCurTime);

					MSG_DWORD stPacket;
					stPacket.Category			= MP_DATE;
					stPacket.Protocol			= MP_DATE_CHALLENGEZONE_END_ALL_DIE;
					stPacket.dwObjectID			= pPlayer->GetID();
					stPacket.dwData				= CHALLENGE_ZONE_END_DELAY_TIME;
					pPlayer->SendMsg(&stPacket, sizeof(stPacket));	

					stPacket.Category			= MP_DATE;
					stPacket.Protocol			= MP_DATE_CHALLENGEZONE_END_ALL_DIE;
					stPacket.dwObjectID			= pPartner->GetID();
					stPacket.dwData				= CHALLENGE_ZONE_END_DELAY_TIME;
					pPartner->SendMsg(&stPacket, sizeof(stPacket));	
				}
			}
			else
			{
				// 죽었는 데, 파트너가 없다? 
				// 파트너가 나가면서 동시에 죽었을 경우 파트너가 나가면서 끝을 낸다.
				// 혼자 들어와있는 데 죽었다? 비정상인적인 상황. 어쨌거나 쫓아내자
				pPlayer->SetChallengeZoneStartState(eEndState);
				pPlayer->SetChallengeZoneStartTimeTick(gCurTime);

				MSG_DWORD stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= MP_DATE_CHALLENGEZONE_END_ALL_DIE;
				stPacket.dwObjectID			= pPlayer->GetID();
				stPacket.dwData				= CHALLENGE_ZONE_END_DELAY_TIME;
				pPlayer->SendMsg(&stPacket, sizeof(stPacket));	
			}
		}
		break;
	// 몬스터를 모두 잡아서 성공!
	case CHALLENGEZONE_END_SUCCESS:
		{
			if (pPlayer->GetChallengeZoneStartState() < CHALLENGEZONE_END)
			{
				CPlayer* pPartner = (CPlayer*)g_pUserTable->FindUser(pPlayer->GetChallengeZonePartnerID());
				if (pPartner)
				{
					UINT nClearTime = gCurTime - pPlayer->GetChallengeZoneStartTimeTick() - CHALLENGE_ZONE_START_DELAY_TIME;

					pPlayer->SetChallengeZoneStartState(eEndState);
					pPlayer->SetChallengeZoneStartTimeTick(gCurTime);

					pPartner->SetChallengeZoneStartState(eEndState);
					pPartner->SetChallengeZoneStartTimeTick(gCurTime);

					// 클리어 타임 설정
					pPlayer->SetChallengeZoneClearTime(nClearTime);
					pPartner->SetChallengeZoneClearTime(nClearTime);
					// DB 저장
					ChallengeZone_Success_Save(pPlayer->GetID(), pPartner->GetID(), pPlayer->GetChallengeZoneSection(), nClearTime);
					ChallengeZone_Success_Save(pPartner->GetID(), pPlayer->GetID(), pPartner->GetChallengeZoneSection(), nClearTime);
				}
			}
		}
		break;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_EnterDateZone Method																							  데이트 존 입장
//
VOID CSHDateManager::SRV_EnterDateZone(CPlayer* pPlayer, DWORD nTargetPlayerID, int nZoneIndex)
{
#if defined(_MAPSERVER_)
	MSG_DWORD stPacket;
	stPacket.Category			= MP_DATE;
	stPacket.Protocol			= MP_DATE_ENTER_DATEZONE_2;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.dwData				= nZoneIndex;

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));

	TCHAR text[MAX_PATH] = {0};
	_sntprintf(
		text,
		_countof(text),
		"map:%d(%d)",
		g_pServerSystem->GetMapNum(),
		pPlayer->GetChannelID());
	LogItemMoney(
		pPlayer->GetID(),
		pPlayer->GetObjectName(),
		0,
		text,
		eLog_DateMatchBegin,
		pPlayer->GetMoney(),
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_EnterDateZone2 Method																							  데이트 존 입장
//
VOID CSHDateManager::SRV_EnterDateZone2(CPlayer* pPlayer, MSG_DWORD4* pPacket)
{
#if defined(_MAPSERVER_)
	MSG_DWORD4 stPacket;
	stPacket.Category			= MP_DATE;
	stPacket.Protocol			= MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_2;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.dwData1			= pPacket->dwData1;
	stPacket.dwData2			= pPacket->dwData2;
	stPacket.dwData3			= pPacket->dwData3;
	stPacket.dwData4			= pPacket->dwData4;

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));	
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_EnterChallengeZone Method																						  챌린지 존 입장
//
VOID CSHDateManager::SRV_EnterChallengeZone(CPlayer* pPlayer, DWORD nTargetPlayerID, int nZoneIndex, DWORD dwExpRate, int nKind)
{
#if defined(_MAPSERVER_)
	RESULT eResult = RESULT_OK;
	MSG_DWORD2 stPacket;

	// 하루 입장 가능 회수를 넘지 않았나?
	if (pPlayer->GetChallengeZoneEnterFreq() >= ENTER_CHALLENGE_ZONE_FREQ_PER_DAY+pPlayer->GetChallengeZoneEnterBonusFreq())
	{
		eResult = RESULT_FAIL_02;
	}

	// 챌린지 존에서는 불가능
	if (IsChallengeZoneHere())
	{
		eResult = RESULT_FAIL_03;
	}

	// PK 모드 중은 불가능
	if (pPlayer->IsPKMode())
	{
		eResult = RESULT_FAIL_05;
	}

//---KES AUTONOTE
	if( pPlayer->GetAutoNoteIdx() )
	{
		eResult = RESULT_FAIL_06;
	}
//--------------

	// 090821 ONS 길드토너먼트맵 내에서는 데이트메칭 불가능.
	if(g_pServerSystem->GetMapNum() == 94)
	{
		eResult = RESULT_FAIL_13;
	}

	DWORD playerExperienceRate = 0;

	// 091124 ONS 챌린지존 경험치 비율 저장.
	if (nKind == 0)
	{
		DWORD dwPlayerExpRate = 0;
		DWORD dwTargetExpRate = 0;
		if( GENDER_MALE == pPlayer->GetGender() )
		{	
			dwPlayerExpRate = 100 - dwExpRate;
			dwTargetExpRate = dwExpRate;
		}	
		else
		{
			dwTargetExpRate = 100 - dwExpRate;
			dwPlayerExpRate = dwExpRate;
		}

		playerExperienceRate = dwPlayerExpRate;

		ChallengeZone_ExpRate_Save(pPlayer->GetID(), dwPlayerExpRate);
		ChallengeZone_ExpRate_Save(nTargetPlayerID,	dwTargetExpRate);
	}
	stPacket.Category			= MP_DATE;
	stPacket.Protocol			= MP_DATE_ENTER_CHALLENGEZONE_2;
	if (nKind != 0)
	{
		stPacket.Protocol		= MP_DATE_ENTER_CHALLENGEZONE_3;
	}
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.dwData1			= nZoneIndex;
	stPacket.dwData2			= eResult;

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));

	TCHAR text[MAX_PATH] = {0};
	_sntprintf(
		text,
		_countof(text),
		"map:%d, %d%%",
		GetMapType(nZoneIndex),
		playerExperienceRate);
	LogItemMoney(
		pPlayer->GetID(),
		pPlayer->GetObjectName(),
		nTargetPlayerID,
		text,
		eLog_DateMatchBegin,
		pPlayer->GetMoney(),
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_EnterChallengeZone Method																						  챌린지 존 입장
//
VOID CSHDateManager::SRV_EnterChallengeZone2(CPlayer* pPlayer, MSG_DWORD5* pPacket, int nKind)
{
#if defined(_MAPSERVER_)
	RESULT eResult = RESULT_OK;
	MSG_DWORD5 stPacket;

	// 하루 입장 가능 회수를 넘지 않았나?
	if (pPlayer->GetChallengeZoneEnterFreq() >= ENTER_CHALLENGE_ZONE_FREQ_PER_DAY+pPlayer->GetChallengeZoneEnterBonusFreq())
	{
		eResult = RESULT_FAIL_02;
	}

	// 챌린지 존에서는 불가능
	if (IsChallengeZoneHere())
	{
		eResult = RESULT_FAIL_03;
	}

	// PK 모드 중은 불가능
	if (pPlayer->IsPKMode())
	{
		eResult = RESULT_FAIL_05;
	}

//---KES AUTONOTE
	if( pPlayer->GetAutoNoteIdx() )
	{
		eResult = RESULT_FAIL_06;	//=_=6번이 멀까.. 일단 사용하자
	}
//--------------

	// 090821 ONS 길드토너먼트맵 내에서는 데이트메칭 불가능.
	if(g_pServerSystem->GetMapNum() == 94)
	{
		eResult = RESULT_FAIL_13;
	}

	// 091124 ONS 챌린지존 경험치 비율 저장.
	if (nKind == 0)
	{
		DWORD dwExpRate = pPacket->dwData5;
		DWORD dwPlayerExpRate = 0;
		DWORD dwTargetExpRate = 0;
		if( GENDER_MALE == pPlayer->GetGender() )
		{	
			dwPlayerExpRate = 100 - dwExpRate;
			dwTargetExpRate = dwExpRate;
		}	
		else
		{
			dwTargetExpRate = 100 - dwExpRate;
			dwPlayerExpRate = dwExpRate;
		}

		ChallengeZone_ExpRate_Save(pPlayer->GetID(), dwPlayerExpRate);
		ChallengeZone_ExpRate_Save(pPacket->dwData2, dwTargetExpRate);
	}

	stPacket.Category			= MP_DATE;
	stPacket.Protocol			= MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_2;
	if (nKind != 0)
	{
		stPacket.Protocol		= MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_4;
	}
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.dwData1			= pPacket->dwData1;
	stPacket.dwData2			= pPacket->dwData2;
	stPacket.dwData3			= pPacket->dwData3;
	stPacket.dwData4			= pPacket->dwData4;
	stPacket.dwData5			= eResult;

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));	
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ParseRequestFromClient Method																 클라이언트 요청 분석/처리(에이전트)
//
VOID CSHDateManager::ASRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_AGENTSERVER)
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;

	// 클라이언트에서 패킷 조작으로 남의 캐릭터 ID 를 보낼 수 있기 때문에
	// g_pUserTableForObjectID->FindUser 로 UserInfo 를 구하면 안 됨!!
	USERINFO* pUserInfo = g_pUserTable->FindUser( dwConnectionID );
	if( pUserInfo == NULL ) return;
	pTempMsg->dwObjectID = pUserInfo->dwCharacterID;

	switch( pTempMsg->Protocol )
	{
	// 데이트 존 입장
	// 챌린지 존 입장
	case MP_DATE_ENTER_DATEZONE:
	case MP_DATE_ENTER_CHALLENGEZONE:
		{
			// 091124 ONS 경험치비율을 전달하기 위해 메세지 수정
			MSG_DWORD3* pPacket = (MSG_DWORD3*)pMsg;
			// 요청자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			// 대상 플레이어 찾기
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pUserInfo->DateMatchingInfo.nChatPlayerID);
			if (pTargetUserInfo == NULL)
			{
				// 채팅을 강제로 끝낸다.
				// ..맵이동, 로그아웃 등등의 경우..
				pUserInfo->DateMatchingInfo.nRequestChatState			= DATE_MATCHING_CHAT_REQUEST_STATE_NONE;
				pUserInfo->DateMatchingInfo.nChatPlayerID				= NULL;
				// ..채팅이 종료되었음을 알린다.
				MSG_DWORD2 stPacket;
				stPacket.Category			= MP_RESIDENTREGIST;
				stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
				stPacket.dwObjectID			= pUserInfo->dwCharacterID;
				stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
				stPacket.dwData2			= NULL;										// 상대ID
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
				return;
			}

			// 대상 플레이어의 대화 상대가 요청자인가?
			if (pTargetUserInfo->DateMatchingInfo.nChatPlayerID != pUserInfo->dwCharacterID)
			{
				// 채팅을 강제로 끝낸다.
				// ..맵이동, 로그아웃 등등의 경우..
				pUserInfo->DateMatchingInfo.nRequestChatState			= DATE_MATCHING_CHAT_REQUEST_STATE_NONE;
				pUserInfo->DateMatchingInfo.nChatPlayerID				= NULL;
				// ..채팅이 종료되었음을 알린다.
				MSG_DWORD2 stPacket;
				stPacket.Category			= MP_RESIDENTREGIST;
				stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
				stPacket.dwObjectID			= pUserInfo->dwCharacterID;
				stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
				stPacket.dwData2			= NULL;										// 상대ID
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
				return;
			}

			// 채팅 상대 ID 입력
			pPacket->dwData2 = pUserInfo->DateMatchingInfo.nChatPlayerID;

			// 상대방의 전투상태 체크를 위해 상대방 클라이언트에 전송 - 전투상태 체크를 클라이언트에서만 함
			g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, pMsg, dwLength );
		}
		break;
	case MP_DATE_ENTER_DATEZONE_DIFF_AGENT:
	case MP_DATE_ENTER_CHALLENGEZONE_DIFF_AGENT:
		{
			MSG_DWORD3* pPacket = (MSG_DWORD3*)pMsg;
			// 요청자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			// 대상 플레이어 찾기
			// ..대상 플레이어 에이전트에 알리기
			MSG_DWORD5 stPacket;
			ZeroMemory(&stPacket, sizeof(stPacket));
			stPacket.Category			= MP_DATE;
			stPacket.Protocol			= pPacket->Protocol;;
			stPacket.dwObjectID			= pPacket->dwData2;
			stPacket.dwData1			= pUserInfo->DateMatchingInfo.nChatPlayerID;
			stPacket.dwData2			= pUserInfo->dwCharacterID;
			stPacket.dwData3			= g_pServerSystem->GetServerNum();
			stPacket.dwData4			= pPacket->dwData1;
			// 091223 ONS 경험치율 전달 처리 추가
			stPacket.dwData5			= pPacket->dwData3;
			g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
		}
		break;
	// 데이트 존 입장 - 상대방 전투 상태 체크 후
	// 챌린지 존 입장 - 상대방 전투 상태 체크 후
	case MP_DATE_ENTER_DATEZONE_2:
	case MP_DATE_ENTER_CHALLENGEZONE_2:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

			// 대상 플레이어 찾기. 대상 플레이어가 파티장임. 헷갈리면 안 되용~
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pUserInfo->DateMatchingInfo.nChatPlayerID);
			if (pTargetUserInfo == NULL)
			{
				return;
			}

			// 요청자가 채팅중인 플레이어인가?
			if (pTargetUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			// 전투 상태인가?
			if (pPacket->dwData2 != RESULT_OK)
			{
				goto FAILED;
			}

			// 채팅 상대 ID 입력
			pPacket->dwObjectID = pTargetUserInfo->dwCharacterID;
			pPacket->dwData2 = pTargetUserInfo->DateMatchingInfo.nChatPlayerID;


			// 맵서버에 전송
			g_Network.Send2Server( pTargetUserInfo->dwMapServerConnectionIndex, pMsg, dwLength );
			return;

FAILED:
			MSG_DWORD stPacket;
			stPacket.Category			= MP_DATE;
			stPacket.Protocol			= pTempMsg->Protocol;
			stPacket.dwObjectID			= pUserInfo->dwCharacterID;
			stPacket.dwData				= pPacket->dwData2;
			g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));

			stPacket.Category			= MP_DATE;
			stPacket.Protocol			= pTempMsg->Protocol;
			stPacket.dwObjectID			= pTargetUserInfo->dwCharacterID;
			stPacket.dwData				= pPacket->dwData2;
			g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));
		}
		break;
	case MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT:
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT:
		{
			MSG_DWORD6* pPacket = (MSG_DWORD6*)pMsg;

			// 전투 상태인가?
			if (pPacket->dwData5 != RESULT_OK)
			{
				MSG_DWORD stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= pTempMsg->Protocol;
				stPacket.dwObjectID			= pUserInfo->dwCharacterID;
				stPacket.dwData				= pPacket->dwData5;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));

				// 대상 플레이어 에이전트에 알리기
				{
					MSG_DWORD2 stPacket;
					stPacket.Category			= MP_DATE;
					stPacket.Protocol			= MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT_CHECK_BATTLE_FOR_OTHER_AGENT;
					if (pPacket->Protocol == MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT)
					{
						stPacket.Protocol = MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_BATTLE_FOR_OTHER_AGENT;
					}
					stPacket.dwObjectID			= pPacket->dwData3;
					stPacket.dwData1			= pPacket->dwData2;
					stPacket.dwData2			= pPacket->dwData5;
					g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
				}
			}
			else
			{
				// 대상 플레이어가 파티장이기에 파티 상태를 체크하기 위해 대상 플레이어 에이전트에 알리기
				{
					MSG_DWORD5 stPacket;
					ZeroMemory(&stPacket, sizeof(stPacket));
					stPacket.Category			= MP_DATE;
					stPacket.Protocol			= MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT;
					if (pPacket->Protocol == MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT)
					{
						stPacket.Protocol = MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT;
					}
					stPacket.dwObjectID			= pPacket->dwData3;
					stPacket.dwData1			= pPacket->dwData2;
					stPacket.dwData2			= pUserInfo->dwCharacterID;
					stPacket.dwData3			= g_pServerSystem->GetServerNum();
					stPacket.dwData4			= pPacket->dwData4;
					// 091223 ONS 경험치율 전달 처리 추가
					stPacket.dwData5			= pPacket->dwData6;
					g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
				}
			}
		}
		break;
	default:
		g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, pMsg, dwLength );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ParseRequestFromServer Method																	   서버 요청 분석/처리(에이전트)
//
VOID CSHDateManager::ASRV_ParseRequestFromServer(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_AGENTSERVER)
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	
	// USERINFO 를 참조하지 않는 경우
	//
	switch( pTempMsg->Protocol )
	{
	case MP_DATE_ENTER_DATEZONE_DIFF_AGENT:
	case MP_DATE_ENTER_CHALLENGEZONE_DIFF_AGENT:
		{
			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;

			// 대상자가 있나?
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData1);
			if (pTargetUserInfo == NULL)
			{
				return;
			}

			// 대상 플레이어의 대화 상대가 요청자인가?
			if (pTargetUserInfo->DateMatchingInfo.nChatPlayerID != pPacket->dwData2)
			{
				return;
			}

			// 상대방의 전투상태 체크를 위해 상대방 클라이언트에 전송 - 전투상태 체크를 클라이언트에서만 함
			g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, pMsg, dwLength );
		}
		return;
	case MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT_CHECK_BATTLE_FOR_OTHER_AGENT:
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_BATTLE_FOR_OTHER_AGENT:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

			// 요청자가 있나?
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData1);
			if (pUserInfo == NULL)
			{
				return;
			}

			// 대상자가 전투상태이다.
			MSG_DWORD stPacket;
			stPacket.Category			= MP_DATE;
			stPacket.Protocol			= pTempMsg->Protocol;
			stPacket.dwObjectID			= pUserInfo->dwCharacterID;
			stPacket.dwData				= pPacket->dwData2;
			g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));
		}
		return;
	case MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT:
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT:
		{
			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;

			// 요청자가 있나?
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData1);
			if (pUserInfo == NULL)
			{
				return;
			}

			// 파티 상태를 체크한다.
			// ..맵서버에 알리기
			MSG_DWORD5 stPacket;
			ZeroMemory(&stPacket, sizeof(stPacket));
			stPacket.Category			= MP_DATE;
			stPacket.Protocol			= pTempMsg->Protocol;
			stPacket.dwObjectID			= pUserInfo->dwCharacterID;
			stPacket.dwData1			= pPacket->dwData1;
			stPacket.dwData2			= pPacket->dwData2;
			stPacket.dwData3			= pPacket->dwData3;
			stPacket.dwData4			= pPacket->dwData4;
			// 091223 ONS 경험치율 전달 처리 추가
			stPacket.dwData5			= pPacket->dwData5;
			g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket));
		}
		return;
	case MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_3:
		{
			MSG_DWORD4* pPacket = (MSG_DWORD4*)pMsg;

			// 요청자가 있나?
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData1);
			if (pUserInfo == NULL)
			{
				return;
			}

			// 데이트 존으로 이동
			// ..이동 인덱스 얻기
			int nZone = 0;
			if (GetDateZoneMoveIndex(pPacket->dwData4, &nZone) == FALSE)
			{
				return;
			}

			MSG_DWORD3 stPacket;
			stPacket.Category	= MP_USERCONN;
			stPacket.Protocol	= MP_USERCONN_CHANGEMAP_SYN;
			stPacket.dwObjectID	= pUserInfo->dwCharacterID;
			stPacket.dwData1	= nZone ;
			UserConn_ChangeMap_Syn(pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}
		return;
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_3:
		{
			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;

			// 요청자가 있나?
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData1);
			if (pUserInfo == NULL)
			{
				return;
			}

			// 대상자의 파티 상태를 체크한다.
			// ..맵서버에 알리기
			MSG_DWORD5 stPacket;
			ZeroMemory(&stPacket, sizeof(stPacket));
			stPacket.Category			= MP_DATE;
			stPacket.Protocol			= MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_3;
			stPacket.dwObjectID			= pUserInfo->dwCharacterID;
			stPacket.dwData1			= pPacket->dwData1;
			stPacket.dwData2			= pPacket->dwData2;
			stPacket.dwData3			= pPacket->dwData3;
			stPacket.dwData4			= pPacket->dwData4;
			// 091223 ONS 경험치율 전달 처리 추가
			stPacket.dwData5			= pPacket->dwData5;
			g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket));
		}
		return;
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_5:
		{
			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;

			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData1);
			if (pUserInfo == NULL)
			{
				return;
			}

			// 파트너가 이미 챌린지 존으로 입장한 후 이곳이 수행되기 때문에 입장 조건을 체크하는 게 별로 의미가 없다. 그냥 입장 시킴.
			// 챌린지 존으로 이동
			// ..맵은 랜덤으로..
			if (m_nChallengeZoneMoveIndexNum == 0) return;
			int nZone = pPacket->dwData5;

			// ..입장 구간 설정
			if (ENTER_CHALLENGE_ZONE_SECTION_NUM == 0 || 
				pPacket->dwData4 >= ENTER_CHALLENGE_ZONE_SECTION_NUM) return;

			pUserInfo->nChallengeZoneEnterSection		= pPacket->dwData4;
			pUserInfo->nChallengeZoneEnterID			= pPacket->dwData3;

			// ..귀환 맵 설정
			// ....챌린지 존에서 챌린지 존으로 이동하는 경우가 아닐 경우에만.
			if (IsChallengeZone(pUserInfo->wUserMapNum) == FALSE)
			{
				pUserInfo->nChallengeZoneReturnMapNum		= pUserInfo->wUserMapNum;
			}

			// ..이동
			MSG_DWORD3 stPacket;

			stPacket.Category	= MP_USERCONN;
			stPacket.Protocol	= MP_USERCONN_CHANGEMAP_SYN;
			stPacket.dwObjectID	= pUserInfo->dwCharacterID;
			stPacket.dwData1	= nZone ;
			UserConn_ChangeMap_Syn(pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}
		return;
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_6:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

			// 요청자가 있나?
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData1);
			if (pUserInfo == NULL)
			{
				return;
			}

			// 클라이언트에 알리기
			MSG_DWORD stPacket;
			stPacket.Category			= MP_DATE;
			stPacket.Protocol			= pTempMsg->Protocol;
			stPacket.dwObjectID			= pUserInfo->dwCharacterID;
			stPacket.dwData				= pPacket->dwData2;
			g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));
		}
		return;
	}

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pTempMsg->dwObjectID);
	if (pUserInfo == NULL) return;


	switch( pTempMsg->Protocol )
	{
	// 데이트 존 입장 - 맵서버에서 파티 확인 후
	case MP_DATE_ENTER_DATEZONE_2:
		{
			MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;
			// 요청자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			// 대상 플레이어 찾기
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pUserInfo->DateMatchingInfo.nChatPlayerID);
			if (pTargetUserInfo == NULL)
			{
				return;
			}

			// 데이트 존으로 이동
			// ..이동 인덱스 얻기
			int nZone = 0;
			if (GetDateZoneMoveIndex(pPacket->dwData, &nZone) == FALSE)
			{
				return;
			}

			MSG_DWORD3 stPacket;

			stPacket.Category	= MP_USERCONN;
			stPacket.Protocol	= MP_USERCONN_CHANGEMAP_SYN;
			stPacket.dwObjectID	= pUserInfo->dwCharacterID;
			stPacket.dwData1	= nZone ;
			UserConn_ChangeMap_Syn(pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

			stPacket.Category	= MP_USERCONN;
			stPacket.Protocol	= MP_USERCONN_CHANGEMAP_SYN;
			stPacket.dwObjectID	= pTargetUserInfo->dwCharacterID;
			stPacket.dwData1	= nZone ;
			UserConn_ChangeMap_Syn( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));
		}
		break;
	case MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_2:
		{
			MSG_DWORD4* pPacket = (MSG_DWORD4*)pMsg;
			// 요청자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			// 데이트 존으로 이동
			// ..이동 인덱스 얻기
			int nZone = 0;
			if (GetDateZoneMoveIndex(pPacket->dwData4, &nZone) == FALSE)
			{
				return;
			}

			MSG_DWORD3 stPacket;

			stPacket.Category	= MP_USERCONN;
			stPacket.Protocol	= MP_USERCONN_CHANGEMAP_SYN;
			stPacket.dwObjectID	= pUserInfo->dwCharacterID;
			stPacket.dwData1	= nZone ;
			UserConn_ChangeMap_Syn(pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

			// 대상자 에이전트에 알리기
			{
				MSG_DWORD4 stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_3;
				stPacket.dwObjectID			= pPacket->dwData3;
				stPacket.dwData1			= pPacket->dwData2;
				stPacket.dwData2			= pUserInfo->dwCharacterID;
				stPacket.dwData3			= g_pServerSystem->GetServerNum();
				stPacket.dwData4			= pPacket->dwData4;
				g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
			}
		}
		break;
	// 챌린지 존 입장 - 맵서버에서 파티 확인 후, 파트너의 입장 가능 회수를 체크..
	case MP_DATE_ENTER_CHALLENGEZONE_2:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;
			// 요청자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			// 대상 플레이어 찾기
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pUserInfo->DateMatchingInfo.nChatPlayerID);
			if (pTargetUserInfo == NULL)
			{
				return;
			}

			if (pPacket->dwData2 == RESULT_OK)
			{
				// 파티장이 파티 상태와 자신의 입장 회수를 체크하고 왔으니
				// 이번엔 파트너의 입장 회수를 체크하기 위해 또 맵서버로.. 헥헥..
				MSG_DWORD2 stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= MP_DATE_ENTER_CHALLENGEZONE_3;
				stPacket.dwObjectID			= pTargetUserInfo->dwCharacterID;
				stPacket.dwData1			= pPacket->dwData1;
				g_Network.Send2Server( pTargetUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			else
			{
				MSG_DWORD stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= pTempMsg->Protocol;
				stPacket.dwObjectID			= pUserInfo->dwCharacterID;
				stPacket.dwData				= pPacket->dwData2;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));
			}
		}
		break;
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_2:
		{
			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;
			// 요청자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			if (pPacket->dwData5 == RESULT_OK)
			{
				// 파티장이 파티 상태와 자신의 입장 회수를 체크하고 왔으니
				// 이번엔 파트너의 입장 회수를 체크하기 위해 또 맵서버로.. 헥헥..
				// 에이전트가 다르므로 대상자 에이전트로..
				MSG_DWORD5 stPacket;
				ZeroMemory(&stPacket, sizeof(stPacket));
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_3;
				stPacket.dwObjectID			= pPacket->dwData3;
				stPacket.dwData1			= pPacket->dwData2;
				stPacket.dwData2			= pUserInfo->dwCharacterID;
				stPacket.dwData3			= g_pServerSystem->GetServerNum();
				stPacket.dwData4			= pPacket->dwData4;
				g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
			}
			else
			{
				MSG_DWORD stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= pTempMsg->Protocol;
				stPacket.dwObjectID			= pUserInfo->dwCharacterID;
				stPacket.dwData				= pPacket->dwData5;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));
			}
		}
		break;
	// 챌린지 존 입장 - 파트너의 입장 가능 회수 확인 후
	case MP_DATE_ENTER_CHALLENGEZONE_3:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;
			// 요청자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			// 대상 플레이어 찾기
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pUserInfo->DateMatchingInfo.nChatPlayerID);
			if (pTargetUserInfo == NULL)
			{
				return;
			}

			if (pPacket->dwData2 == RESULT_OK)
			{
				// 챌린지 존으로 이동
				// ..맵은 랜덤으로..
				if (m_nChallengeZoneMoveIndexNum == 0) return;
				
				const int nRand = rand()%m_nChallengeZoneMoveIndexNum;
				const int nZone = m_pstChallengeZoneMoveIndexList[nRand].nMoveIndex;
				const MAPTYPE mapNum = MAPTYPE(m_pstChallengeZoneMoveIndexList[nRand].nMapNum);

				if (ENTER_CHALLENGE_ZONE_SECTION_NUM == 0 || 
					pPacket->dwData1 >= ENTER_CHALLENGE_ZONE_SECTION_NUM) return;
				pUserInfo->nChallengeZoneEnterSection		= pPacket->dwData1;
				pTargetUserInfo->nChallengeZoneEnterSection = pPacket->dwData1;

				// ..입장 순서 설정
				// ....에이전트가 여러개일 때는 에이전트 별로 구분을 지어야 하므로 에이전트끼리 100000000 의 간격 설정
				// ....0 번 에이전트 0 ~ 100000000, 1 번 에이전트 100000000 ~ 200000000, 2 번 에이전트 200000000 ~ 300000000 ....
				// ....그렇기에 에이전트 번호는 적은 수부터 1씩 증가해야하며 42개(에이전트 하나당 1억을 할당할 때)를 넘지 않는 것이 문제가
				// ....발생할 확률이 적다.
				pUserInfo->nChallengeZoneEnterID			= m_nChallengeZoneEnterNum + g_pServerSystem->GetServerNum()*100000000;
				pTargetUserInfo->nChallengeZoneEnterID		= m_nChallengeZoneEnterNum + g_pServerSystem->GetServerNum()*100000000;
				m_nChallengeZoneEnterNum++;
				// ..혹시 최대범위를 넘을 경우 기본값으로 설정. 서버가 켜진 이후로 42xxxxxxxx 번의 매칭 챌린지 실행...과연 이런일이..?
				if (m_nChallengeZoneEnterNum < 1000) m_nChallengeZoneEnterNum = 1000;

				// ..귀환 맵 설정
				// ....챌린지 존에서 챌린지 존으로 이동하는 경우가 아닐 경우에만.
				if (IsChallengeZone(pUserInfo->wUserMapNum) == FALSE)
				{
					pUserInfo->nChallengeZoneReturnMapNum		= pUserInfo->wUserMapNum;
					pTargetUserInfo->nChallengeZoneReturnMapNum	= pTargetUserInfo->wUserMapNum;
				}

				WORD wServerPort = g_pServerTable->GetServerPort( eSK_MAP, (WORD)mapNum );
				SERVERINFO* pInfo = g_pServerTable->FindServer( wServerPort );				
				if( pInfo )
				{
					MSG_DWORD2 msg;
					msg.Category = MP_PARTY;
					msg.Protocol = MP_PARTY_INSTANTPARTY_REGIST_SYN;
					msg.dwObjectID = pUserInfo->dwCharacterID;
					msg.dwData1 = pUserInfo->dwCharacterID;
					msg.dwData2 = pUserInfo->dwCharacterID;
					g_Network.Send2Server( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );

					msg.Category = MP_PARTY;
					msg.Protocol = MP_PARTY_INSTANTPARTY_REGIST_SYN;
					msg.dwObjectID = pTargetUserInfo->dwCharacterID;
					msg.dwData1 = pTargetUserInfo->dwCharacterID;
					msg.dwData2 = pUserInfo->dwCharacterID;
					g_Network.Send2Server( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
				}

				// ..이동
				MSG_DWORD3 stPacket;

				stPacket.Category	= MP_USERCONN;
				stPacket.Protocol	= MP_USERCONN_CHANGEMAP_SYN;
				stPacket.dwObjectID	= pUserInfo->dwCharacterID;
				stPacket.dwData1	= nZone ;
				UserConn_ChangeMap_Syn(pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

				stPacket.Category	= MP_USERCONN;
				stPacket.Protocol	= MP_USERCONN_CHANGEMAP_SYN;
				stPacket.dwObjectID	= pTargetUserInfo->dwCharacterID;
				stPacket.dwData1	= nZone ;
				UserConn_ChangeMap_Syn( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));
			}
			else
			{
				MSG_DWORD stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= pTempMsg->Protocol;
				stPacket.dwObjectID			= pTargetUserInfo->dwCharacterID;
				stPacket.dwData				= pPacket->dwData2;
				g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));
			}
		}
		break;
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_4:
		{
			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;
			// 요청자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			if (pPacket->dwData5 == RESULT_OK)
			{
				// 챌린지 존으로 이동
				// ..맵은 랜덤으로..
				if (m_nChallengeZoneMoveIndexNum == 0) return;
				
				const int nRand = rand()%m_nChallengeZoneMoveIndexNum;
				const int nZone = m_pstChallengeZoneMoveIndexList[nRand].nMoveIndex;
				const MAPTYPE mapNum = MAPTYPE(m_pstChallengeZoneMoveIndexList[nRand].nMapNum);

				if (ENTER_CHALLENGE_ZONE_SECTION_NUM == 0 || 
					pPacket->dwData4 >= ENTER_CHALLENGE_ZONE_SECTION_NUM) return;
				pUserInfo->nChallengeZoneEnterSection		= pPacket->dwData4;

				// ..입장 순서 설정
				// ....에이전트가 여러개일 때는 에이전트 별로 구분을 지어야 하므로 에이전트끼리 100000000 의 간격 설정
				// ....0 번 에이전트 0 ~ 100000000, 1 번 에이전트 100000000 ~ 200000000, 2 번 에이전트 200000000 ~ 300000000 ....
				// ....그렇기에 에이전트 번호는 적은 수부터 1씩 증가해야하며 42개(에이전트 하나당 1억을 할당할 때)를 넘지 않는 것이 문제가
				// ....발생할 확률이 적다.
				pUserInfo->nChallengeZoneEnterID			= m_nChallengeZoneEnterNum + g_pServerSystem->GetServerNum()*100000000;
				m_nChallengeZoneEnterNum++;
				// ..혹시 최대범위를 넘을 경우 기본값으로 설정. 서버가 켜진 이후로 42xxxxxxxx 번의 매칭 챌린지 실행...과연 이런일이..?
				if (m_nChallengeZoneEnterNum < 1000) m_nChallengeZoneEnterNum = 1000;

				// ..귀환 맵 설정
				// ....챌린지 존에서 챌린지 존으로 이동하는 경우가 아닐 경우에만.
				if (IsChallengeZone(pUserInfo->wUserMapNum) == FALSE)
				{
					pUserInfo->nChallengeZoneReturnMapNum		= pUserInfo->wUserMapNum;
				}

				WORD wServerPort = g_pServerTable->GetServerPort( eSK_MAP, (WORD)mapNum );
				SERVERINFO* pInfo = g_pServerTable->FindServer( wServerPort );				
				if( pInfo )
				{
					MSG_DWORD2 msg;
					msg.Category = MP_PARTY;
					msg.Protocol = MP_PARTY_INSTANTPARTY_REGIST_SYN;
					msg.dwObjectID = pUserInfo->dwCharacterID;
					msg.dwData1 = pUserInfo->dwCharacterID;
					msg.dwData2 = pUserInfo->dwCharacterID;
					g_Network.Send2Server( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );

					msg.Category = MP_PARTY;
					msg.Protocol = MP_PARTY_INSTANTPARTY_REGIST_SYN;
					msg.dwObjectID = pUserInfo->DateMatchingInfo.nChatPlayerID;
					msg.dwData1 = pUserInfo->DateMatchingInfo.nChatPlayerID;
					msg.dwData2 = pUserInfo->dwCharacterID;
					g_Network.Send2Server( pInfo->dwConnectionIndex, (char*)&msg, sizeof(msg) );
				}

				// ..이동
				MSG_DWORD3 stPacket;

				stPacket.Category	= MP_USERCONN;
				stPacket.Protocol	= MP_USERCONN_CHANGEMAP_SYN;
				stPacket.dwObjectID	= pUserInfo->dwCharacterID;
				stPacket.dwData1	= nZone ;
				UserConn_ChangeMap_Syn(pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

				// ..대상자 에이전트에 알리기
				{
					MSG_DWORD5 stPacket;
					stPacket.Category			= MP_DATE;
					stPacket.Protocol			= MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_5;
					stPacket.dwObjectID			= pPacket->dwData3;
					stPacket.dwData1			= pPacket->dwData2;
					stPacket.dwData2			= pPacket->dwData1;
					stPacket.dwData3			= pUserInfo->nChallengeZoneEnterID;
					stPacket.dwData4			= pPacket->dwData4;
					stPacket.dwData5			= nZone;
					g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
				}
			}
			else
			{
				// 클라이언트에 알리기
				MSG_DWORD stPacket;
				stPacket.Category			= MP_DATE;
				stPacket.Protocol			= pTempMsg->Protocol;
				stPacket.dwObjectID			= pUserInfo->dwCharacterID;
				stPacket.dwData				= pPacket->dwData5;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket));

				// 대상자 에이전트에 알리기
				{
					MSG_DWORD2 stPacket;
					stPacket.Category			= MP_DATE;
					stPacket.Protocol			= MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_6;
					stPacket.dwObjectID			= pPacket->dwData3;
					stPacket.dwData1			= pPacket->dwData2;
					stPacket.dwData2			= pPacket->dwData5;
					g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
				}
			}
		}
		break;
	// 챌린지 존에서 이전 맵으로 귀환
	case MP_DATE_CHALLENGEZONE_RETURN:
		{
			LoadCharacterMap(pUserInfo->dwCharacterID);
		}
		break;
	default:
		g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pMsg, dwLength );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ParseRequestFromClient Method																		   클라이언트 요청 분석/처리
//
VOID CSHDateManager::SRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_MAPSERVER_)
	MSGBASE* pPacket = (MSGBASE*)pMsg;

	// 잘못된 플레이어 처리
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pPacket->dwObjectID );
	if( !pPlayer )	
	{
		return;
	}

	switch(pPacket->Protocol)
	{
	// 데이트 존 입장 
	case MP_DATE_ENTER_DATEZONE_2:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;
			SRV_EnterDateZone(pPlayer, pPacket->dwData2, pPacket->dwData1);
		}
		break;
	case MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT:
		{
			MSG_DWORD4* pPacket = (MSG_DWORD4*)pMsg;
			SRV_EnterDateZone2(pPlayer, pPacket);
		}
		break;
	// 챌린지 존 입장 
	case MP_DATE_ENTER_CHALLENGEZONE_2:
		{
			// 091124 ONS 경험치비율을 전달한다.
			MSG_DWORD3* pPacket = (MSG_DWORD3*)pMsg;
			SRV_EnterChallengeZone(pPlayer, pPacket->dwData2, pPacket->dwData1, pPacket->dwData3);
		}
		break;
	case MP_DATE_ENTER_CHALLENGEZONE_3:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;
			// 091124 ONS 경험치비율을 전달한다.
			SRV_EnterChallengeZone(pPlayer, pPacket->dwData2, pPacket->dwData1, 0, 1);
		}
		break;
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT:
		{
			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;
			SRV_EnterChallengeZone2(pPlayer, pPacket);
		}
		break;
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_3:
		{
			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;
			SRV_EnterChallengeZone2(pPlayer, pPacket, 1);
		}
		break;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_IsPossibleEnterDateZone Method																		  데이트 존 입장이 가능한가?
//
BOOL CSHDateManager::CLI_IsPossibleEnterDateZone()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;

	if( pDlg &&
		pDlg->GetChatingDlg()->IsOnChatMode() &&
		HERO->GetGender() != g_csResidentRegistManager.CLI_GetChatPartner().stRegistInfo.nSex)
	{
		return TRUE;
	}

#endif
	return FALSE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_IsPossibleEnterChallengeZone Method																	  챌린지 존 입장이 가능한가?
//
BOOL CSHDateManager::CLI_IsPossibleEnterChallengeZone()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;

	if( pDlg &&
		pDlg->GetChatingDlg()->IsOnChatMode() &&
		HERO->GetGender() != g_csResidentRegistManager.CLI_GetChatPartner().stRegistInfo.nSex &&
		g_csResidentRegistManager.CLI_GetChatPartner().nMatchingPoint >= ENTER_CHALLENGE_ZONE_MATCHING_POINT &&
		IsChallengeZoneHere() == FALSE)
	{
		return TRUE;
	}

#endif
	return FALSE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestDateMatchingEnterDateZone Method															  데이트 매칭 데이트 존 입장 요청
//
VOID CSHDateManager::CLI_RequestDateMatchingEnterDateZone(int nZoneIndex)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (HERO->GetObjectBattleState() == eObjectBattleState_Battle)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1209) );
		return;
	}

	MSG_DWORD2 stPacket;
	stPacket.Category 		= MP_DATE;
	stPacket.Protocol 		= MP_DATE_ENTER_DATEZONE;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData1		= nZoneIndex;

	if (g_csResidentRegistManager.CLI_GetChatPartner().nAgentID >= 1000)
	{
		stPacket.Protocol 		= MP_DATE_ENTER_DATEZONE_DIFF_AGENT;
		stPacket.dwData2		= g_csResidentRegistManager.CLI_GetChatPartner().nAgentID - 1000;
	}

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestDateMatchingEnterChallengeZone Method													  데이트 매칭 챌린지 존 입장 요청
//
VOID CSHDateManager::CLI_RequestDateMatchingEnterChallengeZone(int nZoneIndex, DWORD dwExpRate)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (HERO->GetObjectBattleState() == eObjectBattleState_Battle)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1209) );
		return;
	}

	// 091124 ONS 경험치비율을 전달하도록 패킷 수정
	MSG_DWORD3 stPacket;
	stPacket.Category 		= MP_DATE;
	stPacket.Protocol 		= MP_DATE_ENTER_CHALLENGEZONE;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData1		= nZoneIndex;

	if (g_csResidentRegistManager.CLI_GetChatPartner().nAgentID >= 1000)
	{
		stPacket.Protocol 		= MP_DATE_ENTER_CHALLENGEZONE_DIFF_AGENT;
		stPacket.dwData2		= g_csResidentRegistManager.CLI_GetChatPartner().nAgentID - 1000;
	}

	stPacket.dwData3			= dwExpRate;

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
	WINDOWMGR->MsgBox( MBI_MATCHAT_CONFIRM, MBT_OK, CHATMGR->GetChatMsg( 2219 ) );

#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_ParseAnswerFromSrv Method																			  서버로부터의 응답 분석/처리
//
VOID CSHDateManager::CLI_ParseAnswerFromSrv(void* pMsg)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	MSGBASE* pTmp = (MSGBASE*)pMsg;

	switch(pTmp->Protocol)
	{
	// 데이트 존 입장을 위한 전투 상태 체크
	case MP_DATE_ENTER_DATEZONE:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;
			MSG_DWORD2 stPacket;

			stPacket.Category 		= MP_DATE;
			stPacket.Protocol 		= MP_DATE_ENTER_DATEZONE_2;
			stPacket.dwObjectID		= HEROID;
			stPacket.dwData1		= pPacket->dwData1;
			stPacket.dwData2		= RESULT_OK;
			if (HERO->GetObjectBattleState() == eObjectBattleState_Battle)
			{
				stPacket.dwData2 = RESULT_FAIL_01;
			}

			NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
		}
		break;
	// 데이트 존 입장을 위한 전투 상태 체크
	case MP_DATE_ENTER_DATEZONE_DIFF_AGENT:
		{
			MSG_DWORD4* pPacket = (MSG_DWORD4*)pMsg;
			MSG_DWORD5 stPacket;

			stPacket.Category 		= MP_DATE;
			stPacket.Protocol 		= MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT;
			stPacket.dwObjectID		= HEROID;
			stPacket.dwData1		= pPacket->dwData1;
			stPacket.dwData2		= pPacket->dwData2;
			stPacket.dwData3		= pPacket->dwData3;
			stPacket.dwData4		= pPacket->dwData4;
			stPacket.dwData5		= RESULT_OK;
			if (HERO->GetObjectBattleState() == eObjectBattleState_Battle)
			{
				stPacket.dwData5 = RESULT_FAIL_01;
			}

			NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
		}
		break;
	// 챌린지 존 입장을 위한 전투 상태 체크
	case MP_DATE_ENTER_CHALLENGEZONE:
		{
			// 091124 ONS 경험치비율을 전달하도록 패킷 수정
			MSG_DWORD3* pPacket = (MSG_DWORD3*)pMsg;

			DWORD dwZoneIndex = pPacket->dwData1;
			DWORD dwExpRate = pPacket->dwData3;

			GAMEIN->GetChallengeZoneListDlg()->SetActive(TRUE);
			GAMEIN->GetChallengeZoneListDlg()->SetGuestMode(dwZoneIndex, dwExpRate);
		}
		break;
	// 챌린지 존 입장을 위한 전투 상태 체크
	case MP_DATE_ENTER_CHALLENGEZONE_DIFF_AGENT:
		{
			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;

			DWORD dwPartnerIndex = pPacket->dwData2;
			DWORD dwDiffAgentID = pPacket->dwData3;
			DWORD dwZoneIndex = pPacket->dwData4;
			DWORD dwExpRate = pPacket->dwData5;

			GAMEIN->GetChallengeZoneListDlg()->SetActive(TRUE);
			GAMEIN->GetChallengeZoneListDlg()->SetGuestMode(dwZoneIndex, dwExpRate, TRUE, dwDiffAgentID, dwPartnerIndex);
		}
		break;
	// 데이트 존 입장 불가
	// 챌린지 존 입장 불가
	case MP_DATE_ENTER_DATEZONE_2:
	case MP_DATE_ENTER_CHALLENGEZONE_2:
	case MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT:
	case MP_DATE_ENTER_DATEZONE_2_DIFF_AGENT_CHECK_BATTLE_FOR_OTHER_AGENT:
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT:
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_BATTLE_FOR_OTHER_AGENT:
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_2:
		{
			MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;
			switch(pPacket->dwData)
			{
			case RESULT_FAIL_01:		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1209) );	break;
			case RESULT_FAIL_02:		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1210) );	break;
//			case RESULT_FAIL_03:		CHATMGR->AddMsg( CTC_SYSMSG, "이미 챌린지 중입니다." );		break;
			case RESULT_FAIL_04:		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1211) );	break;
			case RESULT_FAIL_05:		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1263) );	break;
			// 090821 ONS 길드 토너먼트 맵에서 데이트매칭 이동 금지.
			case RESULT_FAIL_13:		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1656) );	break;
			}
		}
		break;
	case MP_DATE_ENTER_CHALLENGEZONE_3:
	case MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT_CHECK_PARTY_FOR_OTHER_AGENT_6:
		{
			MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;
			switch(pPacket->dwData)
			{
			case RESULT_FAIL_02:		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1212) );			break;
//			case RESULT_FAIL_03:		CHATMGR->AddMsg( CTC_SYSMSG, "파트너가 이미 챌린지 중입니다." );	break;
			case RESULT_FAIL_04:		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1213) );			break;
			case RESULT_FAIL_05:		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1264) );	break;
			}
		}
		break;
	// 챌린지 존 시작
	case MP_DATE_CHALLENGEZONE_START:
		{
 			char szTxt[256] = "";
			MSG_NAME2_DWORD3* pPacket = (MSG_NAME2_DWORD3*)pMsg;

			m_nChallengeZoneState = CHALLENGEZONE_START;
			m_nChallengeZoneTimeTick = gCurTime;
			m_nChallengeZoneTime = 0;

			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetActive(TRUE);
			// 구간
			sprintf(szTxt, "LV. %d~%d", m_pstChallengeZoneSectionMonLevel[pPacket->dwData3].nStart, m_pstChallengeZoneSectionMonLevel[pPacket->dwData3].nEnd);
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneLeastClearSection(szTxt);
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneStartTime(m_pstChallengeZoneSectionMonLevel[pPacket->dwData3].nLimitTime - CHALLENGE_ZONE_START_DELAY_TIME);
		
			// 091124 ONS 파티원 정보 : 이름(경험치비율)를 표시
			sprintf(szTxt, "%s ( %u%c )", pPacket->Name1, pPacket->dwData1, '%');
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneExpRatePlayer1(szTxt);
			ZeroMemory(szTxt, sizeof(szTxt));
			sprintf(szTxt, "%s ( %u%c )", pPacket->Name2, pPacket->dwData2, '%');
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneExpRatePlayer2(szTxt);
		}
		break;
	// 챌린지 존 입장 회수 설정
	case MP_DATE_CHALLENGEZONE_ENTER_FREQ:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

 			if (HERO == NULL) return;

			int nPossibleFreq = ENTER_CHALLENGE_ZONE_FREQ_PER_DAY - pPacket->dwData1;
			if (nPossibleFreq < 0) nPossibleFreq = 0;
			nPossibleFreq += pPacket->dwData2;
			GAMEIN->GetDateMatchingDlg()->SetEnterChallengeZoneFreq(nPossibleFreq);
		}
		break;
	// 챌린지 존 실패 - 타임아웃
	case MP_DATE_CHALLENGEZONE_END_TIMEOUT:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1214));
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1220), CHALLENGE_ZONE_END_DELAY_TIME/1000);
			m_nChallengeZoneState = CHALLENGEZONE_END;
			m_nChallengeZoneTimeTick = gCurTime;
			m_nChallengeZoneTime = 0;

			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneStartTime(0);
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneStartTimeTick(0);
			// 이펙트 연출
			if (CHALLENGE_ZONE_MOTION_NUM_FAIL)
				EFFECTMGR->StartEffectProcess(CHALLENGE_ZONE_MOTION_NUM_FAIL, HERO, NULL, 0, 0);
		}
		break;
	// 챌린지 존 실패 - 파트너의 이탈
	case MP_DATE_CHALLENGEZONE_END_PARTNER_OUT:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1215));
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1220), CHALLENGE_ZONE_END_DELAY_TIME/1000);
			m_nChallengeZoneState = CHALLENGEZONE_END;
			m_nChallengeZoneTimeTick = gCurTime;
			m_nChallengeZoneTime = 0;

			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneStartTime(0);
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneStartTimeTick(0);
			// 이펙트 연출
			if (CHALLENGE_ZONE_MOTION_NUM_FAIL)
				EFFECTMGR->StartEffectProcess(CHALLENGE_ZONE_MOTION_NUM_FAIL, HERO, NULL, 0, 0);
		}
		break;
	// 챌린지 존 실패 - 모두 사망
	case MP_DATE_CHALLENGEZONE_END_ALL_DIE:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1216));
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1220), CHALLENGE_ZONE_END_DELAY_TIME/1000);
			m_nChallengeZoneState = CHALLENGEZONE_END;
			m_nChallengeZoneTimeTick = gCurTime;
			m_nChallengeZoneTime = 0;

			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneStartTime(0);
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneStartTimeTick(0);
			// 이펙트 연출
			if (CHALLENGE_ZONE_MOTION_NUM_FAIL)
				EFFECTMGR->StartEffectProcess(CHALLENGE_ZONE_MOTION_NUM_FAIL, HERO, NULL, 0, 0);
		}
		break;
	// 챌린지 존 성공 
	case MP_DATE_CHALLENGEZONE_END_SUCCESS:
		{
			MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;

			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1217));
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1220), CHALLENGE_ZONE_END_DELAY_TIME/1000);
			m_nChallengeZoneState = CHALLENGEZONE_END;
			m_nChallengeZoneTimeTick = gCurTime;
			m_nChallengeZoneTime = 0;

			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneStartTime(0);
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetChallengeZoneStartTimeTick(0);
			// 최단 시간 클리어 타임을 갱신했나?
			// ..이펙트 연출
			if (pPacket->dwData)
			{
				if (CHALLENGE_ZONE_MOTION_NUM_SUCCESS)
					EFFECTMGR->StartEffectProcess(CHALLENGE_ZONE_MOTION_NUM_SUCCESS, HERO, NULL, 0, 0);
			}
			else
			{
				if (CHALLENGE_ZONE_MOTION_NUM_SUCCESS_LEAST_CLEAR_TIME)
					EFFECTMGR->StartEffectProcess(CHALLENGE_ZONE_MOTION_NUM_SUCCESS_LEAST_CLEAR_TIME, HERO, NULL, 0, 0);
			}
		}
		break;
	}
#endif
}

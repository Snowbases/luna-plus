// GuildTournamentMgr.cpp: implementation of the CGuildTournamentMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuildTournamentMgr.h"
#include "Guild.h"
#include "MHFile.h"
#include "UserTable.h"
#include "Player.h"
#include "GuildManager.h"
#include "BattleSystem_Server.h"
#include "Network.h"
#include "CharMove.h"
#include "MapDBMsgParser.h"
#include "GuildMark.h"
#include "PackedData.h"
#include "QuestManager.h"
#include "PartyManager.h"
#include "ItemManager.h"
#include "petmanager.h"


#include "..\[CC]Header\GameResourceManager.h"
#include "../hseos/Date/SHDateManager.h"

extern int g_nServerSetNum;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuildTournamentMgr::CGuildTournamentMgr()
{
	m_pEntryList.Initialize(32);
	m_dwWaitingFlag = GT_WAITING_NONE;
	m_dwWaitingForDBLoad = 15000;
	m_bNeedAdjust = TRUE;
}

CGuildTournamentMgr::~CGuildTournamentMgr()
{
	ReadyToNewTournament();

	GTInfo* pInfo = NULL;
	m_pEntryList.SetPositionHead();
	while((pInfo = m_pEntryList.GetData())!= NULL)
	{
		pInfo->ClearPlayerList();
		delete pInfo;
		pInfo = NULL;
	}
	m_pEntryList.RemoveAll();
}


void CGuildTournamentMgr::Init()
{
	int nMap = g_pServerSystem->GetMapNum();
	if( nMap != GTMAPNUM )		return;

	CMHFile file;

	if(!file.Init("system/Resource/GuildTournament.bin", "rb"))
		return;

	char string[256] = {0,};

	while(!file.IsEOF())
	{
		file.GetString(string);
		if(string[0] == '@')
		{
			file.GetLineX(string, 256);
			continue;
		}
		else if(0==strcmp(string, "#OB_ALLOW"))
		{
			m_GeneralInfo.bObAllow = file.GetBool();
		}
		else if(0==strcmp(string, "#ENTRY_FEE"))
		{
			m_GeneralInfo.dwFee[0] = file.GetDword();
			m_GeneralInfo.dwFee[1] = file.GetDword();
		}
		else if(0==strcmp(string, "#LIMIT_TEAM"))
		{
			m_GeneralInfo.wLimit_Team[0] = file.GetWord();
			m_GeneralInfo.wLimit_Team[1] = file.GetWord();
		}
		else if(0==strcmp(string, "#LIMIT_GUILD"))
		{
			m_GeneralInfo.wLimit_Guild[0] = file.GetWord();
			m_GeneralInfo.wLimit_Guild[1] = file.GetWord();
		}
		else if(0==strcmp(string, "#ENTRY_TERM"))
		{
			m_GeneralInfo.dwEntryStart = file.GetDword();
			m_GeneralInfo.dwEntryEnd = file.GetDword();
		}
		else if(0==strcmp(string, "#START_TIME"))
		{
			m_GeneralInfo.dwStartTime = file.GetDword();
		}
		else if(0==strcmp(string, "#TABLE_OPEN"))
		{
			m_GeneralInfo.wTableOpen = file.GetWord();
		}
		else if(0==strcmp(string, "#PLAY_TYPE"))
		{
			m_GeneralInfo.wPlayRule[0] = file.GetWord();
		}
		else if(0==strcmp(string, "#PLAY_GOAL"))
		{
			m_GeneralInfo.wPlayRule[1] = file.GetWord();
		}
		else if(0==strcmp(string, "#PLAY_TIME"))
		{
			m_GeneralInfo.wPlayTime[0] = file.GetWord();
			m_GeneralInfo.wPlayTime[1] = file.GetWord();
		}
		else if(0==strcmp(string, "#1stREWARD_POINT"))
		{
			m_GeneralInfo.RewardInfo[0].dwPoint = file.GetDword();
		}
		else if(0==strcmp(string, "#1stREWARD_ITEM"))
		{
			GTRewardItem* pItem = new GTRewardItem;
			if(pItem)
			{
				pItem->nReceiver = file.GetByte();
				pItem->dwItemIndex = file.GetDword();
				pItem->wNum = file.GetWord();

				m_GeneralInfo.RewardInfo[0].RewardItemList.Add(pItem, pItem->dwItemIndex);
			}
		}
		// 091113 ONS 길드토너먼트 1위 골드 보상 추가
		else if(0==strcmp(string, "#1stREWARD_GOLD"))
		{
			m_GeneralInfo.RewardInfo[0].dwGold = file.GetDword();
		}
		else if(0==strcmp(string, "#2ndREWARD_POINT"))
		{
			m_GeneralInfo.RewardInfo[1].dwPoint = file.GetDword();
		}
		else if(0==strcmp(string, "#2ndREWARD_ITEM"))
		{
			GTRewardItem* pItem = new GTRewardItem;
			if(pItem)
			{
				pItem->nReceiver = file.GetByte();
				pItem->dwItemIndex = file.GetDword();
				pItem->wNum = file.GetWord();

				m_GeneralInfo.RewardInfo[1].RewardItemList.Add(pItem, pItem->dwItemIndex);
			}
		}
		// 091113 ONS 길드토너먼트 2위 골드 보상 추가
		else if(0==strcmp(string, "#2ndREWARD_GOLD"))
		{
			m_GeneralInfo.RewardInfo[1].dwGold = file.GetDword();
		}
		else if(0==strcmp(string, "#RESPAWNSIZE"))
		{
			int nSize = file.GetInt();
			m_GeneralInfo.nRespawnListSize = nSize;
			m_GeneralInfo.pRespawnList = new GTLevel_Value[nSize];
		}
		else if(0==strcmp(string, "#RESPAWN"))
		{
			static int nRespawnSize = 0;
			if(nRespawnSize <= m_GeneralInfo.nRespawnListSize)
			{
				if(nRespawnSize == 0)
				{
					m_GeneralInfo.pRespawnList[nRespawnSize].wMinLevel = 1;
					m_GeneralInfo.pRespawnList[nRespawnSize].wMaxLevel = file.GetWord();
					m_GeneralInfo.pRespawnList[nRespawnSize].dwValue = file.GetDword();
				}
				else
				{
					m_GeneralInfo.pRespawnList[nRespawnSize].wMinLevel = m_GeneralInfo.pRespawnList[nRespawnSize-1].wMaxLevel+1;
					m_GeneralInfo.pRespawnList[nRespawnSize].wMaxLevel = file.GetWord();
					m_GeneralInfo.pRespawnList[nRespawnSize].dwValue = file.GetDword();
				}

				nRespawnSize++;
			}
		}
		else if(0==strcmp(string, "#IMMORTAL"))
		{
			GTImmortalInfo* pInfo = new GTImmortalInfo;
			if(pInfo)
			{
				pInfo->dwClassCode = file.GetDword();
				pInfo->dwTime = file.GetDword();

				m_GeneralInfo.ImmortalList.Add(pInfo, pInfo->dwClassCode);
			}
		}
	}
	file.Release();

	ReadyToNewTournament();
	m_wTournamentCount = 0;


	// 임시 문제발생시 로그남기기 : 서버기동
	FILE* fp = fopen("gtmap.log", "a+");
	if(fp)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		char buf[256] = {0,};
		fprintf(fp, "\n\n========================================\n");
		sprintf(buf, "Server Init() : %02d/%02d (%02d:%02d)\n", st.wMonth, st.wDay, st.wHour, st.wMinute);
		fprintf(fp, buf);
		fflush(fp);
		fclose(fp);
	}
}

void CGuildTournamentMgr::SetPlayerScore(DWORD dwGuildIdx, DWORD dwPlayerIdx, DWORD dwScore)
{
	m_pEntryList.SetPositionHead();

	GTInfo* pInfo = m_pEntryList.GetData(dwGuildIdx);
	if(pInfo)
	{
		GTPlayerInfo* pPlayerInfo = pInfo->PlayerList.GetData(dwPlayerIdx);
		if(pPlayerInfo)
			pPlayerInfo->dwScore = dwScore;
	}
}

DWORD CGuildTournamentMgr::GetPlayerScore(DWORD dwGuildIdx, DWORD dwPlayerIdx)
{
	m_pEntryList.SetPositionHead();

	GTInfo* pInfo = m_pEntryList.GetData(dwGuildIdx);
	if(pInfo)
	{
		GTPlayerInfo* pPlayerInfo = pInfo->PlayerList.GetData(dwPlayerIdx);
		if(pPlayerInfo)
			return pPlayerInfo->dwScore;
	}

	return 0;
}

void CGuildTournamentMgr::ReadyToNewTournament()
{
	m_bStartGame = FALSE;
	m_bShuffleBattleTable = FALSE;
	m_bIgnoreSchedule = FALSE;

	// 남아있는 유저삭제
	if(g_pUserTable)
	{
		g_pUserTable->SetPositionHead();
		CPlayer* pPlayer = NULL;
		while((pPlayer = (CPlayer*)g_pUserTable->GetData())!= NULL)
		{
			g_pServerSystem->RemovePlayer(pPlayer->GetID(), FALSE);
		}
		g_pUserTable->RemoveAll();
	}

	// 길드신청리스트 삭제
	DWORD dwSize = m_pEntryList.GetDataNum();
	if(0 < dwSize)
	{
		m_pEntryList.SetPositionHead();
		GTInfo* pInfo;

		while((pInfo = m_pEntryList.GetData())!= NULL)
		{
			pInfo->ClearPlayerList();
			delete pInfo;
			pInfo = NULL;
		}
	}
	m_pEntryList.RemoveAll();

	// 대진표정보 삭제
	memset(m_BattleInfo, 0, sizeof(m_BattleInfo));
	memset(m_CurBattleTable, 0, sizeof(m_CurBattleTable));
	memset(m_InitBattleTable, 0, sizeof(m_InitBattleTable));
	

	// 상태초기화
	int nCount = m_GeneralInfo.wLimit_Team[1];
	int nMatch = nCount / 2;

	switch(nMatch)
	{
	case 16:	m_wCurRound = eGTFight_32;	break;
	case 8:		m_wCurRound = eGTFight_16;	break;
	case 4:		m_wCurRound = eGTFight_8;	break;
	case 2:		m_wCurRound = eGTFight_4;	break;
	case 1:		m_wCurRound = eGTFight_2;	break;
	default:	m_wCurRound = eGTFight_End;	break;
	}
	m_wInitRound = m_wCurRound;

	int nMod = nCount;
	m_wMaxRound = 0;
	while(1)
	{
		m_wMaxRound++;
		nMod /= 2;
		if(nMod < 2)
			break;
	}

	ReadyToNewMatch();
	m_wTournamentState = eGTState_BeforeRegist;

	m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
}

void CGuildTournamentMgr::ReadyToNewMatch()
{
	m_wInTournamentTeam = 0;
	m_wEndTournamentTeam = 0;
	m_wTournamentState = eGTState_BeforeEntrance;
	m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

	GTInfo* pInfo = NULL;
	m_pEntryList.SetPositionHead();
	while((pInfo = m_pEntryList.GetData())!= NULL)
		pInfo->ResetPlayerScore();
}

void CGuildTournamentMgr::Process()
{
	if( g_pServerSystem->GetMapNum() != GTMAPNUM )		return;

	if(m_dwWaitingFlag != GT_WAITING_NONE)
		return;

	if(m_bNeedAdjust || m_dwWaitingForDBLoad)
	{
		if(gTickTime < m_dwWaitingForDBLoad)
		{
			m_dwWaitingForDBLoad -= gTickTime;
			return;
		}

		WORD wState = AdjustedState();

		// 임시 문제발생시 로그남기기 : 엔트리추가
		FILE* fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			char buf[256] = {0,};
			sprintf(buf, "AdjustedState() : Adjust(%d), \n", wState);
			fprintf(fp, buf);
			fflush(fp);
			fclose(fp);
		}

		m_bNeedAdjust = FALSE;
		m_dwWaitingForDBLoad = 0;
	}

	switch(m_wTournamentState)
	{
		case eGTState_BeforeRegist:		Process_BeforeRegist();		break;
		case eGTState_Regist:			Process_Regist();			break;
		case eGTState_BeforeEntrance:	Process_BeforeEntrance();	break;
		case eGTState_Entrance:			Process_Entrance();			break;
		case eGTState_Process:			Process_Process();			break;
		case eGTState_Leave:			Process_Leave();			break;		
	}

	static WORD wLastState = m_wTournamentState;
	if(wLastState != m_wTournamentState)
	{
		wLastState = m_wTournamentState;
		GTInfoUpdate(m_wTournamentCount, 0, m_wTournamentState, m_wCurRound);
	}
}

void CGuildTournamentMgr::Process_BeforeRegist()
{
	if(gTickTime < m_dwRemainTime)
		m_dwRemainTime -= gTickTime;
	else
	{
		if(m_bIgnoreSchedule)	return;

		// 임시 문제발생시 로그남기기 : Process_BeforeRegist()
		FILE* fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			fprintf(fp, "Process_BeforeRegist()-Start\n");
			fflush(fp);
			fclose(fp);
		}

		SYSTEMTIME st;
		GetLocalTime(&st);

		WORD wStartDay = (WORD)(m_GeneralInfo.dwEntryStart/10000);
		WORD wEndDay = (WORD)(m_GeneralInfo.dwEntryEnd/10000);

		if(wStartDay!=7 && wEndDay!=7)
		{
			// 주중 1회경기
			m_wTournamentCount++;

			ReadyToNewTournament();
			m_wTournamentState = eGTState_Regist;
			Notify_Send2AllUser(eGTNotify_RegistStart, m_wTournamentCount);	// 신청시작 통보

			m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryEnd);
			m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

			// DB update
			GTInfoInsert(m_wTournamentCount, 0, "*GTRegist", eGTState_Regist, m_wCurRound);	// 새 토너먼트로 회차가 넘어갔음을 기록
			GTInfoLoad();
		}
		else
		{
			// 매일 경기
			if(wStartDay!=7 || wEndDay!=7)						return;	// 입력오류

			WORD wDay, wHour, wMin;
			GetTimeFromGTTime(m_GeneralInfo.dwEntryStart, wDay, wHour, wMin);

			m_wTournamentCount++;

			ReadyToNewTournament();
			m_wTournamentState = eGTState_Regist;
			Notify_Send2AllUser(eGTNotify_RegistStart, m_wTournamentCount);	// 신청시작 통보

			m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryEnd);
			m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

			// DB update
			GTInfoInsert(m_wTournamentCount, 0, "*GTRegist", eGTState_Regist, m_wCurRound);	// 새 토너먼트로 회차가 넘어갔음을 기록
			GTInfoLoad();
		}

		// 임시 문제발생시 로그남기기 : Process_BeforeRegist()
		fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			fprintf(fp, "Process_BeforeRegist()-End\n");
			fflush(fp);
			fclose(fp);
		}
	}
}

void CGuildTournamentMgr::Process_Regist()
{
	if(gTickTime < m_dwRemainTime)
	{
		if(50000<m_dwRemainTime && m_dwRemainTime<=m_dwLastNotifyTime)
		{
			Notify_Send2AllUser(eGTNotify_RegistRemain, m_dwRemainTime);	// 신청마감안내
			if(m_dwRemainTime < GT_NOTIFY_INTERVAL)
				m_dwLastNotifyTime = 0;
			else
				m_dwLastNotifyTime = m_dwRemainTime - GT_NOTIFY_INTERVAL;
		}

		m_dwRemainTime -= gTickTime;
	}
	else
	{
		//if(m_bIgnoreSchedule)	return;

		// 임시 문제발생시 로그남기기 : Process_Regist()
		FILE* fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			fprintf(fp, "Process_Regist()-Start\n");
			fflush(fp);
			fclose(fp);
		}

		Notify_Send2AllUser(eGTNotify_RegistEnd, m_wTournamentCount);	// 신청마감 통보
		MakeBattleTable();	// 신청마감후 바로 대진표생성
		
		m_wTournamentState = eGTState_BeforeEntrance;

		DWORD dwEntranceTime = GetGTTimeOP(m_GeneralInfo.dwStartTime, m_GeneralInfo.wPlayTime[1], eGTTime_OP_Sub);
		m_dwRemainTime = GetRemaindTime(dwEntranceTime);
		m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

		Notify_Send2AllUser(eGTNotify_BattleTableOpen, m_wTournamentCount, m_dwRemainTime);	// 대진표공개 통보

		// 임시 문제발생시 로그남기기 : Process_Regist()
		fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			fprintf(fp, "Process_Regist()-End\n");
			fflush(fp);
			fclose(fp);
		}
	}
}

void CGuildTournamentMgr::Process_BeforeEntrance()
{
	if(gTickTime < m_dwRemainTime)
	{
		if(50000<m_dwRemainTime && m_dwRemainTime <= m_dwLastNotifyTime)
		{
			if(m_bStartGame)
				Notify_Send2AllUser(eGTNotify_RoundStart, m_wCurRound, m_dwRemainTime);	// 라운드 시작통보

			if(m_dwRemainTime < GT_NOTIFY_INTERVAL)
				m_dwLastNotifyTime = 0;
			else
				m_dwLastNotifyTime = m_dwRemainTime - GT_NOTIFY_INTERVAL;
		}

		m_dwRemainTime -= gTickTime;
	}
	else
	{
		//if(m_bIgnoreSchedule)	return;

		// 임시 문제발생시 로그남기기 : Process_BeforeEntrance()
		FILE* fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			fprintf(fp, "Process_BeforeEntrance()-Start\n");
			fflush(fp);
			fclose(fp);
		}

		if(!m_bStartGame)
			Notify_Send2AllUser(eGTNotify_TournamentStart, m_wTournamentCount);	// 토너먼트 시작통보

		CreateBattle(); // 배틀생성

		// 배틀생성결과 결승이 유효한경기가 아니면 종료단계로 점프.
		if(m_bNoMatchRound && m_wCurRound==eGTFight_2)
		{
			m_wTournamentState = eGTState_Leave;
			return;
		}

		m_wTournamentState = eGTState_Entrance;

		if(!m_bStartGame)
		{
			m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwStartTime);
			m_bStartGame = TRUE;
		}
		else
		{
			m_dwRemainTime = m_GeneralInfo.wPlayTime[1] * 60000;
		}

		Notify_Send2AllUser(eGTNotify_RoundStart, m_wCurRound, m_dwRemainTime);	// 라운드 시작통보
		m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;
		Notify_Send2AllGTPlayer(eGTNotify_RoundStartforPlayer, m_wCurRound, m_dwRemainTime);// 라운드 시작통보 for 선수

		// 임시 문제발생시 로그남기기 : Process_BeforeEntrance()
		fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			fprintf(fp, "Process_BeforeEntrance()-End\n");
			fflush(fp);
			fclose(fp);
		}
	}
}

void CGuildTournamentMgr::Process_Entrance()
{
	if(gTickTime < m_dwRemainTime)
	{
		if(50000<m_dwRemainTime && m_dwRemainTime <= m_dwLastNotifyTime)
		{
			if(m_bStartGame)
			{
				Notify_Send2AllUser(eGTNotify_RoundStart, m_wCurRound, m_dwRemainTime);	// 라운드 시작통보
				Notify_Send2AllGTPlayer(eGTNotify_RoundStartforPlayer, m_wCurRound, m_dwRemainTime);// 라운드 시작통보 for 선수
			}

			if(m_dwRemainTime < GT_NOTIFY_INTERVAL*2)
				m_dwLastNotifyTime = 0;
			else
				m_dwLastNotifyTime = m_dwRemainTime - GT_NOTIFY_INTERVAL*2;
		}

		m_dwRemainTime -= gTickTime;
	}
	else
	{
		//if(m_bIgnoreSchedule)	return;

		// 임시 문제발생시 로그남기기 : Process_Entrance()
		FILE* fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			fprintf(fp, "Process_Entrance()-Start\n");
			fflush(fp);
			fclose(fp);
		}

		StartTournament();

		// 라운드 시작 통보
		Notify_Send2AllUser(eGTNotify_RoundStart, m_wCurRound, 0);	// 라운드 시작통보

		m_wTournamentState = eGTState_Process;

		m_dwRemainTime = m_GeneralInfo.wPlayTime[0] * 60000;
		m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

		// 임시 문제발생시 로그남기기 : Process_Entrance()
		fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			fprintf(fp, "Process_Entrance()-End\n");
			fflush(fp);
			fclose(fp);
		}
	}
}

void CGuildTournamentMgr::Process_Process()
{
	////이 라운드의 모든경기가 종료되었으면 즉시 다음단계로
	//if(m_wInTournamentTeam <= m_wEndTournamentTeam)
	//{
	//	m_wTournamentState = eGTState_Leave;
	//	m_dwRemainTime = 3000;
	//	m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;
	//}
	//else
	//{
		if(gTickTime < m_dwRemainTime)
			m_dwRemainTime -= gTickTime;
		else
		{
			m_wTournamentState = eGTState_Leave;
			m_dwRemainTime = 3000;
			m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;
		}
	//}
}

void CGuildTournamentMgr::Process_Leave()
{
	if(gTickTime < m_dwRemainTime)
		m_dwRemainTime -= gTickTime;
	else
	{
		// 임시 문제발생시 로그남기기 : Process_Leave()
		FILE* fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			fprintf(fp, "Process_Leave()-Start\n");
			fflush(fp);
			fclose(fp);
		}

		if(m_wCurRound == eGTFight_2)
		{
			// 라운드 종료통보
			Notify_Send2AllUser(eGTNotify_RoundEnd, m_wCurRound);
			// 토너먼트 종료통보
			Notify_Send2AllUser(eGTNotify_TournamentEnd, m_wTournamentCount);

			GTInfo* pInfo = NULL;
			m_pEntryList.SetPositionHead();
			while((pInfo = m_pEntryList.GetData())!= NULL)
			{
				if(m_wCurRound < pInfo->wRound)
				{
					// 우승팀을 모든유저에게 알린다.
					SEND_GTWINLOSE msg;
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_NOTIFY_WINLOSE;
					msg.GuildIdx = pInfo->pGuild->GetIdx();
					SafeStrCpy(msg.GuildName, pInfo->pGuild->GetGuildName(), MAX_GUILD_NAME+1);
					msg.bWin = 1;
					msg.Param = eGTFight_1;
					msg.TournamentCount = m_wTournamentCount;
					g_Network.Broadcast2AgentServer((char*)&msg, sizeof(msg));

					// 보상을 준다.
					SendReward(eGTFight_1, pInfo->pGuild->GetIdx());
				}
				else if(m_wCurRound == pInfo->wRound)
				{
					// 준우승팀을 모든유저에게 알린다.
					SEND_GTWINLOSE msg;
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_NOTIFY_WINLOSE;
					msg.GuildIdx = pInfo->pGuild->GetIdx();
					SafeStrCpy(msg.GuildName, pInfo->pGuild->GetGuildName(), MAX_GUILD_NAME+1);
					msg.bWin = 0;
					msg.Param = eGTFight_1;
					msg.TournamentCount = m_wTournamentCount;
					g_Network.Broadcast2AgentServer((char*)&msg, sizeof(msg));

					// 보상을 준다.
					SendReward(eGTFight_2, pInfo->pGuild->GetIdx());
				}
			}

			// 결승경기면 토너먼트 종료
			ReadyToNewMatch();
			m_bStartGame = FALSE;
			m_bIgnoreSchedule = FALSE;
			m_wCurRound = eGTFight_End;
			m_wTournamentState = eGTState_BeforeRegist;
			m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
		}
		else
		{
			// 라운드 종료통보
			Notify_Send2AllUser(eGTNotify_RoundEnd, m_wCurRound);

			GTInfo* pInfo = NULL;
			m_pEntryList.SetPositionHead();
			while((pInfo = m_pEntryList.GetData())!= NULL)
			{
				// 승리팀만 일괄적으로 모든유저에게 알린다.
				if(m_wCurRound < pInfo->wRound)
				{
					SEND_GTWINLOSE msg;
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_NOTIFY_WINLOSE;
					msg.GuildIdx = pInfo->pGuild->GetIdx();
					SafeStrCpy(msg.GuildName, pInfo->pGuild->GetGuildName(), MAX_GUILD_NAME+1);
					msg.bWin = 1;
					msg.Param = m_wCurRound + 1;
					msg.TournamentCount = m_wTournamentCount;
					g_Network.Broadcast2AgentServer((char*)&msg, sizeof(msg));
				}
			}
			
			// 결승경기가 아니면 다음라운드 준비
			ReadyToNewMatch();
			m_wCurRound++;

			MakeBattleTable();	// 다음라운드 대진표생성
		}

		GTInfoUpdate(m_wTournamentCount, 0, eGTState_BeforeRegist, m_wCurRound);


		// 임시 문제발생시 로그남기기 : Process_Leave()
		fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			fprintf(fp, "Process_Leave()-End\n");
			fflush(fp);
			fclose(fp);
		}
	}
}

void CGuildTournamentMgr::MakeBattleTable()
{
	// 임시 문제발생시 로그남기기 : MakeBattleTable()
	FILE* fp = fopen("gtmap.log", "a+");
	if(fp)
	{
		char buf[256] = {0,};
		sprintf(buf, "MakeBattleTable()-Start : m_pEntryList(%d)\n", m_pEntryList.GetDataNum());
		fprintf(fp, buf);
		fflush(fp);
		fclose(fp);
	}

	DWORD dwBattleTable[MAXGUILD_INTOURNAMENT] = {0,};
	int i;
	GTInfo* pInfo;

	if(!m_bStartGame && !m_bShuffleBattleTable)
	{
		srand(GetTickCount());

		// 자동스케쥴러 첫라운드는 대진표셔플
		int nRand, nTeamNum=0;

		m_pEntryList.SetPositionHead();
		while((pInfo = m_pEntryList.GetData())!= NULL)
		{
			if(pInfo->wRound == m_wCurRound)
			{
				dwBattleTable[nTeamNum] = pInfo->pGuild->GetIdx();
				nTeamNum++;
			}
		}

		DWORD dwTemp;
		nTeamNum = m_GeneralInfo.wLimit_Team[1];
		for(i=0; i<nTeamNum; i++)
		{
			nRand = i + rand()%(nTeamNum-i);
			dwTemp = dwBattleTable[i];
			dwBattleTable[i] = dwBattleTable[nRand];
			dwBattleTable[nRand] = dwTemp;

			pInfo = m_pEntryList.GetData(dwBattleTable[nRand]);
			if(pInfo)
			{
				pInfo->Position = WORD( nRand );
				// DB update
				GTInfoUpdate(m_wTournamentCount, pInfo->pGuild->GetIdx(), pInfo->Position, pInfo->wRound);
			}

			pInfo = m_pEntryList.GetData(dwBattleTable[i]);
			if(pInfo)
			{
				pInfo->Position = WORD( i );
				// DB update
				GTInfoUpdate(m_wTournamentCount, pInfo->pGuild->GetIdx(), pInfo->Position, pInfo->wRound);
			}
		}
		m_bShuffleBattleTable = TRUE;

		// Fill BattleTable
		memcpy(m_InitBattleTable, dwBattleTable, sizeof(m_InitBattleTable));
		memcpy(m_CurBattleTable, dwBattleTable, sizeof(m_CurBattleTable));
	}
	else if(!m_bStartGame && m_bShuffleBattleTable)
	{
		// 강제등록 첫라운드는 대진표복사
		m_bShuffleBattleTable = TRUE;
		memcpy(m_CurBattleTable, m_InitBattleTable, sizeof(m_CurBattleTable));
	}
	else
	{
		// 두번째이후 라운드 대진표에서 추출
		for(i=0; i<MAXGUILD_INTOURNAMENT; i++)
		{
			pInfo = m_pEntryList.GetData(m_CurBattleTable[i]);
			if(pInfo && m_wCurRound==pInfo->wRound)
			{
				dwBattleTable[i/2] = m_CurBattleTable[i];
			}
		}
		
		// Fill BattleTable
		ZeroMemory(m_CurBattleTable, sizeof(m_CurBattleTable));
		memcpy(m_CurBattleTable, dwBattleTable, sizeof(m_CurBattleTable));
	}

	// 임시 문제발생시 로그남기기 : MakeBattleTable()
	fp = fopen("gtmap.log", "a+");
	if(fp)
	{
		fprintf(fp, "MakeBattleTable()-End : m_bStartGame(%d), m_bShuffleBattleTable(%d)\n", m_bStartGame, m_bShuffleBattleTable);
		fflush(fp);
		fclose(fp);
	}
}

void CGuildTournamentMgr::CreateBattle()
{
	// 임시 문제발생시 로그남기기 : CreateBattle()
	FILE* fp = fopen("gtmap.log", "a+");
	if(fp)
	{
		fprintf(fp, "CreateBattle()-Start\n");
		fflush(fp);
		fclose(fp);
	}

	memset(m_BattleInfo, 0, sizeof(m_BattleInfo));
	m_pEntryList.SetPositionHead();

	DWORD dwIdx_ATeam, dwIdx_BTeam;
	GTInfo *pInfo_ATeam, *pInfo_BTeam;

	int i, nInvaildMatch = 0;

	m_bNoMatchRound = FALSE;
	BOOL bHasMatch = FALSE;

	for(i=0; i<(MAXGUILD_INTOURNAMENT/2); i++)
	{
		dwIdx_ATeam = m_CurBattleTable[i*2];
		dwIdx_BTeam = m_CurBattleTable[i*2+1];

		pInfo_ATeam = m_pEntryList.GetData(dwIdx_ATeam);
		pInfo_BTeam = m_pEntryList.GetData(dwIdx_BTeam);

		if(!pInfo_ATeam && !pInfo_BTeam)
		{
			// 경기성립 안됨.
			nInvaildMatch++;
			continue;
		}
		else if(!pInfo_ATeam || !pInfo_BTeam)
		{
			// 부전승 발생
			NULL==pInfo_BTeam ? SetResult(pInfo_ATeam->pGuild->GetIdx(), TRUE, TRUE) : SetResult(pInfo_BTeam->pGuild->GetIdx(), TRUE, TRUE);
			m_wInTournamentTeam++;

			if(m_wCurRound == eGTFight_2)
			{
				m_bNoMatchRound = TRUE;
				return;
			}
			else
				continue;
		}

		m_BattleInfo[i].BattleKind = eBATTLE_KIND_GTOURNAMENT;
		m_BattleInfo[i].BattleState = eBATTLE_STATE_READY;
		m_BattleInfo[i].BattleTime = 0;
		m_BattleInfo[i].Ranking = 0;

		m_BattleInfo[i].m_dwEntranceTime = m_dwRemainTime;
		m_BattleInfo[i].m_dwFightTime = m_GeneralInfo.wPlayTime[0] * 60000;
		m_BattleInfo[i].m_dwLeaveTime = 15000;
		
		m_BattleInfo[i].MemberInfo.TeamGuildIdx[0] = pInfo_ATeam->pGuild->GetIdx();
		m_BattleInfo[i].MemberInfo.TeamGuildIdx[1] = pInfo_BTeam->pGuild->GetIdx();

		if(BATTLESYSTEM->CreateBattle(&m_BattleInfo[i], GTMAPNUM))
		{
			pInfo_ATeam->pGuild->SetGTBattleID(m_BattleInfo[i].BattleID);
			pInfo_BTeam->pGuild->SetGTBattleID(m_BattleInfo[i].BattleID);

			m_wCurBattleCount++;
			m_wInTournamentTeam += 2;

			bHasMatch = TRUE;
		}
	}

	m_bNoMatchRound = !bHasMatch;

	// 임시 문제발생시 로그남기기 : CreateBattle()
	fp = fopen("gtmap.log", "a+");
	if(fp)
	{
		fprintf(fp, "CreateBattle()-End\n");
		fflush(fp);
		fclose(fp);
	}
}

void CGuildTournamentMgr::StartTournament()
{
	// 임시 문제발생시 로그남기기 : StartTournament()
	FILE* fp = fopen("gtmap.log", "a+");
	if(fp)
	{
		fprintf(fp, "StartTournament()-Start\n");
		fflush(fp);
		fclose(fp);
	}

	int i;
	for(i=0; i<MAXGUILD_INTOURNAMENT; i++)
	{
		if(m_BattleInfo[i].BattleID == 0)
			continue;

		CBattle_GTournament* pBattle = (CBattle_GTournament*)BATTLESYSTEM->GetBattle(m_BattleInfo[i].BattleID);
		if(!pBattle)	continue;

		pBattle->StartBattle();
	}

	// 임시 문제발생시 로그남기기 : StartTournament()
	fp = fopen("gtmap.log", "a+");
	if(fp)
	{
		fprintf(fp, "StartTournament()-End\n");
		fflush(fp);
		fclose(fp);
	}
}

void CGuildTournamentMgr::PlayerRegist(CGuild* pGuild, DWORD* pPlayerList)
{
	if(!pGuild)		return;

	GTInfo* pInfo = m_pEntryList.GetData(pGuild->GetIdx());
	if(pInfo)
	{
		pInfo->ClearPlayerList();

		int i;
		for(i=0; i<MAX_GTOURNAMENT_PLAYER; i++)
		{
			if(pPlayerList[i])
			{
				GTPlayerInfo* pPlayerInfo = new GTPlayerInfo;
				if(pPlayerInfo)
				{
					pPlayerInfo->dwPlayerID = pPlayerList[i];
					pInfo->PlayerList.Add(pPlayerInfo, pPlayerInfo->dwPlayerID);
				}
			}
		}
	}
}

DWORD CGuildTournamentMgr::RegistGuild( CGuild* pGuild)
{
	if(m_wTournamentState != eGTState_Regist)
		return eGTError_NOTREGISTDAY;

	if(!pGuild)
		return eGTError_DONTFINDGUILDINFO;

	if(pGuild->GetLevel() < m_GeneralInfo.wLimit_Guild[0])
		return eGTError_UNDERLEVEL;

	if(pGuild->GetMemberNum() < m_GeneralInfo.wLimit_Guild[1])
		return eGTError_UNDERLIMITEMEMBER;

	if(m_GeneralInfo.wLimit_Team[1] <= m_pEntryList.GetDataNum())
	{
		// 임시 문제발생시 로그남기기 : 등록제한팀초과
		FILE* fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			char buf[256] = {0,};
			sprintf(buf, "GTLimitTeam is Over : Limit(%d), CurrentRegisted(%d)\n", m_GeneralInfo.wLimit_Team[1], m_pEntryList.GetDataNum());
			fprintf(fp, buf);
			fflush(fp);
			fclose(fp);
		}

		return eGTError_MAXGUILDOVER;
	}

	m_pEntryList.SetPositionHead();
	GTInfo* pInfo;
	while((pInfo = m_pEntryList.GetData())!= NULL)
	{
		if(pInfo->pGuild->GetIdx() == pGuild->GetIdx())
			return eGTError_ALREADYREGISTE;
	}

	GTInfo* pAddInfo = new GTInfo;
	if(!pAddInfo)
		return eGTError_FAILTOREGIST;

	pAddInfo->pGuild = pGuild;
	pAddInfo->wRound = m_wCurRound;
	m_pEntryList.Add(pAddInfo, pGuild->GetIdx());

	GTInfoInsert(m_wTournamentCount, pGuild->GetIdx(), pGuild->GetGuildName(), 34, m_wCurRound);

	// 임시 문제발생시 로그남기기 : 엔트리추가
	FILE* fp = fopen("gtmap.log", "a+");
	if(fp)
	{
		char buf[256] = {0,};
		sprintf(buf, "AddEntry : GuildIdx(%d), m_pEntryList(%d)\n", pGuild->GetIdx(), m_pEntryList.GetDataNum());
		fprintf(fp, buf);
		fflush(fp);
		fclose(fp);
	}

	return eGTError_NOERROR;
}


DWORD CGuildTournamentMgr::ForceRegistGuild( CGuild* pGuild, DWORD Round, DWORD Position )
{
	BOOL bRegToEntryList = FALSE;	// 대진표생성을 따르게 할것인가?
	if(Round==99 && Position==99)
		bRegToEntryList = TRUE;

	if(!pGuild)
		return eGTError_DONTFINDGUILDINFO;

	if(!bRegToEntryList && (Position<0 || MAXGUILD_INTOURNAMENT<Position))
		return eGTError_ERROR;

	if(!bRegToEntryList && m_InitBattleTable[Position])
		return eGTError_ALREADYREGISTE;

	GTInfo* pInfo = m_pEntryList.GetData(pGuild->GetIdx());
	if(pInfo)
		return eGTError_ALREADYREGISTE;


	GTInfo* pAddInfo = new GTInfo;
	if(!pAddInfo)
		return eGTError_FAILTOREGIST;

	if(bRegToEntryList)
	{
		pAddInfo->pGuild = pGuild;
		pAddInfo->wRound = m_wCurRound;
		m_pEntryList.Add(pAddInfo, pAddInfo->pGuild->GetIdx());

		GTInfoInsert(m_wTournamentCount, pGuild->GetIdx(), pGuild->GetGuildName(), 34, m_wCurRound);
	}
	else
	{
		pAddInfo->pGuild = pGuild;
		pAddInfo->wRound = (WORD)Round;
		pAddInfo->Position = (WORD)Position;
		m_pEntryList.Add(pAddInfo, pGuild->GetIdx());

		GTInfoInsert(m_wTournamentCount, pGuild->GetIdx(), pGuild->GetGuildName(), (WORD)Position, m_wCurRound);
			
		m_InitBattleTable[Position] = pGuild->GetIdx();
		m_CurBattleTable[Position] = pGuild->GetIdx();

		if(m_wCurRound < (WORD)Round)
			m_wCurRound = (WORD)Round;
	}

	m_bShuffleBattleTable = TRUE;

	//////////////////////////////////////////////
	// 임시로 강제등록시 길드마스터만 선수로 등록
	DWORD dwPlayers[MAX_GTOURNAMENT_PLAYER] = {0,};
	dwPlayers[0] = pGuild->GetMasterIdx();
	PlayerRegist(pGuild, dwPlayers);
	//////////////////////////////////////////////

	return eGTError_NOERROR;
}

void CGuildTournamentMgr::MoveToBattleMap_Syn(DWORD dwConnectionIndex, void* pMsg)
{
	if(g_pServerSystem->GetMapNum() != GTMAPNUM)
	{
		MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
		if(!pPlayer)	return;

		MSG_DWORD5 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_SYN;
		msg.dwData1 = g_pServerSystem->GetMapNum();
		msg.dwData2 = pmsg->dwObjectID;
		msg.dwData3 = pmsg->dwData;	// BattleID
		msg.dwData4 = pPlayer->GetGuildIdx();
		msg.dwData5 = pPlayer->GetUserLevel();
		PACKEDDATA_OBJ->SendToMapServer(GTMAPNUM, &msg, sizeof(msg));
		return;
	}

	//081223 NYJ - 입장실패 로그 남김.
	SYSTEMTIME time;
	char szFile[256] = {0,};
	char szError[256] = {0,};
	GetLocalTime( &time );

	sprintf(szFile, "./Log/GTLog_EnterFail_%04d%02d%02d.txt", time.wYear, time.wMonth, time.wDay );
	FILE* fp;
	fp = fopen(szFile, "a+");

	MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;

	DWORD dwPlayerIdx = pmsg->dwData2;
	DWORD dwGuildIdx = pmsg->dwData4;

	if(m_wTournamentState!=eGTState_Entrance && m_wTournamentState!=eGTState_Process)
	{
		MSG_DWORD2 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK;
		msg.dwData1 = pmsg->dwData2;
		msg.dwData2 = eGTError_NOTENTERTIME;
		PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
		
		sprintf(szError, "eGTError_NOTENTERTIME :: PID[%d], GID[%d] (m_wTournamentState:%d) (Line:%d)\n", dwPlayerIdx, dwGuildIdx, m_wTournamentState, __LINE__);
		goto MOVETOBATTLE_FAIL;
	}

	int nBattleID = pmsg->dwData3;
	if(nBattleID != 0)
	{
		BOOL bEntrance = m_GeneralInfo.bObAllow;
		if(pmsg->dwData5 <= eUSERLEVEL_GM)	// GM 이상이면 입장
			bEntrance = TRUE;

		//옵저버
		if(!bEntrance)
		{
			MSG_DWORD2 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK;
			msg.dwData1 = pmsg->dwData2;
			msg.dwData2 = eGTError_NOTALLOWOBSERVER;
			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
			
			sprintf(szError, "eGTError_NOTALLOWOBSERVER :: PID[%d], GID[%d] (nBattleID:%d) (Line:%d)\n", dwPlayerIdx, dwGuildIdx, nBattleID, __LINE__);
			goto MOVETOBATTLE_FAIL;
		}
		else
		{
			CBattle_GTournament* pBattle = (CBattle_GTournament*)BATTLESYSTEM->GetBattle(nBattleID);
			if(pBattle)
			{
				MSG_DWORD2 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_ACK;
				msg.dwData1 = pmsg->dwData2;
				msg.dwData2 = nBattleID;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));

				if(fp)
					fclose(fp);
				return;
			}

			MSG_DWORD2 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK;
			msg.dwData1 = pmsg->dwData2;
			msg.dwData2 = eGTError_DONTFINDBATTLE;
			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
			
			sprintf(szError, "eGTError_DONTFINDBATTLE :: PID[%d], GID[%d] (nBattleID:%d) (Line:%d)\n", dwPlayerIdx, dwGuildIdx, nBattleID, __LINE__);
			goto MOVETOBATTLE_FAIL;
		}
	}
	else
	{
		DWORD dwGuildIdx = pmsg->dwData4;
		GTInfo* pInfo = m_pEntryList.GetData(dwGuildIdx);

		if(pInfo)
		{
			if(m_wCurRound < pInfo->wRound)
			{
				// 다음라운드 진출판결이 났으면(부전승등...) 진입안시킴.
				MSG_DWORD2 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK;
				msg.dwData1 = pmsg->dwData2;
				msg.dwData2 = eGTError_READYTONEXTMATCH;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
				
				sprintf(szError, "eGTError_READYTONEXTMATCH :: PID[%d], GID[%d] (m_wCurRound:%d) (nextRount:%d) (Line:%d)\n", dwPlayerIdx, dwGuildIdx, m_wCurRound, pInfo->wRound, __LINE__);
				goto MOVETOBATTLE_FAIL;
			}

			nBattleID = GetBattleID(pInfo->pGuild);
			if(nBattleID == -1)
			{
				MSG_DWORD2 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK;
				msg.dwData1 = pmsg->dwData2;
				msg.dwData2 = eGTError_DONTFINDBATTLE;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
				
				sprintf(szError, "eGTError_DONTFINDBATTLE :: PID[%d], GID[%d] (nBattleID:%d) (Line:%d)\n", dwPlayerIdx, dwGuildIdx, nBattleID, __LINE__);
				goto MOVETOBATTLE_FAIL;
			}

			// 090813 ONS 무승부나 부전패의 경우 경기가 끝날때까지 맵이동 금지.
			if( m_wTournamentState == eGTState_Process	&&
				m_wCurRound >= pInfo->wRound			)
			{
				MSG_DWORD2 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK;
				msg.dwData1 = pmsg->dwData2;
				msg.dwData2 = eGTError_NOTENTERTIME;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
				
				sprintf(szError, "eGTError_NOTENTERTIME :: PID[%d], GID[%d] (m_wTournamentState:%d) (Line:%d)\n", dwPlayerIdx, dwGuildIdx, m_wTournamentState, __LINE__);
				goto MOVETOBATTLE_FAIL;
			}


			// 등록된길드
			GTPlayerInfo* pPlayerInfo = pInfo->PlayerList.GetData(pmsg->dwData2);
			if(pPlayerInfo)
			{
				// 선수리스트에 있으면 선수로 입장
				MSG_DWORD2 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_ACK;
				msg.dwData1 = pmsg->dwData2;
				msg.dwData2 = 0;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
			}
			else
			{
				MSG_DWORD2 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK;
				msg.dwData1 = pmsg->dwData2;
				msg.dwData2 = eGTError_DONTFINDBATTLE;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));

				sprintf(szError, "eGTError_NOTPLAYER :: PID[%d], GID[%d] (Line:%d)\n",  dwPlayerIdx, dwGuildIdx, __LINE__);
				goto MOVETOBATTLE_FAIL;
			}
		}
		else
		{
			MSG_DWORD2 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK;
			msg.dwData1 = pmsg->dwData2;
			msg.dwData2 = eGTError_DONTFINDBATTLE;
			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
			
			sprintf(szError, "eGTError_NOTREGGUILD :: PID[%d], GID[%d] (Line:%d)\n", dwPlayerIdx, dwGuildIdx, __LINE__);
			goto MOVETOBATTLE_FAIL;
		}
	}

	if(fp)
		fclose(fp);

MOVETOBATTLE_FAIL:
	if(fp)
	{
		fprintf(fp, szError);
		fflush(fp);
		fclose(fp);
	}
}

void CGuildTournamentMgr::NetworkMsgParse( DWORD dwConnectionIndex, BYTE Protocol,void* pMsg )
{
	switch(Protocol)
	{
	case MP_GTOURNAMENT_REGIST_SYN:
		{
			if(g_pServerSystem->GetMapNum() != GTMAPNUM)
			{
				MSGBASE* pmsg = (MSGBASE*)pMsg;
				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
				if(!pPlayer)	return;

				if(pPlayer->GetGuildMemberRank() != GUILD_MASTER)
				{
					MSG_DWORD msg;
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_REGIST_NACK;
					msg.dwData = eGTError_NOGUILDMASTER;
					pPlayer->SendMsg(&msg, sizeof(msg));
					return;
				}

				MSG_DWORD3 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_REGIST_SYN;
				msg.dwData1 = g_pServerSystem->GetMapNum();
				msg.dwData2 = pmsg->dwObjectID;
				msg.dwData3 = pPlayer->GetGuildIdx();
				PACKEDDATA_OBJ->SendToMapServer(GTMAPNUM, &msg, sizeof(msg));
			}

			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			CGuild* pGuild = GUILDMGR->GetGuild(pmsg->dwData3);

			DWORD dwResult = RegistGuild(pGuild);
			if(eGTError_NOERROR != dwResult)
			{
				MSG_DWORD2 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_REGIST_NACK;
				msg.dwData1 = pmsg->dwData2;
				msg.dwData2 = dwResult;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
				return;
			}

			// 길드포인트와 money는 DB서버에서 처리한다.
			// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
			g_DB.FreeMiddleQuery(
			RGTCheckRegist,
			pmsg->dwData2,
			"EXEC dbo.MP_GT_CHECK_REGIST %d, %d, %d, %d, %d, %d",
			pmsg->dwData2,
			pmsg->dwData1,
			dwConnectionIndex,
			pGuild->GetIdx(),
			m_GeneralInfo.dwFee[0],
			m_GeneralInfo.dwFee[1]
			);
		}
		break;

	case MP_GTOURNAMENT_REGIST_ACK:
		{
			MSG_NAME_DWORD5* pmsg = (MSG_NAME_DWORD5*)pMsg;

			CGuild* pGuild = GUILDMGR->GetGuild(pmsg->dwData5);
			if(pGuild)
			{
				// 길드포인트 새로고침
				GuildAddScore( pmsg->dwData5, 0 );
				InsertLogGuildScore(pmsg->dwData5, -1.0f*pmsg->dwData3, eGuildLog_ScoreRemoveByGTFee );

				MSGBASE message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_SCORE_UPDATE_TO_MAP;
				message.dwObjectID	= pmsg->dwData5;

				g_Network.Broadcast2AgentServer( ( char* )&message, sizeof( message ) );

				
				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwData1);
				if(!pPlayer)	return;

				// 소지금 새로 고침
				pPlayer->SetMoney(
				pmsg->dwData4,
				MONEY_SUBTRACTION,
				MF_FEE );

				LogItemMoney(
					pPlayer->GetID(),
					pPlayer->GetObjectName(),
					pmsg->dwData2,
					"*GT_Reg",
					eLog_GTournamentReg,
					pPlayer->GetMoney() + pmsg->dwData4,
					pPlayer->GetMoney(),
					pmsg->dwData4,
					0,
					0,
					0,
					0,
					0,
					0 );


				if(pGuild->GetMasterIdx() != pmsg->dwData1)
				{
					CPlayer* pGuildMaster = (CPlayer*)g_pUserTable->FindUser(pGuild->GetMasterIdx());
					if(!pGuildMaster)	return;

					MSG_NAME_DWORD4 msg;
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_REGIST_ACK;
					SafeStrCpy(msg.Name, pGuild->GetGuildName(), sizeof(msg.Name));
					msg.dwData1 = pmsg->dwData2;
					msg.dwData2 = pmsg->dwData3;
					msg.dwData3 = pmsg->dwData4;
					msg.dwData4 = pmsg->dwData5;
					pGuildMaster->SendMsg(&msg, sizeof(msg));
				}
				else
				{
					MSG_NAME_DWORD4 msg;
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_REGIST_ACK;
					SafeStrCpy(msg.Name, pGuild->GetGuildName(), sizeof(msg.Name));
					msg.dwData1 = pmsg->dwData2;
					msg.dwData2 = pmsg->dwData3;
					msg.dwData3 = pmsg->dwData4;
					msg.dwData4 = pmsg->dwData5;
					pPlayer->SendMsg(&msg, sizeof(msg));
				}
			}
		}
		break;

	case MP_GTOURNAMENT_REGIST_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwData1);
			if(!pPlayer)	return;

			MSG_DWORD msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_REGIST_NACK;
			msg.dwData = pmsg->dwData2;
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
		break;

	case MP_GTOURNAMENT_REGISTPLAYER_SYN:
		{
			if(g_pServerSystem->GetMapNum() != GTMAPNUM)
			{
				MSGBASE* pmsg = (MSGBASE*)pMsg;

				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
				if(!pPlayer)	return;

				if(GUILD_MASTER != pPlayer->GetGuildMemberRank())
				{
					MSG_DWORD msg;
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_REGISTPLAYER_NACK;
					msg.dwData = eGTError_NOGUILDMASTER;
					pPlayer->SendMsg(&msg, sizeof(msg));
					return;
				}

				MSG_DWORD3 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_REGISTPLAYER_SYN;
				msg.dwData1 = g_pServerSystem->GetMapNum();
				msg.dwData2 = pmsg->dwObjectID;
				msg.dwData3 = pPlayer->GetGuildIdx();
				PACKEDDATA_OBJ->SendToMapServer(GTMAPNUM, &msg, sizeof(msg));
				return;
			}

			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			DWORD dwError = eGTError_NOERROR;

			if(m_wTournamentState!=eGTState_Regist || m_bNeedAdjust)
				dwError = eGTError_NOTREGISTDAY;

			GTInfo* pInfo = m_pEntryList.GetData(pmsg->dwData3);
			if(!pInfo)
				dwError = eGTError_DONTFINDGUILDINFO;

			if(dwError != eGTError_NOERROR)
			{
				MSG_DWORD msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_REGISTPLAYER_NACK;
				msg.dwObjectID = pmsg->dwData2;
				msg.dwData = dwError;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
				return;
			}

			int nCnt = 0;
			MSG_GTEDIT_PLAYERS msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_REGISTPLAYER_ACK;
			msg.dwObjectID = pmsg->dwData2;
			
			GTPlayerInfo* pPlayerInfo = NULL;
			pInfo->PlayerList.SetPositionHead();
			while((pPlayerInfo = pInfo->PlayerList.GetData())!= NULL)
			{
				if(MAX_GTOURNAMENT_PLAYER<=nCnt)
				{
					// 임시 문제발생시 로그남기기 : 등록인원초과
					FILE* fp = fopen("gtmap.log", "a+");
					if(fp)
					{
						char buf[256] = {0,};
						sprintf(buf, "GTPlayer is Over : %d\n", pInfo->PlayerList.GetDataNum());
						fprintf(fp, buf);
						fflush(fp);
						fclose(fp);
						break;
					}
				}

				msg.dwPlayers[nCnt] = pPlayerInfo->dwPlayerID;
				nCnt++;
			}

			msg.dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryEnd);

			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
		}
		break;

	case MP_GTOURNAMENT_REGISTPLAYER_ACK:
		{
			MSG_GTEDIT_PLAYERS* pmsg = (MSG_GTEDIT_PLAYERS*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
				pPlayer->SendMsg(pmsg, sizeof(MSG_GTEDIT_PLAYERS));
		}
		break;

	case MP_GTOURNAMENT_REGISTPLAYER_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
				pPlayer->SendMsg(pmsg, sizeof(MSG_DWORD));
		}
		break;

	case MP_GTOURNAMENT_EDITPLAER_SYN:
		{
			if(g_pServerSystem->GetMapNum() != GTMAPNUM)
			{
				MSG_GTEDIT_PLAYERS* pmsg = (MSG_GTEDIT_PLAYERS*)pMsg;
				
				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
				if(!pPlayer)	return;

				MSG_GTEDIT_PLAYERS_DWORD3 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_EDITPLAER_SYN;
				memcpy(msg.dwPlayers, pmsg->dwPlayers, sizeof(msg.dwPlayers));
				msg.dwData1 = g_pServerSystem->GetMapNum();
				msg.dwData2 = pmsg->dwObjectID;
				msg.dwData3 = pPlayer->GetGuildIdx();
				PACKEDDATA_OBJ->SendToMapServer(GTMAPNUM, &msg, sizeof(msg));
				return;
			}

			MSG_GTEDIT_PLAYERS_DWORD3* pmsg = (MSG_GTEDIT_PLAYERS_DWORD3*)pMsg;
			DWORD dwError = eGTError_NOERROR;

			if(m_wTournamentState != eGTState_Regist)
				dwError = eGTError_NOTREGISTDAY;

			GTInfo* pInfo = m_pEntryList.GetData(pmsg->dwData3);
			if(!pInfo)
				dwError = eGTError_DONTFINDGUILDINFO;

			pInfo->ClearPlayerList();
			int i;
			for(i=0; i<MAX_GTOURNAMENT_PLAYER; i++)
			{
				if(pmsg->dwPlayers[i])
				{
					GTPlayerInfo* pPlayerInfo = new GTPlayerInfo;
					if(pPlayerInfo)
					{
						pPlayerInfo->dwPlayerID = pmsg->dwPlayers[i];
						pInfo->PlayerList.Add(pPlayerInfo, pPlayerInfo->dwPlayerID);
					}
				}
			}

			if(dwError != eGTError_NOERROR)
			{
				MSG_DWORD msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_EDITPLAYER_NACK;
				msg.dwObjectID = pmsg->dwData2;
				msg.dwData = dwError;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
				return;
			}

			GTPlayerInsert(m_wTournamentCount, pmsg->dwData3, pmsg->dwPlayers);

			MSG_DWORD msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_EDITPLAYER_ACK;
			msg.dwObjectID = pmsg->dwData2;
			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
		}
		break;

	case MP_GTOURNAMENT_EDITPLAYER_ACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
				pPlayer->SendMsg(pmsg, sizeof(MSG_DWORD));
		}
		break;

	case MP_GTOURNAMENT_EDITPLAYER_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
				pPlayer->SendMsg(pmsg, sizeof(MSG_DWORD));
		}
		break;

	case MP_GTOURNAMENT_MOVETOBATTLEMAP_SYN:
		{
			MoveToBattleMap_Syn(dwConnectionIndex, pMsg);
		}
		break;
	case MP_GTOURNAMENT_MOVETOBATTLEMAP_ACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwData1);
			if(!pPlayer)	return;

			PARTYMGR->DelMemberSyn(pPlayer->GetID(), pPlayer->GetPartyIdx());
			PETMGR->SealPet(
				PETMGR->GetPet(pPlayer->GetPetItemDbIndex()));

			pPlayer->UpdateLogoutToDB(FALSE);
			g_pServerSystem->RemovePlayer(pPlayer->GetID());

			MSG_DWORD msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_ACK;
			msg.dwData = pmsg->dwData2;
			pPlayer->SendMsg(&msg, sizeof(msg));

			// 081027 LUJ, 입장 로그
			LogGuild(
				pPlayer->GetID(),
				pPlayer->GetGuildIdx(),
				eGuildLog_TournamentAdd,
				GTMGR->GetCurRound(),
				0 );
		}
		break;
	case MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwData1);
			if(!pPlayer)	return;

			MSG_DWORD msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK;
			msg.dwData = pmsg->dwData2;
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
		break;
	case MP_GTOURNAMENT_OBSERVERJOIN_SYN:
		{
			SEND_BATTLEJOIN_INFO* pmsg = (SEND_BATTLEJOIN_INFO*)pMsg;

			CObject* pObject = g_pUserTable->FindUser(pmsg->dwObjectID);				
			if( pObject != NULL )
			{
				if( pObject->GetObjectKind() == eObjectKind_Player )
				{
					MSG_DWORD msg;
					SetProtocol( &msg, MP_GTOURNAMENT, MP_USERCONN_GAMEIN_NACK );
					msg.dwData		= pmsg->dwObjectID;					
					
					g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );					
					return;
				}
			}

			CPlayer* pPlayer = g_pServerSystem->AddPlayer(
				pmsg->dwObjectID,
				dwConnectionIndex,
				pmsg->AgentIdx,
				0,
				eUSERLEVEL(pmsg->UserLevel));

			if(!pPlayer) return;

			FishingData_Load(pmsg->dwObjectID);
			CookingData_Load(pmsg->dwObjectID);
			Cooking_Recipe_Load(pmsg->dwObjectID);
			HouseData_Load(pmsg->dwObjectID);
			
			CharacterNumSendAndCharacterInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
			CharacterSkillTreeInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
			CharacterItemInfo( pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN );


			/// 단축창 정보
			QuickInfo( pmsg->dwObjectID );
			// quest 정보 qurey 추가
			QUESTMGR->CreateQuestForPlayer( pPlayer );
			//퀘스트정보를 받아오도록 한다.
			QuestTotalInfo( pmsg->dwObjectID );	
			//농장 및 기타정보를 받아오도록 한다.
			Farm_LoadTimeDelay( pmsg->dwObjectID );	

			// desc_hseos_데이트 존_01
			// S 데이트 존 추가 added by hseos 2007.11.27	2007.11.28
			// ..함수 순서를 지키는 것이 좋음
			// ..챌린지 존 시작
			g_csDateManager.SRV_StartChallengeZone(pPlayer, pmsg->AgentIdx, 0);
			// ..챌린지 존 입장 회수 로드
			ChallengeZone_EnterFreq_Load(pPlayer->GetID());

			pPlayer->SetGuildIdx( pmsg->GuildIdx );
			pPlayer->SetReturnMapNum( (WORD)pmsg->ReturnMapNum );
			pPlayer->SetObserverBattleIdx( pmsg->BattleIdx );

			CBattle_GTournament* pBattle = (CBattle_GTournament*)BATTLESYSTEM->GetBattle( pmsg->BattleIdx );
			if( !pBattle )
			{
				MSG_DWORD msg;
				SetProtocol( &msg, MP_GTOURNAMENT, MP_GTOURNAMENT_RETURNTOMAP );
				msg.dwData = pmsg->ReturnMapNum;
				pPlayer->SendMsg( &msg, sizeof(msg) );
				return;
			}
						
			pBattle->AddObserverToBattle(pPlayer);
		}
		break;
	case MP_GTOURNAMENT_BATTLEJOIN_SYN:
		{
			SEND_BATTLEJOIN_INFO* pmsg = (SEND_BATTLEJOIN_INFO*)pMsg;

			CObject* pObject = g_pUserTable->FindUser(pmsg->dwObjectID);				
			if( pObject != NULL )
			{
				if( pObject->GetObjectKind() == eObjectKind_Player )
				{
					MSG_DWORD msg;
					SetProtocol( &msg, MP_GTOURNAMENT, MP_USERCONN_GAMEIN_NACK );
					msg.dwData		= pmsg->dwObjectID;					
					
					g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );					
					return;
				}
			}

			CPlayer* pPlayer = g_pServerSystem->AddPlayer(
				pmsg->dwObjectID,
				dwConnectionIndex,
				pmsg->AgentIdx,
				0,
				eUSERLEVEL(pmsg->UserLevel));

			if(!pPlayer) return;

			FishingData_Load(pmsg->dwObjectID);
			CookingData_Load(pmsg->dwObjectID);
			Cooking_Recipe_Load(pmsg->dwObjectID);
			HouseData_Load(pmsg->dwObjectID);
			
			CharacterNumSendAndCharacterInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
			CharacterSkillTreeInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
			CharacterItemInfo( pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN );

			/// 단축창 정보
			QuickInfo( pmsg->dwObjectID );
			// quest 정보 qurey 추가
			QUESTMGR->CreateQuestForPlayer( pPlayer );
			//퀘스트정보를 받아오도록 한다.
			QuestTotalInfo( pmsg->dwObjectID );	
			//농장 및 기타정보를 받아오도록 한다.
			Farm_LoadTimeDelay( pmsg->dwObjectID );	

			// desc_hseos_데이트 존_01
			// S 데이트 존 추가 added by hseos 2007.11.27	2007.11.28
			// ..함수 순서를 지키는 것이 좋음
			// ..챌린지 존 시작
			g_csDateManager.SRV_StartChallengeZone(pPlayer, pmsg->AgentIdx, 0);
			// ..챌린지 존 입장 회수 로드
			ChallengeZone_EnterFreq_Load(pPlayer->GetID());

			pPlayer->SetGuildIdx( pmsg->GuildIdx );
			pPlayer->SetReturnMapNum( (WORD)pmsg->ReturnMapNum );
			pPlayer->SetObserverBattleIdx( 0 );


			CGuild* pGuild = GUILDMGR->GetGuild( pmsg->GuildIdx );	
			int BattleId = GetBattleID( pGuild );
			if( BattleId == -1 )		return;

			CBattle_GTournament* pBattle = (CBattle_GTournament*)BATTLESYSTEM->GetBattle( BattleId );
			if( !pBattle )
			{
				MSG_DWORD msg;
				SetProtocol( &msg, MP_GTOURNAMENT, MP_GTOURNAMENT_RETURNTOMAP );
				msg.dwData = pmsg->ReturnMapNum;
				pPlayer->SendMsg( &msg, sizeof(msg) );
				return;
			}
						
			pBattle->AddObjectToBattle(pPlayer);
		}
		break;
	case MP_GTOURNAMENT_LEAVE_SYN:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
						
			CObject* pObject = g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pObject != NULL )
			{
				if( pObject->GetObjectKind() == eObjectKind_Player )
				{
					MSG_DWORD msg;
					SetProtocol( &msg, MP_GTOURNAMENT, MP_USERCONN_GAMEIN_NACK );
					msg.dwData	= pmsg->dwObjectID;					
					g_Network.Send2Server( dwConnectionIndex, (char*)&msg, sizeof(msg) );
					return;
				}
			}
			
			CPlayer* pPlayer = g_pServerSystem->AddPlayer(
				pmsg->dwObjectID,
				dwConnectionIndex,
				pmsg->dwData1,
				0,
				eUSERLEVEL(pmsg->dwData2));

			if(!pPlayer) return;

			FishingData_Load(pmsg->dwObjectID);
			CookingData_Load(pmsg->dwObjectID);
			Cooking_Recipe_Load(pmsg->dwObjectID);
			HouseData_Load(pmsg->dwObjectID);
			
			CharacterNumSendAndCharacterInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
			CharacterSkillTreeInfo(pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN);
			CharacterItemInfo( pmsg->dwObjectID, MP_USERCONN_GAMEIN_SYN );
			
			/// 단축창 정보
			QuickInfo( pmsg->dwObjectID );
			// quest 정보 qurey 추가
			QUESTMGR->CreateQuestForPlayer( pPlayer );
			//퀘스트정보를 받아오도록 한다.
			QuestTotalInfo( pmsg->dwObjectID );				//QuestMainQuestLoad(), QuestSubQuestLoad(), QuestItemLoad(), TutorialLoad()이 연계된다.
			//농장 및 기타정보를 받아오도록 한다.
			Farm_LoadTimeDelay( pmsg->dwObjectID );			//MostserMeter_Load(), ResidentRegist_LoadInfo()가 연계된다.




			// desc_hseos_데이트 존_01
			// S 데이트 존 추가 added by hseos 2007.11.27	2007.11.28
			// ..함수 순서를 지키는 것이 좋음
			// ..챌린지 존 시작
			g_csDateManager.SRV_StartChallengeZone(pPlayer, pmsg->dwData1, 0);
			// ..챌린지 존 입장 회수 로드
			ChallengeZone_EnterFreq_Load(pPlayer->GetID());
		}
		break;


	case MP_GTOURNAMENT_FORCEREG:
		{
			if(g_pServerSystem->GetMapNum() != GTMAPNUM)
			{
				MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
				if(!pPlayer)	return;

				MSG_DWORD5 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_FORCEREG;
				msg.dwData1 = g_pServerSystem->GetMapNum();
				msg.dwData2 = pmsg->dwObjectID;
				msg.dwData3 = pmsg->dwData1;
				msg.dwData4 = pmsg->dwData2;
				msg.dwData5 = pmsg->dwData3;
				PACKEDDATA_OBJ->SendToMapServer(GTMAPNUM, &msg, sizeof(msg));
				return;
			}

			MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;

			CGuild* pGuild = GUILDMGR->GetGuildFromMasterIdx(pmsg->dwData3);
			if(!pGuild)	return;

			DWORD dwRes = ForceRegistGuild(pGuild, pmsg->dwData4, pmsg->dwData5);
			if(dwRes != eGTError_NOERROR)
			{
				MSG_DWORD2 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_FORCEREG_NACK;
				msg.dwData1 = pmsg->dwData2;
				msg.dwData2 = dwRes;
				PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
				return;
			}

			// 길드포인트와 money는 DB서버에서 처리한다.
			// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
			g_DB.FreeMiddleQuery(
			RGTCheckRegist,
			pmsg->dwData2,
			"EXEC dbo.MP_GT_CHECK_REGIST %d, %d, %d, %d, %d, %d",
			pmsg->dwData2,
			pmsg->dwData1,
			dwConnectionIndex,
			pGuild->GetIdx(),
			m_GeneralInfo.dwFee[0],
			m_GeneralInfo.dwFee[1]
			);
		}
		break;
	case MP_GTOURNAMENT_FORCEREG_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwData1);
			if(!pPlayer)	return;

			MSG_DWORD msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_FORCEREG_NACK;
			msg.dwData = pmsg->dwData2;
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
		break;
	case MP_GTOURNAMENT_STANDINGINFO_SYN:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			if(m_wTournamentState==eGTState_Regist || m_wTournamentCount==0 || m_bNeedAdjust)
			{
				MSG_DWORD2 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_STANDINGINFO_NACK;
				msg.dwObjectID = pmsg->dwData1;
				msg.dwData1 = pmsg->dwData2;
				msg.dwData2 = eGTError_INVALIDDAY;

				g_Network.Send2SpecificAgentServer((char*)&msg, sizeof(msg));
				return;
			}

			SEND_REGISTEDGUILDINFO msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_STANDINGINFO_ACK;
			msg.dwObjectID = pmsg->dwData1;
			msg.MaxTeam = (BYTE)m_GeneralInfo.wLimit_Team[1];
			msg.CurTournament = m_wTournamentCount;
			msg.CurRound = m_wCurRound;
			msg.CurState = m_wTournamentState;
			msg.PlayerID = pmsg->dwData2;
			msg.RemainTime = m_dwRemainTime;

			GTInfo* pInfo;
			m_pEntryList.SetPositionHead();
			
			int nCnt = 0;
			while((pInfo = m_pEntryList.GetData())!= NULL)
			{
				if(!pInfo->pGuild)	continue;

				msg.GInfo[nCnt].GuildIdx = pInfo->pGuild->GetIdx();
				SafeStrCpy(msg.GInfo[nCnt].GuildName, pInfo->pGuild->GetGuildName(), MAX_GUILD_NAME+1);
				msg.GInfo[nCnt].Position = (BYTE)pInfo->Position;
				msg.GInfo[nCnt].Ranking = 0;
				msg.GInfo[nCnt].ProcessTournament = (BYTE)pInfo->wRound;
				
				CGuildMark* pMark = GUILDMGR->GetMark(pInfo->pGuild->GetMarkName());
				if(pMark)
				{
					memcpy(msg.GInfo[nCnt].ImageData, pMark->GetImgData(), GUILDMARK_BUFSIZE);
					msg.GInfo[nCnt].MarkExist = 1;
				}
				else
					msg.GInfo[nCnt].MarkExist = 0;

				if(m_wTournamentState==eGTState_Entrance || m_wTournamentState==eGTState_Process)
				{
					msg.GInfo[nCnt].BattleID = pInfo->pGuild->GetGTBattleID();
				}
				else
					msg.GInfo[nCnt].BattleID = GT_INVALID_BATTLEID;

				nCnt++;
			}
			msg.Count = BYTE( nCnt );

			g_Network.Send2SpecificAgentServer((char*)&msg, msg.GetSize());
		}
		break;
	case MP_GTOURNAMENT_CHEAT_GETSTATE_SYN:
		{
			if(g_pServerSystem->GetMapNum() != GTMAPNUM)
			{
				MSGBASE* pmsg = (MSGBASE*)pMsg;

				MSG_DWORD2 msg;
				msg.Category = pmsg->Category;
				msg.Protocol = pmsg->Protocol;
				msg.dwData1 = g_pServerSystem->GetMapNum();
				msg.dwData2 = pmsg->dwObjectID;
				PACKEDDATA_OBJ->SendToMapServer(GTMAPNUM, &msg, sizeof(msg));
				return;
			}

			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			if(pmsg->dwData1==0 && pmsg->dwData2==0)
			{
				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
				if(!pPlayer)	return;

				MSG_DWORD2 msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_CHEAT_GETSTATE_ACK;
				msg.dwData1 = m_wTournamentState;
				msg.dwData2 = m_dwRemainTime;
				pPlayer->SendMsg(&msg, sizeof(msg));
			}
			
			MSG_DWORD2 msg;
			msg.Category = pmsg->Category;
			msg.Protocol = MP_GTOURNAMENT_CHEAT_GETSTATE_ACK;
			msg.dwObjectID = pmsg->dwData2;
			msg.dwData1 = m_wTournamentState;
			msg.dwData2 = m_dwRemainTime;
			PACKEDDATA_OBJ->SendToMapServer((MAPTYPE)pmsg->dwData1, &msg, sizeof(msg));
		}
		break;
	case MP_GTOURNAMENT_CHEAT_GETSTATE_ACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(!pPlayer)	return;

			MSG_DWORD2 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_CHEAT_GETSTATE_ACK;
			msg.dwData1 = pmsg->dwData1;
			msg.dwData2 = pmsg->dwData2;
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
		break;
	case MP_GTOURNAMENT_CHEAT_SETSTATE:
		{
			if(g_pServerSystem->GetMapNum() != GTMAPNUM)
			{
				PACKEDDATA_OBJ->SendToMapServer(GTMAPNUM, (MSGBASE*)pMsg, sizeof(MSG_DWORD));
				return;
			}
			
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			if(eGTState_Max < pmsg->dwData)
				return;

			SYSTEMTIME st;
			GetLocalTime(&st);

			m_bIgnoreSchedule = TRUE;
			m_wTournamentState = (WORD)pmsg->dwData;
			m_dwRemainTime = 60000 * 5;
			m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

			switch(m_wTournamentState)
			{
			case eGTState_BeforeRegist:
				{
					m_dwRemainTime = 0;
					m_bIgnoreSchedule = FALSE;
				}
				break;
			case eGTState_Regist:
				{
					ReadyToNewTournament();
					m_dwRemainTime = 60000 * 10;

					m_wTournamentCount++;
					m_wTournamentState = eGTState_Regist;
					Notify_Send2AllUser(eGTNotify_RegistStart, m_wTournamentCount);	// 신청시작 통보

					GTInfoInsert(m_wTournamentCount, 0, "*GTRegist(Manual)", eGTState_Regist, m_wCurRound);

					m_bIgnoreSchedule = FALSE;
				}
				break;
			case eGTState_BeforeEntrance:
				{
					Notify_Send2AllUser(eGTNotify_RegistEnd, m_wTournamentCount);	// 신청마감 통보
					MakeBattleTable();	// 신청마감후 바로 대진표생성
					Notify_Send2AllUser(eGTNotify_BattleTableOpen, m_wTournamentCount, m_dwRemainTime);	// 대진표공개 통보
				}
				break;
			case eGTState_Entrance:
				{
					if(!m_bStartGame)
					{
						Notify_Send2AllUser(eGTNotify_TournamentStart, m_wTournamentCount);	// 토너먼트 시작통보
						Notify_Send2AllGTPlayer(eGTNotify_RoundStartforPlayer, m_wCurRound, m_dwLastNotifyTime);// 라운드 시작통보 for 선수
						m_bStartGame = TRUE;
					}
					CreateBattle();
					Notify_Send2AllUser(eGTNotify_RoundStart, m_wCurRound, m_dwRemainTime);	// 라운드시작 통보
				}
				break;
			case eGTState_Process:
				{
					if(m_bStartGame)
						Notify_Send2AllUser(eGTNotify_EnteranceRemain, m_wCurRound, m_dwLastNotifyTime);	// 입장마감안내

					Notify_Send2AllUser(eGTNotify_RoundStart, m_wCurRound, 0);	// 라운드 시작통보
					m_dwRemainTime = m_GeneralInfo.wPlayTime[0] * 60000;
					StartTournament();
				}
				break;
			case eGTState_Leave:
				m_dwRemainTime = 0;
				break;
			case eGTState_AutoSchedule:
				{
					ReadyToNewTournament();
				}
				break;
			}
		}
		break;
	case MP_GTOURNAMENT_CHEAT_RESET:
		{
			if(g_pServerSystem->GetMapNum() != GTMAPNUM)
			{
				PACKEDDATA_OBJ->SendToMapServer(GTMAPNUM, (MSGBASE*)pMsg, sizeof(MSG_DWORD));
				return;
			}

			ReadyToNewTournament();
			GTInfoReset(m_wTournamentCount, GetInitRound());
		}
		break;
	case MP_GTOURNAMENT_NOTIFY2GUILD_TOMAP:
		{
			MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;

			CGuild* pGuild = GUILDMGR->GetGuild(pmsg->dwData1);
			if(!pGuild)	return;

			MSG_DWORD5 msg;
			memcpy(&msg, pmsg, sizeof(MSG_DWORD5));
			msg.Protocol = MP_GTOURNAMENT_NOTIFY2GUILD;		// 클라이언트에게 보내는 프로토콜로 변경

			pGuild->SendMsgToAll((MSGBASE*)&msg, sizeof(msg));
		}
		break;
	// 091202 ONS 길토 우승골드 보상 처리 추가
	case MP_GTOURNAMENT_REWARD_MONEY_TO_MAP:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if(pPlayer)
			{
				DWORD dwMoney = pmsg->dwData;
				pPlayer->SetMoney(dwMoney, MONEY_ADDITION);

				MSG_DWORD msg;
				ZeroMemory(&msg, sizeof(MSG_DWORD));
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_REWARD_MONEY;
				msg.dwData = dwMoney;
				pPlayer->SendMsg(&msg, sizeof(msg));
			}
		}
		break;
	}
}

int CGuildTournamentMgr::GetBattleID( CGuild* pGuild )
{
	if(!pGuild)		return -1;

	int i;
	for(i=0; i<MAXGUILD_INTOURNAMENT; i++)
	{
		if(m_BattleInfo[i].MemberInfo.TeamGuildIdx[0] == pGuild->GetIdx() ||
		m_BattleInfo[i].MemberInfo.TeamGuildIdx[1] == pGuild->GetIdx())
		
		return m_BattleInfo[i].BattleID;
	}

	return -1;
}

void CGuildTournamentMgr::SetResult( DWORD GuildIdx, BOOL Win, BOOL bUnearnedWin )
{
	GTInfo* pInfo = m_pEntryList.GetData(GuildIdx);
	if(!pInfo)	return;

	if(m_wCurRound <= eGTFight_2)
		Notify_Send2Guild(GuildIdx, eGTNotify_WinLose, Win, pInfo->wRound, bUnearnedWin);

	if(Win)
	{
		pInfo->wRound = m_wCurRound + 1;

		GTInfoUpdate(m_wTournamentCount, GuildIdx, pInfo->Position, pInfo->wRound);
	}
	pInfo->pGuild->SetGTBattleID(GT_INVALID_BATTLEID);

	m_wEndTournamentTeam++;
}

void CGuildTournamentMgr::GetTimeFromGTTime(DWORD dwSrcTime, WORD& wD, WORD& wH, WORD& wM)
{
	DWORD dwTime = dwSrcTime;

	wD = (WORD)(dwTime / 10000);
	dwTime -= wD * 10000;

	wH = (WORD)(dwTime / 100);
	dwTime -= wH * 100;

	wM = (WORD)dwTime;
}

DWORD CGuildTournamentMgr::GetGTTimeOP(DWORD dwSrcTime, DWORD dwOPTime, BYTE OPType)
{
	// 무척 거지같은 방식. 자주 호출되는게 아니니 참고 쓴다. ;;
	// GTournament 스크립트에서는 시간을 71230(매일12시30분), 01710(일요일17시10분)방식으로
	// 사용을 해서 이에 따른 연산이 번거로워 만든 유틸리티 함수.
	// dwSrcTime은 연산의 대상이 되는 시간을, dwOPTime연산을 수행할만큼의 시간을 넣는다.
	// ex) dwSrcTime:71230, dwOPTime:00010, OPType(Plus) 71230 에 10분을 더한다.

	WORD wSrcDay, wSrcHour, wSrcMin;
	WORD wOPDay, wOPHour, wOPMin;
	int nCalcDay, nCalcHour, nCalcMin;
	nCalcDay=nCalcHour=nCalcMin = 0;

	GetTimeFromGTTime(dwSrcTime, wSrcDay, wSrcHour, wSrcMin);
	GetTimeFromGTTime(dwOPTime, wOPDay, wOPHour, wOPMin);

	if(eGTTime_OP_Add == OPType)
	{
		nCalcMin = wSrcMin+wOPMin;
		if(59 < nCalcMin)
		{
			nCalcMin = nCalcMin - 60;
			nCalcHour += 1;
		}

		nCalcHour += wSrcHour+wOPHour;
		if(23 < nCalcHour)
		{
			nCalcHour = nCalcHour - 24;
			nCalcDay += 1;
		}

		if(wSrcDay==7)
			return 70000 + (nCalcHour*100) + (nCalcMin);

		nCalcDay += wSrcDay+wOPDay;
		if(6 < nCalcDay)
			nCalcDay = nCalcDay - 7;
		else
			nCalcDay += 1;

		return (nCalcDay*10000) + (nCalcHour*100) + (nCalcMin);
	}
	else if(eGTTime_OP_Sub == OPType)
	{
		nCalcMin = wSrcMin-wOPMin;
		if(nCalcMin < 0)
		{
			nCalcMin = 60 + nCalcMin;
			nCalcHour -= 1;
		}

		nCalcHour += wSrcHour-wOPHour;
		if(nCalcHour < 0)
		{
			nCalcHour = 24 + nCalcHour;
			nCalcDay -= 1;
		}

		if(wSrcDay==7)
			return 70000 + (nCalcHour*100) + (nCalcMin);

		nCalcDay += wSrcDay-wOPDay;
		if(nCalcDay < 0)
			nCalcDay = 7 + nCalcDay;
		
		return (nCalcDay*10000) + (nCalcHour*100) + (nCalcMin);
	}

	return 0;
}

DWORD CGuildTournamentMgr::GetRemaindTime(DWORD dwDestTime)
{
	// destTime 은 71200 (7day 12hour 00min)의 형태로 입력되어야 한다.

	SYSTEMTIME st;
	GetLocalTime(&st);

	WORD wDestDay = (WORD)(dwDestTime / 10000);
	WORD wDestHour, wDestMin;
	int nRemainDay, nRemainHour, nRemainMin, nRemainSec;

	DWORD dwSrcTime, dwRemainTime=0;

	if(wDestDay == 7)
	{
		dwSrcTime = dwDestTime - (wDestDay * 10000);
		wDestHour = (WORD)(dwSrcTime / 100);
		dwSrcTime = dwSrcTime - (wDestHour * 100);
		wDestMin = (WORD)dwSrcTime;

		nRemainSec = 59 - st.wSecond;
		if(wDestMin != 0)
			wDestMin -= 1;

		nRemainMin = wDestMin - st.wMinute;
		if(nRemainMin < 0)
		{
			nRemainMin = 60 + nRemainMin;

			if(wDestHour == 0)
				wDestHour = 23;
			else
				wDestHour -= 1;
		}
		
		nRemainHour = wDestHour - st.wHour;
		if(nRemainHour < 0)
		{
			nRemainHour = 24 + nRemainHour;
		}

		dwRemainTime = GT_CONVERT_HOUR2MS(nRemainHour) + GT_CONVERT_MIN2MS(nRemainMin) + GT_CONVERT_SEC2MS(nRemainSec);
	}
	else
	{
		dwSrcTime = dwDestTime - (wDestDay * 10000);
		wDestHour = (WORD)(dwSrcTime / 100);
		dwSrcTime = dwSrcTime - (wDestHour * 100);
		wDestMin = (WORD)dwSrcTime;

		nRemainSec = 59 - st.wSecond;
		if(wDestMin != 0)
			wDestMin -= 1;

		nRemainMin = wDestMin - st.wMinute;
		if(nRemainMin < 0)
		{
			nRemainMin = 60 + nRemainMin;

			if(wDestHour == 0)
			{
				wDestHour = 23;
				
				if(wDestDay == 0)
					wDestDay = 6;
				else
					wDestDay -= 1;
			}
			else
				wDestHour -= 1;
		}
		
		nRemainHour = wDestHour - st.wHour;
		if(nRemainHour < 0)
		{
			nRemainHour = 24 + nRemainHour;

			if(wDestDay == 0)
				wDestDay = 6;
			else
				wDestDay -= 1;
		}

		nRemainDay = wDestDay - st.wDayOfWeek;
		if(nRemainDay < 0)
		{
			nRemainDay = 7 + nRemainDay;
		}

		dwRemainTime = GT_CONVERT_DAY2MS(nRemainDay) + GT_CONVERT_HOUR2MS(nRemainHour) + GT_CONVERT_MIN2MS(nRemainMin) + GT_CONVERT_SEC2MS(nRemainSec);
	}

	return dwRemainTime;
}

void CGuildTournamentMgr::FillLastGTInfo(DWORD dwGuildIdx, WORD wPosition, WORD wRound)
{
	CGuild* pGuild = GUILDMGR->GetGuild(dwGuildIdx);
	if(!pGuild)		return;

	GTInfo* pPreInfo = m_pEntryList.GetData(dwGuildIdx);
	if(pPreInfo)	return;

	GTInfo* pAddInfo = new GTInfo;
	if(pAddInfo)
	{
		pGuild->SetGTBattleID(GT_INVALID_BATTLEID);
		pAddInfo->pGuild = pGuild;
		pAddInfo->Position = wPosition;
		pAddInfo->wRound = wRound;
		m_pEntryList.Add(pAddInfo, pAddInfo->pGuild->GetIdx());

		if(wPosition < MAXGUILD_INTOURNAMENT)
		{
			m_InitBattleTable[wPosition] = pGuild->GetIdx();
			m_CurBattleTable[wPosition] = pGuild->GetIdx();

			m_bShuffleBattleTable = TRUE;
		}

		// 임시 문제발생시 로그남기기 : 등록팀정보 채우기
		FILE* fp = fopen("gtmap.log", "a+");
		if(fp)
		{
			char buf[256] = {0,};
			sprintf(buf, "FillLastGTInfo : dwGuildIdx(%d), wPosition(%d), wRound(%d), m_pEntryList(%d)\n", dwGuildIdx, wPosition, wRound, m_pEntryList.GetDataNum());
			fprintf(fp, buf);
			fflush(fp);
			fclose(fp);
		}
	}
}

void CGuildTournamentMgr::FillPlayerInfo(WORD wGTCount, DWORD dwGuildIdx, DWORD* pPlayers)
{
	GTInfo* pInfo = m_pEntryList.GetData(dwGuildIdx);

	if(pInfo)
	{
		int i;
		for(i=0; i<MAX_GTOURNAMENT_PLAYER; i++)
		{
			if(pPlayers[i])
			{
				GTPlayerInfo* pPreInfo = pInfo->PlayerList.GetData(pPlayers[i]);
				if(pPreInfo)	continue;

				GTPlayerInfo* pPlayerInfo = new GTPlayerInfo;
				if(pPlayerInfo)
				{
					pPlayerInfo->dwPlayerID = pPlayers[i];
					pInfo->PlayerList.Add(pPlayerInfo, pPlayerInfo->dwPlayerID);
				}
			}
		}
	}
}

void CGuildTournamentMgr::RemoveGTEntryInfo(DWORD dwGuildIdx)
{
	GTInfo* pInfo = m_pEntryList.GetData(dwGuildIdx);

	if(pInfo)
	{
		pInfo->ClearPlayerList();
		delete pInfo;
		pInfo = NULL;
	}

	m_pEntryList.Remove(dwGuildIdx);

	// 임시 문제발생시 로그남기기 : RemoveGTEntryInfo
	FILE* fp = fopen("gtmap.log", "a+");
	if(fp)
	{
		char buf[256] = {0,};
		sprintf(buf, "RemoveGTEntryInfo() : dwGuildIdx(%d), m_pEntryList(%d)\n", dwGuildIdx, m_pEntryList.GetDataNum());
		fprintf(fp, buf);
		fflush(fp);
		fclose(fp);
	}
}

DWORD CGuildTournamentMgr::GetRespawnByLevel(int nLevel)
{
	int i;
	for(i=0; i<m_GeneralInfo.nRespawnListSize; i++)
	{
		if(m_GeneralInfo.pRespawnList[i].IsInRange( WORD( nLevel ) ) )
			return m_GeneralInfo.pRespawnList[i].dwValue;
	}

	return 0;
}

DWORD CGuildTournamentMgr::GetImmortalTimeByClass(WORD wClassCode)
{
	GTImmortalInfo* pInfo = m_GeneralInfo.ImmortalList.GetData(wClassCode);
	if(pInfo)
	{
		return pInfo->dwTime;
	}

	return 0;
}

void CGuildTournamentMgr::GetPlayTypeObject(DWORD& dwType, DWORD& dwObject)
{
	dwType = m_GeneralInfo.wPlayRule[0];
	dwObject = m_GeneralInfo.wPlayRule[1];
}

void CGuildTournamentMgr::SetStateToRegist(WORD wTournamentCount)
{
	ReadyToNewTournament();

	m_wTournamentCount = wTournamentCount;
	m_wTournamentState = eGTState_Regist;
	Notify_Send2AllUser(eGTNotify_RegistStart, m_wTournamentCount);	// 신청시작 통보

	m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryEnd);
	m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

	GTInfoInsert(m_wTournamentCount, 0, "*GTRegist", eGTState_Regist, m_wCurRound);
	GTInfoLoadAll(m_wTournamentCount);
}

WORD CGuildTournamentMgr::AdjustedState()
{
	SYSTEMTIME st;

	GetLocalTime(&st);

	WORD wCheckOpenDay = (WORD)(m_GeneralInfo.dwStartTime / 10000);
	WORD wSDay, wSHour, wSMin;
	WORD wEDay, wEHour, wEMin;

	if(0==m_wLastRound && eGTState_BeforeRegist==m_wLastState)
	{
		m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
		return m_wTournamentState;														// DB에 기록없으니 다음경기로 넘김.
	}
	else if(!(eGTFight_End==m_wLastRound && eGTState_BeforeRegist==m_wLastState) &&		// 이전경기가 정상종료가 아니고
		(GetInitRound()<m_wLastRound || eGTState_Entrance<=m_wLastState))				// 한경기라도 진행된 상태로 비정상종료상태이면
	{
		m_bStartGame = TRUE;
		m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
		m_wTournamentState = eGTState_BeforeRegist;
		return m_wTournamentState;														// 다음경기로 넘김.
	}

	if(7 == wCheckOpenDay)
	{
		// 매일경기
		GetTimeFromGTTime(m_GeneralInfo.dwStartTime, wSDay, wSHour, wSMin);
		if(wSHour<st.wHour || (wSHour==st.wHour && wSMin<=st.wMinute))
		{
			m_bStartGame = TRUE;
			m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
			m_wTournamentState = eGTState_BeforeRegist;
			return m_wTournamentState;					// 경기시간이 지났음
		}

		GetTimeFromGTTime(m_GeneralInfo.dwEntryStart, wSDay, wSHour, wSMin);
		GetTimeFromGTTime(m_GeneralInfo.dwEntryEnd, wEDay, wEHour, wEMin);
		if(wEHour<st.wHour || (wEHour==st.wHour && wEMin<=st.wMinute))
		{
			if(m_bStartGame || m_bShuffleBattleTable)
			{
				m_bStartGame = TRUE;
				m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
				m_wTournamentState = eGTState_BeforeRegist;
				return m_wTournamentState;												// 대진표가 작성된 상태이면 다음경기로 넘김.
			}

			DWORD dwEntranceTime = GetGTTimeOP(m_GeneralInfo.dwStartTime, m_GeneralInfo.wPlayTime[1], eGTTime_OP_Sub);
			m_dwRemainTime = GetRemaindTime(dwEntranceTime);
			m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

			MakeBattleTable();

			m_wTournamentState = eGTState_BeforeEntrance;
			return m_wTournamentState;					// 접수종료시간이 지났음
		}

		if(st.wHour<wEHour || (st.wHour==wEHour && st.wMinute<wEMin))
		{
			if(GetInitRound()==m_wLastRound && eGTState_Regist==m_wLastState)
			{
				m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryEnd);
				m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

				m_wTournamentState = eGTState_Regist;
				return m_wTournamentState;					// 접수종료시간이 남았음
			}
			
			if((eGTFight_End==m_wLastRound && eGTState_BeforeRegist==m_wLastState) &&			// 이전경기가 정상종료이고, 제때 접수시작이 안되었다면
				(wSHour<=st.wHour && (wSHour<=st.wHour && wSMin<st.wMinute)))
			{
				m_wTournamentCount++;
				ReadyToNewTournament();
				GTInfoInsert(m_wTournamentCount, 0, "*GTRegist", eGTState_Regist, m_wCurRound);	// 새 토너먼트로 회차가 넘어갔음을 기록
				GTInfoLoad();

				m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryEnd);
				m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

				m_wTournamentState = eGTState_Regist;
				return m_wTournamentState;					// 접수종료시간이 남았음
			}
		}

		if(st.wHour<wSHour || (st.wHour==wSHour && st.wMinute<wSMin))
		{
			m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
			m_wTournamentState = eGTState_BeforeRegist;
			return m_wTournamentState;					// 접수시작 안했음
		}
	}
	else
	{
		WORD wODay, wOHour, wOMin;
		GetTimeFromGTTime(m_GeneralInfo.dwStartTime, wODay, wOHour, wOMin);
		GetTimeFromGTTime(m_GeneralInfo.dwEntryStart, wSDay, wSHour, wSMin);
		GetTimeFromGTTime(m_GeneralInfo.dwEntryEnd, wEDay, wEHour, wEMin);

		if(IsRegistDay())
		{
			if(0==m_wTournamentCount)	// 1회 토너먼트 시작전(==DB기록없음)
			{
				m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
				m_wTournamentState = eGTState_BeforeRegist;
				return m_wTournamentState;
			}

			m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryEnd);
			m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;
			m_wTournamentState = eGTState_Regist;
			return m_wTournamentState;
		}
		else
		{
			/*
			if(0==m_wTournamentCount || m_wLastState!=eGTState_BeforeEntrance)
			{
				m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
				m_wTournamentState = eGTState_BeforeRegist;
				return m_wTournamentState;
			}
			if(wODay==st.wDayOfWeek)	// 경기당일
			{
				if(st.wHour<=wOHour && st.wMinute<wOMin)
				{
					// 경기시작 전
					DWORD dwEntranceTime = GetGTTimeOP(m_GeneralInfo.dwStartTime, m_GeneralInfo.wPlayTime[1], eGTTime_OP_Sub);
					m_dwRemainTime = GetRemaindTime(dwEntranceTime);
					m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

					MakeBattleTable();

					m_wTournamentState = eGTState_BeforeEntrance;
					return m_wTournamentState;
				}
				else
				{
					// 경기시작 후
					m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
					m_wTournamentState = eGTState_BeforeRegist;
					return m_wTournamentState;
				}
			}
			else
			{
				DWORD dwEntranceTime = GetGTTimeOP(m_GeneralInfo.dwStartTime, m_GeneralInfo.wPlayTime[1], eGTTime_OP_Sub);
				m_dwRemainTime = GetRemaindTime(dwEntranceTime);
				m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

				MakeBattleTable();

				m_wTournamentState = eGTState_BeforeEntrance;
				return m_wTournamentState;
			}
			*/

			// 현재상태 계산방식 변경
			if(0==m_wTournamentCount)	// 1회 토너먼트 시작전(==DB기록없음)
			{
				m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
				m_wTournamentState = eGTState_BeforeRegist;
				return m_wTournamentState;
			}
			
			if(IsBeforeEntrace())
			{
				DWORD dwEntranceTime = GetGTTimeOP(m_GeneralInfo.dwStartTime, m_GeneralInfo.wPlayTime[1], eGTTime_OP_Sub);
				m_dwRemainTime = GetRemaindTime(dwEntranceTime);
				m_dwLastNotifyTime = GT_NOTIFY_STARTTIME;

				MakeBattleTable();

				m_wTournamentState = eGTState_BeforeEntrance;
				return m_wTournamentState;
			}
			else if(IsBeforeRegist())
			{
				m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
				m_wTournamentState = eGTState_BeforeRegist;
				return m_wTournamentState;
			}
		}
	}

	m_dwRemainTime = GetRemaindTime(m_GeneralInfo.dwEntryStart);
	m_wTournamentState = eGTState_BeforeRegist;
	return m_wTournamentState;
}

BOOL CGuildTournamentMgr::IsRegistDay()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	WORD wCheckOpenDay = (WORD)(m_GeneralInfo.dwStartTime / 10000);
	WORD wSDay, wSHour, wSMin;	//접수시작시간
	WORD wEDay, wEHour, wEMin;	//접수종료시간
	WORD wODay, wOHour, wOMin;	//경기시작시간

	GetTimeFromGTTime(m_GeneralInfo.dwEntryStart, wSDay, wSHour, wSMin);
	GetTimeFromGTTime(m_GeneralInfo.dwEntryEnd, wEDay, wEHour, wEMin);
	GetTimeFromGTTime(m_GeneralInfo.dwStartTime, wODay, wOHour, wOMin);

	if(wCheckOpenDay==7)
	{
		if(wSHour==st.wHour && wSMin<st.wMinute)
			return TRUE;
		else if(wEHour==st.wHour && st.wMinute<wEMin)
			return TRUE;
		else if(wSHour<st.wHour && st.wHour<wEHour)
			return TRUE;
	}
	else
	{
		if(st.wDayOfWeek==wSDay)
		{
			if((st.wDayOfWeek==wODay) && (wOHour<st.wHour || (wOHour==st.wHour && wOMin<st.wMinute)))
			{
				return FALSE;
			}
			else if(wSHour<st.wHour || (wSHour==st.wHour && wSMin<st.wMinute))
				return TRUE;
		}
		else if(st.wDayOfWeek==wEDay)
		{
			if((st.wDayOfWeek==wODay) && (wOHour<st.wHour || (wOHour==st.wHour && wOMin<st.wMinute)))
			{
				return FALSE;
			}
			else if(st.wHour<wEHour || (wEHour==st.wHour && st.wMinute<wEMin))
				return TRUE;
		}
		else
		{
			if(wEDay < wSDay)
			{
				if(0<=st.wDayOfWeek && st.wDayOfWeek<=wEDay)
					return TRUE;
				
				if(wSDay<=st.wDayOfWeek && st.wDayOfWeek<=6)
					return TRUE;
			}
			else
			{
				if(wSDay<st.wDayOfWeek && st.wDayOfWeek<wEDay)
					return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CGuildTournamentMgr::IsBeforeEntrace()
{
	// 반드시 IsRegistDay() 함수가 선행되어야 한다.

	SYSTEMTIME st;
	GetLocalTime(&st);

	WORD wCheckOpenDay = (WORD)(m_GeneralInfo.dwStartTime / 10000);
	WORD wEDay, wEHour, wEMin;	//접수종료시간
	WORD wODay, wOHour, wOMin;	//입장전시간

	GetTimeFromGTTime(m_GeneralInfo.dwEntryEnd, wEDay, wEHour, wEMin);
	
	DWORD dwEntranceTime = GetGTTimeOP(m_GeneralInfo.dwStartTime, m_GeneralInfo.wPlayTime[1], eGTTime_OP_Sub);
	GetTimeFromGTTime(dwEntranceTime, wODay, wOHour, wOMin);

	if(wCheckOpenDay==7)
	{
		if(wEHour==st.wHour && wEMin<st.wMinute)
			return TRUE;
		else if(wOHour==st.wHour && st.wMinute<wOMin)
			return TRUE;
		else if(wEHour<st.wHour && st.wHour<wOHour)
			return TRUE;
	}
	else
	{
		if(st.wDayOfWeek==wEDay)
		{
			if((st.wDayOfWeek==wODay) && (wOHour<st.wHour || (wOHour==st.wHour && wOMin<st.wMinute)))
			{
				return FALSE;
			}
			else if(wEHour<st.wHour || (wEHour==st.wHour && wEMin<st.wMinute))
				return TRUE;
		}
		else if(st.wDayOfWeek==wODay)
		{
			if(st.wHour<wOHour || (wOHour==st.wHour && st.wMinute<wOMin))
				return TRUE;
		}
		else
		{
			if(wODay < wEDay)
			{
				if(0<=st.wDayOfWeek && st.wDayOfWeek<=wODay)
					return TRUE;
				
				if(wEDay<=st.wDayOfWeek && st.wDayOfWeek<=6)
					return TRUE;
			}
			else
			{
				if(wEDay<st.wDayOfWeek && st.wDayOfWeek<wODay)
					return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CGuildTournamentMgr::IsBeforeRegist()
{
	// 반드시 IsRegistDay() 함수가 선행되어야 한다. 그후에
	// 반드시 IsBeforeEntrace() 함수가 선행되어야 한다.

	SYSTEMTIME st;
	GetLocalTime(&st);

	WORD wCheckOpenDay = (WORD)(m_GeneralInfo.dwStartTime / 10000);
	WORD wODay, wOHour, wOMin;	//경기시작시간
	WORD wSDay, wSHour, wSMin;	//접수시작시간

	GetTimeFromGTTime(m_GeneralInfo.dwStartTime, wODay, wOHour, wOMin);
	GetTimeFromGTTime(m_GeneralInfo.dwEntryStart, wSDay, wSHour, wSMin);

	if(wCheckOpenDay==7)
	{
		if(wOHour==st.wHour && wOMin<st.wMinute)
			return TRUE;
		else if(wSHour==st.wHour && st.wMinute<wSMin)
			return TRUE;
		else if(wOHour<st.wHour && st.wHour<wSHour)
			return TRUE;
	}
	else
	{
		if(st.wDayOfWeek==wODay)
		{
			if(wOHour<st.wHour || (wOHour==st.wHour && wOMin<st.wMinute))
				return TRUE;
		}
		else if(st.wDayOfWeek==wSDay)
		{
			if(st.wHour<wSHour || (wSHour==st.wHour && st.wMinute<wSMin))
				return TRUE;
		}
		else
		{
			if(wSDay < wODay)
			{
				if(0<=st.wDayOfWeek && st.wDayOfWeek<=wSDay)
					return TRUE;
				
				if(wODay<=st.wDayOfWeek && st.wDayOfWeek<=6)
					return TRUE;
			}
			else
			{
				if(wODay<st.wDayOfWeek && st.wDayOfWeek<wSDay)
					return TRUE;
			}
		}
	}

	return FALSE;
}

void CGuildTournamentMgr::SendReward(WORD wRank, DWORD dwGuildIdx)
{
	CGuild* pGuild = GUILDMGR->GetGuild(dwGuildIdx);
	if(!pGuild)			return;

	GTRewardInfo* pRewardInfo = NULL;
	if(wRank == eGTFight_1)
		pRewardInfo = &m_GeneralInfo.RewardInfo[0];
	else if(wRank == eGTFight_2)
		pRewardInfo = &m_GeneralInfo.RewardInfo[1];

	if(!pRewardInfo)	return;

	// 1. 길드포인트 지급
	GuildAddScore(dwGuildIdx, (float)pRewardInfo->dwPoint);
	InsertLogGuildScore(dwGuildIdx, (float)pRewardInfo->dwPoint, eGuildLog_ScoreAddByGTWin );

	MSGBASE message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_GUILD;
	message.Protocol	= MP_GUILD_SCORE_UPDATE_TO_MAP;
	message.dwObjectID	= dwGuildIdx;

	g_Network.Broadcast2AgentServer( ( char* )&message, sizeof( message ) );

	// 2. 아이템 지급
	GTRewardItem* pReward = NULL;
	pRewardInfo->RewardItemList.SetPositionHead();
	while((pReward = pRewardInfo->RewardItemList.GetData())!= NULL)
	{
		DWORD dwUseTime = 0;
		ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(pReward->dwItemIndex);
		if(pItemInfo)
			dwUseTime = pItemInfo->dwUseTime;

		switch(pReward->nReceiver)
		{
		case eGTRewardReceiver_Master:
			{
				GTRewardItemInsert(pGuild->GetMasterIdx(), m_wTournamentCount, pReward->dwItemIndex, pReward->wNum, ITEMMGR->IsDupItem(pReward->dwItemIndex), dwUseTime);
			}
			break;
		case eGTRewardReceiver_Players:
			{
				GTInfo* pInfo = m_pEntryList.GetData(pGuild->GetIdx());
				if(pInfo)
				{
					GTPlayerInfo* pPlayerInfo = NULL;
					pInfo->PlayerList.SetPositionHead();
					while((pPlayerInfo = pInfo->PlayerList.GetData())!= NULL)
					{
						GTRewardItemInsert(pPlayerInfo->dwPlayerID, m_wTournamentCount, pReward->dwItemIndex, pReward->wNum, ITEMMGR->IsDupItem(pReward->dwItemIndex), dwUseTime);
					}
				}
			}
			break;
		case eGTRewardReceiver_GuildMember:
			{
				pGuild->SendGTRewardToAll(m_wTournamentCount, pReward->dwItemIndex, pReward->wNum, ITEMMGR->IsDupItem(pReward->dwItemIndex), dwUseTime);
			}
			break;
		}
	}

	// 091113 ONS 골드보상 추가
	// 골드(골드 * 참가팀수)를 길드마스터에게 골드 지급
	DWORD dwMoney = 0;
	dwMoney = pRewardInfo->dwGold * m_pEntryList.GetDataNum();
	if(dwMoney > 0)
	{
		// DB에 저장한다.
		GTRewardMoneyInsert( pGuild->GetMasterIdx(), dwMoney );
	}
}

void CGuildTournamentMgr::Notify_Send2Guild(DWORD dwGuildIdx, DWORD dwNotifyType, DWORD dwParam1, DWORD dwParam2, DWORD dwParma3)
{
	CGuild* pGuild = GUILDMGR->GetGuild(dwGuildIdx);
	if(!pGuild)		return;

	MSG_DWORD5 msg;
	msg.Category = MP_GTOURNAMENT;
	msg.Protocol = MP_GTOURNAMENT_NOTIFY2GUILD_TOMAP;
	msg.dwData1 = dwGuildIdx;
	msg.dwData2 = dwNotifyType;
	msg.dwData3 = dwParam1;
	msg.dwData4 = dwParam2;
	msg.dwData5 = dwParma3;

	g_Network.Broadcast2AgentServer((char*)&msg,sizeof(msg));
}

void CGuildTournamentMgr::Notify_Send2GTPlayer(DWORD dwGuildIdx, int nNotifyType, DWORD dwParam1, DWORD dwParam2, DWORD dwParma3)
{
	static int nLastNotify_Send2GTPlayer = -1;
	static WORD wRepeatCnt_Send2GTPlayer = 0;

	if(100 < wRepeatCnt_Send2GTPlayer)
		return;

	GTInfo* pInfo = m_pEntryList.GetData(dwGuildIdx);
	if(!pInfo)		return;

	GTPlayerInfo* pPlayerInfo;
	pInfo->PlayerList.SetPositionHead();
	while( (pPlayerInfo =	pInfo->PlayerList.GetData() ) != NULL)
	{
		MSG_DWORD5 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_NOTIFY2PLAYER_TOMAP;
		msg.dwData1 = pPlayerInfo->dwPlayerID;
		msg.dwData2 = nNotifyType;
		msg.dwData3 = dwParam1;
		msg.dwData4 = dwParam2;
		msg.dwData5 = dwParma3;

		g_Network.Broadcast2AgentServer((char*)&msg,sizeof(msg));
	}

	if(nLastNotify_Send2GTPlayer != nNotifyType)
	{
		nLastNotify_Send2GTPlayer = nNotifyType;
		wRepeatCnt_Send2GTPlayer = 0;
	}

	wRepeatCnt_Send2GTPlayer++;
}

void CGuildTournamentMgr::Notify_Send2AllGTPlayer(int nNotifyType, DWORD dwParam1, DWORD dwParam2, DWORD dwParma3)
{
	static int nLastNotify_Send2AllGTPlayer = -1;
	static WORD wRepeatCnt_Send2AllGTPlayer = 0;

	if(100 < wRepeatCnt_Send2AllGTPlayer)
		return;

	GTInfo* pInfo;
	DWORD dwGuildIdx;
	m_pEntryList.SetPositionHead();

	while( (pInfo = m_pEntryList.GetData()) != NULL)
	{
		if(pInfo->wRound == m_wCurRound)
		{
			dwGuildIdx = pInfo->pGuild->GetIdx();
			Notify_Send2GTPlayer(dwGuildIdx, nNotifyType, dwParam1, dwParam2, dwParma3);
		}
	}

	if(nLastNotify_Send2AllGTPlayer != nNotifyType)
	{
		nLastNotify_Send2AllGTPlayer = nNotifyType;
		wRepeatCnt_Send2AllGTPlayer = 0;
	}

	wRepeatCnt_Send2AllGTPlayer++;
}

void CGuildTournamentMgr::Notify_Send2AllUser(int nNotifyType, DWORD dwParam1, DWORD dwParam2)
{
	static int nLastNotify_Send2AllUser = -1;
	static WORD wRepeatCnt_Send2AllUser = 0;

	if(100 < wRepeatCnt_Send2AllUser)
		return;

	MSG_DWORD3 msg;
	msg.Category = MP_GTOURNAMENT;
	msg.Protocol = MP_GTOURNAMENT_NOTIFY;
	msg.dwData1 = nNotifyType;
	msg.dwData2 = dwParam1;
	msg.dwData3 = dwParam2;

	g_Network.Broadcast2AgentServer((char*)&msg, sizeof(msg));

	if(nLastNotify_Send2AllUser != nNotifyType)
	{
		nLastNotify_Send2AllUser = nNotifyType;
		wRepeatCnt_Send2AllUser = 0;
	}

	wRepeatCnt_Send2AllUser++;
}
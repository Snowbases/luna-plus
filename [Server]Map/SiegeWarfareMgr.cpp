#include "stdafx.h"
#include "SiegeWarfareMgr.h"
#include "Guild.h"
#include "GuildManager.h"
#include "Network.h"
#include "Player.h"
#include "UserTable.h"
#include "MapDBMsgParser.h"
#include "GuildUnionManager.h"
#include "ServerTable.h"
#include "PackedData.h"
#include "MHFile.h"
#include "QuestManager.h"
#include "ObjectStateManager.h"
#include "MapObject.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "CharMove.h"
#include "Channelsystem.h"
#include "./SiegeRecallMgr.h"
#include "./NpcRecallMgr.h"
#include "BootManager.h"
#include "ItemManager.h"

CSiegeWarfareMgr::CSiegeWarfareMgr()
{
	m_dwWarfareType = eSiegeWarfare_Type_Max;
	m_dwMapType = 99;
	ZeroMemory(
		m_dwCastleGuildIdxList,
		sizeof(m_dwCastleGuildIdxList));
	m_wState = eSiegeWarfare_State_Before;
	m_bWaterSeedUsing = FALSE;
	m_dwWaterSeedUsingPlayerIdx = NULL;
	m_wNotifyStep = eSiegeWarfare_Notify_Befor_30m;
	m_bSummonFlag = FALSE;
	m_bWaterSeedFlag = FALSE;
	m_dwBuffSkillPay = 0 ;
	m_IsLoadedSummon = FALSE ;
	m_wCravingDistance = 0 ;
	memset( &m_stDGRP_Rushen, 0, sizeof(VillageWarp) ) ;
	memset( &m_stDGRP_Zevyn, 0, sizeof(VillageWarp) ) ;
}

CSiegeWarfareMgr::~CSiegeWarfareMgr()
{
}

VOID CSiegeWarfareMgr::Process()
{
	DWORD dwCurTick = GetTickCount() ;
	static DWORD dwLastCheckTime = 0;

	if(!m_IsLoadedSummon && GetState()) 
	{
		if( dwCurTick - dwLastCheckTime >= CHECK_SUMMON_TIME )
		{
			SIEGERECALLMGR->Request_ObjFromDB() ;

			dwLastCheckTime = dwCurTick ;

			// 081007 LYW --- SiegeRecallMgr : 이 맵이 소환해야 하는 소환 카운트를 요청한다.
			MSG_WORD msg ;

			msg.Category	= MP_SIEGERECALL ;
			msg.Protocol	= MP_SIEGERECALL_REQUEST_RECALLCOUNT_SYN ;

			msg.dwObjectID	= 0 ;

			msg.wData		= g_pServerSystem->GetMapNum() ;

			g_Network.Send2AgentServer( (char*)&msg, sizeof(MSG_WORD) ) ;
		}
	}

	switch( m_dwWarfareType ) 
	{
	case eSiegeWarfare_Type_Nera:
		{
			switch( m_dwMapType )
			{
			// 네라 공성 마을
			case eNeraCastle_Village:
				VillageProcess();
				break;
			// 네라 지하 맵
			case eNeraCastle_UnderGround:
				UnderGroundProcess();
				break;
			// 네라 공성 맵
			case eNeraCastle_Zebin:
			case eNeraCastle_Lusen:
				EmblemProcess();
				break;
			}
		}
		break;
	}
}

VOID CSiegeWarfareMgr::LoadMapInfo()
{
	CMHFile fp;
	char szFile[256] = {0,};
	char szLine[256] = {0,};
	sprintf(szFile, "./System/Resource/SiegeWarInfo.bin");
	fp.Init(szFile, "rb");
	int nKind = 0;

	if(!fp.IsInited())
	{
		char szTmp[256];
		sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while(FALSE == fp.IsEOF())
	{
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if (strstr(szLine, "END")) 
		{
			break;
		}

		switch( nKind )
		{
		case eSiegeWarfare_Info_Damage_Ratio:
			sscanf(szLine, "%f", &m_stSkillInfo.DecreaseDamageRatio);
			break;
		case eSiegeWarfare_Info_Heal_Ratio:
			sscanf(szLine, "%f", &m_stSkillInfo.DecreaseHealRatio);
			break;
		case eSiegeWarfare_Info_Critical_Rate:
			sscanf(szLine, "%f", &m_stSkillInfo.DecreaseCriticalRatio);
			break;
		case eSiegeWarfare_Info_WarterSeed_GuildLevel:
			sscanf(szLine, "%d", &m_dwWaterSeedUsingGuildLevelLimit);
			break;
		case eSiegeWarfare_Info_WarterSeed_Time:
			sscanf(szLine, "%d", &m_dwWaterSeedUsingSecond);
			break;
		// 081017 LYW --- SiegeWarfareMgr : 각인을 허용하는 거리 값을 받는다.
		case eSiegeWarfare_Info_Craving_Distance :
			sscanf(szLine, "%u", &m_wCravingDistance) ;
			break ;
		case eSiegeWarfare_Info_VilageInfo:
			sscanf(szLine, "%d %f %f", &m_stCastleVilage.MapNum, &m_stCastleVilage.PosX, &m_stCastleVilage.PosZ);
			break;	

		// 081210 LYW --- SiegeWarfareMgr : 루쉔성 부활 위치를 로드한다.
		case eSiegeWarfare_Info_ReviveInfo_Rushen :
			sscanf(szLine, "%d %f %f", &m_stDGRP_Rushen.MapNum, &m_stDGRP_Rushen.PosX, &m_stDGRP_Rushen.PosZ ) ;
			break ;

		// 081210 LYW --- SiegeWarfareMGr : 제뷘성 부활 위치를 로드한다.
		case eSiegeWarfare_Info_ReviveInfo_Zevyn :
			sscanf(szLine, "%d %f %f", &m_stDGRP_Zevyn.MapNum, &m_stDGRP_Zevyn.PosX, &m_stDGRP_Zevyn.PosZ ) ;
		}

		nKind++;
	}

	sprintf(szFile, "./System/Resource/SiegeSchedule.bin");
	fp.Init(szFile, "rb");

	while(FALSE == fp.IsEOF())
	{
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if (strstr(szLine, "END")) 
		{
			break;
		}
		SYSTEMTIME stStartTime, stEndTime;
		// (일:0  월:1  화:2  수:3  목:4  금:5  토:6)
		sscanf(szLine, "%d %d %d %d %d %d", &stStartTime.wDayOfWeek, &stStartTime.wHour, &stStartTime.wMinute, &stEndTime.wDayOfWeek, &stEndTime.wHour, &stEndTime.wMinute);

		m_ScheduleStartTime[eSiegeWarfare_Type_Nera].push_back(stStartTime);
		m_ScheduleEndTime[eSiegeWarfare_Type_Nera].push_back(stEndTime);
	}

	sprintf(szFile, "./System/Resource/SiegeWarMapInfo.bin");
	fp.Init(szFile, "rb");

	if(!fp.IsInited())
	{
		char szTmp[256];
		sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while(FALSE == fp.IsEOF())
	{
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if (strstr(szLine, "END")) 
		{
			break;
		}

		SiegeWarfareKindMapInfo stMapInfo;

		memset(&stMapInfo, NULL, sizeof(SiegeWarfareKindMapInfo));

		sscanf(szLine, "%hd %hd %hd %f %f", &stMapInfo.WarfareType, &stMapInfo.MapNum, &stMapInfo.MapType, &stMapInfo.PosX, &stMapInfo.PosZ);

		m_SiegeWarfareKindMapList.push_back(stMapInfo);
	}

	sprintf(szFile, "./System/Resource/SiegeWarfareFobiddebSkillList.bin");
	fp.Init(szFile, "rb");

	if(!fp.IsInited())
	{
		char szTmp[256];
		sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while(FALSE == fp.IsEOF())
	{
		fp.GetLine(szLine, sizeof(szLine));
		if ( strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if( strstr(szLine, "END") )
		{
			break;
		}

		DWORD skillIdx = 0;
		sscanf(szLine, "%d", &skillIdx);

		m_ForbiddenSkillList.push_back(skillIdx);
	}

	sprintf(szFile, "./System/Resource/SiegeWarfareFobiddebItemList.bin");
	fp.Init(szFile, "rb");

	while(FALSE == fp.IsEOF())
	{
		fp.GetLine(szLine, sizeof(szLine));
		if ( strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if( strstr(szLine, "END") )
		{
			break;
		}

		DWORD itemIdx = 0;
		sscanf(szLine, "%d", &itemIdx);

		m_ForbiddenItemList.push_back(itemIdx);
	}

	fp.Release();

	// 맵정보를 넣어준다.
	std::list<SiegeWarfareKindMapInfo>::iterator BeginIter = m_SiegeWarfareKindMapList.begin();

	// 리스트를 검사하여 공성맵의 Kind를 가져온다.
	while( BeginIter != m_SiegeWarfareKindMapList.end() )
	{
		if( BeginIter->MapNum == g_pServerSystem->GetMapNum() )
		{
			m_dwMapType = BeginIter->MapType;
			m_dwWarfareType = BeginIter->WarfareType;
			break;
		}

		BeginIter++;
	}
	// DB에서 정보를 가져오는 코드 추가예정;;

	if( IsSiegeWarfareZone() )
	{
		LoadSiegeWarfareInfo();	
	}

	// 081006 LYW --- SiegeWarfareMgr : 공성 집행위원이 사용할 맵이동 포인트 로드.
	Load_GuildMapMoveInfo() ;
}

VOID CSiegeWarfareMgr::WarpToVilage()
{
	MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( (WORD)GetVilageInfo()->MapNum );

	VECTOR3 pos;

	pos.x = ChangeInfo->MovePoint.x ;
	pos.z = ChangeInfo->MovePoint.z ;

	g_pUserTable->SetPositionUserHead();

	while(CPlayer* const pPlayer = (CPlayer*)g_pUserTable->GetUserData())
	{
		if(pPlayer->GetObjectKind() != eObjectKind_Player)
		{
			continue;
		}
		else if(CHANNELSYSTEM->GetChannelID( pPlayer->GetChannelID() - 1 ) == 0)
		{
			continue;
		}
		else if( pPlayer->GetGuildIdx() == GetCastleGuildIdx(m_dwMapType) &&
			0 < GetCastleGuildIdx(m_dwMapType))
		{
			continue;
		}

		SaveMapChangePointUpdate( pPlayer->GetID(), ChangeInfo->Kind );
		pPlayer->UpdateLogoutToDB(FALSE);

		pPlayer->SetMapMoveInfo( (MAPTYPE)GetVilageInfo()->MapNum, (DWORD)pos.x, (DWORD)pos.z);

		MSG_WORD2 msg;
		msg.Category	= MP_USERCONN;
		msg.Protocol	= MP_USERCONN_MAP_OUT_WITHMAPNUM;
		msg.dwObjectID	= pPlayer->GetID();
		// 맵Num
		msg.wData1		= (WORD)GetVilageInfo()->MapNum;
		// 채널
		msg.wData2		= (WORD)-1;

		g_Network.Broadcast2AgentServer( (char*)&msg, sizeof(msg));

		g_pServerSystem->RemovePlayer( pPlayer->GetID() );
	}
}

VOID CSiegeWarfareMgr::WarpToVilagePlayer(CPlayer* pPlayer)
{	
	MAPCHANGE_INFO* ChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum( (WORD)GetVilageInfo()->MapNum );

	VECTOR3 pos;

	pos.x = ChangeInfo->MovePoint.x ;
	pos.z = ChangeInfo->MovePoint.z ;

	if( !pPlayer )
		return;

	if( CHANNELSYSTEM->GetChannelID( pPlayer->GetChannelID() - 1 ) == 0 )
		return;

	if( OBJECTSTATEMGR_OBJ->GetObjectState( pPlayer ) == eObjectState_Die )
	{
		OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer,eObjectState_Die);

		pPlayer->SetLife( pPlayer->GetMaxLife() * 30 / 100 );
	}

	SaveMapChangePointUpdate( pPlayer->GetID(), ChangeInfo->Kind );
	pPlayer->UpdateLogoutToDB(FALSE);

	pPlayer->SetMapMoveInfo( (MAPTYPE)GetVilageInfo()->MapNum, (DWORD)pos.x, (DWORD)pos.z);

	MSG_WORD2 msg;
	msg.Category	= MP_USERCONN;
	msg.Protocol	= MP_USERCONN_MAP_OUT_WITHMAPNUM;
	msg.dwObjectID	= pPlayer->GetID();
	// 맵Num
	msg.wData1		= (WORD)GetVilageInfo()->MapNum;
	// 채널
	msg.wData2		= (WORD)-1;

	g_Network.Broadcast2AgentServer( (char*)&msg, sizeof(msg));

	g_pServerSystem->RemovePlayer( pPlayer->GetID() );
}

VOID CSiegeWarfareMgr::Init()
{
	m_wState = eSiegeWarfare_State_Before;
	m_bWaterSeedUsing = FALSE;
	m_dwWaterSeedUsingPlayerIdx = NULL;
	m_wNotifyStep = eSiegeWarfare_Notify_Befor_30m;
	m_bSummonFlag = FALSE;
	m_IsLoadedSummon = FALSE;
}

BOOL CSiegeWarfareMgr::IsBeginSiegeWarfare()
{
	if(_countof(m_ScheduleStartTime) <= m_dwWarfareType)
	{
		return FALSE;
	}

	SYSTEMTIME st;
	GetLocalTime( &st );
	std::list<SYSTEMTIME>::iterator itr = m_ScheduleStartTime[m_dwWarfareType].begin();

	while( itr != m_ScheduleStartTime[m_dwWarfareType].end() )
	{
		SYSTEMTIME stNotify = *itr;

		stNotify.wHour = stNotify.wHour - 1;
		stNotify.wMinute = stNotify.wMinute + 30;

		if( stNotify.wMinute >= 60 )
		{
			stNotify.wHour = stNotify.wHour + 1;
			stNotify.wMinute = stNotify.wMinute - 60;
		}

		if( stNotify.wDayOfWeek == st.wDayOfWeek
			&& st.wHour == stNotify.wHour
			&& st.wMinute == stNotify.wMinute
			&& m_wNotifyStep == eSiegeWarfare_Notify_Befor_30m )
		{
				// 공성 30분전 메시지를 전송해주면 된다.
				MSG_DWORD msg;
				SetProtocol( &msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_NOTIFY);
				msg.dwData = eSiegeWarfare_Notify_Befor_30m;
				g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_DWORD));

				m_wNotifyStep = eSiegeWarfare_Notify_Befor_5m;
		}

		stNotify = *itr;

		stNotify.wHour = stNotify.wHour - 1;
		stNotify.wMinute = stNotify.wMinute + 55;

		if( stNotify.wMinute >= 60 )
		{
			stNotify.wHour = stNotify.wHour + 1;
			stNotify.wMinute = stNotify.wMinute - 60;
		}

		if( stNotify.wDayOfWeek == st.wDayOfWeek
			&& st.wHour == stNotify.wHour
			&& st.wMinute == stNotify.wMinute 
			&& m_wNotifyStep <= eSiegeWarfare_Notify_Befor_5m )
		{
			// 공성 5분전 메시지를 전송해주면 된다.
			MSG_DWORD msg;
			SetProtocol( &msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_NOTIFY);
			msg.dwData = eSiegeWarfare_Notify_Befor_5m;
			g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_DWORD));
			m_wNotifyStep = eSiegeWarfare_Notify_Begin;
		}

		stNotify = *itr;
		if(		st.wDayOfWeek == stNotify.wDayOfWeek
			&&	st.wHour == stNotify.wHour
			&&	st.wMinute == stNotify.wMinute
			)
		{
			// 이부분에서 공성전이 시작 되었다는 메시지 전송을 해준다.
			MSG_DWORD msg;
			SetProtocol( &msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_NOTIFY);
			msg.dwData = eSiegeWarfare_Notify_Begin;
			g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_DWORD));
			return TRUE;
		}
		itr++;
	}

	return FALSE;
}

VOID CSiegeWarfareMgr::VillageProcess()
{
	switch( m_wState )
	{
	case eSiegeWarfare_State_Before:
		{
			// 공성 시작을 체크 하기 위한 함수.
			if( IsBeginSiegeWarfare() )
			{
				SetState(eSiegeWarfare_State_Start, eSiegeWarfare_State_Message_All);

				// 공성 관련 소환물을 소환한다.
				MSG_WORD msg;
				msg.Category = MP_SIEGERECALL;
				msg.Protocol = MP_SIEGERECALL_EXECUTE_STEP_SYN;

				msg.wData = WORD( m_dwWarfareType );

				g_Network.Send2AgentServer((char*)&msg, sizeof(MSG_WORD));

				m_IsLoadedSummon = TRUE ;
			}
		}
		break;
	// 081017 LYW --- SiegeWarfareMgr : 공성 종료 체크 상태를 추가한다.
	case eSiegeWarfare_State_First :
	case eSiegeWarfare_State_Start:
		{
			// 공성 완료 시간을 체크 하기 위한 함수.
			if( IsEndSiegeWarfare() )
			{
				SetState(eSiegeWarfare_State_End, eSiegeWarfare_State_Message_All);

				// 공성전이 끝남을 알리는 메시지를 전송하여 준다.
				MSG_DWORD msg;
				SetProtocol( &msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_NOTIFY);
				msg.dwData = eSiegeWarfare_Notify_End;
				g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_DWORD));
			}
		}
		break;
	case eSiegeWarfare_State_End:
		{
			Init();

			//090812 pdy 공성전이 끝난후 DB정보가 eSiegeWarfare_State_Before로 변경안되는 버그수정
			UpdateSiegeWarfareInfo( TRUE );

			SetState(eSiegeWarfare_State_Before, 0);
		}
		break;
	}
}

VOID CSiegeWarfareMgr::UnderGroundProcess()
{
	switch( m_wState )
	{
	case eSiegeWarfare_State_Before:
		break;
	// 수호석 소환 가능
	case eSiegeWarfare_State_Start:
		break;
	case eSiegeWarfare_State_End:
		{
			WarpToVilage();
			Init();

			//090812 pdy 공성전이 끝난후 DB정보가 eSiegeWarfare_State_Before로 변경안되는 버그수정
			UpdateSiegeWarfareInfo( TRUE );

			SetState(eSiegeWarfare_State_Before, 0);
		}
		break;
	}
}

VOID CSiegeWarfareMgr::EmblemProcess()
{
	switch( m_wState )
	{
	case eSiegeWarfare_State_Before:
		break;
	// 수호석 소환 가능 -> 수호석이 모두 파괴 되었을 시에 State가 eSiegeWarfare_State_First로 변경됨
	case eSiegeWarfare_State_Start:
	// 워터시드 활성화 -> 워터시드를 소유하게 되면 성길드를 제외한 모든 플레이어는 성마을로 워프 시킴
	// 공성 시간이 완료 되었을시에 State가 eSiegeWarfare_State_End로 변경되고 공성전이 완료됨.
	case eSiegeWarfare_State_First:
		WaterSeedProcess();
		break;
	case eSiegeWarfare_State_End:
		{
			WarpToVilage();
			Init();
			UpdateSiegeWarfareInfo(TRUE);
			SetState(eSiegeWarfare_State_Before, 0);
		}
		break;
	}
}

VOID CSiegeWarfareMgr::SetState(WORD State, WORD MessageType)
{
	// 공성의 상황을 변경하여 주면서 변경된 내용을 유저들에게 알린다.
	// 080901 LYW --- SiegeWarfareMgr : 상태 변경을 MMC_SiegeWarfareState를 사용하지 않아, 
	// 현재 시점에서, 공성 상태 값이 db에 저장되지 않는다.
	// 맵이 떨어졌다, 공성중에 다시 살렸을 때, db에서 상태를 받지 못해서, 
	// Before 상태로 있게 된다.
	//m_wState = State;
	MMC_SiegeWarfareState(State) ;

	MSG_WORD2 msg;
	SetProtocol( &msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_STATE_ACK);
	msg.wData1 = MessageType;
	msg.wData2 = m_wState;
	g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_DWORD));

	// 081027 LUJ, 공성 로그
	{
		char text[ MAX_PATH ] = { 0 };
		sprintf(
			text,
			"state:%d",
			State );
		InsertLogSiege(
			eSiegeLogChangeState,
			0,
			0,
			text );
	}

	// 081028 LYW --- SiegeWarfareMgr : 공성 상태 바뀜을 로그로 남긴다.
	char szLog[1024] = {0, } ;
	sprintf( szLog, "MapNum : %d \t State : %d", g_pServerSystem->GetMapNum(), State ) ;
	WriteSiegeSchedule( szLog ) ;

	// 공성이 종료되면, 소환관련 DB삭제.
	if( m_wState == eSiegeWarfare_State_End )
	{
		char txt[256] = {0, } ;

		sprintf(txt, "EXEC  %s ", MP_SIEGERECALL_RESET ) ;

		g_DB.Query( eQueryType_FreeQuery, eSiegeRecallReset, 0, txt ) ;

		// 081012 LYW --- SiegeWarfareMgr : 소환 정보 초기화.
		SIEGERECALLMGR->Clear_RecallInfo() ;
	}
}

BOOL CSiegeWarfareMgr::IsEndSiegeWarfare()
{
	if(_countof(m_ScheduleStartTime) <= m_dwWarfareType)
	{
		return FALSE;
	}

	SYSTEMTIME st;
	GetLocalTime( &st );

	SYSTEMTIME stEndTime;

	std::list<SYSTEMTIME>::iterator	itr = m_ScheduleEndTime[m_dwWarfareType].begin();

	for( itr; itr != m_ScheduleEndTime[m_dwWarfareType].end(); itr++ )
	{
		stEndTime = *itr;
		if( m_wNotifyStep < eSiegeWarfare_Notify_End_Before_10m )
		{
			SYSTEMTIME stNotify = *itr;

			stNotify.wHour = stNotify.wHour - 1;
			stNotify.wMinute = stNotify.wMinute + 50;

			if( stNotify.wMinute >= 60 )
			{
				stNotify.wHour = stNotify.wHour + 1;
				stNotify.wMinute = stNotify.wMinute - 60;
			}

			if( stNotify.wDayOfWeek == st.wDayOfWeek &&
				stNotify.wHour == st.wHour &&
				stNotify.wMinute == st.wMinute )
			{
				MSG_DWORD msg;
				SetProtocol( &msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_NOTIFY);
				msg.dwData = eSiegeWarfare_Notify_End_Before_10m;
				g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_DWORD));

				m_wNotifyStep = eSiegeWarfare_Notify_End_Before_10m;
			}
		}

		
		if( st.wDayOfWeek != stEndTime.wDayOfWeek )
		{
			continue;
		}
		else if( st.wHour != stEndTime.wHour )
		{
			continue;
		}
		else if( st.wMinute == stEndTime.wMinute )
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CSiegeWarfareMgr::IsFobiddenSkill(DWORD SkillIdx)
{
	std::list<DWORD>::iterator BeginIter = m_ForbiddenSkillList.begin();

	while( BeginIter != m_ForbiddenSkillList.end() )
	{
		if( SkillIdx == *BeginIter )
			return TRUE;

		BeginIter++;
	}

	return FALSE;
}

BOOL CSiegeWarfareMgr::IsFobiddenItem(DWORD ItemIdx)
{
	std::list<DWORD>::iterator BeginIter = m_ForbiddenItemList.begin();

	while( BeginIter != m_ForbiddenItemList.end() )
	{
		if( ItemIdx == *BeginIter )
			return TRUE;

		BeginIter++;
	}

	return FALSE;
}

VOID CSiegeWarfareMgr::WaterSeedProcess()
{
	// 워터시드를 사용중이지 않다면 리턴~
	if( !m_bWaterSeedUsing || m_wState != eSiegeWarfare_State_First )
		return;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(m_dwWaterSeedUsingPlayerIdx);

	// 플레이어 정보를 가져올수 없다면 워터시드를 초기화 하고 return
	if( !pPlayer )
	{
		m_bWaterSeedUsing = FALSE;
		m_dwWaterSeedUsingPlayerIdx = NULL;
		// 081009 LYW --- SiegeWarfareMgr : 공성소스머지.
		m_bWaterSeedFlag = FALSE;
		return;
	}

	AbnormalStatus* pAbnormalStatus = pPlayer->GetAbnormalStatus();

	if( pAbnormalStatus->IsSlip ||
		pAbnormalStatus->IsStone ||
		pAbnormalStatus->IsStun ||
		pAbnormalStatus->IsParalysis || 
		pAbnormalStatus->IsFreezing ||
		pAbnormalStatus->IsSilence)
	{
		OutputDebug(
			"%s(%d) player: %u",
			__FUNCTION__,
			__LINE__,
			m_dwWaterSeedUsingPlayerIdx);
		OutputDebug(
			"Server is stopped using waterseed. slip(%s), stone(%s), stun(%s), paralysis(%s), freeze(%s), slience(%s)",
			0 < pAbnormalStatus->IsSlip ? "true" : "false",
			0 < pAbnormalStatus->IsStone ? "true" : "false",
			0 < pAbnormalStatus->IsStun ? "true" : "false",
			0 < pAbnormalStatus->IsParalysis ? "true" : "false",
			0 < pAbnormalStatus->IsFreezing ? "true" : "false",
			0 < pAbnormalStatus->IsSilence ? "true" : "false");

		CancelWaterSeedUsing(pPlayer);
		return;
	}

	CGuild* pGuild = GUILDMGR->GetGuild(pPlayer->GetGuildIdx());

	// 길드 정보를 가지고 올수 없어도 워터시드를 초기화 하고 return
	if( !pGuild )
	{
		m_bWaterSeedUsing = FALSE;
		m_dwWaterSeedUsingPlayerIdx = NULL;
		// 081009 LYW --- SiegeWarfareMgr : 공성소스머지.
		m_bWaterSeedFlag = FALSE;
		return;
	}

	// 080905 LYW --- SiegeWarfareMgr : 타겟이 우물 타입인지 확인한다.
	

	SYSTEMTIME st;
	GetLocalTime( &st );

	BOOL bSucess = FALSE;

	if( st.wHour > m_stWaterSeedUsingCompleteTime.wHour )
	{
		bSucess = TRUE;
	}
	else if( m_stWaterSeedUsingCompleteTime.wHour == st.wHour && 
			st.wMinute > m_stWaterSeedUsingCompleteTime.wMinute )
	{
		bSucess = TRUE;
	}
	else if(m_stWaterSeedUsingCompleteTime.wHour == st.wHour && 
			st.wMinute >= m_stWaterSeedUsingCompleteTime.wMinute &&
			st.wSecond >= m_stWaterSeedUsingCompleteTime.wSecond )
	{
		bSucess = TRUE;
	}

	// 081009 LYW --- SiegeWarfareMgr : 공성소스머지.
	if( bSucess && !m_bWaterSeedFlag )
	{
		m_bWaterSeedFlag = TRUE;
	}
}

BOOL CSiegeWarfareMgr::IsWaterSeedUsingPlayer(DWORD PlayerIdx)
{
	if( PlayerIdx == m_dwWaterSeedUsingPlayerIdx )
		return TRUE;

	return FALSE;
}

VOID CSiegeWarfareMgr::CancelWaterSeedUsing(CPlayer* pPlayer)
{
	OutputDebug(
		"%s(%s %d)",
		__FUNCTION__,
		pPlayer->GetObjectName(),
		pPlayer->GetID());

	if( m_wState != eSiegeWarfare_State_First )
	{
		OutputDebug(
			"State is not eSiegeWarfare_State_First");
		return;
	}

	if( IsWaterSeedUsing() )
	{
		if( IsWaterSeedUsingPlayer( pPlayer->GetID() ) )
		{
			m_bWaterSeedUsing = FALSE;
			m_dwWaterSeedUsingPlayerIdx = NULL;
			// 081009 LYW --- SiegeWarfareMgr : 공성소스머지.
			m_bWaterSeedFlag = FALSE;
			OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_Deal, 0);

			// 091123 공성전 워터시드 사용중 플레그 추가
			pPlayer->SetUsingWaterSeedFlag( FALSE ) ;

			// 081017 LYW --- SiegeWarfareMgr : 워터시드 사용 취소 처리 수정 - 다른 Player가 안보이는 현상 수정.
			// 081017 LYW --- ItemManager : 워터시드 사용 성공 처리를 agent로 보낸다.
			MSG_DWORD2 msg ;

			msg.dwData1 = g_pServerSystem->GetMapNum() ;
			msg.dwData2	= pPlayer->GetID() ;

			SetProtocol( &msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_USE_WATERSEED_CANCEL_ACK) ;
			g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_DWORD2)) ;

			// 081028 LYW --- SiegeWarfareMgr : 워터시드 사용 로그를 남긴다.
			char szLog[1024] = {0, } ;
			sprintf( szLog, "Cancel WaterSeed - Map :%d \t Player : %d %s", g_pServerSystem->GetMapNum(), pPlayer->GetID(), pPlayer->GetObjectName() ) ;
			WriteSiegeSchedule( szLog );

			OutputDebug(
				"You cancelled using waterseed");
		}
		else
		{
			OutputDebug(
				"You are not waterseed user");
		}
	}
	else
	{
		OutputDebug(
			"It is not waterseed using");
	}
}

BOOL CSiegeWarfareMgr::WarpToAgit(CPlayer* pPlayer)
{
	// 채널 정보가 아니라면...
	if( !pPlayer || CHANNELSYSTEM->GetChannelID( pPlayer->GetChannelID() - 1 ) == 0 )
		return false;

	WORD wMapType = eSiegeWarfare_Map_Max;
	SiegeWarfareKindMapInfo* pWarpMapInfo = NULL ;

	for(WORD i = 0;	i <= eSiegeWarfare_Map_Max; i++)
	{
		if( m_dwCastleGuildIdxList[i] == pPlayer->GetGuildIdx() )
			wMapType = i;
	}

	if( wMapType == eSiegeWarfare_Map_Max )
		return FALSE;

	std::list<SiegeWarfareKindMapInfo>::iterator itr = m_SiegeWarfareKindMapList.begin();

	while( itr != m_SiegeWarfareKindMapList.end() )
	{
		pWarpMapInfo = &(*itr);

		if( pWarpMapInfo->MapType == wMapType )
			break;

		itr++;
	}


	// 081010 LYW --- SiegeWarfareMgr : 맵이동 시키기 전에, 서버가 연결 되어있는지 확인한다.
	// 연결되어 있지 않다면, 이동 실패 메시지 처리를 한다.
	MSG_WORD msg ;

	msg.Category	= MP_USERCONN ;
	msg.Protocol	= MP_USERCONN_SIEGEWARFARE_MAP_OUT_WITHMAPNUM_SYN ;

	msg.dwObjectID	= pPlayer->GetID() ;

	msg.wData		= (MAPTYPE)pWarpMapInfo->MapNum ;

	g_Network.Send2AgentServer( (char*)&msg, sizeof(MSG_WORD) );
	return TRUE;
}





// 080922 LYW --- SiegeWarfareMgr : 집행위원을 통해 맵 이동을 하는 함수 추가.
VOID CSiegeWarfareMgr::ChangeMap(CPlayer* pPlayer, WORD wType, WORD wMapNum)
{
	// 에러 타입 정의
	// 0 : 성 마스터 길드가 아니다.
	// 1 : 맵 정보가 잘못 되었다.

	// Player 유효 체크.
	if( !pPlayer ) return ;
	// 090701 LUJ, 맵 이동 시 플레이어 정보를 DB에 쓰는데, 초기화가 끝나지 않았을 경우 쓰레기값이
	//		저장될 수 있다. 초기화가 끝난 객체만 처리한다
	else if( FALSE == pPlayer->GetInited() )
	{
		return;
	}

	// 길드 인덱스를 체크한다.
	// 081106 LYW --- SiegeWarfareMgr : 공성 최초 진행시나,
	// 공성을 소유한 길드가 없을경우예외 처리를 한다.
	DWORD dwCastleGuild = GetCastleGuildIdx(GetMapInfo()) ;
	if( dwCastleGuild != pPlayer->GetGuildIdx() || dwCastleGuild == 0 )
	//if( GetCastleGuildIdx(GetMapInfo()) != pPlayer->GetGuildIdx() )
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGEWARFARE ;
		msg.Protocol	= MP_SIEGEWARFARE_CHANGEMAP_GUILDCOMMITTEE_NACK ;

		msg.dwObjectID	= pPlayer->GetID() ;

		msg.bData		= 0 ;

		pPlayer->SendMsg( &msg, sizeof(msg) ) ;
		return ;
	}


	// 맵 이동 정보를 받는다.
	st_GUILDMAPMOVEINFO pInfo ;
	if( wType == eCastle_Rushen )
	{
		Get_GuildMapMoveInfo_Rushen( &pInfo, wMapNum ) ;
	}
	// 081106 LYW --- SiegeWarfareMgr : 제뷘 집행위원을 통해 맵이동이 안되는 버그 수정.
	//else if( wType == eCastle_Rushen )
	else if( wType == eCastle_Zevyn )
	{
		Get_GuildMapMoveInfo_Zevyn( &pInfo, wMapNum ) ;
	}
	else return ;

	if( !pInfo.mapNum ) 
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGEWARFARE ;
		msg.Protocol	= MP_SIEGEWARFARE_CHANGEMAP_GUILDCOMMITTEE_NACK ;

		msg.dwObjectID	= pPlayer->GetID() ;

		msg.bData		= 1 ;

		pPlayer->SendMsg( &msg, sizeof(msg) ) ;
		return ;
	}

	SaveMapChangePointUpdate_SiegeWarfare( pPlayer->GetID(), pInfo.mapNum, pInfo.fXpos, pInfo.fZpos ) ;

	pPlayer->UpdateLogoutToDB(FALSE) ;


	// 랜덤 위치를 설정한다.
	VECTOR3 RandPos ;

	int temp ;
	temp = rand() % 500 - 250 ;
	RandPos.x = pInfo.fXpos + temp ;
	temp = rand() % 500 - 250 ;
	RandPos.z = pInfo.fZpos + temp ;
	RandPos.y = 0 ;

	// Player의 맵 이동 정보를 저장한다.
	pPlayer->SetMapMoveInfo( pInfo.mapNum, (DWORD)RandPos.x, (DWORD)RandPos.z ) ;

	// 081218 LUJ, 값을 미리 복사해놓는다
	const DWORD		playerIndex		= pPlayer->GetID();
	const DWORD		userIndex		= pPlayer->GetUserID();
	const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
	const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );
	const MAPTYPE	mapType			= wMapNum;
	const WORD		channelIndex	= WORD(-1);

	// Player 정보를 서버에서 삭제한다.
	g_pServerSystem->RemovePlayer( pPlayer->GetID() ) ;

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


VOID CSiegeWarfareMgr::CheckBeforeState()
{
	// 080903 LYW --- SiegeWarfareMgr : 공성 타입을 확인한다.
	// 공성맵인지 확인한다.
	if( m_dwWarfareType >= eSiegeWarfare_Type_Max )//&& SIEGEWARFAREMGR->IsSiegeWarfareZone() )
	{
		char szMsg[256] = {0, } ;
		sprintf( szMsg, "[%s] - Invalid Warfare type!!\n\nWarfareType : %d", __FUNCTION__, m_dwWarfareType) ;
		g_Console.LOG(4, szMsg) ;
		return ;
	}

	SYSTEMTIME st;
	GetLocalTime( &st );

	SYSTEMTIME stStartTime, stEndTime;

	std::list<SYSTEMTIME>::iterator StartItr = m_ScheduleStartTime[m_dwWarfareType].begin();
	std::list<SYSTEMTIME>::iterator EndItr = m_ScheduleEndTime[m_dwWarfareType].begin();

	for( StartItr; StartItr != m_ScheduleStartTime[m_dwWarfareType].end(); StartItr++, EndItr++ )
	{
		stStartTime = *StartItr;
		stEndTime = *EndItr;

		// 100305 pdy 공성맵 이전상태 복구시 시간채크관련 버그 수정
		if(  stStartTime.wDayOfWeek ==  stEndTime.wDayOfWeek  && st.wDayOfWeek != stStartTime.wDayOfWeek )
		{
			//공성시간이 아니다.
			continue;
		}

		if(	stStartTime.wDayOfWeek < stEndTime.wDayOfWeek && 
			( st.wDayOfWeek < stStartTime.wDayOfWeek || st.wDayOfWeek > stEndTime.wDayOfWeek ) )
		{
			//공성시간이 아니다.
			continue;
		}

		if( stStartTime.wDayOfWeek > stEndTime.wDayOfWeek && 
			st.wDayOfWeek <  stStartTime.wDayOfWeek && st.wDayOfWeek > stEndTime.wDayOfWeek )
		{
			//공성시간이 아니다.
			continue;
		}

		//시작 시간보다 작은가 채크 
		if( st.wDayOfWeek == stStartTime.wDayOfWeek )
		{
			//시작하는 시간보다 시간이 작거나 시간이 같고 시작 분보다 작다면 
			if( st.wHour < stStartTime.wHour ||
				st.wHour == stStartTime.wHour && st.wMinute < stStartTime.wMinute )
			{
				//공성시간이 아니다.
				continue;
			}
		}

		//종료 시간을 지났는지 채크 
		if( st.wDayOfWeek == stEndTime.wDayOfWeek )
		{
			//끝나는 시간보다 시간이 지났거나 시간이 같고 끝나는 분보다 같거나 크다면
			if( st.wHour > stEndTime.wHour ||
				st.wHour == stEndTime.wHour && st.wMinute >= stEndTime.wMinute )
			{
				//공성시간이 아니다.
				continue;
			}
		}

		{
			switch( m_wState )
			{
			case eSiegeWarfare_State_Start:
				break;
			case eSiegeWarfare_State_First:
				break;
			case eSiegeWarfare_State_Before:
			case eSiegeWarfare_State_End:
				{
					// 081007 LYW --- SiegeWarfareMgr : 공성 시작 전 맵 종료 상태 -> 공성 시작 후 맵 켰을 때 처리 추가.
					if( m_dwMapType > eNeraCastle_Village )
					{
						if( !m_IsLoadedSummon )
						{
							// 공성 관련 소환물을 소환한다.
							MSG_WORD msg;
							msg.Category = MP_SIEGERECALL;
							msg.Protocol = MP_SIEGERECALL_EXECUTE_STEP_SYN;

							msg.wData = WORD( m_dwWarfareType );

							g_Network.Send2AgentServer((char*)&msg, sizeof(MSG_WORD));

							m_IsLoadedSummon = TRUE ;
						}
					}

					MMC_SiegeWarfareState(eSiegeWarfare_State_Start) ;
				}
				break;
			}

			return;
		}
	}
	
	SetState(
		eSiegeWarfare_State_Before);
	UpdateSiegeWarfareInfo(
		TRUE);
}

BOOL CSiegeWarfareMgr::IsSiegeWarfareZone(DWORD MapNum , BOOL bCheckWarState /*= TRUE*/)
{
	std::list<SiegeWarfareKindMapInfo>::iterator	itr	=	m_SiegeWarfareKindMapList.begin();

	for( itr; itr != m_SiegeWarfareKindMapList.end(); itr++ )
	{
		SiegeWarfareKindMapInfo MapInfo = *itr;
		
		if( MapInfo.MapNum == MapNum
			&& m_dwMapType > eNeraCastle_Village && m_dwMapType < eSiegeWarfare_Map_Max)
		{
			if( bCheckWarState == FALSE )
				return TRUE;
			else if( m_wState > eSiegeWarfare_State_Before )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CSiegeWarfareMgr::IsSiegeWarfareZone()
{
	std::list<SiegeWarfareKindMapInfo>::iterator	itr	=	m_SiegeWarfareKindMapList.begin();

	for( itr; itr != m_SiegeWarfareKindMapList.end(); itr++ )
	{
		SiegeWarfareKindMapInfo MapInfo = *itr;
		
		if( MapInfo.MapNum == g_pServerSystem->GetMapNum() )
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------------------------------------------------
// 패킷 관련 함수 시작


VOID CSiegeWarfareMgr::NetworkMsgParse(DWORD dwConnectionIndex, BYTE Protocol, void* pMsg)
{
	// 공성맵이 아닌곳에서도 사용될 패킷
	switch( Protocol )
	{
		case MP_SIEGEWARFARE_NOTIFY:
		{
			CObject* pObject = NULL;
			g_pUserTable->SetPositionHead();

			while( (pObject = g_pUserTable->GetData()) != NULL)
			{
				if( pObject->GetObjectKind() != eObjectKind_Player )
					continue;
				CPlayer* pPlayer = (CPlayer*)pObject;

				if( !pPlayer )
					return;

				pPlayer->SendMsg((MSG_DWORD*)pMsg, sizeof(MSG_DWORD));
			}
		}
		break;

		case MP_SIEGEWARFARE_WATERSEED_COMPLETE:
		{
			MSG_WATERSEED_COMPLETE* pmsg = (MSG_WATERSEED_COMPLETE*)pMsg;
		
			// 100308 pdy 동시에 같은길드가 성을 먹을수 없도록 처리 추가.
			if( m_dwWaterSeedUsingPlayerIdx != NULL && pmsg->dwMapType != m_dwMapType )
			{
				// 현재 워터시드 사용자와 같은길드의 사람이 다른 성을 먹었다면 캔슬처리
				CPlayer * pPlayer = (CPlayer *)g_pUserTable->FindUser(m_dwWaterSeedUsingPlayerIdx);
				if( pPlayer && pPlayer->GetGuildIdx() == pmsg->dwGuildIdx )
				{
					OutputDebug(
						"MP_SIEGEWARFARE_WATERSEED_COMPLETE player: %d guild: %d",
						pPlayer->GetID(),
						pPlayer->GetGuildIdx());

					CancelWaterSeedUsing( pPlayer );
				}
			}

			// 공성 지역의 주인이 바뀐걸 기록해 준다.
			m_dwCastleGuildIdxList[pmsg->dwMapType] = pmsg->dwGuildIdx;
		}
		break;

		// 081217 LYW --- SiegeWarfareMgr : 공성 길드 던전에서 길드 소속 없이 사망시, 캐릭터 선택 화면으로 이동하는 처리 추가.
		case MP_SIEGEWARFARE_WARPVILLAGE_SYN :
			{
				MSGBASE* pmsg = (MSGBASE*)pMsg ;
				if( !pmsg ) return ;

				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID) ;
				if( !pPlayer ) return ;

				WarpToVilagePlayer( pPlayer ) ;
			}
			break ;

		//090706 pdy 워터시드 아이템 사용시 처리 수정 [아이템메니져 MP_ITEM_USE_SYN -> 길드전메니저 MP_SIEGEWARFARE_USE_WATERSEED_SYN ]
		case MP_SIEGEWARFARE_USE_WATERSEED_SYN:
		{
			OutputDebug(
				"MP_SIEGEWARFARE_USE_WATERSEED_SYN");

			MSG_SIEGEWARFARE_USE_WATERSEED_SYN * pmsg = (MSG_SIEGEWARFARE_USE_WATERSEED_SYN *)pMsg;
			CPlayer * pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);

			if(NULL == pPlayer)
			{
				OutputDebug(
					"Player is not existed(%u)",
					pmsg->dwObjectID);
				return;
			}

			int rt = eItemUseSuccess;

			//090706 pdy 클라이언트 워터시드 시간이 조작되어 있는지 확인한다.   
			if( pmsg->dwWaterSeedUsingSecond != m_dwWaterSeedUsingSecond )
			{
				//워터시드 시간이 조작되어있으면 사용실패 
				InsertLogSiege(
				eSiegeLogSetSeedFailed,
				pPlayer->GetGuildIdx(),
				pPlayer->GetID(),
				"WaterSeedTime Error" );

				rt = eItemUseErr_Err ;

				OutputDebug(
					"client time is invalid(client: %u, server: %u)",
					pmsg->dwWaterSeedUsingSecond,
					m_dwWaterSeedUsingSecond);
			}

			//090706 pdy 해당슬롯의 아이템이 워터시드가 맞는지 채크한다. 
			CItemSlot * pSlot = pPlayer->GetSlot( pmsg->TargetPos );

			const ITEMBASE * pItemBase = (!pSlot) ? NULL : pSlot->GetItemInfoAbs( pmsg->TargetPos ) ;

			const ITEM_INFO * pItemInfo = (!pItemBase) ? NULL : ITEMMGR->GetItemInfo(pItemBase->wIconIdx);

			if( ! pItemInfo )
			{
				rt = eItemUseErr_Err;

				OutputDebug(
					"Item is not existed(pos: %u, item index: %u)",
					pmsg->TargetPos,
					pItemBase->wIconIdx);
			}
			else if( pItemInfo && pItemInfo->SupplyType != ITEM_KIND_SIEGEWARFARE_WATERSEED )
			{
				//아이템 종류가 워터시드가 아니라면 
				InsertLogSiege(
				eSiegeLogSetSeedFailed,
				pPlayer->GetGuildIdx(),
				pPlayer->GetID(),
				"Invalid Item Kind Error" );

				rt = eItemUseErr_Err;

				OutputDebug(
					"Item is not waterseed(item index: %u)",
					pItemInfo->ItemIdx);
			}

			//검사를 통과 했으면 UseItem처리를 해준다. 
			if( rt == eItemUseSuccess )
			{
				rt = ITEMMGR->UseItem(pPlayer, pmsg->TargetPos, pmsg->wItemIdx);
			}

			// 090305 LYW --- ItemManager : 봉인 해제된 아이템은 eItemUseUnsealed값을 리턴한다.
			// 성공 처리를 하도록 한다.
			if(rt == eItemUseSuccess || rt == eItemUseUnsealed )
			{
				MSG_ITEM_USE_ACK msg;
				msg.Category = MP_ITEM ;
				msg.Protocol = MP_ITEM_USE_ACK;
				msg.TargetPos= pmsg->TargetPos;
				msg.dwItemIdx = pmsg->wItemIdx;
				msg.eResult	 = eItemUseSuccess;
				pPlayer->SendMsg(
					&msg,
					sizeof(msg));

				OutputDebug(
					"Item used");
			}
			else if( rt != eItemUseErr_WaterseedErr )
			{
				MSG_WORD msg1;
  				SetProtocol( &msg1, MP_SIEGEWARFARE, MP_SIEGEWARFARE_USE_WATERSEED_NACK);
				msg1.wData = eSiegeWarfare_WarterSeed_Error_Max;
  				g_Network.Broadcast2AgentServer((char*)&msg1, sizeof(MSG_WORD));
  
  				// 워터시드 사용자에게 워터시드 사용 Failed 메시지를 전송한다.
  				pPlayer->SendMsg( &msg1, sizeof(MSG_WORD) ) ;

				MSG_ITEM_ERROR msg2;
				msg2.Category = MP_ITEM;
				msg2.Protocol = MP_ITEM_USE_NACK;
				msg2.ECode = rt;
				pPlayer->SendMsg(
					&msg2,
					sizeof(msg2));

				OutputDebug(
					"#1 You failed to use item(error: %d)",
					rt);
			}
			else
			{
				MSG_ITEM_ERROR msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_USE_NACK;
				msg.ECode = rt;
				pPlayer->SendMsg(
					&msg,
					sizeof(msg));

				OutputDebug(
					"#2 You failed to use item(error: %d)",
					rt);
			}
		}
		break;

	default:
		break;
	}

	if( !IsSiegeWarfareZone() )
		return;

	switch( Protocol )
	{
	case MP_SIEGEWARFARE_STATE_SYN:
		{
			MSGBASE* msg = (MSGBASE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(msg->dwObjectID);
			if( !pPlayer )
				return;

			MSG_WORD2 Msg;
			SetProtocol(&Msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_STATE_ACK);
			Msg.wData1 = eSiegeWarfare_State_Message_Single;
			Msg.wData2 = this->GetState();
			pPlayer->SendMsg(&Msg, sizeof(MSG_WORD2));
		}
		break;
	case MP_SIEGEWARFARE_STATE_ACK:
		{
			MSG_WORD2* msg = (MSG_WORD2*)pMsg;
			MMC_SiegeWarfareState(msg->wData2);
		}
		break;

	case MP_SIEGEWARFARE_USE_WATERSEED_ACK:
		break;
	case MP_SIEGEWARFARE_USE_WATERSEED_NACK:
		break;
	case MP_SIEGEWARFARE_USE_WATERSEED_CANCEL:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( !pPlayer )
				return;

			OutputDebug(
				"Protocol: %d, player: %d",
				Protocol,
				pmsg->dwObjectID);

			CancelWaterSeedUsing(pPlayer);
		}
		break;
	case MP_SIEGEWARFARE_REVIVE_SYN:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( !pPlayer )
				return;
			WarpToVilagePlayer(pPlayer);
		}
		break;
	case MP_SIEGEWARFARE_BUFF_SYN:
		{
			MSG_WORD* pmsg = (MSG_WORD*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			// 081118 LYW --- SiegeWarfareMgr : 버프 요청이 왔을 때, Player 정보가 없으면 return 처리를 한다.
			if( !pPlayer ) return ;

			WORD uniqueNpcIndex = pmsg->wData;

			BYTE byResult = CM_RequestCastleBuff(pPlayer, uniqueNpcIndex) ;
			if( byResult != e_REQUEST_BUFFSKILL_SUCCESS )
			{
				MSG_BYTE Msg;
				SetProtocol(&Msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_BUFF_NACK);
				Msg.bData = byResult ;
				pPlayer->SendMsg(&Msg, sizeof(MSG_BYTE));
				return;
			}
		}
		break;

		// 080929 LYW --- SiegeWarfareMgr : 깃발을 이용이 가능한지 확인하는 함수 추가.
	case MP_SIEGEWARFARE_CHECK_FLAGUSE_SYN : 
		{
			MSG_WORD* const pmsg = (MSG_WORD*)pMsg ;

			if( !pmsg ) return ;

			CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID ) ;

			if( !pPlayer ) return ;


			// npc의 맵 번호를 확인해 어떤 길드가 소유하고 있는지 확인한다.
			DWORD dwMasterGuildIdx = 0 ;
			if( pmsg->wData == 97 )
			{
				dwMasterGuildIdx = GetCastleGuildIdx(eNeraCastle_Zebin) ;
			}
			else if( pmsg->wData == 98 )
			{
				dwMasterGuildIdx = GetCastleGuildIdx(eNeraCastle_Lusen) ;
			}
			else return ;


			// 성을 소유한 길드가 없으면, 예외 메시지를 전송한다.
			if( dwMasterGuildIdx == 0 )
			{
				MSGBASE msg ;

				msg.Category	= MP_SIEGEWARFARE ;
				msg.Protocol	= MP_SIEGEWARFARE_CHECK_FLAGUSE_EMPTYMASTER ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				pPlayer->SendMsg( &msg, sizeof(MSGBASE) ) ;

				return ;
			}


			// 해당 성의 길드 인덱스와 Player의 길드 인덱스가 같으면 깃발 이용 수락 메시지를 전송한다.
			if( dwMasterGuildIdx == pPlayer->GetGuildIdx() )
			{
				MSGBASE msg ;

				msg.Category	= MP_SIEGEWARFARE ;
				msg.Protocol	= MP_SIEGEWARFARE_CHECK_FLAGUSE_ACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				pPlayer->SendMsg( &msg, sizeof(MSGBASE) ) ;
			}
			// 해당 성의 길드 인덱스와 Player의 길드 인덱스가 다르면, 현재 길드 정보를 전송한다.
			else
			{
				// 길드 정보를 받는다.
				CGuild* pGuild = NULL ;
				pGuild = GUILDMGR->GetGuild( dwMasterGuildIdx ) ;

				if( !pGuild ) return ;


				// 마스터명/ 길드명을 가지고 있는 구조체라, 이 구조체를 사용한다.
				SEND_GUILD_INVITE msg ;

				msg.Category	= MP_SIEGEWARFARE ;
				msg.Protocol	= MP_SIEGEWARFARE_CHECK_FLAGUSE_NACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				msg.MasterIdx	= pmsg->wData ;

				SafeStrCpy( msg.MasterName, pGuild->GetMasterName(), MAX_NAME_LENGTH+1 ) ;
				SafeStrCpy( msg.GuildName, pGuild->GetGuildName(), MAX_GUILD_NAME+1 ) ;

				pPlayer->SendMsg( &msg, sizeof(SEND_GUILD_INVITE) ) ;
			}
		}
		break ;

		// 080919 LYW --- SiegeWarfareMgr : 깃발을 통한 아지트 이동 요청이 들어왔을 때 처리.
	case MP_SIEGEWARFARE_WARPTOAGIT_SYN :
		{
			MSGBASE* pmsg = NULL ;
			pmsg = (MSGBASE*)pMsg ;

			if( !pmsg ) return ;

			CPlayer* pPlayer = NULL ;
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID) ;

			if( !pPlayer ) return ;

			WarpToAgit(pPlayer) ;
		}
		break ;

		// 081009 LYW --- SiegeWarfareMgr : 공성소스 머지.
		// WaterSeed Complete Packet
case MP_SIEGEWARFARE_ISCOMPLETE_USING_WATERSEED_SYN:
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(m_dwWaterSeedUsingPlayerIdx);
			// 플레이어 정보를 가져올수 없다면 워터시드를 초기화 하고 return
			if( !pPlayer )
			{
				OutputDebug(
					"MP_SIEGEWARFARE_ISCOMPLETE_USING_WATERSEED_SYN Player(%u) is not existed",
					m_dwWaterSeedUsingPlayerIdx);

				m_bWaterSeedUsing = FALSE;
				m_dwWaterSeedUsingPlayerIdx = NULL;
				m_bWaterSeedFlag = FALSE;
				return;
			}

			if( m_bWaterSeedFlag )
			{
				// 공성 추가작업
				// 100308 pdy 동시에 같은길드가 성을 먹을수 없도록 처리 추가.
				// 바로 처리하지말고 디비에 셀렉트를 날려서 해당길드가 보유한 성 카운트를 가져온다 
				// 0인경우 성공, 1이상인경우 워터시드 캔슬처리  

				CGuild* pGuild = GUILDMGR->GetGuild(pPlayer->GetGuildIdx());

				// 길드 정보를 가지고 올수 없어도 워터시드를 초기화 하고 return
				if( !pGuild )
				{
					m_bWaterSeedUsing = FALSE;
					m_dwWaterSeedUsingPlayerIdx = NULL;
					m_bWaterSeedFlag = FALSE;

					OutputDebug(
						"MP_SIEGEWARFARE_ISCOMPLETE_USING_WATERSEED_SYN Guild is not existed(%u)",
						pPlayer->GetGuildIdx());
					return;
				}

				// DB에 업데이트 요청후 실패시 성을 먹지 못하도록 변경 
				UpdateWaterSeedComplateSynToDB(  m_dwWaterSeedUsingPlayerIdx ,pGuild->GetIdx() );

				OutputDebug(
					"MP_SIEGEWARFARE_ISCOMPLETE_USING_WATERSEED_SYN succeed (player: %u, guild: %u)",
					m_dwWaterSeedUsingPlayerIdx,
					pGuild->GetIdx());
			}
			else
			{
				OutputDebug(
					"MP_SIEGEWARFARE_ISCOMPLETE_USING_WATERSEED_SYN m_bWaterSeedFlag is false(%d)",
					m_bWaterSeedFlag);

				m_bWaterSeedUsing = FALSE;
				m_dwWaterSeedUsingPlayerIdx = NULL;
				m_bWaterSeedFlag = FALSE;
				OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_Deal, 0);
			}
		}
		break;

		// 080922 LYW --- SiegeWarafareMgr : 집행위원을 통한 맵 이동 요청이 왔을 경우 처리.
	case MP_SIEGEWARFARE_CHANGEMAP_GUILDCOMMITTEE_SYN :
		{
			if(	SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
			{
				return;
			}

			MSG_WORD2* pmsg = NULL ;
			pmsg = (MSG_WORD2*)pMsg ;

			if( !pmsg ) return ;

			CPlayer* pPlayer = NULL ;
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID) ;

			if( !pPlayer ) return ;

			ChangeMap(pPlayer, pmsg->wData1, pmsg->wData2) ;
		}
		break ;

		// 080922 LYW --- SiegeWarfareMgr : 던전으로 이동하겠냐는 처리 추가.
	case MP_SIEGEWARFARE_MOVETODUNGEON_SYN :
		{
			if(	SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
			{
				return;
			}

			MSGBASE* pmsg = NULL ;
			pmsg = (MSGBASE*)pMsg ;

			if( !pmsg ) return ;

			// Player 정보를 받는다.
			CPlayer* pPlayer = NULL ;
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID) ;

			if( !pPlayer ) return ;

			// 성을 소유한 길드인지 체크한다.
			CGuild* pGuild = NULL ;
			pGuild = GUILDMGR->GetGuild( pPlayer->GetGuildIdx() ) ;

			if( !pGuild ) 
			{
				MSG_BYTE msg ;

				msg.Category	= MP_SIEGEWARFARE ;
				msg.Protocol	= MP_SIEGEWARFARE_MOVETODUNGEON_NACK ;

				msg.dwObjectID	= pPlayer->GetID() ;

				msg.bData		= 0 ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

				return ;
			}

			// 루센인지 제뷘인지 체크한다.
			DWORD dwGuildIdx = 0 ;
			DWORD dwMapType = 0 ;
			for(WORD i = 0;	i <= eSiegeWarfare_Map_Max; i++)
			{
				if( m_dwCastleGuildIdxList[i] == pPlayer->GetGuildIdx() )
				{
					dwGuildIdx = m_dwCastleGuildIdxList[i] ;
					dwMapType = i ;
					break ;
				}
			}

			// 사용할 수 없는 길드라는 에러 메시지를 전송한다.
			if( dwGuildIdx == 0 )
			{
				MSG_BYTE msg ;

				msg.Category	= MP_SIEGEWARFARE ;
				msg.Protocol	= MP_SIEGEWARFARE_MOVETODUNGEON_NACK ;

				msg.dwObjectID	= pPlayer->GetID() ;

				msg.bData		= 0 ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

				return ;
			}

			// 루센인가 제뷘인가?
			if( dwMapType == eNeraCastle_Zebin)
			{
				// 맵이동 로그인 포인트 저장.
				SaveMapChangePointUpdate( pPlayer->GetID(), 84 ) ;


				// DB에 유저 로그아웃 처리를 한다.
				pPlayer->UpdateLogoutToDB(FALSE) ;


				// 랜덤 위치를 설정한다.
				VECTOR3 RandPos ;

				int temp ;
				temp = rand() % 500 - 250 ;
				RandPos.x = float(100 + temp);
				temp = rand() % 500 - 250 ;
				RandPos.z = float(100 + temp);
				RandPos.y = 0 ;

				// Player의 맵 이동 정보를 저장한다.
				pPlayer->SetMapMoveInfo( 84, (DWORD)RandPos.x, (DWORD)RandPos.z ) ;


				// 맵 아웃 처리를 한다.
				MSG_WORD2 msg ;
				msg.Category	= MP_USERCONN ;
				msg.Protocol	= MP_USERCONN_MAP_OUT_WITHMAPNUM ;
				msg.dwObjectID	= pPlayer->GetID() ;
				// 맵Num
				msg.wData1		= 84 ;
				// 채널
				msg.wData2		= (WORD)-1 ;

				g_Network.Broadcast2AgentServer( (char*)&msg, sizeof(msg));


				// Player 정보를 서버에서 삭제한다.
				g_pServerSystem->RemovePlayer( pPlayer->GetID() ) ;
			}
			else if( dwMapType == eNeraCastle_Lusen )
			{
				// 맵이동 로그인 포인트 저장.
				SaveMapChangePointUpdate( pPlayer->GetID(), 84 ) ;


				// DB에 유저 로그아웃 처리를 한다.
				pPlayer->UpdateLogoutToDB(FALSE) ;


				// 랜덤 위치를 설정한다.
				VECTOR3 RandPos ;

				int temp ;
				temp = rand() % 500 - 250 ;
				RandPos.x = float( 1000 + temp );
				temp = rand() % 500 - 250 ;
				RandPos.z = float( 1000 + temp );
				RandPos.y = 0 ;

				// Player의 맵 이동 정보를 저장한다.
				pPlayer->SetMapMoveInfo( 84, (DWORD)RandPos.x, (DWORD)RandPos.z ) ;


				// 맵 아웃 처리를 한다.
				MSG_WORD2 msg ;
				msg.Category	= MP_USERCONN ;
				msg.Protocol	= MP_USERCONN_MAP_OUT_WITHMAPNUM ;
				msg.dwObjectID	= pPlayer->GetID() ;
				// 맵Num
				msg.wData1		= 84 ;
				// 채널
				msg.wData2		= (WORD)-1 ;

				g_Network.Broadcast2AgentServer( (char*)&msg, sizeof(msg));


				// Player 정보를 서버에서 삭제한다.
				g_pServerSystem->RemovePlayer( pPlayer->GetID() ) ;
			}
			else
			{
				MSG_BYTE msg ;

				msg.Category	= MP_SIEGEWARFARE ;
				msg.Protocol	= MP_SIEGEWARFARE_MOVETODUNGEON_NACK ;

				msg.dwObjectID	= pPlayer->GetID() ;

				msg.bData		= 0 ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

				return ;
			}
		}
		break ;

	// 081010 LYW --- SiegeWarfareMgr : 아지트 워프 이동 처리 추가.
		case MP_USERCONN_SIEGEWARFARE_MAP_OUT_WITHMAPNUM_ACK :
			{
				MSG_WORD* pmsg = NULL ;
				pmsg = (MSG_WORD*)pMsg ;

				if( !pmsg ) return ;


				CPlayer* pPlayer = NULL ;
				pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID) ;

				if( !pPlayer ) return ;


				// 채널 정보가 아니라면...
				if( !pPlayer || CHANNELSYSTEM->GetChannelID( pPlayer->GetChannelID() - 1 ) == 0 )
					return ;

				WORD wMapType = eSiegeWarfare_Map_Max;
				SiegeWarfareKindMapInfo* pWarpMapInfo = NULL;

				for(WORD i = 0;	i <= eSiegeWarfare_Map_Max; i++)
				{
					if( m_dwCastleGuildIdxList[i] == pPlayer->GetGuildIdx() )
						wMapType = i;
				}

				if( wMapType == eSiegeWarfare_Map_Max )
					return ;

				std::list<SiegeWarfareKindMapInfo>::iterator itr = m_SiegeWarfareKindMapList.begin();

				while( itr != m_SiegeWarfareKindMapList.end() )
				{
					pWarpMapInfo = &(*itr);

					if( pWarpMapInfo->MapType == wMapType )
						break;

					itr++;
				}

				pPlayer->UpdateLogoutToDB(FALSE);

				pPlayer->SetMapMoveInfo( (MAPTYPE)pWarpMapInfo->MapNum, (DWORD)pWarpMapInfo->PosX, (DWORD)pWarpMapInfo->PosZ);

				MSG_WORD2 msg;
				msg.Category	= MP_USERCONN;
				msg.Protocol	= MP_USERCONN_MAP_OUT_WITHMAPNUM;
				msg.dwObjectID	= pPlayer->GetID();
				// 맵Num
				msg.wData1		= (WORD)pWarpMapInfo->MapNum;
				// 채널
				msg.wData2		= (WORD)-1;

				g_Network.Broadcast2AgentServer( (char*)&msg, sizeof(msg));

				g_pServerSystem->RemovePlayer( pPlayer->GetID() );
			}
			break ;

		case MP_USERCONN_SIEGEWARFARE_MAP_OUT_WITHMAPNUM_NACK : 
			{
				MSGBASE* pmsg = NULL ;
				pmsg = (MSGBASE*)pMsg ;

				if( !pmsg ) return ;


                CPlayer* pPlayer = NULL ;
				pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID) ;

				if( !pPlayer ) return ;


				MSGBASE msg ;

				msg.Category	= MP_USERCONN ;
				msg.Protocol	= MP_USERCONN_CHANGEMAP_NACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				pPlayer->SendMsg( &msg, sizeof(MSGBASE) ) ;
			}
			break ;

	default : break ;
	}
}

VOID CSiegeWarfareMgr::MMC_SiegeWarfareState(WORD State)
{
	if( m_wState == State )
		return;

	// 081028 LYW --- SiegeWarfareMgr : 로그를 남긴다.
	SetState(State) ;
	//m_wState = State;

	UpdateSiegeWarfareInfo();

	// 081002 LYW --- SiegeWarfareMgr : 공성이 종료되면, 서버의 소환관련 삭제.
	if( m_wState == eSiegeWarfare_State_End )
	{
		SIEGERECALLMGR->EndSiegeWar_ClearRecallInfo() ;
	}
}

WORD CSiegeWarfareMgr::MC_InstallWaterSeed(CPlayer* pPlayer)
//BOOL CSiegeWarfareMgr::MC_InstallWaterSeed(CPlayer* pPlayer)
{
	if( !SIEGERECALLMGR->IsReadyToUseWaterSeed() ) 
	{
		InsertLogSiege(
			eSiegeLogSetSeedFailed,
			pPlayer->GetGuildIdx(),
			pPlayer->GetID(),
			"not ready" );
		return eSiegeWarfare_WarterSeed_Error_NotState ;
	}

	// 100210 pdy 공성전 종료와 일치되는 시점에 워터시드 사용요청시 상태값이 잘못 갱신되어 끝나지 않는 버그 수정
	if(m_wState == eSiegeWarfare_State_Start)
	{
		SetState(eSiegeWarfare_State_First) ;
	}

	// 081017 LYW --- SiegeWarfareMgr : 우물과 시전자의 거리를 체크한다.
	if( !Is_ValidDistance_FromWell( pPlayer ) ) 
	{
		InsertLogSiege(
			eSiegeLogSetSeedFailed,
			pPlayer->GetGuildIdx(),
			pPlayer->GetID(),
			"long distance" );
		return eSiegeWarfare_WarterSeed_Error_SoFarDistance ;
	}

	if( m_wState != eSiegeWarfare_State_First )
	{
		InsertLogSiege(
			eSiegeLogSetSeedFailed,
			pPlayer->GetGuildIdx(),
			pPlayer->GetID(),
			"invalid state" );
		return eSiegeWarfare_WarterSeed_Error_NotState;
	}

	if( !pPlayer || IsWaterSeedUsing() )
	{
		InsertLogSiege(
			eSiegeLogSetSeedFailed,
			pPlayer->GetGuildIdx(),
			pPlayer->GetID(),
			pPlayer ? "already using" : "no player" );
		return eSiegeWarfare_WarterSeed_Error_UsingPlayer;
	}

	CGuild* guild = GUILDMGR->GetGuild(pPlayer->GetGuildIdx());
	if( !guild )
	{
		InsertLogSiege(
			eSiegeLogSetSeedFailed,
			pPlayer->GetGuildIdx(),
			pPlayer->GetID(),
			"no guild" );
		return eSiegeWarfare_WarterSeed_Error_NotGuildJoin;
	}

	if( guild->GetLevel() < m_dwWaterSeedUsingGuildLevelLimit )
	{
		InsertLogSiege(
			eSiegeLogSetSeedFailed,
			pPlayer->GetGuildIdx(),
			pPlayer->GetID(),
			"invalid state" );
		return eSiegeWarfare_WarterSeed_Error_GuildLevel;
	}
	
	for( int loop_i = 0 ; loop_i < eSiegeWarfare_Map_Max ; loop_i++ )
	{
		if( guild->GetIdx() == m_dwCastleGuildIdxList[ loop_i] )
		{
			InsertLogSiege(
				eSiegeLogSetSeedFailed,
				pPlayer->GetGuildIdx(),
				pPlayer->GetID(),
				"owner guild" );
			return eSiegeWarfare_WarterSeed_Error_CastleGuld ;
		}
	}

	char szLog[1024] = {0, } ;
	sprintf( szLog, "Use WaterSeed - Map :%d \t Player : %d %s", g_pServerSystem->GetMapNum(), pPlayer->GetID(), pPlayer->GetObjectName() ) ;
	WriteSiegeSchedule( szLog ) ;

	m_bWaterSeedUsing = TRUE;
	m_dwWaterSeedUsingPlayerIdx = pPlayer->GetID() ;

	pPlayer->SetUsingWaterSeedFlag( TRUE );

	SYSTEMTIME st;
	GetLocalTime( &st );

	m_stWaterSeedUsingCompleteTime = st;

	m_stWaterSeedUsingCompleteTime.wSecond = WORD( m_stWaterSeedUsingCompleteTime.wSecond  + m_dwWaterSeedUsingSecond/1000 ) ;

	if( m_stWaterSeedUsingCompleteTime.wSecond/60 >= 1 )
	{
		m_stWaterSeedUsingCompleteTime.wMinute += m_stWaterSeedUsingCompleteTime.wSecond/60;
		m_stWaterSeedUsingCompleteTime.wSecond -= m_stWaterSeedUsingCompleteTime.wSecond/60*60;
	}

	if( m_stWaterSeedUsingCompleteTime.wMinute/60 >= 1 )
	{
		m_stWaterSeedUsingCompleteTime.wHour += m_stWaterSeedUsingCompleteTime.wMinute/60;
		m_stWaterSeedUsingCompleteTime.wMinute -= m_stWaterSeedUsingCompleteTime.wMinute/60*60;
	}

	// 상태를 Deal로 해줌으로 다른 행동을 할수 없게 한다.
	OBJECTSTATEMGR_OBJ->StartObjectState(pPlayer, eObjectState_Deal, 0);

	InsertLogSiege(
		eSiegeLogSetSeedSucceed,
		pPlayer->GetGuildIdx(),
		pPlayer->GetID() );

	return 0;
}

VOID CSiegeWarfareMgr::LoadSiegeWarfareInfo()
{
	char szText[256];
	sprintf(szText, "EXEC %s",	STORED_SIEGEWARFARE_INFO_LOAD);
	g_DB.Query(eQueryType_FreeQuery, eSiegeWarfareInfoLoad, 0, szText);
}

//090812 pdy 공성전이 끝난후 DB정보가 eSiegeWarfare_State_Before로 변경안되는 버그수정
VOID CSiegeWarfareMgr::UpdateSiegeWarfareInfo(BOOL bInit)
{
	if( !bInit && m_wState == eSiegeWarfare_State_Before )
		return;

	char szText[256];
	sprintf(szText, "EXEC %s %d, %d, %d",	STORED_SIEGEWARFARE_INFO_UPDATE,
									m_dwMapType,
									m_dwCastleGuildIdxList[m_dwMapType],
									m_wState);
	g_DB.Query(eQueryType_FreeQuery, eSiegeWarfareInfoUpdate, 0, szText);
}

BYTE CSiegeWarfareMgr::CM_RequestCastleBuff(CPlayer* pPlayer, WORD wUniqueNpcIndex)
{
	WORD wMapType = eSiegeWarfare_Map_Max;

	// 성을 소유한 길드가 아니라면 실패
	for(WORD i = 0;	i <= eSiegeWarfare_Map_Max; i++)
	{
		if( m_dwCastleGuildIdxList[i] == pPlayer->GetGuildIdx() )
			wMapType = i;
	}

	// 현재 성의 마스터 길드가 아니면 실패.
	// 081113 LYW --- SiegeWarfareMgr : 길드가 없는 캐릭터가 성의 마스터 길드가 없을경우, 버프를 받을 수 있는 버그 수정.
	DWORD dwCastleGuild = GetCastleGuildIdx(GetMapInfo()) ;
	if( dwCastleGuild != pPlayer->GetGuildIdx() || dwCastleGuild == 0 )
		return e_REQUEST_BUFFSKILL_INVALID_GUILD ;

	// 공성 지역이 아니면 실패
	if( wMapType == eSiegeWarfare_Map_Max )
		return e_REQUEST_BUFFSKILL_INVALID_GUILD ;

	if( !IsSiegeWarfareZone() )
		return e_REQUEST_BUFFSKILL_INVALID_SIEGWZONE ;

	// 공성전이 시작이후라면 실패
	if( GetState() != eSiegeWarfare_State_Before )
		return e_REQUEST_BUFFSKILL_INVALID_TIME ;

	return NPCRECALLMGR->ExcuteNPCBuff(pPlayer, wUniqueNpcIndex) ;
}

void CSiegeWarfareMgr::Load_GuildMapMoveInfo()
{
	CMHFile file;
	file.Init("./System/Resource/MapMoveScroll_81.bin", "rb");

	while(!file.IsEOF())
	{
		st_GUILDMAPMOVEINFO info ;
//		SafeStrCpy(info.mapName, file.GetString(), 128 ) ;
		info.mapNum = file.GetWord() ;
		info.fXpos	= file.GetFloat() ;
		info.fZpos	= file.GetFloat() ;

		ASSERT(mapnum > 0 ) ;

		m_List_MapMove_Rushen.push_back( info ) ;
	}

	// 080919 LYW --- cMapMoveDialog : 제뷘 맵이동 리스트 정보.
	if(!file.Init("./System/Resource/MapMoveScroll_83.bin", "rb")) return ;

	while(!file.IsEOF())
	{
		st_GUILDMAPMOVEINFO info ;
//		SafeStrCpy(info.mapName, file.GetString(), 128 ) ;
		info.mapNum = file.GetWord() ;
		info.fXpos	= file.GetFloat() ;
		info.fZpos	= file.GetFloat() ;

		ASSERT(mapnum > 0 ) ;

		m_List_MapMove_Zevyn.push_back( info ) ;
	}

	file.Release() ;
}

void CSiegeWarfareMgr::Get_GuildMapMoveInfo_Rushen(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE map)
{
	if( !pInfo ) return ;
	memset( pInfo, 0, sizeof( st_GUILDMAPMOVEINFO ) ) ;

	L_GUILDMAPMOVEINFO::iterator it ;

	for( it = m_List_MapMove_Rushen.begin() ; it != m_List_MapMove_Rushen.end() ; ++it )
	{
		st_GUILDMAPMOVEINFO info = *it ;

		if( info.mapNum != map ) continue ;

//		SafeStrCpy( pInfo->mapName, info.mapName, 128 ) ;
		pInfo->mapNum	= info.mapNum ;
		pInfo->fXpos	= info.fXpos ;
		pInfo->fZpos	= info.fZpos ;

		return ;
	}
}

void CSiegeWarfareMgr::Get_GuildMapMoveInfo_Zevyn(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE map)
{
	if( !pInfo ) return ;
	memset( pInfo, 0, sizeof( st_GUILDMAPMOVEINFO ) ) ;

	L_GUILDMAPMOVEINFO::iterator it ;

	for( it = m_List_MapMove_Zevyn.begin() ; it != m_List_MapMove_Zevyn.end() ; ++it )
	{
		st_GUILDMAPMOVEINFO info = *it ;

		if( info.mapNum != map ) continue ;

//		SafeStrCpy( pInfo->mapName, info.mapName, 128 ) ;
		pInfo->mapNum	= info.mapNum ;
		pInfo->fXpos	= info.fXpos ;
		pInfo->fZpos	= info.fZpos ;

		return ;
	}
}





// 081017 LYW --- SiegeWarfareMgr : 워터시드와 시전자의 거리를 체크해서, 결과를 반환하는 함수 추가.
BYTE CSiegeWarfareMgr::Is_ValidDistance_FromWell(CPlayer* pPlayer)
{
	// 함수 인자를 확인한다.
	if( !pPlayer ) return FALSE ;


	// NPC의 유니크 인덱스를 확인한다.
	DWORD dwUniqueIdx = 0 ;
	// 공성 타입을 확인한다.
	switch( m_dwWarfareType )
	{
	case eSiegeWarfare_Type_Nera :
		{
			// 맵 타입을 확인한다.
			switch(m_dwMapType)
			{
			case eNeraCastle_Zebin :	dwUniqueIdx = 131 ;	break ;

			case eNeraCastle_Lusen :	dwUniqueIdx = 132 ; break ;

			default : return FALSE ;
			}
		}
		break ;

	default : return FALSE ;
	}


	// Npc 위치 정보를 받는다.
	STATIC_NPCINFO* pNpcInfo = NULL ;
	pNpcInfo = GAMERESRCMNGR->GetStaticNpcInfo( WORD( dwUniqueIdx ) ) ;

	if( !pNpcInfo ) return FALSE ;


	// Player의 위치 정보를 받는다.
	BASEMOVE_INFO * pMoveInfo = NULL ;
	pMoveInfo = ((CObject*)pPlayer)->GetMoveInfo() ;

	if( !pMoveInfo ) return FALSE ;


	// 거리를 확인한다.
	float fDistance = CalcDistanceXZ( &pMoveInfo->CurPosition, &pNpcInfo->vPos ) ;

	if( fDistance > m_wCravingDistance ) return FALSE ;


	// 각인 영역 내에 있음을 반환한다.
	return TRUE ;
}





// 081028 LYW --- SiegeWarfareInfoMgr : 공성 스케쥴 진행을 로그로 남기는 함수 추가.
void CSiegeWarfareMgr::WriteSiegeSchedule( char* pMsg )
{
	// 함수 인자 확인.
	if( !pMsg ) return ;


	// 로그를 남긴다.
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	TCHAR szFile[_MAX_PATH] = {0, } ;
	sprintf( szFile, "Log/Siege_Schedule_%04d%02d%02d.log", time.wYear, time.wMonth, time.wDay ) ;

	FILE *fp = fopen(szFile, "a+") ;
	if (fp)
	{
		fprintf(fp, "[%s]\t%s\n", szTime, pMsg) ;
		fclose(fp) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: WriteLog
//	DESC		: The function to create a error log for siege recall manager.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 10, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeWarfareMgr::WriteLog(char* pMsg)
{
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	TCHAR szFile[_MAX_PATH] = {0, } ;
	sprintf( szFile, "Log/Map%d-SiegeWarfare.log", g_pServerSystem->GetMapNum() ) ;

	FILE *fp = fopen(szFile, "a+") ;
	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pMsg,  szTime) ;
		fclose(fp) ;
	}
}

VOID CSiegeWarfareMgr::UpdateWaterSeedComplateSynToDB( DWORD dwPlayerIdx , DWORD dwGuildidx )
{
	char szText[256];
	sprintf(szText, "EXEC %s %d, %d, %d",	"MP_SIEGEWARFARE_WATERSEED_COMPLETE",
									dwPlayerIdx,
									m_dwMapType,
									dwGuildidx);
	g_DB.Query(eQueryType_FreeQuery, eSiegeWarWarterSeedComplete, 0, szText);
}

VOID CSiegeWarfareMgr::UpdateWaterSeedComplateAckFromDB( DWORD dwResult , DWORD dwPlayerIdx ,DWORD dwMapType ,DWORD dwGuildIdx)
{
	if( dwMapType != m_dwMapType || dwPlayerIdx != m_dwWaterSeedUsingPlayerIdx )
		return;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(m_dwWaterSeedUsingPlayerIdx);
	CGuild* pGuild = (pPlayer) ? GUILDMGR->GetGuild(pPlayer->GetGuildIdx()) : NULL ;

	// 플레이어 정보를 가져올수 없다면 워터시드를 초기화 하고 return
	if( !pPlayer || !pGuild )
	{
		OutputDebug(
			"%s data is invalid player: %u, guild: %u",
			__FUNCTION__,
			m_dwWaterSeedUsingPlayerIdx,
			pPlayer ? pPlayer->GetGuildIdx() : 0);

		m_bWaterSeedUsing = FALSE;
		m_dwWaterSeedUsingPlayerIdx = NULL;
		m_bWaterSeedFlag = FALSE;
		return;
	}

	// 이런일은 없을듯 하지만.. 
	if( pPlayer->GetGuildIdx() != dwGuildIdx )
	{
		OutputDebug(
			"%s guild is not same(guild: %u, %u)",
			__FUNCTION__,
			pPlayer->GetGuildIdx(),
			dwGuildIdx);

		CancelWaterSeedUsing( pPlayer );
		return;
	}

	if( dwResult == 0 )
	{
		OutputDebug(
			"%s result is zero",
			__FUNCTION__);

		// 업데이트 실패시
		CancelWaterSeedUsing( pPlayer );
	}
	else
	{
		OutputDebug(
			"%s succeed",
			__FUNCTION__);

		// 091123 공성전 워터시드 사용중 플레그 추가 
		pPlayer->SetUsingWaterSeedFlag( FALSE ) ;

		// 업데이트 성공시
		OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer, eObjectState_Deal, 0);
		m_dwCastleGuildIdxList[m_dwMapType] = pGuild->GetIdx();
		m_bWaterSeedUsing = FALSE;
		m_dwWaterSeedUsingPlayerIdx = NULL;
		m_bWaterSeedFlag = FALSE;

		//// 메시지 추가
		MSG_WATERSEED_COMPLETE msg;
		SetProtocol( &msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_WATERSEED_COMPLETE);
		msg.dwObjectID = pPlayer->GetID() ;
		msg.dwMapType = m_dwMapType;
		msg.dwGuildIdx = pGuild->GetIdx();
		strcpy(msg.szGuildName, pGuild->GetGuildName());
 		g_Network.Broadcast2AgentServer((char*)&msg, sizeof(MSG_WATERSEED_COMPLETE));

		// 081028 LYW --- SiegeWarfareMgr : 워터시드 사용 로그를 남긴다.
		char szLog[1024] = {0, } ;
		sprintf( szLog, "Complete Use WaterSeed - Map :%d \t Player : %d %s \tGuildInfo : %d %s", 
			g_pServerSystem->GetMapNum(), pPlayer->GetID(), pPlayer->GetObjectName(), pGuild->GetIdx(), pGuild->GetGuildName() ) ;
		WriteSiegeSchedule( szLog ) ;

		WarpToVilage();
		// 081027 LUJ, 로그
		InsertLogSiege(
			eSiegeLogComplete,
			pGuild->GetIdx(),
			pPlayer->GetID() );
	}
}

// 100413 길드 해채시 성점령 취소 처리 추가.
void CSiegeWarfareMgr::GuildBreakUp( DWORD dwGuildidx )
{
	if( dwGuildidx == 0 || IsSiegeWarfareZone() == FALSE )
		return;

	for( int i =0 ; i < eSiegeWarfare_Map_Max ; i++ )
	{
		// 성을 먹은 길드라면 0으로 취소시킨다.
		if( m_dwCastleGuildIdxList[ i ] == dwGuildidx )
		{
			m_dwCastleGuildIdxList[i] = 0;
		}
	}
}

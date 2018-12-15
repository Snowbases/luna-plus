#include "stdafx.h"
#include <sys/stat.h>
#include <sys/types.h>
#include "PCRoomManager.h"
#include "MHFile.h"
#include "UserTable.h"
#include "Player.h"
#include "MapDBMsgParser.h"
#include "ItemManager.h"
#include "PackedData.h"
#include "..\[CC]Header\GameResourceStruct.h"
#include "..\[CC]Skill\Server\Manager\SkillManager.h"
#include "..\[CC]Skill\Server\Info\BuffSkillInfo.h"
#include "..\[CC]Skill\Server\Object\BuffSkillObject.h"

CPCRoomManager::CPCRoomManager()
{
	for( int i = 0; i < GameRoomTypeMaxCount; i++ )
	{
		m_htPCRoomItem[i].Initialize( 40 );
		m_htPCRoomBuff[i].Initialize( 40 );
		m_dwUpdatePointTime[i] = 60000;	// 기본시간 1분
		m_dwUpdatePointValue[i] = 0;
	}
	
	m_PCRoomUser.Initialize( 200 );

	m_wMallNPCIdx = 0;

	m_PCRoomMemberMap.clear();

	LoadScript();
}

CPCRoomManager::~CPCRoomManager()
{
	ReleaseScript();
	Release();
}

// 100305 ONS 서버 기동시 해당맵의 PC방 플레이어 정보가 남아있을 경우 초기화 시킨다.
void CPCRoomManager::Init()
{
	DBUpdatePCRoomBuff( 0,(WORD)eBuffInfoState_Init,0,0,g_pServerSystem->GetMapNum() );
}

void CPCRoomManager::Release()
{
	stPCRoomUserInfo* pPCRoomUserInfo = NULL;
	m_PCRoomUser.SetPositionHead();
	while( (pPCRoomUserInfo = m_PCRoomUser.GetData()) != NULL )
	{
		SAFE_DELETE( pPCRoomUserInfo );
	}
	m_PCRoomUser.RemoveAll();
}

void CPCRoomManager::ReleaseScript()
{
	for( int i = 0; i< GameRoomTypeMaxCount; i++ )
	{
		// Item 정보 제거
		stPCRoomItemInfo* pPCRoomItemInfo = NULL;
		m_htPCRoomItem[i].SetPositionHead();
		while( (pPCRoomItemInfo = m_htPCRoomItem[i].GetData()) != NULL )
		{
			SAFE_DELETE( pPCRoomItemInfo );
		}
		m_htPCRoomItem[i].RemoveAll();

		// Buff 정보 제거
		stPCRoomBuffInfo* pPCRoomBuffInfo = NULL;
		m_htPCRoomBuff[i].SetPositionHead();
		while( (pPCRoomBuffInfo = m_htPCRoomBuff[i].GetData()) != NULL )
		{
			pPCRoomBuffInfo->vecBuffIdx.clear();
			SAFE_DELETE( pPCRoomBuffInfo );
		}
		m_htPCRoomBuff[i].RemoveAll();

		PTRLISTPOS pos = m_PCRoomPartyBuffList[i].GetHeadPosition();

		for(stPCRoomPartyBuffInfo* pPCRoomPartyBuffInfo = (stPCRoomPartyBuffInfo*)m_PCRoomPartyBuffList[i].GetNext(pos);
			0 < pPCRoomPartyBuffInfo;
			pPCRoomPartyBuffInfo = (stPCRoomPartyBuffInfo*)m_PCRoomPartyBuffList[i].GetNext(pos))
		{
			delete pPCRoomPartyBuffInfo;
		}

		m_PCRoomPartyBuffList[i].RemoveAll();
	}

}

void CPCRoomManager::LoadScript()
{
	// 현재 PC방 접속자의 PC방 포인트 Update
	stPCRoomUserInfo* pPCRoomUserInfo = NULL;
	m_PCRoomUser.SetPositionHead();
	while( (pPCRoomUserInfo = m_PCRoomUser.GetData()) != NULL )
	{
		UpdatePoint( pPCRoomUserInfo->dwPlayerIdx );
	}

	ReleaseScript();

	enum Block
	{
		BlockNone,
		BlockItem,
		BlockItemWeekDay,
		BlockBuff,
		BlockBuffWeekDay,
		BlockBuffInfo,
		BlockPartyBuff,
		BlockPartyBuffInfo,
		BlockPoint,
	}
	blockType = BlockNone;

	GameRoomType eGameRoomType = GameRoomIsFree;
	
	DWORD dwOpenCnt = 0;
	BOOL bIsComment = FALSE;
	WORD dwWeekDay = 0;
	WORD wWeekDayItemCnt = 0;
	stPCRoomBuffInfo* pCurBuffInfo = NULL;
	stPCRoomPartyBuffInfo* pCurPartyBuffInfo = NULL;

	const char* const filename = "./System/Resource/PCRoomInfo.bin";
	stat( filename, &m_ScriptFileStat );

	CMHFile file;
	file.Init( (char*)filename, "rb" );

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
		const char* delimit = " \n\t=,~";
		const char* markBegin = "{";
		const char* markEnd = "}";
		char* token = strtok( buff, delimit );

		if( ! token )
		{
			continue;
		}
		else if( bIsComment )
		{
			continue;
		}
		else if( ! stricmp( "MallNPCIndex", token ) )
		{
			token = strtok( 0, delimit );
			if( !token )	break;
			m_wMallNPCIdx = WORD( atoi( token ) );
		}
		// 100811 ONS PC방 유료/무료 가맹점 구분 처리 추가.
		else if( ! stricmp( "PremiumBenefit", token ) )
		{
			eGameRoomType = GameRoomIsPremium;
		}
		else if( ! stricmp( "FreeBenefit", token ) )
		{
			eGameRoomType = GameRoomIsFree;
		}
		else if( ! stricmp( "ProvideItem", token ) )
		{
			blockType = BlockItem;
		}
		else if( ! stricmp( "ProvideBuff", token ) )
		{
			blockType = BlockBuff;
		}
		else if( ! stricmp( "ProvidePartyBuff", token ) )
		{
			blockType = BlockPartyBuff;
		}
		else if( ! stricmp( "ProvidePoint", token ) )
		{
			blockType = BlockPoint;
		}
		else if( ! strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++dwOpenCnt;
		}
		else if( ! strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--dwOpenCnt;

			switch( blockType )
			{
			case BlockItem:
			case BlockBuff:
			case BlockPartyBuff:
			case BlockPoint:
				{
					blockType = BlockNone;
				}
				break;
			case BlockItemWeekDay:
				{
					blockType = BlockItem;
				}
				break;
			case BlockBuffWeekDay:
				{
					blockType = BlockBuff;
					pCurBuffInfo = NULL;
				}
				break;
			case BlockBuffInfo:
				{
					blockType = BlockBuffWeekDay;
					pCurBuffInfo = NULL;
				}
				break;
			case BlockPartyBuffInfo:
				{
					blockType = BlockPartyBuff;
					pCurPartyBuffInfo = NULL;
				}
				break;
			}
		}

		switch( blockType )
		{
		case BlockItem:
			{
				if( stricmp( token, "sunday" ) == 0 )
				{
					dwWeekDay = 0;
					wWeekDayItemCnt = 0;
					blockType = BlockItemWeekDay;
				}
				else if( stricmp( token, "monday" ) == 0 )
				{
					dwWeekDay = 1;
					wWeekDayItemCnt = 0;
					blockType = BlockItemWeekDay;
				}
				else if( stricmp( token, "tuesday" ) == 0 )
				{
					dwWeekDay = 2;
					wWeekDayItemCnt = 0;
					blockType = BlockItemWeekDay;
				}
				else if( stricmp( token, "wednesday" ) == 0 )
				{
					dwWeekDay = 3;
					wWeekDayItemCnt = 0;
					blockType = BlockItemWeekDay;
				}
				else if( stricmp( token, "thursday" ) == 0 )
				{
					dwWeekDay = 4;
					wWeekDayItemCnt = 0;
					blockType = BlockItemWeekDay;
				}
				else if( stricmp( token, "friday" ) == 0 )
				{
					dwWeekDay = 5;
					wWeekDayItemCnt = 0;
					blockType = BlockItemWeekDay;
				}
				else if( stricmp( token, "saturday" ) == 0 )
				{
					dwWeekDay = 6;
					wWeekDayItemCnt = 0;
					blockType = BlockItemWeekDay;
				}
			}
			break;
		case BlockItemWeekDay:
			{
				if( stricmp( token, "item" ) == 0 )
				{
					// 최대 개수 초과시
					if( wWeekDayItemCnt >= MAX_PCROOM_PROVIDE_ITEM_NUM )
						break;

					token = strtok( 0, delimit );
					if( !token )	break;
					DWORD dwItemIdx = DWORD( atoi( token ) );

					token = strtok( 0, delimit );
					if( !token )	break;
					WORD wItemCnt = WORD( atoi( token ) );

					stPCRoomItemInfo* pPCRoomItemInfo = new stPCRoomItemInfo;
					pPCRoomItemInfo->dwItemIdx = dwItemIdx;
					pPCRoomItemInfo->wItemCnt = wItemCnt;

					++wWeekDayItemCnt;
					m_htPCRoomItem[eGameRoomType].Add( pPCRoomItemInfo, dwWeekDay );
				}
			}
			break;
		case BlockBuff:
			{
				if( stricmp( token, "sunday" ) == 0 )
				{
					dwWeekDay = 0;
					blockType = BlockBuffWeekDay;
				}
				else if( stricmp( token, "monday" ) == 0 )
				{
					dwWeekDay = 1;
					blockType = BlockBuffWeekDay;
				}
				else if( stricmp( token, "tuesday" ) == 0 )
				{
					dwWeekDay = 2;
					blockType = BlockBuffWeekDay;
				}
				else if( stricmp( token, "wednesday" ) == 0 )
				{
					dwWeekDay = 3;
					blockType = BlockBuffWeekDay;
				}
				else if( stricmp( token, "thursday" ) == 0 )
				{
					dwWeekDay = 4;
					blockType = BlockBuffWeekDay;
				}
				else if( stricmp( token, "friday" ) == 0 )
				{
					dwWeekDay = 5;
					blockType = BlockBuffWeekDay;
				}
				else if( stricmp( token, "saturday" ) == 0 )
				{
					dwWeekDay = 6;
					blockType = BlockBuffWeekDay;
				}
			}
			break;
		case BlockBuffWeekDay:
			{
				if( stricmp( token, "info" ) == 0 )
				{
					pCurBuffInfo = new stPCRoomBuffInfo;
					m_htPCRoomBuff[eGameRoomType].Add( pCurBuffInfo, dwWeekDay );

					blockType = BlockBuffInfo;
				}
			}
			break;
		case BlockBuffInfo:
			{
				if( !pCurBuffInfo )
				{
					break;
				}
				else if( stricmp( token, "level" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					pCurBuffInfo->minLv = LEVELTYPE( atoi( token ) );

					token = strtok( 0, delimit );
					if( !token )	break;
					pCurBuffInfo->maxLv = LEVELTYPE( atoi( token ) );
				}
				else if( stricmp( token, "buff" ) == 0 )
				{
					while( (token = strtok( 0, delimit )) != NULL )
					{
						pCurBuffInfo->vecBuffIdx.push_back( DWORD(atoi( token )) );
					}
				}
			}
			break;
		case BlockPartyBuff:
			{
				if( stricmp( token, "info" ) == 0 )
				{
					pCurPartyBuffInfo = new stPCRoomPartyBuffInfo;
					m_PCRoomPartyBuffList[eGameRoomType].AddTail( pCurPartyBuffInfo );
					blockType = BlockPartyBuffInfo;
				}
			}
			break;
		case BlockPartyBuffInfo:
			{
				if( !pCurPartyBuffInfo )
					break;
				else if( stricmp( token, "playernum" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					pCurPartyBuffInfo->wConnectedNumMin = WORD( atoi( token ) );

					token = strtok( 0, delimit );
					if( !token )	break;
					pCurPartyBuffInfo->wConnectedNumMax = WORD( atoi( token ) );
				}
				else if( stricmp( token, "buff" ) == 0 )
				{
					while( (token = strtok( 0, delimit )) != NULL )
					{
						pCurPartyBuffInfo->vecBuffIdx.push_back( DWORD(atoi( token )) );
					}
				}
			}
			break;
		case BlockPoint:
			{
				if( stricmp( token, "time" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					m_dwUpdatePointTime[eGameRoomType] = DWORD( atoi( token ) * 1000 );

					// 0이 되는 경우 에러가 나기 때문에 경고할수 있도록 한다.
					if( m_dwUpdatePointTime[eGameRoomType] == 0 )
					{
						m_dwUpdatePointTime[eGameRoomType] = 600000;
						g_Console.LOG( 4, "ERROR !! PCRoomInfo.bin - ProvidePoint/time. It must be non-zero.." );
					}
				}
				else if( stricmp( token, "add" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					m_dwUpdatePointValue[eGameRoomType] = DWORD( atoi( token ) );
				}
			}
			break;
		}
	}
}

void CPCRoomManager::Process()
{
	// 5분마다 체크
	static DWORD dwLastScriptCheckTime;
	if( gCurTime > dwLastScriptCheckTime + 60000*5 )
	{
		dwLastScriptCheckTime = gCurTime;

		// Script Update 검사
		char* filename = "./System/Resource/PCRoomInfo.bin";
		struct stat scriptStat = {0,};
		stat( filename, &scriptStat );
		if( memcmp( &m_ScriptFileStat, &scriptStat, sizeof(scriptStat) ) != 0 )
		{
			LoadScript();
		}
	}

	// 5초마다 체크
	static DWORD dwLastMallCheckTime;
	if( gCurTime > dwLastMallCheckTime + 5000 )
	{
		dwLastMallCheckTime = gCurTime;

		// PC방 상점이 열려있는 경우 Point를 갱신해준다
		stPCRoomUserInfo* pPCRoomUserInfo = NULL;
		m_PCRoomUser.SetPositionHead();
		while( (pPCRoomUserInfo = m_PCRoomUser.GetData()) != NULL )
		{
			if( !pPCRoomUserInfo->bOpenMall )
				continue;

			DWORD dwPoint = GetPoint( pPCRoomUserInfo->dwPlayerIdx );
			DWORD dwUpdatePoint = UpdatePoint( pPCRoomUserInfo->dwPlayerIdx );
			
			if( dwPoint != dwUpdatePoint )
			{
				// 변경된 포인트 전송
				MSG_DWORD2 msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_PCROOM;
				msg.Protocol = MP_PCROOM_CHANGE_POINT;
				msg.dwData1 = pPCRoomUserInfo->dwPlayerIdx;
				msg.dwData2 = dwUpdatePoint;

				CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pPCRoomUserInfo->dwPlayerIdx );
				if( !pPlayer || pPlayer->GetObjectKind() != eObjectKind_Player )
					continue;

				pPlayer->SendMsg( &msg, sizeof(msg) );
			}
		}
	}
}

void CPCRoomManager::ProvideItem( DWORD dwPlayerIdx )
{
	// 지급 가능 여부 검사
	stPCRoomUserInfo* pPCRoomUserInfo = m_PCRoomUser.GetData( dwPlayerIdx );
	if( !pPCRoomUserInfo ||
		!pPCRoomUserInfo->bCanProvideItem )
		return;

	// 요일파악
	__time64_t time;
	_time64( &time );
	struct tm timeWhen = {0,};
	timeWhen = *_localtime64( &time );
	int nWeekDay = timeWhen.tm_wday;

	// Script에서 지급 아이템 정보 설정
	DWORD dwItemIdx[ MAX_PCROOM_PROVIDE_ITEM_NUM ] = {0,};
	WORD wItemCnt[ MAX_PCROOM_PROVIDE_ITEM_NUM ] = {0,};
	ITEM_SEAL_TYPE wSealType[MAX_PCROOM_PROVIDE_ITEM_NUM];
	ZeroMemory(
		wSealType,
		sizeof(wSealType));
	__time64_t dwEndTime[ MAX_PCROOM_PROVIDE_ITEM_NUM ] = {0,};
	WORD wCnt = 0;

	stPCRoomItemInfo* pPCRoomItemInfo = NULL;
	m_htPCRoomItem[pPCRoomUserInfo->eGameRoomType].StartGetMultiData( nWeekDay );
	while( (pPCRoomItemInfo = m_htPCRoomItem[pPCRoomUserInfo->eGameRoomType].GetMultiData()) != NULL )
	{
		dwItemIdx[ wCnt ] = pPCRoomItemInfo->dwItemIdx;

		ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( dwItemIdx[ wCnt ] );
		if( !pItemInfo )
			continue;

		// 100609 ONS PC방아이템 시간설정 기능 추가
		if( eITEM_TYPE_GET_UNSEAL == pItemInfo->wSeal )
		{
			wSealType[ wCnt ] = eITEM_TYPE_UNSEAL;
			if( pItemInfo->nTimeKind == eKIND_REALTIME )
			{
				dwEndTime[ wCnt ] = time + pItemInfo->dwUseTime;
			}
			else
			{
				dwEndTime[ wCnt ] = time;
			}
		}
		else
		{
			wSealType[ wCnt ] = pItemInfo->wSeal;
			dwEndTime[ wCnt ] = time;
		}

		wItemCnt[ wCnt++ ] = (pItemInfo->Stack ? pPCRoomItemInfo->wItemCnt : 0);

		if( wCnt >= MAX_PCROOM_PROVIDE_ITEM_NUM )
			break;
	}

	char txt[1024] = {0,};
	sprintf(txt, "EXEC dbo.MP_GAMEROOM_PROVIDE_ITEM %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d ", 
		dwPlayerIdx,
		dwItemIdx[0], wItemCnt[0], wSealType[0], dwEndTime[0],
		dwItemIdx[1], wItemCnt[1], wSealType[1], dwEndTime[1], 
		dwItemIdx[2], wItemCnt[2], wSealType[2], dwEndTime[2],
		dwItemIdx[3], wItemCnt[3], wSealType[3], dwEndTime[3],
		dwItemIdx[4], wItemCnt[4], wSealType[4], dwEndTime[4]);
	g_DB.Query( eQueryType_FreeQuery, eProvidePCRoomItem, dwPlayerIdx, txt, dwPlayerIdx );

	pPCRoomUserInfo->bCanProvideItem = FALSE;
}

void CPCRoomManager::RemovePlayer( DWORD dwPlayerIdx )
{
	stPCRoomUserInfo* pPCRoomUserInfo = m_PCRoomUser.GetData( dwPlayerIdx );
	if( !pPCRoomUserInfo )
		return;

	UpdatePoint( dwPlayerIdx );

	// PC방 포인트가 변경된 경우
	if( pPCRoomUserInfo->dwLoginPoint != pPCRoomUserInfo->dwPoint )
	{
		g_DB.FreeMiddleQuery(
			0,
			dwPlayerIdx,
			"EXEC dbo.MP_GAMEROOM_UPDATE_POINT %d, %d",
			dwPlayerIdx,
			pPCRoomUserInfo->dwPoint);
		g_DB.LogMiddleQuery(
			0,
			0,
			"EXEC DBO.TP_GAMEROOM_POINT_LOG_INSERT %d, %d, %d",
			eLog_ItemNone,
			pPCRoomUserInfo->dwUserIdx,
			pPCRoomUserInfo->dwPoint);
	}

	// User정보 제거
	SAFE_DELETE( pPCRoomUserInfo );
	m_PCRoomUser.Remove( dwPlayerIdx );
	mPlayerPacketContainer.erase(
		dwPlayerIdx);
}

void CPCRoomManager::AddPlayer(DWORD dwPlayerIdx, DWORD userIndex, DWORD dwPoint, WORD wMemberCount, BOOL bCanProvideItem, BOOL bPrivededBuff, BYTE byDay)
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( dwPlayerIdx );
	if( !pPlayer || pPlayer->GetObjectKind() != eObjectKind_Player )
		return;

	// User정보 설정
	stPCRoomUserInfo* pPCRoomUserInfo = m_PCRoomUser.GetData( dwPlayerIdx );
	if( !pPCRoomUserInfo )
	{
		// 존재하지 않는 경우 생성
		pPCRoomUserInfo = new stPCRoomUserInfo;
		ZeroMemory( pPCRoomUserInfo, sizeof(stPCRoomUserInfo) );
		m_PCRoomUser.Add( pPCRoomUserInfo, dwPlayerIdx );
	}

	pPCRoomUserInfo->dwPCRoomIdx= GetGameRoomIndex(
		pPlayer->GetID());
	pPCRoomUserInfo->dwUserIdx					= userIndex;
	pPCRoomUserInfo->dwPlayerIdx				= dwPlayerIdx;
	pPCRoomUserInfo->dwLastProvidePointTime		= gCurTime;
	pPCRoomUserInfo->dwLoginPoint = pPCRoomUserInfo->dwPoint = dwPoint;
	pPCRoomUserInfo->bCanProvideItem			= bCanProvideItem;
	pPCRoomUserInfo->bOpenMall					= FALSE;
	pPCRoomUserInfo->bPCRoomBuffProvided		= bPrivededBuff;
	pPCRoomUserInfo->byDay						= byDay;

	// 100811 ONS PC방 유료/무료 가맹점 혜택 구분 처리 추가.
	const PlayerPacketContainer::const_iterator iterator = mPlayerPacketContainer.find(pPlayer->GetID());
	if(mPlayerPacketContainer.end() == iterator)
	{
		g_Console.LOG( 4, "ERROR !! PCRoomInfo.bin - Not found gameroom type!!" );
		return;
	}
	pPCRoomUserInfo->eGameRoomType				= GetGameRoomType( iterator->second );

	// 해당 PC방 파티원수를 설정한다.
	SetPartyMemberCountToOtherMap( pPCRoomUserInfo->dwPCRoomIdx, wMemberCount );

	if( m_htPCRoomItem[pPCRoomUserInfo->eGameRoomType].GetDataNum() != 0 )
	{
		// 아이템 지급 가능정보를 전송한다.
		MSG_BYTE msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category = MP_PCROOM;
		msg.Protocol = MP_PCROOM_CONNECT_INFO;
		msg.bData = (BYTE)bCanProvideItem;
		pPlayer->SendMsg( &msg, sizeof(msg) );
	}

	PCRoomBuffLoad(
		*pPlayer);
}



// 100105 ONS PC방버프를 로드한다.
void CPCRoomManager::PCRoomBuffLoad(CPlayer& player)
{
	stPCRoomUserInfo* const pPCRoomUserInfo = m_PCRoomUser.GetData(
		player.GetID());

	if(0 == pPCRoomUserInfo)
	{
		return;
	}

	std::vector<DWORD> vDailyBuffIndex;
	
	if( GetDailyBuffIndex( player.GetLevel(), vDailyBuffIndex, (pPCRoomUserInfo->bPCRoomBuffProvided) ? pPCRoomUserInfo->byDay : GetDayInfo(), pPCRoomUserInfo->eGameRoomType ) )
	{
		BuffSkillStart( pPCRoomUserInfo->dwPlayerIdx, vDailyBuffIndex );
		if( !pPCRoomUserInfo->bPCRoomBuffProvided )
		{
			// PC방 버프 정보를 전송한다.
			MSG_PCROOM_BUFF_INFO msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category	= MP_PCROOM;
			msg.Protocol	= MP_PCROOM_PROVIDE_DAILYBUFF;

			int nIndex = -1;
			
			for(std::vector<DWORD>::const_iterator iter = vDailyBuffIndex.begin();
				iter != vDailyBuffIndex.end();
				++iter)
			{
				msg.dwBuffIndex[++nIndex] = *iter;
			}

			player.SendMsg( &msg, sizeof(msg) );
		}
	}

	std::vector<DWORD> vPartyBuffIndex;
	
	if( GetPartyBuffIndex( pPCRoomUserInfo->dwPCRoomIdx, pPCRoomUserInfo->eGameRoomType, vPartyBuffIndex ) )
	{
		BuffSkillStart( pPCRoomUserInfo->dwPlayerIdx, vPartyBuffIndex );
		WORD wMemberCount = GetPartyMemberCount( pPCRoomUserInfo->dwPCRoomIdx );
		pPCRoomUserInfo->wBuffStage = GetPartyBuffStage( wMemberCount, pPCRoomUserInfo->eGameRoomType );

		if( !pPCRoomUserInfo->bPCRoomBuffProvided )
		{
			MSG_PCROOM_BUFF_INFO msg;
			// PC방 파티버프 정보를 전송한다.
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category	= MP_PCROOM;
			msg.Protocol	= MP_PCROOM_PROVIDE_PARTYBUFF;

			int nIndex = -1;
			
			for(std::vector<DWORD>::const_iterator iter = vPartyBuffIndex.begin();
				iter != vPartyBuffIndex.end();
				++iter)
			{
				msg.dwBuffIndex[++nIndex] = *iter;
			}

			msg.wStage = pPCRoomUserInfo->wBuffStage;
			msg.bIsChanged = FALSE;

			player.SendMsg( &msg, sizeof(msg) );
		}
	}

	if( !pPCRoomUserInfo->bPCRoomBuffProvided )
	{
		if( 0 != m_PCRoomPartyBuffList[pPCRoomUserInfo->eGameRoomType].GetCount() )
		{
			UpdatePartyBuffToOtherMap(
				player.GetID(),
				pPCRoomUserInfo->dwPCRoomIdx,
				pPCRoomUserInfo->eGameRoomType);
		}
		
		if( 0 != m_htPCRoomBuff[pPCRoomUserInfo->eGameRoomType].GetDataNum()	 ||
			0 != m_PCRoomPartyBuffList[pPCRoomUserInfo->eGameRoomType].GetCount() )
		{
			DBUpdatePCRoomBuff(
				pPCRoomUserInfo->dwPlayerIdx, 
				(WORD)eBuffInfoState_Update,
				pPCRoomUserInfo->wBuffStage,
				GetDayInfo());
		}
	}

	FriendGetPCRoomInfo( pPCRoomUserInfo->dwPlayerIdx, pPCRoomUserInfo->dwPCRoomIdx );
}

DWORD CPCRoomManager::GetPoint( DWORD dwPlayerIdx )
{
	stPCRoomUserInfo* pPCRoomUserInfo = m_PCRoomUser.GetData( dwPlayerIdx );
	if( !pPCRoomUserInfo )
		return 0;

	return pPCRoomUserInfo->dwPoint;
}

void CPCRoomManager::SetPoint( DWORD dwPlayerIdx, DWORD dwPoint )
{
	stPCRoomUserInfo* pPCRoomUserInfo = m_PCRoomUser.GetData( dwPlayerIdx );
	if( !pPCRoomUserInfo )
		return;

	pPCRoomUserInfo->dwPoint = dwPoint;
}

DWORD CPCRoomManager::UpdatePoint( DWORD dwPlayerIdx )
{
	stPCRoomUserInfo* pPCRoomUserInfo = m_PCRoomUser.GetData( dwPlayerIdx );
	if( !pPCRoomUserInfo || pPCRoomUserInfo->dwPCRoomIdx == 0 )
		return 0;

	DWORD dwPastTime = (gCurTime > pPCRoomUserInfo->dwLastProvidePointTime ? gCurTime - pPCRoomUserInfo->dwLastProvidePointTime : 0);

	DWORD dwAddPoint = (DWORD)(dwPastTime / m_dwUpdatePointTime[pPCRoomUserInfo->eGameRoomType] * m_dwUpdatePointValue[pPCRoomUserInfo->eGameRoomType]);
	DWORD dwRemainTime = DWORD(dwPastTime % m_dwUpdatePointTime[pPCRoomUserInfo->eGameRoomType]);

	// Update
	pPCRoomUserInfo->dwPoint = pPCRoomUserInfo->dwPoint + dwAddPoint;
	pPCRoomUserInfo->dwLastProvidePointTime = gCurTime - dwRemainTime;

	return pPCRoomUserInfo->dwPoint;
}

void CPCRoomManager::NetworkMsgParse( const MSGBASE* const pMsg, DWORD dwLength )
{
	switch( pMsg->Protocol )
	{
	case MP_PCROOM_PROVIDE_ITEM_SYN:
		{
			ProvideItem( pMsg->dwObjectID );
		}
		break;
	case MP_PCROOM_OPEN_MALL_SYN:
		{
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pMsg->dwObjectID );
			if( !pPlayer || pPlayer->GetObjectKind() != eObjectKind_Player )
				break;

			MSG_DWORD3 msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_PCROOM;
			msg.Protocol = MP_PCROOM_OPEN_MALL_ACK;
			msg.dwData1 = pMsg->dwObjectID;
			msg.dwData2 = m_wMallNPCIdx;
			msg.dwData3 = UpdatePoint( pMsg->dwObjectID );

			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
		break;
	case MP_PCROOM_CLOSE_MALL:
		{
			stPCRoomUserInfo* pPCRoomUserInfo = m_PCRoomUser.GetData( pMsg->dwObjectID );
			if( !pPCRoomUserInfo )
				break;

			pPCRoomUserInfo->bOpenMall = FALSE;
		}
		break;
	// 100312 ONS PC방 파티원리스트 정보를 다른맵에 전달한다.
	case MP_PCROOM_INFO_SYN_OTHERMAP:
		{
			MSG_PCROOM_MEMBER_LIST* pmsg = (MSG_PCROOM_MEMBER_LIST*)pMsg;

			UpdateRCRoomMemberList( pmsg );
		}
		break;
	// 100312 ONS PC방 파티버프정보를 다른맵에 전달한다.
	case MP_PCROOM_UPDATE_PARTYBUFF_OTHERMAP:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			const GameRoomType GameRoomType = PCROOMMGR->GetUserGameRoomType( pmsg->dwObjectID );
			if( GameRoomIsInvalid != GameRoomType ) 
			{
				UpdatePartyBuff( pmsg->dwObjectID, pmsg->dwData, GameRoomType );
			}
		}
		break;
	// 100312 ONS PC방 파티원수를 갱신한다.
	case MP_PCROOM_SETMEMBER_COUNT_OTHERMAP:
		{
			MSG_DWORD_WORD* pmsg = (MSG_DWORD_WORD*)pMsg;

			SetPartyMemberCount( pmsg->dwData, pmsg->wData );
		}
		break;
	}
}

// 100105 ONS PC방버프를 부여한다.
void CPCRoomManager::BuffSkillStart( DWORD dwPlayerIdx, std::vector<DWORD> BuffIndex )
{
	std::vector<DWORD>::const_iterator iter;
	for( iter = BuffIndex.begin(); iter != BuffIndex.end(); iter++)
	{
		const DWORD dwBuffIndex = *iter;
		
		if(const cBuffSkillInfo* const buffSkillInfo = SKILLMGR->GetBuffInfo( dwBuffIndex ))
		{
			SKILLMGR->BuffSkillStart(
				dwPlayerIdx,
				dwBuffIndex,
				max(1, buffSkillInfo->GetInfo().DelayTime),
				buffSkillInfo->GetInfo().Count );
		}
	}
}

// 100105 ONS PC방 혜택 단계와 인원수를 구한다. 
WORD CPCRoomManager::GetMinimumOfStage( WORD wPartyStage, GameRoomType GameRoomType )
{
	WORD wCount = 0;
	WORD wLimitMinimum = 0;
	PTRLISTPOS pos = NULL;
	pos = m_PCRoomPartyBuffList[GameRoomType].GetHeadPosition();
	while( pos )
	{
		stPCRoomPartyBuffInfo* pPCRoomPartyBuffInfo = NULL;
		pPCRoomPartyBuffInfo = (stPCRoomPartyBuffInfo*)m_PCRoomPartyBuffList[GameRoomType].GetNext(pos);
		if( !pPCRoomPartyBuffInfo )
			continue;

		// PC방 인원수 체크
		if( wPartyStage == wCount++ )
		{
			wLimitMinimum = pPCRoomPartyBuffInfo->wConnectedNumMin;
			break;
		}
	}

	return wLimitMinimum;
}

// 100105 ONS PC방지급버프/파티버프를 제거한다.
void CPCRoomManager::RemovePCRoomBuff( DWORD dwPlayerIdx )
{
	stPCRoomUserInfo* pPCRoomUserInfo = NULL;
	m_PCRoomUser.SetPositionHead();
	while( (pPCRoomUserInfo = m_PCRoomUser.GetData()) != NULL )
	{
		if( pPCRoomUserInfo->dwPlayerIdx != dwPlayerIdx )
			continue;

		// PC방 파티원이 제거될때 파티버프를 갱신한다.
		WORD wMemberCount = GetPartyMemberCount( pPCRoomUserInfo->dwPCRoomIdx );
		SetPartyMemberCountToOtherMap( pPCRoomUserInfo->dwPCRoomIdx, wMemberCount - 1 );
		UpdatePartyBuffToOtherMap( dwPlayerIdx, pPCRoomUserInfo->dwPCRoomIdx, pPCRoomUserInfo->eGameRoomType );

		DBUpdatePCRoomBuff( dwPlayerIdx, (WORD)eBuffInfoState_End);

		// PC방리스트를 갱신한다.
		FriendGetPCRoomInfo( pPCRoomUserInfo->dwPlayerIdx, pPCRoomUserInfo->dwPCRoomIdx );
	}
}


void CPCRoomManager::RemoveBuff( DWORD dwPlayerIdx, DWORD dwBuffIndex )
{
	CPlayer* const pPlayer = ( CPlayer* )g_pUserTable->FindUser(dwPlayerIdx);
	if(0 == pPlayer)
	{
		return;
	}

	pPlayer->GetBuffList().SetPositionHead();

	while(cBuffSkillObject* const skillObject = pPlayer->GetBuffList().GetData())
	{
		if(dwBuffIndex == skillObject->GetSkillIdx())
		{
			skillObject->SetEndState();
			skillObject->EndState();	
		}
	}
}

// 100312 ONS 각맵의 PC방 파티원수를 갱신한다.
void CPCRoomManager::SetPartyMemberCountToOtherMap( DWORD dwPartyIdx, WORD wMemberCount )
{
	SetPartyMemberCount( dwPartyIdx, wMemberCount );

	MSG_DWORD_WORD msg;
	ZeroMemory( &msg, sizeof(msg) );
	msg.Category = MP_PCROOM;
	msg.Protocol = MP_PCROOM_SETMEMBER_COUNT_OTHERMAP;
	msg.dwData = dwPartyIdx;
	msg.wData = wMemberCount;

	PACKEDDATA_OBJ->SendToBroadCastMapServer( &msg, sizeof(msg) );
}

void CPCRoomManager::SetPartyMemberCount( DWORD dwPartyIdx, WORD wMemberCount )
{
	m_PCRoomMemberMap[ dwPartyIdx ] = wMemberCount;
}

WORD CPCRoomManager::GetPartyMemberCount( DWORD dwPartyIdx )
{
	return m_PCRoomMemberMap[ dwPartyIdx ] ? m_PCRoomMemberMap[ dwPartyIdx ] : 0;
}

// 100312 ONS 현재 파티원수를 통해서 버프혜택단계를 가져온다.
WORD CPCRoomManager::GetPartyBuffStage( WORD wCurCount, GameRoomType GameRoomType )
{
	WORD nStage = 0;
	PTRLISTPOS pos = NULL;
	pos = m_PCRoomPartyBuffList[GameRoomType].GetHeadPosition();
	while( pos )
	{
		stPCRoomPartyBuffInfo* pPCRoomPartyBuffInfo = NULL;
		pPCRoomPartyBuffInfo = (stPCRoomPartyBuffInfo*)m_PCRoomPartyBuffList[GameRoomType].GetNext(pos);
		if( !pPCRoomPartyBuffInfo )
			continue;

		// PC방 인원수 체크
		const WORD wConnectedMin = pPCRoomPartyBuffInfo->wConnectedNumMin;
		const WORD wConnectedMax = pPCRoomPartyBuffInfo->wConnectedNumMax;
		if( (( wCurCount >= wConnectedMin) && (wCurCount <= wConnectedMax)) )
		{
			break;
		}
		nStage++;
	}

	return nStage;
}

// 100312 ONS 파티버프 갱신을 다른 맵에도 전달한다.
void CPCRoomManager::UpdatePartyBuffToOtherMap( DWORD dwPlayerIdx, DWORD dwPCRoomIdx, GameRoomType GameRoomType )
{
	UpdatePartyBuff( dwPlayerIdx, dwPCRoomIdx, GameRoomType);

	MSG_DWORD msg;
	ZeroMemory( &msg, sizeof(msg) );
	msg.Category = MP_PCROOM;
	msg.Protocol = MP_PCROOM_UPDATE_PARTYBUFF_OTHERMAP;
	msg.dwObjectID = dwPlayerIdx;
	msg.dwData = dwPCRoomIdx;

	PACKEDDATA_OBJ->SendToBroadCastMapServer( &msg, sizeof(msg) );
}

// 100312 ONS 파티버프를 갱신한다.
void CPCRoomManager::UpdatePartyBuff( DWORD dwPlayerIdx, DWORD dwPCRoomIdx, GameRoomType GameRoomType )
{
	WORD wMemberCount = GetPartyMemberCount( dwPCRoomIdx );
	WORD wPartyStage = GetPartyBuffStage( wMemberCount, GameRoomType );

	stPCRoomUserInfo* pPCRoomUserInfo = NULL;
	m_PCRoomUser.SetPositionHead();
	while( (pPCRoomUserInfo = m_PCRoomUser.GetData()) != NULL )
	{
		if( pPCRoomUserInfo->dwPCRoomIdx != dwPCRoomIdx )
			continue;
		else if( pPCRoomUserInfo->wBuffStage == wPartyStage )
			continue;
		
		std::vector<DWORD> RemoveBuffIndex;
		RemoveBuffIndex.clear();
		if(GetPartyBuffIndexByStage( pPCRoomUserInfo->wBuffStage, RemoveBuffIndex, pPCRoomUserInfo->eGameRoomType ))
		{
			std::vector<DWORD>::const_iterator iter;
			for( iter = RemoveBuffIndex.begin(); iter != RemoveBuffIndex.end(); iter++)
			{
				DWORD dwIndex = *iter;
				if( 0 == dwIndex )
					continue;

				RemoveBuff( pPCRoomUserInfo->dwPlayerIdx, dwIndex );
			}
		}

		std::vector<DWORD> UpdateBuffIndex;
		UpdateBuffIndex.clear();
		if(GetPartyBuffIndexByStage( wPartyStage, UpdateBuffIndex, pPCRoomUserInfo->eGameRoomType ))
		{
			BuffSkillStart( pPCRoomUserInfo->dwPlayerIdx, UpdateBuffIndex );
			pPCRoomUserInfo->wBuffStage = wPartyStage;
		}

		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pPCRoomUserInfo->dwPlayerIdx );
		if( pPlayer && pPCRoomUserInfo->dwPlayerIdx != dwPlayerIdx )
		{
			MSG_PCROOM_BUFF_INFO msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category	= MP_PCROOM;
			msg.Protocol	= MP_PCROOM_PROVIDE_PARTYBUFF;

			int nIndex = 0;
			std::vector<DWORD>::const_iterator iter;
			for( iter = UpdateBuffIndex.begin(); iter != UpdateBuffIndex.end(); iter++)
			{
				msg.dwBuffIndex[nIndex++] = *iter;
			}

			msg.wStage		= wPartyStage;
			msg.bIsChanged	= TRUE;
			pPlayer->SendMsg( &msg, sizeof(msg) );	
		}
	}
}

BYTE CPCRoomManager::GetDayInfo()
{
	// 요일정보 설정
	__time64_t time = 0;
	struct tm timeWhen = {0, };
	_time64( &time );
	timeWhen = *_localtime64( &time );

	return (BYTE)timeWhen.tm_wday;
}

// 100312 ONS 플레이어의 레벨과 날짜를 통해서 버프인덱스를 가져온다.
BOOL CPCRoomManager::GetDailyBuffIndex( LEVELTYPE PlayerLevel, std::vector<DWORD>& pBuffIndex, BYTE byDay, GameRoomType GameRoomType )
{
	BOOL bResult = FALSE;
	stPCRoomBuffInfo* pPCRoomBuffInfo = NULL;
	m_htPCRoomBuff[GameRoomType].StartGetMultiData( byDay );
	while( (pPCRoomBuffInfo = m_htPCRoomBuff[GameRoomType].GetMultiData()) != NULL )
	{
		// 게임 로그인시 플레이어의 레벨에 따른 버프를 설정한다.
		const LEVELTYPE minLevel = pPCRoomBuffInfo->minLv;
		const LEVELTYPE maxLevel = pPCRoomBuffInfo->maxLv;
		if( !(( PlayerLevel >= minLevel) && (PlayerLevel <= maxLevel)) )
		{
			continue;
		}

		pBuffIndex = pPCRoomBuffInfo->vecBuffIdx;
		bResult = TRUE;
		break;
	}

	return bResult;
}

// 100312 ONS PC방인덱스를 통해 해당버프 인덱스를 가져온다.
BOOL CPCRoomManager::GetPartyBuffIndex( DWORD dwPCRoomIdx, GameRoomType GameRoomType, std::vector<DWORD>& pBuffIndex )
{
	WORD wMemberCount = GetPartyMemberCount( dwPCRoomIdx );
	WORD wPartyStage = GetPartyBuffStage( wMemberCount, GameRoomType );
	BOOL bResult = FALSE;

	int nCount = 0;
	PTRLISTPOS pos = NULL;
	pos = m_PCRoomPartyBuffList[GameRoomType].GetHeadPosition();
	while( pos )
	{
		stPCRoomPartyBuffInfo* pPCRoomPartyBuffInfo = NULL;
		pPCRoomPartyBuffInfo = (stPCRoomPartyBuffInfo*)m_PCRoomPartyBuffList[GameRoomType].GetNext(pos);
		if( !pPCRoomPartyBuffInfo )
			continue;
		else if( wPartyStage > nCount++ )
			continue;

		pBuffIndex = pPCRoomPartyBuffInfo->vecBuffIdx;
		bResult = TRUE;
		break;
	}

	return bResult;
}


// 100312 ONS 파티혜택 단계수를 통해서 해당 버프인덱스를 가져온다.
BOOL CPCRoomManager::GetPartyBuffIndexByStage( WORD wPartyStage, std::vector<DWORD>& pBuffIndex, GameRoomType GameRoomType )
{
	BOOL bResult = FALSE;
	int nCount = 0;
	PTRLISTPOS pos = NULL;
	pos = m_PCRoomPartyBuffList[GameRoomType].GetHeadPosition();
	while( pos )
	{
		stPCRoomPartyBuffInfo* pPCRoomPartyBuffInfo = NULL;
		pPCRoomPartyBuffInfo = (stPCRoomPartyBuffInfo*)m_PCRoomPartyBuffList[GameRoomType].GetNext(pos);
		if( !pPCRoomPartyBuffInfo )
			continue;
		else if( wPartyStage > nCount++ )
			continue;

		pBuffIndex = pPCRoomPartyBuffInfo->vecBuffIdx;
		bResult = TRUE;
		break;
	}

	return bResult;
}

// 100312 ONS PC방파티원 리스트를 갱신한다.
void CPCRoomManager::UpdateRCRoomMemberList( MSG_PCROOM_MEMBER_LIST* pMsg )
{
	const BYTE dwCount = pMsg->count;

	for( BYTE i = 0; i < dwCount; i++ )
	{
		DWORD dwIndex = pMsg->PCRoomMemList[i].CharIndex;
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( dwIndex );
		if( NULL == pPlayer )
			continue;

		MSG_PCROOM_MEMBER_LIST msg;
		memcpy(&msg, pMsg, sizeof(msg));
		
		msg.Protocol	= MP_PCROOM_INFO_ACK;

		pPlayer->SendMsg( &msg, sizeof(msg) );
	}
}

CPCRoomManager* CPCRoomManager::GetInstance()
{
	static CPCRoomManager instance;

	return &instance;
}

void CPCRoomManager::SetPacketGame(PlayerIndex playerIndex, const _PACKET_GAME& packetGame)
{
	mPlayerPacketContainer[playerIndex] = packetGame;
}

void CPCRoomManager::RemovePacketGame(PlayerIndex playerIndex)
{
	mPlayerPacketContainer.erase(
		playerIndex);
}

DWORD CPCRoomManager::GetGameRoomIndex(PlayerIndex playerIndex) const
{
	const PlayerPacketContainer::const_iterator iterator = mPlayerPacketContainer.find(
		playerIndex);

	if(mPlayerPacketContainer.end() == iterator)
	{
		return 0;
	}

	const _PACKET_GAME& packetGame = iterator->second;

	if( GameRoomIsInvalid == GetGameRoomType( packetGame ) )
	{
		return 0;
	}

	return packetGame.User_Age;
}

GameRoomType CPCRoomManager::GetUserGameRoomType( DWORD playerIndex )
{
	stPCRoomUserInfo* pPCRoomUserInfo = m_PCRoomUser.GetData( playerIndex );
	if( 0 == pPCRoomUserInfo )
	{
		return GameRoomIsInvalid;
	}

	return pPCRoomUserInfo->eGameRoomType;
}
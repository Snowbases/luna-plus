#include "stdafx.h"
#include "MSSystem.h"
#include "Network.h"
#include "DBMessage.h"
#include "UserManager.h"
#include "server.h"
#include "..\common\version.h"


bool CMSSystem::mIsRunning;


CMSSystem::CMSSystem()
{
	Start();
}

CMSSystem::~CMSSystem()
{
	mIsRunning = false;

	End();
}

void CMSSystem::Start()
{
	char host[128],ipName[2][20];
	HOSTENT* hostent;

	WSADATA wsaData ;
	WSAStartup(0x0002,&wsaData);
	gethostname(host,128);
	hostent=gethostbyname(host);

	for(int n=0;n<2;++n)
	{
		if(hostent->h_addr_list[n] == NULL)
			break;

		strncpy(ipName[n],inet_ntoa(*(struct in_addr *)hostent->h_addr_list[n]),20);
	}

	WSACleanup();

	CUSTOM_EVENT ev[1] = { 0 };
	{
		ev[0].dwPeriodicTime	= 1;
		ev[0].pEventFunc		= CMSSystem::Process;
	}	

	DESC_NETWORK desc = { 0 };
	{
		desc.OnAcceptServer						= NULL;
		desc.OnDisconnectServer					= NULL;
		desc.OnAcceptUser						= OnAcceptUser;
		desc.OnDisconnectUser					= OnDisconnectUser;
		desc.OnRecvFromServerTCP				= NULL;
		desc.OnRecvFromUserTCP					= ReceivedMsgFromRMTool;
		desc.dwCustomDefineEventNum 			= sizeof( ev ) / sizeof( *ev );
		desc.pEvent								= ev;

		desc.dwMainMsgQueMaxBufferSize			= 512000;
		desc.dwMaxServerNum						= 0;
		desc.dwMaxUserNum						= 50;
		desc.dwServerBufferSizePerConnection	= 0;
		desc.dwServerMaxTransferSize			= 0;
		desc.dwUserBufferSizePerConnection		= 256000;
		desc.dwUserMaxTransferSize				= 65000;
		desc.dwConnectNumAtSameTime				= 100;
		desc.dwFlag								= NULL;
	}

	bool isSucceed = true;

	{
		printf( "... initializing network" );

		if( NETWORK->Init(&desc) )
		{
			printf( " [OK]\n" );
		}
		else
		{
			printf( " [FAIL]\n" );

			isSucceed	= false;
		}		
	}

	{
		printf( "... starting server" );

		if( NETWORK->StartServer() )
		{
			printf( " [OK]\n" );
		}
		else
		{
			printf( " [FAIL]\n" );

			isSucceed	= false;
		}		
	}

	{
		// 081009 LUJ, 쿼리가 연속적으로 실행될 수 있도록 DB 스레드 개수를 1 -> 4개로 변경
		if( g_DB.Init( 4, 1024, true ) )
		{
			printf( "... initializing database [OK]\n" );
		}
		else
		{
			printf( "... initializing database [FAIL]\n" );

			isSucceed	= false;
		}
	}

	mIsRunning = isSucceed;
}


void CMSSystem::End()
{
	//mIsRunning = false;
	//m_bInit = false;

	{
		printf( "... releasing database" );

		g_DB.Release();

		printf( " [OK]\n" );
	}
	
	{
		printf( "... releasing network\n" );

		NETWORK->Release();

		printf( " [OK]\n" );
	}
	
	{
		printf( "... releasing unused library" );

		CoFreeUnusedLibraries();

		printf( " [OK]\n" );
	}
}


void __stdcall CMSSystem::Process( DWORD eventIndex )
{
	if( mIsRunning )
	{
		g_DB.ProcessingDBMessage();
	}
}


void __stdcall CMSSystem::OnAcceptServer(DWORD connectionIndex)
{}


void __stdcall CMSSystem::OnDisconnectServer(DWORD connectionIndex)
{}


void __stdcall CMSSystem::ReceivedMsgFromServer(DWORD connectionIndex,char* pMsg,DWORD dwLength)
{}


void __stdcall CMSSystem::OnAcceptUser(DWORD connectionIndex)
{
	//CMSSystem* system = g_pMSSystem;
	//ASSERT( system );

	//system->mConnectionSet.insert( connectionIndex );
}


void __stdcall CMSSystem::OnDisconnectUser(DWORD connectionIndex )
{
	CUserManager::GetInstance().RemoveUser( connectionIndex );
}


void __stdcall CMSSystem::ReceivedMsgFromRMTool( DWORD connectionIndex,char* pMsg,DWORD size )
{
	const MSGROOT* message = (MSGROOT*)pMsg;

	if( MP_RM_CONNECT == message->Category )
	{
		ConnentMsgParse( connectionIndex, pMsg, size );
		return;
	}

	switch( message->Category )
	{
	case MP_RM_SEARCH:
		{
			ParseSearch( connectionIndex, message, size );
			break;
		}
	case MP_RM_PLAYER:
		{
			ParsePlayer( connectionIndex, message, size );
			break;
		}
	case MP_RM_ITEM:
		{
			ParseItem( connectionIndex, message, size );
			break;
		}
	case MP_RM_ITEM_FIND:
		{
			ParseItemFind( connectionIndex, message, size );
			break;
		}
	case MP_RM_ITEM_LOG:
		{
			ParseItemLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_ITEM_OPTION_LOG:
		{
			ParseItemOptionLog( connectionIndex, message, size );
			break;
		}
		// 080320 LUJ, 드롭 옵션 로그
	case MP_RM_ITEM_DROP_OPTION_LOG:
		{
			ParseItemDropOptionLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_SKILL:
		{
			ParseSkill( connectionIndex, message, size );
			break;
		}
	case MP_RM_SKILL_LOG:
		{
			ParseSkillLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_FAMILY:
		{
			ParseFamily( connectionIndex, message, size );
			break;
		}
	case MP_RM_QUEST:
		{
			ParseQuest( connectionIndex, message, size );
			break;
		}
	case MP_RM_QUEST_LOG:
		{
			ParseQuestLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_GUILD:
		{
			ParseGuild( connectionIndex, message, size );
			break;
		}
	case MP_RM_GUILD_LOG:
		{
			ParseGuildLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_GUILD_WAREHOUSE_LOG:
		{
			ParseGuildWarehouseLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_EXP_LOG:	
		{
			ParseExperienceLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_STAT_LOG:	
		{
			ParseStatLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_USER:
		{
			ParseUser( connectionIndex, message, size );
			break;
		}
	case MP_RM_JOB_LOG:
		{
			ParseJobLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_GUILD_SCORE_LOG:
		{
			ParseGuildScoreLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_FAMILY_CROP_LOG:
		{
			ParseFamilyCropLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_FAMILY_POINT_LOG:
		{
			ParseFamilyPointLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_PERMISSION:
		{
			ParsePermission( connectionIndex, message, size );
			break;
		}
	case MP_RM_OPERATOR:
		{
			ParseOperator( connectionIndex, message, size );
			break;
		}
	case MP_RM_OPERATOR_LOG:
		{
			ParseOperatorLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_PASSWORD:
		{
			ParsePassword( connectionIndex, message, size );
			break;
		}
	case MP_RM_NAME_LOG:
		{
			ParseNameLog( connectionIndex, message, size );
			break;
		}
		// 080401 LUJ, 농장 로그
	case MP_RM_FARM_LOG:
		{
			ParseFarmLog( connectionIndex, message, size );
			break;
		}
		// 080630 LUJ, 농장 가축 로그
	case MP_RM_LIVESTOCK_LOG:
		{
			ParseLivestockLog( connectionIndex, message, size );
			break;
		}
		// 080716 LUJ, 펫 로그
	case MP_RM_PET_LOG:
		{
			ParsePetLog( connectionIndex, message, size );
			break;
		}
		// 080403 LUJ, 유료 아이템 구입 로그
	case MP_RM_ITEM_SHOP_LOG:
		{
			ParseItemShopLog( connectionIndex, message, size );
			break;
		}
		// 080523 LUJ, 낚시 로그
	case MP_RM_FISH_LOG:
		{
			ParseFishLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_ITEM_GLOBAL_LOG:
		{
			ParseItemGlobalLog( connectionIndex, message, size );
			break;
		}
		// 081021 LUJ, 권한 관리
	case MP_RM_AUTHORITY:
		{
			ParseAuthority( connectionIndex, message, size );
			break;
		}
		// 081027 LUJ, 토너먼트
	case MP_RM_TOURNAMENT:
		{
			ParseTournament( connectionIndex, message, size );
			break;
		}
		// 081027 LUJ, 공성 로그
	case MP_RM_SIEGE_LOG:
		{
			ParseSiegeLog( connectionIndex, message, size );
			break;
		}
		// 081205 LUJ, 채팅 로그
	case MP_RM_CHAT_LOG:
		{
			ParseChatLog( connectionIndex, message, size );
			break;
		}
		// 081205 LUJ, 패밀리 로그
	case MP_RM_FAMILY_LOG:
		{
			ParseFamilyLog( connectionIndex, message, size );
			break;
		}
		// 090122 LUJ, 스크립트 변조 로그
	case MP_RM_SCRIPT_HACK_LOG:
		{
			ParseScriptHackLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_MONEY_LOG:
		{
			ParseMoneyLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_AUTONOTE_LOG:
		{
			ParseAutoNoteLog( connectionIndex, message, size );
			break;
		}
	case MP_RM_HOUSE_LOG:
		{
			ParseHouseLog(
				connectionIndex,
				message,
				size);
			break;
		}
	case MP_RM_COOK_LOG:
		{
			ParseCookLog(
				connectionIndex,
				message,
				size);
			break;
		}
	case MP_RM_CONSIGN_LOG:
		{
			ParseConsignLog(
				connectionIndex,
				message,
				size);
			break;
		}
	case MP_RM_GAMEROOM_POINT_LOG:
		{
			ParseGameRoomPointLog(
				connectionIndex,
				message,
				size);
			break;
		}
	case MP_RM_FORBID_CHAT_LOG:
		{
			ParseForbidChatLog(
				connectionIndex,
				message,
				size);
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}



bool CMSSystem::IsRunning()
{
	return mIsRunning;
}


void CMSSystem::ConnentMsgParse(DWORD connectionIndex,char* pMsg, size_t dwLength)
{
	MSGROOT* pTempMsg = (MSGROOT*)pMsg;

	switch(pTempMsg->Protocol)
	{
	case MP_RM_CONNECT_SYN:
		{
			const MSG_RM_LOGIN* m = ( MSG_RM_LOGIN* )pMsg;

			TCHAR ip[ MAX_NAME_LENGTH + 1 ] = { 0 };
			WORD port;
			NETWORK->GetUserAddress( connectionIndex, ip, &port );

			// 080311 踰꾩쟾 鍮꾧탳
			if( MAJORVERSION != m->mVersion.mMajor ||
				MINORVERSION != m->mVersion.mMinor )
			{
				MSG_INT2 message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_RM_CONNECT;
				message.Protocol	= MP_RM_CONNECT_NACK_BY_VERSION;
				message.nData1		= MAJORVERSION;
				message.nData2		= MINORVERSION;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
				return;
			}

#ifdef _DEBUG
			// 081021 LUJ, 릴리즈 서버는 중복 접속을 차단한다
			// 081027 LUJ, 대만 요청으로 중복 접속을 허용. 디버그 버전에서만 체크 기능을 놔두도록 한다
			// 081127 LUJ, 중복 접속 허용토록 체크 코드 제거
			if( CUserManager::GetInstance().IsConnectedIp( ip ) )
			{
				PutLog(
					"%s connected already from %s",
					m->mId,
					ip );
			}
			else if( CUserManager::GetInstance().IsConnectedUser( m->mId ) )				
			{
				PutLog(
					"%s connected already.",
					m->mId );

				MSGBASE message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_RM_CONNECT;
				message.Protocol	= MP_RM_CONNECT_NACK_BY_ID;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
				break;
			}
#endif
			PutLog( "%s tried log in on %s", m->mId, ip );
			g_DB.LoginMiddleQuery(
				RLoginOperator,
				connectionIndex,
				"EXEC dbo.RP_OPERATORLOGINCHECK \'%s\', \'%s\', \'%s\'",
				m->mId,
				m->mPass,
				ip );
		}
	}
}


bool CMSSystem::IsStopQuery( DWORD connectionIndex ) const
{
	return mStopQuerySet.end() != mStopQuerySet.find( connectionIndex );
}


void CMSSystem::StopQuery( DWORD connectionIndex, RecoveryCategory category, BYTE protocol )
{
	mStopQuerySet.erase( connectionIndex );

	// 쿼리가 스톱되었음을 알린다
	MSGROOT message;

	message.Category	= category;
	message.Protocol	= protocol;

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}


void CMSSystem::ParseSearch( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	switch( message->Protocol )
	{
	case MP_RM_SEARCH_SYN:
		{
			const MSG_RM_OPEN_SYN* m = ( MSG_RM_OPEN_SYN* )message;

			switch( m->mType )
			{
			case MSG_RM_OPEN_SYN::PlayerName:
				{
					g_DB.FreeMiddleQuery(
						RGetUserIndex,
						connectionIndex,
						"EXEC dbo.TP_USER_INDEX_SELECT %d, \'%s\'",
						0,
						m->mKeyword );

					break;
				}
			case MSG_RM_OPEN_SYN::PlayerIndex:
				{
					g_DB.FreeMiddleQuery(
						RGetUserIndex,
						connectionIndex,
						"EXEC dbo.TP_USER_INDEX_SELECT %d, \'%s\'",
						atoi( m->mKeyword ),
						"" );

					break;
				}
			case MSG_RM_OPEN_SYN::LoginId:
				{
					g_DB.LoginMiddleQuery(
						RGetUserList,
						connectionIndex,
						"EXEC dbo.TP_USER_SELECT %d, \'%s\', \'%s\'",
						0,
						m->mKeyword,
						"" );

					break;
				}

			case MSG_RM_OPEN_SYN::RealName:
				{
					g_DB.LoginMiddleQuery(
						RGetUserList,
						connectionIndex,
						"EXEC dbo.TP_USER_SELECT %d, \'%s\', \'%s\'",
						0,
						"",
						m->mKeyword );

					break;
				}
				// 090122 LUJ, 사용자 번호로 검색
			case MSG_RM_OPEN_SYN::UserIndex:
				{
					g_DB.LoginMiddleQuery(
						RGetUserList,
						connectionIndex,
						"EXEC dbo.TP_USER_SELECT %d, \'%s\', \'%s\'",
						atoi( m->mKeyword ),
						"",
						"" );
					break;
				}
			}

			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParsePlayer( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_PLAYER_GET_DATA_SYN:
		{
			const MSG_RM_GET_PLAYER_SYN* m = ( MSG_RM_GET_PLAYER_SYN* )message;

			// 080725 LUJ, 서비스/테스트 서버 간의 데이터 저장 위치가 다른 것이 있어 프로시저로 추출함
			g_DB.FreeQuery(
				RGetPlayerData,
				connectionIndex,
				"EXEC dbo.TP_CHARACTER_DATA_SELECT %d",
				m->mPlayerIndex );

			g_DB.FreeMiddleQuery(
				RGetPlayerLicense,
				connectionIndex,
				"SELECT PlayerID, Sex, Age, Location, Favor,\
				PropensityLike01, PropensityLike02, PropensityLike03, PropensityDisLike01, PropensityDisLike02\
				FROM    TB_RESIDENTREGIST\
				WHERE   (PlayerID = %d)",
				m->mPlayerIndex );

			g_DB.FreeMiddleQuery(
				RGetPlayerExtendedData,
				connectionIndex,
				"SELECT A.CHARACTER_NAME, A.CHARACTER_NICKNAME, ISNULL(C.MemberNickname, \'\') AS FAMILY_NICK,\
				A.CHARACTER_LASTMODIFIED, A.CHARACTER_GENDER, CHARACTER_STAGE, B.FLAG, B.CREATE_DATE,B.CREATE_IP, B.DELETE_DATE,\
				B.DELETE_IP, B.RECOVER_DATE, B.RECOVER_IP,M.PLAYTIMETOTAL, M.KILLMONNUMTOTAL\
				FROM TB_CHARACTER A INNER JOIN TB_CharacterCreateInfo B ON A.CHARACTER_IDX = B.CHARACTER_IDX\
				LEFT OUTER JOIN TB_FAMILY_MEMBER C ON A.CHARACTER_IDX = C.MemberID\
				INNER JOIN TB_MONSTERMETER M ON A.CHARACTER_IDX = M.PLAYERID\
				WHERE   (A.CHARACTER_IDX = %d)",
				m->mPlayerIndex );

			// 080328 LUJ, 패밀리 이름을 쿼리한다
			g_DB.FreeMiddleQuery(
				RGetPlayerFamilyData,
				connectionIndex,
				"SELECT	A.ID, A.NAME\
				FROM	TB_FAMILY A INNER JOIN TB_FAMILY_MEMBER B ON A.ID = B.FAMILYID WHERE B.MEMBERID = %d",
				m->mPlayerIndex );

			// 080526 LUJ, 낚시 정보를 쿼리한다
			g_DB.FreeMiddleQuery(
				RGetFishingData,
				connectionIndex,
				"SELECT CHARACTER_IDX, FISHING_LEVEL, FISHING_EXP, FISH_POINT\
				FROM TB_FISHING WHERE CHARACTER_IDX = %d",
				m->mPlayerIndex);
			g_DB.FreeMiddleQuery(
				RGetCookData,
				connectionIndex,
				"SELECT CHARACTER_IDX, COOK_LEVEL, COOK_COUNT, EAT_COUNT, FIRE_COUNT\
				FROM TB_COOKING WHERE CHARACTER_IDX = %d",
				m->mPlayerIndex);
			g_DB.FreeMiddleQuery(
				RGetCookRecipeData,
				connectionIndex,
				"SELECT CHARACTER_IDX, RECIPEIDX, SLOT, REMAINTIME\
				FROM TB_COOKRECIPE WHERE CHARACTER_IDX = %d",
				m->mPlayerIndex);
			g_DB.FreeMiddleQuery(
				RGetHouseData,
				connectionIndex,
				"SELECT HOUSE_NAME, EXTERIOR_KIND, EXTEND_LEVEL, TOTAL_VISITCOUNT, DAILY_VISITCOUNT, DECO_USEPOINT, HOUSE_POINT, UPDATEDATE\
				FROM TB_HOUSE H INNER JOIN TB_CHARACTER C ON H.USER_INDEX = C.USER_IDX WHERE C.CHARACTER_IDX = %d",
				m->mPlayerIndex);
			g_DB.FreeQuery(
				RGetNoteList,
				connectionIndex,
				"SELECT Note_IDX, CONVERT(VARCHAR(6), SendDate, 12) + \' \' + CONVERT(VARCHAR(8), SendDate, 8),\
				SUBSTRING(Title, 0, 16), SUBSTRING(Title, 16, 16)\
				FROM TB_NOTE WHERE Receiver_IDX = %d ORDER BY Note_IDX DESC",
				m->mPlayerIndex);
			g_DB.FreeMiddleQuery(
				RGetPartyList,
				connectionIndex,
				"SELECT P.CHARACTER_IDX, P.CHARACTER_RANK, C.CHARACTER_NAME FROM TB_PARTY P INNER JOIN TB_CHARACTER C ON P.CHARACTER_IDX = C.CHARACTER_IDX WHERE P.PARTY_IDX IN (SELECT PARTY_IDX FROM TB_PARTY WHERE CHARACTER_IDX = %d)",
				m->mPlayerIndex);
			break;
		}
	case MP_RM_PLAYER_SET_DATA_SYN:
		{
			const MSG_RM_PLAYER_DATA* m = ( MSG_RM_PLAYER_DATA* )message;

			const MSG_RM_PLAYER_DATA::Player&	player		= m->mPlayer;
			const MSG_RM_PLAYER_DATA::Position&	position	= m->mPosition;
			const MSG_RM_PLAYER_DATA::Job&		job			= m->mJob;

			// 080716 LUJ, 서비스/테스트 서버의 저장 위치가 달라 프로시저로 추출함
			g_DB.FreeMiddleQuery(
				0,
				connectionIndex,
				// 081006 LUJ, int형을 벗어나는 수치가 오버플로되지 않도록 포맷을 변경함
				"EXEC dbo.TP_CHARACTER_DATA_UPDATE %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
				player.mIndex,
				player.mStrength,
				player.mDexterity,
				player.mVitality,
				player.mIntelligence,
				player.mWisdom,
				player.mLife,
				player.mMana,
				player.mGrade,
				player.mMaxGrade,
				player.mExp,
				player.mGradeUpPoint,
				player.mMoney,
				player.mSkillPoint,
				player.mExtendedInventorySize,
				position.mMap,
				position.mX,
				position.mY,
				job.mIndex,
				job.mData1,
				job.mData2,
				job.mData3,
				job.mData4,
				job.mData5,
				job.mData6,
				player.mBadFame );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%d\' )",
				RecoveryLogSetPlayer,
				user.mId.c_str(),
				"playerIndex",
				player.mIndex );

			{
				MSGROOT message;
				message.Category	= MP_RM_PLAYER;
				message.Protocol	= MP_RM_PLAYER_SET_DATA_ACK;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}

			break;
		}
	case MP_RM_PLAYER_SET_EXTENDED_DATA_SYN:
		{
			const MSG_RM_PLAYER_EXTENDED_DATA*			m		= ( MSG_RM_PLAYER_EXTENDED_DATA* )message;
			const MSG_RM_PLAYER_EXTENDED_DATA::Player&	player	= m->mPlayer;

			// 081126 LUJ, 패밀리 별명 갱신하도록 함
			g_DB.FreeMiddleQuery(
				RSetPlayerExtendedData,
				connectionIndex,
				"EXEC dbo.TP_CHARACTER_EXTENDED_DATA_UPDATE %d, \'%s\', \'%s\', \'%s\'",
				player.mIndex,
				player.mName,
				player.mGuildNick,
				player.mFamilyNick );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%d\' )",
				RecoveryLogSetExtendedPlayer,
				user.mId.c_str(),
				"playerIndex",
				player.mIndex );

			break;
		}
	case MP_RM_PLAYER_SET_LICENSE_SYN:
		{
			const MSG_RM_PLAYER_LOVE*	m			= ( MSG_RM_PLAYER_LOVE* )message;
			const DWORD					playerIndex = m->dwObjectID;
			const DATE_MATCHING_INFO&	data		= m->mDateMatching;
			
			g_DB.FreeMiddleQuery(
				RPlayerSetLicense,
				connectionIndex,
				"EXEC dbo.TP_PLAYER_LICENSE_UPDATE %d, %d, %d, %d, %d, %d, %d, %d, %d",
				playerIndex,
				data.dwAge,
				data.dwRegion,
				data.dwGoodFeelingDegree,
				data.pdwGoodFeeling[ 0 ],
				data.pdwGoodFeeling[ 1 ],
				data.pdwGoodFeeling[ 2 ],
				data.pdwBadFeeling[ 0 ],
				data.pdwBadFeeling[ 1 ] );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%d\' )",
				RecoveryLogSetExtendedPlayer,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID );

			break;
		}
	// 080526 LUJ, 낚시 정보를 갱신한다
	case MP_RM_PLAYER_SET_FISHING_SYN:
		{
			const MSG_DWORD3*	m			= ( MSG_DWORD3* )message;
			const DWORD			playerIndex	= m->dwObjectID;
			const DWORD			level		= m->dwData1;
			const DWORD			experience	= m->dwData2;
			const DWORD			point		= m->dwData3;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"EXEC dbo.MP_FISHINGDATA_UPDATE %d, %d, %d, %d",
				playerIndex,
				level,
				experience,
				point );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2, KEY3, VALUE3 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogSetFishing,
				user.mId.c_str(),
				"playerIndex",
				playerIndex,
				"level",
				level,
				"experience",
				experience );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogSetFishing,
				user.mId.c_str(),
				"playerIndex",
				playerIndex,
				"point",
				point );

			// 080526 LUJ, 성공 메시지 반환
			{
				MSGBASE message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_RM_PLAYER;
				message.Protocol	= MP_RM_PLAYER_SET_FISHING_ACK;
				message.dwObjectID	= playerIndex;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}
			
			break;
		}
	case MP_RM_PLAYER_SET_HOUSE_SYN:
		{
			const MSG_RM_HOUSE* const receivedMessage = (MSG_RM_HOUSE*)message;
			const DWORD userIndex = receivedMessage->mUserIndex;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"UPDATE TB_HOUSE SET HOUSE_NAME = %s, EXTEND_LEVEL = %d, DECO_USEPOINT = %d, HOUSE_POINT = %d \
				WHERE USER_INDEX = %d",
				userIndex,
				receivedMessage->mHouseName,
				receivedMessage->mExtendLevel,
				receivedMessage->mDecorationPoint,
				receivedMessage->mHousePoint);

			MSGBASE sendMessage;
			ZeroMemory(
				&sendMessage,
				sizeof(sendMessage));
			sendMessage.Category = MP_RM_PLAYER;
			sendMessage.Protocol = MP_RM_PLAYER_SET_HOUSE_ACK;
			NETWORK->Send(
				connectionIndex,
				sendMessage,
				sizeof(sendMessage));
			break;
		}
	case MP_RM_PLAYER_SET_COOK_SYN:
		{
			const MSG_DWORD4* const receivedMessage = (MSG_DWORD4*)message;
			const DWORD playerIndex = receivedMessage->dwObjectID;
			const LEVELTYPE level = LEVELTYPE(receivedMessage->dwData1);
			const DWORD cookCount = receivedMessage->dwData2;
			const DWORD eatCount = receivedMessage->dwData3;
			const DWORD fireCount = receivedMessage->dwData4;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"UPDATE TB_COOKING SET COOK_LEVEL = %d, COOK_COUNT = %d, EAT_COUNT = %d, FIRE_COUNT = %d \
				WHERE CHARACTER_IDX = %d",
				level,
				cookCount,
				eatCount,
				fireCount,
				playerIndex);


			MSGBASE sendMessage;
			ZeroMemory(
				&sendMessage,
				sizeof(sendMessage));
			sendMessage.Category = MP_RM_PLAYER;
			sendMessage.Protocol = MP_RM_PLAYER_SET_COOK_ACK;
			NETWORK->Send(
				connectionIndex,
				sendMessage,
				sizeof(sendMessage));
			break;
		}
	case MP_RM_PLAYER_ADD_COOK_RECIPE_SYN:
		{
			const MSG_DWORD3* const receivedMessage = (MSG_DWORD3*)message;
			const DWORD playerIndex = receivedMessage->dwObjectID;
			const DWORD recipeIndex = receivedMessage->dwData1;
			const DWORD slot = receivedMessage->dwData2;
			const DWORD remainTime = receivedMessage->dwData3;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"INSERT INTO TB_COOKRECIPE (CHARACTER_IDX, RECIPEIDX, SLOT, REMAINTIME) VALUES(%d, %d, %d, %d)",
				playerIndex,
				recipeIndex,
				slot,
				remainTime);

			MSG_DWORD sendMessage;
			ZeroMemory(
				&sendMessage,
				sizeof(sendMessage));
			sendMessage.Category = MP_RM_PLAYER;
			sendMessage.Protocol = MP_RM_PLAYER_ADD_COOK_RECIPE_ACK;
			sendMessage.dwData = recipeIndex;
			NETWORK->Send(
				connectionIndex,
				sendMessage,
				sizeof(sendMessage));
			break;
		}
	case MP_RM_PLAYER_REMOVE_COOK_RECIPE_SYN:
		{
			const MSG_DWORD* const receivedMessage = (MSG_DWORD*)message;
			const DWORD playerIndex = receivedMessage->dwObjectID;
			const DWORD recipeIndex = receivedMessage->dwData;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"DELETE TB_COOKRECIPE WHERE CHARACTER_IDX = %d AND RECIPEIDX = %d",
				playerIndex	,
				recipeIndex);

			MSG_DWORD sendMessage;
			ZeroMemory(
				&sendMessage,
				sizeof(sendMessage));
			sendMessage.Category = MP_RM_PLAYER;
			sendMessage.Protocol = MP_RM_PLAYER_REMOVE_COOK_RECIPE_ACK;
			sendMessage.dwData = recipeIndex;
			NETWORK->Send(
				connectionIndex,
				sendMessage,
				sizeof(sendMessage));
			break;
		}
	case MP_RM_PLAYER_GET_NOTE_SYN:
		{
			const MSGBASE* const receivedMessage = (MSGBASE*)message;
			const DWORD noteIndex = receivedMessage->dwObjectID;

			g_DB.FreeLargeQuery(
				RGetNote,
				connectionIndex,
				"SELECT %d, Sender_Name, bRead, Note FROM TB_NOTE WHERE Note_IDX = %d",
				noteIndex,
				noteIndex);
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

void CMSSystem::ParseItem( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM;
		message.Protocol	= MP_RM_ITEM_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_ITEM_GET_SYN:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			const DWORD playerIndex	= m->dwObjectID;
			const DWORD userIndex	= m->dwData;
			
			GetItemListOnInventory( connectionIndex, playerIndex, 0 );
			GetItemListOnShop( connectionIndex, userIndex, 0 );
			GetItemListOnStorage( connectionIndex, userIndex, 0 );
			
			g_DB.FreeMiddleQuery(
				RGetStorageData,
				connectionIndex,
				"SELECT P.User_IDX, P.PyogukNum, P.PyogukMoney, C.CHARACTER_INVENTORY\
				FROM TB_CHARACTER C INNER JOIN TB_PYOGUK P ON C.USER_IDX = P.User_IDX\
				WHERE C.CHARACTER_IDX = %d",
				playerIndex );
			// 080716 LUJ, 펫 정보를 가져온다
			g_DB.FreeMiddleQuery(
				RGetPetData,
				connectionIndex,
				"SELECT TOP %d PET_IDX, ITEM_DBIDX, PET_KIND, PET_LEVEL, PET_GRADE, SKILL_SLOT, PET_EXP,\
				PET_FRIENDLY, PET_HP, PET_MAXHP, PET_MP, PET_MAXMP,PET_AI, PET_STATE, PET_TYPE, %d\
				FROM TB_PET WHERE MASTER_IDX = %d AND PET_IDX > %d ORDER BY PET_IDX",
				MAX_MIDDLEROW_NUM,
				userIndex,
				userIndex,
				0 );
			// 가구 목록을 가져온다
			g_DB.FreeQuery(
				RGetFurniture,
				connectionIndex,
				"SELECT TOP %d FURNITURE_INDEX, ITEM_INDEX, ROUND(POSX, 0), ROUND(POSY, 0), ROUND(POSZ, 0), ROUND(ANGLE, 0), CATEGORY, SLOT, STATE, NOTDELETE,\
				CONVERT(VARCHAR(6), ENDTIME, 12) + \' \' + CONVERT(VARCHAR(8), ENDTIME, 8)\
				FROM TB_HOUSE_FURNITURE WHERE USER_INDEX = %d",
				MAX_ROW_NUM,
				userIndex);
			break;
		}
	case MP_RM_ITEM_SET_OPTION_SYN:
		{
			const MSG_RM_ITEM_UPDATE_OPTION* m = ( MSG_RM_ITEM_UPDATE_OPTION* )message;

			const DWORD			playerIndex	= m->dwObjectID;
			const ITEM_OPTION&	option		= m->mOption;
			const ITEM_OPTION	emptyOption	= { 0 };

			// 080320 LUJ, 옵션이 있을 경우에만 삽입하도록 한다
			if( &emptyOption.mReinforce,	&option.mReinforce, sizeof( emptyOption.mReinforce ) ||
				&emptyOption.mMix,			&option.mReinforce, sizeof( emptyOption.mMix )		||
				&emptyOption.mEnchant,		&option.mReinforce, sizeof( emptyOption.mEnchant ) )
			{
				const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
				const ITEM_OPTION::Mix&			mix			= option.mMix;
				const ITEM_OPTION::Enchant&		enchant		= option.mEnchant;

				g_DB.FreeMiddleQuery(
					0,
					0,
					"EXEC dbo.MP_ITEM_OPTION_INSERT %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, \'%s\', %d, %d, %d, %d, %d, \'%s\', %d, %d, \'%s\'", 
					option.mItemDbIndex,
					reinforce.mStrength,
					reinforce.mDexterity,
					reinforce.mVitality,
					reinforce.mIntelligence,
					reinforce.mWisdom,
					reinforce.mLife,
					reinforce.mMana,
					reinforce.mLifeRecovery,
					reinforce.mManaRecovery,
					reinforce.mPhysicAttack,
					reinforce.mPhysicDefence,
					reinforce.mMagicAttack,
					reinforce.mMagicDefence,
					reinforce.mMoveSpeed,
					reinforce.mEvade,
					reinforce.mAccuracy,
					reinforce.mCriticalRate,
					reinforce.mCriticalDamage,
					reinforce.mMadeBy,
					mix.mStrength,
					mix.mIntelligence,
					mix.mDexterity,
					mix.mWisdom,
					mix.mVitality,
					mix.mMadeBy,
					enchant.mIndex,
					enchant.mLevel,
					enchant.mMadeBy );

				// 로그를 남기자
				g_DB.LogMiddleQuery(
					0,
					0,
					"EXEC dbo.TP_ITEM_OPTION_LOG_INSERT %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, \'%s\'",
					playerIndex,
					option.mItemDbIndex,
					reinforce.mStrength,
					reinforce.mDexterity,
					reinforce.mVitality,
					reinforce.mIntelligence,
					reinforce.mWisdom,
					reinforce.mLife,
					reinforce.mMana,
					reinforce.mLifeRecovery,
					reinforce.mManaRecovery,
					reinforce.mPhysicAttack,
					reinforce.mPhysicDefence,
					reinforce.mMagicAttack,
					reinforce.mMagicDefence,
					reinforce.mCriticalRate,
					reinforce.mCriticalDamage,
					reinforce.mMoveSpeed,
					reinforce.mEvade,
					reinforce.mAccuracy,
					mix.mStrength,
					mix.mIntelligence,
					mix.mDexterity,
					mix.mWisdom,
					mix.mVitality,
					enchant.mIndex,
					enchant.mLevel,
					eLog_ItemOptionAdd_RM,
					user.mId.c_str() );
			}

			// 080320 LUJ, 드롭 옵션을 삽입
			if( memcmp( &emptyOption.mDrop, &option.mDrop, sizeof( emptyOption.mDrop ) ) )
			{
				const ITEM_OPTION::Drop& drop = option.mDrop;

				const ITEM_OPTION::Drop::Value& value1 = option.mDrop.mValue[ 0 ];
				const ITEM_OPTION::Drop::Value& value2 = option.mDrop.mValue[ 1 ];
				const ITEM_OPTION::Drop::Value& value3 = option.mDrop.mValue[ 2 ];
				const ITEM_OPTION::Drop::Value& value4 = option.mDrop.mValue[ 3 ];
				const ITEM_OPTION::Drop::Value& value5 = option.mDrop.mValue[ 4 ];

				g_DB.FreeMiddleQuery(
					0,
					0,
					"EXEC dbo.MP_ITEM_DROP_OPTION_INSERT %d, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f",
					option.mItemDbIndex,
					value1.mKey,
					value1.mValue,
					value2.mKey,
					value2.mValue,
					value3.mKey,
					value3.mValue,
					value4.mKey,
					value4.mValue,
					value5.mKey,
					value5.mValue );

				g_DB.LogMiddleQuery(
					0,
					0,
					"INSERT INTO TB_ITEM_DROP_OPTION_LOG ( ITEM_DBIDX, KEY1, VALUE1, KEY2, VALUE2, KEY3, VALUE3, KEY4, VALUE4, KEY5, VALUE5, TYPE, MEMO ) VALUES ( %d, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, \'%s\' )",
					option.mItemDbIndex,
					value1.mKey,
					value1.mValue,
					value2.mKey,
					value2.mValue,
					value3.mKey,
					value3.mValue,
					value4.mKey,
					value4.mValue,
					value5.mKey,
					value5.mValue,
					eLog_ItemOptionAdd_RM,
					user.mId.c_str() );
			}
			
			{
				MSG_RM_ITEM_UPDATE_OPTION	message;
				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_SET_OPTION_ACK;
				message.dwObjectID	= playerIndex;
				message.mOption		= option;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogSetItemOption,
				user.mId.c_str(),
				"playerIndex",
				playerIndex,
				"itemDbIndex",
				option.mItemDbIndex );
			break;
		}
	case MP_RM_ITEM_SET_SYN:
		{
			const MSG_RM_ITEM_UPDATE*	m		= ( MSG_RM_ITEM_UPDATE* )message;
			const ITEMBASE&				item	= m->mItem;

			g_DB.FreeMiddleQuery(
				0,
				connectionIndex,
				"UPDATE TB_ITEM\
				SET		ITEM_IDX = %d, ITEM_POSITION = %d, ITEM_DURABILITY = %d, ITEM_SEAL = %d, ITEM_ENDTIME = DATEADD( SECOND, %u, GETDATE() )\
				WHERE   (ITEM_DBIDX = %d)",
				item.wIconIdx,
				item.Position,
				item.Durability,				
				item.nSealed,
				item.nRemainSecond,
				item.dwDBIdx );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogSetItem,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"itemDbIndex",
				m->mItem.dwDBIdx );

			{
				MSG_RM_ITEM_UPDATE message	= *m;
				message.Protocol			= MP_RM_ITEM_SET_ACK;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}

			break;
		}
	case MP_RM_ITEM_DELETE_SYN:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			const DWORD playerIndex	= m->dwObjectID;
			const DWORD itemDbIndex = m->dwData;			

			g_DB.FreeMiddleQuery(
				0,
				0,
				"DELETE FROM TB_ITEM WHERE ITEM_DBIDX = %d",
				itemDbIndex );

			g_DB.FreeMiddleQuery(
				0,
				0,
				"DELETE FROM TB_ITEM_OPTION2 WHERE ITEM_DBIDX = %d",
				itemDbIndex );

			g_DB.FreeMiddleQuery(
				0,
				0,
				"DELETE FROM TB_ITEM_DROP_OPTION WHERE ITEM_DBIDX = %d",
				itemDbIndex );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogRemoveItem,
				user.mId.c_str(),
				"playerIndex",
				playerIndex,
				"itemDbIndex",
				itemDbIndex );

			g_DB.LogMiddleQuery(
				0,
				0,
				"EXEC dbo.UP_ITEMMONEYLOG %d, \'%s\', %d, \'%s\', %d, %u, %u, %u, %d, %d, %d, %d, %d, %d", 
				0,
				std::string( "*" + user.mId ).c_str(), 
				playerIndex,
				"", 
				eLog_ItemRemove,
				0,
				0,
				0,
				0,
				itemDbIndex,
				0,
				0,
				0,
				0 );

			{
				MSG_DWORD message;
				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_DELETE_ACK;
				message.dwObjectID	= playerIndex;
				message.dwData		= itemDbIndex;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}
			break;
		}
	case MP_RM_ITEM_DELETE_OPTION_SYN:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			const DWORD playerIndex = m->dwObjectID;
			const DWORD itemDbIndex	= m->dwData;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"DELETE FROM TB_ITEM_OPTION2 WHERE ITEM_DBIDX = %d",
				itemDbIndex );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogRemoveItemOption,
				user.mId.c_str(),
				"playerIndex",
				playerIndex,
				"itemDbIndex",
				itemDbIndex );

			g_DB.LogMiddleQuery(
				0,
				0,
				"EXEC dbo.TP_ITEM_OPTION_LOG_INSERT %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, \'%s\'",
				0,
				itemDbIndex,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				eLog_ItemOptionRemove_RM,
				user.mId.c_str() );

			{
				MSG_DWORD message;
				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_DELETE_OPTION_ACK;
				message.dwObjectID	= playerIndex;
				message.dwData		= itemDbIndex;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}
			break;
		}
	// 080320 LUJ, 드롭 옵션 삭제
	case MP_RM_ITEM_DELETE_DROP_OPTION_SYN:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			const DWORD playerIndex = m->dwObjectID;
			const DWORD itemDbIndex	= m->dwData;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"DELETE FROM TB_ITEM_DROP_OPTION WHERE ITEM_DBIDX = %d",
				itemDbIndex );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogRemoveItemdDropOption,
				user.mId.c_str(),
				"playerIndex",
				playerIndex,
				"itemDbIndex",
				itemDbIndex );

			{
				MSG_DWORD message;
				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_DELETE_DROP_OPTION_ACK;
				message.dwObjectID	= playerIndex;
				message.dwData		= itemDbIndex;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}
			break;
		}
	case MP_RM_ITEM_ADD_SYN:
		{
			const MSG_RM_ITEM_ADD* m = ( MSG_RM_ITEM_ADD* )message;
			
			const Item& item = m->mItem;

			switch( item.mArea )
			{
			case Item::AreaInventory:
				{
					g_DB.FreeMiddleQuery(
						RAddItem,
						connectionIndex,
						"EXEC dbo.TP_ITEM_INSERT %d, %d, %d, %d, %d, %d, %d, %d",
						m->mPlayerIndex,
						item.wIconIdx,
						item.Position,
						item.Durability,
						0,
						0,
						item.nSealed,
						item.mArea );
					break;
				}
			case Item::AreaPrivateStorage:
				{
					g_DB.FreeMiddleQuery(
						RAddItem,
						connectionIndex,
						"EXEC dbo.TP_ITEM_INSERT %d, %d, %d, %d, %d, %d, %d, %d",
						0,
						item.wIconIdx,
						item.Position,
						item.Durability,
						m->mUserIndex,
						0,
						item.nSealed,
						item.mArea );
					break;
				}
			case Item::AreaCashStorage:
				{
					g_DB.FreeMiddleQuery(
						RAddItem,
						connectionIndex,
						"EXEC dbo.TP_ITEM_INSERT %d, %d, %d, %d, %d, %d, %d, %d",
						0,
						item.wIconIdx,
						item.Position,
						item.Durability,						
						0,
						m->mUserIndex,
						item.nSealed,
						item.mArea );
					break;
				}
			}

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2, KEY3, VALUE3 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogAddItem,
				user.mId.c_str(),
				"playerIndex",
				m->mPlayerIndex,
				"itemIndex",
				m->mItem.wIconIdx,
				"quantity",
				m->mItem.Durability );
			break;
		}
	case MP_RM_ITEM_UPDATE_END_TIME_SYN:
		{
			const MSG_INT2* m = ( MSG_INT2* )message;

			const DWORD itemDbIndex	= m->nData1;
			const DWORD second		= m->nData2;

			g_DB.FreeMiddleQuery(
				RSetItemEndTime,
				connectionIndex,
				"EXEC dbo.TP_ITEM_END_TIME_UPDATE %d, %d",
				itemDbIndex,
				second );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogUpdateItemEndTime,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"itemDbIndex",
				m->nData1 );
			break;
		}
	case MP_RM_ITEM_UPDATE_REMAIN_TIME_SYN:
		{
			const MSG_INT2* m = ( MSG_INT2* )message;

			const DWORD itemDbIndex = m->nData1;
			const DWORD second		= m->nData2;

			g_DB.FreeMiddleQuery(
				0,
				connectionIndex,
				"UPDATE TB_ITEM SET ITEM_REMAINTIME = %d WHERE ITEM_DBIDX = %d",				
				second,
				itemDbIndex );

			{
				MSG_INT2 message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_UPDATE_REMAIN_TIME_ACK;
				message.nData1		= itemDbIndex;
				message.nData2		= second;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}

			break;
		}
	case MP_RM_ITEM_SET_STORAGE_SYN:
		{
			const MSG_DWORD2* m = ( MSG_DWORD2* )message;

			g_DB.FreeMiddleQuery(
				RSetStorageData,
				connectionIndex,
				"EXEC dbo.TP_STORAGE_UPDATE %d, %d, %d",
				m->dwObjectID,
				m->dwData1,
				m->dwData2 );
			
			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2, KEY3, VALUE3 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogSetItemStorage,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"size",
				m->dwData1,
				"money",
				m->dwData2 );

			break;
		}
		// 080716 LUJ, 펫 정보 변경
	case MP_RM_ITEM_SET_PET_SYN:
		{
			const MSG_RM_PET*		m			= ( MSG_RM_PET* )message;
			const MSG_RM_PET::Pet&	pet			= m->mData[ 0 ];
			const DWORD				userIndex	= m->dwObjectID;

			g_DB.FreeMiddleQuery(
				0,
				connectionIndex,
				"UPDATE TB_PET SET PET_KIND = %d, PET_LEVEL = %d, PET_GRADE = %d, SKILL_SLOT = %d, PET_EXP = %d,\
				PET_FRIENDLY = %d, PET_AI = %d, PET_STATE = %d, PET_TYPE = %d\
				WHERE PET_IDX = %u",
				pet.mKind,
				pet.mLevel,
				pet.mGrade,
				pet.mSkillSlot,
				pet.mExperience,
				pet.mFriendship,
				pet.mArtificialIntelligence,
				pet.mState,
				pet.mType,
				pet.mIndex );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogUpdatePet,
				user.mId.c_str(),
				"userIndex",
				userIndex,
				"itemDbIndex",
				pet.mItemDbIndex );

			{
				MSGBASE	message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_SET_PET_ACK;
				message.dwObjectID	= userIndex;
				
				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}
			break;
		}
		// 080731 LUJ, 펫 추가
	case MP_RM_ITEM_ADD_PET_SYN:
		{
			const MSG_RM_PET* m	= ( MSG_RM_PET* )message;

			if( 1 != m->mSize )
			{
				break;
			}

			const DWORD				userIndex	= m->dwObjectID;
			const MSG_RM_PET::Pet&	pet			= m->mData[ 0 ];
			
			g_DB.FreeMiddleQuery(
				RAddPet,
				connectionIndex,
				"EXEC dbo.TP_PET_INSERT %d, %d, %d, %d, %d, %d",
                userIndex,
				pet.mItemDbIndex,
				pet.mSkillSlot,
				pet.mKind,
				pet.mMaxHealth,
				pet.mMaxMana );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogAddPet,
				user.mId.c_str(),
				"userIndex",
				userIndex,
				"itemDbIndex",
				pet.mItemDbIndex );

			break;
		}
		// 080731 LUJ, 펫 삭제
	case MP_RM_ITEM_REMOVE_PET_SYN:
		{
			const MSG_DWORD2*	m			= ( MSG_DWORD2* )message;
			const DWORD			userIndex	= m->dwObjectID;
			const DWORD			itemDbIndex	= m->dwData1;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"DELETE TB_PET WHERE MASTER_IDX = %d AND ITEM_DBIDX = %d",
				userIndex,
				itemDbIndex );

			g_DB.FreeMiddleQuery(
				0,
				0,
				"UPDATE TB_ITEM SET ITEM_SEAL = 1 WHERE ITEM_DBIDX = %d",
				itemDbIndex );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogRemovePet,
				user.mId.c_str(),
				"userIndex",
				userIndex,
				"itemDbIndex",
				itemDbIndex );

			{
				MSGBASE message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_RM_ITEM;
				message.Protocol	= MP_RM_ITEM_REMOVE_PET_ACK;
				message.dwObjectID	= itemDbIndex;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}
			
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseItemFind( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM_FIND;
		message.Protocol	= MP_RM_ITEM_FIND_OWNER_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_ITEM_FIND_OWNER_SYN:
		{
			const MSG_DWORD2* m = ( MSG_DWORD2* )message;

			FindItemOwner( connectionIndex, m->dwData1, m->dwData2 );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseItemLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM_LOG;
		message.Protocol	= MP_RM_ITEM_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_ITEM_LOG_GET_SIZE_SYN:
		{
			const MSG_RM_ITEM_LOG_REQUEST* m = ( MSG_RM_ITEM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetItemLogSize,
				connectionIndex,
				"EXEC dbo.TP_ITEM_LOG_SIZE_SELECT %d, %d, %d, \'%s\', \'%s\', %d",
				m->mPlayerIndex,
				m->mItemDbIndex,
				m->mItemIndex,
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick );

			break;
		}
	case MP_RM_ITEM_LOG_GET_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_ITEM;
			message.Protocol	= MP_RM_ITEM_LOG_GET_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseItemOptionLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM_OPTION_LOG;
		message.Protocol	= MP_RM_ITEM_OPTION_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_ITEM_OPTION_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetItemOptionLogSize,
				connectionIndex,
				"EXEC dbo.TP_ITEM_OPTION_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				m->dwObjectID,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_ITEM_OPTION_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_ITEM_OPTION_LOG;
			message.Protocol	= MP_RM_ITEM_OPTION_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseItemDropOptionLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM_DROP_OPTION_LOG;
		message.Protocol	= MP_RM_ITEM_DROP_OPTION_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_ITEM_DROP_OPTION_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetItemDropOptionLogSize,
				connectionIndex,
				"EXEC dbo.TP_ITEM_DROP_OPTION_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				m->dwObjectID,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_ITEM_DROP_OPTION_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_ITEM_DROP_OPTION_LOG;
			message.Protocol	= MP_RM_ITEM_DROP_OPTION_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseSkill( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_SKILL;
		message.Protocol	= MP_RM_SKILL_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_SKILL_GET_ALL_SYN:
		{
			const MSGBASE*	m			= ( MSGBASE* )message;
			const DWORD		playerIndex	= m->dwObjectID;

			GetSkillList( connectionIndex, playerIndex );
			GetBuffList( connectionIndex, playerIndex, 0 );
			break;
		}
	case MP_RM_SKILL_ADD_SYN:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			AddSkillData( connectionIndex, m->dwObjectID, m->dwData );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogAddSkill,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"skillIndex",
				m->dwData );
			break;
		}
	case MP_RM_SKILL_REMOVE_SYN:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			RemoveSkillData( connectionIndex, m->dwData );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogRemoveSkill,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"skillIndex",
				m->dwData );
			break;
		}
	case MP_RM_SKILL_SET_LEVEL_SYN:
		{
			const MSG_DWORD2* m = ( MSG_DWORD2* )message;

			SetSkillLevel( connectionIndex, m->dwObjectID, m->dwData1, m->dwData2 );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogSetSkill,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"skillLevel",
				m->dwData2 );
			break;
		}
		// 090211 LUJ, 버프를 삭제한다
	case MP_RM_SKILL_BUFF_REMOVE_SYN:
		{
			const MSG_DWORD* const m = ( MSG_DWORD* )message;

			const DWORD playerIndex		= m->dwObjectID;
			const DWORD buffSkillIndex	= m->dwData;

			g_DB.FreeMiddleQuery(
				0,
				connectionIndex,
				"DELETE FROM TB_BUFF WHERE CHARACTER_IDX = %d AND SKILL_IDX = %d",
				playerIndex,
				buffSkillIndex );
			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogRemoveBuff,
				user.mId.c_str(),
				"playerIndex",
				playerIndex,
				"buff",
				buffSkillIndex );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseSkillLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_SKILL_LOG;
		message.Protocol	= MP_RM_SKILL_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;

	}
	switch( message->Protocol )
	{
	case MP_RM_SKILL_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetSkillLogSize,
				connectionIndex,
				"EXEC dbo.TP_SKILL_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				m->dwObjectID,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_SKILL_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_SKILL_LOG;
			message.Protocol	= MP_RM_SKILL_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseFamily( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_FAMILY;
		message.Protocol	= MP_RM_FAMILY_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_FAMILY_GET_LIST_SYN:
		{
			const MSG_NAME* m		= ( MSG_NAME*)message;
			const char*		keyword	= m->Name;
			
			g_DB.FreeQuery(
				RGetFamilyList,
				connectionIndex,
				"SELECT TOP 100 A.ID, A.NAME, COUNT( B.MEMBERID ) AS SIZE, A.MASTERID\
				FROM TB_FAMILY AS A LEFT JOIN TB_FAMILY_MEMBER AS B ON B.FAMILYID = A.ID\
				WHERE A.NAME LIKE '%s' GROUP BY A.ID, A.NAME, A.MASTERID",
				keyword );
			break;
		}
	case MP_RM_FAMILY_GET_DATA_SYN:
		{
			const MSGBASE*	m			= ( MSGBASE* )message;
			const DWORD		familyIndex = m->dwObjectID;

			g_DB.FreeMiddleQuery(
				RGetFamilyData,
				connectionIndex,
				"SELECT ID, NAME, HONORPOINT, NICKNAMEON, MASTERID, BUILD_DATE\
				FROM TB_FAMILY WHERE ID = %d",
				familyIndex );

			g_DB.FreeMiddleQuery(
				RGetFamilyFarm,
				connectionIndex,
				"SELECT ZONE, FARM, FARMSTATE, FARMOWNERID, GARDENGRADE, HOUSEGRADE, WAREHOUSEGRADE, ANIMALCAGEGRADE, FENCEGRADE, BUILD_DATE\
				FROM TB_FARM_FARM WHERE FARMOWNERID IN ( SELECT MASTERID FROM TB_FAMILY WHERE ID = %d)",
				familyIndex	);

			g_DB.FreeMiddleQuery(
				RGetFamilyCrop,
				connectionIndex,
				"SELECT CROP.CROP, CROP.CROPOWNERID, CHAR.CHARACTER_NAME, CROP.CROPKIND, CROP.CROPSTEP, CROP.CROPLIFE, CROP.CROPNEXTSTEPTIME, CROP.CROPSEEDGRADE\
				FROM TB_FAMILY_MEMBER MEMBER INNER JOIN TB_FARM_CROP CROP ON MEMBER.MEMBERID = CROP.CROPOWNERID INNER JOIN\
				TB_CHARACTER CHAR ON MEMBER.MEMBERID = CHAR.CHARACTER_IDX WHERE MEMBER.FAMILYID = %d",
				familyIndex	);

			g_DB.FreeMiddleQuery(
				RGetFamilyLivestock,
				connectionIndex,
				"SELECT A.ANIMAL, A.ANIMALOWNERID, C.CHARACTER_NAME, A.ANIMALKIND, A.ANIMALSTEP, A.ANIMALLIFE, A.ANIMALNEXTSTEPTIME, A.ANIMALCONTENTMENT, A.ANIMALINTEREST\
				FROM TB_FAMILY_MEMBER MEMBER INNER JOIN TB_FARM_ANIMAL A ON MEMBER.MEMBERID = A.ANIMALOWNERID INNER JOIN\
				TB_CHARACTER C ON MEMBER.MEMBERID = C.CHARACTER_IDX WHERE MEMBER.FAMILYID = %d",
				familyIndex );
			break;
		}
	case MP_RM_FAMILY_SET_DATA_SYN:
		{
			const MSG_RM_FAMILY_DATA*	m				= ( MSG_RM_FAMILY_DATA* )message;
			const DWORD					familyIndex		= m->dwObjectID;
			
			g_DB.FreeMiddleQuery(
				0,
				0,
				"UPDATE TB_FAMILY SET HONORPOINT = %d, NAME = \'%s\' WHERE ID = %d",
				m->mHonorPoint,
				m->mName,
				familyIndex );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogSetFamily,
				user.mId.c_str(),
				"familyIndex",
				familyIndex,
				"point",
				m->mHonorPoint );

			MSGBASE message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_FAMILY;
			message.Protocol	= MP_RM_FAMILY_SET_DATA_ACK;
			message.dwObjectID	= familyIndex;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseQuest( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_QUEST_GET_MAIN_DATA_SYN:
		{
			const MSGBASE* m = ( MSGBASE* )message;

			// 서브 퀘스트 아이템 정보를 읽고 메인 퀘스트 정보를 읽는다

			GetQuestSubData( connectionIndex, m->dwObjectID, 0 );
			
			break;
		}
	case MP_RM_QUEST_REMOVE_SYN:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			RemoveQuestData( connectionIndex, m->dwObjectID, m->dwData );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogRemoveQuest,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"mainQuestIdx",
				m->dwData );
			break;
		}
	case MP_RM_QUEST_FINISH_SUB_SYN:
		{
			const MSG_DWORD3* m = ( MSG_DWORD3* )message;

			const DWORD			mainQuestIndex	= m->dwData1;
			const DWORD			subQuestIndex	= m->dwData2;
			const QSTATETYPE	state			= m->dwData3;

			EndSubQuest( connectionIndex, m->dwObjectID, mainQuestIndex, subQuestIndex, state );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogFinishSubQuest,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"subQuestIdx",
				subQuestIndex );
			break;
		}
	case MP_RM_QUEST_ITEM_UPDATE_SYN:
		{
			const MSG_DWORD3* m = ( MSG_DWORD3* )message;

			const DWORD playerIndex		= m->dwObjectID;
			const DWORD questMainIndex	= m->dwData1;
			const DWORD itemIndex		= m->dwData2;
			const DWORD size			= m->dwData3;

			g_DB.FreeMiddleQuery(
				RUpdateQuestItem,
				connectionIndex,
				"EXEC dbo.TP_QUEST_ITEM_UPDATE %d, %d, %d, %d",
				playerIndex,
				questMainIndex,
				itemIndex,
				size );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogSetQuestItem,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"questItemIdx",
				m->dwData2 );
			break;
		}
	case MP_RM_QUEST_ITEM_DELETE_SYN:
		{
			const MSG_DWORD2* m = ( MSG_DWORD2* )message;

			DeleteQuestItem( connectionIndex, m->dwObjectID, m->dwData1, m->dwData2 );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogRemoveQuestItem,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"questItemIdx",
				m->dwData2 );
			break;
		}
	case MP_RM_QUEST_ITEM_ADD_SYN:
		{
			const MSG_DWORD3* m = ( MSG_DWORD3* )message;

			const DWORD playerIndex		= m->dwObjectID;
			const DWORD mainQuestIndex	= m->dwData1;
			const DWORD itemIndex		= m->dwData2;
			const DWORD	quantity		= m->dwData3;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"INSERT INTO TB_QUESTITEM ( CHARACTER_IDX, ITEM_IDX, ITEM_COUNT, QUEST_IDX ) VALUES ( %d, %d, %d, %d )",
				playerIndex,
				itemIndex,
				quantity,
				mainQuestIndex );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2, KEY3, VALUE3 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogAddQuestItem,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"questItemIdx",
				itemIndex,
				"size",
				quantity );

			{
				SEND_QUESTITEM message;
				message.Category	= MP_RM_QUEST;
				message.Protocol	= MP_RM_QUEST_ITEM_ADD_ACK;
				message.wCount		= 1;

				{
					QITEMBASE& item = message.ItemList[ 0 ];

					item.ItemIdx	= WORD( itemIndex );
					item.QuestIdx	= WORD( mainQuestIndex );
					item.Count		= WORD( quantity );
				}

				NETWORK->Send( connectionIndex, message, message.GetSize() );
			}
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseQuestLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_QUEST_LOG;
		message.Protocol	= MP_RM_QUEST_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_QUEST_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetQuestLogSize,
				connectionIndex,
				"EXEC dbo.TP_QUEST_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				m->dwObjectID,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_QUEST_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_QUEST_LOG;
			message.Protocol	= MP_RM_QUEST_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseGuild( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_GUILD_GET_LIST_SYN:
		{
			const MSG_NAME_DWORD* m = ( MSG_NAME_DWORD*)message;

			GetGuildList( connectionIndex, m->Name, 0 < m->dwData );
			break;
		}
	case MP_RM_GUILD_GET_DATA_SYN:
		{
			const MSG_NAME_DWORD* m = ( MSG_NAME_DWORD*)message;

			const char* keyword		= m->Name;
			const DWORD guildIndex	= m->dwData;

			g_DB.FreeMiddleQuery(
				RGetGuildData,
				connectionIndex,
				"EXEC dbo.TP_GUILD_DATA_SELECT \'%s\', %d",
				keyword,
				guildIndex );
			break;
		}
	case MP_RM_GUILD_SET_DATA_SYN:
		{
			const MSG_RM_GUILD_DATA* m = ( MSG_RM_GUILD_DATA* )message;

			g_DB.FreeMiddleQuery(
				RGuildUpdateData,
				connectionIndex,
				"EXEC dbo.TP_GUILD_DATA_UPDATE %d, \'%s\', \'%s\', %d, %d, %d",
				m->mGuildIndex,
				m->mGuildName,
				m->mAllianceName,
				m->mLevel,
				m->mMoney,
				m->mScore );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%d\' )",
				RecoveryLogSetGuild,
				user.mId.c_str(),
				"guildIndex",
				m->mGuildIndex );
			break;
		}
	case MP_RM_GUILD_SET_RANK_SYN:
		{
			const MSG_DWORD2* m = ( MSG_DWORD2* )message;

			const DWORD guildIndex	= m->dwObjectID;
			const DWORD playerIndex	= m->dwData1;
			const DWORD rank		= m->dwData2;

			g_DB.FreeMiddleQuery(
				RGuildSetRank,
				connectionIndex,
				"EXEC dbo.TP_GUILD_RANK_UPDATE %d, %d, %d, %d",
				guildIndex,
				playerIndex,
				rank,
				GUILD_MASTER == rank );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2, KEY3, VALUE3 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogSetGuildRank,
				user.mId.c_str(),
				"guildIndex",
				m->dwData1,
				"playerIndex",
				m->dwObjectID,
				"rank",
				m->dwData2 );
			break;
		}
	case MP_RM_GUILD_KICK_SYN:
		{
			const MSGBASE* m = ( MSGBASE* )message;

			const DWORD playerIndex = m->dwObjectID;

			g_DB.FreeMiddleQuery(
				RGuildKickMember,
				connectionIndex,
				"EXEC dbo.TP_GUILD_MEMBER_DELETE %d",
				playerIndex );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%d\' )",
				RecoveryLogKickGuildMember,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID );
		}
	// 080425 LUJ, 길드 스킬 삭제
	case MP_RM_GUILD_REMOVE_SKILL_SYN:
		{
			const MSG_DWORD2* m = ( MSG_DWORD2* )message;

			const DWORD guildIndex	= m->dwObjectID;
			const DWORD skillIndex	= m->dwData1;
			const DWORD skillLevel	= m->dwData2;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"DELETE FROM TB_GUILD_SKILL WHERE GUILD_IDX = %d AND SKILL_IDX = %d",
				guildIndex,
				skillIndex );

			{
				MSG_DWORD message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_RM_GUILD;
				message.Protocol	= MP_RM_GUILD_REMOVE_SKILL_ACK;
				message.dwObjectID	= guildIndex;
				message.dwData		= skillIndex;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}

			// 080425 LUJ, 濡쒓렇
			{
				char text[ MAX_PATH ] = { 0 };
				sprintf( text, "%d(%d)", skillIndex, skillLevel );

				g_DB.LogMiddleQuery(
					0,
					0,
					"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%s\' )",
					RecoveryLogRemoveGuildSkill,
					user.mId.c_str(),
					"skillIndex",
					text );
			}

			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseGuildLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_GUILD_LOG;
		message.Protocol	= MP_RM_GUILD_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_GUILD_LOG_SIZE_SYN:
		{
			const MSG_RM_GUILD_LOG_REQUEST* const m = ( MSG_RM_GUILD_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetGuildLogSize,
				connectionIndex,
				"EXEC dbo.TP_GUILD_LOG_SIZE_SELECT %d, %d, \'%s\', \'%s\'",
				m->mGuildIndex,
				m->mPlayerIndex,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_GUILD_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_GUILD_LOG;
			message.Protocol	= MP_RM_GUILD_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseGuildWarehouseLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_GUILD_WAREHOUSE_LOG;
		message.Protocol	= MP_RM_GUILD_WAREHOUSE_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_GUILD_WAREHOUSE_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetGuildWarehouseLogSize,
				connectionIndex,
				"EXEC dbo.TP_GUILD_WAREHOUSE_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				m->dwObjectID,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_GUILD_WAREHOUSE_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_GUILD_WAREHOUSE_LOG;
			message.Protocol	= MP_RM_GUILD_WAREHOUSE_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseExperienceLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_EXP_LOG;
		message.Protocol	= MP_RM_EXP_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_EXP_LOG_GET_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetExperienceLogSize,
				connectionIndex,
				"EXEC dbo.TP_EXP_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				m->dwObjectID,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_EXP_LOG_GET_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_EXP_LOG;
			message.Protocol	= MP_RM_EXP_LOG_GET_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseStatLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_STAT_LOG;
		message.Protocol	= MP_RM_STAT_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_STAT_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetStatLogSize,
				connectionIndex,
				"EXEC dbo.TP_STAT_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				m->dwObjectID,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_STAT_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_STAT_LOG;
			message.Protocol	= MP_RM_STAT_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseUser( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_USER;
		message.Protocol	= MP_RM_USER_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_USER_GET_DATA_SYN:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			const DWORD userIndex	= m->dwObjectID;
			TCHAR userName[MAX_PATH] = {0};
			StringCopySafe(
				userName,
				m->Name,
				_countof(userName));

			LPCTSTR seperator = " ";
			LPCTSTR token = _tcstok(
				userName,
				seperator);

			g_DB.LoginMiddleQuery(
				RGetUserData,
				connectionIndex,
				"EXEC dbo.TP_USER_DATA_SELECT %d, \'%s\'",
				userIndex,
				token ? token : "");
			g_DB.FreeMiddleQuery(
				RGetGameRoomData,
				connectionIndex,
				"SELECT PCROOM_INDEX, PCROOM_POINT, PROVIDE_CNT, CONVERT(VARCHAR(6), PROVIDE_TIME, 12) + \' \' + CONVERT(VARCHAR(8), PROVIDE_TIME, 8) FROM TB_GAMEROOM WHERE USER_IDX = %d",
				userIndex);
			break;
		}
	case MP_RM_USER_SET_DATA_SYN:
		{
			const MSG_DWORD5* m = ( MSG_DWORD5* )message;

			const DWORD userIndex	= m->dwData1;
			const DWORD level		= m->dwData2;
			const DWORD state		= m->dwData3;
			const DWORD punishState = m->dwData4;
			const DWORD punishTime	= m->dwData5;

			// 080725 LUJ, 프로시저를 사용하지 않고 직접 쿼리를 전송하도록 함
			g_DB.LoginMiddleQuery(
				0,
				0,
				"UPDATE CHR_LOG_INFO SET USERLEVEL = %d, STA_NUM = %d WHERE PROPID = %d",
				level,
				state,
				userIndex );

			// 080725 LUJ, 상태값을 로그로 남기도록 함
			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2, KEY3, VALUE3 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryLogSetUser,
				user.mId.c_str(),
				"userIndex",
				m->dwData1,
				"level",
				m->dwData2,
				"state",
				m->dwData3 );

			// 090618 ShinJS --- Auto Punish 정보 갱신
			g_DB.LoginMiddleQuery(
				RUserDataUpdate,
				connectionIndex,
				"EXEC dbo.TP_USER_PUNISH_DATA_UPDATE %d, %d, %d",
				userIndex,
				punishState,
                punishTime );

			// 090618 ShinJS --- Auto Punish 정보 갱신에 대한 로그 저장

			
			// 080725 LUJ, 결과 전송
			{
				MSG_DWORD5 message = *m;
				message.Protocol	= MP_RM_USER_SET_DATA_ACK;
				
				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}
			
			break;
		}
	case MP_RM_USER_DELETE_SYN:
		{
			const MSGBASE* m = ( MSGBASE* )message;

			DeleteUser( connectionIndex, m->dwObjectID );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%d\' )",
				RecoveryLogRemovePlayerFromUser,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID );

			break;
		}
	case MP_RM_USER_CREATE_SYN:
		{
			const CHARACTERMAKEINFO* m = ( CHARACTERMAKEINFO* )message;

			UserAdd( connectionIndex, *m );
			break;
		}
	case MP_RM_USER_RESTORE_SYN:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			RestoreUser( connectionIndex, m->dwObjectID, m->Name );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%s\' )",
				RecoveryLogRestorePlayerToUser,
				user.mId.c_str(),
				"playerIndex",
				m->dwObjectID,
				"playerName",
				m->Name );
			break;
		}
	case MP_RM_USER_GAMEROOM_SET_SYN:
		{
			const MSG_DWORD* const receivedMessage = (MSG_DWORD*)message;
			const DWORD gameRoomPoint = receivedMessage->dwData;
			const DWORD userIndex = receivedMessage->dwObjectID;

			g_DB.FreeMiddleQuery(
				0,
				0,
				"UPDATE TB_GAMEROOM SET PCROOM_POINT = %d WHERE USER_IDX = %d",
				gameRoomPoint,
				userIndex);

			{
				MSGBASE sendMessage;
				ZeroMemory(
					&sendMessage,
					sizeof(sendMessage));
				sendMessage.Category = MP_RM_USER;
				sendMessage.Protocol = MP_RM_USER_GAMEROOM_SET_ACK;				
				NETWORK->Send(
					connectionIndex,
					sendMessage,
					sizeof(sendMessage));
			}

			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseJobLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_JOB_LOG;
		message.Protocol	= MP_RM_JOB_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_JOB_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST*	m			= ( MSG_RM_LOG_REQUEST* )message;
			const DWORD					playerIndex = m->dwObjectID;
			
			g_DB.LogMiddleQuery(
				RGetJobLogSize,
				connectionIndex,
				"EXEC dbo.TP_JOB_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				playerIndex,
				m->mBeginDate,
				m->mEndDate );

			//GetJobLogSize( connectionIndex, m->dwObjectID, m->mBeginDate, m->mEndDate );
			break;
		}
	case MP_RM_JOB_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_JOB_LOG;
			message.Protocol	= MP_RM_JOB_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseGuildScoreLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_GUILD_SCORE_LOG;
		message.Protocol	= MP_RM_GUILD_SCORE_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_GUILD_SCORE_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetGuildScoreLogSize,
				connectionIndex,
				"EXEC dbo.TP_GUILD_SCORE_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				m->dwObjectID,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_GUILD_SCORE_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_GUILD_SCORE_LOG;
			message.Protocol	= MP_RM_GUILD_SCORE_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseFamilyCropLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_FAMILY_CROP_LOG;
		message.Protocol	= MP_RM_FAMILY_CROP_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_FAMILY_CROP_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetFarmCropLogSize,
				connectionIndex,
				"EXEC dbo.TP_FARM_CROP_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				m->dwObjectID,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_FAMILY_CROP_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_FAMILY_CROP_LOG;
			message.Protocol	= MP_RM_FAMILY_CROP_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseFamilyPointLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_FAMILY_POINT_LOG;
		message.Protocol	= MP_RM_FAMILY_POINT_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_FAMILY_POINT_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetFamilyPointLogSize,
				connectionIndex,
				"EXEC dbo.TP_FAMILY_POINT_LOG_SIZE_SELECT %d, \'%s\', \'%s\'",
				m->dwObjectID,
				m->mBeginDate,
				m->mEndDate );
			break;
		}
	case MP_RM_FAMILY_POINT_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_FAMILY_POINT_LOG;
			message.Protocol	= MP_RM_FAMILY_POINT_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

void CMSSystem::ParsePermission( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_PERMISSION;
		message.Protocol	= MP_RM_PERMISSION_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_PERMISSION_ADD_SYN:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			AddPermission( connectionIndex, m->Name );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%s\' )",
				RecoveryPermissionAddIp,
				user.mId.c_str(),
				"IP",
				m->Name );
			break;
		}
	case MP_RM_PERMISSION_REMOVE_SYN:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			RemovePermission( connectionIndex, m->Name );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%s\' )",
				RecoveryPermissionRemoveIp,
				user.mId.c_str(),
				"IP",
				m->Name );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseOperator( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_OPERATOR_GET_SYN:
		{
			char buffer[ MAX_PATH ];

			{
				sprintf(
					buffer,
					"SELECT TOP %d OPERIDX, OPERPOWER, OPERID, OPERNAME, CONVERT( VARCHAR( 6 ), OPERREGDATE, 12 ) FROM TB_OPERATOR",
					MAX_ROW_NUM );

				g_DB.LoginQuery(
					RGetOperator,
					connectionIndex,
					buffer );
			}

			{
				sprintf(
					buffer,
					"SELECT TOP %d IPADRESS FROM TB_OPERIP",
					MAX_ROW_NUM );
				
				g_DB.LoginQuery(
					RGetOperatorIp,
					connectionIndex,
					buffer );
			}

			break;
		}
	case MP_RM_OPERATOR_ADD_SYN:
		{
			const MSG_RM_OPERATOR*					m				= ( MSG_RM_OPERATOR* )message;
			const MSG_RM_OPERATOR::Operator&		data			= m->mOperator[ 0 ];

			const char*		defaultPassword = "luna";
			const eGM_POWER	defaultRank		= eGM_POWER_MONITOR;

			g_DB.LoginMiddleQuery(
				RAddOperator,
				connectionIndex,
				"EXEC dbo.TP_OPERATOR_INSERT \'%s\', \'%s\', \'%s\', %d",
				data.mId,
				data.mName,
				defaultPassword,
				defaultRank );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%s\', \'%s\', \'%s\' )",
				RecoveryOperatorAdd,
				user.mId.c_str(),
				"ID",
				data.mId,
				"name",
				data.mName );

			break;
		}
	case MP_RM_OPERATOR_REMOVE_SYN:
		{
			const MSGBASE* m = ( MSGBASE* )message;

			g_DB.LoginMiddleQuery(
				0,
				0,
				"DELETE TB_OPERATOR WHERE OPERIDX = %d",
				m->dwObjectID );

			{
				MSGBASE message;
				message.Category	= MP_RM_OPERATOR;
				message.Protocol	= MP_RM_OPERATOR_REMOVE_ACK;
				message.dwObjectID	= m->dwObjectID;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}			

			break;
		}
	case MP_RM_OPERATOR_SET_SYN:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;
			
			g_DB.LoginMiddleQuery(
				RUpdateOperator,
				connectionIndex,
				"EXEC dbo.TP_OPERATOR_UPDATE %d, %d",
				m->dwObjectID,	// operator index
				m->dwData );	// rank

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
				RecoveryOperatorSet,
				user.mId.c_str(),
				"index",
				m->dwObjectID,
				"rank",
				m->dwData );

			break;
		}
	case MP_RM_OPERATOR_IP_ADD_SYN:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			g_DB.LoginMiddleQuery(
				RAddOperatorIp,
				connectionIndex,
				"EXEC dbo.TP_OPERATOR_IP_INSERT \'%s\'",
				m->Name );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%s\' )",
				RecoveryOperatorAddIp,
				user.mId.c_str(),
				"IP",
				m->Name );
			break;
		}
	case MP_RM_OPERATOR_IP_REMOVE_SYN:
		{
			const MSG_NAME* m = ( MSG_NAME* )message;

			g_DB.LoginMiddleQuery(
				RRemoveOperatorIp,
				connectionIndex,
				"EXEC dbo.TP_OPERATOR_IP_DELETE \'%s\'",
				m->Name );

			g_DB.LogMiddleQuery(
				0,
				0,
				"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', \'%s\' )",
				RecoveryOperatorRemoveIp,
				user.mId.c_str(),
				"IP",
				m->Name );
			break;
		}
	}
}


void CMSSystem::ParseOperatorLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_OPERATOR_LOG;
		message.Protocol	= MP_RM_OPERATOR_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_OPERATOR_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetOperatorLogSize,
				connectionIndex,
				"EXEC dbo.TP_OPERATOR_LOG_SIZE_SELECT \'%s\', \'%s\', %d",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick );

			break;
		}
	}
}


void CMSSystem::ParsePassword( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	switch( message->Protocol )
	{
	case MP_RM_PASSWORD_SET_SYN:
		{
			const MSG_NAME2* m = ( MSG_NAME2* )message;

			const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

			if( user.mId.empty() )
			{
				return;
			}

			g_DB.LoginMiddleQuery(
				RSetPassword,
				connectionIndex,
				"EXEC dbo.TP_PASSWORD_UPDATE \'%s\', \'%s\', \'%s\'",
				user.mId.c_str(),
				m->str1,
				m->str2 );

			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CMSSystem::ParseNameLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	switch( message->Protocol )
	{
	case MP_RM_NAME_LOG_GET_SYN:
		{
			const MSG_RM_RENAME_LOG_REQUEST* const m = ( MSG_RM_RENAME_LOG_REQUEST* )message;

			if( m->dwObjectID )
			{
				g_DB.LogQuery(
					RGetNameLog,
					connectionIndex,
					"SELECT TOP %d IDX, CHARACTER_BEFORE, CHARACTER_AFTER, CONVERT( VARCHAR( 6 ), REG_DATE, 12 ) + ' ' + CONVERT( VARCHAR( 8 ), REG_DATE, 8 ) FROM TB_CHARRENAME_LOG WHERE CHARACTER_IDX = %d",
					MAX_ROW_NUM,
					m->dwObjectID );
			}
			else
			{
				g_DB.LogQuery(
					RGetNameLog,
					connectionIndex,
					"SELECT TOP %d IDX, CHARACTER_BEFORE, CHARACTER_AFTER, CONVERT( VARCHAR( 6 ), REG_DATE, 12 ) + ' ' + CONVERT( VARCHAR( 8 ), REG_DATE, 8 ) FROM TB_CHARRENAME_LOG WHERE CHARACTER_BEFORE LIKE \'%s\' OR CHARACTER_AFTER LIKE \'%s\'",
					MAX_ROW_NUM,
					m->mKeyword,
					m->mKeyword );
			}

			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


// 080401 LUJ, 농장 로그
void CMSSystem::ParseFarmLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_FARM_LOG;
		message.Protocol	= MP_RM_FARM_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_FARM_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetFarmLogSize,
				connectionIndex,
				"EXEC dbo.TP_FARM_LOG_SIZE_SELECT \'%s\', \'%s\', %d",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick );
			break;
		}
	case MP_RM_FARM_LOG_STOP_SYN:
		{
			//g_pMSSystem->mStopQuerySet.insert( connectionIndex );

			MSGROOT message;
			message.Category	= MP_RM_FARM_LOG;
			message.Protocol	= MP_RM_FARM_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


// 080401 LUJ, 유료 아이템 구입 로그
void CMSSystem::ParseItemShopLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM_SHOP_LOG;
		message.Protocol	= MP_RM_ITEM_SHOP_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );

		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_ITEM_SHOP_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetItemShopLogSize,
				connectionIndex,
				"EXEC dbo.TP_ITEM_SHOP_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick,
				m->dwObjectID );
			break;
		}
	case MP_RM_ITEM_SHOP_LOG_STOP_SYN:
		{
			MSGROOT message;
			message.Category	= MP_RM_ITEM_SHOP_LOG;
			message.Protocol	= MP_RM_ITEM_SHOP_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

// 080523 LUJ, 낚시 로그
void CMSSystem::ParseFishLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_FISH_LOG;
		message.Protocol	= MP_RM_FISH_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_FISH_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetFishLogSize,
				connectionIndex,
				"EXEC dbo.TP_FISH_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick,
				m->dwObjectID );
			break;
		}
	case MP_RM_FISH_LOG_STOP_SYN:
		{
			MSGROOT message;
			message.Category	= MP_RM_FISH_LOG;
			message.Protocol	= MP_RM_FISH_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


// 080630 LUJ, 농장 가축 로그
void CMSSystem::ParseLivestockLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_LIVESTOCK_LOG;
		message.Protocol	= MP_RM_LIVESTOCK_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_LIVESTOCK_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetLivestockLogSize,
				connectionIndex,
				"EXEC dbo.TP_FARM_ANIMAL_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick,
				m->dwObjectID );
			break;
		}
	case MP_RM_LIVESTOCK_LOG_STOP_SYN:
		{
			MSGROOT message;
			message.Category	= MP_RM_LIVESTOCK_LOG;
			message.Protocol	= MP_RM_LIVESTOCK_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


// 080716 LUJ, 펫 로그
void CMSSystem::ParsePetLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_PET_LOG;
		message.Protocol	= MP_RM_PET_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_PET_LOG_SIZE_SYN:
		{
			const MSG_RM_PET_LOG_REQUEST* m = ( MSG_RM_PET_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetPetLogSize,
				connectionIndex,
				"EXEC dbo.TP_PET_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d, %d",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick,
				m->dwObjectID,
				m->mPetIndex );
			break;
		}
	case MP_RM_PET_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_PET_LOG;
			message.Protocol	= MP_RM_PET_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


// 080716 LUJ, 전역 아이템 로그
void CMSSystem::ParseItemGlobalLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM_GLOBAL_LOG;
		message.Protocol	= MP_RM_ITEM_GLOBAL_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_ITEM_GLOBAL_LOG_SYN:
		{
			const MSG_RM_ITEM_GLOBAL_LOG_REQUEST* m = ( MSG_RM_ITEM_GLOBAL_LOG_REQUEST* )message;

			g_DB.LogQuery(
				RGetItemGlobalLog,
				connectionIndex,
				"EXEC dbo.TP_ITEM_GLOBAL_LOG_SELECT \'%s\', \'%s\', %d, %d, %d, %d, %d, 0",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick,
				m->dwObjectID,
				m->mLogType,
				m->mItemIndex,
				m->mItemDbIndex );

			// 080716 LUJ, 초기화하기 위해 메시지를 보낸다
			{
				MSGBASE message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_RM_ITEM_GLOBAL_LOG;
				message.Protocol	= MP_RM_ITEM_GLOBAL_LOG_READY_ACK;

				NETWORK->Send( connectionIndex, message, sizeof( message ) );
			}

			break;
		}
	case MP_RM_ITEM_GLOBAL_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_ITEM_GLOBAL_LOG;
			message.Protocol	= MP_RM_ITEM_GLOBAL_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

// 081021 LUJ, 권한 관리
void CMSSystem::ParseAuthority( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mIp.empty() )
	{
		return;
	}
	// 081021 LUJ, 권한 관리는 마스터만 가능하다
	else if( user.mPower != eGM_POWER_MASTER )
	{
		MSGROOT message;
		message.Category	= MP_RM_AUTHORITY;
		message.Protocol	= MP_RM_AUTHORITY_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_AUTHORITY_GET_SYN:
		{
			GetAuthorityToUser(
				connectionIndex,
				eGM_POWER_MASTER,
				AuthorityTypeNone );
			break;
		}
	case MP_RM_AUTHORITY_SET_SYN:
		{
			const MSG_RM_AUTHORITY* m = ( MSG_RM_AUTHORITY* )message;

			CUserManager::AuthorityList authorityList;

			eGM_POWER power = eGM_POWER_MAX;
			
			// 081031 LUJ, 권한마다 쿼리를 대량으로 날리는 것은 비효율적이지만,
			//				가급적 간단하게 수정한다
			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_AUTHORITY::Authority& authority = m->mAuthority[ i ];

				power = authority.mPower;

				g_DB.LoginMiddleQuery(
					0,
					connectionIndex,
					"EXEC dbo.TP_OPERATOR_AUTHORITY_UPDATE %d, %d, \'%s%s\'",
					authority.mType,
					authority.mPower,
					authority.mReadable ? "r" : "x",
					authority.mWriteable ? "w" : "x" );
			}

			if( eGM_POWER_MAX == power )
			{
				MSGBASE message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_RM_AUTHORITY;
				message.Protocol	= MP_RM_AUTHORITY_SET_ACK;
				break;
			}

			// 081127 LUJ, 메모리 저장
			CUserManager::GetInstance().SetAuthority( power, *m );
			// 081127 LUJ, 버전 정보 증가 결과를 받아 성공 여부를 알린다
			IncreaseAuthorityVersion( connectionIndex );			
			break;
		}
	}
}

// 081027 LUJ, 길드 토너먼트
void CMSSystem::ParseTournament( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_TOURNAMENT;
		message.Protocol	= MP_RM_TOURNAMENT_GET_ROUND_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_TOURNAMENT_GET_ROUND_SYN:
		{
			g_DB.FreeMiddleQuery(
				RGetTournamentCount,
				connectionIndex,
				"SELECT MAX( GTCOUNT ) FROM TB_GT_INFO WITH (NOLOCK)" );
			break;
		}
	case MP_RM_TOURNAMENT_GET_DATA_SYN:
		{
			const MSGBASE*	m				= ( MSGBASE* )message;
			const DWORD		tournamentCount	= m->dwObjectID;

			GetTournamentGuild(
				connectionIndex,
				tournamentCount,
				0 );
			break;
		}
	}
}

// 081027 LUJ, 怨듭꽦 濡쒓렇
void CMSSystem::ParseSiegeLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_SIEGE_LOG;
		message.Protocol	= MP_RM_SIEGE_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_SIEGE_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetSiegeLogSize,
				connectionIndex,
				"EXEC dbo.TP_SIEGE_LOG_SIZE_SELECT \'%s\', \'%s\', %d",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick );
			break;
		}
	case MP_RM_SIEGE_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_SIEGE_LOG;
			message.Protocol	= MP_RM_SIEGE_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

// 081205 LUJ, 梨꾪똿 濡쒓렇
void CMSSystem::ParseChatLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_CHAT_LOG;
		message.Protocol	= MP_RM_CHAT_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_CHAT_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetChatLogSize,
				connectionIndex,
				"EXEC dbo.TP_CHAT_LOG_SIZE_SELECT \'%s\', \'%s\', %d",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick );
			break;
		}
	case MP_RM_CHAT_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_CHAT_LOG;
			message.Protocol	= MP_RM_CHAT_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

// 081027 LUJ, 패밀리 로그
void CMSSystem::ParseFamilyLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_FAMILY_LOG;
		message.Protocol	= MP_RM_FAMILY_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_FAMILY_LOG_SIZE_SYN:
		{
			const MSG_RM_FAMILY_LOG_REQUEST* m = ( MSG_RM_FAMILY_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetFamilyLogSize,
				connectionIndex,
				"EXEC dbo.TP_FAMILY_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d, %d",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick,
				m->mFamilyIndex,
				m->mPlayerIndex );
			break;
		}
	case MP_RM_FAMILY_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_FAMILY_LOG;
			message.Protocol	= MP_RM_FAMILY_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

// 090122 LUJ, 스크립트 변조 로그
void CMSSystem::ParseScriptHackLog( DWORD connectionIndex, const MSGROOT* message, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( message->Category ), message->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_SCRIPT_HACK_LOG;
		message.Protocol	= MP_RM_SCRIPT_HACK_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( message->Protocol )
	{
	case MP_RM_SCRIPT_HACK_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* const m = ( MSG_RM_LOG_REQUEST* )message;

			g_DB.LogMiddleQuery(
				RGetScriptHackLogSize,
				connectionIndex,
				"EXEC dbo.TP_SCRIPT_HACK_LOG_SIZE_SELECT \'%s\', \'%s\', %d",
				m->mBeginDate,
				m->mEndDate,
				m->mRequestTick );
			break;
		}
	case MP_RM_SCRIPT_HACK_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_SCRIPT_HACK_LOG;
			message.Protocol	= MP_RM_SCRIPT_HACK_LOG_STOP_ACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

// 090406 LUJ, 돈 로그
void CMSSystem::ParseMoneyLog( DWORD connectionIndex, const MSGROOT* receivedMessage, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( receivedMessage->Category ), receivedMessage->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_MONEY_LOG;
		message.Protocol = MP_RM_MONEY_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( receivedMessage->Protocol )
	{
	case MP_RM_MONEY_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* const message = ( MSG_RM_LOG_REQUEST* )receivedMessage;
			const DWORD playerIndex = message->dwObjectID;

			g_DB.LogMiddleQuery(
				RGetMoneyLogSize,
				connectionIndex,
				"EXEC dbo.TP_MONEY_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d",
				message->mBeginDate,
				message->mEndDate,
				playerIndex,
				message->mRequestTick );
			break;
		}
	case MP_RM_MONEY_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category = MP_RM_MONEY_LOG;
			message.Protocol = MP_RM_MONEY_LOG_STOP_ACK;
			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

void CMSSystem::ParseAutoNoteLog( DWORD connectionIndex, const MSGROOT* receivedMessage, size_t size )
{
	if( ! CUserManager::GetInstance().IsAuthorize( RecoveryCategory( receivedMessage->Category ), receivedMessage->Protocol, connectionIndex ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_AUTONOTE_LOG;
		message.Protocol = MP_RM_AUTONOTE_LOG_NACK_BY_AUTH;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	switch( receivedMessage->Protocol )
	{
	case MP_RM_AUTONOTE_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* const message = ( MSG_RM_LOG_REQUEST* )receivedMessage;
			const DWORD playerIndex = message->dwObjectID;

			g_DB.LogMiddleQuery(
				RGetAutoNoteLogSize,
				connectionIndex,
				"EXEC dbo.TP_AUTONOTE_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d",
				message->mBeginDate,
				message->mEndDate,
				playerIndex,
				message->mRequestTick );
			break;
		}
	case MP_RM_AUTONOTE_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category = MP_RM_AUTONOTE_LOG;
			message.Protocol = MP_RM_AUTONOTE_LOG_STOP_ACK;
			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}

void CMSSystem::ParseHouseLog( DWORD connectionIndex, const MSGROOT* receivedMessage, size_t size )
{
	if(FALSE == CUserManager::GetInstance().IsAuthorize(RecoveryCategory(receivedMessage->Category), receivedMessage->Protocol, connectionIndex))
	{
		MSGROOT message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_HOUSE_LOG;
		message.Protocol = MP_RM_HOUSE_LOG_NACK_BY_AUTH;

		NETWORK->Send(connectionIndex, message, sizeof(message));
		return;
	}

	switch(receivedMessage->Protocol)
	{
	case MP_RM_HOUSE_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* const message = (MSG_RM_LOG_REQUEST*)receivedMessage;
			const DWORD playerIndex = message->dwObjectID;

			g_DB.LogMiddleQuery(
				RGetHouseLogSize,
				connectionIndex,
				"EXEC dbo.TP_HOUSE_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d",
				message->mBeginDate,
				message->mEndDate,
				playerIndex,
				message->mRequestTick);
			break;
		}
	case MP_RM_HOUSE_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory(&message, sizeof(message));
			message.Category = MP_RM_HOUSE_LOG;
			message.Protocol = MP_RM_HOUSE_LOG_STOP_ACK;
			NETWORK->Send(connectionIndex, message, sizeof(message));
			break;
		}
	default:
		{
			ASSERT(0);
			break;
		}
	}
}

void CMSSystem::ParseCookLog( DWORD connectionIndex, const MSGROOT* receivedMessage, size_t size )
{
	if(FALSE == CUserManager::GetInstance().IsAuthorize(RecoveryCategory(receivedMessage->Category), receivedMessage->Protocol, connectionIndex))
	{
		MSGROOT message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_COOK_LOG;
		message.Protocol = MP_RM_COOK_LOG_NACK_BY_AUTH;

		NETWORK->Send(connectionIndex, message, sizeof(message));
		return;
	}

	switch(receivedMessage->Protocol)
	{
	case MP_RM_COOK_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* const message = (MSG_RM_LOG_REQUEST*)receivedMessage;
			const DWORD playerIndex = message->dwObjectID;

			g_DB.LogMiddleQuery(
				RGetCookLogSize,
				connectionIndex,
				"EXEC dbo.TP_COOK_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d",
				message->mBeginDate,
				message->mEndDate,
				playerIndex,
				message->mRequestTick);
			break;
		}
	case MP_RM_COOK_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory(&message, sizeof(message));
			message.Category = MP_RM_COOK_LOG;
			message.Protocol = MP_RM_COOK_LOG_STOP_ACK;
			NETWORK->Send(connectionIndex, message, sizeof(message));
			break;
		}
	case MP_RM_COOK_RECIPE_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* const message = (MSG_RM_LOG_REQUEST*)receivedMessage;
			const DWORD playerIndex = message->dwObjectID;

			g_DB.LogMiddleQuery(
				RGetRecipeLogSize,
				connectionIndex,
				"EXEC dbo.TP_RECIPE_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d",
				message->mBeginDate,
				message->mEndDate,
				playerIndex,
				message->mRequestTick);
			break;
		}
	case MP_RM_COOK_RECIPE_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory(&message, sizeof(message));
			message.Category = MP_RM_COOK_LOG;
			message.Protocol = MP_RM_COOK_RECIPE_LOG_STOP_ACK;
			NETWORK->Send(connectionIndex, message, sizeof(message));
			break;
		}
	default:
		{
			ASSERT(0);
			break;
		}
	}
}

void CMSSystem::ParseConsignLog( DWORD connectionIndex, const MSGROOT* receivedMessage, size_t size )
{
	if(FALSE == CUserManager::GetInstance().IsAuthorize(RecoveryCategory(receivedMessage->Category), receivedMessage->Protocol, connectionIndex))
	{
		MSGROOT message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_CONSIGN_LOG;
		message.Protocol = MP_RM_CONSIGN_LOG_NACK_BY_AUTH;

		NETWORK->Send(connectionIndex, message, sizeof(message));
		return;
	}

	switch(receivedMessage->Protocol)
	{
	case MP_RM_CONSIGN_LOG_SIZE_SYN:
		{
			const MSG_RM_CONSIGN_LOG_REQUEST* const message = (MSG_RM_CONSIGN_LOG_REQUEST*)receivedMessage;
			const DWORD playerIndex = message->dwObjectID;
			const DWORD itemDBIndex = message->mItemDBIndex;
			const DWORD consignIndex = message->mConsignIndex;

			g_DB.LogMiddleQuery(
				RGetConsignLogSize,
				connectionIndex,
				"EXEC dbo.TP_CONSIGN_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d, %d, %d",
				message->mBeginDate,
				message->mEndDate,
				playerIndex,
				itemDBIndex,
				consignIndex,
				message->mRequestTick);
			break;
		}
	case MP_RM_CONSIGN_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory(&message, sizeof(message));
			message.Category = MP_RM_CONSIGN_LOG;
			message.Protocol = MP_RM_CONSIGN_LOG_STOP_ACK;
			NETWORK->Send(connectionIndex, message, sizeof(message));
			break;
		}

	default:
		{
			ASSERT(0);
			break;
		}
	}
}

void CMSSystem::ParseForbidChatLog( DWORD connectionIndex, const MSGROOT* receivedMessage, size_t size )
{
	if(FALSE == CUserManager::GetInstance().IsAuthorize(RecoveryCategory(receivedMessage->Category), receivedMessage->Protocol, connectionIndex))
	{
		MSGROOT message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_FORBID_CHAT_LOG;
		message.Protocol = MP_RM_FORBID_CHAT_LOG_NACK_BY_AUTH;

		NETWORK->Send(connectionIndex, message, sizeof(message));
		return;
	}

	switch(receivedMessage->Protocol)
	{
	case MP_RM_FORBID_CHAT_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* const message = (MSG_RM_LOG_REQUEST*)receivedMessage;
			const DWORD playerIndex = message->dwObjectID;

			g_DB.FreeMiddleQuery(
				RGetForbidChatLogSize,
				connectionIndex,
				"EXEC dbo.TP_FORBID_CHAT_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d",
				message->mBeginDate,
				message->mEndDate,
				playerIndex,
				message->mRequestTick);
			break;
		}
	case MP_RM_FORBID_CHAT_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory(&message, sizeof(message));
			message.Category = MP_RM_FORBID_CHAT_LOG;
			message.Protocol = MP_RM_FORBID_CHAT_LOG_STOP_ACK;
			NETWORK->Send(connectionIndex, message, sizeof(message));
			break;
		}
	default:
		{
			ASSERT(0);
			break;
		}
	}
}

void CMSSystem::ParseGameRoomPointLog( DWORD connectionIndex, const MSGROOT* receivedMessage, size_t size )
{
	if(FALSE == CUserManager::GetInstance().IsAuthorize(RecoveryCategory(receivedMessage->Category), receivedMessage->Protocol, connectionIndex))
	{
		MSGROOT message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_GAMEROOM_POINT_LOG;
		message.Protocol = MP_RM_GAMEROOM_POINT_LOG_NACK_BY_AUTH;

		NETWORK->Send(connectionIndex, message, sizeof(message));
		return;
	}

	switch(receivedMessage->Protocol)
	{
	case MP_RM_GAMEROOM_POINT_LOG_SIZE_SYN:
		{
			const MSG_RM_LOG_REQUEST* const message = (MSG_RM_LOG_REQUEST*)receivedMessage;
			const DWORD userIndex = message->dwObjectID;

			g_DB.LogMiddleQuery(
				RGetGameRoomPointLogSize,
				connectionIndex,
				"EXEC dbo.TP_GAMEROOM_POINT_LOG_SIZE_SELECT \'%s\', \'%s\', %d, %d",
				message->mBeginDate,
				message->mEndDate,
				userIndex,
				message->mRequestTick);
			break;
		}
	case MP_RM_GAMEROOM_POINT_LOG_STOP_SYN:
		{
			MSGROOT message;
			ZeroMemory(&message, sizeof(message));
			message.Category = MP_RM_GAMEROOM_POINT_LOG;
			message.Protocol = MP_RM_GAMEROOM_POINT_LOG_STOP_ACK;
			NETWORK->Send(connectionIndex, message, sizeof(message));
			break;
		}
	default:
		{
			ASSERT(0);
			break;
		}
	}
}
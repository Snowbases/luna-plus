#include "stdafx.h"
#include "DBMessage.h"
#include "Network.h"
#include "UserManager.h"
#include "MSSystem.h"

void RLoginOperator( LPMIDDLEQUERY pData, LPDBMESSAGE pMessage)
{
	const DWORD connectionIndex = pMessage->dwID;

	if( 0 == pMessage->dwResult )
	{
		MSG_DWORD message;
		message.Category	= MP_RM_CONNECT;
		message.Protocol	= MP_RM_CONNECT_NACK;
		message.dwData		= 0;
		
		NETWORK->Send(connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = pData[ 0 ];

	const DWORD check = atoi( ( char* )record.Data[ 0 ] );

	if( check )
	{
		switch( check )
		{
		case 1:
			{
				PutLog( "log in failed by invalid IP" );
				break;
			}
		case 2:
			{
				PutLog( "log in failed by no existed ID" );
				break;
			}
		case 3:
			{
				PutLog( "log in failed by wrong password" );
				break;
			}
		}


		MSG_DWORD message;
		message.Category		= MP_RM_CONNECT;
		message.Protocol		= MP_RM_CONNECT_NACK;
		message.dwData			= check;

		NETWORK->Send(connectionIndex, message, sizeof( message ) );
		return;
	}
	else
	{
		MSGBASE message;
		message.Category	= MP_RM_CONNECT;
		message.Protocol	= MP_RM_CONNECT_ACK;

		NETWORK->Send(connectionIndex, message, sizeof( message ) );
	}

	const char*	id				= 	( char* )record.Data[ 1 ];
	const char*	name			= 	( char* )record.Data[ 2 ];
	const char*	ip				= 	( char* )record.Data[ 3 ];
	const char*	registedDate	=	( char* )record.Data[ 4 ];
	
	const eGM_POWER power		=	eGM_POWER( atoi( ( char* )record.Data[ 5 ] ) );
	// 081021 LUJ, 권한 정보의 버전을 받는다. 이를 이용하여, 사용자 로그인 시마다
	//			새로 권한 정보를 쿼리해야하는지 알아낸다
	const DWORD	authorityVersion=	atoi( ( char* )record.Data[ 6 ] );

	CUserManager& userManager = CUserManager::GetInstance();

	if( ! userManager.AddUser(
			connectionIndex,
			id,
			name,
			ip,
			registedDate,
			power ) )
	{
		ASSERT( 0 );
	}
	
	GetAuthorityToServer( power );
	PutLog(
		"%s connected at %s. Rank is %d",
		id,
		ip,
		power );
}

void RGetUserList( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_SEARCH;
		message.Protocol	= MP_RM_SEARCH_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	for(DWORD i = 0; i < dbMessage->dwResult; ++i)
	{
		const MIDDLEQUERYST& record	= query[ i ];
		const DWORD	userIndex = atoi( ( char* )record.Data[ 0 ] );
		LPCTSTR userName = LPCTSTR(record.Data[1]);
		LPCTSTR enpangName = LPCTSTR(record.Data[2]);

		std::string koreanName = std::string(" ") + std::string(enpangName);

		GetPlayerList(
			connectionIndex,
			userIndex,
			0 == _tcslen(enpangName) ? userName : koreanName.c_str(),
			"",
			0);

		MSG_NAME2 message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_SEARCH;
		message.Protocol	= MP_RM_SEARCH_USER_NAME_ACK;
		message.dwObjectID	= userIndex;
		StringCopySafe(
			message.str1,
			userName,
			_countof(message.str1));
		StringCopySafe(
			message.str2,
			koreanName.c_str(),
			_countof(message.str2));

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
}


void GetPlayerList( DWORD connectionIndex, DWORD userIndex, const char* userName, const char* keyword, DWORD beginPlayerIndex )
{
	g_DB.FreeQuery(
		RGetPlayerList,
		connectionIndex,
		"EXEC dbo.TP_CHARACTER_SELECT %d, \'%s\', \'%s\', %d",
		userIndex,
		userName,
		keyword,
		beginPlayerIndex );
}


void RGetPlayerList( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_OPEN_ACK	message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_SEARCH;
		message.Protocol	= MP_RM_SEARCH_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		MSG_RM_OPEN_ACK::Data&	data	= message.mData[ i ];
		const QUERYST&			record	= query[ i ];

		data.mUserIndex = atoi( ( char* )record.Data[ 0 ] );
		StringCopySafe( data.mUserName,		( char* )record.Data[ 1 ], sizeof( data.mUserName ) );

		data.mPlayerIndex = atoi( ( char* )record.Data[ 2 ] );
		StringCopySafe( data.mPlayerName,	( char* )record.Data[ 3 ], sizeof( data.mPlayerName ) );

		data.mStandIndex = atoi( ( char* )record.Data[ 4 ] );
	}

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		MSG_RM_OPEN_ACK::Data& data = message.mData[ MAX_ROW_NUM - 1 ];

		const char* keyword = ( char* )query[ MAX_ROW_NUM - 1 ].Data[ 5 ];

		GetPlayerList(
			connectionIndex,
			data.mUserIndex,
			data.mUserName,
			keyword,
			data.mPlayerIndex );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void GetPlayerLoginId( DWORD connectionIndex, DWORD userIndex, DWORD playerIndex )
{
	g_DB.LoginMiddleQuery(
		RGetPlayerLoginId,
		connectionIndex,
		"SELECT %d, PROPID, ID_LOGINID, ISNULL(EN_LOGINID, \'\') FROM CHR_LOG_INFO WHERE PROPID = %d",
		playerIndex,
		userIndex);
}


void RGetPlayerLoginId( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_GET_USER_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_NAME2_DWORD message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_PLAYER;
	message.Protocol = MP_RM_PLAYER_GET_USER_ACK;
	message.dwObjectID = atoi( ( char* )record.Data[ 0 ] );	// 플레이어 번호
	message.dwData = atoi( ( char* )record.Data[ 1 ] );	// 유저 번호
	StringCopySafe(
		message.Name1,
		LPCTSTR(record.Data[ 2 ]),
		_countof(message.Name1));
	StringCopySafe(
		message.Name2,
		LPCTSTR(record.Data[3]),
		_countof(message.Name2));

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}


void RGetPlayerData( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_GET_DATA_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const QUERYST&	record = query[ 0 ];
	MSG_RM_PLAYER_DATA	message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_GET_DATA_ACK;
	}

	{
		MSG_RM_PLAYER_DATA::Player& player = message.mPlayer;

		player.mIndex					= atoi( ( char* )record.Data[ 0 ] );		
		player.mBadFame					= atoi( ( char* )record.Data[ 1 ] );
		player.mStrength				= atoi( ( char* )record.Data[ 2 ] );
		player.mDexterity				= atoi( ( char* )record.Data[ 3 ] );
		player.mVitality				= atoi( ( char* )record.Data[ 4 ] );
		player.mIntelligence			= atoi( ( char* )record.Data[ 5 ] );
		player.mWisdom					= atoi( ( char* )record.Data[ 6 ] );
		player.mLife					= atoi( ( char* )record.Data[ 7 ] );
		player.mMana					= atoi( ( char* )record.Data[ 8 ] );
		player.mGrade					= atoi( ( char* )record.Data[ 9 ] );
		player.mMaxGrade				= atoi( ( char* )record.Data[ 10 ] );
		player.mExp						= strtoul( ( char* )record.Data[ 11 ], 0, 10 );
		player.mGradeUpPoint			= atoi( ( char* )record.Data[ 12 ] );
		player.mMoney					= strtoul( ( char* )record.Data[ 13 ], 0, 10 );
		player.mPlayTime				= atoi( ( char* )record.Data[ 14 ] );
		player.mSkillPoint				= atoi( ( char* )record.Data[ 15 ] );
		player.mRace					= atoi( ( char* )record.Data[ 16 ] );
		player.mExtendedInventorySize	= atoi( ( char* )record.Data[ 17 ] );
		player.mBadFame					= atoi( ( char* )record.Data[ 18 ] );
	}

	{
		MSG_RM_PLAYER_DATA::Position& position = message.mPosition;

		position.mMap	= atoi( ( char* )record.Data[ 19 ] );
		position.mX		= atoi( ( char* )record.Data[ 20 ] );
		position.mY		= atoi( ( char* )record.Data[ 21 ] );
	}

	{
		MSG_RM_PLAYER_DATA::Job& job = message.mJob;

		job.mIndex	= atoi( ( char* )record.Data[ 22 ] );
		job.mData1	= atoi( ( char* )record.Data[ 23 ] );
		job.mData2	= atoi( ( char* )record.Data[ 24 ] );
		job.mData3	= atoi( ( char* )record.Data[ 25 ] );
		job.mData4	= atoi( ( char* )record.Data[ 26 ] );
		job.mData5	= atoi( ( char* )record.Data[ 27 ] );
		job.mData6	= atoi( ( char* )record.Data[ 28 ] );
	}

	{
		MSG_RM_PLAYER_DATA::Guild& guild = message.mGuild;

		guild.mIndex		= atoi( ( char* )record.Data[ 29 ] );
		guild.mRank			= atoi( ( char* )record.Data[ 30 ] );

		StringCopySafe( guild.mName, ( char* )record.Data[ 31 ], sizeof( guild.mName ) );
	}

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	{
		const DWORD userIndex = atoi( ( char* )record.Data[ 32 ] );

		GetPlayerLoginId( connectionIndex, userIndex, message.mPlayer.mIndex );
	}
}


void RGetPlayerExtendedData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	// 주의: MIDDLE QUERY는 필드 20개만 결과로 받을 수 있다
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_GET_DATA_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	MSG_RM_PLAYER_EXTENDED_DATA message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_RM_PLAYER;
	message.Protocol = MP_RM_PLAYER_GET_EXTENDED_DATA_ACK;

	const MIDDLEQUERYST& record = query[ 0 ];
	MSG_RM_PLAYER_EXTENDED_DATA::Player& player = message.mPlayer;

	strncpy( player.mName, ( char* )record.Data[ 0 ], sizeof( player.mName ) - 1 );
	strncpy( player.mGuildNick, ( char* )record.Data[ 1 ], sizeof( player.mGuildNick ) - 1 );
	strncpy( player.mFamilyNick, ( char* )record.Data[ 2 ], sizeof( player.mFamilyNick ) - 1 );
	strncpy( player.mLastPlayedTime, ( char* )record.Data[ 3 ], sizeof( player.mLastPlayedTime ) - 1 );
	// 080610 LUJ, 캐릭터 성별
	player.mGender	= atoi( ( char* )record.Data[ 4 ] );
	player.mRace = RaceType(atoi(LPCTSTR(record.Data[5])));

	MSG_RM_PLAYER_EXTENDED_DATA::History& history = message.mHistory;

	history.mIsEnable	= ( 2 == atoi( ( char* )record.Data[ 6 ] ) );
	strncpy( history.mCreateDate,	( char* )record.Data[ 7 ],	sizeof( history.mCreateDate ) - 1 );
	strncpy( history.mCreateIp,		( char* )record.Data[ 8 ],	sizeof( history.mCreateIp ) - 1 );
	strncpy( history.mDeletedDate,	( char* )record.Data[ 9 ],	sizeof( history.mDeletedDate ) - 1 );
	strncpy( history.mDeletedIp,	( char* )record.Data[ 10 ],	sizeof( history.mDeletedIp ) - 1 );
	strncpy( history.mRecoveredDate,( char* )record.Data[ 11 ], sizeof( history.mRecoveredDate ) - 1 );
	strncpy( history.mRecoveredIp,	( char* )record.Data[ 12 ], sizeof( history.mRecoveredIp ) - 1 );
	// 081020 LUJ, 몬스터 미터 정보 쿼리
	history.mPlayTimeTotal		= atoi( ( char* )record.Data[ 13 ] );
	history.mMonsterKillCount	= atoi( ( char* )record.Data[ 14 ] );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}


// 080328 LUJ, 플레이어가 소속된 패밀리 이름을 얻어낸다
void RGetPlayerFamilyData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( 1 != dbMessage->dwResult )
	{
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_NAME_DWORD message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_PLAYER;
	message.Protocol	= MP_RM_PLAYER_GET_FAMILY_ACK;
	message.dwData		= atoi( ( char* )record.Data[ 0 ] );
	StringCopySafe( message.Name, ( char* )record.Data[ 1 ], sizeof( message.Name ) );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}


void RSetPlayerExtendedData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( 1 != dbMessage->dwResult )
	{
		return;
	}

	const DWORD playerIndex = atoi( ( char* )query[ 0 ].Data[ 0 ] );

	if( playerIndex )
	{
		MSGROOT message;
		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_SET_EXTENDED_DATA_ACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
	else
	{
		MSGROOT message;
		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_SET_EXTENDED_DATA_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
}


void RGetPlayerLicense( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_GET_LICENSE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];
	MSG_RM_PLAYER_LOVE message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_GET_LICENSE_ACK;
		message.dwObjectID	= atoi( ( char* )record.Data[ 0 ] );
	}

	{
		DATE_MATCHING_INFO&	data = message.mDateMatching;

		strncpy( data.szName, ( char* )record.Data[ 0 ], sizeof( data.szName ) );
		data.bIsValid				= true;
		data.byGender				= atoi( ( char* )record.Data[ 1 ] );
		data.dwAge					= atoi( ( char* )record.Data[ 2 ] );
		data.dwRegion				= atoi( ( char* )record.Data[ 3 ] );
		data.dwGoodFeelingDegree	= atoi( ( char* )record.Data[ 4 ] );
		data.pdwGoodFeeling[0]		= atoi( ( char* )record.Data[ 5 ] );
		data.pdwGoodFeeling[1]		= atoi( ( char* )record.Data[ 6 ] );
		data.pdwGoodFeeling[2]		= atoi( ( char* )record.Data[ 7 ] );
		data.pdwBadFeeling[0]		= atoi( ( char* )record.Data[ 8 ] );
		data.pdwBadFeeling[1]		= atoi( ( char* )record.Data[ 9 ] );
	}	

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}


void RPlayerSetLicense( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		ASSERT( 0 );

		MSGROOT message;
		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_SET_LICENSE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	MSGROOT message;
	message.Category	= MP_RM_PLAYER;
	message.Protocol	= MP_RM_PLAYER_SET_LICENSE_ACK;

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}


void GetItemListOnInventory( DWORD connectionIndex, DWORD playerIndex, DWORD beginItemDbIndex )
{
	// 080716 LUJ, 펫 인덱스를 쿼리하도록 추가
	g_DB.FreeQuery(
		RGetItemList,
		connectionIndex,
		"SELECT  TOP %d A.ITEM_DBIDX, A.ITEM_IDX, A.ITEM_POSITION, A.ITEM_DURABILITY, A.ITEM_SEAL, DATEDIFF(second, GetDate(), A.ITEM_ENDTIME), A.ITEM_REMAINTIME,\
		ISNULL( B.REINFORCE_STRENGTH, 0 ), ISNULL( B.REINFORCE_DEXTERITY, 0 ), ISNULL( B.REINFORCE_VITALITY, 0 ), ISNULL( B.REINFORCE_INTELLIGENCE, 0 ),\
		ISNULL( B.REINFORCE_WISDOM, 0 ), ISNULL( B.REINFORCE_LIFE, 0 ), ISNULL( B.REINFORCE_MANA, 0 ), ISNULL( B.REINFORCE_LIFE_RECOVERY, 0 ),\
		ISNULL( B.REINFORCE_MANA_RECOVERY, 0 ), ISNULL( B.REINFORCE_PHYSIC_ATTACK, 0 ), ISNULL( B.REINFORCE_PHYSIC_DEFENCE, 0 ),\
		ISNULL( B.REINFORCE_MAGIC_ATTACK, 0 ), ISNULL( B.REINFORCE_MAGIC_DEFENCE, 0 ), ISNULL( B.REINFORCE_MOVE_SPEED, 0 ), ISNULL( B.REINFORCE_EVADE, 0 ),\
		ISNULL( B.REINFORCE_ACCURACY, 0 ),ISNULL( B.REINFORCE_CRITICAL_RATE, 0 ), ISNULL( B.REINFORCE_CRITICAL_DAMAGE, 0 ), ISNULL( B.REINFORCE_MADEBY, \'\' ),\
		ISNULL( B.MIX_STRENGTH, 0 ), ISNULL( B.MIX_INTELLIGENCE, 0 ), ISNULL( B.MIX_DEXTERITY, 0 ), ISNULL( B.MIX_WISDOM, 0 ), ISNULL( B.MIX_VITALITY, 0 ),\
		ISNULL( B.MIX_MADEBY, \'\' ),ISNULL( B.ENCHANT_INDEX, 0 ), ISNULL( B.ENCHANT_LEVEL, 0 ), ISNULL( B.ENCHANT_MADEBY, \'\'),\
		ISNULL( C.KEY1, 0 ), CAST( ROUND( ISNULL( C.VALUE1, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY2, 0 ), CAST( ROUND( ISNULL( C.VALUE2, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY3, 0 ), CAST( ROUND( ISNULL( C.VALUE3, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY4, 0 ), CAST( ROUND( ISNULL( C.VALUE4, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY5, 0 ), CAST( ROUND( ISNULL( C.VALUE5, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		%d, %d, A.PET_IDX\
		FROM TB_ITEM A LEFT OUTER JOIN TB_ITEM_DROP_OPTION C ON A.ITEM_DBIDX = C.ITEM_DBIDX LEFT OUTER JOIN TB_ITEM_OPTION2 B ON A.ITEM_DBIDX = B.ITEM_DBIDX\
		WHERE (A.CHARACTER_IDX = %d) AND (A.ITEM_DBIDX > %d) AND (A.ITEM_SHOPIDX = 0) ORDER BY A.ITEM_DBIDX",
		MAX_ROW_NUM,
		Item::AreaInventory,
		playerIndex,
		playerIndex,
		beginItemDbIndex );
}


void GetItemListOnShop( DWORD connectionIndex, DWORD userIndex, DWORD beginItemDbIndex )
{
	// 080716 LUJ, 펫 인덱스를 쿼리하도록 추가
	g_DB.FreeQuery(
		RGetItemList,
		connectionIndex,
		"SELECT  TOP %d A.ITEM_DBIDX, A.ITEM_IDX, A.ITEM_POSITION, A.ITEM_DURABILITY, A.ITEM_SEAL, DATEDIFF(second, GetDate(), A.ITEM_ENDTIME), A.ITEM_REMAINTIME,\
		ISNULL( B.REINFORCE_STRENGTH, 0 ), ISNULL( B.REINFORCE_DEXTERITY, 0 ), ISNULL( B.REINFORCE_VITALITY, 0 ), ISNULL( B.REINFORCE_INTELLIGENCE, 0 ),\
		ISNULL( B.REINFORCE_WISDOM, 0 ), ISNULL( B.REINFORCE_LIFE, 0 ), ISNULL( B.REINFORCE_MANA, 0 ), ISNULL( B.REINFORCE_LIFE_RECOVERY, 0 ),\
		ISNULL( B.REINFORCE_MANA_RECOVERY, 0 ), ISNULL( B.REINFORCE_PHYSIC_ATTACK, 0 ), ISNULL( B.REINFORCE_PHYSIC_DEFENCE, 0 ),\
		ISNULL( B.REINFORCE_MAGIC_ATTACK, 0 ), ISNULL( B.REINFORCE_MAGIC_DEFENCE, 0 ), ISNULL( B.REINFORCE_MOVE_SPEED, 0 ), ISNULL( B.REINFORCE_EVADE, 0 ),\
		ISNULL( B.REINFORCE_ACCURACY, 0 ),ISNULL( B.REINFORCE_CRITICAL_RATE, 0 ), ISNULL( B.REINFORCE_CRITICAL_DAMAGE, 0 ), ISNULL( B.REINFORCE_MADEBY, \'\' ),\
		ISNULL( B.MIX_STRENGTH, 0 ), ISNULL( B.MIX_INTELLIGENCE, 0 ), ISNULL( B.MIX_DEXTERITY, 0 ), ISNULL( B.MIX_WISDOM, 0 ), ISNULL( B.MIX_VITALITY, 0 ),\
		ISNULL( B.MIX_MADEBY, \'\' ),ISNULL( B.ENCHANT_INDEX, 0 ), ISNULL( B.ENCHANT_LEVEL, 0 ), ISNULL( B.ENCHANT_MADEBY, \'\'),\
		ISNULL( C.KEY1, 0 ), CAST( ROUND( ISNULL( C.VALUE1, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY2, 0 ), CAST( ROUND( ISNULL( C.VALUE2, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY3, 0 ), CAST( ROUND( ISNULL( C.VALUE3, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY4, 0 ), CAST( ROUND( ISNULL( C.VALUE4, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY5, 0 ), CAST( ROUND( ISNULL( C.VALUE5, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		%d, %d,  A.PET_IDX\
		FROM TB_ITEM A LEFT OUTER JOIN TB_ITEM_DROP_OPTION C ON A.ITEM_DBIDX = C.ITEM_DBIDX LEFT OUTER JOIN TB_ITEM_OPTION2 B ON A.ITEM_DBIDX = B.ITEM_DBIDX\
		WHERE (A.ITEM_SHOPIDX = %d) AND (A.ITEM_DBIDX > %d) ORDER BY A.ITEM_DBIDX",
		MAX_ROW_NUM,
		Item::AreaCashStorage,
		userIndex,
		userIndex,
		beginItemDbIndex );
}


// 080716 LUJ, 펫이 가진 아이템을 쿼리한다
// 080731 LUJ, 사용자 번호 대신 펫 번호를 인자로 쓴다
void GetItemListOnPet( DWORD connectionIndex, DWORD petIndex, DWORD beginItemDbIndex )
{
	// 080716 LUJ, 펫 인덱스를 쿼리하도록 추가
	// 080730 LUJ, 서브 쿼리로 조회하면 너무 느리다! 조인하는 테이블 간의 키가 모두 복수이기 때문. 따라서 인덱스를 직접 받아 쿼리하도록 했다
	g_DB.FreeQuery(
		RGetItemList,
		connectionIndex,
		"SELECT  TOP %d A.ITEM_DBIDX, A.ITEM_IDX, A.ITEM_POSITION, A.ITEM_DURABILITY, A.ITEM_SEAL, DATEDIFF(second, GetDate(), A.ITEM_ENDTIME), A.ITEM_REMAINTIME,\
		ISNULL( B.REINFORCE_STRENGTH, 0 ), ISNULL( B.REINFORCE_DEXTERITY, 0 ), ISNULL( B.REINFORCE_VITALITY, 0 ), ISNULL( B.REINFORCE_INTELLIGENCE, 0 ),\
		ISNULL( B.REINFORCE_WISDOM, 0 ), ISNULL( B.REINFORCE_LIFE, 0 ), ISNULL( B.REINFORCE_MANA, 0 ), ISNULL( B.REINFORCE_LIFE_RECOVERY, 0 ),\
		ISNULL( B.REINFORCE_MANA_RECOVERY, 0 ), ISNULL( B.REINFORCE_PHYSIC_ATTACK, 0 ), ISNULL( B.REINFORCE_PHYSIC_DEFENCE, 0 ),\
		ISNULL( B.REINFORCE_MAGIC_ATTACK, 0 ), ISNULL( B.REINFORCE_MAGIC_DEFENCE, 0 ), ISNULL( B.REINFORCE_MOVE_SPEED, 0 ), ISNULL( B.REINFORCE_EVADE, 0 ),\
		ISNULL( B.REINFORCE_ACCURACY, 0 ),ISNULL( B.REINFORCE_CRITICAL_RATE, 0 ), ISNULL( B.REINFORCE_CRITICAL_DAMAGE, 0 ), ISNULL( B.REINFORCE_MADEBY, \'\' ),\
		ISNULL( B.MIX_STRENGTH, 0 ), ISNULL( B.MIX_INTELLIGENCE, 0 ), ISNULL( B.MIX_DEXTERITY, 0 ), ISNULL( B.MIX_WISDOM, 0 ), ISNULL( B.MIX_VITALITY, 0 ),\
		ISNULL( B.MIX_MADEBY, \'\' ),ISNULL( B.ENCHANT_INDEX, 0 ), ISNULL( B.ENCHANT_LEVEL, 0 ), ISNULL( B.ENCHANT_MADEBY, \'\'),\
		ISNULL( C.KEY1, 0 ), CAST( ROUND( ISNULL( C.VALUE1, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY2, 0 ), CAST( ROUND( ISNULL( C.VALUE2, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY3, 0 ), CAST( ROUND( ISNULL( C.VALUE3, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY4, 0 ), CAST( ROUND( ISNULL( C.VALUE4, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY5, 0 ), CAST( ROUND( ISNULL( C.VALUE5, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		%d, %d, A.PET_IDX\
		FROM TB_ITEM A LEFT OUTER JOIN TB_ITEM_DROP_OPTION C ON A.ITEM_DBIDX = C.ITEM_DBIDX LEFT OUTER JOIN TB_ITEM_OPTION2 B ON A.ITEM_DBIDX = B.ITEM_DBIDX\
		WHERE A.PET_IDX = %d AND A.ITEM_DBIDX > %d ORDER BY A.ITEM_DBIDX",
		MAX_ROW_NUM,
		Item::AreaPet,
		petIndex,
		petIndex,
		beginItemDbIndex );
}


void GetItemListOnStorage( DWORD connectionIndex, DWORD userIndex, DWORD beginItemDbIndex )
{
	// 080716 LUJ, 펫 인덱스를 쿼리하도록 추가
	g_DB.FreeQuery(
		RGetItemList,
		connectionIndex,
		"SELECT  TOP %d A.ITEM_DBIDX, A.ITEM_IDX, A.ITEM_POSITION, A.ITEM_DURABILITY, A.ITEM_SEAL, DATEDIFF(second, GetDate(), A.ITEM_ENDTIME), A.ITEM_REMAINTIME,\
		ISNULL( B.REINFORCE_STRENGTH, 0 ), ISNULL( B.REINFORCE_DEXTERITY, 0 ), ISNULL( B.REINFORCE_VITALITY, 0 ), ISNULL( B.REINFORCE_INTELLIGENCE, 0 ),\
		ISNULL( B.REINFORCE_WISDOM, 0 ), ISNULL( B.REINFORCE_LIFE, 0 ), ISNULL( B.REINFORCE_MANA, 0 ), ISNULL( B.REINFORCE_LIFE_RECOVERY, 0 ),\
		ISNULL( B.REINFORCE_MANA_RECOVERY, 0 ), ISNULL( B.REINFORCE_PHYSIC_ATTACK, 0 ), ISNULL( B.REINFORCE_PHYSIC_DEFENCE, 0 ),\
		ISNULL( B.REINFORCE_MAGIC_ATTACK, 0 ), ISNULL( B.REINFORCE_MAGIC_DEFENCE, 0 ), ISNULL( B.REINFORCE_MOVE_SPEED, 0 ), ISNULL( B.REINFORCE_EVADE, 0 ),\
		ISNULL( B.REINFORCE_ACCURACY, 0 ),ISNULL( B.REINFORCE_CRITICAL_RATE, 0 ), ISNULL( B.REINFORCE_CRITICAL_DAMAGE, 0 ), ISNULL( B.REINFORCE_MADEBY, \'\' ),\
		ISNULL( B.MIX_STRENGTH, 0 ), ISNULL( B.MIX_INTELLIGENCE, 0 ), ISNULL( B.MIX_DEXTERITY, 0 ), ISNULL( B.MIX_WISDOM, 0 ), ISNULL( B.MIX_VITALITY, 0 ),\
		ISNULL( B.MIX_MADEBY, \'\' ),ISNULL( B.ENCHANT_INDEX, 0 ), ISNULL( B.ENCHANT_LEVEL, 0 ), ISNULL( B.ENCHANT_MADEBY, \'\'),\
		ISNULL( C.KEY1, 0 ), CAST( ROUND( ISNULL( C.VALUE1, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY2, 0 ), CAST( ROUND( ISNULL( C.VALUE2, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY3, 0 ), CAST( ROUND( ISNULL( C.VALUE3, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY4, 0 ), CAST( ROUND( ISNULL( C.VALUE4, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		ISNULL( C.KEY5, 0 ), CAST( ROUND( ISNULL( C.VALUE5, 0 ), 3, 3 ) AS VARCHAR( 16 ) ),\
		%d, %d, A.PET_IDX\
		FROM TB_ITEM A LEFT OUTER JOIN TB_ITEM_DROP_OPTION C ON A.ITEM_DBIDX = C.ITEM_DBIDX LEFT OUTER JOIN TB_ITEM_OPTION2 B ON A.ITEM_DBIDX = B.ITEM_DBIDX\
		WHERE (A.ITEM_PYOGUKIDX = %d) AND (A.ITEM_DBIDX > %d) ORDER BY A.ITEM_DBIDX",
		MAX_ROW_NUM,
		Item::AreaPrivateStorage,
		userIndex,
		userIndex,
		beginItemDbIndex );
}


void RGetItemList( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_ITEM itemMessage;
	ZeroMemory( &itemMessage, sizeof( itemMessage ) );
	itemMessage.Category	= MP_RM_ITEM;
	itemMessage.Protocol	= MP_RM_ITEM_GET_ACK;
	itemMessage.mSize		= dbMessage->dwResult;

	MSG_RM_ITEM_OPTION optionMessage;
	ZeroMemory( &optionMessage, sizeof( optionMessage ) );
	optionMessage.Category		= MP_RM_ITEM;
	optionMessage.Protocol		= MP_RM_ITEM_GET_OPTION_ACK;

	for( DWORD  i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&	record	= query[ i ];
		Item& item = itemMessage.mData[ i ];
		item.dwDBIdx = atoi( ( char* )record.Data[ 0 ] );
		item.wIconIdx = atoi( ( char* )record.Data[ 1 ] );
		item.Position = atoi( ( char* )record.Data[ 2 ] );
		item.Durability = atoi( ( char* )record.Data[ 3 ] );
		item.nSealed = ITEM_SEAL_TYPE(atoi( ( char* )record.Data[ 4 ] ));
		item.nRemainSecond = max( 0,atoi( ( char* )record.Data[ 5 ] ) );
		item.LastCheckTime = atoi( ( char* )record.Data[ 6 ] );
		item.mArea = Item::Area( atoi( ( char* )record.Data[ 45 ] ) );
		item.mPetIndex = atoi( ( char* )record.Data[ 47 ] );

		{
			ITEM_OPTION option = { 0 };

			{
				ITEM_OPTION::Reinforce& data = option.mReinforce;

				data.mStrength			= atoi( ( char* )record.Data[ 7 ] );
				data.mDexterity			= atoi( ( char* )record.Data[ 8 ] );
				data.mVitality			= atoi( ( char* )record.Data[ 9 ] );
				data.mIntelligence		= atoi( ( char* )record.Data[ 10 ] );
				data.mWisdom			= atoi( ( char* )record.Data[ 11 ] );
				data.mLife				= atoi( ( char* )record.Data[ 12 ] );
				data.mMana				= atoi( ( char* )record.Data[ 13 ] );
				data.mLifeRecovery		= atoi( ( char* )record.Data[ 14 ] );
				data.mManaRecovery		= atoi( ( char* )record.Data[ 15 ] );
				data.mPhysicAttack		= atoi( ( char* )record.Data[ 16 ] );
				data.mPhysicDefence		= atoi( ( char* )record.Data[ 17 ] );
				data.mMagicAttack		= atoi( ( char* )record.Data[ 18 ] );
				data.mMagicDefence		= atoi( ( char* )record.Data[ 19 ] );
				data.mMoveSpeed			= atoi( ( char* )record.Data[ 20 ] );
				data.mEvade				= atoi( ( char* )record.Data[ 21 ] );
				data.mAccuracy			= atoi( ( char* )record.Data[ 22 ] );
				data.mCriticalRate		= atoi( ( char* )record.Data[ 23 ] );
				data.mCriticalDamage	= atoi( ( char* )record.Data[ 24 ] );
				StringCopySafe( data.mMadeBy, ( char* )record.Data[ 25 ], sizeof( data.mMadeBy ) );
			}

			{
				ITEM_OPTION::Mix& data = option.mMix;

				data.mStrength		= atoi( ( char* )record.Data[ 26 ] );
				data.mIntelligence	= atoi( ( char* )record.Data[ 27 ] );
				data.mDexterity		= atoi( ( char* )record.Data[ 28 ] );
				data.mWisdom		= atoi( ( char* )record.Data[ 29 ] );
				data.mVitality		= atoi( ( char* )record.Data[ 30 ] );
				StringCopySafe( data.mMadeBy, ( char* )record.Data[ 31 ], sizeof( data.mMadeBy ) );
			}
			
			{
				ITEM_OPTION::Enchant& data = option.mEnchant;

				data.mIndex	= atoi( ( char* )record.Data[ 32 ] );
				data.mLevel	= atoi( ( char* )record.Data[ 33 ] );
				StringCopySafe( data.mMadeBy, ( char* )record.Data[ 34 ], sizeof( data.mMadeBy ) );
			}

			{
				ITEM_OPTION::Drop& data = option.mDrop;

				data.mValue[ 0 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 35 ] ) );
				data.mValue[ 0 ].mValue	= float( atof( ( char* )record.Data[ 36 ] ) );

				data.mValue[ 1 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 37 ] ) );
				data.mValue[ 1 ].mValue	= float( atof( ( char* )record.Data[ 38 ] ) );

				data.mValue[ 2 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 39 ] ) );
				data.mValue[ 2 ].mValue	= float( atof( ( char* )record.Data[ 40 ] ) );

				data.mValue[ 3 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 41 ] ) );
				data.mValue[ 3 ].mValue	= float( atof( ( char* )record.Data[ 42 ] ) );

				data.mValue[ 4 ].mKey	= ITEM_OPTION::Drop::Key( atoi( ( char* )record.Data[ 43 ] ) );
				data.mValue[ 4 ].mValue	= float( atof( ( char* )record.Data[ 44 ] ) );
			}

			const ITEM_OPTION emptyOption = { 0 };

			if( ! memcmp( &emptyOption, &option, sizeof( option ) ) )
			{
				continue;
			}

			option.mItemDbIndex	= item.dwDBIdx;

			optionMessage.mData[ optionMessage.mSize++ ] = option;
		}
	}

	NETWORK->Send( connectionIndex, optionMessage,	optionMessage.GetSize() );
	NETWORK->Send( connectionIndex, itemMessage,	itemMessage.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&		record			= query[ MAX_ROW_NUM - 1 ];
		const DWORD			lastItemDbIndex	= atoi( ( char* )record.Data[ 0 ] );;
		const Item::Area	area			= Item::Area( atoi( ( char* )record.Data[ 45 ] ) );
		
		switch( area )
		{
		case Item::AreaInventory:
			{
				const DWORD	playerIndex	= atoi( ( char* )record.Data[ 46 ] );;

				GetItemListOnInventory(
					connectionIndex,
					playerIndex,
					lastItemDbIndex );
				break;
			}
		case Item::AreaCashStorage:
			{
				const DWORD	userIndex = atoi( ( char* )record.Data[ 46 ] );;

				GetItemListOnShop(
					connectionIndex,
					userIndex,
					lastItemDbIndex );
				break;
			}
		case Item::AreaPrivateStorage:
			{
				const DWORD	userIndex = atoi( ( char* )record.Data[ 46 ] );;

				GetItemListOnStorage(
					connectionIndex,
					userIndex,
					lastItemDbIndex );
				break;
			}
			// 080716 LUJ, 펫 정보 쿼리
		case Item::AreaPet:
			{
				// 080730 LUJ, 속도 향상을 위해 인자를 펫 번호로 변경
				const DWORD	petIndex = atoi( ( char* )record.Data[ 46 ] );

				GetItemListOnPet(
					connectionIndex,
					petIndex,
					lastItemDbIndex );
				break;
			}
		}
	}
}

void FindItemOwner( DWORD connectionIndex, DWORD itemIndex, DWORD playerIndex )
{
	g_DB.FreeQuery(
		RFindItemOwner,
		connectionIndex,
		"EXEC dbo.TP_ITEM_OWNER_SELECT %d, %d",
		itemIndex,
		playerIndex);
}


void RFindItemOwner( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_ITEM_OWNER message;
	message.Category	= MP_RM_ITEM_FIND;
	message.Protocol	= MP_RM_ITEM_FIND_OWNER_ACK;
	message.mSize		= dbMessage->dwResult;
	message.mItemIndex	= ( dbMessage->dwResult ? atoi( ( char* )query[ 0 ].Data[ 0 ] ) : 0 );

	{
		const DWORD maxSize = sizeof( message.mPlayer ) / sizeof( message.mPlayer[ 0 ] );

		if( maxSize < message.mSize )
		{
			ASSERT( 0 );
			return;
		}
	}
		
	MSG_RM_ITEM_OWNER::Player* player		= message.mPlayer;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i, ++player )
	{
		const QUERYST& record = query[ i ];
		ZeroMemory( player, sizeof( *player ) );

		player->mSlotSize = atoi( ( char* )record.Data[ 1 ] );
		player->mIndex = atoi( ( char* )record.Data[ 2 ] );
		strncpy( player->mName, ( char* )record.Data[ 3 ], sizeof( player->mName ) );
		player->mUserIndex = atoi( ( char* )record.Data[ 4 ] );
		player->mQuantity = atoi(LPCTSTR(record.Data[5]));
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void RAddItem( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM;
		message.Protocol	= MP_RM_ITEM_ADD_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_ITEM_ADD message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category		= MP_RM_ITEM;
		message.Protocol		= MP_RM_ITEM_ADD_ACK;
		message.mPlayerIndex	= atoi( ( char* )record.Data[ 0 ] );
	}

	Item& item = message.mItem;
	{
		item.dwDBIdx		= atoi( ( char* )record.Data[ 1 ] );
		item.wIconIdx		= atoi( ( char* )record.Data[ 2 ] );
		item.Position		= atoi( ( char* )record.Data[ 3 ] );
		item.Durability		= atoi( ( char* )record.Data[ 4 ] );
		item.nSealed		= ITEM_SEAL_TYPE(atoi( ( char* )record.Data[ 5 ] ));
		item.mArea			= Item::Area( atoi( ( char* )record.Data[ 6 ] ) );

	}

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	g_DB.LogMiddleQuery(
		0,
		0,
		"EXEC dbo.UP_ITEMMONEYLOG %d, \'%s\', %d, \'%s\', %d, %u, %u, %u, %d, %d, %d, %d, %d, %d", 
		0,
		"",
		message.mPlayerIndex,
		std::string( "*" + user.mId ).c_str(), 
		eLog_ItemAdd,
		0,
		0,
		0,
		item.wIconIdx,
		item.dwDBIdx,
		0,
		item.Position,
		item.Durability,
		0 );

	g_DB.LogMiddleQuery(
		0,
		0,
		"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2 ) VALUES ( %d, \'%s\', \'%s\', \'%d\', \'%s\', \'%d\' )",
		RecoveryLogAddItem,
		user.mId.c_str(),
		"playerIndex",
		message.mPlayerIndex,
		"itemDbIndex",
		item.dwDBIdx );
}


void RSetItemEndTime( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		return;
	}

	const MIDDLEQUERYST&	record		= query[ 0 ];
	const int				itemDbIndex	= atoi( ( char* )record.Data[ 0 ] );

	if( itemDbIndex )
	{
		MSG_INT2 message;

		message.Category	= MP_RM_ITEM;
		message.Protocol	= MP_RM_ITEM_UPDATE_END_TIME_ACK;
		message.nData1		= itemDbIndex;
		message.nData2		= atoi( ( char* )record.Data[ 1 ] );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
	else
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM;
		message.Protocol	= MP_RM_ITEM_UPDATE_END_TIME_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
}


void GetSkillList( DWORD connectionIndex, DWORD playerIndex )
{
	g_DB.FreeQuery(
		RGetSkillList,
		connectionIndex,
		"EXEC dbo.MP_CHARACTER_SKILLTREEINFO %d",
		playerIndex);
}


void RGetSkillList( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_SKILL message;
	message.Category	= MP_RM_SKILL;
	message.Protocol	= MP_RM_SKILL_GET_ALL_ACK;
	message.mSize		= dbMessage->dwResult;
	
	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		SKILL_BASE&		data	= message.mData[ i ];
		const QUERYST&	record	= query[ i ];

		data.dwDBIdx	= atoi( ( char* )record.Data[ 1 ] );
		data.wSkillIdx	= atoi( ( char* )record.Data[ 2 ] );
		data.Level		= atoi( ( char* )record.Data[ 3 ] );
	}

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}


void SetSkillLevel( DWORD connectionIndex, DWORD playerIndex, DWORD skillDbIndex, DWORD level )
{
	g_DB.FreeMiddleQuery( RSetSkillLevel, connectionIndex, "EXEC dbo.TP_SKILL_LEVEL_UPDATE %d, %d, %d",
		playerIndex,
		skillDbIndex,
		level );
}


void RSetSkillLevel( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_SKILL;
		message.Protocol	= MP_RM_SKILL_SET_LEVEL_NACK;
		
		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD3 message;

	message.Category	= MP_RM_SKILL;
	message.Protocol	= MP_RM_SKILL_SET_LEVEL_ACK;
	message.dwObjectID	= atoi( ( char* )record.Data[ 0 ] );

	// 스킬 DB 인덱스
	message.dwData1		= atoi( ( char* )record.Data[ 1 ] );

	// 스킬 인덱스
	message.dwData2		= atoi( ( char* )record.Data[ 2 ] );

	// 레벨
	message.dwData3		= atoi( ( char* )record.Data[ 3 ] );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	// 로그를 남기자
	{
		const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );
		
		g_DB.LogMiddleQuery( 0, 0, "EXEC dbo.TP_SKILL_LOG_INSERT %d, %d, %d, %d, %d, %d, \'%s\'",
			message.dwObjectID,
            0,
			message.dwData1,
			message.dwData3,
			message.dwData2,			
			eLog_SkillLevel_RM,
			user.mId.c_str() );
	}
}


void AddSkillData( DWORD connectionIndex, DWORD playerIndex, DWORD skillIndex )
{
	g_DB.FreeMiddleQuery( RAddSkillData, connectionIndex, "EXEC dbo.MP_SKILLTREE_INSERT %d, %d, %d",
		playerIndex,
		skillIndex,
		1 );
}


void RAddSkillData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_SKILL;
		message.Protocol	= MP_RM_SKILL_ADD_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_SKILL message;
	message.Category	= MP_RM_SKILL;
	message.Protocol	= MP_RM_SKILL_ADD_ACK;
	message.dwObjectID	= atoi( ( char* )record.Data[ 0 ] );
	message.mSize		= 1;

	SKILL_BASE& data = message.mData[ 0 ];

	{
		data.dwDBIdx	= atoi( ( char* )record.Data[ 1 ] );
		data.wSkillIdx	= atoi( ( char* )record.Data[ 2 ] );
		data.Level		= atoi( ( char* )record.Data[ 3 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	// 로그를 남긴다
	{
		const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );
		
		g_DB.LogMiddleQuery( 0, 0, "EXEC dbo.TP_SKILL_LOG_INSERT %d, %d, %d, %d, %d, %d, %s",
			message.dwObjectID,
			0,
			data.dwDBIdx,
			data.Level,
			data.wSkillIdx,
			eLog_SkillLearn_RM,
			user.mId.c_str() );
	}
}


void RemoveSkillData( DWORD connectionIndex, DWORD skillDbIndex )
{
	g_DB.FreeMiddleQuery( RRemoveSkillData, connectionIndex, "EXEC dbo.TP_SKILL_DELETE %d",
		skillDbIndex );
}


void RRemoveSkillData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_SKILL;
		message.Protocol	= MP_RM_SKILL_REMOVE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD2 message;
	message.Category	= MP_RM_SKILL;
	message.Protocol	= MP_RM_SKILL_REMOVE_ACK;
	message.dwObjectID	= atoi( ( char* )record.Data[ 0 ] );

	// 스킬 DB 인덱스
	message.dwData1		= atoi( ( char* )record.Data[ 1 ] );

	// 스킬 인덱스
	message.dwData2		= atoi( ( char* )record.Data[ 2 ] );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	// 로그를 남긴다
	{
		const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

		g_DB.LogMiddleQuery( 0, 0, "EXEC dbo.TP_SKILL_LOG_INSERT %d, %d, %d, %d, %d, %d, %s",
			message.dwObjectID,
			0,
			message.dwData1,
			0,
			message.dwData2,
			eLog_SkillRemove_RM,
			user.mId.c_str() );
	}
}


void RGetFamilyList( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_FAMILY_LIST message;
	{
		const DWORD maxSize	= sizeof( message.mFamily ) / sizeof( message.mFamily[ 0 ] );
		ASSERT( maxSize >= dbMessage->dwResult );

		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_FAMILY;
		message.Protocol	= MP_RM_FAMILY_GET_LIST_ACK;
		message.mSize		= min( maxSize, dbMessage->dwResult );
	}

	for( DWORD i = 0; i < message.mSize; ++i )
	{
		const QUERYST&				record	= query[ i ];
		MSG_RM_FAMILY_LIST::Family&	data	= message.mFamily[ i ];

		data.mIndex			= atoi( ( char* )record.Data[ 0 ] );
		strncpy( data.mName, ( char* )record.Data[ 1 ], sizeof( data.mName ) );
		data.mSize			= atoi( ( char* )record.Data[ 2 ] );
		data.mMasterIndex	= atoi( ( char* )record.Data[ 3 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void RGetFamilyData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_FAMILY;
		message.Protocol	= MP_RM_FAMILY_GET_DATA_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_FAMILY_DATA message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category		= MP_RM_FAMILY;
	message.Protocol		= MP_RM_FAMILY_GET_DATA_ACK;
	message.mIndex			= atoi( ( char* )record.Data[ 0 ] );
	strncpy( message.mName, ( char* )record.Data[ 1 ], sizeof( message.mName ) );
	message.mHonorPoint		= atoi( ( char* )record.Data[ 2 ] );
	message.mIsEnableNick	= atoi( ( char* )record.Data[ 3 ] );
	strncpy( message.mBuildDate, ( char* )record.Data[ 5 ], sizeof( message.mBuildDate ) );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const DWORD playerIndex = atoi( ( char* )record.Data[ 4 ] );

	g_DB.FreeQuery(
		RGetFamilyMember,
		connectionIndex,
		"EXEC dbo.TP_FAMILY_MEMBER_SELECT %d",
		playerIndex );
}


void RGetFamilyMember( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_FAMILY_MEMBER message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_FAMILY;
	message.Protocol	= MP_RM_FAMILY_GET_MEMBER_ACK;
	message.mSize		= dbMessage->dwResult;
	
	MSG_RM_FAMILY_MEMBER::Player* player = message.mPlayer;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST					record	= query[ i ];
		MSG_RM_FAMILY_MEMBER::Player&	data	= message.mPlayer[ i ];

		data.mIndex		= atoi( ( char* )record.Data[ 0 ] );
		strncpy( data.mNick, ( char* )record.Data[ 1 ], sizeof( data.mNick ) );
		strncpy( data.mName, ( char* )record.Data[ 2 ], sizeof( data.mName ) );
		data.mLevel		= atoi( ( char* )record.Data[ 3 ] );
		data.mIsMaster	= atoi( ( char* )record.Data[ 4 ] );
	}	

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}


void RGetFamilyFarm( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_FAMILY;
		message.Protocol	= MP_RM_FAMILY_GET_FARM_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}
	
	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_FAMILY_FARM message;
	message.Category		= MP_RM_FAMILY;
	message.Protocol		= MP_RM_FAMILY_GET_FARM_ACK;
	message.mZone			= atoi( ( char* )record.Data[ 0 ] );
	message.mFarmIndex		= atoi( ( char* )record.Data[ 1 ] );
	message.mState			= atoi( ( char* )record.Data[ 2 ] );
	message.mOwnerIndex		= atoi( ( char* )record.Data[ 3 ] );
	message.mGardenGrade	= atoi( ( char* )record.Data[ 4 ] );
	message.mHouseGrade		= atoi( ( char* )record.Data[ 5 ] );
	message.mWarehouseGrade	= atoi( ( char* )record.Data[ 6 ] );
	message.mCageGrade		= atoi( ( char* )record.Data[ 7 ] );
	message.mFenceGrade		= atoi( ( char* )record.Data[ 8 ] );
	StringCopySafe( message.mBuildDate, ( char* )record.Data[ 9 ], sizeof( message.mBuildDate ) );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}


void RGetFamilyCrop( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_FAMILY_CROP message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_FAMILY;
	message.Protocol	= MP_RM_FAMILY_GET_CROP_ACK;
	message.mSize		= min( sizeof( message.mCrop ) / sizeof( *message.mCrop ), dbMessage->dwResult );

	for( DWORD i = 0; i < message.mSize; ++i )
	{
		const MIDDLEQUERYST&	record	= query[ i ];
		MSG_RM_FAMILY_CROP::Crop&	data	= message.mCrop[ i ];

		data.mIndex			= atoi( ( char* )record.Data[ 0 ] );
		data.mOwnerIndex	= atoi( ( char* )record.Data[ 1 ] );
		StringCopySafe( data.mOwnerName, ( char* )record.Data[ 2 ], sizeof( data.mOwnerName ) );
		data.mKind			= atoi( ( char* )record.Data[ 3 ] );
		data.mStep			= atoi( ( char* )record.Data[ 4 ] );
		data.mLife			= atoi( ( char* )record.Data[ 5 ] );
		data.mNextStepTime	= atoi( ( char* )record.Data[ 6 ] );
		data.mSeedGrade		= atoi( ( char* )record.Data[ 7 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


// 080731 LUJ, 가축 목록을 읽어온다
void RGetFamilyLivestock( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_LIVESTOCK message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_FAMILY;
	message.Protocol	= MP_RM_FAMILY_GET_LIVESTOCK_ACK;
	message.mSize		= min( sizeof( message.mAnimal ) / sizeof( *message.mAnimal ), dbMessage->dwResult );

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const MIDDLEQUERYST&		record	= query[ i ];
		MSG_RM_LIVESTOCK::Animal&	data	= message.mAnimal[ i ];

		data.mIndex			= atoi( ( char* )record.Data[ 0 ] );
		data.mOwnerIndex	= atoi( ( char* )record.Data[ 1 ] );
		StringCopySafe( data.mOwnerName, ( char* )record.Data[ 2 ], sizeof( data.mOwnerName ) );
		data.mKind			= atoi( ( char* )record.Data[ 3 ] );
		data.mStep			= atoi( ( char* )record.Data[ 4 ] );
		data.mLife			= atoi( ( char* )record.Data[ 5 ] );
		data.mNextStepTime	= atoi( ( char* )record.Data[ 6 ] );
		data.mContentment	= atoi( ( char* )record.Data[ 7 ] );
		data.mInterest		= atoi( ( char* )record.Data[ 8 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void GetQuestMainData( DWORD connectionIndex, DWORD playerIndex, DWORD questIndex )
{
	g_DB.FreeQuery(
		RGetQuestMainData,
		connectionIndex,
		"EXEC dbo.TP_QUEST_MAIN_SELECT %d, %d",
		playerIndex,
		questIndex);
}


void RGetQuestMainData( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	//if( ! dbMessage->dwResult )
	//{
	//	// 퀘스트가 없을 수 있음
	//	return;
	//}

	SEND_MAINQUEST_DATA message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_GET_MAIN_DATA_ACK;
		message.wCount		= WORD( dbMessage->dwResult );
	}	

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		QMBASE&			data	= message.QuestList[ i ];
		const QUERYST&	record	= query[ i ];

		data.QuestIdx = atoi( ( char*)record.Data[ 0 ] );
		data.state.value = ( QSTATETYPE )atoi( ( char*)record.Data[ 1 ] );
		data.EndParam = atoi( ( char*)record.Data[ 2 ] );
		data.registTime = _ttoi64( ( char*)record.Data[ 3 ] );
		data.CheckType = atoi( ( char*)record.Data[ 4 ] );
		data.CheckTime = atoi( ( char*)record.Data[ 5 ] );
	}

	// 최대 크기를 초과했으면 계속 쿼리한다
	{
		const DWORD maxSize = sizeof( message.QuestList ) / sizeof( *message.QuestList );

		ASSERT( maxSize >= message.wCount );

		if( maxSize == message.wCount )
		{
			const QUERYST& record = query[ maxSize - 1 ];

			const DWORD playerIndex		= atoi( ( char* )record.Data[ 6 ] );
			const DWORD lastQuestIndex	= atoi( ( char* )record.Data[ 0 ] );

			GetQuestMainData( connectionIndex, playerIndex, lastQuestIndex );
		}
	}
	
	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void GetQuestSubData( DWORD connectionIndex, DWORD playerIndex, DWORD questIndex )
{
	g_DB.FreeQuery(
		RGetQuestSubData,
		connectionIndex,
		"EXEC dbo.TP_QUEST_SUB_SELECT %d, %d",
		playerIndex,
		questIndex);
}


void RGetQuestSubData( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	SEND_SUBQUEST_DATA message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_GET_SUB_DATA_ACK;
		message.wCount		= WORD( dbMessage->dwResult );
	}	

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		QSUBASE&		data	= message.QuestList[ i ];
		const QUERYST&	record	= query[ i ];

		data.QuestIdx		= atoi( ( char* )record.Data[ 0 ] );
		data.SubQuestIdx	= atoi( ( char* )record.Data[ 1 ] );
		data.state			= atoi( ( char* )record.Data[ 2 ] );
		data.time			= atoi( ( char* )record.Data[ 3 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	const DWORD playerIndex = atoi( ( char* )query[ 0 ].Data[ 4 ] );

	// 최대 크기를 초과했으면 계속 쿼리한다
	{
		const DWORD maxSize = sizeof( message.QuestList ) / sizeof( message.QuestList[ 0 ] );

		ASSERT( maxSize >= message.wCount );

		if( maxSize == message.wCount )
		{
			const DWORD lastQuestIndex	= message.QuestList[ maxSize - 1 ].QuestIdx;
			
			GetQuestSubData( connectionIndex, playerIndex, lastQuestIndex );			
			return;
		}
	}
	
	GetQuestMainData( connectionIndex, playerIndex, 0 );
	GetQuestItemList( connectionIndex, playerIndex, 0 );
}


void GetQuestItemList( DWORD connectionIndex, DWORD playerIndex, DWORD questIndex )
{
	g_DB.FreeQuery(
		RGetQuestItemList,
		connectionIndex,
		"EXEC dbo.TP_QUEST_ITEM_SELECT %d, %d",
		playerIndex,
		questIndex);
}


void RGetQuestItemList( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	SEND_QUESTITEM message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_GET_ITEM_DATA_ACK;
		message.wCount		= WORD( dbMessage->dwResult );
	}	

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&	record	= query[ i ];
		QITEMBASE&		data	= message.ItemList[ i ];

		data.ItemIdx	= atoi( ( char*)record.Data[ 0 ] );
		data.Count		= atoi( ( char*)record.Data[ 1 ] );
		data.QuestIdx	= atoi( ( char*)record.Data[ 2 ] );
	}

	// 최대 크기를 초과했으면 계속 쿼리한다
	{
		const DWORD maxSize = sizeof( message.ItemList ) / sizeof( message.ItemList[ 0 ] );

		ASSERT( maxSize >= message.wCount );

		if( maxSize == message.wCount )
		{
			const QUERYST& record = query[ maxSize - 1 ];

			const DWORD lastQuestIndex	= atoi( ( char* )record.Data[ 2 ] );
			const DWORD	playerIndex		= atoi( ( char* )record.Data[ 3 ] );

			GetQuestItemList( connectionIndex, playerIndex, lastQuestIndex );
		}
	}	

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void EndSubQuest( DWORD connectionIndex, DWORD playerIndex, DWORD mainQuestIndex, DWORD subQuestIndex, QSTATETYPE state )
{
	g_DB.FreeMiddleQuery( REndSubQuest, connectionIndex, "EXEC dbo.TP_QUEST_SUB_DELETE %d, %d, %d, %d",
		playerIndex,
		mainQuestIndex,
		subQuestIndex,
		state );
}


void REndSubQuest( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		ASSERT( 0 );

		MSGROOT message;
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_FINISH_SUB_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST&	record	= query[ 0 ];

	MSG_DWORD3 message;
	{
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_FINISH_SUB_ACK;

		// 메인 퀘스트 인덱스
		message.dwData1	= atoi( ( char*)record.Data[ 0 ] );

		// 서브 퀘스트 인덱스
		message.dwData2 = atoi( ( char*)record.Data[ 1 ] );

		// 서브 퀘스트 종료 플래그
		message.dwData3	= atoi( ( char*)record.Data[ 2 ] );
	}
	
	
	NETWORK->Send( connectionIndex, message, sizeof( message ));
}


void RemoveQuestData( DWORD connectionIndex, DWORD playerIndex, DWORD mainQuestIndex )
{
	g_DB.FreeMiddleQuery( RRemoveQuestData, connectionIndex, "EXEC dbo.TP_QUEST_MAIN_DELETE %d, %d",
		playerIndex,
		mainQuestIndex );
}


void RRemoveQuestData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_REMOVE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_QUEST;
	message.Protocol	= MP_RM_QUEST_REMOVE_ACK;
	message.dwObjectID	= atoi( ( char* )record.Data[ 0 ] );

	// 메인 퀘스트 인덱스
	message.dwData		= atoi( ( char* )record.Data[ 1 ] );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}


void RUpdateQuestItem( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult	||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_ITEM_UPDATE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	{
		MSG_DWORD2	message;
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_ITEM_UPDATE_ACK;
		message.dwData1		= atoi( ( char* )query[ 0 ].Data[ 1 ] );	// 퀘스트 아이템 인덱스
		message.dwData2		= atoi( ( char* )query[ 0 ].Data[ 2 ] );	// 수량

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	
}


void DeleteQuestItem( DWORD connectionIndex, DWORD playerIndex, DWORD questMainIndex, DWORD itemIndex )
{
	g_DB.FreeMiddleQuery(
		RDeleteQuestItem,
		connectionIndex,
		"EXEC dbo.TP_QUEST_ITEM_DELETE %d, %d, %d",
		playerIndex,
		questMainIndex,
		itemIndex );
}


void RDeleteQuestItem( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult	||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_ITEM_DELETE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	{
		MSG_DWORD	message;
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_ITEM_DELETE_ACK;
		message.dwData		= atoi( ( char* )query[ 0 ].Data[ 1 ] );	// 아이템 인덱스
		
		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
}


void RGetQuestLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_QUEST_LOG;
		message.Protocol	= MP_RM_QUEST_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_QUEST_LOG;
	message.Protocol	= MP_RM_QUEST_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* playerIndex		= ( char* )record.Data[ 1 ];
	const char*	beginDate		= ( char* )record.Data[ 2 ];
	const char*	endDate			= ( char* )record.Data[ 3 ];

	g_DB.LogQuery(
		RGetQuestLog,
		connectionIndex,
		"EXEC dbo.TP_QUEST_LOG_SELECT %s, \'%s\', \'%s\', 0",
		playerIndex,
		beginDate,
		endDate );
}


void RGetQuestLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	//if(		g_pMSSystem->IsStopQuery( connectionIndex ) ||
	//	!	CUserManager::GetInstance().IsConnect( connectionIndex ) )
	//{
	//	g_pMSSystem->StopQuery(	connectionIndex, MP_RM_QUEST_LOG, MP_RM_QUEST_LOG_STOP_ACK );
	//	return;
	//}

	MSG_RM_QUEST_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_QUEST_LOG;
		message.Protocol	= MP_RM_QUEST_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&			record	= query[ i ];
		MSG_RM_QUEST_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex			=				atoi( ( char* )record.Data[ 0 ] );
		data.mMainQuestIndex	=				atoi( ( char* )record.Data[ 1 ] );
		data.mSubQuestIndex		= 				atoi( ( char* )record.Data[ 2 ] );
		data.mValue1			= 				atoi( ( char* )record.Data[ 3 ] );
		data.mValue2			=				atoi( ( char* )record.Data[ 4 ] );
		data.mType				= eQuestLog(	atoi( ( char* )record.Data[ 5 ] ) );

		StringCopySafe( data.mDate, ( char* )record.Data[ 6 ], sizeof( data.mDate ) );
		StringCopySafe( data.mMemo, ( char* )record.Data[ 7 ], sizeof( data.mMemo ) );
	}
	
	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];

		const char*	beginDate	=		( char* )record.Data[ 6 ];
		const char* endDate		=		( char* )record.Data[ 8 ];
		const char*	playerIndex	=		( char* )record.Data[ 9 ];
		const DWORD	size		= atoi( ( char* )record.Data[ 10 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetQuestLog,
				connectionIndex,
				"EXEC dbo.TP_QUEST_LOG_SELECT %s, \'%s\', \'%s\', %d",
				playerIndex,
				beginDate,
				endDate,
				size );
		}		
	}
}


void GetGuildList( DWORD connectionIndex, const char* keyword, bool isDimissed )
{
	g_DB.FreeQuery(
		RGetGuildList,
		connectionIndex,
		"EXEC dbo.TP_GUILD_LIST_SELECT \'%s\', %d",
		keyword,
		isDimissed );
}


void RGetGuildList( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;
	
	MSG_RM_GUILD_LIST message;
	{
		const DWORD maxSize	= sizeof( message.mGuild ) / sizeof( message.mGuild[ 0 ] );
		ASSERT( maxSize >= dbMessage->dwResult );

		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_LIST_ACK;
		message.mSize		= min( maxSize, dbMessage->dwResult );
	}

	for( DWORD i = 0; i < message.mSize; ++i )
	{
		const QUERYST&				record	= query[ i ];
		MSG_RM_GUILD_LIST::Guild&	data	= message.mGuild[ i ];

		data.mIndex	= atoi( ( char* )record.Data[ 0 ] );
		strncpy( data.mName, ( char* )record.Data[ 1 ], sizeof( data.mName ) );
		data.mSize	= atoi( ( char* )record.Data[ 2 ] );
		data.mLevel	= atoi( ( char* )record.Data[ 3 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void RGetGuildData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_DATA_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message) );
		return;
	}

	//const QUERYST&	record		= query[ 0 ];
	const MIDDLEQUERYST&	record		= query[ 0 ];
	
	MSG_RM_GUILD_DATA message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_DATA_ACK;
		message.mGuildIndex	= atoi( ( char* )record.Data[ 0 ] );
		message.mLevel		= atoi( ( char* )record.Data[ 1 ] );
		message.mScore		= atoi( ( char* )record.Data[ 2 ] );
		message.mMoney		= atoi( ( char* )record.Data[ 3 ] );
		strncpy( message.mGuildName, ( char* )record.Data[ 4 ], sizeof( message.mGuildName ) );
		strncpy( message.mBuiltDate, ( char* )record.Data[ 5 ], sizeof( message.mBuiltDate ) );
		strncpy( message.mAllianceName, ( char* )record.Data[ 6 ], sizeof( message.mAllianceName ) );
		message.mAllianceEntryDate	= atoi( ( char* )record.Data[ 7 ] );
	}	

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	{
		const DWORD guildIndex = message.mGuildIndex;

		GetGuildEnemy( connectionIndex, guildIndex );
		GetGuildFriendly( connectionIndex, guildIndex );
		GetGuildStore( connectionIndex, guildIndex );
		GetGuildMember( connectionIndex, guildIndex );
	}

	// 080425 LUJ, 길드 스킬 로딩
	g_DB.FreeMiddleQuery(
		RGetGuildSkill,
		connectionIndex,
		"SELECT TOP %d SKILL_IDX, SKILL_LEVEL, GUILD_IDX FROM TB_GUILD_SKILL WHERE GUILD_IDX = %d AND SKILL_IDX > 0",
		MAX_MIDDLEROW_NUM,
		message.mGuildIndex );
}


void RGetGuildSkill( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_SKILL_LIST message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_SKILL_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		SKILL_BASE&				data	= message.mData[ i ];
		const MIDDLEQUERYST&	record	= query[ i ];

		data.wSkillIdx	= atoi( ( char* )record.Data[ 0 ] );
		data.Level		= atoi( ( char* )record.Data[ 1 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_MIDDLEROW_NUM == dbMessage->dwResult )
	{
		const MIDDLEQUERYST&	record			= query[ MAX_MIDDLEROW_NUM - 1 ];
		const DWORD				lastSkillIndex	= atoi( ( char* )record.Data[ 0 ] );
		const DWORD				guildIndex		= atoi( ( char* )record.Data[ 2 ] );

		g_DB.FreeMiddleQuery(
			RGetGuildSkill,
			connectionIndex,
			"SELECT TOP %d SKILL_IDX, SKILL_LEVEL, GUILD_IDX FROM TB_GUILD_SKILL WHERE GUILD_IDX = %d AND SKILL_IDX > %d ORDER BY SKILL_IDX",
			MAX_MIDDLEROW_NUM,
			guildIndex,
			lastSkillIndex );
	}
}

void GetGuildStore( DWORD connectionIndex, DWORD guildIndex )
{
	g_DB.FreeQuery(
		RGetGuildStore,
		connectionIndex,
		"EXEC dbo.TP_GUILD_STORE_SELECT %d",
		guildIndex);
}


void RGetGuildStore( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_GUILD_STORE message;
	{
		const DWORD maxSize	= sizeof( message.mItem ) / sizeof( message.mItem[ 0 ] );
		ASSERT( maxSize >= dbMessage->dwResult );

		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_STORE_ACK;
		message.mSize		= min( maxSize, dbMessage->dwResult );
	}

	for( DWORD i = 0; i < message.mSize; ++i )
	{
		const QUERYST&				record	= query[ i ];
		MSG_RM_GUILD_STORE::Item&	data	= message.mItem[ i ];

		data.mDbIndex	= atoi( ( char* )record.Data[ 0 ] );
		data.mIndex		= atoi( ( char* )record.Data[ 1 ] );
		data.mQuantity	= atoi( ( char* )record.Data[ 2 ] );
	}
	
	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void GetGuildMember( DWORD connectionIndex, DWORD guildIndex )
{
	g_DB.FreeQuery(
		RGetGuildMember,
		connectionIndex,
		"EXEC dbo.TP_GUILD_MEMBER_SELECT %d",
		guildIndex );
}


void RGetGuildMember( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_GUILD_MEMBER message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_MEMBER_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&					record	= query[ i ];
		MSG_RM_GUILD_MEMBER::Player&	data	= message.mPlayer[ i ];

		data.mIndex	= atoi( ( char* )record.Data[ 0 ] );		
		StringCopySafe( data.mName, ( char* )record.Data[ 1 ], sizeof( data.mName ) );
		data.mLevel	= atoi( ( char* )record.Data[ 2 ] );
		data.mRank	= atoi( ( char* )record.Data[ 3 ] );
		StringCopySafe( data.mDate, ( char* )record.Data[ 4 ], sizeof( data.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void GetGuildFriendly( DWORD connectionIndex, DWORD guildIndex )
{
	g_DB.FreeQuery(
		RGetGuildFriendly,
		connectionIndex,
		"EXEC dbo.TP_GUILD_FRIENDLY_SELECT %d",
		guildIndex);
}


void RGetGuildFriendly( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_GUILD_LIST message;
	{
		const DWORD maxSize	= sizeof( message.mGuild ) / sizeof( message.mGuild[ 0 ] );
		ASSERT( maxSize >= dbMessage->dwResult );

		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_FRIENDLY_ACK;
		message.mSize		= min( maxSize, dbMessage->dwResult );
	}

	for( DWORD i = 0; i < message.mSize; ++i )
	{
		const QUERYST&				record	= query[ i ];
		MSG_RM_GUILD_LIST::Guild&	data	= message.mGuild[ i ];

		data.mIndex	= atoi( ( char* )record.Data[ 0 ] );
		strncpy( data.mName, ( char* )record.Data[ 1 ], sizeof( data.mName ) );
		data.mSize	= atoi( ( char* )record.Data[ 2 ] );
		data.mLevel	= atoi( ( char* )record.Data[ 3 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void GetGuildEnemy( DWORD connectionIndex, DWORD guildIndex )
{
	g_DB.FreeQuery(
		RGetGuildEnemy,
		connectionIndex,
		"EXEC dbo.TP_GUILD_ENEMY_SELECT %d",
		guildIndex);
}


void RGetGuildEnemy( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_GUILD_LIST message;
	{
		const DWORD maxSize	= sizeof( message.mGuild ) / sizeof( message.mGuild[ 0 ] );
		ASSERT( maxSize >= dbMessage->dwResult );

		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_ENEMY_ACK;
		message.mSize		= min( maxSize, dbMessage->dwResult );
	}

	for( DWORD i = 0; i < message.mSize; ++i )
	{
		const QUERYST&				record	= query[ i ];
		MSG_RM_GUILD_LIST::Guild&	data	= message.mGuild[ i ];

		data.mIndex	= atoi( ( char* )record.Data[ 0 ] );
		strncpy( data.mName, ( char* )record.Data[ 1 ], sizeof( data.mName ) );
		data.mSize	= atoi( ( char* )record.Data[ 2 ] );
		data.mLevel	= atoi( ( char* )record.Data[ 3 ] );
		data.mMoney	= atoi( ( char* )record.Data[ 4 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void RGuildUpdateData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		ASSERT( 0 );

		MSGROOT message;
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_SET_DATA_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
	else
	{
		MSGROOT message;
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_SET_DATA_ACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	
}


void RGuildSetRank( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] )  )
	{
		MSGROOT message;
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_SET_RANK_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD	message;
	{
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_SET_RANK_ACK;
		message.dwObjectID	= atoi( ( char* )record.Data[ 0 ] );

		// 吏곸쐞
		message.dwData		= atoi( ( char* )record.Data[ 1 ] );
	}

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	// 로그 남기자
	{
		const DWORD guildIndex		= atoi( ( char* )record.Data[ 2 ] );
		const DWORD previousRank	= atoi( ( char* )record.Data[ 3 ] );

		g_DB.LogQuery(
			0,
			0,
			0,
			"EXEC dbo.UP_GUILDLOG %d, %d, %d, %d, %d",
			message.dwObjectID,
			guildIndex,
			eGuildLog_GM_ChangeRank,
			previousRank,
			message.dwData );
	}
}


void RGuildKickMember( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( 1 != dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_KICK_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	// 로그 남기자

	const MIDDLEQUERYST&	record = query[ 0 ];
	const DWORD				result = atoi( ( char* )record.Data[ 0 ] );

	switch( result )
	{
	// 성공
	case 0:
		{
			MSGBASE message;
			message.Category	= MP_RM_GUILD;
			message.Protocol	= MP_RM_GUILD_KICK_ACK;
			message.dwObjectID	= atoi( ( char* )record.Data[ 1 ] );

			NETWORK->Send( connectionIndex, message, sizeof( message ) );

			// 로그 남기자
			{
				const DWORD guildIndex		= atoi( ( char* )record.Data[ 2 ] );
				
				g_DB.LogMiddleQuery(
					0,
					0, 
					"EXEC dbo.UP_GUILDLOG %d, %d, %d, %d, %d",
					message.dwObjectID,
					guildIndex,
					eGuildLog_GM_KIck_Member,
					0,
					0 );
			}

			break;
		}

	// 소속 길드 없음
	case 1:
		{
			MSGBASE message;
			message.Category	= MP_RM_GUILD;
			message.Protocol	= MP_RM_GUILD_KICK_NACK_BY_NO_GUILD;
			message.dwObjectID	= atoi( ( char* )record.Data[ 1 ] );

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}

	// 회원 수 없음
	case 2:
		{
			MSGBASE message;
			message.Category	= MP_RM_GUILD;
			message.Protocol	= MP_RM_GUILD_KICK_NACK_BY_ONE_MEMBER;
			message.dwObjectID	= atoi( ( char* )record.Data[ 1 ] );

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			MSGROOT message;
			message.Category	= MP_RM_GUILD;
			message.Protocol	= MP_RM_GUILD_KICK_NACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	}
}


void RGetExperienceLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
			0 == atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_EXP_LOG;
		message.Protocol	= MP_RM_EXP_LOG_GET_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_EXP_LOG;
	message.Protocol	= MP_RM_EXP_LOG_GET_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
	
	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* playerIndex	= ( char* )record.Data[ 1 ];
	const char*	beginDate	= ( char* )record.Data[ 2 ];
	const char*	endDate		= ( char* )record.Data[ 3 ];
	
	g_DB.LogQuery(
		RGetExperienceLog,
		connectionIndex,
		"EXEC dbo.TP_EXP_LOG_SELECT %s, \'%s\', \'%s\', 0",
		playerIndex,
		beginDate,
		endDate );
}


void RGetExperienceLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_EXP_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_EXP_LOG;
		message.Protocol	= MP_RM_EXP_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&			record	= query[ i ];
		MSG_RM_EXP_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex		=				atoi( ( char* )record.Data[ 0 ] );
		// 080716 LUJ, 잘못된 타입을 수정
		data.mType			= eLogExppoint(	atoi( ( char* )record.Data[ 1 ] ) );
		data.mGrade			= 				atoi( ( char* )record.Data[ 2 ] );
		data.mVariation		=				atoi( ( char* )record.Data[ 3 ] );
		data.mKillerKind	= 				atoi( ( char* )record.Data[ 4 ] );
		data.mKillerIndex	= 				atoi( ( char* )record.Data[ 5 ] );
		// 080716 LUJ, 경험치 수치가 int형을 초과하므로 변경
		data.mExperience	= 				atof( ( char* )record.Data[ 6 ] );
		// 080716 LUJ, 경험치 보호 회수를 가져옴
		data.mCount			= 				atoi( ( char* )record.Data[ 11 ] );
		
		StringCopySafe( data.mDate, ( char* )record.Data[ 7 ], sizeof( data.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const char*		playerIndex	=		( char* )record.Data[ 8 ];
		const char*		beginDate	=		( char* )record.Data[ 7 ];
		const char*		endDate		=		( char* )record.Data[ 9 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 10 ] ) + MAX_ROW_NUM;
		
		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetExperienceLog,
				connectionIndex,
				"EXEC dbo.TP_EXP_LOG_SELECT %s, \'%s\', \'%s\', %d",
				playerIndex,
				beginDate,
				endDate,
				size );
		}		
	}
}


void RGetItemLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM_LOG;
		message.Protocol	= MP_RM_ITEM_LOG_GET_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_ITEM_LOG;
	message.Protocol	= MP_RM_ITEM_LOG_GET_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* playerIndex		= ( char* )record.Data[ 1 ];
	const char* itemDbIndex		= ( char* )record.Data[ 2 ];
	const char* itemIndex		= ( char* )record.Data[ 3 ];
	const char*	beginDate		= ( char* )record.Data[ 4 ];
	const char*	endDate			= ( char* )record.Data[ 5 ];
	const char* requestTick		= ( char* )record.Data[ 6 ];

	g_DB.LogQuery(
		RGetItemLog,
		connectionIndex,
		"EXEC dbo.TP_ITEM_LOG_SELECT %s, %s, %s, \'%s\', \'%s\', %s, 0",
		playerIndex,
		itemDbIndex,
		itemIndex,
		beginDate,
		endDate,
		requestTick );
}


void RGetItemLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	//if(		g_pMSSystem->IsStopQuery( connectionIndex ) ||
	//	!	CUserManager::GetInstance().IsConnect( connectionIndex ) )
	//{
	//	g_pMSSystem->StopQuery(	connectionIndex, MP_RM_ITEM_LOG, MP_RM_ITEM_LOG_GET_STOP_ACK );
	//	return;
	//}

	MSG_RM_ITEM_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category		= MP_RM_ITEM_LOG;
		message.Protocol		= MP_RM_ITEM_LOG_GET_ACK;
		message.mSize			= dbMessage->dwResult;

		if( 0 < dbMessage->dwResult )
		{
			message.mRequestTick = atoi( ( char* )query[ 0 ].Data[ 19 ] );
		}		
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&	record			= query[ i ];
		MSG_RM_ITEM_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex	=						atoi( ( char* )record.Data[ 0 ] );
		data.mType		=	( eLogitemmoney )	atoi( ( char* )record.Data[ 1 ] );
		
		{
			MSG_RM_ITEM_LOG::Player& player = data.mFromPlayer;

			player.mIndex	= atoi( ( char* )record.Data[ 2 ] );
			StringCopySafe( player.mName, ( char* )record.Data[ 3 ], sizeof( player.mName ) );
			player.mMoney	= atoi( ( char* )record.Data[ 4 ] );
		}

		{
			MSG_RM_ITEM_LOG::Player& player = data.mToPlayer;

			player.mIndex	= atoi( ( char* )record.Data[ 5 ] );
			StringCopySafe( player.mName, ( char* )record.Data[ 6 ], sizeof( player.mName ) );
			player.mMoney	= atoi( ( char* )record.Data[ 7 ] );
		}

		data.mMoneyVariation	= atoi( ( char* )record.Data[ 8 ] );

		{
			MSG_RM_ITEM_LOG::Item& item = data.mItem;

			item.mIndex			= atoi( ( char* )record.Data[ 9 ] );
			item.mDbIndex		= atoi( ( char* )record.Data[ 10 ] );
			item.mQuantity		= atoi( ( char* )record.Data[ 11 ] );
			item.mFromPosition	= atoi( ( char* )record.Data[ 12 ] );
			item.mToPosition	= atoi( ( char* )record.Data[ 13 ] );
		}

		StringCopySafe( data.mDate, ( char* )record.Data[ 14 ], sizeof( data.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record	= query[ MAX_ROW_NUM - 1 ];

		const char*	beginDate		=		( char* )record.Data[ 14 ];
		const char* endDate			=		( char* )record.Data[ 15 ];
		const char* playerIndex		=		( char* )record.Data[ 16 ];
		const char* itemDbIndex		=		( char* )record.Data[ 17 ];
		const char* itemIndex		=		( char* )record.Data[ 18 ];
		const DWORD size			= atoi( ( char* )record.Data[ 20 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetItemLog,
				connectionIndex,
				"EXEC dbo.TP_ITEM_LOG_SELECT %s, %s, %s, \'%s\', \'%s\', %d, %d",
				playerIndex,
				itemDbIndex,
				itemIndex,
				beginDate,
				endDate,
				message.mRequestTick,
				size );
		}		
	}
}


void RGetStatLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_STAT_LOG;
		message.Protocol	= MP_RM_STAT_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_STAT_LOG;
	message.Protocol	= MP_RM_STAT_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* playerIndex	= ( char* )record.Data[ 1 ];
	const char*	beginDate	= ( char* )record.Data[ 2 ];
	const char*	endDate		= ( char* )record.Data[ 3 ];

	g_DB.LogQuery(
		RGetStatLog,
		connectionIndex,
		"EXEC dbo.TP_STAT_LOG_SELECT %s, \'%s\', \'%s\', 0",
		playerIndex,
		beginDate,
		endDate );
}


void RGetStatLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	//if(		g_pMSSystem->IsStopQuery( connectionIndex ) ||
	//	!	CUserManager::GetInstance().IsConnect( connectionIndex ) )
	//{
	//	g_pMSSystem->StopQuery(	connectionIndex, MP_RM_STAT_LOG, MP_RM_STAT_LOG_STOP_ACK );
	//	return;
	//}

	MSG_RM_STAT_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_STAT_LOG;
		message.Protocol	= MP_RM_STAT_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&			record	= query[ i ];
		MSG_RM_STAT_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex		= atoi( ( char* )record.Data[ 0 ] );
		data.mLevel			= atoi( ( char* )record.Data[ 1 ] );
		data.mStrength		= atoi( ( char* )record.Data[ 2 ] );
		data.mDexterity		= atoi( ( char* )record.Data[ 3 ] );
		data.mVitality		= atoi( ( char* )record.Data[ 4 ] );
		data.mIntelligence	= atoi( ( char* )record.Data[ 5 ] );
		data.mWisdom		= atoi( ( char* )record.Data[ 6 ] );
		data.mExperience	= atoi( ( char* )record.Data[ 7 ] );

		StringCopySafe( data.mDate, ( char* )record.Data[ 8 ], sizeof( data.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const char*		playerIndex	=		( char* )record.Data[ 9 ];
		const char*		beginDate	=		( char* )record.Data[ 8 ];
		const char*		endDate		=		( char* )record.Data[ 10 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 11 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetStatLog,
				connectionIndex,
				"EXEC dbo.TP_STAT_LOG_SELECT %s, \'%s\', \'%s\', %d",
				playerIndex,
				beginDate,
				endDate,
				size );
		}		
	}
}


void RGetSkillLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_SKILL_LOG;
		message.Protocol	= MP_RM_SKILL_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_SKILL_LOG;
	message.Protocol	= MP_RM_SKILL_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* playerIndex		= ( char* )record.Data[ 1 ];
	const char*	beginDate		= ( char* )record.Data[ 2 ];
	const char*	endDate			= ( char* )record.Data[ 3 ];

	g_DB.LogQuery(
		RGetSkillLog,
		connectionIndex,
		"EXEC dbo.TP_SKILL_LOG_SELECT %s, \'%s\', \'%s\', 0",
		playerIndex,
		beginDate,
		endDate );
}


void RGetSkillLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_SKILL_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_SKILL_LOG;
		message.Protocol	= MP_RM_SKILL_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&			record	= query[ i ];
		MSG_RM_SKILL_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex	=				atoi( ( char* )record.Data[ 0 ] );
		data.mType		= ( eLogSkill )	atoi( ( char* )record.Data[ 1 ] );
		data.mDbIndex	=				atoi( ( char* )record.Data[ 2 ] );
		data.mIndex		= 				atoi( ( char* )record.Data[ 3 ] );
		data.mLevel		=				atoi( ( char* )record.Data[ 4 ] );
		data.mPoint		= 				atoi( ( char* )record.Data[ 5 ] );
		
		StringCopySafe( data.mDate, ( char* )record.Data[ 6 ], sizeof( data.mDate ) );
		StringCopySafe( data.mMemo, ( char* )record.Data[ 7 ], sizeof( data.mMemo ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const char*		playerIndex	=		( char* )record.Data[ 8 ];
		const char*		beginDate	=		( char* )record.Data[ 6 ];
		const char*		endDate		=		( char* )record.Data[ 9 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 10 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetSkillLog,
				connectionIndex,
				"EXEC dbo.TP_SKILL_LOG_SELECT %s, \'%s\', \'%s\', %d",
				playerIndex,
				beginDate,
				endDate,
				size );
		}		
	}
}


void RGetItemOptionLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM_OPTION_LOG;
		message.Protocol	= MP_RM_ITEM_OPTION_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_ITEM_OPTION_LOG;
	message.Protocol	= MP_RM_ITEM_OPTION_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* itemDbIndex	= ( char* )record.Data[ 1 ];
	const char*	beginDate	= ( char* )record.Data[ 2 ];
	const char*	endDate		= ( char* )record.Data[ 3 ];

	g_DB.LogQuery(
		RGetItemOptionLog,
		connectionIndex,
		"EXEC dbo.TP_ITEM_OPTION_LOG_SELECT %s, \'%s\', \'%s\', 0",
		itemDbIndex,
		beginDate,
		endDate );
}


void RGetItemOptionLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_ITEM_OPTION_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_ITEM_OPTION_LOG;
		message.Protocol	= MP_RM_ITEM_OPTION_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&					record	= query[ i ];
		MSG_RM_ITEM_OPTION_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex		= atoi( ( char* )record.Data[ 0 ] );
		data.mItemDbIndex	= atoi( ( char* )record.Data[ 1 ] );
		
		{
			ITEM_OPTION::Reinforce& reinforce = data.mOption.mReinforce;

			reinforce.mStrength			= atoi( ( char* )record.Data[ 2 ] );
			reinforce.mDexterity		= atoi( ( char* )record.Data[ 3 ] );
			reinforce.mVitality			= atoi( ( char* )record.Data[ 4 ] );
			reinforce.mIntelligence		= atoi( ( char* )record.Data[ 5 ] );
			reinforce.mWisdom			= atoi( ( char* )record.Data[ 6 ] );
			reinforce.mLife				= atoi( ( char* )record.Data[ 7 ] );
			reinforce.mMana				= atoi( ( char* )record.Data[ 8 ] );
			reinforce.mLifeRecovery		= atoi( ( char* )record.Data[ 9 ] );
			reinforce.mManaRecovery		= atoi( ( char* )record.Data[ 10 ] );
			reinforce.mPhysicAttack		= atoi( ( char* )record.Data[ 11 ] );	
			reinforce.mPhysicDefence	= atoi( ( char* )record.Data[ 12 ] );
			reinforce.mMagicAttack		= atoi( ( char* )record.Data[ 13 ] );
			reinforce.mMagicDefence		= atoi( ( char* )record.Data[ 14 ] );
			reinforce.mCriticalRate		= atoi( ( char* )record.Data[ 15 ] );
			reinforce.mCriticalDamage	= atoi( ( char* )record.Data[ 16 ] );
			reinforce.mMoveSpeed		= atoi( ( char* )record.Data[ 17 ] );
			reinforce.mEvade			= atoi( ( char* )record.Data[ 18 ] );
			reinforce.mAccuracy			= atoi( ( char* )record.Data[ 19 ] );
		}

		{
			ITEM_OPTION::Mix& mix = data.mOption.mMix;

			mix.mStrength		= atoi( ( char* )record.Data[ 20 ] );
			mix.mIntelligence	= atoi( ( char* )record.Data[ 21 ] );
			mix.mDexterity		= atoi( ( char* )record.Data[ 22 ] );
			mix.mWisdom			= atoi( ( char* )record.Data[ 23 ] );
			mix.mVitality		= atoi( ( char* )record.Data[ 24 ] );
		}

		{
			ITEM_OPTION::Enchant& enchant = data.mOption.mEnchant;
			
            enchant.mIndex	= atoi( ( char* )record.Data[ 25 ] );
			enchant.mLevel	= atoi( ( char* )record.Data[ 26 ] );
		}

		data.mType	= ( eLogitemmoney )atoi( ( char* )record.Data[ 27 ] );

		StringCopySafe( data.mDate, ( char* )record.Data[ 28 ], sizeof( data.mDate ) );
		StringCopySafe( data.mMemo, ( char* )record.Data[ 29 ], sizeof( data.mMemo ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const char*		itemDbIndex	=		( char* )record.Data[ 0 ];
		const char*		beginDate	=		( char* )record.Data[ 28 ];
		const char*		endDate		=		( char* )record.Data[ 30 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 31 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetItemOptionLog,
				connectionIndex,
				"EXEC dbo.TP_ITEM_OPTION_LOG_SELECT %s, \'%s\', \'%s\', %d",
				itemDbIndex,
				beginDate,
				size );
		}		
	}
}


// 080320 LUJ, 드롭 옵션 크기를 알아낸다
void RGetItemDropOptionLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM_DROP_OPTION_LOG;
		message.Protocol	= MP_RM_ITEM_DROP_OPTION_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_ITEM_DROP_OPTION_LOG;
	message.Protocol	= MP_RM_ITEM_DROP_OPTION_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* itemDbIndex	= ( char* )record.Data[ 1 ];
	const char*	beginDate	= ( char* )record.Data[ 2 ];
	const char*	endDate		= ( char* )record.Data[ 3 ];

	g_DB.LogQuery(
		RGetItemDropOptionLog,
		connectionIndex,
		"EXEC dbo.TP_ITEM_DROP_OPTION_LOG_SELECT %s, \'%s\', \'%s\', 0",
		itemDbIndex,
		beginDate,
		endDate );
}


// 080320 LUJ, 드롭 옵션 로그를 가져온다
void RGetItemDropOptionLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_ITEM_OPTION_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_ITEM_DROP_OPTION_LOG;
		message.Protocol	= MP_RM_ITEM_DROP_OPTION_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&					record	= query[ i ];
		MSG_RM_ITEM_OPTION_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex		= atoi( ( char* )record.Data[ 0 ] );
		data.mItemDbIndex	= atoi( ( char* )record.Data[ 1 ] );
		data.mType			= ( eLogitemmoney )atoi( ( char* )record.Data[ 2 ] );
		// 081117 LUJ, 형식을 유니코드로 변경
		StringCopySafe( data.mDate, ( char* )record.Data[ 3 ], sizeof( data.mDate ) );
		StringCopySafe( data.mMemo, ( char* )record.Data[ 4 ], sizeof( data.mMemo ) );
		
		{
			ITEM_OPTION::Drop& drop = data.mOption.mDrop;

			drop.mValue[ 0 ].mKey	= ITEM_OPTION::Drop::Key(	atoi( ( char* )record.Data[ 5 ] ) );
			drop.mValue[ 0 ].mValue	=					float(	atof( ( char* )record.Data[ 6 ] ) );

			drop.mValue[ 1 ].mKey	= ITEM_OPTION::Drop::Key(	atoi( ( char* )record.Data[ 7 ] ) );
			drop.mValue[ 1 ].mValue	=					float(	atof( ( char* )record.Data[ 8 ] ) );

			drop.mValue[ 2 ].mKey	= ITEM_OPTION::Drop::Key(	atoi( ( char* )record.Data[ 9 ] ) );
			drop.mValue[ 2 ].mValue	=					float(	atof( ( char* )record.Data[ 10 ] ) );

			drop.mValue[ 3 ].mKey	= ITEM_OPTION::Drop::Key(	atoi( ( char* )record.Data[ 11 ] ) );
			drop.mValue[ 3 ].mValue	=					float(	atof( ( char* )record.Data[ 12 ] ) );

			drop.mValue[ 4 ].mKey	= ITEM_OPTION::Drop::Key(	atoi( ( char* )record.Data[ 13 ] ) );
			drop.mValue[ 4 ].mValue	=					float(	atof( ( char* )record.Data[ 14 ] ) );
		}
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const DWORD		itemDbIndex	= atoi( ( char* )record.Data[ 1 ] );
		const char*		beginDate	=		( char* )record.Data[ 3 ];
		const char*		endDate		=		( char* )record.Data[ 15 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 16 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetItemDropOptionLog,
				connectionIndex,
				"EXEC dbo.TP_ITEM_DROP_OPTION_LOG_SELECT %s, \'%s\', \'%s\', %d",
				itemDbIndex,
				beginDate,
				endDate,
				size );
		}		
	}
}


void RGetGuildLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_GUILD_LOG;
		message.Protocol	= MP_RM_GUILD_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_GUILD_LOG;
	message.Protocol	= MP_RM_GUILD_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* guildIndex	= ( char* )record.Data[ 1 ];
	const char*	beginDate	= ( char* )record.Data[ 2 ];
	const char*	endDate		= ( char* )record.Data[ 3 ];
	const char* playerIndex	= ( char* )record.Data[ 4 ];
	
	g_DB.LogQuery(
		RGetGuildLog,
		connectionIndex,
		"EXEC dbo.TP_GUILD_LOG_SELECT %s, %s, \'%s\', \'%s\', 0",
		guildIndex,
		playerIndex,
		beginDate,
		endDate );
}


void RGetGuildLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_GUILD_LOG message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_GUILD_LOG;
	message.Protocol	= MP_RM_GUILD_LOG_GET_ACK;
	message.mSize		= dbMessage->dwResult;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&			record	= query[ i ];
		MSG_RM_GUILD_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex		=				atoi( ( char* )record.Data[ 0 ] );
		data.mType			= ( eGuildLog )	atoi( ( char* )record.Data[ 1 ] );
		data.mPlayerIndex	=				atoi( ( char* )record.Data[ 2 ] );
		data.mValue1		= 				atoi( ( char* )record.Data[ 3 ] );
		data.mValue2		= 				atoi( ( char* )record.Data[ 4 ] );
		
		StringCopySafe( data.mDate, ( char* )record.Data[ 5 ], sizeof( data.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const char*		guildIndex	=		( char* )record.Data[ 6 ];
		const char*		beginDate	=		( char* )record.Data[ 5 ];
		const char*		endDate		=		( char* )record.Data[ 7 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 8 ] ) + MAX_ROW_NUM;
		const char*		playerIndex	=		( char* )record.Data[ 9 ];

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetGuildLog,
				connectionIndex,
				"EXEC dbo.TP_GUILD_LOG_SELECT %s, %s, \'%s\', \'%s\', %d",
				guildIndex,
				playerIndex,
				beginDate,
				endDate,
				size );
		}		
	}
}


void RGetGuildWarehouseLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
			0 == atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_GUILD_WAREHOUSE_LOG;
		message.Protocol	= MP_RM_GUILD_WAREHOUSE_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_GUILD_WAREHOUSE_LOG;
	message.Protocol	= MP_RM_GUILD_WAREHOUSE_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* guildIndex	= ( char* )record.Data[ 1 ];
	const char* beginDate	= ( char* )record.Data[ 2 ];
	const char* endDate		= ( char* )record.Data[ 3 ];

	g_DB.LogQuery(
		RGetGuildWarehouseLog,
		connectionIndex,
		"EXEC dbo.TP_GUILD_WAREHOUSE_LOG_SELECT %s, \'%s\', \'%s\', 0",
		guildIndex,
		beginDate,
		endDate );
}


void RGetGuildWarehouseLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_GUILD_WAREHOUSE_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_GUILD_WAREHOUSE_LOG;
		message.Protocol	= MP_RM_GUILD_WAREHOUSE_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&						record	= query[ i ];
		MSG_RM_GUILD_WAREHOUSE_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex		= atoi( ( char* )record.Data[ 0 ] );
		data.mPlayerIndex	= atoi( ( char* )record.Data[ 1 ] );

		{
			ITEMBASE& item = data.mItem;

			item.wIconIdx	= atoi( ( char* )record.Data[ 2 ] );
			item.dwDBIdx	= atoi( ( char* )record.Data[ 3 ] );
			item.Durability	= atoi( ( char* )record.Data[ 4 ] );
			item.Position	= atoi( ( char* )record.Data[ 5 ] );
		}

		data.mItemFromPosition	=					atoi( ( char* )record.Data[ 6 ] );
		data.mMoney				=					atoi( ( char* )record.Data[ 7 ] );
		data.mType				= ( eLogitemmoney )	atoi( ( char* )record.Data[ 8 ] );
		
		StringCopySafe( data.mDate, ( char* )record.Data[ 9 ], sizeof( data.mDate ) );
		StringCopySafe( data.mMemo, ( char* )record.Data[ 10 ], sizeof( data.mMemo ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const char*		guildIndex	=		( char* )record.Data[ 12 ];
		const char*		beginDate	=		( char* )record.Data[ 9 ];
		const char*		endDate		=		( char* )record.Data[ 11 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 13 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetGuildWarehouseLog,
				connectionIndex,
				"EXEC dbo.TP_GUILD_WAREHOUSE_LOG_SELECT %s, \'%s\', \'%s\', %d",
				guildIndex,
				beginDate,
				endDate,
				size );
		}		
	}
}


void RGetUserIndex( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_SEARCH;
		message.Protocol	= MP_RM_SEARCH_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	for( DWORD row = 0; row < dbMessage->dwResult; ++row )
	{
		const MIDDLEQUERYST& record = query[ row ];

		const DWORD userIndex = atoi( ( char* )record.Data[ 0 ] );

		g_DB.LoginMiddleQuery(
			RGetUserList,
			connectionIndex,
			"EXEC dbo.TP_USER_SELECT %d, \'%s\', \'%s\'",
			userIndex,
			"",
			"" );
	}
}


void RGetUserData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_USER;
		message.Protocol	= MP_RM_USER_GET_DATA_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_USER_DATA message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_USER;
	message.Protocol	= MP_RM_USER_GET_DATA_ACK;

	message.mIndex		= atoi( ( char* )record.Data[ 0 ] );
	StringCopySafe( message.mId, ( char* )record.Data[ 1 ], sizeof( message.mId ) );
	StringCopySafe( message.mName, ( char* )record.Data[ 2 ], sizeof( message.mName ) );
	StringCopySafe( message.mJoinedDate, ( char* )record.Data[ 3 ], sizeof( message.mJoinedDate ) );
	StringCopySafe( message.mLogedoutDate, ( char* )record.Data[ 4 ], sizeof( message.mLogedoutDate ) );
	message.mPlayTime	= atoi( ( char* )record.Data[ 5 ] );
	message.mLevel		= atoi( ( char* )record.Data[ 6 ] );
	// 080725 LUJ, 상태값 추가
	message.mState		= atoi( ( char* )record.Data[ 9 ] );
	
	{
		MSG_RM_USER_DATA::Connection& data = message.mConnection;

		StringCopySafe( data.mIp, ( char* )record.Data[ 7 ], sizeof( data.mIp ) );
		data.mAgent	= atoi( ( char* )record.Data[ 8 ] );
	}

	// 090618 ShinJS --- Auto Punish 상태정보 추가
	message.mAutoPunishKind = atoi( ( char* )record.Data[ 10 ] );
	StringCopySafe( message.mAutoPunishStartDate, ( char* )record.Data[ 11 ], sizeof( message.mAutoPunishStartDate ) );
	StringCopySafe( message.mAutoPunishEndDate, ( char* )record.Data[ 12 ], sizeof( message.mAutoPunishEndDate ) );
	message.mAutoPunishTime = atoi( ( char* )record.Data[ 13 ] );
	StringCopySafe(
		message.mEnpangName,
		LPCTSTR(record.Data[14]),
		_countof(message.mEnpangName));

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	g_DB.FreeQuery(
		RGetUserPlayerList,
		connectionIndex,
		"EXEC dbo.TP_CHARACTER_SELECT %d, \'%s\', \'%s\', %d",
		message.mIndex,
		"",
		"",
		0 );
}


void RGetUserPlayerList( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_OPEN_ACK	message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_USER;
		message.Protocol	= MP_RM_USER_GET_PLAYER_LIST_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		MSG_RM_OPEN_ACK::Data&	data	= message.mData[ i ];
		const QUERYST&			record	= query[ i ];

		data.mUserIndex = atoi( ( char* )record.Data[ 0 ] );
		StringCopySafe( data.mUserName,		( char* )record.Data[ 1 ], sizeof( data.mUserName ) );

		data.mPlayerIndex = atoi( ( char* )record.Data[ 2 ] );
		StringCopySafe( data.mPlayerName,	( char* )record.Data[ 3 ], sizeof( data.mPlayerName ) );

		data.mStandIndex = atoi( ( char* )record.Data[ 4 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];

		const char* userIndex	= ( char* )record.Data[ 0 ];
		const char* playerIndex	= ( char* )record.Data[ 2 ];

		g_DB.FreeQuery(
			RGetUserPlayerList,
			connectionIndex,
			"EXEC dbo.TP_CHARACTER_SELECT %s, \'%s\', \'%s\', %s",
			userIndex,
			"",
			"",
			playerIndex );
	}
}


void UserAdd( DWORD connectionIndex, const CHARACTERMAKEINFO& data )
{
	// 에이전트 서버의 CreateCharacter()에서 복사함

	const BYTE StrArray[ 2 ][ 3 ] = { {16, 12,  8 }, { 13, 11,  7 } };
	const BYTE DexArray[ 2 ][ 3 ] = { {10, 14,  7 }, { 12, 15,  8 } };
	const BYTE VitArray[ 2 ][ 3 ] = { {14, 12, 11 }, { 13, 11,  9 } };
	const BYTE IntArray[ 2 ][ 3 ] = { { 6,  7, 13 }, {  7,  8, 14 } };
	const BYTE WisArray[ 2 ][ 3 ] = { { 7,  8, 14 }, {  8,  8, 15 } };

	const BYTE race	= data.RaceType;
	const BYTE job	= data.JobType -1;

	const BYTE Str = StrArray[ race ][ job ] * 5;
	const BYTE Dex = DexArray[ race ][ job ] * 5;
	const BYTE Vit = VitArray[ race ][ job ] * 5;
	const BYTE Int = IntArray[ race ][ job ] * 5;
	const BYTE Wis = WisArray[ race ][ job ] * 5;

	const DWORD item[3][2] =
	{
		11000001,	12000032,
		11000187,	12000032,
		11000249,	12000001
	};

	const int loginPoint = 2019;
	const int map = 19;

	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	g_DB.FreeMiddleQuery(
		RUserAdd,
		connectionIndex,
		"EXEC dbo.MP_CHARACTER_CREATECHARACTER %d, %d, %d, %d, %d, %d, \'%s\', %d, %d, %d, %d, %d, %d, %d, %d, \'GM:%s\', %u, %u",
		data.UserID,
		Str,
		Dex,
		Vit,
		Int,
		Wis, 
		data.Name,
		data.FaceType,
		data.HairType,
		map,
		data.SexType,
		data.RaceType,
		data.JobType,
		loginPoint,
		0,
		user.mId.c_str(),
		item[ job % 3 ][ 0 ],
		item[ job % 3 ][ 1 ] );
}


void RUserAdd( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_USER;
		message.Protocol	= MP_RM_USER_CREATE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	switch( atoi( ( char* )record.Data[ 0 ] ) )
	{
	case 0:
		{
			MSGROOT message;
			{
				message.Category	= MP_RM_USER;
				message.Protocol	= MP_RM_USER_CREATE_ACK;
			}

			NETWORK->Send( connectionIndex, message, sizeof( message ) );

			{
				const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

				if( user.mId.empty() )
				{
					return;
				}

				g_DB.LogMiddleQuery(
					0,
					0,
					"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1 ) VALUES ( %d, \'%s\', \'%s\', %s )",
					RecoveryLogAddPlayerToUser,
					user.mId.c_str(),
					"playerIndex",
					( char* )record.Data[ 1 ] );
			}

			break;
		}
	case 1:
		{
			MSGROOT message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_CREATE_NACK_BY_FULL;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	case 2:
		{
			MSGROOT message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_CREATE_NACK_BY_DUPLICATED_NAME;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	case 3:
		{
			MSGROOT message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_CREATE_NACK_BY_GUILD;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			break;
		}
	}
}


void RestoreUser( DWORD connectionIndex, DWORD playerIndex, const char* playerName )
{
	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	g_DB.FreeMiddleQuery(
		RRestoreUser,
		connectionIndex,
		"EXEC dbo.TP_CHARACTERRECOVER %d, \'%s\', \'GM:%s\', %d",
		playerIndex,
		playerName,
		user.mId.c_str(),		// IP: 복구해준 RM툴 ID를 넣는다
		0 );			// 서버 번호. RM툴이 복구하므로 0을 넣는다
}


void RRestoreUser( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_USER;
		message.Protocol	= MP_RM_USER_RESTORE_NACK;
		
		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	switch( atoi( ( char* )record.Data[ 0 ] ) )
	{
	case 0:
		{
			MSG_NAME message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_RESTORE_ACK;
			message.dwObjectID	= atoi( ( char* )record.Data[ 1 ] );
			StringCopySafe( message.Name, ( char* )record.Data[ 2 ], sizeof( message.Name ) );

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	case 1:
		{
			MSGROOT message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_RESTORE_NACK_BY_NOT_EXIST;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	case 2:
		{
			MSGROOT message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_RESTORE_NACK_BY_FULL;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	case 3:
		{
			MSG_NAME message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_RESTORE_NACK_BY_DUPLICATED_NAME;
			message.dwObjectID	= atoi( ( char* )record.Data[ 1 ] );
			StringCopySafe( message.Name, ( char* )record.Data[ 2 ], sizeof( message.Name ) );

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	case 4:
		{
			MSG_NAME message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_RESTORE_NACK_BY_GUILD;
			message.dwObjectID	= atoi( ( char* )record.Data[ 1 ] );
			StringCopySafe( message.Name, ( char* )record.Data[ 2 ], sizeof( message.Name ) );

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	case 5:
		{
			MSG_NAME message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_RESTORE_NACK_BY_FAMILY;
			message.dwObjectID	= atoi( ( char* )record.Data[ 1 ] );
			StringCopySafe( message.Name, ( char* )record.Data[ 2 ], sizeof( message.Name ) );

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );

			MSGROOT message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_RESTORE_NACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	}
}


void DeleteUser( DWORD connectionIndex, DWORD playerIndex )
{
	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return;
	}

	g_DB.FreeMiddleQuery(
		RDeleteUser,
		connectionIndex,
		"EXEC dbo.MP_CHARACTER_DELETECHARACTER %d, %d, \'GM:%s\'",
		playerIndex,
		0,				// IP: 복구해준 RM툴 ID를 넣는다
		user.mId.c_str() );		// 서버 번호. RM툴이 복구하므로 0을 넣는다
}


void RDeleteUser( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult )
	{
		MSGROOT message;
		message.Category	= MP_RM_USER;
		message.Protocol	= MP_RM_USER_DELETE_NACK;
		
		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	switch( atoi( ( char* )record.Data[ 0 ] ) )
	{
	case 0:
		{
			MSGBASE message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_DELETE_ACK;
			message.dwObjectID	= atoi( ( char* )record.Data[ 1 ] );

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	case 1:
		{
			MSGROOT message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_DELETE_NACK_BY_PARTY;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	case 2:
		{
			MSGROOT message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_DELETE_NACK_BY_GUILD;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	case 3:
		{
			MSGROOT message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_DELETE_NACK_BY_FAMILY;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	default:
		{
			ASSERT( 0 );

			MSGROOT message;
			message.Category	= MP_RM_USER;
			message.Protocol	= MP_RM_USER_DELETE_NACK;

			NETWORK->Send( connectionIndex, message, sizeof( message ) );
			break;
		}
	}
}


void RGetJobLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_JOB_LOG;
		message.Protocol	= MP_RM_JOB_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_JOB_LOG;
	message.Protocol	= MP_RM_JOB_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* playerIndex	= ( char* )record.Data[ 1 ];
	const char*	beginDate	= ( char* )record.Data[ 2 ];
	const char*	endDate		= ( char* )record.Data[ 3 ];

	g_DB.LogQuery(
		RGetJobLog,
		connectionIndex,
		"EXEC dbo.TP_JOB_LOG_SELECT %s, \'%s\', \'%s\', 0",
		playerIndex,
		beginDate,
		endDate );
}


void RGetJobLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	/*if(		g_pMSSystem->IsStopQuery( connectionIndex ) ||
		!	CUserManager::GetInstance().IsConnect( connectionIndex ) )
	{
		g_pMSSystem->StopQuery(	connectionIndex, MP_RM_JOB_LOG, MP_RM_JOB_LOG_STOP_ACK );
		return;
	}*/

	MSG_RM_JOB_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_JOB_LOG;
		message.Protocol	= MP_RM_JOB_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&			record	= query[ i ];
		MSG_RM_JOB_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex		= atoi( ( char* )record.Data[ 0 ] );
		data.mClassIndex	= atoi( ( char* )record.Data[ 1 ] );
		data.mJobGrade		= atoi( ( char* )record.Data[ 2 ] );
		data.mJobIndex		= atoi( ( char* )record.Data[ 3 ] );
		
		StringCopySafe( data.mDate, ( char* )record.Data[ 4 ], sizeof( data.mDate ) );
		StringCopySafe( data.mMemo, ( char* )record.Data[ 5 ], sizeof( data.mMemo ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record = query[ MAX_ROW_NUM - 1 ];
		const char*		playerIndex	=		( char* )record.Data[ 6 ];
		const char*		beginDate	=		( char* )record.Data[ 4 ];
		const char*		endDate		=		( char* )record.Data[ 7 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 8 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetJobLog,
				connectionIndex,
				"EXEC dbo.TP_JOB_LOG_SELECT %s, \'%s\', \'%s\', %d",
				playerIndex,
				beginDate,
				endDate,
				size );
		}		
	}
}


void RGetFarmCropLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_FAMILY_CROP_LOG;
		message.Protocol	= MP_RM_FAMILY_CROP_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_FAMILY_CROP_LOG;
	message.Protocol	= MP_RM_FAMILY_CROP_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* farmIndex	= ( char* )record.Data[ 1 ];
	const char*	beginDate	= ( char* )record.Data[ 2 ];
	const char*	endDate		= ( char* )record.Data[ 3 ];

	g_DB.LogQuery(
		RGetFarmCropLog,
		connectionIndex,
		"EXEC dbo.TP_FARM_CROP_LOG_SELECT %s, \'%s\', \'%s\', 0",
		farmIndex,
		beginDate,
		endDate );
}


void RGetFarmCropLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	/*if(		g_pMSSystem->IsStopQuery( connectionIndex ) ||
		!	CUserManager::GetInstance().IsConnect( connectionIndex ) )
	{
		g_pMSSystem->StopQuery(	connectionIndex, MP_RM_FAMILY_CROP_LOG, MP_RM_FAMILY_CROP_LOG_STOP_ACK );
		return;
	}*/

	MSG_RM_FAMILY_CROP_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_FAMILY_CROP_LOG;
		message.Protocol	= MP_RM_FAMILY_CROP_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&					record	= query[ i ];
		MSG_RM_FAMILY_CROP_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex		= atoi( ( char* )record.Data[ 0 ] );
		data.mOwnerIndex	= atoi( ( char* )record.Data[ 1 ] );
		data.mCropIndex		= atoi( ( char* )record.Data[ 2 ] );
		data.mCropKind		= atoi( ( char* )record.Data[ 3 ] );
		data.mCropStep		= atoi( ( char* )record.Data[ 4 ] );
		data.mCropLife		= atoi( ( char* )record.Data[ 5 ] );
		data.mCropSeedGrade	= atoi( ( char* )record.Data[ 6 ] );
		data.mType			= ( eFamilyLog )	atoi( ( char* )record.Data[ 7 ] );

		StringCopySafe( data.mDate, ( char* )record.Data[ 8 ], sizeof( data.mDate ) );
		StringCopySafe( data.mMemo, ( char* )record.Data[ 9 ], sizeof( data.mMemo ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const char*		farmIndex	=		( char* )record.Data[ 10 ];
		const char*		beginDate	=		( char* )record.Data[ 8 ];
		const char*		endDate		=		( char* )record.Data[ 11 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 12 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetFarmCropLog,
				connectionIndex,
				"EXEC dbo.TP_FARM_CROP_LOG_SELECT %s, \'%s\', \'%s\', %d",
				farmIndex,
				beginDate,
				endDate,
				size );
		}		
	}
}


void RGetFamilyPointLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		!	atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_FAMILY_POINT_LOG;
		message.Protocol	= MP_RM_FAMILY_POINT_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_FAMILY_POINT_LOG;
	message.Protocol	= MP_RM_FAMILY_POINT_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* familyIndex		= ( char* )record.Data[ 1 ];
	const char*	beginDate		= ( char* )record.Data[ 2 ];
	const char*	endDate			= ( char* )record.Data[ 3 ];

	g_DB.LogQuery(
		RGetFamilyPointLog,
		connectionIndex,
		"EXEC dbo.TP_FAMILY_POINT_LOG_SELECT %s, \'%s\', \'%s\', 0",
		familyIndex,
		beginDate,
		endDate );
}


void RGetFamilyPointLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	//if(		g_pMSSystem->IsStopQuery( connectionIndex ) ||
	//	!	CUserManager::GetInstance().IsConnect( connectionIndex ) )
	//{
	//	g_pMSSystem->StopQuery(	connectionIndex, MP_RM_FAMILY_POINT_LOG, MP_RM_FAMILY_POINT_LOG_STOP_ACK );
	//	return;
	//}

	MSG_RM_FAMILY_POINT_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_FAMILY_POINT_LOG;
		message.Protocol	= MP_RM_FAMILY_POINT_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	// 첫번째 레코드는 다음 쿼리를 위한 정보가 담겨있다
	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&					record	= query[ i ];
		MSG_RM_FAMILY_POINT_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex		=				atoi( ( char* )record.Data[ 0 ] );
		data.mPoint			=				atoi( ( char* )record.Data[ 1 ] );
		data.mType			= ( eFamilyLog )atoi( ( char* )record.Data[ 2 ] );

		StringCopySafe( data.mDate, ( char* )record.Data[ 3 ], sizeof( data.mDate ) );
		StringCopySafe( data.mMemo, ( char* )record.Data[ 4 ], sizeof( data.mMemo ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const char*		familyIndex	=		( char* )record.Data[ 5 ];
		const char*		beginDate	=		( char* )record.Data[ 3 ];
		const char*		endDate		=		( char* )record.Data[ 6 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 7 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetFamilyPointLog,
				connectionIndex,
				"EXEC dbo.TP_FAMILY_POINT_LOG_SELECT %s, \'%s\', \'%s\', %d",
				familyIndex,
				beginDate,
				endDate,
				size );
		}		
	}
}


void RGetGuildScoreLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( !	dbMessage->dwResult ||
		0 == atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_GUILD_SCORE_LOG;
		message.Protocol	= MP_RM_GUILD_SCORE_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_DWORD message;
	message.Category	= MP_RM_GUILD_SCORE_LOG;
	message.Protocol	= MP_RM_GUILD_SCORE_LOG_SIZE_ACK;
	message.dwData		= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	const char* guildIndex	=		( char* )record.Data[ 1 ];
	const char*	beginDate	=		( char* )record.Data[ 2 ];
	const char*	endDate		=		( char* )record.Data[ 3 ];

	g_DB.LogQuery(
		RGetGuildScoreLog,
		connectionIndex,
		"EXEC dbo.TP_GUILD_SCORE_LOG_SELECT %s, \'%s\', \'%s\', 0",
		guildIndex,
		beginDate,
		endDate );
}


void RGetGuildScoreLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_GUILD_SCORE_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_GUILD_SCORE_LOG;
		message.Protocol	= MP_RM_GUILD_SCORE_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&					record	= query[ i ];
		MSG_RM_GUILD_SCORE_LOG::Log&	data	= message.mLog[ i ];

		data.mLogIndex		= atoi( ( char* )record.Data[ 0 ] );
		data.mScore			= atoi( ( char* )record.Data[ 1 ] );
		data.mType			= ( eGuildLog )	atoi( ( char* )record.Data[ 2 ] );

		// 날짜 복사
		StringCopySafe( data.mDate, ( char* )record.Data[ 3 ], sizeof( data.mDate ) );
		StringCopySafe( data.mMemo, ( char* )record.Data[ 4 ], sizeof( data.mMemo ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];
		const char* guildIndex	=		( char* )record.Data[ 5 ];
		const char* beginDate	=		( char* )record.Data[ 3 ];
		const char* endDate		=		( char* )record.Data[ 6 ];
		const DWORD	size		= atoi( ( char* )record.Data[ 7 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetGuildScoreLog,
				connectionIndex,
				"EXEC dbo.TP_GUILD_SCORE_LOG_SELECT %s, \'%s\', \'%s\', %d",
				guildIndex,
				beginDate,
				endDate,
				size );
		}
	}
}


void AddPermission( DWORD connectionIndex, const char* ipAddress )
{
    g_DB.LoginMiddleQuery(
		RAddPermission,
		connectionIndex,
		"EXEC dbo.TP_PERMISSION_INSERT \'%s\'",
		ipAddress );
}


void RAddPermission( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		return;
	}

	const MIDDLEQUERYST&	record		= query[ 0 ];
	const bool				isSuccess	= ( 0 < atoi( ( char* )record.Data[ 0 ] ) );

	if( isSuccess )
	{
		MSGROOT message;
		message.Category	= MP_RM_PERMISSION;
		message.Protocol	= MP_RM_PERMISSION_ADD_ACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
	else
	{
		MSGROOT message;
		message.Category	= MP_RM_PERMISSION;
		message.Protocol	= MP_RM_PERMISSION_ADD_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
}


void RemovePermission( DWORD connectionIndex, const char* ipAddress )
{
	g_DB.LoginMiddleQuery(
		RRemovePermission,
		connectionIndex,
		"EXEC dbo.TP_PERMISSION_DELETE \'%s\'",
		ipAddress );
}


void RRemovePermission( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult )
	{
		return;
	}

	const MIDDLEQUERYST&	record		= query[ 0 ];
	const bool				isSuccess	= ( 0 < atoi( ( char* )record.Data[ 0 ] ) );

	if( isSuccess )
	{
		MSGROOT message;
		message.Category	= MP_RM_PERMISSION;
		message.Protocol	= MP_RM_PERMISSION_REMOVE_ACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
	else
	{
		MSGROOT message;
		message.Category	= MP_RM_PERMISSION;
		message.Protocol	= MP_RM_PERMISSION_REMOVE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
}


void RGetStorageData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_DWORD3 message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_ITEM;
	message.Protocol	= MP_RM_ITEM_GET_STORAGE_ACK;

	if( 1 == dbMessage->dwResult )
	{
		const MIDDLEQUERYST& record = query[ 0 ];

		// 창고 크기
		message.dwData1	= atoi( ( char* )record.Data[ 1 ] );

		// 창고 보유금
		message.dwData2	= atoi( ( char* )record.Data[ 2 ] );

		// 확장 인벤토리 크기
		message.dwData3	= atoi( ( char* )record.Data[ 3 ] );	
	}

	NETWORK->Send( connectionIndex, message, sizeof( message ) );	
}


void RSetStorageData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_DWORD2 message;
	ZeroMemory( &message, sizeof( message ) );

	message.Category	= MP_RM_ITEM;
	message.Protocol	= MP_RM_ITEM_SET_STORAGE_ACK;

	if( 1 == dbMessage->dwResult )
	{
		const MIDDLEQUERYST& record = query[ 0 ];

		// 창고 크기
		message.dwData1	= atoi( ( char* )record.Data[ 1 ] );

		// 창고 보유금
		message.dwData2	= atoi( ( char* )record.Data[ 2 ] );
	}

	NETWORK->Send( connectionIndex, message, sizeof( message ) );	
}


void RGetOperator( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_OPERATOR message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}
	
	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&				record	= query[ i ];
		MSG_RM_OPERATOR::Operator&	data	= message.mOperator[ i ];

		data.mIndex	=				atoi( ( char* )record.Data[ 0 ] );
		data.mPower	= eGM_POWER(	atoi( ( char* )record.Data[ 1 ] ) );

		const char* userId = ( char* )record.Data[ 2 ];

		StringCopySafe( data.mId,			userId,						sizeof( data.mId ) );
		StringCopySafe( data.mName,			( char* )record.Data[ 3 ],	sizeof( data.mName ) );
		StringCopySafe( data.mRegistedDate,	( char* )record.Data[ 4 ],	sizeof( data.mRegistedDate ) );

		const CUserManager::User& user = CUserManager::GetInstance().GetUser( userId );

		if( ! user.mId.empty() )
		{
			StringCopySafe( data.mIp, user.mIp.c_str(), sizeof( data.mIp ) );
		}
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void RAddOperator( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult ||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] )  )
	{
		MSGROOT message;
		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_ADD_NACK_BY_ID;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}
	
	const MIDDLEQUERYST& record = query[ 0 ];

	{
		MSG_RM_OPERATOR message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_ADD_ACK;
		message.mSize		= 1;

		MSG_RM_OPERATOR::Operator& data = message.mOperator[ 0 ];
		data.mIndex			=				atoi( ( char* )record.Data[ 0 ] );
		data.mPower			= eGM_POWER(	atoi( ( char* )record.Data[ 1 ] ) );
        
		StringCopySafe( data.mId,			( char* )record.Data[ 2 ],	sizeof( data.mId ) );
		StringCopySafe( data.mName,			( char* )record.Data[ 3 ],	sizeof( data.mName ) );
		StringCopySafe( data.mRegistedDate,	( char* )record.Data[ 4 ],	sizeof( data.mRegistedDate ) );
		
		NETWORK->Send( connectionIndex, message, message.GetSize() );
	}
}


void RUpdateOperator( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult ||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] )  )
	{
		MSGROOT message;
		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_SET_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	{
		MSG_DWORD message;
		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_SET_ACK;

		// operator index
		message.dwObjectID	= atoi( ( char* )record.Data[ 0 ] );

		// rank
		message.dwData		= atoi( ( char* )record.Data[ 1 ] );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
}


void RGetOperatorIp( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_OPERATOR_IP message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_IP_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&		record	= query[ i ];
		MSG_RM_OPERATOR_IP::Address& address = message.mAddress[ i ];

		StringCopySafe( address.mIp, ( char* )record.Data[ 0 ], sizeof( address.mIp ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


void RAddOperatorIp( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult ||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] )  )
	{
		MSGROOT message;
		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_IP_ADD_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	{
		MSG_NAME message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_IP_ADD_ACK;
		StringCopySafe( message.Name, ( char* )record.Data[ 0 ], sizeof( message.Name ) );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
}


void RRemoveOperatorIp( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult ||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] )  )
	{
		MSGROOT message;
		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_IP_REMOVE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	{
		MSG_NAME message;
		message.Category	= MP_RM_OPERATOR;
		message.Protocol	= MP_RM_OPERATOR_IP_REMOVE_ACK;
		StringCopySafe( message.Name, ( char* )record.Data[ 0 ], sizeof( message.Name ) );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
}


void RSetPassword( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult ||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_PASSWORD;
		message.Protocol	= MP_RM_PASSWORD_SET_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	//const MIDDLEQUERYST& record = query[ 0 ];

	{
		MSGROOT message;
		message.Category	= MP_RM_PASSWORD;
		message.Protocol	= MP_RM_PASSWORD_SET_ACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}
}


void RGetOperatorLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_OPERATOR_LOG;
		message.Protocol	= MP_RM_OPERATOR_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];
	
	MSG_RM_LOG_SIZE message;
	{
		message.Category		= MP_RM_OPERATOR_LOG;
		message.Protocol		= MP_RM_OPERATOR_LOG_SIZE_ACK;
		message.mSize			= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick	= atoi( ( char* )record.Data[ 3 ] );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* beginDate	= ( char* )record.Data[ 1 ];
	const char* endDate		= ( char* )record.Data[ 2 ];
	
	g_DB.LogQuery(
		RGetOperatorLog,
		connectionIndex,
		"EXEC dbo.TP_OPERATOR_LOG_SELECT \'%s\', \'%s\', %d, 0",
		beginDate,
		endDate,
		message.mRequestTick );
}


void RGetOperatorLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_OPERATOR_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category		= MP_RM_OPERATOR_LOG;
		message.Protocol		= MP_RM_OPERATOR_LOG_ACK;
		message.mSize			= dbMessage->dwResult;

		if( 0 < dbMessage->dwResult )
		{
			message.mRequestTick	= atoi( ( char* )query[ 0 ].Data[ 11 ] );
		}
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )	
	{
		const QUERYST&				record	= query [ i ];
		MSG_RM_OPERATOR_LOG::Log&	log		= message.mLog[ i ];

		log.mIndex	= atoi( ( char* )record.Data[ 0 ] );
		log.mType	= RecoveryLog( atoi( ( char* )record.Data[ 1 ] ) );
		StringCopySafe( log.mOperator,	( char* )record.Data[ 2 ], sizeof( log.mOperator ) );
		StringCopySafe( log.mDate,		( char* )record.Data[ 3 ], sizeof( log.mDate ) );
		StringCopySafe( log.mKey1,		( char* )record.Data[ 4 ], sizeof( log.mKey1 ) );
		StringCopySafe( log.mValue1,	( char* )record.Data[ 5 ], sizeof( log.mValue1 ) );
		StringCopySafe( log.mKey2,		( char* )record.Data[ 6 ], sizeof( log.mKey2 ) );
		StringCopySafe( log.mValue2,	( char* )record.Data[ 7 ], sizeof( log.mValue2 ) );
		StringCopySafe( log.mKey3,		( char* )record.Data[ 8 ], sizeof( log.mKey3 ) );
		StringCopySafe( log.mValue3,	( char* )record.Data[ 9 ], sizeof( log.mValue3 ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];

		const char* beginDate	=		( char* )record.Data[ 3 ];
		const char* endDate		=		( char* )record.Data[ 10 ];
		const DWORD		size	= atoi( ( char* )record.Data[ 12 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetOperatorLog,
				connectionIndex,
				"EXEC dbo.TP_OPERATOR_LOG_SELECT \'%s\', \'%s\', %d, %d",
				beginDate,
				endDate,
				message.mRequestTick,
				size );
		}		
	}
}


void RGetNameLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_NAME_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_NAME_LOG;
		message.Protocol	= MP_RM_NAME_LOG_GET_ACK;
		message.mSize		= dbMessage->dwResult;
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&			record	= query[ i ];
		MSG_RM_NAME_LOG::Log&	log		= message.mLog[ i ];

		log.mIndex	= atoi( ( char* )record.Data[ 0 ] );

		StringCopySafe( log.mPreviousName,	( char* )record.Data[ 1 ], sizeof( log.mPreviousName ) );
		StringCopySafe( log.mName,			( char* )record.Data[ 2 ], sizeof( log.mName ) );
		StringCopySafe( log.mDate,			( char* )record.Data[ 3 ], sizeof( log.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


// 080401 LUJ, 농장 로그
void RGetFarmLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_FARM_LOG;
		message.Protocol	= MP_RM_FARM_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_LOG_SIZE message;
	{
		message.Category		= MP_RM_FARM_LOG;
		message.Protocol		= MP_RM_FARM_LOG_SIZE_ACK;
		message.mSize			= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick	= atoi( ( char* )record.Data[ 3 ] );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* beginDate	= ( char* )record.Data[ 1 ];
	const char* endDate		= ( char* )record.Data[ 2 ];

	g_DB.LogQuery(
		RGetFarmLog,
		connectionIndex,
		"EXEC dbo.TP_FARM_LOG_SELECT \'%s\', \'%s\', %d, 0",
		beginDate,
		endDate,
		message.mRequestTick );
}


// 080401 LUJ, 농장 로그
void RGetFarmLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_FARM_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category		= MP_RM_FARM_LOG;
		message.Protocol		= MP_RM_FARM_LOG_ACK;
		message.mSize			= dbMessage->dwResult;

		if( 0 < dbMessage->dwResult )
		{
			message.mRequestTick	= atoi( ( char* )query[ 0 ].Data[ 9 ] );
		}
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )	
	{
		const QUERYST&			record	= query [ i ];
		MSG_RM_FARM_LOG::Log&	log		= message.mLog[ i ];

		log.mIndex			= 				atoi( ( char* )record.Data[ 0 ] );
		log.mZone			= 				atoi( ( char* )record.Data[ 1 ] );
		log.mFarmIndex		=				atoi( ( char* )record.Data[ 2 ] );
		log.mKind			= eFamilyLog(	atoi( ( char* )record.Data[ 3 ] ) );
		log.mPlayerIndex	= 				atoi( ( char* )record.Data[ 4 ] );
		log.mValue1			= 				atoi( ( char* )record.Data[ 5 ] );
		log.mValue2			= 				atoi( ( char* )record.Data[ 6 ] );
		StringCopySafe( log.mDate,		( char* )record.Data[ 7 ], sizeof( log.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];

		const char* beginDate	=		( char* )record.Data[ 7 ];
		const char* endDate		=		( char* )record.Data[ 8 ];
		const DWORD	size		= atoi( ( char* )record.Data[ 10 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetFarmLog,
				connectionIndex,
				"EXEC dbo.TP_FARM_LOG_SELECT \'%s\', \'%s\', %d, %d",
				beginDate,
				endDate,
				message.mRequestTick,
				size );
		}	
	}
}


// 080630 LUJ, 농장 가축 로그
void RGetLivestockLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_LIVESTOCK_LOG;
		message.Protocol	= MP_RM_LIVESTOCK_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_LOG_SIZE message;
	{
		message.Category		= MP_RM_LIVESTOCK_LOG;
		message.Protocol		= MP_RM_LIVESTOCK_LOG_SIZE_ACK;
		message.mSize			= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick	= atoi( ( char* )record.Data[ 3 ] );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* beginDate	= ( char* )record.Data[ 1 ];
	const char* endDate		= ( char* )record.Data[ 2 ];
	const char*	farmIndex	= ( char* )record.Data[ 4 ];

	g_DB.LogQuery(
		RGetLivestockLog,
		connectionIndex,
		"EXEC dbo.TP_FARM_ANIMAL_LOG_SELECT \'%s\', \'%s\', %s, %d, 0",
		beginDate,
		endDate,
		farmIndex,
		message.mRequestTick );
}


// 080630 LUJ, 농장 가축 로그
void RGetLivestockLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_LIVESTOCK_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category		= MP_RM_LIVESTOCK_LOG;
		message.Protocol		= MP_RM_LIVESTOCK_LOG_ACK;
		message.mSize			= dbMessage->dwResult;

		if( 0 < dbMessage->dwResult )
		{
			message.mRequestTick	= atoi( ( char* )query[ 0 ].Data[ 13 ] );
		}
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )	
	{
		const QUERYST&					record	= query [ i ];
		MSG_RM_LIVESTOCK_LOG::Log&	log		= message.mLog[ i ];
		log.mIndex			= 				atoi( ( char* )record.Data[ 0 ] );
		log.mType			= eFamilyLog(	atoi( ( char* )record.Data[ 1 ] ) );
		log.mFarmIndex		=				atoi( ( char* )record.Data[ 2 ] );
		StringCopySafe( log.mMemo, ( char* )record.Data[ 3 ], sizeof( log.mMemo ) );
		StringCopySafe( log.mDate, ( char* )record.Data[ 4 ], sizeof( log.mDate ) );
		
		MSG_RM_LIVESTOCK_LOG::Log::Animal& animal = log.mAnimal;
		animal.mOwnerIndex	=				atoi( ( char* )record.Data[ 5 ] );
		animal.mIndex		=				atoi( ( char* )record.Data[ 6 ] );
		animal.mKind		=				atoi( ( char* )record.Data[ 7 ] );
		animal.mStep		=				atoi( ( char* )record.Data[ 8 ] );
		animal.mLife		=				atoi( ( char* )record.Data[ 9 ] );
		animal.mContentment	=				atoi( ( char* )record.Data[ 10 ] );
		animal.mInterest	=				atoi( ( char* )record.Data[ 11 ] );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];

		const char* beginDate	=		( char* )record.Data[ 4 ];
		const char* endDate		=		( char* )record.Data[ 12 ];
		const char* farmIndex	=		( char* )record.Data[ 14 ];
		const DWORD	size		= atoi( ( char* )record.Data[ 15 ] ) + MAX_ROW_NUM;		

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetLivestockLog,
				connectionIndex,
				"EXEC dbo.TP_FARM_ANIMAL_LOG_SELECT \'%s\', \'%s\', %s, %d, %d",
				beginDate,
				endDate,
				farmIndex,
				message.mRequestTick,
				size );
		}	
	}
}


// 080716 LUJ, 펫 로그
void RGetPetLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_PET_LOG;
		message.Protocol	= MP_RM_PET_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_LOG_SIZE message;	
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category		= MP_RM_PET_LOG;
		message.Protocol		= MP_RM_PET_LOG_SIZE_ACK;
		message.mSize			= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick	= atoi( ( char* )record.Data[ 1 ] );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}

	const char* beginDate	= ( char* )record.Data[ 2 ];
	const char* endDate		= ( char* )record.Data[ 3 ];
	const char*	userIndex	= ( char* )record.Data[ 4 ];
	const char* petIndex	= ( char* )record.Data[ 5 ];

	g_DB.LogQuery(
		RGetPetLog,
		connectionIndex,
		"EXEC dbo.TP_PET_LOG_SELECT \'%s\', \'%s\', %s, %s, %d, 0",
		beginDate,
		endDate,
		userIndex,
		petIndex,
		message.mRequestTick );
}


// 080716 LUJ, 펫 로그
void RGetPetLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_PET_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category		= MP_RM_PET_LOG;
		message.Protocol		= MP_RM_PET_LOG_ACK;
		message.mSize			= dbMessage->dwResult;

		if( 0 < dbMessage->dwResult )
		{
			message.mRequestTick	= atoi( ( char* )query[ 0 ].Data[ 16 ] );
		}
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )	
	{
		const QUERYST&			record	= query [ i ];
		MSG_RM_PET_LOG::Log&	log		= message.mLog[ i ];

		log.mIndex	=			atoi( ( char* )record.Data[ 0 ] );
		log.mType	= ePetLog(	atoi( ( char* )record.Data[ 1 ] ) );
		StringCopySafe( log.mDate, ( char* )record.Data[ 2 ], sizeof( log.mDate ) );
		StringCopySafe( log.mMemo, ( char* )record.Data[ 3 ], sizeof( log.mMemo ) );		
		
		MSG_RM_PET_LOG::Pet& pet = log.mPet;
		pet.mID			= 				atoi( ( char* )record.Data[ 4 ] );
		pet.mExperience	= 				atoi( ( char* )record.Data[ 5 ] );
		pet.mFriendship	=				atoi( ( char* )record.Data[ 6 ] );
		pet.mGrade		= 				atoi( ( char* )record.Data[ 7 ] );
		pet.mLevel		= 				atoi( ( char* )record.Data[ 8 ] );
		pet.mKind		= 				atoi( ( char* )record.Data[ 9 ] );
		pet.mState		= ePetState(	atoi( ( char* )record.Data[ 10 ] ) );
		pet.mSkillSlot	=				atoi( ( char* )record.Data[ 11 ] );		
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];

		const char* beginDate	=		( char* )record.Data[ 2 ];
		const char* endDate		=		( char* )record.Data[ 12 ];
		const char* userIndex	=		( char* )record.Data[ 13 ];
		const char* petIndex	=		( char* )record.Data[ 14 ];
		const DWORD	size		= atoi( ( char* )record.Data[ 15 ] ) + MAX_ROW_NUM;
		
		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetPetLog,
				connectionIndex,
				"EXEC dbo.TP_PET_LOG_SELECT \'%s\', \'%s\', %s, %s, %d, %d",
				beginDate,
				endDate,
				userIndex,
				petIndex,
				message.mRequestTick,
				size );
		}	
	}
}


// 080403 LUJ, 유료 아이템 로그 개수를 얻어온다
void RGetItemShopLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_ITEM_SHOP_LOG;
		message.Protocol	= MP_RM_ITEM_SHOP_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_LOG_SIZE message;
	{
		message.Category		= MP_RM_ITEM_SHOP_LOG;
		message.Protocol		= MP_RM_ITEM_SHOP_LOG_SIZE_ACK;
		message.mSize			= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick	= atoi( ( char* )record.Data[ 3 ] );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* beginDate	= ( char* )record.Data[ 1 ];
	const char* endDate		= ( char* )record.Data[ 2 ];
	const char*	userIndex	= ( char* )record.Data[ 4 ];

	g_DB.LogQuery(
		RGetItemShopLog,
		connectionIndex,
		"EXEC dbo.TP_ITEM_SHOP_LOG_SELECT \'%s\', \'%s\', %d, %s, 0",
		beginDate,
		endDate,
		message.mRequestTick,
		userIndex );
}


// 080401 LUJ, 유료 아이템 구입 로그를 가져온다
void RGetItemShopLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_ITEM_SHOP_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category		= MP_RM_ITEM_SHOP_LOG;
		message.Protocol		= MP_RM_ITEM_SHOP_LOG_ACK;
		message.mSize			= dbMessage->dwResult;

		if( 0 < dbMessage->dwResult )
		{
			message.mRequestTick	= atoi( ( char* )query[ 0 ].Data[ 9 ] );
		}
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )	
	{
		const QUERYST&				record	= query [ i ];
		MSG_RM_ITEM_SHOP_LOG::Log&	log		= message.mLog[ i ];

		log.mIndex			= 				atoi( ( char* )record.Data[ 0 ] );
		log.mType			= ItemShopLog(	atoi( ( char* )record.Data[ 1 ] ) );
		log.mUser.mIndex	=				atoi( ( char* )record.Data[ 2 ] );
		StringCopySafe( log.mUser.mId,	( char* )record.Data[ 3 ], sizeof( log.mUser.mId ) );
		log.mItem.mIndex	=				atoi( ( char* )record.Data[ 4 ] );
		log.mItem.mDbIndex	=				atoi( ( char* )record.Data[ 5 ] );
		log.mItem.mQuantity	=				atoi( ( char* )record.Data[ 6 ] );
		StringCopySafe( log.mDate,		( char* )record.Data[ 7 ], sizeof( log.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];

		const char* beginDate	=		( char* )record.Data[ 7 ];
		const char* endDate		=		( char* )record.Data[ 8 ];
		const char* userIndex	=		( char* )record.Data[ 10 ];
		const DWORD	size		= atoi( ( char* )record.Data[ 11 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetItemShopLog,
				connectionIndex,
				"EXEC dbo.TP_ITEM_SHOP_LOG_SELECT \'%s\', \'%s\', %d, %s, %d",
				beginDate,
				endDate,
				message.mRequestTick,
				userIndex,
				size );
		}		
	}
}


// 080523 LUJ, 낚시 로그
void RGetFishLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		message.Category	= MP_RM_FISH_LOG;
		message.Protocol	= MP_RM_FISH_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_LOG_SIZE message;
	{
		message.Category		= MP_RM_FISH_LOG;
		message.Protocol		= MP_RM_FISH_LOG_SIZE_ACK;
		message.mSize			= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick	= atoi( ( char* )record.Data[ 3 ] );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* beginDate	= ( char* )record.Data[ 1 ];
	const char* endDate		= ( char* )record.Data[ 2 ];
	const char*	playerIndex	= ( char* )record.Data[ 4 ];

	g_DB.LogQuery(
		RGetFishLog,
		connectionIndex,
		"EXEC dbo.TP_FISH_LOG_SELECT \'%s\', \'%s\', %d, %s, 0",
		beginDate,
		endDate,
		message.mRequestTick,
		playerIndex );
}


// 080523 LUJ, 낚시 로그
void RGetFishLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_FISH_LOG message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category		= MP_RM_FISH_LOG;
		message.Protocol		= MP_RM_FISH_LOG_ACK;
		message.mSize			= dbMessage->dwResult;

		if( 0 < dbMessage->dwResult )
		{
			message.mRequestTick	= atoi( ( char* )query[ 0 ].Data[ 9 ] );
		}
	}

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )	
	{
		const QUERYST&			record	= query [ i ];
		MSG_RM_FISH_LOG::Log&	log		= message.mLog[ i ];

		log.mIndex			= 				atoi( ( char* )record.Data[ 0 ] );
		log.mType			= eFishingLog(	atoi( ( char* )record.Data[ 1 ] ) );
		log.mPlayerIndex	=				atoi( ( char* )record.Data[ 2 ] );
		log.mVariant		=				atoi( ( char* )record.Data[ 3 ] ); 
		log.mPoint			=				atoi( ( char* )record.Data[ 4 ] ); 
		log.mBuyItemIndex	=				atoi( ( char* )record.Data[ 5 ] ); 
		log.mUseItemIndex	=				atoi( ( char* )record.Data[ 6 ] ); 
		StringCopySafe( log.mDate,	( char* )record.Data[ 7 ], sizeof( log.mDate ) );
		// 080808 LUJ, 경험치/레벨 추가. 경험치는 DB에 bigint로 저장되어 있어, unsigned int 값으로 바꿀 수 있는 strtoul()을 사용했다
		log.mExperience		= EXPTYPE(		atof( ( char* )record.Data[ 12 ] ) );
		log.mLevel			=				atoi( ( char* )record.Data[ 13 ] ); 
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];

		const char* beginDate	=		( char* )record.Data[ 7 ];
		const char* endDate		=		( char* )record.Data[ 8 ];
		const char* playerIndex	=		( char* )record.Data[ 10 ];
		const DWORD	size		= atoi( ( char* )record.Data[ 11 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetFishLog,
				connectionIndex,
				"EXEC dbo.TP_FISH_LOG_SELECT \'%s\', \'%s\', %d, %s, %d",
				beginDate,
				endDate,
				message.mRequestTick,
				playerIndex,
				size );
		}
	}
}


// 080526 LUJ, 낚시 정보를 가져온다
void RGetFishingData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_DWORD3 message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_PLAYER;
	message.Protocol	= MP_RM_PLAYER_GET_FISHING_ACK;
	
	if( 1 == dbMessage->dwResult )
	{
		const MIDDLEQUERYST& record = query[ 0 ];

		message.dwObjectID	= atoi( ( char* )record.Data[ 0 ] );
		message.dwData1		= atoi( ( char* )record.Data[ 1 ] );// 레벨
		message.dwData2		= atoi( ( char* )record.Data[ 2 ] );// 경험치
		message.dwData3		= atoi( ( char* )record.Data[ 3 ] );// 포인트
	}	

	NETWORK->Send( connectionIndex, message, sizeof( message ) );
}

void RGetCookData(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_DWORD4 message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_PLAYER;
	message.Protocol = MP_RM_PLAYER_GET_COOK_ACK;

	if(1 == dbMessage->dwResult)
	{
		const MIDDLEQUERYST& record = query[0];
		message.dwData1 = _ttoi(LPCTSTR(record.Data[1]));
		message.dwData2 = _ttoi(LPCTSTR(record.Data[2]));;
		message.dwData3 = _ttoi(LPCTSTR(record.Data[3]));;
		message.dwData4 = _ttoi(LPCTSTR(record.Data[4]));;
	}

	NETWORK->Send(
		connectionIndex,
		message,
		sizeof(message));
}

void RGetHouseData(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_HOUSE message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_PLAYER;
	message.Protocol = MP_RM_PLAYER_GET_HOUSE_ACK;

	if(1 == dbMessage->dwResult)
	{
		const MIDDLEQUERYST& record = query[0];
		StringCopySafe(
			message.mHouseName,
			(char*)record.Data[0],
			_countof(message.mHouseName));
		message.mExteriorKind = _ttoi(
			(char*)record.Data[1]);
		message.mExtendLevel = _ttoi(
			(char*)record.Data[2]);
		message.mTotalVisitCount = _ttoi(
			(char*)record.Data[3]);
		message.mDailyVisitCount = _ttoi(
			(char*)record.Data[4]);
		message.mDecorationPoint = _ttoi(
			(char*)record.Data[5]);
		message.mHousePoint = _ttoi(
			(char*)record.Data[6]);
	}

	NETWORK->Send(
		connectionIndex,
		message,
		sizeof(message));
}

void RGetCookRecipeData(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_COOK_RECIPE message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_PLAYER;
	message.Protocol = MP_RM_PLAYER_GET_COOK_RECIPE_ACK;
	message.mSize = dbMessage->dwResult;

	for(DWORD i = 0; i < dbMessage->dwResult; ++i)
	{
		const MIDDLEQUERYST& record = query[i];

		MSG_RM_COOK_RECIPE::Recipe& recipe = message.mRecipe[i];
		recipe.mIndex = _ttoi(LPCTSTR(record.Data[1]));
		recipe.mSlot = _ttoi(LPCTSTR(record.Data[2]));
		recipe.mRemainTime = _ttoi(LPCTSTR(record.Data[3]));
	}

	NETWORK->Send(
		connectionIndex,
		message,
		sizeof(message));
}

void RGetPetData( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_PET message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_ITEM;
	message.Protocol	= MP_RM_ITEM_GET_PET_ACK;
	message.mSize		= dbMessage->dwResult;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const MIDDLEQUERYST&	record	= query[ i ];
		MSG_RM_PET::Pet&		pet		= message.mData[ i ];

		pet.mIndex						= 				atoi( ( char* )record.Data[ 0 ] );
		pet.mItemDbIndex				= 				atoi( ( char* )record.Data[ 1 ] );
		pet.mKind						= 				atoi( ( char* )record.Data[ 2 ] );
		pet.mLevel						= 				atoi( ( char* )record.Data[ 3 ] );
		pet.mGrade						= 				atoi( ( char* )record.Data[ 4 ] );
		pet.mSkillSlot					=				atoi( ( char* )record.Data[ 5 ] );
		pet.mExperience					=				atoi( ( char* )record.Data[ 6 ] );
		pet.mFriendship					=				atoi( ( char* )record.Data[ 7 ] );
		pet.mHealth						=				atoi( ( char* )record.Data[ 8 ] );
		pet.mMaxHealth					=				atoi( ( char* )record.Data[ 9 ] );
		pet.mMana						=				atoi( ( char* )record.Data[ 10 ] );
		pet.mMaxMana					= 				atoi( ( char* )record.Data[ 11 ] );
		pet.mArtificialIntelligence		=				atoi( ( char* )record.Data[ 12 ] );
		pet.mState						= ePetState(	atoi( ( char* )record.Data[ 13 ] ) );
		pet.mType						= ePetType(		atoi( ( char* )record.Data[ 14 ] ) );

		// 080730 LUJ, 펫이 가진 아이템을 읽어온다
		GetItemListOnPet( connectionIndex, pet.mIndex, 0 );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_MIDDLEROW_NUM == dbMessage->dwResult )
	{
		const MIDDLEQUERYST& record	= query[ MAX_MIDDLEROW_NUM - 1 ];

		const DWORD userIndex		= atoi( ( char* )record.Data[ 15 ] );
		const DWORD lastPetIndex	= atoi( ( char* )record.Data[ 0 ] );

		// 081118 LUJ, 펫 목록을 추가로 쿼리할 때, 잘못된 쿼리 문을 사용하던 문제 수정
		g_DB.FreeMiddleQuery(
			RGetPetData,
			connectionIndex,
			"SELECT TOP %d PET_IDX, ITEM_DBIDX, PET_KIND, PET_LEVEL, PET_GRADE, SKILL_SLOT, PET_EXP,\
			PET_FRIENDLY, PET_HP, PET_MAXHP, PET_MP, PET_MAXMP,PET_AI, PET_STATE, PET_TYPE, %d\
			FROM TB_PET WHERE MASTER_IDX = %d AND PET_IDX > %d ORDER BY PET_IDX",
			MAX_MIDDLEROW_NUM,
			userIndex,
			userIndex,
			lastPetIndex );
	}
}


// 080716 LUJ, 전역 아이템 로그
void RGetItemGlobalLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_ITEM_GLOBAL_LOG message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category		= MP_RM_ITEM_GLOBAL_LOG;
	message.Protocol		= MP_RM_ITEM_GLOBAL_LOG_ACK;
	message.mSize			= dbMessage->dwResult;
	message.mRequestTick	= atoi( ( char* )query[ 0 ].Data[ 21 ] );

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )	
	{
		const QUERYST& record = query[ i ];

		MSG_RM_ITEM_GLOBAL_LOG::Log& log = message.mLog[ i ];
		log.mLogIndex	=					atoi( ( char* )record.Data[ 0 ] );
		log.mType		= eLogitemmoney(	atoi( ( char* )record.Data[ 1 ] ) );
		log.mKey		=					atoi( ( char* )record.Data[ 22 ] );

		{
			MSG_RM_ITEM_GLOBAL_LOG::Player& player = log.mFromPlayer;
			player.mIndex	= atoi( ( char* )record.Data[ 2 ] );
			StringCopySafe( player.mName, ( char* )record.Data[ 3 ], sizeof( player.mName ) );
			player.mMoney	= atoi( ( char* )record.Data[ 4 ] );
		}

		{
			MSG_RM_ITEM_GLOBAL_LOG::Player& player = log.mToPlayer;
			player.mIndex	= atoi( ( char* )record.Data[ 5 ] );
			StringCopySafe( player.mName, ( char* )record.Data[ 6 ], sizeof( player.mName ) );
			player.mMoney	= atoi( ( char* )record.Data[ 7 ] );
		}

		log.mMoneyVariation	= atoi( ( char* )record.Data[ 8 ] );

		{
			MSG_RM_ITEM_GLOBAL_LOG::Item& item = log.mItem;
			item.mIndex			= atoi( ( char* )record.Data[ 9 ] );
			item.mDbIndex		= atoi( ( char* )record.Data[ 10 ] );
			item.mQuantity		= atoi( ( char* )record.Data[ 11 ] );
			item.mFromPosition	= atoi( ( char* )record.Data[ 12 ] );
			item.mToPosition	= atoi( ( char* )record.Data[ 13 ] );
		}

		StringCopySafe( log.mDate, ( char* )record.Data[ 14 ], sizeof( log.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];

		const char* requestTick	=		( char* )record.Data[ 21 ];
		const char* beginDate	=		( char* )record.Data[ 14 ];
		const char* endDate		=		( char* )record.Data[ 15 ];
		const char* itemIndex	=		( char* )record.Data[ 16 ];
		const char* itemDbIndex	=		( char* )record.Data[ 17 ];
		const char* playerIndex	=		( char* )record.Data[ 18 ];
		const char* logType		=		( char* )record.Data[ 19 ];
		const DWORD	size		= atoi( ( char* )record.Data[ 20 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetItemGlobalLog,
				connectionIndex,
				"EXEC dbo.TP_ITEM_GLOBAL_LOG_SELECT \'%s\', \'%s\', %s, %s, %s, %s, %s, %d",
				beginDate,
				endDate,
				requestTick,
				playerIndex,
				logType,
				itemIndex,
				itemDbIndex,
				size );
		}
	}
}


// 080731 LUJ, 펫을 추가한다
void RAddPet( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( 1 != dbMessage->dwResult ||
		! stricmp( "0", ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_ITEM;
		message.Protocol	= MP_RM_ITEM_ADD_PET_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}
	else
	{
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_ITEM;
		message.Protocol	= MP_RM_ITEM_ADD_PET_ACK;
		// 080731 LUJ, 아이템 DB 인덱스
		message.dwObjectID	= atoi( ( char* )query[ 0 ].Data[ 1 ] );

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}

    const MIDDLEQUERYST& record = query[ 0 ];

	MSG_RM_PET message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_ITEM;
	message.Protocol	= MP_RM_ITEM_GET_PET_ACK;
	message.mSize		= dbMessage->dwResult;

	{
		const MIDDLEQUERYST&	record	= query[ 0 ];
		MSG_RM_PET::Pet&		pet		= message.mData[ 0 ];

		pet.mIndex						= 				atoi( ( char* )record.Data[ 0 ] );
		pet.mItemDbIndex				= 				atoi( ( char* )record.Data[ 1 ] );
		pet.mKind						= 				atoi( ( char* )record.Data[ 2 ] );
		pet.mLevel						= 				atoi( ( char* )record.Data[ 3 ] );
		pet.mGrade						= 				atoi( ( char* )record.Data[ 4 ] );
		pet.mSkillSlot					=				atoi( ( char* )record.Data[ 5 ] );
		pet.mExperience					=				atoi( ( char* )record.Data[ 6 ] );
		pet.mFriendship					=				atoi( ( char* )record.Data[ 7 ] );
		pet.mHealth						=				atoi( ( char* )record.Data[ 8 ] );
		pet.mMaxHealth					=				atoi( ( char* )record.Data[ 9 ] );
		pet.mMana						=				atoi( ( char* )record.Data[ 10 ] );
		pet.mMaxMana					= 				atoi( ( char* )record.Data[ 11 ] );
		pet.mArtificialIntelligence		=				atoi( ( char* )record.Data[ 12 ] );
		pet.mState						= ePetState(	atoi( ( char* )record.Data[ 13 ] ) );
		pet.mType						= ePetType(		atoi( ( char* )record.Data[ 14 ] ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );
}


// 080731 LUJ, 버프 스킬 정보를 획득한다
void GetBuffList( DWORD connectionIndex, DWORD playerIndex, DWORD lastSkillIndex )
{
	g_DB.FreeMiddleQuery(
		RGetBuffList,
		connectionIndex,
		"SELECT TOP %d CHARACTER_IDX, SKILL_IDX, REMAIN_TIME / 1000, REMAIN_COUNT, ISNULL( '', CONVERT( VARCHAR( 6 ), END_DATE, 12 ) + ' ' + CONVERT( VARCHAR( 8 ), END_DATE, 8 ) ) \
		FROM TB_BUFF WHERE CHARACTER_IDX = %d AND SKILL_IDX > %d ORDER BY SKILL_IDX",
		MAX_MIDDLEROW_NUM,
		playerIndex,
		lastSkillIndex );
}


// 080731 LUJ, 버프 스킬 정보를 획득한다
void RGetBuffList( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_SKILL_BUFF message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_SKILL;
	message.Protocol	= MP_RM_SKILL_BUFF_ACK;
	message.mSize		= dbMessage->dwResult;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const MIDDLEQUERYST&		record	= query[ i ];
		MSG_RM_SKILL_BUFF::Buff&	buff	= message.mBuff[ i ];

		buff.mSkillIndex	= atoi( ( char* )record.Data[ 1 ] );
		buff.mRemainTime	= atoi( ( char* )record.Data[ 2 ] );
		buff.mRemainCount	= atoi( ( char* )record.Data[ 3 ] );
		StringCopySafe( buff.mEndDate, ( char* )record.Data[ 4 ], sizeof( buff.mEndDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_MIDDLEROW_NUM == dbMessage->dwResult )
	{
		const MIDDLEQUERYST&	record		= query[ MAX_MIDDLEROW_NUM - 1 ];
		const DWORD				playerIndex = atoi( ( char* )record.Data[ 0 ] );
		const DWORD				skillIndex	= atoi( ( char* )record.Data[ 1 ] );

        GetBuffList(
			connectionIndex,
			playerIndex,
			skillIndex );
	}
}

void GetAuthorityToServer( eGM_POWER power )
{
	char text[ MAX_PATH ] = { 0 };
	// 081021 LUJ, 두 값을 조합하여 고유한 키로 만든다
	sprintf(
		text,
		"SELECT TOP 100 POWER, TYPE, ATTRIBUTE FROM dbo.TB_OPERATOR_AUTHORITY WHERE POWER = %d",
		power );

	g_DB.LoginQuery(
		RGetAuthorityToServer,
		power,
		text );
}

void GetAuthorityToUser( DWORD connectionIndex, eGM_POWER power, AuthorityType type )
{
	char text[ MAX_PATH ] = { 0 };
	// 081224 LUJ, 프로시저로 변경
	sprintf(
		text,
		"EXEC dbo.TP_OPERATOR_AUTHORITY_SELECT %d",
		int( type ) * 100 + int( power ) );

	g_DB.LoginQuery(
		RGetAuthorityToUser,
		connectionIndex,
		text );
}

// 081021 LUJ, 권한 정보를 가져온다
void RGetAuthorityToServer( LPQUERY query, LPDBMESSAGE dbMessage )
{
	CUserManager::AuthorityList authorityList;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST& record = query[ i ];

		const eGM_POWER		power		= eGM_POWER( atoi( ( char* )record.Data[ 0 ] ) );
		const AuthorityType	type		= AuthorityType( atoi(	( char* )record.Data[ 1 ] ) );
		const char*			attribute	= ( char* )record.Data[ 2 ];

		MSG_RM_AUTHORITY::Authority authority;
		ZeroMemory( &authority, sizeof( authority ) );
		authority.mPower		= power;
		authority.mType			= type;
		authority.mWriteable	= ( 0 < strchr( attribute, 'w' ) );
		authority.mReadable		= ( 0 < strchr( attribute, 'r' ) );
		authorityList.push_back( authority );
	}

	const eGM_POWER power = eGM_POWER( dbMessage->dwID );

	CUserManager::GetInstance().SetAuthority( power, authorityList );
}

// 081021 LUJ, 권한 정보를 가져온다
void RGetAuthorityToUser( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_AUTHORITY message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_AUTHORITY;
	message.Protocol	= MP_RM_AUTHORITY_GET_ACK;
	message.mSize		= dbMessage->dwResult;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST& record = query[ i ];

		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ i ];
		authority.mPower		= eGM_POWER( atoi(		( char* )record.Data[ 0 ] ) );
		authority.mType			= AuthorityType( atoi(	( char* )record.Data[ 1 ] ) );
		authority.mReadable		= ( 0 < strchr( ( char* )record.Data[ 2 ], 'r' ) );
		authority.mWriteable	= ( 0 < strchr( ( char* )record.Data[ 2 ], 'w' ) );
	}

	NETWORK->Send(
		connectionIndex,
		message,
		message.GetSize() );
}

// 081027 LUJ, 토너먼트 정보를 가져온다
void RGetTournamentCount( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	MSG_DWORD message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_TOURNAMENT;
	message.Protocol	= MP_RM_TOURNAMENT_GET_ROUND_ACK;
	
	if( 1 == dbMessage->dwResult )
	{
		const MIDDLEQUERYST&	record		= query[ 0 ];
		const DWORD				maxRound	= atoi( ( char* )record.Data[ 0 ] );

		message.dwData = maxRound;
	}

	const DWORD connectionIndex = dbMessage->dwID;

	NETWORK->Send(
		connectionIndex,
		message,
		sizeof( message ) );
}

// 081027 LUJ, 특정 회차에 정보를 가져온다
void GetTournamentGuild( DWORD connectionIndex, DWORD count, DWORD lastGuildIndex )
{
	g_DB.FreeQuery(
		RGetTournamentGuild,
		connectionIndex,
		"SELECT TOP 100 T.GTCOUNT, T.GUILD_IDX, T.GUILD_NAME, T.GTPOS, T.GTROUND,\
		ISNULL( G.PLAYER0, 0 ), ISNULL( G.PLAYER1, 0 ), ISNULL( G.PLAYER2, 0 ), ISNULL( G.PLAYER3, 0 ), ISNULL( G.PLAYER4, 0 ),\
		ISNULL( G.PLAYER5, 0 ), ISNULL( G.PLAYER6, 0 ), ISNULL( G.PLAYER7, 0 ), ISNULL( G.PLAYER8, 0 ), ISNULL( G.PLAYER9, 0 ),\
		ISNULL( G.PLAYER10, 0 ), ISNULL( G.PLAYER11, 0 ), ISNULL( G.PLAYER12, 0 ), ISNULL( G.PLAYER13, 0 )\
		FROM TB_GT_INFO T LEFT JOIN TB_GT_PLAYER G ON T.GTCOUNT = G.GTCOUNT AND T.GUILD_IDX = G.GUILD_IDX\
		WHERE T.GTCOUNT = %d AND T.GUILD_IDX >= %d ORDER BY T.GUILD_IDX",
		count,
		lastGuildIndex );
}

// 081027 LUJ, 특정 회차에 참가한 길드 정보를 가져온다
void RGetTournamentGuild( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_TOURNAMENT_GUILD	message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_TOURNAMENT;
	message.Protocol	= MP_RM_TOURNAMENT_GET_DATA_GUILD_ACK;

	ASSERT( sizeof( message.mGuild ) / sizeof( *message.mGuild ) >= dbMessage->dwResult );

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST& record = query[ i ];

		message.mTournamentCount = atoi( ( char* )record.Data[ 0 ] );

		MSG_RM_TOURNAMENT_GUILD::Guild& guild = message.mGuild[ message.mSize++ ];
		guild.mIndex	= atoi( ( char* )record.Data[ 1 ] );
		StringCopySafe( guild.mName, ( char* )record.Data[ 2 ], sizeof( guild.mName ) );
		guild.mPosition	= atoi( ( char* )record.Data[ 3 ] );
		guild.mRound	= atoi( ( char* )record.Data[ 4 ] );

		const char* player0		= ( char* )record.Data[ 5 ];
		const char* player1		= ( char* )record.Data[ 6 ];
		const char* player2		= ( char* )record.Data[ 7 ];
		const char* player3		= ( char* )record.Data[ 8 ];
		const char* player4		= ( char* )record.Data[ 9 ];
		const char* player5		= ( char* )record.Data[ 10 ];
		const char* player6		= ( char* )record.Data[ 11 ];
		const char* player7		= ( char* )record.Data[ 12 ];
		const char* player8		= ( char* )record.Data[ 13 ];
		const char* player9		= ( char* )record.Data[ 14 ];
		const char* player10	= ( char* )record.Data[ 15 ];
		const char* player11	= ( char* )record.Data[ 16 ];
		const char* player12	= ( char* )record.Data[ 17 ];
		const char* player13	= ( char* )record.Data[ 18 ];

		// 081027 LUJ, 플레이어 정보를 쿼리한다
		g_DB.FreeMiddleQuery(
			RGetTournamentPlayer,
			connectionIndex,
			"SELECT TOP 100 %d, %d, CHARACTER_IDX, CHARACTER_NAME FROM TB_CHARACTER WITH(NOLOCK) WHERE\
			CHARACTER_IDX = %s OR CHARACTER_IDX = %s OR CHARACTER_IDX = %s OR CHARACTER_IDX = %s OR CHARACTER_IDX = %s OR CHARACTER_IDX = %s OR\
			CHARACTER_IDX = %s OR CHARACTER_IDX = %s OR CHARACTER_IDX = %s OR CHARACTER_IDX = %s OR CHARACTER_IDX = %s OR CHARACTER_IDX = %s OR\
			CHARACTER_IDX = %s OR CHARACTER_IDX = %s",
			message.mTournamentCount,
			guild.mIndex,
			player0		? player0 : "0",
			player1		? player1 : "0",
			player2		? player2 : "0",
			player3		? player3 : "0",
			player4		? player4 : "0",
			player5		? player5 : "0",
			player6		? player6 : "0",
			player7		? player7 : "0",
			player8		? player8 : "0",
			player9		? player9 : "0",
			player10	? player10 : "0",
			player11	? player11 : "0",
			player12	? player12 : "0",
			player13	? player13 : "0" );
	}

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record			= query[ MAX_ROW_NUM - 1 ];
		const DWORD		lastGuildIndex	= atoi( ( char* )record.Data[ 1 ] );

		GetTournamentGuild(
			connectionIndex,
			message.mTournamentCount,
			lastGuildIndex );
	}

	NETWORK->Send(
		connectionIndex,
		message,
		message.GetSize() );
}

// 081027 LUJ, 플레이어 정보를 쿼리한다
void RGetTournamentPlayer( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	MSG_RM_TOURNAMENT_PLAYER message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_TOURNAMENT;
	message.Protocol	= MP_RM_TOURNAMENT_GET_DATA_PLAYER_ACK;

	ASSERT( sizeof( message.mPlayer ) / sizeof( *message.mPlayer ) >= dbMessage->dwResult );
	
	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const MIDDLEQUERYST& record = query[ i ];

		message.mTournamentCount	= atoi( ( char* )record.Data[ 0 ] );
		message.mGuildIndex			= atoi( ( char* )record.Data[ 1 ] );
		
		MSG_RM_TOURNAMENT_PLAYER::Player& player = message.mPlayer[ message.mSize++ ];
		player.mIndex	= atoi( ( char* )record.Data[ 2 ] );
		StringCopySafe( player.mName, ( char* )record.Data[ 3 ], sizeof( player.mName ) );
	}

	const DWORD connectionIndex = dbMessage->dwID;

	NETWORK->Send(
		connectionIndex,
		message,
		message.GetSize() );
}

// 081027 LUJ, 怨듭꽦 濡쒓렇
void RGetSiegeLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_SIEGE_LOG;
		message.Protocol	= MP_RM_SIEGE_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST&	record		= query[ 0 ];
	const DWORD				requestTick	= atoi( ( char* )record.Data[ 1 ] );

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category		= MP_RM_SIEGE_LOG;
		message.Protocol		= MP_RM_SIEGE_LOG_SIZE_ACK;
		message.mSize			= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick	= requestTick;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* beginDate	= ( char* )record.Data[ 2 ];
	const char* endDate		= ( char* )record.Data[ 3 ];
	
	g_DB.LogQuery(
		RGetSiegeLog,
		connectionIndex,
		"EXEC dbo.TP_SIEGE_LOG_SELECT \'%s\', \'%s\', %d, 0",
		beginDate,
		endDate,
		requestTick );
}

// 081027 LUJ, 怨듭꽦 濡쒓렇
void RGetSiegeLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_SIEGE_LOG message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category		= MP_RM_SIEGE_LOG;
	message.Protocol		= MP_RM_SIEGE_LOG_ACK;
	message.mSize			= dbMessage->dwResult;
	message.mRequestTick	= ( 0 < dbMessage->dwResult ) ? atoi( ( char* )query[ 0 ].Data[ 16 ] ) : 0;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&			record	= query [ i ];
		MSG_RM_SIEGE_LOG::Log&	log		= message.mLog[ i ];

		log.mIndex			=				atoi( ( char* )record.Data[ 0 ] );
		log.mType			= eSiegeLog(	atoi( ( char* )record.Data[ 1 ] ) );
		log.mGuildIndex		=				atoi( ( char* )record.Data[ 2 ] );
		log.mPlayerIndex	=				atoi( ( char* )record.Data[ 3 ] );
		StringCopySafe( log.mMemo, ( char* )record.Data[ 4 ], sizeof( log.mMemo ) );
		StringCopySafe( log.mDate, ( char* )record.Data[ 5 ], sizeof( log.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];

		const char* beginDate	=		( char* )record.Data[ 5 ];
		const char* endDate		=		( char* )record.Data[ 6 ];
		const DWORD	size		= atoi( ( char* )record.Data[ 7 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetSiegeLog,
				connectionIndex,
				"EXEC dbo.TP_SIEGE_LOG_SELECT \'%s\', \'%s\', %d, %d",
				beginDate,
				endDate,
				message.mRequestTick,
				size );
		}
	}
}

// 081127 LUJ, 버전 정보를 갱신한다. 동기화를 위해 DB에서 처리한다
void IncreaseAuthorityVersion( DWORD connectionIndex, eGM_POWER power, AuthorityType type )
{
	g_DB.LoginMiddleQuery(
		RIncreaseAuthorityVersion,
		connectionIndex,
		"EXEC dbo.TP_OPERATOR_AUTHORITY_VERSION_INCREASE %d, %d",
		power,
		type );
}

// 081127 LUJ, 버전 정보를 갱신한다. 동기화를 위해 DB에서 처리한다
void RIncreaseAuthorityVersion( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex	= dbMessage->dwID;

	if( 1 != dbMessage->dwResult )
	{
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_AUTHORITY;
		message.Protocol	= MP_RM_AUTHORITY_SET_NACK;

		NETWORK->Send(
			connectionIndex,
			message,
			sizeof( message	) );
		return;
	}

	const MIDDLEQUERYST&	record	= query[ 0 ];
	const DWORD				version	= atoi( ( char* )record.Data[ 0 ] );

	{
		MSG_DWORD message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_AUTHORITY;
		message.Protocol	= MP_RM_AUTHORITY_SET_ACK;
		message.dwData		= version;

		NETWORK->Send(
			connectionIndex,
			message,
			sizeof( message	) );
	}
}

// 08205 LUJ, 梨꾪똿 濡쒓렇
void RGetChatLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_CHAT_LOG;
		message.Protocol	= MP_RM_CHAT_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST&	record		= query[ 0 ];
	const DWORD				requestTick	= atoi( ( char* )record.Data[ 1 ] );

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category		= MP_RM_CHAT_LOG;
		message.Protocol		= MP_RM_CHAT_LOG_SIZE_ACK;
		message.mSize			= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick	= requestTick;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* beginDate	= ( char* )record.Data[ 2 ];
	const char* endDate		= ( char* )record.Data[ 3 ];

	g_DB.LogMiddleQuery(
		RGetChatLog,
		connectionIndex,
		"EXEC dbo.TP_CHAT_LOG_SELECT \'%s\', \'%s\', %d, 0",
		beginDate,
		endDate,
		requestTick );
}

// 08205 LUJ, 梨꾪똿 濡쒓렇
void RGetChatLog( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_CHAT_LOG message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category		= MP_RM_CHAT_LOG;
	message.Protocol		= MP_RM_CHAT_LOG_ACK;
	message.mSize			= dbMessage->dwResult;
	message.mRequestTick	= ( 0 < dbMessage->dwResult ) ? atoi( ( char* )query[ 0 ].Data[ 6 ] ) : 0;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const MIDDLEQUERYST&	record	= query [ i ];
		MSG_RM_CHAT_LOG::Log&	log		= message.mLog[ i ];
		log.mIndex			= atoi( ( char* )record.Data[ 0 ] );
		log.mPlayerIndex	= atoi( ( char* )record.Data[ 1 ] );
		StringCopySafe( log.mChat, ( char* )record.Data[ 2 ], sizeof( log.mChat ) );
		StringCopySafe( log.mDate, ( char* )record.Data[ 3 ], sizeof( log.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_MIDDLEROW_NUM == dbMessage->dwResult )
	{
		const MIDDLEQUERYST&	record		= query[ MAX_MIDDLEROW_NUM - 1 ];
		const char*				beginDate	=		( char* )record.Data[ 3 ];
		const char*				endDate		=		( char* )record.Data[ 4 ];
		const DWORD				size		= atoi( ( char* )record.Data[ 5 ] ) + MAX_MIDDLEROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogMiddleQuery(
				RGetChatLog,
				connectionIndex,
				"EXEC dbo.TP_CHAT_LOG_SELECT \'%s\', \'%s\', %d, %d",
				beginDate,
				endDate,
				message.mRequestTick,
				size );
		}
	}
}

// 081205 LUJ, 패밀리 로그
void RGetFamilyLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_FAMILY_LOG;
		message.Protocol	= MP_RM_FAMILY_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST&	record		= query[ 0 ];
	const DWORD				requestTick	= atoi( ( char* )record.Data[ 1 ] );

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category		= MP_RM_FAMILY_LOG;
		message.Protocol		= MP_RM_FAMILY_LOG_SIZE_ACK;
		message.mSize			= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick	= requestTick;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* beginDate	= ( char* )record.Data[ 2 ];
	const char* endDate		= ( char* )record.Data[ 3 ];
	const char*	familyIndex	= ( char* )record.Data[ 4 ];
	const char* playerIndex	= ( char* )record.Data[ 5 ];

	g_DB.LogQuery(
		RGetFamilyLog,
		connectionIndex,
		"EXEC dbo.TP_FAMILY_LOG_SELECT \'%s\', \'%s\', %d, 0, %s, %s",
		beginDate,
		endDate,
		requestTick,
		familyIndex,
		playerIndex );
}

// 081205 LUJ, 패밀리 로그
void RGetFamilyLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_FAMILY_LOG message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category		= MP_RM_FAMILY_LOG;
	message.Protocol		= MP_RM_FAMILY_LOG_ACK;
	message.mSize			= dbMessage->dwResult;
	message.mRequestTick	= ( 0 < dbMessage->dwResult ) ? atoi( ( char* )query[ 0 ].Data[ 0 ] ) : 0;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&			record	= query [ i ];
		MSG_RM_FAMILY_LOG::Log&	log		= message.mLog[ i ];

		log.mIndex			=				atoi( ( char* )record.Data[ 1 ] );
		log.mType			= eFamilyLog(	atoi( ( char* )record.Data[ 2 ] ) );
		log.mFamilyIndex	=				atoi( ( char* )record.Data[ 3 ] );
		log.mPlayerIndex	=				atoi( ( char* )record.Data[ 4 ] );
		StringCopySafe( log.mDate, ( char* )record.Data[ 5 ], sizeof( log.mDate ) );
		StringCopySafe( log.mMemo, ( char* )record.Data[ 6 ], sizeof( log.mMemo ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const char*		beginDate	=		( char* )record.Data[ 5 ];
		const char*		endDate		=		( char* )record.Data[ 7 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 8 ] ) + MAX_ROW_NUM;
		const char*		familyIndex	=		( char* )record.Data[ 9 ];
		const char*		playerIndex =		( char* )record.Data[ 10 ];

		if( MAX_LOG_SIZE > size )
		{
			g_DB.LogQuery(
				RGetFamilyLog,
				connectionIndex,
				"EXEC dbo.TP_FAMILY_LOG_SELECT \'%s\', \'%s\', %d, %d, %s, %s",
				beginDate,
				endDate,
				message.mRequestTick,
				size,
				familyIndex,
				playerIndex );
		}
	}
}

// 090122 LUJ, 스크립트 변조 로그 개수를 가져온다
void RGetScriptHackLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_SCRIPT_HACK_LOG;
		message.Protocol	= MP_RM_SCRIPT_HACK_LOG_SIZE_NACK;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST&	record		= query[ 0 ];
	const DWORD				requestTick	= atoi( ( char* )record.Data[ 1 ] );

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category		= MP_RM_SCRIPT_HACK_LOG;
		message.Protocol		= MP_RM_SCRIPT_HACK_LOG_SIZE_ACK;
		message.mSize			= min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick	= requestTick;

		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* beginDate	= ( char* )record.Data[ 2 ];
	const char* endDate		= ( char* )record.Data[ 3 ];

	GetScriptHackLog(
		connectionIndex,
		beginDate,
		endDate,
		requestTick,
		0 );
}

// 090122 LUJ, 스크립트 변조 로그 쿼리를 요청한다
void GetScriptHackLog( DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD requestTick, DWORD queriedSize )
{
	g_DB.LogQuery(
		RGetScriptHackLog,
		connectionIndex,
		"EXEC dbo.TP_SCRIPT_HACK_LOG_SELECT \'%s\', \'%s\', %d, %d",
		beginDate,
		endDate,
		requestTick,
		queriedSize );
}

// 090122 LUJ, 스크립트 변조 로그를 쿼리한다
void RGetScriptHackLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_SCRIPT_HACK_LOG message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category		= MP_RM_SCRIPT_HACK_LOG;
	message.Protocol		= MP_RM_SCRIPT_HACK_LOG_ACK;
	message.mSize			= dbMessage->dwResult;
	message.mRequestTick	= ( 0 < dbMessage->dwResult ) ? atoi( ( char* )query[ 0 ].Data[ 0 ] ) : 0;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST&					record	= query [ i ];
		MSG_RM_SCRIPT_HACK_LOG::Log&	log		= message.mLog[ i ];

		log.mIndex			= atoi( ( char* )record.Data[ 1 ] );
		log.mUserIndex		= atoi( ( char* )record.Data[ 2 ] );
		log.mPlayerIndex	= atoi( ( char* )record.Data[ 3 ] );
		StringCopySafe( log.mScriptName,	( char* )record.Data[ 4 ], sizeof( log.mScriptName ) );
		StringCopySafe( log.mMemo,			( char* )record.Data[ 5 ], sizeof( log.mMemo ) );
		StringCopySafe( log.mDate,			( char* )record.Data[ 6 ], sizeof( log.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	// 090122 LUJ, 최대로 쿼리한 경우에는 다음 페이지를 쿼리 요청한다
	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST&	record		= query[ MAX_ROW_NUM - 1 ];
		const char*		beginDate	=		( char* )record.Data[ 6 ];
		const char*		endDate		=		( char* )record.Data[ 7 ];
		const DWORD		size		= atoi( ( char* )record.Data[ 8 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			GetScriptHackLog(
				connectionIndex,
				beginDate,
				endDate,
				message.mRequestTick,
				size );
		}
	}
}

// 090406 LUJ, 돈 로그 개수를 가져온다
void RGetMoneyLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult ||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_MONEY_LOG;
		message.Protocol = MP_RM_MONEY_LOG_SIZE_NACK;
		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record	= query[ 0 ];
	const DWORD	requestTick	= atoi( ( char* )record.Data[ 1 ] );

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_MONEY_LOG;
		message.Protocol = MP_RM_MONEY_LOG_SIZE_ACK;
		message.mSize = min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick = requestTick;
		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* const beginDate = ( char* )record.Data[ 2 ];
	const char* const endDate = ( char* )record.Data[ 3 ];
	const char* const playerIndex = ( char* )record.Data[ 4 ];

	GetMoneyLog(
		connectionIndex,
		beginDate,
		endDate,
		atoi( playerIndex ),
		requestTick,
		0 );
}

// 090406 LUJ, 돈 로그 쿼리를 요청한다
void GetMoneyLog( DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize )
{
	g_DB.LogQuery(
		RGetMoneyLog,
		connectionIndex,
		"EXEC dbo.TP_MONEY_LOG_SELECT \'%s\', \'%s\', %d, %d, %d",
		beginDate,
		endDate,
		playerIndex,
		requestTick,
		queriedSize );
}

// 090122 LUJ, 돈 로그를 쿼리한다
void RGetMoneyLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_MONEY_LOG message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_RM_MONEY_LOG;
	message.Protocol = MP_RM_MONEY_LOG_ACK;
	message.mSize = dbMessage->dwResult;
	message.mRequestTick = ( 0 < dbMessage->dwResult ) ? atoi( ( char* )query[ 0 ].Data[ 11 ] ) : 0;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST& record = query [ i ];
		MSG_RM_MONEY_LOG::Log& log = message.mLog[ i ];
		log.mIndex = atoi( ( char* )record.Data[ 0 ] );
		// 090826 ShinJS --- 돈 로그 Type 오류 수정
		log.mType = eMoneyLogtype( atoi( ( char* )record.Data[ 1 ] ) );
		log.mMarginMoney = MONEYTYPE( atof( ( char* )record.Data[ 2 ] ) );
		MSG_RM_MONEY_LOG::Player& sourcePlayer = log.mSourcePlayer;
		sourcePlayer.mIndex = atoi( ( char* )record.Data[ 3 ] );
		sourcePlayer.mInventoryMoney = MONEYTYPE( atof( ( char* )record.Data[ 4 ] ) );
		MSG_RM_MONEY_LOG::Player& targetPlayer = log.mTargetPlayer;
		targetPlayer.mIndex = atoi( ( char* )record.Data[ 5 ] );
		targetPlayer.mInventoryMoney = MONEYTYPE( atof( ( char* )record.Data[ 6 ] ) );
		StringCopySafe( log.mDate, ( char* )record.Data[ 7 ], sizeof( log.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	// 090122 LUJ, 최대로 쿼리한 경우에는 다음 페이지를 쿼리 요청한다
	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];
		const char*	beginDate =	( char* )record.Data[ 7 ];
		const char*	endDate = ( char* )record.Data[ 9 ];
		const char* playerIndex = ( char* )record.Data[ 8 ];
		const DWORD	size = atoi( ( char* )record.Data[ 10 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			GetMoneyLog(
				connectionIndex,
				beginDate,
				endDate,
				atoi( playerIndex ),
				message.mRequestTick,
				size );
		}
	}
}

// 09609 ShinJS, AutoNote 로그 개수를 가져온다
void RGetAutoNoteLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult ||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_AUTONOTE_LOG;
		message.Protocol = MP_RM_AUTONOTE_LOG_SIZE_NACK;
		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record	= query[ 0 ];
	const DWORD	requestTick	= atoi( ( char* )record.Data[ 1 ] );

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_AUTONOTE_LOG;
		message.Protocol = MP_RM_AUTONOTE_LOG_SIZE_ACK;
		message.mSize = min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick = requestTick;
		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* const beginDate = ( char* )record.Data[ 2 ];
	const char* const endDate = ( char* )record.Data[ 3 ];
	const char* const playerIndex = ( char* )record.Data[ 4 ];

	GetAutoNoteLog(
		connectionIndex,
		beginDate,
		endDate,
		atoi( playerIndex ),
		requestTick,
		0 );
}

void GetAutoNoteLog( DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize )
{
	g_DB.LogQuery(
		RGetAutoNoteLog,
		connectionIndex,
		"EXEC dbo.TP_AUTONOTE_LOG_SELECT \'%s\', \'%s\', %d, %d, %d",
		beginDate,
		endDate,
		playerIndex,
		requestTick,
		queriedSize );
}

void RGetAutoNoteLog( LPQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_AUTONOTE_LOG message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_RM_AUTONOTE_LOG;
	message.Protocol = MP_RM_AUTONOTE_LOG_ACK;
	message.mSize = dbMessage->dwResult;
	message.mRequestTick = ( 0 < dbMessage->dwResult ) ? atoi( ( char* )query[ 0 ].Data[ 9 ] ) : 0;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const QUERYST& record = query [ i ];
		MSG_RM_AUTONOTE_LOG::Log& log = message.mLog[ i ];
		log.mIndex = atoi( ( char* )record.Data[ 0 ] );
		log.mType = eAutoNoteLogKind( atoi( ( char* )record.Data[ 1 ] ) );
		log.mReportPlayerIndex = atoi( ( char* )record.Data[ 2 ] );
		StringCopySafe( log.mReportPlayer, ( char* )record.Data[ 3 ], sizeof( log.mReportPlayer ) );
		log.mAutoPlayerIndex = atoi( ( char* )record.Data[ 4 ] );
		StringCopySafe( log.mAutoPlayer, ( char* )record.Data[ 5 ], sizeof( log.mAutoPlayer ) );
		StringCopySafe( log.mDate, ( char* )record.Data[ 6 ], sizeof( log.mDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	// 090122 LUJ, 최대로 쿼리한 경우에는 다음 페이지를 쿼리 요청한다
	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const QUERYST& record = query[ MAX_ROW_NUM - 1 ];
		const char*	beginDate =	( char* )record.Data[ 6 ];
		const char*	endDate = ( char* )record.Data[ 7 ];
		const char* playerIndex = ( char* )record.Data[ 8 ];
		const DWORD	size = atoi( ( char* )record.Data[ 9 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			GetAutoNoteLog(
				connectionIndex,
				beginDate,
				endDate,
				atoi( playerIndex ),
				message.mRequestTick,
				size );
		}
	}
}

void RUserDataUpdate( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	if( ! dbMessage->dwResult	||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
		return;

	const DWORD connectionIndex = dbMessage->dwID;
	const MIDDLEQUERYST& record = query[ 0 ];

	const char* userIndex = ( char* )record.Data[ 0 ];
	const char* punishKind = ( char* )record.Data[ 1 ];
	const char* punishTime = ( char* )record.Data[ 2 ];
	const char* punishStartDate = ( char* )record.Data[ 3 ];
	const char* punishEndDate = ( char* )record.Data[ 4 ];

	MSG_RM_PUNISH_DATA message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_USER;
	message.Protocol	= MP_RM_USER_UPDATE_PUNISH_DATA;
	message.mIndex = atoi( userIndex );
	StringCopySafe( message.mAutoPunishStartDate, punishStartDate, sizeof( message.mAutoPunishStartDate ) );
	StringCopySafe( message.mAutoPunishEndDate, punishEndDate, sizeof( message.mAutoPunishEndDate ) );

	NETWORK->Send( connectionIndex, message, sizeof( message ) );

	// 090618 ShinJS --- Auto Punish 정보 갱신에 대한 로그 저장
	const CUserManager::User& user = CUserManager::GetInstance().GetUser( connectionIndex );

	g_DB.LogMiddleQuery(
		0,
		0,
		"INSERT INTO TB_OPERATOR_LOG ( TYPE, OPER_ID, KEY1, VALUE1, KEY2, VALUE2, KEY3, VALUE3 ) VALUES ( %d, \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\' )",
		RecoveryLogUpdatePunishData,
		user.mId.c_str(),
		"userIndex",
		userIndex,
		"punishKind",
		punishKind,
		"punishTime",
		punishTime);
}

void RGetHouseLogSize(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	if(! dbMessage->dwResult ||
		! atoi((char*)query[0].Data[0]))
	{
		MSGROOT message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_HOUSE_LOG;
		message.Protocol = MP_RM_HOUSE_LOG_SIZE_NACK;
		NETWORK->Send(connectionIndex, message, sizeof(message));
		return;
	}

	const MIDDLEQUERYST& record	= query[0];
	const DWORD	requestTick	= atoi((char*)record.Data[1]);

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_HOUSE_LOG;
		message.Protocol = MP_RM_HOUSE_LOG_SIZE_ACK;
		message.mSize = min(atoi((char*)record.Data[0]), MAX_LOG_SIZE);
		message.mRequestTick = requestTick;
		NETWORK->Send(connectionIndex, message, sizeof(message));
	}

	const char* const beginDate = (char*)record.Data[2];
	const char* const endDate = (char*)record.Data[3];
	const char* const userIndex = (char*)record.Data[4];

	GetHouseLog(
		connectionIndex,
		beginDate,
		endDate,
		atoi(userIndex),
		requestTick,
		0);
}

void GetHouseLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD userIndex, DWORD requestTick, DWORD queriedSize)
{
	g_DB.LogQuery(
		RGetHouseLog,
		connectionIndex,
		"EXEC dbo.TP_HOUSE_LOG_SELECT \'%s\', \'%s\', %d, %d, %d",
		beginDate,
		endDate,
		userIndex,
		requestTick,
		queriedSize);
}

void RGetHouseLog(LPQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_HOUSE_LOG message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_HOUSE_LOG;
	message.Protocol = MP_RM_HOUSE_LOG_ACK;
	message.mSize = dbMessage->dwResult;
	message.mRequestTick = (0 < dbMessage->dwResult) ? atoi((char*)query[0].Data[0]) : 0;

	for(DWORD i = 0; i < dbMessage->dwResult; ++i)
	{
		const QUERYST& record = query[i];
		MSG_RM_HOUSE_LOG::Log& log = message.mLog[i];
		log.mIndex = atoi(LPCTSTR(record.Data[1]));
		log.mUser = atoi(LPCTSTR(record.Data[2]));
		StringCopySafe(
			log.mPlayer,
			LPCTSTR(record.Data[3]),
			sizeof(log.mPlayer));
		log.mType = eHouseLog(atoi(LPCTSTR(record.Data[4])));
		log.mItem.mIndex = atoi(LPCTSTR(record.Data[5]));
		log.mItem.mDbIndex = atoi(LPCTSTR(record.Data[6]));
		log.mFurniture = atoi(LPCTSTR(record.Data[7]));
		log.mSlot = atoi(LPCTSTR(record.Data[8]));
		StringCopySafe(
			log.mHouse,
			LPCTSTR(record.Data[9]),
			sizeof(log.mHouse));
		log.mDecorationPoint = atoi(LPCTSTR(record.Data[10]));
		log.mHousePoint = atoi(LPCTSTR(record.Data[11]));
		StringCopySafe(
			log.mDate,
			LPCTSTR(record.Data[12]),
			sizeof(log.mDate));
	}

	NETWORK->Send(connectionIndex, message, message.GetSize());

	// 090122 LUJ, 최대로 쿼리한 경우에는 다음 페이지를 쿼리 요청한다
	if(MAX_ROW_NUM == dbMessage->dwResult)
	{
		const QUERYST& record = query[MAX_ROW_NUM - 1];
		const char*	beginDate =	(char*)record.Data[12];
		const char*	endDate = (char*)record.Data[13];
		const char* userIndex = (char*)record.Data[14];
		const DWORD	size = atoi((char*)record.Data[15]) + MAX_ROW_NUM;

		if(MAX_LOG_SIZE > size)
		{
			GetHouseLog(
				connectionIndex,
				beginDate,
				endDate,
				atoi(userIndex),
				message.mRequestTick,
				size);
		}
	}
}

void RGetCookLogSize(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	if(! dbMessage->dwResult ||
		! atoi((char*)query[0].Data[0]))
	{
		MSGROOT message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_COOK_LOG;
		message.Protocol = MP_RM_COOK_LOG_SIZE_NACK;
		NETWORK->Send(connectionIndex, message, sizeof(message));
		return;
	}

	const MIDDLEQUERYST& record	= query[0];
	const DWORD	requestTick	= atoi((char*)record.Data[1]);

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_COOK_LOG;
		message.Protocol = MP_RM_COOK_LOG_SIZE_ACK;
		message.mSize = min(atoi((char*)record.Data[0]), MAX_LOG_SIZE);
		message.mRequestTick = requestTick;
		NETWORK->Send(connectionIndex, message, sizeof(message));
	}

	const char* const beginDate = (char*)record.Data[2];
	const char* const endDate = (char*)record.Data[3];
	const char* const userIndex = (char*)record.Data[4];

	GetCookLog(
		connectionIndex,
		beginDate,
		endDate,
		atoi(userIndex),
		requestTick,
		0);
}

void GetCookLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD userIndex, DWORD requestTick, DWORD queriedSize)
{
	g_DB.LogQuery(
		RGetCookLog,
		connectionIndex,
		"EXEC dbo.TP_COOK_LOG_SELECT \'%s\', \'%s\', %d, %d, %d",
		beginDate,
		endDate,
		userIndex,
		requestTick,
		queriedSize);
}

void RGetCookLog(LPQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_COOK_LOG message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_COOK_LOG;
	message.Protocol = MP_RM_COOK_LOG_ACK;
	message.mSize = dbMessage->dwResult;
	message.mRequestTick = (0 < dbMessage->dwResult) ? atoi((char*)query[0].Data[0]) : 0;

	for(DWORD i = 0; i < dbMessage->dwResult; ++i)
	{
		const QUERYST& record = query[i];
		MSG_RM_COOK_LOG::Log& log = message.mLog[i];
		log.mIndex = _ttoi((char*)record.Data[1]);
		log.mPlayerIndex = _ttoi((char*)record.Data[2]);
		log.mType = eCookingLog(_ttoi((char*)record.Data[3]));
		log.mItemIndex = _ttoi((char*)record.Data[4]);
		log.mValue = _ttoi((char*)record.Data[5]);
		StringCopySafe(
			log.mDate,
			(char*)record.Data[8],
			_countof(log.mDate));
	}

	NETWORK->Send(connectionIndex, message, message.GetSize());

	// 090122 LUJ, 최대로 쿼리한 경우에는 다음 페이지를 쿼리 요청한다
	if(MAX_ROW_NUM == dbMessage->dwResult)
	{
		const QUERYST& record = query[MAX_ROW_NUM - 1];
		const char*	beginDate =	(char*)record.Data[8];
		const char*	endDate = (char*)record.Data[9];
		const char* playerIndex = (char*)record.Data[10];
		const DWORD	size = atoi((char*)record.Data[11]) + MAX_ROW_NUM;

		if(MAX_LOG_SIZE > size)
		{
			GetCookLog(
				connectionIndex,
				beginDate,
				endDate,
				atoi(playerIndex),
				message.mRequestTick,
				size);
		}
	}
}

void RGetRecipeLogSize(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	if(! dbMessage->dwResult ||
		! atoi((char*)query[0].Data[0]))
	{
		MSGROOT message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_COOK_LOG;
		message.Protocol = MP_RM_COOK_RECIPE_LOG_SIZE_NACK;
		NETWORK->Send(connectionIndex, message, sizeof(message));
		return;
	}

	const MIDDLEQUERYST& record	= query[0];
	const DWORD	requestTick	= atoi((char*)record.Data[1]);

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_COOK_LOG;
		message.Protocol = MP_RM_COOK_RECIPE_LOG_SIZE_ACK;
		message.mSize = min(atoi((char*)record.Data[0]), MAX_LOG_SIZE);
		message.mRequestTick = requestTick;
		NETWORK->Send(connectionIndex, message, sizeof(message));
	}

	const char* const beginDate = (char*)record.Data[2];
	const char* const endDate = (char*)record.Data[3];
	const char* const userIndex = (char*)record.Data[4];

	GetRecipeLog(
		connectionIndex,
		beginDate,
		endDate,
		atoi(userIndex),
		requestTick,
		0);
}

void GetRecipeLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD userIndex, DWORD requestTick, DWORD queriedSize)
{
	g_DB.LogQuery(
		RGetRecipeLog,
		connectionIndex,
		"EXEC dbo.TP_RECIPE_LOG_SELECT \'%s\', \'%s\', %d, %d, %d",
		beginDate,
		endDate,
		userIndex,
		requestTick,
		queriedSize);
}

void RGetRecipeLog(LPQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_RECIPE_LOG message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_COOK_LOG;
	message.Protocol = MP_RM_COOK_RECIPE_LOG_ACK;
	message.mSize = dbMessage->dwResult;
	message.mRequestTick = (0 < dbMessage->dwResult) ? atoi((char*)query[0].Data[0]) : 0;

	for(DWORD i = 0; i < dbMessage->dwResult; ++i)
	{
		const QUERYST& record = query[i];
		MSG_RM_RECIPE_LOG::Log& log = message.mLog[i];
		log.mLogIndex = _ttoi((char*)record.Data[1]);
		log.mPlayerIndex = _ttoi((char*)record.Data[2]);
		log.mRecipeIndex = _ttoi((char*)record.Data[3]);
		log.mSlot = _ttoi((char*)record.Data[4]);
		log.mType = eCOOKRECIPE(_ttoi((char*)record.Data[5]));
		StringCopySafe(
			log.mDate,
			(char*)record.Data[6],
			_countof(log.mDate));
	}

	NETWORK->Send(connectionIndex, message, message.GetSize());

	// 090122 LUJ, 최대로 쿼리한 경우에는 다음 페이지를 쿼리 요청한다
	if(MAX_ROW_NUM == dbMessage->dwResult)
	{
		const QUERYST& record = query[MAX_ROW_NUM - 1];
		const char*	beginDate =	(char*)record.Data[6];
		const char*	endDate = (char*)record.Data[7];
		const char* playerIndex = (char*)record.Data[8];
		const DWORD	size = atoi((char*)record.Data[9]) + MAX_ROW_NUM;

		if(MAX_LOG_SIZE > size)
		{
			GetRecipeLog(
				connectionIndex,
				beginDate,
				endDate,
				atoi(playerIndex),
				message.mRequestTick,
				size);
		}
	}
}

void RGetFurniture(LPQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_FURNITURE message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_ITEM;
	message.Protocol = MP_RM_ITEM_GET_FURNITURE_ACK;
	message.mSize = dbMessage->dwResult;
	
	for(size_t i = 0; i < dbMessage->dwResult; ++i)
	{
		const QUERYST& record = query[i];

		MSG_RM_FURNITURE::Furniture& furniture = message.mFurniture[i];
		furniture.mFurnitureIndex = _ttoi(LPCTSTR(record.Data[0]));
		furniture.mItemIndex = _ttoi(LPCTSTR(record.Data[1]));
		furniture.mPosition.x = float(_tstof(LPCTSTR(record.Data[2])));
		furniture.mPosition.y = float(_tstof(LPCTSTR(record.Data[3])));
		furniture.mPosition.z = float(_tstof(LPCTSTR(record.Data[4])));
		furniture.mAngle = float(_tstof(LPCTSTR(record.Data[5])));
		furniture.mCategory = HOUSE_HIGHCATEGORY(_ttoi(LPCTSTR(record.Data[6])));
		furniture.mSlot = POSTYPE(_ttoi(LPCTSTR(record.Data[7])));
		furniture.mState = eHOUSEFURNITURE_STATE(_ttoi(LPCTSTR(record.Data[8])));
		furniture.mIsUnableRemove = _ttoi(LPCTSTR(record.Data[9]));
		_tcsncpy(
			furniture.mDate,
			LPCTSTR(record.Data[10]),
			_countof(furniture.mDate));
	}

	NETWORK->Send(
		connectionIndex,
		message,
		message.GetSize());
}

void RGetNoteList(LPQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_NOTE_LIST	message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_PLAYER;
	message.Protocol = MP_RM_PLAYER_GET_NOTE_LIST_ACK;
	message.mSize = dbMessage->dwResult;

	for(size_t i = 0; i < dbMessage->dwResult; ++i)
	{
		const QUERYST& record = query[i];

		MSG_RM_NOTE_LIST::Note& note = message.mNote[i];
		note.mIndex = _ttoi(
			LPCTSTR(record.Data[0]));
		_tcsncpy(
			note.mDate,
			LPCTSTR(record.Data[1]),
			_countof(note.mDate));
		_sntprintf(
			note.mTitle,
			_countof(note.mTitle),
			"%s%s",
			LPCTSTR(record.Data[2]),
			LPCTSTR(record.Data[3]));
	}

	NETWORK->Send(
		connectionIndex,
		message,
		message.GetSize());
}

void RGetNote(LPLARGEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_NOTE message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_PLAYER;
	message.Protocol = MP_RM_PLAYER_GET_NOTE_ACK;

	if(1 == dbMessage->dwResult)
	{
		const LARGEQUERYST& record = query[0];

		message.mNoteIndex = _ttoi(
			LPCTSTR(record.Data[0]));
		_tcsncpy(
			message.mSendedPlayer,
			LPCTSTR(record.Data[1]),
			_countof(message.mSendedPlayer));
		message.mIsRead = _ttoi(
			LPCTSTR(record.Data[2]));
		_tcsncpy(
			message.mNote,
			LPCTSTR(record.Data[3]),
			_countof(message.mNote));
	}

	NETWORK->Send(
		connectionIndex,
		message,
		sizeof(message));
}

void RGetConsignLogSize(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	if(! dbMessage->dwResult ||
		! atoi((char*)query[0].Data[0]))
	{
		MSGROOT message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_CONSIGN_LOG;
		message.Protocol = MP_RM_CONSIGN_LOG_SIZE_NACK;
		NETWORK->Send(connectionIndex, message, sizeof(message));
		return;
	}

	const MIDDLEQUERYST& record	= query[0];
	const DWORD	requestTick	= atoi((char*)record.Data[1]);

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_CONSIGN_LOG;
		message.Protocol = MP_RM_CONSIGN_LOG_SIZE_ACK;
		message.mSize = min(atoi((char*)record.Data[0]), MAX_LOG_SIZE);
		message.mRequestTick = requestTick;
		NETWORK->Send(connectionIndex, message, sizeof(message));
	}

	const char* const beginDate = (char*)record.Data[2];
	const char* const endDate = (char*)record.Data[3];
	const char* const playerIndex = (char*)record.Data[4];
	const char* const itemDBIndex = (char*)record.Data[5];
	const char* const consignIndex = (char*)record.Data[6];

	GetConsignLog(
		connectionIndex,
		beginDate,
		endDate,
		atoi(playerIndex),
		atoi(itemDBIndex),
		atoi(consignIndex),
		requestTick,
		0);
}

void GetConsignLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD itemDBIndex, DWORD consignIndex, DWORD requestTick, DWORD queriedSize)
{
	g_DB.LogQuery(
		RGetConsignLog,
		connectionIndex,
		"EXEC dbo.TP_CONSIGN_LOG_SELECT \'%s\', \'%s\', %d, %d, %d, %d, %d",
		beginDate,
		endDate,
		playerIndex,
		itemDBIndex,
		consignIndex,
		requestTick,
		queriedSize);
}

void RGetConsignLog(LPQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_CONSIGN_LOG message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_CONSIGN_LOG;
	message.Protocol = MP_RM_CONSIGN_LOG_ACK;
	message.mSize = dbMessage->dwResult;
	message.mRequestTick = (0 < dbMessage->dwResult) ? atoi((char*)query[0].Data[0]) : 0;

	for(DWORD i = 0; i < dbMessage->dwResult; ++i)
	{
		const QUERYST& record = query[i];
		MSG_RM_CONSIGN_LOG::Log& log = message.mLog[i];
		log.mLogIndex = _ttoi((char*)record.Data[1]);
		log.mType = eConsignmentLog(_ttoi(LPCTSTR(record.Data[2])));
		log.mPlayerIndex = _ttoi(LPCTSTR(record.Data[3]));
		log.mItemDbIndex = _ttoi(LPCTSTR(record.Data[4]));
		log.mItemIndex = _ttoi(LPCTSTR(record.Data[5]));
		log.mQuantity = _ttoi(LPCTSTR(record.Data[6]));

		const MONEYTYPE baseMoney = _ttoi(LPCTSTR(record.Data[7]));
		const MONEYTYPE upperMoney = _ttoi(LPCTSTR(record.Data[8]));
		const MONEYTYPE lowerMoney = _ttoi(LPCTSTR(record.Data[9]));
		log.mPrice = upperMoney * baseMoney + lowerMoney;
		StringCopySafe(
			log.mDate,
			(char*)record.Data[10],
			_countof(log.mDate));

		log.mConsignIndex = _ttoi(LPCTSTR(record.Data[15]));
	}

	NETWORK->Send(
		connectionIndex,
		message,
		message.GetSize());

	// 090122 LUJ, 최대로 쿼리한 경우에는 다음 페이지를 쿼리 요청한다
	if(MAX_ROW_NUM == dbMessage->dwResult)
	{
		const QUERYST& record = query[MAX_ROW_NUM - 1];
		const char*	beginDate =	(char*)record.Data[10];
		const char*	endDate = (char*)record.Data[11];
		const char* playerIndex = (char*)record.Data[12];
		const DWORD	size = atoi((char*)record.Data[13]) + MAX_ROW_NUM;
		const char* itemDBIndex = (char*)record.Data[14];
		const char* consignIndex = (char*)record.Data[15];

		if(MAX_LOG_SIZE > size)
		{
			GetConsignLog(
				connectionIndex,
				beginDate,
				endDate,
				atoi(playerIndex),
				atoi(itemDBIndex),
				atoi(consignIndex),
				message.mRequestTick,
				size);
		}
	}
}

void RGetGameRoomPointLogSize(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	if(! dbMessage->dwResult ||
		! atoi((char*)query[0].Data[0]))
	{
		MSGROOT message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_GAMEROOM_POINT_LOG;
		message.Protocol = MP_RM_GAMEROOM_POINT_LOG_SIZE_NACK;
		NETWORK->Send(connectionIndex, message, sizeof(message));
		return;
	}

	const MIDDLEQUERYST& record	= query[0];
	const DWORD	requestTick	= atoi((char*)record.Data[4]);

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_RM_GAMEROOM_POINT_LOG;
		message.Protocol = MP_RM_GAMEROOM_POINT_LOG_SIZE_ACK;
		message.mSize = min(atoi((char*)record.Data[0]), MAX_LOG_SIZE);
		message.mRequestTick = requestTick;
		NETWORK->Send(connectionIndex, message, sizeof(message));
	}

	LPCTSTR beginDate = (char*)record.Data[2];
	LPCTSTR const endDate = (char*)record.Data[3];
	LPCTSTR const userIndex = (char*)record.Data[1];

	GetGameRoomPointLog(
		connectionIndex,
		beginDate,
		endDate,
		atoi(userIndex),
		requestTick,
		0);
}

void GetGameRoomPointLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize)
{
	g_DB.LogQuery(
		RGetGameRoomPointLog,
		connectionIndex,
		"EXEC dbo.TP_GAMEROOM_POINT_LOG_SELECT \'%s\', \'%s\', %d, %d, %d",
		beginDate,
		endDate,
		playerIndex,
		requestTick,
		queriedSize);
}

void RGetGameRoomPointLog(LPQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_GAMEROOM_POINT_LOG message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_GAMEROOM_POINT_LOG;
	message.Protocol = MP_RM_GAMEROOM_POINT_LOG_ACK;
	message.mSize = dbMessage->dwResult;
	message.mRequestTick = (0 < dbMessage->dwResult) ? atoi((char*)query[0].Data[0]) : 0;

	for(DWORD i = 0; i < dbMessage->dwResult; ++i)
	{
		const QUERYST& record = query[i];
		MSG_RM_GAMEROOM_POINT_LOG::Log& log = message.mLog[i];
		log.mLogIndex = _ttoi((char*)record.Data[1]);
		log.mUserIndex = _ttoi(LPCTSTR(record.Data[2]));
		log.mType = eLogitemmoney(_ttoi(LPCTSTR(record.Data[3])));		
		log.mPoint = _ttoi(LPCTSTR(record.Data[4]));	
		StringCopySafe(
			log.mDate,
			(char*)record.Data[5],
			_countof(log.mDate));
	}

	NETWORK->Send(
		connectionIndex,
		message,
		message.GetSize());

	// 090122 LUJ, 최대로 쿼리한 경우에는 다음 페이지를 쿼리 요청한다
	if(MAX_ROW_NUM == dbMessage->dwResult)
	{
		const QUERYST& record = query[MAX_ROW_NUM - 1];
		const char*	beginDate =	(char*)record.Data[5];
		const char*	endDate = (char*)record.Data[7];
		const char* userIndex = (char*)record.Data[6];
		const DWORD	size = atoi((char*)record.Data[8]) + MAX_ROW_NUM;

		if(MAX_LOG_SIZE > size)
		{
			GetGameRoomPointLog(
				connectionIndex,
				beginDate,
				endDate,
				atoi(userIndex),
				message.mRequestTick,
				size);
		}
	}
}

void RGetGameRoomData(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	if(0 == dbMessage->dwResult)
	{
		return;
	}

	const DWORD connectionIndex = dbMessage->dwID;
	const MIDDLEQUERYST& record = query[0];

	MSG_NAME_DWORD3 message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_USER;
	message.Protocol = MP_RM_USER_GAMEROOM_GET_ACK;
	message.dwData1 = _ttoi(
		LPCTSTR(record.Data[0]));
	message.dwData2 = _ttoi(
		LPCTSTR(record.Data[1]));
	message.dwData3 = _ttoi(
		LPCTSTR(record.Data[2]));
	StringCopySafe(
		message.Name,
		(char*)record.Data[3],
		_countof(message.Name));
	
	NETWORK->Send(
		connectionIndex,
		message,
		sizeof(message));
}

void RGetPartyList(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	MSG_RM_PARTY message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_PLAYER;
	message.Protocol = MP_RM_PLAYER_GET_PARTY_ACK;
	message.mSize = dbMessage->dwResult;

	const DWORD connectionIndex = dbMessage->dwID;

	for(DWORD i = 0; i < dbMessage->dwResult; ++i)
	{
		const MIDDLEQUERYST& record = query[i];
		MSG_RM_PARTY::Member& member = message.mMember[i];

		member.mPlayerIndex = _ttoi(
			LPCTSTR(record.Data[0]));
		member.mRank = _ttoi(
			LPCTSTR(record.Data[1]));
		StringCopySafe(
			member.mName,
			LPCTSTR(record.Data[2]),
			_countof(member.mName));
	}

	NETWORK->Send(
		connectionIndex,
		message,
		message.GetSize());
}

void RGetForbidChatLogSize( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	if( ! dbMessage->dwResult ||
		! atoi( ( char* )query[ 0 ].Data[ 0 ] ) )
	{
		MSGROOT message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_FORBID_CHAT_LOG;
		message.Protocol = MP_RM_FORBID_CHAT_LOG_SIZE_NACK;
		NETWORK->Send( connectionIndex, message, sizeof( message ) );
		return;
	}

	const MIDDLEQUERYST& record	= query[ 0 ];
	const DWORD	requestTick	= atoi( ( char* )record.Data[ 1 ] );

	{
		MSG_RM_LOG_SIZE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_FORBID_CHAT_LOG;
		message.Protocol = MP_RM_FORBID_CHAT_LOG_SIZE_ACK;
		message.mSize = min( atoi( ( char* )record.Data[ 0 ] ), MAX_LOG_SIZE );
		message.mRequestTick = requestTick;
		NETWORK->Send( connectionIndex, message, sizeof( message ) );
	}	

	const char* const beginDate = ( char* )record.Data[ 2 ];
	const char* const endDate = ( char* )record.Data[ 3 ];
	const char* const playerIndex = ( char* )record.Data[ 4 ];

	GetForbidChatLog(
		connectionIndex,
		beginDate,
		endDate,
		atoi( playerIndex ),
		requestTick,
		0 );
}

void GetForbidChatLog( DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize )
{
	g_DB.FreeMiddleQuery(
		RGetForbidChatLog,
		connectionIndex,
		"EXEC dbo.TP_FORBID_CHAT_LOG_SELECT \'%s\', \'%s\', %d, %d, %d",
		beginDate,
		endDate,
		playerIndex,
		requestTick,
		queriedSize );
}

void RGetForbidChatLog( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectionIndex = dbMessage->dwID;

	MSG_RM_FORBID_CHAT_LOG message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_RM_FORBID_CHAT_LOG;
	message.Protocol = MP_RM_FORBID_CHAT_LOG_ACK;
	message.mSize = dbMessage->dwResult;
	message.mRequestTick = ( 0 < dbMessage->dwResult ) ? atoi( ( char* )query[ 0 ].Data[ 4 ] ) : 0;

	for( DWORD i = 0; i < dbMessage->dwResult; ++i )
	{
		const MIDDLEQUERYST& record = query [ i ];
		MSG_RM_FORBID_CHAT_LOG::Log& log = message.mLog[ i ];
		log.mPlayerIndex = atoi( ( char* )record.Data[ 0 ] );
		StringCopySafe( log.m_OperatorName, ( char* )record.Data[ 1 ], sizeof( log.m_OperatorName ) );
		StringCopySafe( log.mReason, ( char* )record.Data[ 2 ], sizeof( log.mReason ) );
		StringCopySafe( log.mRegisterDate, ( char* )record.Data[ 3 ], sizeof( log.mRegisterDate ) );
		StringCopySafe( log.mReleaseDate, ( char* )record.Data[ 4 ], sizeof( log.mReleaseDate ) );
	}

	NETWORK->Send( connectionIndex, message, message.GetSize() );

	if( MAX_ROW_NUM == dbMessage->dwResult )
	{
		const MIDDLEQUERYST& record = query[ MAX_ROW_NUM - 1 ];
		const char*	beginDate =	( char* )record.Data[ 3 ];
		const char*	endDate = ( char* )record.Data[ 4 ];
		const char* playerIndex = ( char* )record.Data[ 5 ];
		const DWORD	size = atoi( ( char* )record.Data[ 6 ] ) + MAX_ROW_NUM;

		if( MAX_LOG_SIZE > size )
		{
			GetForbidChatLog(
				connectionIndex,
				beginDate,
				endDate,
				atoi( playerIndex ),
				message.mRequestTick,
				size );
		}
	}
}

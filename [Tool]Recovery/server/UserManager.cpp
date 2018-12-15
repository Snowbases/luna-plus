#include "stdafx.h"
#include "UserManager.h"

CUserManager::CUserManager()
{}

CUserManager::~CUserManager()
{}

CUserManager& CUserManager::GetInstance()
{
	static CUserManager instance;

	return instance;
}

bool CUserManager::AddUser( DWORD connectionIndex, const char* id, const char* name, const char* ip, const char* registedDate, eGM_POWER power )
{
	if( mUserMap.end() != mUserMap.find( connectionIndex ) )
	{
		return false;
	}

	User& user = mUserMap[ connectionIndex ];

	user.mId			= id;
	user.mName			= name;
	user.mIp			= ip;
	user.mRegistedDate	= registedDate;
	user.mPower			= power;

	return true;
}


void CUserManager::RemoveUser( DWORD connectionIndex )
{
	mUserMap.erase( connectionIndex );
}


bool CUserManager::IsConnect( DWORD connectionIndex ) const
{
	return mUserMap.end() != mUserMap.find( connectionIndex );
}


bool CUserManager::IsConnectedUser( const char* id ) const
{
	for(
		UserMap::const_iterator it = mUserMap.begin();
		mUserMap.end() != it;
		++it )
	{
		const User& user = it->second;

		if( user.mId == id )
		{
			return true;
		}
	}

	return false;
}


bool CUserManager::IsConnectedIp( const char* ip ) const
{
	for(
		UserMap::const_iterator it = mUserMap.begin();
		mUserMap.end() != it;
		++it )
	{
		const User& user = it->second;

		if( user.mIp == ip )
		{
			return true;
		}
	}

	return false;
}


const CUserManager::User& CUserManager::GetUser( DWORD connectionIndex ) const
{
	static const User emptyUser;

	UserMap::const_iterator it = mUserMap.find( connectionIndex );

	return mUserMap.end() == it ? emptyUser : it->second;
}


const CUserManager::User& CUserManager::GetUser( const char* id ) const
{
	static const User emptyUser;

	for(
		UserMap::const_iterator it = mUserMap.begin();
		mUserMap.end() != it;
		++it )
	{
		const User& user = it->second;

		if( user.mId == id )
		{
			return user;
		}
	}

	return emptyUser;
}


bool CUserManager::IsAuthorize( RecoveryCategory category, BYTE protocol, DWORD connectionIndex ) const
{
	// 081021 LUJ, 실행한 명령이 권한에 부합되는지 체크한다
	PutLog(
		"Authorizing... category: %d, protocol: %d",
		category,
		protocol );

	const User& user = GetUser( connectionIndex );

	if( user.mId.empty() )
	{
		return false;
	}
	else if( eGM_POWER_MASTER == user.mPower )
	{
		PutLog( "Authorized. You are master" );
		return true;
	}

	const AuthorityMap::const_iterator authority_it = mAuthorityMap.find( user.mPower );

	const bool userHasNoAuthority = ( mAuthorityMap.end() == authority_it );

	if( userHasNoAuthority )
	{
		PutLog(
			"Didn't authorize for rank %d",
			user.mPower );
		return false;
	}

	const MessageKeySet& messageKeySet = authority_it->second;

	if( messageKeySet.end() == messageKeySet.find( GetMessageKey( category, protocol ) ) )
	{
		PutLog(
			"Didn't authorize for rank %d",
			user.mPower );
		return false;
	}

	PutLog(
		"Authorized for rank %d",
		user.mPower );
	return true;
}


CUserManager::MessageKey CUserManager::GetMessageKey( RecoveryCategory category, BYTE protocol ) const
{
	/// double 형이 int의 두배가 아니면 메모리 덮어쓰기 등 잡을 수 없는 오류가 발생할 수 있다.
	assert( sizeof( LONGLONG ) == sizeof( int ) * 2 );

	MessageKey key = 0;

	/// 총 8바이트인 double의 전방 4바이트에 x값을 후방 4바이트에 y값을 복사해서 고유키를 만든다.
	/// 값 자체가 중요하게 아니고 유니크하냐 여부가 중요한 거니까...
	memcpy( &key, &category, sizeof( category ) );
	memcpy( ( RecoveryCategory* )&key + 1, &protocol, sizeof( protocol ) );

	return key;
}

// 081021 LUJ, 권한 정보를 갱신한다
void CUserManager::SetAuthority( eGM_POWER power, const MSG_RM_AUTHORITY& message )
{
	AuthorityList authorityList;

	for( DWORD i = 0; i < message.mSize; ++i )
	{
		const MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ i ];

		if( AuthorityTypeVersion == authority.mType )
		{
			continue;
		}

		authorityList.push_back( authority );
	}

	SetAuthority( power, authorityList );
}

// 081021 LUJ, 권한 정보를 갱신한다
//				- 카테고리와 프로토콜이 지정되지 않으면 항상 허용된다
//				- 지정되면 오퍼레이터의 권한과 비교하여 IsAuthority()에서 부울 값을 반환한다
void CUserManager::SetAuthority( eGM_POWER power, const CUserManager::AuthorityList& authorityList )
{
	mAuthorityMap[ power ].clear();

	for(	AuthorityList::const_iterator authority_it = authorityList.begin();
			authorityList.end() != authority_it;
			++authority_it )
	{
		const MSG_RM_AUTHORITY::Authority& authority = *authority_it;
		MessageKeySet& messageKeySet = mAuthorityMap[ authority.mPower ];

		switch( authority.mType )
		{
		case AuthorityTypePlayerLevel:
			{
				if( authority.mReadable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_USER, MP_RM_USER_GET_DATA_SYN ) );
				}

				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_USER, MP_RM_USER_SET_DATA_SYN ) );
				}

				break;
			}
		case AuthorityTypePlayerCreate:
			{
				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_USER, MP_RM_USER_CREATE_SYN ) );
				}

				break;
			}
		case AuthorityTypePlayerDelete:
			{
				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_USER, MP_RM_USER_DELETE_SYN ) );
				}

				break;
			}
		case AuthorityTypePlayerRecover:
			{
				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_USER, MP_RM_USER_RESTORE_SYN ) );
				}

				break;
			}
		case AuthorityTypePlayerUpdate:
			{
				if( authority.mReadable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_PLAYER, MP_RM_PLAYER_GET_DATA_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_PLAYER, MP_RM_PLAYER_GET_NOTE_SYN ) );
				}

				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_PLAYER, MP_RM_PLAYER_SET_DATA_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_PLAYER, MP_RM_PLAYER_SET_EXTENDED_DATA_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_PLAYER, MP_RM_PLAYER_SET_LICENSE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_PLAYER, MP_RM_PLAYER_SET_FISHING_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_PLAYER, MP_RM_PLAYER_SET_HOUSE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_PLAYER, MP_RM_PLAYER_SET_COOK_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_PLAYER, MP_RM_PLAYER_ADD_COOK_RECIPE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_PLAYER, MP_RM_PLAYER_REMOVE_COOK_RECIPE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_USER, MP_RM_USER_GAMEROOM_SET_SYN ) );
				}

				break;
			}
		case AuthorityTypePlayerLog:
			{
				if( authority.mReadable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_FARM_LOG,				MP_RM_FARM_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_LIVESTOCK_LOG,			MP_RM_LIVESTOCK_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_PET_LOG, 				MP_RM_PET_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM_SHOP_LOG,			MP_RM_ITEM_SHOP_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_QUEST_LOG,				MP_RM_QUEST_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_SKILL_LOG,				MP_RM_SKILL_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_EXP_LOG,					MP_RM_EXP_LOG_GET_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_STAT_LOG,				MP_RM_STAT_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_NAME_LOG,				MP_RM_NAME_LOG_GET_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_JOB_LOG,					MP_RM_JOB_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM_DROP_OPTION_LOG,	MP_RM_ITEM_DROP_OPTION_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_GUILD_LOG,				MP_RM_GUILD_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_GUILD_WAREHOUSE_LOG,		MP_RM_GUILD_WAREHOUSE_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_GUILD_SCORE_LOG,			MP_RM_GUILD_SCORE_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM_OPTION_LOG,			MP_RM_ITEM_OPTION_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_FAMILY_POINT_LOG,		MP_RM_FAMILY_POINT_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM_LOG,				MP_RM_ITEM_LOG_GET_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_FAMILY_CROP_LOG,			MP_RM_FAMILY_CROP_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_FISH_LOG,				MP_RM_FISH_LOG_SIZE_SYN ) );		
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM_GLOBAL_LOG,			MP_RM_ITEM_GLOBAL_LOG_SYN ) );
					// 081205 LUJ, 권한 추가
					messageKeySet.insert( GetMessageKey( MP_RM_FAMILY_LOG,				MP_RM_FAMILY_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_CHAT_LOG,				MP_RM_CHAT_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_SCRIPT_HACK_LOG,			MP_RM_SCRIPT_HACK_LOG_SIZE_SYN ) );
					// 090406 LUJ, 돈 로그
					messageKeySet.insert( GetMessageKey( MP_RM_MONEY_LOG, MP_RM_MONEY_LOG_SIZE_SYN ) );
					// 090416 LUJ, 공성 로그
					messageKeySet.insert( GetMessageKey( MP_RM_SIEGE_LOG, MP_RM_SIEGE_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_HOUSE_LOG, MP_RM_HOUSE_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_COOK_LOG, MP_RM_COOK_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_COOK_LOG, MP_RM_COOK_RECIPE_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_CONSIGN_LOG, MP_RM_CONSIGN_LOG_SIZE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_GAMEROOM_POINT_LOG, MP_RM_GAMEROOM_POINT_LOG_SIZE_SYN ) );
				}

				break;
			}
		case AuthorityTypeSkill:
			{
				if( authority.mReadable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_SKILL, MP_RM_SKILL_GET_ALL_SYN ) );
				}

				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_SKILL, MP_RM_SKILL_ADD_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_SKILL, MP_RM_SKILL_REMOVE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_SKILL, MP_RM_SKILL_SET_LEVEL_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_SKILL, MP_RM_SKILL_BUFF_REMOVE_SYN ) );
				}

				break;
			}
		case AuthorityTypeQuest:
			{
				if( authority.mReadable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_QUEST, MP_RM_QUEST_GET_MAIN_DATA_SYN ) );
				}

				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_QUEST, MP_RM_QUEST_REMOVE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_QUEST, MP_RM_QUEST_FINISH_SUB_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_QUEST, MP_RM_QUEST_ITEM_UPDATE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_QUEST, MP_RM_QUEST_ITEM_DELETE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_QUEST, MP_RM_QUEST_ITEM_ADD_SYN ) );
				}

				break;
			}
		case AuthorityTypeGuild:
			{
				if( authority.mReadable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_GUILD, MP_RM_GUILD_GET_LIST_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_GUILD, MP_RM_GUILD_GET_DATA_SYN ) );
					// 090407 LUJ, 길드 토너먼트도 길드 권한으로 관리시킨다
					messageKeySet.insert( GetMessageKey( MP_RM_TOURNAMENT, MP_RM_TOURNAMENT_GET_DATA_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_TOURNAMENT, MP_RM_TOURNAMENT_GET_ROUND_SYN ) );
				}

				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_GUILD, MP_RM_GUILD_SET_DATA_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_GUILD, MP_RM_GUILD_SET_RANK_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_GUILD, MP_RM_GUILD_KICK_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_GUILD, MP_RM_GUILD_REMOVE_SKILL_SYN ) );
				}

				break;
			}
		case AuthorityTypeFamily:
			{
				if( authority.mReadable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_FAMILY, MP_RM_FAMILY_GET_LIST_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_FAMILY, MP_RM_FAMILY_GET_DATA_SYN ) );
				}

				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_FAMILY, MP_RM_FAMILY_SET_DATA_SYN ) );
				}

				break;
			}
		case AuthorityTypeOperator:
			{
				if( authority.mReadable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_OPERATOR, MP_RM_OPERATOR_GET_SYN ) );
				}

				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_OPERATOR, MP_RM_OPERATOR_ADD_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_OPERATOR, MP_RM_OPERATOR_REMOVE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_OPERATOR, MP_RM_OPERATOR_SET_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_OPERATOR, MP_RM_OPERATOR_IP_ADD_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_OPERATOR, MP_RM_OPERATOR_IP_REMOVE_SYN ) );
				}

				break;
			}
		case AuthorityTypeAllowIp:
			{
				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_PERMISSION, MP_RM_PERMISSION_ADD_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_PERMISSION, MP_RM_PERMISSION_REMOVE_SYN ) );
				}

				break;
			}
		case AuthorityTypeOperatorLog:
			{
				if( authority.mReadable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_OPERATOR_LOG, MP_RM_OPERATOR_LOG_SIZE_SYN ) );
				}

				break;
			}
			// 090219 LUJ, 아이템 권한 관리
		case AuthorityTypeItem:
			{
				if( authority.mReadable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_GET_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_GET_EXTENDED_INVENTORY_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM_FIND, MP_RM_ITEM_FIND_OWNER_SYN ) );
				}

				if( authority.mWriteable )
				{
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_SET_STORAGE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_ADD_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_UPDATE_END_TIME_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_UPDATE_REMAIN_TIME_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_DELETE_DROP_OPTION_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_SET_PET_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_ADD_PET_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_REMOVE_PET_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_SET_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_SET_OPTION_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_DELETE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_DELETE_OPTION_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_ADD_FURNITURE_SYN ) );
					messageKeySet.insert( GetMessageKey( MP_RM_ITEM, MP_RM_ITEM_REMOVE_FURNITURE_SYN ) );
				}

				break;
			}
		}
	}
}
// UserIPCheckMgr.cpp: implementation of the CUserIPCheckMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserIPCheckMgr.h"
#include "MonitorNetwork.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CUserIPCheckMgr)

CUserIPCheckMgr::CUserIPCheckMgr() :
mUserSize( 0 )
{}

CUserIPCheckMgr::~CUserIPCheckMgr()
{}

LPCTSTR CUserIPCheckMgr::GetIpAddress( DWORD connectionIndex ) const
{
	static TCHAR ipAddress[ MAX_PATH ] = { 0 };
	WORD port = 0;
	MNETWORK->GetUserAddress( connectionIndex, ipAddress, &port );

	return ipAddress;
}

BOOL CUserIPCheckMgr::CheckIP( DWORD connectionIndex )
{
	return TRUE;
}

// 090511 LUJ, 사용자를 추가함
void CUserIPCheckMgr::AddUser( DWORD connectionIndex, const OPERATOR& operatorData, const IPINFO& ipInfo )
{
	RemoveUser( connectionIndex );

	const DWORD arraySize = sizeof( mUserArray ) / sizeof( *mUserArray );

	for( DWORD i = 0; i < arraySize; ++i )
	{
		User& user = mUserArray[ i ];

		if( user.dwIdx )
		{
			continue;
		}

		user.dwIdx = operatorData.dwOperIdx;
		user.mPower = operatorData.mPower;
		StringCopySafe(
			user.sID,
			operatorData.strOperID,
			sizeof( user.sID ) / sizeof( *( user.sID ) ) );
		StringCopySafe(
			user.mIpAddress,
			ipInfo.strIP,
			sizeof( user.mIpAddress ) / sizeof( *( user.mIpAddress ) ) );

		++mUserSize;
		break;
	}
}

// 090511 LUJ, 사용자를 삭제함
void CUserIPCheckMgr::RemoveUser( DWORD connectionIndex )
{
	User& user = GetUser( connectionIndex );

	if( user.dwIdx )
	{
		user = User();
		--mUserSize;
	}
}

CUserIPCheckMgr::User& CUserIPCheckMgr::GetUser( DWORD connectionIndex )
{
	LPCTSTR ipAddrss = GetIpAddress( connectionIndex );

	for( DWORD i = 0; i < GetUserSize(); ++i )
	{
		User& user = mUserArray[ i ];

		if( 0 == _tcscmp( ipAddrss, user.mIpAddress ) )
		{
			return user;
		}
	}

	static User emptyUser;
	return emptyUser;
}

// 090511 LUJ, 해당 접속자의 권한을 반환하도록 함
BOOL CUserIPCheckMgr::IsInvalidProtocol( DWORD connectionIndex, MP_PROTOCOL_MORNITORTOOL requestedProtocol )
{
	const User& user = GetUser( connectionIndex );

	switch( user.mPower )
	{
	case eOperPower_Master:
		{
			return FALSE;
		}
	case eOperPower_Monitor:
		{
			const MP_PROTOCOL_MORNITORTOOL allowedProtocol[] = {
				MP_MORNITORTOOL_QUERYUSERCOUNT_SYN,
				MP_MORNITORTOOL_EXECUTE_MAPSERVER_SYN,
				MP_MORNITORTOOL_SHUTDOWN_MAPSERVER_SYN,
				MP_MORNITORTOOL_KILL_SERVER_SYN,
				MP_MORNITORTOOL_MAPSERVERSTATE_SYN,
				MP_MORNITORTOOL_CHANGE_USERLEVEL_SYN,
				MP_MORNITORTOOL_MSSTATEALL_SYN,
			};

			const SIZE_T protocolCount = sizeof( allowedProtocol ) / sizeof( *allowedProtocol );

			for( SIZE_T i = 0; i < protocolCount; ++i )
			{
				const MP_PROTOCOL_MORNITORTOOL protocol = allowedProtocol[ i ];

				if( requestedProtocol == protocol )
				{
					return FALSE;
				}
			}

			break;
		}
	case eOperPower_Patroller:
		{
			const MP_PROTOCOL_MORNITORTOOL allowedProtocol[] = {
				MP_MORNITORTOOL_MAPSERVERSTATE_SYN,
				MP_MORNITORTOOL_MSSTATEALL_SYN,
			};

			const SIZE_T protocolCount = sizeof( allowedProtocol ) / sizeof( *allowedProtocol );

			for( SIZE_T i = 0; i < protocolCount; ++i )
			{
				const MP_PROTOCOL_MORNITORTOOL protocol = allowedProtocol[ i ];

				if( requestedProtocol == protocol )
				{
					return FALSE;
				}
			}

			break;
		}
	}

	return TRUE;
}

void CUserIPCheckMgr::Filter( DWORD connectionIndex, MSGMONITORTOOLALLSTATE& message )
{
	const User& user = GetUser( connectionIndex );

	switch( user.mPower )
	{
	case eOperPower_Master:
	case eOperPower_Monitor:
		{
			return;
		}
	}

	for(int i = 0; i < message.num; ++i)
	{
		SERVERSTATE& serverState = message.serverState[ i ];
		serverState.UserCount = 0;
	}
}
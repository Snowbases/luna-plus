
// QuickManager.cpp: implementation of the CQuickManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuickManager.h"
#include "MapDBMsgParser.h"
#include "Player.h"
#include "UserTable.h"
#include "BattleSystem_Server.h"
//#include "ItemSlot.h"
//#include "ItemContainer.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CQuickManager::CQuickManager()
{

}

CQuickManager::~CQuickManager()
{

}

void CQuickManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_QUICK_ADD_SYN:
		{
			MSG_QUICK_ADD_SYN* msg = ( MSG_QUICK_ADD_SYN* )pMsg;
			CPlayer * pPlayer = ( CPlayer* )g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) 
			{
				return;
			}

			if( AddQuickInfo( pPlayer, msg->tabNum, msg->pos, &( msg->slot ) ) )
			{
				msg->Protocol = MP_QUICK_ADD_ACK;
				pPlayer->SendMsg( msg, sizeof( MSG_QUICK_ADD_SYN ) );
			}
			else
			{
			}
		}
		break;
	case MP_QUICK_REMOVE_SYN:
		{
			MSG_QUICK_REMOVE_SYN* msg = ( MSG_QUICK_REMOVE_SYN* )pMsg;
	
			CPlayer * pPlayer = ( CPlayer* )g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) 
			{
				return;
			}

			if( RemoveQuickInfo( pPlayer, msg->tabNum, msg->pos ) )
			{
				msg->Protocol = MP_QUICK_REMOVE_ACK;
				pPlayer->SendMsg( msg, sizeof( MSG_QUICK_REMOVE_SYN ) );
			}
		}
		break;
	case MP_QUICK_CHANGE_SYN:
		{
			MSG_QUICK_CHANGE_SYN* msg = ( MSG_QUICK_CHANGE_SYN* )pMsg;
			CPlayer* pPlayer = ( CPlayer* )g_pUserTable->FindUser( msg->dwObjectID );
			if( !pPlayer ) 
			{
				return;
			}

			if( ChangeQuickInfo( pPlayer, msg->tabNum1, msg->pos1, msg->tabNum2, msg->pos2 ) )
			{
				msg->Protocol = MP_QUICK_CHANGE_ACK;
				pPlayer->SendMsg( msg, sizeof( MSG_QUICK_CHANGE_SYN ) );
			}
			else
			{
			}
		}
		break;
	}
}

void CQuickManager::SendQuickInfo( CPlayer* pPlayer )
{
	MSG_QUICK_INFO msg;

	msg.Category = MP_QUICK;
	msg.Protocol = MP_QUICK_INFO;

	for( BYTE i = 0; i < 8; i++ )
	{
		msg.tab[i].tabNum = i;

		for( WORD j = 0; j < 10; j++ )
		{
			memcpy( &( msg.tab[i].slot[j] ), pPlayer->GetQuick(  i ,  j  ), sizeof( SLOT_INFO ) );
		}
	}

	pPlayer->SendMsg( &msg, sizeof( MSG_QUICK_INFO ) );
}

BOOL CQuickManager::AddQuickInfo( CPlayer* pPlayer, BYTE tabNum, BYTE pos, SLOT_INFO* pSlot )
{
	return pPlayer->AddQuick( tabNum, pos, pSlot );
}

BOOL CQuickManager::RemoveQuickInfo( CPlayer* pPlayer, BYTE tabNum, BYTE pos )
{
	SLOT_INFO temp;
	memset( &temp, 0, sizeof( SLOT_INFO ) );

	return pPlayer->AddQuick( tabNum, pos, &temp );
}

BOOL CQuickManager::ChangeQuickInfo( CPlayer* pPlayer, BYTE tabNum1, BYTE pos1, BYTE tabNum2, BYTE pos2 )
{
	SLOT_INFO temp;

	memcpy( &temp, pPlayer->GetQuick( tabNum1, pos1 ), sizeof( SLOT_INFO ) );
	memcpy( pPlayer->GetQuick( tabNum1, pos1 ), pPlayer->GetQuick( tabNum2, pos2 ), sizeof( SLOT_INFO ) );
	memcpy( pPlayer->GetQuick( tabNum2, pos2 ), &temp, sizeof( SLOT_INFO ) );

	return TRUE;
}



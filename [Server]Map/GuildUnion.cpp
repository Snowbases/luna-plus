// GuildUnion.cpp: implementation of the CGuildUnion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuildUnion.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GuildFieldWarMgr.h"
#include "Player.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuildUnion::CGuildUnion()
{
	m_dwGuildUnionIdx = 0;
	memset( m_sGuildUnionName, 0, MAX_GUILD_NAME+1 );
	memset( m_GuildInfo, 0, sizeof(sGUILDIDXNAME)*MAX_GUILD_UNION_NUM );
	m_dwGuildUnionMarkIdx = 0;
}

CGuildUnion::~CGuildUnion()
{
}

void CGuildUnion::SetGuildInfo( int nIndex, DWORD dwGuildIdx )
{
	CGuild* pGuild = GUILDMGR->GetGuild( dwGuildIdx );
	if( !pGuild )	return;

	m_GuildInfo[nIndex].dwGuildIdx = dwGuildIdx;
	strncpy( m_GuildInfo[nIndex].sGuildName, pGuild->GetGuildName(), MAX_GUILD_NAME+1 );

	// 071015 웅주, 동맹 단위의 전쟁이 가능하므로, 클라이언트가 동맹 마스터가 누군지에 대해 알아야 한다.
	if( 0 == nIndex )
	{
		GUILDINFO& info = pGuild->GetInfo();

		info.mIsUnionMaster = TRUE;
	}
}

void CGuildUnion::LoadUnionInfo( DWORD dwUnionIdx, char* pName, DWORD dwGuild0, DWORD dwGuild1, DWORD dwGuild2, DWORD dwGuild3, DWORD dwGuild4, DWORD dwGuild5, DWORD dwGuild6, DWORD dwMarkIdx )
{
	m_dwGuildUnionIdx = dwUnionIdx;
	strncpy( m_sGuildUnionName, pName, sizeof( m_sGuildUnionName ) );
	m_dwGuildUnionMarkIdx = dwMarkIdx;
	
	SetGuildInfo( 0, dwGuild0 );
	SetGuildInfo( 1, dwGuild1 );
	SetGuildInfo( 2, dwGuild2 );
	SetGuildInfo( 3, dwGuild3 );
	SetGuildInfo( 4, dwGuild4 );
	SetGuildInfo( 5, dwGuild5 );
	SetGuildInfo( 6, dwGuild6 );
}

void CGuildUnion::CreateGuildUnion( DWORD dwGuildUnionIdx, char* pGuildUnionName, DWORD dwMasterGuildIdx )
{
	CGuild* pGuild = GUILDMGR->GetGuild( dwMasterGuildIdx );
	if( !pGuild )	return;

	m_dwGuildUnionIdx = dwGuildUnionIdx;
	strncpy( m_sGuildUnionName, pGuildUnionName, sizeof( m_sGuildUnionName ) );
	m_GuildInfo[0].dwGuildIdx = dwMasterGuildIdx;
	strncpy( m_GuildInfo[0].sGuildName, pGuild->GetGuildName(), sizeof( m_GuildInfo[0].sGuildName ) );
	pGuild->SetGuildUnionInfo( m_dwGuildUnionIdx, m_sGuildUnionName, m_dwGuildUnionMarkIdx, TRUE );
}

void CGuildUnion::Destroy()
{
	CGuild* pGuild = NULL;
	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx )
		{
			pGuild = GUILDMGR->GetGuild( m_GuildInfo[i].dwGuildIdx );
			if( pGuild )
				pGuild->SetGuildUnionInfo( 0, "", 0, FALSE );
		}		
	}

	m_dwGuildUnionIdx = 0;
	memset( m_sGuildUnionName, 0, MAX_GUILD_NAME+1 );
	memset( m_GuildInfo, 0, sizeof(sGUILDIDXNAME)*MAX_GUILD_UNION_NUM );
	m_dwGuildUnionMarkIdx = 0;
}

BOOL CGuildUnion::IsEmptyPosition()
{
	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx == 0 )
			return TRUE;
	}
	return FALSE;
}

BOOL CGuildUnion::IsGuildUnion( DWORD dwGuildIdx )
{
	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx == dwGuildIdx )
			return TRUE;
	}
	return FALSE;
}

BOOL CGuildUnion::IsGuildUnion( CGuild* pGuild )
{
	if( !pGuild )	return FALSE;

	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx == pGuild->GetIdx() )
			return TRUE;
	}
	return FALSE;
}

BOOL CGuildUnion::IsGuildWarWithUnionGuild( DWORD dwGuildIdx )
{
	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx )
		if( GUILDWARMGR->IsGuildWar( m_GuildInfo[i].dwGuildIdx, dwGuildIdx ) )
			return TRUE;
	}
	return FALSE;	
}

//int CGuildUnion::AddGuild( DWORD dwGuildIdx )
//{
//	CGuild* guild = GUILDMGR->GetGuild( dwGuildIdx );
//
//	return AddGuild( guild );
//}

int CGuildUnion::AddGuild( CGuild* guild )
{
	if( !	guild	||
			guild->GetUnionIndex() )
	{
		return -1;
	}
	
	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		sGUILDIDXNAME& info = m_GuildInfo[ i ];

		if( info.dwGuildIdx == 0 )
		{
			info.dwGuildIdx = guild->GetIdx();

			SafeStrCpy( info.sGuildName, guild->GetGuildName(), sizeof( info.sGuildName ) );
			guild->SetGuildUnionInfo( m_dwGuildUnionIdx, m_sGuildUnionName, m_dwGuildUnionMarkIdx, FALSE );
			return i;
		}
	}

	return -1;
}

int CGuildUnion::RemoveGuild( DWORD dwGuildIdx )
{
	CGuild* pGuild = GUILDMGR->GetGuild( dwGuildIdx );
	if( !pGuild )						return -1;
	if( !IsGuildUnion( dwGuildIdx ) )	return -1;

	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx == dwGuildIdx )
		{
			m_GuildInfo[i].dwGuildIdx = 0;
			memset( m_GuildInfo[i].sGuildName, 0, MAX_GUILD_NAME+1 );
			pGuild->SetGuildUnionInfo( 0, "", 0, FALSE );
			return i;
		}		
	}
	return -1;
}

int CGuildUnion::RemoveGuild( CGuild* pGuild )
{
	if( !pGuild )					return -1;
	if( !IsGuildUnion( pGuild ) )	return -1;

	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		sGUILDIDXNAME& info = m_GuildInfo[ i ];

		if( info.dwGuildIdx == pGuild->GetIdx() )
		{
			ZeroMemory( &info, sizeof( info ) );
			
			pGuild->SetGuildUnionInfo( 0, "", 0, FALSE );
			return i;
		}		
	}
	return -1;
}

void CGuildUnion::GetGuildUnionInfo( SEND_GUILD_UNION_INFO* pMsg )
{
	pMsg->dwGuildUnionIdx = m_dwGuildUnionIdx;
	strncpy( pMsg->sGuildUnionName, m_sGuildUnionName, MAX_GUILD_NAME+1 );
	pMsg->dwGuildUnionMarkIdx = m_dwGuildUnionMarkIdx;
	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx )
			pMsg->AddGuildInfo( m_GuildInfo[i].dwGuildIdx, m_GuildInfo[i].sGuildName );
	}
}

void CGuildUnion::SetMarkIndex( DWORD dwMarkIdx )
{
	m_dwGuildUnionMarkIdx = dwMarkIdx;

	CGuild* pGuild = NULL;
	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx != 0 )
		{
			pGuild = GUILDMGR->GetGuild( m_GuildInfo[i].dwGuildIdx );
			if( pGuild )
				pGuild->SetUnionMarkIndex( dwMarkIdx );
		}
	}
}

void CGuildUnion::SendMsgToGuildUnionAll( MSGBASE* pMsg, int nSize )
{
	CGuild* pGuild = NULL;
	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx )
		{
			pGuild = GUILDMGR->GetGuild( m_GuildInfo[i].dwGuildIdx );
			if( pGuild )
				pGuild->SendMsgToAll( pMsg, nSize );
		}
	}
}

void CGuildUnion::GetGuildListInUnion( cPtrList* pList )
{
	CGuild* pGuild = NULL;

	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx )
		{
			pGuild = GUILDMGR->GetGuild( m_GuildInfo[i].dwGuildIdx );
			if( pGuild )			
				pList->AddTail( pGuild );
		}
	}
}


CGuild* CGuildUnion::GetMasterGuild()
{
	if( !m_GuildInfo[0].dwGuildIdx )		return NULL;

	return GUILDMGR->GetGuild( m_GuildInfo[0].dwGuildIdx );	
}

void CGuildUnion::SendAllNote(CPlayer *pPlayer, char* note)
{
	CGuild* pGuild = NULL;
	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( m_GuildInfo[i].dwGuildIdx )
		{
			pGuild = GUILDMGR->GetGuild( m_GuildInfo[i].dwGuildIdx );
            if( pGuild )
			{
				pGuild->SendUnionNote(pPlayer, note);
			}
		}
	}
}


DWORD CGuildUnion::GetSize() const
{
	int size = 0;

	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		const sGUILDIDXNAME& data = m_GuildInfo[ i ];

		if( data.dwGuildIdx )
		{
			++size;
		}
	}

	return size;
}
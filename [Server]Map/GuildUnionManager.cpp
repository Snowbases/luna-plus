#include "stdafx.h"
#include "GuildUnionManager.h"
#include "GuildUnion.h"
#include "Guild.h"
#include "GuildFieldWarMgr.h"
#include "GuildManager.h"
#include "GuildMark.h"
#include "MapDBMsgParser.h"
#include "Player.h"
#include "UserTable.h"
#include "Network.h"
#include "PackedData.h"


CGuildUnionManager::CGuildUnionManager()
{
	m_GuildUnionTable.Initialize( 200 );
	m_GuildUnionMarkTable.Initialize( 64 );
	m_GuildUnionTimeTable.Initialize( 200 );
}

CGuildUnionManager::~CGuildUnionManager()
{
	CGuildUnion* pData = NULL;
	m_GuildUnionTable.SetPositionHead();
	while( (pData = m_GuildUnionTable.GetData()) != NULL)
		delete pData;
	m_GuildUnionTable.RemoveAll();

	CGuildUnionMark* pMark = NULL;
	m_GuildUnionMarkTable.SetPositionHead();
	while( (pMark = m_GuildUnionMarkTable.GetData()) != NULL)
		delete pMark;
	m_GuildUnionMarkTable.RemoveAll();

	stTIME* pTime = NULL;
	m_GuildUnionTimeTable.SetPositionHead();
	while( (pTime = m_GuildUnionTimeTable.GetData()) != NULL)
		delete pTime;
	m_GuildUnionTimeTable.RemoveAll();
}

void CGuildUnionManager::LoadGuildUnion( DWORD dwUnionIdx, char* pName, DWORD dwGuild0, DWORD dwGuild1, DWORD dwGuild2, DWORD dwGuild3, DWORD dwGuild4, DWORD dwGuild5, DWORD dwGuild6, DWORD dwMarkIdx )
{
	if( !dwUnionIdx )	return;
	if( m_GuildUnionTable.GetData( dwUnionIdx ) ) return;

	CGuildUnion* pUnion = new CGuildUnion;
	pUnion->LoadUnionInfo( dwUnionIdx, pName, dwGuild0, dwGuild1, dwGuild2, dwGuild3, dwGuild4, dwGuild5, dwGuild6, dwMarkIdx );
	m_GuildUnionTable.Add( pUnion, dwUnionIdx );
}

void CGuildUnionManager::LoadGuildUnionMark( DWORD dwMarkIdx, DWORD dwGuildUnionIdx, char* pImgData )
{
	if( !dwMarkIdx )		return;
	if( !dwGuildUnionIdx )	return;

	CGuildUnion* pUnion = m_GuildUnionTable.GetData( dwGuildUnionIdx );
	if( !pUnion )	return;

	// 변환
	char Data[GUILDUNIONMARK_BUFSIZE];
	char tempBuf[3] = {0,};
	int curpos = 0;
	for( int n = 0; n < GUILDUNIONMARK_BUFSIZE; ++n )
	{
		strncpy( tempBuf, &pImgData[curpos], 2 ); // "FF"
		Data[n] = HexToByte( tempBuf );
		curpos += 2;
	}

	CGuildUnionMark* pMark = m_GuildUnionMarkTable.GetData( dwGuildUnionIdx );
	if( pMark )
		pMark->Init( dwMarkIdx, Data );
	else
	{
		pMark = new CGuildUnionMark;
		pMark->Init( dwMarkIdx, Data );
		m_GuildUnionMarkTable.Add( pMark, dwGuildUnionIdx );
	}
}

void CGuildUnionManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_GUILD_UNION_CREATE_SYN:				MsgGuildUnionCreateSyn( pMsg );				break;
	case MP_GUILD_UNION_CREATE_NOTIFY_TO_MAP:	MsgGuildUnionCreateNotifyToMap( pMsg );		break;	
	case MP_GUILD_UNION_DESTROY_SYN:			MsgGuildUnionDestroySyn( pMsg );			break;
	case MP_GUILD_UNION_DESTROY_NOTIFY_TO_MAP:	MsgGuildUnionDestroyNotifyToMap( pMsg );	break;
	case MP_GUILD_UNION_INVITE:					MsgGuildUnionInvite( pMsg );				break;
	case MP_GUILD_UNION_INVITE_ACCEPT:			MsgGuildUnionInviteAccept( pMsg );			break;
	case MP_GUILD_UNION_INVITE_ACCEPT_NOTIFY_TO_MAP:	MsgGuildUnionInviteAcceptNotifyToMap( pMsg );	break;	
	case MP_GUILD_UNION_INVITE_DENY:			MsgGuildUnionInviteDeny( pMsg );			break;
	case MP_GUILD_UNION_REMOVE_SYN:				MsgGuildUnionRemoveSyn( pMsg );				break;
	case MP_GUILD_UNION_REMOVE_NOTIFY_TO_MAP:	MsgGuildUnionRemoveNotifyToMap( pMsg );		break;
	case MP_GUILD_UNION_SECEDE_SYN:				MsgGuildUnionSecedeSyn( pMsg );				break;
	case MP_GUILD_UNION_SECEDE_NOTIFY_TO_MAP:	MsgGuildUnionSecedeNotifyToMap( pMsg );		break;
	case MP_GUILD_UNION_MARK_REGIST_SYN:		MsgGuildUnionMarkRegistSyn( pMsg );			break;
	case MP_GUILD_UNION_MARK_REGIST_NOTIFY_TO_MAP:	MsgGuildUnionMarkRegistNotifyToMap( pMsg );	break;
	case MP_GUILD_UNION_MARK_REQUEST_SYN:		MsgGuildUnionMarkRequestSyn( pMsg );		break;
	case MP_GUILD_UNION_NOTE_SYN:				MsgGuildUnionNoteSyn( pMsg ); break;
	}
}

void CGuildUnionManager::MsgGuildUnionCreateSyn( void* pMsg )
{
	MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if( !pPlayer )	return;

	// check condition
	CGuild* pGuild = GUILDMGR->GetGuild( pmsg->dwData );
	if( !pGuild )					
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_CREATE_NACK, eGU_Not_InGuild );
		return;
	}
	else if(pGuild->GetMasterIdx() != pmsg->dwObjectID)
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_CREATE_NACK, eGU_Not_GuildMaster );
		return;
	}
	else if( GUILDWARMGR->IsGuildWar( pGuild->GetIdx() ) )
	{
		// 정상적인 경우 클라이언트에서 검사한다
		return;
	}

	{
		const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( pGuild->GetLevel() );

		if( !	setting ||
				setting->mUnionSize == 0 )
		{
			SendNackMsg( pPlayer, MP_GUILD_UNION_CREATE_NACK, eGU_Not_Level );
			return;
		}
	}	

	if( pGuild->GetUnionIndex() )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_CREATE_NACK, eGU_Aleady_InGuildUnion );
		return;
	}
	DWORD dwTime = CheckEntryTime( pmsg->dwData );
	if( dwTime != 0 )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_CREATE_NACK, eGU_Time, dwTime );
		return;
	}
	if( !IsValidName( pmsg->Name ) )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_CREATE_NACK, eGU_Not_ValidName );
		return;
	}

	// db
	GuildUnionCreate( pmsg->dwObjectID, pmsg->dwData, pmsg->Name );	
}

void CGuildUnionManager::GuildUnionCreateResult( DWORD dwGuildUnionIdx, char* pGuildUnionName, DWORD dwMasterGuildIdx )
{
	if( !dwGuildUnionIdx )
	{
		return;
	}
	
	CGuildUnion* pGuildUnion = m_GuildUnionTable.GetData( dwGuildUnionIdx );

	if( pGuildUnion )
	{
		delete pGuildUnion;
		m_GuildUnionTable.Remove( dwGuildUnionIdx );
	}

	pGuildUnion = new CGuildUnion;
	pGuildUnion->CreateGuildUnion( dwGuildUnionIdx, pGuildUnionName, dwMasterGuildIdx );
	m_GuildUnionTable.Add( pGuildUnion, pGuildUnion->GetIndex() );

	// send msg
	MSG_NAME_DWORD2 message;
	message.Category = MP_GUILD_UNION;
	message.Protocol = MP_GUILD_UNION_CREATE_ACK;
	message.dwData1 = dwGuildUnionIdx;
	message.dwData2 = dwMasterGuildIdx;
	SafeStrCpy( message.Name, pGuildUnionName, sizeof( message.Name ) );
		
	// time
	ClearEntryTime( dwMasterGuildIdx );

	// to curmap
	pGuildUnion->SendMsgToGuildUnionAll( &message, sizeof(message) );	

	// to othermap
	message.Protocol = MP_GUILD_UNION_CREATE_NOTIFY_TO_MAP;
	g_Network.Send2AgentServer( (char*)&message, sizeof(message) );
	LogGuild( dwMasterGuildIdx, dwGuildUnionIdx, eGuildLog_CreateUnion );
}

void CGuildUnionManager::MsgGuildUnionCreateNotifyToMap( void* pMsg )
{
	MSG_NAME_DWORD2* pmsg = (MSG_NAME_DWORD2*)pMsg;

	CGuild* pGuild = GUILDMGR->GetGuild( pmsg->dwData2 );
	if( !pGuild )	return;

	CGuildUnion* pGuildUnion = m_GuildUnionTable.GetData( pmsg->dwData1 );
	if( pGuildUnion )
	{
		delete pGuildUnion;
		m_GuildUnionTable.Remove( pmsg->dwData1 );
	}
	pGuildUnion = new CGuildUnion;
	pGuildUnion->CreateGuildUnion( pmsg->dwData1, pmsg->Name, pmsg->dwData2 );
	m_GuildUnionTable.Add( pGuildUnion, pGuildUnion->GetIndex() );

	// time
	ClearEntryTime( pmsg->dwData2 );

	// to curmap
	pmsg->Protocol = MP_GUILD_UNION_CREATE_ACK;	
	pGuildUnion->SendMsgToGuildUnionAll( pmsg, sizeof(MSG_NAME_DWORD2) );
}

void CGuildUnionManager::MsgGuildUnionDestroySyn( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if( !pPlayer )	return;

	CGuild* pGuild = GUILDMGR->GetGuild( pPlayer->GetGuildIdx() );
	
	if( !pGuild )					
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_DESTROY_NACK, eGU_Not_InGuild );
		return;
	}
	else if(pGuild->GetMasterIdx() != pmsg->dwObjectID)
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_DESTROY_NACK, eGU_Not_GuildMaster );
		return;
	}

	CGuildUnion* pGuildUnion = m_GuildUnionTable.GetData( pmsg->dwData );

	if( !pGuildUnion )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_DESTROY_NACK, eGU_Not_InGuildUnion );
		return;
	}
	else if( pGuildUnion->GetMasterGuildIdx() != pGuild->GetIdx() )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_DESTROY_NACK, eGU_Not_GuildUnionMaster );
		return;
	}
	else if( 1 < pGuildUnion->GetSize() )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_DESTROY_NACK, eGU_No_Lone_Union );
		return;
	}
	else if( GUILDWARMGR->IsGuildWar( pGuildUnion->GetMasterGuildIdx() ) )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_DESTROY_NACK, eGU_Is_GuildFieldWar );
		return;
	}
	
	MSG_DWORD2 Msg1, Msg2;
	Msg1.Category = MP_GUILD_UNION;
	Msg1.Protocol = MP_GUILD_UNION_DESTROY_ACK;
	Msg1.dwData1 = pmsg->dwData;
	Msg1.dwData2 = 0;
	Msg2 = Msg1;
	
	// curmap
	pGuildUnion->SendMsgToGuildUnionAll( &Msg1, sizeof(Msg1) );
	
	// othermap
	Msg2.Protocol = MP_GUILD_UNION_DESTROY_NOTIFY_TO_MAP;
	g_Network.Send2AgentServer( (char*)&Msg2, sizeof(Msg2) );

	// time
	DWORD dwTime = AddEntryTimeForDestroy( pGuildUnion->GetMasterGuildIdx() );

	// dbupdate
	GuildUnionDestroy( pGuildUnion->GetIndex(), pGuildUnion->GetMarkIndex(), pGuildUnion->GetMasterGuildIdx(), dwTime );

	LogGuild( pGuildUnion->GetMasterGuildIdx(), pGuildUnion->GetIndex(), eGuildLog_DestroyUnion );

	// delete
	pGuildUnion->Destroy();
	delete pGuildUnion;
	m_GuildUnionTable.Remove( pmsg->dwData );

}
	
void CGuildUnionManager::MsgGuildUnionDestroyNotifyToMap( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	
	CGuildUnion* pGuildUnion = m_GuildUnionTable.GetData( pmsg->dwData1 );
	if( !pGuildUnion )	return;
	
	MSG_DWORD2 Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_DESTROY_ACK;
	Msg.dwData1 = pmsg->dwData1;
	Msg.dwData2 = 0;
	pGuildUnion->SendMsgToGuildUnionAll( &Msg, sizeof(Msg) );

	// time
	AddEntryTimeForDestroy( pGuildUnion->GetMasterGuildIdx() );

	pGuildUnion->Destroy();
	delete pGuildUnion;
	m_GuildUnionTable.Remove( pmsg->dwData1 );	
}

void CGuildUnionManager::MsgGuildUnionInvite( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CPlayer* masterPlayer = (CPlayer*)g_pUserTable->FindUser(
		pmsg->dwObjectID);
	
	if(0 == masterPlayer)
	{
		return;
	}
	else if(GUILD_MASTER != masterPlayer->GetGuildMemberRank())
	{
		return;
	}

	CPlayer* slavePlayer = (CPlayer*)g_pUserTable->FindUser(
		pmsg->dwData1);

	if(0 == slavePlayer)
	{
		return;
	}
	else if(GUILD_MASTER != slavePlayer->GetGuildMemberRank())
	{
		SendNackMsg(
			masterPlayer,
			MP_GUILD_UNION_INVITE_NACK,
			eGU_Other_Not_GuildMaster);
		return;
	}

	CGuild* const masterGuild = GUILDMGR->GetGuild(
		masterPlayer->GetGuildIdx());

	if(0 == masterGuild)
	{
		return;
	}

	CGuild* const slaveGuild = GUILDMGR->GetGuild(
		slavePlayer->GetGuildIdx());

	if(0 == slaveGuild)
	{
		return;
	}
	
	CGuildUnion* const pGuildUnion = GetUnion(
		masterPlayer->GetGuildUnionIdx());

	if(0 == pGuildUnion)
	{
		SendNackMsg(
			masterPlayer,
			MP_GUILD_UNION_INVITE_NACK,
			eGU_Not_InGuildUnion);
		return;
	}
	else if(masterGuild != pGuildUnion->GetMasterGuild())
	{
		SendNackMsg(
			masterPlayer,
			MP_GUILD_UNION_INVITE_NACK,
			eGU_Not_GuildUnionMaster);
		return;
	}
	
	const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting(
		masterGuild->GetLevel());

	if(0 == setting ||
		setting->mUnionSize == pGuildUnion->GetSize() ||
		FALSE == pGuildUnion->IsEmptyPosition())
	{
		SendNackMsg(
			masterPlayer,
			MP_GUILD_UNION_INVITE_NACK,
			eGU_Is_Full);
		return;
	}

	if(const DWORD dwTime = CheckEntryTime(slavePlayer->GetGuildIdx()))
	{
		SendNackMsg(
			masterPlayer,
			MP_GUILD_UNION_INVITE_NACK,
			eGU_Other_Time,
			dwTime);
		return;
	}
	else if(GUILDWARMGR->IsGuildWar(slavePlayer->GetGuildIdx(), masterPlayer->GetGuildIdx()))
	{
		SendNackMsg(
			masterPlayer,
			MP_GUILD_UNION_INVITE_NACK,
			eGU_Is_GuildFieldWar,
			0);
		return;
	}

	MSGBASE Wait;
	Wait.Category = MP_GUILD_UNION;
	Wait.Protocol = MP_GUILD_UNION_INVITE_WAIT;
	masterPlayer->SendMsg( &Wait, sizeof(Wait) );
	
	MSG_NAME_DWORD2 Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_INVITE;
	Msg.dwData1 = masterPlayer->GetID();
	Msg.dwData2 = pGuildUnion->GetIndex();
	strncpy( Msg.Name, pGuildUnion->GetName(), MAX_GUILD_NAME+1 );	
	slavePlayer->SendMsg( &Msg, sizeof(Msg) );
}

void CGuildUnionManager::MsgGuildUnionInviteAccept( void* pMsg )
{
	const MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	CGuild*	masterGuild	= GUILDMGR->GetGuildFromMasterIdx( pmsg->dwData1 );
	CGuild*	guestGuild	= GUILDMGR->GetGuildFromMasterIdx( pmsg->dwObjectID );

	if( !	masterGuild	||
		!	guestGuild )
	{
		return;
	}

	const DWORD		masterGuildIndex	= masterGuild->GetIdx();
	const DWORD		guestGuildIndex		= guestGuild->GetIdx();
	CGuildUnion*	guildUnion			= GetUnion( masterGuild->GetUnionIndex() );
	
	// 071009 웅주, 가입하려는 길드가 전쟁 중인 경우 가입할 수 없다.
	if( !	guildUnion						||
		!	guildUnion->IsEmptyPosition()	||
			GUILDWARMGR->IsGuildWar( guestGuildIndex ))
	{
		// 전쟁 중인 경우 요청 자체가 가지 않으므로, 해킹 시도이거나 검사 오류이다.
		return;
	}

	const int nIndex = guildUnion->AddGuild( guestGuild );

	if( nIndex == -1 )
	{
		return;
	}

	// time
	ClearEntryTime( guestGuildIndex );

	// 071009 웅주, 마스터 길드가 가진 적 목록을 해당 길드도 등록해야한다.
	GUILDWARMGR->AddUnionEnemyToGuild( guildUnion, guestGuild );

	// to curmap
	{
		SEND_GUILD_UNION_INFO message;
		message.Category = MP_GUILD_UNION;
		message.Protocol = MP_GUILD_UNION_ADD_ACK;

		guildUnion->GetGuildUnionInfo( &message );	
		guildUnion->SendMsgToGuildUnionAll( &message, message.GetSize() );	
	}
	
	// to othermap
	{
		MSG_DWORD2 message;
		message.Category	= MP_GUILD_UNION;
		message.Protocol	= MP_GUILD_UNION_INVITE_ACCEPT_NOTIFY_TO_MAP;
		message.dwData1		= guildUnion->GetIndex();
		message.dwData2		= guestGuildIndex;

		g_Network.Send2AgentServer( (char*)&message, sizeof(message) );
	}
	
	// dbupdate
	GuildUnionAddGuild( guildUnion->GetIndex(), nIndex, guestGuildIndex );
	LogGuild( masterGuildIndex, guildUnion->GetIndex(), eGuildLog_AddUnion, guestGuildIndex );
}

void CGuildUnionManager::MsgGuildUnionInviteAcceptNotifyToMap( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	CGuildUnion* pGuildUnion = m_GuildUnionTable.GetData( pmsg->dwData1 );
	if( !pGuildUnion )	return;	

	CGuild* guild = GUILDMGR->GetGuild( pmsg->dwData2 );

	int nIndex = pGuildUnion->AddGuild( guild );

	if( nIndex == -1 )	return;

	// time
	ClearEntryTime( pmsg->dwData2 );

	// to curmap
	SEND_GUILD_UNION_INFO Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_ADD_ACK;
	pGuildUnion->GetGuildUnionInfo( &Msg );	
	pGuildUnion->SendMsgToGuildUnionAll( &Msg, Msg.GetSize() );	
}

void CGuildUnionManager::MsgGuildUnionInviteDeny( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	CObject* pPlayer1 = g_pUserTable->FindUser( pmsg->dwObjectID );
	CObject* pPlayer2 = g_pUserTable->FindUser( pmsg->dwData1 );
	if( !pPlayer1 || !pPlayer2 )	return;

	MSG_DWORD Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_INVITE_DENY;
	Msg.dwData = pPlayer1->GetID();
	pPlayer2->SendMsg( &Msg, sizeof(Msg) );
}

void CGuildUnionManager::MsgGuildUnionRemoveSyn( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if( !pPlayer )	return;

	CGuild* pGuild1 = GUILDMGR->GetGuild( pPlayer->GetGuildIdx() );
	if( !pGuild1 )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_REMOVE_NACK, eGU_Not_InGuild );
		return;
	}
	else if(pGuild1->GetMasterIdx() != pmsg->dwObjectID)
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_REMOVE_NACK, eGU_Not_GuildMaster );
		return;
	}
	CGuildUnion* pGuildUnion = m_GuildUnionTable.GetData( pmsg->dwData1 );
	if( !pGuildUnion )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_REMOVE_NACK, eGU_Not_InGuildUnion );
		return;		
	}
	if( pGuildUnion->GetMasterGuildIdx() != pGuild1->GetIdx() )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_REMOVE_NACK, eGU_Not_GuildUnionMaster );
		return;
	}
	CGuild* pGuild2 = GUILDMGR->GetGuild( pmsg->dwData2 );
	if( !pGuild2 )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_REMOVE_NACK, eGU_Other_Not_InGuild );
		return;
	}
	if( !pGuildUnion->IsGuildUnion( pGuild2 ) )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_REMOVE_NACK, eGU_Other_Not_InGuildUnion );
		return;
	}

	int nIndex = pGuildUnion->RemoveGuild( pGuild2 );
	if( nIndex == -1 )	return;

	
	// 071010, 동맹이 가진 적 목록을 탈퇴하는 길드 목록에서 제거한다.
	GUILDWARMGR->RemoveUnionEnemyFromGuild( pGuildUnion, pGuild2 );

	// time
	ClearEntryTime( pGuild1->GetIdx() );

	// send msg to remove guild
	MSG_DWORD2 GMsg;
	GMsg.Category = MP_GUILD_UNION;
	GMsg.Protocol = MP_GUILD_UNION_DESTROY_ACK;
	GMsg.dwData1 = pmsg->dwData1;
	GMsg.dwData2 = 2;
	pGuild2->SendMsgToAll( &GMsg, sizeof(GMsg) );

	// to curmap
	SEND_GUILD_UNION_INFO Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_REMOVE_ACK;
	pGuildUnion->GetGuildUnionInfo( &Msg );	
	pGuildUnion->SendMsgToGuildUnionAll( &Msg, Msg.GetSize() );	

	// to othermap
	MSG_DWORD2 Msg2;
	Msg2.Category = MP_GUILD_UNION;
	Msg2.Protocol = MP_GUILD_UNION_REMOVE_NOTIFY_TO_MAP;
	Msg2.dwData1 = pmsg->dwData1;
	Msg2.dwData2 = pmsg->dwData2;
	g_Network.Send2AgentServer( (char*)&Msg2, sizeof(Msg2) );

	// dbupdate	
	GuildUnionRemoveGuild( pGuildUnion->GetIndex(), nIndex, pGuild2->GetIdx() );
	LogGuild( pGuildUnion->GetMasterGuildIdx(), pGuildUnion->GetIndex(), eGuildLog_RemoveUnion, pGuild2->GetIdx() );
}

void CGuildUnionManager::MsgGuildUnionRemoveNotifyToMap( void* pMsg )
{
	const MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	const DWORD		unionIndex	= pmsg->dwData1;
	const DWORD		guildIndex	= pmsg->dwData2;

	CGuild*			guild		= GUILDMGR->GetGuild( guildIndex );
	CGuildUnion*	guildUnion	= GetUnion( unionIndex );

	if( !	guild									||
		!	guildUnion								||
			guildUnion->GetMasterGuild() == guild	||
			guildUnion->GetIndex() != guild->GetUnionIndex() )
	{
		return;
	}

	const int nIndex = guildUnion->RemoveGuild( guild );

	if( nIndex == -1 )
	{
		return;
	}

	ClearEntryTime( guildIndex );

	{
		MSG_DWORD2 message;
		message.Category	= MP_GUILD_UNION;
		message.Protocol	= MP_GUILD_UNION_DESTROY_ACK;
		message.dwData1		= unionIndex;
		message.dwData2		= 2;
		guild->SendMsgToAll( &message, sizeof( message ) );
	}
	
	{
		SEND_GUILD_UNION_INFO message;
		message.Category = MP_GUILD_UNION;
		message.Protocol = MP_GUILD_UNION_REMOVE_ACK;
		guildUnion->GetGuildUnionInfo( &message );	
		guildUnion->SendMsgToGuildUnionAll( &message, message.GetSize() );	
	}
}

void CGuildUnionManager::MsgGuildUnionSecedeSyn( void* pMsg )
{
	const MSGBASE*	pmsg		= ( MSGBASE* )pMsg;
	const DWORD		playerIndex	= pmsg->dwObjectID;
	
	CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( playerIndex );

	if( ! player )
	{
		return;
	}

	const DWORD	guildIndex	= player->GetGuildIdx();
	const DWORD	unionIndex	= player->GetGuildUnionIdx();

	CGuildUnion* guildUnion = m_GuildUnionTable.GetData( unionIndex );

	if( !guildUnion )
	{
		SendNackMsg( player, MP_GUILD_UNION_SECEDE_NACK, eGU_Not_InGuildUnion );
		return;
	}	

	CGuild* guild = GUILDMGR->GetGuild( guildIndex );

	if( !guild )
	{
		SendNackMsg( player, MP_GUILD_UNION_SECEDE_NACK, eGU_Not_InGuild );
		return;
	}
	else if(guild->GetMasterIdx() != playerIndex)
	{
		SendNackMsg( player, MP_GUILD_UNION_SECEDE_NACK, eGU_Not_GuildMaster );
		return;
	}
	// 연합 마스터는 탈퇴 불능
	else if( guildUnion->GetMasterGuild() == guild )
	{
		return;
	}
	
	const int nIndex = guildUnion->RemoveGuild( guild );

	if( nIndex == -1 )
	{
		return;
	}

	{
		MSG_DWORD2 message;
		message.Category	= MP_GUILD_UNION;
		message.Protocol	= MP_GUILD_UNION_DESTROY_ACK;
		message.dwData1		= unionIndex;
		message.dwData2		= 1;
		guild->SendMsgToAll( &message, sizeof(message) );
	}
	
	{
		SEND_GUILD_UNION_INFO message;
		message.Category	= MP_GUILD_UNION;
		message.Protocol	= MP_GUILD_UNION_SECEDE_ACK;
		guildUnion->GetGuildUnionInfo( &message );	
		guildUnion->SendMsgToGuildUnionAll( &message, message.GetSize() );	
	}

	{
		MSG_DWORD2 message;
		message.Category	= MP_GUILD_UNION;
		message.Protocol	= MP_GUILD_UNION_SECEDE_NOTIFY_TO_MAP;
		message.dwData1		= unionIndex;
		message.dwData2		= guildIndex;
		g_Network.Send2AgentServer( ( char* )&message, sizeof( message ) );
	}
	
	{
		const DWORD time = AddEntryTimeForSecede( guildIndex );

		GuildUnionSecedeGuild( unionIndex, nIndex, guildIndex, time );
	}

	LogGuild( guildUnion->GetMasterGuildIdx(), unionIndex, eGuildLog_SecedeUnion, guildIndex );

	// 071010, 동맹이 가진 적 목록을 탈퇴하는 길드 목록에서 제거한다.
	GUILDWARMGR->RemoveUnionEnemyFromGuild( guildUnion, guild );
}

void CGuildUnionManager::MsgGuildUnionSecedeNotifyToMap( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	CGuildUnion* pGuildUnion = m_GuildUnionTable.GetData( pmsg->dwData1 );
	if( !pGuildUnion )	return;	

	DWORD dwGuildIdx = pmsg->dwData2;
	CGuild* pGuild = GUILDMGR->GetGuild( dwGuildIdx );
	if( !pGuild )	return;

	int nIndex = pGuildUnion->RemoveGuild( pGuild );
	if( nIndex == -1 )	return;

	// time
	AddEntryTimeForSecede( pGuild->GetIdx() );

	// send msg to secede guild
	MSG_DWORD2 GMsg;
	GMsg.Category = MP_GUILD_UNION;
	GMsg.Protocol = MP_GUILD_UNION_DESTROY_ACK;
	GMsg.dwData1 = pmsg->dwData1;
	GMsg.dwData2 = 1;
	pGuild->SendMsgToAll( &GMsg, sizeof(GMsg) );

	// to curmap
	SEND_GUILD_UNION_INFO Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_SECEDE_ACK;
	pGuildUnion->GetGuildUnionInfo( &Msg );	
	pGuildUnion->SendMsgToGuildUnionAll( &Msg, Msg.GetSize() );
}

void CGuildUnionManager::MsgGuildUnionMarkRegistSyn( void* pMsg )
{
	MSG_GUILDUNIONMARK_IMG* pmsg = (MSG_GUILDUNIONMARK_IMG*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if( !pPlayer )	return;

	CGuildUnion* pGuildUnion = m_GuildUnionTable.GetData( pmsg->dwGuildUnionIdx );
	if( !pGuildUnion )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_MARK_REGIST_NACK, eGU_Not_InGuildUnion );
		return;
	}	
	CGuild* pGuild = GUILDMGR->GetGuild( pPlayer->GetGuildIdx() );
	if( !pGuild )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_MARK_REGIST_NACK, eGU_Not_InGuild );
		return;
	}
	else if(pGuild->GetMasterIdx() != pmsg->dwObjectID)
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_MARK_REGIST_NACK, eGU_Not_GuildMaster );
		return;
	}
	else if( pGuildUnion->GetMasterGuildIdx() != pGuild->GetIdx() )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_MARK_REGIST_NACK, eGU_Not_GuildUnionMaster );
		return;
	}

	const BOOL isUpdate = (0 < pmsg->dwMarkIdx);

	GuildUnionMarkRegist(
		pmsg->dwObjectID,
		isUpdate,
		pmsg->dwGuildUnionIdx,
		pmsg->imgData);
}

void CGuildUnionManager::GuildUnionMarkRegistResult( DWORD dwCharacterIdx, DWORD dwMarkIdx, DWORD dwGuildUnionIdx, char* pImgData )
{
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( dwCharacterIdx );
	if( !pPlayer )	return;

	CGuildUnion* pUnion = m_GuildUnionTable.GetData( dwGuildUnionIdx );
	if( !pUnion )	return;
	pUnion->SetMarkIndex( dwMarkIdx );

	MSG_GUILDUNIONMARK_IMG Msg1, Msg2;
	char tempBuf[3] = {0,};
	int curpos = 0;
	for( int n = 0; n < GUILDUNIONMARK_BUFSIZE; ++n )
	{
		strncpy( tempBuf, &pImgData[curpos], 2 ); // "FF"
		Msg1.imgData[n] = HexToByte( tempBuf );
		curpos += 2;
	}	
	Msg2 = Msg1;

	// send to othermap
	Msg1.Category = MP_GUILD_UNION;
	Msg1.Protocol = MP_GUILD_UNION_MARK_REGIST_NOTIFY_TO_MAP;
	Msg1.dwGuildUnionIdx = dwGuildUnionIdx;
	Msg1.dwMarkIdx = dwMarkIdx;
	g_Network.Send2AgentServer( (char*)&Msg1, sizeof(Msg1) );

	// regist
	CGuildUnionMark* pMark = m_GuildUnionMarkTable.GetData( dwGuildUnionIdx );
	if( pMark )
		pMark->Init( dwMarkIdx, Msg2.imgData );
	else
	{
		pMark = new CGuildUnionMark;
		pMark->Init( dwMarkIdx, Msg2.imgData );
		m_GuildUnionMarkTable.Add( pMark, dwGuildUnionIdx );
	}

	// send to player within
	Msg2.Category = MP_GUILD_UNION;
	Msg2.Protocol = MP_GUILD_UNION_MARK_REGIST_ACK;
	Msg2.dwObjectID = dwCharacterIdx;
	Msg2.dwGuildUnionIdx = dwGuildUnionIdx;
	Msg2.dwMarkIdx = dwMarkIdx;
	PACKEDDATA_OBJ->QuickSend( pPlayer, &Msg2, sizeof(Msg2) );
}

void CGuildUnionManager::MsgGuildUnionMarkRegistNotifyToMap( void* pMsg )
{
	MSG_GUILDUNIONMARK_IMG* pmsg = (MSG_GUILDUNIONMARK_IMG*)pMsg;

	DWORD dwGuildUnionIdx = pmsg->dwGuildUnionIdx;
	DWORD dwMarkIdx = pmsg->dwMarkIdx;
	CGuildUnion* pUnion = m_GuildUnionTable.GetData( dwGuildUnionIdx );
	if( !pUnion )	return;
	pUnion->SetMarkIndex( dwMarkIdx );

	CGuildUnionMark* pMark = m_GuildUnionMarkTable.GetData( dwGuildUnionIdx );
	if( pMark )
		pMark->Init( dwMarkIdx, pmsg->imgData );
	else
	{
		pMark = new CGuildUnionMark;
		pMark->Init( dwMarkIdx, pmsg->imgData );
		m_GuildUnionMarkTable.Add( pMark, dwGuildUnionIdx );
	}
}

void CGuildUnionManager::MsgGuildUnionMarkRequestSyn( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
	if( !pPlayer )	return;
	CGuildUnion* pUnion = m_GuildUnionTable.GetData( pmsg->dwData1 );
	if( !pUnion )	return;
	CGuildUnionMark* pMark = m_GuildUnionMarkTable.GetData( pmsg->dwData1 );
	if( !pMark )	return;
	
	MSG_GUILDUNIONMARK_IMG Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_MARK_REQUEST_ACK;
	Msg.dwGuildUnionIdx = pmsg->dwData1;
	Msg.dwMarkIdx = pmsg->dwData2;
	memcpy( Msg.imgData, pMark->GetImgData(), GUILDUNIONMARK_BUFSIZE );
	pPlayer->SendMsg( &Msg, sizeof(Msg) );
}


void CGuildUnionManager::MsgGuildUnionNoteSyn( void* pMsg )
{
// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경
//  MSG_GUILD_SEND_NOTE -> MSG_GUILD_UNION_SEND_NOTE
//  msg->FromID -> msg->dwObjectID
//  msg->dwObjectID -> msg->dwGuildUnionIdx
	MSG_GUILD_UNION_SEND_NOTE* msg = (MSG_GUILD_UNION_SEND_NOTE*) pMsg;

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( msg->dwObjectID );
	if( !pPlayer )	return;

	CGuildUnion* pGuildUnion = m_GuildUnionTable.GetData( msg->dwGuildUnionIdx );
	if( !pGuildUnion )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_NOTE_NACK, eGU_Not_InGuildUnion );
		return;
	}	
	CGuild* pGuild = GUILDMGR->GetGuild( pPlayer->GetGuildIdx() );
	if( !pGuild )
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_NOTE_NACK, eGU_Not_InGuild );
		return;
	}
	else if(pGuild->GetMasterIdx() != msg->dwObjectID && !pGuild->IsViceMaster( msg->dwObjectID ))
	{
		SendNackMsg( pPlayer, MP_GUILD_UNION_NOTE_NACK, eGU_Not_GuildMaster );
		return;
	}

	pGuildUnion->SendAllNote(pPlayer, msg->Note);
	
	MSGBASE ack;
	ack.Category = MP_GUILD_UNION;
	ack.Protocol = MP_GUILD_UNION_NOTE_ACK;
	pPlayer->SendMsg(&ack, sizeof(ack));
}

void CGuildUnionManager::SendNackMsg( CPlayer* pPlayer, BYTE Protocol, DWORD dwErr, DWORD dwData /* = 0  */ )
{
	MSG_DWORD2 Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = Protocol;
	Msg.dwData1 = dwErr;
	Msg.dwData2 = dwData;
	pPlayer->SendMsg( &Msg, sizeof(Msg) );
}

void CGuildUnionManager::SendChatMsg( MSG_GUILDUNION_CHAT* pMsg )
{
// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경
//  MSG_CHAT_WITH_SENDERID -> MSG_GUILDUNION_CHAT
//  pMsg->dwSenderID -> pMsg->dwGuildUnionIdx
	CGuildUnion* pUnion = m_GuildUnionTable.GetData( pMsg->dwGuildUnionIdx );
	if( !pUnion )	return;

	MSG_GUILDUNION_CHAT Msg;
	Msg = *pMsg;
	pUnion->SendMsgToGuildUnionAll( &Msg, Msg.GetMsgLength() );
}

void CGuildUnionManager::AddPlayer( CPlayer* pPlayer, DWORD dwGuildUnionIdx )
{
	CGuildUnion* pUnion = m_GuildUnionTable.GetData( dwGuildUnionIdx );
	if( !pUnion )	return;

	SEND_GUILD_UNION_INFO Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_INFO;
	pUnion->GetGuildUnionInfo( &Msg );
	pPlayer->SendMsg( &Msg, Msg.GetSize() );

	pPlayer->SetGuildUnionInfo( dwGuildUnionIdx, pUnion->GetName(), pUnion->GetMarkIndex() );
}

void CGuildUnionManager::LoginPlayer( CPlayer* pPlayer, DWORD dwGuildUnionIdx )
{
	CGuildUnion* pUnion = m_GuildUnionTable.GetData( dwGuildUnionIdx );
	if( !pUnion )	return;

	SEND_GUILD_UNION_INFO Msg;
	Msg.Category = MP_GUILD_UNION;
	Msg.Protocol = MP_GUILD_UNION_INFO;
	pUnion->GetGuildUnionInfo( &Msg );
	pPlayer->SendMsg( &Msg, Msg.GetSize() );
}

void CGuildUnionManager::SetInfoToPlayer( CPlayer* pPlayer, DWORD dwGuildIdx )
{
	CGuild* pGuild = GUILDMGR->GetGuild( dwGuildIdx );

	if( ! pGuild  )
	{
		return;
	}

	CGuildUnion* pUnion = m_GuildUnionTable.GetData( pGuild->GetUnionIndex() );

	if( ! pUnion )
	{
		return;
	}

	pPlayer->InitGuildUnionInfo( pUnion->GetIndex(), pUnion->GetName(), pUnion->GetMarkIndex() );
}

BOOL CGuildUnionManager::IsValidName( const char* pName )
{
	CGuildUnion* pUnion = NULL;
	m_GuildUnionTable.SetPositionHead();
	while( (pUnion = m_GuildUnionTable.GetData()) != NULL)
	{
		if( strcmp( pUnion->GetName(), pName ) == 0 )
			return FALSE;
	}
	return strlen(pName) <= MAX_GUILD_NAME;
}

DWORD CGuildUnionManager::CheckEntryTime( DWORD dwGuildIdx )
{
	stTIME* pTime = m_GuildUnionTimeTable.GetData( dwGuildIdx );
	if( !pTime )	return 0;

	stTIME curTime;
	curTime.value = GetCurTime();

	if( curTime.value > pTime->value )
	{
		pTime->value = 0;
		return 0;
	}

	return pTime->value;
}

void CGuildUnionManager::ClearEntryTime( DWORD dwGuildIdx )
{
	stTIME* pTime = m_GuildUnionTimeTable.GetData( dwGuildIdx );
	if( !pTime )	return;
	pTime->value = 0;
}

void CGuildUnionManager::AddEntryTime( DWORD dwGuildIdx, DWORD dwTime )
{
	stTIME* pTime = m_GuildUnionTimeTable.GetData( dwGuildIdx );
	if( pTime )
		pTime->value = dwTime;
	else
	{
		pTime = new stTIME;
		pTime->value = dwTime;

		m_GuildUnionTimeTable.Add( pTime, dwGuildIdx );
	}
}

DWORD CGuildUnionManager::AddEntryTimeForDestroy( DWORD dwGuildIdx )
{
	stTIME time;
	time.SetTime( 0, 0, 5, 0, 0, 0 );

	stTIME* pTime = m_GuildUnionTimeTable.GetData( dwGuildIdx );
	if( pTime )
	{
		pTime->value = GetCurTime();
		*pTime += time;
	}
	else
	{
		pTime = new stTIME;
		pTime->value = GetCurTime();
		*pTime += time;

		m_GuildUnionTimeTable.Add( pTime, dwGuildIdx );
	}

	return pTime->value;		
}

DWORD CGuildUnionManager::AddEntryTimeForSecede( DWORD dwGuildIdx )
{
	stTIME time;
	time.SetTime( 0, 0, 1, 0, 0, 0 );

	stTIME* pTime = m_GuildUnionTimeTable.GetData( dwGuildIdx );
	if( pTime )
	{
		pTime->value = GetCurTime();
		*pTime += time;
	}
	else
	{
		pTime = new stTIME;
		pTime->value = GetCurTime();
		*pTime += time;

		m_GuildUnionTimeTable.Add( pTime, dwGuildIdx );
	}

	return pTime->value;	
}

//반드시 "FF" 이런식으로 입력이 되어야 한다.
BYTE CGuildUnionManager::HexToByte( char* pStr )
{
	int n1 = convertCharToInt(pStr[0]);
	int n2 = convertCharToInt(pStr[1]);

	return BYTE( n1 * 16 + n2 );
}

int CGuildUnionManager::convertCharToInt( char c )
{
	if('0' <= c && c <= '9')
		return c - '0';
	if('A' <= c && c <= 'F')
		return c - 'A' +10;
	if('a' <= c && c <= 'f')
		return c - 'a' +10;
	return 0;
}


//
BOOL CGuildUnionManager::GetGuildListInUnion( DWORD dwUnionIdx, cPtrList* pList )
{
	CGuildUnion* pUnion = m_GuildUnionTable.GetData( dwUnionIdx );
	if( !pUnion )			return FALSE;

	pUnion->GetGuildListInUnion( pList );
	return TRUE;
}


CGuild*	CGuildUnionManager::GetMasterGuildInUnion( DWORD dwUnionIdx )
{
	CGuildUnion* pUnion = m_GuildUnionTable.GetData( dwUnionIdx );
	if( !pUnion )			return NULL;

	return pUnion->GetMasterGuild();
}


CGuildUnion* CGuildUnionManager::GetUnion( DWORD unionIndex )
{
	return m_GuildUnionTable.GetData( unionIndex );
}
#include "stdafx.h"
#include "Guild.h"
#include "Player.h"
#include "UserTable.h"
#include "GuildManager.h"
#include "MapDBMsgParser.h"
#include "NetWork.h"
// 080417 LUJ, 길드 스킬위해 참조
#include "../[cc]skill/server/manager/skillmanager.h"
#include "../[cc]skill/Server/Info/SkillInfo.h"


CGuild::CGuild( const GUILDINFO& info, MONEYTYPE GuildMoney) :
m_GuildInfo( info ),
m_GTBattleID( 0 ),
m_nStudentCount( 0 ),
m_MarkName( info.MarkName ),
mPlayerInWarehouse(0)
{
	ZeroMemory( &m_RankMemberIdx, sizeof( m_RankMemberIdx ) );
	
	m_GuildWare.Init(0);
}


CGuild::~CGuild()
{
//KES delete추가 MEM
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	GUILDMEMBERINFO* pInfo = NULL;
	while(pos)
	{
		pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);
		SAFE_DELETE(pInfo);
	}
//-------
	//m_MemberList.RemoveAll();
}

BOOL CGuild::IsViceMaster(DWORD PlayerIDX)
{
	return m_RankMemberIdx[0] == PlayerIDX;
}

void CGuild::BreakUp()
{
// set member info clear
	CPlayer* pPlayer = NULL;
	MSGBASE msg;
	msg.Category	= MP_GUILD;
	msg.Protocol	= MP_GUILD_BREAKUP_ACK;
	
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	
	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);
		if( pInfo )
		{
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pInfo->MemberIdx);
			if(pPlayer)
			{
				pPlayer->SetGuildInfo(0, GUILD_NOTMEMBER, "", 0);
				pPlayer->SetNickName("");
				pPlayer->SendMsg(&msg, sizeof(msg));
				if(GetMarkName())
					GUILDMGR->MarkChange(pPlayer, 0, 0);
				else
					GUILDMGR->SendGuildName(pPlayer, 0, "");

				// 080417 LUJ, 보유한 길드 스킬을 갱신한다
				UpdateSkill( pInfo->MemberIdx );
			}
			
		}

		SAFE_DELETE(pInfo);
	}
	m_MemberList.RemoveAll();

	// 080417 LUJ, 보유 스킬을 초기화한다
	mSkillMap.clear();
}


BOOL CGuild::AddMember(GUILDMEMBERINFO* pMemberInfo)
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	GUILDMEMBERINFO* pInfo = NULL;
	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);

		if(pInfo == NULL) return FALSE;
 		if(pInfo->MemberIdx == pMemberInfo->MemberIdx)
 		{
 			return FALSE;
		}
	}
	
	pInfo = new GUILDMEMBERINFO;
	*pInfo = *pMemberInfo;
	
	m_MemberList.AddTail(pInfo);

	switch( pInfo->Rank )
	{
	case GUILD_VICEMASTER:
		{
			m_RankMemberIdx[0] = pInfo->MemberIdx;
			break;
		}
	case GUILD_SENIOR:
		{
			const int pos = IsVacancy(GUILD_SENIOR);

			if((pos <0) || (pos > eGRankPos_Max))
			{
				ASSERTMSG(0, "Load GuildMemberInfo - Set Ranked Member");
				return TRUE;
			}
			m_RankMemberIdx[pos] = pInfo->MemberIdx;

			break;
		}
	case GUILD_STUDENT:
		{
			++m_nStudentCount;
			break;
		}
	}

	// 080417 LUJ, 보유한 길드 스킬을 갱신한다
	UpdateSkill( pMemberInfo->MemberIdx );

	{
		SEND_GUILD_MEMBER_INFO message;
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_ADDMEMBER_ACK;
		message.GuildIdx	= GetIdx();
		message.MemberInfo	= *pMemberInfo;
	
		SendMsgToAllExceptOne(&message, sizeof(message), pMemberInfo->MemberIdx );
	}
	return TRUE;
}

BOOL CGuild::DeleteMember(DWORD PlayerIDX, BYTE bType)
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	
	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetAt(pos);
		if(pInfo)
		{
			if(pInfo->MemberIdx == PlayerIDX)
			{
				if(pInfo->Rank == GUILD_STUDENT)
				{
					--m_nStudentCount;
				}

				ResetRankMemberInfo(PlayerIDX, pInfo->Rank);
				SAFE_DELETE(pInfo);
				m_MemberList.RemoveAt(pos);

				// 080417 LUJ, 보유한 길드 스킬을 갱신한다
				UpdateSkill( PlayerIDX );
				return TRUE;
			}
		}
		m_MemberList.GetNext(pos);
	}

	return FALSE;
}

void CGuild::SendDeleteMsg(DWORD PlayerIDX, BYTE bType)
{
	MSG_DWORDBYTE msg;
	msg.Category	= MP_GUILD;
	msg.Protocol	= MP_GUILD_DELETEMEMBER_ACK;
	msg.dwData		= PlayerIDX;
	msg.bData		= bType;
	SendMsgToAll(&msg, sizeof(msg));
}


void CGuild::SendMsgToAll(MSGBASE* msg, int size)
{
	CPlayer* pPlayer = NULL;
	
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();

	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);

		if( pInfo )
		{
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pInfo->MemberIdx);
			if(pPlayer)
			{
				pPlayer->SendMsg(msg, size);
			}
		}
	}
}

void CGuild::SendMsgToAllExceptOne(MSGBASE* msg, int size, DWORD CharacterIdx)
{
	CPlayer* pPlayer = NULL;

	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);
		
		if( pInfo )
		{
			pPlayer = (CPlayer*)g_pUserTable->FindUser(pInfo->MemberIdx);
			if(pPlayer)
			{
				if(pPlayer->GetID() == CharacterIdx)
					continue;
				pPlayer->SendMsg(msg, size);
			}
		}
	}
}

BOOL CGuild::IsMember(DWORD MemberIDX)
{
	CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(MemberIDX);
	if(pMember)
	{
		if(pMember->GetGuildIdx() == GetIdx())
			return TRUE;
	}
	else
	{
		PTRLISTPOS pos = m_MemberList.GetHeadPosition();
		while(pos)
		{
			GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);

			if(pInfo)
			if(pInfo->MemberIdx == MemberIDX)
				return TRUE;
		}
	}
	return FALSE;
}

void CGuild::GetTotalMember(GUILDMEMBERINFO* pRtInfo)
{
	int i=0;

	for(
		PTRLISTPOS pos = m_MemberList.GetHeadPosition();
		pos;
		)
	{
		const GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);

		if( ! pInfo )
		{
			continue;
		}

		if( i == MAX_GUILD_MEMBER )
		{
			break;
		}

		pRtInfo[ i++ ] = *pInfo;
	}
}

BOOL CGuild::CanAddMember() const
{	
	const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting(GetLevel());
	const DWORD	size = ( setting ? setting->mMemberSize : 0 );

	return size > ( DWORD( m_MemberList.GetCount() ) - m_nStudentCount );
}

BOOL CGuild::ChangeMemberRank(DWORD MemberIdx, BYTE ToRank)
{
	{
		GUILDMEMBERINFO* pMemberInfo = GetMemberInfo(MemberIdx);
		if( ! pMemberInfo )
		{
			ASSERTMSG(0, "Change Rank - No Member Data");
			return FALSE;
		}
		
		ResetRankMemberInfo(MemberIdx, pMemberInfo->Rank);

		if( pMemberInfo->Rank	== GUILD_STUDENT &&
			ToRank				!= GUILD_STUDENT)
		{
			--m_nStudentCount;
		}
		
		pMemberInfo->Rank = ToRank;

		// 080417 LUJ, 보유한 길드 스킬을 갱신한다
		UpdateSkill( pMemberInfo->MemberIdx );
	}

	MSG_DWORDBYTE msg;
	msg.Category	= MP_GUILD;
	msg.Protocol	= MP_GUILD_CHANGERANK_ACK;
	msg.dwData		= MemberIdx;
	msg.bData		= ToRank;
	SendMsgToAll( &msg, sizeof( msg ) );

	return TRUE;
}

int CGuild::IsVacancy(BYTE ToRank)
{
	int end = 0;
	int start = 0;
	switch(ToRank)
	{
	case GUILD_VICEMASTER:
		{
			start = 0; end = 1;
		}
		break;
	case GUILD_SENIOR:
		{
			start = 1; end = 3;
		}
		break;
	case GUILD_MEMBER:
	case GUILD_STUDENT:
		return eGRankPos_Max;
		
	default:
		ASSERTMSG(0, "IsVancancy Err.");
		return eGRankPos_Err;
	}

	BOOL rt = eGRankPos_Err;
	for(int i=start; i<end; ++i)
	{
		if( m_RankMemberIdx[i] == 0 )
		{
			return i;
		}
	}
	return rt;
}

GUILDMEMBERINFO* CGuild::GetMemberInfo(DWORD MemberIdx)
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	GUILDMEMBERINFO* pInfo = NULL;

	while(pos)
	{
		pInfo = (GUILDMEMBERINFO*)m_MemberList.GetAt(pos);
		if(pInfo)
		{
			if(pInfo->MemberIdx == MemberIdx)
			{
				return pInfo;
			}
		}
		m_MemberList.GetNext(pos);
	}
	char buf[64];
	sprintf(buf, "ERR, NO MemberInfo. GuildIdx: %d, MemberIdx: %d", GetIdx(), MemberIdx);
	ASSERTMSG(0, buf);
	return NULL;
}

//BOOL CGuild::DoChangeMemberRank(DWORD MemberIdx, int Pos, BYTE Rank)
//{
//	GUILDMEMBERINFO* pMemberInfo = GetMemberInfo(MemberIdx);
//	if(pMemberInfo == 0)
//	{
//		ASSERTMSG(0, "Change Rank - No Member Data");
//		return FALSE;
//	}
//	if((Pos <0) || (Pos >eGRankPos_Max))
//	{
//		ASSERTMSG(0, "change rank pos boundary error");
//		return FALSE;
//	}
//	ResetRankMemberInfo(MemberIdx, pMemberInfo->Rank);
//
//	if(pMemberInfo->Rank == GUILD_STUDENT && Rank != GUILD_STUDENT)
//	{
//		--m_nStudentCount;
//	}
//	//없는경우이다.
//	//else if(pMemberInfo->Rank != GUILD_STUDENT && Rank == GUILD_STUDENT)
//	//{
//	//	++m_nStudentCount;
//	//}
//	pMemberInfo->Rank = Rank;
//	//if(Pos == eGRankPos_Max)
//	//	return TRUE;
//	//m_RankMemberIdx[Pos] = MemberIdx;
//	return TRUE;
//}

void CGuild::ResetRankMemberInfo(DWORD MemberIdx, BYTE Rank)
{
	switch(Rank)
	{
	case GUILD_VICEMASTER:
		m_RankMemberIdx[0] = 0; break;
	case GUILD_SENIOR:
		{
			if(m_RankMemberIdx[1] == MemberIdx)
				m_RankMemberIdx[1] = 0;
			else if(m_RankMemberIdx[2] == MemberIdx)
				m_RankMemberIdx[2] = 0;
			else 
				ASSERTMSG(0, "ResetRankMemberInfo() - No Rank");
		}
		break;
	}
}

void CGuild::InitGuildItem( const ITEMBASE& item )
{
	m_GuildWare.InitGuildItem( const_cast< ITEMBASE* >( &item ) );
}

CItemSlot* CGuild::GetSlot()
{
	return &m_GuildWare;
}

void CGuild::SetZeroMoney()
{
	CPurse* purse = m_GuildWare.GetPurse();
	if( purse )
		purse->SetZeroMoney();
}

void CGuild::SetMemberLevel(DWORD PlayerIdx, LEVELTYPE PlayerLvl)
{
	GUILDMEMBERINFO* pMemberInfo = GetMemberInfo(PlayerIdx);
	if( pMemberInfo == NULL )
	{
		return;
	}

	pMemberInfo->Memberlvl = PlayerLvl;
	MSG_DWORD2 msg;
	msg.Category = MP_GUILD;
	msg.Protocol = MP_GUILD_MEMBERLEVEL_NOTIFY;
	msg.dwData1 = PlayerIdx;
	msg.dwData2 = PlayerLvl;
	SendMsgToAll(&msg, sizeof(msg));
}

// 081031 LUJ, 로그인한 맵 번호도 저장하도록 한다.
void CGuild::SetLogInfo(DWORD PlayerIdx, BOOL vals, MAPTYPE mapType )
{
	GUILDMEMBERINFO* pMemberInfo = GetMemberInfo(PlayerIdx);
	if( pMemberInfo == NULL )
	{
		return;
	}

	pMemberInfo->bLogged	= vals;
	pMemberInfo->mMapType	= mapType;

	SEND_GUILDMEMBER_LOGININFO msg;
	msg.Category = MP_GUILD;
	msg.Protocol = MP_GUILD_LOGINFO;
	msg.MemberIdx = PlayerIdx;
	msg.bLogIn = vals;
	msg.mMapType = mapType;
	SendMsgToAllExceptOne(&msg, sizeof(msg), PlayerIdx);
}

void CGuild::SetGuildUnionInfo( DWORD index, char* name, DWORD markIndex, BOOL isMaster )
{
	m_GuildInfo.UnionIdx		= index;
	m_GuildInfo.mIsUnionMaster	= isMaster;

	PTRLISTPOS pos = m_MemberList.GetHeadPosition();

	for( const GUILDMEMBERINFO* pInfo; (pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos))!=NULL; )
	{
		CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pInfo->MemberIdx );

		if( pPlayer )
		{
			pPlayer->SetGuildUnionInfo( index, name, markIndex );
		}
	}
}

void CGuild::SetUnionMarkIndex( DWORD dwMarkIdx )
{
	CPlayer* pPlayer = NULL;
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();

	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);

		if( pInfo )
		{
			pPlayer = (CPlayer*)g_pUserTable->FindUser( pInfo->MemberIdx );
			if( pPlayer )
				pPlayer->SetGuildUnionMarkIdx( dwMarkIdx );
		}
	}
}

void CGuild::SendAllNote(CPlayer *pPlayer, char* note)
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);
		
		if( pInfo )
		{
			SendNote(pPlayer, pInfo->MemberName, note);
		}
	}
}

void CGuild::SendUnionNote(CPlayer *pPlayer, char* note)
{
	SendNote(pPlayer, m_GuildInfo.MasterName, note);

	if(m_RankMemberIdx[0])
	{
		GUILDMEMBERINFO* pInfo = GetMemberInfo( m_RankMemberIdx[0] );
		SendNote(pPlayer, pInfo->MemberName, note);
	}
}

void CGuild::SendNote(CPlayer *pPlayer, char* toName, char* note)
{
	MSG_FRIEND_SEND_NOTE msg;
	msg.Category = MP_NOTE;
	msg.Protocol = MP_NOTE_SENDNOTE_SYN;
	msg.FromId = pPlayer->GetID();
	
	SafeStrCpy(msg.FromName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
	SafeStrCpy(msg.ToName, toName, MAX_NAME_LENGTH+1);
	SafeStrCpy(msg.Note, note, MAX_NOTE_LENGTH+1);	

	g_Network.Broadcast2AgentServer((char*)&msg, msg.GetMsgLength());
}

BOOL CGuild::CanAddStudent() const
{
	const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting(
		GetLevel());
	const DWORD	size = ( setting ? setting->mApprenticeSize : 0 );

	return size > m_nStudentCount;
}

void CGuild::ClearItem()
{
	m_GuildWare.Init( 0 );
}


// 080225 LUJ, 회원 정보를 갱신한다
BOOL CGuild::SetMember( const GUILDMEMBERINFO& member )
{
	GUILDMEMBERINFO* storedMember = GetMemberInfo( member.MemberIdx );

	if( ! storedMember )
	{
		return FALSE;
	}

	*storedMember = member;

	return TRUE;
}

// 080417 LUJ
void CGuild::AddSkill( DWORD index, LEVELTYPE level )
{	
	mSkillMap[ index ] = level;

	// 080417 LUJ,	길드원에게 세트 스킬로 세팅한다. 세트 스킬은 플레이어가 임시로 보유하는 스킬을 관리하는데 최적화되어 있기 때문
	//
	//				주의: 길드 스킬과 세트 스킬이 겹치면 안된다
	for(
		PTRLISTPOS position = m_MemberList.GetHeadPosition();
		position; )
	{
		const GUILDMEMBERINFO* info	= ( GUILDMEMBERINFO* )	m_MemberList.GetNext( position );
		
		UpdateSkill( info ? info->MemberIdx : 0 );
	}
}

// 080417 LUJ
void CGuild::RemoveSkill( DWORD index )
{
	mSkillMap.erase( index );	

	// 080417 LUJ, 길드원에게 스킬을 뺏는다
	for(
		PTRLISTPOS position = m_MemberList.GetHeadPosition();
		position; 
		)
	{
		const GUILDMEMBERINFO* info	= ( GUILDMEMBERINFO* )	m_MemberList.GetNext( position );

		UpdateSkill( info ? info->MemberIdx : 0 );
	}
}

// 080417 LUJ, 해당 플레이어의 길드 스킬을 갱신한다
void CGuild::UpdateSkill( DWORD playerIndex )
{
	CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( playerIndex );

	if( ! player )
	{
		return;
	}

	typedef CGuildManager::SkillSettingMap SkillSettingMap;
	const SkillSettingMap* settingMap = GUILDMGR->GetSkillSetting( GetLevel() );

	if( ! settingMap )
	{
		settingMap = GUILDMGR->GetMaxLevelSkillSetting();

		if( ! settingMap )
		{
			return;
		}
	}

	for(
		SkillMap::const_iterator it = mSkillMap.begin();
		mSkillMap.end() != it;
		++it )
	{
		player->RemoveSetSkill(
			it->first,
			it->second);
	}

	const GUILDMEMBERINFO* member = GetMemberInfo( playerIndex );
	
	if( ! member )
	{
		return;
	}

	// 080417 LUJ, 해당 길드 스킬에 대할 설정 정보를 꺼내서 부여 조건에 해당하는지 체크한다
	for(
		SkillMap::const_iterator it = mSkillMap.begin();
		mSkillMap.end() != it;
		++it )
	{
		const DWORD index = it->first;
		const LEVELTYPE	level = it->second;

		SkillSettingMap::const_iterator setting_it = settingMap->find( index );

		if( settingMap->end() == setting_it )
		{
			continue;
		}

		const CGuildManager::SkillSetting& setting = setting_it->second;

		if(	setting.mRank.end() != setting.mRank.find( member->Rank ) )
		{
			player->AddSetSkill( index, level );
		}
	}
}

// 080417 LUJ, 길드가 보유한 스킬을 해당 플레이어에게 전송한다
void CGuild::SendSkill( CObject& object ) const
{
	MSG_SKILL_LIST message;
	{
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_GET_SKILL_ACK;
		message.mSize		= 0;
	}

	for(
		SkillMap::const_iterator it = mSkillMap.begin();
		mSkillMap.end() != it;
		++it )
	{
		SKILL_BASE& skill = message.mData[ message.mSize++ ];

		skill.wSkillIdx = it->first;
		skill.Level		= it->second;
	}
	
	object.SendMsg( &message, message.GetSize() );
}

LEVELTYPE CGuild::GetSkillLevel( DWORD index ) const
{
	SkillMap::const_iterator it = mSkillMap.find( index );

	return mSkillMap.end() == it ? 0 : it->second;
}

void CGuild::SendGTRewardToAll(DWORD gtcount, DWORD itemIdx, DWORD count, BOOL isStack, DWORD endtime)
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);
		if(pInfo)
			GTRewardItemInsert(pInfo->MemberIdx, gtcount, itemIdx, count, isStack, endtime);
	}
}
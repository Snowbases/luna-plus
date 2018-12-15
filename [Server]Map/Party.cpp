// Party.cpp: implementation of the CParty class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Party.h"
#include "UserTable.h"
#include "Network.h"
#include "Dungeon/DungeonMgr.h"
#include "Player.h"
#include "PartyManager.h"

CParty::CParty()
{
	m_MasterChanging = FALSE;
	m_Option = 0;
	ZeroMemory(
		m_Member,
		sizeof(m_Member));
	m_byWinnerIdx = 0;
	m_DiceGrade = 0;
	// 090525 ShinJS --- 파티 인원수 초기화
	m_dwPartyMemberNum = 0;
}

CParty::~CParty()
{

}

void CParty::Init(DWORD id,DWORD MasterID,char* strMasterName,CPlayer* pMaster,BYTE Option, BYTE DiceGrade, LEVELTYPE level)
{
	m_PartyIDx = id;
	PARTY_MEMBER& member = m_Member[0];
	member.dwMemberID = MasterID;
	member.Level = level;
	SafeStrCpy(
		member.Name,
		strMasterName,
		sizeof(member.Name) / sizeof(*member.Name));
	m_MasterChanging = FALSE;
	m_Option = Option;
	m_DiceGrade = DiceGrade;
	// 090525 ShinJS --- 파티 인원수 추가
	m_dwPartyMemberNum = 1;
}

void CParty::SetMasterTipInfo( MAPTYPE MapType, BYTE JobGrade, BYTE* Job, BYTE Race )
{
	PARTY_MEMBER& member = m_Member[0];
	member.bLogged = TRUE;
	member.mMapType = MapType;
	member.mJobGrade = JobGrade;
	member.mRace = Race;
	memcpy(
		member.mJob,
		Job,
		sizeof(member.mJob));
}


void CParty::SetMaster(CPlayer* pMaster)
{
	// 파티 주인 정보를 설정한다
	{
		PARTY_MEMBER& member = m_Member[0];
		member.dwMemberID = pMaster->GetID();
		member.bLogged = TRUE;
		member.Level = pMaster->GetLevel();
		SafeStrCpy(
			member.Name,
			pMaster->GetObjectName(),
			sizeof(member.Name) / sizeof(*member.Name));
		member = GetMember(0);
	}

	// 091214 LUJ, 중복되어있을 수 있으므로 삭제한다
	for(DWORD i = 1; i < MAX_PARTY_LISTNUM; ++i)
	{
		PARTY_MEMBER& member = m_Member[i];

		if(member.dwMemberID == pMaster->GetID())
		{
			ZeroMemory(
				&member,
				sizeof(member));
			break;
		}
	}
}

void CParty::SendPartyInfo(CPlayer* pToPlayer)
{
	PARTY_INFO msg;
	ZeroMemory(
		&msg,
		sizeof(msg));
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_INFO;
	msg.PartyDBIdx = m_PartyIDx;
	msg.Option = GetOption();
	msg.SubOption = GetDiceGradeOpt();

	for( int memberIndex = 0; memberIndex < MAX_PARTY_LISTNUM; ++memberIndex )
	{
		// 100804 ONS 채널이 다를경우 파티원정보(HP,MP,아이콘)를 표시하지 않도록 수정. 
		PARTY_MEMBER& member = m_Member[memberIndex];

		CPlayer* const pPlayer = (CPlayer*)g_pUserTable->GetData(member.dwMemberID);
		if( pPlayer	)
		{
			if( pToPlayer->GetGridID() == pPlayer->GetGridID() )
			{
				pPlayer->GetPosition(&member.mPosition);
				member.LifePercent = pPlayer->GetLifePercent();
				member.ManaPercent = pPlayer->GetManaPercent();
				const CHARACTER_TOTALINFO& totalInfo = pPlayer->GetCharacterTotalInfo();
				member.mRace = totalInfo.Race;
				member.mJobGrade = totalInfo.JobGrade;
				memcpy(
					member.mJob,
					totalInfo.Job,
					sizeof(member.mJob));
			}
			else
			{
				member.LifePercent = 0;
				member.ManaPercent = 0;
			}
		}

		msg.Member[memberIndex] = member;
	}

	pToPlayer->SendMsg(&msg,sizeof(msg));
}

void CParty::AddPartyMember(DWORD AddMemberID,LPCTSTR Name,CPlayer* pPlayer, LEVELTYPE lvl)
{
	if(FALSE == IsPartyMember(AddMemberID))
	{
		for(int n = 1; n < MAX_PARTY_LISTNUM; ++n)
		{
			PARTY_MEMBER& member = m_Member[n];

			if(0 != member.dwMemberID)
			{
				continue;
			}

			member.dwMemberID = AddMemberID;
			member.Level = lvl;
			SafeStrCpy(
				member.Name,
				Name,
				sizeof(member.Name) / sizeof(*member.Name));
			++m_dwPartyMemberNum;
			break;
		}
	}

	if(pPlayer)
	{
		SendPartyInfo(pPlayer);
	}
}

void CParty::RemovePartyMember(DWORD MemberID) 
{
	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		PARTY_MEMBER& member = m_Member[i];

		if(member.dwMemberID != MemberID)
		{
			continue;
		}

		ZeroMemory(
			&member,
			sizeof(member));
		--m_dwPartyMemberNum;
		DungeonMGR->AddNoPartyList(MemberID, GetPartyIdx());
	}
} 

BOOL CParty::ChangeMaster(DWORD FromID,DWORD ToID)
{
	for(DWORD n = 1; n < MAX_PARTY_LISTNUM; ++n) 
	{
		if(m_Member[n].dwMemberID == ToID) 
		{
			PARTY_MEMBER backupMember = m_Member[0];
			m_Member[0] = m_Member[n];
			m_Member[n] = backupMember;
			return TRUE;
		} //if
	} // for
	
	return FALSE;
} 

BOOL CParty::IsPartyMember(DWORD PlayerID)
{
	for(DWORD n = 0; n < MAX_PARTY_LISTNUM; ++n) 
	{
		if(m_Member[n].dwMemberID == PlayerID)
		{
			 return TRUE;
		}
	}

	return FALSE;
}

BOOL CParty::IsMemberLogIn(int n)
{
	return m_Member[n].bLogged;
}

void CParty::UserLogIn(CPlayer* pPlayer,BOOL bNotifyUserLogin) 
{
	DWORD memberIndex = MAX_PARTY_LISTNUM;

	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		PARTY_MEMBER& member = m_Member[i];

		if(pPlayer->GetID() != member.dwMemberID)
		{
			continue;
		}

		member.Level = pPlayer->GetLevel();
		member.mMapType = g_pServerSystem->GetMapNum();
		member.bLogged = TRUE;
		SafeStrCpy(
			member.Name,
			pPlayer->GetObjectName(),
			sizeof(member.Name) / sizeof(*member.Name));
		const CHARACTER_TOTALINFO& totalInfo = pPlayer->GetCharacterTotalInfo();
		member.mJobGrade = totalInfo.JobGrade;
		member.mRace = totalInfo.Race;
		memcpy(
			member.mJob,
			totalInfo.Job,
			sizeof(member.mJob));
		memberIndex = i;
		SendPartyInfo(pPlayer);
		break;
	}

	if(bNotifyUserLogin)
	{
		SendPlayerInfoToOtherMembers(
			memberIndex,
			pPlayer->GetGridID());

		if(! PARTYMGR->CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
		{
			SEND_PARTY_MEMBER_INFO cmsg;
			ZeroMemory(&cmsg, sizeof(cmsg));
			cmsg.Category = MP_PARTY;
			cmsg.Protocol = MP_PARTY_NOTIFYMEMBER_LOGIN_TO_MAPSERVER;
			cmsg.dwObjectID = pPlayer->GetID();
			cmsg.PartyID = pPlayer->GetPartyIdx();
			cmsg.mIsMaster = (GetMasterID() == pPlayer->GetID());
			cmsg.MemberInfo = GetMember(memberIndex);
			g_Network.Send2AgentServer(
				LPTSTR(&cmsg),
				sizeof(cmsg));
		}
	}
}

void CParty::NotifyUserLogIn(SEND_PARTY_MEMBER_INFO* pmsg)
{
	const PARTY_MEMBER& loginMember = pmsg->MemberInfo;

	BOOL bFirstConnMember = TRUE;
	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		if(0 != m_Member[i].dwMemberID)
		{
			bFirstConnMember = FALSE;
			break;
		}
	}

	if(bFirstConnMember && pmsg->mIsMaster)
	{
		// 마스터가 처음접속했다.
		m_Member[0] = pmsg->MemberInfo;
		return;
	}

	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		PARTY_MEMBER& member = m_Member[i];

		if(member.dwMemberID != loginMember.dwMemberID)
		{
			continue;
		}

		member = pmsg->MemberInfo;
		CObject* const object = g_pUserTable->FindUser(loginMember.dwMemberID);

		SendPlayerInfoToOtherMembers(
			i,
			object ? object->GetGridID() : 0);
		break;
	}
}

void CParty::UserLogOut(DWORD PlayerID) 
{
	for(int n=0;n<MAX_PARTY_LISTNUM;++n)
	{
		if(m_Member[n].dwMemberID == PlayerID)
		{
			m_Member[n].bLogged = FALSE;
			m_Member[n].LifePercent = 0;
			m_Member[n].ManaPercent = 0;
			m_Member[n].mMapType = 0;
		}
	}
}

void CParty::SendMsgUserLogOut(DWORD PlayerID)
{
	// 로그아웃한 유저의 정보를 다른 파티원에게 보내준다.
	MSG_DWORD msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_MEMBER_LOGOUT;
	msg.dwData = PlayerID;
	SendMsgToAll(&msg,sizeof(msg));
}

void CParty::SendMsgToAll(MSGBASE* msg, int size) 
{
	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		CObject* const object = g_pUserTable->FindUser(m_Member[i].dwMemberID);

		if(0 == object)
		{
			continue;
		}

		object->SendMsg(
			msg,
			size);
	}
} 

void CParty::SendMsgExceptOne(MSGBASE* msg, int size, DWORD PlayerID) 
{
	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		if(m_Member[i].dwMemberID == PlayerID)
		{
			continue;
		}

		CObject* const object = g_pUserTable->FindUser(m_Member[i].dwMemberID);

		if(0 == object)
		{
			continue;
		}

		object->SendMsg(
			msg,
			size);
	}
}

void CParty::SendMsgExceptOneinChannel(MSGBASE* msg, int size, DWORD PlayerID, DWORD GridID)
{
	for(DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i)
	{
		if(m_Member[i].dwMemberID == PlayerID)
		{
			continue;
		}

		CObject* const object = g_pUserTable->FindUser(m_Member[i].dwMemberID);

		if(0 == object)
		{
			continue;
		}
		else if(object->GetGridID() != GridID)
		{
			continue;
		}

		object->SendMsg(
			msg,
			size);
	}
}

void CParty::BreakUp()
{
	for(int i=0; i<MAX_PARTY_LISTNUM; ++i) 
	{
		CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(m_Member[i].dwMemberID);

		if(0 == pPlayer)
		{
			continue;
		}

		pPlayer->SetPartyIdx(0);
		DungeonMGR->AddNoPartyList(
			pPlayer->GetID(),
			GetPartyIdx());
	}

	ZeroMemory(
		m_Member,
		sizeof(m_Member));
}

const PARTY_MEMBER& CParty::GetMember(DWORD memberIndex)
{
	if(MAX_PARTY_LISTNUM <= memberIndex)
	{
		static const PARTY_MEMBER emptyMember = {0};
		return emptyMember;
	}

	PARTY_MEMBER& member = m_Member[memberIndex];
	CPlayer* const player = (CPlayer*)g_pUserTable->GetData(member.dwMemberID);

	if(player)
	{
		player->GetPosition(&member.mPosition);
		member.LifePercent = player->GetLifePercent();
		member.ManaPercent = player->GetManaPercent();
		const CHARACTER_TOTALINFO& totalInfo = player->GetCharacterTotalInfo();
		member.mRace = totalInfo.Race;
		member.mJobGrade = totalInfo.JobGrade;
		memcpy(
			member.mJob,
			totalInfo.Job,
			sizeof(member.mJob));
	}

	return member;
}

char* CParty::GetMemberName(DWORD MemberID)
{
	for(int i=0; i<MAX_PARTY_LISTNUM; ++i) 
	{
		if(m_Member[i].dwMemberID == MemberID) 
		{
			return m_Member[i].Name;
		} //if
	} //for
//	ASSERTMSG(0,"맴버가 아닌 사람의 아이디를 찾으려 했습니다.");
	return "";
}

void CParty::SendMemberLogInMsg(DWORD PlayerID)
{
	MSG_DWORD msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_MEMBER_LOGINMSG;
	msg.dwData = PlayerID;
	SendMsgToAll(&msg, sizeof(msg)); 
}

void CParty::NotifyMemberLogin(DWORD PlayerID)
{
	if(PARTYMGR->CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
		return;

	MSG_DWORD bmsg;
	bmsg.Category = MP_PARTY;
	bmsg.Protocol = MP_PARTY_NOTIFYMEMBER_LOGINMSG;
	bmsg.dwObjectID = PlayerID;
	bmsg.dwData = GetPartyIdx();
	g_Network.Send2AgentServer((char*)&bmsg, sizeof(bmsg));
}

// 071002 LYW --- Party : Add function to notice to members that changed option of party.
void CParty::NotifyChangedOption(DWORD dwOption)
{
	if(PARTYMGR->CanUseInstantPartyMap(g_pServerSystem->GetMapNum()))
		return;

	MSG_DWORD2 msg ;
	msg.Category = MP_PARTY ;
	msg.Protocol = MP_PARTY_NOTIFY_CHANGE_OPTION ;
	msg.dwObjectID = GetMasterID() ;
	msg.dwData1 = GetPartyIdx() ;
	msg.dwData2 = dwOption ;
	g_Network.Send2AgentServer((char*)&msg, sizeof(msg));
}

int CParty::SetMemberInfo(DWORD MemberID, char* strName, CPlayer* pPlayer, LEVELTYPE lvl, BOOL bLog, MAPTYPE mapType)
{
	for(int n=0;n<MAX_PARTY_LISTNUM;++n)
	{
		PARTY_MEMBER& member = m_Member[n];

		if(member.dwMemberID == MemberID)
		{
			member.dwMemberID = MemberID;
			member.Level = lvl;
			member.bLogged = bLog;
			member.mMapType = mapType;
			SafeStrCpy(
				member.Name,
				strName,
				sizeof(member.Name) / sizeof(*member.Name));
			return n;
		}
	}
	ASSERT( n < MAX_PARTY_LISTNUM );
	return -1;
}

void CParty::SetMember(DWORD index, const PARTY_MEMBER& member)
{
	if(MAX_PARTY_LISTNUM <= index)
	{
		return;
	}

	m_Member[index] = member;
}

void CParty::SetMemberInfo( BYTE nPos, DWORD MemberID, char* strName, LEVELTYPE lvl, MAPTYPE map)
{
	if( nPos >= MAX_PARTY_LISTNUM )
		return;

	PARTY_MEMBER& member = m_Member[nPos];
	member.dwMemberID = MemberID;
	member.Level = lvl;
	member.bLogged = map ? TRUE : FALSE;
	member.mMapType = map;
	SafeStrCpy(
		member.Name,
		strName,
		sizeof(member.Name) / sizeof(*member.Name));
}

void CParty::SendPlayerInfoToOtherMembers(int n, DWORD GridID)
{
	SEND_PARTY_MEMBER_INFO msg;
	ZeroMemory(&msg, sizeof(msg));
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_MEMBER_LOGIN;
	msg.MemberInfo = GetMember(n);
	msg.PartyID = GetPartyIdx();
	msg.mIsMaster = (GetMasterID() == msg.MemberInfo.dwMemberID);
	SendMsgLoginMemberInfo(&msg, GridID);
}

void CParty::SendMsgLoginMemberInfo(SEND_PARTY_MEMBER_INFO* msg, DWORD GridID)
{
	for(int i=0; i<MAX_PARTY_LISTNUM; ++i) 
	{
		CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(m_Member[i].dwMemberID);

		if(0 == pPlayer)
		{
			continue;
		}
		else if(pPlayer->GetGridID() != GridID)
		{
			msg->MemberInfo.LifePercent = 0;
			msg->MemberInfo.ManaPercent = 0;
		}
		else
		{
			msg->MemberInfo.LifePercent = pPlayer->GetLifePercent();
			msg->MemberInfo.ManaPercent = pPlayer->GetManaPercent();
		}

		pPlayer->SendMsg(msg, sizeof(*msg));
	}
}

void CParty::SetMasterChanging(BOOL val)
{
	m_MasterChanging = val;
}

BOOL CParty::IsMasterChanging(CPlayer* pPlayer, BYTE Protocol)
{
	if(m_MasterChanging == TRUE)
	{
		MSG_INT msg;
		msg.Category = MP_PARTY;
		msg.Protocol = Protocol;
		pPlayer->SendMsg(&msg, sizeof(msg));
		return TRUE;
	}
	return FALSE;
}

void CParty::SetMemberLevel(DWORD PlayerID, LEVELTYPE lvl)
{
	for(int n=0;n<MAX_PARTY_LISTNUM;++n)
	{
		if(m_Member[n].dwMemberID == PlayerID)
		{
			m_Member[n].Level = lvl;
			break;
		}
	}
}

void CParty::SendMemberPos(DWORD MoverID, BASEMOVE_INFO* pMoveInfo)
{
	SEND_PARTYICON_MOVEINFO msg;
	msg.Category = MP_PARTY;
	msg.Protocol = MP_PARTY_SENDPOS;
	msg.MoveInfo.dwMoverID = MoverID;
	msg.MoveInfo.tgCount = pMoveInfo->GetMaxTargetPosIdx();
	msg.MoveInfo.cPos.posX = (WORD)pMoveInfo->CurPosition.x;
	msg.MoveInfo.cPos.posZ = (WORD)pMoveInfo->CurPosition.z;
	msg.MoveInfo.KyungGongIdx = pMoveInfo->KyungGongIdx;


	CPlayer* pMover = (CPlayer*)g_pUserTable->FindUser(MoverID);
	if(pMover)
	{
		msg.MoveInfo.fSpeed = pMover->GetMoveSpeed();
	}
	
	for(BYTE i=0; i<msg.MoveInfo.tgCount; i++)
	{
		VECTOR3* pTgPos = pMoveInfo->GetTargetPosition(i);
		msg.MoveInfo.tgPos[i].posX = (WORD)pTgPos->x;
		msg.MoveInfo.tgPos[i].posZ = (WORD)pTgPos->z;
	}
	
	for(i=0; i<MAX_PARTY_LISTNUM; i++)
	{
		if(m_Member[i].dwMemberID == MoverID)
		{
			continue;
		}

		CObject* const object = g_pUserTable->FindUser(m_Member[i].dwMemberID);

		if(0 == object)
		{
			continue;
		}

		object->SendMsg(
			&msg,
			msg.GetSize());
	}
}

void CParty::SetOption(BYTE Option)
{
	m_Option = Option;
}

BYTE CParty::GetOption()
{
	return m_Option;
}

void CParty::SetDiceGradeOpt(BYTE Grade)
{
	m_DiceGrade = Grade;
}

BYTE CParty::GetDiceGradeOpt()
{
	return m_DiceGrade;
}

DWORD CParty::GetRandomMember()
{
	DWORD dwCandidateNum = 0;
	DWORD dwCandidate[MAX_PARTY_LISTNUM] = {0,};

	for(int i=0; i<MAX_PARTY_LISTNUM; i++)
	{
		if(0 == g_pUserTable->FindUser(m_Member[i].dwMemberID))
		{
			continue;
		}

		dwCandidate[dwCandidateNum++] = m_Member[i].dwMemberID;
	}

	int nChoice = rand()%dwCandidateNum;
	return dwCandidate[nChoice];
}

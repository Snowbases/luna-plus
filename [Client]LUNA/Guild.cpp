// Guild.cpp: implementation of the CGuild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Guild.h"
#include "GameIn.h"
#include "GuildDialog.h"
#include "ObjectManager.h"
#include "GuildManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuild::CGuild()
{
	m_GuildIdx = 0;
	m_nStudentCount = 0;
}

CGuild::~CGuild()
{
	Release();
}

void CGuild::Init(GUILDINFO* pInfo)
{
	Release();
	if(pInfo)
	{
		m_GuildIdx = pInfo->GuildIdx;
		m_GuildLvl = pInfo->GuildLevel;
		SafeStrCpy(m_MasterName, pInfo->MasterName, MAX_NAME_LENGTH+1);
		SafeStrCpy(m_GuildName, pInfo->GuildName, MAX_GUILD_NAME+1);
		// 06. 03. 문파공지 - 이영준
		//strcpy(m_GuildNotice, pInfo->GuildNotice);
		m_UnionIdx = pInfo->UnionIdx;
		for(int i=0; i<eRankPos_Max; ++i)
			m_RankMemberIdx[i] = 0;
		
		m_nStudentCount = 0;
	}
}

void CGuild::Release()
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();

	while( pos )
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);
		SAFE_DELETE(pInfo);
	}
	m_MemberList.RemoveAll();

	m_UnionIdx = 0;
	for(int i=0; i<eRankPos_Max; ++i)
		m_RankMemberIdx[i] = 0;
	
	m_MemberMultiMap.clear();
}

BOOL CGuild::DeleteMember(DWORD PlayerIDX)
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	GUILDMEMBERINFO* pInfo = NULL;
	while(pos)
	{
		pInfo = (GUILDMEMBERINFO*)m_MemberList.GetAt(pos);
		if(pInfo)
		{
			//SW060713 문하생
			if(pInfo->Rank == GUILD_STUDENT)
			{
				--m_nStudentCount;
			}

			if(pInfo->MemberIdx == PlayerIDX)
			{
				ResetRankMemberInfo(PlayerIDX, pInfo->Rank);
				SAFE_DELETE(pInfo);
				m_MemberList.RemoveAt(pos);
				return TRUE;
			}
		}
		m_MemberList.GetNext(pos);
	}

	return FALSE;
}

BOOL CGuild::AddMember(GUILDMEMBERINFO* pMemberInfo)
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	
	GUILDMEMBERINFO* pInfo = NULL;
	while(pos)
	{
		pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);

		if(pInfo)
		if(pInfo->MemberIdx == pMemberInfo->MemberIdx)
		{
			return FALSE;
		}
	}
	
	pInfo = new GUILDMEMBERINFO;
	*pInfo = *pMemberInfo;
	
	m_MemberList.AddTail(pInfo);
	if(pInfo->Rank == GUILD_VICEMASTER)
		m_RankMemberIdx[0] = pInfo->MemberIdx;
	else if(pInfo->Rank == GUILD_SENIOR)
	{
		int pos = IsVacancy(GUILD_SENIOR);
		if((pos <0) || (pos > eRankPos_Max))
		{
			ASSERTMSG(0, "Load GuildMemberInfo - Set Ranked Member");
			return TRUE;
		}
		m_RankMemberIdx[pos] = pInfo->MemberIdx;
	}

	//SW00713 문하생
	if(pInfo->Rank == GUILD_STUDENT)
	{
		++m_nStudentCount;
	}

	return TRUE;
}

BOOL CGuild::IsMember(DWORD PlayerIDX)
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();

	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);
	
		if(pInfo)
		if(pInfo->MemberIdx == PlayerIDX)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CGuild::SetTotalInfo(GUILDINFO* pInfo, GUILDMEMBERINFO* pMemberInfo, int MemberNum)
{
	Release();

	m_GuildIdx = pInfo->GuildIdx;
	m_GuildLvl = pInfo->GuildLevel;
	for(int i=0; i<MemberNum; ++i)
	{
		if(HEROID == pMemberInfo[i].MemberIdx)
			pMemberInfo[i].bLogged = TRUE;
		AddMember(&pMemberInfo[i]);
	}

	SafeStrCpy(m_MasterName, pInfo->MasterName, MAX_NAME_LENGTH+1);
    SafeStrCpy(m_GuildName, pInfo->GuildName , MAX_GUILD_NAME+1);
}

GUILDMEMBERINFO* CGuild::GetMemberInfo(DWORD PlayerIDX)
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();

	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);
		
		if(pInfo)
		if(pInfo->MemberIdx == PlayerIDX)
		{
			return pInfo;
		}
	}
	return NULL;
}

char* CGuild::GetMemberName(DWORD MemberID)
{
	PTRLISTPOS pos = m_MemberList.GetHeadPosition();
	
	while(pos)
	{
		GUILDMEMBERINFO* pInfo = (GUILDMEMBERINFO*)m_MemberList.GetNext(pos);
		
		if(pInfo)
		if(pInfo->MemberIdx == MemberID)
		{
			return pInfo->MemberName;
		}
	}
	return "";
}

DWORD CGuild::GetMemberIdx(int num)
{
	int i=0;

	std::multimap< RANK, GUILDMEMBERINFO* >::reverse_iterator rit;
	
	for(rit = m_MemberMultiMap.rbegin(); rit != m_MemberMultiMap.rend(); rit++)
	{
		if(i == num)
		{	
			GUILDMEMBERINFO* pInfo = (*rit).second;
			return pInfo->MemberIdx;
		}
		i++;
	}

	return 0;
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
		return eRankPos_Max;
		
	default:
		ASSERTMSG(0, "IsVancancy Err.");
		return eRankPos_Err;
	}

	int rt = eRankPos_Err;
	for(int i=start; i<end; ++i)
	{
		if( m_RankMemberIdx[i] == 0 )
		{
			return i;
		}
	}
	return rt;
}

BOOL CGuild::ChangeRank(DWORD MemberID, BYTE Rank)
{
	//int Pos = IsVacancy(Rank);
	//if(Pos == eRankPos_Err)
	//{
	//	ASSERTMSG(0, "Client Change Rank IsVacancy Error");
	//	return FALSE;
	//}
	GUILDMEMBERINFO* pMemberInfo = GetMemberInfo(MemberID);
	if(pMemberInfo == 0)
	{
		//ASSERTMSG(0, "Change Rank - No Member Data");
		return FALSE;
	}
	//if((Pos <0) || (Pos >eRankPos_Max))
	//{
	//	ASSERTMSG(0, "change rank pos boundary error");
	//	return FALSE;
	//}
	ResetRankMemberInfo(MemberID, pMemberInfo->Rank);

	//SW060713 문하생
	if(pMemberInfo->Rank == GUILD_STUDENT && Rank != GUILD_STUDENT)
	{
		--m_nStudentCount;
	}

	pMemberInfo->Rank = Rank;
	//if(Pos == eRankPos_Max)
	//	return TRUE;
	//m_RankMemberIdx[Pos] = MemberID;
	return TRUE;
}

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

void CGuild::SetMemberLevel(DWORD MemberIdx, DWORD MemberLvl)
{
	GAMEIN->GetGuildDlg()->SetMemberLevel(
		MemberIdx,
		MemberLvl);
}

void CGuild::SetLogInfo(DWORD MemberIdx, BOOL vals)
{
	GUILDMEMBERINFO* pMemberInfo = GetMemberInfo(MemberIdx);
	if(pMemberInfo == 0)
		return;
	pMemberInfo->bLogged = vals;

	//dlg refresh
}
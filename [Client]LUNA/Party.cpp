// Party.cpp: implementation of the CParty class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Party.h"
#include "PartyManager.h"
#include "GameIn.h"
#include "ObjectManager.h"

// 071002 LYW --- Party.cpp : Include header files.
#include "interface/cWindowManager.h"
#include "windowidenum.h"
#include "PartyBtnDlg.h"

CParty::CParty()
{}

CParty::~CParty()
{
	
}

void CParty::InitParty(PARTY_INFO* pInfo)
{
	ZeroMemory(
		m_Member,
		sizeof(m_Member));
	HERO->SetPartyIdx(pInfo->PartyDBIdx);

	SetMaster(&pInfo->Member[0]);

	for(int n = 1; n < MAX_PARTY_LISTNUM; ++n)
	{
		if(pInfo->Member[n].dwMemberID != 0)
		{
			AddMember(&pInfo->Member[n]);
		}
	}

	CPartyBtnDlg* pPartySet = NULL ;
	pPartySet = (CPartyBtnDlg*)WINDOWMGR->GetWindowForID( PA_BTNDLG );

	if( pPartySet )
	{
		pPartySet->SetDistribute(pInfo->Option);
		pPartySet->SetDiceGrade(pInfo->SubOption);
	}
}
/*
void CParty::SetPartyIdx(DWORD Partyid)
{
	m_PartyDBIdx = Partyid;
	if(HERO)
		HERO->SetPartyIdx(Partyid);
}
*/
void CParty::SetMaster(PARTY_MEMBER* pMasterInfo)
{
	m_Member[0] = *pMasterInfo;
}
void CParty::SetMaster(DWORD MasterIdx,char* strName,BOOL bLog,BYTE LifePercent, BYTE ManaPercent)
{
	PARTY_MEMBER& member = m_Member[0];
	member.dwMemberID = MasterIdx;
	member.bLogged = bLog;
	member.LifePercent = LifePercent;
	member.ManaPercent = ManaPercent;
	SafeStrCpy(
		member.Name,
		strName,
		sizeof(member.Name) / sizeof(*member.Name));
}

DWORD CParty::GetMasterIdx()
{
	return m_Member[0].dwMemberID;
}

char* CParty::GetMasterName()
{
	return m_Member[0].Name;
}


void CParty::AddMember(PARTY_MEMBER* pMemberInfo) 
{
	if(IsPartyMember(pMemberInfo->dwMemberID))
	{
		return;
	}

	for(int i = 1; i < MAX_PARTY_LISTNUM; ++i)
	{
		if(m_Member[i].dwMemberID == 0)
		{
			m_Member[i] = *pMemberInfo;
			return;
		}
	}
//	ASSERT(0);
}

void CParty::DelMember(DWORD id)
{
	for(int n=1;n<MAX_PARTY_LISTNUM;++n)
	{
		if(m_Member[n].dwMemberID == id)
		{
			ZeroMemory(
				&m_Member[n],
				sizeof(m_Member[n]));
		}
	}
}

void CParty::LogIn(PARTY_MEMBER* pMemberInfo)
{
	for(int n=0;n<MAX_PARTY_LISTNUM;++n)
	{
		if(m_Member[n].dwMemberID == pMemberInfo->dwMemberID)
		{
			m_Member[n] = *pMemberInfo;
			return;
		}
	}
//	ASSERT(0);
}

void CParty::LogOut(DWORD PlayerID)
{
	for(int n=0;n<MAX_PARTY_LISTNUM;++n)
	{
		if(m_Member[n].dwMemberID == PlayerID)
		{
			m_Member[n].bLogged = FALSE;
			m_Member[n].LifePercent = 0;
			m_Member[n].ManaPercent = 0;
			return;
		}
	}
//	ASSERT(0);
}

char* CParty::GetMemberName(DWORD MemberID)
{	
	for(int n=0;n<MAX_PARTY_LISTNUM;++n)
	{
		if(m_Member[n].dwMemberID == MemberID)
		{
			return m_Member[n].Name;
		}
	}

	// 없는 파티원을 찾으려 했다.
//	ASSERT(0);
	return "";
}

DWORD CParty::GetMemberID(int num)
{
	return m_Member[num].dwMemberID;
}

void CParty::BreakUpResult()
{
	Clear();
}

void CParty::Clear()
{
	ZeroMemory(
		m_Member,
		sizeof(m_Member));
}

void CParty::ChangeMaster(DWORD dwPlayerID)
{
	for(int n=1;n<MAX_PARTY_LISTNUM;++n)
	{
		if(m_Member[n].dwMemberID == dwPlayerID)
		{
			// 091127 LUJ, 값을 교환한다
			PARTY_MEMBER backupMember = m_Member[0];
			m_Member[0] = m_Member[n];
			m_Member[n] = backupMember;
			break;
		}		
	}
}

BOOL CParty::IsPartyMember(DWORD dwPlayerID)
{
//	ASSERT(dwPlayerID);
	for(int n=0;n<MAX_PARTY_LISTNUM;++n)
	{
		if(m_Member[n].dwMemberID == dwPlayerID)
			return TRUE;
	}
	return FALSE;
}





// 071006 LYW --- Party : Add function to check member for name.
BOOL CParty::IsPartyMember(char* pName)
{
	if( !pName ) return FALSE ;

	for(int n=0;n<MAX_PARTY_LISTNUM;++n)
	{
		if( strcmp(m_Member[n].Name, pName) == 0 &&
			m_Member[n].dwMemberID != 0 )						// 090526 ShinJS --- 멤버확인시 ID만 초기화 하기 때문에 ID확인 필요
		{
			return TRUE ;
		}
	}

	return FALSE ;
}





// 071006 LYW --- Party : Add function to return member id to use name.
DWORD CParty::GetMemberIdForName(char* pName)
{
	if( !pName ) return 0 ;

	for(int n=0;n<MAX_PARTY_LISTNUM;++n)
	{
		if( strcmp(m_Member[n].Name, pName) == 0 )
		{
			return m_Member[n].dwMemberID;
		}
	}

	return 0 ;
}





PARTY_MEMBER* CParty::GetPartyMemberInfo(int i)
{
	return &m_Member[i];
}

int CParty::GetMemberNum()
{
	int num=0;
	for(int n=0;n<MAX_PARTY_LISTNUM;++n)
	{
		if(m_Member[n].dwMemberID != 0)
			++num;
	}
	return num;
}
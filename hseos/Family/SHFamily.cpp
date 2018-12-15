/*********************************************************************

	 파일		: SHFamilyManager.cpp
	 작성자		: hseos
	 작성일		: 2007/07/03

	 파일설명	: CSHFamilyManager 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "SHFamily.h"

#if defined(_AGENTSERVER)
	#include "Network.h"
#elif defined(_MAPSERVER_)
#else
	#include "Player.h"
	#include "ChatManager.h"
	#include "GameIn.h"
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHFamilyMember
//

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHFamilyMember Method																										  생성자
//
CSHFamilyMember::CSHFamilyMember() : CSHGroupMember()
{
	ZeroMemory(&m_stInfoEx, sizeof(m_stInfoEx));
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHFamilyMember Method																										  파괴자
//
CSHFamilyMember::~CSHFamilyMember()
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHFamily
//

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHFamily Method																												  생성자
//
CSHFamily::CSHFamily() : CSHGroup()
{
	ZeroMemory(&m_stInfoEx, sizeof(m_stInfoEx));
	ZeroMemory(&m_pcsMember, sizeof(m_pcsMember));
	m_nEmblemChangedNum = 0;
	m_nMemberNumMax = MAX_MEMBER_NUM;
}

CSHFamily::CSHFamily(DWORD nMasterID, char* pszName) : CSHGroup(nMasterID, pszName)
{
	ZeroMemory(&m_stInfoEx, sizeof(m_stInfoEx));
	ZeroMemory(&m_pcsMember, sizeof(m_pcsMember));
	m_nMemberNumMax = MAX_MEMBER_NUM;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHFamily Method																											  파괴자
//
CSHFamily::~CSHFamily()
{}


// -------------------------------------------------------------------------------------------------------------------------------------
// DelMember Method																											   멤버 삭제
//
VOID CSHFamily::DelMember(DWORD nMemberID)
{
	BOOL bSort = FALSE;
	for(UINT i=0; i<m_stInfo.nMemberNum; i++)
	{
		if (nMemberID == m_pcsMember[i].Get()->nID)
		{
			// 멤버 수 감소
			m_stInfo.nMemberNum--;
			bSort = TRUE;
		}

		// 정렬
		if (i<m_stInfo.nMemberNum && bSort)
		{
			SetMember(GetMember(i+1), i);
		}
	}

	if (bSort)
	{
		// 삭제
		CSHFamilyMember csMember;
		SetMember(&csMember, m_stInfo.nMemberNum);
	}
}


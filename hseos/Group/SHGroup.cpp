/*********************************************************************

	 파일		: SHFamilyManager.cpp
	 작성자		: hseos
	 작성일		: 2007/07/03

	 파일설명	: CSHFamilyManager 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "SHGroup.h"

#if defined(_AGENTSERVER)
	#include "Network.h"
#elif defined(_MAPSERVER_)
#else
	#include "Player.h"
	#include "ChatManager.h"
	#include "GameIn.h"
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHGroupMember
//

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHGroupMember Method																										  생성자
//
CSHGroupMember::CSHGroupMember()
{
	ZeroMemory(&m_stInfo, sizeof(m_stInfo));
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHGroupMember Method																										  파괴자
//
CSHGroupMember::~CSHGroupMember()
{
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHGroup
//

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHGroup Method																												  생성자
//
CSHGroup::CSHGroup()
{
	ZeroMemory(&m_stInfo, sizeof(m_stInfo));

	m_pcsMember = NULL;
	m_nIndexAtTbl = 0;
}

CSHGroup::CSHGroup(DWORD nMasterID, char* pszName)
{
	ZeroMemory(&m_stInfo, sizeof(m_stInfo));

	m_pcsMember = NULL;
	m_nIndexAtTbl = 0;

	m_stInfo.nMasterID = nMasterID;
	SafeStrCpy(m_stInfo.szName, pszName, MAX_NAME_LENGTH+1);
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHGroup Method																												  파괴자
//
CSHGroup::~CSHGroup()
{
}


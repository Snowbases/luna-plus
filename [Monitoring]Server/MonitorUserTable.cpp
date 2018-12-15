// MonitorUserTable.cpp: implementation of the CMonitorUserTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonitorUserTable.h"
#include "MonitorNetwork.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CMonitorUserTable)
CMonitorUserTable::CMonitorUserTable()
{
	m_InfoPool.Init(20, 5, "MonitorUser Info");
	//m_MCIDIndex.Init(100, 1);
	m_InfoTable.Initialize(100);
}

CMonitorUserTable::~CMonitorUserTable()
{
	ReleaseAll();
	m_InfoTable.RemoveAll();
	//m_MCIDIndex.Release();
	m_InfoPool.Release();
}
void CMonitorUserTable::ReleaseAll()
{
	m_InfoTable.SetPositionHead();
	MCINFO * info = NULL;
	while(info = (MCINFO *)m_InfoTable.GetData())
	{
		m_InfoTable.Remove(info->dwConnectionIndex);
		m_InfoPool.Free(info);
	}
}
void CMonitorUserTable::RemoveUser(DWORD dwConnectionIdx)
{
	MCINFO * info = m_InfoTable.GetData(dwConnectionIdx);

	// 080821 LUJ, 체크 코드 추가
	if( ! info )
	{
		return;
	}

	m_InfoTable.Remove(info->dwConnectionIndex);
	m_InfoPool.Free(info);

	MNETWORK->DisconnectUser( dwConnectionIdx );
}
void CMonitorUserTable::AddUser(MCINFO * pInfo, DWORD dwConnectionIdx)
{
	MCINFO * tmp = m_InfoPool.Alloc();
	memcpy(tmp, pInfo, sizeof(MCINFO));
	m_InfoTable.Add(tmp, dwConnectionIdx);
}
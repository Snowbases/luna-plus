#include "StdAfx.h"
#include "CWayPointManager.h"


CWayPointManager::CWayPointManager()
: m_WMStatus(eWMStatus_Inactive)
{

}

CWayPointManager::~CWayPointManager()
{

}

void CWayPointManager::AddWayPoint( const VECTOR3& p_WayPoint )
{
	m_deq_WayPoint.push_front(p_WayPoint);
}

BOOL CWayPointManager::PopWayPoint(VECTOR3* p_WayPoint)
{
	if (m_deq_WayPoint.size() == 0)
	{
		return FALSE;
	}
	
	*p_WayPoint = m_deq_WayPoint.front();
	m_deq_WayPoint.pop_front();
	return TRUE;
}

void CWayPointManager::ClearAllWayPoint()
{
	m_deq_WayPoint.clear();
}

BOOL CWayPointManager::IsWayPointEmpty()
{
	if (m_deq_WayPoint.size() == 0)
	{
		return TRUE;
	}

	return FALSE;
}

void CWayPointManager::ChangeDestination( const VECTOR3& p_Dest )
{
	if (m_deq_WayPoint.size() == 0)
	{
		return;
	}
	else
	{
		m_deq_WayPoint.pop_back();
		m_deq_WayPoint.push_back(p_Dest);

		return;
	}

}

void CWayPointManager::Debug_TRACEWayPoint()
{
#ifdef _GMTOOL
	for (int i=0; i < (int)(m_deq_WayPoint.size()); i++)
	{
		VECTOR3 l_WayPoint = m_deq_WayPoint[i];

		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			"WP(%d) - (%7.2f,%7.2f)\n",
			i,
			l_WayPoint.x,
			l_WayPoint.z);
	}
#endif
}
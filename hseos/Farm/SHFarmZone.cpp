/*********************************************************************

	 파일		: SHFarmZone.cpp
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 농장 지역 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "../Common/SHMath.h"
#include "SHFarmZone.h"


// -------------------------------------------------------------------------------------------------------------------------------------
// CSHFarmZone Method																											  생성자
//
CSHFarmZone::CSHFarmZone(FARM_ZONE eBelongZone, int nMapID, int nFarmNum)
{
	m_csFarm.Initialize( 10 );

	for( int i = 0; i < nFarmNum; ++i )
	{
		CSHFarm* farm = new CSHFarm;
		farm->SetID( (WORD)i );
		farm->SetParent( this );
		AddChild(
			farm,
			FARM_ZONE_EVENT_FARM );
		m_csFarm.Add( farm, i );
	}
	m_eBelongZone = eBelongZone;
	m_nMapID = nMapID;
	m_nEventKind = FARM_ZONE_EVENT_FARM;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHFarmZone Method																											  파괴자
//
CSHFarmZone::~CSHFarmZone()
{
	m_csFarm.SetPositionHead();
	CSHFarm* farm = NULL;
	while( (farm = m_csFarm.GetData()) != NULL )
	{
		SAFE_DELETE( farm );
	}
}

int CSHFarmZone::GetFarmNum( DWORD objectIndex )
{
	DWORD ownedFarmCount = 0;

	m_csFarm.SetPositionHead();
	CSHFarm* farm = NULL;
	while( (farm = m_csFarm.GetData()) != NULL )
	{
		if( farm->GetOwner() == objectIndex )
		{
			++ownedFarmCount;
		}
	}

	return ownedFarmCount;
}
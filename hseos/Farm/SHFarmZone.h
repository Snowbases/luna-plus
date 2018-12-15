/*********************************************************************

	 파일		: SHFarmZone.h
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 농장 지역 클래스의 헤더

 *********************************************************************/

#pragma once

#include "SHFarm.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
class CSHFarmZone : public CSHFarmObj
{
public:
	//----------------------------------------------------------------------------------------------------------------
	typedef enum										// 농장이 속한 지역
	{
		FARM_ZONE_ALKER,
		// 090921 LUJ, 네라 농장 추가
		FARM_ZONE_NERA,
		FARM_ZONE_MAX
	} FARM_ZONE;

	//----------------------------------------------------------------------------------------------------------------
	typedef enum										// 농장 지역 이벤트
	{
		FARM_ZONE_EVENT_NOTHING = 0,					// ..아무것도 아님
		FARM_ZONE_EVENT_FARM,							// ..농장
	} FARM_ZONE_EVENT;

private:
	CYHHashTable< CSHFarm > m_csFarm;
	FARM_ZONE				m_eBelongZone;				// 어느 지역의 농장?
	int						m_nMapID;					// 맵 번호

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHFarmZone(FARM_ZONE eBelongZone, int nMapID, int nFarmNum);
	// 090629 LUJ, 파생 클래스의 소멸자가 호출되도록 수정
	virtual ~CSHFarmZone();
	CSHFarm* GetFarm( DWORD index )	{ return m_csFarm.GetData( index ); }
	int GetMapID() { return m_nMapID; }
	int GetFarmNum() { return m_csFarm.GetDataNum(); }
	// 090629 LUJ, 오브젝트가 소유한 농장 개수를 반환한다
	int GetFarmNum( DWORD objectIndex );
};

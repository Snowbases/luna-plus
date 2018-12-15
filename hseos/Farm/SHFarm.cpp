/*********************************************************************

	 파일		: SHFarm.cpp
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 농장 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "../Common/SHMath.h"
 
#include "SHFarm.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHFarm
//

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHFarm Method																												  생성자
//
CSHFarm::CSHFarm()
{
	// 090629 LUJ, 해쉬 테이블로 교체
	m_pcsGarden.Initialize( 1 );
	m_pcsFence.Initialize( 1 );
	m_pcsAnimalCage.Initialize( 1 );
	m_pcsHouse.Initialize( 1 );
	m_pcsWarehouse.Initialize( 1 );

	m_nEventKind = FARM_EVENT_GARDEN;
	m_bRendering = FALSE;
	SetTaxArrearageFreq(0);
	SetTaxPayPlayerName(0);
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHFarm Method																												  파괴자
//
CSHFarm::~CSHFarm()
{
	Release();
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Init Method																													  초기화
//
VOID CSHFarm::Init()
{
	SetOwnState(CSHFarmObj::OWN_STATE_EMPTY);
	SetOwner(0);
	SetGrade(0);
	GetGarden(0)->SetGrade(0);
	GetHouse(0)->SetGrade(0);
	GetWarehouse(0)->SetGrade(0);
	GetAnimalCage(0)->SetGrade(0);
	GetFence(0)->SetGrade(0);
	SetTaxArrearageFreq(0);
	SetTaxPayPlayerName(0);
}

void CSHFarm::CreateGarden( int gardenSize, int cropSize )
{
	for( int i = 0; i < gardenSize; ++i )
	{
		CSHGarden* garden = GetGarden( i );

		if( 0 == garden )
		{
			garden = new CSHGarden;
			m_pcsGarden.Add( garden, i );
		}

		garden->SetID((WORD)i);
		garden->SetParent(this);
		garden->Create( cropSize );
		AddChild(
			garden,
			FARM_EVENT_GARDEN );
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CreateFence Method																									하위 구성물 생성
//
VOID CSHFarm::CreateFence(int nFenceNum)
{
	for(int i = 0; i < nFenceNum; ++i )
	{
		CSHFarmObj* farmObject = GetFence( i );

		if( 0 == farmObject )
		{
			farmObject = new CSHFarmObj;
			m_pcsFence.Add( farmObject, i );
		}
		
		farmObject->SetID((WORD)i);
		farmObject->SetParent(this);
		AddChild(
			farmObject,
			FARM_EVENT_FENCE );
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CreateAnimalCage Method																								하위 구성물 생성
//
VOID CSHFarm::CreateAnimalCage(int nAnimalCageNum, int animalSize )
{
	for(int i = 0; i < nAnimalCageNum; ++i )
	{
		CPen* pen = GetAnimalCage( i );

		if( 0 == pen )
		{
			pen = new CPen;
			m_pcsAnimalCage.Add( pen, i );
		}

		pen->SetID((WORD)i);
		pen->SetParent(this);
		pen->Create( animalSize );
		AddChild(
			pen,
			FARM_EVENT_ANIMALCAGE );
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CreateHouse Method																									하위 구성물 생성
//
VOID CSHFarm::CreateHouse(int nHouseNum)
{
	for( int i = 0; i < nHouseNum; ++i )
	{
		CSHFarmObj* farmObject = GetHouse( i );

		if( 0 == farmObject )
		{
			farmObject = new CSHFarmObj;
			m_pcsHouse.Add( farmObject, i );
		}
		
		farmObject->SetID( (WORD)i );
		farmObject->SetParent( this );
		AddChild(
			farmObject,
			FARM_EVENT_HOUSE );
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CreateWarehouse Method																								하위 구성물 생성
//
VOID CSHFarm::CreateWarehouse(int nWarehouseNum)
{
	for( int i = 0; i < nWarehouseNum; ++i )
	{
		CSHFarmObj* farmObject = GetWarehouse( i );

		if( 0 == farmObject )
		{
			farmObject = new CSHFarmObj;
			m_pcsWarehouse.Add( farmObject, i );
		}

		farmObject->SetID( (WORD)i );
		farmObject->SetParent( this );
		AddChild(
			farmObject,
			FARM_EVENT_WAREHOUSE );
	}
}

void CSHFarm::Release()
{
	// 090629 LUJ, 정원 삭제
	m_pcsGarden.SetPositionHead();
	CSHGarden* garden = NULL;
	while( (garden = m_pcsGarden.GetData()) != NULL )
	{
		SAFE_DELETE( garden );
	}
	m_pcsGarden.RemoveAll();

	// 090629 LUJ, 울타리 삭제
	m_pcsFence.SetPositionHead();
	CSHFarmObj* farmObject = m_pcsFence.GetData();
	while( (farmObject = m_pcsFence.GetData()) != NULL )
	{
		SAFE_DELETE( farmObject );
	}
	m_pcsFence.RemoveAll();

	// 090629 LUJ, 가축 우리 삭제
	m_pcsAnimalCage.SetPositionHead();
	CPen* pen = NULL;
	while( (pen = m_pcsAnimalCage.GetData()) != NULL )
	{
		SAFE_DELETE( pen );
	}
	m_pcsAnimalCage.RemoveAll();

	// 090629 LUJ, 집 삭제
	m_pcsHouse.SetPositionHead();
	farmObject = NULL;
	while( (farmObject = m_pcsHouse.GetData()) != NULL )
	{
		SAFE_DELETE( farmObject );
	}
	m_pcsHouse.RemoveAll();

	// 090629 LUJ, 창고 삭제
	m_pcsWarehouse.SetPositionHead();
	farmObject = NULL;
	while( (farmObject = m_pcsWarehouse.GetData()) != NULL )
	{
		SAFE_DELETE( farmObject );
	}
	m_pcsWarehouse.RemoveAll();
}
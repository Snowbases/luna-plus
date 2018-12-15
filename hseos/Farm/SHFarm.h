/*********************************************************************

	 파일		: SHFarm.h
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 농장 클래스의 헤더

 *********************************************************************/

#pragma once

#include "SHGarden.h"
#include "Pen.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
class CSHFarm : public CSHFarmObj
{
public:
	//----------------------------------------------------------------------------------------------------------------
	enum FARM_EVENT										// 농장 이벤트
	{
		FARM_EVENT_NOTHING = 0,							// ..아무것도 아님
		FARM_EVENT_GARDEN,								// ..텃밭
		FARM_EVENT_ANIMALCAGE,							// ..축사
		FARM_EVENT_FENCE,								// ..웉타리
		FARM_EVENT_HOUSE,								// ..집
		FARM_EVENT_WAREHOUSE,							// ..창고
		FARM_EVENT_MAX = FARM_EVENT_WAREHOUSE,
	};

private:
	CYHHashTable< CSHGarden > m_pcsGarden;
	CYHHashTable< CSHFarmObj > m_pcsFence;
	CYHHashTable< CPen > m_pcsAnimalCage;
	CYHHashTable< CSHFarmObj > m_pcsHouse;
	CYHHashTable< CSHFarmObj > m_pcsWarehouse;
	BOOL					m_bRendering;								// 렌더 중인가

	UINT					m_nTaxArrearageFreq;						// 관리비 연체 횟수
	char					m_szTaxPayPlayerName[MAX_NAME_LENGTH+1];	// 관리비 납부 플레이어

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHFarm();
	// 090629 LUJ, 파생 클래스의 소멸자가 호출되도록 수정
	virtual ~CSHFarm();
	void Init();
	void Release();
	void CreateGarden( int gardenSize, int cropSize );
	CSHGarden* GetGarden(int nGardenID)	{ return m_pcsGarden.GetData( nGardenID ); }
	int GetGardenNum() { return m_pcsGarden.GetDataNum(); }
	//						울타리 생성
	VOID					CreateFence(int nFenceNum);
	CSHFarmObj*				GetFence(int nFenceID)		{ return m_pcsFence.GetData( nFenceID ); }
	int						GetFenceNum()				{ return m_pcsFence.GetDataNum(); }
	//----------------------------------------------------------------------------------------------------------------
	//						축사 생성
	VOID					CreateAnimalCage( int nAnimalCageNum, int animalSize );
	CPen*					GetAnimalCage(int nAnimalCageID)	{ return m_pcsAnimalCage.GetData( nAnimalCageID ); }
	int						GetAnimalCageNum()					{ return m_pcsAnimalCage.GetDataNum(); }
	//----------------------------------------------------------------------------------------------------------------
	//						집 생성
	VOID					CreateHouse(int nHouseNum);
	CSHFarmObj*				GetHouse(int nHouseID)		{ return m_pcsHouse.GetData( nHouseID ); }
	int						GetHouseNum()				{ return m_pcsHouse.GetDataNum(); }
	//----------------------------------------------------------------------------------------------------------------
	//						창고 생성
	VOID					CreateWarehouse(int nWarehouseNum);
	CSHFarmObj*				GetWarehouse(int nWarehouseID)		{ return m_pcsWarehouse.GetData( nWarehouseID ); }
	int						GetWarehouseNum()					{ return m_pcsWarehouse.GetDataNum(); }

	VOID					SetIsRendering(BOOL bRendering)		{ m_bRendering = bRendering; }
	BOOL					GetIsRendering()					{ return m_bRendering; }

	//----------------------------------------------------------------------------------------------------------------
	//						관리비 연체 횟수 설정
	VOID					SetTaxArrearageFreq(UINT nFreq)		{ m_nTaxArrearageFreq = nFreq; }
	//						관리비 연체 횟수 얻기
	UINT					GetTaxArrearageFreq()				{ return m_nTaxArrearageFreq; }
	//						관리비 납부 플레이어 설정
	VOID					SetTaxPayPlayerName(char* pszName)	{ pszName == NULL ? ZeroMemory(m_szTaxPayPlayerName, sizeof(m_szTaxPayPlayerName)) : SafeStrCpy(m_szTaxPayPlayerName, pszName, MAX_NAME_LENGTH+1); }
	//						관리비 납부 플레이어 얻기
	char*					GetTaxPayPlayerName()				{ return m_szTaxPayPlayerName; }
	//						관리비 납부 상태 얻기
	BOOL					IsPayTax()							{ return m_szTaxPayPlayerName[0]; }
};
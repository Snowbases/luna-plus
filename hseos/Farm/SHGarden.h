/*********************************************************************

	 파일		: SHGarden.h
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 텃밭 클래스의 헤더

 *********************************************************************/

#pragma once

#include "SHCrop.h"
#include "SHFarmObj.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
class CSHGarden : public CSHFarmObj
{
public:
	//----------------------------------------------------------------------------------------------------------------
	typedef enum														// 농장 텃밭 관리 결과
	{
		GARDEN_EVENT_NOTHING = 0,										// ..아무 일도..
		GARDEN_EVENT_CROP,												// ..농작물
	} GARDEN_EVENT;

private:
	CYHHashTable< CSHCrop > m_pcsCrop;

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHGarden();
	// 090629 LUJ, 파생 클래스의 소멸자가 호출되도록 수정
	virtual ~CSHGarden();
	void Create(int nCropNum);
	CSHCrop* GetCrop(int nCropID) { return m_pcsCrop.GetData( nCropID ); }
	int	GetCropNum() { return m_pcsCrop.GetDataNum(); }	
};
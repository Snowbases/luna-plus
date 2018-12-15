/*********************************************************************

	 파일		: Pen.h
	 작성자		: Shinobi
	 작성일		: 2008/03/11

	 파일설명	: 축사 클래스의 헤더

 *********************************************************************/

#pragma once

#include "Animal.h"
#include "SHFarmObj.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
class CPen : public CSHFarmObj
{
public:
	//----------------------------------------------------------------------------------------------------------------
	typedef enum														// 축사 가축 관리 결과
	{
		GARDEN_EVENT_NOTHING = 0,										// ..아무 일도..
		GARDEN_EVENT_ANIMAL,											// ..가축
	} GARDEN_EVENT;

private:
	CYHHashTable< CAnimal > m_pcsAnimal;

public:
	//----------------------------------------------------------------------------------------------------------------
	CPen();
	// 090629 LUJ, 파생 클래스의 소멸자가 호출되도록 수정
	virtual ~CPen();
	void Create( int nAnimalNum );
	CAnimal* GetAnimal( int nAnimalID )	{ return m_pcsAnimal.GetData( nAnimalID ); }
	int GetAnimalNum() { return m_pcsAnimal.GetDataNum(); }
};
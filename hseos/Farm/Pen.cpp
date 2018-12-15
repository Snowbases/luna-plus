/*********************************************************************

	 파일		: Pen.cpp
	 작성자		: Shinobi
	 작성일		: 2008/03/11

	 파일설명	: 축사 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "../Common/SHMath.h"
#include "Pen.h"


// -------------------------------------------------------------------------------------------------------------------------------------
// CPen Method																												  생성자
//
CPen::CPen()
{
	m_pcsAnimal.Initialize( 5 );
	m_nEventKind = GARDEN_EVENT_ANIMAL;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CPen Method																											  파괴자
//
CPen::~CPen()
{
	m_pcsAnimal.SetPositionHead();
	CAnimal* animal = NULL;
	while( (animal = m_pcsAnimal.GetData()) != NULL )
	{
		SAFE_DELETE( animal );
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Create Method																										농작물 공간 생성
//
VOID CPen::Create(int nAnimalNum)
{
	for(int i=0; i<nAnimalNum; i++)
	{
		CAnimal* animal = GetAnimal( i );

		if( 0 == animal )
		{
			animal = new CAnimal;
			m_pcsAnimal.Add( animal, i );
		}

		animal->SetID((WORD)i);
		animal->SetParent(this);
		AddChild( animal, GARDEN_EVENT_ANIMAL );
	}
}
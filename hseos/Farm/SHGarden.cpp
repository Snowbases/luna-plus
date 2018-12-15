/*********************************************************************

	 파일		: SHGarden.cpp
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 텃밭 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "../Common/SHMath.h"
#include "SHGarden.h"


// -------------------------------------------------------------------------------------------------------------------------------------
// CSHGarden Method																												  생성자
//
CSHGarden::CSHGarden()
{
	m_pcsCrop.Initialize( 5 );
	m_nEventKind = GARDEN_EVENT_CROP;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHGarden Method																											  파괴자
//
CSHGarden::~CSHGarden()
{
	m_pcsCrop.SetPositionHead();
	for( CSHCrop* crop = 0;
		(crop = m_pcsCrop.GetData()) != NULL; )
	{
		SAFE_DELETE( crop );
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Create Method																										농작물 공간 생성
//
VOID CSHGarden::Create(int nCropNum)
{
	for( int i = 0; i < nCropNum; ++i )
	{
		CSHCrop* crop = GetCrop( i );

		if( 0 == crop )
		{
			crop = new CSHCrop;
			m_pcsCrop.Add( crop, i );
		}

		crop->SetID( (WORD)i );
		crop->SetParent( this );
		AddChild( crop, GARDEN_EVENT_CROP );
	}
}
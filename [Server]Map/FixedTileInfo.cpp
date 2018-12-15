// FixedTileInfo.cpp: implementation of the CFixedTileInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FixedTileInfo.h"
#include "FixedTile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFixedTileInfo::CFixedTileInfo()
{
}

CFixedTileInfo::~CFixedTileInfo()
{
	SAFE_DELETE_ARRAY(m_pFixedTile);
}

void CFixedTileInfo::Init(int fWidth, int fHeight)
{
	m_nTileWidth = fWidth;
	m_nTileHeight = fHeight;
	m_pFixedTile = new CFixedTile[m_nTileHeight*m_nTileWidth];
}

CFixedTile* CFixedTileInfo::GetFixedTile(int x, int z)
{
	if( x < 0 ||
		z < 0 ||
		x >= m_nTileWidth ||
		z >= m_nTileHeight)
		return NULL;

	return &m_pFixedTile[z * m_nTileWidth + x];
}

const FIXEDTILE_ATTR& CFixedTileInfo::GetTileAttrib(int x, int z)
{
	static FIXEDTILE_ATTR emptyAttr;

	if( m_pFixedTile == NULL )
	{
		return emptyAttr;
	}

	CFixedTile* pFixedTile = GetFixedTile( x, z );
	if( pFixedTile == NULL )
	{
		return emptyAttr;
	}

	return pFixedTile->GetTileAttrib();
}
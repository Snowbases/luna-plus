#include "stdafx.h"
#include "TileGroup.h"
#include "FixedTileInfo.h"

CTileGroup::CTileGroup()
{}

CTileGroup::~CTileGroup()
{}

void CTileGroup::Init(CFixedTileInfo* pFixedTileInfo)
{
	m_pFixedTileInfo = pFixedTileInfo;
}
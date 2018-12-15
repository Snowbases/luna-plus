#include "stdafx.h"
#include "PutTileHistory.h"
#include "4dyuchigxmapeditor.h"
#include "MainFrm.h"
#include "tool.h"

CPutTileHistory::CPutTileHistory()
{}

void CPutTileHistory::SetValue(const CPoint& point, int oldTile, int newTile)
{
	mParameter.mPoint = point;
	mParameter.mOldTile = oldTile;
	mParameter.mNewTile = newTile;
}

CPutTileHistory::~CPutTileHistory(void)
{}

void CPutTileHistory::Undo()
{
	PutTile(
		mParameter.mPoint,
		mParameter.mOldTile);
}

void CPutTileHistory::Redo()
{
	PutTile(
		mParameter.mPoint,
		mParameter.mNewTile);
}

void CPutTileHistory::PutTile(const CPoint& point, int tile)
{
	IHeightField* const heightField = g_pTool->GetHField();

	if( 0 == heightField )
	{
		return;
	}

	heightField->SetTile(
		point.x,
		point.y,
		tile);
}
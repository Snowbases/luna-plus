#pragma once
#include "History.h"

class CPutTileHistory : public CHistory
{
public:
	CPutTileHistory();
	virtual ~CPutTileHistory(void);
	virtual void Undo();
	virtual void Redo();
	void SetValue(const CPoint&, int oldTile, int newTile);

private:
	void PutTile(const CPoint&, int tile);

private:
	struct Parameter
	{
		CPoint mPoint;
		int mOldTile;
		int mNewTile;

		Parameter() :
		mOldTile(0),
		mNewTile(0)
		{}
	}
	mParameter;
};

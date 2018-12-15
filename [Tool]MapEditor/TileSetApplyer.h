#pragma once

#include "TileSet.h"

class CTileSetApplyer  
{
	struct TILEBITARRAY
	{
		TILEBITARRAY()	{	memset(Bits,0,25);		}
		BYTE Bits[25];
		void PutCenterTile(int n);
	protected:
		void AddBits(int x,int y,BYTE Bit);
	};
public:
	CTileSetApplyer();
	virtual ~CTileSetApplyer();
	void Apply(CTileSet*, TILETEXTURE [25], TILETEXTURE*);
};
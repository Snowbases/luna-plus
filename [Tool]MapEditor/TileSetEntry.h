#pragma once

#include "TileSetPiece.h"

#define MAX_PIECE_PER_ENTRY	4

class CTileSetEntry  
{
	CTileSetPiece m_PieceArray[MAX_PIECE_PER_ENTRY];
public:
	CTileSetEntry();
	virtual ~CTileSetEntry();

	void SetPiece(DWORD PieceNum,TILETEXTURE* pTexture,DWORD Probability);
	CTileSetPiece* GetRandomPiece();
	CTileSetPiece* GetPiece(DWORD PieceNum);
};
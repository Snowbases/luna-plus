#pragma once

#include "TileSetEntry.h"

class CTileSet  
{
	CString m_Name;
	CTileSetEntry m_EntryArray[16];
public:
	CTileSet();
	virtual ~CTileSet();
	CTileSetPiece* GetCenterPiece(DWORD PieceNum);
	CTileSetPiece* GetRandomPiece(BYTE Bits);
	inline CString& GetName() {	return m_Name; }
	inline void SetName(CString& str) {	m_Name = str; }	
	void CopyFrom(CTileSet* pSet);
	void SetPiece(DWORD EntryNum,DWORD PieceNum,TILETEXTURE* pTexture,DWORD Probability);
	inline void ClearPiece(DWORD EntryNum,DWORD PieceNum)
	{
		SetPiece(EntryNum,PieceNum,0,0);
	}

	inline CTileSetEntry* GetEntry(DWORD EntryNum)
	{
		return &m_EntryArray[EntryNum];
	}

	void SetEntry(DWORD EntryNum,CTileSetEntry*);
	void Save(FILE*);
	void Load(FILE*);	
};
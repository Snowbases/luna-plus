#pragma once

#define TILE_BRUSH_NAME_LEN 128

struct TILE_BRUSH_PROPERTY
{
	TCHAR szName[TILE_BRUSH_NAME_LEN];
};

struct HFILED_POS;

class CTileBrush
{
	DWORD m_dwWidth;
	DWORD m_dwHeight;
	DWORD m_dwTileNum;
	HFIELD_POS*	m_pTileList;
	WORD* m_pwTexTileIndexList;
	char m_szName[TILE_BRUSH_NAME_LEN];	
	int	m_iListBoxIndex;

public:
	BOOL CreateBrush(LPCTSTR szBrushName, HFIELD_POS*,DWORD dwTileNum,WORD* pwTexTileIndexTable,DWORD dwPitch);
	char* GetName() { return m_szName; }
	DWORD GetTileList(HFIELD_POS**, WORD** ppwTexTileIndexList);
	BOOL Save(FILE*);
	BOOL Load(FILE*);
	CTileBrush();
	virtual ~CTileBrush();
};
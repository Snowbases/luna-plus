// FixedTileInfo.h: interface for the CFixedTileInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIXEDTILEINFO_H__019723C0_21A6_439F_B434_907BECEE303B__INCLUDED_)
#define AFX_FIXEDTILEINFO_H__019723C0_21A6_439F_B434_907BECEE303B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFixedTile;
struct FIXEDTILE_ATTR;

class CFixedTileInfo  
{
	int m_nTileWidth;
	int m_nTileHeight;
	
	CFixedTile* m_pFixedTile; 
	
public:
	CFixedTileInfo();
	virtual ~CFixedTileInfo();
	
	int GetTileWidth() { return m_nTileWidth; }
	void Init(int fWidth, int fHeight);
	CFixedTile* GetFixedTile(int fx, int fz);
	CFixedTile* GetFixedTile() { return m_pFixedTile; }
	inline BOOL IsInTile(int x, int y)
	{
		if( x < 0 || y < 0 || x >= m_nTileWidth || y >= m_nTileHeight )
			return FALSE;
		return TRUE;
	}

	const FIXEDTILE_ATTR& GetTileAttrib(int x, int z);
};

#endif // !defined(AFX_FIXEDTILEINFO_H__019723C0_21A6_439F_B434_907BECEE303B__INCLUDED_)

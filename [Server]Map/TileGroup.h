// TileGroup.h: interface for the CTileGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILEGROUP_H__37A91A76_D547_4FF9_9F13_D3E1DE69EDE3__INCLUDED_)
#define AFX_TILEGROUP_H__37A91A76_D547_4FF9_9F13_D3E1DE69EDE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFixedTileInfo;

class CTileGroup  
{
	CFixedTileInfo* m_pFixedTileInfo;
	
public:
	CTileGroup();
	virtual ~CTileGroup();
	void Init(CFixedTileInfo*);
	CFixedTileInfo* GetFixedTileInfo() { return m_pFixedTileInfo; }
};

#endif // !defined(AFX_TILEGROUP_H__37A91A76_D547_4FF9_9F13_D3E1DE69EDE3__INCLUDED_)

#pragma once
#include "baseitem.h"

class cConductInfo ;

class cConductBase :
	public CBaseItem
{
	cImage		m_pHighLayerImage ;

	DWORD		m_dwFontColor ;

public :
	cConductInfo*	m_pConductInfo ;
public:
	cConductBase(void);
	virtual ~cConductBase(void);

	virtual void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * lowImage, cImage * highImage, LONG ID=0) ;
	virtual void Render() ;

	void SetFontColor( DWORD dwFontColor ) { m_dwFontColor = dwFontColor; }
};

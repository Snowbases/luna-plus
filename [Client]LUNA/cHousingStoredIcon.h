#pragma once

#include "stdAfx.h"
#include "./Item.h"

struct stFurniture;

//가구정보를 링크하는 아이콘 
class cHousingStoredIcon : public cIcon  
{	
	stFurniture*		m_pLinkstFurniture;

public:
	cHousingStoredIcon( stFurniture* pstFuniture );	
	virtual ~cHousingStoredIcon();

	virtual void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	DWORD  ActionEvent(CMouse* mouseInfo);

	virtual BOOL CanDelete();
	virtual BOOL CanMoveToDialog( cDialog* ptargetdlg );		//cHousingStoredIcon은 인벤으로만 이동가능하다.
	virtual void Render();

	void Refresh();
	stFurniture* GetLinkFurniture(){return m_pLinkstFurniture;}

};
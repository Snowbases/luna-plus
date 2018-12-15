#pragma once

#include "stdAfx.h"
#include ".\interface\cWindowHeader.h"
#include "interface/cDialog.h"

//090409 pdy 하우징 하우스 검색 UI추가
class cHousingMainPointDlg : public cDialog  
{
	cStatic* m_pHouseName_Static;
	cStatic* m_pVisitCount_Static;
	cStatic* m_pDecoPoint_Static;

	//091012 pdy 하우징 꾸미기 포인트 버튼 기획변경
	cButton* m_BtDecoPoint;		
	
public:
	cHousingMainPointDlg();
	virtual ~cHousingMainPointDlg();
	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	void Linking();
	virtual void SetActive(BOOL val);

	void SetHouseName(char* szHouseName);
	void SetVisitCount(DWORD dwVisitCount);
	void SetDecoPoint(DWORD dwDecoPoint);

	//091012 pdy 하우징 꾸미기 포인트 버튼 기획변경
	void OnActionEvent(LONG lId, void* p, DWORD we);
};

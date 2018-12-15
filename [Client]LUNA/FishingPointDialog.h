#pragma once
#include ".\Interface\cDialog.h"
#include ".\interface\cButton.h"
#include ".\interface\cStatic.h"
#include "VirtualItem.h"

class cButton;
class cIconGridDialog;

class CFishingPointDialog :
	public cDialog
{
	cIconGridDialog*	m_pIconFish;
	cStatic*			m_pAddStatic;
	cStatic*			m_pCurStatic;

	CVirtualItem		m_pFishList[MAX_FISHLIST_FORPOINT];

	DWORD				m_dwAddFishPoint;

public:
	CFishingPointDialog(void);
	virtual ~CFishingPointDialog(void);

	void Linking();
	virtual void SetActive(BOOL val);

	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );

	CVirtualItem* GetFishList(int idx)	{return (CVirtualItem*)&m_pFishList[idx];}

	void SetPointText(char* pText);
	void SetFishItem(int nIdx, CBaseItem* pBaseItem);
	void ShowDealDialog( BOOL val );


	void ClearFishIcon();
	void SendFishPoint();
};

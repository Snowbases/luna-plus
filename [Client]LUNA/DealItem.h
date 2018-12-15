// DealItem.h: interface for the CDealItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEALITEM_H__AAF8CAFA_E112_45C0_96E6_DD5FDBA39E1C__INCLUDED_)
#define AFX_DEALITEM_H__AAF8CAFA_E112_45C0_96E6_DD5FDBA39E1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cIcon.h"

class CDealItem : public cIcon  
{
public:
	CDealItem();
	virtual ~CDealItem();

	void SetItemIdx(DWORD itemIdx)
	{
		m_wItemIdx = itemIdx;
	}
	DWORD GetItemIdx()
	{
		return m_wItemIdx;
	}
	DWORD GetSellPrice();
	DWORD GetBuyPrice();
	DWORD GetBuyFishPoint();
	char* GetItemName();

	// 100104 ShinJS --- Item Point Type에 대한 툴팁 추가
	void AddPointTypeToolTip();
private:
	DWORD m_wItemIdx;
};

#endif // !defined(AFX_DEALITEM_H__AAF8CAFA_E112_45C0_96E6_DD5FDBA39E1C__INCLUDED_)

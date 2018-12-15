#pragma once
#include "stdafx.h"
#include "./interface/cdialog.h"

class cStatic;
class cButton;


//팝업매뉴 각 슬롯에 대한 정보 
struct stActionPopupMenuSlot
{
	cButton* m_pBtAction;
	stHouseActionInfo*	m_pActionInfo;

	stActionPopupMenuSlot()
	{
		memset(this, 0, sizeof(stActionPopupMenuSlot));
	}
};

//가구액션 팝업매뉴UI
class cHousingActionPopupMenuDlg :
	public cDialog
{
	stFurniture* m_pstSelectedFurniture;
	stActionPopupMenuSlot m_PopupMenuSlotArr[MAX_HOUSE_ACTION_SLOT];

	//액티브되는 시점에 마우스포인터와 가까운 본의 넘버를 정렬하여 넣는다. 
	DWORD m_bySortedNearBoneNumArr[MAX_HOUSE_ATTATCH_SLOT];

public:
	cHousingActionPopupMenuDlg(void);
	virtual ~cHousingActionPopupMenuDlg(void);

	void Linking();
	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;
	virtual void SetActive(BOOL val);
	virtual DWORD ActionEvent(CMouse *mouseInfo);

	DWORD* GetSortedNearBoneNumArr(){return m_bySortedNearBoneNumArr;} 
};

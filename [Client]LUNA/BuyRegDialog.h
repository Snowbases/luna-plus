#pragma once


#include "./Interface/cDialog.h"
//#include "GameResourceStruct.h"

class cListDialog;
class cEditBox;
class cButton;
class CMouse;
class CExchangeItem;
class cStatic;

#define MAX_VOLUME_TEXT	"2000"
/*
enum ITEM_TYPE
{
	WEAPON,
	CLOTHES,
	ACCESSORY,
	POTION,
	MATERIAL,
	ETC,
	ITEM_MALL,
	ITEM_TYPE_COUNT,
};*/

enum ITEM_TYPE
{
	// 091127 ONS 구매상점 아이템리스트 변경/추가
	WEAPON,		//1	무기류
	CLOTHES,	//2	방어구1
	ACCESSORY,	//3	액세서리
	POTION,		//4	소모품
	MATERIAL,	//5	재료
	PET,		//6	펫용품
	COSTUME,	//7	코스튬
	PRODUCTION,	//8	생산
	HOUSING,	//9	하우징
	ETC,		//10	기타

	ITEM_TYPE_COUNT,
};

typedef struct _BUYREGINFO{
	WORD Type;
	WORD Item;
	WORD Class;
	WORD Volume;
	DWORD Money;
} BUY_REG_INFO;


struct ITEM_INFO;


class CBuyRegDialog : public cDialog  
{
	cListDialog* m_pTypeList;
	cListDialog* m_pItemList[ITEM_TYPE_COUNT];
	cListDialog* m_pClassList;
	cEditBox* m_pVolumeEdit;
	cEditBox* m_pMoneyEdit;
	cButton* m_pRegBtn;
	cButton* m_pDltBtn;

	cStatic* m_pClassText;
	cStatic* m_pClassRect1;
	cStatic* m_pClassRect2;
	cStatic* m_pItemBigRect;
	cStatic* m_pItemSmallRect1;
	cStatic* m_pItemSmallRect2;

	WORD m_nType;
	WORD m_nItem;
	WORD m_nClass;
	
	WORD m_nVolume;
	DWORD m_nMoney;

	void LoadItemList();

	DWORD	mMoney;
	DWORD	mColor;

public:
	CBuyRegDialog();
	virtual ~CBuyRegDialog();

	void Linking();
	void Show();
	void Close();
	void InitRegInfo();
	void SetRegInfo(BUY_REG_INFO& RegInfo);

	void UpdateType();
	void UpdateItem();
	void UpdateClass();
	ITEM_INFO* GetSelectItemInfo();
	BOOL GetBuyRegInfo(BUY_REG_INFO& RegInfo);

	void OnActionEvnet(LONG lId, void * p, DWORD we);
	virtual void Render();
};
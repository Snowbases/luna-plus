// StorageDialog.h: interface for the CStorageDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGEDIALOG_H__20A47FB2_F872_47CD_8FF4_747FD52A07FC__INCLUDED_)
#define AFX_STORAGEDIALOG_H__20A47FB2_F872_47CD_8FF4_747FD52A07FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cDialog.h"

class CPrivateWarehouseDialog;
class cListCtrl;
class cPushupButton;
class cButton;
class cStatic;
class CItem;
class CMouse;

struct STORAGE_LISTINFO
{
	STORAGELIST Storage[MAX_STORAGELIST_NUM];
};
enum storagedlg_mode
{
	eStorageMode_StorageListInfo,
	eStorageMode_StorageWare1,
	eStorageMode_StorageWare2,
	eStorageMode_StorageWare3,
	eStorageMode_StorageWare4,
	eStorageMode_StorageWare5,

	eStorageMode_NoWare,	
		
	eStorageMode_Max
};

class CStorageDialog : public cDialog  
{
	int m_CurStorageMode;
	int m_StorageNum;
	BOOL m_bItemInit;
    SIZE mButtonSize;
	cListCtrl* m_pStorageListInfo;
	cPushupButton* m_pStorageBtn[MAX_STORAGELIST_NUM];
	cStatic* m_pMoney;
	cStatic* m_pStorageHaveStc;
	cPtrList m_StorageCtlListArray[eStorageMode_Max];

	CPrivateWarehouseDialog* m_StorageItemDlg[MAX_STORAGELIST_NUM];
	void SetActiveStorageMode(int mode,BOOL bActive);
	void SelectedBtnPushDown(int i);
		
	STORAGE_LISTINFO m_StorageListInfo;

public:
	CStorageDialog();
	virtual ~CStorageDialog();
	virtual void SetActive(BOOL);
	virtual DWORD ActionEvent(CMouse*);
	void LoadStorageListInfo();
	void Linking();

	void SetMode(int mode);
	void SetStorageListInfo();
	void ShowStorageMode(int mode);
	void AddStorageMode(int mode);
	void RefreshDlg();

	BYTE GetSelectedStorage() { return static_cast<BYTE>(m_StorageNum);	} //0, 1, 2, 3, 4
	void SetSelectedStorage(BYTE StorageNum);
	void StorageMsgBoxClr();
	
	//개인창고	
	void SetItemInit(BOOL val) { m_bItemInit = val;	}
	BOOL IsItemInit() { return m_bItemInit;	}
	BOOL FakeMoveIcon(LONG x, LONG y, cIcon * icon);
	void FakeGeneralItemMove( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem);
	BOOL AddItem(ITEMBASE* pItemInfo);
	BOOL AddItem(CItem* pItem);
	BOOL DeleteItem(POSTYPE Pos,CItem** ppItem);
	void DeleteAllStorageItem();

	int GetStorageNum(POSTYPE absPos);
	
	void SetStorageMoney(DWORD Money);
	
	int CanBuyStorage();
	
	MONEYTYPE GetPrice(int n) { return m_StorageListInfo.Storage[n].BuyPrice;	}
	
	CItem * GetItemForPos(POSTYPE absPos);

	void SetDividMsg( CItem* pFromItem, POSTYPE ToPos );
	MSG_ITEM_DIVIDE_SYN	m_divideMsg;

	// 070326 LYW --- StorageDialog : Add function to active or not.
	void ShowStorageDlg( BOOL val ) ;
};

#endif // !defined(AFX_STORAGEDIALOG_H__20A47FB2_F872_47CD_8FF4_747FD52A07FC__INCLUDED_)

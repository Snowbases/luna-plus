#pragma once

#include "./Interface/cDialog.h"

class CItemShopGridDialog;
class CItem;

class CItemShopDialog : public cDialog
{
	CItemShopGridDialog* m_pItemGridDialog;

public:
	CItemShopDialog();
	virtual ~CItemShopDialog();	
	virtual DWORD ActionEvent(CMouse * mouseInfo);
	virtual void SetActive( BOOL val );

	void Linking();
	void SetItemInfo(SEND_SHOPITEM_INFO* pItemInfo);
	BOOL AddItem(CItem* pItem);
	BOOL DeleteItem(POSTYPE, CItem**);
	CItem* GetItemForPos(POSTYPE);
};
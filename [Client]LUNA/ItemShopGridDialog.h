// ItemShopGridDialog.h: interface for the CItemShopGridDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMSHOPGRIDDIALOG_H__44925A58_B5B3_4F97_BC61_D3E19F91C3E9__INCLUDED_)
#define AFX_ITEMSHOPGRIDDIALOG_H__44925A58_B5B3_4F97_BC61_D3E19F91C3E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "./Interface/cIconGridDialog.h"


class CItem;


class CItemShopGridDialog : public cIconGridDialog
{
public:
	CItemShopGridDialog();
	virtual ~CItemShopGridDialog();

	void Init();
	BOOL AddItem(const ITEMBASE*);
	POSTYPE GetRelativePosition(POSTYPE);
	BOOL DeleteItem(POSTYPE,CItem**);
	CItem* GetItemForPos(POSTYPE);
};

#endif // !defined(AFX_ITEMSHOPGRIDDIALOG_H__44925A58_B5B3_4F97_BC61_D3E19F91C3E9__INCLUDED_)

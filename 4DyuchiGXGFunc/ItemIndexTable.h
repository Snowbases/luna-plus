#pragma once

#include "type.h"

struct	ITEM_HANDLE
{
	void*		pItem;
	DWORD		dwLinearIndex;
};

	
class CItemIndexTable
{
public:
	DWORD					m_dwMaxItemNum;
	DWORD*					m_pdwIndexList;
	DWORD					m_dwItemNum;
	ITEM_HANDLE*			m_pItemList;	
	INDEXCR_HANDLE			m_pIC;		

public:
	
	CItemIndexTable();
	~CItemIndexTable();

};




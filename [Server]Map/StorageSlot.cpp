// StorageSlot.cpp: implementation of the CStorageSlot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StorageSlot.h"
#include "Purse.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStorageSlot::CStorageSlot()
{
	m_ExtraSlotCount = 0;
	m_storageNum = 0;
}

CStorageSlot::~CStorageSlot()
{

}
/*
void CStorageSlot::SetStorageMoney(MONEYTYPE m)
{
	m_pPurse->Addition(m);
}
MONEYTYPE	CStorageSlot::GetStorageMoney()
{
	return m_pPurse->GetPurseCurMoney();
}
*/


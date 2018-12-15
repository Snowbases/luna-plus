// StorageSlot.h: interface for the CStorageSlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGESLOT_H__E1C39D74_F581_42E9_9C2B_74E61B2C8ABF__INCLUDED_)
#define AFX_STORAGESLOT_H__E1C39D74_F581_42E9_9C2B_74E61B2C8ABF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ItemSlot.h"

class CStorageSlot : public CItemSlot  
{
	DWORD		m_ExtraSlotCount;

public:
	CStorageSlot();
	virtual ~CStorageSlot();

	BYTE		GetStorageNum()		{ return m_storageNum;	}
	void		SetStorageNum(BYTE num)		{ m_storageNum = num;	}
//	void		SetStorageMoney(MONEYTYPE m);
//	MONEYTYPE	GetStorageMoney();

protected:

	BYTE		m_storageNum; // Ç¥±¹Ã¢¿¡ °³¼ö ÇÏ³ª·Î ÅëÇÕ
};

#endif // !defined(AFX_STORAGESLOT_H__E1C39D74_F581_42E9_9C2B_74E61B2C8ABF__INCLUDED_)

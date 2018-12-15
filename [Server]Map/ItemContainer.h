// ItemContainer.h: interface for the CItemContainer class.
// created by taiyo
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMCONTAINER_H__76632FDD_2DBD_4175_B1F5_2F88F531EC1E__INCLUDED_)
#define AFX_ITEMCONTAINER_H__76632FDD_2DBD_4175_B1F5_2F88F531EC1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPurse;
class CItemSlot;
//class CPlayer;

class CItemContainer
{
	ITEMBASE	m_TotalItemBase[TP_MAX];
	SLOTINFO	m_TotalSlotInfo[TP_MAX];
	CItemSlot	* m_pItemSlot[eItemTable_TableMax];
	CItemSlot * GetSlot(POSTYPE absPos);
	CItemSlot * GetSlot(eITEMTABLE tableIdx);

	friend class CPlayer;
public:
	CItemContainer();
	virtual ~CItemContainer();

	void Init();
	void SetInit(WORD tableIdx, POSTYPE StartPosition, POSTYPE slotNum, CItemSlot*);
	ITEMBASE const *  GetItemInfoAbs(POSTYPE);
	CPurse*	GetPurse(eITEMTABLE);
	inline DWORD GetSize() const { return _countof(m_TotalItemBase); }
};

#endif // !defined(AFX_ITEMCONTAINER_H__76632FDD_2DBD_4175_B1F5_2F88F531EC1E__INCLUDED_)

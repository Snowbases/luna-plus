// ItemShow.h: interface for the CItemShow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMSHOW_H__9E79636A_8195_4C5B_9F7E_4E8F2A1A1DA0__INCLUDED_)
#define AFX_ITEMSHOW_H__9E79636A_8195_4C5B_9F7E_4E8F2A1A1DA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseItem.h"

class CItemShow  : public CBaseItem
{
protected:
	DWORD m_wItemIdx;
	DURTYPE m_Dur;
public:
	CItemShow();
	virtual ~CItemShow();
	void Init(DWORD wItemIdx, DURTYPE dur);
	virtual inline DWORD		GetItemIdx(){ return m_wItemIdx; }
	// 090811 ShinJS --- 아이템 개수 수정/반환 함수 추가
	virtual inline DURTYPE		GetDurability()				{ return m_Dur;	}
	virtual inline void			SetDurability(DURTYPE dur)	{ m_Dur = dur; }
	virtual void Render();
};

#endif // !defined(AFX_ITEMSHOW_H__9E79636A_8195_4C5B_9F7E_4E8F2A1A1DA0__INCLUDED_)

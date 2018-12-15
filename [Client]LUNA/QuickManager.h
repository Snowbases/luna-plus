// QuickManager.h: interface for the CQuickManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUICKMANAGER_H__B3C070DF_4FEC_4F04_BD3C_8F534A23A442__INCLUDED_)
#define AFX_QUICKMANAGER_H__B3C070DF_4FEC_4F04_BD3C_8F534A23A442__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define QUICKMGR USINGTON(CQuickManager)

// 080702 LYW --- QuickManager : 확장 슬롯 추가에 따른 슬롯 최대 페이지를 4로 변경한다.
#define TOTAL_SLOTPAGE	8
#define MAX_SLOTPAGE	4
#define MAX_SLOTNUM		10

class cIcon;
class cQuickItem;
class CItem;

class CQuickManager  
{
	cQuickItem* mQuickItemTable[TOTAL_SLOTPAGE][MAX_SLOTNUM];
	CIndexGenerator m_IconIndexCreator;

public:
	CQuickManager();
	virtual ~CQuickManager();

	cQuickItem* NewQuickItem( WORD tab, WORD pos );

	//BOOL CanEquip(cIcon*);
	void Release();
	
	void UseQuickItem( WORD tab, WORD pos );

	void NetworkMsgParse( BYTE Protocol, void* pMsg );

	void RefreshQickItem();

	BOOL CheckQPosForItemIdx( DWORD ItemIdx );
	BOOL CheckQPosForDBIdx( DWORD DBIdx );
	void DeleteLinkdedQuickItem(DWORD dwDBIdx) ;
	void UnsealQuickItem(DWORD dwDBIdx);
	void ReleaseQuickItem(cQuickItem*);
	void Quick_Info(LPVOID);
	void Quick_Add_Ack(LPVOID);
	void Quick_Remove_Ack(LPVOID);
	void Quick_Change_Ack(LPVOID);
	void AddToolTip(cQuickItem*);
	void RefreshSkillItem(DWORD skillIndex);
	BOOL CheckItemFromStorage( CItem* pItem );

private:
	void GetImage(cQuickItem*);
	void AddQuickItem(WORD tab, WORD pos, SLOT_INFO*);
	void RemoveQuickItem(WORD tab, WORD pos);
	void ChangeQuickItem(WORD tab1, WORD pos1, WORD tab2, WORD pos2);
};

EXTERNGLOBALTON(CQuickManager);
#endif // !defined(AFX_QUICKMANAGER_H__B3C070DF_4FEC_4F04_BD3C_8F534A23A442__INCLUDED_)

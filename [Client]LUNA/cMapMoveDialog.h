#pragma once
//=================================================================================================
//	FILE		: cMapMoveDialog.h
//	PURPOSE		: Dialog interface to move map.
//	PROGRAMMER	: Yongs Lee
//	DATE		: November 30, 2007
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================





//=========================================================================
//		필요한 헤더파일을 불러온다.
//=========================================================================
#include "./Interface/cDialog.h"





//=========================================================================
//		필요한 컨트롤 인터페이스를 선언한다.
//=========================================================================
class cButton ;
class cListDialog ;
class CItem ;

enum eMapMoveType
{
	eMapMoveNone,
	eNormalStyle,
	eCastle_Rushen,
	eCastle_Zevyn,
	eLimitDungeon,
	eDungeon,
};

class cMapMoveDialog : public cDialog
{
	cPtrList	 m_MapMoveList;
	cListDialog* m_pMapListDlg;
	cButton*	 m_pMoveBtn;
	cButton*	 m_pCloseBtn;
	cButton*	 m_pXCloseBtn;
	int			 m_nSelectedMapNum;
	CItem*		 m_pItem;
  	eMapMoveType m_byUseCase;
	DWORD		 m_dwSelectedNpcID;

public:
	cMapMoveDialog(void);
	virtual ~cMapMoveDialog(void);
	void Linking();
	virtual DWORD ActionEvent(CMouse*);
	virtual void OnActionEvent(LONG id, LPVOID, DWORD event );
	void MoveMapSyn();
	void SetItemInfo(CItem* pItem) { m_pItem = pItem; }
	CItem* GetItemInfo() const { return m_pItem; }
	cListDialog* GetMapListDlg() const { return m_pMapListDlg; }
	void SetUseCase(eMapMoveType type, DWORD dwSelectNpcID=0);
	virtual void SetActive(BOOL);
	DWORD GetMapListCount() const;

protected:
	void InitializeMapList();
	void MoveNotice();
	void ReleaseMapList();
  	void SetMapMoveInfo();
	void SetSiegeWarMoveInfo();
	void SetLimitDungeon();
	void SetDungeon();
};

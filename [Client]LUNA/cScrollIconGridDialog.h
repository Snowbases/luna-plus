#pragma once

#include "stdAfx.h"
#include "Interface/cIconGridDialog.h"

class cButton;
class cGuageBar;


class cScrollIconGridDialog : public cIconGridDialog
{
public:
	cScrollIconGridDialog();
	~cScrollIconGridDialog();

protected:
	cGuageBar* m_pGuageBarX;
	cGuageBar* m_pGuageBarY;

protected:
	DWORD m_dwCurLineY;
	DWORD m_dwCurLineX;
	WORD m_nViewRow;
	WORD m_nViewCol;

	bool m_bWheelProcessX;
	bool m_bWheelProcessY;

protected:
	bool IsMaxLineX(LONG LineX);
	bool IsMaxLineY(LONG LineY);
	bool IsInViewGrid(LONG PosX,LONG PosY);
	void SortIconPos(cIcon * icon);
	
	virtual void	RenderDragOverCell();
	virtual void	RenderIconGridCell();
	virtual void	RenderSelectCell();
	virtual void	ActionEventIconGridCell(CMouse * mouseInfo);

public:
	void InitScrollXY();
	void InitScrollXY(WORD wPosition , BOOL bViewUnitScroll = TRUE);
	bool WheelEventProcess(int iWheelValue);

	void SetViewRowCol(WORD Row,WORD Col);

	void SetWheelProcessXY(bool bProcessX,bool bProcessY);
	void SortShowGridIconPos();
	virtual BOOL	GetCellPosition(LONG mouseX, LONG mouseY, WORD& cellX,WORD& cellY);
	virtual BOOL	GetPositionForXYRef(LONG mouseX, LONG mouseY, WORD& pos);
	virtual BOOL	IsDragOverDraw();

	virtual DWORD	ActionEvent(CMouse * mouseInfo);
	virtual	DWORD	ActionKeyboardEvent( CKeyboard* keyInfo );
	virtual BOOL	AddIcon(WORD pos, cIcon*);
	virtual void	Add(cWindow *);
};
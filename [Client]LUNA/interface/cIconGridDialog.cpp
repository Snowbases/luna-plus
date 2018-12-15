// cIconGridDialog.cpp: implementation of the cIconGridDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cIconGridDialog.h"
#include "cWindowManager.h"
#include "cIcon.h"
#include "../Input/Mouse.h"
#include "cScriptManager.h"
#include "../WindowIDEnum.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cIconGridDialog::cIconGridDialog()
{
	m_nRow=m_nCol=0;
	m_IconDlgID=0;
	m_wCellBorderX=0;
	m_wCellBorderY=0;
	m_wCellWidth=0;
	m_wCellHeight=0;
	m_acceptableIconType=0xffffffff;			//all accept
	m_type = WT_ICONGRIDDIALOG;
	SetRect(&m_cellRect,0,0,0,0);
	m_lCurSelCellPos = -1;
	m_lCurDragOverPos = -1;
	m_nIconType	= WT_ICON;
	m_bItemDraged = FALSE;

	m_pIconGridCell = NULL;
	m_bShowGrid = FALSE;
}

cIconGridDialog::~cIconGridDialog()
{
	SAFE_DELETE_ARRAY(m_pIconGridCell);
}

void cIconGridDialog::InitDialog(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, WORD col, WORD row, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	m_nRow = row;
	m_nCol = col;

	m_pIconGridCell = new cIconGridCell[m_nRow*m_nCol];
	for(int i = 0 ; i < m_nRow*m_nCol ; i++)
	{
		m_pIconGridCell[i].icon = NULL;
		m_pIconGridCell[i].use = NOTUSE;
	}
	
	
	m_wCellBorderX = DEFAULT_CELLBORDER;
	m_wCellBorderY = DEFAULT_CELLBORDER;
	m_wCellWidth = DEFAULT_CELLSIZE;
	m_wCellHeight = DEFAULT_CELLSIZE;
	
	SCRIPTMGR->GetImage( 137, &m_GridLockImage, PFT_HARDPATH );
}
void cIconGridDialog::InitGrid(LONG grid_x, LONG grid_y, WORD cellWid, WORD cellHei, WORD borderX, WORD borderY)
{
	m_gridPos.x = (float)grid_x;
	m_gridPos.y = (float)grid_y;
	m_wCellWidth = cellWid;
	m_wCellHeight = cellHei;
	m_wCellBorderX = borderX;
	m_wCellBorderY = borderY;

	m_cellRect.left = (LONG)m_gridPos.x;
	m_cellRect.right = m_cellRect.left + m_nCol*m_wCellWidth+m_wCellBorderX*(m_nCol+1);
	m_cellRect.top = (LONG)m_gridPos.y;
	m_cellRect.bottom = m_cellRect.top + m_nRow*m_wCellHeight+m_wCellBorderY*(m_nRow+1);
}

BOOL cIconGridDialog::PtInCell(LONG x, LONG y)
{
	/*
	// this is not precise calc
	// must be modified at last
	if(m_absPos.x+m_cellRect.left > x || m_absPos.y+m_cellRect.top > y || m_absPos.x+m_cellRect.right < x || m_absPos.y+m_cellRect.bottom < y)
		return FALSE;
	else
		return TRUE;
	*/
	for(int i = 0 ; i < m_nCol*m_nRow ; i++)
		if(m_pIconGridCell[i].use == USE && m_pIconGridCell[i].icon && m_pIconGridCell[i].icon->IsDepend())
		{
			if(m_pIconGridCell[i].icon->PtInWindow(x,y))
				return TRUE;
		}
	return FALSE;
}

void cIconGridDialog::SetCellRect(RECT * rect)
{
	CopyRect(&m_cellRect, rect); 
}

void cIconGridDialog::SetAbsXY(LONG x, LONG y)
{
	LONG tmpX = x - (LONG)m_absPos.x;
	LONG tmpY = y - (LONG)m_absPos.y;

	for(int i = 0 ; i < m_nCol*m_nRow ; i++)
		if(m_pIconGridCell[i].use == USE && m_pIconGridCell[i].icon && m_pIconGridCell[i].icon->IsDepend())
		{
			m_pIconGridCell[i].icon->SetAbsXY(tmpX+(LONG)m_pIconGridCell[i].icon->GetAbsX(), 
											  tmpY+(LONG)m_pIconGridCell[i].icon->GetAbsY());
		}
	cDialog::SetAbsXY(x,y);

}

DWORD cIconGridDialog::ActionEvent(CMouse * mouseInfo)
{
	if(!m_bActive) return WE_NULL;
	DWORD we = WE_NULL;
	POSTYPE pos;

	we = cDialog::ActionEvent(mouseInfo);

	if( !m_bDisable )
	{
		if(we & WE_LBTNCLICK)
		{
			if(GetPositionForXYRef(mouseInfo->GetMouseEventX(),mouseInfo->GetMouseEventY(),pos))
			{
				m_lCurSelCellPos = pos;
				if( m_ID == DMD_MY_GOOD_FEELING_ICONGRID		|| 
					m_ID == DMD_MY_BAD_FEELING_ICONGRID			|| 
					m_ID == DMD_PARTNER_GOOD_FEELING_ICONGRID	|| 
					m_ID == DMD_PARTNER_BAD_FEELING_ICONGRID	 )
				{
					return we ;
				}
				cbWindowFunc(m_ID, m_pParent, WE_LBTNCLICK);	//PPP
			}
		}
		else if(we & WE_LBTNDBLCLICK)
		{
			if(GetPositionForXYRef(mouseInfo->GetMouseEventX(),mouseInfo->GetMouseEventY(),pos))
			{
				m_lCurSelCellPos = pos;
				cbWindowFunc(m_ID, m_pParent, WE_LBTNDBLCLICK);	//PPP
			}
		}
		else if(we & WE_RBTNDBLCLICK)
		{
			if(GetPositionForXYRef(mouseInfo->GetMouseEventX(),mouseInfo->GetMouseEventY(),pos))
			{
				m_lCurSelCellPos = pos;
				cbWindowFunc(m_ID, this, WE_RBTNDBLCLICK);			
			}
		}
		else if(we & WE_RBTNCLICK)
		{
			if(GetPositionForXYRef(mouseInfo->GetMouseEventX(),mouseInfo->GetMouseEventY(),pos))
			{
				m_lCurSelCellPos = pos;
				cbWindowFunc(m_ID, this, WE_RBTNCLICK);			
			}
		}
		else
		{
			if( mouseInfo->LButtonDrag() )
			{
				if( ( we & WE_MOUSEOVER ) && IsDragOverDraw()
					&& GetPositionForXYRef(mouseInfo->GetMouseX(),mouseInfo->GetMouseY(),pos) )
				{
					m_lCurDragOverPos = pos;
					m_bItemDraged = TRUE;
				}
				else
				{
					m_lCurDragOverPos = -1;
					m_bItemDraged = FALSE;
				}
			}
			else
			{
				if( m_bItemDraged )	//드래그후에 끝냈음.
				{
					if( GetPositionForXYRef(mouseInfo->GetMouseEventX(),mouseInfo->GetMouseEventY(),pos) )
						m_lCurSelCellPos = pos;

					m_bItemDraged = FALSE;
				}

				m_lCurDragOverPos = -1;				
			}
		}
	}

	/*for(int i = 0 ; i < m_nCol*m_nRow ; i++)
	{
		if(m_pIconGridCell[i].use == USE && m_pIconGridCell[i].icon->IsActive() && m_pIconGridCell[i].icon && m_pIconGridCell[i].icon->IsDepend())
			m_pIconGridCell[i].icon->ActionEvent(mouseInfo);
	}*/
	//090330 pdy ScrollIconGridDialog 추가
	ActionEventIconGridCell(mouseInfo);

	return we;
}

void cIconGridDialog::ActionEventIconGridCell(CMouse * mouseInfo)
{
	for(int i = 0 ; i < m_nCol*m_nRow ; i++)
	{
		if(m_pIconGridCell[i].use == USE && m_pIconGridCell[i].icon->IsActive() && m_pIconGridCell[i].icon && m_pIconGridCell[i].icon->IsDepend())
			m_pIconGridCell[i].icon->ActionEvent(mouseInfo);
	}
}


void cIconGridDialog::Render()
{
	cDialog::Render();

	//090330 pdy ScrollIconGridDialog 추가
//	VECTOR2 start_pos;

//	if(m_lCurDragOverPos != -1 && !m_DragOverBGImage.IsNull())
////	if(IsDragOverDraw())
//	{
//		start_pos.x = m_absPos.x+m_cellRect.left+(m_wCellBorderX*(m_lCurDragOverPos%m_nCol+1))+(m_lCurDragOverPos%m_nCol*m_wCellWidth)-2;		// 하드 코딩 -2(taiyo temp)
//		start_pos.y = m_absPos.y+m_cellRect.top+(m_wCellBorderY*(m_lCurDragOverPos/m_nCol+1))+(m_lCurDragOverPos/m_nCol*m_wCellHeight)-2;		// 하드 코딩 -2(taiyo temp)
//		m_DragOverBGImage.RenderSprite(NULL,NULL,0,&start_pos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
//	}
	RenderDragOverCell();


	//// inverse order for tooltip
	//for(int i = m_nCol*m_nRow-1 ; i >= 0  ; --i)
	//	if(m_pIconGridCell[i].use == USE && m_pIconGridCell[i].icon && m_pIconGridCell[i].icon->IsDepend())
	//		m_pIconGridCell[i].icon->Render();

	//090330 pdy ScrollIconGridDialog 추가
	RenderIconGridCell();

	//if(m_lCurSelCellPos != -1 && !m_SelectedBGImage.IsNull())
	//{
	//	start_pos.x = m_absPos.x+m_cellRect.left+(m_wCellBorderX*(m_lCurSelCellPos%m_nCol+1))+(m_lCurSelCellPos%m_nCol*m_wCellWidth)-1;		// 하드 코딩 -2(taiyo temp)
	//	start_pos.y = m_absPos.y+m_cellRect.top+(m_wCellBorderY*(m_lCurSelCellPos/m_nCol+1))+(m_lCurSelCellPos/m_nCol*m_wCellHeight)-1;		// 하드 코딩 -2(taiyo temp)
	//	m_SelectedBGImage.RenderSprite(NULL,NULL,0,&start_pos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
	//}

	//090330 pdy ScrollIconGridDialog 추가
	RenderSelectCell();
}

void cIconGridDialog::RenderDragOverCell()
{
	VECTOR2 start_pos;

	if(m_lCurDragOverPos != -1 && !m_DragOverBGImage.IsNull())
//	if(IsDragOverDraw())
	{
		start_pos.x = m_absPos.x+m_cellRect.left+(m_wCellBorderX*(m_lCurDragOverPos%m_nCol+1))+(m_lCurDragOverPos%m_nCol*m_wCellWidth)-2;		// 하드 코딩 -2(taiyo temp)
		start_pos.y = m_absPos.y+m_cellRect.top+(m_wCellBorderY*(m_lCurDragOverPos/m_nCol+1))+(m_lCurDragOverPos/m_nCol*m_wCellHeight)-2;		// 하드 코딩 -2(taiyo temp)
		m_DragOverBGImage.RenderSprite(NULL,NULL,0,&start_pos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
	}
}


//090330 pdy ScrollIconGridDialog 추가
void cIconGridDialog::RenderIconGridCell()
{
	// inverse order for tooltip
	for(int i = m_nCol*m_nRow-1 ; i >= 0  ; --i)
		if(m_pIconGridCell[i].use == USE && m_pIconGridCell[i].icon && m_pIconGridCell[i].icon->IsDepend())
			m_pIconGridCell[i].icon->Render();
}

//090330 pdy ScrollIconGridDialog 추가
void cIconGridDialog::RenderSelectCell()
{
	VECTOR2 start_pos;

	if(m_lCurSelCellPos != -1 && !m_SelectedBGImage.IsNull())
	{
		start_pos.x = m_absPos.x+m_cellRect.left+(m_wCellBorderX*(m_lCurSelCellPos%m_nCol+1))+(m_lCurSelCellPos%m_nCol*m_wCellWidth)-1;		// 하드 코딩 -2(taiyo temp)
		start_pos.y = m_absPos.y+m_cellRect.top+(m_wCellBorderY*(m_lCurSelCellPos/m_nCol+1))+(m_lCurSelCellPos/m_nCol*m_wCellHeight)-1;		// 하드 코딩 -2(taiyo temp)
		m_SelectedBGImage.RenderSprite(NULL,NULL,0,&start_pos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
	}
}

void cIconGridDialog::SetActive(BOOL val)
{
	if( m_bDisable ) return;
	
	cDialog::SetActiveRecursive(val);

/// need confirm
	m_lCurSelCellPos = -1;
///
	for(int i = 0 ; i < m_nCol*m_nRow ; i++)
		if(m_pIconGridCell[i].use == USE && m_pIconGridCell[i].icon && m_pIconGridCell[i].icon->IsDepend())
			m_pIconGridCell[i].icon->SetActive(val);
}

//090119 pdy hide window
void cIconGridDialog::SetHide(BOOL val)
{
	if( m_bDisable ) return;

	cDialog::SetHideRecursive(val);

	for(int i = 0 ; i < m_nCol*m_nRow ; i++)
		if(m_pIconGridCell[i].use == USE && m_pIconGridCell[i].icon && m_pIconGridCell[i].icon->IsDepend())
			m_pIconGridCell[i].icon->SetHide(val);

}

void cIconGridDialog::SetAlpha(BYTE al)
{ 
	cDialog::SetAlpha(al);
	for(int i = 0 ; i < m_nCol*m_nRow ; i++)
	{
		if(m_pIconGridCell[i].use == USE && m_pIconGridCell[i].icon)
			m_pIconGridCell[i].icon->SetAlpha(al);
	}
}
/**/
/**/
/**/
/**/
// 1-dimension
BOOL cIconGridDialog::IsAddable(WORD pos)
{
	if( pos >= m_nRow * m_nCol )
	{
		return FALSE;
	}
	
	if(m_pIconGridCell[pos].use == USE)
		return FALSE;
	return TRUE;
}
BOOL cIconGridDialog::AddIcon(WORD pos, cIcon * icon)
{
	if( pos >= m_nCol*m_nRow )
	{
		return FALSE;
	}

	WORD cellX = pos%m_nCol;
	WORD cellY = pos/m_nCol;

//	ASSERT(m_pIconGridCell[pos].use == USE);
	if(m_pIconGridCell[pos].use == USE) 
	{
		return FALSE;
	}
	icon->SetAbsXY((LONG)(m_absPos.x+m_cellRect.left+(m_wCellBorderX*(cellX+1))+((cellX)*m_wCellWidth)), (LONG)(m_absPos.y+m_cellRect.top+(m_wCellBorderY*(cellY+1))+((cellY)*m_wCellHeight)));
	icon->SetDepend(TRUE);
	icon->SetActive(m_bActive);
	m_pIconGridCell[pos].icon = icon;
	

	m_pIconGridCell[pos].use = USE;
	
	icon->SetParent(this);
	//현재 창이 디스에이블상태면 아이콘도 디스에이블로
	if( m_bDisable )
		icon->SetDisable( TRUE );

	icon->SetCellPosition(cellX,cellY);
	return TRUE;
}

BOOL cIconGridDialog::GetCellAbsPos(WORD pos, int& absX, int& absY)
{
	if(pos >= m_nCol*m_nRow)
	{
		return FALSE;
	}
	
	WORD cellX = pos%m_nCol;
	WORD cellY = pos/m_nCol;

	if(m_pIconGridCell[pos].use == NOTUSE) return FALSE;

	absX = (int)(m_absPos.x+m_cellRect.left+(m_wCellBorderX*(cellX+1))+((cellX)*m_wCellWidth));
	absY = (int)(m_absPos.y+m_cellRect.top+(m_wCellBorderY*(cellY+1))+((cellY)*m_wCellHeight));

	return TRUE;
}

// 2-dimension
BOOL cIconGridDialog::GetCellPosition(LONG mouseX, LONG mouseY, WORD& cellX,WORD& cellY)
{
	for(WORD i = 0 ; i < m_nCol ; ++i)
	{
		for(WORD j = 0 ; j < m_nRow ; ++j)
		{
			int cellpX = (int)(m_absPos.x+m_cellRect.left+(m_wCellBorderX*(i+1))+(i*m_wCellWidth));
			int cellpY = (int)(m_absPos.y+m_cellRect.top+(m_wCellBorderY*(j+1))+(j*m_wCellHeight));
			if(cellpX < mouseX && mouseX < cellpX + DEFAULT_CELLSIZE &&
				cellpY < mouseY && mouseY < cellpY + DEFAULT_CELLSIZE)
			{

				cellX = i;
				cellY = j;
				return TRUE;
			}
		}
		
	}
	return FALSE;
}

BOOL cIconGridDialog::GetPositionForXYRef(LONG mouseX, LONG mouseY, WORD& pos)
{
	WORD x,y;
	BOOL rt = GetCellPosition(mouseX, mouseY, x, y);
	pos = y*m_nCol+x;	//tmp) pos = GetPositionForCell(x,y);
	return rt;
}

WORD cIconGridDialog::GetPositionForCell(WORD cellX, WORD cellY)
{
	return (cellY)*m_nCol+(cellX);
}
BOOL cIconGridDialog::IsAddable(WORD cellX,WORD cellY,cIcon* pIcon)
{
	const WORD position = cellY*m_nCol+cellX;
	
	if( position >= m_nRow * m_nCol )
	{
		return FALSE;
	}
	
	if(m_pIconGridCell[ position ].use == USE)
		return FALSE;
	if(!(pIcon->GetIconType() & m_acceptableIconType))
		return FALSE;

	return TRUE;
}

BOOL cIconGridDialog::AddIcon(WORD cellX, WORD cellY, cIcon * icon)
{
	return AddIcon(cellX+cellY*m_nCol, icon);
}

BOOL cIconGridDialog::MoveIcon(WORD cellX, WORD cellY, cIcon * icon)
{
//KES 040511 에러리턴 수정
	if( !IsAddable( GetPositionForCell( cellX, cellY ) ) )
		return FALSE;

	cIconGridDialog* pBeforeGrid = (cIconGridDialog*)icon->GetParent();

	WORD wOldCellX = icon->GetCellX();
	WORD wOldCellY = icon->GetCellY();

	if( pBeforeGrid->DeleteIcon(icon) == FALSE )
		return FALSE;

	if( AddIcon( cellX, cellY, icon ) )
	{
		return TRUE;
	}
	else
	{
		pBeforeGrid->AddIcon( wOldCellX, wOldCellY, icon );
		return FALSE;
	}
}

BOOL cIconGridDialog::DeleteIcon(cIcon * icon)
{
	cIcon * dummy;
	
	return DeleteIcon( icon->GetCellX(), icon->GetCellY(), &dummy);
}
BOOL cIconGridDialog::DeleteIcon(WORD pos, cIcon ** icon)
{
	if( pos >= m_nCol * m_nRow )
	{
		return FALSE;
	}

	cIconGridCell& cell = m_pIconGridCell[pos];

	if(cell.use == NOTUSE)
	{
		if( icon ) 
		{
			*icon = NULL;
		}
		
		//ASSERT(0);
		return FALSE;
	}

	if( icon ) 
	{
		*icon = cell.icon;
	}
	
	cell.icon->SetCellPosition(0,0);
	cell.icon->SetDepend(FALSE);

	// 071218 LUJ, 이렇게 하지 않을 경우 자칫하면 잠기는 아이콘이 생길 수 있음
	cell.icon->SetDisable( FALSE );
	cell.icon->SetLock( FALSE );

	cell.icon = NULL;
	cell.use = NOTUSE;
	
	return TRUE;
}


BOOL cIconGridDialog::DeleteIcon(WORD cellX, WORD cellY, cIcon ** iconOut)
{
	return DeleteIcon(cellY*m_nCol+cellX, iconOut);
}

void cIconGridDialog::SetDisable( BOOL val )
{
	cDialog::SetDisable( val );

	for(int i = 0 ; i < m_nRow*m_nCol ; i++)
	{
		if( m_pIconGridCell[i].icon )
			m_pIconGridCell[i].icon->SetDisable( val );
	}	
}

BOOL cIconGridDialog::IsDragOverDraw()
{
	if( !WINDOWMGR->IsDragWindow() ) return FALSE;

	cDialog* pDlg = WINDOWMGR->GetDragDlg();

	if( pDlg )
	if( pDlg->GetType() == m_nIconType )
		return TRUE;

	return FALSE;
}


cIcon* cIconGridDialog::GetIconForIdx( WORD idx)
{
	return ( idx >= m_nRow * m_nCol ) ? 0 : m_pIconGridCell[idx].icon;
}
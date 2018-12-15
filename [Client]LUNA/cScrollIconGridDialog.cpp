#include "cScrollIconGridDialog.h"
#include "../WindowIDEnum.h"

#include "Interface/cIconGridDialog.h"
#include "Interface/cGuageBar.h"
#include "Interface/cButton.h"
#include "Interface/cIcon.h"
#include "Interface/cIconDialog.h"
#include "Interface/cWindowManager.h"
#include "../Input/Mouse.h"
#include "../Input/Keyboard.h"
#include "../Input/UserInput.h"

cScrollIconGridDialog::cScrollIconGridDialog()
{
	m_dwCurLineY	 = 0;
	m_dwCurLineX	 = 0;
	m_pGuageBarX	 = NULL;
	m_pGuageBarY	 = NULL;
	SetWheelProcessXY(0,1);		//디폴트는 y만 휠이벤트를 받자

	m_type = WT_SCROLLICONGRIDDLG;
	m_nIconType = WT_HOUSING_STORED_ICON;
}
cScrollIconGridDialog::~cScrollIconGridDialog()
{
}

void cScrollIconGridDialog::InitScrollXY()
{
	m_dwCurLineY	 = 0;
	m_dwCurLineX	 = 0;

	if( m_pGuageBarY ) 
		m_pGuageBarY->SetCurRate( 0.0f) ;

	if( m_pGuageBarX ) 
		m_pGuageBarX->SetCurRate( 0.0f) ;

	SortShowGridIconPos();
}

//아이콘 포지션에 맞게 스크롤을 해준다. 
//bViewUnitScroll가 TRUE면 실제 뷰사이즈 단위로 스크롤해준다  
//bViewUnitScroll가 FALSE면 포지션이 맨위로 오게 스크롤 해준다.
void cScrollIconGridDialog::InitScrollXY(WORD pos , BOOL bViewUnitScroll )
{
	if( pos >= m_nCol*m_nRow )
	{
		//포지션이 한도치를 넘으면 기본 값으로 Init
		InitScrollXY();
		return;
	}

	WORD cellX = pos%m_nCol;
	WORD cellY = pos/m_nCol;

	DWORD dwLineX = IsMaxLineX( cellX ) ? m_nCol - m_nViewCol : cellX ;
	DWORD dwLineY = IsMaxLineY( cellY ) ? m_nRow - m_nViewRow : cellY ;

	if( bViewUnitScroll )
	{
		//ViewCol ViewRow단위로 잘라서 맞춰준다. 
		dwLineX = IsMaxLineX( cellX ) ? dwLineX : dwLineX - (dwLineX % m_nViewCol) ;
		dwLineY = IsMaxLineY( cellY ) ? dwLineY : dwLineY - (dwLineY % m_nViewRow) ;
	}

	m_dwCurLineY	 = dwLineY;
	m_dwCurLineX	 = dwLineX;

	if( m_pGuageBarY ) 
	{
		float fRate = float(m_dwCurLineY) / float( m_nRow - m_nViewRow );
		m_pGuageBarY->SetCurRate( fRate + 0.001f) ;
	}

	if( m_pGuageBarX ) 
	{
		float fRate = float(m_dwCurLineX) / float( m_nCol - m_nViewCol ) ;
		m_pGuageBarX->SetCurRate( fRate + 0.001f) ;
	}

	SortShowGridIconPos();
}

void cScrollIconGridDialog::Add(cWindow *pWin)
{
	if( ! pWin )
		return;

	cIconGridDialog::Add(pWin);

	if( pWin->GetType() == WT_GUAGEBAR )
	{
		cGuageBar* pBar = (cGuageBar*)pWin;

		if( pBar->IsVertical() )
		{
			if( ! m_pGuageBarY ) 
				m_pGuageBarY = pBar;
		}
		else
		{
			if( ! m_pGuageBarX ) 
				m_pGuageBarX = pBar;
		}
	}
}

BOOL cScrollIconGridDialog::AddIcon(WORD pos, cIcon * pIcon)
{
	BOOL bResult = cIconGridDialog::AddIcon(pos , pIcon);

	if( bResult ) 
	{
		if( IsInViewGrid(pIcon->GetCellX(),pIcon->GetCellY()) )
			SortIconPos(pIcon);
	}

	return bResult;
}

void cScrollIconGridDialog::SetViewRowCol(WORD Row,WORD Col)
{
	m_nViewRow=Row;m_nViewCol=Col;
}

void cScrollIconGridDialog::SetWheelProcessXY(bool bProcessX,bool bProcessY)
{
	m_bWheelProcessX = bProcessX ; 
	m_bWheelProcessY = bProcessY ;

	if( ! m_pGuageBarX ) 
		m_bWheelProcessY = TRUE;

	if( ! m_pGuageBarY ) 
		m_bWheelProcessX = TRUE;
}

bool cScrollIconGridDialog::IsMaxLineX(LONG LineX)
{
	//m_nCol	 : 실제 그리드 Col (데이터) MaxLineX
	//m_nViewCol : 보여지는 그리드 Col 
	if( LineX < 0 )
		return true;

	return ( ( LineX + m_nViewCol)  >  m_nCol ); 
}

bool cScrollIconGridDialog::IsMaxLineY(LONG LineY)
{
	//m_nRow	 : 실제 그리드 Row (데이터) MaxLineY
	//m_nViewRow : 보여지는 그리드 Row 
	if( LineY < 0 )
		return true;

	return ( ( LineY + m_nViewRow)  >  m_nRow ); 
}

bool cScrollIconGridDialog::IsInViewGrid(LONG PosX,LONG PosY)
{
	if( PosX < LONG(m_dwCurLineX) ||
		PosY < LONG(m_dwCurLineY) ||
		PosX > LONG( m_dwCurLineX + m_nViewCol-1 ) || 
		PosY > LONG( m_dwCurLineY + m_nViewRow-1 )    )
		return false;

	return true;
}

BOOL cScrollIconGridDialog::GetCellPosition(LONG mouseX, LONG mouseY, WORD& cellX,WORD& cellY)
{
	//마우스 X Y를 넣어서 셀 X Y 인덱스를 가져온다
	//가로
	for(int i = 0 ; i < m_nViewCol ; i++)	
	{
		//세로 
		for(int j = 0 ; j < m_nViewRow ; j++)
		{
			//셀pX = 아이템위치 + 그리드위치 + 가로+1 + (가로 * 셀의 가로길이)
			int cellpX = (int)(m_absPos.x+m_cellRect.left+(m_wCellBorderX*(i+1))+(i*m_wCellWidth));
		
			int cellpY = (int)(m_absPos.y+m_cellRect.top+(m_wCellBorderY*(j+1))+((j)*m_wCellHeight));

			if(cellpX < mouseX && mouseX < cellpX + DEFAULT_CELLSIZE &&
				cellpY < mouseY && mouseY < cellpY + DEFAULT_CELLSIZE)
			{
				cellX = WORD(i+m_dwCurLineX);
				cellY = WORD(j+m_dwCurLineY);
				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL cScrollIconGridDialog::GetPositionForXYRef(LONG mouseX, LONG mouseY, WORD& pos)
{
	WORD x,y;
	BOOL rt = GetCellPosition(mouseX, mouseY, x, y);
	pos = y*m_nCol+x;	//tmp) pos = GetPositionForCell(x,y);
	return rt;

}

BOOL cScrollIconGridDialog::IsDragOverDraw()
{
	if( !WINDOWMGR->IsDragWindow() ) return FALSE;
	cDialog* pDlg = WINDOWMGR->GetDragDlg();

	if( pDlg )
	if( pDlg->GetType() == m_nIconType )
		return TRUE;

	return FALSE;
}

void cScrollIconGridDialog::SortIconPos(cIcon * pIcon)
{
	if( ! pIcon )
		return;

	WORD wPosX = pIcon->GetCellX();
	WORD wPosY = pIcon->GetCellY();
	WORD wScrollHeight = WORD(m_dwCurLineY *  m_wCellHeight);
	WORD wScrollWidth  = WORD(m_dwCurLineX *  m_wCellWidth);
	LONG CurCellAbsX = ( wPosX * m_wCellWidth ) +  (m_wCellBorderX * ( wPosX - m_dwCurLineX +1 ) ) - wScrollWidth;
	LONG CurCellAbsY = ( wPosY * m_wCellHeight) +  (m_wCellBorderY * ( wPosY - m_dwCurLineY +1 ) ) - wScrollHeight;

	pIcon->SetAbsXY(
		LONG(m_absPos.x + m_cellRect.left + CurCellAbsX),
		LONG(m_absPos.y + m_cellRect.top + CurCellAbsY));
}

void cScrollIconGridDialog::SortShowGridIconPos()
{
	cDialog* pDragDlg = NULL;

	if( m_bItemDraged )
		pDragDlg = WINDOWMGR->GetDragDlg();


	for( int y= 0 ; y <  m_nViewRow ;  y++)
	{
		cIconGridCell * pScrollColGridCell = &m_pIconGridCell[ ( (y +  m_dwCurLineY ) * m_nCol) + m_dwCurLineX ];

		for( int x = 0 ; x < m_nViewCol ; x++)
		{
			if(  pScrollColGridCell[x].icon )
			{
				if( pScrollColGridCell[x].icon == pDragDlg )
				{
					continue;
				}
				//정렬되야할 가로 세로로  
				LONG CurCellAbsX =  ( x * m_wCellWidth)  +  (m_wCellBorderX * (x+1) );
				LONG CurCellAbsY =  ( y * m_wCellHeight) +  (m_wCellBorderY * (y+1) );
				pScrollColGridCell[x].icon->SetAbsXY(
					LONG(m_absPos.x+m_cellRect.left + CurCellAbsX),
					LONG(m_absPos.y+m_cellRect.top + CurCellAbsY));
			}
		}
	}
}

void cScrollIconGridDialog::ActionEventIconGridCell(CMouse * mouseInfo)
{
	for( int y= 0 ; y <  m_nViewRow ;  y++)
	{
		cIconGridCell * pScrollColGridCell = &m_pIconGridCell[ ( (y +  m_dwCurLineY ) * m_nCol ) + m_dwCurLineX ];

		for( int x = 0 ; x < m_nViewCol ; x++)
		{
			if(pScrollColGridCell[x].use == USE && pScrollColGridCell[x].icon->IsActive() && pScrollColGridCell[x].icon && pScrollColGridCell[x].icon->IsDepend())
				pScrollColGridCell[x].icon->ActionEvent(mouseInfo);
		}
	}
}

DWORD cScrollIconGridDialog::ActionEvent(CMouse * mouseInfo)
{
	if(!m_bActive) return WE_NULL;

	DWORD we = WE_NULL;

	ActionKeyboardEvent(KEYBOARD);

	if( m_pGuageBarX )
		m_pGuageBarX->SetActive(FALSE);

	if( m_pGuageBarY )
		m_pGuageBarY->SetActive(FALSE);

	we |= cIconGridDialog::ActionEvent(mouseInfo);

	if( m_pGuageBarX )
		m_pGuageBarX->SetActive(TRUE);

	if( m_pGuageBarY )
		m_pGuageBarY->SetActive(TRUE);

	BOOL bSortIconPos = FALSE;

	if( GetParent() )
	if( ((cWindow*)GetParent())->PtInWindow((LONG)mouseInfo->GetMouseX(), (LONG)mouseInfo->GetMouseY()) )	//테스트임 밖에서 빼게 만들자 
	{
		bSortIconPos = WheelEventProcess( mouseInfo->GetWheel() );
	}

	if( m_pGuageBarX)
	{
		we |= m_pGuageBarX->ActionEvent(mouseInfo) ;

		float fRate = m_pGuageBarX->GetCurRate();
		DWORD LineX =  DWORD( (m_nCol - m_nViewCol) * fRate );
	
		if( LineX != m_dwCurLineX)
		{
			if( ! IsMaxLineX(LineX) ) 
			{
				m_dwCurLineX = LineX;
				bSortIconPos = TRUE;
			}
		}	
	}

	if( m_pGuageBarY)
	{
		we |= m_pGuageBarY->ActionEvent(mouseInfo) ;

		float fRate = m_pGuageBarY->GetCurRate();
		DWORD LineY =  DWORD( (m_nRow - m_nViewRow) * fRate );
	
		if( LineY != m_dwCurLineY)
		{
			if( ! IsMaxLineY(LineY) ) 
			{
				m_dwCurLineY = LineY;
				bSortIconPos = TRUE;
			}
		}	
	}

	if( bSortIconPos ) 
		SortShowGridIconPos();

	return we;
}

DWORD cScrollIconGridDialog::ActionKeyboardEvent( CKeyboard* keyInfo )
{
	DWORD we = WE_NULL;

	cIconGridDialog::ActionKeyboardEvent( keyInfo );

	BOOL bCtrl = keyInfo->GetKeyPressed( KEY_CONTROL );
	SetWheelProcessXY(
		TRUE == bCtrl,
		FALSE == bCtrl);

	return we;
}

bool cScrollIconGridDialog::WheelEventProcess(int iWheelValue)
{
	if(	!m_bWheelProcessX && !m_bWheelProcessY ||
		!m_pGuageBarX && !m_pGuageBarY			)
		return FALSE;

	BOOL bResult = FALSE;

	if(iWheelValue)
	{
		if( m_bWheelProcessX && m_pGuageBarX )
		{
			int LineX = m_dwCurLineX;

			if( iWheelValue > 0 )
			{
				if( ! IsMaxLineX(LineX -1) )
				{
					float fRate = 0.0f;
					fRate = float(LineX-1) / float( m_nCol - m_nViewCol ) ;
					m_pGuageBarX->SetCurRate( fRate + 0.001f) ;

					m_dwCurLineX -= 1;
					bResult       = TRUE;
				}
			}
			else
			{
				if( ! IsMaxLineX(LineX +1) ) 
				{
					float fRate =  float(LineX+1) / float( m_nCol - m_nViewCol ) ; 
					m_pGuageBarX->SetCurRate( fRate + 0.001f) ;

					m_dwCurLineX += 1;
					bResult       = TRUE;
				}
			}
		}

		if( m_bWheelProcessY && m_pGuageBarY )
		{
			int LineY = m_dwCurLineY;

			if( iWheelValue > 0 )
			{
				if( ! IsMaxLineY(LineY -1) )
				{
					float fRate = 0.0f;
					fRate = float(LineY-1) / float( m_nRow - m_nViewRow );
					m_pGuageBarY->SetCurRate( fRate + 0.001f);

					m_dwCurLineY -= 1;
					bResult       = TRUE;
				}
			}
			else
			{
				if( ! IsMaxLineY(LineY +1) ) 
				{
					float fRate =  float(LineY+1) / float( m_nRow - m_nViewRow ); 
					m_pGuageBarY->SetCurRate( fRate + 0.001f );

					m_dwCurLineY += 1;
					bResult       = TRUE;
				}
			}
		}

	} //if(Wheel)

	return TRUE == bResult;
}

void cScrollIconGridDialog::RenderDragOverCell()
{
	VECTOR2 start_pos;

	if( IsInViewGrid( m_lCurDragOverPos%m_nCol , m_lCurDragOverPos /m_nCol ) )
	{
		if(m_lCurDragOverPos != -1 && !m_DragOverBGImage.IsNull())
		{
			LONG ViewGridPos = m_lCurDragOverPos - (m_dwCurLineY * m_nCol) - m_dwCurLineX;  

			start_pos.x = m_absPos.x+m_cellRect.left+(m_wCellBorderX*(ViewGridPos%m_nCol+1))+(ViewGridPos%m_nCol*m_wCellWidth)-1;		// 하드 코딩 -2(taiyo temp)
			start_pos.y = m_absPos.y+m_cellRect.top+(m_wCellBorderY*(ViewGridPos/m_nCol+1))+(ViewGridPos/m_nCol*m_wCellHeight)-1;		// 하드 코딩 -2(taiyo temp)
			m_DragOverBGImage.RenderSprite(NULL,NULL,0,&start_pos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
		}
	}

}
void cScrollIconGridDialog::RenderIconGridCell()
{
	cDialog* pDragDlg = NULL;

	if( m_bItemDraged )
		pDragDlg = WINDOWMGR->GetDragDlg();

	for( int y= 0 ; y <  m_nViewRow ; y++)
	{
		cIconGridCell * pScrollColGridCell = &m_pIconGridCell[ ( (y +  m_dwCurLineY ) * m_nCol ) + m_dwCurLineX ];

		for( int x = 0 ; x < m_nViewCol ; x++)
		{
			if(pScrollColGridCell[x].use == cIconDialog::USE && pScrollColGridCell[x].icon && pScrollColGridCell[x].icon->IsDepend())
			{
				if( pScrollColGridCell[x].icon == pDragDlg )
				{
					int a=0;
					a++;
				}
				pScrollColGridCell[x].icon->Render();
			}
		}
	}
}

void cScrollIconGridDialog::RenderSelectCell()
{
	VECTOR2 start_pos;

	if( IsInViewGrid( m_lCurSelCellPos%m_nCol , m_lCurSelCellPos /m_nCol ) )
	{
		if(m_lCurSelCellPos != -1 && !m_SelectedBGImage.IsNull())
		{
			LONG ViewGridPos = m_lCurSelCellPos - (m_dwCurLineY * m_nCol) - m_dwCurLineX;  

			start_pos.x = m_absPos.x+m_cellRect.left+(m_wCellBorderX*(ViewGridPos%m_nCol+1))+(ViewGridPos%m_nCol*m_wCellWidth)-1;		// 하드 코딩 -2(taiyo temp)
			start_pos.y = m_absPos.y+m_cellRect.top+(m_wCellBorderY*(ViewGridPos/m_nCol+1))+(ViewGridPos/m_nCol*m_wCellHeight)-1;		// 하드 코딩 -2(taiyo temp)
			m_SelectedBGImage.RenderSprite(NULL,NULL,0,&start_pos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
		}
	}
}
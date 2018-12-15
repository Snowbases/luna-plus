// cListCtrlEx.cpp: implementation of the cListCtrlEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cListCtrlEx.h"
#include "cGuageBar.h"
#include "cButton.h"
#include "./Interface/cFont.h"

cLCColumn::cLCColumn()
{
	m_nColumnWidth	= 50;	//default
	m_nLeftMargin	= 0;	//default 
	m_nTopMargin	= 0;	//default
	m_nAlign		= TXT_LEFT | TXT_TOP;
}

cLCColumn::~cLCColumn()
{
	RemoveAll();
}


void cLCColumn::RemoveAll()
{
	POS pos = m_ListItem.GetFirstPos();

	for(cLCItemBase* pItem = m_ListItem.GetNextPos(pos);
		0 < pItem;
		pItem = m_ListItem.GetNextPos(pos))
	{
		delete pItem;
	}

	m_ListItem.DeleteAll();
}

void cLCColumn::Render( LONG x, LONG y, int nStartIndex, int nEndIndex, int nRowHeight )
{
	LONG posx = x + m_nLeftMargin;
	LONG posy = y + m_nTopMargin;

	POS pos = m_ListItem.GetIndexPos( nStartIndex );

	cLCItemBase* pItem;

	for( int i = nStartIndex ; i < nEndIndex ; ++i )
	{
		pItem = m_ListItem.GetNextPos(pos);
		if( pItem )
			pItem->Render( posx, posy );

		posy += nRowHeight;
	}
}




//////////////
//cListCotrlEx
cListCtrlEx::cListCtrlEx()
{
	m_pColumn				= NULL;
	m_nColumnNum			= 0;
	m_nRowNum				= 0;
	m_nFirstViewRowIndex	= 0;
	m_nViewRowCount			= 10;	//default
	m_nRowHeight			= 20;	//default

	m_nTopMargin			= 0;
	m_nLeftMargin			= 0;
}

cListCtrlEx::~cListCtrlEx()
{
	Release();
}

void cListCtrlEx::InitListCtrlEx( int nColumnNum, int nViewRowCount, int nRowHeight, int nLeftMargin, int nTopMargin )
{
	m_pColumn		= new cLCColumn[nColumnNum];
	m_nColumnNum	= nColumnNum;
	m_nViewRowCount	= nViewRowCount;
	m_nRowHeight	= nRowHeight;
	m_nTopMargin	= nTopMargin;
	m_nLeftMargin	= nLeftMargin;
}

void cListCtrlEx::Release()
{
	SAFE_DELETE_ARRAY( m_pColumn );
}

void cListCtrlEx::InitListCtrlExImage( cImage* pTop, cImage* pMiddle, cImage* pBottom, cImage* pSelected, cImage* pMouseOn )
{
	m_ImgTop		= *pTop;
	m_ImgMiddle		= *pMiddle;
	m_ImgBottom		= *pBottom;
	m_ImgSelected	= *pSelected;
	m_ImgMouseOn	= *pMouseOn;
}

void cListCtrlEx::CreateNewItem( cLCItemBase** ppItem )
{
	cLCItemBase* pItem = NULL;
	pItem = new cLCItemBase[m_nColumnNum];

	*ppItem = pItem;
}

void cListCtrlEx::AddTail( cLCItemBase** ppItem )
{
	for( int i = 0 ; i < m_nColumnNum ; ++i )
	{
		m_pColumn[i].AddTail( ppItem[i] );
	}

	++m_nRowNum;
}

void cListCtrlEx::AddHead( cLCItemBase** ppItem )
{
	for( int i = 0 ; i < m_nColumnNum ; ++i )
	{
		m_pColumn[i].AddHead( ppItem[i] );
	}

	++m_nRowNum;
}

void cListCtrlEx::Remove( int nIndex )
{
	cLCItemBase* pItem;
	for( int i = 0 ; i < m_nColumnNum ; ++i )
	{
		pItem = m_pColumn[i].Remove( nIndex );
		SAFE_DELETE( pItem );
	}

	--m_nRowNum;
}

void cListCtrlEx::Render()
{
	cDialog::Render();
	if( !m_bActive ) return;

	int nEndIndex = m_nRowNum < m_nFirstViewRowIndex + m_nViewRowCount ?
					m_nRowNum : m_nFirstViewRowIndex + m_nViewRowCount;

	LONG x = (LONG)m_absPos.x;
	LONG y = (LONG)m_absPos.y;

	for( int i = 0 ; i < m_nColumnNum ; ++i )
	{
		m_pColumn[i].Render( x, y, m_nFirstViewRowIndex, nEndIndex, m_nRowHeight );
		x += m_pColumn[i].GetColumnWidth();
	}
}
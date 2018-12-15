#include "StdAfx.h"
#include "SimpleTextListCtrl.h"

CSimpleTextListCtrl::CSimpleTextListCtrl(void)
{
	m_maxColumn = 0;
}

CSimpleTextListCtrl::~CSimpleTextListCtrl(void)
{
}

void CSimpleTextListCtrl::SetItemText( int row, int col, LPCTSTR text )
{
	if( text == NULL )
		return;

	TEXTMAP& textmap = m_textList[ row ];
	textmap[ col ] = text;

	m_maxColumn = m_maxColumn < col + 1 ? col + 1 : m_maxColumn;
}

LPCTSTR CSimpleTextListCtrl::GetItemText( int row, int col ) const
{
	std::map< int, TEXTMAP >::const_iterator iterTextMap = m_textList.find( row );
	if( iterTextMap == m_textList.end() )
		return _T("");

	const TEXTMAP& textmap = iterTextMap->second;
	TEXTMAP::const_iterator iterTextInfo = textmap.find( col );
	if( iterTextInfo == textmap.end() )
		return _T("");

	const std::string& text = iterTextInfo->second;
	return text.c_str();
}

int CSimpleTextListCtrl::GetRowCount() const
{
	return (int)m_textList.size();
}

int CSimpleTextListCtrl::GetColCount() const
{
	return m_maxColumn;
}
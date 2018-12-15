// MouseCursor.cpp: implementation of the CMouseCursor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MouseCursor.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CMouseCursor)


CMouseCursor::CMouseCursor()
{

}

CMouseCursor::~CMouseCursor()
{

}


void CMouseCursor::Init( HINSTANCE hInstance )
{
//	ShowCursor(TRUE);

//	m_hCursor[eCURSOR_DEFAULT] = LoadCursorFromFile("Cursor/default.cur");
//	m_hCursor[eCURSOR_ATTACK] = LoadCursorFromFile("Cursor/attack.cur");
//	m_hCursor[eCURSOR_ITEMSELECT] = LoadCursorFromFile("Cursor/ItemSelect.cur");
//	m_hCursor[eCURSOR_DEALER] = LoadCursorFromFile("Cursor/dealer.cur");
//	m_hCursor[eCURSOR_DONTMOVE] = LoadCursorFromFile("Cursor/dontmove.cur");

	m_bActive = FALSE;

	m_hCursor[eCURSOR_DEFAULT]		= LoadCursorFromFile("./Data/Interface/2DImage/mouse/default.cur");
	m_hCursor[eCURSOR_ATTACK]		= LoadCursorFromFile("./Data/Interface/2DImage/mouse/attack.cur");
	m_hCursor[eCURSOR_ITEMSELECT]	= LoadCursorFromFile("./Data/Interface/2DImage/mouse/itemsel.cur");
	m_hCursor[eCURSOR_ITEMDRAGGING]	= LoadCursorFromFile("./Data/Interface/2DImage/mouse/itemsel.cur");
	m_hCursor[eCURSOR_DEALER]		= LoadCursorFromFile("./Data/Interface/2DImage/mouse/dealer.cur");
	//m_hCursor[eCURSOR_CANTMOVE]		= LoadCursorFromFile("./Data/Interface/2DImage/mouse/cantmove.cur");
	m_hCursor[eCURSOR_CANTMOVE]		= LoadCursorFromFile("./Data/Interface/2DImage/mouse/scv.cur");
	m_hCursor[eCURSOR_DEFAULTCHEAT] = LoadCursorFromFile("./Data/Interface/2DImage/mouse/defaultcheat.cur");
	m_hCursor[eCURSOR_HOUSING_CANNOT_INSTALL] = LoadCursorFromFile("./Data/Interface/2DImage/mouse/Housing_fail.cur");
	m_hCursor[eCURSOR_VEHICLE]		= LoadCursorFromFile("./Data/Interface/2DImage/mouse/vehicle.cur");
	m_hCursor[eCURSOR_HOUSING_ACTION_FURNITURE]	= LoadCursorFromFile("./Data/Interface/2DImage/mouse/Housing_Action.cur");

/*
	m_hCursor[eCURSOR_DEFAULT]		= LoadCursor( hInstance, MAKEINTRESOURCE(IDC_CURSOR_DEFAULT) );
	m_hCursor[eCURSOR_ATTACK]		= LoadCursor( hInstance, MAKEINTRESOURCE(IDC_CURSOR_ATTACK) );
	m_hCursor[eCURSOR_ITEMSELECT]	= LoadCursor( hInstance, MAKEINTRESOURCE(IDC_CURSOR_SELECT) );
	m_hCursor[eCURSOR_ITEMDRAGGING]	= LoadCursor( hInstance, MAKEINTRESOURCE(IDC_CURSOR_SELECT) );
	m_hCursor[eCURSOR_DEALER]		= LoadCursor( hInstance, MAKEINTRESOURCE(IDC_CURSOR_SPEECH) );
	m_hCursor[eCURSOR_CANTMOVE]		= LoadCursor( hInstance, MAKEINTRESOURCE(IDC_CURSOR_CANTMOVE) );
	m_hCursor[eCURSOR_DEFAULTCHEAT] = LoadCursor( hInstance, MAKEINTRESOURCE(IDC_CURSOR_DOT) );
*/
	m_curCursor = eCURSOR_DEFAULT;
	
	m_bCheatCursor = FALSE;
	
//	GetClipCursor(&m_rcOldClip); 
//	GetWindowRect(GetActiveWindow(), &m_rcClip); 
//	ClipCursor(&m_rcClip);
}

void CMouseCursor::Release()
{
//	ClipCursor(&m_rcOldClip);
/*
//	BOOL b;
	DeleteObject( m_hCursor[eCURSOR_DEFAULT] );
//	b =
	DeleteObject( m_hCursor[eCURSOR_ATTACK] );
//	b =
	DeleteObject( m_hCursor[eCURSOR_ITEMSELECT] );
//	b =
	DeleteObject( m_hCursor[eCURSOR_ITEMDRAGGING] );	
//	b =
	DeleteObject( m_hCursor[eCURSOR_DEALER] );
//	b =
	DeleteObject( m_hCursor[eCURSOR_CANTMOVE] );
//	b =
	DeleteObject( m_hCursor[eCURSOR_DEFAULTCHEAT] );
*/
}

void CMouseCursor::Process()
{
	::SetCursor(m_hCursor[m_curCursor]);
}

void CMouseCursor::SetCursor( eCURSOR cursor )
{
#ifdef _GMTOOL_
//	if( CURSOR->GetCursor() == eCURSOR_DEFAULTCHEAT )
//		return;
	if( m_bCheatCursor )
		return;
#endif

	if( m_curCursor == cursor ) return;
	
	m_curCursor = cursor;
	::SetCursor(m_hCursor[m_curCursor]);
}

void CMouseCursor::SetActive( BOOL bActive )
{
	if( m_bActive == bActive ) return;	//showcursor가 같은 인자로 계속 호출되는것 방지
										//show, hide는 누적된다. 그러므로 반복호출되면 
										//커서가 안보이거나 안없어지는 경우가 있다.
	m_bActive = bActive;
	if( bActive )
		::SetCursor(m_hCursor[m_curCursor]);
	
	::ShowCursor( bActive );
}

void CMouseCursor::SetCheatMode( BOOL bCheatMode )
{
	if( m_bCheatCursor == bCheatMode )
		return;

	if( bCheatMode )
		SetCursor(eCURSOR_DEFAULTCHEAT);
	else
		SetCursor(eCURSOR_DEFAULT);

	m_bCheatCursor = bCheatMode;

	SetActive( TRUE );
}


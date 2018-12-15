#include "stdafx.h"
#include "WindowIDEnum.h"
#include "./Input/Keyboard.h"
#include "cIMEex.h"
#include "interface/cButton.h"
#include "Interface/cFont.h"
#include "cMsgBox.h"
#include "./Interface/cResourceManager.h"
#include "./interface/cScriptManager.h"
#include "./interface/cStatic.h"
#include "./interface/cTextArea.h"
#include "./interface/cWindowManager.h"

extern HWND _g_hWnd;

cImage	cMsgBox::m_DlgImg;
cImage	cMsgBox::m_BtnImgBasic;
cImage	cMsgBox::m_BtnImgOver;
cImage	cMsgBox::m_BtnImgPress;
char	cMsgBox::m_BtnText[MB_BTN_COUNT][32]		= {0, };
BOOL	cMsgBox::m_bInit = FALSE;
DWORD	cMsgBox::m_dwColor[3] = { 0x00ffffff, 0x00ffff00, 0x00ffff00 };

cMsgBox::cMsgBox()
{
	m_type = WT_MSGBOX;
	m_nMBType = MBT_NOBTN;
	m_pMsg = NULL;
	m_pButton[0] = NULL;
	m_pButton[1] = NULL;
	m_dwMsgColor = 0x00ffffff;
	m_nDefaultBtn = -1;
	m_dwParam = 0;
	m_dwVisibleTime = 0;
	SetWH(
		200,
		115);
}

cMsgBox::~cMsgBox()
{
}

void cMsgBox::Render()
{
	cDialog::Render();
	PutCounter();
}

void cMsgBox::PutCounter()
{
	if(0 == mCounterStatic)
	{
		return;
	}
	else if(FALSE == mCounterStatic->IsActive())
	{
		return;
	}
	else if(gCurTime < m_dwVisibleTime)
	{
		const DWORD step = m_dwVisibleTime - gCurTime;
		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			step % 2000 > 1000 ? _T(". . . %d") : _T(" . .  %d"),
			step / 1000 + 1);
		mCounterStatic->SetStaticText(text);
		return;
	}

	WINDOWMGR->AddListDestroyWindow(this);
}

void cMsgBox::SetVisibleTime(DWORD dwVisibleTime)
{
	mCounterStatic->SetActive(TRUE);
	m_dwVisibleTime = gCurTime + dwVisibleTime;
}

void cMsgBox::InitMsgBox()
{
	SCRIPTMGR->GetImage( 30, &m_DlgImg ) ;									// 다이얼로그 이미지 세팅.
	SCRIPTMGR->GetImage( 31, &m_BtnImgBasic ) ;								// 버튼 기본 이미지 세팅.
	SCRIPTMGR->GetImage( 32, &m_BtnImgOver ) ;								// 버튼 오버 이미지 세팅.
	SCRIPTMGR->GetImage( 33, &m_BtnImgPress ) ;								// 버튼 프레스 이미지 세팅.

	strcpy( m_BtnText[MB_BTN_OK],		RESRCMGR->GetMsg( 11 ) ) ;			// 버튼 [ 확인 ] 메시지 세팅.
	strcpy( m_BtnText[MB_BTN_YES],		RESRCMGR->GetMsg( 268 ) ) ;			// 버튼 [ 예 ] 메시지 세팅.
	strcpy( m_BtnText[MB_BTN_NO],		RESRCMGR->GetMsg( 269 ) ) ;			// 버튼 [ 아니오 ] 메시지 세팅.
	strcpy( m_BtnText[MB_BTN_CANCEL],	RESRCMGR->GetMsg( 12 ) ) ;			// 버튼 [ 취소 ] 메시지 세팅.

	m_bInit = TRUE ;														// 초기화 여부를 true로 세팅.
}

void cMsgBox::MsgBox(LONG lId, eMBType nMBType, LPCTSTR strMsg, cbFUNC cbMBFunc)
{
	if( !strMsg ) return ;

	m_nMBType = nMBType;
	SHORT_RECT rect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&rect,
		&value,
		&value);
	
	const SIZE buttonSize = {64, 23};
	const POINT textPoint = {15, 42};
	int nMsgBoxWidth = GetWidth();
	int nMsgBoxHeight = GetHeight();
	int nBtn1_Y	= 78;
	int nBtn2_Y	= 78;
	int nTextLine = 0;

	SCRIPTMGR->GetImage( 30, &m_DlgImg ) ;										// 기본 배경. AUTOSIZE 타입의 메세지 박스 출력 후에 항상 필요하기 때문에 여기서

	//--------------------------------------------------------------------------
	// Setting Text area.
	m_pMsg = new cTextArea ;													// 텍스트 라인 수를 구하기 위해 텍스트 먼저 처리

	int  nTextWidth	= nMsgBoxWidth - textPoint.x ;
	RECT rtText		= { 0, 0, 196 - textPoint.x * 2, 200 } ;

	m_pMsg->SetFontIdx( 0 ) ;
	m_pMsg->SetFontColor( RGB(96, 98, 1) );
	m_pMsg->Init(
		textPoint.x,
		textPoint.y,
		WORD(nTextWidth),
		200,
		0);
	m_pMsg->InitTextArea(
		&rtText,
		256);
	m_pMsg->SetAlpha((BYTE)m_alpha) ;
	m_pMsg->SetScriptText( strMsg ) ;
	m_pMsg->SetActive(m_bActive) ;	

	nTextLine = m_pMsg->GetIMEex()->GetTotalLine() ;

	//--------------------------------------------------------------------------
	// Setting Dialog info.
	cImageRect* pRect = NULL ;

	cImage middleBg ;

	WORD wWidth	 = 0 ;
	WORD wHeight = 0 ;

	//--------------------------------------------------------------------------
	// ..상단 이미지
	SCRIPTMGR->GetImage( 119, &m_DlgImg ) ;
	pRect = m_DlgImg.GetImageRect() ;

	nMsgBoxHeight = (int)(pRect->bottom - pRect->top) ;

	//--------------------------------------------------------------------------
	// ..중간 이미지 - 상단 이미지, 중간 이미지, 텍스트 출력 y 좌표가 서로 딱 맞아 떨어지지가 않기 때문에 줄 수에 정확히 맞추기 위해서 따로 추가..
   	cStatic* pMiddle01Bg = new cStatic ;

	SCRIPTMGR->GetImage( 120, &middleBg ) ;
	pRect = middleBg.GetImageRect() ;

	wWidth  = (WORD)(pRect->right-pRect->left) ;
	wHeight = (WORD)(pRect->bottom-pRect->top) ;

	pMiddle01Bg->Init( 0, nMsgBoxHeight, wWidth, wHeight, &middleBg, -1 ) ;
	cImageScale scale0 = {1.0f, 3.0f} ;
 	pMiddle01Bg->SetScale(&scale0) ;
 	Add( pMiddle01Bg ) ;

	nMsgBoxHeight += (int)((pRect->bottom-pRect->top-1)*scale0.y) ;

	//--------------------------------------------------------------------------
	// ..중간 이미지 - 텍스트 공간 만큼의 크기로 설정
   	cStatic* pMiddle02Bg = new cStatic ;

	SCRIPTMGR->GetImage( 120, &middleBg ) ;
	pRect = middleBg.GetImageRect() ;

	wWidth  = (WORD)(pRect->right-pRect->left) ;
	wHeight = (WORD)(pRect->bottom-pRect->top) ;

	pMiddle02Bg->Init( 0, nMsgBoxHeight, wWidth, wHeight, &middleBg, -1 ) ;
	cImageScale scale = {1.0f, 4.0f} ;
	scale.y *= nTextLine ;
 	pMiddle02Bg->SetScale(&scale) ;
 	Add( pMiddle02Bg ) ;

	nMsgBoxHeight += (int)((pRect->bottom-pRect->top-1)*scale.y) ;

	nBtn1_Y = nMsgBoxHeight-7 ;
	nBtn2_Y = nMsgBoxHeight-7 ;

	//--------------------------------------------------------------------------
	// ..하단 이미지
	cStatic* pBottomBg = new cStatic ;

	SCRIPTMGR->GetImage( 121, &middleBg) ;
	pRect = middleBg.GetImageRect() ;

	wWidth  = (WORD)(pRect->right-pRect->left) ;
	wHeight = (WORD)(pRect->bottom-pRect->top) ;

	pBottomBg->Init( 0, nMsgBoxHeight, wWidth, wHeight, &middleBg, -1 );
	Add( pBottomBg ) ;

	nMsgBoxHeight += (int)(pRect->bottom-pRect->top) ;

	switch(nMBType)
	{
	case MBT_OK:
	case MBT_OK_EXIT_PROGRAM:
		{
			m_pButton[0] = new cButton;
			m_pButton[0]->Init(
				(nMsgBoxWidth - buttonSize.cx) / 2,
				nBtn1_Y,
				WORD(buttonSize.cx),
				WORD(buttonSize.cy),
				&m_BtnImgBasic,
				&m_BtnImgOver,
				&m_BtnImgPress,
				0,
				MBI_OK);
			m_pButton[0]->SetText( m_BtnText[MB_BTN_OK], m_dwColor[0], m_dwColor[1], m_dwColor[2] );
			m_pButton[0]->SetTextXY(0, 6) ;
			m_pButton[0]->SetAlign( TXT_CENTER );
			m_pButton[0]->SetActive( m_bActive );
			m_pButton[0]->SetAlpha( (BYTE)m_alpha );
			Add( m_pButton[0] );

			SetOpenSound(
				1256);
			SetCloseSound(
				1257);
		}
		break;

	case MBT_YESNO:
	case MBT_YESNO_EXIT_PROGRAM:
		{
			m_pButton[0] = new cButton;
			m_pButton[0]->Init(
				24,
				nBtn2_Y,
				WORD(buttonSize.cx),
				WORD(buttonSize.cy),
				&m_BtnImgBasic,
				&m_BtnImgOver,
				&m_BtnImgPress,
				0,
				MBI_YES);
			m_pButton[0]->SetText( m_BtnText[MB_BTN_YES], m_dwColor[0], m_dwColor[1], m_dwColor[2] );
			m_pButton[0]->SetTextXY(0, 6) ;
			m_pButton[0]->SetAlign( TXT_CENTER );
			m_pButton[0]->SetActive( m_bActive );
			m_pButton[0]->SetAlpha( (BYTE)m_alpha );
			Add( m_pButton[0] );

			m_pButton[1] = new cButton;
			m_pButton[1]->Init(
				111,
				nBtn1_Y,
				WORD(buttonSize.cx),
				WORD(buttonSize.cy),
				&m_BtnImgBasic,
				&m_BtnImgOver,
				&m_BtnImgPress,
				0,
				MBI_NO);
			m_pButton[1]->SetText( m_BtnText[MB_BTN_NO], m_dwColor[0], m_dwColor[1], m_dwColor[2] );
			m_pButton[1]->SetTextXY(0, 6) ;
			m_pButton[1]->SetAlign( TXT_CENTER );
			m_pButton[1]->SetActive( m_bActive );
			m_pButton[1]->SetAlpha( (BYTE)m_alpha );
			Add( m_pButton[1] );

			SetOpenSound(
				1256);
			SetCloseSound(
				1257);
		}
		break;

	case MBT_CANCEL:
	case MBT_CANCEL_EXIT_PROGRAM:
		{
			m_pButton[0] = new cButton;
			m_pButton[0]->Init(
				(nMsgBoxWidth - buttonSize.cx) / 2,
				nBtn1_Y,
				WORD(buttonSize.cx),
				WORD(buttonSize.cy),
				&m_BtnImgBasic,
				&m_BtnImgOver,
				&m_BtnImgPress,
				0,
				MBI_CANCEL);
			m_pButton[0]->SetText( m_BtnText[MB_BTN_CANCEL], m_dwColor[0], m_dwColor[1], m_dwColor[2] );
			m_pButton[0]->SetTextXY(0, 6) ;
			m_pButton[0]->SetAlign( TXT_CENTER );
			m_pButton[0]->SetActive( m_bActive );
			m_pButton[0]->SetAlpha( (BYTE)m_alpha );
			Add( m_pButton[0] );

			SetOpenSound(
				1256);
			SetCloseSound(
				1257);
		}
		break;
	}

	// ..배경을 먼저 출력해야 하기 때문에 텍스트 추가는 여기서
	Add( m_pMsg );
	cDialog::Init(
		0,
		0,
		GetWidth(),
		GetHeight(),
		&m_DlgImg,
		lId);
	// 100302 LUJ, 위치를 원하는대로 배치하려면 SetAbsXY()를 사용해야한다
	SetAbsXY(
		(rect.right - GetWidth()) / 2,
		(rect.bottom - GetHeight()) / 2);
	SetMovable(TRUE);
	RECT rtCaption = { 0, 0, GetWidth(), 20 };
	SetCaptionRect(&rtCaption);

	m_cbMBFunc = cbMBFunc;
	
	cStatic* pStatic = new cStatic;
	pStatic->Init( 15, 16, 0, 0, NULL, -1 );
	pStatic->SetStaticText( RESRCMGR->GetMsg( 261 ) );
	pStatic->SetShadow( TRUE );
	pStatic->SetFontIdx( 0 );
	pStatic->SetAlign( TXT_LEFT );
	Add( pStatic );

	mCounterStatic = new cStatic;
	mCounterStatic->Init(
		GetWidth() - 30,
		16,
		0,
		0,
		0,
		-1);
	mCounterStatic->SetShadow(TRUE);
	mCounterStatic->SetFontIdx(0);
	mCounterStatic->SetAlign(TXT_RIGHT);
	mCounterStatic->SetActive(FALSE);
	Add(mCounterStatic);
	m_bActive = FALSE;
}

DWORD cMsgBox::ActionEvent( CMouse * mouseInfo )
{
	DWORD we	= WE_NULL ;													// 다이얼로그 이벤트 변수 선언.
	DWORD weBtn = WE_NULL ;													// 버튼 이벤트 변수 선언.
	
	if( !m_bActive ) return we ;											// 메시지 박스가 비활성화 상태면, return we.

	we |= cWindow::ActionEvent( mouseInfo ) ;								// 다이얼로그의 이벤트를 받는다.
	we |= cDialog::ActionEventWindow( mouseInfo) ;							// 버튼 이벤트를 받는다.

	for( int i = 0 ; i < MB_MAX_BTN_COUNT ; ++ i )							// 최대 버튼 수 만큼 for문을 돌린다.
	{
		if( !m_pButton[i] ) continue ;										

		weBtn = m_pButton[i]->ActionEvent( mouseInfo ) ;					// 버튼의 이벤트를 받는다.

		if( weBtn & WE_BTNCLICK )											// 버튼을 클릭했다면,
		{
			ForcePressButton(m_pButton[i]->GetID()) ;						// 버튼 눌림 처리를 실행하고 for문을 빠져나간다.
			break ;
		}
	}

	return we ;																// 이벤트를 return 한다.
}

DWORD cMsgBox::ActionKeyboardEvent( CKeyboard * keyInfo )
{
	if( !m_bActive ) return WE_NULL;

	if( MBT_YESNO == m_nMBType ) 
	{
		for( int i = 0 ; i < MB_MAX_BTN_COUNT ; ++ i )
		{
			if( !m_pButton[i] ) continue ;										

			if( keyInfo->GetKeyDown( KEY_RETURN ) && (MBI_YES == m_pButton[i]->GetID()) ) 
			{
				ForcePressButton( MBI_YES ) ;
				break;
			}
			else if( keyInfo->GetKeyDown( KEY_ESCAPE ) && (MBI_NO == m_pButton[i]->GetID()) ) 
			{
				ForcePressButton( MBI_NO ) ;
				break;
			}
		}
	}
	else if( MBT_OK == m_nMBType )
	{
		if( keyInfo->GetKeyDown( KEY_RETURN ))
		{
			// 100304 ONS 엔터키 처리시 콜백함수 실행하도록 수정.
			ForcePressButton( MBT_OK ) ;
		}
	}

	return WE_NULL;
}

BOOL cMsgBox::PressDefaultBtn()
{
	if( m_nDefaultBtn < 0 || m_nDefaultBtn >= MB_MAX_BTN_COUNT )			// DefaultBtn의 아이디 범위를 체크한다.
	{
		return FALSE ;
	}

	if( m_pButton[m_nDefaultBtn] )											// 버튼의 정보가 유효하면,
	{
		return ForcePressButton( m_pButton[m_nDefaultBtn]->GetID() ) ;		// 해당 버튼의 Press 이벤트 처리를 한다.
	}

	return FALSE ;
}

BOOL cMsgBox::ForcePressButton( LONG lBtnID )
{
	if( !m_cbMBFunc ) return FALSE ;										// callback 함수의 정보를 체크한다.

	m_cbMBFunc( m_ID, this, lBtnID ) ;										// callback 함수를 실행한다.

	SetActive( FALSE ) ;													// 메시지 박스를 닫는다.
	SetDisable( TRUE ) ;													// 메시지 박스를 비활성화 한다.

	WINDOWMGR->AddListDestroyWindow( this ) ;								// 윈도우 매니져에 해제 윈도우로 등록한다.

	// 100702 ShinJS 프로그램 종료 MsgBox인 경우
	switch( m_nMBType )
	{
	case MBT_OK_EXIT_PROGRAM:
	case MBT_YESNO_EXIT_PROGRAM:
	case MBT_CANCEL_EXIT_PROGRAM:
		{
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
	}

	return TRUE ;
}

void cMsgBox::ForceClose()	
{
	switch( m_nMBType )														// 메시지 박스의 타입을 확인한다.
	{
	case MBT_NOBTN :	ForcePressButton( 0 ) ;				break ;
	case MBT_OK :		ForcePressButton( MBI_OK ) ;		break ;
	case MBT_YESNO :	ForcePressButton( MBI_NO ) ;		break ;
	case MBT_CANCEL :	ForcePressButton( MBI_CANCEL ) ;	break ;
	default :			/* default 체크를 꼭~!! 하자. */	break ;
	}
}

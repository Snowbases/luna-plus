#include "stdafx.h"
#include "BalloonOutLine.h"
#include "./Interface/cImage.h"
#include "./Interface/cScriptManager.h"

GLOBALTON(cBalloonOutline);

cBalloonOutline::cBalloonOutline()
{
}

cBalloonOutline::~cBalloonOutline()
{
}

void cBalloonOutline::InitChatBalloon()
{
	SCRIPTMGR->GetImage( 63, &m_PlayerChatImage[eCHATBALLOON_LEFT1] );
	SCRIPTMGR->GetImage( 64, &m_PlayerChatImage[eCHATBALLOON_CENTER1] );
	SCRIPTMGR->GetImage( 65, &m_PlayerChatImage[eCHATBALLOON_RIGHT1] );
	SCRIPTMGR->GetImage( 66, &m_PlayerChatImage[eCHATBALLOON_LEFT2] );
	SCRIPTMGR->GetImage( 67, &m_PlayerChatImage[eCHATBALLOON_CENTER2] );
	SCRIPTMGR->GetImage( 68, &m_PlayerChatImage[eCHATBALLOON_RIGHT2] );
	SCRIPTMGR->GetImage( 69, &m_PlayerChatImage[eCHATBALLOON_LEFT3] );
	SCRIPTMGR->GetImage( 70, &m_PlayerChatImage[eCHATBALLOON_CENTER3] );
	SCRIPTMGR->GetImage( 71, &m_PlayerChatImage[eCHATBALLOON_RIGHT3] );
	SCRIPTMGR->GetImage( 75, &m_PlayerChatImage[eCHATBALLOON_TAIL] );

	SCRIPTMGR->GetImage( 105, &m_PlayerSellTitleImage[eCHATBALLOON_LEFT1] );
	SCRIPTMGR->GetImage( 106, &m_PlayerSellTitleImage[eCHATBALLOON_CENTER1] );
	SCRIPTMGR->GetImage( 107, &m_PlayerSellTitleImage[eCHATBALLOON_RIGHT1] );
	SCRIPTMGR->GetImage( 108, &m_PlayerSellTitleImage[eCHATBALLOON_LEFT2] );
	SCRIPTMGR->GetImage( 109, &m_PlayerSellTitleImage[eCHATBALLOON_CENTER2] );
	SCRIPTMGR->GetImage( 110, &m_PlayerSellTitleImage[eCHATBALLOON_RIGHT2] );
	SCRIPTMGR->GetImage( 111, &m_PlayerSellTitleImage[eCHATBALLOON_LEFT3] );
	SCRIPTMGR->GetImage( 112, &m_PlayerSellTitleImage[eCHATBALLOON_CENTER3] );
	SCRIPTMGR->GetImage( 113, &m_PlayerSellTitleImage[eCHATBALLOON_RIGHT3] );

	SCRIPTMGR->GetImage( 128, &m_PlayerBuyTitleImage[eCHATBALLOON_LEFT1] );
	SCRIPTMGR->GetImage( 129, &m_PlayerBuyTitleImage[eCHATBALLOON_CENTER1] );
	SCRIPTMGR->GetImage( 130, &m_PlayerBuyTitleImage[eCHATBALLOON_RIGHT1] );
	SCRIPTMGR->GetImage( 131, &m_PlayerBuyTitleImage[eCHATBALLOON_LEFT2] );
	SCRIPTMGR->GetImage( 132, &m_PlayerBuyTitleImage[eCHATBALLOON_CENTER2] );
	SCRIPTMGR->GetImage( 133, &m_PlayerBuyTitleImage[eCHATBALLOON_RIGHT2] );
	SCRIPTMGR->GetImage( 134, &m_PlayerBuyTitleImage[eCHATBALLOON_LEFT3] );
	SCRIPTMGR->GetImage( 135, &m_PlayerBuyTitleImage[eCHATBALLOON_CENTER3] );
	SCRIPTMGR->GetImage( 136, &m_PlayerBuyTitleImage[eCHATBALLOON_RIGHT3] );
}


void cBalloonOutline::RenderPlayerChatBalloon( int Sx, int Sy, int TailX, int nWidth, int nLineNum, int nBorder, int nAlpha )
{
	int nMiddleStartPos = nBorder ;
	cImageRect* pRenderRect = m_PlayerChatImage[(nLineNum*3)-3].GetImageRect() ;

	if( pRenderRect )
	{
		nMiddleStartPos = pRenderRect->right - pRenderRect->left ;
	}

	VECTOR2	vPos, vScale;

	vScale.x = (float)nWidth;
	vScale.y = 1.f;

	vPos.x = (float)Sx;
	vPos.y = (float)Sy;
	
	m_PlayerChatImage[(nLineNum*3)-3].RenderSprite( NULL, NULL, 0, &vPos, RGBA_MERGE(0xffffff, nAlpha) );
	vPos.x += nMiddleStartPos;
	m_PlayerChatImage[(nLineNum*3)-2].RenderSprite( &vScale, NULL, 0, &vPos, RGBA_MERGE(0xffffff, nAlpha) );
	vPos.x += nWidth;
	m_PlayerChatImage[(nLineNum*3)-1].RenderSprite( NULL, NULL, 0, &vPos, RGBA_MERGE(0xffffff, nAlpha) );

	//채팅꼬리
	vPos.x = (float)TailX;
	vPos.y = (float)( Sy + ( nLineNum * 10 ) + 30 - 4);

	m_PlayerChatImage[eCHATBALLOON_TAIL].RenderSprite( NULL, NULL, 0, &vPos, RGBA_MERGE(0xffffff, nAlpha) );
}

//---KES 상점검색 2008.3.11
void cBalloonOutline::RenderPlayerSellTitleBalloon( int Sx, int Sy, int TailX, int nWidth, int nLineNum, DWORD dwColor, int nBorder, int nAlpha )
{
	int nMiddleStartPos = nBorder ;
	cImageRect* pRenderRect = m_PlayerSellTitleImage[(nLineNum*3)-3].GetImageRect() ;

	if( pRenderRect )
	{
		nMiddleStartPos = pRenderRect->right - pRenderRect->left ;
	}

	VECTOR2	vPos, vScale;

	vScale.x = (float)nWidth;
	vScale.y = 1.f;

	vPos.x = (float)Sx;
	vPos.y = (float)Sy;
	
	m_PlayerSellTitleImage[(nLineNum*3)-3].RenderSprite( NULL, NULL, 0, &vPos, RGBA_MERGE(dwColor, nAlpha) );
	vPos.x += nMiddleStartPos;
	m_PlayerSellTitleImage[(nLineNum*3)-2].RenderSprite( &vScale, NULL, 0, &vPos, RGBA_MERGE(dwColor, nAlpha) );
	vPos.x += nWidth;
	m_PlayerSellTitleImage[(nLineNum*3)-1].RenderSprite( NULL, NULL, 0, &vPos, RGBA_MERGE(dwColor, nAlpha) );
}

void cBalloonOutline::RenderPlayerBuyTitleBalloon( int Sx, int Sy, int TailX, int nWidth, int nLineNum, DWORD dwColor, int nBorder, int nAlpha )
{
	int nMiddleStartPos = nBorder ;
	cImageRect* pRenderRect = m_PlayerBuyTitleImage[(nLineNum*3)-3].GetImageRect() ;

	if( pRenderRect )
	{
		nMiddleStartPos = pRenderRect->right - pRenderRect->left ;
	}

	VECTOR2	vPos, vScale;

	vScale.x = (float)nWidth;
	vScale.y = 1.f;

	vPos.x = (float)Sx;
	vPos.y = (float)Sy;
	
	m_PlayerBuyTitleImage[(nLineNum*3)-3].RenderSprite( NULL, NULL, 0, &vPos, RGBA_MERGE(dwColor, nAlpha) );
	vPos.x += nMiddleStartPos;
	m_PlayerBuyTitleImage[(nLineNum*3)-2].RenderSprite( &vScale, NULL, 0, &vPos, RGBA_MERGE(dwColor, nAlpha) );
	vPos.x += nWidth;
	m_PlayerBuyTitleImage[(nLineNum*3)-1].RenderSprite( NULL, NULL, 0, &vPos, RGBA_MERGE(dwColor, nAlpha) );
}


/*
void cBalloonOutline::RenderPlayerTitleBalloon( int Sx, int Sy, int TailX, int nWidth, int nLineNum, int nBorder, int nAlpha )
{
	int nMiddleStartPos = nBorder ;
	cImageRect* pRenderRect = m_PlayerTitleImage[(nLineNum*3)-3].GetImageRect() ;

	if( pRenderRect )
	{
		nMiddleStartPos = pRenderRect->right - pRenderRect->left ;
	}

	VECTOR2	vPos, vScale;

	vScale.x = (float)nWidth;
	vScale.y = 1.f;

	vPos.x = (float)Sx;
	vPos.y = (float)Sy;
	
	m_PlayerTitleImage[(nLineNum*3)-3].RenderSprite( NULL, NULL, 0, &vPos, RGBA_MERGE(0xffffff, nAlpha) );
	vPos.x += nMiddleStartPos;
	m_PlayerTitleImage[(nLineNum*3)-2].RenderSprite( &vScale, NULL, 0, &vPos, RGBA_MERGE(0xffffff, nAlpha) );
	vPos.x += nWidth;
	m_PlayerTitleImage[(nLineNum*3)-1].RenderSprite( NULL, NULL, 0, &vPos, RGBA_MERGE(0xffffff, nAlpha) );
}
*/
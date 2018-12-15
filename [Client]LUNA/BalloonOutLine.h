#ifndef _BALLOON_OUTLINE_
#define _BALLOON_OUTLINE_

///////////////////////////////////////////////////////////////////////////////////
// LBS 03.11.5
///////////////////////////////////////////////////////////////////////////////////

#include "./interface/cImage.h"

#define BALLOONOUTLINE USINGTON(cBalloonOutline)

enum {
	eCHATBALLOON_LEFT1=0,
	eCHATBALLOON_CENTER1,
	eCHATBALLOON_RIGHT1,
	eCHATBALLOON_LEFT2,
	eCHATBALLOON_CENTER2,
	eCHATBALLOON_RIGHT2,
	eCHATBALLOON_LEFT3,
	eCHATBALLOON_CENTER3,
	eCHATBALLOON_RIGHT3,
	eCHATBALLOON_TAIL,
	eCHATBALLOON_MAX,
};

class cImage;

class cBalloonOutline 
{
protected:
	cImage			m_PlayerChatImage[eCHATBALLOON_MAX];
	cImage			m_PlayerSellTitleImage[eCHATBALLOON_MAX];
	cImage			m_PlayerBuyTitleImage[eCHATBALLOON_MAX];
//	cImage			m_MonsterChatImage[eCHATBALLOON_MAX];

public:

	cBalloonOutline();
	virtual ~cBalloonOutline();

	void InitChatBalloon();

	void RenderPlayerChatBalloon( int Sx, int Sy, int TailX, int Width, int nLineNum, int nBorder = 8, int nAlpha = 180 );
//---KES 상점검색 2008.3.11
	void RenderPlayerSellTitleBalloon( int Sx, int Sy, int TailX, int Width, int nLineNum, DWORD dwColor = 0xffffffff, int nBorder = 8, int nAlpha = 180 );
	void RenderPlayerBuyTitleBalloon( int Sx, int Sy, int TailX, int Width, int nLineNum, DWORD dwColor = 0xffffffff, int nBorder = 8, int nAlpha = 180 );
//-------------------------
};

EXTERNGLOBALTON(cBalloonOutline)

#endif //_BALLOON_OUTLINE_
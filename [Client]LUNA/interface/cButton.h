//////////////////////////////////////////////////////////////////////
// class name : cButton 
// Lastest update : 2002.10.29. by taiyo
//////////////////////////////////////////////////////////////////////

#ifndef _cBUTTON_H_
#define _cBUTTON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cWindow.h"
#include "cWindowDef.h"

// 071004 LYW --- cButton : Define sound index to use button.
#define		CLICK_SOUND_IDX		115
#define		OVER_SOUND_IDX		116





class cButton : public cWindow  
{
public:

	enum eBtn_State
	{
		eBtn_State_None,
		eBtn_State_Over,
		eBtn_State_Press,
	};

	enum eBtn_Mouse_Evt
	{
		eBtn_Mouse_Evt_None,
		eBtn_Mouse_Evt_Use_Normal_Image,
		eBtn_Mouse_Evt_Use_Over_Image,
		eBtn_Mouse_Evt_Use_Press_Image,
		eBtn_Mouse_Evt_Use_Normal_Ani_Image,
		eBtn_Mouse_Evt_Use_Over_Ani_Image,
		eBtn_Mouse_Evt_Use_Press_Ani_Image,
	};

	cButton();
	virtual ~cButton();

	virtual void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, cImage * overImage, cImage * pressImage, cbFUNC Func, LONG ID=0);
	virtual DWORD ActionEvent(CMouse * mouseInfo);
	virtual void Render();
	void SetText(char * text, DWORD basicColor, DWORD overColor=NULL, DWORD PressColor=NULL);
	void SetTextValue(DWORD value);
	void SetTextXY(LONG x, LONG y) { m_textXY.x = x; m_textXY.y = y; }

	BOOL IsClickInside() { return m_bClickInside; }
	void SetTextAni( BYTE x, BYTE y ) { m_BtnTextAniX = x; m_BtnTextAniY = y; }

	void SetClickSound( SNDIDX idx ) { m_sndIdx = idx; m_bSound = TRUE; }

	// 071004 LYW --- cButton : Add function to setting sound index when the mouse is over to button.
	void SetOverSound( SNDIDX idx ) { m_osndIdx = idx ; m_bOSound = TRUE ; }
	
	void SetShadowTextXY(LONG x, LONG y)	{	m_shadowTextXY.x = x; m_shadowTextXY.y = y;		}
	void SetShadowColor(DWORD color)		{	m_shadowColor = color;							}
	void SetShadow(BOOL val)				{	m_bShadow = val;								}
	void SetAlign(int nAlign) { m_nAlign = nAlign; }
	DWORD GetBasicColor() const { return m_fgBasicColor ; }
	DWORD GetOverColor() const { return m_fgOverColor ; }
	DWORD GetPressColor() const { return m_fgPressColor ; }
	void SetBasicColor(DWORD color) { m_fgBasicColor = color; }
	void SetRenderArea();
	char* GetButtonText() { return m_szText; }
	virtual void Add( cWindow* );

	void AddAnimationImage( eBtn_State state, const std::vector< std::pair< DWORD, cImageRect > >& aniImgList, const DWORD dwFrameTime );
	void SetActiveAnimationImage( const BOOL bActive ) { m_bIsActiveAnimated = bActive; }
	void SetFirstDrawAnimateImage( const BOOL bActive ) { m_FirstDrawAnimateImage = bActive; }

	void SetBtnState( eBtn_State state );
	const eBtn_State GetBtnState() const { return m_BtnState; }

	void SetBtnMouseEvent( eBtn_Mouse_Evt event ) { m_BtnMouseEvent = event; }
	void SetAnimationImageValidXY( const VECTOR2& validxy, const VECTOR2& validxyOver, const VECTOR2& validxyPress );

protected:	
	cImage	m_OverImage;
	cImage	m_PressImage;
	
	cCoord m_textXY;
	
	// Text label of button
	int m_nCurTextLen;
	char m_szText[MAX_TEXT_SIZE];
	DWORD m_fgBasicColor;
	DWORD m_fgOverColor;
	DWORD m_fgPressColor;
	DWORD m_fgCurColor;
	
//KES INTERFACE 031006
	BOOL m_bClickInside;
	BYTE m_BtnTextAniX;
	BYTE m_BtnTextAniY;
	int	 m_nAlign;

	SNDIDX	m_sndIdx;
	BOOL	m_bSound;

	// 071004 LYW --- cButton : Add sound for mouse over part.
	SNDIDX	m_osndIdx ;											// 마우스 오버시 플레이 시킬 사운드 인덱스를 담는 변수.
	BOOL	m_bOSound ;											// 오버 사운드를 플레이 시킬 지 여부를 담는 변수.

	BOOL	m_bPlayOverSound ;									// 오버 사운드가 플레이 되었는지 체크하기 위한 변수.
	
	BOOL			m_bShadow;
	cCoord			m_shadowTextXY;
	DWORD			m_shadowColor;

	eBtn_State m_BtnState;

	eBtn_Mouse_Evt m_BtnMouseEvent;

	// 애니메이션 이미지 활성화 여부
	BOOL m_bIsActiveAnimated;
	// 애니메이션 이미지를 기본이미지보다 먼저 Draw 할지 여부
	BOOL m_FirstDrawAnimateImage;

	// 애니메이션 Frame 관련
	DWORD m_dwAniImgFrameTime;
	DWORD m_dwOverAniImgFrameTime;
	DWORD m_dwPressAniImgFrameTime;
	DWORD m_dwNextChangeFrameTime;
	DWORD m_dwCurAniImgFrame;

	// Normal Animation Image
	std::map< DWORD, cImage > m_mapAniImage;
	// Over Animation Image
	std::map< DWORD, cImage > m_mapOverAniImage;
	// Press Animation Image
	std::map< DWORD, cImage > m_mapPressAniImage;

	VECTOR2 m_validXYAniImage;
	VECTOR2 m_validXYOverAniImage;
	VECTOR2 m_validXYPressAniImage;

	void RenderAnimationImage();
	void RenderAnimationImage( std::map< DWORD, cImage >& mapAniImg, const VECTOR2& validxy );
};

#endif // _cBUTTON_H_
